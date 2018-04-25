#include "ConfigSystem.h"
#include "SafeStatisticsLogger.h"
#include <fstream>

bool Config::readFile(string path) {
	ifstream file(path);
	if (!file.is_open()) {
		LOG_ERROR("Unable to open configuration file: " << path);
		return false;
	}
	while (!file.eof()) {
		string key, value;
		file >> key;
		file >> std::ws; // requires each key to actually have a value! single string lines break.
		getline(file, value);
		options[key] = value;
	}
	file.close();
	return true;
}

int Config::getInt(string key) {
	return atoi(options[key].c_str());
}

string Config::getString(string key){
	return options[key];
}

void Config::set(string key, string value) {
	options[key] = value;
}

bool Config::save(string path) {
	ofstream file(path);
	if (!file.is_open()) {
		LOG_ERROR("Unable to open configuration file for writing: " << path);
		return false;
	}
	map<string, string>::iterator it = options.begin();
	while (it!=options.end()) {
		file << it->first << " " << it->second << "\n";
		it++;
	}
	return true;
}