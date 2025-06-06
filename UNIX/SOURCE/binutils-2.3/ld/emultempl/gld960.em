# This shell script emits a C file. -*- C -*-
# It does some substitutions.
cat >em_${EMULATION_NAME}.c <<EOF
/* Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* 
 * emulate the Intels port of  gld
 */


#include "bfd.h"
#include "sysdep.h"


#include "ld.h"
#include "config.h"
#include "ldemul.h"
#include "ldfile.h"
#include "ldmisc.h"


/* IMPORTS */
extern char *output_filename;
extern  boolean lang_float_flag;


extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;

extern bfd *output_bfd;



#ifdef GNU960

static void
gld960_before_parse()
{
  static char *env_variables[] = { "G960LIB", "G960BASE", 0 };
  char **p;
  char *env ;

  for ( p = env_variables; *p; p++ ){
    env =  (char *) getenv(*p);
    if (env) {
      ldfile_add_library_path(concat(env,"/lib/libbout",""));
    }
  }
  ldfile_output_architecture = bfd_arch_i960;
}

#else	/* not GNU960 */

static void gld960_before_parse()
{
  char *env ;
  env =  getenv("G960LIB");
  if (env) {
    ldfile_add_library_path(env);
  }
  env = getenv("G960BASE");
  if (env) {
    ldfile_add_library_path(concat(env,"/lib",""));
  }
  ldfile_output_architecture = bfd_arch_i960;
}

#endif	/* GNU960 */


static void
gld960_set_output_arch()
{
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, bfd_mach_i960_core);
}

static char *
gld960_choose_target()
{
#ifdef GNU960

  output_filename = "b.out";
  return bfd_make_targ_name(BFD_BOUT_FORMAT, 0);

#else

  char *from_outside = getenv(TARGET_ENVIRON);
  output_filename = "b.out";

  if (from_outside != (char *)NULL)
    return from_outside;

  return "b.out.little";

#endif
}

static char *
gld960_get_script(isfile)
     int *isfile;
EOF

if test -n "$COMPILE_IN"
then
# Scripts compiled in.

# sed commands to quote an ld script as a C string.
sc='s/["\\]/\\&/g
s/$/\\n\\/
1s/^/"{/
$s/$/n}"/
'

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  extern ld_config_type config;

  *isfile = 0;

  if (config.relocateable_output == true && config.build_constructors == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xu`;
  else if (config.relocateable_output == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xr`;
  else if (!config.text_read_only)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xbn`;
  else if (!config.magic_demand_paged)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xn`;
  else
    return `sed "$sc" ldscripts/${EMULATION_NAME}.x`;
}
EOF

else
# Scripts read from the filesystem.

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  extern ld_config_type config;

  *isfile = 1;

  if (config.relocateable_output == true && config.build_constructors == true)
    return "ldscripts/${EMULATION_NAME}.xu";
  else if (config.relocateable_output == true)
    return "ldscripts/${EMULATION_NAME}.xr";
  else if (!config.text_read_only)
    return "ldscripts/${EMULATION_NAME}.xbn";
  else if (!config.magic_demand_paged)
    return "ldscripts/${EMULATION_NAME}.xn";
  else
    return "ldscripts/${EMULATION_NAME}.x";
}
EOF

fi

cat >>em_${EMULATION_NAME}.c <<EOF

struct ld_emulation_xfer_struct ld_gld960_emulation = 
{
  gld960_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_allocation_default,
  gld960_set_output_arch,
  gld960_choose_target,
  before_allocation_default,
  gld960_get_script,
  "960",
  ""
};
EOF
