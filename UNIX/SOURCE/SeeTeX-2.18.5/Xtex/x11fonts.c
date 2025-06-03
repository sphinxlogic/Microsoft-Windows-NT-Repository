/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 
#include "xtex.h"
#include "dvi-simple.h"
#include "x11fonts.h"
#include "libtex/conv.h"
#include <assert.h>
#include <ctype.h>

extern Widget TopLevelWidget;

/* font handlers */

int MagAtShrink[] =
{-1,
   2489, 2074, 1728, 1440, 1200, 1098, 1000, 500,
   333, 250, 200, 166, 142, 125, 111, 0, -1};

#define MAX_SHRINK sizeof(MagAtShrink)/sizeof(int)

XFontStruct **FontsAtMag[ MAX_SHRINK + 2 ];

int FontRefCnt[ MAX_SHRINK + 2 ];


XFontStruct **
  TeXFontRef(usermag)
int usermag;
{
  int magSlot;
  
  for ( magSlot = MIN_SHRINK; magSlot <= MAX_SHRINK; magSlot++ ) {
    if ( usermag >= MagAtShrink[ magSlot ] )
      break;
  }
  
  assert( magSlot >= MIN_SHRINK && magSlot <= (MAX_SHRINK+1) );
  
  if ( FontsAtMag[ magSlot ] == 0 ) {
    
    int nameSize;
    char *name;
    char *args[128];
    int argCnt;
    char tmpStr[128];
    int lth;
    int missing;
    
    int font;
    
    XFontStruct **thisMag = (XFontStruct **)
      XtMalloc( sizeof( XFontStruct *) * (RegisteredFonts + 1) );
    
    for ( font = 0; font <= RegisteredFonts; font++ ) {
      thisMag[ font ] = 0;
    }
    
    /* try to load the fonts */
    
    missing = 0;
    
    for (font = 0; font < RegisteredFonts; font++ ) {
      
      /* get the canonical font name */
      
      char tmpStr[128];
      double dvimag = TheFontInfo[font].paf.paf_DVIMag;
      double dvidsz = TheFontInfo[font].paf.paf_DVIDesignSize;
      
      int mag = usermag;
      
      double dscaled
	= (dvimag * (double) ThePostAmbleInfo.pai_DVIMag) / dvidsz;
      int scaled = ROUND(dscaled);
      
      double dsuff
	= ((double) DviDpi * scaled * mag) / ((double) 1000.0 * 1000.0) ;
      int suff = ROUND(dsuff);
      
      int slop;
      int slopCount;
      
      for (slopCount = 0; slopCount < 3; slopCount++ ) {
	int len;
	char *p;
	
	slop = (slopCount > 1) ? -1 : slopCount;
	
	sprintf(tmpStr,"%s.%d",
		TheFontInfo[font].paf.paf_name, (int) suff + slop);
	
	/* convert to lower-case for X */
	
	len = strlen(tmpStr);
	for (p = tmpStr; len > 0; len--, p++) {
	  if ( isupper(*p) ) {
	    *p = tolower(*p);
	  }
	}

	/* It seems that, despite the XLoadQueryFont documentation,
	   some X implementations will re-load a font even if it is
	   already loaded. The NCD X terminal, under xremote, is an
	   example.  To avoid delays, specially visible at 9600 bauds,
	   we first explicitly check if the font information exists in
	   the server, which is an indication that the font is loaded.

	   Thanks to Martin Beaudoin for discovering this.
	 */

	thisMag[ font ]
	  = XQueryFont( XtDisplay ( TopLevelWidget ), (XID) tmpStr );
	if(thisMag[font]) break;

        thisMag[ font ]
	    	= XLoadQueryFont( XtDisplay ( TopLevelWidget ), tmpStr );
	if (thisMag[font]) break;
	
      }
      
      /*
       * The following added by mjk on 1-14-91 (until next comment),
       * but hacked a lot by dirk -- so don't blame either of us.
       *
       * This code will (optionally) fork a child to create a new font
       */

      if ( (thisMag[ font ]  == 0) && (*(xtexResources.makeFont)) ) {
 	char buf[1024];
 	char *p, *q;
	char *pc;
	
 	for (p=xtexResources.makeFont, q=buf; *p; p++)
 	  if (*p != '%')
 	    *q++ = *p ;
 	  else {
 	    switch (*++p) {
 	    case 'n' : case 'N' :
 	      (void)strcpy(q, TheFontInfo[font].paf.paf_name) ;
 	      break ;
 	    case 'd' : case 'D' :
 	      (void)sprintf(q, "%d", suff) ;
 	      break ;
 	    case 'b' : case 'B' :
 	      (void)sprintf(q, "%d", DviDpi) ;
 	      break ;
 	    case 'm' : case 'M' :
 	      (void)sprintf(q, "%d", scaled * mag / 1000) ;
 	      break;
 	    case 0 :    *q = 0 ;
 	      break ;
 	    default:    *q++ = *p ;
 	      *q = 0 ;
 	      break ;
 	    }
 	    q += strlen(q) ;
 	  }
	
 	*q = 0 ;
 	(void)strcat(buf, " 2>&1") ;
 	
 	/*	(void)system(buf) ; */

	/* Added the automakeFont resource to make all fonts without
	   requiring conformation. (MBoyer) */

	if (xtexResources.automakeFont)
	  pc = buf;
	else
	  pc = TeXConfirmString(TopLevelWidget,
				"Generate Font Using Command:", buf);

	if ( pc ) { 

	  TeXSystemCall(pc);

	  sprintf(tmpStr,"%s.%d",
		  TheFontInfo[font].paf.paf_name, (int) suff);
	
	  /* convert to lower-case for X */
	  
	  { int len;
	    char *p;
	    
	    len = strlen(tmpStr);
	    for (p = tmpStr; len > 0; len--, p++) {
	      if ( isupper(*p) ) {
		*p = tolower(*p);
	      }
	    }
 	  }
 	}
 	
 	thisMag[ font ]
 	  = XLoadQueryFont( XtDisplay ( TopLevelWidget ), tmpStr );
      }
      /* End of additions */
      
      
      
      if ( thisMag[ font ]  == 0 ) {
	missing++;
	
	/* correct the name of the missing font so it's not off by one */
	
	sprintf(tmpStr,"%s.%d",
		TheFontInfo[font].paf.paf_name, (int) suff);
	
	error(0,0,"Missing %s ( -mag %d -scaled %d %s )",
	      tmpStr, mag, scaled, TheFontInfo[font].paf.paf_name);
      }
    }
    
    FontsAtMag[ magSlot ] = thisMag;
  }
  
  FontRefCnt[ magSlot ] ++;
  
  return( FontsAtMag[ magSlot ] );
}

XFontStruct **
  TeXFontUnref(usermag)
int usermag;
{
  int magSlot;
  
  for ( magSlot = MIN_SHRINK; magSlot <= MAX_SHRINK; magSlot++ ) {
    if ( usermag >= MagAtShrink[ magSlot ] )
      break;
  }
  
  assert( magSlot >= MIN_SHRINK && magSlot <= MAX_SHRINK );
  
  if ( FontsAtMag [ magSlot]  ) {
    
    FontRefCnt[ magSlot ] --;
    
    if (FontRefCnt[ magSlot ] <= 0 ) {
      int font;
      XFontStruct **fonts = FontsAtMag[magSlot];
      
      /* fonts list is null terminated */
      
      for (font = 0; fonts[ font ] ; font++ ) {
	XFreeFont(XtDisplay( TopLevelWidget ), fonts[font] );
	fonts[ font ] = 0;
      }
      XtFree( (char *)FontsAtMag[ magSlot ] );
      FontsAtMag[ magSlot ] = 0;
      FontRefCnt[ magSlot ] = 0;
    }
  }
}

void
  TeXFontNewFile()
{
  int mag;
  for ( mag = 1; mag < MAX_SHRINK ; mag++ ) {
    TeXFontUnref( MagAtShrink[ mag ] );
  }
}
