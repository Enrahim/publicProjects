#include "ComponentTypes.h"

/** dummy declarations */
ExampleStruct ExampleStruct::dummy;
Persistent3dPoint Persistent3dPoint::dummy;
Persistent2dPoint Persistent2dPoint::dummy;

void Persistent2dPoint::load(LoadReader r) {
	x = r.readInt();
	y = r.readInt();
}

void Persistent2dPoint::save(SaveWriter w) {
	w.write(x);
	w.write(y);
}

void Persistent3dPoint::load(LoadReader r) {
	x = r.readInt();
	y = r.readInt();
	z = r.readInt();
}

void Persistent3dPoint::save(SaveWriter w) {
	w.write(x);
	w.write(y);
	w.write(z);
}