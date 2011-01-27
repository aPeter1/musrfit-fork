/*
	Cuhre.c
		Adaptive integration using cubature rules
		by Thomas Hahn
		last modified 16 Jun 10 th
*/

/***************************************************************************
 *   Copyright (C) 2004-2010 by Thomas Hahn                                *
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


#include "decl.h"

#define Print(s) puts(s); fflush(stdout)

/*********************************************************************/

static inline void DoSample(This *t, count n, creal *x, real *f)
{
  t->neval += n;
  while( n-- ) {
    if( t->integrand(&t->ndim, x, &t->ncomp, f, t->userdata) == ABORT )
      longjmp(t->abort, 1);
    x += t->ndim;
    f += t->ncomp;
  }
}

/*********************************************************************/

#include "common.c"

Extern void EXPORT(Cuhre)(ccount ndim, ccount ncomp,
  Integrand integrand, void *userdata,
  creal epsrel, creal epsabs,
  cint flags, cnumber mineval, cnumber maxeval,
  ccount key,
  count *pnregions, number *pneval, int *pfail,
  real *integral, real *error, real *prob)
{
  This t;
  t.ndim = ndim;
  t.ncomp = ncomp;
  t.integrand = integrand;
  t.userdata = userdata;
  t.epsrel = epsrel;
  t.epsabs = epsabs;
  t.flags = flags;
  t.mineval = mineval;
  t.maxeval = maxeval;
  t.key = key;
  t.nregions = 0;
  t.neval = 0;
 
  *pfail = Integrate(&t, integral, error, prob);
  *pnregions = t.nregions;
  *pneval = t.neval;
}

/*********************************************************************/

Extern void EXPORT(cuhre)(ccount *pndim, ccount *pncomp,
  Integrand integrand, void *userdata,
  creal *pepsrel, creal *pepsabs,
  cint *pflags, cnumber *pmineval, cnumber *pmaxeval,
  ccount *pkey,
  count *pnregions, number *pneval, int *pfail,
  real *integral, real *error, real *prob)
{
  This t;
  t.ndim = *pndim;
  t.ncomp = *pncomp;
  t.integrand = integrand;
  t.userdata = userdata;
  t.epsrel = *pepsrel;
  t.epsabs = *pepsabs;
  t.flags = *pflags;
  t.mineval = *pmineval;
  t.maxeval = *pmaxeval;
  t.key = *pkey;
  t.nregions = 0;
  t.neval = 0;
 
  *pfail = Integrate(&t, integral, error, prob);
  *pnregions = t.nregions;
  *pneval = t.neval;
}

