#pragma once

#include "ErrorCodes.h"
#include "ActionProtocol.h"
#include "ThreadMessageBase.h"
#include "ThreadQuery.h"
#include "ThreadNotificationSystem.h"
#include "FileSpecifier.h"


class GameSystemInterface {
	static GameSystemInterface* instance;

protected:
	GameSystemInterface() { ; }
public:

	/** Should be called once before first call to get instance
	*	@param gameState A gamestatepersister that implisitely defines the gamestate the system will be working on.
	*					Should be provided from the gamestate layer.
	*	@param actionReg An ActionRegisterer that defines the actions defined for manipulation of the gamestate
	*					Should be provided from the action layer.
	*/
	static void initiate(GamestatePersister& gameState, ActionRegisterer& actionReg );

	/** Returns the singleton; instancated in initiate
	*	Before initiate, will return 0. */
	static GameSystemInterface* getInstance() { return instance; }


	/////////////////////////////
	// Network related functions
	/////////////////////////////

	virtual void startServer() = 0;
	virtual void stopServer() = 0;
	virtual void connect(std::string address, int port, std::string username, std::string password) = 0;
	virtual void disconect() = 0;
	virtual void registerNetworkErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback) = 0;

	//////////////////////////////
	// Gamestate related functions
	//////////////////////////////
	
	virtual void load(GameStateFiles files) = 0;
	/** Obs any state from a load of the files is perserved (specially read only!) */
	virtual void save(GameStateFiles files) = 0;

	virtual void registerGamestateErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback) = 0;
	virtual void registerFileSystemErrorListener(ThreadNotifyCallback<ErrorCodes::Message>* callback) = 0;

	/** The message sender function to return subsequent queries */
	virtual void setQueryReturner(MessageSender& sender) = 0;
	
	/** Sends a newed query. This function get ownership. 
	*	This is usefull for UI defined queries. */
	virtual void sendQuery(IThreadQuery* query) = 0;

	/** Allows running anything on gamestatethread.
	*	Should be limited to messages defined by the action provider.
	*	Particularily usefull for RegisterThreadNotifyMessage instances. */
	virtual void sendGamestateMessage(ThreadMessage* message) = 0;

	/////////////////////////////////////////////////
	// Functions affecting both network and gamestate
	/////////////////////////////////////////////////
	virtual void sendAction(NetworkAction* action) = 0;

};