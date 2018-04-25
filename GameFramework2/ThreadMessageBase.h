#pragma once

/** Base class for the messaging system. All message types implement this.*/
class ThreadMessage {
public:
	/** Run by all threads before the specialiced do functions 
	*	@return false if the thread should shut down */
	virtual bool activate() { return true; };
	
};


/** Makes the targeted thread end. Can be broadcast at system end. */
class QuitMessage : public ThreadMessage {
public:
	virtual bool activate() { return false; }
};

class MessageSender {
public:
	/** Sends a threadMessage somewhere. Function takes ownership of the poiner. */
	virtual void send(ThreadMessage* message) = 0;
};