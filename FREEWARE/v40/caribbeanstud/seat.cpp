/*
 * (c) Copyright 1997, Qun Zhang.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Qun Zhang not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Qun Zhang make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE ABOVE-NAMED DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE ABOVE-NAMED BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
#include <stdlib.h>
#include <iostream.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#ifndef VMS
#include <X11/Xmu/Drawing.h>
#else
#include "VMS_Drawing.h"
#endif
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include "Hands.h"
#include "Seat.h"
#include "Table.h"

#include <assert.h>
#include <strings.h>
#include "Bitmaps.h"

int Seat::_firstTime = 1;
GC  Seat::_redgc;
GC  Seat::_blackgc;
GC  Seat::_whitegc;
GC  Seat::_backgc;
GC  Seat::_cardgc;
 
Pixmap  Seat::s_map[13];
Pixmap  Seat::c_map[13];
Pixmap  Seat::d_map[13];
Pixmap  Seat::h_map[13];
Pixmap  Seat::back_map;

Seat::Seat(Table* table) : _isAvailable(1), _table(table)
{
}

void Seat::Reset() {}

void Seat::Initialize( Display*d, int scr, Widget  drawArea)
{

	XGCValues   gcv;
	long     gcflags;
	XColor   color;
	Colormap cmap;
	GC       logogc;
	int      i;
	Arg      args[10];


	Pixmap   logomap;

	Pixel blackpixel;
	Pixel whitepixel;
	Pixel borderpixel;
	Pixel greenpixel;
	Pixel redpixel;

	_drawingArea = drawArea;
	_dpy = d;
	_screen = scr;
	Window w = XtWindow(_drawingArea);
		
	blackpixel = BlackPixel(_dpy, _screen);
	whitepixel = WhitePixel(_dpy, _screen);
	  

	if(_firstTime){
	   _firstTime = 0;

      // make card maps 
		MakeCardMaps();

		// make gc for white 
		gcv.foreground = WhitePixel(_dpy, _screen);
		gcv.background = BlackPixel(_dpy, _screen);
		gcv.graphics_exposures = False;
		gcflags = GCForeground | GCBackground | GCGraphicsExposures;
		_whitegc = XCreateGC(_dpy, RootWindow(_dpy, _screen), gcflags, &gcv);

		// make gc for black
		gcv.foreground = BlackPixel(_dpy, _screen);
		gcv.background = WhitePixel(_dpy, _screen);
		gcflags = GCForeground | GCBackground | GCGraphicsExposures;

		_blackgc = XCreateGC(_dpy, RootWindow(_dpy, _screen), gcflags, &gcv);

		logomap = XCreatePixmap(_dpy, RootWindow(_dpy, _screen), CARD_MAP_WIDTH,
			CARD_MAP_HEIGHT, DefaultDepth(_dpy, _screen));

		cmap = DefaultColormap(_dpy, _screen);
		XAllocNamedColor(_dpy, cmap, "Yellow", &color, &color);
		gcv.foreground = color.pixel;
		XAllocNamedColor(_dpy, cmap, "Tan", &color, &color);
		gcv.background = color.pixel;
		gcflags = GCForeground | GCBackground;
		logogc = XCreateGC(_dpy, RootWindow(_dpy, _screen), gcflags, &gcv);
		XCopyPlane(_dpy, back_map, logomap, logogc, 0, 0,
					  CARD_MAP_WIDTH, CARD_MAP_HEIGHT, 0, 0, 1);
		XFreeGC(_dpy, logogc);

		gcv.tile = logomap;
		gcv.fill_style = FillTiled;
		gcflags |= GCTile | GCFillStyle | GCGraphicsExposures;

		_backgc = XCreateGC(_dpy, RootWindow(_dpy, _screen), gcflags, &gcv);

		borderpixel = blackpixel;

		cmap = DefaultColormap(_dpy, _screen);

		color.flags = DoRed | DoGreen | DoBlue;

		 
		// color levels are the NeWS RGB values
		
		color.red = 13107;
		color.green = 52428; 
		color.blue = 39321; 
	 
		XAllocColor(_dpy, cmap, &color);
		greenpixel = color.pixel;
	 
		color.red = 52428; 
		color.green = color.blue = 0;
		XAllocColor(_dpy, cmap, &color);
		redpixel = color.pixel;
	 
		gcv.foreground = redpixel;
		gcv.background = WhitePixel(_dpy, _screen);
//	   gcv.font = XLoadFont(_dpy, "9x15bold"); 
	   gcv.font = XLoadFont(_dpy, "10x20"); 
		gcflags = GCForeground | GCBackground | GCGraphicsExposures;
		_redgc = XCreateGC(_dpy, RootWindow(_dpy, _screen), gcflags, &gcv);
	 
	}
	// allocate drawing area background color, set areas 
	XAllocNamedColor(_dpy, cmap, "Forest Green", &color, &color);
	i = 0;
	XtSetArg(args[i], XmNbackground, color.pixel);   i++;
	XtSetArg(args[i], XmNforeground, whitepixel);   i++;
	XtSetValues(_drawingArea, args, i);

}
 
Seat::~Seat()
{
}

void Seat::ShowCards(Hands * hands)
{
	Card ** cards = hands->Cards();
   for(int i=0; i< hands->NumOfCards(); i++)
		DrawCard(GetWindow(), cards[i], PositionCardX(cards[i],i),
												  PositionCardY(cards[i],i));

   ShowMessage(GetWindow(), hands->HandName(), PositionCardX(cards[0],2),
					PositionCardY(cards[0],0) + 20 + CARD_HEIGHT); 
}

void Seat::DrawCardBack(Window win, Card*, int x, int y)
{
   int   delta = CARD_HEIGHT;

   /* change the origin so cards will have the same back anywhere
    * on the Window
    *
    * there should be a tile centered in the card, with the
    * surrounding tiles being partial
    */
   XmuFillRoundedRectangle(_dpy, win, _blackgc, x, y, CARD_WIDTH,
      delta , ROUND_W, ROUND_H);
   XSetTSOrigin(_dpy, _blackgc, x , y);

   XmuFillRoundedRectangle(_dpy, win, _backgc, x , y ,
      CARD_WIDTH , CARD_HEIGHT, ROUND_W, ROUND_H);
}

void Seat::DrawCard(Window win, Card* card, int x, int y)
{

   if (card->Suit() == 'S' || card->Suit() == 'C')  {
      _cardgc = _blackgc;
   } else   {
      _cardgc = _redgc;
   }
 
   // fill the background
   XmuFillRoundedRectangle(_dpy, win, _whitegc, x, y,
      CARD_WIDTH, CARD_HEIGHT,
      ROUND_W, ROUND_H);
 
   // draw border on card
   XmuDrawRoundedRectangle(_dpy, win, _blackgc, x, y,
      CARD_WIDTH, CARD_HEIGHT,
      ROUND_W, ROUND_H);
      _cardIsClipped = False;

   XCopyPlane(_dpy, GetCardMap(card) , win, _cardgc,
      0, 0, CARD_MAP_WIDTH, CARD_MAP_HEIGHT, x+MARGIN_W , y+MARGIN_H , 1);
 
   // clear the clip mask
   XSetClipMask(_dpy, _cardgc, None);
   XSetClipMask(_dpy, _whitegc, None);
   if (_cardgc != _blackgc)
   XSetClipMask(_dpy, _blackgc, None);

}

Pixmap Seat::GetCardMap(Card* card)
{
   assert(card);

   if(card->Rank() < 1 || card->Rank() > 14 )
#ifndef __DECCXX
      throw(" Card Rank out of range. " );
#else
      cout << " Card Rank out of range. " << endl;
#endif
  
    int index = (card->Rank() == 14) ? 1 : card->Rank(); 
    switch(card->Suit()){
    case 'S':
      return s_map[index-1];
    case 'C':
      return c_map[index-1];
    case 'H':
      return h_map[index-1];
    case 'D':
      return d_map[index-1];
    case 'W':
    defaults:
#ifndef __DECCXX
      throw(" Card not used in this game. ");
#else
      cout << " Card not used in this game. " << endl;
#endif
    }
}
 
void Seat::MakeCardMaps()
{
 
   for (int i = 0; i< 13; i++) {
      s_map[i] = XCreateBitmapFromData(_dpy,
         RootWindow(_dpy, _screen),
         s_bits[i], CARD_MAP_WIDTH, CARD_MAP_HEIGHT);
 
      h_map[i] = XCreateBitmapFromData(_dpy,
         RootWindow(_dpy, _screen),
         h_bits[i], CARD_MAP_WIDTH, CARD_MAP_HEIGHT);
 
      c_map[i] = XCreateBitmapFromData(_dpy,
         RootWindow(_dpy, _screen),
         c_bits[i], CARD_MAP_WIDTH, CARD_MAP_HEIGHT);
 
      d_map[i] = XCreateBitmapFromData(_dpy,
         RootWindow(_dpy, _screen),
         d_bits[i], CARD_MAP_WIDTH, CARD_MAP_HEIGHT);
 
   }

   back_map = XCreateBitmapFromData(_dpy,
      RootWindow(_dpy, _screen),
      back_bits, CARD_MAP_WIDTH, CARD_MAP_HEIGHT);
 
}


PlayerSeat::PlayerSeat(Table* table, int from, int to) : Seat(table),
                                                    _from(from),
                                                    _to(to)
{
	Initialize(GetTable()->GetDisplay(), DefaultScreen(GetTable()->GetDisplay()),
				  GetTable()->PlayerDrawingArea());
}


PlayerSeat::~PlayerSeat()
{
}

int  PlayerSeat::PositionCardX(Card* card, int n)
{

	Dimension width;
	int i;
	Arg args[5];

	i = 0;
	XtSetArg(args[i], XmNwidth, &width);	i++;
	XtGetValues(GetTable()->PlayerDrawingArea(), args, i);
#ifdef DEBUG
	cout << "Player Drawing Area width: " << width << endl;
#endif
	
	// spread them out if 6 will fit 
	if (width*(_to-_from)/100 >= (10 + 6*(10+CARD_WIDTH))) {
		return(width*_from/100+10 + n*(10+CARD_WIDTH));
	}
	else {	// otherwise overlap, accomodating 6
		return(width*_from/100+10+ n*(width*(_to-_from)/100-20-CARD_WIDTH)/5);
	}
}

int  PlayerSeat::PositionCardY(Card*, int)
{
	Dimension height;
	int i;
	Arg args[5];

	i = 0;
	XtSetArg(args[i], XmNheight, &height);	i++;
	XtGetValues(GetTable()->PlayerDrawingArea(), args, i);
	return ((height -30- CARD_HEIGHT)/2);
}

DealerSeat::DealerSeat(Table* table) : Seat(table)
{
	Initialize(GetTable()->GetDisplay(), DefaultScreen(GetTable()->GetDisplay()),
				  GetTable()->DealerDrawingArea());
}

DealerSeat::~DealerSeat()
{
   XFreeGC(_dpy,_msggc); 
}

void DealerSeat::ShowCardsBeforeBet(Hands * hands)
{ 
   Card ** cards = hands->Cards();
   if( hands->NumOfCards() )
   {
      for(int i=0; i< hands->NumOfCards() -1; i++)
        DrawCardBack(GetWindow(), cards[i], PositionCardX(cards[i],i),
                                          PositionCardY(cards[i],i));
      DrawCard(GetWindow(), cards[i], PositionCardX(cards[i],i),
                                          PositionCardY(cards[i],i));
   }
}

void DealerSeat::ShowCards(Hands * hands)
{
   Card ** cards = hands->Cards();
   for(int i=0; i< hands->NumOfCards(); i++)
      DrawCard(GetWindow(), cards[i],
               PositionCardX(cards[i], hands->NumOfCards()-1-i),
               PositionCardY(cards[i],  hands->NumOfCards()-1-i));

   ShowMessage(GetWindow(), hands->HandName(),
                 PositionCardX(cards[0], hands->NumOfCards()-2),
                 20);
}

int  DealerSeat::PositionCardX(Card* card, int n)
{
   Dimension width;
   int i;
   Arg args[5];
 
   i = 0;
   XtSetArg(args[i], XmNwidth, &width); i++;
   XtGetValues(GetTable()->DealerDrawingArea(), args, i);
   
#ifdef DEBUG
	cout << "Dealer Drawing Area width: " << width << endl;
#endif

   /* spread them out if 6 will fit */
   if (width >= (10 + 6*(10+CARD_WIDTH))) {
      int marg = (width - (10 + 6*(10+CARD_WIDTH)))/2;
      return width - ( marg + 10 + (n+1)*(10+CARD_WIDTH));
   }
   else {   /* otherwise overlap, accomodating 6 */
      return width - (10+ CARD_WIDTH + n*(width-20-CARD_WIDTH)/5);
   }
}

int  DealerSeat::PositionCardY(Card* card , int n)
{
   Dimension height;
   int i;
   Arg args[5];

   i = 0;
   XtSetArg(args[i], XmNheight, &height); i++;
   XtGetValues(GetTable()->DealerDrawingArea(), args, i);
   return 30+(height -  30 -CARD_HEIGHT)/2;
}

