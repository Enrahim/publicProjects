#pragma once

#include <string>
#include <set>
#include <vector>
using namespace std;

/** Encapsulates the functions required to save primitives */
class SaveWriter;

/** Encapsulates the functions required to load primitives */
class LoadReader;

class ObjectTypeManager;

/** Base class for a collection of a type of components in an object */
struct BaseComponentCollection {
	const ObjectTypeManager* manager;
	const unsigned char componentID;

protected:
	// Should not be instantsated typelessly
	BaseComponentCollection(ObjectTypeManager* m, unsigned char id = 0) : 
		manager(m), componentID(id) { ; }
};

/** Type spesific class for collection of a certain type of component of an object */
template <typename T>
struct ComponentCollection : public BaseComponentCollection {
	vector< T > data;
	ComponentCollection(ObjectTypeManager* m, unsigned char id = 0) :
		BaseComponentCollection(m, id) { ; }
};

/** Generates a sequence of numbers corresponding to used entries in a list */
class UsedIterator {
	const int maxInt;
	int now;
	set<int>::iterator freeIt;
	set<int>::iterator endIt;
public:
	/** Makes an iterable sequence of numbers of used object numbers
	*	@param free A set of unused numbers
	*	@param max The maximum number used */
	UsedIterator(set<int>& free, int max);

	/** Steps to the next number in the list */
	void next();
	/** Test if all numbers have been gotten */
	bool done();

	/** Returns the current number */
	int getNow() { return now; }

	int maxID() { return maxInt; }
};

/** Class for handing out and freeing intigers used as indexes*/
class IndexAllocator {
	set<int> freeList;
	/** The highest number used */
	int maxUsed;
public:
	/** Allocates a new index */
	int allocate();
	/** Frees a used index */
	void free(int id);
	/** Returns a number sequence for all the used numbers */
	UsedIterator getIterator();
	/** Clears the indexes */
	void clear();
	/** Returns the number of used indexes */
	int numUsed();

	int maxID() { return maxUsed; }

	void load(LoadReader& reader);
	void save(SaveWriter& saver);
};


/** Base class that provides an interface and core functions for handeling objects.
*	These objects are buildt as collections of simple base components.
*	The class handles persistence, lookup and list handling for these objects. */
class ObjectTypeManager {
	/** System for handeling the used and unused indexes */
	IndexAllocator idControll;

	/** The file currently assosiated with this object */
	string fileName;

	/** Starts the cleanup process, making this manager a clean slate */
	void baseCleanup();

	/** stores wich collection this object is stored in 
	*	important for pointers */
	unsigned short collectionID;

protected:
	template <typename T>
	class DataIterator;

	/** Get an iterator for the given values, that only gets active members. */
	template<typename T>
	DataIterator<T> getIterator(vector<T>& values) {
		return DataIterator<T>(idControll.getIterator(), values);
	}

	/** Allows an implementation to iterate over only active members of a vector */
	template <typename T>
	class DataIterator {
		UsedIterator used;
		vector<T>& values;

		DataIterator(UsedIterator it, vector<T>& v) :
			used(it), values(v) { ; }
		friend DataIterator<T> ObjectTypeManager::getIterator<T>(vector<T>&);
	public:
		T& operator *() { return value[used.getNow()]; }

		bool isEnd() { return used.done() }
		void step() { used.next(); }
	};



	template <typename T>
	void loadVector(LoadReader& r, vector<T>& v) {
		v.clear();
		v.resize(idControll.maxID());
		DataIterator<T> it = getIterator(v);
		while (!it.isEnd()) {
			(*it).load(r);
			it.step();
		}
	}
	template <typename T>
	void saveVector(SaveWriter& w, vector<T>& v) {
		DataIterator<T> it = getIterator(v);
		while (!it.isEnd()) {
			(*it).save(w);
			it.step();
		}
	}

	/** Free an object of the given id */
	void freeObject(int id);
	/** Returns a new free ID */
	int allocate();

	ObjectTypeManager(unsigned short id) : collectionID(id) { ; }

	/** Should clear all data, making this a blank sheet ready to be rewritten
	*	Should only be called by baseCleanup */
	virtual void cleanup();

public:
	/** Reads object data from file */
	bool readFromFile();
	/** Saves object data to file */
	bool saveToFile();

	/** Sets the file to be assosiated with this object */
	void setFile(string path);

	/** Returns the number of active objects */
	int numObjects();

	unsigned short getCollectionId() const { return collectionID;  }

	/** Fetches an entire interior of a indicated collection */
	template <typename T>
	ComponentCollection<T>* fetchCollection(unsigned char component = 0) {
		try {
			return dynamic_cast<ComponentCollection<T>*>
				(fetchCollection(T::typeID, component));
		}
		catch (std::bad_cast) {
			LOG_ERROR("Attemt to cast collection type with the following parameters failed:"
				<< " type: " << T::typeID << " component: " << (int)component );
		}
		return NULL;
	}

	/** Fetches the indicated data */
	template <typename T>
	T& fetchData(int id, unsigned char component = 0 ) {
		ComponentCollection < T > * tmp = fetchCollection<T>(component);
		if (tmp != NULL) {
			return tmp->data[id];
		}
		else{
			LOG_ERROR("Error when fetching data id: " << id);
		}
		return T::dummy;
	}

	//	Unimplemented interfaces!

	/** must be overloaded by implementation returning
	*	a unique identifier indicating the type.
	*	Used both for pointers and for storage
	*	Should have a assosiated const static ... typeID */
	virtual unsigned short getObjectType() = 0;

protected:
	/** Fetch a pointer to the collection indicated
	*	@param type. A number that indetifies the type
	*	@param compoinent if there are more than one component of each type this indicates which */
	virtual BaseComponentCollection* fetchCollection(unsigned char type, unsigned char component = 0) = 0;

	/** Loader. Should call loadVector for each component. */
	virtual void load(LoadReader& reader) = 0;
	/** Saver. Should call saveVector for each component */
	virtual void save(SaveWriter& writer) = 0;


// hook

	/** Is called when an object is freed */
	virtual void onFree(int id) { ; }

};