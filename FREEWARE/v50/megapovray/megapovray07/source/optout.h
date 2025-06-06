/****************************************************************************
*                   optout.h
*
*  -------------------------------------------------------
*  ATTENTION:
*  This is an unofficial version of optout.h modified by
*  Ryoichi Suzuki, rsuzuki@etl.go.jp, for use  with 
*  "isosurface" shape type.
*  -------------------------------------------------------
*  WARNING: This is a changed copy of the original file.
*  Support of sphere sweep primitive added by Jochen Lippert.
*
*  This module contains all defines, typedefs, and prototypes for OPTOUT.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

/*
 * Modifications For OpenVMS By Robert Alan Byer <byer@mail.ourservers.net>
 * Feb. 12, 2001
 */

#ifndef OPTOUT_H
#define OPTOUT_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* These are used by OPTOUT.C and the machine specific modules */

/*
 * Define An Version Nmber For OpenVMS.
 */
#if defined (VMS) && defined(DECC)
#  define POV_RAY_VERSION "3.1g.0.7 (OpenVMS MegaPov 0.7)"
#else  
#  define POV_RAY_VERSION "3.1g.wmp.0.7 (WinMegaPov 0.7)"
#endif

#define OFFICIAL_VERSION_NUMBER 310
#ifdef UnofficialBlocking
#define UNOFFICIAL_VERSION_STRING "MegaPov"
#define UNOFFICIAL_VERSION_NUMBER 70
#define UNOFFICIAL_DEVELOPMENT_VERSION FALSE
#endif
  
/*
 * The compile script for OpenVMS will prompt the user to enter their
 * name and will define the DISTRIBUTION_MESSAGE_2 for the user.
 */
#if defined(VMS) && defined(DECC)
#  define DISTRIBUTION_MESSAGE_1 "This is an unofficial version (MegaPov) compiled by:"
#  if defined(DISTRIBUTION_MESSAGE_2)
#    undef DISTRIBUTION_MESSAGE_2
#  endif
#  include "vms_compile_name.h"
#  define DISTRIBUTION_MESSAGE_3 "The POV-Ray Team(tm) is not responsible for supporting this version."
#else
#  define DISTRIBUTION_MESSAGE_1 "This is an unofficial version (WinMegaPov) compiled by:"
#  define DISTRIBUTION_MESSAGE_2 "Nathan Kopp (email:  Nathan@Kopp.com)"
#  define DISTRIBUTION_MESSAGE_3 "The POV-Ray Team(tm) is not responsible for supporting this version."
#endif
  
/* Number of help pages (numbered 0 to MAX_HELP_PAGE). */

#define MAX_HELP_PAGE 7



/*****************************************************************************
* Global typedefs
******************************************************************************/




/*****************************************************************************
* Global variables
******************************************************************************/

/* These are available for GUI environments that may display them in a credits dialog */
extern char *Primary_Developers[];
extern char *Contributing_Authors[];


/*****************************************************************************
* Global functions
******************************************************************************/

void Print_Credits (void);
void Print_Options (void);
void Print_Stats (COUNTER *);
void Usage (int n, int f);
void Print_Help_Screens (void);
void Print_Authors (void);



#endif
