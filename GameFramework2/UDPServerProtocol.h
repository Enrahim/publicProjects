#pragma once

#include "PlayerStore.h"
#include "ServerData.h"
#include <string>

/** Not very efficient, but for the time being quite universal
*	way for indicating a adress in all relevant adress spaces */
struct NetworkAdressSpecifier {
	std::string adress; // Domain name should be supported. Other formats could be platform defined.
	std::string port;
	SIMPLE_IO_CALLS;
	template<typename T>
	void io(T& ioSys) {
		ioSys.io(CollectionWrapper<std::string>(adress));
		ioSys.io(CollectionWrapper<std::string>(port));
	}
	NetworkAdressSpecifier() { ; }
	NetworkAdressSpecifier(std::string a, std::string p) : adress(a), port(p) { ; }
};

namespace UDPMsg {
	typedef unsigned int EncryptionSequence;
	typedef unsigned char ErrorCode;
	/**
	enum UDPMessageType {
		MESSAGE_ERROR,
		SHUT_DOWN,
		START_CONNECT_LOGIN,
		HANDSHAKE1,
		HANDSHAKE_BACK,
		CONNECT_LOGIN,
		ACCEPT_LOGIN,
		AUTHENTICATE,
		CONNECT_SERVER,
		CONNECT_CLIENT,
		CONNECT_PLAYER,
		NAT_BREAK,
		DISCONNECT_SERVER,
		DISCONNECT_CLIENT,
		REQUEST_DO_ACTION,
		CONFIRMED_ACTIONS,
		RESEND_ACTIONS,
		DO_TICK,
		CHEAT_REPORT,
		QUERRY
	};
	*/
	namespace MessageType {
		typedef unsigned char Type;
		const Type MESSAGE_ERROR = 0;
		const Type SHUT_DOWN = 1;		
	}

	namespace ERROR_CODES {
		const ErrorCode UNKNOWN_ERROR = 0;
		const ErrorCode MESSAGE_TYPE_UNKNOWN = 1;
		const ErrorCode PERMISSION_ERROR = 2;
		const ErrorCode AUTHENTICATION_ERROR = 3;
		const ErrorCode IMPERSONATION_ERROR = 4;
		const ErrorCode RECIEVE_FAILED = 5;
		//Not neccesarily an error; also sendt when asked to stop.
		//When this is sendt the thread should be safe to delete.
		const ErrorCode SERVER_STOPPED = 100;
		
		/** Add this number to indicate that this is a code recieved from outside */
		const ErrorCode FEEDBACK_OFFSET = 32;
	}

	class NetworkMessageWriter : public SaveWriter {
		vector<char> m_data;
		VectorOutStream stream;
	
	public:
		NetworkMessageWriter(MessageType::Type type) :
			m_data(),
			stream(m_data),
			SaveWriter(stream)
		{
			write(type);
		}
		vector<char>& getData() { return m_data; }
	};


	struct StartConnectLogin {
		std::string username;
		std::string password;
		//ConnectionID server;
		template<typename T> 
		void io(T& ioSys) {
			ioSys.io(CollectionWrapper<std::string>(username));
			ioSys.io(CollectionWrapper<std::string>(password));
//			ioSys.io(server);
		};
		SIMPLE_IO_CALLS;	
	};

	struct Handshake {
		std::string key;
		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
			ioSys.io(CollectionWrapper<std::string>(key));
		}
	};


	/** Client confirms identity based on agreed encryption using password
	*	Actually no neccessary with backlog aut, only updating when 
	*	new message is recieved with correct checksum with new key! 
	*	This aproach would likely be more vournerable! */
	struct Authenticate {
		PlayerID playerID;
		string testCrypt;

		SIMPLE_IO_CALLS;
		template <typename T>
		void io(T& ioSys) {
			ioSys.io(playerID);
			ioSys.io(CollectionWrapper<std::string>(testCrypt));
		}
	};

	struct ConnectPlayer {
//		ConnectionID target;
		PlayerID playerID;
		std::string key;
		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
//			ioSys.io(target);
			ioSys.io(playerID);
			ioSys.io(CollectionWrapper<std::string>(key));
		}
	};

	struct ConnectOther {
//		ConnectionID target;
		std::string key;

		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
//			ioSys.io(target);
			ioSys.io(CollectionWrapper<std::string>(key));
		}
	};

	struct Disconnect {
//		ConnectionID target;
		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
//			ioSys.io(target);
		}
	};



	struct ResendActions {
//		ServerStepCount start;
//		ServerStepCount end;
		SIMPLE_IO_CALLS;

		template <typename T>
		void io(T& ioSys) {
//			ioSys.io(start);
//			ioSys.io(end);
		}
	};
}