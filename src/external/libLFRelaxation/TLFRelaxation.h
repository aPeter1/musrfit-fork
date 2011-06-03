/***************************************************************************

  TLFRelaxation.h

  Author: Bastian M. Wojek

  $Id$

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

#ifndef _TLFRelaxation_H_
#define _TLFRelaxation_H_

#include<vector>
#include<cstdio>
#include<cmath>
#include<map>

using namespace std;

#include "PUserFcnBase.h"
#include "fftw3.h"
#include "BMWIntegrator.h"

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for a static Gaussian depolarization in a longitudinal field using direct integration through GSL
 * See also: R. S. Hayano, Y. J. Uemura, J. Imazato, N. Nishida, T. Yamazaki, and R. Kubo,
 * \htmlonly Phys. Rev. B <b>20</b>, 850&#150;859 (1979), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.20.850">10.1103/PhysRevB.20.850</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{20}, 850--859 (1979), \texttt{http://dx.doi.org/10.1103/PhysRevB.20.850}
 * \endlatexonly
 */
class TLFStatGssKT : public PUserFcnBase {

public:
  TLFStatGssKT();
  ~TLFStatGssKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TIntSinGss *fIntSinGss;                  ///< integrator

  ClassDef(TLFStatGssKT,3)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for a static exponential depolarization in a longitudinal field using direct integration through GSL
 * See also: Y. J. Uemura, T. Yamazaki, D. R. Harshman, M. Senba, and E. J. Ansaldo,
 * \htmlonly Phys. Rev. B <b>31</b>, 546&#150;563 (1985), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.31.546">10.1103/PhysRevB.31.546</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{31}, 546--563 (1985), \texttt{http://dx.doi.org/10.1103/PhysRevB.31.546}
 * \endlatexonly
 */
class TLFStatExpKT : public PUserFcnBase {

public:
  TLFStatExpKT();
  ~TLFStatExpKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TIntBesselJ0Exp *fIntBesselJ0Exp;         ///< integrator

  ClassDef(TLFStatExpKT,3)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for a dynamic Gaussian depolarization in a longitudinal field calculated using Laplace transforms
 * See also: R. S. Hayano, Y. J. Uemura, J. Imazato, N. Nishida, T. Yamazaki, and R. Kubo,
 * \htmlonly Phys. Rev. B <b>20</b>, 850&#150;859 (1979), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.20.850">10.1103/PhysRevB.20.850</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{20}, 850--859 (1979), \texttt{http://dx.doi.org/10.1103/PhysRevB.20.850}
 * \endlatexonly
 *
 * For details regarding the numerical Laplace transform, see e.g.
 * P. Moreno and A. Ramirez,
 * \htmlonly IEEE Trans. Power Delivery <b>23</b>, 2599&#150;2609 (2008), doi:<a href="http://dx.doi.org/10.1109/TPWRD.2008.923404">10.1109/TPWRD.2008.923404</a>
 * \endhtmlonly
 * \latexonly IEEE Trans. Power Delivery \textbf{23}, 2599--2609 (2008), \texttt{http://dx.doi.org/10.1109/TPWRD.2008.923404}
 * \endlatexonly
 */
class TLFDynGssKT : public PUserFcnBase {

public:
  TLFDynGssKT();
  ~TLFDynGssKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;        ///< parameters of the function [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), &#963; (&#956;s<sup>-1</sup>), &#957; (MHz)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $\sigma~(\mu\mathrm{s}^{-1})$, $\nu~(\mathrm{MHz})$ \endlatexonly]
  mutable bool fCalcNeeded;           ///< flag indicating if the expensive Laplace transform has to be done (e.g. after parameters have changed)
  mutable bool fFirstCall;            ///< flag indicating if the function is evaluated for the first time
  bool fUseWisdom;                    ///< flag showing if a FFTW3-wisdom file is used
  string fWisdom;                     ///< path to the wisdom file
  unsigned int fNSteps;               ///< length of the Laplace transform
  double fDt;                         ///< time resolution
  double fDw;                         ///< frequency resolution
  double fC;                          ///< coefficient depending on the length of the Laplace transform and time resolution
  fftwf_plan fFFTplanFORW;            ///< FFTW3 plan: time domain \htmlonly &#8594; \endhtmlonly \latexonly $\rightarrow$ \endlatexonly frequency domain
  fftwf_plan fFFTplanBACK;            ///< FFTW3 plan: time domain \htmlonly &#8592; \endhtmlonly \latexonly $\leftarrow$ \endlatexonly frequency domain
  float *fFFTtime;                    ///< time-domain array
  fftwf_complex *fFFTfreq;            ///< frequency-domain array
  mutable unsigned int fCounter;      ///< counter determining how many Laplace transforms are done (mainly for debugging purposes)

  ClassDef(TLFDynGssKT,2)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for a dynamic exponential depolarization in a longitudinal field calculated using Laplace transforms
 * See also: Y. J. Uemura, T. Yamazaki, D. R. Harshman, M. Senba, and E. J. Ansaldo,
 * \htmlonly Phys. Rev. B <b>31</b>, 546&#150;563 (1985), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.31.546">10.1103/PhysRevB.31.546</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{31}, 546--563 (1985), \texttt{http://dx.doi.org/10.1103/PhysRevB.31.546}
 * \endlatexonly
 *
 * and: R. S. Hayano, Y. J. Uemura, J. Imazato, N. Nishida, T. Yamazaki, and R. Kubo,
 * \htmlonly Phys. Rev. B <b>20</b>, 850&#150;859 (1979), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.20.850">10.1103/PhysRevB.20.850</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{20}, 850--859 (1979), \texttt{http://dx.doi.org/10.1103/PhysRevB.20.850}
 * \endlatexonly
 *
 * For details regarding the numerical Laplace transform, see e.g.
 * P. Moreno and A. Ramirez,
 * \htmlonly IEEE Trans. Power Delivery <b>23</b>, 2599&#150;2609 (2008), doi:<a href="http://dx.doi.org/10.1109/TPWRD.2008.923404">10.1109/TPWRD.2008.923404</a>
 * \endhtmlonly
 * \latexonly IEEE Trans. Power Delivery \textbf{23}, 2599--2609 (2008), \texttt{http://dx.doi.org/10.1109/TPWRD.2008.923404}
 * \endlatexonly
 */
class TLFDynExpKT : public PUserFcnBase {

public:
  TLFDynExpKT();
  ~TLFDynExpKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;         ///< parameters of the function [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), <i>a</i> (&#956;s<sup>-1</sup>), &#957; (MHz)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $a~(\mu\mathrm{s}^{-1})$, $\nu~(\mathrm{MHz})$ \endlatexonly]
  mutable bool fCalcNeeded;            ///< flag indicating if the expensive Laplace transform has to be done (e.g. after parameters have changed)
  mutable bool fFirstCall;             ///< flag indicating if the function is evaluated for the first time
  bool fUseWisdom;                     ///< flag showing if a FFTW3-wisdom file is used
  string fWisdom;                      ///< path to the wisdom file
  unsigned int fNSteps;                ///< length of the Laplace transform
  double fDt;                          ///< time resolution
  double fDw;                          ///< frequency resolution
  double fC;                           ///< coefficient depending on the length of the Laplace transform and time resolution
  fftwf_plan fFFTplanFORW;             ///< FFTW3 plan: time domain \htmlonly &#8594; \endhtmlonly \latexonly $\rightarrow$ \endlatexonly frequency domain
  fftwf_plan fFFTplanBACK;             ///< FFTW3 plan: time domain \htmlonly &#8592; \endhtmlonly \latexonly $\leftarrow$ \endlatexonly frequency domain
  float *fFFTtime;                     ///< time-domain array
  fftwf_complex *fFFTfreq;             ///< frequency-domain array
  mutable unsigned int fCounter;       ///< counter determining how many Laplace transforms are done (mainly for debugging purposes)
  mutable double fL1;                  ///< coefficient used for the high-field and high-hopping-rate approximation
  mutable double fL2;                  ///< coefficient used for the high-field and high-hopping-rate approximation

  ClassDef(TLFDynExpKT,2)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for a dynamic depolarization in a spin glass in a longitudinal field calculated using Laplace transforms
 * See also: Y. J. Uemura, T. Yamazaki, D. R. Harshman, M. Senba, and E. J. Ansaldo,
 * \htmlonly Phys. Rev. B <b>31</b>, 546&#150;563 (1985), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.31.546">10.1103/PhysRevB.31.546</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{31}, 546--563 (1985), \texttt{http://dx.doi.org/10.1103/PhysRevB.31.546}
 * \endlatexonly
 *
 * and: R. S. Hayano, Y. J. Uemura, J. Imazato, N. Nishida, T. Yamazaki, and R. Kubo,
 * \htmlonly Phys. Rev. B <b>20</b>, 850&#150;859 (1979), doi:<a href="http://dx.doi.org/10.1103/PhysRevB.20.850">10.1103/PhysRevB.20.850</a>
 * \endhtmlonly
 * \latexonly Phys. Rev. B \textbf{20}, 850--859 (1979), \texttt{http://dx.doi.org/10.1103/PhysRevB.20.850}
 * \endlatexonly
 *
 * For details regarding the numerical Laplace transform, see e.g.
 * P. Moreno and A. Ramirez,
 * \htmlonly IEEE Trans. Power Delivery <b>23</b>, 2599&#150;2609 (2008), doi:<a href="http://dx.doi.org/10.1109/TPWRD.2008.923404">10.1109/TPWRD.2008.923404</a>
 * \endhtmlonly
 * \latexonly IEEE Trans. Power Delivery \textbf{23}, 2599--2609 (2008), \texttt{http://dx.doi.org/10.1109/TPWRD.2008.923404}
 * \endlatexonly
 */
class TLFDynSG : public PUserFcnBase {

public:
  TLFDynSG();
  ~TLFDynSG();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;               ///< parameters of the dynamic Gaussian depolarization function [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), &#963; (&#956;s<sup>-1</sup>), &#957; (MHz)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $\sigma~(\mu\mathrm{s}^{-1})$, $\nu~(\mathrm{MHz})$ \endlatexonly]
  vector<TLFDynGssKT*> fLFDynGssIntegral;    ///< vector of dynamic Gaussian depolarization functions for a subset of static widths

  ClassDef(TLFDynSG,1)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for a dynamic depolarization in a spin glass in a longitudinal field
 * See also: R. De Renzi and S. Fanesi
 * \htmlonly Physica B <b>289&#150;290</b>, 209&#150;212 (2000), doi:<a href="http://dx.doi.org/10.1016/S0921-4526(00)00368-9">10.1016/S0921-4526(00)00368-9</a>
 * \endhtmlonly
 * \latexonly Physica B \textbf{289--290}, 209--212 (2000), \texttt{http://dx.doi.org/10.1016/S0921-4526(00)00368-9}
 * \endlatexonly
 */
class TLFSGInterpolation : public PUserFcnBase {

public:
  TLFSGInterpolation();
  ~TLFSGInterpolation();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TIntSGInterpolation *fIntegral;           ///< integrator

  ClassDef(TLFSGInterpolation,3)
};


#endif //_TLFRelaxation_H_
