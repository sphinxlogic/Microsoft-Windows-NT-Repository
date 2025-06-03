#ifndef _JED_CONFIG_H_
#define _JED_CONFIG_H_
/*
 *  Copyright (c) 1993, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#if !defined(unix) && !defined(VMS) && !defined(msdos) && !defined(__os2__)
# define unix
#endif

#ifdef unix
# ifndef NO_UNISTD_H
#  include <unistd.h>
# endif
#endif

#ifndef NO_STDLIB_H
# include <stdlib.h>
#endif

#if defined(msdos) || defined (__GO32__) || defined (__os2__)
#  define pc_system
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#if defined(_MSC_VER) || defined(__EMX__)
#define strcmpi stricmp
#define strncmpi strnicmp
#endif

#ifndef HAS_MOUSE
#if !defined(pc_system) 
#define HAS_MOUSE
#endif
#endif

#define LONG long

/* #define USE_EFENCE */

#ifdef USE_EFENCE
extern char *Xstrcpy(char *, char *);
extern int Xstrcmp(char *a, char *b);
extern char *Xstrncpy(char *, char *, int);
#undef HAS_MEMCPY
#undef HAS_MEMCMP
#undef HAS_MEMSET
#define strcpy Xstrcpy
#define strcmp Xstrcmp
#define strncpy Xstrncpy
#endif

#endif
