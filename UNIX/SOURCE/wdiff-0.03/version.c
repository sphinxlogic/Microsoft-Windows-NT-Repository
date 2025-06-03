/* wdiff -- front end to diff for comparing on a word per word basis.
   Copyright (C) 1992 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>.
*/

#include <stdio.h>

char version[] = "wdiff, version 0.03";
char copyright[] = "Copyright (C) 1992 Free Software Foundation, Inc.";

/*-----------------------------------.
| Prints a more detailed Copyright.  |
`-----------------------------------*/

void
print_copyright (void)
{
  fprintf (stderr, "\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2, or (at your option)\n\
any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
\n");
}
