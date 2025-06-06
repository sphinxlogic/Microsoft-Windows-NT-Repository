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
#include <stdio.h>

void name_file(){

    if(fname[0] != 0){
	sprintf(inpbuf,"Old Name: %s, New Name: ",fname);
	minbuflen=strlen(inpbuf);
    }
    else{
	strcat(inpbuf,"New Name: ");
	minbuflen=10;
    }

    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));

    getxstring();

    strcpy(fname,inpbuf+minbuflen);
    inpbuf[0]=0;
    
    XClearWindow(disp,inputw);
}

void comment(){

    minbuflen=9;

    for(;;){
	strcpy(inpbuf,"Comment: ");
	XClearWindow(disp,inputw);
	XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));

	getxstring();

	strcpy(comments[numcomments],inpbuf+minbuflen);
	
	if((comments[numcomments][0]==0)||(++numcomments > (MAXCOMMENTS -2))){
	    XClearWindow(disp,inputw);
	    inpbuf[0]=0;
	    return;
	}
    }
}

void view_comments(){

    int i;
    
    XClearWindow(disp,lifew);
    for(i=0;i < numcomments;i++){
	XDrawString(disp,lifew,ntextgc,10,i*12+25,comments[i],strlen(comments[i]));
    }
    getxstring();
    inpbuf[0]=0;
    XClearWindow(disp,lifew);
    XClearWindow(disp,inputw);
    redrawscreen();
}
