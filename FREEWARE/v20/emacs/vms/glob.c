/*
 * Copyright © 1994 the Free Software Foundation, Inc.
 *
 * This file is a part of GNU VMSLIB, the GNU library for porting GNU
 * software to VMS.
 *
 * GNU VMSLIB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU VMSLIB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU VMSLIB; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <descrip.h>
#include <rmsdef.h>
#include <nam.h>
#include <errno.h>
#include <string.h>
#include <lib$routines.h>

extern char *strdup (char *);

extern void *xmalloc (int);
extern void *xrealloc (void *, int);

/*
 * char **glob (char *try)
 *
 * Synopsis
 *   Return a NULL-terminate array of existing files matching pattern TRY
 *   or NULL if no matches.  Treats '?' the same as '%' for unix
 *   compatibility. 
 *
 * Author:
 *   Roland B Roberts (roberts@nsrl.rochester.edu)
 *   March 1994
 */
char **glob (char *try)
{
  static $DESCRIPTOR(sdd,"SYS$DISK:[]*.*");
  static struct dsc$descriptor_s sid = {
    0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };
  static struct { short length; char string[NAM$C_MAXRSS+1]; } filename;
  static struct dsc$descriptor_s sod = {
    sizeof(filename), DSC$K_DTYPE_VT, DSC$K_CLASS_VS, (void *) &filename };
  long context = 0;
  long status, sz;
  char **list = NULL;
  int nfiles = 0;
  char *qm;
  
  /* reinitialize */
  sid.dsc$a_pointer = strdup(try);
  sid.dsc$w_length  = strlen(try);
  while (qm = strchr(sid.dsc$a_pointer,'?')) *qm = '%';
  
  list = (char **) xmalloc ((sz=10) * sizeof(char *));
  while ((status=lib$find_file(&sid,&sod,&context,&sdd,0,0,0)) == RMS$_NORMAL) {
    filename.string[filename.length] = 0;
    if (sz <= nfiles)
      list = (char **) xrealloc (list, (sz = nfiles+10) * sizeof (char *));
    list[nfiles] = (char *) xmalloc ((1+filename.length) * sizeof(char));
    strcpy (list[nfiles++], filename.string);
  }
  if (status == RMS$_NMF) { /* No more files. */
    lib$find_file_end (&context);
    context = 0;
  }
  else {			/* Something went wrong. */
    errno = EVMSERR;
    vaxc$errno = status;
  }
  if (nfiles == 0) {
    free (list);
    free (sid.dsc$a_pointer);
    return (NULL);
  }
  list = (char **) xrealloc (list, (nfiles+1) * sizeof (char *));
  list[nfiles] = NULL;
  free (sid.dsc$a_pointer);
  return (list);
}
