#include "CDXInclude.h"
#include <fstream>

template <typename T> struct unique_equals_raw
{
	unique_equals_raw(const T* raw)
		:_raw(raw)
    {}
  bool operator()(const std::unique_ptr<T[]>& ptr) const
    {
      return (ptr.get()==_raw);
    }
private:
  const T* _raw;
};


CDXInclude::CDXInclude(const std::string& baseFile)
{
  size_t pos = baseFile.rfind('/');
  if(pos != std::string::npos)
    m_baseDir = baseFile.substr(0, pos) + '/';
}

HRESULT CDXInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
                         LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
  // Open file
  std::ifstream file(m_baseDir + pFileName, std::ios::binary);
  if(!file.is_open())
  {
    *ppData = NULL;
    *pBytes = 0;
    return S_OK;
  }

  // Get filesize
  file.seekg(0, std::ios::end);
  UINT size = static_cast<UINT>(file.tellg());
  file.seekg(0, std::ios::beg);

  // Create buffer and read file
  std::unique_ptr<char[]> data(new char[size+1]);
  file.read(data.get(), size);
  data.get()[size] = '\0';
  *ppData = data.get();
  *pBytes = file.gcount();
  m_data.push_back(std::move(data));
  return S_OK;
}

HRESULT CDXInclude::Close(LPCVOID pData)
{
  m_data.remove_if( unique_equals_raw<char>((char*)pData) );
  return S_OK;
}