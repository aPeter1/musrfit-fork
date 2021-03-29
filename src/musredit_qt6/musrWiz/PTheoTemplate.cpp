/***************************************************************************

  PTheoTemplate.cpp

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

#include "PTheoTemplate.h"

//---------------------------------------------------------------------------
/**
 *
 */
void PTheoTemplate::init()
{
  fPreDefName = QString("");
  fTheory = QString("");
  fParam.clear();
  fFunc.clear();
  fMap.clear();
}

//---------------------------------------------------------------------------
/**
 *
 */
QString PTheoTemplate::getFunc(int idx)
{
  if ((idx < 0) || (idx >= fFunc.size()))
    return QString("");

  return fFunc[idx];
}

//---------------------------------------------------------------------------
/**
 *
 */
int PTheoTemplate::getFuncNo(int idx)
{
  if ((idx < 0) || (idx >= fFunc.size()))
    return -1;

  QString funStr = fFunc[idx];
  int idxEnd = funStr.indexOf("=");
  funStr.remove(idxEnd, funStr.length()-idxEnd);
  funStr.remove(0,3);

  bool ok;
  int ival = funStr.toInt(&ok);
  if (!ok)
    return -1;

  return ival;
}

//---------------------------------------------------------------------------
/**
 *
 */
PParam PTheoTemplate::getMap(int idx)
{
  PParam map;

  if ((idx < 0) || (idx >= fMap.size()))
    return map;

  return fMap[idx];
}

//---------------------------------------------------------------------------
/**
 *
 */
PParam PTheoTemplate::getParam(int idx)
{
  PParam param;

  if ((idx < 0) || (idx >= fParam.size()))
    return param;

  return fParam[idx];
}

//---------------------------------------------------------------------------
/**
 *
 */
void PTheoTemplate::appendFunc(QString str)
{
  // for now just push function string. In the future it might be
  // wise to check if the function is already present.
  fFunc.push_back(str);
}

//---------------------------------------------------------------------------
/**
 *
 */
void PTheoTemplate::appendMap(PParam map)
{
  bool found = false;
  for (int i=0; i<fMap.size(); i++) {
    if (fMap[i].getNumber() == map.getNumber()) {
      found = true;
      break;
    }
  }

  if (!found)
    fMap.push_back(map);
}

//---------------------------------------------------------------------------
/**
 *
 */
void PTheoTemplate::appendParam(PParam param)
{
  bool found = false;
  for (int i=0; i<fParam.size(); i++) {
    if (fParam[i].getNumber() == param.getNumber()) {
      found = true;
      break;
    }
  }

  if (!found)
    fParam.push_back(param);
}

//---------------------------------------------------------------------------
// END
//---------------------------------------------------------------------------
