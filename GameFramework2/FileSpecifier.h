#pragma once
#include "ObjectCollection.h"
#include <string>
#include <map>

struct GamestateFileSpecifier {
	std::string filename;
	bool readOnly;
	bool merge;
};

struct GameStateFiles {
	static const CollectionId READ_ONLY_TREASHOLD = 10;

	std::map<CollectionId, GamestateFileSpecifier> collectionFiles;

	/** Registers a file in the system
	*	It will by default be read only for ids higher than READ_ONLY_TREASHOLD, and never merge */
	virtual void setFileName(CollectionId id, std::string filename);

	/** Sets the filestate for a file already set with setFileName */
	virtual void setFileState(CollectionId id, bool readOnly, bool merge);

	virtual bool saveFileState(std::string filestatefile);
	virtual bool loadFileState(std::string filestatefile);

};