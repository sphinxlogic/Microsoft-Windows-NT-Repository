/*
 * xvpcx.c - load routine for PCX format pictures
 *
 * LoadPCX(fname, pinfo)  -  loads a PCX file
 */

#include "copyright.h"

/*
 * the following code has been derived from code written by
 *  Eckhard Rueggeberg  (Eckhard.Rueggeberg@ts.go.dlr.de)
 */


#include "xv.h"

/* offsets into PCX header */
#define PCX_ID      0
#define PCX_VER     1
#define PCX_ENC     2
#define PCX_BPP     3
#define PCX_XMINL   4
#define PCX_XMINH   5
#define PCX_YMINL   6
#define PCX_YMINH   7
#define PCX_XMAXL   8
#define PCX_XMAXH   9
#define PCX_YMAXL   10
#define PCX_YMAXH   11
                          /* hres (12,13) and vres (14,15) not used */
#define PCX_CMAP    16    /* start of 16*3 colormap data */
#define PCX_PLANES  65 
#define PCX_BPRL    66
#define PCX_BPRH    67

#define PCX_MAPSTART 0x0c	/* Start of appended colormap	*/


static int  pcxLoadImage  PARM((char *, FILE *, byte *, byte *, int, int));
static void pcxLoadRaster PARM((FILE *, byte *, int, byte *, int, int));
static int  pcxError      PARM((char *, char *));



/*******************************************/
int LoadPCX(fname, pinfo)
     char    *fname;
     PICINFO *pinfo;
/*******************************************/
{
  FILE  *fp;
  long   filesize;
  char  *bname, *errstr;
  byte   hdr[128], *image;
  int    i, colors, gray;

  pinfo->type = PIC8;
  pinfo->pic     = (byte *) NULL;
  pinfo->comment = (char *) NULL;

  bname = BaseName(fname);

  /* open the stream */
  fp = xv_fopen(fname,"r");
  if (!fp) return (pcxError(bname, "unable to open file"));
  

  /* figure out the file size */
  fseek(fp, 0L, 2);
  filesize = ftell(fp);
  fseek(fp, 0L, 0);


  /* read the PCX header */
  fread(hdr, (size_t) 128, (size_t) 1, fp);
  if (ferror(fp) || feof(fp)) {
    fclose(fp);
    return pcxError(bname, "EOF reached in PCX header.\n");
  }

  if (hdr[PCX_ID] != 0x0a || hdr[PCX_VER] > 5) {
    fclose(fp);
    return pcxError(bname,"unrecognized magic number");
  }

  pinfo->w = (hdr[PCX_XMAXL] + ((int) hdr[PCX_XMAXH]<<8)) 
           - (hdr[PCX_XMINL] + ((int) hdr[PCX_XMINH]<<8));

  pinfo->h = (hdr[PCX_YMAXL] + ((int) hdr[PCX_YMAXH]<<8)) 
           - (hdr[PCX_YMINL] + ((int) hdr[PCX_YMINH]<<8));

  pinfo->w++;  pinfo->h++;

  colors = 1 << (hdr[PCX_BPP] * hdr[PCX_PLANES]);

  if (DEBUG) {
    fprintf(stderr,"PCX: %dx%d image, version=%d, encoding=%d\n", 
	    pinfo->w, pinfo->h, hdr[PCX_VER], hdr[PCX_ENC]);
    fprintf(stderr,"   BitsPerPixel=%d, planes=%d, BytePerRow=%d, colors=%d\n",
	    hdr[PCX_BPP], hdr[PCX_PLANES], 
	    hdr[PCX_BPRL] + ((int) hdr[PCX_BPRH]<<8),
	    colors);
  }

  if (colors>256) {
    fclose(fp);
    return pcxError(bname,"No more than 256 colors allowed in PCX file.");
  }

  if (hdr[PCX_ENC] != 1) {
    fclose(fp);
    return pcxError(bname,"Unsupported PCX encoding format.");
  }

  /* note:  overallocation to make life easier... */
  image = (byte *) malloc((size_t) (pinfo->h + 1) * pinfo->w + 16);
  if (!image) FatalError("Can't alloc 'image' in LoadPCX()");

  xvbzero((char *) image, (size_t) ((pinfo->h+1) * pinfo->w + 16));

  if (!pcxLoadImage(bname, fp, image, hdr, pinfo->w, pinfo->h)) {
    free(image);
    fclose(fp);
    return 0;
  }


  if (ferror(fp) | feof(fp))    /* just a warning */
    pcxError(bname, "PCX file appears to be truncated.");

  if (colors>16) {       /* handle trailing colormap */
    while (1) {
      i=getc(fp);
      if (i==PCX_MAPSTART || i==EOF) break;
    }

    for (i=0; i<colors; i++) {
      pinfo->r[i] = getc(fp);
      pinfo->g[i] = getc(fp);
      pinfo->b[i] = getc(fp);
    }

    if (ferror(fp) || feof(fp)) {
      pcxError(bname,"Error reading PCX colormap.  Using grayscale.");
      for (i=0; i<256; i++) pinfo->r[i] = pinfo->g[i] = pinfo->b[i] = i;
    }
  }
  else if (colors<=16) {   /* internal colormap */
    for (i=0; i<colors; i++) {
      pinfo->r[i] = hdr[PCX_CMAP + i*3];
      pinfo->g[i] = hdr[PCX_CMAP + i*3 + 1];
      pinfo->b[i] = hdr[PCX_CMAP + i*3 + 2];
    }
  }

  if (colors == 2) {    /* b&w */
    if (MONO(pinfo->r[0], pinfo->g[0], pinfo->b[0]) ==
	MONO(pinfo->r[1], pinfo->g[1], pinfo->b[1])) {
      /* create cmap */
      pinfo->r[0] = pinfo->g[0] = pinfo->b[0] = 255;
      pinfo->r[1] = pinfo->g[1] = pinfo->b[1] = 0;
      if (DEBUG) fprintf(stderr,"PCX: no cmap:  using 0=white,1=black\n");
    }
  }


  fclose(fp);



  /* finally, convert into XV internal format */


  pinfo->pic     = image;
  pinfo->type    = PIC8;
  pinfo->frmType = -1;    /* no default format to save in */

  /* check for grayscaleitude */
  for (i=0; i<colors; i++) {
    if ((pinfo->r[i] != pinfo->g[i]) || (pinfo->r[i] != pinfo->b[i])) break;
  }
  gray = (i==colors) ? 1 : 0;


  if (colors > 2 || (colors==2 && !gray)) {  /* grayscale or PseudoColor */
    pinfo->colType = (gray) ? F_GREYSCALE : F_FULLCOLOR;
    sprintf(pinfo->fullInfo, 
	    "%s PCX, %d plane%s, %d bit%s per pixel.  (%ld bytes)", 
	    (gray) ? "Greyscale" : "Color", 
	    hdr[PCX_PLANES], (hdr[PCX_PLANES]==1) ? "" : "s",
	    hdr[PCX_BPP],    (hdr[PCX_BPP]==1) ? "" : "s",
	    filesize);
  }
  else {
    pinfo->colType = F_BWDITHER;
    sprintf(pinfo->fullInfo, "B&W PCX.  (%ld bytes)", filesize);
  }

  sprintf(pinfo->shrtInfo, "%dx%d PCX.", pinfo->w, pinfo->h);
  pinfo->normw = pinfo->w;   pinfo->normh = pinfo->h;

  return 1;
}



/*****************************/
static int pcxLoadImage(fname, fp, image, hdr, w, h)
     char *fname;
     FILE *fp;
     byte *image, *hdr;
     int   w, h;
{
  switch (hdr[PCX_BPP]) {
  case 1:   pcxLoadRaster(fp, image, 1, hdr, w, h);   break;
  case 8:   pcxLoadRaster(fp, image, 8, hdr, w, h);   break;
  default:
    pcxError(fname, "Unsupported # of bits per plane.");
    return (0);
  }

  return 1;
}




/*****************************/
static void pcxLoadRaster(fp, image, depth, hdr, w,h)
     FILE    *fp;
     byte    *image, *hdr;
     int      depth,w,h;
{
  /* supported:  8 bits per pixel, 1 plane, or 1 bit per pixel, 1-8 planes */

  int row, bcnt, bperlin, pad;
  int i, j, b, cnt, mask, plane, pmask;
  byte *oldimage;

  bperlin = hdr[PCX_BPRL] + ((int) hdr[PCX_BPRH]<<8);
  if (depth == 1) pad = (bperlin * 8) - w;
             else pad = bperlin - w;

  row = bcnt = 0;

  plane = 0;  pmask = 1;  oldimage = image;

  while ( (b=getc(fp)) != EOF) {
    if ((b & 0xC0) == 0xC0) {   /* have a rep. count */
      cnt = b & 0x3F;
      b = getc(fp);
      if (b == EOF) { getc(fp); return; }
    }
    else cnt = 1;
    
    for (i=0; i<cnt; i++) {
      if (depth == 1) {
	for (j=0, mask=0x80; j<8; j++) {
	  *image++ |= ((b & mask) ? pmask : 0);
	  mask = mask >> 1;
	}
      }
      else *image++ = (byte) b;
      
      bcnt++;
	
      if (bcnt == bperlin) {     /* end of a line reached */
	bcnt = 0;
	plane++;  

	if (plane >= (int) hdr[PCX_PLANES]) {   /* moved to next row */
	  plane = 0;
	  image -= pad;
	  oldimage = image;
	  row++;
	  if (row >= h) return;   /* done */
	}
	else {   /* next plane, same row */
	  image = oldimage;
	}	

	pmask = 1 << plane;
      }
    }
  }
}    



/*******************************************/
static int pcxError(fname,st)
     char *fname, *st;
{
  SetISTR(ISTR_WARNING,"%s:  %s", fname, st);
  return 0;
}

