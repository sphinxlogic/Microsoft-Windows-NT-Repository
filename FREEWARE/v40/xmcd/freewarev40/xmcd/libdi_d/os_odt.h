/*
 *   libdi - scsipt SCSI Device Interface Library
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free
 *   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 *   SCO UNIX/Open Desktop/Open Server support
 *
 *   This software fragment contains code that interfaces the
 *   application to the SCO UNIX operating system.  The name "SCO" and
 *   "ODT" are used here for identification purposes only.
 */
#ifndef __OS_ODT_H__
#define __OS_ODT_H__

#if defined(sco) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_odt_h_ident_ = "@(#)os_odt.h	6.9 98/01/02";
#endif

#include <sys/scsi.h>
#include <sys/scsicmd.h>


#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* sco DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_ODT_H__ */

