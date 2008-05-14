/***************************************************************************

  msr2msr.cpp

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

#include <iostream>
#include <fstream>
using namespace std;

#include <cctype>
#include <cstring>
#include <cstdlib>

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

//-------------------------------------------------------------
// msr block header tags
#define MSR_TAG_TITLE        0
#define MSR_TAG_FITPARAMETER 1
#define MSR_TAG_THEORY       2
#define MSR_TAG_FUNCTIONS    3
#define MSR_TAG_RUN          4
#define MSR_TAG_COMMANDS     5
#define MSR_TAG_PLOT         6
#define MSR_TAG_STATISTIC    7
#define MSR_TAG_NO_BLOCK     8

//--------------------------------------------------------------------------
void msr2msr_syntax()
{
  cout << endl << "usage: msr2msr <msr-file-in> <msr-file-out> | [--help]";
  cout << endl << "  <msr-file-in> : input msr-file";
  cout << endl << "  <msr-file-out>: converted msr-output-file";
  cout << endl << "  if the <msr-file-in> is already in the 2008 format";
  cout << endl << "  the output file will be identical to the input file.";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
bool msr2msr_run(char *str)
{
  TString run(str);
  TString line(str);
  TObjArray *tokens;
  TObjString *ostr[2];

  // for filtering
  run.ToUpper();

  // remove run comment, i.e. (name ...
  Ssiz_t idx = line.Index("(");
  if (idx > 0)
    line.Remove(idx);

  // tokenize run
  tokens = line.Tokenize(" \t");
  if (tokens->GetEntries() < 4) {
    cout << endl << "**ERROR**: Something is wrong with the RUN block header:";
    cout << endl << "  >> " << str;
    cout << endl << "  >> no <msr-file-out> is created";
    cout << endl;
    return false;
  }

  if (tokens->GetEntries() == 5) { // alread new msr file, do only add the proper run comment
    sprintf(str, "%s (name beamline institute data-file-format)", line.Data());
    return true;
  }

  if (run.Contains("NEMU")) {
    ostr[0] = dynamic_cast<TObjString*>(tokens->At(1)); // file name
    sprintf(str, "RUN %s MUE4 PSI NEMU   (name beamline institute data-file-format)", ostr[0]->GetString().Data());
  } else if (run.Contains("PSI")) {
    ostr[0] = dynamic_cast<TObjString*>(tokens->At(1)); // file name
    ostr[1] = dynamic_cast<TObjString*>(tokens->At(2)); // beamline
    sprintf(str, "RUN %s %s PSI PSI-BIN   (name beamline institute data-file-format)", 
            ostr[0]->GetString().Data(), ostr[1]->GetString().Data());
  } else if (run.Contains("TRIUMF")) {
    ostr[0] = dynamic_cast<TObjString*>(tokens->At(1)); // file name
    ostr[1] = dynamic_cast<TObjString*>(tokens->At(2)); // beamline
    sprintf(str, "RUN %s %s TRIUMF MUD   (name beamline institute data-file-format)", 
            ostr[0]->GetString().Data(), ostr[1]->GetString().Data());
  } else if (run.Contains("RAL")) {
    ostr[0] = dynamic_cast<TObjString*>(tokens->At(1)); // file name
    ostr[1] = dynamic_cast<TObjString*>(tokens->At(2)); // beamline
    sprintf(str, "RUN %s %s RAL NEXUS   (name beamline institute data-file-format)", 
            ostr[0]->GetString().Data(), ostr[1]->GetString().Data());
  }
  cout << endl;

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  return true;
}

//--------------------------------------------------------------------------
void msr2msr_param(char *str, int &tag)
{
  // check for comment header which needs to be replaced
  if (strstr(str, "Nr.")) {
    strcpy(str, "#       No Name        Value     Step        Pos_Error   Boundaries");
    return;
  }

  // handle parameter line
  TString line(str);
  TObjArray *tokens;
  TObjString *ostr[6];
  char sstr[256];
  char spaces[256];

  tokens = line.Tokenize(" \t");
  unsigned int noTokens = tokens->GetEntries();
  if (noTokens == 4) {
    for (unsigned int i=0; i<4; i++)
      ostr[i] = dynamic_cast<TObjString*>(tokens->At(i));
    // number
    sprintf(sstr, "%10s", ostr[0]->GetString().Data());
    // name
    strcat(sstr, " ");
    strcat(sstr, ostr[1]->GetString().Data());
    memset(spaces, 0, sizeof(spaces));
    memset(spaces, ' ', 12-strlen(ostr[1]->GetString().Data()));
    strcat(sstr, spaces);
    // value
    strcat(sstr, ostr[2]->GetString().Data());
    if (strlen(ostr[2]->GetString().Data()) < 10) {  
      memset(spaces, 0, sizeof(spaces));
      memset(spaces, ' ', 10-strlen(ostr[2]->GetString().Data()));
      strcat(sstr, spaces);
    } else {
      strcat(sstr, " ");
    }
    // step
    strcat(sstr, ostr[3]->GetString().Data());
    if (strlen(ostr[3]->GetString().Data()) < 12) {
      memset(spaces, 0, sizeof(spaces));
      memset(spaces, ' ', 12-strlen(ostr[3]->GetString().Data()));
      strcat(sstr, spaces);
    } else {
      strcat(sstr, " ");
    }
    strcat(sstr, "none");
    strcpy(str, sstr);
  } else if (noTokens == 6) {
    for (unsigned int i=0; i<6; i++)
      ostr[i] = dynamic_cast<TObjString*>(tokens->At(i));
    // number
    sprintf(sstr, "%10s", ostr[0]->GetString().Data());
    // name
    strcat(sstr, " ");
    strcat(sstr, ostr[1]->GetString().Data());
    memset(spaces, 0, sizeof(spaces));
    memset(spaces, ' ', 12-strlen(ostr[1]->GetString().Data()));
    strcat(sstr, spaces);
    // value
    strcat(sstr, ostr[2]->GetString().Data());
    if (strlen(ostr[2]->GetString().Data()) < 10) {  
      memset(spaces, 0, sizeof(spaces));
      memset(spaces, ' ', 10-strlen(ostr[2]->GetString().Data()));
      strcat(sstr, spaces);
    } else {
      strcat(sstr, " ");
    }
    // step
    strcat(sstr, ostr[3]->GetString().Data());
    if (strlen(ostr[3]->GetString().Data()) < 12) {
      memset(spaces, 0, sizeof(spaces));
      memset(spaces, ' ', 12-strlen(ostr[3]->GetString().Data()));
      strcat(sstr, spaces);
    } else {
      strcat(sstr, " ");
    }
    // pos. error
    strcat(sstr, "none        ");
    // lower boundary
    strcat(sstr, ostr[4]->GetString().Data());
    if (strlen(ostr[4]->GetString().Data()) < 8) {
      memset(spaces, 0, sizeof(spaces));
      memset(spaces, ' ', 8-strlen(ostr[4]->GetString().Data()));
      strcat(sstr, spaces);
    } else {
      strcat(sstr, " ");
    }
    // upper boundary
    strcat(sstr, ostr[5]->GetString().Data());
    strcpy(str, sstr);
  }

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  // check if the end of the parameter block is reached
  unsigned int i;
  for (i=0; i<strlen(str); i++) {
    if (!isblank(str[i]) || !iscntrl(str[i]))
      break;
  }
  if (i == strlen(str)) // end reached
    tag = MSR_TAG_NO_BLOCK;
}

//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  // check the number of arguments
  if (argc != 3) {
    msr2msr_syntax();
    return 0;
  }

  // open input msr-file
  ifstream fin;
  fin.open(argv[1], iostream::in);
  if (!fin.is_open()) {
    cout << endl << "**ERROR**: Couldn't open input msr-file " << argv[1];
    cout << endl << "           Will quit." << endl;
    return 0;
  }

  // open input msr-file
  ofstream fout;
  fout.open(argv[2], iostream::out);
  if (!fout.is_open()) {
    cout << endl << "**ERROR**: Couldn't open output msr-file " << argv[2];
    cout << endl << "           Will quit." << endl;
    fin.close();
    return 0;
  }

  // read input file and write output file
  char str[256];
  int  tag;
  bool success = true;
  while (!fin.eof() && success) {
    fin.getline(str, sizeof(str));
    if (strstr(str, "FITPARAMETER")) {
      tag = MSR_TAG_FITPARAMETER;
    } else if (strstr(str, "RUN")) { // analyze and change header
      success = msr2msr_run(str);
    }
    if (tag == MSR_TAG_FITPARAMETER) {
      msr2msr_param(str, tag);
    }
    fout << str << endl;
  }

  // close files
  fout.close();
  fin.close();

  // check if conversion seems to be OK
  if (!success) {
    sprintf(str, "rm -rf %s", argv[2]);
    system(str);
  }

  // clean up

  return 1;
}
