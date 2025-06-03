#ifndef STUBH
#define STUBH

/*	Copyright (C) 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
/*  t. lord	Wed Jan 27 00:58:28 1993	*/


/* Stub functions are used to read arguments from a command_frame
 * and call a generic C function.
 */

#ifdef __STDC__
typedef void (*cmd_invoker) (struct command_frame *);
#else
typedef void (*cmd_invoker) ();
#endif


#ifdef __STDC__
extern cmd_invoker find_stub (void);

#else
extern cmd_invoker find_stub ();

#endif


#endif  /* STUBH */


