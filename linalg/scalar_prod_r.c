/***********************************************************************
 * Copyright (C) 2002,2003,2004,2005,2006,2007,2008 Carsten Urbach
 *
 * This file is part of tmLQCD.
 *
 * tmLQCD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * tmLQCD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with tmLQCD.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/
/*******************************************************************************
 *
 * File scalar_prod_r.c
 *
 *   double scalar_prod_r(spinor * const S,spinor * const R, const int N)
 *     Returns the real part of the scalar product (*R,*S)
 *
 *******************************************************************************/

#ifdef HAVE_CONFIG_H
# include<config.h>
#endif
#ifdef MPI
# include <mpi.h>
#endif
#include "su3.h"
#include "scalar_prod_r.h"

/*  R input, S input */

#include <complex.h>

double scalar_prod_r(spinor * const S, spinor * const R, const int N, const int parallel)
{
  
  static double ks,kc,ds,tr,ts,tt;
  spinor *s,*r;
  
  ks=0.0;
  kc=0.0;
  
#if (defined BGL && defined XLC)
  __alignx(16, S);
  __alignx(16, R);
#endif

  for (int ix = 0; ix < N; ++ix)
  {
    s=(spinor *) S + ix;
    r=(spinor *) R + ix;
    
    ds = r->s0.c0 * conj(s->s0.c0) + r->s0.c1 * conj(s->s0.c1) + r->s0.c2 * conj(s->s0.c2) +
         r->s1.c0 * conj(s->s1.c0) + r->s1.c1 * conj(s->s1.c1) + r->s1.c2 * conj(s->s1.c2) +
	 r->s2.c0 * conj(s->s2.c0) + r->s2.c1 * conj(s->s2.c1) + r->s2.c2 * conj(s->s2.c2) +
         r->s3.c0 * conj(s->s3.c0) + r->s3.c1 * conj(s->s3.c1) + r->s3.c2 * conj(s->s3.c2);
    
    tr=ds+kc;
    ts=tr+ks;
    tt=ts-ks;
    ks=ts;
    kc=tr-tt;
  }
  kc=ks+kc;

#if defined MPI
  if(parallel)
  {
    double buffer = kc;
    MPI_Allreduce(&buffer, &kc, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  }
#endif

  return kc;

}

#ifdef WITHLAPH
double scalar_prod_r_su3vect(su3_vector * const S,su3_vector * const R, const int N, const int parallel)
{
  int ix;
  static double ks,kc,ds,tr,ts,tt;
  su3_vector *s,*r;

  ks=0.0;
  kc=0.0;
  for (int ix = 0; ix < N; ++ix)
  {
    s = (su3_vector *) S + ix;
    r = (su3_vector *) R + ix;
  
    ds = creal(r->c0) * creal(s->c0) + cimag(r->c0) * cimag(s->c0) +
         creal(r->c1) * creal(s->c1) + cimag(r->c1) * cimag(s->c1) +
         creal(r->c2) * creal(s->c2) + cimag(r->c2) * cimag(s->c2);
  
    tr = ds + kc;
    ts = tr + ks;
    tt = ts-ks;
    ks = ts;
    kc = tr-tt;
  }
  kc = ks + kc;
#if defined MPI
  if(parallel)
  {
    MPI_Allreduce(&kc, &ks, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    return ks;
  }
#endif
  return kc;
}

#endif // WITHLAPH
