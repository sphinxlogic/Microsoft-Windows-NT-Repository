#include "patchlevel.h"
/* Clip and PIC reader code hacked from George Phillips' original code. */
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <errno.h>
#include <math.h>
#include <stdio.h>
#ifdef STDLIB
#include <stdlib.h>
#endif
#ifdef UNISTD
#include <unistd.h>
#endif
#ifdef WAIT_H
#include <sys/wait.h>
#endif
#include <ctype.h>
#include "my_types.h"
#include "actions.h"
#include "gif.h"
#include "libutils.h"
#include "my_stdarg.h"
#include "options.h"
#include "parser.h"
#include "pic.h"
#include "pcx.h"
#include "registers.h"
#include "timer.h"
#include "utils.h"
#include "video.h"
#include "windows.h"

struct PIC_register preg[MAXPICREG+1];
struct PIC_register creg[MAXCLPREG+1];
static BOOLEAN load_image(char *fname, int reg, BOOLEAN load_cmap,
			  int trancolor, char *err_msg);

static unsigned int clipx1,clipx2,clipy1,clipy2;

BOOLEAN cmd_box(int argc, my_va_list ap, char *err_msg) /* Draw a box */
{
int x1 = my_va_arg_i(ap);
int y1 = my_va_arg_i(ap);
int x2 = my_va_arg_i(ap);
int y2 = my_va_arg_i(ap);
int width = (argc>4)?my_va_arg_i(ap):1;

    drawbox(x1,y1,x2,y2,width);
    return TRUE;

}

BOOLEAN cmd_break(int argc, my_va_list ap, char *err_msg)	/* Break out of immediately enclosing loop. */
{
    sprintf(err_msg,"Action break not yet implemented");
    return FALSE;
}

BOOLEAN cmd_call(int argc, my_va_list ap, char *err_msg)	/* Run another text file and return */
{
char *prog_name = my_va_arg_c(ap);
char *label = (argc>1)?my_va_arg_c(ap):"";

struct prog_info_rec save_prog;
char fname[FILENAME_MAX],cmd[132];
struct prog_loc dest;

    if (!save_prog_info(&save_prog,err_msg))
	return FALSE;
    extend(strcpy(fname,prog_name),".txt");
    program_fp = reopen_prog(fname,"r");
    if (program_fp==NULL) {
	sprintf(err_msg,"Couldn't call file %s: %s\n",fname,strerror(errno));
	restore_prog_info(&save_prog,err_msg);
	return FALSE;
    }
    strcpy(program_fname,fname);
    exec_line("",err_msg); /* Initialize lastloc */
    if (*label) {
	if (!find_label_location(label,&dest,err_msg))
	    return FALSE;
	if (!set_prog_loc(&dest)) {
	    sprintf(err_msg,"File positioning failed on goto: %s",
		    strerror(errno));
	    restore_prog_info(&save_prog,err_msg);
	    return FALSE;
	}
    }
    while (my_fgets(cmd,132,program_fp) != NULL) {
#ifndef HAS_ASYNC_INPUT
	evt_handler(0);
#endif
	if (!exec_line(cmd,err_msg)) {
	    if (!strcmp(err_msg,"Done"))
		break;
	    return FALSE;
	}
    }
    return restore_prog_info(&save_prog,err_msg);
}

BOOLEAN cmd_cfade(int argc, my_va_list ap, char *err_msg) /* Fade in image */
{
int effect = my_va_arg_i(ap);
int x = my_va_arg_i(ap);
int y = my_va_arg_i(ap);
int reg = (argc>3)?my_va_arg_i(ap):1;
int speed = (argc>4)?my_va_arg_i(ap):0;
int delay = (argc>5)?my_va_arg_i(ap):0;

struct PIC_register *r = &creg[reg];

    if (reg>=MAXCLPREG) {
	sprintf(err_msg,"Clip register %d out of range!",reg);
	return FALSE;
    }

    if (reg<0) reg=1;
    if (speed<0) speed=1;

    if (!setup_creg_for_display(r))
	return FALSE;
#ifdef CLIP_ALL_BUT_CFADE
    if (!set_clipping_region(-1,0,0,0,err_msg))
	return FALSE;
#endif
    if (!put_creg(r,x,y,effect,speed,TO_DRAWING,err_msg))
	return FALSE;
#ifdef CLIP_ALL_BUT_CFADE
    if (!set_clipping_region(clipx1,clipy1,clipx2,clipy2,err_msg))
	return FALSE;
#endif
    sit_for(delay);
    return TRUE;

}

BOOLEAN cmd_cfree(int argc, my_va_list ap, char *err_msg) /* Clear clip reg. */
{
int reg;

    while (argc--) {
	reg = my_va_arg_i(ap);
	if (reg>MAXCLPREG) {
	    sprintf(err_msg,"Clip register %d out of range!",reg);
	    return FALSE;
	}
	clear_creg(&creg[reg]);
    }
    return TRUE;

}

BOOLEAN cmd_cgetbuf(int argc, my_va_list ap, char *err_msg)	/* Get clip register from screen */
{
int reg = my_va_arg_i(ap);
int x1 = (argc<2)?-1:my_va_arg_i(ap);
int y1 = (argc<3)?-1:my_va_arg_i(ap);
int x2 = (argc<4)?-1:my_va_arg_i(ap);
int y2 = (argc<5)?-1:my_va_arg_i(ap);

    return get_reg(&preg[reg],x1,y1,x2,y2,FALSE,err_msg);

}

BOOLEAN cmd_chgcolor(int argc, my_va_list ap, char *err_msg) /* Change EGA cmap */
{
int cindex;
int cval;

    while (argc>=2) {
	cindex = my_va_arg_i(ap);
	cval = my_va_arg_i(ap);
	set_current_cmap_entry(cindex,
	    EGA_color[cval].red, EGA_color[cval].green, EGA_color[cval].blue);
	argc -= 2;
    }
    return TRUE;

}

BOOLEAN cmd_closegl(int argc, my_va_list ap, char *err_msg)	/* Close active graphics library */
{
    sprintf(err_msg,"Action closegl not yet implemented");
    return FALSE;
}

BOOLEAN cmd_circle(int argc, my_va_list ap, char *err_msg) /* Draw an ellipse */
{
int cx = my_va_arg_i(ap);
int cy = my_va_arg_i(ap);
int rx = my_va_arg_i(ap);
int ry = (argc>3)?my_va_arg_i(ap):rx;

    drawellipse(cx,cy,rx,ry);
    return TRUE;

}

BOOLEAN cmd_clearscr(int argc, my_va_list ap, char *err_msg) /* Clear the display */
{

    return put_creg(NULL,0,0,EFFECT_NONE,0,TO_WINDOW,err_msg);

}

BOOLEAN cmd_cload(int argc, my_va_list ap, char *err_msg) /* Load clip reg */
{
char *clip_fname = my_va_arg_c(ap);
int reg = (argc<2)?1:my_va_arg_i(ap);
int shiftno = (argc<3)?0:my_va_arg_i(ap);	/* Don't know what it does */
int trancolor = (argc<4)?-1:my_va_arg_i(ap);	/* Transparent color */

    /* I suspect there can be multiple transparent colors, but I haven't seen
       them... */
    return load_image(clip_fname, reg, FALSE, trancolor, err_msg);

}

BOOLEAN cmd_color(int argc, my_va_list ap, char *err_msg) /* Set primary/2ndary colors */
{
int color = my_va_arg_i(ap);

    set_current_drawcolor(color);
    if (argc>1)
	set_current_drawcolor2(my_va_arg_i(ap));
    return TRUE;

}

BOOLEAN cmd_cycle(int argc, my_va_list ap, char *err_msg) /* Cycle through colors */
{
int times = my_va_arg_i(ap);
int start = (argc>1)?my_va_arg_i(ap):0;
int count = (argc>2)?my_va_arg_i(ap):1;
int delay = (argc>3)?my_va_arg_i(ap):0;

    if (times < 0)
	times=1;
    cycle_colors(start,start+count,times,delay);
    return TRUE;
}

struct prog_loc datapos;
static BOOLEAN data_available = FALSE;
static BOOLEAN look_for_data = TRUE;
static BOOLEAN read_by_line = FALSE;

BOOLEAN cmd_data(int argc, my_va_list ap, char *err_msg)	/* provide immediate data */
{
    /* Do nothing */
    return TRUE;
}

BOOLEAN cmd_databegin(int argc, my_va_list ap, char *err_msg) /* Start reading from labelled data. */
{
struct prog_loc oldpos;
char *label = my_va_arg_c(ap);
int c;

    if (!get_prog_loc(&oldpos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    if (!find_label_location(label,&datapos,err_msg))
	return FALSE;
    if (!set_prog_loc(&datapos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    /* Skip to end of label */
    while (((c=my_getc(program_fp)) >= 0) && (c!=':'));
    if (!get_prog_loc(&datapos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    if (!set_prog_loc(&oldpos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    data_available = TRUE;
    read_by_line = FALSE;
    return TRUE;
}

BOOLEAN cmd_dataend(int argc, my_va_list ap, char *err_msg) /* End of labelled dat.  Should never be executed. */
{

    sprintf(err_msg,"Attempted to execute data");
    return FALSE;

}

BOOLEAN cmd_dataskip(int argc, my_va_list ap, char *err_msg)	/* Skip n elements of data */
{
int i = (argc>0)?my_va_arg_i(ap):1;
    while (i--) {
	if (read_data(err_msg) == NULL)
	    return FALSE;
    }
    return TRUE;
}

char *read_data(char *err_msg) /* Get next element from data list */
{
struct prog_loc oldpos;
char s[MAXDATAOBJ],*p;
int c;

    if (!get_prog_loc(&oldpos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return NULL;
    }
    if (!data_available && look_for_data) {
	rewind_program();
	if (!find_data(&datapos)) {
	    sprintf(err_msg,"Read past end of data");
	    return NULL;
	}
	read_by_line = TRUE;
	data_available = TRUE;
    }
    if (!set_prog_loc(&datapos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return NULL;
    }
    p = s;
    while (((c=my_getc(program_fp)) >= 0) && isspace(c) &&
	   !(read_by_line && ((c == '\r') || (c == '\n'))));
   if (read_by_line && ((c == '\r') || (c == '\n'))) {
	if (!find_data(&datapos)) {
	    sprintf(err_msg,"Read past end of data");
	    return NULL;
	}
   }
    do {
	*p++=c;
	if (c=='"') {
	    while (((c=my_getc(program_fp)) >= 0) && (c != '"'))
		*p++=c;
	    break;
	}
    } while (((c=my_getc(program_fp)) >= 0) && (c != ',') && !isspace(c));
    *p=0;
    while ((c != ',') && !(read_by_line && ((c == '\r') || (c == '\n'))) &&
	   ((c=my_getc(program_fp)) >= 0) && isspace(c));
    if (c != ',')
	my_ungetc(c,program_fp);
    if ((c<0) || !strcmp(preprocess_line(s),"dataend")) {
	sprintf(err_msg,"Read past end of data");
	return NULL;
    }
    if (!get_prog_loc(&datapos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return NULL;
    }
    if (!set_prog_loc(&oldpos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return NULL;
    }
    return s;

}

BOOLEAN cmd_dfree(int argc, my_va_list ap, char *err_msg)	/* Free difference buffer */
{
    sprintf(err_msg,"Action dfree not yet implemented");
    return FALSE;
}

BOOLEAN cmd_dload(int argc, my_va_list ap, char *err_msg)	/* load difference buffer */
{
    sprintf(err_msg,"Action dload not yet implemented");
    return FALSE;
}

BOOLEAN cmd_edge(int argc, my_va_list ap, char *err_msg)
{
char *state = my_va_arg_c(ap);
int i;

    if ((state[0]=='0') || (state[1]=='f')) {
	edge_mode = FALSE;
    } else {
	edge_mode = TRUE;
	set_edge_color(my_va_arg_i(ap));
    }
    return TRUE;

}

BOOLEAN cmd_else(int argc, my_va_list ap, char *err_msg)	/* else (in if-else-endif) */
{
    sprintf(err_msg,"Action else not yet implemented");
    return FALSE;
}

BOOLEAN cmd_endif(int argc, my_va_list ap, char *err_msg)	/* endif (in if-else-endif) */
{
    sprintf(err_msg,"Action endif not yet implemented");
    return FALSE;
}

BOOLEAN cmd_exec(int argc, my_va_list ap, char *err_msg) /* Execute command */
{
char *cmd_name = my_va_arg_c(ap);

int dummy;

    fprintf(stderr,"Exec of %s ignored.\n",cmd_name);
    return TRUE;

}

BOOLEAN cmd_exit(int argc, my_va_list ap, char *err_msg) /* Finish processing */
{

    sprintf(err_msg,"Done");
    return FALSE;

}

BOOLEAN cmd_ffree(int argc, my_va_list ap, char *err_msg) /* Free font space */
{
int i = (argc>0)?my_va_arg_i(ap):1;

    dispose_font(&fonts[i-1]);
    return TRUE;

}

BOOLEAN cmd_fgaps(int argc, my_va_list ap, char *err_msg) /* Set font spacing */
{

    if (argc==0) {
	curfont->cgap=curfont->def_cgap;	/* Gap btw characters */
	curfont->sgap=curfont->def_sgap;	/* Width of space */
    } else {
	curfont->cgap=my_va_arg_i(ap);	/* Gap btw characters */
	curfont->sgap=my_va_arg_i(ap);	/* Width of space */
	curfont->cgap = screen_x(curfont->cgap);
	curfont->sgap = screen_x(curfont->sgap);
    }
    return TRUE;

}

BOOLEAN cmd_fload(int argc, my_va_list ap, char *err_msg) /* Load a font */
{
char *fnt_fname = my_va_arg_c(ap);
int i = (argc>1)?my_va_arg_i(ap):1;

MY_FILE *fp;
byte buf[32], *p;
int charcnt;
unsigned int sz;
char fname[FILENAME_MAX];
MyFont *font = &fonts[i-1];

    extend(strcpy(fname,fnt_fname),".set");
    fp = find_file(fname);
    if (fp==NULL) {
	sprintf(err_msg,"Couldn't open font file %s: %s",
		fname,strerror(errno));
	return FALSE;
    }

    if (!pic_read(buf, 7, fp)) {
	sprintf(err_msg,"Unexpected EOF reading font file %s",fname);
	return FALSE;
    }

    sz = buf[0] + buf[1] * 256 - 7;

    if (sz != (buf[6]*buf[2])) {
	fprintf(stderr,"Warning: Font %s garbled:\n",fname);
	fprintf(stderr,"Font is %dx%d, containing chars from ASCII %d to %d\n",
		buf[4],buf[5],buf[3],buf[3]+buf[2]);
	fprintf(stderr,"Each character is specified as taking %d bytes\n",
		buf[6]);
	if (buf[6] != ((buf[4]+7)>>3)*buf[5])
	    fprintf(stderr,"but they should take %d bytes\n",
		    ((buf[4]+7)>>3)*buf[5]);
	fprintf(stderr,"Size given is %d bytes, but should be %d bytes\n",
		sz,buf[2]*buf[6]);
	fprintf(stderr,"Ignoring font request\n");
	return TRUE;
    }

    dispose_font(font);
    font->style = 0;
    font->charcnt = buf[2];
    font->firstch = buf[3];

    font->width = buf[4];
    font->height = buf[5];
    font->bytes_per_char = buf[6];

    if ((p=malloc(sz)) == NULL) {
	sprintf(err_msg,"Couldn't allocate memory for font %s: %s",
		fname,strerror(errno));
	return FALSE;
    }

    if (!pic_read(p, sz, fp)) {
	fprintf(stderr,"Warning: Short font file %s\n",fname);
    }

    if (!create_font(font,p,err_msg))
	return FALSE;;

    free(p);
    my_fclose(fp);

    return TRUE;

}

BOOLEAN cmd_float(int argc, my_va_list ap, char *err_msg) /* Fly preserving bg */
{
int x1 = my_va_arg_i(ap);
int y1 = my_va_arg_i(ap);
int x2 = my_va_arg_i(ap);
int y2 = my_va_arg_i(ap);
int step = (argc<5)?1:my_va_arg_i(ap);
int delay = (argc<6)?0:my_va_arg_i(ap);

int nregs=argc-6,regs[MAXCLPREG];
int x,y,i,k,l,xdir,ydir,lastx,lasty,lastw,lasth;
struct PIC_register *lastr;
double yslope,xslope,h;

    h = hypot(1.0*(x2-x1),1.0*(y2-y1));
    if (h<0.5) {
	xslope = 0;
	yslope = 0;
    } else {
	yslope=(y2-y1)*1.0/h;
	xslope=(x2-x1)*1.0/h;
    }
    for (i=0;i<nregs;i++) {
	regs[i]=my_va_arg_i(ap);
	if (!setup_creg_for_display(&creg[regs[i]]))
	    return FALSE;
    }
    l = k = i = 0;
    xdir=(x2>x1)?1:-1;
    ydir=(y2>y1)?1:-1;
    if (!save_bg(err_msg)) {
	return FALSE;
    }
    if (step < 0) step = -step;
    do {
	x = x1 + l * xslope;
	y = y1 + l * yslope;
	if (((x > x2) && (xdir>0)) || ((x < x2) && (xdir < 0)))
	    x = x2;
	if (((y > y2) && (ydir>0)) || ((y < y2) && (ydir < 0)))
	    y = y2;
	if (k != 0)
	    restore_bg(lastx,lasty,lastr,x,y,&creg[regs[i]]);
	if (!put_transient(&creg[regs[i]],x,y,EFFECT_NONE,0,TO_WINDOW,err_msg))
	    return FALSE;
	sit_for(delay);
	k++;
	lastx = x;
	lasty = y;
	lastw = creg[regs[i]].width;
	lasth = creg[regs[i]].height;
	lastr = &creg[regs[i]];
	if ((x==x2) && (y==y2) && (k>=nregs)) break;
	l += step;
	i = (i + 1) % nregs;
    } while (1);
    restore_bg(lastx,lasty,lastr,lastx+lastw,lasty+lasth,NULL);
    /* Some .GLs seem to want FLOAT to leave the image up.  Others don't.
       My favorites don't, so I've commented this out...
    if (!put_creg(&creg[regs[i]],x1+(xdir*x),y1+(ydir*y),EFFECT_NONE,0,TO_WINDOW,err_msg))
	return FALSE;
    */
    return TRUE;

}

BOOLEAN cmd_fly(int argc, my_va_list ap, char *err_msg) /* Move pic, trash bg */
{
int x1 = my_va_arg_i(ap);
int y1 = my_va_arg_i(ap);
int x2 = my_va_arg_i(ap);
int y2 = my_va_arg_i(ap);
int step = (argc<5)?1:my_va_arg_i(ap);
int delay = (argc<6)?0:my_va_arg_i(ap);

int nregs=argc-6,regs[MAXCLPREG];
int x,y,i,xdir,ydir,l,k;
double yslope,xslope,h;

    h = hypot(1.0*(x2-x1),1.0*(y2-y1));
    if (h<0.5) {
	xslope = 0;
	yslope = 0;
    } else {
	yslope=(y2-y1)*1.0/h;
	xslope=(x2-x1)*1.0/h;
    }
    for (i=0;i<nregs;i++) {
	regs[i]=my_va_arg_i(ap);
	if (!setup_creg_for_display(&creg[regs[i]]))
	    return FALSE;
    }
    l = k = i = 0;
    xdir=(x2>x1)?1:-1;
    ydir=(y2>y1)?1:-1;
    if (step < 0) step = -step;
    do {
	x = x1 + l * xslope;
	y = y1 + l * yslope;
	if (((x > x2) && (xdir>0)) || ((x < x2) && (xdir < 0)))
	    x = x2;
	if (((y > y2) && (ydir>0)) || ((y < y2) && (ydir < 0)))
	    y = y2;
	if (!put_creg(&creg[regs[i]],x,y,EFFECT_NONE,0,TO_WINDOW,err_msg))
	    return FALSE;
	sit_for(delay);
	k++;
	if ((x==x2) && (y==y2) && (k>=nregs)) break;
	l += step;
	i = (i + 1) % nregs;
    } while (1);
    return TRUE;

}

BOOLEAN cmd_font(int argc, my_va_list ap, char *err_msg)
{
int i = (argc>1)?my_va_arg_i(ap):1;
    curfont = &fonts[i-1];
    return TRUE;
}

BOOLEAN cmd_fstyle(int argc, my_va_list ap, char *err_msg)
{

    curfont->style = my_va_arg_i(ap);
    return TRUE;

}

typedef struct subr_stack {
    struct prog_loc pos;
    struct subr_stack *next;
} SUBR_STACK;
SUBR_STACK *retstack;

push_subr(char *err_msg)
{
struct prog_loc pos;
SUBR_STACK *tmp = malloc(sizeof(*tmp));

    if (tmp == NULL) {
	sprintf(err_msg,"Error allocating space on return stack: %s",
		strerror(errno));
	return FALSE;
    }

    if (!get_prog_loc(&pos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    tmp->pos = pos;
    tmp->next= retstack;
    retstack = tmp;
    return TRUE;

}

BOOLEAN pop_subr(char *err_msg)
{
SUBR_STACK *tmp=retstack;

    if (retstack == NULL) {
	sprintf(err_msg,"Return stack underflow");
	return FALSE;
    }
    if (!set_prog_loc(&retstack->pos)) {
	sprintf(err_msg,"File positioning failed on gosub: %s",
		strerror(errno));
	return FALSE;
    }
    retstack = retstack->next;
    free(tmp);
    return TRUE;

}

BOOLEAN cmd_getcolor(int argc, my_va_list ap, char *err_msg)
{
    fprintf(stderr,"Action getcolor not yet implemented, ignoring\n");
    return TRUE;
}

BOOLEAN cmd_gosub(int argc, my_va_list ap, char *err_msg)
{
char *label = my_va_arg_c(ap);

struct prog_loc dest;

    if (!push_subr(err_msg))
	return FALSE;
    if (!find_label_location(label,&dest,err_msg))
	return FALSE;
    if (!set_prog_loc(&dest)) {
	sprintf(err_msg,"File positioning failed on gosub: %s",
		strerror(errno));
	return FALSE;
    }
    return TRUE;

}

BOOLEAN cmd_goto(int argc, my_va_list ap, char *err_msg)
{
char *label = my_va_arg_c(ap);

struct prog_loc dest;

    if (!find_label_location(label,&dest,err_msg))
	return FALSE;
    if (!set_prog_loc(&dest)) {
	sprintf(err_msg,"File positioning failed on goto: %s",
		strerror(errno));
	return FALSE;
    }
    return TRUE;

}

BOOLEAN cmd_if(int argc, my_va_list ap, char *err_msg)	/* start of if-else-endif */
{
    sprintf(err_msg,"Action if not yet implemented");
    return FALSE;
}

int lastkey;	/* Filled in by waitkey */

BOOLEAN cmd_ifkey(int argc, my_va_list ap, char *err_msg)
{
int key = *my_va_arg_c(ap);

    if (key == lastkey)
	return cmd_goto(argc-1,ap,err_msg);
    return TRUE;

}

BOOLEAN cmd_ifmem(int argc, my_va_list ap, char *err_msg)
{
int mem=my_va_arg_i(ap);
    /* Assume that we can support the amount of memory requested */
    return cmd_goto(argc-1,ap,err_msg);
}

BOOLEAN cmd_ifmouse(int argc, my_va_list ap, char *err_msg)	/* */
{
    sprintf(err_msg,"Action ifmmouse not yet implemented");
    return FALSE;
}

BOOLEAN cmd_ifvideo(int argc, my_va_list ap, char *err_msg)
{
char *vidtype=my_va_arg_c(ap);
    /* For now, assume that we can support the video type requested */
    return cmd_goto(argc-1,ap,err_msg);
}

BOOLEAN cmd_int(int argc, my_va_list ap, char *err_msg)	/* IBM PC interrupt */
{
    sprintf(err_msg,"Action int not yet implemented");
    return FALSE;
}

BOOLEAN cmd_line(int argc, my_va_list ap, char *err_msg) /* Draw line */
{
int x1 = my_va_arg_i(ap);
int y1 = my_va_arg_i(ap);
int x2 = my_va_arg_i(ap);
int y2 = my_va_arg_i(ap);

    drawline(x1,y1,x2,y2);
    return TRUE;

}

typedef struct loop_stack {
    struct prog_loc pos;
    int counter;
    struct loop_stack *next;
} LOOP_STACK;
LOOP_STACK *loopstack;

static BOOLEAN pop_loop(char *err_msg)
{
LOOP_STACK *tmp=loopstack;

    if (loopstack == NULL) {
	sprintf(err_msg,"Loop stack underflow");
	return FALSE;
    }
    loopstack = loopstack->next;
    free(tmp);
    return TRUE;

}

static BOOLEAN push_loop(int counter, struct prog_loc *pos, char *err_msg)
{
LOOP_STACK *tmp=malloc(sizeof(*tmp));

    if (tmp==NULL) {
	sprintf(err_msg,"Couldn't expand loop stack: %s",strerror(errno));
	return FALSE;
    }
    tmp->pos = *pos;
    tmp->counter = counter;
    tmp->next = loopstack;
    loopstack = tmp;
    return TRUE;

}

BOOLEAN cmd_link(int argc, my_va_list ap, char *err_msg) /* Link to new file */
{
char *link_fname = my_va_arg_c(ap);

char fname[FILENAME_MAX];

    extend(strcpy(fname,link_fname),".clp");
    while (pop_loop(err_msg));
    while (pop_subr(err_msg));
    program_fp = reopen_prog(fname,"r");
    if (program_fp == NULL) {
	sprintf(err_msg,"Link to file %s failed: %s",fname,strerror(errno));
	return FALSE;
    }
    strcpy(program_fname,fname);
    return TRUE;

}

BOOLEAN cmd_local(int argc, my_va_list ap, char *err_msg)	/* Create local variable */
{
    sprintf(err_msg,"Action local not yet implemented");
    return FALSE;
}

BOOLEAN cmd_loop(int argc, my_va_list ap, char *err_msg) /* Loop to last mark */
{

    if (loopstack==NULL) {
	sprintf(err_msg,"loop stack empty");
	return FALSE;
    }
    if (--loopstack->counter <= 0) {
	return pop_loop(err_msg);
    }
    if (!set_prog_loc(&loopstack->pos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    return TRUE;

}

BOOLEAN cmd_mark(int argc, my_va_list ap, char *err_msg) /* Mark loop start */
{
int n = my_va_arg_i(ap);

struct prog_loc pos;

    if (!get_prog_loc(&pos)) {
	sprintf(err_msg,"File positioning failed on data read: %s",
		strerror(errno));
	return FALSE;
    }
    return push_loop(n,&pos,err_msg);

}

BOOLEAN cmd_merge(int argc, my_va_list ap, char *err_msg)	/* Merge lines into current source (??) */
{
    sprintf(err_msg,"Action merge not yet implemented");
    return FALSE;
}

BOOLEAN cmd_mode(int argc, my_va_list ap, char *err_msg)
{
int bgcolor = my_va_arg_i(ap);
int palette = (argc>1)?my_va_arg_i(ap):-1;

int i;

    set_current_cmap_entry((MainWindow.maxcolor==2)?1:0,
	    CGA_color[bgcolor].red, CGA_color[bgcolor].green,
	    CGA_color[bgcolor].blue);
    if (palette == -1)
	return TRUE;
    for (i=0;i<3;i++)
	set_current_cmap_entry(i+1,
		CGA_color[CGA_palette[palette][i]].red,
		CGA_color[CGA_palette[palette][i]].green,
		CGA_color[CGA_palette[palette][i]].blue);

    return TRUE;

}

BOOLEAN cmd_mouse(int argc, my_va_list ap, char *err_msg)	/* */
{
    sprintf(err_msg,"Action mouse not yet implemented");
    return FALSE;
}

BOOLEAN cmd_move(int argc, my_va_list ap, char *err_msg)	/* Copy portion of screen to another part of screen */
{
    sprintf(err_msg,"Action move not yet implemented");
    return FALSE;
}

BOOLEAN cmd_noise(int argc, my_va_list ap, char *err_msg)
{
int sfreq = my_va_arg_i(ap);
int efreq = my_va_arg_i(ap);
int duration = my_va_arg_i(ap);

    beep();	/* A beep is better than nothing... */
    return TRUE;

}

BOOLEAN cmd_note(int argc, my_va_list ap, char *err_msg)
{
int freq = my_va_arg_i(ap);
int delay = my_va_arg_i(ap);
int duration = my_va_arg_i(ap);

static int beenhere=FALSE;

    if (!beenhere)
	printf("\007You hear music...\n");
    sit_for(delay);
    return beenhere=TRUE;

}

BOOLEAN cmd_offset(int argc, my_va_list ap, char *err_msg)
{

    MainWindow.xoffs = my_va_arg_i(ap);
    MainWindow.yoffs = my_va_arg_i(ap);
    return TRUE;

}

BOOLEAN cmd_opengl(int argc, my_va_list ap, char *err_msg)	/* Open a graphics library */
{
    sprintf(err_msg,"Action opengl not yet implemented");
    return FALSE;
}

BOOLEAN cmd_out(int argc, my_va_list ap, char *err_msg)	/* Send value out IBM PC port */
{
    fprintf(stderr,"Warning: 'OUT' not supported on this platform\n");
    return TRUE;
}

BOOLEAN cmd_palette(int argc, my_va_list ap, char *err_msg)
{
struct PIC_register *r = &preg[my_va_arg_i(ap)];

    current_cmap = CMaps;
    copy_cmap(current_cmap,r->cmap);

    return TRUE;

}

BOOLEAN cmd_pan(int argc, my_va_list ap, char *err_msg)
{
int x1 = my_va_arg_i(ap);
int y1 = my_va_arg_i(ap);
int x2 = my_va_arg_i(ap);
int y2 = my_va_arg_i(ap);
struct PIC_register *r = &preg[my_va_arg_i(ap)];

    return pan_preg(r,x1,y1,x2,y2,err_msg);

}

BOOLEAN cmd_pfade(int argc, my_va_list ap, char *err_msg)
{
int effect = my_va_arg_i(ap);
int reg = my_va_arg_i(ap);
int speed = (argc>2)?my_va_arg_i(ap):0;
int delay = (argc>3)?my_va_arg_i(ap):0;

struct PIC_register *r = &preg[reg];

    if (speed==-1)
	speed=0;

    if (reg>MAXPICREG) {
	sprintf(err_msg,"PIC register %d out of range!",reg);
	return FALSE;
    }

    if (reg==0)
	r = NULL;
    if (!put_preg(r,0,0,effect,speed,TO_DRAWING,err_msg))
	return FALSE;
    sit_for(delay);
    return TRUE;

}

BOOLEAN cmd_pfree(int argc, my_va_list ap, char *err_msg)
{

    while (argc--)
	clear_preg(&preg[my_va_arg_i(ap)]);
    return TRUE;

}

BOOLEAN cmd_pgetbuf(int argc, my_va_list ap, char *err_msg)	/* Read preg from screen */
{
int reg = my_va_arg_i(ap);
int x1 = (argc<2)?-1:my_va_arg_i(ap);
int y1 = (argc<3)?-1:my_va_arg_i(ap);
int x2 = (argc<4)?-1:my_va_arg_i(ap);
int y2 = (argc<5)?-1:my_va_arg_i(ap);

    return get_reg(&preg[reg],x1,y1,x2,y2,TRUE,err_msg);

}

static BOOLEAN load_image(char *fname, int reg, BOOLEAN load_cmap,
			  int trancolor, char *err_msg)
{
char *ext;
int i;
    ext = strrchr(fname,'.');
    if ((ext!=NULL) && !strcmp(ext,".pcx")) {
	if (MainWindow.theWindow == MainWindow.RootW) {
	    for (i=0;i<VIDTYPES;i++) {
		if (video_modes[i].key == 'l') {
		    start_window(&video_modes[i],'l',err_msg);
		    break;
		}
	    }
	}
	return pcx_load(fname, reg, load_cmap, -1, err_msg);
    } else if ((ext!=NULL) && !strcmp(ext,".gif")) {
	if (MainWindow.theWindow == MainWindow.RootW) {
	    for (i=0;i<VIDTYPES;i++) {
		if (video_modes[i].key == 'l') {
		    start_window(&video_modes[i],'l',err_msg);
		    break;
		}
	    }
	}
	return gifLoad(reg, load_cmap, fname, err_msg);
    } else {
	return pic_load(fname, reg, load_cmap, trancolor, err_msg);
    }
}

BOOLEAN cmd_pload(int argc, my_va_list ap, char *err_msg)
{
char *fname = my_va_arg_c(ap);
int reg = (argc<2)?1:my_va_arg_i(ap);

    return load_image(fname, reg, TRUE, -1, err_msg);

}

BOOLEAN cmd_pnewbuf(int argc, my_va_list ap, char *err_msg)	/* Create new preg */
{
int reg = my_va_arg_i(ap);
int width = (argc<2)?MainWindow.Width:my_va_arg_i(ap);
int height = (argc<3)?MainWindow.Height:my_va_arg_i(ap);
    return create_preg(&preg[reg],width,height,err_msg);
}

BOOLEAN cmd_point(int argc, my_va_list ap, char *err_msg)
{
int x = my_va_arg_i(ap);
int y = my_va_arg_i(ap);

int x2,y2;

    if (argc>2) {
	x2 = my_va_arg_i(ap);
	y2 = my_va_arg_i(ap);
	x += x2*rand();
	y += y2*rand();
    }
    drawpoint(x,y);
    return TRUE;
}

BOOLEAN cmd_poke(int argc, my_va_list ap, char *err_msg)	/* Poke into PC memory */
{
    fprintf(stderr,"Warning: 'POKE' not supported on this platform\n");
    return TRUE;
}

BOOLEAN cmd_pokel(int argc, my_va_list ap, char *err_msg)	/* Poke into PC memory */
{
    fprintf(stderr,"Warning: 'POKEL' not supported on this platform\n");
    return TRUE;
}

BOOLEAN cmd_pokew(int argc, my_va_list ap, char *err_msg)	/* Poke into PC memory */
{
    fprintf(stderr,"Warning: 'POKEW' not supported on this platform\n");
    return TRUE;
}

BOOLEAN cmd_position(int argc, my_va_list ap, char *err_msg)
{
int reg = my_va_arg_i(ap);
int x = my_va_arg_i(ap);
int y = my_va_arg_i(ap);
char *rel = (argc<4)?"":my_va_arg_c(ap);

struct PIC_register *r = &preg[reg];

    put_partial(r,x,y,(*rel=='r'));
    return TRUE;

}

BOOLEAN cmd_psave(int argc, my_va_list ap, char *err_msg)	/* Save preg to file */
{
    sprintf(err_msg,"Action psave not yet implemented");
    return FALSE;
}

BOOLEAN cmd_psetbuf(int argc, my_va_list ap, char *err_msg)	/* Draw to preg instead of screen */
{
int reg = (argc==1)?0:my_va_arg_i(ap);
    return set_drawing_reg(reg,err_msg);
}

BOOLEAN cmd_putdff(int argc, my_va_list ap, char *err_msg)	/* Put diff buffer to screen */
{
    sprintf(err_msg,"Action putdff not yet implemented");
    return FALSE;
}

BOOLEAN cmd_putup(int argc, my_va_list ap, char *err_msg)
{
int x = my_va_arg_i(ap);
int y = my_va_arg_i(ap);
int reg = my_va_arg_i(ap);
int delay = (argc<4)?0:my_va_arg_i(ap);

struct PIC_register *r = &creg[reg];

    if (reg>MAXCLPREG) {
	sprintf(err_msg,"Clip register %d out of range!",reg);
	return FALSE;
    }

    if (!put_creg(r,x,y,EFFECT_NONE,0,TO_WINDOW,err_msg))
	return FALSE;
    sit_for(delay);
    return TRUE;

}

BOOLEAN cmd_resetgl(int argc, my_va_list ap, char *err_msg)	/* Reset GL library */
{
    sprintf(err_msg,"Action resetgl not yet implemented");
    return FALSE;
}

/* resetscr only makes sense in the context of certain hardware modes on an EGA
   monitor. */
BOOLEAN cmd_resetscr(int argc, my_va_list ap, char *err_msg)
{
    return TRUE;
}

BOOLEAN cmd_return(int argc, my_va_list ap, char *err_msg)
{

    return pop_subr(err_msg);

}

BOOLEAN cmd_revpage(int argc, my_va_list ap, char *err_msg)	/* Swap drawing and visible pages */
{
    return set_disp_page(-1,-1,err_msg);
}

BOOLEAN cmd_send(int argc, my_va_list ap, char *err_msg)	/* Send string to device */
{
char *dev = my_va_arg_c(ap);
char *string = my_va_arg_c(ap);
    if (!strncmp(dev,"con",3)) {
	puts(string);
    } else {
	fprintf(stderr,"Warning: 'SEND' not supported on this platform\n");
    }
    return TRUE;
}

BOOLEAN cmd_set(int argc, my_va_list ap, char *err_msg)
{
char *variable = my_va_arg_c(ap);
int  state = my_va_arg_i(ap);

    fprintf(stderr,"set: Variable %s not yet recognized. ignored\n",variable);
    return TRUE;

}

BOOLEAN cmd_setcolor(int argc, my_va_list ap, char *err_msg)
{
int i,cval;

    for (i=0; (i<argc) && (i<MAXEGACOLOR); i++) {
	cval = my_va_arg_i(ap);
	set_current_cmap_entry(i,
	    EGA_color[cval].red, EGA_color[cval].green, EGA_color[cval].blue);
    }
    return TRUE;

}

BOOLEAN cmd_setpage(int argc, my_va_list ap, char *err_msg)
{
int disp = my_va_arg_i(ap);
int draw = (argc<2)?disp:my_va_arg_i(ap);
    return set_disp_page(disp,draw,err_msg);
}

BOOLEAN cmd_setrgb(int argc, my_va_list ap, char *err_msg)
{
int cnum = my_va_arg_i(ap);
int red = my_va_arg_i(ap);
int green = my_va_arg_i(ap);
int blue = my_va_arg_i(ap);

    set_current_cmap_entry(cnum, red, green, blue);
    return TRUE;

}

BOOLEAN cmd_setupscr(int argc, my_va_list ap, char *err_msg)	/* Set up virtual screen for panning */
{
    sprintf(err_msg,"Action setupscr not yet implemented");
    return FALSE;
}

BOOLEAN cmd_spread(int argc, my_va_list ap, char *err_msg)
{
int pal1 = (argc>1)?my_va_arg_i(ap):0;
int pal2 = my_va_arg_i(ap);
int rstep = (argc>2)?my_va_arg_i(ap):1;
int gstep = (argc>3)?my_va_arg_i(ap):rstep;
int bstep = (argc>4)?my_va_arg_i(ap):gstep;

    return spread_colors(pal1?preg[pal1].cmap:current_cmap,preg[pal2].cmap,
			 rstep,gstep,bstep);

}

BOOLEAN cmd_text(int argc, my_va_list ap, char *err_msg)
{
int x = my_va_arg_i(ap);
int y = my_va_arg_i(ap);
char *string = my_va_arg_c(ap);
int delay = (argc<4)?0:my_va_arg_i(ap);

    x = screen_x(x);
    y = screen_y(y,curfont->height);
    while (*string) {
	put_char(*string,x,y);
	if (*string == ' ') {
	    x += curfont->sgap;
	} else {
	    x += curfont->cgap + curfont->cwidths[*string-curfont->firstch].width;
	}
	string++;
    }
    sit_for(delay);
    return TRUE;

}

BOOLEAN cmd_tile(int argc, my_va_list ap, char *err_msg)
{
int reg = my_va_arg_i(ap);
int delay = (argc<2)?0:my_va_arg_i(ap);

    tile_window(&creg[reg]);
    sit_for(delay);
    return TRUE;

}

BOOLEAN cmd_timer(int argc, my_va_list ap, char *err_msg)	/* Initialize timer (??) */
{
    /* I don't think this actually does anything relevant... */
    return TRUE;
}

BOOLEAN cmd_tran(int argc, my_va_list ap, char *err_msg)
{
char *state = my_va_arg_c(ap);
int i;

    if ((state[0]=='0') || (state[1]=='f')) {
	transparent_mode = FALSE;
    } else {
	transparent_mode = TRUE;
	for (i=0;i<MainWindow.maxcolor;i++) is_transparent[i] = FALSE;
	is_transparent[index_to_pixel(0)]=(argc==1);
	while (--argc) {
	    i = my_va_arg_i(ap);
	    is_transparent[index_to_pixel(i)] = TRUE;
	}
    }
    return TRUE;

}

BOOLEAN cmd_video(int argc, my_va_list ap, char *err_msg)
{
char *vidtype = my_va_arg_c(ap);

int i;

    for (i=0;i<VIDTYPES;i++) {
	if (*vidtype == video_modes[i].key)
	    return(start_window(&video_modes[i],*vidtype,err_msg));
    }
    sprintf(err_msg,"Unrecognized video mode %c",*vidtype);
    return FALSE;

}

BOOLEAN cmd_waitkey(int argc, my_va_list ap, char *err_msg)
{
int timeout = (argc<1)?-1:my_va_arg_i(ap);	/* In 100ths of a second */

    if (((lastkey=get_keystroke(timeout)) == -1) && (argc > 1))
	return cmd_goto(argc-1,ap,err_msg);
    return TRUE;

}

BOOLEAN cmd_when(int argc, my_va_list ap, char *err_msg)	/* Perform action on given key press */
{
    sprintf(err_msg,"Action when not yet implemented");
    return FALSE;
}

BOOLEAN cmd_window(int argc, my_va_list ap, char *err_msg) /* Def clipping window */
{
int x1 = (argc<1)?-1:my_va_arg_i(ap);
int y1 = (argc<2)?0:my_va_arg_i(ap);
int x2 = (argc<3)?0:my_va_arg_i(ap);
int y2 = (argc<4)?0:my_va_arg_i(ap);

    return set_clipping_region(clipx1=x1,clipy1=y1,clipx2=x2,clipy2=y2,err_msg);

}
