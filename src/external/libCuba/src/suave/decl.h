/*
	decl.h
		Type declarations
		this file is part of Suave
		last modified 30 Aug 07 th
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

#define MINSAMPLES 10

#define NBINS 64

typedef unsigned char bin_t;
/* Note: bin_t must be wide enough to hold the numbers 0..NBINS */

typedef const bin_t cbin_t;

typedef real Grid[NBINS];

typedef const Grid cGrid;

typedef struct {
  real avg, err, sigsq, chisq;
} Result;

typedef const Result cResult;


typedef struct {
  real lower, upper, mid;
  Grid grid;
} Bounds;

typedef const Bounds cBounds;


#define TYPEDEFREGION \
  typedef struct region { \
    struct region *next; \
    count div, df; \
    number n; \
    Result result[NCOMP]; \
    Bounds bounds[NDIM]; \
    real fluct[NCOMP][NDIM][2]; \
    real w[]; \
  } Region


typedef void (*Integrand)(ccount *, creal *, ccount *, real *, creal *);

