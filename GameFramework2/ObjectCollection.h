#pragma once

#include "NonAssignable.h"
#include "Serialization.h"

/** Header for the defines that allows access to structures that act as collection of objects 
*	For performance reasons these collections are implemented without any common base.
*	The implementation should adhere to certain standard patterns described in the example in the end 
*	Most would usually be buildt by the helper components implemented in the other header files */


typedef int ObjectId;
typedef short CollectionId;

/** Interface that must be implemented by the gamestate provider.
*	Should load and save all data for all collections defined in the gamestate.
*	Typically just for each type call:
*	(*( Topcontainer::getCollection<type>(package) ) ).serialize(writer);
*	(*( Topcontainer::getCollection<type>(package) ) ).deserialize(reader); 
*	But gamestate with a packagetype awarness might filter on type. 
*	Any dependencies should be handled a higher level */
class GamestatePersister {
public:
	virtual void load(LoadReader& reader, CollectionId package) = 0;
	virtual void save(SaveWriter& reader, CollectionId package) = 0;
};

/** Pointer that is handed to static functions on the given type to do manipulations on the targeted object 
*	Also ensures serializability, so "safe" to store.
*	Should only be stored raw if certain that the target will persist; else the more sofisticated link system should be considdered */
template<typename T>
class ObjectPointer {
	T* collection;
	ObjectId id;
	friend T;
public:
	ObjectPointer() : collection(nullptr), id(i) { ; }
	ObjectPointer(T* col, ObjectId i) : collection(col), id(i) { ; }
	bool isNull() { return collection == nullptr; };

	void serialize(SaveWriter& s);
	void deserialize(LoadReader& s);

};

/** A pointer wraper for pointers to a collection of objects. Includes saving and restoring features */
template<typename T>
class CollectionPointer {
	T* collection;
public:
	bool isValid() { return collection != NULL; }
	T& operator *() { return *collection; }
	ObjectPointer<T> operator[](ObjectId id) { return ObjectPointer<T>(collection, id); }
	CollectionPointer(T* c) : collection(c) { ; }


	void serialize(SaveWriter& s);
	void deserialize(LoadReader& r);

};


/** For all practical purposes a singleton that serves as an access point to all the object collections in this game
*	Subclasses might get their own collection sets! */
namespace TopContainer{
	/** Gets a pointer to the collection with the given id.
	*	if no collection already exist with that id, creates a new one before returning pointer 
	*	Collection id 0 should never be used, as it is reserved for nullpointers. */
	template <typename T>
	CollectionPointer<T> getCollection(CollectionId id);
};



/** Helper define for idHandling of the collection; the only required thing
*	a must be the name of the class! */
/* Not really usable due to component store requiering object allocation helper as constructor argument.
#define COMMON_COLLECTION_START(a) class a : public NonAssignable{ private: myId; public: a(CollectionId id) : myId(id) {;} CollectionId getId() { return myId; } private:
*/

/*

class MinimalCollection: public NonAssignable {
typedef ObjectPointer<MinimalCollection> myPointer;
const CollectionId myId;

public:
MinimalCollection(CollectionId id) : myId(id) { ; }
CollectionId getId() { return myId; }
void serialize(SaveWriter& s);
void deserialize(LoadReader& s);

// The collection should have static functions on this form to access objects
static void exampleFunction(myPointer& object, param1, param2...);

};
*/

////////////////////////
//	Function definitions
////////////////////////

template<typename T>
inline void CollectionPointer<T>::serialize(SaveWriter& s)
{
	s.output(collection->getId());
}

template<typename T>
inline void CollectionPointer<T>::deserialize(LoadReader& r)
{
	CollectionId id;
	r.read(&id);
	collection = TopContainer::getCollection<T>(id).collection;
}

template<typename T>
inline void ObjectPointer<T>::serialize(SaveWriter& s)
{
	if (!collection) s.write(0);
	else s.write(collection.getId());
	s.write(id);
}

template<typename T>
inline void ObjectPointer<T>::deserialize(LoadReader& r)
{
	CollectionId colId;
	r.read(&colId);
	if(colId == 0) collection = nullptr
	else collection = &(*(TopContainer::getCollection<T>(colId)));
	r.read(&id);
}


template<typename T>
CollectionPointer<T> TopContainer::getCollection(CollectionId id) {
	static map<CollectionId, T*> storage;
	if (storage[id] == null) {
		storage[id] = new T(id);
	}
	return CollectionPointer<T>(storage[id]);
}
