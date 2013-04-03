/*
** this dll relys one of these dll: 7z.dll, 7za.dll, 7zxa.dll
** author
**   taoabc@gmail.com
*/
#ifndef U7Z_ULT7ZIP_H_
#define U7Z_ULT7ZIP_H_

#include <objbase.h>
#include <InitGuid.h>

// {5B31522B-FB8F-47AD-8EC8-081A2CAB46D4}
DEFINE_GUID(IID_IU7zFactory, 
  0x5b31522b, 0xfb8f, 0x47ad, 0x8e, 0xc8, 0x8, 0x1a, 0x2c, 0xab, 0x46, 0xd4);
// {9C6A2681-48AD-47CB-A841-1857A49777B2}
DEFINE_GUID(IID_IU7zUnzip, 
  0x9c6a2681, 0x48ad, 0x47cb, 0xa8, 0x41, 0x18, 0x57, 0xa4, 0x97, 0x77, 0xb2);
// {1F774AC9-5DE4-4B02-898A-52DEC3A3DFA2}
DEFINE_GUID(IID_IU7zZip, 
  0x1f774ac9, 0x5de4, 0x4b02, 0x89, 0x8a, 0x52, 0xde, 0xc3, 0xa3, 0xdf, 0xa2);
// {14A15AB5-0BB9-4731-BB06-E4498D10C504}
DEFINE_GUID(IID_IU7zUnzipEvent, 
  0x14a15ab5, 0xbb9, 0x4731, 0xbb, 0x6, 0xe4, 0x49, 0x8d, 0x10, 0xc5, 0x4);
// {BA09E117-C7A3-45D3-AC93-D30FEE91A40C}
DEFINE_GUID(IID_IU7zZipEvent, 
  0xba09e117, 0xc7a3, 0x45d3, 0xac, 0x93, 0xd3, 0xf, 0xee, 0x91, 0xa4, 0xc);

#undef  INTERFACE
#define INTERFACE IU7zUnzipEvent
DECLARE_INTERFACE_(INTERFACE, IUnknown) {
  STDMETHOD (QueryInterface)    (THIS_ REFIID riid, void** ppobj) PURE;
  STDMETHOD_(ULONG, AddRef)		  (THIS) PURE;
  STDMETHOD_(ULONG, Release)		(THIS) PURE;
  STDMETHOD (SetTotal)          (ULONGLONG total) PURE;
  STDMETHOD (SetCompleted)      (ULONGLONG completed) PURE;
  STDMETHOD (SetPath)           (LPCWSTR path) PURE;
};

#undef  INTERFACE
#define INTERFACE IU7zZipEvent
DECLARE_INTERFACE_(INTERFACE, IUnknown) {
  STDMETHOD (QueryInterface)    (THIS_ REFIID riid, void** ppobj) PURE;
  STDMETHOD_(ULONG, AddRef)		  (THIS) PURE;
  STDMETHOD_(ULONG, Release)		(THIS) PURE;
};

#undef  INTERFACE
#define INTERFACE IU7zUnzip
DECLARE_INTERFACE_(INTERFACE, IUnknown) {
  STDMETHOD (QueryInterface)    (THIS_ REFIID riid, void** ppobj) PURE;
  STDMETHOD_(ULONG, AddRef)		  (THIS) PURE;
  STDMETHOD_(ULONG, Release)		(THIS) PURE;
  STDMETHOD (Init)              (LPCWSTR xapath) PURE;
  STDMETHOD (SetOpenPassword)   (LPCWSTR password) PURE;
  STDMETHOD (SetExtractPassword)(LPCWSTR password) PURE;
  STDMETHOD (Open)              (LPCWSTR packpath) PURE;
  STDMETHOD (OpenInsideFile)    (LPCWSTR file, ULONGLONG pack_pos, ULONGLONG pack_size) PURE;
  STDMETHOD (OpenMem)           (LPCVOID data, ULONGLONG datalen) PURE;
  STDMETHOD (Extract)           (LPCWSTR targetpath, IU7zUnzipEvent* callback) PURE;
};

#undef  INTERFACE
#define INTERFACE IU7zZip
DECLARE_INTERFACE_(INTERFACE, IUnknown) {
  STDMETHOD (QueryInterface)    (THIS_ REFIID riid, void** ppobj) PURE;
  STDMETHOD_(ULONG, AddRef)		  (THIS) PURE;
  STDMETHOD_(ULONG, Release)		(THIS) PURE;
};

#undef  INTERFACE
#define INTERFACE IU7zFactory
DECLARE_INTERFACE_(INTERFACE, IUnknown) {
  STDMETHOD (QueryInterface)    (THIS_ REFIID riid, void** ppobj) PURE;
  STDMETHOD_(ULONG, AddRef)		  (THIS) PURE;
  STDMETHOD_(ULONG, Release)		(THIS) PURE;
  STDMETHOD (CreateUnzipObject) (IU7zUnzip** ppobj) PURE;
  STDMETHOD (CreateZipObject)   (IU7zZip** ppobj) PURE;
};

STDAPI U7zCreateFactory(IU7zFactory** ppobj);

#endif