#ifndef lint
static char *rcsid = "$Header: /usr1/ben/jkh/tmp/xinfo/RCS/xinfo.c,v 1.4 91/01/07 14:02:59 jkh Exp Locker: jkh $";
#endif

/*
 *
 *                   Copyright 1989, 1990
 *                    Jordan K. Hubbard
 *
 *                PCS Computer Systeme, GmbH.
 *                   Munich, West Germany
 *
 *
 * This file is part of GNU Info widget.
 *
 * The GNU Info widget is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

/*
 * $Log:	xinfo.c,v $
 * Revision 1.4  91/01/07  14:02:59  jkh
 * info string now correct for non-gcc compilers.
 * 
 * Revision 1.3  90/11/12  18:07:52  jkh
 * Changed startup string to be more palatable to weird CPP's.
 * 
 * Revision 1.2  90/11/11  21:37:15  jkh
 * Release 1.01.
 * 
 * Revision 1.1  90/11/06  22:52:39  jkh
 * Initial revision.
 * 
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <stdio.h>
#include "Info.h"

static char info_msg[] = "\n\
XInfo %2.2f, Copyright (C) 1990 Jordan K. Hubbard.\n\
XInfo is free software and you are welcome to distribute copies of it\n\
under certain conditions; you should have received a copy of these\n\
conditions (see file 'COPYING') with this software. Type '?' for\n\
a list of commands.\n\
";

int main(argc, argv)
int argc;
char **argv;
{
     Arg args[5];
     Cardinal i;
     Widget top;
     static void quit();
     static XtCallbackRec call[] = {
	  { quit, NULL },
	  { NULL, NULL },
     };
     static XrmOptionDescRec options[] = {
	  {"-node",	"*info.infoNode",	XrmoptionSepArg,	NULL},
	  {"-file",	"*info.infoFile",	XrmoptionSepArg,	NULL},
	  {"-path",	"*info.infoPath",	XrmoptionSepArg,	NULL},
	  {"-lpr",	"*info.printCommand",	XrmoptionSepArg,	NULL},
     };


     top = XtInitialize("xinfo", "XInfo", options, XtNumber(options),
			&argc, argv);
     i = 0;
     XtSetArg(args[i], XtNcallback, call);		i++;
     XtCreateManagedWidget("info", infoWidgetClass, top, args, i);

     XtRealizeWidget(top);

     /* Tell the user who we are */
     fprintf(stderr, info_msg, XpInfoVersion);

     /* Spin until we get sick */
     XtMainLoop();

     return(0); /* hope not, but lint likes this */
}

static void quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{
     extern void exit();

     exit(0);
}
