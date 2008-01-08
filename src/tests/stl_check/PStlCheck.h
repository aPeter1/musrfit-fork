#ifndef _PSTLCHECK_H_
#define _PSTLCHECK_H_

#include <list>
using namespace std;

#include "PPointObj.h"

class PStlCheck {
  public:
    PStlCheck();
    ~PStlCheck();

    list<PPointObj> fPpo;
};

#endif // _PSTLCHECK_H_
