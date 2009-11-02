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
using namespace std;

#include <boost/algorithm/string/trim.hpp>  // for stripping leading whitespace in std::string 
#include <boost/algorithm/string/case_conv.hpp> // for to_lower() in std::string
#include <boost/algorithm/string/split.hpp> // split strings at certain characters
using namespace boost::algorithm;

#include "PMsr2Data.h"

//-------------------------------------------------------------
/**
 * <p> Write formatted output to file
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
PMsr2Data::PMsr2Data(const string &ext) : fFileExtension(ext), fRunListFile(false)
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
int PMsr2Data::ReadMsrFile(const string &infile)
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

  const int N(4); // number of digits for the runnumber

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
void PMsr2Data::WriteOutput(const string &outfile, bool db, bool withHeader) const
{
  if (!to_lower_copy(outfile).compare("none")) {
    fRunVectorIter++;
    return;
  }

  string msrTitle(fMsrHandler->GetMsrTitle()->Data());
  string msrFileName(fMsrHandler->GetFileName().Data());
  unsigned int msrNoOfParams(fMsrHandler->GetNoOfParams());
  PMsrParamList *msrParamList(fMsrHandler->GetMsrParamList());

  vector<string> dataParamNames;
  vector<string> dataParamLabels;
  vector<double> dataParam, dataParamErr;

  if (fDataHandler) {
    PMsrRunList *msrRunList(fMsrHandler->GetMsrRunList());
    PRawRunData *rawRunData(fDataHandler->GetRunData((*msrRunList)[0].GetRunName()->Data()));

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
          return;
        }
        while (strLine >> val) {
          indVarValues.push_back(val);
        }
        if (indVarValues.size() != fIndVar.size()) {
          cerr << endl << ">> msr2data: **ERROR** The number of data entries in the runlist file for the run number " << runNo;
          cerr << endl << ">> msr2data: **ERROR** does not match the number of labels given in the RUN-line! Please check the file!";
          cerr << endl;
          fRunVectorIter = fRunVector.end();
          return;
        }
        break;
      }
    }
  }

// The RUNLIST file stream and the run vector iterator might get out of synchronization, if the following check is placed before the above block...
  PMsrStatisticStructure *msrStatistic(fMsrHandler->GetMsrStatistic());
  if (!msrStatistic->fValid) {
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

    return;
  }

// open the DB or dat file and write the data

  ofstream outFile(outfile.c_str(), ios::app);
  if (!outFile) {
    cerr << endl << ">> msr2data: **ERROR** The output file " << outfile << " cannot be opened! Please check!";
    cerr << endl;
    fRunVectorIter = fRunVector.end();
    return;
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

      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        outFile << (*msrParamList)[i].fName.Data() << endl;
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

      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        outFile << " " << (*msrParamList)[i].fName.Data();
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

    for (unsigned int i(0); i < msrNoOfParams; ++i) {
      outFile << (*msrParamList)[i].fName.Data() << " = " << (*msrParamList)[i].fValue << ", ";
      if ((*msrParamList)[i].fPosErrorPresent)
        outFile << (*msrParamList)[i].fPosError << ", ";
      else
        outFile << fabs((*msrParamList)[i].fStep) << ", ";
      outFile << fabs((*msrParamList)[i].fStep) << ",\\" << endl;
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
    string s;
    for (unsigned int i(0); i < msrNoOfParams; ++i) {
      s = (*msrParamList)[i].fName.Data();
      length = s.length();
      if (length > maxlength)
        maxlength = length;
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

      for (unsigned int i(0); i < msrNoOfParams; ++i) {
        s = (*msrParamList)[i].fName.Data();
        outFile << setw(maxlength) << left << s \
                << setw(maxlength + 6) << left << s + "PosErr" \
                << setw(maxlength + 6) << left << s + "NegErr";
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

    for (unsigned int i(0); i < msrNoOfParams; ++i) {
      writeValues(outFile, (*msrParamList)[i].fValue, maxlength);

      if ((*msrParamList)[i].fPosErrorPresent)
        writeValues(outFile, (*msrParamList)[i].fPosError, maxlength + 6);
      else
        writeValues(outFile, fabs((*msrParamList)[i].fStep), maxlength + 6);

      writeValues(outFile, fabs((*msrParamList)[i].fStep), maxlength + 6);
    }

    writeValues(outFile, msrStatistic->fMin, maxlength);

    writeValues(outFile, msrStatistic->fNdf, maxlength);

    writeValues(outFile, msrStatistic->fMin/msrStatistic->fNdf, maxlength);

    writeValues(outFile, *fRunVectorIter, maxlength);
    outFile << endl;

  }

  cout << endl << ">> msr2data: **INFO** Parameter data of file " << msrFileName << " have been appended to " << outfile << endl;

  fRunVectorIter++;

  if (fRunVectorIter == fRunVector.end())
    outFile << endl << endl;
  outFile.close();

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

  return;

}

