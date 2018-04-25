#include "UdpSeverCoreWin.h"
#include <WS2tcpip.h>
#include "SafeStatisticsLogger.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <map>

class WSAsystem {
	static WSADATA wsa;
	static bool initiated;
public:
	static bool init();
	static bool isInit();
};

//WSADATA WSAsystem::wsa;
bool WSAsystem::initiated = false;
WSADATA WSAsystem::wsa;

class MasterConnection {
	MasterConnection();
public:
	SOCKET masterSocket;
	static MasterConnection& getInstance();
};


class UDPServerCoreWin : public UDPServerCore {
	static const int MAX_BUFFER_SIZE = 66000;
	SOCKET mySocket;
	DWORD threadID;
	HANDLE threadHandle;
	SOCKADDR_STORAGE myAdress;
	int myAdressLength;
	SOCKADDR_STORAGE lastRecieve;
	SOCKADDR_STORAGE masterAdress;
	std::map<SourceID::Type, SOCKADDR_STORAGE> targetAdresses;
	CharBlob recieverBlob;

public:
	UDPServerCoreWin();
	~UDPServerCoreWin();
	bool masterCall(vector<char>& data);

	virtual bool registerAddress(SourceID::Type targetType, NetworkAdressSpecifier& targetAdress);

	/** Platform spesific implementation that gets a newed message
	*	from udp network. Parses sender.
	*	Blocking. Caller gets ownership. */
	virtual RecievedMessage* getMessage();

	virtual bool sendTo(SourceID::Type target, vector<char>& data);

	/** Sends a message to the last source recieved from */
	virtual void reply(vector<char>& data);

	struct ServerParam {
		ServerBase* handler;
		UDPServerCoreWin* parent;
		ServerParam(ServerBase* h, UDPServerCoreWin* p) :
			handler(h), parent(p) {}
	};

	/** Runs the server in a new thread. Platform spesific implementation.
	*	Should call startPump(handler) in new thread. */
	virtual void startServer(ServerBase* handler);

	static DWORD WINAPI startThread(LPVOID lpParameter);
};


bool WSAsystem::init()
{
	if (!init) {
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			LOG_ERROR("Starting up winsoc failed. Error Code : " << WSAGetLastError());
			return false;
		}
		else {
			initiated = true;
			return true;
		}
	}
	return true;
}

bool WSAsystem::isInit()
{
	return initiated;
}

//WinServerConnection::WinServerConnection(ConnectionID i, PermissionLevel p) : Connection(i, p)
//{
//	WSAsystem::init();
//}
//
//void WinServerConnection::send(ServerData data)
//{
//	sockaddr_in dest;
//	//sockaddr_in local;
//	//local.sin_family = AF_INET;
//	//local.sin_addr.s_addr = INADDR_ANY;
//	//local.sin_port = 0; // choose any
//
//	dest.sin_family = AF_INET;
//	dest.sin_addr.s_addr = getID().getIp();
//	dest.sin_port = htons(getID().getPort());
//
//	// create the socket
//	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if (s == INVALID_SOCKET) {
//		LOG_ERROR("Unable to create socket for connection: " << WSAGetLastError())
//	}
//	// bind to the local address
//	//bind(s, (sockaddr *)&local, sizeof(local));
//	// send the pkt
//	int ret = sendto(s, data.data(), data.size(), 0, (sockaddr *)&dest, sizeof(dest));
//	closesocket(s);
//}

UDPServerCoreWin::UDPServerCoreWin()
{
	WSAsystem::init();
	threadID = 0;
	mySocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	int off = 0;
	setsockopt(mySocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off));
	bind(mySocket, (sockaddr*)&in6addr_any, sizeof(in6addr_any));
	myAdressLength = sizeof(SOCKADDR_STORAGE);
	getsockname(mySocket, (sockaddr*)&myAdress, &myAdressLength);
	recieverBlob.m_data = new char[MAX_BUFFER_SIZE];
}

UDPServerCoreWin::~UDPServerCoreWin()
{
	closesocket(mySocket);
	if (threadID) {
		TerminateThread(threadHandle, 1);
	}
	delete[] recieverBlob.m_data;
}

bool UDPServerCoreWin::masterCall(vector<char> & data)
{
	SOCKET masterSocket = MasterConnection::getInstance().masterSocket;
	int res = sendto(masterSocket, data.data(), data.size(), 0, (sockaddr*)&myAdress, myAdressLength);
	return (res == data.size());
}

RecievedMessage * UDPServerCoreWin::getMessage()
{
	recieverBlob.m_size = recvfrom(mySocket, recieverBlob.m_data, MAX_BUFFER_SIZE, 0, (SOCKADDR *) &lastRecieve, nullptr);
	RecievedMessage* ret = new RecievedMessage(&recieverBlob);
	if (memcmp(&lastRecieve, &masterAdress, sizeof(SOCKADDR_STORAGE)) == 0) {
		ret->sourceType = SourceID::Master;
	}
	return ret;
}

bool UDPServerCoreWin::sendTo(SourceID::Type target, vector<char> & data)
{
	if (targetAdresses.find(target) == targetAdresses.end()) {
		return false;
	}
	SOCKADDR* address = (SOCKADDR*)&(targetAdresses[target]);
	int sendt = sendto(mySocket, data.data(), data.size(), 0, (SOCKADDR*)& address, sizeof(SOCKADDR_STORAGE));
	if (sendt != data.size()) return false;
	return true;
}

void UDPServerCoreWin::reply(vector<char> & data)
{
	sendto(mySocket, data.data(), data.size(), 0, (SOCKADDR*)&lastRecieve, sizeof(lastRecieve));
}

void UDPServerCoreWin::startServer(ServerBase * handler)
{
	threadHandle = CreateThread(0, 0, startThread, new ServerParam(handler, this), 0, &threadID);
}

DWORD UDPServerCoreWin::startThread(LPVOID lpParameter)
{
	ServerParam* param = (ServerParam *)lpParameter;
	param->parent->startPump(param->handler);
	delete param;
	return 0;
}

MasterConnection::MasterConnection() {
	WSAsystem::init();
	masterSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	bind(masterSocket, (sockaddr*)&in6addr_any, sizeof(in6addr_any));
}

MasterConnection & MasterConnection::getInstance()
{
	static MasterConnection master;
	return master;
}

UDPServerCore * UDPServerCoreWinFactory::makeCore()
{
	return new UDPServerCoreWin();
}
