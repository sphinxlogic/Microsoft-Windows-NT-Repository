/***************************************************************************
 *
 * xautolock
 * =========
 *
 * Authors   :	S. De Troch (SDT)
 *		M. Eyckmans (MCE)
 *
 * Date      :	22/07/90
 *
 * Comments  :
 *
 * Review    :	- 12/02/92 (MCE) :
 *		  . Hacked around a dxcalendar problem.
 *		- 21/02/92 (MCE) :
 *		  . Major rewrite.
 *		- 24/02/92 (MCE) :
 *		  . Removed an initialization bug.
 *		- 25/02/92 (MCE) :
 *		  . Added code to detect multiple invocations.
 *		- 06/03/92 (MCE) :
 *		  . Re-arranged the event loop in order to detect defunct
 *		    children as soon as possible.
 *		- 10/03/92 (SDT & MCE) :
 *		  . Added code to detect broken server connections.
 *		- 24/03/92 (MCE) :
 *		  . Don't reset the time-out counter after receiving a
 *		    synthetic or otherwise unexpected event.
 *		- 15/04/92 (MCE) :
 *		  . Changed the default locker to "xlock 2>&- 1>&-".
 *		  . Fixed a couple of event mask bugs. (Thanks to
 *		    jwz@lucid.com for running into these.)
 *		  . Corrected a property type bug in CheckConnection ().
 *		- 20/04/92 (MCE) :
 *		  . Cut Main () into more managable pieces.
 *		  . Periodically call XQueryPointer ().
 *		- 25/04/92 (MCE) :
 *		  . Added the `corners' feature. (Suggested by
 *		    weisen@alw.nih.gov.)
 *		  . Fixed a problem with pseudo-root windows. (Thanks to
 *		    sherman@unx.sas.com, nedwards@titan.trl.OZ.AU,
 *		    dave@elxr.jpl.Nasa.Gov and tmcconne@sedona.intel.com
 *		    for pointing out the problem and testing the patch.)
 *		  . Added `disable/enable on SIGHUP'. (Suggested by
 *		    paul_smith@dg.com.)
 *		  . Added support for multiheaded displays.
 *		- 28/04/92 (MCE) :
 *		  . Use the X resource manager.
 *		- 06/05/92 (MCE) :
 *		  . Fixed a few potential portability problems. (Thanks
 *		    to paul_smith@dg.com again.)
 *		  . CheckConnection () now works properly on multiheaded
 *		    displays. (Thanks to brian@natinst.com for testing
 *		    the `multiheaded' feature.)
 *		  . Better version of Sleep().
 *		  . Recognize X resources for class "Xautolock".
 *		  . Don't update timer while sighupped.
 *		  . Switched to vfork () and execl ().
 *		  . New copyright notice.
 *		- 11/05/92 (MCE) :
 *		  . Close stdout and stderr in stead of using "2>&- 1>&-".
 *		    (Suggested by sinkwitz@ifi.unizh.ch.)
 *		  . Added "-noclose" for debugging.
 *
 *		- 06/11/93 (TA2 [allebrandi@Inland.com])
 *		  . On VMS the sleep() function is not giving useful results.
 *		    For example "timer" is advancing 120 counts in anywhere
 *		    from 30 to 125 seconds. Rework UpdateTimer() for VMS under
 *		    conditional compilation.
 *		- 06/14/94 (HG <goathunter@wkuvx1.wku.edu>)
 *		  . Merged in changes from Rob Spencer for Motif V1.2
 *		    <robbie@winkle.bhpese.oz.au>.
 *		- 06/20/94 (HG <goathunter@wkuvx1.wku.edu>)
 *		  . Merged in corners fix from Jerry Leichter.
 *		- 03/07/95 Ben Thomas
 *		  . Fix command line parsing - structure was freed too soon
 *		  . Make sure to redirect back to screen 0
 *		- 04/04/95 Richard Critz
 *		  . use LIB$SPAWN rather than vfork to work around DECC/VAXC
 *		    interoperability problems
 *
 * -------------------------------------------------------------------------
 *
 * Please send bug reports to detroch@imec.be or eyckmans@imec.be.
 *
 * -------------------------------------------------------------------------
 *
 * Copyright 1990, 1992 by S. De Troch and MCE.
 *
 * Permission to use, copy, modify and distribute this software and the
 * supporting documentation without fee is hereby granted, provided that :
 *
 *  1 : Both the above copyright notice and this permission notice
 *	appear in all copies of both the software and the supporting
 *	documentation.
 *  2 : You don't make a profit out of it.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL THEY BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 ***************************************************************************/



/*
 *  Have a guess what this does...
 *  ==============================
 *
 *  Warning for swm & tvtwm users : xautolock should *not* be compiled
 *  with vroot.h, because it needs to know the real root window.
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#if	(defined(VMS))
#include <string.h>
#include <time.h>
#include <processes.h>
#include "wait.h"
#define pid_t long
#include <lib$routines.h>
#include <descrip.h>
#else
#include <X11/strings.h>
#include <X11/wait.h>
#include <X11/memory.h>
#endif
#include <types.h>
#include <signal.h>
#include <math.h>

#ifdef AIXV3
#include <sys/m_wait.h>
#endif /* AIXV3 */

#if !defined (news1800) && !defined (sun386)

#include <stdlib.h>

#ifndef VMS
#ifndef apollo
#include <X11/malloc.h>
#include <X11/unistd.h>
#endif /* apollo */
#endif

#endif /* !news1800 && !sun386 */

#include "patchlevel.h"




/*
 *  Usefull macros and customization stuff
 *  ======================================
 */
#ifdef HasPrototypes
#define PP(x)			   x
#else /* HasPrototypes */
#define PP(x)			   ()
#endif /* HasPrototypes */


#define FALSE			   0	   /* as it says		   */
#define TRUE			   1	   /* as it says		   */

#ifdef VMS
#include <ssdef.h>
#define ALL_OK			   1	   /* for use by exit ()	   */
#define PROBLEMS		   SS$_ABORT   /* for use by exit ()	   */
#else
#define ALL_OK			   0	   /* for use by exit ()	   */
#define PROBLEMS		   1	   /* for use by exit ()	   */
#endif /* VMS */
#define BELL_PERCENT		   40	   /* as is says		   */
#define MIN_MINUTES		   1	   /* minimum number of minutes
					      before firing up the locker  */
#define MINUTES 		   10	   /* default ...		   */
#define MAX_MINUTES		   60	   /* maximum ...		   */
#define INITIAL_SLEEP		   20	   /* for machines on which the
			 		      login sequence takes forever */
#define INCREMENTAL_SLEEP	   1	   /* time step in seconds	   */
#define CREATION_DELAY		   30	   /* should be > 10 and
					      < min (45,(MIN_MINUTES*30))  */
#define CORNER_SIZE		   10	   /* size in pixels of the
					      force-lock areas		   */
#define CORNER_DELAY		   5	   /* number of seconds to wait
					      before forcing a lock	   */
#define LOCKER			   "xlock" /* NEVER use the -root option!  */
#if	(defined(VMS))
#define LOCKER	  "sys$system:decw$pausesession"
#endif

#define CLASS			   "Xautolock"
			 	   /* as it says		   */

/*#if SystemV == YES
#define vfork			   fork
#endif /* SystemV == YES */

#define Main			   main
#define Min(a,b)		   (a < b ? a : b)
#define forever 		   for (;;)
#define Output0(str)		   (Void) printf (str)
#define Output1(str,arg1)	   (Void) printf (str, arg1)
#define Output2(str,arg1,arg2)	   (Void) printf (str, arg1, arg2)
#define Error0(str)		   (Void) printf (str)
#define Error1(str,arg1)	   (Void) printf (str, arg1)
#define Error2(str,arg1,arg2)	   (Void) printf (str, arg1, arg2)
#if	(!defined(VMS))
#define UpdateTimer(new_val)	   if (!sighupped) timer = new_val
#endif

static void*			   ch_ptr;  /* this is dirty */
#define Skeleton(t,s)		   (ch_ptr = (Void*) malloc ((unsigned) s), \
			       ch_ptr == (Void*) NULL		    \
				    ?	(Error0 ("Out of memory.\n"),       \
			        (Void) exit (PROBLEMS),		    \
			        (t*) NULL			    \
			       ) 				    \
				    : (t*) ch_ptr			    \
				   )					    \

#define New(type)		   Skeleton (type, sizeof (type))
#define NewArray(type,nof_elems)   Skeleton (type, sizeof (type) * nof_elems)




/*
 *  New types
 *  =========
 */
#if defined (apollo) || defined (news1800)
typedef int			   (*XErrorHandler) PP((Display*,
						       XErrorEvent*));
#endif /* apollo || news1800 */

#if defined (news1800) || defined (sun386)
typedef int			   pid_t;
#endif /* news1800  || sun386 */

#define Void			   void     /* no typedef because of VAX */
typedef int			   Int;
typedef char			   Char;
typedef char*			   String;
typedef int			   Boolean;
typedef caddr_t 		   Caddrt;
typedef unsigned long		   Huge;

#ifdef HasVoidSignalReturn
#define SigRet			   Void     /* no typedef because of VAX */
#else /* HasVoidSignalReturn */
typedef Int			   SigRet;
#endif /* HasVoidSignalReturn */

typedef SigRet			   (*SigHandler) PP((/* OS dependent */));
typedef Boolean 		   (*OptAction)  PP((Display*, String,
						     String));
typedef Void			   (*OptChecker) PP((Display*));

typedef enum
	{
	  IGNORE,				  /* ignore this corner  */
	  DONT_LOCK,				  /* never lock 	 */
	  FORCE_LOCK,				  /* lock immediately	 */
	} CornerAction;

typedef struct QueueItem_
	{
	  Window		   window;	  /* as it says 	 */
	  Time			   creationtime;  /* as it says 	 */
	  struct QueueItem_*	   next;	  /* as it says 	 */
	  struct QueueItem_*	   prev;	  /* as it says 	 */
	} aQueueItem, *QueueItem;

typedef struct Queue_
	{
	  struct QueueItem_*	   head;	  /* as it says 	 */
	  struct QueueItem_*	   tail;	  /* as it says 	 */
	} aQueue, *Queue;

typedef struct Opt_
	{
	  String		   name;	  /* as it says 	 */
	  XrmOptionKind 	   kind;	  /* as it says 	 */
	  Caddrt		   value;	  /* XrmOptionNoArg only */
	  OptAction		   action;	  /* as it says 	 */
	  OptChecker		   checker;	  /* as it says 	 */
	} anOpt, *Opt;




/*
 *  Function declarations
 *  =====================
 */
#ifdef news1800
extern Void*	malloc		   PP((unsigned int));
#endif /* news1800 */

static Void	Usage		   PP((String, Int));
static Void	Sleep		   PP((Void));
static Void	EvaluateCounter    PP((Display*,Window*));
static Void	QueryPointer	   PP((Display*));
static Void	ProcessEvents	   PP((Display*, Queue));
static Queue	NewQueue	   PP((Void));
static Void	AddToQueue	   PP((Queue, Window));
static Void	ProcessQueue	   PP((Queue, Display*, Time));
static Void	SelectEvents	   PP((Display*, Window, Boolean));
static Void	CheckConnection    PP((Display*, String));
static Int	FetchFalseAlarm    PP((Display*, XEvent));
static Void	ProcessOpts	   PP((Display*, Int, String*));
static Boolean	TimeAction	   PP((Display*, String, String));
static Boolean	LockerAction	   PP((Display*, String, String));
static Boolean	CornersAction	   PP((Display*, String, String));
static Boolean	CornerSizeAction   PP((Display*, String, String));
static Boolean	CornerDelayAction  PP((Display*, String, String));
static Boolean	NotifyAction	   PP((Display*, String, String));
static Boolean	BellAction	   PP((Display*, String, String));
static Boolean	NoCloseAction	   PP((Display*, String, String));
static Boolean	HelpAction	   PP((Display*, String, String));
static Boolean	VersionAction	   PP((Display*, String, String));
static Boolean	GetPositive	   PP((String, Int*));
static Void	TimeChecker	   PP((Display*));
static Void	NotifyChecker	   PP((Display*));
static Void	CornerSizeChecker  PP((Display*));
static Void	BellChecker	   PP((Display*));
static SigRet	DisableBySignal    PP((Void));
#if	(defined(VMS))
static Void	UpdateTimer	   PP((Int));
#endif

/*
 *  Global variables
 *  ================
 */
#ifdef VMS
static Int	     completion_status;
#endif
static Time	     now = 0;		    /* number of sleeps since we
					       started (not `Int')        */
static Int	     timer = 0; 	    /* as it says (not `Time')    */
static String	     locker = LOCKER;	    /* as it says		  */
static Int	     time_limit = MINUTES;  /* as it says (not `Time')    */
static Int	     notify_margin;	    /* as it says (not `Time')    */
static Int	     bell_percent = BELL_PERCENT;
					    /* as it says		  */
static Int	     corner_size = CORNER_SIZE;
					    /* as it says		  */
static Int	     corner_delay = CORNER_DELAY;
					    /* as it says (not `Time')    */
static Boolean	     sighupped = FALSE;     /* whether to ignore all
					       time-outs		  */
static Boolean	     notify_lock = FALSE;   /* whether to notify the user
					       before locking		  */
static Boolean       ignore_corners= FALSE; /* whether to ignore any set
					       special corner action */
static CornerAction  corners[4] = { IGNORE, IGNORE, IGNORE, IGNORE };
					    /* default CornerActions	  */
static Boolean	     close_output = TRUE;   /* whether to close stdout
					       and stderr		  */
static anOpt	     options[] =
		     {
		       {"help"       , XrmoptionNoArg   ,
			(Caddrt) ""  , HelpAction       , (OptChecker) NULL},
		       {"version"    , XrmoptionNoArg   ,
			(Caddrt) ""  , VersionAction    , (OptChecker) NULL},
		       {"locker"     , XrmoptionSepArg  ,
			(Caddrt) NULL, LockerAction	, (OptChecker) NULL},
		       {"corners"    , XrmoptionSepArg  ,
			(Caddrt) NULL, CornersAction	, (OptChecker) NULL},
		       {"cornersize" , XrmoptionSepArg  ,
			(Caddrt) NULL, CornerSizeAction , CornerSizeChecker},
		       {"cornerdelay", XrmoptionSepArg  ,
			(Caddrt) NULL, CornerDelayAction, (OptChecker) NULL},
		       {"time"       , XrmoptionSepArg  ,
			(Caddrt) NULL, TimeAction	, TimeChecker	   },
		       {"notify"     , XrmoptionSepArg  ,
			(Caddrt) NULL, NotifyAction	, NotifyChecker    },
		       {"bell"       , XrmoptionSepArg  ,
			(Caddrt) NULL, BellAction	, BellChecker	   },
		       {"noclose"    , XrmoptionNoArg   ,
			(Caddrt) ""  , NoCloseAction    , (OptChecker) NULL},
		     }; 		    /* as it says, the order is
					       important		  */


#if	(defined(VMS))
static	time_t	timer_base = 0;
static	time_t	first_base = 0;
static $DESCRIPTOR(locker_d, "mcr decw$pausesession");

/*
 * UpdateTimer() for VMS
 */

static Void UpdateTimer (val)
Int	val;
{
	time_t	t;

if (!sighupped)
	{
	/*
	 * What is the current time?
	 */
	time(&t);

	/*
	 * Dispatch based on the value
	 */
	switch (val)
	 {
	 /*
	  * 0 means reset the timer
	  */
	 case 0:
		{
		timer_base = t;	/* Base the timer at now */
		timer = 0;	/* Reset the ticks since base counter */
		if (first_base == 0) /* Set the program base time */
			first_base = timer_base;
		break;
		}

	 /*
	  * -1 means compute the time since last reset
	  */
	 case -1:
		{
		if (timer_base == 0)
			UpdateTimer(0);

		timer = t - timer_base;	/* Current time - reset time = ticks */
		break;
		}

	 /*
	  * Anything else means set the timer to the given value
	  */
	 default:
		{
		timer = val;
		timer_base = t - val; /* Reset the base so that timer - base
						= val */
		break;
		}
	 }
	}
}
#endif

/*
 *  Command line argument related functions
 *  =======================================
 *
 *  Support functions
 *  -----------------
 */
static Boolean	GetPositive (arg, pos)
String	arg;  /* string to scan 		 */
Int*	pos;  /* adress where to store the stuff */

{
  Char	c;	     /* dummy		 */
  Int	old = *pos;  /* backup old value */


  if (	 sscanf (arg, "%d%c", pos, &c) == 1
      && *pos >= 0
     )
  {
    return TRUE;
  }
  else
  {
    *pos = old;
    return FALSE;
  }
}



/*
 *  Action functions
 *  ----------------
 */
/*ARGSUSED*/
static Boolean	HelpAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  Usage (name, ALL_OK);

  return TRUE;	/* for lint and gcc */
}


/*ARGSUSED*/
static Boolean	VersionAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  Error2 ("%s : patchlevel %d\n", name, PATCHLEVEL);
  (Void) exit (ALL_OK);

  return TRUE;	/* for lint and gcc */
}


/*ARGSUSED*/
static Boolean	CornerSizeAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  return GetPositive (arg, &corner_size);
}


/*ARGSUSED*/
static Boolean	CornerDelayAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  return GetPositive (arg, &corner_delay);
}


/*ARGSUSED*/
static Boolean	TimeAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  return GetPositive (arg, &time_limit);
}


/*ARGSUSED*/
static Boolean	NotifyAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  return notify_lock = GetPositive (arg, &notify_margin);
}


/*ARGSUSED*/
static Boolean	BellAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  return GetPositive (arg, &bell_percent);
}


/*ARGSUSED*/
static Boolean	NoCloseAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  close_output = FALSE;
  return TRUE;
}


/*ARGSUSED*/
static Boolean	LockerAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  locker = arg;
  return TRUE;
}


/*ARGSUSED*/
static Boolean	CornersAction (d, name, arg)
Display*  d;	 /* display pointer */
String	  name;  /* program name    */
String	  arg;	 /* argument value  */

{
  Int  c;  /* loop counter */


  if (strlen (arg) == 4)
  {
    for (c = -1; ++c < 4; )
    {
      switch (arg[c])
      {
	case '0' :
	  corners[c] = IGNORE;
	  continue;

	case '-' :
	  corners[c] = DONT_LOCK;
	  continue;

	case '+' :
	  corners[c] = FORCE_LOCK;
	  continue;

	default :
	  return FALSE;
      }
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}



/*
 *  Consistency checkers
 *  --------------------
 */
/*ARGSUSED*/
static Void  TimeChecker (d)
Display*  d;  /* display pointer */

{
  if (time_limit < MIN_MINUTES)
  {
    Error1 ("Setting time to minimum value of %d minute(s).\n",
	    time_limit = MIN_MINUTES);
  }
  else if (time_limit > MAX_MINUTES)
  {
    Error1 ("Setting time to maximum value of %d minute(s).\n",
	    time_limit = MAX_MINUTES);
  }

  time_limit *= 60; /* convert to seconds */
}


/*ARGSUSED*/
static Void  NotifyChecker (d)
Display*  d;  /* display pointer */

{
  if (	 notify_lock
      && notify_margin >= time_limit / 2
     )
  {
    Error1 ("Notification time set to %d seconds.\n",
	    notify_margin = time_limit / 2);
  }
}


/*ARGSUSED*/
static Void  BellChecker (d)
Display*  d;  /* display pointer */

{
  if (	 bell_percent < 1
      || bell_percent > 100
     )
  {
    Error1 ("Bell percentage set to %d%%.\n",
	    bell_percent = BELL_PERCENT);
  }
}


/*ARGSUSED*/
static Void  CornerSizeChecker (d)
Display*  d;  /* display pointer */

{
  Int	   s;		     /* screen index   */
  Screen*  scr; 	     /* screen pointer */
  Int	   max_corner_size;  /* as it says     */


  for (max_corner_size = 32000, s = -1; ++s < ScreenCount (d); )
  {
    scr = ScreenOfDisplay (d, s);

    if (   max_corner_size > WidthOfScreen (scr) / 4
	|| max_corner_size > HeightOfScreen (scr) / 4
       )
    {
      max_corner_size = Min (WidthOfScreen (scr), HeightOfScreen (scr)) / 4;
    }
  }

  if (corner_size > max_corner_size)
  {
    Error1 ("Corner size set to %d pixels.\n",
	    corner_size = max_corner_size);
  }
}



/*
 *  Function for informing the user about syntax errors
 *  ---------------------------------------------------
 */
static Void  Usage (prog_name, exit_code)
String	prog_name;  /* as it says */
Int	exit_code;  /* as it says */

{
  String  blanks;  /* string full of blanks */
  size_t  len;	   /* number of blanks	    */


 /*
  *  The relative overhead is enormous here, but who cares.
  *  I'm a perfectionist and Usage () doesn't return anyway.
  */
  len = strlen ("Usage : ") + strlen (prog_name) + 1;
  (Void) memset (blanks = NewArray (Char, len + 1), ' ', len);
  blanks[len] = '\0';


 /*
  *  This is where the actual work gets done...
  */
  Error0 ("\n");
  Error1 ("Usage : %s ", prog_name);
  Error0 ("[-help][-version][-time minutes][-locker locker]\n");
  Error0 (blanks);
  Error0 ("[-notify margin][-bell percent][-corners xxxx]\n");
  Error0 (blanks);
  Error0 ("[-cornerdelay secs][-cornersize pixels][-noclose]\n");

  Error0 ("\n");
  Error0 (" -help              : print this message and exit.\n");
  Error0 (" -version           : print version number and exit.\n");
  Error2 (" -time minutes      : time to lock screen [%d < minutes < %d].\n",
				 MIN_MINUTES, MAX_MINUTES);
  Error0 (" -locker locker     : program used to lock.\n");
  Error0 (" -notify margin     : beep this many seconds before locking.\n");
  Error0 (" -bell percent      : loudness of the beep.\n");
  Error0 (" -corners xxxx      : corner actions (0, +, -) in this order :\n");
  Error0 ("                      topleft topright bottomleft bottomright\n");
  Error0 (" -cornerdelay secs  : time to lock screen in a `+' corner.\n");
  Error0 (" -cornersize pixels : size of corner areas.\n");
  Error0 (" -noclose           : do not close stdout and stderr.\n");

  Error0 ("\n");
  Error0 ("Defaults :\n");

  Error0 ("\n");
  Error1 ("  time        : %d minutes\n"  , MINUTES     );
  Error1 ("  locker      : %s\n"          , LOCKER      );
  Error0 ("  notify      : don't beep\n"                );
  Error0 ("  bell        : 40%%\n"                      );
  Error0 ("  corners     : 0000\n"                      );
  Error1 ("  cornerdelay : %d seconds\n"  , CORNER_DELAY);
  Error1 ("  cornersize  : %d pixels\n"   , CORNER_SIZE );

  Error0 ("\n");

  exit (exit_code);
}



/*
 *  Function for processing command line arguments
 *  ----------------------------------------------
 */
static Void  ProcessOpts (d, argc, argv)
Display*  d;	   /* display pointer	  */
Int	  argc;    /* number of arguments */
String	  argv[];  /* array of arguments  */

{
  Int		     nof_options = sizeof (options) / sizeof (anOpt);
				  /* number of supported options   */
  Int		     j; 	  /* loop counter		   */
  Int		     l; 	  /* temporary storage		   */
  Char* 	     ptr;	  /* temporary storage		   */
  Char		     buffer[80];  /* as it says 		   */
  Char* 	     dummy;	  /* as it says 		   */
  XrmValue	     value;	  /* resource value container	   */
  XrmOptionDescList  xoptions;	  /* optionslist in Xlib format    */
  XrmDatabase	     dpydb, db = (XrmDatabase) NULL;
				  /* command line options database */


 /*
  *  Beautify argv[0].
  */
#ifdef VMS
  ptr = strrchr(argv[0], ']');
  if (ptr != NULL) {
	argv[0] = ++ptr;
	}
  ptr = strchr(argv[0], '.');
  if (ptr != NULL) {
	*ptr = '\0';
	}
#else
  for (ptr = argv[0] + strlen (argv[0]) - 1; ptr >= argv[0]; ptr--)
  {
    if (*ptr == '/')
    {
      break;
    }
  }

  argv[0] = ptr + 1;
#endif /* VMS */

 /*
  *  Calling XGetDefault () on a dummy resource is the easiest
  *  way to get both Xrm and d->db initialized.
  */
  (Void) XGetDefault (d, argv[0], "dummy");

  dpydb = XtDatabase (d);

 /*
  *  Parse the command line options into a resource database. (The
  *  command line database and the resource file database are not
  *  merged, because we want to know where exactly each resource
  *  value came from.)
  */
  xoptions = NewArray (XrmOptionDescRec, nof_options);

  for (j = -1; ++j < nof_options; )
  {
    l = strlen (options[j].name);

    (Void) sprintf (xoptions[j].option = NewArray (Char, l + 2),
		    "-%s", options[j].name);
    (Void) sprintf (xoptions[j].specifier = NewArray (Char, l + 2),
		    ".%s", options[j].name);
    xoptions[j].argKind = options[j].kind;
    xoptions[j].value = options[j].value;
  }

  XrmParseCommand (&db, xoptions, nof_options, argv[0], &argc, argv);

  if (--argc)
  {
    Usage (argv[0], PROBLEMS);
  }

/*  for (j = -1; ++j < nof_options; )
  {
    free (xoptions[j].option);
    free (xoptions[j].specifier);
  }

  free (xoptions);
*/

 /*
  *  Call the action functions.
  */
  for (j = -1; ++j < nof_options; )
  {
    (Void) sprintf (buffer, "%s%s", argv[0], xoptions[j].specifier);

    if (XrmGetResource (db, buffer, (String) NULL, &dummy, &value))
    {
      if (!(*(options[j].action)) (d, argv[0], value.addr))
      {
	Usage (argv[0], PROBLEMS);
      }
    }
    else if (XrmGetResource (dpydb, buffer, (String) NULL, &dummy, &value))
    {
      if (!(*(options[j].action)) (d, argv[0], value.addr))
      {
	Error2 ("Can't interprete \"%s\" for \"%s\", using default.\n",
		value.addr, buffer);
      }
    }
    else
    {
      (Void) sprintf (buffer, "%s%s", CLASS, xoptions[j].specifier);

      if (   XrmGetResource (dpydb, buffer, (String) NULL, &dummy, &value)
	  && !(*(options[j].action)) (d, argv[0], value.addr)
	 )
      {
	Error2 ("Can't interprete \"%s\" for \"%s\", using default.\n",
		value.addr, buffer);
      }
    }
  }

  for (j = -1; ++j < nof_options; )
  {
    free (xoptions[j].option);
    free (xoptions[j].specifier);
  }

  free (xoptions);



 /*
  *  Call the consistency checkers.
  */
  for (j = -1; ++j < nof_options; )
  {
    if (options[j].checker != (OptChecker) NULL)
    {
      (*(options[j].checker)) (d);
    }
  }
}




/*
 *  Functions related to the window queue
 *  =====================================
 *
 *  Function for creating a new queue
 *  ---------------------------------
 */
static Queue  NewQueue ()

{
  Queue  queue;  /* return value */


  queue = New (aQueue);
  queue->tail = New (aQueueItem);
  queue->head = New (aQueueItem);

  queue->tail->next = queue->head;
  queue->head->prev = queue->tail;
  queue->tail->prev = queue->head->next = (QueueItem) NULL;

  return queue;
}



/*
 *  Function for adding an item to a queue
 *  --------------------------------------
 */
static Void  AddToQueue (queue, window)
Queue	queue;	 /* as it says */
Window	window;  /* as it says */

{
  QueueItem  new;  /* new item */


  new = New (aQueueItem);

  new->window = window;
  new->creationtime = now;
  new->next = queue->tail->next;
  new->prev = queue->tail;
  queue->tail->next->prev = new;
  queue->tail->next = new;
}



/*
 *  Function for processing those entries that are old enough
 *  ---------------------------------------------------------
 */
static Void  ProcessQueue (queue, d, age)
Queue	  queue;  /* as it says      */
Display*  d;	  /* display pointer */
Time	  age;	  /* required age    */

{
  QueueItem  current;  /* as it says */


  if (now > age)
  {
    current = queue->head->prev;

    while (   current->prev
	   && current->creationtime + age < now
	  )
    {
      SelectEvents (d, current->window, False);
      current = current->prev;
      free (current->next);
    }

    current->next = queue->head;
    queue->head->prev = current;
  }
}




/*
 *  Functions related to (the lack of) user activity
 *  ================================================
 *
 *  Function for processing the event queue
 *  ---------------------------------------
 */
static Void  ProcessEvents (d, queue)
Display*  d;	  /* display pointer */
Queue	  queue;  /* as it says      */

{
  XEvent  event;  /* as it says */


 /*
  *  Read whatever is available for reading.
  */
  while (XPending (d))
  {
    if (XCheckMaskEvent (d, SubstructureNotifyMask, &event))
    {
      if (event.type == CreateNotify)
      {
	AddToQueue (queue, event.xcreatewindow.window);
      }
    }
    else
    {
      XNextEvent (d, &event);
    }


   /*
    *  Reset the counter if and only if the event is of one of
    *  the types we are expecting to get *and* was not generated by
    *  XSendEvent ().
    */
    if (   event.type == KeyPress
	&& !event.xany.send_event
       )
    {
      UpdateTimer (0);
    }
  }


 /*
  *  Check the window queue for entries that are older than
  *  CREATION_DELAY seconds.
  */
  ProcessQueue (queue, d, (Time) CREATION_DELAY);
}



/*
 *  Function for monitoring pointer movements
 *  -----------------------------------------
 */
static Void  QueryPointer (d)
Display*  d;  /* display pointer */

{
  Window	  dummy_w;	      /* as it says		       */
  Int		  dummy_c;	      /* as it says		       */
  Mask		  dummy_m;	      /* as it says		       */
  Int		  root_x;	      /* as it says		       */
  Int		  root_y;	      /* as it says		       */
  Int		  corner;	      /* corner index		       */
  Int		  i;		      /* loop counter		       */
  static Window   root; 	      /* root window the pointer is on */
  static Screen*  screen;	      /* screen the pointer is on      */
  static Int	  prev_root_x = -1;   /* as it says		       */
  static Int	  prev_root_y = -1;   /* as it says		       */
  static Boolean  first_call = TRUE;  /* as it says		       */


 /*
  *  Have a guess...
  */
  if (first_call)
  {
    first_call = FALSE;
    root = DefaultRootWindow (d);
    screen = ScreenOfDisplay (d, DefaultScreen (d));
  }


 /*
  *  Find out whether the pointer has moved. Using XQueryPointer for this
  *  is gross, but it also is the only way never to mess up propagation
  *  of pointer events.
  *
  *  Remark : Unlike XNextEvent(), XPending () doesn't notice if the
  *	      connection to the server is lost. For this reason, earlier
  *	      versions of xautolock periodically called XNoOp (). But
  *	      why not let XQueryPointer () do the job for us, since
  *	      we now call it every INCREMENTAL_SLEEP seconds anyway?
  */
  if (!XQueryPointer (d, root, &root, &dummy_w, &root_x, &root_y,
		      &dummy_c, &dummy_c, &dummy_m))
  {
   /*
    *  Pointer has moved to another screen, so let's find out which one.
    */
    for (i = -1; ++i < ScreenCount (d); )
    {
      if (root == RootWindow (d, i))
      {
	screen = ScreenOfDisplay (d, i);
	break;
      }
    }
  }

  if (	 root_x == prev_root_x
      && root_y == prev_root_y
     )
  {
   /*
    *  If the pointer has not moved since the previous call and
    *  is inside one of the 4 corners, we act according to the
    *  contents of the "corners" array - except that we first have
    *  to check the ignore_corners value.
    */
    if (   (corner = 0,
	       root_x <= corner_size
	    && root_y <= corner_size
	   )
	|| (corner++,
	       root_x >= WidthOfScreen	(screen) - corner_size - 1
	    && root_y <= corner_size
	   )
	|| (corner++,
	       root_x <= corner_size
	    && root_y >= HeightOfScreen (screen) - corner_size - 1
	   )
	|| (corner++,
	       root_x >= WidthOfScreen	(screen) - corner_size - 1
	    && root_y >= HeightOfScreen (screen) - corner_size - 1
	   )
       )
    {
      if (!ignore_corners)
      { switch (corners[corner])
	{
	case FORCE_LOCK :
	  if (timer < time_limit - corner_delay + 2)
	  {
	    UpdateTimer (time_limit - corner_delay + 2);
	  }
	  break;

	case DONT_LOCK :
	  UpdateTimer (0);
	}
      }
    }
    else /* not in a corner - stop ignoring them */
	ignore_corners = FALSE;
  }
  else
  {
    prev_root_x = root_x;
    prev_root_y = root_y;
    UpdateTimer (0);
  }
}



/*
 *  Function for deciding whether to lock
 *  -------------------------------------
 */
static Void  EvaluateCounter (d,r)
Display*  d;  /* display pointer */
Window* r;

{
  static pid_t	locker_pid = 0;  /* child pid  */
  static Time	prev_bell = 0;	 /* as it says */


 /*
  *  Find out whether we should do something special. This can
  *  be one (or more) of the following :
  *
  *   - Wait for the previous locker (if any).
  *   - Ring the bell, if we were asked to and are about to lock.
  *   - Start up a new locker if the time limit has been reached.
  */

  if (locker_pid)
  {
    union wait	status;  /* childs process status */


/*   if (!wait3 (&status, WNOHANG, (struct rusage*) NULL)) */
#ifndef VMS
     if (SmPauseWindow(d,r))
#else
     if (completion_status == 0)
#endif /* VMS */
    {
      UpdateTimer (0);
    }
    else
    {
      locker_pid = 0;
    }
  }

  if (	 notify_lock
      && timer + notify_margin > time_limit
#ifndef VMS
      && prev_bell < now - notify_margin - 1
#endif
     )
  {
    prev_bell = now;
    XBell (d, bell_percent);
    XSync (d, 0);
  }

  if (timer > time_limit)
  {
    if (!locker_pid)
    {
#ifndef VMS
      switch (locker_pid = vfork ())
      {
	case -1 :
	  locker_pid = 0;
	  break;

	case 0 :
	  (Void) close (ConnectionNumber (d));
/*	  (Void) execl ("/bin/sh", "sh", "-c", locker, (String) NULL); */
/*	  Only fire up locker if not there--may have been done manually */
	  if (!SmPauseWindow(d,r)) {
		  (Void) execl (locker, locker, (String) NULL);
          }
	  (Void) _exit (PROBLEMS);

	default :
#else
	completion_status = 0;
	locker_pid = lib$spawn(&locker_d,0,0,&1,0,0,&completion_status);
	if (!(locker_pid&1))
	  exit(locker_pid);
#endif
	UpdateTimer (0);
	ignore_corners = TRUE;	/* Ignore until movement outside */
#ifndef VMS
      }
#endif
    }
  }
}




/*
 *  Miscellaneous functions
 *  =======================
 *
 *  X Error handler
 *  ---------------
 */
/*ARGSUSED*/
static Int  FetchFalseAlarm (d, event)
Display*  d;	  /* display pointer */
XEvent	  event;  /* error event     */

{
  return 0;
}



/*
 *  SIGHUP signal handler
 *  ---------------------
 */
static SigRet  DisableBySignal ()

{
 /*
  *  The order in which things are done is rather important here.
  */
  UpdateTimer (0);
  sighupped = !sighupped;
  (Void) signal (SIGHUP, (SigHandler) DisableBySignal);

#ifndef HasVoidSignalReturn
  return 0;
#endif /* HasVoidSignalReturn */
}



/*
 *  Lazy function
 *  -------------
 */
static Void  Sleep ()

{
  Int  i;  /* loop counter */


  for (i = -1; ++i < INCREMENTAL_SLEEP; )
  {
    (Void) sleep (1);
#if	(defined(VMS))
    UpdateTimer (-1);
    now = timer + timer_base - first_base;
#else
    UpdateTimer (timer + 1);
    now++;
#endif
  }
}



/*
 *  Function for finding out whether another xautolock is already running
 *  ---------------------------------------------------------------------
 */
static Void  CheckConnection (d, prog_name)
Display*  d;	      /* display pointer */
String	  prog_name;  /* as it says	 */

{
  pid_t   pid;	      /* as it says		 */
  Int	  kill_val;   /* return value of kill () */
  Window  r;	      /* root window		 */
  Atom	  property;   /* property atom		 */
  Atom	  type;       /* property type atom	 */
  Int	  format;     /* property format	 */
  Huge	  nof_items;  /* actual number of items  */
  Huge	  after;      /* dummy			 */
  pid_t*  contents;   /* actual property value	 */


  r = RootWindowOfScreen (ScreenOfDisplay (d, 0));
  property = XInternAtom (d, "XAUTOLOCK_SEMAPHORE_PID", False);

  XGrabServer (d);
  XGetWindowProperty (d, r, property, 0L, 2L, False, AnyPropertyType,
		      &type, &format, &nof_items, &after,
		      (unsigned char**) &contents);

  if (type == XA_INTEGER)
  {
   /*
    *  This breaks if the other xautolock is not
    *  running on the same machine.
    */
    kill_val = kill (*contents, 0);

    if (kill_val == 0)
    {
      Error2 ("%s is already running (PID %d).\n",
	      prog_name, *contents);
      (Void) exit (PROBLEMS);
    }
  }

  pid = getpid ();
  XChangeProperty (d, r, property, XA_INTEGER, 8,
		   PropModeReplace, (Char*) &pid, sizeof (pid));
  XUngrabServer (d);

  XFree ((Char*) contents);
}



/*
 *  Function for selecting events on a tree of windows
 *  --------------------------------------------------
 */
static Void  SelectEvents (d, window, substructure_only)
Display*  d;		      /* display pointer   */
Window	  window;	      /* window 	   */
Boolean   substructure_only;  /* as it says	   */

{
  Window	     root;		/* root window of this window */
  Window	     parent;		/* parent of this window      */
  Window*	     children;		/* children of this window    */
  Int		     nof_children = 0;	/* number of children	      */
  Int		     i; 		/* loop counter 	      */
  XWindowAttributes  attribs;		/* attributes of the window   */


 /*
  *  Start by querying the server about parent and child windows.
  */
  if (!XQueryTree (d, window, &root, &parent, &children, &nof_children))
  {
    return;
  }


 /*
  *  Build the appropriate event mask. The basic idea is that we don't
  *  want to interfere with the normal event propagation mechanism if
  *  we don't have to.
  */
  if (substructure_only)
  {
    XSelectInput (d, window, SubstructureNotifyMask);
  }
  else
  {
    if (parent == None)  /* the *real* rootwindow */
    {
      attribs.all_event_masks =
      attribs.do_not_propagate_mask = KeyPressMask;
    }
    else if (XGetWindowAttributes (d, window, &attribs) == 0)
    {
      return;
    }

    XSelectInput (d, window,   SubstructureNotifyMask
			     | (  (  attribs.all_event_masks
				   | attribs.do_not_propagate_mask)
				& KeyPressMask));
  }


 /*
  *  Now do the same thing for all children.
  */
  for (i = -1; ++i < nof_children; )
  {
    SelectEvents (d, children[i], substructure_only);
  }

  if (nof_children != 0)
  {
    XFree ((Char*) children);
  }
}



/*
 *  Main function
 *  -------------
 */
Int  Main (argc, argv)
Int	argc;	 /* number of arguments */
String	argv[];  /* array of arguments	*/

{
  Display*  d;	    /* display pointer */
  Window    r;	    /* root window     */
  Int	    s;	    /* screen index    */
  Queue     queue;  /* as it says      */


 /*
  *  Find out whether there actually is a server on the other side...
  */
  if (	 (d = XOpenDisplay ((String) NULL))
      == (Display*) NULL
     )
  {
    (Void) exit (PROBLEMS);
  }


  fprintf(stderr, "Initializing....");

 /*
  *  Some initializations.
  */
  ProcessOpts (d, argc, argv);

  XSetErrorHandler ((XErrorHandler) FetchFalseAlarm);
  CheckConnection (d, argv[0]);
  (Void) signal (SIGHUP, (SigHandler) DisableBySignal);

  XSync (d, 0);
  fprintf(stderr, "Setting initial sleep....");
  (Void) sleep (INITIAL_SLEEP);

  queue = NewQueue ();

  for (s = -1; ++s < ScreenCount (d); )
  {
    AddToQueue (queue, r = RootWindowOfScreen (ScreenOfDisplay (d, s)));
    SelectEvents (d, r, True);
  }

/* Reset "r" to point to screen 0 */

  r = RootWindowOfScreen(ScreenOfDisplay(d,0));

  if (close_output)
  {
    (Void) fclose (stdout);
    (Void) fclose (stderr);
  }


 /*
  *  Main event loop.
  */
  forever
  {
  fprintf(stderr, "Going to sleep....");
    Sleep ();
  fprintf(stderr, "Waking up....");

    ProcessEvents (d, queue);
    QueryPointer (d);
    EvaluateCounter (d,r);
  }
}
