#pragma once

#include <set>
#include "Serialization.h"

/** Helper class for handeling what ids are free in a given context*/
template <typename ID>
class AllocationHelper : NonAssignable {
	std::set<ID> freeList;
	/** The highest number used + 1; Poor name due to history */
	ID maxUnUsed;
public:
	/** Get a new free id */
	ID getNew();
	/** Free a used Id, so that it is free to reuse */
	void remove(ID);
	class Iterator {
		ID current;
		ID maxUnUsed;
		std::set<ID>::iterator freeIt;
		std::set<ID>::iterator endIt;
	public:
		ID now() { return current; }
		void step();
		bool done() { return current >= maxUsed; }
		Iterator(AllocationHelper<ID>& owner);
	};
	/** Get an iterator that iterates only over used IDs */
	Iterator getIterator() { return Iterator(*this); }

	/** Gets the value of the highest id in use */
	ID getMax() { return maxUnUsed - 1; }

	void serialize(SaveWriter s);
	void deserialize(LoadReader s);

};

template<typename ID>
inline ID AllocationHelper<ID>::getNew()
{
	if (freeList.empty()) {
		return (maxUnUsed++);
	}
	ID tmp = *(freeList.cbegin());
	freeList.erase(freeList.cbegin());
	return tmp;
}

template<typename ID>
inline void AllocationHelper<ID>::remove(ID id)
{
	if (id + 1 == maxUnUsed) {
		maxUsed--;
		while (!freeList.empty() && maxUnUsed == (*freeList.crbegin()) + 1) {
			freeList.erase(--freeList.end());
			maxUnUsed--;
		}
	}
	else {
		freeList.insert(id);
	}
}

template<typename ID>
inline void AllocationHelper<ID>::serialize(SaveWriter s)
{
	s.write(maxUnUsed);
	s.write(freeList.size());
	std::set<ID>::iterator it = freeList.begin();
	while (it != freeList.end()) {
		s.write(*it);
		it++;
	}
}

template<typename ID>
inline void AllocationHelper<ID>::deserialize(LoadReader s)
{
	s.read(&maxUnUsed);
	int size = s.readInt();
	freeList.clear();
	for (int i = 0; i < size; i++) {
		ID tmp;
		s.read(&tmp);
		freeList.insert(freeList.end(), tmp);
	}
}

template<typename ID>
inline void AllocationHelper<ID>::Iterator::step()
{
	current++;
	while (freeIt != endIt && now == *freeIt) {
		current++;
		freeIt++;
	}
}

template<typename ID>
inline AllocationHelper<ID>::Iterator::Iterator(AllocationHelper<ID> & owner) : maxUnUsed(owner.maxUnUsed), freeIt(freeList.begin()), endIt(freeList.end()), current(0) { ; }
