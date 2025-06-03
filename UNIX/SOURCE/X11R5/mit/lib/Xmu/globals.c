/* libXmu : libXmu.c
 * exported data
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 *
 * $XFree86: mit/lib/Xmu/globals.c,v 1.3 1993/03/27 09:17:09 dawes Exp $
 */

#ifdef SVR3SHLIB

#ifdef __STDC__
#define Const const
#else
#define Const /**/
#endif

Const char *_shared_libXmu_version = SOXMUREV;  char __libXmu_sp1__[16] = "";

#define VoidInit(var) void *var = 0

VoidInit(_libXmu__aux__);
VoidInit(_libXmu_XShapeCombineMask);
VoidInit(_libXmu__ctype);
VoidInit(_libXmu__filbuf);
VoidInit(_libXmu__iob);
VoidInit(_libXmu_atoi);
VoidInit(_libXmu_bcopy);
VoidInit(_libXmu_calloc);
VoidInit(_libXmu_exit);
VoidInit(_libXmu_fclose);
VoidInit(_libXmu_fgets);
VoidInit(_libXmu_fopen);
VoidInit(_libXmu_fprintf);
VoidInit(_libXmu_fputs);
VoidInit(_libXmu_free);
VoidInit(_libXmu_getenv);
VoidInit(_libXmu_gethostbyname);
VoidInit(_libXmu_gethostname);
VoidInit(_libXmu_malloc);
VoidInit(_libXmu_qsort);
VoidInit(_libXmu_realloc);
VoidInit(_libXmu_sprintf);
VoidInit(_libXmu_sscanf);
VoidInit(_libXmu_strcat);
VoidInit(_libXmu_strcmp);
VoidInit(_libXmu_strcpy);
VoidInit(_libXmu_strncmp);
VoidInit(_libXmu_strncpy);
VoidInit(_libXmu_strrchr);
VoidInit(_libXmu_uname);

VoidInit(_libXmu__XA_CLASS);
VoidInit(_libXmu__XA_CLIENT_WINDOW);
VoidInit(_libXmu__XA_HOSTNAME);
VoidInit(_libXmu__XA_IP_ADDRESS);
VoidInit(_libXmu__XA_NAME);
VoidInit(_libXmu__XA_NET_ADDRESS);
VoidInit(_libXmu__XA_OWNER_OS);
VoidInit(_libXmu__XA_TARGETS);
VoidInit(_libXmu__XA_TIMESTAMP);
VoidInit(_libXmu__XA_USER);


#ifndef __GNUC__
/*
 * If we are working with floating point aritmetic, stock AT&T cc generates
 * an unresolved reference to __fltused. But we want to make a shared lib from
 * this here and don't want to reference /lib/libc_s.a, just define this sym as
 * (shared lib) static.
 * The trick is that while building the shared lib all references to this
 * symbol are resolved internally. But the symbol will be outside only visible
 * as a static one, so preventing a name conflict with other shared libs.
 */
long __fltused = 0;
#endif

/* A dummy function for free branchtab-slots.
 * This would be a great place for an error-check mechanism for shared libs.
 * (Print error message if not existent function is called.)
 */
extern void _libXmu_dummy() {};

#endif


