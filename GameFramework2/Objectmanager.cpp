#include "ObjectManager.h"
#include "Serialization.h"
#include "SafeStatisticsLogger.h"
#include <fstream>
using namespace std;

int IndexAllocator::allocate() {
	if (freeList.empty()){
		return (++maxUsed);
	}
	int tmp = *(freeList.cbegin());
	freeList.erase(freeList.cbegin());
	return tmp;
}

void IndexAllocator::free(int id) {
	if (id == maxUsed) {
		maxUsed--;
		while (maxUsed == (*freeList.crbegin())) {
			freeList.erase( --freeList.end() );
			maxUsed--;
		}
	}
	else {
		freeList.insert(id);
	}
}

void IndexAllocator::clear() {
	maxUsed = -1;
	freeList.clear();
}

int IndexAllocator::numUsed() {
	return maxUsed - freeList.size();
}

UsedIterator IndexAllocator::getIterator() {
	return UsedIterator(freeList, maxUsed);
}

void IndexAllocator::load(LoadReader& reader) {
	maxUsed = reader.readInt();
	int numFree = reader.readInt();
	for (int i = 0; i < numFree; i++) {
		freeList.insert(freeList.begin(), reader.readInt());
	}
}

void IndexAllocator::save(SaveWriter& writer) {
	writer.write(maxUsed);
	writer.write((int)freeList.size());
	set<int>::reverse_iterator rit = freeList.rbegin();
	while (rit != freeList.rend()) {
		writer.write(*rit);
		rit++;
	}
}


void ObjectTypeManager::freeObject(int id) {
	idControll.free(id);
	onFree(id);
}


int ObjectTypeManager::allocate() {
	return idControll.allocate();
}

void ObjectTypeManager::setFile(string path){
	fileName = path;
}

int ObjectTypeManager::numObjects() {
	return idControll.numUsed();
}

void ObjectTypeManager::baseCleanup()  {
	cleanup();
	idControll.clear();
}


bool ObjectTypeManager::readFromFile() {
	ifstream file(fileName, std::ios::binary);
	if (!file.is_open()) {
		LOG_ERROR("unable to open file: " + fileName + " for loading");
		return false;
	}
	baseCleanup();
	LoadReader loader(file);
	idControll.load(loader);
	load(loader);
	file.close();
	return true;
}

bool ObjectTypeManager::saveToFile() {
	ofstream file(fileName, std::ios::binary | std::ios::trunc);
	if (!file.is_open()) {
		LOG_ERROR("unable to open file: " + fileName + " for loading");
		return false;
	}
	SaveWriter	saver(file);
	idControll.save(saver);
	save(saver);
	file.close();
	return true;
}

UsedIterator::UsedIterator(set<int>& free, int max) :
	maxInt(max), now(0) {
	freeIt = free.begin();
	endIt = free.end();
}

void UsedIterator::next() {
	now++;
	while (freeIt!=endIt && now == *freeIt) {
		now++;
		freeIt++;
	}
}

bool UsedIterator::done() {
	return (now > maxInt);
}