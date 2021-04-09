/***************************************************************************

  musrFT.cpp

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

#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <TApplication.h>
#include <TROOT.h>
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
#include "PFourierCanvas.h"

//----------------------------------------------------------------------------
/**
 * <p>Structure keeping the command line options.
 */
typedef struct {
  PStringVector msrFln;           ///< msr-file names to be used.
  PStringVector dataFln;          ///< raw-data-file names to be used.
  PStringVector dataFileFormat;   ///< file format guess
  TString graphicFormat;          ///< format for the graphical output dump
  TString dumpFln;                ///< dump file name for Fourier data output
  TString msrFlnOut;              ///< dump file name for msr-file generation
  Int_t bkg_range[2];             ///< background range
  PDoubleVector bkg;              ///< background value
  TString fourierOpt;             ///< Fourier options, i.e. real, imag, power, phase, phaseOptReal
  TString apodization;            ///< apodization setting: none, weak, medium, strong
  Int_t fourierPower;             ///< Fourier power for zero padding, i.e. 2^fourierPower points
  TString fourierUnits;           ///< wished Fourier units: Gauss, Tesla, MHz, Mc/s
  Double_t initialPhase;          ///< inital Fourier phase for Real/Imag
  Double_t fourierRange[2];       ///< Fourier range to be plotted. Given in the choosen units.
  Double_t timeRange[2];          ///< time range used for the Fourier
  PIntVector histo;               ///< selection of the histos used from at data file for Fourier
  Bool_t showAverage;             ///< flag indicating if initially the Fourier average over the given histos shall be plotted, this over ALL data sets.
  Bool_t showAveragePerDataSet;   ///< flag indicating if initially the Fourier average over the given histos shall be plotted, this per data set.
  PIntVector t0;                  ///< t0 vector for the histos. If not given t0's will be estimated.
  Int_t packing;                  ///< packing for rebinning the time histograms before Fourier transform.
  TString title;                  ///< title to be shown for the Fourier plot.
  Double_t lifetimecorrection;    ///< is == 0.0 for NO life time correction, otherwise it holds the fudge factor
  Int_t timeout;                  ///< timeout in (sec) after which musrFT will terminate. if <= 0, no automatic termination will take place.
} musrFT_startup_param;

//-------------------------------------------------------------------------
/**
 * <p>prints the musrFT usage.
 */
void musrFT_syntax()
{
  std::cout << std::endl << "usage: musrFT [Options] [<msr-files> | -df, --data-file <data-file>]";
  std::cout << std::endl << "  <msr-files> : msr-file name(s). These msr-files are used for the Fourier transform.";
  std::cout << std::endl << "                It can be a list of msr-files, e.g. musrFT 3110.msr 3111.msr";
  std::cout << std::endl << "                For the syntax of the msr-file check the user manual of musrfit.";
  std::cout << std::endl << "  -df, --data-file <data-file> : This allows to feed only muSR data file(s) to";
  std::cout << std::endl << "                perform the Fourier transform. Since the extended <msr-file> information";
  std::cout << std::endl << "                are missing, they will need to be provided by to options, or musrFT";
  std::cout << std::endl << "                tries to guess, based on musrfit_startup.xml settings.";
  std::cout << std::endl << "  Options: ";
  std::cout << std::endl << "    --help    : display this help and exit";
  std::cout << std::endl << "    --version : output version information and exit";
  std::cout << std::endl << "    -g, --graphic-format <graphic-format-extension> : ";
  std::cout << std::endl << "                will produce a graphic-output-file without starting a root session.";
  std::cout << std::endl << "                the name is based either on the <msr-file> or the <data-file>,";
  std::cout << std::endl << "                e.g. 3310.msr -> 3310_0.png.";
  std::cout << std::endl << "                Supported graphic-format-extension: eps, pdf, gif, jpg, png, svg, xpm, root";
  std::cout << std::endl << "    --dump <fln> : rather than starting a root session and showing Fourier graphs of the data,";
  std::cout << std::endl << "                it will output the Fourier data in an ascii file <fln>.";
  std::cout << std::endl << "    -br, --background-range <start> <end>: background interval used to estimate the background to be";
  std::cout << std::endl << "                subtracted before the Fourier transform. <start>, <end> to be given in bins.";
  std::cout << std::endl << "    -bg, --background <list> : gives the background explicit for each histogram.";
  std::cout << std::endl << "    -fo, --fourier-option <fopt>: <fopt> can be 'real', 'imag', 'real+imag', 'power', 'phase', or 'phaseOptReal'.";
  std::cout << std::endl << "                If this is not defined (neither on the command line nor in the musrfit_startup.xml),";
  std::cout << std::endl << "                default will be 'power'.";
  std::cout << std::endl << "    -ap, --apodization <val> : <val> can be either 'none', 'weak', 'medium', 'strong'.";
  std::cout << std::endl << "                Default will be 'none'.";
  std::cout << std::endl << "    -fp, --fourier-power <N> : <N> being the Fourier power, i.e. 2^<N> used for zero padding.";
  std::cout << std::endl << "                Default is -1, i.e. no zero padding will be performed.";
  std::cout << std::endl << "    -u, --units <units> : <units> is used to define the x-axis of the Fourier transform.";
  std::cout << std::endl << "                One may choose between the fields (Gauss) or (Tesla), the frequency (MHz),";
  std::cout << std::endl << "                and the angular-frequency domain (Mc/s).";
  std::cout << std::endl << "                Default will be 'MHz'.";
  std::cout << std::endl << "    -ph, --phase <val> : defines the initial phase <val>. This only is of concern for 'real',";
  std::cout << std::endl << "                '<imag>', and 'real+imag'.";
  std::cout << std::endl << "                 Default will be 0.0.";
  std::cout << std::endl << "    -fr, --fourier-range <start> <end> : Fourier range. <start>, <end> are interpreted in the units given.";
  std::cout << std::endl << "                 Default will be -1.0 for both which means, take the full Fourier range.";
  std::cout << std::endl << "    -tr, --time-range <start> <end> : time domain range to be used for Fourier transform.";
  std::cout << std::endl << "                 <start>, <end> are to be given in (us). If nothing is given, the full time range";
  std::cout << std::endl << "                 found in the data file(s) will be used.";
  std::cout << std::endl << "    --histo <list> : give the <list> of histograms to be used for the Fourier transform.";
  std::cout << std::endl << "                 E.g. musrFT -df lem15_his_01234.root --histo 1 3, will only be needed together with";
  std::cout << std::endl << "                 the option --data-file. If multiple data file are given, <list> will apply";
  std::cout << std::endl << "                 to all data-files given. If --histo is not given, all histos of a data file will be used.";
  std::cout << std::endl << "                 <list> can be anything like: 2 3 6, or 2-17, or 1-6 9, etc.";
  std::cout << std::endl << "    -a, --average : show the average of ALL Fourier transformed data.";
  std::cout << std::endl << "    -ad, --average-per-data-set : show the average of the Fourier transformed data per data set.";
  std::cout << std::endl << "    --t0 <list> : A list of t0's can be provided. This in conjunction with --data-file and";
  std::cout << std::endl << "                 --fourier-option real allows to get the proper initial phase if t0's are known.";
  std::cout << std::endl << "                 If a single t0 for multiple histos is given, it is assume, that this t0 is common";
  std::cout << std::endl << "                 to all histos.";
  std::cout << std::endl << "                 Example: musrFT -df lem15_his_01234.root -fo real --t0 2750 --histo 1 3";
  std::cout << std::endl << "    -pa, --packing <N> : if <N> (an integer), the time domain data will first be packed/rebinned by <N>.";
  std::cout << std::endl << "    --title <title> : give a global title for the plot.";
  std::cout << std::endl << "    --create-msr-file <fln> : creates a msr-file based on the command line options";
  std::cout << std::endl << "                 provided. This will help on the way to a full fitting model.";
  std::cout << std::endl << "    -lc, --lifetimecorrection <fudge>: try to eliminate muon life time decay. Only makes sense for low";
  std::cout << std::endl << "                 transverse fields. <fudge> is a tweaking factor and should be kept around 1.0.";
  std::cout << std::endl << "    --timeout <timeout> : <timeout> given in seconds after which musrFT terminates.";
  std::cout << std::endl << "                 If <timeout> <= 0, no timeout will take place. Default <timeout> is 3600.";
  std::cout << std::endl << std::endl;
}

//-------------------------------------------------------------------------
/**
 * <p>initialize startup parameters.
 *
 * \param startupParam command line options
 */
void musrFT_init(musrFT_startup_param &startupParam)
{
  startupParam.graphicFormat = TString("");
  startupParam.dumpFln = TString("");
  startupParam.msrFlnOut = TString("");
  startupParam.bkg_range[0] = -1;
  startupParam.bkg_range[1] = -1;
  startupParam.fourierOpt = TString("??");
  startupParam.apodization = TString("none");
  startupParam.fourierPower = -1;
  startupParam.fourierUnits = TString("??");
  startupParam.initialPhase = 0.0;
  startupParam.fourierRange[0] = -1.0;
  startupParam.fourierRange[1] = -1.0;
  startupParam.timeRange[0] = -1.0;
  startupParam.timeRange[1] = -1.0;
  startupParam.showAverage = false;
  startupParam.showAveragePerDataSet = false;
  startupParam.packing = 1;
  startupParam.title = TString("");
  startupParam.lifetimecorrection = 0.0;
  startupParam.timeout = 3600;
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
Bool_t musrFT_filter_histo(Int_t &i, Int_t argc, Char_t *argv[], musrFT_startup_param &startupParam)
{
  Int_t start = i+1, end = 0;

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
    std::cerr << std::endl << ">> musrFT **ERROR** something is wrong with the --histo arguments." << std::endl;
    startupParam.histo.clear();
    return false;
  }

  // handle histo arguments
  TString tstr("");
  for (Int_t j=start; j<end; j++) {
    tstr = argv[j];
    if (!tstr.Contains("-")) { // likely to be a single number
      if (tstr.IsDigit()) {
        startupParam.histo.push_back(tstr.Atoi());
      } else { // not a number -> error
        std::cerr << std::endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is not a number." << std::endl;
        startupParam.histo.clear();
        return false;
      }
    } else { // should be something like h0-hN with h0, hN numbers
      TObjArray *tok = tstr.Tokenize("-");
      if (tok->GetEntries() != 2) {
        std::cerr << std::endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is not of the form <h0>-<hN>." << std::endl;
        startupParam.histo.clear();
        return false;
      }
      TObjString *ostr;
      TString sstr("");
      Int_t first=0, last=0;
      ostr = dynamic_cast<TObjString*>(tok->At(0));
      sstr = ostr->GetString();
      if (sstr.IsDigit()) {
        first = sstr.Atoi();
      } else {
        std::cerr << std::endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is of the form <h0>-<hN>,";
        std::cerr << std::endl << "          but <h0>='" << sstr << "' is not a number." << std::endl;
        startupParam.histo.clear();
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tok->At(1));
      sstr = ostr->GetString();
      if (sstr.IsDigit()) {
        last = sstr.Atoi();
      } else {
        std::cerr << std::endl << ">> musrFT **ERROR** found --histo argument '" << tstr << "' which is of the form <h0>-<hN>,";
        std::cerr << std::endl << "          but <hN>='" << sstr << "' is not a number." << std::endl;
        startupParam.histo.clear();
        return false;
      }

      if (first > last) {
        std::cerr << std::endl << ">> musrFT **ERROR** found --histo argument of the form <h0>-<hN> with h0=" << first << " > hN=" << last << "." << std::endl;
        startupParam.histo.clear();
        return false;
      }

      for (Int_t k=first; k<=last; k++) {
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
 * \param argc number of command line arguments
 * \param argv command line argument array
 * \param startupParam command line data structure
 */
Int_t musrFT_parse_options(Int_t argc, Char_t *argv[], musrFT_startup_param &startupParam)
{
  TString tstr("");

  for (Int_t i=1; i<argc; i++) {
    tstr = argv[i];
    if (tstr.BeginsWith("--version")) {
#ifdef HAVE_CONFIG_H
      std::cout << std::endl << "musrFT version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << ", ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "musrFT git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#endif
      return 1;
    } else if (tstr.BeginsWith("--help")) {
      musrFT_syntax();
      return 1;
    } else if (tstr.BeginsWith("-g") || tstr.BeginsWith("--graphic-format")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --graphic-format without argument!" << std::endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("eps") && !topt.BeginsWith("pdf") && !topt.BeginsWith("gif") && !topt.BeginsWith("jpg") &&
          !topt.BeginsWith("png") && !topt.BeginsWith("svg") && !topt.BeginsWith("xpm") && !topt.BeginsWith("root")) {
        std::cerr << std::endl << ">> musrFT **ERROR** found unrecogniced graphic format '" << topt << "'!" << std::endl;
        return 2;
      }
      startupParam.graphicFormat = topt;
      i++;
    } else if (tstr.BeginsWith("--dump")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --dump without argument!" << std::endl;
        return 2;
      }
      startupParam.dumpFln = argv[i+1];
      i++;
    } else if (tstr.Contains("-br") || tstr.Contains("--background-range")) {
      if (i+2 >= argc) { // something is wrong since there needs to be two arguments here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --background-range with wrong number of arguments." << std::endl;
        return 2;
      }
      TString bkgRange[2];
      bkgRange[0] = argv[i+1];
      bkgRange[1] = argv[i+2];
      if (!bkgRange[0].IsDigit()) {
        std::cerr << std::endl << ">> musrFT **ERROR** <start> bin of option --background-range is NOT an int-number! ('" << bkgRange[0] << "')." << std::endl;
        return 2;
      }
      if (!bkgRange[1].IsDigit()) {
        std::cerr << std::endl << ">> musrFT **ERROR** <end> bin of option --background-range is NOT an int-number! ('" << bkgRange[1] << "')." << std::endl;
        return 2;
      }
      startupParam.bkg_range[0] = bkgRange[0].Atoi();
      startupParam.bkg_range[1] = bkgRange[1].Atoi();
      i += 2;
    } else if (tstr.BeginsWith("-bg") || !tstr.CompareTo("--background")) {
      TString topt("");
      while (++i < argc) {
        if (argv[i][0] == '-') {
          --i;
          break;
        } else {
          topt = argv[i];
          if (!topt.IsFloat()) {
            std::cerr << std::endl << ">> musrFT **ERROR** found option --background='" << topt << "' which is not a float" << std::endl;
            return 2;
          }
          startupParam.bkg.push_back(topt.Atoi());
        }
      }
      if (startupParam.bkg.size() == 0) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --background without argument!" << std::endl;
        return 2;
      }
    } else if (tstr.BeginsWith("-fo") || tstr.BeginsWith("--fourier-option")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --fourier-option without arguments." << std::endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("real") && !topt.BeginsWith("imag") && !topt.BeginsWith("power") &&
          !topt.BeginsWith("phase") && !topt.BeginsWith("phaseOptReal")) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --fourier-option with unrecognized argument '" << topt << "'." << std::endl;
        return 2;
      }
      startupParam.fourierOpt = topt;
      i++;
    } else if (tstr.BeginsWith("-ap") || tstr.BeginsWith("--apodization")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --apodization without arguments." << std::endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("none") && !topt.BeginsWith("weak") && !topt.BeginsWith("medium") && !topt.BeginsWith("strong")) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --apodization with unrecognized argument '" << topt << "'." << std::endl;
        return 2;
      }
      startupParam.apodization = topt;
      i++;
    } else if (tstr.BeginsWith("-fp") || tstr.BeginsWith("--fourier-power")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --fourier-power without arguments." << std::endl;
        return 2;
      }
      TString fourierPower(argv[i+1]);
      if (!fourierPower.IsDigit()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --fourier-power with a power which is not an integer '" << fourierPower << "'." << std::endl;
        return 2;
      }
      startupParam.fourierPower = fourierPower.Atoi();
      if ((startupParam.fourierPower < 1) || (startupParam.fourierPower > 20)) {
        std::cerr << std::endl << ">> musrFT **ERROR** found Fourier power '" << fourierPower << "', which is out of range [1..20]" << std::endl;
        return 2;
      }
      i++;
    } else if (tstr.BeginsWith("-u") || tstr.BeginsWith("--units")) {
      if (i+1 >= argc) { // something is wrong since there needs to be two arguments here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --units without arguments." << std::endl;
        return 2;
      }
      TString topt(argv[i+1]);
      if (!topt.BeginsWith("MHz", TString::kIgnoreCase) && !topt.BeginsWith("Gauss", TString::kIgnoreCase) &&
          !topt.BeginsWith("Tesla", TString::kIgnoreCase) && !topt.BeginsWith("Mc/s", TString::kIgnoreCase)) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --fourier-option with unrecognized argument '" << topt << "'." << std::endl;
        return 2;
      }
      startupParam.fourierUnits = topt;
      i++;
    } else if (tstr.BeginsWith("-ph") || tstr.BeginsWith("--phase")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --phase without argument!" << std::endl;
        return 2;
      }
      TString phase(argv[i+1]);
      if (!phase.IsFloat()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found --phase argument '" << phase << "' which is not a number." << std::endl;
        return 2;
      }
      startupParam.initialPhase = phase.Atof();
      i++;
    } else if (tstr.BeginsWith("-fr") || tstr.BeginsWith("--fourier-range")) {
      if (i+2 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --fourier-range with wrong number of arguments!" << std::endl;
        return 2;
      }
      TString fourierRange[2] = {argv[i+1], argv[i+2]};
      if (!fourierRange[0].IsFloat() || !fourierRange[1].IsFloat()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found invalid --fourier-range arguments '" << fourierRange[0] << "' and/or '" << fourierRange[1] << "'." << std::endl;
        return 2;
      }
      startupParam.fourierRange[0] = fourierRange[0].Atof();
      startupParam.fourierRange[1] = fourierRange[1].Atof();
      i += 2;
    } else if (tstr.BeginsWith("-tr") || tstr.BeginsWith("--time-range")) {
      if (i+2 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --time-range with wrong number of arguments!" << std::endl;
        return 2;
      }
      TString timeRange[2] = {argv[i+1], argv[i+2]};
      if (!timeRange[0].IsFloat() || !timeRange[1].IsFloat()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found invalid --time-range arguments '" << timeRange[0] << "' and/or '" << timeRange[1] << "'." << std::endl;
        return 2;
      }
      startupParam.timeRange[0] = timeRange[0].Atof();
      startupParam.timeRange[1] = timeRange[1].Atof();
      i += 2;
    } else if (!tstr.CompareTo("-a") || !tstr.CompareTo("--average")) {
      startupParam.showAverage = true;
    } else if (!tstr.CompareTo("-ad") || !tstr.CompareTo("--average-per-data-set")) {
      startupParam.showAveragePerDataSet = true;
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
            std::cerr << std::endl << ">> musrFT **ERROR** found option t0='" << topt << "' which is not a number" << std::endl;
            return 2;
          }
          startupParam.t0.push_back(topt.Atoi());
        }
      }
      if (startupParam.t0.size() == 0) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --t0 without argument!" << std::endl;
        return 2;
      }
    } else if (tstr.BeginsWith("--title")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --title without argument!" << std::endl;
        return 2;
      }
      ++i;
      startupParam.title = argv[i];
    } else if (tstr.BeginsWith("-pa") || tstr.BeginsWith("--packing")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --packing without argument!" << std::endl;
        return 2;
      }
      ++i;
      TString pack = TString(argv[i]);
      if (!pack.IsDigit()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --packing with argument '" << pack << "' which is NOT an integer!" << std::endl;
        return 2;
      }
      startupParam.packing = pack.Atoi();
    } else if (tstr.BeginsWith("--create-msr-file")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --create-msr-file without argument!" << std::endl;
        return 2;
      }
      ++i;
      startupParam.msrFlnOut = TString(argv[i]);
    } else if (tstr.BeginsWith("-lc") || tstr.BeginsWith("--lifetimecorrection")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --lifetimecorrection without argument!" << std::endl;
        return 2;
      }
      ++i;
      TString fudge(argv[i]);
      if (!fudge.IsFloat()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --lifetimecorrection with a fudge which is not a double '" << fudge << "'." << std::endl;
        return 2;
      }
      startupParam.lifetimecorrection = fudge.Atof();
    } else if (tstr.BeginsWith("--timeout")) {
      if (i+1 >= argc) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --timeout without argument!" << std::endl;
        return 2;
      }
      ++i;
      TString tt(argv[i]);
      if (!tt.IsDigit()) {
        std::cerr << std::endl << ">> musrFT **ERROR** found option --timeout with a <timeout> which is not an integer '" << tt << "'." << std::endl;
        return 2;
      }
      startupParam.timeout = tt.Atoi();
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
          else if (fln.Contains(".bin", TString::kIgnoreCase))
            fileFormat = "PsiBin";
          else if (fln.Contains(".mdu", TString::kIgnoreCase))
            fileFormat = "PsiMdu";
          else if (fln.Contains(".nxs", TString::kIgnoreCase))
            fileFormat = "NeXus";
          else if (fln.Contains(".msr", TString::kIgnoreCase))
            fileFormat = "Mud";

          if (fileFormat == "??") {
            std::cerr << std::endl << ">> musrFT **ERROR** found data file name with unrecognized data file format ('" << fln << "')." << std::endl;
            return 2;
          } else {
            startupParam.dataFileFormat.push_back(fileFormat);
          }
        }
      }
      if (startupParam.dataFln.size() == 0) { // something is wrong since there needs to be an argument here
        std::cerr << std::endl << ">> musrFT **ERROR** found option --data-file without argument!" << std::endl;
        return 2;
      }
    } else if (tstr.Contains(".msr")) {
      startupParam.msrFln.push_back(tstr);
    } else {
      std::cerr << std::endl << ">> musrFT **ERROR** unrecognized option '" << tstr << "' found." << std::endl;
      return 2;
    }
  }

  // consistency checks
  if ((startupParam.msrFln.size() == 0) && (startupParam.dataFln.size() == 0)) {
    std::cerr << std::endl << ">> musrFT **ERROR** neither <msr-file> nor <data-file> defined." << std::endl;
    return 2;
  }
  if (startupParam.bkg_range[0] > startupParam.bkg_range[1]) {
    std::cerr << std::endl << ">> musrFT **WARNING** in --background-range, start=" << startupParam.bkg_range[0] << " > end=" << startupParam.bkg_range[1] << ", will swap them." << std::endl;
    Double_t swap = startupParam.bkg_range[0];
    startupParam.bkg_range[0] = startupParam.bkg_range[1];
    startupParam.bkg_range[1] = swap;
  }
  if (startupParam.fourierRange[0] > startupParam.fourierRange[1]) {
    std::cerr << std::endl << ">> musrFT **WARNING** in --fourier-range, start=" << startupParam.fourierRange[0] << " > end=" << startupParam.fourierRange[1] << ", will swap them." << std::endl;
    Double_t swap = startupParam.fourierRange[0];
    startupParam.fourierRange[0] = startupParam.fourierRange[1];
    startupParam.fourierRange[1] = swap;
  }
  if (startupParam.showAverage && startupParam.showAveragePerDataSet) {
    std::cerr << std::endl << ">> musrFT **WARNING** Options: --average and --average-per-data-set exclude each other, will choose the latter." << std::endl;
    startupParam.showAverage = false;
  }

  return 0;
}

//----------------------------------------------------------------------------------------
/**
 * <p>Collects the meta information form the raw-data-file.
 *
 * \param fln file name of the raw-data-file
 * \param rawRunData raw-data-file object
 * \param metaInfo return string which will contain the meta information.
 */
void musrFT_getMetaInfo(const TString fln, PRawRunData *rawRunData, TString &metaInfo)
{
  Double_t dval;
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
/**
 * <p>Estimates the t0's of the raw-data-files. It simply is looking for the
 * maximum of the raw-data (assuming a prompt peak). This will fail for LEM
 * and ISIS data for sure.
 *
 * \param rd raw-data-file collection (see PPrepFourier.h)
 */
void musrFT_estimateT0(musrFT_data &rd)
{
  std::cout << std::endl << ">> musrFT **WARNING** try to estimate t0 from maximum in the data set";
  std::cout << std::endl << "                      '" << rd.info << "'";
  std::cout << std::endl << "                      NO warranty this is sensible!" << std::endl;

  UInt_t idx = 0;
  Double_t max = rd.rawData[0];
  for (UInt_t i=1; i<rd.rawData.size(); i++) {
    if (rd.rawData[i] > max) {
      max = rd.rawData[i];
      idx = (Int_t)i;
    }
  }
  std::cout << std::endl << ">> musrFT_estimateT0: estimated t0=" << idx << std::endl;
  rd.t0 = idx;
}

//-------------------------------------------------------------------------
/**
 * <p> deletes a histogram.
 *
 * \param h point to a ROOT histogram object
 */
void musrFT_cleanup(TH1F *h)
{
  if (h) {
    delete h;
    h = 0;
  }
}

//-------------------------------------------------------------------------
/**
 * <p>Dump the Fourier transformed data into an ascii file.
 *
 * \param fln dump file name
 * \param fourierData collection of all the Fourier transformed data.
 * \param start starting point from where the data shall be written to file.
 * \param end ending point up to where the data shall be written to file.
 */
Int_t musrFT_dumpData(TString fln, std::vector<PFourier*> &fourierData, Double_t start, Double_t end)
{
  std::vector<PDoubleVector> data;
  PDoubleVector freq;
  PDoubleVector re;
  PDoubleVector im;
  PDoubleVector pwr;
  TH1F *hRe=nullptr, *hIm=nullptr;
  Double_t dval;

  // make sure start/end are given, otherwise take the minimum/maximum off all data
  hRe = fourierData[0]->GetRealFourier();
  if (start == -1.0) {
    start = hRe->GetBinCenter(1);
    if (end == -1.0)
      end = hRe->GetBinCenter(hRe->GetNbinsX());
  }

  UInt_t minSize = hRe->GetNbinsX()-1;
  musrFT_cleanup(hRe);
  for (UInt_t i=1; i<fourierData.size(); i++) {
    hRe = fourierData[i]->GetRealFourier();
    if (hRe->GetNbinsX()-1 < (Int_t)minSize)
      minSize = hRe->GetNbinsX()-1;
    musrFT_cleanup(hRe);
  }

  for (UInt_t i=0; i<fourierData.size(); i++) {
    hRe = fourierData[i]->GetRealFourier();
    hIm = fourierData[i]->GetImaginaryFourier();
    for (Int_t j=1; j<(Int_t)minSize; j++) {
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

  std::ofstream fout(fln, std::ofstream::out);

  // write header
  fout << "% ";
  for (UInt_t i=0; i<fourierData.size()-1; i++)
    fout << "freq" << i << ", Re[d" << i << "], Im[d" << i << "], Pwr[d" << i << "], ";
  fout << "freq" << fourierData.size()-1 << ", Re[d" << fourierData.size()-1 << "], Im[d" << fourierData.size()-1 << "], Pwr[d" << fourierData.size()-1 << "]" << std::endl;

  // write data
  for (UInt_t j=0; j<data[0].size(); j++) {
    for (UInt_t i=0; i<data.size()-1; i++) {
      fout << data[i][j] << ", ";
    }
    fout << data[data.size()-1][j] << std::endl;
  }
  fout.close();

  return 0;
}

//-------------------------------------------------------------------------
/**
 * <p>Groups the histograms before Fourier transform. This is used to group
 * detectors.
 *
 * \param runDataHandler raw-run-data object containing the data
 * \param global pointer to the GLOBAL block of the msr-file
 * \param run reference to the relevant RUN block of the msr-file
 * \param rd data collection which will hold the grouped histograms.
 */
Int_t musrFT_groupHistos(PRunDataHandler *runDataHandler, PMsrGlobalBlock *global, PMsrRunBlock &run, musrFT_data &rd)
{
  // get proper raw run data set
  TString runName = *(run.GetRunName());
  PRawRunData *rawRunData = runDataHandler->GetRunData(runName);
  if (rawRunData == nullptr) {
    std::cerr << std::endl << ">> musrFT_groupHistos **ERROR** Couldn't get raw run data for run '" << runName << "'." << std::endl;
    return 1;
  }

  // keep histo list
  PIntVector histoList;
  for (UInt_t i=0; i<run.GetForwardHistoNoSize(); i++) {
    histoList.push_back(run.GetForwardHistoNo(i));
  }

  // check if t0's are found and that #t0 == #histos
  PDoubleVector t0;
  t0.resize(histoList.size());
  // init t0 vector
  for (UInt_t i=0; i<t0.size(); i++)
    t0[i] = -1.0;
  // 1st: check in the global block
  for (UInt_t i=0; i<global->GetT0BinSize(); i++) {
    if (i >= t0.size()) { // something is VERY strange
      std::cerr << std::endl << ">> musrFT_groupHistos **WARNING** found #t0's in GLOBAL block > #histos!";
      std::cerr << std::endl << ">>                    This should NEVER happen. Will ignore these entries.";
      std::cerr << std::endl << ">>                    Please check your msr-file!!" << std::endl;
    } else {
      t0[i] = global->GetT0Bin(i);
    }
  }
  // 2nd: check in the run block
  for (UInt_t i=0; i<run.GetT0BinSize(); i++) {
    if (i >= t0.size()) { // something is VERY strange
      std::cerr << std::endl << ">> musrFT_groupHistos **WARNING** found #t0's in RUN block > #histos!";
      std::cerr << std::endl << ">>                    This should NEVER happen. Will ignore these entries.";
      std::cerr << std::endl << ">>                    Please check your msr-file!!" << std::endl;
    } else {
      t0[i] = run.GetT0Bin(i);
    }
  }
  // if still some t0's are == -1, estimate t0
  UInt_t idx;
  Double_t max;
  for (UInt_t i=0; i<t0.size(); i++) {
    if (t0[i] == -1.0) {
      std::cout << std::endl << ">> musrFT_groupHistos **WARNING** try to estimate t0 from maximum in the data set";
      std::cout << std::endl << ">>   '" << runName << "', histo " << histoList[i] << ". NO warranty this is sensible!";
      idx = 0;
      max = rawRunData->GetDataBin(histoList[i])->at(0);
      for (UInt_t j=1; j<rawRunData->GetDataBin(histoList[i])->size(); j++) {
        if (rawRunData->GetDataBin(histoList[i])->at(j) > max) {
          max = rawRunData->GetDataBin(histoList[i])->at(j);
          idx = j;
        }
      }
      std::cout << std::endl << ">>    estimated t0=" << idx << std::endl;
      t0[i] = idx;
    }
  }

  // group histos
  PDoubleVector data = *(rawRunData->GetDataBin(histoList[0]));
  for (UInt_t i=1; i<histoList.size(); i++) {
    for (UInt_t j=0; j<data.size(); j++) {
      if ((j+t0[i]-t0[0] >= 0) && (j+t0[i]-t0[0] < rawRunData->GetDataBin(histoList[i])->size())) {
        data[j] += rawRunData->GetDataBin(histoList[i])->at(j);
      }
    }
  }

  rd.rawData.clear();
  rd.rawData = data;
  rd.t0 = static_cast<Int_t>(t0[0]);

  return 0;
}

//-------------------------------------------------------------------------
/**
 * <p>Dumps an msr-file according to the given command line settings. This
 * is meant to generate an initial msr-file for a given data-file. This
 * routine is 'stupid' in the sense that it knows nothing about the data-files.
 * Hence when feeding it with senseless command line settings, the resulting
 * msr-file fed back to musrFT might do funny things!
 *
 * \param param command line options
 */
void musrFT_dumpMsrFile(musrFT_startup_param &param)
{
  std::ofstream fout(param.msrFlnOut.Data(), std::ofstream::out);

  // write title
  if (param.title.Length() == 0) { // create title if not given
    if (param.dataFln.size() != 0) {
      param.title = param.dataFln[0];
    } else {
      param.title = param.msrFlnOut;
    }
  }
  fout << param.title << std::endl;
  fout << "###############################################################" << std::endl;

  // write GLOBAL block
  fout << "GLOBAL" << std::endl;
  fout << "fittype         0         (single histogram fit)" << std::endl;
  if (param.t0.size() == 1) { // only a single t0 value given, hence assume it is valid for ALL histos
    fout << "t0              " << param.t0[0] << std::endl;
  }
  if ((param.timeRange[0] != -1.0) && (param.timeRange[1] != -1.0)) {
    fout << "fit             " << param.timeRange[0] << "  " << param.timeRange[1] << std::endl;
  }
  fout << "packing         " << param.packing << std::endl;
  fout << std::endl;
  fout << "###############################################################" << std::endl;

  // write RUN block
  // get extension of the data file
  TString fileFormat("MUSR-ROOT");
  for (UInt_t i=0; i<param.dataFln.size(); i++) {
    if (param.dataFileFormat[i].BeginsWith("PsiBin"))
      fileFormat = TString("PSI-BIN");
    else if (param.dataFileFormat[i].BeginsWith("PsiMdu"))
      fileFormat = TString("PSI-MDU");
    else if (param.dataFileFormat[i].BeginsWith("NeXus"))
      fileFormat = TString("NEXUS");
    else if (param.dataFileFormat[i].BeginsWith("Mud"))
      fileFormat = TString("MUD");
    for (UInt_t j=0; j<param.histo.size(); j++) {
      fout << "RUN " << param.dataFln[i] << " BXXX IXX " << fileFormat << "   (name beamline institute data-file-format)" << std::endl;
      fout << "forward         " << param.histo[j] << std::endl;
      if ((param.t0.size() > 1) && (j < param.t0.size())) {
        fout << "t0              " << param.t0[j] << std::endl;
      }
      if ((param.bkg_range[0] > -1) && (param.bkg_range[1] > -1))
        fout << "background      " << param.bkg_range[0] << "     " << param.bkg_range[1] << std::endl;
      fout << "#--------------------------------------------------------------" << std::endl;
    }
  }
  fout << std::endl;
  fout << "###############################################################" << std::endl;

  // write PLOT block
  fout << "PLOT 0   (single histo plot)" << std::endl;
  if (param.histo.size() == 0) {
    fout << "runs     1" << std::endl;
  } else {
    fout << "runs     ";
    for (UInt_t i=0; i<param.histo.size(); i++)
      fout << i+1 << " ";
    fout << std::endl;
  }
  if ((param.timeRange[0] == -1.0) && (param.timeRange[1] == -1.0)) {
    fout << "range    0    10" << std::endl;
  } else {
    fout << "range    " << param.timeRange[0] << "   " << param.timeRange[1] << std::endl;
  }
  fout << std::endl;
  fout << "###############################################################" << std::endl;

  // write FOURIER block
  fout << "FOURIER" << std::endl;
  if (param.fourierUnits.BeginsWith("??")) { // Fourier units not given, hence choose MHz
    fout << "units            MHz   # units either 'Gauss', 'MHz', or 'Mc/s'" << std::endl;
  } else {
    fout << "units            " << param.fourierUnits << "   # units either 'Gauss', 'MHz', or 'Mc/s'" << std::endl;
  }
  if (param.fourierOpt.BeginsWith("??")) { // Fourier plot option not given, hence choose POWER
    fout << "plot             POWER   # REAL, IMAG, REAL_AND_IMAG, POWER, PHASE, PHASE_OPT_REAL" << std::endl;
  } else {
    fout << "plot             " << param.fourierOpt << "     # REAL, IMAG, REAL_AND_IMAG, POWER, PHASE, PHASE_OPT_REAL" << std::endl;
  }
  if (param.fourierPower > 1) {
    fout << "fourier_power    " << param.fourierPower << std::endl;
  }
  fout << "apodization      " << param.apodization << "        # NONE, WEAK, MEDIUM, STRONG" << std::endl;
  if ((param.fourierRange[0] > -1.0) && (param.fourierRange[1] > -1.0)) {
    fout << "range            " << param.fourierRange[0] << "   " << param.fourierRange[1] << std::endl;
  }

  fout.close();
}

//-------------------------------------------------------------------------
/**
 * <p>Gets time a time stamp in msec. Used to measure the calculation time.
 *
 * <b>return:</b> time stamp with msec resolution.
 */
Double_t millitime()
{
  struct timeval now;
  gettimeofday(&now, 0);

  return (static_cast<Double_t>(now.tv_sec) * 1.0e6 + static_cast<Double_t>(now.tv_usec))/1.0e3;
}

//-------------------------------------------------------------------------
/**
 * <p>musrFT is used to do a Fourier transform of uSR data without any fitting.
 * It directly Fourier transforms the raw histogram data (exception see --lifetimecorrection),
 * and hence only will give staisfactory results for applied fields of larger a
 * couple of kGauss. It is meant to be used to get a feeling what time-domain
 * model will be appropriate. It is NOT meant for ANY quantitative analysis!
 *
 * \param argc number of command line arguments
 * \param argv command line argument array
 */
Int_t main(Int_t argc, Char_t *argv[])
{
  Int_t unitTag = FOURIER_UNIT_NOT_GIVEN;
  Int_t apodTag = F_APODIZATION_NONE;
  Int_t fourierPlotTag = FOURIER_PLOT_NOT_GIVEN;

  // only program name alone
  if (argc == 1) {
    musrFT_syntax();
    return PMUSR_SUCCESS;
  }

  musrFT_startup_param startupParam;
  // init startupParam
  musrFT_init(startupParam);

  // parse command line options
  Int_t status = musrFT_parse_options(argc, argv, startupParam);
  if (status != 0) {
    Int_t retVal = PMUSR_SUCCESS;
    if (status == 2) {
      musrFT_syntax();
      retVal = PMUSR_WRONG_STARTUP_SYNTAX;
    }
    return retVal;
  }

  // dump msr-file
  if (startupParam.msrFlnOut.Length() > 0) {
    musrFT_dumpMsrFile(startupParam);
    return PMUSR_SUCCESS;
  }

  // read startup file
  Char_t startup_path_name[128];
  PStartupOptions startup_options;
  startup_options.writeExpectedChisq = false;
  startup_options.estimateN0 = true;
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    std::cerr << std::endl << ">> musrFT **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
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
      std::cerr << std::endl << ">> musrFT **WARNING** Reading/parsing musrfit_startup.xml failed.";
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

  // defines the raw time-domain data vector
  PPrepFourier data(startupParam.packing, startupParam.bkg_range, startupParam.bkg);

  // load msr-file(s)
  std::vector<PMsrHandler*> msrHandler;
  msrHandler.resize(startupParam.msrFln.size());
  for (UInt_t i=0; i<startupParam.msrFln.size(); i++) {
    msrHandler[i] = new PMsrHandler(startupParam.msrFln[i].Data(), &startup_options, true);
    status = msrHandler[i]->ReadMsrFile();
    if (status != PMUSR_SUCCESS) {
      switch (status) {
        case PMUSR_MSR_FILE_NOT_FOUND:
          std::cout << std::endl << ">> musrFT **ERROR** couldn't find " << startupParam.msrFln[i] << std::endl << std::endl;
          break;
        case PMUSR_MSR_SYNTAX_ERROR:
          std::cout << std::endl << ">> musrFT **SYNTAX ERROR** in file " << startupParam.msrFln[i] << ", full stop here." << std::endl << std::endl;
          break;
        default:
          std::cout << std::endl << ">> musrFT **UNKOWN ERROR** when trying to read the msr-file" << std::endl << std::endl;
          break;
      }
      return status;
    }
  }

  std::vector<PRunDataHandler*> runDataHandler;
  runDataHandler.resize(startupParam.msrFln.size()+startupParam.dataFln.size()); // resize to the total number of run data provided
  // load data-file(s) related to msr-file
  for (UInt_t i=0; i<msrHandler.size(); i++) {
    // create run data handler
    if (startupHandler)
      runDataHandler[i] = new PRunDataHandler(msrHandler[i], startupHandler->GetDataPathList());
    else
      runDataHandler[i] = new PRunDataHandler(msrHandler[i]);
  }

  // load data-file(s) provided directly
  for (UInt_t i=msrHandler.size(); i<msrHandler.size()+startupParam.dataFln.size(); i++) {
    // create run data handler
    if (startupHandler)
      runDataHandler[i] = new PRunDataHandler(startupParam.dataFln[i-msrHandler.size()], startupParam.dataFileFormat[i-msrHandler.size()], startupHandler->GetDataPathList());
    else
      runDataHandler[i] = new PRunDataHandler(startupParam.dataFln[i-msrHandler.size()], startupParam.dataFileFormat[i-msrHandler.size()]);
  }

  // read all the data files
  musrFT_data rd;
  rd.dataSetTag = -1;
  Int_t dataSetTagCounter = 0;
  TString prevDataSetPathName("");
  TString str(""), fln("");
  UInt_t idx=0;

  for (UInt_t i=0; i<runDataHandler.size(); i++) {
    runDataHandler[i]->ReadData();

    if (!runDataHandler[i]->IsAllDataAvailable()) {
      if (i < msrHandler.size()) {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't read data from msr-file '" << startupParam.msrFln[i] << "'." << std::endl;
      } else {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't read data-file '" << startupParam.dataFln[i] << "'." << std::endl;
      }
      return PMUSR_DATA_FILE_READ_ERROR;
    }

    // dig out all the necessary time domain data
    PRawRunData *rawRunData = runDataHandler[i]->GetRunData();
    if (rawRunData == nullptr) {
      if (i < msrHandler.size()) {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't obtain the raw run data set from msr-file " << startupParam.msrFln[i] << std::endl;
      } else {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't obtain the raw run data set for " << startupParam.dataFln[i-msrHandler.size()] << std::endl;
      }
      return PMUSR_DATA_FILE_READ_ERROR;
    }

    // first check of histo list makes sense
    if (i >= msrHandler.size()) { // only check if originating from data-files (not msr-files)
      for (UInt_t j=0; j<startupParam.histo.size(); j++) {
        if ((UInt_t)startupParam.histo[j] > rawRunData->GetNoOfHistos()) {
          std::cerr << std::endl << ">> musrFT **ERROR** found histo no " << startupParam.histo[j] << " > # of histo in the file (";
          std::cerr << startupParam.dataFln[i] << " // # histo: " << rawRunData->GetNoOfHistos() << ")." << std::endl;
          return PMUSR_DATA_FILE_READ_ERROR;
        }
      }
      if (startupParam.histo.size() == 0) { // no histo list given
        // set histo list to ALL available histos for the data file
        for (UInt_t j=0; j<rawRunData->GetNoOfHistos(); j++)
          startupParam.histo.push_back(j+1);
      }
    }

    // get meta info, time resolution, time range, raw data sets
    if (i < msrHandler.size()) { // obtain info from msr-files
      // keep title if not overwritten by the command line
      if (startupParam.title.Length() == 0)
        startupParam.title = *(msrHandler[0]->GetMsrTitle());
      // keep PLOT block info
      PMsrPlotList *plot = msrHandler[i]->GetMsrPlotList();
      if (plot == nullptr) {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't obtain PLOT block from msr-handler." << std::endl;
        return PMUSR_DATA_FILE_READ_ERROR;
      }
      // keep RUN block(s) info
      PMsrRunList *runs = msrHandler[i]->GetMsrRunList();
      if (runs == nullptr) {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't obtain RUN block(s) from msr-handler." << std::endl;
        return PMUSR_DATA_FILE_READ_ERROR;
      }
      // keep GLOBAL block info
      PMsrGlobalBlock *global = msrHandler[i]->GetMsrGlobal();
      if (global == nullptr) {
        std::cerr << std::endl << ">> musrFT **ERROR** couldn't obtain GLOBAL block from msr-handler." << std::endl;
        return PMUSR_DATA_FILE_READ_ERROR;
      }
      // keep FOURIER block info
      PMsrFourierStructure *fourierBlock = msrHandler[i]->GetMsrFourierList();
      if (fourierBlock == nullptr) {
        std::cerr << std::endl << ">> msrFT **WARNING** couldn't obtain FOURIER block from msr-handler." << std::endl;
        return PMUSR_DATA_FILE_READ_ERROR;
      } else { // filter out all necessary info
        if (fourierBlock->fFourierBlockPresent) {
          // get units
          unitTag = fourierBlock->fUnits;
          if (startupParam.fourierUnits.BeginsWith("??")) {
            switch (unitTag) {
              case FOURIER_UNIT_GAUSS:
                startupParam.fourierUnits = TString("Gauss");
                break;
              case FOURIER_UNIT_TESLA:
                startupParam.fourierUnits = TString("Tesla");
                break;
              case FOURIER_UNIT_FREQ:
                startupParam.fourierUnits = TString("MHz");
                break;
              case FOURIER_UNIT_CYCLES:
                startupParam.fourierUnits = TString("Mc/s");
                break;
              default:
                break;
            }
          }
          // get fourier power
          if (startupParam.fourierPower == -1) { // no Fourier power given from the command line, hence check FOURIER block
            if (fourierBlock->fFourierPower > 1)
              startupParam.fourierPower = fourierBlock->fFourierPower;
          }
          // get apodization tag
          switch (fourierBlock->fApodization) {
          case FOURIER_APOD_WEAK:
            startupParam.apodization = "weak";
            break;
          case FOURIER_APOD_MEDIUM:
            startupParam.apodization = "medium";
            break;
          case FOURIER_APOD_STRONG:
            startupParam.apodization = "strong";
            break;
          default:
            startupParam.apodization = "none";
            break;
          }
          // get range
          if ((startupParam.fourierRange[0] == -1) && (startupParam.fourierRange[1] == -1)) { // no Fourier range given from the command line
            startupParam.fourierRange[0] = fourierBlock->fPlotRange[0];
            startupParam.fourierRange[1] = fourierBlock->fPlotRange[1];            
          }
          // get Fourier plot option, i.e. real, imag, power, phase
          if (startupParam.fourierOpt.BeginsWith("??")) { // only do something if not overwritten by the command line
            fourierPlotTag = fourierBlock->fPlotTag;
          }
        }
      }

      // get the run information from the msr-file PLOT block 'runs'
      PIntVector runList = plot->at(0).fRuns;

      // loop over all runs listed in the msr-file PLOT block
      for (UInt_t j=0; j<runList.size(); j++) {

        // check if the data set name has changed
        str = *(runs->at(runList[j]-1).GetRunName()); // get the name from the msr-file RUN block
        if (prevDataSetPathName.CompareTo(str)) { // i.e. data set name changed
          rd.dataSetTag = dataSetTagCounter++;
          prevDataSetPathName = str;
        }

        // keep forward histo list
        PIntVector histoList;
        for (UInt_t k=0; k<runs->at(runList[j]-1).GetForwardHistoNoSize(); k++) {
          histoList.push_back(runs->at(runList[j]-1).GetForwardHistoNo(k));
        }

        // handle meta information
        fln  = *(runs->at(runList[j]-1).GetRunName()); // get the name from the msr-file RUN block
        musrFT_getMetaInfo(fln, rawRunData, str);
        TString hh("");
        hh = TString::Format("h%d", histoList[0]);
        for (UInt_t k=1; k<histoList.size(); k++)
          hh += TString::Format("/%d", histoList[k]);
        hh += ":";
        rd.info  = hh;
        rd.info += str;

        // handle time resolution
        rd.timeResolution = rawRunData->GetTimeResolution() / 1.0e3; // time resolution in (us)

        // handle time range
        // take it from msr-file PLOT block 'range' if not overwritten from the command line
        if ((startupParam.timeRange[0] != -1) && (startupParam.timeRange[1] != -1)) {
          rd.timeRange[0] = startupParam.timeRange[0];
          rd.timeRange[1] = startupParam.timeRange[1];
        } else {
          if (plot->at(0).fTmin.size() > 0) {
            rd.timeRange[0] = plot->at(0).fTmin[0];
            rd.timeRange[1] = plot->at(0).fTmax[0];
          }
        }

        // handle data set(s)
        // group forward histos
        if (musrFT_groupHistos(runDataHandler[i], global, runs->at(runList[j]-1), rd)) {
          return PMUSR_DATA_FILE_READ_ERROR;
        }
        // keep data set
        data.AddData(rd);

        // get packing
        Int_t pack = 1;
        if (global->GetPacking() != -1) {
          pack = global->GetPacking();
        }
        if (runs->at(runList[j]-1).GetPacking() != -1) {
          pack = runs->at(runList[j]-1).GetPacking();
        }
        if (startupParam.packing > 1)
          pack = startupParam.packing;
        data.SetPacking(pack);

        // get background range
        Int_t bkgStart=-1, bkgEnd=-1;
        bkgStart = runs->at(runList[j]-1).GetBkgRange(0);
        bkgEnd = runs->at(runList[j]-1).GetBkgRange(1);
        if ((startupParam.bkg_range[0] == -1) && (bkgStart != -1))
          startupParam.bkg_range[0] = bkgStart;
        if ((startupParam.bkg_range[1] == -1) && (bkgEnd != -1))
          startupParam.bkg_range[1] = bkgEnd;
        data.SetBkgRange(startupParam.bkg_range);
      }
    } else { // obtain info from command line options for direct data-file read
      // check if the data set name has changed
      // since data-files are given, each PRunDataHandler object contains only a SINGLE data file.
      str = *(runDataHandler[i]->GetRunData()->GetFileName()); // get the data set name
      if (prevDataSetPathName.CompareTo(str)) { // i.e. data set name changed
        rd.dataSetTag = dataSetTagCounter++;
        prevDataSetPathName = str;
      }

      musrFT_getMetaInfo(startupParam.dataFln[i-msrHandler.size()], rawRunData, str);
      for (UInt_t j=0; j<startupParam.histo.size(); j++) {
        idx = startupParam.histo[j];

        // handle meta information
        rd.info  = TString::Format("h%d:", idx);
        rd.info += str;

        // handle time resolution
        rd.timeResolution = rawRunData->GetTimeResolution() / 1.0e3; // time resolution in (us)

        // handle time range
        rd.timeRange[0] = startupParam.timeRange[0]; // in (us)
        rd.timeRange[1] = startupParam.timeRange[1]; // in (us)

        // handle data set
        rd.rawData.clear();
        rd.rawData = *(rawRunData->GetDataBin(idx));

        // handle t0's
        rd.t0 = -1;
        if (startupParam.t0.size() == 1)
          rd.t0 = startupParam.t0[0];
        else if (j < startupParam.t0.size())
          rd.t0 = startupParam.t0[j];
        if (rd.t0 == -1) { // no t0 given, try to estimate it
          musrFT_estimateT0(rd);
        }

        data.AddData(rd);
      }
    }
  }

  // generate data set label vector
  PIntVector dataSetTag;
  for (UInt_t i=0; i<data.GetNoOfData(); i++) {
    dataSetTag.push_back(data.GetDataSetTag(i));
  }

  // make sure Fourier plot tag is set
  if (fourierPlotTag == FOURIER_PLOT_NOT_GIVEN) {
    if (!startupParam.fourierOpt.CompareTo("real", TString::kIgnoreCase))
      fourierPlotTag = FOURIER_PLOT_REAL;
    else if (!startupParam.fourierOpt.CompareTo("imag", TString::kIgnoreCase))
      fourierPlotTag = FOURIER_PLOT_IMAG;
    else if (!startupParam.fourierOpt.CompareTo("real+imag", TString::kIgnoreCase))
      fourierPlotTag = FOURIER_PLOT_REAL_AND_IMAG;
    else if (!startupParam.fourierOpt.CompareTo("power", TString::kIgnoreCase))
      fourierPlotTag = FOURIER_PLOT_POWER;
    else if (!startupParam.fourierOpt.CompareTo("phase", TString::kIgnoreCase))
      fourierPlotTag = FOURIER_PLOT_PHASE;
    else if (!startupParam.fourierOpt.CompareTo("phaseoptreal", TString::kIgnoreCase))
      fourierPlotTag = FOURIER_PLOT_PHASE_OPT_REAL;
    else
      fourierPlotTag = FOURIER_PLOT_POWER;
  }

  // calculate background levels and subtract them from the data
  data.DoBkgCorrection();

  // do lifetime correction
  if (startupParam.lifetimecorrection != 0.0)
    data.DoLifeTimeCorrection(startupParam.lifetimecorrection);

  // do packing
  data.DoPacking();

  // get all the corrected data
  std::vector<TH1F*> histo = data.GetData();

  // prepare Fourier
  if (startupParam.fourierUnits.BeginsWith("gauss", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_GAUSS;
  else if (startupParam.fourierUnits.BeginsWith("tesla", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_TESLA;
  else if (startupParam.fourierUnits.BeginsWith("mhz", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_FREQ;
  else if (startupParam.fourierUnits.BeginsWith("mc/s", TString::kIgnoreCase))
    unitTag = FOURIER_UNIT_CYCLES;
  else if (startupParam.fourierUnits.BeginsWith("??", TString::kIgnoreCase) && (unitTag == FOURIER_UNIT_NOT_GIVEN))
    unitTag = FOURIER_UNIT_FREQ;

  std::vector<PFourier*> fourier;
  fourier.resize(histo.size());
  for (UInt_t i=0; i<fourier.size(); i++) {
    fourier[i] = new PFourier(histo[i], unitTag, 0.0, 0.0, true, startupParam.fourierPower);
  }

  // Fourier transform data
  if (startupParam.apodization.BeginsWith("weak", TString::kIgnoreCase))
    apodTag = F_APODIZATION_WEAK;
  else if (startupParam.apodization.BeginsWith("medium", TString::kIgnoreCase))
    apodTag = F_APODIZATION_MEDIUM;
  else if (startupParam.apodization.BeginsWith("strong", TString::kIgnoreCase))
    apodTag = F_APODIZATION_STRONG;

  Double_t start = millitime();
  for (UInt_t i=0; i<fourier.size(); i++) {
    fourier[i]->Transform(apodTag);
  }
  Double_t end = millitime();
  std::cout << std::endl << "info> after FFT. calculation time: " << (end-start)/1.0e3  << " (sec)." << std::endl;

  // make sure that a Fourier range is provided, if not calculate one
  if ((startupParam.fourierRange[0] == -1.0) && (startupParam.fourierRange[1] == -1.0)) {
    startupParam.fourierRange[0] = 0.0;
    startupParam.fourierRange[1] = fourier[0]->GetMaxFreq();
  }

  PFourierCanvas *fourierCanvas = nullptr;

  // if Fourier dumped if whished do it now
  if (startupParam.dumpFln.Length() > 0) {
    musrFT_dumpData(startupParam.dumpFln, fourier, startupParam.fourierRange[0], startupParam.fourierRange[1]);
  } else { // do Canvas

    // if Fourier graphical export is whished, switch to batch mode
    Bool_t batch = false;
    if (startupParam.graphicFormat.Length() != 0) {
      batch = true;
      argv[argc] = (Char_t*)malloc(16*sizeof(Char_t));
      strcpy(argv[argc], "-b");
      argc++;
    }

    // plot the Fourier transform
    TApplication app("App", &argc, argv);

    if (startupHandler) {
      fourierCanvas = new PFourierCanvas(fourier, dataSetTag, startupParam.title.Data(),
                                   startupParam.showAverage, startupParam.showAveragePerDataSet,
                                   fourierPlotTag, startupParam.fourierRange, startupParam.initialPhase,
                                   10, 10, 800, 800,
                                   startupHandler->GetMarkerList(),
                                   startupHandler->GetColorList(),
                                   batch);
    } else {
      fourierCanvas = new PFourierCanvas(fourier, dataSetTag, startupParam.title.Data(),
                                   startupParam.showAverage, startupParam.showAveragePerDataSet,
                                   fourierPlotTag, startupParam.fourierRange, startupParam.initialPhase,
                                   10, 10, 800, 800,
                                   batch);
    }

    fourierCanvas->UpdateFourierPad();
    fourierCanvas->UpdateInfoPad();

    Bool_t ok = true;
    if (!fourierCanvas->IsValid()) {
      std::cerr << std::endl << ">> musrFT **SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
      std::cerr << std::endl;
      ok = false;
    } else {
      // connect signal/slot
      TQObject::Connect("TCanvas", "Closed()", "PFourierCanvas", fourierCanvas, "LastCanvasClosed()");

      fourierCanvas->SetTimeout(startupParam.timeout);

      fourierCanvas->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

      if (startupParam.graphicFormat.Length() != 0) {
        TString fileName("");
        // create output filename based on the msr- or raw-data-filename
        if (startupParam.dataFln.size() > 0) {
          fileName = startupParam.dataFln[0];
        }
        if (startupParam.msrFln.size() > 0) {
          fileName = startupParam.msrFln[0];
        }
        Ssiz_t idx = fileName.Last('.');
        fileName.Remove(idx, fileName.Length());
        fileName += ".";
        fileName += startupParam.graphicFormat;
        fourierCanvas->SaveGraphicsAndQuit(fileName.Data());
      }
    }
    // check that everything is ok
    if (ok)
      app.Run(true); // true needed that Run will return after quit so that cleanup works
  }


  // cleanup
  if (fourierCanvas)
    delete fourierCanvas;

  if (startupHandler)
    delete startupHandler;

  for (UInt_t i=0; i<msrHandler.size(); i++)
    if (msrHandler[i])
      delete msrHandler[i];
  msrHandler.clear();

  for (UInt_t i=0; i<runDataHandler.size(); i++)
    if (runDataHandler[i])
      delete runDataHandler[i];
  runDataHandler.clear();

  if (histo.size() > 0) {
    for (UInt_t i=0; i<histo.size(); i++)
      delete histo[i];
    histo.clear();
  }
  if (fourier.size() > 0) {
    for (UInt_t i=0; i<fourier.size(); i++)
      delete fourier[i];
    fourier.clear();
  }

  return PMUSR_SUCCESS;
}
