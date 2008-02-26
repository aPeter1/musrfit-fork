/***************************************************************************

  musrparam.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2008 by Andreas Suter                                   *
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
#include <vector>
using namespace std;

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

void musrparam_syntax()
{
  cout << endl << "usage: musrparam <input-filename> <output-filename>";
  cout << endl << "  <input-filename>: file name of the control file to extract the parameters.";
  cout << endl << "  It has the following structure:";
  cout << endl << "  msr-file-name-1, independent-var1-1, independent-var2-1";
  cout << endl << "  msr-file-name-2, independent-var1-2, independent-var2-2";
  cout << endl << "  etc.";
  cout << endl << "  It is allowed to add comment lines which start with '%'";
  cout << endl;
  cout << endl << "  <output-filename>: file name of the generated output file";
  cout << endl << "  The output will have the structure:";
  cout << endl << "  msr-file-name-1, independent-var1-1, ..., par1, err_par1, par2, err_par2, par3, err_par3, ... ";
  cout << endl << "  msr-file-name-2, independent-var1-2, ..., par1, err_par1, par2, err_par2, par3, err_par3, ... ";
  cout << endl << "  etc.";
  cout << endl << "  If positive and negative errors are present, it will be par1, err_par1-, err_par1+, etc.";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    musrparam_syntax();
    return -1;
  }

  TString inputFln(argv[1]);
  TString outputFln(argv[2]);

  vector<TString> param;

  // open input file for read
  ifstream fin;
  fin.open(inputFln.Data(), iostream::in);
  if (!fin.is_open()) {
    cout << endl << "**ERROR** Couldn't open input file " << inputFln.Data() << " will quit.";
    cout << endl << endl;
    return -1;
  }
  // read file
  int lineNo = 0;
  char str[256];
  TString tstr;
  TString paramStr;
  TObjArray *tokens;
  TObjString *ostr;
  ifstream fmsr;
  while (!fin.eof()) {
    // read a line
    fin.getline(str, sizeof(str));
    lineNo++;

    // check if only a comment or empty line
    if ((str[0] == '%') || strlen(str) == 0)
      continue;

    // tokenize string
    tstr = TString(str);
    tokens = tstr.Tokenize(" ,;\t");

    // keep msr-file name and temperature
    paramStr = TString(str) + TString(", ");

    // open msr-file
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    fmsr.open(ostr->GetString().Data(), iostream::in);
    if (!fmsr.is_open()) {
      cout << endl << "**ERROR** Couldn't open msr file " << ostr->GetString().Data() << " will quit.";
      cout << endl << endl;
      fin.close();
      return -1;
    }

    // read msr-file relevant parts
    bool fitParamFound = false;
    bool done = false;
    TString msrLine;
    TObjArray *msrTokens;
    TObjString *msrOstr;
    while (!fmsr.eof() && !done) {
      // read a line
      fmsr.getline(str, sizeof(str));
      msrLine = TString(str);
      if (msrLine.Contains("FITPARAMETER")) {
        fitParamFound = true;
        continue;
      }
      if (fitParamFound) {
        if (msrLine.BeginsWith("#")) // comment
          continue;
        if (msrLine.IsWhitespace()) { // end if FTIPARAMETER block
          done = true;
          continue;
        }

        // get parameters and errors
        msrTokens = msrLine.Tokenize(" \t");
        if (msrTokens->GetEntries() < 4) { // not enough tokens: no name value error
          cout << endl << "**ERROR** Found " << tokens->GetEntries() << " tokens only (in " << ostr->GetString().Data() << "), will quit.";
          cout << endl << endl;
          fmsr.close();
          fin.close();
          return -1;
        }

        // keep parameter value
        msrOstr = dynamic_cast<TObjString*>(msrTokens->At(2));
        paramStr += msrOstr->GetString();
        paramStr += TString(", ");
        // keep parameter error/neg. error
        msrOstr = dynamic_cast<TObjString*>(msrTokens->At(3));
        paramStr += msrOstr->GetString();
        paramStr += TString(", ");

        if (msrTokens->GetEntries() >= 5) { // check for positive errors
          msrOstr = dynamic_cast<TObjString*>(msrTokens->At(4));
          if (msrOstr->GetString().IsFloat()) {
            paramStr += msrOstr->GetString();
            paramStr += TString(", ");
          }
        }

        // clean up
        if (msrTokens) {
          delete msrTokens;
          msrTokens = 0;
        }
      }
    }
    fmsr.close();

    paramStr.Remove(TString::kTrailing, ' ');
    paramStr.Remove(TString::kTrailing, ',');
    param.push_back(paramStr);

    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }
  fin.close();

  // open output file for write
  ofstream fout;
  fout.open(outputFln.Data(), iostream::out);
  if (!fout.is_open()) {
    cout << endl << "**ERROR** Couldn't open output file " << outputFln.Data() << " will quit.";
    cout << endl << endl;
    return -1;
  }
  for (UInt_t i=0; i<param.size(); i++) {
    fout << param[i] << endl;
  }
  fout.close();

  // clean up
  param.clear();

  return 0;
}
