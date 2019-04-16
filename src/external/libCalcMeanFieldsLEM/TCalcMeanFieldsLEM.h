/***************************************************************************

  TCalcMeanFieldsLEM.h

  Author: Bastian M. Wojek

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *                                                                         *
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

#ifndef _TCalcMeanFieldsLEM_H_
#define _TCalcMeanFieldsLEM_H_

#include "TLondon1D.h"

class TMeanFieldsForScHalfSpace : public PUserFcnBase {

public:
  // default constructor
  TMeanFieldsForScHalfSpace();
  ~TMeanFieldsForScHalfSpace() {delete fImpProfile; fImpProfile = 0;}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;
  double CalcMeanB (double, const TLondon1D_HS&) const;

private:
  TTrimSPData *fImpProfile;

  ClassDef(TMeanFieldsForScHalfSpace,1)
};

class TMeanFieldsForScSingleLayer : public PUserFcnBase {

public:
  // default constructor
  TMeanFieldsForScSingleLayer();
  ~TMeanFieldsForScSingleLayer() {delete fImpProfile; fImpProfile = 0;}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;
  double CalcMeanB (double, const std::vector<double>&, const std::vector<double>&, const TLondon1D_1L&) const;

private:
  TTrimSPData *fImpProfile;

  ClassDef(TMeanFieldsForScSingleLayer,1)
};

class TMeanFieldsForScBilayer : public PUserFcnBase {

public:
  // default constructor
  TMeanFieldsForScBilayer();
  ~TMeanFieldsForScBilayer() {delete fImpProfile; fImpProfile = 0;}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;
  double CalcMeanB (double, const std::vector<double>&, const std::vector<double>&, const TLondon1D_2L&, double) const;

private:
  TTrimSPData *fImpProfile;

  ClassDef(TMeanFieldsForScBilayer,1)
};

class TMeanFieldsForScTrilayer : public PUserFcnBase {

public:
  // default constructor
  TMeanFieldsForScTrilayer();
  ~TMeanFieldsForScTrilayer() {delete fImpProfile; fImpProfile = 0;}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;
  double CalcMeanB (double, const std::vector<double>&, const std::vector<double>&, const TLondon1D_3L&) const;

private:
  TTrimSPData *fImpProfile;

  ClassDef(TMeanFieldsForScTrilayer,1)
};

class TMeanFieldsForScTrilayerWithInsulator : public PUserFcnBase {

public:
  // default constructor
  TMeanFieldsForScTrilayerWithInsulator();
  ~TMeanFieldsForScTrilayerWithInsulator() {delete fImpProfile; fImpProfile = 0;}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;
  double CalcMeanB (double, const std::vector<double>&, const std::vector<double>&, const TLondon1D_3LwInsulator&) const;

private:
  TTrimSPData *fImpProfile;

  ClassDef(TMeanFieldsForScTrilayerWithInsulator,1)
};

#endif /* _TCalcMeanFieldsLEM_H_ */
