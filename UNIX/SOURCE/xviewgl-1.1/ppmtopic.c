/*
** ppmtopic.c - read a portable pixmap and produce a PCPAINT/Pictor file.
**
**	usage: ppmtopic [-p] [-c colormap-ppm-file] [ ppm-file-to-convert ]
**
**	Where -p indicates that the output file should include a colormap (i.e.
**	produce a .PIC file.)  The default is to include a colormap (i.e.
**	produce a .CLP file) unless the -c option is specified.  In order
**	to produce a colormap when using the -c option, you must also specify
**	-p.
**
**	The colormap-ppm-file is the name of a portable pixmap to use in
**	generating the colormap.  Usually it is a collage of several images,
**	which are then converted to .CLP files separately so that they can
**	be displayed in sequence using a single color map.  This option
**	suppresses the generation of an output colormap unless the -p option
**	is also specified.
**
**	The ppm-file-to-convert is just what the name implies, the file to
**	convert.
**
** Adapted from:
**
** ppmtopict.c - read a portable pixmap and produce a PICT2 file.
**
** Copyright (C) 1990 by Ken Yap.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <stdio.h>
#include "ppm.h"
#include "ppmcmap.h"
#include "pict.h"

#define MAXCOLORS 256
colorhash_table cht;

main(argc, argv)
int argc;
char *argv[];
{
	FILE *ifd;
	int argn, rows, cols, colors, i, row, oc;
	register pixel **pixels;
	char *packed;
	pixval maxval;
	long lmaxval, rval, gval, bval;
	colorhist_vector chv;
	char *malloc();
	int write_cmap = 2, got_cmap = 0;

	pm_progname = argv[0];

	argn = 1;
	while ((argn < argc) && (argv[argn][0] == '-'))
	{
	    if (argv[argn][1] == 'p') {
		write_cmap = 1;
		argn++;
	    } else if (argv[argn][1] == 'c') {
		argn++;
		ifd = pm_openr(argv[argn]);
		argn++;
		pixels = ppm_readppm(ifd, &cols, &rows, &maxval);
		lmaxval = (long)maxval;
		pm_close(ifd);

		/* Figure out the colormap. */
		fprintf(stderr, "(Computing colormap...");
		fflush(stderr);
		chv = ppm_computecolorhist(pixels, cols, rows, MAXCOLORS, &colors);
		if (chv == (colorhist_vector) 0)
		{
		    fprintf(stderr, "  Too many colors!  Try running the pixmap through 'ppmquant 256'.\n");
		    exit(1);
		}
		fprintf(stderr, "  Done.  %d colors found.)\n", colors);
		/* Make a hash table for fast color lookup. */
		cht = ppm_colorhisttocolorhash(chv, colors);
		ppm_freearray(pixels,rows);
		got_cmap = 1;
		if (write_cmap == 2)
		    write_cmap = 0;
	    } else {
		pm_error("usage: %s [-p] [-c colormap-ppm-file] [ppm-file-to-convert]",argv[0],0,0,0);
	    }
	}
	if (argn < argc)
	{
	    ifd = pm_openr(argv[argn]);
	    argn++;
	    if (argn < argc)
		pm_error("usage: %s [-p] [-c colormap-ppm-file] [ppm-file-to-convert]",argv[0],0,0,0);
	}
	else
	    ifd = stdin;

	pixels = ppm_readppm(ifd, &cols, &rows, &maxval);
	lmaxval = (long)maxval;
	pm_close(ifd);

	if (!got_cmap) {
	    /* Figure out the colormap. */
	    fprintf(stderr, "(Computing colormap...");
	    fflush(stderr);
	    chv = ppm_computecolorhist(pixels, cols, rows, MAXCOLORS, &colors);
	    if (chv == (colorhist_vector) 0)
	    {
		    fprintf(stderr, "  Too many colors!  Try running the pixmap through 'ppmquant 256'.\n");
		    exit(1);
	    }
	    fprintf(stderr, "  Done.  %d colors found.)\n", colors);
	    /* Make a hash table for fast color lookup. */
	    cht = ppm_colorhisttocolorhash(chv, colors);
	}

	/* write the magic number */
	putShort(stdout, 0x1234);

	/* write width and height */
	putShort(stdout, cols);
	putShort(stdout, rows);

	/* No offsets */
	putShort(stdout, 0);
	putShort(stdout, 0);

	putByte(stdout,8);	/* 8 bits/pixel */
	putByte(stdout,0xff);	/* Marker */
	putByte(stdout,'L');	/* VGA */
	if (!write_cmap) {
	/* No color map.  (.CLP file) */
	    putShort(stdout,0);
	    putShort(stdout, 0);
	} else {
	/* VGA palette */
	    putShort(stdout,4);
	    putShort(stdout, 768);	/* Size of VGA cmap */
	    for (i = 0; i < colors; i++)
	    {
		rval = PPM_GETR(chv[i].color);
		gval = PPM_GETG(chv[i].color);
		bval = PPM_GETB(chv[i].color);
		rval = rval * 63 / lmaxval;
		gval = gval * 63 / lmaxval;
		bval = bval * 63 / lmaxval;
		putByte(stdout, rval);
		putByte(stdout, gval);
		putByte(stdout, bval);
	    }
	    while (i < 256) {
		putByte(stdout, 0);
		putByte(stdout, 0);
		putByte(stdout, 0);
		i++;
	    }
	}

	/* Finally, write out the data. */
	putShort(stdout,rows);		/* One block per row. */
	packed = malloc(cols);
	for (row = rows-1; row >= 0; row--)
	    putRow(stdout, cols, pixels[row], packed);

	exit(0);
}

putByte(fd, i)
FILE *fd;
int i;
{
	putc(i & 0xff, fd);
}

putShort(fd, i)
FILE *fd;
int i;
{
	putc(i & 0xff, fd);
	putc((i >> 8) & 0xff, fd);
}

int putRow(fd, cols, rowpixels, packed)
FILE *fd;
int cols;
pixel *rowpixels;
char *packed;
{
int pixc[256];
int i,minc,minp;
int lastc, run,pix;
char *p=packed;
    memset(pixc,0,256*sizeof(int));
    for	(i=0;i<cols;i++) {
	pixc[ppm_lookupcolor(cht, rowpixels[i])]++;
    }
    minc=cols;
    for (i=0;(minc) && (i<256);i++) {
	if (pixc[i] < minc) {
	    minc = pixc[i];
	    minp = i;
	}
    }
    for (lastc=ppm_lookupcolor(cht, rowpixels[0]),run=1,i=1;i<cols;i++) {
	pix = ppm_lookupcolor(cht, rowpixels[i]);
	if (pix == lastc)
	    run++;
	else if ((run < 3) && (lastc != minp)) {
	    while (run--)
		*p++ = lastc;
	    lastc = pix;
	    run = 1;
	} else {
	    *p++ = minp;
	    if (run>255) {
		*p++ = 0;
		*p++ = run&0xff;
		*p++ = (run>>8)&0xff;
	    } else {
		*p++ = run;
	    }
	    *p++ = lastc;
	    lastc = pix;
	    run = 1;
	}
    }
    if ((run < 4) && (lastc != minp)) {
	while (run--)
	    *p++ = lastc;
    } else {
	*p++ = minp;
	if (run>255) {
	    *p++ = 0;
	    *p++ = run&0xff;
	    *p++ = (run>>8)&0xff;
	} else {
	    *p++ = run;
	}
	*p++ = lastc;
    }
    putShort(stdout,p-packed+5);
    putShort(stdout,cols);
    putByte(stdout,minp);
    while (packed<p) {
	putByte(stdout,*(packed++));
    }
}
