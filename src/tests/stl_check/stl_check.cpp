#include <iostream>
#include <list>
using namespace std;

#include "PStlCheck.h"

int main()
{
  PStlCheck check;
  unsigned int counter = 0;

  for (unsigned int i=0; i<2; i++) {
    check.fPpo.push_back(PPointObj(counter++));
    cout << endl << "----------";
  }

  cout << endl << "size = " << check.fPpo.size();
  cout << endl;

  list<PPointObj>::iterator iter;
  for (iter=check.fPpo.begin(); iter!=check.fPpo.end(); ++iter)
    iter->PrintTest();

  for (iter=check.fPpo.begin(); iter!=check.fPpo.end(); ++iter)
    iter->CleanUp();

  return 0;
}
