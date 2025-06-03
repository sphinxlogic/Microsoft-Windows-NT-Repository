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
 * This module encapsulates cell-box handling. Everything else in the program
 * except possibly the evolution function should go through these functions
 * to change the board state. Entry points are:
 *
 * void initcells()   -- initialize board
 *
 * void addcell(x, y) -- set cell on
 *
 * void deletecell(x, y) -- set cell off
 *
 * void center() -- center visible board on barycenter of pattern
 *
 * void clear() -- blank the board, freeing all storage
 *
 * void redisplay() -- update visible display of current board
 *
 * void redrawscreen() -- redraw display, assume nothing about previous state
 *
 * void saveall() -- save entire board state to file
 *
 * This code knows that cells are packed in 8x8 cell boxes on a hash list, but
 * it doesn't know anything about the internals of cell representation within
 * cell boxes. It relies on the existence of a triad of functions
 *
 * getcell(ptr, dx, dy)      -- get state of cell at x, y in box *ptr
 * setcell(ptr, dx, dy, val) -- set cell at x, y in box *ptr to state val
 * forget(ptr)               -- cause a box to forget last generation's state
 *
 * to access cell state. It also relies on the existence of a function
 *
 * displaybox(x, y, ptr) -- post box state to Xrect arrays
 */

#include "defs.h"
#include "data.h"
#include "cellbox.h"
#include <pwd.h>

extern char *ctime();
extern cellbox *tentative;

#define HASH(x,y) 	(((x>>3) & 0x7f)<< 7) + ((y>>3) & 0x7f)
/* #define BOXSIZE		8  (now in cellbox.h) */
#define MARK		'*'
#define SPACE		'.'
#define	USL_MAX		4294967295	/* max decimal value of "unsigned" */

#ifndef NULL
#define NULL 0
#endif
 
cellbox *head;
cellbox *freep;
cellbox *boxes[HASHSIZE];
XPoint onpoints[MAXON],offpoints[MAXON];
XRectangle onrects[MAXOFF],offrects[MAXOFF];
 
void initcells()
/* initialize the cell hash list */
{
int i;

  bzero(boxes,HASHSIZE*sizeof(cellbox *));   

}

void addcell(x,y)
/* turn a cell to state ON */
unsigned long x,y;
{
    unsigned long ydx,xdx;
    cellbox *ptr;

    ptr = link(x,y);
    ydx = y - ptr->y;
    xdx = x - ptr->x;
    ptr->dead=0;

    setcell(ptr, xdx, ydx, 1);
}

void deletecell(x,y)
/* turn a cell to state OFF */
unsigned long x,y;
{
    unsigned long ydx,xdx;
    cellbox *ptr;
    
    ptr = link(x,y);
    ydx = y - ptr->y;
    xdx = x - ptr->x;

    setcell(ptr, xdx, ydx, 0);
}

void kill(ptr)
cellbox *ptr;
{
    unsigned long hv=HASH(ptr->x,ptr->y);

    if(ptr != head){
	ptr->fore->next=ptr->next;
    }
    else{
	head = ptr->next;
    }
    if(ptr == boxes[hv]){
	boxes[hv] = ptr->hnext;
    }
    else{
	ptr->hfore->hnext=ptr->hnext;
    }
    if(ptr->next) ptr->next->fore=ptr->fore;
    if(ptr->hnext) ptr->hnext->hfore=ptr->hfore;
    if(ptr->rt) ptr->rt->lf=NULL;
    if(ptr->lf) ptr->lf->rt=NULL;
    if(ptr->up) ptr->up->dn=NULL;
    if(ptr->dn) ptr->dn->up=NULL;
    ptr->next=freep;
    freep=ptr;
    numboxes--;
}

cellbox *create(x,y,hv)
unsigned long x,y,hv;
{
    cellbox *ptr;

#ifdef PROF
    create_called++;
#endif PROF

    if(freep == NULL){
#ifdef PROF
	create_null++;
#endif PROF
	if((ptr= (cellbox *)malloc(sizeof(cellbox))) ==NULL){
	    perror("create: malloc error: ");
	    exit(-1);
	}
    }
    else{
	ptr=freep;
	freep=freep->next;
    }
    bzero(ptr,sizeof(cellbox));
    
    ptr->next=head;
    head=ptr;
    
    if(ptr->next != NULL){
	ptr->next->fore=ptr;
    }	
    ptr->hnext=boxes[hv];
    boxes[hv]= ptr;
    
    if(ptr->hnext != NULL){
	ptr->hnext->hfore=ptr;
    }
    ptr->x = x;
    ptr->y = y;
    numboxes++;
    return(ptr);
}


cellbox *link(x,y)
unsigned long x,y;
{
    cellbox *ptr;
    unsigned long hv;    
    
    x &= 0xfffffff8;
    y &= 0xfffffff8;
    hv=HASH(x,y);
    ptr = boxes[hv]; 
#ifdef PROF
    link_called++;
#endif PROF
    if(freep){
	if (ptr==NULL){
	    ptr=freep;
	    boxes[hv]= ptr;
	    freep=freep->next;
	    bzero(ptr,sizeof(cellbox));
	    ptr->x = x;
	    ptr->y = y;
	    ptr->next=head;
	    head=ptr;    

	    if(ptr->next){
		ptr->next->fore=ptr;
	    }	
	    numboxes++;
	    return(ptr);
	}
	else{
	    do{
#ifdef PROF
		link_search++;
#endif PROF
		if ((ptr->x == x) && (ptr->y == y)){
		    return(ptr);
		}
		ptr=ptr->hnext;
	    }while(ptr!=NULL);
	    
	    return(create(x,y,hv));
	}
    }
    else{
	if (ptr==NULL) return(create(x,y,hv));
	do{
#ifdef PROF
	    link_search++;
#endif PROF
	    if ((ptr->x == x) && (ptr->y == y)){
		return(ptr);
	    }
	    ptr=ptr->hnext;
	}while(ptr!=NULL);
	
	return(create(x,y,hv));
    }
}


void displaystats()
{
    if(dispcells){
	sprintf(inpbuf,"Generations: %6lu, Boxes: %6lu, Cells: %7lu ",generations,numboxes,numcells);
    }
    else{	
	sprintf(inpbuf,"Generations: %6lu, Boxes: %6lu",generations,numboxes);
    }
    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));

}

void newrules()
{
    int i,k;
    char *ptr;
    
    ptr=inpbuf;
    strcpy(inpbuf,"Rules:   ");
    ptr=inpbuf+7;
    k=live;
    for(i=0; i<9; i++)
    {	if(k&1)
	{   *ptr=i+48;
	    ptr++;
	}
	k>>=1;
    }
    *ptr='/';
    ptr++;
    k=born;
    for(i=0; i<9; i++)
    {	if(k&1)
	{   *ptr=i+48;
	    ptr++;
	}
	k>>=1;
    }
    strcpy(ptr,"   New Rules:  ");
    minbuflen=strlen(inpbuf);
    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));
    
    getxstring();
    inpbuf[0]=0;
    
    k=0;
    ptr=inpbuf+minbuflen;
    while((*ptr>47)&&(*ptr<58))
    {   k|=(1<<(*ptr-48));
	ptr++;
    }
    live=k;
    k=0;
    if(*ptr=='/')
    {   ptr++;
	while((*ptr>47)&&(*ptr<58))
	{   k|=(1<<(*ptr-48));
	    ptr++;
	}
	born=k;
    }
    XClearWindow(disp,inputw);
    gentab();
}



void center()
/* center the display on the `center of mass' of the live boxes */
{
    double x,y;
    int ctr=0;
    cellbox *ptr;
    x=0.0;
    y=0.0;
    XClearWindow(disp,lifew);
    for (ptr = head; ptr != NULL; ptr = ptr->next){
	if(!ptr->dead){
	    x+= ptr->x;
	    y+= ptr->y;
	    ctr++;
	}
    } 
    if (ctr>0) {
       x/=ctr;
       y/=ctr;
    }
    else {
       x=xorigin;
       y=yorigin;
    }
    xpos=x- (width >> scale);
    ypos=y- (height >> scale);
    redrawscreen();
}

void clear()
/* clear the board, freeing all storage */
{
    cellbox *ptr,*nptr;
   
    initcells();
    ptr=head;

    while(ptr){
	nptr=ptr->next;
	free(ptr);
	ptr=nptr;
    }
    /* free tentative pattern */
    ptr=tentative;
    while(ptr){
	nptr=ptr->next;
	free(ptr);
	ptr=nptr;
    }
    /* restart load script */
    free_loadscript();    

    head=tentative=NULL;
    state=STOP;
    generations=0;
    numboxes=0;
    numcells=0;
    XClearWindow(disp,lifew);
    XClearWindow(disp,inputw);
    displaystats();
}

#define ONSCREEN(x, y)	(((x > xpos-BOXSIZE) && (x < (xpos+BOXSIZE+(width >> (scale-1))))) && ((y > ypos-BOXSIZE) && (y < (ypos+BOXSIZE+(height >> (scale -1))))))

#define POINTS	4000	/* accumulate this many point changes before writing */
#define RECTS	64	/* accumulate this many box changes before writing */

void redisplay()
/* re-display the visible part of the board */
{
    cellbox *ptr;
    unsigned long x,y,ctr=0;
    long tentx,tenty;

    displaystats();    
    if(state==HIDE) return;
    
    onpt=offpt=0;
    onrect=offrect=0;
    
    for (ptr = head; ptr != NULL; ptr = ptr->next){
	x=ptr->x;
	y=ptr->y;
	if(ONSCREEN(x, y)){
	    displaybox(x, y, ptr);
	    ctr++;
	    if(scale == 1){
		if((onpt >= POINTS) || (offpt >= POINTS)){
		    XDrawPoints(disp,lifew,blackgc,offpoints,offpt,CoordModeOrigin);
		    XDrawPoints(disp,lifew,whitegc,onpoints,onpt,CoordModeOrigin);
		    onpt=offpt=0;
		    ctr=0;
		}
	    }
	    else{
		if(ctr == RECTS){
		    XFillRectangles(disp,lifew,blackgc,offrects,offrect);
		    XFillRectangles(disp,lifew,whitegc,onrects,onrect);
		    onrect=offrect=0;
		    ctr=0;
		}
	    }
	}
    }
    /* draw tentative points with appropriate transformation */
    for (ptr = tentative; ptr != NULL; ptr = ptr->next){
	tentx=ptr->x-STARTPOS;
	tenty=ptr->y-STARTPOS;
	if(ONSCREEN(tx(tentx,tenty,txx,txy)+loadx,
                    ty(tentx,tenty,tyx,tyy)+loady)) {
	    trdisplaybox(tentx, tenty, ptr);
	    ctr++;
	    if(scale == 1){
		if((onpt >= POINTS) || (offpt >= POINTS)){
		    XDrawPoints(disp,lifew,blackgc,offpoints,offpt,CoordModeOrigin);
		    XDrawPoints(disp,lifew,whitegc,onpoints,onpt,CoordModeOrigin);
		    onpt=offpt=0;
		    ctr=0;
		}
	    }
	    else{
		if(ctr == RECTS){
		    XFillRectangles(disp,lifew,blackgc,offrects,offrect);
		    XFillRectangles(disp,lifew,whitegc,onrects,onrect);
		    onrect=offrect=0;
		    ctr=0;
		}
	    }
	}
    }
    if(ctr){
	if(scale == 1){
	    XDrawPoints(disp,lifew,blackgc,offpoints,offpt,CoordModeOrigin);
    	    XDrawPoints(disp,lifew,whitegc,onpoints,onpt,CoordModeOrigin);
	    onpt=offpt=0;
	    ctr=0;
	}
	else{
	    XFillRectangles(disp,lifew,blackgc,offrects,offrect);
	    XFillRectangles(disp,lifew,whitegc,onrects,onrect);
	    onrect=offrect=0;
	    ctr=0;
	}
    }
    /* draw pivot of tentative points */
    if (tentative) drawpivot((loadx-xpos) << (scale -1 ),
                             (loady-ypos) << (scale -1 ));

    XFlush(disp);
}

void redrawscreen()
/* force redraw of entire board */
{
    cellbox *ptr;

    for (ptr = head; ptr != NULL; ptr = ptr->next)
	forget(ptr);
    for (ptr = tentative; ptr != NULL; ptr = ptr->next)
	forget(ptr);
    redisplay();
}

void saveall(ofp, mode)
/* save the entire board contents */
FILE	*ofp;
char	mode;
{
    cellbox *ptr;
    int dx, dy, val,x=0;
    unsigned long xmin, ymin, xmax, ymax, numcells,timeval;
    struct passwd *pw;
    char machine[80];

    if(fname[0] != 0){
	fprintf(ofp,"#N %s\n",fname);
    }
	
    timeval=time(0);
    gethostname(machine,80);
    if( (pw = getpwuid(getuid())) != NULL){
	fprintf(ofp,"#O %s \"%s\"@%s %s",pw->pw_name,pw->pw_gecos,machine,ctime(&timeval));
    }
    
    while(x < numcomments){
	fprintf(ofp,"#C %s \n",comments[x]);
	x++;
    }
	
    if (mode == 'A')	/* save in absolute mode */
    {
	fputs("#A\n", ofp);
	for (ptr = head; ptr != NULL; ptr = ptr->next)
	    if (!ptr->dead)
		for (dx = 0; dx < BOXSIZE; dx++)
		    for (dy = 0; dy < BOXSIZE; dy++)
			if (val = getcell(ptr, dx, dy))
			    (void) fprintf(ofp,
					   "%d %d\n", ptr->x+dx,ptr->y+dy);
	return;
    }

    /*
     * Otherwise, determine the bounding box of the live cells.
     */
    xmin = USL_MAX; ymin = USL_MAX; xmax = 0; ymax = 0; numcells = 0;
    for (ptr = head; ptr != NULL; ptr = ptr->next)
	if(!ptr->dead)
	    for (dx = 0; dx < BOXSIZE; dx++)
		for (dy = 0; dy < BOXSIZE; dy++)
		    if (getcell(ptr, dx, dy))
		    {
			numcells++;
			if (ptr->x+dx < xmin)
			    xmin = ptr->x+dx;
			if (ptr->y+dy < ymin)
			    ymin = ptr->y+dy;
			if (ptr->x+dx > xmax)
			    xmax = ptr->x+dx;
			if (ptr->y+dy > ymax)
			    ymax = ptr->y+dy;
		    }

    /* caller may want save to shortest format */
    if (mode == '\0')
	if (((ymax - ymin + 1) * (xmax - xmin + 1)) > numcells * 8)
	    mode = 'R';
	else
	    mode = 'P';

    /* now that we have the bounding box, we can gen the other formats */
    if (mode == 'R')
    {
	for (ptr = head; ptr != NULL; ptr = ptr->next)
	    if (!ptr->dead)
		for (dx = 0; dx < BOXSIZE; dx++)
		    for (dy = 0; dy < BOXSIZE; dy++)
			if (val = getcell(ptr, dx, dy))
			    (void) fprintf(ofp,
					   "%d %d\n",
					   ptr->x+dx-xmin, ptr->y+dy-ymin);
    }
    else
    {
	unsigned long x, y;

	(void) fprintf(ofp, "#P\n");
	for (y = ymin; y <= ymax; y++)
	{
	    for (x = xmin; x <= xmax; x++)
	    {
		cellbox *ptr;

		ptr = link(x,y);
		if (getcell(ptr, x - ptr->x, y - ptr->y) != 0)
		    (void) fputc(MARK, ofp);
		else
		    (void) fputc(SPACE, ofp);
	    }
	    (void) fputc('\n', ofp);
	}
    }
}

