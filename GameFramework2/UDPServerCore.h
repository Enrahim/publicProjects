#pragma once

#include "Serialization.h"
#include "ThreadSystem.h"
#include "ThreadNotificationSystem.h"
#include "UDPServerProtocol.h"
#include <map>
#include <vector>
#include <ctime>

typedef unsigned int ServerStepCount; //the timestamp precission.


namespace SourceID {
	typedef unsigned char Type;
	const Type Ukjent = 0;
	const Type Client = 1;
	const Type MultiCast = 10;
	const Type Server = 50;
	const Type Master = 100;
}

struct RecievedMessage {
	//ConnectionID from;
	CharBlob* data;
	bool success;
	SourceID::Type sourceType;
	RecievedMessage() {};
	RecievedMessage(CharBlob* d) : data(d) { ; }
};

class UDPServerCore;

struct NetworkHomeThreadDesc {
	IThreadSystem* threadSystem; 
	ThreadID networkThreadID;
	ThreadNotifyCallback<UDPMsg::ErrorCode>* errorCallback; //caller retains ownership
	//ConnectionID homeConnection;
	NetworkHomeThreadDesc(IThreadSystem* sys, ThreadID thread) :
		threadSystem(sys), networkThreadID(thread) {;}
	
	NetworkHomeThreadDesc() { ; }
};


struct LoginPolicy {
	bool unknownAllowed;
	long long sessionPassword;
	LoginPolicy() : unknownAllowed(true), sessionPassword(0) { ; }
};

struct BaseServerConfig {
	UDPServerCore* core; //Server gains overship
	NetworkHomeThreadDesc* home; //Caller retains ownership
//	ServerNetworkErrorLog::SecurityPolicy errorPolicy;
	LoginPolicy loginPolicy;
};

/** Interface that must be implemented by a spesific server type */
class ServerBase {
	ThreadNotifyCallback<UDPMsg::ErrorCode>* errorCallback;
	ThreadID networkID;
	LoginPolicy loginPolicy;

	bool running;

	friend UDPServerCore;
	/** Called by platform implemented startServer function */
	void pump();

protected:
	IThreadSystem* threadSys;
	UDPServerCore* core;
	//std::map<ConnectionID, Connection*> connections;
	//ServerNetworkErrorLog errorLog;

	/** Handles a decrypted message. return false if server should shut down. */
	virtual bool handleMessage(CharBlob& data, SourceID::Type source) = 0;
	
	/** Sends the given *new* message. Reciever gets ownership. */
	void sendHome(ThreadMessage* msg) { threadSys->sendMessage(networkID, msg); }

	/** Handles a request to shut down 
	*	@param type indicates the recognized authority of the source.
	*				shut down should only be accepted form master.
	*	@ return true if the server should indeed shut down. */
	bool shutDownRequest(SourceID::Type source);
	void handleError(UDPMsg::ErrorCode error);


	/** Makes a new server with the given servercore.
	*	This server gains ownership of the core. */
	ServerBase(BaseServerConfig& config);

public:

	/** Start the server in a new thread. All config should be done before call to this. */
	void start();

	/** Send a message to this server via master connection
	*	Should be used from network thread.
	*	@return false on error. Quite critical as this might complicate thread cleanup */
	bool sendToMe(vector<char>& data);

	virtual ~ServerBase();
};

/** Handles platform spessific threading and udp communication */
class UDPServerCore {
protected:
	/** Only access to start pumping. Called by startServer */
	void startPump(ServerBase* handler) { handler->pump(); }
public:
	/** Send a message from masterConnection, to recieverThread
	*	Suggestion: Use a singletonSocket to send to core socket
	*	@return if there is a failure (this is critical, as clean thread end might not be possible */
	virtual bool masterCall(vector<char>& data) = 0;

	/** Platform spesific registration of Source categories
	*	Typically called by the factory that makes the UDPServer cores on Network-thread layer
	*	before this core is supplied to a BaseServer implementation.
	*	Convinience method for enabling potential cross-platform reuse.
	*	Implementations of UDPServer core are encurraged have platform spesific implementations along with
	*	thread level factories that provides more efficient initialization without parsing.
	*	@param targetType What kind of target you want to register.
						Most target types are unique (like MASTER and most servers) and will be overwritten.
	*	@param targetAdress an adress that will be parsed by the platform to provide an adress
	*	@return Wetter the target type is valid, and the targetAdress was a parsable and valid address.
	*			Note: a lazy implementation is free to always return false, as long as it provides
	*				a platform spesific way of spesifying what is needed for sendTo.
	*/
	virtual bool registerAddress(SourceID::Type targetType, NetworkAdressSpecifier& targetAdress) = 0;


	/** Platform spesific implementation that gets a newed message
	*	from udp network. Parses sender.
	*	Blocking. Caller gets ownership. */
	virtual RecievedMessage* getMessage() = 0;

	/** Sends a message to the last source recieved from */
	virtual void reply(vector<char>& data) = 0;

	/** Sends a message to the network target.
	*	@param target A unique source type that is targetable
	*			for instance CLIENT would usually not be available
	*			SERVER would not be available for non-client implementations.
	*			MASTER is not callable, use sendHome instead.
	*	@return Wether target is recognized as a valid target, and message is sent. */
	virtual bool sendTo(SourceID::Type target, vector<char>& data) = 0;

	/** Runs the server in a new thread. Platform spesific implementation.
	*	Should call startPump(handler) in new thread. */
	virtual void startServer(ServerBase* handler) = 0;

	class Factory {
	public:
		virtual UDPServerCore* makeCore() = 0;
	};

	/** Virtual entrypoint for destructors */
	virtual ~UDPServerCore() {}

	/** Return a new connection for the approperiate platform */
//	virtual Connection* getNewConnection(ConnectionID i, Connection::PermissionLevel p = Connection::CLIENT) = 0;

};



// Old connection system; not IPv6 Compatible.

//
//class ConnectionID {
//	unsigned int ip;
//	unsigned short port;
//public:
//	ConnectionID(unsigned int i, unsigned int p) : ip(i), port(p) { ; }
//	virtual bool operator< (const ConnectionID& con) const {
//		if (ip < con.ip) return true;
//		if (ip > con.ip) return false;
//		return port < con.port;
//	}
//
//	unsigned int getIp() { return ip; }
//	unsigned short getPort() { return port; }
//
//	template <typename T>
//	void io(T& ioSys) {
//		ioSys.io(ip);
//		ioSys.io(port);
//	}
//
//	SIMPLE_IO_CALLS;
//
//};
//
//class Connection {
//public:
//	enum PermissionLevel { CLIENT=1, SERVER=2, USECURE_LOGIN_SERVER = 4, LOGIN_SERVER=8, MASTER=16 };
//
//private:
//	ConnectionID id;
//	PermissionLevel permissions;
//
//	static clock_t TIMEOUT_TIME;
//	clock_t lastRecieve;
//
//public:
//	Connection(ConnectionID i, PermissionLevel p) : id(i), permissions(p) { }
//	static void setTimeoutTime(clock_t time) { TIMEOUT_TIME = time; }
//
//	/** Platform spesific implemenation for sending raw data */
//	virtual void send(ServerData data) = 0;
//
//
//	virtual ~Connection() {
//	}
//
//	ConnectionID getID() { return id; }
//	PermissionLevel getPermissions() { return permissions; }
//	bool testPermission(PermissionLevel p) { return p | permissions; }
//	
//	/** Called to notify that message is recieved */
//	void recieve();
//
//	/** Can be called on regular intervals on open connections to check if it should be freed */
//	bool timeOut();
//};
//
//class ServerNetworkErrorLog {
//public:
//	struct SecurityPolicy {
//	private:
//		static const unsigned char MAX_ERRORS = 10;
//		static const unsigned char ERROR_TOLERANCE = 4;
//		static const unsigned int FORGIVEMENT_TIME = 3600;
//		static const unsigned char REPLY_MASK = 3; // reply if this & errors == 1
//	public:
//		unsigned char maxErrors;
//		unsigned char errorTolerance;
//		unsigned int forgivementTime;
//		unsigned char replyMask;
//		SecurityPolicy() : maxErrors(MAX_ERRORS), errorTolerance(ERROR_TOLERANCE), forgivementTime(FORGIVEMENT_TIME), replyMask(REPLY_MASK) { ; }
//	};
//
//private:
//	struct ErrorData {
//		clock_t lastError;
//		unsigned int forgivenErrors;
//		unsigned char numErrors;
//	};
//	map<ConnectionID, ErrorData> errorLog;
//	SecurityPolicy policy;
//public:
//	
//	ServerNetworkErrorLog() { ; }
//	ServerNetworkErrorLog(const SecurityPolicy& p) : policy(p) { ; }
//	void setPolicy(SecurityPolicy p) { policy = p; }
//	/** Called when recieved to check if it should be blocked due to suspicious errors */
//	bool acceptRecieve(ConnectionID);
//
//	/** Called when error is detected on connection to see if we should reply */
//	bool replyError(Connection&);
//};