/*    xscreensaver, Copyright (c) 1993-1995 Jamie Zawinski <jwz@netscape.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

/* #### If anyone ever finishes the Athena locking code, remove this.
   But until then, locking requires Motif.
 */
#if defined(NO_MOTIF) && !defined(NO_LOCKING)
# define NO_LOCKING
#endif


#ifndef NO_LOCKING

#if __STDC__
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#endif

#ifdef  HAVE_SHADOW
#include <shadow.h>
#endif

#ifndef VMS
#include <pwd.h>
#else
#include "pwd.h"
extern char *getenv(const char *name);
extern int validate_user(char *name, char *password);
#endif

#include <stdio.h>

#include <X11/Intrinsic.h>

#include "xscreensaver.h"

extern char *screensaver_version;
extern char *progname;
extern XtAppContext app;
extern Bool verbose_p;

#ifdef SCO
/* SCO has some kind of goofy, nonstandard security crap.  This stuff was
   donated by one of their victims, I mean users, Didier Poirot <dp@chorus.fr>.
 */
# include <sys/security.h>
# include <sys/audit.h>
# include <prot.h>
#endif

#if !__STDC__
# define _NO_PROTO
#endif

#ifdef NO_MOTIF

# include <X11/StringDefs.h>
# include <X11/Xaw/Text.h>
# include <X11/Xaw/Label.h>

#else /* Motif */

# include <Xm/Xm.h>
# include <Xm/List.h>
# include <Xm/TextF.h>

#endif /* Motif */

Time passwd_timeout;

extern Widget passwd_dialog;
extern Widget passwd_form;
extern Widget roger_label;
extern Widget passwd_label1;
extern Widget passwd_label3;
extern Widget passwd_text;
extern Widget passwd_done;
extern Widget passwd_cancel;

extern create_passwd_dialog P((Widget));
extern void ungrab_keyboard_and_mouse P((void));

static enum { pw_read, pw_ok, pw_fail, pw_cancel, pw_time } passwd_state;
static char typed_passwd [1024];

static char root_passwd [255];
static char user_passwd [255];

#ifdef VMS
static char * user_vms;
#endif

#ifdef HAVE_SHADOW
# define PWTYPE struct spwd *
# define PWSLOT sp_pwdp
# define GETPW  getspnam
#else
# define PWTYPE struct passwd *
# define PWSLOT pw_passwd
# define GETPW  getpwnam
#endif

#ifdef SCO
# define PRPWTYPE struct pr_passwd *
# define GETPRPW getprpwnam
#endif

Bool
lock_init ()
{
  Bool ok = True;
  char *u;
#ifndef VMS
  PWTYPE p = GETPW ("root");

#ifdef SCO
  PRPWTYPE prpwd = GETPRPW ("root");
  if (prpwd && *prpwd->ufld.fd_encrypt)
    strcpy (root_passwd, prpwd->ufld.fd_encrypt);
#else /* !SCO */
  if (p && p->PWSLOT && p->PWSLOT[0] != '*')
    strcpy (root_passwd, p->PWSLOT);
#endif /* !SCO */
  else
    {
      fprintf (stderr, "%s: couldn't get root's password\n", progname);
      strcpy (root_passwd, "*");
    }

  /* It has been reported that getlogin() returns the wrong user id on some
     very old SGI systems... */
  u = (char *) getlogin ();
  if (u)
    {
#ifdef SCO
      prpwd = GETPRPW (u);
#endif /* SCO */
      p = GETPW (u);
    }
  else
    {
      /* getlogin() fails if not attached to a terminal;
	 in that case, use getpwuid(). */
      struct passwd *p2 = getpwuid (getuid ());
      u = p2->pw_name;
#ifdef HAVE_SHADOW
      p = GETPW (u);
#else
      p = p2;
#endif
    }

#ifdef SCO
  if (prpwd && *prpwd->ufld.fd_encrypt)
    strcpy (user_passwd, prpwd->ufld.fd_encrypt);
#else /* !SCO */
  if (p && p->PWSLOT &&
      /* p->PWSLOT[0] != '*' */		/* sensible */
      (strlen (p->PWSLOT) > 4)		/* solaris */
      )
    strcpy (user_passwd, p->PWSLOT);
#endif /* !SCO */
  else
    {
      fprintf (stderr, "%s: couldn't get password of \"%s\"\n", progname, u);
      strcpy (user_passwd, "*");
      ok = False;
    }
  return ok;
#else
  return ok;
#endif /* VMS */

}



#if defined(NO_MOTIF) || (XmVersion >= 1002)
   /* The `destroy' bug apears to be fixed as of Motif 1.2.1, but
      the `verify-callback' bug is still present. */
# define DESTROY_WORKS
#endif

static void
passwd_cancel_cb (button, client_data, call_data)
     Widget button;
     XtPointer client_data, call_data;
{
  passwd_state = pw_cancel;
}

static void
passwd_done_cb (button, client_data, call_data)
     Widget button;
     XtPointer client_data, call_data;
{
  if (passwd_state != pw_read) return; /* already done */
#ifndef VMS
  if (!strcmp ((char *) crypt (typed_passwd, user_passwd), user_passwd))
    passwd_state = pw_ok;
  /* do not allow root to have empty passwd */
  else if (typed_passwd [0] &&
	   !strcmp ((char *) crypt (typed_passwd, root_passwd), root_passwd))
    passwd_state = pw_ok;
  else
    passwd_state = pw_fail;
#else
   user_vms = getenv("USER");
   if (validate_user(user_vms,typed_passwd) == 1 ) 
       passwd_state = pw_ok;
  else 
       passwd_state = pw_fail;
#endif /* VMS */

}

#if !defined(NO_MOTIF) && defined(VERIFY_CALLBACK_WORKS)

  /* ####  It looks to me like adding any modifyVerify callback causes
     ####  Motif 1.1.4 to free the the TextF_Value() twice.  I can't see
     ####  the bug in the Motif source, but Purify complains, even if
     ####  check_passwd_cb() is a no-op.

     ####  Update: Motif 1.2.1 also loses, but in a different way: it
     ####  writes beyond the end of a malloc'ed block in ModifyVerify().
     ####  Probably this block is the text field's text.
   */

static void 
check_passwd_cb (button, client_data, call_data)
     Widget button;
     XtPointer client_data, call_data;
{
  XmTextVerifyCallbackStruct *vcb = (XmTextVerifyCallbackStruct *) call_data;

  if (passwd_state != pw_read)
    return;
  else if (vcb->reason == XmCR_ACTIVATE)
    {
      passwd_done_cb (0, 0, 0);
    }
  else if (vcb->text->length > 1)	/* don't allow "paste" operations */
    {
      vcb->doit = False;
    }
  else if (vcb->text->ptr != 0)
    {
      int i;
      strncat (typed_passwd, vcb->text->ptr, vcb->text->length);
      typed_passwd [vcb->endPos + vcb->text->length] = 0;
      for (i = 0; i < vcb->text->length; i++)
	vcb->text->ptr [i] = '*';
    }
}

#else /* !VERIFY_CALLBACK_WORKS */

static void keypress();
static void backspace();
static void kill_line();
static void done();

static XtActionsRec actions[] = {{"keypress",  keypress},
				 {"backspace", backspace},
				 {"kill_line", kill_line},
				 {"done",      done}
			        };

#if 0 /* oh fuck, why doesn't this work? */
static char translations[] = "\
<Key>BackSpace:		backspace()\n\
<Key>Delete:		backspace()\n\
Ctrl<Key>H:		backspace()\n\
Ctrl<Key>U:		kill_line()\n\
Ctrl<Key>X:		kill_line()\n\
Ctrl<Key>J:		done()\n\
Ctrl<Key>M:		done()\n\
<Key>:			keypress()\n\
";
#else
static char translations[] = "<Key>:keypress()";
#endif

static void
text_field_set_string (widget, text, position)
     Widget widget;
     char *text;
     int position;
{
#ifdef NO_MOTIF
  XawTextBlock block;
  block.firstPos = 0;
  block.length = strlen (text);
  block.ptr = text;
  block.format = 0;
  XawTextReplace (widget, 0, -1, &block);
  XawTextSetInsertionPoint (widget, position);
#else  /* !NO_MOTIF */
  XmTextFieldSetString (widget, text);
  XmTextFieldSetInsertionPosition (widget, position);
#endif /* !NO_MOTIF */
}


static void
keypress (w, event, argv, argc)
     Widget w;
     XEvent *event;
     String *argv;
     Cardinal *argc;
{
  int i, j;
  char s [sizeof (typed_passwd)];
  int size = XLookupString ((XKeyEvent *) event, s, sizeof (s), 0, 0);
  if (size != 1) return;

  /* hack because I can't get translations to dance to my tune... */
  if (*s == '\010') { backspace (w, event, argv, argc); return; }
  if (*s == '\177') { backspace (w, event, argv, argc); return; }
  if (*s == '\025') { kill_line (w, event, argv, argc); return; }
  if (*s == '\030') { kill_line (w, event, argv, argc); return; }
  if (*s == '\012') { done (w, event, argv, argc); return; }
  if (*s == '\015') { done (w, event, argv, argc); return; }

  i = j = strlen (typed_passwd);
  typed_passwd [i] = *s;
  s [++i] = 0;
  while (i--)
    s [i] = '*';

  text_field_set_string (passwd_text, s, j + 1);
}

static void
backspace (w, event, argv, argc)
     Widget w;
     XEvent *event;
     String *argv;
     Cardinal *argc;
{
  char s [sizeof (typed_passwd)];
  int i = strlen (typed_passwd);
  int j = i;
  if (i == 0)
    return;
  typed_passwd [--i] = 0;
  s [i] = 0;
  while (i--)
    s [i] = '*';

  text_field_set_string (passwd_text, s, j + 1);
}

static void
kill_line (w, event, argv, argc)
     Widget w;
     XEvent *event;
     String *argv;
     Cardinal *argc;
{
  memset (typed_passwd, 0, sizeof (typed_passwd));
  text_field_set_string (passwd_text, "", 0);
}

static void
done (w, event, argv, argc)
     Widget w;
     XEvent *event;
     String *argv;
     Cardinal *argc;
{
  passwd_done_cb (w, 0, 0);
}

#endif /* !VERIFY_CALLBACK_WORKS || NO_MOTIF */

static void
format_into_label (widget, string)
     Widget widget;
     char *string;
{
  char *label;
  char buf [255];
  Arg av[10];
  int ac = 0;

#ifdef NO_MOTIF
  XtSetArg (av [ac], XtNlabel, &label); ac++;
  XtGetValues (widget, av, ac);
#else  /* Motif */
  XmString xm_label = 0;
  XmString new_xm_label;
  XtSetArg (av [ac], XmNlabelString, &xm_label); ac++;
  XtGetValues (widget, av, ac);
  XmStringGetLtoR (xm_label, XmSTRING_DEFAULT_CHARSET, &label);
#endif /* Motif */

  if (!label || !strcmp (label, XtName (widget)))
    strcpy (buf, "ERROR: RESOURCES ARE NOT INSTALLED CORRECTLY");
  else
    sprintf (buf, label, string);

  ac = 0;

#ifdef NO_MOTIF
  XtSetArg (av [ac], XtNlabel, buf); ac++;
#else  /* Motif */
  new_xm_label = XmStringCreate (buf, XmSTRING_DEFAULT_CHARSET);
  XtSetArg (av [ac], XmNlabelString, new_xm_label); ac++;
#endif /* Motif */

  XtSetValues (widget, av, ac);
#ifndef NO_MOTIF
  XmStringFree (new_xm_label);
#endif
  XtFree (label);
}

#if __STDC__
extern void skull (Display *, Window, GC, GC, int, int, int, int);
#endif

static void
roger (button, client_data, call_data)
     Widget button;
     XtPointer client_data, call_data;
{
  Display *dpy = XtDisplay (button);
  Screen *screen = XtScreen (button);
  Window window = XtWindow (button);
  Arg av [10];
  int ac = 0;
  XGCValues gcv;
  Colormap cmap;
  GC draw_gc, erase_gc;
  unsigned int fg, bg;
  int x, y, size;
  XWindowAttributes xgwa;
  XGetWindowAttributes (dpy, window, &xgwa);
  cmap = xgwa.colormap;
  if (xgwa.width > xgwa.height) size = xgwa.height;
  else size = xgwa.width;
  if (size > 40) size -= 30;
  x = (xgwa.width - size) / 2;
  y = (xgwa.height - size) / 2;
  XtSetArg (av [ac], XtNforeground, &fg); ac++;
  XtSetArg (av [ac], XtNbackground, &bg); ac++;
  XtGetValues (button, av, ac);
  /* if it's black on white, swap it cause it looks better (hack hack) */
  if (fg == BlackPixelOfScreen (screen) && bg == WhitePixelOfScreen (screen))
    fg = WhitePixelOfScreen (screen), bg = BlackPixelOfScreen (screen);
  gcv.foreground = bg;
  erase_gc = XCreateGC (dpy, window, GCForeground, &gcv);
  gcv.foreground = fg;
  draw_gc = XCreateGC (dpy, window, GCForeground, &gcv);
  XFillRectangle (dpy, window, erase_gc, 0, 0, xgwa.width, xgwa.height);
  skull (dpy, window, draw_gc, erase_gc, x, y, size, size);
  XFreeGC (dpy, draw_gc);
  XFreeGC (dpy, erase_gc);
}

#ifdef NO_MOTIF

static void
make_passwd_dialog (parent)
     Widget parent;
{
  abort (); /* #### */
}

#else  /* Motif */

static void
make_passwd_dialog (parent)
     Widget parent;
{
  struct passwd *pw;
  create_passwd_dialog (parent);

  XtAddCallback (passwd_done, XmNactivateCallback, passwd_done_cb, 0);
  XtAddCallback (passwd_cancel, XmNactivateCallback, passwd_cancel_cb, 0);
  XtAddCallback (roger_label, XmNexposeCallback, roger, 0);

#ifdef VERIFY_CALLBACK_WORKS
  XtAddCallback (passwd_text, XmNmodifyVerifyCallback, check_passwd_cb, 0);
  XtAddCallback (passwd_text, XmNactivateCallback, check_passwd_cb, 0);
#else
  XtAddCallback (passwd_text, XmNactivateCallback, passwd_done_cb, 0);
  XtOverrideTranslations (passwd_text, XtParseTranslationTable (translations));
#endif

#if !defined(NO_MOTIF) && (XmVersion >= 1002)
  /* The focus stuff changed around; this didn't exist in 1.1.5. */
  XtVaSetValues (passwd_form, XmNinitialFocus, passwd_text, 0);
#endif

  /* Another random thing necessary in 1.2.1 but not 1.1.5... */
  XtVaSetValues (roger_label, XmNborderWidth, 2, 0);

#ifndef VMS
  pw = getpwuid (getuid ());
#else
  pw = getpwuid ();
/*  pw->pw_name = getenv("USER"); */
#endif
  format_into_label (passwd_label3, (pw->pw_name ? pw->pw_name : "???"));
  format_into_label (passwd_label1, screensaver_version);
}

#endif /* Motif */

extern void idle_timer ();

static int passwd_idle_timer_tick;
static XtIntervalId id;

static void
passwd_idle_timer (junk1, junk2)
     void *junk1;
     XtPointer junk2;
{
  Display *dpy = XtDisplay (passwd_form);
  Window window = XtWindow (passwd_form);
  static Dimension x, y, d, s, ss;
  static GC gc = 0;
  int max = passwd_timeout / 1000;

  idle_timer (junk1, junk2);

  if (passwd_idle_timer_tick == max)  /* first time */
    {
      Arg av [10];
      int ac = 0;
      XGCValues gcv;
      unsigned long fg, bg;
      XtSetArg (av [ac], XtNheight, &d); ac++;
      XtGetValues (passwd_done, av, ac);
      ac = 0;
      XtSetArg (av [ac], XtNwidth, &x); ac++;
      XtSetArg (av [ac], XtNheight, &y); ac++;
      XtSetArg (av [ac], XtNforeground, &fg); ac++;
      XtSetArg (av [ac], XtNbackground, &bg); ac++;
      XtGetValues (passwd_form, av, ac);
      x -= d;
      y -= d;
      d -= 4;
      gcv.foreground = fg;
      if (gc) XFreeGC (dpy, gc);
      gc = XCreateGC (dpy, window, GCForeground, &gcv);
      s = 360*64 / (passwd_idle_timer_tick - 1);
      ss = 90*64;
      XFillArc (dpy, window, gc, x, y, d, d, 0, 360*64);
      XSetForeground (dpy, gc, bg);
      x += 1;
      y += 1;
      d -= 2;
    }

  if (--passwd_idle_timer_tick)
    {
      id = XtAppAddTimeOut (app, 1000, passwd_idle_timer, 0);
      XFillArc (dpy, window, gc, x, y, d, d, ss, s);
      ss += s;
    }
}

extern void pop_up_dialog_box ();
extern int BadWindow_ehandler ();

static Bool
pop_passwd_dialog (parent)
     Widget parent;
{
  Display *dpy = XtDisplay (passwd_dialog);
  Window focus;
  int revert_to;
  typed_passwd [0] = 0;
  passwd_state = pw_read;
  text_field_set_string (passwd_text, "", 0);

  XGetInputFocus (dpy, &focus, &revert_to);
#ifndef DESTROY_WORKS
  /* This fucker blows up if we destroy the widget.  I can't figure
     out why.  The second destroy phase dereferences freed memory...
     So we just keep it around; but unrealizing or unmanaging it
     doesn't work right either, so we hack the window directly. FMH.
   */
  if (XtWindow (passwd_form))
    XMapRaised (dpy, XtWindow (passwd_dialog));
#endif

  pop_up_dialog_box (passwd_dialog, passwd_form, 2);
  XtManageChild (passwd_form);

#if !defined(NO_MOTIF) && (XmVersion < 1002)
  /* The focus stuff changed around; this causes problems in 1.2.1
     but is necessary in 1.1.5. */
  XmProcessTraversal (passwd_text, XmTRAVERSE_CURRENT);
#endif

  passwd_idle_timer_tick = passwd_timeout / 1000;
  id = XtAppAddTimeOut (app, 1000, passwd_idle_timer, 0);


  XGrabServer (dpy);				/* ############ DANGER! */

  /* this call to ungrab used to be in main_loop() - see comment in
      xscreensaver.c around line 696. */
  ungrab_keyboard_and_mouse ();

  while (passwd_state == pw_read)
    {
      XEvent event;
      XtAppNextEvent (app, &event);
      /* wait for timer event */
      if (event.xany.type == 0 && passwd_idle_timer_tick == 0)
	passwd_state = pw_time;
      XtDispatchEvent (&event);
    }
  XUngrabServer (dpy);
  XSync (dpy, False);				/* ###### (danger over) */

  if (passwd_state != pw_time)
    XtRemoveTimeOut (id);

  if (passwd_state != pw_ok)
    {
      char *lose;
      switch (passwd_state)
	{
	case pw_time: lose = "Timed out!"; break;
	case pw_fail: lose = "Sorry!"; break;
	case pw_cancel: lose = 0; break;
	default: abort ();
	}
#ifndef NO_MOTIF
      XmProcessTraversal (passwd_cancel, 0); /* turn off I-beam */
#endif
      if (lose)
	{
	  text_field_set_string (passwd_text, lose, strlen (lose) + 1);
	  passwd_idle_timer_tick = 1;
	  id = XtAppAddTimeOut (app, 3000, passwd_idle_timer, 0);
	  while (1)
	    {
	      XEvent event;
	      XtAppNextEvent (app, &event);
	      if (event.xany.type == 0 &&	/* wait for timer event */
		  passwd_idle_timer_tick == 0)
		break;
	      XtDispatchEvent (&event);
	    }
	}
    }
  memset (typed_passwd, 0, sizeof (typed_passwd));
  text_field_set_string (passwd_text, "", 0);
  XtSetKeyboardFocus (parent, None);

#ifdef DESTROY_WORKS
  XtDestroyWidget (passwd_dialog);
  passwd_dialog = 0;
#else
  XUnmapWindow (XtDisplay (passwd_dialog), XtWindow (passwd_dialog));
#endif
  {
    int (*old_handler) ();
    old_handler = XSetErrorHandler (BadWindow_ehandler);
    /* I don't understand why this doesn't refocus on the old selected
       window when MWM is running in click-to-type mode.  The value of
       `focus' seems to be correct. */
    XSetInputFocus (dpy, focus, revert_to, CurrentTime);
    XSync (dpy, False);
    XSetErrorHandler (old_handler);
  }

  return (passwd_state == pw_ok ? True : False);
}

Bool
unlock_p (parent)
     Widget parent;
{
  static Bool initted = False;
  if (! initted)
    {
#ifndef VERIFY_CALLBACK_WORKS
      XtAppAddActions (app, actions, XtNumber (actions));
#endif
      passwd_dialog = 0;
      initted = True;
    }
  if (! passwd_dialog)
    make_passwd_dialog (parent);
  return pop_passwd_dialog (parent);
}

#endif /* !NO_LOCKING */
