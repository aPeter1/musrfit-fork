#include <math.h>

#include <iostream>
using namespace std;

int main()
{
  double value;

  value = nan("NAN");

  if (isnan(value))
    cout << endl << "value is not a number";
  else
    cout << endl << "value is a number";

  cout << endl << "value = " << value;

  cout << endl << "done ..." << endl;

  return 0;
}
