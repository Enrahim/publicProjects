#pragma once

typedef short CollectionId ;
typedef int ObjectId ;


template<typename T>
class ObjectPointer {
	T* collection;
	ObjectId id;
	friend T;
public:

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};

template<typename T>
class CollectionPointer {
	T* collection;
public:
	T& operator *();
	ObjectPointer<T> opperator[](objectId id);
	CollectionPointer(T* c) : collection(c) { ; }

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};


/** Clever trick from http://stackoverflow.com/questions/6077143/disable-copy-constructor  */
class NonAssignable {
	NonAssignable(NonAssignable const& noCopy);
	NonAssignable operator=(NonAssignable const& noCopy);
public:
	NonAssignable() { ; }
};

template <typename ID>
class AllocationHelper : NonAssignable {
public:
	ID getNew();
	void remove(ID);
	class Iterator {
	public:
		ID now();
		void step();
		bool done();
	};
	Iterator getIterator();

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};





template <typename T>
class ComponentStore : public NonAssignable {
	vector<T> data;
	CollectionId parent;
	const AllocationHelper<ObjectId>& allocation;
public:

	class Iterator {
		AllocationHelper<ObjectId>::Iterator it;
		const vector<T>& data;
	public:
		T& now();
		void step();
		bool done();
		Iterator(ComponentStore& owner);
	};
	
	class Pointer {
		ComponentStore* container;
		ObjectId id;
	public:
		T& operator*() { return container->data(id); }
		Pointer(ComponentStore* owner, ObjectId i) : container(owner), id(i) { ; }
		void serialize(SaveWriter s);
		void deserialize(LoadReader s);
	};

	class ComponentStoreRegistery : public NonAssignable {
		vector<ComponentStore< T >* > componentStores;
		vector< set < ComponentPointer <T>* > > unAssigned;
	public:
		void registerStore(ComponentStoreId id, ComponentStore<T>* store); //remember to check unAssigned
		void registerPointer(ComponentStoreId id, ComponentStore<T>::Pointer* pointer);
	};

	static ComponentStore<T>::ComponentStoreRegistery getRegistration();

	Iterator getIterator() { return Iterator(*this); }
	ComponentStore(AllocationHelper& a, CollectionId p) : allocation(a), parent(p) { ; }
	T& operator[](ObjectId);

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};

template<typename T>
class ListComponentStore : public NonAssignable {
	typedef int ListPosId;
	struct Node {
		T data;
		ListPosId next;
	};

	vector <Node> lists;
	vector <ListPosId> starts;

	AllocationHelper<ListPosId> listAllocation;
	const AllocationHelper<ObjectId>& objectAllocation;
	CollectionId parent;

public:
	class ListIterator {
		ListPosId prev;
		ListPosId now;
		vector<Node>& objects;
		AllocationHelper<ListPosId>& parentAlloc;
	public:
		T& now() { return objects[now-1].data; }
		bool done() { return now == 0; }
		void step() {	prev = now;	now = objects[now - 1].next;		}
		void remove() {	if (prev != 0) {parentAlloc.remove(now-1);	now = objects[prev - 1].next = objects[now - 1].next;	}	}
		ListIterator(AllocationHelper<ListPosId>& a, vector<Node> o, ListPosId start);
	};

	ListIterator getList(ObjectId id);

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};

typedef short LinkSignal;
typedef char LocalLinkTypeId;

class ComponentLinkSender {
public:
	virtual void connectionLost(LocalLinkTypeId, ObjectId) = 0;
};

template<typename T>
class ComponentLinkHost {
	const ComponentStore<T>& host;
	vector< set< ComponentBackLink<T> > > references;

	friend ComponentLinkStore<T>;
	void connect(ComponentLinkStore* from, ObjectId fromId, ObjectId toId);
	void disconnect(ComponentLinkStore* from, ObjectId fromId, ObjectId toId);

public:

	void destroy(ObjectId id);
	T& get(ObjectId id);

};

template<typename T>
class ComponentLink {
	ComponentLinkHost<T>* target;
	ObjectId obj;
	friend ComponentLinkStore<T>
public:
	T& operator *();
};

template<typename T>
class ComponentLinkStore {
	vector<ComponentLink< T > > links;
	ComponentLinkSender& host;
	LocalLinkTypeId localId;

	friend ComponentLinkHost <T>;
	void destroyed(ObjectId linkId);
public:

	ComponentLink<T>& connect(ComponentLinkHost* to, ObjectId fromId, ObjectId toId);
	void  disconnect(ObjectId fromId);
	ComponentLink<T>& operator [](int);

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);
};

template<typename T>
class ComponentBackLink {
	ComponentLinkStore<T>* store;
	ObjectId linkId;
	friend ComponentLinkHost<T>;
public:
	bool operator<(const ComponentBackLink& comparer);
};


template<typename TO, typename DATA, typename FROM> 
class ObjectLink{
	ObjectPointer<TO> to;
	ObjectPointer<FROM> from;
	DATA data;
public:
	bool operator <(ObjectLink const & other) const;
};

template<typename TO, typename DATA, typename FROM>
class ObjectLinkHost {
	const T& host;
	vector<set< ObjectLink<TO, DATA, FROM>* > > references;
};

template<typename TO, typename DATA, typename FROM>
class ObjectLinkSimpleStore {
	vector<ObjectLink< TO, DATA, FROM > > links;
	FROM& owner;
public:
	void serialize(SaveWriter s);
	void deserialize(LoadReader s);
	
};

template<typename TO, typename DATA, typename FROM>
class ObjectLinkSetStore {
	vector<set <ObjectLink< TO, DATA, FROM > > > links;
	FROM& owner;
public:
	void serialize(SaveWriter s);
	void deserialize(LoadReader s);
};

//template<typename TO, typename FROM>
//class ObjectLink {
//	ObjectLinkHost<T>* target;
//	ObjectId obj;
//};

//template<typename TO, typename FROM>
//class ObjectBackLink {
//	ObjectLinkStore<T>* store;
//	ObjectId linkId;
//public:
//	bool operator<(const ObjectBackLink& comparer);
//};


/* 
class MinimalCollection: public NonAssignable {
	const CollectionId myId;
	
public:
	MinimalCollection(CollectionId id) : myId(id) { ; }
	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};
*/

class TopContainer : public NonAssignable {
	template <typename T> 
	static CollectionPointer<T> getCollection(CollectionId id);
	//template <typename T> CollectionPointer<T> makeCollection(CollectionId id);
};



template<typename T>
T& CollectionPointer<T>::operator * () {
	return *collection;
}

template<typename T>
CollectionPointer<T> TopContainer::getCollection(CollectionId id) {
	static map<CollectionId, T*> storage;
	if (storage[id] == null) {
		storage[id] = new T(id);
	}
	return CollectionPointer<T>(storage[id]);
}


