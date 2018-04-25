#include "InputHandeling.h"
#include "GameState.h"

void KeyTranslator::handleKeys() {
	list<KeyEntry>::iterator it1=quickKeys.begin();
	while(it1!=quickKeys.end()) {
		if(GetAsyncKeyState(it1->keyCode)) {
			list<int>::iterator it2=it1->engineIDs.begin();
			while(it2!=it1->engineIDs.end()) {
				gGameState.activateEngine(it1->carID, *it2);
				it2++;
			}
		}
		it1++;
	}
}

void KeyTranslator::addEntry(KeyEntry entry) {
	quickKeys.push_back(entry);
}