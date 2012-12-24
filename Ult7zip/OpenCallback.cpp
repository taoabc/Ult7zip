#include "stdafx.h"
#include "OpenCallback.h"

OpenCallback::OpenCallback(void) :
    password_is_defined_(false) {
}

void OpenCallback::SetPassword(const std::wstring& password) {
  password_is_defined_ = true;
  password_ = password;
}

STDMETHODIMP_(ULONG) OpenCallback::AddRef(void) {
  return ++ref_count_;
}

STDMETHODIMP_(ULONG) OpenCallback::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP OpenCallback::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(static_cast<IArchiveOpenCallback*>(this));
  } else if (riid == IID_ICryptoGetTextPassword) {
    *ppobj = static_cast<ICryptoGetTextPassword*>(this);
  } else if (riid == IID_IArchiveOpenVolumeCallback) {
    *ppobj = static_cast<IArchiveOpenVolumeCallback*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}

STDMETHODIMP OpenCallback::SetTotal(const UInt64 *files, const UInt64 *bytes) {
  return S_OK;
}

STDMETHODIMP OpenCallback::SetCompleted(const UInt64 *files, const UInt64 *bytes) {
  return S_OK;
}

STDMETHODIMP OpenCallback::CryptoGetTextPassword(BSTR *password) {
  if (!password_is_defined_) {
    return E_ABORT;
  }
  return StringToBstr(password_.c_str(), password);
}

STDMETHODIMP OpenCallback::GetProperty( PROPID propID, PROPVARIANT *value ) {
  return S_OK;
}

STDMETHODIMP OpenCallback::GetStream( const wchar_t *name, IInStream **inStream ) {
  return S_OK;
}

STDMETHODIMP OpenCallback::SetSubArchiveName( const wchar_t *name ) {
  return S_OK;
}
