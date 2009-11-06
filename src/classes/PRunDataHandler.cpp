/***************************************************************************

  PRunDataHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.ch                                                  *
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
#include <string>
using namespace std;

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TDatime.h>

#include "TLemRunHeader.h"
#include "MuSR_td_PSI_bin.h"
#include "mud.h"

#include "PRunDataHandler.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> Constructor, reading the data histogramm files.
 *
 */
PRunDataHandler::PRunDataHandler(PMsrHandler *msrInfo) : fMsrInfo(msrInfo)
{
  // read files
  if (!ReadFile()) // couldn't read file
    fAllDataAvailable = false;
  else
    fAllDataAvailable = true;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> Constructor, reading the data histogramm files, and keeping a copy
 * of potential search paths.
 */
PRunDataHandler::PRunDataHandler(PMsrHandler *msrInfo, const PStringVector dataPath) : 
      fMsrInfo(msrInfo), fDataPath(dataPath)
{
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
 * <p> Destructor.
 *
 */
PRunDataHandler::~PRunDataHandler()
{
  fData.clear();
}

//--------------------------------------------------------------------------
// GetRunData
//--------------------------------------------------------------------------
/**
 * <p> Checks if runName is found.
 *
 * <b>return:</b> if data are found: pointer to the data.
 * otherwise the null pointer will be returned.
 *
 * \param runName run name, e.g. 2009/lem09_his_1234
 */
PRawRunData* PRunDataHandler::GetRunData(const TString &runName)
{
  UInt_t i;

  for (i=0; i<fData.size(); i++) {
    if (!fData[i].GetRunName()->CompareTo(runName)) // run found
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
 * <p> The main read file routine which is filtering what read sub-routine
 * needs to be called.
 *
 * <b>return:</b> true if reading was successful, false if reading failed.
 */
Bool_t PRunDataHandler::ReadFile()
{
  Bool_t success = true;

  // loop over the full RUN list to see what needs to be read
  PMsrRunList *runList = 0;
  runList = fMsrInfo->GetMsrRunList();
  if (runList == 0) {
    cerr << endl << "PRunDataHandler::ReadFile(): **ERROR** Couldn't obtain run list from PMsrHandler: something VERY fishy";
    cerr << endl;
    return false;
  }

  for (UInt_t i=0; i<runList->size(); i++) {
    for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) {
      fRunName = *(runList->at(i).GetRunName(j));
      // check is file is already read
      if (FileAlreadyRead(*(runList->at(i).GetRunName(j))))
        continue;
      // check if file actually exists
      if (!FileExistsCheck(runList->at(i), j))
        return false;
      // everything looks fine, hence try to read the data file
      if (!runList->at(i).GetFileFormat(j)->CompareTo("root-npp")) // not post pile up corrected histos
        success = ReadRootFile(true);
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("root-ppc")) // post pile up corrected histos
        success = ReadRootFile(false);
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("nexus"))
        success = ReadNexusFile();
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("psi-bin"))
        success = ReadPsiBinFile();
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("mud"))
        success = ReadMudFile();
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("wkm"))
        success = ReadWkmFile();
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("mdu-ascii"))
        success = ReadMduAsciiFile();        
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("ascii"))
        success = ReadAsciiFile();
      else if (!runList->at(i).GetFileFormat(j)->CompareTo("db"))
        success = ReadDBFile();
      else
        success = false;
    }
  }

  return success;
}

//--------------------------------------------------------------------------
// FileAlreadyRead
//--------------------------------------------------------------------------
/**
 * <p> Checks if a file has been already read in order to prevent multiple
 * reading of data files.
 *
 * <b>return:</b> true if the file has been read before, otherwise false.
 */
Bool_t PRunDataHandler::FileAlreadyRead(TString runName)
{
  for (UInt_t i=0; i<fData.size(); i++) {
    if (!fData[i].GetRunName()->CompareTo(runName)) { // run alread read
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------
// FileExistsCheck
//--------------------------------------------------------------------------
/**
 * <p> Checks if a given data file exists.
 *
 * \param runInfo reference to the msr-run-structure
 * \param idx index of the run (needed for ADDRUN feature).
 *
 * <b>return:</b> true if data file exists, otherwise false.
 */
Bool_t PRunDataHandler::FileExistsCheck(PMsrRunBlock &runInfo, const UInt_t idx)
{
  Bool_t success = true;

  // local init
  TROOT root("PRunBase", "PRunBase", 0);
  TString pathName = "???";
  TString str, *pstr;
  TString ext;

  pstr = runInfo.GetBeamline(idx);
  if (pstr == 0) {
    cerr << endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain beamline data." << endl;
    assert(0);
  }
  pstr->ToLower();
  runInfo.SetBeamline(*pstr, idx);
  pstr = runInfo.GetInstitute(idx);
  if (pstr == 0) {
    cerr << endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain institute data." << endl;
    assert(0);
  }
  pstr->ToLower();
  runInfo.SetInstitute(*pstr, idx);
  pstr = runInfo.GetFileFormat(idx);
  if (pstr == 0) {
    cerr << endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain file format data." << endl;
    assert(0);
  }
  pstr->ToLower();
  runInfo.SetFileFormat(*pstr, idx);

  // file extensions for the various formats
  if (!runInfo.GetFileFormat(idx)->CompareTo("root-npp")) // not post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("root-ppc")) // post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("nexus"))
    ext = TString("nexus");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("psi-bin"))
    ext = TString("bin");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("mud"))
    ext = TString("msr");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("wkm")) {
    if (!runInfo.GetBeamline(idx)->CompareTo("mue4"))
      ext = TString("nemu");
    else
      ext = *runInfo.GetBeamline(idx);
  }
  else if (!runInfo.GetFileFormat(idx)->CompareTo("mdu-ascii"))
    ext = TString("mdua");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("ascii"))
    ext = TString("dat");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("db"))
    ext = TString("db");
  else
    success = false;

  // unkown file format found
  if (!success) {
    pstr = runInfo.GetFileFormat(idx);
    if (pstr == 0) {
      cerr << endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain file format data." << endl;
      assert(0);
    }
    pstr->ToUpper();
    cerr << endl << "**ERROR** File Format '" << pstr->Data() << "' unsupported.";
    cerr << endl << "  support file formats are:";
    cerr << endl << "  ROOT-NPP  -> root not post pileup corrected for lem";
    cerr << endl << "  ROOT-PPC  -> root post pileup corrected for lem";
    cerr << endl << "  NEXUS     -> nexus file format";
    cerr << endl << "  PSI-BIN   -> psi bin file format";
    cerr << endl << "  MUD       -> triumf mud file format";
    cerr << endl << "  WKM       -> wkm ascii file format";
    cerr << endl << "  MDU-ASCII -> psi mdu ascii file format";
    cerr << endl << "  ASCII     -> column like file format";
    cerr << endl << "  DB        -> triumf db file \"format\"";
    cerr << endl;
    return success;
  }

  // check if the file is in the local directory
  str = *runInfo.GetRunName(idx) + TString(".") + ext;
  if (gSystem->AccessPathName(str.Data())!=true) { // found in the local dir
    pathName = str;
  }

  // check if the file is found in the <msr-file-directory>
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    str  = *fMsrInfo->GetMsrFileDirectoryPath();
    str += *runInfo.GetRunName(idx) + TString(".") + ext;
    if (gSystem->AccessPathName(str.Data())!=true) { // found
      pathName = str;
    }
  }

  // check if the file is found in the directory given in the startup file
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    for (UInt_t i=0; i<fDataPath.size(); i++) {
      str = fDataPath[i] + TString("/") + *runInfo.GetRunName(idx) + TString(".") + ext;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }

  // check if the file is found in the directories given by WKMFULLDATAPATH
  const Char_t *wkmpath = gSystem->Getenv("WKMFULLDATAPATH");
  if (pathName.CompareTo("???") == 0) { // not found in local directory and xml path
    str = TString(wkmpath);
    // WKMFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/") + *runInfo.GetRunName(idx) + TString(".") + ext;
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
    pstr = runInfo.GetInstitute(idx);
    if (pstr == 0) {
      cerr << endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain institute data." << endl;
      assert(0);
    }
    pstr->ToUpper();
    runInfo.SetInstitute(*pstr, idx);
    pstr = runInfo.GetBeamline(idx);
    if (pstr == 0) {
      cerr << endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain beamline data." << endl;
      assert(0);
    }
    pstr->ToUpper();
    runInfo.SetBeamline(*pstr, idx);
    TDatime datetime;
    TString dt;
    dt += datetime.GetYear();
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/DATA/") +
            *runInfo.GetInstitute(idx) + TString("/") +
            *runInfo.GetBeamline(idx) + TString("/") +
            dt + TString("/") +
            *runInfo.GetRunName(idx) + TString(".") + ext;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }

  // no proper path name found
  if (pathName.CompareTo("???") == 0) {
    cerr << endl << "**ERROR** Couldn't find '" << runInfo.GetRunName(idx)->Data() << "' in any standard path.";
    cerr << endl << "  standard search pathes are:";
    cerr << endl << "  1. the local directory";
    cerr << endl << "  2. the data directory given in the startup XML file";
    cerr << endl << "  3. the directories listed in WKMFULLDATAPATH";
    cerr << endl << "  4. default path construct which is described in the manual";
    return false;
  }

  fRunPathName = pathName;

  return true;
}

//--------------------------------------------------------------------------
// ReadRootFile
//--------------------------------------------------------------------------
/**
 * <p> Reads the LEM-data ROOT-files.
 *
 * \param notPostPileup This flag is used as a switch between "Not Post Pileup Corrected" 
 *                      and "Post Pileup Corrected" histogramms.
 *
 * <b>return:</b> true at successful reading, otherwise false.
 */
Bool_t PRunDataHandler::ReadRootFile(Bool_t notPostPileup)
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
    cerr << endl << "PRunDataHandler::ReadRootFile: **ERROR** Couldn't obtain RunInfo from " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }

  // read header and check if some missing run info need to be fed
  TLemRunHeader *runHeader = dynamic_cast<TLemRunHeader*>(folder->FindObjectAny("TLemRunHeader"));

  // check if run header is valid
  if (!runHeader) {
    cerr << endl << "PRunDataHandler::ReadRootFile: **ERROR** Couldn't obtain run header info from ROOT file " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }

  // get run title
  TObjString ostr = runHeader->GetRunTitle();
  runData.SetRunTitle(ostr.GetString());

  // get temperature
  runData.ClearTemperature();
  runData.SetTemperature(0, runHeader->GetSampleTemperature(), runHeader->GetSampleTemperatureError());

  // get field
  runData.SetField(runHeader->GetSampleBField());

  // get implantation energy
  runData.SetEnergy(runHeader->GetImpEnergy());

  // get moderator HV
  runData.SetTransport(runHeader->GetModeratorHV());

  // get setup
  runData.SetSetup(runHeader->GetLemSetup().GetString());

  // get time resolution
  runData.SetTimeResolution(runHeader->GetTimeResolution());

  // get number of histogramms
  Int_t noOfHistos = runHeader->GetNHist();

  // get t0's
  Double_t *t0 = runHeader->GetTimeZero();
  // check if t0's are there
  if (t0[0] != -1) { // ugly, but at the moment there is no other way
    // copy t0's so they are not lost
    for (Int_t i=0; i<noOfHistos; i++) {
      runData.AppendT0((Int_t)t0[i]);
    }
  }

  // read run summary to obtain ring anode HV values
  TObjArray *runSummary = dynamic_cast<TObjArray*>(folder->FindObjectAny("RunSummary"));

  // check if run summary is valid
  if (!runSummary) {
    cout << endl << "**INFO** Couldn't obtain run summary info from ROOT file " << fRunPathName.Data() << endl;
    // this is not fatal... only RA-HV values are not available
  } else { // it follows a (at least) little bit strange extraction of the RA values from Thomas' TObjArray...
           //streaming of a ASCII-file would be more easy
    TString s, tok;
    TObjArrayIter summIter(runSummary);
    TObjString *os(dynamic_cast<TObjString*>(summIter.Next()));
    TObjArray *oa(0);
    TObjString *objTok(0);
    while (os != 0) {
      s = os->GetString();
      // will put four parallel if's since it may be that more than one RA-values are on one line
      if (s.Contains("RA-L")) {
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0) {
          if (!objTok->GetString().CompareTo("RA-L")) {
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if (objTok != 0 && !objTok->GetString().CompareTo("=")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.SetRingAnode(0, objTok->GetString().Atof()); // fill RA-R value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      if (s.Contains("RA-R")) {
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if (!objTok->GetString().CompareTo("RA-R")) {
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if (objTok != 0 && !objTok->GetString().CompareTo("=")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.SetRingAnode(1, objTok->GetString().Atof()); // fill RA-R value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      if (s.Contains("RA-T")) {
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if (!objTok->GetString().CompareTo("RA-T")) {
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if (objTok != 0 && !objTok->GetString().CompareTo("=")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.SetRingAnode(2, objTok->GetString().Atof()); // fill RA-T value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      if (s.Contains("RA-B")) {
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if (!objTok->GetString().CompareTo("RA-B")) {
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if (objTok != 0 && !objTok->GetString().CompareTo("=")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.SetRingAnode(3, objTok->GetString().Atof()); // fill RA-B value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      os = dynamic_cast<TObjString*>(summIter.Next()); // next summary line...
    }
  }

  // read data ---------------------------------------------------------
  // check if histos folder is found
  f.GetObject("histos", folder);
  if (!folder) {
    cerr << endl << "PRunDataHandler::ReadRootFile: **ERROR** Couldn't obtain histos from " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }
  // get all the data
  Char_t histoName[32];
  if (notPostPileup) { // read the data which are NOT post pileup corrected
    for (Int_t i=0; i<noOfHistos; i++) {
      sprintf(histoName, "hDecay%02d", i);
      TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
      if (!histo) {
        cerr << endl << "PRunDataHandler::ReadRootFile: **ERROR** Couldn't get histo " << histoName;
        cerr << endl;
        return false;
      }
      // keep maximum of histogram as a T0 estimate
      runData.AppendT0Estimated(histo->GetMaximumBin());
      // fill data
      for (Int_t j=1; j<histo->GetNbinsX(); j++) {
        histoData.push_back(histo->GetBinContent(j));
      }
      // store them in runData vector
      runData.AppendDataBin(histoData);
      // clear histoData for the next histo
      histoData.clear();
    }
  } else { // read the data which ARE post pileup corrected
    for (Int_t i=0; i<noOfHistos; i++) {
      sprintf(histoName, "hDecay%02d", i+POST_PILEUP_HISTO_OFFSET);
      TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
      if (!histo) {
        cerr << endl << "PRunDataHandler::ReadRootFile: **ERROR** Couldn't get histo " << histoName;
        cerr << endl;
        return false;
      }
      // keep maximum of histogram as a T0 estimate
      runData.AppendT0Estimated(histo->GetMaximumBin());
      // fill data
      for (Int_t j=1; j<histo->GetNbinsX(); j++)
        histoData.push_back(histo->GetBinContent(j));
      // store them in runData vector
      runData.AppendDataBin(histoData);
      // clear histoData for the next histo
      histoData.clear();
    }
  }

  f.Close();

  // keep run name
  runData.SetRunName(fRunName);

  // add run to the run list
  fData.push_back(runData);

  return true;
}

//--------------------------------------------------------------------------
// ReadNexusFile
//--------------------------------------------------------------------------
/**
 * <p> Will read the NeXuS File Format as soon as PSI will have an implementation.
 *
 *
 * <b>return:</b> true at successful reading, otherwise false.
 */
Bool_t PRunDataHandler::ReadNexusFile()
{
  cout << endl << "PRunDataHandler::ReadNexusFile(): Sorry, not yet implemented, ask Alex Amato ...";
  return false;
}

//--------------------------------------------------------------------------
// ReadWkmFile
//--------------------------------------------------------------------------
/**
 * <p> Reads, for backwards compatibility, the ascii-wkm-file data format.
 * The routine is clever enough to distinguish the different wkm-flavours (argh).
 *
 * <b>return:</b> true at successful reading, otherwise false.
 */
Bool_t PRunDataHandler::ReadWkmFile()
{
  PDoubleVector histoData;
  PRawRunData runData;

  // open file
  ifstream f;

  // open wkm-file
  f.open(fRunPathName.Data(), ifstream::in);
  if (!f.is_open()) {
    cerr << endl << "PRunDataHandler::ReadWkmFile: **ERROR** Couldn't open run data (" << fRunPathName.Data() << ") file for reading, sorry ...";
    cerr << endl;
    return false;
  }

  // read header
  Bool_t    headerInfo = true;
  Char_t    instr[512];
  TString line, linecp;
  Double_t  dval;
  Int_t     ival;
  Bool_t    ok;
  Int_t     groups = 0, channels = 0;

  // skip leading empty lines
  do {
    f.getline(instr, sizeof(instr));
    line = TString(instr);
    if (!line.IsWhitespace())
      break;
  } while (!f.eof());

  // real header data should start here
  Ssiz_t idx;
  do {
    line = TString(instr);
    if (line.IsDigit()) { // end of header reached
      headerInfo = false;
    } else { // real stuff, hence filter data
      if (line.Contains("Title") || line.Contains("Titel")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Title:'
        StripWhitespace(line);
        runData.SetRunTitle(line);
      } else if (line.Contains("Field")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Field:'
        StripWhitespace(line);
        idx = line.Index("G"); // check if unit is given
        if (idx > 0) // unit is indeed given
          line.Resize(idx);
        dval = ToDouble(line, ok);
        if (ok)
          runData.SetField(dval);
      } else if (line.Contains("Setup")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Setup:'
        StripWhitespace(line);
        runData.SetSetup(line);
      } else if (line.Contains("Temp:") || line.Contains("Temp(meas1):")) {
        linecp = line;
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Temp:'
        StripWhitespace(line);
        idx = line.Index("+/-"); // remove "+/- ..." part
        if (idx > 0)
          line.Resize(idx);
        idx = line.Index("K"); // remove "K ..." part
        if (idx > 0)
          line.Resize(idx);
        dval = ToDouble(line, ok);
        if (ok) {
          runData.SetTemperature(0, dval, 0.0);
        }
        idx = linecp.Index("+/-"); // get the error
        linecp.Replace(0, idx+3, 0, 0);
        StripWhitespace(linecp);
        dval = ToDouble(linecp, ok);
        if (ok) {
          runData.SetTempError(0, dval);
        }
      } else if (line.Contains("Temp(meas2):")) {
        linecp = line;
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Temp(meas2):'
        StripWhitespace(line);
        idx = line.Index("+/-"); // remove "+/- ..." part
        if (idx > 0)
          line.Resize(idx);
        idx = line.Index("K"); // remove "K ..." part
        if (idx > 0)
          line.Resize(idx);
        dval = ToDouble(line, ok);
        if (ok) {
          runData.SetTemperature(1, dval, 0.0);
        }
        idx = linecp.Index("+/-"); // get the error
        linecp.Replace(0, idx+3, 0, 0);
        StripWhitespace(linecp);
        dval = ToDouble(linecp, ok);
        if (ok) {
          runData.SetTempError(1, dval);
        }
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
          runData.SetTimeResolution(dval * 1000.0);
      }
    }

    if (headerInfo)
      f.getline(instr, sizeof(instr));
  } while (headerInfo && !f.eof());

  if ((groups == 0) || (channels == 0) || runData.GetTimeResolution() == 0.0) {
    cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** essential header informations are missing!";
    cerr << endl << "  >> groups          = " << groups;
    cerr << endl << "  >> channels        = " << channels;
    cerr << endl << "  >> time resolution = " << runData.GetTimeResolution();
    cerr << endl;
    f.close();
    return false;
  }

  // read data ---------------------------------------------------------
  UInt_t group_counter = 0;
  Int_t val;
  TObjArray *tokens;
  TObjString *ostr;
  TString str;
  do {
    // check if empty line, i.e. new group
    if (IsWhitespace(instr)) {
      runData.AppendDataBin(histoData);
      // get a T0 estimate
      Double_t maxVal = 0.0;
      Int_t maxBin = 0;
      for (UInt_t i=0; i<histoData.size(); i++) {
        if (histoData[i] > maxVal) {
          maxVal = histoData[i];
          maxBin = i;
        }
      }
      runData.AppendT0Estimated(maxBin);
      histoData.clear();
      group_counter++;
    } else {
      // extract values
      line = TString(instr);
      // check if line starts with character. Needed for RAL WKM format
      if (!line.IsDigit()) {
        f.getline(instr, sizeof(instr));
        continue;
      }
      tokens = line.Tokenize(" ");

      if (!tokens) { // no tokens found
        cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: coulnd't tokenize run data.";
        return false;
      }
      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        val = ToInt(str, ok);
        if (ok) {
          histoData.push_back(val);
        } else {
          cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: data line contains non-integer values.";
          // clean up
          delete tokens;
          return false;
        }
      }
      // clean up
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    }

    f.getline(instr, sizeof(instr));

  } while (!f.eof());

  // handle last line if present
  if (strlen(instr) != 0) {
    // extract values
    line = TString(instr);
    tokens = line.Tokenize(" ");
    if (!tokens) { // no tokens found
      cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: coulnd't tokenize run data.";
      return false;
    }
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString();
      val = ToInt(str, ok);
      if (ok) {
        histoData.push_back(val);
      } else {
        cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: data line contains non-integer values.";
        // clean up
        delete tokens;
        return false;
      }
    }
    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }

  // save the last histo if not empty
  if (histoData.size() > 0) {
    runData.AppendDataBin(histoData);
    histoData.clear();
  }

  // close file
  f.close();

  // check if all groups are found
  if ((Int_t) runData.GetNoOfHistos() != groups) {
    cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR**";
    cerr << endl << "  expected " << groups << " histos, but found " << runData.GetNoOfHistos();
    return false;
  }

  // check if all groups have enough channels
  for (UInt_t i=0; i<runData.GetNoOfHistos(); i++) {
    if ((Int_t) runData.GetDataBin(i)->size() != channels) {
      cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR**";
      cerr << endl << "  expected " << channels << " bins in histo " << i << ", but found " << runData.GetDataBin(i)->size();
      return false;
    }
  }

  // keep run name
  runData.SetRunName(fRunName);

  // add run to the run list
  fData.push_back(runData);

  return true;
}

//--------------------------------------------------------------------------
// ReadPsiBinFile
//--------------------------------------------------------------------------
/**
 * <p> Reads the old-fashioned PSI-BIN data-files. The MuSR_td_PSI_bin class
 * of Alex Amato is used. In case of problems, please contact alex.amato@psi.ch.
 *
 * <b>return:</b> true at successful reading, otherwise false.
 */
Bool_t PRunDataHandler::ReadPsiBinFile()
{
  MuSR_td_PSI_bin psiBin;
  Int_t status;
  Bool_t success;

  // read psi bin file
  status = psiBin.read(fRunPathName.Data());
  switch (status) {
    case 0: // everything perfect
      success = true;
      break;
    case 1: // couldn't open file, or failed while reading the header
      cerr << endl << "**ERROR** couldn't open psibin file, or failed while reading the header";
      cerr << endl;
      success = false;
      break;
    case 2: // unsupported version of the data
      cerr << endl << "**ERROR** psibin file: unsupported version of the data";
      cerr << endl;
      success = false;
      break;
    case 3: // error when allocating data buffer
      cerr << endl << "**ERROR** psibin file: error when allocating data buffer";
      cerr << endl;
      success = false;
      break;
    case 4: // number of histograms/record not equals 1
      cerr << endl << "**ERROR** psibin file: number of histograms/record not equals 1";
      cerr << endl;
      success = false;
      break;
    default: // you never should have reached this point
      success = false;
      break;
  }

  // if any reading error happend, get out of here
  if (!success)
    return success;

  // fill necessary header informations
  PIntVector ivec;
  PRawRunData runData;
  Double_t dval;
  // keep run name
  runData.SetRunName(fRunName);
  // get run title
  runData.SetRunTitle(TString(psiBin.get_comment().c_str())); // run title
  // get setup
  runData.SetSetup(TString(psiBin.get_orient().c_str()));
  // set LEM specific information to default value since it is not in the file and not used...
  runData.SetEnergy(PMUSR_UNDEFINED);
  runData.SetTransport(PMUSR_UNDEFINED);
  // get field
  status = sscanf(psiBin.get_field().c_str(), "%lfG", &dval);
  if (status == 1)
    runData.SetField(dval);
  // get temperature
  PDoubleVector tempVec(psiBin.get_temperatures_vector());
  PDoubleVector tempDevVec(psiBin.get_devTemperatures_vector());
  if ((tempVec.size() > 1) && (tempDevVec.size() > 1) && tempVec[0] && tempVec[1]) {
    // take only the first two values for now...
    //maybe that's not enough - e.g. in older GPD data I saw the "correct values in the second and third entry..."
    for (UInt_t i(0); i<2; i++) {
      runData.SetTemperature(i, tempVec[i], tempDevVec[i]);
    }
    tempVec.clear();
    tempDevVec.clear();
  } else {
    status = sscanf(psiBin.get_temp().c_str(), "%lfK", &dval);
    if (status == 1)
      runData.SetTemperature(0, dval, 0.0);
  }

  // get time resolution (ns)
  runData.SetTimeResolution(psiBin.get_binWidth_ns());
  // get t0's
  ivec = psiBin.get_t0_vector();
  if (ivec.empty()) {
    cerr << endl << "**ERROR** psibin file: couldn't obtain any t0's";
    cerr << endl;
    return false;
  }
  for (UInt_t i=0; i<ivec.size(); i++)
    runData.AppendT0(ivec[i]);

  // fill raw data
  PDoubleVector histoData;
  Int_t *histo;
  for (Int_t i=0; i<psiBin.get_numberHisto_int(); i++) {
    histo = psiBin.get_histo_array_int(i);
    for (Int_t j=0; j<psiBin.get_histoLength_bin(); j++) {
      histoData.push_back(histo[j]);
    }
    delete[] histo;
    runData.AppendDataBin(histoData);
    // estimate T0 from maximum of the data
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t j=0; j<histoData.size(); j++) {
      if (histoData[j] > maxVal) {
        maxVal = histoData[j];
        maxBin = j;
      }
    }
    runData.AppendT0Estimated(maxBin);
    histoData.clear();
  }

  // add run to the run list
  fData.push_back(runData);

  return success;
}

//--------------------------------------------------------------------------
// ReadMudFile
//--------------------------------------------------------------------------
/**
 * <p> Reads the triumf mud-file format. <b>Not yet implemented, sorry</b>.
 *
 * <b>return:</b> true at successful reading, otherwise false.
 */
Bool_t PRunDataHandler::ReadMudFile()
{
  Int_t    fh;
  UINT32   type, val;
  Int_t    success;
  Char_t   str[1024];
  Double_t dval;

  PRawRunData runData;

  fh = MUD_openRead((Char_t *)fRunPathName.Data(), &type);
  if (fh == -1) {
    cerr << endl << "**ERROR** Couldn't open mud-file " << fRunPathName.Data() << ", sorry.";
    cerr << endl;
    return false;
  }

  // read necessary header information

  // keep run name
  runData.SetRunName(fRunName);

  // get run title
  success = MUD_getTitle( fh, str, sizeof(str) );
  if ( !success ) {
    cerr << endl << "**WARNING** Couldn't obtain the run title of run " << fRunName.Data();
    cerr << endl;
  }
  runData.SetRunTitle(TString(str));

  // get setup
  TString setup;
  success = MUD_getLab( fh, str, sizeof(str) );
  if (success) {
    setup = TString(str) + TString("/");
  }
  success = MUD_getArea( fh, str, sizeof(str) );
  if (success) {
    setup += TString(str) + TString("/");
  }
  success = MUD_getApparatus( fh, str, sizeof(str) );
  if (success) {
    setup += TString(str) + TString("/");
  }
  success = MUD_getSample( fh, str, sizeof(str) );
  if (success) {
    setup += TString(str);
  }
  runData.SetSetup(setup);

  // set LEM specific information to default value since it is not in the file and not used...
  runData.SetEnergy(PMUSR_UNDEFINED);
  runData.SetTransport(PMUSR_UNDEFINED);

  // get field
  success = MUD_getField( fh, str, sizeof(str) );
  if (success) {
    success = sscanf(str, "%lf G", &dval);
    if (success == 1) {
      runData.SetField(dval);
    } else {
      runData.SetField(PMUSR_UNDEFINED);
    }
  } else {
    runData.SetField(PMUSR_UNDEFINED);
  }

  // get temperature
  success = MUD_getTemperature( fh, str, sizeof(str) );
  if (success) {
    success = sscanf(str, "%lf K", &dval);
    if (success == 1) {
      runData.SetTemperature(0, dval, 0.0);
    } else {
      runData.SetTemperature(0, PMUSR_UNDEFINED, 0.0);
    }
  } else {
    runData.SetTemperature(0, PMUSR_UNDEFINED, 0.0);
  }

  // get number of histogramms
  success = MUD_getHists(fh, &type, &val);
  if ( !success ) {
    cerr << endl << "**ERROR** Couldn't obtain the number of histograms of run " << fRunName.Data();
    cerr << endl;
    MUD_closeRead(fh);
    return false;
  }
  Int_t noOfHistos = (Int_t)val;

  // get time resolution (ns)
  // check that time resolution is identical for all histograms
  // >> currently it is not forseen to handle histos with different time resolutions <<
  // >> perhaps this needs to be reconsidered later on                               <<
  UINT32 fsTimeResolution = 0;
  for (Int_t i=1; i<=noOfHistos; i++) {
    success = MUD_getHistFsPerBin( fh, i, &val );
    if (!success) {
      cerr << endl << "**ERROR** Couldn't obtain the time resolution of run " << fRunName.Data();
      cerr << endl << "          which is fatal, sorry.";
      cerr << endl;
      MUD_closeRead(fh);
      return false;
    }
    if (i==1) {
      fsTimeResolution = val;
    } else {
      if (val != fsTimeResolution) {
        cerr << endl << "**ERROR** various time resolutions found in run " << fRunName.Data();
        cerr << endl << "          this is currently not supported, sorry.";
        cerr << endl;
        MUD_closeRead(fh);
        return false;
      }
    }
  }
  runData.SetTimeResolution((Double_t)fsTimeResolution / 1.0e6); // fs -> ns

  // read histograms
  pair<Int_t, Int_t> valPair;
  UINT32 *pData; // histo memory
  pData = NULL;
  PDoubleVector histoData;
  UInt_t noOfBins;

  for (Int_t i=1; i<=noOfHistos; i++) {

    // get t0's
    success = MUD_getHistT0_Bin( fh, i, &val );
    if ( !success ) {
      cerr << endl << "**WARNING** Couldn't get t0 of histo " << i << " of run " << fRunName.Data();
      cerr << endl;
    }
    runData.AppendT0((Int_t)val);

    // get bkg bins
    success = MUD_getHistBkgd1( fh, i, &val );
    if ( !success ) {
      cerr << endl << "**WARNING** Couldn't get bkg bin 1 of histo " << i << " of run " << fRunName.Data();
      cerr << endl;
      valPair.first = -1;
    } else {
      valPair.first = (Int_t)val;
    }

    success = MUD_getHistBkgd2( fh, i, &val );
    if ( !success ) {
      cerr << endl << "**WARNING** Couldn't get bkg bin 2 of histo " << i << " of run " << fRunName.Data();
      cerr << endl;
      valPair.second = -1;
    }
    valPair.second = (Int_t)val;

    if ((valPair.first != -1) && (valPair.second != -1)) { // bkg bin1 && bkg bin2 found
      runData.AppendBkgBin(valPair);
    }

    // get good data bins
    success = MUD_getHistGoodBin1( fh, i, &val );
    if ( !success ) {
      cerr << endl << "**WARNING** Couldn't get good bin 1 of histo " << i << " of run " << fRunName.Data();
      cerr << endl;
      valPair.first = -1;
    } else {
      valPair.first = (Int_t)val;
    }

    success = MUD_getHistGoodBin2( fh, i, &val );
    if ( !success ) {
      cerr << endl << "**WARNING** Couldn't get good bin 2 of histo " << i << " of run " << fRunName.Data();
      cerr << endl;
      valPair.second = -1;
    }
    valPair.second = (Int_t)val;

    if ((valPair.first != -1) && (valPair.second != -1)) { // good bin1 && good bin2 found
      runData.AppendGoodDataBin(valPair);
    }

    // get number of bins
    success = MUD_getHistNumBins( fh, i, &val );
    if ( !success ) {
      cerr << endl << "**ERROR** Couldn't get the number of bins of histo " << i << ".";
      cerr << endl << "          This is fatal, sorry.";
      cerr << endl;
      MUD_closeRead( fh );
      return false;
    }
    noOfBins = (UInt_t)val;

    pData = (UINT32*)malloc(noOfBins*sizeof(pData));
    if (pData == NULL) {
      cerr << endl << "**ERROR** Couldn't allocate memory for data.";
      cerr << endl << "          This is fatal, sorry.";
      cerr << endl;
      MUD_closeRead( fh );
      return false;
    }

    // get histogram
    success = MUD_getHistData( fh, i, pData );
    if ( !success ) {
      cerr << endl << "**ERROR** Couldn't get histo no " << i << ".";
      cerr << endl << "          This is fatal, sorry.";
      cerr << endl;
      MUD_closeRead( fh );
      return false;
    }

    for (UInt_t j=0; j<noOfBins; j++) {
      histoData.push_back(pData[j]);
    }
    runData.AppendDataBin(histoData);
    // estimate T0 from maximum of the data
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t j=0; j<histoData.size(); j++) {
      if (histoData[j] > maxVal) {
        maxVal = histoData[j];
        maxBin = j;
      }
    }
    runData.AppendT0Estimated(maxBin);
    histoData.clear();

    free(pData);
  }

  MUD_closeRead(fh);

/*
cout << endl << "fRunName        : " << runData.fRunName.Data();
cout << endl << "fRunTitle       : " << runData.fRunTitle.Data();
cout << endl << "fSetup          : " << runData.fSetup.Data();
cout << endl << "fField          : " << runData.fField;
cout << endl << "fTemp           : " << runData.fTemp[0].first;
cout << endl << "noOfHistos      : " << noOfHistos;
cout << endl << "fTimeResolution : " << runData.fTimeResolution;
for (Int_t i=0; i<noOfHistos; i++) {
  cout << endl << "------";
  cout << endl << i << " : t0        = " << runData.fT0s[i];
  cout << endl << i << " : bkg bins  = " << runData.fBkgBin[i].first << "..." << runData.fBkgBin[i].second;
  cout << endl << i << " : good bins = " << runData.fGoodDataBin[i].first << "..." << runData.fGoodDataBin[i].second;
}
cout << endl;
*/

  // add run to the run list
  fData.push_back(runData);

  return true;
}

//--------------------------------------------------------------------------
// ReadMduAsciiFile
//--------------------------------------------------------------------------
/**
 * <p>Reads the mdu ascii files (PSI). Needed to work around PSI-BIN limitations.
 *
 * <p>Lines starting with '#' or '%' are considered as comment lines. The file has 
 * the following structure:
 * \verbatim
 * HEADER
 * TITLE: title-string
 * SETUP: setup-string
 * FIELD: val-string (G) or (T) (e.g. 123456 (G))
 * TEMP: val-string (K)
 * GROUPS: # of histograms written
 * CHANNELS: # of bins per histogram written
 * RESOLUTION: timeresolution (fs) or (ps) or (ns) or (us)
 * \endverbatim
 * followed by the data, which are written in column format, starting with the DATA
 * tag, i.e.
 * \verbatim
 * DATA
 * 
 * \endverbatim
 *
 */
Bool_t PRunDataHandler::ReadMduAsciiFile()
{
  Bool_t success = true;
  
  // open file
  ifstream f;

  // open data-file
  f.open(fRunPathName.Data(), ifstream::in);
  if (!f.is_open()) {
    cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** Couldn't open data file (" << fRunPathName.Data() << ") for reading, sorry ...";
    cerr << endl;
    return false;
  }

  PRawRunData runData;
  
  // keep run name
  runData.SetRunName(fRunName);
  
  Int_t     lineNo = 0;
  Char_t    instr[512];
  TString line, workStr;
  Bool_t    headerTag = false;
  Bool_t    dataTag = false;
  Int_t     dataLineCounter = 0;
  TObjString *ostr;
  TObjArray *tokens = 0;
  TString str;
  Int_t groups = 0;
  Int_t channels = 0;
  Double_t dval = 0.0, unitScaling = 0.0;
  vector<PDoubleVector> data;
  
  while (!f.eof()) {
    f.getline(instr, sizeof(instr));
    line = TString(instr);
    lineNo++;

    // ignore comment lines
    if (line.BeginsWith("#") || line.BeginsWith("%"))
      continue;

    // ignore empty lines
    if (line.IsWhitespace())
      continue;
        
    // check if header tag
    workStr = line;
    workStr.Remove(TString::kLeading, ' '); // remove spaces from the begining
    if (workStr.BeginsWith("header", TString::kIgnoreCase)) {
      headerTag = true;
      dataTag = false;
      continue;
    }

    // check if data tag
    workStr = line;
    workStr.Remove(TString::kLeading, ' '); // remove spaces from the beining
    if (workStr.BeginsWith("data", TString::kIgnoreCase)) {
      headerTag = false;
      dataTag = true;
      continue;
    }
    
    if (headerTag) {
      workStr = line;
      workStr.Remove(TString::kLeading, ' '); // remove spaces from the beining
      if (workStr.BeginsWith("title:", TString::kIgnoreCase)) {
        runData.SetRunTitle(TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("field:", TString::kIgnoreCase)) {
        tokens = workStr.Tokenize(":("); // field: val (units)
        // check if expected number of tokens present
        if (tokens->GetEntries() != 3) {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", invalid field entry in header.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        // check if field value is a number
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        if (ostr->GetString().IsFloat()) {
          dval = ostr->GetString().Atof();
        } else {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", field value is not float/doulbe.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        // check units, accept (G), (T)
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        if (ostr->GetString().Contains("G"))
          unitScaling = 1.0;
        else if (ostr->GetString().Contains("T"))
          unitScaling = 1.0e4;
        else {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", unkown field units.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        runData.SetField(dval*unitScaling);  
        
        // clean up tokens
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
      } else if (workStr.BeginsWith("temp:", TString::kIgnoreCase)) {
        tokens = workStr.Tokenize(":("); // temp: val (units)
        // check if expected number of tokens present
        if (tokens->GetEntries() != 3) {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", invalid temperatue entry in header.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        // check if field value is a number
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        if (ostr->GetString().IsFloat()) {
          dval = ostr->GetString().Atof();
        } else {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", temperature value is not float/doulbe.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        runData.SetTemperature(0, dval, 0.0);  
        
        // clean up tokens
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
      } else if (workStr.BeginsWith("setup:", TString::kIgnoreCase)) {
        runData.SetSetup(TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("groups:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        groups = workStr.Atoi();      
        if (groups == 0) {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", groups is not a number or 0.";
          cerr << endl;
          success = false;
          break;
        }
        data.resize(groups);
      } else if (workStr.BeginsWith("channels:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        channels = workStr.Atoi();
        if (channels == 0) {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", channels is not a number or 0.";
          cerr << endl;
          success = false;
          break;
        }
      } else if (workStr.BeginsWith("resolution:", TString::kIgnoreCase)) {
        tokens = workStr.Tokenize(":("); // resolution: val (units)
        // check if expected number of tokens present
        if (tokens->GetEntries() != 3) {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", invalid time resolution entry in header.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        // check if timeresolution value is a number
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        if (ostr->GetString().IsFloat()) {
          dval = ostr->GetString().Atof();
        } else {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", time resolution value is not float/doulbe.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        // check units, accept (fs), (ps), (ns), (us)
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        if (ostr->GetString().Contains("fs"))
          unitScaling = 1.0e-6;
        else if (ostr->GetString().Contains("ps"))
          unitScaling = 1.0e-3;
        else if (ostr->GetString().Contains("ns"))
          unitScaling = 1.0;
        else if (ostr->GetString().Contains("us"))
          unitScaling = 1.0e3;
        else {
          cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", unkown time resolution units.";
          cerr << endl << line.Data();
          cerr << endl;
          success = false;
          break;
        }
        runData.SetTimeResolution(dval*unitScaling);  
        
        // clean up tokens
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
      } else { // error
        cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", illegal header line.";
        cerr << endl;
        success = false;
        break;
      }
    } else if (dataTag) {
      dataLineCounter++;
      tokens = line.Tokenize(" ,\t");
      // check if the number of data line entries is correct
      if (tokens->GetEntries() != groups+1) {
        cerr << endl << "PRunDataHandler::ReadMduAsciiFile **ERROR** found data line with a wrong data format, cannot be handled (line no " << lineNo << ")";
        cerr << endl << "line:";
        cerr << endl << line.Data();
        cerr << endl;
        success = false;
        break;
      }
      
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        data[i-1].push_back(ostr->GetString().Atof());
      }
      
      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    }
  }
  
  f.close();

  // keep data
  for (UInt_t i=0; i<data.size(); i++) {
    runData.AppendDataBin(data[i]);
  }
  
  // estimate T0 from maximum of the data
  for (UInt_t i=0; i<data.size(); i++) {
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t j=0; j<data[i].size(); j++) {
      if (data[i][j] > maxVal) {
        maxVal = data[i][j];
        maxBin = j;
      }
    }
    runData.AppendT0Estimated(maxBin);
  }

  // clean up
  for (UInt_t i=0; i<data.size(); i++) 
    data[i].clear();
  data.clear();  
  
  if (dataLineCounter != channels) {
    cerr << endl << "PRunDataHandler::ReadMduAsciiFile **WARNING** found " << dataLineCounter << " data bins,";
    cerr << endl << "expected " << channels << " according to the header." << endl;
  }

  fData.push_back(runData);
  
  return success;
}
 
//--------------------------------------------------------------------------
// ReadAsciiFile
//--------------------------------------------------------------------------
/**
 * <p>Reads ascii files. Intended for the nonMuSR data.
 *
 * The file format definition is:
 * Comment lines start with a '#' or '%' character.
 * The file can start with some header info. The header is optional, as all its tags, but 
 * if present it has the following format:
 *
 * \verbatim
 * HEADER
 * TITLE:  title
 * X-AXIS-TITLE: x-axis title
 * Y-AXIS-TITLE: y-axis title
 * SETUP:  setup
 * FIELD:  field
 * TEMP:   temperature
 * ENERGY: energy
 * \endverbatim
 *
 * field is assumed to be given in (G), the temperature in (K), the energy in (keV)
 *
 * The data are read column like and start with the data tag DATA, followed by the
 * data columns, i.e.:
 *
 * \verbatim
 * DATA
 * x, y [, error y]
 * \endverbatim
 *
 * where spaces, column, are a tab are possible separations.
 * If no error in y is present, the weighting in the fit will be equal.
 *
 * <b>return:</b> true at successful reading, otherwise false.
 */
Bool_t PRunDataHandler::ReadAsciiFile()
{
  Bool_t success = true;

  // open file
  ifstream f;

  // open data-file
  f.open(fRunPathName.Data(), ifstream::in);
  if (!f.is_open()) {
    cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** Couldn't open data file (" << fRunPathName.Data() << ") for reading, sorry ...";
    cerr << endl;
    return false;
  }

  PRawRunData runData;

  // init some stuff
  runData.fDataNonMusr.SetFromAscii(true);
  runData.fDataNonMusr.AppendLabel("??"); // x default label
  runData.fDataNonMusr.AppendLabel("??"); // y default label

  runData.SetRunName(fRunName); // keep the run name

  Int_t     lineNo = 0;
  Char_t    instr[512];
  TString line, workStr;
  Bool_t    headerTag = false;
  Bool_t    dataTag = false;
  Double_t  x, y, ey;
  PDoubleVector xVec, exVec, yVec, eyVec;

  while (!f.eof()) {
    f.getline(instr, sizeof(instr));
    line = TString(instr);
    lineNo++;

    // check if comment line
    if (line.BeginsWith("#") || line.BeginsWith("%"))
      continue;

    // check if header tag
    workStr = line;
    workStr.Remove(TString::kLeading, ' '); // remove spaces from the begining
    if (workStr.BeginsWith("header", TString::kIgnoreCase)) {
      headerTag = true;
      dataTag = false;
      continue;
    }

    // check if data tag
    workStr = line;
    workStr.Remove(TString::kLeading, ' '); // remove spaces from the beining
    if (workStr.BeginsWith("data", TString::kIgnoreCase)) {
      headerTag = false;
      dataTag = true;
      continue;
    }

    if (headerTag) {
      if (line.IsWhitespace()) // ignore empty lines
        continue;
      workStr = line;
      workStr.Remove(TString::kLeading, ' '); // remove spaces from the beining
      if (workStr.BeginsWith("title:", TString::kIgnoreCase)) {
        runData.SetRunTitle(TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("setup:", TString::kIgnoreCase)) {
        runData.SetSetup(TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("field:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", field is not a number.";
          cerr << endl;
          success = false;
          break;
        }
        runData.SetField(workStr.Atof());
      } else if (workStr.BeginsWith("x-axis-title:", TString::kIgnoreCase)) {
        runData.fDataNonMusr.SetLabel(0, TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("y-axis-title:", TString::kIgnoreCase)) {
        runData.fDataNonMusr.SetLabel(1, TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("temp:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", temperature is not a number.";
          cerr << endl;
          success = false;
          break;
        }
        runData.SetTemperature(0, workStr.Atof(), 0.0);
      } else if (workStr.BeginsWith("energy:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", energy is not a number.";
          cerr << endl;
          success = false;
          break;
        }
        runData.SetEnergy(workStr.Atof());
        runData.SetTransport(PMUSR_UNDEFINED); // just to initialize the variables to some "meaningful" value
      } else { // error
        cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", illegal header line.";
        cerr << endl;
        success = false;
        break;
      }
    } else if (dataTag) {
      if (line.IsWhitespace()) // ignore empty lines
        continue;
      TObjString *ostr;
      TObjArray *tokens;
      // check if data have x, y [, error y] structure, and that x, y, and error y are numbers
      tokens = line.Tokenize(" ,\t");
      // check if the number of data line entries is 2 or 3
      if ((tokens->GetEntries() != 2) && (tokens->GetEntries() != 3)) {
        cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** found data line with a structure different than \"x, y [, error y]\", cannot be handled (line no " << lineNo << ")";
        cerr << endl;
        success = false;
        break;
      }

      // get x
      ostr = dynamic_cast<TObjString*>(tokens->At(0));
      if (!ostr->GetString().IsFloat()) {
        cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ": x = " << ostr->GetString().Data() << " is not a number, sorry.";
        cerr << endl;
        success = false;
        break;
      }
      x = ostr->GetString().Atof();

      // get y
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      if (!ostr->GetString().IsFloat()) {
        cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ": y = " << ostr->GetString().Data() << " is not a number, sorry.";
        cerr << endl;
        success = false;
        break;
      }
      y = ostr->GetString().Atof();

      // get error y if present
      if (tokens->GetEntries() == 3) {
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        if (!ostr->GetString().IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ": error y = " << ostr->GetString().Data() << " is not a number, sorry.";
          cerr << endl;
          success = false;
          break;
        }
        ey = ostr->GetString().Atof();
      } else {
        ey = 1.0;
      }

      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }

      // keep values
      xVec.push_back(x);
      exVec.push_back(1.0);
      yVec.push_back(y);
      eyVec.push_back(ey);

    } else {
      cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << " neither header nor data line. No idea how to handle it!";
      cerr << endl;
      success = false;
      break;
    }
  }

  f.close();

  // keep values
  runData.fDataNonMusr.AppendData(xVec);
  runData.fDataNonMusr.AppendErrData(exVec);
  runData.fDataNonMusr.AppendData(yVec);
  runData.fDataNonMusr.AppendErrData(eyVec);

  fData.push_back(runData);

  // clean up
  xVec.clear();
  exVec.clear();
  yVec.clear();
  eyVec.clear();

  return success;
}

//--------------------------------------------------------------------------
// ReadDBFile
//--------------------------------------------------------------------------
/**
 * <p>Reads triumf db-files. Intended for the nonMuSR data.
 *
 * <p>The file format definition is:
 * The following is a description of the features of the TRIUMF .db file format that are 
 * currently recognized by musrfit/musrview. The available commands include: title, abstract, 
 * comments, labels, and data.
 *
 * \verbatim
 * TITLE
 *  The following line must contain the title.
 *
 *
 * ABSTRACT
 *  The abstract is read in starting with the following line until an empty line is reached.
 *
 * COMMENTS
 *  The comments are read in starting with the following line until an empty line is reached.
 *
 *
 * LABELS
 *  One label must occupy each subsequent line until an empty line is reached. The number 
 *  of labels should preferably match the number of variables in the data.
 *
 * DATA
 *  On the same line as the DATA command, there must appear a comma-delimited list of variable 
 *  names. These names should be kept short (some applications truncate to 4 characters). The 
 *  numerical data is read in beginning with the following line until an empty line is reached.
 *
 *  In every line, there must appear exactly 3 comma-delimited fields for each specified name. 
 *  The first field is the value, the second is the positive error, and the third is the negative 
 *  error. If you leave the last field blank (the comma is still required), then the positive error 
 *  will be interpreted as a symmetric error. If you include only the value, then the errors will be 
 *  set to zero.
 *
 * To reiterate, if you provide a DATA command with 2 names, e.g. "DATA 1st, 2nd", then every subsequent 
 * line must contain 2*3 - 1 = 5 commas. If you give 3 names, then there must be 3*3 - 1 = 8 commas.
 * 
 * Example 
 * TITLE
 *  Most Excellent Fake Data
 *
 * ABSTRACT
 *  This data was collected over
 *  many minutes of light work
 *  that was required to make it up.
 *
 * COMMENTS
 *  This data was generated using C++.
 *  The file was formatted with Emacs.
 *
 * LABEL
 *  Randomized Linear
 *  Randomized Gaussian
 *  Randomized Lorentzian
 *  Run
 *
 * DATA line, gauss, lrntz, run
 * -1.966, -0.168, -0.106,  0.048, 0.002, 0.005,  0.184, 0.010, 0.017, 1001, , , run 1001 title
 * -1.895, -0.151, -0.128,  0.014, 0.001, 0.001,  0.259, 0.017, 0.015, 1002, , , run 1002 title
 * -1.836, -0.127, -0.184,  0.013, 0.001, 0.001,  0.202, 0.017, 0.020, 1003, , , run 1003 title
 * -1.739, -0.064, -0.166,  0.057, 0.003, 0.004,  0.237, 0.016, 0.018, 1004, , , run 1004 title
 * -1.601, -0.062, -0.147,  0.104, 0.008, 0.006,  0.271, 0.012, 0.025, 1005, , , run 1005 title
 *  .        .        .          .        .        .          .        .        .
 *  .        .        .          .        .        .          .        .        .
 *  .        .        .          .        .        .          .        .        .
 * Alternatively, the data often utilizes the continuation character ('\') and is labelled like
 * DATA line, gauss, lrntz
 * linear = -1.966,  -0.168,  -0.106, \
 * gaussn =  0.048,   0.002,   0.005, \
 * lorntz =  0.184,   0.010,   0.017, \
 * 1001,,, run 1001 title
 * linear = -1.895,  -0.151,  -0.128, \
 * gaussn =  0.014,   0.001,   0.001, \
 * lorntz =  0.259,   0.017,   0.015, |
 * 1002,,, run 1002 title
 * linear = -1.836,  -0.127,  -0.184, \
 * gaussn =  0.013,   0.001,   0.001, \
 * lorntz =  0.202,   0.017,   0.020, |
 * 1003,,, run 1003 title
 *   .        .        .        .
 *   .        .        .        .
 *   .        .        .        .
 * If there is a run line as in the above examples, it must be at the end of the data and given
 * in this just slight odd manner (do not blame me, I haven't invented this format ;-) ).
 * \endverbatim
 *
 * <p><b>WARNING:</b>For the row like data representation (the top DATA representation shown) 
 * it is essential that there are always at least one space inbetween commas (the reason is that
 * I am too lazy to write my own tokenizer), e.g.
 *
 * \verbatim
 * -1.966, -0.168, ,  0.048, , ,  0.184, 0.010, 0.017, 1001, , , run 1001 title
 * \endverbatim
 *
 * <p>which is ok, but
 *
 * \verbatim
 * -1.966, -0.168,,  0.048,,,  0.184, 0.010, 0.017, 1001,,, run 1001 title
 * \endverbatim
 *
 * <p>will <b>not</b> work!
 *
 * <p>Some db-files do have a '\-e' or '\e' label just between the DATA tag line and the real data.
 * This tag will just be ignored.
 */
Bool_t PRunDataHandler::ReadDBFile()
{
  Bool_t success = true;

  // open file
  ifstream f;

  // open db-file
  f.open(fRunPathName.Data(), ifstream::in);
  if (!f.is_open()) {
    cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** Couldn't open data file (" << fRunPathName.Data() << ") for reading, sorry ...";
    cerr << endl;
    return false;
  }

  PRawRunData runData;

  runData.fDataNonMusr.SetFromAscii(false);

  Int_t     lineNo = 0;
  Int_t     idx;
  Int_t     dbTag = -1;
  Char_t    instr[512];
  TString line, workStr;
  Double_t  val;
  Bool_t firstData = true; // needed as a switch to check in which format the data are given.
  Bool_t labelledFormat = true; // flag showing if the data are given in row format, or as labelled format (see description above, default is labelled format)

  // variables needed to tokenize strings
  TString tstr;
  TObjString *ostr;
  TObjArray *tokens = 0;

  while (!f.eof()) {
    // get next line from file
    f.getline(instr, sizeof(instr));
    line = TString(instr);
    lineNo++;

    // check if comment line
    if (line.BeginsWith("#") || line.BeginsWith("%"))
      continue;

    // ignore empty lines
    if (line.IsWhitespace())
      continue;

    // check for db specific tags
    workStr = line;
    workStr.Remove(TString::kLeading, ' '); // remove spaces from the begining
    if (workStr.BeginsWith("title", TString::kIgnoreCase)) {
      dbTag = 0;
      continue;
    } else if (workStr.BeginsWith("abstract", TString::kIgnoreCase)) {
      dbTag = 1;
      continue;
    } else if (workStr.BeginsWith("comments", TString::kIgnoreCase)) {
      dbTag = 2;
      continue;
    } else if (workStr.BeginsWith("label", TString::kIgnoreCase)) {
      dbTag = 3;
      continue;
    } else if (workStr.BeginsWith("data", TString::kIgnoreCase)) {
      dbTag = 4;

      // filter out all data tags
      tokens = workStr.Tokenize(" ,\t");
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        runData.fDataNonMusr.AppendDataTag(ostr->GetString());
      }

      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
      continue;
    }

    switch (dbTag) {
      case 0: // TITLE
        runData.SetRunTitle(workStr);
        break;
      case 1: // ABSTRACT
        // nothing to be done for now
        break;
      case 2: // COMMENTS
        // nothing to be done for now
        break;
      case 3: // LABEL
        runData.fDataNonMusr.AppendLabel(workStr);
        break;
      case 4: // DATA
        // filter out potential start data tag
        if (workStr.BeginsWith("\\-e", TString::kIgnoreCase) ||
            workStr.BeginsWith("\\e", TString::kIgnoreCase)  ||
            workStr.BeginsWith("/-e", TString::kIgnoreCase)  ||
            workStr.BeginsWith("/e", TString::kIgnoreCase)) {
          continue;
        }

        // check if first real data line
        if (firstData) {
          // check if data are given just as rows are as labelled columns (see description above)
          tokens = workStr.Tokenize(",");
          ostr = dynamic_cast<TObjString*>(tokens->At(0));
          if (!ostr->GetString().IsFloat()) {
            labelledFormat = true;
          } else {
            labelledFormat = false;
          }
          // clean up tokens
          if (tokens) {
            delete tokens;
            tokens = 0;
          }

          // prepare data vector for use
          PDoubleVector dummy;
          for (UInt_t i=0; i<runData.fDataNonMusr.GetDataTags()->size(); i++) {
            runData.fDataNonMusr.AppendData(dummy);
            runData.fDataNonMusr.AppendErrData(dummy);
          }

          firstData = false;
        }

        if (labelledFormat) { // handle labelled formated data
          // check if run line
          const Char_t *str = workStr.Data();
          if (isdigit(str[0])) { // run line
            TString run("run");
            idx = GetDataTagIndex(run, runData.fDataNonMusr.GetDataTags());
            if (idx == -1) {
              cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              cerr << endl << ">> " << workStr.Data();
              cerr << endl << ">> found potential run data line without run data tag.";
              return false;
            }
            // split string in tokens
            tokens = workStr.Tokenize(","); // line has structure: runNo,,, runTitle
            ostr = dynamic_cast<TObjString*>(tokens->At(0));
            tstr = ostr->GetString();
            if (!tstr.IsFloat()) {
              cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              cerr << endl << ">> " << workStr.Data();
              cerr << endl << ">> Expected db-data line with structure: runNo,,, runTitle";
              cerr << endl << ">> runNo = " << tstr.Data() << ", seems to be not a number.";
              delete tokens;
              return false;
            }
            val = tstr.Atof();
            runData.fDataNonMusr.AppendSubData(idx, val);
            runData.fDataNonMusr.AppendSubErrData(idx, 1.0);
          } else { // tag = data line
            // remove all possible spaces
            workStr.ReplaceAll(" ", "");
            // split string in tokens
            tokens = workStr.Tokenize("=,"); // line has structure: tag = val,err1,err2,
            if (tokens->GetEntries() < 3) {
              cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              cerr << endl << ">> " << workStr.Data();
              cerr << endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
              delete tokens;
              return false;
            }
            ostr = dynamic_cast<TObjString*>(tokens->At(0));
            tstr = ostr->GetString();
            idx = GetDataTagIndex(tstr, runData.fDataNonMusr.GetDataTags());
            if (idx == -1) {
              cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              cerr << endl << ">> " << workStr.Data();
              cerr << endl << ">> data tag error: " << tstr.Data() << " seems not present in the data tag list";
              delete tokens;
              return false;
            }

            switch (tokens->GetEntries()) {
              case 3: // tag = val,,,
                ostr = dynamic_cast<TObjString*>(tokens->At(1));
                tstr = ostr->GetString();
                if (!tstr.IsFloat()) {
                  cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                  cerr << endl << ">> " << workStr.Data();
                  cerr << endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                  cerr << endl << ">> val = " << tstr.Data() << ", seems to be not a number.";
                  delete tokens;
                  return false;
                }
                val = tstr.Atof();
                runData.fDataNonMusr.AppendSubData(idx, val);
                runData.fDataNonMusr.AppendSubErrData(idx, 1.0);
                break;
              case 4: // tag = val,err,,
              case 5: // tag = val,err1,err2,
                // handle val
                ostr = dynamic_cast<TObjString*>(tokens->At(1));
                tstr = ostr->GetString();
                if (!tstr.IsFloat()) {
                  cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                  cerr << endl << ">> " << workStr.Data();
                  cerr << endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                  cerr << endl << ">> val = " << tstr.Data() << ", seems to be not a number.";
                  delete tokens;
                  return false;
                }
                val = tstr.Atof();
                runData.fDataNonMusr.AppendSubData(idx, val);
                // handle err1 (err2 will be ignored for the time being)
                ostr = dynamic_cast<TObjString*>(tokens->At(2));
                tstr = ostr->GetString();
                if (!tstr.IsFloat()) {
                  cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                  cerr << endl << ">> " << workStr.Data();
                  cerr << endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                  cerr << endl << ">> err1 = " << tstr.Data() << ", seems to be not a number.";
                  delete tokens;
                  return false;
                }
                val = tstr.Atof();
                runData.fDataNonMusr.AppendSubErrData(idx, val);
                break;
              default:
                cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                cerr << endl << ">> " << workStr.Data();
                cerr << endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                delete tokens;
                return false;
                break;
            }
          }

        } else { // handle row formated data
          // split string in tokens
          tokens = workStr.Tokenize(","); // line has structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle
          if (tokens->GetEntries() != (Int_t)(3*runData.fDataNonMusr.GetDataTags()->size()+1)) {
            cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
            cerr << endl << ">> " << workStr.Data();
            cerr << endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
            cerr << endl << ">> found = " << tokens->GetEntries() << " tokens, however expected " << 3*runData.fDataNonMusr.GetDataTags()->size()+1;
            cerr << endl << ">> Perhaps there are commas without space inbetween, like 12.3,, 3.2,...";
            delete tokens;
            return false;
          }
          // extract data
          Int_t j=0;
          for (Int_t i=0; i<tokens->GetEntries()-1; i+=3) {
            // handle value
            ostr = dynamic_cast<TObjString*>(tokens->At(i));
            tstr = ostr->GetString();
            if (!tstr.IsFloat()) {
              cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              cerr << endl << ">> " << workStr.Data();
              cerr << endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
              cerr << endl << ">> value=" << tstr.Data() << " seems not to be a number";
              delete tokens;
              return false;
            }
            runData.fDataNonMusr.AppendSubData(j, tstr.Atof());

            // handle 1st error if present (2nd will be ignored for now)
            ostr = dynamic_cast<TObjString*>(tokens->At(i+1));
            tstr = ostr->GetString();
            if (tstr.IsWhitespace()) {
              runData.fDataNonMusr.AppendSubErrData(j, 1.0);
            } else if (tstr.IsFloat()) {
              runData.fDataNonMusr.AppendSubErrData(j, tstr.Atof());
            } else {
              cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              cerr << endl << ">> " << workStr.Data();
              cerr << endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
              cerr << endl << ">> error1=" << tstr.Data() << " seems not to be a number";
              delete tokens;
              return false;
            }
            j++;
          }
        }
        break;
      default:
        break;
    }
  }

  f.close();

  // check that the number of labels == the number of data tags
  if (runData.fDataNonMusr.GetLabels()->size() != runData.fDataNonMusr.GetDataTags()->size()) {
    cerr << endl << "PRunDataHandler::ReadDBFile **ERROR**";
    cerr << endl << ">> number of LABELS found    = " << runData.fDataNonMusr.GetLabels()->size();
    cerr << endl << ">> number of Data tags found = " << runData.fDataNonMusr.GetDataTags()->size();
    cerr << endl << ">> They have to be equal!!";
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
    return false;
  }

  // check if all vectors have the same size
  for (UInt_t i=1; i<runData.fDataNonMusr.GetData()->size(); i++) {
    if (runData.fDataNonMusr.GetData()->at(i).size() != runData.fDataNonMusr.GetData()->at(i-1).size()) {
      cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo;
      cerr << endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i-1).Data() << ", number data elements = " << runData.fDataNonMusr.GetData()->at(i-1).size();
      cerr << endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i).Data() << ", number data elements = " << runData.fDataNonMusr.GetData()->at(i).size();
      cerr << endl << ">> They have to be equal!!";
      success = false;
      break;
    }
    if (runData.fDataNonMusr.GetErrData()->at(i).size() != runData.fDataNonMusr.GetErrData()->at(i-1).size()) {
      cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo;
      cerr << endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i-1).Data() << ", number data elements = " << runData.fDataNonMusr.GetData()->at(i-1).size();
      cerr << endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i).Data() << ", number error data elements = " << runData.fDataNonMusr.GetErrData()->at(i).size();
      cerr << endl << ">> They have to be equal!!";
      success = false;
      break;
    }
  }

  // clean up tokens
  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  // keep run name
  runData.SetRunName(fRunName);

  fData.push_back(runData);

  return success;
}

//--------------------------------------------------------------------------
// StripWhitespace (private)
//--------------------------------------------------------------------------
/**
 * <p> Strip white spaces from a string. The hope is that future TString
 * implementations of ROOT will make this routine obsolate.
 *
 * \param str string to be stripped. It will be modified directly on success.
 *
 * <b>return:</b> true at success, otherwise false.
 */
Bool_t PRunDataHandler::StripWhitespace(TString &str)
{
  Char_t *s    = 0;
  Char_t *subs = 0;
  Int_t i;
  Int_t start;
  Int_t end;
  Int_t size;

  size = (Int_t)str.Length();
  s = new Char_t[size+1];

  if (!s)
    return false;

  for (Int_t i=0; i<size+1; i++)
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
  subs = new Char_t[end-start+2];
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
// IsWhitespace (private)
//--------------------------------------------------------------------------
/**
 * <p> Check is a string consists only of white spaces, i.e. spaces and/or
 * ctrl-characters.
 *
 * \param str string to be checked
 *
 * <b>return:</b> true if string consist only of white spaces, otherwise false.
 */
Bool_t PRunDataHandler::IsWhitespace(const Char_t *str)
{
  UInt_t i=0;

  while (isblank(str[i]) || iscntrl(str[i])) {
    if (str[i] == 0)
      break;
    i++;
  }

  if (i == strlen(str))
    return true;
  else
    return false;
}

//--------------------------------------------------------------------------
// ToDouble (private)
//--------------------------------------------------------------------------
/**
 * <p> Convert a string to a Double_t.
 *
 * \param str string to be converted
 * \param ok true on success, otherwise false.
 *
 * <b>return:</b> returns the converted string, or 0.0 in case of ok==false
 */
Double_t PRunDataHandler::ToDouble(TString &str, Bool_t &ok)
{
  Char_t *s;
  Double_t value;
  Int_t size, status;

  ok = true;

  size = (Int_t)str.Length();
  s = new Char_t[size+1];

  if (!s) {
    ok = false;
    return 0.0;
  }

  // copy string; stupid way but it works
  for (Int_t i=0; i<size+1; i++)
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
// ToInt (private)
//--------------------------------------------------------------------------
/**
 * <p> Convert a string to an Int_t.
 *
 * \param str string to be converted
 * \param ok true on success, otherwise false.
 *
 * <b>return:</b> returns the converted string, or 0 in case of ok==false
 */
Int_t PRunDataHandler::ToInt(TString &str, Bool_t &ok)
{
  Char_t *s;
  Int_t value;
  Int_t size, status;

  ok = true;

  size = (Int_t)str.Length();
  s = new Char_t[size+1];

  if (!s) {
    ok = false;
    return 0;
  }

  // copy string; stupid way but it works
  for (Int_t i=0; i<size+1; i++)
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

//--------------------------------------------------------------------------
// GetDataTagIndex (private)
//--------------------------------------------------------------------------
/**
 * <p> Checks is str is in a list of data tags
 *
 * \param str data tag string (see description of nonMusr db-data)
 * \param dataTags vector of all data tags
 *
 * <b>return:</b> if found returns the data tag index (from the dataTags vector), otherwise -1
 */
Int_t PRunDataHandler::GetDataTagIndex(TString &str, const PStringVector* dataTags)
{
  Int_t result = -1;

  // check all the other possible data tags
  for (UInt_t i=0; i<dataTags->size(); i++) {
    if (!dataTags->at(i).CompareTo(str, TString::kIgnoreCase)) {
      result = i;
      break;
    }
  }

  return result;
}

