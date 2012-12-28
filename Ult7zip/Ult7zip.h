/*
** this dll relys one of these dll: 7z.dll, 7za.dll, 7zxa.dll
** author
**   taoabc@gmail.com
*/
#ifndef U7Z_ULT7ZIP_H_
#define U7Z_ULT7ZIP_H_

#include <basetyps.h>
#include <Unknwn.h>

// {7FE4C68C-C25F-46D8-B93D-0EEB811C47F1}
static const GUID IID_IU7zFactory = 
{ 0x7fe4c68c, 0xc25f, 0x46d8, { 0xb9, 0x3d, 0xe, 0xeb, 0x81, 0x1c, 0x47, 0xf1 } };
// {04AC8C3C-BAC5-46C4-8216-25220D961B53}
static const GUID IID_IU7zUnzip = 
{ 0x4ac8c3c, 0xbac5, 0x46c4, { 0x82, 0x16, 0x25, 0x22, 0xd, 0x96, 0x1b, 0x53 } };
// {CB93CBB0-99D1-47FF-9238-FC97C0209EB4}
static const GUID IID_IU7zZip = 
{ 0xcb93cbb0, 0x99d1, 0x47ff, { 0x92, 0x38, 0xfc, 0x97, 0xc0, 0x20, 0x9e, 0xb4 } };

interface IU7zExtractEvent {
  STDMETHOD(SetTotal)(ULONGLONG total) PURE;
  STDMETHOD(SetCompleted)(ULONGLONG completed) PURE;
};

interface IU7zZipEvent {
};

interface IU7zUnzip : IUnknown {
  STDMETHOD(Init)(LPCWSTR xapath) PURE;
  STDMETHOD(SetOpenPassword)(LPCWSTR password) PURE;
  STDMETHOD(SetExtractPassword)(LPCWSTR password) PURE;
  STDMETHOD(Open)(LPCWSTR packpath) PURE;
  STDMETHOD(OpenInsideFile)(LPCWSTR file, ULONGLONG pack_pos, ULONGLONG pack_size) PURE;
  STDMETHOD(OpenMem)(LPCVOID data, ULONGLONG datalen) PURE;
  STDMETHOD(Extract)(LPCWSTR targetpath, IU7zExtractEvent* callback) PURE;
};

interface IU7zZip : IUnknown {
};

interface IU7zFactory : IUnknown {
  STDMETHOD(CreateUnzipObject)(IU7zUnzip** ppobj) PURE;
  STDMETHOD(CreateZipObject)(IU7zZip** ppobj) PURE;
};

STDAPI U7zCreateFactory(IU7zFactory** ppobj);

#endif