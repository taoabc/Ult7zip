#ifndef U7Z_FILESTREAM_H_
#define U7Z_FILESTREAM_H_

#include "ComUtility.h"
#include "ult/file-io.h"
#include "7z\cpp\7zip\archive\IArchive.h"

#include <basetyps.h>

#include <string>

class InFileStream :
    public IInStream,
    public IStreamGetSize,
    public UnknownImp {

public:

  InFileStream(void);
  ~InFileStream(void);

  bool Open(const std::wstring& filename);
  bool Open(const std::wstring& filename, ULONGLONG pack_pos, ULONGLONG pack_size);
  bool Open(LPCVOID data, ULONGLONG len);

  //IUnknown
  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID riid, void** ppobj);

  //IInStream
  STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processed_size);
  STDMETHOD(Seek)(Int64 offset, UInt32 seek_origin, UInt64 *new_position);

  //IStreamGetSize
  STDMETHOD(GetSize)(UInt64 *size);

private:

  enum {
    kStreamTypeFile,
    kStreamTypeMemory,
    kStreamTypeInsideFile,
  } stream_type_;

  ult::File file_;
  LPCVOID data_;
  ULONGLONG data_cursor_;
  ULONGLONG data_len_;
  ULONGLONG data_pos_;
};

class OutFileStream :
    public IOutStream, 
    public UnknownImp {

public:

  OutFileStream(void);
  ~OutFileStream(void);

  bool Create(const std::wstring& filename, bool create_always);
  bool SetMTime(const FILETIME* mtime);
  HRESULT Close(void);

  //IUnknown
  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID riid, void** ppobj);

  //IOutStream
  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processed_size); 
  STDMETHOD(Seek)(Int64 offset, UInt32 seek_origin, UInt64 *new_position);
  STDMETHOD(SetSize)(UInt64 new_size);

private:

  ult::File file_;

};

#endif