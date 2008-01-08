#include <iostream>
using namespace std;

#include "PPointObj.h"

#define PPO_SIZE 100

PPointObj::PPointObj(unsigned int counter) : fCounter(counter)
{
  fTest = new int[PPO_SIZE];

  for (unsigned int i=0; i<PPO_SIZE; i++)
    fTest[i] = 2*PPO_SIZE*counter+i;

  cout << endl << "in PPointObj() " << fCounter << ": fTest = " << fTest;
  cout << endl;
}

PPointObj::~PPointObj()
{
  if (fTest) {
    delete [] fTest;
    fTest = 0;
  }

  cout << endl << "in ~PPointObj() " << fCounter << ": fTest = " << fTest;
  cout << endl;
}

void PPointObj::PrintTest()
{
  cout << endl << fCounter << ": ";
  for (unsigned int i=0; i<PPO_SIZE; i++)
    cout << fTest[i] << ", ";
  cout << endl;
}
