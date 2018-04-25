#pragma once
#include <vector>
#include "Serialization.h"
#include "PlayerStore.h"

typedef unsigned int ActionTimeStep;

/** Class that should be implemented by the action provider for a certain game model */
class ActionRegisterer {
public:
	/*	Calls NetworkAction::registerActionType<Type>() once for each
	*	ActionType the system is supposed to be able to send. */
	virtual void operator()() = 0;
};

class NetworkAction {
	PlayerID player;

protected:
	typedef unsigned short ActionID;

	class Factory {
	public:
		/** Return a new network action of the overriding class */
		virtual NetworkAction* getNew() = 0;
	};

	/** Do action spesific deserialization. Is run after core deserialiser */
	virtual void doDeserialize(LoadReader r) = 0;

	/** Do action spesific serialisation. Is run after core serialiser */
	virtual void doSerialize(SaveWriter s) = 0;

	/** Should return the static typeid. 
	*	Needed for virtual table access */
	virtual ActionID getType() = 0;


private:

	static ActionID nextID;
	static std::vector<Factory*> factories;
	
	void setPlayer(PlayerID p) { player = p; }


	/** Deserialisation where reading typeID is skipped. Should not be called outside, rather use fetch new */
	void deserialize(LoadReader& reader);

public: 

	/** Called before sending over network, to confirm valid message */
	virtual bool preValidate() { return true; }
	/** Called before runing the action, to confirm it is valid */
	virtual bool validate() { return true; }
	/** Do the action on the system. Must be implemented by the action */
	virtual void run() = 0;

	/** Serialization where first step should be to write out typeID */
	void serialize(SaveWriter& writer);

	/** Serialisation for situations where player is transmited out of band */
	void serializeWithoutPlayer(SaveWriter& writer);

	/** Reads the stream for a serialised network action and return it */
	static NetworkAction* fetchNew(LoadReader& reader);

	/** As fetchnew, but with player provided */
	static NetworkAction* fetchNewWithoutPlayer(LoadReader& reader, PlayerID player);


	/** Must be called in the begining of the program. Registers all known network Actions 
	*	The definition must be edited by programmer when adding new actions!! */
	static void initialze();

	template <typename T>
	static void registerActionType() {
		T::typeID = nextID++;
		factories.push_back(T::getFactory());
	}

	PlayerID getPlayer() {
		return player;
	}
};


/** Example class showing minimum contents of a new network action. 
*	Can be used to substitute old entries in register table for backwards compatibility */
class NullAction : public NetworkAction {
private:
	friend NetworkAction;
	static ActionID typeID;

	class MyFactory : public Factory {
	public:
		virtual NetworkAction* getNew() { return new NullAction(); }
	};

	static MyFactory* getFactory() { return new MyFactory(); }
	virtual ActionID getType() {
		return typeID;
	}

	virtual void doDeserialize(LoadReader r) {
		;
	}

	virtual void doSerialize(SaveWriter s) {
		;
	}

public:
	virtual void run() { ; }
};

/** A storage of actions stored in a serialized format
*	The storage is immutable except for add operations.
*	Actions can only be accessed in inserted order,
*	and only copies of the actions stored are recieved */
class ActionStore{
	ActionTimeStep m_time;
	int actionCount;
	vector<char> data;
public:
	ActionStore() { ; }
	ActionStore(ActionTimeStep t) : m_time(t) { ; }

	/** Adds action to storage.
	*	Require a newed action, and container gains ownership. */
	void addAction(NetworkAction* action);

	ActionTimeStep getTime() { return m_time; }

	class Accessor {
		int num;
		VectorInStream stream;
		ActionStore& owner;
		friend ActionStore;
		Accessor(ActionStore& owner);
	public:
		bool hasNext();
		/** Gets a newed networkAction object. Caller gains ownership */
		NetworkAction* getNext();
	};

	/** Returns an newed accessor to the network actions 
	*	Caller gains ownership of the pointers. */
	Accessor* getAccessor();

	int size() {
		return sizeof(m_time) + sizeof(actionCount) + data.size();
	}

	void reset(int t) {
		m_time = t;
		actionCount = 0;
		data.clear();
	}

	SIMPLE_IO_CALLS;
	template<typename T>
	void io(T ioSys) {
		ioSys.io(m_time);
		ioSys.io(actionCount);
		ioSys.io(CollectionWrapper<vector<char> >(data));
	}

};