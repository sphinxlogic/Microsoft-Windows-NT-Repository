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
 *   BSDI BSD/OS support
 *
 *   Contributing author: Danny Braniss
 *   E-Mail: danny@cs.huji.ac.il
 *
 *   This software fragment contains code that interfaces the
 *   application to the BSDI BSD/OS (version 2.0 or later)
 *   operating system.
 */
#ifndef __OS_BSDI_H__
#define __OS_BSDI_H__

#if defined(__bsdi__) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_bsdi_h_ident_ = "@(#)os_bsdi.h	6.11 98/01/02";
#endif

#include <dev/scsi/scsi.h>
#include <dev/scsi/scsi_ioctl.h>

#ifdef SCSIRAWCDB
#define OS_BSDI_3	/* BSDI 3.x SCSI subsystem */
#else
#define OS_BSDI_2	/* BSDI 2.x SCSI subsystem */
#endif

#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* __bsdi__ DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_BSDI_H__ */

