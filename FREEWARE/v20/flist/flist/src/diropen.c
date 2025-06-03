/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: diropen.c,v 1.5 1995/10/21 17:48:35 tom Exp $";
#endif

/*
 * Title:	diropen.c
 * Author:	Thomas E. Dickey
 * Created:	04 Jul 1984
 * Last update:	25 Aug 1984, cleanup buffer sizes
 *		23 Jul 1984, enhanced warning messages
 *		15 Jul 1984, to provide 'diropen2' entrypoint.
 *
 * Function:	This procedure is used by FLIST to check that a directory
 *		is both readable and writeable before attempting an operation
 *		(such as EDIT or COPY) which would modify the directory.
 *
 * Parameters:	name_	=> filespec containing, at least, the pathname of
 *			   the directory to check.  For example, if the
 *			   new file will be
 *
 *				DBC4:[DICKEY.C]DIROPEN.C
 *
 *			   then the 'name_' string will include
 *
 *				DBC4:[DICKEY.C]
 *
 *			   DIROPEN then translates this pathname into the
 *			   name of the directory file in the parent directory
 *
 *				DBC4:[DICKEY]C.DIR
 *
 *			   and tests that it is writeable.
 *
 * Returns:	TRUE iff the directory can be modified.  If not, a warning
 *		message is emitted via 'warn'.
 */

#include	<rms.h>

#include	"bool.h"
#include	"flist.h"	/* for 'error()' */
#include	"dirent.h"
#include	"getprot.h"
#include	"pathup.h"

int	diropen (char *name_)
{
	GETPROT	prot;
	char	parent	[MAX_PATH],
		*msg	= 0;

	if (! pathup (parent, name_))
		msg = "Illegal pathname";
	else if (getprot (&prot, parent))
		msg = "Cannot read directory";
	else if (! cmpprot (&prot, "rw"))
		msg = "Directory is not modifiable";

	if (msg)
	{
		warn ("%s: %s", msg, name_);
		return (FALSE);
	}
	else
		return (TRUE);
}

/*
 * Most of FLIST's calls on this module already have a specific 'filelist[]'
 * entry which is being tested.  Use this entrypoint to hide knowledge of the
 * relationship to 'pathlist'.
 */
int	diropen2 (FILENT *z)
{
	return (diropen ( zPATHOF(z) ));
}
