#pragma once

#include "ServerData.h"

typedef unsigned int CheckSum;


class Cryptation {
public:
	/** encrypts a string, returning a checksum of the unencrypted message*/
	virtual CheckSum encrypt(ServerData& data, int offset) { return 0; }
	/** decrypts a string, returning a checksum of the encrypted message */
	virtual CheckSum decrypt(ServerData& data, int offset) { return 0; }
};


namespace CryptationHelper {
	struct CryptationData {
		long long key;
		ServerData& data;
		int offset;
		CheckSum checksum;
		CryptationData(long long k, ServerData& d, int o, CheckSum cs) :
			key(k), data(d), offset(o), checksum(cs) {;}
	};
	
	/** Symetric encryption. 
	*	To decrypt simply run again with same key. 
	*	Encrypt indicates if the checksum is calculated before or after
	*	Return value includes the enw key for further encryption in this sequence */
	CryptationData symiCrypt(CryptationData, bool encrypt);

	unsigned long long power(unsigned long long, unsigned long long);
	std::string ullToString(unsigned long long);
	unsigned long long stringToUll(std::string);
	
	/** Get a not very safe random number for cryptation purposes 
	*	Not threadsafe, but then this is supposed to be random anyway..*/
	unsigned long long getRandom();

	/** As getRandom, but more safe. 
	*	Values from this should never be exposed over network */
	unsigned long long getSecret();

	class CryptSequence {
		unsigned long long key;
		void step();
	public:
		CryptSequence(unsigned long long key);
		void skip(int num);
		char nextChar();
		void reset(unsigned long long key);
		unsigned long long nowKey();
	};
};
 
class BasicEncryption : public Cryptation {
	unsigned int lastSteps;
	long long lastKey;
	
	unsigned int steps;
	long long key;

	CheckSum commonCrypt(ServerData& data, int offset, bool encrypt);

public:
	BasicEncryption(long long k) : key(k), steps(0) {
	}
	virtual CheckSum encrypt(ServerData& data, int offset);
	virtual CheckSum decrypt(ServerData& data, int offset);
	
	/** As this is based on a sequence the stepnumber must be set
	*	before running. This can't roll back, and will overwrite
	*	the backup making a new rollback go here. 
	*	@return true if moving forward and ok, false otherwise. */
	bool setSteps(unsigned int steps);
	/** If checksum is wrong, the previous set steps might be wrong
	*	For these cases call rollback. */
	void rollback();
};

class DecryptingState {
	BasicEncryption* confirmed;
	BasicEncryption* suggested;
public:
	bool tryDecrypt(ServerData& data, int offset, unsigned int step, CheckSum cs);
	void suggestKey(long long key);
	~DecryptingState();
};