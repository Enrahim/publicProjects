#include "GameStateThread.h"
#include "ObjectPointers.h"
#include "ThreadMessageBase.h"
#include "SafeStatisticsLogger.h"

void GameStateThread::initialize() {
	PtrBase::manager = this;
}

bool GameStateThread::activateMessage(ThreadMessage *m)  { 
	return m->activate(*this); 
}

void GameStateThread::cleanup() {

}

ObjectTypeManager* GameStateThread::getManager(unsigned short type, unsigned short collection) {
	ObjectTypeManager*& res = types[type][collection];
	if (res == NULL) {
		res = createManager(type, collection);
	}
	return res;
}


// APPLICATION SPESIFIC!
ObjectTypeManager* createManager(unsigned short type, unsigned short index) {
#define TYPE_CHOICE_ENTRY(a) case (a)::typeID : return new (a)(index);
	switch (type) {
		// One type choice entry should be added for each object type registered in the system
	}
	LOG_ERROR("Unable to find a type corresponding with typeID: " << type);
	return NULL; 
}