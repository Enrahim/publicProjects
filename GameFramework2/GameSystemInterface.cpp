#include "GameSystemInterface.h"
#include "ThreadSystem.h"
#include "ResourceThread.h"
#include "GameStateThread.h"
#include "NetworkThread.h"
#include "ThreadPreDeclarations.h"
#include <sstream>

#ifdef WIN32
#include "ThreadSystemWin.h"
#include "UdpSeverCoreWin.h"
#endif

GameSystemInterface* GameSystemInterface::instance;

class GameSystemImplementation : public GameSystemInterface {
	ResourceThread* gameStateResourceWorker;
	NetworkThread* networkThread;
	GameStateThread* gameStateThread;
	IThreadSystem* threadSystem;
	friend GameSystemInterface;

	MessageSender* hostSenderConnection;

	GameSystemImplementation(GamestatePersister& gameState);

public:
	/////////////////////////////
	// Network related functions
	/////////////////////////////

	void startServer() override;
	void stopServer() override;
	void connect(std::string address, int port, std::string username, std::string password) override;
	void disconect() override;

	virtual void registerNetworkErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback) override;

	//////////////////////////////
	// Gamestate related functions
	//////////////////////////////

	void load(GameStateFiles files) override;
	void save(GameStateFiles files) override;

	virtual void registerGamestateErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback) override;
	virtual void registerFileSystemErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback) override;

	/** The message sender function to return subsequent queries */
	void setQueryReturner(MessageSender& sender) override;

	/** Sends a newed query. This function get ownership.
	*	This is usefull for UI defined queries. */
	void sendQuery(IThreadQuery* query) override;

	/** Allows running anything on gamestatethread.
	*	Should be limited to messages defined by the action provider.
	*	Particularily usefull for RegisterThreadNotifyMessage instances. */
	void sendGamestateMessage(ThreadMessage* message) override;

	/////////////////////////////////////////////////
	// Functions affecting both network and gamestate
	/////////////////////////////////////////////////
	void sendAction(NetworkAction* action) override;

};

void GameSystemInterface::initiate(GamestatePersister& gameState, ActionRegisterer& actionReg)
{
	instance = new GameSystemImplementation(gameState);
	actionReg();
}

GameSystemImplementation::GameSystemImplementation(GamestatePersister& gameState) {
	UDPServerCore::Factory* coreFactory;
#ifdef WIN32
	threadSystem = new ThreadSystemWin();
	coreFactory = new UDPServerCoreWinFactory();
#else
	UNSUPPORTED PLATFORM!!!!;
#endif

	gameStateResourceWorker = new ResourceThread();
	gameStateThread = new GameStateThread(gameState, *gameStateResourceWorker);
	networkThread = new NetworkThread(coreFactory, *gameStateThread);
	threadSystem->startThread(ThreadNames::Resource, gameStateResourceWorker);
	threadSystem->startThread(ThreadNames::GameState, gameStateThread);
	threadSystem->startThread(ThreadNames::Network, networkThread);
}

void GameSystemImplementation::startServer()
{
	networkThread->startServerProxy.makeCall();
}

void GameSystemImplementation::stopServer()
{
	networkThread->stopServerProxy.makeCall();
}

void GameSystemImplementation::connect(std::string address, int port, std::string username, std::string password)
{
	ostringstream stream;
	stream << port;
	networkThread->connectToServerProxy.makeCall(NetworkAdressSpecifier(address, stream.str()), username, password);
}

void GameSystemImplementation::disconect()
{
	networkThread->stopClientProxy.makeCall();
}

void GameSystemImplementation::registerNetworkErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback)
{
	networkThread->sendToMe(new RegisterThreadNotifyMessage<ErrorCodes::Message>(networkThread->errorNotifier, callback));
}

void GameSystemImplementation::load(GameStateFiles files)
{
	gameStateThread->loadGameStateProxy.makeCall(files);
}

void GameSystemImplementation::save(GameStateFiles files)
{
	gameStateThread->saveGameStateProxy.makeCall(files);
}

void GameSystemImplementation::registerGamestateErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback)
{
	gameStateThread->sendToMe(new RegisterThreadNotifyMessage<ErrorCodes::Message>(gameStateThread->errorNotifier, callback));
}

void GameSystemImplementation::registerFileSystemErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback)
{
	gameStateResourceWorker->sendToMe(new RegisterThreadNotifyMessage<ErrorCodes::Message>(gameStateResourceWorker->errorNotifier, callback));
}

void GameSystemImplementation::setQueryReturner(MessageSender & sender)
{
	hostSenderConnection = &sender;
}

void GameSystemImplementation::sendQuery(IThreadQuery * query)
{
	if (hostSenderConnection != nullptr) {
		gameStateThread->sendToMe(new QuerySendMessage(query, *hostSenderConnection));
	}
}

void GameSystemImplementation::sendGamestateMessage(ThreadMessage * message)
{
	gameStateThread->sendToMe(message);
}

void GameSystemImplementation::sendAction(NetworkAction * action)
{
	networkThread->sendActionProxy.makeCall(action);
}


