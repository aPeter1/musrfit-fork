/***************************************************************************

  msr2data.cpp

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

#include "PMsr2Data.h"

#include <algorithm>
#include <sstream>
#include <iostream>
using namespace std;

#include <boost/algorithm/string/case_conv.hpp> // for to_lower() in std::string
using namespace boost::algorithm;

//--------------------------------------------------------------------------
/**
 * <p>
 * std::string
 */
bool isNumber(const string &s)
{
  unsigned int number;
  istringstream iss;
  ostringstream oss;
  iss.str(s);
  iss >> number;
  oss << number;
  if (iss.str().compare(oss.str()))
    return false;
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
void msr2data_syntax()
{
  cout << endl << "usage 1: msr2data <run> <extension> [-o<outputfile>] [data] [noheader] [nosummary]";
  cout << endl << "                  [fit [-k] [-t] | fit-<template>[!] [-k] [-t] | msr-<template>]";
  cout << endl << "usage 2: msr2data <run1> <run2> <extension> [-o<outputfile>] [data] [noheader] [nosummary]";
  cout << endl << "                  [fit [-k] [-t] | fit-<template>[!] [-k] [-t] | msr-<template>]";
  cout << endl << "usage 3: msr2data \\[<run1> <run2> ... <runN>\\] <extension> [-o<outputfile> ] [data] [noheader] [nosummary]";
  cout << endl << "                  [fit [-k] [-t] | fit-<template>[!] [-k] [-t] | msr-<template>]";
  cout << endl << "usage 4: msr2data <runlist> <extension> [-o<outputfile>] [data] [noheader] [nosummary]";
  cout << endl << "                  [fit [-k] [-t] | fit-<template>[!] [-k] [-t] | msr-<template>]";
  cout << endl;
  cout << endl << "       <run>, <run1>, <run2>, ... <runN> : run numbers";
  cout << endl << "       <extension> : msr-file extension, e.g. _tf_h13 for the file name 8472_tf_h13.msr";
  cout << endl << "       -o<outputfile> : specify the name of the DB or column data output file; default: out.db/out.dat";
  cout << endl << "       data : instead of to a DB file the data are written to a simple column structure";
  cout << endl << "       noheader : no file header is written to the output file";
  cout << endl << "       nosummary : no additional data from the run data file is written to the output file";
  cout << endl << "       fit : invoke musrfit to fit the specified runs";
  cout << endl << "              All msr input files are assumed to be present, none is newly generated!";
  cout << endl << "       fit-<template>! : generate msr-files for the runs to be processed from the <template>-run";
  cout << endl << "              and call musrfit for fitting these runs";
  cout << endl << "       fit-<template> : same as above, but the <template>-run is only used for the first file creation - ";
  cout << endl << "              the succeding files are generated using the musrfit-output from the preceding runs";
  cout << endl << "       msr-<template> : same as above without calling musrfit";
  cout << endl << "              In case any fitting-option is present, this option is ignored!";
  cout << endl << "       -k : if fitting is used, pass the option --keep-mn2-output to musrfit";
  cout << endl << "       -t : if fitting is used, pass the option --title-from-data-file to musrfit";
  cout << endl;
  cout << endl << "    For further information please refer to";
  cout << endl << "    https://wiki.intranet.psi.ch/MUSR/Msr2Data";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>
 * std::vector of std::string
 * bool (DB file ore not)
 */
string msr2data_outputfile(vector<string> &arg, bool db = true)
{
  string outputFile;
  if (db)
    outputFile = "out.db";
  else
    outputFile = "out.dat";

  vector<string>::iterator iterNext(arg.begin());
  for (vector<string>::iterator iter(arg.begin()); iter != arg.end(); iter++) {
    iterNext = iter + 1;
    if (!iter->substr(0,2).compare("-o")) {
      if (!iter->compare("-o")) {
        if ((iterNext != arg.end()) && (iterNext->compare("noheader")) && (iterNext->compare("nosummary")) \
            && (iterNext->substr(0,3).compare("fit")) && (iterNext->compare("-k")) && (iterNext->compare("-t")) \
            && (iterNext->compare("data")) && (iterNext->substr(0,3).compare("msr")) ) {
          outputFile = *iterNext;
          arg.erase(iterNext);
          arg.erase(iter);
          break;
        } else {
          cout << endl;
          cout << ">> msr2data: **WARNING** You did not specify an output file! The default one (" << outputFile << ") will be used." << endl;
          arg.erase(iter);
          break;
        }
      } else {
        outputFile = iter->substr(2);
        arg.erase(iter);
        break;
      }
    }
  }

  return outputFile;
}

//--------------------------------------------------------------------------
/**
 * <p>
 * std::vector of std::string
 * string
 */
bool msr2data_useOption(vector<string> &arg, const string &s)
{
  bool option(true);

  vector<string>::iterator iter;
  iter = find(arg.begin(), arg.end(), s);

  if (iter != arg.end()) {
    option = false;
    arg.erase(iter);
  }
  return option;
}

//--------------------------------------------------------------------------
/**
 * <p>
 * std::vector of std::string
 * bool
 */
int msr2data_doFitting(vector<string> &arg, bool &chainfit)
{
  int temp(0);

  string s;
  istringstream iss;
  vector<string>::iterator iter(arg.begin());
  while (iter != arg.end()) {
    if (!iter->compare("fit")) {
      if (temp) {
        return -2; // fatal error - another fit-<temp> option is specified
      }
      temp = -1;
      chainfit = false;
      iter = arg.erase(iter);
    }
    else if (!iter->substr(0,4).compare("fit-")) {
      if (temp) {
        return -2; // fatal error - another fit option is specified
      }
      s = iter->substr(4);
      string::size_type loc = s.rfind('!');
      if (loc != string::npos)
        chainfit = false;
      else
        chainfit = true;
      iss.str(s);
      iss >> temp;
      iter = arg.erase(iter);
    } else {
      iter++;
    }
  }

  return temp;
}

//--------------------------------------------------------------------------
/**
 * <p>
 * std::vector of std::string
 * bool
 */
unsigned int msr2data_doInputCreation(vector<string> &arg, bool &inputOnly)
{
  unsigned int temp(0);

  string s;
  istringstream iss;
  for (vector<string>::iterator iter(arg.begin()); iter != arg.end(); iter++) {
    if (!iter->substr(0,4).compare("msr-")) {
      s = iter->substr(4);
      iss.str(s);
      iss >> temp;
      arg.erase(iter);
      inputOnly = true;
      break;
    }
  }

  return temp;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param argc
 * \param argv
 */
int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc < 3) {
    msr2data_syntax();
    return 0;
  }

  // use a string-vector for the arguments to get rid of god damn char* as far as possible...
  vector<string> arg;
  for (int i(1); i<argc; i++) {
    arg.push_back(argv[i]);
  }

  unsigned int runTAG;
  vector<unsigned int> run_vec;
  string run_list;
  string msrExtension;

  if (arg[0].at(0) == '[') { // In case a list of runs is given by [...]
    runTAG = 1;

    unsigned int firstRunNumberInArg(0), lastRunNumberInArg(0);
    unsigned int rightbracket(999);

    if (!arg[0].compare("["))
      firstRunNumberInArg = 1;
    else
      arg[0]=arg[0].substr(1);

    for (unsigned int i(firstRunNumberInArg); i<arg.size(); i++) {
      string::size_type loc = arg[i].rfind(']');
      if ( loc != string::npos ) {
        rightbracket = i;
        if (!arg[i].compare("]") && i > 0)
          lastRunNumberInArg = i-1;
        else {
          arg[i]=arg[i].substr(0,loc);
          lastRunNumberInArg = i;
        }
        break;
      }
    }
    if (rightbracket == 999) {
       cout << endl;
       cout << ">> msr2data: **ERROR** You used the list specification without closing bracket (])! Quitting now." << endl;
       return 0;
    }

    for (unsigned int i(firstRunNumberInArg); i<=lastRunNumberInArg; ++i)
      run_vec.push_back(atoi(arg[i].c_str()));

    msrExtension = arg[rightbracket + 1];

    vector<string>::iterator iter(arg.begin());
    for (unsigned int i(0); i<rightbracket + 2; i++) {
      arg.erase(iter);
      iter = arg.begin();
    }

  } else {
    bool argOneIsNumber(isNumber(arg[0])), argTwoIsNumber(isNumber(arg[1]));

    if (argOneIsNumber && argTwoIsNumber) { // the first two arguments are numbers: an interval of numbers is given
      runTAG = 2;

      if (arg.size() < 3) {
        cout << endl;
        cout << ">> msr2data: **ERROR** No msr-file extension specified! Quitting now..." << endl;
        run_vec.clear();
        arg.clear();
        return 0;
      }

      run_vec.push_back(atoi(arg[0].c_str()));
      run_vec.push_back(atoi(arg[1].c_str()));

      msrExtension = arg[2];

      vector<string>::iterator iter(arg.begin());
      for (unsigned int i(0); i < 3; i++) {
        arg.erase(iter);
        iter = arg.begin();
      }
    } else if (argOneIsNumber && !argTwoIsNumber) { // only one run number is given
      runTAG = 3;
      run_vec.push_back(atoi(arg[0].c_str()));
      msrExtension = arg[1];

      vector<string>::iterator iter(arg.begin());
      for (unsigned int i(0); i < 2; i++) {
        arg.erase(iter);
        iter = arg.begin();
      }
    } else { // assume a runlist-file with "independent variables" is given
      runTAG = 4;
      run_list = arg[0];
      msrExtension = arg[1];

      vector<string>::iterator iter(arg.begin());
      for (unsigned int i(0); i < 2; i++) {
        arg.erase(iter);
        iter = arg.begin();
      }
    }
  }

// check if the output format is DB or data
  bool db(msr2data_useOption(arg, "data"));

// check the arguments for the "-o" option and set the output filename
  string outputFile(msr2data_outputfile(arg, db));

// introduce check, if no output should be generated - in that case we do not need msrfile and rundata handlers later
  bool realOutput(true);
  if (!to_lower_copy(outputFile).compare("none"))
    realOutput = false;

// create the msr2data-object and set the run numbers according to the runTAG above
  PMsr2Data msr2dataHandler(msrExtension);

  int status;

  switch(runTAG) {
    case 1:
      status = msr2dataHandler.SetRunNumbers(run_vec);
      break;
    case 2:
      status = msr2dataHandler.SetRunNumbers(run_vec[0], run_vec[1]);
      break;
    case 3:
      status = msr2dataHandler.SetRunNumbers(run_vec[0]);
      break;
    case 4:
      status = msr2dataHandler.SetRunNumbers(run_list);
      break;
    default:
      cout << endl;
      cout << ">> msr2data: **ERROR** None of the possible run list specifications has been detected! Quitting now..." << endl;
      run_vec.clear();
      arg.clear();
      return 0;
  }

  if (status == 1) {
    cout << endl;
    cout << ">> msr2data: **ERROR** The run numbers are out of range! Quitting..." << endl;
    run_vec.clear();
    arg.clear();
    return status;
  }

  run_vec.clear();

// check if fitting should be done and in case, which template run number to use
  int temp(0);
  bool chainfit(true), onlyInputCreation(false);
  string musrfitOptions;

  temp = msr2data_doFitting(arg, chainfit);

  if (temp == -2) {
    cout << endl;
    cout << ">> msr2data: **ERROR** More than one fitting options are specified! Quitting..." << endl;
    run_vec.clear();
    arg.clear();
    return status;
  }

// check if any options should be passed to musrfit
  if (temp) {
    if (!msr2data_useOption(arg, "-k"))
      musrfitOptions.append("-k ");
    if (!msr2data_useOption(arg, "-t"))
      musrfitOptions.append("-t ");
  }

// if no fitting should be done, check if only the input files should be created

  if(!temp) {
    temp = msr2data_doInputCreation(arg, onlyInputCreation);
    if (onlyInputCreation) {
      // if only input files should be created, do not write data to an output file (no matter, what has been determined earlier)
      realOutput = false;
      outputFile = "none";
    }
  }

  bool writeHeader(false), writeSummary(false);

  if (realOutput) {
// check the arguments for the "noheader" option
    writeHeader = msr2data_useOption(arg, "noheader");

// check the arguments for the "nosummary" option
    writeSummary = msr2data_useOption(arg, "nosummary");
  }

// read musrfit startup file
  if (writeSummary) {
    status = msr2dataHandler.ParseXmlStartupFile();
  }

// Processing the run list, do the fitting and write the data to the DB or data output file
  bool firstrun(true);
  unsigned int oldtemp(0); // should be accessed only when updated before...

  while (msr2dataHandler.GetPresentRun()) {
    ostringstream strInfile;
    strInfile << msr2dataHandler.GetPresentRun() << msrExtension << ".msr";

    // if fitting should be done, prepare a new input file
    if (temp) {
      if (temp > 0) {
        bool success(true);
        if (firstrun || !chainfit)
          success = msr2dataHandler.PrepareNewInputFile(temp);
        else
          success = msr2dataHandler.PrepareNewInputFile(oldtemp);
        if (firstrun)
          firstrun = false;
        oldtemp = msr2dataHandler.GetPresentRun();

        if (!success) {
          cout << endl << ">> msr2data: **ERROR** Input file generation has not been successful! Quitting..." << endl;
          arg.clear();
          return -1;
        }
      }

    // and do the fitting
      if (!onlyInputCreation) {
        ostringstream oss;
        oss << "musrfit" << " " << strInfile.str() << " " << musrfitOptions;
        cout << endl << ">> msr2data: **INFO** Calling " << oss.str() << endl;
        system(oss.str().c_str());
      }
    }

  // read msr-file
    if (realOutput) {
      status = msr2dataHandler.ReadMsrFile(strInfile.str());
      if (status != PMUSR_SUCCESS) {
        arg.clear();
        return status;
      }
    }

  // read data files
    if (writeSummary)
      writeSummary = msr2dataHandler.ReadRunDataFile();

  // write DB or dat file
    msr2dataHandler.WriteOutput(outputFile, db, writeHeader);

  }

  if (!arg.empty()) {
    cout << endl << ">> msr2data: **INFO** The following command line arguments have been specified but not been used: " << endl;
    cout << ">> msr2data: **INFO**";
    for (unsigned int i(0); i<arg.size(); i++)
      cout << " " << arg[i];
    cout << endl;
    arg.clear();
  }

  cout << endl << ">> msr2data: done ..." << endl;

  return 1;
}
