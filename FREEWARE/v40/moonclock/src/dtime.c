/*
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

/* dtime.c - extracted from the phoon/libtws package
*/


#include "tws.h"
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#ifdef  SYSV
extern int  daylight;
extern long timezone;
extern char *tzname[];
#else	SYSV
#include <sys/timeb.h>
#endif	SYSV

static struct zone
    {
    char *std;
    char *dst;
    int shift;
    }
    zones[] = {
	"GMT", "BST", 0,
	"EST", "EDT", -5,
	"CST", "CDT", -6,
	"MST", NULL, -7,
	"PST", "PDT", -8,
	"A", NULL, -1,
	"B", NULL, -2,
	"C", NULL, -3,
	"D", NULL, -4,
	"E", NULL, -5,
	"F", NULL, -6,
	"G", NULL, -7,
	"H", NULL, -8,
	"I", NULL, -9,
	"K", NULL, -10,
	"L", NULL, -11,
	"M", NULL, -12,
	"N", NULL, 1,
#ifndef	HUJI
	"O", NULL, 2,
#else	HUJI
	"JST", "JDT", 2,
#endif	HUJI
	"P", NULL, 3,
	"Q", NULL, 4,
	"R", NULL, 5,
	"S", NULL, 6,
	"T", NULL, 7,
	"U", NULL, 8,
	"V", NULL, 9,
	"W", NULL, 10,
	"X", NULL, 11,
	"Y", NULL, 12,
	NULL };

#ifndef __DECC
long time( );
#endif /* __DECC */
struct tm *localtime( );


struct tws *
dtwstime( )
    {
    long clock;

    (void) time( &clock );
    return ( dlocaltime( &clock ) );
    }


struct tws *
dlocaltime( clock )
long *clock;
    {
    register struct tm *tm;
#ifndef SYSV
    struct timeb tb;
#endif not SYSV
    static struct tws tw;

    if ( clock == NULL )
	return ( NULL );
    tw.tw_flags = TW_NULL;

    tm = localtime( clock );
    tw.tw_sec = tm -> tm_sec;
    tw.tw_min = tm -> tm_min;
    tw.tw_hour = tm -> tm_hour;
    tw.tw_mday = tm -> tm_mday;
    tw.tw_mon = tm -> tm_mon;
    tw.tw_year = tm -> tm_year;
    tw.tw_wday = tm -> tm_wday;
    tw.tw_yday = tm -> tm_yday;
    if ( tm -> tm_isdst )
	tw.tw_flags |= TW_DST;
#ifndef  SYSV
    ftime( &tb );
    tw.tw_zone = -tb.timezone;
#else   SYSV
    tzset( );
    tw.tw_zone = -(timezone / 60);
#endif  SYSV
    tw.tw_flags &= ~TW_SDAY;
    tw.tw_flags |= TW_SEXP;
    tw.tw_clock = *clock;

    return ( &tw );
    }
