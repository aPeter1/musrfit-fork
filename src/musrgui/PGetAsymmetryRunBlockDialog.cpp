/****************************************************************************

  PGetAsymmetryRunBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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

#include <qlineedit.h>
#include <qvalidator.h>

#include "PGetAsymmetryRunBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetAsymmetryRunBlockDialog::PGetAsymmetryRunBlockDialog()
{
  fForward_lineEdit->setValidator( new QIntValidator(fForward_lineEdit) );
  fBackward_lineEdit->setValidator( new QIntValidator(fBackward_lineEdit) );
  fDataForwardStart_lineEdit->setValidator( new QIntValidator(fDataForwardStart_lineEdit) );
  fDataForwardEnd_lineEdit->setValidator( new QIntValidator(fDataForwardEnd_lineEdit) );
  fDataBackwardStart_lineEdit->setValidator( new QIntValidator(fDataBackwardStart_lineEdit) );
  fDataBackwardEnd_lineEdit->setValidator( new QIntValidator(fDataBackwardEnd_lineEdit) );
  fBackgroundForwardStart_lineEdit->setValidator( new QIntValidator(fBackgroundForwardStart_lineEdit) );
  fBackgroundForwardEnd_lineEdit->setValidator( new QIntValidator(fBackgroundForwardEnd_lineEdit) );
  fBackgroundBackwardStart_lineEdit->setValidator( new QIntValidator(fBackgroundBackwardStart_lineEdit) );
  fBackgroundBackwardEnd_lineEdit->setValidator( new QIntValidator(fBackgroundBackwardEnd_lineEdit) );
  fBackgroundForwardFix_lineEdit->setValidator( new QDoubleValidator(fBackgroundForwardFix_lineEdit) );
  fBackgroundBackwardFix_lineEdit->setValidator( new QDoubleValidator(fBackgroundBackwardFix_lineEdit) );
  fFitRangeStart_lineEdit->setValidator( new QDoubleValidator(fFitRangeStart_lineEdit) );
  fFitRangeEnd_lineEdit->setValidator( new QDoubleValidator(fFitRangeEnd_lineEdit) );
  fPacking_lineEdit->setValidator( new QIntValidator(fPacking_lineEdit) );
  fAlpha_lineEdit->setValidator( new QIntValidator(fAlpha_lineEdit) );
  fBeta_lineEdit->setValidator( new QIntValidator(fBeta_lineEdit) );
  fT0Forward_lineEdit->setValidator( new QIntValidator(fT0Forward_lineEdit) );
  fT0Backward_lineEdit->setValidator( new QIntValidator(fT0Backward_lineEdit) );
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
