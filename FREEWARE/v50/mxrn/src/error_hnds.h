#ifndef ERROR_HANDLERS_H
#define ERROR_HANDLERS_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/error_hnds.h,v 1.5 1993/01/11 02:14:55 ricks Exp $
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
 * error_handlers.h: routines for error/signal handling
 *
 */

extern void warning _ARGUMENTS((char *));
extern void ehErrorExitXRN _ARGUMENTS((char *));
extern void ehSignalExitXRN _ARGUMENTS((char *));
extern void ehCleanExitXRN();
extern void ehNoUpdateExitXRN();
extern int ehErrorRetryXRN();
extern void flushPrintQueue();

/* install the X and Xtoolkit error handlers */
extern void ehInstallErrorHandlers();

/* install the signal handlers */
extern void ehInstallSignalHandlers();

#ifndef VMS
extern int errno, sys_nerr;
extern char *sys_errlist[];
#define errmsg(a) ((a < sys_nerr) ? sys_errlist[a] : "unknown error")
#else
#include <errno.h>
#include <perror.h>
/* #ifdef __DECC      
#define errmsg(a) ((a < __ERRNO_MAX) ? strerror(a) : "unknown error")
#else    */
#define errmsg(a) ((a < sys_nerr) ? sys_errlist[a] : "unknown error")
/* #endif  */
extern void ehVMSerror _ARGUMENTS((char *, int, int));
#endif

#endif /* ERROR_HANDLERS_H */
