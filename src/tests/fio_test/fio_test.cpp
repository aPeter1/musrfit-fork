#include <fstream>
using namespace std;

int main()
{
  ofstream f;

  // open mlog-file
  f.open("fio_test.txt", iostream::out);
  if (!f.is_open()) {
    return -1;
  }

  const int prec=6;
  double val = 0.258466548916354835498465;

  f.width(9);
  f.precision(prec);
  f << left << val;
  f << " ";
  f.width(9);
  f.precision(prec);
  f << left << val;
  f << " ";
  f.width(9);
  f.precision(prec);
  f << left << val;

  f.close();

  return 0;
}
