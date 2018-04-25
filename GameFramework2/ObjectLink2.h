#pragma once

#include <vector>
#include <list>
#include <algorithm>
#include "Serialization.h"
#include "ObjectCollection.h"
#include "NonAssignable.h"

/** FROM and TO should be a object containers (Relies on object pointers).
*	DATA has to implement serialize and deserialise.*/
template <typename FROM, typename TO, typename DATA>
class ForwardLinkHolder;

template <typename FROM, typename TO, typename DATA>
class BackLinkHolder;

template <typename FROM, typename TO, typename DATA = void>
struct ForwardLink {
	const BackLinkHolder<FROM, TO, DATA>* base;
	const ObjectId id;
	DATA data;
	ForwardLink(BackLinkHolder<FROM, TO>* b, ObjectId i) : base(b), id(i) { ; }
	serialize(SaveWriter& writer);
	/** Links are so managed that they can't have autonomous deserialization*/
	dataDeserialize(LoadReader& reader);
};

template <typename FROM, typename TO, void>
struct ForwardLink {
	const BackLinkHolder<FROM, TO, DATA>* base;
	const ObjectId id;
	ForwardLink(BackLinkHolder<FROM, TO>* b, ObjectId i) : base(b), id(i) { ; }
	serialize(SaveWriter& writer);
	/** Links are so managed that they can't have autonomous deserialization*/
	dataDeserialize(LoadReader& reader);
};

template <typename FROM, typename TO, typename DATA = void>
struct BackLink {
	const ForwardLinkHolder<FROM, TO, DATA>* base;
	const ObjectId id;
	BackLink(ForwardLinkHolder<FROM, TO, DATA>* b, ObjectId i) : base(b), id(i) { ; }
};

template <typename FROM, typename TO, typename DATA = void>
class ForwardLinkHolder : NonAssignable {
	std::vector< std::list< ForwardLink<FROM, TO, DATA > > > links;
	FROM* const owner;
	void (*removeCallback)(ObjectPointer<FROM>, ObjectPointer<TO>) ;
	ForwardLink<FROM, TO, DATA> (*attachFunction)(ObjectPointer<TO>, BackLink<FROM, TO, DATA>) ;

	friend FROM;
	/** Makes a new link holder. Requires a couple of functions
	*	@param o Pointer to the owning object (generally this pointer called in constructor for object type)
	*	@param rc Pointer to function on the from object to call when target asks to be released (can be NULL)
	*	@param af Function to call on the target object type when asked to attach to an object (non null!) */
	ForwardLinkHolder(FROM* o, void(*rc)(ObjectPointer<FROM>, ObjectPointer<TO>), ForwardLink<FROM, TO, DATA>(*af)(ObjectPointer<TO>, BackLink<FROM, TO, DATA>)) :
		owner(o), removeCallback(rc), attachFunction(af) { ; }

	/** Creates a new link. Only callable from the FROM class. 
	*	Returns a reference to the new forward link (for datafilling). base is null on failure. */
	ForwardLink<FROM, TO, DATA>& attach(ObjectId id, ObjectPointer<TO> to);
	
	/** Detatches the given link from the object with the given ID */
	bool detach(ObjectId fromID, ForwardLink<FROM, TO, DATA> link);

	/** Detatches all links from the given object*/
	void destroy(ObjectId removed);

	/** Returns iterators to the start and end of the links list. 
	*	Used for data manipulation, and link extraction. */
	std::pair<std::list< ForwardLink<FROM, TO, DATA> >::iterator&, std::list< ForwardLink<FROM, TO, DATA> >::iterator&>  getLinks(ObjectId id) {
		return std::pair<std::list< ForwardLink<FROM, TO, DATA> >::iterator, std::list< ForwardLink<FROM, TO, DATA> >::iterator&>(links[id].begin(), links[id].end());
	}

public:
	ObjectPointer<FROM> toObjectPointer(BackLink<FROM, TO, DATA> link) {
		return ObjectPointer<FROM>(owner, link.id);
	}

	/** Called by back holder when it want it's link to be removed. Returns false if remove failed 
	*	(only return false on severe error, as the integrity of the dataset is then broken) */
	bool backRemove(ForwardLink<FROM, TO, DATA>& link, ObjectPointer<TO> target, ObjectId fromID);

	/** Serialises the entire link array. Partial serialisation not supported */
	void serialize(SaveWriter& writer);

	/** Deserialises the entire link array. Partial serialisation not supported. Overwrites entire link system */
	void deserialize(LoadReader& reader);

	/** Get pointers to all objects linked to the given object.
	*	Expensive convenience function. 
	*	Serious manipulation should by done by getLinks */
	std::vector< ObjectPointer<TO> > getLinkedObjects(ObjectId id);

};

template <typename FROM, typename TO, typename DATA = void>
class BackLinkHolder : NonAssignable {
	std::vector < std::list < BackLink<FROM, TO, DATA> > > links;
	TO* const owner;
	void(*removeCallback)(ObjectPointer<TO>, ObjectPointer<FROM>);

	friend TO; // Not allowed before c++11;
	BackLinkHolder(TO* o, void(*rc)(ObjectPointer<TO>, ObjectPointer<FROM>)) :
		owner(o), removeCallback(rc) { ; }
	
	/** Called by the owner when an object is destroyed. Closes all links */
	void destroy(ObjectId id);

	/** Registers a new link. Called by the owning object trough the attachement function */
	ForwardLink<FROM, TO, DATA> makeLink(ObjectId id, BackLink<FROM, TO, DATA>);

public:

	ObjectPointer<TO> toObjectPointer(ForwardLink<FROM, TO, DATA>& link) {
		return ObjectPointer<TO>(owner, link.id);
	}

	/** Called by the other end when it want to detatch */
	bool forwardRemove(BackLink<FROM, TO, DATA>& link, ObjectPointer<FROM> sender, ObjectId toID);

//	vector< ObjectPointer<FROM> > getLinks(ObjectId id);
	
};


/* Example class demonstrating a minimal use of the linking system

class SelfReferer {
	const CollectionId myId;

	ForwardLinkHolder<SelfReferer, SelfReferer> targets;
	BackLinkHolder<SelfReferer, SelfReferer> hunters;

	static ForwardLink<SelfReferer, SelfReferer> huntMe(ObjectPointer<SelfReferer> hunted, BackLink<SelfReferer, SelfReferer> hunter) {
		return hunted.collection->hunters.makeLink(hunted.id, hunter);
	}

	public:
	SelfReferer(CollectionId id) : myId(id), targets(this, NULL, SelfReferer::huntMe), hunters(this, NULL) { ; }

	CollectionId getId() { return myId; }
	void serialize(SaveWriter& s) {
		s.write(targets);
	}
	void deserialize(LoadReader& s) {
	s.read(targets);
	}

	static void startHunt(ObjectPointer<SelfReferer> me, ObjectPointer<SelfReferer> target)  {
		me.collection->targets.attach(me, target);
	}

	static void kill(ObjectPointer<SelfReferer> me) {
		me.collection->hunters.destroy(me.id);
		me.collection->targets.destroy(me.id);
	}

};

*/


/// START OF FUNCTION DEFENITIONS

template<typename FROM, typename TO, typename DATA>
inline ForwardLink<FROM, TO, DATA>& ForwardLinkHolder<FROM, TO, DATA>::attach(ObjectId from, ObjectPointer<TO> to)
{
	ForwardLink<FROM, TO, DATA> now = attachFunction(to, BackLink<FROM, TO, DATA>(this, from));
	if (!now.base) return ForwardLink<FROM, TO, DATA>(0, 0); //Unable to connect
	if (links.size() <= from) links.resize(from + 1);
	links[from].push_front(now);
	return links[from].front;
}

template<typename FROM, typename TO, typename DATA>
inline bool ForwardLinkHolder<FROM, TO, DATA>::detach(ObjectId fromID, ForwardLink<FROM, TO, DATA> link)
{
	if (links.size() <= fromID) {
		//TODO: LOG ERROR - empty link holder attempted detatched.
		return true;
	}
	std::list< ForwardLink<FROM, TO, DATA> >::iterator found = std::find(links[fromId].begin(), links[fromId].end(), link);
	if (found == links[fromId].end()) {
		//TODO: LOG ERROR - invalid input, the link is not found.
		return false;
	}
	links[fromId].erase(found);
	return link.base->forwardRemove(BackLink<FROM, TO, DATA>(this, fromId), ObjectPointer<FROM>(owner, fromId), link.id);
}

template<typename FROM, typename TO, typename DATA>
inline void ForwardLinkHolder<FROM, TO, DATA>::destroy(ObjectId removed)
{
	if (links.size() <= removed) return;
	std::list< ForwardLink<FROM, TO, DATA> >::iterator it = links[removed].begin();
	while (it != links[fromId].end()) {
		now = it;
		it++;
		link.base->forwardRemove(BackLink<FROM, TO, DATA>(this, removed), ObjectPointer<FROM>(owner, removed), now->id);
		links[fromId].erase(now);
	}
}

template<typename FROM, typename TO, typename DATA>
inline bool ForwardLinkHolder<FROM, TO, DATA>::backRemove(ForwardLink<FROM, TO, DATA>& link, ObjectPointer<TO> target, ObjectId fromID)
{
	if (links.size() <= fromID) {
		// Major error TODO: log
		return false;
	}
	std::list< ForwardLink<FROM, TO> >::iterator found = std::find(links[fromId].begin(), links[fromId].end(), link);
	if (found == links[fromId].end()) {
		return false; //Major error TODO: log
	}
	links[fromId].erase(found);
	if(removeCallback) removeCallback(ObjectPointer<FROM>(owner, fromID), target);
	
	return true;
}

template<typename FROM, typename TO, typename DATA>
inline void ForwardLinkHolder<FROM, TO, DATA>::serialize(SaveWriter& writer)
{
	writer.write((int)links.size());
	for (int i = 0; i < links.size(); i++) {
		writer.write((int)links[i].size());
		std::list< ForwardLink<FROM, TO, DATA> >::iterator it = links[i].begin();
		while (it != links[i].end) {
			it->serialize(writer);
		}
	}
}

template<typename FROM, typename TO, typename DATA>
inline void ForwardLinkHolder<FROM, TO, DATA>::deserialize(LoadReader& reader)
{
	if (!links.empty()) {
		//TODO: log warning; deserialising a non new link holder shouldn't happen
		links.clear();
	}
	int size;
	reader.read(&size);
	links.resize(size);
	for (int i = 0; i < size; i++) {
		int ls;
		reader.read(&ls);
		for (int j = 0; j < ls; j++) {
			ObjectPointer<TO> target;
			reader.read(&target);
			ForwardLink<FROM, TO, DATA> now = attachFunction(target, BackLink<FROM, TO, DATA>(this, i));
			links[i].push_back(now);
			links[i].back().dataDeserialize(reader);

		}
	}
}

template<typename FROM, typename TO, typename DATA>
inline std::vector<ObjectPointer<TO>> ForwardLinkHolder<FROM, TO, DATA>::getLinkedObjects(ObjectId id)
{
	if (links.size() <= id) return std::vector< ObjectPointer<TO> >();
	std::vector< ObjectPointer<TO> > res;
	res.reserve(links[id].size());
	std::list< ForwardLink<FROM, TO, DATA> >::iterator it = links[id].begin();
	while (it != links[id].end()) {
		res.push_back(it->base->toObjectPointer((*it));
		it++;
	}
	return res;
}

template<typename FROM, typename TO, typename DATA>
inline void BackLinkHolder<FROM, TO, DATA>::destroy(ObjectId id)
{
	if (links.size() <= id) return;
	std::list< BackLink<FROM, TO > >::iterator it = links[id].begin();
	while (it != links[id].end) {
		it->base->backRemove(ForwardLink<FROM, TO, DATA>(this, id), ObjectPointer<TO>(owner, id), it->id);
	}
	links[id].clear();
}

template<typename FROM, typename TO, typename DATA>
inline ForwardLink<FROM, TO, DATA> BackLinkHolder<FROM, TO, DATA>::makeLink(ObjectId id, BackLink<FROM, TO, DATA> link)
{
	if (id > links.size()) {
		links.resize(id);
	}
	links[id].push_front(link);
	return ForwardLink<FROM, TO, DATA>(this, id);
}

template<typename FROM, typename TO, typename DATA>
inline bool BackLinkHolder<FROM, TO, DATA>::forwardRemove(BackLink<FROM, TO, DATA>& link, ObjectPointer<FROM> sender, ObjectId toID)
{
	if (links.size() <= toID) {
		return false; //Major error TODO: log
	}
	std::list< ForwardLink<FROM, TO> >::iterator found = std::find(links[toID].begin(), links[toID].end(), link);
	if (found == links[toID].end()) {
		return false; //Major error TODO: log
	}
	links[toID].erase(found);
	if (removeCallback) removeCallback(ObjectPointer<TO>(owner, fromID), sender);
	return true;
}

template<typename FROM, typename TO, typename DATA>
inline ForwardLink<FROM, TO, DATA>::serialize(SaveWriter& writer)
{
	writer.write(base->toObjectPointer(*this));
	writer.write(data);
}

template<typename FROM, typename TO, typename DATA>
inline ForwardLink<FROM, TO, DATA>::dataDeserialize(LoadReader& reader)
{
	reader.read(&data);
}

template<typename FROM, typename TO, void>
inline ForwardLink<FROM, TO, DATA>::serialize(SaveWriter& writer)
{
	writer.write(base->toObjectPointer(*this));
}

template<typename FROM, typename TO, void>
inline ForwardLink<FROM, TO, DATA>::dataDeserialize(LoadReader& reader)
{
	;
}


