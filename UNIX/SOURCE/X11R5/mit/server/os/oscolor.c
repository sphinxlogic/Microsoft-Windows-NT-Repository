/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: mit/server/os/oscolor.c,v 2.4 1994/02/10 21:27:30 dawes Exp $ */
/* $XConsortium: oscolor.c,v 1.20 91/06/30 15:58:30 rws Exp $ */


#ifndef USE_RGB_TXT

#ifdef AMOEBA
#include <sdbm.h>
#else
#ifdef NDBM
#include <X11/Xos.h>
#if (BSD >= 199103)
#include <sys/types.h>
#endif
#ifdef SDBM
#include <sdbm.h>
#else
#include <ndbm.h>
#endif
#else
#if defined(SVR4) || defined(ISC)
#include <rpcsvc/dbm.h>
#else
#include <dbm.h>
#endif
#endif
#endif /* AMOEBA */
#include "rgb.h"
#include "os.h"
#include "opaque.h"

/* Note that we are assuming there is only one database for all the screens. */

#ifdef NDBM
DBM *rgb_dbm = (DBM *)NULL;
#else
int rgb_dbm = 0;
#endif

extern void CopyISOLatin1Lowered();

int
OsInitColors()
{
    if (!rgb_dbm)
    {
#ifdef NDBM
	rgb_dbm = dbm_open(rgbPath, 0, 0);
#else
	if (dbminit(rgbPath) == 0)
	    rgb_dbm = 1;
#endif
	if (!rgb_dbm) {
	    ErrorF( "Couldn't open RGB_DB '%s'\n", rgbPath );
	    return FALSE;
	}
    }
    return TRUE;
}

/*ARGSUSED*/
int
OsLookupColor(screen, name, len, pred, pgreen, pblue)
    int		screen;
    char	*name;
    unsigned	len;
    unsigned short	*pred, *pgreen, *pblue;

{
    datum		dbent;
    RGB			rgb;
    char		buf[64];
    char		*lowername;

    if(!rgb_dbm)
	return(0);

    /* we use Xalloc here so that we can compile with cc without alloca
     * when otherwise using gcc */
    if (len < sizeof(buf))
	lowername = buf;
    else if (!(lowername = (char *)Xalloc(len + 1)))
	return(0);
    CopyISOLatin1Lowered ((unsigned char *) lowername, (unsigned char *) name,
			  (int)len);

    dbent.dptr = lowername;
    dbent.dsize = len;
#ifdef NDBM
    dbent = dbm_fetch(rgb_dbm, dbent);
#else
    dbent = fetch (dbent);
#endif

    if (len >= sizeof(buf))
	Xfree(lowername);

    if(dbent.dptr)
    {
	bcopy(dbent.dptr, (char *) &rgb, sizeof (RGB));
	*pred = rgb.red;
	*pgreen = rgb.green;
	*pblue = rgb.blue;
	return (1);
    }
    return(0);
}


#else /* USE_RGB_TXT */


/*
 * Sorry, but I can't stand it anymore !!! Why using a libdbm thing, indstead
 * of reading the plain rgb.txt and create the database online while starting
 * the server ?? As result we would have fewer disk accesses ...
 */

#include <stdio.h>
#include "os.h"
#include "opaque.h"

#define HASHSIZE 511

typedef struct _dbEntry * dbEntryPtr;
typedef struct _dbEntry {
  dbEntryPtr     link;
  unsigned short red;
  unsigned short green;
  unsigned short blue;
  char           name[1];	/* some compilers complain if [0] */
} dbEntry;


extern void CopyISOLatin1Lowered();

static dbEntryPtr hashTab[HASHSIZE];


static dbEntryPtr
lookup(name, len, create)
     char *name;
     int  len;
     Bool create;
{
  unsigned int h = 0, g;
  dbEntryPtr   entry, *prev;
  char         *str = name;

  if (!(name = (char*)ALLOCATE_LOCAL(len +1))) return NULL;
  CopyISOLatin1Lowered(name, str, len);
  name[len] = '\0';

  for(str = name; *str; str++) {
    h = (h << 4) + *str;
    if ((g = h) & 0xf0000000) h ^= (g >> 24);
    h &= g;
  }
  h %= HASHSIZE;

  if ( entry = hashTab[h] )
    {
      for( ; entry; prev = (dbEntryPtr*)entry, entry = entry->link )
	if (! strcmp(name, entry->name) ) break;
    }
  else
    prev = &(hashTab[h]);

  if (!entry && create && (entry = (dbEntryPtr)Xalloc(sizeof(dbEntry) +len)))
    {
      *prev = entry;
      entry->link = NULL;
      strcpy( entry->name, name );
    }

  DEALLOCATE_LOCAL(name);

  return entry;
}


Bool
OsInitColors()
{
  FILE       *rgb;
  char       *path;
  char       line[BUFSIZ];
  char       name[BUFSIZ];
  int        red, green, blue, lineno = 0;
  dbEntryPtr entry;

  static Bool was_here = FALSE;

  if (!was_here)
    {
      path = (char*)ALLOCATE_LOCAL(strlen(rgbPath) +5);
      strcpy(path, rgbPath);
      strcat(path, ".txt");

      if (!(rgb = fopen(path, "r")))
        {
	   ErrorF( "Couldn't open RGB_DB '%s'\n", rgbPath );
	   DEALLOCATE_LOCAL(path);
	   return FALSE;
	}

      while(fgets(line, sizeof(line), rgb))
	{
	  lineno++;
	  if (sscanf(line,"%d %d %d %[^\n]\n", &red, &green, &blue, name) == 4)
	    {
	      if (red >= 0   && red <= 0xff &&
		  green >= 0 && green <= 0xff &&
		  blue >= 0  && blue <= 0xff)
		{
		  if (entry = lookup(name, strlen(name), TRUE))
		    {
		      entry->red   = (red * 65535)   / 255;
		      entry->green = (green * 65535) / 255;
		      entry->blue  = (blue  * 65535) / 255;
		    }
		}
	      else
		ErrorF("Value for \"%s\" out of range: %s:%d\n",
		       name, path, lineno);
	    }
	  else if (*line && *line != '#')
	    ErrorF("Syntax Error: %s:%d\n", path, lineno);
	}
      
      fclose(rgb);
      DEALLOCATE_LOCAL(path);

      was_here = TRUE;
    }

  return TRUE;
}



Bool
OsLookupColor(screen, name, len, pred, pgreen, pblue)
    int		   screen;
    char	   *name;
    unsigned	   len;
    unsigned short *pred, *pgreen, *pblue;

{
  dbEntryPtr entry;

  if (entry = lookup(name, len, FALSE))
    {
      *pred   = entry->red;
      *pgreen = entry->green;
      *pblue  = entry->blue;
      return TRUE;
    }

  return FALSE;
}

#endif /* USE_RGB_TXT */
