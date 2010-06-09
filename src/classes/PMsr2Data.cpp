/***************************************************************************

  PMsr2Data.cpp

  Author: Bastian M. Wojek / Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek / Andreas Suter                *
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

#include <cctype>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;

#include <boost/algorithm/string/trim.hpp>  // for stripping leading whitespace in std::string 
#include <boost/algorithm/string/case_conv.hpp> // for to_lower() in std::string
#include <boost/algorithm/string/split.hpp> // split strings at certain characters
using namespace boost::algorithm;

#include <boost/lexical_cast.hpp> // for atoi-replacement

#include "PMsr2Data.h"

//-------------------------------------------------------------
/**
 * <p> Write formatted output to column-formatted ASCII output file
 */
void writeValues(ofstream &outFile, const double &value, const unsigned int &width)
{
  if (fabs(value) >= 1.0e6 || (fabs(value) < 1.0e-4 && fabs(value) > 0.0))
    outFile << scientific << setprecision(width - 8);
  else
    outFile.setf(ios::floatfield);
  outFile << setw(width) << left << value;
  return;
}

//-------------------------------------------------------------
/**
 * <p>
 */
PMsr2Data::PMsr2Data(const string &ext) : fFileExtension(ext), fRunListFile(false), fNumGlobalParam(0), fNumSpecParam(0), fNumTempRunBlocks(0)
{
  fRunVector.clear();
  fRunVectorIter = fRunVector.end();
  fRunListFileStream = 0;
  fSaxParser = 0;
  fStartupHandler = 0;
  fDataHandler = 0;
  fMsrHandler = 0;
}

//-------------------------------------------------------------
/**
 * <p>
 */
PMsr2Data::~PMsr2Data()
{
  fRunVector.clear();
  fRunVectorIter = fRunVector.end();
  fIndVar.clear();

  if (fRunListFileStream) {
    fRunListFileStream->close();
    delete fRunListFileStream;
    fRunListFileStream = 0;
  }
  if (fSaxParser) {
    delete fSaxParser;
    fSaxParser = 0;
  }
  if (fStartupHandler) {
    delete fStartupHandler;
    fStartupHandler = 0;
  }
  if (fDataHandler) {
    delete fDataHandler;
    fDataHandler = 0;
  }
  if (fMsrHandler) {
    delete fMsrHandler;
    fMsrHandler = 0;
  }
}

//-------------------------------------------------------------
/**
 * <p>
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
 * <p> 
 */
int PMsr2Data::SetRunNumbers(unsigned int runNo)
{
  if (runNo > 9999 || runNo < 1)
    return 1;

  fRunVector.clear();
  fRunVector.push_back(runNo);
  fRunVectorIter = fRunVector.begin();

  return 0;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
int PMsr2Data::SetRunNumbers(unsigned int runNoStart, unsigned int runNoEnd)
{
  if ((runNoStart > 9999) || (runNoEnd > 9999) || (runNoStart < 1) || (runNoEnd < 1))
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
 * <p> 
 */
int PMsr2Data::SetRunNumbers(const vector<unsigned int> &runListVector)
{
  if (runListVector.empty())
    return -1;

  for (vector<unsigned int>::const_iterator iter(runListVector.begin()); iter!=runListVector.end(); iter++) {
    if (*iter > 9999 || *iter < 1)
      return 1;
  }

  fRunVector = runListVector;
  fRunVectorIter = fRunVector.begin();

  return 0;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
int PMsr2Data::SetRunNumbers(const string &runListFile)
{
  fRunVector.clear();

  ifstream in(runListFile.c_str());
  if (!in) {
    cerr << endl << ">> msr2data: **ERROR** The runlist file " << runListFile << " cannot be opened! Please check!";
    cerr << endl;
    return -1;
  }

  string line, indvar;
  istringstream strLine;
  vector<string> splitVec;
  unsigned int cntr(0);
  unsigned int runNo;

  while (getline(in, line)) {
    trim(line);
    if (line.empty())
      continue;
    else if (line.at(0) == '#')
      continue;
    else
      cntr++;

    split( splitVec, line, is_any_of("#") ); // split the string if any comments appear on the line

    if (cntr == 1) { // Read in the names of the independent variables in the runlist file
      strLine.clear();
      strLine.str(splitVec[0]);
      strLine >> indvar; // "RUN"
      if (indvar.compare("RUN")) {
        cerr << endl << ">> msr2data: **ERROR** The format of the runlist file " << runListFile << " is not correct! Please check!";
        cerr << endl;
      }
      while (strLine >> indvar)
        fIndVar.push_back(indvar);
    }

    if (cntr > 1) {
      strLine.clear();
      strLine.str(splitVec[0]);
      strLine >> runNo;
      if (runNo > 9999 || runNo < 1)
        return 1;
      fRunVector.push_back(runNo);
    }

    splitVec.clear();
  }

  in.close();
  fRunVectorIter = fRunVector.begin();
  fRunListFile = true;
  fRunListFileStream = new ifstream(runListFile.c_str());
  return 0;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
int PMsr2Data::ParseXmlStartupFile()
{
  int status;
  fSaxParser = new TSAXParser();
  fStartupHandler = new PStartupHandler();
  string startup_path_name(fStartupHandler->GetStartupFilePath().Data());
  fSaxParser->ConnectToHandler("PStartupHandler", fStartupHandler);
  status = fSaxParser->ParseFile(startup_path_name.c_str());
  // check for parse errors
  if (status) { // error
    cerr << endl << ">> msr2data: **WARNING** reading/parsing musrfit_startup.xml." << endl;
    // clean up
    if (fSaxParser) {
      delete fSaxParser;
      fSaxParser = 0;
    }
    if (fStartupHandler) {
      delete fStartupHandler;
      fStartupHandler = 0;
    }
  }
  return status;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
int PMsr2Data::ReadMsrFile(const string &infile) const
{
  int status;
  fMsrHandler = new PMsrHandler(infile.c_str());
  status = fMsrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        cerr << endl << ">> msr2data: **ERROR** Could not find " << infile << endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        cerr << endl << ">> msr2data: **SYNTAX ERROR** in file " << infile << ", full stop here." << endl;
        break;
      default:
        cerr << endl << ">> msr2data: **UNKOWN ERROR** when trying to read the msr-file" << endl;
        break;
    }
  }
  return status;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
bool PMsr2Data::ReadRunDataFile()
{
  if (fStartupHandler)
    fDataHandler = new PRunDataHandler(fMsrHandler, fStartupHandler->GetDataPathList());
  else
    fDataHandler = new PRunDataHandler(fMsrHandler);

  bool success = fDataHandler->IsAllDataAvailable();
  if (!success) {
    cerr << endl << ">> msr2data: **WARNING** Could not read all data files, will continue without the data file information..." << endl;
    delete fDataHandler;
    fDataHandler = 0;
  }
  return success;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
bool PMsr2Data::PrepareNewInputFile(unsigned int tempRun) const
{
  if (fRunVectorIter == fRunVector.end())
    return false;

  if (*fRunVectorIter == tempRun)
    return true;

  const unsigned int N(4); // number of digits for the runnumber

  ostringstream strInfile;
  strInfile << tempRun << fFileExtension << ".msr";
  ifstream in(strInfile.str().c_str());
  if (!in) {
    cerr << endl << ">> msr2data: **ERROR** The template msr-file " << strInfile.str() << " cannot be opened! Please check!";
    cerr << endl;
    return false;
  }
  ostringstream strOutfile;
  strOutfile << *fRunVectorIter << fFileExtension << ".msr";
  ofstream out(strOutfile.str().c_str());
  if (!out) {
    cerr << endl << ">> msr2data: **ERROR** The new msr file " << strOutfile.str() << " cannot be opened! Please check!";
    cerr << endl;
    return false;
  }

  ostringstream tempRunNumber;
  tempRunNumber.fill('0');
  tempRunNumber.setf(ios::internal, ios::adjustfield);
  tempRunNumber.width(N);
  tempRunNumber << tempRun;

  ostringstream newRunNumber;
  newRunNumber.fill('0');
  newRunNumber.setf(ios::internal, ios::adjustfield);
  newRunNumber.width(N);
  newRunNumber << *fRunVectorIter;

  cout << endl << ">> msr2data: **INFO** Generating new input msr file " << strOutfile.str() << endl;

  string line, firstOnLine;
  istringstream strLine;

  // write the run number in the TITLE line of the msr-file (is overwritten later by musrfit if called with the -t option)
  getline(in, line);
  out << *fRunVectorIter << endl;

  while (getline(in, line)) {
    if (line.empty()) {
      out << endl;
      continue;
    }
    if (!line.compare("SET BATCH") || !line.compare("END RETURN"))
      continue;

    strLine.clear();
    strLine.str(line);
    strLine >> firstOnLine;
    if (!firstOnLine.compare("RUN")) {
       string::size_type loc = line.rfind(tempRunNumber.str());
       if ( loc != string::npos ) {
         line.replace(loc, N, newRunNumber.str());
       } else {
         cerr << endl << ">> msr2data: **WARNING** The template run file number does not match the \"file index\"";
         cerr << endl << ">> msr2data: **WARNING** Unexpected things will happen... (for sure)";
         cerr << endl;
       }
    }
    out << line << endl;
  }
  in.close();
  out.close();
  return true;
}


//-------------------------------------------------------------
/**
 * <p> 
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
 * <p> 
 */
bool PMsr2Data::PrepareGlobalInputFile(unsigned int tempRun, const string &msrOutFile) const
{
  const unsigned int N(4); // number of digits for the runnumber
  const TString alpha("alpha"), beta("beta"), norm("norm"), bkgfit("bkgfit"), lifetime("lifetime");

  ostringstream strInfile;
  strInfile << tempRun << fFileExtension << ".msr";
//   ifstream in(strInfile.str().c_str());
//   if (!in) {
//     cerr << endl << ">> msr2data: **ERROR** The template msr-file " << strInfile.str() << " cannot be opened! Please check!";
//     cerr << endl;
//     return false;
//   }
//  ostringstream strOutfile;
//  strOutfile << tempRun << "+global" << fFileExtension << ".msr";
//   ofstream out(strOutfile.str().c_str());
//   if (!out) {
//     cerr << endl << ">> msr2data: **ERROR** The new msr file " << strOutfile.str() << " cannot be opened! Please check!";
//     cerr << endl;
//     return false;
//   }

  // read template msr-file
  int status(ReadMsrFile(strInfile.str()));
  if (status != PMUSR_SUCCESS)
    return false;

  // check how many RUN blocks are in the template msr-file
  fNumTempRunBlocks = fMsrHandler->GetMsrRunList()->size();

  // check that the RUN block run numbers match the template run number
  ostringstream tempRunNumber;
  tempRunNumber.fill('0');
  tempRunNumber.setf(ios::internal, ios::adjustfield);
  tempRunNumber.width(N);
  tempRunNumber << tempRun;

  string tempRunName;
  for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
    tempRunName = fMsrHandler->GetMsrRunList()->at(i).GetRunName()->Data();
    string::size_type loc = tempRunName.rfind(tempRunNumber.str());
    if ( loc == string::npos ) {
      cerr << endl << ">> msr2data: **ERROR** A template run file number does not match the \"file index\"";
      cerr << endl << ">> msr2data: **ERROR** Please check the template file!";
      cerr << endl;
      return false;
    }
  }

  cout << endl << ">> msr2data: **INFO** Generating new global input msr file " << msrOutFile << endl;

  // search parameter list for run-specific parameters - the rest is assumed to be global
  string tempParamName;
  for (unsigned int i(0); i < fMsrHandler->GetMsrParamList()->size(); ++i) {
    tempParamName = fMsrHandler->GetMsrParamList()->at(i).fName.Data();
    string::size_type loc = tempParamName.rfind(tempRunNumber.str());
    if ((tempParamName.length() > N) && (loc == tempParamName.length() - N)) {
      fMsrHandler->GetMsrParamList()->at(i).fIsGlobal = false;
      ++fNumSpecParam;
    } else {
      fMsrHandler->GetMsrParamList()->at(i).fIsGlobal = true;
      ++fNumGlobalParam;
    }
    //cout << fMsrHandler->GetMsrParamList()->at(i).fNo << " is global: " << fMsrHandler->GetMsrParamList()->at(i).fIsGlobal << endl;
  }

  // Sort the parameters: global ones first, then run-specific
  sort(fMsrHandler->GetMsrParamList()->begin(), fMsrHandler->GetMsrParamList()->end(), compare_parameters);

  // Change the parameter numbers in all blocks according to the new order
  PMsrLines *tempLines;
  vector<string> tempVec;
  int tempPar;
  string line;
  // THEORY block
  tempLines = fMsrHandler->GetMsrTheory();
  for (unsigned int i(0); i < tempLines->size(); ++i) {
    line = (*tempLines)[i].fLine.Data();
    split( tempVec, line, is_any_of(" ") ); // split the theory line at spaces
    for (unsigned int j(1); j < tempVec.size(); ++j) {
      try {
        tempPar = boost::lexical_cast<unsigned int>(tempVec[j]);
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (unsigned int k(0); k < fMsrHandler->GetMsrParamList()->size(); ++k) {
          if (tempPar == fMsrHandler->GetMsrParamList()->at(k).fNo) {
            tempVec[j] = boost::lexical_cast<string>(k + 1);
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
  tempLines = fMsrHandler->GetMsrFunctions();
  for (unsigned int i(0); i < tempLines->size(); ++i) {
    line = (*tempLines)[i].fLine.Data();
    split( tempVec, line, is_any_of(" ") ); // split the function line at spaces
    for (unsigned int j(2); j < tempVec.size(); ++j) {
      if (!tempVec[j].substr(0,3).compare("par")) {
        try {
          tempPar = boost::lexical_cast<unsigned int>(tempVec[j].substr(3));
          // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
          for (unsigned int k(0); k < fMsrHandler->GetMsrParamList()->size(); ++k) {
            if (tempPar == fMsrHandler->GetMsrParamList()->at(k).fNo) {
              tempVec[j] = "par";
              tempVec[j].append(boost::lexical_cast<string>(k + 1));
              break;
            }
          }
        }
        catch(boost::bad_lexical_cast &) {
          cerr << endl << ">> msr2data: **ERROR** Something is wrong with the parameters used in the FUNCTIONS block!";
          cerr << endl << ">> msr2data: **ERROR** Please report a bug - function parsing should have failed earlier!";
          cerr << endl;
          return false;
        }
      }
    }
    // replace the old function line by the new one
    (*tempLines)[i].fLine.Clear();
    for (unsigned int j(0); j < tempVec.size(); ++j) {
      (*tempLines)[i].fLine += TString(tempVec[j]) + TString(" ");
    }
    tempVec.clear();
  }
  
  // RUN blocks
  // substitute the old parameter numbers by the new ones and also
  // search the RUN-blocks for run-specific parameters

  vector<vector<pair<int, bool> > > runBlockParamIsGlobal(fNumTempRunBlocks);
  pair<int, bool> runBlockTempParam;
  vector<int> *tempMap;
  unsigned int l(0);

  // look in the following order through the RUN-blocks: norm, bkg-fit, alpha, beta, lifetime, maps
  for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
    tempPar = fMsrHandler->GetMsrRunList()->at(i).GetNormParamNo();
    if (tempPar > 0) {
      // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
      for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
        if (tempPar == fMsrHandler->GetMsrParamList()->at(l).fNo) {
          fMsrHandler->GetMsrRunList()->at(i).SetNormParamNo(l + 1);
          break;
        }
      }
      if (l > fMsrHandler->GetMsrParamList()->size()) {
        cerr << endl << ">> msr2data: **ERROR** The norm parameter specified in RUN block " << i + 1 << " does not exist!";
        cerr << endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
        cerr << endl;
        return false;
      }
      if (fMsrHandler->GetMsrParamList()->at(l).fIsGlobal) { // norm of RUN block i is global
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(norm, 1);
      } else {
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(norm, 0);
      }
    }
    tempPar = fMsrHandler->GetMsrRunList()->at(i).GetBkgFitParamNo();
    if (tempPar > 0) {
      // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
      for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
        if (tempPar == fMsrHandler->GetMsrParamList()->at(l).fNo) {
          fMsrHandler->GetMsrRunList()->at(i).SetBkgFitParamNo(l + 1);
          break;
        }
      }
      if (l > fMsrHandler->GetMsrParamList()->size()) {
        cerr << endl << ">> msr2data: **ERROR** The backgr.fit parameter specified in RUN block " << i + 1 << " does not exist!";
        cerr << endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
        cerr << endl;
        return false;
      }
      if (fMsrHandler->GetMsrParamList()->at(l).fIsGlobal) { // bkg-fit of RUN block i is global
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(bkgfit, 1);
      } else {
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(bkgfit, 0);
      }
    }
    tempPar = fMsrHandler->GetMsrRunList()->at(i).GetAlphaParamNo();
    if (tempPar > 0) {
      // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
      for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
        if (tempPar == fMsrHandler->GetMsrParamList()->at(l).fNo) {
          fMsrHandler->GetMsrRunList()->at(i).SetAlphaParamNo(l + 1);
          break;
        }
      }
      if (l > fMsrHandler->GetMsrParamList()->size()) {
        cerr << endl << ">> msr2data: **ERROR** The alpha parameter specified in RUN block " << i + 1 << " does not exist!";
        cerr << endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
        cerr << endl;
        return false;
      }
      if (fMsrHandler->GetMsrParamList()->at(l).fIsGlobal) { // alpha of RUN block i is global
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(alpha, 1);
      } else {
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(alpha, 0);
      }
    }
    tempPar = fMsrHandler->GetMsrRunList()->at(i).GetBetaParamNo();
    if (tempPar > 0) {
      // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
      for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
        if (tempPar == fMsrHandler->GetMsrParamList()->at(l).fNo) {
          fMsrHandler->GetMsrRunList()->at(i).SetBetaParamNo(l + 1);
          break;
        }
      }
      if (l > fMsrHandler->GetMsrParamList()->size()) {
        cerr << endl << ">> msr2data: **ERROR** The beta parameter specified in RUN block " << i + 1 << " does not exist!";
        cerr << endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
        cerr << endl;
        return false;
      }
      if (fMsrHandler->GetMsrParamList()->at(l).fIsGlobal) { // beta of RUN block i is global
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(beta, 1);
      } else {
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(beta, 0);
      }
    }
    tempPar = fMsrHandler->GetMsrRunList()->at(i).GetLifetimeParamNo();
    if (tempPar > 0) {
      // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
      for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
        if (tempPar == fMsrHandler->GetMsrParamList()->at(l).fNo) {
          fMsrHandler->GetMsrRunList()->at(i).SetLifetimeParamNo(l + 1);
          break;
        }
      }
      if (l > fMsrHandler->GetMsrParamList()->size()) {
        cerr << endl << ">> msr2data: **ERROR** The lifetime parameter specified in RUN block " << i + 1 << " does not exist!";
        cerr << endl << ">> msr2data: **ERROR** Please report a bug - file checks should have failed earlier!";
        cerr << endl;
        return false;
      }
      if (fMsrHandler->GetMsrParamList()->at(l).fIsGlobal) { // lifetime of RUN block i is global
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(lifetime, 1);
      } else {
        fMsrHandler->GetMsrRunList()->at(i).SetParGlobal(lifetime, 0);
      }
    }
    tempMap = fMsrHandler->GetMsrRunList()->at(i).GetMap();
    for (unsigned int j(0); j < tempMap->size(); ++j) {
      tempPar = (*tempMap)[j];
      if (tempPar > 0) {
        // go through the whole parameter list and look for the correct parameter - this is stupid, however, I do not know what else to do
        for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
          if (tempPar == fMsrHandler->GetMsrParamList()->at(l).fNo) {
            fMsrHandler->GetMsrRunList()->at(i).SetMap(l + 1, j);
            break;
          }
        }
        if (fMsrHandler->GetMsrParamList()->at((*tempMap)[j] - 1).fIsGlobal) { // map j of RUN block i is global
          fMsrHandler->GetMsrRunList()->at(i).SetMapGlobal(j, 1);
        } else {
          fMsrHandler->GetMsrRunList()->at(i).SetMapGlobal(j, 0);
        }
      }
    }
  }
  tempMap = 0;

  // finally fix the PARAMETER block after the reorganization
  for (l = 0; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
    fMsrHandler->GetMsrParamList()->at(l).fNo = l + 1;
  }

  // now go through the specified runs and add run-specific parameters and RUN blocks

  map<UInt_t, TString> commentsP, commentsR;
  TString tempTstr;
  ostringstream newRunNumber;
  newRunNumber.fill('0');
  newRunNumber.setf(ios::internal, ios::adjustfield);
  newRunNumber.width(N);
  newRunNumber << *fRunVectorIter;

  //cout << "Number of run specific parameters: " << fNumSpecParam << endl;

  if (newRunNumber.str().compare(tempRunNumber.str())) { // first run number does not match the template run number
    // substitute the template run-numbers in the parameter names
    for (l = fNumGlobalParam; l < fMsrHandler->GetMsrParamList()->size(); ++l) {
      tempParamName = fMsrHandler->GetMsrParamList()->at(l).fName.Data();
      string::size_type loc = tempParamName.rfind(tempRunNumber.str());
      if ( loc != string::npos ) {
        tempParamName.replace(loc, N, newRunNumber.str());
        fMsrHandler->GetMsrParamList()->at(l).fName = tempParamName;
      } else {
        cerr << endl << ">> msr2data: **ERROR** The indices of the run specific parameters do not match the template run number!";
        cerr << endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
        cerr << endl;
      }
    }
    // substitute the template run-numbers in the RUN names
    for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
      tempRunName = fMsrHandler->GetMsrRunList()->at(i).GetRunName()->Data();
      string::size_type loc = tempRunName.rfind(tempRunNumber.str());
      if ( loc != string::npos ) {
        tempRunName.replace(loc, N, newRunNumber.str());
        tempTstr = TString(tempRunName);
        fMsrHandler->GetMsrRunList()->at(i).SetRunName(tempTstr, 0);
      } else {
        cerr << endl << ">> msr2data: **ERROR** A template run file number does not match the \"file index\"";
        cerr << endl << ">> msr2data: **ERROR** Please check the template file!";
        cerr << endl;
        return false;
      }
    }
  }

  ostringstream tempStrStr;

  // comments for the output-file
  if (fNumGlobalParam) {
    commentsP[1] = TString("Global parameters for all runs");
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
  map<TString, Int_t> *runParGlobal(0);
  map<TString, Int_t>::iterator iter;
  PIntVector *runMapGlobal(0);

  while (fRunVectorIter != fRunVector.end()) {
    tempRunNumber.str(newRunNumber.str());
    newRunNumber.str("");
    newRunNumber.clear();
    newRunNumber.fill('0');
    newRunNumber.setf(ios::internal, ios::adjustfield);
    newRunNumber.width(N);
    newRunNumber << *fRunVectorIter;

    // add parameters for each run
    for (l = 0; l < fNumSpecParam; ++l) {
      fMsrHandler->GetMsrParamList()->push_back(fMsrHandler->GetMsrParamList()->at(fNumGlobalParam + runcounter*fNumSpecParam + l));
      tempParamName = fMsrHandler->GetMsrParamList()->back().fName.Data();
      string::size_type loc = tempParamName.rfind(tempRunNumber.str());
      if ( loc != string::npos ) {
        tempParamName.replace(loc, N, newRunNumber.str());
        fMsrHandler->GetMsrParamList()->back().fName = tempParamName;
        fMsrHandler->GetMsrParamList()->back().fNo += fNumSpecParam;
      } else {
        cerr << endl << ">> msr2data: **ERROR** Something went wrong when appending new parameters!";
        cerr << endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
        cerr << endl;
      }
    }

    // add RUN blocks for each run
    for (unsigned int i(0); i < fNumTempRunBlocks; ++i) {
      fMsrHandler->GetMsrRunList()->push_back(fMsrHandler->GetMsrRunList()->at(runcounter*fNumTempRunBlocks + i));
      tempRunName = fMsrHandler->GetMsrRunList()->back().GetRunName()->Data();
      string::size_type loc = tempRunName.rfind(tempRunNumber.str());
      if ( loc != string::npos ) {
        tempRunName.replace(loc, N, newRunNumber.str());
        tempTstr = TString(tempRunName);
        fMsrHandler->GetMsrRunList()->back().SetRunName(tempTstr, 0);
      } else {
        cerr << endl << ">> msr2data: **ERROR** Something went wrong when appending new RUN blocks!";
        cerr << endl << ">> msr2data: **ERROR** This should not happen! Please report a bug!";
        cerr << endl;
        return false;
      }
      // change run specific parameter numbers in the new RUN block
      runParGlobal = fMsrHandler->GetMsrRunList()->back().GetParGlobal();
      iter = runParGlobal->find(norm);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          fMsrHandler->GetMsrRunList()->back().SetNormParamNo(fMsrHandler->GetMsrRunList()->back().GetNormParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(bkgfit);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          fMsrHandler->GetMsrRunList()->back().SetBkgFitParamNo(fMsrHandler->GetMsrRunList()->back().GetBkgFitParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(alpha);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          fMsrHandler->GetMsrRunList()->back().SetAlphaParamNo(fMsrHandler->GetMsrRunList()->back().GetAlphaParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(beta);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          fMsrHandler->GetMsrRunList()->back().SetBetaParamNo(fMsrHandler->GetMsrRunList()->back().GetBetaParamNo() + fNumSpecParam);
      }
      iter = runParGlobal->find(lifetime);
      if (iter != runParGlobal->end()) {
        if (!iter->second)
          fMsrHandler->GetMsrRunList()->back().SetLifetimeParamNo(fMsrHandler->GetMsrRunList()->back().GetLifetimeParamNo() + fNumSpecParam);
      }
      runMapGlobal = fMsrHandler->GetMsrRunList()->back().GetMapGlobal();
      for (l = 0; l < runMapGlobal->size(); ++l) {
        if (!(*runMapGlobal)[l])
           fMsrHandler->GetMsrRunList()->back().SetMap(fMsrHandler->GetMsrRunList()->back().GetMap()->at(l) + fNumSpecParam, l);
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

   // write the global msr-file
   status = fMsrHandler->WriteMsrFile(msrOutFile.c_str(), &commentsP, 0, 0, &commentsR);

   // set back the run-iterator to the start
   fRunVectorIter = fRunVector.begin();

//   in.close();
//   out.close();
   return true;
}

//-------------------------------------------------------------
/**
 * <p> 
 */
int PMsr2Data::WriteOutput(const string &outfile, bool db, bool withHeader, bool global, unsigned int counter) const
{
  if (!to_lower_copy(outfile).compare("none")) {
    fRunVectorIter++;
    return PMUSR_SUCCESS;
  }

  const unsigned int N(4);

  ostringstream curRunNumber;
  curRunNumber.fill('0');
  curRunNumber.setf(ios::internal, ios::adjustfield);
  curRunNumber.width(N);
  curRunNumber << *fRunVectorIter;

  string msrTitle(fMsrHandler->GetMsrTitle()->Data());
  string msrFileName(fMsrHandler->GetFileName().Data());
  unsigned int msrNoOfParams(fMsrHandler->GetNoOfParams());
  PMsrParamList *msrParamList(fMsrHandler->GetMsrParamList());
  PMsrRunList *msrRunList(fMsrHandler->GetMsrRunList());

  if (global && fRunVectorIter == fRunVector.begin()) {
    // since the DB-ASCII-output is in principle independent of the original msr-file-generation
    // the number of global and run specific parameters as well as the number of RUN blocks per run number have to be determined again
    // in case no msr-file has been created before.
    if (!fNumGlobalParam && !fNumSpecParam && !fNumTempRunBlocks) { // if not all parameters are zero they have been determined before
      ostringstream tempRunNumber;
      tempRunNumber.fill('0');
      tempRunNumber.setf(ios::internal, ios::adjustfield);
      tempRunNumber.width(N);
      tempRunNumber << fRunVector.front();

      // search parameter list for run-specific parameters
      string tempName;
      for (unsigned int i(0); i < msrParamList->size(); ++i) {
        tempName = msrParamList->at(i).fName.Data();
        string::size_type loc = tempName.rfind(tempRunNumber.str());
        if ((tempName.length() > N) && (loc == tempName.length() - N)) {
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
        string::size_type loc = tempName.rfind(tempRunNumber.str());
        if ( loc != string::npos ) {
          ++fNumTempRunBlocks;
        }
      }
      cerr << endl << ">> msr2data: **WARNING** At the moment the full integrity of the global msr file cannot be checked!";
      cerr << endl << ">> msr2data: **WARNING** It should therefore be checked carefully that the run order is consistent";
      cerr << endl << ">> msr2data: **WARNING** in the specified run list, the parameters and the RUN blocks in the msr file!";
      cerr << endl << ">> msr2data: **WARNING** A simple check for the first specified run yields:";
      cerr << endl << ">> msr2data: **WARNING** Number of global parameters: " << fNumGlobalParam;
      cerr << endl << ">> msr2data: **WARNING** Number of run specific parameters: " << fNumSpecParam;
      cerr << endl << ">> msr2data: **WARNING** Number of RUN blocks per run number: " << fNumTempRunBlocks;
      cerr << endl;
    }
    // Two more simple consistency checks
    bool okP(true), okR(true);
    if ((msrParamList->size() - fNumGlobalParam)%fNumSpecParam) {
      okP = false;
    } else if ((msrParamList->size() - fNumGlobalParam)/fNumSpecParam != fRunVector.size()) {
      okP = false;
    }

    if (!okP) {
      cerr << endl << ">> msr2data: **ERROR** The number of parameters is not consistent with the specified run list!";
      cerr << endl << ">> msr2data: **ERROR** Please check carefully the integrity of the msr file!";
      cerr << endl << ">> msr2data: **ERROR** No output will be written!";
      cerr << endl;
      return -1;
    }

    if (msrRunList->size()%fNumTempRunBlocks) {
      okR = false;
    } else if (msrRunList->size()/fNumTempRunBlocks != fRunVector.size()) {
      okR = false;
    }

    if (!okR) {
      cerr << endl << ">> msr2data: **ERROR** The number of RUN blocks is not consistent with the specified run list!";
      cerr << endl << ">> msr2data: **ERROR** Please check carefully the integrity of the msr file!";
      cerr << endl << ">> msr2data: **ERROR** No output will be written!";
      cerr << endl;
      return -1;
    }
  }

  vector<string> dataParamNames;
  vector<string> dataParamLabels;
  vector<double> dataParam, dataParamErr;

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
        ostringstream oss;
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
      dataParamLabels.push_back("mu0 H (G)");
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
      dataParamNames.push_back("dataRALRAR");
      dataParamLabels.push_back("RAL-RAR (kV)");
      dataParam.push_back(ra[0]-ra[1]);
    }
    if (ra.size() == 4) {
      dataParamNames.push_back("dataRATRAB");
      dataParamLabels.push_back("RAT-RAB (kV)");
      dataParam.push_back(ra[2]-ra[3]);
    }
    rawRunData = 0;
  }

// get the independent variable values from the runlist file if needed
  PDoubleVector indVarValues;

  if (fRunListFile) {
    string line;
    vector<string> splitVec;
    unsigned int runNo;
    double val;
    istringstream strLine;

    while (getline(*fRunListFileStream, line)) {
      trim(line);
      if (line.empty())
        continue;
      else if (line.at(0) == '#' || !line.substr(0,3).compare("RUN"))
        continue;
      else {
        split( splitVec, line, is_any_of("#") ); // split the string if any comments appear on the line
        strLine.clear();
        strLine.str(splitVec[0]);
        strLine >> runNo;
        if (runNo != *fRunVectorIter) {
          cerr << endl << ">> msr2data: **ERROR** The run number in the runlist file does not match the one which should be processed...";
          cerr << endl << ">> msr2data: **ERROR** Something is very strange... Please report this bug!";
          cerr << endl;
          fRunVectorIter = fRunVector.end();
          return -1;
        }
        while (strLine >> val) {
          indVarValues.push_back(val);
        }
        if (indVarValues.size() != fIndVar.size()) {
          cerr << endl << ">> msr2data: **ERROR** The number of data entries in the runlist file for the run number " << runNo;
          cerr << endl << ">> msr2data: **ERROR** does not match the number of labels given in the RUN-line! Please check the file!";
          cerr << endl;
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
    cerr << endl << ">> msr2data: **WARNING** The fit of run " << *fRunVectorIter << " has not converged!";
    cerr << endl << ">> msr2data: **WARNING** Its parameter data have not been appended to the output file " << outfile;
    cerr << endl;
    fRunVectorIter++;

    delete fMsrHandler;
    fMsrHandler = 0;

    if (fDataHandler) {
      delete fDataHandler;
      fDataHandler = 0;
    }

// clean up some vectors
    dataParamNames.clear();
    dataParamLabels.clear();
    dataParam.clear();
    dataParamErr.clear();
    indVarValues.clear();

    return -1;
  }

// open the DB or dat file and write the data

  ofstream outFile(outfile.c_str(), ios::app);
  if (!outFile) {
    cerr << endl << ">> msr2data: **ERROR** The output file " << outfile << " cannot be opened! Please check!";
    cerr << endl;
    fRunVectorIter = fRunVector.end();
    return -1;
  }

  if (db) {

    if (withHeader && (fRunVectorIter == fRunVector.begin())) {
      outFile << "TITLE" << endl;
      outFile << ">>>Put your title here<<<" << endl << endl;
      outFile << "Abstract" << endl;
      outFile << ">>>Put your abstract here<<<" << endl << endl;
      outFile << "LABELS" << endl;

      if (fDataHandler) {
        for (unsigned int i(0); i < dataParamLabels.size(); ++i) {
         outFile << dataParamLabels[i] << endl;
        }
      }

      if (fRunListFile) {
        for (unsigned int i(0); i < fIndVar.size(); ++i) {
          outFile << fIndVar[i] << endl;
        }
      }

      // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
      if (global) {
        string tempName;
        for (unsigned int i(0); i < fNumGlobalParam; ++i) {
          outFile << (*msrParamList)[i].fName.Data() << endl;
        }
        for (unsigned int i(0); i < fNumSpecParam; ++i) {
          tempName = (*msrParamList)[fNumGlobalParam + fNumSpecParam*counter + i].fName.Data();
          string::size_type loc = tempName.rfind(curRunNumber.str());
          if (loc == tempName.length() - N) {
            outFile << tempName.substr(0, loc) << endl;
          } else {
            cerr << endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
            cerr << endl << ">> msr2data: **ERROR** The output will be flawed!";
            cerr << endl;
          }
        }
      } else {
        for (unsigned int i(0); i < msrNoOfParams; ++i) {
          outFile << (*msrParamList)[i].fName.Data() << endl;
        }
      }

      if (msrStatistic->fChisq)
        outFile << "CHISQ" << endl;
      else
        outFile << "maxLH" << endl;

      outFile << "NDF" << endl;

      if (msrStatistic->fChisq)
        outFile << "CHISQred" << endl;
      else
        outFile << "maxLHred" << endl;

      outFile << "RUN" << endl;

      outFile << endl << "Data";

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
        string tempName;
        for (unsigned int i(0); i < fNumGlobalParam; ++i) {
          outFile << " " << (*msrParamList)[i].fName.Data();
        }
        for (unsigned int i(0); i < fNumSpecParam; ++i) {
          tempName = (*msrParamList)[fNumGlobalParam + fNumSpecParam*counter + i].fName.Data();
          string::size_type loc = tempName.rfind(curRunNumber.str());
          if (loc == tempName.length() - N) {
            outFile << " " << tempName.substr(0, loc);
          } else {
            cerr << endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
            cerr << endl << ">> msr2data: **ERROR** The output will be flawed!";
            cerr << endl;
          }
        }
      } else {
        for (unsigned int i(0); i < msrNoOfParams; ++i) {
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

      outFile << " " << "RUN" << endl;

      outFile << "\\-e" << endl;
    }

    if (fDataHandler) {
      for (unsigned int i(0); i < dataParam.size(); ++i) {
        if (i < dataParamErr.size())
          outFile << dataParamNames[i] << " = " << dataParam[i] << ", " \
          << dataParamErr[i] << ", " << dataParamErr[i] << ",\\" << endl;
        else
          outFile << dataParamNames[i] << " = " << dataParam[i] << ", 0, 0,\\" << endl;
      }
    }

    if (fRunListFile) {
      for (unsigned int i(0); i < indVarValues.size(); ++i) {
        outFile << fIndVar[i] << " = " << indVarValues[i] << ", 0, 0,\\" << endl;
      }
    }

    // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
    if (global) {
      string tempName;
      unsigned int idx;
      for (unsigned int i(0); i < fNumGlobalParam; ++i) {
        outFile << (*msrParamList)[i].fName.Data() << " = " << (*msrParamList)[i].fValue << ", ";
        if ((*msrParamList)[i].fPosErrorPresent)
          outFile << (*msrParamList)[i].fPosError << ", ";
        else
          outFile << fabs((*msrParamList)[i].fStep) << ", ";
        outFile << fabs((*msrParamList)[i].fStep) << ",\\" << endl;
      }
      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        idx = fNumGlobalParam + fNumSpecParam*counter + i;
        tempName = (*msrParamList)[idx].fName.Data();
        string::size_type loc = tempName.rfind(curRunNumber.str());
        if (loc == tempName.length() - N) {
          outFile << tempName.substr(0, loc) << " = " << (*msrParamList)[idx].fValue << ", ";
          if ((*msrParamList)[idx].fPosErrorPresent)
            outFile << (*msrParamList)[idx].fPosError << ", ";
          else
            outFile << fabs((*msrParamList)[idx].fStep) << ", ";
          outFile << fabs((*msrParamList)[idx].fStep) << ",\\" << endl;
        }
      }
    } else {
      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        outFile << (*msrParamList)[i].fName.Data() << " = " << (*msrParamList)[i].fValue << ", ";
        if ((*msrParamList)[i].fPosErrorPresent)
          outFile << (*msrParamList)[i].fPosError << ", ";
        else
          outFile << fabs((*msrParamList)[i].fStep) << ", ";
        outFile << fabs((*msrParamList)[i].fStep) << ",\\" << endl;
      }
    }

    if (msrStatistic->fChisq)
      outFile << "CHISQ = " << msrStatistic->fMin << ", 0, 0,\\" << endl;
    else
      outFile << "maxLH = " << msrStatistic->fMin << ", 0, 0,\\" << endl;

    outFile << "NDF = " << msrStatistic->fNdf << ", 0, 0,\\" << endl;

    if (msrStatistic->fChisq)
      outFile << "CHISQred = " << msrStatistic->fMin/msrStatistic->fNdf << ", 0, 0,\\" << endl;
    else
      outFile << "maxLHred = " << msrStatistic->fMin/msrStatistic->fNdf << ", 0, 0,\\" << endl;

    outFile << *fRunVectorIter << ",,, " << msrTitle << endl;

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
    string s;
    if (global) {
      for (unsigned int i(0); i < fNumGlobalParam; ++i) {
        s = (*msrParamList)[i].fName.Data();
        length = s.length();
        if (length > maxlength)
          maxlength = length;
      }
      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        s = (*msrParamList)[fNumGlobalParam + fNumSpecParam*counter + i].fName.Data();
        string::size_type loc = s.rfind(curRunNumber.str());
        if (loc == s.length() - N) {
          length = s.length() - N;
          if (length > maxlength)
            maxlength = length;
        } else {
          cerr << endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
          cerr << endl << ">> msr2data: **ERROR** The output will be flawed!";
          cerr << endl;
        }
      }
    } else {
      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        s = (*msrParamList)[i].fName.Data();
        length = s.length();
        if (length > maxlength)
          maxlength = length;
      }
    }
    if (maxlength < 13)
      maxlength = 13; // will use a minimum field width of 13 which corresponds to: -1.23456e-07 + ' '
    else
      maxlength += 1; // maximum length of parameter names + ' '

    if (withHeader && (fRunVectorIter == fRunVector.begin())) {

      if (fDataHandler) {
        for (unsigned int i(0); i < dataParamNames.size(); ++i) {
          s = dataParamNames[i];
          if (i < dataParamErr.size()) {
            outFile << setw(maxlength) << left << s << setw(maxlength + 3) << left << s + "Err";
          } else
            outFile << setw(maxlength) << left << s;
        }
      }

      if (fRunListFile) {
        for (unsigned int i(0); i < fIndVar.size(); ++i) {
          outFile << setw(maxlength) << left << fIndVar[i];
        }
      }

      // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
      if (global) {
        for (unsigned int i(0); i < fNumGlobalParam; ++i) {
          s = (*msrParamList)[i].fName.Data();
          outFile << setw(maxlength) << left << s \
                << setw(maxlength + 6) << left << s + "PosErr" \
                << setw(maxlength + 6) << left << s + "NegErr";
        }
        for (unsigned int i(0); i < fNumSpecParam; ++i) {
          s = (*msrParamList)[fNumGlobalParam + fNumSpecParam*counter + i].fName.Data();
          string::size_type loc = s.rfind(curRunNumber.str());
          if (loc == s.length() - N) {
            s = s.substr(0, loc);
            outFile << setw(maxlength) << left << s \
                    << setw(maxlength + 6) << left << s + "PosErr" \
                    << setw(maxlength + 6) << left << s + "NegErr";
          } else {
            cerr << endl << ">> msr2data: **ERROR** The run index of some parameter does not match the run number being processed!";
            cerr << endl << ">> msr2data: **ERROR** The output will be flawed!";
            cerr << endl;
          }
        }
      } else {
        for (unsigned int i(0); i < msrNoOfParams; ++i) {
          s = (*msrParamList)[i].fName.Data();
          outFile << setw(maxlength) << left << s \
                  << setw(maxlength + 6) << left << s + "PosErr" \
                  << setw(maxlength + 6) << left << s + "NegErr";
        }
      }
      s.clear();

      if (msrStatistic->fChisq)
        outFile << setw(maxlength) << left << "CHISQ";
      else
        outFile << setw(maxlength) << left << "maxLH";

      outFile << setw(maxlength) << left << "NDF";

      if (msrStatistic->fChisq)
        outFile << setw(maxlength) << left << "CHISQred";
      else
        outFile << setw(maxlength) << left << "maxLHred";

      outFile << setw(maxlength) << left << "RUN" << endl;
    }

    if (fDataHandler) {
      for (unsigned int i(0); i < dataParam.size(); ++i) {
        if (i < dataParamErr.size()) {
          writeValues(outFile, dataParam[i], maxlength);
          writeValues(outFile, dataParamErr[i], maxlength + 3);
        } else {
          writeValues(outFile, dataParam[i], maxlength);
        }
      }
    }

    if (fRunListFile) {
      for (unsigned int i(0); i < indVarValues.size(); ++i) {
        writeValues(outFile, indVarValues[i], maxlength);
      }
    }

    // in the GLOBAL mode write only global parameters and those which belong to the actual run - in the NORMAL mode write all parameters
    if (global) {
      for (unsigned int i(0); i < fNumGlobalParam; ++i) {
        writeValues(outFile, (*msrParamList)[i].fValue, maxlength);

        if ((*msrParamList)[i].fPosErrorPresent)
          writeValues(outFile, (*msrParamList)[i].fPosError, maxlength + 6);
        else
          writeValues(outFile, fabs((*msrParamList)[i].fStep), maxlength + 6);

        writeValues(outFile, fabs((*msrParamList)[i].fStep), maxlength + 6);
      }
      unsigned int idx;
      for (unsigned int i(0); i < fNumSpecParam; ++i) {
        idx = fNumGlobalParam + fNumSpecParam*counter + i;
        writeValues(outFile, (*msrParamList)[idx].fValue, maxlength);

        if ((*msrParamList)[idx].fPosErrorPresent)
          writeValues(outFile, (*msrParamList)[idx].fPosError, maxlength + 6);
        else
          writeValues(outFile, fabs((*msrParamList)[idx].fStep), maxlength + 6);

        writeValues(outFile, fabs((*msrParamList)[idx].fStep), maxlength + 6);
      }
    } else {
      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        writeValues(outFile, (*msrParamList)[i].fValue, maxlength);

        if ((*msrParamList)[i].fPosErrorPresent)
          writeValues(outFile, (*msrParamList)[i].fPosError, maxlength + 6);
        else
          writeValues(outFile, fabs((*msrParamList)[i].fStep), maxlength + 6);

        writeValues(outFile, fabs((*msrParamList)[i].fStep), maxlength + 6);
      }
    }

    writeValues(outFile, msrStatistic->fMin, maxlength);

    writeValues(outFile, msrStatistic->fNdf, maxlength);

    writeValues(outFile, msrStatistic->fMin/msrStatistic->fNdf, maxlength);

    writeValues(outFile, *fRunVectorIter, maxlength);
    outFile << endl;

  }

  if (global) {
    cout << endl << ">> msr2data: **INFO** Parameter data of run " << *fRunVectorIter << " of file " << msrFileName \
                 << " have been appended to " << outfile << endl;
  } else {
    cout << endl << ">> msr2data: **INFO** Parameter data of file " << msrFileName << " have been appended to " << outfile << endl;
  }

  fRunVectorIter++;

  if (fRunVectorIter == fRunVector.end())
    outFile << endl << endl;
  outFile.close();

  if (!global || (fRunVectorIter == fRunVector.end())) {
    delete fMsrHandler;
    fMsrHandler = 0;

    if (fDataHandler) {
      delete fDataHandler;
      fDataHandler = 0;
    }
  }

  msrParamList = 0;
  msrRunList = 0;

// clean up some vectors
  dataParamNames.clear();
  dataParamLabels.clear();
  dataParam.clear();
  dataParamErr.clear();
  indVarValues.clear();

  return PMUSR_SUCCESS;

}


