/* File: tablewin.h
 *
 *	Window to display table of standard X11 colors.
 */

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>

#include "tablewin.h"
#include "appwin.h"
#include "colors.h"


TableWin * TableWin_create(Widget parent, char *rgb_file_name, struct _appwin *app_win)
{
	TableWin		*gui;		/* return value */
	Display			*dsp = XtDisplay(parent);
	XmString		string;
	int			i;
	Widget			form;		/* XmForm inside of colorRows */
	XWindowAttributes	win_attr;	/* to get parent's visual */
	unsigned long		plane_mask;	/* from XAllocColorCells() */

	/*** allocate structure to return ***/
	gui = malloc(sizeof(TableWin));
	if (!gui) {
		fprintf(stderr, "ERROR:  cannot allocate memory\n");
		exit(-1);
	}
	gui->app_win = app_win;

	/*** open color file ***/
	strncpy(gui->file_name, rgb_file_name, 256);
	gui->rgb_file = fopen(gui->file_name, "r");
	if (!gui->rgb_file) {
		fprintf(stderr, "ERROR:  cannot open RGB file \"%s\"\n", gui->file_name);
		exit(-1);
	}
	gui->file_pos = 0;

	/*** get colormap ***/
	XGetWindowAttributes(dsp, DefaultRootWindow(dsp), &win_attr);
	gui->cmap = XCreateColormap(dsp, DefaultRootWindow(dsp), win_attr.visual, AllocNone);

	/*** get color cells ***/
	if (!XAllocColorCells(dsp, gui->cmap, True, &plane_mask, 1, gui->colors, SHOW_COLORS)) {
		fprintf(stderr, "ERROR:  cannot allocate private color cells\n");
		exit(-1);
	}

	/*** create window ***/
	gui->mainShell = XtVaCreatePopupShell("tableShell",
					      topLevelShellWidgetClass,
					      parent,
					      XmNcolormap, gui->cmap,
					      NULL);

	string = XmStringCreateSimple("XColorEdit - Standard X11 Colors");	/* title of window */
	gui->mainForm = XtVaCreateManagedWidget("mainForm",
						xmFormWidgetClass,
						gui->mainShell,
						XmNdialogTitle, string,
						XmNnoResize, True,
						XmNwidth, 530,
						XmNheight, 700,
						XmNfractionBase, 100,
						NULL);
	XmStringFree(string);

	/*** close button ***/
	string = XmStringCreateSimple("Close");
	gui->closeButton = XtVaCreateManagedWidget("closeButton",
						   xmPushButtonWidgetClass,
						   gui->mainForm,
						   XmNlabelString, string,
						   XmNbottomAttachment, XmATTACH_FORM,
						   XmNbottomOffset, 4,
						   XmNleftAttachment, XmATTACH_POSITION,
						   XmNleftPosition, 42,
						   XmNrightAttachment, XmATTACH_POSITION,
						   XmNrightPosition, 58,
						   NULL);
	XmStringFree(string);
	XtAddCallback(gui->closeButton, XmNactivateCallback,
		      (XtCallbackProc)TableWin_close, (XtPointer)gui);

	/*** inner box ***/
	gui->outerFrame = XtVaCreateManagedWidget("outerFrame",
						  xmFrameWidgetClass,
						  gui->mainForm,
						  XmNshadowType, XmSHADOW_ETCHED_OUT,
						  XmNshadowThickness, 4,
						  XmNtopAttachment, XmATTACH_FORM,
						  XmNtopOffset, 8,
						  XmNrightAttachment, XmATTACH_FORM,
						  XmNrightOffset, 8,
						  XmNleftAttachment, XmATTACH_FORM,
						  XmNleftOffset, 8,
						  XmNbottomAttachment, XmATTACH_WIDGET,
						  XmNbottomWidget, gui->closeButton,
						  XmNbottomOffset, 4,
						  NULL);

	gui->innerForm = XtVaCreateManagedWidget("innerForm",
						 xmFormWidgetClass,
						 gui->outerFrame,
						 XmNfractionBase, 100,
						 NULL);

	/*** next button ***/
	string = XmStringCreateSimple("Next");
	gui->nextButton = XtVaCreateManagedWidget("nextButton",
						  xmPushButtonWidgetClass,
						  gui->innerForm,
						  XmNlabelString, string,
						  XmNwidth, 80,
						  XmNbottomAttachment, XmATTACH_FORM,
						  XmNbottomOffset, 8,
						  XmNrightAttachment, XmATTACH_FORM,
						  XmNrightOffset, 8,
						  NULL);
	XmStringFree(string);
	XtAddCallback(gui->nextButton, XmNactivateCallback,
		      (XtCallbackProc)TableWin_next, (XtPointer)gui);

	/*** previous button ***/
	string = XmStringCreateSimple("Previous");
	gui->prevButton = XtVaCreateManagedWidget("prevButton",
						  xmPushButtonWidgetClass,
						  gui->innerForm,
						  XmNlabelString, string,
						  XmNwidth, 80,
						  XmNbottomAttachment, XmATTACH_FORM,
						  XmNbottomOffset, 8,
						  XmNleftAttachment, XmATTACH_FORM,
						  XmNleftOffset, 8,
						  NULL);
	XmStringFree(string);
	XtAddCallback(gui->prevButton, XmNactivateCallback,
		      (XtCallbackProc)TableWin_prev, (XtPointer)gui);

	/*** color table ***/
	gui->colorRows = XtVaCreateManagedWidget("colorRows",
						 xmRowColumnWidgetClass,
						 gui->innerForm,
						 XmNborderWidth, 2,
						 XmNorientation, XmVERTICAL,
						 XmNpacking, XmPACK_COLUMN,
						 XmNnumColumns, 2,
						 XmNadjustLast, False,
						 XmNspacing, 4,
						 XmNtopAttachment, XmATTACH_FORM,
						 XmNtopOffset, 8,
						 XmNrightAttachment, XmATTACH_FORM,
						 XmNrightOffset, 8,
						 XmNleftAttachment, XmATTACH_FORM,
						 XmNleftOffset, 8,
						 XmNbottomAttachment, XmATTACH_WIDGET,
						 XmNbottomWidget, gui->prevButton,
						 XmNbottomOffset, 8,
						 NULL);

	for (i = 0; i < SHOW_COLORS; i++) {
		/*** each color entry has a form ***/
		form = XtVaCreateManagedWidget("colorForm",
					       xmFormWidgetClass,
					       gui->colorRows,
					       XmNfractionBase, 100,
					       NULL);
		string = XmStringCreateSimple("");
		/*** push button for selecting color ***/
		gui->colorButton[i] = XtVaCreateManagedWidget("colorButton",
							      xmPushButtonWidgetClass,
							      form,
							      XmNlabelString, string,
							      XmNwidth, 60,
							      XmNheight, 20,
							      XmNrecomputeSize, False,
							      XmNbackground, gui->colors[i],
							      XmNshadowThickness, 0,
							      XmNtopAttachment, XmATTACH_FORM,
							      XmNbottomAttachment, XmATTACH_FORM,
							      XmNleftAttachment, XmATTACH_FORM,
							      NULL);
		XmStringFree(string);
		XtAddCallback(gui->colorButton[i], XmNactivateCallback,
			      (XtCallbackProc)TableWin_selectColor, (XtPointer)gui);
		/*** name of color ***/
		gui->nameLabel[i] = XtVaCreateManagedWidget("nameLabel",
							    xmLabelWidgetClass,
							    form,
							    XmNwidth, 180,
							    XmNrecomputeSize, False,
							    XmNalignment, XmALIGNMENT_BEGINNING,
							    XmNtopAttachment, XmATTACH_FORM,
							    XmNbottomAttachment, XmATTACH_FORM,
							    XmNrightAttachment, XmATTACH_FORM,
							    XmNleftAttachment, XmATTACH_WIDGET,
							    XmNleftWidget, gui->colorButton[i],
							    XmNleftOffset, 4,
							    NULL);
	}
	TableWin_readFile(gui);

	/*** return prepared structure ***/
	return gui;
}


void TableWin_close(Widget widget, TableWin *gui)
{
	XtPopdown(gui->mainShell);
}


void TableWin_show(TableWin *gui)
{
	XtPopup(gui->mainShell, XtGrabNone);
}


void TableWin_free(TableWin *gui)
{
	Display	*dsp = XtDisplay(gui->mainForm);

	fclose(gui->rgb_file);

	/*** free the colors ***/
	XFreeColors(dsp, gui->cmap, gui->colors, SHOW_COLORS, 0);
	XFreeColormap(dsp, gui->cmap);

	free(gui);
}


void TableWin_readFile(TableWin *gui)
{
	int		i;
	unsigned short	red, green, blue;
	char		name[64];
	XmString	string;
	XColor		color;

	for (i = 0; i < SHOW_COLORS; i++) {
		if (feof(gui->rgb_file)) TableWin_resetFile(gui);

		/*** read data from file ***/
		fscanf(gui->rgb_file, "%hd %hd %hd", &red, &green, &blue);
		get_string(gui->rgb_file, name);

		/*** set colorButton ***/
		color.red = red << 8, color.blue = blue << 8, color.green = green << 8;
		color.flags = DoRed | DoGreen | DoBlue;
		color.pixel = gui->colors[i];
		XStoreColor(XtDisplay(gui->mainForm), gui->cmap, &color);

		/*** set nameLabel ***/
		string = XmStringCreateSimple(name);
		XtVaSetValues(gui->nameLabel[i], XmNlabelString, string, NULL);
		XmStringFree(string);

		/*** increment file position ***/
		gui->file_pos++;
	}
}


void TableWin_resetFile(TableWin *gui)
{
	fclose(gui->rgb_file);

	gui->rgb_file = fopen(gui->file_name, "r");
	if (!gui->rgb_file) {
		fprintf(stderr, "ERROR:  cannot open RGB file \"%s\"\n", gui->file_name);
		exit(-1);
	}
	gui->file_pos = 0;
}


void get_string(FILE *fp, char *s)
{
	char	c;

	/*** skip leading whitespace ***/
	do {
		c = fgetc(fp);
	} while ((c == ' ' || c == '\t' || c == '\n') && !feof(fp));

	/*** copy the string ***/
	do {
		*s++ = c;
		c = fgetc(fp);
	} while (c != '\n' && !feof(fp));

	/*** terminate string ***/
	*s = '\0';
}


void TableWin_next(Widget widget, TableWin *gui)
{
	TableWin_readFile(gui);
}


void TableWin_prev(Widget widget, TableWin *gui)
{
	int	i;
	char	line[256];

	if (gui->file_pos == 0) return;

	/*** find new file position ***/
	gui->file_pos -= SHOW_COLORS * 2;
	if (gui->file_pos < 0) gui->file_pos = 0;
	i = gui->file_pos;

	/*** move to file position ***/
	TableWin_resetFile(gui);
	gui->file_pos = i;
	while (i--)
		fgets(line, 256, gui->rgb_file);

	TableWin_readFile(gui);
}


void TableWin_selectColor(Widget widget, TableWin *gui)
{
	Display		*dsp = XtDisplay(widget);
	XColor		color;
	int		pixel;

	/*** get the RGB values ***/
	XtVaGetValues(widget, XmNbackground, &pixel, NULL);
	color.pixel = pixel;
	XQueryColor(dsp, gui->cmap, &color);

	/*** set the value widget ***/
	AppWin_setFore(gui->app_win, color.red, color.green, color.blue);
	AppWin_setValue(gui->app_win);
	AppWin_setScales(gui->app_win);
}
