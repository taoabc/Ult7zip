// Ult7zip.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "Ult7zip.h"

STDMETHODIMP U7zCreateFactory(IU7zFactory** ppobj) {
  return S_OK;
}