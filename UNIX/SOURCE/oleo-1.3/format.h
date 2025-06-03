#ifndef FORMATH
#define FORMATH

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
/*  t. lord	Thu Jan 21 15:35:45 1993	*/



#include "global.h"
#include "cell.h"

#ifdef __STDC__
extern char * fmt_to_str (int fmt);
extern int str_to_fmt (char *ptr);
extern char * jst_to_str (int jst);
extern int chr_to_jst (int chr);

#else
extern char * fmt_to_str ();
extern int str_to_fmt ();
extern char * jst_to_str ();
extern int chr_to_jst ();

#endif

#endif /* FORMATH */

