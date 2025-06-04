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
 *   Siemens Pyramid SINIX (Reliant UNIX) support
 *
 *   Contributing author: Eckhard Einert
 *   E-Mail: einert.pad@sni.de
 *
 *   This software fragment contains code that interfaces the
 *   application to the SINIX operating system.  The names
 *   "Siemens Pyramid" and "SINIX" are used here for identification
 *   purposes only.
 */
#ifndef __OS_SINIX_H__
#define __OS_SINIX_H__

#if defined(SNI) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_sinix_h_ident_ = "@(#)os_sinix.h	6.11 98/03/24";
#endif

#include <sys/ioccom.h>
#include <sys/cpl/ios_ioi.h>

#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* SNI DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_SINIX_H__ */

