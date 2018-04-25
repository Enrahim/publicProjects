#include "PlayerStore.h"

long long PlayerStore::getPasswordHash(PlayerID id)
{
	return players[id].passwordHash;
}

PlayerID PlayerStore::getPlayerId(string username)
{
	auto it = userNamesToId.find(username);
	if (it != userNamesToId.end()) {
		return it->second;
	}
	return 0;
}

void PlayerStore::storeKeySuggetion(PlayerID pId, long long key)
{
	auto it = players.find(pId);
	if (it != players.end()) {
		it->second.decrypter.suggestKey(key);
	}
}

DecryptingState * PlayerStore::getDecrypter(PlayerID pId)
{
	auto it = players.find(pId);
	if (it != players.end()) {
		return &(it->second.decrypter);
	}
	return nullptr;
}

void PlayerStore::serialize(SaveWriter s)
{
	s.write(players.size());
	for (auto it = players.begin(); it != players.end(); it++) {
		it->second.serialize(s);
	}
}

void PlayerStore::deserialize(LoadReader r) {
	size_t num;
	r.read(&num);
	for (int i = 0; i < num; i++) {
		PlayerStatus tmp;
		tmp.deserialize(r);
		userNamesToId[tmp.username] = tmp.id;
		players[tmp.id] = tmp;
	}
}

