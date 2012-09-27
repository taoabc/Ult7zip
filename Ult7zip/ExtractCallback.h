#ifndef U7Z_EXTRACTCALLBACK_H_
#define U7Z_EXTRACTCALLBACK_H_

#include "Ult7zip.h"
#include "ComUtility.h"
#include "FileStream.h"

#include "ult/file-dir.h"

#include <initguid.h>
#include "7z/cpp/7zip/archive/IArchive.h"
#include "7z/cpp/7zip/IPassword.h"

#include <string>

class ExtractCallback :
    public IArchiveExtractCallback,
    public ICryptoGetTextPassword,
    public UnknownImp {

public:

  ExtractCallback(void);

  void Init(IInArchive* archive_handler, const std::wstring& directory_path, IU7zExtractEvent* callback);
  void SetPassword(const std::wstring& password);

  //IUnknown
  STDMETHOD_(ULONG, AddRef)(void);
  STDMETHOD_(ULONG, Release)(void);
  STDMETHOD(QueryInterface)(REFIID riid, void** ppobj);

  //IProgress
  STDMETHOD(SetTotal)(UInt64 total);
  STDMETHOD(SetCompleted)(const UInt64 *completed);

  //IArchiveExtractCallback
  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **out_stream,  Int32 ask_extract_mode);
  STDMETHOD(PrepareOperation)(Int32 ask_extract_mode);
  STDMETHOD(SetOperationResult)(Int32 operation_result);

  //ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

private:

  ComPtr<IInArchive> archive_handler_;
  ComPtr<ISequentialOutStream> out_filestream_;
  OutFileStream* out_filestream_spec_;

  IU7zExtractEvent* callback_;

  bool extract_mode_;
  
  std::wstring directory_path_; // Output directory
  std::wstring file_path_;      //name inside archive
  std::wstring disk_file_path_; // full path to file on disk

  struct ProcessedFileInfo {
    FILETIME mtime;
    DWORD attrib;
    bool isdir;
    bool attrib_defined;
    bool mtime_defined;
  } processed_fileinfo_;

  bool password_is_defined_;
  std::wstring password_;

  static LPCWSTR kEmptyFileAlias_;
};

#endif