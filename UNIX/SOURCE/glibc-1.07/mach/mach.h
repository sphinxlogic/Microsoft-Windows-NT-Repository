/* Standard header for all Mach programs.
Copyright (C) 1993 Free Software Foundation, Inc.
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
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#ifndef	_MACH_H

#define	_MACH_H	1

/* Get the basic types used by Mach.  */
#include <mach/mach_types.h>

/* This declares the basic variables and macros everything needs.  */
#include <mach_init.h>

/* This declares all the real system call functions.  */
#include <mach/mach_traps.h>

/* These are MiG-generated headers for the kernel interfaces commonly used.  */
#include <mach/mach_interface.h> /* From <mach/mach.defs>.  */
#include <mach/mach_port.h>
#include <mach/mach_host.h>

/* For the kernel RPCs which have system call shortcut versions,
   the MiG-generated header in fact declares `CALL_rpc' rather than `CALL'.
   This file declares the simple `CALL' functions.  */
#include <mach-shortcuts.h>


/* Receive RPC request messages on RCV_NAME and pass them to DEMUX, which
   decodes them and produces reply messages.  MAX_SIZE is the maximum size
   (in bytes) of the request and reply buffers.  */
extern mach_msg_return_t
__mach_msg_server (boolean_t (*__demux) (mach_msg_header_t *__request,
					 mach_msg_header_t *__reply),
		   mach_msg_size_t __max_size,
		   mach_port_t __rcv_name),
mach_msg_server (boolean_t (*__demux) (mach_msg_header_t *__request,
				       mach_msg_header_t *__reply),
		 mach_msg_size_t __max_size,
		 mach_port_t __rcv_name);

/* Just like `mach_msg_server', but the OPTION and TIMEOUT parameters are
   passed on to `mach_msg'.  */
extern mach_msg_return_t
__mach_msg_server_timeout (boolean_t (*__demux) (mach_msg_header_t *__request,
						 mach_msg_header_t *__reply),
			   mach_msg_size_t __max_size,
			   mach_port_t __rcv_name,
			   mach_msg_option_t __option,
			   mach_msg_timeout_t __timeout),
mach_msg_server_timeout (boolean_t (*__demux) (mach_msg_header_t *__request,
					       mach_msg_header_t *__reply),
			 mach_msg_size_t __max_size,
			 mach_port_t __rcv_name,
			 mach_msg_option_t __option,
			 mach_msg_timeout_t __timeout);


#define __need_FILE
#include <stdio.h>

/* Open a stream on a Mach device.  */
extern FILE *mach_open_devstream (mach_port_t device_port, const char *mode);


#endif	/* mach.h */
