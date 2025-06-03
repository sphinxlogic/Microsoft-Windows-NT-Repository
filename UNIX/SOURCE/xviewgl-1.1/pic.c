#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_types.h"
#include "libutils.h"
#include "pic.h"
#include "registers.h"
#include "utils.h"
#include "windows.h"

int pic_read(byte *buf, int n, MY_FILE *fp)
{
int r;
    r = my_read(fp, buf, n);
    if (r<n)
	return 0;
    else
	return r;
}

static int pic_bufsiz=0;

void *pic_alloc(int sz)
{
    pic_bufsiz = sz;
    return malloc(sz);
}

void pic_free(byte *b)
{
    free(b);
    pic_bufsiz = 0;
}

int pic_blread(byte **b, MY_FILE *fp, byte *esc)
{
byte buf[5];
int len;

    if (!pic_read(buf, 5, fp))
	return -1;

    len = buf[0] + buf[1] * 256 - 5;
    /* size of buffer needed to store next block of pixels? */
    /* bufsize = buf[2] + buf[3] * 256; */
    *esc = buf[4];

    if ((*b==NULL) || (len>pic_bufsiz)) {
	*b=realloc(*b,len);
    }
    if (!pic_read(*b, len, fp))
	return -1;

    return len;

}

BOOLEAN pic_load(char *fname, int reg, BOOLEAN load_cmap, int trancolor, char *err_msg)
{
MY_FILE *fp;
char fullname[FILENAME_MAX],vtype;
byte *buf=pic_alloc(32), *p, *clist, esc;
struct PIC_register *r = load_cmap?&preg[reg]:&creg[reg];
byte *img;
int bytesleft, blocksleft;
int blen, i, c, l, planes, bpp,magic,edesc,elen;
unsigned cscalemax=MainWindow.cscalemax;

    clear_preg(r);

    if (load_cmap)
	extend(strcpy(fullname,fname),".pic");
    else
	extend(strcpy(fullname,fname),".clp");

    if ((fp=find_file(fullname)) == NULL) {
	sprintf(err_msg,"Can't open pic file %s: %s",
		fname,strerror(errno));
	return FALSE;
    }

    if (!pic_read(buf, 2, fp)) {
	sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
	return FALSE;
    }

    magic = buf[0] + buf[1]*256;

    if (magic != 0x1234) {

	/* The following is just a heuristic guess...  I don't know
	   the real format... */
	if (!pic_read(buf, 4, fp)) {
	    sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
	    return FALSE;
	}
	r->width = buf[0] + buf[1] * 256;
	r->height = buf[2] + buf[3] * 256;
	bytesleft = magic-6;
	bpp = bytesleft/(((r->width+7)/8)*r->height);
	if (bytesleft%(((r->width+7)/8)*r->height))
	    bpp++;
	planes = 1;
	edesc = 1;

    } else {

	if (!pic_read(buf, 15, fp)) {
	    sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
	    return FALSE;
	}
	vtype = my_tolower(buf[10]);
	for (i=0;i<VIDTYPES;i++) {
	    if (vtype == video_modes[i].key) {
		cscalemax=video_modes[i].cmax;
		if (MainWindow.theWindow == MainWindow.RootW) {
		    start_window(&video_modes[i],vtype,err_msg);
		}
		break;
	    }
	}
	if (i > VIDTYPES) {
	    fprintf(stderr, "Warning: unsupported image type '%c' in %s.\n",
		    vtype,fname);
	    fprintf(stderr,"Trying to interpret it anyway...\n");
	}
	    
	r->width = buf[0] + buf[1] * 256;
	r->height = buf[2] + buf[3] * 256;
	/* 6-7 & 8-9 are x & y offset of lower left corner. */
	bpp = buf[8]&0xf;
	planes = ((buf[8]&0xf0)>>4)+1;
	/* Byte 9 is always 0xff */
	edesc = buf[11];
	elen = buf[13] + buf[14]*256;
	bytesleft = ((r->width*bpp+7)/8)*r->height*planes;

    }

    r->depth = planes*bpp;

    switch (edesc) {
	case 0:
	    break;
	case 1:	/* palette number + Border color (2 bytes) */
	    if (!pic_read(buf, 2, fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    clist = malloc(4*3);
	    clist[0] = CGA_color[buf[1]].red>>8;
	    clist[1] = CGA_color[buf[1]].green>>8;
	    clist[2] = CGA_color[buf[1]].blue>>8;
	    for (i=0;i<3;i++) {
		clist[3*(i+1)] = CGA_color[CGA_palette[buf[0]][i]].red>>8;
		clist[3*(i+1)+1] = CGA_color[CGA_palette[buf[0]][i]].green>>8;
		clist[3*(i+1)+2] = CGA_color[CGA_palette[buf[0]][i]].blue>>8;
	    }
	    if (load_cmap) {
		r->cmap = convert_clist_to_cmap(clist,reg,cscalemax);
	    } else {
		r->hascmap=FALSE;
	    }
	    free(clist);
	    break;
	case 2:	/* PCjr colors.  16 1 byte color numbers */
	    if (!pic_read(buf, 16, fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    clist = malloc(16*3);
	    for (i=0;i<16;i++) {
		clist[3*i] = CGA_color[buf[i]].red>>8;
		clist[3*i+1] = CGA_color[buf[i]].green>>8;
		clist[3*i+2] = CGA_color[buf[i]].blue>>8;
	    }
	    if (load_cmap) {
		r->cmap = convert_clist_to_cmap(clist,reg,cscalemax);
	    } else {
		r->hascmap=FALSE;
	    }
	    free(clist);
	    break;
	case 3:	/* EGA 16/64.  16 numbers in range 0-63. */
	    if (!pic_read(buf, 16, fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    clist = malloc(16*3);
	    for (i=0;i<16;i++) {
		clist[3*i] = EGA_color[buf[i]].red>>8;
		clist[3*i+1] = EGA_color[buf[i]].green>>8;
		clist[3*i+2] = EGA_color[buf[i]].blue>>8;
	    }
	    if (load_cmap) {
		r->cmap = convert_clist_to_cmap(clist,reg,cscalemax);
	    } else {
		r->hascmap=FALSE;
	    }
	    free(clist);
	    break;
	case 5:	/* Super EGA color map */
	case 4:	/* VGA color map */
	    r->hascmap = TRUE;

	    /* Read cmap */
	    if ((clist=malloc(3*(1<<r->depth))) == NULL) {
		sprintf(err_msg, "Error allocating cmap for pic file %s: %s",
			fname, strerror(errno));
		return FALSE;
	    }
	    if (!pic_read(clist, 3*(1<<r->depth), fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    if (load_cmap) {
		r->cmap = convert_clist_to_cmap(clist,reg,cscalemax);
	    } else {
		r->hascmap=FALSE;
	    }
	    free(clist);
	    break;
	default:
	    fprintf(stderr,
		    "Unrecognized extended info type %d in %s.  Skipping %d bytes...\n",
		    edesc, fname, elen);
	    if (!elen)
		break;
	    clist = malloc(elen);
	    if (!pic_read(buf, elen, fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    free(clist);
    }

    if ((p = img = malloc(bytesleft)) == NULL) {
	sprintf(err_msg, "Error allocating memory for pic file %s: %s",
		fname, strerror(errno));
	return FALSE;
    }

    if (magic != 0x1234) {

	/* Data is not run-length encoded */
	if (!pic_read(p,bytesleft,fp)) {
	    sprintf(err_msg,"Unexpected EOF reading BSAVE pic file %s",fname);
	    return FALSE;
	}

    } else {

	r->maxpval=0;

	if (!pic_read(buf, 2, fp)) {
	    sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
	    return FALSE;
	}
	blocksleft = buf[0] + buf[1] * 256;

	if (blocksleft == 0) {

	    /* Data is not run-length encoded */
	    if (!pic_read(p,bytesleft,fp)) {
		sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
		return FALSE;
	    }
	    bytesleft = 0;

	} else {

	    while ((bytesleft > 0) && (blocksleft-- > 0) &&
		   ((blen=pic_blread(&buf, fp, &esc)) >= 0)) {
		for (i=0; (bytesleft > 0) && (i<blen);) {
		    c = buf[i++];
		    if (c == esc) {
			/* This code assumes that an escaped character will
			   never cross a block boundary. */
			l = buf[i++];
			if (l == 0) {
			    l = buf[i+1] * 256 + buf[i];
			    c = buf[i+2];
			    i += 3;
			}
			else {
			    c = buf[i++];
			}
		    } else {
			l = 1;
		    }
		    if (c > r->maxpval)
			r->maxpval=c;
		    memset(p,c,l);
		    p += l;
		    bytesleft -= l;
		    if (bytesleft < 0) {
			fprintf(stderr,"File %s too long.  Ignoring extra.\n",
				fname);
		    }
		}
	    }
	}

	if (bytesleft > 0) {
	    sprintf(err_msg,"Unexpected EOF reading pic file %s",fname);
	    return FALSE;
	}

    }
    if (planes>1) {
    int planesize,srclinesize,dstlinesize,mask,pmask,sshiftl,tshiftl,dshiftl,j;
    byte *p2,*lastplane,*img2;
	r->maxpval = (1<<r->depth) - 1;
	srclinesize = (r->width*bpp+7)/8;
	planesize = r->height * srclinesize;
	lastplane = img + (planes-1)*planesize;
	dstlinesize = (r->width*r->depth+7)/8;
	pmask = (1<<bpp) - 1;
	if ((img2 = calloc(r->height * dstlinesize,1))
	    == NULL) {
	    sprintf(err_msg, "Error allocating memory while decoding pic file %s: %s",
		    fname, strerror(errno));
	    return FALSE;
	}
	for (i=0;i<r->height;i++) {
	    for (dshiftl=sshiftl=8-bpp,j=0;j<r->width;j++) {
		mask = pmask << sshiftl;
		p = lastplane + i*srclinesize + j*bpp/8;
		p2 = img2 + i*dstlinesize + j*r->depth/8;
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
	if (r->depth < 8)
	    r->maxpval = (1<<r->depth) - 1;
    }
    if (!convert_img_for_screen(r,img,trancolor,err_msg))
	return FALSE;
    free(img);
    setup_preg_for_display(r);
    pic_free(buf);
    my_fclose(fp);

    return TRUE;

}
