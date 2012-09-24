#ifndef U7Z_FACTORY_H_
#define U7Z_FACTORY_H_

#include "Ult7zip.h"
#include "ComUtility.h"

class Factory :
    public IU7zFactory,
    public UnknownImp {

public:

  Factory(void);
  ~Factory(void);

  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID riid, void** ppobj);

  STDMETHOD(CreateUnzipObject)(IU7zUnzip** ppobj);
  STDMETHOD(CreateZipObject)(IU7zZip** ppobj);

private:


};

#endif