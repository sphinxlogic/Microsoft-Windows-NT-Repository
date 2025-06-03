#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_types.h"
#include "libutils.h"
#include "pcx.h"
#include "pic.h"
#include "registers.h"
#include "utils.h"
#include "windows.h"

BOOLEAN pcx_load(char *fname, int reg, BOOLEAN load_cmap, int trancolor, char *err_msg)
{
MY_FILE *fp;
char fullname[FILENAME_MAX],vtype;
byte *buf=pic_alloc(128), *p, *clist, esc;
struct PIC_register *r = load_cmap?&preg[reg]:&creg[reg];
byte *img;
int bytesperline, bytesleft, blocksleft;
int blen, i, c, l, planes, bpp,magic,edesc,elen;
unsigned cscalemax=MainWindow.cscalemax;

    clear_preg(r);

    if ((fp=find_file(fname)) == NULL) {
	sprintf(err_msg,"Can't open pic file %s: %s",
		fname,strerror(errno));
	return FALSE;
    }

    if (!pic_read(buf, 128, fp)) {
	sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
	return FALSE;
    }

    /* Byte 0 is magic number - always 0xa */
    if (buf[0] != 0xa) {
	sprintf(err_msg,"File %s is not a PC Paintbrush file",fname);
	return FALSE;
    }
    /* Byte 1 is version number */
    /* Byte 2 is encoding type: */
    if (buf[2] != 1) {
	sprintf(err_msg,"File %s is not run-length encoded",fname);
	return FALSE;
    }

    bpp = buf[3];

    /* Width = xmax - xmin + 1 */
    r->width = buf[8] + buf[9] * 256 - buf[4] - buf[5] * 256 + 1;
    /* Height = ymax - ymin + 1 */
    r->height = buf[10] + buf[11] * 256 - buf[6] - buf[7] * 256 + 1;
    /* Bytes 12-13 are horizontal resolution, 14-15 are vertical res */
    /* Bytes 16-63 are 16-color RGB color map */
    /* Byte 64 is screen mode (not used) */
    planes = buf[65];
    r->depth = planes*bpp;
    bytesperline = buf[66] + buf[67] * 256;
    /* Bytes 67-68 contain palette info */
    r->hascmap = load_cmap;
    if (load_cmap && (r->depth <= 4)) {
    /* Get 16-color RGB color map */
	clist = malloc(16*3);
	for (i=0; i< 16; i++) {
	    clist[i*3] = buf[i*3+16];	/* Red */
	    clist[i*3+1] = buf[i*3+17];	/* Green */
	    clist[i*3+2] = buf[i*3+18];	/* Blue */
	}
	r->cmap = convert_clist_to_cmap(clist,reg,255);
	free(clist);
    }
    /* 8 bit color maps are stored after the image */

    r->maxpval=0;
    if ((p = img = malloc(bytesleft = bytesperline*r->height*planes))
	== NULL) {
	sprintf(err_msg, "Error allocating memory for PCX file %s: %s",
		fname, strerror(errno));
	return FALSE;
    }
    while (bytesleft > 0) {
	c = my_getc(fp);
	/* If bit 6 or bit 7 is clear, the character is a literal. */
	if ((c&0xc0) != 0xc0) {
	    *p++=c;
	    bytesleft--;
	} else {
	    l = c & 0x3f; /* Mask out run-length info flags to get run length */
	    c = my_getc(fp);
	    memset(p,c,l);
	    p += l;
	    bytesleft -= l;
	    if (bytesleft < 0) {
		fprintf(stderr,"File %s too long.  Ignoring extra.\n", fname);
	    }
	}
	if (c>r->maxpval)
	    r->maxpval = c;
    }

    if (r->depth > 4) {
    /* Get 256-color RGB color map */
	/* 0xC indicates color map follows */
	if ((c = my_getc(fp)) == 0xc) {
	    clist = malloc(256*3);
	    if (!pic_read(clist,256*3,fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    if (load_cmap)
		r->cmap = convert_clist_to_cmap(clist,reg,255);
	    free(clist);
	}
    }

    /* Decode planes and turn image upside down (so convert_img_for_screen
       will do the right thing.) */
    if (planes>1) {
    int planesize,dstlinesize,mask,pmask,sshiftl,tshiftl,dshiftl,j;
    byte *p2,*lastplane,*img2;
	r->maxpval = (1<<r->depth) - 1;
	planesize = r->height * bytesperline;
	lastplane = img + (planes-1)*planesize;
	dstlinesize = (r->width*r->depth+7)/8;
	pmask = (1<<bpp) - 1;
	if ((img2 = calloc(r->height * dstlinesize,1)) == NULL) {
	    sprintf(err_msg, "Error allocating memory while decoding pcx file %s: %s",
		    fname, strerror(errno));
	    return FALSE;
	}
	for (i=0;i<r->height;i++) {
	    for (dshiftl=sshiftl=8-bpp,j=0;j<r->width;j++) {
		mask = pmask << sshiftl;
		p = lastplane + i*bytesperline + j*bpp/8;
		p2 = img2 + (r->height-i)*dstlinesize + j*r->depth/8;
		switch(planes) {
		    case 8:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 7:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 6:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 5:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 4:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 3:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 2:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
			p -= planesize;
		    case 1:
			*p2 |= (((*p)&mask)>>sshiftl)<<dshiftl;
			dshiftl -= bpp;
		}
		sshiftl -= bpp;
		if (sshiftl<0) {
		    sshiftl = 8-bpp;
		}
		if (dshiftl<0) {
		    dshiftl = 8-bpp;
		}
	    }
	}
	free(img);
	img = img2;
    } else {
    byte *linebuf;
	if (r->depth < 8)
	    r->maxpval = (1<<r->depth) - 1;
	/* Turn image upside-down for convert_img_for_screen. */
	linebuf = malloc(bytesperline);
	for (i=0;i<r->height/2;i++) {
	    memcpy(linebuf,img+i*bytesperline,bytesperline);
	    memcpy(img+i*bytesperline,img+(r->height-i)*bytesperline,
		   bytesperline);
	    memcpy(img+(r->height-i-1)*bytesperline,linebuf,bytesperline);
	}
	free(linebuf);
    }
    if (!convert_img_for_screen(r,img,trancolor,err_msg))
	return FALSE;
    free(img);
    setup_preg_for_display(r);
    pic_free(buf);
    my_fclose(fp);

    return TRUE;

}
