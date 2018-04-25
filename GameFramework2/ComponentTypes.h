#pragma once

#include "ObjectManager.h"
#include "Serialization.h"
#include "ObjectPointers.h"

/**
*	Defines all structs that can be managed by the object manager
*	The types must have a static typeID field, and have an entry in the component typefactory
*	They must also implement void load(LoadReader r) and void save(SaveWriter s)
*	Finally a static dummy is required for missed fetches.
*/

struct ExampleStruct {
	static const int typeID = 0;

	/** Simple load. Just call r.read*** for each member
	*	and load for each pointer */
	void load(LoadReader r) { ; }
	/** Simple save. Just call s.write(...) for each member 
	*	and save for each pointer */
	void save(SaveWriter s) { ; }

	static ExampleStruct dummy;
};

struct Persistent3dPoint {
	union {
		int x, y, z;
		int comp[3];
	};
	static const int typeID = 1;

	/** Simple load. Just call r.read*** for each member */
	void load(LoadReader r) ; 
	/** Simple save. Just call s.write(...) for each member */
	void save(SaveWriter s) ; 

	static Persistent3dPoint dummy;
};

struct Persistent2dPoint {
	union {
		int x, y;
		int comp[2];
	};
	static const int typeID = 2;

	/** Simple load. Just call r.read*** for each member */
	void load(LoadReader r);
	/** Simple save. Just call s.write(...) for each member */
	void save(SaveWriter s);

	static Persistent2dPoint dummy;
};

//class ComponentTypeFactory {
//	IComponent& exemplar(int typeID){
//		switch (typeID) {
//		case ExampleStruct::typeID:
//			return ComponentImpl<ExampleStruct>::dummy();
//		case Persistent2dPoint::typeID:
//			return ComponentImpl<Persistent2dPoint>::dummy();
//		case Persistent3dPoint::typeID:
//			return ComponentImpl<Persistent3dPoint>::dummy();
//		}
//	}
//};