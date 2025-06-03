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
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <sys/param.h>
#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#include "options.h"
#include "xcoral.h"
#include "browser.h"
#include "flist.h"

Display		*dpy;
EdWin		*TWin [MAXWIN], *edwin; 	/* La table des fenetres */
XContext	EdContext;			/* Pour switcher */
static void	contHandler (), stopHandler ();


/*
**	Initialise les resources, les menus, les bouttons etc...
**	Creer la premiere fenetre de texte et entre dans
**	la boucle d'evenements.
*/
main ( argc, argv ) 
int argc;
register char **argv;
{
	EdWin *CreateWindow ();
	char pathname [MAXPATHLEN];
	extern void exit ();
	extern char  *getcwd();

#if defined(DEBUG) && defined(sparc)
	malloc_debug(2);
	malloc_verify();
#endif
	/*
	 * Ou suis-je dans quelle etagere.
	 */
	if ( getcwd ( (char *) pathname, MAXPATHLEN + 2 ) == 0 ) {
		(void) fprintf ( stderr, "Getwd error\n" );
		(void) exit ( 1 );
	}
	(void) bzero ( (char *) TWin, MAXWIN );

	/*
	 * Initialisation du ressource manager, connexion avec le serveur,
	 * creation d'un contexte graphique pour le top et bottom shadow.
	 * Calcul des options ( parametres de la commande, .Xdefaults etc... )
	 */
	XrmInitialize ();		
	ParseOpenDisp ( &argc, argv );
	CreateRGC ( dpy );
	GetUserDatabase ();
	MergeOptions ();

	EdContext = XUniqueContext ();

	/*
 	 * Initialisation des ressources pour les elements Text,
	 * Menus et Panel de controle.
	 * Les ressources sont :
	 * 	une fonte et 4 couleurs (foreground, background,
	 *	top_shadow et bottom shadow ).
	 */
	InitTextRes ( dpy, GetOpFont ( OP_TEXT_FONT ), GetOpColor ( OP_TEXT_FG ),
		GetOpColor ( OP_TEXT_BG ), GetOpColor ( OP_MENU_TS ),
		 GetOpColor ( OP_MENU_BS ));

	InitMenusRes ( dpy, GetOpFont ( OP_MENU_FONT ), GetOpColor ( OP_MENU_FG ),
		GetOpColor ( OP_MENU_BG ), GetOpColor ( OP_MENU_TS ),
		 GetOpColor ( OP_MENU_BS ));

	InitControlRes ( GetOpColor ( OP_DIAL_FG ), GetOpColor ( OP_DIAL_BG ),
		GetOpColor ( OP_DIAL_TS ),  GetOpColor ( OP_DIAL_BS ));

	/*
	 * Encore quelques initialisations.
	 */
	InitEvent ();
	InitScroll ( dpy );
	InitKillBuf ();
	InitBrowser ();
    	InitDialogWindow (); 
	InitFileSelector ();
	SetBrowserMode ( default_mode );
	SetBrowserDir ( (char *) pathname );

	/*
	 * Allons-y pour la premiere fenetre d'edition. 
	 */
	if ( (edwin = CreateWindow ()) == 0 ) { 
		( void ) fprintf ( stderr,"Create window error\n" );
		(void) exit (1);
	}
	(void) strcpy ( edwin -> text -> filename, (char *) GetOpFilename () );
	(void) strcpy ( edwin -> text -> current_dir, pathname );

	/*
	 * Si un nom de fichier est passe en argument on le charge
	 * dans la premiere fenetre de texte.
	 */	
	if ( strcmp ( (char *) GetOpFilename (), "NoName" ) != 0 ) {
		if ( LoadFile ( edwin -> text, (char *) GetOpFilename (), NEW ) == -1 ) {
			XStoreName ( dpy, edwin -> w_frame, edwin -> text -> filename ); 
			SetDirAndFilename ( edwin -> text,  (char *) GetOpFilename () );
  		}
	}
	
	/*
	 * On ignore les signaux habituels, et on attrape le stop et
	 * le continue (pour des raisons tordues).
	 */
	(void) signal ( SIGINT, SIG_IGN );
#ifndef DEBUG
	(void) signal ( SIGQUIT, SIG_IGN );
#endif
	(void) signal ( SIGTSTP, stopHandler );
	(void) signal ( SIGCONT, contHandler );

	/* 
	 * Affichage de la premiere fenetre et on attend que les Events y se
	 * pointent.
	 */
	XMapWindow ( dpy, edwin->w_frame );
	XFlush ( dpy );
	WaitForEvent ();

	/*NOTREACHED*/
}

/*
**	Name : stopHandler
**
**	Description : Attrape le signal 'stop'. S'il reste des requetes
**		ou des	evenements on ignore le signal.
**
*/
static void stopHandler ()
{

#ifdef DEBUG
	(void) fprintf ( stderr, "Stop\n" );
#endif
	(void) signal ( SIGTSTP, SIG_IGN );
	XSync ( dpy, False );
	if ( QLength ( dpy ) == 0 ) {	/* On peut stopper */
		(void) signal ( SIGCONT, contHandler );
		(void) signal ( SIGTSTP, SIG_DFL );
		(void) kill ( getpid(), SIGTSTP );
	}
	else
		(void) signal ( SIGTSTP,  stopHandler );
}


/*
**	Name : contHandler 
**
**	Description : Attrape le signal 'continue' pour virer tous
**		les evenements de type ButtonPress recus pendant que le
**		process etait stoppe. 
*/
static void contHandler ()
{
	XEvent event;

#ifdef DEBUG
	(void) fprintf ( stderr, "Continue\n" );
#endif
	(void) signal ( SIGCONT, SIG_IGN );

	XSync ( dpy, False );
	if ( QLength ( dpy ) != 0 ) 
		while ( XCheckMaskEvent ( dpy, ButtonPress, &event ));

	(void) signal ( SIGTSTP, stopHandler );
	(void) signal ( SIGCONT, SIG_DFL );
	(void) kill ( getpid (), SIGCONT );

	XRaiseWindow ( dpy, edwin -> w_frame );
	XFlush ( dpy ); 
}

