#module uaf_main "X-2"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the main processing routine  for the
**		DECwindows AUTHORIZE utility.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:  25-Nov-1992
**
**  MODIFICATION HISTORY:
**
**	X-2	LMP		L. Mark Pilant,		28-JAN-1993  16:04
**		Convert to C from BLISS sources.
**
**	X-1	LMP		L. Mark Pilant,		25-NOV-1992
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include	<JPIDEF>
#include	<PRVDEF>

#include	string

#include	<decw$include:mrmappl>

#include	"uaf_header"

/*
**
** FORWARD ROUTINES.
**
*/

#pragma noinline (AUTHORIZE$MAIN)

unsigned int AUTHORIZE$MAIN (argc, argv)

MAIN_PROGRAM

unsigned int		argc;
char			*argv[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Main routine for the DECwindows AUTHORIZE utility.
**
**  FORMAL PARAMETERS:
**
**	None.
**
**  RETURN VALUE:
**
**	SS$_NORMAL if successful, error code otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines */

extern unsigned int	AUTHORIZE$BUILD_MASTER_LIST ();
extern unsigned int	AUTHORIZE$BUILD_USER_LIST ();
extern void		AUTHORIZE$DECW_ERROR ();
extern unsigned int	AUTHORIZE$DECW_INIT ();
extern void		AUTHORIZE$DECW_MAIN_LOOP ();
extern void		AUTHORIZE$INIT_GLOBAL ();
extern unsigned int	AUTHORIZE$OPEN_UAF ();
extern void		AUTHORIZE$SET_DEFAULT_FILTER ();
extern void		AUTHORIZE$SET_UP_USERNAME ();

/* Global references. */

globalref Widget	uaf_r_main_top_level_widget;			/* Main window widget shell info */
globalref char		*uaf_t_default_filter;				/* Default username filter */

/* Local storage. */

struct ITMDEF		jpi_itmlst[2];					/* $GETJPI item list */
union prvdef		process_privileges;				/* Process privileges */
unsigned int		status;						/* Routine exit status */

/* Initialize local storage. */

memset (&jpi_itmlst, 0, sizeof jpi_itmlst);

jpi_itmlst[0].itm$w_bufsiz = sizeof process_privileges;
jpi_itmlst[0].itm$w_itmcod = JPI$_CURPRIV;
jpi_itmlst[0].itm$l_bufadr = &process_privileges;

/* Get the process' current privileges. */

CHECK_RETURN (SYS$GETJPI (NULL,
			  NULL,
			  NULL,
			  &jpi_itmlst,
			  NULL,
			  NULL,
			  NULL));

/* There is an anomoly in the $GETUAI and $SETUAI system services that will
** cause some problems if SYSPRV is not present.  Even though the authorization
** file has been opened outside the system services, they still perform the
** protection check.  (Even though the process could write the authorization
** record directly...sigh.)  So to avoid some weird problems, check to see
** if the process is running with SYSPRV.  If it is not, mention it. */

if (FALSE (process_privileges.prv$v_sysprv) &&
    FALSE (process_privileges.prv$v_bypass)) LIB$SIGNAL (UAF$_NOUAIPRIVS);

/* Initialize the global storage. */

AUTHORIZE$INIT_GLOBAL ();

/* Initialize the DECwindows environment. */

SIGNAL_RETURN (UAF$_INITDISPLAY, AUTHORIZE$DECW_INIT ());

/* Set up the default username filter. */

AUTHORIZE$SET_DEFAULT_FILTER (uaf_t_default_filter);

/* If the INITIAL_OPEN symbol has been defined, open up the authorization
** file using either the logical name SYSUAF or the file name SYS$DISK:
** SYSUAF.DAT. */

#ifdef	INITIAL_OPEN

/* Access the authorization file. */

SIGNAL_RETURN (UAF$_UAFOPENERR, AUTHORIZE$OPEN_UAF (&NULL));

/* Get a list of the user's and groups. */

SIGNAL_RETURN (UAF$_UAFOPENERR, AUTHORIZE$BUILD_MASTER_LIST ());
SIGNAL_RETURN (UAF$_UAFOPENERR, AUTHORIZE$BUILD_USER_LIST ());

/* Set up for the first username in the list. */

AUTHORIZE$SET_UP_USERNAME (NULL,
			   1);
#endif	/* INITIAL_OPEN */

/* Go finish up the DECwindows initialization and wait for X events. */

AUTHORIZE$DECW_MAIN_LOOP ();

/* Should never get here. */

return SS$_NORMAL;
}
