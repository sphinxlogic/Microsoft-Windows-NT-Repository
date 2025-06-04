/* Interface to time support for VMS.
   Copyright (C) 1993 Free Software Foundation.

This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef vmstime_h
#define vmstime_h

#include <time.h>
#include <libdtdef.h>

typedef struct { unsigned long t[2]; } vms_emacs_time;

long __vms_time_secs (vms_emacs_time time);
long __vms_time_usecs (vms_emacs_time time);
void __vms_set_secs (vms_emacs_time * time, int secs);
void __vms_set_usecs (vms_emacs_time * time, int secs);
void __vms_add_time (vms_emacs_time * dest,
		     vms_emacs_time time1, vms_emacs_time time2);
void __vms_sub_time (vms_emacs_time * dest,
		     vms_emacs_time time1, vms_emacs_time time2);

extern long timezone;
extern int daylight;
#ifndef HAVE_TZNAME
extern char *tzname[2];
#define HAVE_TZNAME
#endif

void sys_tzset();
struct tm *sys_localtime(const time_t *clock);
struct tm *sys_gmtime(const time_t *clock);

#endif /* vmstime_h */
