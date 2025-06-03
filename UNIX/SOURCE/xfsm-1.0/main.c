/* ************************************************************************* *
   xfsm - (C) Copyright 1993 Robert Gasch (rgasch@nl.oracle.com)

   Permission to use, copy, modify and distribute this software for any 
   purpose and without fee is hereby granted, provided that this copyright
   notice appear in all copies as well as supporting documentation. All
   work developed as a consequence of the use of this program should duly
   acknowledge such use.

   No representations are made about the suitability of this software for
   any purpose. This software is provided "as is" without express or implied 
   warranty.

   All commercial uses of xfsm must be done by agreement with the autor.
 * ************************************************************************* */



/* *************************************************************************
   This program displays a list of bar graphs for the file systems of the 
   machine you are on. The following switches are supported:
	-r		displays space available to root rather than user
				not available on TOS
	-b		draw bars black
	-m		minimize window size
	-p		don't display percent
	-a		toggle display mode to absolute bar size
	-f		displays percent free rather than percent in use
	-s <arg>	scale sleep interval by <arg>
	-i <arg>	interval at which display is updated (default = 300s)
	-d <arg1 ... argn> do not include these file systems
	-?		get this description


   Version History:
	24-3-93: 1.00 
	29-3-93: 1.01 
			Added support for gray filling of graphs
	10-4-93: 1.10 
			Added resize support, detail window title,
			fixed interval handling in msleep delay loop,
			added -m, -p and -s option
	12-4-93: 1.11
			Fixed resize bug which resulted in graph
			heights being too small
	14-4-93: 1.20
			Fixed percent display placement, added -t flag 
			and toggle handling
	16-4-93: 1.21
			Added MB line for detail window
	18-4-93: 1.22
			Fixed overflow problem for MB line (detail window)
			for very large file systems, adapted sleep 
			handling to allow larger scaling of sleep time
	19-4-93: 1.23
			Added LINUX ifdefs
   ************************************************************************* */


#include "main.h"

Display			*mydisplay;
WinType			main_win,
			menu[MENU_ITEMS],
			fs_win[MAXFS],
			detail_win;
XGCValues		gray_gc_val;
XEvent			myevent;
unsigned long		fg, bg;
KeySym  		mykey;
XWindowAttributes  	attribs;
XSizeHints		myhint;
int 			done,
			show_use=TRUE,
			upd_interval=300,
			NFS=0,
			detail_open=NOGOOD,
			minimize=FALSE,
			percent=TRUE,
			absolute=FALSE;
			gray=TRUE;
long			usec=SLEEPT,
			sec=0,
			b_blocks=0;
float			fs_perc[MAXFS];
string  		fs_name[MAXFS],
			hname;

#ifdef TOS
			root=TRUE;
#else
			root=FALSE;
#endif

#if defined (SUNOS) || defined (TOS) || defined (AIX) || defined (LINUX)
struct statfs 		stats[MAXFS];
# else
# ifdef SVR4
struct statvfs		stats[MAXFS];
# endif
#endif



main (argc, argv)
int argc;
char **argv;
{
	int			i=0, t, myscreen, scrdepth;
	unsigned long int 	gray_mask;
	FILE 			*fp;

	/* *** see if we catch a help flag *** */
	for (i=1; i<argc; i++)
		if (HELPARG)
			do_help(argv[0]); 

	/* *** Open Pipe to appropriate command *** */
	if ((fp=popen (GET_HOSTNAME, "r")) == NULL)
		{
		fprintf (stderr, "Error opening pipe: %s ... Exiting\n",
			GET_HOSTNAME);
		exit (1);
		}
	if (fgets(hname,MAXCPL,fp)!=NULL)
		{
		/* *** convert trailing \n to \0 *** */
		char *c;
		if ((c=strchr (hname, '\n'))!=NULL)
			*c='\0';
		printf ("Hostname = %s\n", hname);
		}
	else
		fprintf (stderr, "Can't read hostname ...\n");
	pclose (fp);

	/* *** open pipe to get file system names *** */
	if ((fp=popen (GET_FS, "r")) == NULL)
		{
		fprintf (stderr, "Error opening pipe: %s ... Exiting\n",
			GET_FS);
		exit (1);
		}
#ifdef AIX
	fgets(fs_name[0],MAXCPL,fp);	/* skip header line */
#endif

	/* *** read all file systems *** */
	while (NFS<MAXFS && (fgets(fs_name[NFS++],MAXCPL,fp)!=NULL))
		{
		/* *** convert trailing \n to \0 *** */
		char *c;
		if ((c=strchr (fs_name[NFS-1], '\n'))!=NULL)
			*c='\0';
		printf ("%s\n", fs_name[NFS-1]);
		}
	if (NFS==MAXFS)
	   	{
	   	printf ("Maximum number of file systems (%d) exceeded...",
			MAXFS);
	   	printf ("Ignoring Rest\n");
	   	}
	else
		NFS--;	/* *** fix blank line at end *** */
	pclose (fp);
	if (NFS < 1)
		{
		fprintf (stderr, "Mount returned no valid file syetems ... \
exiting\n");
		exit (1);
		}
	printf ("%d file systems found ...\n", NFS);

	/* *** process arguments *** */
	process_args (argc, argv);

	/* *** get the file system statistics *** */
	get_fs_stat();

	/* *** connect to X Server and assign screen *** */
	if ((mydisplay = XOpenDisplay("")) == NULL)
		{
		fprintf (stderr, "Error Opening Display ... Exiting\n");
		exit (1);
		}
	myscreen = DefaultScreen (mydisplay);
	scrdepth = DefaultDepth (mydisplay, myscreen);
	init_all_windows();

	/* *** assign foreground and backgound colors *** */
	bg = WhitePixel (mydisplay, myscreen);
	fg = BlackPixel (mydisplay, myscreen);

	/* *** create Main Program Windows *** */
	create_window (DefaultRootWindow(mydisplay), &main_win, argc, argv);
	myhint.min_width=MIN_WIN_X;
	myhint.min_height=((MIN_INTERVAL*NFS)+BEGIN_NFS);
	myhint.flags=PMinSize;
	XSetStandardProperties (mydisplay, main_win.win, 0, 0, None, argv, 
		argc, &myhint);
	
	/* *** create all the buttons using child windows *** */
 	for (i=0; i < MENU_ITEMS ; ++i) 
		create_window (main_win.win, &menu[i], argc, argv); 
	for (i=0; i < NFS; i++)
		{
		create_window (main_win.win, &fs_win[i], argc, argv); 
		if (gray)
			{
			gray_gc_val.tile=XCreatePixmapFromBitmapData (mydisplay,
				fs_win[i].win, gray1_bits, gray1_width, 
				gray1_height, fg, bg, scrdepth);
			if (!gray_gc_val.tile)
				{
				fprintf (stderr, "Couldn't create gray tile \
... escaping to black fill\n");
				gray=FALSE;
				}
			else
				{
				gray_gc_val.fill_style=FillTiled;
				gray_mask= GCFillStyle | GCTile;
				XChangeGC (mydisplay, fs_win[i].gc, gray_mask, 
					&gray_gc_val);
				}
			}
		}

	/* *** Map window definitions onto screen *** */
	XMapRaised (mydisplay, main_win.win);
	XMapSubwindows (mydisplay, main_win.win);

#if DEBUG
	fprintf (stdout, "Setup is Done!\n");
#endif

/* ************************************************************ */
/* ************** setup is done - main event loop ************* */
/* ************************************************************ */

	do_event_loop (argc, argv);

	/* *** we are done - destroy all the windows *** */
	if (detail_open != NOGOOD)
		{
		XFreeGC (mydisplay, detail_win.gc);
		XDestroyWindow (mydisplay, detail_win.win);
		}
	destroy_menu (menu, MENU_ITEMS);
	destroy_menu (fs_win, NFS);
	XFreeGC (mydisplay, main_win.gc);
	XDestroyWindow (mydisplay, main_win.win);
	XCloseDisplay (mydisplay);
	exit (0);
}



void do_event_loop (argc, argv)
int argc;
char **argv;
{
	XEvent 	tevent;
	int	count, cont, this_item, i, t, ppnfs; 
	long	target=time(NULL)+upd_interval;
	char	text[10];

	done=FALSE;
	while (!done)
		{
		/* *** here we wait while there are no events *** */
		/* *** and count out down our interval. We    *** */
		/* *** wait .25 seconds each turn to save CPU *** */
		while (XPending (mydisplay) == 0)
		{
		msleep ();
		if ((time(NULL)) >= target)
			{
#ifdef DEBUG
			printf ("Update interval finished ... updating\n");
#endif
			redraw_main_win ();
			if (detail_open != NOGOOD)
				write_detail (detail_open);
			target=target+upd_interval;
			}
		}
  		/* read the next event */
  		XNextEvent (mydisplay, &myevent);
  		switch (myevent.type)
  		    {
		    /* *** expose event -> redraw the window *** */
		    case Expose:
			/* ********************************************* *
			 *  since a window can generate multiple expose  *
			 *  events we can collapse these into one redraw *
			 * ********************************************* */
			count=0;
			do
				{
				XPeekEvent (mydisplay, &tevent);
				if (tevent.xexpose.window == 
				    myevent.xexpose.window)
					{
					XNextEvent (mydisplay, &myevent);
					count++;
					}
				}
			while(tevent.xexpose.window==myevent.xexpose.window);
	
#if DEBUG
			if (!count)
				fprintf(stdout, 
					"got expose event in window: %d\n",
                			myevent.xexpose.window);
			else
				
				fprintf(stdout, 
					"Compressed %d expose events into 1 \
in window: %d\n", count, myevent.xexpose.window);
#endif

			/* ******************************************* *
			 * ** now deal with the actual expose event ** *
			 * ******************************************* */
  			if (myevent.xexpose.window == main_win.win)
			   	{
				for (i=0; i<NFS; i++)
					{
					XDrawImageString (mydisplay, 
					   main_win.win, main_win.gc, OFF_X+2,
					   fs_win[i].y-2, fs_name[i], 
					   strlen(fs_name[i]));
					if (percent)
						write_percent (i);
					}
				break;
				}
			else
			if (myevent.xexpose.window == detail_win.win)
				write_detail (detail_open);
			cont=TRUE;
			cont = expose_win (menu, MENU_ITEMS);
			if (!cont) break;
			for (i=0; i<NFS; i++)
				if (myevent.xexpose.window == fs_win[i].win)
					{
					redraw_fs_win (i);
					i=NFS;
					}
			break;

		    /* *** change work window if main window changes *** */
		    case ConfigureNotify:
#if DEBUG
			fprintf(stdout, 
				"got configure notify event in window: %d\n",
                		myevent.xconfigure.window);
#endif
			if (myevent.xconfigure.window != main_win.win)
				break;
			if (myevent.xconfigure.width==main_win.width &&
			    myevent.xconfigure.height==main_win.height)
				break;

			/* *** figure out how many points per nfs we have *** */

			ppnfs=((myevent.xconfigure.height-BEGIN_NFS)/NFS);
#if DEBUG
			fprintf (stdout, "got resize event - new interval = \
%d\nwidth = %d height = %d\n", ppnfs, myevent.xconfigure.width,
myevent.xconfigure.height);
#endif
			XClearWindow (mydisplay, main_win.win);
			main_win.width=myevent.xconfigure.width;
			main_win.height=myevent.xconfigure.height;
			XResizeWindow (mydisplay, main_win.win, main_win.width,
				main_win.height);
			for (i=0; i<MENU_ITEMS; i++)
				{
				menu[i].x=((myevent.xconfigure.width-
				    SM_MENU_WIDTH)/2); 
				XMoveWindow (mydisplay, menu[i].win,
				    menu[i].x, menu[i].y);
				}
			for (i=0; i<NFS; i++)
				{
				fs_win[i].y=(BEGIN_NFS+(ppnfs*i));
				fs_win[i].width=
					myevent.xconfigure.width-OFF_X*2;
				if (!absolute)
					{
					fs_win[i].width=(int)(fs_win[i].width*
						(((float)stats[i].f_blocks)/
						((float)b_blocks)));
					if (!fs_win[i].width)
						fs_win[i].width=1;
                        		}
				fs_win[i].height=ppnfs-LETTER_SPACE-
					FREE_LETTER_SP;
				XClearWindow (mydisplay, fs_win[i].win);
				XMoveResizeWindow (mydisplay, fs_win[i].win,
					OFF_X, fs_win[i].y, fs_win[i].width,
					fs_win[i].height);
				XFillRectangle (mydisplay, fs_win[i].win, 
					fs_win[i].gc, 0, 0, (int)
					(float)(fs_perc[i]*fs_win[i].width), 
					fs_win[i].height); 
				XDrawImageString (mydisplay, main_win.win, 
					main_win.gc, OFF_X+2, fs_win[i].y-2, 
					fs_name[i], strlen(fs_name[i]));
				redraw_fs_win(i);
				} 

  			break;

		    /* *** process keyboard mapping changes *** */
		    case MappingNotify:
#if DEBUG
			fprintf(stdout, 
				"got mapping notify event in window: %d\n",
                	   	myevent.xexpose.window);
#endif
  			XRefreshKeyboardMapping ((XMappingEvent *) &myevent );
  			break;

		    /* *** drag in work window *** */
		    case MotionNotify:
#if DEBUG
			fprintf(stdout, 
				"got motion notify event in window: %d\n",
			   	myevent.xbutton.window);
#endif
  			break;


		   /* *** mouse enters a window *** */
		   case EnterNotify:
			this_item = highlight_menu (menu, MENU_ITEMS, 
				TRUE);
			break;

		   /* *** Mouse Leaves a window *** */
		   case LeaveNotify:
			this_item = highlight_menu (menu, MENU_ITEMS, 
				FALSE);
			break;

		   /* *** process mouse-button presses *** */
		   case ButtonPress:
#if DEBUG
			fprintf(stdout, "button press (%d) in window: %d\n",
				myevent.xbutton.button, myevent.xbutton.window);
#endif
			if (myevent.xbutton.button==1 ||
			    myevent.xbutton.button==2)
			   {
			   this_item=
			   	which_button_pressed (menu, MENU_ITEMS);
  			   if (this_item == UPDATE)
				{
				redraw_main_win ();
				if (detail_open != NOGOOD)
					write_detail (detail_open);
				target=time(NULL)+upd_interval;
				break;
				}
			   else
			   if (this_item == QUIT)
				{
				done=TRUE;
				break;
				}

			   /* *** see if btnpress was in a graph window *** */
			   this_item=
				which_button_pressed (fs_win, NFS);
			   if (this_item == NOGOOD)
				break;

			   /* *** open a detail window *** */
			   if (detail_open == NOGOOD)
				{
				create_window (DefaultRootWindow(mydisplay),
					&detail_win, argc, argv);
				myhint.min_width=DETAIL_X;
				myhint.min_height=DETAIL_Y;
				myhint.max_width=DETAIL_X;
				myhint.max_height=DETAIL_Y;
				myhint.flags=PMinSize|PMaxSize;
				XSetStandardProperties (mydisplay, 
					detail_win.win, 0, 0, None, argv, 
					argc, &myhint);
				write_detail (this_item);
				XMapRaised (mydisplay, detail_win.win);
				detail_open = this_item;
				}
			   /* *** det_win for this graph open - close it *** */
			   else
			   if (detail_open == this_item)
				{
				XFreeGC (mydisplay, detail_win.gc);
				XDestroyWindow (mydisplay, detail_win.win);
				detail_open=NOGOOD;
				}
			   else
				{
				XClearWindow (mydisplay, detail_win.win);
				detail_open=this_item;
				write_detail (this_item);
				}
			   }
			   else
				toggle_mode();

			   break;

		/* *** process mouse-button release *** */
		case ButtonRelease:
#if DEBUG
			fprintf(stdout, "button release in window: %d\n",
				myevent.xbutton.window);
#endif
  			break;


		/* *** process Resize *** */
		case ResizeRequest:
#if DEBUG
			fprintf (stdout, "got resize event \n");
#endif
			break;

		/* *** process keyboard input *** */
		case KeyPress:
#if DEBUG
			fprintf (stdout, "got keypress event in window: %d\n",
                		myevent.xkey.window);
#endif
  			i=XLookupString ((XKeyEvent *)&myevent, text, 
				10, &mykey, 0 );
			if (text[0]==UPDATE_KEY)
				{
				redraw_main_win ();
				if (detail_open != NOGOOD)
					write_detail (detail_open);
				target=time(NULL)+upd_interval;
				}
			else
			if (text[0]==QUIT_KEY)
			   if (detail_open != NOGOOD)
				if (myevent.xkey.window == detail_win.win)
				   {
				   XFreeGC (mydisplay, detail_win.gc);
				   XDestroyWindow (mydisplay, detail_win.win);
				   detail_open=NOGOOD;
				   }
			if (text[0]==TOGGLE_KEY)
				toggle_mode ();

			break;
 		} /* switch (myevent.type) */
	} /* while (done == 0) */
}
