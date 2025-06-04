/*
 *   xmcd - Motif(tm) CD Audio Player
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef LINT
static char *_main_c_ident_ = "@(#)main.c	6.22 98/05/07";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/resource.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/cdfunc.h"
#include "libdi_d/libdi.h"


/* Global data */
char			*progname,	/* The path name we are invoked with */
			*cdisplay;	/* Command-line specified display */
appdata_t		app_data;	/* Options data */
widgets_t		widgets;	/* Holder of all widgets */
pixmaps_t		pixmaps;	/* Holder of all pixmaps */
FILE			*errfp;		/* Error message stream */

/* Data global to this module only */
STATIC curstat_t	status;		/* Current CD player status */
STATIC XtAppContext	app_context;	/* Application context */


/***********************
 *   public routines   *
 ***********************/

/*
 * curstat_addr
 *	Return the address of the curstat_t structure.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
curstat_t *
curstat_addr(void)
{
	return (&status);
}


/***********************
 *  internal routines  *
 ***********************/


/*
 * usage
 *	Display command line usage syntax
 *
 * Args:
 *	argc, argv
 *
 * Return:
 *	Nothing.
 */
STATIC void
usage(int argc, char **argv)
{
	int	i;

	(void) fprintf(errfp, "%s\n", app_data.str_badopts);
	for (i = 1; i < argc; i++)
		(void) fprintf(errfp, "%s ", argv[i]);

	(void) fprintf(errfp, "\n\n%s %s [-dev device] [-debug]",
		       app_data.str_usage, argv[0]);

#if defined(SVR4) && defined(sun)
	/* Solaris 2 volume manager auto-start support */
	(void) fprintf(errfp, " [-c device] [-X] [-o]");
#endif

 	(void) fprintf(errfp,
	    "\n\nStandard Xt Intrinsics and Motif options are supported.\n");
}


/*
 * event_loop
 *	Used to handle X events while waiting on I/O.
 *
 * Args:
 *	flag - Currently unused
 *
 * Return:
 *	Nothing.
 */
/* ARGSUSED */
void
event_loop(int flag)
{
	while (XtAppPending(app_context))
		XtAppProcessEvent(app_context, XtIMAll);
}


/*
 * main
 *	The main function
 */
int
main(int argc, char **argv)
{
	int	i;

	/* Error message stream */
	errfp = stderr;

	/* Initialize variables */
	progname = argv[0];

	/* Handle some signals */
	if (signal(SIGINT, onsig) == SIG_IGN)
		(void) signal(SIGINT, SIG_IGN);
	if (signal(SIGHUP, onsig) == SIG_IGN)
		(void) signal(SIGHUP, SIG_IGN);
	if (signal(SIGTERM, onsig) == SIG_IGN)
		(void) signal(SIGTERM, SIG_IGN);

	/* Set SIGCHLD handler to default */
#ifndef __VMS_VER
	(void) signal(SIGCHLD, SIG_DFL);
#else
#if (__VMS_VER >= 70000000) && (__DECC_VER > 50230003)
	/* OpenVMS v7.0 and DECCV5.2 have these defined */
	(void) signal(SIGCHLD, SIG_DFL);
#endif
#endif

	/* Hack: Aside from stdin, stdout, stderr, there shouldn't
	 * be any other files open, so force-close them.  This is
	 * necessary in case xmcd was inheriting any open file
	 * descriptors from a parent process which is for the
	 * CD-ROM device, and violating exclusive-open requirements
	 * on some platforms.
	 */
	for (i = 3; i < 10; i++)
		(void) close(i);

#if (XtSpecificationRelease >= 5)
	/* Set locale */
	XtSetLanguageProc(NULL, NULL, NULL);
#endif

	/* Snoop command line for -display before XtVaAppInitialize
	 * consumes it
	 */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-display") == 0) {
			cdisplay = argv[++i];
			break;
		}
	}

	/* Initialize X toolkit */
	widgets.toplevel = XtVaAppInitialize(
		&app_context,
		"XMcd",
		options, XtNumber(options),
		&argc, argv,
		NULL,
		NULL
	);

	/* Get application options */
	XtVaGetApplicationResources(
		widgets.toplevel,
		(XtPointer) &app_data,
		resources,
		XtNumber(resources),
		NULL
	);
		
	/* Check command line for unknown arguments */
	if (argc > 1) {
		usage(argc, argv);
		exit(1);
	}

	/* Create all widgets */
	create_widgets(&widgets);

	/* Configure resources before realizing widgets */
	pre_realize_config(&widgets);

	/* Display widgets */
	XtRealizeWidget(widgets.toplevel);

	/* Configure resources after realizing widgets */
	post_realize_config(&widgets, &pixmaps);

	/* Register callback routines */
	register_callbacks(&widgets, &status);

	/* Initialize various subsystems */
	cd_init(&status);

	/* Start various subsystems */
	cd_start(&status);

	/* Main event processing loop */
	XtAppMainLoop(app_context);

	exit(0);

	/*NOTREACHED*/
}

