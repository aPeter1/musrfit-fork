/***************************************************************************

  PVarHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#include "PVarHandler.h"

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::PVarHandler
 */
PVarHandler::PVarHandler()
{
  fInput = "";
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::parse
 * @return
 */
bool PVarHandler::parse()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::semcheck
 * @return
 */
bool PVarHandler::semcheck()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getValues
 * @return
 */
std::vector<double> PVarHandler::getValues()
{
  std::vector<double> result;

  return result;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getErrors
 * @return
 */
std::vector<double> PVarHandler::getErrors()
{
  std::vector<double> result;

  return result;
}
