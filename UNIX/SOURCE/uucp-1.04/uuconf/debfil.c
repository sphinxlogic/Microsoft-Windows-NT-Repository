/* debfil.c
   Get the name of the UUCP debugging file.

   Copyright (C) 1992 Ian Lance Taylor

   This file is part of the Taylor UUCP uuconf library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   The author of the program may be contacted at ian@airs.com or
   c/o Infinity Development Systems, P.O. Box 520, Waltham, MA 02254.
   */

#include "uucnfi.h"

#if USE_RCS_ID
const char _uuconf_debfil_rcsid[] = "$Id: debfil.c,v 1.3 1992/06/15 18:40:43 ian Rel $";
#endif

/* Get the name of the UUCP debugging file.  */

int
uuconf_debugfile (pglobal, pzdebug)
     pointer pglobal;
     const char **pzdebug;
{
  struct sglobal *qglobal = (struct sglobal *) pglobal;

  *pzdebug = qglobal->qprocess->zdebugfile;
  return UUCONF_SUCCESS;
}
