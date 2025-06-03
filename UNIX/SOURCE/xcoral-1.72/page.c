/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

#include "buffer.h"
#include "text.h"
#include "scroll.h"

extern Display *dpy;
static int DrawLineOfText ();
static void ScrollUpFewLines ();
static void ScrollDownFewLines ();
static void ShiftLinesTable ();

/*
**	Function name : FirstPage
**
**	Description : Affichage de la premiere page du buffer courant.
**
**	Input : Le text courant.
**	Ouput :
*/
void FirstPage ( text )
	Text *text;
{
	text -> sl = 0;
	HoleToLeft ( text -> buf );
	text -> n1 = 0;
	text -> n2 = text -> lines_in_page - 1;
#ifdef DEBUG
	(void) fprintf ( stderr,"ht = %d\n", text -> height );
#endif
	SetAndDisplayPage ( text );

	if ( text -> stat & MESSAGE ) {
	                	DisplayMessage ( text -> mwin, "  " );
		MCLEAR ( text -> stat, MESSAGE );
	}

	(void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );
	text -> no_current_line = 1;

       	MSET ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, LASTPAGE );

#ifdef DEBUG
	(void) fprintf ( stderr, "cur_line = %d n1 = %d n2 = %d\n",
		 text -> no_current_line, text -> n1, text -> n2 );
#endif
}


/*
**	Function name : ClearPage
**
**	Description : Comme son nom l'indique.
**
**	Input : Le text courant.
**	Ouput :
*/
void ClearPage ( text )
	Text *text;
{
	XClearArea ( dpy, text -> window,
		MARGE, MARGE, text -> width - ( 2 * MARGE ),
		text -> height - ( 2 * MARGE ), False );
}


/*
**	Function name : ClipOn
**
**	Description : Met le clip a partir de la ligne no n
**		de la page courante.
**		La ligne n n'est pas touchee par le clip.
**
**	Input : Le text courant, no de ligne.
**	Ouput :
*/
void ClipOn ( text, line )
	Text *text;
	register int line;
{
	XRectangle rect [1];

	rect [0].x = MARGE;
	rect [0].width = text -> width - ( 2 * MARGE );

	if ( line <= 1 ) {
	       rect [0].y = MARGE;
	       rect [0].height = text -> height - ( 2 * MARGE );
	}
	else {
	       rect [0].y = MARGE + (line  * text -> font_height);
       	       rect [0].height = text -> height - ( 2 * MARGE )
		 - (line * text -> font_height );
	}

	XSetClipRectangles ( dpy, text -> Cgc, 0, 0, rect, 1, False );
}


/*
**	Function name : ClipOff
**
**	Description : Remet la zone de clip par defaut.
**
**	Input : Le text courant.
**	Ouput :
*/
void ClipOff ( text )
	Text *text;
{
	XSetClipMask ( dpy, text -> Cgc, None );
}


/*
**	Function name : ExposePage
**
**	Description : Exposition de la page courante.
**		Deux cas possibles : 
**			-Configure ( Resize )
**			-Expose-Region
**
**	Input : La region exposee, le text courant.
**	Ouput :
*/
void ExposePage ( r, text )
	Region r;
	Text *text;
{
	XRectangle rect [1];
	Region inter, region;

	if (  TextInBuf ( text ) != True )
		return;

	/* Configure */
	if ( text -> n1 > text -> lines_in_page - 1 ) {

	       text -> n1 = text -> lines_in_page - 1;
	       text -> n2 = 0;
	       SetAndDisplayPage ( text );
	       (void) MoveScrollBar ( dpy, text -> swin, CURRENT, text -> no_current_line - 1 - text -> n1 );
	       return;
	}

	/* Expose */
	region = XCreateRegion ();
	inter = XCreateRegion ();

	rect [0].x = MARGE;
	rect [0].y = MARGE;
	rect [0].width = text -> width - ( 2 * MARGE );
	rect [0].height = text -> height - ( 2 * MARGE );

	XUnionRectWithRegion ( rect, region, region );

	XIntersectRegion ( r, region, inter );
	XSetRegion ( dpy, text -> Cgc, inter );

	(void) SetLinesTable ( text );
	RefreshPage ( text );

	XDestroyRegion ( inter );
	XDestroyRegion ( region );
}


/*
**	Function name : SetLinesTable
**
**	Description : Mise a jour de la table des lignes
**		pour la page courante.
**
**	Input : Le text courant.
**	Ouput :
*/
void SetLinesTable ( text )
	Text *text;
{
	register int line, n = 0, wl = 1;
	register char *s;
	char *HscrollString ();
	int len;

	/*
	 * Si ya rien dans le buffer, il faut absolument
	 * virer le pointeur de la ligne courante car
	 * celui-ci est global a tous les buffers
	 * 
	 */
	if (  TextInBuf ( text ) != True ) {
		text -> page.sline [ text -> n1 + 1 ] = (char *) 0;
		return;
	}


   	for ( line = text -> n1; line > 0 ; line-- ) {
		s = ( char * ) GetBackwardLine ( text -> buf, -line, &len );
		if ( s ) {
		        if ( text -> sl )
			       s = HscrollString ( s, text -> sl, text -> tab_width ); 
			text -> page.sline [ wl ] = s;
		}
		else {
#ifdef DEBUG
        (void) fprintf ( stderr,"sline null wl = %d\n", wl );
#endif
		       text -> page.sline [ wl ] = ( char * ) 0;
		       n++;
		       continue;
		}
		wl++;
	}
	for ( line = 0; line < text -> n2 + 1 + n; line++ ) {
		if ( line == 0 )
			s = ( char * ) GetCurrentLine ( text -> buf, &len );
		else
			s = ( char * ) GetForwardLine ( text -> buf, line, &len );
		if ( s ) {
			if ( text -> sl )
				s = HscrollString ( s, text -> sl, text -> tab_width );
			text -> page.sline [ wl ] = s;
		}
		else {
#ifdef DEBUG
        (void) fprintf ( stderr,"sline null wl = %d\n", wl );
#endif
		  text -> page.sline [ wl ] = ( char * ) 0;
               		}
		wl ++;
	}
	text -> n1 -= n;
	text -> n2 += n;
}


/*
**	Function name : SetCurrentLine
**
**	Description : Affiche la ligne courante.
**
**	Input : Le text courant.
**	Ouput :
*/
void SetCurrentLine ( text )
	Text *text;
{
       register char *s;
       int len;
       register int line = text -> n1 + 1;
       register int max = text -> width - ( 2 * MARGE );

       ClearLine ( text, line );
       ClipOn ( text, 0 );

	s = ( char * ) GetCurrentLine ( text -> buf, &len ); 
       
       if ( s ) {
              if ( text -> sl )
		       s = (char *) HscrollString ( s, text -> sl, text -> tab_width );
	      text -> page.sline [ line ] = s;

	      text -> page.wline [ line ] =  DrawLineOfText ( text, line );

		     if ( text -> page.wline [ line ] > max )
		            text -> page.wline [ line ] = max;
	      
       }
       ClipOff ( text );
}


/*
**	Function name : RefreshPage
**
**	Description : Affiche la page courante.
**
**	Input : Le text courant.
**	Ouput :
*/
void RefreshPage ( text )
	Text *text;
{
	register int i = 1;
	register int max = text -> width - ( 2 * MARGE );

	do {
		ClearLine ( text, i );
		if ( text -> page.sline [ i ] ) {
			text -> page.wline [i] = DrawLineOfText ( text, i );
			if ( text -> page.wline [ i ] > max )
				text -> page.wline [ i ] = max;
		}
		else
			text -> page.wline [ i ] = 0;
		i++;
	} while ( i < text -> lines_in_page + 1);
}


/*
**	Function name : ClearLine
**
**	Description : Efface la ligne i.
**
**	Input : Le text courant, le no de la ligne.
**	Ouput :
*/
void ClearLine ( text, i )
	Text *text;
	register int i;
{
       register int width;

       XSetForeground ( dpy, text -> Cgc, text -> bg );

       if ( (width = text -> page.wline [i]) != 0 ) {
              XFillRectangle ( dpy, text -> window, text -> Cgc,
		     MARGE,
	             MARGE + ( (i - 1) * text -> font_height ),
		     width + 1, text -> font_height + 1 );

              text -> cursor_stat = OFF;
       }

       XSetForeground ( dpy, text -> Cgc, text -> fg );
}


/*
**	Function name : DrawLineOfText
**
**	Description : Affiche la ligne i en
**		tenant compte des tabs.
**
**	Input : Le text courant, le no de ligne.
**	Ouput :
*/
static int DrawLineOfText ( text, li )
	Text *text;
	register int li;
{
	register int n = 1;  /* Caracteres en cours de traitement    */
	register char  *p, *s;
	register int xx, width;

	register int sizetab = text -> tab_width * text -> char_width_ave;

	register int y = MARGE + text -> font -> max_bounds.ascent  
	                 + ( (li -1) * ( text -> font_height ));
	register int x = MARGE;
	register char *end_of_buf = BottomBuf ( text -> buf ); 
	register char *top_of_buf = TopBuf ( text -> buf ); 
	register int type = False;

	s = p = text -> page.sline [li];
	if ( (p >= top_of_buf) && (p <= end_of_buf) ) 
		type = True;

	while ( *p != '\n' ) {
		if ( *p == 0 ) 
         			break;
		if ( (type == True) && (p > end_of_buf) )
			(void) fprintf ( stderr, "Internal Error 1...\n" );
		if ( *p == '\t' ) {
			if ( n > 1 )
				XDrawImageString ( dpy, text -> window,
					text -> Cgc, x, y, s, n - 1 );
			xx =  sizetab;

			width = XTextWidth ( text -> font, s, n - 1); 
			while ( width >= xx ) 	

				xx += sizetab;
			x += xx;
			p++;
			n = 1;
			if ( (type == True) && (p > end_of_buf) ) 
				break;
			s = p;
		}
		else {
			n++;
			p++;
			if ( (type == True) && (p > end_of_buf) ) 
				break;
		}
	}
	XDrawImageString ( dpy, text -> window, text -> Cgc, x, y, s, n - 1 );
	return ( x + XTextWidth ( text -> font, s, n - 1 ));
}


/*
**	Function name : LastPage
**
**	Description : Affiche la derniere page du buffer courant.
**
**	Input : Le text courant.
**	Ouput :
*/
void LastPage ( text )
	Text *text;
{
	Buf *buf;

	buf = text -> buf;
	if ( text -> stat & LASTPAGE ) {
	       DisplayMessage ( text -> mwin, END_MESS );
	       MSET ( text -> stat, MESSAGE );
	       return;
	}
	HoleToRight ( buf );
	text -> no_current_line = text -> lines_in_buf;
	text -> n1 = text -> lines_in_page - 1;
	text -> n2 = 0;

	SetAndDisplayPage ( text );
	(void) MoveScrollBar ( dpy, text -> swin,
		CURRENT, text -> no_current_line - text -> n1 - 1 );
	DisplayMessage ( text -> mwin, " " );
	MSET ( text -> stat, LASTPAGE );
	MCLEAR ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, MESSAGE );

#ifdef DEBUG
	(void) fprintf ( stderr, "cur_line = %d n1 = %d n2 = %d\n",
		 text -> no_current_line, text -> n1, text -> n2 );
#endif
}


/*
**	Function name : PreviousPage
**
**	Description : Affiche la page precedente.
**
**	Input : Le text courant.
**	Ouput :
*/
void PreviousPage ( text )
	Text *text;
{
	Buf *buf;
	int len;

	buf = text -> buf;

#ifdef DEBUG
	(void) fprintf ( stderr, "n1 = %d ncl = %d\n",
		text -> n1, text -> no_current_line );
#endif
	if ( (text -> no_current_line == 1)
	    || (! GetBackwardLine ( buf, - ( text -> n1 + 1 ), &len ))) {
		DisplayMessage ( text -> mwin, "zz top..." );
		MSET ( text -> stat, MESSAGE );
		MSET ( text -> stat, FIRSTPAGE );
		return;
	}

	if ( MoveToLine ( buf, - text -> n1 ) == -1 ) {
		(void) fprintf ( stderr, "Move error\n" );
		return;
	}
	text -> no_current_line -= text -> n1;
	text -> n1 = text -> lines_in_page - 1;
	text -> n2 = 0;

	SetAndDisplayPage ( text );

	if ( text -> no_current_line - text -> n1 == 1 )
	       (void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );
	else
	       (void) MoveScrollBar ( dpy, text -> swin, PREVIOUS, 0 );

	DisplayMessage ( text -> mwin, " " );
	MCLEAR ( text -> stat, MESSAGE );
	MCLEAR ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, LASTPAGE );

#ifdef DEBUG
	(void) fprintf ( stderr, "cur_line = %d n1 = %d n2 = %d\n",
		 text -> no_current_line, text -> n1, text -> n2 );
#endif

}


/*
**	Function name : NextPage
**
**	Description : Affiche la page suivante.
**
**	Input : Le text courant.
**	Ouput :
*/
void NextPage ( text )
	Text *text;
{
	Buf *buf;

	buf = text -> buf;

#ifdef DEBUG
	(void) fprintf ( stderr, "n2 = %d\n", text -> n2 );
#endif
	if ( text -> stat & LASTPAGE ) {
		DisplayMessage ( text -> mwin, END_MESS );
		MSET ( text -> stat, MESSAGE );
		return;
	}

	if ( MoveToLine ( buf, text -> n2 ) == -1 ) { 
		DisplayMessage ( text -> mwin, END_MESS );
		MSET ( text -> stat, LASTPAGE );
		MSET ( text -> stat, MESSAGE );
		
		return;
	}

	text -> no_current_line += text -> n2;
	text -> n1 = 0;
	text -> n2 = text -> lines_in_page - 1;

	SetAndDisplayPage ( text );

	(void) MoveScrollBar ( dpy, text -> swin, NEXT, 0 );

	DisplayMessage ( text -> mwin, " " );
	MCLEAR ( text -> stat, MESSAGE );
	MCLEAR ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, LASTPAGE );

#ifdef DEBUG
	(void) fprintf ( stderr, "cur_line = %d n1 = %d n2 = %d\n",
		 text -> no_current_line, text -> n1, text -> n2 );
#endif
}


/*
**	Function name : GotoEnd
**
**	Description : Va a la fin du buffer.
**
**	Input : Le text courant.
**	Ouput :
*/
void GotoEnd ( text )
	Text *text;
{
	HoleToRight ( text -> buf );
	GotoLineNumber ( text, text -> lines_in_buf );
}


/*
**	Function name : CurrentLineToTop
**
**	Description : Met la ligne courante au debut de la
**		page.
**
**	Input : Le text courant.
**	Ouput :
*/
void CurrentLineToTop ( text )
	Text *text;
{
        register int n;

        text -> n1 = 0;
	text -> n2 = text -> lines_in_page - 1;

	SetAndDisplayPage ( text );
	
	n = text -> no_current_line - text -> n1 - 1;

	if ( n > 0 )
		(void) MoveScrollBar ( dpy, text -> swin, CURRENT, n );
	else
		(void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );
}


/*
**	Function name : GotoLineNumber
**
**	Description : Va a la ligne n.
**
**	Input : Le text courant, le no de ligne.
**	Ouput :
*/
void GotoLineNumber ( text, n )
	Text *text;
	register int n;
{
       if ( n < 1 ) return;
       if ( n > text -> lines_in_buf ) 
              n = text -> lines_in_buf;

       text -> sl = 0;
       (void) MoveToLine ( text -> buf, n - text -> no_current_line );

       text -> no_current_line += ( n - text -> no_current_line );
}


/*
**	Function name : ScrollNline
**
**	Description : Scroll n lignes de texte.
**		Vers le debut ou la fin du buffer suivant
**		le signe de n.
**
**	Input : Le nombre de ligne a scroller, le text courant.
**	Ouput :
*/
void ScrollNLine ( n, text )
	register int n;
	Text *text;
{
	Buf *buf;
#define PAS 5

	buf = text -> buf;
	if ( n < 0 ) {        /* On va vers le debut du buffer */
	        if ( -n > PAS ) {
		       (void) MoveToLine ( buf, n );
		       text -> no_current_line -= (-n);
		       if ( text -> no_current_line < text -> n1 ) {
		              text -> n1 = text -> no_current_line - 1;
			      text -> n2 = text -> lines_in_page - 1 - text -> n1;
		       }
		       SetAndDisplayPage ( text );
		}
		else
		       (void) ScrollUpFewLines ( text, n );
	 }
	 else {             /* On va vers ls fin du buffer */
	        if ( n > PAS ) {
		       register int tmp;
		       tmp = text -> lines_in_buf - text -> no_current_line;
		       if ( n <= tmp ) {
		              (void) MoveToLine ( buf, n );
			      text -> no_current_line += n;
		       }
		       else {
			      (void) MoveToLine ( buf, tmp );
		              text -> no_current_line += tmp;
			      text -> n1 -= ( n - tmp );
			      text -> n2 = text -> lines_in_page - 1 - text -> n1;
		       }
		       SetAndDisplayPage ( text );
		}
	        else 
		       (void) ScrollDownFewLines ( text, n );
	}
}
		       

/*
**	Function name : ScrollUpFewLines
**
**	Description : Scroll n lignes. Le texte defile
**		vers le bas.
**
**	Input : Le text courant, le nombre de ligne a scroller.
**	Ouput :
*/
static void ScrollUpFewLines ( text, n )
	Text *text;
	register int n;
{
	register Buf *buf;
	int len;

	buf = text -> buf;
#ifdef DEBUG
	(void) fprintf ( stderr, "Up n = %d\n", n );
#endif
	ClipOn ( text, 0 );
        while ( n ) {
	       (void) ShiftLinesTable ( text, DOWN );
	       text -> page.sline [1] = 
		GetBackwardLine ( buf, - ( text -> n1 + 1 ), &len ); 
	       RefreshPage ( text );

	       if ( text -> n2 == 0 ) {
	               (void) MoveToLine ( buf, -1 );
		       text -> no_current_line --;
	       }
	       else {
		       text -> n1 ++;
		       text -> n2 --;
	       }
	       n++;
	}
	ClipOff ( text );

	DisplayMessage ( text -> mwin, " " );
	MCLEAR ( text -> stat, MESSAGE );
	MCLEAR ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, LASTPAGE );

#ifdef DEBUG
	(void) fprintf ( stderr, "cur_line = %d n1 = %d n2 = %d\n",
		 text -> no_current_line, text -> n1, text -> n2 );
#endif
}


/*
**	Function name : ScrollDownFewLines
**
**	Description : Scroll n ligne. Le texte defile vers
**		le haut.
**
**	Input : Le text courant, le nombre de lignes a scroller.
**	Ouput :
*/
static void ScrollDownFewLines ( text, n )
	Text *text;
	register int n;
{
	register Buf *buf;
	int len;

	buf = text -> buf;
#ifdef DEBUG
	(void) fprintf ( stderr, "Down n = %d\n", n );
#endif
	ClipOn ( text, 0 );
        while ( n ) {
	       (void) ShiftLinesTable ( text, UP );
	       text -> page.sline [text -> lines_in_page] = 
		GetForwardLine ( buf, text -> n2 + 1, &len );
	       RefreshPage ( text );

	       if ( text -> n1 == 0 ) {
	               (void) MoveToLine ( buf, 1 );
		       text -> no_current_line ++;
	       }
	       else {
	              text -> n1 --;
		      text -> n2 ++;
	       }
	       n--;
       }
        ClipOff ( text );
	DisplayMessage ( text -> mwin, " " );
	MCLEAR ( text -> stat, MESSAGE );
	MCLEAR ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, LASTPAGE );

#ifdef DEBUG
	(void) fprintf ( stderr, "cur_line = %d n1 = %d n2 = %d\n",
		 text -> no_current_line, text -> n1, text -> n2 );
#endif
}


/*
**	Function name : ShiftLinesTable
**
**	Description : Delalage des pointeurs de la table
**		des lignes dans le cas d'un scroll.
**
**	Input : Le text courant, la direction du scroll.
**	Ouput :
*/
static void ShiftLinesTable ( text, dir )
	Text *text;
	register int dir;
{
       char  *save[256];
       register int i = 1;
       
       do {
              save [i] = text -> page.sline [i];
	      i++;
       } while ( i < text -> lines_in_page + 1 );

       switch ( dir ) {
       case UP: 
	      i = 1;
	      while ( i < text -> lines_in_page ) {
	             text -> page.sline [i] = save [i+1];
		     i++;
	      }
	      break;
       case DOWN:
	      i = 1;
	      while ( i < text -> lines_in_page ) {
	             text -> page.sline [i+1] = save [i];
		     i++;
	      }
	      break;
       }
}


/*
**	Function name : CurrentLineToMiddle
**
**	Description : Met la ligne courante au milieu
**		de la page.
**
**	Input : Le text courant.
**	Ouput :
*/
void CurrentLineToMiddle ( text )
	Text *text;
{
	register int n;

	if ( text -> no_current_line >= ( text -> lines_in_page / 2 )) { 
		text -> n1 = text -> lines_in_page / 2;
		text -> n2 = text -> lines_in_page - text -> n1 - 1;
	}
	else {
		text -> n1 = text -> no_current_line - 1;
		text -> n2 = text -> lines_in_page - text -> n1 - 1;
	}

	SetAndDisplayPage ( text );

	n = text -> no_current_line - text -> n1 - 1;

#ifdef DEBUG
	(void) fprintf ( stderr, "n = %d n1 = %d n2 = %d current = %d\n", 
		n, text -> n1, text -> n2, text -> no_current_line );
#endif
	
	if ( n != 0 )
		(void) MoveScrollBar ( dpy, text -> swin, CURRENT, n );
	else
		(void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );

	MCLEAR ( text -> stat, MESSAGE );
	MCLEAR ( text -> stat, FIRSTPAGE );
	MCLEAR ( text -> stat, LASTPAGE );
}


/*
**	Function name : SetAndDisplayPage
**
**	Description : Met la table des lignes a jour,
**		et affiche la page courante.
**
**	Input : Le text courant.
**	Ouput :
*/
void SetAndDisplayPage ( text )
	Text *text;
{
	ClipOn ( text, 0 );
	(void) SetLinesTable ( text );
	RefreshPage ( text );
	ClipOff ( text );
}


/*
**	Function name : ScrollDownCont
**
**	Description : Scroll tant que le 'ButtonPress' est 
**		maintenu. Le Texte defile vers le haut.
**	Input : 
**	Ouput :
*/
void ScrollDownCont ( text )
	Text *text;
{
       XEvent event;

       for (;;) {
	      if ( XCheckMaskEvent ( dpy,  ButtonReleaseMask, &event ) == True )
	             break;
	      if ( text -> no_current_line == text -> lines_in_buf ) continue;
              ScrollNLine ( 1, text );
	      (void) MoveScrollBar ( dpy, text -> swin, CURRENT,
	             text -> no_current_line - text -> n1 - 1 );
	      XSync ( dpy, False );
       }
}


/*
**	Function name : ScrollUpCont
**
**	Description : Scroll tant que le 'ButtonPress' est
**		maintenu. Le Texte defile vers le bas.
**
**	Input : Le text courant.
**	Ouput :
*/
void ScrollUpCont ( text )
Text *text;
{
       XEvent event;

       for (;;) {
	      if ( XCheckMaskEvent ( dpy,  ButtonReleaseMask, &event ) == True )
	             break;
	      if ( (text -> no_current_line - text -> n1) == 1 ) {
		     (void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );
		     continue;
	      }
	      ScrollNLine ( -1, text );
	      (void) MoveScrollBar ( dpy, text -> swin, CURRENT,
	             text -> no_current_line - text -> n1 - 1 );
	      XSync ( dpy, False );
       }
}


/*
**	Function name : klaxon
**
**	Description : 
**	Input : 
**	Ouput :
*/
klaxon ()
{
       XBell ( dpy, 5 );
}
