#include "patchlevel.h"
/*
 * pictoppm.c -- convert .pic files to ppm
 * (Mouse systems PC paint or grasp .gl files)
 *
 * Author:  George Phillips <phillips@cs.ubc.ca>
 *
 * (just stdin to stdout for now) 
 */
 
#include <stdio.h>
extern unsigned char* malloc();
 
int blocksleft;
int curlen = 0;
int spec;
int verbose = 0; /* 0,1 or 2 */
 
#define nl()	if (verbose > 1) fputc('\n', stderr)
 
	int bytesleft;
main()
{
	unsigned char buf[32];
	unsigned char cmap[256*3];
	int hascmap;
	int i, r, c, maxc=0;
	int width, height, bpp, w, h;
	unsigned char*	img;
	unsigned char*	p;
 
	if (!rd(buf, 17))
		ueof();
	nl();
 
	if (buf[15] != 0) {
		if (!rd(buf + 17, 2))
			ueof();
		nl();
	}
	
	if (buf[0] != 0x34 || buf[1] != 0x12) {
		fprintf(stderr, "not a .pic file\n");
		exit(1);
	}
 
	if (buf[12] != 'L' && buf[12] != 'C')
		fprintf(stderr, "I may not do the right thing...\n");
	
	hascmap = buf[13] & 4;
 
	width = buf[2] + buf[3] * 256;
	height = buf[4] + buf[5] * 256;
	bpp = buf[10];
	/* I'm not real sure about the bits-per-pixel, above */
 
	if (verbose)
		fprintf(stderr, "%d x %d x %d type %c, %s coloumap\n",
			width, height, 1 << bpp, buf[12], hascmap ? "has a" : "no");
	
 
	/* allocation equation depends on 8 | width (| = divides) */
	if ((p = img = malloc(bytesleft = width * height * bpp / 8)) == NULL) {
		fprintf(stderr, "not enough memory\n");
		exit(1);
	}
 
	if (hascmap) {
		if (!rd(cmap, 256 * 3))
			ueof();
		nl();
	
		for (i = 0; i < 256 * 3; i++)
			cmap[i] *= 4;
	}
 
	if (!rd(buf, 2))
		ueof();
 
	blocksleft = buf[0] + buf[1] * 256;
 
	while (brd(buf, 1)) {
		c = buf[0];
		if (c == spec) {
			brd(buf, 1);
			r = buf[0];
			if (r == 0) {
				brd(buf, 3);
				r = buf[1] * 256 + buf[0];
				c = buf[2];
				nl();
				if (verbose)
					fprintf(stderr, "%d repeats %d\n", c, r);
			}
			else {
				brd(buf, 1);
				c = buf[0];
			}
		}
		else {
			r = 1;
		}
		/*if (c == 0) {
			fprintf(stderr, "a zero at byte %d (%o)ish repeated %d times\n",
				ftell(stdin), ftell(stdin), r);
			exit(0);
		}*/
		while (r-- > 0) {
			if (--bytesleft < 0) {
				fprintf(stderr, "file too long, ignoring extra\n");
				goto dump;
			}
			*p++ = c;
		}
		if (c>maxc) maxc=c;
		if (bytesleft == 0)
			break;
	}
	if (!hascmap) {
		for (i = 0; i < 255 * 3; i++)
			cmap[i] = (255.0/maxc)*(i/3);
	}
 
	dump:
 
	if (bpp == 8) {
		printf("P6\n%d %d\n255\n", width, height);
 
		for (h = height - 1; h >= 0; h--)
			for (w = 0; w < width; w++) {
				c = img[h * width + w];
				putchar(cmap[c * 3]);
				putchar(cmap[c * 3 + 1]);
				putchar(cmap[c * 3 + 2]);
			}
	}
	else {
		printf("P4\n%d %d\n", width, height);
 
		for (h = height - 1; h >= 0; h--)
			for (w = 0; w < width / 8; w++)
				putchar(255 - img[h * width / 8 + w]);
	}
	
	exit(0);
}
 
rd(b, len)
char*	b;
int		len;
{
	int	i;
	int	c;
 
	for (i = 0; i < len; i++) {
		if ((c = getchar()) == EOF)
			return 0;
		b[i] = (c & 255);
		if (verbose > 1)
			fprintf(stderr, "%02x ", b[i]);
	}
	return 1;
}
 
brd(b, len)
char*	b;
int		len;
{
	unsigned char buf[5];
 
	if (curlen <= 0) {
		if (blocksleft == 0) {
			fprintf(stderr, "out of blocks (need %d more bytes)\n", bytesleft);
			exit(1);
		}
 
		nl();
		if (!rd(buf, 5))
			ueof();
 
		curlen = buf[0] + buf[1] * 256 - 5;
		/* size of buffer needed to store next block of pixels? */
		/* bufsize = buf[2] + buf[3] * 256; */
		spec = buf[4];
		blocksleft--;
		if (verbose /*|| spec*/) {
			nl();
			fprintf(stderr, "%d: block len=%d, bufsize=%d spec=%d (still need %d)\n",
				blocksleft, curlen, buf[2] + buf[3] * 256, spec, bytesleft);
		}
	}
 
	/* this should be done right and allow for overlapping reads! :-( */
 
	if (len > curlen) {
		fprintf(stderr, "arg, need to read over block boundry!\n");
		exit(1);
	}
 
	if (!rd(b, len))
		ueof();
	
	curlen -= len;
	
	return 1;
}
 
 
ueof()
{
	fprintf(stderr, "unexpected EOF\n");
	exit(1);
}
