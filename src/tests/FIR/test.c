/**************************************************************************
 * Parks-McClellan algorithm for FIR filter design (C version)
 *-------------------------------------------------
 *  Copyright (C) 1995  Jake Janovetz (janovetz@coewl.cen.uiuc.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *************************************************************************/

/***************************************************************************
 * Test program for the remez() function.  Sends appropriate arguments to
 * remez() to generate a filter.  Then, prints the resulting coefficients
 * to stdout.
 **************************************************************************/

#include "remez.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main()
{
   double *weights, *desired, *bands;
   double *h;
   int i;

   bands = (double *)malloc(10 * sizeof(double));
   weights = (double *)malloc(5 * sizeof(double));
   desired = (double *)malloc(5 * sizeof(double));
   h = (double *)malloc(600 * sizeof(double));

   desired[0] = 0;
   desired[1] = 1;
   desired[2] = 0;
//   desired[3] = 1;
//   desired[4] = 0;

/*
   weights[0] = 10;
   weights[1] = 1;
   weights[2] = 3;
   weights[3] = 1;
   weights[4] = 20;
*/

   weights[0] = 1;
   weights[1] = 1;
   weights[2] = 1;
//   weights[3] = 1;
//   weights[4] = 1;

   bands[0] = 0;
   bands[1] = 0.12;
   bands[2] = 0.17;
   bands[3] = 0.28;
   bands[4] = 0.33;
   bands[5] = 0.50;
//   bands[6] = 0.3;
//   bands[7] = 0.36;
//   bands[8] = 0.41;
//   bands[9] = 0.5;

   remez(h, 102, 2, bands, desired, weights, BANDPASS);
   for (i=0; i<102; i++)
   {
     printf("%3d, %23.20f\n", i, h[i]);
   }

   free(bands);
   free(weights);
   free(desired);
   free(h);

   return 1;
}

