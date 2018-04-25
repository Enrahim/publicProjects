#include "ActionProtocol.h"

std::vector<NetworkAction::Factory*> NetworkAction::factories;
NetworkAction::ActionID NetworkAction::nextID = 1;

NetworkAction::ActionID NullAction::typeID = 0;

/** Add new line here for all new action types you make*/
void NetworkAction::initialze()
{
	registerActionType<NullAction>();
}


void NetworkAction::deserialize(LoadReader & reader)
{
	reader.read(&player);
	doDeserialize(reader);
}


void NetworkAction::serialize(SaveWriter & writer)
{
	writer.write(getType());
	writer.write(player);
	doSerialize(writer);
}

void NetworkAction::serializeWithoutPlayer(SaveWriter & writer)
{
	writer.write(getType());
	doSerialize(writer);
}

NetworkAction * NetworkAction::fetchNew(LoadReader& reader)
{
	ActionID id;
	reader.read(&id);
	NetworkAction* res = factories[id]->getNew();
	res->deserialize(reader);
	return res;
}

NetworkAction * NetworkAction::fetchNewWithoutPlayer(LoadReader & reader, PlayerID player)
{
	ActionID id;
	reader.read(&id);
	NetworkAction* res = factories[id]->getNew();
	res->setPlayer(player);
	res->doDeserialize(reader);
	return res;
}

void ActionStore::addAction(NetworkAction * action)
{
	VectorOutStream stream(data);
	SaveWriter writer(stream);
	action->serialize(writer);
	actionCount++;
}

ActionStore::Accessor * ActionStore::getAccessor()
{
	return new Accessor(*this);
}

ActionStore::Accessor::Accessor(ActionStore & o) : stream(o.data), num(0), owner(o)
{
}

bool ActionStore::Accessor::hasNext()
{
	return num<owner.actionCount;
}

NetworkAction * ActionStore::Accessor::getNext()
{
	LoadReader reader(stream);
	return NetworkAction::fetchNew(reader);
}
