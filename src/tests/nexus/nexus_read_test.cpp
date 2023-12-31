/***************************************************************************

  nexus_read_test.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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
  cout << endl << ">> usage: nexus_read_test <nexus-in-filename> [<nexus-out-filename> <nexus-write-format> <idf>]";
  cout << endl << ">>        This will try to read a nexus-files <nexus-in-filename> and send the relevant";
  cout << endl << ">>        information to the standard output.";
  cout << endl << ">>        At the same time the read file is written back to <nexus-out-filename>, where";
  cout << endl << ">>        the extension will be added based on the <nexus-write-format>.";
  cout << endl << ">>        <nexus-write-format>: hdf4, hdf5, xml";
  cout << endl << ">>        <idf>: 1 | 2";
  cout << endl << ">>---------------------------------------------------------------------------------------";
  cout << endl << endl;
}

//---------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{  
  if ((argc != 5) && (argc !=2)) {
    nexus_read_test_syntax();
    return -1;
  }

  PNeXus *nxs_file = new PNeXus(argv[1]);

  if (nxs_file->IsValid(false)) {
    nxs_file->Dump();

    if (argc == 5) {
      long int idf = strtol(argv[4], (char **)NULL, 10);
      if ((idf != 1) && (idf != 2)) {
        if (nxs_file)
          delete nxs_file;
        nexus_read_test_syntax();
        return -1;
      }

      char filename[128];
      if (strstr(argv[3], "hdf") || strstr(argv[3], "xml")) {
        snprintf(filename, sizeof(filename), "%s.%s", argv[2], argv[3]);
      } else {
        cerr << endl << "**ERROR** unkown nexus write format found" << endl;
        nexus_read_test_syntax();
        return -1;
      }

      if (nxs_file->WriteFile(filename, argv[3], (unsigned int)idf) != NX_OK) {
        cerr << endl << nxs_file->GetErrorMsg() << " (" << nxs_file->GetErrorCode() << ")" << endl << endl;
      } else {
        cout << endl << "file " << filename << " written successfully." << endl << endl;
      }
    }
  }

  if (nxs_file)
    delete nxs_file;

  return 0;
}
