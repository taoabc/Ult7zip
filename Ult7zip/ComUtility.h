#ifndef U7Z_COMUTILITY_H_
#define U7Z_COMUTILITY_H_

#include <basetyps.h>
#include <Unknwn.h>

template <typename T>
inline bool IsNull(const T& p) {
  return (p == NULL || p == nullptr);
}

class UnknownImp {

public:

  UnknownImp(void) : 
      ref_count_(0) {
  }

protected:

  ULONG ref_count_;
}; //class UnknownImp

template <typename T>
class ComPtr {

public:

  ComPtr(void) :
      p_(nullptr) {
  }

  ComPtr(T* p) {
    p_ = p;
    if (!IsNull(p_)) {
      p_->AddRef();
    }
  }

  ComPtr(const ComPtr<T>& p) {
    p_ = p.p_;
    if (!IsNull(p_)) {
      p_->AddRef();
    }
  }

  ~ComPtr(void) {
    if (!IsNull(p_)) {
      p_->Release();
    }
  }

  void Release(void) {
    if (!IsNull(p_)) {
      p_->Release();
      p_ = nullptr;
    }
  }

  operator T*() const {
    return static_cast<T*>(p_);
  }

  T** operator&() {
    return &p_;
  }

  T* operator->() const {
    return p_;
  }

  T* operator=(T* p) {
    if (!IsNull(p)) {
      p->AddRef();
    }
    if (!IsNull(p_)) {
      p_->Release();
    }
    p_ = p;
    return p_;
  }

  T* operator=(const ComPtr<T>& p) {
    return (*this = p.p_);
  }

  bool operator!() const {
    return IsNull(p_);
  }

  void Attach(T* p) {
    Release();
    p_ = p;
  }

  T* Detach(void) {
    T* p = p_;
    p_ = NULL;
    return p;
  }

  template <typename Tp>
  HRESULT QueryInterface(REFIID riid, Tp** ppobj) const {
    return p_->QueryInterface(riid, (void**)ppobj);
  }

private:

  T* p_;
}; // class ComPtr

#endif