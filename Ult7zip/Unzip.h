#pragma once

#include "Ult7zip.h"
#include "ComUtility.h"

class Unzip :
    public IU7zUnzip,
    public UnknownImp {

public:

  Unzip(void);
  ~Unzip(void);

  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID riid, void** ppobj);
  
  STDMETHOD(Init)(LPCWSTR xapath);
  STDMETHOD(Open)(LPCWSTR packpath);
  STDMETHOD(Open)(LPCVOID data, ULONGLONG datalen);
  STDMETHOD(Extractor)(LPCWSTR targetpath, IU7zExtractEvent* callback);

private:

};