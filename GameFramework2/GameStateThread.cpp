#include "GameStateThread.h"

#include <sstream>

void GameStateThread::cleanup()
{
	actionBuffer.deleteAll();
}

void GameStateThread::loadComplete(vector<char> data, unsigned int loadId, CollectionId collection)
{
	if (loadId != this->loadId) return; // Old load asynchronously completed.
	if (data.empty()) return; // Load must have failed!
	VectorInStream stream(data);
	LoadReader reader(stream);
	CollectionId readCollection;
	reader.read(&readCollection);
	if (readCollection != collection) {
		ostringstream stream;
		stream << readCollection;
		errorNotifier.notify( ErrorCodes::Message(ErrorCodes::COLLECTION_ERROR, stream.str() ));
		return; // This is so dire the entire integrity of pointers are compromised. No load done will be sendt.
	}
	ActionTimeStep time;
	reader.read(&time);
	GamestateFileSpecifier& fileStatus = loadedFiles[collection];
	if (!fileStatus.merge && !fileStatus.readOnly) {
		if (currentTime != 0 && time != currentTime) {
			errorNotifier.notify(ErrorCodes::Message(ErrorCodes::VERSONING_ERROR, fileStatus.filename));
			return; // Merge not set, so user would not like this to be considdered loaded. No load done will be sendt.
		}
		currentTime = time;
	}

	persister.load(reader, collection);
	fileStates[collection].loaded = true;
	auto it = unLoaded.find(collection);
	if (it != unLoaded.end()) {
		unLoaded.erase(it);
	}
	if (!isLoaded && unLoaded.empty()) {
		isLoaded = true;
		doneLoadingNotifier.notify(currentTime);
	}
}

GameStateThread::GameStateThread(GamestatePersister& p, ResourceThread& resource) :
	performActionsProxy(*this, &GameStateThread::performActions),
	loadGameStateProxy(*this, &GameStateThread::loadGameState),
	saveGameStateProxy(*this, &GameStateThread::saveGameState),
	persister(p),
	resourceThread(resource)
{
}

void GameStateThread::performActions(ActionStore *action)
{
	actionBuffer.insert(action->getTime(), action);
	while (actionBuffer.hasNext()) {
		ActionStore* store = actionBuffer.getNext();
		ActionStore::Accessor * access = store->getAccessor();
		while (access->hasNext()) {
			NetworkAction* a = access->getNext();
			if (a->validate()) {
				a->run();
			}
			delete a;
		}
		delete access;
		delete store;
	}
}

void GameStateThread::loadGameState(GameStateFiles specifiers)
{
	isLoaded = false;
	loadId++;
	unLoaded.clear();
	loadedFiles.clear();
	fileStates.clear();
	for (auto it = specifiers.collectionFiles.begin(); it != specifiers.collectionFiles.end(); it++) {
		unLoaded.insert(it->first);
		fileStates[it->first].loaded = false;
		loadedFiles[it->first] = (it->second);
	}
	for (auto it = loadedFiles.begin(); it != loadedFiles.end(); it++) {
		resourceThread.loadProxy.makeCall(it->second.filename, new LoaderCallback(*this, loadId, it->first));
	}
}

void GameStateThread::saveGameState(GameStateFiles newSpecifiers)
{
	for (auto it = newSpecifiers.collectionFiles.begin(); it != newSpecifiers.collectionFiles.end(); it++) {
		auto entry = loadedFiles.find(it->first);
		if (entry != loadedFiles.end()) {
			entry->second.filename = it->second.filename;
		}
		else {
			loadedFiles[it->first] = it->second;
		}
	}

	for (auto it = loadedFiles.begin(); it != loadedFiles.end(); it++) {
		if (!it->second.readOnly) {
			vector<char> outData;
			VectorOutStream outStream(outData);
			SaveWriter writer(outStream);
			writer.write(it->first);
			writer.write(currentTime);
			persister.save(writer, it->first);
			resourceThread.saveProxy.makeCall(it->second.filename, outData);
		}
	}
}

GameStateThread::LoaderCallback::LoadCompleteMessage::LoadCompleteMessage(GameStateThread & t, vector<char> d, unsigned int load, CollectionId collection)
	: thread(t), data(d), loadId(load), collectionId(collection)
{ ; }

bool GameStateThread::LoaderCallback::LoadCompleteMessage::activate()
{
	thread.loadComplete(data, loadId, collectionId);
	return true;
}

GameStateThread::LoaderCallback::LoadCompleteMessage * GameStateThread::LoaderCallback::getSendMessage(vector<char> data)
{
	return new LoadCompleteMessage(thread, std::move(data), loadId, collectionId);
}

GameStateThread::LoaderCallback::LoaderCallback(GameStateThread & t, unsigned int load, CollectionId collection)
	: ThreadNotifyCallback(thread.getSender()), thread(t), loadId(load), collectionId(collection)
{
}
