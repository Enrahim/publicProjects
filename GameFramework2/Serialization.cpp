#include "Serialization.h"

/* Binary restricting primitive load and write helper functions */


void SaveWriter::write(int i) {
	write((char)(i));
	write((char)(i >> 8));
	write((char)(i >> 16));
	write((char)(i >> 24));
}

void SaveWriter::write(short i) {
	write((char)(i));
	write((char)(i >> 8));
}

void SaveWriter::write(char i) {
	output.write( i );
}

void SaveWriter::write(unsigned int i) {
	write(static_cast<int>(i));
}

void SaveWriter::write(unsigned short i) {
	write(static_cast<short>(i));
}

void SaveWriter::write(unsigned char i) {
	write(static_cast<char>(i));
}

void SaveWriter::write(long long i)
{
	write((int)i);
	write((int)(i >> 32));
}

void SaveWriter::write(unsigned long long i)
{
	write(static_cast<long long>(i));
}

int LoadReader::readInt(){
	char tmp[4];
	input.read(tmp, 4);
	return tmp[0] | (((int)(tmp[1])) << 8) | (((int)(tmp[2])) << 16) | (((int)(tmp[3])) << 24);
}

short LoadReader::readShort(){
	char tmp[2];
	input.read(tmp, 2);
	return tmp[0] | (((short)(tmp[1])) << 8);
}

char LoadReader::readChar() {
	char tmp;
	input.read(&tmp, 1);
	return tmp;
}

void LoadReader::read(int* i) {
	(*i) = readInt();
}

void LoadReader::read(short* i) {
	(*i) = readShort();
}

void LoadReader::read(char* i) {
	(*i) = readChar();
}

void LoadReader::read(unsigned int* i) {
	read((int*)i);
}

void LoadReader::read(unsigned short* i) {
	read((short*)i);
}

void LoadReader::read(unsigned char* i) {
	read((char*)i);
}

void LoadReader::read(long long* i) {
	(*i) = readInt();
	(*i) <<= 32;
	(*i) |= readInt();
}

void LoadReader::read(unsigned long long * i)
{
	read((long long*) i);
}
