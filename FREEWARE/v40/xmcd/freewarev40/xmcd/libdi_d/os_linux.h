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
 *   Linux support
 *
 *   This software fragment contains code that interfaces the
 *   application to the Linux operating system.
 */
#ifndef __OS_LINUX_H__
#define __OS_LINUX_H__

#if defined(linux) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_linux_h_ident_ = "@(#)os_linux.h	6.9 98/01/02";
#endif


/* Command result word - these should be in a system header file
 * that a user program can include, but they aren't.
 */
#define status_byte(result)		(((result) >> 1) & 0xf)
#define msg_byte(result)		(((result) >> 8) & 0xff)
#define host_byte(result)		(((result) >> 16) & 0xff)
#define driver_byte(result)		(((result) >> 24) & 0xff)

/* Linux SCSI ioctl commands - these should be in a system header file
 * that a user program can include, but they aren't.
 */
#define SCSI_IOCTL_SEND_COMMAND		1
#define SCSI_IOCTL_TEST_UNIT_READY	2


#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* linux DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_LINUX_H__ */

