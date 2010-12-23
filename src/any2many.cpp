/***************************************************************************

  any2many.cpp

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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <fstream>
using namespace std;

#include <TString.h>
#include <TSAXParser.h>

#include "PMusr.h"
#include "PStartupHandler.h"
#include "PRunDataHandler.h"

//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void any2many_syntax()
{
  cout << endl << "usage: any2many [--help]   : will show this help.";
  cout << endl << "       any2many --version  : will show the svn version.";
  cout << endl << "       any2many -I <filename> | -i <runList-input> -c <convert-options>  [-p <output-path>] [-o <output>] [-y <year>]";
  cout << endl << "                 [-t <in-template> <out-template>] [-s] [-rebin <n>]";
  cout << endl << "          <filename> : a single file name, e.g. 2010/lem10_his_0111.root, but not starting with a '-'";
  cout << endl << "          <runList-input>  : a list of run numbers separated by ' ' of the form: <run1>  <run2>  <run3> etc.";
  cout << endl << "                     or a sequence of runs <runStart>-<runEnd>, e.g. 111-222";
  cout << endl << "          <convert-options> : <inFormat> <outFormat>";
  cout << endl << "              <inFormat> : input data file format. Supported formats are:";
  cout << endl << "                           PSI-BIN, ROOT (LEM), MUD, NeXuS, PSI-MDU, WKM";
  cout << endl << "              <outFormat>: ouput data file format. Supported formats are:";
  cout << endl << "                           PSI-BIN, ROOT (LEM), MUD, NeXuS, MDU, WKM, ASCII";
  cout << endl << "          <output-path> : output path for the converted files. If nothing is given, the current directory will be used,";
  cout << endl << "                     unless the option '-s' is used.";
  cout << endl << "          <output> : single output file name. If <input> is a list of runs, this option";
  cout << endl << "                     should be omitted since the file names are generated according to the";
  cout << endl << "                     run number and the file format, e.g. 111 -> lemyy_his_0111.root, where yy is";
  cout << endl << "                     the current year.";
  cout << endl << "          <year>   : if the option -y is used, here a year in the form 'yy' can be given, if this";
  cout << endl << "                     is the case, any automatic file name generation needs a year, this number will";
  cout << endl << "                     be used.";
  cout << endl << "          <in-template>";
  cout << endl << "          <out-template> : template file name. Needed for run-lists in order to generate the proper file names.";
  cout << endl << "                     The following template tags can be used: [yy] for year, and [rrrr] for the run number.";
  cout << endl << "                     If the run number tag is used, the number of 'r' will give the number of digits used";
  cout << endl << "                     with leading zeros, e.g. [rrrrrr] and run numbe 123 will result in 000123.";
  cout << endl << "                     The same is true for the year, i.e. [yyyy] will result in something like 1999.";
  cout << endl << "          -s       : with this option the output data file will be sent to the standard output.";
  cout << endl << "          <n>      : number of bins to be packed";
  cout << endl;
  cout << endl << "          If the output option '-o' is missing, the output file name will be generated according to the";
  cout << endl << "          input data file name, and the output data format.";
  cout << endl;
  cout << endl << "examples:" << endl;
  cout << endl << "  any2many -I 2010/lem10_his_0123.root -c ROOT ASCII -rebin 25";
  cout << endl << "      Will take the LEM ROOT file '2010/lem10_his_0123.root' rebin it with 25 and convert it to ASCII.";
  cout << endl << "      The output file name will be lem10_his_0123.ascii, and the file will be saved in the current directory.";
  cout << endl << "  any2many -i 123 137 -c PSI-BIN MUD -t d[yyyy]/deltat_tdc_gps_[rrrr].bin [rrrrrr].msr -y 2001";
  cout << endl << "      Will take the run 123 and 137, will generate the input file names d2001/deltat_tdc_gps_0123.bin and";
  cout << endl << "      d2001/deltat_tdc_gps_0137.bin, and output file names 000123.msr and 000137.msr";
  cout << endl << "  any2many -i 100-117 -c PSI-MDU ASCII -t d[yyyy]/deltat_tdc_alc_[rrrr].mdu [rrr].ascii -y 2011 -s";
  cout << endl << "      Will take the runs 100 through 117 and convert the PSI-MDU input files to ASCII output and instead of";
  cout << endl << "      saving them into a file, they will be spit to the standard output.";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>any2many is used to convert any musr-data-file format in any other including ascii.
 *
 * <b>return:</b>
 * - PMUSR_SUCCESS if everthing went smooth
 * - PMUSR_WRONG_STARTUP_SYNTAX if syntax error is encountered
 *
 * \param argc number of input arguments
 * \param argv list of input arguments
 */
int main(int argc, char *argv[])
{
  bool show_syntax = false;
  int  status;
  PAny2ManyInfo info;
  PStringVector inputFormat;
  PStringVector outputFormat;

  // init inputFormat
  inputFormat.push_back("psi-bin");
  inputFormat.push_back("root");
  inputFormat.push_back("mud");
  inputFormat.push_back("nexus");
  inputFormat.push_back("psi-mdu");
  inputFormat.push_back("wkm");

  // init outputFormat
  outputFormat.push_back("psi-bin");
  outputFormat.push_back("root");
  outputFormat.push_back("mud");
  outputFormat.push_back("nexus");
  outputFormat.push_back("psi-mdu");
  outputFormat.push_back("wkm");
  outputFormat.push_back("ascii");

  // init info structure
  info.useStandardOutput = false;
  info.rebin = 1;

  // call any2many without arguments
  if (argc == 1) {
    any2many_syntax();
    return PMUSR_SUCCESS;
  }

  // call any2many --help or any2many --version
  if (argc == 2) {
    if (strstr(argv[1], "--h"))
      any2many_syntax();
    else if (strstr(argv[1], "--v"))
      cout << endl << "any2many version: $Id$" << endl << endl;
    else {
      any2many_syntax();
      return PMUSR_WRONG_STARTUP_SYNTAX;
    }
    return PMUSR_SUCCESS;
  }

  info.outPath = TString("./");

  // handle the other command options
  Int_t ival;
  for (int i=1; i<argc; i++) {

    // handle year option
    if (strstr(argv[i], "-y")) {
      if (i+1 < argc) {
        ival=0;
        status = sscanf(argv[i+1], "%d", &ival);
        if (status == 1) {
          info.year = argv[i+1];
          i++;
        } else {
          cerr << endl << ">> any2many **ERROR** found in option '-y' the argument '" << argv[i+1] << "' which is not a number." << endl;
          show_syntax = true;
          break;
        }
      } else {
        show_syntax = true;
        break;
      }
    }

    // handle standard output option '-s'
    if (strstr(argv[i], "-s"))
      info.useStandardOutput = true;

    // set input option tag
    if (strstr(argv[i], "-I")) {
      if (i+1 < argc) {
        if (argv[i+1][0] == '-') { // something wrong
          cerr << endl << ">> any2many **ERROR** found input option '-i' with filename " << argv[i+1] << ". This is not allowed." << endl;
          show_syntax = true;
          break;
        } else {
          info.inFileName = argv[i+1];
          i++; // shift input to the proper place
        }
      } else {
        cerr << endl << ">> any2many **ERROR** found input option '-I' without any arguments" << endl;
        show_syntax = true;
        break;
      }
    }
    if (strstr(argv[i], "-i")) {
      if (i+1 < argc) {
        // first check for run list sequence of the form <runStartNo>-<runEndNo>
        int startNo, endNo;
        status = sscanf(argv[i+1], "%d-%d", &startNo, &endNo);
        if (status == 2) { // this is run list sequence
          if (endNo < startNo) {
            ival = startNo;
            startNo = endNo;
            endNo = ival;
          }
          for (int j=startNo; j<=endNo; j++)
            info.runList.push_back(j);
        } else { // check for run list of the form <run1>  <run2>  ...  <runN>
          bool done = false;
          int j = i+1;
          do {
            status = sscanf(argv[j], "%d", &ival);
            if (status == 1) {
              info.runList.push_back(ival);
            } else {
              done = true;
            }
            j++;
          } while (!done && (j<argc));
        }
      } else {
        cerr << endl << ">> any2many **ERROR** found input option '-i' without any arguments" << endl;
        show_syntax = true;
        break;
      }

      // check if any valid input option was found
      if ((info.inFileName.Length() == 0) && (info.runList.size() == 0)) {
        cerr << endl << ">> any2many **ERROR** found input option '-i' without any valid arguments" << endl;
        show_syntax = true;
        break;
      }
    }

    // set output option tag
    if (strstr(argv[i], "-o")) {
      if (i+1 < argc) {
        info.outFileName = argv[i+1];
      } else {
        cerr << endl << ">> any2many **ERROR** found output option '-o' without any argument." << endl;
        show_syntax = true;
        break;
      }
    }

    // set convert option tag
    if (strstr(argv[i], "-c")) {
      bool found = false;
      string sval;
      if (i+2 < argc) {
        sval = argv[i+1];
        found = false;
        for (unsigned int j=0; j<inputFormat.size(); j++) {
          if (!inputFormat[j].CompareTo(sval, TString::kIgnoreCase)) {
            info.inFormat = sval;
            found = true;
            break;
          }
        }
        if (!found) {
          cerr << endl << ">> any2many **ERROR** found unkown input data file format option '" << sval << "'" << endl;
          show_syntax = true;
          break;
        }
        sval = argv[i+2];
        found = false;
        for (unsigned int j=0; j<outputFormat.size(); j++) {
          if (!outputFormat[j].CompareTo(sval, TString::kIgnoreCase)) {
            info.outFormat = sval;
            found = true;
            break;
          }
        }
        if (!found) {
          cerr << endl << ">> any2many **ERROR** found unkown output data file format option '" << sval << "'" << endl;
          show_syntax = true;
          break;
        }
        i += 2; // shift argument position
      } else {
        cerr << endl << ">> any2many **ERROR** found option '-c' with missing arguments" << endl;
        show_syntax = true;
        break;
      }
    }

    // filter output path name flag
    if (strstr(argv[i], "-p")) {
      if (i+1 < argc) {
        info.outPath = argv[i+1];
      } else {
        cerr << endl << ">> any2many **ERROR** found output option '-p' without any argument." << endl;
        show_syntax = true;
        break;
      }
    }

    // filter out rebinning option
    if (strstr(argv[i], "-rebin")) {
      if (i+1 < argc) {
        status = sscanf(argv[i+1], "%d", &ival);
        if (status == 1) {
          info.rebin = ival;
        } else {
          cerr << endl << ">> any2many **ERROR** found in option '-rebin " << argv[i+1] << "' which doesn't make any sense." << endl;
          show_syntax = true;
          break;
        }
      } else {
        cerr << endl << ">> any2many **ERROR** found output option '-rebin' without any argument." << endl;
        show_syntax = true;
        break;
      }
    }

    // filter out the input/output file template
    if (strstr(argv[i], "-t")) {
      if (i+2 < argc) {
        if ((argv[i+1][0] == '-') || (argv[i+2][0] == '-')) {
          cerr << endl << ">> any2many **ERROR** found invalid template in option '-t'" << endl;
          show_syntax = true;
          break;
        }
        info.inTemplate = argv[i+1];
        info.outTemplate = argv[i+2];
        i += 2; // shift argument position
      } else {
        cerr << endl << ">> any2many **ERROR** found option '-t' with missing arguments" << endl;
        show_syntax = true;
        break;
      }
    }
  }

/*
cout << endl << "debug> info.year='" << info.year << "', info.year.length()=" << info.year.Length();
cout << endl << "debug> info.useStandardOutput=" << info.useStandardOutput;
cout << endl << "debug> info.inFormat=" << info.inFormat;
cout << endl << "debug> info.outFormat=" << info.outFormat;
cout << endl << "debug> info.outFileName=" << info.outFileName;
cout << endl << "debug> info.inFileName=" << info.inFileName;
cout << endl << "debug> info.runList=";
for (unsigned int i=0; i<info.runList.size(); i++)
  cout << info.runList[i] << ", ";
cout << endl << "debug> info.outPath=" << info.outPath;
cout << endl << "debug> info.rebin=" << info.rebin;
cout << endl << "debug> info.inTemplate=" << info.inTemplate;
cout << endl << "debug> info.outTemplate=" << info.outTemplate;
cout << endl;
*/

  if (show_syntax) {
    info.runList.clear();
    any2many_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  if (!info.inFormat.CompareTo(info.outFormat, TString::kIgnoreCase)) {
    info.runList.clear();
    cout << endl << "**INFO** since input data format == output data format, I will not do anything." << endl;
    return PMUSR_SUCCESS;
  }

  // read startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    cerr << endl << "**WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
    cerr << endl;
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = 0;
    }
    if (startupHandler) {
      delete startupHandler;
      startupHandler = 0;
    }
  } else {
    strcpy(startup_path_name, startupHandler->GetStartupFilePath().Data());
    saxParser->ConnectToHandler("PStartupHandler", startupHandler);
    status = saxParser->ParseFile(startup_path_name);
    // check for parse errors
    if (status) { // error
      cerr << endl << "**WARNING** reading/parsing musrfit_startup.xml.";
      cerr << endl;
      // clean up
      if (saxParser) {
        delete saxParser;
        saxParser = 0;
      }
      if (startupHandler) {
        delete startupHandler;
        startupHandler = 0;
      }
    }
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler = new PRunDataHandler(&info, startupHandler->GetDataPathList());

  bool success = dataHandler->IsAllDataAvailable();
  if (!success) {
    cout << endl << ">> any2many **ERROR** Couldn't read all data files, will quit ..." << endl;
  }

  // clean up
  info.runList.clear();
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }
  if (dataHandler) {
    delete dataHandler;
    dataHandler = 0;
  }

  return PMUSR_SUCCESS;
}

// end ---------------------------------------------------------------------

