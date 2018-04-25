#pragma once
#include <vector>
#include <string>
#include "ErrorCodes.h"
#include "ThreadBase.h"
#include "ThreadNotificationSystem.h"
#include "ThreadProxyCaller.h"

class ResourceThread : public Thread {
public:
	ResourceThread();

	// Proxies for calling save and load functionality
	ThreadProxyCallerFactory<ResourceThread, void, std::string, std::vector<char> > saveProxy;
	/** The callback sendt must be newed; this thread gains ownership. */
	ThreadProxyCallerFactory<ResourceThread, void, std::string, ThreadNotifyCallback<std::vector<char> >* > loadProxy;

	ThreadNotifierBase<ErrorCodes::Message> errorNotifier;

	// Should not be called direclty, but rather using the proxies defined above.
	void save(std::string filename, std::vector<char> data);

	/** Load the file and return it trough the callback that has to be newed and is deleted upon completion. */
	void load(std::string filename, ThreadNotifyCallback<std::vector<char> >* returData);

	void cleanup() override { ; }
};