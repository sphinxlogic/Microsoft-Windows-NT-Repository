/* sin, cos, etc, for S-Lang */
/* 
 * Copyright (c) 1992, 1994 John E. Davis 
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#include <math.h>

#ifndef FLOAT_TYPE 
#define FLOAT_TYPE 5
#endif

#include "slang.h"
#include "_slang.h"


#if defined(_MSC_VER) && defined(_MT)
static FLOAT dmath1(double (_pascal *f) (double))
#else
static FLOAT dmath1(double (*f) (double))
#endif
{
   FLOAT x; 
   int dum1, dum2;
   if (SLang_pop_float(&x, &dum1, &dum2)) return(0.0);
   
   return (FLOAT) (*f)((double) x);
}

#if defined(_MSC_VER) && defined(_MT)
static FLOAT dmath2(double (_pascal *f) (double, double))
#else
static FLOAT dmath2(double (*f) (double, double))
#endif
{
   FLOAT x, y; 
   int dum1, dum2;
   if (SLang_pop_float(&y, &dum1, &dum2)
       || SLang_pop_float(&x, &dum1, &dum2)) return (0.0);

   return (FLOAT) (*f)((double) x, (double) y);
}



FLOAT math_cos	(void) { return (FLOAT) dmath1(cos); }
FLOAT math_sin	(void) { return (FLOAT) dmath1(sin); }
FLOAT math_tan	(void) { return (FLOAT) dmath1(tan); }
FLOAT math_atan	(void) { return (FLOAT) dmath1(atan); }
FLOAT math_acos	(void) { return (FLOAT) dmath1(acos); }
FLOAT math_asin	(void) { return (FLOAT) dmath1(asin); }
FLOAT math_exp	(void) { return (FLOAT) dmath1(exp); }
FLOAT math_log	(void) { return (FLOAT) dmath1(log); }
FLOAT math_sqrt	(void) { return (FLOAT) dmath1(sqrt); }
FLOAT math_log10(void) { return (FLOAT) dmath1(log10); }
FLOAT math_pow(void)   { return (FLOAT) dmath2(pow); }

/* usage here is a1 a2 ... an n x ==> a1x^n + a2 x ^(n - 1) + ... + an */
FLOAT math_poly(void)
{
   int n;
   int dum1, dum2;
   double xn = 1.0, sum = 0.0;
   FLOAT an, x;
   
   if ((SLang_pop_float(&x, &dum1, &dum2))
       || (SLang_pop_integer(&n))) return(0.0);
   
   while (n-- > 0)
     {
	if (SLang_pop_float(&an, &dum1, &dum2)) break;
	(void) dum1; (void) dum2;
	sum += an * xn;
	xn = xn * x;
     }
   return((FLOAT) sum);
}

#ifdef USE_DOUBLE
static FLOAT Const_E =  2.718281828459045;
static FLOAT Const_Pi = 3.141592653589793;
#else
static FLOAT Const_E =  2.7182818;
static FLOAT Const_Pi = 3.1415926;
#endif

static FLOAT slmath_do_float(void)
{
   FLOAT f = 0.0;
   unsigned char stype;
   SLang_Object_Type obj;
   

   if (SLang_pop(&obj)) return(f);

   stype = obj.sub_type;
   if (stype == INT_TYPE)
     {
	f = (FLOAT) obj.v.i_val;
     }
   else if (stype == FLOAT_TYPE)
     {
	f = obj.v.f_val;
     }
   else if (stype == STRING_TYPE)
     {
	/* Should check for parse error here but later. */
	f = atof(obj.v.s_val);
	if (obj.main_type == SLANG_DATA) SLFREE(obj.v.s_val);
     }
   else SLang_Error = TYPE_MISMATCH;
   return f;
}

static SLang_Name_Type slmath_table[] =
{
   MAKE_INTRINSIC(".polynom", math_poly, FLOAT_TYPE, 0),
   /* Usage:
       a b .. c n x polynom  =y
      This computes:
       ax^n + bx^(n - 1) + ... c = y  */
   MAKE_INTRINSIC(".sin", math_sin, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".cos", math_cos, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".tan", math_tan, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".atan", math_atan, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".acos", math_acos, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".asin", math_asin, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".exp", math_exp, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".log", math_log, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".sqrt", math_sqrt, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".log10", math_log10, FLOAT_TYPE, 0),
   MAKE_INTRINSIC(".pow", math_pow, FLOAT_TYPE, 0),
   MAKE_VARIABLE(".E", &Const_E, FLOAT_TYPE, 1),
   MAKE_VARIABLE(".PI", &Const_Pi, FLOAT_TYPE, 1),
   MAKE_INTRINSIC(".float",  slmath_do_float, FLOAT_TYPE, 0),
   /* Convert from integer or string representation to floating point.  
      For example, "12.34" float returns 12.34 to stack.
      as another example, consider:
      1 2 /   ==>  0  since 1 and 2 are integers
      1 2 float / ==> 0.5 since float converts 2 to 2.0 and floating point 
      division is used.
      */
   SLANG_END_TABLE
};

int init_SLmath(void)
{
   return SLang_add_table(slmath_table, "_Math");
}

