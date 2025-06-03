/*
 *	util.h : External defs for callers of misc routines.
 *
 *      George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *      $Id: util.h,v 2.1 91/02/28 11:21:39 ferguson Exp $
 *
 */
#ifndef UTIL_H
#define UTIL_H

extern int computeDOW(), firstDOW(), lastDay();
extern void nextDay(),prevDay(),getCurrentDate();
extern int parseDate();
extern void parseLine();
extern char *expandFilename();
extern int strtotime();
extern char *timetostr();

#endif /* UTIL_H */
