#include <iostream>
using namespace std;

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1F.h>

#include "TLemRunHeader.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cout << endl;
    cout << "SYNTAX: nemuRootFileRead PathFileName";
    cout << endl;
    return 1;
  }

  TROOT root("nemuRootFileRead", "nemuRootFileRead", 0);

  // get file name
  char *str = gSystem->ExpandPathName(argv[1]);

  // check that the file name exists
  if (gSystem->AccessPathName(str)==true) {
    cout << endl << "file " << str << " doesn't exist :-(" << endl;
    cout << endl << "will quit now ..." << endl;
    return 1;
  }

  // define file
  TFile f(str);
  if (f.IsZombie()) {
    cout << endl << "couldn't open file " << str << endl;
    cout << endl << "will quit now ..." << endl;
    return 1;
  }

  cout << endl << "opened file " << str;

  // get RunInfo Folder
  TFolder *folder = 0;
  f.GetObject("RunInfo", folder);

  // check if RunInfo Folder is valid
  if (!folder) {
    cout << endl << "Couldn't obtain RunInfo folder from ROOT file " << str;
    cout << endl << "will quit now ..." << endl;
    return 1;
  }

  // get run header
  TLemRunHeader *runHeader = dynamic_cast<TLemRunHeader*>(folder->FindObjectAny("TLemRunHeader"));

  // check if run header is valid
  if (!runHeader) {
    cout << endl << "Couldn't obtain run header info from ROOT file " << str;
    cout << endl << "will quit now ..." << endl;
    return 1;
  }

  cout << endl << "Run No   : " << runHeader->GetRunNumber();
  cout << endl << "Run Title: " << runHeader->GetRunTitle().GetString().Data();

  // get histos folder
  f.GetObject("histos", folder);

  // check if histos Folder is valid
  if (!folder) {
    cout << endl << "Couldn't obtain histos folder from ROOT file " << str;
    cout << endl << "will quit now ..." << endl;
    return 1;
  }

  // get histo hDecay00
  TH1F *histo = dynamic_cast<TH1F*>(folder->FindObject("DecayAnaModule/hDecay00"));

  // check if histo hDecay00 was found
  if (!histo) {
    cout << endl << "Couldn't obtain hDecay00 histo from ROOT file " << str;
    cout << endl << "will quit now ..." << endl;
    return 1;
  }

  // print out first 10 values of hDecay00 and some other info
  cout << endl << "hDecay00: No of Bins: " << histo->GetNbinsX();
  int max;
  if (max > histo->GetNbinsX())
    max = histo->GetNbinsX();
  else
    max = 10;
  cout << endl << "hDecay00: Data:";
  for (int i=0; i<max; i++) {
    cout << endl << i << ": " << histo->GetBinContent(i+1);
  }

  if (f.IsOpen())
    f.Close();

  cout << endl << "closed file .." << endl;

  if (str)
    delete str;

  cout << endl;

  return 0;
}
