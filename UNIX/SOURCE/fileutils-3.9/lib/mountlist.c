/* mountlist.c -- return a list of mounted filesystems
   Copyright (C) 1991, 1992 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef HAVE_CONFIG_H
#if defined (CONFIG_BROKETS)
/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because it found this file in $srcdir).  */
#include <config.h>
#else
#include "config.h"
#endif
#endif

#include <stdio.h>
#include <sys/types.h>
#include "mountlist.h"

#ifdef STDC_HEADERS
#include <stdlib.h>
#else
void free ();
#endif
#if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
#include <string.h>
#else
#include <strings.h>
#endif

char *strstr ();
char *xmalloc ();
char *xrealloc ();
char *xstrdup ();
void error ();

#if defined (MOUNTED_GETFSSTAT)	/* __alpha running OSF_1 */
#  include <sys/mount.h>
#  include <sys/fs_types.h>
#endif /* MOUNTED_GETFSSTAT */

#ifdef MOUNTED_GETMNTENT1	/* 4.3BSD, SunOS, HP-UX, Dynix, Irix.  */
#include <mntent.h>
#if !defined(MOUNTED)
#  if defined(MNT_MNTTAB)	/* HP-UX.  */
#    define MOUNTED MNT_MNTTAB
#  endif
#  if defined(MNTTABNAME)	/* Dynix.  */
#    define MOUNTED MNTTABNAME
#  endif
#endif
#endif

#ifdef MOUNTED_GETMNTINFO	/* 4.4BSD.  */
#include <sys/mount.h>
#endif

#ifdef MOUNTED_GETMNT		/* Ultrix.  */
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/fs_types.h>
#endif

#ifdef MOUNTED_FREAD		/* SVR2.  */
#include <mnttab.h>
#endif

#ifdef MOUNTED_FREAD_FSTYP	/* SVR3.  */
#include <mnttab.h>
#include <sys/fstyp.h>
#include <sys/statfs.h>
#endif

#ifdef MOUNTED_GETMNTENT2	/* SVR4.  */
#include <sys/mnttab.h>
#endif

#ifdef MOUNTED_VMOUNT		/* AIX.  */
#include <fshelp.h>
#include <sys/vfs.h>
#endif

#ifdef DOLPHIN
/* So special that it's not worth putting this in autoconf.  */
#undef MOUNTED_FREAD_FSTYP
#define MOUNTED_GETMNTTBL
#endif

#ifdef MOUNTED_GETMNTENT1	/* 4.3BSD, SunOS, HP-UX, Dynix, Irix.  */
/* Return the value of the hexadecimal number represented by CP.
   No prefix (like '0x') or suffix (like 'h') is expected to be
   part of CP. */

static int
xatoi (cp)
     char *cp;
{
  int val;

  val = 0;
  while (*cp)
    {
      if (*cp >= 'a' && *cp <= 'f')
	val = val * 16 + *cp - 'a' + 10;
      else if (*cp >= 'A' && *cp <= 'F')
	val = val * 16 + *cp - 'A' + 10;
      else if (*cp >= '0' && *cp <= '9')
	val = val * 16 + *cp - '0';
      else
	break;
      cp++;
    }
  return val;
}
#endif /* MOUNTED_GETMNTENT1.  */

#ifdef MOUNTED_GETMNTINFO	/* 4.4BSD.  */
static char *
fstype_to_string (t)
     short t;
{
  switch (t)
    {
    case MOUNT_UFS:
      return "ufs";
    case MOUNT_NFS:
      return "nfs";
#ifdef MOUNT_PC
    case MOUNT_PC:
      return "pc";
#endif
#ifdef MOUNT_MFS
    case MOUNT_MFS:
      return "mfs";
#endif
#ifdef MOUNT_LO
    case MOUNT_LO:
      return "lo";
#endif
#ifdef MOUNT_TFS
    case MOUNT_TFS:
      return "tfs";
#endif
#ifdef MOUNT_TMP
    case MOUNT_TMP:
      return "tmp";
#endif
    default:
      return "?";
    }
}
#endif /* MOUNTED_GETMNTINFO */

#ifdef MOUNTED_VMOUNT		/* AIX.  */
static char *
fstype_to_string (t)
     int t;
{
  struct vfs_ent *e;

  e = getvfsbytype (t);
  if (!e || !e->vfsent_name)
    return "none";
  else
    return e->vfsent_name;
}
#endif /* MOUNTED_VMOUNT */

/* Return a list of the currently mounted filesystems, or NULL on error.
   Add each entry to the tail of the list so that they stay in order.
   If NEED_FS_TYPE is nonzero, ensure that the filesystem type fields in
   the returned list are valid.  Otherwise, they might not be.
   If ALL_FS is zero, do not return entries for filesystems that
   are automounter (dummy) entries.  */

struct mount_entry *
read_filesystem_list (need_fs_type, all_fs)
     int need_fs_type, all_fs;
{
  struct mount_entry *mount_list;
  struct mount_entry *me;
  struct mount_entry *mtail;

  /* Start the list off with a dummy entry. */
  me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
  me->me_next = NULL;
  mount_list = mtail = me;

#ifdef MOUNTED_GETMNTENT1	/* 4.3BSD, SunOS, HP-UX, Dynix, Irix.  */
  {
    struct mntent *mnt;
    char *table = MOUNTED;
    FILE *fp;
    char *devopt;

    fp = setmntent (table, "r");
    if (fp == NULL)
      return NULL;

    while ((mnt = getmntent (fp)))
      {
	if (!all_fs && (!strcmp (mnt->mnt_type, "ignore")
			|| !strcmp (mnt->mnt_type, "auto")))
	  continue;

	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	me->me_devname = xstrdup (mnt->mnt_fsname);
	me->me_mountdir = xstrdup (mnt->mnt_dir);
	me->me_type = xstrdup (mnt->mnt_type);
	devopt = strstr (mnt->mnt_opts, "dev=");
	if (devopt)
	  {
	    if (devopt[4] == '0' && (devopt[5] == 'x' || devopt[5] == 'X'))
	      me->me_dev = xatoi (devopt + 6);
	    else
	      me->me_dev = xatoi (devopt + 4);
	  }
	else
	  me->me_dev = -1;	/* Magic; means not known yet. */
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }

    if (endmntent (fp) == 0)
      return NULL;
  }
#endif /* MOUNTED_GETMNTENT1. */

#ifdef MOUNTED_GETMNTINFO	/* 4.4BSD.  */
  {
    struct statfs *fsp;
    int entries;

    entries = getmntinfo (&fsp, MNT_NOWAIT);
    if (entries < 0)
      return NULL;
    while (entries-- > 0)
      {
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	me->me_devname = xstrdup (fsp->f_mntfromname);
	me->me_mountdir = xstrdup (fsp->f_mntonname);
	me->me_type = fstype_to_string (fsp->f_type);
	me->me_dev = -1;	/* Magic; means not known yet. */
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
	fsp++;
      }
  }
#endif /* MOUNTED_GETMNTINFO */

#ifdef MOUNTED_GETMNT		/* Ultrix.  */
  {
    int offset = 0;
    int val;
    struct fs_data fsd;

    while ((val = getmnt (&offset, &fsd, sizeof (fsd), NOSTAT_MANY,
			  (char *) 0)) > 0)
      {
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	me->me_devname = xstrdup (fsd.fd_req.devname);
	me->me_mountdir = xstrdup (fsd.fd_req.path);
	me->me_type = gt_names[fsd.fd_req.fstype];
	me->me_dev = fsd.fd_req.dev;
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }
    if (val < 0)
      return NULL;
  }
#endif /* MOUNTED_GETMNT. */

#if defined (MOUNTED_GETFSSTAT)	/* __alpha running OSF_1 */
  {
    int numsys, counter, bufsize;
    struct statfs *stats;

    numsys = getfsstat ((struct statfs *)0, 0L, MNT_WAIT);
    if (numsys < 0)
      return (NULL);

    bufsize = (1 + numsys) * sizeof (struct statfs);
    stats = (struct statfs *)xmalloc (bufsize);
    numsys = getfsstat (stats, bufsize, MNT_WAIT);

    if (numsys < 0)
      {
	free (stats);
	return (NULL);
      }

    for (counter = 0; counter < numsys; counter++)
      {
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	me->me_devname = xstrdup (stats[counter].f_mntfromname);
	me->me_mountdir = xstrdup (stats[counter].f_mntonname);
	me->me_type = mnt_names[stats[counter].f_type];
	me->me_dev = -1;	/* Magic; means not known yet. */
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }

    free (stats);
  }
#endif /* MOUNTED_GETFSSTAT */

#if defined (MOUNTED_FREAD) || defined (MOUNTED_FREAD_FSTYP) /* SVR[23].  */
  {
    struct mnttab mnt;
    char *table = "/etc/mnttab";
    FILE *fp;

    fp = fopen (table, "r");
    if (fp == NULL)
      return NULL;

    while (fread (&mnt, sizeof mnt, 1, fp) > 0)
      {
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
#ifdef GETFSTYP			/* SVR3.  */
	me->me_devname = xstrdup (mnt.mt_dev);
#else
	me->me_devname = xmalloc (strlen (mnt.mt_dev) + 6);
	strcpy (me->me_devname, "/dev/");
	strcpy (me->me_devname + 5, mnt.mt_dev);
#endif
	me->me_mountdir = xstrdup (mnt.mt_filsys);
	me->me_dev = -1;	/* Magic; means not known yet. */
	me->me_type = "";
#ifdef GETFSTYP			/* SVR3.  */
	if (need_fs_type)
	  {
	    struct statfs fsd;
	    char typebuf[FSTYPSZ];

	    if (statfs (me->me_mountdir, &fsd, sizeof fsd, 0) != -1
		&& sysfs (GETFSTYP, fsd.f_fstyp, typebuf) != -1)
	      me->me_type = xstrdup (typebuf);
	  }
#endif
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }

    if (fclose (fp) == EOF)
      return NULL;
  }
#endif /* MOUNTED_FREAD || MOUNTED_FREAD_FSTYP.  */

#ifdef MOUNTED_GETMNTTBL	/* DolphinOS goes it's own way */
  {
    struct mntent **mnttbl=getmnttbl(),**ent;
    for (ent=mnttbl;*ent;ent++)
      {
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	me->me_devname = xstrdup ( (*ent)->mt_resource);
	me->me_mountdir = xstrdup( (*ent)->mt_directory);
	me->me_type =  xstrdup ((*ent)->mt_fstype);
	me->me_dev = -1;	/* Magic; means not known yet. */
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }
    endmnttbl();
  }
#endif

#ifdef MOUNTED_GETMNTENT2	/* SVR4.  */
  {
    struct mnttab mnt;
    char *table = MNTTAB;
    FILE *fp;
    int ret;

    fp = fopen (table, "r");
    if (fp == NULL)
      return NULL;

    while ((ret = getmntent (fp, &mnt)) == 0)
      {
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	me->me_devname = xstrdup (mnt.mnt_special);
	me->me_mountdir = xstrdup (mnt.mnt_mountp);
	me->me_type = xstrdup (mnt.mnt_fstype);
	me->me_dev = -1;	/* Magic; means not known yet. */
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }

    if (ret > 0)
      return NULL;
   if (fclose (fp) == EOF)
      return NULL;
  }
#endif /* MOUNTED_GETMNTENT2.  */

#ifdef MOUNTED_VMOUNT		/* AIX.  */
  {
    int bufsize;
    char *entries, *thisent;
    struct vmount *vmp;

    /* Ask how many bytes to allocate for the mounted filesystem info.  */
    mntctl (MCTL_QUERY, sizeof bufsize, (struct vmount *) &bufsize);
    entries = xmalloc (bufsize);

    /* Get the list of mounted filesystems.  */
    mntctl (MCTL_QUERY, bufsize, (struct vmount *) entries);

    for (thisent = entries; thisent < entries + bufsize;
	 thisent += vmp->vmt_length)
      {
	vmp = (struct vmount *) thisent;
	me = (struct mount_entry *) xmalloc (sizeof (struct mount_entry));
	if (vmp->vmt_flags & MNT_REMOTE)
	  {
	    char *host, *path;

	    /* Prepend the remote pathname.  */
	    host = thisent + vmp->vmt_data[VMT_HOSTNAME].vmt_off;
	    path = thisent + vmp->vmt_data[VMT_OBJECT].vmt_off;
	    me->me_devname = xmalloc (strlen (host) + strlen (path) + 2);
	    strcpy (me->me_devname, host);
	    strcat (me->me_devname, ":");
	    strcat (me->me_devname, path);
	  }
	else
	  {
	    me->me_devname = xstrdup (thisent +
				      vmp->vmt_data[VMT_OBJECT].vmt_off);
	  }
	me->me_mountdir = xstrdup (thisent + vmp->vmt_data[VMT_STUB].vmt_off);
	me->me_type = xstrdup (fstype_to_string (vmp->vmt_gfstype));
	me->me_dev = -1;	/* vmt_fsid might be the info we want.  */
	me->me_next = NULL;

	/* Add to the linked list. */
	mtail->me_next = me;
	mtail = me;
      }
    free (entries);
  }
#endif /* MOUNTED_VMOUNT. */

  /* Free the dummy head. */
  me = mount_list;
  mount_list = mount_list->me_next;
  free (me);
  return mount_list;
}
