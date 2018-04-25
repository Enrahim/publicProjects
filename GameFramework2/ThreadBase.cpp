#include "SafeStatisticsLogger.h"
#include "ThreadSystem.h"
#include "InternalMessageSender.h"

void Thread::initialize(IThreadSystem* system, int id) {
	m_system = system;
	m_threadID = id;
	mySender = new InternalMessageSender(system, id);
	onInitialized();
}

void Thread::sendToMe(ThreadMessage * message)
{
	m_system->sendMessage(m_threadID, message);
}

void Thread::pump(){
	if (isPumping) {
		LOG_ERROR("Attempt to start pumping thread with id " << m_threadID 
			<< ", but it is already pumping!");
		return;
	}
	if (!m_system) {
		LOG_ERROR("Attempt to start pumping thread with id " << m_threadID
			<< ", but it is not initialized!");
		return;
	}
	isPumping = true;
	bool goOn=true;
	while (goOn) {
		ThreadMessage* m = m_system->getMessage(m_threadID);
		if (m == NULL) continue;
		goOn = m->activate();
		delete m;
	}
	cleanup();
}

Thread::~Thread()
{
	if (mySender) {
		delete mySender;
	}
}
