#include "ThreadQuery.h"
#include "ThreadSystem.h"

bool QuerySendMessage::activate()
{
	if (query) {
		query->onReciever();
		sender.send(new QueryReplyMessage(query));
	}
	return true;
}

bool QueryReplyMessage::activate()
{
	if (query) {
		query->onSender();
		delete query;
	}
	return true;
}
