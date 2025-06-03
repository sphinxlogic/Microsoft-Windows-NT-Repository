/*
 *	date-strings.c : Strings used everywhere, parsed from
 *		the app-resources into arrays.
 *
 *	George Ferguson, ferguson@cs.rochester.edu,  27 Feb 1991.
 *
 *	$Id: date-strings.c,v 2.1 91/02/28 11:21:07 ferguson Exp $
 */
#include <stdio.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include "app-resources.h"
extern char *program;

/*
 * Functions defined here:
 */
void initDateStrings();
static void parseStrings();

/*
 * Data defined here:
 */
char *longDowStr[7], *shortDowStr[7], *longMonthStr[12], *shortMonthStr[12];

/*	-	-	-	-	-	-	-	-	*/

void
initDateStrings()
{
    parseStrings(appResources.longDowStrings,longDowStr,7);
    parseStrings(appResources.shortDowStrings,shortDowStr,7);
    parseStrings(appResources.longMonthStrings,longMonthStr,12);
    parseStrings(appResources.shortMonthStrings,shortMonthStr,12);
}

static void
parseStrings(s,array,num)
char *s;
char *array[];
int num;
{
    char buf[64];
    int i,n;

    for (i = 0; i < num; i++) {
	while (isspace(*s))
	    s += 1;
	n = 0;
	while (*s && !isspace(*s) && n < 63)
	    buf[n++] = *s++;
	buf[n] = '\0';
	if (n == 63 && *s && !isspace(*s))
	    fprintf(stderr,"%s: date-string too long: \"%s...\"\n",program,buf);
	array[i] = XtNewString(buf);
    }
}
