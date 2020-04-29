/***************************************************************************

  BMWStartupHandler.h

  Author: Bastian M. Wojek

  based upon:
  PStartupHandler.h
  by Andreas Suter

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter, Bastian M. Wojek            *
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

#ifndef _BMWSTARTUPHANDLER_H_
#define _BMWSTARTUPHANDLER_H_

#include <TList.h>
#include <TQObject.h>
#include <string>
#include <vector>
#include <map>

/**
 * <p>Handles the XML musrfit startup file (BMW_startup.xml) where default settings for some plugin libraries are stored:
 * - TRIM.SP data file path and energies
 * - time and field resolutions for Fourier transforms
 * - paths to FFTW3 wisdom files (double and float)
 * - number of steps for one-dimensional theory functions (where needed)
 * - number of steps for two-dimensional grids when calculating spatial field distributions in vortex lattices
 * - time resolutions and lengths of Laplace transforms used in the calculation of LF-relaxation functions
 * - flag for debugging the information contained in the startup file
 */
class BMWStartupHandler : public TQObject {
  public:
    BMWStartupHandler();
    virtual ~BMWStartupHandler();

    virtual void OnStartDocument(); // SLOT
    virtual void OnEndDocument(); // SLOT
    virtual void OnStartElement(const char*, const TList*); // SLOT
    virtual void OnEndElement(const char*); // SLOT
    virtual void OnCharacters(const char*); // SLOT
    virtual void OnComment(const char*); // SLOT
    virtual void OnWarning(const char*); // SLOT
    virtual void OnError(const char*); // SLOT
    virtual void OnFatalError(const char*); // SLOT
    virtual void OnCdataBlock(const char*, int); // SLOT

    virtual void CheckLists();

    virtual const std::string GetDataPath() const { return fDataPath; } ///< returns the path to TRIM.SP files
    virtual std::map<double, std::string> GetEnergies() const { return fEnergies; } ///< returns energies and file labels of available TRIM.SP files
    virtual const double GetDeltat() const { return fDeltat; } ///< returns the time resolution of P(t) when using Fourier transforms
    virtual const double GetDeltaB() const { return fDeltaB; } ///< returns the field resolution of p(B) when using Fourier transforms
    virtual const std::string GetWisdomFile() const { return fWisdomFile; } ///< returns the path to the FFTW3 double-wisdom file
    virtual const std::string GetWisdomFileFloat() const { return fWisdomFileFloat; } ///< returns the path to the FFTW3 float-wisdom file
    virtual const unsigned int GetNSteps() const { return fNSteps; } ///< returns the number of steps in one-dimensional theory functions
    virtual const unsigned int GetGridSteps() const { return fGridSteps; } ///< returns the number of steps in each direction when calculating two-dimensional spatial field distributions
    virtual const double GetDeltatLF() const { return fDeltatLF; } ///< returns the time resolution of P(t) when using Laplace transforms for the calculation of LF-relaxation functions
    virtual const unsigned int GetNStepsLF() const { return fNStepsLF; } ///< returns the length of the Laplace transforms for the calculation of LF-relaxation functions
    virtual const bool GetDebug() const { return fDebug; } ///< true = debug the xml-entries

  private:
    enum EKeyWords {eEmpty, eComment, eDebug, eLEM, eVortex, eLF, eDataPath, eEnergyLabel, \
                    eEnergy, eEnergyList, eDeltat, eDeltaB, eWisdomFile, eWisdomFileFloat, \
                    eNSteps, eGridSteps, eDeltatLF, eNStepsLF};

    EKeyWords       fKey; ///< xml filter key

    bool            fDebug;           ///< debug flag
    bool            fLEM;             ///< low-energy muSR flag
    bool            fVortex;          ///< vortex-lattice flag
    bool            fLF;              ///< longitudinal-field flag
    std::string     fDataPath;        ///< path to TRIM.SP files
    std::vector<std::string> fEnergyLabelList; ///< file labels of the TRIM.SP files
    std::vector<double> fEnergyList;  ///< muon implantation energies of the TRIM.SP files
    std::map<double, std::string> fEnergies;   ///< muon implantation energies and file labels of the TRIM.SP files
    double          fDeltat;          ///< time resolution of P(t) when using Fourier transforms
    double          fDeltaB;          ///< field resolution of p(B) when using Fourier transforms
    std::string     fWisdomFile;      ///< FFTW3 double-wisdom file
    std::string     fWisdomFileFloat; ///< FFTW3 float-wisdom file
    unsigned int    fNSteps;          ///< number of steps in one-dimensional theory functions
    unsigned int    fGridSteps;       ///< number of steps in each direction when calculating two-dimensional spatial field distributions
    double          fDeltatLF;        ///< time resolution of P(t) when using Laplace transforms for the calculation of LF-relaxation functions
    unsigned int    fNStepsLF;        ///< length of the Laplace transforms for the calculation of LF-relaxation functions

  ClassDef(BMWStartupHandler, 1)
};

#endif // _BMWSTARTUPHANDLER_H_

