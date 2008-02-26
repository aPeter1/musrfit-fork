/***************************************************************************

  PRunDataHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <fstream>
using namespace std;

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1F.h>

#include "TLemRunHeader.h"
#include "PRunDataHandler.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunDataHandler::PRunDataHandler(PMsrHandler *msrInfo)
{
//  cout << endl << "in PRunDataHandler::PRunDataHandler()";

  fMsrInfo = msrInfo;

  // read files
  if (!ReadFile()) // couldn't read file
    fAllDataAvailable = false;
  else
    fAllDataAvailable = true;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunDataHandler::~PRunDataHandler()
{
  for (unsigned int i=0; i<fData.size(); i++) {
    fData[i].fT0s.clear();
    for (unsigned int j=0; j<fData[i].fDataBin.size(); j++)
      fData[i].fDataBin[j].clear();
  }
  fData.clear();
}

//--------------------------------------------------------------------------
// GetRunData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
PRawRunData* PRunDataHandler::GetRunData(TString runName)
{
  unsigned int i;

  for (i=0; i<fData.size(); i++) {
    if (!fData[i].fRunName.CompareTo(runName)) // run found
      break;
  }

  if (i == fData.size())
    return 0;
  else
    return &fData[i];
}

//--------------------------------------------------------------------------
// ReadFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadFile()
{
  bool success = true;

  // loop over the full RUN list to see what needs to be read
  PMsrRunList *runList = 0;
  runList = fMsrInfo->GetMsrRunList();
  if (runList == 0) {
    cout << endl << "PRunDataHandler::ReadFile(): Couldn't obtain run list from PMsrHandler: something VERY fishy";
    return false;
  }

  PMsrRunList::iterator run_it;
  for (run_it = runList->begin(); run_it != runList->end(); ++run_it) {
//cout << endl << "run : " << run_it->fRunName.Data();
    fRunName = run_it->fRunName;
    // check is file is already read
    if (FileAlreadyRead(*run_it))
      continue;
    // check if file actually exists
    if (!FileExistsCheck(*run_it))
      return false;
    // everything looks fine, hence try to read the data file
    if (!run_it->fFileFormat.CompareTo("root-npp")) // not post pile up corrected histos
      success = ReadRootFile();
    else if (!run_it->fFileFormat.CompareTo("root-ppc")) // post pile up corrected histos
      success = ReadRootFile();
    else if (!run_it->fFileFormat.CompareTo("nexus"))
      success = ReadNexusFile();
    else if (!run_it->fFileFormat.CompareTo("psi-bin"))
      success = ReadPsiBinFile();
    else if (!run_it->fFileFormat.CompareTo("mud"))
      success = ReadMudFile();
    else if (!run_it->fFileFormat.CompareTo("nemu"))
      success = ReadNemuFile();
    else if (!run_it->fFileFormat.CompareTo("ascii"))
      success = ReadAsciiFile();
    else
      success = false;
  }

  return success;
}

//--------------------------------------------------------------------------
// FileAlreadyRead
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::FileAlreadyRead(PMsrRunStructure &runInfo)
{
  for (unsigned int i=0; i<fData.size(); i++) {
    if (!fData[i].fRunName.CompareTo(runInfo.fRunName)) // run alread read
      return true;
  }

  return false;
}

//--------------------------------------------------------------------------
// FileExistsCheck
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::FileExistsCheck(PMsrRunStructure &runInfo)
{
  bool success = true;

  // local init
  TROOT root("PRunBase", "PRunBase", 0);
  TString pathName = "???";
  TString str;
  TString ext;

  runInfo.fBeamline.ToLower();
  runInfo.fInstitute.ToLower();
  runInfo.fFileFormat.ToLower();

  // file extensions for the various formats
  if (!runInfo.fFileFormat.CompareTo("root-npp")) // not post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.fFileFormat.CompareTo("root-ppc")) // post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.fFileFormat.CompareTo("nexus"))
    ext = TString("nexus");
  else if (!runInfo.fFileFormat.CompareTo("psi-bin"))
    ext = TString("bin");
  else if (!runInfo.fFileFormat.CompareTo("mud"))
    ext = TString("mud");
  else if (!runInfo.fFileFormat.CompareTo("nemu"))
    ext = TString("nemu");
  else if (!runInfo.fFileFormat.CompareTo("ascii"))
    ext = TString("dat");
  else
    success = false;

  // unkown file format found
  if (!success) {
    str = runInfo.fFileFormat;
    str.ToUpper();
    cout << endl << "File Format '" << str.Data() << "' unsupported.";
    cout << endl << "  support file formats are:";
    cout << endl << "  ROOT-NPP -> root not post pileup corrected for lem";
    cout << endl << "  ROOT-PPC -> root post pileup corrected for lem";
    cout << endl << "  NEXUS    -> nexus file format";
    cout << endl << "  PSI-BIN  -> psi bin file format";
    cout << endl << "  MUD      -> triumf mud file format";
    cout << endl << "  NEMU     -> lem ascii file format";
    cout << endl << "  ASCII    -> column like file format";
    cout << endl;
    return success;
  }

  // check if the file is in the local directory
  str = runInfo.fRunName + TString(".") + ext;
  if (gSystem->AccessPathName(str.Data())!=true) { // found in the local dir
    pathName = str;
  }

  // check if the file is found in the directory given in the startup file
  // ** STILL MISSING **

  // check if the file is found in the directories given by WKMFULLDATAPATH
  const char *wkmpath = gSystem->Getenv("WKMFULLDATAPATH");
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    str = TString(wkmpath);
    // WKMFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (int i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + "/" + runInfo.fRunName + TString(".") + ext;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }

  // check if the file is found in the WKM generated default path
  if (pathName.CompareTo("???") == 0) { // not found in WKMFULLDATAPATH search
    str = TString(wkmpath);
    // WKMFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (int i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/data/") +
            runInfo.fInstitute + TString("/") +
            runInfo.fBeamline + TString("/") +
            runInfo.fRunName + TString(".") + ext;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }

  // no proper path name found
  if (pathName.CompareTo("???") == 0) {
    cout << endl << "ERROR: Couldn't find '" << runInfo.fRunName << "' in any standard path.";
    cout << endl << "  standard search pathes are:";
    cout << endl << "  1. the local directory";
    cout << endl << "  2. the data directory given in the startup XML file";
    cout << endl << "  3. the directories listed in WKMFULLDATAPATH";
    cout << endl << "  4. default path construct which is described in the manual"; 
    return false;
  }

  fRunPathName = pathName;

  return true;
}

//--------------------------------------------------------------------------
// ReadRootFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadRootFile()
{
  PDoubleVector histoData;
  PRawRunData   runData;

  TFile f(fRunPathName.Data());
  if (f.IsZombie()) {
    return false;
  }

  TFolder *folder;
  f.GetObject("RunInfo", folder);
  if (!folder) {
    cout << endl << "Couldn't obtain RunInfo from " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }

  // read header and check if some missing run info need to be fed
  TLemRunHeader *runHeader = dynamic_cast<TLemRunHeader*>(folder->FindObjectAny("TLemRunHeader"));

  // check if run header is valid
  if (!runHeader) {
    cout << endl << "Couldn't obtain run header info from ROOT file " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }

  // get time resolution
  runData.fTimeResolution = runHeader->GetTimeResolution();

  // get number of histogramms
  int noOfHistos = runHeader->GetNHist();

  // get t0's
  Int_t *t0 = runHeader->GetTimeZero();
  // check if t0's are there
  if (t0[0] != -1) { // ugly, but at the moment there is no other way
    // copy t0's so they are not lost
    for (int i=0; i<noOfHistos; i++)
      runData.fT0s.push_back(t0[i]);
  }

  // read data ---------------------------------------------------------
  // check if histos folder is found
  f.GetObject("histos", folder);
  if (!folder) {
    cout << endl << "Couldn't obtain histos from " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }
  // get all the data
  char histoName[32];
  // read first the data which are NOT post pileup corrected
  for (int i=0; i<noOfHistos; i++) {
    sprintf(histoName, "hDecay%02d", i);
    TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
    if (!histo) {
      cout << endl << "PRunDataHandler::ReadRootFile: Couldn't get histo " << histoName;
      return false;
    }
    // fill data
    for (int j=1; j<histo->GetNbinsX(); j++)
      histoData.push_back(histo->GetBinContent(j));
    // store them in runData vector
    runData.fDataBin.push_back(histoData);
    // clear histoData for the next histo
    histoData.clear();
  }
  // now read the data which ARE post pileup corrected
  for (int i=0; i<noOfHistos; i++) {
    sprintf(histoName, "hDecay%02d", i+POST_PILEUP_HISTO_OFFSET);
    TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
    if (!histo) {
      cout << endl << "PRunDataHandler::ReadRootFile: Couldn't get histo " << histoName;
      return false;
    }
    // fill data
    for (int j=1; j<histo->GetNbinsX(); j++)
      histoData.push_back(histo->GetBinContent(j));
    // store them in runData vector
    runData.fDataBin.push_back(histoData);
    // clear histoData for the next histo
    histoData.clear();
  }

  f.Close();

  // keep run name
  runData.fRunName = fRunName;

  // add run to the run list
  fData.push_back(runData);

  return true;
}

//--------------------------------------------------------------------------
// ReadNexusFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadNexusFile()
{
  cout << endl << "PRunDataHandler::ReadNexusFile(): Sorry, not yet implemented ...";
  return false;
}

//--------------------------------------------------------------------------
// ReadNemuFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadNemuFile()
{
//  cout << endl << "PRunDataHandler::ReadNemuFile(): Sorry, not yet implemented ...";

  PDoubleVector histoData;
  PRawRunData runData;

  // init runData header info
  runData.fRunName        = TString("");
  runData.fRunTitle       = TString("");
  runData.fSetup          = TString("");
  runData.fField          = nan("NAN");
  runData.fTemp           = nan("NAN");
  runData.fTimeResolution = nan("NAN");

  // open file
  ifstream f;

  // open dump-file
  f.open(fRunPathName.Data(), ifstream::in);
  if (!f.is_open()) {
    cout << endl << "Couldn't open run data (" << fRunPathName.Data() << ") file for reading, sorry ...";
    cout << endl;
    return false;
  }

  // read header
  bool    headerInfo = true;
  char    instr[512];
  TString line;
  double  dval;
  int     ival;
  bool    ok;
  int     groups = 0, channels = 0;

  Ssiz_t idx;
  do {
    f.getline(instr, sizeof(instr));
    line = TString(instr);
    if (line.IsWhitespace()) { // end of header reached
      headerInfo = false;
    } else { // real stuff, hence filter data
      if (line.Contains("Title")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Title:'
        StripWhitespace(line);
        runData.fRunTitle = line;
      } else if (line.Contains("Field")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Field:'
        StripWhitespace(line);
        dval = ToDouble(line, ok);
        if (ok)
          runData.fField = dval;
      } else if (line.Contains("Setup")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Setup:'
        StripWhitespace(line);
        runData.fSetup = line;
      } else if (line.Contains("Temp")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Temp:'
        StripWhitespace(line);
        dval = ToDouble(line, ok);
        if (ok)
          runData.fTemp = dval;
      } else if (line.Contains("Groups")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Groups:'
        StripWhitespace(line);
        ival = ToInt(line, ok);
        if (ok)
          groups = ival;
      } else if (line.Contains("Channels")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Channels:'
        StripWhitespace(line);
        ival = ToInt(line, ok);
        if (ok)
          channels = ival;
      } else if (line.Contains("Resolution")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Resolution:'
        StripWhitespace(line);
        dval = ToDouble(line, ok);
        if (ok)
          runData.fTimeResolution = dval * 1000.0;
      }
    }
  } while (headerInfo);

  if ((groups == 0) || (channels == 0) || isnan(runData.fTimeResolution)) {
    cout << endl << "PRunDataHandler::ReadNemuFile(): essential header informations are missing!";
    f.close();
    return false;
  }
/*
cout << endl << ">> run title: '" << runData.fRunTitle.Data() << "'";
cout << endl << ">> setup    : '" << runData.fSetup.Data() << "'";
cout << endl << ">> field    : " << runData.fField;
cout << endl << ">> temp     : " << runData.fTemp;
cout << endl << ">> groups   : " << groups;
cout << endl << ">> channels : " << channels;
cout << endl << ">> time resolution : " << runData.fTimeResolution;
*/

  // read data ---------------------------------------------------------
  int status;
  unsigned int group_counter = 0;
  int val[10];
  while (!f.eof()) {
    f.getline(instr, sizeof(instr));
    // check if empty line, i.e. new group
    if (IsWhitespace(instr)) {
      runData.fDataBin.push_back(histoData);
      histoData.clear();
      group_counter++;
    }
    // extract values
    status = sscanf(instr, "%d %d %d %d %d %d %d %d %d %d", 
                    &val[0], &val[1], &val[2], &val[3], &val[4],
                    &val[5], &val[6], &val[7], &val[8], &val[9]);
    // no values found: error
    if (status == 0) {
      cout << endl << "PRunDataHandler::ReadNemuFile(): **ERROR** while reading data ...";
      // clean up
      for (unsigned int i=0; i<group_counter; i++)
        runData.fDataBin[i].clear();
      runData.fDataBin.clear();
      return false;
    }
    // feed data
    for (int i=0; i<status; i++)
      histoData.push_back(val[i]);
  }

  // save the last histo if not empty
  if (histoData.size() > 0) {
    runData.fDataBin.push_back(histoData);
    histoData.clear();
  }

  // close file
  f.close();

  // check if all groups are found
  if ((int) runData.fDataBin.size() != groups) {
    cout << endl << "PRunDataHandler::ReadNemuFile(): **ERROR**";
    cout << endl << "  expected " << groups << " histos, but found " << runData.fDataBin.size();
    // clean up
    for (unsigned int i=0; i<runData.fDataBin.size(); i++)
      runData.fDataBin[i].clear();
    runData.fDataBin.clear();
    return false;
  }

  // check if all groups have enough channels
  for (unsigned int i=0; i<runData.fDataBin.size(); i++) {
    if ((int) runData.fDataBin[i].size() != channels) {
      cout << endl << "PRunDataHandler::ReadNemuFile(): **ERROR**";
      cout << endl << "  expected " << channels << " bins in histo " << i << ", but found " << runData.fDataBin[i].size();
      // clean up
      for (unsigned int i=0; i<runData.fDataBin.size(); i++)
        runData.fDataBin[i].clear();
      runData.fDataBin.clear();
      return false;
    }
  }

  // keep run name
  runData.fRunName = fRunName;

  // add run to the run list
  fData.push_back(runData);

  return true;
}

//--------------------------------------------------------------------------
// ReadPsiBinFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadPsiBinFile()
{
  cout << endl << "PRunDataHandler::ReadPsiBinFile(): Sorry, not yet implemented ...";
  return false;
}

//--------------------------------------------------------------------------
// ReadMudFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadMudFile()
{
  cout << endl << "PRunDataHandler::ReadMudFile(): Sorry, not yet implemented ...";
  return false;
}

//--------------------------------------------------------------------------
// ReadAsciiFile
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunDataHandler::ReadAsciiFile()
{
  cout << endl << "PRunDataHandler::ReadAsciiFile(): Sorry, not yet implemented ...";
  return false;
}

//--------------------------------------------------------------------------
// StripWhitespace
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
bool PRunDataHandler::StripWhitespace(TString &str)
{
  char *s    = 0;
  char *subs = 0;
  int i;
  int start;
  int end;
  int size;

  size = (int)str.Length();
  s = new char[size+1];

  if (!s)
    return false;

  for (int i=0; i<size+1; i++)
    s[i] = str[i];
  s[size] = 0;

  // check for whitespaces at the beginning of the string
  i = 0;
  while (isblank(s[i]) || iscntrl(s[i])) {
    i++;
  }
  start = i;

  // check for whitespaces at the end of the string
  i = strlen(s);
  while (isblank(s[i]) || iscntrl(s[i])) {
    i--;
  }
  end = i;

  if (end < start)
    return false;

  // make substring
  subs = new char[end-start+2];
  if (!subs)
    return false;

  strncpy(subs, s+start, end-start+1);
  subs[end-start+1] = 0;

  str = TString(subs);

  // clean up
  if (subs) {
    delete [] subs;
    subs = 0;
  }
  if (s) {
    delete [] s;
    s = 0;
  }

  return true;
}

//--------------------------------------------------------------------------
// IsWhitespace
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
bool PRunDataHandler::IsWhitespace(const char *str)
{
  unsigned int i=0;

  while (isblank(str[i]) || (iscntrl(str[i])) && str[i] != 0)
    i++;

  if (i == strlen(str))
    return true;
  else
    return false;
}

//--------------------------------------------------------------------------
// ToDouble
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 * \param ok
 */
double PRunDataHandler::ToDouble(TString &str, bool &ok)
{
  char *s;
  double value;
  int size, status;

  ok = true;

  size = (int)str.Length();
  s = new char[size+1];

  if (!s) {
    ok = false;
    return 0.0;
  }

  // copy string; stupid way but it works
  for (int i=0; i<size+1; i++)
    s[i] = str[i];
  s[size] = 0;

  // extract value
  status = sscanf(s, "%lf", &value);
  if (status != 1) {
    ok = false;
    return 0.0;
  }

  // clean up
  if (s) {
    delete [] s;
    s = 0;
  }

  return value;
}

//--------------------------------------------------------------------------
// ToInt
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 * \param ok
 */
int PRunDataHandler::ToInt(TString &str, bool &ok)
{
  char *s;
  int value;
  int size, status;

  ok = true;

  size = (int)str.Length();
  s = new char[size+1];

  if (!s) {
    ok = false;
    return 0;
  }

  // copy string; stupid way but it works
  for (int i=0; i<size+1; i++)
    s[i] = str[i];
  s[size] = 0;

  // extract value
  status = sscanf(s, "%d", &value);
  if (status != 1) {
    ok = false;
    return 0;
  }

  // clean up
  if (s) {
    delete [] s;
    s = 0;
  }

  return value;
}
