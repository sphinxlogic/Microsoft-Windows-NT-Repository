/*
 *	alert.c : A popup click-to-remove message box.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 17 July 1990.
 *
 *	$Id: alert.c,v 1.1 91/02/28 11:20:47 ferguson Exp $
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Cardinals.h>
#include <stdio.h>
#include <varargs.h>
extern Widget toplevel;			/* this is the only external */

/*
 * Functions defined here
 */
void alert();			/* main public routine */
void alertOk();			/* public action procedure */

static Widget popup,alerter;
static Boolean alertDone;

void
alert(va_alist)
va_dcl
{
    va_list vptr;
    char prompt[BUFSIZ],*fmt;
    Arg args[2];
    XEvent event;
    Window rwin,child;
    int rx,ry,cx,cy,but,x,y;
    Dimension w,h;

    if (popup == NULL) {
	popup = XtCreatePopupShell("popup",transientShellWidgetClass,
							toplevel,NULL,ZERO);
	alerter = XtCreateManagedWidget("alert",dialogWidgetClass,
							popup,NULL,ZERO);
	XawDialogAddButton(alerter,"okButton",alertOk,NULL);
	XtRealizeWidget(popup);							
    }
/*
 * This vsprintf() code from Dan Heller (argv@sun.com).
 */
    va_start(vptr);
    fmt = va_arg(vptr,char *);
#if defined(sun) || defined(SYSV) || defined(VPRINTF)
    vsprintf(prompt, fmt, vptr);
#else /* !VPRINTF */
    {
        /* we're on a BSD machine that has no vsprintf() */
        FILE foo;
        foo._cnt = BUFSIZ;
        foo._base = foo._ptr = prompt; /* may have to cast(unsigned char *) */
        foo._flag = _IOWRT+_IOSTRG;
        (void) _doprnt(fmt, vptr, &foo);
        *foo._ptr = '\0'; /* plant terminating null character */
    }
#endif /* VPRINTF */
    va_end (vptr);

    XtSetArg(args[0],XtNlabel,prompt);
    XtSetValues(alerter,args,ONE);
    XtSetArg(args[0],XtNwidth,&w);
    XtSetArg(args[1],XtNheight,&h);
    XtGetValues(popup,args,TWO);
    XQueryPointer(XtDisplay(toplevel),XtWindow(toplevel),
					&rwin,&child,&rx,&ry,&cx,&cy,&but);
    x = rx-w/2;
    if (x < 0)
	x = 0;
    else if (x > WidthOfScreen(XtScreen(toplevel))-w)
	x = WidthOfScreen(XtScreen(toplevel))-w;
    y = ry-h/2;
    if (y < 0)
	y = 0;
    else if (y > HeightOfScreen(XtScreen(toplevel))-h)
	y = WidthOfScreen(XtScreen(toplevel))-h;
    XtSetArg(args[0],XtNx,x);
    XtSetArg(args[1],XtNy,y);
    XtSetValues(popup,args,TWO);

    XBell(XtDisplay(toplevel),0);
    alertDone = False;
    XtPopup(popup,XtGrabExclusive);
    while (!alertDone) {
	XtAppNextEvent(XtWidgetToApplicationContext(toplevel),&event);
	XtDispatchEvent(&event);
    }
    XtPopdown(popup);
}

void
alertOk(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
    alertDone = True;
}
