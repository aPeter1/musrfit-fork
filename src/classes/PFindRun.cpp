/***************************************************************************

  PFindRun.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2022 by Andreas Suter                              *
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
#include <boost/filesystem.hpp>

#include "PFindRun.h"

//----------------------------------------------------------------------------
/**
 * @brief PFindRun::PFindRun
 * @param runNameTemplateList
 */
PFindRun::PFindRun(const PStringVector path, const PRunNameTemplateList runNameTemplateList) :
  fPath(path), fRunNameTemplateList(runNameTemplateList)
{
  // nothing to be done here
}

//----------------------------------------------------------------------------
/**
 * @brief PFindRun::PFindRun
 *
 * @param path
 * @param runNameTemplateList
 * @param instrument
 * @param year
 * @param run
 */
PFindRun::PFindRun(const PStringVector path, const PRunNameTemplateList runNameTemplateList,
                   const TString &instrument, const UInt_t year, const UInt_t run) :
  fPath(path), fRunNameTemplateList(runNameTemplateList), fInstrument(instrument), fYear(year), fRun(run)
{
  // nothing to be done here
}

//----------------------------------------------------------------------------
/**
 * @brief PFindRun::DumpTemplateList
 */
void PFindRun::DumpTemplateList()
{
  std::cout << "debug> instrument: " << fInstrument << std::endl;
  std::cout << "debug> year: " << fYear << std::endl;
  std::cout << "debug> run: " << fRun << std::endl;
  std::cout << "debug> ++++" << std::endl;
  std::cout << "debug> run name template list:" << std::endl;
  for (UInt_t i=0; i<fRunNameTemplateList.size(); i++) {
    std::cout << i << ": instrument: " << fRunNameTemplateList[i].instrument << ", template: " << fRunNameTemplateList[i].runNameTemplate << std::endl;
  }
  std::cout << "debug> ++++" << std::endl;
}

//----------------------------------------------------------------------------
/**
 * @brief PFindRun::CreatePathName
 * @param path
 * @param runNameTemplate
 * @return
 */
TString PFindRun::CreatePathName(const TString path, const TString runNameTemplate)
{
  TString pathName{""};
  TString runName = runNameTemplate;
  TString yearStr = TString::Format("%d", fYear);
  TString yyStr;
  if (fYear < 2000)
    yyStr = TString::Format("%02d", fYear-1900);
  else
    yyStr = TString::Format("%02d", fYear-2000);
  runName.ReplaceAll("%yyyy%", yearStr);
  runName.ReplaceAll("%yy%", yyStr);

  // run handling slightly more complicated, since various number of digits possible
  Int_t idx=-1;
  TString rr{""};
  for (Int_t i=2; i<10; i++) {
    rr ="%";
    for (Int_t j=0; j<i; j++)
      rr += "r";
    rr += "%";
    if (runName.Index(rr) != -1) {
      idx = i;
      break;
    }
  }
  TString format = TString::Format("%%0%dd", idx);
  TString runStr = TString::Format(format, fRun);

  if (idx != -1)
    runName.ReplaceAll(rr, runStr);

  pathName = path;
  pathName += "/";
  pathName += runName;

  return pathName;
}

//----------------------------------------------------------------------------
/**
 * @brief PFindRun::FoundPathName
 * @return
 */
Bool_t PFindRun::FoundPathName()
{
  // find instrument name in path list
  TString pathName{""};
  for (Int_t i=0; i<fPath.size(); i++) {
    if (fPath[i].Index(fInstrument) != -1) {
      for (Int_t j=0; j<fRunNameTemplateList.size(); j++) {
        if (fRunNameTemplateList[j].instrument == fInstrument) {
          pathName = CreatePathName(fPath[i], fRunNameTemplateList[j].runNameTemplate);
          if (boost::filesystem::exists(pathName.Data())) {
            fPathName = pathName;
            return true;
          }
        }
      }
    }
  }
  return false;
}
