#include "stdafx.h"
#include "Unzip.h"


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
  return S_OK;
}

STDMETHODIMP Unzip::Open(LPCWSTR packpath) {
  return S_OK;
}

STDMETHODIMP Unzip::Open(LPCVOID data, ULONGLONG datalen) {
  return S_OK;
}

STDMETHODIMP Unzip::Extractor(LPCWSTR targetpath, IU7zExtractEvent* callback) {
  return S_OK;
}