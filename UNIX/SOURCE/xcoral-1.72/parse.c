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
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <string.h>
#include <pwd.h>
#ifndef apollo
#include <malloc.h>
#endif

#include "config.h"
#include "options.h"
#include "text.h"

extern 	Display *dpy;
extern 	void exit ();
extern 	char *getenv ();
extern 	void GetShadow ();

static XrmOptionDescRec opTable [] = {

	{"=",		"*geometry",	XrmoptionIsArg,		(caddr_t) 0 },
	{"-d",		".display",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-display",	".display",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-fn",		"*font",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-font",	"*font",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-fg",		"*foreground",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-foreground",	"*foreground",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-bg",		"*background",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-background",	"*background",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-mfn",	"*mfont",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-mfont",	"*mfont",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-mfg",	"*mfg",		XrmoptionSepArg,	(caddr_t) 0 },
	{"-mforeground","*mforeground",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-mbg",	"*mbg",		XrmoptionSepArg,	(caddr_t) 0 },
	{"-mbackground","*mbackground",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-dbg",	"*dbg",		XrmoptionSepArg,	(caddr_t) 0 },
	{"-dfg",	"*dfg",		XrmoptionSepArg,	(caddr_t) 0 },
	{"-textmode",	"*textmode",	XrmoptionSepArg,	(caddr_t) 0 },
	{"-dw",		"*displaywarning",	XrmoptionIsArg,	(caddr_t) 0 },   
};

static 	int opTableEntries = 19;
static 	XrmDatabase	commandlineDB, savecommandlineDB;
static	XrmDatabase	rDB;
static	Options		options;
static	int		IsColor;

static void SetGeometry (), SetFont (), SetTextFG (), SetTextBG (), SetWarningMode ();
static void SetMenuFont (), SetMenuFG (), SetMenuBG (), SetDialogBG (), SetDialogFG ();
static void	SetDefaultMode();
static char *GetHomeDir ();

#define Message(str) fprintf(stderr, "%s\n", str )

/*
**	Function name : ParseOpenDisp
**
**	Description : Parse la ligne de commande et ouvre le
**		display. 
**	Input : Le nombre d'arguments, la table des arguments.
**	Ouput :
*/
void ParseOpenDisp ( argc, argv )
int *argc;
register char *argv [];
{
	XrmValue value;
	char *str_type [10];
	char *displayname;
	char buf [32];

	/*
	 * On peche les options de la ligne de commande	
	 */
	XrmParseCommand ( &commandlineDB, opTable,
		opTableEntries, argv [0], argc, argv );

	/*
	 * J'ai besoin des options de la ligne de commande pour plus tard.
	 * En fait, c'est a cause de la fonction XMerge... qui est destructrice
	 * Malheureusement il faut passer par un fichier intermediaire.
	 */
	bzero ( buf, 32 );
	(void) sprintf ( buf, "/tmp/xcoral_db%d", getuid() );
	XrmPutFileDatabase ( commandlineDB, buf );
	if ( (savecommandlineDB = XrmGetFileDatabase (  buf )) != 0 ) 
		(void) unlink ( buf ) ;

	/*
	 * Un nom de fichier peut etre...
	 */
	if ( *argc == 2 )
		if ( argv[1][0] == '-' )
			Usage ();
		else
			(void) strcpy ( options.filename, argv[1] );
	else 
		(void) strncpy ( options.filename, "NoName", 6 );
		
	/*
	 * Avant d'ouvrir la connexion avec le serveur, on verifie
	 * si la commande positionne le display. Sinon si displayname == 0
	 * c'est la variable d'environnemt DISPLAY qui est utilisee.
	 */
	displayname = 0;
	if ( XrmGetResource ( commandlineDB, "xcoral.display",
		"xcoral.display",	str_type, &value ) == True ) {
			displayname = (char *) malloc ( (unsigned) (value.size + 1) );
			bzero ( displayname, (int) (value.size + 1));
			(void) strncpy ( displayname, value.addr, (int) value.size );
	}
	
	if ( ! (dpy = XOpenDisplay ( displayname ))) {
		(void) fprintf ( stderr, "%s : Cant'open display '%s'\n",
			argv [0], XDisplayName ( displayname ));
		(void) exit (1);
	}
   	if ( displayname != 0 )
		(void) free (displayname); 
 
	if ( DefaultDepth ( dpy, DefaultScreen ( dpy )) == 1 ) 
		IsColor = False;
	else
		IsColor = True;

}


/*
**	Function name : GetUserDataBase
**
**	Description : Recherche des ressources dans la
**		hierarchie habituelle.
**	Input : 
**	Ouput :
*/
void GetUserDatabase ()
{
	XrmDatabase homeDB, serverDB, applicationDB;

	char 	filename [128];	
	char 	*environment;
	char 	name [256];
	char *str;
	
	(void)  sprintf ( name, "%s/%s", APP_DEFAULT, "Xcoral" );

	applicationDB = XrmGetFileDatabase ( name );
	(void) XrmMergeDatabases ( applicationDB, &rDB );
/*
	if ( (serverDB = XrmGetDatabase (dpy)) == 0 ) { 
		(void) GetHomeDir ( filename );
		(void) strcat ( filename, "/.Xdefaults" );
		serverDB = XrmGetFileDatabase ( filename );
	}
*/
	if ( (str = XResourceManagerString ( dpy )) != 0 ) 
		serverDB = XrmGetStringDatabase ( str );
	else {	
		(void) GetHomeDir ( filename );
		(void) strcat ( filename, "/.Xdefaults" );
		serverDB = XrmGetFileDatabase ( filename );
	}

	(void) XrmMergeDatabases ( serverDB, &rDB );
	
	if ( (environment = getenv ( "XENVIRONEMENT" )) == 0 ) {
		environment = GetHomeDir ( filename );
		(void) strcat ( environment, "/.Xdefaults" );
	}
	
	homeDB = XrmGetFileDatabase ( environment );
	(void) XrmMergeDatabases ( homeDB, &rDB );
}


/*
**	Function name : MergeOptions
**
**	Description :  On ajoute les arguments de la ligne
*		de commande dans la base commune.
**	Input : 
**	Ouput :
*/
void MergeOptions ()
{
	XColor xcolor;

	XrmMergeDatabases ( commandlineDB, &rDB );

	/*
	 * Enfin, on remplie la table des options.
	 */ 
	(void) SetGeometry ();
	(void) SetFont (); 
	(void) SetTextFG ();
	(void) SetTextBG ();
	(void) SetMenuFont ();
	(void) SetMenuFG ();
	(void) SetMenuBG ();
	(void) SetDialogFG ();
	(void) SetDialogBG ();
	SetDefaultMode();
   	SetWarningMode();

	/*
	 * Calcul des top et bottom shadow
	 */
	xcolor.pixel = (unsigned long) options.mbg;
	XQueryColor ( dpy, DefaultColormap ( dpy, DefaultScreen ( dpy ) ),&xcolor ); 
	GetShadow ( dpy, &xcolor, &options.menu_top_shadow, &options.menu_bottom_shadow );
	
	xcolor.pixel = (unsigned long) options.dbg;
	XQueryColor ( dpy, DefaultColormap ( dpy, DefaultScreen ( dpy ) ), &xcolor ); 
	GetShadow ( dpy, &xcolor, &options.dialog_top_shadow, &options.dialog_bottom_shadow );

	xcolor.pixel = (unsigned long) options.bg;
	XQueryColor ( dpy, DefaultColormap ( dpy, DefaultScreen ( dpy ) ), &xcolor );
	GetShadow ( dpy, &xcolor, &options.text_top_shadow, &options.text_bottom_shadow );

	/* A partir de la, les ressources sont valides et utilisables par le programme */
}

/*
**	Function name : SetDefaultBG
**
**	Description :  Lorqu' une couleur de fond n'est pas precisee,
**		on utilise celle par defaut. Il faut verifier que cette
**		couleur n'est pas identique a celle de devant ( si fg
**		est une option de la ligne commande, ca peut etre le cas ).
**	Input :  Foreground, background, les valeurs par default.
**	Ouput :
*/
SetDefaultBG ( bg, fg, bwbg, bwfg, colorbg, colorfg)
unsigned long *bg; /* RETURN */
unsigned long *fg;
register char *bwbg, *bwfg, *colorbg, *colorfg; /*Default bw an color bg */
{	
	int result;

	if ( IsColor == False )  {
		*bg = PixelValue ( dpy, bwbg, &result );
		if ( *bg == *fg ) {
			*bg = PixelValue ( dpy, bwfg, &result );
		}
	}
	else { /* color */
		*bg = PixelValue ( dpy, colorbg, &result );
		if ( *bg == *fg ) {
			*bg = PixelValue ( dpy, colorfg, &result );
		}
	}
}


/*
**	Function name : SetDialogFG
**
**	Description : Calcul du foreground pour la fenetre 
**		de controle.
**	Input : 
**	Ouput :
*/
static void SetDialogFG ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	int result;

	if ( XrmGetResource ( rDB, "xcoral.dfg",
		"xcoral.DialogForeground", str_type,&value ) == True ) {

		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		if ( IsColor == False ) {
			if ( (strcmp ( buf, "black" ) != 0 ) && (strcmp ( buf, "white" ) != 0 )) {
				options.dfg = PixelValue ( dpy, BW_DIALOG_FG, &result );
			}
			else {
				options.dfg = PixelValue ( dpy, buf, &result );
			}
		}
		else { /* Color Display */
			options.dfg = PixelValue ( dpy, buf, &result );
			if ( result == False ) {
				options.dfg = PixelValue ( dpy, COLOR_DIALOG_FG, &result );
			}
 		}
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else  {
		options.dfg = IsColor ? PixelValue ( dpy, COLOR_DIALOG_FG, &result ) :
			PixelValue ( dpy, BW_DIALOG_FG, &result );
	}
}

/*
**	Function name : SetDialogBG
**
**	Description : Calcul du background pour la fenetre de
**		controle.
**	Input : 
**	Ouput :
*/
static void SetDialogBG ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	int result;

	if ( XrmGetResource ( rDB, "xcoral.dbg",
		"xcoral.DialogBackground", str_type,&value ) == True ) { 

		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		if ( IsColor == False ) {
			if ( (strcmp ( buf, "black" ) != 0 ) && (strcmp ( buf, "white" ) != 0 )) 
				options.dbg = PixelValue ( dpy, BW_DIALOG_BG, &result );
			else  
				options.dbg = PixelValue ( dpy, buf, &result );
			if ( options.dbg == options.dfg ) {
				if ( XrmGetResource ( savecommandlineDB, "xcoral*dbg",
					"xcoral*DialogBackground", str_type,&value ) == True ) {
					if ( options.dbg == PixelValue ( dpy, BW_DIALOG_BG, &result ))
						options.dfg = PixelValue ( dpy, BW_DIALOG_FG, &result );
					else
						options.dfg = PixelValue ( dpy, BW_DIALOG_BG, &result );
				}
				else {
					if ( strcmp ( buf, BW_DIALOG_BG ) == 0 ) 
						options.dbg = PixelValue ( dpy, BW_DIALOG_FG, &result );
					else 
						options.dbg = PixelValue ( dpy, BW_DIALOG_BG, &result );
				}
			}
		}
		else { /* Color */
			options.dbg = PixelValue ( dpy, buf, &result );
			if ( result == False )
				options.dbg = PixelValue ( dpy, COLOR_DIALOG_BG, &result );
			if ( options.dbg == options.dfg ) {
				options.dbg = PixelValue ( dpy, COLOR_DIALOG_BG, &result );
				options.dfg = PixelValue ( dpy, COLOR_DIALOG_FG, &result );
			}
		}
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else 
		SetDefaultBG ( &options.dbg, &options.dfg, BW_DIALOG_BG, BW_DIALOG_FG,
			COLOR_DIALOG_BG, COLOR_DIALOG_FG );
}

/*
**	Function name : SetMenuFG
**
**	Description : Calcul du foreground pour les menus.
**	Input : 
**	Ouput :
*/
static void SetMenuFG ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	int result;

	if ( XrmGetResource ( rDB, "xcoral.mfg",
		"xcoral.mforeground", str_type,&value ) == True ) {

		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		if ( IsColor == False ) {
			if ( (strcmp ( buf, "black" ) != 0 ) && (strcmp ( buf, "white" ) != 0 ))
				options.mfg = PixelValue ( dpy, BW_MENU_FG, &result );
			else 
				options.mfg = PixelValue ( dpy, buf, &result );
		}
		else { /* Color Display */
			options.mfg = PixelValue ( dpy, buf, &result );
			if ( result == False ) 
				options.mfg = PixelValue ( dpy, COLOR_MENU_FG, &result );
		}
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else 
		options.mfg = IsColor ? PixelValue ( dpy, COLOR_MENU_FG, &result ) :
			PixelValue ( dpy, BW_MENU_FG, &result );
}

/*
**	Function name : SetMenuBG
**
**	Description : Calcul du background pour les menus.
**	Input : 
**	Ouput :
*/
static void SetMenuBG ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	int result;

	if ( XrmGetResource ( rDB, "xcoral.mbg",
		"xcoral.mbackground", str_type,&value ) == True ) {
		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		if ( IsColor == False ) {
			if ( (strcmp ( buf, "black" ) != 0 ) && (strcmp ( buf, "white" ) != 0 )) 
				options.mbg = PixelValue ( dpy, BW_MENU_BG, &result );
			else  
				options.mbg = PixelValue ( dpy, buf, &result );
			if ( options.mbg == options.mfg ) {
				if ( XrmGetResource ( savecommandlineDB, "xcoral*mbg",
					"xcoral*MenuBackground", str_type,&value ) == True ) {
					if ( options.mbg == PixelValue ( dpy, BW_MENU_BG, &result ))
						options.mfg = PixelValue ( dpy, BW_MENU_FG, &result );
					else
						options.mfg = PixelValue ( dpy, BW_MENU_BG, &result );
				}
				else {
					if ( strcmp ( buf, BW_MENU_BG ) == 0 ) 
						options.bg = PixelValue ( dpy, BW_MENU_FG, &result );
					else 
						options.bg = PixelValue ( dpy, BW_MENU_BG, &result );
				}
			}
		}
		else { /* Color */
			options.mbg = PixelValue ( dpy, buf, &result );
			if ( result == False )
				options.mbg = PixelValue ( dpy, COLOR_MENU_BG, &result );
			if ( options.mbg == options.mfg ) {
				options.mbg = PixelValue ( dpy, COLOR_MENU_BG, &result );
				options.mfg = PixelValue ( dpy, COLOR_MENU_FG, &result );
			}
		}
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else 
		SetDefaultBG ( &options.mbg, &options.mfg, BW_MENU_BG, BW_MENU_FG,
			COLOR_MENU_BG, COLOR_MENU_FG );
}

/*
**	Function name : SetTextFG
**
**	Description : Calcul du foreground pour les fenetres de texte.
**	Input : 
**	Ouput :
*/
static void SetTextFG ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	int result;

	if ( XrmGetResource ( rDB, "xcoral.foreground",
		"xcoral.Foreground", str_type,&value ) == True ) {

		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		if ( IsColor == False ) {
			if ( (strcmp ( buf, "black" ) != 0 ) && (strcmp ( buf, "white" ) != 0 ))
				options.fg = PixelValue ( dpy, BW_TEXT_FG, &result );
			else 
				options.fg = PixelValue ( dpy, buf, &result );
		}
		else { /* Color Display */
			options.fg = PixelValue ( dpy, buf, &result );
			if ( result == False ) {
				options.fg = PixelValue ( dpy, COLOR_TEXT_FG, &result );
			}
		}
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else {
		options.fg = IsColor ? PixelValue ( dpy, COLOR_TEXT_FG, &result ) :
			PixelValue ( dpy, BW_TEXT_FG, &result );
	}
}

static void SetDefaultMode()
{
	XrmValue	value;
	char		*str_type[20];
	static char	*names[] ={"text", "C", "C++", 0};
	static int	mode[] = { TEXT, STD_C, CPLUS };
	int		i;
	
	if ( XrmGetResource( rDB, "xcoral.textmode", 
			"xcoral.Textmode", str_type, &value ) == True ) {		
		for(i=0;names[i] != 0; i++)
			if (strcmp(names[i],value.addr) == 0)
				break;
		
		if (names[i] == 0)
			(void) fprintf(stderr,"textmode %s not recognised\n", value.addr);
		else
			default_mode = mode[i];
	}
}

static void SetWarningMode()
{
	XrmValue	value;
	char		*str_type[20];

	if ( XrmGetResource( rDB, "xcoral.displaywarning", 
			"xcoral.Displaywarning", str_type, &value ) == True ) {
      		if ( (strcmp (value.addr, "True") == 0) 
			|| (strcmp (value.addr, "-dw") == 0) )
      			options.verb = True;
      		else 
      			options.verb = False;
	}
   	else 
   		options.verb = False;
}

/*
**	Function name : SetTextBG
**
**	Description : Calcul du background pour les fenetres de texte.
**	Input : 
**	Ouput :
*/
static void SetTextBG ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	XFontStruct *LoadFont ();
	int result;

	/* Le Foreground est deja connu */

	if ( XrmGetResource ( rDB, "xcoral.background",
		"xcoral.Background", str_type,&value ) == True ) {
		/* 
		 * Le foreground est defini dans les resources ou dans
		 * la ligne de commande
		 */
		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		if ( IsColor == False ) {
			if ( (strcmp ( buf, "black" ) != 0 ) && (strcmp ( buf, "white" ) != 0 )) 
				options.bg = PixelValue ( dpy, BW_TEXT_BG, &result );
			else  
				options.bg = PixelValue ( dpy, buf, &result );
			if ( options.bg == options.fg ) {
				if ( XrmGetResource ( savecommandlineDB, "xcoral*background",
					"xcoral*Background", str_type,&value ) == True ) {
					if ( options.bg == PixelValue ( dpy, BW_TEXT_BG, &result ))
						options.fg = PixelValue ( dpy, BW_TEXT_FG, &result );
					else
						options.fg = PixelValue ( dpy, BW_TEXT_BG, &result );
				}
				else {
					if ( strcmp ( buf, BW_TEXT_BG ) == 0 ) 
						options.bg = PixelValue ( dpy, BW_TEXT_FG, &result );
					else 
						options.bg = PixelValue ( dpy, BW_TEXT_BG, &result );
				}
			}
		}
		else { /* Color */
			options.bg = PixelValue ( dpy, buf, &result );
			if ( result == False )
				options.bg = PixelValue ( dpy, COLOR_TEXT_BG, &result );
			if ( options.bg == options.fg ) {
				options.bg = PixelValue ( dpy, COLOR_TEXT_BG, &result );
				options.fg = PixelValue ( dpy, COLOR_TEXT_FG, &result );
			}
		}
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else {
		SetDefaultBG ( &options.bg, &options.fg, BW_TEXT_BG, BW_TEXT_FG,
			COLOR_TEXT_BG, COLOR_TEXT_FG );
	}
}

/*
**	Function name : SetMenuFont
**
**	Description : La fonte pour les menus.
**	Input : 
**	Ouput :
*/
static void SetMenuFont ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	XFontStruct *LoadFont ();

	if ( XrmGetResource ( rDB, "xcoral.mfont",
		"xcoral.Mfont", str_type,&value ) == True ) {

		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );

		options.menu_font = LoadFont ( dpy, buf );
      		if ( buf != 0 )
			(void) free ( buf );
	}
	else {
		options.menu_font = LoadFont ( dpy, MENU_FONT );
	}
}


/*
**	Function name : SetFont
**
**	Description : La fonte pour les fenetres texte.
**	Input : 
**	Ouput :
*/
static void SetFont ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	XFontStruct *LoadFont ();

	if ( XrmGetResource ( rDB, "xcoral.font",
		"xcoral.Font", str_type,&value ) == True ) {
		/*
		 * Ya des resssources
		 */
		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		bzero ( buf, (int) (value.size + 1));
		(void) strncpy ( buf, value.addr, (int) value.size );
		options.text_font = LoadFont ( dpy, buf );
      		if ( buf != 0 )
			(void) free (buf);
	}
	else {
		options.text_font = LoadFont ( dpy, TEXT_FONT );
	}
}


/*
**	Function name : SetGeometry
**
**	Description : Initialisation de la geometrie de 
**		la feneter printcipale.
**	Input : 
**	Ouput :
*/
static void SetGeometry ()
{
	XrmValue value;
	char *str_type[20];
	register char *buf;
	int x, y;
	unsigned int width, height;
	long flags;

	if ( XrmGetResource ( rDB, "xcoral.geometry",
		"xcoral.Geometry", str_type,&value ) == True ) {
		/*
		 * Ya des resssources
		 */
		buf = (char *) malloc ( (unsigned) (value.size + 1) );
		(void) strncpy ( buf, value.addr, (int) value.size ); 

		flags = XParseGeometry ( buf,  &x, &y, &width, &height );
      		if ( buf != 0 )
			(void) free ( buf );

		if (!((WidthValue | HeightValue) & flags) )
			Usage ();

		if ( XValue & flags ) {
			if ( XNegative & flags )
				x = DisplayWidth ( dpy, DefaultScreen ( dpy ) ) + x;
			options.x = x;
		}
		else
			options.x = 0;

		if ( YValue & flags ) {
			if ( YNegative & flags )
				y = DisplayHeight ( dpy, DefaultScreen (dpy ) ) + y;
			options.y = y;
		}
		else
		        options.y = 0;

		if ( x == 0 || x > DisplayWidth ( dpy, DefaultScreen (dpy )) )
		        options.x = 100;

		if ( y == 0 || y > DisplayHeight ( dpy, DefaultScreen (dpy )) )
		        options.y = 100;

		if ( (width < ((DisplayWidth ( dpy, DefaultScreen (dpy )) * 2 )  / 5 ))
		    || width  > DisplayWidth ( dpy, DefaultScreen (dpy ) ) )
		       options.width = DisplayWidth ( dpy, DefaultScreen (dpy ) ) / 2;
		else
		       options.width = width;

		if ( (height < ((DisplayHeight ( dpy, DefaultScreen (dpy )) * 2 ) /5 )) 
		    || height > DisplayHeight ( dpy, DefaultScreen (dpy ) ) )
		       options.height = 
				(DisplayHeight ( dpy, DefaultScreen (dpy ) ) * 2) / 3;
		else
		       options.height = height;
	}
	else {
		/*
		 *	Largeur par defaut =  2/3 de l'ecran.
		 *	Hauteur par defaut =  2/3 de l'ecran.
		 */
		options.width = (DisplayWidth ( dpy, DefaultScreen (dpy ) ) * 2) / 3;
		options.height = (DisplayHeight ( dpy, DefaultScreen (dpy ) ) * 2) / 3;
	}
}



/*
**	Function name : GetHomeDir
**
**	Description : Comme son nom l'indique.
**	Input : Le container.
**	Ouput :
*/
static char *GetHomeDir ( dest )
char * dest;
{
	int uid;
/*	struct passwd *getpwuid (); */
	struct passwd *pw;
	register char *ptr;
	
	if ((ptr = getenv ("HOME")) != 0 ) {
		(void) strcpy ( dest, ptr );
	}
	else {
		if ((ptr = getenv ("USER")) != 0 ) {
			pw = getpwnam (ptr);
		}
		else {
			uid = getuid ();
			pw = getpwuid ( uid );
		}
		if (pw) {
			(void) strcpy ( (char *) dest, (char *) pw -> pw_dir );
		}
		else {
			*dest = ' ';
		}
	}
	return dest;
}


/*
**	Function name : Usage
**
**	Description : 
**	Input : 
**	Ouput :
*/
Usage ()
{
	(void) fprintf ( stderr,"Usage : xcoral [-options...]\n");
	(void) fprintf ( stderr,"Option :\n");
	(void) fprintf ( stderr,
		"\t[=WidthxHeight+X+Y]\t -Geometry\n" );
	(void) fprintf ( stderr,
		"\t[-d/-display]\t\t -Display\n" );
	(void) fprintf ( stderr,
		"\t[-fn/-font]\t\t -Font in text window\n" );
	(void) fprintf ( stderr,
		"\t[-bg/-background]\t -Background color in text window\n" );
	(void) fprintf ( stderr,
		"\t[-fg/-foreground]\t -Foreground color in text window\n" );
	(void) fprintf ( stderr,
		"\t[-mfn/-mfont]\t\t -Font in menu\n" );
	(void) fprintf ( stderr,
		"\t[-mbg/-mbackground]\t -Background color in menu\n" );
	(void) fprintf ( stderr,
		"\t[-mfg/-mforeground]\t -Foreground color in menu\n" );
	(void) fprintf ( stderr,
		"\t[-dbg/-mbackground]\t -Background color in dialog box\n" );
	(void) fprintf ( stderr,
		"\t[-dfg/-mforeground]\t -Foreground color in dialog box\n" );
	(void) fprintf ( stderr,
		"\t[-textmode]\t\t -Text mode : C, C++, text \n" );
	(void) fprintf ( stderr,
		"\t[-dw/-displaywarning]\t -Browser warning messages\n" );
	
	(void) exit (1);
}


/*
**	Function name : PixelValue
**
**	Description : 
**	Input : 
**	Ouput :
*/
unsigned long PixelValue ( display, str, result )
Display *display;
char *str;
int *result;
{
	XColor  color, tcolor;
	int status;

	status = XAllocNamedColor ( display,
		DefaultColormap ( display, DefaultScreen ( display ) ), 
		str, &color, &tcolor );

	if ( status == True )
		*result = True;
	else {
		if ( IsColor == True ) {
			(void) fprintf ( stderr, "Can't alloc named color %s\n", str );
			(void) exit (1);
		}
		else
			*result = False;
	}
	return (color.pixel);
}


/*
**	Function name : GetOpFont
**
**	Description : Donne la fonte pour le type
**		d'objet passe en argument.
**	Input : Le type
**	Ouput : Un pointeur sur la fonte.
*/
XFontStruct *GetOpFont ( type )
register int type;
{
	XFontStruct *font;

	switch ( type ) {
	case OP_TEXT_FONT:
		font = options.text_font;
		break;
	case OP_MENU_FONT:
		font = options.menu_font;
		break;
	}
	return (font);
}


/*
**	Function name : GetOpColor
**
**	Description : Donne la couleur du type d'objet
**		passe en argument.
**	Input : Le type
**	Ouput : La valeur en pixel.
*/
unsigned long GetOpColor ( type )
register int type;
{
	unsigned long color;

	switch ( type ) {
	case OP_TEXT_FG:
		color = options.fg;
		break;
	case OP_TEXT_BG:
		color =  options.bg;
		break;
	case OP_TEXT_TS:
		color =  options.text_top_shadow;
		break;
	case OP_TEXT_BS:
		color = options.text_bottom_shadow;
		break;
	case OP_MENU_FG:
		color =  options.mfg;
		break;
	case OP_MENU_BG:
		color = options.mbg;
		break;
	case OP_MENU_TS:
		color =  options.menu_top_shadow;
		break;
	case OP_MENU_BS:
		color = options.menu_bottom_shadow;
		break;
	case OP_DIAL_FG:
		return options.dfg;
		break;
	case OP_DIAL_BG:
		return options.dbg;
		break;
	case OP_DIAL_TS:
		color = options.dialog_top_shadow;
		break;
	case OP_DIAL_BS:
		color = options.dialog_bottom_shadow;
		break;
	}
	return color;
}


/*
**	Function name : GetOpFilename
**
**	Description : Donne le filename de la ligne de commande.
**	Input : 
**	Ouput : Le filename.
*/
char *GetOpFilename ()
{
    	return ( options.filename ); 
}


/*
**	Function name : GetOpBW
**
**	Description : La Largeur du bord.
**	Input : 
**	Ouput :
*/
unsigned int GetOpBW ()
{
	return options.bw;
}


/*
**	Function name : GetOpBD
**
**	Description : Le border 
**	Input : 
**	Ouput :
*/
unsigned  long GetOpBD ()
{
	return options.bd;
}


/*
**	Function name : GetOpGeo
**
**	Description : Donne un element de la geometrie.
**	Input : Le type.
**	Ouput : Valeur en pixels.
*/
int GetOpGeo ( type )
register int type;
{
	register int result;

	switch ( type ) {
		case OP_X:
			result = options.x;
			break;
		case OP_Y:
			result = options.y;
			break;
		case OP_WIDTH:
			result = options.width;
			break;
		case OP_HEIGHT:
			result = options.height;
			break;
	}
	return result;
}


/*
**	Function name : OpVerbose
**
**	Description : 
**	Input : 
**	Ouput :
*/
OpVerbose ()
{
	return (options.verb);
}
