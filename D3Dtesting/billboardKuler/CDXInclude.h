#pragma once

#include <D3Dcompiler.h>
#include <string>
#include <list>
#include <memory>

/** Gotten from http://www.gamedev.net/topic/648071-d3dcompile-default-include-interface/ 8 jan 2015.
	Thank you sebi707!! */
class CDXInclude : public ID3DInclude
{
public:
  CDXInclude(const std::string& baseFile);

  HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
                         LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);

  HRESULT __stdcall Close(LPCVOID pData);

private:
  std::string m_baseDir;
  std::list<std::unique_ptr<char[]>> m_data;
};