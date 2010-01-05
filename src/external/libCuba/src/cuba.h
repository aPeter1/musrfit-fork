/*
	cuba.h
		Prototypes for the Cuba library
		this file is part of Cuba
		last modified 5 Dec 08 th
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

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*integrand_t)(const int *, const double [],
  const int *, double []);

/* Note: Divonne actually passes a fifth argument, a const int *
   which points to the integration phase.  This is used only rarely
   and most users are confused by the warnings the compiler emits
   if the `correct' prototype is used.  Thus, if you need to access
   this argument, use an explicit cast to integrand_t when invoking 
   Divonne. */


void Vegas(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const int mineval, const int maxeval,
  const int nstart, const int nincrease,
  int *neval, int *fail,
  double integral[], double error[], double prob[]);

void llVegas(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const long long int mineval, const long long int maxeval,
  const long long int nstart, const long long int nincrease,
  long long int *neval, int *fail,
  double integral[], double error[], double prob[]);

void Suave(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const int mineval, const int maxeval,
  const int nnew, const double flatness,
  int *nregions, int *neval, int *fail,
  double integral[], double error[], double prob[]);

void llSuave(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const long long int mineval, const long long int maxeval,
  const long long int nnew, const double flatness,
  int *nregions, long long int *neval, int *fail,
  double integral[], double error[], double prob[]);

void Divonne(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const int mineval, const int maxeval,
  const int key1, const int key2, const int key3, const int maxpass,
  const double border, const double maxchisq, const double mindeviation,
  const int ngiven, const int ldxgiven, double xgiven[],
  const int nextra,
  void (*peakfinder)(const int *, const double [], int *, double []),
  int *nregions, int *neval, int *fail,
  double integral[], double error[], double prob[]);

void llDivonne(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const long long int mineval, const long long int maxeval,
  const int key1, const int key2, const int key3, const int maxpass,
  const double border, const double maxchisq, const double mindeviation,
  const long long int ngiven, const int ldxgiven, double xgiven[],
  const long long int nextra,
  void (*peakfinder)(const int *, const double [], int *, double []),
  int *nregions, long long int *neval, int *fail,
  double integral[], double error[], double prob[]);

void Cuhre(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const int mineval, const int maxeval,
  const int key,
  int *nregions, int *neval, int *fail,
  double integral[], double error[], double prob[]);

void llCuhre(const int ndim, const int ncomp, integrand_t integrand,
  const double epsrel, const double epsabs,
  const int flags, const long long int mineval, const long long int maxeval,
  const int key,
  int *nregions, long long int *neval, int *fail,
  double integral[], double error[], double prob[]);

extern int vegasnbatch;
extern int vegasgridno;
extern char vegasstate[128];

extern int llvegasnbatch;
extern int llvegasgridno;
extern char llvegasstate[128];

extern unsigned int mersenneseed;

#ifdef __cplusplus
}
#endif

