/* Copyright (C) 1992, 1993 Free Software Foundation, Inc.
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

#include <hurd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


struct _hurd_port *_hurd_ports;
unsigned int _hurd_nports;
mode_t _hurd_umask;

void _hurd_proc_init (char **argv);

/* Initialize the library data structures from the
   ints and ports passed to us by the exec server.

   PORTARRAY and INTARRAY are vm_deallocate'd.  */

void
_hurd_init (int flags, char **argv,
	    mach_port_t *portarray, size_t portarraysize,
	    int *intarray, size_t intarraysize)
{
  int i;

  _hurd_ports = malloc (portarraysize * sizeof (*_hurd_ports));
  if (_hurd_ports == NULL)
    __libc_fatal ("Can't allocate _hurd_ports\n");
  _hurd_nports = portarraysize;

  /* See what ports we were passed.  */
  for (i = 0; i < portarraysize; ++i)
    {
      _hurd_port_init (&_hurd_ports[i], portarray[i]);

      switch (i)
	{
	case INIT_PORT_PROC:
	  /* Tell the proc server we exist, if it does.  */
	  if (portarray[i] != MACH_PORT_NULL)
	    _hurd_proc_init (argv);
	  break;

	case INIT_PORT_BOOTSTRAP:
	  /* When the user asks for the bootstrap port,
	     he will get the one the exec server passed us.  */
	  __task_set_special_port (__mach_task_self (),
				   TASK_BOOTSTRAP_PORT, portarray[i]);
	  break;
	}
    }

  if (intarraysize > INIT_UMASK)
    _hurd_umask = intarray[INIT_UMASK] & 0777;
  else
    _hurd_umask = CMASK;

  /* All done with init ints and ports.  */
  __vm_deallocate (__mach_task_self (),
		   (vm_address_t) intarray,
		   intarraysize * sizeof (int));
  __vm_deallocate (__mach_task_self (),
		   (vm_address_t) portarray,
		   portarraysize * sizeof (mach_port_t));

  if (flags & EXEC_SECURE)
    /* XXX if secure exec, elide environment variables
       which the library uses and could be security holes.
       CORESERVER, COREFILE
       */ ;
}

/* The user can do "int _hide_arguments = 1;" to make
   sure the arguments are never visible with `ps'.  */
int _hide_arguments, _hide_environment;

/* Do startup handshaking with the proc server just installed in _hurd_ports.
   Call _hurdsig_init to set up signal processing.  */

void
_hurd_proc_init (char **argv)
{
  mach_port_t oldmsg;
  int dealloc;
  process_t procserver;

#ifdef notyet
  /* Initialize the signal code; Mach exceptions will become signals.
     This function will be a no-op on calls after the first.
     On the first call, it sets up the message port and the signal thread.  */
  _hurdsig_init ();
#endif

  /* The signal thread is now prepared to receive messages.
     It is safe to give the port to the proc server.  */

  procserver = _hurd_port_get (&_hurd_ports[INIT_PORT_PROC], &dealloc);

#ifdef notyet
  /* Give the proc server our message port.  */
  __proc_setmsgport (procserver, _hurd_msgport, &oldmsg);
  if (oldmsg != MACH_PORT_NULL)
    /* Deallocate the old msg port we replaced.  */
    __mach_port_deallocate (__mach_task_self (), oldmsg);
#endif

  /* Tell the proc server where our args and environment are.  */
  __proc_setprocargs (procserver,
		      _hide_arguments ? 0 : (vm_address_t) argv,
		      _hide_environment ? 0 : (vm_address_t) __environ);

  _hurd_port_free (&_hurd_ports[INIT_PORT_PROC], &dealloc, procserver);

#ifdef notyet
  /* Initialize proc server-assisted fault recovery for the signal thread.  */
  _hurdsig_fault_init ();
#endif
}
