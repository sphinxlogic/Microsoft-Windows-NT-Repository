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

#include "defs.h"
#include "data.h"
#include "cellbox.h"
#include "stdio.h"

int getcell(ptr, xdx, ydx)
/* get state of cell xdx, ydx within box *ptr */
cellbox	*ptr;
int xdx, ydx;
{
    if (ydx > 3)
	return(ptr->live2 & 1 << ( (ydx - 4)*8  + xdx));
    else
	return(ptr->live1 & 1 << ( ydx*8  + xdx));
}

void setcell(ptr, xdx, ydx, val)
/* set state of cell xdx, ydx within box *ptr */
cellbox	*ptr;
int xdx, ydx, val;
{
    if (val)
    {
	if (ydx > 3)
	    ptr->live2 |= 1 << ( (ydx - 4)*8  + xdx);
	else
	    ptr->live1 |= 1 << ( ydx*8  + xdx);
    }
    else
    {
	if (ydx > 3)
	    ptr->live2 &= 0xffffffff^(1 << ( (ydx - 4)*8  + xdx));
	else
	    ptr->live1 &= 0xffffffff^(1 << ( ydx*8  + xdx));
    }
}

void forget(ptr)
/* remove a box's info about last generation's state */
cellbox *ptr;
{
    ptr->olive1 ^= ptr->olive1;
    ptr->olive2 ^= ptr->olive2;
}

static void displayline(line,x,y,oline)
/* post changes in a half-cell to the X structure arrays */
unsigned long line,x,y,oline;
{
    int sc,yy;
    unsigned long diff;
    sc = (1 << (scale -1)) - (scale > 2);
    diff=line ^ oline;
    if (scale == 1){
	if(diff & 0x1){
	    if(line & 0x1){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x2){
	    if(line & 0x2){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x4){
	    if(line & 0x4){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x8){
	   if(line & 0x8){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x10){
	    if(line & 0x10){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x20){
	    if(line & 0x20){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x40){
	    if(line & 0x40){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;	
	if(diff & 0x80){
	    if(line & 0x80){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
    }
    else{
	yy=((y - ypos) << (scale -1));
	if(diff & 0x1){
	    if(line & 0x1){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x2){
	    if(line & 0x2){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x4){
	    if(line & 0x4){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x8){
	    if(line & 0x8){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x10){
	    if(line & 0x10){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x20){
	    if(line & 0x20){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x40){
	    if(line & 0x40){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x80){
	    if(line & 0x80){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x  - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
    }
	
}

void displaybox(x, y, ptr)
unsigned long x, y;
cellbox	*ptr;
{
    register unsigned long live1 = ptr->live1;
    register unsigned long live2 = ptr->live2;
    register unsigned long olive1 = ptr->olive1;
    register unsigned long olive2 = ptr->olive2;

    displayline(live1,x,y,olive1);
    displayline(live1>>8,x,++y,olive1>>8);
    displayline(live1>>16,x,++y,olive1>>16);
    displayline(live1>>24,x,++y,olive1>>24);
    displayline(live2,x,++y,olive2);
    displayline(live2>>8,x,++y,olive2>>8);
    displayline(live2>>16,x,++y,olive2>>16);
    displayline(live2>>24,x,++y,olive2>>24);
    
}


static void trdisplayline(line,x,y,oline)
/* post changes in a half-cell to the X structure arrays */
/* use global geometric transformations */
unsigned long line,oline;
long x,y;
{
    int sc,yy;
    unsigned long diff;
    sc = (1 << (scale -1)) - (scale > 2);
    diff=line ^ oline;
    if (scale == 1){
	if(diff & 0x1){
	    if(line & 0x1){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x2){
	    if(line & 0x2){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x4){
	    if(line & 0x4){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x8){
	   if(line & 0x8){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x10){
	    if(line & 0x10){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x20){
	    if(line & 0x20){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x40){
	    if(line & 0x40){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
	++x;	
	if(diff & 0x80){
	    if(line & 0x80){
		onpoints[onpt].x= tx(x,y,txx,txy)+loadx-xpos;
		onpoints[onpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= tx(x,y,txx,txy)+loadx-xpos;
		offpoints[offpt].y= ty(x,y,tyx,tyy)+loady-ypos;
		offpt++;
	    }
	}
    }
    else{
	if(diff & 0x1){
	    if(line & 0x1){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x2){
	    if(line & 0x2){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x4){
	    if(line & 0x4){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x8){
	    if(line & 0x8){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x10){
	    if(line & 0x10){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x20){
	    if(line & 0x20){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x40){
	    if(line & 0x40){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x80){
	    if(line & 0x80){
		onrects[onrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		onrects[onrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((tx(x,y,txx,txy)+loadx-xpos) << (scale -1));
		offrects[offrect].y= ((ty(x,y,tyx,tyy)+loady-ypos) << (scale -1));
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
    }
	
}

void trdisplaybox(x, y, ptr)
long x, y;
cellbox	*ptr;
{
    register unsigned long live1 = ptr->live1;
    register unsigned long live2 = ptr->live2;
    register unsigned long olive1 = ptr->olive1;
    register unsigned long olive2 = ptr->olive2;

    trdisplayline(live1,x,y,olive1);
    trdisplayline(live1>>8,x,++y,olive1>>8);
    trdisplayline(live1>>16,x,++y,olive1>>16);
    trdisplayline(live1>>24,x,++y,olive1>>24);
    trdisplayline(live2,x,++y,olive2);
    trdisplayline(live2>>8,x,++y,olive2>>8);
    trdisplayline(live2>>16,x,++y,olive2>>16);
    trdisplayline(live2>>24,x,++y,olive2>>24);
    
}


