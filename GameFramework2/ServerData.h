#pragma once
#include <vector>

/** Simple blob with parts of a vector like interface.
*	The memory has to be managed from outside. 
*	Main diference from vector is no automtic resize.
*	Usefull for several c-like platform functions. */
struct CharBlob {
	char* m_data;
	int m_size;

	char& operator[](int num){
#ifdef _DEBUG
		if (num > m_size) {
			throw std::exception("Array out of bounds!");
		}
#endif
		return m_data[num];
	}

	char* data() {
		return m_data;
	}

	int& size() {
		return m_size;
	}
};

class ServerData {
public:
	virtual char& operator[](int i) = 0;
	virtual int size() = 0;
};


/** Server data implementation usfull for blob, vector and string. (used in cryptography) */
template <typename T>
class ServerDataStdImpl : public ServerData {
	T& data;
public:
	ServerDataStdImpl(T& b) : data(b) { ; }
	char& operator[](int i) { return data[i]; };
	int size() { return data.size(); }
};

/**
class BlobServerData {
	CharBlob& blob;
public:
	BlobServerData(CharBlob& b) blob(b);
	char& operator[](int i) { return blob[i]; };
	int size() { return blob.size(); }
};
*/

// typedef CharBlob ServerData;