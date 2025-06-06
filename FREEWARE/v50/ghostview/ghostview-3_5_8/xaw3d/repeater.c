/*
 * $XConsortium: Repeater.c,v 1.9 91/10/16 21:39:36 eswu Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 * 
 * This widget is used for press-and-hold style buttons.
 */

#ifdef VMS
#   include <X11_DIRECTORY/IntrinsicP.h>	/* for toolkit inheritance stuff */
#   include <X11_DIRECTORY/StringDefs.h>	/* for XtN and XtC defines */
#   if (XlibSpecificationRelease<5) /*###jp### inserted 22.10.94*/
#      define _XtStringDefs_h_
#   endif
#   include <XAW_DIRECTORY/XawInit.h>		/* for XawInitializeWidgetSet() */
#   include <XAW_DIRECTORY/RepeaterP.h>		/* us */
#else
#   include <X11/IntrinsicP.h>		/* for toolkit inheritance stuff */
#   include <X11/StringDefs.h>		/* for XtN and XtC defines */
#   include <X11/Xaw3d/XawInit.h>	/* for XawInitializeWidgetSet() */
#   include <X11/Xaw3d/RepeaterP.h>	/* us */
#endif

static void tic();			/* clock timeout */

#define DO_CALLBACK(rw) \
    XtCallCallbackList ((Widget) rw, rw->command.callbacks, (XtPointer)NULL)


#define ADD_TIMEOUT(rw,delay) \
  XtAppAddTimeOut (XtWidgetToApplicationContext ((Widget) rw), \
		   (unsigned long) delay, tic, (XtPointer) rw)

#define CLEAR_TIMEOUT(rw) \
  if ((rw)->repeater.timer) { \
      XtRemoveTimeOut ((rw)->repeater.timer); \
      (rw)->repeater.timer = 0; \
  }


/*
 * Translations to give user interface of press-notify...-release_or_leave
 */
static char defaultTranslations[] = 
  "<EnterWindow>:     highlight() \n\
   <LeaveWindow>:     unhighlight() \n\
   <Btn1Down>:        set() start() \n\
   <Btn1Up>:          stop() unset() ";


/*
 * Actions added by this widget
 */
static void ActionStart(), ActionStop();

static XtActionsRec actions[] = {
    { "start", ActionStart },		/* trigger timers */
    { "stop", ActionStop },		/* clear timers */
};


/*
 * New resources added by this widget
 */
static XtResource resources[] = {
#define off(field) XtOffsetOf(RepeaterRec, repeater.field)
    { XtNdecay, XtCDecay, XtRInt, sizeof (int),
	off(decay), XtRImmediate, (XtPointer) REP_DEF_DECAY },
    { XtNinitialDelay, XtCDelay, XtRInt, sizeof (int),
	off(initial_delay), XtRImmediate, (XtPointer) REP_DEF_INITIAL_DELAY },
    { XtNminimumDelay, XtCMinimumDelay, XtRInt, sizeof (int),
	off(minimum_delay), XtRImmediate, (XtPointer) REP_DEF_MINIMUM_DELAY },
    { XtNrepeatDelay, XtCDelay, XtRInt, sizeof (int),
	off(repeat_delay), XtRImmediate, (XtPointer) REP_DEF_REPEAT_DELAY },
    { XtNflash, XtCBoolean, XtRBoolean, sizeof (Boolean),
	off(flash), XtRImmediate, (XtPointer) FALSE },
    { XtNstartCallback, XtCStartCallback, XtRCallback, sizeof (XtPointer),
	off(start_callbacks), XtRImmediate, (XtPointer) NULL },
    { XtNstopCallback, XtCStopCallback, XtRCallback, sizeof (XtPointer),
	off(stop_callbacks), XtRImmediate, (XtPointer) NULL },
#undef off
};


/*
 * Class Methods
 */

static void Initialize();		/* setup private data */
static void Destroy();			/* clear timers */
static Boolean SetValues();		/* set resources */

RepeaterClassRec repeaterClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &commandClassRec,
    /* class_name		*/	"Repeater",
    /* widget_size		*/	sizeof(RepeaterRec),
    /* class_initialize		*/	XawInitializeWidgetSet,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	XtInheritResize,
    /* expose			*/	XtInheritExpose,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	defaultTranslations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* simple fields */
    /* change_sensitive		*/	XtInheritChangeSensitive
  },
  { /* threeD fields */
    /* shadowdraw		*/	XtInheritXaw3dShadowDraw
  },
  { /* label fields */
    /* ignore			*/	0
  },
  { /* command fields */
    /* ignore			*/	0
  },
  { /* repeater fields */
    /* ignore                   */	0
  }
};

WidgetClass repeaterWidgetClass = (WidgetClass) &repeaterClassRec;


/*****************************************************************************
 *                                                                           *
 *			   repeater utility routines                         *
 *                                                                           *
 *****************************************************************************/

/* ARGSUSED */
static void tic (client_data, id)
    XtPointer client_data;
    XtIntervalId *id;
{
    RepeaterWidget rw = (RepeaterWidget) client_data;

    rw->repeater.timer = 0;		/* timer is removed */
    if (rw->repeater.flash) {
	XtExposeProc expose;
	expose = repeaterWidgetClass->core_class.superclass->core_class.expose;
	XClearWindow (XtDisplay((Widget) rw), XtWindow((Widget) rw));
	rw->command.set = FALSE;
	(*expose) ((Widget) rw, (XEvent *) NULL, (Region) NULL);
	XClearWindow (XtDisplay((Widget) rw), XtWindow((Widget) rw));
	rw->command.set = TRUE;
	(*expose) ((Widget) rw, (XEvent *) NULL, (Region) NULL);
    }
    DO_CALLBACK (rw);

    rw->repeater.timer = ADD_TIMEOUT (rw, rw->repeater.next_delay);

					/* decrement delay time, but clamp */
    if (rw->repeater.decay) {
	rw->repeater.next_delay -= rw->repeater.decay;
	if (rw->repeater.next_delay < rw->repeater.minimum_delay)
	  rw->repeater.next_delay = rw->repeater.minimum_delay;
    }
}


/*****************************************************************************
 *                                                                           *
 * 			    repeater class methods                           *
 *                                                                           *
 *****************************************************************************/

/* ARGSUSED */
static void Initialize (greq, gnew, args, num_args)
    Widget greq, gnew;
    ArgList args;
    Cardinal *num_args;
{
    RepeaterWidget new = (RepeaterWidget) gnew;

    if (new->repeater.minimum_delay < 0) new->repeater.minimum_delay = 0;
    new->repeater.timer = (XtIntervalId) 0;
}

static void Destroy (gw)
    Widget gw;
{
    CLEAR_TIMEOUT ((RepeaterWidget) gw);
}

/* ARGSUSED */
static Boolean SetValues (gcur, greq, gnew, args, num_args)
    Widget gcur, greq, gnew;
    ArgList args;
    Cardinal *num_args;
{
    RepeaterWidget cur = (RepeaterWidget) gcur;
    RepeaterWidget new = (RepeaterWidget) gnew;
    Boolean redisplay = FALSE;

    if (cur->repeater.minimum_delay != new->repeater.minimum_delay) {
	if (new->repeater.next_delay < new->repeater.minimum_delay) 
	  new->repeater.next_delay = new->repeater.minimum_delay;
    }

    return redisplay;
}

/*****************************************************************************
 *                                                                           *
 * 			     repeater action procs                           *
 *                                                                           *
 *****************************************************************************/

/* ARGSUSED */
static void ActionStart (gw, event, params, num_params)
    Widget gw;
    XEvent *event;
    String *params;			/* unused */
    Cardinal *num_params;		/* unused */
{
    RepeaterWidget rw = (RepeaterWidget) gw;

    CLEAR_TIMEOUT (rw);
    if (rw->repeater.start_callbacks) 
      XtCallCallbackList (gw, rw->repeater.start_callbacks, (XtPointer)NULL);

    DO_CALLBACK (rw);
    rw->repeater.timer = ADD_TIMEOUT (rw, rw->repeater.initial_delay);
    rw->repeater.next_delay = rw->repeater.repeat_delay;
}


/* ARGSUSED */
static void ActionStop (gw, event, params, num_params)
    Widget gw;
    XEvent *event;
    String *params;			/* unused */
    Cardinal *num_params;		/* unused */
{
    RepeaterWidget rw = (RepeaterWidget) gw;

    CLEAR_TIMEOUT ((RepeaterWidget) gw);
    if (rw->repeater.stop_callbacks) 
      XtCallCallbackList (gw, rw->repeater.stop_callbacks, (XtPointer)NULL);
}

