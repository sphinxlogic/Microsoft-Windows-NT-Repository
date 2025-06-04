#ifndef NO_IDENT
static char *Id = "$Id: syfom.c,v 1.5 1995/06/06 13:46:52 tom Exp $";
#endif

/*
 * Title:	sysfom.c - Display figures-of-merit
 * Author:	Thomas E. Dickey
 * Created:	27 Oct 1983
 * Last update:
 *		19 Feb 1995, prototypes
 *		27 Oct 1983
 *
 * Function:	Obtain a character string from the operating system which
 *		shows both elapsed and actual CPU times.  Return this string
 *		to the user for inclusion in the process-listing.
 *
 * Parameters:	co_	=> caller's output buffer.  If null pointer, initialize
 *			   timer-control-block.
 */

#include	<lib$routines.h>
#include	<string.h>

#include "sysutils.h"

static	char	bfr[256];
static	char	*handle_ = 0;
static	int	code	= 0;

#define SYSFOM_DATA struct _sysfom_data
SYSFOM_DATA {
	short len;
	short typ;
	char  *c_;
	};

static void
sysfom_x (SYSFOM_DATA *dx_)
{
	int	len	= dx_->len;
	char	*c_	= dx_->c_;

	strncpy (bfr, dx_->c_, len);
	bfr[len] = '\0';
}

/*
 * Main procedure:
 */
void
sysfom (char *co_)
{
	if ((co_ == 0) || (handle_ == 0))
		lib$init_timer(&handle_);
	if (co_)
	{
		lib$show_timer (&handle_, &code, sysfom_x);
		strcpy (co_, &bfr[1]);
	}
}
