void exportToAscii(const char *flnRoot, const char *flnAscii)
{
  // load library
  gSystem->Load("$ROOTSYS/lib/libTLemRunHeader");
  gSystem->Load("$ROOTSYS/lib/libTMusrRunHeader");

  // open MusrRoot file
  TFile fin(flnRoot, "READ");
  if (fin.IsZombie()) {
    cout << endl << "**ERROR** Couldn't open " << flnRoot << " for reading." << endl << endl;
    return;
  }

  // get time resolution
  TFolder *runHeader = 0;
  fin.GetObject("RunHeader", runHeader);
  if (runHeader == 0) {
    cerr << endl << ">> **ERROR** Couldn't get top folder RunHeader";
    fin.Close();
    return -1;
  }

  TMusrRunHeader *header = new TMusrRunHeader(flnRoot);

  if (!header->ExtractAll(runHeader)) {
    cerr << endl << ">> **ERROR** couldn't extract all RunHeader information";
    fin.Close();
    return -1;
  }

  TMusrRunPhysicalQuantity prop;
  Bool_t ok;
  header->Get("RunInfo/Time Resolution", prop, ok);
  if (!ok) {
    cerr << endl << "**ERROR** couldn't extract time resolution :-(, will quit" << endl;
    fin.Close();
    return -1;
  }
  Double_t timeResolutionValue = prop.GetValue();
  TString timeResolutionUnit = prop.GetUnit();
  cout << endl << "Time Resolution: " << timeResolutionValue << " (" << timeResolutionUnit << ")" << endl;

  Int_t ival = 0;
  Int_t noOfHistos = 0;
  header->Get("RunInfo/No of Histos", ival, ok);
  if (ok) {
    noOfHistos = ival;
  }
  cout << endl << "Found " << noOfHistos << " number of histos." << endl;

  // get the data
  TFolder *folder=0;
  // check if histos folder is found
  fin.GetObject("histos", folder);
  if (!folder) {
    cerr << endl << "**ERROR** couldn't get histo folder." << endl;
    fin.Close();
    return -1;
  }

  vector< vector<double> > data;
  TString str;
  data.resize(noOfHistos);
  for (Int_t i=0; i<noOfHistos; i++) {
    str.Form("hDecay%03d", i+1);
    TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(str.Data()));
    if (!histo) {
      cerr << endl << "**ERROR** Couldn't get histo " << str << endl;
      fin.Close();
      return false;
    }

    for (Int_t j=0; j<histo->GetNbinsX(); j++) {
      data[i].push_back(histo->GetBinContent(j));
    }
  }

  fin.Close();

  // write data
  ofstream fout(flnAscii, ofstream::out);
  fout << "% Time Resolution: " << timeResolutionValue << " (" << timeResolutionUnit << ")" << endl;
  fout << "% # histos: " << noOfHistos << endl;
  for (Int_t i=1; i<data[0].size(); i++) {
    for (Int_t j=0; j<data.size()-1; j++) {
      fout << data[j][i] << ", ";
    }
    fout << data[data.size()-1][i] << endl;
  }
  fout.close();
}
