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
  int firstRun;                ///< first run number of a sequence of runs (usage 2 of msr2data)
  int lastRun;                 ///< last run number of a sequence of runs (usage 2 of msr2data)
  QString runList;             ///< list of run numbers (usage 3 of msr2data)
  QString runListFileName;     ///< run list filename (usage 4 of msr2data)
  QString msrFileExtension;    ///< msr filename extension, e.g. '0100_h13.msr' -> '_h13'
  int templateRunNo;           ///< fit template run number
  QString dbOutputFileName;    ///< output file name for the generated (trumf-like) db-file.
  bool writeDbHeader;          ///< flag indicating if a db header shall be generated (== !noheader in msr2data)
  bool summaryFilePresent;     ///< flag indicating if a LEM summary file is present (== !nosummary in msr2data)
  bool keepMinuit2Output;      ///< flag indicating if the minuit2 output shall be kept ('-k' in msr2data)
  bool writeColumnData;        ///< flag indicating if instead of a db-file a column data ascii file shall be written ('data' in msr2data)
  bool recreateDbFile;         ///< flag: true = recreate db-file, false = append to present db-file
  bool chainFit;               ///< flag: true = chain fit, i.e. the template for a fit is the preceeding run. false = the template is always the source for the new msr-file
  bool openFilesAfterFitting;  ///< flag: true = open msr-file after fit in musredit. false = do not open msr-file after fit.
  bool titleFromDataFile;      ///< flag indicating if the title for the msr-file shall be extracted from the data-file ('-t' in msr2data)
  bool createMsrFileOnly;      ///< flag: true = just create the msr-files without any fitting ('msr-<template>' in msr2data)
  bool fitOnly;                ///< flag: true = just perform the fits wihtout generating any msr-files ('fit' in msr2data).
  bool global;                 ///< flag: true = 'global' option
} PMsr2DataParam;

//-------------------------------------------------------------------------------------------------
/**
 * This structure is used to handle find (and replace) within <code>musredit</code> properly.
 */
typedef struct {
  QString findText;     ///< text to be found
  QString replaceText;  ///< replacement string
  bool caseSensitive;   ///< true = case sensitive
  bool wholeWordsOnly;  ///< true = look for whole words only
  bool fromCursor;      ///< true = start the find/replace form the cursor position only
  bool findBackwards;   ///< true = reversed search
  bool selectedText;    ///< true = handle only the selected text
  bool promptOnReplace; ///< true = request on OK from the user before performing the replace action
} PFindReplaceData;

#endif // _MUSREDIT_H_
