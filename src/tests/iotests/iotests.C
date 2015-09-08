// in order to illustrate the c++ io stream formatting
// usage: 1) start root
//        2) root 5.34.x -> .L iotests.C // root 6.x.y -> .L iotests.C+
//        3) iotests()

#include <iostream>
using namespace std;

void dump_ioflags(const ios_base::fmtflags flags)
{
  cout << endl << ">> flags decoded:" << endl;
  if (flags & ios_base::boolalpha)
    cout << ">> read/write bool elements as alphabetic strings (true and false)." << endl;
  if (flags & ios_base::showbase)
    cout << ">> write integral values preceded by their corresponding numeric base prefix." << endl;
  if (flags & ios_base::showpoint)
    cout << ">> write floating-point values including always the decimal point." << endl;
  if (flags & ios_base::showpos)
    cout << ">> write non-negative numerical values preceded by a plus sign (+)." << endl;
  if (flags & ios_base::skipws)
    cout << ">> skip leading whitespaces on certain input operations." << endl;
  if (flags & ios_base::unitbuf)
    cout << ">> flush output after each inserting operation." << endl;
  if (flags & ios_base::uppercase)
    cout << ">> write uppercase letters replacing lowercase letters in certain insertion operations." << endl;
  if (flags & ios_base::dec)
    cout << ">> read/write integral values using decimal base format." << endl;
  if (flags & ios_base::hex)
    cout << ">> read/write integral values using hexadecimal base format." << endl;
  if (flags & ios_base::oct)
    cout << ">> read/write integral values using octal base format." << endl;
  if (flags & ios_base::fixed)
    cout << ">> write floating point values in fixed-point notation." << endl;
  if (flags & ios_base::scientific)
    cout << ">> write floating-point values in scientific notation." << endl;
  if (flags & ios_base::internal)
    cout << ">> the output is padded to the field width by inserting fill characters at a specified internal point." << endl;
  if (flags & ios_base::left)
    cout << ">> the output is padded to the field width appending fill characters at the end." << endl;
  if (flags & ios_base::right)
    cout << ">> the output is padded to the field width by inserting fill characters at the beginning." << endl;
  cout << "---" << endl;
}

void iotests()
{
  Double_t dval = 13.24;
  
  // default settings printout
  cout << "default flags = " << cout.flags() << ":" << endl;
  dump_ioflags(cout.flags());
  cout << "dval = " << dval << endl;
  cout << "++++" << endl << endl;
  // printout of the 'floatfield' flags
  cout << "ios_base::floatfield :" << ios_base::floatfield << endl;
  dump_ioflags(ios_base::floatfield);
  cout << "set floatfield flags." << endl;
  cout.setf(ios_base::floatfield);
  cout << "dval = " << dval << endl;
  cout << "++++" << endl << endl;
  // unset 'floatfield' flags in order to see if the default is recovered
  cout.unsetf(ios::floatfield);
  cout << "cout.unsetf(ios::floatfield) -> cout.flags()=" << cout.flags() << endl;
  dump_ioflags(cout.flags());
  cout << "dval = " << dval << endl;
  cout << "++++" << endl << endl;
  // set 'fixed' flag
  cout.setf(ios_base::fixed);
  cout << "cout.setf(ios_base::fixed) -> cout.flags()=" << cout.flags() << endl;
  dump_ioflags(cout.flags());
  cout << "dval = " << dval << endl;
  cout << "++++" << endl << endl;
  // unset 'fixed' flag and set 'scientific' instead
  cout.unsetf(ios_base::fixed);
  cout << "cout.unsetf(ios_base::fixed)" << endl;
  cout.setf(ios_base::scientific);
  cout << "cout.setf(ios_base::scientific) -> cout.flags()=" << cout.flags() << endl;
  dump_ioflags(cout.flags());
  cout << "dval = " << dval << endl;
  cout << "++++" << endl << endl;

  // precision playground according to http://www.cplusplus.com/reference/ios/ios_base/precision/
  cout << "precision playground according to http://www.cplusplus.com/reference/ios/ios_base/precision/" << endl;
  Double_t f = 3.14159;
  cout.unsetf ( ios::floatfield );                // floatfield not set
  cout << "cout.unsetf ( ios::floatfield ) -> cout.flags()=" << cout.flags() << endl; 
  dump_ioflags(cout.flags());
  cout.precision(5);
  cout << "set cout.precision(5)" << endl;
  cout << f << endl;
  cout.precision(10);
  cout << "set cout.precision(10)" << endl;
  cout << f << endl;
  cout.setf( ios::fixed,  ios::floatfield ); // floatfield set to fixed
  cout << "cout.setf( ios::fixed,  ios::floatfield ) -> cout.flags()=" << cout.flags() << endl; 
  dump_ioflags(cout.flags());
  cout << f << endl;
  cout << "++++" << endl << endl;
}
