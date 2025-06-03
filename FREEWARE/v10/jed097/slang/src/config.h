#ifndef _DAVIS_CONFIG_H_
#define _DAVIS_CONFIG_H_
/* 
 * Copyright (c) 1992, 1994 John E. Davis 
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#ifdef msdos
#define pc_system
#else
#if defined (__GO32__) || defined (__os2__)
#define pc_system
#endif
#endif

#define LONG long

/* #define USE_EFENCE */

#ifdef USE_EFENCE
extern char *Xstrcpy(char *, char *);
extern int Xstrcmp(char *a, char *);
extern char *Xstrncpy(char *, char *, int);
#undef HAS_MEMCPY
#undef HAS_MEMCMP
#undef HAS_MEMSET
#define strcpy Xstrcpy
#define strcmp Xstrcmp
#define strncpy Xstrncpy
#endif

#ifdef ultrix
# ifndef NO_PROTOTYPES
#  define NO_PROTOTYPES
# endif
#endif

#endif
