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

/* 
Enhancements to #I format added by Paul Callahan (callahan@cs.jhu.edu).
New format is backward compatible with old format.
*/

#include "defs.h"
#include "data.h"
#include "cellbox.h"
#include <stdio.h>
#include <pwd.h>
#include <ctype.h>

extern cellbox *boxes[];

/* global pointer to tentative population */
cellbox *tentative;
/* temporary hash table */
cellbox *tempboxes[HASHSIZE];

/* pattern name of tentative population */
char tentpat[PATNAMESIZ];
/* global header cell for load script */ 
LoadReq *loadscript;
 
/* matrix for rotations */
static int rotxx[]={1, 0,-1, 0}; 
static int rotxy[]={0,-1, 0, 1}; 
static int rotyx[]={0, 1, 0,-1}; 
static int rotyy[]={1, 0,-1, 0}; 

char *gethome()
{
    return(getpwuid(getuid())->pw_dir);
}


char *checktilda(stng)
char *stng;
{
    static char full[1024];
    struct passwd *pw;
    int i;
    if (stng[0]=='~') {
	i=1;
	while(stng[i]!='/' && stng[i]!='\0')
	    full[i-1]=stng[i], i++;
	if (i==1) 
	    strcpy(full,gethome());
	else {
	    full[i-1]='\0';
	    if((pw = getpwnam(full)) == NULL){
		XClearWindow(disp,inputw);
                strcpy(inpbuf,SYSERR);
		XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),inpbuf,strlen(inpbuf));
	    }
	    else{
		strcpy(full,pw->pw_dir);
	    }
	}
	strcat(full,stng+i);
    } else
	strcpy(full,stng);
    return(full);
}

char *addlifeext(buf)
char *buf;
{
    int len=strlen(buf);
    if(strcmp(buf+len-5,".life")){
	return(".life");
    }
    return("");
}

void savefile(){

    FILE *savefl;
    char outbuf[100];
    
    strcpy(inpbuf,"Save to:");
    minbuflen=8;
    strcat(inpbuf, "./");
    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));

    getxstring();

    strcpy(outbuf,checktilda(inpbuf+8));
    inpbuf[0]=0;
    strcat(outbuf,addlifeext(outbuf));
    if((savefl=fopen(outbuf,"w")) != NULL){
	saveall(savefl, '\0');
	fclose(savefl);
    }
    else{
	XClearWindow(disp,inputw);
        strcpy(inpbuf,SYSERR);
	XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),inpbuf,strlen(inpbuf));
    }
}

char *seppatdir(filename,filefield) 
char *filename,*filefield;
{
char *ffptr,*colptr;
/* Separates path from rest of pattern's name.  */

   /* Make sure "/" in pattern block name isn't confusing by temporarily
      eliminating ":" suffix */
   if (colptr=strchr(filename,':')) (*colptr)='\0';

   ffptr=strrchr(filename,'/');
   if (ffptr) {
      strcpy(filefield,ffptr+1);
      *(ffptr+1)='\0';
   }
   else {
      strcpy(filefield,filename);
      *(filename)='\0';
   } 

   /* restore ":" suffix */
   if (colptr) (*colptr)=':';
}

/* Krufty Kode (TM) to get around problems with global variables */

#define SAVEGLOB \
cellbox *tmphead=head; \
unsigned long tmpnb=numboxes,tmpnc=numcells,tmpg=generations

#define RESTOREGLOB \
head=tmphead; \
numboxes=tmpnb; numcells=tmpnc; generations=tmpg


/* procedure to swap hash tables between tentative and real populations */
swaphash() 
{
cellbox **ptr,**tentptr;
cellbox *tmp;

ptr=boxes;
tentptr=tempboxes;
while(ptr!=boxes+HASHSIZE) {
    tmp= *ptr;
    (*ptr++)= *tentptr;
    (*tentptr++)=tmp;
}

}


int breakreq()
{
XComposeStatus stat;
int breakval=0;

    if (XCheckMaskEvent(disp,KeyPressMask,&event)) {
         XLookupString(&event.xkey, keybuf, 16, &ks, &stat);
         breakval=(keybuf[0]=='X');
         keybuf[0]='\0';
    } 
    return breakval;
}

#define divis(x,y) (((long)((x)/(y)))*(y)==(x)) 

do_loadreq(loadqueue)
LoadReq *loadqueue;
{
int quitload=0;
char errstr[80],thispat[PATNAMESIZ],badpat[PATNAMESIZ];
SAVEGLOB; /* save globals in order to work on temporary population */


   /* Save name of original file to be loaded (for load script) */
   if (strncmp(loadqueue->patname,DIR,strlen(DIR))) {
       strcpy(tentpat,loadqueue->patname);
   }
   else {
       strcpy(tentpat,loadqueue->patname+strlen(DIR));
   }
   /* start with empty population */
   head=NULL; 
   numboxes=numcells=0;
   generations=0;

   swaphash();

   /* clear tentative hash table */
   initcells();   

   badpat[0]='\0';
   while(loadqueue!=NULL) {
      while(loadqueue->loadtime > generations) {
           if (divis(loadqueue->loadtime-generations,100)) {
               sprintf(inpbuf,
                      "Generating: %d steps left (type X to halt)",
                       loadqueue->loadtime-generations);
                XClearWindow(disp,inputw);
                XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),
                            inpbuf, strlen(inpbuf));
           }
           generate();
           if (quitload=breakreq()) break;
      }
      strcpy(thispat,loadqueue->patname+loadqueue->relpath);
      sprintf(inpbuf,
             "Loading %s (type X to halt)",thispat);
      XClearWindow(disp,inputw);
      XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),
                  inpbuf, strlen(inpbuf));
      if (!do_loadfile(&loadqueue)) {
          strcpy(badpat,thispat);
      }
      if (quitload = (quitload || breakreq())) break;
   }
   XClearWindow(disp,inputw);

   /* set global tentative info */
   tentative=head;
   tentnumboxes=numboxes;
   tentnumcells=numcells;
   tentgenerations=generations;
 
   swaphash();
   
RESTOREGLOB; /* restore globals */

   XClearWindow(disp,lifew);
   redrawscreen();

   /* echo name of last pattern that couldn't be read */
   if (strlen(badpat)) { 
     strcpy(inpbuf,"Can't load ");
     strcat(inpbuf,badpat);
     XClearWindow(disp,inputw);
     XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),inpbuf,strlen(inpbuf));
   }
}

int add_loadreq(loadqueue, loadtime, patname, relpath, 
                 hotx, hoty, rxx, rxy, ryx, ryy)
    LoadReq **loadqueue;
    long loadtime;
    unsigned long hotx,hoty;
    char *patname;
    int relpath;
    int rxx, rxy, ryx, ryy;
{
  LoadReq *newreq;

  /* Find first entry where request can go while maintaining time order.
     A heap would have better theoretical time complexity, of course, but
     in practice, the list probably won't get too long.   

     After loop, we have a pointer to the pointer that must be modified
     (Could be pointer to list itself). */ 

  while ((*loadqueue)!=NULL && ((*loadqueue)->loadtime < loadtime)) 
     loadqueue= &((*loadqueue)->next); 

  /* Create new request block and load it up */
  newreq=(LoadReq *)malloc(sizeof(LoadReq)); 
  newreq->loadtime=loadtime;
    /* Silently truncates long file names--should probably tell user */
  strncpy(newreq->patname,patname,PATNAMESIZ);
  newreq->relpath=relpath;
  newreq->hotx=hotx;
  newreq->hoty=hoty;
  newreq->rxx=rxx;
  newreq->rxy=rxy;
  newreq->ryx=ryx;
  newreq->ryy=ryy;

  /* Insert new request in queue */
  newreq->next=(*loadqueue);
  (*loadqueue)=newreq; 
}

void parse_patname(patname,patfield)
char *patname,*patfield;
{
char *pfptr;
/* Breaks "<filename>:<pattern>" into two strings. */

   pfptr=strchr(patname,':');
   if (pfptr) {
       *pfptr='\0';
       strcpy(patfield,pfptr+1);
   }
   else patfield[0]='\0';
}


int do_loadfile(loadqueue)
    LoadReq **loadqueue;
{
    char patname[PATNAMESIZ],patfield[PATNAMESIZ],tmpstring[PATNAMESIZ];
    unsigned long hotx,hoty;
    int relpath, rxx, rxy, ryx, ryy;
    FILE *loadfl;
    char patfile[BUFSIZ];
    LoadReq *tmpqptr;

    int x,y, linect = 0;
#define M_ABSOLUTE	0
#define M_RELATIVE	1
#define M_PICTURE	2
    int loadmode = M_ABSOLUTE;

/* Get load request */
    strcpy(patname,(*loadqueue)->patname);
    relpath=(*loadqueue)->relpath;
    hotx=(*loadqueue)->hotx;
    hoty=(*loadqueue)->hoty;
    rxx=(*loadqueue)->rxx;
    rxy=(*loadqueue)->rxy;
    ryx=(*loadqueue)->ryx;
    ryy=(*loadqueue)->ryy;

/* Delete request from queue */
    tmpqptr=(*loadqueue)->next; 
    free(*loadqueue);
   (*loadqueue)=tmpqptr;

    /* separate filename from pattern name */
    parse_patname(patname,patfield);

    /* add .life to filename if needed */
    (void) strcat(patname, addlifeext(patname));

    /* open file */
    strcpy(patfile,patname);
    if ((loadfl=fopen(patfile,"r")) == NULL)
      {
       /* look for included pattern in pattern directory 
          if not found in given directory */
        strcpy(patfile,DIR);
        strcat(patfile,patname+relpath);
        if ((loadfl=fopen(patfile,"r")) == NULL) {
           /* if all else fails, try current directory */
           strcpy(patfile,patname+relpath);
           loadfl=fopen(patfile,"r");
        };
      }

    if (loadfl!=NULL)
    {
	char	buf[BUFSIZ],patid[PATNAMESIZ];
	int	xoff = 0, yoff = 0;
        int     endpattern=0,found=0;

        /* If we are searching for a specific pattern in the file,
           then we skip lines till we find it.  This is a pretty tacky way
           to handle multiple pattern definitions in the same file,
           but files with #I format probably won't get big enough for
           anyone to notice.  Really, we should implement a dictionary to
           save the location of a pattern definition in a file the first time 
           we see it. 
        */
        if (patfield[0]!='\0') { 
	   while (!found && fgets(buf, BUFSIZ, loadfl) != (char *)NULL) { 
              if (buf[0]=='#' && buf[1]=='B') {
		 (void) sscanf(buf+2, " %s", patid); 
                 found=!strcmp(patfield,patid);
              } 
           }
           if (!found) {
	      fclose(loadfl);
              return 0; 
           }
        }
	while (fgets(buf, BUFSIZ, loadfl) != (char *)NULL && !endpattern)
	{
	    if (buf[0] == '#')
	    {
		char	incl[BUFSIZ];
		char	pardir[PATNAMESIZ];
		int	lx, ly, rotate, flip, relpathoffset=0;
		long loadtime;

		incl[0] = '\0';
		switch(buf[1])
		{
                  case 'B':
                    /* Anything between #B and #E is ignored, since it
                       is a pattern definition to be included later.
                       #B and #E cannot be nested, so we just skip till
                       we pass #E */
	            while (fgets(buf, BUFSIZ, loadfl) != (char *)NULL
                           && !(buf[0]=='#' && buf[1]=='E')) {}
                    break;

                  case 'E': 
                    /* if we hit a "#E" at this point, then it must be
                       the one that closes the "#B <patfield>" that we
                       are reading (assuming a syntactically correct file) */
                    endpattern=1;
                    break;

		  case 'N':
		    (void) strcat(fname,buf+2);
		    fname[strlen(fname)-1]=0;
		    break;

		  case 'A':
		    loadmode = M_ABSOLUTE;
		    break;
		    
		  case 'R':
		    loadmode = M_RELATIVE;
		    xoff = yoff = 0;
		    (void) sscanf(buf+2, " %d %d", &xoff, &yoff);
		    break;
		    
		  case 'P':
		    loadmode = M_PICTURE;
		    xoff = yoff = 0;
		    (void) sscanf(buf+2, " %d %d", &xoff, &yoff); 
                    linect=0;
		    break;
		    
		  case 'I':
		    xoff = yoff = rotate = loadtime = 0;
                    flip = 1;
		    (void) sscanf(buf+2, " %s %d %d %d %d %d", incl, 
                                 &xoff, &yoff, &rotate, &flip, &loadtime); 
                    /* Silently ignore invalid flips */
                    if ((flip!=1) && (flip!= -1)) flip=1;

                    /* if included pattern begins with ':' then assume
                       it's in current file */
                    if (incl[0]==':') {
                       strcpy(tmpstring,patfile);
                       strcat(tmpstring,incl);
                       strcpy(incl,tmpstring);
                    } else {
                       /* if relative path given, assume directory of parent */
                       if (!strchr("/~",incl[0])) {
                          strcpy(pardir,patfile);
                          seppatdir(pardir,tmpstring);
                          relpathoffset=strlen(pardir);
                          strcat(pardir,incl);
                          strcpy(incl,pardir);
                       }
                    } 
                    add_loadreq(
                       loadqueue, generations+loadtime, incl, relpathoffset, 
                       hotx + tx(xoff,yoff,rxx,rxy), 
                       hoty + ty(xoff,yoff,ryx,ryy),
                       rxx*rotxx[mod(rotate,4)]+rxy*rotyx[mod(rotate,4)],
                       flip*(rxx*rotxy[mod(rotate,4)]+rxy*rotyy[mod(rotate,4)]),
                       ryx*rotxx[mod(rotate,4)]+ryy*rotyx[mod(rotate,4)],
                       flip*(ryx*rotxy[mod(rotate,4)]+ryy*rotyy[mod(rotate,4)])
                     );
		    break;
		    
		  case 'L':
		    (void) sscanf(buf+2, " %d %d %[^\n]", &lx, &ly, incl); 
		    XDrawString(disp, lifew, ntextgc,
				RXPOS(xoff+lx, xpos), RYPOS(yoff+ly, ypos),
				incl, strlen(incl));
		    break;

		    
		  default:
		    break;
		}
	    }
	    else if (loadmode == M_ABSOLUTE && sscanf(buf,"%d %d\n",&x,&y)==2)
	    {
		addcell(xpos + tx(x,y,rxx,rxy), 
                          ypos + ty(x,y,ryx,ryy));
		numcells++;
	    }
	    else if (loadmode == M_RELATIVE && sscanf(buf,"%d %d\n",&x,&y)==2)
	    {
		addcell(hotx + tx(xoff+x,yoff+y,rxx,rxy), 
                        hoty + ty(xoff+x,yoff+y,ryx,ryy));
		numcells++;
	    }
	    else /* loadmode == M_PICTURE */
	    {
		char	*cp;

		for (cp = buf; *cp; cp++)
		    if (*cp == '*')
		    {
			addcell(hotx + tx(xoff+cp-buf, yoff+linect,rxx,rxy),
		                hoty + ty(xoff+cp-buf, yoff+linect,ryx,ryy));
			numcells++;
		    }
	    }
	    linect++;
	}
	fclose(loadfl);
	return 1;
   } else return 0;
}


get_rot_flip(rxx,rxy,ryx,ryy,rot,flip) 
int rxx,rxy,ryx,ryy,*rot,*flip;
{
/* compute rotation and flip from given transformation */
/* No fancy mathematics; just search table of rotations */ 
for (*rot=0; *rot<4; (*rot)++ ) {
    if ((rxx==rotxx[*rot])&&(ryx==rotyx[*rot])) {
         if ((rxy== -rotxy[*rot]) && (ryy== -rotyy[*rot])) *flip= -1;
         else *flip=1;
         break;
    }
}

}

void free_loadscript() {
/* reinitialize load script to one self-connected cell */
LoadReq *nptr,*ptr;

    for(ptr=loadscript->next; ptr!=loadscript;) {
         nptr=ptr->next;
         free(ptr);
         ptr=nptr;
    }
    loadscript->next=loadscript; 

}

void saveloadscript(){

    FILE *savefl;
    char outbuf[100];
    static char savemsg[]="Save load script to:";
    static char *ans;
    int rot,flip;  
    long mingener;
    unsigned long savex,savey;
    LoadReq *ptr;

    /* Confirm latest load before saving script */
    confirmload(); 
    XClearWindow(disp,lifew);
    redrawscreen();

    /* get name of file to write into into */
    strcpy(inpbuf,savemsg);
    minbuflen=strlen(savemsg);
    strcat(inpbuf, "./");
    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));

    getxstring();
    XClearWindow(disp,inputw);

    savex=XPOS(event.xmotion.x,xpos);
    savey=YPOS(event.xmotion.y,ypos);

    strcpy(outbuf,checktilda(inpbuf+strlen(savemsg)));
    inpbuf[0]=0;
    strcat(outbuf,addlifeext(outbuf));
    if((savefl=fopen(outbuf,"w")) != NULL){
       /* write load script */ 
       /* first, find minimum generation at which a pattern is
          to be included so include times can be normalized */
       mingener=generations;
       for(ptr=loadscript->next; ptr!=loadscript;) {
          ptr=ptr->next;
          if (mingener>ptr->loadtime) mingener=ptr->loadtime; 
       }
       /* now output file */
       for(ptr=loadscript->next; ptr!=loadscript;) {
          ptr=ptr->next;
          get_rot_flip(ptr->rxx,ptr->rxy,ptr->ryx,ptr->ryy,&rot,&flip);
          fprintf(savefl,"#I %s %d %d %d %d %d\n",
                  ptr->patname, 
                  ptr->hotx-savex, 
                  ptr->hoty-savey, 
                  rot,flip, ptr->loadtime-mingener);
       }

       /* close file */
       fclose(savefl);
    }
    else{
	XClearWindow(disp,inputw);
        strcpy(inpbuf,SYSERR);
	XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),inpbuf,strlen(inpbuf));
    }
}

add_include_entry(patname,loadtime,hotx,hoty,xx,xy,yx,yy)
char *patname;
long loadtime;
unsigned long hotx,hoty;
int xx,xy,yx,yy;
{
LoadReq *newreq;

  /* Create new include block and load it up */
  newreq=(LoadReq *)malloc(sizeof(LoadReq)); 
  strncpy(newreq->patname,patname,PATNAMESIZ);
  newreq->loadtime=loadtime;
  newreq->hotx=hotx;
  newreq->hoty=hoty;
  newreq->rxx=xx;
  newreq->rxy=xy;
  newreq->ryx=yx;
  newreq->ryy=yy;

  /* Insert new request at end of list */
  newreq->next= loadscript->next;
  loadscript->next= newreq; 
  loadscript=newreq;

}


confirmload() {
cellbox *ptr,*nptr;
int dx,dy;
long x,y;

   if (tentative!=NULL) {
        /* add completed load to load script */
       add_include_entry(tentpat,generations-tentgenerations,
                         loadx,loady,txx,txy,tyx,tyy);
   }
   /* officially add loaded cells with given transformations */
        ptr=tentative;
        while(ptr != NULL) {
	    if (!ptr->dead)
		for (dx = 0; dx < BOXSIZE; dx++)
		    for (dy = 0; dy < BOXSIZE; dy++)
			if (getcell(ptr, dx, dy)) {
			   x=ptr->x+dx-STARTPOS;
                           y=ptr->y+dy-STARTPOS;
                           addcell(tx(x,y,txx,txy)+loadx,
                                   ty(x,y,tyx,tyy)+loady);
                         }
            nptr = ptr->next;
            free(ptr); 
            ptr=nptr;
         }
        tentative=NULL; 
}

undoload() {
cellbox *ptr,*nptr;
   /* get rid of loaded cells */
        ptr=tentative;
        while(ptr != NULL) {
            nptr = ptr->next;
            free(ptr); 
            ptr=nptr;
         }
        tentative=NULL; 
}

genload() {
  /* perform some generations on loaded cells */
static char genmsg[]="Number of generations to perform (default=1): ";
int gens=1;
SAVEGLOB; /* save globals in order to work on temporary population */

   strcpy(inpbuf,genmsg);
   minbuflen=strlen(genmsg);
   XClearWindow(disp,inputw);
   XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));
   getxstring();
   XClearWindow(disp,inputw);
   sscanf(inpbuf+strlen(genmsg),"%d",&gens);
   inpbuf[0]='\0';

   /* pass tentative info via globals */
   head=tentative; 
   numboxes=tentnumboxes; 
   numcells=tentnumcells;
   generations=tentgenerations;

   swaphash();
   while (gens-- > 0) {
      if (divis(gens,100)) {
         sprintf(inpbuf,
                "Generating: %d steps left (type X to halt)",gens);
                 XClearWindow(disp,inputw);
                 XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),
                             inpbuf, strlen(inpbuf));
      }
      generate();     /* Parameters?  Never heard the word. */
      if (breakreq()) break;
   }
   swaphash();

   /* pass info back to tentative globals */
   tentative=head; 
   tentnumboxes=numboxes; 
   tentnumcells=numcells;
   tentgenerations=generations;

RESTOREGLOB; /* restore globals */
}

void loadfile(){

    char outbuf[100],tmp[PATNAMESIZ];
    LoadReq *loadqueue=NULL;
  
    confirmload(); /* confirm any previously loaded cells */ 
    XClearWindow(disp,lifew);
    redrawscreen();

    strcpy(inpbuf,"Load from:");
    minbuflen=10;
    strcat(inpbuf,loadirbuf);
    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));

    getxstring();
       
    strcpy(outbuf,checktilda(inpbuf+10));

    /* redefine user load directory (but not pattern library) */
    strcpy(loadirbuf,outbuf);
    seppatdir(loadirbuf,tmp);

    inpbuf[0]=0; 
    /* don't need to add life extension now, since it will be added in
       do_loadfile  */

    loadx=XPOS(event.xmotion.x, xpos);
    loady=YPOS(event.xmotion.y, ypos);
    txx=1;
    txy=0;
    tyx=0;
    tyy=1; 
    add_loadreq(&loadqueue, 0, outbuf, 0, 
                STARTPOS, STARTPOS, 1,0,0,1); 
    do_loadreq(loadqueue);
/*		    scale = 1;	*/
   
}
			
