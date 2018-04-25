#pragma once
#include "UDPServerProtocol.h"
#include "ActionProtocol.h"

namespace UDPMsg {
	typedef unsigned int ServerStepCount;

	namespace MessageType {
		const Type CONNECT_LOGIN = 20;
		const Type ACCEPT_LOGIN = 21;
		const Type REQUEST_DO_ACTION = 22;
		const Type CONFIRMED_ACTIONS = 25;
	}

	/** First call from client */
	struct ConnectLogin {
		std::string username;
		unsigned long long cryptBase;
		unsigned long long cryptRes;

		SIMPLE_IO_CALLS;
		template<typename T>
		void io(T& ioSys) {
			ioSys.io(CollectionWrapper<std::string>(username));
			ioSys.io(cryptBase);
			ioSys.io(cryptRes);
		}
	};

	/** Reply from server */
	struct AcceptLogin {
		PlayerID playerID;
		unsigned long long cryptRes;
		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
			ioSys.io(playerID);
			ioSys.io(cryptRes);
		}
	};

	struct ActionRequestHeader {
		PlayerID source;
		int cryptNum;
		CheckSum checkSum;
		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
			ioSys.io(source);
			ioSys.io(cryptNum);
			ioSys.io(checkSum);
		}
	};

	/**
	struct ActionRequest {
		ActionRequestHeader header;
		ServerData data;
	};

	struct ConfirmedActions {
		ServerStepCount stepNr;
		ActionStore actions;
	};
	*/
}