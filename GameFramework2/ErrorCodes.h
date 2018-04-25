#pragma once
#include <string>

namespace ErrorCodes {
	typedef short Type;
	struct Message {
		Type type;
		std::string message;
		Message(Type t, std::string m) : type(t), message(m) { ; }
	};

	static const Type SAVE_FAILED = 100;
	static const Type LOAD_FAILED = 101;

	static const Type VERSONING_ERROR = 200;
	static const Type COLLECTION_ERROR = 201;
	// Network errors are NETWORK_ERROR_BASE + char defined at network layer.
	static const Type NETWORK_ERROR_BASE = 1000;
}