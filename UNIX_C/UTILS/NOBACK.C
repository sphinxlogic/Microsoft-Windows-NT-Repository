Article 304 of comp.sources.misc:
Path: brl-smoke!brl-adm!cmcl2!husc6!necntc!ncoast!allbery
From: paul@vixie.UUCP (Paul Vixie Esq)
Newsgroups: comp.sources.misc
Subject: v02i016: filter backspaces into multiple lines
Date: 26 Jan 88 03:50:50 GMT
Approved: allbery@ncoast.UUCP
X-Archive: comp.sources.misc/8801/16
Comp.sources.misc: Volume 2, Issue 16
Submitted-By: Paul Vixie Esq <paul@vixie.UUCP>
Archive-Name: noback

Comp.sources.misc: Volume 2, Issue 16
Submitted-By: Paul Vixie Esq <paul@vixie.UUCP>
Archive-Name: overstrike

/* noback - convert lines with backspaces to multiple lines
 * vix 16jun87 [made it leave lines containing ESC alone (for 'col -f')]
 * vix 16dec86 [add test for CR as end-of-line so that files already
 *              in overstrike format won't blow up the buffer...]
 * vix 10oct86 [written......again......sigh]
 */

#include <stdio.h>

#define	MDEPTH		10
#define	MWIDTH		256
#define	MYBUFSIZ	2000
#define	EOL		0x0A
#define	CR		0x0D
#define	TAB		0x09
#define	BS		0x08
#define	SPACE		0x20
#define	ESC		0x1B

main()
{
	int	depth[MWIDTH],		/* depth of stack for each position */
		length[MDEPTH],		/* number of positions used / level */
		data[MDEPTH][MWIDTH],	/* actual data, all positions/levels */
		buffer[MYBUFSIZ+1],	/* pre-scan buffer */
		bufptr,			/* buffer-pointer, what else? */
		has_esc,		/* flag: line has escape in it */
		i,			/* the inevitable generic integer */
		ch,			/* current character being processed */
		pos,			/* current position (0..MWIDTH-1) */
		curdepth,		/* depth of current position */
		maxdepth,		/* depth of deepest position's stack */
		end_int;		/* all useful variables end with ',' */

	/* I feel gross today. how about a goto?
	 */
next_line:
	/* load a line into the pre-scan buffer
	 * (I call it that because it didn't used to be here)
	 */
	has_esc = 0;
	i = 0;
	do {
		ch = getchar();
		if (ch == ESC)
			has_esc++;
		if (i == MYBUFSIZ) {
			fprintf(stderr, "noback: pre-scan buffer overflow\n");
			exit(2);
		}
		buffer[i++] = ch;
	} while (ch != EOL && ch != EOF);
	buffer[i] = 0;

	/* if the line has an escape in it, write it straight out and go back
	 * for another line.  escapes mean that some variable number of chars
	 * following will not occupy a print position, and since we can't know
	 * how many chars that will be, we don't handle the situation.
	 */
	if (has_esc) {
		i = 0;
		do { putchar(buffer[i++]); }
		while (buffer[i]);
		goto next_line;
	}

	bufptr = 0;
	ch = buffer[bufptr++];

	/* we are beginning a line
	 */
	for (i = 0;  i < MWIDTH;  i++)
		depth[i] = -1;
	for (i = 0;  i < MDEPTH;  i++)
		length[i] = 0;
	maxdepth = -1;
	pos = 0;

	while (ch != EOF && ch != EOL)
	{
		/* on a backspace, we send the position back one,
		 * then we go get the next character.
		 *
		 * NOTE: backspace as first character on the line
		 * is ignored.
		 */
		if (ch == BS)
		{
			if (pos > 0)
				pos -= 1;
			ch = buffer[bufptr++];
			continue;
		}

		/* on a carriage-return, we set the position back
		 * to the beginning of the line, then go back for
		 * the next character.
		 */
		if (ch == CR)
		{
			pos = 0;
			ch = buffer[bufptr++];
			continue;
		}

		/* on a tab, we skip to the tab position and continue.
		 */
		if (ch == TAB)
		{
			/* increment first or it won't move from a
			 * tab position.
			 */
			do  {pos += 1;}  while ((pos % 8) != 0);
			ch = buffer[bufptr++];
			continue;
		}

		/* a normal character.
		 *	-> push onto stack for this position
		 *	-> (conditionally initialize new line)
		 *	-> adjust max-length for this line
		 *	-> get next character
		 *	-> continue
		 */
		curdepth = ++depth[pos];
		if (curdepth > maxdepth)
		{
			maxdepth = curdepth;
			for (i = 0;  i < MWIDTH;  i++)
				data[curdepth][i] = SPACE;
		}
		data[curdepth][pos] = ch;
		pos += 1;
		if (pos > length[curdepth])
			length[curdepth] = pos;
		ch = buffer[bufptr++];
	}

	/* end of line or file. either way, dump the lines out.
	 * draw from the bottom up, so that the overstriking
	 * characters are drawn first.  this is because on CRTs,
	 * spaces (used in lines after the first for positioning)
	 * are destructive.
	 */
	for (i = maxdepth;  i >= 0;  i--)
	{
		for (pos = 0;  pos < length[i];  pos++)
			putchar(data[i][pos]);
		putchar(CR);
	}
	putchar(EOL);

	if (ch != EOF)
		goto next_line;

	/* end of file. bye kids...
	 */
}
-- 
Paul A Vixie Esq
paul%vixie@uunet.uu.net
{uunet,ptsfa,hoptoad}!vixie!paul
San Francisco, (415) 647-7023


