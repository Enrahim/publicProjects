#pragma once

#include "ThreadMessageBase.h"
#include <list>

template <typename T>
class ThreadNotifyCallback{
	MessageSender& listenerThread;
protected:
	/** Returns a *new* threadMessage that is sent to caller when the assosiated notification is fired. 
	*	Must be implemented to give wanted callback behavior. */
	virtual ThreadMessage* getSendMessage(T data) = 0;
public:
	ThreadNotifyCallback(MessageSender& listener) : listenerThread(listener) {
	}

	void call(T data);
};

template <typename T>
class ThreadNotifierBase {
	std::list<ThreadNotifyCallback<T>* > notifyCallbacks;
	
public:
	/** Should not be called directly, rather send a RegisterNotifyMessage. */
	void registerCallback(ThreadNotifyCallback < T >* callback);

	/** Call to notify all registered listener of a event of type T */
	void notify(T data);

	~ThreadNotifierBase();
};


template <typename T>
class RegisterThreadNotifyMessage : public ThreadMessage {
	ThreadNotifierBase<T>& m_base;
	ThreadNotifyCallback<T>* m_callback;
public:
	/** Prepares a register notify message for sending. 
	*	@param base: Pointer to the notification base for the given callback to be attahed to. 
	*	@param callback: Pointer to a newed callback to owner thread. This message claims ownership. */
	RegisterThreadNotifyMessage(ThreadNotifierBase<T>& base, ThreadNotifyCallback<T>* callback) :
		m_base(base), m_callback(callback) {;}

	virtual bool activate();
};



///////////////////
// Implementations
///////////////////

template<typename T>
inline void ThreadNotifyCallback<T>::call(T data)
{
	listenerThread.send(getSendMessage(data));
}

template<typename T>
inline void ThreadNotifierBase<T>::registerCallback(ThreadNotifyCallback<T>* callback)
{
	notifyCallbacks.push_back(callback);
}

template<typename T>
inline void ThreadNotifierBase<T>::notify(T data)
{
	for (auto it = notifyCallbacks.begin(); it != notifyCallbacks.end(); it++) {
		(*it)->call(data);
	}
}

template<typename T>
inline ThreadNotifierBase<T>::~ThreadNotifierBase()
{
	for (auto it = notifyCallbacks.begin(); it != notifyCallbacks.end(); it++) {
		if ((*it) != nullptr) delete (*it);
	}
}

template<typename T>
inline bool RegisterThreadNotifyMessage<T>::activate()
{
	m_base.registerCallback(m_callback);
	return true;
}