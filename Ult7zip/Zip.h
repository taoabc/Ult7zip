#pragma once

#include "Ult7zip.h"
#include "ComUtility.h"

class Zip :
    public IU7zZip,
    public UnknownImp {

public:

  Zip(void);
  ~Zip(void);

  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID iid, void** ppobj);

};