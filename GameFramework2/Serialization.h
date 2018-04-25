#pragma once
#include <iostream>
#include <vector>
#include "ServerData.h"

using namespace std;

class SimpleOutStream {
public:
	virtual void write(char out) = 0;
};

class SimpleInStream {
public:
	virtual void read(char* in, int length) = 0;
};

class StdOutStream : public SimpleOutStream {
	ostream& output;
public:
	virtual void write(char out) { output << out; }
	StdOutStream(ostream& o) : output(o) { ; }
};

class StdInStream : public SimpleInStream {
	istream& input;
public:
	virtual void read(char* in, int length) { input.read(in, length); }
	StdInStream(istream& i) : input(i) { ; }
};

class VectorOutStream : public SimpleOutStream {
	vector<char>& output;
public:
	virtual void write(char out) { output.push_back(out); }
	VectorOutStream(vector<char>& o) : output(o)  {}
};

class VectorInStream : public SimpleInStream {
	vector<char>& input;
	int pos;
public:
	virtual void read(char* in, int length) {
		for (int i = 0; i < length && pos < input.size(); i++) {
			in[i] = input[pos++];
		}
	}
	VectorInStream(vector<char>& i, int p = 0) : input(i), pos(p) { ; }
};

class BlobInStream : public SimpleInStream {
	CharBlob& input;
	int pos;
public:
	virtual void read(char* in, int length) {
		for (int i = 0; (i < length) && (pos < input.m_size); i++) {
			in[i] = input[pos++];
		}
	}
	BlobInStream(CharBlob& i, int p = 0) : input(i), pos(p) { ; }
};

class SaveWriter {
	SimpleOutStream& output;
public:
	void write(int i);
	void write(short i);
	void write(char i);
	void write(unsigned int i);
	void write(unsigned short i);
	void write(unsigned char i);
	void write(long long i);
	void write(unsigned long long i);

	template<typename T>
	void write(T& data) { data.serialize(*this); }

	template<typename T>
	void io(T& data) { write(data); }

	SaveWriter(SimpleOutStream& os) : output(os) { ; }
};



class LoadReader {
	SimpleInStream& input;
public:
	int readInt();
	short readShort();
	char readChar();

	void read(int* i);
	void read(short* i);
	void read(char* i);
	void read(unsigned int* i);
	void read(unsigned short* i);
	void read(unsigned char* i);
	void read(long long* i);
	void read(unsigned long long* i);

	template<typename T>
	void read(T* data) { data->deserialize(*this); }

	template<typename T>
	void io(T& data) { read(&data); }


	LoadReader(SimpleInStream& is) : input(is) { ; }
};

/** Serialisable wrapper for standard library collection that supports emplace back */
template<typename T> 
class CollectionWrapper{
	T& data; 
public:
	void serialize(SaveWriter& writer) {
		writer.write(data.size());
		for (T::iterator it = data.begin(); it != data.end(); it++) {
			writer.write(*it);
		}
	}

	void deserialize(LoadReader& reader)  {
		data.clear();
		int size; 
		reader.read(&size);
		for (int i = 0; i < size; i++) {
			data.emplace_back();
			reader.read(&data.back());
		}
	}
	CollectionWrapper<T>(T& d) : data(d) { ; }
};

template<>
class CollectionWrapper<std::string> {
	std::string& data;
public:
	void serialize(SaveWriter& writer) {
		writer.write(data.size());
		for (std::string::iterator it = data.begin(); it != data.end(); it++) {
			writer.write(*it);
		}
	}

	void deserialize(LoadReader& reader) {
		data.clear();
		int size;
		reader.read(&size);
		for (int i = 0; i < size; i++) {
			char d;
			reader.read(&d);
			data.push_back(d);
		}
	}
	CollectionWrapper(std::string& d) : data(d) { ; }
};

#define SIMPLE_IO_CALLS void serialize(SaveWriter& s) { io( s ); }\
						void deserialize(LoadReader& r) { io( r ); }\

// io function:
/*
SIMPLE_IO_CALLS;

template <typename T>
void io(T& ioSys) {
	ioSys.io(attrib1);
	ioSys.io(attrib2);
....
*/