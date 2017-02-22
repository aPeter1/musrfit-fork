/***************************************************************************

  msr2data.cpp

  Author: Bastian M. Wojek / Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2014 by Bastian M. Wojek / Andreas Suter           *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "git-revision.h"
#include "PMusr.h"
#include "PMsr2Data.h"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
using namespace std;


#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp> // for to_lower() in std::string
using namespace boost::algorithm;

#include <boost/lexical_cast.hpp> // for atoi-replacement
// #include <boost/filesystem.hpp> // for a clean way to check if a file exists

//--------------------------------------------------------------------------
/**
 * <p>Checks if a string is an integer
 *
 * <p><b>return:</b>
 * - true if s is a number string
 * - false otherwise
 *
 * \param s string
 */
bool isNumber(const string &s)
{
  unsigned int number(0);
  try {
    number = boost::lexical_cast<unsigned int>(s);
    return true;
  }
  catch(boost::bad_lexical_cast &) {
    return false;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void msr2data_syntax()
{
  cout << endl << "usage 0: msr2data [--version] | [--help]";
  cout << endl << "usage 1: msr2data <run> <extension> options";
  cout << endl << "usage 2: msr2data <run1> <run2> <extension> options";
  cout << endl << "usage 3: msr2data \\[<runList>\\] <extension> options";
  cout << endl << "usage 4: msr2data <runListFileName> <extension> options";
  cout << endl;
  cout << endl << "       <runList> can be:";
  cout << endl << "         (i)   <run0>, <run1>, <run2>, ... <runN> : run numbers, e.g. 123 124";
  cout << endl << "         (ii)  <run0>-<runN> : a range, e.g. 123-125 -> 123 124 125";
  cout << endl << "         (iii) <run0>:<runN>:<step> : a sequence, e.g. 123:127:2 -> 123 125 127";
  cout << endl << "               <step> will give the step width and has to be a positive number!";
  cout << endl << "         a <runList> can also combine (i)-(iii), e.g. 123 128-130 133, etc.";
  cout << endl << "       <runListFileName> : an ASCII file containing a list of run numbers and optional";
  cout << endl << "                           external parameters is passed to msr2data. For details see";
  cout << endl << "                           the online documentation: http://lmu.web.psi.ch/musrfit/user/MUSR/Msr2Data.html";
  cout << endl << "       <extension> : msr-file extension, e.g. _tf_h13 for the file name 8472_tf_h13.msr";
  cout << endl;
  cout << endl << "options:";
  cout << endl << "       -o<outputfile> : specify the name of the DB or column-data output file; default: out.db/out.dat";
  cout << endl << "                        if the option '-o none' is used, no output file will be written.";
  cout << endl << "       new : before writing a new output file, delete the contents of any existing file with the same name";
  cout << endl << "       data : instead of to a DB file the data are written to a simple column structure";
  cout << endl << "       header : force writing of the file header to the output file";
  cout << endl << "       noheader : no file header is written to the output file";
  cout << endl << "              If either none or both of the header options are given, the file header will be written";
  cout << endl << "              if a new file is created, but not if the output file exists already!";
  cout << endl << "       nosummary : no additional data from the run data file is written to the output file";
  cout << endl << "       paramList <param> : option used to select the parameters which shall be exported.";
  cout << endl << "               <param> is a list of parameter numbers to be exported. Allowed lists are:";
  cout << endl << "               1-16 will export parameters 1 to 16. 1 3 5 will export parameters 1 3 5.";
  cout << endl << "               A combination of both is possible, e.g. 1-16 19 31 62, and so on.";
  cout << endl << "       fit : invoke musrfit to fit the specified runs";
  cout << endl << "              All msr input files are assumed to be present, none is newly generated!";
  cout << endl << "       fit-<template>! : generate msr files for the runs to be processed from the <template> run";
  cout << endl << "              and call musrfit for fitting these runs";
  cout << endl << "       fit-<template> : same as above, but the <template> run is only used for the first file creation---";
  cout << endl << "              the successive files are generated using the musrfit output from the preceding runs";
  cout << endl << "       msr-<template> : same as above without calling musrfit";
  cout << endl << "              In case any fitting option is present, this option is ignored!";
  cout << endl << "       -k, --keep-mn2-output : if fitting is used, pass the option --keep-mn2-output to musrfit";
  cout << endl << "       -t, --title-from-data-file : if fitting is used, pass the option --title-from-data-file to musrfit";
  cout << endl << "       -e, --estimateN0: estimate N0 for single histogram fits.";
  cout << endl << "       -p, --per-run-block-chisq: will per run block chisq to the msr-file.";
  cout << endl;
  cout << endl << "       global : switch on the global-fit mode";
  cout << endl << "              Within that mode all specified runs will be united in a single msr file!";
  cout << endl << "              The fit parameters can be either run specific or common to all runs.";
  cout << endl << "              For a complete description of this feature please refer to the manual.";
  cout << endl;
  cout << endl << "       global+[!] : operate in the global-fit mode, however, in case a global input file is created";
  cout << endl << "              all specified runs are pre-analyzed first one by one using the given template.";
  cout << endl << "              For the generation of the global input file, the run-specific parameter values are taken";
  cout << endl << "              from this pre-analysis for each run---they are not just copied from the template.";
  cout << endl << "              The specification of '!' determines which fit mode (see above) is used for this pre-analysis.";
  cout << endl;
  cout << endl << "    Typical examples:";
  cout << endl;
  cout << endl << "       msr2data 2047 2050 _tf_histo fit-2046";
  cout << endl << "          will use 2046_tf_histo.msr as templete, and subsequently generating 2047_tf_histo.msr until";
  cout << endl << "          2050_tf_histo.msr and fit them.";
  cout << endl;
  cout << endl << "       msr2data 2047 2050 _tf_histo msr-2046";
  cout << endl << "          will use 2046_tf_histo.msr as templete, and subsequently generating 2047_tf_histo.msr until";
  cout << endl << "          2050_tf_histo.msr, but NO fitting will be done.";
  cout << endl;
  cout << endl << "       msr2data 2046 2050 _tf_histo -o fitParam.db";
  cout << endl << "          will collect the fit parameters from runs 2046-2050 (msr-files 2046_tf_histo.msr etc.) and";
  cout << endl << "          write them to the file fitParam.db (DB-format).";
  cout << endl;
  cout << endl << "       msr2data [2047:2053:2 2056] _tf_histo fit-2045";
  cout << endl << "          will use 2045_tf_histo.msr as templete, and subsequently generating msr-files from the run-list:";
  cout << endl << "          2047 2049 2051 2053 2056 (2047_tf_histo.msr etc.) and fit them.";
  cout << endl;
  cout << endl << "       msr2data 2046 2058 _tf_histo paramList 1-12 data -o fitParam.dat";
  cout << endl << "          will export the parameters number 1 trough 12 in a column like fashion of the runs 2046 to 2058,";
  cout << endl << "          collected form the msr-files 2046_tf_histo.msr and so on.";
  cout << endl;
  cout << endl << "    For further information please refer to";
  cout << endl << "       http://lmu.web.psi.ch/musrfit/user/MUSR/Msr2Data.html";
  cout << endl << "       https://intranet.psi.ch/MUSR/Msr2Data";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>Cleaning up some used variables
 *
 * \param msr2dataHandler pointer to PMsr2Data object
 * \param run_vec vector containing the run numbers
 * \param arg vector containing the command line arguments
 *
 */
void msr2data_cleanup(PMsr2Data *msr2dataHandler, vector<unsigned int> &run_vec, vector<string> &arg)
{
  delete msr2dataHandler;
  msr2dataHandler = 0;
  run_vec.clear();
  arg.clear();
  return;
}

//--------------------------------------------------------------------------
/**
 * <p>Cleaning up some used variables
 *
 * \param msr2dataHandler pointer to PMsr2Data object
 * \param arg vector containing the command line arguments
 *
 */
void msr2data_cleanup(PMsr2Data *msr2dataHandler, vector<string> &arg)
{
  delete msr2dataHandler;
  msr2dataHandler = 0;
  arg.clear();
  return;
}

//--------------------------------------------------------------------------
/**
 * <p>Checks if only valid options appear in the argument list
 *
 * <p><b>return:</b>
 * - empty string if everything is fine
 * - first found wrong argument in case of an error
 *
 * \param arg list of arguments
 *
 */
string msr2data_validArguments(const vector<string> &arg)
{
  string word;

  for (vector<string>::const_iterator iter(arg.begin()); iter != arg.end(); ++iter) {
    if ( (!iter->compare("header")) || (!iter->compare("noheader")) || (!iter->compare("nosummary")) \
      || (!iter->substr(0,3).compare("fit")) || (!iter->compare("-k")) || (!iter->compare("--keep-mn2-output")) \
      || (!iter->compare("-t")) || (!iter->compare("--title-from-data-file")) \
      || (!iter->compare("-e")) || (!iter->compare("--estimateN0")) \
      || (!iter->compare("-p")) || (!iter->compare("--per-run-block-chisq")) \
      || (!iter->compare("data")) || (!iter->substr(0,4).compare("msr-")) || (!iter->compare("global")) \
      || (!iter->compare("global+")) || (!iter->compare("global+!")) || (!iter->compare("new")) \
      || !iter->compare("paramList") )
      word.clear();
    else if (!iter->substr(0,2).compare("-o")) {
      word.clear();
      if (!iter->compare("-o")) {
        if (++iter == arg.end())
          break;
        else
          continue;
      }
    } else {
      word = *iter;
      break;
    }
  }

  return word;

}

//--------------------------------------------------------------------------
/**
 * <p>filters out the output file name from the argument string
 *
 * <p><b>return:</b>
 * - output file name if present in the argument list
 * - otherwise 'out.db' (db==true), 'out.dat' (db==false)
 *
 * \param arg argument string list from the msr2data call
 * \param db true if output file is a db-file
 */
string msr2data_outputfile(vector<string> &arg, bool db = true)
{
  string outputFile;
  if (db)
    outputFile = "out.db";
  else
    outputFile = "out.dat";

  vector<string>::iterator iterNext(arg.begin());
  for (vector<string>::iterator iter(arg.begin()); iter != arg.end(); ++iter) {
    iterNext = iter + 1;
    if (!iter->substr(0,2).compare("-o")) {
      if (!iter->compare("-o")) {
        if ((iterNext != arg.end()) && (iterNext->compare("header")) && (iterNext->compare("noheader")) && (iterNext->compare("nosummary")) \
            && (iterNext->substr(0,3).compare("fit")) && (iterNext->compare("-k")) && (iterNext->compare("-t")) \
            && (iterNext->compare("-e")) && (iterNext->compare("-p")) \
            && (iterNext->compare("data")) && (iterNext->substr(0,3).compare("msr")) && (iterNext->compare("global")) \
            && (iterNext->compare("global+")) && (iterNext->compare("global+!")) && (iterNext->compare("new"))) {
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
 * <p>Checks if option string is present in the argument input list
 *
 * <p><b>return:</b>
 * - true if option is <b>not</b> present
 * - false otherwise
 *
 * \param arg list of arguments
 * \param s option string
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
 * <p>Filters out the template run number (if present) and check at the
 * same time if only already existing msr-files shall be fitted (no template).
 *
 * <p><b>return:</b>
 * - template runNo if everything is OK
 * - -1 : tag: fit only, do not prepare input files
 * - -2 : fatal error - more than one fit-&lt;temp&gt; options are specified
 * - -3 : fit-&lt;temp&gt; option found, but &lt;temp&gt; is not a valid number
 *
 * \param arg list of arguments
 * \param chainfit if true
 *
 */
int msr2data_doFitting(vector<string> &arg, bool &chainfit)
{
  int temp(0);

  string s;
  vector<string>::iterator iter(arg.begin());
  while (iter != arg.end()) {
    if (!iter->compare("fit")) { // fit found
      if (temp) { // temp already found previously
        return -2; // fatal error - another fit-<temp> option is specified
      }
      temp = -1; // fit only, do not prepare input files
      chainfit = false;
      iter = arg.erase(iter);
    }
    else if (!iter->substr(0,4).compare("fit-")) { // 'fit-' found
      if (temp) {
        return -2; // fatal error - another fit option is specified
      }
      s = iter->substr(4);
      string::size_type loc = s.rfind('!');
      if (loc != string::npos) {
        chainfit = false;
        s.erase(loc);
      }
      else
        chainfit = true;
      try {
        temp = boost::lexical_cast<int>(s);
        arg.erase(iter);
      }
      catch(boost::bad_lexical_cast &) {
        return -3; // the specified template is no number
      }
    } else {
      ++iter;
    }
  }

  return temp;
}

//--------------------------------------------------------------------------
/**
 * <p>Filters out the template run number from which the new msr-files should be created.
 *
 * <b>return:</b>
 * - template run number, or
 * - -3 if the string after 'msr-' is not a number
 *
 * \param arg list of arguments
 * \param inputOnly flag, if true create msr-files only (no fitting)
 */
int msr2data_doInputCreation(vector<string> &arg, bool &inputOnly)
{
  int temp(0);

  string s;
  for (vector<string>::iterator iter(arg.begin()); iter != arg.end(); ++iter) {
    if (!iter->substr(0,4).compare("msr-")) {
      s = iter->substr(4);
      try {
        temp = boost::lexical_cast<int>(s);
        arg.erase(iter);
      }
      catch(boost::bad_lexical_cast &) {
        temp = -3; // the specified template is no number
      }
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
 */
int msr2data_paramList(vector<string> &arg, vector<unsigned int> &paramList)
{
  paramList.clear(); // make sure paramList is empty

  unsigned int idx=0;
  // check if paramList tag is present
  for (unsigned int i=0; i<arg.size(); i++) {
    if (!arg[i].compare("paramList")) {
      idx = i+1;
      break;
    }
  }

  if (idx == 0) { // paramList tag NOT present
    return 0;
  }

  // make sure there are parameter list elements to follow
  if (idx == arg.size()) {
    cerr << endl << "**ERROR** found paramList without any arguments!" << endl;
    msr2data_syntax();
    return -1;
  }

  // paramList tag present and further elements present: collect them
  vector<string> str;
  unsigned int idx_end=0;
  size_t pos=string::npos;
  for (unsigned int i=idx; i<arg.size(); i++) {
    pos = arg[i].find("-");
    if (pos == 0) { // likely something like -o, -k, etc.
      idx_end = i;
      break;
    } else if (pos != string::npos) { // looks like a parameter list like n0-n1
      boost::split(str, arg[i], boost::is_any_of("-"));
      if (str.size() != 2) { // something is wrong, since the structure n0-n1 is expected
        cerr << endl << "**ERROR** found token " << arg[i] << " in paramList command which cannot be handled." << endl;
        msr2data_syntax();
        return -1;
      }
      if (!str[0].compare("fit") || !str[0].compare("msr")) {
        idx_end = i;
        break;
      }
      if (!isNumber(str[0]) || !isNumber(str[1])) {
        cerr << endl << "**ERROR** found token " << arg[i] << " in paramList command which cannot be handled." << endl;
        msr2data_syntax();
        return -1;
      }
      unsigned int start=boost::lexical_cast<unsigned int>(str[0]);
      unsigned int end=boost::lexical_cast<unsigned int>(str[1]);
      for (unsigned int j=start; j<=end; j++)
        paramList.push_back(j);
    } else if (isNumber(arg[i])) { // a single number
      paramList.push_back(boost::lexical_cast<unsigned int>(arg[i]));
    } else { // likely the next argument not related to paramList
      idx_end = i;
      break;
    }
  }
  if (idx_end == 0)
    idx_end = arg.size();

  // remove all the paramList arguments for arg
  arg.erase(arg.begin()+idx-1, arg.begin()+idx_end);

  // go through the parameter list and make sure the values are unique
  for (unsigned int i=0; i<paramList.size(); i++) {
    for (unsigned int j=i+1; j<paramList.size(); j++) {
      if (paramList[i] == paramList[j]) {
        cerr << endl << "**ERROR** the parameter list numbers have to be unique. Found " << paramList[i] << " at least 2 times." << endl;
        msr2data_syntax();
        return -1;
      }
    }
  }

  return paramList.size();
}

//--------------------------------------------------------------------------
/**
 * <p>msr2data is used to generate msr-files based on template msr-files, automatically fit these new msr-files,
 * collect fitting parameters, etc. For a detailed description see
 * \htmlonly <a href="https://intranet.psi.ch/MUSR/Msr2Data">musr2data online help</a>
 * \endhtmlonly
 * \latexonly msr2data online help: \texttt{https://intranet.psi.ch/MUSR/Msr2Data}
 * \endlatexonly
 *
 * \param argc number of arguments
 * \param argv list of arguments
 */
int main(int argc, char *argv[])
{
  // check if version dump is wanted
  if (argc == 2) {
    if (!strcmp(argv[1], "--help")) {
      msr2data_syntax();
      return 0;
    } else if (!strcmp(argv[1], "--version")) {
#ifdef HAVE_CONFIG_H
      cout << endl << "msr2data version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << endl << endl;
#else
      cout << endl << "msr2data git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << endl << endl;
#endif
      return 0;
    } else {
      msr2data_syntax();
      return 0;
    }
  }

  // check the number of arguments
  if (argc < 2) {
    msr2data_syntax();
    return 0;
  }

  // use a string-vector for the arguments to get rid of char* as far as possible...
  vector<string> arg;
  for (int i(1); i<argc; ++i) {
    arg.push_back(argv[i]);
  }

  unsigned int runTAG;
  vector<unsigned int> run_vec;
  string run_list;
  string msrExtension;
  vector<unsigned int> param_vec;

  try {
    if (arg[0].at(0) == '[') { // In case a list of runs is given by [...]
      runTAG = 1;

      unsigned int firstRunNumberInArg(0), lastRunNumberInArg(0);
      unsigned int rightbracket(numeric_limits<unsigned int>::max());

      if (!arg[0].compare("["))
        firstRunNumberInArg = 1;
      else
        arg[0]=arg[0].substr(1);

      for (unsigned int i(firstRunNumberInArg); i<arg.size(); ++i) {
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
      if (rightbracket == numeric_limits<unsigned int>::max()) {
        cerr << endl;
        cerr << ">> msr2data: **ERROR** You used the list specification without closing bracket (])! Quitting now." << endl;
        return 0;
      }

      // generate run_list string
      for (unsigned int i(firstRunNumberInArg); i<=lastRunNumberInArg; ++i) {
        run_list += arg[i] + " ";
      }
      // parse run_list string
      PStringNumberList *nl = new PStringNumberList(run_list);
      if (nl == 0) { // couldn't invoke object
        cerr << endl;
        cerr << ">> msr2data: **ERROR** Couldn't invoke run_list parser object! Quitting now." << endl;
        return 0;
      }
      string errorMsg("");
      if (!nl->Parse(errorMsg)) {
        cerr << endl;
        cerr << ">> msr2data: " << errorMsg << " - Quitting now." << endl;
        return 0;
      }
      // get run list vector
      run_vec = nl->GetList();
      delete nl;

      msrExtension = arg[rightbracket + 1];

      vector<string>::iterator iter(arg.begin());
      for (unsigned int i(0); i<rightbracket + 2; ++i) {
        arg.erase(iter);
        iter = arg.begin();
      }

    } else {
      bool argOneIsNumber(isNumber(arg[0])), argTwoIsNumber(isNumber(arg[1]));

      if (argOneIsNumber && argTwoIsNumber) { // the first two arguments are numbers: an interval of numbers is given
        runTAG = 2;

        if (arg.size() < 3) {
          cerr << endl;
          cerr << ">> msr2data: **ERROR** No msr-file extension specified! Quitting now..." << endl;
          run_vec.clear();
          arg.clear();
          return 0;
        }

        run_vec.push_back(boost::lexical_cast<unsigned int>(arg[0]));
        run_vec.push_back(boost::lexical_cast<unsigned int>(arg[1]));

        msrExtension = arg[2];

        vector<string>::iterator iter(arg.begin());
        for (unsigned int i(0); i < 3; i++) {
          arg.erase(iter);
          iter = arg.begin();
        }
      } else if (argOneIsNumber && !argTwoIsNumber) { // only one run number is given
        runTAG = 3;
        run_vec.push_back(boost::lexical_cast<unsigned int>(arg[0]));
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
  }
  catch(boost::bad_lexical_cast &) {
    cerr << endl;
    cerr << ">> msr2data: **ERROR** At least one given run number is out of range! Quitting..." << endl;
    run_vec.clear();
    arg.clear();
    return -1;
  }

  // check if parameter list is given
  int noParamList(msr2data_paramList(arg, param_vec));
  if (noParamList == -1) {
    arg.clear();
    return -1;
  }

  // check the validity of the command line given command line arguments
  string wrongArgument(msr2data_validArguments(arg));
  if (!wrongArgument.empty()) {
    cerr << endl;
    cerr << ">> msr2data: **ERROR** Unknown argument: " << wrongArgument << ". Quitting..." << endl;
    run_vec.clear();
    arg.clear();
    return -1;
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
  PMsr2Data *msr2dataHandler = new PMsr2Data(msrExtension);

  int status;

  switch(runTAG) {
    case 1:
      status = msr2dataHandler->SetRunNumbers(run_vec);
      break;
    case 2:
      status = msr2dataHandler->SetRunNumbers(run_vec[0], run_vec[1]);
      break;
    case 3:
      status = msr2dataHandler->SetRunNumbers(run_vec[0]);
      break;
    case 4:
      status = msr2dataHandler->SetRunNumbers(run_list);
      break;
    default:
      cerr << endl;
      cerr << ">> msr2data: **ERROR** None of the possible run list specifications has been detected! Quitting now..." << endl;
      msr2data_cleanup(msr2dataHandler, run_vec, arg);
      return 0;
  }

  if (status == 1) {
    cerr << endl;
    cerr << ">> msr2data: **ERROR** The run numbers are out of range! Quitting..." << endl;
    msr2data_cleanup(msr2dataHandler, run_vec, arg);
    return status;
  } else if (status == -1) {
    msr2data_cleanup(msr2dataHandler, run_vec, arg);
    return status;
  }

  run_vec.clear();

  // check if fitting should be done and in case, which template run number to use
  int temp(0);
  bool chainfit(true), onlyInputCreation(false);
  string musrfitOptions;

  temp = msr2data_doFitting(arg, chainfit);

  if (temp == -2) {
    cerr << endl;
    cerr << ">> msr2data: **ERROR** More than one fitting options are specified! Quitting..." << endl;
    msr2data_cleanup(msr2dataHandler, arg);
    return temp;
  } else if (temp == -3) {
    cerr << endl;
    cerr << ">> msr2data: **ERROR** The given template has not a valid run number! Quitting..." << endl;
    msr2data_cleanup(msr2dataHandler, arg);
    return temp;
  }


  // check if any options should be passed to musrfit
  if (temp) {
    if (!msr2data_useOption(arg, "-k") || !msr2data_useOption(arg, "--keep-mn2-output"))
      musrfitOptions.append("-k ");
    if (!msr2data_useOption(arg, "-t") || !msr2data_useOption(arg, "--title-from-data-file"))
      musrfitOptions.append("-t ");
    if (!msr2data_useOption(arg, "-e") || !msr2data_useOption(arg, "--estimateN0"))
      musrfitOptions.append("-e ");
    if (!msr2data_useOption(arg, "-p") || !msr2data_useOption(arg, "--per-run-block-chisq"))
      musrfitOptions.append("-p ");
  }

  // if no fitting should be done, check if only the input files should be created
  if (!temp) {
    temp = msr2data_doInputCreation(arg, onlyInputCreation);
    if (onlyInputCreation) {
      // if only input files should be created, do not write data to an output file (no matter, what has been determined earlier)
      realOutput = false;
      outputFile = "none";
    }
    if (temp == -3) {
      cerr << endl;
      cerr << ">> msr2data: **ERROR** The given template has not a valid run number! Quitting..." << endl;
      msr2data_cleanup(msr2dataHandler, arg);
      return temp;
    }
  }

  // check if msr2data should work in the global fit regime
  bool setNormalMode(msr2data_useOption(arg, "global"));
  unsigned int globalMode(0);

  if (!msr2data_useOption(arg, "global+!")) {
    setNormalMode = false;
    globalMode = 1;
  } else if (!msr2data_useOption(arg, "global+")) {
    setNormalMode = false;
    globalMode = 2;
  }

  // At this point it should be clear if any template for input-file generation is given or not.
  // Therefore, the number of digits in the run number format is determined only here.
  if(temp > 0) {
    status = msr2dataHandler->DetermineRunNumberDigits(temp, setNormalMode);
  } else {
    status = msr2dataHandler->DetermineRunNumberDigits(msr2dataHandler->GetPresentRun(), setNormalMode);
  }

  if(status) {
    msr2data_cleanup(msr2dataHandler, arg);
    return status;
  }

  // Check if all given run numbers are covered by the formatting of the data file name
  status = msr2dataHandler->CheckRunNumbersInRange();
  if(status) {
    cerr << endl;
    cerr << ">> msr2data: **ERROR** At least one given run number is out of range! Quitting..." << endl;
    msr2data_cleanup(msr2dataHandler, arg);
    return status;
  }

  bool writeSummary(false);
  unsigned int writeHeader(2);
  // writeHeader: 0 - no header
  //              1 - write header explicitly (even if the file is present already)
  //              2 - write header automatically if a new file is created and do not if the data is appended to an existing file

  fstream *fileOutput = 0;
  if (realOutput) {
    // check the arguments for the "header" and "noheader" options
    if (!msr2data_useOption(arg, "header")) {
      if (!msr2data_useOption(arg, "noheader")) {
        writeHeader = 2;
      } else {
        writeHeader = 1;
      }
    } else if (!msr2data_useOption(arg, "noheader")) {
      writeHeader = 0;
    }

    // check the arguments for the "nosummary" option
    writeSummary = msr2data_useOption(arg, "nosummary");

    // delete old db/data file if the "new" option is given
    if (!msr2data_useOption(arg, "new")) {
      fileOutput = new fstream;
      fileOutput->open(outputFile.c_str(), ios::in);
      if (fileOutput->is_open()) {
        cout << endl << ">> msr2data: **INFO** Deleting output file " << outputFile << endl;
        fileOutput->close();
        fileOutput->open(outputFile.c_str(), ios::out | ios::trunc);
        fileOutput->close();
      } else {
        cout << endl << ">> msr2data: **INFO** Ignoring the 'new' option since " << outputFile << " does not exist yet." << endl;
      }
      delete fileOutput;
      fileOutput = 0;
      if (writeHeader == 2) {
        writeHeader = 1;
      }
    }
  }

  // GLOBAL MODE
  if (!setNormalMode) {
    ostringstream strInfile;
    strInfile << msr2dataHandler->GetPresentRun() << "+global" << msrExtension << ".msr";

    // if fitting should be done, prepare a new input file
    if (temp) {
      if (temp > 0) { // if it is smaller no input file should be generated
        bool success(msr2dataHandler->PrepareGlobalInputFile(temp, strInfile.str(), globalMode));

        if (!success) {
          cerr << endl << ">> msr2data: **ERROR** Input file generation has not been successful! Quitting..." << endl;
          msr2data_cleanup(msr2dataHandler, arg);
          return -1;
        }
      }

      // and do the fitting
      if (!onlyInputCreation) {
        // check if MUSRFITPATH is set, if not issue a warning
        string path("");
        bool pathSet(false);
        char *pathPtr(getenv("MUSRFITPATH"));
        if (pathPtr) {
          path = boost::lexical_cast<string>(pathPtr);
          if (!path.empty()) {
            pathSet = true;
            path.append("/");
          }
        }
        if (!pathSet) {
          cerr << endl << ">> msr2data: **WARNING** The MUSRFITPATH environment variable is not set!";
          cerr << endl << ">> msr2data: **WARNING** Please set it or at least ensure that musrfit can be found on the PATH!" << endl;
        }
        ostringstream oss;
        oss << path << "musrfit" << " " << strInfile.str() << " " << musrfitOptions;
        cout << endl << ">> msr2data: **INFO** Calling " << oss.str() << endl;
        system(oss.str().c_str());
      }
    }

    if (realOutput) {
      // read musrfit startup file
      if (writeSummary) {
        status = msr2dataHandler->ParseXmlStartupFile();
      }

      // Read msr file
      status = msr2dataHandler->ReadMsrFile(strInfile.str());
      if (status != PMUSR_SUCCESS) {
        arg.clear();
        return status;
      }

      // read data files
      if (writeSummary)
        status = msr2dataHandler->ReadRunDataFile();

      unsigned int counter(0);

      while (msr2dataHandler->GetPresentRun()) {
        // write DB or dat file
        status = msr2dataHandler->WriteOutput(outputFile, param_vec, db, writeHeader, !setNormalMode, counter);
        if (status == -1) {
          msr2data_cleanup(msr2dataHandler, arg);
          return status;
        }
        ++counter;
      }
    }
  } else { // NORMAL MODE - one msr-file for each run

    // read musrfit startup file
    if (writeSummary) {
      status = msr2dataHandler->ParseXmlStartupFile();
    }

    // Processing the run list, do the fitting and write the data to the DB or data output file
    bool firstrun(true);
    unsigned int oldtemp(0); // should be accessed only when updated before...
    ostringstream strInfile;

    while (msr2dataHandler->GetPresentRun()) {
      strInfile.clear();
      strInfile.str("");
      strInfile << msr2dataHandler->GetPresentRun() << msrExtension << ".msr";

      // if fitting should be done, prepare a new input file
      if (temp) {
        if (temp > 0) {
          bool success(true);
          if (firstrun || !chainfit)
            success = msr2dataHandler->PrepareNewInputFile(temp, false);
          else
            success = msr2dataHandler->PrepareNewInputFile(oldtemp, false);
          if (firstrun)
            firstrun = false;
          oldtemp = msr2dataHandler->GetPresentRun();

          if (!success) {
            cerr << endl << ">> msr2data: **ERROR** Input file generation has not been successful! Quitting..." << endl;
            msr2data_cleanup(msr2dataHandler, arg);
            return -1;
          }
        }

        // and do the fitting
        if (!onlyInputCreation) {
          // check if MUSRFITPATH is set, if not issue a warning
          string path("");
          bool pathSet(false);
          char *pathPtr(getenv("MUSRFITPATH"));
          if (pathPtr) {
            path = boost::lexical_cast<string>(pathPtr);
            if (!path.empty()) {
              pathSet = true;
              path.append("/");
            }
          }
          if (!pathSet) {
            cerr << endl << ">> msr2data: **WARNING** The MUSRFITPATH environment variable is not set!";
            cerr << endl << ">> msr2data: **WARNING** Please set it or at least ensure that musrfit can be found on the PATH!" << endl;
          }
          ostringstream oss;
          oss << path << "musrfit" << " " << strInfile.str() << " " << musrfitOptions;
          cout << endl << ">> msr2data: **INFO** Calling " << oss.str() << endl;
          system(oss.str().c_str());
        }
      }

      // read msr-file
      if (realOutput) {
        status = msr2dataHandler->ReadMsrFile(strInfile.str());
        if (status != PMUSR_SUCCESS) {
          // if the msr-file cannot be read, write no output but proceed to the next run
          status = msr2dataHandler->WriteOutput("none", param_vec, db, writeHeader);
          if (status == -1) {
            msr2data_cleanup(msr2dataHandler, arg);
            return status;
          } else {
            continue;
          }
        }
      }

      // read data files
      if (writeSummary)
        status = msr2dataHandler->ReadRunDataFile();

      // write DB or dat file
      status = msr2dataHandler->WriteOutput(outputFile, param_vec, db, writeHeader);
      if (status == -1) {
        msr2data_cleanup(msr2dataHandler, arg);
        return status;
      }
    }
  }

  // Make sure that the empty line at the end of the output file gets written
  // This is needed to create a "valid db-file", however, we do it for all output files
  // Unfortunately, this can be done in a coherent way only on that level
  // Unfortunately, there are also problems with boost::filesystem::exists(outputFile)
  // Therefore, first try to open the file for reading and if this works, write to it - not clean but it works
  if (realOutput) {
    fileOutput = new fstream;
    fileOutput->open(outputFile.c_str(), ios::in);
    if (fileOutput->is_open()) {
      fileOutput->close();
      fileOutput->open(outputFile.c_str(), ios::out | ios::app);
      if (fileOutput->is_open()) {
        *fileOutput << endl << endl;
        fileOutput->close();
      } else {
        cerr << endl << ">> msr2data: **ERROR** The output file " << outputFile << " cannot be opened! Please check!";
        cerr << endl;
      }
    } else {
      cerr << endl << ">> msr2data: **WARNING** No output has been written to the file " << outputFile << "!";
      cerr << endl << ">> msr2data: **WARNING** Please check the range of runs and the specified options!" << endl;
    }
    delete fileOutput;
    fileOutput = 0;
  }

  if (!arg.empty()) {
    cout << endl << ">> msr2data: **INFO** The following command line arguments have been specified but not been used: " << endl;
    cout << ">> msr2data: **INFO**";
    for (unsigned int i(0); i<arg.size(); ++i)
      cout << " " << arg[i];
    cout << endl;
  }

  msr2data_cleanup(msr2dataHandler, arg);

  cout << endl << ">> msr2data: done ..." << endl;

  return 1;
}
