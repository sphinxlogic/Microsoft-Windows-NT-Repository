/*
      Copyright (c) 1991, 1992 by Simon Marshall, University of Hull, UK

		   If you still end up late, don't blame me!
				       
  Permission to use, copy, modify, distribute, and sell this software and its
       documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notice appear in all copies and that
	both that copyright notice and this permission notice appear in
			   supporting documentation.
				       
  This software is provided AS IS with no warranties of any kind.  The author
    shall have no liability with respect to the infringement of copyrights,
     trade secrets or any patents by this file or any part thereof.  In no
      event will the author be liable for any lost revenue or profits or
	      other special, indirect and consequential damages.
*/

/* 
 * Parse times.
 */



#include "xalarm.h"
#include "dates.h"

#define		VALUE(ch)	((int) (ch) - (int) '0')
#define		INSTRING(ch, s)	(((ch) != '\0') and (index ((s), (ch))))


long		TimeToMilliSeconds();
Boolean		ParseTimeString();
extern time_t	time();
extern struct tm *localtime();



extern AlarmData xalarm;



/* 
 * Convert the string into milliseconds, or INVALID if 
 * not recognised.
 * Recognises quite a lot, really, but doesn't like non absolute times 
 * having "am" or "pm" etc.
 */	

long TimeToMilliSeconds (timestr)
  String   timestr;
{
    Boolean 	in24hrformat;
    int 	hours, minutes, seconds;
    time_t 	abstime;
    struct tm  *now;

    if (not ParseTimeString (timestr, &hours, &minutes, &in24hrformat))
	return ((long) INVALID);

    if ((*timestr == '+') or (STREQUAL (timestr, NOW)))
	seconds = (hours * SECSIN1HR) + (minutes * SECSIN1MIN);
    else {
	(void) time (&abstime);
	now = localtime (&abstime);
	seconds = ((hours - now->tm_hour) * SECSIN1HR) +
	    ((minutes - now->tm_min) * SECSIN1MIN) - now->tm_sec;
	if ((seconds < 0) and (not in24hrformat))
	    seconds += (12 * SECSIN1HR);
    }

    if ((xalarm.dateout > 0) or ((xalarm.dateout == 0) and (seconds >= 0)))
	return ((long) seconds * MSECSIN1SEC);
    else {
	ADDERROR (TIMEPASSED, (String) NULL);
	return ((long) INVALID);
    }
}



/* 
 * Parse that string.  Robbed and hacked from xcal (3.2) by Peter 
 * Collinson et al.
 */

Boolean ParseTimeString (str, hrs, mins, in24hrf)
  String    str;
  int 	   *hrs, *mins;
  Boolean  *in24hrf;
{
    enum      {Hhmm, hHmm, hh_mm, hhMm, hhmM, AmPm, aMpM} state;
    Boolean   relative, finished = False;
    int       numdigits = 0;
    char     *s = str, badtime[TEXT];
		      
    RESETERROR ();

    if (relative = (*str == '+'))
	str++;
    else if (STREQUAL (str, NOW)) {
	*hrs = *mins = 0;
	return (True);
    } else if (STREQUAL (str, NOON)) {
	*hrs = 12;
	*mins = 0;
	return (*in24hrf = True);
    }

    while (*s != '\0')
	if (isdigit (*s++))
	    numdigits++;

    if (relative)
	switch (numdigits) {
	 case 1: state = hhmM; break;
	 case 2: state = hhMm; break;
	 case 3: state = hHmm; break;
	 case 4: state = Hhmm; break;
	 default:
	    (void) sprintf (badtime, "just %d", numdigits);
	    ADDERROR (WRONGNUMDIGITS, badtime);
	    return (False);
	}
    else
	switch (numdigits) {
	 case 1:
	 case 2:
	 case 4: state = Hhmm; break;
	 case 3: state = hHmm; break;
	 default:
	    (void) sprintf (badtime, "just %d", numdigits);
	    ADDERROR (WRONGNUMDIGITS, badtime);
	    return (False);
	}

    *hrs = *mins = 0;
    *in24hrf = False;

    while (not finished)
	switch (state) {
	 case Hhmm:
	    if (isdigit (*str)) {
		*hrs = VALUE (*str++);
		state = hHmm;
		continue;
	    } else {
		ADDERROR (UNRECOGNISED, str);
		return (False);
	    }
	 case hHmm:
	    if (isdigit (*str)) {
		*hrs = *hrs*10 + VALUE (*str++);
		state = hh_mm;
		continue;
	    }
	 case hh_mm:
	    if (INSTRING (*str, ".:-")) {
		state = hhMm;
		str++;
		continue;
	    }
	 case hhMm:
	    if (isdigit (*str)) {
		*mins = VALUE (*str++);
		state = hhmM;
		continue;
	    }
	 case hhmM:
	    if (isdigit (*str)) {
		*mins = *mins*10 + VALUE (*str++);
		state = AmPm;
		continue;
	    }
	 case AmPm:
	    if (*str == '\0') {
		*in24hrf = (*hrs > 12) or ((*hrs < 10) and (numdigits % 2 == 0));
		finished = True;
		continue;
	    } else if (relative) {
		ADDERROR (UNRECOGNISED, str);
		return (False);
	    }

	    if (INSTRING (*str, "Aa")) {
		if (*hrs == 12)
		    *hrs = 0;
		state = aMpM;
		continue;
	    } else if (INSTRING (*str, "Pp")) {
		    if (*hrs < 12)
			*hrs = *hrs + 12;
		    state = aMpM;
		    continue;
		}
	    ADDERROR (EXPECTEDAMPM, str);
	    return (False);
	 case aMpM:
	    str++;
	    if (INSTRING (*str, "Mm")) {
		*in24hrf = finished = True;
		str++;
	    } else {
		ADDERROR (EXPECTEDAMPM, str);
		return (False);
	    }
	}

    if ((*hrs < 24) and (*mins < 60) and (*str == '\0'))
	return (True);
    else {
	if (*hrs >= 24) {
	    (void) sprintf (badtime, "%d", *hrs);
	    ADDERROR (BADHOURS, badtime);
	} else if (*mins >= 60) {
	    (void) sprintf (badtime, "%d", *mins);
	    ADDERROR (BADMINUTES, badtime);
	} else {
	    ADDERROR (UNRECOGNISED, str);
	}
	return (False);
    }
}
