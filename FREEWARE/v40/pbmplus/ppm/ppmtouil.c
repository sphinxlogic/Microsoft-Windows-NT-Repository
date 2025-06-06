/* ppmtouil.c - read a portable pixmap and produce a Motif UIL icon file
**
** Converted by Jef Poskanzer from ppmtoxpm.c, which is
** Copyright (C) 1990 by Mark W. Snitily.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <ctype.h>
#include "ppm.h"
#include "ppmcmap.h"

/* Max number of colors allowed in ppm input. */
#define MAXCOLORS 256

/* Lower bound and upper bound of character-pixels printed in UIL output. */
#define LOW_CHAR '`'
#define HIGH_CHAR '~'

typedef struct {	/* character-pixel mapping */
    char* cixel;	/* character string printed for pixel */
    char* rgbname;	/* ascii rgb color, either mnemonic or #rgb value */
    char* barname;	/* ascii rgb color with spaces replaced by underbars */
    } cixel_map;

static char* gen_numstr ARGS(( int i, int base, char low_char, int digits ));
static void gen_cmap ARGS(( colorhist_vector chv, int ncolors, pixval maxval, cixel_map cmap[MAXCOLORS], int* charsppP ));

#ifndef VMS
void
#endif
main( argc, argv )
    int argc;
    char* argv[];
    {
    FILE* ifp;
    pixel** pixels;
    register pixel* pP;
    int argn, rows, cols, ncolors, row, col, i;
    pixval maxval;
    colorhash_table cht;
    colorhist_vector chv;
    cixel_map cmap[MAXCOLORS];
    int charspp;
    char out_name[100];
    char* cp;
    int printed_one;
    char* usage = "[-name <uilname>] [ppmfile]";

#ifdef VMS
    getredirection (&argc, &argv);
#endif

    ppm_init( &argc, argv );
    out_name[0] = '\0';

    argn = 1;

    /* Check for command line options. */
    while ( argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0' )
	{
	if ( pm_keymatch( argv[argn], "-name", 2 ) )
	    {
	    ++argn;
	    if ( argn == argc )
		pm_usage( usage );
	    (void) strcpy( out_name, argv[argn] );
	    }
	else
	    pm_usage( usage );
	++argn;
	}

    if ( argn != argc )
	{
	/* Open the input file. */
	ifp = pm_openr( argv[argn] );

	/* If output filename not specified, use input filename as default. */
	if ( out_name[0] == '\0' )
	    {
	    (void) strcpy( out_name, argv[argn] );
	    cp = index( out_name, '.' );
	    if ( cp != 0 )
		*cp = '\0';	/* remove extension */
	    if ( strcmp( out_name, "-" ) == 0 )
		(void) strcpy( out_name, "noname" );
	    /* Remove trailing _icon in name. */
	    cp = rindex( out_name, '_' );
	    if ( cp != (char*) 0 &&
		 cp[1] == 'i' && cp[2] == 'c' && cp[3] == 'o' &&
		 cp[4] == 'n' && cp[5] == '\0' )
		*cp = '\0';
	    }
	++argn;
	}
    else
	{
	/* No input file specified. */
	ifp = stdin;
	if ( out_name[0] == '\0' )
	    (void) strcpy( out_name, "noname" );
	}

    if ( argn != argc )
	pm_usage( usage );

    /* Read in the ppm file. */
    pixels = ppm_readppm( ifp, &cols, &rows, &maxval );
    pm_close( ifp );

    /* Figure out the colormap. */
    pm_message( "computing colormap..." );
    chv = ppm_computecolorhist( pixels, cols, rows, MAXCOLORS, &ncolors );
    if ( chv == (colorhist_vector) 0 )
	pm_error(
	    "too many colors - try doing a 'ppmquant %d'", MAXCOLORS );
    pm_message( "%d colors found", ncolors );

    /* Make a hash table for fast color lookup. */
    cht = ppm_colorhisttocolorhash( chv, ncolors );

    /* Now generate the character-pixel colormap table. */
    gen_cmap( chv, ncolors, maxval, cmap, &charspp );

    /* Write out the UIL header. */
    printf( "module %s\n", out_name );
    printf( "version = 'V1.0'\n" );
    printf( "names = case_sensitive\n" );
    printf( "include file 'XmAppl.uil';\n" );

    /* Write out the colors. */
    printf( "\n" );
    printf( "value\n" );
    for ( i = 0; i < ncolors; ++i )
	if ( cmap[i].barname != (char*) 0 )
	    printf(
		"    %s : color( '%s' );\n", cmap[i].barname, cmap[i].rgbname );

    /* Write out the ascii colormap. */
    printf( "\n" );
    printf( "value\n" );
    printf( "  %s_rgb : color_table (\n", out_name );
    printed_one = 0;
    for ( i = 0; i < ncolors; ++i )
	if ( cmap[i].barname != (char*) 0 )
	    {
	    if ( printed_one )
		printf( ",\n" );
	    printf( "    %s = '%s'", cmap[i].barname, cmap[i].cixel );
	    printed_one = 1;
	    }     
    printf( "\n    );\n" );

    /* Write out the ascii character-pixel image. */
    printf( "\n" );
    printf(
	"%s_icon : exported icon( color_table = %s_rgb,\n",
	out_name, out_name );
    for ( row = 0; row < rows; ++row )
	{
	printf( "    '" );
	for ( col = 0, pP = pixels[row]; col < cols; ++col, ++pP )
	    {
	    if ( ( col * charspp ) % 70 == 0 && col > 0 )
		printf( "\\\n" );		/* line continuation */
	    printf( "%s", cmap[ppm_lookupcolor(cht, pP)].cixel );
	    }
	if ( row != rows - 1 )
	    printf( "',\n" );
	else
	    printf( "'\n") ; 
	}
    printf( ");\n" );
    printf( "\n" );
    printf( "end module;\n" );

    exit( 0 );
    }

/* Given a number and a base, this routine prints the number into a
** malloc'ed string and returns it.  The length of the string is
** specified by "digits".  The ascii characters of the printed
** number range from low_char to low_char + base.  The string is
** low_char filled.
*/

#if __STDC__
static char*
gen_numstr( int i, int base, char low_char, int digits )
#else /*__STDC__*/
static char*
gen_numstr( i, base, low_char, digits )
    int i, base, digits;
    char low_char;
#endif /*__STDC__*/
    {
    char* str;
    char* p;
    int d;

    /* Allocate memory for printed number.  Abort if error. */
    str = (char*) malloc( digits + 1 );
    if ( str == 0 )
	pm_error( "out of memory allocating number string" );

    /* Generate characters starting with least significant digit. */
    p = str + digits;
    *p-- = '\0';	/* nul terminate string */
    while ( p >= str )
	{
	d = i % base;
	i /= base;
	*p-- = low_char + d;
	}

    return str;
    }

#if __STDC__
static void
gen_cmap( colorhist_vector chv, int ncolors, pixval maxval, cixel_map cmap[MAXCOLORS], int* charsppP )
#else /*__STDC__*/
static void
gen_cmap( chv, ncolors, maxval, cmap, charsppP )
    colorhist_vector chv;
    int ncolors;
    pixval maxval;
    cixel_map cmap[MAXCOLORS];
    int* charsppP;
#endif /*__STDC__*/
    {
    int i, j, base;
    char* colorname;
    char* cp1;
    char* cp2;

    /* Figure out how many characters per pixel we'll be using.  Don't want
    ** to be forced to link with libm.a, so using a division loop rather than
    ** a log function.
    */
    base = (int) HIGH_CHAR - (int) LOW_CHAR + 1;
    for ( *charsppP = 0, i = ncolors; i > 0; ++(*charsppP) )
	i /= base;

    /* Generate the character-pixel string and the rgb name for each colormap
    ** entry.
    */
    for ( i = 0; i < ncolors; ++i )
	{
	/* Generate color name string. */
	colorname = ppm_colorname( &(chv[i].color), maxval, 0 );

	/* Check whether we've already assigned a character to this name,
	** via the closest-match code.
	*/
	for ( j = 0; j < i; ++j )
	    {
	    if ( cmap[j].rgbname != (char*) 0 &&
		 strcmp( colorname, cmap[j].rgbname ) == 0 )
		{
		cmap[i].cixel = cmap[j].cixel;
		cmap[i].rgbname = (char*) 0;
		cmap[i].barname = (char*) 0;
		goto nexti;
		}
	    }

	/* Copy it. */
	cmap[i].rgbname = (char*) malloc( strlen( colorname ) + 1 );
	if ( cmap[i].rgbname == 0 )
	    pm_error( "out of memory allocating color name" );
	(void) strcpy( cmap[i].rgbname, colorname );

	/* Copy it again and replace the blanks with underbars. */
	cmap[i].barname = malloc( strlen( colorname ) + 1 );
	if ( cmap[i].barname == 0 )
	    pm_error( "out of memory allocating color name", 0,0,0,0,0 );
	cp1 = colorname;
	cp2 = cmap[i].barname;
	for ( ; ; )
	    {
	    if ( *cp1 == ' ' )
		*cp2 = '_';
	    else
		*cp2 = *cp1;
	    if ( *cp1 == '\0' )
		break;
	    ++cp1;
	    ++cp2;
	    }

	/* Generate color value characters. */
	cmap[i].cixel = gen_numstr( i, base, LOW_CHAR, *charsppP );

	nexti: ;
	}
    }
