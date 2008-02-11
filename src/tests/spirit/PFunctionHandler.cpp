/***************************************************************************

  PFunctionHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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

#include <string>
#include <cassert>

#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>

#include "PFunctionHandler.h"

//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param fln
 */
PFunctionHandler::PFunctionHandler(char *fln, bool debug) : fDebug(debug)
{
  fValid = true;
  fFileName = QString(fln);

  cout << endl << "in PFunctionHandler(char *fln)";
  cout << endl << "fFileName = " << fFileName.latin1();

  fValid = ReadFile();
  if (fValid)
    fValid = MapsAreValid();
}

//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param lines
 */
PFunctionHandler::PFunctionHandler(vector<QString> lines)
{
  fValid = true;
  fFileName = "";

  cout << endl << "in PFunctionHandler(vector<QString> lines)";

  if (lines.size() == 0) {
    fValid = false;
    return;
  }

  // analyze input
  bool done = false;
  int status;
  int val[10];
  double dval[10];
  bool inFcnBlock = false;
  for (unsigned int i=0; i<lines.size(); i++) {
    if (lines[i].startsWith("#")) // comment hence ignore
      continue;
    lines[i] = lines[i].upper();
    if (lines[i].startsWith("PAR")) {
      cout << endl << "this is a parameter line ...";
      status = sscanf(lines[i].latin1(), "PAR %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                      &dval[0], &dval[1], &dval[2], &dval[3], &dval[4],
                      &dval[5], &dval[6], &dval[7], &dval[8], &dval[9]);
      if (status < 0) {
        done = true;
        fValid = false;
        cout << endl << "invalid PAR line, sorry ...";
      } else { // fill map
        cout << endl << "PAR line, status = " << status;
        for (int i=0; i<status; i++)
          fParam.push_back(dval[i]);
      }
    } else if (lines[i].startsWith("MAP")) {
      cout << endl << "this is a map line ...";
      status = sscanf(lines[i].latin1(), "MAP %d %d %d %d %d %d %d %d %d %d",
                      &val[0], &val[1], &val[2], &val[3], &val[4],
                      &val[5], &val[6], &val[7], &val[8], &val[9]);
      if (status < 0) {
        done = true;
        fValid = false;
        cout << endl << "invalid MAP line, sorry ...";
      } else { // fill map
        cout << endl << "MAP line, status = " << status;
        for (int i=0; i<status; i++)
          fMap.push_back(val[i]);
      }
    } else if (lines[i].startsWith("FUNCTIONS")) {
      cout << endl << "the functions block start ...";
      inFcnBlock = true;
    } else if (lines[i].startsWith("END")) {
      cout << endl << "end tag found; rest will be ignored";
      done = true;
    } else if (inFcnBlock) {
      fLines.push_back(lines[i]);
    }
  }

  // check if all blocks are given
  if ((fMap.size() == 0) || (fParam.size() == 0) || (fLines.size() == 0)) {
    if (fMap.size() == 0)
      cout << endl << "MAP block is missing ...";
    if (fParam.size() == 0)
      cout << endl << "PAR block is missing ...";
    if (fLines.size() == 0)
      cout << endl << "FUNCTION block is missing ...";
    fValid = false;
  }

  fValid = MapsAreValid();

if (fValid) {
  cout << endl << "Functions: ";
  for (unsigned int i=0; i<fLines.size(); i++)
    cout << endl << fLines[i].latin1();
}
}

//-------------------------------------------------------------
// Destructor
//-------------------------------------------------------------
/**
 * <p>
 *
 */
PFunctionHandler::~PFunctionHandler()
{
  cout << endl << "in ~PFunctionHandler()" << endl << endl;
  fParam.clear();
  fMap.clear();
  fLines.clear();

  fFuncs.clear();
}

//-------------------------------------------------------------
// DoParse (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
bool PFunctionHandler::DoParse()
{
  cout << endl << "in PFunctionHandler::DoParse() ...";

  bool success = true;
  PFunctionGrammar function;

  for (unsigned int i=0; i<fLines.size(); i++) {
cout << endl << "fLines[" << i << "] = '" << fLines[i].latin1() << "'";

    tree_parse_info<> info = ast_parse(fLines[i].latin1(), function, space_p);

    if (info.full) {
      cout << endl << "parse successfull ..." << endl;
      PFunction func(info, fParam, fMap, fDebug);
      fFuncs.push_back(func);
    } else {
      cout << endl << "parse failed ... (" << i << ")" << endl;
      success = false;
      break;
    }
  }

  // check that all functions are valid. It could be that parsing was fine but
  // the parameter index, or map index was out of range
  if (success) {
    for (unsigned int i=0; i<fFuncs.size(); i++) {
      if (!fFuncs[i].IsValid()) {
        cout << endl << "**ERROR**: function fun" << fFuncs[i].GetFuncNo();
        cout << " has a problem with either parameter or map out of range!";
        success = false;
        break;
      }
    }
  }

  // check that the function numbers are unique
  if (success) {
    for (unsigned int i=0; i<fFuncs.size(); i++) {
      for (unsigned int j=i+1; j<fFuncs.size(); j++) {
        if (fFuncs[i].GetFuncNo() == fFuncs[j].GetFuncNo()) {
          cout << endl << "**ERROR**: function number " << fFuncs[i].GetFuncNo();
          cout << " is at least twice present! Fix this first.";
          success = false;
        }
      }
    }
  }

  if (success) {
    for (unsigned int i=0; i<fFuncs.size(); i++)
      cout << endl << "func number = " << fFuncs[i].GetFuncNo();
  }

  return success;
}

//-------------------------------------------------------------
// Eval (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 */
double PFunctionHandler::Eval(int i)
{
  if (GetFuncIndex(i) == -1) {
    cout << endl << "**ERROR**: Couldn't find FUN" << i << " for evaluation";
    return 0.0;
  }

cout << endl << "PFunctionHandler::Eval: GetFuncIndex("<<i<<") = " << GetFuncIndex(i);
cout << endl;

  return fFuncs[GetFuncIndex(i)].Eval();
}

//-------------------------------------------------------------
// GetFuncNo (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 */
unsigned int PFunctionHandler::GetFuncNo(unsigned int i)
{
  if (i > fFuncs.size())
    return -1;

  return fFuncs[i].GetFuncNo();
}

//-------------------------------------------------------------
// ReadFile (private)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
bool PFunctionHandler::ReadFile()
{
  cout << endl << "in ~PFunctionHandler::ReadFile()";

  if (fFileName.isEmpty()) {
    cout << endl << "PFunctionHandler::ReadFile(): **ERROR**";
    cout << endl << "  no file name given :-(. Will quit";
    return false;
  }

  QFile f(fFileName);
  if (!f.exists()) {
    cout << endl << "PFunctionHandler::ReadFile(): **ERROR**";
    cout << endl << "  File '" << fFileName.latin1() << "' does not exist.";
    return false;
  }

  if (!f.open(IO_ReadOnly)) {
    cout << endl << "PFunctionHandler::ReadFile(): **ERROR**";
    cout << endl << "  File '" << fFileName.latin1() << "' couldn't being opened.";
    return false;
  }

  QTextStream stream(&f);
  QString line;
  bool done = false;
  bool success = true;
  int status;
  int val[10];
  double dval[10];
  bool inFcnBlock = false;
  while ( !stream.atEnd() && !done) {
    line = stream.readLine(); // line of text excluding '\n'
    if (line.startsWith("#")) // comment hence ignore
      continue;
    line = line.upper();
    if (line.startsWith("PAR")) {
      cout << endl << "this is a parameter line ...";
      status = sscanf(line.latin1(), "PAR %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                      &dval[0], &dval[1], &dval[2], &dval[3], &dval[4],
                      &dval[5], &dval[6], &dval[7], &dval[8], &dval[9]);
      if (status < 0) {
        done = true;
        success = false;
        cout << endl << "invalid PAR line, sorry ...";
      } else { // fill map
        cout << endl << "PAR line, status = " << status;
        for (int i=0; i<status; i++)
          fParam.push_back(dval[i]);
      }
    } else if (line.startsWith("MAP")) {
      cout << endl << "this is a map line ...";
      status = sscanf(line.latin1(), "MAP %d %d %d %d %d %d %d %d %d %d",
                      &val[0], &val[1], &val[2], &val[3], &val[4],
                      &val[5], &val[6], &val[7], &val[8], &val[9]);
      if (status < 0) {
        done = true;
        success = false;
        cout << endl << "invalid MAP line, sorry ...";
      } else { // fill map
        cout << endl << "MAP line, status = " << status;
        for (int i=0; i<status; i++)
          fMap.push_back(val[i]);
      }
    } else if (line.startsWith("FUNCTIONS")) {
      cout << endl << "the functions block start ...";
      inFcnBlock = true;
    } else if (line.startsWith("END")) {
      cout << endl << "end tag found; rest will be ignored";
      done = true;
    } else if (inFcnBlock) {
      fLines.push_back(line);
    }
  }


  f.close();

  // check if all blocks are given
  if ((fMap.size() == 0) || (fParam.size() == 0) || (fLines.size() == 0)) {
    if (fMap.size() == 0)
      cout << endl << "MAP block is missing ...";
    if (fParam.size() == 0)
      cout << endl << "PAR block is missing ...";
    if (fLines.size() == 0)
      cout << endl << "FUNCTION block is missing ...";
    success = false;
  }

if (success) {
  cout << endl << "Functions: ";
  for (unsigned int i=0; i<fLines.size(); i++)
    cout << endl << fLines[i].latin1();
}

  return success;
}

//-------------------------------------------------------------
// MapsAreValid (private)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
bool PFunctionHandler::MapsAreValid()
{
  bool success = true;

  int maxParam = fParam.size();
  for (unsigned int i=0; i<fMap.size(); i++)
    if (fMap[i] > maxParam)
      success = false;

  if (!success)
    cout << endl << "invalid MAP found ...";

  return success;
}

//-------------------------------------------------------------
// GetFuncIndex (private)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param funcNo
 */
int PFunctionHandler::GetFuncIndex(int funcNo)
{
  int index = -1;

  for (unsigned int i=0; i<fFuncs.size(); i++) {
    if (fFuncs[i].GetFuncNo() == funcNo) {
      index = i;
      break;
    }
  }

  return index;
}

