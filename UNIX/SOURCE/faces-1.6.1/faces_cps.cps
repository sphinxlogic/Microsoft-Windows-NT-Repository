
%  These are NeWS dependent graphics routines used by faces.
%
%  @(#)faces_cps.cps 1.1 90/06/24
%
%  Copyright (c) Rich Burridge - Sun Microsystems Australia.
%                                All rights reserved.
%
%  Date	    Vers    Who	    	What
%  ===========================================================================
%  90Jun23  1.8.2   PMLashley	Converted to a cps file
%
%
%  Modifications to MakeFrame and ShowDisplay by Neil Crellin and
%  Mark Andrews {neilc,marka}@natmlab.dms.oz.au to fix the (0,0)
%  redraw bug.
%
%  Permission is given to distribute these sources, as long as the
%  copyright messages are not removed, and no monies are exchanged.
%
%  No responsibility is taken for any errors or inaccuracies inherent
%  either to the comments or the code of this program, but if reported
%  to me, then an attempt will be made to fix them.

   cdef ps_clearbox(int x, int y, int width, int height, postscript cv)
       cv setcanvas
       x y width height rectpath 0 setgray fill


   cdef ps_dopixrects(int width, int height, postscript cv)
       cv setcanvas
       0 0 width height rectpath
       cv reshapecanvas


   cdef ps_execfile(string psfile, int width, int height, int x, int y)
       /Iconic? Frame send {
	   /IconCanvas Frame send setcanvas
       } {
	   /ClientCanvas Frame send setcanvas
       } ifelse

       x y translate
       width height scale
       psfile (r) file cvx exec
       


   cdef ps_initfont()
       /Font /Courier findfont 7 scalefont def



   cdef ps_makeframe(int FrameX, int FrameY, int FrameWidth, int FrameHeight,
       int IconX, int IconY, int IsIcon)
       
       /DIED	    	100 	    	    def
       /PAINTED	    	101 	    	    def
       /LEFTDOWN    	102 	    	    def
       /ClientHeight	FrameHeight 10 sub  def
       /ClientWidth 	FrameWidth 10 sub   def
       /IconHeight  	FrameHeight 	    def
       /IconWidth   	FrameWidth  	    def
       
       /PR framebuffer /new ClassCanvas send def
       {
	   /Transparent false promote
	   /Mapped false promote
	   /Retained true promote
       } PR send
       
       /MPR framebuffer /new ClassCanvas send def
       {
	   /Transparent false promote
	   /Mapped false promote
	   /Retained true promote
       } MPR send
       
       /Frame framebuffer /new DefaultWindow send def
       {
	   /IconX   IconX   def
	   /IconY   IconY   def
	   /FrameX  FrameX  def
	   /FrameY  FrameY  def
	   /BorderTop 5 def
	   /BorderLeft 5 def
	   /BorderBottom 5 def
	   /BorderRight 5 def
	   /ControlSize 0 def
	   /ForkPaintClient? false def

	   /destroy {
	       DIED typedprint
	       DestroyFrame DestroyClient
	   } installmethod

	   /flipiconic {
	       PAINTED typedprint
	       /unmap self send
	       /Iconic? Iconic? not def
	       IconX null eq {
		   FrameX FrameY FrameHeight add IconHeight sub /move self send
	       } if
	       ZoomProc
	       /map self send
	   } installmethod

	   /PaintClient {
	       gsave
	       1 fillcanvas
	       ClientWidth ClientHeight scale
	       0 setgray
	       Invert PR imagemaskcanvas
	       grestore
	   } installmethod

	   /PaintIcon {
	       gsave
	       clippath pathbbox pop pop translate
	       1 fillcanvas
	       IconWidth IconHeight scale
	       0 setgray
	       Invert MPR imagemaskcanvas
	       grestore
	   } installmethod

	   /ShapeIconCanvas {
	       gsave
	       ParentCanvas setcanvas
	       IconX null eq IconY null eq or {
		   0 0
	       } {
		   IconX IconY
	       } ifelse
	       
	       IconWidth IconHeight IconPath IconCanvas reshapecanvas
	       grestore
	   } installmethod

	   /ForkFrameEventMgr {
	       ClientMenu null ne {
		   FrameInterests /ClientMenuEvent
		   MenuButton { /showat ClientMenu send }
		   /DownTransition ClientCanvas
		   eventmgrinterest put
	       } if

	       FrameInterests /CheckTheMailDrooper
	       PointButton { LEFTDOWN typedprint }
	       /DownTransition ClientCanvas
	       eventmgrinterest put
	       /FrameEventMgr FrameInterests forkeventmgr def
	   } installmethod

	   /ShowDisplay {   %  cw  ch  iw  ih  =>  -
	       /IconHeight  	exch	promote
	       /IconWidth   	exch	promote
	       /ClientHeight    exch	promote
	       /ClientWidth 	exch	promote

	       FrameX FrameY
	       ClientWidth 10 add  ClientWidth 10 add
	       reshape
	       
	       ShapeIconCanvas
	       painticon
	       
	       ShapeClientCanvas
	       paintclient
	       map
	   } installmethod
       } Frame send

       IsIcon 1 eq { /flipiconic Frame send } if


   cdef ps_maketext(string str, int x, int y, postscript cv)
       cv setcanvas
       Font setfont
       1 setgray
       x y moveto
       str show


   cdef ps_moveimage(postscript src, int width, int height, int x, int y, postscript dest)
       dest setcanvas
       gsave
           x y translate
           width height scale
           src imagecanvas
       grestore


   cdef ps_shiftimage(int dx, int width, int height, postscript cv)
       cv setcanvas
       0 0 width height rectpath
       dx 0 copyarea


   cdef ps_showdisplay(int cw, int ch, int iw, int ih)
       cw ch iw ih /ShowDisplay Frame send

   cdef ps_buildicon(string name, int width, int height, cstring data)
       name cvlit width height 1 {} { data } buildimage def
