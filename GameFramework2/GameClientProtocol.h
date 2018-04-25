#pragma once

#include "UDPServerProtocol.h"
namespace UDPMsg {
	namespace MessageType {
		const Type COMMAND_CONNECT_LOGIN = 41;
		const Type COMMAND_REQUEST_DO_ACTION = 44;
	}

	struct CommandConnectLogin {
		NetworkAdressSpecifier address;
		std::string username;
		long long passwordHash;

		SIMPLE_IO_CALLS;
		template<typename T>
		void io(T& ioSys) {
			ioSys.io(address);
			ioSys.io(CollectionWrapper<std::string>(username));
			ioSys.io(passwordHash);
		}
	};

}