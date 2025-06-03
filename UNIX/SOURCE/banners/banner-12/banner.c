#include <stdio.h>
#include "flags.h"

banflags banFlagDefault = {
	0,	/* not italic */
	0,	/* single width */
	0,	/* no leading half space */
	1,	/* single height */
	'*'	/* use asterisk */
	};

extern unsigned char charset[];	/* Zeichensatz */

#define ROWSPERCH (8)
#define COLSPERCH (8)

/*
 * b a n n e r   f u n c t i o n
 */

banner(fp, str, flagsp)
FILE		*fp;
char		*str;
banflags	*flagsp;
{
	int	linenum, chnum, j, k;
	int	ch_off;
	char	ch, *space;

	if (flagsp->dblwidth) {
		space = "  ";
	} else {
		space = " ";
	}

	for (linenum = 0; linenum < ROWSPERCH; linenum++) {
		for (j = 0; j < flagsp->height; j++) {
			if (flagsp->halfspace) {
				for (k = 0; k < COLSPERCH/2; k++)
					(void)fputs(space, fp);
			}

			if (flagsp->italic) {
				/* shift for italics */
				for (k = flagsp->height * (ROWSPERCH - linenum) - (j + 1);
					k > 0; k--)
					(void)putc(' ', fp);
			}

			for (chnum = 0; chnum < strlen(str); chnum++) {
				ch	= str[chnum];
				ch_off	= (int) ch * 8;
				outline(fp, (flagsp->bannerch == '\0') ? ch : flagsp->bannerch,
				charset[ch_off + linenum],
				(int)flagsp->dblwidth);
			}

			(void)putc('\n', fp);
		}
	}
}


/*
 * o u t l i n e
 */

static outline(fp, outchar, outbyte, dblsize)
FILE 		*fp;
char 		outchar;
unsigned char 	outbyte;
int		dblsize;
{
	int bc;
	char ch;

	for (bc = ROWSPERCH-1; bc >= 0; bc--) {
		ch = (outbyte & (0x01 << bc)) ? outchar : ' ';
		(void)putc(ch, fp);
		if (dblsize) {
			/* if double size, repeat it */
			(void)putc(ch, fp);
		}
	}
}
