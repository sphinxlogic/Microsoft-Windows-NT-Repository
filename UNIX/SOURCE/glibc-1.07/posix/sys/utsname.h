/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the, 1992 Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 *	POSIX Standard: 4.4 System Identification	<sys/utsname.h>
 */

#ifndef	_SYS_UTSNAME_H

#define	_SYS_UTSNAME_H	1
#include <features.h>

__BEGIN_DECLS

#include <utsnamelen.h>

/* Structure describing the system and machine.  */
struct utsname
  {
    /* Name of the implementation of the operating system.  */
    char sysname[_UTSNAME_LENGTH];

    /* Name of this node on the network.  */
    char nodename[_UTSNAME_LENGTH];

    /* Current release level of this implementation.  */
    char release[_UTSNAME_LENGTH];
    /* Current version level of this release.  */
    char version[_UTSNAME_LENGTH];

    /* Name of the hardware type the system is running on.  */
    char machine[_UTSNAME_LENGTH];
  };


/* Put information about the system in NAME.  */
extern int uname __P ((struct utsname * __name));


__END_DECLS

#endif /* sys/utsname.h  */
