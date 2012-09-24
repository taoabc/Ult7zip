#include "stdafx.h"
#include "Zip.h"


Zip::Zip(void) {
}


Zip::~Zip(void) {
}

STDMETHODIMP_(ULONG) Zip::AddRef(void) {
  return ++ref_count_;
}

STDMETHODIMP_(ULONG) Zip::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP Zip::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(this);
  } else if (riid == IID_IU7zZip) {
    *ppobj = static_cast<IU7zZip*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}