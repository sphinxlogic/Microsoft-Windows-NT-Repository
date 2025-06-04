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

#ifndef Table_h
#define Table_h
#include "Seat.h"

class Dealer;

class Table 
{

  public:

      Table(int argc, char **argv);
      ~Table();

      Seat*    GetDealerSeat() const;
      Seat*    GetPlayerSeat(int i = 1) const; // start from 1
      Dealer*  GetDealer()  const;
      void     SetDealer(Dealer*);
      int      NumPlayers() const;
		Widget   DealerDrawingArea() { return DealerHand;}
		Widget   PlayerDrawingArea() { return PlayerHand;}
		Display* GetDisplay() { return _display; }
		static   void SetLabel(Widget w, char* label);
      static   void AnteScrollCB(Widget, XtPointer , XmScaleCallbackStruct*);
      static   void DealerDrawingAreaCB(Widget, XtPointer ,
													 XmDrawingAreaCallbackStruct*);
      static   void PlayerDrawingAreaCB(Widget, XtPointer ,
													 XmDrawingAreaCallbackStruct*);
      static   void BetCB ( Widget w, XtPointer client_data, caddr_t call_data);
      static   void DealCB( Widget w, XtPointer client_data, caddr_t call_data);
      static   void FoldCB( Widget w, XtPointer client_data, caddr_t call_data);
      static   void ManageHelpRulesDialogCB( Widget w, XtPointer client_data,
                            caddr_t call_data);
		static   void DoExceptionDialog(const char * exceptionMsg);
		         void Start();
  protected:
      void  Initialize();
	   void  CreateWidgets(Display *display, char *app_name, int app_argc, char **app_argv);
      void  CurrentSeat(int);
	   int   NumOfSeats();
	   void  AddCallbacks();
	   static void  Enable ( Widget);
	   static void  Disable( Widget);
      static void  PayTime(Table*);

  private:
      const Table & operator=(const Table &right);
      int operator==(const Table &right) const;
      int operator!=(const Table &right) const;
      Table(const Table &right);

  private: 
      int   _totalSeats;
      Dealer* _dealer;
      int    _bankroll; 
      int    _betUnit;
      int    _minBet; 
      int    _jackpot; 
      int    _minjackpot; 
      int    _players;
      int    _dealerFace;
		static Widget toplevel;
		static Widget Layout ;
		static Widget PlayTable ;
		static Widget ControlTable ;
		static Widget DealerHand ;
		static Widget PlayerHand ;
		static Widget DealButton ;
		static Widget AnteScroll ;
		static Widget BetButton  ;
		static Widget FoldButton ;
		static Widget ProgressiveButton ;
		static Widget JackPotLabel;
		static Widget ExceptionDialog;
		static Widget YouHaveLabel[6];
		static Widget YouHaveLabelValue[6];
		static Widget YouWinLoseValue[6];
		static Widget DealerTableLabel;

     // Widgets for Rules...
      static Widget HelpRulesDialog;
		static Widget HelpRulesButton;
		static Widget HelpRulesScrolledText;

		XtAppContext app_context;
		Display *_display;
      Seat*   _playerSeats[7];
};

inline void Table::SetDealer(Dealer* dealer)
{
	_dealer = dealer;
}

inline Dealer* Table::GetDealer() const
{
	return _dealer;
}


inline Seat* Table::GetDealerSeat() const
{
     return _playerSeats[0];
}


inline Seat* Table::GetPlayerSeat(int i) const
{
	if( i<_totalSeats)
			return _playerSeats[i];
	else{
		char str[50];
		sprintf(str, "Player %d out of range.", i );
#ifndef __DECCXX
		throw( str );
#endif
   }
}

inline int  Table::NumPlayers() const
{
	return _totalSeats;
}

#endif
