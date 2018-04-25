#pragma once
#include <map>
#include <vector>

/** Class for handeling an indexed queue where elements might arrive into the queue "randomly"
*	Relevant for instance for a queue of messages recieved over unreliable netwokr connection. */
template<typename T>
class QueueBuffer{
	struct Element {
		Element* next;
		T* data;
	};

	std::map <unsigned int, Element*> freeList;

	Element* head;
	Element* tail;
	unsigned int headPos;
	unsigned int tailPos;

	void commonInit();

public:
	QueueBuffer();
	QueueBuffer(unsigned int startPos);
	/** Puts element into the queue. Owner keeps ownership */
	bool insert(unsigned int pos, T* data);

	/** Return if the next element is available. The queue might still not be empty*/
	bool hasNext() { return head->data != nullptr; }

	/** Return next element if queue is not empty.
	*	Still increments pos if not empty, even if not having next (returning null) */
	T* getNext();

	/** Get a list of positions that is missing in the queue */
	std::vector<unsigned int> getMissing();

	/** if empty the queue can be reset to a given pos. 
	 *	Return false if unsuccessfull due to non-empty queue */
	bool reset(unsigned int pos);

	/** Deletes all entries stored. 
	 *	If this queue is used as only store, this
	 *	should be called upon desruction of the queue */
	void deleteAll();

	virtual ~QueueBuffer();
};

template<typename T>
inline void QueueBuffer<T>::commonInit()
{
	Element* startElement = new Element();
	head = startElement;
	tail = startElement;
}

template<typename T>
inline QueueBuffer<T>::QueueBuffer()
{
	commonInit();
}

template<typename T>
inline QueueBuffer<T>::QueueBuffer(unsigned int startPos) : headPos(startPos), tailPos(startPos)
{
	commonInit();
}

template<typename T>
inline bool QueueBuffer<T>::insert(unsigned int pos, T * data)
{
	if (pos < headPos) return false;
	if (pos < tailPos) {
		std::map<unsigned int, Element*>::iterator it = freeList.find(pos);
		if (it == freeList.end()) return false;
		it->second->data = data;
		freeList.erase(it);
		return true;
	} 
	while( pos != tailPos ) {
		tail->next = new Element();
		freeList[pos] = tail;
		tail = tail->next;
		tailPos++;
	}
	tail->data = data;
	tail->next = new Element();
	tail = tail->next;
	tailPos++;
	return true;
}

template<typename T>
inline T * QueueBuffer<T>::getNext()
{
	if (head == tail) return nullptr;
	T * data = head->data;
	Element* next = head->next;
	delete head;
	head = next;
	headPos++;
	return data;
}

template<typename T>
inline std::vector<unsigned int> QueueBuffer<T>::getMissing()
{
	std::vector<unsigned int> res;
	res.reserve(freeList.size());
	auto it = freeList.begin();
	while (it != freeList.end()) {
		res.push_back(it->first);
		it++;
	}
	return std::vector<unsigned int>();
}

template<typename T>
inline bool QueueBuffer<T>::reset(unsigned int pos)
{
	if(head!=tail) return false;
	headPos = pos;
	tailPos = pos;
}

template<typename T>
inline void QueueBuffer<T>::deleteAll()
{
	Element* now = head;
	while (now != tail) {
		Element* next = now->next;
		delete now->data;
		delete now;
		now = next;
	}
	headPos = tailPos;
}

template<typename T>
inline QueueBuffer<T>::~QueueBuffer()
{
	Element* now = head;
	while (now) {
		Element* next = now->next;
		delete now;
		now = next;
	}
}
