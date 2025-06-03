static char rcsid[] = "@(#)$Id: getarpdate.c,v 5.5 1992/12/12 01:29:26 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.5 $   $State: Exp $
 *
 *			Copyright (c) 1988-1992 USENET Community Trust
 *			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: getarpdate.c,v $
 * Revision 5.5  1992/12/12  01:29:26  syd
 * Fix double inclusion of sys/types.h
 * From: Tom Moore <tmoore@wnas.DaytonOH.NCR.COM>
 *
 * Revision 5.4  1992/11/15  02:18:15  syd
 * Change most of the rest of the BSDs to TZNAME
 * From: Syd
 *
 * Revision 5.3  1992/11/15  02:10:58  syd
 * change tzname ifdef from ndefBSD to ifdef TZNAME on its own
 * configure variable
 * From: Syd
 *
 * Revision 5.2  1992/11/07  19:27:30  syd
 * Symbol change for AIX370
 * From: uri@watson.ibm.com
 *
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** 

**/

#include "headers.h"

#ifdef I_TIME
#  include <time.h>
#endif
#ifdef I_SYSTIME
#  include <sys/time.h>
#endif
#ifdef BSD
#  include <sys/timeb.h>
#endif

#include <ctype.h>

#ifndef	_POSIX_SOURCE
extern struct tm *localtime();
extern struct tm *gmtime();
extern time_t	  time();
#endif

static char *arpa_dayname[] = { "Sun", "Mon", "Tue", "Wed", "Thu",
		  "Fri", "Sat", "" };

static char *arpa_monname[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

#ifdef TZNAME
  extern char *tzname[];
#else
  char *timezone();
#endif

#ifdef _AIX370
#undef ALTCHECK
#endif /* _AIX370 */

char *
get_arpa_date()
{
	/** returns an ARPA standard date.  The format for the date
	    according to DARPA document RFC-822 is exemplified by;

	       	      Mon, 12 Aug 85 6:29:08 MST

	**/

	static char buffer[SLEN];	/* static character buffer       */
	struct tm *the_time;		/* Time structure, see CTIME(3C) */
	time_t	   junk;		/* time in seconds....		 */
	long       tzmin;		/* number of minutes off gmt 	 */
	char	  *tzsign;		/* + or - gmt 			 */

#ifndef TZNAME
# ifndef TZ_MINUTESWEST
	struct timeb	loc_time;

	junk = time((time_t *) 0);
	ftime(&loc_time);
# else /* TZ_MINUTESWEST */
	struct  timeval  time_val;		
	struct  timezone time_zone;

	gettimeofday(&time_val, &time_zone);
	junk = time_val.tv_sec;
# endif /* TZ_MINUTESWEST */

#else /* TZNAME */
	extern	time_t	timezone;
# ifdef ALTCHECK
	extern	time_t	altzone;
# endif

	junk = time((time_t *) 0);	/* this must be here for it to work! */
#endif /* TZNAME */

	the_time = localtime(&junk);
	if (the_time->tm_year < 100)
		the_time->tm_year += 1900;
#ifndef TZNAME
# ifdef TZ_MINUTESWEST
	if (the_time->tm_isdst && time_zone.tz_dsttime != DST_NONE)
		tzmin = - (time_zone.tz_minuteswest - 60);
	else
		tzmin = - time_zone.tz_minuteswest;
# else /* TZ_MINUTESWEST */
	tzmin = the_time->tm_gmtoff / 60;
# endif /* TZ_MINUTESWEST */
#else /* TZNAME */
# ifdef ALTCHECK
	if (the_time->tm_isdst)
		tzmin = - (altzone / 60);
	else
		tzmin = - (timezone / 60);
# else /* ALTCHECK */
	tzmin = - (timezone / 60);
# endif /* ALTCHECK */
#endif /* TZNAME */

	if (tzmin >= 0)
		tzsign = "+";
	else {
		tzsign = "-";
		tzmin = -tzmin;
	}

	sprintf(buffer, "%s, %d %s %d %02d:%02d:%02d %s%02d%02d (%s)",
	  arpa_dayname[the_time->tm_wday],
	  the_time->tm_mday,
	  arpa_monname[the_time->tm_mon], the_time->tm_year,
	  the_time->tm_hour, the_time->tm_min, the_time->tm_sec,
	  tzsign, tzmin / 60, tzmin % 60,
#ifndef TZNAME
#ifdef TZ_MINUTESWEST
# ifdef GOULD_NP1
	  the_time->tm_zone);
# else
	  timezone(time_zone.tz_minuteswest, the_time->tm_isdst));
# endif
#else
	  timezone(loc_time.timezone, the_time->tm_isdst));
#endif
#else
	  tzname[the_time->tm_isdst]);
#endif
	
	return( (char *) buffer);
}
