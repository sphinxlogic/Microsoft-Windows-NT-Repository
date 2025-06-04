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
 *   Digital OpenVMS support
 *
 *   Contributing author: Rick Jones
 *   E-Mail: rjones@zko.dec.com
 *
 *   This software fragment contains code that interfaces the
 *   application to the Digital OpenVMS operating system.
 *   The terms Digital and OpenVMS are used here for identification
 *   purposes only.
 */
#ifndef __OS_VMS_H__
#define __OS_VMS_H__

#if defined(__VMS) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_vms_h_ident_ = "@(#)os_vms.h	6.12 98/05/31";
#endif

#include <fscndef>
#include <ssdef>
#include <iodef.h>

#define OS_MODULE	/* Indicate that this is compiled on a supported OS */


#define FLAGS_DISCONNECT        2
#define SCSI_STATUS_MASK        0x3e
#define GOOD_SCSI_STATUS        0


/* Public function prototypes */

extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* __VMS DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_VMS_H__ */

