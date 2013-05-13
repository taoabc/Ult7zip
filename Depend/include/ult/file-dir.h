/*
** 提供文件及磁盘操作
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_FILEDIR_H_
#define ULT_FILE_FILEDIR_H_

#include <Windows.h>
#include <Shellapi.h>
#include <shlobj.h>
#include <string>
#include <vector>

namespace ult {
  
namespace detail {

struct IsPathFileExist {
  bool operator()(const std::wstring& path) {
    if (-1 != _waccess(path.c_str(), 0)) {
      return true;
    } else {
      return false;
    }
  }
};

struct ToPurenameAndExtension {
  void operator()(
      const std::wstring& fullname,
      std::wstring* purename,
      std::wstring* extension) {
    size_t pos = fullname.rfind(L".");
    if (pos == std::wstring::npos) {
      purename->assign(fullname);
      extension->clear();
    } else {
      purename->assign(fullname.substr(0, pos));
      extension->assign(fullname.substr(pos+1));
    }
  }
};

struct ToUpperpathAndFilename {
  void operator()(
    const std::wstring& fullpath,
    std::wstring* pathprefix,
    std::wstring* filename,
    const std::wstring& pathseparator) {
      size_t separator_len = pathseparator.length();
      size_t pos = fullpath.rfind(pathseparator);
      if (pos == std::wstring::npos) {
        pathprefix->clear();
        filename->assign(fullpath);
      } else {
        pathprefix->assign(fullpath.substr(0, pos+separator_len));
        filename->assign(fullpath.substr(pos+separator_len));
      }
  }
};

struct AddPathBackslash {
  void operator()(std::wstring* dirpath) {
    if (dirpath->empty()) {
      return;
    }
    if (dirpath->at(dirpath->length()-1) != L'\\') {
      dirpath->push_back(L'\\');
    }
  }
};

struct RemovePathBackslash {
  void operator()(std::wstring* dirpath) {
    if (dirpath->empty()) {
      return;
    }
    if (dirpath->at(dirpath->length()-1) == L'\\') {
      dirpath->pop_back();
    }
  }
};

struct GetDriveInType {
  std::vector<std::wstring> operator()(UINT type) {
    DWORD buf_len = ::GetLogicalDriveStrings(0, NULL);
    wchar_t* buf = new wchar_t [buf_len];

    std::vector<std::wstring> vec;
    if (0 != ::GetLogicalDriveStrings(buf_len, buf)) {
      wchar_t* drive_tmp = buf;
      DWORD i = 0;
      while (i <= buf_len) {
        if (type == ::GetDriveType(drive_tmp)) {
          vec.push_back(drive_tmp);
        }
        i += static_cast<DWORD>(wcslen(drive_tmp)) + 1;
        drive_tmp = buf + i;
      }
    }
    delete[] buf;
    return vec;
  }
};

struct GetRootDirectory {
  std::wstring operator()(const std::wstring& path) {
    size_t pos = path.find(L'\\');
    if (pos == 0) {
      pos = path.find(L'\\', 2); 
    }
    if (pos != std::wstring::npos) {
      return path.substr(0, pos+1);
    }
    return L"";
  }
};

struct GetDiskFreeSpace {
  ULONGLONG operator()(const std::wstring& directory) {
    const std::wstring root_dir = GetRootDirectory()(directory);
    ULARGE_INTEGER freespace;
    if (FALSE == ::GetDiskFreeSpaceEx(root_dir.c_str(), &freespace, NULL, NULL)) {
      return static_cast<ULONGLONG>(-1);
    }
    return freespace.QuadPart;
  }
};

struct GetMaxFreeSpaceDrive {
  std::wstring operator()(ULONGLONG* freesize) {
    std::vector<std::wstring> vec = GetDriveInType()(DRIVE_FIXED);
    ULONGLONG maxfree = 0;
    std::wstring drive;
    for (std::vector<std::wstring>::const_iterator iter = vec.begin();
      iter != vec.end(); ++iter) {
        ULONGLONG t = GetDiskFreeSpace()(*iter);
        if (t > maxfree) {
          maxfree = t;
          drive = *iter;
        }
    }
    if (freesize != NULL) {
      *freesize = maxfree;
    }
    return drive;
}
};

struct  GetUpperDirectory {
  std::wstring operator()(const std::wstring& path) {
    std::wstring tmp(path);
    RemovePathBackslash()(&tmp);
    size_t pos = tmp.rfind(L'\\');
    if (pos == std::wstring::npos) {
      return L"";
    }
    return tmp.substr(0, pos+1);
  }
};

struct IsPathDirectory {
  bool operator()(const std::wstring& path) {
    DWORD attr = ::GetFileAttributes(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
      return false;
    }
    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
  }
};

struct CreateDirectories {
  bool operator()(const std::wstring path) {
    if (path.empty()) {
      return false;
    }
    if (IsPathDirectory()(path)) {
      return true;
    } else{
      if (IsPathFileExist()(path)) {
        return false;
      }
    }
    if (!operator()(GetUpperDirectory()(path))) {
      return false;
    }
    return TRUE == ::CreateDirectory(path.c_str(), NULL);
  }
};

struct RecursiveRemoveDirectory {
  bool operator()(const std::wstring& directory) {
    //pFrom and pTo need double-null terminate
    wchar_t tmp[MAX_PATH+1];
    wcscpy_s(tmp, directory.c_str());
    tmp[directory.length()] = L'\0';
    tmp[directory.length()+1] = L'\0';
    SHFILEOPSTRUCT fileop;
    fileop.hwnd = NULL;
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = tmp;
    fileop.pTo = NULL;
    fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    fileop.lpszProgressTitle = NULL;

    return (::SHFileOperation(&fileop) == 0);
  }
};

struct GetFolderPath {
  std::wstring operator()(int csidl, HANDLE htoken, DWORD flags) {
    wchar_t buffer[MAX_PATH];
    HRESULT hr = ::SHGetFolderPath(NULL, csidl, htoken, flags, buffer);
    if (FAILED(hr)) {
      return L"";
    }
    return buffer;
  }
};

struct GetFileSize {
  ULONGLONG operator()(const std::wstring& file) {
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (FALSE == ::GetFileAttributesEx(file.c_str(), ::GetFileExInfoStandard, &fad)) {
      return static_cast<ULONGLONG>(-1);
    }
    if ((fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
      return static_cast<ULONGLONG>(-1);
    }
    return (static_cast<ULONGLONG>(fad.nFileSizeHigh)
        << (sizeof(fad.nFileSizeLow)*8)) + fad.nFileSizeLow;
  }
};

} /* namespace detail */

inline void ToPurenameAndExtension(
    const std::wstring& fullname,
    std::wstring* purename,
    std::wstring* extension) {
  detail::ToPurenameAndExtension()(fullname, purename, extension);
}

inline void ToUpperpathAndFilename(
    const std::wstring& fullpath,
    std::wstring* pathprefix,
    std::wstring* filename,
    const std::wstring& pathseparator = L"\\") {
  detail::ToUpperpathAndFilename()(fullpath, pathprefix, filename, pathseparator);
}

inline ULONGLONG GetFileSize(const std::wstring& file) {
  return detail::GetFileSize()(file);
}

inline void AddPathBackslash(std::wstring* dirpath) {
  detail::AddPathBackslash()(dirpath);
}

inline void RemovePathBackslash(std::wstring* dirpath) {
  detail::RemovePathBackslash()(dirpath);
}

inline std::wstring AppendPath(const std::wstring& path,
                       const std::wstring& more) {
  std::wstring t(path);
  AddPathBackslash(&t);
  if (more.at(0) == L'\\') {
    t += (more.c_str()+1);
  } else {
    t += more;
  }
  return t;
}

inline ULONGLONG GetDiskFreeSpace(const std::wstring& directory) {
  return detail::GetDiskFreeSpace()(directory);
}

inline std::vector<std::wstring> GetDriveInType(UINT type = DRIVE_FIXED) {
  return detail::GetDriveInType()(type);
}

inline std::wstring GetMaxFreeSpaceDrive(ULONGLONG* freesize = NULL) {
  return detail::GetMaxFreeSpaceDrive()(freesize);
}

inline std::wstring GetFolderPath(int csidl, HANDLE htoken = NULL, DWORD flags = SHGFP_TYPE_CURRENT) {
  return detail::GetFolderPath()(csidl, htoken, flags);
}

inline std::wstring GetProgramFilesDirectory(void) {
  return GetFolderPath(CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE);
}

inline std::wstring GetAppDataDirectory(void) {
  return GetFolderPath(CSIDL_APPDATA | CSIDL_FLAG_CREATE);
}

inline std::wstring GetSystemDirectory(void) {
  return GetFolderPath(CSIDL_SYSTEM);
}

inline bool IsPathFileExist(const std::wstring& path) {
  return detail::IsPathFileExist()(path);
}

inline std::wstring GetUpperDirectory(const std::wstring& path) {
  return detail::GetUpperDirectory()(path);
}

inline std::wstring GetRootDirectory(const std::wstring& path) {
  return detail::GetRootDirectory()(path);
}

inline std::wstring GetSelfModulePath(void) {
  wchar_t buf[MAX_PATH];
  ::GetModuleFileName(NULL, buf, MAX_PATH);
  return buf;
}

inline std::wstring GetSelfModuleDirectory(void) {
  return GetUpperDirectory(ult::GetSelfModulePath());
}

inline std::wstring GetNamedModuleDirectory(const std::wstring& module_name) {
  wchar_t buf[MAX_PATH];
  ::GetModuleFileName(GetModuleHandle(module_name.c_str()), buf, MAX_PATH);
  return GetUpperDirectory(buf);
}

inline bool CreateDirectories(const std::wstring& path) {
  return detail::CreateDirectories()(path);
}

inline bool DeleteFileAlways(const std::wstring& filename) {
  ::SetFileAttributes(filename.c_str(), 0);
  if (::DeleteFile(filename.c_str()) == 0) {
    return false;
  }
  return true;
}

inline bool SetFileAttributes(const std::wstring& filename, DWORD file_attributes) {
  return ::SetFileAttributes(filename.c_str(), file_attributes) != 0;
}

inline bool RecursiveRemoveDirectory(const std::wstring& directory) {
  return detail::RecursiveRemoveDirectory()(directory);
}

inline bool IsPathDirectory(const std::wstring& path) {
  return detail::IsPathDirectory()(path);
}
} //namespace ult

#endif // ULT_FILE_DIR_H_