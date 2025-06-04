/*
**
** string.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/


/*
#define MESSAGES
*/
#define MESSAGES
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)

#include "types.h"
#include "d_memdebug.h"

char* mprintf(char* fmt,...)
{
  va_list a;
  size_t l=1;
  char *p;

  BEGINMESSAGE(mprintf)
  va_start(a,fmt);
  for (p=fmt;*p; p++) {
    if (*p != '%') {
      l++;
      continue;
    }
    switch (*++p) {
      case 's':
        l += strlen(va_arg(a,char*));
        break;
      default:
        l++;
        break;
    }
  }
  va_end(a);
  p = GV_XtMalloc(l*sizeof(char));
  va_start(a,fmt);
  vsprintf(p,fmt,a);
  va_end(a);
  SMESSAGE(p)
  ENDMESSAGE(mprintf)
  return(p);
}

