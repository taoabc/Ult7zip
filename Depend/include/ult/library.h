/*
** 轻度封装DLL调用相关函数
** author
**   taoabc@gmail.com
*/
#ifndef ULT_LIBRARY_H_
#define ULT_LIBRARY_H_

#include <string>
#include <windows.h>
#include <vector>

namespace ult {

class Library {

public:

  Library(void) : module_(NULL) {
  };

  ~Library(void) {
    Free();
  }

  operator HMODULE() const {
    return module_;
  }

  HMODULE* operator&() {
    return &module_;
  }

  bool IsLoaded(void) const {
    return (module_ != NULL);
  }

  void Attach(HMODULE m) {
    Free();
    module_ = m;
  }

  HMODULE Detach(void) {
    HMODULE m = module_;
    module_ = NULL;
    return m;
  }

  bool Free(void) {
    if (module_ == NULL) {
      return true;
    }
    if (FALSE == ::FreeLibrary(module_)) {
      return false;
    }
    module_ = NULL;
    return true;
  }

  bool Load(const std::wstring& filename) {
    return LoadOperations(::LoadLibrary(filename.c_str()));
  }

  bool LoadEx(const std::wstring& filename, DWORD flags) {
    return LoadOperations(::LoadLibraryEx(filename.c_str(), NULL, flags));
  }

  FARPROC GetProc(const std::string& procname) const {
    return ::GetProcAddress(module_, procname.c_str());
  }

  FARPROC GetProc(const std::wstring& procname) const {
    int len = (int)procname.length();
    std::vector<char> buffer(len * 3);
    int outlen = ::WideCharToMultiByte(CP_ACP, 0, procname.c_str(), len, buffer.data(), len*3, NULL, NULL);
    std::string ansi_procname(buffer.data(), outlen);
    return GetProc(ansi_procname);
  }

private:

  bool LoadOperations(HMODULE new_module) {
    if (new_module == NULL) {
      return false;
    }
    if (!Free()) {
      return false;
    }
    module_ = new_module;
    return true;
  }

  HMODULE module_;
};
} //namespace ult

#endif