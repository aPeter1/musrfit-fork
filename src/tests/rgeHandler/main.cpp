/***************************************************************************

  main.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2022 by Andreas Suter                              *
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

#include <cstring>
#include <iostream>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "PRgeHandler.h"

void syntax()
{
  std::cout << std::endl;
  std::cout << "usage : rgeHandlerTest [options]" << std::endl;
  std::cout << "  -x, --xml <fln>: path-name of the xml-startup file." << std::endl;
  std::cout << "  -d, --dump     : dump rge-infos." << std::endl;
  std::cout << "  -s, --set <no> : dump rge-vector number <no>." << std::endl;
  std::cout << "  -v, --version  : rgeHandlerTest version" << std::endl;
  std::cout << "  -h, --help     : this help." << std::endl;
  std::cout << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc == 1) {
    syntax();
    return 0;
  }

  std::string fln("");
  bool dump(false);
  int set_no = -1;
  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "-x") || !strcmp(argv[i], "--xml")) {
      if (i+1 >= argc) {
        std::cout << std::endl;
        std::cout << "**ERROR** found -x/ --xml without file name." << std::endl;
        std::cout << std::endl;
        syntax();
        return 1;
      }
      fln = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dump")) {
      dump = true;
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--set")) {
      if (i+1 > argc) {
        std::cout << std::endl;
        std::cout << "**ERROR** found -s/--set without rge set number." << std::endl;
        std::cout << std::endl;
        syntax();
        return 1;
      }
      try {
        set_no = std::stoi(argv[i+1]);
      } catch(std::invalid_argument& e) {
        std::cout << std::endl;
        std::cout << "**ERROR** set number '" << argv[i+1] << "' is not a number." << std::endl;
        std::cout << std::endl;
        return 3;
      } catch(std::out_of_range& e) {
        std::cout << std::endl;
        std::cout << "**ERROR** set number '" << argv[i+1] << "' is out-of-range." << std::endl;
        std::cout << std::endl;
        return 3;
      } catch(...) {
        std::cout << std::endl;
        std::cout << "**ERROR** set number '" << argv[i+1] << "' leads to an unexpected error." << std::endl;
        std::cout << std::endl;
        return 3;
      }
      i++;
    } else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
      std::cout << std::endl;
#ifdef HAVE_CONFIG_H
      std::cout << "regHandlerTest Version: " << PACKAGE_VERSION << std::endl;
#else
      std::cout << "regHandlerTest Version: unkown." << std::endl;
#endif
      std::cout << std::endl;
      return 0;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      syntax();
      return 0;
    }
  }

  // read XML startup file
  PRgeHandler *rgeHandler = new PRgeHandler(fln);

  // check if everything went fine with the startup handler
  if (!rgeHandler->IsValid()) {
    std::cout << std::endl;
    std::cout << std::endl << ">> **ERROR** startup handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    delete rgeHandler;
    return 2;
  }

  if (dump) {
    std::cout << std::endl;
    std::cout << "RGE info from xml-startup-file " << fln << std::endl;
    PRgeDataList list = rgeHandler->GetRgeData();
    std::cout << "number of rge data sets: " << list.size() << std::endl;
    for (int i=0; i<list.size(); i++) {
      std::cout << "rge set #" << i+1 << ": energy: " << list[i].energy << " (eV), no of particles: " << list[i].noOfParticles << std::endl;
    }
    std::cout << std::endl;
  }
  if (set_no != -1) {
    PRgeDataList list = rgeHandler->GetRgeData();
    if (set_no > list.size()) {
      std::cout << std::endl;
      std::cout << "**ERROR** requested set number " << set_no << " > number of rge-data sets (" << list.size() << ")." << std::endl;
      std::cout << std::endl;
      delete rgeHandler;
      return 4;
    } else if (set_no == 0) {
      std::cout << std::endl;
      std::cout << "**ERROR** rge set-number count start at 1 not 0." << std::endl;
      std::cout << std::endl;
      delete rgeHandler;
      return 4;
    }
    std::cout << "rge-data set " << set_no << ": energy: " << list[set_no-1].energy << " (eV), no-of-particles: " << list[set_no-1].noOfParticles << std::endl;
    std::cout << "depth (nm), ampl, amplNorm" << std::endl;
    for (int i=0; i<list[set_no-1].depth.size(); i++) {
      std::cout << list[set_no-1].depth[i] << ", " << list[set_no-1].amplitude[i] << ", " << list[set_no-1].nn[i] << std::endl;
    }
  }

  if (rgeHandler) {
    delete rgeHandler;
  }

  return 0;
}
