/***************************************************************************

  vortex-film.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2020 by Andreas Suter                                   *
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

#include <cstdlib>

#include <memory>
#include <iostream>

#include "TFilmTriVortexFieldCalc.h"

void vortex_film_syntax()
{
  std::cout << std::endl;
  std::cout << "usage: vortex_film field penetration_depth coherence_length layer_thickness [stepXY stepZ]" << std::endl;
  std::cout << std::endl;
}

int main(int argc, char *argv[])
{
  if ((argc != 5) && (argc != 7)) {
    vortex_film_syntax();
    return -1;
  }

  std::vector<float> param;
  for (unsigned int i=1; i<5; i++)
    param.push_back((float)atof(argv[i]));

  unsigned int stepXY = 256;
  unsigned int stepZ  = 32;

  if (argc == 7) {
    stepXY = (unsigned int)strtol(argv[5], nullptr, 10);
    stepZ  = (unsigned int)strtol(argv[6], nullptr, 10);
  }

  std::string wisdom("");
  // c++11
  std::unique_ptr<TFilmTriVortexNGLFieldCalc> vl(new TFilmTriVortexNGLFieldCalc(wisdom));
/* will only work for c++14
  std::unique_ptr<TFilmTriVortexNGLFieldCalc> vl = std::make_unique<TFilmTriVortexNGLFieldCalc>(wisdom);
*/
  // set parameter
  vl->SetParameters(param);

  // calculate the vortex lattice
  vl->CalculateGrid();

  std::cout << ">> Bmin: " << vl->GetBmin() << std::endl;
  std::cout << ">> Bmax: " << vl->GetBmax() << std::endl;

  std::vector<float*> field = vl->DataB();

  for (unsigned int i=0; i<stepXY; i++) {
     std::cout << i << ", " << field[2][stepZ*i] << ", " << field[2][stepXY*stepXY/2+stepZ*i] << std::endl;
  }

  return 0;
}
