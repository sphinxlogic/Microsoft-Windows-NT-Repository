/*
 *	day.h : Data types and external defs for day stuff
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: day.h,v 2.1 91/02/28 11:21:12 ferguson Exp $
 */
#ifndef DAY_H
#define DAY_H

#include "month.h"
#include "db.h"

typedef struct _DayTextData {
	Widget time;		/* Label */
	Widget text;		/* Text */
} DayTextData;
typedef struct _DayFormData {
	int day,month,year;	/* who am i */
	DayTextData **items;	/* widgets */
	Widget date;		/* Label (if used) */
	Widget form;		/* Form */
	Widget shell;		/* Shell for popups */
	DayButtonData *buttonData;	/* button pressed if any */
	String *msgText;	/* original texts */
	Msgrec **msg;		/* pointers to db entries */
	Boolean changed;	/* any edit here? */
	Boolean editing;	/* being edited? */
} DayFormData;

extern DayFormData *createDayFormData(),*createPopupDayFormData();
extern void setDayFormData(),clearDayFormData();
extern void checkpointAppoints();
extern int timetopos();
extern void postotime();


extern DayFormData *currentDayFormData;
extern int numDisplayedAppoints;

#endif /* DAY_H */
