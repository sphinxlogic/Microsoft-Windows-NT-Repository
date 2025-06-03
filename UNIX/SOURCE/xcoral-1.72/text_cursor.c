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

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef SVR4
#include <string.h>
#else
#include <strings.h>
#endif

#include "text.h"

static void GetCurrentXY ();
static int MyTextWidth ();
extern Display *dpy;

/*
**	Function name : TextCursorOn
**
**	Description : Allume le curseur.
**
**	Input : Le text courant.
**	Ouput :
*/
void TextCursorOn ( text )
	Text    *text;
{
	int x, y;
	char c;
	register int width;

	SetLinesTable ( text );
	(void) GetCurrentXY ( text, &x, &y, &c );

	if ( (c == '\t') || (c == 0) || (c == '\n') ) 
		width = text -> char_width_ave;
	else 
		width = XTextWidth ( text -> font, &c, 1 );

	text -> x_pos = x;
	text -> y_pos = y;
	text -> cursor_width = width;

	if ( text -> cursor_stat == ON )
		return;

	if ( text -> mouse_in == True ) {
		if ( text -> cursor_stat == FREESE )
			UnFreeseTextCursor ( text );
#ifdef DEBUG
		(void) fprintf ( stderr, "Cursor On\n" );
#endif
		XFillRectangle ( dpy, text -> window,text -> Igc,
			x, y, text -> cursor_width, text -> font_height );
		text -> cursor_stat = ON;
	}
	else
		FreeseTextCursor ( text );
}


/*
**	Function name : FreeseTextCursor
**
**	Description : Gele le curseur.
**	Input : Le text courant.
**	Ouput :
*/
void FreeseTextCursor ( text )
	Text *text;
{

#ifdef DEBUG
	(void) fprintf ( stderr, "FreeseCursor x = %d y = %d\n",
		text -> x_pos, text -> y_pos);
#endif
	if ( text -> cursor_stat != OFF )
		return;

	XDrawRectangle ( dpy, text -> window, text -> Igc,
		text -> x_pos, text -> y_pos,
		text -> cursor_width, text -> font_height );

	text -> cursor_stat = FREESE;
}


/*
**	Function name : UnFreeseTextCursor
**
**	Description : Degele le curseur.
**	Input : Le text courant.
**	Ouput :
*/
void UnFreeseTextCursor ( text )
	Text *text;
{
#ifdef DEBUG
	(void) fprintf ( stderr, "UnFreeseCursor x = %d y = %d\n",
		text -> x_pos, text -> y_pos);
#endif
	if ( text -> cursor_stat != FREESE )
		return;

	XDrawRectangle ( dpy, text -> window, text -> Igc,
		text -> x_pos, text -> y_pos,
		text -> cursor_width, text -> font_height );
	text -> cursor_stat = OFF;
}



/*
**	Function name : TextCursorOff
**
**	Description : Eteint le curseur.
**	Input : Le text courant.
**	Ouput :
*/
void TextCursorOff ( text )
	Text    *text;
{
	if ( text -> cursor_stat == OFF )
		return;

	if ( text -> cursor_stat == FREESE ) {
		UnFreeseTextCursor ( text );
		return;
	}
#ifdef DEBUG
	(void) fprintf ( stderr, "Cursor Off x = %d y = %d\n",
		text  -> x_pos, text -> y_pos);
#endif
	XFillRectangle ( dpy, text -> window,text -> Igc,
		text -> x_pos, text -> y_pos, text -> cursor_width,
		text -> font_height );
	text -> cursor_stat = OFF;
}


/*
**	Function name : MoveToXYinTextWindow
**
**	Description : Deplace le curseur a la position (x, y)
**		dans la page courante.
**
**	Input : Le text courant, la nouvelle position.
**	Ouput : 0 si Ok -1 sinon
*/
int MoveToXYinTextWindow ( text, x, y )
	Text *text;
	register int x, y;
{
	register char *s, *sh;
	register int i, cur_line, new_line, delta;
	char *HscrollString ();
	int len;

	x -= MARGE;
	y -= MARGE;

	cur_line = text -> n1 + 1;
	new_line = (y / text -> font_height) + 1;

	if ( new_line > text -> lines_in_page )
		new_line = text -> lines_in_page;

	if ( (delta = new_line - cur_line) == 0 ) {  /* meme ligne */
		s = GetCurrentLine ( text -> buf, &len );
		if ( text -> sl )
			sh = HscrollString ( s, text -> sl, text -> tab_width );
		else
			sh = s;
		for ( i = 1; i < (len -(sh -s) + 1); i++ ) { 
			if ( MyTextWidth ( text , sh, i ) > x ) 
				break;
		}
		i += (sh -s);
		(void) MoveHole ( text -> buf, i - GetNcFromLeft ( text -> buf ) - 1 );
		return 0;
	}
	if ( (delta + text -> no_current_line) > text -> lines_in_buf ) {
		return -1;		
	}
	(void) MoveToLine ( text -> buf, delta); /* On va sur la bonne ligne */
	text -> n1 += delta;
	text -> n2 -= delta;
	text -> no_current_line += delta;

	SetLinesTable ( text );
	s = GetCurrentLine ( text -> buf, &len );
	if ( text -> sl )
		sh = HscrollString ( s, text -> sl, text -> tab_width );
	else
		sh = s;
	for ( i = 1; i < (len -(sh-s)+1); i++ ) {
		if ( MyTextWidth ( text, sh, i ) > x ) 
			break;
	}
	i += (sh - s);
	(void) MoveHole ( text -> buf, i - 1 );
	return (0);
}


/*
**	Function name : MoveToBline
**
**	Description : Deplace le curseur au debut de la ligne courante
**	Input : Le text courant.
**	Ouput :
*/
void MoveToBline ( text )
	Text *text;
{
	register int n;

	if ( text -> sl )
		GotoLeft ( text );

	n = GetNcFromLeft ( text -> buf );
	(void) MoveHole ( text -> buf, -n );
}


/*
**	Function name : MoveToEline
**
**	Description : Deplace le curseur a la fin de la ligne courante.
**
**	Input : Le text courant.
**	Ouput :
*/
void MoveToEline ( text )
	Text *text;
{
	register int n;

 	n = GetNcFromRight ( text -> buf );
	(void) MoveHole ( text -> buf, n );
}


/*
**	Function name : UpCursor
**
**	Description : Deplace le curseur sur la ligne
**		precedente.
**	Input : Le text courant.
**	Ouput :
*/
void UpCursor ( text )
	Text *text;
{
#ifdef DEBUG
       (void) fprintf ( stderr, "Up cursor\n" );
#endif

       if ( text -> no_current_line == 1 ) {
	      klaxon ();
              return;
       }

       if ( text -> n1 == 0 )                /* il faut scroller */
	     CurrentLineToMiddle ( text );

       (void) MoveToXYinTextWindow ( text, text -> x_pos,
		text -> n1 * text -> font_height );
}


/*
**	Function name : DownCursor
**
**	Description : Delace le curseur sur la ligne suivante.
**	Input : Le text courant.
**	Ouput :
*/
void DownCursor ( text )
	Text *text;
{
       register int y;

#ifdef DEBUG
       (void) fprintf ( stderr, "Down cursor\n" );
#endif

       if ( text -> no_current_line >= text -> lines_in_buf ) {
	      klaxon ();
              return;
       }

       if ( text -> n2 == 0 ) {
	      CurrentLineToMiddle ( text );

	      y = text -> y_pos -
		(text -> n1 -1 ) * text -> font_height ;
       }
       else
              y = text -> y_pos + text -> font_height;

       (void) MoveToXYinTextWindow ( text, text -> x_pos, y );
}


/*
**	Function name : ForwardChar
**
**	Description : Deplace le curseur de 1 caractere vers la droite.
**	Input : Le text courant.
**	Ouput : True si Ok, False si fin de buffer.
*/
int ForwardChar ( text )
	Text *text;
{
       char c;
#ifdef DEBUG
       (void) fprintf ( stderr, "Forward char\n" );
#endif
       GetCurrentChar ( text -> buf, &c );
       if ( c == '\n' ) {
	      if ( text -> no_current_line > text -> lines_in_buf )
		     return False;
	      text -> no_current_line ++;
	      (void) MoveToLine ( text -> buf, 1 );
	      if ( text -> n2 == 0 ) {
		     CurrentLineToMiddle ( text );
	      }
	      else {
	             text -> n1 ++;
		     text -> n2 --;
	      }
       }
       else if ( c == '\0' ) {
#ifdef DEBUG
	      (void) fprintf ( stderr, " Fin du buffer\n" );
#endif
	      klaxon ();
	      return False;
       }
       else 
	      (void) MoveHole ( text -> buf, 1 );
       return True;
}


/*
**	Function name : BackwardChar
**
**	Description : Delplace le curseur de 1 caractere vers la gauche.
**	Input : Le text courant.
**	Ouput :
*/
void BackwardChar ( text )
Text *text;
{
       char c;
#ifdef DEBUG
       (void) fprintf ( stderr, "Backward char\n" );
#endif
       if ( MoveHole ( text -> buf, -1 ) < 0 ) {
	      klaxon ();
              return;
       }
       GetCurrentChar ( text -> buf, &c );
       if ( c == '\n' ) {
	      text -> no_current_line --;
	      if ( text -> n1 == 0 )
		     CurrentLineToMiddle ( text );
	      else {
	             text -> n1 --;
		     text -> n2 ++;
	      }
       }
}


/*
**	Function name : GetCurrentXY
**
**	Description : Retourne la position courante dans la page,
**		(x, y) ainsi que le caractere courant.
**	Input : Le text courant, la position et la caractere.
**	Ouput :
*/
static void GetCurrentXY ( text, x, y, c )
	Text *text;
	int *x, *y; 	/* RETURN */
	char *c;	/* RETURN */
{
	register char *s;  /* Current line */
	register char *cl; /* Current line from left */
	register int n, n0;
	int len, clipx;
	char *HscrollString ();

	if ( TextInBuf ( text ) == False ) {
		*x = *y = MARGE;
		*c = 0;
		return;
	}

	clipx = text -> width - ( 2 * MARGE );
	cl = GetCurrentLine ( text -> buf, &len );

	if ( text -> page.sline [ text -> n1 + 1 ] == 0 )
		(void) fprintf ( stderr, "Warning page.sline 0\n" );

	if ( text -> sl )
		s = HscrollString ( cl, text -> sl, text -> tab_width );
	else
		s = cl;

	if ( s == 0 ) 
		(void) fprintf ( stderr, "Internal Error 2...\n" );

	*y = MARGE + ( text -> n1 * text -> font_height );
	n0 = GetNcFromLeft ( text -> buf );
	n = n0 - ( s - cl );

	if ( (n <= 0) && (text -> sl) ) {
		do {
			text -> sl --;
			SetLinesTable ( text );
			s = text -> page.sline [ text -> n1 + 1 ];
			*x = MARGE + MyTextWidth ( text, s, n );
			n = n0 - ( s - cl );
		} while ( (n <= 0) && (text -> sl));
	       
		ClipOn ( text, 0 );
		ClearPage ( text );
		RefreshPage ( text );
		ClipOff ( text );
	}

	if ( ( *x = MARGE + MyTextWidth ( text, s, n )) > clipx ) {
		do {
			text -> sl ++;
			SetLinesTable ( text );
			s = text -> page.sline [ text -> n1 + 1 ];
			n = n0 - ( s - cl );
		} while ( ( *x = MARGE + MyTextWidth ( text, s, n )) > clipx );

		ClipOn ( text, 0 );
		ClearPage ( text );
		RefreshPage ( text );
		ClipOff ( text );
	} 

	if ( text -> no_current_line > text -> lines_in_buf )
		*x = MARGE;

	(void) strncpy ( c,(char *) (s+n), 1 );
#ifdef DEBUG
	(void) fprintf ( stderr, "n = %d x = %d c = %c\n", n, *x, *c );
#endif
}


/*
**	Function name : MyTextWidth
**
**	Description : Retourne la longueur en pixel des i premiers
**		caracteres de la chaine s.
**		Cette fonction tient compte des tabs.
**	Input : Le text courant, la chaine, nb caracteres.
**	Ouput : La longueur en pixel.
*/
static int MyTextWidth ( text, str, i )
	Text *text;
	register char *str;
	register int i;
{
        register char *p, *s;
	register int n = 1; /* Caractere en cours de traitement */
	register int xx, x, width;
	register int sizetab = text -> tab_width * text -> char_width_ave;

	xx = x = width = 0;
	p = s = str;

	if ( str == 0 ) 
	        (void) fprintf ( stderr, "Internal Error 3...\n" );

	while ( *p != '\n' ) {
	        if ( (*p == 0) || (i == 0) )
		        break;
		if ( *p == '\t' ) {
			xx =  sizetab;
			width = XTextWidth ( text -> font, s, n - 1); 
			while ( width >= xx ) 	
			        xx += sizetab;
			x += xx;
			s = ++p;
			n = 1;
		}
		else {
		        n++;
			p++;
		}
		i--;
	}
	return ( x + XTextWidth ( text -> font, s, n - 1 ));
}


/*
**	Function name : HscrollString
**
**	Description : Calcul du scroll horizontal.
**	Input : La chaine, le nb de tabulation et la taille de celle-ci
**	Ouput : Le premier caractere de la chaine corrspondant
**		a la tabulation souhaitee.
*/
char *HscrollString ( s, n, tabwidth )
	register char *s;
	register int n, tabwidth;
{
	register int d = n * tabwidth; 

	while ( d > 0 ) {
		if ( *s == '\0' )
			return 0;
		if ( *s == '\n' )
			break;
		if ( *s == '\t' ) {
			d -= tabwidth;
			s++;
			continue;
		}
		s++;
		d--;
	}
	return s;
}


/*
**	Function name : GotoLeft
**
**	Description : Met le scroll horizontal a zero.
**	Input : Le text courant.
**	Ouput :
*/
void GotoLeft ( text )
Text *text;
{
        text -> sl = 0;
	ClearPage ( text );
	SetAndDisplayPage ( text );
	DisplayMessage ( text -> mwin, " " );
	MCLEAR ( text -> stat, MESSAGE);
}

