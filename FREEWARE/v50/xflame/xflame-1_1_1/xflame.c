/*****************************************************************************/
/*                               XFlame v1.1.1                               */
/*****************************************************************************/
/* Originally By:                                                            */
/*     The Rasterman (Carsten Haitzler)                                      */
/*      Copyright (C) 1996                                                   */
/*****************************************************************************/
/* Current Maintainer:                                                       */
/*     Rahul Jain                                                            */
/*       16 and higher bit displays                                          */
/*       Frame rate limiting                                                 */
/* Other Contributors:                                                       */
/*     Alex D. Baxter                                                        */
/*       Window title setting                                                */
/*     Kent Howard                                                           */
/*       Silly bug in delay-checking code fixed                              */
/*****************************************************************************/
/* This code is Freeware. You may copy it, modify it or do with it as you    */
/* please, but you may not claim copyright on any code wholly or partly      */
/* based on this code. I accept no responisbility for any consequences of    */
/* using this code, be they proper or otherwise.                             */
/*****************************************************************************/
/* Okay, now all the legal mumbo-jumbo is out of the way, I will just say    */
/* this: enjoy this program, do with it as you please and watch out for more */
/* code releases from The Rasterman running under X... the only way to code. */
/*****************************************************************************/


/* INCLUDES! */
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <time.h>
#include <sys/time.h>
#ifdef VMS
#if __VMS_VER < 70000000
#include "unix_time.h"
#endif 
#include <timers.h>
#include <lib$routines.h>
unsigned long int statvms;
float seconds;
#endif

/* DEFINES */
#define GEN 500
#define MAX 300
#define WID 80
#define HIH 60
#define HSPREAD 26
#define VSPREAD 78
#define VFALLOFF 14
#define VARIANCE 5
#define VARTREND 2
#define RESIDUAL 68

#define NONE 0x00
#define ROOT 0x01
#define CMAP 0x02
#define NICE 0x04
#define DELT 0x08
#define CLEN 0x10
#define BLOK 0x20
#define LACE 0x40
#define RES8 0x80

/*This structure contains all of the "Global" variables for my program so */
/*that I just pass a pointer to my functions, not a million parameters */
struct globaldata
{
  /* These entries are all for X */
  Display *disp; /* the display I'm using*/
  Window win; /* the window I'm using */
  Visual *vis; /* the visual I'm using */
  int scr; /* the screen number i'm using */
  int depth;
  GC gc; /* the graphics context for the window */
  XImage *xim; /* the buffered image. */
  unsigned long *carray;
  unsigned char *im; /* the actual data for the buffered image */
  int ims; /* the size of the image as 2^ims pixels */
  unsigned long wht; /* the color index for white*/
  unsigned long blk; /* the color index for black*/
  long microsperframe;
  unsigned short RBASE,GBASE,BBASE;
};

void printhelp()
{
  printf("XFlame v1.1.1\n");
  printf("     Originally By: The Rasterman (Carsten Haitzler)\n");
  printf("         e-mail: s2154962@cse.unsw.edu.au\n");
  printf("         web: http://www.cse.unsw.edu.au/~s2154962/\n");
  printf("     Current Maintainer: Rahul Jain\n");
  printf("         e-mail: rahul@rice.edu\n");
  printf("     Other Contributers:\n");
  printf("         Alex D. Baxter (alex@oenone.demon.co.uk)\n");
  printf("         Kent Howard\n");
  printf("     Copyright (C) 1996-1998\n");
  printf("  Please read the DOCS!\n");
  printf("Options:\n");
  printf("  -h : Prints this help\n");
  printf("  -root : runs XFlame in the root window\n");
  printf("  -title <t>: XFlame's window title will be <t>\n");
  printf("  -cmap : XFlame uses its own colormap in its window\n");
  printf("  -nicecmap : XFlame allocates fewer colors\n");
  printf("  -block : XFlame updates the screen in one large block\n");
  printf("  -delta : XFlame updates the screen in deltas (8 bit color only)\n");
  printf("  -lace : XFlame updates the screen using extended interlacing\n");
  printf("  -clean : XFlame uses save-unders and backing store\n");
  printf("  -width <w> : XFlame uses a window of width <w> pixels\n");
  printf("  -height <h> : XFlame uses a window of height <h> pixels\n");
  printf("  -display <d> : XFlame will run on display <d>\n");
  printf("  -fps <f> : XFlame will show at most <f> frames per second\n");
  printf("  -color <c> : XFlame will use <c> as its base color.\n");
  printf("               This is hard to explain, just experiment a bit :-)\n");
  printf("               <c> is an X color string,\n");
  printf("               e.g. #FFAA66, green, rgb:5/b/b, cieluv:70/.5/.2\n");
}

void fewargs()
{
  /* If someone gives too few arguments, then tell them so! */
  printf("Too few Arguments!\n");
  return;
}

int powerof(unsigned int n)
{
  /* This returns the power of a number (eg powerof(8)==3, powerof(256)==8, */
  /* powerof(1367)==11, powerof(2568)==12) */
  int p=32;
  if (n<=0x80000000) p=31;
  if (n<=0x40000000) p=30;
  if (n<=0x20000000) p=29;
  if (n<=0x10000000) p=28;
  if (n<=0x08000000) p=27;
  if (n<=0x04000000) p=26;
  if (n<=0x02000000) p=25;
  if (n<=0x01000000) p=24;
  if (n<=0x00800000) p=23;
  if (n<=0x00400000) p=22;
  if (n<=0x00200000) p=21;
  if (n<=0x00100000) p=20;
  if (n<=0x00080000) p=19;
  if (n<=0x00040000) p=18;
  if (n<=0x00020000) p=17;
  if (n<=0x00010000) p=16;
  if (n<=0x00008000) p=15;
  if (n<=0x00004000) p=14;
  if (n<=0x00002000) p=13;
  if (n<=0x00001000) p=12;
  if (n<=0x00000800) p=11;
  if (n<=0x00000400) p=10;
  if (n<=0x00000200) p=9;
  if (n<=0x00000100) p=8;
  if (n<=0x00000080) p=7;
  if (n<=0x00000040) p=6;
  if (n<=0x00000020) p=5;
  if (n<=0x00000010) p=4;
  if (n<=0x00000008) p=3;
  if (n<=0x00000004) p=2;
  if (n<=0x00000002) p=1;
  if (n<=0x00000001) p=0;
  return p;
}

int powerofless(unsigned int n)
{
  /* This returns the power of a number (eg powerof(8)==3, powerof(256)==8, */
  /* powerof(1367)==10, powerof(2568)==11) */
  if (n<=0x00000001) return 0;
  if (n<=0x00000002) return 1;
  if (n<=0x00000004) return 2;
  if (n<=0x00000008) return 3;
  if (n<=0x00000010) return 4;
  if (n<=0x00000020) return 5;
  if (n<=0x00000040) return 6;
  if (n<=0x00000080) return 7;
  if (n<=0x00000100) return 8;
  if (n<=0x00000200) return 9;
  if (n<=0x00000400) return 10;
  if (n<=0x00000800) return 11;
  if (n<=0x00001000) return 12;
  if (n<=0x00002000) return 13;
  if (n<=0x00004000) return 14;
  if (n<=0x00008000) return 15;
  if (n<=0x00010000) return 16;
  if (n<=0x00020000) return 17;
  if (n<=0x00040000) return 18;
  if (n<=0x00080000) return 19;
  if (n<=0x00100000) return 20;
  if (n<=0x00200000) return 21;
  if (n<=0x00400000) return 22;
  if (n<=0x00800000) return 23;
  if (n<=0x01000000) return 24;
  if (n<=0x02000000) return 25;
  if (n<=0x04000000) return 26;
  if (n<=0x08000000) return 27;
  if (n<=0x10000000) return 28;
  if (n<=0x20000000) return 29;
  if (n<=0x40000000) return 30;
  if (n<=0x80000000) return 31;
  return 32;
}


void GetRootWindow(struct globaldata *g)
{
  /*This function gets a handle on the default root window*/
  g->win=DefaultRootWindow(g->disp);
}

int OpenDisp(struct globaldata *g, char *d)
{
  /*This routine opens up a connection to the X display d */
  Display *disp;
  
  disp=XOpenDisplay(d);
  if (disp==NULL)
    {
      /* If there was a problem in opening the display, return 0 */
      return 0;
    }
  else
    {
      /* We opened the display sucessfully, now get a few parameters of the */
      /* display */
      g->disp=disp;
      /* Get the defalt screen number */
      g->scr=DefaultScreen(disp);
      /* Get the color index of the black pixel */
      g->blk=BlackPixel(disp,g->scr);
      /* Get the color index of the white pixel */
      g->wht=WhitePixel(disp,g->scr);
      /* Get the Visual for the display */
      g->vis=DefaultVisual(disp,g->scr);
	  /* Get the depth for the display */
      g->depth=DefaultDepth(disp,g->scr);
      return 1;
    }
}

int MakeGC(struct globaldata *g)
{
  /*This function Creates a GraphicsContext for a Window */
  XGCValues gcvals;
  GC gc;
  
  /* Create the GC */
  gc=XCreateGC(g->disp,g->win,(unsigned long)0,&gcvals);
  if (gc)
    {
      /* If we got our GC, set then fg & bg and return 1) */	
      XSetForeground(g->disp,gc,g->wht);
      XSetBackground(g->disp,gc,g->blk);
      g->gc=gc;
      return 1;
    }
  else
    {
      /* if we couldn't get our gc, return 0 */
      return 0;
    }
}

int OpenWindow(struct globaldata *g,int w,int h, char *title)
{
  /*This function opens up a window of width w and height h*/
  /* title of window (and icon) is title - added by alex@oenone */
  g->win=XCreateSimpleWindow(g->disp,RootWindow(g->disp,g->scr)
			     ,10,10,w,h,5,g->blk,g->blk);
  if (!MakeGC(g))
    {
      XDestroyWindow(g->disp,g->win);
      return 0;
    }
  XMapWindow(g->disp,g->win);
  XmbSetWMProperties(g->disp, g->win, title, title, NULL, 0, NULL, NULL, NULL);
  XFlush(g->disp);
  return 1;
}

void InitColorArray(struct globaldata *gb) {
  unsigned int cval,rmask,gmask,bmask;
  int r,g,b,i,rshift,gshift,bshift;
  unsigned long *array=gb->carray;
  rshift=powerofless(rmask = gb->vis->red_mask)-8;
  gshift=powerofless(gmask = gb->vis->green_mask)-8;
  bshift=powerofless(bmask = gb->vis->blue_mask)-8;
  for (i=0;i<MAX;i++) {
	  r=(i-255+gb->RBASE)*3;
	  g=(i-255+gb->GBASE)*3;
	  b=(i-255+gb->BBASE)*3;
      if (r<0) r=0;
      else if (r>255) r=255;
      if (g<0) g=0;
      else if (g>255) g=255;
      if (b<0) b=0;
      else if (b>255) b=255;

	  if (rshift>0) {
		  cval=r<<rshift & rmask;
	  } else if (rshift<0) {
		  cval=r>>-rshift & rmask;
	  } else {
		  cval=r&rmask;
	  }
	  if (gshift>0) {
		  cval+=g<<gshift & gmask;
	  } else if (gshift<0) {
		  cval+=g>>-gshift & gmask;
	  } else {
		  cval+=g&gmask;
	  }
	  if (bshift>0) {
		  cval+=b<<bshift & bmask;
	  } else if (bshift<0) {
		  cval+=b>>(-bshift) & bmask;
	  } else {
		  cval+=b&bmask;
	  }
      array[i]=cval;
  }
}

void SetFlamePalette(struct globaldata *gb, int f,int *ctab)
{
  /*This function sets the flame palette */
  Colormap cm;
  XColor xcl;
  int r,g,b,i;
  
  if (f&CMAP)
    {
      /* if the user asled for the windows own colormap, take all the colors */
      /* the display has when this window is active */
      cm=XCreateColormap(gb->disp,gb->win,gb->vis,AllocNone);
      for (i=0;i<MAX;i++)
	{
	  r=(i-255+gb->RBASE)*3;
	  g=(i-255+gb->GBASE)*3;
	  b=(i-255+gb->BBASE)*3;
	  if (r<0) r=0;
	  if (r>255) r=255;
	  if (g<0) g=0;
	  if (g>255) g=255;
	  if (b<0) b=0;
	  if (b>255) b=255;
	  xcl.red=(unsigned short)(r<<8);
	  xcl.green=(unsigned short)(g<<8);
	  xcl.blue=(unsigned short)(b<<8);
	  xcl.flags=DoRed|DoGreen|DoBlue;
	  XAllocColor(gb->disp,cm,&xcl);
	  ctab[i]=(int)xcl.pixel;
	}
    }
  else if (f&NICE)
    {
      /* if the user wishes for the program to not take too many colors from */
      /* the colormap, this will reduce the RGB accuracy of the color */ 
      /* allocated and so reduce the number of colors grabbed */
      cm=DefaultColormap(gb->disp,gb->scr);
      for (i=0;i<MAX;i++)
	{
	  r=(i-255+gb->RBASE)*3;
	  g=(i-255+gb->GBASE)*3;
	  b=(i-255+gb->BBASE)*3;
	  if (r<0) r=0;
	  if (r>255) r=255;
	  if (g<0) g=0;
	  if (g>255) g=255;
	  if (b<0) b=0;
	  if (b>255) b=255;
	  xcl.red=(unsigned short)((r&0xe0)<<8);
	  xcl.green=(unsigned short)((g&0xe0)<<8);
	  xcl.blue=(unsigned short)((b&0xe0)<<8);
	  xcl.flags=DoRed|DoGreen|DoBlue;
	  XAllocColor(gb->disp,cm,&xcl);
	  ctab[i]=(int)xcl.pixel;
	}
    }
  else
    {
      /* by default we will take only the unallocated color cells in the */
      /* defualt colormap, to prevent palette switching when the window is */
      /* active/inactive */
      cm=DefaultColormap(gb->disp,gb->scr);
      for (i=0;i<MAX;i++)
	{
	  r=(i-255+gb->RBASE)*3;
	  g=(i-255+gb->GBASE)*3;
	  b=(i-255+gb->BBASE)*3;
	  if (r<0) r=0;
	  if (r>255) r=255;
	  if (g<0) g=0;
	  if (g>255) g=255;
	  if (b<0) b=0;
	  if (b>255) b=255;
	  xcl.red=(unsigned short)(r<<8);
	  xcl.green=(unsigned short)(g<<8);
	  xcl.blue=(unsigned short)(b<<8);
	  xcl.flags=DoRed|DoGreen|DoBlue;
	  XAllocColor(gb->disp,cm,&xcl);
	  ctab[i]=(int)xcl.pixel;
	}
    }
  /* attatch the colormap to the window so it comes into effect */
  XSetWindowColormap(gb->disp,gb->win,cm);
}

int GetWinWidth(struct globaldata *g)
{
  /*This function is returns the width of thw window */
  XWindowAttributes wa;
  
  XGetWindowAttributes(g->disp, g->win, &wa);
  return wa.width;
}

int GetWinHeight(struct globaldata *g)
{
  /*This function is returns the height of the window */
  XWindowAttributes wa;
  
  XGetWindowAttributes(g->disp, g->win, &wa);
  return wa.height;
}

void SetWindowAttributes(struct globaldata *g)
{
  /*This function sets certain attributes of the window */
  XSetWindowAttributes wa;
  unsigned long valmask;
  
  valmask=0x00002120;
  wa.background_pixel=g->blk;
  wa.backing_store=Always;
  wa.save_under=1;
  XChangeWindowAttributes(g->disp,g->win,valmask,&wa);
}

void SetWinBlack(struct globaldata *g)
{
  /*This function sets certain attributes of the window */
  XSetWindowAttributes wa;
  unsigned long valmask;
  
  valmask=0x00002000;
  wa.background_pixel=g->blk;
  XChangeWindowAttributes(g->disp,g->win,valmask,&wa);
}

int SetupXImage(struct globaldata *g,int w, int h)
{
  /*This function sets up the buffered image on which all drawing is done */
  /*and when completed, is copied (in interlaced fashion for better results */
  /*or even using 16x16 square dynamic delta techniques to increase update */
  /*speed on VERY large screens, like the root window, if the user so says) */
  /*to the window, to speedup drawing, and give us Double Buffering */
  int hbits,hs;
  int imsize;
  unsigned char *im;  
  
  hbits=powerof(w);
  hs=(1<<hbits);
  /* calculate the memory needed for the image data */
  imsize=hs*h*g->depth;
  /* allocate the memory for the image data */
  im=(char *)malloc(imsize);
  /* if we couldn't allocate the memory, return 0 */
  if (!im) return 0;
  /* create the XImage from the data */
  g->xim=XCreateImage(g->disp,g->vis,g->depth,ZPixmap,0,im,hs,h,32,0);
  g->im=im;
  g->ims=hbits;
  XMapWindow(g->disp,g->win);
  XFlush(g->disp);
  return 1;
}

void XFSetFlameZero(int *f, int w, int ws, int h)
{
  /*This function sets the entire flame array to zero*/
  int x,y,*ptr;
  
  for (y=0;y<h;y++)
    {
      for (x=0;x<w;x++)
	{
	  ptr=f+(y<<ws)+x;
	  *ptr=0;
	}
    }
}

void XFSetDeltaZero(int *d, int *dd, int w, int ds, int h)
{
  /*This function sets the entire delta array to zero and copies the d */
  /*array into the dd array fro double buffering */
  int x,y,*ptr,*p;
  
  for (y=0;y<h;y++)
    {
      for (x=0;x<w;x++)
	{
	  ptr=d+(y<<ds)+x;
	  p=dd+(y<<ds)+x;
	  *p=*ptr;
	  *ptr=0;
	}
    }
}

void XFSetRandomFlameBase(int *f, int w, int ws, int h)
{
  /*This function sets the base of the flame to random values */
  int x,y,*ptr;
  
  /* initialize a random number seed from the time, so we get random */
  /* numbers each time */
  srand(time(NULL));
  y=h-1;
  for (x=0;x<w;x++)
    {
      ptr=f+(y<<ws)+x;
      *ptr=rand()%MAX;
    }
}

void XFModifyFlameBase(int *f, int w, int ws, int h)
{
  /*This function modifies the base of the flame with random values */
  int x,y,*ptr,val;
  
  y=h-1;
  for (x=0;x<w;x++)
    {
      ptr=f+(y<<ws)+x;
      *ptr+=((rand()%VARIANCE)-VARTREND);
      val=*ptr;
      if (val>MAX) *ptr=0;
      if (val<0) *ptr=0;
    }
}

void XFProcessFlame(int *f, int w, int ws, int h, int *ff)
{
  /*This function processes entire flame array */
  int x,y,*ptr,*p,tmp,val;
  
  for (y=(h-1);y>=2;y--)
    {
      for (x=1;x<(w-1);x++)
	{
	  ptr=f+(y<<ws)+x;
	  val=(int)*ptr;
	  if (val>MAX) *ptr=(int)MAX;
	  val=(int)*ptr;
	  if (val>0)
	    {
	      tmp=(val*VSPREAD)>>8;
	      p=ptr-(2<<ws);				
	      *p=*p+(tmp>>1);
	      p=ptr-(1<<ws);				
	      *p=*p+tmp;
	      tmp=(val*HSPREAD)>>8;
	      p=ptr-(1<<ws)-1;
	      *p=*p+tmp;
	      p=ptr-(1<<ws)+1;
	      *p=*p+tmp;
	      p=ptr-1;
	      *p=*p+(tmp>>1);
	      p=ptr+1;
	      *p=*p+(tmp>>1);
	      p=ff+(y<<ws)+x;
	      *p=val;
	      if (y<(h-1)) *ptr=(val*RESIDUAL)>>8;
	    }
	}
    }
}

void XFDataToBuffer(struct globaldata *g,int *f, int w, int ws, int h, int *ctab) {
  int x,y,*ptr,xx,yy,cl,cl1,cl2,cl3,cl4,ims,i;
  unsigned short *iptr,*ip;
  unsigned long *lptr,*lp, *c,color;
  unsigned char *cptr,*im,*p;

  /*for (i=0;i<MAX;i++) { printf("%u: %X\n",i,g->carray[i]); }*/
  /* get pointer to the image data amd the image shift amount */
  im=g->im;
  ims=g->ims;
  /* copy the calculated flame array to the image buffer */
  for (y=0;y<(h-1);y++) {
      for (x=0;x<(w-1);x++) {
	  xx=x<<1;
	  yy=y<<1;
	  ptr=f+(y<<ws)+x;
	  cl1=cl=(int)*ptr;
	  ptr=f+(y<<ws)+x+1;
	  cl2=(int)*ptr;
	  ptr=f+((y+1)<<ws)+x+1;
	  cl3=(int)*ptr;
	  ptr=f+((y+1)<<ws)+x;
	  cl4=(int)*ptr;
	  switch (g->xim->bits_per_pixel) {
		case 8:
	      cptr=im+(yy<<ims)+xx;
	      *cptr=(unsigned char)ctab[cl%MAX];
	      p=cptr+1;
	      *p=(unsigned char)ctab[((cl1+cl2)>>1)%MAX];
	      p=cptr+1+(1<<ims);
	      *p=(unsigned char)ctab[((cl1+cl3)>>1)%MAX];
	      p=cptr+(1<<ims);
	      *p=(unsigned char)ctab[((cl1+cl4)>>1)%MAX];
		  break;
	    case 16:
	      c=g->carray;
	      iptr=(unsigned short *) im;
		  iptr+=((yy<<ims)+xx);
	      *iptr=(unsigned short) c[cl%MAX];
		  /*printf("%u: %X\n",cl%MAX,c[cl%MAX]);*/
	      ip=iptr+1;
	      *ip=(unsigned short) c[((cl1+cl2)>>1)%MAX];
	      ip=iptr+1+(1<<ims);
	      *ip=(unsigned short) c[((cl1+cl3)>>1)%MAX];
	      ip=iptr+(1<<ims);
	      *ip=(unsigned short) c[((cl1+cl4)>>1)%MAX];
		  break;
	    case 32:
	      c=g->carray;
	      lptr=(unsigned long *) im;
		  lptr+=((yy<<ims)+xx);
	      *lptr=c[cl%MAX];
	      lp=lptr+1;
	      *lp=c[((cl1+cl2)>>1)%MAX];
	      lp=lptr+1+(1<<ims);
	      *lp=c[((cl1+cl3)>>1)%MAX];
	      lp=lptr+(1<<ims);
	      *lp=c[((cl1+cl4)>>1)%MAX];
		  break;
		default:
		  printf("Error: Unsupported color depth");
		  exit(1);
	  }
	}
  }
}

void XFDrawFlameBLOK(struct globaldata *g,int *f, int w, int ws, int h, int *ctab)
{
  /*This function copies & displays the flame image in one large block */
  
  XFDataToBuffer(g,f,w,ws,h,ctab);
  
  /* copy the image to the screen in one large chunk */
  XPutImage(g->disp,g->win,g->gc,g->xim,0,0,0,0,(w<<1),(h<<1));
  XFlush(g->disp);
}

void XFDrawFlameDELT(struct globaldata *g,int *f, int w, int ws, int h, int *d, int *dd, int dw, int ds, int dh, int *ctab)
{
  /*This function copies & displays the flame image in DELTA fashion. */
  /*this method is far more complex than plain BLOCK transfer or even */
  /*interlaced method, as it only copies what it absolutely has to to */
  /*the screen. the image is divided up into 32x32 pixel DELTAs, and if */
  /*the contents of this DELTA isn't completely black, it is copied to */
  /*the screen, so if the flame image only takes approximately 1/3 of */
  /*the entire image, only 1/3 of the pixles are copied, in 32x32 pixel */
  /*blocks, so on LARGE flame windows (such as in root), we save a LOT */
  /*of image copying, by only copying what is necessary*/
  int x,y,*ptr,xx,yy,cl,cl1,cl2,cl3,cl4,ims,*pp;
  unsigned char *cptr,*im,*p;
  
  /* get pointer to the image data amd the image shift amount */
  im=g->im;
  ims=g->ims;
  /* set the delta array to ZERO */
  XFSetDeltaZero(d,dd,dw,ds,dh);
  /* copy the calculated flame array to the image buffer */
  for (y=0;y<(h-1);y++)
    {
      for (x=0;x<(w-1);x++)
	{
	  xx=x<<1;
	  yy=y<<1;
	  ptr=f+(y<<ws)+x;
	  cl1=cl=(int)*ptr;
	  ptr=f+(y<<ws)+x+1;
	  cl2=(int)*ptr;
	  ptr=f+((y+1)<<ws)+x+1;
	  cl3=(int)*ptr;
	  ptr=f+((y+1)<<ws)+x;
	  cl4=(int)*ptr;
	  cptr=im+(yy<<ims)+xx;
	  *cptr=(unsigned char)ctab[cl%MAX];
	  p=cptr+1;
	  *p=(unsigned char)ctab[((cl1+cl2)>>1)%MAX];
	  p=cptr+1+(1<<ims);
	  *p=(unsigned char)ctab[((cl1+cl3)>>1)%MAX];
	  p=cptr+(1<<ims);
	  *p=(unsigned char)ctab[((cl1+cl4)>>1)%MAX];
	  /* now add the pixel value to the total value of this pixels DELTA */
	  xx=xx>>5;
	  yy=yy>>5;
	  ptr=d+(yy<<ds)+xx;
	  *ptr+=(int)cl;
	}
    }
  /* copy the image to the screen in one delta at a time */
  for (y=0;y<dh;y++)
    {
      for (x=0;x<dw;x++)
	{
	  ptr=d+(y<<ds)+x;
	  pp=dd+(y<<ds)+x;			
	  if ((*ptr)||(*pp))
	    {
	      XPutImage(g->disp,g->win,g->gc,g->xim,x<<5,y<<5,x<<5,y<<5,32,32);
	    }
	}
    }
  XFlush(g->disp);
}

void XFDrawFlameLACE(struct globaldata *g,int *f, int w, int ws, int h, int *ctab)
{
  /*This function copies & displays the flame image in interlaced fashion */
  /*that it, it first processes and copies the even lines to the screen, */
  /* then is processes and copies the odd lines of the image to the screen */
  int y;

  XFDataToBuffer(g,f,w,ws,h,ctab);
  
  /* copy the even lines to the screen */
  for (y=0;y<((h<<1)-1);y+=4)
    {
      XPutImage(g->disp,g->win,g->gc,g->xim,0,y,0,y,(w<<1),1);
    }
  XFlush(g->disp);
  /* copy the odd lines to the screen */
  for (y=2;y<((h<<1)-1);y+=4)
    {
      XPutImage(g->disp,g->win,g->gc,g->xim,0,y,0,y,(w<<1),1);
    }
  XFlush(g->disp);
  /* copy the even lines to the screen */
  for (y=1;y<((h<<1)-1);y+=4)
    {
      XPutImage(g->disp,g->win,g->gc,g->xim,0,y,0,y,(w<<1),1);
    }
  XFlush(g->disp);
  /* copy the odd lines to the screen */
  for (y=3;y<((h<<1)-1);y+=4)
    {
      XPutImage(g->disp,g->win,g->gc,g->xim,0,y,0,y,(w<<1),1);
    }
  XFlush(g->disp);
}

void XFDrawFlame(struct globaldata *g,int *f, int w, int ws, int h, int *ctab)
{
  /*This function copies & displays the flame image in interlaced fashion */
  /*that it, it first processes and copies the even lines to the screen, */
  /* then is processes and copies the odd lines of the image to the screen */
  int y;

  XFDataToBuffer(g,f,w,ws,h,ctab);
  
  /* copy the even lines to the screen */
  for (y=0;y<((h<<1)-1);y+=2)
    {
      XPutImage(g->disp,g->win,g->gc,g->xim,0,y,0,y,(w<<1),1);
    }
  XFlush(g->disp);
  /* copy the odd lines to the screen */
  for (y=1;y<((h<<1)-1);y+=2)
    {
      XPutImage(g->disp,g->win,g->gc,g->xim,0,y,0,y,(w<<1),1);
    }
  XFlush(g->disp);
}

int Xflame(struct globaldata *g,int w, int h, int f, int *ctab)
{
  /*This function is the hub of XFlame.. it initialises the flame array, */
  /*processes the array, genereates the flames and displays them */
  int x,y,cl,cll,*flame,*delta,flamesize,deltawidth,deltaheight,ws,ds;
  int flamewidth,flameheight,deltasize,dwidth,*flame2,*delta2,flag;
  long MICROSPERFRAME=g->microsperframe,microsleft;
  long elapsedmicros;
  struct timeval rtv1,rtv2,*tv1,*tv2;
  struct timezone rtz,*tz;
  struct timespec rtwait,rtleft,*twait,*tleft;
  
  tv1=&rtv1;
  tv2=&rtv2;
  tz=&rtz;
  twait=&rtwait;
  tleft=&rtleft;
  twait->tv_sec=0;
  
  /* work out the size needed for the flame array */
  flamewidth=w>>1;
  flameheight=h>>1;
  ws=powerof(flamewidth);
  flamesize=(1<<ws)*flameheight*sizeof(int);
  /* allocate the memory for the flame array */
  flame=(int *)malloc(flamesize);
  /* if we didn't get the memory, return 0 */
  if (!flame) return 0;
  /* allocate the memory for the second flame array */
  flame2=(int *)malloc(flamesize);
  /* if we didn't get the memory, return 0 */
  if (!flame2) return 0;
  /* if the user specified DELTA updating, allocate a delta array */
  if (f&DELT)
    {
      dwidth=w>>5;
      if (dwidth==0) dwidth=0;
      deltawidth=(ws-5);
      ds=powerof(deltawidth);
      if (deltawidth==0) deltawidth=1;
      deltaheight=(h>>5)+1;
      if (deltaheight==0) deltaheight=1;
      /* calculate the delta size */
      deltasize=deltawidth*deltaheight*sizeof(int);
      /* allocate the memory for the delta array */
      delta=(int *)malloc(deltasize);
      /* if we couldn't get the memory, return 0 */
      if (!delta) return 0;
      /* allocate the memory for the second delta array */
      delta2=(int *)malloc(deltasize);
      /* if we couldn't get the memory, return 0 */
      if (!delta2) return 0;
    }
  /* set the flame array to ZERO */
  XFSetFlameZero(flame,w>>1,ws,h>>1);
  /* set the base of the flame to something random */
  XFSetRandomFlameBase(flame,w>>1,ws,h>>1);
  /* now loop forever, generating and displaying flames */
  for (;;)
    {
	  /* get the current time for later comparison */
	  gettimeofday(tv1,tz);
      /* modify the base of the flame */
      XFModifyFlameBase(flame,w>>1,ws,h>>1);
      /* process the flame array, propagating the flames up the array */
      XFProcessFlame(flame,w>>1,ws,h>>1,flame2);
      /* if the user selected DELTA display method, then draw the flames */
      /* using DELTAs instead of interlaced */
      if (f&DELT)
	{
	  XFDrawFlameDELT(g,flame2,w>>1,ws,h>>1,delta,delta2,dwidth,ds,
			  deltaheight,ctab);
	}
      /* if the user selected BLOCK display method, then display the flame */
      /* all in one go, no fancy upating techniques involved */
      else if (f&BLOK)
	{
	  XFDrawFlameBLOK(g,flame2,w>>1,ws,h>>1,ctab);
	}
      else if (f&LACE)
	{
	  XFDrawFlameLACE(g,flame2,w>>1,ws,h>>1,ctab);
	}
      else
	/* the default of displaying the flames INTERLACED */
	{
	  XFDrawFlame(g,flame2,w>>1,ws,h>>1,ctab);
	}
	/* get the time */
	gettimeofday(tv2,tz);
	/* find out how much time elapsed in the last frame */
	if (tv1->tv_sec==tv2->tv_sec) {
		elapsedmicros=tv2->tv_usec-tv1->tv_usec;
	} else if (1+tv1->tv_sec==tv2->tv_sec) {
		elapsedmicros=tv1->tv_usec-tv2->tv_usec+100000l;
	} else { continue; }
	/* wait for the remaining time if necessary */
	if (elapsedmicros<MICROSPERFRAME) {
		microsleft=MICROSPERFRAME-elapsedmicros;
		twait->tv_nsec=microsleft*1000l;
#ifndef VMS

		flag=nanosleep(twait,tleft);
		while (flag==-1) flag=nanosleep(tleft,tleft);
#else
                seconds = ((float) microsleft)/1000000.0;
                statvms = lib$wait(&seconds);
#endif
	}
  }
}

/* Here's the MAIN part of the program */
int main(int argc, char **argv)
{
  struct globaldata glob;
  char disp[256],colorspec[256],title[80];
  int flags,useDefColor=1;
  int width,height,i,colortab[MAX],fps;
  unsigned long carray[MAX];
  XColor basecolor;
  Status status;
  
  /* Set all the variable to default values */
#ifndef VMS
  if (getenv("DISPLAY")) strcpy(disp,(char *) getenv("DISPLAY"));
#else
  if (getenv("DECW$DISPLAY")) strcpy(disp,(char *) getenv("DECW$DISPLAY"));
#endif
  else strcpy(disp,":0.0");
  flags=NONE;
  width=64;
  height=64;
  (&glob)->microsperframe=10000l;
  strcpy(title,"XFlame");
  
  /* Check command line for arguments */
  if (argc>1)
    {
      for (i=1;i<=argc;i++)
	{
	  /* if the user requests help */
	  if (!strcmp("-h",argv[i-1]))
	    {
	      printhelp();
	      exit(0); 
	    }
	  /* if the user requests to run in the root window */
	  if (!strcmp("-root",argv[i-1]))
	    {
	      flags|=ROOT;
	    }
	  /* run with a particular title, added by alex@oenone */
	  if (!strcmp("-title",argv[i-1]))
	    {
	      if ((i+1)>argc)
	        {
	          fewargs();
	          exit(1);
	        }
	      strncpy(title,argv[i],79);
	      *(title+80)='\0'; /* make sure it's terminated */
	      i++;
	    }
	  /* if the user requests to run with own colormap*/
	  if (!strcmp("-cmap",argv[i-1]))
	    {
	      flags|=CMAP;
	    }
	  /* if the user requests to run being nice to the colormap */
	  if (!strcmp("-nicecmap",argv[i-1]))
	    {
	      flags|=NICE;
	    }
	  /* if the user requests to use Delta updating of the image */
	  if (!strcmp("-delta",argv[i-1]))
	    {
	      flags|=DELT;
	    }
	  /* if the user requests to use Lace updating of the image */
	  if (!strcmp("-lace",argv[i-1]))
	    {
	      flags|=LACE;
	    }
	  /* if the user requests to use Block updating of the image */
	  if (!strcmp("-block",argv[i-1]))
	    {
	      flags|=BLOK;
	    }
	  /* if the user requests to use saveunders, backing stores etc. */
	  if (!strcmp("-clean",argv[i-1]))
	    {
	      flags|=CLEN;
	    }
	  /* if the user requests to run on a particular display */
	  if (!strcmp("-display",argv[i-1]))
	    {
	      if ((i+1)>argc)
		{
		  fewargs();
		  exit(1);
		}
	      strcpy(disp,argv[i]);
	      i++;
	    }
	  /* if the user requests a particular width */
	  if (!strcmp("-width",argv[i-1]))
	    {
	      if ((i+1)>argc)
		{
		  fewargs();
		  exit(1);
		}
	      width=atoi(argv[i]);
	      if (width<16)
		{
		  width=16;
		}
	      i++;
	    }
	  /* if the user requests a particular height */
	  if (!strcmp("-height",argv[i-1]))
	    {
	      if ((i+1)>argc)
		{
		  fewargs();
		  exit(1);
		}
	      height=atoi(argv[i]);
	      if (height<16)
		{
		  height=16;
		}
	      i++;
	    }
	  if (!strcmp("-fps",argv[i-1])) {
	      if ((i+1)>argc) {
		      fewargs();
		      exit(1);
		  }
	      fps=atoi(argv[i]);
	      i++;
		  (&glob)->microsperframe=100000l/fps;
	  }
	  if (!strcmp("-color",argv[i-1])) {
	      if ((i+1)>argc) {
		      fewargs();
		      exit(1);
		  }
		  strcpy(colorspec,argv[i]);
		  useDefColor=0;
	      i++;
	  }
	}
    }
  if (!OpenDisp(&glob,disp))
    {
      printf("Could not open Display %s\n",disp);
      exit(1);
    }
  if (flags&ROOT)
    {
      GetRootWindow(&glob);
      MakeGC(&glob);
    }
  else
    {
      if (!OpenWindow(&glob,width,height,title))
	{
	  printf("Could not create a GC for the window!\n");
	  exit(1);
	}
    }
  if (useDefColor==1) {
	(&glob)->RBASE=255;
	(&glob)->GBASE=255-80;
	(&glob)->BBASE=255-160;
  } else {
  	status=XParseColor(
			  (&glob)->disp,
			  XCreateColormap((&glob)->disp,DefaultRootWindow((&glob)->disp),(&glob)->vis,AllocNone),
			  colorspec,
			  &basecolor);
	  if (status==0) {
		  printf("Invalid color specification %s\n",colorspec);
		  exit(1);
	  }
	  (&glob)->RBASE=((&basecolor)->red)>>8;
	  (&glob)->GBASE=((&basecolor)->green)>>8;
	  (&glob)->BBASE=((&basecolor)->blue)>>8;
  }
  
  /* if the user requested a CLEAN display method, set the window up */
  /* accordingly with backing store, saveunders etc. */

  /* Set up the palette for the flame according to user flags */
  if ((&glob)->vis->class==TrueColor)
    {
	  (&glob)->carray=carray;
      InitColorArray(&glob);
    }
   else
    {
      SetFlamePalette(&glob,flags,colortab);
    }
  /* Check the dimensions of the window (in case it didn't end up being the */
  /* size we requested) */
  width=GetWinWidth(&glob)+2;
  height=GetWinHeight(&glob)+2;
  /* Allocate ourselves a buffered image */
  if (!SetupXImage(&glob,width,height))
    {
      printf("Could not allocate memory for image buffer!\n");
      exit(1);
    }
  /* Start displaying the flame!*/
  if (!Xflame(&glob,width,height,flags,colortab))
    {
      /* if Xflame returned 0, it encountered an error in allocating memory */
      printf("Not enough memory to allocate to the flame arrays\n");
      exit(1);
    }
}
