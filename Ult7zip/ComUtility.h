#ifndef U7Z_COMUTILITY_H_
#define U7Z_COMUTILITY_H_

#include <basetyps.h>
#include <Unknwn.h>

class UnknownImp {

public:

  UnknownImp(void) : 
      ref_count_(0) {
  }

protected:

  ULONG ref_count_;
};

template <typename T>
class ComPtr {

public:

  ComPtr(void) :
      p_(nullptr) {
  }

private:

  T* p_;
};

#endif