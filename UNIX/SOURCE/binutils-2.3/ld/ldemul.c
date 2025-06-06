/* clearing house for ld emulation states
Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"

#include "config.h"
#include "ld.h"
#include "ldemul.h"
#include "ldmisc.h"
#include "ldemul-list.h"

ld_emulation_xfer_type *ld_emulation;
extern bfd *output_bfd;
extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;

void
ldemul_hll(name)
char *name;
{
  ld_emulation->hll(name);
}


void ldemul_syslib(name)
char *name;
{
  ld_emulation->syslib(name);
}

void
ldemul_after_parse()
{
  ld_emulation->after_parse();
}

void
ldemul_before_parse()
{
  ld_emulation->before_parse();
}

void 
ldemul_after_allocation()
{
  ld_emulation->after_allocation();
}

void 
ldemul_before_allocation()
{
  if (ld_emulation->before_allocation) {
    ld_emulation->before_allocation();
  }
}


void
ldemul_set_output_arch()
{
  ld_emulation->set_output_arch();
}

void
ldemul_finish()
{
  if (ld_emulation->finish) {
    ld_emulation->finish();
  }
}

void
ldemul_create_output_section_statements()
{
  if (ld_emulation->create_output_section_statements) {
    ld_emulation->create_output_section_statements();
  }
}

char *
ldemul_get_script(isfile)
     int *isfile;
{
  return ld_emulation->get_script(isfile);
}

char *
ldemul_choose_target()
{
  return ld_emulation->choose_target();
}

/* The default choose_target function.  */

char *
ldemul_default_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return ld_emulation->target_name;
}

void 
after_parse_default()
{

}

void
after_allocation_default()
{

}

void
before_allocation_default()
{

}

void
set_output_arch_default()
{
  /* Set the output architecture and machine if possible */
  bfd_set_arch_mach(output_bfd,
	            ldfile_output_architecture, ldfile_output_machine);
}

void
syslib_default(ignore)
     char  *ignore;
{
  info_msg ("%S SYSLIB ignored\n");
}

void
hll_default(ignore)
     char  *ignore;
{
  info_msg ("%S HLL ignored\n");
}

ld_emulation_xfer_type *ld_emulations[] = { EMULATION_LIST };

void
ldemul_choose_mode(target)
char *target;
{
    ld_emulation_xfer_type **eptr = ld_emulations;
    /* Ignore "gld" prefix. */
    if (target[0] == 'g' && target[1] == 'l' && target[2] == 'd')
	target += 3;
    for (; *eptr; eptr++) {
	if (strcmp(target, (*eptr)->emulation_name) == 0) {
	    ld_emulation = *eptr;
	    return;
	}
    }
    einfo("%P%F: unrecognised emulation mode: %s\n",target);
}
