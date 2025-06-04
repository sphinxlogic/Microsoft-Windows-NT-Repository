/* File: tablewin.h
 *
 *	Window to display table of standard X11 colors.
 */

#ifndef TABLEWIN_H
#define TABLEWIN_H

extern struct _appwin;

/*** number of colors displayed at one time ***/
#define SHOW_COLORS	48

/*** TableWin class ***/
typedef struct _tablewin {
	Widget		mainShell;
	Widget		mainForm;
	Widget		closeButton;		/* close the window */
	Widget		outerFrame;		/* around innerForm */
	Widget		innerForm;
	Widget		colorRows;		/* rows of colors */
	Widget		nextButton;		/* show next set of colors */
	Widget		prevButton;		/* show previous set of colors */
	Widget		colorButton[SHOW_COLORS];	/* actual color values */
	Widget		nameLabel[SHOW_COLORS];		/* actual color names */

	Colormap	cmap;			/* private colormap */
	unsigned long	colors[SHOW_COLORS];
	char		file_name[256];		/* full pathname of "rgb.txt" file */
	FILE 		*rgb_file;		/* "rgb.txt" file */
	int		file_pos;		/* current line number in "rgb.txt" */

	struct _appwin	*app_win;
} TableWin;

/*** standard methods ***/
TableWin * TableWin_create(Widget parent, char *rgb_file_name, struct _appwin *app_win);
void TableWin_show(TableWin *gui);
void TableWin_free(TableWin *gui);
void TableWin_readFile(TableWin *gui);
void TableWin_resetFile(TableWin *gui);

/*** XmPushButton callbacks ***/
void TableWin_close(Widget widget, TableWin *gui);
void TableWin_next(Widget widget, TableWin *gui);
void TableWin_prev(Widget widget, TableWin *gui);
void TableWin_selectColor(Widget widget, TableWin *gui);

/*** miscellaneous functions ***/
void get_string(FILE *fp, char *s);

#endif
