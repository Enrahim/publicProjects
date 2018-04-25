#include "ResourceThread.h"
#include <fstream>

ResourceThread::ResourceThread() :
	saveProxy(*this, &ResourceThread::save),
	loadProxy(*this, &ResourceThread::load)	
{
}

void ResourceThread::save(std::string filename, std::vector<char> data)
{
	ofstream stream(filename);
	if (stream.bad()) {
		errorNotifier.notify(ErrorCodes::Message(ErrorCodes::SAVE_FAILED, filename));
		return;
	}
	stream.write(data.data(), data.size());
	stream.close();
}

void ResourceThread::load(std::string filename, ThreadNotifyCallback<std::vector<char> >* returData)
{
	static const unsigned int BUFFERSIZE = 1000;
	vector<char> res;
	ifstream stream(filename);
	if (stream.bad()) {
		errorNotifier.notify(ErrorCodes::Message(ErrorCodes::LOAD_FAILED, filename));
		returData->call(res);
		delete returData;
		return;
	}
	char buffer[BUFFERSIZE];
	std::streamsize acum = 0;
	while (!stream.eof()) {
		std::streamsize read = stream.readsome(buffer, BUFFERSIZE);
		res.reserve(acum + read);
		memcpy(&res[acum], buffer, (size_t)read);
	}
	returData->call(res);
	delete returData;
}
