#ifndef UTILS_H
#define UTILS_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/utils.h,v 1.8 1993/02/04 18:22:36 ricks Exp $
 */

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * utils.h: random utility functions and macros for xrn
 */

/*
 * for XtMalloc, etc.
 */
#ifndef MOTIF
#ifndef VMS
#include <X11/Intrinsic.h>
#else	/* VMS */
#include <decw$include:Intrinsic.h>
#endif	/* VMS */
#else	/* MOTIF */
#include <X11/Intrinsic.h>
#endif	/* MOTIF */

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#if defined(_ANSI_C_SOURCE)
#if defined(__STDC__)
typedef void    (*SIG_PF0) (int);
#else /* __STDC__ */
typedef void    (*SIG_PF0) ();
#endif /* __STDC__ */
#else /* _ANSI_C_SOURCE */
#if defined(SYSV) || defined(SVR4)
#if defined(__STDC__)
typedef void    (*SIG_PF0) (int, ...);
#else /* __STDC__ */
typedef void    (*SIG_PF0) ();
#endif /* __STDC__ */
#else /* SYSV || SVR4 */
#if defined(__STDC__)
typedef int     (*SIG_PF0) (int, ...);
#else /* __STDC__ */
typedef int     (*SIG_PF0) ();
#endif /* __STDC__ */
#endif /* SYSV || SVR4 */
#endif /* _ANSI_C_SOURCE, SYSV/SVR4, __STDC__ */

#if !defined(_POSIX_SOURCE)
extern char *strtok _ARGUMENTS((char *, char const *));
extern char *getenv _ARGUMENTS((const char *));
#else
#ifndef NOSTDHDRS
#include <stdlib.h>
#endif /* NOSTDHDRS */
#endif /* !_POSIX_SOURCE */
#if !defined(_XOPEN_SOURCE)
extern char *mktemp _ARGUMENTS((char *));
#else
#define mktemp(string)    tmpnam(string)
#endif
#if !defined(_OSF_SOURCE)
extern char *index _ARGUMENTS((const char *, int));
extern char *rindex _ARGUMENTS((const char *, int));
#else
#include <string.h>
#endif

#ifdef macII
extern int strcmp();
#endif

/* allocation macros */
#define ALLOC(type)           (type *) XtMalloc((unsigned) sizeof(type))
#define ARRAYALLOC(type, sz)  (type *) XtMalloc((unsigned) (sizeof(type) * (sz)))
#define NIL(type)             (type *) 0
#define FREE(item)            if ((char *) item != NIL(char)) XtFree((char *) item), item = 0
#ifdef VMS
extern int utGroupToVmsFilename(char *filename, char *group);
#endif
#define STREQ(a,b)            (strcmp(a, b) == 0)
#define STREQN(a,b,n)         (strncmp(a, b, n) == 0)

extern char *utTrimSpaces _ARGUMENTS((char *));
extern char *utNameExpand _ARGUMENTS((char *));
extern char *utTildeExpand _ARGUMENTS((char *));
extern int utSubstring _ARGUMENTS((char *, char *));
extern void utDowncase _ARGUMENTS((char *));
extern void utLowerCase _ARGUMENTS((char *, char *, int));
extern void utCopyFile _ARGUMENTS((char *, char *));

#define utStrlen(s)	((s) ? strlen(s) : 0)

#if defined(__STDC__) && __STDC__
extern int utSubjectCompare _ARGUMENTS((const char *, const char *));
#else
extern int utSubjectCompare ();
#endif

#ifdef NEED_TEMPNAM
extern char *utTempnam _ARGUMENTS((char *, char *));
#endif

#ifdef SYSV_REGEX
extern char *regcmp();
#else
extern char *re_comp();
extern int re_exec();
#endif

#if !defined(ultrix) && !defined(VMS) && !defined(__osf__)
#if defined(__STDC__)
extern FILE * popen _ARGUMENTS((const char *, const char *));
#else
extern FILE * popen _ARGUMENTS((char *, char *));
#endif
extern int pclose _ARGUMENTS((FILE *));
#endif

# if defined(NEED_STRCASECMP) && !defined(__osf__)
extern int strcasecmp _ARGUMENTS((const char *, const char *));
extern int strncasecmp _ARGUMENTS((const char *, const char *, size_t));
#endif

#if defined __STDC__ && !defined(VMS)
#include <sys/time.h>
extern int gettimeofday _ARGUMENTS((struct timeval *, struct timezone *));
#endif

extern int tconvert _ARGUMENTS((char *, char *));
extern char * string_pool _ARGUMENTS((char *));
#ifdef XLATE
extern void utXlate _ARGUMENTS((char *));
extern void utUnXlate _ARGUMENTS ((char *));
#endif

#endif /* UTILS_H */
