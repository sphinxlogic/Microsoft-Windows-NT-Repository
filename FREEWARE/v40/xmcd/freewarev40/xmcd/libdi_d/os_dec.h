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
 *   Digital UNIX (OSF/1) and Ultrix support
 *
 *   Contributing author: Matt Thomas
 *   E-Mail: thomas@lkg.dec.com
 *
 *   This software fragment contains code that interfaces the
 *   application to the Digital UNIX and Ultrix operating systems.
 *   The term Digital, Ultrix and OSF/1 are used here for identification
 *   purposes only.
 */
#ifndef __OS_DEC_H__
#define __OS_DEC_H__

#if ((defined(__alpha) && defined(__osf__)) || \
     defined(ultrix) || defined(__ultrix)) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_dec_h_ident_ = "@(#)os_dec.h	6.10 98/01/02";
#endif

#ifdef __osf__
#include <io/common/iotypes.h>
#else /* must be ultrix */
#include <sys/devio.h>
#endif
#include <io/cam/cam.h>
#include <io/cam/uagt.h> 
#include <io/cam/dec_cam.h>
#include <io/cam/scsi_all.h>  


#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* setuid root privilege is required */


#define DEV_CAM		"/dev/cam"	/* CAM device path */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* __alpha __osf__ ultrix __ultrix DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_DEC_H__ */

