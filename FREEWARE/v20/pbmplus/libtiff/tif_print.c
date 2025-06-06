#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/libtiff/RCS/tif_print.c,v 1.32 91/07/16 16:30:59 sam Exp $";
#endif

/*
 * Copyright (c) 1988, 1989, 1990, 1991 Sam Leffler
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library.
 *
 * Directory Printing Support
 */
#include <stdio.h>
#include "tiffioP.h"

static char *ResponseUnitNames[] = {
	"#0",
	"10ths",
	"100ths",
	"1,000ths",
	"10,000ths",
	"100,000ths",
};
static	float ResponseUnit[] = { 1., .1, .01, .001, .0001, .00001 };
#define	MAXRESPONSEUNIT \
    (sizeof (ResponseUnitNames) / sizeof (ResponseUnitNames[0]))

#ifdef JPEG_SUPPORT
static
JPEGPrintQTable(fd, prec, tab)
	FILE *fd;
	int prec;
	u_short tab[64];
{
	int i, j;
	char *sep;

	fputc('\n', fd);
	for (i = 0; i < 8; i++) {
		sep = "    ";
		for (j = 0; j < 8; j++) {
			fprintf(fd, "%s%2u", sep, tab[8*i+j]);
			sep = ", ";
		}
		fputc('\n', fd);
	}
}

static
JPEGPrintCTable(fd, tab)
	FILE *fd;
	u_char *tab;
{
	int i, n, count;
	char *sep;

	fprintf(fd, "\n    Bits:");
	count = 0;
	for (i = 0; i < 16; i++) {
		fprintf(fd, " %u", tab[i]);
		count += tab[i];
	}
	n = 0;
	for (; count > 0; count--) {
		if ((n % 8) == 0) {
			fputc('\n', fd);
			sep = "    ";
		}
		fprintf(fd, "%s0x%02x", sep, tab[i++]);
		sep = ", ";
		n++;

	}
	if (n % 8)
		fputc('\n', fd);
}
#endif

/*
 * Print the contents of the current directory
 * to the specified stdio file stream.
 */
void
TIFFPrintDirectory(tif, fd, flags)
	TIFF *tif;
	FILE *fd;
	long flags;
{
	register TIFFDirectory *td;
	char *sep;
	int i;
	long n;
	float unit;

	fprintf(fd, "TIFF Directory at offset 0x%x\n", tif->tif_diroff);
	td = &tif->tif_dir;
	if (TIFFFieldSet(tif,FIELD_SUBFILETYPE)) {
		fprintf(fd, "  Subfile Type:");
		sep = " ";
		if (td->td_subfiletype & FILETYPE_REDUCEDIMAGE) {
			fprintf(fd, "%sreduced-resolution image", sep);
			sep = "/";
		}
		if (td->td_subfiletype & FILETYPE_PAGE) {
			fprintf(fd, "%smulti-page document", sep);
			sep = "/";
		}
		if (td->td_subfiletype & FILETYPE_MASK) {
			fprintf(fd, "%stransparency mask", sep);
			sep = "/";
		}
		fprintf(fd, " (%u = 0x%x)\n",
		    td->td_subfiletype, td->td_subfiletype);
	}
	if (TIFFFieldSet(tif,FIELD_IMAGEDIMENSIONS)) {
		fprintf(fd, "  Image Width: %lu Image Length: %lu",
		    td->td_imagewidth, td->td_imagelength);
		if (TIFFFieldSet(tif,FIELD_IMAGEDEPTH))
			fprintf(fd, " Image Depth: %lu", td->td_imagedepth);
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_TILEDIMENSIONS)) {
		fprintf(fd, "  Tile Width: %lu Tile Length: %lu",
		    td->td_tilewidth, td->td_tilelength);
		if (TIFFFieldSet(tif,FIELD_TILEDEPTH))
			fprintf(fd, " Tile Depth: %lu", td->td_tiledepth);
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_RESOLUTION)) {
		fprintf(fd, "  Resolution: %g, %g",
		    td->td_xresolution, td->td_yresolution);
		if (TIFFFieldSet(tif,FIELD_RESOLUTIONUNIT)) {
			switch (td->td_resolutionunit) {
			case RESUNIT_NONE:
				fprintf(fd, " (unitless)");
				break;
			case RESUNIT_INCH:
				fprintf(fd, " pixels/inch");
				break;
			case RESUNIT_CENTIMETER:
				fprintf(fd, " pixels/cm");
				break;
			default:
				fprintf(fd, " (unit %u = 0x%x)",
				    td->td_resolutionunit,
				    td->td_resolutionunit);
				break;
			}
		}
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_POSITION))
		fprintf(fd, "  Position: %g, %g\n",
		    td->td_xposition, td->td_yposition);
	if (TIFFFieldSet(tif,FIELD_BITSPERSAMPLE))
		fprintf(fd, "  Bits/Sample: %u\n", td->td_bitspersample);
	if (TIFFFieldSet(tif,FIELD_DATATYPE)) {
		fprintf(fd, "  Data Type: ");
		switch (td->td_datatype) {
		case DATATYPE_VOID:
			fprintf(fd, "void\n");
			break;
		case DATATYPE_INT:
			fprintf(fd, "signed integer\n");
			break;
		case DATATYPE_UINT:
			fprintf(fd, "unsigned integer\n");
			break;
		case DATATYPE_IEEEFP:
			fprintf(fd, "IEEE floating point\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_datatype, td->td_datatype);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_COMPRESSION)) {
		fprintf(fd, "  Compression Scheme: ");
		switch (td->td_compression) {
		case COMPRESSION_NONE:
			fprintf(fd, "none\n");
			break;
		case COMPRESSION_CCITTRLE:
			fprintf(fd, "CCITT modified Huffman encoding\n");
			break;
		case COMPRESSION_CCITTFAX3:
			fprintf(fd, "CCITT Group 3 facsimile encoding\n");
			break;
		case COMPRESSION_CCITTFAX4:
			fprintf(fd, "CCITT Group 4 facsimile encoding\n");
			break;
		case COMPRESSION_CCITTRLEW:
			fprintf(fd, "CCITT modified Huffman encoding %s\n",
			    "w/ word alignment");
			break;
		case COMPRESSION_PACKBITS:
			fprintf(fd, "Macintosh PackBits encoding\n");
			break;
		case COMPRESSION_THUNDERSCAN:
			fprintf(fd, "ThunderScan 4-bit encoding\n");
			break;
		case COMPRESSION_LZW:
			fprintf(fd, "Lempel-Ziv & Welch encoding\n");
			break;
		case COMPRESSION_PICIO:
			fprintf(fd, "Pixar picio encoding\n");
			break;
		case COMPRESSION_NEXT:
			fprintf(fd, "NeXT 2-bit encoding\n");
			break;
		case COMPRESSION_JPEG:
			fprintf(fd, "JPEG encoding\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_compression, td->td_compression);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PHOTOMETRIC)) {
		fprintf(fd, "  Photometric Interpretation: ");
		switch (td->td_photometric) {
		case PHOTOMETRIC_MINISWHITE:
			fprintf(fd, "min-is-white\n");
			break;
		case PHOTOMETRIC_MINISBLACK:
			fprintf(fd, "min-is-black\n");
			break;
		case PHOTOMETRIC_RGB:
			fprintf(fd, "RGB color\n");
			break;
		case PHOTOMETRIC_PALETTE:
			fprintf(fd, "palette color (RGB from colormap)\n");
			break;
		case PHOTOMETRIC_MASK:
			fprintf(fd, "transparency mask\n");
			break;
		case PHOTOMETRIC_SEPARATED:
			fprintf(fd, "separated\n");
			break;
		case PHOTOMETRIC_YCBCR:
			fprintf(fd, "YCbCr\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_photometric, td->td_photometric);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_MATTEING))
		fprintf(fd, "  Matteing: %s\n", td->td_matteing ?
		    "pre-multiplied with alpha channel" : "none");
	if (TIFFFieldSet(tif,FIELD_INKSET)) {
		fprintf(fd, "  Ink Set: ");
		switch (td->td_inkset) {
		case INKSET_CMYK:
			fprintf(fd, "CMYK\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_inkset, td->td_inkset);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_THRESHHOLDING)) {
		fprintf(fd, "  Thresholding: ");
		switch (td->td_threshholding) {
		case THRESHHOLD_BILEVEL:
			fprintf(fd, "bilevel art scan\n");
			break;
		case THRESHHOLD_HALFTONE:
			fprintf(fd, "halftone or dithered scan\n");
			break;
		case THRESHHOLD_ERRORDIFFUSE:
			fprintf(fd, "error diffused\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_threshholding, td->td_threshholding);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_FILLORDER)) {
		fprintf(fd, "  FillOrder: ");
		switch (td->td_fillorder) {
		case FILLORDER_MSB2LSB:
			fprintf(fd, "msb-to-lsb\n");
			break;
		case FILLORDER_LSB2MSB:
			fprintf(fd, "lsb-to-msb\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_fillorder, td->td_fillorder);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PREDICTOR)) {
		fprintf(fd, "  Predictor: ");
		switch (td->td_predictor) {
		case 1:
			fprintf(fd, "none\n");
			break;
		case 2:
			fprintf(fd, "horizontal differencing\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_predictor, td->td_predictor);
			break;
		}
	}
#ifdef JPEG_SUPPORT
	if (TIFFFieldSet(tif,FIELD_JPEGPROC)) {
		fprintf(fd, "  JPEG Processing Mode: ");
		switch (td->td_jpegproc) {
		case JPEGPROC_BASELINE:
			fprintf(fd, "baseline algorithm\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_jpegproc, td->td_jpegproc);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_JPEGQTABLEPREC)) {
		fprintf(fd, "  JPEG Quantization Table Precision: ");
		switch (td->td_jpegprec) {
		case JPEGQTABLEPREC_8BIT:
			fprintf(fd, "8-bit\n");
			break;
		case JPEGQTABLEPREC_16BIT:
			fprintf(fd, "16-bit\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_jpegprec, td->td_jpegprec);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_JPEGQTABLES)) {
		fprintf(fd, "  JPEG Quantization Tables: ");
		if (flags & TIFFPRINT_JPEGQTABLES) {
			for (i = 0; i < td->td_samplesperpixel; i++)
				JPEGPrintQTable(fd,
				    td->td_jpegprec,td->td_qtab[i]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_JPEGDCTABLES)) {
		fprintf(fd, "  JPEG DC Tables: ");
		if (flags & TIFFPRINT_JPEGDCTABLES) {
			for (i = 0; i < td->td_samplesperpixel; i++)
				JPEGPrintCTable(fd, td->td_dctab[i]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_JPEGACTABLES)) {
		fprintf(fd, "  JPEG AC Tables: ");
		if (flags & TIFFPRINT_JPEGACTABLES) {
			for (i = 0; i < td->td_samplesperpixel; i++)
				JPEGPrintCTable(fd, td->td_actab[i]);
		} else
			fprintf(fd, "(present)\n");
	}
#endif
	if (TIFFFieldSet(tif,FIELD_ARTIST))
		fprintf(fd, "  Artist: \"%s\"\n", td->td_artist);
	if (TIFFFieldSet(tif,FIELD_DATETIME))
		fprintf(fd, "  Date & Time: \"%s\"\n", td->td_datetime);
	if (TIFFFieldSet(tif,FIELD_HOSTCOMPUTER))
		fprintf(fd, "  Host Computer: \"%s\"\n", td->td_hostcomputer);
	if (TIFFFieldSet(tif,FIELD_SOFTWARE))
		fprintf(fd, "  Software: \"%s\"\n", td->td_software);
	if (TIFFFieldSet(tif,FIELD_DOCUMENTNAME))
		fprintf(fd, "  Document Name: \"%s\"\n", td->td_documentname);
	if (TIFFFieldSet(tif,FIELD_IMAGEDESCRIPTION))
		fprintf(fd, "  Image Description: \"%s\"\n",
		    td->td_imagedescription);
	if (TIFFFieldSet(tif,FIELD_MAKE))
		fprintf(fd, "  Make: \"%s\"\n", td->td_make);
	if (TIFFFieldSet(tif,FIELD_MODEL))
		fprintf(fd, "  Model: \"%s\"\n", td->td_model);
	if (TIFFFieldSet(tif,FIELD_ORIENTATION)) {
		fprintf(fd, "  Orientation: ");
		switch (td->td_orientation) {
		case ORIENTATION_TOPLEFT:
			fprintf(fd, "row 0 top, col 0 lhs\n");
			break;
		case ORIENTATION_TOPRIGHT:
			fprintf(fd, "row 0 top, col 0 rhs\n");
			break;
		case ORIENTATION_BOTRIGHT:
			fprintf(fd, "row 0 bottom, col 0 rhs\n");
			break;
		case ORIENTATION_BOTLEFT:
			fprintf(fd, "row 0 bottom, col 0 lhs\n");
			break;
		case ORIENTATION_LEFTTOP:
			fprintf(fd, "row 0 lhs, col 0 top\n");
			break;
		case ORIENTATION_RIGHTTOP:
			fprintf(fd, "row 0 rhs, col 0 top\n");
			break;
		case ORIENTATION_RIGHTBOT:
			fprintf(fd, "row 0 rhs, col 0 bottom\n");
			break;
		case ORIENTATION_LEFTBOT:
			fprintf(fd, "row 0 lhs, col 0 bottom\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_orientation, td->td_orientation);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_SAMPLESPERPIXEL))
		fprintf(fd, "  Samples/Pixel: %u\n", td->td_samplesperpixel);
	if (TIFFFieldSet(tif,FIELD_ROWSPERSTRIP)) {
		fprintf(fd, "  Rows/Strip: ");
		if (td->td_rowsperstrip == 0xffffffffL)
			fprintf(fd, "(infinite)\n");
		else
			fprintf(fd, "%u\n", td->td_rowsperstrip);
	}
	if (TIFFFieldSet(tif,FIELD_MINSAMPLEVALUE))
		fprintf(fd, "  Min Sample Value: %u\n", td->td_minsamplevalue);
	if (TIFFFieldSet(tif,FIELD_MAXSAMPLEVALUE))
		fprintf(fd, "  Max Sample Value: %u\n", td->td_maxsamplevalue);
	if (TIFFFieldSet(tif,FIELD_PLANARCONFIG)) {
		fprintf(fd, "  Planar Configuration: ");
		switch (td->td_planarconfig) {
		case PLANARCONFIG_CONTIG:
			fprintf(fd, "single image plane\n");
			break;
		case PLANARCONFIG_SEPARATE:
			fprintf(fd, "separate image planes\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_planarconfig, td->td_planarconfig);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PAGENAME))
		fprintf(fd, "  Page Name: \"%s\"\n", td->td_pagename);
	if (TIFFFieldSet(tif,FIELD_GRAYRESPONSEUNIT)) {
		fprintf(fd, "  Gray Response Unit: ");
		if (td->td_grayresponseunit < MAXRESPONSEUNIT)
			fprintf(fd, "%s\n",
			    ResponseUnitNames[td->td_grayresponseunit]);
		else
			fprintf(fd, "%u (0x%x)\n",
			    td->td_grayresponseunit, td->td_grayresponseunit);
	}
	if (TIFFFieldSet(tif,FIELD_GRAYRESPONSECURVE)) {
		fprintf(fd, "  Gray Response Curve: ");
		if (flags & TIFFPRINT_CURVES) {
			fprintf(fd, "\n");
			unit = ResponseUnit[td->td_grayresponseunit];
			n = 1L<<td->td_bitspersample;
			for (i = 0; i < n; i++)
				fprintf(fd, "    %2d: %g (%u)\n",
				    i,
				    td->td_grayresponsecurve[i] * unit,
				    td->td_grayresponsecurve[i]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_GROUP3OPTIONS)) {
		fprintf(fd, "  Group 3 Options:");
		sep = " ";
		if (td->td_group3options & GROUP3OPT_2DENCODING)
			fprintf(fd, "%s2-d encoding", sep), sep = "+";
		if (td->td_group3options & GROUP3OPT_FILLBITS)
			fprintf(fd, "%sEOL padding", sep), sep = "+";
		if (td->td_group3options & GROUP3OPT_UNCOMPRESSED)
			fprintf(fd, "%sno compression", sep), sep = "+";
		fprintf(fd, " (%u = 0x%x)\n",
		    td->td_group3options, td->td_group3options);
	}
	if (TIFFFieldSet(tif,FIELD_CLEANFAXDATA)) {
		fprintf(fd, "  Fax Data: ");
		switch (td->td_cleanfaxdata) {
		case CLEANFAXDATA_CLEAN:
			fprintf(fd, "clean\n");
			break;
		case CLEANFAXDATA_REGENERATED:
			fprintf(fd, "receiver regenerated\n");
			break;
		case CLEANFAXDATA_UNCLEAN:
			fprintf(fd, "uncorrected errors\n");
			break;
		default:
			fprintf(fd, "(%u = 0x%x)\n",
			    td->td_cleanfaxdata, td->td_cleanfaxdata);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_BADFAXLINES))
		fprintf(fd, "  Bad Fax Lines: %u\n", td->td_badfaxlines);
	if (TIFFFieldSet(tif,FIELD_BADFAXRUN))
		fprintf(fd, "  Consecutive Bad Fax Lines: %u\n",
		    td->td_badfaxrun);
	if (TIFFFieldSet(tif,FIELD_GROUP4OPTIONS))
		fprintf(fd, "  Group 4 Options: 0x%x\n", td->td_group4options);
	if (TIFFFieldSet(tif,FIELD_PAGENUMBER))
		fprintf(fd, "  Page Number: %u-%u\n",
		    td->td_pagenumber[0], td->td_pagenumber[1]);
	if (TIFFFieldSet(tif,FIELD_COLORMAP)) {
		fprintf(fd, "  Color Map: ");
		if (flags & TIFFPRINT_COLORMAP) {
			fprintf(fd, "\n");
			n = 1L<<td->td_bitspersample;
			for (i = 0; i < n; i++)
				fprintf(fd, "   %5d: %5u %5u %5u\n",
				    i,
				    td->td_redcolormap[i],
				    td->td_greencolormap[i],
				    td->td_bluecolormap[i]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_COLORRESPONSEUNIT)) {
		fprintf(fd, "  Color Response Unit: ");
		if (td->td_colorresponseunit < MAXRESPONSEUNIT)
			fprintf(fd, "%s\n",
			    ResponseUnitNames[td->td_colorresponseunit]);
		else
			fprintf(fd, "%u (0x%x)\n",
			    td->td_colorresponseunit, td->td_colorresponseunit);
	}
	if (TIFFFieldSet(tif,FIELD_COLORRESPONSECURVE)) {
		fprintf(fd, "  Color Response Curve: ");
		if (flags & TIFFPRINT_CURVES) {
			fprintf(fd, "\n");
			unit = ResponseUnit[td->td_colorresponseunit];
			n = 1L<<td->td_bitspersample;
			for (i = 0; i < n; i++)
				fprintf(fd, "    %2d: %6.4f %6.4f %6.4f\n",
				    i,
				    td->td_redresponsecurve[i] * unit,
				    td->td_greenresponsecurve[i] * unit,
				    td->td_blueresponsecurve[i] * unit);
		} else
			fprintf(fd, "(present)\n");
	}
	if ((flags & TIFFPRINT_STRIPS) &&
	    TIFFFieldSet(tif,FIELD_STRIPOFFSETS)) {
		fprintf(fd, "  %u %s:\n",
		    td->td_nstrips,
		    isTiled(tif) ? "Tiles" : "Strips");
		for (i = 0; i < td->td_nstrips; i++)
			fprintf(fd, "    %3d: [%8u, %8u]\n",
			    i, td->td_stripoffset[i], td->td_stripbytecount[i]);
	}
}
