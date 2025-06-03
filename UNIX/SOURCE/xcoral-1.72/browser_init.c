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
#ifdef apollo
#include <sys/dir.h>
#define dirent direct
#else
#include <dirent.h>
#endif
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#ifndef apollo
#include <malloc.h>
#endif
#include "options.h"
#include "browser.h"
#include "flist.h"

extern Display  	*dpy;
Browser		br;

char *b_name[] = {
	"Classes", 	"Parents",	"Children", "Methods",
	"Functions", "Files","Visit", 0
};

static void BrowserButtons ();
static void BrowserTextWindows ();
static void SetFileBuf ();
static int PathCompare ();

static void ConfigClass (), ConfigParent (), ConfigChild ();
static void ConfigFiles (), ConfigMethod (), ConfigProc ();

static int already_map = False;
static  Atom wm_delwin;

/*
**	Function name : InitBrowser
**
**	Description : Initialisation du browser.
**		Creation des fenetres, boutons etc...
**		Lecture de la directorie courante.
**	Input : 
**	Ouput :
*/
void InitBrowser ()
{
	Window 	root;
	int 	screen;
	unsigned long black, white;
	XSizeHints sizehints;
	XGCValues	 gcv;
	char pathname [MAXPATHLEN];
	char *visit_raise, *br_start;
	extern char  *getcwd(), *getenv ();

	screen = DefaultScreen ( dpy );
	root = RootWindow ( dpy, screen );
	black = BlackPixel ( dpy, DefaultScreen ( dpy ));
	white = WhitePixel ( dpy, DefaultScreen ( dpy ));

	br.fg = (DefaultDepth ( dpy, screen ) == 1) ? black : GetOpColor ( OP_MENU_FG );
	br.bg = (DefaultDepth ( dpy, screen ) == 1) ? white : GetOpColor ( OP_MENU_BG );

	br.text_bg = GetOpColor ( OP_TEXT_BG );
	br.text_fg = GetOpColor ( OP_TEXT_FG );
	br.ts = GetOpColor ( OP_MENU_TS );
	br.bs = GetOpColor ( OP_MENU_BS );
	br.font = GetOpFont ( OP_TEXT_FONT );

	br.methods_save = br.class_save = br.files_save = 0;
	br.dec_imp_flag = DEC_MODE;
	br.mode = CPLUS;
	br.title_name = 0;

	br.gc = XCreateGC ( dpy, DefaultRootWindow ( dpy ), 0,  &gcv );
 	XCopyGC ( dpy, DefaultGC (dpy, screen ), (~0), br.gc );

	XSetFont ( dpy, br.gc, br.font -> fid );
	XSetForeground ( dpy, br.gc, br.fg );
	XSetBackground ( dpy, br.gc, br.bg );

	br.title_height = br.font -> ascent + br.font-> descent + 5;
	br.button_height = br.font -> ascent + br.font-> descent + 5;
	br.width = (DisplayWidth ( dpy,DefaultScreen ( dpy )) * 5 )  / 11;
	br.height = (DisplayHeight ( dpy,DefaultScreen ( dpy )) * 5) / 8;

	br.frame = XCreateSimpleWindow (dpy, root,
		0, 0, br.width, br.height, 0, black, black );
	br.title = XCreateSimpleWindow (dpy, br.frame, 
		0, 0, br.width, br.title_height, 0 , black, br.bg );
	br.main = XCreateSimpleWindow (dpy, br.frame, 
		0, br.title_height, br.width, br.height - br.title_height, 0, black, br.bg );
	
	XSelectInput ( dpy, br.frame, ExposureMask | StructureNotifyMask 
		| KeyPressMask );
	XSelectInput ( dpy, br.title, ExposureMask );
	XSelectInput ( dpy, br.main, ExposureMask | VisibilityChangeMask  );
	
	sizehints.flags = PSize |  PMinSize | PMaxSize;
	sizehints.width = br.width;
	sizehints.height = br.height;
	sizehints.min_width = DisplayWidth ( dpy,DefaultScreen ( dpy )) / 3;
	sizehints.min_height = (DisplayHeight ( dpy,DefaultScreen ( dpy )) * 2) /  4;
	sizehints.max_width = (DisplayWidth ( dpy,DefaultScreen ( dpy )) * 3) / 4;
	sizehints.max_height = (DisplayHeight ( dpy,DefaultScreen ( dpy )) * 9) / 10;
	XSetStandardProperties ( dpy, br.frame, "Browser","Browser",
		None, 0, 0, &sizehints );

	wm_delwin = XInternAtom( dpy, "WM_DELETE_WINDOW", False);
	(void) XSetWMProtocols ( dpy, br.frame, &wm_delwin, 1);

	(void) BrowserButtons ();
	init_browser (); /* Le moteur */

	br_start = (char *) getenv ( "XCORAL_PARSE_FILES_AT_STARTUP" );
	if ( (br_start == 0) || strcmp("True", br_start ) == 0 ){
		(void) getcwd ( pathname, MAXPATHLEN + 2 );
      		br_start = (char *) getenv ( "XCORAL_PARSE_PATH" );
      		if ( br_start == 0 )
			(void) LoadDir ( pathname, NO_RECURSIVE );
      		else {
	      		ParseDir ( br_start );
		}
	}
	(void) BrowserTextWindows (); 

   	br.stat = B_UNMAP;
	
	visit_raise = (char *) getenv ( "XCORAL_VISIT_RAISE" );
	br.visit_raise = ( visit_raise == 0 || strcmp ( "True", visit_raise ) == 0 ) ? True : False;

	sizehints.min_height /= 3; 
	sizehints.min_width /= 3;
	XSetStandardProperties ( dpy, br.tbw[W_VISIT].frame, "Visit Window","Visit Window",
		None, 0, 0, &sizehints );
	br.old_click = 0;
	br.click_time = 0;
}


/*
**	Function name : ParseDir
**
**	Description : 
**	Input : 
**	Ouput :
*/
ParseDir (path)
register char *path;
{
	register char *p, *dirlist;

	dirlist  = (char *) malloc ( strlen (path + 2 ));
   	strcpy ( dirlist, path );
	p = dirlist;
   
	for (;;) {
		while (*p && *p != ':') ++p;
		if ((p - dirlist) == 0 ) {
   			/* vide */
   			printf ( "result = null\n" );
      			break;
		}
		else {
         
         			if ( *p == 0 ) {
				LoadDir ( dirlist, RECURSIVE );
         				break;
			} 
			else {
				*p = 0;
				LoadDir ( dirlist, RECURSIVE );
			}
		}
		p++;
		if  (*p == 0 )
			break;
		dirlist = p;      
	}
	(void) free ( p );
}

/*
**	Function name : SetBrowserDir
**
**	Description : Positionne la directorie courante.
**
**	Input : La nom de la directorie.
**	Ouput :
*/
void SetBrowserDir ( dirname )
	register char *dirname;
{
	(void) strcpy ( br.dir, dirname );	
}


/*
**	Function name : BrowserButtons
**
**	Description : Creation des bouttons :add, del, dec/imp, edit
**	Input : 
**	Ouput :
*/
static void BrowserButtons ()
{
	unsigned long black = black = BlackPixel ( dpy, DefaultScreen ( dpy ));

	br.dec = XCreateSimpleWindow (dpy, br.main, 0, 0,
		B_BUTTON_WIDTH, br.button_height, 0, black, br.bg );
	br.add = XCreateSimpleWindow (dpy, br.main, 0, 0,
		B_BUTTON_WIDTH,br.button_height, 0, black, br.bg );
	br.del = XCreateSimpleWindow (dpy, br.main, 0, 0,
		B_BUTTON_WIDTH,br.button_height, 0, black, br.bg );
	br.edit = XCreateSimpleWindow (dpy, br.main, 0, 0,
		B_BUTTON_WIDTH, br.button_height, 0, black, br.bg );
	
	XSelectInput ( dpy, br.dec, ExposureMask | ButtonPressMask | ButtonReleaseMask );
	XSelectInput ( dpy, br.add, ExposureMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput ( dpy, br.del, ExposureMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput ( dpy, br.edit, ExposureMask | ButtonPressMask | ButtonReleaseMask);	

}


/*
**	Function name : LoadDir
**
**	Description : Parse la directorie passee en argument.
**
**	Input : Le nom de la directorie.
**	Ouput :
*/
void LoadDir ( dir, flag )
register char *dir;
register int flag;
{
	DIR *dirp;
	struct dirent *dp;
    	struct stat st;
	register char *buf;

	dirp = opendir( dir );
	if ( dirp == 0 ) 
		return;
	for (dp = readdir(dirp); dp != 0; dp = readdir(dirp)) {
      		if ( strcmp ( dp -> d_name, "." ) == 0 
         			|| strcmp ( dp -> d_name, ".." ) == 0 
      			|| strcmp ( dp -> d_name, "SCCS" ) == 0
			|| strcmp ( dp -> d_name, "RCS" ) == 0 )
      			continue;
		if ( * dp -> d_name == '#' ) continue;
/*		if ( GoodSuffix ( dp->d_name ) == True ) { */
			if ( strcmp ( dir, "." ) == 0 ) {
				buf = dp->d_name;			
			}
			else {
				buf = (char *) malloc ( (unsigned) (strlen ( dir ) + strlen ( dp->d_name ) + 4) );
				(void) sprintf ( buf, "%s/%s", dir, dp->d_name );
			}
			(void) stat ( buf , &st );
			if ( (S_ISDIR(st.st_mode) != 0) && (flag == RECURSIVE) ) {
				LoadDir ( buf, RECURSIVE );
			}

   		if ( GoodSuffix ( dp->d_name ) == True ) { 
			if ( S_ISREG(st.st_mode) != 0 ) {
#ifdef DEBUG
	fprintf ( stderr, "file : %s\n", buf );
#endif
				parse_file ( buf );
			}
			if ( buf != 0 )
				(void) free ( buf );
		}
	}
	(void) closedir (dirp);
}

/*
**	Function name : BrowserTextWindows
**
**	Description : Creation des fenetres de texte.
**	Input : 
**	Ouput :
*/
static void BrowserTextWindows ()
{
	Window 	root, frame, title, main;
	int 	screen;
	unsigned long black;
	register int i;

	screen = DefaultScreen ( dpy );
	root = RootWindow ( dpy, screen );
	black = BlackPixel ( dpy, DefaultScreen ( dpy ));

	for ( i = 0; i < 7; i++ ) {	
		if ( i == W_VISIT ) {
			frame = XCreateSimpleWindow (dpy, root, 0, 0,
				500, 300, 0, black, br.bg ); 
			XSelectInput ( dpy, frame, 
				ExposureMask | StructureNotifyMask | KeyPressMask );
			wm_delwin = XInternAtom( dpy, "WM_DELETE_WINDOW", False);
			(void) XSetWMProtocols ( dpy, frame, &wm_delwin, 1);
		}
		else {
			frame = XCreateSimpleWindow (dpy, br.main, B_INTER,
				B_INTER, br.width, br.height, 0, black, br.bg ); 
			XSelectInput ( dpy, frame, ExposureMask | ButtonReleaseMask );
		}
	
		br.tbw[i].frame = frame; 
		title = XCreateSimpleWindow (dpy, frame, 0, 
			0,  br.width, br.height, 0, black, br.bg );
		br.tbw [i].title = title;

		main = XCreateSimpleWindow (dpy, frame, B_SPACE, 
			br.title_height + B_SPACE,  br.width, br.height, 0, black, br.bg );
		br.tbw [i].main = main;

		XSelectInput ( dpy, title, ExposureMask );
		XSelectInput ( dpy, main, ExposureMask ); 

		br.tbw [i].text = ( Text * ) MakeTextWindow ( dpy, main, 0, 0 );
		br.tbw [i].scroll = ( SWin  * ) MakeScroll ( dpy, main,  0, 0 );
		br.tbw [i].text -> swin = br.tbw [i].scroll;
		br.tbw [i].scroll -> text = (char *) br.tbw [i].text;
		br.tbw [i].text -> mwin = 0;

       		if ( i == W_VISIT ) {
			br.tbw [i].buf = (Buf *) GetBuffer ( (unsigned) (B_BUFFER_SIZE *3));
		}
       		else
       			br.tbw [i].buf = (Buf *) GetBuffer ( (unsigned) B_BUFFER_SIZE );

		(void) strcpy ( br.tbw [i].text -> filename, "NoName" ); 
        
		br.tbw [i].text -> buf = br.tbw [i].buf;
		br.tbw [i].select = 0;
	}
	/* 
	 * L'autre buffer pour les noms de fichiers.
	 */
	br.filebuf = (Buf *) GetBuffer ( (unsigned) B_BUFFER_SIZE );
	(void) SetInfos (); 
}


/*
**	Function name : SetInfos
**
**	Description : Chargement de la base dans les buffers
**	Input : 
**	Ouput :
*/
void SetInfos ()
{
	register char **tmp;

      	if ( br.class_save != 0 )
		(void) free ( (char *) br.class_save );
	br.class_save = ( char **)  get_classes_list ();
	ExtractInfos ( br.class_save, W_CLASS );
   	tmp = ( char **)  get_procs_list ();

   	ExtractInfos ( tmp, W_PROC ); 
   	if ( tmp != 0 )
		(void) free ( (char *) tmp );
   	if ( br.files_save != 0 )
		(void) free ( (char *) br.files_save );
	br.files_save = (char **)  get_files_list ();
	ExtractInfos ( br.files_save, W_FILES );
}


/*
**	Function name : ExtractInfos
**
**	Description : Recherche des infos concernant le type
**		passe en argument.
**		Les infos sont chargees dans le buffer text correspondant.
**
**	Input : La base, le type.
**	Ouput :
*/
void ExtractInfos ( names, type )
register char **names;
register int type;
{
	register int n;
	register char *infos_name;
	BWin *bwin = &br.tbw[type];

	if ( names != (char **) Null )  {
		ClearBuffer ( bwin -> buf ); 
		while ( *names != (char *) Null ) {
			switch ( type ) {
			case W_CLASS:
			case W_PARENT: 
			case W_CHILD:
				infos_name = GET_CLASS_INFO(*names);
				break;
			case W_METHOD:
				infos_name = GET_METHOD_INFO(*names);
				break;
			case W_PROC:
				infos_name = GET_PROCEDURE_INFO(*names);
				infos_name = *names;
				break;
			case W_FILES:
				infos_name = *names;
				break;	
			}
			InsertNchar ( bwin -> buf, infos_name, strlen ( infos_name ));
			InsertNchar ( bwin -> buf, "\n", 1 );
			names ++;
		}
		if ( type == W_FILES )
			SetFileBuf ( bwin -> buf );

		bwin -> text -> modif = True;
		(void) strcpy ( (char *) bwin -> text -> filename, "NoName" );
		n = GetNumberOfLineInBuf ( bwin -> text -> buf );
		bwin -> text -> lines_in_buf = n;
		SetScrollLine ( bwin -> text -> swin, n );
		HoleToLeft ( bwin -> buf ); 
	}
}

/*
**	Function name : SetFileBuf
**
**	Description : Mise a jour et tri des nom de fichiers dans la
**		fenetre texte associee. 
**	Input : Le buffer contenant les noms des fichiers.
**	Ouput :
*/
static void SetFileBuf ( buf )
	Buf *buf;
{
	register char **tmp, *p;
	register int nbfiles, i;
	int len;
	int PathCompare ();

	HoleToLeft ( buf ); /* On se place au debut */
	/* 
	 * Ce buffer contient les pathnames complets et tries des fichiers.
	 * Dans la fenetre, seuls les noms des fichiers doivent apparaitre.
	 * Il faut donc 2 buffers. ( br.filebuf contiendra la totalite des
	 * informations ).
	 */
	nbfiles = GetNumberOfLineInBuf ( buf );
	nbfiles--;

      	/* Copie des paths dans tmp */
	tmp = (char **) malloc ( (unsigned) (sizeof (char *) *(nbfiles + 2)));
	for ( i=0; i<nbfiles;i++ ) {
		p = (char *) GetCurrentLine ( buf, &len );
		tmp [i] = (char *) malloc ( (unsigned) (len + 1) );
		(void) strncpy ( (char *) tmp[i], p, len );
		tmp[i][len] = '\0';
		(void) MoveToLine ( buf, 1 );
	}


	/* Tri */
	if ( nbfiles )
		qsort ( (char *) tmp, nbfiles-1, sizeof ( char *), PathCompare );

   	/* Recopie dans les buffers */
	ClearBuffer ( buf );
	ClearBuffer ( br.filebuf );
	for ( i=0; i<nbfiles;i++ ) {
		if ( tmp[i] == 0 ) 
			(void) fprintf ( stderr, "Internal Error 4...\n" );
        		InsertNchar ( br.filebuf, (char *) tmp [i], 
            			strlen ((char *) tmp [i] ));
		InsertNchar ( br.filebuf, "\n", 1 );
		p = strrchr ((char *) tmp [i], '/' );
		if ( p == 0 )
			(void) fprintf ( stderr, "Internal Error 5...\n" );
		p++;
        		InsertNchar ( buf, p, strlen ( p ));
		InsertNchar ( buf, "\n", 1);
      		if ( tmp[i] != 0 )
			(void) free ( (char *) tmp[i] );	
	}
   	if ( tmp != 0 )
		(void) free ( (char *) tmp );
}

/*
**	Function name : PathCompare
**
**	Description : La fonction de comparaison pour les
**		pathnames.
**
**	Input : 
**	Ouput :
*/
static int PathCompare ( s1, s2 )
register char **s1, **s2;
{
	return ( strcmp ( strrchr (*s1,'/'), strrchr (*s2, '/')));
}



/*
**	Function name : ConfigBrowser
**
**	Description : Positionne la geometrie du browser, au premier
**		affichage ou lorsqu'il y a un resize.
**
**	Input : Largeur, hauteur.
**	Ouput :
*/
void ConfigBrowser ( width, height )
	register int width, height;
{
#define GetBWheight(i) 	br.tbw[i].f_height
#define GetBWwidth(i)	br.tbw[i].f_width
#define GetBWx(i)	br.tbw[i].x
#define GetBWy(i)	br.tbw[i].y
#define GetBWmwidth(i)	br.tbw[i].m_width;
#define GetBWmheight(i)	br.tbw[i].m_height;
#define GetBwidth	br.width;
#define GetBheight	br.height;

	register int y;
#ifdef DEBUG
	(void) fprintf ( stderr, "Config Browser\n" );
#endif
	br.width = width;
	br.height = height;

	XMoveResizeWindow ( dpy, br.title, 0, 0, br.width, br.title_height );
	XResizeWindow ( dpy, br.main, br. width, br.height - br.title_height );

	ConfigFiles (); 
	switch ( br.mode ) {
	case CPLUS:
		ConfigClass ();
		ConfigParent ();
		ConfigChild ();
		ConfigMethod ();
		ConfigProc ();
		break;
	case STD_C:
		ConfigProc ();
		break;
	case LATEX:
		break;
	default:
		break;
	}

	/* Les bouttons */
	y = GetBWheight(W_FILES) + ( 2 * B_INTER);

	XMoveWindow ( dpy, br.add, (br.width/5) - (B_BUTTON_WIDTH/2) , y );
	XMoveWindow ( dpy, br.del, ((br.width*2)/5) - (B_BUTTON_WIDTH/2) , y );
	if ( br.mode != STD_C )
		XMoveWindow ( dpy, br.dec, ((br.width*3)/5) - (B_BUTTON_WIDTH/2) , y );
	XMoveWindow ( dpy, br.edit, ((br.width*4)/5) - (B_BUTTON_WIDTH/2) , y );
}


/*
**	Function name : ConfigVisitWindow
**
**	Description : Positionne la geometrie la de fenetre de visit,
**		au premier affichage ou lorsqu'il y a un resize.
**		
**	Input : Largeur, hauteur.
**	Ouput :
*/
void ConfigVisitWindow ( width, height )
	register int width, height;
{
#ifdef DEBUG
	(void) fprintf ( stderr, "Config Visit Window\n" );
#endif

	if ( (br.tbw[W_VISIT].f_width == width) && (br.tbw[W_VISIT].f_height == height) )
		return;
	br.tbw[W_VISIT].f_width = width;
	br.tbw[W_VISIT].f_height = height;
	br.tbw[W_VISIT].m_width = width - (2*B_SPACE);
	br.tbw[W_VISIT].m_height = height - br.title_height - (2*B_SPACE) ;

	XMoveResizeWindow ( dpy, br.tbw[W_VISIT].title, 0, 0, width, height );
	XResizeWindow ( dpy, br.tbw[W_VISIT].main, width,  height - br.title_height );
	XResizeWindow ( dpy, br.tbw[W_VISIT].title, width, br.title_height ); 

	ConfigScroll ( &br.tbw[W_VISIT] ); 
}


/*
**	Function name : ConfigClass
**
**	Description : Postionne la geometrie de la fenetre class
**	Input : Le contexte
**	Ouput :
*/
static void ConfigClass ()
{
	BWin *bwin = &br.tbw[W_CLASS];
   
	bwin -> x = B_INTER;
	bwin -> y = B_INTER;
	bwin -> f_width = (br.width - ( 4*B_INTER )) / 3;
	bwin -> f_height = (br.height - ( 4*B_INTER ) - (2*br.title_height)) / 2;

	XResizeWindow ( dpy, bwin -> frame, bwin -> f_width, bwin -> f_height );
	XResizeWindow ( dpy, bwin -> title, bwin -> f_width, br.title_height ); 

	bwin -> m_width = bwin -> f_width - (2*B_SPACE);
	bwin -> m_height = bwin -> f_height - br.title_height - (2*B_SPACE);
	XResizeWindow ( dpy, bwin -> main, bwin -> m_width, bwin -> m_height );

	ConfigScroll ( &br.tbw[W_CLASS] ); 
}


/*
**	Function name : ConfigParent
**
**	Description : Postionne la geometrie de la fenetre parent.
**	Input : Le contexte
**	Ouput :
*/
static void ConfigParent ()
{
	BWin *bwin = &br.tbw[W_PARENT];
      
	bwin -> x = GetBWwidth(W_CLASS) + ( 2 * B_INTER );
	bwin -> y = B_INTER;
	bwin -> f_width = GetBWwidth(W_CLASS);
	bwin -> f_height = ( GetBWheight(W_CLASS) - B_INTER ) / 2;

	XMoveResizeWindow ( dpy, bwin -> frame,
		 bwin -> x, bwin -> y,
		bwin -> f_width, bwin -> f_height );

	XResizeWindow ( dpy, bwin -> title, bwin -> f_width, br.title_height ); 

	bwin -> m_width = GetBWmwidth(W_CLASS);
	bwin -> m_height = bwin -> f_height - br.title_height - (2*B_SPACE);
	XResizeWindow ( dpy, bwin -> main, bwin -> m_width, bwin -> m_height );

	ConfigScroll ( &br.tbw[W_PARENT] );
}


/*
**	Function name : ConfigChild
**
**	Description : Postionne la geometrie de la fenetre child.
**	Input : Le contexte.
**	Ouput :
*/
static void ConfigChild ()
{
	BWin *bwin =  &br.tbw[W_CHILD];

	bwin -> x = GetBWwidth(W_CLASS) + ( 2 * B_INTER );
	bwin -> y = GetBWheight(W_PARENT) + ( 2 * B_INTER);
	bwin -> f_width = GetBWwidth(W_PARENT);
	bwin -> f_height = GetBWheight(W_PARENT);

	XMoveResizeWindow ( dpy, bwin -> frame,
		 bwin -> x, bwin -> y,
		bwin -> f_width, bwin -> f_height );

	XResizeWindow ( dpy, bwin -> title, bwin -> f_width, br.title_height ); 

	bwin -> m_width = GetBWmwidth(W_CLASS);
	bwin -> m_height = bwin -> f_height - br.title_height - (2*B_SPACE);
	XResizeWindow ( dpy, bwin -> main, bwin -> m_width, bwin -> m_height );

	ConfigScroll ( &br.tbw[W_CHILD] );
}


/*
**	Function name : ConfigFiles
**
**	Description : Postionne la geometrie de la fenetre des
**		nom de fichiers.
**	Input : Le contexte.
**	Ouput :
*/
static void ConfigFiles ()
{
	BWin *bwin = &br.tbw[W_FILES];

   	switch ( br.mode ) {
	case CPLUS:
		bwin -> f_width = (br.width - ( 4*B_INTER )) / 3;
		bwin -> f_height = (br.height - ( 4*B_INTER ) - (2*br.title_height)) / 2;
		bwin -> x = ( 3 * B_INTER ) + ( (br.width - ( 4*B_INTER )) *2/ 3);
		bwin -> y = B_INTER;
		break;
	case STD_C:
		bwin -> f_width = (br.width - ( 3*B_INTER )) / 2;
		bwin -> f_height = (br.height - ( 3*B_INTER ) - br.title_height - br.button_height );
		bwin -> x = B_INTER;
		bwin -> y = B_INTER;
		break;
	}

	XMoveResizeWindow ( dpy, bwin -> frame, 
		 bwin -> x, bwin -> y,
		bwin -> f_width, bwin -> f_height );

	XResizeWindow ( dpy, bwin -> title, bwin -> f_width, br.title_height ); 

	bwin -> m_width = bwin -> f_width - ( 2 * B_SPACE );
	bwin -> m_height = bwin -> f_height - br.title_height - (2*B_SPACE);
	XResizeWindow ( dpy, bwin -> main, bwin -> m_width, bwin -> m_height );

	ConfigScroll ( &br.tbw[W_FILES] );
}



/*
**	Function name : ConfigMethod
**
**	Description : Positionne la geometrie la fenetre des methodes.
**	Input : Le contexte
**	Ouput :
*/
static void ConfigMethod ()
{
	BWin *bwin = &br.tbw[W_METHOD];
#ifdef DEBUG
	(void) fprintf ( stderr, "Config Methods\n" );
#endif
	bwin -> x = B_INTER;
	bwin -> y = GetBWy(W_CLASS) + GetBWheight(W_CLASS)
		+ br.button_height + ( 2 * B_INTER );
	bwin -> f_width = ( br.width - (3 * B_INTER)) / 2;
	bwin -> f_height = GetBWheight(W_CLASS);

	XMoveResizeWindow ( dpy, bwin -> frame, 
		 bwin -> x, bwin -> y,
		bwin -> f_width, bwin -> f_height );

	XResizeWindow ( dpy, bwin -> title, bwin -> f_width, br.title_height ); 

	bwin -> m_width = bwin -> f_width - ( 2 * B_SPACE );
	bwin -> m_height = bwin -> f_height - br.title_height - (2*B_SPACE);
	XResizeWindow ( dpy, bwin -> main, bwin -> m_width, bwin -> m_height );

	ConfigScroll ( &br.tbw[W_METHOD] );
}


/*
**	Function name : ConfigProc
**
**	Description : Postionne la geopmetrie de la fenetre
**		des fonctions.
**	Input : Le contexte.
**	Ouput :
*/
static void ConfigProc ()
{
	BWin *bwin = &br.tbw[W_PROC];

	switch ( br.mode ) {
	case CPLUS:
		bwin -> x = (2 *B_INTER) + GetBWwidth(W_METHOD);
		bwin -> y = GetBWy(W_CLASS) + GetBWheight(W_CLASS)
			+ br.button_height + ( 2 * B_INTER );
		bwin -> f_width = GetBWwidth(W_METHOD);
		bwin -> f_height = GetBWheight(W_METHOD);
		bwin -> m_width = GetBWmwidth(W_METHOD);
		break;
	case STD_C:
		bwin -> x = (2 *B_INTER) + GetBWwidth(W_FILES);
		bwin -> y = B_INTER;
		bwin -> f_width = GetBWwidth(W_FILES);
		bwin -> f_height = GetBWheight(W_FILES);
		bwin -> m_width = GetBWmwidth(W_FILES);
		break;
	default:
		break;
	}

	XMoveResizeWindow ( dpy, bwin -> frame,
		 bwin -> x, bwin -> y,
		bwin -> f_width, bwin -> f_height );

	XResizeWindow ( dpy, bwin -> title, bwin -> f_width, br.title_height ); 

	bwin -> m_height = bwin -> f_height - br.title_height - (2*B_SPACE);
	XResizeWindow ( dpy, bwin -> main, bwin -> m_width, bwin -> m_height );

	ConfigScroll ( &br.tbw[W_PROC] );
}


/*
**	Function name : Config Scroll
**
**	Description : Positionne la geometrie du scroll
**		pour une fenetre de texte.
**
**	Input : Le contexte.
**	Ouput :
*/
void ConfigScroll ( bw )
	BWin *bw;
{
	register int x, i;

	x = bw -> m_height; 
	x -= ( 2 * MARGE );

	i = x / ( bw -> text) -> font_height;

	SetScrollLinePage (  bw -> scroll, i ); 
	ShowWindowText ( dpy, bw -> text, 
		bw -> m_width - GetScrollWidth () - 1, 
		bw -> m_height );
	i = bw -> text -> no_current_line - bw -> text -> n1 - 1;
	RefreshScroll ( dpy,  bw -> scroll,  bw -> m_width, bw -> m_height, i );
}
	

/*
**	Function name : DisplayBrowser
**
**	Description : Affichage du browser.
**	Input : 
**	Ouput :
*/
void DisplayBrowser()
{
	register int i;

	if ( br.stat == B_MAP ) {
      		XMapRaised ( dpy, br.frame );
	                return;
	}
	if ( br.mode == TEXT )
		br.mode = CPLUS;

	switch ( br.mode ) {
		case CPLUS:
			i = 0;
			XMapWindow ( dpy, br.dec );
			break;
		case STD_C:
			i = C_FIRST_WIN;
			break;
/*
		case LATEX:
			i = 5;
			break;
*/
		default:
			i = 0;
			break;
	}

	for ( i;i < B_NB_WIN; i++ ) { 
		XMapSubwindows ( dpy, br.tbw[i].main ); 
		XMapSubwindows ( dpy, br.tbw[i].frame ); 
		XMapWindow ( dpy, br.tbw[i].main ); 
		XMapWindow ( dpy, br.tbw[i].frame ); 
      		/*
      		 * Plus tard les explications de ce 'FirstPage'
      		 */
      		if ( (br.tbw[i].text -> lines_in_buf > 1) &&(already_map == True)) {
   	      		FirstPage ( br.tbw[i].text );
		}	
	XMapWindow ( dpy, br.main );
	XMapWindow ( dpy, br.title );
	XMapWindow ( dpy, br.add );
	XMapWindow ( dpy, br.del );
	XMapWindow ( dpy, br.edit );
	XMapRaised ( dpy, br.frame );
	}

	br.stat = B_MAP;
	if ( already_map == False)
		already_map = True;	
}


/*
**	Function name : UnmapBrowser
**
**	Description : Cache la fenetre de controle et de visit.
**	Input : 
**	Ouput :
*/
void UnmapBrowser ()
{
	if ( br.stat == B_UNMAP )
		return;

	XUnmapSubwindows ( dpy, br.main );
	XUnmapWindow ( dpy, br.main );
	XUnmapSubwindows ( dpy, br.frame );
	XUnmapWindow ( dpy, br.frame );

	XUnmapWindow ( dpy, br.tbw[W_VISIT].frame ); 
	br.stat = B_UNMAP;
}


/*
**	Function name : SetBrowserMode
**
**	Description : Positionne le mode courant.
**
**	Input : Le mode.
**	Ouput :
*/
void SetBrowserMode ( mode )
register int mode;
{
	if ( br.mode == mode )
		return;
	else
		br.mode = mode;
}


/*
**	Function name : RefreshBrowserControl
**
**	Description : Expose la fenetre de controle
**	Input : 
**	Ouput :
*/
void RefreshBrowserControl ()
{
	if ( br.stat == B_UNMAP )
		return;
	else {
		br.stat = B_UNMAP;
		XUnmapSubwindows ( dpy, br.main );
		DisplayBrowser ();
		ConfigBrowser ( br.width, br.height );
	}
}
