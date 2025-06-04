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
 *   SunOS and Solaris support
 *
 *   This software fragment contains code that interfaces the
 *   application to the SunOS operating systems.  The name "Sun"
 *   and "SunOS" are used here for identification purposes only.
 */
#ifndef __OS_SUN_H__
#define __OS_SUN_H__

#if (defined(sun) || defined(__sun__)) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_sun_h_ident_ = "@(#)os_sun.h	6.18 98/09/26";
#endif

#ifdef SVR4

#include <sys/scsi/impl/uscsi.h>
#include <sys/dkio.h>

#ifndef SOL2_VOLMGT
#define SOL2_VOLMGT			/* Enable Solaris Vol Mgr support */
#endif

#define USCSI_STATUS_GOOD	0

#else	/* !SVR4 */

#include <scsi/impl/uscsi.h>
#undef USCSI_WRITE
#define USCSI_WRITE		0

/* This is a hack to work around a bug in SunOS 4.x's _IOWR macro
 * in <sys/ioccom.h> which makes it incompatible with ANSI compilers.
 * If Sun ever changes the definition of USCSICMD or _IOWR then
 * this will have to change...
 */
#undef _IOWR
#undef USCSICMD

#define _IOWR(x,y,t)	( \
				_IOC_INOUT | \
				((sizeof(t) & _IOCPARM_MASK) << 16) | \
				((x) << 8) | (y) \
			)
#define USCSICMD	_IOWR('u', 1, struct uscsi_cmd)

#endif	/* SVR4 */


#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);
extern bool_t	sol2_volmgt_eject(int);

#endif	/* sun __sun__ DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_SUN_H__ */

