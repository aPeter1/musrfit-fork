/*
	common.c
		includes most of the modules
		this file is part of Divonne
		last modified 5 May 09 th
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


static bool Explore(count iregion, cSamples *samples, cint depth, cint flags);

static void Split(count iregion, int depth);

#include "Random.c"
#include "ChiSquare.c"
#include "Rule.c"
#include "Sample.c"
#include "FindMinimum.c"
#include "Explore.c"
#include "Split.c"
#include "Integrate.c"


static inline bool BadDimension(ccount ndim, cint flags, ccount key)
{
#if NDIM > 0
  if( ndim > NDIM ) return true;
#endif
  if( IsSobol(key) ) return
    ndim < SOBOL_MINDIM || (!PSEUDORNG && ndim > SOBOL_MAXDIM);
  if( IsRule(key, ndim) ) return ndim < 1;
  return ndim < KOROBOV_MINDIM || ndim > KOROBOV_MAXDIM;
}


static inline bool BadComponent(cint ncomp)
{
#if NCOMP > 0
  if( ncomp > NCOMP ) return true;
#endif
  return ncomp < 1;
}

