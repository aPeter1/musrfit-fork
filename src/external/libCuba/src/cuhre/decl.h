/*
	decl.h
		Type declarations
		this file is part of Cuhre
		last modified 8 Apr 09 th
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

#include "stddecl.h"


typedef struct {
  real avg, err;
  count bisectdim;
} Result;

typedef const Result cResult;


typedef struct {
  real avg, err, lastavg, lasterr;
  real weightsum, avgsum;
  real guess, chisum, chisqsum, chisq;
} Totals;

typedef const Totals cTotals;


typedef struct {
  real lower, upper;
} Bounds;

typedef const Bounds cBounds;


typedef struct {
  real *x, *f;
  void *first, *last;
  real errcoeff[3];
  count n;
} Rule;

typedef const Rule cRule;


#define TYPEDEFREGION \
  typedef struct region { \
    count div; \
    Result result[NCOMP]; \
    Bounds bounds[NDIM]; \
  } Region


typedef void (*Integrand)(ccount *, creal *, ccount *, real *);

