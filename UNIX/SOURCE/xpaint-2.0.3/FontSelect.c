/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xos.h>
#include <math.h>
#include <stdio.h>

/*
**  swiped from X11/Xfuncproto.h
**   since qsort() may or may not be defined with a constant sub-function
*/
#ifndef _Xconst
#if __STDC__ || defined(__cplusplus) || defined(c_plusplus) || (FUNCPROTO&4)
#define _Xconst const
#else
#define _Xconst
#endif
#endif /* _Xconst */

#ifndef NOSTDHDRS
#include <stdlib.h>
#include <unistd.h>
#endif

#include "Misc.h"

typedef struct {
	char	*fontFamily;
	char	*weight;
	char	*slant;
	char	*setWidth;
	char	*points;
	char	*spacing;
} FontName;

typedef struct StringList_s {
	char			*str;
	struct StringList_s	*next;
} StringList;

typedef struct {
	int		number;
	FontName	*names;
	char		**namesStorage;
	Widget		text, paint;
	Widget		family, weight, point, spacing;
	Widget		expect;
	Boolean		expectFlag;
} arg_t;

static struct shortSizes_s {
	char	*name;
	char	*weight;
	char	*slant;
	char	*setWidth;
	char	*serif;
} shortSizes[128];
static int shortSizesNum = 0;

static char	*getShortName(char *weight, char *slant, char *setWidth, char *serif)
{
	int	i;
	for (i = 0; i < shortSizesNum; i++) {
		if (strcmp(shortSizes[i].weight, weight) == 0 &&
		    strcmp(shortSizes[i].slant, slant) == 0 &&
		    strcmp(shortSizes[i].setWidth, setWidth) == 0 &&
		    strcmp(shortSizes[i].serif, serif) == 0)
			return shortSizes[i].name;
	}
	return "UNKNOWN";
}

static void	makeShortName(char *weight, char *slant, char *setWidth, char *serif)
{
	char 	*weStr, *slStr, *swStr, *seStr;
	char	*cp, buf[128];

	if (strcmp(getShortName(weight, slant, setWidth, serif), "UNKNOWN") != 0)
		return;

	shortSizes[shortSizesNum].weight = weight;
	shortSizes[shortSizesNum].slant = slant;
	shortSizes[shortSizesNum].setWidth = setWidth;
	shortSizes[shortSizesNum].serif = serif;

	if (*weight == '\0')
		weStr = NULL;
	else if (strcmp(weight, "medium") == 0)
		weStr = "";
	else if (strcmp(weight, "bold") == 0)
		weStr = "Bold";
	else
		weStr = weight;
	
	if (*slant == '\0')
		slStr = NULL;
	else if (*slant == 'r')
		slStr = "";
	else if (*slant == 'i')
		slStr = "Italic";
	else if (*slant == 'o')
		slStr = "Oblique";
	else
		slStr = slant;

	if (*setWidth == '\0')
		swStr = NULL;
	else if (strcmp(setWidth, "normal") == 0)
		swStr = "";
	else if (strcmp(setWidth, "semicondensed") == 0)
		swStr = "Condensed";
	else
		swStr = setWidth;

	if (*serif == '\0')
		seStr = NULL;
	else if (strcmp(serif, "sans") == 0)
		seStr = "";
	else
		seStr = serif;

	if (weStr == NULL && slStr == NULL && swStr == NULL && seStr == NULL) {
		shortSizes[shortSizesNum++].name = "Unspecified";
		return;
	}
	if (weStr == NULL) weStr = "";
	if (slStr == NULL) slStr = "";
	if (swStr == NULL) swStr = "";
	if (seStr == NULL) seStr = "";

	if (*weStr == '\0' && *slStr == '\0' && *swStr == '\0' && *seStr == '\0') {
		shortSizes[shortSizesNum++].name = "Normal";
		return;
	}

	/*
	** Make the string, eat the leading and trailing white space.
	*/
	sprintf(buf, "%s %s %s %s", weStr, slStr, swStr, seStr);
	for (cp = buf + strlen(buf) - 1; *cp == ' '; cp--)
		;
	*++cp = '\0';
	for (cp = buf; *cp == ' '; cp++)
		;
	shortSizes[shortSizesNum++].name = XtNewString(cp);
}

static FontName	*getSettings(arg_t *arg)
{
	static char		pointBuf[20];
	static FontName		name;
	XawListReturnStruct	*item;
	int			i;

	name.fontFamily = NULL;
	name.weight     = NULL;
	name.slant      = NULL;
	name.setWidth   = NULL;
	name.points     = NULL;
	name.spacing    = NULL;

	if ((item = XawListShowCurrent(arg->family))->list_index != XAW_LIST_NONE)
		name.fontFamily = item->string;
	if ((item = XawListShowCurrent(arg->point))->list_index != XAW_LIST_NONE) {
		float	ps = atof(item->string);
		sprintf(pointBuf,"%d",(int)(ps * 10.0));
		name.points = pointBuf;
	}
	if ((item = XawListShowCurrent(arg->weight))->list_index != XAW_LIST_NONE) {
		for (i = 0; i < XtNumber(shortSizes); i++) {
			if (strcmp(item->string, shortSizes[i].name) == 0) {
				name.weight = shortSizes[i].weight;
				name.slant = shortSizes[i].slant;
				name.setWidth = shortSizes[i].setWidth;
				break;
			}
		}
	}
	if ((item = XawListShowCurrent(arg->spacing))->list_index != XAW_LIST_NONE) {
		if (strcmp(item->string, "Fixed") == 0)
			name.spacing = "m";
		else if (strcmp(item->string, "Variable") == 0)
			name.spacing = "p";
		else if (strcmp(item->string, "Cell") == 0)
			name.spacing = "c";
		else
			name.spacing = item->string;
	}

	return &name;
}

static int strqsortcmp(char **a, char **b)
{ 
	return strcmp(*a, *b);
}

static int valqsortcmp(char **a, char **b)
{ 
	float	fa = atof(*a);
	float	fb = atof(*b);
	if (fa == fb)
		return 0;
	if (fa < fb)
		return -1;
	return 1;
}

static void cleanLists(arg_t *arg)
{
	String	*strs;
	int	num;

	XtVaGetValues(arg->spacing, XtNnumberStrings, &num, XtNlist, &strs, NULL);
	if (strs != NULL && num != 0) XtFree((XtPointer)strs);
	XtVaGetValues(arg->family, XtNnumberStrings, &num, XtNlist, &strs, NULL);
	if (strs != NULL && num != 0) XtFree((XtPointer)strs);
	XtVaGetValues(arg->weight, XtNnumberStrings, &num, XtNlist, &strs, NULL);
	if (strs != NULL && num != 0) XtFree((XtPointer)strs);
	XtVaGetValues(arg->point, XtNnumberStrings, &num, XtNlist, &strs, NULL);
	if (strs != NULL && num != 0) {
		int	i;
		for (i = 0; i < num; i++)
			XtFree((XtPointer)strs[i]);
		XtFree((XtPointer)strs);
	}
}

static void	match(arg_t *arg)
{
	static int	firstTime = True;
	int		i;
	int		count = 0;
	char		*nm;
	FontName	*cur = arg->names, *last;
	FontName	*set = getSettings(arg);
	StringList	*familyList = NULL, *pointList = NULL;
	StringList	*weightList = NULL, *spacingList = NULL;
	int		familyListLen = 0, pointListLen = 0;
	int		weightListLen = 0, spacingListLen = 0;
	StringList	*curStr, *nxtStr;
	String		*list;

	if (firstTime)
		firstTime = False;
	else
		cleanLists(arg);

	for (i = 0; i < arg->number; i++, cur++) {

#define TEST(f)	if (set->f != NULL && strcmp(set->f, cur->f) != 0) continue
		TEST(fontFamily);
		TEST(weight);
		TEST(slant);
		TEST(setWidth);
		TEST(points);
		TEST(spacing);
#undef TEST

		/*
		**  Add the family name
		*/
		switch (*cur->spacing) {
		case 'm': nm = "Fixed";		break;
		case 'p': nm = "Variable";	break;
		case 'c': nm = "Cell";		break;
		default:  nm = cur->spacing;	break;
		}
		for (curStr = spacingList; curStr != NULL; curStr = curStr->next) {
			if (strcmp(curStr->str, nm) == 0)
				break;
		}
		if (curStr == NULL) {
			curStr = XtNew(StringList);
			curStr->next = spacingList;
			curStr->str = nm;
			spacingList = curStr;
			spacingListLen++;
		}

		/*
		**  Add the family name
		*/
		for (curStr = familyList; curStr != NULL; curStr = curStr->next) {
			if (strcmp(curStr->str, cur->fontFamily) == 0)
				break;
		}
		if (curStr == NULL) {
			curStr = XtNew(StringList);
			curStr->next = familyList;
			curStr->str = cur->fontFamily;
			familyList = curStr;
			familyListLen++;
		}

		/*
		**  Add the width info
		*/
		nm = getShortName(cur->weight, cur->slant, cur->setWidth, "");
		for (curStr = weightList; curStr != NULL; curStr = curStr->next) {
			if (strcmp(curStr->str, nm) == 0)
				break;
		}
		if (curStr == NULL) {
			curStr = XtNew(StringList);
			curStr->next = weightList;
			curStr->str = nm;
			weightList = curStr;
			weightListLen++;
		}

		/*
		**  Add the point size
		*/
		for (curStr = pointList; curStr != NULL; curStr = curStr->next) {
			if ((int)(atof(curStr->str)*10.0) == atoi(cur->points)) 
				break;
		}
		if (curStr == NULL) {
			char	buf[14];
			int	p = atoi(cur->points);

			curStr = XtNew(StringList);
			curStr->next = pointList;
			if (p % 10 == 0) {
				sprintf(buf,"%d", p / 10);
			} else {
				sprintf(buf,"%d.%d", p / 10, p % 10);
			}
			curStr->str = XtNewString(buf);
			pointList = curStr;
			pointListLen++;
		}

		count++;
		last = cur;
	}

	if (count == 1) {
		char		buf[512];
		XFontStruct	*info;
		sprintf(buf, "-*-%s-%s-%s-%s-*-*-%s-*-*-%s-*-*-*",
				last->fontFamily,
				last->weight == NULL ? "" : last->weight,
				last->slant == NULL ? "" : last->slant,
				last->setWidth == NULL ? "" : last->setWidth,
				last->points,
				last->spacing);

		if ((info = XLoadQueryFont(XtDisplay(arg->text), buf)) != NULL) {
			XtVaSetValues(arg->text, 
				XtNfont, info,
				XtNstring, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijlmnopqrstuvwxyz0123456789", 
				NULL);
		}
	} else {
		XtVaSetValues(arg->text, XtNstring, "", NULL);
	}

	/*
	**  Set the widgets
	*/

	list = (String *)XtCalloc(spacingListLen + 1, sizeof(String));
	for (i = 0, curStr = spacingList; curStr != NULL; curStr = curStr->next, i++) 
		list[i] = curStr->str;
	qsort(list, i, sizeof(String), (int (*)(_Xconst void *,_Xconst void *))strqsortcmp);
	XawListChange(arg->spacing, list, spacingListLen, 0, True);
	if (set->spacing)
		XawListHighlight(arg->spacing, 0);

	list = (String *)XtCalloc(familyListLen + 1, sizeof(String));
	for (i = 0, curStr = familyList; curStr != NULL; curStr = curStr->next, i++) 
		list[i] = curStr->str;
	qsort(list, i, sizeof(String), (int (*)(_Xconst void *,_Xconst void *))strqsortcmp);
	XawListChange(arg->family, list, familyListLen, 0, True);
	if (set->fontFamily)
		XawListHighlight(arg->family, 0);
	
	list = (String *)XtCalloc(weightListLen + 1, sizeof(String));
	for (i = 0, curStr = weightList; curStr != NULL; curStr = curStr->next, i++) 
		list[i] = curStr->str;
	qsort(list, i, sizeof(String), (int (*)(_Xconst void *,_Xconst void *))strqsortcmp);
	XawListChange(arg->weight, list, weightListLen, 0, True);
	if (set->weight)
		XawListHighlight(arg->weight, 0);

	list = (String *)XtCalloc(pointListLen + 1, sizeof(String));
	for (i = 0, curStr = pointList; curStr != NULL; curStr = curStr->next, i++) 
		list[i] = curStr->str;
	qsort(list, i, sizeof(String), (int (*)(_Xconst void *,_Xconst void *))valqsortcmp);
	XawListChange(arg->point, list, pointListLen, 0, True);
	if (set->points)
		XawListHighlight(arg->point, 0);

	/*
	**  Clean up
	*/
	for (curStr = spacingList; curStr != NULL; curStr = nxtStr) {
		nxtStr = curStr->next;
		XtFree((XtPointer)curStr);
	}
	for (curStr = familyList; curStr != NULL; curStr = nxtStr) {
		nxtStr = curStr->next;
		XtFree((XtPointer)curStr);
	}
	for (curStr = weightList; curStr != NULL; curStr = nxtStr) {
		nxtStr = curStr->next;
		XtFree((XtPointer)curStr);
	}
	for (curStr = pointList; curStr != NULL; curStr = nxtStr) {
		nxtStr = curStr->next;
		XtFree((XtPointer)curStr);
	}
}

static FontName	*chop(int *number, char **names)
{
	int		i, j, count = 0;
	FontName	*n = (FontName*)XtCalloc(*number, sizeof(FontName));

	if (n == NULL)
		return NULL;

#define NEXT(p) { 	char	*np;				\
			if ((np = strchr(p, '-')) == NULL)	\
				continue;			\
			*np++ = '\0';				\
			p = np;					\
		}

	for (i = 0; i < *number; i++) {
		char		*cp = names[i];
		FontName	*cur = &n[count];

		NEXT(cp);	/* foundry */
		NEXT(cp);	/* font family */
		cur->fontFamily = cp;
		NEXT(cp);	/* weight */
		cur->weight = cp;
		NEXT(cp);	/* slant */
		cur->slant = cp;
		NEXT(cp);	/* set width */
		cur->setWidth = cp;
		NEXT(cp);	/* NULL */
		NEXT(cp);	/* pixels */
		NEXT(cp);	/* point size */
		cur->points = cp;
		NEXT(cp);	/* horizontal resolution */
		NEXT(cp);	/* vertical resolution */
		NEXT(cp);	/* spacing */
		cur->spacing = cp;
		NEXT(cp);	/* average width */
		NEXT(cp);	/* character set */
		NEXT(cp);	/* character class */

		/*
		**  Check to make sure that this is a unique font
		**   from the rest of the set.
		*/
		for (j = 0; j < count; j++) {
			if (strcmp(cur->fontFamily, n[j].fontFamily) == 0 && 
			    strcmp(cur->weight, n[j].weight) == 0 && 
			    strcmp(cur->slant, n[j].slant) == 0 && 
			    strcmp(cur->setWidth, n[j].setWidth) == 0 && 
			    strcmp(cur->points, n[j].points) == 0 && 
			    strcmp(cur->spacing, n[j].spacing) == 0)
				break;
		}
		makeShortName(cur->weight, cur->slant, cur->setWidth, "");
		if (j != count)
			continue;
		count++;
	}
#undef NEXT

	*number = count;
	return n;
}

static void applySetCallback(Widget paint, void *info)
{
	XtVaSetValues(paint, XtNfont, (XFontStruct*)info, NULL);
}
static void applyCallback(Widget w, XtPointer argArg, XtPointer junk)
{
	arg_t		*arg = (arg_t *)argArg;
	XFontStruct     *info;

	XtVaGetValues(arg->text, XtNfont, &info, NULL);
#if 0
	XtVaSetValues(arg->paint, XtNfont, info, NULL);
#else
	GraphicAll(applySetCallback, info);
#endif
}
static void closeCallback(Widget w, XtPointer argArg, XtPointer junk)
{
	arg_t	*arg = (arg_t *)argArg;

	XtPopdown(GetShell(arg->family));
}
static void listCallback(Widget w, XtPointer argArg, XtPointer junk)
{
	arg_t	*arg = (arg_t *)argArg;
	arg->expectFlag = True;
	arg->expect = None;
	match(arg);
}
static void expectCallback(Widget w, XtPointer argArg, XEvent *event, Boolean *junk)
{
	arg_t	*arg = (arg_t*)argArg;

	if (event->type == ButtonRelease) {
		if (arg->expect != None)
			match(arg);
		arg->expect = None;
		arg->expectFlag = False;
	} else if (arg->expectFlag == False) {
		arg->expect = w;
	}
}

void FontSelect(Widget w, Widget paint)
{
	static Widget	shell = None;
	Widget		pane, form;
	Widget		okButton, doneButton, vport, label;
	arg_t		*arg = XtNew(arg_t);

	if (shell != None) {
		XtPopup(shell, XtGrabNone);
		return;
	}

	shell = XtVaCreatePopupShell("fontSelect",
			topLevelShellWidgetClass, GetToplevel(w),
			NULL);

	pane = XtVaCreateManagedWidget("pane",
			panedWidgetClass, shell,
			XtNborderWidth, 0,
			NULL);

	label = XtVaCreateManagedWidget("title",
			labelWidgetClass, pane,
			XtNlabel, "Select the font properties you wish",
			XtNborderWidth, 0,
			XtNshowGrip, False,
			NULL);

	form = XtVaCreateManagedWidget("form",
			formWidgetClass, pane,
			XtNborderWidth, 0,
			NULL);

	/*
	**  lists of items to select
	*/
	label = XtVaCreateManagedWidget("spacingLabel",
			labelWidgetClass, form,
			XtNborderWidth, 0,
			NULL);
	vport = XtVaCreateManagedWidget("vport",
			viewportWidgetClass, form,
			XtNuseBottom, True,
			XtNuseRight, True,
			XtNallowHoriz, True,
			XtNallowVert, True,
			XtNfromVert, label,
			NULL);
	arg->spacing = XtVaCreateManagedWidget("spacing",
				listWidgetClass, vport,
				XtNverticalList, True,
				XtNforceColumns, True,
				XtNdefaultColumns, 1,
				XtNnumberStrings, 0,
				NULL);
	label = XtVaCreateManagedWidget("familyLabel",
			labelWidgetClass, form,
			XtNborderWidth, 0,
			XtNfromHoriz, vport,
			NULL);
	vport = XtVaCreateManagedWidget("vport",
			viewportWidgetClass, form,
			XtNuseBottom, True,
			XtNuseRight, True,
			XtNallowHoriz, True,
			XtNallowVert, True,
			XtNfromVert, label,
			XtNfromHoriz, vport,
			NULL);
	arg->family = XtVaCreateManagedWidget("font",
				listWidgetClass, vport,
				XtNverticalList, True,
				XtNforceColumns, True,
				XtNdefaultColumns, 1,
				XtNnumberStrings, 0,
				NULL);
	label = XtVaCreateManagedWidget("weightLabel",
			labelWidgetClass, form,
			XtNborderWidth, 0,
			XtNfromHoriz, vport,
			NULL);
	vport = XtVaCreateManagedWidget("vport",
			viewportWidgetClass, form,
			XtNuseBottom, True,
			XtNuseRight, True,
			XtNallowHoriz, True,
			XtNallowVert, True,
			XtNfromHoriz, vport,
			XtNfromVert, label,
			NULL);
	arg->weight = XtVaCreateManagedWidget("weight",
				listWidgetClass, vport,
				XtNverticalList, True,
				XtNforceColumns, True,
				XtNdefaultColumns, 1,
				XtNnumberStrings, 0,
				NULL);
	label = XtVaCreateManagedWidget("pointLabel",
			labelWidgetClass, form,
			XtNborderWidth, 0,
			XtNfromHoriz, vport,
			NULL);
	vport = XtVaCreateManagedWidget("vport",
			viewportWidgetClass, form,
			XtNuseBottom, True,
			XtNuseRight, True,
			XtNallowHoriz, True,
			XtNallowVert, True,
			XtNfromHoriz, vport,
			XtNfromVert, label,
			NULL);
	arg->point = XtVaCreateManagedWidget("point",
				listWidgetClass, vport,
				XtNverticalList, True,
				XtNforceColumns, True,
				XtNdefaultColumns, 1,
				XtNnumberStrings, 0,
				NULL);

	/*
	**  The text area and buttons
	*/
	form = XtVaCreateManagedWidget("form2",
			formWidgetClass, pane,
			XtNborderWidth, 0,
			NULL);

	arg->text = XtVaCreateManagedWidget("text",
			asciiTextWidgetClass, form,
			XtNallowResize, True,
			XtNstring, "",
			XtNbottom, XtChainBottom,
			NULL);

	okButton = XtVaCreateManagedWidget("apply",
			commandWidgetClass, form,
			XtNfromVert, arg->text,
			XtNtop, XtChainBottom,
			XtNright, XtChainLeft,
			NULL);

	doneButton = XtVaCreateManagedWidget("done",
			commandWidgetClass, form,
			XtNfromVert, arg->text,
			XtNfromHoriz, okButton,
			XtNtop, XtChainBottom,
			XtNleft, XtChainLeft,
			XtNright, XtChainLeft,
			NULL);

	/*
	**  Init the world
	*/
	arg->paint = paint;
	arg->namesStorage = XListFonts(XtDisplay(shell), "*", 32767, &arg->number),
	arg->names = chop(&arg->number, arg->namesStorage);
	match(arg);

	XtAddEventHandler(arg->spacing, ButtonPressMask|ButtonReleaseMask, False, expectCallback, (XtPointer)arg);
	XtAddEventHandler(arg->family, ButtonPressMask|ButtonReleaseMask, False, expectCallback, (XtPointer)arg);
	XtAddEventHandler(arg->weight, ButtonPressMask|ButtonReleaseMask, False, expectCallback, (XtPointer)arg);
	XtAddEventHandler(arg->point, ButtonPressMask|ButtonReleaseMask, False, expectCallback, (XtPointer)arg);

	XtAddCallback(arg->spacing, XtNcallback, listCallback, (XtPointer)arg);
	XtAddCallback(arg->family, XtNcallback, listCallback, (XtPointer)arg);
	XtAddCallback(arg->weight, XtNcallback, listCallback, (XtPointer)arg);
	XtAddCallback(arg->point, XtNcallback, listCallback, (XtPointer)arg);

	XtAddCallback(okButton, XtNcallback, applyCallback, (XtPointer)arg);
	XtAddCallback(doneButton, XtNcallback, closeCallback, (XtPointer)arg);
	AddDestroyCallback(shell, 
		(void (*)(Widget, void *, XEvent *))closeCallback, arg);
	
	XtPopup(shell, XtGrabNone);
}
