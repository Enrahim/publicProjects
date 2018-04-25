#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>
#include <map>
#include <forward_list>
#include <time.h>
#include "ThreadSystem.h"

using namespace std;

/** Modeless thread class for easy hook in thread safe logging */
class LoggerThread : public Thread {
	virtual bool activateMessage(ThreadMessage* m) { return m->activate( ); };
	virtual void cleanup() { ; }
};

namespace StatisticsLogger {

	struct FunctionEnteringData {
		string name;
		clock_t startTime;
	};

	struct FunctionStatistics{
		int numCalled;
		clock_t processingTime;
		FunctionStatistics() : numCalled(0), processingTime(0){ ; }
	};

	/** Thread message that can be sendt to enable logging. For internal use. */
	class LogMessage : public ThreadMessage {
		forward_list<string>& log;
		const string msg;
	public:
		bool activate(IThreadSystem* comSys) {
			log.push_front(msg);
			return true;
		}
		LogMessage(forward_list<string>& l, const string m) : log(l), msg(m) { ; }
	};

	class Logger {
		stack<FunctionEnteringData*> functionStack;
		map<string, FunctionStatistics> functionData;
		time_t startTime;
		string filenameStart;
		bool outputToFile;
		forward_list<string> tracelog;
		forward_list<string> debuglog;
		forward_list<string> infolog;
		forward_list<string> warninglog;
		forward_list<string> errorlog;

		/** For threadsafety when threaded environement is on */
		IThreadSystem* threadSystem;
		int logThreadId;

		/** apilies a timestamp and saves the message in
		*	the given log.
		*	@param log one of the lists where this should be logged
		*	@param msg the message to be saved */
		void log(forward_list<string>& log, const string msg);

		/** outputs the given loglist to the given outstream */
		void printlog(ostream& out, const forward_list<string> log);

		/** outputs the functionstatistics to the given output */
		void printFunctionStatistics(ostream& out);

	public:
		/** Enables logging of function entering
		*	requires a corresponding exitFunction in order to work
		*	properly. Should thus only be called trough macros
		*	called when a profiled function is entered */
		void enterFunction(const string functionName);
		/** Called when a function entered with enterFunction is exited */
		void exitFunction();


		/** functions adding log entries in correct log */
		void trace(const string msg) { log(tracelog, msg); }
		void debug(const string msg) { log(debuglog, msg); }
		void info(const string msg) { log(infolog, msg); }
		void warning(const string msg) { log(warninglog, msg); }
		void error(const string msg) { log(errorlog, msg); }
		void fatal(const string msg) { log(errorlog, msg); printout(); }

		/** prints out the current loging information
		*	if file loging, the old logfile will be overwritten */
		void printout();

		void enableThreading(IThreadSystem* sys, int id);

		Logger() : startTime(time(NULL)), outputToFile(false), threadSystem(NULL) { ; }
	};

	extern Logger defLogger;

}

#define DEBUGLEVEL 6

/** logmacros with diferent logingheights
*	first empty, then redefined if loglevel permits */
#define LOG_TRACE(a)
#define LOG_DEBUG(a)
#define LOG_INFO(a)
#define LOG_WARNING(a)
#define LOG_ERROR(a)
#define LOG_FATAL(a)

/** the parameters of these loggingmacros should be of the form
*	[data]<<[data]<<[data] without parantesises.
*	cannot be empty */
#if DEBUGLEVEL>0
#undef LOG_FATAL 
#define LOG_FATAL(a) StatisticsLogger::defLogger.fatal \
	(((ostringstream*)(&(ostringstream() << __FILE__ << " " << __LINE__ << ": " << a)))->str());

#if DEBUGLEVEL>1
#undef LOG_ERROR
#define LOG_ERROR(a) StatisticsLogger::defLogger.error \
	(((ostringstream*)(&(ostringstream() << __FILE__ << " " << __LINE__ << ": " << a)))->str());

#if DEBUGLEVEL>2
#undef LOG_WARNING
#define LOG_WARNING(a) StatisticsLogger::defLogger.warning \
	(((ostringstream*)(&(ostringstream() << __FILE__ << " " << __LINE__ << ": " << a)))->str());

#if DEBUGLEVEL>3
#undef LOG_INFO
#define LOG_INFO(a) StatisticsLogger::defLogger.info \
	(((ostringstream*)(&(ostringstream() << __FILE__ << " " << __LINE__ << ": " << a)))->str());

#if DEBUGLEVEL>4
#undef LOG_DEBUG
#define LOG_DEBUG(a) StatisticsLogger::defLogger.debug \
	(((ostringstream*)(&(ostringstream() << __FILE__ << " " << __LINE__ << ": " << a)))->str());

#if DEBUGLEVEL>5
#undef LOG_TRACE
#define LOG_TRACE(a) StatisticsLogger::defLogger.trace \
	(((ostringstream*)(&(ostringstream() << __FILE__ << " " << __LINE__ << ": " << a)))->str());
#endif
#endif
#endif
#endif
#endif
#endif


/** Profiling macros. As these are incredibly nasty they should only
*	be used when needed. That is code as usual, then when profiling
*	put these macros around the corresponding parts that you want to
*	profile, and remove them again imediately after profiling */

/** probably the prefered way of doing the profiling */
#define LFCALL(a) StatisticsLogger::defLogger.getInstance()->enterFunction(#a);\
	a; StatisticsLogger::defLogger.exitFunction();

/** incredibly dangerous macro, handle with care!
*	a should be the standard beginning of a function declaration.
*	That is follow the following pattern:
*	typename functionname(typename in name, [...] ) */
#define LFBEGIN(a) a { \
	StatisticsLogger::defLogger.enterFunction(#a);

/** Still dangerous macros. Not meant for use in any critical aplications
*	covering all exit from function situations */
#define LFEND() StatisticsLogger::defLogger.exitFunction(); \
	}

#define LFRETEND(b) StatisticsLogger::defLogger.exitFunction(); \
	return (b); \
		}

#define LFRETURNVOID() StatisticsLogger::defLogger.exitFunction(); \
	return;

#define LFRETURN(b) StatisticsLogger::defLogger.exitFunction(); \
	return (b);
