/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <stdio.h>
#include "Image.h"
#include "libpnmrw.h"

#define TRUE 1
#define FALSE 0

Image	*ReadPNM(char *file)
{
	int		x, y;
	xel		**els = NULL;
	xel		*curel;
	xelval		maxval;
	int		pnm_type;
	FILE		*fd;
	Image		*image;
	int		format;
	int		wth, hth;
	unsigned char	*dp;

	pnm_init2(file);
	if ((fd = fopen(file, "r")) == NULL)
		return NULL;
	if ((els = pnm_readpnm(fd, &wth, &hth, &maxval, &format))==NULL)
		return NULL;
	pnm_type = PNM_FORMAT_TYPE(format);
	fclose(fd);
	
	if (pnm_type == PBM_TYPE) {
		image = ImageNewBW(wth, hth);
	} else if (pnm_type == PGM_TYPE) {
		image = ImageNewGrey(wth, hth);
	} else {
		image = ImageNew(wth, hth);
	}
	dp = image->data;

	for (y = 0; y < hth; y++) {
		for (curel = els[y], x = 0; x < wth; x++, curel++) {
			if (pnm_type == PPM_TYPE) {
				*dp++ = 255 * PPM_GETR(*curel) / maxval;
				*dp++ = 255 * PPM_GETG(*curel) / maxval;
				*dp++ = 255 * PPM_GETB(*curel) / maxval;
			} else if (pnm_type == PGM_TYPE) {
				*dp++ = 255 * PNM_GET1(*curel) / maxval;
			} else {
				*dp++ = (PNM_GET1(*curel) != PBM_WHITE) ? 1 : 0;
			}
		}
	}

	pnm_freearray(els, hth);

	return image;
}

int WritePNM(char *file, Image *image)
{
	xel		**xels;
	int		x, y;
	unsigned char	*ip;
	FILE		*fd;
	int		format;

	if ((fd = fopen(file, "w")) == NULL) 
		return FALSE;

	if (image->isBW)
		format = PBM_FORMAT;
	else if (image->isGrey)
		format = PGM_FORMAT;
	else
		format = PPM_FORMAT;

	pnm_init2(file);

	xels = pnm_allocarray(image->width, image->height);

	for (y = 0; y < image->height; y++) {
		for (x = 0; x < image->width; x++)  {
			ImagePixel(image, x, y, ip);
			PPM_ASSIGN(xels[y][x], ip[0], ip[1], ip[2]);
		}
	}

	pnm_writepnm(fd, xels, image->width, image->height, 255, format, FALSE);

	pnm_freearray(xels, image->height);
	fclose(fd);

	return TRUE;
}

int TestPNM(char *file)
{
	FILE	*fd;
	int	ret = FALSE;
	char	buf[4];

	if ((fd = fopen(file, "r")) == NULL) 
		return FALSE;

	if (fread(buf, sizeof(char), 3, fd) == 3) {
		if (buf[0] == 'P') {
			switch (buf[1]) {
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
				ret = TRUE;
			default:
				break;
			}
		}
	}

	fclose(fd);

	return ret;
}
