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

#include "options.h"
#include "xcoral.h"
#include "browser.h"
#include "flist.h"

char *menu_names[] = { 
	"File", "Window", "Modes", "Search", "Region", "Font", "Misc", 0
};

static char *file_menu [] = {
	"New File        ^x^k",
	"Read File       ^x^f",
	"Insert File     ^x i",
	"Save File       ^x^s",
	"Write File as   ^x^w",
	"Quit            ^x^c",
	0
};
			
static char *window_menu [] = {
	"New Text Window     ^x 2",
	"Delete Text Window  ^x^c",
	"Display Kill Buffer",
	"Display Open Files  ^x b",
	"Display Browser",
	"Unmap Browser",
	0
};

static char *mode_menu [] = {
	"Default text",
	"C",
	"C ++",
	0 
};
			
static char *search_menu [] = { 
	"Forward Search        ^s",
	"Backward Search       ^r",
	"Query Replace      Esc q",
	"Global Replace     Esc r",
	"Goto Line Number    ^x l",
	0
};

static char *mark_menu [] = {
	"Set Mark              ^space",
	"Exchange point mark     ^x^x",
	"Kill Region               ^w",
	"Copy Region            Esc w",
	"Paste Region              ^y",
	0
};

static char *font_menu [] = { 
	"Courier 10",
	"Courier 12",
	"Courier bold 14",
	"Courier 18",
	"Courier 24",
	"Helvetica 14",
	"Helvetica 20",
	"Times bold 14",
	"Times 18",
	"Times 20",
	"Times 24",
	"Schoolbook 18",
	0 
};

static char *font_names [] = {
	"-adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1",
	"-adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-1",
	"-adobe-courier-bold-r-normal--14-140-75-75-m-90-iso8859-1",
	"-adobe-courier-medium-r-normal--18-180-75-75-m-110-iso8859-1",
	"-adobe-courier-medium-r-normal--24-240-75-75-m-150-iso8859-1",	
	"-adobe-helvetica-medium-r-normal--14-100-100-100-p-76-iso8859-1",
	"-adobe-helvetica-medium-r-normal--20-140-100-100-p-100-iso8859-1",
	"-adobe-times-bold-r-normal--14-140-75-75-p-77-iso8859-1",
	"-adobe-times-medium-r-normal--18-180-75-75-p-94-iso8859-1",
	"-adobe-times-medium-r-normal--20-140-100-100-p-100-iso8859-1",
	"-adobe-times-medium-r-normal--24-240-75-75-p-132-iso8859-1",
	"-adobe-new century schoolbook-medium-r-normal--18-180-75-75-p-103-iso8859-1",
	0
};

static char *misc_menu [] = {
	"New C++ Class",
	"New C++ Method",
	"New C Function",
	"Version", 
	"help", 
	0
};

char **item_names [] = {
	file_menu, window_menu, mode_menu, search_menu, mark_menu, font_menu, misc_menu, 0
};

static void (*f_file []) () = {
	MenuNewFile,
	MenuReadFile,
	MenuInsertFile,
	MenuSaveFile,
	MenuWriteFile,
	(void (*)()) DeleteWindow, 
	0
};

static void (*f_window []) () = { 
	NewWindow,
	(void (*)()) DeleteWindow,
	DisplayKillBuffer,
	DisplayOpenFiles,
	DisplayBrowser,
	UnmapBrowser,
	0
};

static void (*f_mode []) () = {
	SetTextMode,
	SetCMode,
	SetCCMode,
	0
};

static  void (*f_search []) () = { 
	MenuForwardSearch,
	MenuBackwardSearch,
	MenuQueryReplace,
	MenuGlobalReplace,
	MenuGotoLine,
	0
};

static void (*f_mark []) () = {
	SetMark,
	ExchangePointMark,
	KillRegion,
	CopyRegion,
	PasteRegion,
	0
};

static void (*f_font []) () = { 
	0
};

static void (*f_misc []) () = {
	MakeClass,
	MakeMethod,
	MakeFunction,
	Version,
	Help,
	0
};

void (**func_names []) () = {
	f_file, f_window, f_mode, f_search, f_mark, f_font, f_misc, 0
};

#define M_FONTS 5

/*
**	Function name : ExecMenuFunc
**
**	Description : Les fonctions utilisables a partir dee menus.
**	Input : 
**	Ouput :
*/
void ExecMenuFunc ( vm, item )
	register vm, item;
{
	extern void exit ();

	if ( vm == -1 ) return;

	if ( vm ==  M_FONTS ) {
		ChangeTextFont ( dpy, edwin -> text, font_names [item] );
		return;
	}
	((func_names [vm]) [item]) ( edwin -> text );
	if ( IsLastWindow ( 0 ) == True ) {
		XCloseDisplay ( dpy );
		(void) exit (0);
	}
}

