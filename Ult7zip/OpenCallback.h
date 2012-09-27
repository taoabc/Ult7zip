#ifndef U7Z_OPENCALLBACK_H_
#define U7Z_OPENCALLBACK_H_

#include "ComUtility.h"
#include <initguid.h>
#include "7z/cpp/7zip/IPassword.h"
#include "7z/cpp/7zip/archive/IArchive.h"

#include <string>

class OpenCallback :
    public IArchiveOpenCallback,
    public ICryptoGetTextPassword,
    public UnknownImp {

public:

  OpenCallback(void);

  void SetPassword(const std::wstring& password);

  //IUnknown
  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID riid, void** ppobj);

  //IArchiveOpenCallback
  STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
  STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);

  //ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

private:

  std::wstring password_;
  bool password_is_defined_;
};

#endif