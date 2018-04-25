#include "NetworkThread.h"
#include "Serialization.h"
#include "ThreadSystem.h"
#include "ThreadPreDeclarations.h"
#include "GameClientProtocol.h"
#include "ThreadNotificationSystem.h"
#include <sstream>

NetworkThread::NetworkThread(UDPServerCore::Factory * coreFactory, GameStateThread& gs) :
	coreProvider(coreFactory),
	gameStateThread(gs),
	sendActionProxy(*this, &NetworkThread::sendAction),
	startServerProxy(*this, &NetworkThread::startServer),
	stopServerProxy(*this, &NetworkThread::stopServer),
	startClientProxy(*this, &NetworkThread::startServer),
	connectToServerProxy(*this, &NetworkThread::connectToServer),
	trySetPasswordProxy(*this, &NetworkThread::trySetPassword),
	stopClientProxy(*this, &NetworkThread::stopClient),
	doTickProxy(*this, &NetworkThread::doTick),
	clientErrorCallback(*this),
	serverErrorCallback(*this),
	serverActionCallback(*this),
	playerIdCallback(*this),
	actionsRecievedCallback(*this)
{
}

void NetworkThread::cleanup()
{
	// This is quite heavy handed, as it will be interupting threads;
	if (server) {
		delete server;
	}
	if (client) {
		delete client;
	}
}

void NetworkThread::commonErrorHandle(UDPMsg::ErrorCode error, std::string description)
{
	errorNotifier.notify(ErrorCodes::Message(ErrorCodes::NETWORK_ERROR_BASE + error, description) );
}

void NetworkThread::onInitialized()
{
	clientHomeDesc.networkThreadID = getID();
	clientHomeDesc.threadSystem = getSystem();
	clientHomeDesc.errorCallback = &clientErrorCallback;

	serverHomeDesc.networkThreadID = getID();
	serverHomeDesc.threadSystem = getSystem();
	serverHomeDesc.errorCallback = &serverErrorCallback;
}

/** Temporary dummy implementation of send action for testing purposes*/
void NetworkThread::sendAction(NetworkAction * action)
{
	if (!client || !action) {
		if (action) delete action;
		return;
	}
	UDPMsg::NetworkMessageWriter writer(UDPMsg::MessageType::COMMAND_REQUEST_DO_ACTION);
	action->serializeWithoutPlayer(writer);
	client->sendToMe(writer.getData());
	delete action;
	////Shortcircut the network system
	//ostringstream data;
	//SaveWriter writer(data);
	//action->serialize(writer);
	//istringstream input(data.str);
	//LoadReader reader(input);
	//NetworkAction* result = NetworkAction::fetchNew(reader);
	//IThreadSystem* system = getSystem();
	//int recipentID = system->getID(GAME_STATE_THREAD_NAME);
	//system->sendMessage(recipentID, new ActionMessage(result));
}

bool NetworkThread::startServer()
{
	if (server) return false; //Can't make two servers. Stop this server first!
	UDPServerCore* core = coreProvider->makeCore();
	GameServerConfig config;
	config.core = core;
	config.home = &serverHomeDesc;
	config.actionCallback = &serverActionCallback;
	config.loginPolicy.sessionPassword = 0;
	config.loginPolicy.unknownAllowed = true;
	config.players = &playerStatus;
	server = new SimpleGameServer(config);
	server->start();
	return true;
}

bool NetworkThread::stopServer()
{
	if (!server) {
		return false;
	}
	UDPMsg::NetworkMessageWriter writer(UDPMsg::MessageType::SHUT_DOWN);
	server->sendToMe(writer.getData());
	return true;
}

bool NetworkThread::startClient()
{
	if (client) {
		return false; //client already started
	}
	GameClientConfig config;
	config.actionsRecievedCallback = &actionsRecievedCallback;
	config.home = &clientHomeDesc;
	config.core = coreProvider->makeCore();
	config.idRecievedCallback = &playerIdCallback;
	config.loginPolicy.unknownAllowed = false;
	client = new SimpleGameClient(config);
	client->start();

	return true;
}

namespace Cryptography {
	long long hash(string password) {
		long long h = 1241165499441;
		for (int i = 0; i < password.size(); i++) {
			h *= password[i];
			h >>= 2;
		}
		return h;
	}
}

bool NetworkThread::connectToServer(NetworkAdressSpecifier serverAdress, std::string username, std::string password)
{
	if (!client) {
		return false;
	}
	UDPMsg::CommandConnectLogin login;
	login.username = username;
	login.passwordHash = Cryptography::hash(password);
	login.address = serverAdress;
	UDPMsg::NetworkMessageWriter writer(UDPMsg::MessageType::COMMAND_CONNECT_LOGIN);
	login.serialize(writer);
	client->sendToMe(writer.getData());
	return true;
}

bool NetworkThread::stopClient()
{
	if (!client) {
		return false;
	}
	UDPMsg::NetworkMessageWriter writer(UDPMsg::MessageType::SHUT_DOWN);
	client->sendToMe(writer.getData());
	return true;
}

bool NetworkThread::doTick()
{
	if (!server) {
		return false;
	}
	UDPMsg::NetworkMessageWriter writer(UDPMsg::MessageType::CONFIRMED_ACTIONS);
	workingStore.serialize(writer);
	server->sendToMe(writer.getData());
	workingStore.reset(++currentTime);
	return true;
}

void NetworkThread::handleClientError(UDPMsg::ErrorCode error)
{
	switch (error) {
	case UDPMsg::ERROR_CODES::SERVER_STOPPED:
		if (client) {
			delete client;
		}
		client = nullptr;
		break;
	default:
		commonErrorHandle(error, "");
	}
}

void NetworkThread::handleServerError(UDPMsg::ErrorCode error)
{
	switch (error) {
	case UDPMsg::ERROR_CODES::SERVER_STOPPED:
		if (server) {
			delete server;
		}
		server = nullptr;
		break;
	default:
		commonErrorHandle(error, "");
	}
}

void NetworkThread::serverAction(NetworkAction * action)
{
	workingStore.addAction(action);
}

void NetworkThread::playerIdRecieved(PlayerID id)
{
	playerIdRecievedNotifier.notify(id);
}

void NetworkThread::actionsRecieved(ActionStore * actions)
{
	confirmedActions.insert(actions->getTime(), actions);
	while (confirmedActions.hasNext()) {
		gameStateThread.performActionsProxy.makeCall(confirmedActions.getNext());
	}
}
