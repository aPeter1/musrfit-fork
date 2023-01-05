/****************************************************************************

  PGetAsymmetryRunBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2023 by Andreas Suter                              *
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

#include <QLineEdit>
#include <QValidator>
#include <QComboBox>
#include <QMessageBox>

#include "PHelp.h"

#include "PGetAsymmetryRunBlockDialog.h"


//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param helpUrl help url for the asymmetry run block
 */
PGetAsymmetryRunBlockDialog::PGetAsymmetryRunBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  fForwardHistoNo_lineEdit->setValidator( new QIntValidator(fForwardHistoNo_lineEdit) );
  fBackwardHistoNo_lineEdit->setValidator( new QIntValidator(fBackwardHistoNo_lineEdit) );
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
/**
 * <p>returns the run information line of the asymmetry run block.
 */
QString PGetAsymmetryRunBlockDialog::getRunHeaderInfo()
{
  QString str;

  str  = "RUN " + fRunFileName_lineEdit->text() + " ";
  str += fBeamline_lineEdit->text().toUpper() + " ";
  str += fInstitute_comboBox->currentText() + " ";
  str += fFileFormat_comboBox->currentText() + "   (name beamline institute data-file-format)\n";

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the alpha parameter for the asymmetry run block.
 *
 * \param present flag indicating if the alpha parameter is used, or a default alpha==1 is going to be used.
 */
QString PGetAsymmetryRunBlockDialog::getAlphaParameter(bool &present)
{
  QString str = "alpha           " + fAlpha_lineEdit->text() + "\n";

  if (str.isEmpty())
    present = false;
  else
    present = true;

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the beta parameter for the asymmetry run block
 *
 * \param present flag indicating if the beta parameter is used, or a default beta==1 is going to be used.
 */
QString PGetAsymmetryRunBlockDialog::getBetaParameter(bool &present)
{
  QString str = "beta            " + fBeta_lineEdit->text() + "\n";

  if (str.isEmpty())
    present = false;
  else
    present = true;

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the map for the asymmetry run block.
 *
 * \param valid flag indicating if the map is potentially valid.
 */
QString PGetAsymmetryRunBlockDialog::getMap(bool &valid)
{
  QString str = fMap_lineEdit->text().trimmed().remove(" ");

  // check if potentially proper map line
  for (int i=0; i<str.length(); i++) {
    if (!str[i].isDigit()) {
      valid = false;
      break;
    }
  }

  str = "map             " + fMap_lineEdit->text() + "\n";

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the background information for the asymmetry run block.
 *
 * \param valid flag indicating if a valid background (either backgr.fix or background is given, but not both)
 * is present.
 */
QString PGetAsymmetryRunBlockDialog::getBackground(bool &valid)
{
  QString str = "";

  valid = true;

  // check that either backgr.fix or background is given, but not both
  if (fBackgroundForwardStart_lineEdit->text().isEmpty() && fBackgroundForwardEnd_lineEdit->text().isEmpty() &&
      fBackgroundBackwardStart_lineEdit->text().isEmpty() && fBackgroundBackwardEnd_lineEdit->text().isEmpty() &&
      fBackgroundForwardFix_lineEdit->text().isEmpty() && fBackgroundBackwardFix_lineEdit->text().isEmpty()) {
    valid = false;
    str = "background      0  10  0  10\n";
  } else {
    if (!fBackgroundForwardStart_lineEdit->text().isEmpty()) { // assume the rest is given, not fool prove but ...
      str  = "background      ";
      str += fBackgroundForwardStart_lineEdit->text() + "   ";
      str += fBackgroundForwardEnd_lineEdit->text() + "   ";
      str += fBackgroundBackwardStart_lineEdit->text() + "   ";
      str += fBackgroundBackwardEnd_lineEdit->text() + "\n";
    }
    if (!fBackgroundForwardFix_lineEdit->text().isEmpty()) { // assume the rest is given, not fool prove but ...
      str  = "backgr.fix      ";
      str += fBackgroundForwardFix_lineEdit->text() + "   ";
      str += fBackgroundBackwardFix_lineEdit->text() + "\n";
    }
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns a data range (in bins) entry for the asymmetry run block.
 *
 * \param valid flag indicating if the data entries are valid.
 */
QString PGetAsymmetryRunBlockDialog::getData(bool &valid)
{
  QString str = "";

  if (fDataForwardStart_lineEdit->text().isEmpty() || fDataForwardEnd_lineEdit->text().isEmpty() ||
      fDataBackwardStart_lineEdit->text().isEmpty() || fDataBackwardEnd_lineEdit->text().isEmpty()) {
    valid = false;
  } else {
    str  = "data            ";
    str += fDataForwardStart_lineEdit->text() + "   ";
    str += fDataForwardEnd_lineEdit->text() + "   ";
    str += fDataBackwardStart_lineEdit->text() + "   ";
    str += fDataBackwardEnd_lineEdit->text() + "\n";
    valid = true;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns a t0 parameter for the asymmetry run block.
 *
 * \param present flag indicating if a t0 parameter is set.
 */
QString PGetAsymmetryRunBlockDialog::getT0(bool &present)
{
  QString str = "";

  if (!fT0Forward_lineEdit->text().isEmpty() && !fT0Forward_lineEdit->text().isEmpty()) {
    str  = "t0              ";
    str += fT0Forward_lineEdit->text() + "   ";
    str += fT0Backward_lineEdit->text() + "\n";
    present = true;
  } else {
    present = false;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns a fit range entry for the asymmetry run block. If no fit range has been provided,
 * a fit range [0,10] will be set and the valid flag will be set to false.
 *
 * \param valid flag indicating if a fit range was provided.
 */
QString PGetAsymmetryRunBlockDialog::getFitRange(bool &valid)
{
  QString str = "";

  if (fFitRangeStart_lineEdit->text().isEmpty() || fFitRangeEnd_lineEdit->text().isEmpty()) {
    str += "fit             0.0   10.0\n";
    valid = false;
  } else {
    str += "fit             ";
    str += fFitRangeStart_lineEdit->text() + "   ";
    str += fFitRangeEnd_lineEdit->text() + "\n";
    valid = true;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the packing/binning of the asymmetry run block. If no packing has been provided,
 * a packing of '1' will be set and the present flag will be set to false.
 *
 * \param present flag indicating if a packing parameter was provided.
 */
QString PGetAsymmetryRunBlockDialog::getPacking(bool &present)
{
  QString str = "";

  if (fPacking_lineEdit->text().isEmpty()) {
    present = false;
    str += "packing         1\n";
  } else {
    present = true;
    str += "packing         " + fPacking_lineEdit->text() + "\n\n";
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description of the asymmetry run block.
 */
void PGetAsymmetryRunBlockDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
  } else {
    #ifdef _WIN32GCC
    QMessageBox::information(this, "**INFO**", "If a newer Qt version was available, a help window would be shown!");
    #else
    PHelp *help = new PHelp(fHelpUrl);
    help->show();
    #endif // _WIN32GCC
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
