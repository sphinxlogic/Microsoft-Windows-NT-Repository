#ifndef PRINTH
#define PRINTH

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
/*  t. lord	Sun Jan  3 20:19:59 1993	*/


#ifdef __STDC__
void put_ps_string (char *str, FILE *fp);
void psprint_region (FILE * fp, struct rng * rng, float wid, float hgt, char * font);
void set_page_size_cmd (char * whole_str);
void psprint_region_cmd (FILE *fp, struct rng *rng);

#else
void put_ps_string ();
void psprint_region ();
void set_page_size_cmd ();
void psprint_region_cmd ();

#endif

#endif /* PRINTH */
