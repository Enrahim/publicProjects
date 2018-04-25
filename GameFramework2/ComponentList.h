#pragma once

#include "ObjectManager.h"


/**Helper for managed list. The type T must support the .save and .load commands! */
template <typename T>
class ManagedListBase {
	struct ListItem {
		T data;
		int next;
	};
	vector< listItem > data;
	vector<int> itemBase;
	IndexAllocator idControll;

public:

	/** Access point for the program into the saved list data */
	class ListIterator {
		/** Pointer to previous link for easy change */
		int* prev;
		ListItem* now;
		vector<listItem>& base;
		/** used for freeing.
		*	Important!! All ids provided is one to low, as 0 is a unused id. */
		IndexAllocator& allocator;

		friend ManagedListBase < T >::getList;
		/** Constructor should only be got from a list base's getList */
		ListIterator(int& start, vector<listItem>& b, IndexAllocator& ids) :
			prev(&start), base(b), now(NULL), allocator(ids) {
			if (start != 0) now = b[start];
		}

	public:

		void next() {
			if (now == NULL) {
				LOG_WARNING("Calling next on a list that already is beyond bounds");
				return;
			}
			prev = &(now->next);
			if (now.next == 0) now = NULL;
			else now = base[now->next];
		}
		T* nowPtr(){ if (now) return (&now->data); return NULL; }
		bool done(){ return (now == NULL); }
		
		/** Removes the pointed to item from the list and steps forward */
		void remove() {
			if (now == NULL){
				LOG_ERROR("Attempt to delete null-list item!");
				return;
			}
			allocator.free(*prev);
			*prev = now->next;
			if (now.next == 0) now = NULL;
			now = base[now->next];
		}
	
	};

	ListIterator getList(int id) {
		return ListIterator(itemBase[id], data, idControll)
	}

	/** inserts the info to the list with the given id */
	void insert(int id, T info) {
		int pos = allocator.allocate() + 1;
		if (pos >= data.size()) data.resize(pos + 1);
		data[pos].data = info;
		data[pos].next = itemBase[id];
		itemBase[id] = pos;
	}

	/** load requires a used iterator from the object manager */
	load(LoadReader& r, UsedIterator& it) {
		allocator.clear();
		data.clear();
		itemBase.clear();
		itemBase.resize(it.maxID());
		while (!it.done()) {
			itemBase[*it] = r.getInt();
			it.next();
		}
		allocator.load(r);
		UsedIterator it2 = allocator.getIterator();
		data.resize(it2.maxID() + 1);
		while (!it2.done()){
			data[*it2 + 1].load(r);
			it2.next();
		}
	}

	/** save requires a used iterator from the object manager */
	save(SaveWriter& w, UsedIterator& it) {
		while (!it.done()) {
			w.write(itemBase[*it]);
			it.next();
		}
		UsedIterator it2 = allocator.getIterator();
		while (!it2.done()){
			data[*it2 + 1].save(w);
			it2.next();
		}
	}

	/** Prepares a new list at the given id 
	*	Does not overwrite old data! */
	void addList(int id) {
		if (id >= itemBase.size()) itemBase.resize(id + 1);
	}

	/** Removes a list. 
	*	Should only be called by manager when an object dies */
	void removeList(int id) {
		ListIterator it= getList(id); 
		while(!it.done()) {
			it.remove();
		}
	}
};