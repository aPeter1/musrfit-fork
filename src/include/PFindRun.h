/***************************************************************************

  PFindRun.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#ifndef _PFINDRUN_H_
#define _PFINDRUN_H_

#include "PMusr.h"

class PFindRun {
  public:
    PFindRun(const PStringVector path, const PRunNameTemplateList runNameTemplateList);
    PFindRun(const PStringVector path, const PRunNameTemplateList runNameTemplateList,
             const TString &instrument, const UInt_t year, const UInt_t run);

    Bool_t  FoundPathName();
    TString GetPathName() { return fPathName; }
    void DumpTemplateList();

  private:
    const PStringVector fPath;
    const PRunNameTemplateList fRunNameTemplateList;
    TString fInstrument{""};
    Int_t fYear{-1};
    Int_t fRun{-1};
    TString fPathName{""};

    TString CreatePathName(const TString path, const TString runNameTemplate);
};

#endif // _PFINDRUN_H_
