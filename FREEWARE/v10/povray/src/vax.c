/****************************************************************************
*                vax.c
*
*  This module contains VAX/VMS 5.3 specific routines.
*
*  from Persistence of Vision Raytracer
*  Copyright 1991 Persistence of Vision Team
*---------------------------------------------------------------------------
*  Copying, distribution and legal info is in the file povlegal.doc which
*  should be distributed with this file. If povlegal.doc is not available
*  or for more info please contact:
*
*       Drew Wells [POV-Team Leader]
*       CIS: 73767,1244  Internet: 73767.1244@compuserve.com
*       Phone: (213) 254-4041
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#include "config.h"

void display_finished ()
{
#ifdef XWINDOWS
  x_display_finished () ;
#endif
}

void display_init (unsigned width, unsigned height)
{
#ifdef XWINDOWS
  x_display_init (width, height) ;
#endif
}

void display_close ()
{
#ifdef XWINDOWS
  x_display_close () ;
#endif
}

void display_plot (x, y, Red, Green, Blue)
int x, y;
char Red, Green, Blue;
{
#ifdef XWINDOWS
  x_display_plot (x, y, Red, Green, Blue) ;
#endif
}
