#include <cstring>

#include <iostream>
#include <string>
using namespace std;

#include "PFunctionHandler.h"

//-----------------------------------------------------
void syntax()
{
  cout << endl << "spirit_fcn_test [--file <filename> [--debug]] | [--help]";
  cout << endl << "  without arguments: interactive mode";
  cout << endl << "  --file <filename>: function block etc. from file";
  cout << endl << "  --help:  this help";
  cout << endl << "  --debug: will print the ast tree only (no evaluatio)";
  cout << endl << endl;
}

//-----------------------------------------------------
void handle_input(vector<string> &lines)
{
  cout << endl << "will handle input ...";
  cout << endl << "you should provide a PAR, a MAP, and a FUNCTION block";
  cout << endl << "  Map block:";
  cout << endl << "  MAP <map1> <map2> ... <mapM>";
  cout << endl << "  Parameter block:";
  cout << endl << "  PAR <par1> <par2> ... <parN>";
  cout << endl << "  Function Block:";
  cout << endl << "  FUNCTION";
  cout << endl << "  fun1 = <function1>";
  cout << endl << "  fun2 = <function2>";
  cout << endl << "  ...";
  cout << endl << "  funX = <functionX>";
  cout << endl << "  END";
  cout << endl << "to get out of the input handle type '.q'";
  cout << endl;
  bool done = false;
  char str[128];
  do {
    cout << ">> ";
    cin.getline(str, sizeof(str));
    if (!strcmp(str, ".q"))
      done = true;
    else
      lines.push_back(str);
  } while (!done);
}

//-----------------------------------------------------
int main(int argc, char *argv[])
{
  bool inputFile = false;
  bool debug     = false;

  if (argc > 4) {
    syntax();
    return 0;
  } else if (argc == 2) {
    syntax();
    return 0;
  } else if (argc >= 3) {
    if (strcmp(argv[1], "--file")) {
      syntax();
      return 0;
    } else {
      inputFile = true;
    }
    if (argc == 4) {
      if (strcmp(argv[3], "--debug")) {
        syntax();
        return 0;
      } else {
        debug = true;
      }
    }
  }

  PFunctionHandler *fcnHandler = 0;

  if (inputFile) {
    fcnHandler = new PFunctionHandler(argv[2], debug);
  } else {
    vector<string> lines;
    handle_input(lines);
    fcnHandler = new PFunctionHandler(lines);
  }

  if (fcnHandler == 0) {
   cout << endl << "Couldn't invoke function handler, sorry ..." << endl;
   return 0;
  }

  bool go_on = fcnHandler->IsValid();

  if (go_on) {
    cout << endl << "will do the parsing ...";
    if (fcnHandler->DoParse()) {
       if (!debug) {
         cout << endl << "will do the evaluation ...";
         for (unsigned int i=0; i<fcnHandler->GetNoOfFuncs(); i++)
           cout << endl << "FUN" << fcnHandler->GetFuncNo(i) << " = " << fcnHandler->Eval(fcnHandler->GetFuncNo(i));
       }
    }
  }

  // clean up
  if (fcnHandler) {
    delete fcnHandler;
    fcnHandler = 0;
  }

  return 1;
}
