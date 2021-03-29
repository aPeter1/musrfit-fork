/***************************************************************************

  PMsr2Data.cpp

  Author: Bastian M. Wojek / Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Bastian M. Wojek / Andreas Suter           *
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

// note: msr2data is on purpose implemented in a way that shows string handling can be done solely
//       using std::string, boost and related standard C++ features
//       This implies, however, occasionally strange constructs when interoperating with PMusr-classes
//       which mostly rely on ROOT's TString.

#include <cctype>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>

#include <boost/algorithm/string/trim.hpp>  // for stripping leading whitespace in std::string 
#include <boost/algorithm/string/case_conv.hpp> // for to_lower() in std::string
#include <boost/algorithm/string/split.hpp> // split strings at certain characters
using namespace boost::algorithm;

#include <boost/lexical_cast.hpp> // for atoi-replacement

#include "PMsr2Data.h"

//-------------------------------------------------------------
/**
 * <p> Constructor
 *
 * \param ext extension/suffix of the msr-files to be processed
 */
PMsr2Data::PMsr2Data(const std::string &ext) : fFileExtension(ext), fRunListFile(false), fNumGlobalParam(0), fNumSpecParam(0), fNumTempRunBlocks(0), fRunNumberDigits(4), fHeaderWritten(false)
{
  fRunVector.clear();
  fRunVectorIter = fRunVector.end();
  fRunListFileStream = nullptr;
  fSaxParser = nullptr;
  fStartupHandler = nullptr;
  fDataHandler = nullptr;
  fMsrHandler = nullptr;
}

//-------------------------------------------------------------
/**
 * <p> Destructor
 */
PMsr2Data::~PMsr2Data()
{
  fRunVector.clear();
  fRunVectorIter = fRunVector.end();
  fIndVar.clear();

  if (fRunListFileStream) {
    fRunListFileStream->close();
    delete fRunListFileStream;
    fRunListFileStream = nullptr;
  }
  if (fSaxParser) {
    delete fSaxParser;
    fSaxParser = nullptr;
  }
  if (fStartupHandler) {
    delete fStartupHandler;
    fStartupHandler = nullptr;
  }
  if (fDataHandler) {
    delete fDataHandler;
    fDataHandler = nullptr;
  }
  if (fMsrHandler) {
    delete fMsrHandler;
    fMsrHandler = nullptr;
  }
}

//-------------------------------------------------------------
/**
 * <p> Determines the number of digits used for the run number in the data file name from the first msr-file that is processed
 *     If the specified run number is the first one of the list of runs and it cannot be opened, then the rest of the given runs is checked
 *     until an existing msr-file is found
 *
 * <p><b>return:</b>
 * - 0 if the number has been determined and set successfully
 * - -1 in case the msr-file cannot be read
 * - -2 if the msr-file-number does not match the data-file-number
 * - -3 if the msr-file does not contain a RUN block
 *
 *  \param runNo run number of an msr-file
 *  \param normalMode false for global mode
 *
 */
int PMsr2Data::DetermineRunNumberDigits(unsigned int runNo, bool normalMode) const
{
  std::ostringstream strInfile;
  strInfile << runNo << fFileExtension << ".msr";
  std::ifstream *in = new std::ifstream(strInfile.str().c_str());
  if (!in->is_open()) {
    delete in;
    if (!normalMode && (runNo == *fRunVectorIter)) {
      std::string fileNameCopy(strInfile.str());
      strInfile.clear();
      strInfile.str("");
      strInfile << runNo << "+global" << fFileExtension << ".msr";
      in = new std::ifstream(strInfile.str().c_str());
      if (!in->is_open()) {
        std::cerr << std::endl << ">> msr2data: **ERROR** Neither the file " << fileNameCopy << " nor the file " << strInfile.str() << " can be opened! Please check!";
        std::cerr << std::endl;
        delete in;
        return -1;
      }
    } else if (runNo == *fRunVectorIter) { // the first run of the runlist was given - if it did not exist, try the rest of the runlist
      if (++fRunVectorIter != fRunVector.end()) {
        return DetermineRunNumberDigits(*fRunVectorIter, true);
      } else {
        std::cerr << std::endl << ">> msr2data: **ERROR** None of the given msr-files can be opened! Please check!";
        std::cerr << std::endl;
        return -1;
      }
    } else {
      std::cerr << std::endl << ">> msr2data: **ERROR** The given template " << strInfile.str() << " cannot be opened! Please check!";
      std::cerr << std::endl;
      return -1;
    }
  }

  std::ostringstream tempRunNumber;
  tempRunNumber.fill('0');
  tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  tempRunNumber.width(fRunNumberDigits);
  tempRunNumber << runNo;

  fRunNumberDigits = tempRunNumber.str().length();

  std::string line, firstOnLine;
  std::istringstream strLine;

  while (getline(*in, line)) {
    strLine.clear();
    strLine.str(line);
    strLine >> firstOnLine;
    if (!to_lower_copy(firstOnLine).compare("run")) {
      firstOnLine.clear();
      strLine >> firstOnLine;
      std::string::size_type loc = firstOnLine.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        while ( loc > 0 ) {
          if (isdigit(firstOnLine.at(--loc))) {
            ++fRunNumberDigits;
          } else {
            break;
          }
        }
        in->close();
        delete in;
        in = nullptr;
        fRunVectorIter = fRunVector.begin(); // set back the runlist-iterator which might have changed during the search for the correct file
        return 0;
      } else {
         std::cerr << std::endl << ">> msr2data: **ERROR** The first processed run file number does not match the \"file index\"!";
         std::cerr << std::endl << ">> msr2data: **ERROR** The number of digits to be used for formatting the run numbers cannot be determined!";
         std::cerr << std::endl << ">> msr2data: **ERROR** Please check the first msr-file that should be processed;";
         std::cerr << std::endl << ">> msr2data: **ERROR** this is either some template or the first existing file from the run list.";
         std::cerr << std::endl;
         in->close();
         delete in;
         in = nullptr;
         return -2;
      }
    }
  }
  std::cerr << std::endl << ">> msr2data: **ERROR** Please check the first msr-file that should be processed;";
  std::cerr << std::endl << ">> msr2data: **ERROR** this is either some template or the first file from the run list.";
  std::cerr << std::endl << ">> msr2data: **ERROR** Obviously it contains no RUN block...";
  std::cerr << std::endl;
  in->close();
  delete in;
  in = nullptr;
  return -3;
}

//-------------------------------------------------------------
/**
 * <p> Checks if all given run numbers are in the range covered by the number of digits used in the data file name
 *
 * <p><b>return:</b>
 * - 0 if everything is fine
 * - -1 if a given run number is too big
 */
int PMsr2Data::CheckRunNumbersInRange() const
{
  double max(pow(static_cast<double>(10), static_cast<int>(fRunNumberDigits)) - 1.0);
  unsigned int max_UInt;
  max > static_cast<double>(std::numeric_limits<unsigned int>::max()) ? max_UInt = std::numeric_limits<unsigned int>::max()
                                                                 : max_UInt = static_cast<unsigned int>(max);

  for (std::vector<unsigned int>::const_iterator iter(fRunVector.begin()); iter != fRunVector.end(); ++iter) {
    if (*iter > max_UInt) {
      return -1;
    }
  }
  return 0;
}

//-------------------------------------------------------------
/**
 * <p> Determines the current run number
 *
 * <p><b>return:</b>
 * - current run number
 * - 0 if all runs have been processed already
 */
unsigned int PMsr2Data::GetPresentRun() const
{
  if (fRunVectorIter != fRunVector.end())
    return *fRunVectorIter;
  else
    return 0;
}

//-------------------------------------------------------------
/**
 * <p> Initialization of the internal list of runs using a single run number
 *
 * <p><b>return:</b>
 * - 0 if the run number is valid
 * - 1 otherwise
 *
 * \param runNo run number
 */
int PMsr2Data::SetRunNumbers(unsigned int runNo)
{
  if (runNo < 1)
    return 1;

  fRunVector.clear();
  fRunVector.push_back(runNo);
  fRunVectorIter = fRunVector.begin();

  return 0;
}

//-------------------------------------------------------------
/**
 * <p> Initialization of the internal list of runs using first and last run numbers
 *
 * <p><b>return:</b>
 * - 0 if the run numbers are valid
 * - 1 otherwise
 *
 * \param runNoStart first run number
 * \param runNoEnd last run number
 */
int PMsr2Data::SetRunNumbers(unsigned int runNoStart, unsigned int runNoEnd)
{
  if ((runNoStart < 1) || (runNoEnd < 1))
    return 1;

  fRunVector.clear();
  if (runNoStart <= runNoEnd) {
    for (unsigned int i(runNoStart); i<=runNoEnd; ++i)
      fRunVector.push_back(i);
  } else {
    for (unsigned int i(runNoStart); i>=runNoEnd; --i)
      fRunVector.push_back(i);
  }
  fRunVectorIter = fRunVector.begin();

  return 0;
}

//-------------------------------------------------------------
/**
 * <p> Initialization of the internal list of runs using explicitly specified run numbers
 *
 * <p><b>return:</b>
 * - -1 if the vector is empty
 * - 0 if all run numbers are valid
 * - 1 otherwise
 *
 * \param runListVector vector containing the run numbers to be processed
 */
int PMsr2Data::SetRunNumbers(const std::vector<unsigned int> &runListVector)
{
  if (runListVector.empty())
    return -1;

  for (std::vector<unsigned int>::const_iterator iter(runListVector.begin()); iter!=runListVector.end(); ++iter) {
    if (*iter < 1)
      return 1;
  }

  fRunVector = runListVector;
  fRunVectorIter = fRunVector.begin();

  return 0;
}

//-------------------------------------------------------------
/**
 * <p> Initialization of the internal list of runs using a run list file
 *
 * <p><b>return:</b>
 * - -1 if the run list file cannot be opened
 * - 0 if all run numbers are valid
 * - 1 otherwise
 *
 * \param runListFile name of run list file
 */
int PMsr2Data::SetRunNumbers(const std::string &runListFile)
{
  fRunVector.clear();

  std::ifstream in(runListFile.c_str());
  if (!in) {
    std::cerr << std::endl << ">> msr2data: **ERROR** The runlist file " << runListFile << " cannot be opened! Please check!";
    std::cerr << std::endl;
    return -1;
  }

  std::string line, indvar;
  std::istringstream strLine;
  std::vector<std::string> splitVec;
  unsigned int cntr(0);
  unsigned int runNo;

  while (getline(in, line)) {
    trim(line);
    if (line.empty())
      continue;
    else if (line.at(0) == '#')
      continue;
    else
      ++cntr;

    split( splitVec, line, is_any_of("#") ); // split the string if any comments appear on the line

    if (cntr == 1) { // Read in the names of the independent variables in the runlist file
      strLine.clear();
      strLine.str(splitVec[0]);
      strLine >> indvar; // "RUN"
      if (to_lower_copy(indvar).compare("run")) {
        std::cerr << std::endl << ">> msr2data: **ERROR** The format of the runlist file " << runListFile << " is not correct! Please check!";
        std::cerr << std::endl;
      }
      while (strLine >> indvar)
        fIndVar.push_back(indvar);
    }

    if (cntr > 1) {
      strLine.clear();
      strLine.str(splitVec[0]);
      strLine >> runNo;
      if (runNo < 1)
        return 1;
      fRunVector.push_back(runNo);
    }

    splitVec.clear();
  }

  in.close();
  fRunVectorIter = fRunVector.begin();
  fRunListFile = true;
  fRunListFileStream = new std::ifstream(runListFile.c_str());
  return 0;
}

//-------------------------------------------------------------
/**
 * <p> Parse the musrfit startup xml file
 *
 * <p><b>return:</b>
 * - 0 if everything went fine
 * - return value of the parseXmlFile function otherwise
 */
int PMsr2Data::ParseXmlStartupFile()
{
  int status;
  fSaxParser = new TSAXParser();
  fStartupHandler = new PStartupHandler();
  std::string startup_path_name(fStartupHandler->GetStartupFilePath().Data());
  fSaxParser->ConnectToHandler("PStartupHandler", fStartupHandler);
  //status = fSaxParser->ParseFile(startup_path_name.c_str());
  // parsing the file as above seems to lead to problems in certain environments;
  // use the parseXmlFile function instead (see PStartupHandler.cpp for the definition)
  status = parseXmlFile(fSaxParser, startup_path_name.c_str());
  // check for parse errors
  if (status) { // error
    std::cerr << std::endl << ">> msr2data: **WARNING** Reading/parsing musrfit_startup.xml failed." << std::endl;
    // clean up
    if (fSaxParser) {
      delete fSaxParser;
      fSaxParser = nullptr;
    }
    if (fStartupHandler) {
      delete fStartupHandler;
      fStartupHandler = nullptr;
    }
  }
  return status;
}

//-------------------------------------------------------------
/**
 * <p> Read in a msr-file into the default structure
 *
 * <p><b>return:</b>
 * - PMUSR_SUCCESS if everything is OK
 * - return value of the ReadMsrFile-method otherwise
 *
 * \param infile name of the msr-file to be read
 */
int PMsr2Data::ReadMsrFile(const std::string &infile) const
{
  int status;
  fMsrHandler = new PMsrHandler(infile.c_str());
  status = fMsrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        std::cerr << std::endl << ">> msr2data: **ERROR** Could not find " << infile << std::endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        std::cerr << std::endl << ">> msr2data: **SYNTAX ERROR** in file " << infile << ", full stop here." << std::endl;
        break;
      default:
        std::cerr << std::endl << ">> msr2data: **UNKOWN ERROR** when trying to read the msr-file" << std::endl;
        break;
    }
  }
  return status;
}

//-------------------------------------------------------------
/**
 * <p> Read in the single run msr-file corresponding to the position in the run-vector
 *     into a secondary msr-handler different from the class member
 *
 * <p><b>return:</b>
 * - pointer to the secondary msr-handler or 0 in case of an error
 *
 */
PMsrHandler* PMsr2Data::GetSingleRunMsrFile() const
{
  std::ostringstream singleMsrInFile;
  singleMsrInFile << *fRunVectorIter << "-OneRunFit" << fFileExtension << ".msr";
  PMsrHandler *singleRunMsrFile = new PMsrHandler(singleMsrInFile.str().c_str());

  int status = singleRunMsrFile->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        std::cerr << std::endl << ">> msr2data: **ERROR** Could not find " << singleMsrInFile.str() << std::endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        std::cerr << std::endl << ">> msr2data: **SYNTAX ERROR** in file " << singleMsrInFile.str() << ", full stop here." << std::endl;
        break;
      default:
        std::cerr << std::endl << ">> msr2data: **UNKOWN ERROR** when trying to read the msr-file" << std::endl;
        break;
    }
    return nullptr;
  }
  return singleRunMsrFile;
}

//-------------------------------------------------------------
/**
 * <p> Read in a run data-file
 *
 * <p><b>return:</b>
 * - 0 if everything is OK
 * - 1 otherwise
 */
int PMsr2Data::ReadRunDataFile()
{
  if (fStartupHandler)
    fDataHandler = new PRunDataHandler(fMsrHandler, fStartupHandler->GetDataPathList());
  else
    fDataHandler = new PRunDataHandler(fMsrHandler);

  fDataHandler->ReadData();

  bool success = fDataHandler->IsAllDataAvailable();
  if (!success) {
    std::cerr << std::endl << ">> msr2data: **WARNING** Could not read all data files, will continue without the data file information..." << std::endl;
    delete fDataHandler;
    fDataHandler = nullptr;
    return 1;
  }
  return 0;
}

//-------------------------------------------------------------
/**
 * <p> Generate a new single-run msr file from a template
 *
 * <p><b>return:</b>
 * - true if everything is OK
 * - false otherwise
 *
 * \param tempRun template run number
 * \param calledFromGlobalMode tag specifying if the routine is called as part of the global msr-file generation
 */
bool PMsr2Data::PrepareNewInputFile(unsigned int tempRun, bool calledFromGlobalMode) const
{
  if (fRunVectorIter == fRunVector.end())
    return false;

  if (*fRunVectorIter == tempRun)
    return true;

  std::string globalTag("");
  if (calledFromGlobalMode)
    globalTag = "-OneRunFit";

  std::ostringstream strInfile;
  strInfile << tempRun << globalTag << fFileExtension << ".msr";

  std::ifstream in(strInfile.str().c_str());
  if (!in) {
    std::cerr << std::endl << ">> msr2data: **ERROR** The template msr-file " << strInfile.str() << " cannot be opened! Please check!";
    std::cerr << std::endl;
    return false;
  }
  std::ostringstream strOutfile;
  strOutfile << *fRunVectorIter << globalTag << fFileExtension << ".msr";
  std::ofstream out(strOutfile.str().c_str());
  if (!out) {
    std::cerr << std::endl << ">> msr2data: **ERROR** The new msr file " << strOutfile.str() << " cannot be opened! Please check!";
    std::cerr << std::endl;
    return false;
  }

  std::ostringstream tempRunNumber;
  tempRunNumber.fill('0');
  tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  tempRunNumber.width(fRunNumberDigits);
  tempRunNumber << tempRun;

  std::ostringstream newRunNumber;
  newRunNumber.fill('0');
  newRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  newRunNumber.width(fRunNumberDigits);
  newRunNumber << *fRunVectorIter;

  std::cout << std::endl << ">> msr2data: **INFO** Generating new input msr file " << strOutfile.str() << std::endl;

  std::string line, firstOnLine;
  std::istringstream strLine;
  unsigned int emptyLineCounter(0);

  // write the run number in the TITLE line of the msr-file (is overwritten later by musrfit if called with the -t option)
  getline(in, line);
  out << *fRunVectorIter << std::endl;

  while (getline(in, line)) {
    if (line.empty()) {
      if (++emptyLineCounter < 3) // allow only two successive empty lines in the input files (just for convenience)
        out << std::endl;
      continue;
    } else
      emptyLineCounter = 0;

    if (!line.compare("SET BATCH") || !line.compare("END RETURN"))
      continue;

    strLine.clear();
    strLine.str(line);
    strLine >> firstOnLine;
    if (!to_lower_copy(firstOnLine).compare("run")) {
      strLine >> firstOnLine;
      std::string::size_type loc = firstOnLine.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        firstOnLine.replace(loc, fRunNumberDigits, newRunNumber.str());
      } else {
        std::cerr << std::endl << ">> msr2data: **WARNING** The template run file number does not match the \"file index\"";
        std::cerr << std::endl << ">> msr2data: **WARNING** Unexpected things will happen... (for sure)";
        std::cerr << std::endl;
      }
      out << "RUN " << firstOnLine;
      while (strLine >> firstOnLine)
        out << " " << firstOnLine;
      out << std::endl;
    } else if (!to_lower_copy(firstOnLine).compare("chisq") || !to_lower_copy(firstOnLine).compare("maxlh")) {
      out << "*** FIT DID NOT CONVERGE ***" << std::endl;
      break;
    } else {
      out << line << std::endl;
    }
  }
  in.close();
  out.close();
  return true;
}


//-------------------------------------------------------------
/**
 * <p> Function defining the parameter order for the sort algorithm when a global msr-file is generated
 *
 * <p><b>return:</b>
 * - true if global parameters are listed before run specific ones
 * - true if within the global parameters par1 appears before par2
 * - true if within the run specific parameters par1 appears before par2
 * - false otherwise
 *
 * \param par1 fit parameter
 * \param par2 fit parameter
 */
bool compare_parameters(const PMsrParamStructure &par1, const PMsrParamStructure &par2)
{
  if (par1.fIsGlobal) {
    if (par2.fIsGlobal && (par2.fNo > par1.fNo))
      return true;
    else if (par2.fIsGlobal && (par2.fNo < par1.fNo))
      return false;
    else
      return true;
  }
  else if (par2.fIsGlobal)
    return false;
  else if (par2.fNo > par1.fNo)
    return true;
  else
    return false;
}

//-------------------------------------------------------------
/**
 * <p> Generate a new global msr-file from a template
 *
 * <p><b>return:</b>
 * - true if everything is OK
 * - false otherwise
 *
 * \param tempRun template run number
 * \param msrOutFile name of the global msr-file to be written
 * \param globalPlus tag of the global mode to be used:
 *                   - 0: simple global mode with the same template starting parameters for each run
 *                   - 1: global mode including single run fits and data extraction for the global msr-file (same template for all runs)
 *                   - 2: global mode including single run fits and data extraction for the global msr-file (successive template generation)
 */
bool PMsr2Data::PrepareGlobalInputFile(unsigned int tempRun, const std::string &msrOutFile, unsigned int globalPlus) const
{
  const TString alpha("alpha"), beta("beta"), norm("norm"), bkgfit("bkgfit"), lifetime("lifetime");

  std::ostringstream strInfile;
  strInfile << tempRun << fFileExtension << ".msr";

  // read template msr-file
  int status(ReadMsrFile(strInfile.str()));
  if (status != PMUSR_SUCCESS)
    return false;

  // define two heavily used variables
  PMsrParamList *msrParamList(fMsrHandler->GetMsrParamList());
  PMsrRunList *msrRunList(fMsrHandler->GetMsrRunList());

  // check how many RUN blocks are in the template msr-file
  fNumTempRunBlocks = msrRunList->size();

  // check that the RUN block run numbers match the template run number
  std::ostringstream tempRunNumber;
  tempRunNumber.fill('0');
  tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  tempRunNumber.width(fRunNumberDigits);
  tempRunNumber << tempRun;

  std::string tempRunName;
  for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
    tempRunName = msrRunList->at(i).GetRunName()->Data();
    std::string::size_type loc = tempRunName.rfind(tempRunNumber.str());
    if ( loc == std::string::npos ) {
      std::cerr << std::endl << ">> msr2data: **ERROR** A template run file number does not match the \"file index\"";
      std::cerr << std::endl << ">> msr2data: **ERROR** Please check the template file!";
      std::cerr << std::endl;
      return false;
    }
  }

  std::cout << std::endl << ">> msr2data: **INFO** Generating new global input msr file " << msrOutFile << std::endl;

  // set some title for the msr-file
  std::ostringstream titleStream;
  titleStream << "Global msr file for the runs: ";
  while (fRunVectorIter != fRunVector.end()) {
    titleStream << *fRunVectorIter << " ";
    ++fRunVectorIter;
  }
  fRunVectorIter = fRunVector.begin();
  fMsrHandler->SetMsrTitle(TString(titleStream.str()));
  titleStream.clear();
  titleStream.str("");

  // search parameter list for run-specific parameters - the rest is assumed to be global
  std::string tempParamName;
  for (unsigned int i(0); i < msrParamList->size(); ++i) {
    tempParamName = msrParamList->at(i).fName.Data();
    std::string::size_type loc = tempParamName.rfind(tempRunNumber.str());
    if ((tempParamName.length() > fRunNumberDigits) && (loc == tempParamName.length() - fRunNumberDigits)) {
      msrParamList->at(i).fIsGlobal = false;
      ++fNumSpecParam;
    } else {
      msrParamList->at(i).fIsGlobal = true;
      ++fNumGlobalParam;
    }
    // std::cout << "debug> " << msrParamList->at(i).fNo << ": " << msrParamList->at(i).fName.Data() << " is global: " << msrParamList->at(i).fIsGlobal << std::endl;
  }

  // check if parameters have been sorted correctly from the beginning
  bool wasSorted(true);
  for (unsigned int i(0); i < fNumGlobalParam; ++i) {
    if(!msrParamList->at(i).fIsGlobal) {
      wasSorted = false;
      break;
    }
  }

  int tempPar;
  std::vector<int> *tempMap;
  PMsrLines *tempLines;
  std::vector<std::string> tempVec;
  std::string line, tempString;

  if (!wasSorted) {
    // Sort the parameters: global ones first, then run-specific
    sort(msrParamList->begin(), msrParamList->end(), compare_parameters);

    // Change the parameter numbers in all blocks according to the new order

    // THEORY block
    tempLines = fMsrHandler->GetMsrTheory();
    bool mapExists(false);
    for (unsigned int i(0); i < tempLines->size(); ++i) {
      line = (*tempLines)[i].fLine.Data();
      split( tempVec, line, is_any_of(" \t"), token_compress_on ); // split the theory line at spaces
      for (unsigned int j(1); j < tempVec.size(); ++j) {
        try {
          tempPar = boost::lexical_cast<unsigned int>(tempVec[j]);
          // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
          for (unsigned int k(0); k < msrParamList->size(); ++k) {
            if (tempPar == msrParamList->at(k).fNo) {
              if (msrParamList->at(k).fIsGlobal) {
                tempVec[j] = boost::lexical_cast<std::string>(k + 1);
              } else {
                std::cerr << std::endl << ">> msr2data: **WARNING** The parameter " << msrParamList->at(k).fName.Data() \
                             << " is recognized as run specific!";
                std::cerr << std::endl << ">> msr2data: **WARNING** Still it appears directly (un-mapped) in the template THEORY block.";
                std::cerr << std::endl << ">> msr2data: **WARNING** The THEORY block entry will be substituted by a mapped parameter.";
                std::cerr << std::endl << ">> msr2data: **WARNING** In case, this is not what has been intended, please review the new msr-file!";
                std::cerr << std::endl;

                unsigned int l(0);
                tempMap = msrRunList->at(0).GetMap();
                while (l < tempMap->size()) {
                  if ((*tempMap)[l] == tempPar) {
                    mapExists = true;
                    for (unsigned int m(1); m < fNumTempRunBlocks; ++m) {
                      if (msrRunList->at(m).GetMap()->at(l) != tempPar) {
                        mapExists = false;
                        break;
                      }
                    }
                  }
                  if (mapExists) {
                    break;
                  } else {
                    ++l;
                  }
                }

                if (mapExists) {
                  tempVec[j] = "map";
                  tempVec[j].append(boost::lexical_cast<std::string>(l + 1));
                  mapExists = false;
                } else { // look for the first not used map entry
                  for (l = 0; l < tempMap->size(); ++l) {
                    if (!(*tempMap)[l]) {
                      break;
                    }
                  }
                  for (unsigned int m(0); m < fNumTempRunBlocks; ++m) {
                    msrRunList->at(m).SetMap(tempPar, l);
                  }
                  tempVec[j] = "map";
                  tempVec[j].append(boost::lexical_cast<std::string>(l + 1));
                }
              }
              break;
            }
          }
        }
        catch(boost::bad_lexical_cast &) {
          // in case the cast does not work: do nothing - this means the entry is not a simple parameter
        }
      }
      // replace the old theory line by the new one
      (*tempLines)[i].fLine.Clear();
      for (unsigned int j(0); j < tempVec.size(); ++j) {
        (*tempLines)[i].fLine += TString(tempVec[j]) + TString(" ");
      }
      tempVec.clear();
    }

    // FUNCTIONS block
    std::string::size_type pos(0);
    tempLines = fMsrHandler->GetMsrFunctions();
    for (unsigned int i(0); i < tempLines->size(); ++i) {
      line = (*tempLines)[i].fLine.Data();
      split( tempVec, line, is_any_of(" ()+-*/=\t,") ); // split the function line at spaces and some characters that might be juxtaposed to the parameters
      for (unsigned int j(1); j < tempVec.size(); ++j) {
        if (!tempVec[j].substr(0,3).compare("par")) {
          try {
            tempPar = boost::lexical_cast<unsigned int>(tempVec[j].substr(3));
            // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
            for (unsigned int k(0); k < msrParamList->size(); ++k) {
              if (tempPar == msrParamList->at(k).fNo) {
                if (msrParamList->at(k).fIsGlobal) {
                  pos = line.find(tempVec[j], pos);
                  if ( pos != std::string::npos ) {
                    tempString = "par";
                    tempString.append(boost::lexical_cast<std::string>(k + 1));
                    line.replace(pos, tempVec[j].length(), tempString);
                  } else {
                    std::cerr << std::endl << ">> msr2data: **ERROR** A previously identified parameter in the FUNCTIONS block is not found any more!";
                    std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
                    std::cerr << std::endl;
                  }
                } else {
                  std::cerr << std::endl << ">> msr2data: **WARNING** The parameter " << msrParamList->at(k).fName.Data() \
                               << " is recognized as run specific!";
                  std::cerr << std::endl << ">> msr2data: **WARNING** Still it appears directly (un-mapped) in the template FUNCTIONS block.";
                  std::cerr << std::endl << ">> msr2data: **WARNING** The FUNCTIONS block entry will be substituted by a mapped parameter.";
                  std::cerr << std::endl << ">> msr2data: **WARNING** In case, this is not what has been intended, please review the new msr-file!";
                  std::cerr << std::endl;

                  unsigned int l(0);
                  tempMap = msrRunList->at(0).GetMap();
                  while (l < tempMap->size()) {
                    if ((*tempMap)[l] == tempPar) {
                      mapExists = true;
                      for (unsigned int m(1); m < fNumTempRunBlocks; ++m) {
                        if (msrRunList->at(m).GetMap()->at(l) != tempPar) {
                          mapExists = false;
                          break;
                        }
                      }
                    }
                    if (mapExists) {
                      break;
                    } else {
                      ++l;
                    }
                  }

                  if (!mapExists) {
                    for (l = 0; l < tempMap->size(); ++l) {
                      if (!(*tempMap)[l]) {
                        break;
                      }
                    }
                    for (unsigned int m(0); m < fNumTempRunBlocks; ++m) {
                      msrRunList->at(m).SetMap(tempPar, l);
                    }
                  }
                  pos = line.find(tempVec[j], pos);
                  if ( pos != std::string::npos ) {
                    tempString = "map";
                    tempString.append(boost::lexical_cast<std::string>(l + 1));
                    line.replace(pos, tempVec[j].length(), tempString);
                  } else {
                    std::cerr << std::endl << ">> msr2data: **ERROR** A previously identified parameter in the FUNCTIONS block is not found any more!";
                    std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
                    std::cerr << std::endl;
                  }
                  mapExists = false;
                }
                break;
              }
            }
          }
          catch(boost::bad_lexical_cast &) {
            std::cerr << std::endl << ">> msr2data: **ERROR** Something is wrong with the parameters used in the FUNCTIONS block!";
            std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - function parsing should have failed earlier!";
            std::cerr << std::endl;
            return false;
          }
        }
        ++pos;
      }
      // replace the old function line by the new one
      (*tempLines)[i].fLine = TString(line);
      tempVec.clear();
      pos = 0;
    }

    // RUN blocks
    // substitute the old parameter numbers by the new ones

    unsigned int l(0);

    // look in the following order through the RUN-blocks: norm, bkg-fit, alpha, beta, lifetime, maps
    for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
      tempPar = msrRunList->at(i).GetNormParamNo();
      if (tempPar > 0) {
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (l = 0; l < msrParamList->size(); ++l) {
          if (tempPar == msrParamList->at(l).fNo) {
            msrRunList->at(i).SetNormParamNo(l + 1);
            break;
          }
        }
        if (l > msrParamList->size()) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The norm parameter specified in RUN block " << i + 1 << " does not exist!";
          std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
          std::cerr << std::endl;
          return false;
        }
      }
      tempPar = msrRunList->at(i).GetBkgFitParamNo();
      if (tempPar > 0) {
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (l = 0; l < msrParamList->size(); ++l) {
          if (tempPar == msrParamList->at(l).fNo) {
            msrRunList->at(i).SetBkgFitParamNo(l + 1);
            break;
          }
        }
        if (l > msrParamList->size()) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The backgr.fit parameter specified in RUN block " << i + 1 << " does not exist!";
          std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
          std::cerr << std::endl;
          return false;
        }
      }
      tempPar = msrRunList->at(i).GetAlphaParamNo();
      if (tempPar > 0) {
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (l = 0; l < msrParamList->size(); ++l) {
          if (tempPar == msrParamList->at(l).fNo) {
            msrRunList->at(i).SetAlphaParamNo(l + 1);
            break;
          }
        }
        if (l > msrParamList->size()) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The alpha parameter specified in RUN block " << i + 1 << " does not exist!";
          std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
          std::cerr << std::endl;
          return false;
        }
      }
      tempPar = msrRunList->at(i).GetBetaParamNo();
      if (tempPar > 0) {
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (l = 0; l < msrParamList->size(); ++l) {
          if (tempPar == msrParamList->at(l).fNo) {
            msrRunList->at(i).SetBetaParamNo(l + 1);
            break;
          }
        }
        if (l > msrParamList->size()) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The beta parameter specified in RUN block " << i + 1 << " does not exist!";
          std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
          std::cerr << std::endl;
          return false;
        }
      }
      tempPar = msrRunList->at(i).GetLifetimeParamNo();
      if (tempPar > 0) {
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (l = 0; l < msrParamList->size(); ++l) {
          if (tempPar == msrParamList->at(l).fNo) {
            msrRunList->at(i).SetLifetimeParamNo(l + 1);
            break;
          }
        }
        if (l > msrParamList->size()) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The lifetime parameter specified in RUN block " << i + 1 << " does not exist!";
          std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
          std::cerr << std::endl;
          return false;
        }
      }
      tempMap = msrRunList->at(i).GetMap();
      for (unsigned int j(0); j < tempMap->size(); ++j) {
        tempPar = (*tempMap)[j];
        if (tempPar > 0) {
          // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
          for (l = 0; l < msrParamList->size(); ++l) {
            if (tempPar == msrParamList->at(l).fNo) {
              msrRunList->at(i).SetMap(l + 1, j);
              break;
            }
          }
        }
      }
      tempMap = nullptr;
    }

    // FOURIER block - in case a parameter is used for the phase
    if (fMsrHandler->GetMsrFourierList()->fPhaseParamNo.size() > 0) {
      // go through the whole parameter list ...
      for (unsigned int k(0); k < msrParamList->size(); ++k) {
        if (tempPar == msrParamList->at(k).fNo) {
          fMsrHandler->GetMsrFourierList()->fPhaseParamNo.push_back(k + 1);
          break;
        }
      }
    }

    // PLOT block - in case a parameter is used for the RRFphase
    for (unsigned int i(0); i < fMsrHandler->GetMsrPlotList()->size(); ++i) {
      tempPar = fMsrHandler->GetMsrPlotList()->at(i).fRRFPhaseParamNo;
      if (tempPar > 0) {
        // go through the whole parameter list ...
        for (unsigned int k(0); k < msrParamList->size(); ++k) {
          if (tempPar == msrParamList->at(k).fNo) {
            fMsrHandler->GetMsrPlotList()->at(i).fRRFPhaseParamNo = k + 1;
            break;
          }
        }
      }
    }

    // finally fix the PARAMETER block after the reorganization
    for (l = 0; l < msrParamList->size(); ++l) {
      msrParamList->at(l).fNo = l + 1;
    }
  } else { // if the parameters were sorted just check the THEORY and FUNCTIONS blocks for run specific parameters
    bool lineChanged(false), mapExists(false);
    // THEORY block
    tempLines = fMsrHandler->GetMsrTheory();
    for (unsigned int i(0); i < tempLines->size(); ++i) {
      line = (*tempLines)[i].fLine.Data();
      split( tempVec, line, is_any_of(" \t"), token_compress_on ); // split the theory line at spaces

      for (unsigned int j(1); j < tempVec.size(); ++j) {
        try {
          tempPar = boost::lexical_cast<unsigned int>(tempVec[j]);
          if (!msrParamList->at(tempPar - 1).fIsGlobal) {
            std::cerr << std::endl << ">> msr2data: **WARNING** The parameter " << msrParamList->at(tempPar - 1).fName.Data() \
                         << " is recognized as run specific!";
            std::cerr << std::endl << ">> msr2data: **WARNING** Still it appears directly (un-mapped) in the template THEORY block.";
            std::cerr << std::endl << ">> msr2data: **WARNING** The THEORY block entry will be substituted by a mapped parameter.";
            std::cerr << std::endl << ">> msr2data: **WARNING** In case, this is not what has been intended, please review the new msr-file!";
            std::cerr << std::endl;

            unsigned int l(0);
            tempMap = msrRunList->at(0).GetMap();
            while (l < tempMap->size()) {
              if ((*tempMap)[l] == tempPar) {
                mapExists = true;
                for (unsigned int m(1); m < fNumTempRunBlocks; ++m) {
                  if (msrRunList->at(m).GetMap()->at(l) != tempPar) {
                    mapExists = false;
                    break;
                  }
                }
              }
              if (mapExists) {
                break;
              } else {
                ++l;
              }
            }

            if (mapExists) {
              tempVec[j] = "map";
              tempVec[j].append(boost::lexical_cast<std::string>(l + 1));
              lineChanged = true;
              mapExists = false;
            } else {
              for (l = 0; l < tempMap->size(); ++l) {
                if (!(*tempMap)[l]) {
                  break;
                }
              }
              for (unsigned int m(0); m < fNumTempRunBlocks; ++m) {
                msrRunList->at(m).SetMap(tempPar, l);
              }
              tempVec[j] = "map";
              tempVec[j].append(boost::lexical_cast<std::string>(l + 1));
              lineChanged = true;
            }
          }
        }
        catch(boost::bad_lexical_cast &) {
          // in case the cast does not work: do nothing - this means the entry is not a simple parameter
        }
      }
      // replace the old theory line by the new one
      if (lineChanged) {
        (*tempLines)[i].fLine.Clear();
        for (unsigned int j(0); j < tempVec.size(); ++j) {
          (*tempLines)[i].fLine += TString(tempVec[j]) + TString(" ");
        }
        lineChanged = false;
      }
      tempVec.clear();
    }

    // FUNCTIONS block
    tempLines = fMsrHandler->GetMsrFunctions();
    std::string::size_type pos(0);
    for (unsigned int i(0); i < tempLines->size(); ++i) {
      line = (*tempLines)[i].fLine.Data();
      split( tempVec, line, is_any_of(" ()+-*/=\t,") ); // split the function line at spaces and some characters that might be juxtaposed to the parameters
      for (unsigned int j(1); j < tempVec.size(); ++j) {
        if (!tempVec[j].substr(0,3).compare("par")) {
          try {
            tempPar = boost::lexical_cast<unsigned int>(tempVec[j].substr(3));

            if (!msrParamList->at(tempPar - 1).fIsGlobal) {

              std::cerr << std::endl << ">> msr2data: **WARNING** The parameter " << msrParamList->at(tempPar - 1).fName.Data() \
                           << " is recognized as run specific!";
              std::cerr << std::endl << ">> msr2data: **WARNING** Still it appears directly (un-mapped) in the template FUNCTIONS block.";
              std::cerr << std::endl << ">> msr2data: **WARNING** The FUNCTIONS block entry will be substituted by a mapped parameter.";
              std::cerr << std::endl << ">> msr2data: **WARNING** In case, this is not what has been intended, please review the new msr-file!";
              std::cerr << std::endl;

              unsigned int l(0);
              tempMap = msrRunList->at(0).GetMap();
              while (l < tempMap->size()) {
                if ((*tempMap)[l] == tempPar) {
                  mapExists = true;
                  for (unsigned int m(1); m < fNumTempRunBlocks; ++m) {
                    if (msrRunList->at(m).GetMap()->at(l) != tempPar) {
                      mapExists = false;
                      break;
                    }
                  }
                }
                if (mapExists) {
                  break;
                } else {
                  ++l;
                }
              }

              if (!mapExists) {
                for (l = 0; l < tempMap->size(); ++l) {
                  if (!(*tempMap)[l]) {
                    break;
                  }
                }
                for (unsigned int m(0); m < fNumTempRunBlocks; ++m) {
                  msrRunList->at(m).SetMap(tempPar, l);
                }
              }
              pos = line.find(tempVec[j], pos);
              if ( pos != std::string::npos ) {
                tempString = "map";
                tempString.append(boost::lexical_cast<std::string>(l + 1));
                line.replace(pos, tempVec[j].length(), tempString);
              } else {
                std::cerr << std::endl << ">> msr2data: **ERROR** A previously identified parameter in the FUNCTIONS block is not found any more!";
                std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
                std::cerr << std::endl;
              }
              mapExists = false;
              lineChanged = true;
            }
          }

          catch(boost::bad_lexical_cast &) {
            std::cerr << std::endl << ">> msr2data: **ERROR** Something is wrong with the parameters used in the FUNCTIONS block!";
            std::cerr << std::endl << ">> msr2data: **ERROR** Please report a bug - function parsing should have failed earlier!";
            std::cerr << std::endl;
            return false;
          }
        }
        ++pos;
      }
      // replace the old function line by the new one
      if (lineChanged) {
        (*tempLines)[i].fLine = TString(line);
        lineChanged = false;
      }
      tempVec.clear();
      pos = 0;
    }
  }

  // go once more trough the template RUN blocks and look for common parameters
  // look in the following order through the RUN-blocks: norm, bkg-fit, alpha, beta, lifetime, maps
  for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
    tempPar = msrRunList->at(i).GetNormParamNo();
    if ((tempPar > 0) && (tempPar < MSR_PARAM_FUN_OFFSET)) { // if the norm-parameter is given by a function we do not have to care about it here!
      if (msrParamList->at(tempPar-1).fIsGlobal) { // norm of RUN block i is global
        msrRunList->at(i).SetParGlobal(norm, 1);
      } else {
        msrRunList->at(i).SetParGlobal(norm, 0);
      }
    }
    tempPar = msrRunList->at(i).GetBkgFitParamNo();
    if (tempPar > 0) {
      if (msrParamList->at(tempPar-1).fIsGlobal) { // bkg-fit of RUN block i is global
        msrRunList->at(i).SetParGlobal(bkgfit, 1);
      } else {
        msrRunList->at(i).SetParGlobal(bkgfit, 0);
      }
    }
    tempPar = msrRunList->at(i).GetAlphaParamNo();
    if (tempPar > 0) {
      if (msrParamList->at(tempPar-1).fIsGlobal) { // alpha of RUN block i is global
        msrRunList->at(i).SetParGlobal(alpha, 1);
      } else {
        msrRunList->at(i).SetParGlobal(alpha, 0);
      }
    }
    tempPar = msrRunList->at(i).GetBetaParamNo();
    if (tempPar > 0) {
      if (msrParamList->at(tempPar-1).fIsGlobal) { // beta of RUN block i is global
        msrRunList->at(i).SetParGlobal(beta, 1);
      } else {
        msrRunList->at(i).SetParGlobal(beta, 0);
      }
    }
    tempPar = msrRunList->at(i).GetLifetimeParamNo();
    if (tempPar > 0) {
      if (msrParamList->at(tempPar-1).fIsGlobal) { // lifetime of RUN block i is global
        msrRunList->at(i).SetParGlobal(lifetime, 1);
      } else {
        msrRunList->at(i).SetParGlobal(lifetime, 0);
      }
    }
    tempMap = msrRunList->at(i).GetMap();
    for (unsigned int j(0); j < tempMap->size(); ++j) {
      tempPar = (*tempMap)[j];
      if (tempPar > 0) {
        if (msrParamList->at(tempPar-1).fIsGlobal) { // map j of RUN block i is global
          msrRunList->at(i).SetMapGlobal(j, 1);
        } else {
          msrRunList->at(i).SetMapGlobal(j, 0);
        }
      }
    }
    tempMap = nullptr;
  }

  // if the global+ option is specified, generate new single run msr-files and fit them
  // to obtain better starting parameter values for the global file

  if (globalPlus) {
    // for the first file (or if the !-option is given), prepare a sorted input from the internal data-structure
    // if chain-fitting is used, the successive files are copied from the first
    unsigned int oldTempRun(0);
    bool firstrun(true);
    bool success(true);

    while(fRunVectorIter != fRunVector.end()) {
      if (firstrun || (globalPlus == 1))
        success = PrepareNewSortedInputFile(tempRun);
      else
        success = PrepareNewInputFile(oldTempRun, true);
      if (firstrun)
        firstrun = false;
      oldTempRun = *fRunVectorIter;

      if (!success) {
        std::cout << std::endl << ">> msr2data: **ERROR** Input file generation has not been successful! Quitting..." << std::endl;
        return false;
      }

      // and do the fitting
      // check if MUSRFITPATH is set, if not issue a warning
      std::string path("");
      bool pathSet(false);
      char *pathPtr(getenv("MUSRFITPATH"));
      if (pathPtr) {
        path = boost::lexical_cast<std::string>(pathPtr);
        if (!path.empty()) {
          pathSet = true;
          path.append("/");
        }
      }
      if (!pathSet) {
        std::cerr << std::endl << ">> msr2data: **WARNING** The MUSRFITPATH environment variable is not set!";
        std::cerr << std::endl << ">> msr2data: **WARNING** Please set it or at least ensure that musrfit can be found on the PATH!" << std::endl;
      }
      std::ostringstream oss;
      oss << path << "musrfit" << " " << *fRunVectorIter << "-OneRunFit" << fFileExtension << ".msr";
      std::cout << std::endl << ">> msr2data: **INFO** Calling " << oss.str() << std::endl;
      system(oss.str().c_str());

      ++fRunVectorIter;
    }
    // set back the iterator to the first run
    fRunVectorIter = fRunVector.begin();

    std::cout << std::endl << ">> msr2data: **INFO** Continuing with the generation of the global input msr file " << msrOutFile << std::endl;
  }


  // now go through the specified runs and add run-specific parameters and RUN blocks

  PMsrHandler *singleRunMsrFile;
  singleRunMsrFile = nullptr;

  std::map<UInt_t, TString> commentsP, commentsR;
  TString tempTstr;
  std::ostringstream newRunNumber;
  newRunNumber.fill('0');
  newRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  newRunNumber.width(fRunNumberDigits);
  newRunNumber << *fRunVectorIter;

  //std::cout << "Number of run specific parameters: " << fNumSpecParam << std::endl;

  if (newRunNumber.str().compare(tempRunNumber.str())) { // first run number does not match the template run number
    // in global+ mode, read in the single run msr-file of the corresponding run
    if (globalPlus)
      singleRunMsrFile = GetSingleRunMsrFile();

    // substitute the template run-numbers in the parameter names
    for (unsigned int l(fNumGlobalParam); l < msrParamList->size(); ++l) {
      tempParamName = msrParamList->at(l).fName.Data();
      std::string::size_type loc = tempParamName.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        tempParamName.replace(loc, fRunNumberDigits, newRunNumber.str());
        msrParamList->at(l).fName = tempParamName;
        if (globalPlus && singleRunMsrFile) {
          fMsrHandler->SetMsrParamValue(l, singleRunMsrFile->GetMsrParamList()->at(l).fValue);
          fMsrHandler->SetMsrParamStep(l, singleRunMsrFile->GetMsrParamList()->at(l).fStep);
        }
      } else {
        std::cerr << std::endl << ">> msr2data: **ERROR** The indices of the run specific parameters do not match the template run number!";
        std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
        std::cerr << std::endl;
      }
    }
    if (singleRunMsrFile) {
      delete singleRunMsrFile;
      singleRunMsrFile = nullptr;
    }

    // substitute the template run-numbers in the RUN names
    for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
      tempRunName = msrRunList->at(i).GetRunName()->Data();
      std::string::size_type loc = tempRunName.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        tempRunName.replace(loc, fRunNumberDigits, newRunNumber.str());
        tempTstr = TString(tempRunName);
        msrRunList->at(i).SetRunName(tempTstr, 0);
      } else {
        std::cerr << std::endl << ">> msr2data: **ERROR** A template run file number does not match the \"file index\"";
        std::cerr << std::endl << ">> msr2data: **ERROR** Please check the template file!";
        std::cerr << std::endl;
        return false;
      }
    }
  }

  std::ostringstream tempStrStr;

  // comments for the output-file
  if (fNumGlobalParam) {
    commentsP[1] = TString("Common parameters for all runs");
  }
  if (fNumSpecParam) {
    tempStrStr.str("");
    tempStrStr << "Specific parameters for run " << *fRunVectorIter;
    tempTstr = tempStrStr.str();
    commentsP[fNumGlobalParam + 1] = tempTstr;
  }
  if (fNumTempRunBlocks) {
    tempStrStr.str("");
    tempStrStr << "RUN blocks for run " << *fRunVectorIter;
    tempTstr = tempStrStr.str();
    commentsR[1] = tempTstr;
  }

  ++fRunVectorIter;

  UInt_t runcounter(0);
  std::map<TString, Int_t> *runParGlobal(nullptr);
  std::map<TString, Int_t>::iterator iter;
  PIntVector *runMapGlobal(nullptr);

  while (fRunVectorIter != fRunVector.end()) {
    tempRunNumber.str(newRunNumber.str());
    newRunNumber.str("");
    newRunNumber.clear();
    newRunNumber.fill('0');
    newRunNumber.setf(std::ios::internal, std::ios::adjustfield);
    newRunNumber.width(fRunNumberDigits);
    newRunNumber << *fRunVectorIter;

    // in global+ mode, read in the single run msr-file
    if (globalPlus)
      singleRunMsrFile = GetSingleRunMsrFile();

    // add parameters for each run
    for (unsigned int l(0); l < fNumSpecParam; ++l) {
      msrParamList->push_back(msrParamList->at(fNumGlobalParam + runcounter*fNumSpecParam + l));
      tempParamName = msrParamList->back().fName.Data();
      std::string::size_type loc = tempParamName.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        tempParamName.replace(loc, fRunNumberDigits, newRunNumber.str());
        msrParamList->back().fName = tempParamName;
        msrParamList->back().fNo += fNumSpecParam;
        if (globalPlus && singleRunMsrFile) {
          fMsrHandler->SetMsrParamValue(msrParamList->size() - 1, singleRunMsrFile->GetMsrParamList()->at(fNumGlobalParam + l).fValue);
          fMsrHandler->SetMsrParamStep(msrParamList->size() - 1, singleRunMsrFile->GetMsrParamList()->at(fNumGlobalParam + l).fStep);
        }
      } else {
        std::cerr << std::endl << ">> msr2data: **ERROR** Something went wrong when appending new parameters!";
        std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
        std::cerr << std::endl;
      }
    }
    if (singleRunMsrFile) {
      delete singleRunMsrFile;
      singleRunMsrFile = nullptr;
    }

    // add RUN blocks for each run
    for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
      msrRunList->push_back(msrRunList->at(runcounter*fNumTempRunBlocks + i));
      tempRunName = msrRunList->back().GetRunName()->Data();
      std::string::size_type loc = tempRunName.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        tempRunName.replace(loc, fRunNumberDigits, newRunNumber.str());
        tempTstr = TString(tempRunName);
        msrRunList->back().SetRunName(tempTstr, 0);
      } else {
        std::cerr << std::endl << ">> msr2data: **ERROR** Something went wrong when appending new RUN blocks!";
        std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
        std::cerr << std::endl;
        return false;
      }
      // change run specific parameter numbers in the new RUN block
      runParGlobal = msrRunList->back().GetParGlobal();
      iter = runParGlobal->find(norm);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          msrRunList->back().SetNormParamNo(msrRunList->back().GetNormParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(bkgfit);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          msrRunList->back().SetBkgFitParamNo(msrRunList->back().GetBkgFitParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(alpha);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          msrRunList->back().SetAlphaParamNo(msrRunList->back().GetAlphaParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(beta);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          msrRunList->back().SetBetaParamNo(msrRunList->back().GetBetaParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(lifetime);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          msrRunList->back().SetLifetimeParamNo(msrRunList->back().GetLifetimeParamNo() + fNumSpecParam);
      }
      runMapGlobal = msrRunList->back().GetMapGlobal();
      for (unsigned int l(0); l < runMapGlobal->size(); ++l) {
        if (!(*runMapGlobal)[l])
           msrRunList->back().SetMap(msrRunList->back().GetMap()->at(l) + fNumSpecParam, l);
      }
    }

    ++runcounter;

    // comments for the output-file
    if (fNumSpecParam) {
      tempStrStr.str("");
      tempStrStr << "Specific parameters for run " << *fRunVectorIter;
      tempTstr = tempStrStr.str();
      commentsP[fNumGlobalParam + runcounter*fNumSpecParam + 1] = tempTstr;
    }
    if (fNumTempRunBlocks) {
      tempStrStr.str("");
      tempStrStr << "RUN blocks for run " << *fRunVectorIter;
      tempTstr = tempStrStr.str();
      commentsR[runcounter*fNumTempRunBlocks + 1] = tempTstr;
    }

    ++fRunVectorIter;
  }

   // set the convergence flag to false because no fit has been performed using the newly generated file
   fMsrHandler->GetMsrStatistic()->fValid = false;

   // write the global msr-file
   status = fMsrHandler->WriteMsrFile(msrOutFile.c_str(), &commentsP, nullptr, nullptr, &commentsR);

   if (status != PMUSR_SUCCESS) {
     std::cerr << std::endl << ">> msr2data: **ERROR** Writing the new msr-file has not been successful!";
     std::cerr << std::endl;
     return false;
   }

   // set back the run-iterator to the start
   fRunVectorIter = fRunVector.begin();

   msrParamList = nullptr;
   msrRunList = nullptr;

   return true;
}

//-------------------------------------------------------------
/**
 * <p> Generate a new msr-file for a single run in which the parameters are sorted for a global fit
 *     An internal sorted msr-file data structure has to be present.
 *
 * <p><b>return:</b>
 * - true if everything is OK
 * - false otherwise
 *
 * \param tempRun template run number
 */
bool PMsr2Data::PrepareNewSortedInputFile(unsigned int tempRun) const
{

  PMsrParamList *msrParamList(fMsrHandler->GetMsrParamList());
  PMsrRunList *msrRunList(fMsrHandler->GetMsrRunList());

  // make a copy of the msr-file-data to be able to restore it after the operations (parameter name and run number changes)
  PMsrParamList paramListCopy(*msrParamList);
  PMsrRunList runListCopy(*msrRunList);
  TString titleCopy(*(fMsrHandler->GetMsrTitle()));

  std::ostringstream tempRunNumber;
  tempRunNumber.fill('0');
  tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  tempRunNumber.width(fRunNumberDigits);
  tempRunNumber << tempRun;

  std::ostringstream newRunNumber;
  newRunNumber.fill('0');
  newRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  newRunNumber.width(fRunNumberDigits);
  newRunNumber << *fRunVectorIter;

  std::string tempParamName, tempRunName;
  TString tempTstr;

  std::ostringstream msrOutFile;
  msrOutFile << *fRunVectorIter << "-OneRunFit" << fFileExtension << ".msr";
  std::cout << std::endl << ">> msr2data: **INFO** Generating new input msr file " << msrOutFile.str() << std::endl;

  tempTstr = TString(newRunNumber.str());
  fMsrHandler->SetMsrTitle(tempTstr);

  // remove the template run-numbers in the parameter names
  for (unsigned int l(fNumGlobalParam); l < msrParamList->size(); ++l) {
    tempParamName = msrParamList->at(l).fName.Data();
    std::string::size_type loc = tempParamName.rfind(tempRunNumber.str());
    if ( loc != std::string::npos ) {
      tempParamName.erase(loc);
      msrParamList->at(l).fName = tempParamName;
    } else {
      std::cerr << std::endl << ">> msr2data: **ERROR** The indices of the run specific parameters do not match the template run number!";
      std::cerr << std::endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
      std::cerr << std::endl;
    }
  }

  if (newRunNumber.str().compare(tempRunNumber.str())) { // first run number does not match the template run number
    // substitute the template run-numbers in the RUN names
    for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
      tempRunName = msrRunList->at(i).GetRunName()->Data();
      std::string::size_type loc = tempRunName.rfind(tempRunNumber.str());
      if ( loc != std::string::npos ) {
        tempRunName.replace(loc, fRunNumberDigits, newRunNumber.str());
        tempTstr = TString(tempRunName);
        msrRunList->at(i).SetRunName(tempTstr, 0);
      } else {
        std::cerr << std::endl << ">> msr2data: **ERROR** A template run file number does not match the \"file index\"";
        std::cerr << std::endl << ">> msr2data: **ERROR** Please check the template file!";
        std::cerr << std::endl;
        return false;
      }
    }
  }

  // set the convergence flag to false because no fit has been performed using the newly generated file
  fMsrHandler->GetMsrStatistic()->fValid = false;

  // write the msr-file
  int status = fMsrHandler->WriteMsrFile(msrOutFile.str().c_str());

  // restore original msr-file-data
  *msrParamList = paramListCopy;
  *msrRunList = runListCopy;
  fMsrHandler->SetMsrTitle(titleCopy);

  paramListCopy.clear();
  runListCopy.clear();
  titleCopy.Clear();

  if (status != PMUSR_SUCCESS) {
    std::cerr << std::endl << ">> msr2data: **ERROR** Writing the new msr file has not been successful!";
    std::cerr << std::endl;
    return false;
  }

  return true;

}

//-------------------------------------------------------------
/**
 * <p> Append fit parameters of a msr file to the DB or ASCII file
 *
 * <p><b>return:</b>
 * - PMUSR_SUCCESS if everything is OK
 * - -1 in case of a fatal error
 * - -2 if a fit has not converged (and the data is not appended to the output file)
 *
 * \param outfile name of the DB/ASCII output file
 * \param paramList parameter list which shall be written to the output file
 * \param db DB or plain ASCII output
 * \param withHeader write output file header or not
 * \param global global mode or not
 * \param counter counter used within the global mode to determine how many runs have been processed already
 */
int PMsr2Data::WriteOutput(const std::string &outfile, const std::vector<unsigned int>& paramList, bool db,
                           unsigned int withHeader, bool global, unsigned int counter) const
{
  // make sure that the parameter number of the parameter list stays within proper bounds
  for (unsigned int i=0; i<paramList.size(); i++) {
    if (paramList[i] > fMsrHandler->GetMsrParamList()->size()) {
      std::cerr << "msr2data: **ERROR** found parameter " << paramList[i] << " which is out of bound (>" << fMsrHandler->GetMsrParamList()->size() << ")." << std::endl;
      return -1;
    }
  }

  if (!to_lower_copy(outfile).compare("none")) {
    fRunVectorIter++;
    return PMUSR_SUCCESS;
  }

  std::ostringstream curRunNumber;
  curRunNumber.fill('0');
  curRunNumber.setf(std::ios::internal, std::ios::adjustfield);
  curRunNumber.width(fRunNumberDigits);
  curRunNumber << *fRunVectorIter;

  std::string msrTitle(fMsrHandler->GetMsrTitle()->Data());
  std::string msrFileName(fMsrHandler->GetFileName().Data());
  unsigned int msrNoOfParams(fMsrHandler->GetNoOfParams());
  PMsrParamList *msrParamList(fMsrHandler->GetMsrParamList());
  PMsrRunList *msrRunList(fMsrHandler->GetMsrRunList());

  if (global && (fRunVectorIter == fRunVector.begin())) {
    // since the DB-ASCII-output is in principle independent of the original msr-file-generation
    // the number of global and run specific parameters as well as the number of RUN blocks per run number have to be determined again
    // in case no msr-file has been created before.
    std::ostringstream tempRunNumber;
    std::string tempName;
    if (!fNumGlobalParam && !fNumSpecParam && !fNumTempRunBlocks) { // if not all parameters are zero they have been determined before
      tempRunNumber.fill('0');
      tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
      tempRunNumber.width(fRunNumberDigits);
      tempRunNumber << fRunVector.front();

      // search parameter list for run-specific parameters
      for (unsigned int i(0); i < msrParamList->size(); ++i) {
        tempName = msrParamList->at(i).fName.Data();
        std::string::size_type loc = tempName.rfind(tempRunNumber.str());
        if ((tempName.length() > fRunNumberDigits) && (loc == tempName.length() - fRunNumberDigits)) {
          if (!fNumSpecParam) {
            fNumGlobalParam = i;
          }
          ++fNumSpecParam;
        }
      }
      if (!fNumSpecParam) {
        fNumGlobalParam = msrParamList->size();
      }

      // look through the RUN blocks and count them
      for (unsigned int i(0); i < msrRunList->size(); ++i) {
        tempName = msrRunList->at(i).GetRunName()->Data();
        std::string::size_type loc = tempName.rfind(tempRunNumber.str());
        if ( loc != std::string::npos ) {
          ++fNumTempRunBlocks;
        }
      }
//       std::cerr << std::endl << ">> msr2data: **WARNING** At the moment the full integrity of the global msr file cannot be checked!";
//       std::cerr << std::endl << ">> msr2data: **WARNING** It should therefore be checked carefully that the run order is consistent";
//       std::cerr << std::endl << ">> msr2data: **WARNING** in the specified run list, the parameters and the RUN blocks in the msr file!";
//       std::cerr << std::endl << ">> msr2data: **WARNING** A simple check for the first specified run yields:";
//       std::cerr << std::endl << ">> msr2data: **WARNING** Number of global parameters: " << fNumGlobalParam;
//       std::cerr << std::endl << ">> msr2data: **WARNING** Number of run specific parameters: " << fNumSpecParam;
//       std::cerr << std::endl << ">> msr2data: **WARNING** Number of RUN blocks per run number: " << fNumTempRunBlocks;
//       std::cerr << std::endl;
    }
    // Two more simple consistency checks
    bool okP(true), okR(true);
    if ((msrParamList->size() - fNumGlobalParam)%fNumSpecParam) {
      okP = false;
    } else if ((msrParamList->size() - fNumGlobalParam)/fNumSpecParam != fRunVector.size()) {
      okP = false;
    }

    if (!okP) {
      std::cerr << std::endl << ">> msr2data: **ERROR** The number of parameters or their grouping is not consistent with the specified run list!";
      std::cerr << std::endl << ">> msr2data: **ERROR** Please check carefully the integrity of the msr file!";
      std::cerr << std::endl << ">> msr2data: **ERROR** No output will be written!";
      std::cerr << std::endl;
      return -1;
    }

    if (msrRunList->size()%fNumTempRunBlocks) {
      okR = false;
    } else if (msrRunList->size()/fNumTempRunBlocks != fRunVector.size()) {
      okR = false;
    }

    if (!okR) {
      std::cerr << std::endl << ">> msr2data: **ERROR** The number of RUN blocks or their grouping is not consistent with the specified run list!";
      std::cerr << std::endl << ">> msr2data: **ERROR** Please check carefully the integrity of the msr file!";
      std::cerr << std::endl << ">> msr2data: **ERROR** No output will be written!";
      std::cerr << std::endl;
      return -1;
    }

    // With all the gathered information look once more through the FITPARAMETER and RUN blocks and check them for the correct run numbers
    for (unsigned int a(0); a < fRunVector.size(); ++a) {
      tempRunNumber.clear();
      tempRunNumber.str("");
      tempRunNumber.fill('0');
      tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
      tempRunNumber.width(fRunNumberDigits);
      tempRunNumber << fRunVector[a];

      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        tempName = msrParamList->at(fNumGlobalParam + a*fNumSpecParam + i).fName.Data();
        std::string::size_type loc = tempName.rfind(tempRunNumber.str());
        if (!(tempName.length() > fRunNumberDigits) || !(loc == tempName.length() - fRunNumberDigits)) {
          okP = false;
          break;
        }
      }
      if (!okP) {
        break;
      }
    }

    if (!okP) {
      std::cerr << std::endl << ">> msr2data: **ERROR** The run specific parameter names are not consistent with the specified run list!";
      std::cerr << std::endl << ">> msr2data: **ERROR** Please check carefully the integrity of the msr file and the run list!";
      std::cerr << std::endl << ">> msr2data: **ERROR** No output will be written!";
      std::cerr << std::endl;
      return -1;
    }

    for (unsigned int a(0); a < fRunVector.size(); ++a) {
      tempRunNumber.clear();
      tempRunNumber.str("");
      tempRunNumber.fill('0');
      tempRunNumber.setf(std::ios::internal, std::ios::adjustfield);
      tempRunNumber.width(fRunNumberDigits);
      tempRunNumber << fRunVector[a];

      for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
        tempName = msrRunList->at(a*fNumTempRunBlocks + i).GetRunName()->Data();
        std::string::size_type loc = tempName.rfind(tempRunNumber.str());
        if (loc == std::string::npos) {
          okR = false;
          break;
        }
      }
      if (!okR) {
        break;
      }
    }

    if (!okR) {
      std::cerr << std::endl << ">> msr2data: **ERROR** The run names in the RUN blocks are not consistent with the specified run list!";
      std::cerr << std::endl << ">> msr2data: **ERROR** Please check carefully the integrity of the msr file and the run list!";
      std::cerr << std::endl << ">> msr2data: **ERROR** No output will be written!";
      std::cerr << std::endl;
      return -1;
    }
  }

  std::vector<std::string> dataParamNames;
  std::vector<std::string> dataParamLabels;
  std::vector<double> dataParam, dataParamErr;

  if (fDataHandler) {
    PRawRunData *rawRunData;
    if (global) {
      rawRunData = fDataHandler->GetRunData((*msrRunList)[fNumTempRunBlocks*counter].GetRunName()->Data());
    } else {
      rawRunData = fDataHandler->GetRunData((*msrRunList)[0].GetRunName()->Data());
    }

    switch (rawRunData->GetNoOfTemperatures()) {
      case 1:
        dataParamNames.push_back("dataT");
        dataParamLabels.push_back("T (K)");
        dataParam.push_back(rawRunData->GetTemperature(0));
        dataParamErr.push_back(rawRunData->GetTempError(0));
        break;
      default:
        std::ostringstream oss;
        for (unsigned int i(0); i<rawRunData->GetNoOfTemperatures(); i++) {
          oss << "dataT" << i;
          dataParamNames.push_back(oss.str());
          oss.str("");
          oss << "T" << i << " (K)";
          dataParamLabels.push_back(oss.str());
          oss.str("");
          dataParam.push_back(rawRunData->GetTemperature(i));
          dataParamErr.push_back(rawRunData->GetTempError(i));
        }
        break;
    }

    double value;
    value = rawRunData->GetField();
    if (value != PMUSR_UNDEFINED) {
      dataParamNames.push_back("dataB");
      dataParamLabels.push_back("B (G)");
      dataParam.push_back(value);
    }

    value = rawRunData->GetEnergy();
    if (value != PMUSR_UNDEFINED) {
      dataParamNames.push_back("dataE");
      dataParamLabels.push_back("Implantation Energy (keV)");
      dataParam.push_back(value);
    }

    value = rawRunData->GetTransport();
    if (value != PMUSR_UNDEFINED) {
      dataParamNames.push_back("dataTr");
      dataParamLabels.push_back("Transport (kV)");
      dataParam.push_back(value);
    }

    PDoubleVector ra(rawRunData->GetRingAnode());
    if (ra.size() > 1) {
      if ((ra[0] != PMUSR_UNDEFINED) && (ra[1] != PMUSR_UNDEFINED)) {
        dataParamNames.push_back("dataRALRAR");
        dataParamLabels.push_back("RAL-RAR (kV)");
        dataParam.push_back(ra[0]-ra[1]);
      }
    }
    if (ra.size() == 4) {
      if ((ra[2] != PMUSR_UNDEFINED) && (ra[3] != PMUSR_UNDEFINED)) {
        dataParamNames.push_back("dataRATRAB");
        dataParamLabels.push_back("RAT-RAB (kV)");
        dataParam.push_back(ra[2]-ra[3]);
      }
    }

    value = rawRunData->GetMuonSpinAngle();
    if (value != PMUSR_UNDEFINED) {
      dataParamNames.push_back("dataSpinRot");
      dataParamLabels.push_back("Spin Rotation Angle (degree)");
      dataParam.push_back(value);
    }

    rawRunData = nullptr;
  }

// get the independent variable values from the runlist file if needed
  PDoubleVector indVarValues;

  if (fRunListFile) {
    std::string line;
    std::vector<std::string> splitVec;
    unsigned int runNo;
    double val;
    std::istringstream strLine;

    while (getline(*fRunListFileStream, line)) {
      trim(line);
      if (line.empty())
        continue;
      else if (line.at(0) == '#' || !to_lower_copy(line.substr(0,3)).compare("run"))
        continue;
      else {
        split( splitVec, line, is_any_of("#") ); // split the string if any comments appear on the line
        strLine.clear();
        strLine.str(splitVec[0]);
        strLine >> runNo;
        if (runNo != *fRunVectorIter) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The run number in the runlist file does not match the one which should be processed...";
          std::cerr << std::endl << ">> msr2data: **ERROR** Something is very strange... Please report this bug!";
          std::cerr << std::endl;
          fRunVectorIter = fRunVector.end();
          return -1;
        }
        while (strLine >> val) {
          indVarValues.push_back(val);
        }
        if (indVarValues.size() != fIndVar.size()) {
          std::cerr << std::endl << ">> msr2data: **ERROR** The number of data entries in the runlist file for the run number " << runNo;
          std::cerr << std::endl << ">> msr2data: **ERROR** does not match the number of labels given in the RUN-line! Please check the file!";
          std::cerr << std::endl;
          fRunVectorIter = fRunVector.end();
          return -1;
        }
        break;
      }
    }
  }

// The RUNLIST file stream and the run vector iterator might get out of synchronization, if the following check is placed before the above block...
  PMsrStatisticStructure *msrStatistic(fMsrHandler->GetMsrStatistic());
  if (!msrStatistic->fValid) { // in the GLOBAL mode this has already been checked before -> check should be negative anyway
    std::cerr << std::endl << ">> msr2data: **WARNING** The fit of run " << *fRunVectorIter << " has not converged!";
    std::cerr << std::endl << ">> msr2data: **WARNING** Its parameter data have not been appended to the output file " << outfile;
    std::cerr << std::endl;
    fRunVectorIter++;

    delete fMsrHandler;
    fMsrHandler = nullptr;

    if (fDataHandler) {
      delete fDataHandler;
      fDataHandler = nullptr;
    }

// clean up some vectors
    dataParamNames.clear();
    dataParamLabels.clear();
    dataParam.clear();
    dataParamErr.clear();
    indVarValues.clear();

    return -2;
  }

// open the DB or dat file and write the data
  std::fstream outFile;
  outFile.open(outfile.c_str(), std::ios::in | std::ios::out | std::ios::ate);
  if (outFile.is_open()) {
    if (((withHeader == 0) || (withHeader == 1)) && !fHeaderWritten) {
      // Header should (not) be written explicitly, start writing at the end of the file
      // This also ensures the backward compatibility to the old "noheader" option
      outFile.seekp(0, std::ios::end);
    } else {
      if (!fHeaderWritten) { // File already present: assume header is present already and find the last written block
        fHeaderWritten = true;
      }
      int size(outFile.tellg());
      std::string s;

      for (int i(1); i<=size; ++i) { // find the last non-empty line
        outFile.seekg(-i, std::ios::end);
        getline(outFile, s);
        trim(s); // remove whitespace
        if (s.empty()) { // trim cuts off also characters like '\n', therefore this should work also with M$-DOS linebreaks
          if (i == size) {
            outFile.seekp(0);
            fHeaderWritten = false;  // if the file contained only empty lines, default to writing the header
            break;
          } else {
            continue;
          }
        } else {
          outFile.seekp(0, std::ios::cur);
          break;
        }
      }
    }
  } else {
    outFile.open(outfile.c_str(), std::ios::out);
    if (!outFile.is_open()) {
      std::cerr << std::endl << ">> msr2data: **ERROR** The output file " << outfile << " cannot be opened! Please check!";
      std::cerr << std::endl;
      fRunVectorIter = fRunVector.end();
      return -1;
    }
  }

  if (db) {

    if (withHeader && !fHeaderWritten) {
      std::cout << std::endl << ">> msr2data: **INFO** Write a new DB file header to " << outfile << std::endl;

      outFile << "TITLE" << std::endl;
      outFile << ">>>Put your title here<<<" << std::endl << std::endl;
      outFile << "Abstract" << std::endl;
      outFile << ">>>Put your abstract here<<<" << std::endl << std::endl;
      outFile << "LABELS" << std::endl;

      if (fDataHandler) {
        for (unsigned int i(0); i < dataParamLabels.size(); ++i) {
          outFile << dataParamLabels[i] << std::endl;
        }
      }

      if (fRunListFile) {
        for (unsigned int i(0); i < fIndVar.size(); ++i) {
          outFile << fIndVar[i] << std::endl;
        }
      }

      // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
      if (global) {
        std::string tempName;
        for (unsigned int i(0); i < fNumGlobalParam; ++i) {
          if (InParameterList(i, paramList))
            outFile << (*msrParamList)[i].fName.Data() << std::endl;
        }
        for (unsigned int i(0); i < fNumSpecParam; ++i) {
          if (InParameterList(fNumGlobalParam + fNumSpecParam*counter + i, paramList)) {
            tempName = (*msrParamList)[fNumGlobalParam + fNumSpecParam*counter + i].fName.Data();
            std::string::size_type loc = tempName.rfind(curRunNumber.str());
            if (loc == tempName.length() - fRunNumberDigits) {
              outFile << tempName.substr(0, loc) << std::endl;
            } else {
              std::cerr << std::endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
              std::cerr << std::endl << ">> msr2data: **ERROR** The output will be flawed!";
              std::cerr << std::endl;
            }
          }
        }
      } else {
        for (unsigned int i(0); i < msrNoOfParams; ++i) {
          if (InParameterList(i, paramList))
            outFile << (*msrParamList)[i].fName.Data() << std::endl;
        }
      }

      if (msrStatistic->fChisq)
        outFile << "CHISQ" << std::endl;
      else
        outFile << "maxLH" << std::endl;

      outFile << "NDF" << std::endl;

      if (msrStatistic->fChisq)
        outFile << "CHISQred" << std::endl;
      else
        outFile << "maxLHred" << std::endl;

      outFile << "RUN" << std::endl;

      outFile << std::endl << "Data";

      if (fDataHandler) {
        for (unsigned int i(0); i < dataParamNames.size(); ++i) {
          outFile << " " << dataParamNames[i];
        }
      }

      if (fRunListFile) {
        for (unsigned int i(0); i < fIndVar.size(); ++i) {
          outFile << " " << fIndVar[i];
        }
      }

      // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
      if (global) {
        std::string tempName;
        for (unsigned int i(0); i < fNumGlobalParam; ++i) {
          if (InParameterList(i, paramList))
            outFile << " " << (*msrParamList)[i].fName.Data();
        }
        for (unsigned int i(0); i < fNumSpecParam; ++i) {
          if (InParameterList(i, paramList)) {
            tempName = (*msrParamList)[fNumGlobalParam + fNumSpecParam*counter + i].fName.Data();
            std::string::size_type loc = tempName.rfind(curRunNumber.str());
            if (loc == tempName.length() - fRunNumberDigits) {
              outFile << " " << tempName.substr(0, loc);
            } else {
              std::cerr << std::endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
              std::cerr << std::endl << ">> msr2data: **ERROR** The output will be flawed!";
              std::cerr << std::endl;
            }
          }
        }
      } else {
        for (unsigned int i(0); i < msrNoOfParams; ++i) {
          if (InParameterList(i, paramList))
            outFile << " " << (*msrParamList)[i].fName.Data();
        }
      }

      if (msrStatistic->fChisq)
        outFile << " " << "CHISQ";
      else
        outFile << " " << "maxLH";

      outFile << " " << "NDF";

      if (msrStatistic->fChisq)
        outFile << " " << "CHISQred";
      else
        outFile << " " << "maxLHred";

      outFile << " " << "RUN" << std::endl;

      outFile << "\\-e" << std::endl;

      fHeaderWritten = true;
    }

    if (fDataHandler) {
      for (unsigned int i(0); i < dataParam.size(); ++i) {
        if (i < dataParamErr.size())
          outFile << dataParamNames[i] << " = " << dataParam[i] << ", " \
          << dataParamErr[i] << ", " << dataParamErr[i] << ",\\" << std::endl;
        else
          outFile << dataParamNames[i] << " = " << dataParam[i] << ", 0, 0,\\" << std::endl;
      }
    }

    if (fRunListFile) {
      for (unsigned int i(0); i < indVarValues.size(); ++i) {
        outFile << fIndVar[i] << " = " << indVarValues[i] << ", 0, 0,\\" << std::endl;
      }
    }

    // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
    if (global) {
      std::string tempName;
      unsigned int idx;
      for (unsigned int i(0); i < fNumGlobalParam; ++i) {
        if (InParameterList(i, paramList)) {
          outFile << (*msrParamList)[i].fName.Data() << " = ";
          if ((*msrParamList)[i].fPosErrorPresent) {
            WriteValue(outFile, (*msrParamList)[i].fValue, (*msrParamList)[i].fPosError, outFile.width(), db);
            outFile << ", ";
          } else {
            outFile << (*msrParamList)[i].fValue << ", ";
          }

          if ((*msrParamList)[i].fPosErrorPresent)
            outFile << (*msrParamList)[i].fPosError << ", ";
          else
            outFile << fabs((*msrParamList)[i].fStep) << ", ";
          outFile << fabs((*msrParamList)[i].fStep) << ",\\" << std::endl;
        }
      }
      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        idx = fNumGlobalParam + fNumSpecParam*counter + i;
        if (InParameterList(idx, paramList)) {
          tempName = (*msrParamList)[idx].fName.Data();
          std::string::size_type loc = tempName.rfind(curRunNumber.str());
          if (loc == tempName.length() - fRunNumberDigits) {
            outFile << tempName.substr(0, loc) << " = ";
            if ((*msrParamList)[idx].fPosErrorPresent) {
              WriteValue(outFile, (*msrParamList)[idx].fValue, (*msrParamList)[idx].fPosError, outFile.width(), db);
              outFile << ", ";
            } else {
              outFile << (*msrParamList)[idx].fValue << ", ";
            }
            if ((*msrParamList)[idx].fPosErrorPresent) {
              WriteValue(outFile, (*msrParamList)[idx].fPosError, (*msrParamList)[idx].fPosError, outFile.width(), db);
              outFile << ", ";
            } else {
              WriteValue(outFile, (*msrParamList)[idx].fStep, (*msrParamList)[idx].fStep, outFile.width(), db);
              outFile << ", ";
            }
            WriteValue(outFile, (*msrParamList)[idx].fStep, (*msrParamList)[idx].fStep, outFile.width(), db);
            outFile << ",\\" << std::endl;
          }
        }
      }
    } else {
      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        if (InParameterList(i, paramList)) {
          outFile << (*msrParamList)[i].fName.Data() << " = ";
          if ((*msrParamList)[i].fPosErrorPresent) {
            WriteValue(outFile, (*msrParamList)[i].fValue, (*msrParamList)[i].fPosError, outFile.width(), db);
            outFile << ", ";
          } else {
            WriteValue(outFile, (*msrParamList)[i].fValue, fabs((*msrParamList)[i].fStep), outFile.width(), db);
            outFile << ", ";
          }
          if ((*msrParamList)[i].fPosErrorPresent) {
            WriteValue(outFile, (*msrParamList)[i].fPosError, (*msrParamList)[i].fPosError, outFile.width(), db);
            outFile << ", ";
          } else {
            WriteValue(outFile, (*msrParamList)[i].fStep, (*msrParamList)[i].fStep, outFile.width(), db);
            outFile << ", ";
          }
          WriteValue(outFile, (*msrParamList)[i].fStep, (*msrParamList)[i].fStep, outFile.width(), db);
          outFile << ",\\" << std::endl;
        }
      }
    }

    if (msrStatistic->fChisq)
      outFile << "CHISQ = " << msrStatistic->fMin << ", 0, 0,\\" << std::endl;
    else
      outFile << "maxLH = " << msrStatistic->fMin << ", 0, 0,\\" << std::endl;

    outFile << "NDF = " << msrStatistic->fNdf << ", 0, 0,\\" << std::endl;

    if (msrStatistic->fChisq)
      outFile << "CHISQred = " << msrStatistic->fMin/msrStatistic->fNdf << ", 0, 0,\\" << std::endl;
    else
      outFile << "maxLHred = " << msrStatistic->fMin/msrStatistic->fNdf << ", 0, 0,\\" << std::endl;

    outFile << *fRunVectorIter << ",,, " << msrTitle << std::endl;

  } else { // no DB file but ASCII file with data columns

    unsigned int length(0), maxlength(12);
    for (unsigned int i(0); i < dataParamNames.size(); ++i) {
      length = dataParamNames[i].length();
      if (length > maxlength)
        maxlength = length;
    }
    for (unsigned int i(0); i < fIndVar.size(); ++i) {
      length = fIndVar[i].length();
      if (length > maxlength)
        maxlength = length;
    }
    // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
    std::string s;
    if (global) {
      for (unsigned int i(0); i < fNumGlobalParam; ++i) {
        if (InParameterList(i, paramList)) {
          s = (*msrParamList)[i].fName.Data();
          length = s.length();
          if (length > maxlength)
            maxlength = length;
        }
      }
      unsigned int idx;
      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        idx = fNumGlobalParam + fNumSpecParam*counter + i;
        if (InParameterList(idx, paramList)) {
          s = (*msrParamList)[idx].fName.Data();
          std::string::size_type loc = s.rfind(curRunNumber.str());
          if (loc == s.length() - fRunNumberDigits) {
            length = s.length() - fRunNumberDigits;
            if (length > maxlength)
              maxlength = length;
          } else {
            std::cerr << std::endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
            std::cerr << std::endl << ">> msr2data: **ERROR** The output will be flawed!";
            std::cerr << std::endl;
          }
        }
      }
    } else {
      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        if (InParameterList(i, paramList)) {
          s = (*msrParamList)[i].fName.Data();
          length = s.length();
          if (length > maxlength)
            maxlength = length;
        }
      }
    }
    if (maxlength < 13)
      maxlength = 13; // will use a minimum field width of 13 which corresponds to: -1.23456e-07 + ' '
    else
      maxlength += 1; // maximum length of parameter names + ' '

    if (withHeader && !fHeaderWritten) {
      std::cout << std::endl << ">> msr2data: **INFO** Write a new simple-ASCII file header to " << outfile << std::endl;

      if (fDataHandler) {
        for (unsigned int i(0); i < dataParamNames.size(); ++i) {
          s = dataParamNames[i];
          if (i < dataParamErr.size()) {
            outFile << std::setw(maxlength) << std::left << s << std::setw(maxlength + 3) << std::left << s + "Err";
          } else
            outFile << std::setw(maxlength) << std::left << s;
        }
      }

      if (fRunListFile) {
        for (unsigned int i(0); i < fIndVar.size(); ++i) {
          outFile << std::setw(maxlength) << std::left << fIndVar[i];
        }
      }

      // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
      if (global) {
        for (unsigned int i(0); i < fNumGlobalParam; ++i) {
          if (InParameterList(i, paramList)) {
            s = (*msrParamList)[i].fName.Data();
            outFile << std::setw(maxlength) << std::left << s \
                    << std::setw(maxlength + 6) << std::left << s + "PosErr" \
                    << std::setw(maxlength + 6) << std::left << s + "NegErr";
          }
        }
        unsigned int idx;
        for (unsigned int i(0); i < fNumSpecParam; ++i) {
          idx = fNumGlobalParam + fNumSpecParam*counter + i;
          if (InParameterList(idx, paramList)) {
            s = (*msrParamList)[idx].fName.Data();
            std::string::size_type loc = s.rfind(curRunNumber.str());
            if (loc == s.length() - fRunNumberDigits) {
              s = s.substr(0, loc);
              outFile << std::setw(maxlength) << std::left << s \
                      << std::setw(maxlength + 6) << std::left << s + "PosErr" \
                      << std::setw(maxlength + 6) << std::left << s + "NegErr";
            } else {
              std::cerr << std::endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
              std::cerr << std::endl << ">> msr2data: **ERROR** The output will be flawed!";
              std::cerr << std::endl;
            }
          }
        }
      } else {
        for (unsigned int i(0); i < msrNoOfParams; ++i) {
          if (InParameterList(i, paramList)) {
            s = (*msrParamList)[i].fName.Data();
            outFile << std::setw(maxlength) << std::left << s \
                    << std::setw(maxlength + 6) << std::left << s + "PosErr" \
                    << std::setw(maxlength + 6) << std::left << s + "NegErr";
          }
        }
      }
      s.clear();

      if (msrStatistic->fChisq)
        outFile << std::setw(maxlength) << std::left << "CHISQ";
      else
        outFile << std::setw(maxlength) << std::left << "maxLH";

      outFile << std::setw(maxlength) << std::left << "NDF";

      if (msrStatistic->fChisq)
        outFile << std::setw(maxlength) << std::left << "CHISQred";
      else
        outFile << std::setw(maxlength) << std::left << "maxLHred";

      outFile << std::setw(maxlength) << std::left << "RUN" << std::endl;

      fHeaderWritten = true;
    }

    if (fDataHandler) {
      for (unsigned int i(0); i < dataParam.size(); ++i) {
        if (i < dataParamErr.size()) {
          WriteValue(outFile, dataParam[i], maxlength);
          WriteValue(outFile, dataParamErr[i], maxlength + 3);
        } else {
          WriteValue(outFile, dataParam[i], maxlength);
        }
      }
    }

    if (fRunListFile) {
      for (unsigned int i(0); i < indVarValues.size(); ++i) {
        WriteValue(outFile, indVarValues[i], maxlength);
      }
    }

    // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
    if (global) {
      for (unsigned int i(0); i < fNumGlobalParam; ++i) {
        if (InParameterList(i, paramList)) {
          if ((*msrParamList)[i].fPosErrorPresent)
            WriteValue(outFile, (*msrParamList)[i].fValue, (*msrParamList)[i].fPosError, maxlength, db);
          else
            WriteValue(outFile, (*msrParamList)[i].fValue, maxlength);

          if ((*msrParamList)[i].fPosErrorPresent)
            WriteValue(outFile, (*msrParamList)[i].fPosError, (*msrParamList)[i].fPosError, maxlength, db);
          else
            WriteValue(outFile, fabs((*msrParamList)[i].fStep), (*msrParamList)[i].fStep, maxlength, db);

          WriteValue(outFile, fabs((*msrParamList)[i].fStep), (*msrParamList)[i].fStep, maxlength, db);
        }
      }
      unsigned int idx;
      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        idx = fNumGlobalParam + fNumSpecParam*counter + i;
        if (InParameterList(idx, paramList)) {
          if ((*msrParamList)[idx].fPosErrorPresent)
            WriteValue(outFile, (*msrParamList)[idx].fValue, (*msrParamList)[idx].fPosError, maxlength, db);
          else
            WriteValue(outFile, (*msrParamList)[idx].fValue, maxlength);

          if ((*msrParamList)[idx].fPosErrorPresent)
            WriteValue(outFile, (*msrParamList)[idx].fPosError, (*msrParamList)[idx].fPosError, maxlength, db);
          else
            WriteValue(outFile, (*msrParamList)[idx].fStep, (*msrParamList)[idx].fStep, maxlength, db);

          WriteValue(outFile, (*msrParamList)[idx].fStep, (*msrParamList)[idx].fStep, maxlength, db);
        }
      }
    } else {
      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        if (InParameterList(i, paramList)) {
          if ((*msrParamList)[i].fPosErrorPresent)
            WriteValue(outFile, (*msrParamList)[i].fValue, (*msrParamList)[i].fPosError, maxlength, db);
          else
            WriteValue(outFile, (*msrParamList)[i].fValue, fabs((*msrParamList)[i].fStep), maxlength, db);

          if ((*msrParamList)[i].fPosErrorPresent)
            WriteValue(outFile, (*msrParamList)[i].fPosError, (*msrParamList)[i].fPosError, maxlength, db);
          else
            WriteValue(outFile, fabs((*msrParamList)[i].fStep), fabs((*msrParamList)[i].fStep), maxlength, db);

          WriteValue(outFile, fabs((*msrParamList)[i].fStep), fabs((*msrParamList)[i].fStep), maxlength, db);
        }
      }
    }

    WriteValue(outFile, msrStatistic->fMin, maxlength);

    WriteValue(outFile, msrStatistic->fNdf, maxlength);

    WriteValue(outFile, msrStatistic->fMin/msrStatistic->fNdf, maxlength);

    WriteValue(outFile, *fRunVectorIter, maxlength);
    outFile << std::endl;

  }

  if (global) {
    std::cout << std::endl << ">> msr2data: **INFO** Parameter data of run " << *fRunVectorIter << " of file " << msrFileName \
                 << " have been appended to " << outfile << std::endl;
  } else {
    std::cout << std::endl << ">> msr2data: **INFO** Parameter data of file " << msrFileName << " have been appended to " << outfile << std::endl;
  }

  fRunVectorIter++;

  outFile.close();

  if (!global || (fRunVectorIter == fRunVector.end())) {
    delete fMsrHandler;
    fMsrHandler = nullptr;

    if (fDataHandler) {
      delete fDataHandler;
      fDataHandler = nullptr;
    }
  }

  msrParamList = nullptr;
  msrRunList = nullptr;

// clean up some vectors
  dataParamNames.clear();
  dataParamLabels.clear();
  dataParam.clear();
  dataParamErr.clear();
  indVarValues.clear();

  return PMUSR_SUCCESS;

}

//-------------------------------------------------------------
/**
 * <p>Write formatted output to column-formatted ASCII output file
 *
 * \param outFile output file stream to the ASCII file
 * \param value number to be written to the ASCII file
 * \param width column width of the ASCII file
 */
void PMsr2Data::WriteValue(std::fstream &outFile, const double &value, const unsigned int &width) const
{
  if ((fabs(value) >= 1.0e6) || ((fabs(value) < 1.0e-4) && (fabs(value) > 0.0)))
    outFile << std::scientific << std::setprecision(width - 8);
  else
    outFile.unsetf(std::ios::floatfield);
  outFile << std::setw(width) << std::left << value;
}

//-------------------------------------------------------------
/**
 * <p>
 *
 * \param outFile
 * \param value
 * \param errValue
 * \param width
 */
void PMsr2Data::WriteValue(std::fstream &outFile, const double &value, const double &errValue, const unsigned int &width, const bool &db) const
{
  Int_t previous_prec = outFile.precision();
  Int_t prec = GetFirstSignificantDigit(errValue);

  // for iostream also the number of digit before the decimal points are needed
  if (fabs(value) > 0.0)
    prec += static_cast<int>(log10(fabs(value)));

  if ((fabs(value) >= 1.0e6) || ((fabs(value) < 1.0e-4) && (fabs(value) > 0)))
    outFile << std::scientific;
  else
    outFile.unsetf(std::ios::floatfield);

  outFile.precision(prec);
  outFile << std::setw(width) << std::left << value;
  outFile.precision(previous_prec);

  // make sure there is at least one space before the next number starts
  if (!db) {
    outFile << " ";
  }
}

//-------------------------------------------------------------
/**
 * <p>
 *
 * \param value
 */
int PMsr2Data::GetFirstSignificantDigit(const double &value) const
{
  int prec=6;

  int i=0;
  bool done = false;
  double dval = value;
  do {
    if (fabs(dval) >= 1.0)
      done = true;
    i++;
    dval *= 10.0;
  } while ((i<20) && !done);

  if (i<20)
    prec = i;

  return prec+1;
}

//-------------------------------------------------------------
/**
 *
 */
bool PMsr2Data::InParameterList(const unsigned int &paramValue, const std::vector<unsigned int> &paramList) const
{
  // if paramList.size() == 0, i.e. use ALL parameters
  if (paramList.size() == 0)
    return true;

  for (unsigned int i=0; i<paramList.size(); i++) {
    if (paramValue+1 == paramList[i])
      return true;
  }

  return false;
}

//-------------------------------------------------------------------------------------------------------
// end
//-------------------------------------------------------------------------------------------------------
