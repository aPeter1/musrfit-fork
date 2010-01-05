/*
	Suave.c
		Subregion-adaptive Vegas Monte-Carlo integration
		by Thomas Hahn
		last modified 30 Aug 07 th
*/

/***************************************************************************
 *   Copyright (C) 2004-2009 by Thomas Hahn                                *
 *   hahn@feynarts.de                                                      *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "util.c"

#define Print(s) puts(s); fflush(stdout)

static Integrand integrand_;

/*********************************************************************/

static inline void DoSample(number n, creal *w, creal *x, real *f)
{
  neval_ += n;
  while( n-- ) {
    integrand_(&ndim_, x, &ncomp_, f, w++);
    x += ndim_;
    f += ncomp_;
  }
}

/*********************************************************************/

#include "common.c"

Extern void EXPORT(Suave)(ccount ndim, ccount ncomp,
  Integrand integrand,
  creal epsrel, creal epsabs,
  cint flags, cnumber mineval, cnumber maxeval,
  cnumber nnew, creal flatness,
  count *pnregions, number *pneval, int *pfail,
  real *integral, real *error, real *prob)
{
  ndim_ = ndim;
  ncomp_ = ncomp;

  if( BadComponent(ncomp) || BadDimension(ndim, flags) ) *pfail = -1;
  else {
    neval_ = 0;
    integrand_ = integrand;

    *pfail = Integrate(epsrel, Max(epsabs, NOTZERO),
      flags, mineval, maxeval, nnew, flatness,
      integral, error, prob);
    *pnregions = nregions_;
    *pneval = neval_;
  }
}

/*********************************************************************/

Extern void EXPORT(suave)(ccount *pndim, ccount *pncomp,
  Integrand integrand,
  creal *pepsrel, creal *pepsabs,
  cint *pflags, cnumber *pmineval, cnumber *pmaxeval,
  cnumber *pnnew, creal *pflatness,
  count *pnregions, number *pneval, int *pfail,
  real *integral, real *error, real *prob)
{
  EXPORT(Suave)(*pndim, *pncomp,
    integrand,
    *pepsrel, *pepsabs,
    *pflags, *pmineval, *pmaxeval,
    *pnnew, *pflatness,
    pnregions, pneval, pfail,
    integral, error, prob);
}

