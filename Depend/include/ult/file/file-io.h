/*
** ÎÄ¼þIOÀà
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_FILEIO_H_
#define ULT_FILE_FILEIO_H_

#include <string>
#include <windows.h>

namespace ult {

class File {

public:

  File(void) : hfile_(NULL) {

  }

  ~File(void) {
    if (hfile_ != NULL) {
      Close();
    }
  }

  HANDLE GetHandle(void) {
    return hfile_;
  }

  bool IsOpened(void) {
    return hfile_ != NULL;
  }

  void Attach(HANDLE hfile) {
    Close();
    hfile_ = hfile;
  }

  HANDLE Detach(void) {
    HANDLE f = hfile_;
    hfile_ = NULL;
    return f;
  }

  bool Close(void) {
    if (hfile_ == NULL) {
      return true;
    }
    if (!::CloseHandle(hfile_)) {
      return false;
    }
    hfile_ = NULL;
    return true;
  }

  bool Open(const std::wstring& filename, DWORD dwaccess = GENERIC_READ) {
    return OpenOperation(filename, dwaccess);
  }

  bool Create(const std::wstring& filename, bool create_always) {
    DWORD create_disposition;
    if (create_always) {
      create_disposition = CREATE_ALWAYS;
    } else {
      create_disposition = CREATE_NEW;
    }
    hfile_ = ::CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ, NULL, create_disposition, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile_ == INVALID_HANDLE_VALUE) {
      return false;
    }
    return true;
  }

  unsigned __int64 GetPosition(void) {
    unsigned __int64 cur_position;
    if (!Seek(0, &cur_position, FILE_CURRENT)) {
      return 0;
    }
    return cur_position;
  }

  unsigned __int64 GetSize(void) {
    DWORD size_high;
    DWORD size_low = ::GetFileSize(hfile_, &size_high);
    if (size_low == INVALID_FILE_SIZE && GetLastError != NO_ERROR) {
      return 0;
    }
    return (((unsigned __int64)size_high) << 32) + size_low;
  }

  bool SetEndOfFile(void) {
    return (::SetEndOfFile(hfile_) != 0);
  }

  bool SetSize(unsigned __int64 size) {
    unsigned __int64 new_position;
    if (!Seek(size, &new_position)) {
      return false;
    }
    if (new_position != size) {
      return false;
    }
    return SetEndOfFile();
  }

  bool SetFileTime(const FILETIME* creation_time,
                   const FILETIME* lastaccess_time,
                   const FILETIME* lastwrite_time) {
    return (::SetFileTime(hfile_, creation_time, lastaccess_time, lastwrite_time) != 0);
  }

  bool Seek(__int64 distance, __int64* new_position, DWORD move_method) {
    LARGE_INTEGER li;
    li.QuadPart = distance;
    li.LowPart = ::SetFilePointer(hfile_, li.LowPart, &li.HighPart, move_method);
    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
      *new_position = -1;
      return false;
    }
    *new_position = li.QuadPart;
    return true;
  }

  bool Seek(unsigned __int64 distance, unsigned __int64* new_position, DWORD move_method) {
    return Seek((__int64)distance, (__int64*)new_position, move_method);
  }

  bool Seek(unsigned __int64 position, unsigned __int64* new_position) {
    return Seek(position, new_position, FILE_BEGIN);
  }

  bool SeekToBegin(void) {
    unsigned __int64 new_position;
    return Seek(0, &new_position);
  }

  bool SeekToEnd(unsigned __int64* new_position) {
    return Seek(0, new_position, FILE_END);
  }

  bool Read(void* buffer, DWORD toread, DWORD* readed) {
    *readed = 0;
    DWORD t = toread;
    unsigned long readed_once = 0;
    do {
      bool ret = ReadPart(buffer, toread, &readed_once);
      *readed += readed_once;
      if (*readed > t) {
        int a = 0;
      }
      if (!ret) {
        return false;
      }
      if (readed_once == 0) {
        return true;
      }
      buffer = (void*)((unsigned char*)buffer + readed_once);
      toread -= readed_once;
    } while (toread > 0);
    return true;
  }

  bool Write(const void* buffer, DWORD towrite, DWORD* written) {
    *written = 0;
    unsigned long write_once = 0;
    do {
      bool ret = WritePart(buffer, towrite, &write_once);
      *written += write_once;
      if (!ret) {
        return false;
      }
      if (write_once == 0) {
        return true;
      }
      buffer = (const void*)((const unsigned char*)buffer + write_once);
      towrite -= write_once;
    } while (towrite > 0);
    return true;
  }

  bool ReadPart(void* buffer, DWORD toread, DWORD* readed) {
    if (toread > kChunkSizeMax_) {
      toread = kChunkSizeMax_;
    }
    DWORD readed_once = 0;
    bool ret = (::ReadFile(hfile_, buffer, toread, &readed_once, NULL) != 0);
    *readed = readed_once;
    return ret;
  }

  bool WritePart(const void* buffer, DWORD towrite, DWORD* written) {
    if (towrite > kChunkSizeMax_) {
      towrite = kChunkSizeMax_;
    }
    DWORD writed_once = 0;
    bool ret = (::WriteFile(hfile_, buffer, towrite, &writed_once, NULL) != 0);
    *written = writed_once;
    return ret;
  }

private:
  
  bool OpenOperation(const std::wstring& filename, DWORD dwaccess) {
    hfile_ = ::CreateFile(filename.c_str(), dwaccess, FILE_SHARE_READ, NULL,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile_ == INVALID_HANDLE_VALUE) {
      return false;
    }
    return true;
  }

  HANDLE hfile_;

  static const unsigned int kChunkSizeMax_ = 1 << 25;
}; //class File
} //namespace ult

#endif