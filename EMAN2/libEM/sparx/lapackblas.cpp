/*
 * Author: Chao Yang
 * Copyright (c) 2000-2019
 *
 * This software is issued under a joint BSD/GNU license. You may use the
 * source code in this file under either license. However, note that the
 * complete EMAN2 and SPARX software packages have some GPL dependencies,
 * so you are responsible for compliance with the licenses of these packages
 * if you opt to use BSD licensing. The warranty disclaimer below holds
 * in either instance.
 *
 * This complete copyright notice must be included in any revised version of the
 * source code. Additional authorship citations may be added, but existing
 * author citations must be preserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <cstdio>
#include <cstdlib>

#include "lapackblas.h"

int s_cat(char *lp, const char **rpp, integer *rnp, integer *np, ftnlen ll)
//VOID s_cat(char *lp, char *rpp[], ftnlen rnp[], ftnlen *np, ftnlen ll)
{
   ftnlen i, n, nc;
   const char *f__rp;

   n = (int)*np;
   for(i = 0 ; i < n ; ++i) {
      nc = ll;
      if(rnp[i] < nc) nc = rnp[i];
      ll -= nc;
      f__rp = rpp[i];
      while(--nc >= 0)	*lp++ = *f__rp++;
   }
   while(--ll >= 0)
   *lp++ = ' ';
   return 0; 
}

integer ieeeck_(integer *ispec, real *zero, real *one)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1998   


    Purpose   
    =======   

    IEEECK is called from the ILAENV to verify that Infinity and   
    possibly NaN arithmetic is safe (i.e. will not trap).   

    Arguments   
    =========   

    ISPEC   (input) INTEGER   
            Specifies whether to test just for inifinity arithmetic   
            or whether to test for infinity and NaN arithmetic.   
            = 0: Verify infinity arithmetic only.   
            = 1: Verify infinity and NaN arithmetic.   

    ZERO    (input) REAL   
            Must contain the value 0.0   
            This is passed to prevent the compiler from optimizing   
            away this code.   

    ONE     (input) REAL   
            Must contain the value 1.0   
            This is passed to prevent the compiler from optimizing   
            away this code.   

    RETURN VALUE:  INTEGER   
            = 0:  Arithmetic failed to produce the correct answers   
            = 1:  Arithmetic produced the correct answers */
    /* System generated locals */
    integer ret_val;
    /* Local variables */
    static real neginf, posinf, negzro, newzro, nan1, nan2, nan3, nan4, nan5, 
	    nan6;


    ret_val = 1;

    posinf = *one / *zero;
    if (posinf <= *one) {
	ret_val = 0;
	return ret_val;
    }

    neginf = -(*one) / *zero;
    if (neginf >= *zero) {
	ret_val = 0;
	return ret_val;
    }

    negzro = *one / (neginf + *one);
    if (negzro != *zero) {
	ret_val = 0;
	return ret_val;
    }

    neginf = *one / negzro;
    if (neginf >= *zero) {
	ret_val = 0;
	return ret_val;
    }

    newzro = negzro + *zero;
    if (newzro != *zero) {
	ret_val = 0;
	return ret_val;
    }

    posinf = *one / newzro;
    if (posinf <= *one) {
	ret_val = 0;
	return ret_val;
    }

    neginf *= posinf;
    if (neginf >= *zero) {
	ret_val = 0;
	return ret_val;
    }

    posinf *= posinf;
    if (posinf <= *one) {
	ret_val = 0;
	return ret_val;
    }




/*     Return if we were only asked to check infinity arithmetic */

    if (*ispec == 0) {
	return ret_val;
    }

    nan1 = posinf + neginf;

    nan2 = posinf / neginf;

    nan3 = posinf / posinf;

    nan4 = posinf * *zero;

    nan5 = neginf * negzro;

    nan6 = nan5 * 0.f;

    if (nan1 == nan1) {
	ret_val = 0;
	return ret_val;
    }

    if (nan2 == nan2) {
	ret_val = 0;
	return ret_val;
    }

    if (nan3 == nan3) {
	ret_val = 0;
	return ret_val;
    }

    if (nan4 == nan4) {
	ret_val = 0;
	return ret_val;
    }

    if (nan5 == nan5) {
	ret_val = 0;
	return ret_val;
    }

    if (nan6 == nan6) {
	ret_val = 0;
	return ret_val;
    }

    return ret_val;
} /* ieeeck_ */




integer ilaenv_(integer *ispec, const char *name__, const char *, integer *n1, 
	integer *n2, integer *, integer *n4, ftnlen name_len, ftnlen )
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    ILAENV is called from the LAPACK routines to choose problem-dependent   
    parameters for the local environment.  See ISPEC for a description of   
    the parameters.   

    This version provides a set of parameters which should give good,   
    but not optimal, performance on many of the currently available   
    computers.  Users are encouraged to modify this subroutine to set   
    the tuning parameters for their particular machine using the option   
    and problem size information in the arguments.   

    This routine will not function correctly if it is converted to all   
    lower case.  Converting it to all upper case is allowed.   

    Arguments   
    =========   

    ISPEC   (input) INTEGER   
            Specifies the parameter to be returned as the value of   
            ILAENV.   
            = 1: the optimal blocksize; if this value is 1, an unblocked   
                 algorithm will give the best performance.   
            = 2: the minimum block size for which the block routine   
                 should be used; if the usable block size is less than   
                 this value, an unblocked routine should be used.   
            = 3: the crossover point (in a block routine, for N less   
                 than this value, an unblocked routine should be used)   
            = 4: the number of shifts, used in the nonsymmetric   
                 eigenvalue routines   
            = 5: the MINIMUM Column dimension for blocking to be used;   
                 rectangular blocks must have dimension at least k by m,   
                 where k is given by ILAENV(2,...) and m by ILAENV(5,...)   
            = 6: the crossover point for the SVD (when reducing an m by n   
                 matrix to bidiagonal form, if f2cmax(m,n)/min(m,n) exceeds   
                 this value, a QR factorization is used first to reduce   
                 the matrix to a triangular form.)   
            = 7: the number of processors   
            = 8: the crossover point for the multishift QR and QZ methods   
                 for nonsymmetric eigenvalue problems.   
            = 9: maximum size of the subproblems at the bottom of the   
                 computation tree in the divide-and-conquer algorithm   
                 (used by xGELSD and xGESDD)   
            =10: ieee NaN arithmetic can be trusted not to trap   
            =11: infinity arithmetic can be trusted not to trap   

    NAME    (input) CHARACTER*(*)   
            The name of the calling subroutine, in either upper case or   
            lower case.   

    OPTS    (input) CHARACTER*(*)   
            The character options to the subroutine NAME, concatenated   
            into a single character string.  For example, UPLO = 'U',   
            TRANS = 'T', and DIAG = 'N' for a triangular routine would   
            be specified as OPTS = 'UTN'.   

    N1      (input) INTEGER   
    N2      (input) INTEGER   
    N3      (input) INTEGER   
    N4      (input) INTEGER   
            Problem dimensions for the subroutine NAME; these may not all   
            be required.   

   (ILAENV) (output) INTEGER   
            >= 0: the value of the parameter specified by ISPEC   
            < 0:  if ILAENV = -k, the k-th argument had an illegal value.   

    Further Details   
    ===============   

    The following conventions have been used when calling ILAENV from the   
    LAPACK routines:   
    1)  OPTS is a concatenation of all of the character options to   
        subroutine NAME, in the same order that they appear in the   
        argument list for NAME, even if they are not used in determining   
        the value of the parameter specified by ISPEC.   
    2)  The problem dimensions N1, N2, N3, N4 are specified in the order   
        that they appear in the argument list for NAME.  N1 is used   
        first, N2 second, and so on, and unused problem dimensions are   
        passed a value of -1.   
    3)  The parameter value returned by ILAENV is checked for validity in   
        the calling subroutine.  For example, ILAENV is used to retrieve   
        the optimal blocksize for STRTRI as follows:   

        NB = ILAENV( 1, 'STRTRI', UPLO // DIAG, N, -1, -1, -1 )   
        IF( NB.LE.1 ) NB = MAX( 1, N )   

    ===================================================================== */
    /* Table of constant values */
    static integer c__0 = 0;
    static real c_b162 = 0.f;
    static real c_b163 = 1.f;
    static integer c__1 = 1;
    
    /* System generated locals */
    integer ret_val;
    /* Builtin functions   
       Subroutine */ void s_copy(char *, const char *, ftnlen, ftnlen);
    integer s_cmp(char *, const char *, ftnlen, ftnlen);
    /* Local variables */
    static integer i__;
    static logical cname, sname;
    static integer nbmin;
    static char c1[1], c2[2], c3[3], c4[2];
    static integer ic, nb;
    extern integer ieeeck_(integer *, real *, real *);
    static integer iz, nx;
    static char subnam[6];




    switch (*ispec) {
	case 1:  goto L100;
	case 2:  goto L100;
	case 3:  goto L100;
	case 4:  goto L400;
	case 5:  goto L500;
	case 6:  goto L600;
	case 7:  goto L700;
	case 8:  goto L800;
	case 9:  goto L900;
	case 10:  goto L1000;
	case 11:  goto L1100;
    }

/*     Invalid value for ISPEC */

    ret_val = -1;
    return ret_val;

L100:

/*     Convert NAME to upper case if the first character is lower case. */

    ret_val = 1;
    s_copy(subnam, name__, (ftnlen)6, name_len);
    ic = *(unsigned char *)subnam;
    iz = 'Z';
    if (iz == 90 || iz == 122) {

/*        ASCII character set */

	if (ic >= 97 && ic <= 122) {
	    *(unsigned char *)subnam = (char) (ic - 32);
	    for (i__ = 2; i__ <= 6; ++i__) {
		ic = *(unsigned char *)&subnam[i__ - 1];
		if (ic >= 97 && ic <= 122) {
		    *(unsigned char *)&subnam[i__ - 1] = (char) (ic - 32);
		}
/* L10: */
	    }
	}

    } else if (iz == 233 || iz == 169) {

/*        EBCDIC character set */

	if (ic >= 129 && ic <= 137 || ic >= 145 && ic <= 153 || ic >= 162 && 
		ic <= 169) {
	    *(unsigned char *)subnam = (char) (ic + 64);
	    for (i__ = 2; i__ <= 6; ++i__) {
		ic = *(unsigned char *)&subnam[i__ - 1];
		if (ic >= 129 && ic <= 137 || ic >= 145 && ic <= 153 || ic >= 
			162 && ic <= 169) {
		    *(unsigned char *)&subnam[i__ - 1] = (char) (ic + 64);
		}
/* L20: */
	    }
	}

    } else if (iz == 218 || iz == 250) {

/*        Prime machines:  ASCII+128 */

	if (ic >= 225 && ic <= 250) {
	    *(unsigned char *)subnam = (char) (ic - 32);
	    for (i__ = 2; i__ <= 6; ++i__) {
		ic = *(unsigned char *)&subnam[i__ - 1];
		if (ic >= 225 && ic <= 250) {
		    *(unsigned char *)&subnam[i__ - 1] = (char) (ic - 32);
		}
/* L30: */
	    }
	}
    }

    *(unsigned char *)c1 = *(unsigned char *)subnam;
    sname = *(unsigned char *)c1 == 'S' || *(unsigned char *)c1 == 'D';
    cname = *(unsigned char *)c1 == 'C' || *(unsigned char *)c1 == 'Z';
    if (! (cname || sname)) {
	return ret_val;
    }
    s_copy(c2, subnam + 1, (ftnlen)2, (ftnlen)2);
    s_copy(c3, subnam + 3, (ftnlen)3, (ftnlen)3);
    s_copy(c4, c3 + 1, (ftnlen)2, (ftnlen)2);

    switch (*ispec) {
	case 1:  goto L110;
	case 2:  goto L200;
	case 3:  goto L300;
    }

L110:

/*     ISPEC = 1:  block size   

       In these examples, separate code is provided for setting NB for   
       real and complex.  We assume that NB will take the same value in   
       single or double precision. */

    nb = 1;

    if (s_cmp(c2, "GE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	} else if (s_cmp(c3, "QRF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, 
		"RQF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "LQF", (ftnlen)
		3, (ftnlen)3) == 0 || s_cmp(c3, "QLF", (ftnlen)3, (ftnlen)3) 
		== 0) {
	    if (sname) {
		nb = 32;
	    } else {
		nb = 32;
	    }
	} else if (s_cmp(c3, "HRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 32;
	    } else {
		nb = 32;
	    }
	} else if (s_cmp(c3, "BRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 32;
	    } else {
		nb = 32;
	    }
	} else if (s_cmp(c3, "TRI", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (s_cmp(c2, "PO", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (s_cmp(c2, "SY", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	} else if (sname && s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 32;
	} else if (sname && s_cmp(c3, "GST", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 64;
	}
    } else if (cname && s_cmp(c2, "HE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 64;
	} else if (s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 32;
	} else if (s_cmp(c3, "GST", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 64;
	}
    } else if (sname && s_cmp(c2, "OR", (ftnlen)2, (ftnlen)2) == 0) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	}
    } else if (cname && s_cmp(c2, "UN", (ftnlen)2, (ftnlen)2) == 0) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nb = 32;
	    }
	}
    } else if (s_cmp(c2, "GB", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		if (*n4 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    } else {
		if (*n4 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    }
	}
    } else if (s_cmp(c2, "PB", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		if (*n2 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    } else {
		if (*n2 <= 64) {
		    nb = 1;
		} else {
		    nb = 32;
		}
	    }
	}
    } else if (s_cmp(c2, "TR", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRI", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (s_cmp(c2, "LA", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "UUM", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nb = 64;
	    } else {
		nb = 64;
	    }
	}
    } else if (sname && s_cmp(c2, "ST", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "EBZ", (ftnlen)3, (ftnlen)3) == 0) {
	    nb = 1;
	}
    }
    ret_val = nb;
    return ret_val;

L200:

/*     ISPEC = 2:  minimum block size */

    nbmin = 2;
    if (s_cmp(c2, "GE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "QRF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "RQF", (
		ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "LQF", (ftnlen)3, (
		ftnlen)3) == 0 || s_cmp(c3, "QLF", (ftnlen)3, (ftnlen)3) == 0)
		 {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	} else if (s_cmp(c3, "HRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	} else if (s_cmp(c3, "BRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	} else if (s_cmp(c3, "TRI", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 2;
	    } else {
		nbmin = 2;
	    }
	}
    } else if (s_cmp(c2, "SY", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRF", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nbmin = 8;
	    } else {
		nbmin = 8;
	    }
	} else if (sname && s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nbmin = 2;
	}
    } else if (cname && s_cmp(c2, "HE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nbmin = 2;
	}
    } else if (sname && s_cmp(c2, "OR", (ftnlen)2, (ftnlen)2) == 0) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	}
    } else if (cname && s_cmp(c2, "UN", (ftnlen)2, (ftnlen)2) == 0) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	} else if (*(unsigned char *)c3 == 'M') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nbmin = 2;
	    }
	}
    }
    ret_val = nbmin;
    return ret_val;

L300:

/*     ISPEC = 3:  crossover point */

    nx = 0;
    if (s_cmp(c2, "GE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "QRF", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "RQF", (
		ftnlen)3, (ftnlen)3) == 0 || s_cmp(c3, "LQF", (ftnlen)3, (
		ftnlen)3) == 0 || s_cmp(c3, "QLF", (ftnlen)3, (ftnlen)3) == 0)
		 {
	    if (sname) {
		nx = 128;
	    } else {
		nx = 128;
	    }
	} else if (s_cmp(c3, "HRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nx = 128;
	    } else {
		nx = 128;
	    }
	} else if (s_cmp(c3, "BRD", (ftnlen)3, (ftnlen)3) == 0) {
	    if (sname) {
		nx = 128;
	    } else {
		nx = 128;
	    }
	}
    } else if (s_cmp(c2, "SY", (ftnlen)2, (ftnlen)2) == 0) {
	if (sname && s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nx = 32;
	}
    } else if (cname && s_cmp(c2, "HE", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(c3, "TRD", (ftnlen)3, (ftnlen)3) == 0) {
	    nx = 32;
	}
    } else if (sname && s_cmp(c2, "OR", (ftnlen)2, (ftnlen)2) == 0) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nx = 128;
	    }
	}
    } else if (cname && s_cmp(c2, "UN", (ftnlen)2, (ftnlen)2) == 0) {
	if (*(unsigned char *)c3 == 'G') {
	    if (s_cmp(c4, "QR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "RQ", 
		    (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "LQ", (ftnlen)2, (
		    ftnlen)2) == 0 || s_cmp(c4, "QL", (ftnlen)2, (ftnlen)2) ==
		     0 || s_cmp(c4, "HR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(
		    c4, "TR", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(c4, "BR", (
		    ftnlen)2, (ftnlen)2) == 0) {
		nx = 128;
	    }
	}
    }
    ret_val = nx;
    return ret_val;

L400:

/*     ISPEC = 4:  number of shifts (used by xHSEQR) */

    ret_val = 6;
    return ret_val;

L500:

/*     ISPEC = 5:  minimum column dimension (not used) */

    ret_val = 2;
    return ret_val;

L600:

/*     ISPEC = 6:  crossover point for SVD (used by xGELSS and xGESVD) */

    ret_val = (integer) ((real) f2cmin(*n1,*n2) * 1.6f);
    return ret_val;

L700:

/*     ISPEC = 7:  number of processors (not used) */

    ret_val = 1;
    return ret_val;

L800:

/*     ISPEC = 8:  crossover point for multishift (used by xHSEQR) */

    ret_val = 50;
    return ret_val;

L900:

/*     ISPEC = 9:  maximum size of the subproblems at the bottom of the   
                   computation tree in the divide-and-conquer algorithm   
                   (used by xGELSD and xGESDD) */

    ret_val = 25;
    return ret_val;

L1000:

/*     ISPEC = 10: ieee NaN arithmetic can be trusted not to trap   

       ILAENV = 0 */
    ret_val = 1;
    if (ret_val == 1) {
	ret_val = ieeeck_(&c__0, &c_b162, &c_b163);
    }
    return ret_val;

L1100:

/*     ISPEC = 11: infinity arithmetic can be trusted not to trap   

       ILAENV = 0 */
    ret_val = 1;
    if (ret_val == 1) {
	ret_val = ieeeck_(&c__1, &c_b162, &c_b163);
    }
    return ret_val;

/*     End of ILAENV */

} /* ilaenv_ */



logical lsame_(const char *ca, const char *cb)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    LSAME returns .TRUE. if CA is the same letter as CB regardless of   
    case.   

    Arguments   
    =========   

    CA      (input) CHARACTER*1   
    CB      (input) CHARACTER*1   
            CA and CB specify the single characters to be compared.   

   ===================================================================== 
  


       Test if the characters are equal */
    /* System generated locals */
    logical ret_val;
    /* Local variables */
    static integer inta, intb, zcode;


    ret_val = *(unsigned char *)ca == *(unsigned char *)cb;
    if (ret_val) {
	return ret_val;
    }

/*     Now test for equivalence if both characters are alphabetic. */

    zcode = 'Z';

/*     Use 'Z' rather than 'A' so that ASCII can be detected on Prime   
       machines, on which ICHAR returns a value with bit 8 set.   
       ICHAR('A') on Prime machines returns 193 which is the same as   
       ICHAR('A') on an EBCDIC machine. */

    inta = *(unsigned char *)ca;
    intb = *(unsigned char *)cb;

    if (zcode == 90 || zcode == 122) {

/*        ASCII is assumed - ZCODE is the ASCII code of either lower o
r   
          upper case 'Z'. */

	if (inta >= 97 && inta <= 122) {
	    inta += -32;
	}
	if (intb >= 97 && intb <= 122) {
	    intb += -32;
	}

    } else if (zcode == 233 || zcode == 169) {

/*        EBCDIC is assumed - ZCODE is the EBCDIC code of either lower
 or   
          upper case 'Z'. */

	if (inta >= 129 && inta <= 137 || inta >= 145 && inta <= 153 || inta 
		>= 162 && inta <= 169) {
	    inta += 64;
	}
	if (intb >= 129 && intb <= 137 || intb >= 145 && intb <= 153 || intb 
		>= 162 && intb <= 169) {
	    intb += 64;
	}

    } else if (zcode == 218 || zcode == 250) {

/*        ASCII is assumed, on Prime machines - ZCODE is the ASCII cod
e   
          plus 128 of either lower or upper case 'Z'. */

	if (inta >= 225 && inta <= 250) {
	    inta += -32;
	}
	if (intb >= 225 && intb <= 250) {
	    intb += -32;
	}
    }
    ret_val = inta == intb;

/*     RETURN   

       End of LSAME */

    return ret_val;
} /* lsame_ */



#ifdef KR_headers
double pow_ri(ap, bp) real *ap; integer *bp;
#else
double pow_ri(real *ap, integer *bp)
#endif
{
double pow, x;
integer n;
unsigned long u;

pow = 1;
x = *ap;
n = *bp;

if(n != 0)
	{
	if(n < 0)
		{
		n = -n;
		x = 1/x;
		}
	for(u = n; ; )
		{
		if(u & 01)
			pow *= x;
		if(u >>= 1)
			x *= x;
		else
			break;
		}
	}
return(pow);
}

#ifdef KR_headers
integer pow_ii(ap, bp) integer *ap, *bp;
#else
integer pow_ii(integer *ap, integer *bp)
#endif
{
	integer pow, x, n;
	unsigned long u;

	x = *ap;
	n = *bp;

	if (n <= 0) {
		if (n == 0 || x == 1)
			return 1;
		if (x != -1)
			return x != 0 ? 1/x : 0;
		n = -n;
		}
	u = n;
	for(pow = 1; ; )
		{
		if(u & 01)
			pow *= x;
		if(u >>= 1)
			x *= x;
		else
			break;
		}
	return(pow);
	}

#ifdef KR_headers
double r_sign(a,b) real *a, *b;
#else
double r_sign(real *a, real *b)
#endif
{
double x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}



/* Subroutine */ int saxpy_(integer *n, real *sa, real *sx, integer *incx, 
	real *sy, integer *incy)
{
    /* System generated locals */
    integer i__1;
    /* Local variables */
    static integer i__, m, ix, iy, mp1;
/*     constant times a vector plus a vector.   
       uses unrolled loop for increments equal to one.   
       jack dongarra, linpack, 3/11/78.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sy;
    --sx;
    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*sa == 0.f) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }
/*        code for unequal increments or equal increments   
            not equal to 1 */
    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sy[iy] += *sa * sx[ix];
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;
/*        code for both increments equal to 1   
          clean-up loop */
L20:
    m = *n % 4;
    if (m == 0) {
	goto L40;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sy[i__] += *sa * sx[i__];
/* L30: */
    }
    if (*n < 4) {
	return 0;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 4) {
	sy[i__] += *sa * sx[i__];
	sy[i__ + 1] += *sa * sx[i__ + 1];
	sy[i__ + 2] += *sa * sx[i__ + 2];
	sy[i__ + 3] += *sa * sx[i__ + 3];
/* L50: */
    }
    return 0;
} /* saxpy_ */



/* compare two strings */

#ifdef KR_headers
integer s_cmp(a0, b0, la, lb) char *a0, *b0; ftnlen la, lb;
#else
integer s_cmp(char *a0, const char *b0, ftnlen la, ftnlen lb)
#endif
{
unsigned char *a, *aend, *b, *bend;
a = (unsigned char *)a0;
b = (unsigned char *)b0;
aend = a + la;
bend = b + lb;

if(la <= lb)
	{
	while(a < aend)
		if(*a != *b)
			return( *a - *b );
		else
			{ ++a; ++b; }

	while(b < bend)
		if(*b != ' ')
			return( ' ' - *b );
		else	++b;
	}

else
	{
	while(b < bend)
		if(*a == *b)
			{ ++a; ++b; }
		else
			return( *a - *b );
	while(a < aend)
		if(*a != ' ')
			return(*a - ' ');
		else	++a;
	}
return(0);
}
/* Unless compiled with -DNO_OVERWRITE, this variant of s_copy allows the
 * target of an assignment to appear on its right-hand side (contrary
 * to the Fortran 77 Standard, but in accordance with Fortran 90),
 * as in  a(2:5) = a(4:7) .
 */



/* assign strings:  a = b */

#ifdef KR_headers
VOID s_copy(a, b, la, lb) char *a, *b; ftnlen la, lb;
#else
void s_copy(char *a, const char *b, ftnlen la, ftnlen lb)
#endif
{
	char *aend;
	const char *bend;

	aend = a + la;

	if(la <= lb)
#ifndef NO_OVERWRITE
		if (a <= b || a >= b + la)
#endif
			while(a < aend)
				*a++ = *b++;
#ifndef NO_OVERWRITE
		else
			for(b += la; a < aend; )
				*--aend = *--b;
#endif

	else {
		bend = b + lb;
#ifndef NO_OVERWRITE
		if (a <= b || a >= bend)
#endif
			while(b < bend)
				*a++ = *b++;
#ifndef NO_OVERWRITE
		else {
			a += lb;
			while(b < bend)
				*--a = *--bend;
			a += lb;
			}
#endif
		while(a < aend)
			*a++ = ' ';
		}
	}



/* Subroutine */ int scopy_(integer *n, real *sx, integer *incx, real *sy, 
	integer *incy)
{
    /* System generated locals */
    integer i__1;
    /* Local variables */
    static integer i__, m, ix, iy, mp1;
/*     copies a vector, x, to a vector, y.   
       uses unrolled loops for increments equal to 1.   
       jack dongarra, linpack, 3/11/78.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sy;
    --sx;
    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }
/*        code for unequal increments or equal increments   
            not equal to 1 */
    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sy[iy] = sx[ix];
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;
/*        code for both increments equal to 1   
          clean-up loop */
L20:
    m = *n % 7;
    if (m == 0) {
	goto L40;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sy[i__] = sx[i__];
/* L30: */
    }
    if (*n < 7) {
	return 0;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 7) {
	sy[i__] = sx[i__];
	sy[i__ + 1] = sx[i__ + 1];
	sy[i__ + 2] = sx[i__ + 2];
	sy[i__ + 3] = sx[i__ + 3];
	sy[i__ + 4] = sx[i__ + 4];
	sy[i__ + 5] = sx[i__ + 5];
	sy[i__ + 6] = sx[i__ + 6];
/* L50: */
    }
    return 0;
} /* scopy_ */




doublereal sdot_(integer *n, real *sx, integer *incx, real *sy, integer *incy)
{
    /* System generated locals */
    integer i__1;
    real ret_val;
    /* Local variables */
    static integer i__, m;
    static real stemp;
    static integer ix, iy, mp1;
/*     forms the dot product of two vectors.   
       uses unrolled loops for increments equal to one.   
       jack dongarra, linpack, 3/11/78.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sy;
    --sx;
    /* Function Body */
    stemp = 0.f;
    ret_val = 0.f;
    if (*n <= 0) {
	return ret_val;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }
/*        code for unequal increments or equal increments   
            not equal to 1 */
    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	stemp += sx[ix] * sy[iy];
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    ret_val = stemp;
    return ret_val;
/*        code for both increments equal to 1   
          clean-up loop */
L20:
    m = *n % 5;
    if (m == 0) {
	goto L40;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	stemp += sx[i__] * sy[i__];
/* L30: */
    }
    if (*n < 5) {
	goto L60;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 5) {
	stemp = stemp + sx[i__] * sy[i__] + sx[i__ + 1] * sy[i__ + 1] + sx[
		i__ + 2] * sy[i__ + 2] + sx[i__ + 3] * sy[i__ + 3] + sx[i__ + 
		4] * sy[i__ + 4];
/* L50: */
    }
L60:
    ret_val = stemp;
    return ret_val;
} /* sdot_ */




/* Subroutine */ int sgemm_(const char *transa, const char *transb, integer *m, integer *
	n, integer *k, real *alpha, real *a, integer *lda, real *b, integer *
	ldb, real *beta, real *c__, integer *ldc)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, i__1, i__2, 
	    i__3;
    /* Local variables */
    static integer info;
    static logical nota, notb;
    static real temp;
    static integer i__, j, l, ncola;
    extern logical lsame_(const char *, const char *);
    static integer nrowa, nrowb;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]
/*  Purpose   
    =======   
    SGEMM  performs one of the matrix-matrix operations   
       C := alpha*op( A )*op( B ) + beta*C,   
    where  op( X ) is one of   
       op( X ) = X   or   op( X ) = X',   
    alpha and beta are scalars, and A, B and C are matrices, with op( A )   
    an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix.   
    Parameters   
    ==========   
    TRANSA - CHARACTER*1.   
             On entry, TRANSA specifies the form of op( A ) to be used in   
             the matrix multiplication as follows:   
                TRANSA = 'N' or 'n',  op( A ) = A.   
                TRANSA = 'T' or 't',  op( A ) = A'.   
                TRANSA = 'C' or 'c',  op( A ) = A'.   
             Unchanged on exit.   
    TRANSB - CHARACTER*1.   
             On entry, TRANSB specifies the form of op( B ) to be used in   
             the matrix multiplication as follows:   
                TRANSB = 'N' or 'n',  op( B ) = B.   
                TRANSB = 'T' or 't',  op( B ) = B'.   
                TRANSB = 'C' or 'c',  op( B ) = B'.   
             Unchanged on exit.   
    M      - INTEGER.   
             On entry,  M  specifies  the number  of rows  of the  matrix   
             op( A )  and of the  matrix  C.  M  must  be at least  zero.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry,  N  specifies the number  of columns of the matrix   
             op( B ) and the number of columns of the matrix C. N must be   
             at least zero.   
             Unchanged on exit.   
    K      - INTEGER.   
             On entry,  K  specifies  the number of columns of the matrix   
             op( A ) and the number of rows of the matrix op( B ). K must   
             be at least  zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, ka ), where ka is   
             k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.   
             Before entry with  TRANSA = 'N' or 'n',  the leading  m by k   
             part of the array  A  must contain the matrix  A,  otherwise   
             the leading  k by m  part of the array  A  must contain  the   
             matrix A.   
             Unchanged on exit.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program. When  TRANSA = 'N' or 'n' then   
             LDA must be at least  f2cmax( 1, m ), otherwise  LDA must be at   
             least  f2cmax( 1, k ).   
             Unchanged on exit.   
    B      - REAL             array of DIMENSION ( LDB, kb ), where kb is   
             n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.   
             Before entry with  TRANSB = 'N' or 'n',  the leading  k by n   
             part of the array  B  must contain the matrix  B,  otherwise   
             the leading  n by k  part of the array  B  must contain  the   
             matrix B.   
             Unchanged on exit.   
    LDB    - INTEGER.   
             On entry, LDB specifies the first dimension of B as declared   
             in the calling (sub) program. When  TRANSB = 'N' or 'n' then   
             LDB must be at least  f2cmax( 1, k ), otherwise  LDB must be at   
             least  f2cmax( 1, n ).   
             Unchanged on exit.   
    BETA   - REAL            .   
             On entry,  BETA  specifies the scalar  beta.  When  BETA  is   
             supplied as zero then C need not be set on input.   
             Unchanged on exit.   
    C      - REAL             array of DIMENSION ( LDC, n ).   
             Before entry, the leading  m by n  part of the array  C must   
             contain the matrix  C,  except when  beta  is zero, in which   
             case C need not be set on entry.   
             On exit, the array  C  is overwritten by the  m by n  matrix   
             ( alpha*op( A )*op( B ) + beta*C ).   
    LDC    - INTEGER.   
             On entry, LDC specifies the first dimension of C as declared   
             in  the  calling  (sub)  program.   LDC  must  be  at  least   
             f2cmax( 1, m ).   
             Unchanged on exit.   
    Level 3 Blas routine.   
    -- Written on 8-February-1989.   
       Jack Dongarra, Argonne National Laboratory.   
       Iain Duff, AERE Harwell.   
       Jeremy Du Croz, Numerical Algorithms Group Ltd.   
       Sven Hammarling, Numerical Algorithms Group Ltd.   
       Set  NOTA  and  NOTB  as  true if  A  and  B  respectively are not   
       transposed and set  NROWA, NCOLA and  NROWB  as the number of rows   
       and  columns of  A  and the  number of  rows  of  B  respectively.   
       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    /* Function Body */
    nota = lsame_(transa, "N");
    notb = lsame_(transb, "N");
    if (nota) {
	nrowa = *m;
	ncola = *k;
    } else {
	nrowa = *k;
	ncola = *m;
    }
    if (notb) {
	nrowb = *k;
    } else {
	nrowb = *n;
    }
/*     Test the input parameters. */
    info = 0;
    if (! nota && ! lsame_(transa, "C") && ! lsame_(
	    transa, "T")) {
	info = 1;
    } else if (! notb && ! lsame_(transb, "C") && ! 
	    lsame_(transb, "T")) {
	info = 2;
    } else if (*m < 0) {
	info = 3;
    } else if (*n < 0) {
	info = 4;
    } else if (*k < 0) {
	info = 5;
    } else if (*lda < f2cmax(1,nrowa)) {
	info = 8;
    } else if (*ldb < f2cmax(1,nrowb)) {
	info = 10;
    } else if (*ldc < f2cmax(1,*m)) {
	info = 13;
    }
    if (info != 0) {
	xerbla_("SGEMM ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*m == 0 || *n == 0 || (*alpha == 0.f || *k == 0) && *beta == 1.f) {
	return 0;
    }
/*     And if  alpha.eq.zero. */
    if (*alpha == 0.f) {
	if (*beta == 0.f) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    c___ref(i__, j) = 0.f;
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    c___ref(i__, j) = *beta * c___ref(i__, j);
/* L30: */
		}
/* L40: */
	    }
	}
	return 0;
    }
/*     Start the operations. */
    if (notb) {
	if (nota) {
/*           Form  C := alpha*A*B + beta*C. */
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (*beta == 0.f) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = 0.f;
/* L50: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = *beta * c___ref(i__, j);
/* L60: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= i__2; ++l) {
		    if (b_ref(l, j) != 0.f) {
			temp = *alpha * b_ref(l, j);
			i__3 = *m;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    c___ref(i__, j) = c___ref(i__, j) + temp * a_ref(
				    i__, l);
/* L70: */
			}
		    }
/* L80: */
		}
/* L90: */
	    }
	} else {
/*           Form  C := alpha*A'*B + beta*C */
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    temp = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= i__3; ++l) {
			temp += a_ref(l, i__) * b_ref(l, j);
/* L100: */
		    }
		    if (*beta == 0.f) {
			c___ref(i__, j) = *alpha * temp;
		    } else {
			c___ref(i__, j) = *alpha * temp + *beta * c___ref(i__,
				 j);
		    }
/* L110: */
		}
/* L120: */
	    }
	}
    } else {
	if (nota) {
/*           Form  C := alpha*A*B' + beta*C */
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (*beta == 0.f) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = 0.f;
/* L130: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = *beta * c___ref(i__, j);
/* L140: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= i__2; ++l) {
		    if (b_ref(j, l) != 0.f) {
			temp = *alpha * b_ref(j, l);
			i__3 = *m;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    c___ref(i__, j) = c___ref(i__, j) + temp * a_ref(
				    i__, l);
/* L150: */
			}
		    }
/* L160: */
		}
/* L170: */
	    }
	} else {
/*           Form  C := alpha*A'*B' + beta*C */
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    temp = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= i__3; ++l) {
			temp += a_ref(l, i__) * b_ref(j, l);
/* L180: */
		    }
		    if (*beta == 0.f) {
			c___ref(i__, j) = *alpha * temp;
		    } else {
			c___ref(i__, j) = *alpha * temp + *beta * c___ref(i__,
				 j);
		    }
/* L190: */
		}
/* L200: */
	    }
	}
    }
    return 0;
/*     End of SGEMM . */
} /* sgemm_ */
#undef c___ref
#undef b_ref
#undef a_ref




/* Subroutine */ int sgemv_(const char *trans, integer *m, integer *n, real *alpha, 
	real *a, integer *lda, real *x, integer *incx, real *beta, real *y, 
	integer *incy)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    /* Local variables */
    static integer info;
    static real temp;
    static integer lenx, leny, i__, j;
    extern logical lsame_(const char *, const char *);
    static integer ix, iy, jx, jy, kx, ky;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
/*  Purpose   
    =======   
    SGEMV  performs one of the matrix-vector operations   
       y := alpha*A*x + beta*y,   or   y := alpha*A'*x + beta*y,   
    where alpha and beta are scalars, x and y are vectors and A is an   
    m by n matrix.   
    Parameters   
    ==========   
    TRANS  - CHARACTER*1.   
             On entry, TRANS specifies the operation to be performed as   
             follows:   
                TRANS = 'N' or 'n'   y := alpha*A*x + beta*y.   
                TRANS = 'T' or 't'   y := alpha*A'*x + beta*y.   
                TRANS = 'C' or 'c'   y := alpha*A'*x + beta*y.   
             Unchanged on exit.   
    M      - INTEGER.   
             On entry, M specifies the number of rows of the matrix A.   
             M must be at least zero.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry, N specifies the number of columns of the matrix A.   
             N must be at least zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, n ).   
             Before entry, the leading m by n part of the array A must   
             contain the matrix of coefficients.   
             Unchanged on exit.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program. LDA must be at least   
             f2cmax( 1, m ).   
             Unchanged on exit.   
    X      - REAL             array of DIMENSION at least   
             ( 1 + ( n - 1 )*abs( INCX ) ) when TRANS = 'N' or 'n'   
             and at least   
             ( 1 + ( m - 1 )*abs( INCX ) ) otherwise.   
             Before entry, the incremented array X must contain the   
             vector x.   
             Unchanged on exit.   
    INCX   - INTEGER.   
             On entry, INCX specifies the increment for the elements of   
             X. INCX must not be zero.   
             Unchanged on exit.   
    BETA   - REAL            .   
             On entry, BETA specifies the scalar beta. When BETA is   
             supplied as zero then Y need not be set on input.   
             Unchanged on exit.   
    Y      - REAL             array of DIMENSION at least   
             ( 1 + ( m - 1 )*abs( INCY ) ) when TRANS = 'N' or 'n'   
             and at least   
             ( 1 + ( n - 1 )*abs( INCY ) ) otherwise.   
             Before entry with BETA non-zero, the incremented array Y   
             must contain the vector y. On exit, Y is overwritten by the   
             updated vector y.   
    INCY   - INTEGER.   
             On entry, INCY specifies the increment for the elements of   
             Y. INCY must not be zero.   
             Unchanged on exit.   
    Level 2 Blas routine.   
    -- Written on 22-October-1986.   
       Jack Dongarra, Argonne National Lab.   
       Jeremy Du Croz, Nag Central Office.   
       Sven Hammarling, Nag Central Office.   
       Richard Hanson, Sandia National Labs.   
       Test the input parameters.   
       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --x;
    --y;
    /* Function Body */
    info = 0;
    if (! lsame_(trans, "N") && ! lsame_(trans, "T") && ! lsame_(trans, "C")
	    ) {
	info = 1;
    } else if (*m < 0) {
	info = 2;
    } else if (*n < 0) {
	info = 3;
    } else if (*lda < f2cmax(1,*m)) {
	info = 6;
    } else if (*incx == 0) {
	info = 8;
    } else if (*incy == 0) {
	info = 11;
    }
    if (info != 0) {
	xerbla_("SGEMV ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*m == 0 || *n == 0 || *alpha == 0.f && *beta == 1.f) {
	return 0;
    }
/*     Set  LENX  and  LENY, the lengths of the vectors x and y, and set   
       up the start points in  X  and  Y. */
    if (lsame_(trans, "N")) {
	lenx = *n;
	leny = *m;
    } else {
	lenx = *m;
	leny = *n;
    }
    if (*incx > 0) {
	kx = 1;
    } else {
	kx = 1 - (lenx - 1) * *incx;
    }
    if (*incy > 0) {
	ky = 1;
    } else {
	ky = 1 - (leny - 1) * *incy;
    }
/*     Start the operations. In this version the elements of A are   
       accessed sequentially with one pass through A.   
       First form  y := beta*y. */
    if (*beta != 1.f) {
	if (*incy == 1) {
	    if (*beta == 0.f) {
		i__1 = leny;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[i__] = 0.f;
/* L10: */
		}
	    } else {
		i__1 = leny;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[i__] = *beta * y[i__];
/* L20: */
		}
	    }
	} else {
	    iy = ky;
	    if (*beta == 0.f) {
		i__1 = leny;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[iy] = 0.f;
		    iy += *incy;
/* L30: */
		}
	    } else {
		i__1 = leny;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[iy] = *beta * y[iy];
		    iy += *incy;
/* L40: */
		}
	    }
	}
    }
    if (*alpha == 0.f) {
	return 0;
    }
    if (lsame_(trans, "N")) {
/*        Form  y := alpha*A*x + y. */
	jx = kx;
	if (*incy == 1) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (x[jx] != 0.f) {
		    temp = *alpha * x[jx];
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			y[i__] += temp * a_ref(i__, j);
/* L50: */
		    }
		}
		jx += *incx;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (x[jx] != 0.f) {
		    temp = *alpha * x[jx];
		    iy = ky;
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			y[iy] += temp * a_ref(i__, j);
			iy += *incy;
/* L70: */
		    }
		}
		jx += *incx;
/* L80: */
	    }
	}
    } else {
/*        Form  y := alpha*A'*x + y. */
	jy = ky;
	if (*incx == 1) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		temp = 0.f;
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    temp += a_ref(i__, j) * x[i__];
/* L90: */
		}
		y[jy] += *alpha * temp;
		jy += *incy;
/* L100: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		temp = 0.f;
		ix = kx;
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    temp += a_ref(i__, j) * x[ix];
		    ix += *incx;
/* L110: */
		}
		y[jy] += *alpha * temp;
		jy += *incy;
/* L120: */
	    }
	}
    }
    return 0;
/*     End of SGEMV . */
} /* sgemv_ */
#undef a_ref




/* Subroutine */ int sger_(integer *m, integer *n, real *alpha, real *x, 
	integer *incx, real *y, integer *incy, real *a, integer *lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    /* Local variables */
    static integer info;
    static real temp;
    static integer i__, j, ix, jy, kx;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
/*  Purpose   
    =======   
    SGER   performs the rank 1 operation   
       A := alpha*x*y' + A,   
    where alpha is a scalar, x is an m element vector, y is an n element   
    vector and A is an m by n matrix.   
    Parameters   
    ==========   
    M      - INTEGER.   
             On entry, M specifies the number of rows of the matrix A.   
             M must be at least zero.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry, N specifies the number of columns of the matrix A.   
             N must be at least zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   
    X      - REAL             array of dimension at least   
             ( 1 + ( m - 1 )*abs( INCX ) ).   
             Before entry, the incremented array X must contain the m   
             element vector x.   
             Unchanged on exit.   
    INCX   - INTEGER.   
             On entry, INCX specifies the increment for the elements of   
             X. INCX must not be zero.   
             Unchanged on exit.   
    Y      - REAL             array of dimension at least   
             ( 1 + ( n - 1 )*abs( INCY ) ).   
             Before entry, the incremented array Y must contain the n   
             element vector y.   
             Unchanged on exit.   
    INCY   - INTEGER.   
             On entry, INCY specifies the increment for the elements of   
             Y. INCY must not be zero.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, n ).   
             Before entry, the leading m by n part of the array A must   
             contain the matrix of coefficients. On exit, A is   
             overwritten by the updated matrix.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program. LDA must be at least   
             f2cmax( 1, m ).   
             Unchanged on exit.   
    Level 2 Blas routine.   
    -- Written on 22-October-1986.   
       Jack Dongarra, Argonne National Lab.   
       Jeremy Du Croz, Nag Central Office.   
       Sven Hammarling, Nag Central Office.   
       Richard Hanson, Sandia National Labs.   
       Test the input parameters.   
       Parameter adjustments */
    --x;
    --y;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    /* Function Body */
    info = 0;
    if (*m < 0) {
	info = 1;
    } else if (*n < 0) {
	info = 2;
    } else if (*incx == 0) {
	info = 5;
    } else if (*incy == 0) {
	info = 7;
    } else if (*lda < f2cmax(1,*m)) {
	info = 9;
    }
    if (info != 0) {
	xerbla_("SGER  ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*m == 0 || *n == 0 || *alpha == 0.f) {
	return 0;
    }
/*     Start the operations. In this version the elements of A are   
       accessed sequentially with one pass through A. */
    if (*incy > 0) {
	jy = 1;
    } else {
	jy = 1 - (*n - 1) * *incy;
    }
    if (*incx == 1) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (y[jy] != 0.f) {
		temp = *alpha * y[jy];
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a_ref(i__, j) = a_ref(i__, j) + x[i__] * temp;
/* L10: */
		}
	    }
	    jy += *incy;
/* L20: */
	}
    } else {
	if (*incx > 0) {
	    kx = 1;
	} else {
	    kx = 1 - (*m - 1) * *incx;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (y[jy] != 0.f) {
		temp = *alpha * y[jy];
		ix = kx;
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a_ref(i__, j) = a_ref(i__, j) + x[ix] * temp;
		    ix += *incx;
/* L30: */
		}
	    }
	    jy += *incy;
/* L40: */
	}
    }
    return 0;
/*     End of SGER  . */
} /* sger_ */
#undef a_ref




/* Subroutine */ int slae2_(real *a, real *b, real *c__, real *rt1, real *rt2)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAE2  computes the eigenvalues of a 2-by-2 symmetric matrix   
       [  A   B  ]   
       [  B   C  ].   
    On return, RT1 is the eigenvalue of larger absolute value, and RT2   
    is the eigenvalue of smaller absolute value.   

    Arguments   
    =========   

    A       (input) REAL   
            The (1,1) element of the 2-by-2 matrix.   

    B       (input) REAL   
            The (1,2) and (2,1) elements of the 2-by-2 matrix.   

    C       (input) REAL   
            The (2,2) element of the 2-by-2 matrix.   

    RT1     (output) REAL   
            The eigenvalue of larger absolute value.   

    RT2     (output) REAL   
            The eigenvalue of smaller absolute value.   

    Further Details   
    ===============   

    RT1 is accurate to a few ulps barring over/underflow.   

    RT2 may be inaccurate if there is massive cancellation in the   
    determinant A*C-B*B; higher precision or correctly rounded or   
    correctly truncated arithmetic would be needed to compute RT2   
    accurately in all cases.   

    Overflow is possible only if RT1 is within a factor of 5 of overflow.   
    Underflow is harmless if the input data is 0 or exceeds   
       underflow_threshold / macheps.   

   =====================================================================   


       Compute the eigenvalues */
    /* System generated locals */
    real r__1;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static real acmn, acmx, ab, df, tb, sm, rt, adf;


    sm = *a + *c__;
    df = *a - *c__;
    adf = dabs(df);
    tb = *b + *b;
    ab = dabs(tb);
    if (dabs(*a) > dabs(*c__)) {
	acmx = *a;
	acmn = *c__;
    } else {
	acmx = *c__;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	r__1 = ab / adf;
	rt = adf * sqrt(r__1 * r__1 + 1.f);
    } else if (adf < ab) {
/* Computing 2nd power */
	r__1 = adf / ab;
	rt = ab * sqrt(r__1 * r__1 + 1.f);
    } else {

/*        Includes case AB=ADF=0 */

	rt = ab * sqrt(2.f);
    }
    if (sm < 0.f) {
	*rt1 = (sm - rt) * .5f;

/*        Order of execution important.   
          To get fully accurate smaller eigenvalue,   
          next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.f) {
	*rt1 = (sm + rt) * .5f;

/*        Order of execution important.   
          To get fully accurate smaller eigenvalue,   
          next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5f;
	*rt2 = rt * -.5f;
    }
    return 0;

/*     End of SLAE2 */

} /* slae2_ */




/* Subroutine */ int slaev2_(real *a, real *b, real *c__, real *rt1, real *
	rt2, real *cs1, real *sn1)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAEV2 computes the eigendecomposition of a 2-by-2 symmetric matrix   
       [  A   B  ]   
       [  B   C  ].   
    On return, RT1 is the eigenvalue of larger absolute value, RT2 is the   
    eigenvalue of smaller absolute value, and (CS1,SN1) is the unit right   
    eigenvector for RT1, giving the decomposition   

       [ CS1  SN1 ] [  A   B  ] [ CS1 -SN1 ]  =  [ RT1  0  ]   
       [-SN1  CS1 ] [  B   C  ] [ SN1  CS1 ]     [  0  RT2 ].   

    Arguments   
    =========   

    A       (input) REAL   
            The (1,1) element of the 2-by-2 matrix.   

    B       (input) REAL   
            The (1,2) element and the conjugate of the (2,1) element of   
            the 2-by-2 matrix.   

    C       (input) REAL   
            The (2,2) element of the 2-by-2 matrix.   

    RT1     (output) REAL   
            The eigenvalue of larger absolute value.   

    RT2     (output) REAL   
            The eigenvalue of smaller absolute value.   

    CS1     (output) REAL   
    SN1     (output) REAL   
            The vector (CS1, SN1) is a unit right eigenvector for RT1.   

    Further Details   
    ===============   

    RT1 is accurate to a few ulps barring over/underflow.   

    RT2 may be inaccurate if there is massive cancellation in the   
    determinant A*C-B*B; higher precision or correctly rounded or   
    correctly truncated arithmetic would be needed to compute RT2   
    accurately in all cases.   

    CS1 and SN1 are accurate to a few ulps barring over/underflow.   

    Overflow is possible only if RT1 is within a factor of 5 of overflow.   
    Underflow is harmless if the input data is 0 or exceeds   
       underflow_threshold / macheps.   

   =====================================================================   


       Compute the eigenvalues */
    /* System generated locals */
    real r__1;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static real acmn, acmx, ab, df, cs, ct, tb, sm, tn, rt, adf, acs;
    static integer sgn1, sgn2;


    sm = *a + *c__;
    df = *a - *c__;
    adf = dabs(df);
    tb = *b + *b;
    ab = dabs(tb);
    if (dabs(*a) > dabs(*c__)) {
	acmx = *a;
	acmn = *c__;
    } else {
	acmx = *c__;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	r__1 = ab / adf;
	rt = adf * sqrt(r__1 * r__1 + 1.f);
    } else if (adf < ab) {
/* Computing 2nd power */
	r__1 = adf / ab;
	rt = ab * sqrt(r__1 * r__1 + 1.f);
    } else {

/*        Includes case AB=ADF=0 */

	rt = ab * sqrt(2.f);
    }
    if (sm < 0.f) {
	*rt1 = (sm - rt) * .5f;
	sgn1 = -1;

/*        Order of execution important.   
          To get fully accurate smaller eigenvalue,   
          next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.f) {
	*rt1 = (sm + rt) * .5f;
	sgn1 = 1;

/*        Order of execution important.   
          To get fully accurate smaller eigenvalue,   
          next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5f;
	*rt2 = rt * -.5f;
	sgn1 = 1;
    }

/*     Compute the eigenvector */

    if (df >= 0.f) {
	cs = df + rt;
	sgn2 = 1;
    } else {
	cs = df - rt;
	sgn2 = -1;
    }
    acs = dabs(cs);
    if (acs > ab) {
	ct = -tb / cs;
	*sn1 = 1.f / sqrt(ct * ct + 1.f);
	*cs1 = ct * *sn1;
    } else {
	if (ab == 0.f) {
	    *cs1 = 1.f;
	    *sn1 = 0.f;
	} else {
	    tn = -cs / tb;
	    *cs1 = 1.f / sqrt(tn * tn + 1.f);
	    *sn1 = tn * *cs1;
	}
    }
    if (sgn1 == sgn2) {
	tn = *cs1;
	*cs1 = -(*sn1);
	*sn1 = tn;
    }
    return 0;

/*     End of SLAEV2 */

} /* slaev2_ */



doublereal slamch_(const char *cmach)
{
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMCH determines single precision machine parameters.   

    Arguments   
    =========   

    CMACH   (input) CHARACTER*1   
            Specifies the value to be returned by SLAMCH:   
            = 'E' or 'e',   SLAMCH := eps   
            = 'S' or 's ,   SLAMCH := sfmin   
            = 'B' or 'b',   SLAMCH := base   
            = 'P' or 'p',   SLAMCH := eps*base   
            = 'N' or 'n',   SLAMCH := t   
            = 'R' or 'r',   SLAMCH := rnd   
            = 'M' or 'm',   SLAMCH := emin   
            = 'U' or 'u',   SLAMCH := rmin   
            = 'L' or 'l',   SLAMCH := emax   
            = 'O' or 'o',   SLAMCH := rmax   

            where   

            eps   = relative machine precision   
            sfmin = safe minimum, such that 1/sfmin does not overflow   
            base  = base of the machine   
            prec  = eps*base   
            t     = number of (base) digits in the mantissa   
            rnd   = 1.0 when rounding occurs in addition, 0.0 otherwise   
            emin  = minimum exponent before (gradual) underflow   
            rmin  = underflow threshold - base**(emin-1)   
            emax  = largest exponent before overflow   
            rmax  = overflow threshold  - (base**emax)*(1-eps)   

   ===================================================================== 
*/
/* >>Start of File<<   
       Initialized data */
    static logical first = TRUE_;
    /* System generated locals */
    integer i__1;
    real ret_val;
    /* Builtin functions */
    double pow_ri(real *, integer *);
    /* Local variables */
    static real base;
    static integer beta;
    static real emin, prec, emax;
    static integer imin, imax;
    static logical lrnd;
    static real rmin, rmax, t, rmach;
    extern logical lsame_(const char *, const char *);
    static real small, sfmin;
    extern /* Subroutine */ int slamc2_(integer *, integer *, logical *, real 
	    *, integer *, real *, integer *, real *);
    static integer it;
    static real rnd, eps;



    if (first) {
	first = FALSE_;
	slamc2_(&beta, &it, &lrnd, &eps, &imin, &rmin, &imax, &rmax);
	base = (real) beta;
	t = (real) it;
	if (lrnd) {
	    rnd = 1.f;
	    i__1 = 1 - it;
	    eps = pow_ri(&base, &i__1) / 2;
	} else {
	    rnd = 0.f;
	    i__1 = 1 - it;
	    eps = pow_ri(&base, &i__1);
	}
	prec = eps * base;
	emin = (real) imin;
	emax = (real) imax;
	sfmin = rmin;
	small = 1.f / rmax;
	if (small >= sfmin) {

/*           Use SMALL plus a bit, to avoid the possibility of rou
nding   
             causing overflow when computing  1/sfmin. */

	    sfmin = small * (eps + 1.f);
	}
    }

    if (lsame_(cmach, "E")) {
	rmach = eps;
    } else if (lsame_(cmach, "S")) {
	rmach = sfmin;
    } else if (lsame_(cmach, "B")) {
	rmach = base;
    } else if (lsame_(cmach, "P")) {
	rmach = prec;
    } else if (lsame_(cmach, "N")) {
	rmach = t;
    } else if (lsame_(cmach, "R")) {
	rmach = rnd;
    } else if (lsame_(cmach, "M")) {
	rmach = emin;
    } else if (lsame_(cmach, "U")) {
	rmach = rmin;
    } else if (lsame_(cmach, "L")) {
	rmach = emax;
    } else if (lsame_(cmach, "O")) {
	rmach = rmax;
    }

    ret_val = rmach;
    return ret_val;

/*     End of SLAMCH */

} /* slamch_ */



/* Subroutine */ int slamc1_(integer *beta, integer *t, logical *rnd, logical 
	*ieee1)
{
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC1 determines the machine parameters given by BETA, T, RND, and   
    IEEE1.   

    Arguments   
    =========   

    BETA    (output) INTEGER   
            The base of the machine.   

    T       (output) INTEGER   
            The number of ( BETA ) digits in the mantissa.   

    RND     (output) LOGICAL   
            Specifies whether proper rounding  ( RND = .TRUE. )  or   
            chopping  ( RND = .FALSE. )  occurs in addition. This may not 
  
            be a reliable guide to the way in which the machine performs 
  
            its arithmetic.   

    IEEE1   (output) LOGICAL   
            Specifies whether rounding appears to be done in the IEEE   
            'round to nearest' style.   

    Further Details   
    ===============   

    The routine is based on the routine  ENVRON  by Malcolm and   
    incorporates suggestions by Gentleman and Marovich. See   

       Malcolm M. A. (1972) Algorithms to reveal properties of   
          floating-point arithmetic. Comms. of the ACM, 15, 949-951.   

       Gentleman W. M. and Marovich S. B. (1974) More on algorithms   
          that reveal properties of floating point arithmetic units.   
          Comms. of the ACM, 17, 276-277.   

   ===================================================================== 
*/
    /* Initialized data */
    static logical first = TRUE_;
    /* System generated locals */
    real r__1, r__2;
    /* Local variables */
    static logical lrnd;
    static real a, b, c, f;
    static integer lbeta;
    static real savec;
    static logical lieee1;
    static real t1, t2;
    extern doublereal slamc3_(real *, real *);
    static integer lt;
    static real one, qtr;



    if (first) {
	first = FALSE_;
	one = 1.f;

/*        LBETA,  LIEEE1,  LT and  LRND  are the  local values  of  BE
TA,   
          IEEE1, T and RND.   

          Throughout this routine  we use the function  SLAMC3  to ens
ure   
          that relevant values are  stored and not held in registers, 
 or   
          are not affected by optimizers.   

          Compute  a = 2.0**m  with the  smallest positive integer m s
uch   
          that   

             fl( a + 1.0 ) = a. */

	a = 1.f;
	c = 1.f;

/* +       WHILE( C.EQ.ONE )LOOP */
L10:
	if (c == one) {
	    a *= 2;
	    c = slamc3_(&a, &one);
	    r__1 = -(doublereal)a;
	    c = slamc3_(&c, &r__1);
	    goto L10;
	}
/* +       END WHILE   

          Now compute  b = 2.0**m  with the smallest positive integer 
m   
          such that   

             fl( a + b ) .gt. a. */

	b = 1.f;
	c = slamc3_(&a, &b);

	/**"e2msa.py test.hdf basis.hdf --nbasis=5 --verbose" will occupying 100% 
	 * cpu and never finish on some platform. This printf statement can fix it.
	 * Don't now why. --Grant Tang*/
	printf("\n");	
	
/* +       WHILE( C.EQ.A )LOOP */
L20:
	if (c == a) {
	    b *= 2;
	    c = slamc3_(&a, &b);
	    goto L20;
	}
/* +       END WHILE   

          Now compute the base.  a and c  are neighbouring floating po
int   
          numbers  in the  interval  ( beta**t, beta**( t + 1 ) )  and
 so   
          their difference is beta. Adding 0.25 to c is to ensure that
 it   
          is truncated to beta and not ( beta - 1 ). */

	qtr = one / 4;
	savec = c;
	r__1 = -(doublereal)a;
	c = slamc3_(&c, &r__1);
	lbeta = static_cast<integer>(c + qtr);

/*        Now determine whether rounding or chopping occurs,  by addin
g a   
          bit  less  than  beta/2  and a  bit  more  than  beta/2  to 
 a. */

	b = (real) lbeta;
	r__1 = b / 2;
	r__2 = -(doublereal)b / 100;
	f = slamc3_(&r__1, &r__2);
	c = slamc3_(&f, &a);
	if (c == a) {
	    lrnd = TRUE_;
	} else {
	    lrnd = FALSE_;
	}
	r__1 = b / 2;
	r__2 = b / 100;
	f = slamc3_(&r__1, &r__2);
	c = slamc3_(&f, &a);
	if (lrnd && c == a) {
	    lrnd = FALSE_;
	}

/*        Try and decide whether rounding is done in the  IEEE  'round
 to   
          nearest' style. B/2 is half a unit in the last place of the 
two   
          numbers A and SAVEC. Furthermore, A is even, i.e. has last  
bit   
          zero, and SAVEC is odd. Thus adding B/2 to A should not  cha
nge   
          A, but adding B/2 to SAVEC should change SAVEC. */

	r__1 = b / 2;
	t1 = slamc3_(&r__1, &a);
	r__1 = b / 2;
	t2 = slamc3_(&r__1, &savec);
	lieee1 = t1 == a && t2 > savec && lrnd;

/*        Now find  the  mantissa, t.  It should  be the  integer part
 of   
          log to the base beta of a,  however it is safer to determine
  t   
          by powering.  So we find t as the smallest positive integer 
for   
          which   

             fl( beta**t + 1.0 ) = 1.0. */

	lt = 0;
	a = 1.f;
	c = 1.f;

/* +       WHILE( C.EQ.ONE )LOOP */
L30:
	if (c == one) {
	    ++lt;
	    a *= lbeta;
	    c = slamc3_(&a, &one);
	    r__1 = -(doublereal)a;
	    c = slamc3_(&c, &r__1);
	    goto L30;
	}
/* +       END WHILE */

    }

    *beta = lbeta;
    *t = lt;
    *rnd = lrnd;
    *ieee1 = lieee1;
    return 0;

/*     End of SLAMC1 */

} /* slamc1_ */



/* Subroutine */ int slamc2_(integer *beta, integer *t, logical *rnd, real *
	eps, integer *emin, real *rmin, integer *emax, real *rmax)
{
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC2 determines the machine parameters specified in its argument   
    list.   

    Arguments   
    =========   

    BETA    (output) INTEGER   
            The base of the machine.   

    T       (output) INTEGER   
            The number of ( BETA ) digits in the mantissa.   

    RND     (output) LOGICAL   
            Specifies whether proper rounding  ( RND = .TRUE. )  or   
            chopping  ( RND = .FALSE. )  occurs in addition. This may not 
  
            be a reliable guide to the way in which the machine performs 
  
            its arithmetic.   

    EPS     (output) REAL   
            The smallest positive number such that   

               fl( 1.0 - EPS ) .LT. 1.0,   

            where fl denotes the computed value.   

    EMIN    (output) INTEGER   
            The minimum exponent before (gradual) underflow occurs.   

    RMIN    (output) REAL   
            The smallest normalized number for the machine, given by   
            BASE**( EMIN - 1 ), where  BASE  is the floating point value 
  
            of BETA.   

    EMAX    (output) INTEGER   
            The maximum exponent before overflow occurs.   

    RMAX    (output) REAL   
            The largest positive number for the machine, given by   
            BASE**EMAX * ( 1 - EPS ), where  BASE  is the floating point 
  
            value of BETA.   

    Further Details   
    ===============   

    The computation of  EPS  is based on a routine PARANOIA by   
    W. Kahan of the University of California at Berkeley.   

   ===================================================================== 
*/
    /* Table of constant values */
//    static integer c__1 = 1;	//not used in this function
    
    /* Initialized data */
    static logical first = TRUE_;
    static logical iwarn = FALSE_;
    /* System generated locals */
    integer i__1;
    real r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
    double pow_ri(real *, integer *);
    /* Local variables */
    static logical ieee;
    static real half;
    static logical lrnd;
    static real leps, zero, a, b, c;
    static integer i, lbeta;
    static real rbase;
    static integer lemin, lemax, gnmin;
    static real small;
    static integer gpmin;
    static real third, lrmin, lrmax, sixth;
    static logical lieee1;
    extern /* Subroutine */ int slamc1_(integer *, integer *, logical *, 
	    logical *);
    extern doublereal slamc3_(real *, real *);
    extern /* Subroutine */ int slamc4_(integer *, real *, integer *), 
	    slamc5_(integer *, integer *, integer *, logical *, integer *, 
	    real *);
    static integer lt, ngnmin, ngpmin;
    static real one, two;



    if (first) {
	first = FALSE_;
	zero = 0.f;
	one = 1.f;
	two = 2.f;

/*        LBETA, LT, LRND, LEPS, LEMIN and LRMIN  are the local values
 of   
          BETA, T, RND, EPS, EMIN and RMIN.   

          Throughout this routine  we use the function  SLAMC3  to ens
ure   
          that relevant values are stored  and not held in registers, 
 or   
          are not affected by optimizers.   

          SLAMC1 returns the parameters  LBETA, LT, LRND and LIEEE1. 
*/

	slamc1_(&lbeta, &lt, &lrnd, &lieee1);

/*        Start to find EPS. */

	b = (real) lbeta;
	i__1 = -lt;
	a = pow_ri(&b, &i__1);
	leps = a;

/*        Try some tricks to see whether or not this is the correct  E
PS. */

	b = two / 3;
	half = one / 2;
	r__1 = -(doublereal)half;
	sixth = slamc3_(&b, &r__1);
	third = slamc3_(&sixth, &sixth);
	r__1 = -(doublereal)half;
	b = slamc3_(&third, &r__1);
	b = slamc3_(&b, &sixth);
	b = dabs(b);
	if (b < leps) {
	    b = leps;
	}

	leps = 1.f;

/* +       WHILE( ( LEPS.GT.B ).AND.( B.GT.ZERO ) )LOOP */
L10:
	if (leps > b && b > zero) {
	    leps = b;
	    r__1 = half * leps;
/* Computing 5th power */
	    r__3 = two, r__4 = r__3, r__3 *= r__3;
/* Computing 2nd power */
	    r__5 = leps;
	    r__2 = r__4 * (r__3 * r__3) * (r__5 * r__5);
	    c = slamc3_(&r__1, &r__2);
	    r__1 = -(doublereal)c;
	    c = slamc3_(&half, &r__1);
	    b = slamc3_(&half, &c);
	    r__1 = -(doublereal)b;
	    c = slamc3_(&half, &r__1);
	    b = slamc3_(&half, &c);
	    goto L10;
	}
/* +       END WHILE */

	if (a < leps) {
	    leps = a;
	}

/*        Computation of EPS complete.   

          Now find  EMIN.  Let A = + or - 1, and + or - (1 + BASE**(-3
)).   
          Keep dividing  A by BETA until (gradual) underflow occurs. T
his   
          is detected when we cannot recover the previous A. */

	rbase = one / lbeta;
	small = one;
	for (i = 1; i <= 3; ++i) {
	    r__1 = small * rbase;
	    small = slamc3_(&r__1, &zero);
/* L20: */
	}
	a = slamc3_(&one, &small);
	slamc4_(&ngpmin, &one, &lbeta);
	r__1 = -(doublereal)one;
	slamc4_(&ngnmin, &r__1, &lbeta);
	slamc4_(&gpmin, &a, &lbeta);
	r__1 = -(doublereal)a;
	slamc4_(&gnmin, &r__1, &lbeta);
	ieee = FALSE_;

	if (ngpmin == ngnmin && gpmin == gnmin) {
	    if (ngpmin == gpmin) {
		lemin = ngpmin;
/*            ( Non twos-complement machines, no gradual under
flow;   
                e.g.,  VAX ) */
	    } else if (gpmin - ngpmin == 3) {
		lemin = ngpmin - 1 + lt;
		ieee = TRUE_;
/*            ( Non twos-complement machines, with gradual und
erflow;   
                e.g., IEEE standard followers ) */
	    } else {
		lemin = f2cmin(ngpmin,gpmin);
/*            ( A guess; no known machine ) */
		iwarn = TRUE_;
	    }

	} else if (ngpmin == gpmin && ngnmin == gnmin) {
	    if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1) {
		lemin = f2cmax(ngpmin,ngnmin);
/*            ( Twos-complement machines, no gradual underflow
;   
                e.g., CYBER 205 ) */
	    } else {
		lemin = f2cmin(ngpmin,ngnmin);
/*            ( A guess; no known machine ) */
		iwarn = TRUE_;
	    }

	} else if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1 && gpmin == gnmin)
		 {
	    if (gpmin - f2cmin(ngpmin,ngnmin) == 3) {
		lemin = f2cmax(ngpmin,ngnmin) - 1 + lt;
/*            ( Twos-complement machines with gradual underflo
w;   
                no known machine ) */
	    } else {
		lemin = f2cmin(ngpmin,ngnmin);
/*            ( A guess; no known machine ) */
		iwarn = TRUE_;
	    }

	} else {
/* Computing MIN */
	    i__1 = f2cmin(ngpmin,ngnmin), i__1 = f2cmin(i__1,gpmin);
	    lemin = f2cmin(i__1,gnmin);
/*         ( A guess; no known machine ) */
	    iwarn = TRUE_;
	}
/* **   
   Comment out this if block if EMIN is ok */
	if (iwarn) {
	    first = TRUE_;
	    printf("\n\n WARNING. The value EMIN may be incorrect:- ");
	    printf("EMIN = %8i\n",lemin);
	    printf("If, after inspection, the value EMIN looks acceptable");
            printf("please comment out \n the IF block as marked within the"); 
            printf("code of routine SLAMC2, \n otherwise supply EMIN"); 
            printf("explicitly.\n");
	}
/* **   

          Assume IEEE arithmetic if we found denormalised  numbers abo
ve,   
          or if arithmetic seems to round in the  IEEE style,  determi
ned   
          in routine SLAMC1. A true IEEE machine should have both  thi
ngs   
          true; however, faulty machines may have one or the other. */

	ieee = ieee || lieee1;

/*        Compute  RMIN by successive division by  BETA. We could comp
ute   
          RMIN as BASE**( EMIN - 1 ),  but some machines underflow dur
ing   
          this computation. */

	lrmin = 1.f;
	i__1 = 1 - lemin;
	for (i = 1; i <= 1-lemin; ++i) {
	    r__1 = lrmin * rbase;
	    lrmin = slamc3_(&r__1, &zero);
/* L30: */
	}

/*        Finally, call SLAMC5 to compute EMAX and RMAX. */

	slamc5_(&lbeta, &lt, &lemin, &ieee, &lemax, &lrmax);
    }

    *beta = lbeta;
    *t = lt;
    *rnd = lrnd;
    *eps = leps;
    *emin = lemin;
    *rmin = lrmin;
    *emax = lemax;
    *rmax = lrmax;

    return 0;


/*     End of SLAMC2 */

} /* slamc2_ */



doublereal slamc3_(real *a, real *b)
{
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC3  is intended to force  A  and  B  to be stored prior to doing 
  
    the addition of  A  and  B ,  for use in situations where optimizers 
  
    might hold one of these in a register.   

    Arguments   
    =========   

    A, B    (input) REAL   
            The values A and B.   

   ===================================================================== 
*/
/* >>Start of File<<   
       System generated locals */
    real ret_val;



    ret_val = *a + *b;

    return ret_val;

/*     End of SLAMC3 */

} /* slamc3_ */



/* Subroutine */ int slamc4_(integer *emin, real *start, integer *base)
{
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC4 is a service routine for SLAMC2.   

    Arguments   
    =========   

    EMIN    (output) EMIN   
            The minimum exponent before (gradual) underflow, computed by 
  
            setting A = START and dividing by BASE until the previous A   
            can not be recovered.   

    START   (input) REAL   
            The starting point for determining EMIN.   

    BASE    (input) INTEGER   
            The base of the machine.   

   ===================================================================== 
*/
    /* System generated locals */
    integer i__1;
    real r__1;
    /* Local variables */
    static real zero, a;
    static integer i;
    static real rbase, b1, b2, c1, c2, d1, d2;
    extern doublereal slamc3_(real *, real *);
    static real one;



    a = *start;
    one = 1.f;
    rbase = one / *base;
    zero = 0.f;
    *emin = 1;
    r__1 = a * rbase;
    b1 = slamc3_(&r__1, &zero);
    c1 = a;
    c2 = a;
    d1 = a;
    d2 = a;
/* +    WHILE( ( C1.EQ.A ).AND.( C2.EQ.A ).AND.   
      $       ( D1.EQ.A ).AND.( D2.EQ.A )      )LOOP */
L10:
    if (c1 == a && c2 == a && d1 == a && d2 == a) {
	--(*emin);
	a = b1;
	r__1 = a / *base;
	b1 = slamc3_(&r__1, &zero);
	r__1 = b1 * *base;
	c1 = slamc3_(&r__1, &zero);
	d1 = zero;
	i__1 = *base;
	for (i = 1; i <= *base; ++i) {
	    d1 += b1;
/* L20: */
	}
	r__1 = a * rbase;
	b2 = slamc3_(&r__1, &zero);
	r__1 = b2 / rbase;
	c2 = slamc3_(&r__1, &zero);
	d2 = zero;
	i__1 = *base;
	for (i = 1; i <= *base; ++i) {
	    d2 += b2;
/* L30: */
	}
	goto L10;
    }
/* +    END WHILE */

    return 0;

/*     End of SLAMC4 */

} /* slamc4_ */



/* Subroutine */ int slamc5_(integer *beta, integer *p, integer *emin, 
	logical *ieee, integer *emax, real *rmax)
{
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC5 attempts to compute RMAX, the largest machine floating-point   
    number, without overflow.  It assumes that EMAX + abs(EMIN) sum   
    approximately to a power of 2.  It will fail on machines where this   
    assumption does not hold, for example, the Cyber 205 (EMIN = -28625, 
  
    EMAX = 28718).  It will also fail if the value supplied for EMIN is   
    too large (i.e. too close to zero), probably with overflow.   

    Arguments   
    =========   

    BETA    (input) INTEGER   
            The base of floating-point arithmetic.   

    P       (input) INTEGER   
            The number of base BETA digits in the mantissa of a   
            floating-point value.   

    EMIN    (input) INTEGER   
            The minimum exponent before (gradual) underflow.   

    IEEE    (input) LOGICAL   
            A logical flag specifying whether or not the arithmetic   
            system is thought to comply with the IEEE standard.   

    EMAX    (output) INTEGER   
            The largest exponent before overflow   

    RMAX    (output) REAL   
            The largest machine floating-point number.   

   ===================================================================== 
  


       First compute LEXP and UEXP, two powers of 2 that bound   
       abs(EMIN). We then assume that EMAX + abs(EMIN) will sum   
       approximately to the bound that is closest to abs(EMIN).   
       (EMAX is the exponent of the required number RMAX). */
    /* Table of constant values */
    static real c_b5 = 0.f;
    
    /* System generated locals */
    integer i__1;
    real r__1;
    /* Local variables */
    static integer lexp;
    static real oldy;
    static integer uexp, i;
    static real y, z;
    static integer nbits;
    extern doublereal slamc3_(real *, real *);
    static real recbas;
    static integer exbits, expsum, try__;



    lexp = 1;
    exbits = 1;
L10:
    try__ = lexp << 1;
    if (try__ <= -(*emin)) {
	lexp = try__;
	++exbits;
	goto L10;
    }
    if (lexp == -(*emin)) {
	uexp = lexp;
    } else {
	uexp = try__;
	++exbits;
    }

/*     Now -LEXP is less than or equal to EMIN, and -UEXP is greater   
       than or equal to EMIN. EXBITS is the number of bits needed to   
       store the exponent. */

    if (uexp + *emin > -lexp - *emin) {
	expsum = lexp << 1;
    } else {
	expsum = uexp << 1;
    }

/*     EXPSUM is the exponent range, approximately equal to   
       EMAX - EMIN + 1 . */

    *emax = expsum + *emin - 1;
    nbits = exbits + 1 + *p;

/*     NBITS is the total number of bits needed to store a   
       floating-point number. */

    if (nbits % 2 == 1 && *beta == 2) {

/*        Either there are an odd number of bits used to store a   
          floating-point number, which is unlikely, or some bits are 
  
          not used in the representation of numbers, which is possible
,   
          (e.g. Cray machines) or the mantissa has an implicit bit,   
          (e.g. IEEE machines, Dec Vax machines), which is perhaps the
   
          most likely. We have to assume the last alternative.   
          If this is true, then we need to reduce EMAX by one because 
  
          there must be some way of representing zero in an implicit-b
it   
          system. On machines like Cray, we are reducing EMAX by one 
  
          unnecessarily. */

	--(*emax);
    }

    if (*ieee) {

/*        Assume we are on an IEEE machine which reserves one exponent
   
          for infinity and NaN. */

	--(*emax);
    }

/*     Now create RMAX, the largest machine number, which should   
       be equal to (1.0 - BETA**(-P)) * BETA**EMAX .   

       First compute 1.0 - BETA**(-P), being careful that the   
       result is less than 1.0 . */

    recbas = 1.f / *beta;
    z = *beta - 1.f;
    y = 0.f;
    i__1 = *p;
    for (i = 1; i <= *p; ++i) {
	z *= recbas;
	if (y < 1.f) {
	    oldy = y;
	}
	y = slamc3_(&y, &z);
/* L20: */
    }
    if (y >= 1.f) {
	y = oldy;
    }

/*     Now multiply by BETA**EMAX to get RMAX. */

    i__1 = *emax;
    for (i = 1; i <= *emax; ++i) {
	r__1 = y * *beta;
	y = slamc3_(&r__1, &c_b5);
/* L30: */
    }

    *rmax = y;
    return 0;

/*     End of SLAMC5 */

} /* slamc5_ */




doublereal slanst_(const char *norm, integer *n, real *d__, real *e)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLANST  returns the value of the one norm,  or the Frobenius norm, or   
    the  infinity norm,  or the  element of  largest absolute value  of a   
    real symmetric tridiagonal matrix A.   

    Description   
    ===========   

    SLANST returns the value   

       SLANST = ( f2cmax(abs(A(i,j))), NORM = 'M' or 'm'   
                (   
                ( norm1(A),         NORM = '1', 'O' or 'o'   
                (   
                ( normI(A),         NORM = 'I' or 'i'   
                (   
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'   

    where  norm1  denotes the  one norm of a matrix (maximum column sum),   
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and   
    normF  denotes the  Frobenius norm of a matrix (square root of sum of   
    squares).  Note that  f2cmax(abs(A(i,j)))  is not a  matrix norm.   

    Arguments   
    =========   

    NORM    (input) CHARACTER*1   
            Specifies the value to be returned in SLANST as described   
            above.   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.  When N = 0, SLANST is   
            set to zero.   

    D       (input) REAL array, dimension (N)   
            The diagonal elements of A.   

    E       (input) REAL array, dimension (N-1)   
            The (n-1) sub-diagonal or super-diagonal elements of A.   

    =====================================================================   


       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer i__1;
    real ret_val, r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static integer i__;
    static real scale;
    extern logical lsame_(const char *, const char *);
    static real anorm;
    extern /* Subroutine */ int slassq_(integer *, real *, integer *, real *, 
	    real *);
    static real sum;


    --e;
    --d__;

    /* Function Body */
    if (*n <= 0) {
	anorm = 0.f;
    } else if (lsame_(norm, "M")) {

/*        Find f2cmax(abs(A(i,j))). */

	anorm = (r__1 = d__[*n], dabs(r__1));
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    r__2 = anorm, r__3 = (r__1 = d__[i__], dabs(r__1));
	    anorm = df2cmax(r__2,r__3);
/* Computing MAX */
	    r__2 = anorm, r__3 = (r__1 = e[i__], dabs(r__1));
	    anorm = df2cmax(r__2,r__3);
/* L10: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1' || lsame_(norm, "I")) {

/*        Find norm1(A). */

	if (*n == 1) {
	    anorm = dabs(d__[1]);
	} else {
/* Computing MAX */
	    r__3 = dabs(d__[1]) + dabs(e[1]), r__4 = (r__1 = e[*n - 1], dabs(
		    r__1)) + (r__2 = d__[*n], dabs(r__2));
	    anorm = df2cmax(r__3,r__4);
	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
		r__4 = anorm, r__5 = (r__1 = d__[i__], dabs(r__1)) + (r__2 = 
			e[i__], dabs(r__2)) + (r__3 = e[i__ - 1], dabs(r__3));
		anorm = df2cmax(r__4,r__5);
/* L20: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.f;
	sum = 1.f;
	if (*n > 1) {
	    i__1 = *n - 1;
	    slassq_(&i__1, &e[1], &c__1, &scale, &sum);
	    sum *= 2;
	}
	slassq_(n, &d__[1], &c__1, &scale, &sum);
	anorm = scale * sqrt(sum);
    }

    ret_val = anorm;
    return ret_val;

/*     End of SLANST */

} /* slanst_ */




doublereal slansy_(const char *norm, char *uplo, integer *n, real *a, integer *lda, 
	real *work)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLANSY  returns the value of the one norm,  or the Frobenius norm, or   
    the  infinity norm,  or the  element of  largest absolute value  of a   
    real symmetric matrix A.   

    Description   
    ===========   

    SLANSY returns the value   

       SLANSY = ( f2cmax(abs(A(i,j))), NORM = 'M' or 'm'   
                (   
                ( norm1(A),         NORM = '1', 'O' or 'o'   
                (   
                ( normI(A),         NORM = 'I' or 'i'   
                (   
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'   

    where  norm1  denotes the  one norm of a matrix (maximum column sum),   
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and   
    normF  denotes the  Frobenius norm of a matrix (square root of sum of   
    squares).  Note that  f2cmax(abs(A(i,j)))  is not a  matrix norm.   

    Arguments   
    =========   

    NORM    (input) CHARACTER*1   
            Specifies the value to be returned in SLANSY as described   
            above.   

    UPLO    (input) CHARACTER*1   
            Specifies whether the upper or lower triangular part of the   
            symmetric matrix A is to be referenced.   
            = 'U':  Upper triangular part of A is referenced   
            = 'L':  Lower triangular part of A is referenced   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.  When N = 0, SLANSY is   
            set to zero.   

    A       (input) REAL array, dimension (LDA,N)   
            The symmetric matrix A.  If UPLO = 'U', the leading n by n   
            upper triangular part of A contains the upper triangular part   
            of the matrix A, and the strictly lower triangular part of A   
            is not referenced.  If UPLO = 'L', the leading n by n lower   
            triangular part of A contains the lower triangular part of   
            the matrix A, and the strictly upper triangular part of A is   
            not referenced.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(N,1).   

    WORK    (workspace) REAL array, dimension (LWORK),   
            where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise,   
            WORK is not referenced.   

   =====================================================================   


       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real ret_val, r__1, r__2, r__3;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static real absa;
    static integer i__, j;
    static real scale;
    extern logical lsame_(const char *, const char *);
    static real value;
    extern /* Subroutine */ int slassq_(integer *, real *, integer *, real *, 
	    real *);
    static real sum;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.f;
    } else if (lsame_(norm, "M")) {

/*        Find f2cmax(abs(A(i,j))). */

	value = 0.f;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		    r__2 = value, r__3 = (r__1 = a_ref(i__, j), dabs(r__1));
		    value = df2cmax(r__2,r__3);
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
/* Computing MAX */
		    r__2 = value, r__3 = (r__1 = a_ref(i__, j), dabs(r__1));
		    value = df2cmax(r__2,r__3);
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	value = 0.f;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = 0.f;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    absa = (r__1 = a_ref(i__, j), dabs(r__1));
		    sum += absa;
		    work[i__] += absa;
/* L50: */
		}
		work[j] = sum + (r__1 = a_ref(j, j), dabs(r__1));
/* L60: */
	    }
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
		r__1 = value, r__2 = work[i__];
		value = df2cmax(r__1,r__2);
/* L70: */
	    }
	} else {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] = 0.f;
/* L80: */
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = work[j] + (r__1 = a_ref(j, j), dabs(r__1));
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    absa = (r__1 = a_ref(i__, j), dabs(r__1));
		    sum += absa;
		    work[i__] += absa;
/* L90: */
		}
		value = df2cmax(value,sum);
/* L100: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.f;
	sum = 1.f;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j - 1;
		slassq_(&i__2, &a_ref(1, j), &c__1, &scale, &sum);
/* L110: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		slassq_(&i__2, &a_ref(j + 1, j), &c__1, &scale, &sum);
/* L120: */
	    }
	}
	sum *= 2;
	i__1 = *lda + 1;
	slassq_(n, &a[a_offset], &i__1, &scale, &sum);
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of SLANSY */

} /* slansy_ */

#undef a_ref





doublereal slapy2_(real *x, real *y)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAPY2 returns sqrt(x**2+y**2), taking care not to cause unnecessary   
    overflow.   

    Arguments   
    =========   

    X       (input) REAL   
    Y       (input) REAL   
            X and Y specify the values x and y.   

    ===================================================================== */
    /* System generated locals */
    real ret_val, r__1;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static real xabs, yabs, w, z__;



    xabs = dabs(*x);
    yabs = dabs(*y);
    w = df2cmax(xabs,yabs);
    z__ = df2cmin(xabs,yabs);
    if (z__ == 0.f) {
	ret_val = w;
    } else {
/* Computing 2nd power */
	r__1 = z__ / w;
	ret_val = w * sqrt(r__1 * r__1 + 1.f);
    }
    return ret_val;

/*     End of SLAPY2 */

} /* slapy2_ */




/* Subroutine */ int slarfb_(const char *side, const char *trans, const char *direct, const char *
	storev, integer *m, integer *n, integer *k, real *v, integer *ldv, 
	real *t, integer *ldt, real *c__, integer *ldc, real *work, integer *
	ldwork)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLARFB applies a real block reflector H or its transpose H' to a   
    real m by n matrix C, from either the left or the right.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply H or H' from the Left   
            = 'R': apply H or H' from the Right   

    TRANS   (input) CHARACTER*1   
            = 'N': apply H (No transpose)   
            = 'T': apply H' (Transpose)   

    DIRECT  (input) CHARACTER*1   
            Indicates how H is formed from a product of elementary   
            reflectors   
            = 'F': H = H(1) H(2) . . . H(k) (Forward)   
            = 'B': H = H(k) . . . H(2) H(1) (Backward)   

    STOREV  (input) CHARACTER*1   
            Indicates how the vectors which define the elementary   
            reflectors are stored:   
            = 'C': Columnwise   
            = 'R': Rowwise   

    M       (input) INTEGER   
            The number of rows of the matrix C.   

    N       (input) INTEGER   
            The number of columns of the matrix C.   

    K       (input) INTEGER   
            The order of the matrix T (= the number of elementary   
            reflectors whose product defines the block reflector).   

    V       (input) REAL array, dimension   
                                  (LDV,K) if STOREV = 'C'   
                                  (LDV,M) if STOREV = 'R' and SIDE = 'L'   
                                  (LDV,N) if STOREV = 'R' and SIDE = 'R'   
            The matrix V. See further details.   

    LDV     (input) INTEGER   
            The leading dimension of the array V.   
            If STOREV = 'C' and SIDE = 'L', LDV >= f2cmax(1,M);   
            if STOREV = 'C' and SIDE = 'R', LDV >= f2cmax(1,N);   
            if STOREV = 'R', LDV >= K.   

    T       (input) REAL array, dimension (LDT,K)   
            The triangular k by k matrix T in the representation of the   
            block reflector.   

    LDT     (input) INTEGER   
            The leading dimension of the array T. LDT >= K.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by H*C or H'*C or C*H or C*H'.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDA >= f2cmax(1,M).   

    WORK    (workspace) REAL array, dimension (LDWORK,K)   

    LDWORK  (input) INTEGER   
            The leading dimension of the array WORK.   
            If SIDE = 'L', LDWORK >= f2cmax(1,N);   
            if SIDE = 'R', LDWORK >= f2cmax(1,M).   

    =====================================================================   


       Quick return if possible   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static real c_b14 = 1.f;
    static real c_b25 = -1.f;
    
    /* System generated locals */
    integer c_dim1, c_offset, t_dim1, t_offset, v_dim1, v_offset, work_dim1, 
	    work_offset, i__1, i__2;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *), scopy_(integer *, real *, 
	    integer *, real *, integer *), strmm_(const char *, const char *, const char *, 
	    const char *, integer *, integer *, real *, real *, integer *, real *, 
	    integer *);
    static char transt[1];
#define work_ref(a_1,a_2) work[(a_2)*work_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]
#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]


    v_dim1 = *ldv;
    v_offset = 1 + v_dim1 * 1;
    v -= v_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1 * 1;
    t -= t_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1 * 1;
    work -= work_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (lsame_(trans, "N")) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

    if (lsame_(storev, "C")) {

	if (lsame_(direct, "F")) {

/*           Let  V =  ( V1 )    (first K rows)   
                       ( V2 )   
             where  V1  is unit lower triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in WORK)   

                W := C1' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(n, &c___ref(j, 1), ldc, &work_ref(1, j), &c__1);
/* L10: */
		}

/*              W := W * V1 */

		strmm_("Right", "Lower", "No transpose", "Unit", n, k, &c_b14,
			 &v[v_offset], ldv, &work[work_offset], ldwork);
		if (*m > *k) {

/*                 W := W + C2'*V2 */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "No transpose", n, k, &i__1, &c_b14, &
			    c___ref(*k + 1, 1), ldc, &v_ref(*k + 1, 1), ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Upper", transt, "Non-unit", n, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V * W' */

		if (*m > *k) {

/*                 C2 := C2 - V2 * W' */

		    i__1 = *m - *k;
		    sgemm_("No transpose", "Transpose", &i__1, n, k, &c_b25, &
			    v_ref(*k + 1, 1), ldv, &work[work_offset], ldwork,
			     &c_b14, &c___ref(*k + 1, 1), ldc);
		}

/*              W := W * V1' */

		strmm_("Right", "Lower", "Transpose", "Unit", n, k, &c_b14, &
			v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(j, i__) = c___ref(j, i__) - work_ref(i__, j);
/* L20: */
		    }
/* L30: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )   

                W := C * V  =  (C1*V1 + C2*V2)  (stored in WORK)   

                W := C1 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(m, &c___ref(1, j), &c__1, &work_ref(1, j), &c__1);
/* L40: */
		}

/*              W := W * V1 */

		strmm_("Right", "Lower", "No transpose", "Unit", m, k, &c_b14,
			 &v[v_offset], ldv, &work[work_offset], ldwork);
		if (*n > *k) {

/*                 W := W + C2 * V2 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, k, &i__1, &
			    c_b14, &c___ref(1, *k + 1), ldc, &v_ref(*k + 1, 1)
			    , ldv, &c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Upper", trans, "Non-unit", m, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V' */

		if (*n > *k) {

/*                 C2 := C2 - W * V2' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, &i__1, k, &c_b25, &
			    work[work_offset], ldwork, &v_ref(*k + 1, 1), ldv,
			     &c_b14, &c___ref(1, *k + 1), ldc);
		}

/*              W := W * V1' */

		strmm_("Right", "Lower", "Transpose", "Unit", m, k, &c_b14, &
			v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = c___ref(i__, j) - work_ref(i__, j);
/* L50: */
		    }
/* L60: */
		}
	    }

	} else {

/*           Let  V =  ( V1 )   
                       ( V2 )    (last K rows)   
             where  V2  is unit upper triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in WORK)   

                W := C2' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(n, &c___ref(*m - *k + j, 1), ldc, &work_ref(1, j), 
			    &c__1);
/* L70: */
		}

/*              W := W * V2 */

		strmm_("Right", "Upper", "No transpose", "Unit", n, k, &c_b14,
			 &v_ref(*m - *k + 1, 1), ldv, &work[work_offset], 
			ldwork);
		if (*m > *k) {

/*                 W := W + C1'*V1 */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "No transpose", n, k, &i__1, &c_b14, &
			    c__[c_offset], ldc, &v[v_offset], ldv, &c_b14, &
			    work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V * W' */

		if (*m > *k) {

/*                 C1 := C1 - V1 * W' */

		    i__1 = *m - *k;
		    sgemm_("No transpose", "Transpose", &i__1, n, k, &c_b25, &
			    v[v_offset], ldv, &work[work_offset], ldwork, &
			    c_b14, &c__[c_offset], ldc)
			    ;
		}

/*              W := W * V2' */

		strmm_("Right", "Upper", "Transpose", "Unit", n, k, &c_b14, &
			v_ref(*m - *k + 1, 1), ldv, &work[work_offset], 
			ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(*m - *k + j, i__) = c___ref(*m - *k + j, i__) 
				- work_ref(i__, j);
/* L80: */
		    }
/* L90: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )   

                W := C * V  =  (C1*V1 + C2*V2)  (stored in WORK)   

                W := C2 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(m, &c___ref(1, *n - *k + j), &c__1, &work_ref(1, j)
			    , &c__1);
/* L100: */
		}

/*              W := W * V2 */

		strmm_("Right", "Upper", "No transpose", "Unit", m, k, &c_b14,
			 &v_ref(*n - *k + 1, 1), ldv, &work[work_offset], 
			ldwork);
		if (*n > *k) {

/*                 W := W + C1 * V1 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, k, &i__1, &
			    c_b14, &c__[c_offset], ldc, &v[v_offset], ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V' */

		if (*n > *k) {

/*                 C1 := C1 - W * V1' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, &i__1, k, &c_b25, &
			    work[work_offset], ldwork, &v[v_offset], ldv, &
			    c_b14, &c__[c_offset], ldc)
			    ;
		}

/*              W := W * V2' */

		strmm_("Right", "Upper", "Transpose", "Unit", m, k, &c_b14, &
			v_ref(*n - *k + 1, 1), ldv, &work[work_offset], 
			ldwork);

/*              C2 := C2 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, *n - *k + j) = c___ref(i__, *n - *k + j) 
				- work_ref(i__, j);
/* L110: */
		    }
/* L120: */
		}
	    }
	}

    } else if (lsame_(storev, "R")) {

	if (lsame_(direct, "F")) {

/*           Let  V =  ( V1  V2 )    (V1: first K columns)   
             where  V1  is unit upper triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V'  =  (C1'*V1' + C2'*V2') (stored in WORK)   

                W := C1' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(n, &c___ref(j, 1), ldc, &work_ref(1, j), &c__1);
/* L130: */
		}

/*              W := W * V1' */

		strmm_("Right", "Upper", "Transpose", "Unit", n, k, &c_b14, &
			v[v_offset], ldv, &work[work_offset], ldwork);
		if (*m > *k) {

/*                 W := W + C2'*V2' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", n, k, &i__1, &c_b14, &
			    c___ref(*k + 1, 1), ldc, &v_ref(1, *k + 1), ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Upper", transt, "Non-unit", n, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V' * W' */

		if (*m > *k) {

/*                 C2 := C2 - V2' * W' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", &i__1, n, k, &c_b25, &
			    v_ref(1, *k + 1), ldv, &work[work_offset], ldwork,
			     &c_b14, &c___ref(*k + 1, 1), ldc);
		}

/*              W := W * V1 */

		strmm_("Right", "Upper", "No transpose", "Unit", n, k, &c_b14,
			 &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(j, i__) = c___ref(j, i__) - work_ref(i__, j);
/* L140: */
		    }
/* L150: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )   

                W := C * V'  =  (C1*V1' + C2*V2')  (stored in WORK)   

                W := C1 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(m, &c___ref(1, j), &c__1, &work_ref(1, j), &c__1);
/* L160: */
		}

/*              W := W * V1' */

		strmm_("Right", "Upper", "Transpose", "Unit", m, k, &c_b14, &
			v[v_offset], ldv, &work[work_offset], ldwork);
		if (*n > *k) {

/*                 W := W + C2 * V2' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, k, &i__1, &c_b14, &
			    c___ref(1, *k + 1), ldc, &v_ref(1, *k + 1), ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Upper", trans, "Non-unit", m, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V */

		if (*n > *k) {

/*                 C2 := C2 - W * V2 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, &i__1, k, &
			    c_b25, &work[work_offset], ldwork, &v_ref(1, *k + 
			    1), ldv, &c_b14, &c___ref(1, *k + 1), ldc);
		}

/*              W := W * V1 */

		strmm_("Right", "Upper", "No transpose", "Unit", m, k, &c_b14,
			 &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = c___ref(i__, j) - work_ref(i__, j);
/* L170: */
		    }
/* L180: */
		}

	    }

	} else {

/*           Let  V =  ( V1  V2 )    (V2: last K columns)   
             where  V2  is unit lower triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V'  =  (C1'*V1' + C2'*V2') (stored in WORK)   

                W := C2' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(n, &c___ref(*m - *k + j, 1), ldc, &work_ref(1, j), 
			    &c__1);
/* L190: */
		}

/*              W := W * V2' */

		strmm_("Right", "Lower", "Transpose", "Unit", n, k, &c_b14, &
			v_ref(1, *m - *k + 1), ldv, &work[work_offset], 
			ldwork);
		if (*m > *k) {

/*                 W := W + C1'*V1' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", n, k, &i__1, &c_b14, &
			    c__[c_offset], ldc, &v[v_offset], ldv, &c_b14, &
			    work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V' * W' */

		if (*m > *k) {

/*                 C1 := C1 - V1' * W' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", &i__1, n, k, &c_b25, &v[
			    v_offset], ldv, &work[work_offset], ldwork, &
			    c_b14, &c__[c_offset], ldc);
		}

/*              W := W * V2 */

		strmm_("Right", "Lower", "No transpose", "Unit", n, k, &c_b14,
			 &v_ref(1, *m - *k + 1), ldv, &work[work_offset], 
			ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(*m - *k + j, i__) = c___ref(*m - *k + j, i__) 
				- work_ref(i__, j);
/* L200: */
		    }
/* L210: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )   

                W := C * V'  =  (C1*V1' + C2*V2')  (stored in WORK)   

                W := C2 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    scopy_(m, &c___ref(1, *n - *k + j), &c__1, &work_ref(1, j)
			    , &c__1);
/* L220: */
		}

/*              W := W * V2' */

		strmm_("Right", "Lower", "Transpose", "Unit", m, k, &c_b14, &
			v_ref(1, *n - *k + 1), ldv, &work[work_offset], 
			ldwork);
		if (*n > *k) {

/*                 W := W + C1 * V1' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, k, &i__1, &c_b14, &
			    c__[c_offset], ldc, &v[v_offset], ldv, &c_b14, &
			    work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b14, &t[
			t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V */

		if (*n > *k) {

/*                 C1 := C1 - W * V1 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, &i__1, k, &
			    c_b25, &work[work_offset], ldwork, &v[v_offset], 
			    ldv, &c_b14, &c__[c_offset], ldc);
		}

/*              W := W * V2 */

		strmm_("Right", "Lower", "No transpose", "Unit", m, k, &c_b14,
			 &v_ref(1, *n - *k + 1), ldv, &work[work_offset], 
			ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, *n - *k + j) = c___ref(i__, *n - *k + j) 
				- work_ref(i__, j);
/* L230: */
		    }
/* L240: */
		}

	    }

	}
    }

    return 0;

/*     End of SLARFB */

} /* slarfb_ */

#undef v_ref
#undef c___ref
#undef work_ref





/* Subroutine */ int slarf_(const char *side, integer *m, integer *n, real *v, 
	integer *incv, real *tau, real *c__, integer *ldc, real *work)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLARF applies a real elementary reflector H to a real m by n matrix   
    C, from either the left or the right. H is represented in the form   

          H = I - tau * v * v'   

    where tau is a real scalar and v is a real vector.   

    If tau = 0, then H is taken to be the unit matrix.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': form  H * C   
            = 'R': form  C * H   

    M       (input) INTEGER   
            The number of rows of the matrix C.   

    N       (input) INTEGER   
            The number of columns of the matrix C.   

    V       (input) REAL array, dimension   
                       (1 + (M-1)*abs(INCV)) if SIDE = 'L'   
                    or (1 + (N-1)*abs(INCV)) if SIDE = 'R'   
            The vector v in the representation of H. V is not used if   
            TAU = 0.   

    INCV    (input) INTEGER   
            The increment between elements of v. INCV <> 0.   

    TAU     (input) REAL   
            The value tau in the representation of H.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by the matrix H * C if SIDE = 'L',   
            or C * H if SIDE = 'R'.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= f2cmax(1,M).   

    WORK    (workspace) REAL array, dimension   
                           (N) if SIDE = 'L'   
                        or (M) if SIDE = 'R'   

    =====================================================================   


       Parameter adjustments */
    /* Table of constant values */
    static real c_b4 = 1.f;
    static real c_b5 = 0.f;
    static integer c__1 = 1;
    
    /* System generated locals */
    integer c_dim1, c_offset;
    real r__1;
    /* Local variables */
    extern /* Subroutine */ int sger_(integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *);
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int sgemv_(const char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *);


    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (lsame_(side, "L")) {

/*        Form  H * C */

	if (*tau != 0.f) {

/*           w := C' * v */

	    sgemv_("Transpose", m, n, &c_b4, &c__[c_offset], ldc, &v[1], incv,
		     &c_b5, &work[1], &c__1);

/*           C := C - v * w' */

	    r__1 = -(*tau);
	    sger_(m, n, &r__1, &v[1], incv, &work[1], &c__1, &c__[c_offset], 
		    ldc);
	}
    } else {

/*        Form  C * H */

	if (*tau != 0.f) {

/*           w := C * v */

	    sgemv_("No transpose", m, n, &c_b4, &c__[c_offset], ldc, &v[1], 
		    incv, &c_b5, &work[1], &c__1);

/*           C := C - w * v' */

	    r__1 = -(*tau);
	    sger_(m, n, &r__1, &work[1], &c__1, &v[1], incv, &c__[c_offset], 
		    ldc);
	}
    }
    return 0;

/*     End of SLARF */

} /* slarf_ */




/* Subroutine */ int slarfg_(integer *n, real *alpha, real *x, integer *incx, 
	real *tau)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLARFG generates a real elementary reflector H of order n, such   
    that   

          H * ( alpha ) = ( beta ),   H' * H = I.   
              (   x   )   (   0  )   

    where alpha and beta are scalars, and x is an (n-1)-element real   
    vector. H is represented in the form   

          H = I - tau * ( 1 ) * ( 1 v' ) ,   
                        ( v )   

    where tau is a real scalar and v is a real (n-1)-element   
    vector.   

    If the elements of x are all zero, then tau = 0 and H is taken to be   
    the unit matrix.   

    Otherwise  1 <= tau <= 2.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the elementary reflector.   

    ALPHA   (input/output) REAL   
            On entry, the value alpha.   
            On exit, it is overwritten with the value beta.   

    X       (input/output) REAL array, dimension   
                           (1+(N-2)*abs(INCX))   
            On entry, the vector x.   
            On exit, it is overwritten with the vector v.   

    INCX    (input) INTEGER   
            The increment between elements of X. INCX > 0.   

    TAU     (output) REAL   
            The value tau.   

    =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer i__1;
    real r__1;
    /* Builtin functions */
    double r_sign(real *, real *);
    /* Local variables */
    static real beta;
    extern doublereal snrm2_(integer *, real *, integer *);
    static integer j;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static real xnorm;
    extern doublereal slapy2_(real *, real *), slamch_(const char *);
    static real safmin, rsafmn;
    static integer knt;

    --x;

    /* Function Body */
    if (*n <= 1) {
	*tau = 0.f;
	return 0;
    }

    i__1 = *n - 1;
    xnorm = snrm2_(&i__1, &x[1], incx);

    if (xnorm == 0.f) {

/*        H  =  I */

	*tau = 0.f;
    } else {

/*        general case */

	r__1 = slapy2_(alpha, &xnorm);
	beta = -r_sign(&r__1, alpha);
	safmin = slamch_("S") / slamch_("E");
	if (dabs(beta) < safmin) {

/*           XNORM, BETA may be inaccurate; scale X and recompute them */

	    rsafmn = 1.f / safmin;
	    knt = 0;
L10:
	    ++knt;
	    i__1 = *n - 1;
	    sscal_(&i__1, &rsafmn, &x[1], incx);
	    beta *= rsafmn;
	    *alpha *= rsafmn;
	    if (dabs(beta) < safmin) {
		goto L10;
	    }

/*           New BETA is at most 1, at least SAFMIN */

	    i__1 = *n - 1;
	    xnorm = snrm2_(&i__1, &x[1], incx);
	    r__1 = slapy2_(alpha, &xnorm);
	    beta = -r_sign(&r__1, alpha);
	    *tau = (beta - *alpha) / beta;
	    i__1 = *n - 1;
	    r__1 = 1.f / (*alpha - beta);
	    sscal_(&i__1, &r__1, &x[1], incx);

/*           If ALPHA is subnormal, it may lose relative accuracy */

	    *alpha = beta;
	    i__1 = knt;
	    for (j = 1; j <= i__1; ++j) {
		*alpha *= safmin;
/* L20: */
	    }
	} else {
	    *tau = (beta - *alpha) / beta;
	    i__1 = *n - 1;
	    r__1 = 1.f / (*alpha - beta);
	    sscal_(&i__1, &r__1, &x[1], incx);
	    *alpha = beta;
	}
    }

    return 0;

/*     End of SLARFG */

} /* slarfg_ */




/* Subroutine */ int slarft_(const char *direct, const char *storev, integer *n, integer *
	k, real *v, integer *ldv, real *tau, real *t, integer *ldt)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLARFT forms the triangular factor T of a real block reflector H   
    of order n, which is defined as a product of k elementary reflectors.   

    If DIRECT = 'F', H = H(1) H(2) . . . H(k) and T is upper triangular;   

    If DIRECT = 'B', H = H(k) . . . H(2) H(1) and T is lower triangular.   

    If STOREV = 'C', the vector which defines the elementary reflector   
    H(i) is stored in the i-th column of the array V, and   

       H  =  I - V * T * V'   

    If STOREV = 'R', the vector which defines the elementary reflector   
    H(i) is stored in the i-th row of the array V, and   

       H  =  I - V' * T * V   

    Arguments   
    =========   

    DIRECT  (input) CHARACTER*1   
            Specifies the order in which the elementary reflectors are   
            multiplied to form the block reflector:   
            = 'F': H = H(1) H(2) . . . H(k) (Forward)   
            = 'B': H = H(k) . . . H(2) H(1) (Backward)   

    STOREV  (input) CHARACTER*1   
            Specifies how the vectors which define the elementary   
            reflectors are stored (see also Further Details):   
            = 'C': columnwise   
            = 'R': rowwise   

    N       (input) INTEGER   
            The order of the block reflector H. N >= 0.   

    K       (input) INTEGER   
            The order of the triangular factor T (= the number of   
            elementary reflectors). K >= 1.   

    V       (input/output) REAL array, dimension   
                                 (LDV,K) if STOREV = 'C'   
                                 (LDV,N) if STOREV = 'R'   
            The matrix V. See further details.   

    LDV     (input) INTEGER   
            The leading dimension of the array V.   
            If STOREV = 'C', LDV >= f2cmax(1,N); if STOREV = 'R', LDV >= K.   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i).   

    T       (output) REAL array, dimension (LDT,K)   
            The k by k triangular factor T of the block reflector.   
            If DIRECT = 'F', T is upper triangular; if DIRECT = 'B', T is   
            lower triangular. The rest of the array is not used.   

    LDT     (input) INTEGER   
            The leading dimension of the array T. LDT >= K.   

    Further Details   
    ===============   

    The shape of the matrix V and the storage of the vectors which define   
    the H(i) is best illustrated by the following example with n = 5 and   
    k = 3. The elements equal to 1 are not stored; the corresponding   
    array elements are modified but restored on exit. The rest of the   
    array is not used.   

    DIRECT = 'F' and STOREV = 'C':         DIRECT = 'F' and STOREV = 'R':   

                 V = (  1       )                 V = (  1 v1 v1 v1 v1 )   
                     ( v1  1    )                     (     1 v2 v2 v2 )   
                     ( v1 v2  1 )                     (        1 v3 v3 )   
                     ( v1 v2 v3 )   
                     ( v1 v2 v3 )   

    DIRECT = 'B' and STOREV = 'C':         DIRECT = 'B' and STOREV = 'R':   

                 V = ( v1 v2 v3 )                 V = ( v1 v1  1       )   
                     ( v1 v2 v3 )                     ( v2 v2 v2  1    )   
                     (  1 v2 v3 )                     ( v3 v3 v3 v3  1 )   
                     (     1 v3 )   
                     (        1 )   

    =====================================================================   


       Quick return if possible   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static real c_b8 = 0.f;
    
    /* System generated locals */
    integer t_dim1, t_offset, v_dim1, v_offset, i__1, i__2, i__3;
    real r__1;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int sgemv_(const char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *), strmv_(const char *, const char *, const char *, integer *, real *, 
	    integer *, real *, integer *);
    static real vii;
#define t_ref(a_1,a_2) t[(a_2)*t_dim1 + a_1]
#define v_ref(a_1,a_2) v[(a_2)*v_dim1 + a_1]


    v_dim1 = *ldv;
    v_offset = 1 + v_dim1 * 1;
    v -= v_offset;
    --tau;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1 * 1;
    t -= t_offset;

    /* Function Body */
    if (*n == 0) {
	return 0;
    }

    if (lsame_(direct, "F")) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (tau[i__] == 0.f) {

/*              H(i)  =  I */

		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    t_ref(j, i__) = 0.f;
/* L10: */
		}
	    } else {

/*              general case */

		vii = v_ref(i__, i__);
		v_ref(i__, i__) = 1.f;
		if (lsame_(storev, "C")) {

/*                 T(1:i-1,i) := - tau(i) * V(i:n,1:i-1)' * V(i:n,i) */

		    i__2 = *n - i__ + 1;
		    i__3 = i__ - 1;
		    r__1 = -tau[i__];
		    sgemv_("Transpose", &i__2, &i__3, &r__1, &v_ref(i__, 1), 
			    ldv, &v_ref(i__, i__), &c__1, &c_b8, &t_ref(1, 
			    i__), &c__1);
		} else {

/*                 T(1:i-1,i) := - tau(i) * V(1:i-1,i:n) * V(i,i:n)' */

		    i__2 = i__ - 1;
		    i__3 = *n - i__ + 1;
		    r__1 = -tau[i__];
		    sgemv_("No transpose", &i__2, &i__3, &r__1, &v_ref(1, i__)
			    , ldv, &v_ref(i__, i__), ldv, &c_b8, &t_ref(1, 
			    i__), &c__1);
		}
		v_ref(i__, i__) = vii;

/*              T(1:i-1,i) := T(1:i-1,1:i-1) * T(1:i-1,i) */

		i__2 = i__ - 1;
		strmv_("Upper", "No transpose", "Non-unit", &i__2, &t[
			t_offset], ldt, &t_ref(1, i__), &c__1);
		t_ref(i__, i__) = tau[i__];
	    }
/* L20: */
	}
    } else {
	for (i__ = *k; i__ >= 1; --i__) {
	    if (tau[i__] == 0.f) {

/*              H(i)  =  I */

		i__1 = *k;
		for (j = i__; j <= i__1; ++j) {
		    t_ref(j, i__) = 0.f;
/* L30: */
		}
	    } else {

/*              general case */

		if (i__ < *k) {
		    if (lsame_(storev, "C")) {
			vii = v_ref(*n - *k + i__, i__);
			v_ref(*n - *k + i__, i__) = 1.f;

/*                    T(i+1:k,i) :=   
                              - tau(i) * V(1:n-k+i,i+1:k)' * V(1:n-k+i,i) */

			i__1 = *n - *k + i__;
			i__2 = *k - i__;
			r__1 = -tau[i__];
			sgemv_("Transpose", &i__1, &i__2, &r__1, &v_ref(1, 
				i__ + 1), ldv, &v_ref(1, i__), &c__1, &c_b8, &
				t_ref(i__ + 1, i__), &c__1);
			v_ref(*n - *k + i__, i__) = vii;
		    } else {
			vii = v_ref(i__, *n - *k + i__);
			v_ref(i__, *n - *k + i__) = 1.f;

/*                    T(i+1:k,i) :=   
                              - tau(i) * V(i+1:k,1:n-k+i) * V(i,1:n-k+i)' */

			i__1 = *k - i__;
			i__2 = *n - *k + i__;
			r__1 = -tau[i__];
			sgemv_("No transpose", &i__1, &i__2, &r__1, &v_ref(
				i__ + 1, 1), ldv, &v_ref(i__, 1), ldv, &c_b8, 
				&t_ref(i__ + 1, i__), &c__1);
			v_ref(i__, *n - *k + i__) = vii;
		    }

/*                 T(i+1:k,i) := T(i+1:k,i+1:k) * T(i+1:k,i) */

		    i__1 = *k - i__;
		    strmv_("Lower", "No transpose", "Non-unit", &i__1, &t_ref(
			    i__ + 1, i__ + 1), ldt, &t_ref(i__ + 1, i__), &
			    c__1);
		}
		t_ref(i__, i__) = tau[i__];
	    }
/* L40: */
	}
    }
    return 0;

/*     End of SLARFT */

} /* slarft_ */

#undef v_ref
#undef t_ref





/* Subroutine */ int slartg_(real *f, real *g, real *cs, real *sn, real *r__)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLARTG generate a plane rotation so that   

       [  CS  SN  ]  .  [ F ]  =  [ R ]   where CS**2 + SN**2 = 1.   
       [ -SN  CS  ]     [ G ]     [ 0 ]   

    This is a slower, more accurate version of the BLAS1 routine SROTG,   
    with the following other differences:   
       F and G are unchanged on return.   
       If G=0, then CS=1 and SN=0.   
       If F=0 and (G .ne. 0), then CS=0 and SN=1 without doing any   
          floating point operations (saves work in SBDSQR when   
          there are zeros on the diagonal).   

    If F exceeds G in magnitude, CS will be positive.   

    Arguments   
    =========   

    F       (input) REAL   
            The first component of vector to be rotated.   

    G       (input) REAL   
            The second component of vector to be rotated.   

    CS      (output) REAL   
            The cosine of the rotation.   

    SN      (output) REAL   
            The sine of the rotation.   

    R       (output) REAL   
            The nonzero component of the rotated vector.   

    ===================================================================== */
    /* Initialized data */
    static logical first = TRUE_;
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    /* Builtin functions */
//    double log(doublereal), pow_ri(real *, integer *), sqrt(doublereal);
    double pow_ri(real *, integer *);
    /* Local variables */
    static integer i__;
    static real scale;
    static integer count;
    static real f1, g1, safmn2, safmx2;
    extern doublereal slamch_(const char *);
    static real safmin, eps;



    if (first) {
	first = FALSE_;
	safmin = slamch_("S");
	eps = slamch_("E");
	r__1 = slamch_("B");
	i__1 = (integer) (log(safmin / eps) / log(slamch_("B")) / 
		2.f);
	safmn2 = pow_ri(&r__1, &i__1);
	safmx2 = 1.f / safmn2;
    }
    if (*g == 0.f) {
	*cs = 1.f;
	*sn = 0.f;
	*r__ = *f;
    } else if (*f == 0.f) {
	*cs = 0.f;
	*sn = 1.f;
	*r__ = *g;
    } else {
	f1 = *f;
	g1 = *g;
/* Computing MAX */
	r__1 = dabs(f1), r__2 = dabs(g1);
	scale = df2cmax(r__1,r__2);
	if (scale >= safmx2) {
	    count = 0;
L10:
	    ++count;
	    f1 *= safmn2;
	    g1 *= safmn2;
/* Computing MAX */
	    r__1 = dabs(f1), r__2 = dabs(g1);
	    scale = df2cmax(r__1,r__2);
	    if (scale >= safmx2) {
		goto L10;
	    }
/* Computing 2nd power */
	    r__1 = f1;
/* Computing 2nd power */
	    r__2 = g1;
	    *r__ = sqrt(r__1 * r__1 + r__2 * r__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	    i__1 = count;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*r__ *= safmx2;
/* L20: */
	    }
	} else if (scale <= safmn2) {
	    count = 0;
L30:
	    ++count;
	    f1 *= safmx2;
	    g1 *= safmx2;
/* Computing MAX */
	    r__1 = dabs(f1), r__2 = dabs(g1);
	    scale = df2cmax(r__1,r__2);
	    if (scale <= safmn2) {
		goto L30;
	    }
/* Computing 2nd power */
	    r__1 = f1;
/* Computing 2nd power */
	    r__2 = g1;
	    *r__ = sqrt(r__1 * r__1 + r__2 * r__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	    i__1 = count;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*r__ *= safmn2;
/* L40: */
	    }
	} else {
/* Computing 2nd power */
	    r__1 = f1;
/* Computing 2nd power */
	    r__2 = g1;
	    *r__ = sqrt(r__1 * r__1 + r__2 * r__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	}
	if (dabs(*f) > dabs(*g) && *cs < 0.f) {
	    *cs = -(*cs);
	    *sn = -(*sn);
	    *r__ = -(*r__);
	}
    }
    return 0;

/*     End of SLARTG */

} /* slartg_ */




/* Subroutine */ int slascl_(const char *type__, integer *kl, integer *ku, real *
	cfrom, real *cto, integer *m, integer *n, real *a, integer *lda, 
	integer *info)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLASCL multiplies the M by N real matrix A by the real scalar   
    CTO/CFROM.  This is done without over/underflow as long as the final   
    result CTO*A(I,J)/CFROM does not over/underflow. TYPE specifies that   
    A may be full, upper triangular, lower triangular, upper Hessenberg,   
    or banded.   

    Arguments   
    =========   

    TYPE    (input) CHARACTER*1   
            TYPE indices the storage type of the input matrix.   
            = 'G':  A is a full matrix.   
            = 'L':  A is a lower triangular matrix.   
            = 'U':  A is an upper triangular matrix.   
            = 'H':  A is an upper Hessenberg matrix.   
            = 'B':  A is a symmetric band matrix with lower bandwidth KL   
                    and upper bandwidth KU and with the only the lower   
                    half stored.   
            = 'Q':  A is a symmetric band matrix with lower bandwidth KL   
                    and upper bandwidth KU and with the only the upper   
                    half stored.   
            = 'Z':  A is a band matrix with lower bandwidth KL and upper   
                    bandwidth KU.   

    KL      (input) INTEGER   
            The lower bandwidth of A.  Referenced only if TYPE = 'B',   
            'Q' or 'Z'.   

    KU      (input) INTEGER   
            The upper bandwidth of A.  Referenced only if TYPE = 'B',   
            'Q' or 'Z'.   

    CFROM   (input) REAL   
    CTO     (input) REAL   
            The matrix A is multiplied by CTO/CFROM. A(I,J) is computed   
            without over/underflow if the final result CTO*A(I,J)/CFROM   
            can be represented without over/underflow.  CFROM must be   
            nonzero.   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,M)   
            The matrix to be multiplied by CTO/CFROM.  See TYPE for the   
            storage type.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(1,M).   

    INFO    (output) INTEGER   
            0  - successful exit   
            <0 - if INFO = -i, the i-th argument had an illegal value.   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;
    /* Local variables */
    static logical done;
    static real ctoc;
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer itype, k1, k2, k3, k4;
    static real cfrom1;
    extern doublereal slamch_(const char *);
    static real cfromc;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static real bignum, smlnum, mul, cto1;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    *info = 0;

    if (lsame_(type__, "G")) {
	itype = 0;
    } else if (lsame_(type__, "L")) {
	itype = 1;
    } else if (lsame_(type__, "U")) {
	itype = 2;
    } else if (lsame_(type__, "H")) {
	itype = 3;
    } else if (lsame_(type__, "B")) {
	itype = 4;
    } else if (lsame_(type__, "Q")) {
	itype = 5;
    } else if (lsame_(type__, "Z")) {
	itype = 6;
    } else {
	itype = -1;
    }

    if (itype == -1) {
	*info = -1;
    } else if (*cfrom == 0.f) {
	*info = -4;
    } else if (*m < 0) {
	*info = -6;
    } else if (*n < 0 || itype == 4 && *n != *m || itype == 5 && *n != *m) {
	*info = -7;
    } else if (itype <= 3 && *lda < f2cmax(1,*m)) {
	*info = -9;
    } else if (itype >= 4) {
/* Computing MAX */
	i__1 = *m - 1;
	if (*kl < 0 || *kl > f2cmax(i__1,0)) {
	    *info = -2;
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = *n - 1;
	    if (*ku < 0 || *ku > f2cmax(i__1,0) || (itype == 4 || itype == 5) && 
		    *kl != *ku) {
		*info = -3;
	    } else if (itype == 4 && *lda < *kl + 1 || itype == 5 && *lda < *
		    ku + 1 || itype == 6 && *lda < (*kl << 1) + *ku + 1) {
		*info = -9;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLASCL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *m == 0) {
	return 0;
    }

/*     Get machine parameters */

    smlnum = slamch_("S");
    bignum = 1.f / smlnum;

    cfromc = *cfrom;
    ctoc = *cto;

L10:
    cfrom1 = cfromc * smlnum;
    cto1 = ctoc / bignum;
    if (dabs(cfrom1) > dabs(ctoc) && ctoc != 0.f) {
	mul = smlnum;
	done = FALSE_;
	cfromc = cfrom1;
    } else if (dabs(cto1) > dabs(cfromc)) {
	mul = bignum;
	done = FALSE_;
	ctoc = cto1;
    } else {
	mul = ctoc / cfromc;
	done = TRUE_;
    }

    if (itype == 0) {

/*        Full matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L20: */
	    }
/* L30: */
	}

    } else if (itype == 1) {

/*        Lower triangular matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j; i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L40: */
	    }
/* L50: */
	}

    } else if (itype == 2) {

/*        Upper triangular matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = f2cmin(j,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L60: */
	    }
/* L70: */
	}

    } else if (itype == 3) {

/*        Upper Hessenberg matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = j + 1;
	    i__2 = f2cmin(i__3,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L80: */
	    }
/* L90: */
	}

    } else if (itype == 4) {

/*        Lower half of a symmetric band matrix */

	k3 = *kl + 1;
	k4 = *n + 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = k3, i__4 = k4 - j;
	    i__2 = f2cmin(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L100: */
	    }
/* L110: */
	}

    } else if (itype == 5) {

/*        Upper half of a symmetric band matrix */

	k1 = *ku + 2;
	k3 = *ku + 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__2 = k1 - j;
	    i__3 = k3;
	    for (i__ = f2cmax(i__2,1); i__ <= i__3; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L120: */
	    }
/* L130: */
	}

    } else if (itype == 6) {

/*        Band matrix */

	k1 = *kl + *ku + 2;
	k2 = *kl + 1;
	k3 = (*kl << 1) + *ku + 1;
	k4 = *kl + *ku + 1 + *m;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__3 = k1 - j;
/* Computing MIN */
	    i__4 = k3, i__5 = k4 - j;
	    i__2 = f2cmin(i__4,i__5);
	    for (i__ = f2cmax(i__3,k2); i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j) * mul;
/* L140: */
	    }
/* L150: */
	}

    }

    if (! done) {
	goto L10;
    }

    return 0;

/*     End of SLASCL */

} /* slascl_ */

#undef a_ref





/* Subroutine */ int slaset_(const char *uplo, integer *m, integer *n, real *alpha, 
	real *beta, real *a, integer *lda)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLASET initializes an m-by-n matrix A to BETA on the diagonal and   
    ALPHA on the offdiagonals.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies the part of the matrix A to be set.   
            = 'U':      Upper triangular part is set; the strictly lower   
                        triangular part of A is not changed.   
            = 'L':      Lower triangular part is set; the strictly upper   
                        triangular part of A is not changed.   
            Otherwise:  All of the matrix A is set.   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    ALPHA   (input) REAL   
            The constant to which the offdiagonal elements are to be set.   

    BETA    (input) REAL   
            The constant to which the diagonal elements are to be set.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On exit, the leading m-by-n submatrix of A is set as follows:   

            if UPLO = 'U', A(i,j) = ALPHA, 1<=i<=j-1, 1<=j<=n,   
            if UPLO = 'L', A(i,j) = ALPHA, j+1<=i<=m, 1<=j<=n,   
            otherwise,     A(i,j) = ALPHA, 1<=i<=m, 1<=j<=n, i.ne.j,   

            and, for all UPLO, A(i,i) = BETA, 1<=i<=min(m,n).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(1,M).   

   =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    if (lsame_(uplo, "U")) {

/*        Set the strictly upper triangular or trapezoidal part of the   
          array to ALPHA. */

	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = j - 1;
	    i__2 = f2cmin(i__3,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = *alpha;
/* L10: */
	    }
/* L20: */
	}

    } else if (lsame_(uplo, "L")) {

/*        Set the strictly lower triangular or trapezoidal part of the   
          array to ALPHA. */

	i__1 = f2cmin(*m,*n);
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = *alpha;
/* L30: */
	    }
/* L40: */
	}

    } else {

/*        Set the leading m-by-n submatrix to ALPHA. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = *alpha;
/* L50: */
	    }
/* L60: */
	}
    }

/*     Set the first f2cmin(M,N) diagonal elements to BETA. */

    i__1 = f2cmin(*m,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	a_ref(i__, i__) = *beta;
/* L70: */
    }

    return 0;

/*     End of SLASET */

} /* slaset_ */

#undef a_ref





/* Subroutine */ int slasr_(const char *side, const char *pivot, const char *direct, integer *m,
	 integer *n, real *c__, real *s, real *a, integer *lda)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLASR   performs the transformation   

       A := P*A,   when SIDE = 'L' or 'l'  (  Left-hand side )   

       A := A*P',  when SIDE = 'R' or 'r'  ( Right-hand side )   

    where A is an m by n real matrix and P is an orthogonal matrix,   
    consisting of a sequence of plane rotations determined by the   
    parameters PIVOT and DIRECT as follows ( z = m when SIDE = 'L' or 'l'   
    and z = n when SIDE = 'R' or 'r' ):   

    When  DIRECT = 'F' or 'f'  ( Forward sequence ) then   

       P = P( z - 1 )*...*P( 2 )*P( 1 ),   

    and when DIRECT = 'B' or 'b'  ( Backward sequence ) then   

       P = P( 1 )*P( 2 )*...*P( z - 1 ),   

    where  P( k ) is a plane rotation matrix for the following planes:   

       when  PIVOT = 'V' or 'v'  ( Variable pivot ),   
          the plane ( k, k + 1 )   

       when  PIVOT = 'T' or 't'  ( Top pivot ),   
          the plane ( 1, k + 1 )   

       when  PIVOT = 'B' or 'b'  ( Bottom pivot ),   
          the plane ( k, z )   

    c( k ) and s( k )  must contain the  cosine and sine that define the   
    matrix  P( k ).  The two by two plane rotation part of the matrix   
    P( k ), R( k ), is assumed to be of the form   

       R( k ) = (  c( k )  s( k ) ).   
                ( -s( k )  c( k ) )   

    This version vectorises across rows of the array A when SIDE = 'L'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            Specifies whether the plane rotation matrix P is applied to   
            A on the left or the right.   
            = 'L':  Left, compute A := P*A   
            = 'R':  Right, compute A:= A*P'   

    DIRECT  (input) CHARACTER*1   
            Specifies whether P is a forward or backward sequence of   
            plane rotations.   
            = 'F':  Forward, P = P( z - 1 )*...*P( 2 )*P( 1 )   
            = 'B':  Backward, P = P( 1 )*P( 2 )*...*P( z - 1 )   

    PIVOT   (input) CHARACTER*1   
            Specifies the plane for which P(k) is a plane rotation   
            matrix.   
            = 'V':  Variable pivot, the plane (k,k+1)   
            = 'T':  Top pivot, the plane (1,k+1)   
            = 'B':  Bottom pivot, the plane (k,z)   

    M       (input) INTEGER   
            The number of rows of the matrix A.  If m <= 1, an immediate   
            return is effected.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  If n <= 1, an   
            immediate return is effected.   

    C, S    (input) REAL arrays, dimension   
                    (M-1) if SIDE = 'L'   
                    (N-1) if SIDE = 'R'   
            c(k) and s(k) contain the cosine and sine that define the   
            matrix P(k).  The two by two plane rotation part of the   
            matrix P(k), R(k), is assumed to be of the form   
            R( k ) = (  c( k )  s( k ) ).   
                     ( -s( k )  c( k ) )   

    A       (input/output) REAL array, dimension (LDA,N)   
            The m by n matrix A.  On exit, A is overwritten by P*A if   
            SIDE = 'R' or by A*P' if SIDE = 'L'.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(1,M).   

    =====================================================================   


       Test the input parameters   

       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    /* Local variables */
    static integer info;
    static real temp;
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static real ctemp, stemp;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

    --c__;
    --s;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    info = 0;
    if (! (lsame_(side, "L") || lsame_(side, "R"))) {
	info = 1;
    } else if (! (lsame_(pivot, "V") || lsame_(pivot, 
	    "T") || lsame_(pivot, "B"))) {
	info = 2;
    } else if (! (lsame_(direct, "F") || lsame_(direct, 
	    "B"))) {
	info = 3;
    } else if (*m < 0) {
	info = 4;
    } else if (*n < 0) {
	info = 5;
    } else if (*lda < f2cmax(1,*m)) {
	info = 9;
    }
    if (info != 0) {
	xerbla_("SLASR ", &info);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }
    if (lsame_(side, "L")) {

/*        Form  P * A */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a_ref(j + 1, i__);
			    a_ref(j + 1, i__) = ctemp * temp - stemp * a_ref(
				    j, i__);
			    a_ref(j, i__) = stemp * temp + ctemp * a_ref(j, 
				    i__);
/* L10: */
			}
		    }
/* L20: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a_ref(j + 1, i__);
			    a_ref(j + 1, i__) = ctemp * temp - stemp * a_ref(
				    j, i__);
			    a_ref(j, i__) = stemp * temp + ctemp * a_ref(j, 
				    i__);
/* L30: */
			}
		    }
/* L40: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m;
		for (j = 2; j <= i__1; ++j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a_ref(j, i__);
			    a_ref(j, i__) = ctemp * temp - stemp * a_ref(1, 
				    i__);
			    a_ref(1, i__) = stemp * temp + ctemp * a_ref(1, 
				    i__);
/* L50: */
			}
		    }
/* L60: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m; j >= 2; --j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a_ref(j, i__);
			    a_ref(j, i__) = ctemp * temp - stemp * a_ref(1, 
				    i__);
			    a_ref(1, i__) = stemp * temp + ctemp * a_ref(1, 
				    i__);
/* L70: */
			}
		    }
/* L80: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a_ref(j, i__);
			    a_ref(j, i__) = stemp * a_ref(*m, i__) + ctemp * 
				    temp;
			    a_ref(*m, i__) = ctemp * a_ref(*m, i__) - stemp * 
				    temp;
/* L90: */
			}
		    }
/* L100: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a_ref(j, i__);
			    a_ref(j, i__) = stemp * a_ref(*m, i__) + ctemp * 
				    temp;
			    a_ref(*m, i__) = ctemp * a_ref(*m, i__) - stemp * 
				    temp;
/* L110: */
			}
		    }
/* L120: */
		}
	    }
	}
    } else if (lsame_(side, "R")) {

/*        Form A * P' */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a_ref(i__, j + 1);
			    a_ref(i__, j + 1) = ctemp * temp - stemp * a_ref(
				    i__, j);
			    a_ref(i__, j) = stemp * temp + ctemp * a_ref(i__, 
				    j);
/* L130: */
			}
		    }
/* L140: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a_ref(i__, j + 1);
			    a_ref(i__, j + 1) = ctemp * temp - stemp * a_ref(
				    i__, j);
			    a_ref(i__, j) = stemp * temp + ctemp * a_ref(i__, 
				    j);
/* L150: */
			}
		    }
/* L160: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a_ref(i__, j);
			    a_ref(i__, j) = ctemp * temp - stemp * a_ref(i__, 
				    1);
			    a_ref(i__, 1) = stemp * temp + ctemp * a_ref(i__, 
				    1);
/* L170: */
			}
		    }
/* L180: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n; j >= 2; --j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a_ref(i__, j);
			    a_ref(i__, j) = ctemp * temp - stemp * a_ref(i__, 
				    1);
			    a_ref(i__, 1) = stemp * temp + ctemp * a_ref(i__, 
				    1);
/* L190: */
			}
		    }
/* L200: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a_ref(i__, j);
			    a_ref(i__, j) = stemp * a_ref(i__, *n) + ctemp * 
				    temp;
			    a_ref(i__, *n) = ctemp * a_ref(i__, *n) - stemp * 
				    temp;
/* L210: */
			}
		    }
/* L220: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a_ref(i__, j);
			    a_ref(i__, j) = stemp * a_ref(i__, *n) + ctemp * 
				    temp;
			    a_ref(i__, *n) = ctemp * a_ref(i__, *n) - stemp * 
				    temp;
/* L230: */
			}
		    }
/* L240: */
		}
	    }
	}
    }

    return 0;

/*     End of SLASR */

} /* slasr_ */

#undef a_ref





/* Subroutine */ int slasrt_(const char *id, integer *n, real *d__, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    Sort the numbers in D in increasing order (if ID = 'I') or   
    in decreasing order (if ID = 'D' ).   

    Use Quick Sort, reverting to Insertion sort on arrays of   
    size <= 20. Dimension of STACK limits N to about 2**32.   

    Arguments   
    =========   

    ID      (input) CHARACTER*1   
            = 'I': sort D in increasing order;   
            = 'D': sort D in decreasing order.   

    N       (input) INTEGER   
            The length of the array D.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the array to be sorted.   
            On exit, D has been sorted into increasing order   
            (D(1) <= ... <= D(N) ) or into decreasing order   
            (D(1) >= ... >= D(N) ), depending on ID.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input paramters.   

       Parameter adjustments */
    /* System generated locals */
    integer i__1, i__2;
    /* Local variables */
    static integer endd, i__, j;
    extern logical lsame_(const char *, const char *);
    static integer stack[64]	/* was [2][32] */;
    static real dmnmx, d1, d2, d3;
    static integer start;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static integer stkpnt, dir;
    static real tmp;
#define stack_ref(a_1,a_2) stack[(a_2)*2 + a_1 - 3]

    --d__;

    /* Function Body */
    *info = 0;
    dir = -1;
    if (lsame_(id, "D")) {
	dir = 0;
    } else if (lsame_(id, "I")) {
	dir = 1;
    }
    if (dir == -1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLASRT", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

    stkpnt = 1;
    stack_ref(1, 1) = 1;
    stack_ref(2, 1) = *n;
L10:
    start = stack_ref(1, stkpnt);
    endd = stack_ref(2, stkpnt);
    --stkpnt;
    if (endd - start <= 20 && endd - start > 0) {

/*        Do Insertion sort on D( START:ENDD ) */

	if (dir == 0) {

/*           Sort into decreasing order */

	    i__1 = endd;
	    for (i__ = start + 1; i__ <= i__1; ++i__) {
		i__2 = start + 1;
		for (j = i__; j >= i__2; --j) {
		    if (d__[j] > d__[j - 1]) {
			dmnmx = d__[j];
			d__[j] = d__[j - 1];
			d__[j - 1] = dmnmx;
		    } else {
			goto L30;
		    }
/* L20: */
		}
L30:
		;
	    }

	} else {

/*           Sort into increasing order */

	    i__1 = endd;
	    for (i__ = start + 1; i__ <= i__1; ++i__) {
		i__2 = start + 1;
		for (j = i__; j >= i__2; --j) {
		    if (d__[j] < d__[j - 1]) {
			dmnmx = d__[j];
			d__[j] = d__[j - 1];
			d__[j - 1] = dmnmx;
		    } else {
			goto L50;
		    }
/* L40: */
		}
L50:
		;
	    }

	}

    } else if (endd - start > 20) {

/*        Partition D( START:ENDD ) and stack parts, largest one first   

          Choose partition entry as median of 3 */

	d1 = d__[start];
	d2 = d__[endd];
	i__ = (start + endd) / 2;
	d3 = d__[i__];
	if (d1 < d2) {
	    if (d3 < d1) {
		dmnmx = d1;
	    } else if (d3 < d2) {
		dmnmx = d3;
	    } else {
		dmnmx = d2;
	    }
	} else {
	    if (d3 < d2) {
		dmnmx = d2;
	    } else if (d3 < d1) {
		dmnmx = d3;
	    } else {
		dmnmx = d1;
	    }
	}

	if (dir == 0) {

/*           Sort into decreasing order */

	    i__ = start - 1;
	    j = endd + 1;
L60:
L70:
	    --j;
	    if (d__[j] < dmnmx) {
		goto L70;
	    }
L80:
	    ++i__;
	    if (d__[i__] > dmnmx) {
		goto L80;
	    }
	    if (i__ < j) {
		tmp = d__[i__];
		d__[i__] = d__[j];
		d__[j] = tmp;
		goto L60;
	    }
	    if (j - start > endd - j - 1) {
		++stkpnt;
		stack_ref(1, stkpnt) = start;
		stack_ref(2, stkpnt) = j;
		++stkpnt;
		stack_ref(1, stkpnt) = j + 1;
		stack_ref(2, stkpnt) = endd;
	    } else {
		++stkpnt;
		stack_ref(1, stkpnt) = j + 1;
		stack_ref(2, stkpnt) = endd;
		++stkpnt;
		stack_ref(1, stkpnt) = start;
		stack_ref(2, stkpnt) = j;
	    }
	} else {

/*           Sort into increasing order */

	    i__ = start - 1;
	    j = endd + 1;
L90:
L100:
	    --j;
	    if (d__[j] > dmnmx) {
		goto L100;
	    }
L110:
	    ++i__;
	    if (d__[i__] < dmnmx) {
		goto L110;
	    }
	    if (i__ < j) {
		tmp = d__[i__];
		d__[i__] = d__[j];
		d__[j] = tmp;
		goto L90;
	    }
	    if (j - start > endd - j - 1) {
		++stkpnt;
		stack_ref(1, stkpnt) = start;
		stack_ref(2, stkpnt) = j;
		++stkpnt;
		stack_ref(1, stkpnt) = j + 1;
		stack_ref(2, stkpnt) = endd;
	    } else {
		++stkpnt;
		stack_ref(1, stkpnt) = j + 1;
		stack_ref(2, stkpnt) = endd;
		++stkpnt;
		stack_ref(1, stkpnt) = start;
		stack_ref(2, stkpnt) = j;
	    }
	}
    }
    if (stkpnt > 0) {
	goto L10;
    }
    return 0;

/*     End of SLASRT */

} /* slasrt_ */

#undef stack_ref





/* Subroutine */ int slassq_(integer *n, real *x, integer *incx, real *scale, 
	real *sumsq)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SLASSQ  returns the values  scl  and  smsq  such that   

       ( scl**2 )*smsq = x( 1 )**2 +...+ x( n )**2 + ( scale**2 )*sumsq,   

    where  x( i ) = X( 1 + ( i - 1 )*INCX ). The value of  sumsq  is   
    assumed to be non-negative and  scl  returns the value   

       scl = f2cmax( scale, abs( x( i ) ) ).   

    scale and sumsq must be supplied in SCALE and SUMSQ and   
    scl and smsq are overwritten on SCALE and SUMSQ respectively.   

    The routine makes only one pass through the vector x.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The number of elements to be used from the vector X.   

    X       (input) REAL array, dimension (N)   
            The vector for which a scaled sum of squares is computed.   
               x( i )  = X( 1 + ( i - 1 )*INCX ), 1 <= i <= n.   

    INCX    (input) INTEGER   
            The increment between successive values of the vector X.   
            INCX > 0.   

    SCALE   (input/output) REAL   
            On entry, the value  scale  in the equation above.   
            On exit, SCALE is overwritten with  scl , the scaling factor   
            for the sum of squares.   

    SUMSQ   (input/output) REAL   
            On entry, the value  sumsq  in the equation above.   
            On exit, SUMSQ is overwritten with  smsq , the basic sum of   
            squares from which  scl  has been factored out.   

   =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer i__1, i__2;
    real r__1;
    /* Local variables */
    static real absxi;
    static integer ix;

    --x;

    /* Function Body */
    if (*n > 0) {
	i__1 = (*n - 1) * *incx + 1;
	i__2 = *incx;
	for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
	    if (x[ix] != 0.f) {
		absxi = (r__1 = x[ix], dabs(r__1));
		if (*scale < absxi) {
/* Computing 2nd power */
		    r__1 = *scale / absxi;
		    *sumsq = *sumsq * (r__1 * r__1) + 1;
		    *scale = absxi;
		} else {
/* Computing 2nd power */
		    r__1 = absxi / *scale;
		    *sumsq += r__1 * r__1;
		}
	    }
/* L10: */
	}
    }
    return 0;

/*     End of SLASSQ */

} /* slassq_ */




/* Subroutine */ int slatrd_(char *uplo, integer *n, integer *nb, real *a, 
	integer *lda, real *e, real *tau, real *w, integer *ldw)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLATRD reduces NB rows and columns of a real symmetric matrix A to   
    symmetric tridiagonal form by an orthogonal similarity   
    transformation Q' * A * Q, and returns the matrices V and W which are   
    needed to apply the transformation to the unreduced part of A.   

    If UPLO = 'U', SLATRD reduces the last NB rows and columns of a   
    matrix, of which the upper triangle is supplied;   
    if UPLO = 'L', SLATRD reduces the first NB rows and columns of a   
    matrix, of which the lower triangle is supplied.   

    This is an auxiliary routine called by SSYTRD.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER   
            Specifies whether the upper or lower triangular part of the   
            symmetric matrix A is stored:   
            = 'U': Upper triangular   
            = 'L': Lower triangular   

    N       (input) INTEGER   
            The order of the matrix A.   

    NB      (input) INTEGER   
            The number of rows and columns to be reduced.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading   
            n-by-n upper triangular part of A contains the upper   
            triangular part of the matrix A, and the strictly lower   
            triangular part of A is not referenced.  If UPLO = 'L', the   
            leading n-by-n lower triangular part of A contains the lower   
            triangular part of the matrix A, and the strictly upper   
            triangular part of A is not referenced.   
            On exit:   
            if UPLO = 'U', the last NB columns have been reduced to   
              tridiagonal form, with the diagonal elements overwriting   
              the diagonal elements of A; the elements above the diagonal   
              with the array TAU, represent the orthogonal matrix Q as a   
              product of elementary reflectors;   
            if UPLO = 'L', the first NB columns have been reduced to   
              tridiagonal form, with the diagonal elements overwriting   
              the diagonal elements of A; the elements below the diagonal   
              with the array TAU, represent the  orthogonal matrix Q as a   
              product of elementary reflectors.   
            See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= (1,N).   

    E       (output) REAL array, dimension (N-1)   
            If UPLO = 'U', E(n-nb:n-1) contains the superdiagonal   
            elements of the last NB columns of the reduced matrix;   
            if UPLO = 'L', E(1:nb) contains the subdiagonal elements of   
            the first NB columns of the reduced matrix.   

    TAU     (output) REAL array, dimension (N-1)   
            The scalar factors of the elementary reflectors, stored in   
            TAU(n-nb:n-1) if UPLO = 'U', and in TAU(1:nb) if UPLO = 'L'.   
            See Further Details.   

    W       (output) REAL array, dimension (LDW,NB)   
            The n-by-nb matrix W required to update the unreduced part   
            of A.   

    LDW     (input) INTEGER   
            The leading dimension of the array W. LDW >= f2cmax(1,N).   

    Further Details   
    ===============   

    If UPLO = 'U', the matrix Q is represented as a product of elementary   
    reflectors   

       Q = H(n) H(n-1) . . . H(n-nb+1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(i:n) = 0 and v(i-1) = 1; v(1:i-1) is stored on exit in A(1:i-1,i),   
    and tau in TAU(i-1).   

    If UPLO = 'L', the matrix Q is represented as a product of elementary   
    reflectors   

       Q = H(1) H(2) . . . H(nb).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i) = 0 and v(i+1) = 1; v(i+1:n) is stored on exit in A(i+1:n,i),   
    and tau in TAU(i).   

    The elements of the vectors v together form the n-by-nb matrix V   
    which is needed, with W, to apply the transformation to the unreduced   
    part of the matrix, using a symmetric rank-2k update of the form:   
    A := A - V*W' - W*V'.   

    The contents of A on exit are illustrated by the following examples   
    with n = 5 and nb = 2:   

    if UPLO = 'U':                       if UPLO = 'L':   

      (  a   a   a   v4  v5 )              (  d                  )   
      (      a   a   v4  v5 )              (  1   d              )   
      (          a   1   v5 )              (  v1  1   a          )   
      (              d   1  )              (  v1  v2  a   a      )   
      (                  d  )              (  v1  v2  a   a   a  )   

    where d denotes a diagonal element of the reduced matrix, a denotes   
    an element of the original matrix that is unchanged, and vi denotes   
    an element of the vector defining H(i).   

    =====================================================================   


       Quick return if possible   

       Parameter adjustments */
    /* Table of constant values */
    static real c_b5 = -1.f;
    static real c_b6 = 1.f;
    static integer c__1 = 1;
    static real c_b16 = 0.f;
    
    /* System generated locals */
    integer a_dim1, a_offset, w_dim1, w_offset, i__1, i__2, i__3;
    /* Local variables */
    extern doublereal sdot_(integer *, real *, integer *, real *, integer *);
    static integer i__;
    static real alpha;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    sgemv_(const char *, integer *, integer *, real *, real *, integer *, 
	    real *, integer *, real *, real *, integer *), saxpy_(
	    integer *, real *, real *, integer *, real *, integer *), ssymv_(
	    const char *, integer *, real *, real *, integer *, real *, integer *, 
	    real *, real *, integer *);
    static integer iw;
    extern /* Subroutine */ int slarfg_(integer *, real *, real *, integer *, 
	    real *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define w_ref(a_1,a_2) w[(a_2)*w_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --e;
    --tau;
    w_dim1 = *ldw;
    w_offset = 1 + w_dim1 * 1;
    w -= w_offset;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

    if (lsame_(uplo, "U")) {

/*        Reduce last NB columns of upper triangle */

	i__1 = *n - *nb + 1;
	for (i__ = *n; i__ >= i__1; --i__) {
	    iw = i__ - *n + *nb;
	    if (i__ < *n) {

/*              Update A(1:i,i) */

		i__2 = *n - i__;
		sgemv_("No transpose", &i__, &i__2, &c_b5, &a_ref(1, i__ + 1),
			 lda, &w_ref(i__, iw + 1), ldw, &c_b6, &a_ref(1, i__),
			 &c__1);
		i__2 = *n - i__;
		sgemv_("No transpose", &i__, &i__2, &c_b5, &w_ref(1, iw + 1), 
			ldw, &a_ref(i__, i__ + 1), lda, &c_b6, &a_ref(1, i__),
			 &c__1);
	    }
	    if (i__ > 1) {

/*              Generate elementary reflector H(i) to annihilate   
                A(1:i-2,i) */

		i__2 = i__ - 1;
		slarfg_(&i__2, &a_ref(i__ - 1, i__), &a_ref(1, i__), &c__1, &
			tau[i__ - 1]);
		e[i__ - 1] = a_ref(i__ - 1, i__);
		a_ref(i__ - 1, i__) = 1.f;

/*              Compute W(1:i-1,i) */

		i__2 = i__ - 1;
		ssymv_("Upper", &i__2, &c_b6, &a[a_offset], lda, &a_ref(1, 
			i__), &c__1, &c_b16, &w_ref(1, iw), &c__1);
		if (i__ < *n) {
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    sgemv_("Transpose", &i__2, &i__3, &c_b6, &w_ref(1, iw + 1)
			    , ldw, &a_ref(1, i__), &c__1, &c_b16, &w_ref(i__ 
			    + 1, iw), &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(1, i__ 
			    + 1), lda, &w_ref(i__ + 1, iw), &c__1, &c_b6, &
			    w_ref(1, iw), &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    sgemv_("Transpose", &i__2, &i__3, &c_b6, &a_ref(1, i__ + 
			    1), lda, &a_ref(1, i__), &c__1, &c_b16, &w_ref(
			    i__ + 1, iw), &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    sgemv_("No transpose", &i__2, &i__3, &c_b5, &w_ref(1, iw 
			    + 1), ldw, &w_ref(i__ + 1, iw), &c__1, &c_b6, &
			    w_ref(1, iw), &c__1);
		}
		i__2 = i__ - 1;
		sscal_(&i__2, &tau[i__ - 1], &w_ref(1, iw), &c__1);
		i__2 = i__ - 1;
		alpha = tau[i__ - 1] * -.5f * sdot_(&i__2, &w_ref(1, iw), &
			c__1, &a_ref(1, i__), &c__1);
		i__2 = i__ - 1;
		saxpy_(&i__2, &alpha, &a_ref(1, i__), &c__1, &w_ref(1, iw), &
			c__1);
	    }

/* L10: */
	}
    } else {

/*        Reduce first NB columns of lower triangle */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i:n,i) */

	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(i__, 1), lda, &
		    w_ref(i__, 1), ldw, &c_b6, &a_ref(i__, i__), &c__1);
	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    sgemv_("No transpose", &i__2, &i__3, &c_b5, &w_ref(i__, 1), ldw, &
		    a_ref(i__, 1), lda, &c_b6, &a_ref(i__, i__), &c__1);
	    if (i__ < *n) {

/*              Generate elementary reflector H(i) to annihilate   
                A(i+2:n,i)   

   Computing MIN */
		i__2 = i__ + 2;
		i__3 = *n - i__;
		slarfg_(&i__3, &a_ref(i__ + 1, i__), &a_ref(f2cmin(i__2,*n), i__)
			, &c__1, &tau[i__]);
		e[i__] = a_ref(i__ + 1, i__);
		a_ref(i__ + 1, i__) = 1.f;

/*              Compute W(i+1:n,i) */

		i__2 = *n - i__;
		ssymv_("Lower", &i__2, &c_b6, &a_ref(i__ + 1, i__ + 1), lda, &
			a_ref(i__ + 1, i__), &c__1, &c_b16, &w_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		sgemv_("Transpose", &i__2, &i__3, &c_b6, &w_ref(i__ + 1, 1), 
			ldw, &a_ref(i__ + 1, i__), &c__1, &c_b16, &w_ref(1, 
			i__), &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(i__ + 1, 1)
			, lda, &w_ref(1, i__), &c__1, &c_b6, &w_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		sgemv_("Transpose", &i__2, &i__3, &c_b6, &a_ref(i__ + 1, 1), 
			lda, &a_ref(i__ + 1, i__), &c__1, &c_b16, &w_ref(1, 
			i__), &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b5, &w_ref(i__ + 1, 1)
			, ldw, &w_ref(1, i__), &c__1, &c_b6, &w_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *n - i__;
		sscal_(&i__2, &tau[i__], &w_ref(i__ + 1, i__), &c__1);
		i__2 = *n - i__;
		alpha = tau[i__] * -.5f * sdot_(&i__2, &w_ref(i__ + 1, i__), &
			c__1, &a_ref(i__ + 1, i__), &c__1);
		i__2 = *n - i__;
		saxpy_(&i__2, &alpha, &a_ref(i__ + 1, i__), &c__1, &w_ref(i__ 
			+ 1, i__), &c__1);
	    }

/* L20: */
	}
    }

    return 0;

/*     End of SLATRD */

} /* slatrd_ */

#undef w_ref
#undef a_ref





doublereal snrm2_(integer *n, real *x, integer *incx)
{
/*        The following loop is equivalent to this call to the LAPACK   
          auxiliary routine:   
          CALL SLASSQ( N, X, INCX, SCALE, SSQ ) */
    /* System generated locals */
    integer i__1, i__2;
    real ret_val, r__1;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static real norm, scale, absxi;
    static integer ix;
    static real ssq;
/*  SNRM2 returns the euclidean norm of a vector via the function   
    name, so that   
       SNRM2 := sqrt( x'*x )   
    -- This version written on 25-October-1982.   
       Modified on 14-October-1993 to inline the call to SLASSQ.   
       Sven Hammarling, Nag Ltd.   
       Parameter adjustments */
    --x;
    /* Function Body */
    if (*n < 1 || *incx < 1) {
	norm = 0.f;
    } else if (*n == 1) {
	norm = dabs(x[1]);
    } else {
	scale = 0.f;
	ssq = 1.f;


	i__1 = (*n - 1) * *incx + 1;
	i__2 = *incx;
	for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
	    if (x[ix] != 0.f) {
		absxi = (r__1 = x[ix], dabs(r__1));
		if (scale < absxi) {
/* Computing 2nd power */
		    r__1 = scale / absxi;
		    ssq = ssq * (r__1 * r__1) + 1.f;
		    scale = absxi;
		} else {
/* Computing 2nd power */
		    r__1 = absxi / scale;
		    ssq += r__1 * r__1;
		}
	    }
/* L10: */
	}
	norm = scale * sqrt(ssq);
    }

    ret_val = norm;
    return ret_val;

/*     End of SNRM2. */

} /* snrm2_ */




/* Subroutine */ int sorg2l_(integer *m, integer *n, integer *k, real *a, 
	integer *lda, real *tau, real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SORG2L generates an m by n real matrix Q with orthonormal columns,   
    which is defined as the last n columns of a product of k elementary   
    reflectors of order m   

          Q  =  H(k) . . . H(2) H(1)   

    as returned by SGEQLF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. M >= N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the   
            matrix Q. N >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the (n-k+i)-th column must contain the vector which   
            defines the elementary reflector H(i), for i = 1,2,...,k, as   
            returned by SGEQLF in the last k columns of its array   
            argument A.   
            On exit, the m by n matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= f2cmax(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQLF.   

    WORK    (workspace) REAL array, dimension (N)   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument has an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1;
    /* Local variables */
    static integer i__, j, l;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    slarf_(const char *, integer *, integer *, real *, integer *, real *, 
	    real *, integer *, real *);
    static integer ii;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORG2L", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

/*     Initialise columns 1:n-k to columns of the unit matrix */

    i__1 = *n - *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (l = 1; l <= i__2; ++l) {
	    a_ref(l, j) = 0.f;
/* L10: */
	}
	a_ref(*m - *n + j, j) = 1.f;
/* L20: */
    }

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ii = *n - *k + i__;

/*        Apply H(i) to A(1:m-k+i,1:n-k+i) from the left */

	a_ref(*m - *n + ii, ii) = 1.f;
	i__2 = *m - *n + ii;
	i__3 = ii - 1;
	slarf_("Left", &i__2, &i__3, &a_ref(1, ii), &c__1, &tau[i__], &a[
		a_offset], lda, &work[1]);
	i__2 = *m - *n + ii - 1;
	r__1 = -tau[i__];
	sscal_(&i__2, &r__1, &a_ref(1, ii), &c__1);
	a_ref(*m - *n + ii, ii) = 1.f - tau[i__];

/*        Set A(m-k+i+1:m,n-k+i) to zero */

	i__2 = *m;
	for (l = *m - *n + ii + 1; l <= i__2; ++l) {
	    a_ref(l, ii) = 0.f;
/* L30: */
	}
/* L40: */
    }
    return 0;

/*     End of SORG2L */

} /* sorg2l_ */

#undef a_ref





/* Subroutine */ int sorg2r_(integer *m, integer *n, integer *k, real *a, 
	integer *lda, real *tau, real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SORG2R generates an m by n real matrix Q with orthonormal columns,   
    which is defined as the first n columns of a product of k elementary   
    reflectors of order m   

          Q  =  H(1) H(2) . . . H(k)   

    as returned by SGEQRF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. M >= N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the   
            matrix Q. N >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the i-th column must contain the vector which   
            defines the elementary reflector H(i), for i = 1,2,...,k, as   
            returned by SGEQRF in the first k columns of its array   
            argument A.   
            On exit, the m-by-n matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= f2cmax(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQRF.   

    WORK    (workspace) REAL array, dimension (N)   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument has an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real r__1;
    /* Local variables */
    static integer i__, j, l;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    slarf_(const char *, integer *, integer *, real *, integer *, real *, 
	    real *, integer *, real *), xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORG2R", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

/*     Initialise columns k+1:n to columns of the unit matrix */

    i__1 = *n;
    for (j = *k + 1; j <= i__1; ++j) {
	i__2 = *m;
	for (l = 1; l <= i__2; ++l) {
	    a_ref(l, j) = 0.f;
/* L10: */
	}
	a_ref(j, j) = 1.f;
/* L20: */
    }

    for (i__ = *k; i__ >= 1; --i__) {

/*        Apply H(i) to A(i:m,i:n) from the left */

	if (i__ < *n) {
	    a_ref(i__, i__) = 1.f;
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__;
	    slarf_("Left", &i__1, &i__2, &a_ref(i__, i__), &c__1, &tau[i__], &
		    a_ref(i__, i__ + 1), lda, &work[1]);
	}
	if (i__ < *m) {
	    i__1 = *m - i__;
	    r__1 = -tau[i__];
	    sscal_(&i__1, &r__1, &a_ref(i__ + 1, i__), &c__1);
	}
	a_ref(i__, i__) = 1.f - tau[i__];

/*        Set A(1:i-1,i) to zero */

	i__1 = i__ - 1;
	for (l = 1; l <= i__1; ++l) {
	    a_ref(l, i__) = 0.f;
/* L30: */
	}
/* L40: */
    }
    return 0;

/*     End of SORG2R */

} /* sorg2r_ */

#undef a_ref





/* Subroutine */ int sorgql_(integer *m, integer *n, integer *k, real *a, 
	integer *lda, real *tau, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORGQL generates an M-by-N real matrix Q with orthonormal columns,   
    which is defined as the last N columns of a product of K elementary   
    reflectors of order M   

          Q  =  H(k) . . . H(2) H(1)   

    as returned by SGEQLF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. M >= N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the   
            matrix Q. N >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the (n-k+i)-th column must contain the vector which   
            defines the elementary reflector H(i), for i = 1,2,...,k, as   
            returned by SGEQLF in the last k columns of its array   
            argument A.   
            On exit, the M-by-N matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= f2cmax(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQLF.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= f2cmax(1,N).   
            For optimum performance LWORK >= N*NB, where NB is the   
            optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument has an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    /* Local variables */
    static integer i__, j, l, nbmin, iinfo;
    extern /* Subroutine */ int sorg2l_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *);
    static integer ib, nb, kk, nx;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "SORGQL", " ", m, n, k, &c_n1, (ftnlen)6, (ftnlen)1);
    lwkopt = f2cmax(1,*n) * nb;
    work[1] = (real) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -5;
    } else if (*lwork < f2cmax(1,*n) && ! lquery) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGQL", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *n;
    if (nb > 1 && nb < *k) {

/*        Determine when to cross over from blocked to unblocked code.   

   Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "SORGQL", " ", m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);
	if (nx < *k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and   
                determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "SORGQL", " ", m, n, k, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		nbmin = f2cmax(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < *k && nx < *k) {

/*        Use blocked code after the first block.   
          The last kk columns are handled by the block method.   

   Computing MIN */
	i__1 = *k, i__2 = (*k - nx + nb - 1) / nb * nb;
	kk = f2cmin(i__1,i__2);

/*        Set A(m-kk+1:m,1:n-kk) to zero. */

	i__1 = *n - kk;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = *m - kk + 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = 0.f;
/* L10: */
	    }
/* L20: */
	}
    } else {
	kk = 0;
    }

/*     Use unblocked code for the first or only block. */

    i__1 = *m - kk;
    i__2 = *n - kk;
    i__3 = *k - kk;
    sorg2l_(&i__1, &i__2, &i__3, &a[a_offset], lda, &tau[1], &work[1], &iinfo)
	    ;

    if (kk > 0) {

/*        Use blocked code */

	i__1 = *k;
	i__2 = nb;
	for (i__ = *k - kk + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += 
		i__2) {
/* Computing MIN */
	    i__3 = nb, i__4 = *k - i__ + 1;
	    ib = f2cmin(i__3,i__4);
	    if (*n - *k + i__ > 1) {

/*              Form the triangular factor of the block reflector   
                H = H(i+ib-1) . . . H(i+1) H(i) */

		i__3 = *m - *k + i__ + ib - 1;
		slarft_("Backward", "Columnwise", &i__3, &ib, &a_ref(1, *n - *
			k + i__), lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(1:m-k+i+ib-1,1:n-k+i-1) from the left */

		i__3 = *m - *k + i__ + ib - 1;
		i__4 = *n - *k + i__ - 1;
		slarfb_("Left", "No transpose", "Backward", "Columnwise", &
			i__3, &i__4, &ib, &a_ref(1, *n - *k + i__), lda, &
			work[1], &ldwork, &a[a_offset], lda, &work[ib + 1], &
			ldwork);
	    }

/*           Apply H to rows 1:m-k+i+ib-1 of current block */

	    i__3 = *m - *k + i__ + ib - 1;
	    sorg2l_(&i__3, &ib, &ib, &a_ref(1, *n - *k + i__), lda, &tau[i__],
		     &work[1], &iinfo);

/*           Set rows m-k+i+ib:m of current block to zero */

	    i__3 = *n - *k + i__ + ib - 1;
	    for (j = *n - *k + i__; j <= i__3; ++j) {
		i__4 = *m;
		for (l = *m - *k + i__ + ib; l <= i__4; ++l) {
		    a_ref(l, j) = 0.f;
/* L30: */
		}
/* L40: */
	    }
/* L50: */
	}
    }

    work[1] = (real) iws;
    return 0;

/*     End of SORGQL */

} /* sorgql_ */

#undef a_ref





/* Subroutine */ int sorgqr_(integer *m, integer *n, integer *k, real *a, 
	integer *lda, real *tau, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORGQR generates an M-by-N real matrix Q with orthonormal columns,   
    which is defined as the first N columns of a product of K elementary   
    reflectors of order M   

          Q  =  H(1) H(2) . . . H(k)   

    as returned by SGEQRF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. M >= N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the   
            matrix Q. N >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the i-th column must contain the vector which   
            defines the elementary reflector H(i), for i = 1,2,...,k, as   
            returned by SGEQRF in the first k columns of its array   
            argument A.   
            On exit, the M-by-N matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= f2cmax(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQRF.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= f2cmax(1,N).   
            For optimum performance LWORK >= N*NB, where NB is the   
            optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument has an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, j, l, nbmin, iinfo, ib;
    extern /* Subroutine */ int sorg2r_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *);
    static integer nb, ki, kk, nx;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "SORGQR", " ", m, n, k, &c_n1, (ftnlen)6, (ftnlen)1);
    lwkopt = f2cmax(1,*n) * nb;
    work[1] = (real) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -5;
    } else if (*lwork < f2cmax(1,*n) && ! lquery) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGQR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *n;
    if (nb > 1 && nb < *k) {

/*        Determine when to cross over from blocked to unblocked code.   

   Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "SORGQR", " ", m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);
	if (nx < *k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and   
                determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "SORGQR", " ", m, n, k, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		nbmin = f2cmax(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < *k && nx < *k) {

/*        Use blocked code after the last block.   
          The first kk columns are handled by the block method. */

	ki = (*k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = *k, i__2 = ki + nb;
	kk = f2cmin(i__1,i__2);

/*        Set A(1:kk,kk+1:n) to zero. */

	i__1 = *n;
	for (j = kk + 1; j <= i__1; ++j) {
	    i__2 = kk;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = 0.f;
/* L10: */
	    }
/* L20: */
	}
    } else {
	kk = 0;
    }

/*     Use unblocked code for the last or only block. */

    if (kk < *n) {
	i__1 = *m - kk;
	i__2 = *n - kk;
	i__3 = *k - kk;
	sorg2r_(&i__1, &i__2, &i__3, &a_ref(kk + 1, kk + 1), lda, &tau[kk + 1]
		, &work[1], &iinfo);
    }

    if (kk > 0) {

/*        Use blocked code */

	i__1 = -nb;
	for (i__ = ki + 1; i__1 < 0 ? i__ >= 1 : i__ <= 1; i__ += i__1) {
/* Computing MIN */
	    i__2 = nb, i__3 = *k - i__ + 1;
	    ib = f2cmin(i__2,i__3);
	    if (i__ + ib <= *n) {

/*              Form the triangular factor of the block reflector   
                H = H(i) H(i+1) . . . H(i+ib-1) */

		i__2 = *m - i__ + 1;
		slarft_("Forward", "Columnwise", &i__2, &ib, &a_ref(i__, i__),
			 lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(i:m,i+ib:n) from the left */

		i__2 = *m - i__ + 1;
		i__3 = *n - i__ - ib + 1;
		slarfb_("Left", "No transpose", "Forward", "Columnwise", &
			i__2, &i__3, &ib, &a_ref(i__, i__), lda, &work[1], &
			ldwork, &a_ref(i__, i__ + ib), lda, &work[ib + 1], &
			ldwork);
	    }

/*           Apply H to rows i:m of current block */

	    i__2 = *m - i__ + 1;
	    sorg2r_(&i__2, &ib, &ib, &a_ref(i__, i__), lda, &tau[i__], &work[
		    1], &iinfo);

/*           Set rows 1:i-1 of current block to zero */

	    i__2 = i__ + ib - 1;
	    for (j = i__; j <= i__2; ++j) {
		i__3 = i__ - 1;
		for (l = 1; l <= i__3; ++l) {
		    a_ref(l, j) = 0.f;
/* L30: */
		}
/* L40: */
	    }
/* L50: */
	}
    }

    work[1] = (real) iws;
    return 0;

/*     End of SORGQR */

} /* sorgqr_ */

#undef a_ref





/* Subroutine */ int sorgtr_(char *uplo, integer *n, real *a, integer *lda, 
	real *tau, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORGTR generates a real orthogonal matrix Q which is defined as the   
    product of n-1 elementary reflectors of order N, as returned by   
    SSYTRD:   

    if UPLO = 'U', Q = H(n-1) . . . H(2) H(1),   

    if UPLO = 'L', Q = H(1) H(2) . . . H(n-1).   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            = 'U': Upper triangle of A contains elementary reflectors   
                   from SSYTRD;   
            = 'L': Lower triangle of A contains elementary reflectors   
                   from SSYTRD.   

    N       (input) INTEGER   
            The order of the matrix Q. N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the vectors which define the elementary reflectors,   
            as returned by SSYTRD.   
            On exit, the N-by-N orthogonal matrix Q.   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= f2cmax(1,N).   

    TAU     (input) REAL array, dimension (N-1)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SSYTRD.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= f2cmax(1,N-1).   
            For optimum performance LWORK >= (N-1)*NB, where NB is   
            the optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer iinfo;
    static logical upper;
    static integer nb;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int sorgql_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *), sorgqr_(
	    integer *, integer *, integer *, real *, integer *, real *, real *
	    , integer *, integer *);
    static logical lquery;
    static integer lwkopt;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*n)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = 1, i__2 = *n - 1;
	if (*lwork < f2cmax(i__1,i__2) && ! lquery) {
	    *info = -7;
	}
    }

    if (*info == 0) {
	if (upper) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    i__3 = *n - 1;
	    nb = ilaenv_(&c__1, "SORGQL", " ", &i__1, &i__2, &i__3, &c_n1, (
		    ftnlen)6, (ftnlen)1);
	} else {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    i__3 = *n - 1;
	    nb = ilaenv_(&c__1, "SORGQR", " ", &i__1, &i__2, &i__3, &c_n1, (
		    ftnlen)6, (ftnlen)1);
	}
/* Computing MAX */
	i__1 = 1, i__2 = *n - 1;
	lwkopt = f2cmax(i__1,i__2) * nb;
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGTR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	work[1] = 1.f;
	return 0;
    }

    if (upper) {

/*        Q was determined by a call to SSYTRD with UPLO = 'U'   

          Shift the vectors which define the elementary reflectors one   
          column to the left, and set the last row and column of Q to   
          those of the unit matrix */

	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = a_ref(i__, j + 1);
/* L10: */
	    }
	    a_ref(*n, j) = 0.f;
/* L20: */
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    a_ref(i__, *n) = 0.f;
/* L30: */
	}
	a_ref(*n, *n) = 1.f;

/*        Generate Q(1:n-1,1:n-1) */

	i__1 = *n - 1;
	i__2 = *n - 1;
	i__3 = *n - 1;
	sorgql_(&i__1, &i__2, &i__3, &a[a_offset], lda, &tau[1], &work[1], 
		lwork, &iinfo);

    } else {

/*        Q was determined by a call to SSYTRD with UPLO = 'L'.   

          Shift the vectors which define the elementary reflectors one   
          column to the right, and set the first row and column of Q to   
          those of the unit matrix */

	for (j = *n; j >= 2; --j) {
	    a_ref(1, j) = 0.f;
	    i__1 = *n;
	    for (i__ = j + 1; i__ <= i__1; ++i__) {
		a_ref(i__, j) = a_ref(i__, j - 1);
/* L40: */
	    }
/* L50: */
	}
	a_ref(1, 1) = 1.f;
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    a_ref(i__, 1) = 0.f;
/* L60: */
	}
	if (*n > 1) {

/*           Generate Q(2:n,2:n) */

	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    i__3 = *n - 1;
	    sorgqr_(&i__1, &i__2, &i__3, &a_ref(2, 2), lda, &tau[1], &work[1],
		     lwork, &iinfo);
	}
    }
    work[1] = (real) lwkopt;
    return 0;

/*     End of SORGTR */

} /* sorgtr_ */

#undef a_ref





/* Subroutine */ int sscal_(integer *n, real *sa, real *sx, integer *incx)
{
    /* System generated locals */
    integer i__1, i__2;
    /* Local variables */
    static integer i__, m, nincx, mp1;
/*     scales a vector by a constant.   
       uses unrolled loops for increment equal to 1.   
       jack dongarra, linpack, 3/11/78.   
       modified 3/93 to return if incx .le. 0.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sx;
    /* Function Body */
    if (*n <= 0 || *incx <= 0) {
	return 0;
    }
    if (*incx == 1) {
	goto L20;
    }
/*        code for increment not equal to 1 */
    nincx = *n * *incx;
    i__1 = nincx;
    i__2 = *incx;
    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	sx[i__] = *sa * sx[i__];
/* L10: */
    }
    return 0;
/*        code for increment equal to 1   
          clean-up loop */
L20:
    m = *n % 5;
    if (m == 0) {
	goto L40;
    }
    i__2 = m;
    for (i__ = 1; i__ <= i__2; ++i__) {
	sx[i__] = *sa * sx[i__];
/* L30: */
    }
    if (*n < 5) {
	return 0;
    }
L40:
    mp1 = m + 1;
    i__2 = *n;
    for (i__ = mp1; i__ <= i__2; i__ += 5) {
	sx[i__] = *sa * sx[i__];
	sx[i__ + 1] = *sa * sx[i__ + 1];
	sx[i__ + 2] = *sa * sx[i__ + 2];
	sx[i__ + 3] = *sa * sx[i__ + 3];
	sx[i__ + 4] = *sa * sx[i__ + 4];
/* L50: */
    }
    return 0;
} /* sscal_ */




/* Subroutine */ int ssteqr_(const char *compz, integer *n, real *d__, real *e, 
	real *z__, integer *ldz, real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SSTEQR computes all eigenvalues and, optionally, eigenvectors of a   
    symmetric tridiagonal matrix using the implicit QL or QR method.   
    The eigenvectors of a full or band symmetric matrix can also be found   
    if SSYTRD or SSPTRD or SSBTRD has been used to reduce this matrix to   
    tridiagonal form.   

    Arguments   
    =========   

    COMPZ   (input) CHARACTER*1   
            = 'N':  Compute eigenvalues only.   
            = 'V':  Compute eigenvalues and eigenvectors of the original   
                    symmetric matrix.  On entry, Z must contain the   
                    orthogonal matrix used to reduce the original matrix   
                    to tridiagonal form.   
            = 'I':  Compute eigenvalues and eigenvectors of the   
                    tridiagonal matrix.  Z is initialized to the identity   
                    matrix.   

    N       (input) INTEGER   
            The order of the matrix.  N >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the diagonal elements of the tridiagonal matrix.   
            On exit, if INFO = 0, the eigenvalues in ascending order.   

    E       (input/output) REAL array, dimension (N-1)   
            On entry, the (n-1) subdiagonal elements of the tridiagonal   
            matrix.   
            On exit, E has been destroyed.   

    Z       (input/output) REAL array, dimension (LDZ, N)   
            On entry, if  COMPZ = 'V', then Z contains the orthogonal   
            matrix used in the reduction to tridiagonal form.   
            On exit, if INFO = 0, then if  COMPZ = 'V', Z contains the   
            orthonormal eigenvectors of the original symmetric matrix,   
            and if COMPZ = 'I', Z contains the orthonormal eigenvectors   
            of the symmetric tridiagonal matrix.   
            If COMPZ = 'N', then Z is not referenced.   

    LDZ     (input) INTEGER   
            The leading dimension of the array Z.  LDZ >= 1, and if   
            eigenvectors are desired, then  LDZ >= f2cmax(1,N).   

    WORK    (workspace) REAL array, dimension (f2cmax(1,2*N-2))   
            If COMPZ = 'N', then WORK is not referenced.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  the algorithm has failed to find all the eigenvalues in   
                  a total of 30*N iterations; if INFO = i, then i   
                  elements of E have not converged to zero; on exit, D   
                  and E contain the elements of a symmetric tridiagonal   
                  matrix which is orthogonally similar to the original   
                  matrix.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static real c_b9 = 0.f;
    static real c_b10 = 1.f;
    static integer c__0 = 0;
    static integer c__1 = 1;
    static integer c__2 = 2;
    
    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    real r__1, r__2;
    /* Builtin functions */
//    double sqrt(doublereal), r_sign(real *, real *);
    double r_sign(real *, real *);
    /* Local variables */
    static integer lend, jtot;
    extern /* Subroutine */ int slae2_(real *, real *, real *, real *, real *)
	    ;
    static real b, c__, f, g;
    static integer i__, j, k, l, m;
    static real p, r__, s;
    extern logical lsame_(const char *, const char *);
    static real anorm;
    extern /* Subroutine */ int slasr_(const char *, const char *, const char *, integer *, 
	    integer *, real *, real *, real *, integer *);
    static integer l1;
    extern /* Subroutine */ int sswap_(integer *, real *, integer *, real *, 
	    integer *);
    static integer lendm1, lendp1;
    extern /* Subroutine */ int slaev2_(real *, real *, real *, real *, real *
	    , real *, real *);
    extern doublereal slapy2_(real *, real *);
    static integer ii, mm, iscale;
    extern doublereal slamch_(const char *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static real safmax;
    extern /* Subroutine */ int slascl_(const char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *);
    static integer lendsv;
    extern /* Subroutine */ int slartg_(real *, real *, real *, real *, real *
	    ), slaset_(const char *, integer *, integer *, real *, real *, real *, 
	    integer *);
    static real ssfmin;
    static integer nmaxit, icompz;
    static real ssfmax;
    extern doublereal slanst_(const char *, integer *, real *, real *);
    extern /* Subroutine */ int slasrt_(const char *, integer *, real *, integer *);
    static integer lm1, mm1, nm1;
    static real rt1, rt2, eps;
    static integer lsv;
    static real tst, eps2;
#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]


    --d__;
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

    if (lsame_(compz, "N")) {
	icompz = 0;
    } else if (lsame_(compz, "V")) {
	icompz = 1;
    } else if (lsame_(compz, "I")) {
	icompz = 2;
    } else {
	icompz = -1;
    }
    if (icompz < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || icompz > 0 && *ldz < f2cmax(1,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSTEQR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (icompz == 2) {
	    z___ref(1, 1) = 1.f;
	}
	return 0;
    }

/*     Determine the unit roundoff and over/underflow thresholds. */

    eps = slamch_("E");
/* Computing 2nd power */
    r__1 = eps;
    eps2 = r__1 * r__1;
    safmin = slamch_("S");
    safmax = 1.f / safmin;
    ssfmax = sqrt(safmax) / 3.f;
    ssfmin = sqrt(safmin) / eps2;

/*     Compute the eigenvalues and eigenvectors of the tridiagonal   
       matrix. */

    if (icompz == 2) {
	slaset_("Full", n, n, &c_b9, &c_b10, &z__[z_offset], ldz);
    }

    nmaxit = *n * 30;
    jtot = 0;

/*     Determine where the matrix splits and choose QL or QR iteration   
       for each block, according to whether top or bottom diagonal   
       element is smaller. */

    l1 = 1;
    nm1 = *n - 1;

L10:
    if (l1 > *n) {
	goto L160;
    }
    if (l1 > 1) {
	e[l1 - 1] = 0.f;
    }
    if (l1 <= nm1) {
	i__1 = nm1;
	for (m = l1; m <= i__1; ++m) {
	    tst = (r__1 = e[m], dabs(r__1));
	    if (tst == 0.f) {
		goto L30;
	    }
	    if (tst <= sqrt((r__1 = d__[m], dabs(r__1))) * sqrt((r__2 = d__[m 
		    + 1], dabs(r__2))) * eps) {
		e[m] = 0.f;
		goto L30;
	    }
/* L20: */
	}
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = slanst_("I", &i__1, &d__[l], &e[l]);
    iscale = 0;
    if (anorm == 0.f) {
	goto L10;
    }
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n, 
		info);
	i__1 = lend - l;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n, 
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n, 
		info);
	i__1 = lend - l;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n, 
		info);
    }

/*     Choose between QL and QR iteration */

    if ((r__1 = d__[lend], dabs(r__1)) < (r__2 = d__[l], dabs(r__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend > l) {

/*        QL Iteration   

          Look for small subdiagonal element. */

L40:
	if (l != lend) {
	    lendm1 = lend - 1;
	    i__1 = lendm1;
	    for (m = l; m <= i__1; ++m) {
/* Computing 2nd power */
		r__2 = (r__1 = e[m], dabs(r__1));
		tst = r__2 * r__2;
		if (tst <= eps2 * (r__1 = d__[m], dabs(r__1)) * (r__2 = d__[m 
			+ 1], dabs(r__2)) + safmin) {
		    goto L60;
		}
/* L50: */
	    }
	}

	m = lend;

L60:
	if (m < lend) {
	    e[m] = 0.f;
	}
	p = d__[l];
	if (m == l) {
	    goto L80;
	}

/*        If remaining matrix is 2-by-2, use SLAE2 or SLAEV2   
          to compute its eigensystem. */

	if (m == l + 1) {
	    if (icompz > 0) {
		slaev2_(&d__[l], &e[l], &d__[l + 1], &rt1, &rt2, &c__, &s);
		work[l] = c__;
		work[*n - 1 + l] = s;
		slasr_("R", "V", "B", n, &c__2, &work[l], &work[*n - 1 + l], &
			z___ref(1, l), ldz);
	    } else {
		slae2_(&d__[l], &e[l], &d__[l + 1], &rt1, &rt2);
	    }
	    d__[l] = rt1;
	    d__[l + 1] = rt2;
	    e[l] = 0.f;
	    l += 2;
	    if (l <= lend) {
		goto L40;
	    }
	    goto L140;
	}

	if (jtot == nmaxit) {
	    goto L140;
	}
	++jtot;

/*        Form shift. */

	g = (d__[l + 1] - p) / (e[l] * 2.f);
	r__ = slapy2_(&g, &c_b10);
	g = d__[m] - p + e[l] / (g + r_sign(&r__, &g));

	s = 1.f;
	c__ = 1.f;
	p = 0.f;

/*        Inner loop */

	mm1 = m - 1;
	i__1 = l;
	for (i__ = mm1; i__ >= i__1; --i__) {
	    f = s * e[i__];
	    b = c__ * e[i__];
	    slartg_(&g, &f, &c__, &s, &r__);
	    if (i__ != m - 1) {
		e[i__ + 1] = r__;
	    }
	    g = d__[i__ + 1] - p;
	    r__ = (d__[i__] - g) * s + c__ * 2.f * b;
	    p = s * r__;
	    d__[i__ + 1] = g + p;
	    g = c__ * r__ - b;

/*           If eigenvectors are desired, then save rotations. */

	    if (icompz > 0) {
		work[i__] = c__;
		work[*n - 1 + i__] = -s;
	    }

/* L70: */
	}

/*        If eigenvectors are desired, then apply saved rotations. */

	if (icompz > 0) {
	    mm = m - l + 1;
	    slasr_("R", "V", "B", n, &mm, &work[l], &work[*n - 1 + l], &
		    z___ref(1, l), ldz);
	}

	d__[l] -= p;
	e[l] = g;
	goto L40;

/*        Eigenvalue found. */

L80:
	d__[l] = p;

	++l;
	if (l <= lend) {
	    goto L40;
	}
	goto L140;

    } else {

/*        QR Iteration   

          Look for small superdiagonal element. */

L90:
	if (l != lend) {
	    lendp1 = lend + 1;
	    i__1 = lendp1;
	    for (m = l; m >= i__1; --m) {
/* Computing 2nd power */
		r__2 = (r__1 = e[m - 1], dabs(r__1));
		tst = r__2 * r__2;
		if (tst <= eps2 * (r__1 = d__[m], dabs(r__1)) * (r__2 = d__[m 
			- 1], dabs(r__2)) + safmin) {
		    goto L110;
		}
/* L100: */
	    }
	}

	m = lend;

L110:
	if (m > lend) {
	    e[m - 1] = 0.f;
	}
	p = d__[l];
	if (m == l) {
	    goto L130;
	}

/*        If remaining matrix is 2-by-2, use SLAE2 or SLAEV2   
          to compute its eigensystem. */

	if (m == l - 1) {
	    if (icompz > 0) {
		slaev2_(&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2, &c__, &s)
			;
		work[m] = c__;
		work[*n - 1 + m] = s;
		slasr_("R", "V", "F", n, &c__2, &work[m], &work[*n - 1 + m], &
			z___ref(1, l - 1), ldz);
	    } else {
		slae2_(&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2);
	    }
	    d__[l - 1] = rt1;
	    d__[l] = rt2;
	    e[l - 1] = 0.f;
	    l += -2;
	    if (l >= lend) {
		goto L90;
	    }
	    goto L140;
	}

	if (jtot == nmaxit) {
	    goto L140;
	}
	++jtot;

/*        Form shift. */

	g = (d__[l - 1] - p) / (e[l - 1] * 2.f);
	r__ = slapy2_(&g, &c_b10);
	g = d__[m] - p + e[l - 1] / (g + r_sign(&r__, &g));

	s = 1.f;
	c__ = 1.f;
	p = 0.f;

/*        Inner loop */

	lm1 = l - 1;
	i__1 = lm1;
	for (i__ = m; i__ <= i__1; ++i__) {
	    f = s * e[i__];
	    b = c__ * e[i__];
	    slartg_(&g, &f, &c__, &s, &r__);
	    if (i__ != m) {
		e[i__ - 1] = r__;
	    }
	    g = d__[i__] - p;
	    r__ = (d__[i__ + 1] - g) * s + c__ * 2.f * b;
	    p = s * r__;
	    d__[i__] = g + p;
	    g = c__ * r__ - b;

/*           If eigenvectors are desired, then save rotations. */

	    if (icompz > 0) {
		work[i__] = c__;
		work[*n - 1 + i__] = s;
	    }

/* L120: */
	}

/*        If eigenvectors are desired, then apply saved rotations. */

	if (icompz > 0) {
	    mm = l - m + 1;
	    slasr_("R", "V", "F", n, &mm, &work[m], &work[*n - 1 + m], &
		    z___ref(1, m), ldz);
	}

	d__[l] -= p;
	e[lm1] = g;
	goto L90;

/*        Eigenvalue found. */

L130:
	d__[l] = p;

	--l;
	if (l >= lend) {
	    goto L90;
	}
	goto L140;

    }

/*     Undo scaling if necessary */

L140:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	slascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv], 
		n, info);
	i__1 = lendsv - lsv;
	slascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &e[lsv], n, 
		info);
    } else if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	slascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv], 
		n, info);
	i__1 = lendsv - lsv;
	slascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &e[lsv], n, 
		info);
    }

/*     Check for no convergence to an eigenvalue after a total   
       of N*MAXIT iterations. */

    if (jtot < nmaxit) {
	goto L10;
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.f) {
	    ++(*info);
	}
/* L150: */
    }
    goto L190;

/*     Order eigenvalues and eigenvectors. */

L160:
    if (icompz == 0) {

/*        Use Quick Sort */

	slasrt_("I", n, &d__[1], info);

    } else {

/*        Use Selection Sort to minimize swaps of eigenvectors */

	i__1 = *n;
	for (ii = 2; ii <= i__1; ++ii) {
	    i__ = ii - 1;
	    k = i__;
	    p = d__[i__];
	    i__2 = *n;
	    for (j = ii; j <= i__2; ++j) {
		if (d__[j] < p) {
		    k = j;
		    p = d__[j];
		}
/* L170: */
	    }
	    if (k != i__) {
		d__[k] = d__[i__];
		d__[i__] = p;
		sswap_(n, &z___ref(1, i__), &c__1, &z___ref(1, k), &c__1);
	    }
/* L180: */
	}
    }

L190:
    return 0;

/*     End of SSTEQR */

} /* ssteqr_ */

#undef z___ref





/* Subroutine */ int ssterf_(integer *n, real *d__, real *e, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SSTERF computes all eigenvalues of a symmetric tridiagonal matrix   
    using the Pal-Walker-Kahan variant of the QL or QR algorithm.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the matrix.  N >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the n diagonal elements of the tridiagonal matrix.   
            On exit, if INFO = 0, the eigenvalues in ascending order.   

    E       (input/output) REAL array, dimension (N-1)   
            On entry, the (n-1) subdiagonal elements of the tridiagonal   
            matrix.   
            On exit, E has been destroyed.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  the algorithm failed to find all of the eigenvalues in   
                  a total of 30*N iterations; if INFO = i, then i   
                  elements of E have not converged to zero.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__0 = 0;
    static integer c__1 = 1;
    static real c_b32 = 1.f;
    
    /* System generated locals */
    integer i__1;
    real r__1, r__2, r__3;
    /* Builtin functions */
//    double sqrt(doublereal), r_sign(real *, real *);
    double r_sign(real *, real *);
    /* Local variables */
    static real oldc;
    static integer lend, jtot;
    extern /* Subroutine */ int slae2_(real *, real *, real *, real *, real *)
	    ;
    static real c__;
    static integer i__, l, m;
    static real p, gamma, r__, s, alpha, sigma, anorm;
    static integer l1;
    static real bb;
    extern doublereal slapy2_(real *, real *);
    static integer iscale;
    static real oldgam;
    extern doublereal slamch_(const char *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static real safmax;
    extern /* Subroutine */ int slascl_(const char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *);
    static integer lendsv;
    static real ssfmin;
    static integer nmaxit;
    static real ssfmax;
    extern doublereal slanst_(const char *, integer *, real *, real *);
    extern /* Subroutine */ int slasrt_(const char *, integer *, real *, integer *);
    static real rt1, rt2, eps, rte;
    static integer lsv;
    static real eps2;


    --e;
    --d__;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("SSTERF", &i__1);
	return 0;
    }
    if (*n <= 1) {
	return 0;
    }

/*     Determine the unit roundoff for this environment. */

    eps = slamch_("E");
/* Computing 2nd power */
    r__1 = eps;
    eps2 = r__1 * r__1;
    safmin = slamch_("S");
    safmax = 1.f / safmin;
    ssfmax = sqrt(safmax) / 3.f;
    ssfmin = sqrt(safmin) / eps2;

/*     Compute the eigenvalues of the tridiagonal matrix. */

    nmaxit = *n * 30;
    sigma = 0.f;
    jtot = 0;

/*     Determine where the matrix splits and choose QL or QR iteration   
       for each block, according to whether top or bottom diagonal   
       element is smaller. */

    l1 = 1;

L10:
    if (l1 > *n) {
	goto L170;
    }
    if (l1 > 1) {
	e[l1 - 1] = 0.f;
    }
    i__1 = *n - 1;
    for (m = l1; m <= i__1; ++m) {
	if ((r__3 = e[m], dabs(r__3)) <= sqrt((r__1 = d__[m], dabs(r__1))) * 
		sqrt((r__2 = d__[m + 1], dabs(r__2))) * eps) {
	    e[m] = 0.f;
	    goto L30;
	}
/* L20: */
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = slanst_("I", &i__1, &d__[l], &e[l]);
    iscale = 0;
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n, 
		info);
	i__1 = lend - l;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n, 
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n, 
		info);
	i__1 = lend - l;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n, 
		info);
    }

    i__1 = lend - 1;
    for (i__ = l; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	r__1 = e[i__];
	e[i__] = r__1 * r__1;
/* L40: */
    }

/*     Choose between QL and QR iteration */

    if ((r__1 = d__[lend], dabs(r__1)) < (r__2 = d__[l], dabs(r__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend >= l) {

/*        QL Iteration   

          Look for small subdiagonal element. */

L50:
	if (l != lend) {
	    i__1 = lend - 1;
	    for (m = l; m <= i__1; ++m) {
		if ((r__2 = e[m], dabs(r__2)) <= eps2 * (r__1 = d__[m] * d__[
			m + 1], dabs(r__1))) {
		    goto L70;
		}
/* L60: */
	    }
	}
	m = lend;

L70:
	if (m < lend) {
	    e[m] = 0.f;
	}
	p = d__[l];
	if (m == l) {
	    goto L90;
	}

/*        If remaining matrix is 2 by 2, use SLAE2 to compute its   
          eigenvalues. */

	if (m == l + 1) {
	    rte = sqrt(e[l]);
	    slae2_(&d__[l], &rte, &d__[l + 1], &rt1, &rt2);
	    d__[l] = rt1;
	    d__[l + 1] = rt2;
	    e[l] = 0.f;
	    l += 2;
	    if (l <= lend) {
		goto L50;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(e[l]);
	sigma = (d__[l + 1] - p) / (rte * 2.f);
	r__ = slapy2_(&sigma, &c_b32);
	sigma = p - rte / (sigma + r_sign(&r__, &sigma));

	c__ = 1.f;
	s = 0.f;
	gamma = d__[m] - sigma;
	p = gamma * gamma;

/*        Inner loop */

	i__1 = l;
	for (i__ = m - 1; i__ >= i__1; --i__) {
	    bb = e[i__];
	    r__ = p + bb;
	    if (i__ != m - 1) {
		e[i__ + 1] = s * r__;
	    }
	    oldc = c__;
	    c__ = p / r__;
	    s = bb / r__;
	    oldgam = gamma;
	    alpha = d__[i__];
	    gamma = c__ * (alpha - sigma) - s * oldgam;
	    d__[i__ + 1] = oldgam + (alpha - gamma);
	    if (c__ != 0.f) {
		p = gamma * gamma / c__;
	    } else {
		p = oldc * bb;
	    }
/* L80: */
	}

	e[l] = s * p;
	d__[l] = sigma + gamma;
	goto L50;

/*        Eigenvalue found. */

L90:
	d__[l] = p;

	++l;
	if (l <= lend) {
	    goto L50;
	}
	goto L150;

    } else {

/*        QR Iteration   

          Look for small superdiagonal element. */

L100:
	i__1 = lend + 1;
	for (m = l; m >= i__1; --m) {
	    if ((r__2 = e[m - 1], dabs(r__2)) <= eps2 * (r__1 = d__[m] * d__[
		    m - 1], dabs(r__1))) {
		goto L120;
	    }
/* L110: */
	}
	m = lend;

L120:
	if (m > lend) {
	    e[m - 1] = 0.f;
	}
	p = d__[l];
	if (m == l) {
	    goto L140;
	}

/*        If remaining matrix is 2 by 2, use SLAE2 to compute its   
          eigenvalues. */

	if (m == l - 1) {
	    rte = sqrt(e[l - 1]);
	    slae2_(&d__[l], &rte, &d__[l - 1], &rt1, &rt2);
	    d__[l] = rt1;
	    d__[l - 1] = rt2;
	    e[l - 1] = 0.f;
	    l += -2;
	    if (l >= lend) {
		goto L100;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(e[l - 1]);
	sigma = (d__[l - 1] - p) / (rte * 2.f);
	r__ = slapy2_(&sigma, &c_b32);
	sigma = p - rte / (sigma + r_sign(&r__, &sigma));

	c__ = 1.f;
	s = 0.f;
	gamma = d__[m] - sigma;
	p = gamma * gamma;

/*        Inner loop */

	i__1 = l - 1;
	for (i__ = m; i__ <= i__1; ++i__) {
	    bb = e[i__];
	    r__ = p + bb;
	    if (i__ != m) {
		e[i__ - 1] = s * r__;
	    }
	    oldc = c__;
	    c__ = p / r__;
	    s = bb / r__;
	    oldgam = gamma;
	    alpha = d__[i__ + 1];
	    gamma = c__ * (alpha - sigma) - s * oldgam;
	    d__[i__] = oldgam + (alpha - gamma);
	    if (c__ != 0.f) {
		p = gamma * gamma / c__;
	    } else {
		p = oldc * bb;
	    }
/* L130: */
	}

	e[l - 1] = s * p;
	d__[l] = sigma + gamma;
	goto L100;

/*        Eigenvalue found. */

L140:
	d__[l] = p;

	--l;
	if (l >= lend) {
	    goto L100;
	}
	goto L150;

    }

/*     Undo scaling if necessary */

L150:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	slascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv], 
		n, info);
    }
    if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	slascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv], 
		n, info);
    }

/*     Check for no convergence to an eigenvalue after a total   
       of N*MAXIT iterations. */

    if (jtot < nmaxit) {
	goto L10;
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.f) {
	    ++(*info);
	}
/* L160: */
    }
    goto L180;

/*     Sort eigenvalues in increasing order. */

L170:
    slasrt_("I", n, &d__[1], info);

L180:
    return 0;

/*     End of SSTERF */

} /* ssterf_ */




/* Subroutine */ int sswap_(integer *n, real *sx, integer *incx, real *sy, 
	integer *incy)
{
    /* System generated locals */
    integer i__1;
    /* Local variables */
    static integer i__, m;
    static real stemp;
    static integer ix, iy, mp1;
/*     interchanges two vectors.   
       uses unrolled loops for increments equal to 1.   
       jack dongarra, linpack, 3/11/78.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sy;
    --sx;
    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }
/*       code for unequal increments or equal increments not equal   
           to 1 */
    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	stemp = sx[ix];
	sx[ix] = sy[iy];
	sy[iy] = stemp;
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;
/*       code for both increments equal to 1   
         clean-up loop */
L20:
    m = *n % 3;
    if (m == 0) {
	goto L40;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	stemp = sx[i__];
	sx[i__] = sy[i__];
	sy[i__] = stemp;
/* L30: */
    }
    if (*n < 3) {
	return 0;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 3) {
	stemp = sx[i__];
	sx[i__] = sy[i__];
	sy[i__] = stemp;
	stemp = sx[i__ + 1];
	sx[i__ + 1] = sy[i__ + 1];
	sy[i__ + 1] = stemp;
	stemp = sx[i__ + 2];
	sx[i__ + 2] = sy[i__ + 2];
	sy[i__ + 2] = stemp;
/* L50: */
    }
    return 0;
} /* sswap_ */




/* Subroutine */ int ssyev_(char *jobz, char *uplo, integer *n, real *a, 
	integer *lda, real *w, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK driver routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SSYEV computes all eigenvalues and, optionally, eigenvectors of a   
    real symmetric matrix A.   

    Arguments   
    =========   

    JOBZ    (input) CHARACTER*1   
            = 'N':  Compute eigenvalues only;   
            = 'V':  Compute eigenvalues and eigenvectors.   

    UPLO    (input) CHARACTER*1   
            = 'U':  Upper triangle of A is stored;   
            = 'L':  Lower triangle of A is stored.   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA, N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the   
            leading N-by-N upper triangular part of A contains the   
            upper triangular part of the matrix A.  If UPLO = 'L',   
            the leading N-by-N lower triangular part of A contains   
            the lower triangular part of the matrix A.   
            On exit, if JOBZ = 'V', then if INFO = 0, A contains the   
            orthonormal eigenvectors of the matrix A.   
            If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')   
            or the upper triangle (if UPLO='U') of A, including the   
            diagonal, is destroyed.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(1,N).   

    W       (output) REAL array, dimension (N)   
            If INFO = 0, the eigenvalues in ascending order.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The length of the array WORK.  LWORK >= f2cmax(1,3*N-1).   
            For optimal efficiency, LWORK >= (NB+2)*N,   
            where NB is the blocksize for SSYTRD returned by ILAENV.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  if INFO = i, the algorithm failed to converge; i   
                  off-diagonal elements of an intermediate tridiagonal   
                  form did not converge to zero.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__0 = 0;
    static real c_b17 = 1.f;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real r__1;
    /* Builtin functions */
//    double sqrt(doublereal);
    /* Local variables */
    static integer inde;
    static real anrm;
    static integer imax;
    static real rmin, rmax;
    static integer lopt;
    static real sigma;
    extern logical lsame_(const char *, const char *);
    static integer iinfo;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static logical lower, wantz;
    static integer nb, iscale;
    extern doublereal slamch_(const char *);
    static real safmin;
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static real bignum;
    extern /* Subroutine */ int slascl_(const char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *);
    static integer indtau, indwrk;
    extern /* Subroutine */ int ssterf_(integer *, real *, real *, integer *);
    extern doublereal slansy_(const char *, char *, integer *, real *, integer *, 
	    real *);
    static integer llwork;
    static real smlnum;
    static integer lwkopt;
    static logical lquery;
    extern /* Subroutine */ int sorgtr_(char *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *), ssteqr_(const char *, 
	    integer *, real *, real *, real *, integer *, real *, integer *), ssytrd_(char *, integer *, real *, integer *, real *, 
	    real *, real *, real *, integer *, integer *);
    static real eps;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --w;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lower = lsame_(uplo, "L");
    lquery = *lwork == -1;

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < f2cmax(1,*n)) {
	*info = -5;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = 1, i__2 = *n * 3 - 1;
	if (*lwork < f2cmax(i__1,i__2) && ! lquery) {
	    *info = -8;
	}
    }

    if (*info == 0) {
	nb = ilaenv_(&c__1, "SSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, (ftnlen)6,
		 (ftnlen)1);
/* Computing MAX */
	i__1 = 1, i__2 = (nb + 2) * *n;
	lwkopt = f2cmax(i__1,i__2);
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSYEV ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	work[1] = 1.f;
	return 0;
    }

    if (*n == 1) {
	w[1] = a_ref(1, 1);
	work[1] = 3.f;
	if (wantz) {
	    a_ref(1, 1) = 1.f;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = slamch_("Safe minimum");
    eps = slamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1.f / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = slansy_("M", uplo, n, &a[a_offset], lda, &work[1]);
    iscale = 0;
    if (anrm > 0.f && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	slascl_(uplo, &c__0, &c__0, &c_b17, &sigma, n, n, &a[a_offset], lda, 
		info);
    }

/*     Call SSYTRD to reduce symmetric matrix to tridiagonal form. */

    inde = 1;
    indtau = inde + *n;
    indwrk = indtau + *n;
    llwork = *lwork - indwrk + 1;
    ssytrd_(uplo, n, &a[a_offset], lda, &w[1], &work[inde], &work[indtau], &
	    work[indwrk], &llwork, &iinfo);
    lopt = static_cast<integer>( (*n << 1) + work[indwrk] );

/*     For eigenvalues only, call SSTERF.  For eigenvectors, first call   
       SORGTR to generate the orthogonal matrix, then call SSTEQR. */

    if (! wantz) {
	ssterf_(n, &w[1], &work[inde], info);
    } else {
	sorgtr_(uplo, n, &a[a_offset], lda, &work[indtau], &work[indwrk], &
		llwork, &iinfo);
	ssteqr_(jobz, n, &w[1], &work[inde], &a[a_offset], lda, &work[indtau],
		 info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	if (*info == 0) {
	    imax = *n;
	} else {
	    imax = *info - 1;
	}
	r__1 = 1.f / sigma;
	sscal_(&imax, &r__1, &w[1], &c__1);
    }

/*     Set WORK(1) to optimal workspace size. */

    work[1] = (real) lwkopt;

    return 0;

/*     End of SSYEV */

} /* ssyev_ */

#undef a_ref





/* Subroutine */ int ssymv_(const char *uplo, integer *n, real *alpha, real *a, 
	integer *lda, real *x, integer *incx, real *beta, real *y, integer *
	incy)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    /* Local variables */
    static integer info;
    static real temp1, temp2;
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer ix, iy, jx, jy, kx, ky;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
/*  Purpose   
    =======   
    SSYMV  performs the matrix-vector  operation   
       y := alpha*A*x + beta*y,   
    where alpha and beta are scalars, x and y are n element vectors and   
    A is an n by n symmetric matrix.   
    Parameters   
    ==========   
    UPLO   - CHARACTER*1.   
             On entry, UPLO specifies whether the upper or lower   
             triangular part of the array A is to be referenced as   
             follows:   
                UPLO = 'U' or 'u'   Only the upper triangular part of A   
                                    is to be referenced.   
                UPLO = 'L' or 'l'   Only the lower triangular part of A   
                                    is to be referenced.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry, N specifies the order of the matrix A.   
             N must be at least zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, n ).   
             Before entry with  UPLO = 'U' or 'u', the leading n by n   
             upper triangular part of the array A must contain the upper   
             triangular part of the symmetric matrix and the strictly   
             lower triangular part of A is not referenced.   
             Before entry with UPLO = 'L' or 'l', the leading n by n   
             lower triangular part of the array A must contain the lower   
             triangular part of the symmetric matrix and the strictly   
             upper triangular part of A is not referenced.   
             Unchanged on exit.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program. LDA must be at least   
             f2cmax( 1, n ).   
             Unchanged on exit.   
    X      - REAL             array of dimension at least   
             ( 1 + ( n - 1 )*abs( INCX ) ).   
             Before entry, the incremented array X must contain the n   
             element vector x.   
             Unchanged on exit.   
    INCX   - INTEGER.   
             On entry, INCX specifies the increment for the elements of   
             X. INCX must not be zero.   
             Unchanged on exit.   
    BETA   - REAL            .   
             On entry, BETA specifies the scalar beta. When BETA is   
             supplied as zero then Y need not be set on input.   
             Unchanged on exit.   
    Y      - REAL             array of dimension at least   
             ( 1 + ( n - 1 )*abs( INCY ) ).   
             Before entry, the incremented array Y must contain the n   
             element vector y. On exit, Y is overwritten by the updated   
             vector y.   
    INCY   - INTEGER.   
             On entry, INCY specifies the increment for the elements of   
             Y. INCY must not be zero.   
             Unchanged on exit.   
    Level 2 Blas routine.   
    -- Written on 22-October-1986.   
       Jack Dongarra, Argonne National Lab.   
       Jeremy Du Croz, Nag Central Office.   
       Sven Hammarling, Nag Central Office.   
       Richard Hanson, Sandia National Labs.   
       Test the input parameters.   
       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --x;
    --y;
    /* Function Body */
    info = 0;
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	info = 1;
    } else if (*n < 0) {
	info = 2;
    } else if (*lda < f2cmax(1,*n)) {
	info = 5;
    } else if (*incx == 0) {
	info = 7;
    } else if (*incy == 0) {
	info = 10;
    }
    if (info != 0) {
	xerbla_("SSYMV ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*n == 0 || *alpha == 0.f && *beta == 1.f) {
	return 0;
    }
/*     Set up the start points in  X  and  Y. */
    if (*incx > 0) {
	kx = 1;
    } else {
	kx = 1 - (*n - 1) * *incx;
    }
    if (*incy > 0) {
	ky = 1;
    } else {
	ky = 1 - (*n - 1) * *incy;
    }
/*     Start the operations. In this version the elements of A are   
       accessed sequentially with one pass through the triangular part   
       of A.   
       First form  y := beta*y. */
    if (*beta != 1.f) {
	if (*incy == 1) {
	    if (*beta == 0.f) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[i__] = 0.f;
/* L10: */
		}
	    } else {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[i__] = *beta * y[i__];
/* L20: */
		}
	    }
	} else {
	    iy = ky;
	    if (*beta == 0.f) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[iy] = 0.f;
		    iy += *incy;
/* L30: */
		}
	    } else {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    y[iy] = *beta * y[iy];
		    iy += *incy;
/* L40: */
		}
	    }
	}
    }
    if (*alpha == 0.f) {
	return 0;
    }
    if (lsame_(uplo, "U")) {
/*        Form  y  when A is stored in upper triangle. */
	if (*incx == 1 && *incy == 1) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		temp1 = *alpha * x[j];
		temp2 = 0.f;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    y[i__] += temp1 * a_ref(i__, j);
		    temp2 += a_ref(i__, j) * x[i__];
/* L50: */
		}
		y[j] = y[j] + temp1 * a_ref(j, j) + *alpha * temp2;
/* L60: */
	    }
	} else {
	    jx = kx;
	    jy = ky;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		temp1 = *alpha * x[jx];
		temp2 = 0.f;
		ix = kx;
		iy = ky;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    y[iy] += temp1 * a_ref(i__, j);
		    temp2 += a_ref(i__, j) * x[ix];
		    ix += *incx;
		    iy += *incy;
/* L70: */
		}
		y[jy] = y[jy] + temp1 * a_ref(j, j) + *alpha * temp2;
		jx += *incx;
		jy += *incy;
/* L80: */
	    }
	}
    } else {
/*        Form  y  when A is stored in lower triangle. */
	if (*incx == 1 && *incy == 1) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		temp1 = *alpha * x[j];
		temp2 = 0.f;
		y[j] += temp1 * a_ref(j, j);
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    y[i__] += temp1 * a_ref(i__, j);
		    temp2 += a_ref(i__, j) * x[i__];
/* L90: */
		}
		y[j] += *alpha * temp2;
/* L100: */
	    }
	} else {
	    jx = kx;
	    jy = ky;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		temp1 = *alpha * x[jx];
		temp2 = 0.f;
		y[jy] += temp1 * a_ref(j, j);
		ix = jx;
		iy = jy;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    ix += *incx;
		    iy += *incy;
		    y[iy] += temp1 * a_ref(i__, j);
		    temp2 += a_ref(i__, j) * x[ix];
/* L110: */
		}
		y[jy] += *alpha * temp2;
		jx += *incx;
		jy += *incy;
/* L120: */
	    }
	}
    }
    return 0;
/*     End of SSYMV . */
} /* ssymv_ */
#undef a_ref




/* Subroutine */ int ssyr2_(char *uplo, integer *n, real *alpha, real *x, 
	integer *incx, real *y, integer *incy, real *a, integer *lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    /* Local variables */
    static integer info;
    static real temp1, temp2;
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer ix, iy, jx, jy, kx, ky;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
/*  Purpose   
    =======   
    SSYR2  performs the symmetric rank 2 operation   
       A := alpha*x*y' + alpha*y*x' + A,   
    where alpha is a scalar, x and y are n element vectors and A is an n   
    by n symmetric matrix.   
    Parameters   
    ==========   
    UPLO   - CHARACTER*1.   
             On entry, UPLO specifies whether the upper or lower   
             triangular part of the array A is to be referenced as   
             follows:   
                UPLO = 'U' or 'u'   Only the upper triangular part of A   
                                    is to be referenced.   
                UPLO = 'L' or 'l'   Only the lower triangular part of A   
                                    is to be referenced.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry, N specifies the order of the matrix A.   
             N must be at least zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   
    X      - REAL             array of dimension at least   
             ( 1 + ( n - 1 )*abs( INCX ) ).   
             Before entry, the incremented array X must contain the n   
             element vector x.   
             Unchanged on exit.   
    INCX   - INTEGER.   
             On entry, INCX specifies the increment for the elements of   
             X. INCX must not be zero.   
             Unchanged on exit.   
    Y      - REAL             array of dimension at least   
             ( 1 + ( n - 1 )*abs( INCY ) ).   
             Before entry, the incremented array Y must contain the n   
             element vector y.   
             Unchanged on exit.   
    INCY   - INTEGER.   
             On entry, INCY specifies the increment for the elements of   
             Y. INCY must not be zero.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, n ).   
             Before entry with  UPLO = 'U' or 'u', the leading n by n   
             upper triangular part of the array A must contain the upper   
             triangular part of the symmetric matrix and the strictly   
             lower triangular part of A is not referenced. On exit, the   
             upper triangular part of the array A is overwritten by the   
             upper triangular part of the updated matrix.   
             Before entry with UPLO = 'L' or 'l', the leading n by n   
             lower triangular part of the array A must contain the lower   
             triangular part of the symmetric matrix and the strictly   
             upper triangular part of A is not referenced. On exit, the   
             lower triangular part of the array A is overwritten by the   
             lower triangular part of the updated matrix.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program. LDA must be at least   
             f2cmax( 1, n ).   
             Unchanged on exit.   
    Level 2 Blas routine.   
    -- Written on 22-October-1986.   
       Jack Dongarra, Argonne National Lab.   
       Jeremy Du Croz, Nag Central Office.   
       Sven Hammarling, Nag Central Office.   
       Richard Hanson, Sandia National Labs.   
       Test the input parameters.   
       Parameter adjustments */
    --x;
    --y;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    /* Function Body */
    info = 0;
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	info = 1;
    } else if (*n < 0) {
	info = 2;
    } else if (*incx == 0) {
	info = 5;
    } else if (*incy == 0) {
	info = 7;
    } else if (*lda < f2cmax(1,*n)) {
	info = 9;
    }
    if (info != 0) {
	xerbla_("SSYR2 ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*n == 0 || *alpha == 0.f) {
	return 0;
    }
/*     Set up the start points in X and Y if the increments are not both   
       unity. */
    if (*incx != 1 || *incy != 1) {
	if (*incx > 0) {
	    kx = 1;
	} else {
	    kx = 1 - (*n - 1) * *incx;
	}
	if (*incy > 0) {
	    ky = 1;
	} else {
	    ky = 1 - (*n - 1) * *incy;
	}
	jx = kx;
	jy = ky;
    }
/*     Start the operations. In this version the elements of A are   
       accessed sequentially with one pass through the triangular part   
       of A. */
    if (lsame_(uplo, "U")) {
/*        Form  A  when A is stored in the upper triangle. */
	if (*incx == 1 && *incy == 1) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (x[j] != 0.f || y[j] != 0.f) {
		    temp1 = *alpha * y[j];
		    temp2 = *alpha * x[j];
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			a_ref(i__, j) = a_ref(i__, j) + x[i__] * temp1 + y[
				i__] * temp2;
/* L10: */
		    }
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (x[jx] != 0.f || y[jy] != 0.f) {
		    temp1 = *alpha * y[jy];
		    temp2 = *alpha * x[jx];
		    ix = kx;
		    iy = ky;
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			a_ref(i__, j) = a_ref(i__, j) + x[ix] * temp1 + y[iy] 
				* temp2;
			ix += *incx;
			iy += *incy;
/* L30: */
		    }
		}
		jx += *incx;
		jy += *incy;
/* L40: */
	    }
	}
    } else {
/*        Form  A  when A is stored in the lower triangle. */
	if (*incx == 1 && *incy == 1) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (x[j] != 0.f || y[j] != 0.f) {
		    temp1 = *alpha * y[j];
		    temp2 = *alpha * x[j];
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			a_ref(i__, j) = a_ref(i__, j) + x[i__] * temp1 + y[
				i__] * temp2;
/* L50: */
		    }
		}
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (x[jx] != 0.f || y[jy] != 0.f) {
		    temp1 = *alpha * y[jy];
		    temp2 = *alpha * x[jx];
		    ix = jx;
		    iy = jy;
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			a_ref(i__, j) = a_ref(i__, j) + x[ix] * temp1 + y[iy] 
				* temp2;
			ix += *incx;
			iy += *incy;
/* L70: */
		    }
		}
		jx += *incx;
		jy += *incy;
/* L80: */
	    }
	}
    }
    return 0;
/*     End of SSYR2 . */
} /* ssyr2_ */
#undef a_ref




/* Subroutine */ int ssyr2k_(char *uplo, const char *trans, integer *n, integer *k, 
	real *alpha, real *a, integer *lda, real *b, integer *ldb, real *beta,
	 real *c__, integer *ldc)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, i__1, i__2, 
	    i__3;
    /* Local variables */
    static integer info;
    static real temp1, temp2;
    static integer i__, j, l;
    extern logical lsame_(const char *, const char *);
    static integer nrowa;
    static logical upper;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]
/*  Purpose   
    =======   
    SSYR2K  performs one of the symmetric rank 2k operations   
       C := alpha*A*B' + alpha*B*A' + beta*C,   
    or   
       C := alpha*A'*B + alpha*B'*A + beta*C,   
    where  alpha and beta  are scalars, C is an  n by n  symmetric matrix   
    and  A and B  are  n by k  matrices  in the  first  case  and  k by n   
    matrices in the second case.   
    Parameters   
    ==========   
    UPLO   - CHARACTER*1.   
             On  entry,   UPLO  specifies  whether  the  upper  or  lower   
             triangular  part  of the  array  C  is to be  referenced  as   
             follows:   
                UPLO = 'U' or 'u'   Only the  upper triangular part of  C   
                                    is to be referenced.   
                UPLO = 'L' or 'l'   Only the  lower triangular part of  C   
                                    is to be referenced.   
             Unchanged on exit.   
    TRANS  - CHARACTER*1.   
             On entry,  TRANS  specifies the operation to be performed as   
             follows:   
                TRANS = 'N' or 'n'   C := alpha*A*B' + alpha*B*A' +   
                                          beta*C.   
                TRANS = 'T' or 't'   C := alpha*A'*B + alpha*B'*A +   
                                          beta*C.   
                TRANS = 'C' or 'c'   C := alpha*A'*B + alpha*B'*A +   
                                          beta*C.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry,  N specifies the order of the matrix C.  N must be   
             at least zero.   
             Unchanged on exit.   
    K      - INTEGER.   
             On entry with  TRANS = 'N' or 'n',  K  specifies  the number   
             of  columns  of the  matrices  A and B,  and on  entry  with   
             TRANS = 'T' or 't' or 'C' or 'c',  K  specifies  the  number   
             of rows of the matrices  A and B.  K must be at least  zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, ka ), where ka is   
             k  when  TRANS = 'N' or 'n',  and is  n  otherwise.   
             Before entry with  TRANS = 'N' or 'n',  the  leading  n by k   
             part of the array  A  must contain the matrix  A,  otherwise   
             the leading  k by n  part of the array  A  must contain  the   
             matrix A.   
             Unchanged on exit.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n'   
             then  LDA must be at least  f2cmax( 1, n ), otherwise  LDA must   
             be at least  f2cmax( 1, k ).   
             Unchanged on exit.   
    B      - REAL             array of DIMENSION ( LDB, kb ), where kb is   
             k  when  TRANS = 'N' or 'n',  and is  n  otherwise.   
             Before entry with  TRANS = 'N' or 'n',  the  leading  n by k   
             part of the array  B  must contain the matrix  B,  otherwise   
             the leading  k by n  part of the array  B  must contain  the   
             matrix B.   
             Unchanged on exit.   
    LDB    - INTEGER.   
             On entry, LDB specifies the first dimension of B as declared   
             in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n'   
             then  LDB must be at least  f2cmax( 1, n ), otherwise  LDB must   
             be at least  f2cmax( 1, k ).   
             Unchanged on exit.   
    BETA   - REAL            .   
             On entry, BETA specifies the scalar beta.   
             Unchanged on exit.   
    C      - REAL             array of DIMENSION ( LDC, n ).   
             Before entry  with  UPLO = 'U' or 'u',  the leading  n by n   
             upper triangular part of the array C must contain the upper   
             triangular part  of the  symmetric matrix  and the strictly   
             lower triangular part of C is not referenced.  On exit, the   
             upper triangular part of the array  C is overwritten by the   
             upper triangular part of the updated matrix.   
             Before entry  with  UPLO = 'L' or 'l',  the leading  n by n   
             lower triangular part of the array C must contain the lower   
             triangular part  of the  symmetric matrix  and the strictly   
             upper triangular part of C is not referenced.  On exit, the   
             lower triangular part of the array  C is overwritten by the   
             lower triangular part of the updated matrix.   
    LDC    - INTEGER.   
             On entry, LDC specifies the first dimension of C as declared   
             in  the  calling  (sub)  program.   LDC  must  be  at  least   
             f2cmax( 1, n ).   
             Unchanged on exit.   
    Level 3 Blas routine.   
    -- Written on 8-February-1989.   
       Jack Dongarra, Argonne National Laboratory.   
       Iain Duff, AERE Harwell.   
       Jeremy Du Croz, Numerical Algorithms Group Ltd.   
       Sven Hammarling, Numerical Algorithms Group Ltd.   
       Test the input parameters.   
       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    /* Function Body */
    if (lsame_(trans, "N")) {
	nrowa = *n;
    } else {
	nrowa = *k;
    }
    upper = lsame_(uplo, "U");
    info = 0;
    if (! upper && ! lsame_(uplo, "L")) {
	info = 1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans, 
	    "T") && ! lsame_(trans, "C")) {
	info = 2;
    } else if (*n < 0) {
	info = 3;
    } else if (*k < 0) {
	info = 4;
    } else if (*lda < f2cmax(1,nrowa)) {
	info = 7;
    } else if (*ldb < f2cmax(1,nrowa)) {
	info = 9;
    } else if (*ldc < f2cmax(1,*n)) {
	info = 12;
    }
    if (info != 0) {
	xerbla_("SSYR2K", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*n == 0 || (*alpha == 0.f || *k == 0) && *beta == 1.f) {
	return 0;
    }
/*     And when  alpha.eq.zero. */
    if (*alpha == 0.f) {
	if (upper) {
	    if (*beta == 0.f) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = 0.f;
/* L10: */
		    }
/* L20: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = *beta * c___ref(i__, j);
/* L30: */
		    }
/* L40: */
		}
	    }
	} else {
	    if (*beta == 0.f) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			c___ref(i__, j) = 0.f;
/* L50: */
		    }
/* L60: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			c___ref(i__, j) = *beta * c___ref(i__, j);
/* L70: */
		    }
/* L80: */
		}
	    }
	}
	return 0;
    }
/*     Start the operations. */
    if (lsame_(trans, "N")) {
/*        Form  C := alpha*A*B' + alpha*B*A' + C. */
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (*beta == 0.f) {
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = 0.f;
/* L90: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c___ref(i__, j) = *beta * c___ref(i__, j);
/* L100: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= i__2; ++l) {
		    if (a_ref(j, l) != 0.f || b_ref(j, l) != 0.f) {
			temp1 = *alpha * b_ref(j, l);
			temp2 = *alpha * a_ref(j, l);
			i__3 = j;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    c___ref(i__, j) = c___ref(i__, j) + a_ref(i__, l) 
				    * temp1 + b_ref(i__, l) * temp2;
/* L110: */
			}
		    }
/* L120: */
		}
/* L130: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (*beta == 0.f) {
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			c___ref(i__, j) = 0.f;
/* L140: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			c___ref(i__, j) = *beta * c___ref(i__, j);
/* L150: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= i__2; ++l) {
		    if (a_ref(j, l) != 0.f || b_ref(j, l) != 0.f) {
			temp1 = *alpha * b_ref(j, l);
			temp2 = *alpha * a_ref(j, l);
			i__3 = *n;
			for (i__ = j; i__ <= i__3; ++i__) {
			    c___ref(i__, j) = c___ref(i__, j) + a_ref(i__, l) 
				    * temp1 + b_ref(i__, l) * temp2;
/* L160: */
			}
		    }
/* L170: */
		}
/* L180: */
	    }
	}
    } else {
/*        Form  C := alpha*A'*B + alpha*B'*A + C. */
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    temp1 = 0.f;
		    temp2 = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= i__3; ++l) {
			temp1 += a_ref(l, i__) * b_ref(l, j);
			temp2 += b_ref(l, i__) * a_ref(l, j);
/* L190: */
		    }
		    if (*beta == 0.f) {
			c___ref(i__, j) = *alpha * temp1 + *alpha * temp2;
		    } else {
			c___ref(i__, j) = *beta * c___ref(i__, j) + *alpha * 
				temp1 + *alpha * temp2;
		    }
/* L200: */
		}
/* L210: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    temp1 = 0.f;
		    temp2 = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= i__3; ++l) {
			temp1 += a_ref(l, i__) * b_ref(l, j);
			temp2 += b_ref(l, i__) * a_ref(l, j);
/* L220: */
		    }
		    if (*beta == 0.f) {
			c___ref(i__, j) = *alpha * temp1 + *alpha * temp2;
		    } else {
			c___ref(i__, j) = *beta * c___ref(i__, j) + *alpha * 
				temp1 + *alpha * temp2;
		    }
/* L230: */
		}
/* L240: */
	    }
	}
    }
    return 0;
/*     End of SSYR2K. */
} /* ssyr2k_ */
#undef c___ref
#undef b_ref
#undef a_ref




/* Subroutine */ int ssytd2_(char *uplo, integer *n, real *a, integer *lda, 
	real *d__, real *e, real *tau, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SSYTD2 reduces a real symmetric matrix A to symmetric tridiagonal   
    form T by an orthogonal similarity transformation: Q' * A * Q = T.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies whether the upper or lower triangular part of the   
            symmetric matrix A is stored:   
            = 'U':  Upper triangular   
            = 'L':  Lower triangular   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading   
            n-by-n upper triangular part of A contains the upper   
            triangular part of the matrix A, and the strictly lower   
            triangular part of A is not referenced.  If UPLO = 'L', the   
            leading n-by-n lower triangular part of A contains the lower   
            triangular part of the matrix A, and the strictly upper   
            triangular part of A is not referenced.   
            On exit, if UPLO = 'U', the diagonal and first superdiagonal   
            of A are overwritten by the corresponding elements of the   
            tridiagonal matrix T, and the elements above the first   
            superdiagonal, with the array TAU, represent the orthogonal   
            matrix Q as a product of elementary reflectors; if UPLO   
            = 'L', the diagonal and first subdiagonal of A are over-   
            written by the corresponding elements of the tridiagonal   
            matrix T, and the elements below the first subdiagonal, with   
            the array TAU, represent the orthogonal matrix Q as a product   
            of elementary reflectors. See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(1,N).   

    D       (output) REAL array, dimension (N)   
            The diagonal elements of the tridiagonal matrix T:   
            D(i) = A(i,i).   

    E       (output) REAL array, dimension (N-1)   
            The off-diagonal elements of the tridiagonal matrix T:   
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.   

    TAU     (output) REAL array, dimension (N-1)   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    If UPLO = 'U', the matrix Q is represented as a product of elementary   
    reflectors   

       Q = H(n-1) . . . H(2) H(1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in   
    A(1:i-1,i+1), and tau in TAU(i).   

    If UPLO = 'L', the matrix Q is represented as a product of elementary   
    reflectors   

       Q = H(1) H(2) . . . H(n-1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i),   
    and tau in TAU(i).   

    The contents of A on exit are illustrated by the following examples   
    with n = 5:   

    if UPLO = 'U':                       if UPLO = 'L':   

      (  d   e   v2  v3  v4 )              (  d                  )   
      (      d   e   v3  v4 )              (  e   d              )   
      (          d   e   v4 )              (  v1  e   d          )   
      (              d   e  )              (  v1  v2  e   d      )   
      (                  d  )              (  v1  v2  v3  e   d  )   

    where d and e denote diagonal and off-diagonal elements of T, and vi   
    denotes an element of the vector defining H(i).   

    =====================================================================   


       Test the input parameters   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static real c_b8 = 0.f;
    static real c_b14 = -1.f;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static real taui;
    extern doublereal sdot_(integer *, real *, integer *, real *, integer *);
    static integer i__;
    extern /* Subroutine */ int ssyr2_(char *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *);
    static real alpha;
    extern logical lsame_(const char *, const char *);
    static logical upper;
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *), ssymv_(const char *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, real *, integer *), 
	    xerbla_(const char *, integer *), slarfg_(integer *, real *, 
	    real *, integer *, real *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tau;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSYTD2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

    if (upper) {

/*        Reduce the upper triangle of A */

	for (i__ = *n - 1; i__ >= 1; --i__) {

/*           Generate elementary reflector H(i) = I - tau * v * v'   
             to annihilate A(1:i-1,i+1) */

	    slarfg_(&i__, &a_ref(i__, i__ + 1), &a_ref(1, i__ + 1), &c__1, &
		    taui);
	    e[i__] = a_ref(i__, i__ + 1);

	    if (taui != 0.f) {

/*              Apply H(i) from both sides to A(1:i,1:i) */

		a_ref(i__, i__ + 1) = 1.f;

/*              Compute  x := tau * A * v  storing x in TAU(1:i) */

		ssymv_(uplo, &i__, &taui, &a[a_offset], lda, &a_ref(1, i__ + 
			1), &c__1, &c_b8, &tau[1], &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		alpha = taui * -.5f * sdot_(&i__, &tau[1], &c__1, &a_ref(1, 
			i__ + 1), &c__1);
		saxpy_(&i__, &alpha, &a_ref(1, i__ + 1), &c__1, &tau[1], &
			c__1);

/*              Apply the transformation as a rank-2 update:   
                   A := A - v * w' - w * v' */

		ssyr2_(uplo, &i__, &c_b14, &a_ref(1, i__ + 1), &c__1, &tau[1],
			 &c__1, &a[a_offset], lda);

		a_ref(i__, i__ + 1) = e[i__];
	    }
	    d__[i__ + 1] = a_ref(i__ + 1, i__ + 1);
	    tau[i__] = taui;
/* L10: */
	}
	d__[1] = a_ref(1, 1);
    } else {

/*        Reduce the lower triangle of A */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector H(i) = I - tau * v * v'   
             to annihilate A(i+2:n,i)   

   Computing MIN */
	    i__2 = i__ + 2;
	    i__3 = *n - i__;
	    slarfg_(&i__3, &a_ref(i__ + 1, i__), &a_ref(f2cmin(i__2,*n), i__), &
		    c__1, &taui);
	    e[i__] = a_ref(i__ + 1, i__);

	    if (taui != 0.f) {

/*              Apply H(i) from both sides to A(i+1:n,i+1:n) */

		a_ref(i__ + 1, i__) = 1.f;

/*              Compute  x := tau * A * v  storing y in TAU(i:n-1) */

		i__2 = *n - i__;
		ssymv_(uplo, &i__2, &taui, &a_ref(i__ + 1, i__ + 1), lda, &
			a_ref(i__ + 1, i__), &c__1, &c_b8, &tau[i__], &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		i__2 = *n - i__;
		alpha = taui * -.5f * sdot_(&i__2, &tau[i__], &c__1, &a_ref(
			i__ + 1, i__), &c__1);
		i__2 = *n - i__;
		saxpy_(&i__2, &alpha, &a_ref(i__ + 1, i__), &c__1, &tau[i__], 
			&c__1);

/*              Apply the transformation as a rank-2 update:   
                   A := A - v * w' - w * v' */

		i__2 = *n - i__;
		ssyr2_(uplo, &i__2, &c_b14, &a_ref(i__ + 1, i__), &c__1, &tau[
			i__], &c__1, &a_ref(i__ + 1, i__ + 1), lda)
			;

		a_ref(i__ + 1, i__) = e[i__];
	    }
	    d__[i__] = a_ref(i__, i__);
	    tau[i__] = taui;
/* L20: */
	}
	d__[*n] = a_ref(*n, *n);
    }

    return 0;

/*     End of SSYTD2 */

} /* ssytd2_ */

#undef a_ref





/* Subroutine */ int ssytrd_(char *uplo, integer *n, real *a, integer *lda, 
	real *d__, real *e, real *tau, real *work, integer *lwork, integer *
	info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SSYTRD reduces a real symmetric matrix A to real symmetric   
    tridiagonal form T by an orthogonal similarity transformation:   
    Q**T * A * Q = T.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            = 'U':  Upper triangle of A is stored;   
            = 'L':  Lower triangle of A is stored.   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading   
            N-by-N upper triangular part of A contains the upper   
            triangular part of the matrix A, and the strictly lower   
            triangular part of A is not referenced.  If UPLO = 'L', the   
            leading N-by-N lower triangular part of A contains the lower   
            triangular part of the matrix A, and the strictly upper   
            triangular part of A is not referenced.   
            On exit, if UPLO = 'U', the diagonal and first superdiagonal   
            of A are overwritten by the corresponding elements of the   
            tridiagonal matrix T, and the elements above the first   
            superdiagonal, with the array TAU, represent the orthogonal   
            matrix Q as a product of elementary reflectors; if UPLO   
            = 'L', the diagonal and first subdiagonal of A are over-   
            written by the corresponding elements of the tridiagonal   
            matrix T, and the elements below the first subdiagonal, with   
            the array TAU, represent the orthogonal matrix Q as a product   
            of elementary reflectors. See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= f2cmax(1,N).   

    D       (output) REAL array, dimension (N)   
            The diagonal elements of the tridiagonal matrix T:   
            D(i) = A(i,i).   

    E       (output) REAL array, dimension (N-1)   
            The off-diagonal elements of the tridiagonal matrix T:   
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.   

    TAU     (output) REAL array, dimension (N-1)   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.  LWORK >= 1.   
            For optimum performance LWORK >= N*NB, where NB is the   
            optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    Further Details   
    ===============   

    If UPLO = 'U', the matrix Q is represented as a product of elementary   
    reflectors   

       Q = H(n-1) . . . H(2) H(1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in   
    A(1:i-1,i+1), and tau in TAU(i).   

    If UPLO = 'L', the matrix Q is represented as a product of elementary   
    reflectors   

       Q = H(1) H(2) . . . H(n-1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i),   
    and tau in TAU(i).   

    The contents of A on exit are illustrated by the following examples   
    with n = 5:   

    if UPLO = 'U':                       if UPLO = 'L':   

      (  d   e   v2  v3  v4 )              (  d                  )   
      (      d   e   v3  v4 )              (  e   d              )   
      (          d   e   v4 )              (  v1  e   d          )   
      (              d   e  )              (  v1  v2  e   d      )   
      (                  d  )              (  v1  v2  v3  e   d  )   

    where d and e denote diagonal and off-diagonal elements of T, and vi   
    denotes an element of the vector defining H(i).   

    =====================================================================   


       Test the input parameters   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    static real c_b22 = -1.f;
    static real c_b23 = 1.f;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer nbmin, iinfo;
    static logical upper;
    static integer nb, kk;
    extern /* Subroutine */ int ssytd2_(char *, integer *, real *, integer *, 
	    real *, real *, real *, integer *), ssyr2k_(char *, const char *
	    , integer *, integer *, real *, real *, integer *, real *, 
	    integer *, real *, real *, integer *);
    static integer nx;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slatrd_(char *, integer *, integer *, real *, 
	    integer *, real *, real *, real *, integer *);
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*n)) {
	*info = -4;
    } else if (*lwork < 1 && ! lquery) {
	*info = -9;
    }

    if (*info == 0) {

/*        Determine the block size. */

	nb = ilaenv_(&c__1, "SSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, (ftnlen)6,
		 (ftnlen)1);
	lwkopt = *n * nb;
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSYTRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	work[1] = 1.f;
	return 0;
    }

    nx = *n;
    iws = 1;
    if (nb > 1 && nb < *n) {

/*        Determine when to cross over from blocked to unblocked code   
          (last block is always handled by unblocked code).   

   Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__3, "SSYTRD", uplo, n, &c_n1, &c_n1, &
		c_n1, (ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);
	if (nx < *n) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  determine the   
                minimum value of NB, and reduce NB or force use of   
                unblocked code by setting NX = N.   

   Computing MAX */
		i__1 = *lwork / ldwork;
		nb = f2cmax(i__1,1);
		nbmin = ilaenv_(&c__2, "SSYTRD", uplo, n, &c_n1, &c_n1, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		if (nb < nbmin) {
		    nx = *n;
		}
	    }
	} else {
	    nx = *n;
	}
    } else {
	nb = 1;
    }

    if (upper) {

/*        Reduce the upper triangle of A.   
          Columns 1:kk are handled by the unblocked method. */

	kk = *n - (*n - nx + nb - 1) / nb * nb;
	i__1 = kk + 1;
	i__2 = -nb;
	for (i__ = *n - nb + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += 
		i__2) {

/*           Reduce columns i:i+nb-1 to tridiagonal form and form the   
             matrix W which is needed to update the unreduced part of   
             the matrix */

	    i__3 = i__ + nb - 1;
	    slatrd_(uplo, &i__3, &nb, &a[a_offset], lda, &e[1], &tau[1], &
		    work[1], &ldwork);

/*           Update the unreduced submatrix A(1:i-1,1:i-1), using an   
             update of the form:  A := A - V*W' - W*V' */

	    i__3 = i__ - 1;
	    ssyr2k_(uplo, "No transpose", &i__3, &nb, &c_b22, &a_ref(1, i__), 
		    lda, &work[1], &ldwork, &c_b23, &a[a_offset], lda);

/*           Copy superdiagonal elements back into A, and diagonal   
             elements into D */

	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a_ref(j - 1, j) = e[j - 1];
		d__[j] = a_ref(j, j);
/* L10: */
	    }
/* L20: */
	}

/*        Use unblocked code to reduce the last or only block */

	ssytd2_(uplo, &kk, &a[a_offset], lda, &d__[1], &e[1], &tau[1], &iinfo);
    } else {

/*        Reduce the lower triangle of A */

	i__2 = *n - nx;
	i__1 = nb;
	for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__1) {

/*           Reduce columns i:i+nb-1 to tridiagonal form and form the   
             matrix W which is needed to update the unreduced part of   
             the matrix */

	    i__3 = *n - i__ + 1;
	    slatrd_(uplo, &i__3, &nb, &a_ref(i__, i__), lda, &e[i__], &tau[
		    i__], &work[1], &ldwork);

/*           Update the unreduced submatrix A(i+ib:n,i+ib:n), using   
             an update of the form:  A := A - V*W' - W*V' */

	    i__3 = *n - i__ - nb + 1;
	    ssyr2k_(uplo, "No transpose", &i__3, &nb, &c_b22, &a_ref(i__ + nb,
		     i__), lda, &work[nb + 1], &ldwork, &c_b23, &a_ref(i__ + 
		    nb, i__ + nb), lda);

/*           Copy subdiagonal elements back into A, and diagonal   
             elements into D */

	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a_ref(j + 1, j) = e[j];
		d__[j] = a_ref(j, j);
/* L30: */
	    }
/* L40: */
	}

/*        Use unblocked code to reduce the last or only block */

	i__1 = *n - i__ + 1;
	ssytd2_(uplo, &i__1, &a_ref(i__, i__), lda, &d__[i__], &e[i__], &tau[
		i__], &iinfo);
    }

    work[1] = (real) lwkopt;
    return 0;

/*     End of SSYTRD */

} /* ssytrd_ */

#undef a_ref





/* Subroutine */ int strmm_(const char *side, const char *uplo, const char *transa, const char *diag, 
	integer *m, integer *n, real *alpha, real *a, integer *lda, real *b, 
	integer *ldb)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer info;
    static real temp;
    static integer i__, j, k;
    static logical lside;
    extern logical lsame_(const char *, const char *);
    static integer nrowa;
    static logical upper;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static logical nounit;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
/*  Purpose   
    =======   
    STRMM  performs one of the matrix-matrix operations   
       B := alpha*op( A )*B,   or   B := alpha*B*op( A ),   
    where  alpha  is a scalar,  B  is an m by n matrix,  A  is a unit, or   
    non-unit,  upper or lower triangular matrix  and  op( A )  is one  of   
       op( A ) = A   or   op( A ) = A'.   
    Parameters   
    ==========   
    SIDE   - CHARACTER*1.   
             On entry,  SIDE specifies whether  op( A ) multiplies B from   
             the left or right as follows:   
                SIDE = 'L' or 'l'   B := alpha*op( A )*B.   
                SIDE = 'R' or 'r'   B := alpha*B*op( A ).   
             Unchanged on exit.   
    UPLO   - CHARACTER*1.   
             On entry, UPLO specifies whether the matrix A is an upper or   
             lower triangular matrix as follows:   
                UPLO = 'U' or 'u'   A is an upper triangular matrix.   
                UPLO = 'L' or 'l'   A is a lower triangular matrix.   
             Unchanged on exit.   
    TRANSA - CHARACTER*1.   
             On entry, TRANSA specifies the form of op( A ) to be used in   
             the matrix multiplication as follows:   
                TRANSA = 'N' or 'n'   op( A ) = A.   
                TRANSA = 'T' or 't'   op( A ) = A'.   
                TRANSA = 'C' or 'c'   op( A ) = A'.   
             Unchanged on exit.   
    DIAG   - CHARACTER*1.   
             On entry, DIAG specifies whether or not A is unit triangular   
             as follows:   
                DIAG = 'U' or 'u'   A is assumed to be unit triangular.   
                DIAG = 'N' or 'n'   A is not assumed to be unit   
                                    triangular.   
             Unchanged on exit.   
    M      - INTEGER.   
             On entry, M specifies the number of rows of B. M must be at   
             least zero.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry, N specifies the number of columns of B.  N must be   
             at least zero.   
             Unchanged on exit.   
    ALPHA  - REAL            .   
             On entry,  ALPHA specifies the scalar  alpha. When  alpha is   
             zero then  A is not referenced and  B need not be set before   
             entry.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, k ), where k is m   
             when  SIDE = 'L' or 'l'  and is  n  when  SIDE = 'R' or 'r'.   
             Before entry  with  UPLO = 'U' or 'u',  the  leading  k by k   
             upper triangular part of the array  A must contain the upper   
             triangular matrix  and the strictly lower triangular part of   
             A is not referenced.   
             Before entry  with  UPLO = 'L' or 'l',  the  leading  k by k   
             lower triangular part of the array  A must contain the lower   
             triangular matrix  and the strictly upper triangular part of   
             A is not referenced.   
             Note that when  DIAG = 'U' or 'u',  the diagonal elements of   
             A  are not referenced either,  but are assumed to be  unity.   
             Unchanged on exit.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program.  When  SIDE = 'L' or 'l'  then   
             LDA  must be at least  f2cmax( 1, m ),  when  SIDE = 'R' or 'r'   
             then LDA must be at least f2cmax( 1, n ).   
             Unchanged on exit.   
    B      - REAL             array of DIMENSION ( LDB, n ).   
             Before entry,  the leading  m by n part of the array  B must   
             contain the matrix  B,  and  on exit  is overwritten  by the   
             transformed matrix.   
    LDB    - INTEGER.   
             On entry, LDB specifies the first dimension of B as declared   
             in  the  calling  (sub)  program.   LDB  must  be  at  least   
             f2cmax( 1, m ).   
             Unchanged on exit.   
    Level 3 Blas routine.   
    -- Written on 8-February-1989.   
       Jack Dongarra, Argonne National Laboratory.   
       Iain Duff, AERE Harwell.   
       Jeremy Du Croz, Numerical Algorithms Group Ltd.   
       Sven Hammarling, Numerical Algorithms Group Ltd.   
       Test the input parameters.   
       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    /* Function Body */
    lside = lsame_(side, "L");
    if (lside) {
	nrowa = *m;
    } else {
	nrowa = *n;
    }
    nounit = lsame_(diag, "N");
    upper = lsame_(uplo, "U");
    info = 0;
    if (! lside && ! lsame_(side, "R")) {
	info = 1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	info = 2;
    } else if (! lsame_(transa, "N") && ! lsame_(transa,
	     "T") && ! lsame_(transa, "C")) {
	info = 3;
    } else if (! lsame_(diag, "U") && ! lsame_(diag, 
	    "N")) {
	info = 4;
    } else if (*m < 0) {
	info = 5;
    } else if (*n < 0) {
	info = 6;
    } else if (*lda < f2cmax(1,nrowa)) {
	info = 9;
    } else if (*ldb < f2cmax(1,*m)) {
	info = 11;
    }
    if (info != 0) {
	xerbla_("STRMM ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*n == 0) {
	return 0;
    }
/*     And when  alpha.eq.zero. */
    if (*alpha == 0.f) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b_ref(i__, j) = 0.f;
/* L10: */
	    }
/* L20: */
	}
	return 0;
    }
/*     Start the operations. */
    if (lside) {
	if (lsame_(transa, "N")) {
/*           Form  B := alpha*A*B. */
	    if (upper) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (k = 1; k <= i__2; ++k) {
			if (b_ref(k, j) != 0.f) {
			    temp = *alpha * b_ref(k, j);
			    i__3 = k - 1;
			    for (i__ = 1; i__ <= i__3; ++i__) {
				b_ref(i__, j) = b_ref(i__, j) + temp * a_ref(
					i__, k);
/* L30: */
			    }
			    if (nounit) {
				temp *= a_ref(k, k);
			    }
			    b_ref(k, j) = temp;
			}
/* L40: */
		    }
/* L50: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    for (k = *m; k >= 1; --k) {
			if (b_ref(k, j) != 0.f) {
			    temp = *alpha * b_ref(k, j);
			    b_ref(k, j) = temp;
			    if (nounit) {
				b_ref(k, j) = b_ref(k, j) * a_ref(k, k);
			    }
			    i__2 = *m;
			    for (i__ = k + 1; i__ <= i__2; ++i__) {
				b_ref(i__, j) = b_ref(i__, j) + temp * a_ref(
					i__, k);
/* L60: */
			    }
			}
/* L70: */
		    }
/* L80: */
		}
	    }
	} else {
/*           Form  B := alpha*A'*B. */
	    if (upper) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    for (i__ = *m; i__ >= 1; --i__) {
			temp = b_ref(i__, j);
			if (nounit) {
			    temp *= a_ref(i__, i__);
			}
			i__2 = i__ - 1;
			for (k = 1; k <= i__2; ++k) {
			    temp += a_ref(k, i__) * b_ref(k, j);
/* L90: */
			}
			b_ref(i__, j) = *alpha * temp;
/* L100: */
		    }
/* L110: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			temp = b_ref(i__, j);
			if (nounit) {
			    temp *= a_ref(i__, i__);
			}
			i__3 = *m;
			for (k = i__ + 1; k <= i__3; ++k) {
			    temp += a_ref(k, i__) * b_ref(k, j);
/* L120: */
			}
			b_ref(i__, j) = *alpha * temp;
/* L130: */
		    }
/* L140: */
		}
	    }
	}
    } else {
	if (lsame_(transa, "N")) {
/*           Form  B := alpha*B*A. */
	    if (upper) {
		for (j = *n; j >= 1; --j) {
		    temp = *alpha;
		    if (nounit) {
			temp *= a_ref(j, j);
		    }
		    i__1 = *m;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			b_ref(i__, j) = temp * b_ref(i__, j);
/* L150: */
		    }
		    i__1 = j - 1;
		    for (k = 1; k <= i__1; ++k) {
			if (a_ref(k, j) != 0.f) {
			    temp = *alpha * a_ref(k, j);
			    i__2 = *m;
			    for (i__ = 1; i__ <= i__2; ++i__) {
				b_ref(i__, j) = b_ref(i__, j) + temp * b_ref(
					i__, k);
/* L160: */
			    }
			}
/* L170: */
		    }
/* L180: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    temp = *alpha;
		    if (nounit) {
			temp *= a_ref(j, j);
		    }
		    i__2 = *m;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			b_ref(i__, j) = temp * b_ref(i__, j);
/* L190: */
		    }
		    i__2 = *n;
		    for (k = j + 1; k <= i__2; ++k) {
			if (a_ref(k, j) != 0.f) {
			    temp = *alpha * a_ref(k, j);
			    i__3 = *m;
			    for (i__ = 1; i__ <= i__3; ++i__) {
				b_ref(i__, j) = b_ref(i__, j) + temp * b_ref(
					i__, k);
/* L200: */
			    }
			}
/* L210: */
		    }
/* L220: */
		}
	    }
	} else {
/*           Form  B := alpha*B*A'. */
	    if (upper) {
		i__1 = *n;
		for (k = 1; k <= i__1; ++k) {
		    i__2 = k - 1;
		    for (j = 1; j <= i__2; ++j) {
			if (a_ref(j, k) != 0.f) {
			    temp = *alpha * a_ref(j, k);
			    i__3 = *m;
			    for (i__ = 1; i__ <= i__3; ++i__) {
				b_ref(i__, j) = b_ref(i__, j) + temp * b_ref(
					i__, k);
/* L230: */
			    }
			}
/* L240: */
		    }
		    temp = *alpha;
		    if (nounit) {
			temp *= a_ref(k, k);
		    }
		    if (temp != 1.f) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    b_ref(i__, k) = temp * b_ref(i__, k);
/* L250: */
			}
		    }
/* L260: */
		}
	    } else {
		for (k = *n; k >= 1; --k) {
		    i__1 = *n;
		    for (j = k + 1; j <= i__1; ++j) {
			if (a_ref(j, k) != 0.f) {
			    temp = *alpha * a_ref(j, k);
			    i__2 = *m;
			    for (i__ = 1; i__ <= i__2; ++i__) {
				b_ref(i__, j) = b_ref(i__, j) + temp * b_ref(
					i__, k);
/* L270: */
			    }
			}
/* L280: */
		    }
		    temp = *alpha;
		    if (nounit) {
			temp *= a_ref(k, k);
		    }
		    if (temp != 1.f) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    b_ref(i__, k) = temp * b_ref(i__, k);
/* L290: */
			}
		    }
/* L300: */
		}
	    }
	}
    }
    return 0;
/*     End of STRMM . */
} /* strmm_ */
#undef b_ref
#undef a_ref




/* Subroutine */ int strmv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	real *a, integer *lda, real *x, integer *incx)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    /* Local variables */
    static integer info;
    static real temp;
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer ix, jx, kx;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static logical nounit;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
/*  Purpose   
    =======   
    STRMV  performs one of the matrix-vector operations   
       x := A*x,   or   x := A'*x,   
    where x is an n element vector and  A is an n by n unit, or non-unit,   
    upper or lower triangular matrix.   
    Parameters   
    ==========   
    UPLO   - CHARACTER*1.   
             On entry, UPLO specifies whether the matrix is an upper or   
             lower triangular matrix as follows:   
                UPLO = 'U' or 'u'   A is an upper triangular matrix.   
                UPLO = 'L' or 'l'   A is a lower triangular matrix.   
             Unchanged on exit.   
    TRANS  - CHARACTER*1.   
             On entry, TRANS specifies the operation to be performed as   
             follows:   
                TRANS = 'N' or 'n'   x := A*x.   
                TRANS = 'T' or 't'   x := A'*x.   
                TRANS = 'C' or 'c'   x := A'*x.   
             Unchanged on exit.   
    DIAG   - CHARACTER*1.   
             On entry, DIAG specifies whether or not A is unit   
             triangular as follows:   
                DIAG = 'U' or 'u'   A is assumed to be unit triangular.   
                DIAG = 'N' or 'n'   A is not assumed to be unit   
                                    triangular.   
             Unchanged on exit.   
    N      - INTEGER.   
             On entry, N specifies the order of the matrix A.   
             N must be at least zero.   
             Unchanged on exit.   
    A      - REAL             array of DIMENSION ( LDA, n ).   
             Before entry with  UPLO = 'U' or 'u', the leading n by n   
             upper triangular part of the array A must contain the upper   
             triangular matrix and the strictly lower triangular part of   
             A is not referenced.   
             Before entry with UPLO = 'L' or 'l', the leading n by n   
             lower triangular part of the array A must contain the lower   
             triangular matrix and the strictly upper triangular part of   
             A is not referenced.   
             Note that when  DIAG = 'U' or 'u', the diagonal elements of   
             A are not referenced either, but are assumed to be unity.   
             Unchanged on exit.   
    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared   
             in the calling (sub) program. LDA must be at least   
             f2cmax( 1, n ).   
             Unchanged on exit.   
    X      - REAL             array of dimension at least   
             ( 1 + ( n - 1 )*abs( INCX ) ).   
             Before entry, the incremented array X must contain the n   
             element vector x. On exit, X is overwritten with the   
             tranformed vector x.   
    INCX   - INTEGER.   
             On entry, INCX specifies the increment for the elements of   
             X. INCX must not be zero.   
             Unchanged on exit.   
    Level 2 Blas routine.   
    -- Written on 22-October-1986.   
       Jack Dongarra, Argonne National Lab.   
       Jeremy Du Croz, Nag Central Office.   
       Sven Hammarling, Nag Central Office.   
       Richard Hanson, Sandia National Labs.   
       Test the input parameters.   
       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --x;
    /* Function Body */
    info = 0;
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	info = 1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans, 
	    "T") && ! lsame_(trans, "C")) {
	info = 2;
    } else if (! lsame_(diag, "U") && ! lsame_(diag, 
	    "N")) {
	info = 3;
    } else if (*n < 0) {
	info = 4;
    } else if (*lda < f2cmax(1,*n)) {
	info = 6;
    } else if (*incx == 0) {
	info = 8;
    }
    if (info != 0) {
	xerbla_("STRMV ", &info);
	return 0;
    }
/*     Quick return if possible. */
    if (*n == 0) {
	return 0;
    }
    nounit = lsame_(diag, "N");
/*     Set up the start point in X if the increment is not unity. This   
       will be  ( N - 1 )*INCX  too small for descending loops. */
    if (*incx <= 0) {
	kx = 1 - (*n - 1) * *incx;
    } else if (*incx != 1) {
	kx = 1;
    }
/*     Start the operations. In this version the elements of A are   
       accessed sequentially with one pass through A. */
    if (lsame_(trans, "N")) {
/*        Form  x := A*x. */
	if (lsame_(uplo, "U")) {
	    if (*incx == 1) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    if (x[j] != 0.f) {
			temp = x[j];
			i__2 = j - 1;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    x[i__] += temp * a_ref(i__, j);
/* L10: */
			}
			if (nounit) {
			    x[j] *= a_ref(j, j);
			}
		    }
/* L20: */
		}
	    } else {
		jx = kx;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    if (x[jx] != 0.f) {
			temp = x[jx];
			ix = kx;
			i__2 = j - 1;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    x[ix] += temp * a_ref(i__, j);
			    ix += *incx;
/* L30: */
			}
			if (nounit) {
			    x[jx] *= a_ref(j, j);
			}
		    }
		    jx += *incx;
/* L40: */
		}
	    }
	} else {
	    if (*incx == 1) {
		for (j = *n; j >= 1; --j) {
		    if (x[j] != 0.f) {
			temp = x[j];
			i__1 = j + 1;
			for (i__ = *n; i__ >= i__1; --i__) {
			    x[i__] += temp * a_ref(i__, j);
/* L50: */
			}
			if (nounit) {
			    x[j] *= a_ref(j, j);
			}
		    }
/* L60: */
		}
	    } else {
		kx += (*n - 1) * *incx;
		jx = kx;
		for (j = *n; j >= 1; --j) {
		    if (x[jx] != 0.f) {
			temp = x[jx];
			ix = kx;
			i__1 = j + 1;
			for (i__ = *n; i__ >= i__1; --i__) {
			    x[ix] += temp * a_ref(i__, j);
			    ix -= *incx;
/* L70: */
			}
			if (nounit) {
			    x[jx] *= a_ref(j, j);
			}
		    }
		    jx -= *incx;
/* L80: */
		}
	    }
	}
    } else {
/*        Form  x := A'*x. */
	if (lsame_(uplo, "U")) {
	    if (*incx == 1) {
		for (j = *n; j >= 1; --j) {
		    temp = x[j];
		    if (nounit) {
			temp *= a_ref(j, j);
		    }
		    for (i__ = j - 1; i__ >= 1; --i__) {
			temp += a_ref(i__, j) * x[i__];
/* L90: */
		    }
		    x[j] = temp;
/* L100: */
		}
	    } else {
		jx = kx + (*n - 1) * *incx;
		for (j = *n; j >= 1; --j) {
		    temp = x[jx];
		    ix = jx;
		    if (nounit) {
			temp *= a_ref(j, j);
		    }
		    for (i__ = j - 1; i__ >= 1; --i__) {
			ix -= *incx;
			temp += a_ref(i__, j) * x[ix];
/* L110: */
		    }
		    x[jx] = temp;
		    jx -= *incx;
/* L120: */
		}
	    }
	} else {
	    if (*incx == 1) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    temp = x[j];
		    if (nounit) {
			temp *= a_ref(j, j);
		    }
		    i__2 = *n;
		    for (i__ = j + 1; i__ <= i__2; ++i__) {
			temp += a_ref(i__, j) * x[i__];
/* L130: */
		    }
		    x[j] = temp;
/* L140: */
		}
	    } else {
		jx = kx;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    temp = x[jx];
		    ix = jx;
		    if (nounit) {
			temp *= a_ref(j, j);
		    }
		    i__2 = *n;
		    for (i__ = j + 1; i__ <= i__2; ++i__) {
			ix += *incx;
			temp += a_ref(i__, j) * x[ix];
/* L150: */
		    }
		    x[jx] = temp;
		    jx += *incx;
/* L160: */
		}
	    }
	}
    }
    return 0;
/*     End of STRMV . */
} /* strmv_ */
#undef a_ref




/* Subroutine */ int xerbla_(const char *srname, integer *info)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    XERBLA  is an error handler for the LAPACK routines.   
    It is called by an LAPACK routine if an input parameter has an   
    invalid value.  A message is printed and execution stops.   

    Installers may consider modifying the STOP statement in order to   
    call system-specific exception-handling facilities.   

    Arguments   
    =========   

    SRNAME  (input) CHARACTER*6   
            The name of the routine which called XERBLA.   

    INFO    (input) INTEGER   
            The position of the invalid parameter in the parameter list   

            of the calling routine.   

   ===================================================================== 
*/

    printf("** On entry to %6s, parameter number %2i had an illegal value\n",
		srname, *info);

/*     End of XERBLA */

    return 0;
} /* xerbla_ */


/* Subroutine */ int sstedc_(const char *compz, integer *n, real *d__, real *e, 
	real *z__, integer *ldz, real *work, integer *lwork, integer *iwork, 
	integer *liwork, integer *info)
{
/*  -- LAPACK driver routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SSTEDC computes all eigenvalues and, optionally, eigenvectors of a   
    symmetric tridiagonal matrix using the divide and conquer method.   
    The eigenvectors of a full or band real symmetric matrix can also be   
    found if SSYTRD or SSPTRD or SSBTRD has been used to reduce this   
    matrix to tridiagonal form.   

    This code makes very mild assumptions about floating point   
    arithmetic. It will work on machines with a guard digit in   
    add/subtract, or on those binary machines without guard digits   
    which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2.   
    It could conceivably fail on hexadecimal or decimal machines   
    without guard digits, but we know of none.  See SLAED3 for details.   

    Arguments   
    =========   

    COMPZ   (input) CHARACTER*1   
            = 'N':  Compute eigenvalues only.   
            = 'I':  Compute eigenvectors of tridiagonal matrix also.   
            = 'V':  Compute eigenvectors of original dense symmetric   
                    matrix also.  On entry, Z contains the orthogonal   
                    matrix used to reduce the original matrix to   
                    tridiagonal form.   

    N       (input) INTEGER   
            The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the diagonal elements of the tridiagonal matrix.   
            On exit, if INFO = 0, the eigenvalues in ascending order.   

    E       (input/output) REAL array, dimension (N-1)   
            On entry, the subdiagonal elements of the tridiagonal matrix.   
            On exit, E has been destroyed.   

    Z       (input/output) REAL array, dimension (LDZ,N)   
            On entry, if COMPZ = 'V', then Z contains the orthogonal   
            matrix used in the reduction to tridiagonal form.   
            On exit, if INFO = 0, then if COMPZ = 'V', Z contains the   
            orthonormal eigenvectors of the original symmetric matrix,   
            and if COMPZ = 'I', Z contains the orthonormal eigenvectors   
            of the symmetric tridiagonal matrix.   
            If  COMPZ = 'N', then Z is not referenced.   

    LDZ     (input) INTEGER   
            The leading dimension of the array Z.  LDZ >= 1.   
            If eigenvectors are desired, then LDZ >= max(1,N).   

    WORK    (workspace/output) REAL array,   
                                           dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            If COMPZ = 'N' or N <= 1 then LWORK must be at least 1.   
            If COMPZ = 'V' and N > 1 then LWORK must be at least   
                           ( 1 + 3*N + 2*N*lg N + 3*N**2 ),   
                           where lg( N ) = smallest integer k such   
                           that 2**k >= N.   
            If COMPZ = 'I' and N > 1 then LWORK must be at least   
                           ( 1 + 4*N + N**2 ).   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    IWORK   (workspace/output) INTEGER array, dimension (LIWORK)   
            On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK.   

    LIWORK  (input) INTEGER   
            The dimension of the array IWORK.   
            If COMPZ = 'N' or N <= 1 then LIWORK must be at least 1.   
            If COMPZ = 'V' and N > 1 then LIWORK must be at least   
                           ( 6 + 6*N + 5*N*lg N ).   
            If COMPZ = 'I' and N > 1 then LIWORK must be at least   
                           ( 3 + 5*N ).   

            If LIWORK = -1, then a workspace query is assumed; the   
            routine only calculates the optimal size of the IWORK array,   
            returns this value as the first entry of the IWORK array, and   
            no error message related to LIWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  The algorithm failed to compute an eigenvalue while   
                  working on the submatrix lying in rows and columns   
                  INFO/(N+1) through mod(INFO,N+1).   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   
    Modified by Francoise Tisseur, University of Tennessee.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__2 = 2;
    static integer c__9 = 9;
    static integer c__0 = 0;
    static real c_b18 = 0.f;
    static real c_b19 = 1.f;
    static integer c__1 = 1;
    
    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    real r__1, r__2;
    /* Builtin functions */
    //double log(doublereal);
    integer pow_ii(integer *, integer *);
    //double sqrt(doublereal);
    /* Local variables */
    static real tiny;
    static integer i__, j, k, m;
    static real p;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    static integer lwmin, start;
    extern /* Subroutine */ int sswap_(integer *, real *, integer *, real *, 
	    integer *), slaed0_(integer *, integer *, integer *, real *, real 
	    *, real *, integer *, real *, integer *, real *, integer *, 
	    integer *);
    static integer ii;
    extern doublereal slamch_(const char *);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slascl_(const char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *), slacpy_(const char *, integer *, integer *, real *, integer *, 
	    real *, integer *), slaset_(const char *, integer *, integer *, 
	    real *, real *, real *, integer *);
    static integer liwmin, icompz;
    static real orgnrm;
    extern doublereal slanst_(const char *, integer *, real *, real *);
    extern /* Subroutine */ int ssterf_(integer *, real *, real *, integer *),
	     slasrt_(const char *, integer *, real *, integer *);
    static logical lquery;
    static integer smlsiz;
    extern /* Subroutine */ int ssteqr_(const char *, integer *, real *, real *, 
	    real *, integer *, real *, integer *);
    static integer storez, strtrw, end, lgn;
    static real eps;
#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]


    --d__;
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1 || *liwork == -1;

    if (lsame_(compz, "N")) {
	icompz = 0;
    } else if (lsame_(compz, "V")) {
	icompz = 1;
    } else if (lsame_(compz, "I")) {
	icompz = 2;
    } else {
	icompz = -1;
    }
    if (*n <= 1 || icompz <= 0) {
	liwmin = 1;
	lwmin = 1;
    } else {
	lgn = (integer) (log((real) (*n)) / log(2.f));
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (icompz == 1) {
/* Computing 2nd power */
	    i__1 = *n;
	    lwmin = *n * 3 + 1 + (*n << 1) * lgn + i__1 * i__1 * 3;
	    liwmin = *n * 6 + 6 + *n * 5 * lgn;
	} else if (icompz == 2) {
/* Computing 2nd power */
	    i__1 = *n;
	    lwmin = (*n << 2) + 1 + i__1 * i__1;
	    liwmin = *n * 5 + 3;
	}
    }
    if (icompz < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || icompz > 0 && *ldz < f2cmax(1,*n)) {
	*info = -6;
    } else if (*lwork < lwmin && ! lquery) {
	*info = -8;
    } else if (*liwork < liwmin && ! lquery) {
	*info = -10;
    }

    if (*info == 0) {
	work[1] = (real) lwmin;
	iwork[1] = liwmin;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSTEDC", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*n == 1) {
	if (icompz != 0) {
	    z___ref(1, 1) = 1.f;
	}
	return 0;
    }

    smlsiz = ilaenv_(&c__9, "SSTEDC", " ", &c__0, &c__0, &c__0, &c__0, (
	    ftnlen)6, (ftnlen)1);

/*     If the following conditional clause is removed, then the routine   
       will use the Divide and Conquer routine to compute only the   
       eigenvalues, which requires (3N + 3N**2) real workspace and   
       (2 + 5N + 2N lg(N)) integer workspace.   
       Since on many architectures SSTERF is much faster than any other   
       algorithm for finding eigenvalues only, it is used here   
       as the default.   

       If COMPZ = 'N', use SSTERF to compute the eigenvalues. */

    if (icompz == 0) {
	ssterf_(n, &d__[1], &e[1], info);
	return 0;
    }

/*     If N is smaller than the minimum divide size (SMLSIZ+1), then   
       solve the problem with another solver. */

    if (*n <= smlsiz) {
	if (icompz == 0) {
	    ssterf_(n, &d__[1], &e[1], info);
	    return 0;
	} else if (icompz == 2) {
	    ssteqr_("I", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1], 
		    info);
	    return 0;
	} else {
	    ssteqr_("V", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1], 
		    info);
	    return 0;
	}
    }

/*     If COMPZ = 'V', the Z matrix must be stored elsewhere for later   
       use. */

    if (icompz == 1) {
	storez = *n * *n + 1;
    } else {
	storez = 1;
    }

    if (icompz == 2) {
	slaset_("Full", n, n, &c_b18, &c_b19, &z__[z_offset], ldz);
    }

/*     Scale. */

    orgnrm = slanst_("M", n, &d__[1], &e[1]);
    if (orgnrm == 0.f) {
	return 0;
    }

    eps = slamch_("Epsilon");

    start = 1;

/*     while ( START <= N ) */

L10:
    if (start <= *n) {

/*     Let END be the position of the next subdiagonal entry such that   
       E( END ) <= TINY or END = N if no such subdiagonal exists.  The   
       matrix identified by the elements between START and END   
       constitutes an independent sub-problem. */

	end = start;
L20:
	if (end < *n) {
	    tiny = eps * sqrt((r__1 = d__[end], dabs(r__1))) * sqrt((r__2 = 
		    d__[end + 1], dabs(r__2)));
	    if ((r__1 = e[end], dabs(r__1)) > tiny) {
		++end;
		goto L20;
	    }
	}

/*        (Sub) Problem determined.  Compute its size and solve it. */

	m = end - start + 1;
	if (m == 1) {
	    start = end + 1;
	    goto L10;
	}
	if (m > smlsiz) {
	    *info = smlsiz;

/*           Scale. */

	    orgnrm = slanst_("M", &m, &d__[start], &e[start]);
	    slascl_("G", &c__0, &c__0, &orgnrm, &c_b19, &m, &c__1, &d__[start]
		    , &m, info);
	    i__1 = m - 1;
	    i__2 = m - 1;
	    slascl_("G", &c__0, &c__0, &orgnrm, &c_b19, &i__1, &c__1, &e[
		    start], &i__2, info);

	    if (icompz == 1) {
		strtrw = 1;
	    } else {
		strtrw = start;
	    }
	    slaed0_(&icompz, n, &m, &d__[start], &e[start], &z___ref(strtrw, 
		    start), ldz, &work[1], n, &work[storez], &iwork[1], info);
	    if (*info != 0) {
		*info = (*info / (m + 1) + start - 1) * (*n + 1) + *info % (m 
			+ 1) + start - 1;
		return 0;
	    }

/*           Scale back. */

	    slascl_("G", &c__0, &c__0, &c_b19, &orgnrm, &m, &c__1, &d__[start]
		    , &m, info);

	} else {
	    if (icompz == 1) {

/*     Since QR won't update a Z matrix which is larger than the   
       length of D, we must solve the sub-problem in a workspace and   
       then multiply back into Z. */

		ssteqr_("I", &m, &d__[start], &e[start], &work[1], &m, &work[
			m * m + 1], info);
		slacpy_("A", n, &m, &z___ref(1, start), ldz, &work[storez], n);
		sgemm_("N", "N", n, &m, &m, &c_b19, &work[storez], ldz, &work[
			1], &m, &c_b18, &z___ref(1, start), ldz);
	    } else if (icompz == 2) {
		ssteqr_("I", &m, &d__[start], &e[start], &z___ref(start, 
			start), ldz, &work[1], info);
	    } else {
		ssterf_(&m, &d__[start], &e[start], info);
	    }
	    if (*info != 0) {
		*info = start * (*n + 1) + end;
		return 0;
	    }
	}

	start = end + 1;
	goto L10;
    }

/*     endwhile   

       If the problem split any number of times, then the eigenvalues   
       will not be properly ordered.  Here we permute the eigenvalues   
       (and the associated eigenvectors) into ascending order. */

    if (m != *n) {
	if (icompz == 0) {

/*        Use Quick Sort */

	    slasrt_("I", n, &d__[1], info);

	} else {

/*        Use Selection Sort to minimize swaps of eigenvectors */

	    i__1 = *n;
	    for (ii = 2; ii <= i__1; ++ii) {
		i__ = ii - 1;
		k = i__;
		p = d__[i__];
		i__2 = *n;
		for (j = ii; j <= i__2; ++j) {
		    if (d__[j] < p) {
			k = j;
			p = d__[j];
		    }
/* L30: */
		}
		if (k != i__) {
		    d__[k] = d__[i__];
		    d__[i__] = p;
		    sswap_(n, &z___ref(1, i__), &c__1, &z___ref(1, k), &c__1);
		}
/* L40: */
	    }
	}
    }

    work[1] = (real) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of SSTEDC */

} /* sstedc_ */

#undef z___ref


/* Subroutine */ int sstevd_(char *jobz, integer *n, real *d__, real *e, real 
	*z__, integer *ldz, real *work, integer *lwork, integer *iwork, 
	integer *liwork, integer *info)
{
/*  -- LAPACK driver routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SSTEVD computes all eigenvalues and, optionally, eigenvectors of a   
    real symmetric tridiagonal matrix. If eigenvectors are desired, it   
    uses a divide and conquer algorithm.   

    The divide and conquer algorithm makes very mild assumptions about   
    floating point arithmetic. It will work on machines with a guard   
    digit in add/subtract, or on those binary machines without guard   
    digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or   
    Cray-2. It could conceivably fail on hexadecimal or decimal machines   
    without guard digits, but we know of none.   

    Arguments   
    =========   

    JOBZ    (input) CHARACTER*1   
            = 'N':  Compute eigenvalues only;   
            = 'V':  Compute eigenvalues and eigenvectors.   

    N       (input) INTEGER   
            The order of the matrix.  N >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the n diagonal elements of the tridiagonal matrix   
            A.   
            On exit, if INFO = 0, the eigenvalues in ascending order.   

    E       (input/output) REAL array, dimension (N)   
            On entry, the (n-1) subdiagonal elements of the tridiagonal   
            matrix A, stored in elements 1 to N-1 of E; E(N) need not   
            be set, but is used by the routine.   
            On exit, the contents of E are destroyed.   

    Z       (output) REAL array, dimension (LDZ, N)   
            If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal   
            eigenvectors of the matrix A, with the i-th column of Z   
            holding the eigenvector associated with D(i).   
            If JOBZ = 'N', then Z is not referenced.   

    LDZ     (input) INTEGER   
            The leading dimension of the array Z.  LDZ >= 1, and if   
            JOBZ = 'V', LDZ >= max(1,N).   

    WORK    (workspace/output) REAL array,   
                                           dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            If JOBZ  = 'N' or N <= 1 then LWORK must be at least 1.   
            If JOBZ  = 'V' and N > 1 then LWORK must be at least   
                           ( 1 + 4*N + N**2 ).   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    IWORK   (workspace/output) INTEGER array, dimension (LIWORK)   
            On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK.   

    LIWORK  (input) INTEGER   
            The dimension of the array IWORK.   
            If JOBZ  = 'N' or N <= 1 then LIWORK must be at least 1.   
            If JOBZ  = 'V' and N > 1 then LIWORK must be at least 3+5*N.   

            If LIWORK = -1, then a workspace query is assumed; the   
            routine only calculates the optimal size of the IWORK array,   
            returns this value as the first entry of the IWORK array, and   
            no error message related to LIWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  if INFO = i, the algorithm failed to converge; i   
                  off-diagonal elements of E did not converge to zero.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    real r__1;
    /* Builtin functions */
    // double sqrt(doublereal);
    /* Local variables */
    static real rmin, rmax, tnrm, sigma;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static integer lwmin;
    static logical wantz;
    static integer iscale;
    extern doublereal slamch_(const char *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static real bignum;
    extern /* Subroutine */ int sstedc_(const char *, integer *, real *, real *, 
	    real *, integer *, real *, integer *, integer *, integer *, 
	    integer *);
    static integer liwmin;
    extern doublereal slanst_(const char *, integer *, real *, real *);
    extern /* Subroutine */ int ssterf_(integer *, real *, real *, integer *);
    static real smlnum;
    static logical lquery;
    static real eps;
#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]


    --d__;
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    liwmin = 1;
    lwmin = 1;
    if (*n > 1 && wantz) {
/* Computing 2nd power */
	i__1 = *n;
	lwmin = (*n << 2) + 1 + i__1 * i__1;
	liwmin = *n * 5 + 3;
    }

    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -6;
    } else if (*lwork < lwmin && ! lquery) {
	*info = -8;
    } else if (*liwork < liwmin && ! lquery) {
	*info = -10;
    }

    if (*info == 0) {
	work[1] = (real) lwmin;
	iwork[1] = liwmin;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSTEVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (wantz) {
	    z___ref(1, 1) = 1.f;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = slamch_("Safe minimum");
    eps = slamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1.f / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    tnrm = slanst_("M", n, &d__[1], &e[1]);
    if (tnrm > 0.f && tnrm < rmin) {
	iscale = 1;
	sigma = rmin / tnrm;
    } else if (tnrm > rmax) {
	iscale = 1;
	sigma = rmax / tnrm;
    }
    if (iscale == 1) {
	sscal_(n, &sigma, &d__[1], &c__1);
	i__1 = *n - 1;
	sscal_(&i__1, &sigma, &e[1], &c__1);
    }

/*     For eigenvalues only, call SSTERF.  For eigenvalues and   
       eigenvectors, call SSTEDC. */

    if (! wantz) {
	ssterf_(n, &d__[1], &e[1], info);
    } else {
	sstedc_("I", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1], lwork, 
		&iwork[1], liwork, info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	r__1 = 1.f / sigma;
	sscal_(n, &r__1, &d__[1], &c__1);
    }

    work[1] = (real) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of SSTEVD */

} /* sstevd_ */

#undef z___ref


/* Subroutine */ int slaed0_(integer *icompq, integer *qsiz, integer *n, real 
	*d__, real *e, real *q, integer *ldq, real *qstore, integer *ldqs, 
	real *work, integer *iwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SLAED0 computes all eigenvalues and corresponding eigenvectors of a   
    symmetric tridiagonal matrix using the divide and conquer method.   

    Arguments   
    =========   

    ICOMPQ  (input) INTEGER   
            = 0:  Compute eigenvalues only.   
            = 1:  Compute eigenvectors of original dense symmetric matrix   
                  also.  On entry, Q contains the orthogonal matrix used   
                  to reduce the original matrix to tridiagonal form.   
            = 2:  Compute eigenvalues and eigenvectors of tridiagonal   
                  matrix.   

    QSIZ   (input) INTEGER   
           The dimension of the orthogonal matrix used to reduce   
           the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1.   

    N      (input) INTEGER   
           The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    D      (input/output) REAL array, dimension (N)   
           On entry, the main diagonal of the tridiagonal matrix.   
           On exit, its eigenvalues.   

    E      (input) REAL array, dimension (N-1)   
           The off-diagonal elements of the tridiagonal matrix.   
           On exit, E has been destroyed.   

    Q      (input/output) REAL array, dimension (LDQ, N)   
           On entry, Q must contain an N-by-N orthogonal matrix.   
           If ICOMPQ = 0    Q is not referenced.   
           If ICOMPQ = 1    On entry, Q is a subset of the columns of the   
                            orthogonal matrix used to reduce the full   
                            matrix to tridiagonal form corresponding to   
                            the subset of the full matrix which is being   
                            decomposed at this time.   
           If ICOMPQ = 2    On entry, Q will be the identity matrix.   
                            On exit, Q contains the eigenvectors of the   
                            tridiagonal matrix.   

    LDQ    (input) INTEGER   
           The leading dimension of the array Q.  If eigenvectors are   
           desired, then  LDQ >= max(1,N).  In any case,  LDQ >= 1.   

    QSTORE (workspace) REAL array, dimension (LDQS, N)   
           Referenced only when ICOMPQ = 1.  Used to store parts of   
           the eigenvector matrix when the updating matrix multiplies   
           take place.   

    LDQS   (input) INTEGER   
           The leading dimension of the array QSTORE.  If ICOMPQ = 1,   
           then  LDQS >= max(1,N).  In any case,  LDQS >= 1.   

    WORK   (workspace) REAL array,   
           If ICOMPQ = 0 or 1, the dimension of WORK must be at least   
                       1 + 3*N + 2*N*lg N + 2*N**2   
                       ( lg( N ) = smallest integer k   
                                   such that 2^k >= N )   
           If ICOMPQ = 2, the dimension of WORK must be at least   
                       4*N + N**2.   

    IWORK  (workspace) INTEGER array,   
           If ICOMPQ = 0 or 1, the dimension of IWORK must be at least   
                          6 + 6*N + 5*N*lg N.   
                          ( lg( N ) = smallest integer k   
                                      such that 2^k >= N )   
           If ICOMPQ = 2, the dimension of IWORK must be at least   
                          3 + 5*N.   

    INFO   (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  The algorithm failed to compute an eigenvalue while   
                  working on the submatrix lying in rows and columns   
                  INFO/(N+1) through mod(INFO,N+1).   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__9 = 9;
    static integer c__0 = 0;
    static integer c__2 = 2;
    static real c_b23 = 1.f;
    static real c_b24 = 0.f;
    static integer c__1 = 1;
    
    /* System generated locals */
    integer q_dim1, q_offset, qstore_dim1, qstore_offset, i__1, i__2;
    real r__1;
    /* Builtin functions */
    // double log(doublereal);
    integer pow_ii(integer *, integer *);
    /* Local variables */
    static real temp;
    static integer curr, i__, j, k;
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    static integer iperm, indxq, iwrem;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    static integer iqptr, tlvls;
    extern /* Subroutine */ int slaed1_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, real *, integer *, integer *), 
	    slaed7_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, integer *, real *, integer *
	    , real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *);
    static integer iq, igivcl;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    static integer igivnm, submat;
    extern /* Subroutine */ int slacpy_(const char *, integer *, integer *, real *, 
	    integer *, real *, integer *);
    static integer curprb, subpbs, igivpt, curlvl, matsiz, iprmpt, smlsiz;
    extern /* Subroutine */ int ssteqr_(const char *, integer *, real *, real *, 
	    real *, integer *, real *, integer *);
    static integer lgn, msd2, smm1, spm1, spm2;
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]
#define qstore_ref(a_1,a_2) qstore[(a_2)*qstore_dim1 + a_1]


    --d__;
    --e;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    qstore_dim1 = *ldqs;
    qstore_offset = 1 + qstore_dim1 * 1;
    qstore -= qstore_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 2) {
	*info = -1;
    } else if (*icompq == 1 && *qsiz < f2cmax(0,*n)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldq < f2cmax(1,*n)) {
	*info = -7;
    } else if (*ldqs < f2cmax(1,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED0", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    smlsiz = ilaenv_(&c__9, "SLAED0", " ", &c__0, &c__0, &c__0, &c__0, (
	    ftnlen)6, (ftnlen)1);

/*     Determine the size and placement of the submatrices, and save in   
       the leading elements of IWORK. */

    iwork[1] = *n;
    subpbs = 1;
    tlvls = 0;
L10:
    if (iwork[subpbs] > smlsiz) {
	for (j = subpbs; j >= 1; --j) {
	    iwork[j * 2] = (iwork[j] + 1) / 2;
	    iwork[(j << 1) - 1] = iwork[j] / 2;
/* L20: */
	}
	++tlvls;
	subpbs <<= 1;
	goto L10;
    }
    i__1 = subpbs;
    for (j = 2; j <= i__1; ++j) {
	iwork[j] += iwork[j - 1];
/* L30: */
    }

/*     Divide the matrix into SUBPBS submatrices of size at most SMLSIZ+1   
       using rank-1 modifications (cuts). */

    spm1 = subpbs - 1;
    i__1 = spm1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	submat = iwork[i__] + 1;
	smm1 = submat - 1;
	d__[smm1] -= (r__1 = e[smm1], dabs(r__1));
	d__[submat] -= (r__1 = e[smm1], dabs(r__1));
/* L40: */
    }

    indxq = (*n << 2) + 3;
    if (*icompq != 2) {

/*        Set up workspaces for eigenvalues only/accumulate new vectors   
          routine */

	temp = log((real) (*n)) / log(2.f);
	lgn = (integer) temp;
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	iprmpt = indxq + *n + 1;
	iperm = iprmpt + *n * lgn;
	iqptr = iperm + *n * lgn;
	igivpt = iqptr + *n + 2;
	igivcl = igivpt + *n * lgn;

	igivnm = 1;
	iq = igivnm + (*n << 1) * lgn;
/* Computing 2nd power */
	i__1 = *n;
	iwrem = iq + i__1 * i__1 + 1;

/*        Initialize pointers */

	i__1 = subpbs;
	for (i__ = 0; i__ <= i__1; ++i__) {
	    iwork[iprmpt + i__] = 1;
	    iwork[igivpt + i__] = 1;
/* L50: */
	}
	iwork[iqptr] = 1;
    }

/*     Solve each submatrix eigenproblem at the bottom of the divide and   
       conquer tree. */

    curr = 0;
    i__1 = spm1;
    for (i__ = 0; i__ <= i__1; ++i__) {
	if (i__ == 0) {
	    submat = 1;
	    matsiz = iwork[1];
	} else {
	    submat = iwork[i__] + 1;
	    matsiz = iwork[i__ + 1] - iwork[i__];
	}
	if (*icompq == 2) {
	    ssteqr_("I", &matsiz, &d__[submat], &e[submat], &q_ref(submat, 
		    submat), ldq, &work[1], info);
	    if (*info != 0) {
		goto L130;
	    }
	} else {
	    ssteqr_("I", &matsiz, &d__[submat], &e[submat], &work[iq - 1 + 
		    iwork[iqptr + curr]], &matsiz, &work[1], info);
	    if (*info != 0) {
		goto L130;
	    }
	    if (*icompq == 1) {
		sgemm_("N", "N", qsiz, &matsiz, &matsiz, &c_b23, &q_ref(1, 
			submat), ldq, &work[iq - 1 + iwork[iqptr + curr]], &
			matsiz, &c_b24, &qstore_ref(1, submat), ldqs);
	    }
/* Computing 2nd power */
	    i__2 = matsiz;
	    iwork[iqptr + curr + 1] = iwork[iqptr + curr] + i__2 * i__2;
	    ++curr;
	}
	k = 1;
	i__2 = iwork[i__ + 1];
	for (j = submat; j <= i__2; ++j) {
	    iwork[indxq + j] = k;
	    ++k;
/* L60: */
	}
/* L70: */
    }

/*     Successively merge eigensystems of adjacent submatrices   
       into eigensystem for the corresponding larger matrix.   

       while ( SUBPBS > 1 ) */

    curlvl = 1;
L80:
    if (subpbs > 1) {
	spm2 = subpbs - 2;
	i__1 = spm2;
	for (i__ = 0; i__ <= i__1; i__ += 2) {
	    if (i__ == 0) {
		submat = 1;
		matsiz = iwork[2];
		msd2 = iwork[1];
		curprb = 0;
	    } else {
		submat = iwork[i__] + 1;
		matsiz = iwork[i__ + 2] - iwork[i__];
		msd2 = matsiz / 2;
		++curprb;
	    }

/*     Merge lower order eigensystems (of size MSD2 and MATSIZ - MSD2)   
       into an eigensystem of size MATSIZ.   
       SLAED1 is used only for the full eigensystem of a tridiagonal   
       matrix.   
       SLAED7 handles the cases in which eigenvalues only or eigenvalues   
       and eigenvectors of a full symmetric matrix (which was reduced to   
       tridiagonal form) are desired. */

	    if (*icompq == 2) {
		slaed1_(&matsiz, &d__[submat], &q_ref(submat, submat), ldq, &
			iwork[indxq + submat], &e[submat + msd2 - 1], &msd2, &
			work[1], &iwork[subpbs + 1], info);
	    } else {
		slaed7_(icompq, &matsiz, qsiz, &tlvls, &curlvl, &curprb, &d__[
			submat], &qstore_ref(1, submat), ldqs, &iwork[indxq + 
			submat], &e[submat + msd2 - 1], &msd2, &work[iq], &
			iwork[iqptr], &iwork[iprmpt], &iwork[iperm], &iwork[
			igivpt], &iwork[igivcl], &work[igivnm], &work[iwrem], 
			&iwork[subpbs + 1], info);
	    }
	    if (*info != 0) {
		goto L130;
	    }
	    iwork[i__ / 2 + 1] = iwork[i__ + 2];
/* L90: */
	}
	subpbs /= 2;
	++curlvl;
	goto L80;
    }

/*     end while   

       Re-merge the eigenvalues/vectors which were deflated at the final   
       merge step. */

    if (*icompq == 1) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
	    scopy_(qsiz, &qstore_ref(1, j), &c__1, &q_ref(1, i__), &c__1);
/* L100: */
	}
	scopy_(n, &work[1], &c__1, &d__[1], &c__1);
    } else if (*icompq == 2) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
	    scopy_(n, &q_ref(1, j), &c__1, &work[*n * i__ + 1], &c__1);
/* L110: */
	}
	scopy_(n, &work[1], &c__1, &d__[1], &c__1);
	slacpy_("A", n, n, &work[*n + 1], n, &q[q_offset], ldq);
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
/* L120: */
	}
	scopy_(n, &work[1], &c__1, &d__[1], &c__1);
    }
    goto L140;

L130:
    *info = submat * (*n + 1) + submat + matsiz - 1;

L140:
    return 0;

/*     End of SLAED0 */

} /* slaed0_ */

#undef qstore_ref
#undef q_ref



/* Subroutine */ int slaed7_(integer *icompq, integer *n, integer *qsiz, 
	integer *tlvls, integer *curlvl, integer *curpbm, real *d__, real *q, 
	integer *ldq, integer *indxq, real *rho, integer *cutpnt, real *
	qstore, integer *qptr, integer *prmptr, integer *perm, integer *
	givptr, integer *givcol, real *givnum, real *work, integer *iwork, 
	integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLAED7 computes the updated eigensystem of a diagonal   
    matrix after modification by a rank-one symmetric matrix. This   
    routine is used only for the eigenproblem which requires all   
    eigenvalues and optionally eigenvectors of a dense symmetric matrix   
    that has been reduced to tridiagonal form.  SLAED1 handles   
    the case in which all eigenvalues and eigenvectors of a symmetric   
    tridiagonal matrix are desired.   

      T = Q(in) ( D(in) + RHO * Z*Z' ) Q'(in) = Q(out) * D(out) * Q'(out)   

       where Z = Q'u, u is a vector of length N with ones in the   
       CUTPNT and CUTPNT + 1 th elements and zeros elsewhere.   

       The eigenvectors of the original matrix are stored in Q, and the   
       eigenvalues are in D.  The algorithm consists of three stages:   

          The first stage consists of deflating the size of the problem   
          when there are multiple eigenvalues or if there is a zero in   
          the Z vector.  For each such occurence the dimension of the   
          secular equation problem is reduced by one.  This stage is   
          performed by the routine SLAED8.   

          The second stage consists of calculating the updated   
          eigenvalues. This is done by finding the roots of the secular   
          equation via the routine SLAED4 (as called by SLAED9).   
          This routine also calculates the eigenvectors of the current   
          problem.   

          The final stage consists of computing the updated eigenvectors   
          directly using the updated eigenvalues.  The eigenvectors for   
          the current problem are multiplied with the eigenvectors from   
          the overall problem.   

    Arguments   
    =========   

    ICOMPQ  (input) INTEGER   
            = 0:  Compute eigenvalues only.   
            = 1:  Compute eigenvectors of original dense symmetric matrix   
                  also.  On entry, Q contains the orthogonal matrix used   
                  to reduce the original matrix to tridiagonal form.   

    N      (input) INTEGER   
           The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    QSIZ   (input) INTEGER   
           The dimension of the orthogonal matrix used to reduce   
           the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1.   

    TLVLS  (input) INTEGER   
           The total number of merging levels in the overall divide and   
           conquer tree.   

    CURLVL (input) INTEGER   
           The current level in the overall merge routine,   
           0 <= CURLVL <= TLVLS.   

    CURPBM (input) INTEGER   
           The current problem in the current level in the overall   
           merge routine (counting from upper left to lower right).   

    D      (input/output) REAL array, dimension (N)   
           On entry, the eigenvalues of the rank-1-perturbed matrix.   
           On exit, the eigenvalues of the repaired matrix.   

    Q      (input/output) REAL array, dimension (LDQ, N)   
           On entry, the eigenvectors of the rank-1-perturbed matrix.   
           On exit, the eigenvectors of the repaired tridiagonal matrix.   

    LDQ    (input) INTEGER   
           The leading dimension of the array Q.  LDQ >= max(1,N).   

    INDXQ  (output) INTEGER array, dimension (N)   
           The permutation which will reintegrate the subproblem just   
           solved back into sorted order, i.e., D( INDXQ( I = 1, N ) )   
           will be in ascending order.   

    RHO    (input) REAL   
           The subdiagonal element used to create the rank-1   
           modification.   

    CUTPNT (input) INTEGER   
           Contains the location of the last eigenvalue in the leading   
           sub-matrix.  min(1,N) <= CUTPNT <= N.   

    QSTORE (input/output) REAL array, dimension (N**2+1)   
           Stores eigenvectors of submatrices encountered during   
           divide and conquer, packed together. QPTR points to   
           beginning of the submatrices.   

    QPTR   (input/output) INTEGER array, dimension (N+2)   
           List of indices pointing to beginning of submatrices stored   
           in QSTORE. The submatrices are numbered starting at the   
           bottom left of the divide and conquer tree, from left to   
           right and bottom to top.   

    PRMPTR (input) INTEGER array, dimension (N lg N)   
           Contains a list of pointers which indicate where in PERM a   
           level's permutation is stored.  PRMPTR(i+1) - PRMPTR(i)   
           indicates the size of the permutation and also the size of   
           the full, non-deflated problem.   

    PERM   (input) INTEGER array, dimension (N lg N)   
           Contains the permutations (from deflation and sorting) to be   
           applied to each eigenblock.   

    GIVPTR (input) INTEGER array, dimension (N lg N)   
           Contains a list of pointers which indicate where in GIVCOL a   
           level's Givens rotations are stored.  GIVPTR(i+1) - GIVPTR(i)   
           indicates the number of Givens rotations.   

    GIVCOL (input) INTEGER array, dimension (2, N lg N)   
           Each pair of numbers indicates a pair of columns to take place   
           in a Givens rotation.   

    GIVNUM (input) REAL array, dimension (2, N lg N)   
           Each number indicates the S value to be used in the   
           corresponding Givens rotation.   

    WORK   (workspace) REAL array, dimension (3*N+QSIZ*N)   

    IWORK  (workspace) INTEGER array, dimension (4*N)   

    INFO   (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  if INFO = 1, an eigenvalue did not converge   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__2 = 2;
    static integer c__1 = 1;
    static real c_b10 = 1.f;
    static real c_b11 = 0.f;
    static integer c_n1 = -1;
    
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    /* Local variables */
    static integer indx, curr, i__, k, indxc;
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    static integer indxp, n1, n2;
    extern /* Subroutine */ int slaed8_(integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, integer *, real *, integer *
	    , real *, real *, real *, integer *, real *, integer *, integer *,
	     integer *, real *, integer *, integer *, integer *), slaed9_(
	    integer *, integer *, integer *, integer *, real *, real *, 
	    integer *, real *, real *, real *, real *, integer *, integer *), 
	    slaeda_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, real *
	    , real *, integer *);
    static integer idlmda, is, iw, iz;
    extern /* Subroutine */ int xerbla_(const char *, integer *), slamrg_(
	    integer *, integer *, real *, integer *, integer *, integer *);
    static integer coltyp, iq2, ptr, ldq2;
#define givcol_ref(a_1,a_2) givcol[(a_2)*2 + a_1]
#define givnum_ref(a_1,a_2) givnum[(a_2)*2 + a_1]


    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --qstore;
    --qptr;
    --prmptr;
    --perm;
    --givptr;
    givcol -= 3;
    givnum -= 3;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*icompq == 1 && *qsiz < *n) {
	*info = -4;
    } else if (*ldq < f2cmax(1,*n)) {
	*info = -9;
    } else if (f2cmin(1,*n) > *cutpnt || *n < *cutpnt) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED7", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     The following values are for bookkeeping purposes only.  They are   
       integer pointers which indicate the portion of the workspace   
       used by a particular array in SLAED8 and SLAED9. */

    if (*icompq == 1) {
	ldq2 = *qsiz;
    } else {
	ldq2 = *n;
    }

    iz = 1;
    idlmda = iz + *n;
    iw = idlmda + *n;
    iq2 = iw + *n;
    is = iq2 + *n * ldq2;

    indx = 1;
    indxc = indx + *n;
    coltyp = indxc + *n;
    indxp = coltyp + *n;

/*     Form the z-vector which consists of the last row of Q_1 and the   
       first row of Q_2. */

    ptr = pow_ii(&c__2, tlvls) + 1;
    i__1 = *curlvl - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *tlvls - i__;
	ptr += pow_ii(&c__2, &i__2);
/* L10: */
    }
    curr = ptr + *curpbm;
    slaeda_(n, tlvls, curlvl, curpbm, &prmptr[1], &perm[1], &givptr[1], &
	    givcol[3], &givnum[3], &qstore[1], &qptr[1], &work[iz], &work[iz 
	    + *n], info);

/*     When solving the final problem, we no longer need the stored data,   
       so we will overwrite the data from this level onto the previously   
       used storage space. */

    if (*curlvl == *tlvls) {
	qptr[curr] = 1;
	prmptr[curr] = 1;
	givptr[curr] = 1;
    }

/*     Sort and Deflate eigenvalues. */

    slaed8_(icompq, &k, n, qsiz, &d__[1], &q[q_offset], ldq, &indxq[1], rho, 
	    cutpnt, &work[iz], &work[idlmda], &work[iq2], &ldq2, &work[iw], &
	    perm[prmptr[curr]], &givptr[curr + 1], &givcol_ref(1, givptr[curr]
	    ), &givnum_ref(1, givptr[curr]), &iwork[indxp], &iwork[indx], 
	    info);
    prmptr[curr + 1] = prmptr[curr] + *n;
    givptr[curr + 1] += givptr[curr];

/*     Solve Secular Equation. */

    if (k != 0) {
	slaed9_(&k, &c__1, &k, n, &d__[1], &work[is], &k, rho, &work[idlmda], 
		&work[iw], &qstore[qptr[curr]], &k, info);
	if (*info != 0) {
	    goto L30;
	}
	if (*icompq == 1) {
	    sgemm_("N", "N", qsiz, &k, &k, &c_b10, &work[iq2], &ldq2, &qstore[
		    qptr[curr]], &k, &c_b11, &q[q_offset], ldq);
	}
/* Computing 2nd power */
	i__1 = k;
	qptr[curr + 1] = qptr[curr] + i__1 * i__1;

/*     Prepare the INDXQ sorting permutation. */

	n1 = k;
	n2 = *n - k;
	slamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &indxq[1]);
    } else {
	qptr[curr + 1] = qptr[curr];
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indxq[i__] = i__;
/* L20: */
	}
    }

L30:
    return 0;

/*     End of SLAED7 */

} /* slaed7_ */

#undef givnum_ref
#undef givcol_ref


/* Subroutine */ int slaed1_(integer *n, real *d__, real *q, integer *ldq, 
	integer *indxq, real *rho, integer *cutpnt, real *work, integer *
	iwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SLAED1 computes the updated eigensystem of a diagonal   
    matrix after modification by a rank-one symmetric matrix.  This   
    routine is used only for the eigenproblem which requires all   
    eigenvalues and eigenvectors of a tridiagonal matrix.  SLAED7 handles   
    the case in which eigenvalues only or eigenvalues and eigenvectors   
    of a full symmetric matrix (which was reduced to tridiagonal form)   
    are desired.   

      T = Q(in) ( D(in) + RHO * Z*Z' ) Q'(in) = Q(out) * D(out) * Q'(out)   

       where Z = Q'u, u is a vector of length N with ones in the   
       CUTPNT and CUTPNT + 1 th elements and zeros elsewhere.   

       The eigenvectors of the original matrix are stored in Q, and the   
       eigenvalues are in D.  The algorithm consists of three stages:   

          The first stage consists of deflating the size of the problem   
          when there are multiple eigenvalues or if there is a zero in   
          the Z vector.  For each such occurence the dimension of the   
          secular equation problem is reduced by one.  This stage is   
          performed by the routine SLAED2.   

          The second stage consists of calculating the updated   
          eigenvalues. This is done by finding the roots of the secular   
          equation via the routine SLAED4 (as called by SLAED3).   
          This routine also calculates the eigenvectors of the current   
          problem.   

          The final stage consists of computing the updated eigenvectors   
          directly using the updated eigenvalues.  The eigenvectors for   
          the current problem are multiplied with the eigenvectors from   
          the overall problem.   

    Arguments   
    =========   

    N      (input) INTEGER   
           The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    D      (input/output) REAL array, dimension (N)   
           On entry, the eigenvalues of the rank-1-perturbed matrix.   
           On exit, the eigenvalues of the repaired matrix.   

    Q      (input/output) REAL array, dimension (LDQ,N)   
           On entry, the eigenvectors of the rank-1-perturbed matrix.   
           On exit, the eigenvectors of the repaired tridiagonal matrix.   

    LDQ    (input) INTEGER   
           The leading dimension of the array Q.  LDQ >= max(1,N).   

    INDXQ  (input/output) INTEGER array, dimension (N)   
           On entry, the permutation which separately sorts the two   
           subproblems in D into ascending order.   
           On exit, the permutation which will reintegrate the   
           subproblems back into sorted order,   
           i.e. D( INDXQ( I = 1, N ) ) will be in ascending order.   

    RHO    (input) REAL   
           The subdiagonal entry used to create the rank-1 modification.   

    CUTPNT (input) INTEGER   
           The location of the last eigenvalue in the leading sub-matrix.   
           min(1,N) <= CUTPNT <= N/2.   

    WORK   (workspace) REAL array, dimension (4*N + N**2)   

    IWORK  (workspace) INTEGER array, dimension (4*N)   

    INFO   (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  if INFO = 1, an eigenvalue did not converge   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   
    Modified by Francoise Tisseur, University of Tennessee.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    /* Local variables */
    static integer indx, i__, k, indxc, indxp;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    static integer n1, n2;
    extern /* Subroutine */ int slaed2_(integer *, integer *, integer *, real 
	    *, real *, integer *, integer *, real *, real *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *), 
	    slaed3_(integer *, integer *, integer *, real *, real *, integer *
	    , real *, real *, real *, integer *, integer *, real *, real *, 
	    integer *);
    static integer idlmda, is, iw, iz;
    extern /* Subroutine */ int xerbla_(const char *, integer *), slamrg_(
	    integer *, integer *, real *, integer *, integer *, integer *);
    static integer coltyp, iq2, cpp1;
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]


    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    } else if (*ldq < f2cmax(1,*n)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MIN */
	i__1 = 1, i__2 = *n / 2;
	if (f2cmin(i__1,i__2) > *cutpnt || *n / 2 < *cutpnt) {
	    *info = -7;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED1", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     The following values are integer pointers which indicate   
       the portion of the workspace   
       used by a particular array in SLAED2 and SLAED3. */

    iz = 1;
    idlmda = iz + *n;
    iw = idlmda + *n;
    iq2 = iw + *n;

    indx = 1;
    indxc = indx + *n;
    coltyp = indxc + *n;
    indxp = coltyp + *n;


/*     Form the z-vector which consists of the last row of Q_1 and the   
       first row of Q_2. */

    scopy_(cutpnt, &q_ref(*cutpnt, 1), ldq, &work[iz], &c__1);
    cpp1 = *cutpnt + 1;
    i__1 = *n - *cutpnt;
    scopy_(&i__1, &q_ref(cpp1, cpp1), ldq, &work[iz + *cutpnt], &c__1);

/*     Deflate eigenvalues. */

    slaed2_(&k, n, cutpnt, &d__[1], &q[q_offset], ldq, &indxq[1], rho, &work[
	    iz], &work[idlmda], &work[iw], &work[iq2], &iwork[indx], &iwork[
	    indxc], &iwork[indxp], &iwork[coltyp], info);

    if (*info != 0) {
	goto L20;
    }

/*     Solve Secular Equation. */

    if (k != 0) {
	is = (iwork[coltyp] + iwork[coltyp + 1]) * *cutpnt + (iwork[coltyp + 
		1] + iwork[coltyp + 2]) * (*n - *cutpnt) + iq2;
	slaed3_(&k, n, cutpnt, &d__[1], &q[q_offset], ldq, rho, &work[idlmda],
		 &work[iq2], &iwork[indxc], &iwork[coltyp], &work[iw], &work[
		is], info);
	if (*info != 0) {
	    goto L20;
	}

/*     Prepare the INDXQ sorting permutation. */

	n1 = k;
	n2 = *n - k;
	slamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &indxq[1]);
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indxq[i__] = i__;
/* L10: */
	}
    }

L20:
    return 0;

/*     End of SLAED1 */

} /* slaed1_ */

#undef q_ref


/* Subroutine */ int slacpy_(const char *uplo, integer *m, integer *n, real *a, 
	integer *lda, real *b, integer *ldb)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLACPY copies all or part of a two-dimensional matrix A to another   
    matrix B.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies the part of the matrix A to be copied to B.   
            = 'U':      Upper triangular part   
            = 'L':      Lower triangular part   
            Otherwise:  All of the matrix A   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input) REAL array, dimension (LDA,N)   
            The m by n matrix A.  If UPLO = 'U', only the upper triangle   
            or trapezoid is accessed; if UPLO = 'L', only the lower   
            triangle or trapezoid is accessed.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    B       (output) REAL array, dimension (LDB,N)   
            On exit, B = A in the locations specified by UPLO.   

    LDB     (input) INTEGER   
            The leading dimension of the array B.  LDB >= max(1,M).   

    =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]

    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;

    /* Function Body */
    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = f2cmin(j,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b_ref(i__, j) = a_ref(i__, j);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(uplo, "L")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j; i__ <= i__2; ++i__) {
		b_ref(i__, j) = a_ref(i__, j);
/* L30: */
	    }
/* L40: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b_ref(i__, j) = a_ref(i__, j);
/* L50: */
	    }
/* L60: */
	}
    }
    return 0;

/*     End of SLACPY */

} /* slacpy_ */

#undef b_ref
#undef a_ref


/* Subroutine */ int slamrg_(integer *n1, integer *n2, real *a, integer *
	strd1, integer *strd2, integer *index)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLAMRG will create a permutation list which will merge the elements   
    of A (which is composed of two independently sorted sets) into a   
    single set which is sorted in ascending order.   

    Arguments   
    =========   

    N1     (input) INTEGER   
    N2     (input) INTEGER   
           These arguements contain the respective lengths of the two   
           sorted lists to be merged.   

    A      (input) REAL array, dimension (N1+N2)   
           The first N1 elements of A contain a list of numbers which   
           are sorted in either ascending or descending order.  Likewise   
           for the final N2 elements.   

    STRD1  (input) INTEGER   
    STRD2  (input) INTEGER   
           These are the strides to be taken through the array A.   
           Allowable strides are 1 and -1.  They indicate whether a   
           subset of A is sorted in ascending (STRDx = 1) or descending   
           (STRDx = -1) order.   

    INDEX  (output) INTEGER array, dimension (N1+N2)   
           On exit this array will contain a permutation such that   
           if B( I ) = A( INDEX( I ) ) for I=1,N1+N2, then B will be   
           sorted in ascending order.   

    =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer i__1;
    /* Local variables */
    static integer i__, ind1, ind2, n1sv, n2sv;

    --index;
    --a;

    /* Function Body */
    n1sv = *n1;
    n2sv = *n2;
    if (*strd1 > 0) {
	ind1 = 1;
    } else {
	ind1 = *n1;
    }
    if (*strd2 > 0) {
	ind2 = *n1 + 1;
    } else {
	ind2 = *n1 + *n2;
    }
    i__ = 1;
/*     while ( (N1SV > 0) & (N2SV > 0) ) */
L10:
    if (n1sv > 0 && n2sv > 0) {
	if (a[ind1] <= a[ind2]) {
	    index[i__] = ind1;
	    ++i__;
	    ind1 += *strd1;
	    --n1sv;
	} else {
	    index[i__] = ind2;
	    ++i__;
	    ind2 += *strd2;
	    --n2sv;
	}
	goto L10;
    }
/*     end while */
    if (n1sv == 0) {
	i__1 = n2sv;
	for (n1sv = 1; n1sv <= i__1; ++n1sv) {
	    index[i__] = ind2;
	    ++i__;
	    ind2 += *strd2;
/* L20: */
	}
    } else {
/*     N2SV .EQ. 0 */
	i__1 = n1sv;
	for (n2sv = 1; n2sv <= i__1; ++n2sv) {
	    index[i__] = ind1;
	    ++i__;
	    ind1 += *strd1;
/* L30: */
	}
    }

    return 0;

/*     End of SLAMRG */

} /* slamrg_ */

/* Subroutine */ int slaed8_(integer *icompq, integer *k, integer *n, integer 
	*qsiz, real *d__, real *q, integer *ldq, integer *indxq, real *rho, 
	integer *cutpnt, real *z__, real *dlamda, real *q2, integer *ldq2, 
	real *w, integer *perm, integer *givptr, integer *givcol, real *
	givnum, integer *indxp, integer *indx, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,   
       Courant Institute, NAG Ltd., and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLAED8 merges the two sets of eigenvalues together into a single   
    sorted set.  Then it tries to deflate the size of the problem.   
    There are two ways in which deflation can occur:  when two or more   
    eigenvalues are close together or if there is a tiny element in the   
    Z vector.  For each such occurrence the order of the related secular   
    equation problem is reduced by one.   

    Arguments   
    =========   

    ICOMPQ  (input) INTEGER   
            = 0:  Compute eigenvalues only.   
            = 1:  Compute eigenvectors of original dense symmetric matrix   
                  also.  On entry, Q contains the orthogonal matrix used   
                  to reduce the original matrix to tridiagonal form.   

    K      (output) INTEGER   
           The number of non-deflated eigenvalues, and the order of the   
           related secular equation.   

    N      (input) INTEGER   
           The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    QSIZ   (input) INTEGER   
           The dimension of the orthogonal matrix used to reduce   
           the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1.   

    D      (input/output) REAL array, dimension (N)   
           On entry, the eigenvalues of the two submatrices to be   
           combined.  On exit, the trailing (N-K) updated eigenvalues   
           (those which were deflated) sorted into increasing order.   

    Q      (input/output) REAL array, dimension (LDQ,N)   
           If ICOMPQ = 0, Q is not referenced.  Otherwise,   
           on entry, Q contains the eigenvectors of the partially solved   
           system which has been previously updated in matrix   
           multiplies with other partially solved eigensystems.   
           On exit, Q contains the trailing (N-K) updated eigenvectors   
           (those which were deflated) in its last N-K columns.   

    LDQ    (input) INTEGER   
           The leading dimension of the array Q.  LDQ >= max(1,N).   

    INDXQ  (input) INTEGER array, dimension (N)   
           The permutation which separately sorts the two sub-problems   
           in D into ascending order.  Note that elements in the second   
           half of this permutation must first have CUTPNT added to   
           their values in order to be accurate.   

    RHO    (input/output) REAL   
           On entry, the off-diagonal element associated with the rank-1   
           cut which originally split the two submatrices which are now   
           being recombined.   
           On exit, RHO has been modified to the value required by   
           SLAED3.   

    CUTPNT (input) INTEGER   
           The location of the last eigenvalue in the leading   
           sub-matrix.  min(1,N) <= CUTPNT <= N.   

    Z      (input) REAL array, dimension (N)   
           On entry, Z contains the updating vector (the last row of   
           the first sub-eigenvector matrix and the first row of the   
           second sub-eigenvector matrix).   
           On exit, the contents of Z are destroyed by the updating   
           process.   

    DLAMDA (output) REAL array, dimension (N)   
           A copy of the first K eigenvalues which will be used by   
           SLAED3 to form the secular equation.   

    Q2     (output) REAL array, dimension (LDQ2,N)   
           If ICOMPQ = 0, Q2 is not referenced.  Otherwise,   
           a copy of the first K eigenvectors which will be used by   
           SLAED7 in a matrix multiply (SGEMM) to update the new   
           eigenvectors.   

    LDQ2   (input) INTEGER   
           The leading dimension of the array Q2.  LDQ2 >= max(1,N).   

    W      (output) REAL array, dimension (N)   
           The first k values of the final deflation-altered z-vector and   
           will be passed to SLAED3.   

    PERM   (output) INTEGER array, dimension (N)   
           The permutations (from deflation and sorting) to be applied   
           to each eigenblock.   

    GIVPTR (output) INTEGER   
           The number of Givens rotations which took place in this   
           subproblem.   

    GIVCOL (output) INTEGER array, dimension (2, N)   
           Each pair of numbers indicates a pair of columns to take place   
           in a Givens rotation.   

    GIVNUM (output) REAL array, dimension (2, N)   
           Each number indicates the S value to be used in the   
           corresponding Givens rotation.   

    INDXP  (workspace) INTEGER array, dimension (N)   
           The permutation used to place deflated values of D at the end   
           of the array.  INDXP(1:K) points to the nondeflated D-values   
           and INDXP(K+1:N) points to the deflated eigenvalues.   

    INDX   (workspace) INTEGER array, dimension (N)   
           The permutation used to sort the contents of D into ascending   
           order.   

    INFO   (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   



       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static real c_b3 = -1.f;
    static integer c__1 = 1;
    
    /* System generated locals */
    integer q_dim1, q_offset, q2_dim1, q2_offset, i__1;
    real r__1;
    /* Builtin functions */
    // double sqrt(doublereal);
    /* Local variables */
    static integer jlam, imax, jmax;
    extern /* Subroutine */ int srot_(integer *, real *, integer *, real *, 
	    integer *, real *, real *);
    static real c__;
    static integer i__, j;
    static real s, t;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static integer k2;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    static integer n1, n2;
    extern doublereal slapy2_(real *, real *);
    static integer jp;
    extern doublereal slamch_(const char *);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer isamax_(integer *, real *, integer *);
    extern /* Subroutine */ int slamrg_(integer *, integer *, real *, integer 
	    *, integer *, integer *), slacpy_(const char *, integer *, integer *, 
	    real *, integer *, real *, integer *);
    static integer n1p1;
    static real eps, tau, tol;
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]
#define q2_ref(a_1,a_2) q2[(a_2)*q2_dim1 + a_1]
#define givcol_ref(a_1,a_2) givcol[(a_2)*2 + a_1]
#define givnum_ref(a_1,a_2) givnum[(a_2)*2 + a_1]


    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --z__;
    --dlamda;
    q2_dim1 = *ldq2;
    q2_offset = 1 + q2_dim1 * 1;
    q2 -= q2_offset;
    --w;
    --perm;
    givcol -= 3;
    givnum -= 3;
    --indxp;
    --indx;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -3;
    } else if (*icompq == 1 && *qsiz < *n) {
	*info = -4;
    } else if (*ldq < f2cmax(1,*n)) {
	*info = -7;
    } else if (*cutpnt < f2cmin(1,*n) || *cutpnt > *n) {
	*info = -10;
    } else if (*ldq2 < f2cmax(1,*n)) {
	*info = -14;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED8", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    n1 = *cutpnt;
    n2 = *n - n1;
    n1p1 = n1 + 1;

    if (*rho < 0.f) {
	sscal_(&n2, &c_b3, &z__[n1p1], &c__1);
    }

/*     Normalize z so that norm(z) = 1 */

    t = 1.f / sqrt(2.f);
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	indx[j] = j;
/* L10: */
    }
    sscal_(n, &t, &z__[1], &c__1);
    *rho = (r__1 = *rho * 2.f, dabs(r__1));

/*     Sort the eigenvalues into increasing order */

    i__1 = *n;
    for (i__ = *cutpnt + 1; i__ <= i__1; ++i__) {
	indxq[i__] += *cutpnt;
/* L20: */
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = d__[indxq[i__]];
	w[i__] = z__[indxq[i__]];
/* L30: */
    }
    i__ = 1;
    j = *cutpnt + 1;
    slamrg_(&n1, &n2, &dlamda[1], &c__1, &c__1, &indx[1]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__[i__] = dlamda[indx[i__]];
	z__[i__] = w[indx[i__]];
/* L40: */
    }

/*     Calculate the allowable deflation tolerence */

    imax = isamax_(n, &z__[1], &c__1);
    jmax = isamax_(n, &d__[1], &c__1);
    eps = slamch_("Epsilon");
    tol = eps * 8.f * (r__1 = d__[jmax], dabs(r__1));

/*     If the rank-1 modifier is small enough, no more needs to be done   
       except to reorganize Q so that its columns correspond with the   
       elements in D. */

    if (*rho * (r__1 = z__[imax], dabs(r__1)) <= tol) {
	*k = 0;
	if (*icompq == 0) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		perm[j] = indxq[indx[j]];
/* L50: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		perm[j] = indxq[indx[j]];
		scopy_(qsiz, &q_ref(1, perm[j]), &c__1, &q2_ref(1, j), &c__1);
/* L60: */
	    }
	    slacpy_("A", qsiz, n, &q2_ref(1, 1), ldq2, &q_ref(1, 1), ldq);
	}
	return 0;
    }

/*     If there are multiple eigenvalues then the problem deflates.  Here   
       the number of equal eigenvalues are found.  As each equal   
       eigenvalue is found, an elementary reflector is computed to rotate   
       the corresponding eigensubspace so that the corresponding   
       components of Z are zero in this new basis. */

    *k = 0;
    *givptr = 0;
    k2 = *n + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (*rho * (r__1 = z__[j], dabs(r__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    indxp[k2] = j;
	    if (j == *n) {
		goto L110;
	    }
	} else {
	    jlam = j;
	    goto L80;
	}
/* L70: */
    }
L80:
    ++j;
    if (j > *n) {
	goto L100;
    }
    if (*rho * (r__1 = z__[j], dabs(r__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	indxp[k2] = j;
    } else {

/*        Check if eigenvalues are close enough to allow deflation. */

	s = z__[jlam];
	c__ = z__[j];

/*        Find sqrt(a**2+b**2) without overflow or   
          destructive underflow. */

	tau = slapy2_(&c__, &s);
	t = d__[j] - d__[jlam];
	c__ /= tau;
	s = -s / tau;
	if ((r__1 = t * c__ * s, dabs(r__1)) <= tol) {

/*           Deflation is possible. */

	    z__[j] = tau;
	    z__[jlam] = 0.f;

/*           Record the appropriate Givens rotation */

	    ++(*givptr);
	    givcol_ref(1, *givptr) = indxq[indx[jlam]];
	    givcol_ref(2, *givptr) = indxq[indx[j]];
	    givnum_ref(1, *givptr) = c__;
	    givnum_ref(2, *givptr) = s;
	    if (*icompq == 1) {
		srot_(qsiz, &q_ref(1, indxq[indx[jlam]]), &c__1, &q_ref(1, 
			indxq[indx[j]]), &c__1, &c__, &s);
	    }
	    t = d__[jlam] * c__ * c__ + d__[j] * s * s;
	    d__[j] = d__[jlam] * s * s + d__[j] * c__ * c__;
	    d__[jlam] = t;
	    --k2;
	    i__ = 1;
L90:
	    if (k2 + i__ <= *n) {
		if (d__[jlam] < d__[indxp[k2 + i__]]) {
		    indxp[k2 + i__ - 1] = indxp[k2 + i__];
		    indxp[k2 + i__] = jlam;
		    ++i__;
		    goto L90;
		} else {
		    indxp[k2 + i__ - 1] = jlam;
		}
	    } else {
		indxp[k2 + i__ - 1] = jlam;
	    }
	    jlam = j;
	} else {
	    ++(*k);
	    w[*k] = z__[jlam];
	    dlamda[*k] = d__[jlam];
	    indxp[*k] = jlam;
	    jlam = j;
	}
    }
    goto L80;
L100:

/*     Record the last eigenvalue. */

    ++(*k);
    w[*k] = z__[jlam];
    dlamda[*k] = d__[jlam];
    indxp[*k] = jlam;

L110:

/*     Sort the eigenvalues and corresponding eigenvectors into DLAMDA   
       and Q2 respectively.  The eigenvalues/vectors which were not   
       deflated go into the first K slots of DLAMDA and Q2 respectively,   
       while those which were deflated go into the last N - K slots. */

    if (*icompq == 0) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    jp = indxp[j];
	    dlamda[j] = d__[jp];
	    perm[j] = indxq[indx[jp]];
/* L120: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    jp = indxp[j];
	    dlamda[j] = d__[jp];
	    perm[j] = indxq[indx[jp]];
	    scopy_(qsiz, &q_ref(1, perm[j]), &c__1, &q2_ref(1, j), &c__1);
/* L130: */
	}
    }

/*     The deflated eigenvalues and their corresponding vectors go back   
       into the last N - K slots of D and Q respectively. */

    if (*k < *n) {
	if (*icompq == 0) {
	    i__1 = *n - *k;
	    scopy_(&i__1, &dlamda[*k + 1], &c__1, &d__[*k + 1], &c__1);
	} else {
	    i__1 = *n - *k;
	    scopy_(&i__1, &dlamda[*k + 1], &c__1, &d__[*k + 1], &c__1);
	    i__1 = *n - *k;
	    slacpy_("A", qsiz, &i__1, &q2_ref(1, *k + 1), ldq2, &q_ref(1, *k 
		    + 1), ldq);
	}
    }

    return 0;

/*     End of SLAED8 */

} /* slaed8_ */

#undef givnum_ref
#undef givcol_ref
#undef q2_ref
#undef q_ref

/*  -- translated by f2c (version 19990503).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

/* Table of constant values */

static real c_b3 = -1.f;
static integer c__1 = 1;

/* Subroutine */ int slaed2_(integer *k, integer *n, integer *n1, real *d__, 
	real *q, integer *ldq, integer *indxq, real *rho, real *z__, real *
	dlamda, real *w, real *q2, integer *indx, integer *indxc, integer *
	indxp, integer *coltyp, integer *info)
{
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    // double sqrt(doublereal);

    /* Local variables */
    static integer imax, jmax, ctot[4];
    extern /* Subroutine */ int srot_(integer *, real *, integer *, real *, 
	    integer *, real *, real *);
    static real c__;
    static integer i__, j;
    static real s, t;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static integer k2;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    static integer n2;
    extern doublereal slapy2_(real *, real *);
    static integer ct, nj, pj, js;
    extern doublereal slamch_(const char *);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer isamax_(integer *, real *, integer *);
    extern /* Subroutine */ int slamrg_(integer *, integer *, real *, integer 
	    *, integer *, integer *), slacpy_(const char *, integer *, integer *, 
	    real *, integer *, real *, integer *);
    static integer iq1, iq2, n1p1;
    static real eps, tau, tol;
    static integer psm[4];


#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]


/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SLAED2 merges the two sets of eigenvalues together into a single   
    sorted set.  Then it tries to deflate the size of the problem.   
    There are two ways in which deflation can occur:  when two or more   
    eigenvalues are close together or if there is a tiny entry in the   
    Z vector.  For each such occurrence the order of the related secular   
    equation problem is reduced by one.   

    Arguments   
    =========   

    K      (output) INTEGER   
           The number of non-deflated eigenvalues, and the order of the   
           related secular equation. 0 <= K <=N.   

    N      (input) INTEGER   
           The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    N1     (input) INTEGER   
           The location of the last eigenvalue in the leading sub-matrix.   
           f2cmin(1,N) <= N1 <= N/2.   

    D      (input/output) REAL array, dimension (N)   
           On entry, D contains the eigenvalues of the two submatrices to   
           be combined.   
           On exit, D contains the trailing (N-K) updated eigenvalues   
           (those which were deflated) sorted into increasing order.   

    Q      (input/output) REAL array, dimension (LDQ, N)   
           On entry, Q contains the eigenvectors of two submatrices in   
           the two square blocks with corners at (1,1), (N1,N1)   
           and (N1+1, N1+1), (N,N).   
           On exit, Q contains the trailing (N-K) updated eigenvectors   
           (those which were deflated) in its last N-K columns.   

    LDQ    (input) INTEGER   
           The leading dimension of the array Q.  LDQ >= max(1,N).   

    INDXQ  (input/output) INTEGER array, dimension (N)   
           The permutation which separately sorts the two sub-problems   
           in D into ascending order.  Note that elements in the second   
           half of this permutation must first have N1 added to their   
           values. Destroyed on exit.   

    RHO    (input/output) REAL   
           On entry, the off-diagonal element associated with the rank-1   
           cut which originally split the two submatrices which are now   
           being recombined.   
           On exit, RHO has been modified to the value required by   
           SLAED3.   

    Z      (input) REAL array, dimension (N)   
           On entry, Z contains the updating vector (the last   
           row of the first sub-eigenvector matrix and the first row of   
           the second sub-eigenvector matrix).   
           On exit, the contents of Z have been destroyed by the updating   
           process.   

    DLAMDA (output) REAL array, dimension (N)   
           A copy of the first K eigenvalues which will be used by   
           SLAED3 to form the secular equation.   

    W      (output) REAL array, dimension (N)   
           The first k values of the final deflation-altered z-vector   
           which will be passed to SLAED3.   

    Q2     (output) REAL array, dimension (N1**2+(N-N1)**2)   
           A copy of the first K eigenvectors which will be used by   
           SLAED3 in a matrix multiply (SGEMM) to solve for the new   
           eigenvectors.   

    INDX   (workspace) INTEGER array, dimension (N)   
           The permutation used to sort the contents of DLAMDA into   
           ascending order.   

    INDXC  (output) INTEGER array, dimension (N)   
           The permutation used to arrange the columns of the deflated   
           Q matrix into three groups:  the first group contains non-zero   
           elements only at and above N1, the second contains   
           non-zero elements only below N1, and the third is dense.   

    INDXP  (workspace) INTEGER array, dimension (N)   
           The permutation used to place deflated values of D at the end   
           of the array.  INDXP(1:K) points to the nondeflated D-values   
           and INDXP(K+1:N) points to the deflated eigenvalues.   

    COLTYP (workspace/output) INTEGER array, dimension (N)   
           During execution, a label which will indicate which of the   
           following types a column in the Q2 matrix is:   
           1 : non-zero in the upper half only;   
           2 : dense;   
           3 : non-zero in the lower half only;   
           4 : deflated.   
           On exit, COLTYP(i) is the number of columns of type i,   
           for i=1 to 4 only.   

    INFO   (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   
    Modified by Francoise Tisseur, University of Tennessee.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --indxq;
    --z__;
    --dlamda;
    --w;
    --q2;
    --indx;
    --indxc;
    --indxp;
    --coltyp;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -2;
    } else if (*ldq < f2cmax(1,*n)) {
	*info = -6;
    } else /* if(complicated condition) */ {
/* Computing F2CMIN */
	i__1 = 1, i__2 = *n / 2;
	if (f2cmin(i__1,i__2) > *n1 || *n / 2 < *n1) {
	    *info = -3;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    n2 = *n - *n1;
    n1p1 = *n1 + 1;

    if (*rho < 0.f) {
	sscal_(&n2, &c_b3, &z__[n1p1], &c__1);
    }

/*     Normalize z so that norm(z) = 1.  Since z is the concatenation of   
       two normalized vectors, norm2(z) = sqrt(2). */

    t = 1.f / sqrt(2.f);
    sscal_(n, &t, &z__[1], &c__1);

/*     RHO = ABS( norm(z)**2 * RHO ) */

    *rho = (r__1 = *rho * 2.f, dabs(r__1));

/*     Sort the eigenvalues into increasing order */

    i__1 = *n;
    for (i__ = n1p1; i__ <= i__1; ++i__) {
	indxq[i__] += *n1;
/* L10: */
    }

/*     re-integrate the deflated parts from the last pass */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = d__[indxq[i__]];
/* L20: */
    }
    slamrg_(n1, &n2, &dlamda[1], &c__1, &c__1, &indxc[1]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	indx[i__] = indxq[indxc[i__]];
/* L30: */
    }

/*     Calculate the allowable deflation tolerance */

    imax = isamax_(n, &z__[1], &c__1);
    jmax = isamax_(n, &d__[1], &c__1);
    eps = slamch_("Epsilon");
/* Computing MAX */
    r__3 = (r__1 = d__[jmax], dabs(r__1)), r__4 = (r__2 = z__[imax], dabs(
	    r__2));
    tol = eps * 8.f * df2cmax(r__3,r__4);

/*     If the rank-1 modifier is small enough, no more needs to be done   
       except to reorganize Q so that its columns correspond with the   
       elements in D. */

    if (*rho * (r__1 = z__[imax], dabs(r__1)) <= tol) {
	*k = 0;
	iq2 = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__ = indx[j];
	    scopy_(n, &q_ref(1, i__), &c__1, &q2[iq2], &c__1);
	    dlamda[j] = d__[i__];
	    iq2 += *n;
/* L40: */
	}
	slacpy_("A", n, n, &q2[1], n, &q[q_offset], ldq);
	scopy_(n, &dlamda[1], &c__1, &d__[1], &c__1);
	goto L190;
    }

/*     If there are multiple eigenvalues then the problem deflates.  Here   
       the number of equal eigenvalues are found.  As each equal   
       eigenvalue is found, an elementary reflector is computed to rotate   
       the corresponding eigensubspace so that the corresponding   
       components of Z are zero in this new basis. */

    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	coltyp[i__] = 1;
/* L50: */
    }
    i__1 = *n;
    for (i__ = n1p1; i__ <= i__1; ++i__) {
	coltyp[i__] = 3;
/* L60: */
    }


    *k = 0;
    k2 = *n + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	nj = indx[j];
	if (*rho * (r__1 = z__[nj], dabs(r__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    coltyp[nj] = 4;
	    indxp[k2] = nj;
	    if (j == *n) {
		goto L100;
	    }
	} else {
	    pj = nj;
	    goto L80;
	}
/* L70: */
    }
L80:
    ++j;
    nj = indx[j];
    if (j > *n) {
	goto L100;
    }
    if (*rho * (r__1 = z__[nj], dabs(r__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	coltyp[nj] = 4;
	indxp[k2] = nj;
    } else {

/*        Check if eigenvalues are close enough to allow deflation. */

	s = z__[pj];
	c__ = z__[nj];

/*        Find sqrt(a**2+b**2) without overflow or   
          destructive underflow. */

	tau = slapy2_(&c__, &s);
	t = d__[nj] - d__[pj];
	c__ /= tau;
	s = -s / tau;
	if ((r__1 = t * c__ * s, dabs(r__1)) <= tol) {

/*           Deflation is possible. */

	    z__[nj] = tau;
	    z__[pj] = 0.f;
	    if (coltyp[nj] != coltyp[pj]) {
		coltyp[nj] = 2;
	    }
	    coltyp[pj] = 4;
	    srot_(n, &q_ref(1, pj), &c__1, &q_ref(1, nj), &c__1, &c__, &s);
/* Computing 2nd power */
	    r__1 = c__;
/* Computing 2nd power */
	    r__2 = s;
	    t = d__[pj] * (r__1 * r__1) + d__[nj] * (r__2 * r__2);
/* Computing 2nd power */
	    r__1 = s;
/* Computing 2nd power */
	    r__2 = c__;
	    d__[nj] = d__[pj] * (r__1 * r__1) + d__[nj] * (r__2 * r__2);
	    d__[pj] = t;
	    --k2;
	    i__ = 1;
L90:
	    if (k2 + i__ <= *n) {
		if (d__[pj] < d__[indxp[k2 + i__]]) {
		    indxp[k2 + i__ - 1] = indxp[k2 + i__];
		    indxp[k2 + i__] = pj;
		    ++i__;
		    goto L90;
		} else {
		    indxp[k2 + i__ - 1] = pj;
		}
	    } else {
		indxp[k2 + i__ - 1] = pj;
	    }
	    pj = nj;
	} else {
	    ++(*k);
	    dlamda[*k] = d__[pj];
	    w[*k] = z__[pj];
	    indxp[*k] = pj;
	    pj = nj;
	}
    }
    goto L80;
L100:

/*     Record the last eigenvalue. */

    ++(*k);
    dlamda[*k] = d__[pj];
    w[*k] = z__[pj];
    indxp[*k] = pj;

/*     Count up the total number of the various types of columns, then   
       form a permutation which positions the four column types into   
       four uniform groups (although one or more of these groups may be   
       empty). */

    for (j = 1; j <= 4; ++j) {
	ctot[j - 1] = 0;
/* L110: */
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	ct = coltyp[j];
	++ctot[ct - 1];
/* L120: */
    }

/*     PSM(*) = Position in SubMatrix (of types 1 through 4) */

    psm[0] = 1;
    psm[1] = ctot[0] + 1;
    psm[2] = psm[1] + ctot[1];
    psm[3] = psm[2] + ctot[2];
    *k = *n - ctot[3];

/*     Fill out the INDXC array so that the permutation which it induces   
       will place all type-1 columns first, all type-2 columns next,   
       then all type-3's, and finally all type-4's. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	js = indxp[j];
	ct = coltyp[js];
	indx[psm[ct - 1]] = js;
	indxc[psm[ct - 1]] = j;
	++psm[ct - 1];
/* L130: */
    }

/*     Sort the eigenvalues and corresponding eigenvectors into DLAMDA   
       and Q2 respectively.  The eigenvalues/vectors which were not   
       deflated go into the first K slots of DLAMDA and Q2 respectively,   
       while those which were deflated go into the last N - K slots. */

    i__ = 1;
    iq1 = 1;
    iq2 = (ctot[0] + ctot[1]) * *n1 + 1;
    i__1 = ctot[0];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	scopy_(n1, &q_ref(1, js), &c__1, &q2[iq1], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq1 += *n1;
/* L140: */
    }

    i__1 = ctot[1];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	scopy_(n1, &q_ref(1, js), &c__1, &q2[iq1], &c__1);
	scopy_(&n2, &q_ref(*n1 + 1, js), &c__1, &q2[iq2], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq1 += *n1;
	iq2 += n2;
/* L150: */
    }

    i__1 = ctot[2];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	scopy_(&n2, &q_ref(*n1 + 1, js), &c__1, &q2[iq2], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq2 += n2;
/* L160: */
    }

    iq1 = iq2;
    i__1 = ctot[3];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	scopy_(n, &q_ref(1, js), &c__1, &q2[iq2], &c__1);
	iq2 += *n;
	z__[i__] = d__[js];
	++i__;
/* L170: */
    }

/*     The deflated eigenvalues and their corresponding vectors go back   
       into the last N - K slots of D and Q respectively. */

    slacpy_("A", n, &ctot[3], &q2[iq1], n, &q_ref(1, *k + 1), ldq);
    i__1 = *n - *k;
    scopy_(&i__1, &z__[*k + 1], &c__1, &d__[*k + 1], &c__1);

/*     Copy CTOT into COLTYP for referencing in SLAED3. */

    for (j = 1; j <= 4; ++j) {
	coltyp[j] = ctot[j - 1];
/* L180: */
    }

L190:
    return 0;

/*     End of SLAED2 */

} /* slaed2_ */

#undef q_ref


/* Subroutine */ int slaed9_(integer *k, integer *kstart, integer *kstop, 
	integer *n, real *d__, real *q, integer *ldq, real *rho, real *dlamda,
	 real *w, real *s, integer *lds, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,   
       Courant Institute, NAG Ltd., and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLAED9 finds the roots of the secular equation, as defined by the   
    values in D, Z, and RHO, between KSTART and KSTOP.  It makes the   
    appropriate calls to SLAED4 and then stores the new matrix of   
    eigenvectors for use in calculating the next level of Z vectors.   

    Arguments   
    =========   

    K       (input) INTEGER   
            The number of terms in the rational function to be solved by   
            SLAED4.  K >= 0.   

    KSTART  (input) INTEGER   
    KSTOP   (input) INTEGER   
            The updated eigenvalues Lambda(I), KSTART <= I <= KSTOP   
            are to be computed.  1 <= KSTART <= KSTOP <= K.   

    N       (input) INTEGER   
            The number of rows and columns in the Q matrix.   
            N >= K (delation may result in N > K).   

    D       (output) REAL array, dimension (N)   
            D(I) contains the updated eigenvalues   
            for KSTART <= I <= KSTOP.   

    Q       (workspace) REAL array, dimension (LDQ,N)   

    LDQ     (input) INTEGER   
            The leading dimension of the array Q.  LDQ >= max( 1, N ).   

    RHO     (input) REAL   
            The value of the parameter in the rank one update equation.   
            RHO >= 0 required.   

    DLAMDA  (input) REAL array, dimension (K)   
            The first K elements of this array contain the old roots   
            of the deflated updating problem.  These are the poles   
            of the secular equation.   

    W       (input) REAL array, dimension (K)   
            The first K elements of this array contain the components   
            of the deflation-adjusted updating vector.   

    S       (output) REAL array, dimension (LDS, K)   
            Will contain the eigenvectors of the repaired matrix which   
            will be stored for subsequent Z vector calculation and   
            multiplied by the previously accumulated eigenvectors   
            to update the system.   

    LDS     (input) INTEGER   
            The leading dimension of S.  LDS >= max( 1, K ).   

    INFO    (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  if INFO = 1, an eigenvalue did not converge   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer q_dim1, q_offset, s_dim1, s_offset, i__1, i__2;
    real r__1;
    /* Builtin functions */
    // double sqrt(doublereal), r_sign(real *, real *);
    /* Local variables */
    static real temp;
    extern doublereal snrm2_(integer *, real *, integer *);
    static integer i__, j;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), slaed4_(integer *, integer *, real *, real *, real *, 
	    real *, real *, integer *);
    extern doublereal slamc3_(real *, real *);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]
#define s_ref(a_1,a_2) s[(a_2)*s_dim1 + a_1]


    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --dlamda;
    --w;
    s_dim1 = *lds;
    s_offset = 1 + s_dim1 * 1;
    s -= s_offset;

    /* Function Body */
    *info = 0;

    if (*k < 0) {
	*info = -1;
    } else if (*kstart < 1 || *kstart > f2cmax(1,*k)) {
	*info = -2;
    } else if (f2cmax(1,*kstop) < *kstart || *kstop > f2cmax(1,*k)) {
	*info = -3;
    } else if (*n < *k) {
	*info = -4;
    } else if (*ldq < f2cmax(1,*k)) {
	*info = -7;
    } else if (*lds < f2cmax(1,*k)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED9", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 0) {
	return 0;
    }

/*     Modify values DLAMDA(i) to make sure all DLAMDA(i)-DLAMDA(j) can   
       be computed with high relative accuracy (barring over/underflow).   
       This is a problem on machines without a guard digit in   
       add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2).   
       The following code replaces DLAMDA(I) by 2*DLAMDA(I)-DLAMDA(I),   
       which on any of these machines zeros out the bottommost   
       bit of DLAMDA(I) if it is 1; this makes the subsequent   
       subtractions DLAMDA(I)-DLAMDA(J) unproblematic when cancellation   
       occurs. On binary machines with a guard digit (almost all   
       machines) it does not change DLAMDA(I) at all. On hexadecimal   
       and decimal machines with a guard digit, it slightly   
       changes the bottommost bits of DLAMDA(I). It does not account   
       for hexadecimal or decimal machines without guard digits   
       (we know of none). We use a subroutine call to compute   
       2*DLAMBDA(I) to prevent optimizing compilers from eliminating   
       this code. */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = slamc3_(&dlamda[i__], &dlamda[i__]) - dlamda[i__];
/* L10: */
    }

    i__1 = *kstop;
    for (j = *kstart; j <= i__1; ++j) {
	slaed4_(k, &j, &dlamda[1], &w[1], &q_ref(1, j), rho, &d__[j], info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    goto L120;
	}
/* L20: */
    }

    if (*k == 1 || *k == 2) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *k;
	    for (j = 1; j <= i__2; ++j) {
		s_ref(j, i__) = q_ref(j, i__);
/* L30: */
	    }
/* L40: */
	}
	goto L120;
    }

/*     Compute updated W. */

    scopy_(k, &w[1], &c__1, &s[s_offset], &c__1);

/*     Initialize W(I) = Q(I,I) */

    i__1 = *ldq + 1;
    scopy_(k, &q[q_offset], &i__1, &w[1], &c__1);
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[i__] *= q_ref(i__, j) / (dlamda[i__] - dlamda[j]);
/* L50: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    w[i__] *= q_ref(i__, j) / (dlamda[i__] - dlamda[j]);
/* L60: */
	}
/* L70: */
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__1 = sqrt(-w[i__]);
	w[i__] = r_sign(&r__1, &s_ref(i__, 1));
/* L80: */
    }

/*     Compute eigenvectors of the modified rank-1 modification. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    q_ref(i__, j) = w[i__] / q_ref(i__, j);
/* L90: */
	}
	temp = snrm2_(k, &q_ref(1, j), &c__1);
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    s_ref(i__, j) = q_ref(i__, j) / temp;
/* L100: */
	}
/* L110: */
    }

L120:
    return 0;

/*     End of SLAED9 */

} /* slaed9_ */

#undef s_ref
#undef q_ref


integer isamax_(integer *n, real *sx, integer *incx)
{
    /* System generated locals */
    integer ret_val, i__1;
    real r__1;
    /* Local variables */
    static real smax;
    static integer i__, ix;
/*     finds the index of element having max. absolute value.   
       jack dongarra, linpack, 3/11/78.   
       modified 3/93 to return if incx .le. 0.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sx;
    /* Function Body */
    ret_val = 0;
    if (*n < 1 || *incx <= 0) {
	return ret_val;
    }
    ret_val = 1;
    if (*n == 1) {
	return ret_val;
    }
    if (*incx == 1) {
	goto L20;
    }
/*        code for increment not equal to 1 */
    ix = 1;
    smax = dabs(sx[1]);
    ix += *incx;
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	if ((r__1 = sx[ix], dabs(r__1)) <= smax) {
	    goto L5;
	}
	ret_val = i__;
	smax = (r__1 = sx[ix], dabs(r__1));
L5:
	ix += *incx;
/* L10: */
    }
    return ret_val;
/*        code for increment equal to 1 */
L20:
    smax = dabs(sx[1]);
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	if ((r__1 = sx[i__], dabs(r__1)) <= smax) {
	    goto L30;
	}
	ret_val = i__;
	smax = (r__1 = sx[i__], dabs(r__1));
L30:
	;
    }
    return ret_val;
} /* isamax_ */

/* Subroutine */ int srot_(integer *n, real *sx, integer *incx, real *sy, 
	integer *incy, real *c__, real *s)
{
    /* System generated locals */
    integer i__1;
    /* Local variables */
    static integer i__;
    static real stemp;
    static integer ix, iy;
/*     applies a plane rotation.   
       jack dongarra, linpack, 3/11/78.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --sy;
    --sx;
    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }
/*       code for unequal increments or equal increments not equal   
           to 1 */
    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	stemp = *c__ * sx[ix] + *s * sy[iy];
	sy[iy] = *c__ * sy[iy] - *s * sx[ix];
	sx[ix] = stemp;
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;
/*       code for both increments equal to 1 */
L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	stemp = *c__ * sx[i__] + *s * sy[i__];
	sy[i__] = *c__ * sy[i__] - *s * sx[i__];
	sx[i__] = stemp;
/* L30: */
    }
    return 0;
} /* srot_ */

/* Subroutine */ int slaed4_(integer *n, integer *i__, real *d__, real *z__, 
	real *delta, real *rho, real *dlam, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,   
       Courant Institute, NAG Ltd., and Rice University   
       December 23, 1999   


    Purpose   
    =======   

    This subroutine computes the I-th updated eigenvalue of a symmetric   
    rank-one modification to a diagonal matrix whose elements are   
    given in the array d, and that   

               D(i) < D(j)  for  i < j   

    and that RHO > 0.  This is arranged by the calling routine, and is   
    no loss in generality.  The rank-one modified system is thus   

               diag( D )  +  RHO *  Z * Z_transpose.   

    where we assume the Euclidean norm of Z is 1.   

    The method consists of approximating the rational functions in the   
    secular equation by simpler interpolating rational functions.   

    Arguments   
    =========   

    N      (input) INTEGER   
           The length of all arrays.   

    I      (input) INTEGER   
           The index of the eigenvalue to be computed.  1 <= I <= N.   

    D      (input) REAL array, dimension (N)   
           The original eigenvalues.  It is assumed that they are in   
           order, D(I) < D(J)  for I < J.   

    Z      (input) REAL array, dimension (N)   
           The components of the updating vector.   

    DELTA  (output) REAL array, dimension (N)   
           If N .ne. 1, DELTA contains (D(j) - lambda_I) in its  j-th   
           component.  If N = 1, then DELTA(1) = 1.  The vector DELTA   
           contains the information necessary to construct the   
           eigenvectors.   

    RHO    (input) REAL   
           The scalar in the symmetric updating formula.   

    DLAM   (output) REAL   
           The computed lambda_I, the I-th updated eigenvalue.   

    INFO   (output) INTEGER   
           = 0:  successful exit   
           > 0:  if INFO = 1, the updating process failed.   

    Internal Parameters   
    ===================   

    Logical variable ORGATI (origin-at-i?) is used for distinguishing   
    whether D(i) or D(i+1) is treated as the origin.   

              ORGATI = .true.    origin at i   
              ORGATI = .false.   origin at i+1   

     Logical variable SWTCH3 (switch-for-3-poles?) is for noting   
     if we are working with THREE poles!   

     MAXIT is the maximum number of iterations allowed for each   
     eigenvalue.   

    Further Details   
    ===============   

    Based on contributions by   
       Ren-Cang Li, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   


       Since this routine is called in an inner loop, we do no argument   
       checking.   

       Quick return for N=1 and 2.   

       Parameter adjustments */
    /* System generated locals */
    integer i__1;
    real r__1;
    /* Builtin functions */
    // double sqrt(doublereal);
    /* Local variables */
    static real dphi, dpsi;
    static integer iter;
    static real temp, prew, temp1, a, b, c__;
    static integer j;
    static real w, dltlb, dltub, midpt;
    static integer niter;
    static logical swtch;
    extern /* Subroutine */ int slaed5_(integer *, real *, real *, real *, 
	    real *, real *), slaed6_(integer *, logical *, real *, real *, 
	    real *, real *, real *, integer *);
    static logical swtch3;
    static integer ii;
    static real dw;
    extern doublereal slamch_(const char *);
    static real zz[3];
    static logical orgati;
    static real erretm, rhoinv;
    static integer ip1;
    static real del, eta, phi, eps, tau, psi;
    static integer iim1, iip1;

    --delta;
    --z__;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n == 1) {

/*         Presumably, I=1 upon entry */

	*dlam = d__[1] + *rho * z__[1] * z__[1];
	delta[1] = 1.f;
	return 0;
    }
    if (*n == 2) {
	slaed5_(i__, &d__[1], &z__[1], &delta[1], rho, dlam);
	return 0;
    }

/*     Compute machine epsilon */

    eps = slamch_("Epsilon");
    rhoinv = 1.f / *rho;

/*     The case I = N */

    if (*i__ == *n) {

/*        Initialize some basic variables */

	ii = *n - 1;
	niter = 1;

/*        Calculate initial guess */

	midpt = *rho / 2.f;

/*        If ||Z||_2 is not one, then TEMP should be set to   
          RHO * ||Z||_2^2 / TWO */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - midpt;
/* L10: */
	}

	psi = 0.f;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / delta[j];
/* L20: */
	}

	c__ = rhoinv + psi;
	w = c__ + z__[ii] * z__[ii] / delta[ii] + z__[*n] * z__[*n] / delta[*
		n];

	if (w <= 0.f) {
	    temp = z__[*n - 1] * z__[*n - 1] / (d__[*n] - d__[*n - 1] + *rho) 
		    + z__[*n] * z__[*n] / *rho;
	    if (c__ <= temp) {
		tau = *rho;
	    } else {
		del = d__[*n] - d__[*n - 1];
		a = -c__ * del + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n]
			;
		b = z__[*n] * z__[*n] * del;
		if (a < 0.f) {
		    tau = b * 2.f / (sqrt(a * a + b * 4.f * c__) - a);
		} else {
		    tau = (a + sqrt(a * a + b * 4.f * c__)) / (c__ * 2.f);
		}
	    }

/*           It can be proved that   
                 D(N)+RHO/2 <= LAMBDA(N) < D(N)+TAU <= D(N)+RHO */

	    dltlb = midpt;
	    dltub = *rho;
	} else {
	    del = d__[*n] - d__[*n - 1];
	    a = -c__ * del + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n];
	    b = z__[*n] * z__[*n] * del;
	    if (a < 0.f) {
		tau = b * 2.f / (sqrt(a * a + b * 4.f * c__) - a);
	    } else {
		tau = (a + sqrt(a * a + b * 4.f * c__)) / (c__ * 2.f);
	    }

/*           It can be proved that   
                 D(N) < D(N)+TAU < LAMBDA(N) < D(N)+RHO/2 */

	    dltlb = 0.f;
	    dltub = midpt;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - tau;
/* L30: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.f;
	psi = 0.f;
	erretm = 0.f;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L40: */
	}
	erretm = dabs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / delta[*n];
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8.f + erretm - phi + rhoinv + dabs(tau) * (
		dpsi + dphi);

	w = rhoinv + phi + psi;

/*        Test for convergence */

	if (dabs(w) <= eps * erretm) {
	    *dlam = d__[*i__] + tau;
	    goto L250;
	}

	if (w <= 0.f) {
	    dltlb = df2cmax(dltlb,tau);
	} else {
	    dltub = df2cmin(dltub,tau);
	}

/*        Calculate the new step */

	++niter;
	c__ = w - delta[*n - 1] * dpsi - delta[*n] * dphi;
	a = (delta[*n - 1] + delta[*n]) * w - delta[*n - 1] * delta[*n] * (
		dpsi + dphi);
	b = delta[*n - 1] * delta[*n] * w;
	if (c__ < 0.f) {
	    c__ = dabs(c__);
	}
	if (c__ == 0.f) {
/*          ETA = B/A   
             ETA = RHO - TAU */
	    eta = dltub - tau;
	} else if (a >= 0.f) {
	    eta = (a + sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1)))) / (
		    c__ * 2.f);
	} else {
	    eta = b * 2.f / (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(
		    r__1))));
	}

/*        Note, eta should be positive if w is negative, and   
          eta should be negative otherwise. However,   
          if for some reason caused by roundoff, eta*w > 0,   
          we simply use one Newton step instead. This way   
          will guarantee eta*w < 0. */

	if (w * eta > 0.f) {
	    eta = -w / (dpsi + dphi);
	}
	temp = tau + eta;
	if (temp > dltub || temp < dltlb) {
	    if (w < 0.f) {
		eta = (dltub - tau) / 2.f;
	    } else {
		eta = (dltlb - tau) / 2.f;
	    }
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
/* L50: */
	}

	tau += eta;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.f;
	psi = 0.f;
	erretm = 0.f;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L60: */
	}
	erretm = dabs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / delta[*n];
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8.f + erretm - phi + rhoinv + dabs(tau) * (
		dpsi + dphi);

	w = rhoinv + phi + psi;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 30; ++niter) {

/*           Test for convergence */

	    if (dabs(w) <= eps * erretm) {
		*dlam = d__[*i__] + tau;
		goto L250;
	    }

	    if (w <= 0.f) {
		dltlb = df2cmax(dltlb,tau);
	    } else {
		dltub = df2cmin(dltub,tau);
	    }

/*           Calculate the new step */

	    c__ = w - delta[*n - 1] * dpsi - delta[*n] * dphi;
	    a = (delta[*n - 1] + delta[*n]) * w - delta[*n - 1] * delta[*n] * 
		    (dpsi + dphi);
	    b = delta[*n - 1] * delta[*n] * w;
	    if (a >= 0.f) {
		eta = (a + sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1)))) /
			 (c__ * 2.f);
	    } else {
		eta = b * 2.f / (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(
			r__1))));
	    }

/*           Note, eta should be positive if w is negative, and   
             eta should be negative otherwise. However,   
             if for some reason caused by roundoff, eta*w > 0,   
             we simply use one Newton step instead. This way   
             will guarantee eta*w < 0. */

	    if (w * eta > 0.f) {
		eta = -w / (dpsi + dphi);
	    }
	    temp = tau + eta;
	    if (temp > dltub || temp < dltlb) {
		if (w < 0.f) {
		    eta = (dltub - tau) / 2.f;
		} else {
		    eta = (dltlb - tau) / 2.f;
		}
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
/* L70: */
	    }

	    tau += eta;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.f;
	    psi = 0.f;
	    erretm = 0.f;
	    i__1 = ii;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / delta[j];
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L80: */
	    }
	    erretm = dabs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    temp = z__[*n] / delta[*n];
	    phi = z__[*n] * temp;
	    dphi = temp * temp;
	    erretm = (-phi - psi) * 8.f + erretm - phi + rhoinv + dabs(tau) * 
		    (dpsi + dphi);

	    w = rhoinv + phi + psi;
/* L90: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	*dlam = d__[*i__] + tau;
	goto L250;

/*        End for the case I = N */

    } else {

/*        The case for I < N */

	niter = 1;
	ip1 = *i__ + 1;

/*        Calculate initial guess */

	del = d__[ip1] - d__[*i__];
	midpt = del / 2.f;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - midpt;
/* L100: */
	}

	psi = 0.f;
	i__1 = *i__ - 1;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / delta[j];
/* L110: */
	}

	phi = 0.f;
	i__1 = *i__ + 2;
	for (j = *n; j >= i__1; --j) {
	    phi += z__[j] * z__[j] / delta[j];
/* L120: */
	}
	c__ = rhoinv + psi + phi;
	w = c__ + z__[*i__] * z__[*i__] / delta[*i__] + z__[ip1] * z__[ip1] / 
		delta[ip1];

	if (w > 0.f) {

/*           d(i)< the ith eigenvalue < (d(i)+d(i+1))/2   

             We choose d(i) as origin. */

	    orgati = TRUE_;
	    a = c__ * del + z__[*i__] * z__[*i__] + z__[ip1] * z__[ip1];
	    b = z__[*i__] * z__[*i__] * del;
	    if (a > 0.f) {
		tau = b * 2.f / (a + sqrt((r__1 = a * a - b * 4.f * c__, dabs(
			r__1))));
	    } else {
		tau = (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1)))) /
			 (c__ * 2.f);
	    }
	    dltlb = 0.f;
	    dltub = midpt;
	} else {

/*           (d(i)+d(i+1))/2 <= the ith eigenvalue < d(i+1)   

             We choose d(i+1) as origin. */

	    orgati = FALSE_;
	    a = c__ * del - z__[*i__] * z__[*i__] - z__[ip1] * z__[ip1];
	    b = z__[ip1] * z__[ip1] * del;
	    if (a < 0.f) {
		tau = b * 2.f / (a - sqrt((r__1 = a * a + b * 4.f * c__, dabs(
			r__1))));
	    } else {
		tau = -(a + sqrt((r__1 = a * a + b * 4.f * c__, dabs(r__1)))) 
			/ (c__ * 2.f);
	    }
	    dltlb = -midpt;
	    dltub = 0.f;
	}

	if (orgati) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] = d__[j] - d__[*i__] - tau;
/* L130: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] = d__[j] - d__[ip1] - tau;
/* L140: */
	    }
	}
	if (orgati) {
	    ii = *i__;
	} else {
	    ii = *i__ + 1;
	}
	iim1 = ii - 1;
	iip1 = ii + 1;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.f;
	psi = 0.f;
	erretm = 0.f;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L150: */
	}
	erretm = dabs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.f;
	phi = 0.f;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / delta[j];
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L160: */
	}

	w = rhoinv + phi + psi;

/*        W is the value of the secular function with   
          its ii-th element removed. */

	swtch3 = FALSE_;
	if (orgati) {
	    if (w < 0.f) {
		swtch3 = TRUE_;
	    }
	} else {
	    if (w > 0.f) {
		swtch3 = TRUE_;
	    }
	}
	if (ii == 1 || ii == *n) {
	    swtch3 = FALSE_;
	}

	temp = z__[ii] / delta[ii];
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w += temp;
	erretm = (phi - psi) * 8.f + erretm + rhoinv * 2.f + dabs(temp) * 3.f 
		+ dabs(tau) * dw;

/*        Test for convergence */

	if (dabs(w) <= eps * erretm) {
	    if (orgati) {
		*dlam = d__[*i__] + tau;
	    } else {
		*dlam = d__[ip1] + tau;
	    }
	    goto L250;
	}

	if (w <= 0.f) {
	    dltlb = df2cmax(dltlb,tau);
	} else {
	    dltub = df2cmin(dltub,tau);
	}

/*        Calculate the new step */

	++niter;
	if (! swtch3) {
	    if (orgati) {
/* Computing 2nd power */
		r__1 = z__[*i__] / delta[*i__];
		c__ = w - delta[ip1] * dw - (d__[*i__] - d__[ip1]) * (r__1 * 
			r__1);
	    } else {
/* Computing 2nd power */
		r__1 = z__[ip1] / delta[ip1];
		c__ = w - delta[*i__] * dw - (d__[ip1] - d__[*i__]) * (r__1 * 
			r__1);
	    }
	    a = (delta[*i__] + delta[ip1]) * w - delta[*i__] * delta[ip1] * 
		    dw;
	    b = delta[*i__] * delta[ip1] * w;
	    if (c__ == 0.f) {
		if (a == 0.f) {
		    if (orgati) {
			a = z__[*i__] * z__[*i__] + delta[ip1] * delta[ip1] * 
				(dpsi + dphi);
		    } else {
			a = z__[ip1] * z__[ip1] + delta[*i__] * delta[*i__] * 
				(dpsi + dphi);
		    }
		}
		eta = b / a;
	    } else if (a <= 0.f) {
		eta = (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1)))) /
			 (c__ * 2.f);
	    } else {
		eta = b * 2.f / (a + sqrt((r__1 = a * a - b * 4.f * c__, dabs(
			r__1))));
	    }
	} else {

/*           Interpolation using THREE most relevant poles */

	    temp = rhoinv + psi + phi;
	    if (orgati) {
		temp1 = z__[iim1] / delta[iim1];
		temp1 *= temp1;
		c__ = temp - delta[iip1] * (dpsi + dphi) - (d__[iim1] - d__[
			iip1]) * temp1;
		zz[0] = z__[iim1] * z__[iim1];
		zz[2] = delta[iip1] * delta[iip1] * (dpsi - temp1 + dphi);
	    } else {
		temp1 = z__[iip1] / delta[iip1];
		temp1 *= temp1;
		c__ = temp - delta[iim1] * (dpsi + dphi) - (d__[iip1] - d__[
			iim1]) * temp1;
		zz[0] = delta[iim1] * delta[iim1] * (dpsi + (dphi - temp1));
		zz[2] = z__[iip1] * z__[iip1];
	    }
	    zz[1] = z__[ii] * z__[ii];
	    slaed6_(&niter, &orgati, &c__, &delta[iim1], zz, &w, &eta, info);
	    if (*info != 0) {
		goto L250;
	    }
	}

/*        Note, eta should be positive if w is negative, and   
          eta should be negative otherwise. However,   
          if for some reason caused by roundoff, eta*w > 0,   
          we simply use one Newton step instead. This way   
          will guarantee eta*w < 0. */

	if (w * eta >= 0.f) {
	    eta = -w / dw;
	}
	temp = tau + eta;
	if (temp > dltub || temp < dltlb) {
	    if (w < 0.f) {
		eta = (dltub - tau) / 2.f;
	    } else {
		eta = (dltlb - tau) / 2.f;
	    }
	}

	prew = w;

/* L170: */
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
/* L180: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.f;
	psi = 0.f;
	erretm = 0.f;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L190: */
	}
	erretm = dabs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.f;
	phi = 0.f;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / delta[j];
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L200: */
	}

	temp = z__[ii] / delta[ii];
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w = rhoinv + phi + psi + temp;
	erretm = (phi - psi) * 8.f + erretm + rhoinv * 2.f + dabs(temp) * 3.f 
		+ (r__1 = tau + eta, dabs(r__1)) * dw;

	swtch = FALSE_;
	if (orgati) {
	    if (-w > dabs(prew) / 10.f) {
		swtch = TRUE_;
	    }
	} else {
	    if (w > dabs(prew) / 10.f) {
		swtch = TRUE_;
	    }
	}

	tau += eta;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 30; ++niter) {

/*           Test for convergence */

	    if (dabs(w) <= eps * erretm) {
		if (orgati) {
		    *dlam = d__[*i__] + tau;
		} else {
		    *dlam = d__[ip1] + tau;
		}
		goto L250;
	    }

	    if (w <= 0.f) {
		dltlb = df2cmax(dltlb,tau);
	    } else {
		dltub = df2cmin(dltub,tau);
	    }

/*           Calculate the new step */

	    if (! swtch3) {
		if (! swtch) {
		    if (orgati) {
/* Computing 2nd power */
			r__1 = z__[*i__] / delta[*i__];
			c__ = w - delta[ip1] * dw - (d__[*i__] - d__[ip1]) * (
				r__1 * r__1);
		    } else {
/* Computing 2nd power */
			r__1 = z__[ip1] / delta[ip1];
			c__ = w - delta[*i__] * dw - (d__[ip1] - d__[*i__]) * 
				(r__1 * r__1);
		    }
		} else {
		    temp = z__[ii] / delta[ii];
		    if (orgati) {
			dpsi += temp * temp;
		    } else {
			dphi += temp * temp;
		    }
		    c__ = w - delta[*i__] * dpsi - delta[ip1] * dphi;
		}
		a = (delta[*i__] + delta[ip1]) * w - delta[*i__] * delta[ip1] 
			* dw;
		b = delta[*i__] * delta[ip1] * w;
		if (c__ == 0.f) {
		    if (a == 0.f) {
			if (! swtch) {
			    if (orgati) {
				a = z__[*i__] * z__[*i__] + delta[ip1] * 
					delta[ip1] * (dpsi + dphi);
			    } else {
				a = z__[ip1] * z__[ip1] + delta[*i__] * delta[
					*i__] * (dpsi + dphi);
			    }
			} else {
			    a = delta[*i__] * delta[*i__] * dpsi + delta[ip1] 
				    * delta[ip1] * dphi;
			}
		    }
		    eta = b / a;
		} else if (a <= 0.f) {
		    eta = (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1))
			    )) / (c__ * 2.f);
		} else {
		    eta = b * 2.f / (a + sqrt((r__1 = a * a - b * 4.f * c__, 
			    dabs(r__1))));
		}
	    } else {

/*              Interpolation using THREE most relevant poles */

		temp = rhoinv + psi + phi;
		if (swtch) {
		    c__ = temp - delta[iim1] * dpsi - delta[iip1] * dphi;
		    zz[0] = delta[iim1] * delta[iim1] * dpsi;
		    zz[2] = delta[iip1] * delta[iip1] * dphi;
		} else {
		    if (orgati) {
			temp1 = z__[iim1] / delta[iim1];
			temp1 *= temp1;
			c__ = temp - delta[iip1] * (dpsi + dphi) - (d__[iim1] 
				- d__[iip1]) * temp1;
			zz[0] = z__[iim1] * z__[iim1];
			zz[2] = delta[iip1] * delta[iip1] * (dpsi - temp1 + 
				dphi);
		    } else {
			temp1 = z__[iip1] / delta[iip1];
			temp1 *= temp1;
			c__ = temp - delta[iim1] * (dpsi + dphi) - (d__[iip1] 
				- d__[iim1]) * temp1;
			zz[0] = delta[iim1] * delta[iim1] * (dpsi + (dphi - 
				temp1));
			zz[2] = z__[iip1] * z__[iip1];
		    }
		}
		slaed6_(&niter, &orgati, &c__, &delta[iim1], zz, &w, &eta, 
			info);
		if (*info != 0) {
		    goto L250;
		}
	    }

/*           Note, eta should be positive if w is negative, and   
             eta should be negative otherwise. However,   
             if for some reason caused by roundoff, eta*w > 0,   
             we simply use one Newton step instead. This way   
             will guarantee eta*w < 0. */

	    if (w * eta >= 0.f) {
		eta = -w / dw;
	    }
	    temp = tau + eta;
	    if (temp > dltub || temp < dltlb) {
		if (w < 0.f) {
		    eta = (dltub - tau) / 2.f;
		} else {
		    eta = (dltlb - tau) / 2.f;
		}
	    }

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
/* L210: */
	    }

	    tau += eta;
	    prew = w;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.f;
	    psi = 0.f;
	    erretm = 0.f;
	    i__1 = iim1;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / delta[j];
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L220: */
	    }
	    erretm = dabs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    dphi = 0.f;
	    phi = 0.f;
	    i__1 = iip1;
	    for (j = *n; j >= i__1; --j) {
		temp = z__[j] / delta[j];
		phi += z__[j] * temp;
		dphi += temp * temp;
		erretm += phi;
/* L230: */
	    }

	    temp = z__[ii] / delta[ii];
	    dw = dpsi + dphi + temp * temp;
	    temp = z__[ii] * temp;
	    w = rhoinv + phi + psi + temp;
	    erretm = (phi - psi) * 8.f + erretm + rhoinv * 2.f + dabs(temp) * 
		    3.f + dabs(tau) * dw;
	    if (w * prew > 0.f && dabs(w) > dabs(prew) / 10.f) {
		swtch = ! swtch;
	    }

/* L240: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	if (orgati) {
	    *dlam = d__[*i__] + tau;
	} else {
	    *dlam = d__[ip1] + tau;
	}

    }

L250:

    return 0;

/*     End of SLAED4 */

} /* slaed4_ */

/* Subroutine */ int slaeda_(integer *n, integer *tlvls, integer *curlvl, 
	integer *curpbm, integer *prmptr, integer *perm, integer *givptr, 
	integer *givcol, real *givnum, real *q, integer *qptr, real *z__, 
	real *ztemp, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLAEDA computes the Z vector corresponding to the merge step in the   
    CURLVLth step of the merge process with TLVLS steps for the CURPBMth   
    problem.   

    Arguments   
    =========   

    N      (input) INTEGER   
           The dimension of the symmetric tridiagonal matrix.  N >= 0.   

    TLVLS  (input) INTEGER   
           The total number of merging levels in the overall divide and   
           conquer tree.   

    CURLVL (input) INTEGER   
           The current level in the overall merge routine,   
           0 <= curlvl <= tlvls.   

    CURPBM (input) INTEGER   
           The current problem in the current level in the overall   
           merge routine (counting from upper left to lower right).   

    PRMPTR (input) INTEGER array, dimension (N lg N)   
           Contains a list of pointers which indicate where in PERM a   
           level's permutation is stored.  PRMPTR(i+1) - PRMPTR(i)   
           indicates the size of the permutation and incidentally the   
           size of the full, non-deflated problem.   

    PERM   (input) INTEGER array, dimension (N lg N)   
           Contains the permutations (from deflation and sorting) to be   
           applied to each eigenblock.   

    GIVPTR (input) INTEGER array, dimension (N lg N)   
           Contains a list of pointers which indicate where in GIVCOL a   
           level's Givens rotations are stored.  GIVPTR(i+1) - GIVPTR(i)   
           indicates the number of Givens rotations.   

    GIVCOL (input) INTEGER array, dimension (2, N lg N)   
           Each pair of numbers indicates a pair of columns to take place   
           in a Givens rotation.   

    GIVNUM (input) REAL array, dimension (2, N lg N)   
           Each number indicates the S value to be used in the   
           corresponding Givens rotation.   

    Q      (input) REAL array, dimension (N**2)   
           Contains the square eigenblocks from previous levels, the   
           starting positions for blocks are given by QPTR.   

    QPTR   (input) INTEGER array, dimension (N+2)   
           Contains a list of pointers which indicate where in Q an   
           eigenblock is stored.  SQRT( QPTR(i+1) - QPTR(i) ) indicates   
           the size of the block.   

    Z      (output) REAL array, dimension (N)   
           On output this vector contains the updating vector (the last   
           row of the first sub-eigenvector matrix and the first row of   
           the second sub-eigenvector matrix).   

    ZTEMP  (workspace) REAL array, dimension (N)   

    INFO   (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__2 = 2;
    static integer c__1 = 1;
    static real c_b24 = 1.f;
    static real c_b26 = 0.f;
    
    /* System generated locals */
    integer i__1, i__2, i__3;
    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    // double sqrt(doublereal);
    /* Local variables */
    static integer curr;
    extern /* Subroutine */ int srot_(integer *, real *, integer *, real *, 
	    integer *, real *, real *);
    static integer bsiz1, bsiz2, psiz1, psiz2, i__, k, zptr1;
    extern /* Subroutine */ int sgemv_(const char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *), scopy_(integer *, real *, integer *, real *, integer *), 
	    xerbla_(const char *, integer *);
    static integer mid, ptr;
#define givcol_ref(a_1,a_2) givcol[(a_2)*2 + a_1]
#define givnum_ref(a_1,a_2) givnum[(a_2)*2 + a_1]


    --ztemp;
    --z__;
    --qptr;
    --q;
    givnum -= 3;
    givcol -= 3;
    --givptr;
    --perm;
    --prmptr;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAEDA", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine location of first number in second half. */

    mid = *n / 2 + 1;

/*     Gather last/first rows of appropriate eigenblocks into center of Z */

    ptr = 1;

/*     Determine location of lowest level subproblem in the full storage   
       scheme */

    i__1 = *curlvl - 1;
    curr = ptr + *curpbm * pow_ii(&c__2, curlvl) + pow_ii(&c__2, &i__1) - 1;

/*     Determine size of these matrices.  We add HALF to the value of   
       the SQRT in case the machine underestimates one of these square   
       roots. */

    bsiz1 = (integer) (sqrt((real) (qptr[curr + 1] - qptr[curr])) + .5f);
    bsiz2 = (integer) (sqrt((real) (qptr[curr + 2] - qptr[curr + 1])) + .5f);
    i__1 = mid - bsiz1 - 1;
    for (k = 1; k <= i__1; ++k) {
	z__[k] = 0.f;
/* L10: */
    }
    scopy_(&bsiz1, &q[qptr[curr] + bsiz1 - 1], &bsiz1, &z__[mid - bsiz1], &
	    c__1);
    scopy_(&bsiz2, &q[qptr[curr + 1]], &bsiz2, &z__[mid], &c__1);
    i__1 = *n;
    for (k = mid + bsiz2; k <= i__1; ++k) {
	z__[k] = 0.f;
/* L20: */
    }

/*     Loop thru remaining levels 1 -> CURLVL applying the Givens   
       rotations and permutation and then multiplying the center matrices   
       against the current Z. */

    ptr = pow_ii(&c__2, tlvls) + 1;
    i__1 = *curlvl - 1;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *curlvl - k;
	i__3 = *curlvl - k - 1;
	curr = ptr + *curpbm * pow_ii(&c__2, &i__2) + pow_ii(&c__2, &i__3) - 
		1;
	psiz1 = prmptr[curr + 1] - prmptr[curr];
	psiz2 = prmptr[curr + 2] - prmptr[curr + 1];
	zptr1 = mid - psiz1;

/*       Apply Givens at CURR and CURR+1 */

	i__2 = givptr[curr + 1] - 1;
	for (i__ = givptr[curr]; i__ <= i__2; ++i__) {
	    srot_(&c__1, &z__[zptr1 + givcol_ref(1, i__) - 1], &c__1, &z__[
		    zptr1 + givcol_ref(2, i__) - 1], &c__1, &givnum_ref(1, 
		    i__), &givnum_ref(2, i__));
/* L30: */
	}
	i__2 = givptr[curr + 2] - 1;
	for (i__ = givptr[curr + 1]; i__ <= i__2; ++i__) {
	    srot_(&c__1, &z__[mid - 1 + givcol_ref(1, i__)], &c__1, &z__[mid 
		    - 1 + givcol_ref(2, i__)], &c__1, &givnum_ref(1, i__), &
		    givnum_ref(2, i__));
/* L40: */
	}
	psiz1 = prmptr[curr + 1] - prmptr[curr];
	psiz2 = prmptr[curr + 2] - prmptr[curr + 1];
	i__2 = psiz1 - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    ztemp[i__ + 1] = z__[zptr1 + perm[prmptr[curr] + i__] - 1];
/* L50: */
	}
	i__2 = psiz2 - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    ztemp[psiz1 + i__ + 1] = z__[mid + perm[prmptr[curr + 1] + i__] - 
		    1];
/* L60: */
	}

/*        Multiply Blocks at CURR and CURR+1   

          Determine size of these matrices.  We add HALF to the value of   
          the SQRT in case the machine underestimates one of these   
          square roots. */

	bsiz1 = (integer) (sqrt((real) (qptr[curr + 1] - qptr[curr])) + .5f);
	bsiz2 = (integer) (sqrt((real) (qptr[curr + 2] - qptr[curr + 1])) + 
		.5f);
	if (bsiz1 > 0) {
	    sgemv_("T", &bsiz1, &bsiz1, &c_b24, &q[qptr[curr]], &bsiz1, &
		    ztemp[1], &c__1, &c_b26, &z__[zptr1], &c__1);
	}
	i__2 = psiz1 - bsiz1;
	scopy_(&i__2, &ztemp[bsiz1 + 1], &c__1, &z__[zptr1 + bsiz1], &c__1);
	if (bsiz2 > 0) {
	    sgemv_("T", &bsiz2, &bsiz2, &c_b24, &q[qptr[curr + 1]], &bsiz2, &
		    ztemp[psiz1 + 1], &c__1, &c_b26, &z__[mid], &c__1);
	}
	i__2 = psiz2 - bsiz2;
	scopy_(&i__2, &ztemp[psiz1 + bsiz2 + 1], &c__1, &z__[mid + bsiz2], &
		c__1);

	i__2 = *tlvls - k;
	ptr += pow_ii(&c__2, &i__2);
/* L70: */
    }

    return 0;

/*     End of SLAEDA */

} /* slaeda_ */

#undef givnum_ref
#undef givcol_ref


/* Subroutine */ int slaed3_(integer *k, integer *n, integer *n1, real *d__, 
	real *q, integer *ldq, real *rho, real *dlamda, real *q2, integer *
	indx, integer *ctot, real *w, real *s, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,   
       Courant Institute, NAG Ltd., and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SLAED3 finds the roots of the secular equation, as defined by the   
    values in D, W, and RHO, between 1 and K.  It makes the   
    appropriate calls to SLAED4 and then updates the eigenvectors by   
    multiplying the matrix of eigenvectors of the pair of eigensystems   
    being combined by the matrix of eigenvectors of the K-by-K system   
    which is solved here.   

    This code makes very mild assumptions about floating point   
    arithmetic. It will work on machines with a guard digit in   
    add/subtract, or on those binary machines without guard digits   
    which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2.   
    It could conceivably fail on hexadecimal or decimal machines   
    without guard digits, but we know of none.   

    Arguments   
    =========   

    K       (input) INTEGER   
            The number of terms in the rational function to be solved by   
            SLAED4.  K >= 0.   

    N       (input) INTEGER   
            The number of rows and columns in the Q matrix.   
            N >= K (deflation may result in N>K).   

    N1      (input) INTEGER   
            The location of the last eigenvalue in the leading submatrix.   
            min(1,N) <= N1 <= N/2.   

    D       (output) REAL array, dimension (N)   
            D(I) contains the updated eigenvalues for   
            1 <= I <= K.   

    Q       (output) REAL array, dimension (LDQ,N)   
            Initially the first K columns are used as workspace.   
            On output the columns 1 to K contain   
            the updated eigenvectors.   

    LDQ     (input) INTEGER   
            The leading dimension of the array Q.  LDQ >= max(1,N).   

    RHO     (input) REAL   
            The value of the parameter in the rank one update equation.   
            RHO >= 0 required.   

    DLAMDA  (input/output) REAL array, dimension (K)   
            The first K elements of this array contain the old roots   
            of the deflated updating problem.  These are the poles   
            of the secular equation. May be changed on output by   
            having lowest order bit set to zero on Cray X-MP, Cray Y-MP,   
            Cray-2, or Cray C-90, as described above.   

    Q2      (input) REAL array, dimension (LDQ2, N)   
            The first K columns of this matrix contain the non-deflated   
            eigenvectors for the split problem.   

    INDX    (input) INTEGER array, dimension (N)   
            The permutation used to arrange the columns of the deflated   
            Q matrix into three groups (see SLAED2).   
            The rows of the eigenvectors found by SLAED4 must be likewise   
            permuted before the matrix multiply can take place.   

    CTOT    (input) INTEGER array, dimension (4)   
            A count of the total number of the various types of columns   
            in Q, as described in INDX.  The fourth column type is any   
            column which has been deflated.   

    W       (input/output) REAL array, dimension (K)   
            The first K elements of this array contain the components   
            of the deflation-adjusted updating vector. Destroyed on   
            output.   

    S       (workspace) REAL array, dimension (N1 + 1)*K   
            Will contain the eigenvectors of the repaired matrix which   
            will be multiplied by the previously accumulated eigenvectors   
            to update the system.   

    LDS     (input) INTEGER   
            The leading dimension of S.  LDS >= max(1,K).   

    INFO    (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  if INFO = 1, an eigenvalue did not converge   

    Further Details   
    ===============   

    Based on contributions by   
       Jeff Rutter, Computer Science Division, University of California   
       at Berkeley, USA   
    Modified by Francoise Tisseur, University of Tennessee.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static real c_b22 = 1.f;
    static real c_b23 = 0.f;
    
    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    real r__1;
    /* Builtin functions */
    // double sqrt(doublereal), r_sign(real *, real *);
    /* Local variables */
    static real temp;
    extern doublereal snrm2_(integer *, real *, integer *);
    static integer i__, j;
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *), scopy_(integer *, real *, 
	    integer *, real *, integer *);
    static integer n2;
    extern /* Subroutine */ int slaed4_(integer *, integer *, real *, real *, 
	    real *, real *, real *, integer *);
    extern doublereal slamc3_(real *, real *);
    static integer n12, ii, n23;
    extern /* Subroutine */ int xerbla_(const char *, integer *), slacpy_(
	    const char *, integer *, integer *, real *, integer *, real *, integer *
	    ), slaset_(const char *, integer *, integer *, real *, real *, 
	    real *, integer *);
    static integer iq2;
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]


    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    --dlamda;
    --q2;
    --indx;
    --ctot;
    --w;
    --s;

    /* Function Body */
    *info = 0;

    if (*k < 0) {
	*info = -1;
    } else if (*n < *k) {
	*info = -2;
    } else if (*ldq < f2cmax(1,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAED3", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 0) {
	return 0;
    }

/*     Modify values DLAMDA(i) to make sure all DLAMDA(i)-DLAMDA(j) can   
       be computed with high relative accuracy (barring over/underflow).   
       This is a problem on machines without a guard digit in   
       add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2).   
       The following code replaces DLAMDA(I) by 2*DLAMDA(I)-DLAMDA(I),   
       which on any of these machines zeros out the bottommost   
       bit of DLAMDA(I) if it is 1; this makes the subsequent   
       subtractions DLAMDA(I)-DLAMDA(J) unproblematic when cancellation   
       occurs. On binary machines with a guard digit (almost all   
       machines) it does not change DLAMDA(I) at all. On hexadecimal   
       and decimal machines with a guard digit, it slightly   
       changes the bottommost bits of DLAMDA(I). It does not account   
       for hexadecimal or decimal machines without guard digits   
       (we know of none). We use a subroutine call to compute   
       2*DLAMBDA(I) to prevent optimizing compilers from eliminating   
       this code. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = slamc3_(&dlamda[i__], &dlamda[i__]) - dlamda[i__];
/* L10: */
    }

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	slaed4_(k, &j, &dlamda[1], &w[1], &q_ref(1, j), rho, &d__[j], info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    goto L120;
	}
/* L20: */
    }

    if (*k == 1) {
	goto L110;
    }
    if (*k == 2) {
	i__1 = *k;
	for (j = 1; j <= i__1; ++j) {
	    w[1] = q_ref(1, j);
	    w[2] = q_ref(2, j);
	    ii = indx[1];
	    q_ref(1, j) = w[ii];
	    ii = indx[2];
	    q_ref(2, j) = w[ii];
/* L30: */
	}
	goto L110;
    }

/*     Compute updated W. */

    scopy_(k, &w[1], &c__1, &s[1], &c__1);

/*     Initialize W(I) = Q(I,I) */

    i__1 = *ldq + 1;
    scopy_(k, &q[q_offset], &i__1, &w[1], &c__1);
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[i__] *= q_ref(i__, j) / (dlamda[i__] - dlamda[j]);
/* L40: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    w[i__] *= q_ref(i__, j) / (dlamda[i__] - dlamda[j]);
/* L50: */
	}
/* L60: */
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__1 = sqrt(-w[i__]);
	w[i__] = r_sign(&r__1, &s[i__]);
/* L70: */
    }

/*     Compute eigenvectors of the modified rank-1 modification. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    s[i__] = w[i__] / q_ref(i__, j);
/* L80: */
	}
	temp = snrm2_(k, &s[1], &c__1);
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    ii = indx[i__];
	    q_ref(i__, j) = s[ii] / temp;
/* L90: */
	}
/* L100: */
    }

/*     Compute the updated eigenvectors. */

L110:

    n2 = *n - *n1;
    n12 = ctot[1] + ctot[2];
    n23 = ctot[2] + ctot[3];

    slacpy_("A", &n23, k, &q_ref(ctot[1] + 1, 1), ldq, &s[1], &n23)
	    ;
    iq2 = *n1 * n12 + 1;
    if (n23 != 0) {
	sgemm_("N", "N", &n2, k, &n23, &c_b22, &q2[iq2], &n2, &s[1], &n23, &
		c_b23, &q_ref(*n1 + 1, 1), ldq);
    } else {
	slaset_("A", &n2, k, &c_b23, &c_b23, &q_ref(*n1 + 1, 1), ldq);
    }

    slacpy_("A", &n12, k, &q[q_offset], ldq, &s[1], &n12);
    if (n12 != 0) {
	sgemm_("N", "N", n1, k, &n12, &c_b22, &q2[1], n1, &s[1], &n12, &c_b23,
		 &q[q_offset], ldq);
    } else {
	slaset_("A", n1, k, &c_b23, &c_b23, &q_ref(1, 1), ldq);
    }


L120:
    return 0;

/*     End of SLAED3 */

} /* slaed3_ */

#undef q_ref


/* Subroutine */ int slaed6_(integer *kniter, logical *orgati, real *rho, 
	real *d__, real *z__, real *finit, real *tau, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,   
       Courant Institute, NAG Ltd., and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SLAED6 computes the positive or negative root (closest to the origin)   
    of   
                     z(1)        z(2)        z(3)   
    f(x) =   rho + --------- + ---------- + ---------   
                    d(1)-x      d(2)-x      d(3)-x   

    It is assumed that   

          if ORGATI = .true. the root is between d(2) and d(3);   
          otherwise it is between d(1) and d(2)   

    This routine will be called by SLAED4 when necessary. In most cases,   
    the root sought is the smallest in magnitude, though it might not be   
    in some extremely rare situations.   

    Arguments   
    =========   

    KNITER       (input) INTEGER   
                 Refer to SLAED4 for its significance.   

    ORGATI       (input) LOGICAL   
                 If ORGATI is true, the needed root is between d(2) and   
                 d(3); otherwise it is between d(1) and d(2).  See   
                 SLAED4 for further details.   

    RHO          (input) REAL   
                 Refer to the equation f(x) above.   

    D            (input) REAL array, dimension (3)   
                 D satisfies d(1) < d(2) < d(3).   

    Z            (input) REAL array, dimension (3)   
                 Each of the elements in z must be positive.   

    FINIT        (input) REAL   
                 The value of f at 0. It is more accurate than the one   
                 evaluated inside this routine (if someone wants to do   
                 so).   

    TAU          (output) REAL   
                 The root of the equation f(x).   

    INFO         (output) INTEGER   
                 = 0: successful exit   
                 > 0: if INFO = 1, failure to converge   

    Further Details   
    ===============   

    Based on contributions by   
       Ren-Cang Li, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   

       Parameter adjustments */
    /* Initialized data */
    static logical first = TRUE_;
    /* System generated locals */
    integer i__1;
    real r__1, r__2, r__3, r__4;
    /* Builtin functions */
    // double sqrt(doublereal), log(doublereal), pow_ri(real *, integer *);
    /* Local variables */
    static real base;
    static integer iter;
    static real temp, temp1, temp2, temp3, temp4, a, b, c__, f;
    static integer i__;
    static logical scale;
    static integer niter;
    static real small1, small2, fc, df, sminv1, sminv2, dscale[3], sclfac;
    extern doublereal slamch_(const char *);
    static real zscale[3], erretm, sclinv, ddf, eta, eps;

    --z__;
    --d__;

    /* Function Body */

    *info = 0;

    niter = 1;
    *tau = 0.f;
    if (*kniter == 2) {
	if (*orgati) {
	    temp = (d__[3] - d__[2]) / 2.f;
	    c__ = *rho + z__[1] / (d__[1] - d__[2] - temp);
	    a = c__ * (d__[2] + d__[3]) + z__[2] + z__[3];
	    b = c__ * d__[2] * d__[3] + z__[2] * d__[3] + z__[3] * d__[2];
	} else {
	    temp = (d__[1] - d__[2]) / 2.f;
	    c__ = *rho + z__[3] / (d__[3] - d__[2] - temp);
	    a = c__ * (d__[1] + d__[2]) + z__[1] + z__[2];
	    b = c__ * d__[1] * d__[2] + z__[1] * d__[2] + z__[2] * d__[1];
	}
/* Computing MAX */
	r__1 = dabs(a), r__2 = dabs(b), r__1 = f2cmax(r__1,r__2), r__2 = dabs(
		c__);
	temp = df2cmax(r__1,r__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.f) {
	    *tau = b / a;
	} else if (a <= 0.f) {
	    *tau = (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1)))) / (
		    c__ * 2.f);
	} else {
	    *tau = b * 2.f / (a + sqrt((r__1 = a * a - b * 4.f * c__, dabs(
		    r__1))));
	}
	temp = *rho + z__[1] / (d__[1] - *tau) + z__[2] / (d__[2] - *tau) + 
		z__[3] / (d__[3] - *tau);
	if (dabs(*finit) <= dabs(temp)) {
	    *tau = 0.f;
	}
    }

/*     On first call to routine, get machine parameters for   
       possible scaling to avoid overflow */

    if (first) {
	eps = slamch_("Epsilon");
	base = slamch_("Base");
	i__1 = (integer) (log(slamch_("SafMin")) / log(base) / 3.f)
		;
	small1 = pow_ri(&base, &i__1);
	sminv1 = 1.f / small1;
	small2 = small1 * small1;
	sminv2 = sminv1 * sminv1;
	first = FALSE_;
    }

/*     Determine if scaling of inputs necessary to avoid overflow   
       when computing 1/TEMP**3 */

    if (*orgati) {
/* Computing MIN */
	r__3 = (r__1 = d__[2] - *tau, dabs(r__1)), r__4 = (r__2 = d__[3] - *
		tau, dabs(r__2));
	temp = df2cmin(r__3,r__4);
    } else {
/* Computing MIN */
	r__3 = (r__1 = d__[1] - *tau, dabs(r__1)), r__4 = (r__2 = d__[2] - *
		tau, dabs(r__2));
	temp = df2cmin(r__3,r__4);
    }
    scale = FALSE_;
    if (temp <= small1) {
	scale = TRUE_;
	if (temp <= small2) {

/*        Scale up by power of radix nearest 1/SAFMIN**(2/3) */

	    sclfac = sminv2;
	    sclinv = small2;
	} else {

/*        Scale up by power of radix nearest 1/SAFMIN**(1/3) */

	    sclfac = sminv1;
	    sclinv = small1;
	}

/*        Scaling up safe because D, Z, TAU scaled elsewhere to be O(1) */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__] * sclfac;
	    zscale[i__ - 1] = z__[i__] * sclfac;
/* L10: */
	}
	*tau *= sclfac;
    } else {

/*        Copy D and Z to DSCALE and ZSCALE */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__];
	    zscale[i__ - 1] = z__[i__];
/* L20: */
	}
    }

    fc = 0.f;
    df = 0.f;
    ddf = 0.f;
    for (i__ = 1; i__ <= 3; ++i__) {
	temp = 1.f / (dscale[i__ - 1] - *tau);
	temp1 = zscale[i__ - 1] * temp;
	temp2 = temp1 * temp;
	temp3 = temp2 * temp;
	fc += temp1 / dscale[i__ - 1];
	df += temp2;
	ddf += temp3;
/* L30: */
    }
    f = *finit + *tau * fc;

    if (dabs(f) <= 0.f) {
	goto L60;
    }

/*        Iteration begins   

       It is not hard to see that   

             1) Iterations will go up monotonically   
                if FINIT < 0;   

             2) Iterations will go down monotonically   
                if FINIT > 0. */

    iter = niter + 1;

    for (niter = iter; niter <= 20; ++niter) {

	if (*orgati) {
	    temp1 = dscale[1] - *tau;
	    temp2 = dscale[2] - *tau;
	} else {
	    temp1 = dscale[0] - *tau;
	    temp2 = dscale[1] - *tau;
	}
	a = (temp1 + temp2) * f - temp1 * temp2 * df;
	b = temp1 * temp2 * f;
	c__ = f - (temp1 + temp2) * df + temp1 * temp2 * ddf;
/* Computing MAX */
	r__1 = dabs(a), r__2 = dabs(b), r__1 = f2cmax(r__1,r__2), r__2 = dabs(
		c__);
	temp = df2cmax(r__1,r__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.f) {
	    eta = b / a;
	} else if (a <= 0.f) {
	    eta = (a - sqrt((r__1 = a * a - b * 4.f * c__, dabs(r__1)))) / (
		    c__ * 2.f);
	} else {
	    eta = b * 2.f / (a + sqrt((r__1 = a * a - b * 4.f * c__, dabs(
		    r__1))));
	}
	if (f * eta >= 0.f) {
	    eta = -f / df;
	}

	temp = eta + *tau;
	if (*orgati) {
	    if (eta > 0.f && temp >= dscale[2]) {
		eta = (dscale[2] - *tau) / 2.f;
	    }
	    if (eta < 0.f && temp <= dscale[1]) {
		eta = (dscale[1] - *tau) / 2.f;
	    }
	} else {
	    if (eta > 0.f && temp >= dscale[1]) {
		eta = (dscale[1] - *tau) / 2.f;
	    }
	    if (eta < 0.f && temp <= dscale[0]) {
		eta = (dscale[0] - *tau) / 2.f;
	    }
	}
	*tau += eta;

	fc = 0.f;
	erretm = 0.f;
	df = 0.f;
	ddf = 0.f;
	for (i__ = 1; i__ <= 3; ++i__) {
	    temp = 1.f / (dscale[i__ - 1] - *tau);
	    temp1 = zscale[i__ - 1] * temp;
	    temp2 = temp1 * temp;
	    temp3 = temp2 * temp;
	    temp4 = temp1 / dscale[i__ - 1];
	    fc += temp4;
	    erretm += dabs(temp4);
	    df += temp2;
	    ddf += temp3;
/* L40: */
	}
	f = *finit + *tau * fc;
	erretm = (dabs(*finit) + dabs(*tau) * erretm) * 8.f + dabs(*tau) * df;
	if (dabs(f) <= eps * erretm) {
	    goto L60;
	}
/* L50: */
    }
    *info = 1;
L60:

/*     Undo scaling */

    if (scale) {
	*tau *= sclinv;
    }
    return 0;

/*     End of SLAED6 */

} /* slaed6_ */

/* Subroutine */ int slaed5_(integer *i__, real *d__, real *z__, real *delta, 
	real *rho, real *dlam)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,   
       Courant Institute, NAG Ltd., and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    This subroutine computes the I-th eigenvalue of a symmetric rank-one   
    modification of a 2-by-2 diagonal matrix   

               diag( D )  +  RHO *  Z * transpose(Z) .   

    The diagonal elements in the array D are assumed to satisfy   

               D(i) < D(j)  for  i < j .   

    We also assume RHO > 0 and that the Euclidean norm of the vector   
    Z is one.   

    Arguments   
    =========   

    I      (input) INTEGER   
           The index of the eigenvalue to be computed.  I = 1 or I = 2.   

    D      (input) REAL array, dimension (2)   
           The original eigenvalues.  We assume D(1) < D(2).   

    Z      (input) REAL array, dimension (2)   
           The components of the updating vector.   

    DELTA  (output) REAL array, dimension (2)   
           The vector DELTA contains the information necessary   
           to construct the eigenvectors.   

    RHO    (input) REAL   
           The scalar in the symmetric updating formula.   

    DLAM   (output) REAL   
           The computed lambda_I, the I-th updated eigenvalue.   

    Further Details   
    ===============   

    Based on contributions by   
       Ren-Cang Li, Computer Science Division, University of California   
       at Berkeley, USA   

    =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    real r__1;
    /* Builtin functions */
    // double sqrt(doublereal);
    /* Local variables */
    static real temp, b, c__, w, del, tau;

    --delta;
    --z__;
    --d__;

    /* Function Body */
    del = d__[2] - d__[1];
    if (*i__ == 1) {
	w = *rho * 2.f * (z__[2] * z__[2] - z__[1] * z__[1]) / del + 1.f;
	if (w > 0.f) {
	    b = del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[1] * z__[1] * del;

/*           B > ZERO, always */

	    tau = c__ * 2.f / (b + sqrt((r__1 = b * b - c__ * 4.f, dabs(r__1))
		    ));
	    *dlam = d__[1] + tau;
	    delta[1] = -z__[1] / tau;
	    delta[2] = z__[2] / (del - tau);
	} else {
	    b = -del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[2] * z__[2] * del;
	    if (b > 0.f) {
		tau = c__ * -2.f / (b + sqrt(b * b + c__ * 4.f));
	    } else {
		tau = (b - sqrt(b * b + c__ * 4.f)) / 2.f;
	    }
	    *dlam = d__[2] + tau;
	    delta[1] = -z__[1] / (del + tau);
	    delta[2] = -z__[2] / tau;
	}
	temp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	delta[1] /= temp;
	delta[2] /= temp;
    } else {

/*     Now I=2 */

	b = -del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	c__ = *rho * z__[2] * z__[2] * del;
	if (b > 0.f) {
	    tau = (b + sqrt(b * b + c__ * 4.f)) / 2.f;
	} else {
	    tau = c__ * 2.f / (-b + sqrt(b * b + c__ * 4.f));
	}
	*dlam = d__[2] + tau;
	delta[1] = -z__[1] / (del + tau);
	delta[2] = -z__[2] / tau;
	temp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	delta[1] /= temp;
	delta[2] /= temp;
    }
    return 0;

/*     End OF SLAED5 */

} /* slaed5_ */


//==============================================================================
/* Table of constant values */

static integer c__6 = 6;
static integer c__0 = 0;
static integer c__2 = 2;
//static integer c__1 = 1;
static integer c_n1 = -1;
static real c_b416 = 0.f;
static real c_b438 = 1.f;


/* Subroutine */ int sgesvd_(char *jobu, char *jobvt, integer *m, integer *n, 
	real *a, integer *lda, real *s, real *u, integer *ldu, real *vt, 
	integer *ldvt, real *work, integer *lwork, integer *info)
{
    /* System generated locals */
    typedef const char *address;

    address a__1[2];
    integer a_dim1, a_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1[2], 
	    i__2, i__3, i__4;
    char ch__1[2];

    /* Builtin functions   
       Subroutine */ int s_cat(char *, const char **, integer *, integer *, ftnlen);
    //double sqrt(doublereal);

    /* Local variables */
    static integer iscl;
    static real anrm;
    static integer ierr, itau, ncvt, nrvt, i__;
    extern logical lsame_(const char *, const char *);
    static integer chunk;
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    static integer minmn, wrkbl, itaup, itauq, mnthr, iwork;
    static logical wntua, wntva, wntun, wntuo, wntvn, wntvo, wntus, wntvs;
    static integer ie, ir, bdspac, iu;
    extern /* Subroutine */ int sgebrd_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, real *, integer *, integer *);
    extern doublereal slamch_(const char *), slange_(const char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    static real bignum;
    extern /* Subroutine */ int sgelqf_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *, integer *), slascl_(const char *, integer 
	    *, integer *, real *, real *, integer *, integer *, real *, 
	    integer *, integer *), sgeqrf_(integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *), slacpy_(const char 
	    *, integer *, integer *, real *, integer *, real *, integer *), slaset_(const char *, integer *, integer *, real *, real *, 
	    real *, integer *), sbdsqr_(const char *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), sorgbr_(
	    const char *, integer *, integer *, integer *, real *, integer *, real *
	    , real *, integer *, integer *), sormbr_(const char *, const char *, 
	    const char *, integer *, integer *, integer *, real *, integer *, real *
	    , real *, integer *, real *, integer *, integer *);
    static integer ldwrkr, minwrk, ldwrku, maxwrk;
    extern /* Subroutine */ int sorglq_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *);
    static real smlnum;
    extern /* Subroutine */ int sorgqr_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *);
    static logical lquery, wntuas, wntvas;
    static integer blk, ncu;
    static real dum[1], eps;
    static integer nru;


#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define u_ref(a_1,a_2) u[(a_2)*u_dim1 + a_1]
#define vt_ref(a_1,a_2) vt[(a_2)*vt_dim1 + a_1]


/*  -- LAPACK driver routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SGESVD computes the singular value decomposition (SVD) of a real   
    M-by-N matrix A, optionally computing the left and/or right singular   
    vectors. The SVD is written   

         A = U * SIGMA * transpose(V)   

    where SIGMA is an M-by-N matrix which is zero except for its   
    min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and   
    V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA   
    are the singular values of A; they are real and non-negative, and   
    are returned in descending order.  The first min(m,n) columns of   
    U and V are the left and right singular vectors of A.   

    Note that the routine returns V**T, not V.   

    Arguments   
    =========   

    JOBU    (input) CHARACTER*1   
            Specifies options for computing all or part of the matrix U:   
            = 'A':  all M columns of U are returned in array U:   
            = 'S':  the first min(m,n) columns of U (the left singular   
                    vectors) are returned in the array U;   
            = 'O':  the first min(m,n) columns of U (the left singular   
                    vectors) are overwritten on the array A;   
            = 'N':  no columns of U (no left singular vectors) are   
                    computed.   

    JOBVT   (input) CHARACTER*1   
            Specifies options for computing all or part of the matrix   
            V**T:   
            = 'A':  all N rows of V**T are returned in the array VT;   
            = 'S':  the first min(m,n) rows of V**T (the right singular   
                    vectors) are returned in the array VT;   
            = 'O':  the first min(m,n) rows of V**T (the right singular   
                    vectors) are overwritten on the array A;   
            = 'N':  no rows of V**T (no right singular vectors) are   
                    computed.   

            JOBVT and JOBU cannot both be 'O'.   

    M       (input) INTEGER   
            The number of rows of the input matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the input matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the M-by-N matrix A.   
            On exit,   
            if JOBU = 'O',  A is overwritten with the first min(m,n)   
                            columns of U (the left singular vectors,   
                            stored columnwise);   
            if JOBVT = 'O', A is overwritten with the first min(m,n)   
                            rows of V**T (the right singular vectors,   
                            stored rowwise);   
            if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A   
                            are destroyed.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    S       (output) REAL array, dimension (min(M,N))   
            The singular values of A, sorted so that S(i) >= S(i+1).   

    U       (output) REAL array, dimension (LDU,UCOL)   
            (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.   
            If JOBU = 'A', U contains the M-by-M orthogonal matrix U;   
            if JOBU = 'S', U contains the first min(m,n) columns of U   
            (the left singular vectors, stored columnwise);   
            if JOBU = 'N' or 'O', U is not referenced.   

    LDU     (input) INTEGER   
            The leading dimension of the array U.  LDU >= 1; if   
            JOBU = 'S' or 'A', LDU >= M.   

    VT      (output) REAL array, dimension (LDVT,N)   
            If JOBVT = 'A', VT contains the N-by-N orthogonal matrix   
            V**T;   
            if JOBVT = 'S', VT contains the first min(m,n) rows of   
            V**T (the right singular vectors, stored rowwise);   
            if JOBVT = 'N' or 'O', VT is not referenced.   

    LDVT    (input) INTEGER   
            The leading dimension of the array VT.  LDVT >= 1; if   
            JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK;   
            if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged   
            superdiagonal elements of an upper bidiagonal matrix B   
            whose diagonal is in S (not necessarily sorted). B   
            satisfies A = U * B * VT, so it has the same singular values   
            as A, and singular vectors related by U and VT.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= 1.   
            LWORK >= MAX(3*MIN(M,N)+MAX(M,N),5*MIN(M,N)).   
            For good performance, LWORK should generally be larger.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  if SBDSQR did not converge, INFO specifies how many   
                  superdiagonals of an intermediate bidiagonal form B   
                  did not converge to zero. See the description of WORK   
                  above for details.   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --s;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    --work;

    /* Function Body */
    *info = 0;
    minmn = f2cmin(*m,*n);
/* Writing concatenation */
    i__1[0] = 1, a__1[0] = jobu;
    i__1[1] = 1, a__1[1] = jobvt;
    s_cat(ch__1, a__1, i__1, &c__2, (ftnlen)2);
    mnthr = ilaenv_(&c__6, "SGESVD", ch__1, m, n, &c__0, &c__0, (ftnlen)6, (
	    ftnlen)2);
    wntua = lsame_(jobu, "A");
    wntus = lsame_(jobu, "S");
    wntuas = wntua || wntus;
    wntuo = lsame_(jobu, "O");
    wntun = lsame_(jobu, "N");
    wntva = lsame_(jobvt, "A");
    wntvs = lsame_(jobvt, "S");
    wntvas = wntva || wntvs;
    wntvo = lsame_(jobvt, "O");
    wntvn = lsame_(jobvt, "N");
    minwrk = 1;
    lquery = *lwork == -1;

    if (! (wntua || wntus || wntuo || wntun)) {
	*info = -1;
    } else if (! (wntva || wntvs || wntvo || wntvn) || wntvo && wntuo) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -6;
    } else if (*ldu < 1 || wntuas && *ldu < *m) {
	*info = -9;
    } else if (*ldvt < 1 || wntva && *ldvt < *n || wntvs && *ldvt < minmn) {
	*info = -11;
    }

/*     Compute workspace   
        (Note: Comments in the code beginning "Workspace:" describe the   
         minimal amount of workspace needed at that point in the code,   
         as well as the preferred amount for good performance.   
         NB refers to the optimal block size for the immediately   
         following subroutine, as returned by ILAENV.) */

    if (*info == 0 && (*lwork >= 1 || lquery) && *m > 0 && *n > 0) {
	if (*m >= *n) {

/*           Compute space needed for SBDSQR */

	    bdspac = *n * 5;
	    if (*m >= mnthr) {
		if (wntun) {

/*                 Path 1 (M much larger than N, JOBU='N') */

		    maxwrk = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    maxwrk = f2cmax(i__2,i__3);
		    if (wntvo || wntvas) {
/* Computing MAX */
			i__2 = maxwrk, i__3 = *n * 3 + (*n - 1) * ilaenv_(&
				c__1, "SORGBR", "P", n, n, n, &c_n1, (ftnlen)
				6, (ftnlen)1);
			maxwrk = f2cmax(i__2,i__3);
		    }
		    maxwrk = f2cmax(maxwrk,bdspac);
/* Computing MAX */
		    i__2 = *n << 2;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntuo && wntvn) {

/*                 Path 2 (M much larger than N, JOBU='O', JOBVT='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "SORGQR", 
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n + *n;
		    maxwrk = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntuo && wntvas) {

/*                 Path 3 (M much larger than N, JOBU='O', JOBVT='S' or   
                   'A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "SORGQR", 
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n + *n;
		    maxwrk = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntus && wntvn) {

/*                 Path 4 (M much larger than N, JOBU='S', JOBVT='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "SORGQR", 
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntus && wntvo) {

/*                 Path 5 (M much larger than N, JOBU='S', JOBVT='O') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "SORGQR", 
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = (*n << 1) * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntus && wntvas) {

/*                 Path 6 (M much larger than N, JOBU='S', JOBVT='S' or   
                   'A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "SORGQR", 
			    " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntua && wntvn) {

/*                 Path 7 (M much larger than N, JOBU='A', JOBVT='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "SORGQR", 
			    " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntua && wntvo) {

/*                 Path 8 (M much larger than N, JOBU='A', JOBVT='O') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "SORGQR", 
			    " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = (*n << 1) * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntua && wntvas) {

/*                 Path 9 (M much larger than N, JOBU='A', JOBVT='S' or   
                   'A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "SGEQRF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "SORGQR", 
			    " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORGBR"
			    , "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		}
	    } else {

/*              Path 10 (M at least N, but not much larger) */

		maxwrk = *n * 3 + (*m + *n) * ilaenv_(&c__1, "SGEBRD", " ", m,
			 n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
		if (wntus || wntuo) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + *n * ilaenv_(&c__1, "SORG"
			    "BR", "Q", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    maxwrk = f2cmax(i__2,i__3);
		}
		if (wntua) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + *m * ilaenv_(&c__1, "SORG"
			    "BR", "Q", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
		    maxwrk = f2cmax(i__2,i__3);
		}
		if (! wntvn) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    maxwrk = f2cmax(i__2,i__3);
		}
		maxwrk = f2cmax(maxwrk,bdspac);
/* Computing MAX */
		i__2 = *n * 3 + *m;
		minwrk = f2cmax(i__2,bdspac);
		maxwrk = f2cmax(maxwrk,minwrk);
	    }
	} else {

/*           Compute space needed for SBDSQR */

	    bdspac = *m * 5;
	    if (*n >= mnthr) {
		if (wntvn) {

/*                 Path 1t(N much larger than M, JOBVT='N') */

		    maxwrk = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    maxwrk = f2cmax(i__2,i__3);
		    if (wntuo || wntuas) {
/* Computing MAX */
			i__2 = maxwrk, i__3 = *m * 3 + *m * ilaenv_(&c__1, 
				"SORGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (
				ftnlen)1);
			maxwrk = f2cmax(i__2,i__3);
		    }
		    maxwrk = f2cmax(maxwrk,bdspac);
/* Computing MAX */
		    i__2 = *m << 2;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntvo && wntun) {

/*                 Path 2t(N much larger than M, JOBU='N', JOBVT='O') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "SORGLQ", 
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n + *m;
		    maxwrk = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntvo && wntuas) {

/*                 Path 3t(N much larger than M, JOBU='S' or 'A',   
                   JOBVT='O') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "SORGLQ", 
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "SORGBR"
			    , "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n + *m;
		    maxwrk = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntvs && wntun) {

/*                 Path 4t(N much larger than M, JOBU='N', JOBVT='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "SORGLQ", 
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntvs && wntuo) {

/*                 Path 5t(N much larger than M, JOBU='O', JOBVT='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "SORGLQ", 
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "SORGBR"
			    , "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = (*m << 1) * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntvs && wntuas) {

/*                 Path 6t(N much larger than M, JOBU='S' or 'A',   
                   JOBVT='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "SORGLQ", 
			    " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "SORGBR"
			    , "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntva && wntun) {

/*                 Path 7t(N much larger than M, JOBU='N', JOBVT='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "SORGLQ", 
			    " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntva && wntuo) {

/*                 Path 8t(N much larger than M, JOBU='O', JOBVT='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "SORGLQ", 
			    " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "SORGBR"
			    , "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = (*m << 1) * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		} else if (wntva && wntuas) {

/*                 Path 9t(N much larger than M, JOBU='S' or 'A',   
                   JOBVT='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "SGELQF", " ", m, n, &
			    c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "SORGLQ", 
			    " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1, 
			    "SGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)6, (
			    ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "P", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    wrkbl = f2cmax(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "SORGBR"
			    , "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    wrkbl = f2cmax(i__2,i__3);
		    wrkbl = f2cmax(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = f2cmax(i__2,bdspac);
		    maxwrk = f2cmax(maxwrk,minwrk);
		}
	    } else {

/*              Path 10t(N greater than M, but not much larger) */

		maxwrk = *m * 3 + (*m + *n) * ilaenv_(&c__1, "SGEBRD", " ", m,
			 n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
		if (wntvs || wntvo) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + *m * ilaenv_(&c__1, "SORG"
			    "BR", "P", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    maxwrk = f2cmax(i__2,i__3);
		}
		if (wntva) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + *n * ilaenv_(&c__1, "SORG"
			    "BR", "P", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
		    maxwrk = f2cmax(i__2,i__3);
		}
		if (! wntun) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1, 
			    "SORGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
			    1);
		    maxwrk = f2cmax(i__2,i__3);
		}
		maxwrk = f2cmax(maxwrk,bdspac);
/* Computing MAX */
		i__2 = *m * 3 + *n;
		minwrk = f2cmax(i__2,bdspac);
		maxwrk = f2cmax(maxwrk,minwrk);
	    }
	}
	work[1] = (real) maxwrk;
    }

    if (*lwork < minwrk && ! lquery) {
	*info = -13;
    }
    if (*info != 0) {
	i__2 = -(*info);
	xerbla_("SGESVD", &i__2);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	if (*lwork >= 1) {
	    work[1] = 1.f;
	}
	return 0;
    }

/*     Get machine constants */

    eps = slamch_("P");
    smlnum = sqrt(slamch_("S")) / eps;
    bignum = 1.f / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = slange_("M", m, n, &a[a_offset], lda, dum);
    iscl = 0;
    if (anrm > 0.f && anrm < smlnum) {
	iscl = 1;
	slascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, &
		ierr);
    } else if (anrm > bignum) {
	iscl = 1;
	slascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, &
		ierr);
    }

    if (*m >= *n) {

/*        A has at least as many rows as columns. If A has sufficiently   
          more rows than columns, first reduce using the QR   
          decomposition (if sufficient workspace available) */

	if (*m >= mnthr) {

	    if (wntun) {

/*              Path 1 (M much larger than N, JOBU='N')   
                No left singular vectors to be computed */

		itau = 1;
		iwork = itau + *n;

/*              Compute A=Q*R   
                (Workspace: need 2*N, prefer N+N*NB) */

		i__2 = *lwork - iwork + 1;
		sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &
			i__2, &ierr);

/*              Zero out below R */

		i__2 = *n - 1;
		i__3 = *n - 1;
		slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref(2, 1), 
			lda);
		ie = 1;
		itauq = ie + *n;
		itaup = itauq + *n;
		iwork = itaup + *n;

/*              Bidiagonalize R in A   
                (Workspace: need 4*N, prefer 3*N+2*N*NB) */

		i__2 = *lwork - iwork + 1;
		sgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[iwork], &i__2, &ierr);
		ncvt = 0;
		if (wntvo || wntvas) {

/*                 If right singular vectors desired, generate P'.   
                   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &
			    work[iwork], &i__2, &ierr);
		    ncvt = *n;
		}
		iwork = ie + *n;

/*              Perform bidiagonal QR iteration, computing right   
                singular vectors of A in A if desired   
                (Workspace: need BDSPAC) */

		sbdsqr_("U", n, &ncvt, &c__0, &c__0, &s[1], &work[ie], &a[
			a_offset], lda, dum, &c__1, dum, &c__1, &work[iwork], 
			info);

/*              If right singular vectors desired in VT, copy them there */

		if (wntvas) {
		    slacpy_("F", n, n, &a[a_offset], lda, &vt[vt_offset], 
			    ldvt);
		}

	    } else if (wntuo && wntvn) {

/*              Path 2 (M much larger than N, JOBU='O', JOBVT='N')   
                N left singular vectors to be overwritten on A and   
                no right singular vectors to be computed   

   Computing MAX */
		i__2 = *n << 2;
		if (*lwork >= *n * *n + f2cmax(i__2,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *lda * *n + *n;
		    if (*lwork >= f2cmax(i__2,i__3) + *lda * *n) {

/*                    WORK(IU) is LDA by N, WORK(IR) is LDA by N */

			ldwrku = *lda;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__2 = wrkbl, i__3 = *lda * *n + *n;
			if (*lwork >= f2cmax(i__2,i__3) + *n * *n) {

/*                    WORK(IU) is LDA by N, WORK(IR) is N by N */

			    ldwrku = *lda;
			    ldwrkr = *n;
			} else {

/*                    WORK(IU) is LDWRKU by N, WORK(IR) is N by N */

			    ldwrku = (*lwork - *n * *n - *n) / *n;
			    ldwrkr = *n;
			}
		    }
		    itau = ir + ldwrkr * *n;
		    iwork = itau + *n;

/*                 Compute A=Q*R   
                   (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
			    , &i__2, &ierr);

/*                 Copy R to WORK(IR) and zero out below it */

		    slacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
		    i__2 = *n - 1;
		    i__3 = *n - 1;
		    slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[ir + 1]
			    , &ldwrkr);

/*                 Generate Q in A   
                   (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize R in WORK(IR)   
                   (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Generate left vectors bidiagonalizing R   
                   (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &
			    work[iwork], &i__2, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left   
                   singular vectors of R in WORK(IR)   
                   (Workspace: need N*N+BDSPAC) */

		    sbdsqr_("U", n, &c__0, n, &c__0, &s[1], &work[ie], dum, &
			    c__1, &work[ir], &ldwrkr, dum, &c__1, &work[iwork]
			    , info);
		    iu = ie + *n;

/*                 Multiply Q in A by left singular vectors of R in   
                   WORK(IR), storing result in WORK(IU) and copying to A   
                   (Workspace: need N*N+2*N, prefer N*N+M*N+N) */

		    i__2 = *m;
		    i__3 = ldwrku;
		    for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__3) {
/* Computing MIN */
			i__4 = *m - i__ + 1;
			chunk = f2cmin(i__4,ldwrku);
			sgemm_("N", "N", &chunk, n, n, &c_b438, &a_ref(i__, 1)
				, lda, &work[ir], &ldwrkr, &c_b416, &work[iu],
				 &ldwrku);
			slacpy_("F", &chunk, n, &work[iu], &ldwrku, &a_ref(
				i__, 1), lda);
/* L10: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    ie = 1;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize A   
                   (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB) */

		    i__3 = *lwork - iwork + 1;
		    sgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__3, &ierr);

/*                 Generate left vectors bidiagonalizing A   
                   (Workspace: need 4*N, prefer 3*N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &
			    work[iwork], &i__3, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left   
                   singular vectors of A in A   
                   (Workspace: need BDSPAC) */

		    sbdsqr_("U", n, &c__0, m, &c__0, &s[1], &work[ie], dum, &
			    c__1, &a[a_offset], lda, dum, &c__1, &work[iwork],
			     info);

		}

	    } else if (wntuo && wntvas) {

/*              Path 3 (M much larger than N, JOBU='O', JOBVT='S' or 'A')   
                N left singular vectors to be overwritten on A and   
                N right singular vectors to be computed in VT   

   Computing MAX */
		i__3 = *n << 2;
		if (*lwork >= *n * *n + f2cmax(i__3,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__3 = wrkbl, i__2 = *lda * *n + *n;
		    if (*lwork >= f2cmax(i__3,i__2) + *lda * *n) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by N */

			ldwrku = *lda;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__3 = wrkbl, i__2 = *lda * *n + *n;
			if (*lwork >= f2cmax(i__3,i__2) + *n * *n) {

/*                    WORK(IU) is LDA by N and WORK(IR) is N by N */

			    ldwrku = *lda;
			    ldwrkr = *n;
			} else {

/*                    WORK(IU) is LDWRKU by N and WORK(IR) is N by N */

			    ldwrku = (*lwork - *n * *n - *n) / *n;
			    ldwrkr = *n;
			}
		    }
		    itau = ir + ldwrkr * *n;
		    iwork = itau + *n;

/*                 Compute A=Q*R   
                   (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
			    , &i__3, &ierr);

/*                 Copy R to VT, zeroing out below it */

		    slacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
			    ldvt);
		    i__3 = *n - 1;
		    i__2 = *n - 1;
		    slaset_("L", &i__3, &i__2, &c_b416, &c_b416, &vt_ref(2, 1)
			    , ldvt);

/*                 Generate Q in A   
                   (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__3, &ierr);
		    ie = itau;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize R in VT, copying result to WORK(IR)   
                   (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		    i__3 = *lwork - iwork + 1;
		    sgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &
			    work[itauq], &work[itaup], &work[iwork], &i__3, &
			    ierr);
		    slacpy_("L", n, n, &vt[vt_offset], ldvt, &work[ir], &
			    ldwrkr);

/*                 Generate left vectors bidiagonalizing R in WORK(IR)   
                   (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &
			    work[iwork], &i__3, &ierr);

/*                 Generate right vectors bidiagonalizing R in VT   
                   (Workspace: need N*N+4*N-1, prefer N*N+3*N+(N-1)*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], 
			    &work[iwork], &i__3, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left   
                   singular vectors of R in WORK(IR) and computing right   
                   singular vectors of R in VT   
                   (Workspace: need N*N+BDSPAC) */

		    sbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &vt[
			    vt_offset], ldvt, &work[ir], &ldwrkr, dum, &c__1, 
			    &work[iwork], info);
		    iu = ie + *n;

/*                 Multiply Q in A by left singular vectors of R in   
                   WORK(IR), storing result in WORK(IU) and copying to A   
                   (Workspace: need N*N+2*N, prefer N*N+M*N+N) */

		    i__3 = *m;
		    i__2 = ldwrku;
		    for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ +=
			     i__2) {
/* Computing MIN */
			i__4 = *m - i__ + 1;
			chunk = f2cmin(i__4,ldwrku);
			sgemm_("N", "N", &chunk, n, n, &c_b438, &a_ref(i__, 1)
				, lda, &work[ir], &ldwrkr, &c_b416, &work[iu],
				 &ldwrku);
			slacpy_("F", &chunk, n, &work[iu], &ldwrku, &a_ref(
				i__, 1), lda);
/* L20: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    itau = 1;
		    iwork = itau + *n;

/*                 Compute A=Q*R   
                   (Workspace: need 2*N, prefer N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
			    , &i__2, &ierr);

/*                 Copy R to VT, zeroing out below it */

		    slacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
			    ldvt);
		    i__2 = *n - 1;
		    i__3 = *n - 1;
		    slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &vt_ref(2, 1)
			    , ldvt);

/*                 Generate Q in A   
                   (Workspace: need 2*N, prefer N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize R in VT   
                   (Workspace: need 4*N, prefer 3*N+2*N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &
			    work[itauq], &work[itaup], &work[iwork], &i__2, &
			    ierr);

/*                 Multiply Q in A by left vectors bidiagonalizing R   
                   (Workspace: need 3*N+M, prefer 3*N+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sormbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, &
			    work[itauq], &a[a_offset], lda, &work[iwork], &
			    i__2, &ierr);

/*                 Generate right vectors bidiagonalizing R in VT   
                   (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], 
			    &work[iwork], &i__2, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left   
                   singular vectors of A in A and computing right   
                   singular vectors of A in VT   
                   (Workspace: need BDSPAC) */

		    sbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &vt[
			    vt_offset], ldvt, &a[a_offset], lda, dum, &c__1, &
			    work[iwork], info);

		}

	    } else if (wntus) {

		if (wntvn) {

/*                 Path 4 (M much larger than N, JOBU='S', JOBVT='N')   
                   N left singular vectors to be computed in U and   
                   no right singular vectors to be computed   

   Computing MAX */
		    i__2 = *n << 2;
		    if (*lwork >= *n * *n + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IR) is LDA by N */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is N by N */

			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R   
                      (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy R to WORK(IR), zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[ir 
				+ 1], &ldwrkr);

/*                    Generate Q in A   
                      (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IR)   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate left vectors bidiagonalizing R in WORK(IR)   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of R in WORK(IR)   
                      (Workspace: need N*N+BDSPAC) */

			sbdsqr_("U", n, &c__0, n, &c__0, &s[1], &work[ie], 
				dum, &c__1, &work[ir], &ldwrkr, dum, &c__1, &
				work[iwork], info);

/*                    Multiply Q in A by left singular vectors of R in   
                      WORK(IR), storing result in U   
                      (Workspace: need N*N) */

			sgemm_("N", "N", m, n, n, &c_b438, &a[a_offset], lda, 
				&work[ir], &ldwrkr, &c_b416, &u[u_offset], 
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref(2,
				 1), lda);

/*                    Bidiagonalize R in A   
                      (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left vectors bidiagonalizing R   
                      (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork], 
				&i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", n, &c__0, m, &c__0, &s[1], &work[ie], 
				dum, &c__1, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntvo) {

/*                 Path 5 (M much larger than N, JOBU='S', JOBVT='O')   
                   N left singular vectors to be computed in U and   
                   N right singular vectors to be overwritten on A   

   Computing MAX */
		    i__2 = *n << 2;
		    if (*lwork >= (*n << 1) * *n + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is LDA by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *n) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is N by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			} else {

/*                       WORK(IU) is N by N and WORK(IR) is N by N */

			    ldwrku = *n;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R   
                      (Workspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ 1], &ldwrku);

/*                    Generate Q in A   
                      (Workspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to   
                      WORK(IR)   
                      (Workspace: need 2*N*N+4*N,   
                                  prefer 2*N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("U", n, n, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate left bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need 2*N*N+4*N, prefer 2*N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
				, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR)   
                      (Workspace: need 2*N*N+4*N-1,   
                                  prefer 2*N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &work[ir], &ldwrkr, &work[itaup]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of R in WORK(IU) and computing   
                      right singular vectors of R in WORK(IR)   
                      (Workspace: need 2*N*N+BDSPAC) */

			sbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &work[
				ir], &ldwrkr, &work[iu], &ldwrku, dum, &c__1, 
				&work[iwork], info);

/*                    Multiply Q in A by left singular vectors of R in   
                      WORK(IU), storing result in U   
                      (Workspace: need N*N) */

			sgemm_("N", "N", m, n, n, &c_b438, &a[a_offset], lda, 
				&work[iu], &ldwrku, &c_b416, &u[u_offset], 
				ldu);

/*                    Copy right singular vectors of R to A   
                      (Workspace: need N*N) */

			slacpy_("F", n, n, &work[ir], &ldwrkr, &a[a_offset], 
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref(2,
				 1), lda);

/*                    Bidiagonalize R in A   
                      (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left vectors bidiagonalizing R   
                      (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork], 
				&i__2, &ierr)
				;

/*                    Generate right vectors bidiagonalizing R in A   
                      (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U and computing right   
                      singular vectors of A in A   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &a[
				a_offset], lda, &u[u_offset], ldu, dum, &c__1,
				 &work[iwork], info);

		    }

		} else if (wntvas) {

/*                 Path 6 (M much larger than N, JOBU='S', JOBVT='S'   
                           or 'A')   
                   N left singular vectors to be computed in U and   
                   N right singular vectors to be computed in VT   

   Computing MAX */
		    i__2 = *n << 2;
		    if (*lwork >= *n * *n + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IU) is LDA by N */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is N by N */

			    ldwrku = *n;
			}
			itau = iu + ldwrku * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R   
                      (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ 1], &ldwrku);

/*                    Generate Q in A   
                      (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to VT   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("U", n, n, &work[iu], &ldwrku, &vt[vt_offset],
				 ldvt);

/*                    Generate left bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
				, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT   
                      (Workspace: need N*N+4*N-1,   
                                  prefer N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of R in WORK(IU) and computing   
                      right singular vectors of R in VT   
                      (Workspace: need N*N+BDSPAC) */

			sbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &work[iu], &ldwrku, dum, &
				c__1, &work[iwork], info);

/*                    Multiply Q in A by left singular vectors of R in   
                      WORK(IU), storing result in U   
                      (Workspace: need N*N) */

			sgemm_("N", "N", m, n, n, &c_b438, &a[a_offset], lda, 
				&work[iu], &ldwrku, &c_b416, &u[u_offset], 
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R to VT, zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &vt_ref(
				2, 1), ldvt);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in VT   
                      (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], 
				&work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors   
                      in VT   
                      (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, 
				&work[itauq], &u[u_offset], ldu, &work[iwork],
				 &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT   
                      (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U and computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    } else if (wntua) {

		if (wntvn) {

/*                 Path 7 (M much larger than N, JOBU='A', JOBVT='N')   
                   M left singular vectors to be computed in U and   
                   no right singular vectors to be computed   

   Computing MAX */
		    i__2 = *n + *m, i__3 = *n << 2, i__2 = f2cmax(i__2,i__3);
		    if (*lwork >= *n * *n + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IR) is LDA by N */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is N by N */

			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Copy R to WORK(IR), zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[ir 
				+ 1], &ldwrkr);

/*                    Generate Q in U   
                      (Workspace: need N*N+N+M, prefer N*N+N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IR)   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR)   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of R in WORK(IR)   
                      (Workspace: need N*N+BDSPAC) */

			sbdsqr_("U", n, &c__0, n, &c__0, &s[1], &work[ie], 
				dum, &c__1, &work[ir], &ldwrkr, dum, &c__1, &
				work[iwork], info);

/*                    Multiply Q in U by left singular vectors of R in   
                      WORK(IR), storing result in A   
                      (Workspace: need N*N) */

			sgemm_("N", "N", m, n, n, &c_b438, &u[u_offset], ldu, 
				&work[ir], &ldwrkr, &c_b416, &a[a_offset], 
				lda);

/*                    Copy left singular vectors of A from A to U */

			slacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need N+M, prefer N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref(2,
				 1), lda);

/*                    Bidiagonalize R in A   
                      (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors   
                      in A   
                      (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork], 
				&i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", n, &c__0, m, &c__0, &s[1], &work[ie], 
				dum, &c__1, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntvo) {

/*                 Path 8 (M much larger than N, JOBU='A', JOBVT='O')   
                   M left singular vectors to be computed in U and   
                   N right singular vectors to be overwritten on A   

   Computing MAX */
		    i__2 = *n + *m, i__3 = *n << 2, i__2 = f2cmax(i__2,i__3);
		    if (*lwork >= (*n << 1) * *n + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is LDA by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *n) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is N by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			} else {

/*                       WORK(IU) is N by N and WORK(IR) is N by N */

			    ldwrku = *n;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need 2*N*N+N+M, prefer 2*N*N+N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ 1], &ldwrku);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to   
                      WORK(IR)   
                      (Workspace: need 2*N*N+4*N,   
                                  prefer 2*N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("U", n, n, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate left bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need 2*N*N+4*N, prefer 2*N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
				, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR)   
                      (Workspace: need 2*N*N+4*N-1,   
                                  prefer 2*N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &work[ir], &ldwrkr, &work[itaup]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of R in WORK(IU) and computing   
                      right singular vectors of R in WORK(IR)   
                      (Workspace: need 2*N*N+BDSPAC) */

			sbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &work[
				ir], &ldwrkr, &work[iu], &ldwrku, dum, &c__1, 
				&work[iwork], info);

/*                    Multiply Q in U by left singular vectors of R in   
                      WORK(IU), storing result in A   
                      (Workspace: need N*N) */

			sgemm_("N", "N", m, n, n, &c_b438, &u[u_offset], ldu, 
				&work[iu], &ldwrku, &c_b416, &a[a_offset], 
				lda);

/*                    Copy left singular vectors of A from A to U */

			slacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Copy right singular vectors of R from WORK(IR) to A */

			slacpy_("F", n, n, &work[ir], &ldwrkr, &a[a_offset], 
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need N+M, prefer N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &a_ref(2,
				 1), lda);

/*                    Bidiagonalize R in A   
                      (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors   
                      in A   
                      (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork], 
				&i__2, &ierr)
				;

/*                    Generate right bidiagonalizing vectors in A   
                      (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U and computing right   
                      singular vectors of A in A   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &a[
				a_offset], lda, &u[u_offset], ldu, dum, &c__1,
				 &work[iwork], info);

		    }

		} else if (wntvas) {

/*                 Path 9 (M much larger than N, JOBU='A', JOBVT='S'   
                           or 'A')   
                   M left singular vectors to be computed in U and   
                   N right singular vectors to be computed in VT   

   Computing MAX */
		    i__2 = *n + *m, i__3 = *n << 2, i__2 = f2cmax(i__2,i__3);
		    if (*lwork >= *n * *n + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IU) is LDA by N */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is N by N */

			    ldwrku = *n;
			}
			itau = iu + ldwrku * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need N*N+N+M, prefer N*N+N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ 1], &ldwrku);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to VT   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("U", n, n, &work[iu], &ldwrku, &vt[vt_offset],
				 ldvt);

/*                    Generate left bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
				, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT   
                      (Workspace: need N*N+4*N-1,   
                                  prefer N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of R in WORK(IU) and computing   
                      right singular vectors of R in VT   
                      (Workspace: need N*N+BDSPAC) */

			sbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &work[iu], &ldwrku, dum, &
				c__1, &work[iwork], info);

/*                    Multiply Q in U by left singular vectors of R in   
                      WORK(IU), storing result in A   
                      (Workspace: need N*N) */

			sgemm_("N", "N", m, n, n, &c_b438, &u[u_offset], ldu, 
				&work[iu], &ldwrku, &c_b416, &a[a_offset], 
				lda);

/*                    Copy left singular vectors of A from A to U */

			slacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U   
                      (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			sgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
				ldu);

/*                    Generate Q in U   
                      (Workspace: need N+M, prefer N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R from A to VT, zeroing out below it */

			slacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);
			i__2 = *n - 1;
			i__3 = *n - 1;
			slaset_("L", &i__2, &i__3, &c_b416, &c_b416, &vt_ref(
				2, 1), ldvt);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in VT   
                      (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], 
				&work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors   
                      in VT   
                      (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, 
				&work[itauq], &u[u_offset], ldu, &work[iwork],
				 &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT   
                      (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U and computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    }

	} else {

/*           M .LT. MNTHR   

             Path 10 (M at least N, but not much larger)   
             Reduce to bidiagonal form without QR decomposition */

	    ie = 1;
	    itauq = ie + *n;
	    itaup = itauq + *n;
	    iwork = itaup + *n;

/*           Bidiagonalize A   
             (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB) */

	    i__2 = *lwork - iwork + 1;
	    sgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[iwork], &i__2, &ierr);
	    if (wntuas) {

/*              If left singular vectors desired in U, copy result to U   
                and generate left bidiagonalizing vectors in U   
                (Workspace: need 3*N+NCU, prefer 3*N+NCU*NB) */

		slacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);
		if (wntus) {
		    ncu = *n;
		}
		if (wntua) {
		    ncu = *m;
		}
		i__2 = *lwork - iwork + 1;
		sorgbr_("Q", m, &ncu, n, &u[u_offset], ldu, &work[itauq], &
			work[iwork], &i__2, &ierr);
	    }
	    if (wntvas) {

/*              If right singular vectors desired in VT, copy result to   
                VT and generate right bidiagonalizing vectors in VT   
                (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		slacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
		i__2 = *lwork - iwork + 1;
		sorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &
			work[iwork], &i__2, &ierr);
	    }
	    if (wntuo) {

/*              If left singular vectors desired in A, generate left   
                bidiagonalizing vectors in A   
                (Workspace: need 4*N, prefer 3*N+N*NB) */

		i__2 = *lwork - iwork + 1;
		sorgbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &work[
			iwork], &i__2, &ierr);
	    }
	    if (wntvo) {

/*              If right singular vectors desired in A, generate right   
                bidiagonalizing vectors in A   
                (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		i__2 = *lwork - iwork + 1;
		sorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[
			iwork], &i__2, &ierr);
	    }
	    iwork = ie + *n;
	    if (wntuas || wntuo) {
		nru = *m;
	    }
	    if (wntun) {
		nru = 0;
	    }
	    if (wntvas || wntvo) {
		ncvt = *n;
	    }
	    if (wntvn) {
		ncvt = 0;
	    }
	    if (! wntuo && ! wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing   
                left singular vectors in U and computing right singular   
                vectors in VT   
                (Workspace: need BDSPAC) */

		sbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &u[u_offset], ldu, dum, &c__1, &
			work[iwork], info);
	    } else if (! wntuo && wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing   
                left singular vectors in U and computing right singular   
                vectors in A   
                (Workspace: need BDSPAC) */

		sbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &a[
			a_offset], lda, &u[u_offset], ldu, dum, &c__1, &work[
			iwork], info);
	    } else {

/*              Perform bidiagonal QR iteration, if desired, computing   
                left singular vectors in A and computing right singular   
                vectors in VT   
                (Workspace: need BDSPAC) */

		sbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &a[a_offset], lda, dum, &c__1, &
			work[iwork], info);
	    }

	}

    } else {

/*        A has more columns than rows. If A has sufficiently more   
          columns than rows, first reduce using the LQ decomposition (if   
          sufficient workspace available) */

	if (*n >= mnthr) {

	    if (wntvn) {

/*              Path 1t(N much larger than M, JOBVT='N')   
                No right singular vectors to be computed */

		itau = 1;
		iwork = itau + *m;

/*              Compute A=L*Q   
                (Workspace: need 2*M, prefer M+M*NB) */

		i__2 = *lwork - iwork + 1;
		sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &
			i__2, &ierr);

/*              Zero out above L */

		i__2 = *m - 1;
		i__3 = *m - 1;
		slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref(1, 2), 
			lda);
		ie = 1;
		itauq = ie + *m;
		itaup = itauq + *m;
		iwork = itaup + *m;

/*              Bidiagonalize L in A   
                (Workspace: need 4*M, prefer 3*M+2*M*NB) */

		i__2 = *lwork - iwork + 1;
		sgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[iwork], &i__2, &ierr);
		if (wntuo || wntuas) {

/*                 If left singular vectors desired, generate Q   
                   (Workspace: need 4*M, prefer 3*M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq], &
			    work[iwork], &i__2, &ierr);
		}
		iwork = ie + *m;
		nru = 0;
		if (wntuo || wntuas) {
		    nru = *m;
		}

/*              Perform bidiagonal QR iteration, computing left singular   
                vectors of A in A if desired   
                (Workspace: need BDSPAC) */

		sbdsqr_("U", m, &c__0, &nru, &c__0, &s[1], &work[ie], dum, &
			c__1, &a[a_offset], lda, dum, &c__1, &work[iwork], 
			info);

/*              If left singular vectors desired in U, copy them there */

		if (wntuas) {
		    slacpy_("F", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		}

	    } else if (wntvo && wntun) {

/*              Path 2t(N much larger than M, JOBU='N', JOBVT='O')   
                M right singular vectors to be overwritten on A and   
                no left singular vectors to be computed   

   Computing MAX */
		i__2 = *m << 2;
		if (*lwork >= *m * *m + f2cmax(i__2,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *lda * *n + *m;
		    if (*lwork >= f2cmax(i__2,i__3) + *lda * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by M */

			ldwrku = *lda;
			chunk = *n;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__2 = wrkbl, i__3 = *lda * *n + *m;
			if (*lwork >= f2cmax(i__2,i__3) + *m * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is M by M */

			    ldwrku = *lda;
			    chunk = *n;
			    ldwrkr = *m;
			} else {

/*                    WORK(IU) is M by CHUNK and WORK(IR) is M by M */

			    ldwrku = *m;
			    chunk = (*lwork - *m * *m - *m) / *m;
			    ldwrkr = *m;
			}
		    }
		    itau = ir + ldwrkr * *m;
		    iwork = itau + *m;

/*                 Compute A=L*Q   
                   (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
			    , &i__2, &ierr);

/*                 Copy L to WORK(IR) and zero out above it */

		    slacpy_("L", m, m, &a[a_offset], lda, &work[ir], &ldwrkr);
		    i__2 = *m - 1;
		    i__3 = *m - 1;
		    slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[ir + 
			    ldwrkr], &ldwrkr);

/*                 Generate Q in A   
                   (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize L in WORK(IR)   
                   (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Generate right vectors bidiagonalizing L   
                   (Workspace: need M*M+4*M-1, prefer M*M+3*M+(M-1)*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &
			    work[iwork], &i__2, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing right   
                   singular vectors of L in WORK(IR)   
                   (Workspace: need M*M+BDSPAC) */

		    sbdsqr_("U", m, m, &c__0, &c__0, &s[1], &work[ie], &work[
			    ir], &ldwrkr, dum, &c__1, dum, &c__1, &work[iwork]
			    , info);
		    iu = ie + *m;

/*                 Multiply right singular vectors of L in WORK(IR) by Q   
                   in A, storing result in WORK(IU) and copying to A   
                   (Workspace: need M*M+2*M, prefer M*M+M*N+M) */

		    i__2 = *n;
		    i__3 = chunk;
		    for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__3) {
/* Computing MIN */
			i__4 = *n - i__ + 1;
			blk = f2cmin(i__4,chunk);
			sgemm_("N", "N", m, &blk, m, &c_b438, &work[ir], &
				ldwrkr, &a_ref(1, i__), lda, &c_b416, &work[
				iu], &ldwrku);
			slacpy_("F", m, &blk, &work[iu], &ldwrku, &a_ref(1, 
				i__), lda);
/* L30: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    ie = 1;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize A   
                   (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

		    i__3 = *lwork - iwork + 1;
		    sgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__3, &ierr);

/*                 Generate right vectors bidiagonalizing A   
                   (Workspace: need 4*M, prefer 3*M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &
			    work[iwork], &i__3, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing right   
                   singular vectors of A in A   
                   (Workspace: need BDSPAC) */

		    sbdsqr_("L", m, n, &c__0, &c__0, &s[1], &work[ie], &a[
			    a_offset], lda, dum, &c__1, dum, &c__1, &work[
			    iwork], info);

		}

	    } else if (wntvo && wntuas) {

/*              Path 3t(N much larger than M, JOBU='S' or 'A', JOBVT='O')   
                M right singular vectors to be overwritten on A and   
                M left singular vectors to be computed in U   

   Computing MAX */
		i__3 = *m << 2;
		if (*lwork >= *m * *m + f2cmax(i__3,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__3 = wrkbl, i__2 = *lda * *n + *m;
		    if (*lwork >= f2cmax(i__3,i__2) + *lda * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by M */

			ldwrku = *lda;
			chunk = *n;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__3 = wrkbl, i__2 = *lda * *n + *m;
			if (*lwork >= f2cmax(i__3,i__2) + *m * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is M by M */

			    ldwrku = *lda;
			    chunk = *n;
			    ldwrkr = *m;
			} else {

/*                    WORK(IU) is M by CHUNK and WORK(IR) is M by M */

			    ldwrku = *m;
			    chunk = (*lwork - *m * *m - *m) / *m;
			    ldwrkr = *m;
			}
		    }
		    itau = ir + ldwrkr * *m;
		    iwork = itau + *m;

/*                 Compute A=L*Q   
                   (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
			    , &i__3, &ierr);

/*                 Copy L to U, zeroing about above it */

		    slacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		    i__3 = *m - 1;
		    i__2 = *m - 1;
		    slaset_("U", &i__3, &i__2, &c_b416, &c_b416, &u_ref(1, 2),
			     ldu);

/*                 Generate Q in A   
                   (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__3, &ierr);
		    ie = itau;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize L in U, copying result to WORK(IR)   
                   (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		    i__3 = *lwork - iwork + 1;
		    sgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__3, &ierr);
		    slacpy_("U", m, m, &u[u_offset], ldu, &work[ir], &ldwrkr);

/*                 Generate right vectors bidiagonalizing L in WORK(IR)   
                   (Workspace: need M*M+4*M-1, prefer M*M+3*M+(M-1)*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &
			    work[iwork], &i__3, &ierr);

/*                 Generate left vectors bidiagonalizing L in U   
                   (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    sorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &
			    work[iwork], &i__3, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing left   
                   singular vectors of L in U, and computing right   
                   singular vectors of L in WORK(IR)   
                   (Workspace: need M*M+BDSPAC) */

		    sbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[ir], 
			    &ldwrkr, &u[u_offset], ldu, dum, &c__1, &work[
			    iwork], info);
		    iu = ie + *m;

/*                 Multiply right singular vectors of L in WORK(IR) by Q   
                   in A, storing result in WORK(IU) and copying to A   
                   (Workspace: need M*M+2*M, prefer M*M+M*N+M)) */

		    i__3 = *n;
		    i__2 = chunk;
		    for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ +=
			     i__2) {
/* Computing MIN */
			i__4 = *n - i__ + 1;
			blk = f2cmin(i__4,chunk);
			sgemm_("N", "N", m, &blk, m, &c_b438, &work[ir], &
				ldwrkr, &a_ref(1, i__), lda, &c_b416, &work[
				iu], &ldwrku);
			slacpy_("F", m, &blk, &work[iu], &ldwrku, &a_ref(1, 
				i__), lda);
/* L40: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    itau = 1;
		    iwork = itau + *m;

/*                 Compute A=L*Q   
                   (Workspace: need 2*M, prefer M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
			    , &i__2, &ierr);

/*                 Copy L to U, zeroing out above it */

		    slacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		    i__2 = *m - 1;
		    i__3 = *m - 1;
		    slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &u_ref(1, 2),
			     ldu);

/*                 Generate Q in A   
                   (Workspace: need 2*M, prefer M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize L in U   
                   (Workspace: need 4*M, prefer 3*M+2*M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Multiply right vectors bidiagonalizing L by Q in A   
                   (Workspace: need 3*M+N, prefer 3*M+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    sormbr_("P", "L", "T", m, n, m, &u[u_offset], ldu, &work[
			    itaup], &a[a_offset], lda, &work[iwork], &i__2, &
			    ierr);

/*                 Generate left vectors bidiagonalizing L in U   
                   (Workspace: need 4*M, prefer 3*M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    sorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &
			    work[iwork], &i__2, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing left   
                   singular vectors of A in U and computing right   
                   singular vectors of A in A   
                   (Workspace: need BDSPAC) */

		    sbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &a[
			    a_offset], lda, &u[u_offset], ldu, dum, &c__1, &
			    work[iwork], info);

		}

	    } else if (wntvs) {

		if (wntun) {

/*                 Path 4t(N much larger than M, JOBU='N', JOBVT='S')   
                   M right singular vectors to be computed in VT and   
                   no left singular vectors to be computed   

   Computing MAX */
		    i__2 = *m << 2;
		    if (*lwork >= *m * *m + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IR) is LDA by M */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is M by M */

			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q   
                      (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy L to WORK(IR), zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[ir 
				+ ldwrkr], &ldwrkr);

/*                    Generate Q in A   
                      (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(m, n, m, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IR)   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate right vectors bidiagonalizing L in   
                      WORK(IR)   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right   
                      singular vectors of L in WORK(IR)   
                      (Workspace: need M*M+BDSPAC) */

			sbdsqr_("U", m, m, &c__0, &c__0, &s[1], &work[ie], &
				work[ir], &ldwrkr, dum, &c__1, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IR) by   
                      Q in A, storing result in VT   
                      (Workspace: need M*M) */

			sgemm_("N", "N", m, n, m, &c_b438, &work[ir], &ldwrkr,
				 &a[a_offset], lda, &c_b416, &vt[vt_offset], 
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy result to VT */

			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref(1,
				 2), lda);

/*                    Bidiagonalize L in A   
                      (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right vectors bidiagonalizing L by Q in VT   
                      (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", m, n, &c__0, &c__0, &s[1], &work[ie], &
				vt[vt_offset], ldvt, dum, &c__1, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntuo) {

/*                 Path 5t(N much larger than M, JOBU='O', JOBVT='S')   
                   M right singular vectors to be computed in VT and   
                   M left singular vectors to be overwritten on A   

   Computing MAX */
		    i__2 = *m << 2;
		    if (*lwork >= (*m << 1) * *m + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is LDA by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *m) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is M by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			} else {

/*                       WORK(IU) is M by M and WORK(IR) is M by M */

			    ldwrku = *m;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q   
                      (Workspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out below it */

			slacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ ldwrku], &ldwrku);

/*                    Generate Q in A   
                      (Workspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(m, n, m, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to   
                      WORK(IR)   
                      (Workspace: need 2*M*M+4*M,   
                                  prefer 2*M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("L", m, m, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate right bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need 2*M*M+4*M-1,   
                                  prefer 2*M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
				, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR)   
                      (Workspace: need 2*M*M+4*M, prefer 2*M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of L in WORK(IR) and computing   
                      right singular vectors of L in WORK(IU)   
                      (Workspace: need 2*M*M+BDSPAC) */

			sbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &work[ir], &ldwrkr, dum, &c__1, 
				&work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by   
                      Q in A, storing result in VT   
                      (Workspace: need M*M) */

			sgemm_("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku,
				 &a[a_offset], lda, &c_b416, &vt[vt_offset], 
				ldvt);

/*                    Copy left singular vectors of L to A   
                      (Workspace: need M*M) */

			slacpy_("F", m, m, &work[ir], &ldwrkr, &a[a_offset], 
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref(1,
				 2), lda);

/*                    Bidiagonalize L in A   
                      (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right vectors bidiagonalizing L by Q in VT   
                      (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors of L in A   
                      (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, compute left   
                      singular vectors of A in A and compute right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &a[a_offset], lda, dum, &
				c__1, &work[iwork], info);

		    }

		} else if (wntuas) {

/*                 Path 6t(N much larger than M, JOBU='S' or 'A',   
                           JOBVT='S')   
                   M right singular vectors to be computed in VT and   
                   M left singular vectors to be computed in U   

   Computing MAX */
		    i__2 = *m << 2;
		    if (*lwork >= *m * *m + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IU) is LDA by N */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is LDA by M */

			    ldwrku = *m;
			}
			itau = iu + ldwrku * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q   
                      (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ ldwrku], &ldwrku);

/*                    Generate Q in A   
                      (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(m, n, m, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to U   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("L", m, m, &work[iu], &ldwrku, &u[u_offset], 
				ldu);

/*                    Generate right bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need M*M+4*M-1,   
                                  prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
				, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of L in U and computing right   
                      singular vectors of L in WORK(IU)   
                      (Workspace: need M*M+BDSPAC) */

			sbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by   
                      Q in A, storing result in VT   
                      (Workspace: need M*M) */

			sgemm_("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku,
				 &a[a_offset], lda, &c_b416, &vt[vt_offset], 
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to U, zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], 
				ldu);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &u_ref(1,
				 2), ldu);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in U   
                      (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in U by Q   
                      in VT   
                      (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("P", "L", "T", m, n, m, &u[u_offset], ldu, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U   
                      (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U and computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    } else if (wntva) {

		if (wntun) {

/*                 Path 7t(N much larger than M, JOBU='N', JOBVT='A')   
                   N right singular vectors to be computed in VT and   
                   no left singular vectors to be computed   

   Computing MAX */
		    i__2 = *n + *m, i__3 = *m << 2, i__2 = f2cmax(i__2,i__3);
		    if (*lwork >= *m * *m + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IR) is LDA by M */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is M by M */

			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Copy L to WORK(IR), zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[ir 
				+ ldwrkr], &ldwrkr);

/*                    Generate Q in VT   
                      (Workspace: need M*M+M+N, prefer M*M+M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IR)   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR)   
                      (Workspace: need M*M+4*M-1,   
                                  prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right   
                      singular vectors of L in WORK(IR)   
                      (Workspace: need M*M+BDSPAC) */

			sbdsqr_("U", m, m, &c__0, &c__0, &s[1], &work[ie], &
				work[ir], &ldwrkr, dum, &c__1, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IR) by   
                      Q in VT, storing result in A   
                      (Workspace: need M*M) */

			sgemm_("N", "N", m, n, m, &c_b438, &work[ir], &ldwrkr,
				 &vt[vt_offset], ldvt, &c_b416, &a[a_offset], 
				lda);

/*                    Copy right singular vectors of A from A to VT */

			slacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need M+N, prefer M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref(1,
				 2), lda);

/*                    Bidiagonalize L in A   
                      (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in A by Q   
                      in VT   
                      (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", m, n, &c__0, &c__0, &s[1], &work[ie], &
				vt[vt_offset], ldvt, dum, &c__1, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntuo) {

/*                 Path 8t(N much larger than M, JOBU='O', JOBVT='A')   
                   N right singular vectors to be computed in VT and   
                   M left singular vectors to be overwritten on A   

   Computing MAX */
		    i__2 = *n + *m, i__3 = *m << 2, i__2 = f2cmax(i__2,i__3);
		    if (*lwork >= (*m << 1) * *m + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is LDA by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *m) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is M by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			} else {

/*                       WORK(IU) is M by M and WORK(IR) is M by M */

			    ldwrku = *m;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need 2*M*M+M+N, prefer 2*M*M+M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ ldwrku], &ldwrku);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to   
                      WORK(IR)   
                      (Workspace: need 2*M*M+4*M,   
                                  prefer 2*M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("L", m, m, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate right bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need 2*M*M+4*M-1,   
                                  prefer 2*M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
				, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR)   
                      (Workspace: need 2*M*M+4*M, prefer 2*M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq]
				, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of L in WORK(IR) and computing   
                      right singular vectors of L in WORK(IU)   
                      (Workspace: need 2*M*M+BDSPAC) */

			sbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &work[ir], &ldwrkr, dum, &c__1, 
				&work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by   
                      Q in VT, storing result in A   
                      (Workspace: need M*M) */

			sgemm_("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku,
				 &vt[vt_offset], ldvt, &c_b416, &a[a_offset], 
				lda);

/*                    Copy right singular vectors of A from A to VT */

			slacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Copy left singular vectors of A from WORK(IR) to A */

			slacpy_("F", m, m, &work[ir], &ldwrkr, &a[a_offset], 
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need M+N, prefer M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &a_ref(1,
				 2), lda);

/*                    Bidiagonalize L in A   
                      (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in A by Q   
                      in VT   
                      (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in A   
                      (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in A and computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &a[a_offset], lda, dum, &
				c__1, &work[iwork], info);

		    }

		} else if (wntuas) {

/*                 Path 9t(N much larger than M, JOBU='S' or 'A',   
                           JOBVT='A')   
                   N right singular vectors to be computed in VT and   
                   M left singular vectors to be computed in U   

   Computing MAX */
		    i__2 = *n + *m, i__3 = *m << 2, i__2 = f2cmax(i__2,i__3);
		    if (*lwork >= *m * *m + f2cmax(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IU) is LDA by M */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is M by M */

			    ldwrku = *m;
			}
			itau = iu + ldwrku * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need M*M+M+N, prefer M*M+M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &work[iu 
				+ ldwrku], &ldwrku);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to U   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			slacpy_("L", m, m, &work[iu], &ldwrku, &u[u_offset], 
				ldu);

/*                    Generate right bidiagonalizing vectors in WORK(IU)   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
				, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U   
                      (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of L in U and computing right   
                      singular vectors of L in WORK(IU)   
                      (Workspace: need M*M+BDSPAC) */

			sbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by   
                      Q in VT, storing result in A   
                      (Workspace: need M*M) */

			sgemm_("N", "N", m, n, m, &c_b438, &work[iu], &ldwrku,
				 &vt[vt_offset], ldvt, &c_b416, &a[a_offset], 
				lda);

/*                    Copy right singular vectors of A from A to VT */

			slacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT   
                      (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
				ldvt);

/*                    Generate Q in VT   
                      (Workspace: need M+N, prefer M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to U, zeroing out above it */

			slacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], 
				ldu);
			i__2 = *m - 1;
			i__3 = *m - 1;
			slaset_("U", &i__2, &i__3, &c_b416, &c_b416, &u_ref(1,
				 2), ldu);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in U   
                      (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			sgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in U by Q   
                      in VT   
                      (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			sormbr_("P", "L", "T", m, n, m, &u[u_offset], ldu, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U   
                      (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			sorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left   
                      singular vectors of A in U and computing right   
                      singular vectors of A in VT   
                      (Workspace: need BDSPAC) */

			sbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    }

	} else {

/*           N .LT. MNTHR   

             Path 10t(N greater than M, but not much larger)   
             Reduce to bidiagonal form without LQ decomposition */

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    iwork = itaup + *m;

/*           Bidiagonalize A   
             (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

	    i__2 = *lwork - iwork + 1;
	    sgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[iwork], &i__2, &ierr);
	    if (wntuas) {

/*              If left singular vectors desired in U, copy result to U   
                and generate left bidiagonalizing vectors in U   
                (Workspace: need 4*M-1, prefer 3*M+(M-1)*NB) */

		slacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		i__2 = *lwork - iwork + 1;
		sorgbr_("Q", m, m, n, &u[u_offset], ldu, &work[itauq], &work[
			iwork], &i__2, &ierr);
	    }
	    if (wntvas) {

/*              If right singular vectors desired in VT, copy result to   
                VT and generate right bidiagonalizing vectors in VT   
                (Workspace: need 3*M+NRVT, prefer 3*M+NRVT*NB) */

		slacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
		if (wntva) {
		    nrvt = *n;
		}
		if (wntvs) {
		    nrvt = *m;
		}
		i__2 = *lwork - iwork + 1;
		sorgbr_("P", &nrvt, n, m, &vt[vt_offset], ldvt, &work[itaup], 
			&work[iwork], &i__2, &ierr);
	    }
	    if (wntuo) {

/*              If left singular vectors desired in A, generate left   
                bidiagonalizing vectors in A   
                (Workspace: need 4*M-1, prefer 3*M+(M-1)*NB) */

		i__2 = *lwork - iwork + 1;
		sorgbr_("Q", m, m, n, &a[a_offset], lda, &work[itauq], &work[
			iwork], &i__2, &ierr);
	    }
	    if (wntvo) {

/*              If right singular vectors desired in A, generate right   
                bidiagonalizing vectors in A   
                (Workspace: need 4*M, prefer 3*M+M*NB) */

		i__2 = *lwork - iwork + 1;
		sorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[
			iwork], &i__2, &ierr);
	    }
	    iwork = ie + *m;
	    if (wntuas || wntuo) {
		nru = *m;
	    }
	    if (wntun) {
		nru = 0;
	    }
	    if (wntvas || wntvo) {
		ncvt = *n;
	    }
	    if (wntvn) {
		ncvt = 0;
	    }
	    if (! wntuo && ! wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing   
                left singular vectors in U and computing right singular   
                vectors in VT   
                (Workspace: need BDSPAC) */

		sbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &u[u_offset], ldu, dum, &c__1, &
			work[iwork], info);
	    } else if (! wntuo && wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing   
                left singular vectors in U and computing right singular   
                vectors in A   
                (Workspace: need BDSPAC) */

		sbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &a[
			a_offset], lda, &u[u_offset], ldu, dum, &c__1, &work[
			iwork], info);
	    } else {

/*              Perform bidiagonal QR iteration, if desired, computing   
                left singular vectors in A and computing right singular   
                vectors in VT   
                (Workspace: need BDSPAC) */

		sbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &a[a_offset], lda, dum, &c__1, &
			work[iwork], info);
	    }

	}

    }

/*     If SBDSQR failed to converge, copy unconverged superdiagonals   
       to WORK( 2:MINMN ) */

    if (*info != 0) {
	if (ie > 2) {
	    i__2 = minmn - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__ + 1] = work[i__ + ie - 1];
/* L50: */
	    }
	}
	if (ie < 2) {
	    for (i__ = minmn - 1; i__ >= 1; --i__) {
		work[i__ + 1] = work[i__ + ie - 1];
/* L60: */
	    }
	}
    }

/*     Undo scaling if necessary */

    if (iscl == 1) {
	if (anrm > bignum) {
	    slascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
	if (*info != 0 && anrm > bignum) {
	    i__2 = minmn - 1;
	    slascl_("G", &c__0, &c__0, &bignum, &anrm, &i__2, &c__1, &work[2],
		     &minmn, &ierr);
	}
	if (anrm < smlnum) {
	    slascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
	if (*info != 0 && anrm < smlnum) {
	    i__2 = minmn - 1;
	    slascl_("G", &c__0, &c__0, &smlnum, &anrm, &i__2, &c__1, &work[2],
		     &minmn, &ierr);
	}
    }

/*     Return optimal workspace in WORK(1) */

    work[1] = (real) maxwrk;

    return 0;

/*     End of SGESVD */

} /* sgesvd_ */

#undef vt_ref
#undef u_ref
#undef a_ref


//===============================================================================

/* Subroutine */ int sorgl2_(integer *m, integer *n, integer *k, real *a, 
	integer *lda, real *tau, real *work, integer *info)

{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORGL2 generates an m by n real matrix Q with orthonormal rows,   
    which is defined as the first m rows of a product of k elementary   
    reflectors of order n   

          Q  =  H(k) . . . H(2) H(1)   

    as returned by SGELQF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. N >= M.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the   
            matrix Q. M >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the i-th row must contain the vector which defines   
            the elementary reflector H(i), for i = 1,2,...,k, as returned   
            by SGELQF in the first k rows of its array argument A.   
            On exit, the m-by-n matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= max(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGELQF.   

    WORK    (workspace) REAL array, dimension (M)   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument has an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real r__1;
    /* Local variables */
    static integer i__, j, l;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    slarf_(const char *, integer *, integer *, real *, integer *, real *, 
	    real *, integer *, real *), xerbla_(const char *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < *m) {
	*info = -2;
    } else if (*k < 0 || *k > *m) {
	*info = -3;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGL2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	return 0;
    }

    if (*k < *m) {

/*        Initialise rows k+1:m to rows of the unit matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (l = *k + 1; l <= i__2; ++l) {
		a_ref(l, j) = 0.f;
/* L10: */
	    }
	    if (j > *k && j <= *m) {
		a_ref(j, j) = 1.f;
	    }
/* L20: */
	}
    }

    for (i__ = *k; i__ >= 1; --i__) {

/*        Apply H(i) to A(i:m,i:n) from the right */

	if (i__ < *n) {
	    if (i__ < *m) {
		a_ref(i__, i__) = 1.f;
		i__1 = *m - i__;
		i__2 = *n - i__ + 1;
		slarf_("Right", &i__1, &i__2, &a_ref(i__, i__), lda, &tau[i__]
			, &a_ref(i__ + 1, i__), lda, &work[1]);
	    }
	    i__1 = *n - i__;
	    r__1 = -tau[i__];
	    sscal_(&i__1, &r__1, &a_ref(i__, i__ + 1), lda);
	}
	a_ref(i__, i__) = 1.f - tau[i__];

/*        Set A(i,1:i-1) to zero */

	i__1 = i__ - 1;
	for (l = 1; l <= i__1; ++l) {
	    a_ref(i__, l) = 0.f;
/* L30: */
	}
/* L40: */
    }
    return 0;

/*     End of SORGL2 */

} /* sorgl2_ */

#undef a_ref

//==========================================================

/* Subroutine */ int sorglq_(integer *m, integer *n, integer *k, real *a, 
	integer *lda, real *tau, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORGLQ generates an M-by-N real matrix Q with orthonormal rows,   
    which is defined as the first M rows of a product of K elementary   
    reflectors of order N   

          Q  =  H(k) . . . H(2) H(1)   

    as returned by SGELQF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. N >= M.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the   
            matrix Q. M >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the i-th row must contain the vector which defines   
            the elementary reflector H(i), for i = 1,2,...,k, as returned   
            by SGELQF in the first k rows of its array argument A.   
            On exit, the M-by-N matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= max(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGELQF.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= max(1,M).   
            For optimum performance LWORK >= M*NB, where NB is   
            the optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument has an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, j, l, nbmin, iinfo;
    extern /* Subroutine */ int sorgl2_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *);
    static integer ib, nb, ki, kk, nx;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "SORGLQ", " ", m, n, k, &c_n1, (ftnlen)6, (ftnlen)1);
    lwkopt = f2cmax(1,*m) * nb;
    work[1] = (real) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < *m) {
	*info = -2;
    } else if (*k < 0 || *k > *m) {
	*info = -3;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -5;
    } else if (*lwork < f2cmax(1,*m) && ! lquery) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGLQ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *m;
    if (nb > 1 && nb < *k) {

/*        Determine when to cross over from blocked to unblocked code.   

   Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "SORGLQ", " ", m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);
	if (nx < *k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *m;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and   
                determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "SORGLQ", " ", m, n, k, &c_n1,
			 (ftnlen)6, (ftnlen)1);
		nbmin = f2cmax(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < *k && nx < *k) {

/*        Use blocked code after the last block.   
          The first kk rows are handled by the block method. */

	ki = (*k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = *k, i__2 = ki + nb;
	kk = f2cmin(i__1,i__2);

/*        Set A(kk+1:m,1:kk) to zero. */

	i__1 = kk;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = kk + 1; i__ <= i__2; ++i__) {
		a_ref(i__, j) = 0.f;
/* L10: */
	    }
/* L20: */
	}
    } else {
	kk = 0;
    }

/*     Use unblocked code for the last or only block. */

    if (kk < *m) {
	i__1 = *m - kk;
	i__2 = *n - kk;
	i__3 = *k - kk;
	sorgl2_(&i__1, &i__2, &i__3, &a_ref(kk + 1, kk + 1), lda, &tau[kk + 1]
		, &work[1], &iinfo);
    }

    if (kk > 0) {

/*        Use blocked code */

	i__1 = -nb;
	for (i__ = ki + 1; i__1 < 0 ? i__ >= 1 : i__ <= 1; i__ += i__1) {
/* Computing MIN */
	    i__2 = nb, i__3 = *k - i__ + 1;
	    ib = f2cmin(i__2,i__3);
	    if (i__ + ib <= *m) {

/*              Form the triangular factor of the block reflector   
                H = H(i) H(i+1) . . . H(i+ib-1) */

		i__2 = *n - i__ + 1;
		slarft_("Forward", "Rowwise", &i__2, &ib, &a_ref(i__, i__), 
			lda, &tau[i__], &work[1], &ldwork);

/*              Apply H' to A(i+ib:m,i:n) from the right */

		i__2 = *m - i__ - ib + 1;
		i__3 = *n - i__ + 1;
		slarfb_("Right", "Transpose", "Forward", "Rowwise", &i__2, &
			i__3, &ib, &a_ref(i__, i__), lda, &work[1], &ldwork, &
			a_ref(i__ + ib, i__), lda, &work[ib + 1], &ldwork);
	    }

/*           Apply H' to columns i:n of current block */

	    i__2 = *n - i__ + 1;
	    sorgl2_(&ib, &i__2, &ib, &a_ref(i__, i__), lda, &tau[i__], &work[
		    1], &iinfo);

/*           Set columns 1:i-1 of current block to zero */

	    i__2 = i__ - 1;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + ib - 1;
		for (l = i__; l <= i__3; ++l) {
		    a_ref(l, j) = 0.f;
/* L30: */
		}
/* L40: */
	    }
/* L50: */
	}
    }

    work[1] = (real) iws;
    return 0;

/*     End of SORGLQ */

} /* sorglq_ */

#undef a_ref

//=====================================================================

doublereal slange_(const char *norm, integer *m, integer *n, real *a, integer *lda, 
	real *work)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLANGE  returns the value of the one norm,  or the Frobenius norm, or   
    the  infinity norm,  or the  element of  largest absolute value  of a   
    real matrix A.   

    Description   
    ===========   

    SLANGE returns the value   

       SLANGE = ( max(abs(A(i,j))), NORM = 'M' or 'm'   
                (   
                ( norm1(A),         NORM = '1', 'O' or 'o'   
                (   
                ( normI(A),         NORM = 'I' or 'i'   
                (   
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'   

    where  norm1  denotes the  one norm of a matrix (maximum column sum),   
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and   
    normF  denotes the  Frobenius norm of a matrix (square root of sum of   
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.   

    Arguments   
    =========   

    NORM    (input) CHARACTER*1   
            Specifies the value to be returned in SLANGE as described   
            above.   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.  When M = 0,   
            SLANGE is set to zero.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.  When N = 0,   
            SLANGE is set to zero.   

    A       (input) REAL array, dimension (LDA,N)   
            The m by n matrix A.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(M,1).   

    WORK    (workspace) REAL array, dimension (LWORK),   
            where LWORK >= M when NORM = 'I'; otherwise, WORK is not   
            referenced.   

   =====================================================================   


       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real ret_val, r__1, r__2, r__3;
    /* Builtin functions */
    //double sqrt(doublereal);
    /* Local variables */
    static integer i__, j;
    static real scale;
    extern logical lsame_(const char *, const char *);
    static real value;
    extern /* Subroutine */ int slassq_(integer *, real *, integer *, real *, 
	    real *);
    static real sum;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (f2cmin(*m,*n) == 0) {
	value = 0.f;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.f;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		r__2 = value, r__3 = (r__1 = a_ref(i__, j), dabs(r__1));
		value = df2cmax(r__2,r__3);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.f;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = 0.f;
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		sum += (r__1 = a_ref(i__, j), dabs(r__1));
/* L30: */
	    }
	    value = df2cmax(value,sum);
/* L40: */
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.f;
/* L50: */
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__] += (r__1 = a_ref(i__, j), dabs(r__1));
/* L60: */
	    }
/* L70: */
	}
	value = 0.f;
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    r__1 = value, r__2 = work[i__];
	    value = df2cmax(r__1,r__2);
/* L80: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.f;
	sum = 1.f;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    slassq_(m, &a_ref(1, j), &c__1, &scale, &sum);
/* L90: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of SLANGE */

} /* slange_ */

#undef a_ref

//===================================================

/* Subroutine */ int sgebrd_(integer *m, integer *n, real *a, integer *lda, 
	real *d__, real *e, real *tauq, real *taup, real *work, integer *
	lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SGEBRD reduces a general real M-by-N matrix A to upper or lower   
    bidiagonal form B by an orthogonal transformation: Q**T * A * P = B.   

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows in the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns in the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the M-by-N general matrix to be reduced.   
            On exit,   
            if m >= n, the diagonal and the first superdiagonal are   
              overwritten with the upper bidiagonal matrix B; the   
              elements below the diagonal, with the array TAUQ, represent   
              the orthogonal matrix Q as a product of elementary   
              reflectors, and the elements above the first superdiagonal,   
              with the array TAUP, represent the orthogonal matrix P as   
              a product of elementary reflectors;   
            if m < n, the diagonal and the first subdiagonal are   
              overwritten with the lower bidiagonal matrix B; the   
              elements below the first subdiagonal, with the array TAUQ,   
              represent the orthogonal matrix Q as a product of   
              elementary reflectors, and the elements above the diagonal,   
              with the array TAUP, represent the orthogonal matrix P as   
              a product of elementary reflectors.   
            See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    D       (output) REAL array, dimension (min(M,N))   
            The diagonal elements of the bidiagonal matrix B:   
            D(i) = A(i,i).   

    E       (output) REAL array, dimension (min(M,N)-1)   
            The off-diagonal elements of the bidiagonal matrix B:   
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;   
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.   

    TAUQ    (output) REAL array dimension (min(M,N))   
            The scalar factors of the elementary reflectors which   
            represent the orthogonal matrix Q. See Further Details.   

    TAUP    (output) REAL array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors which   
            represent the orthogonal matrix P. See Further Details.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The length of the array WORK.  LWORK >= max(1,M,N).   
            For optimum performance LWORK >= (M+N)*NB, where NB   
            is the optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    The matrices Q and P are represented as products of elementary   
    reflectors:   

    If m >= n,   

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)   

    Each H(i) and G(i) has the form:   

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'   

    where tauq and taup are real scalars, and v and u are real vectors;   
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);   
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);   
    tauq is stored in TAUQ(i) and taup in TAUP(i).   

    If m < n,   

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)   

    Each H(i) and G(i) has the form:   

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'   

    where tauq and taup are real scalars, and v and u are real vectors;   
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);   
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);   
    tauq is stored in TAUQ(i) and taup in TAUP(i).   

    The contents of A on exit are illustrated by the following examples:   

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):   

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )   
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )   
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )   
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )   
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )   
      (  v1  v2  v3  v4  v5 )   

    where d and e denote diagonal and off-diagonal elements of B, vi   
    denotes an element of the vector defining H(i), and ui an element of   
    the vector defining G(i).   

    =====================================================================   


       Test the input parameters   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    static real c_b21 = -1.f;
    static real c_b22 = 1.f;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    /* Local variables */
    static integer i__, j, nbmin, iinfo;
    extern /* Subroutine */ int sgemm_(const char *, const char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    static integer minmn;
    extern /* Subroutine */ int sgebd2_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, real *, integer *);
    static integer nb, nx;
    extern /* Subroutine */ int slabrd_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, real *, real *, real *, integer *, 
	    real *, integer *);
    static real ws;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    static integer ldwrkx, ldwrky, lwkopt;
    static logical lquery;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    --work;

    /* Function Body */
    *info = 0;
/* Computing MAX */
    i__1 = 1, i__2 = ilaenv_(&c__1, "SGEBRD", " ", m, n, &c_n1, &c_n1, (
	    ftnlen)6, (ftnlen)1);
    nb = f2cmax(i__1,i__2);
    lwkopt = (*m + *n) * nb;
    work[1] = (real) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = f2cmax(1,*m);
	if (*lwork < f2cmax(i__1,*n) && ! lquery) {
	    *info = -10;
	}
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("SGEBRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    minmn = f2cmin(*m,*n);
    if (minmn == 0) {
	work[1] = 1.f;
	return 0;
    }

    ws = (real) f2cmax(*m,*n);
    ldwrkx = *m;
    ldwrky = *n;

    if (nb > 1 && nb < minmn) {

/*        Set the crossover point NX.   

   Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__3, "SGEBRD", " ", m, n, &c_n1, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);

/*        Determine when to switch from blocked to unblocked code. */

	if (nx < minmn) {
	    ws = (real) ((*m + *n) * nb);
	    if ((real) (*lwork) < ws) {

/*              Not enough work space for the optimal NB, consider using   
                a smaller block size. */

		nbmin = ilaenv_(&c__2, "SGEBRD", " ", m, n, &c_n1, &c_n1, (
			ftnlen)6, (ftnlen)1);
		if (*lwork >= (*m + *n) * nbmin) {
		    nb = *lwork / (*m + *n);
		} else {
		    nb = 1;
		    nx = minmn;
		}
	    }
	}
    } else {
	nx = minmn;
    }

    i__1 = minmn - nx;
    i__2 = nb;
    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {

/*        Reduce rows and columns i:i+nb-1 to bidiagonal form and return   
          the matrices X and Y which are needed to update the unreduced   
          part of the matrix */

	i__3 = *m - i__ + 1;
	i__4 = *n - i__ + 1;
	slabrd_(&i__3, &i__4, &nb, &a_ref(i__, i__), lda, &d__[i__], &e[i__], 
		&tauq[i__], &taup[i__], &work[1], &ldwrkx, &work[ldwrkx * nb 
		+ 1], &ldwrky);

/*        Update the trailing submatrix A(i+nb:m,i+nb:n), using an update   
          of the form  A := A - V*Y' - X*U' */

	i__3 = *m - i__ - nb + 1;
	i__4 = *n - i__ - nb + 1;
	sgemm_("No transpose", "Transpose", &i__3, &i__4, &nb, &c_b21, &a_ref(
		i__ + nb, i__), lda, &work[ldwrkx * nb + nb + 1], &ldwrky, &
		c_b22, &a_ref(i__ + nb, i__ + nb), lda)
		;
	i__3 = *m - i__ - nb + 1;
	i__4 = *n - i__ - nb + 1;
	sgemm_("No transpose", "No transpose", &i__3, &i__4, &nb, &c_b21, &
		work[nb + 1], &ldwrkx, &a_ref(i__, i__ + nb), lda, &c_b22, &
		a_ref(i__ + nb, i__ + nb), lda);

/*        Copy diagonal and off-diagonal elements of B back into A */

	if (*m >= *n) {
	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a_ref(j, j) = d__[j];
		a_ref(j, j + 1) = e[j];
/* L10: */
	    }
	} else {
	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a_ref(j, j) = d__[j];
		a_ref(j + 1, j) = e[j];
/* L20: */
	    }
	}
/* L30: */
    }

/*     Use unblocked code to reduce the remainder of the matrix */

    i__2 = *m - i__ + 1;
    i__1 = *n - i__ + 1;
    sgebd2_(&i__2, &i__1, &a_ref(i__, i__), lda, &d__[i__], &e[i__], &tauq[
	    i__], &taup[i__], &work[1], &iinfo);
    work[1] = ws;
    return 0;

/*     End of SGEBRD */

} /* sgebrd_ */

#undef a_ref

//===============================================================

/* Subroutine */ int sgebd2_(integer *m, integer *n, real *a, integer *lda, 
	real *d__, real *e, real *tauq, real *taup, real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SGEBD2 reduces a real general m by n matrix A to upper or lower   
    bidiagonal form B by an orthogonal transformation: Q' * A * P = B.   

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows in the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns in the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the m by n general matrix to be reduced.   
            On exit,   
            if m >= n, the diagonal and the first superdiagonal are   
              overwritten with the upper bidiagonal matrix B; the   
              elements below the diagonal, with the array TAUQ, represent   
              the orthogonal matrix Q as a product of elementary   
              reflectors, and the elements above the first superdiagonal,   
              with the array TAUP, represent the orthogonal matrix P as   
              a product of elementary reflectors;   
            if m < n, the diagonal and the first subdiagonal are   
              overwritten with the lower bidiagonal matrix B; the   
              elements below the first subdiagonal, with the array TAUQ,   
              represent the orthogonal matrix Q as a product of   
              elementary reflectors, and the elements above the diagonal,   
              with the array TAUP, represent the orthogonal matrix P as   
              a product of elementary reflectors.   
            See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    D       (output) REAL array, dimension (min(M,N))   
            The diagonal elements of the bidiagonal matrix B:   
            D(i) = A(i,i).   

    E       (output) REAL array, dimension (min(M,N)-1)   
            The off-diagonal elements of the bidiagonal matrix B:   
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;   
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.   

    TAUQ    (output) REAL array dimension (min(M,N))   
            The scalar factors of the elementary reflectors which   
            represent the orthogonal matrix Q. See Further Details.   

    TAUP    (output) REAL array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors which   
            represent the orthogonal matrix P. See Further Details.   

    WORK    (workspace) REAL array, dimension (max(M,N))   

    INFO    (output) INTEGER   
            = 0: successful exit.   
            < 0: if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    The matrices Q and P are represented as products of elementary   
    reflectors:   

    If m >= n,   

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)   

    Each H(i) and G(i) has the form:   

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'   

    where tauq and taup are real scalars, and v and u are real vectors;   
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);   
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);   
    tauq is stored in TAUQ(i) and taup in TAUP(i).   

    If m < n,   

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)   

    Each H(i) and G(i) has the form:   

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'   

    where tauq and taup are real scalars, and v and u are real vectors;   
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);   
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);   
    tauq is stored in TAUQ(i) and taup in TAUP(i).   

    The contents of A on exit are illustrated by the following examples:   

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):   

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )   
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )   
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )   
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )   
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )   
      (  v1  v2  v3  v4  v5 )   

    where d and e denote diagonal and off-diagonal elements of B, vi   
    denotes an element of the vector defining H(i), and ui an element of   
    the vector defining G(i).   

    =====================================================================   


       Test the input parameters   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int slarf_(const char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *), xerbla_(
	    const char *, integer *), slarfg_(integer *, real *, real *, 
	    integer *, real *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -4;
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("SGEBD2", &i__1);
	return 0;
    }

    if (*m >= *n) {

/*        Reduce to upper bidiagonal form */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector H(i) to annihilate A(i+1:m,i)   

   Computing MIN */
	    i__2 = i__ + 1;
	    i__3 = *m - i__ + 1;
	    slarfg_(&i__3, &a_ref(i__, i__), &a_ref(f2cmin(i__2,*m), i__), &c__1,
		     &tauq[i__]);
	    d__[i__] = a_ref(i__, i__);
	    a_ref(i__, i__) = 1.f;

/*           Apply H(i) to A(i:m,i+1:n) from the left */

	    i__2 = *m - i__ + 1;
	    i__3 = *n - i__;
	    slarf_("Left", &i__2, &i__3, &a_ref(i__, i__), &c__1, &tauq[i__], 
		    &a_ref(i__, i__ + 1), lda, &work[1]);
	    a_ref(i__, i__) = d__[i__];

	    if (i__ < *n) {

/*              Generate elementary reflector G(i) to annihilate   
                A(i,i+2:n)   

   Computing MIN */
		i__2 = i__ + 2;
		i__3 = *n - i__;
		slarfg_(&i__3, &a_ref(i__, i__ + 1), &a_ref(i__, f2cmin(i__2,*n))
			, lda, &taup[i__]);
		e[i__] = a_ref(i__, i__ + 1);
		a_ref(i__, i__ + 1) = 1.f;

/*              Apply G(i) to A(i+1:m,i+1:n) from the right */

		i__2 = *m - i__;
		i__3 = *n - i__;
		slarf_("Right", &i__2, &i__3, &a_ref(i__, i__ + 1), lda, &
			taup[i__], &a_ref(i__ + 1, i__ + 1), lda, &work[1]);
		a_ref(i__, i__ + 1) = e[i__];
	    } else {
		taup[i__] = 0.f;
	    }
/* L10: */
	}
    } else {

/*        Reduce to lower bidiagonal form */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector G(i) to annihilate A(i,i+1:n)   

   Computing MIN */
	    i__2 = i__ + 1;
	    i__3 = *n - i__ + 1;
	    slarfg_(&i__3, &a_ref(i__, i__), &a_ref(i__, f2cmin(i__2,*n)), lda, &
		    taup[i__]);
	    d__[i__] = a_ref(i__, i__);
	    a_ref(i__, i__) = 1.f;

/*           Apply G(i) to A(i+1:m,i:n) from the right   

   Computing MIN */
	    i__2 = i__ + 1;
	    i__3 = *m - i__;
	    i__4 = *n - i__ + 1;
	    slarf_("Right", &i__3, &i__4, &a_ref(i__, i__), lda, &taup[i__], &
		    a_ref(f2cmin(i__2,*m), i__), lda, &work[1]);
	    a_ref(i__, i__) = d__[i__];

	    if (i__ < *m) {

/*              Generate elementary reflector H(i) to annihilate   
                A(i+2:m,i)   

   Computing MIN */
		i__2 = i__ + 2;
		i__3 = *m - i__;
		slarfg_(&i__3, &a_ref(i__ + 1, i__), &a_ref(f2cmin(i__2,*m), i__)
			, &c__1, &tauq[i__]);
		e[i__] = a_ref(i__ + 1, i__);
		a_ref(i__ + 1, i__) = 1.f;

/*              Apply H(i) to A(i+1:m,i+1:n) from the left */

		i__2 = *m - i__;
		i__3 = *n - i__;
		slarf_("Left", &i__2, &i__3, &a_ref(i__ + 1, i__), &c__1, &
			tauq[i__], &a_ref(i__ + 1, i__ + 1), lda, &work[1]);
		a_ref(i__ + 1, i__) = e[i__];
	    } else {
		tauq[i__] = 0.f;
	    }
/* L20: */
	}
    }
    return 0;

/*     End of SGEBD2 */

} /* sgebd2_ */

#undef a_ref

//============================================================

/* Subroutine */ int sormbr_(const char *vect, const char *side, const char *trans, integer *m, 
	integer *n, integer *k, real *a, integer *lda, real *tau, real *c__, 
	integer *ldc, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    If VECT = 'Q', SORMBR overwrites the general real M-by-N matrix C   
    with   
                    SIDE = 'L'     SIDE = 'R'   
    TRANS = 'N':      Q * C          C * Q   
    TRANS = 'T':      Q**T * C       C * Q**T   

    If VECT = 'P', SORMBR overwrites the general real M-by-N matrix C   
    with   
                    SIDE = 'L'     SIDE = 'R'   
    TRANS = 'N':      P * C          C * P   
    TRANS = 'T':      P**T * C       C * P**T   

    Here Q and P**T are the orthogonal matrices determined by SGEBRD when   
    reducing a real matrix A to bidiagonal form: A = Q * B * P**T. Q and   
    P**T are defined as products of elementary reflectors H(i) and G(i)   
    respectively.   

    Let nq = m if SIDE = 'L' and nq = n if SIDE = 'R'. Thus nq is the   
    order of the orthogonal matrix Q or P**T that is applied.   

    If VECT = 'Q', A is assumed to have been an NQ-by-K matrix:   
    if nq >= k, Q = H(1) H(2) . . . H(k);   
    if nq < k, Q = H(1) H(2) . . . H(nq-1).   

    If VECT = 'P', A is assumed to have been a K-by-NQ matrix:   
    if k < nq, P = G(1) G(2) . . . G(k);   
    if k >= nq, P = G(1) G(2) . . . G(nq-1).   

    Arguments   
    =========   

    VECT    (input) CHARACTER*1   
            = 'Q': apply Q or Q**T;   
            = 'P': apply P or P**T.   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q, Q**T, P or P**T from the Left;   
            = 'R': apply Q, Q**T, P or P**T from the Right.   

    TRANS   (input) CHARACTER*1   
            = 'N':  No transpose, apply Q  or P;   
            = 'T':  Transpose, apply Q**T or P**T.   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    K       (input) INTEGER   
            If VECT = 'Q', the number of columns in the original   
            matrix reduced by SGEBRD.   
            If VECT = 'P', the number of rows in the original   
            matrix reduced by SGEBRD.   
            K >= 0.   

    A       (input) REAL array, dimension   
                                  (LDA,min(nq,K)) if VECT = 'Q'   
                                  (LDA,nq)        if VECT = 'P'   
            The vectors which define the elementary reflectors H(i) and   
            G(i), whose products determine the matrices Q and P, as   
            returned by SGEBRD.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.   
            If VECT = 'Q', LDA >= max(1,nq);   
            if VECT = 'P', LDA >= max(1,min(nq,K)).   

    TAU     (input) REAL array, dimension (min(nq,K))   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i) or G(i) which determines Q or P, as returned   
            by SGEBRD in the array argument TAUQ or TAUP.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the M-by-N matrix C.   
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q   
            or P*C or P**T*C or C*P or C*P**T.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            If SIDE = 'L', LWORK >= max(1,N);   
            if SIDE = 'R', LWORK >= max(1,M).   
            For optimum performance LWORK >= N*NB if SIDE = 'L', and   
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal   
            blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__2 = 2;

    typedef const char *address;    

    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2];
    char ch__1[2];
    /* Builtin functions   
       Subroutine */ int s_cat(char *, const char **, integer *, integer *, ftnlen);
    /* Local variables */
    static logical left;
    extern logical lsame_(const char *, const char *);
    static integer iinfo, i1, i2, nb, mi, ni, nq, nw;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    static logical notran, applyq;
    static char transt[1];
    extern /* Subroutine */ int sormlq_(const char *, const char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *, integer *);
    static integer lwkopt;
    static logical lquery;
    extern /* Subroutine */ int sormqr_(const char *, const char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *, integer *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    applyq = lsame_(vect, "Q");
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q or P and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if (! applyq && ! lsame_(vect, "P")) {
	*info = -1;
    } else if (! left && ! lsame_(side, "R")) {
	*info = -2;
    } else if (! notran && ! lsame_(trans, "T")) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*k < 0) {
	*info = -6;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = 1, i__2 = f2cmin(nq,*k);
	if (applyq && *lda < f2cmax(1,nq) || ! applyq && *lda < f2cmax(i__1,i__2)) {
	    *info = -8;
	} else if (*ldc < f2cmax(1,*m)) {
	    *info = -11;
	} else if (*lwork < f2cmax(1,nw) && ! lquery) {
	    *info = -13;
	}
    }

    if (*info == 0) {
	if (applyq) {
	    if (left) {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *m - 1;
		i__2 = *m - 1;
		nb = ilaenv_(&c__1, "SORMQR", ch__1, &i__1, n, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    } else {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *n - 1;
		i__2 = *n - 1;
		nb = ilaenv_(&c__1, "SORMQR", ch__1, m, &i__1, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    }
	} else {
	    if (left) {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *m - 1;
		i__2 = *m - 1;
		nb = ilaenv_(&c__1, "SORMLQ", ch__1, &i__1, n, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    } else {
/* Writing concatenation */
		i__3[0] = 1, a__1[0] = side;
		i__3[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
		i__1 = *n - 1;
		i__2 = *n - 1;
		nb = ilaenv_(&c__1, "SORMLQ", ch__1, m, &i__1, &i__2, &c_n1, (
			ftnlen)6, (ftnlen)2);
	    }
	}
	lwkopt = f2cmax(1,nw) * nb;
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORMBR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    work[1] = 1.f;
    if (*m == 0 || *n == 0) {
	return 0;
    }

    if (applyq) {

/*        Apply Q */

	if (nq >= *k) {

/*           Q was determined by a call to SGEBRD with nq >= k */

	    sormqr_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		    c_offset], ldc, &work[1], lwork, &iinfo);
	} else if (nq > 1) {

/*           Q was determined by a call to SGEBRD with nq < k */

	    if (left) {
		mi = *m - 1;
		ni = *n;
		i1 = 2;
		i2 = 1;
	    } else {
		mi = *m;
		ni = *n - 1;
		i1 = 1;
		i2 = 2;
	    }
	    i__1 = nq - 1;
	    sormqr_(side, trans, &mi, &ni, &i__1, &a_ref(2, 1), lda, &tau[1], 
		    &c___ref(i1, i2), ldc, &work[1], lwork, &iinfo);
	}
    } else {

/*        Apply P */

	if (notran) {
	    *(unsigned char *)transt = 'T';
	} else {
	    *(unsigned char *)transt = 'N';
	}
	if (nq > *k) {

/*           P was determined by a call to SGEBRD with nq > k */

	    sormlq_(side, transt, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		    c_offset], ldc, &work[1], lwork, &iinfo);
	} else if (nq > 1) {

/*           P was determined by a call to SGEBRD with nq <= k */

	    if (left) {
		mi = *m - 1;
		ni = *n;
		i1 = 2;
		i2 = 1;
	    } else {
		mi = *m;
		ni = *n - 1;
		i1 = 1;
		i2 = 2;
	    }
	    i__1 = nq - 1;
	    sormlq_(side, transt, &mi, &ni, &i__1, &a_ref(1, 2), lda, &tau[1],
		     &c___ref(i1, i2), ldc, &work[1], lwork, &iinfo);
	}
    }
    work[1] = (real) lwkopt;
    return 0;

/*     End of SORMBR */

} /* sormbr_ */

#undef c___ref
#undef a_ref

//=================================================================

/* Subroutine */ int sgelqf_(integer *m, integer *n, real *a, integer *lda, 
	real *tau, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SGELQF computes an LQ factorization of a real M-by-N matrix A:   
    A = L * Q.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the M-by-N matrix A.   
            On exit, the elements on and below the diagonal of the array   
            contain the m-by-min(m,n) lower trapezoidal matrix L (L is   
            lower triangular if m <= n); the elements above the diagonal,   
            with the array TAU, represent the orthogonal matrix Q as a   
            product of elementary reflectors (see Further Details).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    TAU     (output) REAL array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.  LWORK >= max(1,M).   
            For optimum performance LWORK >= M*NB, where NB is the   
            optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    Further Details   
    ===============   

    The matrix Q is represented as a product of elementary reflectors   

       Q = H(k) . . . H(2) H(1), where k = min(m,n).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n),   
    and tau in TAU(i).   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    /* Local variables */
    static integer i__, k, nbmin, iinfo;
    extern /* Subroutine */ int sgelq2_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *);
    static integer ib, nb, nx;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "SGELQF", " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)
	    1);
    lwkopt = *m * nb;
    work[1] = (real) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -4;
    } else if (*lwork < f2cmax(1,*m) && ! lquery) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGELQF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    k = f2cmin(*m,*n);
    if (k == 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *m;
    if (nb > 1 && nb < k) {

/*        Determine when to cross over from blocked to unblocked code.   

   Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "SGELQF", " ", m, n, &c_n1, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *m;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and   
                determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "SGELQF", " ", m, n, &c_n1, &
			c_n1, (ftnlen)6, (ftnlen)1);
		nbmin = f2cmax(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < k && nx < k) {

/*        Use blocked code initially */

	i__1 = k - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = f2cmin(i__3,nb);

/*           Compute the LQ factorization of the current block   
             A(i:i+ib-1,i:n) */

	    i__3 = *n - i__ + 1;
	    sgelq2_(&ib, &i__3, &a_ref(i__, i__), lda, &tau[i__], &work[1], &
		    iinfo);
	    if (i__ + ib <= *m) {

/*              Form the triangular factor of the block reflector   
                H = H(i) H(i+1) . . . H(i+ib-1) */

		i__3 = *n - i__ + 1;
		slarft_("Forward", "Rowwise", &i__3, &ib, &a_ref(i__, i__), 
			lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(i+ib:m,i:n) from the right */

		i__3 = *m - i__ - ib + 1;
		i__4 = *n - i__ + 1;
		slarfb_("Right", "No transpose", "Forward", "Rowwise", &i__3, 
			&i__4, &ib, &a_ref(i__, i__), lda, &work[1], &ldwork, 
			&a_ref(i__ + ib, i__), lda, &work[ib + 1], &ldwork);
	    }
/* L10: */
	}
    } else {
	i__ = 1;
    }

/*     Use unblocked code to factor the last or only block. */

    if (i__ <= k) {
	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	sgelq2_(&i__2, &i__1, &a_ref(i__, i__), lda, &tau[i__], &work[1], &
		iinfo);
    }

    work[1] = (real) iws;
    return 0;

/*     End of SGELQF */

} /* sgelqf_ */

#undef a_ref

//===========================================================

/* Subroutine */ int sormlq_(const char *side, const char *trans, integer *m, integer *n, 
	integer *k, real *a, integer *lda, real *tau, real *c__, integer *ldc,
	 real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORMLQ overwrites the general real M-by-N matrix C with   

                    SIDE = 'L'     SIDE = 'R'   
    TRANS = 'N':      Q * C          C * Q   
    TRANS = 'T':      Q**T * C       C * Q**T   

    where Q is a real orthogonal matrix defined as the product of k   
    elementary reflectors   

          Q = H(k) . . . H(2) H(1)   

    as returned by SGELQF. Q is of order M if SIDE = 'L' and of order N   
    if SIDE = 'R'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q**T from the Left;   
            = 'R': apply Q or Q**T from the Right.   

    TRANS   (input) CHARACTER*1   
            = 'N':  No transpose, apply Q;   
            = 'T':  Transpose, apply Q**T.   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines   
            the matrix Q.   
            If SIDE = 'L', M >= K >= 0;   
            if SIDE = 'R', N >= K >= 0.   

    A       (input) REAL array, dimension   
                                 (LDA,M) if SIDE = 'L',   
                                 (LDA,N) if SIDE = 'R'   
            The i-th row must contain the vector which defines the   
            elementary reflector H(i), for i = 1,2,...,k, as returned by   
            SGELQF in the first k rows of its array argument A.   
            A is modified by the routine but restored on exit.   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= max(1,K).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGELQF.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the M-by-N matrix C.   
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            If SIDE = 'L', LWORK >= max(1,N);   
            if SIDE = 'R', LWORK >= max(1,M).   
            For optimum performance LWORK >= N*NB if SIDE = 'L', and   
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal   
            blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__2 = 2;
    static integer c__65 = 65;

    typedef const char *address;    
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4, 
	    i__5;
    char ch__1[2];
    /* Builtin functions   
       Subroutine */ int s_cat(char *, const char **, integer *, integer *, ftnlen);
    /* Local variables */
    static logical left;
    static integer i__;
    static real t[4160]	/* was [65][64] */;
    extern logical lsame_(const char *, const char *);
    static integer nbmin, iinfo, i1, i2, i3, ib, ic, jc;
    extern /* Subroutine */ int sorml2_(const char *, const char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *);
    static integer nb, mi, ni, nq, nw;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static logical notran;
    static integer ldwork;
    static char transt[1];
    static integer lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if (! left && ! lsame_(side, "R")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T")) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < f2cmax(1,*k)) {
	*info = -7;
    } else if (*ldc < f2cmax(1,*m)) {
	*info = -10;
    } else if (*lwork < f2cmax(1,nw) && ! lquery) {
	*info = -12;
    }

    if (*info == 0) {

/*        Determine the block size.  NB may be at most NBMAX, where NBMAX   
          is used to define the local array T.   

   Computing MIN   
   Writing concatenation */
	i__3[0] = 1, a__1[0] = side;
	i__3[1] = 1, a__1[1] = trans;
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	i__1 = 64, i__2 = ilaenv_(&c__1, "SORMLQ", ch__1, m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)2);
	nb = f2cmin(i__1,i__2);
	lwkopt = f2cmax(1,nw) * nb;
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORMLQ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    ldwork = nw;
    if (nb > 1 && nb < *k) {
	iws = nw * nb;
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/* Computing MAX   
   Writing concatenation */
	    i__3[0] = 1, a__1[0] = side;
	    i__3[1] = 1, a__1[1] = trans;
	    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	    i__1 = 2, i__2 = ilaenv_(&c__2, "SORMLQ", ch__1, m, n, k, &c_n1, (
		    ftnlen)6, (ftnlen)2);
	    nbmin = f2cmax(i__1,i__2);
	}
    } else {
	iws = nw;
    }

    if (nb < nbmin || nb >= *k) {

/*        Use unblocked code */

	sorml2_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		c_offset], ldc, &work[1], &iinfo);
    } else {

/*        Use blocked code */

	if (left && notran || ! left && ! notran) {
	    i1 = 1;
	    i2 = *k;
	    i3 = nb;
	} else {
	    i1 = (*k - 1) / nb * nb + 1;
	    i2 = 1;
	    i3 = -nb;
	}

	if (left) {
	    ni = *n;
	    jc = 1;
	} else {
	    mi = *m;
	    ic = 1;
	}

	if (notran) {
	    *(unsigned char *)transt = 'T';
	} else {
	    *(unsigned char *)transt = 'N';
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__4 = nb, i__5 = *k - i__ + 1;
	    ib = f2cmin(i__4,i__5);

/*           Form the triangular factor of the block reflector   
             H = H(i) H(i+1) . . . H(i+ib-1) */

	    i__4 = nq - i__ + 1;
	    slarft_("Forward", "Rowwise", &i__4, &ib, &a_ref(i__, i__), lda, &
		    tau[i__], t, &c__65);
	    if (left) {

/*              H or H' is applied to C(i:m,1:n) */

		mi = *m - i__ + 1;
		ic = i__;
	    } else {

/*              H or H' is applied to C(1:m,i:n) */

		ni = *n - i__ + 1;
		jc = i__;
	    }

/*           Apply H or H' */

	    slarfb_(side, transt, "Forward", "Rowwise", &mi, &ni, &ib, &a_ref(
		    i__, i__), lda, t, &c__65, &c___ref(ic, jc), ldc, &work[1]
		    , &ldwork);
/* L10: */
	}
    }
    work[1] = (real) lwkopt;
    return 0;

/*     End of SORMLQ */

} /* sormlq_ */

#undef c___ref
#undef a_ref

//======================================================

/* Subroutine */ int sormqr_(const char *side, const char *trans, integer *m, integer *n, 
	integer *k, real *a, integer *lda, real *tau, real *c__, integer *ldc,
	 real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORMQR overwrites the general real M-by-N matrix C with   

                    SIDE = 'L'     SIDE = 'R'   
    TRANS = 'N':      Q * C          C * Q   
    TRANS = 'T':      Q**T * C       C * Q**T   

    where Q is a real orthogonal matrix defined as the product of k   
    elementary reflectors   

          Q = H(1) H(2) . . . H(k)   

    as returned by SGEQRF. Q is of order M if SIDE = 'L' and of order N   
    if SIDE = 'R'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q**T from the Left;   
            = 'R': apply Q or Q**T from the Right.   

    TRANS   (input) CHARACTER*1   
            = 'N':  No transpose, apply Q;   
            = 'T':  Transpose, apply Q**T.   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines   
            the matrix Q.   
            If SIDE = 'L', M >= K >= 0;   
            if SIDE = 'R', N >= K >= 0.   

    A       (input) REAL array, dimension (LDA,K)   
            The i-th column must contain the vector which defines the   
            elementary reflector H(i), for i = 1,2,...,k, as returned by   
            SGEQRF in the first k columns of its array argument A.   
            A is modified by the routine but restored on exit.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.   
            If SIDE = 'L', LDA >= max(1,M);   
            if SIDE = 'R', LDA >= max(1,N).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQRF.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the M-by-N matrix C.   
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            If SIDE = 'L', LWORK >= max(1,N);   
            if SIDE = 'R', LWORK >= max(1,M).   
            For optimum performance LWORK >= N*NB if SIDE = 'L', and   
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal   
            blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__2 = 2;
    static integer c__65 = 65;
    
    /* System generated locals */
    typedef const char *address;
    address a__1[2];
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2, i__3[2], i__4, 
	    i__5;
    char ch__1[2];
    /* Builtin functions   
       Subroutine */ int s_cat(char *, const char **, integer *, integer *, ftnlen);
    /* Local variables */
    static logical left;
    static integer i__;
    static real t[4160]	/* was [65][64] */;
    extern logical lsame_(const char *, const char *);
    static integer nbmin, iinfo, i1, i2, i3, ib, ic, jc, nb;
    extern /* Subroutine */ int sorm2r_(const char *, const char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *);
    static integer mi, ni, nq, nw;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static logical notran;
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if (! left && ! lsame_(side, "R")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T")) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < f2cmax(1,nq)) {
	*info = -7;
    } else if (*ldc < f2cmax(1,*m)) {
	*info = -10;
    } else if (*lwork < f2cmax(1,nw) && ! lquery) {
	*info = -12;
    }

    if (*info == 0) {

/*        Determine the block size.  NB may be at most NBMAX, where NBMAX   
          is used to define the local array T.   

   Computing MIN   
   Writing concatenation */
	i__3[0] = 1, a__1[0] = side;
	i__3[1] = 1, a__1[1] = trans;
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	i__1 = 64, i__2 = ilaenv_(&c__1, "SORMQR", ch__1, m, n, k, &c_n1, (
		ftnlen)6, (ftnlen)2);
	nb = f2cmin(i__1,i__2);
	lwkopt = f2cmax(1,nw) * nb;
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORMQR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    ldwork = nw;
    if (nb > 1 && nb < *k) {
	iws = nw * nb;
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/* Computing MAX   
   Writing concatenation */
	    i__3[0] = 1, a__1[0] = side;
	    i__3[1] = 1, a__1[1] = trans;
	    s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)2);
	    i__1 = 2, i__2 = ilaenv_(&c__2, "SORMQR", ch__1, m, n, k, &c_n1, (
		    ftnlen)6, (ftnlen)2);
	    nbmin = f2cmax(i__1,i__2);
	}
    } else {
	iws = nw;
    }

    if (nb < nbmin || nb >= *k) {

/*        Use unblocked code */

	sorm2r_(side, trans, m, n, k, &a[a_offset], lda, &tau[1], &c__[
		c_offset], ldc, &work[1], &iinfo);
    } else {

/*        Use blocked code */

	if (left && ! notran || ! left && notran) {
	    i1 = 1;
	    i2 = *k;
	    i3 = nb;
	} else {
	    i1 = (*k - 1) / nb * nb + 1;
	    i2 = 1;
	    i3 = -nb;
	}

	if (left) {
	    ni = *n;
	    jc = 1;
	} else {
	    mi = *m;
	    ic = 1;
	}

	i__1 = i2;
	i__2 = i3;
	for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__4 = nb, i__5 = *k - i__ + 1;
	    ib = f2cmin(i__4,i__5);

/*           Form the triangular factor of the block reflector   
             H = H(i) H(i+1) . . . H(i+ib-1) */

	    i__4 = nq - i__ + 1;
	    slarft_("Forward", "Columnwise", &i__4, &ib, &a_ref(i__, i__), 
		    lda, &tau[i__], t, &c__65);
	    if (left) {

/*              H or H' is applied to C(i:m,1:n) */

		mi = *m - i__ + 1;
		ic = i__;
	    } else {

/*              H or H' is applied to C(1:m,i:n) */

		ni = *n - i__ + 1;
		jc = i__;
	    }

/*           Apply H or H' */

	    slarfb_(side, trans, "Forward", "Columnwise", &mi, &ni, &ib, &
		    a_ref(i__, i__), lda, t, &c__65, &c___ref(ic, jc), ldc, &
		    work[1], &ldwork);
/* L10: */
	}
    }
    work[1] = (real) lwkopt;
    return 0;

/*     End of SORMQR */

} /* sormqr_ */

#undef c___ref
#undef a_ref

//====================================================

/* Subroutine */ int sgelq2_(integer *m, integer *n, real *a, integer *lda, 
	real *tau, real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SGELQ2 computes an LQ factorization of a real m by n matrix A:   
    A = L * Q.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the m by n matrix A.   
            On exit, the elements on and below the diagonal of the array   
            contain the m by min(m,n) lower trapezoidal matrix L (L is   
            lower triangular if m <= n); the elements above the diagonal,   
            with the array TAU, represent the orthogonal matrix Q as a   
            product of elementary reflectors (see Further Details).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    TAU     (output) REAL array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    WORK    (workspace) REAL array, dimension (M)   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument had an illegal value   

    Further Details   
    ===============   

    The matrix Q is represented as a product of elementary reflectors   

       Q = H(k) . . . H(2) H(1), where k = min(m,n).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n),   
    and tau in TAU(i).   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, k;
    extern /* Subroutine */ int slarf_(const char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *), xerbla_(
	    const char *, integer *), slarfg_(integer *, real *, real *, 
	    integer *, real *);
    static real aii;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGELQ2", &i__1);
	return 0;
    }

    k = f2cmin(*m,*n);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Generate elementary reflector H(i) to annihilate A(i,i+1:n)   

   Computing MIN */
	i__2 = i__ + 1;
	i__3 = *n - i__ + 1;
	slarfg_(&i__3, &a_ref(i__, i__), &a_ref(i__, f2cmin(i__2,*n)), lda, &tau[
		i__]);
	if (i__ < *m) {

/*           Apply H(i) to A(i+1:m,i:n) from the right */

	    aii = a_ref(i__, i__);
	    a_ref(i__, i__) = 1.f;
	    i__2 = *m - i__;
	    i__3 = *n - i__ + 1;
	    slarf_("Right", &i__2, &i__3, &a_ref(i__, i__), lda, &tau[i__], &
		    a_ref(i__ + 1, i__), lda, &work[1]);
	    a_ref(i__, i__) = aii;
	}
/* L10: */
    }
    return 0;

/*     End of SGELQ2 */

} /* sgelq2_ */

#undef a_ref

//=============================================

/*  -- translated by f2c (version 19990503).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

/* Table of constant values */

static doublereal c_b15 = -.125;
//static integer c__1 = 1;
static real c_b49 = 1.f;
static real c_b72 = -1.f;

/* Subroutine */ int sbdsqr_(const char *uplo, integer *n, integer *ncvt, integer *
	nru, integer *ncc, real *d__, real *e, real *vt, integer *ldvt, real *
	u, integer *ldu, real *c__, integer *ldc, real *work, integer *info)
{
    /* System generated locals */
    integer c_dim1, c_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1, 
	    i__2;
    real r__1, r__2, r__3, r__4;
    doublereal d__1;

    /* Builtin functions */
    //    double pow_dd(doublereal *, doublereal *), sqrt(doublereal), r_sign(real *, real *);

    /* Local variables */
    static real abse;
    static integer idir;
    static real abss;
    static integer oldm;
    static real cosl;
    static integer isub, iter;
    static real unfl, sinl, cosr, smin, smax, sinr;
    extern /* Subroutine */ int srot_(integer *, real *, integer *, real *, 
	    integer *, real *, real *), slas2_(real *, real *, real *, real *,
	     real *);
    static real f, g, h__;
    static integer i__, j, m;
    static real r__;
    extern logical lsame_(const char *, const char *);
    static real oldcs;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static integer oldll;
    static real shift, sigmn, oldsn;
    static integer maxit;
    static real sminl;
    extern /* Subroutine */ int slasr_(const char *, const char *, const char *, integer *, 
	    integer *, real *, real *, real *, integer *);
    static real sigmx;
    static logical lower;
    extern /* Subroutine */ int sswap_(integer *, real *, integer *, real *, 
	    integer *), slasq1_(integer *, real *, real *, real *, integer *),
	     slasv2_(real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *);
    static real cs;
    static integer ll;
    static real sn, mu;
    extern doublereal slamch_(const char *);
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static real sminoa;
    extern /* Subroutine */ int slartg_(real *, real *, real *, real *, real *
	    );
    static real thresh;
    static logical rotate;
    static real sminlo;
    static integer nm1;
    static real tolmul;
    static integer nm12, nm13, lll;
    static real eps, sll, tol;


#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]
#define u_ref(a_1,a_2) u[(a_2)*u_dim1 + a_1]
#define vt_ref(a_1,a_2) vt[(a_2)*vt_dim1 + a_1]


/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SBDSQR computes the singular value decomposition (SVD) of a real   
    N-by-N (upper or lower) bidiagonal matrix B:  B = Q * S * P' (P'   
    denotes the transpose of P), where S is a diagonal matrix with   
    non-negative diagonal elements (the singular values of B), and Q   
    and P are orthogonal matrices.   

    The routine computes S, and optionally computes U * Q, P' * VT,   
    or Q' * C, for given real input matrices U, VT, and C.   

    See "Computing  Small Singular Values of Bidiagonal Matrices With   
    Guaranteed High Relative Accuracy," by J. Demmel and W. Kahan,   
    LAPACK Working Note #3 (or SIAM J. Sci. Statist. Comput. vol. 11,   
    no. 5, pp. 873-912, Sept 1990) and   
    "Accurate singular values and differential qd algorithms," by   
    B. Parlett and V. Fernando, Technical Report CPAM-554, Mathematics   
    Department, University of California at Berkeley, July 1992   
    for a detailed description of the algorithm.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            = 'U':  B is upper bidiagonal;   
            = 'L':  B is lower bidiagonal.   

    N       (input) INTEGER   
            The order of the matrix B.  N >= 0.   

    NCVT    (input) INTEGER   
            The number of columns of the matrix VT. NCVT >= 0.   

    NRU     (input) INTEGER   
            The number of rows of the matrix U. NRU >= 0.   

    NCC     (input) INTEGER   
            The number of columns of the matrix C. NCC >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the n diagonal elements of the bidiagonal matrix B.   
            On exit, if INFO=0, the singular values of B in decreasing   
            order.   

    E       (input/output) REAL array, dimension (N)   
            On entry, the elements of E contain the   
            offdiagonal elements of the bidiagonal matrix whose SVD   
            is desired. On normal exit (INFO = 0), E is destroyed.   
            If the algorithm does not converge (INFO > 0), D and E   
            will contain the diagonal and superdiagonal elements of a   
            bidiagonal matrix orthogonally equivalent to the one given   
            as input. E(N) is used for workspace.   

    VT      (input/output) REAL array, dimension (LDVT, NCVT)   
            On entry, an N-by-NCVT matrix VT.   
            On exit, VT is overwritten by P' * VT.   
            VT is not referenced if NCVT = 0.   

    LDVT    (input) INTEGER   
            The leading dimension of the array VT.   
            LDVT >= max(1,N) if NCVT > 0; LDVT >= 1 if NCVT = 0.   

    U       (input/output) REAL array, dimension (LDU, N)   
            On entry, an NRU-by-N matrix U.   
            On exit, U is overwritten by U * Q.   
            U is not referenced if NRU = 0.   

    LDU     (input) INTEGER   
            The leading dimension of the array U.  LDU >= max(1,NRU).   

    C       (input/output) REAL array, dimension (LDC, NCC)   
            On entry, an N-by-NCC matrix C.   
            On exit, C is overwritten by Q' * C.   
            C is not referenced if NCC = 0.   

    LDC     (input) INTEGER   
            The leading dimension of the array C.   
            LDC >= max(1,N) if NCC > 0; LDC >=1 if NCC = 0.   

    WORK    (workspace) REAL array, dimension (4*N)   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  If INFO = -i, the i-th argument had an illegal value   
            > 0:  the algorithm did not converge; D and E contain the   
                  elements of a bidiagonal matrix which is orthogonally   
                  similar to the input matrix B;  if INFO = i, i   
                  elements of E have not converged to zero.   

    Internal Parameters   
    ===================   

    TOLMUL  REAL, default = max(10,min(100,EPS**(-1/8)))   
            TOLMUL controls the convergence criterion of the QR loop.   
            If it is positive, TOLMUL*EPS is the desired relative   
               precision in the computed singular values.   
            If it is negative, abs(TOLMUL*EPS*sigma_max) is the   
               desired absolute accuracy in the computed singular   
               values (corresponds to relative accuracy   
               abs(TOLMUL*EPS) in the largest singular value.   
            abs(TOLMUL) should be between 1 and 1/EPS, and preferably   
               between 10 (for fast convergence) and .1/EPS   
               (for there to be some accuracy in the results).   
            Default is to lose at either one eighth or 2 of the   
               available decimal digits in each computed singular value   
               (whichever is smaller).   

    MAXITR  INTEGER, default = 6   
            MAXITR controls the maximum number of passes of the   
            algorithm through its inner loop. The algorithms stops   
            (and so fails to converge) if the number of passes   
            through the inner loop exceeds MAXITR*N**2.   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    --d__;
    --e;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1 * 1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1 * 1;
    u -= u_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    lower = lsame_(uplo, "L");
    if (! lsame_(uplo, "U") && ! lower) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ncvt < 0) {
	*info = -3;
    } else if (*nru < 0) {
	*info = -4;
    } else if (*ncc < 0) {
	*info = -5;
    } else if (*ncvt == 0 && *ldvt < 1 || *ncvt > 0 && *ldvt < f2cmax(1,*n)) {
	*info = -9;
    } else if (*ldu < f2cmax(1,*nru)) {
	*info = -11;
    } else if (*ncc == 0 && *ldc < 1 || *ncc > 0 && *ldc < f2cmax(1,*n)) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SBDSQR", &i__1);
	return 0;
    }
    if (*n == 0) {
	return 0;
    }
    if (*n == 1) {
	goto L160;
    }

/*     ROTATE is true if any singular vectors desired, false otherwise */

    rotate = *ncvt > 0 || *nru > 0 || *ncc > 0;

/*     If no singular vectors desired, use qd algorithm */

    if (! rotate) {
	slasq1_(n, &d__[1], &e[1], &work[1], info);
	return 0;
    }

    nm1 = *n - 1;
    nm12 = nm1 + nm1;
    nm13 = nm12 + nm1;
    idir = 0;

/*     Get machine constants */

    eps = slamch_("Epsilon");
    unfl = slamch_("Safe minimum");

/*     If matrix lower bidiagonal, rotate to be upper bidiagonal   
       by applying Givens rotations on the left */

    if (lower) {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    slartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    work[i__] = cs;
	    work[nm1 + i__] = sn;
/* L10: */
	}

/*        Update singular vectors if desired */

	if (*nru > 0) {
	    slasr_("R", "V", "F", nru, n, &work[1], &work[*n], &u[u_offset], 
		    ldu);
	}
	if (*ncc > 0) {
	    slasr_("L", "V", "F", n, ncc, &work[1], &work[*n], &c__[c_offset],
		     ldc);
	}
    }

/*     Compute singular values to relative accuracy TOL   
       (By setting TOL to be negative, algorithm will compute   
       singular values to absolute accuracy ABS(TOL)*norm(input matrix))   

   Computing MAX   
   Computing MIN */
    d__1 = (doublereal) eps;
//chao changed pow_dd to pow
    r__3 = 100.f, r__4 = pow(d__1, c_b15);
    r__1 = 10.f, r__2 = df2cmin(r__3,r__4);
    tolmul = df2cmax(r__1,r__2);
    tol = tolmul * eps;

/*     Compute approximate maximum, minimum singular values */

    smax = 0.f;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__2 = smax, r__3 = (r__1 = d__[i__], dabs(r__1));
	smax = df2cmax(r__2,r__3);
/* L20: */
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__2 = smax, r__3 = (r__1 = e[i__], dabs(r__1));
	smax = df2cmax(r__2,r__3);
/* L30: */
    }
    sminl = 0.f;
    if (tol >= 0.f) {

/*        Relative accuracy desired */

	sminoa = dabs(d__[1]);
	if (sminoa == 0.f) {
	    goto L50;
	}
	mu = sminoa;
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    mu = (r__2 = d__[i__], dabs(r__2)) * (mu / (mu + (r__1 = e[i__ - 
		    1], dabs(r__1))));
	    sminoa = df2cmin(sminoa,mu);
	    if (sminoa == 0.f) {
		goto L50;
	    }
/* L40: */
	}
L50:
	sminoa /= sqrt((real) (*n));
/* Computing MAX */
	r__1 = tol * sminoa, r__2 = *n * 6 * *n * unfl;
	thresh = df2cmax(r__1,r__2);
    } else {

/*        Absolute accuracy desired   

   Computing MAX */
	r__1 = dabs(tol) * smax, r__2 = *n * 6 * *n * unfl;
	thresh = df2cmax(r__1,r__2);
    }

/*     Prepare for main iteration loop for the singular values   
       (MAXIT is the maximum number of passes through the inner   
       loop permitted before nonconvergence signalled.) */

    maxit = *n * 6 * *n;
    iter = 0;
    oldll = -1;
    oldm = -1;

/*     M points to last element of unconverged part of matrix */

    m = *n;

/*     Begin main iteration loop */

L60:

/*     Check for convergence or exceeding iteration count */

    if (m <= 1) {
	goto L160;
    }
    if (iter > maxit) {
	goto L200;
    }

/*     Find diagonal block of matrix to work on */

    if (tol < 0.f && (r__1 = d__[m], dabs(r__1)) <= thresh) {
	d__[m] = 0.f;
    }
    smax = (r__1 = d__[m], dabs(r__1));
    smin = smax;
    i__1 = m - 1;
    for (lll = 1; lll <= i__1; ++lll) {
	ll = m - lll;
	abss = (r__1 = d__[ll], dabs(r__1));
	abse = (r__1 = e[ll], dabs(r__1));
	if (tol < 0.f && abss <= thresh) {
	    d__[ll] = 0.f;
	}
	if (abse <= thresh) {
	    goto L80;
	}
	smin = df2cmin(smin,abss);
/* Computing MAX */
	r__1 = f2cmax(smax,abss);
	smax = df2cmax(r__1,abse);
/* L70: */
    }
    ll = 0;
    goto L90;
L80:
    e[ll] = 0.f;

/*     Matrix splits since E(LL) = 0 */

    if (ll == m - 1) {

/*        Convergence of bottom singular value, return to top of loop */

	--m;
	goto L60;
    }
L90:
    ++ll;

/*     E(LL) through E(M-1) are nonzero, E(LL-1) is zero */

    if (ll == m - 1) {

/*        2 by 2 block, handle separately */

	slasv2_(&d__[m - 1], &e[m - 1], &d__[m], &sigmn, &sigmx, &sinr, &cosr,
		 &sinl, &cosl);
	d__[m - 1] = sigmx;
	e[m - 1] = 0.f;
	d__[m] = sigmn;

/*        Compute singular vectors, if desired */

	if (*ncvt > 0) {
	    srot_(ncvt, &vt_ref(m - 1, 1), ldvt, &vt_ref(m, 1), ldvt, &cosr, &
		    sinr);
	}
	if (*nru > 0) {
	    srot_(nru, &u_ref(1, m - 1), &c__1, &u_ref(1, m), &c__1, &cosl, &
		    sinl);
	}
	if (*ncc > 0) {
	    srot_(ncc, &c___ref(m - 1, 1), ldc, &c___ref(m, 1), ldc, &cosl, &
		    sinl);
	}
	m += -2;
	goto L60;
    }

/*     If working on new submatrix, choose shift direction   
       (from larger end diagonal element towards smaller) */

    if (ll > oldm || m < oldll) {
	if ((r__1 = d__[ll], dabs(r__1)) >= (r__2 = d__[m], dabs(r__2))) {

/*           Chase bulge from top (big end) to bottom (small end) */

	    idir = 1;
	} else {

/*           Chase bulge from bottom (big end) to top (small end) */

	    idir = 2;
	}
    }

/*     Apply convergence tests */

    if (idir == 1) {

/*        Run convergence test in forward direction   
          First apply standard test to bottom of matrix */

	if ((r__2 = e[m - 1], dabs(r__2)) <= dabs(tol) * (r__1 = d__[m], dabs(
		r__1)) || tol < 0.f && (r__3 = e[m - 1], dabs(r__3)) <= 
		thresh) {
	    e[m - 1] = 0.f;
	    goto L60;
	}

	if (tol >= 0.f) {

/*           If relative accuracy desired,   
             apply convergence criterion forward */

	    mu = (r__1 = d__[ll], dabs(r__1));
	    sminl = mu;
	    i__1 = m - 1;
	    for (lll = ll; lll <= i__1; ++lll) {
		if ((r__1 = e[lll], dabs(r__1)) <= tol * mu) {
		    e[lll] = 0.f;
		    goto L60;
		}
		sminlo = sminl;
		mu = (r__2 = d__[lll + 1], dabs(r__2)) * (mu / (mu + (r__1 = 
			e[lll], dabs(r__1))));
		sminl = df2cmin(sminl,mu);
/* L100: */
	    }
	}

    } else {

/*        Run convergence test in backward direction   
          First apply standard test to top of matrix */

	if ((r__2 = e[ll], dabs(r__2)) <= dabs(tol) * (r__1 = d__[ll], dabs(
		r__1)) || tol < 0.f && (r__3 = e[ll], dabs(r__3)) <= thresh) {
	    e[ll] = 0.f;
	    goto L60;
	}

	if (tol >= 0.f) {

/*           If relative accuracy desired,   
             apply convergence criterion backward */

	    mu = (r__1 = d__[m], dabs(r__1));
	    sminl = mu;
	    i__1 = ll;
	    for (lll = m - 1; lll >= i__1; --lll) {
		if ((r__1 = e[lll], dabs(r__1)) <= tol * mu) {
		    e[lll] = 0.f;
		    goto L60;
		}
		sminlo = sminl;
		mu = (r__2 = d__[lll], dabs(r__2)) * (mu / (mu + (r__1 = e[
			lll], dabs(r__1))));
		sminl = df2cmin(sminl,mu);
/* L110: */
	    }
	}
    }
    oldll = ll;
    oldm = m;

/*     Compute shift.  First, test if shifting would ruin relative   
       accuracy, and if so set the shift to zero.   

   Computing MAX */
    r__1 = eps, r__2 = tol * .01f;
    if (tol >= 0.f && *n * tol * (sminl / smax) <= df2cmax(r__1,r__2)) {

/*        Use a zero shift to avoid loss of relative accuracy */

	shift = 0.f;
    } else {

/*        Compute the shift from 2-by-2 block at end of matrix */

	if (idir == 1) {
	    sll = (r__1 = d__[ll], dabs(r__1));
	    slas2_(&d__[m - 1], &e[m - 1], &d__[m], &shift, &r__);
	} else {
	    sll = (r__1 = d__[m], dabs(r__1));
	    slas2_(&d__[ll], &e[ll], &d__[ll + 1], &shift, &r__);
	}

/*        Test if shift negligible, and if so set to zero */

	if (sll > 0.f) {
/* Computing 2nd power */
	    r__1 = shift / sll;
	    if (r__1 * r__1 < eps) {
		shift = 0.f;
	    }
	}
    }

/*     Increment iteration count */

    iter = iter + m - ll;

/*     If SHIFT = 0, do simplified QR iteration */

    if (shift == 0.f) {
	if (idir == 1) {

/*           Chase bulge from top to bottom   
             Save cosines and sines for later singular vector updates */

	    cs = 1.f;
	    oldcs = 1.f;
	    i__1 = m - 1;
	    for (i__ = ll; i__ <= i__1; ++i__) {
		r__1 = d__[i__] * cs;
		slartg_(&r__1, &e[i__], &cs, &sn, &r__);
		if (i__ > ll) {
		    e[i__ - 1] = oldsn * r__;
		}
		r__1 = oldcs * r__;
		r__2 = d__[i__ + 1] * sn;
		slartg_(&r__1, &r__2, &oldcs, &oldsn, &d__[i__]);
		work[i__ - ll + 1] = cs;
		work[i__ - ll + 1 + nm1] = sn;
		work[i__ - ll + 1 + nm12] = oldcs;
		work[i__ - ll + 1 + nm13] = oldsn;
/* L120: */
	    }
	    h__ = d__[m] * cs;
	    d__[m] = h__ * oldcs;
	    e[m - 1] = h__ * oldsn;

/*           Update singular vectors */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "F", &i__1, ncvt, &work[1], &work[*n], &
			vt_ref(ll, 1), ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		slasr_("R", "V", "F", nru, &i__1, &work[nm12 + 1], &work[nm13 
			+ 1], &u_ref(1, ll), ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "F", &i__1, ncc, &work[nm12 + 1], &work[nm13 
			+ 1], &c___ref(ll, 1), ldc);
	    }

/*           Test convergence */

	    if ((r__1 = e[m - 1], dabs(r__1)) <= thresh) {
		e[m - 1] = 0.f;
	    }

	} else {

/*           Chase bulge from bottom to top   
             Save cosines and sines for later singular vector updates */

	    cs = 1.f;
	    oldcs = 1.f;
	    i__1 = ll + 1;
	    for (i__ = m; i__ >= i__1; --i__) {
		r__1 = d__[i__] * cs;
		slartg_(&r__1, &e[i__ - 1], &cs, &sn, &r__);
		if (i__ < m) {
		    e[i__] = oldsn * r__;
		}
		r__1 = oldcs * r__;
		r__2 = d__[i__ - 1] * sn;
		slartg_(&r__1, &r__2, &oldcs, &oldsn, &d__[i__]);
		work[i__ - ll] = cs;
		work[i__ - ll + nm1] = -sn;
		work[i__ - ll + nm12] = oldcs;
		work[i__ - ll + nm13] = -oldsn;
/* L130: */
	    }
	    h__ = d__[ll] * cs;
	    d__[ll] = h__ * oldcs;
	    e[ll] = h__ * oldsn;

/*           Update singular vectors */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "B", &i__1, ncvt, &work[nm12 + 1], &work[
			nm13 + 1], &vt_ref(ll, 1), ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		slasr_("R", "V", "B", nru, &i__1, &work[1], &work[*n], &u_ref(
			1, ll), ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "B", &i__1, ncc, &work[1], &work[*n], &
			c___ref(ll, 1), ldc);
	    }

/*           Test convergence */

	    if ((r__1 = e[ll], dabs(r__1)) <= thresh) {
		e[ll] = 0.f;
	    }
	}
    } else {

/*        Use nonzero shift */

	if (idir == 1) {

/*           Chase bulge from top to bottom   
             Save cosines and sines for later singular vector updates */

	    f = ((r__1 = d__[ll], dabs(r__1)) - shift) * (r_sign(&c_b49, &d__[
		    ll]) + shift / d__[ll]);
	    g = e[ll];
	    i__1 = m - 1;
	    for (i__ = ll; i__ <= i__1; ++i__) {
		slartg_(&f, &g, &cosr, &sinr, &r__);
		if (i__ > ll) {
		    e[i__ - 1] = r__;
		}
		f = cosr * d__[i__] + sinr * e[i__];
		e[i__] = cosr * e[i__] - sinr * d__[i__];
		g = sinr * d__[i__ + 1];
		d__[i__ + 1] = cosr * d__[i__ + 1];
		slartg_(&f, &g, &cosl, &sinl, &r__);
		d__[i__] = r__;
		f = cosl * e[i__] + sinl * d__[i__ + 1];
		d__[i__ + 1] = cosl * d__[i__ + 1] - sinl * e[i__];
		if (i__ < m - 1) {
		    g = sinl * e[i__ + 1];
		    e[i__ + 1] = cosl * e[i__ + 1];
		}
		work[i__ - ll + 1] = cosr;
		work[i__ - ll + 1 + nm1] = sinr;
		work[i__ - ll + 1 + nm12] = cosl;
		work[i__ - ll + 1 + nm13] = sinl;
/* L140: */
	    }
	    e[m - 1] = f;

/*           Update singular vectors */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "F", &i__1, ncvt, &work[1], &work[*n], &
			vt_ref(ll, 1), ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		slasr_("R", "V", "F", nru, &i__1, &work[nm12 + 1], &work[nm13 
			+ 1], &u_ref(1, ll), ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "F", &i__1, ncc, &work[nm12 + 1], &work[nm13 
			+ 1], &c___ref(ll, 1), ldc);
	    }

/*           Test convergence */

	    if ((r__1 = e[m - 1], dabs(r__1)) <= thresh) {
		e[m - 1] = 0.f;
	    }

	} else {

/*           Chase bulge from bottom to top   
             Save cosines and sines for later singular vector updates */

	    f = ((r__1 = d__[m], dabs(r__1)) - shift) * (r_sign(&c_b49, &d__[
		    m]) + shift / d__[m]);
	    g = e[m - 1];
	    i__1 = ll + 1;
	    for (i__ = m; i__ >= i__1; --i__) {
		slartg_(&f, &g, &cosr, &sinr, &r__);
		if (i__ < m) {
		    e[i__] = r__;
		}
		f = cosr * d__[i__] + sinr * e[i__ - 1];
		e[i__ - 1] = cosr * e[i__ - 1] - sinr * d__[i__];
		g = sinr * d__[i__ - 1];
		d__[i__ - 1] = cosr * d__[i__ - 1];
		slartg_(&f, &g, &cosl, &sinl, &r__);
		d__[i__] = r__;
		f = cosl * e[i__ - 1] + sinl * d__[i__ - 1];
		d__[i__ - 1] = cosl * d__[i__ - 1] - sinl * e[i__ - 1];
		if (i__ > ll + 1) {
		    g = sinl * e[i__ - 2];
		    e[i__ - 2] = cosl * e[i__ - 2];
		}
		work[i__ - ll] = cosr;
		work[i__ - ll + nm1] = -sinr;
		work[i__ - ll + nm12] = cosl;
		work[i__ - ll + nm13] = -sinl;
/* L150: */
	    }
	    e[ll] = f;

/*           Test convergence */

	    if ((r__1 = e[ll], dabs(r__1)) <= thresh) {
		e[ll] = 0.f;
	    }

/*           Update singular vectors if desired */

	    if (*ncvt > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "B", &i__1, ncvt, &work[nm12 + 1], &work[
			nm13 + 1], &vt_ref(ll, 1), ldvt);
	    }
	    if (*nru > 0) {
		i__1 = m - ll + 1;
		slasr_("R", "V", "B", nru, &i__1, &work[1], &work[*n], &u_ref(
			1, ll), ldu);
	    }
	    if (*ncc > 0) {
		i__1 = m - ll + 1;
		slasr_("L", "V", "B", &i__1, ncc, &work[1], &work[*n], &
			c___ref(ll, 1), ldc);
	    }
	}
    }

/*     QR iteration finished, go back and check convergence */

    goto L60;

/*     All singular values converged, so make them positive */

L160:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (d__[i__] < 0.f) {
	    d__[i__] = -d__[i__];

/*           Change sign of singular vectors, if desired */

	    if (*ncvt > 0) {
		sscal_(ncvt, &c_b72, &vt_ref(i__, 1), ldvt);
	    }
	}
/* L170: */
    }

/*     Sort the singular values into decreasing order (insertion sort on   
       singular values, but only one transposition per singular vector) */

    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Scan for smallest D(I) */

	isub = 1;
	smin = d__[1];
	i__2 = *n + 1 - i__;
	for (j = 2; j <= i__2; ++j) {
	    if (d__[j] <= smin) {
		isub = j;
		smin = d__[j];
	    }
/* L180: */
	}
	if (isub != *n + 1 - i__) {

/*           Swap singular values and vectors */

	    d__[isub] = d__[*n + 1 - i__];
	    d__[*n + 1 - i__] = smin;
	    if (*ncvt > 0) {
		sswap_(ncvt, &vt_ref(isub, 1), ldvt, &vt_ref(*n + 1 - i__, 1),
			 ldvt);
	    }
	    if (*nru > 0) {
		sswap_(nru, &u_ref(1, isub), &c__1, &u_ref(1, *n + 1 - i__), &
			c__1);
	    }
	    if (*ncc > 0) {
		sswap_(ncc, &c___ref(isub, 1), ldc, &c___ref(*n + 1 - i__, 1),
			 ldc);
	    }
	}
/* L190: */
    }
    goto L220;

/*     Maximum number of iterations exceeded, failure to converge */

L200:
    *info = 0;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.f) {
	    ++(*info);
	}
/* L210: */
    }
L220:
    return 0;

/*     End of SBDSQR */

} /* sbdsqr_ */

#undef vt_ref
#undef u_ref
#undef c___ref

//==========================================

/* Subroutine */ int sgeqrf_(integer *m, integer *n, real *a, integer *lda, 
	real *tau, real *work, integer *lwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SGEQRF computes a QR factorization of a real M-by-N matrix A:   
    A = Q * R.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the M-by-N matrix A.   
            On exit, the elements on and above the diagonal of the array   
            contain the min(M,N)-by-N upper trapezoidal matrix R (R is   
            upper triangular if m >= n); the elements below the diagonal,   
            with the array TAU, represent the orthogonal matrix Q as a   
            product of min(m,n) elementary reflectors (see Further   
            Details).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    TAU     (output) REAL array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.  LWORK >= max(1,N).   
            For optimum performance LWORK >= N*NB, where NB is   
            the optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    Further Details   
    ===============   

    The matrix Q is represented as a product of elementary reflectors   

       Q = H(1) H(2) . . . H(k), where k = min(m,n).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),   
    and tau in TAU(i).   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    static integer c__3 = 3;
    static integer c__2 = 2;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    /* Local variables */
    static integer i__, k, nbmin, iinfo;
    extern /* Subroutine */ int sgeqr2_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *);
    static integer ib, nb, nx;
    extern /* Subroutine */ int slarfb_(const char *, const char *, const char *, const char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(const char *, const char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *);
    static integer ldwork, lwkopt;
    static logical lquery;
    static integer iws;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "SGEQRF", " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)
	    1);
    lwkopt = *n * nb;
    work[1] = (real) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -4;
    } else if (*lwork < f2cmax(1,*n) && ! lquery) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGEQRF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    k = f2cmin(*m,*n);
    if (k == 0) {
	work[1] = 1.f;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *n;
    if (nb > 1 && nb < k) {

/*        Determine when to cross over from blocked to unblocked code.   

   Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "SGEQRF", " ", m, n, &c_n1, &c_n1, (
		ftnlen)6, (ftnlen)1);
	nx = f2cmax(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and   
                determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "SGEQRF", " ", m, n, &c_n1, &
			c_n1, (ftnlen)6, (ftnlen)1);
		nbmin = f2cmax(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < k && nx < k) {

/*        Use blocked code initially */

	i__1 = k - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = f2cmin(i__3,nb);

/*           Compute the QR factorization of the current block   
             A(i:m,i:i+ib-1) */

	    i__3 = *m - i__ + 1;
	    sgeqr2_(&i__3, &ib, &a_ref(i__, i__), lda, &tau[i__], &work[1], &
		    iinfo);
	    if (i__ + ib <= *n) {

/*              Form the triangular factor of the block reflector   
                H = H(i) H(i+1) . . . H(i+ib-1) */

		i__3 = *m - i__ + 1;
		slarft_("Forward", "Columnwise", &i__3, &ib, &a_ref(i__, i__),
			 lda, &tau[i__], &work[1], &ldwork);

/*              Apply H' to A(i:m,i+ib:n) from the left */

		i__3 = *m - i__ + 1;
		i__4 = *n - i__ - ib + 1;
		slarfb_("Left", "Transpose", "Forward", "Columnwise", &i__3, &
			i__4, &ib, &a_ref(i__, i__), lda, &work[1], &ldwork, &
			a_ref(i__, i__ + ib), lda, &work[ib + 1], &ldwork);
	    }
/* L10: */
	}
    } else {
	i__ = 1;
    }

/*     Use unblocked code to factor the last or only block. */

    if (i__ <= k) {
	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	sgeqr2_(&i__2, &i__1, &a_ref(i__, i__), lda, &tau[i__], &work[1], &
		iinfo);
    }

    work[1] = (real) iws;
    return 0;

/*     End of SGEQRF */

} /* sgeqrf_ */

#undef a_ref


//===============================================

/* Subroutine */ int sorml2_(const char *side, const char *trans, integer *m, integer *n, 
	integer *k, real *a, integer *lda, real *tau, real *c__, integer *ldc,
	 real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SORML2 overwrites the general real m by n matrix C with   

          Q * C  if SIDE = 'L' and TRANS = 'N', or   

          Q'* C  if SIDE = 'L' and TRANS = 'T', or   

          C * Q  if SIDE = 'R' and TRANS = 'N', or   

          C * Q' if SIDE = 'R' and TRANS = 'T',   

    where Q is a real orthogonal matrix defined as the product of k   
    elementary reflectors   

          Q = H(k) . . . H(2) H(1)   

    as returned by SGELQF. Q is of order m if SIDE = 'L' and of order n   
    if SIDE = 'R'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q' from the Left   
            = 'R': apply Q or Q' from the Right   

    TRANS   (input) CHARACTER*1   
            = 'N': apply Q  (No transpose)   
            = 'T': apply Q' (Transpose)   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines   
            the matrix Q.   
            If SIDE = 'L', M >= K >= 0;   
            if SIDE = 'R', N >= K >= 0.   

    A       (input) REAL array, dimension   
                                 (LDA,M) if SIDE = 'L',   
                                 (LDA,N) if SIDE = 'R'   
            The i-th row must contain the vector which defines the   
            elementary reflector H(i), for i = 1,2,...,k, as returned by   
            SGELQF in the first k rows of its array argument A.   
            A is modified by the routine but restored on exit.   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= max(1,K).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGELQF.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace) REAL array, dimension   
                                     (N) if SIDE = 'L',   
                                     (M) if SIDE = 'R'   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* System generated locals */
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;
    /* Local variables */
    static logical left;
    static integer i__;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int slarf_(const char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *);
    static integer i1, i2, i3, ic, jc, mi, ni, nq;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static logical notran;
    static real aii;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]

    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

/*     NQ is the order of Q */

    if (left) {
	nq = *m;
    } else {
	nq = *n;
    }
    if (! left && ! lsame_(side, "R")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T")) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < f2cmax(1,*k)) {
	*info = -7;
    } else if (*ldc < f2cmax(1,*m)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORML2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	return 0;
    }

    if (left && notran || ! left && ! notran) {
	i1 = 1;
	i2 = *k;
	i3 = 1;
    } else {
	i1 = *k;
	i2 = 1;
	i3 = -1;
    }

    if (left) {
	ni = *n;
	jc = 1;
    } else {
	mi = *m;
	ic = 1;
    }

    i__1 = i2;
    i__2 = i3;
    for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	if (left) {

/*           H(i) is applied to C(i:m,1:n) */

	    mi = *m - i__ + 1;
	    ic = i__;
	} else {

/*           H(i) is applied to C(1:m,i:n) */

	    ni = *n - i__ + 1;
	    jc = i__;
	}

/*        Apply H(i) */

	aii = a_ref(i__, i__);
	a_ref(i__, i__) = 1.f;
	slarf_(side, &mi, &ni, &a_ref(i__, i__), lda, &tau[i__], &c___ref(ic, 
		jc), ldc, &work[1]);
	a_ref(i__, i__) = aii;
/* L10: */
    }
    return 0;

/*     End of SORML2 */

} /* sorml2_ */

#undef c___ref
#undef a_ref


//===============================

/* Subroutine */ int slabrd_(integer *m, integer *n, integer *nb, real *a, 
	integer *lda, real *d__, real *e, real *tauq, real *taup, real *x, 
	integer *ldx, real *y, integer *ldy)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLABRD reduces the first NB rows and columns of a real general   
    m by n matrix A to upper or lower bidiagonal form by an orthogonal   
    transformation Q' * A * P, and returns the matrices X and Y which   
    are needed to apply the transformation to the unreduced part of A.   

    If m >= n, A is reduced to upper bidiagonal form; if m < n, to lower   
    bidiagonal form.   

    This is an auxiliary routine called by SGEBRD   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows in the matrix A.   

    N       (input) INTEGER   
            The number of columns in the matrix A.   

    NB      (input) INTEGER   
            The number of leading rows and columns of A to be reduced.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the m by n general matrix to be reduced.   
            On exit, the first NB rows and columns of the matrix are   
            overwritten; the rest of the array is unchanged.   
            If m >= n, elements on and below the diagonal in the first NB   
              columns, with the array TAUQ, represent the orthogonal   
              matrix Q as a product of elementary reflectors; and   
              elements above the diagonal in the first NB rows, with the   
              array TAUP, represent the orthogonal matrix P as a product   
              of elementary reflectors.   
            If m < n, elements below the diagonal in the first NB   
              columns, with the array TAUQ, represent the orthogonal   
              matrix Q as a product of elementary reflectors, and   
              elements on and above the diagonal in the first NB rows,   
              with the array TAUP, represent the orthogonal matrix P as   
              a product of elementary reflectors.   
            See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    D       (output) REAL array, dimension (NB)   
            The diagonal elements of the first NB rows and columns of   
            the reduced matrix.  D(i) = A(i,i).   

    E       (output) REAL array, dimension (NB)   
            The off-diagonal elements of the first NB rows and columns of   
            the reduced matrix.   

    TAUQ    (output) REAL array dimension (NB)   
            The scalar factors of the elementary reflectors which   
            represent the orthogonal matrix Q. See Further Details.   

    TAUP    (output) REAL array, dimension (NB)   
            The scalar factors of the elementary reflectors which   
            represent the orthogonal matrix P. See Further Details.   

    X       (output) REAL array, dimension (LDX,NB)   
            The m-by-nb matrix X required to update the unreduced part   
            of A.   

    LDX     (input) INTEGER   
            The leading dimension of the array X. LDX >= M.   

    Y       (output) REAL array, dimension (LDY,NB)   
            The n-by-nb matrix Y required to update the unreduced part   
            of A.   

    LDY     (output) INTEGER   
            The leading dimension of the array Y. LDY >= N.   

    Further Details   
    ===============   

    The matrices Q and P are represented as products of elementary   
    reflectors:   

       Q = H(1) H(2) . . . H(nb)  and  P = G(1) G(2) . . . G(nb)   

    Each H(i) and G(i) has the form:   

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'   

    where tauq and taup are real scalars, and v and u are real vectors.   

    If m >= n, v(1:i-1) = 0, v(i) = 1, and v(i:m) is stored on exit in   
    A(i:m,i); u(1:i) = 0, u(i+1) = 1, and u(i+1:n) is stored on exit in   
    A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i).   

    If m < n, v(1:i) = 0, v(i+1) = 1, and v(i+1:m) is stored on exit in   
    A(i+2:m,i); u(1:i-1) = 0, u(i) = 1, and u(i:n) is stored on exit in   
    A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i).   

    The elements of the vectors v and u together form the m-by-nb matrix   
    V and the nb-by-n matrix U' which are needed, with X and Y, to apply   
    the transformation to the unreduced part of the matrix, using a block   
    update of the form:  A := A - V*Y' - X*U'.   

    The contents of A on exit are illustrated by the following examples   
    with nb = 2:   

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):   

      (  1   1   u1  u1  u1 )           (  1   u1  u1  u1  u1  u1 )   
      (  v1  1   1   u2  u2 )           (  1   1   u2  u2  u2  u2 )   
      (  v1  v2  a   a   a  )           (  v1  1   a   a   a   a  )   
      (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  )   
      (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  )   
      (  v1  v2  a   a   a  )   

    where a denotes an element of the original matrix which is unchanged,   
    vi denotes an element of the vector defining H(i), and ui an element   
    of the vector defining G(i).   

    =====================================================================   


       Quick return if possible   

       Parameter adjustments */
    /* Table of constant values */
    static real c_b4 = -1.f;
    static real c_b5 = 1.f;
    static integer c__1 = 1;
    static real c_b16 = 0.f;
    
    /* System generated locals */
    integer a_dim1, a_offset, x_dim1, x_offset, y_dim1, y_offset, i__1, i__2, 
	    i__3;
    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    sgemv_(const char *, integer *, integer *, real *, real *, integer *, 
	    real *, integer *, real *, real *, integer *), slarfg_(
	    integer *, real *, real *, integer *, real *);
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define x_ref(a_1,a_2) x[(a_2)*x_dim1 + a_1]
#define y_ref(a_1,a_2) y[(a_2)*y_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1 * 1;
    x -= x_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1 * 1;
    y -= y_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (*m >= *n) {

/*        Reduce to upper bidiagonal form */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i:m,i) */

	    i__2 = *m - i__ + 1;
	    i__3 = i__ - 1;
	    sgemv_("No transpose", &i__2, &i__3, &c_b4, &a_ref(i__, 1), lda, &
		    y_ref(i__, 1), ldy, &c_b5, &a_ref(i__, i__), &c__1);
	    i__2 = *m - i__ + 1;
	    i__3 = i__ - 1;
	    sgemv_("No transpose", &i__2, &i__3, &c_b4, &x_ref(i__, 1), ldx, &
		    a_ref(1, i__), &c__1, &c_b5, &a_ref(i__, i__), &c__1);

/*           Generate reflection Q(i) to annihilate A(i+1:m,i)   

   Computing MIN */
	    i__2 = i__ + 1;
	    i__3 = *m - i__ + 1;
	    slarfg_(&i__3, &a_ref(i__, i__), &a_ref(f2cmin(i__2,*m), i__), &c__1,
		     &tauq[i__]);
	    d__[i__] = a_ref(i__, i__);
	    if (i__ < *n) {
		a_ref(i__, i__) = 1.f;

/*              Compute Y(i+1:n,i) */

		i__2 = *m - i__ + 1;
		i__3 = *n - i__;
		sgemv_("Transpose", &i__2, &i__3, &c_b5, &a_ref(i__, i__ + 1),
			 lda, &a_ref(i__, i__), &c__1, &c_b16, &y_ref(i__ + 1,
			 i__), &c__1);
		i__2 = *m - i__ + 1;
		i__3 = i__ - 1;
		sgemv_("Transpose", &i__2, &i__3, &c_b5, &a_ref(i__, 1), lda, 
			&a_ref(i__, i__), &c__1, &c_b16, &y_ref(1, i__), &
			c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b4, &y_ref(i__ + 1, 1)
			, ldy, &y_ref(1, i__), &c__1, &c_b5, &y_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *m - i__ + 1;
		i__3 = i__ - 1;
		sgemv_("Transpose", &i__2, &i__3, &c_b5, &x_ref(i__, 1), ldx, 
			&a_ref(i__, i__), &c__1, &c_b16, &y_ref(1, i__), &
			c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		sgemv_("Transpose", &i__2, &i__3, &c_b4, &a_ref(1, i__ + 1), 
			lda, &y_ref(1, i__), &c__1, &c_b5, &y_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *n - i__;
		sscal_(&i__2, &tauq[i__], &y_ref(i__ + 1, i__), &c__1);

/*              Update A(i,i+1:n) */

		i__2 = *n - i__;
		sgemv_("No transpose", &i__2, &i__, &c_b4, &y_ref(i__ + 1, 1),
			 ldy, &a_ref(i__, 1), lda, &c_b5, &a_ref(i__, i__ + 1)
			, lda);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		sgemv_("Transpose", &i__2, &i__3, &c_b4, &a_ref(1, i__ + 1), 
			lda, &x_ref(i__, 1), ldx, &c_b5, &a_ref(i__, i__ + 1),
			 lda);

/*              Generate reflection P(i) to annihilate A(i,i+2:n)   

   Computing MIN */
		i__2 = i__ + 2;
		i__3 = *n - i__;
		slarfg_(&i__3, &a_ref(i__, i__ + 1), &a_ref(i__, f2cmin(i__2,*n))
			, lda, &taup[i__]);
		e[i__] = a_ref(i__, i__ + 1);
		a_ref(i__, i__ + 1) = 1.f;

/*              Compute X(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = *n - i__;
		sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(i__ + 1, 
			i__ + 1), lda, &a_ref(i__, i__ + 1), lda, &c_b16, &
			x_ref(i__ + 1, i__), &c__1);
		i__2 = *n - i__;
		sgemv_("Transpose", &i__2, &i__, &c_b5, &y_ref(i__ + 1, 1), 
			ldy, &a_ref(i__, i__ + 1), lda, &c_b16, &x_ref(1, i__)
			, &c__1);
		i__2 = *m - i__;
		sgemv_("No transpose", &i__2, &i__, &c_b4, &a_ref(i__ + 1, 1),
			 lda, &x_ref(1, i__), &c__1, &c_b5, &x_ref(i__ + 1, 
			i__), &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(1, i__ + 1)
			, lda, &a_ref(i__, i__ + 1), lda, &c_b16, &x_ref(1, 
			i__), &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b4, &x_ref(i__ + 1, 1)
			, ldx, &x_ref(1, i__), &c__1, &c_b5, &x_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *m - i__;
		sscal_(&i__2, &taup[i__], &x_ref(i__ + 1, i__), &c__1);
	    }
/* L10: */
	}
    } else {

/*        Reduce to lower bidiagonal form */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i,i:n) */

	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    sgemv_("No transpose", &i__2, &i__3, &c_b4, &y_ref(i__, 1), ldy, &
		    a_ref(i__, 1), lda, &c_b5, &a_ref(i__, i__), lda);
	    i__2 = i__ - 1;
	    i__3 = *n - i__ + 1;
	    sgemv_("Transpose", &i__2, &i__3, &c_b4, &a_ref(1, i__), lda, &
		    x_ref(i__, 1), ldx, &c_b5, &a_ref(i__, i__), lda);

/*           Generate reflection P(i) to annihilate A(i,i+1:n)   

   Computing MIN */
	    i__2 = i__ + 1;
	    i__3 = *n - i__ + 1;
	    slarfg_(&i__3, &a_ref(i__, i__), &a_ref(i__, f2cmin(i__2,*n)), lda, &
		    taup[i__]);
	    d__[i__] = a_ref(i__, i__);
	    if (i__ < *m) {
		a_ref(i__, i__) = 1.f;

/*              Compute X(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = *n - i__ + 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(i__ + 1, 
			i__), lda, &a_ref(i__, i__), lda, &c_b16, &x_ref(i__ 
			+ 1, i__), &c__1);
		i__2 = *n - i__ + 1;
		i__3 = i__ - 1;
		sgemv_("Transpose", &i__2, &i__3, &c_b5, &y_ref(i__, 1), ldy, 
			&a_ref(i__, i__), lda, &c_b16, &x_ref(1, i__), &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b4, &a_ref(i__ + 1, 1)
			, lda, &x_ref(1, i__), &c__1, &c_b5, &x_ref(i__ + 1, 
			i__), &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__ + 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b5, &a_ref(1, i__), 
			lda, &a_ref(i__, i__), lda, &c_b16, &x_ref(1, i__), &
			c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b4, &x_ref(i__ + 1, 1)
			, ldx, &x_ref(1, i__), &c__1, &c_b5, &x_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *m - i__;
		sscal_(&i__2, &taup[i__], &x_ref(i__ + 1, i__), &c__1);

/*              Update A(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b4, &a_ref(i__ + 1, 1)
			, lda, &y_ref(i__, 1), ldy, &c_b5, &a_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *m - i__;
		sgemv_("No transpose", &i__2, &i__, &c_b4, &x_ref(i__ + 1, 1),
			 ldx, &a_ref(1, i__), &c__1, &c_b5, &a_ref(i__ + 1, 
			i__), &c__1);

/*              Generate reflection Q(i) to annihilate A(i+2:m,i)   

   Computing MIN */
		i__2 = i__ + 2;
		i__3 = *m - i__;
		slarfg_(&i__3, &a_ref(i__ + 1, i__), &a_ref(f2cmin(i__2,*m), i__)
			, &c__1, &tauq[i__]);
		e[i__] = a_ref(i__ + 1, i__);
		a_ref(i__ + 1, i__) = 1.f;

/*              Compute Y(i+1:n,i) */

		i__2 = *m - i__;
		i__3 = *n - i__;
		sgemv_("Transpose", &i__2, &i__3, &c_b5, &a_ref(i__ + 1, i__ 
			+ 1), lda, &a_ref(i__ + 1, i__), &c__1, &c_b16, &
			y_ref(i__ + 1, i__), &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		sgemv_("Transpose", &i__2, &i__3, &c_b5, &a_ref(i__ + 1, 1), 
			lda, &a_ref(i__ + 1, i__), &c__1, &c_b16, &y_ref(1, 
			i__), &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b4, &y_ref(i__ + 1, 1)
			, ldy, &y_ref(1, i__), &c__1, &c_b5, &y_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *m - i__;
		sgemv_("Transpose", &i__2, &i__, &c_b5, &x_ref(i__ + 1, 1), 
			ldx, &a_ref(i__ + 1, i__), &c__1, &c_b16, &y_ref(1, 
			i__), &c__1);
		i__2 = *n - i__;
		sgemv_("Transpose", &i__, &i__2, &c_b4, &a_ref(1, i__ + 1), 
			lda, &y_ref(1, i__), &c__1, &c_b5, &y_ref(i__ + 1, 
			i__), &c__1);
		i__2 = *n - i__;
		sscal_(&i__2, &tauq[i__], &y_ref(i__ + 1, i__), &c__1);
	    }
/* L20: */
	}
    }
    return 0;

/*     End of SLABRD */

} /* slabrd_ */

#undef y_ref
#undef x_ref
#undef a_ref

/* Subroutine */ int sgeqr2_(integer *m, integer *n, real *a, integer *lda, 
	real *tau, real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SGEQR2 computes a QR factorization of a real m by n matrix A:   
    A = Q * R.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the m by n matrix A.   
            On exit, the elements on and above the diagonal of the array   
            contain the min(m,n) by n upper trapezoidal matrix R (R is   
            upper triangular if m >= n); the elements below the diagonal,   
            with the array TAU, represent the orthogonal matrix Q as a   
            product of elementary reflectors (see Further Details).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    TAU     (output) REAL array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    WORK    (workspace) REAL array, dimension (N)   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument had an illegal value   

    Further Details   
    ===============   

    The matrix Q is represented as a product of elementary reflectors   

       Q = H(1) H(2) . . . H(k), where k = min(m,n).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),   
    and tau in TAU(i).   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, k;
    extern /* Subroutine */ int slarf_(const char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *), xerbla_(
	    const char *, integer *), slarfg_(integer *, real *, real *, 
	    integer *, real *);
    static real aii;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGEQR2", &i__1);
	return 0;
    }

    k = f2cmin(*m,*n);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Generate elementary reflector H(i) to annihilate A(i+1:m,i)   

   Computing MIN */
	i__2 = i__ + 1;
	i__3 = *m - i__ + 1;
	slarfg_(&i__3, &a_ref(i__, i__), &a_ref(f2cmin(i__2,*m), i__), &c__1, &
		tau[i__]);
	if (i__ < *n) {

/*           Apply H(i) to A(i:m,i+1:n) from the left */

	    aii = a_ref(i__, i__);
	    a_ref(i__, i__) = 1.f;
	    i__2 = *m - i__ + 1;
	    i__3 = *n - i__;
	    slarf_("Left", &i__2, &i__3, &a_ref(i__, i__), &c__1, &tau[i__], &
		    a_ref(i__, i__ + 1), lda, &work[1]);
	    a_ref(i__, i__) = aii;
	}
/* L10: */
    }
    return 0;

/*     End of SGEQR2 */

} /* sgeqr2_ */

#undef a_ref


//================================

/* Subroutine */ int sorm2r_(const char *side, const char *trans, integer *m, integer *n, 
	integer *k, real *a, integer *lda, real *tau, real *c__, integer *ldc,
	 real *work, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SORM2R overwrites the general real m by n matrix C with   

          Q * C  if SIDE = 'L' and TRANS = 'N', or   

          Q'* C  if SIDE = 'L' and TRANS = 'T', or   

          C * Q  if SIDE = 'R' and TRANS = 'N', or   

          C * Q' if SIDE = 'R' and TRANS = 'T',   

    where Q is a real orthogonal matrix defined as the product of k   
    elementary reflectors   

          Q = H(1) H(2) . . . H(k)   

    as returned by SGEQRF. Q is of order m if SIDE = 'L' and of order n   
    if SIDE = 'R'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q' from the Left   
            = 'R': apply Q or Q' from the Right   

    TRANS   (input) CHARACTER*1   
            = 'N': apply Q  (No transpose)   
            = 'T': apply Q' (Transpose)   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines   
            the matrix Q.   
            If SIDE = 'L', M >= K >= 0;   
            if SIDE = 'R', N >= K >= 0.   

    A       (input) REAL array, dimension (LDA,K)   
            The i-th column must contain the vector which defines the   
            elementary reflector H(i), for i = 1,2,...,k, as returned by   
            SGEQRF in the first k columns of its array argument A.   
            A is modified by the routine but restored on exit.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.   
            If SIDE = 'L', LDA >= max(1,M);   
            if SIDE = 'R', LDA >= max(1,N).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQRF.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace) REAL array, dimension   
                                     (N) if SIDE = 'L',   
                                     (M) if SIDE = 'R'   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, c_dim1, c_offset, i__1, i__2;
    /* Local variables */
    static logical left;
    static integer i__;
    extern logical lsame_(const char *, const char *);
    extern /* Subroutine */ int slarf_(const char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *);
    static integer i1, i2, i3, ic, jc, mi, ni, nq;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    static logical notran;
    static real aii;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1 * 1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

/*     NQ is the order of Q */

    if (left) {
	nq = *m;
    } else {
	nq = *n;
    }
    if (! left && ! lsame_(side, "R")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T")) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
	*info = -5;
    } else if (*lda < f2cmax(1,nq)) {
	*info = -7;
    } else if (*ldc < f2cmax(1,*m)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORM2R", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	return 0;
    }

    if (left && ! notran || ! left && notran) {
	i1 = 1;
	i2 = *k;
	i3 = 1;
    } else {
	i1 = *k;
	i2 = 1;
	i3 = -1;
    }

    if (left) {
	ni = *n;
	jc = 1;
    } else {
	mi = *m;
	ic = 1;
    }

    i__1 = i2;
    i__2 = i3;
    for (i__ = i1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	if (left) {

/*           H(i) is applied to C(i:m,1:n) */

	    mi = *m - i__ + 1;
	    ic = i__;
	} else {

/*           H(i) is applied to C(1:m,i:n) */

	    ni = *n - i__ + 1;
	    jc = i__;
	}

/*        Apply H(i) */

	aii = a_ref(i__, i__);
	a_ref(i__, i__) = 1.f;
	slarf_(side, &mi, &ni, &a_ref(i__, i__), &c__1, &tau[i__], &c___ref(
		ic, jc), ldc, &work[1]);
	a_ref(i__, i__) = aii;
/* L10: */
    }
    return 0;

/*     End of SORM2R */

} /* sorm2r_ */

#undef c___ref
#undef a_ref

//============================

/* Subroutine */ int sorgbr_(const char *vect, integer *m, integer *n, integer *k, 
	real *a, integer *lda, real *tau, real *work, integer *lwork, integer 
	*info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    SORGBR generates one of the real orthogonal matrices Q or P**T   
    determined by SGEBRD when reducing a real matrix A to bidiagonal   
    form: A = Q * B * P**T.  Q and P**T are defined as products of   
    elementary reflectors H(i) or G(i) respectively.   

    If VECT = 'Q', A is assumed to have been an M-by-K matrix, and Q   
    is of order M:   
    if m >= k, Q = H(1) H(2) . . . H(k) and SORGBR returns the first n   
    columns of Q, where m >= n >= k;   
    if m < k, Q = H(1) H(2) . . . H(m-1) and SORGBR returns Q as an   
    M-by-M matrix.   

    If VECT = 'P', A is assumed to have been a K-by-N matrix, and P**T   
    is of order N:   
    if k < n, P**T = G(k) . . . G(2) G(1) and SORGBR returns the first m   
    rows of P**T, where n >= m >= k;   
    if k >= n, P**T = G(n-1) . . . G(2) G(1) and SORGBR returns P**T as   
    an N-by-N matrix.   

    Arguments   
    =========   

    VECT    (input) CHARACTER*1   
            Specifies whether the matrix Q or the matrix P**T is   
            required, as defined in the transformation applied by SGEBRD:   
            = 'Q':  generate Q;   
            = 'P':  generate P**T.   

    M       (input) INTEGER   
            The number of rows of the matrix Q or P**T to be returned.   
            M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q or P**T to be returned.   
            N >= 0.   
            If VECT = 'Q', M >= N >= min(M,K);   
            if VECT = 'P', N >= M >= min(N,K).   

    K       (input) INTEGER   
            If VECT = 'Q', the number of columns in the original M-by-K   
            matrix reduced by SGEBRD.   
            If VECT = 'P', the number of rows in the original K-by-N   
            matrix reduced by SGEBRD.   
            K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the vectors which define the elementary reflectors,   
            as returned by SGEBRD.   
            On exit, the M-by-N matrix Q or P**T.   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= max(1,M).   

    TAU     (input) REAL array, dimension   
                                  (min(M,K)) if VECT = 'Q'   
                                  (min(N,K)) if VECT = 'P'   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i) or G(i), which determines Q or P**T, as   
            returned by SGEBRD in its array argument TAUQ or TAUP.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= max(1,min(M,N)).   
            For optimum performance LWORK >= min(M,N)*NB, where NB   
            is the optimal blocksize.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    =====================================================================   


       Test the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static integer c_n1 = -1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    /* Local variables */
    static integer i__, j;
    extern logical lsame_(const char *, const char *);
    static integer iinfo;
    static logical wantq;
    static integer nb, mn;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int sorglq_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *), sorgqr_(
	    integer *, integer *, integer *, real *, integer *, real *, real *
	    , integer *, integer *);
    static integer lwkopt;
    static logical lquery;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    wantq = lsame_(vect, "Q");
    mn = f2cmin(*m,*n);
    lquery = *lwork == -1;
    if (! wantq && ! lsame_(vect, "P")) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*n < 0 || wantq && (*n > *m || *n < f2cmin(*m,*k)) || ! wantq && (
	    *m > *n || *m < f2cmin(*n,*k))) {
	*info = -3;
    } else if (*k < 0) {
	*info = -4;
    } else if (*lda < f2cmax(1,*m)) {
	*info = -6;
    } else if (*lwork < f2cmax(1,mn) && ! lquery) {
	*info = -9;
    }

    if (*info == 0) {
	if (wantq) {
	    nb = ilaenv_(&c__1, "SORGQR", " ", m, n, k, &c_n1, (ftnlen)6, (
		    ftnlen)1);
	} else {
	    nb = ilaenv_(&c__1, "SORGLQ", " ", m, n, k, &c_n1, (ftnlen)6, (
		    ftnlen)1);
	}
	lwkopt = f2cmax(1,mn) * nb;
	work[1] = (real) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGBR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	work[1] = 1.f;
	return 0;
    }

    if (wantq) {

/*        Form Q, determined by a call to SGEBRD to reduce an m-by-k   
          matrix */

	if (*m >= *k) {

/*           If m >= k, assume m >= n >= k */

	    sorgqr_(m, n, k, &a[a_offset], lda, &tau[1], &work[1], lwork, &
		    iinfo);

	} else {

/*           If m < k, assume m = n   

             Shift the vectors which define the elementary reflectors one   
             column to the right, and set the first row and column of Q   
             to those of the unit matrix */

	    for (j = *m; j >= 2; --j) {
		a_ref(1, j) = 0.f;
		i__1 = *m;
		for (i__ = j + 1; i__ <= i__1; ++i__) {
		    a_ref(i__, j) = a_ref(i__, j - 1);
/* L10: */
		}
/* L20: */
	    }
	    a_ref(1, 1) = 1.f;
	    i__1 = *m;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		a_ref(i__, 1) = 0.f;
/* L30: */
	    }
	    if (*m > 1) {

/*              Form Q(2:m,2:m) */

		i__1 = *m - 1;
		i__2 = *m - 1;
		i__3 = *m - 1;
		sorgqr_(&i__1, &i__2, &i__3, &a_ref(2, 2), lda, &tau[1], &
			work[1], lwork, &iinfo);
	    }
	}
    } else {

/*        Form P', determined by a call to SGEBRD to reduce a k-by-n   
          matrix */

	if (*k < *n) {

/*           If k < n, assume k <= m <= n */

	    sorglq_(m, n, k, &a[a_offset], lda, &tau[1], &work[1], lwork, &
		    iinfo);

	} else {

/*           If k >= n, assume m = n   

             Shift the vectors which define the elementary reflectors one   
             row downward, and set the first row and column of P' to   
             those of the unit matrix */

	    a_ref(1, 1) = 1.f;
	    i__1 = *n;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		a_ref(i__, 1) = 0.f;
/* L40: */
	    }
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		for (i__ = j - 1; i__ >= 2; --i__) {
		    a_ref(i__, j) = a_ref(i__ - 1, j);
/* L50: */
		}
		a_ref(1, j) = 0.f;
/* L60: */
	    }
	    if (*n > 1) {

/*              Form P'(2:n,2:n) */

		i__1 = *n - 1;
		i__2 = *n - 1;
		i__3 = *n - 1;
		sorglq_(&i__1, &i__2, &i__3, &a_ref(2, 2), lda, &tau[1], &
			work[1], lwork, &iinfo);
	    }
	}
    }
    work[1] = (real) lwkopt;
    return 0;

/*     End of SORGBR */

} /* sorgbr_ */

#undef a_ref

//===================================
/* Table of constant values */
/*
static integer c__1 = 1;
static integer c__2 = 2;
static integer c__0 = 0;
*/

/* Subroutine */ int slasq1_(integer *n, real *d__, real *e, real *work, 
	integer *info)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2, r__3;

    /* Builtin functions */
    //double sqrt(doublereal);

    /* Local variables */
    extern /* Subroutine */ int slas2_(real *, real *, real *, real *, real *)
	    ;
    static integer i__;
    static real scale;
    static integer iinfo;
    static real sigmn, sigmx;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), slasq2_(integer *, real *, integer *);
    extern doublereal slamch_(const char *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(const char *, integer *), slascl_(
	    const char *, integer *, integer *, real *, real *, integer *, integer *
	    , real *, integer *, integer *), slasrt_(const char *, integer *
	    , real *, integer *);
    static real eps;


/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SLASQ1 computes the singular values of a real N-by-N bidiagonal   
    matrix with diagonal D and off-diagonal E. The singular values   
    are computed to high relative accuracy, in the absence of   
    denormalization, underflow and overflow. The algorithm was first   
    presented in   

    "Accurate singular values and differential qd algorithms" by K. V.   
    Fernando and B. N. Parlett, Numer. Math., Vol-67, No. 2, pp. 191-230,   
    1994,   

    and the present implementation is described in "An implementation of   
    the dqds Algorithm (Positive Case)", LAPACK Working Note.   

    Arguments   
    =========   

    N     (input) INTEGER   
          The number of rows and columns in the matrix. N >= 0.   

    D     (input/output) REAL array, dimension (N)   
          On entry, D contains the diagonal elements of the   
          bidiagonal matrix whose SVD is desired. On normal exit,   
          D contains the singular values in decreasing order.   

    E     (input/output) REAL array, dimension (N)   
          On entry, elements E(1:N-1) contain the off-diagonal elements   
          of the bidiagonal matrix whose SVD is desired.   
          On exit, E is overwritten.   

    WORK  (workspace) REAL array, dimension (4*N)   

    INFO  (output) INTEGER   
          = 0: successful exit   
          < 0: if INFO = -i, the i-th argument had an illegal value   
          > 0: the algorithm failed   
               = 1, a split was marked by a positive value in E   
               = 2, current block of Z not diagonalized after 30*N   
                    iterations (in inner while loop)   
               = 3, termination criterion of outer while loop not met   
                    (program created more than N unreduced blocks)   

    =====================================================================   


       Parameter adjustments */
    --work;
    --e;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -2;
	i__1 = -(*info);
	xerbla_("SLASQ1", &i__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {
	d__[1] = dabs(d__[1]);
	return 0;
    } else if (*n == 2) {
	slas2_(&d__[1], &e[1], &d__[2], &sigmn, &sigmx);
	d__[1] = sigmx;
	d__[2] = sigmn;
	return 0;
    }

/*     Estimate the largest singular value. */

    sigmx = 0.f;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__[i__] = (r__1 = d__[i__], dabs(r__1));
/* Computing MAX */
	r__2 = sigmx, r__3 = (r__1 = e[i__], dabs(r__1));
	sigmx = df2cmax(r__2,r__3);
/* L10: */
    }
    d__[*n] = (r__1 = d__[*n], dabs(r__1));

/*     Early return if SIGMX is zero (matrix is already diagonal). */

    if (sigmx == 0.f) {
	slasrt_("D", n, &d__[1], &iinfo);
	return 0;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__1 = sigmx, r__2 = d__[i__];
	sigmx = df2cmax(r__1,r__2);
/* L20: */
    }

/*     Copy D and E into WORK (in the Z format) and scale (squaring the   
       input data makes scaling by a power of the radix pointless). */

    eps = slamch_("Precision");
    safmin = slamch_("Safe minimum");
    scale = sqrt(eps / safmin);
    scopy_(n, &d__[1], &c__1, &work[1], &c__2);
    i__1 = *n - 1;
    scopy_(&i__1, &e[1], &c__1, &work[2], &c__2);
    i__1 = (*n << 1) - 1;
    i__2 = (*n << 1) - 1;
    slascl_("G", &c__0, &c__0, &sigmx, &scale, &i__1, &c__1, &work[1], &i__2, 
	    &iinfo);

/*     Compute the q's and e's. */

    i__1 = (*n << 1) - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	r__1 = work[i__];
	work[i__] = r__1 * r__1;
/* L30: */
    }
    work[*n * 2] = 0.f;

    slasq2_(n, &work[1], info);

    if (*info == 0) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__] = sqrt(work[i__]);
/* L40: */
	}
	slascl_("G", &c__0, &c__0, &scale, &sigmx, n, &c__1, &d__[1], n, &
		iinfo);
    }

    return 0;

/*     End of SLASQ1 */

} /* slasq1_ */

//===============================================

/* Table of constant values */

static integer c__10 = 10;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__11 = 11;

/* Subroutine */ int slasq2_(integer *n, real *z__, integer *info)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    //double sqrt(doublereal);

    /* Local variables */
    static logical ieee;
    static integer nbig;
    static real dmin__, emin, emax;
    static integer ndiv, iter;
    static real qmin, temp, qmax, zmax;
    static integer splt;
    static real d__, e;
    static integer k;
    static real s, t;
    static integer nfail;
    static real desig, trace, sigma;
    static integer iinfo, i0, i4, n0;
    extern /* Subroutine */ int slasq3_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, integer *, integer *, integer *
	    , logical *);
    static integer pp;
    extern doublereal slamch_(const char *);
    static integer iwhila, iwhilb;
    static real oldemn, safmin;
    extern /* Subroutine */ int xerbla_(const char *, integer *);
    extern integer ilaenv_(integer *, const char *, const char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern /* Subroutine */ int slasrt_(const char *, integer *, real *, integer *);
    static real eps, tol;
    static integer ipn4;
    static real tol2;


/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SLASQ2 computes all the eigenvalues of the symmetric positive   
    definite tridiagonal matrix associated with the qd array Z to high   
    relative accuracy are computed to high relative accuracy, in the   
    absence of denormalization, underflow and overflow.   

    To see the relation of Z to the tridiagonal matrix, let L be a   
    unit lower bidiagonal matrix with subdiagonals Z(2,4,6,,..) and   
    let U be an upper bidiagonal matrix with 1's above and diagonal   
    Z(1,3,5,,..). The tridiagonal is L*U or, if you prefer, the   
    symmetric tridiagonal to which it is similar.   

    Note : SLASQ2 defines a logical variable, IEEE, which is true   
    on machines which follow ieee-754 floating-point standard in their   
    handling of infinities and NaNs, and false otherwise. This variable   
    is passed to SLASQ3.   

    Arguments   
    =========   

    N     (input) INTEGER   
          The number of rows and columns in the matrix. N >= 0.   

    Z     (workspace) REAL array, dimension ( 4*N )   
          On entry Z holds the qd array. On exit, entries 1 to N hold   
          the eigenvalues in decreasing order, Z( 2*N+1 ) holds the   
          trace, and Z( 2*N+2 ) holds the sum of the eigenvalues. If   
          N > 2, then Z( 2*N+3 ) holds the iteration count, Z( 2*N+4 )   
          holds NDIVS/NIN^2, and Z( 2*N+5 ) holds the percentage of   
          shifts that failed.   

    INFO  (output) INTEGER   
          = 0: successful exit   
          < 0: if the i-th argument is a scalar and had an illegal   
               value, then INFO = -i, if the i-th argument is an   
               array and the j-entry had an illegal value, then   
               INFO = -(i*100+j)   
          > 0: the algorithm failed   
                = 1, a split was marked by a positive value in E   
                = 2, current block of Z not diagonalized after 30*N   
                     iterations (in inner while loop)   
                = 3, termination criterion of outer while loop not met   
                     (program created more than N unreduced blocks)   

    Further Details   
    ===============   
    Local Variables: I0:N0 defines a current unreduced segment of Z.   
    The shifts are accumulated in SIGMA. Iteration count is in ITER.   
    Ping-pong is controlled by PP (alternates between 0 and 1).   

    =====================================================================   


       Test the input arguments.   
       (in case SLASQ2 is not called by SLASQ1)   

       Parameter adjustments */
    --z__;

    /* Function Body */
    *info = 0;
    eps = slamch_("Precision");
    safmin = slamch_("Safe minimum");
    tol = eps * 100.f;
/* Computing 2nd power */
    r__1 = tol;
    tol2 = r__1 * r__1;

    if (*n < 0) {
	*info = -1;
	xerbla_("SLASQ2", &c__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {

/*        1-by-1 case. */

	if (z__[1] < 0.f) {
	    *info = -201;
	    xerbla_("SLASQ2", &c__2);
	}
	return 0;
    } else if (*n == 2) {

/*        2-by-2 case. */

	if (z__[2] < 0.f || z__[3] < 0.f) {
	    *info = -2;
	    xerbla_("SLASQ2", &c__2);
	    return 0;
	} else if (z__[3] > z__[1]) {
	    d__ = z__[3];
	    z__[3] = z__[1];
	    z__[1] = d__;
	}
	z__[5] = z__[1] + z__[2] + z__[3];
	if (z__[2] > z__[3] * tol2) {
	    t = (z__[1] - z__[3] + z__[2]) * .5f;
	    s = z__[3] * (z__[2] / t);
	    if (s <= t) {
		s = z__[3] * (z__[2] / (t * (sqrt(s / t + 1.f) + 1.f)));
	    } else {
		s = z__[3] * (z__[2] / (t + sqrt(t) * sqrt(t + s)));
	    }
	    t = z__[1] + (s + z__[2]);
	    z__[3] *= z__[1] / t;
	    z__[1] = t;
	}
	z__[2] = z__[3];
	z__[6] = z__[2] + z__[1];
	return 0;
    }

/*     Check for negative data and compute sums of q's and e's. */

    z__[*n * 2] = 0.f;
    emin = z__[2];
    qmax = 0.f;
    zmax = 0.f;
    d__ = 0.f;
    e = 0.f;

    i__1 = *n - 1 << 1;
    for (k = 1; k <= i__1; k += 2) {
	if (z__[k] < 0.f) {
	    *info = -(k + 200);
	    xerbla_("SLASQ2", &c__2);
	    return 0;
	} else if (z__[k + 1] < 0.f) {
	    *info = -(k + 201);
	    xerbla_("SLASQ2", &c__2);
	    return 0;
	}
	d__ += z__[k];
	e += z__[k + 1];
/* Computing MAX */
	r__1 = qmax, r__2 = z__[k];
	qmax = df2cmax(r__1,r__2);
/* Computing MIN */
	r__1 = emin, r__2 = z__[k + 1];
	emin = df2cmin(r__1,r__2);
/* Computing MAX */
	r__1 = f2cmax(qmax,zmax), r__2 = z__[k + 1];
	zmax = df2cmax(r__1,r__2);
/* L10: */
    }
    if (z__[(*n << 1) - 1] < 0.f) {
	*info = -((*n << 1) + 199);
	xerbla_("SLASQ2", &c__2);
	return 0;
    }
    d__ += z__[(*n << 1) - 1];
/* Computing MAX */
    r__1 = qmax, r__2 = z__[(*n << 1) - 1];
    qmax = df2cmax(r__1,r__2);
    zmax = df2cmax(qmax,zmax);

/*     Check for diagonality. */

    if (e == 0.f) {
	i__1 = *n;
	for (k = 2; k <= i__1; ++k) {
	    z__[k] = z__[(k << 1) - 1];
/* L20: */
	}
	slasrt_("D", n, &z__[1], &iinfo);
	z__[(*n << 1) - 1] = d__;
	return 0;
    }

    trace = d__ + e;

/*     Check for zero data. */

    if (trace == 0.f) {
	z__[(*n << 1) - 1] = 0.f;
	return 0;
    }

/*     Check whether the machine is IEEE conformable. */

    ieee = ilaenv_(&c__10, "SLASQ2", "N", &c__1, &c__2, &c__3, &c__4, (ftnlen)
	    6, (ftnlen)1) == 1 && ilaenv_(&c__11, "SLASQ2", "N", &c__1, &c__2,
	     &c__3, &c__4, (ftnlen)6, (ftnlen)1) == 1;

/*     Rearrange data for locality: Z=(q1,qq1,e1,ee1,q2,qq2,e2,ee2,...). */

    for (k = *n << 1; k >= 2; k += -2) {
	z__[k * 2] = 0.f;
	z__[(k << 1) - 1] = z__[k];
	z__[(k << 1) - 2] = 0.f;
	z__[(k << 1) - 3] = z__[k - 1];
/* L30: */
    }

    i0 = 1;
    n0 = *n;

/*     Reverse the qd-array, if warranted. */

    if (z__[(i0 << 2) - 3] * 1.5f < z__[(n0 << 2) - 3]) {
	ipn4 = i0 + n0 << 2;
	i__1 = i0 + n0 - 1 << 1;
	for (i4 = i0 << 2; i4 <= i__1; i4 += 4) {
	    temp = z__[i4 - 3];
	    z__[i4 - 3] = z__[ipn4 - i4 - 3];
	    z__[ipn4 - i4 - 3] = temp;
	    temp = z__[i4 - 1];
	    z__[i4 - 1] = z__[ipn4 - i4 - 5];
	    z__[ipn4 - i4 - 5] = temp;
/* L40: */
	}
    }

/*     Initial split checking via dqd and Li's test. */

    pp = 0;

    for (k = 1; k <= 2; ++k) {

	d__ = z__[(n0 << 2) + pp - 3];
	i__1 = (i0 << 2) + pp;
	for (i4 = (n0 - 1 << 2) + pp; i4 >= i__1; i4 += -4) {
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = 0.f;
		d__ = z__[i4 - 3];
	    } else {
		d__ = z__[i4 - 3] * (d__ / (d__ + z__[i4 - 1]));
	    }
/* L50: */
	}

/*        dqd maps Z to ZZ plus Li's test. */

	emin = z__[(i0 << 2) + pp + 1];
	d__ = z__[(i0 << 2) + pp - 3];
	i__1 = (n0 - 1 << 2) + pp;
	for (i4 = (i0 << 2) + pp; i4 <= i__1; i4 += 4) {
	    z__[i4 - (pp << 1) - 2] = d__ + z__[i4 - 1];
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = 0.f;
		z__[i4 - (pp << 1) - 2] = d__;
		z__[i4 - (pp << 1)] = 0.f;
		d__ = z__[i4 + 1];
	    } else if (safmin * z__[i4 + 1] < z__[i4 - (pp << 1) - 2] && 
		    safmin * z__[i4 - (pp << 1) - 2] < z__[i4 + 1]) {
		temp = z__[i4 + 1] / z__[i4 - (pp << 1) - 2];
		z__[i4 - (pp << 1)] = z__[i4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[i4 - (pp << 1)] = z__[i4 + 1] * (z__[i4 - 1] / z__[i4 - (
			pp << 1) - 2]);
		d__ = z__[i4 + 1] * (d__ / z__[i4 - (pp << 1) - 2]);
	    }
/* Computing MIN */
	    r__1 = emin, r__2 = z__[i4 - (pp << 1)];
	    emin = df2cmin(r__1,r__2);
/* L60: */
	}
	z__[(n0 << 2) - pp - 2] = d__;

/*        Now find qmax. */

	qmax = z__[(i0 << 2) - pp - 2];
	i__1 = (n0 << 2) - pp - 2;
	for (i4 = (i0 << 2) - pp + 2; i4 <= i__1; i4 += 4) {
/* Computing MAX */
	    r__1 = qmax, r__2 = z__[i4];
	    qmax = df2cmax(r__1,r__2);
/* L70: */
	}

/*        Prepare for the next iteration on K. */

	pp = 1 - pp;
/* L80: */
    }

    iter = 2;
    nfail = 0;
    ndiv = n0 - i0 << 1;

    i__1 = *n + 1;
    for (iwhila = 1; iwhila <= i__1; ++iwhila) {
	if (n0 < 1) {
	    goto L150;
	}

/*        While array unfinished do   

          E(N0) holds the value of SIGMA when submatrix in I0:N0   
          splits from the rest of the array, but is negated. */

	desig = 0.f;
	if (n0 == *n) {
	    sigma = 0.f;
	} else {
	    sigma = -z__[(n0 << 2) - 1];
	}
	if (sigma < 0.f) {
	    *info = 1;
	    return 0;
	}

/*        Find last unreduced submatrix's top index I0, find QMAX and   
          EMIN. Find Gershgorin-type bound if Q's much greater than E's. */

	emax = 0.f;
	if (n0 > i0) {
	    emin = (r__1 = z__[(n0 << 2) - 5], dabs(r__1));
	} else {
	    emin = 0.f;
	}
	qmin = z__[(n0 << 2) - 3];
	qmax = qmin;
	for (i4 = n0 << 2; i4 >= 8; i4 += -4) {
	    if (z__[i4 - 5] <= 0.f) {
		goto L100;
	    }
	    if (qmin >= emax * 4.f) {
/* Computing MIN */
		r__1 = qmin, r__2 = z__[i4 - 3];
		qmin = df2cmin(r__1,r__2);
/* Computing MAX */
		r__1 = emax, r__2 = z__[i4 - 5];
		emax = df2cmax(r__1,r__2);
	    }
/* Computing MAX */
	    r__1 = qmax, r__2 = z__[i4 - 7] + z__[i4 - 5];
	    qmax = df2cmax(r__1,r__2);
/* Computing MIN */
	    r__1 = emin, r__2 = z__[i4 - 5];
	    emin = df2cmin(r__1,r__2);
/* L90: */
	}
	i4 = 4;

L100:
	i0 = i4 / 4;

/*        Store EMIN for passing to SLASQ3. */

	z__[(n0 << 2) - 1] = emin;

/*        Put -(initial shift) into DMIN.   

   Computing MAX */
	r__1 = 0.f, r__2 = qmin - sqrt(qmin) * 2.f * sqrt(emax);
	dmin__ = -df2cmax(r__1,r__2);

/*        Now I0:N0 is unreduced. PP = 0 for ping, PP = 1 for pong. */

	pp = 0;

	nbig = (n0 - i0 + 1) * 30;
	i__2 = nbig;
	for (iwhilb = 1; iwhilb <= i__2; ++iwhilb) {
	    if (i0 > n0) {
		goto L130;
	    }

/*           While submatrix unfinished take a good dqds step. */

	    slasq3_(&i0, &n0, &z__[1], &pp, &dmin__, &sigma, &desig, &qmax, &
		    nfail, &iter, &ndiv, &ieee);

	    pp = 1 - pp;

/*           When EMIN is very small check for splits. */

	    if (pp == 0 && n0 - i0 >= 3) {
		if (z__[n0 * 4] <= tol2 * qmax || z__[(n0 << 2) - 1] <= tol2 *
			 sigma) {
		    splt = i0 - 1;
		    qmax = z__[(i0 << 2) - 3];
		    emin = z__[(i0 << 2) - 1];
		    oldemn = z__[i0 * 4];
		    i__3 = n0 - 3 << 2;
		    for (i4 = i0 << 2; i4 <= i__3; i4 += 4) {
			if (z__[i4] <= tol2 * z__[i4 - 3] || z__[i4 - 1] <= 
				tol2 * sigma) {
			    z__[i4 - 1] = -sigma;
			    splt = i4 / 4;
			    qmax = 0.f;
			    emin = z__[i4 + 3];
			    oldemn = z__[i4 + 4];
			} else {
/* Computing MAX */
			    r__1 = qmax, r__2 = z__[i4 + 1];
			    qmax = df2cmax(r__1,r__2);
/* Computing MIN */
			    r__1 = emin, r__2 = z__[i4 - 1];
			    emin = df2cmin(r__1,r__2);
/* Computing MIN */
			    r__1 = oldemn, r__2 = z__[i4];
			    oldemn = df2cmin(r__1,r__2);
			}
/* L110: */
		    }
		    z__[(n0 << 2) - 1] = emin;
		    z__[n0 * 4] = oldemn;
		    i0 = splt + 1;
		}
	    }

/* L120: */
	}

	*info = 2;
	return 0;

/*        end IWHILB */

L130:

/* L140: */
	;
    }

    *info = 3;
    return 0;

/*     end IWHILA */

L150:

/*     Move q's to the front. */

    i__1 = *n;
    for (k = 2; k <= i__1; ++k) {
	z__[k] = z__[(k << 2) - 3];
/* L160: */
    }

/*     Sort and compute sum of eigenvalues. */

    slasrt_("D", n, &z__[1], &iinfo);

    e = 0.f;
    for (k = *n; k >= 1; --k) {
	e += z__[k];
/* L170: */
    }

/*     Store trace, sum(eigenvalues) and information on performance. */

    z__[(*n << 1) + 1] = trace;
    z__[(*n << 1) + 2] = e;
    z__[(*n << 1) + 3] = (real) iter;
/* Computing 2nd power */
    i__1 = *n;
    z__[(*n << 1) + 4] = (real) ndiv / (real) (i__1 * i__1);
    z__[(*n << 1) + 5] = nfail * 100.f / (real) iter;
    return 0;

/*     End of SLASQ2 */

} /* slasq2_ */

//=====================================

/* Subroutine */ int slasq3_(integer *i0, integer *n0, real *z__, integer *pp,
	 real *dmin__, real *sigma, real *desig, real *qmax, integer *nfail, 
	integer *iter, integer *ndiv, logical *ieee)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       May 17, 2000   


    Purpose   
    =======   

    SLASQ3 checks for deflation, computes a shift (TAU) and calls dqds.   
    In case of failure it changes shifts, and tries again until output   
    is positive.   

    Arguments   
    =========   

    I0     (input) INTEGER   
           First index.   

    N0     (input) INTEGER   
           Last index.   

    Z      (input) REAL array, dimension ( 4*N )   
           Z holds the qd array.   

    PP     (input) INTEGER   
           PP=0 for ping, PP=1 for pong.   

    DMIN   (output) REAL   
           Minimum value of d.   

    SIGMA  (output) REAL   
           Sum of shifts used in current segment.   

    DESIG  (input/output) REAL   
           Lower order part of SIGMA   

    QMAX   (input) REAL   
           Maximum value of q.   

    NFAIL  (output) INTEGER   
           Number of times shift was too big.   

    ITER   (output) INTEGER   
           Number of iterations.   

    NDIV   (output) INTEGER   
           Number of divisions.   

    TTYPE  (output) INTEGER   
           Shift type.   

    IEEE   (input) LOGICAL   
           Flag for IEEE or non IEEE arithmetic (passed to SLASQ5).   

    =====================================================================   

       Parameter adjustments */
    /* Initialized data */
    static integer ttype = 0;
    static real dmin1 = 0.f;
    static real dmin2 = 0.f;
    static real dn = 0.f;
    static real dn1 = 0.f;
    static real dn2 = 0.f;
    static real tau = 0.f;
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    /* Builtin functions */
    //double sqrt(doublereal);
    /* Local variables */
    static real temp, s, t;
    static integer j4;
    extern /* Subroutine */ int slasq4_(integer *, integer *, real *, integer 
	    *, integer *, real *, real *, real *, real *, real *, real *, 
	    real *, integer *), slasq5_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, real *, real *, real *, 
	    logical *), slasq6_(integer *, integer *, real *, integer *, real 
	    *, real *, real *, real *, real *, real *);
    static integer nn;
    extern doublereal slamch_(const char *);
    static real safmin, eps, tol;
    static integer n0in, ipn4;
    static real tol2;

    --z__;

    /* Function Body */

    n0in = *n0;
    eps = slamch_("Precision");
    safmin = slamch_("Safe minimum");
    tol = eps * 100.f;
/* Computing 2nd power */
    r__1 = tol;
    tol2 = r__1 * r__1;

/*     Check for deflation. */

L10:

    if (*n0 < *i0) {
	return 0;
    }
    if (*n0 == *i0) {
	goto L20;
    }
    nn = (*n0 << 2) + *pp;
    if (*n0 == *i0 + 1) {
	goto L40;
    }

/*     Check whether E(N0-1) is negligible, 1 eigenvalue. */

    if (z__[nn - 5] > tol2 * (*sigma + z__[nn - 3]) && z__[nn - (*pp << 1) - 
	    4] > tol2 * z__[nn - 7]) {
	goto L30;
    }

L20:

    z__[(*n0 << 2) - 3] = z__[(*n0 << 2) + *pp - 3] + *sigma;
    --(*n0);
    goto L10;

/*     Check  whether E(N0-2) is negligible, 2 eigenvalues. */

L30:

    if (z__[nn - 9] > tol2 * *sigma && z__[nn - (*pp << 1) - 8] > tol2 * z__[
	    nn - 11]) {
	goto L50;
    }

L40:

    if (z__[nn - 3] > z__[nn - 7]) {
	s = z__[nn - 3];
	z__[nn - 3] = z__[nn - 7];
	z__[nn - 7] = s;
    }
    if (z__[nn - 5] > z__[nn - 3] * tol2) {
	t = (z__[nn - 7] - z__[nn - 3] + z__[nn - 5]) * .5f;
	s = z__[nn - 3] * (z__[nn - 5] / t);
	if (s <= t) {
	    s = z__[nn - 3] * (z__[nn - 5] / (t * (sqrt(s / t + 1.f) + 1.f)));
	} else {
	    s = z__[nn - 3] * (z__[nn - 5] / (t + sqrt(t) * sqrt(t + s)));
	}
	t = z__[nn - 7] + (s + z__[nn - 5]);
	z__[nn - 3] *= z__[nn - 7] / t;
	z__[nn - 7] = t;
    }
    z__[(*n0 << 2) - 7] = z__[nn - 7] + *sigma;
    z__[(*n0 << 2) - 3] = z__[nn - 3] + *sigma;
    *n0 += -2;
    goto L10;

L50:

/*     Reverse the qd-array, if warranted. */

    if (*dmin__ <= 0.f || *n0 < n0in) {
	if (z__[(*i0 << 2) + *pp - 3] * 1.5f < z__[(*n0 << 2) + *pp - 3]) {
	    ipn4 = *i0 + *n0 << 2;
	    i__1 = *i0 + *n0 - 1 << 1;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		temp = z__[j4 - 3];
		z__[j4 - 3] = z__[ipn4 - j4 - 3];
		z__[ipn4 - j4 - 3] = temp;
		temp = z__[j4 - 2];
		z__[j4 - 2] = z__[ipn4 - j4 - 2];
		z__[ipn4 - j4 - 2] = temp;
		temp = z__[j4 - 1];
		z__[j4 - 1] = z__[ipn4 - j4 - 5];
		z__[ipn4 - j4 - 5] = temp;
		temp = z__[j4];
		z__[j4] = z__[ipn4 - j4 - 4];
		z__[ipn4 - j4 - 4] = temp;
/* L60: */
	    }
	    if (*n0 - *i0 <= 4) {
		z__[(*n0 << 2) + *pp - 1] = z__[(*i0 << 2) + *pp - 1];
		z__[(*n0 << 2) - *pp] = z__[(*i0 << 2) - *pp];
	    }
/* Computing MIN */
	    r__1 = dmin2, r__2 = z__[(*n0 << 2) + *pp - 1];
	    dmin2 = df2cmin(r__1,r__2);
/* Computing MIN */
	    r__1 = z__[(*n0 << 2) + *pp - 1], r__2 = z__[(*i0 << 2) + *pp - 1]
		    , r__1 = f2cmin(r__1,r__2), r__2 = z__[(*i0 << 2) + *pp + 3];
	    z__[(*n0 << 2) + *pp - 1] = df2cmin(r__1,r__2);
/* Computing MIN */
	    r__1 = z__[(*n0 << 2) - *pp], r__2 = z__[(*i0 << 2) - *pp], r__1 =
		     f2cmin(r__1,r__2), r__2 = z__[(*i0 << 2) - *pp + 4];
	    z__[(*n0 << 2) - *pp] = df2cmin(r__1,r__2);
/* Computing MAX */
	    r__1 = *qmax, r__2 = z__[(*i0 << 2) + *pp - 3], r__1 = f2cmax(r__1,
		    r__2), r__2 = z__[(*i0 << 2) + *pp + 1];
	    *qmax = df2cmax(r__1,r__2);
	    *dmin__ = 0.f;
	}
    }

/* L70:   

   Computing MIN */
    r__1 = z__[(*n0 << 2) + *pp - 1], r__2 = z__[(*n0 << 2) + *pp - 9], r__1 =
	     f2cmin(r__1,r__2), r__2 = dmin2 + z__[(*n0 << 2) - *pp];
    if (*dmin__ < 0.f || safmin * *qmax < df2cmin(r__1,r__2)) {

/*        Choose a shift. */

	slasq4_(i0, n0, &z__[1], pp, &n0in, dmin__, &dmin1, &dmin2, &dn, &dn1,
		 &dn2, &tau, &ttype);

/*        Call dqds until DMIN > 0. */

L80:

	slasq5_(i0, n0, &z__[1], pp, &tau, dmin__, &dmin1, &dmin2, &dn, &dn1, 
		&dn2, ieee);

	*ndiv += *n0 - *i0 + 2;
	++(*iter);

/*        Check status. */

	if (*dmin__ >= 0.f && dmin1 > 0.f) {

/*           Success. */

	    goto L100;

	} else if (*dmin__ < 0.f && dmin1 > 0.f && z__[(*n0 - 1 << 2) - *pp] <
		 tol * (*sigma + dn1) && dabs(dn) < tol * *sigma) {

/*           Convergence hidden by negative DN. */

	    z__[(*n0 - 1 << 2) - *pp + 2] = 0.f;
	    *dmin__ = 0.f;
	    goto L100;
	} else if (*dmin__ < 0.f) {

/*           TAU too big. Select new TAU and try again. */

	    ++(*nfail);
	    if (ttype < -22) {

/*              Failed twice. Play it safe. */

		tau = 0.f;
	    } else if (dmin1 > 0.f) {

/*              Late failure. Gives excellent shift. */

		tau = (tau + *dmin__) * (1.f - eps * 2.f);
		ttype += -11;
	    } else {

/*              Early failure. Divide by 4. */

		tau *= .25f;
		ttype += -12;
	    }
	    goto L80;
	} else if (*dmin__ != *dmin__) {

/*           NaN. */

	    tau = 0.f;
	    goto L80;
	} else {

/*           Possible underflow. Play it safe. */

	    goto L90;
	}
    }

/*     Risk of underflow. */

L90:
    slasq6_(i0, n0, &z__[1], pp, dmin__, &dmin1, &dmin2, &dn, &dn1, &dn2);
    *ndiv += *n0 - *i0 + 2;
    ++(*iter);
    tau = 0.f;

L100:
    if (tau < *sigma) {
	*desig += tau;
	t = *sigma + *desig;
	*desig -= t - *sigma;
    } else {
	t = *sigma + tau;
	*desig = *sigma - (t - tau) + *desig;
    }
    *sigma = t;

    return 0;

/*     End of SLASQ3 */

} /* slasq3_ */

//===================================

/* Subroutine */ int slasq4_(integer *i0, integer *n0, real *z__, integer *pp,
	 integer *n0in, real *dmin__, real *dmin1, real *dmin2, real *dn, 
	real *dn1, real *dn2, real *tau, integer *ttype)
{
    /* Initialized data */

    static real g = 0.f;

    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    //double sqrt(doublereal);

    /* Local variables */
    static real s, a2, b1, b2;
    static integer i4, nn, np;
    static real gam, gap1, gap2;


/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SLASQ4 computes an approximation TAU to the smallest eigenvalue   
    using values of d from the previous transform.   

    I0    (input) INTEGER   
          First index.   

    N0    (input) INTEGER   
          Last index.   

    Z     (input) REAL array, dimension ( 4*N )   
          Z holds the qd array.   

    PP    (input) INTEGER   
          PP=0 for ping, PP=1 for pong.   

    NOIN  (input) INTEGER   
          The value of N0 at start of EIGTEST.   

    DMIN  (input) REAL   
          Minimum value of d.   

    DMIN1 (input) REAL   
          Minimum value of d, excluding D( N0 ).   

    DMIN2 (input) REAL   
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).   

    DN    (input) REAL   
          d(N)   

    DN1   (input) REAL   
          d(N-1)   

    DN2   (input) REAL   
          d(N-2)   

    TAU   (output) REAL   
          This is the shift.   

    TTYPE (output) INTEGER   
          Shift type.   

    Further Details   
    ===============   
    CNST1 = 9/16   

    =====================================================================   

       Parameter adjustments */
    --z__;

    /* Function Body   

       A negative DMIN forces the shift to take that absolute value   
       TTYPE records the type of shift. */

    if (*dmin__ <= 0.f) {
	*tau = -(*dmin__);
	*ttype = -1;
	return 0;
    }

    nn = (*n0 << 2) + *pp;
    if (*n0in == *n0) {

/*        No eigenvalues deflated. */

	if (*dmin__ == *dn || *dmin__ == *dn1) {

	    b1 = sqrt(z__[nn - 3]) * sqrt(z__[nn - 5]);
	    b2 = sqrt(z__[nn - 7]) * sqrt(z__[nn - 9]);
	    a2 = z__[nn - 7] + z__[nn - 5];

/*           Cases 2 and 3. */

	    if (*dmin__ == *dn && *dmin1 == *dn1) {
		gap2 = *dmin2 - a2 - *dmin2 * .25f;
		if (gap2 > 0.f && gap2 > b2) {
		    gap1 = a2 - *dn - b2 / gap2 * b2;
		} else {
		    gap1 = a2 - *dn - (b1 + b2);
		}
		if (gap1 > 0.f && gap1 > b1) {
/* Computing MAX */
		    r__1 = *dn - b1 / gap1 * b1, r__2 = *dmin__ * .5f;
		    s = df2cmax(r__1,r__2);
		    *ttype = -2;
		} else {
		    s = 0.f;
		    if (*dn > b1) {
			s = *dn - b1;
		    }
		    if (a2 > b1 + b2) {
/* Computing MIN */
			r__1 = s, r__2 = a2 - (b1 + b2);
			s = df2cmin(r__1,r__2);
		    }
/* Computing MAX */
		    r__1 = s, r__2 = *dmin__ * .333f;
		    s = df2cmax(r__1,r__2);
		    *ttype = -3;
		}
	    } else {

/*              Case 4. */

		*ttype = -4;
		s = *dmin__ * .25f;
		if (*dmin__ == *dn) {
		    gam = *dn;
		    a2 = 0.f;
		    if (z__[nn - 5] > z__[nn - 7]) {
			return 0;
		    }
		    b2 = z__[nn - 5] / z__[nn - 7];
		    np = nn - 9;
		} else {
		    np = nn - (*pp << 1);
		    b2 = z__[np - 2];
		    gam = *dn1;
		    if (z__[np - 4] > z__[np - 2]) {
			return 0;
		    }
		    a2 = z__[np - 4] / z__[np - 2];
		    if (z__[nn - 9] > z__[nn - 11]) {
			return 0;
		    }
		    b2 = z__[nn - 9] / z__[nn - 11];
		    np = nn - 13;
		}

/*              Approximate contribution to norm squared from I < NN-1. */

		a2 += b2;
		i__1 = (*i0 << 2) - 1 + *pp;
		for (i4 = np; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.f) {
			goto L20;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (df2cmax(b2,b1) * 100.f < a2 || .563f < a2) {
			goto L20;
		    }
/* L10: */
		}
L20:
		a2 *= 1.05f;

/*              Rayleigh quotient residual bound. */

		if (a2 < .563f) {
		    s = gam * (1.f - sqrt(a2)) / (a2 + 1.f);
		}
	    }
	} else if (*dmin__ == *dn2) {

/*           Case 5. */

	    *ttype = -5;
	    s = *dmin__ * .25f;

/*           Compute contribution to norm squared from I > NN-2. */

	    np = nn - (*pp << 1);
	    b1 = z__[np - 2];
	    b2 = z__[np - 6];
	    gam = *dn2;
	    if (z__[np - 8] > b2 || z__[np - 4] > b1) {
		return 0;
	    }
	    a2 = z__[np - 8] / b2 * (z__[np - 4] / b1 + 1.f);

/*           Approximate contribution to norm squared from I < NN-2. */

	    if (*n0 - *i0 > 2) {
		b2 = z__[nn - 13] / z__[nn - 15];
		a2 += b2;
		i__1 = (*i0 << 2) - 1 + *pp;
		for (i4 = nn - 17; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.f) {
			goto L40;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (df2cmax(b2,b1) * 100.f < a2 || .563f < a2) {
			goto L40;
		    }
/* L30: */
		}
L40:
		a2 *= 1.05f;
	    }

	    if (a2 < .563f) {
		s = gam * (1.f - sqrt(a2)) / (a2 + 1.f);
	    }
	} else {

/*           Case 6, no information to guide us. */

	    if (*ttype == -6) {
		g += (1.f - g) * .333f;
	    } else if (*ttype == -18) {
		g = .083250000000000005f;
	    } else {
		g = .25f;
	    }
	    s = g * *dmin__;
	    *ttype = -6;
	}

    } else if (*n0in == *n0 + 1) {

/*        One eigenvalue just deflated. Use DMIN1, DN1 for DMIN and DN. */

	if (*dmin1 == *dn1 && *dmin2 == *dn2) {

/*           Cases 7 and 8. */

	    *ttype = -7;
	    s = *dmin1 * .333f;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.f) {
		goto L60;
	    }
	    i__1 = (*i0 << 2) - 1 + *pp;
	    for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
		a2 = b1;
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (df2cmax(b1,a2) * 100.f < b2) {
		    goto L60;
		}
/* L50: */
	    }
L60:
	    b2 = sqrt(b2 * 1.05f);
/* Computing 2nd power */
	    r__1 = b2;
	    a2 = *dmin1 / (r__1 * r__1 + 1.f);
	    gap2 = *dmin2 * .5f - a2;
	    if (gap2 > 0.f && gap2 > b2 * a2) {
/* Computing MAX */
		r__1 = s, r__2 = a2 * (1.f - a2 * 1.01f * (b2 / gap2) * b2);
		s = df2cmax(r__1,r__2);
	    } else {
/* Computing MAX */
		r__1 = s, r__2 = a2 * (1.f - b2 * 1.01f);
		s = df2cmax(r__1,r__2);
		*ttype = -8;
	    }
	} else {

/*           Case 9. */

	    s = *dmin1 * .25f;
	    if (*dmin1 == *dn1) {
		s = *dmin1 * .5f;
	    }
	    *ttype = -9;
	}

    } else if (*n0in == *n0 + 2) {

/*        Two eigenvalues deflated. Use DMIN2, DN2 for DMIN and DN.   

          Cases 10 and 11. */

	if (*dmin2 == *dn2 && z__[nn - 5] * 2.f < z__[nn - 7]) {
	    *ttype = -10;
	    s = *dmin2 * .333f;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.f) {
		goto L80;
	    }
	    i__1 = (*i0 << 2) - 1 + *pp;
	    for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (b1 * 100.f < b2) {
		    goto L80;
		}
/* L70: */
	    }
L80:
	    b2 = sqrt(b2 * 1.05f);
/* Computing 2nd power */
	    r__1 = b2;
	    a2 = *dmin2 / (r__1 * r__1 + 1.f);
	    gap2 = z__[nn - 7] + z__[nn - 9] - sqrt(z__[nn - 11]) * sqrt(z__[
		    nn - 9]) - a2;
	    if (gap2 > 0.f && gap2 > b2 * a2) {
/* Computing MAX */
		r__1 = s, r__2 = a2 * (1.f - a2 * 1.01f * (b2 / gap2) * b2);
		s = df2cmax(r__1,r__2);
	    } else {
/* Computing MAX */
		r__1 = s, r__2 = a2 * (1.f - b2 * 1.01f);
		s = df2cmax(r__1,r__2);
	    }
	} else {
	    s = *dmin2 * .25f;
	    *ttype = -11;
	}
    } else if (*n0in > *n0 + 2) {

/*        Case 12, more than two eigenvalues deflated. No information. */

	s = 0.f;
	*ttype = -12;
    }

    *tau = s;
    return 0;

/*     End of SLASQ4 */

} /* slasq4_ */

//====================================

/* Subroutine */ int slasq5_(integer *i0, integer *n0, real *z__, integer *pp,
	 real *tau, real *dmin__, real *dmin1, real *dmin2, real *dn, real *
	dnm1, real *dnm2, logical *ieee)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       May 17, 2000   


    Purpose   
    =======   

    SLASQ5 computes one dqds transform in ping-pong form, one   
    version for IEEE machines another for non IEEE machines.   

    Arguments   
    =========   

    I0    (input) INTEGER   
          First index.   

    N0    (input) INTEGER   
          Last index.   

    Z     (input) REAL array, dimension ( 4*N )   
          Z holds the qd array. EMIN is stored in Z(4*N0) to avoid   
          an extra argument.   

    PP    (input) INTEGER   
          PP=0 for ping, PP=1 for pong.   

    TAU   (input) REAL   
          This is the shift.   

    DMIN  (output) REAL   
          Minimum value of d.   

    DMIN1 (output) REAL   
          Minimum value of d, excluding D( N0 ).   

    DMIN2 (output) REAL   
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).   

    DN    (output) REAL   
          d(N0), the last value of d.   

    DNM1  (output) REAL   
          d(N0-1).   

    DNM2  (output) REAL   
          d(N0-2).   

    IEEE  (input) LOGICAL   
          Flag for IEEE or non IEEE arithmetic.   

    =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    /* Local variables */
    static real emin, temp, d__;
    static integer j4, j4p2;

    --z__;

    /* Function Body */
    if (*n0 - *i0 - 1 <= 0) {
	return 0;
    }

    j4 = (*i0 << 2) + *pp - 3;
    emin = z__[j4 + 4];
    d__ = z__[j4] - *tau;
    *dmin__ = d__;
    *dmin1 = -z__[j4];

    if (*ieee) {

/*        Code for IEEE arithmetic. */

	if (*pp == 0) {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 2] = d__ + z__[j4 - 1];
		temp = z__[j4 + 1] / z__[j4 - 2];
		d__ = d__ * temp - *tau;
		*dmin__ = df2cmin(*dmin__,d__);
		z__[j4] = z__[j4 - 1] * temp;
/* Computing MIN */
		r__1 = z__[j4];
		emin = df2cmin(r__1,emin);
/* L10: */
	    }
	} else {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 3] = d__ + z__[j4];
		temp = z__[j4 + 2] / z__[j4 - 3];
		d__ = d__ * temp - *tau;
		*dmin__ = df2cmin(*dmin__,d__);
		z__[j4 - 1] = z__[j4] * temp;
/* Computing MIN */
		r__1 = z__[j4 - 1];
		emin = df2cmin(r__1,emin);
/* L20: */
	    }
	}

/*        Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = (*n0 - 2 << 2) - *pp;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
	*dmin__ = df2cmin(*dmin__,*dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
	*dmin__ = df2cmin(*dmin__,*dn);

    } else {

/*        Code for non IEEE arithmetic. */

	if (*pp == 0) {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 2] = d__ + z__[j4 - 1];
		if (d__ < 0.f) {
		    return 0;
		} else {
		    z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
		    d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]) - *tau;
		}
		*dmin__ = df2cmin(*dmin__,d__);
/* Computing MIN */
		r__1 = emin, r__2 = z__[j4];
		emin = df2cmin(r__1,r__2);
/* L30: */
	    }
	} else {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 3] = d__ + z__[j4];
		if (d__ < 0.f) {
		    return 0;
		} else {
		    z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
		    d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]) - *tau;
		}
		*dmin__ = df2cmin(*dmin__,d__);
/* Computing MIN */
		r__1 = emin, r__2 = z__[j4 - 1];
		emin = df2cmin(r__1,r__2);
/* L40: */
	    }
	}

/*        Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = (*n0 - 2 << 2) - *pp;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	if (*dnm2 < 0.f) {
	    return 0;
	} else {
	    z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	    *dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
	}
	*dmin__ = df2cmin(*dmin__,*dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	if (*dnm1 < 0.f) {
	    return 0;
	} else {
	    z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	    *dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
	}
	*dmin__ = df2cmin(*dmin__,*dn);

    }

    z__[j4 + 2] = *dn;
    z__[(*n0 << 2) - *pp] = emin;
    return 0;

/*     End of SLASQ5 */

} /* slasq5_ */

//====================================

/* Subroutine */ int slasq6_(integer *i0, integer *n0, real *z__, integer *pp,
	 real *dmin__, real *dmin1, real *dmin2, real *dn, real *dnm1, real *
	dnm2)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Local variables */
    static real emin, temp, d__;
    static integer j4;
    extern doublereal slamch_(const char *);
    static real safmin;
    static integer j4p2;


/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1999   


    Purpose   
    =======   

    SLASQ6 computes one dqd (shift equal to zero) transform in   
    ping-pong form, with protection against underflow and overflow.   

    Arguments   
    =========   

    I0    (input) INTEGER   
          First index.   

    N0    (input) INTEGER   
          Last index.   

    Z     (input) REAL array, dimension ( 4*N )   
          Z holds the qd array. EMIN is stored in Z(4*N0) to avoid   
          an extra argument.   

    PP    (input) INTEGER   
          PP=0 for ping, PP=1 for pong.   

    DMIN  (output) REAL   
          Minimum value of d.   

    DMIN1 (output) REAL   
          Minimum value of d, excluding D( N0 ).   

    DMIN2 (output) REAL   
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).   

    DN    (output) REAL   
          d(N0), the last value of d.   

    DNM1  (output) REAL   
          d(N0-1).   

    DNM2  (output) REAL   
          d(N0-2).   

    =====================================================================   


       Parameter adjustments */
    --z__;

    /* Function Body */
    if (*n0 - *i0 - 1 <= 0) {
	return 0;
    }

    safmin = slamch_("Safe minimum");
    j4 = (*i0 << 2) + *pp - 3;
    emin = z__[j4 + 4];
    d__ = z__[j4];
    *dmin__ = d__;

    if (*pp == 0) {
	i__1 = *n0 - 3 << 2;
	for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
	    z__[j4 - 2] = d__ + z__[j4 - 1];
	    if (z__[j4 - 2] == 0.f) {
		z__[j4] = 0.f;
		d__ = z__[j4 + 1];
		*dmin__ = d__;
		emin = 0.f;
	    } else if (safmin * z__[j4 + 1] < z__[j4 - 2] && safmin * z__[j4 
		    - 2] < z__[j4 + 1]) {
		temp = z__[j4 + 1] / z__[j4 - 2];
		z__[j4] = z__[j4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
		d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]);
	    }
	    *dmin__ = df2cmin(*dmin__,d__);
/* Computing MIN */
	    r__1 = emin, r__2 = z__[j4];
	    emin = df2cmin(r__1,r__2);
/* L10: */
	}
    } else {
	i__1 = *n0 - 3 << 2;
	for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
	    z__[j4 - 3] = d__ + z__[j4];
	    if (z__[j4 - 3] == 0.f) {
		z__[j4 - 1] = 0.f;
		d__ = z__[j4 + 2];
		*dmin__ = d__;
		emin = 0.f;
	    } else if (safmin * z__[j4 + 2] < z__[j4 - 3] && safmin * z__[j4 
		    - 3] < z__[j4 + 2]) {
		temp = z__[j4 + 2] / z__[j4 - 3];
		z__[j4 - 1] = z__[j4] * temp;
		d__ *= temp;
	    } else {
		z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
		d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]);
	    }
	    *dmin__ = df2cmin(*dmin__,d__);
/* Computing MIN */
	    r__1 = emin, r__2 = z__[j4 - 1];
	    emin = df2cmin(r__1,r__2);
/* L20: */
	}
    }

/*     Unroll last two steps. */

    *dnm2 = d__;
    *dmin2 = *dmin__;
    j4 = (*n0 - 2 << 2) - *pp;
    j4p2 = j4 + (*pp << 1) - 1;
    z__[j4 - 2] = *dnm2 + z__[j4p2];
    if (z__[j4 - 2] == 0.f) {
	z__[j4] = 0.f;
	*dnm1 = z__[j4p2 + 2];
	*dmin__ = *dnm1;
	emin = 0.f;
    } else if (safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] < 
	    z__[j4p2 + 2]) {
	temp = z__[j4p2 + 2] / z__[j4 - 2];
	z__[j4] = z__[j4p2] * temp;
	*dnm1 = *dnm2 * temp;
    } else {
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]);
    }
    *dmin__ = df2cmin(*dmin__,*dnm1);

    *dmin1 = *dmin__;
    j4 += 4;
    j4p2 = j4 + (*pp << 1) - 1;
    z__[j4 - 2] = *dnm1 + z__[j4p2];
    if (z__[j4 - 2] == 0.f) {
	z__[j4] = 0.f;
	*dn = z__[j4p2 + 2];
	*dmin__ = *dn;
	emin = 0.f;
    } else if (safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] < 
	    z__[j4p2 + 2]) {
	temp = z__[j4p2 + 2] / z__[j4 - 2];
	z__[j4] = z__[j4p2] * temp;
	*dn = *dnm1 * temp;
    } else {
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]);
    }
    *dmin__ = df2cmin(*dmin__,*dn);

    z__[j4 + 2] = *dn;
    z__[(*n0 << 2) - *pp] = emin;
    return 0;

/*     End of SLASQ6 */

} /* slasq6_ */

//===================================

/* Subroutine */ int slasv2_(real *f, real *g, real *h__, real *ssmin, real *
	ssmax, real *snr, real *csr, real *snl, real *csl)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLASV2 computes the singular value decomposition of a 2-by-2   
    triangular matrix   
       [  F   G  ]   
       [  0   H  ].   
    On return, abs(SSMAX) is the larger singular value, abs(SSMIN) is the   
    smaller singular value, and (CSL,SNL) and (CSR,SNR) are the left and   
    right singular vectors for abs(SSMAX), giving the decomposition   

       [ CSL  SNL ] [  F   G  ] [ CSR -SNR ]  =  [ SSMAX   0   ]   
       [-SNL  CSL ] [  0   H  ] [ SNR  CSR ]     [  0    SSMIN ].   

    Arguments   
    =========   

    F       (input) REAL   
            The (1,1) element of the 2-by-2 matrix.   

    G       (input) REAL   
            The (1,2) element of the 2-by-2 matrix.   

    H       (input) REAL   
            The (2,2) element of the 2-by-2 matrix.   

    SSMIN   (output) REAL   
            abs(SSMIN) is the smaller singular value.   

    SSMAX   (output) REAL   
            abs(SSMAX) is the larger singular value.   

    SNL     (output) REAL   
    CSL     (output) REAL   
            The vector (CSL, SNL) is a unit left singular vector for the   
            singular value abs(SSMAX).   

    SNR     (output) REAL   
    CSR     (output) REAL   
            The vector (CSR, SNR) is a unit right singular vector for the   
            singular value abs(SSMAX).   

    Further Details   
    ===============   

    Any input parameter may be aliased with any output parameter.   

    Barring over/underflow and assuming a guard digit in subtraction, all   
    output quantities are correct to within a few units in the last   
    place (ulps).   

    In IEEE arithmetic, the code works correctly if one matrix element is   
    infinite.   

    Overflow will not occur unless the largest singular value itself   
    overflows or is within a few ulps of overflow. (On machines with   
    partial overflow, like the Cray, overflow may occur if the largest   
    singular value is within a factor of 2 of overflow.)   

    Underflow is harmless if underflow is gradual. Otherwise, results   
    may correspond to a matrix modified by perturbations of size near   
    the underflow threshold.   

   ===================================================================== */
    /* Table of constant values */
    static real c_b3 = 2.f;
    static real c_b4 = 1.f;
    
    /* System generated locals */
    real r__1;
    /* Builtin functions */
    //double sqrt(doublereal), r_sign(real *, real *);
    /* Local variables */
    static integer pmax;
    static real temp;
    static logical swap;
    static real a, d__, l, m, r__, s, t, tsign, fa, ga, ha, ft, gt, ht, mm;
    static logical gasmal;
    extern doublereal slamch_(const char *);
    static real tt, clt, crt, slt, srt;




    ft = *f;
    fa = dabs(ft);
    ht = *h__;
    ha = dabs(*h__);

/*     PMAX points to the maximum absolute element of matrix   
         PMAX = 1 if F largest in absolute values   
         PMAX = 2 if G largest in absolute values   
         PMAX = 3 if H largest in absolute values */

    pmax = 1;
    swap = ha > fa;
    if (swap) {
	pmax = 3;
	temp = ft;
	ft = ht;
	ht = temp;
	temp = fa;
	fa = ha;
	ha = temp;

/*        Now FA .ge. HA */

    }
    gt = *g;
    ga = dabs(gt);
    if (ga == 0.f) {

/*        Diagonal matrix */

	*ssmin = ha;
	*ssmax = fa;
	clt = 1.f;
	crt = 1.f;
	slt = 0.f;
	srt = 0.f;
    } else {
	gasmal = TRUE_;
	if (ga > fa) {
	    pmax = 2;
	    if (fa / ga < slamch_("EPS")) {

/*              Case of very large GA */

		gasmal = FALSE_;
		*ssmax = ga;
		if (ha > 1.f) {
		    *ssmin = fa / (ga / ha);
		} else {
		    *ssmin = fa / ga * ha;
		}
		clt = 1.f;
		slt = ht / gt;
		srt = 1.f;
		crt = ft / gt;
	    }
	}
	if (gasmal) {

/*           Normal case */

	    d__ = fa - ha;
	    if (d__ == fa) {

/*              Copes with infinite F or H */

		l = 1.f;
	    } else {
		l = d__ / fa;
	    }

/*           Note that 0 .le. L .le. 1 */

	    m = gt / ft;

/*           Note that abs(M) .le. 1/macheps */

	    t = 2.f - l;

/*           Note that T .ge. 1 */

	    mm = m * m;
	    tt = t * t;
	    s = sqrt(tt + mm);

/*           Note that 1 .le. S .le. 1 + 1/macheps */

	    if (l == 0.f) {
		r__ = dabs(m);
	    } else {
		r__ = sqrt(l * l + mm);
	    }

/*           Note that 0 .le. R .le. 1 + 1/macheps */

	    a = (s + r__) * .5f;

/*           Note that 1 .le. A .le. 1 + abs(M) */

	    *ssmin = ha / a;
	    *ssmax = fa * a;
	    if (mm == 0.f) {

/*              Note that M is very tiny */

		if (l == 0.f) {
		    t = r_sign(&c_b3, &ft) * r_sign(&c_b4, &gt);
		} else {
		    t = gt / r_sign(&d__, &ft) + m / t;
		}
	    } else {
		t = (m / (s + t) + m / (r__ + l)) * (a + 1.f);
	    }
	    l = sqrt(t * t + 4.f);
	    crt = 2.f / l;
	    srt = t / l;
	    clt = (crt + srt * m) / a;
	    slt = ht / ft * srt / a;
	}
    }
    if (swap) {
	*csl = srt;
	*snl = crt;
	*csr = slt;
	*snr = clt;
    } else {
	*csl = clt;
	*snl = slt;
	*csr = crt;
	*snr = srt;
    }

/*     Correct signs of SSMAX and SSMIN */

    if (pmax == 1) {
	tsign = r_sign(&c_b4, csr) * r_sign(&c_b4, csl) * r_sign(&c_b4, f);
    }
    if (pmax == 2) {
	tsign = r_sign(&c_b4, snr) * r_sign(&c_b4, csl) * r_sign(&c_b4, g);
    }
    if (pmax == 3) {
	tsign = r_sign(&c_b4, snr) * r_sign(&c_b4, snl) * r_sign(&c_b4, h__);
    }
    *ssmax = r_sign(ssmax, &tsign);
    r__1 = tsign * r_sign(&c_b4, f) * r_sign(&c_b4, h__);
    *ssmin = r_sign(ssmin, &r__1);
    return 0;

/*     End of SLASV2 */

} /* slasv2_ */

//=================================

/* Subroutine */ int slas2_(real *f, real *g, real *h__, real *ssmin, real *
	ssmax)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLAS2  computes the singular values of the 2-by-2 matrix   
       [  F   G  ]   
       [  0   H  ].   
    On return, SSMIN is the smaller singular value and SSMAX is the   
    larger singular value.   

    Arguments   
    =========   

    F       (input) REAL   
            The (1,1) element of the 2-by-2 matrix.   

    G       (input) REAL   
            The (1,2) element of the 2-by-2 matrix.   

    H       (input) REAL   
            The (2,2) element of the 2-by-2 matrix.   

    SSMIN   (output) REAL   
            The smaller singular value.   

    SSMAX   (output) REAL   
            The larger singular value.   

    Further Details   
    ===============   

    Barring over/underflow, all output quantities are correct to within   
    a few units in the last place (ulps), even in the absence of a guard   
    digit in addition/subtraction.   

    In IEEE arithmetic, the code works correctly if one matrix element is   
    infinite.   

    Overflow will not occur unless the largest singular value itself   
    overflows, or is within a few ulps of overflow. (On machines with   
    partial overflow, like the Cray, overflow may occur if the largest   
    singular value is within a factor of 2 of overflow.)   

    Underflow is harmless if underflow is gradual. Otherwise, results   
    may correspond to a matrix modified by perturbations of size near   
    the underflow threshold.   

    ==================================================================== */
    /* System generated locals */
    real r__1, r__2;
    /* Builtin functions */
    //double sqrt(doublereal);
    /* Local variables */
    static real fhmn, fhmx, c__, fa, ga, ha, as, at, au;



    fa = dabs(*f);
    ga = dabs(*g);
    ha = dabs(*h__);
    fhmn = df2cmin(fa,ha);
    fhmx = df2cmax(fa,ha);
    if (fhmn == 0.f) {
	*ssmin = 0.f;
	if (fhmx == 0.f) {
	    *ssmax = ga;
	} else {
/* Computing 2nd power */
	    r__1 = df2cmin(fhmx,ga) / df2cmax(fhmx,ga);
	    *ssmax = df2cmax(fhmx,ga) * sqrt(r__1 * r__1 + 1.f);
	}
    } else {
	if (ga < fhmx) {
	    as = fhmn / fhmx + 1.f;
	    at = (fhmx - fhmn) / fhmx;
/* Computing 2nd power */
	    r__1 = ga / fhmx;
	    au = r__1 * r__1;
	    c__ = 2.f / (sqrt(as * as + au) + sqrt(at * at + au));
	    *ssmin = fhmn * c__;
	    *ssmax = fhmx / c__;
	} else {
	    au = fhmx / ga;
	    if (au == 0.f) {

/*              Avoid possible harmful underflow if exponent range   
                asymmetric (true SSMIN may not underflow even if   
                AU underflows) */

		*ssmin = fhmn * fhmx / ga;
		*ssmax = ga;
	    } else {
		as = fhmn / fhmx + 1.f;
		at = (fhmx - fhmn) / fhmx;
/* Computing 2nd power */
		r__1 = as * au;
/* Computing 2nd power */
		r__2 = at * au;
		c__ = 1.f / (sqrt(r__1 * r__1 + 1.f) + sqrt(r__2 * r__2 + 1.f)
			);
		*ssmin = fhmn * c__ * au;
		*ssmin += *ssmin;
		*ssmax = ga / (c__ + c__);
	    }
	}
    }
    return 0;

/*     End of SLAS2 */

} /* slas2_ */
