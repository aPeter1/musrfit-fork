/***************************************************************************

  nexus_read_test.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id: nexus_read_test.cpp 4981 2011-08-23 17:22:29Z nemu $

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
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
#include <string>
#include <vector>
using namespace std;

#include "PNeXus.h"

//---------------------------------------------------------------------------------------

void nexus_read_test_syntax()
{
  cout << endl << ">>---------------------------------------------------------------------------------------";
  cout << endl << ">> usage: nexus_dump <nexus-in-filename>";
  cout << endl << ">>        This will try to read a nexus-files <nexus-in-filename> and send the relevant";
  cout << endl << ">>        information to the standard output.";
  cout << endl << ">>---------------------------------------------------------------------------------------";
  cout << endl << endl;
}

//---------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{  
  if (argc !=2) {
    nexus_read_test_syntax();
    return -1;
  }

  PNeXus *nxs_file = new PNeXus(argv[1]);

  if (nxs_file->IsValid(false)) {
    nxs_file->Dump();
  }

  if (nxs_file)
    delete nxs_file;

  return 0;
}
