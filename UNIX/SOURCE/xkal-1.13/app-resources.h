/*
 *	app-resources.h : External definitions for access to the application
 *		      resources.
 *
 *	George Ferguson, ferguson@cs.rochester.edu,  27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: app-resources.h,v 2.2 91/03/13 13:31:02 ferguson Exp $
 *
 */
#ifndef APP_RESOURCES_H
#define APP_RESOURCES_H

#include <X11/Intrinsic.h>

typedef struct {
    String systemAppoints;		/* file for non-editable appoints */
    String personalAppoints;		/* file for editable appoints */
    String backupExtension;		/* suffix for backup appoints file */
    String outputFormat;		/* how to write out appoints */
    Boolean daySlashMonth;		/* dd/mm rather than mm/dd */
    String date;			/* override "current" date */
    int numMonths;			/* display 1, 3, or 12 months */
    Boolean bothShown;			/* put day and month in same frame */
    Boolean useTitlebar;		/* put date on titlebar */
    Boolean listOnly;			/* non-graphic mode */
    Boolean silent;			/* no output (just return value) */
    Boolean exitUsesLevels;		/* return level sum or number */
    Boolean version;			/* print version number and quit */
    Boolean opaqueDates;		/* draw dates opaquely on calendar */
    int checkpointInterval;		/* seconds between automatic updates */
    Boolean dowLabels;			/* put dow at top of columns */
    String longDowStrings;		/* full names of dows */
    String shortDowStrings;		/* abbreviated names of dows */
    String longMonthStrings;		/* full names of months */
    String shortMonthStrings;		/* abbreviated names of months */
    int dowOffset;			/* offset to first dow (0=Sun, 1=Mon) */
    String appsStartTime;		/* time of first appointment slot */
    String appsEndTime;			/* time of last appointment slot */
    String appsIncrement;		/* time between them */
    Boolean appsUseAmPm;		/* display using am/pm or 24hr */
    int numNotes;			/* how many notes slots */
    String notesLabel;			/* and the labels for them */
    String noonStr;			/* name of 12:00 */
    String midnightStr;			/* name of 0:00 */
    Boolean rearrangeSilently;		/* don't say when appoints moved */
    String levelDelim;			/* chars to delimit level in file */
    int maxLevel;			/* number of levels for appoints */
    int defaultLevel;			/* default level for new appoints */
    /* fonts for date in calendar */
    Font dateFont1,dateFont2,dateFont3,dateFont12;
    /* offset in square */
    String datePosition1,datePosition2,datePosition3,datePosition12;
    /* width of square */
    int dateWidth1,dateWidth2,dateWidth3,dateWidth12;
    /* height of square */
    int dateHeight1,dateHeight2,dateHeight3,dateHeight12;
    String noDayShade;			/* pattern for non-days */
    String shades;			/* patterns for days w/ appoints */
    String colors;			/* colors for days w/ appoints */
    String revision;			/* version of app-defaults */
} AppResources;
extern AppResources appResources;

#endif /* APP_RESOURCES_H */
