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
#include "cellbox.h"

extern cellbox *tentative;

int DoKeySymIn(keysym)
 KeySym keysym;
{
     switch (keysym) {
	case XK_4:
	case XK_KP_4:
	case XK_Left:
	  XClearWindow(disp,lifew);
	  xpos-= width/2 >> scale;
	  redrawscreen();
	  break;
	case XK_6:
	case XK_KP_6:
	case XK_Right:
	  XClearWindow(disp,lifew);
	  xpos+= width/2 >> scale;
	  redrawscreen();
	  break;
	case XK_8:
	case XK_KP_8:
	case XK_Up:
	  XClearWindow(disp,lifew);
	  ypos-= height/2 >> scale;
	  redrawscreen();
	  break;
	case XK_2:
	case XK_KP_2:
	case XK_Down:
	  XClearWindow(disp,lifew);
	  ypos+= height/2 >> scale;
	  redrawscreen();
	  break;
	case XK_7:
	case XK_KP_7:
	  XClearWindow(disp,lifew);
	  xpos-= width/2 >> scale;
	  ypos-= height/2 >> scale;
	  redrawscreen();
	  break;
	case XK_9:
	case XK_KP_9:
	  XClearWindow(disp,lifew);
	  xpos+= width/2 >> scale;
	  ypos-= height/2 >> scale;
	  redrawscreen();
	  break;
	case XK_3:
	case XK_KP_3:
	  XClearWindow(disp,lifew);
	  xpos+= width/2 >> scale;
	  ypos+= height/2 >> scale;
	  redrawscreen();
	  break;
	case XK_1:
	case XK_KP_1:
	  XClearWindow(disp,lifew);
	  xpos-= width/2 >> scale;
	  ypos+= height/2 >> scale;
	  redrawscreen();
	  break;
	case XK_5:
	case XK_KP_5:
	  center();
	  break;
	case XK_Help:
	  help();
	  break;
	default:
	  return 0;
     }

     /* could process it */
     return 1;
}

void DoKeyIn(kbuf)
 char kbuf[16];
{
char pstring[50];

    switch(kbuf[0]){
	case 'r':
	  redrawscreen();
	break;
	case 'R':
	  newrules();
	  break;
	case '=':
	case '+':
	  if(scale < 7){
	      XClearWindow(disp,lifew);
	      scale++;
	      xpos += XPOS(event.xmotion.x, 0);
	      ypos += YPOS(event.xmotion.y, 0);
	      redrawscreen();
	  }
 	  break;
        case '.':
          XClearWindow(disp,lifew);
          xpos += (event.xbutton.x - width/2) >> (scale -1);
          ypos += (event.xbutton.y - height/2) >> (scale -1);
          redrawscreen();
	case '-':
	  if(scale > 1){
	      XClearWindow(disp,lifew);
	      xpos -= XPOS(event.xmotion.x, 0);
	      ypos -= YPOS(event.xmotion.y, 0);
	      scale--;
	      redrawscreen();
	  }
	  break;
	case 'g':
          confirmload();
	  state=(state==RUN)?STOP:RUN;
	  XClearWindow(disp,lifew);
	  redrawscreen();
	  break;
	case 'c':
	  dispcells ^=1;
	  break;
	case 'o':
          confirmload();
	  generate();
	  XClearWindow(disp,lifew);
	  redrawscreen();
	  break;
	case 'p':
          if (dispcoord) {
              dispcoord=0;
          } else {
              lastx=lastx-1; /* fake move to force coordinates to print */
              dispcoord=1;
          }
          /* force resize of input window */
          XResizeWindow(disp,inputw,
                        width-dispcoord*(COORDW+BORDERWIDTH)-BORDERWIDTH*2,
                        INPUTH);
	  break;
	case 'O':
          strcpy(inpbuf,"Origin set to current cell");
	  XClearWindow(disp,inputw);
          XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),
                      inpbuf,strlen(inpbuf));
          xorigin=XPOS(event.xmotion.x,xpos);
          yorigin=YPOS(event.xmotion.y,ypos);
	  break;
	case 'C':
	  clear();
	  break;
	case 'S':
	  savefile();
	  break;
	case 'W':
	  saveloadscript();
	  free_loadscript();
	  break;
	case 'D':
	  free_loadscript();
          if (tentative) {
             undoload();
	     XClearWindow(disp,lifew);
	     redrawscreen();
             strcpy(inpbuf,"Load script (and latest load) discarded");
          } else strcpy(inpbuf,"Load script discarded"); 
	  XClearWindow(disp,inputw);
          XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),
                      inpbuf,strlen(inpbuf));
	  break;
	case 'l':
	  loadfile();
	  break;
	case 'h':
          confirmload();
	  if(state==HIDE){
	      state=STOP;
	      XClearWindow(disp,lifew);
	      redrawscreen();
	  }
	  else{
	      state=HIDE;
	  }
	  break;
	case '?':
	  help();
	  break;
	case 'f':
	  settimeout(DELAY_FAST);
	  break;
	case 'm':
	  settimeout(DELAY_MED);
	  break;
	case 's':
	  settimeout(DELAY_SLOW);
	  break;
	case '!':
	  randomize();
	  break;
	case 'N':
	  name_file();
	  break;
	case 'A':
	  comment();
	  break;
	case 'V':
	  view_comments();
	  break;
	case 'B':
	  benchmark();
	  break;
	case 'Q':
	  exit(0);
	  break;
        case 'U':
          /* Get rid of loaded pattern */
          undoload();
	  XClearWindow(disp,lifew);
	  redrawscreen();
	  break;
        case 'I':
          /* Force confirm of loaded pattern */
          confirmload();
	  XClearWindow(disp,lifew);
	  redrawscreen();
	  break;
        case 'G':
          /* perform some generations on loaded pattern */ 
          genload();
	  XClearWindow(disp,lifew);
	  redrawscreen();
	  break;
	default:
	  break;
    }
    kbuf[0]='\0'; /* get rid of old keystroke so shift doesn't bring it back */
}
