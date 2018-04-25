#pragma once
/** Header file for making structs from field descriptions, that automatically support IO.
*	Requirement is that all basic fields must support IO 
*	Can be used as core storage in variadic templated classes. */

#include "Serialization.h"

typedef char FieldTypeID;

namespace detail {
	template< typename Field, typename ... FieldDescs>
	class FieldedStructBase {
		Field field;
		FieldedStructBase< fieldDescs... > tail;
	public:
		template<typename WrongField> 
		WrongField& get() { return tail.get<WrongField>(); }
		template<Field>
		Field& get() { return field; }

		SIMPLE_IO_CALLS;
		template <typename T>
		void io(T io) {
			io.io(field);
			io.io(tail);
		}
	};

	template< typename Field >
	class FieldedStructBase {
		Field field;
	public:
		template<Field>
		Field& get() { return field; }

		SIMPLE_IO_CALLS;
		template <typename T>
		void io(T io) {
			io.io(field);
		}
	};
}

/** Describes a field using typename and an optinal identifier
*	The identifier is used if there is several fields of the same type
*	that need to be destinguished. Must be a static const identifier. 
*	if it is guaranteed that there is never gonna be more than one field of a certain type,
*	this particular encapsulation may be skipped. */
template <typename T, FieldTypeID id = 0>
struct FieldData {
	T data;

	SIMPLE_IO_CALLS;
	template <typename T>
	void io(T io) {
		io.io(data);
	}
};

/** Container storing fields of different types, that can be retrieved based on type.
*	If there is more than one field of a certain type, FieldData is recomended used.
*	Example use:
*	FieldedStruct<int, FieldData<int, ADDER_TERM> > myData;
*	int res = myData.get<int>() + myData.get<FieldData<int, ADDER_TERM> >().data; 
*/
template<typename ... fieldDescs> 
class FieldedStruct{
	detail::FieldedStructBase< fieldDescs... > fields;
public:
	template<typename T>
	T& get() { return fields.get<T>(); }
	
	template<typename T, FieldTypeID id> 
	T& get() { return fields.get< FieldData<T, id> >().data; }
	
	SIMPLE_IO_CALLS;
	template <typename T>
	void io(T io) {
		io.io(fields);
	}
};

/** Extended example:
	namespace StatDefs {
		const FieldTypeID STRENGTH = 1;
		const FieldTypeID INTELIGENCE = 2;
		const FieldTypeID WISDOM = 3;
		const FieldTypeID DEXTERITY = 4;
		const FieldTypeID CONSTITUTION = 5;
		const FieldTypeID CHARISMA = 6;
	}

	using namespace StatDefs

	typedef FieldedStruct <
		FieldData<char, STRENGTH>,
		FieldData<char, INTELIGENCE>,
		FieldData<char, WISDOM>,
		FieldData<char, DEXTERITY>,
		FieldData<char, CONSTITUTION>,
		FieldData<char, CHARISMA>
	> BaseStats;

	void main() {
		BaseStats myStats;
		BaseStats yourSats;
		myStats.get<char, INTELIGENCE>() = 18;
		YourStats.get<char, INTELIGENCE>() = 10;

		SaveWriter s ("mySupremacy.dat");
		s.io(myStats);
		s.io(yourStats);
		s.close();
		LoadReader l ("mySupremacy.dat";
		l.io(myStats);
		l.io(yourStats);
		l.close();

		if(myStats.get<char, INTELIGENCE>() < yourStats.get<char, INTELIGENCE>) {
			cout << "I am smarter than you!" << endl;
		}	
	}
*/