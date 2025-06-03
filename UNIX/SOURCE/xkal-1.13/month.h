/*
 *	month.h : Data types and external defs for month stuff
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: month.h,v 2.1 91/02/28 11:21:24 ferguson Exp $
 */
#ifndef MONTH_H
#define MONTH_H

typedef struct _DayButtonData {
	int day,month,year;
	Pixmap pixmap;
	Widget button;		/* Command */
} DayButtonData;
typedef struct _MonthFormData {
	int month,year;
	DayButtonData *days[42];
	Widget label;		/* Label */
	Widget form;		/* Form */
} MonthFormData;

extern MonthFormData *createMonthFormData();
extern void setMonthFormData();
extern void selectDay();

#endif /* MONTH_H */
