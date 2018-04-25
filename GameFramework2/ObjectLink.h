#pragma once

#include "ObjectPointers.h"

//TODO: This needs fixing; propper backpointer for instane neccesarry

template<typename TO, typename DATA, typename FROM>
class ObjectLink {
	ObjectPointer<TO> to;
	ObjectPointer<FROM> from;
	DATA data;
public:
	bool operator <(ObjectLink const & other) const;
};

template<typename TO, typename DATA, typename FROM>
class ObjectLinkHost {
	const T& host;
	vector<set< ObjectLink<TO, DATA, FROM>* > > references; //TODO: Pointer to objectlink not stable.
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