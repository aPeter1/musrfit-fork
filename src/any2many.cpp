/***************************************************************************

  any2many.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <fstream>

#include <TString.h>
#include <TSAXParser.h>

#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif

#include "PMusr.h"
#include "PStartupHandler.h"
#include "PRunDataHandler.h"

//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void any2many_syntax()
{
  std::cout << std::endl << "usage: any2many [--help]   : will show this help.";
  std::cout << std::endl << "       any2many --version  : will show the git version.";
  std::cout << std::endl << "       any2many -f <filenameList-input> | -r <runList-input>";
  std::cout << std::endl << "                -c <convert-options> [-p <output-path>] [-y <year>]";
  std::cout << std::endl << "                [-o <outputFileName> | -t <in-template> <out-template>] [-s]";
  std::cout << std::endl << "                [-rebin <n>] [-z <compressed>]";
  std::cout << std::endl << "          -f <filenameList-input> : where <filenameList-input> is space";
  std::cout << std::endl << "                separeted a list of file names (not starting with a '-'),";
  std::cout << std::endl << "                e.g. 2010/lem10_his_0111.root 2010/lem10_his_0112.root";
  std::cout << std::endl << "          -o <outputFileName> : this option only makes sense, if <filenameList-input>";
  std::cout << std::endl << "                is a single input file name!";
  std::cout << std::endl << "          -r <runList-input> : can be:";
  std::cout << std::endl << "               (i)   <run0>, <run1>, <run2>, ... <runN> : run numbers, e.g. 123 124";
  std::cout << std::endl << "               (ii)  <run0>-<runN> : a range, e.g. 123-125 -> 123 124 125";
  std::cout << std::endl << "               (iii) <run0>:<runN>:<step> : a sequence, e.g. 123:127:2 -> 123 125 127";
  std::cout << std::endl << "                     <step> will give the step width and has to be a positive number!";
  std::cout << std::endl << "               a <runList> can also combine (i)-(iii), e.g. 123 128-130 133, etc.";
  std::cout << std::endl << "          -t <in-template> <out-template> : ";
  std::cout << std::endl << "             <in-/out-template> : template file name. Needed for run-lists in";
  std::cout << std::endl << "                order to generate the proper file names. The following template";
  std::cout << std::endl << "                tags can be used: [yy] for year, and [rrrr] for the run number.";
  std::cout << std::endl << "                If the run number tag is used, the number of 'r' will give the";
  std::cout << std::endl << "                number of digits used with leading zeros, e.g. [rrrrrr] and run";
  std::cout << std::endl << "                number 123 will result in 000123. The same is true for the";
  std::cout << std::endl << "                year, i.e. [yyyy] will result in something like 1999.";
  std::cout << std::endl << "          -c <convert-options> : <inFormat> <outFormat>";
  std::cout << std::endl << "             <inFormat>  : input data file format. Supported formats are:";
  std::cout << std::endl << "                MusrRoot, PSI-BIN, ROOT (LEM), MUD, NeXus, PSI-MDU, WKM";
  std::cout << std::endl << "             <outFormat> : ouput data file format. Supported formats are:";
  std::cout << std::endl << "                PSI-BIN, MusrRoot, ROOT, MUD, NeXus1-HDF4, NeXus1-HDF5, NeXus1-XML,";
  std::cout << std::endl << "                NeXus2-HDF4, NeXus2-HDF5, NeXus2-XML, WKM, ASCII";
  std::cout << std::endl << "                Comment: ROOT is superseeded by MusrRoot. If there is not a very good";
  std::cout << std::endl << "                reason, avoid it!";
  std::cout << std::endl << "          -h <histo-group-list> : This option is for MusrRoot input files only!";
  std::cout << std::endl << "                Select the the histo groups to be exported. <histo-group-list> is a space";
  std::cout << std::endl << "                separated list of the histo group, e.g. -h 0, 20 will try to export the histo";
  std::cout << std::endl << "                0 (NPP) and 20 (PPC).";
  std::cout << std::endl << "          -p <output-path> : where <output-path> is the output path for the";
  std::cout << std::endl << "               converted files. If nothing is given, the current directory";
  std::cout << std::endl << "               will be used, unless the option '-s' is used.";
  std::cout << std::endl << "          -y <year> : if the option -y is used, here a year in the form 'yy' or 'yyyy' can";
  std::cout << std::endl << "                be given, if this is the case, any automatic file name";
  std::cout << std::endl << "                generation needs a year, this number will be used.";
  std::cout << std::endl << "          -s : with this option the output data file will be sent to the stdout.";
  std::cout << std::endl << "          -rebin <n> : where <n> is the number of bins to be packed";
  std::cout << std::endl << "          -z [g|b] <compressed> : where <compressed> is the output file name";
  std::cout << std::endl << "                (without extension) of the compressed data collection, and";
  std::cout << std::endl << "                'g' will result in .tar.gz, and 'b' in .tar.bz2 files.";
  std::cout << std::endl;
  std::cout << std::endl << "          If the template option '-t' is absent, the output file name will be";
  std::cout << std::endl << "          generated according to the input data file name, and the output data";
  std::cout << std::endl << "          format.";
  std::cout << std::endl;
  std::cout << std::endl << "examples:" << std::endl;
  std::cout << std::endl << "  any2many -f 2010/lem10_his_0123.root -c ROOT ASCII -rebin 25";
  std::cout << std::endl << "      Will take the LEM ROOT file '2010/lem10_his_0123.root' rebin it with 25";
  std::cout << std::endl << "      and convert it to ASCII. The output file name will be";
  std::cout << std::endl << "      lem10_his_0123.ascii, and the file will be saved in the current directory." << std::endl;
  std::cout << std::endl << "  any2many -f 2010/lem10_his_0123.root -c MusrRoot NEXUS2-HDF5 -o 2010/lem10_his_0123_v2.nxs";
  std::cout << std::endl << "      Will take the MusrRoot file '2010/lem10_his_0123.root' ";
  std::cout << std::endl << "      and convert it to NeXus IDF V2. The output file name will be";
  std::cout << std::endl << "      lem10_his_0123_v2.nxs, and the file will be saved in the current directory." << std::endl;
  std::cout << std::endl << "  any2many -r 123 137 -c PSI-BIN MUD -t d[yyyy]/deltat_tdc_gps_[rrrr].bin \\";
  std::cout << std::endl << "      [rrrrrr].msr -y 2001";
  std::cout << std::endl << "      Will take the run 123 and 137, will generate the input file names:";
  std::cout << std::endl << "      d2001/deltat_tdc_gps_0123.bin and d2001/deltat_tdc_gps_0137.bin, and";
  std::cout << std::endl << "      output file names 000123.msr and 000137.msr" << std::endl;
  std::cout << std::endl << "  any2many -r 100-117 -c PSI-MDU ASCII -t d[yyyy]/deltat_tdc_alc_[rrrr].mdu \\";
  std::cout << std::endl << "      [rrr].ascii -y 2011 -s";
  std::cout << std::endl << "      Will take the runs 100 through 117 and convert the PSI-MDU input files to";
  std::cout << std::endl << "      ASCII output and instead of saving them into a file, they will be spit to";
  std::cout << std::endl << "      the standard output." << std::endl;
  std::cout << std::endl << "  any2many -r 100-117 -c NEXUS MusrRoot -t d[yyyy]/psi_gps_[rrrr].NXS \\";
  std::cout << std::endl << "      psi_[yyyy]_gps_[rrrr].root -z b psi_gps_run_100to117";
  std::cout << std::endl << "      Will take the runs 100 through 117 and convert the NEXUS input files";
  std::cout << std::endl << "      to MusrRoot output. Afterwards these new files will be collected in a";
  std::cout << std::endl << "      compressed archive psi_gps_run_100to117.tar.bz2." << std::endl;
  std::cout << std::endl << "  any2many -f 2010/lem10_his_0123.root 2010/lem10_his_0012.root -c MusrRoot MusrRoot -rebin 25";
  std::cout << std::endl << "      Will read the two files '2010/lem10_his_0123.root' and '2010/lem10_his_0012.root',";
  std::cout << std::endl << "      rebin them with 25 and export them as LEM ROOT files with adding rebin25 to the";
  std::cout << std::endl << "      name, e.g. 2010/lem10_his_0123_rebin25.root";
  std::cout << std::endl << std::endl;
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
  TString outputFileName = TString("");

  // init inputFormat
  inputFormat.push_back("musrroot");
  inputFormat.push_back("psi-bin");
  inputFormat.push_back("root");
  inputFormat.push_back("mud");
  inputFormat.push_back("nexus");
  inputFormat.push_back("psi-mdu");
  inputFormat.push_back("wkm");

  // init outputFormat
  outputFormat.push_back("psi-bin");
  outputFormat.push_back("musrroot");
  outputFormat.push_back("root");
  outputFormat.push_back("mud");
  outputFormat.push_back("nexus1-hdf4");
  outputFormat.push_back("nexus1-hdf5");
  outputFormat.push_back("nexus1-xml");
  outputFormat.push_back("nexus2-hdf4");
  outputFormat.push_back("nexus2-hdf5");
  outputFormat.push_back("nexus2-xml");
  outputFormat.push_back("wkm");
  outputFormat.push_back("ascii");

  // init info structure
  info.useStandardOutput = false;
  info.rebin = 1;
  info.compressionTag = 0; // no compression as default
  info.idf = 0; // undefined
  info.inTemplate = TString("");
  info.outTemplate = TString("");
  info.outFileName = TString("");

  // call any2many without arguments
  if (argc == 1) {
    any2many_syntax();
    return PMUSR_SUCCESS;
  }

  // call any2many --help or any2many --version
  if (argc == 2) {
    if (!strncmp(argv[1], "--help", 128))
      any2many_syntax();
    else if (strstr(argv[1], "--v")) {
#ifdef HAVE_CONFIG_H
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "any2many version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "any2many version: " << PACKAGE_VERSION << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#endif
#else
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "any2many git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#else
      std::cout << std::endl << "any2many version: unkown." << std::endl << std::endl;
#endif
#endif
    } else {
      any2many_syntax();
      return PMUSR_WRONG_STARTUP_SYNTAX;
    }
    return PMUSR_SUCCESS;
  }

  info.outPath = TString("./");

  // handle the other command options
  Int_t ival;
  for (int i=1; i<argc; i++) {

    if (!strcmp(argv[i], "-y")) { // handle year option
      if (i+1 < argc) {
        // check that date if it is either of the form 'yy' or 'yyyy'
        if ((strlen(argv[i+1]) != 2) && (strlen(argv[i+1]) != 4)) {
          std::cerr << std::endl << ">> any2many **ERROR** found in option '-y' the argument '" << argv[i+1] << "' which is neither of the form 'yy' nor 'yyyy'." << std::endl;
          show_syntax = true;
          break;
        }
        ival=0;
        status = sscanf(argv[i+1], "%d", &ival);
        if (status == 1) {
          info.year = argv[i+1];
          i++;
        } else {
          std::cerr << std::endl << ">> any2many **ERROR** found in option '-y' the argument '" << argv[i+1] << "' which is not a number." << std::endl;
          show_syntax = true;
          break;
        }
      } else {
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-s")) { // handle standard output option '-s'
      info.useStandardOutput = true;
    } else if (!strcmp(argv[i], "-f")) { // set input option tag
      if (i+1 < argc) {
        bool done = false;
        int j = i+1;
        do {
          if (argv[j][0] == '-') { // assuming the next option is found
            done = true;
          } else {
            info.inFileName.push_back(argv[j]);
            j++; // shift input to the proper place
          }
        } while (!done && (j<argc));
        i = j-1;
        if (j >= argc) // make sure that counter is still in range
          break;
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found input option '-f' without any arguments" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-o")) { // handle output file name option '-o'
      if (i+1 < argc) {
        outputFileName = argv[i+1];
        i++;
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found output file name option '-o' without any arguments" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-r")) {
      if (i+1 < argc) {
        int pos{i+1};
        std::string runStr{""};
        // collect run list string from input
        for (int j=i+1; j<argc; j++) {
          pos = j;
          if ((argv[j][0] == '-') && isalpha(argv[j][1])) { // next command
            pos = j-1;
            break;
          } else {
            runStr += argv[j];
            runStr += " ";
          }
        }
        // extract run list from string
        PStringNumberList rl(runStr);
        std::string errMsg{""};
        if (!rl.Parse(errMsg)) {
          std::cerr << "**ERROR** in run list: -rl " << runStr << std::endl;
          std::cerr << errMsg << std::endl;
          return false;
        }
        info.runList = rl.GetList();
        // move the argument counter to the proper position
        i = pos;
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found input option '-r' without any arguments" << std::endl;
        show_syntax = true;
        break;
      }

      // check if any valid input option was found
      if (info.runList.size() == 0) {
        std::cerr << std::endl << ">> any2many **ERROR** found input option '-r' without any valid arguments" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-c")) { // set convert option tag
      bool found = false;
      std::string sval;
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
          std::cerr << std::endl << ">> any2many **ERROR** found unkown input data file format option '" << sval << "'" << std::endl;
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
          std::cerr << std::endl << ">> any2many **ERROR** found unkown output data file format option '" << sval << "'" << std::endl;
          show_syntax = true;
          break;
        }
        i += 2; // shift argument position
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found option '-c' with missing arguments" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-h")) { // filter histo group list (for MusrRoot and ROOT (LEM) only!)
      bool done = false;
      int j = i+1;
      do {
        status = sscanf(argv[j], "%d", &ival);
        if (status == 1) {
          info.groupHistoList.push_back(ival);
          j++;
        } else {
          done = true;
        }
      } while (!done && (j<argc));
      i = j-1;
      if (j >= argc) // make sure that counter is still in range
        break;
    } else if (!strcmp(argv[i], "-p")) { // filter output path name flag
      if (i+1 < argc) {
        info.outPath = argv[i+1];
        if (!info.outPath.EndsWith("/"))
          info.outPath += "/";
        i++;
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found output option '-p' without any argument." << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-rebin")) { // filter out rebinning option
      if (i+1 < argc) {
        status = sscanf(argv[i+1], "%d", &ival);
        if (status == 1) {
          info.rebin = ival;
          i++;
        } else {
          std::cerr << std::endl << ">> any2many **ERROR** found in option '-rebin " << argv[i+1] << "' which doesn't make any sense." << std::endl;
          show_syntax = true;
          break;
        }
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found output option '-rebin' without any argument." << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-t")) { // filter out the input/output file template
      if (i+2 < argc) {
        if ((argv[i+1][0] == '-') || (argv[i+2][0] == '-')) {
          std::cerr << std::endl << ">> any2many **ERROR** found invalid template in option '-t'" << std::endl;
          show_syntax = true;
          break;
        }
        info.inTemplate = argv[i+1];
        info.outTemplate = argv[i+2];
        i += 2; // shift argument position
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found option '-t' with missing arguments" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-z")) { // filter out if compression is whished
      if (i+2 < argc) {
        if ((argv[i+1][0] == '-') || (argv[i+2][0] == '-')) {
          std::cerr << std::endl << ">> any2many **ERROR** found invalid template in option '-t'" << std::endl;
          show_syntax = true;
          break;
        }
        if (argv[i+1][0] == 'g') {
          info.compressionTag = 1;
        } else if (argv[i+1][0] == 'b') {
          info.compressionTag = 2;
        } else {
          std::cerr << std::endl << ">> any2many **ERROR** found in option '-z' compression tag '" << argv[i+1] << "' which is not supported." << std::endl;
          show_syntax = true;
          break;
        }
        info.compressFileName = argv[i+2];
        i += 2; // shift argument position
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found option '-z' with missing arguments" << std::endl;
        show_syntax = true;
        break;
      }
    } else { // unrecognized command
      std::cerr << std::endl << ">> any2many **ERROR** found unrecognized option " << argv[i] << std::endl;
      show_syntax = true;
      break;
    }
  }

  // make sure that either a filenameList or a runList has been provided
  if ((info.inFileName.size()==0) && (info.runList.size()==0)) {
    std::cerr << std::endl << ">> any2many **ERROR** neither a input filename list, nor a run list was given." << std::endl;
    show_syntax = true;
  }

  // make sure that in/out formats are given
  if ((info.inFormat.Length() == 0) || (info.outFormat.Length() == 0)) {
    std::cerr << std::endl << ">> any2many **ERROR** conversion information is missing." << std::endl;
    show_syntax = true;
  }

  // check if the output format is nexus
  if (info.outFormat.Contains("nexus1", TString::kIgnoreCase))
    info.idf = 1;
  if (info.outFormat.Contains("nexus2", TString::kIgnoreCase))
    info.idf = 2;

  // in case the '-o' is present, make sure that inFileName is only a single file name
  if (outputFileName.Length() > 0) {
    if (info.inFileName.size() == 1) {
      // make sure there is not in addition the template option given
      if ((info.inTemplate.Length() == 0) && (info.outTemplate.Length() == 0)) {
        info.outFileName = outputFileName;
      } else {
        std::cerr << std::endl << ">> any2many **ERROR** found option '-o' cannot be combined with option '-t'." << std::endl;
        show_syntax = true;
      }
    } else {
      std::cerr << std::endl << ">> any2many **ERROR** found option '-o' with multiple input file names, which doesn't make any sense." << std::endl;
      show_syntax = true;
    }
  }

  if (show_syntax) {
    info.runList.clear();
    any2many_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  if (!info.inFormat.CompareTo(info.outFormat, TString::kIgnoreCase) && (info.rebin == 1)) {
    info.runList.clear();
    std::cerr << std::endl << ">> any2many **ERROR** input data format == output data format, only allowed if rebin != 1.";
    std::cerr << std::endl << "            will ignore the request." << std::endl << std::endl;
    return PMUSR_SUCCESS;
  }

  // read startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    std::cerr << std::endl << ">> any2many **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
    std::cerr << std::endl;
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = nullptr;
    }
    if (startupHandler) {
      delete startupHandler;
      startupHandler = nullptr;
    }
  } else {
    strcpy(startup_path_name, startupHandler->GetStartupFilePath().Data());
    saxParser->ConnectToHandler("PStartupHandler", startupHandler);
    //status = saxParser->ParseFile(startup_path_name);
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PStartupHandler.cpp for the definition)
    status = parseXmlFile(saxParser, startup_path_name);
    // check for parse errors
    if (status) { // error
      std::cerr << std::endl << ">> any2many **WARNING** Reading/parsing musrfit_startup.xml failed.";
      std::cerr << std::endl;
      // clean up
      if (saxParser) {
        delete saxParser;
        saxParser = nullptr;
      }
      if (startupHandler) {
        delete startupHandler;
        startupHandler = nullptr;
      }
    }
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler;
  if (startupHandler)
    dataHandler = new PRunDataHandler(&info, startupHandler->GetDataPathList());
  else
    dataHandler = new PRunDataHandler(&info);

  // read and convert all data
  dataHandler->ConvertData();

  // check if it has been successfull
  bool success = dataHandler->IsAllDataAvailable();
  if (!success) {
    std::cerr << std::endl << ">> any2many **ERROR** Couldn't read all data files, will quit ..." << std::endl;
  }

  // clean up
  info.runList.clear();
  if (saxParser) {
    delete saxParser;
    saxParser = nullptr;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = nullptr;
  }
  if (dataHandler) {
    delete dataHandler;
    dataHandler = nullptr;
  }

  return PMUSR_SUCCESS;
}

// end ---------------------------------------------------------------------

