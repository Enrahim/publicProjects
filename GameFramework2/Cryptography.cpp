#include "Cryptography.h"
#include <ctime>

unsigned long long CryptationHelper::power(unsigned long long base, unsigned long long exponent)
{
	static unsigned long long tmp[64];
	tmp[0] = base;
	for (int i = 1; i < 64; i++) {
		tmp[i] = tmp[i - 1] * tmp[i - 1];
	}
	unsigned long long res = 1;
	for (int i = 0; i < 64 && exponent>0; i++) {
		if (exponent & 1) res *= tmp[i];
		exponent >>= 1;
	}
	return res;
}

std::string CryptationHelper::ullToString(unsigned long long num)
{
	std::string res("        ");
	for (int i = 0; i < 8; i++) {
		res[i] = num & 255;
		num >>= 8;
	}
	return res;
}

unsigned long long CryptationHelper::stringToUll(std::string str)
{
	unsigned long long res = 0;
	for (int i = 7; i >= 0; i++) {
		res |= str[i];
		res <<= 8;
	}
	return res;
}

unsigned long long CryptationHelper::getRandom()
{
	static CryptSequence sequence(clock());
	sequence.skip(2);
	return sequence.nowKey();
}

unsigned long long CryptationHelper::getSecret()
{
	time_t dummy;
	CryptSequence crypt(clock() * time(&dummy));
	std::string code = "dweoed2f";
	for (int i = 0; i < 8; i++) {
		code[i] ^= crypt.nextChar();
	}
	return stringToUll(code);
}

using namespace CryptationHelper;
CryptationData CryptationHelper::symiCrypt(CryptationData d, bool encrypt) {
	CryptSequence keys(d.key);
	int checksum = d.checksum;
	for (int i = d.offset; i < d.data.size(); i++) {
		if (encrypt) checksum += d.data[i];
		d.data[i] ^= keys.nextChar();
		if (!encrypt) checksum += d.data[i];
	}
	d.key = keys.nowKey();
	d.checksum = checksum;
	return d;
}

void CryptationHelper::CryptSequence::step()
{
	key *= 5325123231;
}

CryptationHelper::CryptSequence::CryptSequence(unsigned long long k) : key(k)
{

}

void CryptationHelper::CryptSequence::skip(int num)
{
	for (int i = 0; i < num; i++) step();
}

char CryptationHelper::CryptSequence::nextChar()
{
	step();
	return (char) (key >> 50);
}

void CryptationHelper::CryptSequence::reset(unsigned long long k)
{
	key = k;
}

unsigned long long CryptationHelper::CryptSequence::nowKey()
{
	return key;
}


using namespace CryptationHelper;
CheckSum BasicEncryption::commonCrypt(ServerData & sdata, int offset, bool encrypt)
{
	CryptationData cdata(key, sdata, offset, 0);
	CryptationData cdata2(symiCrypt(cdata, encrypt));
	key = cdata2.key;
	steps += sdata.size() - offset;
	return cdata2.checksum;
}

CheckSum BasicEncryption::encrypt(ServerData & sdata, int offset)
{
	return commonCrypt(sdata, offset, true);
}

CheckSum BasicEncryption::decrypt(ServerData & sdata, int offset)
{
	return commonCrypt(sdata, offset, true);
}

bool BasicEncryption::setSteps(unsigned int s)
{
	if (s < steps) return false;
	lastKey = key;
	lastSteps = steps;

	if (s == steps) return true;
	CryptSequence cs(key);
	cs.skip(s - steps);
	key = cs.nowKey();
	steps = s;
	return true;
}

void BasicEncryption::rollback()
{
	steps = lastSteps;
	key = lastKey;
}

bool DecryptingState::tryDecrypt(ServerData & data, int offset, unsigned int step, CheckSum cs)
{
	if (confirmed) {
		if (confirmed->setSteps(step)) {
			if (cs == confirmed->decrypt(data, offset)) return true;
			confirmed->rollback();
			confirmed->setSteps(step);
			confirmed->encrypt(data, offset);
			confirmed->rollback();
		}
	}
	if (suggested) {
		if (suggested->setSteps(step)) {
			if (cs == suggested->decrypt(data, offset)) {
				if (confirmed) {
					delete confirmed;
				}
				confirmed = suggested;
				suggested = NULL;
				return true;
			}
		}
	}
	return false;
}

void DecryptingState::suggestKey(long long key)
{
	if (suggested) delete suggested;
	suggested = new BasicEncryption(key);
}

DecryptingState::~DecryptingState()
{
	if (confirmed) delete confirmed;
	if (suggested) delete suggested;
}
