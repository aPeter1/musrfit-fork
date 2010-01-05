/*
	util.c
		Utility functions
		this file is part of Suave
		last modified 9 Feb 05 th
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

#include "decl.h"

static count ndim_, ncomp_, nregions_;
static number neval_;


#define RegionAlloc(p, n, nnew) \
  MemAlloc(p, sizeof(Region) + \
              (n)*(ndim_ + ncomp_ + 1)*sizeof(real) + \
              (nnew)*ndim_*sizeof(bin_t))


#ifdef DEBUG
#include "debug.c"
#endif

