#include "stdafx.h"
#include "OpenCallback.h"

OpenCallback::OpenCallback(void) :
    password_is_defined_(false) {
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