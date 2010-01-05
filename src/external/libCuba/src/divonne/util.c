/*
	util.c
		Utility functions
		this file is part of Divonne
		last modified 9 Apr 09 th
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

static count ndim_, ncomp_, selectedcomp_, nregions_;
static number neval_, neval_opt_, neval_cut_;
static int sign_, phase_;

static Bounds border_;

static Samples samples_[3];
static Rule rule7_, rule9_, rule11_, rule13_;
static real *xgiven_, *fgiven_, *xextra_, *fextra_;
static count ldxgiven_;
static number ngiven_, nextra_;

static Totals *totals_;

static void *voidregion_;
#define region_ ((Region *)voidregion_)
static count size_;

#ifdef DEBUG
#include "debug.c"
#endif

