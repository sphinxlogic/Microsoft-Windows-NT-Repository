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

#include "xcoral.h"
#include "browser.h"
#include "flist.h"

EdWin			*CreateWindow ();
Window			HandleMenu ();
ST			*automate ();
extern Browser		br;

static XEvent           event;
static XEvent           *ev = &event;

static void HandleButtonPress (), HandleConfigure (), HandleEnter (),
	HandleLeave (),	HandleVisibility (), HandleKeyRelease (),
	HandleKeyPress (), HandleExpose (), HandleMap (), HandleMessage ();

extern void exit ();

#define MAX_EVENT 64
void (* events_Handler[MAX_EVENT]) ();
static void IgnoreEvent () {}

/*
**	Function name : InitEvent
**
**	Description : Initialise la table des evennements.
**	Input : 
**	Ouput :
*/
void InitEvent ()
{
	register int i;
	for ( i= 0; i < MAX_EVENT; i++ )
		events_Handler[i] = IgnoreEvent;

	events_Handler [ButtonPress] = HandleButtonPress;
	events_Handler [ConfigureNotify] = HandleConfigure;
	events_Handler [EnterNotify] = HandleEnter;
	events_Handler [LeaveNotify] = HandleLeave;
	events_Handler [VisibilityNotify] = HandleVisibility;
	events_Handler [KeyRelease] = HandleKeyRelease;
	events_Handler [KeyPress] = HandleKeyPress;
	events_Handler [Expose] = HandleExpose;
	events_Handler [MappingNotify] = HandleMap;
	events_Handler [ClientMessage] = HandleMessage;
}

/*
**	Function name : WaitForEvent
**
**	Description : 
**	Input : 
**	Ouput :
*/
void WaitForEvent ()
{
        ev = &event;

	for (;;) {
		XNextEvent ( dpy, &event );
		if ( event.type < MAX_EVENT )
			(* events_Handler [event.type] ) ();
	}
}


/*
**	Function name : HandleMessage
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleMessage ()
{
	register EdWin **t;

	if ( strcmp (XGetAtomName(dpy,ev -> xclient.message_type), "WM_PROTOCOLS") != 0 )
		return;

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue;
		if ( ev -> xclient.window == (*t) -> w_frame ) {
			if ( DeleteWindow ( (*t) -> text ) == 0 ) {
				if ( IsLastWindow (0) == True ) {
					XCloseDisplay ( dpy );
					(void) exit (0);
				}
			}
			break;
		}
	}
	if ( (ev -> xclient.window == GetBrowserFrame ()) 
		|| (ev -> xclient.window == GetBrowserVisit())) {
		UnmapBrowser ();
	}
}

/*
**	Function name : HandleMap
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleMap ()
{
#ifdef DEBUG			
	(void) fprintf ( stderr, "Keyboard Mapping\n" );
#endif
	XRefreshKeyboardMapping ( (XMappingEvent *) ev );
}


/*
**	Function name : HandleConfigure
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleConfigure ()
{
	register EdWin	**t;

#ifdef DEBUG
	( void ) fprintf ( stderr, "Configure %d\n", ev -> xconfigure.window );
#endif

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue;
		if ( ev -> xconfigure.window == ( * t) -> w_frame ) {

		        if ( ((*t) -> width != ev -> xconfigure.width) ||
			     ((*t) -> height != ev -> xconfigure.height) )

			    ConfigWindow ( *t, ev -> xconfigure.width,
				      ev -> xconfigure.height );
#ifdef DEBUG
			( void ) fprintf ( stderr,"width = %d height = %d\n",
				ev -> xconfigure.width, ev -> xconfigure.height  );
#endif
			while ( XCheckWindowEvent ( dpy,
				ev -> xconfigure.window,
				StructureNotifyMask, ev ));

			return;
		}
	}
	if ( ev -> xconfigure.window == GetBrowserFrame () ) {
		ConfigBrowser ( ev -> xconfigure.width,
			ev -> xconfigure.height );
		return;
	}
	if ( ev -> xconfigure.window == GetBrowserVisit () ) {
		ConfigVisitWindow ( ev -> xconfigure.width,
			ev -> xconfigure.height );
		return;
	}
}


/*
**	Function name : HandleEnter
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleEnter ()
{
	register EdWin	**t;

#ifdef DEBUG
	( void ) fprintf ( stderr, "Enter Notify %d mode = %d\n", 
			ev -> xcrossing.window,
			ev -> xcrossing.mode );
#endif

	if ( ev -> xcrossing.mode != NotifyNormal
		&&  ev -> xcrossing.mode != NotifyUngrab )
		return;

	if ( MouseInMenuBar ( dpy, ev -> xcrossing.window ) == True )
		return;

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue; 
		if ( (*t) -> w_frame != ev -> xcrossing.window ) continue;
#ifdef DEBUG
	( void ) fprintf ( stderr, "Changement de contexte \n" );
#endif
		if ( XFindContext ( dpy, ev -> xcrossing.window,
			EdContext, (caddr_t *) &edwin ) == XCNOENT ) {
			(void) fprintf ( stderr,"Context Error\n" );
			(void) exit(1);
		}
		MouseIn ( edwin -> text );
		UnFreeseTextCursor ( edwin -> text );
		TextCursorOn ( edwin -> text );
		ChangeDir ( edwin -> text );
        }
	SetButton ( edwin -> mwin );
}


/*
**	Function name : HandleLeave
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleLeave ()
{
       register EdWin	**t;

#ifdef DEBUG
       (void) fprintf ( stderr, "Leave Notify w = %d mode = %d detail = %d\n",
		       ev -> xcrossing.window, ev -> xcrossing.mode,
		       ev -> xcrossing.detail  );
#endif

       if ( ev -> xcrossing.mode != NotifyNormal )
	      return;

       if ( ev -> xcrossing.detail == NotifyInferior )
	      return;

       for ( t = TWin; t < TWin + MAXWIN; t++ ) {
               if ( *t == 0 ) continue;
	       if ( ev -> xcrossing.window == ( * t) -> w_frame ) {
	        	MouseOut( ( * t ) -> text );
			TextCursorOff (( * t ) -> text );
			FreeseTextCursor (( * t ) -> text );
			DisplayMessage ( (*t)  -> mwin, " "  );
			return;
	       }
       }
}


/*
**	Function name : HandleVisibility
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleVisibility ()
{
	register EdWin	**t;

	if ( SetBrowserVisibility ( ev ) == True )
		return;
	else {
#ifdef DEBUG
	( void ) fprintf ( stderr, "Visibility window = %d state = %d\n",
		ev -> xvisibility.window, ev -> xvisibility.state );
#endif
		for ( t = TWin; t < TWin + MAXWIN; t++ ) {
			if ( *t == 0 ) continue;
			if ( SetTextVisibility ( ev -> xvisibility.window,
				( *t ) -> text, ev -> xvisibility.state ))
				break; 
		}
	}
}


/*
**	Function name : HandleKeyRelease
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleKeyRelease ()
{
	register EdWin	**t;
	
	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue;
#ifdef DEBUG
		( void ) fprintf ( stderr, "Key Release\n" );
#endif
	}			
}


/*
**	Function name : HandleKeyPress
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleKeyPress ()
{
	ST *stat;
#ifdef DEBUG
	fprintf ( stderr, "Key Press Event\n");
#endif

	if ( KeyPressInBrowser ( (XKeyEvent *) ev ) == True )
		return;
	stat = (ST *) automate ( ( XKeyEvent *) ev, edwin -> stat );
	if ( (int) stat != -1 )
		edwin -> stat = stat;

	if ( IsLastWindow ( 0 ) == True ) {
		XCloseDisplay ( dpy );
		(void) exit (0);
	}
}


/*
**	Function name : HandleButtonPress
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleButtonPress ()
{
	register int 	i;
	unsigned int	button;
	int 		vm, item;
	Window 		w_stat;
	int 		result;

#ifdef DEBUG
	( void ) fprintf ( stderr, "ButtonPress %d\n", ev -> xbutton.window );
#endif

	if ( ButtonBrowser ( (XButtonEvent *) ev ) == True )
		return;
	/*
	 * Button press in menus
	 */
	if ( (i = ButtonPressInMenu ( ev -> xbutton.window, edwin -> menu )) != -1 ) {
		w_stat = HandleMenu ( dpy, (XButtonPressedEvent *) ev, edwin->w_frame, edwin->menu, i, &vm, &item );
		ExecMenuFunc ( vm, item );
		if ( w_stat != 0 ) {
			XFindContext ( dpy, w_stat, EdContext,
				(caddr_t *) &edwin );
		}
		return;
	}
	/*
	 * Button press in button window,
	 */
	if ( (i = IsButtonInControl ( ev -> xbutton.window )) >= 0 ) {
		if ( GetCursorStat ( edwin -> text ) != OFF )
	        	TextCursorOff ( edwin -> text );
		ButtonPressInControl ( ev, edwin -> text, i );
		if ( GetCursorStat ( edwin -> text ) != ON )
			TextCursorOn ( edwin -> text );
		return;
	}
	
	/* 
	 * Button press in scroll cursor,
	 */
	if ( ButtonPressInScroll ( edwin -> scroll, ev -> xbutton.window, 
		ev -> xbutton.y, &result ) == True) {
		switch ( result ) {
		case CURSOR:
			if ( GetCursorStat ( edwin -> text ) != OFF )
				TextCursorOff ( edwin -> text );
			if ( GetScrollStat( edwin -> text ))
				GotoLeft ( edwin -> text );
			if ( (TextInBuf ( edwin -> text ) == True) 
				&& ( GetNbLinesInBuf ( edwin -> text ) > 1))			
				HandleScrollBar ( dpy, edwin -> scroll, ScrollNLine );
			if ( GetCursorStat ( edwin -> text ) != ON )
				TextCursorOn ( edwin -> text );
			RefreshScrollBar ( dpy, edwin -> scroll ); 
			break;
		case NEXT:
			TextCursorOff ( edwin -> text );
			NextPage ( edwin -> text  );
			TextCursorOn ( edwin -> text );
			break;
		case PREVIOUS:
			TextCursorOff ( edwin -> text );
			PreviousPage ( edwin -> text );
			TextCursorOn ( edwin -> text );
			break;
		}
		return;
	}

	/*
 	* Button press in current text window
	* Update cursor position and grap pointer until button release.
	*/
	if ( ev -> xbutton.window == edwin -> text -> window ) {

	       button = ev -> xbutton.button;

	       XGrabPointer ( dpy, edwin -> w_frame, True,
		     ButtonReleaseMask, GrabModeAsync, GrabModeAsync,
		     edwin -> w_frame, None, CurrentTime );

		switch ( ev -> xbutton.button ) {
		case Button1:
			TextCursorOff ( edwin -> text );
			(void) MoveToXYinTextWindow ( edwin -> text,
				ev -> xbutton.x, ev -> xbutton.y );
			TextCursorOn ( edwin -> text );
			break;
		case Button2:
			if ( ev -> xbutton.state == ControlMask ) 
				DeleteBytesFromCutBuffer ( edwin -> text );
			else 
				GetBytesFromCutBuffer ( edwin -> text );
			break;
		case Button3:
			TextCursorOff ( edwin -> text );
			XSync ( dpy, False );
			StoreBytesInCutBuffer ( edwin -> text,
				ev -> xbutton.x, ev -> xbutton.y );
			TextCursorOn ( edwin -> text );
			break;
		}
		(void) WaitButtonRelease ( button );
	}
}


/*
**	Function name : WaitButtonRelease
**
**	Description : 
**	Input : 
**	Ouput :
*/
void WaitButtonRelease ( button )
	register unsigned int button;
{
	XEvent event;

	for (;;) {
		XNextEvent ( dpy, &event );
		switch ( event.type ) {
		case ButtonRelease:
			if ( event.xbutton.button == button ) {
				XUngrabPointer ( dpy, CurrentTime );
				return;
			}
		}
	}
}


/*
**	Function name : HandleExpose
**
**	Description : 
**	Input : 
**	Ouput :
*/
static void HandleExpose ()
{
        static XRectangle rec [2];
	static Region region;
	EdWin 		**t;

#ifdef DEBUG
	( void ) fprintf ( stderr, "Expose %d\n", ev -> xexpose.window );
#endif

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue;
		if ( ev -> xexpose.window == ( *t ) -> w_frame 
 			|| ev -> xexpose.window == GetMenuWindowBar ( (*t) -> menu )  ) {
#ifdef DEBUG
			( void ) fprintf ( stderr, "Expose Refreshbar\n" );
#endif
			RefreshMenuBar ( dpy, ( *t ) -> menu );

			while ( XCheckWindowEvent ( dpy,
				ev -> xexpose.window,
				ExposureMask, ev ));
			return;
		}
		
   		if ( ev -> xexpose.window == GetTextWindow ( ( *t ) -> text )) {
      
#ifdef DEBUG
			( void ) fprintf ( stderr, "Expose text window\n" );
#endif
			ExposeTextWindow ( dpy, ( *t ) -> text, ev );
			return;
		}

		if ( ExposeInControlePanel ( ev -> xexpose.window, ( *t ) -> mwin ) == True ) {
#ifdef DEBUG
			( void ) fprintf ( stderr, "Expose control panel\n" );
#endif
			return;
		}

		if ( ExposeInScroll ( dpy, ev -> xexpose.window, ( *t ) -> scroll ) == True ) {
#ifdef DEBUG
     			(void) fprintf ( stderr, "Expose sroll\n");
#endif
			return;
		}
	}
	ExposeBrowser ( ev );
}

