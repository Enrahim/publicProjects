#pragma once

#include <string>
#pragma once
#include <vector>
#include "ThreadMessageBase.h"

typedef int ThreadID;

class Thread;

using namespace std;

/**	System for registering and comunication between threads. Platform spesific implementation required. */
class IThreadSystem {
public:
	/** Registers the main thread with a name 
	*	@return the assosiated threadID of this main thread */
	virtual ThreadID mainThread(string name) = 0;

	/** Starts a new thread with the given name, and 
	*	@return the new thread id on succes, -1 on failure */
	virtual ThreadID startThread(string threadName, Thread* tread) = 0;

	/** Get the internal indentification of the thread with the given threadname.
	*	Used in most calls to this library */
	virtual ThreadID getID(string threadName) const = 0;

	/** Gets the next message. If none, wait for it to come.
	*	@param id the internal indentification number of the calling thread - might be ignored.
	*	@return Pointer to the next message. NULL on error. Recipent is responsible for deleting message. */
	virtual ThreadMessage* getMessage(ThreadID id) = 0;

	/** Gets the next message. If none, return imidiately.
	*	@param id the internal indentification number of the calling thread - might be ignored.
	*	@return Pointer to the next message. NULL on error, or no message. Recipent is responsible for deleting message. */
	virtual ThreadMessage* peekMessage(ThreadID id) = 0;

	/** Tests if there is any messages waiting.
	*	@param id  the internal indentification number of the calling thread - might be ignored.
	*	@return true if there is messages, false if not. */
	virtual bool isMessageWaiting(ThreadID id) = 0;

	/** Sends a message to a recipent
	*	@param recipentID the ID gotten by getID the recipent threadName
	*	@param msg A pointer to a message allocated with new. The recipent is responsible for deleting it, and its's contents.
	*	@return true if succefull, false if not */
	virtual bool sendMessage(ThreadID recipentID, ThreadMessage* msg) = 0;

	/** Broadcast a message to all threads
	*	@param msg The message to be sendt
	*	@return true if succefull, false if not */
	virtual bool broadcast(ThreadMessage* msg) = 0;

};


class Thread {
	/** pointer to the operating system spesific commands for threading */
	IThreadSystem* m_system;
	/** identifier for the thread in the given system */
	ThreadID m_threadID;
	/** tests if the thread is already pumping */
	bool isPumping;

	MessageSender* mySender;

protected:
	/** Type message activation. */
	//	bool activateMessage(ThreadMessage* m) { m->activate(*m_system); }
	/** called when thread is exiting */
	virtual void cleanup() = 0;

	/** function that is called just after initialization */
	virtual void onInitialized() { ; }

	/** Gets a sender reference. Warning: should not be called unitialized */
	MessageSender& getSender() { return *mySender; }

public:
	Thread() : isPumping(false), m_system(nullptr), mySender(nullptr) { ; }

	void initialize(IThreadSystem* system, int id);

	IThreadSystem* getSystem() { return m_system; }

	ThreadID getID() { return m_threadID; }

	/** Sends a message to this thread. This thread get ownership of the message.
	*	Can be called by other threads, and should be thread safe*/
	void sendToMe(ThreadMessage* message);

	/** Starts the message pump for this thread.
	*	Should only be called once by the thread system!
	*	Called after initialization */
	void pump();

	virtual ~Thread();
};