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



#include "main.h"

extern Display		*mydisplay;
extern XEvent		myevent;
extern WinType		main_win,
			menu[],
			fs_win[],
			detail_win;
extern unsigned long 	fg, bg;
extern XSizeHints 	myhint;
extern int		NFS,
			Height, 
			root,
			show_use,
			gray,
			minimize,
			percent,
			absolute,
			upd_interval;
extern long		sec,
			usec,
			b_blocks;
extern float 		fs_perc[];
extern string		fs_name[],
			hname;

#if defined (SUNOS) || defined (TOS) || defined (AIX) || defined (LINUX)
extern struct statfs 	stats[];
# else
# ifdef SVR4
extern struct statvfs 	stats[];
# endif
#endif



char *Menu_Text[MENU_ITEMS]={
		" Update Now ",
		"    Quit    "};



/* *********************************************************************** */
/* ********* toggle the display mode and update the windows ************** */
/* *********************************************************************** */
void toggle_mode ()
{
	int i;

	if (absolute)
		absolute=FALSE;
	else
		absolute=TRUE;
	for (i=0; i<NFS; i++)
		{
		fs_win[i].width=main_win.width-OFF_X*2;
		if (!absolute)
			{
			fs_win[i].width=(int)(fs_win[i].width*
				(((float)stats[i].f_blocks)/((float)b_blocks)));
			if (!fs_win[i].width)
				fs_win[i].width=1;
			}
		XResizeWindow (mydisplay,fs_win[i].win, fs_win[i].width,
			fs_win[i].height);
		redraw_fs_win (i);
		}
}



/* *********************************************************************** */
/* *********************** redraw the main window ************************ */
/* *********************************************************************** */
void redraw_main_win ()
{
	int i;

	get_fs_stat ();
	XClearWindow (mydisplay, main_win.win);
	for (i=0; i<NFS; i++)
		{
		XDrawImageString (mydisplay, main_win.win, main_win.gc, 
			OFF_X+2, fs_win[i].y-2, fs_name[i], strlen(fs_name[i]));
		redraw_fs_win (i);
		if (percent)
			write_percent (i);
		}
} 


/* *********************************************************************** */
/* ******************** write out the detailed info ********************** */
/* *********************************************************************** */
void write_detail (i)
int i;
{
	int c=3, rhs=105;
#if defined (SUNOS) || defined (TOS) || defined (AIX) || defined (LINUX)
	long bsize=stats[i].f_bsize;
#else
	long bsize=stats[i].f_frsize;
#endif
	char s[30];

	XClearWindow (mydisplay, detail_win.win);
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		(((DETAIL_X-OFF_X*2)/2)-(DPC/2*strlen (fs_name[i]))), 
		OFF_Y+MENU_HEIGHT, fs_name[i], strlen (fs_name[i]));

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Total Size", 
		strlen ("Total Size"));
	if (stats[i].f_blocks == -1 || bsize == -1)
		sprintf (s, "Undefined");
	else
		sprintf (s, "%.2f MB", 
		/* *** use this computational order to avoid overflows *** */
		   (((double)(bsize)/(float)(MB)))*(double)(stats[i].f_blocks));
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Space Free", 
		strlen ("Space Free"));
	if (stats[i].f_bfree == -1 || bsize == -1)
		sprintf (s, "Undefined");
	else
		sprintf (s, "%.2f MB",
		/* *** use this computational order to avoid overflows *** */
		(((double)(bsize)/(float)(MB)))*(double)(stats[i].f_bfree));
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

#ifndef TOS
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Space Available", 
		strlen ("Space Available"));
	if (stats[i].f_bfree == -1 || bsize == -1)
		sprintf (s, "Undefined");
	else
		sprintf (s, "%.2f MB",
		/* *** use this computational order to avoid overflows *** */
		(((double)(bsize)/(float)(MB)))*(double)(stats[i].f_bavail));
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));
#endif 

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Block Size", 
		strlen ("Block Size"));
	sprintf (s, "%ld", bsize);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Blocks", strlen ("Blocks"));
	sprintf (s, "%ld", stats[i].f_blocks);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Blocks Free",
		strlen ("Blocks Free"));
	sprintf (s, "%ld", stats[i].f_bfree);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

#ifndef TOS
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Blocks Available",
		strlen ("Blocks Available"));
	sprintf (s, "%ld", stats[i].f_bavail);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));
#endif

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "File Inodes",
		strlen ("File Inodes"));
	sprintf (s, "%ld", stats[i].f_files);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Free Inodes",
		strlen ("Free Inodes"));
	sprintf (s, "%ld", stats[i].f_ffree);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));

#ifdef SVR4
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X, OFF_Y+MENU_HEIGHT*c, "Inodes Available",
		strlen ("Inodes Available"));
	sprintf (s, "%ld", stats[i].f_favail);
	CHECKNA;
	XDrawImageString (mydisplay, detail_win.win, detail_win.gc, 
		OFF_X+rhs, OFF_Y+MENU_HEIGHT*c++, s, strlen (s));
#endif
}



/* *********************************************************************** */
/* ************************* write the percentage ************************ */
/* *********************************************************************** */
void write_percent(i)
int i;
{
	char 	s[30];
	int	c=0;

	sprintf (s, "%d%%", (int)(fs_perc[i]*100));
	XDrawImageString (mydisplay, main_win.win, main_win.gc, 
		(main_win.width-OFF_Y-(strlen(s)*DPC)), NFS_TEXT_Y, 
		s, strlen (s));
}



/* *********************************************************************** */
/* *************************** redraw the bars *************************** */
/* *********************************************************************** */
void redraw_fs_win(i)
int i;
{
	/* *** now draw bar *** */
	XClearWindow (mydisplay, fs_win[i].win);
	XFillRectangle (mydisplay, fs_win[i].win, fs_win[i].gc, 
		0, 0, (int)(fs_perc[i]*fs_win[i].width), 
		fs_win[i].height);
}



/* *********************************************************************** */
/* ********************** get the file system stats ********************** */
/* *********************************************************************** */
void get_fs_stat()
{
	int i, t, x;
#if defined (SVR4) || defined (LINUX)
	string s;
#endif

 	/* *** read file system status *** */
        for (i=0; i<NFS; i++)
	   {
#if defined (SUNOS) || defined (LINUX) || defined (AIX)
           t=statfs (fs_name[i], &stats[i]);
# else
# ifdef SVR4
           t=statvfs (fs_name[i], &stats[i]);
#  else
#  ifdef TOS
           t=statvfs (fs_name[i], &stats[i]), sizeof (struct statfs), 0);
#  endif
# endif
#endif

	/* *** see if the return code is an error *** */
	   if (t == -1)
#if defined (SVR4) || defined (LINUX)
		{
		sprintf (s, "Can't get status on [%s]\n", fs_name[i]);
                perror (s);
		}
#else
                perror ("Can't get status on [%s]\n", fs_name[i]);
#endif 
	   else
		{
#ifdef DEBUG
                printf ("Got status on %s\n", fs_name[i]);
#endif
		/* *** find largest file system size *** */
		if (stats[i].f_blocks > b_blocks)
			b_blocks=stats[i].f_blocks;

		/* *** throw out file systems of size 0 *** */
		if (stats[i].f_blocks == 0)
			{
			printf ("File system %s is of size 0 - removing it!\n",
				fs_name[i]);
			for (x=i; x<NFS; x++)
				{
				strcpy (fs_name[x], fs_name[x+1]) ;
				stats[x]=stats[x+1];
				}
			i--;
			NFS--;
			}
		else
			{
			/* *** figure out percentage of free blocks *** */
			if (root)
			   {
	   		   if (show_use)
				{
	   			fs_perc[i]=((float)
				   (stats[i].f_blocks-stats[i].f_bfree))/
				   ((float)stats[i].f_blocks);
#ifdef DEBUG
	   			printf ("Free: %ld \t Total: %ld == %2f%%\n", 
				   stats[i].f_bfree, stats[i].f_blocks, 
				   fs_perc[i]);
#endif
				}
	   		   else
				{
	   			fs_perc[i]=((float)stats[i].f_bfree)/
					((float)stats[i].f_blocks);
#ifdef DEBUG
	   			printf ("Free: %ld \t Total: %ld == %.2f%%\n", 
					stats[i].f_bfree, stats[i].f_blocks, 
					fs_perc[i]);
#endif
				}
			   }
			else
			   {
	   		   if (show_use)
				{
				fs_perc[i]=((float)
					stats[i].f_blocks-stats[i].f_bavail)/
					((float)stats[i].f_blocks);
#ifdef DEBUG
	   			printf ("Free: %ld \t Total: %ld == %.2f%%\n", 
					stats[i].f_bfree, stats[i].f_blocks, 
					fs_perc[i]);
#endif
				} 
	   		  else
				{
	   			fs_perc[i]=((float)stats[i].f_bavail)/
			   		((float)stats[i].f_blocks);
#ifdef DEBUG
	   			printf ("Free: %ld \t Total: %ld == %.2f%%\n", 
					stats[i].f_bfree, stats[i].f_bavail, 
					fs_perc[i]);
#endif
				}
			   }
			}
		}
	}
	/* *** No file systems left *** */
	if (NFS < 1)
		{
		fprintf(stderr,"No File systems left ... Exiting\n");
		exit (0);
		}
}


/* *********************************************************************** */
/* *********************** create a single window ************************ */
/* *********************************************************************** */
void create_window (parent_win, this_win, argc, argv)
Window	parent_win; 
WinTypePtr this_win;
int 	argc;
char 	**argv;
{
	/* ***  default program-specified window attributes *** */
	myhint.x = this_win->x; 
	myhint.y = this_win->y; 
	myhint.width = this_win->width;  
	myhint.height = this_win->height;
	myhint.flags = this_win->flags;

	/* *** Create Actual Window *** */
	this_win->win = XCreateSimpleWindow (mydisplay, parent_win, 
		myhint.x, myhint.y, myhint.width, myhint.height,
		this_win->line_thick, fg, bg);

	if (!this_win->win)
		{
		fprintf (stderr,"Error creating Simple Window %s ... Exiting\n",
			this_win->text);
		exit (1);
		}
#if DEBUG
	fprintf (stdout, "Created Simple Window %s (%d)\n", 
		this_win->text, this_win->win);
#endif

	/* *** cheap way to set the main properties *** */
	XSetStandardProperties(mydisplay, this_win->win, this_win->text,
		this_win->text, None, argv, argc, &myhint);

	/* *** set Graphics Content creation and initialize *** */
	this_win->gc = XCreateGC (mydisplay, this_win->win, 0, 0);

	if (! this_win->gc)
		{
		 fprintf (stderr, "Error creating GC for  %s ... Exiting\n",
                        this_win->text);
                exit (1);
		}
#if DEBUG
	fprintf (stdout, "Created GC %d\n", this_win->gc);
#endif

	XSetBackground (mydisplay, this_win->gc, bg);
	XSetForeground (mydisplay, this_win->gc, fg);

	/* *** specify which input we want this window to process *** */
	XSelectInput (mydisplay, this_win->win, this_win->event_mask);
}




/* *********************************************************************** */
/* highlight menu item (window) in response to a mouse entering or leaving */
/* *********************************************************************** */
int highlight_menu (menu, menu_num,highlight)
WinType menu[];
int	menu_num;
int	highlight;
{
	int i, state=NOGOOD;

	/* *** figure out which menu *** */
	for (i=0; i<menu_num; i++) 
		if (myevent.xcrossing.window==menu[i].win) 
			{
			state=i;
			i=menu_num;
			}
	/* *** check if menu is valid and change fg & bg *** */
  	if (state>=0 && state<menu_num)
		{
		if (highlight)
			{
#if DEBUG
			fprintf (stdout, "Enter state in button %s\n", 
				menu[state].text);
#endif
			XSetBackground (mydisplay, menu[state].gc, fg);
			XSetForeground (mydisplay, menu[state].gc, bg);
			}
		else
			{
#if DEBUG
			fprintf (stdout, "Exit state in button %s\n", 
				menu[state].text);
#endif
			XSetBackground (mydisplay, menu[state].gc, bg);
			XSetForeground (mydisplay, menu[state].gc, fg);
			}
	 	for (i=0; i<menu_num; ++i)
    			XDrawImageString (mydisplay, menu[i].win, menu[i].gc, 1,
			   LETTER_HEIGHT, menu[i].text, strlen(menu[i].text)); 
		return (state);
		}
	else
		return (NOGOOD);
}




/* *********************************************************************** */
/* ************** redraw the string in an exposed window ***************** */
/* *********************************************************************** */
int expose_win (menu, menu_num)
WinType	menu[];
int	menu_num;
{
	int i;

	for (i=0; i<menu_num; i++)
		{
		if (myevent.xexpose.window == menu[i].win)
			{
			XDrawImageString (mydisplay, menu[i].win, 	
			   menu[i].gc, 1, LETTER_HEIGHT, menu[i].text,
			   strlen(menu[i].text));
			return (FALSE);
			}
		}
	return (TRUE);
}




/* *********************************************************************** */
/* *********************** destroy an entire menu  *********************** */
/* *********************************************************************** */
void destroy_menu (menu, menu_num)
WinType	menu[];
int	menu_num;
{
	int i;

	for (i=0; i<menu_num; i++)
		{
		XDestroyWindow (mydisplay, menu[i].win);
#if DEBUG
		fprintf (stdout, "Destroyed menu: %s\n", menu[i].text);
#endif
		XFreeGC (mydisplay, menu[i].gc);
#if DEBUG
		fprintf (stdout, "Destroyed GC: %d\n", menu[i].gc);
#endif
		}
}




/* *********************************************************************** */
/* *************** return the number of the button pressed *************** */
/* *********************************************************************** */
int which_button_pressed (menu, menu_num)
WinType	menu[];
int	menu_num;
{
	int i;

	for (i=0; i<menu_num; i++)
		if (myevent.xbutton.window == menu[i].win)
			return (i);
	return (NOGOOD);
}



/* ***************************************************************** */
/* ****************** Process the progrma arguments **************** */
/* ***************************************************************** */
void process_args (argc, argv)
int argc;
char **argv;
{
	int i, x, t, removed=FALSE;

	for (i=1; i<argc; i++)
	   if (strcmp (argv[i], "-r") == NULL)
		{
		root=TRUE;
#ifndef TOS
		printf ("-r falg caught - will give statistics for root\n");
#else
		fprintf (stderr, "-r flag not available on TOS\n");
#endif
		}
	   else
	   if (strcmp (argv[i], "-b") == NULL)
		{
		gray=FALSE;
		printf ("-b flag caught - will draw bars black\n");
		}
	   else
	   if (strcmp (argv[i], "-m") == NULL)
		{
		minimize=TRUE;
		printf ("-m flag caught - will minimize window size\n");
		}
	   else
	   if (strcmp (argv[i], "-p") == NULL)
		{
		percent=FALSE;
		printf ("-p flag caught - will toggle percent display off\n");
		}
	   else
	   if (strcmp (argv[i], "-a") == NULL)
		{
		absolute=TRUE;
		printf ("-a flag caught - will display sizes relative to \
absolute FS size\n");
		}
	   else
	   if (strcmp (argv[i], "-k") == NULL)
		{
		percent=FALSE;
		printf ("-k flag caught - will keep file ssytem opf file 0\n");
		}
	   else
	   if (strcmp (argv[i], "-s") == NULL)
		{
		i++;
		if (atoi(argv[i]) < 1 || atoi(argv[i]) > MAX_SLEEP_SCALE)
			{
			fprintf(stderr,
				"Supplied scaling factor out of range (1-%d)\n",
				MAX_SLEEP_SCALE);
			}
		else
			{
			usec*=(long)atoi(argv[i]);
			if (usec >= MILLION)
				{
				sec = usec/MILLION;
				usec = (usec-(sec*MILLION));
				}
			else
				{
				sec = 0;
				usec = usec;
				}
			printf ("-s flag caught - will multiply sleep \
time by %d\n", atoi(argv[i]));
			printf ("\tWarning: response may be unacceptably \
slow!\n");
			}
		}
	   else
	   if (strcmp (argv[i], "-f") == NULL)
		{
		show_use=FALSE;
		printf("-f flag caught - will show space free rather than \
space used\n");
		}
	   else
	   if (strcmp (argv[i], "-i") == NULL)
		{
		upd_interval=atoi(argv[++i]);
		if (upd_interval < 1)
		   {
		   fprintf (stderr,
			"Supplied interval (%d) too small (minimum=1)\n",
			upd_interval);
		   exit (1);
		   }
		printf ("-i flag caught - update interval changed to %d sec\n",
		   upd_interval);
		}
	   else
	   if (strcmp (argv[i], "-d") == NULL)
		/* *** does it begin with an "/" *** */
		while (i<(argc-1) && argv[i+1][0]=='/')
		   {
		   /* *** check all file systems for arg *** */
		   for (x=0; x<NFS; x++)
			/* *** we found a match *** */
			if (strcmp (argv[i+1], fs_name[x]) == NULL)
				{
				/* *** shift files systenms 1 down *** */
				for (t=x; t<NFS; t++)
					strcpy (fs_name[t], fs_name[t+1]);
				x=--NFS;
				removed=TRUE;
				printf ("-d flag caught - %s will be ignored\n",
					argv[i+1]);
				}
		   /* *** did we remove a file system with this arg *** */
		   if (x==NFS)
			fprintf (stderr,"%s is not a file system ... ignored\n",
				argv[i]);
		   i++;
		   }
	   else
		printf ("Argument %s not recognized as an option ... ignored\n",
			argv[i]);

	if (NFS < 1)
		{
		fprintf(stderr,"No File systems left ... Exiting\n");
		exit (0);
		}
#ifdef DEBUG
	if (removed)
		{
		printf ("The following file systems remain:\n");
		for (x=0; x<NFS; x++)
			printf ("%s\n", fs_name[x]);
		}
#endif
}


void do_help (prog)
char *prog;
{
	PRINT_COPYRIGHT
	printf ("\n\
%s displays a list of bar graphs for the file systems of the host you are on.\n\
Clicking on a file system gives you detailed information - clicking on it \n\
again closes the detail window. Use %c or the right mouse button to toggle \n\
between absolute and relative display modes.\n\
The following switches are supported:\n", prog, TOGGLE_KEY);
#ifndef TOS
printf ("	-r              graphs space available to root rather \
than user\n");
#endif
printf ("	-b		 draw graphs black\n\
	-m		minimize window size\n\
	-p		don't display percent\n\
	-a		toggle display mode to absolute bar size\n\
	-f              displays percent free rather then percent in use\n\
	-s <arg> 	scale sleep time by argument\n\
        -i <arg>        interval at which display is updated (default = 300s)\n\
        -d <arg1 ... argn> do not include these file systems\n\
        -?              get this description\n");
	PRINT_KNOWN_BUGS
	exit (0);
}



/* **************************************************************** */
/*  msleep will wait for n microseconds for a dummy I/O descriptor  */
/*  **** take from Tom Boutell's Broken Throne with permission **** */
/* **************************************************************** */
void msleep ()
{
	struct timeval  sleept;
		
	sleept.tv_sec = sec;
	sleept.tv_usec = usec; 
	select(FD_SETSIZE, NULL, NULL, NULL, &sleept);
}




/* ***************************************************************** */
/* ********** this basically inits the window definitions ********** */
/* ***************************************************************** */
void init_all_windows ()
{
        int i;
	char s[30];
	unsigned long 	small_event_mask, big_event_mask, fs_mask, these_flags;

        these_flags = PPosition | PSize;
	small_event_mask= PPosition | PSize | StructureNotifyMask | 
		ExposureMask| KeyPressMask | EnterWindowMask | LeaveWindowMask;
        big_event_mask=ButtonPressMask | ExposureMask | EnterWindowMask 
		| LeaveWindowMask;
	fs_mask=ButtonPressMask | ExposureMask;
	sprintf (s, "%s", hname);
#ifndef TOS
	if (root)
		sprintf (s, "%s root", s);
	else
		sprintf (s, "%s user", s);
#endif

	if (show_use)
		sprintf (s, "%s used", s);
	else
		sprintf (s, "%s free", s);
		
	strcpy (main_win.text, s);
        main_win.x=WIN_X;
        main_win.y=WIN_Y;
	if (minimize)
		{
        	main_win.width=MIN_WIN_X;
        	main_win.height=((MIN_INTERVAL*NFS)+BEGIN_NFS);
		}
	else
		{
        	main_win.width=WIN_X;
        	main_win.height=((INTERVAL*NFS)+BEGIN_NFS);
		}
        main_win.line_thick=4;
        main_win.flags=these_flags;
        main_win.event_mask=small_event_mask;
        for (i=0; i<MENU_ITEMS; i++)
                {
                strcpy (menu[i].text, Menu_Text[i]);
		menu[i].x=((main_win.width-SM_MENU_WIDTH)/2);
                menu[i].y=MENU_Y+MENU_HEIGHT*i;
                menu[i].width=SM_MENU_WIDTH;
                menu[i].height=MENU_HEIGHT;
                menu[i].line_thick=0;
                menu[i].event_mask=big_event_mask;
                menu[i].flags=these_flags;
                }
        for (i=0; i<NFS; i++)
                {
		fs_win[i].x=OFF_X;
		if (minimize)
			{
                	fs_win[i].y=((MIN_INTERVAL*i)+BEGIN_NFS);
                	fs_win[i].height=MIN_MENU_HEIGHT;
			}
		else
			{
                	fs_win[i].y=((INTERVAL*i)+BEGIN_NFS);
                	fs_win[i].height=MENU_HEIGHT;
			}
        	fs_win[i].width=main_win.width-OFF_X-OFF_X; 
		if (!absolute) 
			{
			fs_win[i].width=(int)(fs_win[i].width*
				(((float)stats[i].f_blocks)/((float)b_blocks)));
			if (!fs_win[i].width)
				fs_win[i].width=1;
			}
                fs_win[i].line_thick=1;
                fs_win[i].event_mask=fs_mask;
                fs_win[i].flags=these_flags;
                }
	sprintf (s, "%s detail", hname);
	strcpy (detail_win.text, s);
	detail_win.x=OFF_X;
	detail_win.y=OFF_Y;
	detail_win.width=DETAIL_X;
	detail_win.height=DETAIL_Y;
	detail_win.line_thick=4;
	detail_win.event_mask=small_event_mask;
	detail_win.flags=these_flags;
}
