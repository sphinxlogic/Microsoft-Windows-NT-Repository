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
#ifndef __OS_SVR4_H__
#define __OS_SVR4_H__

#if (defined(SVR4) || defined(SVR5)) && \
    !defined(sun) && !defined(__sun__) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_os_svr4_h_ident_ = "@(#)os_svr4.h	6.13 98/05/11";
#endif

#if defined(i386) || defined(__i386__) || (defined(_FTX) && defined(__hppa))
/*
 *   UNIX SVR4.x/x86, SVR5/x86 support
 *   Stratus UNIX SVR4/PA-RISC FTX 3.x support
 *   Portable Device Interface (PDI) / Storage Device Interface (SDI)
 *
 *   This software fragment contains code that interfaces the
 *   application to the UNIX System V Release 4.x (AT&T, USL,
 *   Univel/Novell/SCO UnixWare) and UNIX System V Release 5
 *   (SCO UnixWare 7) operating systems for the Intel x86 hardware
 *   platforms and Stratus FTX 3.x on the Continuum systems.
 *   The company and product names used here are for identification
 *   purposes only.
 */

#include <sys/scsi.h>
#include <sys/sdi_edt.h>
#include <sys/sdi.h>


#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */

#endif	/* i386 _FTX __hppa */


#ifdef MOTOROLA
/*
 *   Motorola 88k UNIX SVR4 support
 *
 *   This software fragment contains code that interfaces the
 *   application to the System V Release 4 operating system from
 *   Motorola.  The name "Motorola" is used here for identification
 *   purposes only.
 */

#include <sys/param.h>
#include <sys/dk.h>


#define OS_MODULE	/* Indicate that this is compiled on a supported OS */
#define SETUID_ROOT	/* Setuid root privilege is required */


#endif	/* MOTOROLA */


/* Public function prototypes */
extern bool_t	pthru_send(int, byte_t *, int, byte_t *, int, byte_t,
			   int, bool_t);
extern int	pthru_open(char *);
extern void	pthru_close(int);
extern char	*pthru_vers(void);

#endif	/* SVR4 SVR5 sun __sun__ DI_SCSIPT DEMO_ONLY */

#endif	/* __OS_SVR4_H__ */

