/* maxuxq.c
   Get the maximum number of simultaneous uuxqt executions.

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
const char _uuconf_maxuxq_rcsid[] = "$Id: maxuxq.c,v 1.3 1992/06/15 18:40:43 ian Rel $";
#endif

/* Get the maximum number of simultaneous uuxqt executions.  When
   using TAYLOR_CONFIG, this is from the ``max-uuxqts'' command in
   config.  Otherwise, when using HDB_CONFIG, we read the file
   Maxuuxqts.  */

int
uuconf_maxuuxqts (pglobal, pcmax)
     pointer pglobal;
     int *pcmax;
{
#if HAVE_TAYLOR_CONFIG
  {
    struct sglobal *qglobal = (struct sglobal *) pglobal;

    *pcmax = qglobal->qprocess->cmaxuuxqts;
    return UUCONF_SUCCESS;
  }
#else /* ! HAVE_TAYLOR_CONFIG */
#if HAVE_HDB_CONFIG
  {
    char ab[sizeof OLDCONFIGLIB + sizeof HDB_MAXUUXQTS - 1];
    FILE *e;

    *pcmax = 0;

    memcpy ((pointer) ab, (constpointer) OLDCONFIGLIB,
	    sizeof OLDCONFIGLIB - 1);
    memcpy ((pointer) (ab + sizeof OLDCONFIGLIB - 1),
	    (constpointer) HDB_MAXUUXQTS, sizeof HDB_MAXUUXQTS);
    e = fopen (ab, "r");
    if (e != NULL)
      {
	char *z;
	size_t c;

	z = NULL;
	c = 0;
	if (getline (&z, &c, e) > 0)
	  {
	    *pcmax = (int) strtol (z, (char **) NULL, 10);
	    if (*pcmax < 0)
	      *pcmax = 0;
	    free ((pointer) z);
	  }
	(void) fclose (e);
      }

    return UUCONF_SUCCESS;
  }
#else /* ! HAVE_HDB_CONFIG */
  *pcmax = 0;
  return UUCONF_SUCCESS;
#endif /* ! HAVE_HDB_CONFIG */
#endif /* ! HAVE_TAYLOR_CONFIG */
}
