#pragma once
#include "ThreadMessageBase.h"
#include "ThreadNotificationSystem.h"


namespace detail {
	template<typename R, typename ... ARGS>
	class ArgStore;

	template<typename R>
	class ArgStore<R> {
	protected:
		virtual R doCall() = 0;
	public:
		virtual R call() {
			return doCall();
		}
	};

	template<typename R, typename A, typename... ARGS>
	class ArgStore<R, A, ARGS...> : public ArgStore<R, ARGS...> {
		A arg;

	protected: 
		virtual R doCall(A a, ARGS... args) = 0;

		R doCall(ARGS... args) {
			return doCall(arg, args...);
		}
	public:
		ArgStore(A a, ARGS ... args) : arg(a), ArgStore<R, ARGS...>(args...) { ; }
	};

	template<typename T, typename R, typename ... ARGS>
	class ThreadProxyCaller : private detail::ArgStore<R, ARGS...>, public ThreadMessage {
		ThreadNotifyCallback<R>* callback;
		T& thread;
		R (T::*function)(ARGS...);
	protected:
		R doCall(ARGS ... args) { return (thread.*function)(args...); }
	public:
		ThreadProxyCaller(T& t, R (T::*fn)(ARGS...), ThreadNotifyCallback<R>* c, ARGS...args) :
			thread(t), function(fn), callback(c), detail::ArgStore<R, ARGS...>(args...) { ; }

		bool activate() {
			R res = call();
			if (callback) callback->call(res);
			return true;
		}
	};

	template<typename T, typename ... ARGS>	
	class ThreadProxyCaller<T, void, ARGS...> : private detail::ArgStore<void, ARGS...>, public ThreadMessage {
		ThreadNotifyCallback<void>* callback;
		T& thread;
		void(T::*function)(ARGS...);
	protected:
		void doCall(ARGS ... args) { (thread.*function)(args...); }
	public:
		ThreadProxyCaller(T& t, void(T::*fn)(ARGS...), ThreadNotifyCallback<void>* c, ARGS...args) :
			thread(t), function(fn), callback(c), ArgStore<void, ARGS...>(args...) { ; }

		bool activate() {
			return true;
		}
	};
}

template<typename T, typename R, typename ... ARGS>
class ThreadProxyCallerFactory {
	T& thread;
	R(T::*function)(ARGS...);
public:
	/** Makes a factory for messages that calls the given function on the given object */
	ThreadProxyCallerFactory(T& t, R (T::*fn)(ARGS...)) :
		thread(t), function(fn) {;}

	/** Make a call with a callback for handling the return value */
	void makeCall(ThreadNotifyCallback<R>* callback, ARGS... args) {
		detail::ThreadProxyCaller<T, R, ARGS...>* caller = new detail::ThreadProxyCaller<T, R, ARGS...>(
			thread, function, callback, args...);
		thread.sendToMe(caller);
	}

	/** Make call where you don't care about the returnvalue by just sending arguments */
	void makeCall(ARGS... args) {
		makeCall(nullptr, args...);
	}
};

//class A {
//public:
//	bool test(int);
//};
//
//class B {
//	bool (A::*function)(int);
//public:
//	B(bool (A::*fn)(int)) : function(fn) { ; }
//};
//
//bool A::test(int a){
//	B b(test);
//	B* andreB = new B(test);
//	ThreadProxyCallerFactory<B, bool, int> pf(b, (bool (B::*)(int)) A::test);
//	ThreadProxyCallerFactory<B, bool, int> t2 = ThreadProxyCallerFactory<B, bool, int>(b, (bool (B::*)(int)) A::test);
//}