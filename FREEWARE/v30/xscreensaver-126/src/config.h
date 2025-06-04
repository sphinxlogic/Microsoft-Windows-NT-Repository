/*
 * Config file for xscreensaver, Copyright (c) 1991-1995 Jamie Zawinski.
 * This file is included by the various Imakefiles.
 */

/*  Uncomment the following line if you have the XIDLE extension installed.
 *  This extension resides in .../contrib/extensions/xidle/ on the X11R5
 *  contrib tape.  (Turning on this flag lets XScreenSaver work better with
 *  servers which support this extension; but it will still work with servers
 *  which do not suport this extension, so it's a good idea to compile in
 *  support for it if you can.)
 */
/* #define HAVE_XIDLE_EXTENSION */

/*  Uncomment the following line if you have the MIT-SCREEN-SAVER extension
 *  installed.  This extension resides in .../contrib/extensions/screensaver/
 *  on the X11R6 contrib tape.
 *
 *  This extension does basically the same thing that the XIDLE extension does,
 *  but there are two things wrong with it: first, because of the way the 
 *  extension was designed, the `fade' option to XScreenSaver will be uglier:
 *  just before the screen fades out, there will be an unattractive flicker to
 *  black, because this extension blanks the screen *before* telling us that it
 *  is time to do so.  Second, this extension is known to be buggy; on the
 *  systems I use, it works, but some people have reported X server crashes as
 *  a result of using it.  XScreenSaver uses this extension rather
 *  conservatively, because when I tried to use any of its more complicated
 *  features, I could get it to crash the server at the drop of a hat.
 *
 *  I wish someone would port the XIDLE extension to R6.  Or I wish someone
 *  would make the MIT-SCREEN-SAVER extension not be such a piece of junk.
 *
 *  Note that the SGI X server also has an extension called SCREEN_SAVER.
 *  That is a completely different extension, and XScreenSaver contains no
 *  support for it (though it probably wouldn't be hard.)
 */
/* #define HAVE_SAVER_EXTENSION */

/*  Uncomment the following line if you have the XPM library installed.
 *  Some of the demos can make use of this if it is available.
 */
#define HAVE_XPM

/*  Uncomment the following line if you don't have Motif.  If you don't have
 *  Motif, then the screensaver won't have any dialog boxes, which means
 *  that it won't be compiled with support for demo-mode or display-locking.
 *  But other than that, it will work fine.
 */
/* #define NO_MOTIF */

/*  Uncomment the following line if for some reason the locking code doesn't
 *  work (for example, if you don't have the crypt() system call, or if you
 *  don't use standard passwd files.)  If you need to do this, please let me
 *  know.
 */
/* #define NO_LOCKING */

/*  Uncomment the following line if your system doesn't have the select()
 *  system call.  If you need to do this, please let me know.
 */
#define NO_SELECT 

/*  Uncomment the following line if your system doesn't have the setuid(),
 *  setregid(), and getpwnam() library routines.
 *
 *  WARNING: if you do this, it will be unsafe to run xscreensaver as root
 *  (which probably means you can't have it be started by xdm.)  If you are
 *  on such a system, please try to find the corresponding way to do this,
 *  and then tell me what it is.
 */
#define NO_SETUID

/*  Uncomment the following line if your system uses `shadow' passwords,
 *  that is, the passwords live in /etc/shadow instead of /etc/passwd,
 *  and one reads them with getspnam() instead of getpwnam().
 */
/* #define HAVE_SHADOW */

/*  You may need to edit these to correspond to where Motif is installed.
 */
/*#ifndef NO_MOTIF                          */
/*  MOTIFINCLUDES = -I/usr/local/include/   */
/* MOTIFLDOPTIONS = -L/usr/local/lib/       */
/*      MOTIFLIBS = -lXm                    */
/*#endif                                    */

/*  On some systems, only programs running as root can use the getpwent()
    library routine.  This means that, in order for locking to work, the
    screensaver must be installed as setuid to root.  Define this to make
    that happen.  (You must run "make install" as root for it to work.)
    (What systems other than HP and AIX need this?  Let me know.)
 */
#if defined(HPArchitecture) || defined(AIXArchitecture) || defined(HAVE_SHADOW)
# define INSTALL_SETUID
#endif

#ifdef HPArchitecture
      CCOPTIONS = -Aa -D_HPUX_SOURCE	/* eat me */
# if (ProjectX <= 4)
  MOTIFINCLUDES = -I/usr/include/Motif1.1
 MOTIFLDOPTIONS = -L/usr/lib/Motif1.1
# else /* R5 */
  MOTIFINCLUDES = -I/usr/include/Motif1.2
 MOTIFLDOPTIONS = -L/usr/lib/Motif1.2
# endif /* R5 */
#endif /* HPArchitecture */

#ifdef MacIIArchitecture
      CCOPTIONS = -D_POSIX_SOURCE
#endif /* MacIIArchitecture */

#if (ProjectX <= 4)
# define R5ISMS -DXPointer="char*"
#else /* r5 or better */
# define R5ISMS
#endif

/* It seems that some versions of Sun's dynamic X libraries are broken; if
   you get link errors about _get_wmShellWidgetClass being undefined, try
   adding -Bstatic to the link command.
 */
