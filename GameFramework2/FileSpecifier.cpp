#include "FileSpecifier.h"
#include <fstream>

void GameStateFiles::setFileName(CollectionId id, std::string filename)
{
	if(collectionFiles.find(id)==collectionFiles.end())
	{
		collectionFiles[id].merge = false;
		collectionFiles[id].readOnly = id > READ_ONLY_TREASHOLD;
	}
	collectionFiles[id].filename = filename;
}

void GameStateFiles::setFileState(CollectionId id, bool readOnly, bool merge)
{
	if (collectionFiles.find(id) != collectionFiles.end())
	{
		collectionFiles[id].merge = merge;
		collectionFiles[id].readOnly = readOnly;
	}
}

bool GameStateFiles::saveFileState(std::string filestatefile)
{
	std::ofstream file(filestatefile);
	if (file.bad()) {
		return false;
	}
	for (auto it = collectionFiles.begin(); it != collectionFiles.end(); it++) {
		file << it->first << " " << it->second.readOnly << " " << it->second.merge << " " << it->second.filename << endl;
	}
	return true;
}

bool GameStateFiles::loadFileState(std::string filestatefile) {
	std::ifstream file(filestatefile);
	if (file.bad()) {
		return false;
	}
	file >> skipws;
	while (!file.eof()) {
		CollectionId id;
		GamestateFileSpecifier specifier;
		file >> id;
		file >> specifier.readOnly;
		file >> specifier.merge;
		file >> specifier.filename;
		collectionFiles[id] = specifier;
	}
	return true;
}
