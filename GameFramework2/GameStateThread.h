#pragma once
#include "ErrorCodes.h"
#include "ObjectCollection.h"
#include "ThreadBase.h"
#include "ResourceThread.h"
#include "QueueBuffer.h"
#include "ActionProtocol.h"
#include "ThreadProxyCaller.h"
#include "FileSpecifier.h"
#include <set>

struct GamestateFileState {
	ActionTimeStep timestep; //current version for file. Might be used as a version for read only files. Internal use.
	bool loaded; //internal use.
};

/** Accessorpoint for the action system. Also provide loading and saving features. */
class GameStateThread : public Thread {
	ResourceThread& resourceThread;
	QueueBuffer<ActionStore> actionBuffer;
	GamestatePersister& persister;
	set<CollectionId> unLoaded;
	map<CollectionId, GamestateFileSpecifier> loadedFiles;
	map<CollectionId, GamestateFileState> fileStates;
	unsigned int loadId;
	bool isLoaded;
	ActionTimeStep currentTime;

public:
	GameStateThread(GamestatePersister& persister, ResourceThread& resource);

	ThreadProxyCallerFactory<GameStateThread, void, ActionStore*> performActionsProxy;
	ThreadProxyCallerFactory<GameStateThread, void, GameStateFiles > loadGameStateProxy;
	ThreadProxyCallerFactory<GameStateThread, void, GameStateFiles > saveGameStateProxy;

	/** Notifies that the state is done loading at ActionTimeStep */
	ThreadNotifierBase<ActionTimeStep> doneLoadingNotifier;
	ThreadNotifierBase<ErrorCodes::Message> errorNotifier;

	// Should be called on init; internaly, or by one of the above proxies

	/** Registeres a set of actions that should be run preferably at once. 
	*	It might be buffered in case of out of order message arrival or pre load messaages.
	*	This recieves ownership of the action. */
	void performActions(ActionStore *);
	void loadGameState(GameStateFiles);
	void saveGameState(GameStateFiles);
	

	// Performed by the system
	virtual void cleanup() override;

private:
	void loadComplete(vector<char> data, unsigned int loadId, CollectionId collection);

	class LoaderCallback : public ThreadNotifyCallback< vector<char> > {
		class LoadCompleteMessage : public ThreadMessage {
			GameStateThread& thread;
			unsigned int loadId;
			CollectionId collectionId;
			vector<char> data;
		public:
			LoadCompleteMessage(GameStateThread& thread, vector<char> d, unsigned int loadId, CollectionId collection);
			bool activate() override;
		};

		GameStateThread& thread;
		unsigned int loadId;
		CollectionId collectionId;
	protected:
		LoadCompleteMessage* getSendMessage(vector<char>) override;
	public:
		LoaderCallback(GameStateThread& thread, unsigned int load, CollectionId collection);
	};
};