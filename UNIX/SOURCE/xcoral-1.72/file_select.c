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
#include <stdio.h>
#ifdef apollo
#include <sys/dir.h>
#define dirent direct
#else
#include <dirent.h>
#endif
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef apollo
#define S_ISDIR
#else
#include <malloc.h>
#endif
#include <string.h>

#include "text.h"
#include "options.h"
#include "flist.h"

extern Display *dpy;
extern char  *getcwd();

#define FS_SHADOW 	6
#define FS_SPACE	5
#define FS_BUFFER_SIZE	10000

typedef struct {
	Window 		frame, shadow, title, dir, main, ok, cancel;
/* new */	Window		dirframe, fileframe, dirtitle, filetitle, dirtext, filetext;
/* new */	Window		fieldframe, fieldtext;
	unsigned long 	fg, bg, ts, bs, text_fg, text_bg;
	XFontStruct	*font;
    	GC		gc;
    	int 		width, height, title_height, button_width, button_height;
    	Text		*text;
    	Buf		*buf;
    	SWin		*scroll;
    	int		select;
    	int		m_width, m_height;
    	char		*dirname;
	Time		click_time;
	Time		old_click;
} Selector;

/*
 * Public
 */
extern void InitFileSelector ();
extern char *GetStringFromSelect ( /* parent, type */ );

/* 
 * Private
 */
static Selector fs;
static void ButtonFileSelect (), ExposeFileSelect (), SetParams (),
	CleanButton (),	RefreshFileSelect (), RefreshDirWindow (),
	SelectFileItem (), UpdateFileItem (),ConfigFileSelect (),
	SortFiles (), UnmapFileSelect ();
static int MyCompare ();


/*
**	Function name : InitFileSelector
**
**	Description : Creation des fenetres, boutons, scroll etc...
**	Input : 
**	Ouput :
*/
void InitFileSelector ()
{
	XSetWindowAttributes att_f;    
	Window 	root;
	int 	screen;
	unsigned long black, white;
	XGCValues	 gcv;
 
    	screen = DefaultScreen ( dpy );
	root = RootWindow ( dpy, screen );
	black = BlackPixel ( dpy, DefaultScreen ( dpy ));
	white = WhitePixel ( dpy, DefaultScreen ( dpy ));

	fs.fg = (DefaultDepth ( dpy, screen ) == 1) ? black : GetOpColor ( OP_MENU_FG );
	fs.bg = (DefaultDepth ( dpy, screen ) == 1) ? white : GetOpColor ( OP_MENU_BG );
	fs.text_bg = GetOpColor ( OP_TEXT_BG );
	fs.text_fg = GetOpColor ( OP_TEXT_FG );
	fs.ts = GetOpColor ( OP_MENU_TS );
	fs.bs = GetOpColor ( OP_MENU_BS );
	fs.font = GetOpFont ( OP_TEXT_FONT );

	fs.gc = XCreateGC ( dpy, DefaultRootWindow ( dpy ), 0,  &gcv );
 	XCopyGC ( dpy, DefaultGC (dpy, screen ), (~0), fs.gc );

	XSetFont ( dpy, fs.gc, fs.font -> fid );
	XSetForeground ( dpy, fs.gc, fs.fg );
	XSetBackground ( dpy, fs.gc, fs.bg );

	fs.title_height = fs.font -> ascent + fs.font-> descent + 8;
	fs.button_height = fs.font -> ascent + fs.font-> descent + 8;
	fs.width = DisplayWidth ( dpy,DefaultScreen ( dpy )) / 2;
	fs.height = DisplayHeight ( dpy,DefaultScreen ( dpy ))/2;

    	att_f.win_gravity = NorthWestGravity;
	att_f.event_mask = 0;
	att_f.override_redirect = False;
	att_f.do_not_propagate_mask = NoEventMask;
	att_f.cursor = None;
	att_f.win_gravity = NorthWestGravity;
 
    	fs.frame = XCreateWindow ( dpy, root, 0, 0, fs.width, fs.height, 0, 0,
		InputOutput, CopyFromParent,
		CWWinGravity | CWEventMask | CWOverrideRedirect |
		CWDontPropagate | CWCursor, &att_f );

            	fs.shadow = XCreateSimpleWindow ( dpy, fs.frame, FS_SHADOW, FS_SHADOW, 
		fs.width, fs.height, 0, black, black);

    	fs.title = XCreateSimpleWindow (dpy, fs.frame, 
		0, 0, fs.width, fs.title_height, 0 , black, fs.bg );

        	fs.dir = XCreateSimpleWindow (dpy, fs.frame, 
		0, fs.title_height, fs.width, fs.title_height, 0 , black, fs.bg );

    	fs.main = XCreateSimpleWindow (dpy, fs.frame, 
		0, (2*(fs.title_height)),
        		fs.width, fs.height - fs.title_height, 0, black, fs.bg );
       	fs.button_width = XTextWidth ( fs.font, "  Ok...  ", 9 ) + 10;		
	fs.ok = XCreateSimpleWindow (dpy, fs.main, 0, 0,
		fs.button_width,fs.button_height, 0, black, fs.bg );
	fs.cancel = XCreateSimpleWindow (dpy, fs.main, 0, 0,
		fs.button_width, fs.button_height, 0, black, fs.bg );

	XSelectInput ( dpy, fs.title, ExposureMask );
    	XSelectInput ( dpy, fs.dir, ExposureMask );
	XSelectInput ( dpy, fs.main, ExposureMask );
	XSelectInput ( dpy, fs.ok, ExposureMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput ( dpy, fs.cancel, ExposureMask | ButtonPressMask | ButtonReleaseMask);	

    	fs.text = ( Text * ) MakeTextWindow ( dpy, fs.main, FS_SPACE, FS_SPACE );
	fs.scroll = ( SWin  * ) MakeScroll ( dpy, fs.main,  0, FS_SPACE ); 
	fs.text -> swin = fs.scroll;
	fs.scroll -> text = (char *) fs.text;
	fs.text -> mwin = 0;
	fs.buf = (Buf *) GetBuffer ( (unsigned) FS_BUFFER_SIZE );
	fs.text -> buf = fs.buf;
	fs.select = 0;
	fs.dirname = 0;
	fs.old_click = 0;
}


/*
**	Function name : ReadDir
**
**	Description :  Charge les noms de fichiers dans le buffer.
**	Input : Le nom de la directorie.
**	Ouput : 0 si OK -1 sinon
*/
static int ReadDir ( dir )
char *dir;
{
	DIR *dirp;
	struct dirent *dp;
     	register int n;
    	char **tmp;
    	register int i=0;
    	register int dir_len = 0;

	dirp = opendir( dir );
    	if ( dirp == 0 ) 
		return -1;

	ClearBuffer ( fs.buf ); 
	for (dp = readdir(dirp); dp != 0; dp = readdir(dirp)) 
    		dir_len ++;
    	(void) closedir ( dirp );
    	dirp = opendir( dir );
    	tmp = (char **) malloc ( (unsigned) ( sizeof (char *) * (dir_len + 2)));

	for (dp = readdir(dirp); dp != 0; dp = readdir(dirp)) {
		if ( (dp->d_name != 0) && (strlen(dp->d_name) != 0) ) {
            			tmp [i] = (char *) malloc ( (unsigned) strlen (dp->d_name) + 2);
			(void) strcpy ( (char *) tmp[i], dp->d_name );
			i++;
    		}
    	}
	qsort ( (char *) tmp, i, sizeof ( char *), MyCompare ); 
	for ( n = 0; n < i; n++ ) {
        		InsertNchar ( fs.buf, (char *) tmp [n], 
            			strlen ((char *) tmp [n] ));
      		if ( tmp[n] != 0 )
			(void) free ( (char *) tmp[n] );
		InsertNchar ( fs.buf, "\n", 1 );
    	}
   	if ( tmp != 0 )
		(void) free ( (char *) tmp );
	SetParams ( FILESELECT, dir );
    	(void) closedir ( dirp );
    	return 0;
}


/*
**	Function name : SetParams
**
**	Description : Positionne quelques parametres, en fonction
**		du type d'objet a afficher.
**	Input : Le type, la directorie.
**	Ouput :
*/
static void SetParams ( type, dir )
	register int type;
	register char *dir;
{
	register char *pathname;
   	register int n;

	fs.text -> modif = True;
	(void) strcpy ( (char *) fs.text -> filename, "NoName" );
	n = GetNumberOfLineInBuf ( fs.text -> buf );
	fs.text -> lines_in_buf = n;
	SetScrollLine ( fs.text -> swin, n );
	HoleToLeft ( fs.buf );
	
	if ( fs.dirname != 0 )
		(void) free ( fs.dirname );

	switch ( type ) {
		case FILESELECT:
		    	pathname = (char *) malloc ( (unsigned) MAXPATHLEN + 2 );
			(void) getcwd ( (char*) pathname, MAXPATHLEN + 2 );
			if ( strcmp ( dir, "." ) == 0 ) {
		    		fs.dirname = ( char *) malloc ( (unsigned) strlen (pathname) +2 );
					(void) strcpy ( fs.dirname, (char *) pathname );
			}
			else {
	    			fs.dirname = ( char *) malloc ( (unsigned) strlen (dir) +2 );
		    		(void) strcpy ( fs.dirname, dir );
			}
      			if ( pathname != 0 )
			    	(void)  free ( pathname );
			break;
		case KILLBUF:
			fs.dirname = ( char *) malloc ( (unsigned) 64 );
		    	(void) strcpy ( fs.dirname, "Select : " );
			break;
		case OPENFILES:
			fs.dirname = ( char *) malloc ( (unsigned) 256 );
		    	(void) strcpy ( fs.dirname, "Select : " );
			break;
		default:
			break;	
	}
}


/*
**	Function name : MyCompare
**
**	Description : Compare  2 chaines
**	Input : 2 tableaux de chaines
**	Ouput : Vrai si s1 = s2 Faux sinon
*/
static int MyCompare ( s1, s2 )
	register char **s1, **s2;
{
	return ( strcmp ( *s1, *s2 ));
}


/*
**	Function name : GetStringFromSelect
**
**	Description : Retourne la chaine selectionnee.
**	Input : La fenetre parent, le type d'objet
**	Ouput : La chaine.
*/
char *GetStringFromSelect ( window, type )
Window window;
register int type;
{
	register char *str, *tmp;
	XEvent event;
	int len;

	switch ( type ) {
		case FILESELECT:
		     	(void) ReadDir ( "." );
			break;
		case KILLBUF:
		      	ClearBuffer ( fs.buf ); 		
			(void) LoadKillBuffer ( fs.buf );
			SetParams ( KILLBUF, (char *) 0 );
			break;
		case OPENFILES:
			ClearBuffer ( fs.buf );
			LoadFileNames ( fs.buf ); 
			SetParams ( OPENFILES, (char *) 0 );
			break;
		default:
		break;
		
	}
	ConfigFileSelect ( window );

   	XGrabPointer ( dpy, fs.frame, True, ButtonReleaseMask, GrabModeAsync,
		GrabModeAsync, fs.frame, None, CurrentTime );

   	for (;;) {
		XNextEvent ( dpy, &event );
		switch ( event.type ) {
		case Expose:
			ExposeFileSelect ( event.xexpose.window, type );
			break;
		case ButtonPress:
			fs.click_time = event.xbutton.time;
			if ( event.xbutton.window == fs.cancel 
				|| event.xbutton.window == fs.ok 
				|| FsDoubleClick () == True ) {
				CleanButton ( event.xbutton.window, event.xbutton.button );
				if ( event.xbutton.window != fs.cancel ) {
					tmp = (char *) GetCurrentLine ( fs.buf, &len );
					tmp [len] = 0;
					switch ( type ) {
					case FILESELECT:
						str = (char *) malloc ( (unsigned) len + strlen(fs.dirname) + 2 );
						if ( strcmp ( tmp, "." ) == 0 || fs.select == 0 ) 
							(void) sprintf ( str, "%s", fs.dirname );
	   					else {
							if ( strcmp ( fs.dirname, "/" ) == 0 ) 
								(void) sprintf ( str, "/%s", tmp );	
							else
		      						(void) sprintf ( str, "%s/%s", fs.dirname, tmp );
						}
						break;
					case KILLBUF:
					case OPENFILES:
						str = (char *) malloc ( (unsigned) len + 2 );
						(void) sprintf ( str, "%s", tmp );
						break;
					default:
						break;
					}
					fs.select = 0;
					XUngrabPointer ( dpy, CurrentTime );
					UnmapFileSelect ();
					return ( str );
				}
				fs.select = 0;
				XUngrabPointer ( dpy, CurrentTime );
				UnmapFileSelect ();
				return 0;				
			}
			else
				ButtonFileSelect ( event.xbutton.window,
					event.xbutton.x, event.xbutton.y, type );
			break;
		default :
			break;	
		}
	}
}


/*
**	Function name : FsDoubleClick
**
**	Description : 
**	Input : 
**	Ouput :
*/
int FsDoubleClick ()
{
	if ( fs.old_click == 0 ) 
		return False;
	
	if ( (fs.click_time - fs.old_click) < 200)
		return True;
	else
		return False;
}


/*
**	Function name : ButtonFileSelect
**
**	Description : Traitement de l'evennement 'ButtonPress'
**	Input : La fenetre, la position, le type. 
**	Ouput :
*/
static void ButtonFileSelect ( w, x, y, type )
Window w;
register int x, y;
register int type;
{
	int result;

	if ( ButtonPressInScroll ( fs.scroll,w, y, &result )) {
		UpdateFileItem ( fs.select );
		switch ( result ) {
		case CURSOR:
			/* Pour mettre a jour la ligne courante */
			TextCursorOn ( fs.text );
			TextCursorOff ( fs.text );
			if ( (TextInBuf ( fs.text ) == True) 
				&& ( fs.text -> lines_in_buf > 1 )) {
				HandleScrollBar ( dpy, fs.scroll, ScrollNLine );
				RefreshScrollBar ( dpy, fs.scroll );
				XGrabPointer ( dpy, fs.frame, True, ButtonReleaseMask, GrabModeAsync,
					GrabModeAsync, fs.frame, None, CurrentTime );
			}
			break;
		case NEXT:
			TextCursorOff ( fs.text );
			NextPage ( fs.text  );
			break;
		case PREVIOUS:
			TextCursorOff ( fs.text );
			PreviousPage ( fs.text );
			break;
		}
		TextCursorOn ( fs.text );
		TextCursorOff ( fs.text );
		UpdateFileItem ( fs.select );
	}
	else if ( w == fs.text -> window ) 
		SelectFileItem ( x, y, type );
}


/*
**	Function name : ExposeFileSelect
**
**	Description : Traitement de l'evennement 'Expose'
**	Input : La fenetre, le type
**	Ouput :
*/
static void ExposeFileSelect ( w, type )
	Window w;
	register int type;
{
	register int x;
	XEvent event;
	char s_title [32];

	switch ( type ) {
		case FILESELECT:
			(void) strcpy ( s_title, "File Selection" );
			break;
		case KILLBUF:
			(void) strcpy ( s_title, "Kill Buffers" );
			break;
		case OPENFILES:
			(void) strcpy ( s_title, "Open Files" );
			break;
		default:
			break;
	}

	if ( w == fs.title ) {
	    	x = XTextWidth ( fs.font, s_title, 14 );
		XDrawString ( dpy, fs.title, fs.gc, (fs.m_width - x)/2,
			fs.font -> ascent + 5, s_title, strlen (s_title) );
		Display3D ( dpy, fs.title, fs.ts, fs.bs, 1, 0 );
	}
	if ( w == fs.dir ) {
		FirstPage ( fs.text );
		if ( type == FILESELECT ) 
			RefreshDirWindow ( fs.dirname, FILESELECT  );
		else
			RefreshDirWindow ( fs.dirname, KILLBUF  );
	}
	else if ( w == fs.main ) 
		Display3D ( dpy, fs.main, fs.ts, fs.bs, 1, 0 );
	else if ( w == fs.ok ) {
		switch ( type ) {
			case FILESELECT:
				(void) strcpy ( s_title, "  Ok...  " );
				break;
			case KILLBUF:
				(void) strcpy ( s_title, "Restore" );
				break;
			case OPENFILES:
				(void) strcpy ( s_title, "Raise" );
				break;
			default:
				break;
		}
		x = XTextWidth ( fs.font, s_title, strlen (s_title) );
		XDrawString ( dpy, fs.ok, fs.gc,  (fs.button_width - x)/2,
			fs.font -> ascent + 3, s_title, strlen (s_title) );
		Display3D ( dpy, fs.ok, fs.ts, fs.bs, 1, 0 );
	}
	else if ( w == fs.cancel ) {
		x = XTextWidth ( fs.font, "Cancel", 6 );
		XDrawString ( dpy, fs.cancel, fs.gc, (fs.button_width - x)/2,
			fs.font -> ascent + 3, "Cancel", 6 );
		Display3D ( dpy, fs.cancel, fs.ts, fs.bs, 1, 0 );
	}
	else if ( w == fs.text -> window ) {
		RefreshFileSelect ();
	}
	else if ( w == fs.scroll -> frame ) {
		RefreshScrollFrame ( dpy,  fs.scroll );
	}
	else if ( w == fs.scroll -> scroll ) {
		RefreshScrollBar ( dpy, fs.scroll );
	}
	while ( XCheckWindowEvent ( dpy, w, ExposureMask, &event ));
}


/*
**	Function name : CleanButton
**
**	Description : Remet un boutton dans sa forme initiale.
**	Input : La fenetre, le boutton.
**	Ouput :
*/
static void CleanButton ( w, button )
	Window w;
	unsigned int button;
{
	Display3D ( dpy, w, fs.ts, fs.bs, 1, 1 ); 
	XGrabPointer ( dpy, w, True, ButtonReleaseMask, GrabModeAsync,
		GrabModeAsync, w, None, CurrentTime );
	WaitButtonRelease ( button );
	Display3D ( dpy, w, fs.ts, fs.bs, 1, 0 );
}


/*
**	Function name : RefreshFileSelect
**
**	Description : Postionne les elements
**	Input : 
**	Ouput :
*/
static void RefreshFileSelect ()
{
	XClearWindow ( dpy, fs.text -> window );
	Display3D ( dpy, fs.text -> window, fs.ts, fs.bs, 2, 1 ); 
	ClipOn ( fs.text, 0 );
	SetLinesTable ( fs.text );
	RefreshPage ( fs.text );
	ShowScrollFrame ( dpy, fs.text -> swin );    
	ClipOff ( fs.text );
}


/*
**	Function name : RefreshDirWindow
**
**	Description : Met a jour le texte dans le fenetre dir
**	Input : Le nom, le type.
**	Ouput :
*/
static void RefreshDirWindow ( dir, type )
	register char *dir;
	register int type;
{
	register int start, width;
	register char *old = dir;

	XClearWindow ( dpy, fs.dir );
   	if ( type == KILLBUF || type == OPENFILES ) {
      		XDrawString ( dpy, fs.dir, fs.gc, MARGE, fs.font -> ascent + 5, dir, strlen(dir));
      		Display3D ( dpy, fs.dir, fs.ts, fs.bs, 1, 0 );	
      		return;
    	}
	start = MARGE + XTextWidth ( fs.font, "Dir : ", 6 ) + 3;
	width = fs.m_width - start -MARGE - XTextWidth ( fs.font, "...", 3 );
	while ( XTextWidth ( fs.font, dir, strlen(dir)) > width ) 
		dir ++;
	XDrawString ( dpy, fs.dir, fs.gc, MARGE, fs.font -> ascent + 5, "Dir : ", 6 );
	if ( old != dir ) {
		XDrawString ( dpy, fs.dir, fs.gc, start, fs.font -> ascent + 5, "...", 3 );
	 	XDrawString ( dpy, fs.dir, fs.gc,
			start + XTextWidth ( fs.font, "...", 3 ), 
			fs.font -> ascent + 5, dir, strlen(dir) );		
	}
	else
	 	XDrawString ( dpy, fs.dir, fs.gc,
			start, fs.font -> ascent + 5, dir, strlen(dir) );
	Display3D ( dpy, fs.dir, fs.ts, fs.bs, 1, 0 );
}


/*
**	Function name : SlectFileItem
**
**	Description : Selection d'un item.
**	Input : La position, le type.
**	Ouput :
*/
static void SelectFileItem ( x, y, type )
register int x, y;
register int type;
{
	register int n;
    	register char *s, *end_name;
    	int len;
    	struct stat st;
	char *tmp, *name_select;

	(void) MoveToXYinTextWindow (  fs.text,  x, y );
	TextCursorOn ( fs.text );
	TextCursorOff ( fs.text );

	if ( (n = fs.text -> no_current_line) >= fs.text -> lines_in_buf )
		return;

	UpdateFileItem ( fs.select ); /* Eteint l'item courant */
	UpdateFileItem ( n );	 /* Allume l'item selectionne */
	fs.select = n;
	s = (char * ) GetCurrentLine ( fs.buf, &len );
   
   	if ( type == KILLBUF || type == OPENFILES ) {
      		(void) sprintf ( fs.dirname, "Select : " );
      		(void) strncat ( fs.dirname, s, len );
		len += strlen ( "Select : " );
		fs.dirname [len] = '\0';
      		RefreshDirWindow ( fs.dirname, KILLBUF );
		fs.old_click = fs.click_time;
      		return;		
 	}
	name_select = (char *) malloc ( (unsigned) len + 2 );
	(void) strncpy ( name_select, s, len);
	name_select [len] = '\0';
#ifdef DEBUG
	fprintf ( stderr, "\nfs.dirname = %s\nname_select = %s\n", fs.dirname, name_select );
#endif
        	len = strlen(fs.dirname) + strlen(name_select);
	tmp = (char *) malloc ( (unsigned) len + 2 ); 
	(void) sprintf ( tmp, "%s/%s", fs.dirname, name_select );
    	
    	if ( (strcmp ( tmp, "/.." ) == 0) || (strcmp ( name_select, ".") == 0) ) {
   		if ( tmp != 0 )
	        		(void) free (tmp);
   		if ( name_select != 0 )
	        		(void) free (name_select);
        		return;
	}
	if ( stat ( tmp, &st ) == 0 ) {
		if ( S_ISDIR(st.st_mode) != 0 ) {
			fs.old_click = 0;					
			if ( strcmp (name_select, "..") == 0 ) {
       				end_name = (char * ) strrchr ( (char *) fs.dirname, '/' );
				len = strlen(fs.dirname) - strlen(end_name);
				tmp [len] = 0;
				if ( len == 0 ) {	/* Root */
					(void) strncpy ( tmp, "/", 1 );
					tmp [1] = 0;
				}
			}
			else {
				if ( strcmp (fs.dirname, "/") == 0 )
					(void) sprintf ( tmp, "%s%s", fs.dirname, name_select );
				else 
					(void) sprintf (tmp, "%s/%s", fs.dirname, name_select ); 
			}
			if ( ReadDir ( tmp ) == 0 ) {
				UpdateFileItem ( fs.select ); 
				fs.select = 0;
				RefreshDirWindow ( fs.dirname , FILESELECT );
				FirstPage ( fs.text );
				RefreshFileSelect ();
			}
			else
				RefreshDirWindow ( "Can't open dir", FILESELECT );
		}
		else {
			fs.old_click = fs.click_time;
			RefreshDirWindow ( fs.dirname, FILESELECT );
		}
	}
	else
		RefreshDirWindow ( "Stat error", FILESELECT );
	if ( name_select != 0 )
		(void) free (name_select); 
	if ( tmp != 0 )
	    	(void) free (tmp);
}


/*
**	Function name : UpdateFileItem
**
**	Description : Allume ou eteint l'item courant.
**	Input : Le no de l'item. 
**	Ouput :
*/
static void UpdateFileItem ( n )
	register int n;
{
	register int y;

	if ( (n == 0)
		|| ( n < (fs.text -> no_current_line - fs.text -> n1))
		|| ( n > (fs.text -> no_current_line + fs.text -> n2)))
		return;

	y = n - ( fs.text -> no_current_line - fs.text -> n1 );
	y *= fs.text -> font_height;
	y += MARGE;

	XFillRectangle ( dpy, fs.text -> window,
		fs.text -> Igc, MARGE,  y, 
		fs.text -> width - ( 2 * MARGE ),
		fs.text -> font_height + 1 );
}


/*
**	Function name : UnmapFileSelect
**
**	Description : Cache le file selecteur.
**	Input : 
**	Ouput :
*/
static void UnmapFileSelect ()
{
	XUnmapSubwindows ( dpy, fs.main );
	XUnmapWindow ( dpy, fs.main );
	XUnmapSubwindows ( dpy, fs.frame );
	XUnmapWindow ( dpy, fs.frame );

	XReparentWindow ( dpy, fs.frame, DefaultRootWindow (dpy), 0, 0 );
}


/*
**	Function name : ConfigFileSelect
**
**	Description : Prepare l'affichage, geometrie etc...
**	Input : La fenetre parent. 
**	Ouput :
*/
static void ConfigFileSelect ( parent )
	Window parent;
{
	XWindowAttributes att;
	register int width, height, x, y, i;

	XGetWindowAttributes ( dpy, parent, &att );
	width = (att.width / 4) * 3;
	height =  (att.height / 4) * 3;
	x = att.width / 8;
	y = att.height / 8;

        	fs.m_width = width - FS_SHADOW;
    	fs.m_height = height - (2*fs.title_height) - FS_SHADOW;

    	XReparentWindow ( dpy, fs.frame, parent, x, y );
	XResizeWindow ( dpy, fs.frame, width, height );
	XResizeWindow ( dpy, fs.shadow, width - FS_SHADOW , height - FS_SHADOW );
	XResizeWindow ( dpy, fs.main, 
		width - FS_SHADOW, height - (2*fs.title_height) - FS_SHADOW );
	XResizeWindow ( dpy, fs.title,  width -FS_SHADOW , fs.title_height );
    	XResizeWindow ( dpy, fs.dir,  width -FS_SHADOW , fs.title_height );
	XMoveWindow ( dpy, fs.ok, ( width/3 ) - ( fs.button_width/2 ), 
        		fs.m_height - fs.title_height - FS_SPACE );
	XMoveWindow ( dpy, fs.cancel, ( width*2 / 3 ) - ( fs.button_width/2 ), 
        		fs.m_height - fs.title_height - FS_SPACE );
	XMapSubwindows (dpy, fs.main );
	XMapWindow ( dpy, fs.main );
	XMapSubwindows ( dpy, fs.frame);
	XMapRaised ( dpy, fs.frame);

	x = fs.m_height - (3 * FS_SPACE) -  fs.button_height; 
	x -= ( 2 * MARGE );
	i = x / fs.text -> font_height;
	x = fs.m_height - (3 * FS_SPACE) -  fs.title_height;

	SetScrollLinePage (  fs.scroll, i ); 
        	ShowWindowText ( dpy, fs.text, 
        		fs.m_width - GetScrollWidth () - 1 - ( 2 * FS_SPACE) , x );
	i = fs.text -> no_current_line - fs.text -> n1 - 1;
	RefreshScroll ( dpy,  fs.scroll,  fs.m_width - FS_SPACE, x, i );
}
