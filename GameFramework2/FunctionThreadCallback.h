#pragma once

#include "ThreadNotificationSystem.h"
#include "InternalMessageSender.h"

/**
*	A convinience template for callbacks where a member function is requested called
*	@param T Type of class that contains the function to be called.
*			Must extend thread (or at least implement "getSystem()" and "getID()"
*	@param A Type of parameter to the function
*	@param FN function that is called on the given threads.
*/
template<typename T, typename A, void (T::*FN)(A) >
class FunctionThreadCallback : public ThreadNotifyCallback<A> {
	class FunctionCallMessage : public ThreadMessage {
		T& base;
		A arg;
	public:
		FunctionCallMessage(T& b, A a) : base(b), arg(a) { ; }
		bool activate() {
			(base.*FN)(arg);
			return true;
		}
	};

	T& base;
protected:
	ThreadMessage* getSendMessage(A data) { return new FunctionCallMessage(base, data); }
public:
	FunctionThreadCallback(T& b) :
		ThreadNotifyCallback<A>(InternalMessageSender(b.getSystem(), b.getID())),
		base(b) { ; }
};

/*
class NetworkThreadErrorMessage : public ThreadMessage {
NetworkThread& home;
UDPMsg::ErrorCode code;
public:
NetworkThreadErrorMessage(NetworkThread& h, UDPMsg::ErrorCode error) : home(h), code(error) { ; }
bool activate(IThreadSystem& ioSys);
};

bool NetworkThreadErrorMessage::activate(IThreadSystem & ioSys)
{
home.handleError(code);
return false;
}

class NetworkThreadErrorCallback : public ThreadNotifyCallback<UDPMsg::ErrorCode> {
NetworkThread& home;
public:
NetworkThreadErrorCallback(NetworkThread& h) : home(home), ThreadNotifyCallback<UDPMsg::ErrorCode>(h.getID()) { ; }
virtual ThreadMessage* getSendMessage(UDPMsg::ErrorCode data) { return new NetworkThreadErrorMessage(home, data); }
};
*/