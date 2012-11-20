#include "stdafx.h"
#include "Unzip.h"

#include <algorithm>

LPCWSTR Unzip::kDllNameArr_[] = {
  L"7z.dll",
  L"7za.dll",
  L"7zxa.dll",
};

LPCWSTR Unzip::kSelfDllName_ = L"Ult7zip.dll";

Unzip::Unzip(void) {
}

Unzip::~Unzip(void) {
}

STDMETHODIMP_(ULONG) Unzip::AddRef(void) {
  return ++ref_count_;
}

STDMETHODIMP_(ULONG) Unzip::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP Unzip::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(this);
  } else if (riid == IID_IU7zUnzip) {
    *ppobj = static_cast<IU7zUnzip*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}

STDMETHODIMP Unzip::Init(LPCWSTR xapath) {
  lib_.Free();
  //use default strategy
  if (IsNull(xapath) || wcslen(xapath) == 0) {
    std::wstring process_dir;
    ult::GetSelfModuleDirectory(&process_dir);
    
    bool load_result = TryLoadDll(process_dir);
    if (!load_result) {
      std::wstring self_dlldir;
      ult::GetNamedModuleDirectory(kSelfDllName_, &self_dlldir);
      load_result = TryLoadDll(self_dlldir);
    }
    if (!load_result) {
      return E_FAIL;
    }
  //use user strategy
  } else {
    lib_.Load(xapath);
    if (!lib_.IsLoaded()) {
      return E_FAIL;
    }
  }

  PCreateObject CreateObject = (PCreateObject)lib_.GetProc("CreateObject");
  if (IsNull(CreateObject)) {
    return E_FAIL;
  }

  if (CreateObject(&CLSID_CFormat7z, &IID_IInArchive, (void**)&archive_) != S_OK) {
    return E_FAIL;
  }
  
  in_stream_spec_ = new InFileStream;
  open_callback_spec_ = new OpenCallback;
  extract_callback_spec_ = new ExtractCallback;

  in_stream_ = in_stream_spec_;
  open_callback_ = open_callback_spec_;
  extract_callback_ = extract_callback_spec_;

  return S_OK;
}

STDMETHODIMP Unzip::SetOpenPassword(LPCWSTR password) {
  open_callback_spec_->SetPassword(password);
  return S_OK;
}

STDMETHODIMP Unzip::SetExtractPassword(LPCWSTR password) {
  extract_callback_spec_->SetPassword(password);
  return S_OK;
}

STDMETHODIMP Unzip::Open(LPCWSTR packpath) {
  if (!in_stream_spec_->Open(packpath)) {
    return E_FAIL;
  }
  if (FAILED(archive_->Open(in_stream_, NULL, open_callback_))) {
    return E_FAIL;
  }
  return S_OK;
}

STDMETHODIMP Unzip::Open(LPCVOID data, ULONGLONG datalen) {
  if (!in_stream_spec_->Open(data, datalen)) {
    return E_FAIL;
  }
  if (FAILED(archive_->Open(in_stream_, NULL, open_callback_))) {
    return E_FAIL;
  }
  return S_OK;
}

STDMETHODIMP Unzip::Extract(LPCWSTR targetpath, IU7zExtractEvent* callback) {
  if (IsNull(targetpath) || wcslen(targetpath) == 0) {
    return E_ABORT;
  }
  if (!ult::MakeSureFolderExist(targetpath)) {
    return E_FAIL;
  }
  extract_callback_spec_->Init(archive_, targetpath, callback);
  return archive_->Extract(NULL, (UInt32)(Int32)(-1), false, extract_callback_);
}

bool Unzip::TryLoadDll(const std::wstring& dir) {
  std::wstring full_dllpath;
  int count = sizeof (kDllNameArr_) / sizeof (kDllNameArr_[0]);
  for (int i = 0; i < count; ++i) {
    full_dllpath = dir;
    ult::AppendPath(&full_dllpath, kDllNameArr_[i]);
    lib_.Load(full_dllpath);
    if (lib_.IsLoaded()) {
      break;
    }
  }
  return lib_.IsLoaded();
}