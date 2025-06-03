/*
 *	date-strings.h: Definitions of global string constants
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Feb 1991.
 *
 *	$Id: date-strings.h,v 2.1 91/02/28 11:21:09 ferguson Exp $
 */
#ifndef DATE_STRINGS_H
#define DATE_STRINGS_H

extern char *shortDowStr[7],*longDowStr[7];
extern char *shortMonthStr[12],*longMonthStr[12];
extern void initDateStrings();

#endif /* DATE_STRINGS_H */
