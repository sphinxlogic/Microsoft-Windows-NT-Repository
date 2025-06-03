/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

extern int sys_nerr, errno;
extern char *sys_errlist[];
#define SYSERR sys_errlist[(errno > sys_nerr? 0 : errno)]

GLOBAL Display *disp;
GLOBAL Window rootw, mainw, lifew, inputw, coordw;
GLOBAL int screen;
GLOBAL unsigned long fcolor, bcolor;
GLOBAL XEvent event;
GLOBAL XFontStruct *nfont, *bfont;
GLOBAL int getinput;
#define INPBUFLEN 255
GLOBAL char inpbuf[INPBUFLEN];
GLOBAL int minbuflen;
#define DIRBUFLEN 100
GLOBAL int numcomments;
#define MAXCOMMENTS 50
GLOBAL char comments[MAXCOMMENTS][256];
GLOBAL char loadirbuf[DIRBUFLEN];
GLOBAL char loadir[DIRBUFLEN];
GLOBAL char keybuf[16];
GLOBAL char lookup[0xfffff];
GLOBAL char fname[256];
GLOBAL KeySym ks;
GLOBAL XGCValues xgcv;
GLOBAL GC ntextgc, btextgc,inputgc,blackgc,whitegc,xorgc;
GLOBAL Pixmap lifepm;
GLOBAL Cursor cursor;

GLOBAL char *itoa();

GLOBAL unsigned long xpos,ypos,xorigin,yorigin,lastx,lasty;

GLOBAL int getcell();
GLOBAL void generate();
GLOBAL void addcell();
GLOBAL void deletecell();
GLOBAL void redisplay();
GLOBAL void forget();
GLOBAL void Motion();
GLOBAL void setcell();
#define kill LOCAL_KILL
GLOBAL void kill();
GLOBAL void clear();
GLOBAL void savefile();
GLOBAL void loadfile();
GLOBAL void center();
GLOBAL void newrules();
GLOBAL void redrawscreen();
GLOBAL void help();
GLOBAL void randomize();
GLOBAL void settimeout();
GLOBAL void gentab();
GLOBAL void saveall();
GLOBAL void getxstring();
GLOBAL void name_file();
GLOBAL void comment();
GLOBAL void view_comments();
GLOBAL void benchmark();
GLOBAL unsigned long dispcells;
GLOBAL unsigned long load;
GLOBAL unsigned long save;
GLOBAL unsigned long scale;
GLOBAL unsigned long born;
GLOBAL unsigned long live;
GLOBAL unsigned long run;
GLOBAL unsigned long maxdead;
GLOBAL unsigned long hide;
GLOBAL unsigned long generations;
GLOBAL unsigned long numboxes;
GLOBAL unsigned long numcells;
GLOBAL unsigned long collisions;
#define CHASHSIZE 65537
#define CHASHMULT 16383

GLOBAL unsigned long hashcnt[CHASHSIZE];
GLOBAL int width;
GLOBAL int height;
GLOBAL int inputlength;
GLOBAL int onpt,offpt,onrect,offrect;
GLOBAL int state,dispcoord;
GLOBAL struct timeval timeout;


GLOBAL int link_called;
GLOBAL int link_search;
GLOBAL int create_called;
GLOBAL int create_null;

GLOBAL int txx,txy,tyx,tyy;  /* transformation for new points */ 
GLOBAL unsigned long loadx, loady; /* location to load new points */

/* globals for tentative population */
GLOBAL unsigned long tentnumboxes,tentnumcells,tentgenerations;
