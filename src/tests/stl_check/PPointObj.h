#ifndef _PPOINTOBJ_H_
#define _PPOINTOBJ_H_

class PPointObj {
  public:
    PPointObj(unsigned int counter);
    ~PPointObj();

    void PrintTest();
    void CleanUp();

  private:
    int fCounter;
    int *fTest;
};

#endif // _PPOINTOBJ_H_
