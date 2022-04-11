/***************************************************************************

  msr2msr.cpp

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

#include <iostream>
#include <fstream>

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

//-------------------------------------------------------------
// msr theory tags
#define MSR_THEORY_INTERN_FLD    0
#define MSR_THEORY_INTERN_BESSEL 1

//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void msr2msr_syntax()
{
  std::cout << std::endl << "usage: msr2msr <msr-file-in> <msr-file-out> | [--help]";
  std::cout << std::endl << "  <msr-file-in> : input msr-file";
  std::cout << std::endl << "  <msr-file-out>: converted msr-output-file";
  std::cout << std::endl << "  if the <msr-file-in> is already in the 2008 format";
  std::cout << std::endl << "  the output file will be identical to the input file.";
  std::cout << std::endl << std::endl;
}

//--------------------------------------------------------------------------
/**
 * <p>If the msr-file line is the RUN line, change it to the new format.
 *
 * <b>return:</b>
 * - true if everything went smooth
 * - false otherwise
 *
 * \param str msr-file line
 */
bool msr2msr_run(char *str)
{
  // not the RUN line itself, hence nothing to be done
  if (!strstr(str, "RUN"))
    return true;

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
    std::cout << std::endl << "**ERROR**: Something is wrong with the RUN block header:";
    std::cout << std::endl << "  >> " << str;
    std::cout << std::endl << "  >> no <msr-file-out> is created";
    std::cout << std::endl;
    return false;
  }

  if (tokens->GetEntries() == 5) { // already a new msr file, do only add the proper run comment
    sprintf(str, "%s (name beamline institute data-file-format)", line.Data());
    return true;
  }

  if (run.Contains("NEMU")) {
    ostr[0] = dynamic_cast<TObjString*>(tokens->At(1)); // file name
    sprintf(str, "RUN %s MUE4 PSI WKM   (name beamline institute data-file-format)", ostr[0]->GetString().Data());
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

  // clean up
  if (tokens) {
    delete tokens;
    tokens = nullptr;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Changes the fit parameter lines to the new msr-file format. In the new msr-file
 * format there is an additional column "Pos_Error". Before a fit takes place the value
 * is set to "none".
 *
 * <b>return:</b>
 * - true if everything went smooth
 * - false otherwise
 *
 * \param str msr-file fit parameter line
 */
bool msr2msr_param(char *str)
{
  // check for comment header which needs to be replaced
  if (strstr(str, "Nr.")) {
    strcpy(str, "#       No Name        Value     Step        Pos_Error   Boundaries");
    return true;
  }

  // handle parameter line
  TString line(str);
  TObjArray *tokens;
  TObjString *ostr[6];
  char sstr[256];
  char spaces[256];

  tokens = line.Tokenize(" \t");
  Int_t noTokens = tokens->GetEntries();
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
    tokens = nullptr;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>In the theory the table functions needs to be replaced by the numerical ones, and some
 * functions have new a different parameterization.
 *
 * <b>return:</b>
 * - true if everything went smooth
 * - false otherwise
 *
 * \param str msr-file theory line
 * \param tag if a function has new some additional parameters, this flag will be set, such that these new parameters can be added to the fit parameter block
 * \param noOfAddionalParams counts the number of additional parameters needed.
 */
bool msr2msr_theory(char *str, int &tag, int &noOfAddionalParams)
{
  // handle theory line
  TString line(str);
  TObjArray *tokens;
  TObjString *ostr;
  char sstr[256];

  if ((line.Contains("sktt") || line.Contains("statKTTab")) && line.Contains("glf")) { // static Gauss KT LF table
    // change cmd name
    strcpy(sstr, "statGssKTLF ");

    // tokenize the rest and extract the first two parameters
    tokens = line.Tokenize(" \t");
    Int_t noTokens = tokens->GetEntries();
    if (noTokens < 3) {
      std::cout << std::endl << "**ERROR** in THEORY block";
      std::cout << std::endl << "  Line: '" << str << "' is not a valid statKTTab statement.";
      std::cout << std::endl << "  Cannot handle file." << std::endl;
      return false;
    }
    for (Int_t i=1; i<3; i++) {
      strcat(sstr, "  ");
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      strcat(sstr, ostr->GetString().Data());
    }
    strcat(sstr, "  (frequency damping)");
    strcpy(str, sstr);
  } else if ((line.Contains("sktt") || line.Contains("statKTTab")) && line.Contains("llf")) { // static Lorentz KT LF table
    // change cmd name
    strcpy(sstr, "statExpKTLF ");

    // tokenize the rest and extract the first two parameters
    tokens = line.Tokenize(" \t");
    Int_t noTokens = tokens->GetEntries();
    if (noTokens < 3) {
      std::cout << std::endl << "**ERROR** in THEORY block";
      std::cout << std::endl << "  Line: '" << str << "' is not a valid statKTTab statement.";
      std::cout << std::endl << "  Cannot handle file." << std::endl;
      return false;
    }
    for (Int_t i=1; i<3; i++) {
      strcat(sstr, "  ");
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      strcat(sstr, ostr->GetString().Data());
    }
    strcat(sstr, "  (frequency damping)");
    strcpy(str, sstr);
  } else if ((line.Contains("dktt") || line.Contains("dynmKTTab")) && line.Contains("kdglf")) { // dynamic Gauss KT LF table
    // change cmd name
    strcpy(sstr, "dynGssKTLF ");

    // tokenize the rest and extract the first three parameters
    tokens = line.Tokenize(" \t");
    Int_t noTokens = tokens->GetEntries();
    if (noTokens < 4) {
      std::cout << std::endl << "**ERROR** in THEORY block";
      std::cout << std::endl << "  Line: '" << str << "' is not a valid dynmKTTab statement.";
      std::cout << std::endl << "  Cannot handle file." << std::endl;
      return false;
    }
    for (Int_t i=1; i<4; i++) {
      strcat(sstr, "  ");
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      strcat(sstr, ostr->GetString().Data());
    }
    strcat(sstr, "  (frequency damping hopping-rate)");
    strcpy(str, sstr);
  } else if ((line.Contains("dktt") || line.Contains("dynmKTTab")) && line.Contains("kdllf")) { // dynamic Lorentz KT LF table
    // change cmd name
    strcpy(sstr, "dynExpKTLF ");

    // tokenize the rest and extract the first three parameters
    tokens = line.Tokenize(" \t");
    Int_t noTokens = tokens->GetEntries();
    if (noTokens < 4) {
      std::cout << std::endl << "**ERROR** in THEORY block";
      std::cout << std::endl << "  Line: '" << str << "' is not a valid dynmKTTab statement.";
      std::cout << std::endl << "  Cannot handle file." << std::endl;
      return false;
    }
    for (Int_t i=1; i<4; i++) {
      strcat(sstr, "  ");
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      strcat(sstr, ostr->GetString().Data());
    }
    strcat(sstr, "  (frequency damping hopping-rate)");
    strcpy(str, sstr);
  } else if (line.Contains("internFld")) {
    tag = MSR_THEORY_INTERN_FLD;
    noOfAddionalParams++;

    // change cmd name
    strcpy(sstr, "internFld ");

    // tokenize the rest and extract the first three parameters
    tokens = line.Tokenize(" \t");
    Int_t noTokens = tokens->GetEntries();
    if (noTokens < 4) {
      std::cout << std::endl << "**ERROR** in THEORY block";
      std::cout << std::endl << "  Line: '" << str << "' is not a valid internFld statement.";
      std::cout << std::endl << "  Cannot handle file." << std::endl;
      return false;
    }
    strcat(sstr, "  _x_");
    for (Int_t i=1; i<4; i++) {
      strcat(sstr, "  ");
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      strcat(sstr, ostr->GetString().Data());
    }
    strcat(sstr, "  (fraction phase frequency Trate Lrate)");
    strcpy(str, sstr);
  } else if (line.Contains("internBsl")) {
    tag = MSR_THEORY_INTERN_BESSEL;
    noOfAddionalParams++;

    // change cmd name
    strcpy(sstr, "internBsl ");

    // tokenize the rest and extract the first three parameters
    tokens = line.Tokenize(" \t");
    Int_t noTokens = tokens->GetEntries();
    if (noTokens < 4) {
      std::cout << std::endl << "**ERROR** in THEORY block";
      std::cout << std::endl << "  Line: '" << str << "' is not a valid internBsl statement.";
      std::cout << std::endl << "  Cannot handle file." << std::endl;
      return false;
    }
    strcat(sstr, "  _x_");
    for (Int_t i=1; i<4; i++) {
      strcat(sstr, "  ");
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      strcat(sstr, ostr->GetString().Data());
    }
    strcat(sstr, "  (fraction phase frequency Trate Lrate)");
    strcpy(str, sstr);
  }

  return true;
}


//--------------------------------------------------------------------------
/**
 * <p>Checks if a msr-line is a comment line
 *
 * <b>return:</b>
 * - true if line is a comment line
 * - false otherwise
 *
 * \param str msr-file line
 */
bool msr2msr_is_comment(char *str)
{
  bool isComment = false;

  for (unsigned int i=0; i<strlen(str); i++) {
    if ((str[i] == ' ') || (str[i] == '\t'))
      continue;
    if (str[i] == '#') {
      isComment = true;
      break;
    } else {
      isComment = false;
      break;
    }
  }

  return isComment;
}

//--------------------------------------------------------------------------
/**
 * <p>Checks if a msr-line consists only out of whitespaces.
 *
 * <b>return:</b>
 * - true if line is a whitespace
 * - false otherwise
 *
 * \param str msr-file line
 */
bool msr2msr_is_whitespace(char *str)
{
  bool isWhitespace = true;

  if (strlen(str) != 0) {
    for (unsigned int i=0; i<strlen(str); i++) {
      if ((str[i] != ' ') && (str[i] != '\t')) {
        isWhitespace = false;
        break;
      }
    }
  }

  return isWhitespace;
}

//--------------------------------------------------------------------------
/**
 * <p>Replaces temporarily set labels by the proper parameter number.
 *
 * \param str msr-file line
 * \param paramNo parameter number
 */
void msr2msr_replace(char *str, int paramNo)
{
  char temp[128];
  char no[16];

  memset(temp, 0, sizeof(temp));

  sprintf(no, "%d", paramNo);

  int j=0;
  for (unsigned int i=0; i<strlen(str); i++) {
    if (str[i] != '_') {
      temp[j] = str[i];
      j++;
    } else {
      for (unsigned int k=0; k<strlen(no); k++)
        temp[j+k] = no[k];
      j += strlen(no);
      i += 2;
    }
  }

  strcpy(str, temp);
}

//--------------------------------------------------------------------------
/**
 * <p>If a theory function was found which has newly a different number of parameters,
 * this function will finalize the msr-file, i.e. replacing all temporarily set tags.
 *
 * \param fln file name
 * \param theoryTag theory tag if the changed function
 * \param noOfAddionalParams number of additional fit parameters
 */
bool msr2msr_finalize_theory(char *fln, int theoryTag, int noOfAddionalParams)
{  
  std::ifstream fin;
  fin.open(fln, std::iostream::in);
  if (!fin.is_open()) {
    std::cout << std::endl << "**ERROR**: Couldn't open input msr-file " << fln;
    std::cout << std::endl << "           Will quit." << std::endl;
    return 0;
  }

  // open temporary output msr-file
  std::ofstream fout;
  fout.open("__temp.msr", std::iostream::out);
  if (!fout.is_open()) {
    std::cout << std::endl << "**ERROR**: Couldn't open output msr-file __temp.msr";
    std::cout << std::endl << "           Will quit." << std::endl;
    fin.close();
    return 0;
  }

  char str[256];
  int  tag = -1;
  bool success = true;
  int  param = 0;
  int  count = 0;
  while (!fin.eof() && success) {
    fin.getline(str, sizeof(str));

    if (strstr(str, "FITPARAMETER")) {
      tag = MSR_TAG_FITPARAMETER;
    } else if (strstr(str, "THEORY")) {
      tag = MSR_TAG_THEORY;
    }

    if ((tag == MSR_TAG_FITPARAMETER) && !strstr(str, "FITPARAMETER")) {
      if ((theoryTag == MSR_THEORY_INTERN_FLD) || (theoryTag == MSR_THEORY_INTERN_BESSEL)) {
        if (!msr2msr_is_comment(str)) {
          param++;
          if (msr2msr_is_whitespace(str)) {
            // add needed parameters
            for (int i=0; i<noOfAddionalParams; i++) {
              fout << "        " << param+i << " frac" << i+1 << "       0.333333 0.0        none" << std::endl;
            }
          }
        }
      }
    }

    if (tag == MSR_TAG_THEORY) {
      if ((theoryTag == MSR_THEORY_INTERN_FLD) || (theoryTag == MSR_THEORY_INTERN_BESSEL)) {
        if (strstr(str, "_x_")) {
          msr2msr_replace(str, param+count);
          count++;
        }
      }
    }

    fout << str << std::endl;
  }

  // close files
  fout.close();
  fin.close();


  // cp __temp.msr fln
  sprintf(str, "cp __temp.msr %s", fln);
  if (system(str) == -1) {
    std::cerr << "**ERROR** cmd: " << str << " failed." << std::endl;
    return false;
  }
  // rm __temp.msr
  strcpy(str, "rm __temp.msr");
  if (system(str) == -1) {
    std::cerr << "**ERROR** cmd: " << str << " failed." << std::endl;
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Filters out the old chi-square related informations. And sets them according
 * to the new scheme.
 *
 * <b>return:</b>
 * - true if everything went smooth
 * - false otherwise
 *
 * \param str msr-file statistic block line
 */
bool msr2msr_statistic(char *str) {
  bool success = true;

  char *pstr;
  int status;
  double chisq, chisqred;
  if (strstr(str, "  chi")) {
    pstr = strstr(str, "abs = ");
    if (pstr != nullptr) {
      status = sscanf(pstr, "abs = %lf", &chisq);
      if (status != 1) {
        success = false;
      }
    }
    pstr = strstr(str, "norm = ");
    if (pstr != nullptr) {
      status = sscanf(pstr, "norm = %lf", &chisqred);
      if (status != 1) {
        success = false;
      }
    }
    if (success) {
      sprintf(str, "  chisq = %lf, NDF = %d, chisq/NDF = %lf", chisq, static_cast<int>(chisq/chisqred), chisqred);
    }
  }

  return success;
}

//--------------------------------------------------------------------------
/**
 * <p>msr2msr is needed to convert old WKM msr-files to musrfit msr-files. Eventhough the syntax is <b>very</b>
 * close, there are some shortcomings in the WKM msr-files, which forced me to slightly change the file format.
 *
 * \param argc number of arguments
 * \param argv list of arguments
 */
int main(int argc, char *argv[])
{

  // check the number of arguments
  if (argc != 3) {
    msr2msr_syntax();
    return 0;
  }

  // open input msr-file
  std::ifstream fin;
  fin.open(argv[1], std::iostream::in);
  if (!fin.is_open()) {
    std::cout << std::endl << "**ERROR**: Couldn't open input msr-file " << argv[1];
    std::cout << std::endl << "           Will quit." << std::endl;
    return 0;
  }

  // open output msr-file
  std::ofstream fout;
  fout.open(argv[2], std::iostream::out);
  if (!fout.is_open()) {
    std::cout << std::endl << "**ERROR**: Couldn't open output msr-file " << argv[2];
    std::cout << std::endl << "           Will quit." << std::endl;
    fin.close();
    return 0;
  }

  // read input file and write output file
  char str[256];
  int  tag = -1;
  int  theoryTag = -1;
  int  noOfAddionalParams = 0;
  bool success = true;
  while (!fin.eof() && success) {
    fin.getline(str, sizeof(str));

    if (strstr(str, "FITPARAMETER")) {
      tag = MSR_TAG_FITPARAMETER;
    } else if (strstr(str, "RUN")) { // analyze and change header
      tag = MSR_TAG_RUN;
    } else if (strstr(str, "THEORY")) {
      tag = MSR_TAG_THEORY;
    } else if (strstr(str, "STATISTIC")) {
      tag = MSR_TAG_STATISTIC;
    }

    switch (tag) {
      case MSR_TAG_FITPARAMETER:
        success = msr2msr_param(str);
        break;
      case MSR_TAG_THEORY:
        success = msr2msr_theory(str, theoryTag, noOfAddionalParams);
        break;
      case MSR_TAG_RUN:
        success = msr2msr_run(str);
        break;
      case MSR_TAG_STATISTIC:
        success = msr2msr_statistic(str);
        break;
      default:
        break;
    }

    fout << str << std::endl;
  }

  // close files
  fout.close();
  fin.close();

  // check if conversion seems to be OK
  if (!success) {
    sprintf(str, "rm -rf %s", argv[2]);
    if (system(str) == -1) {
      std::cerr << "**ERROR** cmd: " << str << " failed." << std::endl;
      return 0;
    }
  }

  if (theoryTag != -1) {
    msr2msr_finalize_theory(argv[2], theoryTag, noOfAddionalParams);
  }

  std::cout << std::endl << "done ..." << std::endl;

  return 1;
}
