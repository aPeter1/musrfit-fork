/*
	decl.h
		Type declarations
		this file is part of Divonne
		last modified 25 May 09 th
*/

/***************************************************************************
 *   Copyright (C) 2004-2009 by Thomas Hahn                                *
 *   hahn@feynarts.de                                                      *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Library General Public License for more details.                      *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; if not, write to the Free            *
 *   Foundation, Inc.,                                                     *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/


#include "stddecl.h"

#define EXTRAPOLATE_EPS (.25*border_.lower)
/*#define EXTRAPOLATE_EPS 0x1p-26*/


typedef struct {
  real lower, upper;
} Bounds;

typedef const Bounds cBounds;


typedef struct {
  real avg, spreadsq;
  real spread, secondspread;
  real nneed, maxerrsq, mindevsq;
  int iregion;
} Totals;


typedef struct {
  void *first, *last;
  real errcoeff[3];
  count n;
} Rule;

typedef const Rule cRule;


typedef struct samples {
  real weight;
  real *x, *f, *avg, *err;
  void (*sampler)(const struct samples *, cBounds *, creal);
  cRule *rule;
  count coeff;
  number n, neff;
} Samples;

typedef const Samples cSamples;


#define TYPEDEFREGION \
  typedef struct { \
    real avg, err, spread, chisq; \
    real fmin, fmax; \
    real xmin[NDIM], xmax[NDIM]; \
  } Result; \
  typedef const Result cResult; \
  typedef struct region { \
    count cutcomp, depth, xmajor; \
    real fmajor, fminor, vol; \
    Bounds bounds[NDIM]; \
    Result result[NCOMP]; \
  } Region

#define CHUNKSIZE 4096


typedef void (*Integrand)(ccount *, creal *, ccount *, real *, cint *);

typedef void (*PeakFinder)(ccount *, cBounds *, number *, real *);

