#ifndef U7Z_OPENCALLBACK_H_
#define U7Z_OPENCALLBACK_H_

#include "ComUtility.h"
#include <initguid.h>
#include "7z/cpp/7zip/IPassword.h"
#include "7z/cpp/7zip/archive/IArchive.h"

#include <string>

class OpenCallback :
    public IArchiveOpenCallback,
    public IArchiveOpenVolumeCallback,
    public ICryptoGetTextPassword,
    public IArchiveOpenSetSubArchiveName,
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

  //IArchiveOpenVolumeCallback
  STDMETHOD(GetProperty)(PROPID propID, PROPVARIANT *value);
  STDMETHOD(GetStream)(const wchar_t *name, IInStream **inStream);

  //ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

  //IArchiveOpenSetSubArchiveName
  STDMETHOD(SetSubArchiveName)(const wchar_t *name);

private:

  std::wstring password_;
  bool password_is_defined_;
};

#endif