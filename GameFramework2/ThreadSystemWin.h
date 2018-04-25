#pragma once

#include "ThreadSystem.h"
#include <map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/** hook to rescue messages from the modal loop in the get messages */
#define MSGF_CENTRALTHREAD 0
#define WM_CENTRALTHREAD WM_APP+1

/** Windows spesific implementation of the threading functionality */

class ThreadSystemWin : public IThreadSystem {
	/** Entrypoint for WINAPI */
	static DWORD WINAPI startThread(LPVOID lpParameter);
	map<string, ThreadID> threadNames;
	vector<DWORD> threadIDs;
	vector<HWND> windowIDs; //only nonzero if registered window
	HANDLE* events; //would have been best volatile, but not compatible with windows

public:
	virtual ThreadID mainThread(string name);

	/** Starts a new thread with the given name, and returns the new thread id on succes, -1 on failure */
	virtual ThreadID startThread(string threadName, Thread* tread);
	virtual ThreadID getID(string threadName) const;
	
	/** Get and peek message for windows get the current message pump message */
	virtual ThreadMessage* getMessage(ThreadID id) { return getMessage(); }
	ThreadMessage* getMessage();
	virtual ThreadMessage* peekMessage(ThreadID id) { return peekMessage(); };
	ThreadMessage* peekMessage();
	virtual bool isMessageWaiting(ThreadID id) { return isMessageWaiting(); };
	bool isMessageWaiting();

	virtual bool sendMessage(ThreadID recipentID, ThreadMessage* msg);
	virtual bool broadcast(ThreadMessage* msg);


	/** Registeres that the thread has gone to windowed mode
	*	If a thread makes it's first window, it should register it asap.
	*	Messages should after this be recieved trough the windowProc
	*	Can only be called once for each thread
	*	@param threadID the internal id of the calling thread
	*	@param windowID handle to the newly created window
	*	@returns true on success, false on error */
	bool registerWindow(int threadID, HWND windowID);
};