#ifndef _PSTLCHECK_H_
#define _PSTLCHECK_H_

#include <vector>
using namespace std;

#include "PPointObj.h"

class PStlCheck {
  public:
    PStlCheck();
    ~PStlCheck();

    void Add(unsigned int count);
    void CleanUp();
    void PrintTest();

  private:
    vector<PPointObj*> fPpo;
};

#endif // _PSTLCHECK_H_
