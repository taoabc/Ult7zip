// Ult7zip.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "Ult7zip.h"
#include "Factory.h"

STDMETHODIMP U7zCreateFactory(IU7zFactory** ppobj) {
  Factory* p = new Factory;
  return p->QueryInterface(IID_IU7zFactory, (void**)ppobj);
}