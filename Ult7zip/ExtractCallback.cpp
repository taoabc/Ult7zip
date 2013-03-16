#include "stdafx.h"
#include "ExtractCallback.h"

LPCWSTR ExtractCallback::kEmptyFileAlias_ = L"[Content]";

static HRESULT IsArchiveItemProp(IInArchive* archive, UInt32 index, PROPID propID, bool* result) {
  PROPVARIANT prop;
  memset(&prop, 0, sizeof(prop));
  RETURN_IF_FAILED(archive->GetProperty(index, propID, &prop));
  if (prop.vt == VT_BOOL)
    *result = (prop.boolVal != VARIANT_FALSE);
  else if (prop.vt == VT_EMPTY)
    *result = false;
  else
    return E_FAIL;
  return S_OK;
}

static HRESULT IsArchiveItemFolder(IInArchive* archive, UInt32 index, bool* result) {
  return IsArchiveItemProp(archive, index, kpidIsDir, result);
}

static UInt64 ConvertPropVariantToUInt64(const PROPVARIANT &prop) {
  switch (prop.vt) {
  case VT_UI1: return prop.bVal;
  case VT_UI2: return prop.uiVal;
  case VT_UI4: return prop.ulVal;
  case VT_UI8: return (UInt64)prop.uhVal.QuadPart;
  default: throw 151199;
  }
}

ExtractCallback::ExtractCallback(void) :
    password_is_defined_(false) {
}

void ExtractCallback::Init(IInArchive* archive_handler, const std::wstring& directory_path, IU7zUnzipEvent* callback) {
  archive_handler_ = archive_handler;
  directory_path_ = directory_path;
  callback_ = callback;
  ult::AddPathBackslash(&directory_path_);
}

void ExtractCallback::SetPassword(const std::wstring& password) {
  password_is_defined_ = true;
  password_ = password;
}

STDMETHODIMP_(ULONG) ExtractCallback::AddRef(void) {
  return ++ref_count_;
}

STDMETHODIMP_(ULONG) ExtractCallback::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP ExtractCallback::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(static_cast<ICryptoGetTextPassword*>(this));
  } else if (riid == IID_ICryptoGetTextPassword) {
    *ppobj = static_cast<ICryptoGetTextPassword*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}

STDMETHODIMP ExtractCallback::SetTotal(UInt64 total) {
  if (!IsNull(callback_)) {
    RETURN_IF_FAILED(callback_->SetTotal(total));
  }
  return S_OK;
}
  
STDMETHODIMP ExtractCallback::SetCompleted(const UInt64 *completed) {
  if (!IsNull(callback_)) {
    RETURN_IF_FAILED(callback_->SetCompleted(*completed));
  }
  return S_OK;
}

  //IArchiveExtractCallback
STDMETHODIMP ExtractCallback::GetStream(UInt32 index, ISequentialOutStream **out_stream,  Int32 ask_extract_mode) {
  *out_stream = NULL;
  out_filestream_.Release();
  PROPVARIANT prop;

  {
    //Get Name
    memset(&prop, 0, sizeof (prop));
    RETURN_IF_FAILED(archive_handler_->GetProperty(index, kpidPath, &prop));

    std::wstring fullpath;
    if (prop.vt == VT_EMPTY) {
      fullpath = kEmptyFileAlias_;
    } else {
      if (prop.vt != VT_BSTR) {
        return E_FAIL;
      }
      fullpath = prop.bstrVal;
    }
    file_path_ = fullpath;
    if (!IsNull(callback_)) {
      RETURN_IF_FAILED(callback_->SetPath(fullpath.c_str()));
    }
  }

  if (ask_extract_mode != NArchive::NExtract::NAskMode::kExtract) {
    return S_OK;
  }

  {
    //Get Arrtibute
    memset(&prop, 0, sizeof (prop));
    RETURN_IF_FAILED(archive_handler_->GetProperty(index, kpidAttrib, &prop));
    if (prop.vt == VT_EMPTY) {
      processed_fileinfo_.attrib = 0;
      processed_fileinfo_.attrib_defined = false;
    } else {
      if (prop.vt != VT_UI4)
        return E_FAIL;
      processed_fileinfo_.attrib = prop.ulVal;
      processed_fileinfo_.attrib_defined = true;
    }
  }

  {
    //Get modified time
    memset(&prop, 0, sizeof (prop));
    RETURN_IF_FAILED(archive_handler_->GetProperty(index, kpidMTime, &prop));
    processed_fileinfo_.mtime_defined = false;
    switch (prop.vt) {
    case VT_EMPTY:
      break;
    case VT_FILETIME:
      processed_fileinfo_.mtime = prop.filetime;
      processed_fileinfo_.mtime_defined = true;
      break;
    default:
      return E_FAIL;
    }
  }

  {
    //Get size
    memset(&prop, 0, sizeof (prop));
    RETURN_IF_FAILED(archive_handler_->GetProperty(index, kpidSize, &prop));
    UInt64 new_filesize;
    if (prop.vt != VT_EMPTY) {
      new_filesize = ConvertPropVariantToUInt64(prop);
    }
  }

  {
    //Create folders for file
    int pos = file_path_.rfind(L"\\");
    if (pos != std::wstring::npos) {
      ult::MakeSureFolderExist(directory_path_ + file_path_.substr(0, pos));
    }
  }

  RETURN_IF_FAILED(IsArchiveItemFolder(archive_handler_, index, &processed_fileinfo_.isdir));

  std::wstring full_processed_path = directory_path_ + file_path_;
  disk_file_path_ = full_processed_path;
  if (processed_fileinfo_.isdir) {
    ult::MakeSureFolderExist(full_processed_path);
  } else {
    if (ult::IsPathFileExist(full_processed_path)) {
      if (!ult::DeleteFileAlways(full_processed_path)) {
        return E_ABORT;
      }
    }
    out_filestream_spec_ = new OutFileStream;
    ComPtr<ISequentialOutStream> out_stream_loc(out_filestream_spec_);
    if (!out_filestream_spec_->Create(full_processed_path, true)) {
      return E_ABORT;
    }
    out_filestream_ = out_stream_loc;
    *out_stream = out_stream_loc.Detach();
  }

  return S_OK;
}
  
STDMETHODIMP ExtractCallback::PrepareOperation(Int32 ask_extract_mode) {
  extract_mode_ = false;
  switch (ask_extract_mode) {
  case NArchive::NExtract::NAskMode::kExtract:
    extract_mode_ = true;
    break;
  }
  return S_OK;
}
  
STDMETHODIMP ExtractCallback::SetOperationResult(Int32 operation_result) {
  if (!IsNull(out_filestream_)) {
    if (processed_fileinfo_.mtime_defined) {
      //set modified time back, otherwise there were be current time
      out_filestream_spec_->SetMTime(&processed_fileinfo_.mtime);
    }
    RETURN_IF_FAILED(out_filestream_spec_->Close());
  }
  out_filestream_.Release();
  if (extract_mode_ && processed_fileinfo_.attrib_defined) {
    ult::SetFileAttributes(disk_file_path_, processed_fileinfo_.attrib);
  }
  return S_OK;
}

  //ICryptoGetTextPassword
STDMETHODIMP ExtractCallback::CryptoGetTextPassword(BSTR *password) {
  if (!password_is_defined_) {
    return E_ABORT;
  }
  return StringToBstr(password_.c_str(), password);
}