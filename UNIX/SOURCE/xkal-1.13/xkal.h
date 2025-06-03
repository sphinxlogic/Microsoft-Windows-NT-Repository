/*
 *	xkal.h : Global decls for X, widgets, and other stuff
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: xkal.h,v 2.1 91/02/28 11:21:45 ferguson Exp $
 *
 */
#ifndef XKAL_H
#define XKAL_H

extern Display *display;
extern Screen *screen;
extern Window root;
extern GC dateGC1,dateGC2,dateGC3,dateGC12,emptyGC,*shadeGC;

extern XtAppContext app_con;
extern Widget toplevel;

extern char *program;
extern int currentDay,currentMon,currentYear;
extern int mainDay,mainMon,mainYear;
extern int appointsChanged;
extern XtIntervalId timeoutId;

extern int main();
extern void quit(),quit_nosave(),save(),next(),prev(),today(),setNumMonths();
extern void focusThisItem(), focusNoItem(), focusNextItem(), focusPrevItem();
extern void escapeToSystem();
extern void timeoutProc();

#endif /* XKAL_H */
