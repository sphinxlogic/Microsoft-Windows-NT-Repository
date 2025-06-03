#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "my_types.h"
#include "parser.h"
#include "options.h"
#include "libutils.h"
#include "timer.h"
#include "windows.h"

char *cmd_name;
#define USAGE "[-d del] [-f] [-F] [-i] [-q] [-s] [-t] [-x n] [-y n] gl-lib-directory-or-script-name [script-in-lib-or-directory]"

/*******************************************/
main(argc, argv)
    int   argc;
    char *argv[];
/*******************************************/
{
    char err_msg[132], cmd[132], *script_name;

    if (!init_window(&argc,argv)) {
	exit(1);
    }

    cmd_name=argv[0];

    /* I know this is a sleazy way to parse command line args, but I'm
       lazy... */
    while ((argc > 1) && (argv[1][0]=='-')) {
	switch(argv[1][1]) {
	    case 'd':	/* Set minimum delay */
		argc--;
		argv++;
		mindel=atol(argv[1]);
		break;
	    case 'f':	/* Fix up aspect ration as specified in tables */
		fixaspect=TRUE;
		break;
	    case 'F':	/* Try to minimize disruption of default color map */
		color_friendly=TRUE;
		break;
	    case 'i':	/* Install colormap.  Unnecessary on most machines */
		install_cmap=TRUE;
		break;
	    case 'q':	/* Synchronize display.  For debugging. */
		quiet_mode=TRUE;
		break;
	    case 's':	/* Synchronize display.  For debugging. */
		synchronize_display();
		break;
	    case 't':	/* Display each command before executing. */
		trace=TRUE;
		break;
	    case 'x':	/* Set x scale multiplier */
		argc--;
		argv++;
		xaspect=atol(argv[1]);
		break;
	    case 'y':	/* Set y scale multiplier */
		argc--;
		argv++;
		yaspect=atol(argv[1]);
		break;
	    default:
		fprintf(stderr,"Usage: %s %s\n",cmd_name,USAGE);
		exit(1);
	}
	argc--;
	argv++;
    }
    if ((argc < 2) || (argc > 3)) {
	fprintf(stderr,"Usage: %s %s\n",cmd_name,USAGE);
	exit(1);
    }

    if (argc==3)
	script_name=argv[2];
    else
	script_name=NULL;

    /* Init_lib opens the library and finds the first text file, filling
       in program_fname and program_fp appropriately. */
    if (!init_lib(argv[1],script_name,err_msg)) {
	fprintf(stderr,"%s: %s\n",cmd_name,err_msg);
	exit(1);
    }

    exec_line("",err_msg); /* Initialize lastloc */
    while (my_fgets(cmd,132,program_fp) != NULL) {
#ifndef HAS_ASYNC_INPUT
	evt_handler(0);
#endif
	if (!exec_line(cmd,err_msg)) {
	    if (!strcmp(err_msg,"Done"))
		break;
	    fprintf(stderr,"%s: %s\n",cmd_name,err_msg);
	    /* line_no always indicates *next* line to execute */
	    fprintf(stderr,"At line %d in file %s\n",line_no-1,program_fname);
	    cleanup();
	    exit(1);
	}
    }

    sit_for(200);
    cleanup();
    exit(0);

}
