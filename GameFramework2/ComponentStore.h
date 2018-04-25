#pragma once

#include "AllocationHelper.h"

typedef short ComponentStoreId;

/** Class that stores a slice of a object in a quickly accessible vector-format
*	Nice for components that is needed to be copied to shaders, like positions, colors, velocities etc. 
*	Helper class that can be included in any object; and suport persistence
*	// NOT SUPPORTED: Also by handeling static ids for persistent linking to the given type
*	T must implement the serialize and deserialize 
*/
template <typename T>
class ComponentStore : public NonAssignable {
	vector<T> data;
//	ComponentStoreId myId;
	const AllocationHelper<ObjectId>& allocation;
public:

	class Iterator {
		AllocationHelper<ObjectId>::Iterator it;
		const vector<T>& data;
	public:
		T& now() { return data[it.now()]; }
		T& operator*() { return data[it.now()]; }

		void step() { it.step(); }

		bool done() {
			return it.done();
		}

		Iterator(ComponentStore& owner) it(owner.allocation.getIterator() ), data(owner.data) {
			;
		}
	};

	//class Pointer {
	//	ComponentStore* container;
	//	ObjectId id;
	//public:
	//	T& operator*() { return container->data(id); }
	//	Pointer(ComponentStore* owner, ObjectId i) : container(owner), id(i) { ; }
	//	void serialize(SaveWriter s);
	//	void deserialize(LoadReader s);
	//};

	///** Internal psaudo singelton for handeling the ids of the different components */
	//class ComponentStoreRegistery : public NonAssignable {
	//	static vector<ComponentStore< T >* >* getComponentStores();
	//	static vector< set < ComponentPointer <T>* > >* getUnAssigned();
	//public:
	//	static void registerStore(ComponentStoreId id, ComponentStore<T>* store); //remember to check unAssigned
	//	static void registerPointer(ComponentStoreId id, ComponentStore<T>::Pointer* pointer);
	//};

	//static ComponentStore<T>::ComponentStoreRegistery getRegistration();

	Iterator getIterator() { return Iterator(*this); }
	ComponentStore(AllocationHelper<ObjectId>& a) : allocation(a) { ; }
	T& operator[](ObjectId id);

	/** Serializes the vector, but requires the object to have stored how many entries there are */
	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};

template<typename T>
inline T & ComponentStore<T>::operator[](ObjectId id)
{
	if (id > data.size()) data.resize(id + 1);
	return data[id];
}

template<typename T>
inline void ComponentStore<T>::serialize(SaveWriter s)
{
	// number of objects should already be stored at object-level via allocator save
	for (int i = 0; i < allocation.getMax(); i++) {
		it->serialize(s);
	}
}

/** Important: allocator must be deserialised before this! */
template<typename T>
inline void ComponentStore<T>::deserialize(LoadReader s)
{
	unsigned int maxObject = allocation.getMax();
	data.clear();
	data.resize(maxObject);
	for (int i = 0; i < maxObject; i++) {
		data[i].deserialize(s);
	}
}
