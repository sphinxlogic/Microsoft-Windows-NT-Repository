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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/param.h>
#ifdef USG
#include <sys/unistd.h>
#include <sys/x.out.h>
#include <sys/vnode.h>
#include <sys/uio.h>
#include <sys/exec.h>
#endif
#ifndef apollo
#include <malloc.h>
#endif
#include <string.h> 

#include <errno.h>

#include "m_key.h"
#include "menus.h"
#include "buttons.h"
#include "scroll.h"
#include "text.h"
#include "browser.h"
#include "flist.h"

#ifdef sun
#define _SUN_OS
#include <sys/exec.h>
#define ISMAG(x) (((x) == OMAGIC) || ((x) == NMAGIC) || ((x) == ZMAGIC))
#endif

#ifdef apollo
#define _APOLLO
#include <filehdr.h>
#define ISMAG(x) ISCOFF(x)
#endif

#if  (m68k && _AUX_SOURCE)
#define _APPLE_A_UX
#include <filehdr.h>
#define ISMAG(x) (((x)==MC68MAGIC) || ((x)==MC68TVMAGIC) || ((x)==M68MAGIC) || ((x)==M68TVMAGIC) \
|| ((x)==M68NSMAGIC))
#endif

#ifdef hpux
#define _HPUX
#include <filehdr.h>
#define ISMAG(x) (((x) == RELOC_MAGIC) || ((x) == EXEC_MAGIC) || ((x) == SHARE_MAGIC) \
	|| ((x) == DEMAND_MAGIC) || ((x) == DL_MAGIC) || ((x) == SHL_MAGIC))
#endif

#if ((__osf__) && (__alpha))
#define _DEC_OSF
#include <sys/exec.h>
#define ISMAG(x) (((x) == OMAGIC) || ((x) == NMAGIC) || ((x) == ZMAGIC))
#endif

#if ((ultrix) && (mips))
#define _DEC_ULTRIX
#include <sys/exec.h>
#define ISMAG(x) (((x) == OMAGIC) || ((x) == NMAGIC) || ((x) == ZMAGIC))
#endif

#ifdef _AIX
#include <filehdr.h>
#define ISMAG(x) (((x) == U802WRMAGIC) || ((x) == U802ROMAGIC) || \
	((x) == U802TOCMAGIC) || ((x) == U800WRMAGIC) || \
	((x) == U800ROMAGIC)  || ((x) == U800TOCMAGIC))
#endif

#if ((sgi) || (__sgi))
#define _SGI
#include <filehdr.h>
#define ISMAG(x) ISCOFF(x)
#endif

#ifdef linux
#define _LINUX
#include <unistd.h>
#include <a.out.h>
#define ISMAG(x) (((x)==OMAGIC)||((x)==NMAGIC)||((x)==ZMAGIC))
#endif

#ifndef ISMAG
#define _BAD_SYS
#define ISMAG(x) (((x) == OMAGIC) || ((x) == NMAGIC) || ((x) == ZMAGIC))
#endif
#ifdef SVR4
#undef _BAD_SYS
#endif

extern char *GetString (), *GetStringFromDialogBox ();
extern 	Display	*dpy;
static int CheckFile (), IsBinary ();
static void WriteFile ();

/*
**	Function name : KbdReadFile
**
**	Description : Commande 'read file' a partir du clavier.
**	Input : Le text courant.
**	Ouput :
*/
void KbdReadFile ( text )
Text *text;
{
	register char *str; 
     	Text *result;
	char c = '\007'; /* ^G */

	if ( GetModif ( text ) == True ) {
      		if ( SaveCurrentBuffer ( text, F_KEY ) != True )
      			return;
	}
	ClearMessageWindow ( text -> mwin );

	str = GetString ( text, "Read File : ", (char *) 0 );

	if ( (str == 0) || (strncmp(str, &c, 1)) == 0 ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}
	TextCursorOff ( text );
	if ( LoadFile ( text, str, NEW ) != -1 ) {
		ShowScrollFrame ( dpy, text -> swin );
		FirstPage ( text );
		SetTextSave ( text );
		if ( IsAlreadyLoad ( text -> filename, &result ) > 1 ) {
			XBell ( dpy, 10 );
			DisplayMessage ( text -> mwin, "Warning ...Already loaded" );
		}
	}
	TextCursorOn ( text );
}


/*
**	Function name : KbdInsertFile
**
**	Description : Commande 'insert file' a partir du clavier.
**	Input : 
**	Ouput :
*/
void KbdInsertFile ( text )
Text *text;
{
	register char *str;
	char c = '\007'; /* ^G */

   	ClearMessageWindow ( text -> mwin );
	
	str = GetString ( text, "Insert File : ", (char *) 0 );
		
	if ( (str == 0) || (strncmp(str, &c, 1)) == 0 ) {
		DisplayMessage ( text -> mwin, "Abort" );
		return;
	}

	TextCursorOff ( text );
	if ( LoadFile ( text, str,  INSERT ) != -1 ) {
		SetTextModif ( text );		
		SetAndDisplayPage ( text ); 
		ShowScrollFrame ( dpy, text -> swin );
		(void) MoveScrollBar ( dpy, text -> swin, 
			CURRENT, text -> no_current_line - text -> n1 - 1 );
	}
	TextCursorOn ( text );
}


/*
**	Function name : LoadFile
**
**	Description : Charge un fichier.
**	Input : Le text courant, le nom du fichier, type de
**		l'operation ( nouveau ou insert ).
**	Ouput : 0 si OK -1 sinon
*/
int LoadFile ( text, s, type )
	Text *text;
	register char *s;
	register int type;
{
	static int len;
	register int n;
	FILE *fd;
#ifdef DEBUG
	(void) fprintf ( stderr, "filename = %s\n", s );
#endif
	if ( CheckFile ( s, &len, text ) == 0 ) {
		fd = fopen ( s, "r" );
		if ( fd == 0 ) {
			(void) fprintf ( stderr, "Open error\n" );
			return -1;
		}
		if ( IsBinary ( fd, text, len ) == True ) {
			(void) fclose ( fd );
			return -1;
		}
		rewind ( fd );
		switch ( type ) {
		case NEW:
			(void) LoadFileInBuffer ( text -> buf, fd, len, NEW );
			SetDirAndFilename ( text, s );
			XStoreName ( dpy,  text ->  w_parent, text -> filename ); 
			break;
		case INSERT:
			(void) LoadFileInBuffer ( text -> buf, fd, len,  INSERT );
			break;
		}
		n = GetNumberOfLineInBuf ( text -> buf );
#ifdef DEBUG
	(void) fprintf ( stderr, "n = %d\n", n );
#endif
		text -> lines_in_buf = n;
		SetScrollLine ( text -> swin, n );
		(void) fclose ( fd );
		return 0;
	}
	else
		return -1;
}

/*
**	Function name : SetDirFilename
**
**	Description : Positionne la directorie et le
**		pathname complet.
**	Input : Le text courant, le nom du fichier.
**	Ouput :
*/
void SetDirAndFilename ( text, name )
	Text *text;
	register char *name;
{
	register char *p;
   	register char *old_dir;
   	char pathname [ MAXPATHLEN ];
	extern char  *getcwd();

	/*
	 * Ici, name est un nom de fichier valide. On doit utiliser le pathname
	 * complet a cause du browser. Plusieurs cas sont possibles :
	 * name commence par / 
	 */
	old_dir = (char *) malloc ( (unsigned) strlen ( text -> current_dir ) + 2 );
   	(void) strcpy ( old_dir, text -> current_dir );
   
	if ( (p = strrchr ( name, '/' )) == 0 ) {  /* Nom de fichier de la directorie courante */
		(void) sprintf ( text -> filename,"%s/%s", text -> current_dir, name );
	}
	else if ( (p - name) == 0 ) { /* On doit aller dans la racine */
		(void) chdir ( "/" );
		(void) strcpy ( text -> current_dir, "/" );
		(void) sprintf ( text -> filename,"%s", name );
	}
	else {	/* Ya 1 ou plusieurs '/' */
      		if ( *name == '/' ) {
         			(void) strncpy ( text -> current_dir, name, ( p - name ));
			text -> current_dir [p - name] = '\0';
       		}
      		else {
       			(void) strncat ( text -> current_dir, "/", 1);
			(void) strncat ( text -> current_dir, name, ( p - name ));
       		}
		(void) sprintf ( text -> filename, "%s/%s", text -> current_dir, (char *) (p + 1) );
    	}
	if ( strcmp ( old_dir, text -> current_dir ) != 0 ) {
      		(void) chdir ( text -> current_dir );
	   	/*
   		 * A cause de l'automount et des liens symboliques il faut
	   	 * de nouveau verifier le path.
   		 */
	   	(void) getcwd ( pathname, MAXPATHLEN + 2 );
   		if ( strcmp ( pathname, text -> current_dir ) != 0 ) {
	      		(void) strcpy ( text -> current_dir, pathname );
      			(void) sprintf ( text -> filename, "%s/%s",
         				text -> current_dir, (char *) (p + 1) );
	    	}
      	}
#ifdef DEBUG      
	printf ( "dirname = %s\n", text -> current_dir );
      	printf ( "filename = %s\n", text -> filename );
#endif
	if ( old_dir != 0 )
		(void) free ( old_dir );
   	return;
}


/*
**	Function name : KbdSaveFile
**
**	Description : Commande 'save file' a partir du clavier.
**	Input : Le text courant.
**	Ouput :
*/
void KbdSaveFile ( text )
	Text *text;
{
	register char   *name;
	char c = '\007';

	if ( strcmp (text -> filename, "NoName") == 0 ) {
		name = GetString ( text, "Write file : ", (char *) 0 );
		if ( (name == 0) || strncmp(name, &c,1) == 0 ) {
			DisplayMessage ( text -> mwin, "Abort" );
			return;
		}
		else {
			if ( *name == '/' ) {
				(void) strcpy ( text -> filename, name );
			}
			else {
				(void) strcpy ( text -> filename,  text -> current_dir );
				(void) strcat ( text -> filename, "/" );
				(void) strcat ( text -> filename, name );
			}
			(void) WriteFile ( text );
		}
	}
	else {
#ifdef DEBUG
		(void) sprintf ( buf, "cp %s '#%s'\n",
		text -> filename, text -> filename );
		system ( buf );
#endif
		if ( text -> modif == True )
			(void) WriteFile ( text );
		else 
			DisplayMessage ( text -> mwin, "No changes" );	
	}
}


/*
**	Function name : WriteFile
**
**	Description : Ecriture du buffer courant dans un fichier.
**	Input : Le text courant.
**	Ouput :
*/
static void WriteFile ( text )
	Text *text;
{
	FILE *fd;
	register int size;
	extern char *sys_errlist[]; 
	char buf [128];
	register char *name;

	if ( (fd = fopen ( text -> filename, "w" )) == 0 ) {
		DisplayMessage ( text -> mwin, sys_errlist[errno] );
		return;
	}
	DisplayMessage ( text -> mwin, "Write ..." );
   	XSync ( dpy, False );
	size = WriteCurrentFile ( text -> buf, fd );
	if ( size == -1 )  {
		(void) sprintf ( buf, "Write error" );
		DisplayMessage ( text -> mwin, buf );
      		(void) fclose ( fd );
      		return;
	}
	else {
		name = 0;
		if ( (name = (char * ) strrchr ( text -> filename, '/' )) != 0 )
			(void) sprintf ( buf, "Write %s, %d bytes.\n", ++name, size );
		else {
			(void) sprintf ( buf, "Write %s, %d bytes.\n", text -> filename, size );
		}
		DisplayMessage ( text -> mwin, buf );
		SetTextSave ( text );
	}
	(void) fclose ( fd );
   	if ( (text -> filename != 0) && strlen ( text -> filename) != 0 ) {
      		if ( GoodSuffix ( text -> filename ) == True ) {
#ifdef DEBUG
	(void) fprintf ( stderr, "touch %s\n", text -> filename ); 
#endif
			parse_file ( text -> filename ); 
			RefreshBrowserInfos ();
			TextCursorOff ( text );
			TextCursorOn ( text );
		}
	}
}


/*
**	Function name : KbdWriteFile
**
**	Description : Commande 'write file' a partir du clavier.
**	Input : Le text courant.
**	Ouput :
*/
void KbdWriteFile ( text )
	Text *text;
{
	register char *s;
	char buf [128];
	char c = '\007';
	char *name;

#ifdef DEBUG
	(void) fprintf ( stderr, "Write File from key\n" );
#endif

	if ( (name = (char * ) strrchr ( text -> filename, '/' )) != 0 )
		(void) sprintf ( buf, "Write file [%s]  : ", ++name );
	else
		(void) sprintf ( buf, "Write file [%s]  : ", text -> filename );	

	s = GetString ( text, buf, (char *) 0 );
	if ( (s == 0) || ((strncmp(s,"y",1) == 0) &&(strlen(s)==1)) ) {
		WriteFile ( text );
	}
	else if ( ((strncmp(s,"n",1 ) == 0)&&(strlen(s)==1)) || (strncmp(s,&c,1)==0) ) {
		DisplayMessage ( text -> mwin, "Abort" );
	}
	else {
		if ( *s == '/' ) {
			(void) strcpy ( text -> filename, s );
		}
		else {
			(void) strcpy ( text -> filename,  text -> current_dir );
			(void) strcat ( text -> filename, "/" );
			(void) strcat ( text -> filename, s );
		}
		WriteFile ( text );
      		XStoreName ( dpy, text ->  w_parent, text -> filename );
	}
}


/*
**	Function name : CheckFile
**
**	Description : Quelles les permissions sur le fichier.
**	Input : Le pathname, la longueur, le text courant.
**	Ouput : 0 Ok -1 sinon
*/
static int CheckFile ( path, len, text )
	char *path;
	int *len;	/* RETURN */
	Text *text;
{
	struct stat st;

	(void) stat ( path, &st );
	if ( access ( path, F_OK ) != 0 ) {
		DisplayMessage  ( text -> mwin, "File not found" );
		return (-1);
	}
	if ( access ( path, R_OK ) != 0 ) {
		DisplayMessage ( text -> mwin, "Permission denied" );
		return (-1);
	}
	if ( access ( path, W_OK ) != 0 ) {
		DisplayMessage ( text -> mwin, "Read only file" );
	}
	if ( ! S_ISREG(st.st_mode) ) {
		DisplayMessage  ( text -> mwin, "Not a regular file" );
		return (-1);
	}

	*len = (int) st.st_size;
	return 0;
}

/*
**	Function name : IsBinary
**
**	Description : On verifie que le fichier n'est pas
**		un binaire ou une archive.
**	Input : Le stream associe, le text courant, la longueur du fichier.
**	Ouput : Vrai si binaire Faux sinon.
*/
static int IsBinary ( fd, text, len )
	FILE *fd;
	Text *text;
	int len;
{
   	unsigned short magic;
#ifdef _SUN_OS
	struct exec head;
#endif
#ifdef _HPUX
	struct header head;
#endif
#ifdef _DEC_OSF
	struct exec head;
#endif
#ifdef _DEC_ULTRIX
	struct exec head;
#endif
#ifdef _APPLE_A_UX
	struct filehdr head;
#endif
#ifdef _AIX
	struct filehdr head;
#endif
#ifdef _SGI
	struct filehdr head;
#endif
#ifdef _APOLLO
	struct filehdr head;
#endif
#ifdef _LINUX
	struct exec head;
#endif
#ifdef _BAD_SYS
	struct exec head;
#endif
#ifdef SVR4
	struct execsw head;
#endif
	if ( len < sizeof (head) )	/* Un peu court ... */
		return False;
	if (fread((char *) &head, sizeof (head), 1, fd) != 1) {
		(void) fprintf ( stderr, "Header read error\n" );
		return False;
    	}
#ifdef _SUN_OS
	magic = head.a_magic;
#endif
#ifdef _DEC_ULTRIX
	magic = head.ex_o.magic;
#endif
#ifdef _DEC_OSF
	magic = head.ex_o.magic;
#endif
#ifdef _HPUX
	magic = head.a_magic;
#endif
#ifdef _APPLE_A_UX
	magic = head.f_magic;
#endif
#ifdef _SGI
	magic = head.f_magic;
#endif
#ifdef _AIX
	magic = head.f_magic;
#endif
#ifdef _APOLLO
	magic = head.f_magic;
#endif
#ifdef _LINUX
	magic = (short) head.a_info;
#endif
 	if ( ISMAG (magic) 
		|| (!strncmp ( (char *) &head, (char *) "!<arch>", 6)) ) {
			DisplayMessage  ( text -> mwin, "%@#7&^/_!...brrr..." );
			return True;
	}
	return False ;
}


/*
**	Function name : MenuReadFile
**
**	Description : Commade 'read file' a partir du menu.
**	Input : Le text courant.
**	Ouput :
*/
void MenuReadFile ( text )
	Text *text;
{
	register 	char *str = 0;
   	Text *result;
	char c = '\007'; /* ^G */

   	if ( GetVisibility ( dpy, text ) == False )
		return;

	if ( GetModif ( text ) == True ) {
      		if ( SaveCurrentBuffer ( text, F_MENU ) != True )
      			return;
	}
	ClearMessageWindow ( text -> mwin );
	str = (char *) GetStringFromSelect ( text -> window, FILESELECT ); 
	if ( (str == 0) || (strncmp(str, &c, 1)) == 0 ) {
		DisplayMessage ( text -> mwin, "Abort" );
      		if ( str != 0 )
			(void) free ( str );
		return;
	}
	TextCursorOff ( text );
	if ( LoadFile ( text, str, NEW ) != -1 ) {
		ShowScrollFrame ( dpy, text -> swin );
		FirstPage ( text );
		SetTextSave ( text );
		if ( IsAlreadyLoad ( text -> filename, &result ) > 1 ) {
			XBell ( dpy, 10 );
			DisplayMessage ( text -> mwin, "Warning ...Already loaded" );
		}
	}
	TextCursorOn ( text );
   	if ( str != 0 )
		(void) free ( str );
}


/*
**	Function name : MenuInsertFile
**
**	Description : Commande 'insert file' a partir du menu.
**	Input : Le text courant.
**	Ouput :
*/
void MenuInsertFile ( text )
	Text *text;
{
	register char *str;
	char c = '\007'; /* ^G */

 	if ( GetVisibility ( dpy, text ) == False )
		return;

	ClearMessageWindow ( text -> mwin );
	str = (char *) GetStringFromSelect ( text -> window, FILESELECT );
	if ( (str == 0) || (strncmp(str, &c, 1)) == 0 ) {
		DisplayMessage ( text -> mwin, "Abort" );
      		if ( str != 0 )
			(void) free ( str );
		return;
	}
	TextCursorOff ( text );
	if ( LoadFile ( text, str,  INSERT ) != -1 ) {
		SetTextModif ( text );
		SetAndDisplayPage ( text );
		ShowScrollFrame ( dpy, text -> swin );
		(void) MoveScrollBar ( dpy, text -> swin, 
			CURRENT, text -> no_current_line - text -> n1 - 1 );
	}
	TextCursorOn ( text );
   	if ( str != 0 )
		(void) free ( str );
}


/*
**	Function name : MenuSaveFile
**
**	Description : Commande 'save file' a partir du menu.
**	Input : Le text courant.
**	Ouput :
*/
void MenuSaveFile ( text )
	Text *text;
{
	register char   *name;
	char c = '\007';
/*
 	if ( GetVisibility ( dpy, text ) == False )
		return;
*/
	if ( strcmp (text -> filename, "NoName") == 0 ) {
		name = (char *) GetStringFromDialogBox ( text -> window, "Write file : " );
		if ( (name == 0) || strncmp(name, &c,1) == 0 ) {
			DisplayMessage ( text -> mwin, "Abort" );
			return;
		}
		else {
			if ( *name == '/' ) {
				(void) strcpy ( text -> filename, name );
			}
			else {
				(void) strcpy ( text -> filename,  text -> current_dir );
				(void) strcat ( text -> filename, "/" );
				(void) strcat ( text -> filename, name );
			}
			WriteFile ( text );
		}
	}
	else {
#ifdef DEBUG
		(void) sprintf ( buf, "cp %s '#%s'\n",
			text -> filename, text -> filename );
		system ( buf );
#endif
		if ( text -> modif == True )
			WriteFile ( text );
		else
			DisplayMessage ( text -> mwin, "No changes" );
	}
}


/*
**	Function name : MenuWriteFile
**
**	Description : Commande 'write file' a partir du menu.
**	Input : Le text courant.
**	Ouput :
*/
void MenuWriteFile ( text )
	Text *text;
{
	register char *s;
	char buf [128];
	char c = '\007';
	register char *name;
/*
 	if ( GetVisibility ( dpy, text ) == False )
		return;
*/
#ifdef DEBUG
	(void) fprintf ( stderr, "Write File from menu\n" );
#endif
	if ( (name = (char * ) strrchr ( text -> filename, '/' )) != 0 )
		(void) sprintf ( buf, "Write file [%s]  : ", ++name );
	else
		(void) sprintf ( buf, "Write file [%s]  : ", text -> filename );	

	s = GetStringFromDialogBox ( text -> window, buf );
	if ( (s == 0) || ((strncmp(s,"y",1) == 0)&&(strlen(s)==1)) ) {
		WriteFile ( text );
	}
	else if ( ((strncmp(s,"n",1 ) == 0)&&(strlen(s)==1)) || (strncmp(s,&c,1)== 0) ) {
		DisplayMessage ( text -> mwin, "Abort" );
	}
	else {
		if ( *s == '/' ) {
			(void) strcpy ( text -> filename, s );
		}
		else {
			(void) strcpy ( text -> filename,  text -> current_dir );
			(void) strcat ( text -> filename, "/" );
			(void) strcat ( text -> filename, s );
		}
		WriteFile ( text );
      		XStoreName ( dpy, text ->  w_parent, text -> filename );   
	}
}


/*
**	Function name : SaveCurrentBuffer
**
**	Description : Sauve le buffer courant.
**	Input : Le text courant, clavier/menu.
**	Ouput :
*/
int SaveCurrentBuffer ( text, from )
	Text *text;
	register int from;
{
	register 	char *str = 0;
	char c = '\007'; /* ^G */
   	register char *buf, *name;
	
   	buf = (char *) malloc ( (unsigned) strlen ( text -> filename ) + 64 );

	if ( (name = (char * ) strrchr ( text -> filename, '/' )) != 0 )
		(void) sprintf ( buf, "Save buffer [%s] ? [y/n]", ++name );
	else
	(void) sprintf ( buf ,"Save buffer [%s] ? [y/n] ", text -> filename );

	if ( from == F_KEY )   
		str = (char * ) GetString ( text, buf, (char *) 0);
	else 
		str = (char * ) GetStringFromDialogBox ( text -> window, buf );
	while ( True ) {
		if ( (str == 0) || ((strncmp(str,"y",1) == 0)&&(strlen(str)==1)) ) {
			WriteFile ( text );
			break;
		}
		if ( strncmp(str,&c,1) == 0 ) {
			DisplayMessage ( text -> mwin, "Abort" );
         			if ( buf != 0 )
				(void) free ( buf );
			return False;
		}
		if ( ((strncmp(str,"n",1) == 0)&&(strlen(str)==1)) ) {
			break;
		}
		if ( from == F_KEY )
			str = (char *) GetString ( text,
				"Please answer y or n : ", (char *) 0 );
		else
			str = (char * ) GetStringFromDialogBox ( text -> window,
				"Please answer y or n : "  );	
	}
   	if ( buf != 0 )
	   	(void) free ( buf );
	return True;
}


/*
**	Function name : MenuGotoLine
**
**	Description : Va a la ligne n.
**	Input : Le text courant.
**	Ouput :
*/
void MenuGotoLine ( text )
	Text *text;
{
	register int n;
	register char *str;
	char c = '\007';
/*
 	if ( GetVisibility ( dpy, text ) == False )
		return;
*/
	str = (char * ) GetStringFromDialogBox ( text -> window, "Line Number : " );
	if ( (str == 0) || (strncmp (str, &c, 1 ) == 0) || ((n = atoi ( str )) == 0) )
		DisplayMessage ( text -> mwin, "Abort" );
	else {
		TextCursorOff ( text );
		GotoLineNumber ( text, n );
		CurrentLineToMiddle ( text ); 
		TextCursorOn ( text );
	}
}


/*
**	Function name : KillCurrentBuffer
**
**	Description : Comme son nom l'indique.
**	Input : Le text courant.
**	Ouput :
*/
void KillCurrentBuffer ( text, from )
	Text *text;
	register int from;
{
	if ( GetModif ( text ) == True ) {
		if ( from == F_KEY ) {
			if ( SaveCurrentBuffer ( text, F_KEY ) != True )
				return;
		}
		else {
			if ( SaveCurrentBuffer ( text, F_MENU ) != True )
				return;
		}
	}
	KillText ( dpy, text );
	RefreshScroll ( dpy,  text -> swin, 
		text -> width + GetScrollWidth () + W_SPACE + 1, text -> height, 0 );
	SetTextSave ( text );
   	SetDirAndFilename ( text, "NoName" );
	XStoreName ( dpy, text ->  w_parent, "NoName" );   
}


/*
**	Function name : MenuNewFile
**
**	Description : 
**	Input : 
**	Ouput :
*/
void MenuNewFile ( text )
Text *text;
{
	TextCursorOff ( text );
	KillCurrentBuffer ( text, F_MENU );
	TextCursorOn ( text );
}

