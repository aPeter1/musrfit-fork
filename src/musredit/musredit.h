/****************************************************************************

  musredit.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Andreas Suter                                   *
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

#ifndef _MUSREDIT_H_
#define _MUSREDIT_H_

#include <QString>

//-------------------------------------------------------------------------------------------------
/**
 * <p> This structure is used in conjunction to <code>msr2data</code>. It stores the necessary
 * parameters to handle <code>msr2data</code>. For a detailed description of the meaning of these
 * parameters see <code>msr2data --help</code> and the online documentation.
 */
typedef struct {
  int firstRun;
  int lastRun;
  QString runList;
  QString runListFileName;
  QString msrFileExtension;
  int templateRunNo;
  QString dbOutputFileName;
  bool writeDbHeader;
  bool summaryFilePresent;
  bool keepMinuit2Output;
  bool writeColumnData;
  bool recreateDbFile;
  bool chainFit;
  bool openFilesAfterFitting;
  bool titleFromDataFile;
  bool createMsrFileOnly;
  bool fitOnly;
} PMsr2DataParam;

//-------------------------------------------------------------------------------------------------
/**
 * This structure is used to handle find (and replace) within <code>musredit</code> properly.
 */
typedef struct {
  QString findText;
  QString replaceText;
  bool caseSensitive;
  bool wholeWordsOnly;
  bool fromCursor;
  bool findBackwards;
  bool selectedText;
  bool promptOnReplace;
} PFindReplaceData;

#endif // _MUSREDIT_H_
