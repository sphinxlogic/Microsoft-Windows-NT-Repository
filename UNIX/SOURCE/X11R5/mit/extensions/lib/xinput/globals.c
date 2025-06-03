/* libXi : libXi.c
 * exported data
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/extensions/lib/xinput/globals.c,v 1.2 1993/03/27 08:57:03 dawes Exp $
 */

#include "XI.h"         /* outside the guards to get a dependence */

#ifdef SVR3SHLIB

/***********************************************************************
 *
 * Input extension versions.
 *
 */

XExtensionVersion versions[] = {{XI_Absent,0,0},
        {XI_Present, XI_Initial_Release_Major, XI_Initial_Release_Minor},
        {XI_Present, XI_Add_XDeviceBell_Major, XI_Add_XDeviceBell_Minor},
        {XI_Present, XI_Add_XSetDeviceValuators_Major,
         XI_Add_XSetDeviceValuators_Minor},
        {XI_Present, XI_Add_XChangeDeviceControl_Major, 
         XI_Add_XChangeDeviceControl_Minor}};

/***********************************************************************
 */

#define VoidInit(var) void *var = 0

VoidInit(_libXi_free);
VoidInit(_libXi_malloc);
VoidInit(_libXi_printf);
VoidInit(_libXi_sprintf);


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
extern void _libXi_dummy() {};

#endif


