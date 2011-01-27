/*
	common.c
		includes most of the modules
		this file is part of Suave
		last modified 2 Jun 10 th
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


#define RegionAlloc(p, n, nnew) MemAlloc(p, \
  sizeof(Region) + \
  (n)*(t->ndim + t->ncomp + 1)*sizeof(real) + \
  (nnew)*t->ndim*sizeof(bin_t))

static inline bool BadDimension(cThis *t)
{
  if( t->ndim > NDIM ) return true;
  return t->ndim < SOBOL_MINDIM ||
    (t->seed == 0 && t->ndim > SOBOL_MAXDIM);
}

static inline bool BadComponent(cThis *t)
{
  if( t->ncomp > NCOMP ) return true;
  return t->ncomp < 1;
}

#include "Random.c"
#include "ChiSquare.c"
#include "Grid.c"
#include "Sample.c"
#include "Fluct.c"
#include "Integrate.c"

