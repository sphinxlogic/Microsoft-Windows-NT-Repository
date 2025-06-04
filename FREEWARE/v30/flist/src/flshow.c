/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: flshow.c,v 1.11 1995/10/25 15:51:02 tom Exp $";
#endif

/*
 * Title:	flshow.c ("show" commands for FLIST)
 * Author:	T.E.Dickey
 * Created:	04 May 1985 (from main program)
 * Last update:
 *		25 Oct 1995, long/unsigned/DATENT fixes
 *		18 Mar 1995, prototypes
 *		04 Nov 1988, added expired date
 *		12 Nov 1985, added "?OWNER"
 *		05 Oct 1985, added key-argument to 'flist_help'.  Use it here.
 *		24 Sep 1985, don't need to uppercase 'xdcl_' stuff.
 *		13 Aug 1985, broke out "?FIND" to 'flfind_show'
 *		25 Jul 1985, added "?READ" command
 *		04 Jul 1985, cleanup 'filelist' definition.
 *		10 Jun 1985, added "?FIND" command
 *		14 May 1985, forgot to remove trace
 *		04 May 1985
 */

#include	<ctype.h>
#include	<string.h>
#include	<unixlib.h>	/* for 'getuid()' */

#include	<starlet.h>
#include	<lib$routines.h>
#include	<descrip.h>
#include	<stsdef.h>

#include	"flist.h"

#include	"dirent.h"
#include	"dircmd.h"
#include	"dds.h"

#include	"sysutils.h"

extern	int	more(int argc, char **argv);
extern	char	*dired_release(void);
extern	void	shoquota(char *);

import(filelist);	import(numfiles);

import(AnyXAB);		import(A_opt);		import(D_opt);
import(M_opt);		import(P_opt);		import(V_opt);
import(dateflag);	import(datechek);
import(readlist);	import(readllen);

static	void	flshow_lis (int curfile);

tDIRCMD(flshow)
{
#define	len_date	21
	DATENT	date;
	int	j;
	unsigned total, total2;	/* misc variables used in commands */
	char	bfr[CRT_COLS+MAX_PATH];

	switch (xdcl_->dcl_text[0])
	{
	case 'C':		/* Display screen format	*/
		flcols_show ();
		break;

	case 'D':		/* Display current date, time	*/
		sys$gettim (&date);
		sysasctim (bfr, &date, len_date);
		flist_tell ("Current date: %s", bfr);
		break;

	case 'F':
		flfind_show ();
		break;

	case 'L':		/* Display current level number	*/
		flist_tell ("Current level: %d", flist_nest());
		break;

	case 'M':		/* Display dynamic memory usage	*/
		lib$show_vm (0, &flist_tell_dsc);
		break;

	case 'O':		/* Display owner/uid of current process */
		flist_tell ("Your uid is [%03o,%03o]", getgid(), getuid());
		break;

	case 'Q':		/* Display current disk quota, etc.*/
		shoquota (bfr);
		flist_tell ("%s", bfr);
		break;

	case 'R':		/* Display read-list and pertinent options */
		flist_lis ("Options in effect:");
		if (dateflag[0])
		{
			sysasctim (bfr, &datechek, 18);
			flist_lis ("    /%s=%s",
				dateflag[0] > 0 ? "AFTER" : "BEFORE", bfr);
			switch (dateflag[1])
			{
			case 4:	flist_lis ("    /EXPIRED");	break;
			case 3:	flist_lis ("    /MODIFIED");	break;
			case 2:	flist_lis ("    /BACKUP");	break;
			default:flist_lis ("    /CREATED");
			}
		}
		if (P_opt)	flist_lis ("    /ALL");
		if (D_opt)	flist_lis ("    /DATE");
		if (!AnyXAB)	flist_lis ("    /FAST");
		if (M_opt)	flist_lis ("    /PROTECTION");
		if (A_opt)	flist_lis ("    /SIZE");
		flist_lis ("    /%sVERSIONS", V_opt ? "" : "NO");
		flist_lis ("\nRead-List:");
		for (j = 0; j < readllen; j++)
		{
			dirent_glue (bfr, &readlist[j]);
			flist_lis ("    %s", bfr);
		}
		flshow_lis (*curfile_);
		break;

	case 'S':		/* Display total number of blocks */
		for (j = 0, total = total2 = 0; j < numfiles; j++)
			if (!DELETED(j))
			{
				total += FK(j).fsize;
				total2 += FK(j).fallc;
			}
		flist_tell ("Total blocks: %d (%d allocated)",
			total, total2);
		break;

	case 'V':		/* Display current version	*/
		flist_tell ("%s", dired_release());
		break;

	case 'T':		/* Display cpu time, etc.	*/
		sysfom (bfr);
		flist_tell ("%s", bfr);
		if (!strchr(xcmd_, '?'))	/* "/TIMER"	*/
			sysfom (nullC);		/* Reset timer	*/
		break;
	default:
		for (*bfr = EOS, xdcl_ = xdcl_->dcl_next;
			xdcl_;
				xdcl_ = xdcl_->dcl_next)
		{
			strcat (bfr, xdcl_->dcl_text);
			strcat (bfr, " ");
		}
		flist_help (*curfile_, bfr);
	}
}

/* <flshow_lis>:
 * Close and browse the listing file.
 */
static
void	flshow_lis (int curfile)
{
	char	*argv[3];

	argv[0]	=
	argv[1]	= "BROWSE";
	argv[2]	= flist_lis(0);

	more (3, argv);
	if (!didwarn())	dds_all (-1, curfile);
}
