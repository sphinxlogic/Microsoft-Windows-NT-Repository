/*
 *	xkal2xremind.c : Prints weekly or one-shot reminders in xremind
 *		format, minus 5 minutes from the scheduled time. Output
 *		to given file or ~/.xremind.
 *
 *	George Ferguson, ferguson@cs.rochester.edu,  19 Feb 1991.
 *
 *	$Id: xkal2xremind.c,v 1.2 91/03/13 13:31:51 ferguson Exp $
 *
 */
#ifndef lint
static char rcs_id[] = "$Id: xkal2xremind.c,v 1.2 91/03/13 13:31:51 ferguson Exp $";
#endif
#include <stdio.h>
#include <X11/Intrinsic.h>
#include "app-resources.h"
#include "db.h"
#include "date-strings.h"
extern char *getenv();
extern int errno;

/*
 * Functions declared in this file
 */
int main();
static void syntax();

/*
 * Data declared in this file
 */
char *program;
AppResources appResources;
int appointsChanged = False;		/* need this for linkage */

/*	-	-	-	-	-	-	-	-	*/

int
main(argc, argv)
int argc;
char **argv;
{
    FILE *fp;
    char *s,output[1024];		/* how big shall we be? */
    int dow,err;
    Yearrec *yp;
    Monrec *mp;
    Dayrec *dp;
    Msgrec *xp;

    program = *argv;
    XtToolkitInitialize();	/* gotta have this, although no display */
    getResources(&argc,argv);
    if (argc == 2) {
	strcpy(output,argv[1]);
    } else if (argc == 1) {
	strcpy(output,"");
	if ((s=getenv("HOME")) != NULL) {
	    strcpy(output,s);
	    strcat(output,"/");
	}
	strcat(output,".xremind");
    } else {
	exit(1);
    }
    initDb();
    initDateStrings();
    if (appResources.systemAppoints && *appResources.systemAppoints)
	readDb(appResources.systemAppoints,True);
    if (appResources.personalAppoints && *appResources.personalAppoints)
	readDb(appResources.personalAppoints,False);
    appResources.outputFormat = "%n%/%d%/%Y%w %t ";
    if (strcmp(output,"-") == 0) {
	fp = stdout;
    } else if ((fp=fopen(output,"w")) == NULL) {
	err = errno;
	fprintf(stderr,"%s: ",program);
	errno = err;
	perror(output);
	exit(1);
    }
    for (dow = 0; dow < 8; dow++)
      for (yp = Db[dow]; yp != NULL; yp = yp->next)
	for (mp = yp->mons; mp != NULL; mp = mp->next)
	    for (dp = mp->days; dp != NULL; dp = dp->next)
		for (xp = dp->msgs; xp != NULL; xp = xp->next)
		    if (((!dow && xp->year && xp->month && xp->day) ||
		    	 (dow && !xp->year && !xp->month && !xp->day)) &&
			xp->hour != -1) {
			if (xp->hour == 0) {
			    xp->mins = 55;
			    xp->hour = 23;
			} else if (xp->mins == -1) {
			    xp->mins = 55;
			    xp->hour -= 1;
			} else if (xp->mins < 5) {
			    xp->mins = 55 + xp->mins;
			    xp->hour -= 1;
			} else {
			    xp->mins -= 5;
			}
			writeAppoint(fp,xp);
		    }
    fclose(fp);
    exit(0);
}

#ifdef USE_ALERT
/*
 * alert() : This function is defined so that db.c will link happily,
 *	although it will never be called since xkal2xremind runs in non-
 *	interactive mode.
 */
/*VARARGS*/
void
alert()
{
    /* notused */
}
#endif

static void
syntax()
{
    fprintf(stderr,"usage: %s [file]  (default is ~/.xremind)\n",program);
}
