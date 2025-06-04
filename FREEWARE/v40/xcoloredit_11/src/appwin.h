/* File: appwin.h
 *
 *	Main application window for XColorEdit.
 */

#ifndef APPWIN_H
#define APPWIN_H

#include "tablewin.h"

/*** number of preset colors ***/
#define NUM_COLORS	32

/*** AppWin class ***/
typedef struct _appwin {
	Widget		mainForm;
	Widget		quitButton;	/* exit application */
	Widget		outerFrame;	/* around innerForm */
	Widget		innerForm;
	Widget		presetRows;	/* rows of selectable colors */
	Widget		foreLabel;	/* selected foreground color */
	Widget		redScale;	/* to set/display red portion of foreground color */
	Widget		greenScale;	/* to set/display green portion of foreground color */
	Widget		blueScale;	/* to set/display blue portion of foreground color */
	Widget		valueField;	/* to display foreground RGB value */
	Widget		tableButton;	/* to display table */

	Colormap	cmap;		/* private colormap */
	unsigned long	colors[NUM_COLORS];
	unsigned long	forePixel;	/* pixel to display in foreLabel */

	TableWin	*table;
} AppWin;

/*** standard methods ***/
AppWin * AppWin_create(Widget parent, char *rgb_file_name);
void AppWin_addColor(AppWin *gui, unsigned long pixel);
void AppWin_setValue(AppWin *gui);
void AppWin_setScales(AppWin *gui);
void AppWin_setFore(AppWin *gui, unsigned short red, unsigned short green, unsigned short blue);

/*** XmPushButton callbacks ***/
void AppWin_quit(Widget widget, AppWin *gui);
void AppWin_showTable(Widget widget, AppWin *gui);

/*** XmRowColumn callbacks ***/
void AppWin_usePreset(Widget widget, AppWin *gui, XmRowColumnCallbackStruct *cbs);

/*** XmScale callbacks ***/
void AppWin_readScale(Widget widget, AppWin *gui, XmScaleCallbackStruct *cbs);

#endif
