#include <iostream>
using namespace std;

#include <TROOT.h>
#include <TSystem.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cout << endl;
    cout << "SYNTAX: rootSystemTest PathFileName";
    cout << endl;
    return 1;
  }

  TROOT root("rootSystemTest", "rootSystemTest", 0);

  cout << endl;
  cout << "Path name:          " << argv[1] << endl;
  char *str = gSystem->ExpandPathName(argv[1]);
  cout << "Path name expanded: " << str << endl;

  if (gSystem->AccessPathName(str)==true) {
    cout << endl << "file " << str << " doesn't exist :-(" << endl;
  } else {
    cout << endl << "file " << str << " exists :-)" << endl;
  }

  cout << "done ..." << endl;

  if (str)
    delete str;

  return 0;
}
