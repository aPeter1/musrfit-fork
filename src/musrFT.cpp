/***************************************************************************

  musrFT.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2015 by Andreas Suter                              *
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

#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TSAXParser.h>

#include "git-revision.h"
#include "PMusr.h"
#include "PStartupHandler.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PPrepFourier.h"
#include "PFourier.h"

//----------------------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  vector<TString> msrFln;
  vector<TString> dataFln;
  vector<TString> dataFileFormat;
  TString graphicFormat;
  TString dumpFln;
  int bkg[2];
  TString fourierOpt;
  TString apotization;
  int fourierPower;
  TString fourierUnits;
  double initialPhase;
  double fourierRange[2];
  double timeRange[2];
  vector<int> histo;
  bool showAverage;
  vector<int> t0;
  int packing;
  TString title;
} musrFT_startup_param;

//-------------------------------------------------------------------------
/**
 * <p>
 */
void musrFT_syntax()
{
  cout << endl << "usage: musrFT [Options] [<msr-files> | -df, --data-file <data-file>]";
  cout << endl << "  <msr-files> : msr-file name(s). These msr-files are used for the Fourier transform.";
  cout << endl << "                It can be a list of msr-files, e.g. musrFT 3110.msr 3111.msr";
  cout << endl << "                For the syntax of the msr-file check the user manual of musrfit.";
  cout << endl << "  -df, --data-file <data-file> : This allows to feed only muSR data file(s) to";
  cout << endl << "                perform the Fourier transform. Since the extended <msr-file> information";
  cout << endl << "                are missing, they will need to be provided by to options, or musrFT";
  cout << endl << "                tries to guess, based on musrFT_startup.xml settings.";
  cout << endl << "  Options: ";
  cout << endl << "    --help    : display this help and exit";
  cout << endl << "    --version : output version information and exit";
  cout << endl << "    -g, --graphic-format <graphic-format-extension> : ";
  cout << endl << "                will produce a graphic-output-file without starting a root session.";
  cout << endl << "                the name is based either on the <msr-file> or the <data-file>,";
  cout << endl << "                e.g. 3310.msr -> 3310_0.png.";
  cout << endl << "                Supported graphic-format-extension: eps, pdf, gif, jpg, png, svg, xpm, root";
  cout << endl << "    --dump <fln> : rather than starting a root session and showing Fourier graphs of the data,";
  cout << endl << "                it will output the Fourier data in an ascii file <fln>.";
  cout << endl << "    --filter  : filter and filter-specific-information -- TO BE WRITTEN YET.";
  cout << endl << "    -b, --background <start> <end>: background interval used to estimate the backround to be";
  cout << endl << "                subtracted before the Fourier transform. <start>, <end> to be given in bins.";
  cout << endl << "    -fo, --fourier-option <fopt>: <fopt> can be 'real', 'power', 'imag', 'real+imag', of 'phase'.";
  cout << endl << "                If this is not defined (neither on the command line nor in the musrFT_startup.xml),";
  cout << endl << "                default will be 'real'.";
  cout << endl << "    -apod, --apodization <val> : <val> can be either 'none', 'weak', 'medium', 'strong'.";
  cout << endl << "                Default will be 'none'.";
  cout << endl << "    -fp, --fourier-power <N> : <N> being the Fourier power, i.e. 2^<N> used for zero padding.";
  cout << endl << "                Default is -1, i.e. no zero padding will be performed.";
  cout << endl << "    -u, --units <units> : <units> is used to define the x-axis of the Fourier transform.";
  cout << endl << "                One may choose between the fields (Gauss) or (Tesla), the frequency (MHz),";
  cout << endl << "                and the angular-frequency domain (Mc/s).";
  cout << endl << "                Default will be 'MHz'.";
  cout << endl << "    -ph, --phase <val> : defines the inital phase <val>. This only is of concern for 'real',";
  cout << endl << "                '<imag>', and 'real+imag'.";
  cout << endl << "                 Default will be 0.0.";
  cout << endl << "    -fr, --fourier-range <start> <end> : Fourier range. <start>, <end> are interpreted in the units given.";
  cout << endl << "                 Default will be -1.0 for both which means, take the full Fourier range.";
  cout << endl << "    -tr, --time-range <start> <end> : time domain range to be used for Fourier transform.";
  cout << endl << "                 <start>, <end> are to be given in (us). If nothing is given, the full time range";
  cout << endl << "                 found in the data file(s) will be used.";
  cout << endl << "    --histo <list> : give the <list> of histograms to be used for the Fourier transform.";
  cout << endl << "                 E.g. musrFT -df lem15_his_01234.root --histo 1 3, will only be needed together with";
  cout << endl << "                 the option --data-file. If multiple data file are given, <list> will apply";
  cout << endl << "                 to all data-files given. If --histo is not given, all histos of a data file will be used.";
  cout << endl << "    -a, --average : show the average of all Fourier transformed data.";
  cout << endl << "    --t0 <list> : A list of t0's can be provided. This in conjunction with --data-file and";
  cout << endl << "                  --fourier-option real allows to get the proper inital phase if t0's are known.";
  cout << endl << "                  If a single t0 for multiple histos is given, it is assume, that this t0 is common";
  cout << endl << "                  to all histos.";
  cout << endl << "                  Example: musrFT -df lem15_his_01234.root -fo real --t0 2750 --histo 1 3";
  cout << endl << "    -pa, --packing <N> : if <N> (an integer), the time domain data will first be packed/rebinned by <N>.";
  cout << endl << "    -t, --title <title> : give a global title for the plot.";
  cout << endl << endl;
}

//-------------------------------------------------------------------------
/**
 * <p>initialize startup parameters.
 *
 * \param startupParam
 */
void musrFT_init(musrFT_startup_param &startupParam)
{
  startupParam.graphicFormat = TString("");
  startupParam.dumpFln = TString("");
  startupParam.bkg[0] = -1;
  startupParam.bkg[1] = -1;
  startupParam.fourierOpt = TString("real");
  startupParam.apotization = TString("none");
  startupParam.fourierPower = -1;
  startupParam.fourierUnits = TString("MHz");
  startupParam.initialPhase = 0.0;
  startupParam.fourierRange[0] = -1.0;
  startupParam.fourierRange[1] = -1.0;
  startupParam.timeRange[0] = -1.0;
  startupParam.timeRange[1] = -1.0;
  startupParam.showAverage = false;
  startupParam.packing = 1;
  startupParam.title = TString("");
}

//-------------------------------------------------------------------------
/**
 * <p>Parses the musrFT --histo options. Allowed --histo options are:
 * \<h0\> \<h1\> ... \<hN\>, e.g. --histo 1 3 5
 * \<h0\>-\<hN\>, e.g. --histo 1-16
 * or a combination of both
 * e.g. --histo 1 3 7-12 15
 *
 * <b>return:</b> true if everything is OK, false otherwise.
 *
 * \param i position of the --histo option within argv. At return it will be shifted
 *        to the last element of the --histo option.
 * \param argc number of elements in argv
 * \param argv list of command line tokens
 * \param startupParam startup parameter structure
 */
bool musrFT_filter_histo(int &i, int argc, char *argv[], musrFT_startup_param &startupParam)
{
  int start = i+1, end = 0;

  // find last element of histo option
  while (++i < argc) {
    if (argv[i][0] == '-') {
      if (!isdigit(argv[i][1]) || (argv[i][1] == '-'))
        break;
    }
  }
  end = i;
  --i;
  if (end < start) {
    cerr << endl << ">> musrFT **ERROR** something is wrong with the --histo arguments." << endl;
    startupParam.histo.clear();
    return false;
  }

  // handle histo arguments
  TString tstr("");
  for (int j=start; j<end; j++) {
    tstr = argv[j];
    if (!tstr.Contains("-")) { // likely to be a single number
      if (tstr.IsDigit()) {
        startupParam.histo.push_back(tstr.Atoi());
      } else { // not a number -> error
        cerr << endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is not a number." << endl;
        startupParam.histo.clear();
        return false;
      }
    } else { // should be something like h0-hN with h0, hN numbers
      TObjArray *tok = tstr.Tokenize("-");
      if (tok->GetEntries() != 2) {
        cerr << endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is not of the form <h0>-<hN>." << endl;
        startupParam.histo.clear();
        return false;
      }
      TObjString *ostr;
      TString sstr("");
      int first=0, last=0;
      ostr = dynamic_cast<TObjString*>(tok->At(0));
      sstr = ostr->GetString();
      if (sstr.IsDigit()) {
        first = sstr.Atoi();
      } else {
        cerr << endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is of the form <h0>-<hN>,";
        cerr << endl << "          but <h0>='" << sstr << "' is not a number." << endl;
        startupParam.histo.clear();
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tok->At(1));
      sstr = ostr->GetString();
      if (sstr.IsDigit()) {
        last = sstr.Atoi();
      } else {
        cerr << endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is of the form <h0>-<hN>,";
        cerr << endl << "          but <hN>='" << sstr << "' is not a number." << endl;
        startupParam.histo.clear();
        return false;
      }

      if (first > last) {
        cerr << endl << ">> musrFT **ERROR** found --histo argument of the form <h0>-<hN> with h0=" << first << " > hN=" << last << "." << endl;
        startupParam.histo.clear();
        return false;
      }

      for (int k=first; k<=last; k++) {
        startupParam.histo.push_back(k);
      }

      // clean up
      if (tok)
        delete tok;
    }
  }

  return true;
}

//-------------------------------------------------------------------------
/**
 * <p>Parses the musrFT command line options.
 *
 * <b>return:</b> 0 if everything is OK, 1 for --version or --help, 2 for an error.
 *
 * \param argc
 * \param argv
 * \param startupParam
 */
int musrFT_parse_options(int argc, char *argv[], musrFT_startup_param &startupParam)
{
  TString tstr("");

  for (int i=1; i<argc; i++) {
    tstr = argv[i];
    if (tstr.BeginsWith("--version")) {
#ifdef HAVE_CONFIG_H
      cout << endl << "musrFT version: " << PACKAGE_VERSION << ", git-rev: " << GIT_REVISION << endl << endl;
#else
      cout << endl << "musrFT git-rev: " << GIT_REVISION << endl << endl;
#endif
      return 1;
    } else if (tstr.BeginsWith("--help")) {
      musrFT_syntax();
      return 1;
    } else if (tstr.BeginsWith("-g") || tstr.BeginsWith("--graphic-format")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --graphic-format without argument!" << endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("eps") && !topt.BeginsWith("pdf") && !topt.BeginsWith("gif") && !topt.BeginsWith("jpg") &&
          !topt.BeginsWith("png") && !topt.BeginsWith("svg") && !topt.BeginsWith("xpm") && !topt.BeginsWith("root")) {
        cerr << endl << ">> musrFT **ERROR** found unrecogniced graphic format '" << topt << "'!" << endl;
        return 2;
      }
      startupParam.graphicFormat = topt;
      i++;
    } else if (tstr.BeginsWith("--dump")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --dump without argument!" << endl;
        return 2;
      }
      startupParam.dumpFln = argv[i+1];
      i++;
    } else if (tstr.Contains("--filter")) {
      cout << endl << "debug> found option filter. NOT YET ANY FUNCTIONALITY." << endl;
    } else if (tstr.Contains("-b") || tstr.Contains("--background")) {
      if (i+2 >= argc) { // something is wrong since there needs to be two arguments here
        cerr << endl << ">> musrFT **ERROR** found option --background with wrong number of arguments." << endl;
        return 2;
      }
      TString bkg[2];
      bkg[0] = argv[i+1];
      bkg[1] = argv[i+2];
      if (!bkg[0].IsDigit()) {
        cerr << endl << ">> musrFT **ERROR** <start> bin of option --background is NOT an int-number! ('" << bkg[0] << "')." << endl;
        return 2;
      }
      if (!bkg[1].IsDigit()) {
        cerr << endl << ">> musrFT **ERROR** <end> bin of option --background is NOT an int-number! ('" << bkg[1] << "')." << endl;
        return 2;
      }
      startupParam.bkg[0] = bkg[0].Atoi();
      startupParam.bkg[1] = bkg[1].Atoi();
      i += 2;
    } else if (tstr.BeginsWith("-fo") || tstr.BeginsWith("--fourier-option")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        cerr << endl << ">> musrFT **ERROR** found option --fourier-option without arguments." << endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("real") && !topt.BeginsWith("imag") && !topt.BeginsWith("power") && !topt.BeginsWith("phase")) {
        cerr << endl << ">> musrFT **ERROR** found option --fourier-option with unrecognized argument '" << topt << "'." << endl;
        return 2;
      }
      startupParam.fourierOpt = topt;
      i++;
    } else if (tstr.BeginsWith("-apod") || tstr.BeginsWith("--apodization")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        cerr << endl << ">> musrFT **ERROR** found option --apodization without arguments." << endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("none") && !topt.BeginsWith("weak") && !topt.BeginsWith("medium") && !topt.BeginsWith("strong")) {
        cerr << endl << ">> musrFT **ERROR** found option --apodization with unrecognized argument '" << topt << "'." << endl;
        return 2;
      }
      startupParam.apotization = topt;
      i++;
    } else if (tstr.BeginsWith("-fp") || tstr.BeginsWith("--fourier-power")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        cerr << endl << ">> musrFT **ERROR** found option --fourier-power without arguments." << endl;
        return 2;
      }
      TString fourierPower(argv[i+1]);
      if (!fourierPower.IsDigit()) {
        cerr << endl << ">> musrFT **ERROR** found option --fourier-power with a power which is not an integer '" << fourierPower << "'." << endl;
        return 2;
      }
      startupParam.fourierPower = fourierPower.Atoi();
      if ((startupParam.fourierPower < 1) || (startupParam.fourierPower > 20)) {
        cerr << endl << ">> musrFT **ERROR** found Fourier power '" << fourierPower << "', which is out of range [1..20]" << endl;
        return 2;
      }
      i++;
    } else if (tstr.BeginsWith("-u") || tstr.BeginsWith("--units")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        cerr << endl << ">> musrFT **ERROR** found option --units without arguments." << endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("MHz", TString::kIgnoreCase) && !topt.BeginsWith("Gauss", TString::kIgnoreCase) &&
          !topt.BeginsWith("Tesla", TString::kIgnoreCase) && !topt.BeginsWith("Mc/s", TString::kIgnoreCase)) {
        cerr << endl << ">> musrFT **ERROR** found option --fourier-option with unrecognized argument '" << topt << "'." << endl;
        return 2;
      }
      startupParam.fourierUnits = topt;
      i++;
    } else if (tstr.BeginsWith("-ph") || tstr.BeginsWith("--phase")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --phase without argument!" << endl;
        return 2;
      }
      TString phase(argv[i+1]);
      if (!phase.IsFloat()) {
        cerr << endl << ">> musrFT **ERROR** found --phase argument '" << phase << "' which is not a number." << endl;
        return 2;
      }
      startupParam.initialPhase = phase.Atof();
      i++;
    } else if (tstr.BeginsWith("-fr") || tstr.BeginsWith("--fourier-range")) {
      if (i+2 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --fourier-range with wrong number of arguments!" << endl;
        return 2;
      }
      TString fourierRange[2] = {argv[i+1], argv[i+2]};
      if (!fourierRange[0].IsFloat() || !fourierRange[1].IsFloat()) {
        cerr << endl << ">> musrFT **ERROR** found invalid --fourier-range arguments '" << fourierRange[0] << "' and/or '" << fourierRange[1] << "'." << endl;
        return 2;
      }
      startupParam.fourierRange[0] = fourierRange[0].Atof();
      startupParam.fourierRange[1] = fourierRange[1].Atof();
      i += 2;
    } else if (tstr.BeginsWith("-tr") || tstr.BeginsWith("--time-range")) {
      if (i+2 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --time-range with wrong number of arguments!" << endl;
        return 2;
      }
      TString timeRange[2] = {argv[i+1], argv[i+2]};
      if (!timeRange[0].IsFloat() || !timeRange[1].IsFloat()) {
        cerr << endl << ">> musrFT **ERROR** found invalid --time-range arguments '" << timeRange[0] << "' and/or '" << timeRange[1] << "'." << endl;
        return 2;
      }
      startupParam.timeRange[0] = timeRange[0].Atof();
      startupParam.timeRange[1] = timeRange[1].Atof();
      i += 2;
    } else if (tstr.BeginsWith("-a") || tstr.BeginsWith("--average")) {
      startupParam.showAverage = true;
    } else if (tstr.BeginsWith("--histo")) {
      if (!musrFT_filter_histo(i, argc, argv, startupParam))
        return 2;
    } else if (tstr.BeginsWith("--t0")) {
      TString topt("");
      while (++i < argc) {
        if (argv[i][0] == '-') {
          --i;
          break;
        } else {
          topt = argv[i];
          if (!topt.IsDigit()) {
            cerr << endl << ">> musrFT **ERROR** found option t0='" << topt << "' which is not a number" << endl;
            return 2;
          }
          startupParam.t0.push_back(topt.Atoi());
        }
      }
      if (startupParam.dataFln.size() == 0) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --t0 without argument!" << endl;
        return 2;
      }
    } else if (tstr.BeginsWith("-t ") || tstr.BeginsWith("--title")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --title without argument!" << endl;
        return 2;
      }
      ++i;
      startupParam.title = argv[i];
    } else if (tstr.BeginsWith("-pa") || tstr.BeginsWith("--packing")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --packing without argument!" << endl;
        return 2;
      }
      ++i;
      TString pack = TString(argv[i]);
      if (!pack.IsDigit()) {
        cerr << endl << ">> musrFT **ERROR** found option --packing with argument '" << pack << "' which is NOT an integer!" << endl;
        return 2;
      }
      startupParam.packing = pack.Atoi();
    } else if (tstr.BeginsWith("-df") || tstr.BeginsWith("--data-file")) {
      while (++i < argc) {
        if (argv[i][0] == '-') {
          --i;
          break;
        } else {
          startupParam.dataFln.push_back(argv[i]);
          TString fln(argv[i]);
          TString fileFormat("??");
          if (fln.Contains(".root", TString::kIgnoreCase))
            fileFormat = "MusrRoot";
          else if (fln.Contains(".bin", TString::kIgnoreCase) || fln.Contains(".mdu", TString::kIgnoreCase))
            fileFormat = "PsiBin";
          else if (fln.Contains(".nxs", TString::kIgnoreCase))
            fileFormat = "NeXus";
          else if (fln.Contains(".msr", TString::kIgnoreCase))
            fileFormat = "Mud";

          if (fileFormat == "??") {
            cerr << endl << ">> musrFT **ERROR** found data file name with unrecognized data file format ('" << fln << "')." << endl;
            return 2;
          } else {
            startupParam.dataFileFormat.push_back(fileFormat);
          }
        }
      }
      if (startupParam.dataFln.size() == 0) { // something is wrong since there needs to be an argument here
        cerr << endl << ">> musrFT **ERROR** found option --data-file without argument!" << endl;
        return 2;
      }
    } else if (tstr.Contains(".msr")) {
      startupParam.msrFln.push_back(tstr);
    } else {
      cerr << endl << ">> musrFT **ERROR** unrecognized option '" << tstr << "' found." << endl;
      return 2;
    }
  }

  // consistency checks
  if ((startupParam.msrFln.size() == 0) && (startupParam.dataFln.size() == 0)) {
    cerr << endl << ">> musrFT **ERROR** neither <msr-file> nor <data-file> defined." << endl;
    return 2;
  }
  if (startupParam.bkg[0] > startupParam.bkg[1]) {
    cerr << endl << ">> musrFT **WARNING** in --background, start=" << startupParam.bkg[0] << " > end=" << startupParam.bkg[1] << ", will swap them." << endl;
    double swap = startupParam.bkg[0];
    startupParam.bkg[0] = startupParam.bkg[1];
    startupParam.bkg[1] = swap;
  }
  if (startupParam.fourierRange[0] > startupParam.fourierRange[1]) {
    cerr << endl << ">> musrFT **WARNING** in --fourier-range, start=" << startupParam.fourierRange[0] << " > end=" << startupParam.fourierRange[1] << ", will swap them." << endl;
    double swap = startupParam.fourierRange[0];
    startupParam.fourierRange[0] = startupParam.fourierRange[1];
    startupParam.fourierRange[1] = swap;
  }

  return 0;
}

void musrFT_getMetaInfo(const TString fln, PRawRunData *rawRunData, TString &metaInfo)
{
  double dval;
  TString str = fln;
  // file name
  // trunc it in case a path-name is given
  size_t idx = str.Last('/');
  if (idx > 0)
    str.Remove(0, idx+1);
  metaInfo = str;
  metaInfo += ",";
  // temperature
  for (UInt_t i=0; i<rawRunData->GetNoOfTemperatures(); i++) {
    metaInfo += TString::Format("T%d=%0.2fK,", i, rawRunData->GetTemperature(i));
  }
  // magnetic field
  dval = rawRunData->GetField();
  if (dval == PMUSR_UNDEFINED)
    metaInfo += TString("B=??,");
  else if (dval < 5000.0)
    metaInfo += TString::Format("B=%0.1fG,", dval);
  else
    metaInfo += TString::Format("B=%0.1fT,", dval/1.0e4);
  // implantation energy
  dval = rawRunData->GetEnergy();
  if (dval == PMUSR_UNDEFINED)
    metaInfo += TString("E=??;");
  else if (dval < 1000.0)
    metaInfo += TString::Format("E=%0.1fkeV;", dval);
  else
    metaInfo += TString::Format("E=%0.1fMeV;", dval/1.0e3);

  metaInfo += *rawRunData->GetCryoName();
  metaInfo += ";";
  metaInfo += *rawRunData->GetSample();
}

//-------------------------------------------------------------------------
void musrFT_estimateT0(musrFT_data &rd)
{
  cout << endl << ">> musrFT **WARNING** try to estimate t0 from maximum in the data set";
  cout << endl << "                      '" << rd.info << "'";
  cout << endl << "                      NO warranty this is sensible!" << endl;

  unsigned int idx = 0;
  double max = rd.rawData[0];
  for (unsigned int i=1; i<rd.rawData.size(); i++) {
    if (rd.rawData[i] > max) {
      max = rd.rawData[i];
      idx = (int)i;
    }
  }
  cout << endl << "debug> estimated t0=" << idx << endl;
  rd.t0 = idx;
}

//-------------------------------------------------------------------------
void musrFT_cleanup(TH1F *h)
/**
 * <p>
 *
 * \param h histogram to be deleted
 */
{
  if (h) {
    delete h;
    h = 0;
  }
}

//-------------------------------------------------------------------------
/**
 * <p>
 *
 * \param data
 */
int musrFT_dumpData(TString fln, vector<PFourier*> &fourierData, double start, double end)
{
  vector<PDoubleVector> data;
  PDoubleVector freq;
  PDoubleVector re;
  PDoubleVector im;
  PDoubleVector pwr;
  TH1F *hRe=0, *hIm=0;
  Double_t dval;

  // make sure start/end are given, otherwise take the minimum/maximum off all data
  hRe = fourierData[0]->GetRealFourier();
  if (start == -1.0) {
    start = hRe->GetBinCenter(1);
    if (end == -1.0)
      end = hRe->GetBinCenter(hRe->GetNbinsX());
  }

  unsigned int minSize = hRe->GetNbinsX()-1;
  musrFT_cleanup(hRe);
  for (unsigned int i=1; i<fourierData.size(); i++) {
    hRe = fourierData[i]->GetRealFourier();
    if (hRe->GetNbinsX()-1 < minSize)
      minSize = hRe->GetNbinsX()-1;
    musrFT_cleanup(hRe);
  }

  for (unsigned int i=0; i<fourierData.size(); i++) {
    hRe = fourierData[i]->GetRealFourier();
    hIm = fourierData[i]->GetImaginaryFourier();
    for (int j=1; j<minSize; j++) {
      dval = hRe->GetBinCenter(j);
      if ((dval >= start) && (dval <= end)) {
        freq.push_back(dval);
        re.push_back(hRe->GetBinContent(j));
        im.push_back(hIm->GetBinContent(j));
        pwr.push_back(hRe->GetBinContent(j)*hRe->GetBinContent(j)+hIm->GetBinContent(j)*hIm->GetBinContent(j));
      }
    }
    data.push_back(freq);
    data.push_back(re);
    data.push_back(im);
    data.push_back(pwr);
    // cleanup
    freq.clear();
    re.clear();
    im.clear();
    pwr.clear();
    musrFT_cleanup(hRe);
    musrFT_cleanup(hIm);
  }

  ofstream fout(fln, ofstream::out);

  // write header
  fout << "% ";
  for (unsigned int i=0; i<fourierData.size()-1; i++)
    fout << "freq" << i << ", Re[d" << i << "], Im[d" << i << "], Pwr[d" << i << "], ";
  fout << "freq" << fourierData.size()-1 << ", Re[d" << fourierData.size()-1 << "], Im[d" << fourierData.size()-1 << "], Pwr[d" << fourierData.size()-1 << "]" << endl;

  // write data
  for (unsigned int j=0; j<data[0].size(); j++) {
    for (unsigned int i=0; i<data.size()-1; i++) {
      fout << data[i][j] << ", ";
    }
    fout << data[data.size()-1][j] << endl;
  }
  fout.close();

  return 0;
}

//---------------------------------------------------
double millitime()
{
  struct timeval now;
  gettimeofday(&now, 0);

  return ((double)now.tv_sec * 1.0e6 + (double)now.tv_usec)/1.0e3;
}

//-------------------------------------------------------------------------
/**
 * <p>
 *
 * \param argc
 * \param argv
 */
int main(int argc, char *argv[])
{
  // only program name alone
  if (argc == 1) {
    musrFT_syntax();
    return PMUSR_SUCCESS;
  }

  musrFT_startup_param startupParam;
  // init startupParam
  musrFT_init(startupParam);

  // parse options
  int status = musrFT_parse_options(argc, argv, startupParam);
  if (status != 0) {
    int retVal = PMUSR_SUCCESS;
    if (status == 2) {
      musrFT_syntax();
      retVal = PMUSR_WRONG_STARTUP_SYNTAX;
    }
    return retVal;
  }

  // read startup file
  char startup_path_name[128];
  PStartupOptions startup_options;
  startup_options.writeExpectedChisq = false;
  startup_options.estimateN0 = true;
  startup_options.alphaEstimateN0 = 0.0;
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    cerr << endl << ">> musrFT **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
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
    //status = saxParser->ParseFile(startup_path_name);
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PStartupHandler.cpp for the definition)
    status = parseXmlFile(saxParser, startup_path_name);
    // check for parse errors
    if (status) { // error
      cerr << endl << ">> musrFT **WARNING** Reading/parsing musrfit_startup.xml failed.";
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
  startupHandler->SetStartupOptions(startup_options);

  // defines the raw time-domain data vector
  PPrepFourier data(startupParam.bkg, startupParam.packing);

  // load msr-file(s)
  cout << endl << "debug> before reading msr-files ...";
  vector<PMsrHandler*> msrHandler;
  msrHandler.resize(startupParam.msrFln.size());
  for (unsigned int i=0; i<startupParam.msrFln.size(); i++) {
    msrHandler[i] = new PMsrHandler(startupParam.msrFln[i].Data(), startupHandler->GetStartupOptions(), true);
    status = msrHandler[i]->ReadMsrFile();
    if (status != PMUSR_SUCCESS) {
      switch (status) {
        case PMUSR_MSR_FILE_NOT_FOUND:
          cout << endl << ">> musrFT **ERROR** couldn't find " << startupParam.msrFln[i] << endl << endl;
          break;
        case PMUSR_MSR_SYNTAX_ERROR:
          cout << endl << ">> musrFT **SYNTAX ERROR** in file " << startupParam.msrFln[i] << ", full stop here." << endl << endl;
          break;
        default:
          cout << endl << ">> musrFT **UNKOWN ERROR** when trying to read the msr-file" << endl << endl;
          break;
      }
      return status;
    }
  }

  cout << endl << "debug> before reading data-files ...";
  vector<PRunDataHandler*> runDataHandler;
  runDataHandler.resize(startupParam.msrFln.size()+startupParam.dataFln.size()); // resize to the total number of run data provided
  // load data-file(s) related to msr-file
  for (unsigned int i=0; i<msrHandler.size(); i++) {
    // create run data handler
    if (startupHandler)
      runDataHandler[i] = new PRunDataHandler(msrHandler[i], startupHandler->GetDataPathList());
    else
      runDataHandler[i] = new PRunDataHandler(msrHandler[i]);
  }

  // load data-file(s) provided directly
  for (unsigned int i=msrHandler.size(); i<msrHandler.size()+startupParam.dataFln.size(); i++) {
    cout << endl << "debug> dataFln[" << i-msrHandler.size() << "]=" << startupParam.dataFln[i-msrHandler.size()];
    // create run data handler
    if (startupHandler)
      runDataHandler[i] = new PRunDataHandler(startupParam.dataFln[i-msrHandler.size()], startupParam.dataFileFormat[i-msrHandler.size()], startupHandler->GetDataPathList());
    else
      runDataHandler[i] = new PRunDataHandler(startupParam.dataFln[i-msrHandler.size()], startupParam.dataFileFormat[i-msrHandler.size()]);

    // read the data files
    runDataHandler[i]->ReadData();

    if (!runDataHandler[i]->IsAllDataAvailable()) {
      cerr << endl << ">> musrFT **ERROR** couldn't read data-file '" << startupParam.dataFln[i-msrHandler.size()] << "'.";
      return PMUSR_DATA_FILE_READ_ERROR;
    }

    // dig out all the necessary time domain data
    PRawRunData *rawRunData = runDataHandler[i]->GetRunData();
    if (rawRunData == 0) {
      cerr << endl << ">> musrFT **ERROR** couldn't obtain the raw run data set for " << startupParam.dataFln[i-msrHandler.size()] << endl;
      return PMUSR_DATA_FILE_READ_ERROR;
    }

    // first check of histo list makes sense
    for (unsigned int j=0; j<startupParam.histo.size(); j++) {
      if ((unsigned int)startupParam.histo[j] > rawRunData->GetNoOfHistos()) {
        cerr << endl << ">> musrFT **ERROR** found histo no " << startupParam.histo[j] << " > # of histo in the file (";
        cerr << startupParam.dataFln[i-msrHandler.size()] << " // # histo: " << rawRunData->GetNoOfHistos() << ")." << endl;
        return PMUSR_DATA_FILE_READ_ERROR;
      }
    }

    musrFT_data rd;
    TString str("");
    unsigned int idx=0;
    musrFT_getMetaInfo(startupParam.dataFln[i-msrHandler.size()], rawRunData, str);
    for (unsigned int j=0; j<startupParam.histo.size(); j++) {
      idx = startupParam.histo[j];
      // meta information
      rd.info  = TString::Format("h%d:", idx);
      rd.info += str;
/*
      cout << endl << "debug> ++++++++";
      cout << endl << "debug> info: " << rd.info;
*/
      // time resolution
      rd.timeResolution = rawRunData->GetTimeResolution() / 1.0e3; // time resolution in (us)
//      cout << endl << "debug> time resolution: " << rd.timeResolution << " (us)";
      // time range
      rd.timeRange[0] = startupParam.timeRange[0]; // in (us)
      rd.timeRange[1] = startupParam.timeRange[1]; // in (us)
//      cout << endl << "debug> time range: " << rd.timeRange[0] << ".." << rd.timeRange[1] << " (us)";
      // data set
      rd.rawData.clear();
      rd.rawData = *(rawRunData->GetDataBin(idx));
      // check for potentially given t0's
      rd.t0 = -1;
      if (startupParam.t0.size() == 1)
        rd.t0 = startupParam.t0[0];
      else if (j < startupParam.t0.size())
        rd.t0 = startupParam.t0[j];
//      cout << endl << "debug> t0 = " << rd.t0;
      if (rd.t0 == -1) { // no t0 given, try to estimate it
        musrFT_estimateT0(rd);
      }
/*
      cout << endl << "debug> rd.rawData.size()=" << rd.rawData.size();
      cout << endl << "debug> rd.rawData: h" << idx << ": ";
*/
      for (unsigned int k=rd.t0; k<rd.t0+15; k++)
        cout << rd.rawData[k] << ", ";
      data.AddData(rd);
    }
  }

/*
  cout << endl << "debug> +++++++++";
  cout << endl << "debug> data.GetNoOfData()=" << data.GetNoOfData();
*/

  // calculate background levels and subtract them from the data
  data.DoBkgCorrection();

  // do the time domain filtering now
  data.DoFiltering();

  // do packing
  data.DoPacking();

  // get all the corrected data
  vector<TH1F*> histo = data.GetData();
/*
  if (histo.size() == 0)
    cout << endl << "debug> histo.size()==0!! :-(" << endl;
  for (unsigned int i=0; i<histo.size(); i++) {
    cout << endl << "debug> histo: time resolution " << histo[i]->GetBinWidth(1) << " (ns).";
    cout << endl << "debug> histo[" << i << "]->GetNbinsX()=" << histo[i]->GetNbinsX();
    cout << endl << "debug> histo data: ";
    for (unsigned int j=1; j<=15; j++)
      cout << histo[i]->GetBinContent(j) << ", ";
    cout << endl;
  }
*/
  // prepare Fourier
  Int_t unitTag = FOURIER_UNIT_NOT_GIVEN;
  if (startupParam.fourierUnits.BeginsWith("gauss", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_GAUSS;
  else if (startupParam.fourierUnits.BeginsWith("tesla", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_TESLA;
  else if (startupParam.fourierUnits.BeginsWith("mhz", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_FREQ;
  else if (startupParam.fourierUnits.BeginsWith("mc/s", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_CYCLES;

  vector<PFourier*> fourier;
  fourier.resize(histo.size());
  for (unsigned int i=0; i<fourier.size(); i++) {
    fourier[i] = new PFourier(histo[i], unitTag, 0.0, 0.0, true, startupParam.fourierPower);
  }

  // Fourier transform data
  Int_t apodTag = F_APODIZATION_NONE;
  if (startupParam.apotization.BeginsWith("weak", TString::kIgnoreCase))
    apodTag = F_APODIZATION_WEAK;
  else if (startupParam.apotization.BeginsWith("medium", TString::kIgnoreCase))
    apodTag = F_APODIZATION_MEDIUM;
  else if (startupParam.apotization.BeginsWith("strong", TString::kIgnoreCase))
    apodTag = F_APODIZATION_STRONG;

  double start = millitime();
  for (unsigned int i=0; i<fourier.size(); i++) {
    fourier[i]->Transform(apodTag);
  }
  double end = millitime();
  cout << endl << "debug> after FFT. calculation time: " << (end-start)/1.0e3  << " (sec)." << endl;

  // if Fourier dumped if whished do it now
  if (startupParam.dumpFln.Length() > 0) {
    musrFT_dumpData(startupParam.dumpFln, fourier, startupParam.fourierRange[0], startupParam.fourierRange[1]);
  } else {

  // if Fourier graphical export is whished, switch to batch mode

  // plot the Fourier transform

  }


  // cleanup
  if (startupHandler)
    delete startupHandler;

  for (unsigned int i=0; i<msrHandler.size(); i++)
    if (msrHandler[i])
      delete msrHandler[i];
  msrHandler.clear();

  for (unsigned int i=0; i<runDataHandler.size(); i++)
    if (runDataHandler[i])
      delete runDataHandler[i];
  runDataHandler.clear();

  if (histo.size() > 0) {
    for (unsigned int i=0; i<histo.size(); i++)
      delete histo[i];
    histo.clear();
  }
  if (fourier.size() > 0) {
    for (unsigned int i=0; i<fourier.size(); i++)
      delete fourier[i];
    fourier.clear();
  }

  return PMUSR_SUCCESS;
}
