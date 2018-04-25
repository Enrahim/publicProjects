#pragma once

#include "ThreadMessageBase.h";

/** Definitions should be moved out to their own header for most thread types */


class SoundThread : public Thread {
	virtual bool activateMessage(ThreadMessage* m) { return m->activate(*this); };
	virtual void cleanup();
};


class ResourceThread : public Thread {
	virtual bool activateMessage(ThreadMessage* m) { return m->activate(*this); };
	virtual void cleanup();
};
