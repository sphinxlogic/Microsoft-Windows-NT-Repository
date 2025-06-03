/* @(#)acconfig.h	1.2 (11 Oct 1993) */

/* LOCAL acconfig.h

   Descriptive text for the C preprocessor macros that
   the local (non-standard) code in configure.in can define.

   This file is read by autoheader in addition to the standard acconfig.h,
   which covers the stuff in the standard autoconf macros.

   Leave the following blank line there!!  Autoheader needs it.  */


/* This appears to have been left out of the standard acconfig.h, as the
 * standard AC_ALLOCA references it:
 */
#undef CRAY_STACKSEG_END

#undef HAVE_ANSI_PROTOS

#undef HAVE_RESOLVER

#undef HAVE_VOID

#undef HAVE_VOID_POINTER

/* This also appears to have been left out of the standard acconfig.h */
#undef HAVE_TIME_WITH_SYS_TIME


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */
