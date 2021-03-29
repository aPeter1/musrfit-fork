/***************************************************************************

  PInstrumentDef.cpp

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

#include "PInstrumentDef.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 *
 */
PSetup::PSetup()
{
  fName = "";
  fNoOfDetectors = -1;
  fFgbOffset = -1;
  fLgb = -1;
  fBkgStartBin = -1;
  fBkgEndBin = -1;
}

//--------------------------------------------------------------------------
/**
 * @brief PSetup::getDetector
 * @param idx
 * @return
 */
PDetector* PSetup::getDetector(int idx)
{
  if (idx >= fLogicDetectors.size())
    return 0;

  return &fLogicDetectors[idx];
}

//--------------------------------------------------------------------------
/**
 * @brief PSetup::getAsymDetector
 * @param idx
 * @return
 */
PDetector* PSetup::getAsymDetector(int idx)
{
  if (idx >= fLogicAsymDetectors.size())
    return 0;

  return &fLogicAsymDetectors[idx];
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 *
 */
PInstrument::PInstrument()
{
  fRunNameTemplate = "";
  fBeamline = "";
  fDataFileFormat = "";
  fInstitue = "";
  fName = "";
}

//--------------------------------------------------------------------------
/**
 * @brief PInstrument::getZFSetup
 * @param name
 * @return
 */
PSetup* PInstrument::getZFSetup(QString name)
{
  for (int i=0; i<fZF.size(); i++) {
    if (fZF[i].getName() == name)
      return &fZF[i];
  }
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PInstrument::getTFSetup
 * @param name
 * @return
 */
PSetup* PInstrument::getTFSetup(QString name)
{
  for (int i=0; i<fTF.size(); i++) {
    if (fTF[i].getName() == name)
      return &fTF[i];
  }
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PInstrument::getLFSetup
 * @param name
 * @return
 */
PSetup* PInstrument::getLFSetup(QString name)
{
  for (int i=0; i<fLF.size(); i++) {
    if (fLF[i].getName() == name)
      return &fLF[i];
  }
  return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 *
 */
PInstitute::PInstitute()
{
  fName = "";
}

//--------------------------------------------------------------------------
/**
 *
 */
PInstrument *PInstitute::getInstrument(QString name)
{
  for (int i=0; i<fInstrument.size(); i++) {
    if (fInstrument[i].getName() == name)
      return &fInstrument[i];
  }

  return 0;
}
