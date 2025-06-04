#ifndef NO_IDENT
static char *Id = "$Id: termsize.c,v 1.5 1995/10/21 18:40:38 tom Exp $";
#endif

/*
 * Title:	termsize.c
 * Author:	T.E.Dickey
 * Created:	02 Oct 1985
 * Last update:
 *		18 Feb 1995, port to AXP (renamed 'alarm')
 *		17 Aug 1988, use SYS$COMMAND rather than SYS$INPUT.
 *		03 Oct 1985
 *
 * Function:	This module interrogates/alters the terminal screen size.  If
 *		the terminal is a DEC-crt with advanced-video, the caller may
 *		use 'termsize' to switch between 80/132 column width at the
 *		same time.  (If non-AVO, no change is made to the screen size).
 *
 * Arguments:	reset	True if caller wishes a change to be made to the screen
 *			size, based on the other arguments.
 *		width_	Address of 32-bit integer containing caller's screen
 *			width.  If a zero is passed here, it denotes that the
 *			caller does not wish to modify the current value.
 *			If a value greater than 80 is passed, the terminal must
 *			have AVO to perform the change.  Also, if the current
 *			value is greater than 80, AVO must be set to perform
 *			a change.
 *		length_	Address of 32-bit integer containing caller's screen
 *			length.  If a zero is passed here, it denotes that the
 *			caller does not wish to modify the current value.
 *
 * Returns:	TRUE iff the procedure executed normally.  If a screen-width
 *		change was requested which could not be performed, then the
 *		routine returns FALSE.  I/O errors are all fatal (we should get
 *		none).
 *
 *		The resulting screen size is always returned in the width/length
 *		arguments.
 */

#include	<starlet.h>
#include	<lib$routines.h>
#include	<dcdef.h>
#include	<ttdef.h>
#include	<tt2def.h>

#include	<descrip.h>
#include	<iodef.h>
#include	<stsdef.h>

#include	"crt.h"

/*
 * Local definitions:
 */
#define	OK(f)	$VMS_STATUS_SUCCESS(status=f)
#define	SYS(f)	if (!OK(f)) lib$stop(status)
#define	QIO(mask)	sys$qiow (\
			0,		/* Event flag number		*/\
			tty_chan,\
			mask,\
			&iosb,0,0,\
			&cbfr,		/* buffer address		*/\
			sizeof(cbfr),	/* buffer size			*/\
			0,		/* P3 -- speed specifier	*/\
			0,		/* P4 -- fill specifier		*/\
			0,		/* P5 -- parity flags		*/\
			0)		/* prompt-string buffer size	*/

#define	FAIL	{	sound_alarm();\
			*width_ = width;	*length_= length;\
			return (0);	}

/*
 * Local (static) data:
 */
static	$DESCRIPTOR(tty_name,"SYS$COMMAND");
static	unsigned short	tty_chan = 0;

int
termsize (int reset, int *width_, int *length_)
{
	int	status,
		DEC_avo,
		width,
		length;
	struct	{
		short	sts,
			count;
		unsigned device;
		}	iosb;
	struct	{
		unsigned
		char	class,
			type;
		short	width;
		unsigned ttdef,		/* basic characteristics	*/
			tt2def;		/* extended characteristics	*/
		}	cbfr;

	if (! tty_chan)
	{
		SYS(sys$assign(&tty_name, &tty_chan, 0,0));
	}

	SYS(QIO(IO$_SENSEMODE));
	DEC_avo	= (cbfr.tt2def & TT2$M_AVO) && (cbfr.tt2def & TT2$M_DECCRT);
	width	= cbfr.width;
	length	= cbfr.ttdef >> TT$V_PAGE;

	if (reset && (*width_ || *length_))
	{
		if (*width_  <= 0)	*width_ = width;
		if (*length_ <= 0)	*length_= length;

		if (*width_ > 80 && width <= 80)
		{
			if (DEC_avo)	putraw ("\033[?3h");	/* 132-col */
			else		FAIL;
		}
		else if (width > 80 && *width_ <= 80)
		{
			if (DEC_avo)	putraw ("\033[?3l");	/* 80-col */
			else		FAIL;
		}

		cbfr.width = *width_;
		cbfr.ttdef &= ~TT$M_PAGE;
		cbfr.ttdef |= (*length_ << TT$V_PAGE);
		SYS(QIO(IO$_SETMODE));
	}
	else
	{
		*width_ = width;
		*length_= length;
	}
	return (1);
}
