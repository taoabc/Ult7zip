#include "stdafx.h"
#include "FileStream.h"

static inline HRESULT ConvertBoolToHRESULT(bool result) {
  if (result) {
    return S_OK;
  }
  DWORD lasterror = ::GetLastError();
  if (lasterror == 0) {
    return E_FAIL;
  }
  return HRESULT_FROM_WIN32(lasterror);
}

InFileStream::InFileStream(void) {
}

InFileStream::~InFileStream(void) {
}

bool InFileStream::Open(const std::wstring& filename) {
  if (!file_.Open(filename)) {
    return false;
  }
  stream_type_ = kStreamTypeFile;
  return true;
}

bool InFileStream::Open(LPCVOID data, ULONGLONG len) {
  if (IsNull(data) || len <= 0) {
    return false;
  }
  data_ = data;
  data_len_ = len;
  data_cursor_ = 0;
  stream_type_ = kStreamTypeMemory;
  return true;
}

bool InFileStream::Open(const std::wstring& filename, ULONGLONG pack_pos, ULONGLONG pack_size) {
  if (!file_.Open(filename)) {
    return false;
  }
  ULONGLONG new_position;
  file_.Seek(pack_pos, &new_position);
  data_pos_ = pack_pos;
  data_len_ = pack_size;
  stream_type_ = kStreamTypeInsideFile;
  return true;
}

  //IUnknown
STDMETHODIMP_(ULONG) InFileStream::AddRef(void) {
  return ++ref_count_;
}
  
STDMETHODIMP_(ULONG) InFileStream::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP InFileStream::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(static_cast<IInStream*>(this));
  } else if (riid == IID_IInStream) {
    *ppobj = static_cast<IInStream*>(this);
  } else if (riid == IID_IStreamGetSize) {
    *ppobj = static_cast<IStreamGetSize*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}

  //IInStream
STDMETHODIMP InFileStream::Read(void *data, UInt32 size, UInt32 *processed_size) {
  if (stream_type_ == kStreamTypeFile) {
    DWORD real_processed_size;
    bool result = file_.ReadPart(data, size, &real_processed_size);
    if (!IsNull(processed_size)) {
      *processed_size = real_processed_size;
    }
    return ConvertBoolToHRESULT(result);
  } else if (stream_type_ == kStreamTypeMemory) {
    if (size > data_len_ - data_cursor_) {
      return E_FAIL;
    }
    memcpy(data, (char*)data_ + data_cursor_, size);
    data_cursor_ += size;
    if (processed_size != NULL) {
      *processed_size = size;
    }
    return S_OK;
  } else if (stream_type_ == kStreamTypeInsideFile) {
    DWORD real_processed_size;
    bool result = file_.ReadPart(data, size, &real_processed_size);
    if (!IsNull(processed_size)) {
      *processed_size = real_processed_size;
    }
    return ConvertBoolToHRESULT(result);
  }
  return E_FAIL;
}
  
STDMETHODIMP InFileStream::Seek(Int64 offset, UInt32 seek_origin, UInt64 *new_position) {
  if (seek_origin > 2) {
    return STG_E_INVALIDFUNCTION;
  }
  if (stream_type_ == kStreamTypeFile) {
    unsigned __int64 real_new_position;
    bool result = file_.Seek(offset, &real_new_position, seek_origin);
    if (!IsNull(new_position)) {
      *new_position = real_new_position;
    }
    return ConvertBoolToHRESULT(result);
  } else if (stream_type_ == kStreamTypeMemory) {
    if (seek_origin == FILE_BEGIN) {
      if ((UInt64)offset > data_len_ || offset < 0) {
        return E_FAIL;
      }
      data_cursor_ = offset;
    } else if (seek_origin == FILE_END) {
      data_cursor_ = data_len_ + offset;
    } else if (seek_origin == FILE_CURRENT) {
      UInt64 pos = data_cursor_ + offset;
      if (pos > data_len_ || pos < 0) {
        return E_FAIL;
      }
      data_cursor_ = pos;
    }
    if (!IsNull(new_position)) {
      *new_position = data_cursor_;
    }
    return S_OK;
  } else if (stream_type_ == kStreamTypeInsideFile) {
    unsigned __int64 real_new_position;
    Int64 real_offset;
    bool result;
    if (seek_origin == FILE_BEGIN) {
      real_offset = data_pos_ + offset;
    } else if (seek_origin == FILE_END) {
      real_offset = offset - (file_.GetSize() - (data_pos_ + data_len_));
    } else if (seek_origin == FILE_CURRENT) {
      real_offset = offset;
    }
    result = file_.Seek(real_offset, &real_new_position, seek_origin);
    real_new_position -= data_pos_;
    if (!IsNull(new_position)) {
      *new_position = real_new_position;
    }
    return ConvertBoolToHRESULT(result);
  }
  return E_FAIL;
}

  //IStreamGetSize
STDMETHODIMP InFileStream::GetSize(UInt64 *size) {
  if (stream_type_ == kStreamTypeFile) {
    *size = file_.GetSize();
    return S_OK;
  } else if (stream_type_ == kStreamTypeMemory) {
    *size = data_len_;
    return S_OK;
  } else if (stream_type_ == kStreamTypeInsideFile) {
    *size = data_len_;
    return S_OK;
  }
  return E_FAIL;
}

OutFileStream::OutFileStream(void) {
}

OutFileStream::~OutFileStream(void) {
}

bool OutFileStream::Create(const std::wstring& filename, bool create_always) {
  return file_.Create(filename, create_always);
}

bool OutFileStream::SetMTime(const FILETIME* mtime) {
  return file_.SetFileTime(NULL, NULL, mtime);
}
 
HRESULT OutFileStream::Close(void) {
  return ConvertBoolToHRESULT(file_.Close());
}

  //IUnknown
STDMETHODIMP_(ULONG) OutFileStream::AddRef(void) {
  return ++ref_count_;
}
  
STDMETHODIMP_(ULONG) OutFileStream::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP OutFileStream::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(static_cast<IOutStream*>(this));
  } else if (riid == IID_IOutStream) {
    *ppobj = static_cast<IOutStream*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}

  //IOutStream
STDMETHODIMP OutFileStream::Write(const void *data, UInt32 size, UInt32 *processed_size) {
  DWORD real_processed_size;
  bool result = file_.WritePart(data, size, &real_processed_size);
  if (!IsNull(processed_size)) {
    *processed_size = real_processed_size;
  }
  return ConvertBoolToHRESULT(result);
}

STDMETHODIMP OutFileStream::Seek(Int64 offset, UInt32 seek_origin, UInt64 *new_position) {
  if (seek_origin > 2) {
    return STG_E_INVALIDFUNCTION;
  }
  unsigned __int64 real_new_position;
  bool result = file_.Seek(offset, &real_new_position, seek_origin);
  if (!IsNull(new_position)) {
    *new_position = real_new_position;
  }
  return ConvertBoolToHRESULT(result);
}

STDMETHODIMP OutFileStream::SetSize(UInt64 new_size) {
  unsigned __int64 current_pos;
  if (!file_.Seek(0, &current_pos, FILE_CURRENT)) {
    return E_FAIL;
  }
  bool result = file_.SetSize(new_size);
  unsigned __int64 current_pos2;
  result = result && file_.Seek(current_pos, &current_pos2);
  return result ? S_OK : E_FAIL;
}