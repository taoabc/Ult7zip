#include "stdafx.h"
#include "Factory.h"

#include "Unzip.h"

Factory::Factory(void) {
}

Factory::~Factory(void) {
}

STDMETHODIMP_(ULONG) Factory::AddRef(void) {
  return ++ref_count_;
}

STDMETHODIMP_(ULONG) Factory::Release(void) {
  if (--ref_count_ == 0) {
    delete this;
    return 0;
  }
  return ref_count_;
}

STDMETHODIMP Factory::QueryInterface(REFIID riid, void** ppobj) {
  if (riid == IID_IUnknown) {
    *ppobj = static_cast<IUnknown*>(this);
  } else if (riid == IID_IU7zFactory) {
    *ppobj = static_cast<IU7zFactory*>(this);
  } else {
    *ppobj = NULL;
    return E_NOINTERFACE;
  }
  reinterpret_cast<IUnknown*>(*ppobj)->AddRef();
  return S_OK;
}

STDMETHODIMP Factory::CreateUnzipObject(IU7zUnzip** ppobj) {
  Unzip* p = new Unzip;
  return p->QueryInterface(IID_IU7zUnzip, (void**)ppobj);
}

STDMETHODIMP Factory::CreateZipObject(IU7zZip** ppobj) {
  return E_NOTIMPL;
}