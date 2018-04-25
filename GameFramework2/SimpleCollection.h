#pragma once

#include "ObjectCollection.h"
#include "ComponentStore.h"

template <typename T>
class SimpleCollection {
	typedef ObjectPointer< SimpleCollection<T> > myPointer;
	const CollectionId myId;

	AllocationHelper allocator;
	ComponentStore<T> internalStore;

public:
	SimpleCollection<T>(CollectionId id) : myId(id), internalStore(allocator) { ; }
	CollectionId getId() { return myId; }
	SIMPLE_IO_CALLS;
	template <typename S>
	void io(S io) {
		io.io(allocator);
		io.io(internalStore);
	}

	ComponentStore<T>::Iterator getIterator() {
		return internalStore.getIterator();
	}
	
	myPointer store(T data) {
		myPointer res;
		res.collection = this;
		res.id = allocator.getNew();
		internalStore[res.id] = data;
	}

	// The collection should have static functions on this form to access objects
	static T& get(myPointer& object) {
		return object.collection.store[object.id];
	}

	static void remove(myPointer& object) {
		object.collection.allocator.free(object.id);
	}

};

/** To get a new container use this:
ContainerPointer<SimpleCollection <MyClass> > container = TopContainer::getCollection< SimpleCollection <MyClass> >(requestedID);
*/