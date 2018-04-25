#pragma once
#include "ThreadMessageBase.h"
#include "ThreadSystem.h"
#include "ThreadBase.h"

class InternalMessageSender : public MessageSender {
	IThreadSystem* system;
	ThreadID threadID;
public:
	InternalMessageSender(IThreadSystem* s, ThreadID thread) : system(s), threadID(thread) { ; }
	void setSystem(IThreadSystem* s) { system = s; }
	void setThreadID(ThreadID id) { threadID = id; }
	void send(ThreadMessage* msg) override {
		system->sendMessage(threadID, msg);
	}
};
