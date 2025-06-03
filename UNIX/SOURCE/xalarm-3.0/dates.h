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
 * Various stuff to do with dates & times.
 * 
 * Maximum allowed days based on a 32-bit unsigned long, ie. 2^32 / 24*60*60*1000.
 */

#define		MAXDAYS		(49)

/*
 * Change these if you want a different notation/language.  Keep it to 
 * 3 chars each, lower case only.
 */

#define		WEEKDAYS	"sun", "mon", "tue", "wed", "thu", "fri", "sat"
#define		MONTHS		"jan", "feb", "mar", "apr", "may", "jun", \
				"jul", "aug", "sep", "oct", "nov", "dec"
#define		DAYSINMONTHS	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
#define		TODAY		"tod"
#define		DAILY		"dai"
#define		TOMORROW	"tom"
#define		WEEK		"wee"
#define		WEEKLY		"wee"
#define		NOW		"now"
#define		NOON		"noo"

/* 
 * Error messages.
 */

#define		DATEPASSED	"The date has passed."
#define		TOOFARAHEAD	"Too far ahead, I'm afraid."
#define		ANOTHERWEEKDAY	"Another weekday?"
#define		ANOTHERMONTH	"Another month?"
#define		ANOTHERYEAR	"Another year?"
#define		WRONGDAYSMONTH	"Wrong days in month?"
#define		WRONGNUMDIGITS	"Wrong number of digits."
#define		TIMEPASSED	"The time has passed today."
#define		EXPECTEDAMPM	"Expected am/pm."
#define		BADHOURS	"Bad number of hours."
#define		BADMINUTES	"Bad number of minutes."
#define		UNRECOGNISED	"Don't recognise this."
