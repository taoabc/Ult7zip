#ifndef U7z_ULT7ZIP_H_
#define U7z_ULT7ZIP_H_

#include <basetyps.h>
#include <Unknwn.h>

interface IU7zFactory : IUnknown {
};

STDAPI U7zCreateFactory(IU7zFactory** ppobj);

#endif