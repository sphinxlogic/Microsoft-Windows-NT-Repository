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

#include "text.h"

#define KILLREGION		0
#define COPYREGION	1

extern Display *dpy;
extern void StoreInKillBuf ();
static void SmallRegion (), BigRegion ();

/*
**	Function name : SetMark
**
**	Description : Marque la position courante.
**
**	Input : Le text courant.
**	Ouput :
*/
void SetMark ( text )
	Text *text;
{
	text -> markline = text -> no_current_line;
	text -> markpos = GetNcFromLeft ( text -> buf );
	DisplayMessage ( text -> mwin, "Set Mark" );
}


/*
**	Function name : ExchangePointMark, 
**
**	Description : Va de la marque au curseur et inversement.
**
**	Input : Le text courant.
**	Ouput :
*/
void ExchangePointMark ( text)
	Text *text;
{
	register int pos, cline;

	TextCursorOn ( text );
	/* On sauve la position courante */
	pos = GetNcFromLeft ( text -> buf );
#ifdef DEBUG
	(void) fprintf ( stderr, "current = %d mark = %d\n",
		pos, text -> markpos );
#endif
	cline = text -> no_current_line;
   	if ( (cline == text -> markline) && (pos == text -> markpos) ) 
   		return;
   
	if ( text -> markline != cline ) {
		GotoLineNumber ( text, text -> markline );
		if ( text -> markpos != 0 )
			(void) MoveHole ( text -> buf, text -> markpos );
		CurrentLineToMiddle ( text );
		SetAndDisplayPage ( text );
		if ( text -> no_current_line != 1 )
			(void) MoveScrollBar ( dpy, text -> swin, CURRENT,
				text -> no_current_line - text -> n1 - 1 );
		else
			(void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );
	}
   	else {
		(void) MoveHole ( text -> buf,  text -> markpos - pos );
		SetCurrentLine ( text );
    	}
	text -> markline = cline;
	text -> markpos = pos;
	TextCursorOn ( text );
}


/*
**	Function name : KillRegion
**
**	Description : Efface le texte, de la marque au cursor.
**
**	Input :  Le text courant.
**	Ouput :
*/
void KillRegion ( text )
	Text *text;
{
	if ( text -> markline == 0 ) {
		DisplayMessage ( text -> mwin, "No mark" );
		return;
	}
   	if ( text -> no_current_line == text -> markline ) 
		(void) SmallRegion ( text, KILLREGION );
   	else 
		(void) BigRegion ( text, KILLREGION );
	TextCursorOn ( text );
}

/*
**	Function name : CopyRegion
**
**	Description : Copie de la marque au curseur.
**
**	Input : Le text courant.
**	Ouput :
*/
void CopyRegion ( text )
Text *text;
{
	if ( text -> markline == 0 ) {
		DisplayMessage ( text -> mwin, "No mark" );
		return;
	}

   	if ( text -> no_current_line == text -> markline ) 
		(void) SmallRegion ( text, COPYREGION );
   	else 
		(void) BigRegion ( text, COPYREGION );
	TextCursorOn ( text );
}

/*
**	Function name : SmallRegion
**
**	Description : Copie ou efface une region contenue
**		dans la ligne courante.
**
**	Input : Le text courant, l'operation a effectuer.
**	Ouput :
*/
static void SmallRegion ( text, op )
	Text *text;
	register int op;
{
	register int diff;
	register int c_pos;
	char buf [32];
 
	/*
	 * La marque et le curseur sont sur la meme ligne
	 */
	TextCursorOff ( text );
	c_pos = GetNcFromLeft ( text -> buf );
	if ( (diff = text -> markpos - c_pos) == 0 ) {
		TextCursorOn ( text );
		return;
	}
	if ( diff > 0 ) {
		/* curseur avant la marque */
		StoreInKillBuf ( RightBuf ( text -> buf ) , diff, 1 ); 
		if ( op = KILLREGION ) {
			(void) MoveHole ( text -> buf, diff );
			DeleteNchar ( text -> buf, diff );
		}
		else {
			(void) sprintf ( buf, "%d bytes copied", diff );
			DisplayMessage ( text -> mwin, buf );
		}
	}
	else if ( diff < 0 ) {
		/* curseur apres la marque */
		StoreInKillBuf ( LeftBuf ( text -> buf ) + diff + 1, - diff, 1 ); 
		if ( op == KILLREGION ) {
			DeleteNchar ( text -> buf, -diff );
		}
		else {
			(void) sprintf ( buf, "%d bytes copied", -diff );
			DisplayMessage ( text -> mwin, buf );
		}
	}
	SetCurrentLine ( text );
	if ( op == KILLREGION )
		SetTextModif ( text);
}


/*
**	Function name : BigRegion
**
**	Description : Copie ou efface une region de plusieurs lignes.
**
**	Input : Le text courant, l'operation a effectuer.
**	Ouput :
*/
static void BigRegion ( text, op )
	Text *text;
	register int op;
{
	register int diff, lines;
	int len;
	char buf [32];

	if ( (lines = text -> no_current_line - text -> markline) > 0 ) 
      		(void) GetBackwardLine ( text -> buf, -lines, &len );
	else
		(void) GetForwardLine ( text -> buf,  -lines, &len );
         		
	if ( (diff = len + text -> markpos) < 0 )
		diff *= -1;
      
	if ( lines > 0 )
           		StoreInKillBuf ( LeftBuf ( text -> buf ) - diff + 1, diff, lines );
	else {
		StoreInKillBuf ( RightBuf ( text -> buf ) , diff, -lines );
		if ( op == KILLREGION )
			(void) MoveHole ( text -> buf, diff );
	}
     	if ( op == KILLREGION ) {
		DeleteNchar ( text -> buf, diff );
		if ( lines > 0 )
  			text -> no_current_line -= lines;
		text -> lines_in_buf = GetNumberOfLineInBuf ( text -> buf ); 
		SetScrollLine ( text -> swin , text -> lines_in_buf );
		SetScrollBarSize ( dpy, text -> swin );
		CurrentLineToMiddle ( text ); 
	}
	else {
		(void) sprintf ( buf, "%d bytes copied", diff );
		DisplayMessage ( text -> mwin, buf );
	}
	TextCursorOff ( text );
	if ( op == KILLREGION )
		SetTextModif ( text);
}


/*
**	Function name : PasteRegion
**
**	Description : 
**	Input : 
**	Ouput :
*/
void PasteRegion ( text )
	Text *text;
{
	Control_Y ( text, 0 );	
}
