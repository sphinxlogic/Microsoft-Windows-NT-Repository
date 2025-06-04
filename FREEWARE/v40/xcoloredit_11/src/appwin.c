/* File: appwin.h
 *
 *	Main application window for XColorEdit.
 */

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>

#include "appwin.h"
#include "colors.h"


/*** preset colors ***/
static unsigned short color_values[NUM_COLORS][3] = {
	{ 0x0000, 0x0000, 0x0000 },
	{ 0xFFFF, 0xFFFF, 0xFFFF },
	{ 0xFFFF, 0x0000, 0x0000 },
	{ 0x0000, 0xFFFF, 0x0000 },
	{ 0x0000, 0x0000, 0xFFFF },
	{ 0xFFFF, 0xFFFF, 0x0000 },
	{ 0x0000, 0xFFFF, 0xFFFF },
	{ 0x6000, 0x6000, 0x6000 },

	{ 0xE000, 0x0000, 0x0000 },
	{ 0xC000, 0x0000, 0x0000 },
	{ 0xA000, 0x0000, 0x0000 },
	{ 0x8000, 0x0000, 0x0000 },
	{ 0x0000, 0xE000, 0x0000 },
	{ 0x0000, 0xC000, 0x0000 },
	{ 0x0000, 0xA000, 0x0000 },
	{ 0x0000, 0x8000, 0x0000 },

	{ 0x0000, 0x0000, 0xE000 },
	{ 0x0000, 0x0000, 0xC000 },
	{ 0x0000, 0x0000, 0xA000 },
	{ 0x0000, 0x0000, 0x8000 },
	{ 0xE000, 0xE000, 0xE000 },
	{ 0xC000, 0xC000, 0xC000 },
	{ 0xA000, 0xA000, 0xA000 },
	{ 0x8000, 0x8000, 0x8000 },

	{ 0xFFFF, 0xA000, 0x6000 },
	{ 0xD000, 0x8000, 0x4000 },
	{ 0x9000, 0x6000, 0x3000 },
	{ 0xFFFF, 0x0000, 0xFFFF },
	{ 0xC000, 0x0000, 0xFFFF },
	{ 0x8000, 0x0000, 0xE000 },
	{ 0xE000, 0x0000, 0x8000 },
	{ 0xFFFF, 0x0000, 0xC000 }
};


/* AppWin_create() - create an instance of the AppWin class.
 *	rgb_file_name = full path of standard X11 "rgb.txt" file
 */
AppWin * AppWin_create(Widget parent, char *rgb_file_name)
{
	AppWin			*gui;		/* return value */
	Display			*dsp = XtDisplay(parent);
	XmString		string;
	int			i;
	unsigned long		plane_mask;	/* from XAllocColorCells() */

	/*** allocate structure to return ***/
	gui = malloc(sizeof(AppWin));
	if (!gui) {
		fprintf(stderr, "ERROR:  cannot allocate memory\n");
		exit(-1);
	}

	/*** get colormap ***/
	XtVaGetValues(parent, XmNcolormap, &gui->cmap, NULL);

	/*** get preset colors ***/
	get_cmap_colors(dsp, gui->cmap, NUM_COLORS, color_values, gui->colors);

	/*** get foreLabel pixel ***/
	if (!XAllocColorCells(dsp, gui->cmap, True, &plane_mask, 1, &(gui->forePixel), 1)) {
		fprintf(stderr, "ERROR:  cannot allocate private color\n");
		exit(-1);
	}

	/*** create window ***/
	string = XmStringCreateSimple("XColorEdit 1.1");	/* title of window */
	gui->mainForm = XtVaCreateManagedWidget("mainForm",
						xmFormWidgetClass,
						parent,
						XmNdialogTitle, string,
						XmNnoResize, True,
/*						XmNwidth, 460,      */
						XmNwidth, 490,
						XmNheight, 352,
						XmNfractionBase, 100,
						XmNcolormap, gui->cmap,
						NULL);
	XmStringFree(string);

	/*** quit button ***/
	string = XmStringCreateSimple("Quit");
	gui->quitButton = XtVaCreateManagedWidget("quitButton",
						  xmPushButtonWidgetClass,
						  gui->mainForm,
						  XmNlabelString, string,
						  XmNbottomAttachment, XmATTACH_FORM,
						  XmNbottomOffset, 4,
						  XmNleftAttachment, XmATTACH_POSITION,
						  XmNleftPosition, 44,
						  XmNrightAttachment, XmATTACH_POSITION,
						  XmNrightPosition, 56,
						  NULL);
	XmStringFree(string);
	XtAddCallback(gui->quitButton, XmNactivateCallback,
		      (XtCallbackProc)AppWin_quit, (XtPointer)gui);

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
						  XmNbottomWidget, gui->quitButton,
						  XmNbottomOffset, 4,
						  NULL);

	gui->innerForm = XtVaCreateManagedWidget("innerForm",
						 xmFormWidgetClass,
						 gui->outerFrame,
						 XmNfractionBase, 100,
						 NULL);

	/*** preset colors ***/
	gui->presetRows = XtVaCreateManagedWidget("presetRows",
						  xmRowColumnWidgetClass,
						  gui->innerForm,
						  XmNborderWidth, 2,
						  XmNorientation, XmHORIZONTAL,
						  XmNpacking, XmPACK_COLUMN,
						  XmNnumColumns, 8,
						  XmNadjustLast, False,
						  XmNtopAttachment, XmATTACH_FORM,
						  XmNtopOffset, 8,
						  XmNrightAttachment, XmATTACH_FORM,
						  XmNrightOffset, 8,
						  XmNbottomAttachment, XmATTACH_FORM,
						  XmNbottomOffset, 8,
						  NULL);
	XtAddCallback(gui->presetRows, XmNentryCallback,
		      (XtCallbackProc)AppWin_usePreset, (XtPointer)gui);
	for (i = 0; i < NUM_COLORS; i++) AppWin_addColor(gui, i);

	/*** table Button ***/
	string = XmStringCreateLtoR("Show\nStandard\nColors", "button_font");
	gui->tableButton = XtVaCreateManagedWidget("tableButton",
						   xmPushButtonWidgetClass,
						   gui->innerForm,
						   XmNlabelString, string,
						   XmNbottomAttachment, XmATTACH_FORM,
						   XmNbottomOffset, 8,
						   XmNleftAttachment, XmATTACH_FORM,
						   XmNleftOffset, 8,
						   NULL);
	XmStringFree(string);
	XtAddCallback(gui->tableButton, XmNactivateCallback,
		      (XtCallbackProc)AppWin_showTable, (XtPointer)gui);

	/*** foreground color value ***/
	gui->valueField = XtVaCreateManagedWidget("valueField",
						  xmTextFieldWidgetClass,
						  gui->innerForm,
						  XmNeditable, False,
						  XmNcolumns, 14,
						  XmNleftAttachment, XmATTACH_FORM,
						  XmNleftOffset, 8,
						  XmNbottomAttachment, XmATTACH_WIDGET,
						  XmNbottomWidget, gui->tableButton,
						  XmNbottomOffset, 8,
						  NULL);

	/*** foreground color ***/
	string = XmStringCreateSimple("");
	gui->foreLabel = XtVaCreateManagedWidget("foreLabel",
						 xmLabelWidgetClass,
						 gui->innerForm,
						 XmNlabelString, string,
						 XmNrecomputeSize, False,
						 XmNbackground, gui->forePixel,
						 XmNborderWidth, 2,
						 XmNtopAttachment, XmATTACH_FORM,
						 XmNtopOffset, 8,
						 XmNleftAttachment, XmATTACH_FORM,
						 XmNleftOffset, 8,
						 XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
						 XmNrightWidget, gui->valueField,
						 XmNbottomAttachment, XmATTACH_WIDGET,
						 XmNbottomWidget, gui->valueField,
						 XmNbottomOffset, 4,
						 NULL);
	XmStringFree(string);

	/*** red scale **/
	string = XmStringCreateSimple("R");
	gui->redScale = XtVaCreateManagedWidget("redScale",
						xmScaleWidgetClass,
						gui->innerForm,
						XmNtitleString, string,
						XmNmaximum, 0xffff,
						XmNscaleWidth, 28,
						XmNscaleMultiple, 0x1111,
						XmNtopAttachment, XmATTACH_FORM,
						XmNtopOffset, 8,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, gui->foreLabel,
						XmNleftOffset, 16,
						XmNbottomAttachment, XmATTACH_FORM,
						XmNbottomOffset, 8,
						NULL);
	XmStringFree(string);
	XtAddCallback(gui->redScale, XmNdragCallback,
		      (XtCallbackProc)AppWin_readScale, (XtPointer)gui);
	XtAddCallback(gui->redScale, XmNvalueChangedCallback,
		      (XtCallbackProc)AppWin_readScale, (XtPointer)gui);

	/*** green scale **/
	string = XmStringCreateSimple("G");
	gui->greenScale = XtVaCreateManagedWidget("greenScale",
						  xmScaleWidgetClass,
						  gui->innerForm,
						  XmNtitleString, string,
						  XmNmaximum, 0xffff,
						  XmNscaleWidth, 28,
						  XmNscaleMultiple, 0x1111,
						  XmNtopAttachment, XmATTACH_FORM,
						  XmNtopOffset, 8,
						  XmNleftAttachment, XmATTACH_WIDGET,
						  XmNleftWidget, gui->redScale,
						  XmNbottomAttachment, XmATTACH_FORM,
						  XmNbottomOffset, 8,
						  NULL);
	XmStringFree(string);
	XtAddCallback(gui->greenScale, XmNdragCallback,
		      (XtCallbackProc)AppWin_readScale, (XtPointer)gui);
	XtAddCallback(gui->greenScale, XmNvalueChangedCallback,
		      (XtCallbackProc)AppWin_readScale, (XtPointer)gui);

	/*** blue scale **/
	string = XmStringCreateSimple("B");
	gui->blueScale = XtVaCreateManagedWidget("blueScale",
						 xmScaleWidgetClass,
						 gui->innerForm,
						 XmNtitleString, string,
						 XmNmaximum, 0xffff,
						 XmNscaleWidth, 28,
						 XmNscaleMultiple, 0x1111,
						 XmNtopAttachment, XmATTACH_FORM,
						 XmNtopOffset, 8,
						 XmNleftAttachment, XmATTACH_WIDGET,
						 XmNleftWidget, gui->greenScale,
						 XmNbottomAttachment, XmATTACH_FORM,
						 XmNbottomOffset, 8,
						 NULL);
	XmStringFree(string);
	XtAddCallback(gui->blueScale, XmNdragCallback,
		      (XtCallbackProc)AppWin_readScale, (XtPointer)gui);
	XtAddCallback(gui->blueScale, XmNvalueChangedCallback,
		      (XtCallbackProc)AppWin_readScale, (XtPointer)gui);

	/*** create external interfaces ***/
	gui->table = TableWin_create(parent, rgb_file_name, gui);

	/*** return prepared structure ***/
	return gui;
}


void AppWin_quit(Widget widget, AppWin *gui)
{
	Display	*dsp = XtDisplay(widget);

	/*** free the preset colors ***/
	XFreeColors(dsp, gui->cmap, gui->colors, NUM_COLORS, 0);
	XFreeColors(dsp, gui->cmap, &gui->forePixel, 1, 0);
	XFreeColormap(dsp, gui->cmap);

	/*** free external interfaces ***/
	TableWin_free(gui->table);

	/*** free class structure ***/
	free(gui);

	/*** exit program immediately ***/
	exit(0);
}


void AppWin_addColor(AppWin *gui, unsigned long pixel)
{
	XmString	string;
	Widget		w;
	char		name[16];

	sprintf(name, "color%d", pixel);
	string = XmStringCreateSimple("");
	w = XtVaCreateManagedWidget(name,
				    xmPushButtonWidgetClass,
				    gui->presetRows,
				    XmNlabelString, string,
				    XmNwidth, 32,
				    XmNheight, 32,
				    XmNrecomputeSize, False,
				    XmNbackground, gui->colors[pixel],
				    XmNshadowThickness, 0,
				    NULL);
	XmStringFree(string);
}


void AppWin_usePreset(Widget widget, AppWin *gui, XmRowColumnCallbackStruct *cbs)
{
	Display	*dsp = XtDisplay(widget);
	XColor	color;

	/*** get the selected pixel ***/
	XtVaGetValues(cbs->widget, XmNbackground, &color.pixel, NULL);
	XQueryColor(dsp, gui->cmap, &color);

	/*** store the forePixel ***/
	color.flags = DoRed | DoGreen | DoBlue;
	color.pixel = gui->forePixel;
	XStoreColor(dsp, gui->cmap, &color);

	AppWin_setValue(gui);
	AppWin_setScales(gui);
}


void AppWin_setValue(AppWin *gui)
{
	Display		*dsp = XtDisplay(gui->foreLabel);
	XColor		color;
	char		value[15];

	/*** get the RGB values ***/
	color.pixel = gui->forePixel;
	XQueryColor(dsp, gui->cmap, &color);

	/*** set the value widget ***/
	sprintf(value, "%04X %04X %04X", color.red, color.green, color.blue);
	XmTextFieldSetString(gui->valueField, value);
}


void AppWin_setScales(AppWin *gui)
{
	Display		*dsp = XtDisplay(gui->foreLabel);
	XColor		color;

	/*** get the RGB values ***/
	color.pixel = gui->forePixel;
	XQueryColor(dsp, gui->cmap, &color);

	/*** set the scale widgets ***/
	XtVaSetValues(gui->redScale, XmNvalue, color.red, NULL);
	XtVaSetValues(gui->greenScale, XmNvalue, color.green, NULL);
	XtVaSetValues(gui->blueScale, XmNvalue, color.blue, NULL);
}


void AppWin_showTable(Widget widget, AppWin *gui)
{
	TableWin_show(gui->table);
}


void AppWin_readScale(Widget widget, AppWin *gui, XmScaleCallbackStruct *cbs)
{
	Display *dsp = XtDisplay(widget);
	char	value[15];
	int	red, green, blue;

	/*** get scale values ***/
	XtVaGetValues(gui->redScale, XmNvalue, &red, NULL);
	XtVaGetValues(gui->greenScale, XmNvalue, &green, NULL);
	XtVaGetValues(gui->blueScale, XmNvalue, &blue, NULL);

	/*** set value field ***/
	sprintf(value, "%04X %04X %04X", red, green, blue);
	XmTextFieldSetString(gui->valueField, value);

	/*** store the forePixel ***/
	AppWin_setFore(gui, red, green, blue);
}


void AppWin_setFore(AppWin *gui, unsigned short red, unsigned short green, unsigned short blue)
{
	Display *dsp = XtDisplay(gui->mainForm);
	XColor	color;

	/*** store the forePixel ***/
	color.red = red, color.blue = blue, color.green = green;
	color.flags = DoRed | DoGreen | DoBlue;
	color.pixel = gui->forePixel;
	XStoreColor(dsp, gui->cmap, &color);
}
