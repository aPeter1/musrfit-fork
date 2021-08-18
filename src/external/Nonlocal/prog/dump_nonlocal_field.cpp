/***************************************************************************

  dump_nonlocal_field.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Andreas Suter                              *
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>

#include <boost/algorithm/string.hpp>

#include "PNL_PippardFitter.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_DNLF_CONFIG_H
#include "dnlf_config.h"
#endif
#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif

void dnlf_syntax()
{
  std::cout << std::endl;
  std::cout << "usage: dump_nonlocal_field [<msr-file> --out <field-dump> [--step <stepVal>] | --version | --help]" << std::endl;
  std::cout << "   <msr-file> : nonlocal msr-file name." << std::endl;
  std::cout << "   --out <field-dump> : ascii field dump output file name." << std::endl;
  std::cout << "   --step <stepVal>   : this optional parameters allows to define the z-value step size in (nm)." << std::endl;
  std::cout << "   --version          : dumps the version" << std::endl;
  std::cout << "   --help             : will dump this help" << std::endl;
  std::cout << std::endl << std::endl;
}

bool dnlf_read_msr(const std::string fln, std::vector<double> &param)
{
  std::ifstream fin(fln.c_str(), std::ifstream::in);
  if (!fin.is_open()) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** couldn't open msr-file: " << fln << std::endl;
    std::cerr << std::endl;
    return false;
  }

  std::vector<std::string> msrData;
  std::string line;
  while (fin.good()) {
    std::getline(fin, line);
    msrData.push_back(line);
  }

  fin.close();

  // find userFcn line
  line = "";
  for (int i=0; i<msrData.size(); i++) {
    if (msrData[i].find("userFcn") != std::string::npos) {
      line = msrData[i];
      if (line.find("PNL_PippardFitter") != std::string::npos)
        break;
      line = "";
    }
  }

  if (line.empty()) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** msr-file: " << fln << " seems not a proper nonlocal fit file." << std::endl;
    std::cerr << std::endl;
    return false;
  }

  // tokenize parameter part.
  size_t pos=0;
  pos = line.find(" PNL_PippardFitter");
  line = line.substr(pos+18); // remove the 'userFcn libPNL_PippardFitter PNL_PippardFitter' part
  boost::algorithm::trim(line); // remove leading/trailing whitespaces
  std::vector<std::string> tok;
  boost::split(tok, line, boost::is_any_of(" \t"), boost::token_compress_on);
  std::vector<int> paramIdx;
  int ival;
  for (int i=0; i<tok.size(); i++) {
    try {
      ival = std::stoi(tok[i]);
    }
    catch(std::exception& e) {
      ival = 0;
    }
    paramIdx.push_back(ival);
  }

  // find parameter value which corresponds to param index
  bool inFitParamBlock(false);
  std::map<int, double> paramMap;
  double dval;
  for (int i=0; i<msrData.size(); i++) {
    if (inFitParamBlock) {
      if (msrData[i].empty()) {
        continue;
      } else if (msrData[i][0] == '#') {
        continue;
      } else if (msrData[i].find("THEORY") != std::string::npos) {
        inFitParamBlock = false;
        break;
      } else {
        tok.clear();
        line = msrData[i];
        boost::algorithm::trim(line); // remove leading/trailing whitespaces
        boost::split(tok, line, boost::is_any_of(" \t"), boost::token_compress_on);
        if (tok.size() < 3) {
          std::cerr << "**ERROR** bug in the fit parameter block!" << std::endl;
          std::cerr << "'" << msrData[i] << "'" << std::endl;
          return false;
        }
        // param index
        try {
          ival = std::stoi(tok[0]);
        }
        catch(std::exception& e) {
          ival = 0;
        }
        // param value
        try {
          dval = std::stod(tok[2]);
        }
        catch(std::exception& e) {
          dval = 0.0;
        }
        paramMap[ival] = dval;
      }
    } else {
      if (msrData[i].find("FITPARAMETER") != std::string::npos) {
        inFitParamBlock = true;
      }
    }
  }

  for (int i=0; i<paramIdx.size(); i++) {
    if (paramIdx[i] != 0)
      dval = paramMap.find(paramIdx[i])->second;
    else
      dval = 0.0;
    param.push_back(dval);
  }

  return true;
}

int main(int argc, char* argv[])
{
  std::string msrFileName("");
  std::string outFileName("");
  double step(0.0);
  bool show_syntax(false);

  if (argc == 1) {
    dnlf_syntax();
    return 0;
  }

  for (int i=0; i<argc; i++) {
    if (argv[i][0] != '-') { // must be the msr-file name
      msrFileName = argv[i];
    } else if (!strcmp(argv[i], "--version")) {
#ifdef HAVE_CONFIG_H
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "dump_nonlocal_field version: " << PROJECT_VERSION << " (" << PACKAGE_VERSION << "), git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "dump_nonlocal_field version: " << PROJECT_VERSION << " (" << PACKAGE_VERSION << "/" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#endif
#else
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "dump_nonlocal_field git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#else
      std::cout << std::endl << "dump_nonlocal_field version: unknown" << std::endl << std::endl;
#endif
#endif
      return 0;
    } else if (!strcmp(argv[i], "--help")) {
      dnlf_syntax();
      return 0;
    } else if (!strcmp(argv[i], "--out")) {
      if (i < argc-1) {
        outFileName = argv[i+1];
        i++;
      } else {
        std::cerr << std::endl << "dump_nonlocal_field: **ERROR** found option --out without <field-dump>" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "--step")) {
      if (i < argc-1) {
        try {
          step = std::stod(argv[i+1]);
        }
        catch(std::exception& e) {
          std::cout << "dump_nonlocal_field: **ERROR** <stepVal> '" << argv[i+1] << "' seems not to be a double." << std::endl;
          show_syntax = true;
          break;
        }
        i++;
      } else {
        std::cerr << std::endl << "dump_nonlocal_field: **ERROR** found option --step without <stepVal>" << std::endl;
        show_syntax = true;
        break;
      }
    }
  }

  if (show_syntax) {
    dnlf_syntax();
    return -1;
  }

  std::vector<double> param;
  if (!dnlf_read_msr(msrFileName, param)) {
    return -2;
  }


  // calculate field and write it to file
  PNL_PippardFitterGlobal pip;
  pip.CalculateField(param);

  std::ofstream fout(outFileName.c_str(), std::ofstream::out);
  if (!fout.is_open()) {
    std::cout << std::endl;
    std::cerr << "**ERROR** can not write to file '" << outFileName << "'." << std::endl;
    std::cout << std::endl;
    return -3;
  }
  // write header
  fout << "% z (nm), field (G)" << std::endl;

  // write data
  double z=0.0;
  if (step == 0.0)
    step = 0.1;
  double prevField=0.0, field=0.0;
  bool done(false);
  double deadLayer = param[8];
  double b0 = param[6];
  do {
    if (z < deadLayer) {
      fout << z << ", " << b0 << std::endl;
    } else {
      field = pip.GetMagneticField(z-deadLayer);
      fout << z << ", " << b0*field << std::endl;
      if (fabs(prevField-field) < 1.0e-10)
        done = true;
      prevField = field;
    }
    z += step;
  } while (!done);

  fout.close();

  return 0;
}
