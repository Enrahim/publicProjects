#pragma once
#include "Serialization.h"
#include "GameStateThread.h"

struct PtrBase {
	/** The current game state thread owning the ptr system */
	static GameStateThread* manager;

	/** All pointers point toward a given id in a collection */
	int id;
	bool isValid() { return (id != -1); }
};


/** Pointer to persistent in witch the context always clearly defines in what
*	component collection you will find the component in question */
template<typename T, unsigned short OBJTYPE, unsigned short COLL, int COMP = 0>
struct SmallSlowComponentPtr : private PtrBase {
	void load(LoadReader r) {
		id = r.readInt();
	}
	void save(SaveWriter s) {
		s.write(id);
	}

	T& operator*(){
		return manager->getManager(OBJTYPE, COLL)->fetchData<T>(id, COMP);
	}
};



/** Pointer to a persistent component in a given collection */
template <typename T>
struct FastComponentPtr : private PtrBase {
	ComponentCollection<T>* base;

	void load(LoadReader& r){
		id = r.readInt();
		unsigned short collectionType = r.readShort();
		unsigned short collectionID = r.readShort();
		unsigned char component = r.readChar();
		base = manager->getManager(collectionType, collectionID)->
			fetchCollection(component);
	}
	void save(SaveWriter& s) {
		s.write(id);
		s.write(base->manager->getObjectType());
		s.write(base->manager->getCollectionId());
		s.write(base->componentID);
		s.write(component);
	}

	T& operator*() {
		return base->data[id];
	}
};

/** pointer to an object in a object manager of type T */
template <typename T> 
struct ObjectPointer{
	T* objectManager;
	void load(LoadReader& reader) {
		unsigned short collectionID = reader.readShort();
		objectManager = manager->GetManager<T>(collectionID);

	}

	void save(SaveWriter& writer) {
		//TODO fix this
	}
};