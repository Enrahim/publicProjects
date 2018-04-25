#pragma once
#include <vector>
#include <list>
#include <map>
#include <set>

#include "UDPServerCore.h"



class RelayServer : public ServerBase {
public:
	struct Config {
		int RECOVER_LIMIT = 100;
		int STEP_DELAY_MARGINS = 3;
	};

private:

	Config config;

	Connection* loginServer;
	Connection* topServer;
	std::vector<Connection*> sourceServers;
	std::list<Connection*> clients;
//	ConnectionID master;
	// True if this server recieves actions directly from the topserver.
	bool directConnect;
	
	
	std::map<ServerStepCount, ServerData> downStream;
	std::set<ServerStepCount> missingFields;


	void broadcastShutDownMessage();

	void reportCheat(ServerData& data, Connection& from);
	void sendLoginRequest(ServerData& data);
	void connectServer(ServerData& data);
	void connectClient(ServerData& data);
	bool requestDoActions(ServerData& data);
	void confirmedActions(ServerData&data);
	void resendActions(Connection& from, ServerData& data);
	void natBreak(ServerData& data);

public:
	virtual bool handleMessage(ServerData& data, Connection& from);
};