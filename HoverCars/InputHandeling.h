#ifndef INPUTHANDELING_H
#define INPUTHANDELING_H

#include "common.h"
#include <list>

struct KeyEntry{
	int keyCode;
	int carID;
	list<int> engineIDs;
};

class KeyTranslator{
	list<KeyEntry> quickKeys;
public:
	void handleKeys();
	void addEntry(KeyEntry entry);
};

#endif