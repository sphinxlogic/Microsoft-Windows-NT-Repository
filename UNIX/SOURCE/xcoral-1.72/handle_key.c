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
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <ctype.h>
#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#include "xcoral.h"
#include "flist.h"

extern EdWin *edwin;
extern Display *dpy;

extern Trans st_initial[], st_control_x[], st_escape[];
static int	f_ascii (), f_nothing (), f_controle (), f_escape (), f_ctr (),
	f_special (), f_ctrx ();

#if ((__osf__) && (__alpha))
Trans st_initial [8] = {
	{ KEY, f_ascii, (long) st_initial },
	{ CONTROL, f_nothing, (long) st_initial },
	{ CONTROL_AND_KEY, f_ctr, (long) st_initial },
	{ CONTROL_AND_X, f_nothing, (long) st_control_x },
	{ ESCAPE, f_escape, (long) st_escape },
	{ ARROW, f_ascii, (long) st_initial },
	{ SPECIAL, f_special, (long) st_initial },
	{ 0, 0, 0 }
};
Trans st_control_x [8] = {
	{ KEY, f_ctrx, (long) st_initial },
	{ CONTROL, f_nothing, (long) st_control_x },
	{ CONTROL_AND_KEY, f_ctrx, (long) st_initial },
	{ CONTROL_AND_X, f_ctrx, (long) st_initial },
	{ ESCAPE, f_escape, (long) st_escape },
	{ ARROW, f_ascii, (long) st_initial },
	{ SPECIAL, f_special, (long) st_control_x },
	{ 0, 0, 0 }
};

Trans st_escape [8] = {
	{ KEY, f_escape, (long) st_initial },
	{ CONTROL, f_nothing, (long) st_initial },
	{ ESCAPE, f_escape, (long) st_initial },
	{ ARROW, f_ascii, (long) st_escape },
	{ SPECIAL, f_special, (long) st_escape },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 }
};
#else
Trans st_initial [8] = {
	{ KEY, f_ascii, (int) st_initial },
	{ CONTROL, f_nothing, (int) st_initial },
	{ CONTROL_AND_KEY, f_ctr, (int) st_initial },
	{ CONTROL_AND_X, f_nothing, (int) st_control_x },
	{ ESCAPE, f_escape, (int) st_escape },
	{ ARROW, f_ascii, (int) st_initial },
	{ SPECIAL, f_special, (int) st_initial },
	{ 0, 0, 0 }
};

Trans st_control_x [8] = {
	{ KEY, f_ctrx, (int) st_initial },
	{ CONTROL, f_nothing, (int) st_control_x },
	{ CONTROL_AND_KEY, f_ctrx, (int) st_initial },
	{ CONTROL_AND_X, f_ctrx, (int) st_initial },
	{ ESCAPE, f_escape, (int) st_escape },
	{ ARROW, f_ascii, (int) st_initial },
	{ SPECIAL, f_special, (int) st_control_x },
	{ 0, 0, 0 }
};

Trans st_escape [8] = {
	{ KEY, f_escape, (int) st_initial },
	{ CONTROL, f_nothing, (int) st_initial },
	{ ESCAPE, f_escape, (int) st_initial },
	{ ARROW, f_ascii, (int) st_escape },
	{ SPECIAL, f_special, (int) st_escape },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 }
};
#endif
static int 	num;		/* Pour les repetitions		*/
static char 	ns [32];		
static int 	last_stat;	/* Pour les sequences escapes	*/
static deletewindow = False;

static KeySym	ksym;
XComposeStatus	compose;
static int 	n_bytes;

static void GetInfosKey (), GetInfosAsciiKey (), GetDigit ();

/*
**	Function name : automate
**
**	Description : Un petit automate, pour s'amuser...
**		Il a trois etats possibles :
**		- initial ( etat par default )
**		- controle_x ( sequence Ctr X ... )
**		- escape ( Avec en plus le "repeat" )
**	Input : 
**	Ouput :
*/
ST *automate ( event, current_st )
	XKeyEvent *event;
	ST *current_st;
{
	char buf [32];
	register int i;
	InfosKey infos;
   	
   	bzero (buf, 32);
	n_bytes = XLookupString ( event, buf, 32, &ksym, &compose );
#ifdef DEBUG
	printf ( "n_bytes = %d %X\n", n_bytes, ksym );
#endif
	(void) GetInfosKey ( ksym, buf [0], &infos );

	deletewindow = False;

	for ( i=0; current_st -> trans [i].type != 0; i++ ){
		if ( current_st -> trans [i].type == infos.type ) {
			if ( current_st -> trans [i].fnt == 0 || current_st -> trans [i].dest_stat == 0 ) {
				(void) fprintf ( stderr, "Error trans = %d\n", i );
         				if ( deletewindow == False )
					return ( (ST *) current_st );
         				else
         					return ( (ST *) - 1 );	
			}
			current_st -> trans [i].fnt ( &infos );
      			if ( deletewindow == False )
				return ( ( ST * ) current_st -> trans [i].dest_stat );
      			else
      				return ( (ST *) - 1 );	
		}
	}
	(void) fprintf ( stderr, "Automate error\n" );
	return ( (ST *) st_initial );
}


/*
**	Function name : GetInfosKey
**
**	Description : GetInfosKey remplie la structure InfosKey
**		en fonction du symbol relatif a l'evenement
**		touche enfoncee.
**		On cherche a savoir dans un premier temps si la
**		touche est de type controle, escape, ou fleche.
**	Input : Le symbole, le caractere, les infos.
**	Ouput :
*/
static void GetInfosKey ( ksym, c, infos )
	KeySym		ksym;
	char 		c;
	InfosKey 	*infos;
{
	switch ( ksym ) {
		case XK_Control_L :
		case XK_Control_R :
			infos -> type = CONTROL;
			infos -> ch = 0;
			ClearMessageWindow ( edwin -> mwin );
			break;
		case XK_Escape 	:
		case XK_Meta_L    :
		case XK_Meta_R    :
			infos -> type = ESCAPE;
			infos -> ch = c;
			ClearMessageWindow ( edwin -> mwin );
			break;
		case XK_Right :
		        infos -> ch = 'f';
			infos -> type = ARROW;
			break;
		case XK_Left 	:
			infos -> ch = 'b';
			infos -> type = ARROW;
			break;
		case XK_Up	:
		        infos -> ch = 'p';
			infos -> type = ARROW;
		        break;
		case XK_Down	:
			infos -> ch = 'n';
			infos -> type = ARROW;
			break;
		default:
			(void) GetInfosAsciiKey ( c, infos );
			break;
	}
}

/*
**	Function name : GetInfosAsciiKey
**
**	Description : GetInfosAsciiKey remplie la structure InfosKey
**		dans les cas suivants:
**		- Sequence Controle X
**		- Sequence Controle + Key
**		- Return, tab, delete, backspace space
**		- Caractere imprimable.
**	Input : Le symbole, le caractere et les infos.
**	Ouput :
*/
static void GetInfosAsciiKey ( c, infos )
	char 		c;
	InfosKey	*infos;
{

	if ( c >= 0 && c <= CtrZ && n_bytes != 0 ) {
		switch ( c ) {
			case CtrX	:
				infos -> type = CONTROL_AND_X;
				break;
			case RETURN 	:
			case LINEFEED	:
			case TAB 	:
			case BACKSPACE	:
			        infos -> type = KEY;
				break;
			default:
				infos -> type = CONTROL_AND_KEY;
				break;
		}
		ClearMessageWindow ( edwin -> mwin );
		infos -> ch = c;
		return;
	}
	else if ( last_stat == REPEAT ) {
		if ( isdigit ( c ))
			infos -> type = ESCAPE;
		else {
			last_stat = 0;
			infos -> type = KEY;
			ClearMessageWindow ( edwin -> mwin );
		}
		infos -> ch = c;
	}
	else if ( n_bytes == 1 ) {
	        infos -> ch = c;
		infos -> type = KEY;
	}
	else {
		infos -> type = SPECIAL; infos -> ch = 0;
	}
}


/*
**	Function name : f_ascii
**
**	Description : Traitement des caracteres ascii
**	Input : Le infos.
**	Ouput :
*/
static f_ascii ( infos )
	InfosKey *infos;	
{
	if ( infos -> type == ARROW ) {
	       TextCursorOff ( edwin -> text );
	       switch ( infos -> ch ) {
	       case 'n':
		 DownCursor ( edwin -> text );
		 break;
	       case 'p':
		 UpCursor ( edwin -> text );
		 break;
	       case 'f':
		 (void) ForwardChar ( edwin -> text );
		 break;
	       case 'b':
		 BackwardChar ( edwin -> text );
		 break;
	       }
	       TextCursorOn ( edwin -> text );
	       return;
	}
	
	switch ( infos -> ch ) {
		case TAB :
		       f_tab ( edwin -> text );
		       break;
		case DELETE:
		case BACKSPACE:
		       f_delete ( edwin -> text );
		       break;
		case LINEFEED:
		case RETURN:
			f_return ( edwin -> text );
		       break;
                default :
		       if ( isprint ( infos -> ch ) )
		              f_impc ( edwin -> text, infos -> ch );
	}
}


/*
**	Function name : f_ctrx
**
**	Description : Traitement des sequences ^X ^key
**	Input : Les infos. 
**	Ouput :
*/
static int f_ctrx ( infos )
	InfosKey *infos;
{
	register int n;
   	register char *str;
	char c = '\007';

	switch ( infos -> ch ) {
		case CtrC :
      			if ( DeleteWindow ( edwin -> text ) == 0 )
				deletewindow = True;	
			break;
		case CtrS :
			KbdSaveFile ( edwin -> text );
			break;
		case CtrF :
			KbdReadFile ( edwin -> text );
			break;
		case CtrW :
			KbdWriteFile ( edwin -> text );
			break;
		case CtrX:
			ExchangePointMark ( edwin -> text );
			break;
		case 'b':
			DisplayOpenFiles ( edwin -> text );
			break;
		case 'k':
			TextCursorOff ( edwin -> text );
			KillCurrentBuffer ( edwin -> text, F_KEY );
			TextCursorOn ( edwin -> text );
			break;
		case 'l':
			TextCursorOff ( edwin -> text );
      			str = (char *) GetString ( edwin -> text, "Goto Line : ", (char *) 0 );
			if ( (str == 0) || (strncmp (str, &c, 1 ) == 0) || ((n = atoi ( str )) == 0) )
				DisplayMessage ( edwin -> text -> mwin, "Abort" );
      			else {
				GotoLineNumber ( edwin -> text, n );
				CurrentLineToMiddle ( edwin -> text ); 
			}
			TextCursorOn ( edwin -> text );
			num = 0;
			break;
		case 'i':
			KbdInsertFile ( edwin -> text );
			break;

		case '2':
			NewWindow ( edwin -> text );
			break;
		default :
			break;
	}
}


/*
**	Function name : f_ctr
**
**	Description : Traitement des sequences ^key
**	Input : Les infos.
**	Ouput :
*/
static int f_ctr ( infos )
	InfosKey *infos;
{
	TextCursorOff ( edwin -> text );	

	switch ( infos -> ch ) {
		case CtrA:
			MoveToBline ( edwin ->text );
			break;
		case CtrB:
			BackwardChar ( edwin -> text );
			break;
		case CtrD :
			Control_D ( edwin -> text );
			break;
		case CtrE:
			MoveToEline ( edwin -> text );
			break;
		case CtrF:
			(void) ForwardChar ( edwin -> text );
			break;
		case CtrG:
			DisplayMessage ( edwin -> mwin, "Abort" );
			num = 0;
			if ( last_stat == REPEAT )
				last_stat = 0;
			ResetSearchString ();
			break;
		case CtrK:
			if ( num == 0 ) num++;
			Control_K ( edwin -> text, num );
			num = 0;
			break;
		case CtrL:
			ClipOn ( edwin -> text, 0 );
			RefreshPage ( edwin -> text );
			ClipOff ( edwin -> text );
			break;
		case CtrN:
			DownCursor ( edwin -> text );
			break;
		case CtrO:
			f_return ( edwin -> text );
			TextCursorOff ( edwin -> text );			
			BackwardChar ( edwin -> text );
			break;
		case CtrP:
			UpCursor (  edwin -> text );
			break;
		case CtrR:
			BackwardSearch ( edwin -> text );
			break;
		case CtrS:
			ForwardSearch ( edwin -> text );
			break;
		case CtrV:
			NextPage ( edwin -> text );
			break;
		case CtrW:
			KillRegion ( edwin -> text );
			break;
	                case CtrY:
			if ( num == 0 )
			       Control_Y ( edwin -> text, 0 );
			else 
			       Control_Y ( edwin -> text, num-1 );
			num = 0;
			break;
		case Ctr_sp:
			SetMark ( edwin -> text );
			break;
		default :
			;
	}
	TextCursorOn ( edwin -> text );
}


/*
**	Function name : f_escape
**
**	Description : Traitement des sequences escape.
**	Input : 
**	Ouput :
*/
static int f_escape ( infos )
	InfosKey	*infos;
{
	TextCursorOff ( edwin -> text );	

	switch ( infos -> ch ) {
		case '<':
			FirstPage ( edwin -> text );
			break;
		case '>' :
			GotoEnd ( edwin -> text );
			CurrentLineToMiddle ( edwin -> text );
			break;
		case 'v' :
			PreviousPage ( edwin -> text );
			break;
		case 'r':
			GlobalReplace ( edwin -> text );
			break;
		case 'q':
			QueryReplace ( edwin -> text );
			break;
		case 'w':
			CopyRegion ( edwin -> text );
			break;
		default  :
			GetDigit ( infos );
			break;
	}

	TextCursorOn ( edwin -> text );
}

/*
**	Function name : GetDigit
**
**	Description : Traitement la sequence escape num
**	Input : Les infos.
**	Ouput :
*/
static void GetDigit ( infos )
	InfosKey *infos;
{
	char str [32];
	char tmp[2];

	if ( isdigit ( infos -> ch )) {
		tmp [0] = infos -> ch; tmp [1] = 0;
		(void) strcat  ( ns, tmp );

		if ( atoi(ns) > MAXREPEAT ) {
			DisplayMessage ( edwin -> mwin, "Too much...bye" );
			last_stat = 0;
			ns [0] = num = 0;
			return;
		}

		if ( last_stat == REPEAT )
			(void) sprintf ( str,"Repeat : %d%c", num, infos -> ch );
		else {
			(void) sprintf ( str,"Repeat : %c", infos -> ch );
			last_stat = REPEAT;
		}
		DisplayMessage ( edwin -> mwin,  str );
		num = atoi (ns);	
	}
	else {
		last_stat = 0;
		ns [0] = num = 0;
		if ( isprint ( infos -> ch ) && infos -> ch != ESCAPE ) {
			f_impc ( edwin -> text, infos -> ch );
			ClearMessageWindow ( edwin -> mwin);
		}
	}
}

static int f_nothing (){}
static int f_special ( infos, edwin )
	InfosKey *infos;
	EdWin	*edwin;
{
#ifdef lint
	(void) fprintf ( stderr, "dpy = %d infos = %d edwin = %d\n",
		(int) dpy, (int) infos, (int) edwin );
#endif
}
