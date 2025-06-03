/* pnmcrop.c - crop a portable anymap
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include "pnm.h"

#define LEFT 1
#define RIGHT 2
#define TOP 4
#define BOTTOM 8

#ifndef VMS
void
#endif
main( argc, argv )
    int argc;
    char* argv[];
    {
    FILE* ifp;
    xel** xels;
    register xel* xP;
    xel background;
    xelval maxval;
    int argn, format, backdefault, backblack;
    int rows, cols, row, col, newrows, newcols;
    int top, bottom, left, right;
    int which;
    char *usage = "[-white|-black] [-left] [-right] [-top] [-bottom] [pnmfile]";

#ifdef VMS
    getredirection (&argc, &argv);
#endif

    pnm_init( &argc, argv );

    argn = 1;
    backdefault = 1;
    which = 0;

    /* Check for flags. */
    if ( argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0' )
	{
	if ( pm_keymatch( argv[argn], "-white", 2 ) )
	    {
	    backdefault = 0;
	    backblack = 0;
	    }
	else if ( pm_keymatch( argv[argn], "-black", 2 ) )
	    {
	    backdefault = 0;
	    backblack = 1;
	    }
	else if ( pm_keymatch( argv[argn], "-left", 2 ) )
	    {
	    which |= LEFT;
	    }
	else if ( pm_keymatch( argv[argn], "-right", 2 ) )
	    {
	    which |= RIGHT;
	    }
	else if ( pm_keymatch( argv[argn], "-top", 2 ) )
	    {
	    which |= TOP;
	    }
	else if ( pm_keymatch( argv[argn], "-bottom", 2 ) )
	    {
	    which |= BOTTOM;
	    }
	else
	    pm_usage( usage );
	++argn;
	}

    if ( argn != argc )
	{
	ifp = pm_openr( argv[argn] );
	++argn;
	}
    else
	ifp = stdin;

    if ( argn != argc )
	pm_usage( usage );

    xels = pnm_readpnm( ifp, &cols, &rows, &maxval, &format );
    pm_close( ifp );

    if (!which)
        which = LEFT|RIGHT|TOP|BOTTOM;

    if ( backdefault )
	background = pnm_backgroundxel( xels, cols, rows, maxval, format );
    else
	if ( backblack )
	    background = pnm_blackxel( maxval, format );
	else
	    background = pnm_whitexel( maxval, format );

    /* Find first non-background line. */
    if (which & TOP) {
       for ( top = 0; top < rows; top++ )
	 for ( col = 0, xP = xels[top]; col < cols; col++, xP++ )
	     if ( ! PNM_EQUAL( *xP, background ) )
		 goto gottop;
    } else top = 0;
gottop:

    /* Find last non-background line. */
    if (which & BOTTOM) {
       for ( bottom = rows - 1; bottom >= top; bottom-- )
	 for ( col = 0, xP = xels[bottom]; col < cols; col++, xP++ )
	     if ( ! PNM_EQUAL( *xP, background ) )
		 goto gotbottom;
    } else bottom = rows - 1;
gotbottom:

    /* Find first non-background column.  To avoid massive paging on
    ** large anymaps, we use a different loop than the above two cases. */
    if (which & LEFT) {
       left = cols - 1;
       for ( row = top; row <= bottom; row++ )
	   {
	   int thisleft;

	   for ( thisleft = 0; thisleft < left; thisleft++ )
	       if ( ! PNM_EQUAL( xels[row][thisleft], background ) )
		   {
		   left = thisleft;
		   break;
		   }
	   }
    } else left = 0;

    /* Find last non-background column.  Again, use row-major loop. */
    if (which & RIGHT) {
       right = left + 1;
       for ( row = top; row <= bottom; row++ )
	   {
	   int thisright;

	   for ( thisright = cols - 1; thisright > right; thisright-- )
	       if ( ! PNM_EQUAL( xels[row][thisright], background ) )
		   {
		   right = thisright;
		   break;
		   }
	   }
    } else right = cols - 1;

#define ending(n) (((n) > 1) ? "s" : "")
    if ( top > 0 )
	pm_message( "cropping %d row%s off the top", top, ending(top), 0,0,0 );
    if ( bottom < rows - 1 )
	pm_message( "cropping %d row%s off the bottom", rows-1-bottom, ending(rows-1-bottom), 0,0,0 );
    if ( left > 0 )
	pm_message( "cropping %d col%s off the left", left, ending(left), 0,0,0 );
    if ( right < cols - 1 )
	pm_message( "cropping %d col%s off the right", cols-1-right, ending(cols-1-right), 0,0,0 );

    /* Now write out the new anymap. */
    newcols = right - left + 1;
    newrows = bottom - top + 1;
    pnm_writepnminit( stdout, newcols, newrows, maxval, format, 0 );
    for ( row = top; row <= bottom; row++ )
	pnm_writepnmrow( stdout, &(xels[row][left]), newcols, maxval, format, 0 );
    pm_close( stdout );

    exit( 0 );
    }
