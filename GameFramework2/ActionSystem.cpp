#include "ActionSystem.h"
#include "NetworkThread.h"
#include "SafeStatisticsLogger.h"

bool ActionMessage::activate(NetworkThread & network)
{
	if (!action) {
		LOG_ERROR("Attempting to send a null action mesage over network!");
		return true;
	}
	if (!action->preValidate()) {
		LOG_WARNING("attempted action failed pre-validation");
		return true;
	}
	network.sendAction(action);
	delete action;
	action = NULL;
	return true;
}

bool ActionMessage::activate(GameStateThread & gameState)
{
	if (!action) {
		LOG_ERROR("Attempting to activate a null action!");
		return true;
	}
	if (action->validate()) {
		action->run();
	}
	delete action;
	action = NULL;
}

void ActionMessage::enact(IThreadSystem* sys, NetworkAction * a)
{
	if (!a) {
		LOG_ERROR("Attempt at enacting a NULL network action!");
		return;
	}
	sys->sendMessage(sys->getID(NETWORK_THREAD_NAME), new ActionMessage(a));
}
