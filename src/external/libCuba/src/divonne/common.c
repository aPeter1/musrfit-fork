/*
	common.c
		includes most of the modules
		this file is part of Divonne
		last modified 8 Jun 10 th
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


#include "Random.c"
#include "ChiSquare.c"
#include "Rule.c"
#include "Sample.c"
#include "FindMinimum.c"

static bool Explore(This *t, count iregion, cSamples *samples,
  cint depth, cint flags);

static void Split(This *t, count iregion, int depth);

#include "Explore.c"
#include "Split.c"

static inline bool BadDimension(cThis *t, ccount key)
{
  if( t->ndim > NDIM ) return true;
  if( IsSobol(key) ) return
    t->ndim < SOBOL_MINDIM || (t->seed == 0 && t->ndim > SOBOL_MAXDIM);
  if( IsRule(key, t->ndim) ) return t->ndim < 1;
  return t->ndim < KOROBOV_MINDIM || t->ndim > KOROBOV_MAXDIM;
}

static inline bool BadComponent(cThis *t)
{
  if( t->ncomp > NCOMP ) return true;
  return t->ncomp < 1;
}

#include "Integrate.c"

