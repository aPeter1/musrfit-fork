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
 * <p> Checks if runName is found.
 *
 * <b>return:</b> if data are found: pointer to the data.
 * otherwise the null pointer will be returned.
 *
 * \param runName run name, e.g. 2009/lem09_his_1234
 */
PRawRunData* PRunDataHandler::GetRunData(const TString &runName)
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
 * <p> The main read file routine which is filtering what read sub-routine
 * needs to be called.
 *
 * <b>return:</b> true if reading was successful, false if reading failed.
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

  for (unsigned int i=0; i<runList->size(); i++) {
    for (unsigned int j=0; j<runList->at(i).fRunName.size(); j++) {
      fRunName = runList->at(i).fRunName[j];
      // check is file is already read
      if (FileAlreadyRead(runList->at(i).fRunName[j]))
        continue;
      // check if file actually exists
      if (!FileExistsCheck(runList->at(i), j))
        return false;
      // everything looks fine, hence try to read the data file
      if (!runList->at(i).fFileFormat[j].CompareTo("root-npp")) // not post pile up corrected histos
        success = ReadRootFile(true);
      else if (!runList->at(i).fFileFormat[j].CompareTo("root-ppc")) // post pile up corrected histos
        success = ReadRootFile(false);
      else if (!runList->at(i).fFileFormat[j].CompareTo("nexus"))
        success = ReadNexusFile();
      else if (!runList->at(i).fFileFormat[j].CompareTo("psi-bin"))
        success = ReadPsiBinFile();
      else if (!runList->at(i).fFileFormat[j].CompareTo("mud"))
        success = ReadMudFile();
      else if (!runList->at(i).fFileFormat[j].CompareTo("wkm"))
        success = ReadWkmFile();
      else if (!runList->at(i).fFileFormat[j].CompareTo("ascii"))
        success = ReadAsciiFile();
      else if (!runList->at(i).fFileFormat[j].CompareTo("db"))
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
bool PRunDataHandler::FileAlreadyRead(TString runName)
{
  for (unsigned int i=0; i<fData.size(); i++) {
    if (!fData[i].fRunName.CompareTo(runName)) { // run alread read
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
bool PRunDataHandler::FileExistsCheck(PMsrRunStructure &runInfo, const unsigned int idx)
{
  bool success = true;

  // local init
  TROOT root("PRunBase", "PRunBase", 0);
  TString pathName = "???";
  TString str;
  TString ext;

  runInfo.fBeamline[idx].ToLower();
  runInfo.fInstitute[idx].ToLower();
  runInfo.fFileFormat[idx].ToLower();

  // file extensions for the various formats
  if (!runInfo.fFileFormat[idx].CompareTo("root-npp")) // not post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.fFileFormat[idx].CompareTo("root-ppc")) // post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.fFileFormat[idx].CompareTo("nexus"))
    ext = TString("nexus");
  else if (!runInfo.fFileFormat[idx].CompareTo("psi-bin"))
    ext = TString("bin");
  else if (!runInfo.fFileFormat[idx].CompareTo("mud"))
    ext = TString("mud");
  else if (!runInfo.fFileFormat[idx].CompareTo("wkm")) {
    if (!runInfo.fBeamline[idx].CompareTo("mue4"))
      ext = TString("nemu");
    else
      ext = runInfo.fBeamline[idx];
  }
  else if (!runInfo.fFileFormat[idx].CompareTo("ascii"))
    ext = TString("dat");
  else if (!runInfo.fFileFormat[idx].CompareTo("db"))
    ext = TString("db");
  else
    success = false;

  // unkown file format found
  if (!success) {
    str = runInfo.fFileFormat[idx];
    str.ToUpper();
    cout << endl << "File Format '" << str.Data() << "' unsupported.";
    cout << endl << "  support file formats are:";
    cout << endl << "  ROOT-NPP -> root not post pileup corrected for lem";
    cout << endl << "  ROOT-PPC -> root post pileup corrected for lem";
    cout << endl << "  NEXUS    -> nexus file format";
    cout << endl << "  PSI-BIN  -> psi bin file format";
    cout << endl << "  MUD      -> triumf mud file format";
    cout << endl << "  WKM      -> wkm ascii file format";
    cout << endl << "  ASCII    -> column like file format";
    cout << endl << "  DB       -> triumf db file \"format\"";
    cout << endl;
    return success;
  }

  // check if the file is in the local directory
  str = runInfo.fRunName[idx] + TString(".") + ext;
  if (gSystem->AccessPathName(str.Data())!=true) { // found in the local dir
    pathName = str;
  }

  // check if the file is found in the <msr-file-directory>
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    str  = *fMsrInfo->GetMsrFileDirectoryPath();
    str += runInfo.fRunName[idx] + TString(".") + ext;
    if (gSystem->AccessPathName(str.Data())!=true) { // found
      pathName = str;
    }
  }

  // check if the file is found in the directory given in the startup file
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    for (unsigned int i=0; i<fDataPath.size(); i++) {
      str = fDataPath[i] + TString("/") + runInfo.fRunName[idx] + TString(".") + ext;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }

  // check if the file is found in the directories given by WKMFULLDATAPATH
  const char *wkmpath = gSystem->Getenv("WKMFULLDATAPATH");
  if (pathName.CompareTo("???") == 0) { // not found in local directory and xml path
    str = TString(wkmpath);
    // WKMFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (int i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/") + runInfo.fRunName[idx] + TString(".") + ext;
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
    runInfo.fInstitute[idx].ToUpper();
    runInfo.fBeamline[idx].ToUpper();
    TDatime datetime;
    TString dt;
    dt += datetime.GetYear();
    for (int i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/DATA/") +
            runInfo.fInstitute[idx] + TString("/") +
            runInfo.fBeamline[idx] + TString("/") +
            dt + TString("/") +
            runInfo.fRunName[idx] + TString(".") + ext;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }

  // no proper path name found
  if (pathName.CompareTo("???") == 0) {
    cerr << endl << "**ERROR** Couldn't find '" << runInfo.fRunName[idx] << "' in any standard path.";
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
bool PRunDataHandler::ReadRootFile(bool notPostPileup)
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

  // get run title
  TObjString ostr = runHeader->GetRunTitle();
  runData.fRunTitle = ostr.GetString();

  // get temperature
  runData.fTemp.resize(1);
  runData.fTemp[0].first = runHeader->GetSampleTemperature();
  runData.fTemp[0].second = runHeader->GetSampleTemperatureError();

  // get field
  runData.fField = runHeader->GetSampleBField();

  // get implantation energy
  runData.fEnergy = runHeader->GetImpEnergy();

  // get moderator HV
  runData.fTransport = runHeader->GetModeratorHV();

  // get setup
  runData.fSetup = runHeader->GetLemSetup().GetString();

  // get time resolution
  runData.fTimeResolution = runHeader->GetTimeResolution();

  // get number of histogramms
  int noOfHistos = runHeader->GetNHist();

  // get t0's
  Double_t *t0 = runHeader->GetTimeZero();
  // check if t0's are there
  if (t0[0] != -1) { // ugly, but at the moment there is no other way
    // copy t0's so they are not lost
    for (int i=0; i<noOfHistos; i++) {
      runData.fT0s.push_back((int)t0[i]);
    }
  }

  // read run summary to obtain ring anode HV values
  TObjArray *runSummary = dynamic_cast<TObjArray*>(folder->FindObjectAny("RunSummary"));

  // check if run summary is valid
  if (!runSummary) {
    cout << endl << "Couldn't obtain run summary info from ROOT file " << fRunPathName.Data() << endl;
    // this is not fatal... only RA-HV values are not available
  } else { // it follows a (at least) little bit strange extraction of the RA values from Thomas' TObjArray...
           //streaming of a ASCII-file would be more easy
    TString s, tok;
    TObjArrayIter summIter(runSummary);
    TObjString *os(dynamic_cast<TObjString*>(summIter.Next()));
    TObjArray *oa(0);
    TObjString *objTok(0);
    while(os != 0){
      s = os->GetString();
      // will put four parallel if's since it may be that more than one RA-values are on one line
      if(s.Contains("RA-L")){
        if(runData.fRingAnode.empty() || runData.fRingAnode.size() == 2)
          runData.fRingAnode.resize(2);
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if(!objTok->GetString().CompareTo("RA-L")){
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if(objTok != 0 && !objTok->GetString().CompareTo("=")){
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.fRingAnode[0] = objTok->GetString().Atof(); // fill RA-R value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      if(s.Contains("RA-R")){
        if(runData.fRingAnode.empty() || runData.fRingAnode.size() == 2)
          runData.fRingAnode.resize(2);
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if(!objTok->GetString().CompareTo("RA-R")){
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if(objTok != 0 && !objTok->GetString().CompareTo("=")){
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.fRingAnode[1] = objTok->GetString().Atof(); // fill RA-R value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      if(s.Contains("RA-T")){
        if(runData.fRingAnode.empty() || runData.fRingAnode.size() == 2)
          runData.fRingAnode.resize(4);
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if(!objTok->GetString().CompareTo("RA-T")){
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if(objTok != 0 && !objTok->GetString().CompareTo("=")){
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.fRingAnode[2] = objTok->GetString().Atof(); // fill RA-T value into the runData structure
              break;
            }
          }
        objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
        }
      }

      if(s.Contains("RA-B")){
        if(runData.fRingAnode.empty() || runData.fRingAnode.size() == 2)
          runData.fRingAnode.resize(4);
        oa = s.Tokenize(" ");
        TObjArrayIter lineIter(oa);
        objTok = dynamic_cast<TObjString*>(lineIter.Next());
        while (objTok != 0){
          if(!objTok->GetString().CompareTo("RA-B")){
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
            if(objTok != 0 && !objTok->GetString().CompareTo("=")){
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
              runData.fRingAnode[3] = objTok->GetString().Atof(); // fill RA-B value into the runData structure
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
    cout << endl << "Couldn't obtain histos from " << fRunPathName.Data() << endl;
    f.Close();
    return false;
  }
  // get all the data
  char histoName[32];
  if (notPostPileup) { // read the data which are NOT post pileup corrected
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
  } else { // read the data which ARE post pileup corrected
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
  }

  f.Close();

  // keep run name
  runData.fRunName = fRunName;

  // add run to the run list
  fData.push_back(runData);

  // clean up
  for (unsigned int i=0; i<runData.fDataBin.size(); i++)
    runData.fDataBin[i].clear();
  runData.fDataBin.clear();

  runData.fRingAnode.clear();

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
bool PRunDataHandler::ReadNexusFile()
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
bool PRunDataHandler::ReadWkmFile()
{
  PDoubleVector histoData;
  PRawRunData runData;

  // init runData header info
  runData.fRunName        = TString("");
  runData.fRunTitle       = TString("");
  runData.fSetup          = TString("");
  runData.fField          = -9.9e99;
  runData.fTemp.clear();
  runData.fEnergy         = -999.0; // standard LEM values...
  runData.fTransport      = -999.0; // standard LEM values...
  runData.fRingAnode.clear();
  runData.fTimeResolution = 0.0;

  // open file
  ifstream f;

  // open wkm-file
  f.open(fRunPathName.Data(), ifstream::in);
  if (!f.is_open()) {
    cout << endl << "Couldn't open run data (" << fRunPathName.Data() << ") file for reading, sorry ...";
    cout << endl;
    return false;
  }

  // read header
  bool    headerInfo = true;
  char    instr[512];
  TString line, linecp;
  double  dval;
  int     ival;
  bool    ok;
  int     groups = 0, channels = 0;

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
        runData.fRunTitle = line;
      } else if (line.Contains("Field")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Field:'
        StripWhitespace(line);
        idx = line.Index("G"); // check if unit is given
        if (idx > 0) // unit is indeed given
          line.Resize(idx);
        dval = ToDouble(line, ok);
        if (ok)
          runData.fField = dval;
      } else if (line.Contains("Setup")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, 0, 0); // remove 'Setup:'
        StripWhitespace(line);
        runData.fSetup = line;
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
        if (ok){
          runData.fTemp.resize(1);
          runData.fTemp[0].first = dval;
          runData.fTemp[0].second = 0.0;
        }
        idx = linecp.Index("+/-"); // get the error
        linecp.Replace(0, idx+3, 0, 0);
        StripWhitespace(linecp);
        dval = ToDouble(linecp, ok);
        if (ok && !runData.fTemp.empty()){
          runData.fTemp[0].second = dval;
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
        if (ok){
          runData.fTemp.resize(2);
          runData.fTemp[1].first = dval;
          runData.fTemp[1].second = 0.0;
        }
        idx = linecp.Index("+/-"); // get the error
        linecp.Replace(0, idx+3, 0, 0);
        StripWhitespace(linecp);
        dval = ToDouble(linecp, ok);
        if (ok && runData.fTemp.size() > 1){
          runData.fTemp[1].second = dval;
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
          runData.fTimeResolution = dval * 1000.0;
      }
    }

    if (headerInfo)
      f.getline(instr, sizeof(instr));
  } while (headerInfo && !f.eof());

  if ((groups == 0) || (channels == 0) || runData.fTimeResolution == 0.0) {
    cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** essential header informations are missing!";
    cerr << endl << "  >> groups          = " << groups;
    cerr << endl << "  >> channels        = " << channels;
    cerr << endl << "  >> time resolution = " << runData.fTimeResolution;
    cerr << endl;
    f.close();
    return false;
  }

  // read data ---------------------------------------------------------
  unsigned int group_counter = 0;
  int val;
  TObjArray *tokens;
  TObjString *ostr;
  TString str;
  do {
    // check if empty line, i.e. new group
    if (IsWhitespace(instr)) {
      runData.fDataBin.push_back(histoData);
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
        // clean up
        for (unsigned int i=0; i<group_counter; i++)
          runData.fDataBin[i].clear();
        runData.fDataBin.clear();
        return false;
      }
      for (int i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        val = ToInt(str, ok);
        if (ok) {
          histoData.push_back(val);
        } else {
          cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: data line contains non-integer values.";
          // clean up
          for (unsigned int i=0; i<group_counter; i++)
            runData.fDataBin[i].clear();
          runData.fDataBin.clear();
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
      // clean up
      for (unsigned int i=0; i<group_counter; i++)
        runData.fDataBin[i].clear();
      runData.fDataBin.clear();
      return false;
    }
    for (int i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString();
      val = ToInt(str, ok);
      if (ok) {
        histoData.push_back(val);
      } else {
        cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: data line contains non-integer values.";
        // clean up
        for (unsigned int i=0; i<group_counter; i++)
          runData.fDataBin[i].clear();
        runData.fDataBin.clear();
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
    runData.fDataBin.push_back(histoData);
    histoData.clear();
  }

  // close file
  f.close();

  // check if all groups are found
  if ((int) runData.fDataBin.size() != groups) {
    cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR**";
    cerr << endl << "  expected " << groups << " histos, but found " << runData.fDataBin.size();
    // clean up
    for (unsigned int i=0; i<runData.fDataBin.size(); i++)
      runData.fDataBin[i].clear();
    runData.fDataBin.clear();
    return false;
  }

  // check if all groups have enough channels
  for (unsigned int i=0; i<runData.fDataBin.size(); i++) {
    if ((int) runData.fDataBin[i].size() != channels) {
      cerr << endl << "PRunDataHandler::ReadWkmFile(): **ERROR**";
      cerr << endl << "  expected " << channels << " bins in histo " << i << ", but found " << runData.fDataBin[i].size();
      // clean up
      for (unsigned int j=0; j<runData.fDataBin.size(); j++)
        runData.fDataBin[j].clear();
      runData.fDataBin.clear();
      return false;
    }
  }

  // keep run name
  runData.fRunName = fRunName;

  // add run to the run list
  fData.push_back(runData);

  // clean up
  for (unsigned int i=0; i<runData.fDataBin.size(); i++)
    runData.fDataBin[i].clear();
  runData.fDataBin.clear();

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
bool PRunDataHandler::ReadPsiBinFile()
{
//  cout << endl << "PRunDataHandler::ReadPsiBinFile(): Sorry, not yet implemented ...";

  MuSR_td_PSI_bin psiBin;
  int status;
  bool success;

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
  double dval;
  // keep run name
  runData.fRunName = fRunName;
  // get run title
  runData.fRunTitle = TString(psiBin.get_comment().c_str()); // run title
  // get setup
  runData.fSetup = TString(psiBin.get_orient().c_str());
  // set LEM specific information to default value since it is not in the file and not used...
  runData.fEnergy = -999.0;
  runData.fTransport = -999.0;
  runData.fRingAnode.clear();
  // get field
  status = sscanf(psiBin.get_field().c_str(), "%lfG", &dval);
  if (status == 1)
    runData.fField = dval;
  // get temperature
  PDoubleVector tempVec(psiBin.get_temperatures_vector());
  PDoubleVector tempDevVec(psiBin.get_devTemperatures_vector());
  if(tempVec.size() > 1 && tempDevVec.size() > 1 && tempVec[0] && tempVec[1]) {
    runData.fTemp.resize(2);
    // take only the first two values for now...
    //maybe that's not enough - e.g. in older GPD data I saw the "correct values in the second and third entry..."
    for (unsigned int i(0); i<2; i++){
      runData.fTemp[i].first = tempVec[i];
      runData.fTemp[i].second = tempDevVec[i];
    }
    tempVec.clear();
    tempDevVec.clear();
  } else {
    status = sscanf(psiBin.get_temp().c_str(), "%lfK", &dval);
    if (status == 1)
      runData.fTemp.resize(1);
      runData.fTemp[0].first = dval;
      runData.fTemp[0].second = 0.0;
  }

  // get time resolution (ns)
  runData.fTimeResolution = psiBin.get_binWidth_ns();
  // get t0's
  ivec = psiBin.get_t0_vector();
  if (ivec.empty()) {
    cerr << endl << "**ERROR** psibin file: couldn't obtain any t0's";
    cerr << endl;
    return false;
  }
  for (unsigned int i=0; i<ivec.size(); i++)
    runData.fT0s.push_back(ivec[i]);

  // fill raw data
  PDoubleVector histoData;
  int *histo;
  for (int i=0; i<psiBin.get_numberHisto_int(); i++) {
    histo = psiBin.get_histo_array_int(i);
    for (int j=0; j<psiBin.get_histoLength_bin(); j++) {
      histoData.push_back(histo[j]);
    }
    delete histo;
    runData.fDataBin.push_back(histoData);
    histoData.clear();
  }

  // add run to the run list
  fData.push_back(runData);

  // clean up
  runData.fT0s.clear();
  for (unsigned int i=0; i<runData.fDataBin.size(); i++)
    runData.fDataBin[i].clear();
  runData.fDataBin.clear();

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
bool PRunDataHandler::ReadMudFile()
{
  cout << endl << "PRunDataHandler::ReadMudFile(): Sorry, not yet implemented ...";
  return false;
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
bool PRunDataHandler::ReadAsciiFile()
{
  bool success = true;

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
  runData.fDataNonMusr.fFromAscii = true;
  runData.fDataNonMusr.fLabels.push_back("??"); // x default label
  runData.fDataNonMusr.fLabels.push_back("??"); // y default label

  runData.fRunName = fRunName; // keep the run name

  int     lineNo = 0;
  char    instr[512];
  TString line, workStr;
  bool    headerTag = false;
  bool    dataTag = false;
  double  x, y, ey;
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
        runData.fRunTitle = TString(workStr.Data()+workStr.First(":")+2);
      } else if (workStr.BeginsWith("setup:", TString::kIgnoreCase)) {
        runData.fSetup = TString(workStr.Data()+workStr.First(":")+2);
      } else if (workStr.BeginsWith("field:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", field is not a number.";
          cerr << endl;
          success = false;
          break;
        }
        runData.fField = workStr.Atof();
      } else if (workStr.BeginsWith("x-axis-title:", TString::kIgnoreCase)) {
        runData.fDataNonMusr.fLabels[0] = TString(workStr.Data()+workStr.First(":")+2);
      } else if (workStr.BeginsWith("y-axis-title:", TString::kIgnoreCase)) {
        runData.fDataNonMusr.fLabels[1] = TString(workStr.Data()+workStr.First(":")+2);
      } else if (workStr.BeginsWith("temp:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", temperature is not a number.";
          cerr << endl;
          success = false;
          break;
        }
        runData.fTemp.resize(1);
        runData.fTemp[0].first = workStr.Atof();
        runData.fTemp[0].second = 0.0;
      } else if (workStr.BeginsWith("energy:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          cerr << endl << "PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", energy is not a number.";
          cerr << endl;
          success = false;
          break;
        }
        runData.fEnergy = workStr.Atof();
        runData.fTransport = -999.0; // just to initialize the variables to some "meaningful" value
        runData.fRingAnode.clear();
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
  runData.fDataNonMusr.fData.push_back(xVec);
  runData.fDataNonMusr.fErrData.push_back(exVec);
  runData.fDataNonMusr.fData.push_back(yVec);
  runData.fDataNonMusr.fErrData.push_back(eyVec);

  fData.push_back(runData);

  // clean up
  xVec.clear();
  exVec.clear();
  yVec.clear();
  eyVec.clear();
  runData.fDataNonMusr.fData.clear();
  runData.fDataNonMusr.fErrData.clear();

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
bool PRunDataHandler::ReadDBFile()
{
  bool success = true;

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

  runData.fDataNonMusr.fFromAscii = false;

  int     lineNo = 0;
  int     idx;
  int     dbTag = -1;
  char    instr[512];
  TString line, workStr;
  double  val;
  bool firstData = true; // needed as a switch to check in which format the data are given.
  bool labelledFormat = true; // flag showing if the data are given in row format, or as labelled format (see description above, default is labelled format)

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
      for (int i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        runData.fDataNonMusr.fDataTags.push_back(ostr->GetString());
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
        runData.fRunTitle = workStr;
        break;
      case 1: // ABSTRACT
        // nothing to be done for now
        break;
      case 2: // COMMENTS
        // nothing to be done for now
        break;
      case 3: // LABEL
        runData.fDataNonMusr.fLabels.push_back(workStr);
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
          for (unsigned int i=0; i<runData.fDataNonMusr.fDataTags.size(); i++) {
            runData.fDataNonMusr.fData.push_back(dummy);
            runData.fDataNonMusr.fErrData.push_back(dummy);
          }

          firstData = false;
        }

        if (labelledFormat) { // handle labelled formated data
          // check if run line
          const char *str = workStr.Data();
          if (isdigit(str[0])) { // run line
            TString run("run");
            idx = GetDataTagIndex(run, runData.fDataNonMusr.fDataTags);
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
            runData.fDataNonMusr.fData[idx].push_back(val);
            runData.fDataNonMusr.fErrData[idx].push_back(1.0);
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
            idx = GetDataTagIndex(tstr, runData.fDataNonMusr.fDataTags);
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
                runData.fDataNonMusr.fData[idx].push_back(val);
                runData.fDataNonMusr.fErrData[idx].push_back(1.0);
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
                runData.fDataNonMusr.fData[idx].push_back(val);
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
                runData.fDataNonMusr.fErrData[idx].push_back(val);
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
          if (tokens->GetEntries() != (int)(3*runData.fDataNonMusr.fDataTags.size()+1)) {
            cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
            cerr << endl << ">> " << workStr.Data();
            cerr << endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
            cerr << endl << ">> found = " << tokens->GetEntries() << " tokens, however expected " << 3*runData.fDataNonMusr.fDataTags.size()+1;
            cerr << endl << ">> Perhaps there are commas without space inbetween, like 12.3,, 3.2,...";
            delete tokens;
            return false;
          }
          // extract data
          int j=0;
          for (int i=0; i<tokens->GetEntries()-1; i+=3) {
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
            runData.fDataNonMusr.fData[j].push_back(tstr.Atof());

            // handle 1st error if present (2nd will be ignored for now)
            ostr = dynamic_cast<TObjString*>(tokens->At(i+1));
            tstr = ostr->GetString();
            if (tstr.IsWhitespace()) {
              runData.fDataNonMusr.fErrData[j].push_back(1.0);
            } else if (tstr.IsFloat()) {
              runData.fDataNonMusr.fErrData[j].push_back(tstr.Atof());
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
  if (runData.fDataNonMusr.fLabels.size() != runData.fDataNonMusr.fDataTags.size()) {
    cerr << endl << "PRunDataHandler::ReadDBFile **ERROR**";
    cerr << endl << ">> number of LABELS found    = " << runData.fDataNonMusr.fLabels.size();
    cerr << endl << ">> number of Data tags found = " << runData.fDataNonMusr.fDataTags.size();
    cerr << endl << ">> They have to be equal!!";
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
    return false;
  }

  // check if all vectors have the same size
  for (unsigned int i=1; i<runData.fDataNonMusr.fData.size(); i++) {
    if (runData.fDataNonMusr.fData[i].size() != runData.fDataNonMusr.fData[i-1].size()) {
      cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo;
      cerr << endl << ">> label: " << runData.fDataNonMusr.fDataTags[i-1].Data() << ", number data elements = " << runData.fDataNonMusr.fData[i-1].size();
      cerr << endl << ">> label: " << runData.fDataNonMusr.fDataTags[i].Data() << ", number data elements = " << runData.fDataNonMusr.fData[i].size();
      cerr << endl << ">> They have to be equal!!";
      success = false;
      break;
    }
    if (runData.fDataNonMusr.fErrData[i].size() != runData.fDataNonMusr.fErrData[i-1].size()) {
      cerr << endl << "PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo;
      cerr << endl << ">> label: " << runData.fDataNonMusr.fDataTags[i-1].Data() << ", number data elements = " << runData.fDataNonMusr.fData[i-1].size();
      cerr << endl << ">> label: " << runData.fDataNonMusr.fDataTags[i].Data() << ", number error data elements = " << runData.fDataNonMusr.fErrData[i].size();
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
  runData.fRunName = fRunName;

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
bool PRunDataHandler::IsWhitespace(const char *str)
{
  unsigned int i=0;

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
 * <p> Convert a string to a double.
 *
 * \param str string to be converted
 * \param ok true on success, otherwise false.
 *
 * <b>return:</b> returns the converted string, or 0.0 in case of ok==false
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
// ToInt (private)
//--------------------------------------------------------------------------
/**
 * <p> Convert a string to an int.
 *
 * \param str string to be converted
 * \param ok true on success, otherwise false.
 *
 * <b>return:</b> returns the converted string, or 0 in case of ok==false
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
int PRunDataHandler::GetDataTagIndex(TString &str, PStringVector &dataTags)
{
  int result = -1;

  // check all the other possible data tags
  for (unsigned int i=0; i<dataTags.size(); i++) {
    if (!dataTags[i].CompareTo(str, TString::kIgnoreCase)) {
      result = i;
      break;
    }
  }

  return result;
}

