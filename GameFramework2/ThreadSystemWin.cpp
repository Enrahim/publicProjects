#include "ThreadSystemWin.h"
#include "SafeStatisticsLogger.h"
#include "ThreadBase.h"


ThreadID ThreadSystemWin::mainThread(string name) {
	if (threadNames.find(name) != threadNames.end()) {
		LOG_ERROR("a thread has already registered with main thread name: " << name << 
			"! Current action: Reregistering.");
	}
	int id = GetCurrentThreadId();
	threadNames[name] = threadIDs.size();
	threadIDs.push_back(id);
	windowIDs.push_back(0);

	return threadIDs.size() - 1;
}

ThreadID ThreadSystemWin::getID(string name) const {
	map<string, int>::const_iterator it = threadNames.find(name);
	if (it == threadNames.end()) return 0;
	return it->second;
}

ThreadMessage* ThreadSystemWin::getMessage() {
	MSG msg;
	while (true) {
		if (GetMessage(&msg, NULL, 0, 0) == -1) {
			LOG_ERROR("Critical error when getting message: " << GetLastError());
			return NULL;
		}
		if (msg.message != WM_CENTRALTHREAD) {
			if (!CallMsgFilter(&msg, MSGF_CENTRALTHREAD)) {
//				LOG_TRACE("Thread system got stdMessage: " << msg.message << " wParam: " << msg.wParam << " lParam: " << msg.lParam);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			return (ThreadMessage*)msg.lParam;
		}
	}
}

ThreadMessage* ThreadSystemWin::peekMessage() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message != WM_CENTRALTHREAD) {
			if (!CallMsgFilter(&msg, MSGF_CENTRALTHREAD)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			return (ThreadMessage*)msg.lParam;
		}
	}
	return NULL;
}

bool ThreadSystemWin::isMessageWaiting() {
	MSG msg;
	return(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE));
}

bool ThreadSystemWin::sendMessage(ThreadID recipentID, ThreadMessage* msg) {
	if (windowIDs[recipentID] != 0) {
		if (PostMessage(windowIDs[recipentID], WM_CENTRALTHREAD, 0, (LPARAM)msg)) {
			return true;
		}
		else {
			LOG_ERROR("Error when posting message to window: " << windowIDs[recipentID]
				<< " of thread: " << recipentID
				<< "With error code" << GetLastError());
			return false;
		}
	}
	else {
		if (PostThreadMessage(threadIDs[recipentID], WM_CENTRALTHREAD, 0, (LPARAM)msg)) {
			return true;
		}
		else {
			LOG_ERROR("Error when posting message to thread: " << threadIDs[recipentID]
				<< " of thread: " << recipentID
				<< " With error code: " << GetLastError());
			return false;
		}
	}
	return false;
}

bool ThreadSystemWin::broadcast(ThreadMessage* msg) {
	bool success = true;
	for (unsigned int i = 0; i<threadIDs.size(); i++) {
		if (!sendMessage(i, msg)) success = false;
	}
	return success;
}

bool ThreadSystemWin::registerWindow(ThreadID threadID, HWND windowID) {
	if (windowIDs[threadID] != 0) {
		LOG_ERROR("Thread with internal id: " << threadID
			<< " Trying to register window: " << windowID
			<< " When window id " << windowIDs[threadID]
			<< " is already registered on it");
		return false;
	}
	windowIDs[threadID] = windowID;
	return true;
}

struct ThreadParam {
	Thread* t;
	HANDLE e1, e2;
};

int ThreadSystemWin::startThread(string threadName, Thread* thread) {
	if (threadNames.find(threadName) != threadNames.end()) {
		LOG_ERROR("Trying to start a thread with same name as a previous thread: " << threadName);
		return -1;
	}
	DWORD threadID;
	HANDLE e1 = CreateEvent(0, 0, 0, 0);
	if (e1 == NULL) {
		LOG_ERROR("Unable to create event for thread success report");
		return -1;
	}

	HANDLE e2 = CreateEvent(0, 0, 0, 0);
	if (e2 == NULL) {
		LOG_ERROR("Unable to create event for thread ok to start report");
		return -1;
	}

	ThreadParam param;
	param.e1 = e1;
	param.e2 = e2;
	param.t = thread;

	HANDLE hThread = CreateThread(0, 0, startThread, &param, 0, &threadID);
	if (hThread == NULL) {
		LOG_ERROR("Unable to start a thread");
		CloseHandle(e1);
		CloseHandle(e2);
		return -1;
	}
	CloseHandle(hThread);
	thread->initialize(this, threadID);
	SetEvent(e2);
	DWORD res = WaitForSingleObject(e1, INFINITE);

	CloseHandle(e1);

	threadNames[threadName] = threadID;
	threadIDs.push_back(threadID);
	windowIDs.push_back(0);
	if (res != WAIT_OBJECT_0) {
		LOG_ERROR("Unable to get confirmation that thread has started: " << threadName);
	}

	//return threadID;
	return threadIDs.size()-1;
}

DWORD WINAPI ThreadSystemWin::startThread(LPVOID lpParameter) {
	ThreadParam data = *((ThreadParam*)lpParameter);
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	SetEvent(data.e1);
	DWORD res = WaitForSingleObject(data.e2, INFINITE);
	CloseHandle(data.e2);
	if (res != WAIT_OBJECT_0) {
		LOG_ERROR("Unable to get confirmation to start thread! Result code: " << res << " LastError: "<<GetLastError() << " Handle: "<< data.e2 );
		return -1;
	}
	data.t->pump();
	return 0;
}


