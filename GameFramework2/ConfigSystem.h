#pragma once
#include <map>
#include <string>

using namespace std;

class Config {
	map<string, string> options;
public:
	/** Reads config options from the given file.
	*	@return false on error */
	bool readFile(string path);
	int getInt(string key);
	string getString(string key);
	void set(string key, string value);
	/** Reads config options from the given file.
	*	@return false on error */
	bool save(string path);
};