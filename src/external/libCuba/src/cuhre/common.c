/*
	common.c
		includes most of the modules
		this file is part of Cuhre
		last modified 7 Jun 10 th
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


#include "ChiSquare.c"
#include "Rule.c"

static inline bool BadDimension(cThis *t)
{
  if( t->ndim > NDIM ) return true;
  return t->ndim < 2;
}

static inline bool BadComponent(cThis *t)
{
  if( t->ncomp > NCOMP ) return true;
  return t->ncomp < 1;
}

#include "Integrate.c"

