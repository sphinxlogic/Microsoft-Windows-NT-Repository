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
#include <X11/cursorfont.h>
#ifdef apollo
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "options.h"
#include "browser.h"
#include "xcoral.h"
#include "flist.h"

extern Browser br;
extern char *b_name[];

static void 	RefreshBrowserWindow (), ManageFiles (), ButtonEdit (),
	SelectItem (), ClassSelect (), UpdateTitle (), ProcSelect (),
	FileSelect (), MethodSelect (), UpdateItem (), ParentChildSelect (), 
	EditFile (), GoodPage (), TitleVisit (), AddFiles (), RemoveFiles (),
	ButtonInVisitWindow ();

/*
**	Function name : ExposeBrowser
**
**	Description : Traitement d'un 'expose event'
**	Input : L'event.
**	Ouput :
*/
void ExposeBrowser ( ev )
XEvent *ev;
{
	register int i;
	register int width;

	if ( ev -> xexpose.window == br.title ) {
		if ( br.title_name != 0 )
			UpdateTitle ( br.title_name );
		else 
			Display3D ( dpy, br.title, br.ts, br.bs, 1, 0 );	
		return;
	}

	if ( ev -> xexpose.window == br.main ) {
		Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 );
		return;
	}
	if ( ev -> xexpose.window == br.dec ) {
		XClearWindow ( dpy, br.dec );
		Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 );
		if ( br.dec_imp_flag == DEC_MODE )
			XDrawString ( dpy, ev -> xexpose.window, br.gc,
         				( B_BUTTON_WIDTH - XTextWidth ( br.font, "Decl", 4 )) / 2,
   				br.font -> ascent + 3, "Decl", 4 );
		else 
			XDrawString ( dpy, ev -> xexpose.window, br.gc, 
   				( B_BUTTON_WIDTH - XTextWidth ( br.font, "Impl", 4 )) / 2,
				br.font -> ascent + 3, "Impl", 4 );
		return;
	}
	if ( ev -> xexpose.window == br.add ) {
		Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 ); 
		XDrawString ( dpy, ev -> xexpose.window, br.gc, 
			( B_BUTTON_WIDTH - XTextWidth ( br.font, "Add", 3 )) / 2,
   			br.font -> ascent + 3, "Add", 3 );
		return;
	}
	if ( ev -> xexpose.window == br.del ) {
		Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 );
		XDrawString ( dpy, ev -> xexpose.window, br.gc, 
   			( B_BUTTON_WIDTH - XTextWidth ( br.font, "Del", 3 )) / 2,
			br.font -> ascent + 3, "Del", 3 );
		return;
	}
	if ( ev -> xexpose.window == br.edit ) {
		Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 ); 
		XDrawString ( dpy, ev -> xexpose.window, br.gc,
			( B_BUTTON_WIDTH - XTextWidth ( br.font, "Edit", 4 )) / 2,
			br.font -> ascent + 3, "Edit", 4 );
		return;
	}

	for ( i = 0; i < 7; i++ ) {
		if ( ev -> xexpose.window == br.tbw[i].frame ) {
			Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 ); 
			break;
		} 
		if ( ev -> xexpose.window == br.tbw[i].title) {
			Display3D ( dpy, ev -> xexpose.window, br.ts, br.bs, 1, 0 );
            			if ( i == W_VISIT ) {
				TitleVisit ( GetFileName ( br.tbw[W_VISIT].text )); 
			}
            			else {
               				width = XTextWidth ( br.font, b_name [i], strlen (b_name [i]));
				XDrawString ( dpy, ev -> xexpose.window, br.gc, 
					(br.tbw[i].f_width - width)/2, br.font -> ascent + 3,            
					b_name [i], strlen (b_name [i]));                                                            
			}                
			break;
		} 
		if ( ev -> xexpose.window == br.tbw[i].text -> window ) {
			RefreshBrowserWindow ( &br.tbw[i] ) ;
			if ( i == W_VISIT ) {
				TextCursorOn ( br.tbw[W_VISIT].text );
			}
			break;
		} 
		if ( ev -> xexpose.window == br.tbw[i].scroll -> frame ) {
			RefreshScrollFrame ( dpy,  br.tbw[i].scroll );
			break;
		} 
		if ( ev -> xexpose.window == br.tbw[i].scroll -> scroll ) {
			RefreshScrollBar ( dpy, br.tbw[i].scroll );
			break;
		} 
	}
	while ( XCheckWindowEvent ( dpy, ev -> xexpose.window, ExposureMask, ev ));
}	


/*
**	Function name : SetBrowserVisibility
**
**	Description : Positionne l'etat de visibilite de la
**		fenetre de controle.
**
**	Input : L'event.
**	Ouput :
*/
int SetBrowserVisibility ( ev )
XEvent *ev;
{
	if ( ev -> xvisibility.window == br.main ) {
		br.visible = ev -> xvisibility.state;
		return True;
	}
	return False;
}


/*
**	Function name : RefreshBrowserWindow
**
**	Description : Comme son nom l'indique.
**	Input : Le contexte
**	Ouput :
*/
static void RefreshBrowserWindow ( bwin )
BWin *bwin;
{
	XClearWindow ( dpy, bwin -> text -> window );
	Display3D ( dpy, bwin -> text -> window, br.ts, br.bs, 2, 1 ); 
	ClipOn ( bwin -> text, 0 );
	SetLinesTable ( bwin -> text );
	RefreshPage ( bwin -> text );
	ClipOff ( bwin -> text );
	UpdateItem ( bwin, bwin -> select );
	XFlush ( dpy );
}


/*
**	Function name : ButtonBrowser
**
**	Description : Traitement d'un 'button press event'
**	Input : L'event.
**	Ouput : Vrai si le button press concerne le browser.
*/
int ButtonBrowser ( ev )
XButtonEvent *ev;
{
	Window w = ev -> window;
	register int i;
	int result;
	
	br.click_time = ev -> time;
	if ( BrDoubleClick () == True ) {
/*		
		XGrabPointer ( dpy, w, True, ButtonReleaseMask, GrabModeAsync,
			GrabModeAsync, w, None, CurrentTime );
		WaitButtonRelease ( ev -> button );
*/
		ButtonEdit ();
		return True;
	}
	if ( (w == br.dec) ||  (w == br.add) || ( w == br.del) || ( w == br.edit ) ) {
		Display3D ( dpy, w, br.ts, br.bs, 1, 1 );
		XGrabPointer ( dpy, w, True, ButtonReleaseMask, GrabModeAsync,
			GrabModeAsync, w, None, CurrentTime );
		WaitButtonRelease ( ev -> button );
        		Display3D ( dpy, w, br.ts, br.bs, 1, 0 );
		if ( w == br.dec ) {
			br.dec_imp_flag = ! br.dec_imp_flag;
			XClearWindow ( dpy, w );
			if ( br.dec_imp_flag == DEC_MODE )
				XDrawString ( dpy, w,br.gc, 
            					(B_BUTTON_WIDTH - XTextWidth ( br.font, "Decl", 4 )) / 2,
      					br.font -> ascent + 3, "Decl", 4 );
			else 
				XDrawString ( dpy, w, br.gc, 
            					(B_BUTTON_WIDTH - XTextWidth ( br.font, "Impl", 4 )) / 2,
					br.font -> ascent + 3, "Impl", 4 );
	        		Display3D ( dpy, w, br.ts, br.bs, 1, 0 );
			if ( br.tbw[W_METHOD].select != 0 )
				MethodSelect ( &br.tbw[W_METHOD] );
		}
		else if ( w == br.edit )
			ButtonEdit ();
		else if ( w == br.add )
			ManageFiles ( B_ADD );
		else if ( w == br.del )
			ManageFiles ( B_DEL );
		return True;
	}
	for ( i = 0; i< 7; i++ ) {
		if ( w == br.tbw[i].text -> window ) {
			if ( i != W_VISIT ) {
				SelectItem ( &br.tbw[i], ev -> x, ev -> y, i );
			}
			else
				ButtonInVisitWindow ( &br.tbw[W_VISIT], ev );
#ifdef DEBUG
			(void) fprintf ( stderr, "Button BrowserText\n" );
#endif
			
			return True;
		}
		if ( ButtonPressInScroll ( br.tbw[i].scroll, ev -> window, ev -> y, &result )) {
			switch ( result ) {
			case CURSOR:
				UpdateItem ( &br.tbw[i], br.tbw[i].select );

				TextCursorOn ( br.tbw[i].text ); 
				TextCursorOff ( br.tbw[i].text );
#ifdef DEBUG
				fprintf ( stderr, "Button BrowserScroll\n" );
#endif
				if ( (TextInBuf ( br.tbw[i].text ) == True) 
					&& ( br.tbw[i].text -> lines_in_buf > 1 )) {
					HandleScrollBar ( dpy, br.tbw[i].scroll,  ScrollNLine );
					RefreshScrollBar ( dpy, br.tbw[i].scroll ); 
				
				}
				if ( i != W_VISIT ) 
					UpdateItem ( &br.tbw[i], br.tbw[i].select );
				else 
					TextCursorOn ( br.tbw[i].text );
				break;
			case NEXT:
				UpdateItem ( &br.tbw[i], br.tbw[i].select );
				TextCursorOff ( br.tbw[i].text );
				NextPage ( br.tbw[i].text );
				TextCursorOff ( br.tbw[i].text );
				UpdateItem ( &br.tbw[i], br.tbw[i].select );
				break;
			case PREVIOUS:
				UpdateItem ( &br.tbw[i], br.tbw[i].select );
				TextCursorOff ( br.tbw[i].text );
				PreviousPage ( br.tbw[i].text );
				TextCursorOff ( br.tbw[i].text );
				UpdateItem ( &br.tbw[i], br.tbw[i].select );
				break;
			}
			return True;
		}
	}
	return False;
}

/*
**	Function name : BrDoubleClick
**
**	Description : 
**	Input : 
**	Ouput :
*/
int BrDoubleClick ()
{
	if ( br.old_click == 0 ) 
		return False;

	if ( (br.click_time - br.old_click) < 300)
		return True;
	else
		return False;
}

/*
**	Function name : ManageFiles
**
**	Description : Modification de de la base.
**		Ajoute un fichier/directorie ou
**		retire un fichier.
**
**	Input : Le type d'operation a effectuer.
**	Ouput :
*/
static void ManageFiles ( op )
	register int op;
{
   	int len;

	if ( br.visible != VisibilityUnobscured ) {
		XRaiseWindow ( dpy, br.frame );
		return;
	}

   	if ( op == B_DEL ) {
      		register char *del_filename, *tmp, *buf;
		if ( br.tbw[W_FILES].select != 0 )  {
			HoleToLeft ( br.filebuf );
			if ( (br.tbw[W_FILES].select - 1) != 0)
				(void) MoveToLine ( br.filebuf, br.tbw[W_FILES].select - 1 );
   			tmp = GetCurrentLine ( br.filebuf, &len  );
         			*(tmp + len ) = '\0'; /* On vire le newline */
         			del_filename =  (char *) malloc ( (unsigned) len + 2 );
         			(void) strcpy ( del_filename, tmp );
			buf = malloc ( (unsigned) len + 32 );
			(void) sprintf ( buf, "Remove file selected [y/n/all] : "  );

        			tmp = (char *) GetStringFromDialogBox ( br.frame, buf );
			if ( (tmp == 0) || (strncmp(tmp, "y", 1) == 0 )) 
				RemoveFiles ( del_filename );
			else if ( strcmp ( tmp, "all" ) == 0 ) 
				RemoveFiles ( (char *) 0 );
			if ( del_filename != 0 )
	         			(void) free ( del_filename );
         			if ( buf != 0 )
	  			(void) free ( buf );
		}
		else {
			tmp = (char *) GetStringFromDialogBox ( br.frame, 
				"Delete all files [y/n] : " );
			if ( (tmp == 0) || (strncmp(tmp, "y", 1) == 0 )
				|| (strncmp(tmp, "all", 3)) == 0 )
				RemoveFiles ( (char *) 0 );
		}
 	}
	else 
		AddFiles ();
}


/*
**	Function name : AddFiles
**
**	Description : Ajoute un fichier ou une directorie.
**	Input : 
**	Ouput :
*/
static void AddFiles ()
{
	register char *s;
   	struct stat st;

	(void) chdir ( br.dir );

	s = (char *) GetStringFromSelect ( br.frame, FILESELECT ); 
	if ( (s != 0) && (strlen (s) != 0 )) {
		(void) stat ( s, &st );
		if ( S_ISDIR(st.st_mode) != 0 ) {
		   	XDefineCursor ( dpy, br.frame, 
				XCreateFontCursor ( dpy, XC_watch ) );
         			XFlush (dpy ); 
#ifdef DEBUG
			(void) fprintf ( stderr, "dir : %s\n", s );
#endif
	         		LoadDir ( s, RECURSIVE );
			XUndefineCursor ( dpy, br.frame );
			SetBrowserDir ( s );
		}
		else if ( S_ISREG(st.st_mode) != 0 ) {
			if ( GoodSuffix ( s ) == True )
				parse_file ( s ); 
		}
		RefreshBrowserInfos ();
	}
   	if ( s != 0 )
		(void) free (s);
}



/*
**	Function name : RemoveFiles
**
**	Description : Retire un ficher ou tous les fichiers
**		de la base.
**	Input : Le nom du fichiers.
**	Ouput :
*/
static void RemoveFiles ( name )
register char *name;
{
	register char *tmp, *p;
	register int i, nbfiles;
	int len;

	if ( name != 0 )
		delete_file ( name );
	else {
		HoleToLeft ( br.filebuf ); 			/* Debut du buffer */
		nbfiles = GetNumberOfLineInBuf ( br.filebuf ); 	/* Nb fichiers */
		nbfiles--;
	
		for ( i=0; i < nbfiles; i++ ) {
			p = (char *) GetCurrentLine ( br.filebuf, &len );
			tmp = (char *) malloc ( (unsigned) (len + 1) );
			(void) strncpy ( (char *) tmp, p, len );
			tmp [len] = '\0';
			(void) MoveToLine ( br.filebuf, 1 );
			delete_file ( tmp );
         			if ( tmp != 0 )
				(void) free ( tmp );
		}
	}

	/* Refresh windows */
	RefreshBrowserInfos ();
	if ( name == 0) { /* Clear Browser */
		KillText ( dpy, br.tbw[W_VISIT].text );
		ConfigScroll ( &br.tbw[W_VISIT] );
		TitleVisit ( " " );
		UpdateTitle ( " " );
	}
	else { /* Un fichier seulement */
		if  ( strcmp ( br.title_name, name ) == 0 ) {
			KillText ( dpy, br.tbw[W_VISIT].text );	
			ConfigScroll ( &br.tbw[W_VISIT] );
			TitleVisit ( " " );
		}
	}
}


/*
**	Function name : GoodSuffix
**
**	Description : Check filename.
**
**	Input : Le nom du fichier.
**	Ouput : Vrai si .c .h .cc .cxx .C. Faux sinon
*/
int GoodSuffix ( s )
	register char *s;
{
	register char *end_name;

	end_name = (char * ) strrchr ( (char *) s, '.' );
	if ( end_name == 0 )
	        return False;
       
	if ( (strcmp( end_name, ".c") == 0) 
		|| ( strcmp( end_name, ".h") == 0 )
		|| ( strcmp( end_name, ".cc") == 0 )
		|| ( strcmp( end_name, ".cxx") == 0 )
      		|| ( strcmp( end_name, ".hxx") == 0 )
		|| ( strcmp( end_name, ".C") == 0 )) 
		return True;
	else
		return False;
}


/*
**	Function name : RefreshBrowserInfos
**
**	Description : Prepare la mise a jour des infos.
**	Input : 
**	Ouput :
*/
void RefreshBrowserInfos ()
{
	register int i;
	XEvent event;

	SetInfos (); 	/* Classes, Procs, Files */
	br.tbw[W_CLASS].select = br.tbw[W_PROC].select = br.tbw[W_FILES].select = 0;
	br.tbw[W_PARENT].select = br.tbw[W_CHILD].select = br.tbw[W_METHOD].select = 0;

	if ( br.mode == STD_C ) 
		i = 4;
	else if ( br.mode == CPLUS )
		i = 0;
	else
		return;

   	if ( br.stat == B_UNMAP )
      		return;
 
	for ( i; i < 6; i++ ) {
		while ( XCheckWindowEvent ( dpy,
    			br.tbw[i].text -> window , ExposureMask, &event ));
		switch ( i ) {
		case W_FILES:
		case W_PROC:
		case W_CLASS:
			RefreshBrowserWindow ( &br.tbw[i] );
			FirstPage ( br.tbw[i].text );
			break;
		case W_PARENT:
		case W_CHILD:
		case W_METHOD:
			KillText ( dpy, br.tbw[i].text ); 
			break;
		default:
			break;
		}
		ConfigScroll ( &br.tbw[i] );			
	}
}



/*
**	Function name : ButtonEdit
**
**	Description : Creation d'une fenetre d'edition pour
**		l'objet courant.
**	Input : 
**	Ouput :
*/
static void ButtonEdit ()
{
	EdWin 	*ew;
	register char *filename;
   	extern EdWin *CreateWindow ();
	Text *text;
   
	filename = GetFileName ( br.tbw[W_VISIT].text ); 
   	if ( IsAlreadyLoad ( filename,  &text ) ) {
    		XMapRaised ( dpy, text -> w_parent );      
            		GotoLineNumber ( text, br.tbw[W_VISIT].text -> no_current_line );
		CurrentLineToTop ( text );      
      		return;
	}
   	
	ew = (EdWin * ) CreateWindow ();
	if ( ew == 0 ) {
		( void ) fprintf ( stderr, "Create_env error\n" );
		return;
	}
	switch ( br.mode ) {
		case CPLUS:
			ew -> text -> mode = CPLUS;
			ew -> text -> mwin -> mode = CPLUS;
			SetBrowserMode ( CPLUS );
			break;
		case STD_C:
			ew -> text -> mode = STD_C;
			ew -> text -> mwin -> mode = STD_C;
			SetBrowserMode ( STD_C );
			break;
		default:
			break;
	}	
/*	filename = GetFileName ( br.tbw[W_VISIT].text ); */
	if ( (filename != 0) && (strcmp(filename, "NoName")!=0) ) {
		(void) strcpy ( ew -> text -> current_dir, br.dir );
		if ( LoadFile ( ew -> text, filename, NEW ) != -1 )
			GotoLineNumber ( ew ->text, br.tbw[W_VISIT].text -> no_current_line );
      		else 
      			return;
	}
	else {
		(void) strcpy ( ew -> text -> current_dir, br.dir );
	}
	XStoreName ( dpy, ew -> w_frame, ew -> text -> filename );
	XMapWindow ( dpy, ew->w_frame );
	XFlush ( dpy );
}


/*
**	Function name : SelectItem
**
**	Description : Un item a ete selectionne dans une
**		des fenetres de texte.
**
**	Input : Le contexte, la position, le type.
**	Ouput :
*/
static void SelectItem ( bwin, x, y, i )
	BWin *bwin;
	register int x, y;
	register int i;
{
	register int n;
	register int scroll = bwin -> text -> sl;

	if ( ( bwin -> text -> lines_in_buf == 1 ) 
		|| (MoveToXYinTextWindow ( bwin -> text,  x, y ) == -1 ))
			return;

	TextCursorOn ( bwin -> text );
	TextCursorOff ( bwin -> text );

	if ( (n = bwin -> text -> no_current_line) >= bwin -> text -> lines_in_buf ) {
		return;
	}

	if ( bwin -> text -> sl == scroll ) {
		UpdateItem ( bwin, bwin -> select ); /* Eteint l'item courant */
	}
	UpdateItem ( bwin, n );	 /* Allume l'item selectionne */
	bwin -> select = n;

	switch ( i ) {
		case W_CLASS:
			ClassSelect ( bwin );
			break;
		case W_PARENT:
		case W_CHILD:
			ParentChildSelect ( &br.tbw[i] );
			break;
		case W_METHOD:
			MethodSelect ( bwin );
			break;
		case W_PROC:
			ProcSelect ( bwin );
			break;
		case W_FILES:
			FileSelect ( bwin );
			break;
		default:
			break;
	}
	br.old_click = br.click_time;
	if ( br.visit_raise == True )
		XMapRaised ( dpy, br.tbw[W_VISIT].frame );
}


/*
**	Function name : ClassSelect
**
**	Description : Selection d'une classe
**	Input : Le contexte
**	Ouput :
*/
static void ClassSelect ( bwin )
	BWin *bwin;
{
	register char  *class_name;
	register char **t_class;
	int len;
	Position *infos;
	register char **tmp;

	class_name = (char *) GetCurrentLine ( br.tbw[W_CLASS].buf, &len);
	class_name [len] = '\0';
	UpdateTitle ( class_name );
	infos = get_class_decl ( REMOVE_CLASS_INFO (class_name) );
	
	if ( infos == 0 ) return;

	ClearBuffer ( br.tbw[W_VISIT].buf );
	EditFile ( &br.tbw[W_VISIT], infos -> file_name );
	GoodPage ( &br.tbw[W_VISIT], infos -> line_number );
   	if ( infos != 0 )
		(void) free ( (char *) infos );

	t_class = br.class_save + (bwin -> select -1);

	if ( t_class != 0 ) {
      		if ( br.methods_save != 0 )
			(void) free ( (char *) br.methods_save );
		br.methods_save = ( char **)  get_methods_list ( *t_class);
		ExtractInfos ( br.methods_save, W_METHOD );
		ConfigScroll ( &br.tbw[W_METHOD] ); 
		br.tbw[W_METHOD].select = 0;
		ClearPage ( br.tbw[W_METHOD].text );
		FirstPage ( br.tbw[W_METHOD].text ); 

		tmp = ( char **)  get_parents_list ( *t_class);
		ExtractInfos ( tmp, W_PARENT );
      		if ( tmp != 0 )
			free ( (char *) tmp );
		ConfigScroll ( &br.tbw[W_PARENT] ); 
		br.tbw[W_PARENT].select = 0;
		ClearPage ( br.tbw[W_PARENT].text );
		FirstPage ( br.tbw[W_PARENT].text ); 

      		FirstPage ( br.tbw[W_CHILD].text );
		tmp = ( char **)  get_sons_list ( *t_class);
		ExtractInfos ( tmp, W_CHILD );
      		if ( tmp != 0 )
			free ( (char *) tmp );
		ConfigScroll ( &br.tbw[W_CHILD] );
		br.tbw[W_CHILD].select = 0;
 		ClearPage ( br.tbw[W_CHILD].text );
		FirstPage ( br.tbw[W_CHILD].text ); 
	}
}


/*
**	Function name : UpdateTitle
**
**	Description : Mise a jour du titre principale.
**
**	Input : Le titre
**	Ouput :
*/
static void UpdateTitle ( name )
	register char *name;
{
	if ( name == 0 )
		return;
	XClearWindow ( dpy, br.title );
	XDrawString ( dpy, br.title, br.gc, 5, br.font -> ascent + 3, name, strlen ( name ));
	Display3D ( dpy,br.title , br.ts, br.bs, 1, 0 );

	if ( br.title_name != 0 ) {
		if ( strcmp ( br.title_name, name ) == 0 )
			return;
      		if ( br.title_name != 0 )
			(void) free ( br.title_name );
	}
	br.title_name = (char *) malloc ( (unsigned int) strlen (name) +1 );
	(void) strcpy ( br.title_name, name );
}


/*
**	Function name : ProcSelect
**
**	Description : Selection d'une fonction
**	Input : Le contexte.
**	Ouput :
*/
static void ProcSelect ( bwin )
	BWin *bwin;
{
	register char  *proc_name;
	int len;
	Position *infos;

	proc_name = (char *) GetCurrentLine ( bwin -> buf, &len);
	proc_name [len] = '\0';
	UpdateTitle ( proc_name );
	infos = get_proc_impl ( proc_name );

	if ( infos != 0 ) {
		ClearBuffer ( br.tbw[W_VISIT].buf );
		EditFile ( &br.tbw[W_VISIT], infos -> file_name );
		GoodPage ( &br.tbw[W_VISIT], infos -> line_number );
	}
   	if ( infos != 0 )
		(void) free ( (char *) infos );
}


/*
**	Function name : FileSelect
**
**	Description : Selection d'un fichier
**	Input : Le contexte.
**	Ouput :
*/
static void FileSelect ( bwin )
BWin *bwin;
{
	register  char *name, **t_name;
	int len;
	
	HoleToLeft ( br.filebuf );

	if ( (bwin -> select - 1) != 0) 
		(void) MoveToLine ( br.filebuf, bwin -> select - 1 );

	name = GetCurrentLine ( br.filebuf, &len);

	name [len] = '\0';
	UpdateTitle ( name );
	t_name = br.files_save;

	while ( t_name != 0 ) {
		if ( (*t_name != 0) && (strcmp(name, *t_name) == 0) ) 
			break;
		t_name ++;
	}
	 	
	if ( name != 0 ) {
		ClearBuffer ( br.tbw[W_VISIT].buf );
		EditFile ( &br.tbw[W_VISIT], *t_name );
		FirstPage ( br.tbw[W_VISIT].text );
		TextCursorOn ( br.tbw[W_VISIT].text );
	}
}


/*
**	Function name : SelectMethod
**
**	Description : Selection d'une methode.
**	Input : Le contexte.
**	Ouput :
*/
static void MethodSelect ( bwin )
	BWin *bwin;
{
	register char  *method_name;
	register char **t_methods;
	int len;
	Position *infos;
    	char *buf;

	method_name = (char *) GetCurrentLine ( br.tbw[W_METHOD].buf, &len);
	method_name [len] = '\0';
	t_methods = br.methods_save + ( bwin -> select - 1 );
#ifndef lint
	buf = (char *) malloc ( (unsigned) (strlen (GET_METHOD_CLASS(*t_methods))
    		+ strlen ( *t_methods ) + 4) );
	(void) strcpy ( buf, GET_METHOD_CLASS(*t_methods) );
#else 
	buf = 0;
#endif
	(void) strcat ( buf, "::" );
	(void) strcat ( buf, *t_methods );
    	UpdateTitle ( buf );
	if ( buf != 0 )
		(void) free ( buf );
	
	if ( br.dec_imp_flag == DEC_MODE ) 
#ifndef lint
		infos =  get_method_decl ( GET_METHOD_CLASS(*t_methods), *t_methods );
	else
		infos =  get_method_impl ( GET_METHOD_CLASS(*t_methods), *t_methods );
#else
	infos = (Position*) t_methods;	/* affecte infos, et utilise t_methods */
#endif
	if ( infos != 0 ) {
		ClearBuffer ( br.tbw[W_VISIT].buf );
		EditFile ( &br.tbw[W_VISIT], infos -> file_name );
		GoodPage ( &br.tbw[W_VISIT], infos -> line_number );
	}
	if ( infos != 0 )
		(void) free ( (char *) infos );
}


/*
**	Function name : UpdateItem
**
**	Description : Allume ou eteint l'item n dans une des fenetres
**		du browser.
**	Input : Le contexte, l'item.
**	Ouput :
*/
static void UpdateItem ( bwin, n )
	BWin *bwin;
	register int n;
{
	register int y;

	if ( (n == 0)
		|| ( n < (bwin -> text -> no_current_line - bwin -> text -> n1))
		|| ( n > (bwin -> text -> no_current_line + bwin -> text -> n2)))
		return;

	y = n - ( bwin -> text -> no_current_line - bwin -> text -> n1 );
	y *= bwin -> text -> font_height;
	y += MARGE;

	XFillRectangle ( dpy, bwin -> text -> window,
		bwin -> text -> Igc, MARGE,  y, 
		bwin -> text -> width - ( 2 * MARGE ),
		bwin -> text -> font_height + 1 );
}


/*
**	Function name : PrentChildSelect
**
**	Description : Selection d'un parent/child
**	Input : Le contexte.
**	Ouput :
*/
static void ParentChildSelect ( bwin )
	BWin *bwin;
{
	register char *class_name;
	register char **t_class;
	register int i = 1;
	int len;
	register BWin *b_class = &br.tbw[W_CLASS];

	class_name = (char *) GetCurrentLine ( bwin -> buf, &len);
	class_name [len] = '\0';

	UpdateTitle ( class_name );
	class_name = REMOVE_CLASS_INFO(class_name);
	
	t_class = br.class_save;

	while ( t_class != 0 ) {
		if ( (*t_class == 0) || (strcmp(class_name, *t_class) == 0) ) 
			break;
		t_class ++;
		i++;
	}
	if ( *t_class == 0 ) {
		UpdateItem ( &br.tbw[W_PARENT], br.tbw[W_PARENT].select );
		UpdateTitle ( " " );
		br.tbw[W_PARENT].select = 0;
		return;
	}
	/* i = no ligne dans la liste des classes */
	UpdateItem ( b_class, b_class -> select ); /* eteint l'item courant */
	GotoLineNumber ( b_class -> text, i );
	CurrentLineToTop ( b_class -> text );
	UpdateItem ( b_class, b_class -> select ); /* allume l'item courant */
	SelectItem ( b_class, 5, 5, W_CLASS );
}


/*
**	Function name : EditFile
**
**	Description : Charge un fichier dans la fenetre de visite.
**	Input : Le contexte, le nom du fichier.
**	Ouput :
*/
static void EditFile ( bwin, name )
	BWin *bwin;
	register char *name;
{
	TextCursorOff ( bwin -> text );
	(void) strcpy ( bwin -> text -> current_dir, br.dir );
	if ( LoadFile ( bwin ->text, name, NEW ) != -1 ) {
		ShowScrollFrame ( dpy, bwin ->text -> swin );
		FirstPage ( bwin ->text ); 
	    	TitleVisit ( name );
	}
	TextCursorOn ( bwin ->text );
}


/*
**	Function name : TitleVisit
**
**	Description : Met a jour le titre de la fenetre de visit.
**	Input : Le nom a mettre.
**	Ouput :
*/
static void TitleVisit ( name )
	register char *name;
{
	if ( (name != 0) && strlen (name) != 0 ) {
		XClearWindow ( dpy, br.tbw[W_VISIT].title );
		XDrawString ( dpy, br.tbw[W_VISIT].title, br.gc, 5, br.font -> ascent + 3,
			name, strlen ( name) );
		Display3D ( dpy,br.tbw[W_VISIT].title, br.ts, br.bs, 1, 0 );
        }
}    

/*
**	Function name : GoodPage
**
**	Description : Positionne le fichier a la bonne page
**	Input : Le contexte, le numero de ligne.
**	Ouput :
*/
static void GoodPage ( bwin, line )
	BWin *bwin;
	register int line;
{
	TextCursorOff ( bwin ->text );
	GotoLineNumber ( bwin ->text, line );
	CurrentLineToTop ( bwin ->text );
	TextCursorOn ( bwin ->text );
}


/*
**	Function name : ButtonInVisitWindow
**
**	Description : Traitement d'un 'button press' dans
**		la fenetre de visit.
**	Input : Le contexte, l'event.
**	Ouput :
*/
static void ButtonInVisitWindow ( bwin, ev )
	BWin *bwin;
	XButtonEvent *ev;
{
	XGrabPointer ( dpy, bwin -> text -> window , True,
		ButtonReleaseMask, GrabModeAsync, GrabModeAsync,
		bwin -> text -> window, None, CurrentTime );

	switch ( ev -> button ) {
		case Button1:
			TextCursorOff ( bwin -> text );
			(void) MoveToXYinTextWindow ( bwin -> text, ev -> x, ev -> y );
			TextCursorOn ( bwin -> text );
			break;
		case Button3:
			TextCursorOff ( bwin -> text );
			XSync ( dpy, False );
			StoreBytesInCutBuffer ( bwin -> text, ev -> x, ev -> y );
			TextCursorOn ( bwin -> text );
			break;
		default:
			break;
	}
	WaitButtonRelease ( ev -> button );
}


/*
**	Function name : KeyPressInBrowser
**
**	Description : Comme son nom l'indique.
**	Input : L'event.
**	Ouput : Vrai si 'Key press' dans une des fenetres
**		du browser.
*/
int KeyPressInBrowser ( ev )
	XKeyEvent *ev;
{
	register int i;

	for ( i = 0; i < 7; i++ ) {
		if ( ev -> window == br.tbw [i].text -> window ) {
#ifdef DEBUG				
			fprintf ( stderr, "Key Press In Browser text\n");
#endif
			return True;
		}
	}
	if ( ev -> window == br.frame ) {
#ifdef DEBUG
		fprintf ( stderr, "Key Press In Browser frame\n");
#endif
		return True;
	}
	else
		return False;
}
