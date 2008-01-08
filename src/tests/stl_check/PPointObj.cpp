#include <iostream>
using namespace std;

#include "PPointObj.h"

PPointObj::PPointObj(unsigned int counter) : fCounter(counter)
{
  fTest = new int[5];

  for (unsigned int i=0; i<5; i++)
    fTest[i] = 10*counter+i;

  cout << endl << "in PPointObj() " << fCounter << ": fTest = " << fTest;
  cout << endl;
}

PPointObj::~PPointObj()
{
  cout << endl << "in ~PPointObj() " << fCounter << ": fTest = " << fTest;
  cout << endl;
}

void PPointObj::PrintTest()
{
  cout << endl << fCounter << ": ";
  for (unsigned int i=0; i<5; i++)
    cout << fTest[i] << ", ";
  cout << endl;
}

void PPointObj::CleanUp()
{
  if (fTest) {
    delete [] fTest;
    fTest = 0;
  }
}
