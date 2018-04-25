#pragma once

#include "ThreadMessageBase.h"

class IThreadQuery {
public:
	/** To be called in the thread that is queried.
	*	Should not change anything outside itself! */
	virtual void onReciever() = 0;
	/** To be called on the thread that is sending, after query is returned */
	virtual void onSender() = 0;
};

class QuerySendMessage : public ThreadMessage {
	IThreadQuery* query;
	MessageSender& sender;
public:
	/** Makes a new query message with the given *newed* query.
	*	This message gets ownership of the pointer, and it will be deleted after onSender is called */
	QuerySendMessage(IThreadQuery* q, MessageSender& fromCon) : query(q), sender(fromCon) { ; }
	virtual bool activate() override;
};

class QueryReplyMessage : public ThreadMessage {
	IThreadQuery* query;
public:
	QueryReplyMessage(IThreadQuery* q) : query(q) { ; }
	bool activate() override;
};