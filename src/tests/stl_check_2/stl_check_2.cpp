#include <iostream>
#include <vector>
using namespace std;

#include "PStlCheck.h"

int main()
{
  PStlCheck check;

  for (unsigned int i=0; i<2; i++) {
    check.Add(i);
    cout << endl << "----------";
  }

  check.PrintTest();

  check.CleanUp();

  return 0;
}
