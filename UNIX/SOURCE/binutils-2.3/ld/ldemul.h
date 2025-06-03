/* ld-emul.h - Linker emulation header file

   Copyright 1991, 1992 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.  */

void
ldemul_hll PARAMS ((char *));

void
ldemul_syslib PARAMS ((char *));

void
ldemul_after_parse PARAMS ((void));

void
ldemul_before_parse PARAMS ((void));

void
ldemul_after_allocation PARAMS ((void));

void
ldemul_before_allocation PARAMS ((void));

void
ldemul_set_output_arch PARAMS ((void));

char *
ldemul_choose_target PARAMS ((void));

void
ldemul_choose_mode PARAMS ((char *));

char *
ldemul_get_script PARAMS ((int *isfile));

void
ldemul_finish PARAMS ((void));

char
*ldemul_default_target PARAMS ((void));

void
after_parse_default PARAMS ((void));

void
after_allocation_default PARAMS ((void));

void
before_allocation_default PARAMS ((void));

void
set_output_arch_default PARAMS ((void));

void
syslib_default PARAMS ((char*));

void
hll_default PARAMS ((char*));

typedef struct ld_emulation_xfer_struct
{
  void   (*before_parse) PARAMS ((void));
  void   (*syslib) PARAMS ((char *));
  void   (*hll) PARAMS ((char *));
  void   (*after_parse) PARAMS ((void));
  void   (*after_allocation) PARAMS ( (void));
  void   (*set_output_arch) PARAMS ((void));
  char * (*choose_target) PARAMS ((void));
  void   (*before_allocation) PARAMS ((void));
  char * (*get_script) PARAMS ((int *isfile));
  char *emulation_name;
  char *target_name;
  void	(*finish) PARAMS ((void));
  void	(*create_output_section_statements) PARAMS ((void));
} ld_emulation_xfer_type;

typedef enum 
{
  intel_ic960_ld_mode_enum,
  default_mode_enum ,
  intel_gld960_ld_mode_enum
} lang_emulation_mode_enum_type;
