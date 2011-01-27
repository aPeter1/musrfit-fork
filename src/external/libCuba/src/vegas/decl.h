/*
	decl.h
		Type declarations
		this file is part of Vegas
		last modified 13 Sep 10 th
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


#include "stddecl.h"

#define MAXGRIDS 10

#define NBINS 128

typedef unsigned char bin_t;
/* Note: bin_t must be wide enough to hold the numbers 0..NBINS */

typedef const bin_t cbin_t;

typedef real Grid[NBINS];

typedef struct {
  real sum, sqsum;
  real weightsum, avgsum;
  real chisum, chisqsum, guess;
  real avg, err, chisq;
} Cumulants;

typedef const Cumulants cCumulants;

typedef int (*Integrand)(ccount *, creal *, ccount *, real *,
  void *, creal *, cint *);

typedef struct _this {
  count ndim, ncomp;
#ifndef MLVERSION
  Integrand integrand;
  void *userdata;
#endif
  real epsrel, epsabs;
  int flags, seed;
  number mineval, maxeval;
  number nstart, nincrease, nbatch;
  int gridno;
  cchar *statefile;
  number neval;
  RNGState rng;
  jmp_buf abort;
} This;

typedef const This cThis;

static Grid *gridptr_[MAXGRIDS];
static count griddim_[MAXGRIDS];

