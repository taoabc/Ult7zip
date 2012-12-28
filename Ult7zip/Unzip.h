#pragma once

#include "Ult7zip.h"
#include "ComUtility.h"
#include "ExtractCallback.h"
#include "OpenCallback.h"

#include "ult/library.h"

#include <string>

typedef UINT32 (__stdcall *PCreateObject)
    (const GUID* clsid, const GUID* interfaceid, void**outobject);

DEFINE_GUID(CLSID_CFormat7z,
    0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);

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
  STDMETHOD(SetOpenPassword)(LPCWSTR password);
  STDMETHOD(SetExtractPassword)(LPCWSTR password);
  STDMETHOD(Open)(LPCWSTR packpath);
  STDMETHOD(OpenInsideFile)(LPCWSTR file, ULONGLONG pack_pos, ULONGLONG pack_size);
  STDMETHOD(OpenMem)(LPCVOID data, ULONGLONG datalen);
  STDMETHOD(Extract)(LPCWSTR targetpath, IU7zExtractEvent* callback);

private:

  bool TryLoadDll(const std::wstring& dir);

  ult::Library lib_;

  ComPtr<IInArchive> archive_;

  InFileStream* in_stream_spec_;
  OpenCallback* open_callback_spec_;
  ExtractCallback* extract_callback_spec_;
  ComPtr<IInStream> in_stream_;
  ComPtr<IArchiveOpenCallback> open_callback_;
  ComPtr<IArchiveExtractCallback> extract_callback_;

  static LPCWSTR kDllNameArr_[];
  static LPCWSTR kSelfDllName_;
};