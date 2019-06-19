/************************************************************
 * Copyright (C) 1998 Ian Holmes (ihh@sanger.ac.uk)
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-2003 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 ************************************************************/

/* postprob.h
 * Author: Ian Holmes (ihh@sanger.ac.uk, Jun 5 1998)
 * Derived from core_algorithms.c (SRE, Nov 11 1996)
 * Incorporated SRE, Sat Nov  6 09:07:02 1999
 * 
 * Functions for working with posterior probabilities,
 * including unfussed "backwards" and "optimal accuracy"
 * implementations.
 */

#ifndef POSTPROB_INCLUDED
#define POSTPROB_INCLUDED
#include "config.h"
#include "structs.h"
#include "funcs.h"
#include "squid.h"

/* Extra algorithms to work with posterior probabilities.
 */

extern float P7OptimalAccuracy(char *dsq, int L, struct plan7_s *hmm, 
			       struct p7trace_s **ret_tr);

extern float P7Backward(char *dsq, int L, struct plan7_s *hmm, 
			struct dpmatrix_s **ret_mx);

extern void  P7EmitterPosterior(int L, struct plan7_s *hmm,
				struct dpmatrix_s *forward,
				struct dpmatrix_s *backward,
				struct dpmatrix_s *mx);

extern float P7FillOptimalAccuracy(int L, int M,
				   struct dpmatrix_s *posterior,
				   struct dpmatrix_s *mx,
				   struct p7trace_s **ret_tr);

extern void  P7OptimalAccuracyTrace(int L, int M,
				    struct dpmatrix_s *posterior,
				    struct dpmatrix_s *mx,
				    struct p7trace_s **ret_tr);

#endif

