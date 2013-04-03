// Ult7zip.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "Ult7zip.h"
#include "Unzip.h"

STDMETHODIMP U7zCreateInstance(REFIID riid, void** ppobj) {
  IUnknown* punkn = NULL;
  HRESULT hr;
  if (riid == IID_IU7zUnzip) {
    punkn = new Unzip;
    punkn->AddRef();
    hr = punkn->QueryInterface(IID_IU7zUnzip, ppobj);
    punkn->Release();
  } else if (riid == IID_IU7zZip) {
    hr = E_NOTIMPL;
  } else {
    return E_NOINTERFACE;
  }
  return hr;
}