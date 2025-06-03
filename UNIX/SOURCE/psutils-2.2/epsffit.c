/* epsffit.c
 * AJCD 6 Dec 90
 * fit epsf file into constrained size
 * Usage:
 *       epsffit [-c] [-r] [-a] [-s] llx lly urx ury
 *               -c centres the image in the bounding box given
 *               -r rotates the image by 90 degrees anti-clockwise
 *               -a alters the aspect ratio to fit the bounding box
 *               -s adds a showpage at the end of the image
 */

#include <stdio.h>
#include <ctype.h>

#define min(x,y) ((x) > (y) ? (y) : (x))
#define max(x,y) ((x) > (y) ? (x) : (y))

static char *prog;

usage()
{
   fprintf(stderr, "Usage: %s [-c] [-r] [-a] [-s] llx lly urx ury\n", prog);
   exit(1);
}

main(argc, argv)
     int argc;
     char **argv;
{
   int fit[4], i;
   int bbfound = 0;              /* %%BoundingBox: found */
   int urx, ury, llx, lly;
   int furx, fury, fllx, flly, fwidth, fheight;
   int showpage = 0, centre = 0, rotate = 0, aspect = 0;
   char buf[BUFSIZ];

   prog = *argv++; argc--;

   while (argc > 0 && argv[0][0] == '-') {
      switch (argv[0][1]) {
      case 'c': centre = 1; break;
      case 's': showpage = 1; break;
      case 'r': rotate = 1; break;
      case 'a': aspect = 1; break;
      default:  usage();
      }
      argc--;
      argv++;
   }

   if (argc != 4) usage();
   fllx = atoi(argv[0]);
   flly = atoi(argv[1]);
   furx = atoi(argv[2]);
   fury = atoi(argv[3]);
   if (rotate) {
      fwidth = fury - flly;
      fheight = furx - fllx;
   } else {
      fwidth = furx - fllx;
      fheight = fury - flly;
   }

   while (fgets(buf, BUFSIZ, stdin)) {
      if (buf[0] == '%' && (buf[1] == '%' || buf[1] == '!')) {
	 /* still in comment section */
	 if (!strncmp(buf, "%%BoundingBox:", 14)) {
	    if (sscanf(buf, "%%%%BoundingBox:%d %d %d %d\n",
		       &llx, &lly, &urx, &ury) == 4)
	       bbfound = 1;
	 } else if (!strncmp(buf, "%%EndComments", 13)) {
	    strcpy(buf, "\n"); /* don't repeat %%EndComments */
	    break;
	 } else fputs(buf,stdout);
      } else break;
   }
   if (bbfound) { /* put BB, followed by scale&translate */
      double width = urx-llx, height = ury-lly;
      double xscale = fwidth/width, yscale = fheight/height;
      double xoffset = fllx, yoffset = flly;
      if (!aspect) {       /* preserve aspect ratio ? */
	 xscale = yscale = min(xscale,yscale);
      }
      width *= xscale;     /* actual width and height after scaling */
      height *= yscale;
      if (centre) {
	 if (rotate) {
	    xoffset += (fheight - height)/2;
	    yoffset += (fwidth - width)/2;
	 } else {
	    xoffset += (fwidth - width)/2;
	    yoffset += (fheight - height)/2;
	 }
      }
      printf("%%%%BoundingBox: %d %d %d %d\n", (int)xoffset, (int)yoffset,
	     (int)(xoffset+(rotate ? height : width)),
	     (int)(yoffset+(rotate ? width : height)));
      if (rotate) {  /* compensate for original image shift */
	 xoffset += height + lly * yscale;  /* displacement for rotation */
	 yoffset -= llx * xscale;
      } else {
	 xoffset -= llx * xscale;
	 yoffset -= lly * yscale;
      }
      puts("%%EndComments");
      if (showpage)
	 puts("save /showpage{}def /copypage{}def /erasepage{}def");
      else
	 puts("%%BeginProcSet: epsffit 1 0");
      puts("gsave");
      printf("%.3lf %.3lf translate\n", xoffset, yoffset);
      if (rotate)
	 puts("90 rotate");
      printf("%.3lf %.3lf scale\n", xscale, yscale);
      if (!showpage)
	 puts("%%EndProcSet");
   }
   do {
      fputs(buf,stdout);
   } while (fgets(buf, BUFSIZ, stdin));
   if (bbfound) {
      puts("grestore");
      if (showpage)
	 puts("restore showpage"); /* just in case */
   } else {
      fprintf(stderr, "%s: no %%%%BoundingBox:\n", prog);
      exit(1);
   }
   exit(0);
}
