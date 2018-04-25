#pragma once

#include "ActionProtocol.h"
#include "ThreadMessageBase.h"

class ActionSystem {
public:
	/**	Calls NetworkAction::registerActionType<Type>() once for each 
	*	ActionType the system is supposed to be able to send. */
	void registerActions() = 0;

	/** Called by UI to request doing the given action */
	void sendToNetwork(NetworkAction* action);
};


class ActionMessageToNetwork : public ThreadMessage {
	NetworkAction* action;
	ActionMessageToNetwork(NetworkAction* a);
public:

	/** Creates a new Action message, and send it trough the network to network thread
	*	This function takes ownership over the action 
	*	@param com A working comunication system, with */
	static void sendAction(IThreadSystem& com, NetworkAction* action);

	/** Sends the action over network for synchronisation */
	virtual bool activate(IThreadSystem& ioSystem);
};

class ActionStoreToGamestate : public ThreadMessage {
	ActionStore* action;
	ActionStoreToGamestate(ActionStore* a);
public:
	static void sendAction(IThreadSystem& com, ActionStore* action);
	virtual bool activate(IThreadSystem& ioSystem);
};

class ActionMessage : public ThreadMessage {
	NetworkAction* action;
public:
	/** Sends the action over network for synchronisation */
//	virtual bool activate(NetworkThread& network);
	/** Enacts the action on the gameState */
//	virtual bool activate(GameStateThread& gameState);

	/** Makes a new thread message conveying the action pointed.
	*	The message gains the responsibility for freeing the action. */
	ActionMessage(NetworkAction* a) : action(a) { ; }

	/** Called by the io system when they want to do a syncronised action on the gamestate.
	*	All changes on the gamestate by a user should be using this path! */
	static void enact(IThreadSystem* sys, NetworkAction* a);
};


