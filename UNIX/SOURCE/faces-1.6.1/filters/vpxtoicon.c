
/*  @(#)vpxtoicon.c 1.1 91/05/06
 *
 *  Program to convert the output of Sun VideoPix (rasterfile format) files
 *  for use with faces. It converts the output of a black and white image
 *  (0 to 127) to a Sun icon format file.
 *
 *  Copyright (c) Dick Keene (Dick.Keene@East.Sun.COM)
 *                Sun Microsystems - April 1991.
 *                All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged.
 *
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 */

#include <stdio.h>
#include <strings.h>
#include <pixrect/pixrect_hs.h>

#define SCALEFP 256


char *command_name;

prusage()
{
    fprintf(stderr, 
	"\007Usage is: %s [options] <in raster> <out icon>\n", command_name);
    fprintf(stderr, "Options are:\n");
    fprintf(stderr, "    -x xsize, how big to make result. (default is 48)\n");
    fprintf(stderr, "    -y ysize, how big to make result. (default is 48)\n");
    fprintf(stderr, "    -n +-noise level, how much noise to inject to smooth the dither. (default is 2)\n");
    fprintf(stderr, "    -v, verbose (default is off)\n");
    fprintf(stderr, "    -c, don't contrast stretch. (default is to do stretch)\n");
    fprintf(stderr, "    -8, Don't dither, dump 8 bit rastefile. (default is to dump 1 bit icon)\n");
    fprintf(stderr, "    -1, Dump as 1 bit rasterfile. (default is to dump 1 bit icon)\n");
    fprintf(stderr, "    - for a file name means stdin or stdout.\n");
    exit(1);
}

/* a histogram equalization, so there ! */
void
histeq(pr)
Pixrect *pr;
{
    int hist[256];
    int integ[256];
    int i;
    int x, y;
    int sum;

    if(pr->pr_depth != 8) {
	fprintf(stderr, "\007histeq: Pixrect must be depth = 8 bits/pixel\n");
	prusage();
    }

    for(i=0; i<256; i++)
	hist[i] = 0;

    /* do histogram ... */
    for(y=0; y<pr->pr_size.y; y++) {
	for(x=0; x<pr->pr_size.x; x++) {
	    hist[pr_get(pr, x, y)] += 1;
	}
    }

    /* integrate ... */
    for(i=0, sum=0; i<256; i++) {
	sum += hist[i];
	integ[i] = (sum * 255) / (pr->pr_size.y * pr->pr_size.x);
	if(integ[i] > 255)
	    integ[i] = 255;
    }

    /* convert pixels ... */
    for(y=0; y<pr->pr_size.y; y++) {
	for(x=0; x<pr->pr_size.x; x++) {
	    pr_put(pr, x, y, integ[pr_get(pr, x, y)]);
	}
    }
}

main(argc, argv)
int argc;
char *argv[];
{
    int e;
    int p;
    int x,y; /* for scaleing */
    int xscale;
    int yscale;
    short bits;
    Pixrect *pr_orig; /* the orriginal loaded pixrect */
    Pixrect *pr; /* The 48 x 48 8 bit pixrect */
    Pixrect *pr1; /* The 1 bit result */
    char iname[256];
    char oname[256];
    FILE *ifp;
    FILE *ofp;
    int argcnt;
    char *argp;
    int minpix, maxpix; /* for contrast stretch */
    int facex;
    int facey;
    int verbose;
    int noise;
    int nostretch;
    int dump8bit;
    int dump1bit;

    command_name = argv[0];

    /* setup defaults */
    strcpy(iname, "");
    strcpy(oname, "");
    verbose = 0;
    facex = 48;
    facey = 48;
    noise = 2;
    verbose = 0;
    nostretch = 0;
    dump8bit = 0;
    dump1bit = 0;

    /* read command line if there */
    /* format is <argv[0]> [-v (verbose)] <in raster> <outfile> */
    for(argcnt = 1; argcnt < argc; argcnt++) {
	if(*argv[argcnt] == '-') {
	    argp = argv[argcnt] + 1;
	    if(*argp == '\0') { /* was just a "-" */
		if(*iname == '\0')
		    strcpy(iname, "-"); /* stdin */
		else if(*oname == '\0')
		    strcpy(oname, "-"); /* stdout */
		else {
			fprintf(stderr, "Too many dashes '-'\n");
			prusage();
		}
	    }
	    while(*argp != '\0') {
		if(*argp == 'v') {
		    verbose = 1;
		} else if(*argp == 'c') { /* don't contrast stretch */
		    nostretch = 1;
		} else if(*argp == '8') { /* Dump the 8 bit raster file */
		    dump8bit = 1;
		} else if(*argp == '1') { /* Dump the 1 bit raster file */
		    dump1bit = 1;
		} else if(*argp == 'x') { /* result x size */
		    if((argcnt + 1) >= argc) {
			fprintf(stderr, "-x needs a number after it, eg. -x 64\n");
			prusage();
		    }
		    argcnt++;
		    sscanf(argv[argcnt], "%d", &facex);
		} else if(*argp == 'y') { /* result y size */
		    if((argcnt + 1) >= argc) {
			fprintf(stderr, "-y needs a number after it, eg. -y 64\n");
			prusage();
		    }
		    argcnt++;
		    sscanf(argv[argcnt], "%d", &facey);
		} else if(*argp == 'n') { /* noise level */
		    if((argcnt + 1) >= argc) {
			fprintf(stderr, "-n needs a number after it, eg. -n 4\n");
			prusage();
		    }
		    argcnt++;
		    sscanf(argv[argcnt], "%d", &noise);
		} else {
		    fprintf(stderr, "Bad flag `%c` in %s\n", *argp, argv[argcnt]);
		    prusage();
		}
		argp++;
	    }
	} else { /* it's a file name */
	    if(*iname == '\0')
		strcpy(iname, argv[argcnt]);
	    else if(*oname == '\0')
		strcpy(oname, argv[argcnt]);
	    else {
		fprintf(stderr, "Too many file names.\n");
		prusage();
	    }
	}
    }
    
    /* open input file */
    if((strcmp(iname, "") == 0) || (strcmp(iname, "-") == 0)) {
	ifp = stdin;
    } else {
	ifp = fopen(iname, "r");
	if(ifp == NULL) {
	    fprintf(stderr, "\007%s unable to open input file %s\n", 
			argv[0], iname);
	    prusage();
	}
    }

    /* open output file */
    if((strcmp(oname, "-") == 0) || (strcmp(oname, "") == 0)) {
	ofp = stdout;
    } else {
	ofp = fopen(oname, "w");
	if(ofp == NULL) {
	    fprintf(stderr, "\007%s unable to open output file %s\n", 
			argv[0], oname);
	    prusage();
	}
    }

    /* load the original image */
    pr_orig = pr_load(ifp, NULL);
    if(pr_orig == NULL) {
	fprintf(stderr, "\007%s: Error loading raster file %s\n", argv[0], iname);
	prusage();
    }

    if(pr_orig->pr_depth != 8) {
	fprintf(stderr, "\007Must be 8 bits/pixel, %s is %d bits/pixel.\n",
		iname, pr_orig->pr_depth);
	prusage();
    }

    /* too small ? */
    if((pr_orig->pr_size.x < facex) || (pr_orig->pr_size.y < facey)) {
	fprintf(stderr, "\007%s:Raster file too small, must be at least %d x %d\n",
		 argv[0], facex, facey);
	fprintf(stderr, "(File is %d x %d)\n", pr_orig->pr_size.x, pr_orig->pr_size.y);
	prusage();
    }

    /* scale to facex x facey */
    if((pr_orig->pr_size.x > facex) || (pr_orig->pr_size.y > facey)) { /* scale it down */
	xscale = (facex * SCALEFP) / pr_orig->pr_size.x;
	yscale = (facey * SCALEFP) / pr_orig->pr_size.y;
	if(verbose)
	    fprintf(stderr, "Scaleing down to 48 x 48, scale factor = %lf x %lf\n", 
					(double)xscale / (double)SCALEFP,
					(double)yscale / (double)SCALEFP);
	pr = (Pixrect *)mem_create(facex, facey, 8);
	if(pr == NULL) {
	    fprintf(stderr,
		"\007%s: OUT OF MEMORY, unable to create scaled image\n", argv[0]);
	    prusage();
 	}
	for(y=0; y<facey; y++) {
	    for(x=0; x<facex; x++) {
		pr_put(pr, x, y, 
		    pr_get(pr_orig, (x * SCALEFP) / xscale, (y * SCALEFP) / yscale));
	    }
	}
    } else {
	pr = pr_orig;
    }

    /* contrast stretch */
    if(nostretch == 0) {
	histeq(pr);
    }

    if(dump8bit) {
        pr_dump(pr, ofp, NULL, RT_STANDARD, 0);
        exit(0);
   }

    /* error diffuse down to 1 bit/pixel */
    pr1 = (Pixrect *)mem_create(facex, facey, 1);
    if(pr1 == NULL) {
	fprintf(stderr, "\007%s: OUT OF MEMORY, unable to create output image\n", argv[0]);
	prusage();
    }

    e = 0;
    for(y=0; y<facey; y++) {
	for(x=0; x<facex; x++) {
	    p = pr_get(pr, x, y);
	    if((p + e) > 127) {
		pr_put(pr1, x, y, 0);
		e = p + e - 255 + (random() % (noise * 2 + 1)) - noise;
	    } else {
		pr_put(pr1, x, y, 1);
		e = p + e + (random() % (noise * 2 + 1)) - noise;
	    }
	}
    }

    if(dump1bit) {
        pr_dump(pr1, ofp, NULL, RT_STANDARD, 0);
        exit(0);
    }
    
    /* dump as icon */
    if((facex >= 64) || (facey >= 64)) {
	fprintf(stderr, "Icons can't be bigger than 64x64, cropping...\n");
    }

   fprintf(ofp, 
	"/* Format_version=1, Width=64, Height=64, Depth=1, Valid_bits_per_item=16\n");
    fprintf(ofp, " */");
    fprintf(ofp, "\n\t");
    bits = 0;
    for(y=0; y<64; y++) {
	for(x=0; x<64; x++) {
	    if(pr_get(pr1, x, y) == 0) {
		bits <<= 1;
	    } else {
		bits <<= 1;
		bits |= 0x01;
	    }
	    if((x % 16) == 15) {
		fprintf(ofp, "0x%04x", 0x0000ffff & bits);
	   	bits = 0;
	        if((x == 63) && (y == 63)) {
		    exit(0); /* done */
	        }
	        if((x % 64) == 63)
		    fprintf(ofp, ",\n\t");
	        else
		    fprintf(ofp, ",");
	    }
	}
    }   
    exit(0); /* should never get here */
}
