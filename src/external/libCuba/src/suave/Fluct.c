/*
	Fluct.c
		compute the fluctuation in the left and right half
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

#if defined(HAVE_LONG_DOUBLE) && defined(HAVE_POWL)

typedef long double xdouble;
#define XDBL_MAX_EXP LDBL_MAX_EXP
#define XDBL_MAX LDBL_MAX
#define powx powl

#else

typedef double xdouble;
#define XDBL_MAX_EXP DBL_MAX_EXP
#define XDBL_MAX DBL_MAX
#define powx pow

#endif

typedef struct {
  xdouble fluct;
  number n;
} Var;

/*********************************************************************/

static void Fluct(Var *var, real flatness,
  cBounds *b, creal *w, number n, ccount comp, creal avg, creal err)
{
  creal *x = w + n, *f = x + n*ndim_ + comp;
  creal max = ldexp(1., (int)((XDBL_MAX_EXP - 2)/flatness));
  creal norm = 1/(err*Max(fabs(avg), err));
  count nvar = 2*ndim_;

  Clear(var, nvar);

  while( n-- ) {
    count dim;
    const xdouble t =
      powx(Min(1 + fabs(*w++)*Sq(*f - avg)*norm, max), flatness);

    f += ncomp_;

    for( dim = 0; dim < ndim_; ++dim ) {
      Var *v = &var[2*dim + (*x++ >= b[dim].mid)];
      const xdouble f = v->fluct + t;
      v->fluct = (f > XDBL_MAX/2) ? XDBL_MAX/2 : f;
      ++v->n;
    }
  }

  flatness = 2/3./flatness;
  while( nvar-- ) {
    var->fluct = powx(var->fluct, flatness);
    ++var;
  }
}

