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
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#ifndef apollo
#include <malloc.h>
#endif
#include "text.h"
#include "flist.h"

extern Display *dpy;
static int toggle_del = 0;
static int MatchChar (), MatchChar ();
static void AligneChar (), InsertLines (), Indent (), donothing (),
	SmallTime (), DisplayModel () ;


/*
**	Function name : f_impc
**
**	Description : Imprime un caractere ( position courrante ). 
**	Input : Le caractere.
**	Ouput :
*/
void f_impc ( text, c )
	Text *text;
	char c;
{
	register int newpos;
	register char cc;
	register int old_x, old_y;
       
	TextCursorOff ( text );
	InsertNchar ( text -> buf, &c, 1 );

	if ( (text -> modif == False) && (strcmp(text -> filename,"NoName")==0) ) { 
		text -> modif = True; 
		SetScrollLine ( text -> swin , 1 );
		ShowScrollFrame ( dpy, text -> swin );
	}

	SetCurrentLine ( text );
	switch ( c ) {
		case ')': cc = '(';break;
		case '}': cc = '{';break;
		case ']': cc = '[';break;
		default: cc = '\0';break;
	}
	if ( cc != '\0' ) {
		if ( (newpos = MatchChar ( text , cc, c, text -> n1 )) >= 0 ) {
			TextCursorOn ( text );
			TextCursorOff ( text );
			old_x = text -> x_pos; old_y = text -> y_pos;
			GotoPos ( text, LeftBuf (text -> buf),  newpos, -1 );
			TextCursorOn ( text );
			XSync ( dpy, False );
			SmallTime ( (long) 200000  );
			TextCursorOff ( text );
			(void) MoveToXYinTextWindow ( text, old_x, old_y );
	        		AligneChar ( text, c );         	
		}
	}	
	TextCursorOn ( text );
	SetTextModif ( text );
}


/*
**	Function name : AligneChar
**
**	Description : aligne le caractere '}' en mode C et C++
**	Input : Le text courant, le caractere a aligner.
**	Ouput :
*/
static void AligneChar ( text, c )
	Text *text;
	register char c;
{
	int len;
	register char *s = (char *) GetCurrentLine ( text -> buf, &len );
	register int len_s;

	if ( (text -> mode == TEXT) || (text -> mode) == LATEX || c != '}' )
		return;

	len_s =  --len;	/* On ne tient pas compte du dernier caractere */
	while ( len ) { 	/* On verifie que la ligne est vide */
		if ( (*s != '\t') && (*s != ' ') ) return;
		s ++; 
		len --;
	}
	len = strlen ( text -> indent ); 	/* nb de caractere a enlever */
	if ( len > len_s ) return; 	/* Pour ne pas sortir de la ligne courante */

	BackwardChar ( text );		/* On saute le '}' */
	DeleteNchar ( text -> buf, len );	/* On enleve ce qu'il faut */

	(void) ForwardChar ( text );

	ClipOn ( text, 0 );
	ClearLine ( text, text -> n1 + 1 );	/* On affiche la ligne */
	SetCurrentLine ( text );
	ClipOff ( text );
}


/*
**	Function name : MatchChar
**
**	Description : En mode C, C++, recherche le '{',
**		'(', ou '[' qui match.
**	Input : Le text courant,
**		le caratere courant '}', ')', ou ']',
**		le caractere a matcher '{', '(' ou '[', 
**		le nombre de ligne visible au dessus de
**		la ligne courante.
**	Ouput : La position relative du caractere a matcher.
*/
static int MatchChar ( text, c,ic, n )
	Text *text;
	register char c, ic;
	register int n;
{
	register char *start = (char * ) LeftBuf ( text -> buf );
	register char *end = (char * ) TopBuf ( text -> buf );
	register int line = 1;
	register int i = 2;
	register int count = 0;
	register int audela = False;
	char tmp [32];
	
	start --; /* On saute le premier pour ne pas incrementer count */
	ClearMessageWindow ( text -> mwin );
	while ( start >= end ) {
		if ( *start == c ) {
			if ( count != 0 )
				count--;
			else {
				if ( audela == True ) {
					(void) sprintf ( tmp, "Match '%c' at line %d",  c,
						text -> no_current_line - line );
					DisplayMessage ( text -> mwin, tmp );
					return -1;
				}
				else
					return i;
			}
		}
		if ( n == 0 ) {
			/* On est sur la premiere ligne de la page courante */
			if ( line > 1 ) audela = True;
		}
		else 
			if ( line > n ) audela = True;

		if ( *start == '\n' ) line++;
		if ( *start == ic ) count++;
		start --;
		i ++;
	}
	DisplayMessage ( text -> mwin, "No match" );
	return -1;
}


/*
**	Function name : f_tab
**
**	Description : Tabulation.
**	Input : Le text courant. 
**	Ouput :
*/
void f_tab ( text )
	Text *text;
{
	register int ligne = text -> n1 + 1;

	TextCursorOff ( text );
	InsertNchar ( text -> buf, "\t", 1 );

	if ( (text -> modif == False) && (strcmp(text -> filename,"NoName")==0) ) { 
      		text -> modif = True;
		SetScrollLine ( text -> swin , 1 );
		ShowScrollFrame ( dpy, text -> swin );
	}

	ClipOn ( text, 0 );
	ClearLine ( text, ligne );
	SetCurrentLine ( text );
	ClipOff ( text );
	TextCursorOn ( text );
   	SetTextModif ( text );
}


/*
**	Function name : f_return
**
**	Description : Retour chariot.
**	Input : Le text courant.
**	Ouput :
*/
void f_return ( text )
	Text *text;
{
	TextCursorOff ( text );

	if ( (text -> modif == False) && (strcmp(text -> filename,"NoName")==0) ) { 
		text -> modif = True;
		SetScrollLine ( text -> swin , 1 );
		ShowScrollFrame ( dpy, text -> swin );
	}

	if ( text -> n2 == 0 ) 
		CurrentLineToMiddle ( text );

	InsertNchar ( text -> buf, "\n", 1 );
	
	text -> n1 ++;
	text -> n2 --;
	text ->  no_current_line ++;
	text -> lines_in_buf ++;

	if ( (text -> mode == CPLUS ) || ( text -> mode == STD_C )) {
		Indent ( text );
	}

	SetLinesTable ( text );
	ClipOn ( text, text -> n1 -1);
	RefreshPage ( text );
	ClipOff ( text );

	SetScrollLine ( text -> swin , text -> lines_in_buf );
	ShowScrollFrame ( dpy, text -> swin );
	(void) MoveScrollBar ( dpy, text -> swin, 
		CURRENT, text -> no_current_line - text -> n1 - 1 );

	TextCursorOn ( text );
	SetTextModif ( text );
}


/*
**	Function name : Indent
**
**	Description : Very trivial auto indent.
**	Input : Le text courant.
**	Ouput :
*/
static void Indent ( text )
	Text *text;
{
	register char *start = (char * ) LeftBuf ( text -> buf );
	register char *end = (char * ) TopBuf ( text -> buf );
	register int indent = 0;

	start --; /* On saute le dernier return */
	while ( start >= end ) {
		if ( (*start == '\n') && (*(start+1) != ' ') && (*(start+1) != '\t') &&(*(start+1) != '\n') ) {
			break;
		}
		if ( (*start == '(') || (*start == '{') || (*start == '[') ) indent++;
		if ( (*start == ')') || (*start == '}') || (*start == ']') ) indent--;
		start --;
	}
	if ( indent < 0 ) indent = 0;
	while ( indent > 0 ) {
		InsertNchar ( text -> buf, text -> indent, strlen ( text -> indent) );		
		indent--;
	}
}


/*
**	Function name : f_delete
**
**	Description : Delete ou backspace.
**	Input : Le text courant.
**	Ouput :
*/
void f_delete ( text)
	Text *text;
{
	char c;

	if ( TextInBuf ( text ) == False ) 
		return;
	TextCursorOff ( text );
	GetPrevChar ( text -> buf, &c );

	if ( c == '\n' ) {
		if ( text -> n1 == 0 ) 
			CurrentLineToMiddle ( text );
		DeleteNchar ( text -> buf, 1 );
		text -> n1 --;
		text -> n2 ++;
		text -> no_current_line--;
		text -> lines_in_buf --;
		SetLinesTable ( text );
		ClipOn ( text, text -> n1 );
		RefreshPage ( text );
		ClipOff ( text );
		SetScrollLine ( text -> swin , text -> lines_in_buf );
		ShowScrollFrame ( dpy, text -> swin );
		if ( text -> no_current_line == 1 ) {
			(void) MoveScrollBar ( dpy, text -> swin, FIRST, 0 );
		}
		else
			(void) MoveScrollBar ( dpy, text -> swin, 
				CURRENT, text -> no_current_line - text -> n1 - 1 );
	}
	else {
		DeleteNchar ( text -> buf, 1 );
		ClipOn ( text, 0 );
		ClearLine ( text, text -> n1 + 1 );
		SetCurrentLine ( text );
		ClipOff ( text );
       }
	TextCursorOn ( text );
	SetTextModif ( text );	
}


/*
**	Function name : Controle_D
**
**	Description : Efface le caractere courant.
**	Input : Le text courant.
**	Ouput :
*/
void Control_D ( text )
	Text *text;
{
	if ( TextInBuf ( text ) == False ) 
		return;
	if ( ForwardChar ( text ) == True ) {
		f_delete ( text );
		SetTextModif ( text );
	}
}


/*
**	Function name : Control_K
**
**	Description : Delete une ou plusieurs lignes
**	Input : Le text courant.
**	Ouput : Le nb de lignes.
*/
void Control_K ( text, n )
	Text *text;
	register int n;
{
       register char *p;
       int len, dn;
       if ( TextInBuf ( text ) == False ) 
	      return;

       p = (char *) DeleteLines ( text -> buf, n, &len, &dn );
       
       StoreInKillBuf ( p, len, dn );
#ifdef DEBUG
       fprintf ( stderr, "n = %d Delete lines = %d len = %d\n", n, dn, len );
#endif
       SetLinesTable ( text );

       ClipOn ( text, text -> n1 );
       RefreshPage ( text );
       ClipOff ( text );

       if ( dn > 0 ) {
	      text -> lines_in_buf = GetNumberOfLineInBuf ( text -> buf );
	      SetScrollLine ( text -> swin , text -> lines_in_buf );
	      ShowScrollFrame ( dpy, text -> swin );
	      (void) MoveScrollBar ( dpy, text -> swin, 
	             CURRENT, text -> no_current_line - text -> n1 - 1 );
       }
	SetTextModif ( text );
}


/*
**	Function name : Control_Y
**
**	Description : Restore un portion de texte.
**	Input : Le text courant, le no a restorer.
**	Ouput :
*/
void Control_Y ( text, i )
	Text *text;
	register int i;
{
	register char *p;
	int len, dn;

	TextCursorOff ( text );
	p = (char *) RestoreKillBuf ( i, &len, &dn );
	if ( p == 0 ) {
		TextCursorOn ( text );
		return;
	}
#ifdef DEBUG
       fprintf ( stderr, "i = %d len = %d Restore lines = %d\n", i, len, dn );
       write ( 1, p, len );
#endif
	ClipOn ( text, 0 );
	SetTextModif ( text );
	if ( dn == 1 ) {
		if ( len == 1 ) {  /* ligne vide */
			f_return ( text );
		}
		else {
			InsertNchar ( text -> buf, p, len );      
			ClearLine ( text, text -> n1 + 1 );
			SetCurrentLine ( text );
		}
	}
	else 
		InsertLines ( text, p, len, dn );

	ClipOff ( text );
	TextCursorOn ( text );
}


/*
**	Function name : DisplayKillBuffer
**
**	Description : Affiche la fenetre des choses tuees.
**		et restore eventuellement.
**	Input : Le text courant.
**	Ouput :
*/
void DisplayKillBuffer ( text ) 
Text *text;
{
	register char *str;
  
   	if ( GetVisibility ( dpy, text ) == False )
		return;

	str = (char *) GetStringFromSelect ( text -> window, KILLBUF );
	if ( str != 0 ) {
	         	/* str += 9; */
		/* On vire le "Select : " */
		 /* On ne prend que les 2 premier caracteres */
		*(str+2) = 0;
		/* On restore la nieme entree */
		Control_Y ( text, atoi(str) - 1 );
      		if ( str != 0 )
	      		(void) free ( str ); 
          	}
	else
		DisplayMessage ( text -> mwin, "Abort" );
}


/*
**	Function name : GetBytesFromCutBuffer
**
**	Description : Comme son nom l'indique.
**	Input : Le text courant.
**	Ouput :
*/
void GetBytesFromCutBuffer ( text )
	Text *text;
{
	register int lines;
	register char *s;
	int nbytes;

	s = XFetchBuffer ( dpy, &nbytes, 0 );

	if ( (s != 0) && (nbytes != 0 )) {
		TextCursorOff ( text );
		ClipOn ( text, 0 );
		lines = GetNewLine ( s, nbytes);
		SetTextModif ( text );
		if ( lines == 0 ) {
			InsertNchar ( text -> buf, s, nbytes);
			ClearLine ( text, text -> n1 + 1 );
			SetCurrentLine ( text );
		}
		else {
			InsertLines ( text, s, nbytes, lines );
			RefreshScrollBar ( dpy, text -> swin );
		}
		TextCursorOn ( text );
		ClipOff ( text );
	}
}


/*
**	Function name : InsertLines
**
**	Description : Insere une ou plusieurs lignes.
**	Input : Le text courant, la chaine, sa longueur et
**		le nombre de lignes.
**	Ouput :
*/
static void InsertLines ( text, s, nbytes, lines )
	Text *text;
	register char *s;
	register int nbytes;
	register int lines;
{
	InsertNchar ( text -> buf, s, nbytes );      
	text -> no_current_line += lines;
	text -> lines_in_buf = GetNumberOfLineInBuf ( text -> buf );
	SetScrollLine ( text -> swin , text -> lines_in_buf );
	ShowScrollFrame ( dpy, text -> swin );
     
	if ( text -> n2 < lines ) {
		CurrentLineToMiddle ( text );
	}
	else {
		text -> n1 += lines;
		text -> n2 -= lines;
		SetLinesTable ( text );
		RefreshPage ( text );
		(void) MoveScrollBar ( dpy, text -> swin, 
			CURRENT, text -> no_current_line - text -> n1 - 1 );
	}
}


/*
**	Function name : StoreBytesInCutBuffer
**
**	Description : Selection.
**	Input : Le text, la position du cursor.
**	Ouput :
*/
void StoreBytesInCutBuffer ( text, x, y )
	Text *text;
	register int x, y;
{
	register old_x = text -> x_pos;
	register old_y = text -> y_pos;
	register char *old_left = (char *) LeftBuf ( text -> buf );
	register char *old_right = (char *) RightBuf ( text -> buf );
	register char *new_left, *new_right;

	(void) MoveToXYinTextWindow ( text, x, y );
	TextCursorOn ( text );
	XSync ( dpy, False );
	
	new_left = (char *) LeftBuf ( text -> buf );
	new_right = (char *) RightBuf ( text -> buf );

	if ( new_left > old_left )  { /* Deplacement vers la droite du curseur */
		XStoreBytes ( dpy, old_left +1, new_left - old_left );
	}
	else {
		XStoreBytes ( dpy, new_right, old_right - new_right );
	}
	toggle_del = False;
	SmallTime ( (long) 200000 );	/* 0.20 secondes */
	TextCursorOff ( text );
	(void) MoveToXYinTextWindow ( text, old_x, old_y );
}


/*
**	Function name : DeleteBytesFromCutBuffer
**
**	Description : Efface une slection.
**	Input : Le text courant.
**	Ouput :
*/
void DeleteBytesFromCutBuffer ( text )
	Text *text;
{
	register int lines;
	register char *s;
	int nbytes;

	s = XFetchBuffer ( dpy, &nbytes, 0 );

	if ( (s != 0) && (nbytes != 0 ) && (toggle_del == False) ) {
		if ( (text -> modif == False) && (strcmp(text -> filename,"NoName")==0) ) {
			return;
		}
		TextCursorOff ( text );
		ClipOn ( text, 0 );
		lines = GetNewLine ( s, nbytes);
		(void) MoveHole ( text -> buf, nbytes );
		DeleteNchar ( text -> buf, nbytes);

		SetTextModif ( text );

		if ( lines == 0 ) {
			ClearLine ( text, text -> n1 + 1 );
			SetCurrentLine ( text );
		}
		else {
			text -> lines_in_buf = GetNumberOfLineInBuf ( text -> buf );
			SetScrollLine ( text -> swin , text -> lines_in_buf );
			ShowScrollFrame ( dpy, text -> swin );
			SetLinesTable ( text );
			RefreshPage ( text );
			(void) MoveScrollBar ( dpy, text -> swin, 
				CURRENT, text -> no_current_line - text -> n1 - 1 );
			RefreshScrollBar ( dpy, text -> swin );
		}
		TextCursorOn ( text );
		ClipOff ( text );
		toggle_del = True;
	}
}

static void donothing () {}


/*
**	Function name : SmallTime
**
**	Description : Un reveil
**	Input : Le temps.
**	Ouput :
*/
static void SmallTime ( t )
	register long t; /* micros-secondes */
{
	struct itimerval value, ovalue;
	value.it_interval.tv_sec = 0;

	value.it_interval.tv_usec = t;

	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = t;
	
	(void) signal ( SIGALRM, donothing );
	(void) setitimer ( ITIMER_REAL, &value, &ovalue );
	(void) pause ();
	(void) signal ( SIGALRM, SIG_IGN );
}


/*
**	Function name : MakeClass
**
**	Description : Fabrique un squelette de classe.
**	Input : Le text courant.
**	Ouput :
*/
void MakeClass ( text )
	Text *text;
{
	register char *class_name, *parent_name, *str, *buf;
   	register int len;
	char c = '\007';
/*
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent ); 
		return;
	}
*/
   	str = ( char *)  GetStringFromDialogBox ( text -> window, "Class name : " );

	if ( (str == 0) || (strncmp(str, &c, 1) == 0) ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}

   	class_name = ( char * ) malloc ( (unsigned int) strlen (str) + 1);
   	(void) strcpy ( class_name, str );
   	str = ( char *)  GetStringFromDialogBox ( text -> window, "Parent class name : " );

   	if ( str != 0 ) {
	   	if ( strncmp(str, &c, 1) == 0 ) {
			DisplayMessage ( text -> mwin, "Abort" );
			return;
		}		
	     	parent_name = ( char * ) malloc ( (unsigned int) strlen (str) + 1);
   		(void) strcpy ( parent_name, str );
		len = 6 * strlen ( class_name ) + strlen ( parent_name);
    	}
	else 
		len = 6 * strlen ( class_name );

	len += 256; /* Pour les commantaires */

	buf = ( char *) malloc ( (unsigned int) len );
	bzero ( buf,  len );

	(void) sprintf ( buf, "\n//\n//\tClass name : %s\n//\n//\tDescription : \n//\nclass %s",
		class_name, class_name );
   	if ( str != 0 ) {
		(void) strcat ( buf, " : public " );
		(void) strcat ( buf, parent_name );
	}
	(void) sprintf ( buf + strlen (buf),
		" {\npublic:\n\t%s ();\n\t%s ( const %s & );\n\t~%s ();\n\t\nprotected:\nprivate:\n};\n",
		 class_name, class_name, class_name, class_name );

	DisplayModel ( text, buf, 15, 4 );
	if ( class_name != 0 )
	   	(void) free ( class_name );
	if ( str != 0 ) {
      		if ( parent_name != 0 )
	   		(void) free ( parent_name );
	}
   	if ( buf != 0 )
		(void) free ( buf );
}


/*
**	Function name : MakeMethod
**
**	Description : Fabrique un squelette de methode.
**	Input : Le text courant.
**	Ouput :
*/
void MakeMethod ( text )
	Text *text;
{
	register char *buf, *method_name, *class_name, *str;
	register int len;
	char c = '\007';
/*
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent ); 
		return;
	}
*/
   	str = ( char *)  GetStringFromDialogBox ( text -> window, "Method name : " );
   	if ( (str == 0) || (strncmp(str, &c, 1) == 0) ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
   	method_name = ( char * ) malloc ( (unsigned int) strlen (str) + 1);
   	(void) strcpy ( method_name, str );
  
	str = ( char *)  GetStringFromDialogBox ( text -> window, "Class name : " );

   	if ( (str == 0) || (strncmp(str, &c, 1) == 0 ) ) {
      		if ( method_name != 0 )
			(void) free ( method_name );
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}

   	class_name = ( char * ) malloc ( (unsigned int) strlen (str) + 1);
	(void) strcpy ( class_name, str );

	len = ( 2 * strlen ( method_name )) + strlen ( class_name ) + 128;
	buf = ( char *) malloc ( (unsigned int) len );
	bzero ( buf,  len );

	(void) sprintf ( buf, 
		"\n//\n//\tMethod name : %s\n//\n//\tDescription : \n//\tInput : \n//\tOuput :\n//\n%s :: %s ()\n{\n\t\n}\n",
		method_name, class_name, method_name );

	DisplayModel ( text, buf, 12, 2 );
	if ( method_name != 0 )
	   	(void) free ( method_name );
   	if ( class_name != 0 )
		(void) free ( class_name );
   	if ( buf != 0 )
		(void) free ( buf );
}


/*
**	Function name : MakeFunction
**
**	Description : Fabrique un squelette de fonction.
**	Input : Le text courant.
**	Ouput :
*/
void MakeFunction ( text )
	Text *text;
{
	register char *buf, *f_name, *str;
	register int len;
	char c = '\007';
/*
	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, text -> w_parent ); 
		return;
	}
*/
   	str = ( char *)  GetStringFromDialogBox ( text -> window, "Function name : " );
   	if ( (str == 0) || (strncmp(str, &c, 1) == 0) ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
   	f_name = ( char * ) malloc ( (unsigned int) strlen (str) + 1);
   	(void) strcpy ( f_name, str );

	len = strlen ( f_name ) + 128;
	buf = ( char *) malloc ( (unsigned int) len );
	bzero ( buf,  len );

	(void) sprintf ( buf, 
		"\n/*\n**\tFunction name : %s\n**\n**\tDescription : \n**\tInput : \n**\tOuput :\n*/\n%s ()\n{\n\t\n}\n",
		f_name, f_name );

	DisplayModel ( text, buf, 12, 2 );

   	if ( f_name != 0 )
	   	(void) free ( f_name );
   	if ( buf != 0 )
		(void) free ( buf );
}


/*
**	Function name : DisplayModel
**
**	Description : Affiche les differents modeles
**	Input : Le text courant, le buffer a afficher,
**		le nombres de lignes, pour le retour
**		du curseur.
**	Ouput :
*/
static void DisplayModel ( text, buf, lines, x )
	Text *text;
	register char *buf;
	register int lines, x;	/* nb lines to insert, back */
{
	TextCursorOff ( text);
	ClipOn ( text, 0 );
	InsertLines ( text, buf, strlen (buf), lines ); 
	SetTextModif ( text );
	ClipOff ( text );

   	(void) MoveToLine ( text -> buf, -x ); 
   	MoveToEline ( text );
	text -> n1 -= x;
   	text -> n2 += x;
   	text -> no_current_line -= x;
   	(void) MoveScrollBar ( dpy, text -> swin, 
		CURRENT, text -> no_current_line - text -> n1 - 1 );

   	TextCursorOn ( text);
	SetAndDisplayPage ( text ); 
}

