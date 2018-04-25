#pragma once

#include "Cryptography.h"
#include "Serialization.h"
#include <map>

typedef unsigned int PlayerID;

struct PlayerStatus {
	PlayerID id;
	string username;
	long long passwordHash;
	DecryptingState decrypter;

	/** Save playerstatus except decryption state */
	SIMPLE_IO_CALLS;
	template <typename T>
	void io(T ioSys) {
		ioSys.io(id);
		ioSys.io(CollectionWrapper<std::string>(username));
		ioSys.io(passwordHash);
	}
};

/** Stores relevant login information and active status of players */
class PlayerStore {

	std::map<std::string, PlayerID> userNamesToId;
	std::map<PlayerID, PlayerStatus> players;

public:
	long long getPasswordHash(PlayerID);
	PlayerID getPlayerId(string username);
	void storeKeySuggetion(PlayerID pId, long long key);
	DecryptingState* getDecrypter(PlayerID pId);

	/** Saves playerdata except cryptationstatus */
	void serialize(SaveWriter s);
	/** Refresh playerstatus. 
	*	Decryption status is reset as well, so all players must reconnect */
	void deserialize(LoadReader r);
};