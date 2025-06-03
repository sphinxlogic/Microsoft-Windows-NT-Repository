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
#include <string.h>
#ifndef apollo
#include <malloc.h>
#endif
#include <X11/Xlib.h>

#include "text.h"
#include "flist.h"

static char *str_save = 0;
static char *str_old = 0;
static char *str_new = 0;
static int abort_debug = False;

#define FORWARD	1
#define BACKWARD	-1
#define GLOBAL	1
#define QUERY	2
#define S_MENU	0
#define S_KBD	1

extern Display *dpy;
static void HandleForward (), HandleBackward (), HandleGlobal ();
static int SetReplaceString (), BmSearch ();
static char *SetString();


/*
**	Function name : ForwardSearch
**
**	Description : Prepare la recherche vers l'avant.
**
**	Input : Le text courant.
**	Ouput :
*/
void ForwardSearch ( text )
	Text *text;
{
	register char *str;

    	if ( (str = (char *) SetString ( text, "Forward search : ", S_KBD )) == 0 )  {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
	else
		DisplayMessage ( text -> mwin, "Search..." );
	
    	HandleForward ( str, text );
}


/*
**	Function name : HandleForward
**
**	Description : Recherche vers l'avant la chaine 'str'
**
**	Input : La chaine recherchee, le text courant.
**	Ouput :
*/
static void HandleForward ( str, text )	
register char *str;
Text *text;
{	
	register int newpos;
	register char *start,  *end;

	start = (char *) RightBuf ( text -> buf ); 
	end = (char *) BottomBuf ( text -> buf );

	if ( (newpos=BmSearch( start, str,  end -start, FORWARD )) != -1 ) {
		if ( newpos == 0 ) { 	/* On est pile dessus */
			(void) MoveHole ( text -> buf, 1 );	/* Voyons plus loin */
			start ++;
			if ( (newpos=BmSearch( start, str,  end - start, FORWARD )) == -1 ) {
				DisplayMessage ( text -> mwin, "Search failed" );
				klaxon ();
				(void) MoveHole ( text -> buf, -1 );
				return;
			}
		}
		GotoPos ( text, start, newpos, FORWARD );
		return;
	}
	DisplayMessage ( text -> mwin, "Search failed" );
	klaxon ();
}


/*
**	Function name : BackwardSearch
**
**	Description : Prepare la recherche vers l'arriere.
**
**	Input : Le text.
**	Ouput :
*/
void BackwardSearch ( text )
	Text *text;
{
	register char *str;

	if ( (str = (char *) SetString ( text, "Backward search : ", S_KBD )) == 0 ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
	else
		DisplayMessage ( text -> mwin, "Search..." );	

	HandleBackward ( str, text );
}


/*
**	Function name : HandleBackward
**
**	Description : Recherche vers l'arriere la chaine 'str'
**
**	Input :  La chaine recherchee, le text courant.
**	Ouput :
*/
static void HandleBackward ( str, text )
register char *str;
Text *text;
{
	register int newpos, oldpos;
	register char *start,  *end;
	int len;

	oldpos = GetNcFromRight ( text -> buf );
	start = (char *) GetCurrentLine ( text -> buf, &len );
	end = (char *) TopBuf ( text -> buf );

	start += ( len -1);	/* On part de la fin de la ligne */
	if ( (newpos=BmSearch( start, str, len -1, BACKWARD )) != -1 ) {
		if ( (newpos - oldpos) > 0 ) { /* On est a l'interieur */
			(void) MoveHole ( text -> buf, -(newpos - oldpos) );
			return;
		}
	}
	start = (char *) LeftBuf ( text -> buf ); /* Allons voir plus loin */

	if ( (newpos=BmSearch ( start, str,  start - end, BACKWARD )) == -1 ) {
		DisplayMessage ( text -> mwin, "Search failed" );
		klaxon ();
		return;
	}	
	GotoPos ( text, start, newpos, BACKWARD );
	return;
}


/*
**	Function name : GlobalReplace
**
**	Description : Prepare une substitution globale.
**
**	Input : Le text courant.
**	Ouput :
*/
void GlobalReplace ( text )
	Text *text;
{
	if ( SetReplaceString ( text, GLOBAL, S_KBD ) == -1 )
		return;
	HandleGlobal ( text );
}


/*
**	Function name : HandleGlobal
**
**	Description : Substitution globale.
**
**	Input : Le text courant.
**	Ouput :
*/
static void HandleGlobal ( text )
	Text *text;
{
	register int line, newpos, modif;
	register char *start,  *end;

	line = text -> no_current_line;
/*	HoleToLeft ( text -> buf ); */
	start = (char *) RightBuf ( text -> buf );
	end = (char *) BottomBuf ( text -> buf );

	while ( (newpos=BmSearch( start, str_old,  end - start, FORWARD )) != -1 ) {
		(void) MoveHole ( text -> buf, newpos + strlen (str_old)); /* On se place a la fin du mot */
		DeleteNchar ( text -> buf, strlen ( str_old) );
		InsertNchar ( text -> buf, str_new, strlen ( str_new));
		modif = True;
		start = (char *) RightBuf ( text -> buf ); /* Position courante */
	}
   	HoleToLeft ( text -> buf );
	text -> no_current_line = 1;
	GotoLineNumber ( text, line );
	SetAndDisplayPage ( text );
	if ( modif == True )
		SetTextModif ( text );
}


/*
**	Function name : QueryReplace
**
**	Description : Substitution sur demande.
**
**	Input : Le text courant.
**	Ouput :
*/
void QueryReplace ( text )
	Text *text;
{
	register int newpos;
	register char *start, *end;
	register char *reply;
	register int abort, modif;
	char c = '\007'; /* ^G */

	if ( SetReplaceString ( text, QUERY, S_KBD ) == -1 ) return;

	start = (char *) RightBuf ( text -> buf ); 
	end = (char *) BottomBuf ( text -> buf );
	modif = abort = False;

	while ( (newpos=BmSearch( start, str_old,  end -start, FORWARD )) != -1 ) {
		GotoPos ( text, start, newpos, FORWARD ); 
		SetAndDisplayPage ( text );
		TextCursorOn ( text );
		(void) MoveHole ( text -> buf, strlen (str_old)); /* On se place a la fin du mot */
		reply = (char * ) GetString ( text, "Replace [y,n,q] : ", " " );

		if ( (reply == 0) || (strncmp(reply, &c, 1)) == 0 || (strncmp(reply, "q", 1) == 0 )) {
			abort = True;
			break;
		}
		if ( strncmp ( reply, "y", 1 ) == 0 ) {
			DeleteNchar ( text -> buf, strlen ( str_old) );
			InsertNchar ( text -> buf, str_new, strlen ( str_new));
			modif = True;
		}
		start = (char *) RightBuf ( text -> buf ); 
		end = (char *) BottomBuf ( text -> buf );
	}

	if ( abort == True )
		DisplayMessage ( text -> mwin, "Abort" );
	else
		DisplayMessage ( text -> mwin, "Done" );
	
	/* On revient au debut du mot */

	SetAndDisplayPage ( text );
	if ( modif == True )
		SetTextModif ( text );
}


/*
**	Function name : MenuForwardSearch
**
**	Description : Prepare la recherche en avant,
**		 ( a partir du menu ).
**
**	Input : Le text courant.
**	Ouput :
*/
void MenuForwardSearch ( text )
Text *text;
{
	register char *str;
/*
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent );
		return;
	}
*/
    	(void) ResetSearchString ();
	if ( (str = (char *) SetString ( text, "Forward search : ", S_MENU )) == 0 )  {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
    	(void) HandleForward ( str, text );
	(void) ResetSearchString ();
}


/*
**	Function name : MenuBackwardSearch
**
**	Description : Prepare la recherche en arriere,
**		( a partir du Menu ).
**
**	Input : Le text.
**	Ouput :
*/
void MenuBackwardSearch ( text )
Text *text;
{
	register char *str;

/*
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent );
		return;
	}
*/
	(void) ResetSearchString ();
	if ( (str = (char *) SetString ( text, "Backward search : ", S_MENU )) == 0 )  {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
	HandleBackward ( str, text );
	(void) ResetSearchString ();
}


/*
**	Function name : MenuQueryReplace
**
**	Description : Substitution a la demande,
**		 ( a partir du menu ).
**
**	Input : Le text.
**	Ouput :
*/
void MenuQueryReplace ( text )
Text *text;
{
/*	
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent );
		return;
	}
*/
	(void)  GetStringFromDialogBox ( text -> window,
		"Warning : Please use esc q" );
}


/*
**	Function name : MenuGlobalReplace
**
**	Description : Substitution globale,
**		( a partir du menu ).
**
**	Input : Le text courant.
**	Ouput :
*/
void MenuGlobalReplace ( text )
Text *text;
{
/*	
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent );
		return;
	}
*/
	if ( SetReplaceString ( text, GLOBAL, S_MENU ) == -1 )
		return;
	HandleGlobal ( text );
}


/*
**	Function name : BmSearch
**
**	Description : La recherche avec le fameux
**		algo Boyer-Moore.
**
**	Input : voir plus loin.
**	Ouput : La position relative du pattern recherche,
**		sinon -1.
*/
static int BmSearch ( buf, str, buf_len, dir )
	register char *buf;	/* Le buffer */
	register char *str;	/* La string */
	register int buf_len;	/* Longueur du buffer */
	register int dir;	/* Forward (1), Backward (-1)*/
{
	register int s_len = strlen ( str );
	register int len = s_len -1;
	register int i, j, k, t;
	int index [128];
	
	for ( i = 0 ; i < 128 ; i++ )  index [i] = s_len;	

	for ( i = 0; i < s_len; i++ ) {
		k = ( dir == FORWARD ) ? len - i : i;
		if ( index [(int) str [k]] == s_len ) 
			index [(int) str [k]] = i;
	}

#ifdef DEBUG
	for ( i = 0 ; i < 128 ; i++ ) 
		if ( index[i] != s_len )
			fprintf ( stderr, "index %c %d = %d\n", i, index [i],  (char) i);
#endif
	for ( i = ( dir == FORWARD) ? len : 0, 
		j = ( dir == FORWARD) ? len : 0;
		(( dir == FORWARD) ? ( j>= 0) : (j <= len));
		i -= ( dir == FORWARD) ? FORWARD : BACKWARD,
		j -= ( dir == FORWARD) ? FORWARD : BACKWARD ) { 
#ifdef DEBUG
	(void) fprintf ( stderr, "i = %d, j = %d\n", i, j );
#endif
		while ( buf [i] != str [j] ) {
			t = index [(int) buf [i] ];
			if ( dir == FORWARD ) {
				i += (( s_len - j ) > t) ? s_len - j : t;
				if ( i >= buf_len +1 ) return -1;
				j = len;
			}
			else {
#ifdef DEBUG 
	(void) fprintf ( stderr, "buf[%d] = %c str[%d] = %c t = %d\n", i, buf[i], j, str[j], t );
#endif
				i -= ( j > t) ? s_len : t; 

				if ( (buf_len +1 +i) < 0 ) return -1; 
				j = 0; 
			}
		}
	}
	return ( dir == FORWARD ) ? (i + 1) : (- ( i - s_len -1 ));
}


/*
**	Function name : SetString
**
**	Description : Positionne le pattern a rechercher.
**
**	Input : Le text courant, le prompt, clavier/menu
**
**	Ouput : Le pattern.
*/
static char *SetString ( text, prompt, from )
	Text *text;
	register char *prompt;
	register int from;
{
	register char *str;
	char c = '\007'; /* ^G */
	register int len;

	if ( (str_save == 0) || (strlen (str_save)) == 0 ) {
		if ( from == S_KBD )
			str = (char *) GetString ( text, prompt, (char *) 0 );
		else
			str = ( char *) GetStringFromDialogBox ( text -> window, prompt );

		if ( str == 0 )
			return 0;
		if ( strncmp(str, &c, 1) == 0 ) {
			abort_debug = True;
			return 0;
		}
		if ( str_save != 0 )
			(void) free ( str_save );
		len = strlen (str);
		str_save = (char *) malloc ( (unsigned) len + 1);
		(void) strncpy ( str_save, str,  len);
		*(str_save + len) = '\0';
	}
	return str_save;
}


/*
**	Function name : SetReplaceString
**
**	Description : Positionne les patterns utilises
**		lors des substitutions.
**
**	Input : Le text courant, Query/Global, menu/clavier.
**	Ouput : Ok 0, sinon -1.
*/
static int SetReplaceString ( text, type, from )
	Text *text;
	register int type;
	register int from;
{
	register char *str;
	char buf [64], tmp[64];
	register int len;

	if ( type == GLOBAL )
		(void) strcpy ( tmp, "Global replace," );
	else
		(void) strcpy ( tmp, "Query replace," );	
	(void) ResetSearchString ();
	(void) strcpy ( buf, tmp );
	(void) strcat ( buf, " Old String : " );
	if ( (str = (char *) SetString ( text, buf, from )) == 0 ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return -1;
	}
	len = strlen (str);
	if ( str_old != 0 )
		(void) free ( str_old );
	str_old = (char * ) malloc ( (unsigned) len + 1);
	(void) strncpy ( str_old, str, len );
	*(str_old + len) = '\0';

	(void) ResetSearchString ();

	buf [0] = '\0';
	(void) strcpy ( buf, tmp );
	(void) strcat ( buf, " New String : " );

	if ( (str = (char *) SetString ( text, buf, from )) == 0 ) {
		if ( abort_debug == True ) {
			DisplayMessage ( text -> mwin, "Abort" );
			abort_debug = False;
			return -1;
		}
		else {
			(void) strcpy ( buf, "" );
			str = buf;
		}
	}

	len = strlen (str);
	if ( str_new != 0 )
		(void) free ( str_new );
	str_new = (char *) malloc ( (unsigned) len + 1 );
	(void) strncpy ( str_new, str, len);
	*(str_new+ len) = '\0';
	(void) ResetSearchString ();
	return 0;
}


/*
**	Function name : GotoPos
**
**	Description : Va a la position relative a
**		partir de la position courante.
**
**	Input : Le text cournant, la position de depart,
**		nb octets, direction.
**
**	Ouput :
*/
void GotoPos ( text, start, newpos, dir )
	Text *text;
	register char *start;
	register int newpos;
	register int dir;
{
	register int line;
	register int x = ( dir == FORWARD ) ? newpos : -newpos;

	line = GetNewLine ( start, x );
	text -> no_current_line += (dir == FORWARD ) ? line : -line;

	(void) MoveHole ( text -> buf, x );	
	if ( line != 0 ) {
		if ( (line > (( dir == FORWARD) ? text -> n2 : text -> n1 )))
			CurrentLineToMiddle ( text );
		else {
			text -> n1 += (dir == FORWARD) ? line : -line;
			text -> n2 -= (dir == FORWARD) ? line : -line;				
			SetAndDisplayPage ( text );
		}
	}
}


/*
**	Function name : DeleteStrings
**
**	Description : 
**	Input : 
**	Ouput :
*/
void DeleteStrings ()
{
   	if ( str_save != 0 )
		(void) free ( str_save );
   	if ( str_old != 0 )   
		(void) free ( str_old );
      	if ( str_new != 0 )
		(void) free ( str_new );
}


/*
**	Function name : ResetSearchString
**
**	Description : 
**	Input : 
**	Ouput :
*/
void ResetSearchString ()
{
	if ( str_save != 0 )
		(void) free ( str_save );
	str_save = 0;
}

