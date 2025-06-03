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
 * Parse dates.
 */



#include "xalarm.h"
#include "dates.h"


Boolean		AppointmentWithin();
unsigned long	DateToMilliSeconds();
int		DaysTo();
static int	DaysBetween(), DaysBeforeDate();
static Boolean	ParseDateString();
extern Boolean	ParseTimeString(), IsInteger();
extern String	NextWord();
extern long	TimeToMilliSeconds();
extern time_t	time();
extern struct tm *localtime();



extern AlarmData xalarm;



/* 
 * Is the number of days before the date within the number of days given.
 */

Boolean AppointmentWithin (withindays, line, timestr, datestr, chpos)
  String   line, *timestr, *datestr;
  int 	   withindays, *chpos;
{
    String 	    date, word;
    Boolean 	    dummy;
    unsigned long   dateout;
    long 	    timeout;
    int 	    days, dayofweek, dayofmonth, month, year, hrs, mins;

    if ((*line == '\n') or (*line == '#') or (*line == '!') or (*line == '\0'))
	return (False);

    /* 
     * Get the date.
     */
    date = XtMalloc (TEXT);
    date[0] = '\0';
    while (strcmp (word = NextWord (line, chpos), "-"))
	if (*word == '\0')
	    break;
	else if (date[0] == '\0')
	    (void) strcpy (date, word);
	else
	    (void) sprintf (ENDOF (date), " %s", word);

    if (date[0] == '\0')
	*datestr = XtNewString ("today");
    else
	*datestr = XtNewString (date);

    if (not ParseDateString (*datestr, &days, &dayofweek, &dayofmonth, &month, &year)) {
	(void) fprintf (stderr, "%s%s", xalarm.errormessage, line);
	return (False);
    }

    /*
     * Get the time.
     * There might not be a time there, so we ignore any errors.
     */
    if (*word == '\0')
	*timestr = XtNewString ("+0");
    else {
	*timestr = NextWord (line, chpos);
	if (not ParseTimeString (*timestr, &hrs, &mins, &dummy)) {
	    if (*timestr[0] != '\0')
		do
		    (*chpos)--;
		while (line[*chpos] != ' ');
	    *timestr = XtNewString ("+0");
	}
    }
    
    dateout = DateToMilliSeconds (*datestr);
    timeout = TimeToMilliSeconds (*timestr);

    if (ISVALID (dateout)) {
	/* 
	 * We can't just see if the sum is positive 'cos dateout is unsigned...
	 */
	days = (int) (dateout / MSECSIN1DAY);
	return (((days > 0) and (days <= withindays)) or
		((days == 0) and (timeout >= 0)));
    } else {
	/* 
	 * We don't report errors if the alarm is just too far ahead or gone...
	 */
	(void) DateToMilliSeconds (*datestr);
	if (not (ERRORIS (TOOFARAHEAD) or ERRORIS (DATEPASSED) or ERRORIS (TIMEPASSED)))
	    (void) fprintf (stderr, "%s%s", xalarm.errormessage, line);
	return (False);
    }
}



/* 
 * Returns the number of milliseconds to the date given in datestr.
 */

unsigned long DateToMilliSeconds (datestr)
  String   datestr;
{
    char   baddate[TEXT];
    int    days, dayofweek, dayofmonth, month, year;

    if (*datestr == '\0')
	return ((unsigned long) 0);
    else if (not ParseDateString (datestr, &days, &dayofweek, &dayofmonth, &month, &year))
	return ((unsigned long) INVALID);

    if (ISINVALID (days))
	days = DaysBeforeDate (dayofweek, dayofmonth, month, year);

    if (((days > 0) and (days <= MAXDAYS)) or
	((days == 0) and (ISINVALID (xalarm.timeout) or (xalarm.timeout >= 0))))
	return ((unsigned long) days * MSECSIN1DAY);
    else {
	if ((days == 0) and (ISVALID (xalarm.timeout)) and (xalarm.timeout < 0)) {
	    ADDERROR (TIMEPASSED, (String) NULL);
	} else if (days < 0) {
	    (void) sprintf (baddate, "%d days", days);
	    ADDERROR (DATEPASSED, baddate);
	} else {
	    (void) sprintf (baddate, "+%d days", days);
	    ADDERROR (TOOFARAHEAD, baddate);
	}
	return ((unsigned long) INVALID);
    }
}



/* 
 * Parse that string.  I did this one myself!
 * Works in whole days only.
 */

static Boolean ParseDateString (str, days, dayofweek, dayofmonth, month, year)
  String   str;
  int 	  *days, *dayofweek, *dayofmonth, *month, *year;
{
    static int 	  daysin[] = {DAYSINMONTHS};
    static char   strings[][4] = {WEEKDAYS, MONTHS, TODAY, DAILY, TOMORROW, WEEK, WEEKLY,
				  "!!!", "!!!"};
    String 	  word, monthstr = (String) NULL;
    time_t 	  now;
    struct tm 	 *today;
    int 	  thisyear, num, i, chpos = 0;
    
    RESETERROR ();
    
    *days = *dayofweek = *dayofmonth = *month = *year = -1;

    (void) time (&now);
    today = localtime (&now);

    if ((*str == '+') and (IsInteger (str)))
	*days = atoi (str);
    else
	while (strcmp (word = NextWord (str, &chpos), "")) {
	    i = 0;
	    while ((not (STREQUAL (word, strings[i]))) and (i < XtNumber (strings)))
		i++;

	    if (i == 19)				/* TODAY. */
		*days = 0;
	    else if (i == 20)				/* DAILY. */
		*days = 1;
	    else if (i == 21)				/* TOMORROW. */
		if (*days < 0)
		    *days = 1;
		else
		    (*days)++;
	    else if ((i == 22) or (i == 23))		/* WEEK or WEEKLY. */
		if (*days < 0)
		    *days = 7;
		else
		    (*days) += 7;
	    else if (i < 7)				/* Weekday. */
		if (*dayofweek < 0)
		    *dayofweek = i;
		else {
		    ADDERROR (ANOTHERWEEKDAY, word);
		    return (False);
		}
	    else if (i < 19)				/* Month. */
		if (*month < 0) {
		    *month = i-7;
		    monthstr = word;
		} else {
		    ADDERROR (ANOTHERMONTH, word);
		    return (False);
		}
	    else if (((num = atoi (word)) <= 0) or (not IsInteger (word))) {
		    ADDERROR (UNRECOGNISED, word);
		    return (False);
		}
		/*
		 * Otherwise assume it's a year or a month day.
		 */
		else if (num > 90)
		    if (*year < 0)
			*year = num;
		    else {
			ADDERROR (ANOTHERYEAR, word);
			return (False);
		    }
		else
		    if (*dayofmonth < 0)
			*dayofmonth = num;
		    else {
			ADDERROR (ANOTHERMONTH, word);
			return (False);
		    }
	}
    
    /* 
     * Have we been given a day + day of week, like "week tues"?
     */
    if ((*days > 0) and (*dayofweek >= 0))
	(*days) += ((*dayofweek + 7) - today->tm_wday) % 7;

    /* 
     * Check that the day of month is OK for this month.
     */
    if ((*month >= 0) and (*dayofmonth > 0)) {
	if ((thisyear = *year) < 0)
	    thisyear = today->tm_year + 1900;
	daysin[1] = DAYSINFEB (thisyear);
	if (*dayofmonth > daysin[*month]) {
	    ADDERROR (WRONGDAYSMONTH, monthstr);
	    return (False);
	}
    }
    
    return (True);
}



/* 
 * Calculates the number of *whole* days before the date given by the args.
 */

static int DaysBeforeDate (dayofweek, dayofmonth, month, year)
  int 	   dayofweek, dayofmonth, month, year;
{
    static int 	 daysin[] = {DAYSINMONTHS};
    struct tm 	 today, *date;
    time_t 	 now;

    (void) time (&now);
    date = localtime (&now);
    today.tm_yday = date->tm_yday;
    today.tm_year = date->tm_year;

    if (year > 99)
	year = year - 1900;

    if (dayofweek < 0) {
	/* 
	 * If no day of week is given, just use the day/month/year date.
	 * Cycle through the month/year.
	 */
	if (dayofmonth >= 0)		date->tm_mday = dayofmonth;
	if (month >= 0)			date->tm_mon = month;
	if (year >= 0)			date->tm_year = year;
	while ((year < 0) and (date->tm_year < (2500-1900)) and
	       (DaysBetween (&today, date) < 0))
	    if (month < 0)
		date->tm_mon = (date->tm_mon + 1) % 12;
	    else
		date->tm_year++;
    } else
	/* 
	 * We assume Buck Rodgers will have something better than xalarm...
	 * Cycle through dates until we find it.
	 */
	while ((date->tm_year < (2500-1900)) and 
	       (((dayofweek >= 0) and	(dayofweek != date->tm_wday)) or
		((dayofmonth >= 0) and	(dayofmonth != date->tm_mday)) or
		((month >= 0) and	(month != date->tm_mon)) or
		((year >= 0) and	(year != date->tm_year)))) {
	    /* 
	     * Just add one to the week day & month day...
	     */
	    date->tm_wday = (date->tm_wday + 1) % 7;
	    date->tm_mday++;
	    daysin[1] = DAYSINFEB (date->tm_year + 1900);
	    if (date->tm_mday > daysin[date->tm_mon]) {
		date->tm_mday = 1;
		date->tm_mon++;
		if (date->tm_mon > 11) {
		    date->tm_mon = 0;
		    date->tm_year++;
		}
	    }
	}

    return (DaysBetween (&today, date));
}



/* 
 * Why oh why don't all systems have timelocal()?
 */

static int DaysBetween (today, date)
  struct tm  *today, *date;
{
    static int 	 daysin[] = {DAYSINMONTHS};
    int 	 i, dayofyear = date->tm_mday-1, daysbetween = 0;

    for (i=0; i<date->tm_mon; i++)
	if (i == 1)
	    dayofyear += DAYSINFEB (date->tm_year + 1900);
	else
	    dayofyear += daysin[i];

    if (date->tm_year == today->tm_year)
	return (dayofyear - today->tm_yday);
    else {
	for (i = 1 + MIN (date->tm_year, today->tm_year);
	     i < MAX (date->tm_year, today->tm_year); i++)
	    daysbetween += DAYSINYEAR (i + 1900);

	daysbetween += (date->tm_year > today->tm_year) ?
			dayofyear + (DAYSINYEAR (today->tm_year+1900) - today->tm_yday) :
			today->tm_yday + (DAYSINYEAR (date->tm_year+1900) - dayofyear);

	return ((date->tm_year > today->tm_year) ? daysbetween : -daysbetween);
    }
}



/* 
 * Returns the number of days before the date given.  WEEK is special.
 * WEEK means until the end of the current week.
 */

int DaysTo (datestr, inst)
  String     datestr;
  Instance   inst;
{
    time_t 	    abstime;
    struct tm 	   *now;
    unsigned long   millisecs;

    (void) time (&abstime);
    now = localtime (&abstime);

    if (STREQUAL (datestr, WEEK))
	if (inst == Daemon)
	    return (DAEMONWEEKLY);
	else
	    return (7 - now->tm_wday);
    else
	if (ISVALID (millisecs = DateToMilliSeconds (datestr)))
	    return ((int) (millisecs / MSECSIN1DAY));
	else
	    return (INVALID);
}
