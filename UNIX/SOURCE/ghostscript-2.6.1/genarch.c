/* Copyright (C) 1989, 1992, 1993 Aladdin Enterprises.  All rights reserved.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* genarch.c */
/* Generate a header file (arch.h) with parameters */
/* reflecting the machine architecture and compiler characteristics. */

#include <stdio.h>

/* We should write the result on stdout, but the original Turbo C 'make' */
/* can't handle output redirection (sigh). */

main(argc, argv)
    int argc;
    char *argv[];
{	char *fname = argv[1];
	long one = 1;
#define ffs_strlen 16
	char *ffs = "ffffffffffffffff";
	struct { char c; short s; } ss;
	struct { char c; int i; } si;
	struct { char c; long l; } sl;
	struct { char c; char *p; } sp;
	struct { char c; float f; } sf;
	struct { char c; double d; } sd;
	long lm1 = -1;
	long lr1 = lm1 >> 1, lr2 = lm1 >> 2;
	unsigned long um1 = ~(unsigned long)0;
	int im1 = -1;
	int ir1 = im1 >> 1, ir2 = im1 >> 2;
	union { long l; char *p; } pl0, pl1;
	int ars;
	int lwidth = sizeof(long) * 8;
	union { float f; int i; long l; } f0, f1, fm1;
	FILE *f = fopen(fname, "w");
	if ( f == NULL )
	   {	fprintf(stderr, "genarch.c: can't open %s for writing\n", fname);
		exit(1);
	   }
	fprintf(f, "/* Parameters derived from machine and compiler architecture */\n");

	fprintf(f, "\n\t/* Scalar alignments */\n");
#define offset(s, e) (int)((char *)&s.e - (char *)&s)
	fprintf(f, "#define arch_align_short_mod %d\n", offset(ss, s));
	fprintf(f, "#define arch_align_int_mod %d\n", offset(si, i));
	fprintf(f, "#define arch_align_long_mod %d\n", offset(sl, l));
	fprintf(f, "#define arch_align_ptr_mod %d\n", offset(sp, p));
	fprintf(f, "#define arch_align_float_mod %d\n", offset(sf, f));
	fprintf(f, "#define arch_align_double_mod %d\n", offset(sd, d));
#undef offset

	fprintf(f, "\n\t/* Scalar sizes */\n");
	fprintf(f, "#define arch_sizeof_short %d\n", sizeof(short));
	fprintf(f, "#define arch_sizeof_int %d\n", sizeof(int));
	fprintf(f, "#define arch_sizeof_long %d\n", sizeof(long));
	fprintf(f, "#define arch_sizeof_ptr %d\n", sizeof(char *));
	fprintf(f, "#define arch_sizeof_float %d\n", sizeof(float));
	fprintf(f, "#define arch_sizeof_double %d\n", sizeof(double));

	fprintf(f, "\n\t/* Unsigned max values */\n");
#define pmax(str, typ, tstr, l)\
  fprintf(f, "#define arch_max_%s ((%s)0x%s%s + (%s)0)\n",\
    str, tstr, ffs + ffs_strlen - sizeof(typ) * 2, l, tstr)
	pmax("uchar", unsigned char, "unsigned char", "");
	pmax("ushort", unsigned short, "unsigned short", "");
	pmax("uint", unsigned int, "unsigned int", "");
	pmax("ulong", unsigned long, "unsigned long", "L");
#undef pmax

	fprintf(f, "\n\t/* Miscellaneous */\n");
	fprintf(f, "#define arch_is_big_endian %d\n", 1 - *(char *)&one);
	pl0.l = 0;
	pl1.l = -1;
	fprintf(f, "#define arch_ptrs_are_signed %d\n",
		(pl1.p < pl0.p));
	f0.f = 0.0, f1.f = 1.0, fm1.f = -1.0;
	/* We have to test the size dynamically here, */
	/* because the preprocessor can't evaluate sizeof. */
	fprintf(f, "#define arch_floats_are_IEEE %d\n",
		((sizeof(float) == sizeof(int) ?
		  f0.i == 0 && f1.i == (int)0x3f800000 && fm1.i == (int)0xbf800000 :
		  f0.l == 0 && f1.l == 0x3f800000L && fm1.l == 0xbf800000L)
		 ? 1 : 0));
	/* There are three cases for arithmetic right shift: */
	/* always correct, correct except for right-shifting a long by 1 */
	/* (a bug in some versions of the Turbo C compiler), and */
	/* never correct. */
	ars = (lr2 != -1 || ir1 != -1 || ir2 != -1 ? 0 :
		lr1 != -1 ? 1 :		/* Turbo C problem */
		2);
	fprintf(f, "#define arch_arith_rshift %d\n", ars);
	/* Some machines can't handle a variable shift by */
	/* the full width of a long. */
	fprintf(f, "#define arch_can_shift_full_long %d\n",
		um1 >> lwidth == 0);
	fclose(f);
	return 0;
}
