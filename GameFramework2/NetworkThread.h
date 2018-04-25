#pragma once
#include "ErrorCodes.h"
#include "ThreadBase.h"
#include "ThreadMessageBase.h"
#include "ThreadProxyCaller.h"
#include "FunctionThreadCallback.h"
#include "ActionProtocol.h"
#include "UDPServerCore.h"
#include "GameClient.h"
#include "GameServer.h"
#include "QueueBuffer.h"
#include "GameStateThread.h"

class NetworkThread : public Thread {
	UDPServerCore::Factory* coreProvider;
	SimpleGameServer* server;
	SimpleGameClient* client;
	NetworkHomeThreadDesc clientHomeDesc;
	NetworkHomeThreadDesc serverHomeDesc;
	PlayerStore playerStatus;
	ActionStore workingStore;
	ActionTimeStep currentTime;
	QueueBuffer<ActionStore> confirmedActions;
	GameStateThread& gameStateThread;

	/** Renser opp med hard hånd. 
	*	Ingen stopp blir sendt til klient eller server; 
	*	Trådene blir bare terminert */
	virtual void cleanup();
//	virtual bool activateMessage(ThreadMessage* m) { return m->activate(*getSystem()); };

	void commonErrorHandle(UDPMsg::ErrorCode error, std::string description);
protected:
	virtual void onInitialized();

public:
	NetworkThread(UDPServerCore::Factory* coreFactory, GameStateThread& gameState); 	
	//Notification slots
	ThreadNotifierBase<ErrorCodes::Message> errorNotifier;
	ThreadNotifierBase<PlayerID> playerIdRecievedNotifier;
	
	// Convinience objects for sending call messages from foreign thread
	ThreadProxyCallerFactory<NetworkThread, void, NetworkAction*> sendActionProxy;
	ThreadProxyCallerFactory<NetworkThread, bool> startServerProxy;
	ThreadProxyCallerFactory<NetworkThread, bool> stopServerProxy;
	ThreadProxyCallerFactory<NetworkThread, bool> startClientProxy;
	ThreadProxyCallerFactory<NetworkThread, bool, NetworkAdressSpecifier, std::string, std::string> connectToServerProxy;
	ThreadProxyCallerFactory<NetworkThread, bool, std::string> trySetPasswordProxy;
	ThreadProxyCallerFactory<NetworkThread, bool> stopClientProxy;
	ThreadProxyCallerFactory<NetworkThread, bool> doTickProxy;
	
	// Important! All these following functions should only be called from this object's own thread!
	// Use threadMessage classes or proxies (above) for the remote way to call these.

	/** Sends a newed network action. This function takes ownership of the action. */
	void sendAction(NetworkAction * action);

	bool startServer();
	bool stopServer();

	bool startClient();
	bool connectToServer(NetworkAdressSpecifier serverAdress, std::string username, std::string password);
	bool trySetPassword(std::string password);
	bool stopClient();

	/** Sends the current messagestore from the server; confirming it. */
	bool doTick();


	// Callback functions, only to be called from the bellow callback helpers

	void handleClientError(UDPMsg::ErrorCode error);
	void handleServerError(UDPMsg::ErrorCode error);

	void serverAction(NetworkAction* action);

	void playerIdRecieved(PlayerID id);
	void actionsRecieved(ActionStore* actions);

private:
	FunctionThreadCallback< NetworkThread, UDPMsg::ErrorCode, &handleClientError> clientErrorCallback;
	FunctionThreadCallback< NetworkThread, UDPMsg::ErrorCode, &handleServerError> serverErrorCallback;
	FunctionThreadCallback< NetworkThread, NetworkAction*, &serverAction> serverActionCallback;
	FunctionThreadCallback< NetworkThread, PlayerID, &playerIdRecieved> playerIdCallback;
	FunctionThreadCallback< NetworkThread, ActionStore*, &actionsRecieved> actionsRecievedCallback;
};

