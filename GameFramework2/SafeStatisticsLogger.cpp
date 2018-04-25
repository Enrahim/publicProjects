#include "SafeStatisticsLogger.h"

namespace StatisticsLogger{
	Logger defLogger;
}

using namespace StatisticsLogger;

void Logger::enableThreading(IThreadSystem* sys, int id) {
	threadSystem = sys;
	logThreadId = id;
}

void Logger::enterFunction(const string functionName) {
	functionData[functionName].numCalled++;
	FunctionEnteringData* n = new FunctionEnteringData();
	n->name = functionName;
	n->startTime = clock();
	functionStack.push(n);
}

void Logger::exitFunction() {
	clock_t deltaT = clock() - functionStack.top()->startTime;
	string name = functionStack.top()->name;
	delete functionStack.top();
	functionStack.pop();
	functionData[name].processingTime += deltaT;
}

void Logger::log(forward_list<string>& log, const string msg) {
	stringstream ss;
	ss << ((clock() * 1000) / CLOCKS_PER_SEC) << " " << msg;
	if (!threadSystem) {
		log.push_front(ss.str());
	}
	else {
		threadSystem->sendMessage(logThreadId, new LogMessage(log, ss.str()));
	}
	//log.push_front(((ostringstream*)(&(ostringstream() << ((clock()*1000)/CLOCKS_PER_SEC) << " " << msg)))->str());
}

void Logger::printlog(ostream& out, forward_list<string> log) {
	forward_list<string>::iterator it = log.begin();
	while (it != log.end()) {
		out << *it << endl;
		it++;
	}
}

void Logger::printFunctionStatistics(ostream& out) {
	map<string, FunctionStatistics>::iterator it = functionData.begin();
	while (it != functionData.end()) {
		out << it->first << " " << it->second.numCalled << " " <<
			(it->second.processingTime * 1000) / CLOCKS_PER_SEC << endl;
		it++;
	}
}

void Logger::printout() {
	ostream* out;
	if (outputToFile) {
		stringstream filename;
		filename << filenameStart << "log" << startTime;
		out = new ofstream(filename.str(), fstream::out, fstream::trunc);
	}
	else  {
		out = &cout;
	}
	*out << "Logging time (in ms): " << ((clock() * 1000) / CLOCKS_PER_SEC) << endl << endl;
	*out << "Errorlog:" << endl;
	printlog(*out, errorlog);
	*out << endl << "Warninglog:" << endl;
	printlog(*out, warninglog);
	*out << endl << "Infolog:" << endl;
	printlog(*out, infolog);
	*out << endl << "Debuglog:" << endl;
	printlog(*out, debuglog);
	*out << endl << "Tracelog:" << endl;
	printlog(*out, tracelog);

	*out << endl << "Function Statistic:" << endl;
	printFunctionStatistics(*out);

	if (outputToFile) {
		ofstream* temp = (ofstream*)out;
		temp->close();
		delete temp;
	}
}




/** Example of how functions might be defined and used
*	should only be done on crititcal functions.

LFBEGIN(void testfunksjon())
int test=0;
test++;
LFEND()

LFBEGIN(int testfunksjon2(int a, int b))
LFRETEND(a+b)

*/