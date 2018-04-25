#include "UDPServerCore.h"

//clock_t Connection::TIMEOUT_TIME = 300;

bool ServerBase::sendToMe(vector<char> & data)
{
	return core->masterCall(data);
}

bool ServerBase::shutDownRequest(SourceID::Type source)
{
	if (source == SourceID::Master) {
		return false;
	}
	else {
		handleError(UDPMsg::ERROR_CODES::AUTHENTICATION_ERROR);
		return true;
	}
}

void ServerBase::handleError(UDPMsg::ErrorCode error)
{
	errorCallback->call(error);
	vector<char> errorMessage;
	VectorOutStream stream(errorMessage);
	SaveWriter writer(stream);
	writer.write(UDPMsg::MessageType::MESSAGE_ERROR);
	writer.write(error);
	core->reply(errorMessage);
}

ServerBase::ServerBase(BaseServerConfig& config) :
	core(config.core),
	threadSys(config.home->threadSystem),
	networkID(config.home->networkThreadID),
	errorCallback(config.home->errorCallback),
//	errorLog(config.errorPolicy),
	loginPolicy(config.loginPolicy)
{
//	connections[config.home->homeConnection] = core->getNewConnection(config.home->homeConnection, Connection::MASTER);
}

void ServerBase::start()
{
	if (!running) {
		running = true;
		core->startServer(this);
	}
}

ServerBase::~ServerBase() 
{
	if (core) delete core;
	//for (auto it = connections.begin(); it != connections.end(); it++) {
	//	if (it->second) delete it->second;
	//}
}

void ServerBase::pump()
{
	while (true) {
		RecievedMessage* now = core->getMessage();
		//if (!errorLog.acceptRecieve(now->from)) {
		//	delete now;
		//	continue;
		//}
		//auto sourceCon = connections.find(now->from);
		//if (sourceCon == connections.end()) {
		//	if (loginPolicy.unknownAllowed) {
		//		connections[now->from] = core->getNewConnection(now->from);
		//		sourceCon = connections.find(now->from);
		//	}
		//	else {
		//		delete now;
		//		continue;
		//	}
		//}
		//sourceCon->second->recieve();
		
		if (now->success && !handleMessage(*now->data, now->sourceType)) {
			delete now;
			break;
		}
		else if(!now->success) {
			errorCallback->call(UDPMsg::ERROR_CODES::RECIEVE_FAILED);
		}
		delete now;
	}
	running = false;
	// Shut down actions should have been handled in handle Message.
	errorCallback->call(UDPMsg::ERROR_CODES::SERVER_STOPPED);
}

//void Connection::recieve() {
//	lastRecieve = clock();
//}
//
//bool ServerNetworkErrorLog::acceptRecieve(ConnectionID id)
//{
//	auto it = errorLog.find(id);
//	if (it == errorLog.end()) return true;
//	if (it->second.numErrors < policy.maxErrors) return true;
//	if (clock() - it->second.lastError > policy.forgivementTime) {
//		it->second.forgivenErrors += it->second.numErrors;
//		it->second.numErrors = 0;
//		return true;
//	}
//	return false;
//}
//
//bool ServerNetworkErrorLog::replyError(Connection& connection)
//{
//	if (connection.testPermission(Connection::MASTER)) return true;
//	ErrorData data = errorLog[connection.getID()];
//	data.lastError = clock();
//	data.numErrors++;
//	return(data.numErrors < policy.errorTolerance || (data.numErrors & policy.replyMask) == 1);
//}
//
//bool Connection::timeOut()
//{
//	if ( testPermission(MASTER) || testPermission(SERVER) ) return false; //These permisions should not be removed.
//	return (clock() - lastRecieve > TIMEOUT_TIME);
//} 
