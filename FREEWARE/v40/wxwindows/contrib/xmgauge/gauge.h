/* Copyright 1994 GROUPE BULL -- See license conditions in file COPYRIGHT */
/* $Id: */
#ifndef __GAUGE_H
#define __GAUGE_H



/****************************************************************
 *
 * Gauge widget public header
 *
 ****************************************************************/


typedef struct _XmGaugeClassRec*	XmGaugeWidgetClass;
typedef struct _XmGaugeRec*	        XmGaugeWidget;

#ifdef __cplusplus
extern "C" WidgetClass xmGaugeWidgetClass;
#else
extern WidgetClass xmGaugeWidgetClass;
#endif

typedef struct _XmGaugeCallbackStruct{
    int reason;
    XEvent *event;
    int value;
} XmGaugeCallbackStruct;


void
XmGaugeSetValue(Widget w, int value);

int
XmGaugeGetValue(Widget w);



#endif /* __GAUGE_H */
