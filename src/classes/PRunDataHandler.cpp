/***************************************************************************

  PRunDataHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2022 by Andreas Suter                              *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TDatime.h>

#include "TMusrRunHeader.h"
#include "TLemRunHeader.h"
#include "MuSR_td_PSI_bin.h"
#include "mud.h"

#ifdef PNEXUS_ENABLED
#include "PNeXus.h"
#endif

#include "PRunDataHandler.h"

#define PRH_MUSR_ROOT  0
#define PRH_LEM_ROOT   1

#define PRH_NPP_OFFSET 0
#define PRH_PPC_OFFSET 20

#define A2M_UNDEFINED  0
#define A2M_ROOT       1
#define A2M_MUSR_ROOT  2
#define A2M_PSIBIN     3
#define A2M_PSIMDU     4
#define A2M_MUD        5
#define A2M_NEXUS      6
#define A2M_WKM        7
#define A2M_ASCII      8

#define PHR_INIT_ALL      0
#define PHR_INIT_MSR      1
#define PHR_INIT_ANY2MANY 2

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Empty Constructor
 */
PRunDataHandler::PRunDataHandler()
{
  Init();
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor which just reads a single muSR data file.
 *
 * \param fileName
 * \param fileFormat
 */
PRunDataHandler::PRunDataHandler(TString fileName, const TString fileFormat) : fFileFormat(fileFormat)
{
  Init();

  FileExistsCheck(fileName);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor which just reads a single muSR data file.
 *
 * \param fileName
 * \param fileFormat
 * \param dataPath
 */
PRunDataHandler::PRunDataHandler(TString fileName, const TString fileFormat, const PStringVector dataPath) : fDataPath(dataPath), fFileFormat(fileFormat)
{
  Init();

  FileExistsCheck(fileName);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor which just writes a single muSR data file.
 *
 * \param fileName
 * \param fileFormat
 * \param dataPath
 * \param runData
 */
PRunDataHandler::PRunDataHandler(TString fileName, const TString fileFormat, const TString dataPath, PRawRunData &runData)
{
  Init();
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor, reading the data histogramm files.
 *
 * \param msrInfo pointer to the msr-file handler
 */
PRunDataHandler::PRunDataHandler(PAny2ManyInfo *any2ManyInfo) : fAny2ManyInfo(any2ManyInfo)
{
  Init(PHR_INIT_ANY2MANY);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor, reading the data histogramm files.
 *
 * \param any2ManyInfo pointer to the PAny2ManyInfo structure needed to convert data
 * \param dataPath contains all data search paths.
 */
PRunDataHandler::PRunDataHandler(PAny2ManyInfo *any2ManyInfo, const PStringVector dataPath) :
    fAny2ManyInfo(any2ManyInfo), fDataPath(dataPath)
{
  Init(PHR_INIT_ANY2MANY);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor, reading the data histogramm files.
 *
 * \param msrInfo pointer to the msr-file handler
 */
PRunDataHandler::PRunDataHandler(PMsrHandler *msrInfo) : fMsrInfo(msrInfo)
{
  Init(PHR_INIT_MSR);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> Constructor, reading the data histogramm files, and keeping a copy
 * of potential search paths.
 *
 * \param msrInfo pointer to the msr-file handler
 * \param dataPath vector containing search paths where to find raw data files.
 */
PRunDataHandler::PRunDataHandler(PMsrHandler *msrInfo, const PStringVector dataPath) : 
      fMsrInfo(msrInfo), fDataPath(dataPath)
{
  Init(PHR_INIT_MSR);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p> Destructor.
 */
PRunDataHandler::~PRunDataHandler()
{
  fDataPath.clear();
  fData.clear();
}

//--------------------------------------------------------------------------
// GetRunData
//--------------------------------------------------------------------------
/**
 * <p>Checks if runName is found, and if so return these data.
 * runName is as given in the msr-file.
 *
 * <b>return:</b>
 * - if data are found: pointer to the data.
 * - otherwise the null pointer will be returned.
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
    return nullptr;
  else
    return &fData[i];
}

//--------------------------------------------------------------------------
// GetRunData
//--------------------------------------------------------------------------
/**
 * <p>return data-set with index idx.
 *
 * <b>return:</b>
 * - if data are found: pointer to the data.
 * - otherwise the null pointer will be returned.
 *
 * \param idx index of the raw data set.
 */
PRawRunData* PRunDataHandler::GetRunData(const UInt_t idx)
{
  if (idx >= fData.size())
    return nullptr;
  else
    return &fData[idx];
}

//--------------------------------------------------------------------------
// ReadData
//--------------------------------------------------------------------------
/**
 * <p>Read data. Used to read data, either msr-file triggered, or a single
 * explicit data file should be read.
 */
void PRunDataHandler::ReadData()
{
  if (fMsrInfo) { // i.e. msr-file triggered
    if (!ReadFilesMsr()) // couldn't read file
      fAllDataAvailable = false;
    else
      fAllDataAvailable = true;
  } else if (!fRunPathName.IsWhitespace()) { // i.e. file name triggered
    if (fFileFormat == "") { // try to guess the file format if fromat hasn't been provided
      TString ext=fRunPathName;
      Ssiz_t pos=ext.Last('.');
      ext.Remove(0, pos+1);
      if (!ext.CompareTo("bin", TString::kIgnoreCase))
        fFileFormat = "psibin";
      else if (!ext.CompareTo("root", TString::kIgnoreCase))
        fFileFormat = "musrroot";
      else if (!ext.CompareTo("msr", TString::kIgnoreCase))
        fFileFormat = "mud";
      else if (!ext.CompareTo("nxs", TString::kIgnoreCase))
        fFileFormat = "nexus";
      else if (!ext.CompareTo("mdu", TString::kIgnoreCase))
        fFileFormat = "psimdu";
    }
    if ((fFileFormat == "MusrRoot") || (fFileFormat == "musrroot")) {
      fAllDataAvailable = ReadRootFile();
    } else if ((fFileFormat == "NeXus") || (fFileFormat == "nexus")) {
      fAllDataAvailable = ReadNexusFile();
    } else if ((fFileFormat == "PsiBin") || (fFileFormat == "psibin") ||
               (fFileFormat == "PsiMdu") || (fFileFormat == "psimdu")) {
      fAllDataAvailable = ReadPsiBinFile();
    } else if ((fFileFormat == "Mud") || (fFileFormat == "mud")) {
      fAllDataAvailable = ReadMudFile();
    } else if ((fFileFormat == "Wkm") || (fFileFormat == "wkm")) {
      fAllDataAvailable = ReadWkmFile();
    } else {
      std::cerr << std::endl << ">> PRunDataHandler::ReadData(): **ERROR** unkown file format \"" << fFileFormat << "\" found." << std::endl;
      fAllDataAvailable = false;
    }
  } else {
    std::cerr << std::endl << ">> PRunDataHandler::ReadData(): **ERROR** Couldn't read files." << std::endl;
    fAllDataAvailable = false;
  }
}

//--------------------------------------------------------------------------
// ConvertData
//--------------------------------------------------------------------------
/**
 * <p>Read data and convert it. This routine is used by any2many.
 */
void PRunDataHandler::ConvertData()
{
  if (!ReadWriteFilesList()) // couldn't read file
    fAllDataAvailable = false;
  else
    fAllDataAvailable = true;
}

//--------------------------------------------------------------------------
// SetRunData
//--------------------------------------------------------------------------
/**
 * <p>Set a raw run data set.
 *
 * @param data pointer to the raw run data set
 * @param idx index to where to write it.
 *
 * @return true in case of success, false otherwise.
 */
Bool_t PRunDataHandler::SetRunData(PRawRunData *data, UInt_t idx)
{
  if ((idx == 0) && (fData.size() == 0)) {
    fData.resize(1);
  }
  if (idx >= fData.size()) {
    std::cerr << std::endl << ">>PRunDataHandler::SetRunData(): **ERROR** idx=" << idx << " is out-of-range (0.." << fData.size() << ")." << std::endl;
    return false;
  }

  fData[idx] = *data;

  return true;
}

//--------------------------------------------------------------------------
// WriteData
//--------------------------------------------------------------------------
/**
 * <p>Write data. This routine is used to write a single file.
 */
Bool_t PRunDataHandler::WriteData(TString fileName)
{
  if ((fAny2ManyInfo == nullptr) && (fileName="")) {
    std::cerr << std::endl << ">> PRunDataHandler::WriteData(): **ERROR** insufficient information: no fileName nor fAny2ManyInfo object.";
    std::cerr << std::endl << "        Cannot write data under this conditions." << std::endl;
    return false;
  }

  // get output file format tag, first try via fAny2ManyInfo
  Int_t outTag = A2M_UNDEFINED;
  if (fAny2ManyInfo != nullptr) {
    if (!fAny2ManyInfo->outFormat.CompareTo("musrroot", TString::kIgnoreCase))
      outTag = A2M_MUSR_ROOT;
    else if (!fAny2ManyInfo->outFormat.CompareTo("psibin", TString::kIgnoreCase))
      outTag = A2M_PSIBIN;
    else if (!fAny2ManyInfo->outFormat.CompareTo("psimdu", TString::kIgnoreCase))
      outTag = A2M_PSIMDU;
    else if (!fAny2ManyInfo->outFormat.CompareTo("mud",TString::kIgnoreCase))
      outTag = A2M_MUD;
    else if (fAny2ManyInfo->outFormat.BeginsWith("nexus", TString::kIgnoreCase))
      outTag = A2M_NEXUS;
    else
      outTag = A2M_UNDEFINED;
  } else { // only fileName is given, try to guess from the extension
    // STILL MISSING
  }

  if (outTag == A2M_UNDEFINED) {
    std::cerr << std::endl << ">> PRunDataHandler::WriteData(): **ERROR** no valid output data file format found: '" << fAny2ManyInfo->outFormat.Data() << "'" << std::endl;
    return false;
  }

  Bool_t success{true};
  switch (outTag) {
    case A2M_MUSR_ROOT:
      if (fAny2ManyInfo->outFileName.Length() == 0)
        success = WriteMusrRootFile(fileName);
      else
        success = WriteMusrRootFile(fAny2ManyInfo->outFileName);
      break;
    case A2M_PSIBIN:
    case A2M_PSIMDU:
      if (fAny2ManyInfo->outFileName.Length() == 0)
        success = WritePsiBinFile(fileName);
      else
        success = WritePsiBinFile(fAny2ManyInfo->outFileName);
      break;
    case A2M_MUD:
      if (fAny2ManyInfo->outFileName.Length() == 0)
        success = WriteMudFile(fileName);
      else
        success = WriteMudFile(fAny2ManyInfo->outFileName);
      break;
    case A2M_NEXUS:
      if (fAny2ManyInfo->outFileName.Length() == 0)
        success = WriteNexusFile(fileName);
      else
        success = WriteNexusFile(fAny2ManyInfo->outFileName);
      break;
    default:
      break;
  }

  return success;
}

//--------------------------------------------------------------------------
// Init (private)
//--------------------------------------------------------------------------
/**
 * <p>Common initializer.
 */
void PRunDataHandler::Init(const Int_t tag)
{
  if ((tag==PHR_INIT_ALL) || (tag==PHR_INIT_ANY2MANY))
    fMsrInfo = nullptr;
  if ((tag==PHR_INIT_ALL) || (tag==PHR_INIT_MSR))
    fAny2ManyInfo = nullptr;
  fAllDataAvailable = false;
  if (tag!=PHR_INIT_ALL)
    fFileFormat = TString("");
  fRunName = TString("");
  fRunPathName = TString("");
}

//--------------------------------------------------------------------------
// ReadFilesMsr (private)
//--------------------------------------------------------------------------
/**
 * <p> The main read file routine which is filtering what read sub-routine
 * needs to be called. Called when the input is a msr-file.
 *
 * <b>return:</b>
 * - true if reading was successful,
 * - false if reading failed.
 */
Bool_t PRunDataHandler::ReadFilesMsr()
{
  Bool_t success = true;

  // loop over the full RUN list to see what needs to be read
  PMsrRunList *runList = nullptr;
  runList = fMsrInfo->GetMsrRunList();
  if (runList == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadFilesMsr(): **ERROR** Couldn't obtain run list from PMsrHandler: something VERY fishy";
    std::cerr << std::endl;
    return false;
  }

  char str[1024], *p_str=nullptr;
  UInt_t year=0;
  TString musrRoot("musr-root");

  for (UInt_t i=0; i<runList->size(); i++) {
    for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) {
      fRunName = *(runList->at(i).GetRunName(j));
      // check if file actually exists
      if (!FileExistsCheck(runList->at(i), j))
        return false;

      // get year from string if LEM data file
      strcpy(str, fRunName.Data());
      p_str = strstr(str, "lem");
      if (p_str != nullptr)
        sscanf(p_str, "lem%d_his", &year);

      // check special case for ROOT-NPP/ROOT-PPC (LEM)
      if (!runList->at(i).GetFileFormat(j)->CompareTo("root-npp")) { // not post pile up corrected histos
        // if LEM file header is already TMusrRoot, change the data-file-format
        if (year >= 12)
          runList->at(i).SetFileFormat(musrRoot, j);
        // check if forward/backward histoNo are within proper bounds, i.e. < PRH_PPC_OFFSET
        for (UInt_t k=0; k<runList->at(i).GetForwardHistoNoSize(); k++) {
          if (runList->at(i).GetForwardHistoNo(k) > PRH_PPC_OFFSET)
            runList->at(i).SetForwardHistoNo(runList->at(i).GetForwardHistoNo(k)-PRH_PPC_OFFSET, k);
        }
        for (UInt_t k=0; k<runList->at(i).GetBackwardHistoNoSize(); k++) {
          if (runList->at(i).GetBackwardHistoNo(k) > PRH_PPC_OFFSET)
            runList->at(i).SetBackwardHistoNo(runList->at(i).GetBackwardHistoNo(k)-PRH_PPC_OFFSET, k);
        }
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("root-ppc")) { // post pile up corrected histos
        // if LEM file header is already TMusrRoot, change the data-file-format
        if (year >= 12)
          runList->at(i).SetFileFormat(musrRoot, j);
        // check if forward/backward histoNo are within proper bounds, i.e. > PRH_PPC_OFFSET
        for (UInt_t k=0; k<runList->at(i).GetForwardHistoNoSize(); k++) {
          if (runList->at(i).GetForwardHistoNo(k) < PRH_PPC_OFFSET)
            runList->at(i).SetForwardHistoNo(runList->at(i).GetForwardHistoNo(k)+PRH_PPC_OFFSET, k);
        }
        for (UInt_t k=0; k<runList->at(i).GetBackwardHistoNoSize(); k++) {
          if (runList->at(i).GetBackwardHistoNo(k) < PRH_PPC_OFFSET)
            runList->at(i).SetBackwardHistoNo(runList->at(i).GetBackwardHistoNo(k)+PRH_PPC_OFFSET, k);
        }
      }

      // check is file is already read
      if (FileAlreadyRead(*(runList->at(i).GetRunName(j))))
        continue;
      // everything looks fine, hence try to read the data file
      if (!runList->at(i).GetFileFormat(j)->CompareTo("root-npp")) { // not post pile up corrected histos
        success = ReadRootFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("root-ppc")) { // post pile up corrected histos
        success = ReadRootFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("musr-root")) { // MusrRoot style file
        success = ReadRootFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("nexus")) {
        success = ReadNexusFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("psi-bin")) {
        success = ReadPsiBinFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("psi-mdu")) {
        success = ReadPsiBinFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("mud")) {
        success = ReadMudFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("wkm")) {
        success = ReadWkmFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("mdu-ascii")) {
        success = ReadMduAsciiFile();        
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("ascii")) {
        success = ReadAsciiFile();
      } else if (!runList->at(i).GetFileFormat(j)->CompareTo("db")) {
        success = ReadDBFile();
      } else {
        success = false;
      }
    }
  }

  return success;
}

//--------------------------------------------------------------------------
// ReadWriteFilesList (private)
//--------------------------------------------------------------------------
/**
 * <p> The main read file routine which is filtering what read sub-routine
 * needs to be called. Called when the input is a list of runs. Used with
 * any2many. If the input data file is successfully read, it will write the
 * converted data file.
 *
 * <b>return:</b>
 * - true if reading was successful,
 * - false if reading failed.
 */
Bool_t PRunDataHandler::ReadWriteFilesList()
{
  if ((fAny2ManyInfo->inFileName.size() == 0) && (fAny2ManyInfo->runList.size() == 0)) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't obtain run list from fAny2ManyInfo: something VERY fishy";
    std::cerr << std::endl;
    return false;
  }

  // get input file format tag
  Int_t inTag = A2M_UNDEFINED;
  if (!fAny2ManyInfo->inFormat.CompareTo("root", TString::kIgnoreCase))
    inTag = A2M_ROOT;
  else if (!fAny2ManyInfo->inFormat.CompareTo("musrroot", TString::kIgnoreCase))
    inTag = A2M_MUSR_ROOT;
  else if (!fAny2ManyInfo->inFormat.CompareTo("psi-bin", TString::kIgnoreCase))
    inTag = A2M_PSIBIN;
  else if (!fAny2ManyInfo->inFormat.CompareTo("psi-mdu", TString::kIgnoreCase))
    inTag = A2M_PSIMDU;
  else if (!fAny2ManyInfo->inFormat.CompareTo("mud", TString::kIgnoreCase))
    inTag = A2M_MUD;
  else if (!fAny2ManyInfo->inFormat.CompareTo("nexus", TString::kIgnoreCase))
    inTag = A2M_NEXUS;
  else if (!fAny2ManyInfo->inFormat.CompareTo("wkm", TString::kIgnoreCase))
    inTag = A2M_WKM;
  else
    inTag = A2M_UNDEFINED;

  if (inTag == A2M_UNDEFINED) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** no valid input data file format found: '" << fAny2ManyInfo->inFormat.Data() << "'" << std::endl;
    return false;
  }

  // get output file format tag
  Int_t outTag = A2M_UNDEFINED;
  if (!fAny2ManyInfo->outFormat.CompareTo("root", TString::kIgnoreCase))
    outTag = A2M_ROOT;
  else if (!fAny2ManyInfo->outFormat.CompareTo("musrroot", TString::kIgnoreCase))
    outTag = A2M_MUSR_ROOT;
  else if (!fAny2ManyInfo->outFormat.CompareTo("psi-bin", TString::kIgnoreCase))
    outTag = A2M_PSIBIN;
  else if (!fAny2ManyInfo->outFormat.CompareTo("mud",TString::kIgnoreCase))
    outTag = A2M_MUD;
  else if (fAny2ManyInfo->outFormat.BeginsWith("nexus", TString::kIgnoreCase))
    outTag = A2M_NEXUS;
  else if (!fAny2ManyInfo->outFormat.CompareTo("wkm", TString::kIgnoreCase))
    outTag = A2M_WKM;
  else if (!fAny2ManyInfo->outFormat.CompareTo("ascii", TString::kIgnoreCase))
    outTag = A2M_ASCII;
  else
    outTag = A2M_UNDEFINED;

  if (outTag == A2M_UNDEFINED) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** no valid output data file format found: '" << fAny2ManyInfo->outFormat.Data() << "'" << std::endl;
    return false;
  }

  if (fAny2ManyInfo->inFileName.size() != 0) { // file name list given

    // loop over all runs of the run list
    for (UInt_t i=0; i<fAny2ManyInfo->inFileName.size(); i++) {
      if (!FileExistsCheck(true, i)) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't find file " << fAny2ManyInfo->inFileName[i].Data() << std::endl;
        return false;
      }

      // read input file
      Bool_t success = false;
      switch (inTag) {
      case A2M_ROOT:
      case A2M_MUSR_ROOT:
        success = ReadRootFile();
        break;
      case A2M_PSIBIN:
      case A2M_PSIMDU:
        success = ReadPsiBinFile();
        break;
      case A2M_NEXUS:
        success = ReadNexusFile();
        break;
      case A2M_MUD:
        success = ReadMudFile();
        break;
      case A2M_WKM:
        success = ReadWkmFile();
        break;
      default:
        break;
      }

      if (!success) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't read file " << fAny2ManyInfo->inFileName[i].Data() << std::endl;
        return false;
      }

      // write 'converted' output data file
      success = false;
      switch (outTag) {
      case A2M_ROOT:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WriteRootFile();
        else
          success = WriteRootFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_MUSR_ROOT:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WriteMusrRootFile();
        else
          success = WriteMusrRootFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_PSIBIN:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WritePsiBinFile();
        else
          success = WritePsiBinFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_PSIMDU:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WritePsiBinFile();
        else
          success = WritePsiBinFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_NEXUS:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WriteNexusFile();
        else
          success = WriteNexusFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_MUD:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WriteMudFile();
        else
          success = WriteMudFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_WKM:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WriteWkmFile();
        else
          success = WriteWkmFile(fAny2ManyInfo->outFileName);
        break;
      case A2M_ASCII:
        if (fAny2ManyInfo->outFileName.Length() == 0)
          success = WriteAsciiFile();
        else
          success = WriteAsciiFile(fAny2ManyInfo->outFileName);
        break;
      default:
        break;
      }

      if (success == false) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't write converted output file." << std::endl;
        return false;
      }

      // throw away the current data set
      fData.clear();
    }
  }

  if (fAny2ManyInfo->runList.size() != 0) { // run list given

    // loop over all runs of the run list
    for (UInt_t i=0; i<fAny2ManyInfo->runList.size(); i++) {
      if (!FileExistsCheck(false, i)) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't find run " << fAny2ManyInfo->runList[i] << std::endl;
        return false;
      }

      // read input file
      Bool_t success = false;
      switch (inTag) {
      case A2M_ROOT:
      case A2M_MUSR_ROOT:
        success = ReadRootFile();
        break;
      case A2M_PSIBIN:
      case A2M_PSIMDU:
        success = ReadPsiBinFile();
        break;
      case A2M_NEXUS:
        success = ReadNexusFile();
        break;
      case A2M_MUD:
        success = ReadMudFile();
        break;
      case A2M_WKM:
        success = ReadWkmFile();
        break;
      default:
        break;
      }

      if (!success) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't read file " << fRunPathName.Data() << std::endl;
        return false;
      }

      TString year("");
      TDatime dt;
      year += dt.GetYear();
      if (fAny2ManyInfo->year.Length() > 0)
        year = fAny2ManyInfo->year;
      Bool_t ok;
      TString fln = FileNameFromTemplate(fAny2ManyInfo->outTemplate, fAny2ManyInfo->runList[i], year, ok);
      if (!ok) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't create necessary output file name." << std::endl;
        return false;
      }

      // write 'converted' output data file
      success = false;
      switch (outTag) {
      case A2M_ROOT:
        success = WriteRootFile(fln);
        break;
      case A2M_MUSR_ROOT:
        success = WriteMusrRootFile(fln);
        break;
      case A2M_PSIBIN:
        success = WritePsiBinFile(fln);
        break;
      case A2M_PSIMDU:
        success = WritePsiBinFile(fln);
        break;
      case A2M_NEXUS:
        success = WriteNexusFile(fln);
        break;
      case A2M_MUD:
        success = WriteMudFile(fln);
        break;
      case A2M_WKM:
        success = WriteWkmFile(fln);
        break;
      case A2M_ASCII:
        success = WriteAsciiFile(fln);
        break;
      default:
        break;
      }

      if (success == false) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't write converted output file." << std::endl;
        return false;
      }

      // throw away the current data set
      fData.clear();
    }

  }

  // check if compression is wished
  if (fAny2ManyInfo->compressionTag > 0) {
    TString fln = fAny2ManyInfo->outPath + fAny2ManyInfo->compressFileName;

    // currently system call is used, which means this is only running under Linux and Mac OS X but not under Windows
    char cmd[256];
    if (fAny2ManyInfo->outPathFileName.size() == 1) {
      if (fAny2ManyInfo->compressionTag == 1) // gzip
        fln += TString(".tar.gz");
      else // bzip2
        fln += TString(".tar.bz2");
      if (fAny2ManyInfo->compressionTag == 1) // gzip
        sprintf(cmd, "tar -zcf %s %s", fln.Data(), fAny2ManyInfo->outPathFileName[0].Data());
      else // bzip2
        sprintf(cmd, "tar -jcf %s %s", fln.Data(), fAny2ManyInfo->outPathFileName[0].Data());
      if (system(cmd) == -1) {
        std::cerr << "**ERROR** cmd: " << cmd << " failed." << std::endl;
      }
    } else {
      fln += TString(".tar");
      for (UInt_t i=0; i<fAny2ManyInfo->outPathFileName.size(); i++) {
        if (i==0) {
          sprintf(cmd, "tar -cf %s %s", fln.Data(), fAny2ManyInfo->outPathFileName[i].Data());
        } else {
          sprintf(cmd, "tar -rf %s %s", fln.Data(), fAny2ManyInfo->outPathFileName[i].Data());
        }
        if (system(cmd) == -1) {
          std::cerr << "**ERROR** cmd: " << cmd << " failed." << std::endl;
        }
      }
      if (fAny2ManyInfo->compressionTag == 1) { // gzip
        sprintf(cmd, "gzip %s", fln.Data());
        fln += ".gz";
      } else {
        sprintf(cmd, "bzip2 -z %s", fln.Data());
        fln += ".bz2";
      }
      if (system(cmd) == -1) {
        std::cerr << "**ERROR** cmd: " << cmd << " failed." << std::endl;
      }
    }

    // check if the compressed file shall be streamed to the stdout
    if (fAny2ManyInfo->useStandardOutput) {
      // stream file to stdout
      std::ifstream is;
      int length=1024;
      char *buffer;

      is.open(fln.Data(), std::ios::binary);
      if (!is.is_open()) {
        std::cerr << std::endl << "PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't open the file for streaming." << std::endl;
        remove(fln.Data());
        return false;
      }

      // get length of file
      is.seekg(0, std::ios::end);
      length = is.tellg();
      is.seekg(0, std::ios::beg);

      if (length == -1) {
        std::cerr << std::endl << "PRunDataHandler::ReadWriteFilesList(): **ERROR** Couldn't determine the file size." << std::endl;
        remove(fln.Data());
        return false;
      }

      // allocate memory
      buffer = new char [length];

      // read data as a block
      while (!is.eof()) {
        is.read(buffer, length);
        std::cout.write(buffer, length);
      }

      is.close();

      delete [] buffer;

      // delete temporary root file
      remove(fln.Data());
    }

    // remove all the converted files
    for (UInt_t i=0; i<fAny2ManyInfo->outPathFileName.size(); i++)
      remove(fAny2ManyInfo->outPathFileName[i].Data());
  }

  return true;
}

//--------------------------------------------------------------------------
// FileAlreadyRead (private)
//--------------------------------------------------------------------------
/**
 * <p> Checks if a file has been already read in order to prevent multiple
 * reading of data files.
 *
 * <b>return:</b>
 * - true if the file has been read before,
 * - otherwise false.
 *
 * \param runName run name to be check if the corresponding file is already read.
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
// TestFileName (private)
//--------------------------------------------------------------------------
/**
 * <p> Tests if a file exists (with or without given extension).
 *     The given file-name string will be modified to show the found file name or an empty string if no file is found.
 *
 * \param runName run name with full path to be checked
 * \param ext extension to be checked
 */
void PRunDataHandler::TestFileName(TString &runName, const TString &ext)
{
  TString tmpStr(runName), tmpExt(ext);

  // check first if the file exists with the default extension which is not given with the run name
  tmpStr += TString(".") + ext;
  if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
    runName = tmpStr;
    return;
  }

  // test if the file exists with the given run name but an only upper-case extension which is not included in the file name
  tmpStr = runName;
  tmpExt.ToUpper();
  tmpStr += TString(".") + tmpExt;
  if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
    runName = tmpStr;
    return;
  }

  // test if the file exists with the given run name but an only lower-case extension which is not included in the file name
  tmpStr = runName;
  tmpExt.ToLower();
  tmpStr += TString(".") + tmpExt;
  if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
    runName = tmpStr;
    return;
  }

  // test if the file exists with only upper-case letters
  tmpStr = runName + TString(".") + tmpExt;
  tmpStr.ToUpper();
  if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
    runName = tmpStr;
    return;
  }

  // test if the file exists with only lower-case letters
  tmpStr.ToLower();
  if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
    runName = tmpStr;
    return;
  }

  tmpStr = runName;

  // the extension is already part of the file name, therefore, do not append it
  if (tmpStr.EndsWith(ext.Data(), TString::kIgnoreCase)) {
    if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
      runName = tmpStr;
      return;
    }

    // assume some extension is part of the given file name but the real data file ends with an lower-case extension
    tmpExt.ToLower();
    tmpStr = tmpStr.Replace(static_cast<Ssiz_t>(tmpStr.Length()-tmpExt.Length()), tmpExt.Length(), tmpExt);
    if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
      runName = tmpStr;
      return;
    }

    // assume some extension is part of the given file name but the real data file ends with an upper-case extension
    tmpExt.ToUpper();
    tmpStr = runName;
    tmpStr = tmpStr.Replace(static_cast<Ssiz_t>(tmpStr.Length()-tmpExt.Length()), tmpExt.Length(), tmpExt);
    if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
      runName = tmpStr;
      return;
    }

    // test if the file exists with only lower-case letters and the extension already included in the file name
    tmpStr = runName;
    tmpStr.ToLower();
    if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
      runName = tmpStr;
      return;
    }

    // test if the file exists with only upper-case letters and the extension already included in the file name
    tmpStr = runName;
    tmpStr.ToUpper();
    if (gSystem->AccessPathName(tmpStr.Data()) != true) { // found
      runName = tmpStr;
      return;
    }
  }

  // if the file has not been found, set the run name to be empty
  runName = "";

  return;
}

//--------------------------------------------------------------------------
// FileExistsCheck (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if a given data file exists.
 *
 * <b>return:</b>
 * - true if data file exists,
 * - otherwise false.
 *
 * \param runInfo reference to the msr-run-structure
 * \param idx index of the run (needed for ADDRUN feature).
 */
Bool_t PRunDataHandler::FileExistsCheck(PMsrRunBlock &runInfo, const UInt_t idx)
{
  Bool_t success = true;

  // local init
  TROOT root("PRunBase", "PRunBase", nullptr);
  TString pathName = "???";
  TString str, *pstr;
  TString ext;

  pstr = runInfo.GetBeamline(idx);
  if (pstr == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain beamline data." << std::endl;
    assert(0);
  }
  pstr->ToLower();
  runInfo.SetBeamline(*pstr, idx);
  pstr = runInfo.GetInstitute(idx);
  if (pstr == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain institute data." << std::endl;
    assert(0);
  }
  pstr->ToLower();
  runInfo.SetInstitute(*pstr, idx);
  pstr = runInfo.GetFileFormat(idx);
  if (pstr == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain file format data." << std::endl;
    assert(0);
  }
  pstr->ToLower();
  runInfo.SetFileFormat(*pstr, idx);

  // file extensions for the various formats
  if (!runInfo.GetFileFormat(idx)->CompareTo("root-npp")) // not post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("root-ppc")) // post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("musr-root")) // post pile up corrected histos
    ext = TString("root");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("nexus"))
    ext = TString("NXS");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("psi-bin"))
    ext = TString("bin");
  else if (!runInfo.GetFileFormat(idx)->CompareTo("psi-mdu"))
    ext = TString("mdu");
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
    if (pstr == nullptr) {
      std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain file format data." << std::endl;
      assert(0);
    }
    pstr->ToUpper();
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** File Format '" << pstr->Data() << "' unsupported.";
    std::cerr << std::endl << ">>   support file formats are:";
    std::cerr << std::endl << ">>   ROOT-NPP   -> root not post pileup corrected for lem";
    std::cerr << std::endl << ">>   ROOT-PPC   -> root post pileup corrected for lem";
    std::cerr << std::endl << ">>   MUSR-ROOT  -> MusrRoot file format";
    std::cerr << std::endl << ">>   NEXUS      -> nexus file format, HDF4, HDF5, or XML";
    std::cerr << std::endl << ">>   PSI-BIN    -> psi bin file format";
    std::cerr << std::endl << ">>   PSI-MDU    -> psi mdu file format (see also MDU-ASCII)";
    std::cerr << std::endl << ">>   MUD        -> triumf mud file format";
    std::cerr << std::endl << ">>   WKM        -> wkm ascii file format";
    std::cerr << std::endl << ">>   MDU-ASCII  -> psi mdu ascii file format";
    std::cerr << std::endl << ">>   ASCII      -> column like file format";
    std::cerr << std::endl << ">>   DB         -> triumf db file \"format\"";
    std::cerr << std::endl;
    return success;
  }

  // check if the file is in the local directory
  str = *runInfo.GetRunName(idx);
  TestFileName(str, ext);
  if (!str.IsNull()) {
    pathName = str;
  }

  // check if the file is found in the <msr-file-directory>
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    str = *fMsrInfo->GetMsrFileDirectoryPath() + *runInfo.GetRunName(idx);
    TestFileName(str, ext);
    if (!str.IsNull()) {
      pathName = str;
    }
  }

  // check if the file is found in the directory given in the startup file
  if (pathName.CompareTo("???") == 0) { // not found in local directory search and not found in the <msr-file-directory> search
    for (UInt_t i=0; i<fDataPath.size(); i++) {
      str = fDataPath[i] + TString("/") + *runInfo.GetRunName(idx);
      TestFileName(str, ext);
      if (!str.IsNull()) {
        pathName = str;
        break;
      }
    }
  }

  // check if the file is found in the directories given by MUSRFULLDATAPATH
  const Char_t *musrpath = gSystem->Getenv("MUSRFULLDATAPATH");
  if (pathName.CompareTo("???") == 0) { // not found in local directory and xml path
    str = TString(musrpath);
    // MUSRFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/") + *runInfo.GetRunName(idx);
      TestFileName(str, ext);
      if (!str.IsNull()) {
        pathName = str;
        break;
      }
    }
    // cleanup
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  // check if the file is found in the generated default path
  if (pathName.CompareTo("???") == 0) { // not found in MUSRFULLDATAPATH search
    str = TString(musrpath);
    // MUSRFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    pstr = runInfo.GetInstitute(idx);
    if (pstr == nullptr) {
      std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain institute data." << std::endl;
      assert(0);
    }
    pstr->ToUpper();
    runInfo.SetInstitute(*pstr, idx);
    pstr = runInfo.GetBeamline(idx);
    if (pstr == nullptr) {
      std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck: **ERROR** Couldn't obtain beamline data." << std::endl;
      assert(0);
    }
    pstr->ToUpper();
    runInfo.SetBeamline(*pstr, idx);
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/DATA/") +
            *runInfo.GetInstitute(idx) + TString("/") +
            *runInfo.GetBeamline(idx) + TString("/") +
            *runInfo.GetRunName(idx);
      TestFileName(str, ext);
      if (!str.IsNull()) { // found
        pathName = str;
        break;
      }
    }
    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  // no proper path name found
  if (pathName.CompareTo("???") == 0) {
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck(): **ERROR** Couldn't find '" << runInfo.GetRunName(idx)->Data() << "' in any standard path.";
    std::cerr << std::endl << ">>  standard search pathes are:";
    std::cerr << std::endl << ">>  1. the local directory";
    std::cerr << std::endl << ">>  2. the data directory given in the startup XML file";
    std::cerr << std::endl << ">>  3. the directories listed in MUSRFULLDATAPATH";
    std::cerr << std::endl << ">>  4. default path construct which is described in the manual";
    return false;
  }

  fRunPathName = pathName;

  return true;
}

//--------------------------------------------------------------------------
// FileExistsCheck (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if a given data file exists. Used for the any2many program.
 *
 * <b>return:</b>
 * - true if data file exists,
 * - otherwise false.
 *
 * \param fileName flag showing if a file name shall be handled or a run number
 * \param idx index of the run. idx == -1 means that a single input data file name is given.
 */
Bool_t PRunDataHandler::FileExistsCheck(const Bool_t fileName, const Int_t idx)
{
  TString pathName("???");
  TString str("");
  TString fln("");

  if (fileName) { // single input file name
    if (idx >= static_cast<Int_t>(fAny2ManyInfo->inFileName.size())) {
      std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck(): **ERROR** idx=" << idx << " out of range. (inFileName.size()==" << fAny2ManyInfo->inFileName.size() << ")" << std::endl;
      return false;
    }
    fln = fAny2ManyInfo->inFileName[idx];
  } else { // run file list entry shall be handled
    if (idx >= static_cast<Int_t>(fAny2ManyInfo->runList.size())) {
      std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck(): **ERROR** idx=" << idx << " out of range. (inFileName.size()==" << fAny2ManyInfo->runList.size() << ")" << std::endl;
      return false;
    }
    // check for input/output templates
    if ((fAny2ManyInfo->inTemplate.Length() == 0) || (fAny2ManyInfo->outTemplate.Length() == 0)) {
      std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck(): **ERROR** when using run lists, input/output templates are needed as well." << std::endl;
      return false;
    }
    // make the input file name according to the input template
    TString year("");
    TDatime dt;
    year += dt.GetYear();
    if (fAny2ManyInfo->year.Length() > 0)
      year = fAny2ManyInfo->year;
    Bool_t ok;
    fln = FileNameFromTemplate(fAny2ManyInfo->inTemplate, fAny2ManyInfo->runList[idx], year, ok);
    if (!ok)
      return false;
  }

  // check if the file is in the local directory
  if (gSystem->AccessPathName(fln) != true) { // found in the local dir
    pathName = fln;
  }
  // check if the file is found in the directory given in the startup file
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    for (UInt_t i=0; i<fDataPath.size(); i++) {
      str = fDataPath[i] + TString("/") + fln;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }
  // check if the file is found in the directories given by MUSRFULLDATAPATH
  const Char_t *musrpath = gSystem->Getenv("MUSRFULLDATAPATH");
  if (pathName.CompareTo("???") == 0) { // not found in local directory and xml path
    str = TString(musrpath);
    // MUSRFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/") + fln;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
    // cleanup
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }
  // no proper path name found
  if (pathName.CompareTo("???") == 0) {
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck(): **ERROR** Couldn't find '" << fln.Data() << "' in any standard path.";
    std::cerr << std::endl << ">>  standard search pathes are:";
    std::cerr << std::endl << ">>  1. the local directory";
    std::cerr << std::endl << ">>  2. the data directory given in the startup XML file";
    std::cerr << std::endl << ">>  3. the directories listed in MUSRFULLDATAPATH";
    return false;
  }

  fRunPathName = pathName;

  return true;
}

//--------------------------------------------------------------------------
// FileExistsCheck (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if a given data file exists. Used for the any2many program.
 *
 * <b>return:</b>
 * - true if data file exists,
 * - otherwise false.
 *
 * \param fileName file name
 */
Bool_t PRunDataHandler::FileExistsCheck(const TString fileName)
{
  TString pathName("???");
  TString str("");

  // check if the file is in the local directory
  if (gSystem->AccessPathName(fileName) != true) { // found in the local dir
    pathName = fileName;
  }
  // check if the file is found in the directory given in the startup file
  if (pathName.CompareTo("???") == 0) { // not found in local directory search
    for (UInt_t i=0; i<fDataPath.size(); i++) {
      str = fDataPath[i] + TString("/") + fileName;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
  }
  // check if the file is found in the directories given by MUSRFULLDATAPATH
  const Char_t *musrpath = gSystem->Getenv("MUSRFULLDATAPATH");
  if (pathName.CompareTo("???") == 0) { // not found in local directory and xml path
    str = TString(musrpath);
    // MUSRFULLDATAPATH has the structure: path_1:path_2:...:path_n
    TObjArray *tokens = str.Tokenize(":");
    TObjString *ostr;
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString() + TString("/") + fileName;
      if (gSystem->AccessPathName(str.Data())!=true) { // found
        pathName = str;
        break;
      }
    }
    // cleanup
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }
  // no proper path name found
  if (pathName.CompareTo("???") == 0) {
    std::cerr << std::endl << ">> PRunDataHandler::FileExistsCheck(): **ERROR** Couldn't find '" << fileName.Data() << "' in any standard path.";
    std::cerr << std::endl << ">>  standard search pathes are:";
    std::cerr << std::endl << ">>  1. the local directory";
    std::cerr << std::endl << ">>  2. the data directory given in the startup XML file";
    std::cerr << std::endl << ">>  3. the directories listed in MUSRFULLDATAPATH";
    return false;
  }

  fRunPathName = pathName;

  return true;
}

//--------------------------------------------------------------------------
// ReadRootFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Reads both, the "old" LEM-data ROOT-files with TLemRunHeader, and the more general
 * new MusrRoot file.
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadRootFile()
{
  PDoubleVector  histoData;
  PRawRunData    runData;
  PRawRunDataSet dataSet;

  TFile f(fRunPathName.Data());
  if (f.IsZombie()) {
    return false;
  }

  UInt_t fileType = PRH_MUSR_ROOT;

  TFolder *folder;
  f.GetObject("RunInfo", folder); // try first LEM-ROOT style file (used until 2011).
  if (!folder) { // either something is wrong, or it is a MusrRoot file
    f.GetObject("RunHeader", folder);
    if (!folder) { // something is wrong!!
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't neither obtain RunInfo (LEM),";
      std::cerr << std::endl << "   nor RunHeader (MusrRoot) from " << fRunPathName.Data() << std::endl;
      f.Close();
      return false;
    } else {
      fileType = PRH_MUSR_ROOT;
    }
  } else {
    fileType = PRH_LEM_ROOT;
  }

  if (fileType == PRH_LEM_ROOT) {
    // read header and check if some missing run info need to be fed
    TLemRunHeader *runHeader = dynamic_cast<TLemRunHeader*>(folder->FindObjectAny("TLemRunHeader"));

    // check if run header is valid
    if (!runHeader) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't obtain run header info from ROOT file " << fRunPathName.Data() << std::endl;
      f.Close();
      return false;
    }

    // set laboratory / beamline / instrument
    runData.SetLaboratory("PSI");
    runData.SetBeamline("muE4");
    runData.SetInstrument("LEM");
    runData.SetMuonSource("low energy muon source");
    runData.SetMuonSpecies("positive muons");

    // get run title
    TObjString ostr = runHeader->GetRunTitle();
    runData.SetRunTitle(ostr.GetString());

    // get run number
    runData.SetRunNumber(static_cast<Int_t>(runHeader->GetRunNumber()));

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

    // get start time/date
    // start date
    time_t idt = static_cast<time_t>(runHeader->GetStartTime());
    runData.SetStartDateTime(idt);
    struct tm *dt = localtime(&idt);
    char str[128];
    strftime(str, sizeof(str), "%F", dt);
    TString stime(str);
    runData.SetStartDate(stime);
    // start time
    memset(str, 0, sizeof(str));
    strftime(str, sizeof(str), "%T", dt);
    stime = str;
    runData.SetStartTime(stime);

    // get stop time/date
    // stop date
    idt = static_cast<time_t>(runHeader->GetStopTime());
    runData.SetStopDateTime(idt);
    dt = localtime(&idt);
    memset(str, 0, sizeof(str));
    strftime(str, sizeof(str), "%F", dt);
    stime = str;
    runData.SetStopDate(stime);
    // stop time
    memset(str, 0, sizeof(str));
    strftime(str, sizeof(str), "%T", dt);
    stime = str;
    runData.SetStopTime(stime);

    // get time resolution
    runData.SetTimeResolution(runHeader->GetTimeResolution());

    // get number of histogramms
    Int_t noOfHistos = runHeader->GetNHist();

    // get t0's there will be handled together with the data
    Double_t *t0 = runHeader->GetTimeZero();

    // read run summary to obtain ring anode HV values
    TObjArray *runSummary = dynamic_cast<TObjArray*>(folder->FindObjectAny("RunSummary"));

    // check if run summary is valid
    if (!runSummary) {
      std::cout << std::endl << "**INFO** Couldn't obtain run summary info from ROOT file " << fRunPathName.Data() << std::endl;
      // this is not fatal... only RA-HV values are not available
    } else { // it follows a (at least) little bit strange extraction of the RA values from Thomas' TObjArray...
      //streaming of a ASCII-file would be more easy
      TString s;
      TObjArrayIter summIter(runSummary);
      TObjString *os(dynamic_cast<TObjString*>(summIter.Next()));
      TObjArray *oa(nullptr);
      TObjString *objTok(nullptr);
      while (os != nullptr) {
        s = os->GetString();
        // will put four parallel if's since it may be that more than one RA-values are on one line
        if (s.Contains("RA-L")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr) {
            if (!objTok->GetString().CompareTo("RA-L")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(0, objTok->GetString().Atof()); // fill RA-R value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        if (s.Contains("RA-R")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr){
            if (!objTok->GetString().CompareTo("RA-R")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if (objTok != nullptr && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(1, objTok->GetString().Atof()); // fill RA-R value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        if (s.Contains("RA-T")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr){
            if (!objTok->GetString().CompareTo("RA-T")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(2, objTok->GetString().Atof()); // fill RA-T value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        if (s.Contains("RA-B")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr){
            if (!objTok->GetString().CompareTo("RA-B")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(3, objTok->GetString().Atof()); // fill RA-B value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        os = dynamic_cast<TObjString*>(summIter.Next()); // next summary line...
      }
    }

    // read data ---------------------------------------------------------

    // check if histos folder is found
    f.GetObject("histos", folder);
    if (!folder) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't obtain histos from " << fRunPathName.Data() << std::endl;
      f.Close();
      return false;
    }

    // get all the data
    Char_t histoName[32];
    for (Int_t i=0; i<noOfHistos; i++) {
      sprintf(histoName, "hDecay%02d", i);
      TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
      if (!histo) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't get histo " << histoName;
        std::cerr << std::endl;
        f.Close();
        return false;
      }

      // fill data
      for (Int_t j=1; j<=histo->GetNbinsX(); j++) {
        histoData.push_back(histo->GetBinContent(j));
      }

      // fill the data set
      dataSet.Clear();
      dataSet.SetName(histo->GetTitle());
      dataSet.SetHistoNo(i+1);
      dataSet.SetTimeZeroBin(t0[i]);
      dataSet.SetTimeZeroBinEstimated(histo->GetMaximumBin());
      dataSet.SetFirstGoodBin(static_cast<Int_t>(t0[i]));
      dataSet.SetLastGoodBin(histo->GetNbinsX()-1);
      dataSet.SetData(histoData);
      runData.SetDataSet(dataSet);

      // clear histoData for the next histo
      histoData.clear();
    }
    // check if any post pileup histos are present at all (this is not the case for LEM data 2006 and earlier)
    sprintf(histoName, "hDecay%02d", POST_PILEUP_HISTO_OFFSET);
    if (!folder->FindObjectAny(histoName)) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **WARNING** Couldn't get histo " << histoName;
      std::cerr << std::endl << ">>   most probably this is an old (2006 or earlier) LEM file without post pileup histos.";
      std::cerr << std::endl;
    } else {
      for (Int_t i=0; i<noOfHistos; i++) {
        sprintf(histoName, "hDecay%02d", i+POST_PILEUP_HISTO_OFFSET);
        TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
        if (!histo) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't get histo " << histoName;
          std::cerr << std::endl;
          f.Close();
          return false;
        }

        // fill data
        for (Int_t j=1; j<=histo->GetNbinsX(); j++)
          histoData.push_back(histo->GetBinContent(j));

        // fill the data set
        dataSet.Clear();
        dataSet.SetName(histo->GetTitle());
        dataSet.SetHistoNo(i+1+POST_PILEUP_HISTO_OFFSET);
        dataSet.SetTimeZeroBin(t0[i]);
        dataSet.SetTimeZeroBinEstimated(histo->GetMaximumBin());
        dataSet.SetFirstGoodBin(static_cast<Int_t>(t0[i]));
        dataSet.SetLastGoodBin(histo->GetNbinsX()-1);
        dataSet.SetData(histoData);
        runData.SetDataSet(dataSet);

        // clear histoData for the next histo
        histoData.clear();
      }
    }
  } else { // MusrRoot file
    // invoke the MusrRoot header object
    TMusrRunHeader *header = new TMusrRunHeader(true); // read quite
    if (header == nullptr) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't invoke MusrRoot RunHeader in file:" << fRunPathName;
      std::cerr << std::endl;
      f.Close();
      return false;
    }

    // try to populate the MusrRoot header object
    if (!header->ExtractAll(folder)) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't invoke MusrRoot RunHeader in file:" << fRunPathName;
      std::cerr << std::endl;
      f.Close();
      return false;
    }

    // get all the header information
    Bool_t ok;
    TString str, path, pathName;
    Int_t ival, noOfHistos=0;
    Double_t dval;
    TMusrRunPhysicalQuantity prop;
    PIntVector ivec, redGreenOffsets;

    header->Get("RunInfo/Version", str, ok);
    if (ok)
      runData.SetVersion(str);

    header->Get("RunInfo/Generic Validator URL", str, ok);
    if (ok)
      runData.SetGenericValidatorUrl(str);

    header->Get("RunInfo/Specific Validator URL", str, ok);
    if (ok)
      runData.SetSpecificValidatorUrl(str);

    header->Get("RunInfo/Generator", str, ok);
    if (ok)
      runData.SetGenerator(str);

    header->Get("RunInfo/File Name", str, ok);
    if (ok)
      runData.SetFileName(str);

    header->Get("RunInfo/Run Title", str, ok);
    if (ok)
      runData.SetRunTitle(str);

    header->Get("RunInfo/Run Number", ival, ok);
    if (ok)
      runData.SetRunNumber(ival);

    header->Get("RunInfo/Run Start Time", str, ok);
    if (ok) {
      Ssiz_t pos = str.Index(' ');
      TString substr = str;
      substr.Remove(pos, str.Length());
      runData.SetStartDate(substr);
      substr = str;
      substr.Remove(0, pos+1);
      runData.SetStartTime(substr);
    }

    header->Get("RunInfo/Run Stop Time", str, ok);
    if (ok) {
      Ssiz_t pos = str.Index(' ');
      TString substr = str;
      substr.Remove(pos, str.Length());
      runData.SetStopDate(substr);
      substr = str;
      substr.Remove(0, pos+1);
      runData.SetStopTime(substr);
    }

    header->Get("RunInfo/Laboratory", str, ok);
    if (ok)
      runData.SetLaboratory(str);

    header->Get("RunInfo/Instrument", str, ok);
    if (ok)
      runData.SetInstrument(str);

    header->Get("RunInfo/Muon Beam Momentum", prop, ok);
    if (ok) {
      if (!prop.GetUnit().CompareTo("MeV/c"))
        runData.SetMuonBeamMomentum(prop.GetValue());
    }

    header->Get("RunInfo/Muon Species", str, ok);
    if (ok)
      runData.SetMuonSpecies(str);

    header->Get("RunInfo/Muon Source", str, ok);
    if (ok)
      runData.SetMuonSource(str);

    header->Get("RunInfo/Muon Spin Angle", prop, ok);
    if (ok) {
      runData.SetMuonSpinAngle(prop.GetValue());
    }

    header->Get("RunInfo/Setup", str, ok);
    if (ok)
      runData.SetSetup(str);

    header->Get("RunInfo/Comment", str, ok);
    if (ok)
      runData.SetComment(str);

    header->Get("RunInfo/Sample Name", str, ok);
    if (ok)
      runData.SetSample(str);

    header->Get("RunInfo/Sample Temperature", prop, ok);
    if (ok)
      runData.SetTemperature(0, prop.GetValue(), prop.GetError());

    header->Get("RunInfo/Sample Magnetic Field", prop, ok);
    if (ok) {
      dval = MRH_UNDEFINED;
      if (!prop.GetUnit().CompareTo("G") || !prop.GetUnit().CompareTo("Gauss"))
        dval = prop.GetValue();
      else if (!prop.GetUnit().CompareTo("T") || !prop.GetUnit().CompareTo("Tesla"))
        dval = prop.GetValue() * 1.0e4;
      runData.SetField(dval);
    }

    header->Get("RunInfo/No of Histos", ival, ok);
    if (ok) {
      noOfHistos = ival;
    }

    header->Get("RunInfo/Time Resolution", prop, ok);
    if (ok) {
      dval = -1.0;
      if (!prop.GetUnit().CompareTo("ps") || !prop.GetUnit().CompareTo("picosec"))
        dval = prop.GetValue()/1.0e3;
      else if (!prop.GetUnit().CompareTo("ns") || !prop.GetUnit().CompareTo("nanosec"))
        dval = prop.GetValue();
      else if (!prop.GetUnit().CompareTo("us") || !prop.GetUnit().CompareTo("microsec"))
        dval = prop.GetValue()*1.0e3;
      else
        std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Found unrecognized Time Resolution unit: " << prop.GetUnit() << std::endl;
      runData.SetTimeResolution(dval);
    }

    header->Get("RunInfo/RedGreen Offsets", ivec, ok);
    if (ok) {
      // check if any2many is used and a group histo list is defined, if NOT, only take the 0-offset data!
      if (fAny2ManyInfo) { // i.e. any2many is called
        if (fAny2ManyInfo->groupHistoList.size() == 0) { // NO group list defined -> use only the 0-offset data
          redGreenOffsets.push_back(0);
        } else { // group list defined
          // make sure that the group list elements is a subset of present RedGreen offsets
          Bool_t found = false;
          for (UInt_t i=0; i<fAny2ManyInfo->groupHistoList.size(); i++) {
            found = false;
            for (UInt_t j=0; j<ivec.size(); j++) {
              if (fAny2ManyInfo->groupHistoList[i] == ivec[j])
                found = true;
            }
            if (!found) {
              std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** requested histo group " << fAny2ManyInfo->groupHistoList[i];
              std::cerr << std::endl << ">>     which is NOT present in the data file." << std::endl;
              return false;
            }
          }
          // found all requested histo groups, hence stuff it to the right places
          redGreenOffsets = fAny2ManyInfo->groupHistoList;
          runData.SetRedGreenOffset(fAny2ManyInfo->groupHistoList);
        }
      } else { // not any2many, i.e. musrfit, musrview, ...
        redGreenOffsets = ivec;
        runData.SetRedGreenOffset(ivec);
      }
    }

    // check further for LEM specific stuff in RunInfo

    header->Get("RunInfo/Moderator HV", prop, ok);
    if (ok)
      runData.SetTransport(prop.GetValue());

    header->Get("RunInfo/Implantation Energy", prop, ok);
    if (ok)
      runData.SetEnergy(prop.GetValue());

    // read the SampleEnvironmentInfo

    header->Get("SampleEnvironmentInfo/Cryo", str, ok);
    if (ok)
      runData.SetCryoName(str);

    // read the MagneticFieldEnvironmentInfo

    header->Get("MagneticFieldEnvironmentInfo/Magnet Name", str, ok);
    if (ok)
      runData.SetMagnetName(str);

    // read the BeamlineInfo

    header->Get("BeamlineInfo/Name", str, ok);
    if (ok)
      runData.SetBeamline(str);

    // read run summary to obtain ring anode HV values
    TObjArray *runSummary = dynamic_cast<TObjArray*>(folder->FindObjectAny("RunSummary"));

    // check if run summary is valid
    if (!runSummary) {
      std::cout << std::endl << "**INFO** Couldn't obtain run summary info from ROOT file " << fRunPathName.Data() << std::endl;
      // this is not fatal... only RA-HV values are not available
    } else { // it follows a (at least) little bit strange extraction of the RA values from Thomas' TObjArray...
      //streaming of a ASCII-file would be more easy
      TString s;
      TObjArrayIter summIter(runSummary);
      TObjString *os(dynamic_cast<TObjString*>(summIter.Next()));
      TObjArray *oa(nullptr);
      TObjString *objTok(nullptr);
      while (os != nullptr) {
        s = os->GetString();
        // will put four parallel if's since it may be that more than one RA-values are on one line
        if (s.Contains("RA-L")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr) {
            if (!objTok->GetString().CompareTo("RA-L")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(0, objTok->GetString().Atof()); // fill RA-R value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        if (s.Contains("RA-R")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr){
            if (!objTok->GetString().CompareTo("RA-R")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(1, objTok->GetString().Atof()); // fill RA-R value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        if (s.Contains("RA-T")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr){
            if (!objTok->GetString().CompareTo("RA-T")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(2, objTok->GetString().Atof()); // fill RA-T value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        if (s.Contains("RA-B")) {
          oa = s.Tokenize(" ");
          TObjArrayIter lineIter(oa);
          objTok = dynamic_cast<TObjString*>(lineIter.Next());
          while (objTok != nullptr){
            if (!objTok->GetString().CompareTo("RA-B")) {
              objTok = dynamic_cast<TObjString*>(lineIter.Next()); // "="
              if ((objTok != nullptr) && !objTok->GetString().CompareTo("=")) {
                objTok = dynamic_cast<TObjString*>(lineIter.Next()); // HV value
                runData.SetRingAnode(3, objTok->GetString().Atof()); // fill RA-B value into the runData structure
                break;
              }
            }
            objTok = dynamic_cast<TObjString*>(lineIter.Next()); // next token...
          }
          // clean up
          if (oa) {
            delete oa;
            oa = nullptr;
          }
        }

        os = dynamic_cast<TObjString*>(summIter.Next()); // next summary line...
      }
    }

    // read data ---------------------------------------------------------

    // check if histos folder is found
    f.GetObject("histos", folder);
    if (!folder) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't obtain histos from " << fRunPathName.Data() << std::endl;
      f.Close();
      return false;
    }

    // get all the data
    for (UInt_t i=0; i<redGreenOffsets.size(); i++) {
      for (Int_t j=0; j<noOfHistos; j++) {
        str.Form("hDecay%03d", redGreenOffsets[i]+j+1);
        TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(str.Data()));
        if (!histo) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadRootFile: **ERROR** Couldn't get histo " << str;
          std::cerr << std::endl;
          f.Close();
          return false;
        }

        dataSet.Clear();
        dataSet.SetName(histo->GetTitle());
        dataSet.SetHistoNo(redGreenOffsets[i]+j+1);

        // get detector info
        path.Form("DetectorInfo/Detector%03d/", redGreenOffsets[i]+j+1);
        pathName = path + "Time Zero Bin";
        header->Get(pathName, dval, ok);
        if (ok)
          dataSet.SetTimeZeroBin(dval);
        pathName = path + "First Good Bin";
        header->Get(pathName, ival, ok);
        if (ok)
          dataSet.SetFirstGoodBin(ival);
        pathName = path + "Last Good Bin";
        header->Get(pathName, ival, ok);
        if (ok)
          dataSet.SetLastGoodBin(ival);
        dataSet.SetTimeZeroBinEstimated(histo->GetMaximumBin());

        // fill data
        for (Int_t j=1; j<=histo->GetNbinsX(); j++) {
          histoData.push_back(histo->GetBinContent(j));
        }
        dataSet.SetData(histoData);
        runData.SetDataSet(dataSet);

        // clear histoData for the next histo
        histoData.clear();
      }
    }

    // clean up
    if (header) {
      delete header;
      header=nullptr;
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
// ReadNexusFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Will read the NeXuS File Format as soon as PSI will have an implementation.
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadNexusFile()
{
#ifdef PNEXUS_ENABLED
  std::cout << std::endl << ">> PRunDataHandler::ReadNexusFile(): Will read nexus file " << fRunPathName.Data() << " ...";

  PDoubleVector  histoData;
  PRawRunData    runData;
  PRawRunDataSet dataSet;
  TString str;
  std::string sstr;
  Double_t dval;
  bool ok;

  PNeXus *nxs_file = new PNeXus(fRunPathName.Data());
  if (!nxs_file->IsValid()) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadNexusFile(): Not a valid NeXus file.";
    std::cerr << std::endl << ">> Error Message: " << nxs_file->GetErrorMsg().data() << std::endl;
    return false;
  }

  if (nxs_file->GetIdfVersion() == 1) {
    if (!nxs_file->IsValid()) {
      std::cout << std::endl << "**ERROR** invalid NeXus IDF 2 version file found." << std::endl;
      return false;
    }

    // get header information

    // get/set laboratory
    str = TString(nxs_file->GetEntryIdf1()->GetLaboratory());
    runData.SetLaboratory(str);

    // get/set beamline
    str = TString(nxs_file->GetEntryIdf1()->GetBeamline());
    runData.SetBeamline(str);

    // get/set instrument
    str = TString(nxs_file->GetEntryIdf1()->GetInstrument()->GetName());
    runData.SetInstrument(str);

    // get/set run title
    str = TString(nxs_file->GetEntryIdf1()->GetTitle());
    runData.SetRunTitle(str);

    // get/set run number
    runData.SetRunNumber(nxs_file->GetEntryIdf1()->GetRunNumber());

    // get/set temperature
    dval = nxs_file->GetEntryIdf1()->GetSample()->GetPhysPropValue("temperature", ok);
    if (ok)
      runData.SetTemperature(0, dval, 0.0);

    // get/set field
    dval = nxs_file->GetEntryIdf1()->GetSample()->GetPhysPropValue("magnetic_field", ok);
    nxs_file->GetEntryIdf1()->GetSample()->GetPhysPropUnit("magnetic_field", sstr, ok);
    str = sstr;
    // since field has to be given in Gauss, check the units
    Double_t factor=1.0;
    if (!str.CompareTo("gauss", TString::kIgnoreCase))
      factor=1.0;
    else if (!str.CompareTo("tesla", TString::kIgnoreCase))
      factor=1.0e4;
    else
      factor=1.0;
    runData.SetField(factor*dval);

    // get/set implantation energy
    runData.SetEnergy(PMUSR_UNDEFINED);

    // get/set moderator HV
    runData.SetTransport(PMUSR_UNDEFINED);

    // get/set RA HV's (LEM specific)
    for (UInt_t i=0; i<4; i++)
      runData.SetRingAnode(i, PMUSR_UNDEFINED);

    // get/set setup
    runData.SetSetup(nxs_file->GetEntryIdf1()->GetNotes());

    // get/set sample
    runData.SetSample(nxs_file->GetEntryIdf1()->GetSample()->GetName());

    // get/set orientation
    runData.SetOrientation("??");

    // get/set time resolution (ns)
    runData.SetTimeResolution(nxs_file->GetEntryIdf1()->GetData()->GetTimeResolution("ns"));

    // get/set start/stop time
    sstr = nxs_file->GetEntryIdf1()->GetStartTime();
    str = sstr;
    TString date, time;
    SplitTimeDate(str, time, date, ok);
    if (ok) {
      runData.SetStartTime(time);
      runData.SetStartDate(date);
    }

    sstr = nxs_file->GetEntryIdf1()->GetStopTime();
    str = sstr;
    SplitTimeDate(str, time, date, ok);
    if (ok) {
      runData.SetStopTime(time);
      runData.SetStopDate(date);
    }

    // get/set t0, firstGoodBin, lastGoodBin
    std::vector<unsigned int> *t0  = nxs_file->GetEntryIdf1()->GetData()->GetT0s();
    std::vector<unsigned int> *fgb = nxs_file->GetEntryIdf1()->GetData()->GetFirstGoodBins();
    std::vector<unsigned int> *lgb = nxs_file->GetEntryIdf1()->GetData()->GetLastGoodBins();

    // get/set data
    std::vector<unsigned int> *pdata;
    unsigned int max=0, binMax=0;
    PDoubleVector data;
    for (UInt_t i=0; i<nxs_file->GetEntryIdf1()->GetData()->GetNoOfHistos(); i++) {
      pdata = nxs_file->GetEntryIdf1()->GetData()->GetHisto(i);
      for (UInt_t j=0; j<pdata->size(); j++) {
        data.push_back(pdata->at(j));
        if (pdata->at(j) > max) {
          max = pdata->at(j);
          binMax = j;
        }
      }

      // fill data set
      dataSet.Clear();
      dataSet.SetHistoNo(i+1); // i.e. histo numbers start with 1
      // set time zero bin
      if (i<t0->size())
        dataSet.SetTimeZeroBin(t0->at(i));
      else
        dataSet.SetTimeZeroBin(t0->at(0));
      // set time zero bin estimate
      dataSet.SetTimeZeroBinEstimated(binMax);
      // set first good bin
      if (i<fgb->size())
        dataSet.SetFirstGoodBin(fgb->at(i));
      else
        dataSet.SetFirstGoodBin(fgb->at(0));
      // set last good bin
      if (i<lgb->size())
        dataSet.SetFirstGoodBin(lgb->at(i));
      else
        dataSet.SetFirstGoodBin(lgb->at(0));
      dataSet.SetData(data);

      runData.SetDataSet(dataSet);
      data.clear();
    }

    // keep run name from the msr-file
    runData.SetRunName(fRunName);

    // keep the information
    fData.push_back(runData);
  } else if (nxs_file->GetIdfVersion() == 2) {
    if (!nxs_file->IsValid()) {
      std::cout << std::endl << "**ERROR** invalid NeXus IDF 2 version file found." << std::endl;
      return false;
    }
    // get header information

    // get/set laboratory
    str = TString(nxs_file->GetEntryIdf2()->GetInstrument()->GetSource()->GetName());
    runData.SetLaboratory(str);

    // get/set beamline
    str = TString(nxs_file->GetEntryIdf2()->GetInstrument()->GetName());
    runData.SetBeamline(str);

    // get/set instrument
    str = TString(nxs_file->GetEntryIdf2()->GetInstrument()->GetName());
    runData.SetInstrument(str);

    // get/set muon source
    str = TString(nxs_file->GetEntryIdf2()->GetInstrument()->GetSource()->GetType());
    runData.SetMuonSource(str);

    // get/set muon species
    str = TString(nxs_file->GetEntryIdf2()->GetInstrument()->GetSource()->GetProbe());
    runData.SetMuonSpecies(str);

    // get/set run title
    str = TString(nxs_file->GetEntryIdf2()->GetTitle());
    runData.SetRunTitle(str);

    // get/set run number
    runData.SetRunNumber(nxs_file->GetEntryIdf2()->GetRunNumber());

    // get/set temperature
    dval = nxs_file->GetEntryIdf2()->GetSample()->GetPhysPropValue("temperature", ok);
    if (ok)
      runData.SetTemperature(0, dval, 0.0);

    // get/set field
    dval = nxs_file->GetEntryIdf2()->GetSample()->GetPhysPropValue("magnetic_field", ok);
    nxs_file->GetEntryIdf2()->GetSample()->GetPhysPropUnit("magnetic_field", sstr, ok);
    str = sstr;
    // since field has to be given in Gauss, check the units
    Double_t factor=1.0;
    if (!str.CompareTo("gauss", TString::kIgnoreCase))
      factor=1.0;
    else if (!str.CompareTo("tesla", TString::kIgnoreCase))
      factor=1.0e4;
    else
      factor=1.0;
    runData.SetField(factor*dval);

    // get/set implantation energy
    runData.SetEnergy(PMUSR_UNDEFINED);

    // get/set moderator HV
    runData.SetTransport(PMUSR_UNDEFINED);

    // get/set RA HV's (LEM specific)
    for (UInt_t i=0; i<4; i++)
      runData.SetRingAnode(i, PMUSR_UNDEFINED);

    // get/set setup take NXsample/temperature_1_env and NXsample/magnetic_field_1_env
    sstr  = nxs_file->GetEntryIdf2()->GetSample()->GetEnvironmentTemp() + std::string("/");
    sstr += nxs_file->GetEntryIdf2()->GetSample()->GetEnvironmentField();
    str = sstr;
    runData.SetSetup(str);

    // get/set sample
    runData.SetSample(nxs_file->GetEntryIdf2()->GetSample()->GetName());

    // get/set orientation
    runData.SetOrientation("??");

    // get/set time resolution (ns)
    runData.SetTimeResolution(nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetTimeResolution("ns"));

    // get/set start/stop time
    sstr = nxs_file->GetEntryIdf2()->GetStartTime();
    str = sstr;
    TString date, time;
    SplitTimeDate(str, time, date, ok);
    if (ok) {
      runData.SetStartTime(time);
      runData.SetStartDate(date);
    }

    sstr = nxs_file->GetEntryIdf2()->GetStopTime();
    str = sstr;
    SplitTimeDate(str, time, date, ok);
    if (ok) {
      runData.SetStopTime(time);
      runData.SetStopDate(date);
    }

    // get/set data, t0, fgb, lgb
    PDoubleVector data;
    PRawRunDataSet dataSet;
    UInt_t histoNo = 0;
    Int_t ival;
    int *histos = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetHistos();
    if (nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfPeriods() > 0) { // counts[][][]
      for (int i=0; i<nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfPeriods(); i++) {
        for (int j=0; j<nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra(); j++) {
          for (int k=0; k<nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfBins(); k++) {
            data.push_back(*(histos+i*nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra()+j*nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfBins()+k));
          }
          dataSet.Clear();
          dataSet.SetHistoNo(++histoNo); // i.e. histo numbers start with 1
          // get t0
          ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetT0(i,j);
          if (ival == -1) // i.e. single value only
            ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetT0();
          dataSet.SetTimeZeroBin(ival);
          // get first good bin
          ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetFirstGoodBin(i,j);
          if (ival == -1) // i.e. single value only
            ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetFirstGoodBin();
          dataSet.SetFirstGoodBin(ival);
          // get last good bin
          ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetLastGoodBin(i,j);
          if (ival == -1) // i.e. single value only
            ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetLastGoodBin();
          dataSet.SetLastGoodBin(ival);

          dataSet.SetData(data);
          runData.SetDataSet(dataSet);
          data.clear();
        }
      }
    } else {
      if (nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra() > 0) { // counts[][]
        for (int i=0; i<nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++) {
          for (int j=0; j<nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfBins(); j++) {
            data.push_back(*(histos+i*nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfBins()+j));
          }
          dataSet.Clear();
          dataSet.SetHistoNo(++histoNo); // i.e. histo numbers start with 1
          // get t0
          ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetT0(-1,i);
          if (ival == -1) // i.e. single value only
            ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetT0();
          dataSet.SetTimeZeroBin(ival);
          // get first good bin
          ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetFirstGoodBin(-1,i);
          if (ival == -1) // i.e. single value only
            ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetFirstGoodBin();
          dataSet.SetFirstGoodBin(ival);
          // get last good bin
          ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetLastGoodBin(-1,i);
          if (ival == -1) // i.e. single value only
            ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetLastGoodBin();
          dataSet.SetLastGoodBin(ival);

          dataSet.SetData(data);
          runData.SetDataSet(dataSet);
          data.clear();
        }
      } else { // counts[]
        for (int i=0; i<nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfBins(); i++) {
          data.push_back(*(histos+i));
        }
        dataSet.Clear();
        dataSet.SetHistoNo(++histoNo); // i.e. histo numbers start with 1
        // get t0
        ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetT0();
        dataSet.SetTimeZeroBin(ival);
        // get first good bin
        ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetFirstGoodBin();
        dataSet.SetFirstGoodBin(ival);
        // get last good bin
        ival = nxs_file->GetEntryIdf2()->GetInstrument()->GetDetector()->GetLastGoodBin();
        dataSet.SetLastGoodBin(ival);

        dataSet.SetData(data);
        runData.SetDataSet(dataSet);
        data.clear();
      }
    }

    // keep run name from the msr-file
    runData.SetRunName(fRunName);

    // keep the information
    fData.push_back(runData);
  } else {
    std::cout << std::endl << ">> PRunDataHandler::ReadNexusFile(): IDF version " << nxs_file->GetIdfVersion() << ", not implemented." << std::endl;
  }

  // clean up
  if (nxs_file) {
    delete nxs_file;
    nxs_file = nullptr;
  }
#else
  std::cout << std::endl << ">> PRunDataHandler::ReadNexusFile(): Sorry, not enabled at configuration level, i.e. --enable-NeXus when executing configure" << std::endl << std::endl;
#endif
  return true;
}

//--------------------------------------------------------------------------
// ReadWkmFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Reads, for backwards compatibility, the ascii-wkm-file data format.
 * The routine is clever enough to distinguish the different wkm-flavours (argh).
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadWkmFile()
{
  PDoubleVector histoData;
  PRawRunData runData;

  // open file
  std::ifstream f;

  // open wkm-file
  f.open(fRunPathName.Data(), std::ifstream::in);
  if (!f.is_open()) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile: **ERROR** Couldn't open run data (" << fRunPathName.Data() << ") file for reading, sorry ...";
    std::cerr << std::endl;
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
        line.Replace(0, idx+1, nullptr, 0); // remove 'Title:'
        StripWhitespace(line);
        runData.SetRunTitle(line);
      } else if (line.Contains("Run:")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Run:'
        StripWhitespace(line);
        ival = ToInt(line, ok);
        if (ok)
          runData.SetRunNumber(ival);
      } else if (line.Contains("Field")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Field:'
        StripWhitespace(line);
        idx = line.Index("G"); // check if unit is given
        if (idx > 0) // unit is indeed given
          line.Resize(idx);
        dval = ToDouble(line, ok);
        if (ok)
          runData.SetField(dval);
      } else if (line.Contains("Setup")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Setup:'
        StripWhitespace(line);
        runData.SetSetup(line);
      } else if (line.Contains("Temp:") || line.Contains("Temp(meas1):")) {
        linecp = line;
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Temp:'
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
        linecp.Replace(0, idx+3, nullptr, 0);
        StripWhitespace(linecp);
        dval = ToDouble(linecp, ok);
        if (ok) {
          runData.SetTempError(0, dval);
        }
      } else if (line.Contains("Temp(meas2):")) {
        linecp = line;
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Temp(meas2):'
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
        linecp.Replace(0, idx+3, nullptr, 0);
        StripWhitespace(linecp);
        dval = ToDouble(linecp, ok);
        if (ok) {
          runData.SetTempError(1, dval);
        }
      } else if (line.Contains("Groups")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Groups:'
        StripWhitespace(line);
        ival = ToInt(line, ok);
        if (ok)
          groups = ival;
      } else if (line.Contains("Channels")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Channels:'
        StripWhitespace(line);
        ival = ToInt(line, ok);
        if (ok)
          channels = ival;
      } else if (line.Contains("Resolution")) {
        idx = line.Index(":");
        line.Replace(0, idx+1, nullptr, 0); // remove 'Resolution:'
        StripWhitespace(line);
        dval = ToDouble(line, ok);
        if (ok)
          runData.SetTimeResolution(dval*1.0e3); // us -> ns
      }
    }

    if (headerInfo)
      f.getline(instr, sizeof(instr));
  } while (headerInfo && !f.eof());

  if ((groups == 0) || (channels == 0) || runData.GetTimeResolution() == 0.0) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR** essential header informations are missing!";
    std::cerr << std::endl << ">> groups          = " << groups;
    std::cerr << std::endl << ">> channels        = " << channels;
    std::cerr << std::endl << ">> time resolution = " << runData.GetTimeResolution();
    std::cerr << std::endl;
    f.close();
    return false;
  }

  // read data ---------------------------------------------------------
  UInt_t group_counter = 0;
  Int_t val;
  TObjArray *tokens;
  TObjString *ostr;
  TString str;
  UInt_t histoNo = 0;
  PRawRunDataSet dataSet;
  do {
    // check if empty line, i.e. new group
    if (IsWhitespace(instr)) {
      dataSet.Clear();
      dataSet.SetHistoNo(++histoNo);
      dataSet.SetData(histoData);

      // get a T0 estimate
      Double_t maxVal = 0.0;
      Int_t maxBin = 0;
      for (UInt_t i=0; i<histoData.size(); i++) {
        if (histoData[i] > maxVal) {
          maxVal = histoData[i];
          maxBin = i;
        }
      }
      dataSet.SetTimeZeroBinEstimated(maxBin);

      runData.SetDataSet(dataSet);

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
        std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: coulnd't tokenize run data.";
        return false;
      }
      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        val = ToInt(str, ok);
        if (ok) {
          histoData.push_back(val);
        } else {
          std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: data line contains non-integer values.";
          // clean up
          delete tokens;
          return false;
        }
      }
      // clean up
      if (tokens) {
        delete tokens;
        tokens = nullptr;
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
      std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: coulnd't tokenize run data.";
      return false;
    }
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString();
      val = ToInt(str, ok);
      if (ok) {
        histoData.push_back(val);
      } else {
        std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR** while reading data: data line contains non-integer values.";
        // clean up
        delete tokens;
        return false;
      }
    }
    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  // save the last histo if not empty
  if (histoData.size() > 0) {
    dataSet.Clear();
    dataSet.SetHistoNo(++histoNo);
    dataSet.SetData(histoData);

    // get a T0 estimate
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t i=0; i<histoData.size(); i++) {
      if (histoData[i] > maxVal) {
        maxVal = histoData[i];
        maxBin = i;
      }
    }
    dataSet.SetTimeZeroBinEstimated(maxBin);

    runData.SetDataSet(dataSet);
    histoData.clear();
  }

  // close file
  f.close();

  // check if all groups are found
  if (static_cast<Int_t>(runData.GetNoOfHistos()) != groups) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR**";
    std::cerr << std::endl << ">> expected " << groups << " histos, but found " << runData.GetNoOfHistos();
    return false;
  }

  // check if all groups have enough channels
  for (UInt_t i=0; i<runData.GetNoOfHistos(); i++) {
    if (static_cast<Int_t>(runData.GetDataBin(i+1)->size()) != channels) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadWkmFile(): **ERROR**";
      std::cerr << std::endl << ">> expected " << channels << " bins in histo " << i+1 << ", but found " << runData.GetDataBin(i)->size();
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
// ReadPsiBinFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Reads the old-fashioned PSI-BIN data-files. The MuSR_td_PSI_bin class
 * of Alex Amato is used. In case of problems, please contact alex.amato@psi.ch.
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadPsiBinFile()
{
  MuSR_td_PSI_bin psiBin;
  Int_t status;
  Bool_t success;

  // read psi bin file
  status = psiBin.Read(fRunPathName.Data());
  switch (status) {
    case 0: // everything perfect
      success = true;
      break;
    case 1: // couldn't open file, or failed while reading the header
      std::cerr << std::endl << ">> **ERROR** couldn't open psi-bin file, or failed while reading the header";
      std::cerr << std::endl;
      success = false;
      break;
    case 2: // unsupported version of the data
      std::cerr << std::endl << ">> **ERROR** psi-bin file: unsupported version of the data";
      std::cerr << std::endl;
      success = false;
      break;
    case 3: // error when allocating data buffer
      std::cerr << std::endl << ">> **ERROR** psi-bin file: error when allocating data buffer";
      std::cerr << std::endl;
      success = false;
      break;
    case 4: // number of histograms/record not equals 1
      std::cerr << std::endl << ">> **ERROR** psi-bin file: number of histograms/record not equals 1";
      std::cerr << std::endl;
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
  PRawRunData runData;
  Double_t dval;

  // set file name
  Ssiz_t pos = fRunPathName.Last('/');
  TString fln(fRunPathName);
  fln.Remove(0, pos+1);
  runData.SetFileName(fln);

  // set laboratory
  runData.SetLaboratory("PSI");

  // filter from the file name the instrument
  TString instrument("n/a"), beamline("n/a");
  TString muonSource("n/a"), muonSpecies("n/a");
  if (fRunPathName.Contains("_gps_", TString::kIgnoreCase)) {
    instrument = "GPS";
    beamline = "piM3.2";
    muonSource = "continuous surface muon source";
    muonSpecies = "positive muons";
  } else if (fRunPathName.Contains("_ltf_", TString::kIgnoreCase)) {
    instrument = "LTF";
    beamline = "piM3.3";
    muonSource = "continuous surface muon source";
    muonSpecies = "positive muons";
  } else if (fRunPathName.Contains("_gpd_", TString::kIgnoreCase)) {
    instrument = "GPD";
    beamline = "muE1";
    muonSource = "continuous decay channel muon source";
    muonSpecies = "positive muons";
  } else if (fRunPathName.Contains("_dolly_", TString::kIgnoreCase)) {
    instrument = "DOLLY";
    beamline = "piE1";
    muonSource = "continuous surface muon source";
    muonSpecies = "positive muons";
  } else if (fRunPathName.Contains("_alc_", TString::kIgnoreCase)) {
    instrument = "ALC";
    beamline = "piE3";
    muonSource = "continuous surface muon source";
    muonSpecies = "positive muons";
  } else if (fRunPathName.Contains("_hifi_", TString::kIgnoreCase)) {
    instrument = "HIFI";
    beamline = "piE3";
    muonSource = "continuous surface muon source";
    muonSpecies = "positive muons";
  }
  runData.SetInstrument(instrument);
  runData.SetBeamline(beamline);
  runData.SetMuonSource(muonSource);
  runData.SetMuonSpecies(muonSpecies);

  // keep run name
  runData.SetRunName(fRunName);
  // get run title
  runData.SetRunTitle(TString(psiBin.GetComment().c_str())); // run title
  // get run number
  runData.SetRunNumber(psiBin.GetRunNumberInt());
  // get setup
  runData.SetSetup(TString(psiBin.GetComment().c_str()));
  // get sample
  runData.SetSample(TString(psiBin.GetSample().c_str()));
  // get orientation
  runData.SetOrientation(TString(psiBin.GetOrient().c_str()));
  // get comment
  runData.SetComment(TString(psiBin.GetComment().c_str()));
  // set LEM specific information to default value since it is not in the file and not used...
  runData.SetEnergy(PMUSR_UNDEFINED);
  runData.SetTransport(PMUSR_UNDEFINED);
  // get field
  Double_t scale = 0.0;
  if (psiBin.GetField().rfind("G") != std::string::npos)
    scale = 1.0;
  if (psiBin.GetField().rfind("T") != std::string::npos)
    scale = 1.0e4;
  status = sscanf(psiBin.GetField().c_str(), "%lf", &dval);
  if (status == 1)
    runData.SetField(scale*dval);
  // get temperature
  PDoubleVector tempVec(psiBin.GetTemperaturesVector());
  PDoubleVector tempDevVec(psiBin.GetDevTemperaturesVector());
  if ((tempVec.size() > 1) && (tempDevVec.size() > 1) && tempVec[0] && tempVec[1]) {
    // take only the first two values for now...
    //maybe that's not enough - e.g. in older GPD data I saw the "correct values in the second and third entry..."
    for (UInt_t i(0); i<2; i++) {
      runData.SetTemperature(i, tempVec[i], tempDevVec[i]);
    }
    tempVec.clear();
    tempDevVec.clear();
  } else {
    status = sscanf(psiBin.GetTemp().c_str(), "%lfK", &dval);
    if (status == 1)
      runData.SetTemperature(0, dval, 0.0);
  }

  // get time resolution (ns)
  runData.SetTimeResolution(psiBin.GetBinWidthNanoSec());

  // get start/stop time
  std::vector<std::string> sDateTime = psiBin.GetTimeStartVector();
  if (sDateTime.size() < 2) {
    std::cerr << std::endl << ">> **WARNING** psi-bin file: couldn't obtain run start date/time" << std::endl;
  }
  std::string date("");
  if (DateToISO8601(sDateTime[0], date)) {
    runData.SetStartDate(date);
  } else {
    std::cerr << std::endl << ">> **WARNING** failed to convert start date: " << sDateTime[0] << " into ISO 8601 date." << std::endl;
    runData.SetStartDate(sDateTime[0]);
  }
  runData.SetStartTime(sDateTime[1]);
  sDateTime.clear();

  sDateTime = psiBin.GetTimeStopVector();
  if (sDateTime.size() < 2) {
    std::cerr << std::endl << ">> **WARNING** psi-bin file: couldn't obtain run stop date/time" << std::endl;
  }
  date = std::string("");
  if (DateToISO8601(sDateTime[0], date)) {
    runData.SetStopDate(date);
  } else {
    std::cerr << std::endl << ">> **WARNING** failed to convert stop date: " << sDateTime[0] << " into ISO 8601 date." << std::endl;
    runData.SetStopDate(sDateTime[0]);
  }
  runData.SetStopTime(sDateTime[1]);
  sDateTime.clear();

  // get t0's
  PIntVector t0 = psiBin.GetT0Vector();

  if (t0.empty()) {
    std::cerr << std::endl << ">> **ERROR** psi-bin file: couldn't obtain any t0's";
    std::cerr << std::endl;
    return false;
  }

  // get first good bin
  PIntVector fgb = psiBin.GetFirstGoodVector();
  if (fgb.empty()) {
    std::cerr << std::endl << ">> **ERROR** psi-bin file: couldn't obtain any fgb's";
    std::cerr << std::endl;
    return false;
  }

  // get last good bin
  PIntVector lgb = psiBin.GetLastGoodVector();
  if (lgb.empty()) {
    std::cerr << std::endl << ">> **ERROR** psi-bin file: couldn't obtain any lgb's";
    std::cerr << std::endl;
    return false;
  }

  // fill raw data
  PRawRunDataSet dataSet;
  PDoubleVector histoData;
  std::vector<Int_t> histo;
  for (Int_t i=0; i<psiBin.GetNumberHistoInt(); i++) {
    histo = psiBin.GetHistoArrayInt(i);
    for (Int_t j=0; j<psiBin.GetHistoLengthBin(); j++) {
      histoData.push_back(histo[j]);
    }

    // estimate T0 from maximum of the data
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t j=0; j<histoData.size(); j++) {
      if (histoData[j] > maxVal) {
        maxVal = histoData[j];
        maxBin = j;
      }
    }

    dataSet.Clear();
    dataSet.SetName(psiBin.GetNameHisto(i).c_str());
    dataSet.SetHistoNo(i+1); // i.e. hist numbering starts at 1
    if (i < static_cast<Int_t>(t0.size()))
      dataSet.SetTimeZeroBin(t0[i]);
    dataSet.SetTimeZeroBinEstimated(maxBin);
    if (i < static_cast<Int_t>(fgb.size()))
      dataSet.SetFirstGoodBin(fgb[i]);
    if (i < static_cast<Int_t>(lgb.size()))
      dataSet.SetLastGoodBin(lgb[i]);
    dataSet.SetData(histoData);

    runData.SetDataSet(dataSet);

    histoData.clear();
  }

  // add run to the run list
  fData.push_back(runData);

  return success;
}

//--------------------------------------------------------------------------
// ReadMudFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Reads the triumf mud-file format.
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadMudFile()
{
  Int_t    fh;
  UINT32   type, val;
  Int_t    success;
  Char_t   str[1024];
  Double_t dval;

  PRawRunData runData;

  fh = MUD_openRead((char*)fRunPathName.Data(), &type);
  if (fh == -1) {
    std::cerr << std::endl << ">> **ERROR** Couldn't open mud-file " << fRunPathName.Data() << ", sorry.";
    std::cerr << std::endl;
    return false;
  }

  // read necessary header information

  // keep run name
  runData.SetRunName(fRunName);

  // get/set the lab
  success = MUD_getLab( fh, str, sizeof(str) );
  if ( !success ) {
    std::cerr << std::endl << ">> **WARNING** Couldn't obtain the laboratory name of run " << fRunName.Data();
    std::cerr << std::endl;
    strcpy(str, "n/a");
  }
  runData.SetLaboratory(TString(str));

  // get/set the beamline
  success = MUD_getArea( fh, str, sizeof(str) );
  if ( !success ) {
    std::cerr << std::endl << ">> **WARNING** Couldn't obtain the beamline of run " << fRunName.Data();
    std::cerr << std::endl;
    strcpy(str, "n/a");
  }
  runData.SetBeamline(TString(str));

  // get/set the instrument
  success = MUD_getApparatus( fh, str, sizeof(str) );
  if ( !success ) {
    std::cerr << std::endl << ">> **WARNING** Couldn't obtain the instrument name of run " << fRunName.Data();
    std::cerr << std::endl;
    strcpy(str, "n/a");
  }
  runData.SetInstrument(TString(str));

  // get run title
  success = MUD_getTitle( fh, str, sizeof(str) );
  if ( !success ) {
    std::cerr << std::endl << ">> **WARNING** Couldn't obtain the run title of run " << fRunName.Data();
    std::cerr << std::endl;
  }
  runData.SetRunTitle(TString(str));

  // get run number
  success = MUD_getRunNumber( fh, &val );
  if (success) {
    runData.SetRunNumber(static_cast<Int_t>(val));
  }

  // get start/stop time of the run
  time_t tval;
  struct tm *dt;
  TString stime("");
  success = MUD_getTimeBegin( fh, (UINT32*)&tval );
  if (success) {
    runData.SetStartDateTime(static_cast<const time_t>(tval));
    dt = localtime((const time_t*)&tval);

    if (dt) {
      // start date
      strftime(str, sizeof(str), "%F", dt);
      stime = str;
      runData.SetStartDate(stime);
      // start time
      memset(str, 0, sizeof(str));
      strftime(str, sizeof(str), "%T", dt);
      stime = str;
      runData.SetStartTime(stime);
    } else {
      std::cerr << "PRunDataHandler::ReadMudFile: **WARNING** run start time readback wrong, will set it to 1900-01-01, 00:00:00" << std::endl;
      stime = "1900-01-01";
      runData.SetStartDate(stime);
      stime = "00:00:00";
      runData.SetStartTime(stime);
    }
  }

  stime = TString("");
  success = MUD_getTimeEnd( fh, (UINT32*)&tval );
  if (success) {
    runData.SetStopDateTime((const time_t)tval);
    dt = localtime((const time_t*)&tval);

    if (dt) {
      // stop date
      strftime(str, sizeof(str), "%F", dt);
      stime = str;
      runData.SetStopDate(stime);
      // stop time
      memset(str, 0, sizeof(str));
      strftime(str, sizeof(str), "%T", dt);
      stime = str;
      runData.SetStopTime(stime);
    } else {
      std::cerr << "PRunDataHandler::ReadMudFile: **WARNING** run stop time readback wrong, will set it to 1900-01-01, 00:00:00" << std::endl;
      stime = "1900-01-01";
      runData.SetStopDate(stime);
      stime = "00:00:00";
      runData.SetStopTime(stime);
    }
  }

  // get setup
  TString setup;
  REAL64 timeResMultiplier = 1.0e9; // Multiplier time resolution
  success = MUD_getLab( fh, str, sizeof(str) );
  if (success) {
    setup = TString(str) + TString("/");
  }
  success = MUD_getArea( fh, str, sizeof(str) );
  if (success) {
    setup += TString(str) + TString("/");
    if (TString(str) == "BNQR" || TString(str) == "BNMR") {
      std::cerr << "PRunDataHandler::ReadMudFile: **INFORMATION** this run was performed on " << str << std::endl;
      std::cerr << "PRunDataHandler::ReadMudFile: **INFORMATION** apply correction to time resolution" << std::endl;
      // identified BNMR/BNQR, correct time resolution.
      timeResMultiplier = 1.0e9;
    }
  }
  success = MUD_getApparatus( fh, str, sizeof(str) );
  if (success) {
    setup += TString(str) + TString("/");
  }
  success = MUD_getSample( fh, str, sizeof(str) );
  if (success) {
    setup += TString(str);
    runData.SetSample(str);
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
    std::cerr << std::endl << ">> **ERROR** Couldn't obtain the number of histograms of run " << fRunName.Data();
    std::cerr << std::endl;
    MUD_closeRead(fh);
    return false;
  }
  Int_t noOfHistos = static_cast<Int_t>(val);

  // get time resolution (ns)
  // check that time resolution is identical for all histograms
  // >> currently it is not forseen to handle histos with different time resolutions <<
  // >> perhaps this needs to be reconsidered later on                               <<
  REAL64 timeResolution = 0.0; // in seconds!!
  REAL64 lrval = 0.0;
  for (Int_t i=1; i<=noOfHistos; i++) {
    success = MUD_getHistSecondsPerBin( fh, i, &lrval );
    if (!success) {
      std::cerr << std::endl << ">> **ERROR** Couldn't obtain the time resolution of run " << fRunName.Data();
      std::cerr << std::endl << ">>           which is fatal, sorry.";
      std::cerr << std::endl;
      MUD_closeRead(fh);
      return false;
    }
    if (i==1) {
      timeResolution = lrval;
    } else {
      if (lrval != timeResolution) {
        std::cerr << std::endl << ">> **ERROR** various time resolutions found in run " << fRunName.Data();
        std::cerr << std::endl << ">>           this is currently not supported, sorry.";
        std::cerr << std::endl;
        MUD_closeRead(fh);
        return false;
      }
    }
  }

  runData.SetTimeResolution(static_cast<Double_t>(timeResolution) * timeResMultiplier); // s -> ns or s -> ms for bNMR
  // Other possibility:
  // Check if it is a bNMR run and fix it or check if "timeres" line
  // was introduced in the msr file


  // read histograms
  UINT32 *pData; // histo memory
  pData = nullptr;
  PDoubleVector histoData;
  PRawRunDataSet dataSet;
  UInt_t noOfBins;

  for (Int_t i=1; i<=noOfHistos; i++) {
    dataSet.Clear();

    dataSet.SetHistoNo(i);

    // get t0's
    success = MUD_getHistT0_Bin( fh, i, &val );
    if ( !success ) {
      std::cerr << std::endl << ">> **WARNING** Couldn't get t0 of histo " << i << " of run " << fRunName.Data();
      std::cerr << std::endl;
    }
    dataSet.SetTimeZeroBin(static_cast<Double_t>(val));

    // get bkg bins
    success = MUD_getHistBkgd1( fh, i, &val );
    if ( !success ) {
      std::cerr << std::endl << ">> **WARNING** Couldn't get bkg bin 1 of histo " << i << " of run " << fRunName.Data();
      std::cerr << std::endl;
      val = 0;
    }
    dataSet.SetFirstBkgBin(static_cast<Int_t>(val));

    success = MUD_getHistBkgd2( fh, i, &val );
    if ( !success ) {
      std::cerr << std::endl << ">> **WARNING** Couldn't get bkg bin 2 of histo " << i << " of run " << fRunName.Data();
      std::cerr << std::endl;
      val = 0;
    }
    dataSet.SetLastBkgBin(static_cast<Int_t>(val));

    // get good data bins
    success = MUD_getHistGoodBin1( fh, i, &val );
    if ( !success ) {
      std::cerr << std::endl << ">> **WARNING** Couldn't get good bin 1 of histo " << i << " of run " << fRunName.Data();
      std::cerr << std::endl;
      val = 0;
    }
    dataSet.SetFirstGoodBin(static_cast<Int_t>(val));

    success = MUD_getHistGoodBin2( fh, i, &val );
    if ( !success ) {
      std::cerr << std::endl << ">> **WARNING** Couldn't get good bin 2 of histo " << i << " of run " << fRunName.Data();
      std::cerr << std::endl;
      val = 0;
    }
    dataSet.SetLastGoodBin(static_cast<Int_t>(val));

    // get number of bins
    success = MUD_getHistNumBins( fh, i, &val );
    if ( !success ) {
      std::cerr << std::endl << ">> **ERROR** Couldn't get the number of bins of histo " << i << ".";
      std::cerr << std::endl << ">>           This is fatal, sorry.";
      std::cerr << std::endl;
      MUD_closeRead( fh );
      return false;
    }
    noOfBins = static_cast<UInt_t>(val);

    pData = (UINT32*)malloc(noOfBins*sizeof(pData));
    if (pData == nullptr) {
      std::cerr << std::endl << ">> **ERROR** Couldn't allocate memory for data.";
      std::cerr << std::endl << ">>           This is fatal, sorry.";
      std::cerr << std::endl;
      MUD_closeRead( fh );
      return false;
    }

    // get histogram
    success = MUD_getHistData( fh, i, pData );
    if ( !success ) {
      std::cerr << std::endl << ">> **ERROR** Couldn't get histo no " << i << ".";
      std::cerr << std::endl << ">>           This is fatal, sorry.";
      std::cerr << std::endl;
      MUD_closeRead( fh );
      return false;
    }

    for (UInt_t j=0; j<noOfBins; j++) {
      histoData.push_back(pData[j]);
    }
    dataSet.SetData(histoData);

    // estimate T0 from maximum of the data
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t j=0; j<histoData.size(); j++) {
      if (histoData[j] > maxVal) {
        maxVal = histoData[j];
        maxBin = j;
      }
    }
    dataSet.SetTimeZeroBinEstimated(maxBin);

    runData.SetDataSet(dataSet);

    histoData.clear();

    free(pData);
  }

  MUD_closeRead(fh);

  // add run to the run list
  fData.push_back(runData);

  return true;
}

//--------------------------------------------------------------------------
// ReadMduAsciiFile (private)
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
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadMduAsciiFile()
{
  Bool_t success = true;
  
  // open file
  std::ifstream f;

  // open data-file
  f.open(fRunPathName.Data(), std::ifstream::in);
  if (!f.is_open()) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** Couldn't open data file (" << fRunPathName.Data() << ") for reading, sorry ...";
    std::cerr << std::endl;
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
  TObjArray *tokens = nullptr;
  TString str;
  Int_t groups = 0;
  Int_t channels = 0;
  Double_t dval = 0.0, unitScaling = 0.0;
  std::vector<PDoubleVector> data;
  
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
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", invalid field entry in header.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
          success = false;
          break;
        }
        // check if field value is a number
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        if (ostr->GetString().IsFloat()) {
          dval = ostr->GetString().Atof();
        } else {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", field value is not float/doulbe.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
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
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", unkown field units.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
          success = false;
          break;
        }
        runData.SetField(dval*unitScaling);  
        
        // clean up tokens
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (workStr.BeginsWith("temp:", TString::kIgnoreCase)) {
        tokens = workStr.Tokenize(":("); // temp: val (units)
        // check if expected number of tokens present
        if (tokens->GetEntries() != 3) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", invalid temperatue entry in header.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
          success = false;
          break;
        }
        // check if field value is a number
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        if (ostr->GetString().IsFloat()) {
          dval = ostr->GetString().Atof();
        } else {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", temperature value is not float/doulbe.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
          success = false;
          break;
        }
        runData.SetTemperature(0, dval, 0.0);  
        
        // clean up tokens
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (workStr.BeginsWith("setup:", TString::kIgnoreCase)) {
        runData.SetSetup(TString(workStr.Data()+workStr.First(":")+2));
      } else if (workStr.BeginsWith("groups:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        groups = workStr.Atoi();      
        if (groups == 0) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", groups is not a number or 0.";
          std::cerr << std::endl;
          success = false;
          break;
        }
        data.resize(groups);
      } else if (workStr.BeginsWith("channels:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        channels = workStr.Atoi();
        if (channels == 0) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", channels is not a number or 0.";
          std::cerr << std::endl;
          success = false;
          break;
        }
      } else if (workStr.BeginsWith("resolution:", TString::kIgnoreCase)) {
        tokens = workStr.Tokenize(":("); // resolution: val (units)
        // check if expected number of tokens present
        if (tokens->GetEntries() != 3) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", invalid time resolution entry in header.";
          std::cerr << std::endl << line.Data();
          std::cerr << std::endl;
          success = false;
          break;
        }
        // check if timeresolution value is a number
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        if (ostr->GetString().IsFloat()) {
          dval = ostr->GetString().Atof();
        } else {
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", time resolution value is not float/doulbe.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
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
          std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", unkown time resolution units.";
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl;
          success = false;
          break;
        }
        runData.SetTimeResolution(dval*unitScaling);  
        
        // clean up tokens
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else { // error
        std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** line no " << lineNo << ", illegal header line.";
        std::cerr << std::endl;
        success = false;
        break;
      }
    } else if (dataTag) {
      dataLineCounter++;
      tokens = line.Tokenize(" ,\t");
      // check if the number of data line entries is correct
      if (tokens->GetEntries() != groups+1) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **ERROR** found data line with a wrong data format, cannot be handled (line no " << lineNo << ")";
        std::cerr << std::endl << ">> line:";
        std::cerr << std::endl << ">> " << line.Data();
        std::cerr << std::endl;
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
        tokens = nullptr;
      }
    }
  }
  
  f.close();

  // keep data
  PRawRunDataSet dataSet;
  for (UInt_t i=0; i<data.size(); i++) {
    dataSet.Clear();
    dataSet.SetHistoNo(i);
    dataSet.SetData(data[i]);
  
    // estimate T0 from maximum of the data
    Double_t maxVal = 0.0;
    Int_t maxBin = 0;
    for (UInt_t j=0; j<data[i].size(); j++) {
      if (data[i][j] > maxVal) {
        maxVal = data[i][j];
        maxBin = j;
      }
    }
    dataSet.SetTimeZeroBinEstimated(maxBin);
    runData.SetDataSet(dataSet);
  }

  // clean up
  for (UInt_t i=0; i<data.size(); i++) 
    data[i].clear();
  data.clear();  
  
  if (dataLineCounter != channels) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadMduAsciiFile **WARNING** found " << dataLineCounter << " data bins,";
    std::cerr << std::endl << ">> expected " << channels << " according to the header." << std::endl;
  }

  fData.push_back(runData);
  
  return success;
}
 
//--------------------------------------------------------------------------
// ReadAsciiFile (private)
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
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadAsciiFile()
{
  Bool_t success = true;

  // open file
  std::ifstream f;

  // open data-file
  f.open(fRunPathName.Data(), std::ifstream::in);
  if (!f.is_open()) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** Couldn't open data file (" << fRunPathName.Data() << ") for reading, sorry ...";
    std::cerr << std::endl;
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
          std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", field is not a number.";
          std::cerr << std::endl;
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
          std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", temperature is not a number.";
          std::cerr << std::endl;
          success = false;
          break;
        }
        runData.SetTemperature(0, workStr.Atof(), 0.0);
      } else if (workStr.BeginsWith("energy:", TString::kIgnoreCase)) {
        workStr = TString(workStr.Data()+workStr.First(":")+2);
        if (!workStr.IsFloat()) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", energy is not a number.";
          std::cerr << std::endl;
          success = false;
          break;
        }
        runData.SetEnergy(workStr.Atof());
        runData.SetTransport(PMUSR_UNDEFINED); // just to initialize the variables to some "meaningful" value
      } else { // error
        std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ", illegal header line.";
        std::cerr << std::endl;
        success = false;
        break;
      }
    } else if (dataTag) {
      if (line.IsWhitespace()) // ignore empty lines
        continue;
      TObjString *ostr;
      TObjArray *tokens;

      // Remove trailing end of line
      line.Remove(TString::kTrailing, '\r'); 

      // check if data have x, y [, error y] structure, and that x, y, and error y are numbers
      tokens = line.Tokenize(" ,\t");
      // check if the number of data line entries is 2 or 3
      if ((tokens->GetEntries() != 2) && (tokens->GetEntries() != 3)) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** found data line with a structure different than \"x, y [, error y]\", cannot be handled (line no " << lineNo << ")";
        std::cerr << std::endl;
        success = false;
        break;
      }

      // get x
      ostr = dynamic_cast<TObjString*>(tokens->At(0));
      if (!ostr->GetString().IsFloat()) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ": x = " << ostr->GetString().Data() << " is not a number, sorry.";
        std::cerr << std::endl;
        success = false;
        break;
      }
      x = ostr->GetString().Atof();

      // get y
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      if (!ostr->GetString().IsFloat()) {
        std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ": y = " << ostr->GetString().Data() << " is not a number, sorry.";
        std::cerr << std::endl;
        success = false;
        break;
      }
      y = ostr->GetString().Atof();

      // get error y if present
      if (tokens->GetEntries() == 3) {
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        if (!ostr->GetString().IsFloat()) {
          std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << ": error y = " << ostr->GetString().Data() << " is not a number, sorry.";
          std::cerr << std::endl;
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
        tokens = nullptr;
      }

      // keep values
      xVec.push_back(x);
      exVec.push_back(1.0);
      yVec.push_back(y);
      eyVec.push_back(ey);

    } else {
      std::cerr << std::endl << ">> PRunDataHandler::ReadAsciiFile **ERROR** line no " << lineNo << " neither header nor data line. No idea how to handle it!";
      std::cerr << std::endl;
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
// ReadDBFile (private)
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
 * <p>Some db-files do have a '\\-e' or '\\e' label just between the DATA tag line and the real data.
 * This tag will just be ignored.
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::ReadDBFile()
{
  Bool_t success = true;

  // open file
  std::ifstream f;

  // open db-file
  f.open(fRunPathName.Data(), std::ifstream::in);
  if (!f.is_open()) {
    std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** Couldn't open data file (" << fRunPathName.Data() << ") for reading, sorry ...";
    std::cerr << std::endl;
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
  Bool_t dataTagsRead = false; // flag showing if the data tags are alread read

  // variables needed to tokenize strings
  TString tstr;
  TObjString *ostr;
  TObjArray *tokens = nullptr;

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
    } else if (workStr.BeginsWith("data", TString::kIgnoreCase) && !dataTagsRead) {
      dataTagsRead = true;
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
        tokens = nullptr;
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
            tokens = nullptr;
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
              std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              std::cerr << std::endl << ">> " << workStr.Data();
              std::cerr << std::endl << ">> found potential run data line without run data tag.";
              return false;
            }
            // split string in tokens
            tokens = workStr.Tokenize(","); // line has structure: runNo,,, runTitle
            ostr = dynamic_cast<TObjString*>(tokens->At(0));
            tstr = ostr->GetString();
            if (!tstr.IsFloat()) {
              std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              std::cerr << std::endl << ">> " << workStr.Data();
              std::cerr << std::endl << ">> Expected db-data line with structure: runNo,,, runTitle";
              std::cerr << std::endl << ">> runNo = " << tstr.Data() << ", seems to be not a number.";
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
              std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              std::cerr << std::endl << ">> " << workStr.Data();
              std::cerr << std::endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
              delete tokens;
              return false;
            }
            ostr = dynamic_cast<TObjString*>(tokens->At(0));
            tstr = ostr->GetString();
            idx = GetDataTagIndex(tstr, runData.fDataNonMusr.GetDataTags());
            if (idx == -1) {
              std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              std::cerr << std::endl << ">> " << workStr.Data();
              std::cerr << std::endl << ">> data tag error: " << tstr.Data() << " seems not present in the data tag list";
              delete tokens;
              return false;
            }

            switch (tokens->GetEntries()) {
              case 3: // tag = val,,,
                ostr = dynamic_cast<TObjString*>(tokens->At(1));
                tstr = ostr->GetString();
                if (!tstr.IsFloat()) {
                  std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                  std::cerr << std::endl << ">> " << workStr.Data();
                  std::cerr << std::endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                  std::cerr << std::endl << ">> val = " << tstr.Data() << ", seems to be not a number.";
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
                  std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                  std::cerr << std::endl << ">> " << workStr.Data();
                  std::cerr << std::endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                  std::cerr << std::endl << ">> val = " << tstr.Data() << ", seems to be not a number.";
                  delete tokens;
                  return false;
                }
                val = tstr.Atof();
                runData.fDataNonMusr.AppendSubData(idx, val);
                // handle err1 (err2 will be ignored for the time being)
                ostr = dynamic_cast<TObjString*>(tokens->At(2));
                tstr = ostr->GetString();
                if (!tstr.IsFloat()) {
                  std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                  std::cerr << std::endl << ">> " << workStr.Data();
                  std::cerr << std::endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                  std::cerr << std::endl << ">> err1 = " << tstr.Data() << ", seems to be not a number.";
                  delete tokens;
                  return false;
                }
                val = tstr.Atof();
                runData.fDataNonMusr.AppendSubErrData(idx, val);
                break;
              default:
                std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
                std::cerr << std::endl << ">> " << workStr.Data();
                std::cerr << std::endl << ">> Expected db-data line with structure: tag = val,err1,err2,\\";
                delete tokens;
                return false;
            }
          }

        } else { // handle row formated data
          // split string in tokens
          tokens = workStr.Tokenize(","); // line has structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle
          if (tokens->GetEntries() != static_cast<Int_t>(3*runData.fDataNonMusr.GetDataTags()->size()+1)) {
            std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
            std::cerr << std::endl << ">> " << workStr.Data();
            std::cerr << std::endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
            std::cerr << std::endl << ">> found = " << tokens->GetEntries() << " tokens, however expected " << 3*runData.fDataNonMusr.GetDataTags()->size()+1;
            std::cerr << std::endl << ">> Perhaps there are commas without space inbetween, like 12.3,, 3.2,...";
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
              std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              std::cerr << std::endl << ">> " << workStr.Data();
              std::cerr << std::endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
              std::cerr << std::endl << ">> value=" << tstr.Data() << " seems not to be a number";
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
              std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo << ":";
              std::cerr << std::endl << ">> " << workStr.Data();
              std::cerr << std::endl << ">> Expected db-data line with structure: val1, err11, err12, ..., valn, errn1, errn2, runNo, , , , runTitle";
              std::cerr << std::endl << ">> error1=" << tstr.Data() << " seems not to be a number";
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
    std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR**";
    std::cerr << std::endl << ">> number of LABELS found    = " << runData.fDataNonMusr.GetLabels()->size();
    std::cerr << std::endl << ">> number of Data tags found = " << runData.fDataNonMusr.GetDataTags()->size();
    std::cerr << std::endl << ">> They have to be equal!!";
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
    return false;
  }

  // check if all vectors have the same size
  for (UInt_t i=1; i<runData.fDataNonMusr.GetData()->size(); i++) {
    if (runData.fDataNonMusr.GetData()->at(i).size() != runData.fDataNonMusr.GetData()->at(i-1).size()) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo;
      std::cerr << std::endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i-1).Data() << ", number data elements = " << runData.fDataNonMusr.GetData()->at(i-1).size();
      std::cerr << std::endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i).Data() << ", number data elements = " << runData.fDataNonMusr.GetData()->at(i).size();
      std::cerr << std::endl << ">> They have to be equal!!";
      success = false;
      break;
    }
    if (runData.fDataNonMusr.GetErrData()->at(i).size() != runData.fDataNonMusr.GetErrData()->at(i-1).size()) {
      std::cerr << std::endl << ">> PRunDataHandler::ReadDBFile **ERROR** in line no " << lineNo;
      std::cerr << std::endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i-1).Data() << ", number data elements = " << runData.fDataNonMusr.GetData()->at(i-1).size();
      std::cerr << std::endl << ">> label: " << runData.fDataNonMusr.GetDataTags()->at(i).Data() << ", number error data elements = " << runData.fDataNonMusr.GetErrData()->at(i).size();
      std::cerr << std::endl << ">> They have to be equal!!";
      success = false;
      break;
    }
  }

  // clean up tokens
  if (tokens) {
    delete tokens;
    tokens = nullptr;
  }

  // keep run name
  runData.SetRunName(fRunName);

  fData.push_back(runData);

  return success;
}

//--------------------------------------------------------------------------
// WriteMusrRootFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the MusrRoot file format. Only the required entries will be handled.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WriteMusrRootFile(TString fln)
{
  Bool_t ok = false;
  fln = GenerateOutputFileName(fln, ".root", ok);
  if (!ok)
    return false;

  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WriteMusrRootFile(): writing a root data file (" << fln.Data() << ") ... " << std::endl;

  // generate data file
  TFolder *histosFolder;
  TFolder *decayAnaModule;
  TFolder *runHeader;

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");

  runHeader = gROOT->GetRootFolder()->AddFolder("RunHeader", "MusrRoot Run Header Info");
  gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeader");
  TMusrRunHeader *header = new TMusrRunHeader(true);
  gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeader");

  // feed header info
  TString str, pathName;
  Int_t ival;
  Double_t dval[2];
  TMusrRunPhysicalQuantity prop;

  // feed RunInfo
  str = fData[0].GetGenericValidatorUrl()->Copy();
  header->Set("RunInfo/Generic Validator URL", str);
  str = fData[0].GetSpecificValidatorUrl()->Copy();
  header->Set("RunInfo/Specific Validator URL", str);
  str = fData[0].GetGenerator()->Copy();
  header->Set("RunInfo/Generator", str);
  str = fData[0].GetFileName()->Copy();
  header->Set("RunInfo/File Name", str);
  str = fData[0].GetRunTitle()->Copy();
  header->Set("RunInfo/Run Title", str);
  header->Set("RunInfo/Run Number", fData[0].GetRunNumber());
  str = fData[0].GetStartDate()->Copy() + " " + fData[0].GetStartTime()->Copy();
  header->Set("RunInfo/Run Start Time", str);
  str = fData[0].GetStopDate()->Copy() + " " + fData[0].GetStopTime()->Copy();
  header->Set("RunInfo/Run Stop Time", str);
  ival = fData[0].GetStopDateTime() - fData[0].GetStartDateTime();
  prop.Set("Run Duration", ival, "sec");
  header->Set("RunInfo/Run Duration", prop);
  str = fData[0].GetLaboratory()->Copy();
  header->Set("RunInfo/Laboratory", str);
  str = fData[0].GetInstrument()->Copy();
  header->Set("RunInfo/Instrument", str);
  dval[0] = fData[0].GetMuonBeamMomentum();
  prop.Set("Muon Beam Momentum", dval[0], "MeV/c");
  header->Set("RunInfo/Muon Beam Momentum", prop);
  str = fData[0].GetMuonSpecies()->Copy();
  header->Set("RunInfo/Muon Species", str);
  str = fData[0].GetMuonSource()->Copy();
  header->Set("RunInfo/Muon Source", str);
  str = fData[0].GetSetup()->Copy();
  header->Set("RunInfo/Setup", str);
  str = fData[0].GetComment()->Copy();
  header->Set("RunInfo/Comment", str);
  str = fData[0].GetSample()->Copy();
  header->Set("RunInfo/Sample Name", str);
  dval[0] = fData[0].GetTemperature(0);
  dval[1] = fData[0].GetTempError(0);
  prop.Set("Sample Temperature", MRH_UNDEFINED, dval[0], dval[1], "K");
  header->Set("RunInfo/Sample Temperature", prop);
  dval[0] = fData[0].GetField();
  prop.Set("Sample Magnetic Field", dval[0], "G");
  header->Set("RunInfo/Sample Magnetic Field", prop);
  header->Set("RunInfo/No of Histos", static_cast<Int_t>(fData[0].GetNoOfHistos()));
  dval[0] = fData[0].GetTimeResolution();
  prop.Set("Time Resolution", dval[0], "ns");
  header->Set("RunInfo/Time Resolution", prop);
  header->Set("RunInfo/RedGreen Offsets", fData[0].GetRedGreenOffset());

  // feed DetectorInfo
  Int_t histoNo = 0;
  PRawRunDataSet *dataSet;
  UInt_t size = fData[0].GetNoOfHistos();
  for (UInt_t i=0; i<size; i++) {
    dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
    if (dataSet == nullptr) { // something is really wrong
      std::cerr << std::endl << ">> PRunDataHandler::WriteMusrRootFile: **ERROR** Couldn't get data set (idx=" << i << ")";
      std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
      return false;
    }
    histoNo = dataSet->GetHistoNo();
    pathName.Form("DetectorInfo/Detector%03d/Name", histoNo);
    str = dataSet->GetName();
    if (!str.CompareTo("n/a"))
      str.Form("Detector%3d", histoNo);
    header->Set(pathName, str);
    pathName.Form("DetectorInfo/Detector%03d/Histo Number", histoNo);
    header->Set(pathName, histoNo);
    pathName.Form("DetectorInfo/Detector%03d/Histo Length", histoNo);
    header->Set(pathName, static_cast<Int_t>(dataSet->GetData()->size()/fAny2ManyInfo->rebin));
    pathName.Form("DetectorInfo/Detector%03d/Time Zero Bin", histoNo);
    header->Set(pathName, dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin);
    pathName.Form("DetectorInfo/Detector%03d/First Good Bin", histoNo);
    ival = dataSet->GetFirstGoodBin();
    header->Set(pathName, static_cast<Int_t>(ival/fAny2ManyInfo->rebin));
    pathName.Form("DetectorInfo/Detector%03d/Last Good Bin", histoNo);
    ival = dataSet->GetLastGoodBin();
    header->Set(pathName, static_cast<Int_t>(ival/fAny2ManyInfo->rebin));
  }

  // feed SampleEnvironmentInfo
  str = fData[0].GetCryoName()->Copy();
  header->Set("SampleEnvironmentInfo/Cryo", str);

  // feed MagneticFieldEnvironmentInfo
  str = fData[0].GetMagnetName()->Copy();
  header->Set("MagneticFieldEnvironmentInfo/Magnet Name", str);

  // feed BeamlineInfo
  str = fData[0].GetBeamline()->Copy();
  header->Set("BeamlineInfo/Name", str);

  // feed histos
  std::vector<TH1F*> histos;
  TH1F *histo = nullptr;
  UInt_t length = 0;
  if (fAny2ManyInfo->rebin == 1) {
    for (UInt_t i=0; i<size; i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WriteMusrRootFile: **ERROR** Couldn't get data set (idx=" << i << ")";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      str.Form("hDecay%03d", dataSet->GetHistoNo());
      length = dataSet->GetData()->size();
      histo = new TH1F(str.Data(), str.Data(), length+1, -0.5, static_cast<Double_t>(length)+0.5);
      for (UInt_t j=0; j<length; j++) {
        histo->SetBinContent(j+1, dataSet->GetData()->at(j));
      }
      histos.push_back(histo);
    }
  } else { // rebin > 1
    UInt_t dataRebin = 0;
    UInt_t dataCount = 0;
    for (UInt_t i=0; i<size; i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WriteMusrRootFile: **ERROR** Couldn't get data set (idx=" << i << ")";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      str.Form("hDecay%03d", dataSet->GetHistoNo());
      length = dataSet->GetData()->size();
      histo = new TH1F(str.Data(), str.Data(), static_cast<Int_t>(length/fAny2ManyInfo->rebin)+1, -0.5, static_cast<Double_t>(static_cast<Int_t>(length/fAny2ManyInfo->rebin))+0.5);
      dataCount = 0;
      for (UInt_t j=0; j<length; j++) {
        if ((j > 0) && (j % fAny2ManyInfo->rebin == 0)) {
          dataCount++;
          histo->SetBinContent(dataCount, dataRebin);
          dataRebin = 0;
        }
        dataRebin += static_cast<UInt_t>(dataSet->GetData()->at(j));
      }
      histos.push_back(histo);
    }
  }

  // add histos to the DecayAnaModule folder
  for (UInt_t i=0; i<histos.size(); i++)
    decayAnaModule->Add(histos[i]);

  // write file
  TFile *fout = new TFile(fln, "RECREATE", fln);
  if (fout == nullptr) {
    std::cerr << std::endl << "PRunDataHandler::WriteMusrRootFile(): **ERROR** Couldn't create ROOT file '" << fln << "'" << std::endl;
    return false;
  }

  fout->cd();
  if (header->FillFolder(runHeader))
    runHeader->Write();
  histosFolder->Write();
  fout->Close();

  // check if root file shall be streamed to stdout
  if (fAny2ManyInfo->useStandardOutput && (fAny2ManyInfo->compressionTag == 0)) {
    // stream file to stdout
    std::ifstream is;
    int length=1024;
    char *buffer;

    is.open(fln.Data(), std::ios::binary);
    if (!is.is_open()) {
      std::cerr << std::endl << "PRunDataHandler::WriteMusrRootFile(): **ERROR** Couldn't open the root-file for streaming." << std::endl;
      remove(fln.Data());
      return false;
    }

    // get length of file
    is.seekg(0, std::ios::end);
    length = is.tellg();
    is.seekg(0, std::ios::beg);

    if (length == -1) {
      std::cerr << std::endl << "PRunDataHandler::WriteMusrRootFile(): **ERROR** Couldn't determine the root-file size." << std::endl;
      remove(fln.Data());
      return false;
    }

    // allocate memory
    buffer = new char [length];

    // read data as a block
    while (!is.eof()) {
      is.read(buffer, length);
      std::cout.write(buffer, length);
    }

    is.close();

    delete [] buffer;

    // delete temporary root file
    remove(fln.Data());
  }

  return true;
}

//--------------------------------------------------------------------------
// WriteRootFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the PSI LEM root-file format.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WriteRootFile(TString fln)
{
  Bool_t ok = false;
  fln = GenerateOutputFileName(fln, ".root", ok);
  if (!ok)
    return false;


  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WriteRootFile(): writing a root data file (" << fln.Data() << ") ... " << std::endl;

  // generate data file
  TFolder *histosFolder;
  TFolder *decayAnaModule;
  TFolder *runInfo;

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");

  runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "LEM RunInfo");
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");
  TLemRunHeader *header = new TLemRunHeader();
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");

  // feed header info
  header->SetRunTitle(fData[0].GetRunTitle()->Data());
  header->SetLemSetup(fData[0].GetSetup()->Data());
  header->SetRunNumber(fData[0].GetRunNumber());
  TString dt = *fData[0].GetStartDate() + "/" + *fData[0].GetStartTime();
  header->SetStartTimeString(dt.Data());
  dt = *fData[0].GetStopDate() + "/" + *fData[0].GetStopTime();
  header->SetStopTimeString(dt.Data());
  header->SetStartTime(fData[0].GetStartDateTime());
  header->SetStopTime(fData[0].GetStopDateTime());
  header->SetModeratorHV(-999.9, 0.0);
  header->SetSampleHV(-999.9, 0.0);
  header->SetImpEnergy(-999.9);
  header->SetSampleTemperature(fData[0].GetTemperature(0), fData[0].GetTempError(0));
  header->SetSampleBField(fData[0].GetField(), 0.0);
  header->SetTimeResolution(fData[0].GetTimeResolution());
  PRawRunDataSet *dataSet = fData[0].GetDataSet(0, false); // i.e. the false means, that i is the index and NOT the histo number
  header->SetNChannels(static_cast<UInt_t>(dataSet->GetData()->size()/fAny2ManyInfo->rebin));
  header->SetNHist(fData[0].GetNoOfHistos());
  header->SetCuts("none");
  header->SetModerator("none");

  // feed t0's if possible
  UInt_t NoT0s = fData[0].GetNoOfHistos();
  if (fData[0].GetNoOfHistos() > NHIST) {
    std::cerr << std::endl << ">> PRunDataHandler::WriteRootFile: **WARNING** found more T0's (" << NoT0s << ") than can be handled (" << NHIST << ").";
    std::cerr << std::endl << ">> Will only write the first " << NHIST << " T0s!!" << std::endl;
    NoT0s = NHIST;
  }
  Double_t *tt0 = new Double_t[NoT0s];

  for (UInt_t i=0;  i<NoT0s; i++) {
    dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
    tt0[i] = dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin;
  }
  header->SetTimeZero(tt0);
  runInfo->Add(header); // add header to RunInfo folder

  // feed histos
  std::vector<TH1F*> histos;
  TH1F *histo = nullptr;
  Char_t str[32];
  UInt_t size = 0;
  if (fAny2ManyInfo->rebin == 1) {
     for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
       dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
       if (dataSet == nullptr) { // something is really wrong
         std::cerr << std::endl << ">> PRunDataHandler::WriteRootFile: **ERROR** Couldn't get data set (idx=0" << i << ")";
         std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
         return false;
       }
       size = dataSet->GetData()->size();
       sprintf(str, "hDecay%02d", static_cast<Int_t>(i));
       histo = new TH1F(str, str, size+1, -0.5, static_cast<Double_t>(size)+0.5);
       for (UInt_t j=0; j<size; j++) {
         histo->SetBinContent(j+1, dataSet->GetData()->at(j));
       }
       histos.push_back(histo);
     }
  } else { // rebin > 1
    UInt_t dataRebin = 0;
    UInt_t dataCount = 0;
    for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WriteNexusFile: **ERROR** Couldn't get data set (idx=0" << i << ")";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      size = dataSet->GetData()->size();
      sprintf(str, "hDecay%02d", static_cast<Int_t>(i));
      histo = new TH1F(str, str, static_cast<UInt_t>(size/fAny2ManyInfo->rebin)+1, -0.5, static_cast<Double_t>(size)/static_cast<Double_t>(fAny2ManyInfo->rebin)+0.5);
      dataCount = 0;
      for (UInt_t j=0; j<size; j++) {
        if ((j > 0) && (j % fAny2ManyInfo->rebin == 0)) {
          dataCount++;
          histo->SetBinContent(dataCount, dataRebin);
          dataRebin = 0;
        }
        dataRebin += static_cast<UInt_t>(dataSet->GetData()->at(j));
      }
      histos.push_back(histo);
    }
  }

  // add histos to the DecayAnaModule folder
  for (UInt_t i=0; i<histos.size(); i++)
    decayAnaModule->Add(histos[i]);

  // write file
  TFile *fout = new TFile(fln, "RECREATE", fln);
  if (fout == nullptr) {
    std::cerr << std::endl << "PRunDataHandler::WriteRootFile(): **ERROR** Couldn't create ROOT file '" << fln << "'" << std::endl;
    return false;
  }

  fout->cd();
  runInfo->Write();
  histosFolder->Write();
  fout->Close();

  // clean up
  for (UInt_t i=0; i<histos.size(); i++) {
    delete histos[i];
  }
  histos.clear();
  delete fout;
  delete [] tt0;
  delete header;

  // check if root file shall be streamed to stdout
  if (fAny2ManyInfo->useStandardOutput && (fAny2ManyInfo->compressionTag == 0)) {
    // stream file to stdout
    std::ifstream is;
    int length=1024;
    char *buffer;

    is.open(fln.Data(), std::ios::binary);
    if (!is.is_open()) {
      std::cerr << std::endl << "PRunDataHandler::WriteRootFile(): **ERROR** Couldn't open the root-file for streaming." << std::endl;
      remove(fln.Data());
      return false;
    }

    // get length of file
    is.seekg(0, std::ios::end);
    length = is.tellg();
    is.seekg(0, std::ios::beg);

    if (length == -1) {
      std::cerr << std::endl << "PRunDataHandler::WriteRootFile(): **ERROR** Couldn't determine the root-file size." << std::endl;
      remove(fln.Data());
      return false;
    }

    // allocate memory
    buffer = new char [length];

    // read data as a block
    while (!is.eof()) {
      is.read(buffer, length);
      std::cout.write(buffer, length);
    }

    is.close();

    delete [] buffer;

    // delete temporary root file
    remove(fln.Data());
  }

  return true;
}

//--------------------------------------------------------------------------
// WriteNexusFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the nexus-file format.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WriteNexusFile(TString fln)
{
#ifdef PNEXUS_ENABLED
  Bool_t ok = false;
  fln = GenerateOutputFileName(fln, ".nxs", ok);
  if (!ok)
    return false;

  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WriteNexusFile(): writing a NeXus data file (" << fln.Data() << ") ... " << std::endl;

  // create NeXus object
  PNeXus *nxs = new PNeXus();
  if (nxs == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::WriteNexusFile(): **ERROR** couldn't invoke the NeXus object." << std::endl;
    return false;
  }

  // set IDF version
  nxs->SetIdfVersion(fAny2ManyInfo->idf);

  if (fAny2ManyInfo->idf == 1) {
    // fill necessary data structures
    nxs->SetFileName(fln.Data());

    // set file creating time
    time_t now;
    struct tm *tm;
    time(&now);
    tm = localtime(&now);
    std::string str("");
    char cstr[128];
    strftime(cstr, sizeof(cstr), "%FT%T", tm);
    str = std::string(cstr);
    nxs->SetFileTime(str);

    nxs->GetEntryIdf1()->SetProgramName("any2many");
    nxs->GetEntryIdf1()->SetProgramVersion("$Id$");
    nxs->GetEntryIdf1()->SetRunNumber(fData[0].GetRunNumber());
    nxs->GetEntryIdf1()->SetTitle(fData[0].GetRunTitle()->Data());
    nxs->GetEntryIdf1()->SetNotes("n/a");
    nxs->GetEntryIdf1()->SetAnalysis("muonTD");
    if (*fData[0].GetLaboratory() != "n/a")
      nxs->GetEntryIdf1()->SetLaboratory(fData[0].GetLaboratory()->Data());
    if (*fData[0].GetBeamline() != "n/a")
      nxs->GetEntryIdf1()->SetBeamline(fData[0].GetBeamline()->Data());
    str = std::string(fData[0].GetStartDate()->Data()) + std::string("T") + std::string(fData[0].GetStartTime()->Data());
    nxs->GetEntryIdf1()->SetStartTime(str);
    str = std::string(fData[0].GetStopDate()->Data()) + std::string("T") + std::string(fData[0].GetStopTime()->Data());
    nxs->GetEntryIdf1()->SetStopTime(str);
    nxs->GetEntryIdf1()->SetSwitchingState(1);
    nxs->GetEntryIdf1()->GetUser()->SetName("n/a");
    nxs->GetEntryIdf1()->GetUser()->SetExperimentNumber("n/a");
    nxs->GetEntryIdf1()->GetSample()->SetName(fData[0].GetSample()->Data());
    nxs->GetEntryIdf1()->GetSample()->SetPhysProp("temperature", fData[0].GetTemperature(0), "Kelvin");
    nxs->GetEntryIdf1()->GetSample()->SetPhysProp("magnetic_field", fData[0].GetField(), "Gauss");
    nxs->GetEntryIdf1()->GetSample()->SetEnvironment(fData[0].GetSetup()->Data());
    nxs->GetEntryIdf1()->GetSample()->SetShape("n/a");
    nxs->GetEntryIdf1()->GetSample()->SetMagneticFieldVectorAvailable(0);
    if (*fData[0].GetInstrument() != "n/a")
      nxs->GetEntryIdf1()->GetInstrument()->SetName(fData[0].GetInstrument()->Data());
    nxs->GetEntryIdf1()->GetInstrument()->GetDetector()->SetNumber(fData[0].GetNoOfHistos());
    nxs->GetEntryIdf1()->GetInstrument()->GetCollimator()->SetType("n/a");
    // calculate the total number of counts
    double total_counts = 0;
    PRawRunDataSet *dataSet = nullptr;
    for (unsigned int i=0; i<fData[0].GetNoOfHistos(); i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WriteNexusFile: **ERROR** Couldn't get data set (idx=0" << i << ")";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      for (unsigned int j=0; j<dataSet->GetData()->size(); j++)
        total_counts += dataSet->GetData()->at(j);
    }
    double total_counts_mev = (double) total_counts / 1.0e6;
    nxs->GetEntryIdf1()->GetInstrument()->GetBeam()->SetTotalCounts(total_counts_mev);
    nxs->GetEntryIdf1()->GetInstrument()->GetBeam()->SetUnits("Mev");

    nxs->GetEntryIdf1()->GetData()->SetTimeResolution(fData[0].GetTimeResolution()*fAny2ManyInfo->rebin, "ns");

    for (unsigned int i=0; i<fData[0].GetNoOfHistos(); i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      nxs->GetEntryIdf1()->GetData()->SetT0(static_cast<Int_t>(dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin), i);
      nxs->GetEntryIdf1()->GetData()->SetFirstGoodBin(static_cast<Int_t>(dataSet->GetFirstGoodBin()/fAny2ManyInfo->rebin), i);
      nxs->GetEntryIdf1()->GetData()->SetLastGoodBin(static_cast<Int_t>(dataSet->GetLastGoodBin()/fAny2ManyInfo->rebin), i);
    }

    // feed histos
    PUIntVector data;
    UInt_t size = 0;
    if (fAny2ManyInfo->rebin == 1) {
      for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
        dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
        if (dataSet == nullptr) { // something is really wrong
          std::cerr << std::endl << ">> PRunDataHandler::WriteNexusFile: **ERROR** Couldn't get data set (idx=" << i << ")";
          std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
          return false;
        }
        size = dataSet->GetData()->size();
        for (UInt_t j=0; j<size; j++) {
          data.push_back((UInt_t)dataSet->GetData()->at(j));
        }
        nxs->GetEntryIdf1()->GetData()->SetHisto(data, i);
        data.clear();
      }
    } else { // rebin > 1
      UInt_t dataRebin = 0;
      UInt_t dataCount = 0;
      for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
        dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
        if (dataSet == nullptr) { // something is really wrong
          std::cerr << std::endl << ">> PRunDataHandler::WriteNexusFile: **ERROR** Couldn't get data set (idx=" << i << ")";
          std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
          return false;
        }
        size = dataSet->GetData()->size();
        dataCount = 0;
        for (UInt_t j=0; j<size; j++) {
          if ((j > 0) && (j % fAny2ManyInfo->rebin == 0)) {
            dataCount++;
            data.push_back(dataRebin);
            dataRebin = 0;
          }
          dataRebin += static_cast<UInt_t>(dataSet->GetData()->at(j));
        }
        nxs->GetEntryIdf1()->GetData()->SetHisto(data, i);
        data.clear();
      }
    }
  } else if (fAny2ManyInfo->idf == 2) {
    // fill necessary data structures
    nxs->SetFileName(fln.Data());

    // set file creating time
    time_t now;
    struct tm *tm;
    time(&now);
    tm = localtime(&now);
    std::string str("");
    char cstr[128];
    strftime(cstr, sizeof(cstr), "%FT%T", tm);
    str = std::string(cstr);
    nxs->SetFileTime(str);

    // NXroot info
    nxs->SetCreator("PSI: any2many");

    // NXentry info
    nxs->GetEntryIdf2()->SetDefinition("muonTD");
    nxs->GetEntryIdf2()->SetProgramName("any2many");
    nxs->GetEntryIdf2()->SetProgramVersion("$Id$");
    nxs->GetEntryIdf2()->SetRunNumber(fData[0].GetRunNumber());
    nxs->GetEntryIdf2()->SetTitle(fData[0].GetRunTitle()->Data());
    str = std::string(fData[0].GetStartDate()->Data()) + std::string("T") + std::string(fData[0].GetStartTime()->Data());
    nxs->GetEntryIdf2()->SetStartTime(str);
    str = std::string(fData[0].GetStopDate()->Data()) + std::string("T") + std::string(fData[0].GetStopTime()->Data());
    nxs->GetEntryIdf2()->SetStopTime(str);

    nxs->GetEntryIdf2()->SetExperimentIdentifier("n/a");

    // NXuser info
    nxs->GetEntryIdf2()->GetUser()->SetName("n/a");

    // NXsample info
    nxs->GetEntryIdf2()->GetSample()->SetName(fData[0].GetSample()->Data());
    nxs->GetEntryIdf2()->GetSample()->SetDescription("n/a");
    nxs->GetEntryIdf2()->GetSample()->SetPhysProp("temperature_1", fData[0].GetTemperature(0), "Kelvin");
    nxs->GetEntryIdf2()->GetSample()->SetPhysProp("magnetic_field_1", fData[0].GetField(), "Gauss");
    nxs->GetEntryIdf2()->GetSample()->SetEnvironmentTemp(fData[0].GetSetup()->Data());
    nxs->GetEntryIdf2()->GetSample()->SetEnvironmentField("n/a");

    // here would be the information for NXinstrument. Currently there are not much information to feed this
    nxs->GetEntryIdf2()->GetInstrument()->SetName(fData[0].GetInstrument()->Data());

    // NXinstrument/NXsource
    nxs->GetEntryIdf2()->GetInstrument()->GetSource()->SetName(fData[0].GetLaboratory()->Data());
    nxs->GetEntryIdf2()->GetInstrument()->GetSource()->SetType(fData[0].GetMuonSource()->Data());
    nxs->GetEntryIdf2()->GetInstrument()->GetSource()->SetProbe(fData[0].GetMuonSpecies()->Data());

    // NXinstrument/NXbeamline
    nxs->GetEntryIdf2()->GetInstrument()->GetBeamline()->SetName(fData[0].GetBeamline()->Data());

    // NXinstrument/NXdetector
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetDescription(fData[0].GetInstrument()->Data()); // assume that this should be the instrument name
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetNoOfPeriods(0); // currently red/green is not distinguished
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetNoOfSpectra(fData[0].GetNoOfHistos());
    PRawRunDataSet *dataSet = fData[0].GetDataSet(0, false); // i.e. the false means, that i is the index and NOT the histo number
    if (dataSet == nullptr) { // something is really wrong
      std::cerr << std::endl << ">> PRunDataHandler::WriteNeXusFile: **ERROR** Couldn't get data set (idx=0)";
      std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
      return false;
    }
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetNoOfBins((unsigned int)(dataSet->GetData()->size() / fAny2ManyInfo->rebin));
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetTimeResolution(fData[0].GetTimeResolution()*fAny2ManyInfo->rebin, "ns");
    int *histo = nullptr;
    int idx = 0;
    if (fAny2ManyInfo->rebin == 1) {
      histo = new int[fData[0].GetNoOfHistos()*dataSet->GetData()->size()];
      idx = 0;
      for (int i=0; i<nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++) {
        dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
        if (dataSet == nullptr) { // something is really wrong
          std::cerr << std::endl << ">> PRunDataHandler::WriteNeXusFile: **ERROR** Couldn't get data set (idx=" << i << ")";
          std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
          return false;
        }
        for (unsigned int j=0; j<dataSet->GetData()->size(); j++) {
          *(histo+idx++) = (int) dataSet->GetData()->at(j);
        }
      }
      nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetHistos(histo);
      // clean up
      if (histo) {
        delete [] histo;
        histo = nullptr;
      }
    } else { // rebin > 1
      histo = new int[fData[0].GetNoOfHistos()*(int)(dataSet->GetData()->size()/fAny2ManyInfo->rebin)];
      int counts = 0;
      idx = 0;
      for (int i=0; i<nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++) {
        dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
        if (dataSet == nullptr) { // something is really wrong
          std::cerr << std::endl << ">> PRunDataHandler::WriteNeXusFile: **ERROR** Couldn't get data set (idx=" << i << ")";
          std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
          return false;
        }
        for (unsigned int j=0; j<dataSet->GetData()->size(); j++) {
          if ((j>0) && (j % fAny2ManyInfo->rebin == 0)) {
            *(histo+idx++) = counts;
            counts = 0;
          }
          counts += (int) dataSet->GetData()->at(j);
        }
      }
      nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetHistos(histo);
      // clean up
      if (histo) {
        delete [] histo;
        histo = nullptr;
      }
    }

    // handle spectrum index
    for (int i=0; i<nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++)
      nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetSpectrumIndex(i+1);

    // handle histogram resolution
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetTimeResolution(fData[0].GetTimeResolution()*fAny2ManyInfo->rebin, "ns");

    // handle raw time
    std::vector<double> raw_time;
    UInt_t size = (unsigned int)(dataSet->GetData()->size() / fAny2ManyInfo->rebin);
    for (unsigned int i=0; i<size; i++) {
      raw_time.push_back((double)i * fData[0].GetTimeResolution() * fAny2ManyInfo->rebin * 1.0e-3); // since time resolution is given in ns, the factor 1.0e-3 is needed to convert to us
    }
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetRawTime(raw_time);
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetRawTimeUnit("micro.second");
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetRawTimeName("time");
    raw_time.clear();

    // handle t0
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetT0Tag(2); // i.e. t0[#histo] format
    int *t0 = new int[fData[0].GetNoOfHistos()];
    int *fgb = new int[fData[0].GetNoOfHistos()];
    int *lgb = new int[fData[0].GetNoOfHistos()];
    if ((t0==0) || (fgb==0) || (lgb==0)) {
      std::cerr << std::endl << ">> PRunDataHandler::WriteNeXusFile: **ERROR** Couldn't allocate memory for t0, fgb, lgb" << std::endl;
      return false;
    }
    for (unsigned int i=0; i<fData[0].GetNoOfHistos(); i++) {
      PRawRunDataSet *dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WriteNeXusFile: **ERROR** Couldn't get data set (idx=0)";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      t0[i] = (int)(dataSet->GetTimeZeroBin() / fAny2ManyInfo->rebin);
      fgb[i] = (int)(dataSet->GetFirstGoodBin() / fAny2ManyInfo->rebin);
      lgb[i] = (int)(dataSet->GetLastGoodBin() / fAny2ManyInfo->rebin);
    }
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetT0(t0);
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetFirstGoodBin(fgb);
    nxs->GetEntryIdf2()->GetInstrument()->GetDetector()->SetLastGoodBin(lgb);

    // clean up
    if (t0) delete [] t0;
    if (fgb) delete [] fgb;
    if (lgb) delete [] lgb;
  } else {
    // clean up
    if (nxs != 0) {
      delete nxs;
      nxs = nullptr;
    }
    return false;
  }

  // filter out the proper file type, i.e. HDF4, HDF5, or XML
  char fileType[32];
  memset(fileType, '\0', 32);
  if (!fAny2ManyInfo->outFormat.CompareTo("nexus1-hdf4", TString::kIgnoreCase) || !fAny2ManyInfo->outFormat.CompareTo("nexus2-hdf4", TString::kIgnoreCase))
    strncpy(fileType, "hdf4", sizeof(fileType));
  else if (!fAny2ManyInfo->outFormat.CompareTo("nexus1-hdf5", TString::kIgnoreCase) || !fAny2ManyInfo->outFormat.CompareTo("nexus2-hdf5", TString::kIgnoreCase))
    strncpy(fileType, "hdf5", sizeof(fileType));
  else if (!fAny2ManyInfo->outFormat.CompareTo("nexus1-xml", TString::kIgnoreCase) || !fAny2ManyInfo->outFormat.CompareTo("nexus2-xml", TString::kIgnoreCase))
    strncpy(fileType, "xml", sizeof(fileType));
  else {
    std::cerr << std::endl << ">> PRunDataHandler::WriteNexusFile(): **ERROR** undefined output NeXus format " << fAny2ManyInfo->outFormat.Data() << " found.";
    std::cerr << std::endl << ">>   Allowed are: hdf4, hdf5, xml" << std::endl;
    if (nxs != 0) {
      delete nxs;
      nxs = nullptr;
    }
    return false;
  }

  // write file
  nxs->WriteFile(fln, fileType, fAny2ManyInfo->idf);

  if (nxs != 0) {
    delete nxs;
    nxs = nullptr;
  }
#else
  std::cout << std::endl << ">> PRunDataHandler::WriteNexusFile(): Sorry, not enabled at configuration level, i.e. --enable-NeXus when executing configure" << std::endl << std::endl;
#endif

  return true;
}

//--------------------------------------------------------------------------
// WriteWkmFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the wkm-file format.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WriteWkmFile(TString fln)
{
  // check if a LEM nemu file needs to be written
  bool lem_wkm_style = false;
  if (!fData[0].GetBeamline()->CompareTo("mue4", TString::kIgnoreCase))
    lem_wkm_style = true;

  Bool_t ok = false;
  if (lem_wkm_style)
    fln = GenerateOutputFileName(fln, ".nemu", ok);
  else
    fln = GenerateOutputFileName(fln, ".wkm", ok);

  if (!ok)
    return false;

  TString fileName = fln;

  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WriteWkmFile(): writing a wkm data file (" << fln.Data() << ") ... " << std::endl;

  // write ascii file
  std::ofstream fout;
  std::streambuf* strm_buffer = nullptr;

  if (!fAny2ManyInfo->useStandardOutput || (fAny2ManyInfo->compressionTag > 0)) {
    // open data-file
    fout.open(fln.Data(), std::ofstream::out);
    if (!fout.is_open()) {
      std::cerr << std::endl << ">> PRunDataHandler::WriteWkmFile **ERROR** Couldn't open data file (" << fln.Data() << ") for writing, sorry ...";
      std::cerr << std::endl;
      return false;
    }

    // save output buffer of the stream
    strm_buffer = std::cout.rdbuf();

    // redirect output into the file
    std::cout.rdbuf(fout.rdbuf());
  }

  // write header
  std::cout << "- WKM data file converted with any2many";
  if (lem_wkm_style) {
    std::cout << std::endl << "NEMU_Run:            " << fData[0].GetRunNumber();
    std::cout << std::endl << "nemu_Run:            " << fileName.Data();
  } else {
    std::cout << std::endl << "Run:                 " << fData[0].GetRunNumber();
  }
  std::cout << std::endl << "Date:                " << fData[0].GetStartTime()->Data() << " " << fData[0].GetStartDate()->Data() << " / " << fData[0].GetStopTime()->Data() << " " << fData[0].GetStopDate()->Data();
  if (fData[0].GetRunTitle()->Length() > 0)
    std::cout << std::endl << "Title:               " << fData[0].GetRunTitle()->Data();
  if (fData[0].GetField() != PMUSR_UNDEFINED) {
    std::cout << std::endl << "Field:               " << fData[0].GetField();
  } else {
    std::cout << std::endl << "Field:               ??";
  }
  std::cout << std::endl << "Setup:               " << fData[0].GetSetup()->Data();
  if (fData[0].GetNoOfTemperatures() == 1) {
    std::cout << std::endl << "Temp:                " << fData[0].GetTemperature(0);
  } else if (fData[0].GetNoOfTemperatures() > 1) {
    std::cout << std::endl << "Temp(meas1):         " << fData[0].GetTemperature(0) << " +- " << fData[0].GetTempError(0);
    std::cout << std::endl << "Temp(meas2):         " << fData[0].GetTemperature(1) << " +- " << fData[0].GetTempError(1);
  } else {
    std::cout << std::endl << "Temp:                ??";
  }
  if (lem_wkm_style)
    std::cout << std::endl << "TOF(M3S1):           nocut";
  std::cout << std::endl << "Groups:              " << fData[0].GetNoOfHistos();
  UInt_t histo0 = 1;
  if (fAny2ManyInfo->groupHistoList.size() != 0) { // red/green list found
    histo0 = fAny2ManyInfo->groupHistoList[0]+1; // take the first available red/green entry
  }
  std::cout << std::endl << "Channels:            " << static_cast<UInt_t>(fData[0].GetDataBin(histo0)->size()/fAny2ManyInfo->rebin);
  std::cout.precision(10);
  std::cout << std::endl << "Resolution:          " << fData[0].GetTimeResolution()*fAny2ManyInfo->rebin/1.0e3; // ns->us
  std::cout.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed

  // write data
  UInt_t no_of_bins_per_line = 16;
  if (lem_wkm_style)
    no_of_bins_per_line = 10;

  PRawRunDataSet *dataSet;

  if (fAny2ManyInfo->rebin == 1) {
    for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
      std::cout << std::endl << std::endl;
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      for (UInt_t j=0; j<dataSet->GetData()->size(); j++) {
        if ((j > 0) && (j % no_of_bins_per_line == 0))
          std::cout << std::endl;
        if (lem_wkm_style)
          std::cout << std::setw(8) << static_cast<Int_t>(dataSet->GetData()->at(j));
        else
          std::cout << static_cast<Int_t>(dataSet->GetData()->at(j)) << " ";
      }
    }
  } else { // rebin > 1
    Int_t dataRebin = 0;
    UInt_t count = 0;
    for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
      std::cout << std::endl << std::endl;
      count = 0;
      dataRebin = 0; // reset rebin
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      for (UInt_t j=0; j<dataSet->GetData()->size(); j++) {
        if ((j > 0) && (j % fAny2ManyInfo->rebin == 0)) {
          if (lem_wkm_style)
            std::cout << std::setw(8) << dataRebin;
          else
            std::cout << dataRebin << " ";
          count++;
          dataRebin = 0;
          if ((count > 0) && (count % no_of_bins_per_line == 0) && (j != dataSet->GetData()->size()-1))
            std::cout << std::endl;
        } else {
          dataRebin += static_cast<Int_t>(dataSet->GetData()->at(j));
        }
      }
    }
  }

  if (!fAny2ManyInfo->useStandardOutput || (fAny2ManyInfo->compressionTag > 0)) {
    // restore old output buffer
    std::cout.rdbuf(strm_buffer);

    fout.close();
  }

  return true;
}

//--------------------------------------------------------------------------
// WritePsiBinFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the psi-bin-file format.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WritePsiBinFile(TString fln)
{
  Bool_t ok = false;
  fln = GenerateOutputFileName(fln, ".bin", ok);
  if (!ok)
    return false;

  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WritePsiBinFile(): writing a psi-bin data file (" << fln.Data() << ") ... " << std::endl;

  MuSR_td_PSI_bin psibin;
  int status = 0;

  // fill header information
  // run number
  psibin.PutRunNumberInt(fData[0].GetRunNumber());
  // length of histograms
  UInt_t histo0 = 1;
  if (fAny2ManyInfo->groupHistoList.size() != 0) { // red/green list found
    histo0 = fAny2ManyInfo->groupHistoList[0]+1; // take the first available red/green entry
  }
  psibin.PutHistoLengthBin(static_cast<int>(fData[0].GetDataBin(histo0)->size()/fAny2ManyInfo->rebin));
  // number of histograms
  psibin.PutNumberHistoInt(static_cast<int>(fData[0].GetNoOfHistos()));
  // run title = sample (10 char) / temp (10 char) / field (10 char) / orientation (10 char)
  char cstr[11];
  // sample
  if (fData[0].GetSample()->Length() > 0)
    strncpy(cstr, fData[0].GetSample()->Data(), 10);
  else
    strcpy(cstr, "??");
  cstr[10] = '\0';
  psibin.PutSample(cstr);
  // temp
  if (fData[0].GetNoOfTemperatures() > 0)
    snprintf(cstr, 10, "%.1f K", fData[0].GetTemperature(0));
  else
    strcpy(cstr, "?? K");
  cstr[10] = '\0';
  psibin.PutTemp(cstr);
  // field
  if (fData[0].GetField() > 0)
    snprintf(cstr, 10, "%.1f G", fData[0].GetField());
  else
    strcpy(cstr, "?? G");
  cstr[10] = '\0';
  psibin.PutField(cstr);
  // orientation
  if (fData[0].GetOrientation()->Length() > 0)
    strncpy(cstr, fData[0].GetOrientation()->Data(), 10);
  else
    strcpy(cstr, "??");
  cstr[10] = '\0';
  psibin.PutOrient(cstr);
  // setup
  if (fData[0].GetSetup()->Length() > 0)
    strncpy(cstr, fData[0].GetSetup()->Data(), 10);
  else
    strcpy(cstr, "??");
  cstr[10] = '\0';
  psibin.PutSetup(cstr);

  // handle PSI-BIN start/stop Time/Date. PSI-BIN requires: Time -> HH:MM:SS, and Date -> DD-MMM-YY
  //                                      internally given: Time -> HH:MM:SS, and Date -> YYYY-MM-DD
  // run start date
  std::vector<std::string> svec;
  TString str, date;
  TDatime dt;
  int     year, month, day;
  
  // 28-Aug-2014, TP: the following line does not work, it generates the wrong date
  //dt.Set(fData[0].GetStartDateTime()); //as35
  // the following generates the correct date entry
  date.Append(*fData[0].GetStartDate());
  sscanf((const char*)date.Data(),"%04d-%02d-%02d", &year, &month, &day);
  dt.Set(year, month, day, 0, 0, 0);
  
  date.Form("%02d-", dt.GetDay());
  date.Append(GetMonth(dt.GetMonth()));
  date.Append("-");
  date.Append(GetYear(dt.GetYear()));
  strncpy(cstr, date.Data(), 9);
  cstr[9] = '\0';
  svec.push_back(cstr);
  // run start time
  strncpy(cstr, fData[0].GetStartTime()->Data(), 8);
  cstr[8] = '\0';
  svec.push_back(cstr);
  psibin.PutTimeStartVector(svec);
  svec.clear();

  // run stop date
  // 28-Aug-2014, TP: the following line does not work, it generates the wrong date
  //dt.Set(fData[0].GetStopDateTime());
  // the following generates the correct date entry
  date.Clear();
  date.Append(*fData[0].GetStopDate());
  sscanf((const char*)date.Data(),"%04d-%02d-%02d", &year, &month, &day);
  dt.Set(year, month, day, 0, 0, 0);
  
  date.Form("%02d-", dt.GetDay());
  date.Append(GetMonth(dt.GetMonth()));
  date.Append("-");
  date.Append(GetYear(dt.GetYear()));
  strncpy(cstr, date.Data(), 9);
  cstr[9] = '\0';
  svec.push_back(cstr);
  // run stop time
  strncpy(cstr, fData[0].GetStopTime()->Data(), 8);
  cstr[8] = '\0';
  svec.push_back(cstr);
  psibin.PutTimeStopVector(svec);
  svec.clear();

  // number of measured temperatures
  psibin.PutNumberTemperatureInt(fData[0].GetNoOfTemperatures());

  // mean temperatures
  std::vector<double> dvec;
  for (UInt_t i=0; i<fData[0].GetNoOfTemperatures(); i++)
    dvec.push_back(fData[0].GetTemperature(i));
  psibin.PutTemperaturesVector(dvec);

  // standard deviation of temperatures
  dvec.clear();
  for (UInt_t i=0; i<fData[0].GetNoOfTemperatures(); i++)
    dvec.push_back(fData[0].GetTempError(i));
  psibin.PutDevTemperaturesVector(dvec);

  // write comment
  psibin.PutComment(fData[0].GetRunTitle()->Data());

  // write time resolution
  psibin.PutBinWidthNanoSec(fData[0].GetTimeResolution()*fAny2ManyInfo->rebin);

  // write scaler dummies
  psibin.PutNumberScalerInt(0);

  // feed detector related info like, histogram names, t0, fgb, lgb
  Int_t ival = 0;
  PRawRunDataSet *dataSet;
  UInt_t size = fData[0].GetNoOfHistos();
  for (UInt_t i=0; i<size; i++) {
    dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
    if (dataSet == nullptr) { // something is really wrong
      std::cerr << std::endl << ">> PRunDataHandler::WritePsiBinFile: **ERROR** Couldn't get data set (idx=" << i << ")";
      std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
      return false;
    }

    // detector name
    str = dataSet->GetName();
    if (!str.CompareTo("n/a"))
      str.Form("Detector%3d", i+1);
    psibin.PutNameHisto(str.Data(), i);
    // time zero bin
    ival = static_cast<Int_t>(dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin);
    psibin.PutT0Int(i, ival);
    // first good bin
    ival = static_cast<Int_t>(dataSet->GetFirstGoodBin()/fAny2ManyInfo->rebin);
    psibin.PutFirstGoodInt(i, ival);
    // last good bin
    ival = static_cast<Int_t>(dataSet->GetLastGoodBin()/fAny2ManyInfo->rebin);
    psibin.PutLastGoodInt(i, ival);
  }

  // feed histos
  std::vector< std::vector<int> > histos;
  histos.resize(fData[0].GetNoOfHistos());
  UInt_t length = 0;
  if (fAny2ManyInfo->rebin == 1) {
    for (UInt_t i=0; i<size; i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WritePsiBinFile: **ERROR** Couldn't get data set (idx=" << i << ")";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      length = dataSet->GetData()->size();
      histos[i].resize(length);
      for (UInt_t j=0; j<length; j++) {
        histos[i][j] = static_cast<Int_t>(dataSet->GetData()->at(j));
      }
    }
  } else { // rebin > 1
    UInt_t dataRebin = 0;
    UInt_t dataCount = 0;
    for (UInt_t i=0; i<size; i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      if (dataSet == nullptr) { // something is really wrong
        std::cerr << std::endl << ">> PRunDataHandler::WritePsiBinFile: **ERROR** Couldn't get data set (idx=" << i << ")";
        std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
        return false;
      }
      length = dataSet->GetData()->size();
      dataCount = 0;
      for (UInt_t j=0; j<length; j++) {
        if ((j > 0) && (j % fAny2ManyInfo->rebin == 0)) {
          dataCount++;
          histos[i].push_back(dataRebin);
          dataRebin = 0;
        }
        dataRebin += static_cast<UInt_t>(dataSet->GetData()->at(j));
      }
    }
  }
  status = psibin.PutHistoArrayInt(histos, 2); // tag 2 means: lift histo length restriction on only make sure it is < 32512
  if (status != 0) {
    std::cerr << std::endl << ">> PRunDataHandler::WritePsiBinFile(): " << psibin.ConsistencyStatus() << std::endl;
    return false;
  }

  if (!psibin.CheckDataConsistency(2)) {
    std::cerr << std::endl << ">> PRunDataHandler::WritePsiBinFile(): " << psibin.ConsistencyStatus() << std::endl;
    return false;
  }

  // write data to file
  status = psibin.Write(fln.Data());

  if (status != 0) {
    std::cerr << std::endl << ">> PRunDataHandler::WritePsiBinFile(): " << psibin.WriteStatus() << std::endl;
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
// WriteMudFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the mud-file format.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WriteMudFile(TString fln)
{
  Bool_t ok = false;
  fln = GenerateOutputFileName(fln, ".msr", ok);
  if (!ok)
    return false;

  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WriteMudFile(): writing a mud data file (" << fln.Data() << ") ... " << std::endl;

  // generate the mud data file
  int fd = MUD_openWrite((char*)fln.Data(), MUD_FMT_TRI_TD_ID);
  if (fd == -1) {
    std::cerr << std::endl << ">> PRunDataHandler::WriteMudFile(): **ERROR** couldn't open mud data file for write ..." << std::endl;
    return false;
  }

  // generate header information
  char dummy[32], info[128];
  strcpy(dummy, "???");
  MUD_setRunDesc(fd, MUD_SEC_GEN_RUN_DESC_ID);
  MUD_setExptNumber(fd, 0);
  MUD_setRunNumber(fd, fData[0].GetRunNumber());
  Int_t ival = fData[0].GetStopDateTime()-fData[0].GetStartDateTime();
  MUD_setElapsedSec(fd, ival);
  MUD_setTimeBegin(fd, fData[0].GetStartDateTime());
  MUD_setTimeEnd(fd, fData[0].GetStopDateTime());
  MUD_setTitle(fd, (char *)fData[0].GetRunTitle()->Data());
  MUD_setLab(fd, (char *)fData[0].GetLaboratory()->Data());
  MUD_setArea(fd, (char *)fData[0].GetBeamline()->Data());
  MUD_setMethod(fd, (char *)fData[0].GetSetup()->Data());
  MUD_setApparatus(fd, (char *)fData[0].GetInstrument()->Data());
  MUD_setInsert(fd, dummy);
  MUD_setSample(fd, (char *)fData[0].GetSample()->Data());
  MUD_setOrient(fd, (char *)fData[0].GetOrientation()->Data());
  MUD_setDas(fd, dummy);
  MUD_setExperimenter(fd, dummy);
  sprintf(info, "%lf+-%lf (K)", fData[0].GetTemperature(0), fData[0].GetTempError(0));
  MUD_setTemperature(fd, info);
  sprintf(info, "%lf", fData[0].GetField());
  MUD_setField(fd, info);

  // generate the histograms
  MUD_setHists(fd, MUD_GRP_TRI_TD_HIST_ID, fData[0].GetNoOfHistos());

  UInt_t *data, dataSize = fData[0].GetDataSet(0, false)->GetData()->size()/fAny2ManyInfo->rebin + 1;
  data = new UInt_t[dataSize];
  if (data == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::WriteMudFile(): **ERROR** couldn't allocate memory for the data ..." << std::endl;
    MUD_closeWrite(fd);
    return false;
  }

  UInt_t noOfEvents = 0, k = 0;
  ival = 0;
  PRawRunDataSet *dataSet;
  for (UInt_t i=0; i<fData[0].GetNoOfHistos(); i++) {
    dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
    if (dataSet == nullptr) { // something is really wrong
      std::cerr << std::endl << ">> PRunDataHandler::WriteMudFile: **ERROR** Couldn't get data set (idx=" << i << ")";
      std::cerr << std::endl << ">>   something is really wrong!" << std::endl;
      return false;
    }

    // fill data
    for (UInt_t j=0; j<dataSize; j++)
      data[j] = 0;
    noOfEvents = 0;
    k = 0;
    for (UInt_t j=0; j<dataSet->GetData()->size(); j++) {
      if ((j > 0) && (j % fAny2ManyInfo->rebin == 0)) {
        data[k] = ival;
        noOfEvents += ival;
        k++;
        ival = 0;
      }
      ival += static_cast<UInt_t>(dataSet->GetData()->at(j));
    }

    // feed data relevant information
    // the numbering of the histograms start from '1', hence i+1 needed!!
    MUD_setHistType(fd, i+1, MUD_GRP_TRI_TD_HIST_ID);
    MUD_setHistNumBytes(fd, i+1, sizeof(data));
    MUD_setHistNumBins(fd, i+1, dataSize);
    MUD_setHistBytesPerBin(fd, i+1, 0);
    MUD_setHistFsPerBin(fd, i+1, static_cast<UINT32>(1.0e6*fAny2ManyInfo->rebin*fData[0].GetTimeResolution())); // time resolution is given in (ns)
    MUD_setHistT0_Ps(fd, i+1, static_cast<UINT32>(1.0e3*fData[0].GetTimeResolution()*((dataSet->GetTimeZeroBin()+fAny2ManyInfo->rebin/2)/fAny2ManyInfo->rebin)));
    MUD_setHistT0_Bin(fd, i+1, static_cast<UINT32>(dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin));
    MUD_setHistGoodBin1(fd, i+1, static_cast<UINT32>(dataSet->GetFirstGoodBin()/fAny2ManyInfo->rebin));
    MUD_setHistGoodBin2(fd, i+1, static_cast<UINT32>(dataSet->GetLastGoodBin()/fAny2ManyInfo->rebin));
    MUD_setHistBkgd1(fd, i+1, 0);
    MUD_setHistBkgd2(fd, i+1, 0);
    MUD_setHistNumEvents(fd, i+1, (UINT32)noOfEvents);
    MUD_setHistTitle(fd, i+1, (char *)dataSet->GetName().Data());
    REAL64 timeResolution = (fAny2ManyInfo->rebin*fData[0].GetTimeResolution())/1.0e9; // ns -> s
    MUD_setHistSecondsPerBin(fd, i+1, timeResolution);

    MUD_setHistData(fd, i+1, data);
  }

  MUD_closeWrite(fd);

  delete [] data;

  // check if mud file shall be streamed to stdout
  if (fAny2ManyInfo->useStandardOutput && (fAny2ManyInfo->compressionTag == 0)) {
    // stream file to stdout
    std::ifstream is;
    int length=1024;
    char *buffer;

    is.open(fln.Data(), std::ios::binary);
    if (!is.is_open()) {
      std::cerr << std::endl << "PRunDataHandler::WriteMudFile(): **ERROR** Couldn't open the mud-file for streaming." << std::endl;
      remove(fln.Data());
      return false;
    }

    // get length of file
    is.seekg(0, std::ios::end);
    length = is.tellg();
    is.seekg(0, std::ios::beg);

    if (length == -1) {
      std::cerr << std::endl << "PRunDataHandler::WriteMudFile(): **ERROR** Couldn't determine the mud-file size." << std::endl;
      remove(fln.Data());
      return false;
    }

    // allocate memory
    buffer = new char [length];

    // read data as a block
    while (!is.eof()) {
      is.read(buffer, length);
      std::cout.write(buffer, length);
    }

    is.close();

    delete [] buffer;

    // delete temporary root file
    remove(fln.Data());
  }
  return true;
}

//--------------------------------------------------------------------------
// WriteAsciiFile (private)
//--------------------------------------------------------------------------
/**
 * <p> Write the ascii-file format.
 *
 * <b>return:</b>
 * - true on successful writting,
 * - otherwise false.
 *
 * \param fln file name. If empty, the routine will try to construct one
 */
Bool_t PRunDataHandler::WriteAsciiFile(TString fln)
{
  Bool_t ok = false;
  fln = GenerateOutputFileName(fln, ".ascii", ok);
  if (!ok)
    return false;

  TString fileName = fln;

  if (!fAny2ManyInfo->useStandardOutput)
    std::cout << std::endl << ">> PRunDataHandler::WriteAsciiFile(): writing an ascii data file (" << fln.Data() << ") ... " << std::endl;

  // write ascii file
  std::ofstream fout;
  std::streambuf* strm_buffer = nullptr;

  if (!fAny2ManyInfo->useStandardOutput || (fAny2ManyInfo->compressionTag > 0)) {
    // open data-file
    fout.open(fln.Data(), std::ofstream::out);
    if (!fout.is_open()) {
      std::cerr << std::endl << ">> PRunDataHandler::WriteAsciiFile **ERROR** Couldn't open data file (" << fln.Data() << ") for writing, sorry ...";
      std::cerr << std::endl;
      return false;
    }

    // save output buffer of the stream
    strm_buffer = std::cout.rdbuf();

    // redirect output into the file
    std::cout.rdbuf(fout.rdbuf());
  }

  // write header
  std::cout << "%*************************************************************************";
  std::cout << std::endl << "% file name   : " << fileName.Data();
  if (fData[0].GetRunTitle()->Length() > 0)
    std::cout << std::endl << "% title       : " << fData[0].GetRunTitle()->Data();
  if (fData[0].GetRunNumber() >= 0)
    std::cout << std::endl << "% run number  : " << fData[0].GetRunNumber();
  if (fData[0].GetSetup()->Length() > 0)
    std::cout << std::endl << "% setup       : " << fData[0].GetSetup()->Data();
  std::cout << std::endl << "% field       : " << fData[0].GetField() << " (G)";
  if (fData[0].GetStartTime()->Length() > 0)
    std::cout << std::endl << "% date        : " << fData[0].GetStartTime()->Data() << " " << fData[0].GetStartDate()->Data() << " / " << fData[0].GetStopTime()->Data() << " " << fData[0].GetStopDate()->Data();
  if (fData[0].GetNoOfTemperatures() > 0) {
    std::cout << std::endl << "% temperature : ";
    for (UInt_t i=0; i<fData[0].GetNoOfTemperatures()-1; i++) {
      std::cout << fData[0].GetTemperature(i) << "+-" << fData[0].GetTempError(i) << "(K) , ";
    }
    std::cout << fData[0].GetTemperature(fData[0].GetNoOfTemperatures()-1) << "+-" << fData[0].GetTempError(fData[0].GetNoOfTemperatures()-1) << "(K)";
  }
  if (fData[0].GetEnergy() != PMUSR_UNDEFINED)
    std::cout << std::endl << "% energy      : " << fData[0].GetEnergy() << " (keV)";
  if (fData[0].GetTransport() != PMUSR_UNDEFINED)
    std::cout << std::endl << "% transport   : " << fData[0].GetTransport() << " (kV)";
  if (fData[0].GetTimeResolution() != PMUSR_UNDEFINED) {
    std::cout.precision(10);
    std::cout << std::endl << "% time resolution : " << fData[0].GetTimeResolution()*fAny2ManyInfo->rebin << " (ns)";
    std::cout.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
  }  
  PRawRunDataSet *dataSet;
  std::cout << std::endl << "% t0          : ";
  for (UInt_t i=0; i<fData[0].GetNoOfHistos()-1; i++) {
    dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
    std::cout << static_cast<UInt_t>(dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin) << ", ";
  }
  dataSet = fData[0].GetDataSet(fData[0].GetNoOfHistos()-1, false); // i.e. the false means, that i is the index and NOT the histo number
  std::cout << static_cast<UInt_t>(dataSet->GetTimeZeroBin()/fAny2ManyInfo->rebin);

  std::cout << std::endl << "% # histos    : " << fData[0].GetNoOfHistos();
  dataSet = fData[0].GetDataSet(0, false); // i.e. the false means, that i is the index and NOT the histo number
  std::cout << std::endl << "% # of bins   : " << static_cast<UInt_t>(dataSet->GetData()->size()/fAny2ManyInfo->rebin);
  std::cout << std::endl << "%*************************************************************************";

  // write data
  UInt_t length = fData[0].GetDataSet(0,false)->GetData()->size();
  if (fAny2ManyInfo->rebin == 1) {
    for (UInt_t i=0; i<length; i++) {
      std::cout << std::endl;
      for (UInt_t j=0; j<fData[0].GetNoOfHistos(); j++) {
        dataSet = fData[0].GetDataSet(j, false); // i.e. the false means, that i is the index and NOT the histo number
        std::cout.width(8);
        std::cout << static_cast<Int_t>(dataSet->GetData()->at(i));
      }
    }
  } else {
    PUIntVector dataRebin(fData[0].GetNoOfHistos());

    // initialize the dataRebin vector
    for (UInt_t i=0; i<dataRebin.size(); i++) {
      dataSet = fData[0].GetDataSet(i, false); // i.e. the false means, that i is the index and NOT the histo number
      dataRebin[i] = static_cast<UInt_t>(dataSet->GetData()->at(0));
    }

    for (UInt_t i=0; i<length; i++) {
      if ((i > 0) && ((i % fAny2ManyInfo->rebin) == 0)) {
        std::cout << std::endl;
        for (UInt_t j=0; j<dataRebin.size(); j++) {
          std::cout.width(8);
          std::cout << dataRebin[j];
        }
        // initialize the dataRebin vector for the next pack
        for (UInt_t j=0; j<dataRebin.size(); j++) {
          dataSet = fData[0].GetDataSet(j, false); // i.e. the false means, that i is the index and NOT the histo number
          dataRebin[j] = static_cast<UInt_t>(dataSet->GetData()->at(i));
        }
      } else {
        for (UInt_t j=0; j<fData[0].GetNoOfHistos(); j++) {
          dataSet = fData[0].GetDataSet(j, false); // i.e. the false means, that i is the index and NOT the histo number
          dataRebin[j] += static_cast<UInt_t>(dataSet->GetData()->at(i));
        }
      }
    }
  }

  std::cout << std::endl;

  if (!fAny2ManyInfo->useStandardOutput || (fAny2ManyInfo->compressionTag > 0)) {
    // restore old output buffer
    std::cout.rdbuf(strm_buffer);

    fout.close();
  }

  return true;
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
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 */
Bool_t PRunDataHandler::StripWhitespace(TString &str)
{
  Char_t *s    = nullptr;
  Char_t *subs = nullptr;
  Int_t i;
  Int_t start;
  Int_t end;
  Int_t size;

  size = static_cast<Int_t>(str.Length());
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
  subs[end-start+1] = '\0';

  str = TString(subs);

  // clean up
  if (subs) {
    delete [] subs;
    subs = nullptr;
  }
  if (s) {
    delete [] s;
    s = nullptr;
  }

  return true;
}

//--------------------------------------------------------------------------
// IsWhitespace (private)
//--------------------------------------------------------------------------
/**
 * <p> Check if a string consists only of white spaces, i.e. spaces and/or
 * ctrl-characters.
 *
 * <b>return:</b>
 * - true at successful reading,
 * - otherwise false.
 *
 * \param str string to be checked
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
 * <b>return:</b>
 * - returns the converted string
 * - otherwise 0.0 with ok==false
 *
 * \param str string to be converted
 * \param ok true on success, otherwise false.
 */
Double_t PRunDataHandler::ToDouble(TString &str, Bool_t &ok)
{
  Char_t *s;
  Double_t value;
  Int_t size, status;

  ok = true;

  size = static_cast<Int_t>(str.Length());
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
    s = nullptr;
  }

  return value;
}

//--------------------------------------------------------------------------
// ToInt (private)
//--------------------------------------------------------------------------
/**
 * <p> Convert a string to an Int_t.
 *
 * <b>return:</b>
 * - returns the converted string
 * - otherwise 0.0 with ok==false
 *
 * \param str string to be converted
 * \param ok true on success, otherwise false.
 */
Int_t PRunDataHandler::ToInt(TString &str, Bool_t &ok)
{
  Char_t *s;
  Int_t value;
  Int_t size, status;

  ok = true;

  size = static_cast<Int_t>(str.Length());
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
    s = nullptr;
  }

  return value;
}

//--------------------------------------------------------------------------
// GetDataTagIndex (private)
//--------------------------------------------------------------------------
/**
 * <p> Checks if str is in a list of data tags
 *
 * <b>return:</b>
 * - if found returns the data tag index (from the dataTags vector),
 * - otherwise -1
 *
 * \param str data tag string (see description of nonMusr db-data)
 * \param dataTags vector of all data tags
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


//--------------------------------------------------------------------------
// GenerateOutputFileName (private)
//--------------------------------------------------------------------------
/**
 * <p>Generates the output file name (any2many). It also makes sure that the
 * generated output file name does not coincidentally is identical to an already
 * existing file.
 *
 * <b>return:</b>
 * - constructed file name
 * - empty string
 *
 * \param fileName
 * \param extension if the file name to be constructed
 * \param ok flag which is 'true' if the file name could be constructed, 'false' otherwise
 */
TString PRunDataHandler::GenerateOutputFileName(const TString fileName, const TString extension, Bool_t &ok)
{
  TString fln = fileName;
  ok = true;

  if (fAny2ManyInfo == nullptr)
    return fln;

  // generate output file name if needed
  if (!fAny2ManyInfo->useStandardOutput || (fAny2ManyInfo->compressionTag > 0)) {
    if (fln.Length() == 0) {
      ok = false;
      fln = GetFileName(extension, ok);
      if (!ok) {
        fln = "";
        return fln;
      }
    } else {
      fln.Prepend(fAny2ManyInfo->outPath);
    }

    // make sure that the file name doesn't already exist as a real file
    if (!gSystem->AccessPathName(fln)) { // file name already exists!!
      Int_t count = 1;
      TString newFln;
      do {
        newFln = fln;
        newFln.Insert(newFln.Last('.'), TString::Format(".%d", count++));
      } while (!gSystem->AccessPathName(newFln));
      std::cerr << std::endl << ">> PRunDataHandler::GenerateOutputFileName **WARNING** needed to modify output filename to " << newFln << ",";
      std::cerr << std::endl << ">>    due to potential conflict with already existing files." << std::endl << std::endl;
      fln = newFln;
    }

    // keep the file name if compression is whished
    fAny2ManyInfo->outPathFileName.push_back(fln);
  } else {
    fln = fAny2ManyInfo->outPath + TString("__tmp.") + extension;
  }

  return fln;
}

//--------------------------------------------------------------------------
// GetFileName (private)
//--------------------------------------------------------------------------
/**
 * <p>Construct the file name based on the any2many request.
 *
 * <b>return:</b>
 * - constructed file name
 * - empty string
 *
 * \param extension if the file name to be constructed
 * \param ok flag which is 'true' if the file name could be constructed, 'false' otherwise
 */
TString PRunDataHandler::GetFileName(const TString extension, Bool_t &ok)
{
  TString fileName = "";
  ok = true;

  Int_t start = fRunPathName.Last('/');
  Int_t end = fRunPathName.Last('.');
  if (end == -1) {
    std::cerr << std::endl << ">> PRunDataHandler::GetFileName(): **ERROR** couldn't generate the output file name ..." << std::endl;
    ok = false;
    return fileName;
  }
  // cut out the filename (get rid of the extension, and the path)
  Char_t str1[1024], str2[1024];
  strncpy(str1, fRunPathName.Data(), sizeof(str1));
  for (Int_t i=0; i<end-start-1; i++) {
    str2[i] = str1[i+start+1];
  }
  str2[end-start-1] = 0;

  if (fAny2ManyInfo->inFormat == fAny2ManyInfo->outFormat) { // only rebinning
    TString rebinStr;
    rebinStr += fAny2ManyInfo->rebin;
    fileName = fAny2ManyInfo->outPath + str2 + "_rebin" + rebinStr + extension;
  } else { // real conversion
    fileName = fAny2ManyInfo->outPath + str2 + extension;
  }

  return fileName;
}

//--------------------------------------------------------------------------
// FileNameFromTemplate (private)
//--------------------------------------------------------------------------
/**
 * <p>Will, based on the fileNameTemplate construct a file name. The file name template has a run tag
 * and a year tag. The run tag has the form [rrrr], whereas the number of 'r' defines the length of
 * this string part. If the run number is shorter, leading zeros will be filled, e.g. [rrrrrr] and run==123
 * will lead to '000123'. The same is true for the year tag [yy], except that the length must agree.
 * For example: fileNameTemplate = d[yyyy]/deltat_tdc_gps_[rrrr].bin, run = 123, year = 2009 will result
 * in 'd2009/deltat_tdc_gps_0123.bin'.
 *
 * <b>return:</b>
 * - constructed file name from template, run number, and year
 * - empty string
 *
 * \param template template string
 * \param run run number
 * \param year year
 * \param ok true if operation went smooth, otherwise false
 */
TString PRunDataHandler::FileNameFromTemplate(TString &fileNameTemplate, Int_t run, TString &year, Bool_t &ok)
{
  TString result("");

  TObjArray *tok=nullptr;
  TObjString *ostr;
  TString str;

  // check year string
  if ((year.Length() != 2) && (year.Length() != 4)) {
    std::cerr << std::endl << ">> PRunDataHandler::FileNameFromTemplate: **ERROR** year needs to be of the format";
    std::cerr << std::endl << ">>    'yy' or 'yyyy', found " << year << std::endl;
    return result;
  }

  // make a short year version 'yy' and a long year version 'yyyy'
  TString yearShort="", yearLong="";
  if (year.Length() == 2) {
    yearShort = year;
    yearLong = "20" ;
    yearLong += year;
  } else {
    yearShort = year[2];
    yearShort += year[3];
    yearLong = year;
  }

  // tokenize template string
  tok = fileNameTemplate.Tokenize("[]");
  if (tok == nullptr) {
    std::cerr << std::endl << ">> PRunDataHandler::FileNameFromTemplate: **ERROR** couldn't tokenize template!" << std::endl;
    return result;
  }
  if (tok->GetEntries()==1) {
    std::cerr << std::endl << ">> PRunDataHandler::FileNameFromTemplate: **WARNING** template without tags." << std::endl;
  }

  // go through the tokens and generate the result string
  for (Int_t i=0; i<tok->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tok->At(i));
    str = ostr->GetString();

    // check tokens
    if (!str.CompareTo("yy", TString::kExact)) { // check for 'yy'
      result += yearShort;
    } else if (!str.CompareTo("yyyy", TString::kExact)) { // check for 'yyyy'
      result += yearLong;
    } else if (str.Contains("rr")) { // check for run
      // make sure ONLY 'r' are present
      Int_t idx;
      for (idx=0; idx<str.Length(); idx++) {
        if (str[idx] != 'r')
          break;
      }
      if (idx == str.Length()) { // 'r' only
        TString runStr("");
        char fmt[128];
        sprintf(fmt , "%%0%dd", str.Length());
        runStr.Form(fmt, run);
        result += runStr;
      } else { // not only 'r'
        result += str;
      }
    } else { // all parts which are NOT tags
      result += str;
    }
  }

  // clean up
  if (tok)
    delete tok;

  // everything fine here
  ok = true;

  return result;
}

//--------------------------------------------------------------------------
// DateToISO8601 (private)
//--------------------------------------------------------------------------
/**
 * <p>If possible converts a inDate, into a ISO8601 standard date.
 *
 * <p><b>return:</b> true if conversion was successfull otherwise false.
 *
 * \param inDate input date which should be converted to an ISO 8601 date.
 * \param iso8601Date on success the converted iso8601Date, otherwise an empty string
 */
bool PRunDataHandler::DateToISO8601(std::string inDate, std::string &iso8601Date)
{
  iso8601Date = std::string("");

  struct tm tm;

  // currently only dates like dd-mmm-yy are handled, where dd day of the month, mmm month as abbrivation, e.g. JAN, and yy as the year
  if (!strptime(inDate.c_str(), "%d-%b-%y", &tm)) // failed
    return false;

  TString str("");
  str.Form("%04d-%02d-%02d", 1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday);

  iso8601Date = str.Data();

  return true;
}

//--------------------------------------------------------------------------
// SplitTimeData (private)
//--------------------------------------------------------------------------
/**
 * <p>splits an ISO 8601 timeDate into seperate time and data. The flag ok
 * shows if it was successfull.
 *
 * \param timeData ISO 8601 timeData
 * \param time part of timeData
 * \param date part of timeData
 * \param ok flag showing if the splitting was successfull
 */
void PRunDataHandler::SplitTimeDate(TString timeData, TString &time, TString &date, Bool_t &ok)
{
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(timeData.Data(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(timeData.Data(), "%Y-%m-%dT%H:%M:S", &tm);

  if (tm.tm_year == 0) {
    ok = false;
    return;
  }

  time = TString::Format("%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
  date = TString::Format("%04d-%02d-%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
}

//--------------------------------------------------------------------------
// GetMonth (private)
//--------------------------------------------------------------------------
/**
 * <p>given the month as number, convert it to a 3 character month.
 * If month is out of range (i.e. not between 1 and 12) a '???' will
 * be returned.
 *
 * \param month as number
 */
TString PRunDataHandler::GetMonth(Int_t month)
{
  TString monthString = "???";

  switch (month) {
    case 1:
      monthString = "JAN";
      break;
    case 2:
      monthString = "FEB";
      break;
    case 3:
      monthString = "MAR";
      break;
    case 4:
      monthString = "APR";
      break;
    case 5:
      monthString = "MAY";
      break;
    case 6:
      monthString = "JUN";
      break;
    case 7:
      monthString = "JUL";
      break;
    case 8:
      monthString = "AUG";
      break;
    case 9:
      monthString = "SEP";
      break;
    case 10:
      monthString = "OCT";
      break;
    case 11:
      monthString = "NOV";
      break;
    case 12:
      monthString = "DEC";
      break;
    default:
      break;
  }

  return monthString;
}

//--------------------------------------------------------------------------
// GetYear (private)
//--------------------------------------------------------------------------
/**
 * <p>given the year as number, convert it to a 2 character year.
 * If year is out of range '??' will be returned. This routine will
 * break in the year 2100 or later ;-)
 *
 * \param year as number
 */
TString PRunDataHandler::GetYear(Int_t year)
{
  TString yearString = "??";
  Int_t yy=0;

  if (year < 2000) {
    yy = year - 1900;
    if (yy > 0)
      yearString.Form("%02d", yy);
  } else {
    yy = year - 2000;
    if ((yy >= 0) && (yy <= 99))
      yearString.Form("%02d", yy);
  }

  return yearString;
}
