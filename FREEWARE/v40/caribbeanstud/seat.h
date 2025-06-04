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

#ifndef Seat_h
#define Seat_h 1

//## Class: Seat
#include <stdio.h>
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

class Card;
class Hands;
class Table; 

class Seat
{
  public:

      Seat(Table*);
      virtual ~Seat();

		int  Available() const;
		void Available(int isAvailable);
	   virtual void ShowCards(Hands *);
		Table*  GetTable() ;
		virtual void Reset();
		
  protected:
		virtual void Initialize( Display*d, int scr, Widget drawArea);
		virtual int PositionCardX(Card*, int) = 0;
		virtual int PositionCardY(Card*, int) = 0;
		void			DrawCard(Window, Card*, int x, int y);
		void			DrawCardBack(Window, Card*, int x, int y);
      void        ShowMessage(Window, const char*, int x, int y);
		Window  		GetWindow(); 
		void			MakeCardMaps();

     enum { ROUND_W = 7, ROUND_H = 7,
            MARGIN_W = 3, MARGIN_H = 3,
				CARD_MAP_HEIGHT = 123,
				CARD_MAP_WIDTH = 80,
				CARD_HEIGHT = 128,
				CARD_WIDTH = 85
          } DRAW_CARD_ENUM;

  private:

      Seat();
      Seat(const Seat &right);
      const Seat & operator=(const Seat &right);
      int operator==(const Seat &right) const;
      int operator!=(const Seat &right) const;

		Pixmap GetCardMap(Card *);

  protected:  //## implementation

	int    _isAvailable;
	Table* _table;
	Bool   _cardIsClipped; 

	Display * _dpy;
	int     _screen;
	Widget  _drawingArea;

  private:  //## implementation

   static int _firstTime; 
	static GC  _redgc;
	static GC  _blackgc;
	static GC  _whitegc;
	static GC  _backgc;
	static GC  _cardgc;

	static  Pixmap  s_map[13], c_map[13];
	static  Pixmap  d_map[13], h_map[13];
	static  Pixmap  back_map;
};

class PlayerSeat : public Seat
{
  public:

		PlayerSeat(Table* table, int from, int to);
      virtual ~PlayerSeat();   

  protected:
      virtual int PositionCardX(Card*, int); 
      virtual int PositionCardY(Card*, int); 
 
  private:
      PlayerSeat();
      PlayerSeat(const PlayerSeat &right);
      const PlayerSeat & operator=(const PlayerSeat &right);
      int operator==(const PlayerSeat &right) const;
      int operator!=(const PlayerSeat &right) const;
  private:
 
		int _from;
		int _to;
};

class DealerSeat : public Seat
{
  public:
      DealerSeat(Table* table);
      virtual ~DealerSeat();
		void	  ShowCardsBeforeBet(Hands *);
		virtual void ShowCards(Hands * hands);
 
  protected:
      virtual int PositionCardX(Card*, int);
      virtual int PositionCardY(Card*, int);
 
  private:
      DealerSeat();
      DealerSeat(const DealerSeat &right);
      const DealerSeat & operator=(const DealerSeat &right);
      int operator==(const DealerSeat &right) const;
      int operator!=(const DealerSeat &right) const;

      GC   _msggc;
};

// Class Seat 
inline Seat::Available() const
{
	return _isAvailable;
}

inline void Seat::Available(int isAvailable)
{
	_isAvailable = isAvailable;
}

inline Table* Seat::GetTable() { return _table;}

inline Window Seat::GetWindow() { return XtWindow(_drawingArea); }

inline void   Seat::ShowMessage(Window w, const char* msg , int x, int y)
{
     XDrawString(_dpy, w, _redgc, x, y, msg, strlen(msg));
}
#endif
