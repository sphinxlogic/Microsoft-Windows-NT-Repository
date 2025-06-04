#ifndef NO_IDENT
static char *Id = "$Id: snapshot.c,v 1.9 1995/10/28 14:13:13 tom Exp $";
#endif

/*
 * Title:	snapshot.c
 * Author:	Thomas E. Dickey
 * Created:	15 Oct 1984
 * Last update:
 *		28 Oct 1995, rewrote using rmsio
 *		19 Feb 1995, prototypes
 *		18 Feb 1995, port to AXP (renamed 'alarm'). 
 *		27 Jan 1985, save old-sgr on status line to restore it.
 *		17 Oct 1984
 *
 * Function:	This procedure copies the current screen-contents from the
 *		CRT module to the output file SNAPSHOT.CRT, doing minor
 *		formatting to make it readable on a printer.
 */

#include	<starlet.h>
#include	<stdio.h>
#include	<signal.h>	/* for 'sleep()' */
#include	<string.h>
#include	<ctype.h>

#include	"crt.h"
#include	"rmsio.h"
#include	"strutils.h"
#include	"sysutils.h"

static	RFILE	*fp	= 0;
static	int	calls	= 0;

#define PUTLINE rputr(fp, dline, strlen(dline)); dline[0] = '\0'
#define	CORNER	strcat(dline,"+")
#define	BAR	CORNER; \
		for(j=0; j < width; j++) \
			strcat(dline,"-"); \
		CORNER; \
		PUTLINE

#define	VERT	strcat(dline, "|")
#define	VEC(j)	((j == lpp1) ? sline : crtvec[j])

void
snapshot (void)
{
	int	top	= crt_top (),
		end	= crt_end (),
		lpp	= crt_lpp (),
		width	= crt_width () - 1,
		save_x	= crt_x (),
		save_y	= crt_y (),
		oldsgr	= crt_qsgr(lpp-1);
	DATENT	date;
	register
	int	lpp1	= lpp - 1,
		j,	k,	over;
	char	bfr	[CRT_COLS],
		dline	[CRT_COLS*3],	/* full-line + underlining */
		sline	[CRT_COLS];	/* saved original message-line */
	register
	char	*s_,	c;

	if (!fp)
	{
		fp = ropen ("sys$login:snapshot.crt", "w");
		if (!fp)
		{
			sound_alarm ();
			return;
		}
		calls = 0;
	}

	strcpy (sline, crtvec[lpp1]);
	sprintf (bfr, "%03d: Copying screen to SNAPSHOT.CRT ... ", ++calls);
	crt_high (bfr, strlen(bfr));
	crt_text (bfr, lpp1, 2);	/* Show this in REVERSE		*/

	sys$gettim (&date);
	sysasctim (bfr, &date, 21);

	strcpy(dline, "\f");
	PUTLINE;
	PUTLINE;

	sprintf(dline, "Screen dumped: %s", bfr);
	PUTLINE;

	sprintf (dline, "Image top: %d  bottom: %d", top, end);
	PUTLINE;
	PUTLINE;

	BAR;
	for (j = 0; j < lpp; j++)
	{
		VERT;
		for (s_ = VEC(j), k = over = 0;
			k < width;
				k++)
		{
			if (*s_)
			{
				c = *s_++;
				if (!isascii(c))
				{
					c = toascii(c);
					over = k + 1;
				}
				sprintf (strnull(dline), "%c", c);
			}
			else
				strcat (dline, " ");
		}
		VERT;
		if (over)
		{
			strcat(dline,"\r ");
			for (s_ = VEC(j), k = 0; k < over; k++)
			{
				c = *s_++;
				sprintf (strnull(dline), "%c",
					isascii(c) ? ' ' : '_');
			}
		}
		PUTLINE;
	}
	BAR;

	sleep(1);			/* Make sure user sees message	*/
	crt_text ("",    lpp1, 0);	/* flush differences in highlit	*/
	crt_text (sline, lpp1, oldsgr);	/* Use original highlighting	*/
	crt_move (save_y, save_x);
}
