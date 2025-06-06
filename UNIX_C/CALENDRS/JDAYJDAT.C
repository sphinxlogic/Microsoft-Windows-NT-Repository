 5-Jul-85 08:21:39-MDT,2308;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Fri 5 Jul 85 08:18:08-MDT
Received: from usenet by TGR.BRL.ARPA id a027771; 5 Jul 85 9:46 EDT
From: nrh@inmet.uucp
Newsgroups: net.sources
Subject: Re: date <-> day-number software sought
Message-ID: <12900006@inmet.UUCP>
Date: 3 Jul 85 14:06:00 GMT
Nf-ID: #R:bocklin:-23200:inmet:12900006:000:1746
Nf-From: inmet!nrh    Jul  3 10:06:00 1985
To:       unix-sources@BRL-TGR.ARPA


Here are two routines, jday.c and jdate.c that have served
me pretty well.  They are translations from ALGOL in Collected
Algorithms of CACM.
/*
** Takes a date, and returns a Julian day. A Julian day is the number of
** days since some base date  (in the very distant past).
** Handy for getting date of x number of days after a given Julian date
** (use jdate to get that from the Gregorian date).
** Author: Robert G. Tantzen, translator: Nat Howard
** Translated from the algol original in Collected Algorithms of CACM
** (This and jdate are algorithm 199).
*/
long
jday(mon, day, year)
int mon, day, year;
{
	long m = mon, d = day, y = year;
	long c, ya, j;

	if(m > 2) m -= 3;
	else {
		m += 9;
		--y;
	}
	c = y/100L;
	ya = y - (100L * c);
	j = (146097L * c) /4L + (1461L * ya) / 4L + (153L * m + 2L)/5L + d + 1721119L;
	return(j);
}
/* Julian date converter. Takes a julian date (the number of days since
** some distant epoch or other), and returns an int pointer to static space.
** ip[0] = month;
** ip[1] = day of month;
** ip[2] = year (actual year, like 1977, not 77 unless it was  77 a.d.);
** ip[3] = day of week (0->Sunday to 6->Saturday)
** These are Gregorian.
** Copied from Algorithm 199 in Collected algorithms of the CACM
** Author: Robert G. Tantzen, Translator: Nat Howard
*/
int *
jdate(j)
long j;
{
	static int ret[4];

	long d, m, y;

	ret[3] = (j + 1L)%7L;
	j -= 1721119L;
	y = (4L * j - 1L)/146097L;
	j = 4L * j - 1L - 146097L * y;
	d = j/4L;
	j = (4L * d + 3L)/1461L;
	d = 4L * d + 3L - 1461L * j;
	d = (d + 4L)/4L;
	m = (5L * d - 3L)/153L;
	d = 5L * d - 3 - 153L * m;
	d = (d + 5L) / 5L;
	y = 100L * y + j;
	if(m < 10) 
		m += 3;
	else {
		m -= 9;
		++y;
	}
	ret[0] =  m;
	ret[1] = d;
	ret[2] = y;
	return(ret);
}
