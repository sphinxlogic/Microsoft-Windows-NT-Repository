#ifndef NO_IDENT
static char *Id = "$Id: flcrea.c,v 1.2 1995/10/27 23:09:33 tom Exp $";
#endif

/*
 * Title:	flcrea.c
 * Author:	T.E.Dickey
 * Created:	27 Oct 1995
 * Last update:
 *
 * Function:	This module supports an create-function for "FLIST".
 *		No options, no wildcards.
 */

#include	<starlet.h>
#include	<stsdef.h>
#include	<rms.h>

#include	<stdlib.h>
#include	<stdio.h>
#include	<errno.h>
#include	<string.h>	/* 'strerror()', etc. */
#include	<unixio.h>	/* 'creat()' */
#include	<unixlib.h>	/* 'mkdir()' */

#include	"bool.h"
#include	"dclarg.h"
#include	"flist.h"	/* 'flist_sysmsg()' */

#include	"dds.h"
#include	"dircmd.h"
#include	"dirent.h"
#include	"pathup.h"
#include	"rmsinit.h"

#include	"strutils.h"

static	int	looks_like_directory (char *filespec);

tDIRCMD(flcrea)
{
	unsigned status;
	FILENT	znew, *z = &znew;
	char	leaf_0[MAX_PATH];
	char	full_1[MAX_PATH];
	char	full_2[MAX_PATH];
	char	*spec	= dclinx (xdcl_, 1, 0);
	int	fd;
	int	code;
	char	*s;

	/*
	 * If the new item looks like a directory, create a directory.
	 */
	switch ((code = looks_like_directory(spec)))
	{
	case 3:	/* Make both directory and filename */
		for (s = spec + strlen(spec) - 1; (s - spec) >= 0; s--)
		{
			if (*s == '>' || *s == ']')
			{
				s++;
				break;
			}
		}
		strcpy(leaf_0, s);
		*s = EOS;
		/* FALLTHRU */
		goto bracketed_name;

	case 2:	/* Translate name to bracketed form */
		status = dirent_chop (z, spec, (struct NAM *)0);
		if (! $VMS_STATUS_SUCCESS(status))
		{
			flist_sysmsg (status);
			return;
		}
		(void)pathdown (spec = full_1, zPATHOF(z), z->fname);
		/*FALLTHRU*/

	bracketed_name:
	case 1: /* Make a directory */
		if (mkdir(spec, 0777) < 0)
		{
			warn("%s: %s", spec, strerror(errno));
			return;
		}
		/* Translate the name into a ".dir" form, since that's how
		 * it'll get put into the file list
		 */
		if (!pathup(full_2, spec))
		{
			warn("%s: unexpected error", spec);
			return;
		}
		if (code != 3)
		{
			spec = full_2;
			break;
		}

		if (dirent_chk (z, full_2))
			dds_add (z);
		strcat(spec, leaf_0);
		/* FALLTHRU */

		/*
		 * Otherwise, it's a file.
		 */
	default:
		if ((fd = creat(spec, 0777)) < 0)
		{
			warn("%s: %s", spec, strerror(errno));
			return;
		}
		close(fd);
		break;
	}

	/*
	 * Update the display, putting the new entry at the first free-slot.
	 *
	 * FIXME: should we make this work with V_opt?
	 * FIXME: if we create a multilevel directory, we don't add all levels
	 */
	if (dirent_chk (z, spec))
		dds_add (z);
}

/*
 * Returns nonzero if the file specification ends with a bracket, or the suffix
 * ".DIR"
 */
static
int	looks_like_directory (char *filespec)
{
	char	temp[MAX_PATH];
	size_t	len;
	unsigned status;
	struct	FAB	fab;
	struct	NAM	nam;
	char	rsa	[NAM$C_MAXRSS],	/* resultant string (SYS$SEARCH)*/
		esa	[NAM$C_MAXRSS];	/* expanded string (SYS$PARSE)	*/

	(void) strucpy(temp, filespec);
	if ((len = strlen(temp)) > 1)
	{
		char	*vers = strrchr(temp, ';');
		if (vers != 0
		 && (!strcmp(vers, ";") || !strcmp(vers, ";1")))
		{
		 	*vers = EOS;
			return looks_like_directory(temp);
		}
		if (temp[len-1] == ']'
		 || temp[len-1] == '>')
		 	return 1;
		if (len > 4)
		{
			if (!strcmp(temp+len-4, ".DIR"))
				return 2;
		}
	}

	/*
	 * It looks like a filename. Double-check to ensure that the directory
	 * exists.
	 */
	rmsinit_fab (&fab, &nam, "", 0);
	fab.fab$l_fna	= temp;
	fab.fab$b_fns	= strlen(temp);
	rmsinit_nam (&nam, rsa, esa);
	if ((status = sys$parse(&fab)) == RMS$_DNF) 
		return 3;
	return 0;
}
