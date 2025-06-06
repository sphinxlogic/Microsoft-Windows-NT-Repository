/* $XConsortium: type1.c,v 1.5 91/10/10 11:20:06 rws Exp $ */
/* Copyright International Business Machines, Corp. 1991
 * All Rights Reserved
 * Copyright Lexmark International, Inc. 1991
 * All Rights Reserved
 * Portions Copyright (c) 1990 Adobe Systems Incorporated.
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM or Lexmark or Adobe
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 *
 * IBM, LEXMARK, AND ADOBE PROVIDE THIS SOFTWARE "AS IS", WITHOUT ANY
 * WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT
 * LIMITED TO ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  THE
 * ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE, INCLUDING
 * ANY DUTY TO SUPPORT OR MAINTAIN, BELONGS TO THE LICENSEE.  SHOULD ANY
 * PORTION OF THE SOFTWARE PROVE DEFECTIVE, THE LICENSEE (NOT IBM,
 * LEXMARK, OR ADOBE) ASSUMES THE ENTIRE COST OF ALL SERVICING, REPAIR AND
 * CORRECTION.  IN NO EVENT SHALL IBM, LEXMARK, OR ADOBE BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
/*********************************************************************/
/*                                                                   */
/* Type 1 module - Converting fonts in Adobe Type 1 Font Format      */
/*                 to scaled and hinted paths for rasterization.     */
/*                 Files: type1.c, type1.h, and blues.h.             */
/*                                                                   */
/* Authors:   Sten F. Andler, IBM Almaden Research Center            */
/*                 (Type 1 interpreter, stem & flex hints)           */
/*                                                                   */
/*            Patrick A. Casey, Lexmark International, Inc.          */
/*                 (Font level hints & stem hints)                   */
/*                                                                   */
/*********************************************************************/
 
/******************/
/* Include Files: */
/******************/
#include  "types.h"
#include  <stdio.h>          /* a system-dependent include, usually */
#include  <math.h>

#include  "objects.h"
#include  "spaces.h"
#include  "paths.h"
#include  "fonts.h"        /* understands about TEXTTYPEs */
#include  "pictures.h"     /* understands about handles */
 
typedef struct xobject xobject;
#include  "util.h"       /* PostScript objects */
#include  "fontfcn.h"
#include  "blues.h"          /* Blues structure for font-level hints */
 
/**********************************/
/* Type1 Constants and Structures */
/**********************************/
#define MAXSTACK 24        /* Adobe Type1 limit */
#define MAXCALLSTACK 10    /* Adobe Type1 limit */
#define MAXPSFAKESTACK 32  /* Max depth of fake PostScript stack (local) */
#define MAXSTRLEN 512      /* Max length of a Type 1 string (local) */
#define MAXLABEL 256       /* Maximum number of new hints */
#define MAXSTEMS 512       /* Maximum number of VSTEM and HSTEM hints */
#define EPS 0.001          /* Small number for comparisons */
 
/************************************/
/* Adobe Type 1 CharString commands */
/************************************/
#define HSTEM        1
#define VSTEM        3
#define VMOVETO      4
#define RLINETO      5
#define HLINETO      6
#define VLINETO      7
#define RRCURVETO    8
#define CLOSEPATH    9
#define CALLSUBR    10
#define RETURN      11
#define ESCAPE      12
#define HSBW        13
#define ENDCHAR     14
#define RMOVETO     21
#define HMOVETO     22
#define VHCURVETO   30
#define HVCURVETO   31

/* The following charstring code appears in some old Adobe font files
   in space and .notdef character and does not seems to do anything
   useful aside from taking two args from the stack. We allow this
   command and ignore it. The source code of ghostscript states that
   this command is obsolete *and* undocumented.
   This code may also appear in an Escape-sequence! */
#define UNKNOWN_15  15

/*******************************************/
/* Adobe Type 1 CharString Escape commands */
/*******************************************/
#define DOTSECTION       0
#define VSTEM3           1
#define HSTEM3           2
#define SEAC             6
#define SBW              7
#define DIV             12
#define CALLOTHERSUBR   16
#define POP             17
#define SETCURRENTPOINT 33
 

/* Note: We use routines from libm because in the original macro definitions,
   the evaluation order of tmpx is undefined by C-standard! */
#define USE_MATHLIB_ROUTINES

#ifdef USE_MATHLIB_ROUTINES

#define FABS(x) (fabs (x))
#define CEIL(x) ((LONG) ceil (x))
#define FLOOR(x) ((LONG) floor (x))

#else

/*****************/
/* Useful macros */
/*****************/
static DOUBLE tmpx;  /* Store macro argument in tmpx to avoid re-evaluation */
static LONG tmpi;    /* Store converted value in tmpi to avoid re-evaluation */
#define FABS(x) (((tmpx = (x)) < 0.0) ? -tmpx : tmpx)
#define CEIL(x) (((tmpi = (LONG) (tmpx = (x))) < tmpx) ? ++tmpi : tmpi)
#define FLOOR(x) (((tmpi = (LONG) (tmpx = (x))) > tmpx) ? --tmpi : tmpi)

#endif

#define ROUND(x) FLOOR((x) + 0.5)
#define ODD(x) (((int)(x)) & 01)

#define CC IfTrace1(TRUE, "Char \"%s\": ", currentchar)

/* To make some compiler happy we have to care about return  types! */
#define Errori {errflag = TRUE; return 0;}    /* integer */
#define Errord {errflag = TRUE; return 0.0;}  /* double */
#define Errorv {errflag = TRUE; return;}      /* void */
 
#define Error0i(errmsg) { CC; IfTrace0(TRUE, errmsg); Errori;}
#define Error0d(errmsg) { CC; IfTrace0(TRUE, errmsg); Errord;}
#define Error0v(errmsg) { CC; IfTrace0(TRUE, errmsg); Errorv;}
 
#define Error1i(errmsg,arg) { CC; IfTrace1(TRUE, errmsg, arg); Errori;}
#define Error1d(errmsg,arg) { CC; IfTrace1(TRUE, errmsg, arg); Errord;}
#define Error1v(errmsg,arg) { CC; IfTrace1(TRUE, errmsg, arg); Errorv;}
 
/********************/
/* global variables */
/********************/
struct stem {                     /* representation of a STEM hint */
    int vertical;                 /* TRUE if vertical, FALSE otherwise */
    DOUBLE x, dx;                 /* interval of vertical stem */
    DOUBLE y, dy;                 /* interval of horizontal stem */
    struct segment *lbhint, *lbrevhint;   /* left  or bottom hint adjustment */
    struct segment *rthint, *rtrevhint;   /* right or top    hint adjustment */
};
 
extern struct XYspace *IDENTITY;
 
static DOUBLE escapementX, escapementY;
static DOUBLE sidebearingX, sidebearingY;
static DOUBLE accentoffsetX, accentoffsetY;
 
static struct segment *path;
static int errflag;
 
/*************************************************/
/* Global variables to hold Type1Char parameters */
/*************************************************/
static char *Environment;
static char *currentchar;
static struct XYspace *CharSpace;
static psobj *CharStringP, *SubrsP, *OtherSubrsP;
static int *ModeP;
 
/************************/
/* Forward declarations */
/************************/
static DOUBLE Div();
static DOUBLE PSFakePop();
static int DoCommand();
static int Escape();
static int HStem();
static int VStem();
static int RLineTo();
static int RRCurveTo();
static int DoClosePath();
static int CallSubr();
static int Return();
static int EndChar();
static int RMoveTo();
static int DotSection();
static int Seac();
static int Sbw();
static int CallOtherSubr();
static int SetCurrentPoint();

/*****************************************/
/* statics for Flex procedures (FlxProc) */
/*****************************************/
static struct segment *FlxOldPath; /* save path before Flex feature */
 
/******************************************************/
/* statics for Font level hints (Blues) (see blues.h) */
/******************************************************/
static struct blues_struct *blues; /* the blues structure */
static struct alignmentzone alignmentzones[MAXALIGNMENTZONES];
int numalignmentzones;          /* total number of alignment zones */
 
/****************************************************************/
/* Subroutines for the Font level hints (Alignment zones, etc.) */
/****************************************************************/



/* Flags to control the rasterizer */
#define T1_IGNORE_FORCEBOLD           0x0001
#define T1_IGNORE_FAMILYALIGNMENT     0x0002
#define T1_IGNORE_HINTING             0x0004

#define T1_DEBUG_LINE                 0x0100
#define T1_DEBUG_REGION               0x0200
#define T1_DEBUG_PATH                 0x0400
#define T1_DEBUG_FONT                 0x0800
#define T1_DEBUG_HINT                 0x1000

int T1_Type1OperatorFlags; /* for manipulation from t1lib */


static void SetRasterFlags( void)
{

  if (T1_Type1OperatorFlags & T1_IGNORE_HINTING)
    ProcessHints=0;
  else
    ProcessHints=1;
  
  if ( T1_Type1OperatorFlags & T1_DEBUG_LINE)
    LineDebug=1;
  else
    LineDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_REGION)
    RegionDebug=1;
  else
    RegionDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_PATH)
    PathDebug=1;
  else
    PathDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_FONT)
    FontDebug=1;
  else
    FontDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_HINT)
    HintDebug=1;
  else
    HintDebug=0;
  return;
  
}


/******************************************/
/* Fill in the alignment zone structures. */
/******************************************/
static int ComputeAlignmentZones()
{
  int i;
  DOUBLE dummy, bluezonepixels, familyzonepixels;
  struct segment *p;
 
  numalignmentzones = 0;     /* initialize total # of zones */

  /* Remarks by RMz (Author of t1lib): The handling of substitution of
     the BlueValues by the FamilyBlues and correspondingly for the
     OtherBlues and FamilyOtherBlues is not clearly documented.
     These are the problems:
  
     1) Does the number of FamilyBlues entries need to be identical to
        that of BlueValues?

     2) Obviously, the order of the alignment zones in the BlueValues
        and the FamilyBlues need not be same (see TimesBold.pfa)

     3) Is it wise/recommended to make the substitution on a per
        alignment-zone level or global, i.e., if once then for all
	zones?

     4) The principle found below, checking the delta-height of an
        alignment-zone and making a decision based on this is incorrect.
	The decision has to be done according to absolute pixel values
	at which a feature would be rendered with the BlueValues and the
	FamilyBlues respectively.

     To conclude, it seems better to disable the Family-feature until
     these things are well-known/defined.
     */
  
  /* do the BlueValues zones */
  for (i = 0; i < blues->numBlueValues; i +=2, ++numalignmentzones) {
    /* the 0th & 1st numbers in BlueValues are for a bottom zone */
    /* the rest are topzones */
    if (i == 0)           /* bottom zone */
      alignmentzones[numalignmentzones].topzone = FALSE;
    else                  /* top zone */
      alignmentzones[numalignmentzones].topzone = TRUE;
    /* Check FamilyAlignment suppression */
    if ( (T1_Type1OperatorFlags & T1_IGNORE_FAMILYALIGNMENT)==0) {
      if (i < blues->numFamilyBlues) {    /* we must consider FamilyBlues */
	p = ILoc(CharSpace,0,blues->BlueValues[i] - blues->BlueValues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &bluezonepixels);
	Destroy(p);
	p = ILoc(CharSpace,0,blues->FamilyBlues[i] - blues->FamilyBlues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &familyzonepixels);
	Destroy(p);
	/* is the difference in size of the zones less than 1 pixel? */
	if (FABS(bluezonepixels - familyzonepixels) < 1.0) {
	  /* use the Family zones */
	  alignmentzones[numalignmentzones].bottomy =
	    blues->FamilyBlues[i];
	  alignmentzones[numalignmentzones].topy =
	    blues->FamilyBlues[i+1];
	  continue;
	}
      }
    }
    /* use this font's Blue zones */
    alignmentzones[numalignmentzones].bottomy = blues->BlueValues[i];
    alignmentzones[numalignmentzones].topy = blues->BlueValues[i+1];
  }
 
  /* do the OtherBlues zones */
  for (i = 0; i < blues->numOtherBlues; i +=2, ++numalignmentzones) {
    /* all of the OtherBlues zones are bottom zones */
    alignmentzones[numalignmentzones].topzone = FALSE;
    /* Check FamilyAlignment suppression */
    if ( (T1_Type1OperatorFlags & T1_IGNORE_FAMILYALIGNMENT)==0) {
      if (i < blues->numFamilyOtherBlues) {/* consider FamilyOtherBlues  */
	p = ILoc(CharSpace,0,blues->OtherBlues[i] - blues->OtherBlues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &bluezonepixels);
	Destroy(p);
	p = ILoc(CharSpace,0,blues->FamilyOtherBlues[i] -
		 blues->FamilyOtherBlues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &familyzonepixels);
	Destroy(p);
	/* is the difference in size of the zones less than 1 pixel? */
	if (FABS(bluezonepixels - familyzonepixels) < 1.0) {
	  /* use the Family zones */
	  alignmentzones[numalignmentzones].bottomy =
	    blues->FamilyOtherBlues[i];
	  alignmentzones[numalignmentzones].topy =
	    blues->FamilyOtherBlues[i+1];
	  continue;
	}
      }
    }
    /* use this font's Blue zones (as opposed to the Family Blues */
    alignmentzones[numalignmentzones].bottomy = blues->OtherBlues[i];
    alignmentzones[numalignmentzones].topy = blues->OtherBlues[i+1];
  }
  return(0);
  
}
 
/**********************************************************************/
/* Subroutines and statics for handling of the VSTEM and HSTEM hints. */
/**********************************************************************/
int InDotSection;             /* DotSection flag */
struct stem stems[MAXSTEMS];  /* All STEM hints */
int numstems;                 /* Number of STEM hints */
int currstartstem;            /* The current starting stem. */
int oldvert, oldhor;          /* Remember hint in effect */
int oldhorhalf, oldverthalf;  /* Remember which half of the stem */
DOUBLE wsoffsetX, wsoffsetY;  /* White space offset - for VSTEM3,HSTEM3 */
int wsset;                    /* Flag for whether we've set wsoffsetX,Y */
 
static int InitStems()  /* Initialize the STEM hint data structures */
{
  InDotSection = FALSE;
  currstartstem = numstems = 0;
  oldvert = oldhor = -1;
  return(0);
  
}
 
static int FinitStems()  /* Terminate the STEM hint data structures */
{
  int i;
 
  for (i = 0; i < numstems; i++) {
    Destroy(stems[i].lbhint);
    Destroy(stems[i].lbrevhint);
    Destroy(stems[i].rthint);
    Destroy(stems[i].rtrevhint);
  }
  return(0);
  
}
 
/*******************************************************************/
/* Compute the dislocation that a stemhint should cause for points */
/* inside the stem.                                                */
/*******************************************************************/
static int ComputeStem(stemno)
int stemno;
{
  int verticalondevice, idealwidth;
  DOUBLE stemstart, stemwidth;
  struct segment *p;
  int i;
  DOUBLE stembottom, stemtop, flatposition;
  DOUBLE Xpixels, Ypixels;
  DOUBLE unitpixels, onepixel;
  int suppressovershoot, enforceovershoot;
  DOUBLE stemshift, flatpospixels, overshoot;
  DOUBLE widthdiff; /* Number of character space units to adjust width */
  DOUBLE lbhintvalue, rthintvalue;
  DOUBLE cxx, cyx, cxy, cyy; /* Transformation matrix */
  int rotated; /* TRUE if character is on the side, FALSE if upright */
 
  /************************************************/
  /* DETERMINE ORIENTATION OF CHARACTER ON DEVICE */
  /************************************************/
 
  QuerySpace(CharSpace, &cxx, &cyx, &cxy, &cyy); /* Transformation matrix */
 
  if (FABS(cxx) < 0.00001 || FABS(cyy) < 0.00001)
    rotated = TRUE; /* Char is on side (90 or 270 degrees), possibly oblique. */
  else if (FABS(cyx) < 0.00001 || FABS(cxy) < 0.00001)
    rotated = FALSE; /* Char is upright (0 or 180 degrees), possibly oblique. */
  else {
    stems[stemno].lbhint = NULL; /* Char is at non-axial angle, ignore hints. */
    stems[stemno].lbrevhint = NULL;
    stems[stemno].rthint = NULL;
    stems[stemno].rtrevhint = NULL;
    return(0);
  }
 
  /* Determine orientation of stem */
 
  if (stems[stemno].vertical) {
    verticalondevice = !rotated;
    stemstart = stems[stemno].x;
    stemwidth = stems[stemno].dx;
  } else {
    verticalondevice = rotated;
    stemstart = stems[stemno].y;
    stemwidth = stems[stemno].dy;
  }
 
  /* Determine how many pixels (non-negative) correspond to 1 character space
     unit (unitpixels), and how many character space units (non-negative)
     correspond to one pixel (onepixel). */
 
  if (stems[stemno].vertical)
    p = ILoc(CharSpace, 1, 0);
  else
    p = ILoc(CharSpace, 0, 1);
  QueryLoc(p, IDENTITY, &Xpixels, &Ypixels);
  Destroy(p);
  if (verticalondevice)
    unitpixels = FABS(Xpixels);
  else
    unitpixels = FABS(Ypixels);
 
  onepixel = 1.0 / unitpixels;
 
  /**********************/
  /* ADJUST STEM WIDTHS */
  /**********************/
 
  widthdiff = 0.0;
 
  /* Find standard stem with smallest width difference from this stem */
  if (stems[stemno].vertical) { /* vertical stem */
    if (blues->StdVW != 0)      /* there is an entry for StdVW */
      widthdiff = blues->StdVW - stemwidth;
    for (i = 0; i < blues->numStemSnapV; ++i) { /* now look at StemSnapV */
      if (blues->StemSnapV[i] - stemwidth < widthdiff)
        /* this standard width is the best match so far for this stem */
        widthdiff = blues->StemSnapV[i] - stemwidth;
    }
  } else {                      /* horizontal stem */
    if (blues->StdHW != 0)      /* there is an entry for StdHW */
      widthdiff = blues->StdHW - stemwidth;
    for (i = 0; i < blues->numStemSnapH; ++i) { /* now look at StemSnapH */
      if (blues->StemSnapH[i] - stemwidth < widthdiff)
        /* this standard width is the best match so far for this stem */
        widthdiff = blues->StemSnapH[i] - stemwidth;
    }
  }
 
  /* Only expand or contract stems if they differ by less than 1 pixel from
     the closest standard width, otherwise make the width difference = 0. */
  if (FABS(widthdiff) > onepixel)
    widthdiff = 0.0;
 
  /* Expand or contract stem to the nearest integral number of pixels. */
  idealwidth = ROUND((stemwidth + widthdiff) * unitpixels);
  /* Ensure that all stems are at least one pixel wide. */
  if (idealwidth == 0)
    idealwidth = 1;

  /* Apply ForceBold to vertical stems. */
  if (blues->ForceBold && stems[stemno].vertical &&
      ((T1_Type1OperatorFlags & T1_IGNORE_FORCEBOLD)==0))
    /* Force this vertical stem to be at least DEFAULTBOLDSTEMWIDTH wide. */
    if (idealwidth < DEFAULTBOLDSTEMWIDTH)
      idealwidth = DEFAULTBOLDSTEMWIDTH;
  /* Now compute the number of character space units necessary */
  widthdiff = idealwidth * onepixel - stemwidth;

  /*********************************************************************/
  /* ALIGNMENT ZONES AND OVERSHOOT SUPPRESSION - HORIZONTAL STEMS ONLY */
  /*********************************************************************/
 
  stemshift = 0.0;
 
  if (!stems[stemno].vertical) {
 
    /* Get bottom and top boundaries of the stem. */
    stembottom = stemstart;
    stemtop = stemstart + stemwidth;
 
    /* Find out if this stem intersects an alignment zone (the BlueFuzz  */
    /* entry in the Private dictionary specifies the number of character */
    /* units to extend (in both directions) the effect of an alignment   */
    /* zone on a horizontal stem.  The default value of BlueFuzz is 1.   */
    for (i = 0; i < numalignmentzones; ++i) {
      if (alignmentzones[i].topzone) {
        if (stemtop >= alignmentzones[i].bottomy &&
            stemtop <= alignmentzones[i].topy + blues->BlueFuzz) {
          break; /* We found a top-zone */
        }
      } else {
        if (stembottom <= alignmentzones[i].topy &&
            stembottom >= alignmentzones[i].bottomy - blues->BlueFuzz) {
          break; /* We found a bottom-zone */
        }
      }
    }
 
    if (i < numalignmentzones) { /* We found an intersecting zone (number i). */
      suppressovershoot = FALSE;
      enforceovershoot = FALSE;
 
      /* When 1 character space unit is rendered smaller than BlueScale
         device units (pixels), we must SUPPRESS overshoots.  Otherwise,
         if the top (or bottom) of this stem is more than BlueShift character
         space units away from the flat position, we must ENFORCE overshoot. */
      
      if (unitpixels < blues->BlueScale){
        suppressovershoot = TRUE;
      }
      else{
        if (alignmentzones[i].topzone){
          if (stemtop >= alignmentzones[i].bottomy + blues->BlueShift){
            enforceovershoot = TRUE;
	  }
        else
          if (stembottom <= alignmentzones[i].topy - blues->BlueShift){
            enforceovershoot = TRUE;
	  }
	}
      }
      
      
      /*************************************************/
      /* ALIGN THE FLAT POSITION OF THE ALIGNMENT ZONE */
      /*************************************************/
 
      /* Compute the position of the alignment zone's flat position in
         device space and the amount of shift needed to align it on a
         pixel boundary. Move all stems this amount. */
 
      if (alignmentzones[i].topzone)
        flatposition = alignmentzones[i].bottomy;
      else
        flatposition = alignmentzones[i].topy;
 
      /* Find the flat position in pixels */
      flatpospixels = flatposition * unitpixels;
 
      /* Find the stem shift necessary to align the flat
         position on a pixel boundary, and use this shift for all stems */
      stemshift = (ROUND(flatpospixels) - flatpospixels) * onepixel;
 
      /************************************************/
      /* HANDLE OVERSHOOT ENFORCEMENT AND SUPPRESSION */
      /************************************************/
 
      /* Compute overshoot amount (non-negative) */
      if (alignmentzones[i].topzone)
        overshoot = stemtop - flatposition;
      else
        overshoot = flatposition - stembottom;
 
      if (overshoot > 0.0) {
        /* ENFORCE overshoot by shifting the entire stem (if necessary) so that
           it falls at least one pixel beyond the flat position. */
 
        if (enforceovershoot){
          if (overshoot < onepixel){
            if (alignmentzones[i].topzone)
              stemshift += onepixel - overshoot;
            else
              stemshift -= onepixel - overshoot;
	  }
	}
	
 
        /* SUPPRESS overshoot by aligning the stem to the alignment zone's
           flat position. */
 
        if (suppressovershoot){
          if (alignmentzones[i].topzone)
            stemshift -= overshoot;
          else
            stemshift += overshoot;
	}
      }
 
      /************************************************************/
      /* COMPUTE HINT VALUES FOR EACH SIDE OF THE HORIZONTAL STEM */
      /************************************************************/
 
      /* If the stem was aligned by a topzone, we expand or contract the stem
         only at the bottom - since the stem top was aligned by the zone.
         If the stem was aligned by a bottomzone, we expand or contract the stem
         only at the top - since the stem bottom was aligned by the zone. */
      if (alignmentzones[i].topzone) {
        lbhintvalue = stemshift - widthdiff; /* bottom */
        rthintvalue = stemshift;             /* top    */
      } else {
        lbhintvalue = stemshift;             /* bottom */
        rthintvalue = stemshift + widthdiff; /* top    */
      }

      stems[stemno].lbhint    = (struct segment *)Permanent(Loc(CharSpace, 0.0,  lbhintvalue));
      stems[stemno].lbrevhint = (struct segment *)Permanent(Loc(CharSpace, 0.0, -lbhintvalue));
      stems[stemno].rthint    = (struct segment *)Permanent(Loc(CharSpace, 0.0,  rthintvalue));
      stems[stemno].rtrevhint = (struct segment *)Permanent(Loc(CharSpace, 0.0, -rthintvalue));
 
      return(0);
 
    } /* endif (i < numalignmentzones) */
 
    /* We didn't find any alignment zones intersecting this stem, so
       proceed with normal stem alignment below. */
 
  } /* endif (!stems[stemno].vertical) */
 
  /* Align stem with pixel boundaries on device */
  stemstart = stemstart - widthdiff / 2;
  stemshift = ROUND(stemstart * unitpixels) * onepixel - stemstart;
 
  /* Adjust the boundaries of the stem */
  lbhintvalue = stemshift - widthdiff / 2; /* left  or bottom */
  rthintvalue = stemshift + widthdiff / 2; /* right or top    */
 
  if (stems[stemno].vertical) {
    stems[stemno].lbhint    = (struct segment *)Permanent(Loc(CharSpace,  lbhintvalue, 0.0));
    stems[stemno].lbrevhint = (struct segment *)Permanent(Loc(CharSpace, -lbhintvalue, 0.0));
    stems[stemno].rthint    = (struct segment *)Permanent(Loc(CharSpace,  rthintvalue, 0.0));
    stems[stemno].rtrevhint = (struct segment *)Permanent(Loc(CharSpace, -rthintvalue, 0.0));
  } else {
    stems[stemno].lbhint    = (struct segment *)Permanent(Loc(CharSpace, 0.0,  lbhintvalue));
    stems[stemno].lbrevhint = (struct segment *)Permanent(Loc(CharSpace, 0.0, -lbhintvalue));
    stems[stemno].rthint    = (struct segment *)Permanent(Loc(CharSpace, 0.0,  rthintvalue));
    stems[stemno].rtrevhint = (struct segment *)Permanent(Loc(CharSpace, 0.0, -rthintvalue));
  }
  return(0);
  
}
 
#define LEFT   1
#define RIGHT  2
#define BOTTOM 3
#define TOP    4
 
/*********************************************************************/
/* Adjust a point using the given stem hint.  Use the left/bottom    */
/* hint value or the right/top hint value depending on where the     */
/* point lies in the stem.                                           */
/*********************************************************************/
static struct segment *Applyhint(p, stemnumber, half)
struct segment *p;
int stemnumber, half;
{
  if (half == LEFT || half == BOTTOM)
    return Join(p, stems[stemnumber].lbhint); /* left  or bottom hint */
  else
    return Join(p, stems[stemnumber].rthint); /* right or top    hint */
}
 
/*********************************************************************/
/* Adjust a point using the given reverse hint.  Use the left/bottom */
/* hint value or the right/top hint value depending on where the     */
/* point lies in the stem.                                           */
/*********************************************************************/
static struct segment *Applyrevhint(p, stemnumber, half)
struct segment *p;
int stemnumber, half;
{
  if (half == LEFT || half == BOTTOM)
    return Join(p, stems[stemnumber].lbrevhint); /* left  or bottom hint */
  else
    return Join(p, stems[stemnumber].rtrevhint); /* right or top    hint */
}
 
/***********************************************************************/
/* Find the vertical and horizontal stems that the current point       */
/* (x, y) may be involved in.  At most one horizontal and one vertical */
/* stem can apply to a single point, since there are no overlaps       */
/* allowed.                                                            */
/*   The actual hintvalue is returned as a location.                   */
/* Hints are ignored inside a DotSection.                              */
/***********************************************************************/
static struct segment *FindStems(x, y, dx, dy)
DOUBLE x, y, dx, dy;
{
  int i;
  int newvert, newhor;
  struct segment *p;
  int newhorhalf, newverthalf;
 
  if (InDotSection) return(NULL);
 
  newvert = newhor = -1;
  newhorhalf = newverthalf = -1;
 
  for (i = currstartstem; i < numstems; i++) {
    if (stems[i].vertical) { /* VSTEM hint */
      if ((x >= stems[i].x - EPS) &&
          (x <= stems[i].x+stems[i].dx + EPS)) {
        newvert = i;
        if (dy != 0.0) {
          if (dy < 0) newverthalf = LEFT;
          else        newverthalf = RIGHT;
        } else {
          if (x < stems[i].x+stems[i].dx / 2) newverthalf = LEFT;
          else                                newverthalf = RIGHT;
        }
      }
    } else {                 /* HSTEM hint */
      if ((y >= stems[i].y - EPS) &&
          (y <= stems[i].y+stems[i].dy + EPS)) {
        newhor = i;
        if (dx != 0.0) {
          if (dx < 0) newhorhalf = TOP;
          else        newhorhalf = BOTTOM;
        } else {
          if (y < stems[i].y+stems[i].dy / 2) newhorhalf = BOTTOM;
          else                                newhorhalf = TOP;
        }
      }
    }
  }
 
  p = NULL;
 
  if (newvert == -1 && oldvert == -1) ; /* Outside of any hints */
  else if (newvert == oldvert &&
    newverthalf == oldverthalf); /* No hint change */
  else if (oldvert == -1) { /* New vertical hint in effect */
    p = Applyhint(p, newvert, newverthalf);
  } else if (newvert == -1) { /* Old vertical hint no longer in effect */
    p = Applyrevhint(p, oldvert, oldverthalf);
  } else { /* New vertical hint in effect, old hint no longer in effect */
    p = Applyrevhint(p, oldvert, oldverthalf);
    p = Applyhint(p, newvert, newverthalf);
  }
 
  if (newhor == -1 && oldhor == -1) ; /* Outside of any hints */
  else if (newhor == oldhor &&
    newhorhalf == oldhorhalf) ; /* No hint change */
  else if (oldhor == -1) { /* New horizontal hint in effect */
    p = Applyhint(p, newhor, newhorhalf);
  } else if (newhor == -1) { /* Old horizontal hint no longer in effect */
    p = Applyrevhint(p, oldhor, oldhorhalf);
  }
  else { /* New horizontal hint in effect, old hint no longer in effect */
    p = Applyrevhint(p, oldhor, oldhorhalf);
    p = Applyhint(p, newhor, newhorhalf);
  }
 
  oldvert = newvert; oldverthalf = newverthalf;
  oldhor  = newhor;  oldhorhalf  = newhorhalf;
 
  return p;
}
 
/******************************************************/
/* Subroutines and statics for the Type1Char routines */
/******************************************************/
 
static int strindex; /* index into PostScript string being interpreted */
static DOUBLE currx, curry; /* accumulated x and y values for hints */
 
struct callstackentry {
  psobj *currstrP;        /* current CharStringP */
  int currindex;          /* current strindex */
  unsigned short currkey; /* current decryption key */
  };
 
static DOUBLE Stack[MAXSTACK];
static int Top;
static struct callstackentry CallStack[MAXCALLSTACK];
static int CallTop;
static DOUBLE PSFakeStack[MAXPSFAKESTACK];
static int PSFakeTop;
 
static int ClearStack()
{
  Top = -1;
  return(0);
  
}
 
static int Push(Num)
        DOUBLE Num;
{
  if (++Top < MAXSTACK) Stack[Top] = Num;
  else Error0i("Push: Stack full\n");
  return(0);
  
}
 
static int ClearCallStack()
{
  CallTop = -1;
  return(0);
}
 
static int PushCall(CurrStrP, CurrIndex, CurrKey)
  psobj *CurrStrP;
  int CurrIndex;
  unsigned short CurrKey;
{
  if (++CallTop < MAXCALLSTACK) {
    CallStack[CallTop].currstrP = CurrStrP;   /* save CharString pointer */
    CallStack[CallTop].currindex = CurrIndex; /* save CharString index */
    CallStack[CallTop].currkey = CurrKey;     /* save decryption key */
  }
  else Error0i("PushCall: Stack full\n");
  return(0);
}
 
static int PopCall(CurrStrPP, CurrIndexP, CurrKeyP)
  psobj **CurrStrPP;
  int *CurrIndexP;
  unsigned short *CurrKeyP;
{
  if (CallTop >= 0) {
    *CurrStrPP = CallStack[CallTop].currstrP; /* restore CharString pointer */
    *CurrIndexP = CallStack[CallTop].currindex; /* restore CharString index */
    *CurrKeyP = CallStack[CallTop--].currkey;   /* restore decryption key */
  }
  else Error0i("PopCall: Stack empty\n");
  return(0);
}

 
static int ClearPSFakeStack()
{
  PSFakeTop = -1;
  return(0);
}
 
/* PSFakePush: Pushes a number onto the fake PostScript stack */
static int PSFakePush(Num)
  DOUBLE Num;
{
  if (++PSFakeTop < MAXPSFAKESTACK) PSFakeStack[PSFakeTop] = Num;
  else Error0i("PSFakePush: Stack full\n");
  return(0);
}
 
/* PSFakePop: Removes a number from the top of the fake PostScript stack */
static DOUBLE PSFakePop ()
{
  if (PSFakeTop >= 0) return(PSFakeStack[PSFakeTop--]);
  
  else Error0d("PSFakePop : Stack empty\n");

  /*NOTREACHED*/
}
 
/***********************************************************************/
/* Center a stem on the pixel grid -- used by HStem3 and VStem3        */
/***********************************************************************/
static struct segment *CenterStem(edge1, edge2)
    DOUBLE edge1;
    DOUBLE edge2;
{
  int idealwidth, verticalondevice;
  DOUBLE leftx, lefty, rightx, righty, center, width;
  DOUBLE widthx, widthy;
  DOUBLE shift, shiftx, shifty;
  DOUBLE Xpixels, Ypixels;
  struct segment *p;
 
  p = Loc(CharSpace, edge1, 0.0);
  QueryLoc(p, IDENTITY, &leftx, &lefty);
 
  p = Join(p, Loc(CharSpace, edge2, 0.0));
  QueryLoc(p, IDENTITY, &rightx, &righty);
  Destroy(p);
 
  widthx = FABS(rightx - leftx);
  widthy = FABS(righty - lefty);
 
  if (widthy <= EPS) { /* verticalondevice hint */
    verticalondevice = TRUE;
    center = (rightx + leftx) / 2.0;
    width = widthx;
  }
  else if (widthx <= EPS) { /* horizontal hint */
    verticalondevice = FALSE;
    center = (righty + lefty) / 2.0;
    width = widthy;
  }
  else { /* neither horizontal nor verticalondevice and not oblique */
    return (NULL);
  }
 
  idealwidth = ROUND(width);
  if (idealwidth == 0) idealwidth = 1;
  if (ODD(idealwidth)) {       /* is ideal width odd? */
    /* center stem over pixel */
    shift = FLOOR(center) + 0.5 - center;
  }
  else {
    /* align stem on pixel boundary */
    shift = ROUND(center) - center;
  }
 
  if (verticalondevice) {
    shiftx = shift;
    shifty = 0.0;
  } else {
    shifty = shift;
    shiftx = 0.0;
  }
 
  p = Loc(IDENTITY, shiftx, shifty);
  QueryLoc(p, CharSpace, &Xpixels, &Ypixels);
  wsoffsetX = Xpixels; wsoffsetY = Ypixels;
  currx += wsoffsetX; curry += wsoffsetY;
 
  return (p);
}
 
/*-----------------------------------------------------------------------
  Decrypt - From Adobe Type 1 book page 63, with some modifications
-----------------------------------------------------------------------*/
#define KEY 4330 /* Initial key (seed) for CharStrings decryption */
#define C1 52845 /* Multiplier for pseudo-random number generator */
#define C2 22719 /* Constant for pseudo-random number generator */
 
static unsigned short r; /* Pseudo-random sequence of keys */
 
static unsigned char Decrypt(cipher)
unsigned char cipher;
{
  unsigned char plain;
 
  plain = cipher ^ (r >> 8);
  r = (cipher + r) * C1 + C2;
  return plain;
}
 
/* Get the next byte from the codestring being interpreted */
static int DoRead(CodeP)
  int *CodeP;
{
  if (strindex >= CharStringP->len) return(FALSE); /* end of string */
  /* We handle the non-documented Adobe convention to use lenIV=-1 to
     suppress charstring encryption. */
  if (blues->lenIV==-1) {
    *CodeP = (unsigned char) CharStringP->data.stringP[strindex++];
  }
  else { 
    *CodeP = Decrypt((unsigned char) CharStringP->data.stringP[strindex++]);
  }
  
  return(TRUE);
}
 
/* Strip blues->lenIV bytes from CharString and update encryption key */
/* (the lenIV entry in the Private dictionary specifies the number of */
/* random bytes at the beginning of each CharString; default is 4)    */
static void StartDecrypt()
{
  int Code;
 
  r = KEY; /* Initial key (seed) for CharStrings decryption */
  for (strindex = 0; strindex < blues->lenIV;){
    if (!DoRead(&Code)) /* Read a byte and update decryption key */
      Error0v("StartDecrypt: Premature end of CharString\n");
  }
  
}

#undef DecodeDebug

static int Decode(Code)
  int Code;
{
  int Code1, Code2, Code3, Code4;
 
  if (Code <= 31){                           /* Code is [0,31]    */
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d -> Command\n", Code);
#endif
    DoCommand(Code);
  }
  else if (Code <= 246){                     /* Code is [32,246]  */
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d -> number=%f\n",
	    Code, (DOUBLE)(Code-139));
#endif
    Push((DOUBLE)(Code - 139));
  }
  else if (Code <= 250) {                   /* Code is [247,250] */
    if (!DoRead(&Code2)) goto ended;
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d next Code=%d -> number=%f\n",
	    Code, Code2, (DOUBLE)(((Code - 247) << 8) + Code2 + 108));
#endif
    Push((DOUBLE)(((Code - 247) << 8) + Code2 + 108));
  }
  else if (Code <= 254) {                   /* Code is [251,254] */
    if (!DoRead(&Code2)) goto ended;
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d, next Code=%d -> number=%f\n",
	    Code, Code2, (DOUBLE)( -((Code - 251) << 8) - Code2 - 108));
#endif
    Push((DOUBLE)( -((Code - 251) << 8) - Code2 - 108));
  }
  else {                                    /* Code is 255 */
    if (!DoRead(&Code1)) goto ended;
    if (!DoRead(&Code2)) goto ended;
    if (!DoRead(&Code3)) goto ended;
    if (!DoRead(&Code4)) goto ended;
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=255, Code1=%d, Code2=%d, Code3=%d, Code4=%d -> number=%f\n",
	    Code1, Code2, Code3, Code4,
	    (DOUBLE)((((((Code1<<8) + Code2)<<8) + Code3)<<8) + Code4));
#endif
    Push((DOUBLE)((((((Code1<<8) + Code2)<<8) + Code3)<<8) + Code4));
  }
  return(0);
 
ended: Error0i("Decode: Premature end of Type 1 CharString");
}

#undef DoCommandDebug 

/* Interpret a command code */
static int DoCommand(Code)
  int Code;
{
  switch(Code) {
    case HSTEM: /* |- y dy HSTEM |- */
#ifdef DoCommandDebug
      printf("DoCommand: HStem\n");
#endif
      /* Vertical range of a horizontal stem zone */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      HStem(Stack[0], Stack[1]);
      ClearStack();
      break;
    case VSTEM: /* |- x dx VSTEM |- */
#ifdef DoCommandDebug
      printf("DoCommand: VStem\n");
#endif
      /* Horizontal range of a vertical stem zone */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      VStem(Stack[0], Stack[1]);
      ClearStack();
      break;
    case VMOVETO: /* |- dy VMOVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: VMoveto\n");
#endif
      /* Vertical MOVETO, equivalent to 0 dy RMOVETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RMoveTo(0.0, Stack[0]);
      ClearStack();
      break;
    case RLINETO: /* |- dx dy RLINETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: RLineto\n");
#endif
      /* Like RLINETO in PostScript */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RLineTo(Stack[0], Stack[1]);
      ClearStack();
      break;
    case HLINETO: /* |- dx HLINETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: HLineto\n");
#endif
      /* Horizontal LINETO, equivalent to dx 0 RLINETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RLineTo(Stack[0], 0.0);
      ClearStack();
      break;
    case VLINETO: /* |- dy VLINETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: VLineto\n");
#endif
      /* Vertical LINETO, equivalent to 0 dy RLINETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RLineTo(0.0, Stack[0]);
      ClearStack();
      break;
    case RRCURVETO:
#ifdef DoCommandDebug
      printf("DoCommand: RRCurveto\n");
#endif
      /* |- dx1 dy1 dx2 dy2 dx3 dy3 RRCURVETO |- */
      /* Relative RCURVETO, equivalent to dx1 dy1 */
      /* (dx1+dx2) (dy1+dy2) (dx1+dx2+dx3) */
      /* (dy1+dy2+dy3) RCURVETO in PostScript */
      if (Top < 5) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RRCurveTo(Stack[0], Stack[1], Stack[2], Stack[3],
        Stack[4], Stack[5]);
      ClearStack();
      break;
    case CLOSEPATH: /* - CLOSEPATH |- */
#ifdef DoCommandDebug
      printf("DoCommand: ClosePath\n");
#endif
      /* Closes a subpath without repositioning the */
      /* current point */
      DoClosePath();
      ClearStack();
      break;
    case CALLSUBR: /* subr# CALLSUBR - */
#ifdef DoCommandDebug
      printf("DoCommand: CallSubr\n");
#endif
      /* Calls a CharString subroutine with index */
      /* subr# from the Subrs array */
      if (Top < 0) Error1i("DoCommand: Stack low\n (Code=%d)", Code);
      CallSubr((int)Stack[Top--]);
      break;
    case RETURN: /* - RETURN - */
#ifdef DoCommandDebug
      printf("DoCommand: Return\n");
#endif
      /* Returns from a Subrs array CharString */
      /* subroutine called with CALLSUBR */
      Return();
      break;
    case ESCAPE: /* ESCAPE to two-byte command code */
#ifdef DoCommandDebug
      printf("DoCommand: Escape to 2 Byte Code (Code=%d)\n", Code);
#endif
      if (!DoRead(&Code)) Error0i("DoCommand: ESCAPE is last byte\n");
      Escape(Code);
      break;
    case HSBW: /* |- sbx wx HSBW |- */
#ifdef DoCommandDebug
      printf("DoCommand: HSBW\n");
#endif
      /* Set the left sidebearing point to (sbx,0), */
      /* set the character width vector to (wx,0). */
      /* Equivalent to sbx 0 wx 0 SBW.  Space */
      /* character should have sbx = 0 */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Sbw(Stack[0], 0.0, Stack[1], 0.0);
      ClearStack();
      break;
    case ENDCHAR: /* - ENDCHAR |- */
#ifdef DoCommandDebug
      printf("DoCommand: EndChar\n");
#endif
      /* Finishes a CharString outline */
      EndChar();
      ClearStack();
      break;
    case RMOVETO: /* |- dx dy RMOVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: RMoveto\n");
#endif
      /* Behaves like RMOVETO in PostScript */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RMoveTo(Stack[0], Stack[1]);
      ClearStack();
      break;
    case HMOVETO: /* |- dx HMOVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: HMovetoUnassigned\n");
#endif
      /* Horizontal MOVETO. Equivalent to dx 0 RMOVETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RMoveTo(Stack[0], 0.0);
      ClearStack();
      break;
    case VHCURVETO: /* |- dy1 dx2 dy2 dx3 VHCURVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: VHCurveto\n");
#endif
      /* Vertical-Horizontal CURVETO, equivalent to */
      /* 0 dy1 dx2 dy2 dx3 0 RRCURVETO */
      if (Top < 3) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RRCurveTo(0.0, Stack[0], Stack[1], Stack[2],
              Stack[3], 0.0);
      ClearStack();
      break;
    case HVCURVETO: /* |- dx1 dx2 dy2 dy3 HVCURVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: HCurveto\n");
#endif
      /* Horizontal-Vertical CURVETO, equivalent to */
      /* dx1 0 dx2 dy2 0 dy3 RRCURVETO */
      if (Top < 3) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RRCurveTo(Stack[0], 0.0, Stack[1], Stack[2], 0.0, Stack[3]);
      ClearStack();
      break;
  case UNKNOWN_15:
    if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
    ClearStack();
    break;
    default: /* Unassigned command code */
#ifdef DoCommandDebug
      printf("DoCommand: Unassigned\n");
#endif
      ClearStack();
      Error1i("DoCommand: Unassigned code %d\n", Code);
  }
  return(0);
  
}
 
static int Escape(Code)
  int Code;
{
  int i, Num;
  struct segment *p;
 
  switch(Code) {
    case DOTSECTION: /* - DOTSECTION |- */
      /* Brackets an outline section for the dots in */
      /* letters such as "i", "j", and "!". */
      DotSection();
      ClearStack();
      break;
    case VSTEM3: /* |- x0 dx0 x1 dx1 x2 dx2 VSTEM3 |- */
      /* Declares the horizontal ranges of three */
      /* vertical stem zones between x0 and x0+dx0, */
      /* x1 and x1+dx1, and x2 and x2+dx2. */
      if (Top < 5) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      if (!wsset && ProcessHints) {
        /* Shift the whole character so that the middle stem is centered. */
        p = CenterStem(Stack[2] + sidebearingX, Stack[3]);
        path = Join(path, p);
        wsset = 1;
      }
 
      VStem(Stack[0], Stack[1]);
      VStem(Stack[2], Stack[3]);
      VStem(Stack[4], Stack[5]);
      ClearStack();
      break;
    case HSTEM3: /* |- y0 dy0 y1 dy1 y2 dy2 HSTEM3 |- */
      /* Declares the vertical ranges of three hori- */
      /* zontal stem zones between y0 and y0+dy0, */
      /* y1 and y1+dy1, and y2 and y2+dy2. */
      if (Top < 5) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      HStem(Stack[0], Stack[1]);
      HStem(Stack[2], Stack[3]);
      HStem(Stack[4], Stack[5]);
      ClearStack();
      break;
    case SEAC: /* |- asb adx ady bchar achar SEAC |- */
      /* Standard Encoding Accented Character. */
      if (Top < 4) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Seac(Stack[0], Stack[1], Stack[2],
        (unsigned char) Stack[3],
        (unsigned char) Stack[4]);
      ClearStack();
      break;
    case SBW: /* |- sbx sby wx wy SBW |- */
      /* Set the left sidebearing point to (sbx,sby), */
      /* set the character width vector to (wx,wy). */
      if (Top < 3) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Sbw(Stack[0], Stack[1], Stack[2], Stack[3]);
      ClearStack();
      break;
    case DIV: /* num1 num2 DIV quotient */
      /* Behaves like DIV in the PostScript language */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Stack[Top-1] = Div(Stack[Top-1], Stack[Top]);
      Top--;
      break;
    case CALLOTHERSUBR:
      /* arg1 ... argn n othersubr# CALLOTHERSUBR - */
      /* Make calls on the PostScript interpreter */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Num = Stack[Top-1];
      if (Top < Num+1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      for (i = 0; i < Num; i++) PSFakePush(Stack[Top - i - 2]);
      Top -= Num + 2;
      CallOtherSubr((int)Stack[Top + Num + 2]);
      break;
    case POP: /* - POP number */
      /* Removes a number from the top of the */
      /* PostScript interpreter stack and pushes it */
      /* onto the Type 1 BuildChar operand stack */
      Push(PSFakePop());
      break;
  case SETCURRENTPOINT: /* |- x y SETCURRENTPOINT |- */
    /* Sets the current point to (x,y) in absolute */
    /* character space coordinates without per- */
    /* forming a CharString MOVETO command */
    if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
    SetCurrentPoint(Stack[0], Stack[1]);
    ClearStack();
    break;
  case UNKNOWN_15:
    if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
    ClearStack();
    break;
  default: /* Unassigned escape code command */
    ClearStack();
    Error1i("Escape: Unassigned code %d\n", Code);
  }
  return(0);

}
 
/* |- y dy HSTEM |- */
/* Declares the vertical range of a horizontal stem zone */
/* between coordinates y and y + dy */
/* y is relative to the left sidebearing point */
static int HStem(y, dy)
  DOUBLE y, dy;
{
  IfTrace2((FontDebug), "Hstem %f %f\n", y, dy);
  if (ProcessHints) {
    if (numstems >= MAXSTEMS) Error0i("HStem: Too many hints\n");
    if (dy < 0.0) {y += dy; dy = -dy;}
    stems[numstems].vertical = FALSE;
    stems[numstems].x = 0.0;
    stems[numstems].y = sidebearingY + y + wsoffsetY;
    stems[numstems].dx = 0.0;
    stems[numstems].dy = dy;
    ComputeStem(numstems);
    numstems++;
  }
  return(0);
}
 
/* |- x dx VSTEM |- */
/* Declares the horizontal range of a vertical stem zone */
/* between coordinates x and x + dx */
/* x is relative to the left sidebearing point */
static int VStem(x, dx)
  DOUBLE x, dx;
{
  IfTrace2((FontDebug), "Vstem %f %f\n", x, dx);
  if (ProcessHints) {
    if (numstems >= MAXSTEMS) Error0i("VStem: Too many hints\n");
    if (dx < 0.0) {x += dx; dx = -dx;}
    stems[numstems].vertical = TRUE;
    stems[numstems].x = sidebearingX + x + wsoffsetX;
    stems[numstems].y = 0.0;
    stems[numstems].dx = dx;
    stems[numstems].dy = 0.0;
    ComputeStem(numstems);
    numstems++;
  }
  return(0);
}
 
/* |- dx dy RLINETO |- */
/* Behaves like RLINETO in PostScript */
static int RLineTo(dx, dy)
  DOUBLE dx, dy;
{
  struct segment *B;
 
  IfTrace2((FontDebug), "RLineTo %f %f\n", dx, dy);
 
  B = Loc(CharSpace, dx, dy);
 
  if (ProcessHints) {
    currx += dx;
    curry += dy;
    /* B = Join(B, FindStems(currx, curry)); */
    B = Join(B, FindStems(currx, curry, dx, dy));
  }
 
  path = Join(path, Line(B));
  return(0);
}
 
/* |- dx1 dy1 dx2 dy2 dx3 dy3 RRCURVETO |- */
/* Relative RCURVETO, equivalent to dx1 dy1 */
/* (dx1+dx2) (dy1+dy2) (dx1+dx2+dx3) */
/* (dy1+dy2+dy3) RCURVETO in PostScript */
static int RRCurveTo(dx1, dy1, dx2, dy2, dx3, dy3)
  DOUBLE dx1, dy1, dx2, dy2, dx3, dy3;
{
  struct segment *B, *C, *D;
 
  IfTrace4((FontDebug), "RRCurveTo %f %f %f %f ", dx1, dy1, dx2, dy2);
  IfTrace2((FontDebug), "%f %f\n", dx3, dy3);
 
  B = Loc(CharSpace, dx1, dy1);
  C = Loc(CharSpace, dx2, dy2);
  D = Loc(CharSpace, dx3, dy3);
 
  if (ProcessHints) {
    /* For a Bezier curve, we apply the full hint value to
       the Bezier C point (and thereby D point). */
    currx += dx1 + dx2 + dx3;
    curry += dy1 + dy2 + dy3;
    /* C = Join(C, FindStems(currx, curry)); */
    C = Join(C, FindStems(currx, curry, dx3, dy3));
  }
 
  /* Since XIMAGER is not completely relative, */
  /* we need to add up the delta values */
 
  C = Join(C, Dup(B));
  D = Join(D, Dup(C));
 
  path = Join(path, Bezier(B, C, D));
  return(0);
}
 
/* - CLOSEPATH |- */
/* Closes a subpath WITHOUT repositioning the */
/* current point */
static int DoClosePath()
{
  struct segment *CurrentPoint;
 
  IfTrace0((FontDebug), "DoClosePath\n");
  CurrentPoint = Phantom(path);
  path = ClosePath(path);
  path = Join(Snap(path), CurrentPoint);
  return(0);
}
 
/* subr# CALLSUBR - */
/* Calls a CharString subroutine with index */
/* subr# from the Subrs array */
static int CallSubr(subrno)
  int subrno;
{
  IfTrace1((FontDebug), "CallSubr %d\n", subrno);
  if ((subrno < 0) || (subrno >= SubrsP->len))
    Error0i("CallSubr: subrno out of range\n");
  PushCall(CharStringP, strindex, r);
  CharStringP = &SubrsP->data.arrayP[subrno];
  StartDecrypt();
  return(0);
}
 
/* - RETURN - */
/* Returns from a Subrs array CharString */
/* subroutine called with CALLSUBR */
static int Return()
{
  IfTrace0((FontDebug), "Return\n");
  PopCall(&CharStringP, &strindex, &r);
  return(0);
}
 
/* - ENDCHAR |- */
/* Finishes a CharString outline */
/* Executes SETCHACHEDEVICE using a bounding box */
/* it computes directly from the character outline */
/* and using the width information acquired from a previous */
/* HSBW or SBW.  It then calls a special version of FILL */
/* or STROKE depending on the value of PaintType in the */
/* font dictionary */
static int EndChar()
{
  IfTrace0((FontDebug), "EndChar\n");
 
  /* There is no need to compute and set bounding box for
     the cache, since XIMAGER does that on the fly. */
 
  /* Perform a Closepath just in case the command was left out */
  path = ClosePath(path);
 
  /* Set character width */
  path = Join(Snap(path), Loc(CharSpace, escapementX, escapementY));
  return(0);
 
}
 
/* |- dx dy RMOVETO |- */
/* Behaves like RMOVETO in PostScript */
static int RMoveTo(dx,dy)
  DOUBLE dx,dy;
{
  struct segment *B;
 
  IfTrace2((FontDebug), "RMoveTo %f %f\n", dx, dy);
 
  B = Loc(CharSpace, dx, dy);
 
  if (ProcessHints) {
    currx += dx;
    curry += dy;
    /* B = Join(B, FindStems(currx, curry)); */
    B = Join(B, FindStems(currx, curry, 0.0, 0.0));
  }
 
  path = Join(path, B);
  return(0);
}
 
/* - DOTSECTION |- */
/* Brackets an outline section for the dots in */
/* letters such as "i", "j", and "!". */
static int DotSection()
{
  IfTrace0((FontDebug), "DotSection\n");
  InDotSection = !InDotSection;
  return(0);
}
 
/* |- asb adx ady bchar achar SEAC |- */
/* Standard Encoding Accented Character. */
static int Seac(asb, adx, ady, bchar, achar)
  DOUBLE asb, adx, ady;
  unsigned char bchar, achar;
{
  int Code;
  struct segment *mypath;
 
  IfTrace4((FontDebug), "SEAC %f %f %f %d ", asb, adx, ady, bchar);
  IfTrace1((FontDebug), "%d\n", achar);
 
  /* Move adx - asb, ady over and up from base char's sbpoint. */
  /* (We use adx - asb to counteract the accents sb shift.) */
  /* The variables accentoffsetX/Y modify sidebearingX/Y in Sbw(). */
  /* Note that these incorporate the base character's sidebearing shift by */
  /* using the current sidebearingX, Y values. */
  accentoffsetX = sidebearingX + adx - asb;
  accentoffsetY = sidebearingY + ady;
 
  /* Set path = NULL to avoid complaints from Sbw(). */
  path = NULL;
 
  /* Go find the CharString for the accent's code via an upcall */
  CharStringP = GetType1CharString(Environment, achar);
  if (CharStringP == NULL) {
     Error1i("Invalid accent ('%03o) in SEAC\n", achar);
  }
  StartDecrypt();
 
  ClearStack();
  ClearPSFakeStack();
  ClearCallStack();
 
  for (;;) {
    if (!DoRead(&Code)) break;
    Decode(Code);
    if (errflag) return(0);
  }
  /* Copy snapped path to mypath and set path to NULL as above. */
  mypath = Snap(path);
  path = NULL;
 
  /* We must reset these to null now. */
  accentoffsetX = accentoffsetY = 0;
 
  /* go find the CharString for the base char's code via an upcall */
  CharStringP = GetType1CharString(Environment, bchar);
  StartDecrypt();
 
  ClearStack();
  ClearPSFakeStack();
  ClearCallStack();
 
  FinitStems();
  InitStems();
 
  for (;;) {
    if (!DoRead(&Code)) break;
    Decode(Code);
    if (errflag) return(0);
  }
  path = Join(mypath, path);
  return(0);
}
 
 
/* |- sbx sby wx wy SBW |- */
/* Set the left sidebearing point to (sbx,sby), */
/* set the character width vector to (wx,wy). */
static int Sbw(sbx, sby, wx, wy)
  DOUBLE sbx, sby, wx, wy;
{
  IfTrace4((FontDebug), "SBW %f %f %f %f\n", sbx, sby, wx, wy);
 
  escapementX = wx; /* Character width vector */
  escapementY = wy;
 
  /* Sidebearing values are sbx, sby args, plus accent offset from Seac(). */
  sidebearingX = sbx + accentoffsetX;
  sidebearingY = sby + accentoffsetY;
 
  path = Join(path, Loc(CharSpace, sidebearingX, sidebearingY));
  if (ProcessHints) {currx = sidebearingX; curry = sidebearingY;}
  return(0);
}
 
 /* num1 num2 DIV quotient */
/* Behaves like DIV in the PostScript language */
static DOUBLE Div(num1, num2)
  DOUBLE num1, num2;
{
  IfTrace2((FontDebug), "Div %f %f\n", num1, num2);
  return(num1 / num2);
}
 
/*
  The following four subroutines (FlxProc, FlxProc1, FlxProc2, and
  HintReplace) are C versions of the OtherSubrs Programs, which were
  were published in the Adobe Type 1 Font Format book.
 
  The Flex outline fragment is described by
    c1: (x0, y0) = c3: (x0, yshrink(y0)) or (xshrink(x0), y0)
     "  (x1, y1) =  "  (x1, yshrink(y1)) or (xshrink(x1), y1)
     "  (x2, y2) - reference point
    c2: (x0, y0) = c4: (x0, yshrink(y0)) or (xshrink(x0), y0)
     "  (x1, y1) =  "  (x1, yshrink(y1)) or (xshrink(x1), y1)
     "  (x2, y2) =  "  (x2, y2), rightmost endpoint
    c3: (x0, y0) - control point, 1st Bezier curve
     "  (x1, y1) - control point,      -"-
     "  (x2, y2) - end point,          -"-
    c4: (x0, y0) - control point, 2nd Bezier curve
     "  (x1, y1) - control point,      -"-
     "  (x2, y2) - end point,          -"-
    ep: (epY, epX) - final endpoint (should be same as c4: (x2, y2))
    idmin - minimum Flex height (1/100 pixel) at which to render curves
*/
 
#define dtransform(dxusr,dyusr,dxdev,dydev) { \
  register struct segment *point = Loc(CharSpace, dxusr, dyusr); \
  QueryLoc(point, IDENTITY, dxdev, dydev); \
  Destroy(point); \
}
 
#define itransform(xdev,ydev,xusr,yusr) { \
  register struct segment *point = Loc(IDENTITY, xdev, ydev); \
  QueryLoc(point, CharSpace, xusr, yusr); \
  Destroy(point); \
}
 
#define transform(xusr,yusr,xdev,ydev) dtransform(xusr,yusr,xdev,ydev)
 
#define PaintType (0)
 
#define lineto(x,y) { \
  struct segment *CurrentPoint; \
  DOUBLE CurrentX, CurrentY; \
  CurrentPoint = Phantom(path); \
  QueryLoc(CurrentPoint, CharSpace, &CurrentX, &CurrentY); \
  Destroy(CurrentPoint); \
  RLineTo(x - CurrentX, y - CurrentY); \
}
 
#define curveto(x0,y0,x1,y1,x2,y2) { \
  struct segment *CurrentPoint; \
  DOUBLE CurrentX, CurrentY; \
  CurrentPoint = Phantom(path); \
  QueryLoc(CurrentPoint, CharSpace, &CurrentX, &CurrentY); \
  Destroy(CurrentPoint); \
  RRCurveTo(x0 - CurrentX, y0 - CurrentY, x1 - x0, y1 - y0, x2 - x1, y2 - y1); \
}
 
#define xshrink(x) ((x - c4x2) * shrink +c4x2)
#define yshrink(y) ((y - c4y2) * shrink +c4y2)
 
#define PickCoords(flag) \
  if (flag) { /* Pick "shrunk" coordinates */ \
    x0 = c1x0; y0 = c1y0; \
    x1 = c1x1; y1 = c1y1; \
    x2 = c1x2; y2 = c1y2; \
    x3 = c2x0; y3 = c2y0; \
    x4 = c2x1; y4 = c2y1; \
    x5 = c2x2; y5 = c2y2; \
  } else { /* Pick original coordinates */ \
    x0 = c3x0; y0 = c3y0; \
    x1 = c3x1; y1 = c3y1; \
    x2 = c3x2; y2 = c3y2; \
    x3 = c4x0; y3 = c4y0; \
    x4 = c4x1; y4 = c4y1; \
    x5 = c4x2; y5 = c4y2; \
  }
 
/* FlxProc() = OtherSubrs[0]; Main part of Flex          */
/*   Calling sequence: 'idmin epX epY 3 0 callothersubr' */
/*   Computes Flex values, and renders the Flex path,    */
/*   and returns (leaves) ending coordinates on stack    */
static void FlxProc(c1x2, c1y2, c3x0, c3y0, c3x1, c3y1, c3x2, c3y2,
             c4x0, c4y0, c4x1, c4y1, c4x2, c4y2, epY, epX, idmin)
  DOUBLE c1x2, c1y2;
  DOUBLE c3x0, c3y0, c3x1, c3y1, c3x2, c3y2;
  DOUBLE c4x0, c4y0, c4x1, c4y1, c4x2, c4y2;
  DOUBLE epX, epY;
  int idmin;
{
  DOUBLE dmin;
  DOUBLE c1x0, c1y0, c1x1, c1y1;
  DOUBLE c2x0, c2y0, c2x1, c2y1, c2x2, c2y2;
  char yflag;
  DOUBLE x0, y0, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
  DOUBLE cxx, cyx, cxy, cyy; /* Transformation matrix */
  int flipXY;
  DOUBLE x, y;
  DOUBLE erosion = 1; /* Device parameter */
    /* Erosion may have different value specified in 'internaldict' */
  DOUBLE shrink;
  DOUBLE dX, dY;
  char erode;
  DOUBLE eShift;
  DOUBLE cx, cy;
  DOUBLE ex, ey;
 
  Destroy(path);
  path = FlxOldPath; /* Restore previous path (stored in FlxProc1) */
 
  if (ProcessHints) {
    dmin = TYPE1_ABS(idmin) / 100.0; /* Minimum Flex height in pixels */
 
    c2x2 = c4x2; c2y2 = c4y2; /* Point c2 = c4 */
 
    yflag = FABS(c1y2 - c3y2) > FABS(c1x2 - c3x2); /* Flex horizontal? */
 
    QuerySpace(CharSpace, &cxx, &cyx, &cxy, &cyy); /* Transformation matrix */
 
    if (FABS(cxx) < 0.00001 || FABS(cyy) < 0.00001)
      flipXY = -1; /* Char on side */
    else if (FABS(cyx) < 0.00001 || FABS(cxy) < 0.00001)
      flipXY = 1; /* Char upright */
    else
      flipXY = 0; /* Char at angle */
 
    if (yflag) { /* Flex horizontal */
      if (flipXY == 0 || c3y2 == c4y2) { /* Char at angle or Flex height = 0 */
        PickCoords(FALSE); /* Pick original control points */
      } else {
        shrink = FABS((c1y2 - c4y2) / (c3y2 - c4y2)); /* Slope */
 
        c1x0 = c3x0; c1y0 = yshrink(c3y0);
        c1x1 = c3x1; c1y1 = yshrink(c3y1);
        c2x0 = c4x0; c2y0 = yshrink(c4y0);
        c2x1 = c4x1; c2y1 = yshrink(c4y1);
 
        dtransform(0.0, ROUND(c3y2-c1y2), &x, &y); /* Flex height in pixels */
        dY = FABS((flipXY == 1) ? y : x);
        PickCoords(dY < dmin); /* If Flex small, pick 'shrunk' control points */
 
        if (FABS(y2 - c1y2) > 0.001) { /* Flex 'non-zero'? */
          transform(c1x2, c1y2, &x, &y);
 
          if (flipXY == 1) {
            cx = x; cy = y;
          } else {
            cx = y; cy = x;
          }
 
          dtransform(0.0, ROUND(y2-c1y2), &x, &y);
          dY = (flipXY == 1) ? y : x;
          if (ROUND(dY) != 0)
            dY = ROUND(dY);
          else
            dY = (dY < 0) ? -1 : 1;
 
          erode = PaintType != 2 && erosion >= 0.5;
          if (erode)
            cy -= 0.5;
          ey = cy + dY;
          ey = CEIL(ey) - ey + FLOOR(ey);
          if (erode)
            ey += 0.5;
 
          if (flipXY == 1) {
            itransform(cx, ey, &x, &y);
          } else {
            itransform(ey, cx, &x, &y);
          }
 
          eShift = y - y2;
          y1 += eShift;
          y2 += eShift;
          y3 += eShift;
        }
      }
    } else { /* Flex vertical */
      if (flipXY == 0 || c3x2 == c4x2) { /* Char at angle or Flex height = 0 */
        PickCoords(FALSE); /* Pick original control points */
      } else {
        shrink = FABS((c1x2 - c4x2) / (c3x2 - c4x2)); /* Slope */
 
        c1x0 = xshrink(c3x0); c1y0 = c3y0;
        c1x1 = xshrink(c3x1); c1y1 = c3y1;
        c2x0 = xshrink(c4x0); c2y0 = c4y0;
        c2x1 = xshrink(c4x1); c2y1 = c4y1;
 
        dtransform(ROUND(c3x2 - c1x2), 0.0, &x, &y); /* Flex height in pixels */
        dX = FABS((flipXY == -1) ? y : x);
        PickCoords(dX < dmin); /* If Flex small, pick 'shrunk' control points */
 
        if (FABS(x2 - c1x2) > 0.001) {
          transform(c1x2, c1y2, &x, &y);
          if (flipXY == -1) {
            cx = y; cy = x;
          } else {
            cx = x; cy = y;
          }
 
          dtransform(ROUND(x2-c1x2), 0.0, &x, &y);
          dX = (flipXY == -1) ? y : x;
          if (ROUND(dX) != 0)
            dX = ROUND(dX);
          else
            dX = (dX < 0) ? -1 : 1;
 
          erode = PaintType != 2 && erosion >= 0.5;
          if (erode)
            cx -= 0.5;
          ex = cx + dX;
          ex = CEIL(ex) - ex + FLOOR(ex);
          if (erode)
            ex += 0.5;
 
          if (flipXY == -1) {
            itransform(cy, ex, &x, &y);
          } else {
            itransform(ex, cy, &x, &y);
          }
 
          eShift = x - x2;
          x1 += eShift;
          x2 += eShift;
          x3 += eShift;
        }
      }
    }
 
    if (x2 == x5 || y2 == y5) {
      lineto(x5, y5);
    } else {
      curveto(x0, y0, x1, y1, x2, y2);
      curveto(x3, y3, x4, y4, x5, y5);
    }
  } else { /* ProcessHints is off */
    PickCoords(FALSE); /* Pick original control points */
    curveto(x0, y0, x1, y1, x2, y2);
    curveto(x3, y3, x4, y4, x5, y5);
  }
 
  PSFakePush(epY);
  PSFakePush(epX);
}
 
/* FlxProc1() = OtherSubrs[1]; Part of Flex            */
/*   Calling sequence: '0 1 callothersubr'             */
/*   Saves and clears path, then restores currentpoint */
static void FlxProc1()
{
  struct segment *CurrentPoint;
 
  CurrentPoint = Phantom(path);
 
  FlxOldPath = path;
  path = CurrentPoint;
}
 
/* FlxProc2() = OtherSubrs[2]; Part of Flex */
/*   Calling sequence: '0 2 callothersubr'  */
/*   Returns currentpoint on stack          */
static void FlxProc2()
{
  struct segment *CurrentPoint;
  DOUBLE CurrentX, CurrentY;
 
  CurrentPoint = Phantom(path);
  QueryLoc(CurrentPoint, CharSpace, &CurrentX, &CurrentY);
  Destroy(CurrentPoint);
 
  /* Push CurrentPoint on fake PostScript stack */
  PSFakePush(CurrentX);
  PSFakePush(CurrentY);
}
 
/* HintReplace() = OtherSubrs[3]; Hint Replacement            */
/*   Calling sequence: 'subr# 1 3 callothersubr pop callsubr' */
/*   Reinitializes stem hint structure                        */
static void HintReplace()
{
  /* Effectively retire the current stems, but keep them around for */
  /* revhint use in case we are in a stem when we replace hints. */
  currstartstem = numstems;
 
  /* 'subr#' is left on PostScript stack (for 'pop callsubr') */
}
 
/* arg1 ... argn n othersubr# CALLOTHERSUBR - */
/* Make calls on the PostScript interpreter (or call equivalent C code) */
/* NOTE: The n arguments have been pushed on the fake PostScript stack */
static int CallOtherSubr(othersubrno)
  int othersubrno;
{
  IfTrace1((FontDebug), "CallOtherSubr %d\n", othersubrno);
  
  switch(othersubrno) {
  case 0: /* OtherSubrs[0]; Main part of Flex */
    if (PSFakeTop < 16) Error0i("CallOtherSubr: PSFakeStack low");
    ClearPSFakeStack();
    FlxProc(
	    PSFakeStack[0],  PSFakeStack[1],  PSFakeStack[2],  PSFakeStack[3],
	    PSFakeStack[4],  PSFakeStack[5],  PSFakeStack[6],  PSFakeStack[7],
	    PSFakeStack[8],  PSFakeStack[9],  PSFakeStack[10], PSFakeStack[11],
	    PSFakeStack[12], PSFakeStack[13], PSFakeStack[14], PSFakeStack[15],
	    (int) PSFakeStack[16]
	    );
    break;
  case 1: /* OtherSubrs[1]; Part of Flex */
    FlxProc1();
    break;
  case 2: /* OtherSubrs[2]; Part of Flex */
    FlxProc2();
    break;
  case 3: /* OtherSubrs[3]; Hint Replacement */
    HintReplace();
    break;
  case 12: /* OtherSubrs[12]: Counter Control Hinting */
    /* We do nothing, because later OtherSubrs[13] will
       remove data from the PS-stack */
    break;
  case 13:
    /* We pop all elements from PSFakeStack. They may either have been left
       from previous calls to OtherSubr #12 or have been pushed for this
       call */
    ClearPSFakeStack();
  break;
  default: { /* call OtherSubrs[4] or higher if PostScript is present */
    
  }
  }
  return(0);
}
 
/* |- x y SETCURRENTPOINT |- */
/* Sets the current point to (x,y) in absolute */
/* character space coordinates without per- */
/* forming a CharString MOVETO command */
static int SetCurrentPoint(x, y)
  DOUBLE x, y;
{
  IfTrace2((FontDebug), "SetCurrentPoint %f %f\n", x, y);
 
  currx = x;
  curry = y;
  return(0);
}
 
/* The Type1Char routine for use by PostScript. */
/************************************************/
struct xobject *Type1Char(psfont *env, struct XYspace *S,
			  psobj *charstrP, psobj *subrsP,
			  psobj *osubrsP,
			  struct blues_struct *bluesP,
			  int *modeP, char *charname)
{
  int Code;
 
  path = NULL;
  errflag = FALSE;
 
  /* Make parameters available to all Type1 routines */
  currentchar=charname;
  Environment = (char *)env;
  CharSpace = S; /* used when creating path elements */
  CharStringP = charstrP;
  SubrsP = subrsP;
  OtherSubrsP = osubrsP;
  ModeP = modeP;
 
  blues = bluesP;
 
  /* compute the alignment zones */
  SetRasterFlags();
  ComputeAlignmentZones();
  StartDecrypt();
  ClearStack();
  ClearPSFakeStack();
  ClearCallStack();
  InitStems();
 
  currx = curry = 0;
  escapementX = escapementY = 0;
  sidebearingX = sidebearingY = 0;
  accentoffsetX = accentoffsetY = 0;
  wsoffsetX = wsoffsetY = 0;           /* No shift to preserve whitspace. */
  wsset = 0;                           /* wsoffsetX,Y haven't been set yet. */
 
  for (;;) {
    if (!DoRead(&Code)) break;
    Decode(Code);
    if (errflag) break;
  }
  FinitStems();

  /* Report a possible error: */
  *modeP=errflag;
  
  /* Clean up if an error has occurred */
  if (errflag) {
    if (path != NULL) {
      Destroy(path); /* Reclaim storage */
      path = NULL;   /* Indicate that character could not be built */
    }
  }
 
  return((struct xobject *) path);
}
 

/* We add a function to implement underlining in Type1-Routines. */
struct xobject *Type1Line(psfont *env, struct XYspace *S,
			  float line_position,
			  float line_thickness,
			  float line_length)
{
  
  path = NULL;
  errflag = FALSE;
  
  /* Make parameters available to all Type1 routines */
  Environment = (char *)env;
  CharSpace = S; /* used when creating path elements */

  currx = curry = 0;
  escapementX = escapementY = 0;
  sidebearingX = sidebearingY = 0;
  /* We have to initialize the stems-structures because
     the lineto commands refer to them! */
  SetRasterFlags();
  InitStems();
  
  /* Draw Character */
  Sbw(-1.0*line_length, 0.0, 0.0, 0.0);
  RMoveTo( 0.0,  (double) (line_position+0.5*line_thickness));
  RLineTo( 0.0, (double) -line_thickness);
  RLineTo( (double) line_length, 0.0);
  RLineTo( 0.0, (double) line_thickness);
  DoClosePath();  
  EndChar();

  /* De-Initialize the stems (of course there have not been
     defined any) */
  FinitStems();
  
  return((struct xobject *)path);
  
}

