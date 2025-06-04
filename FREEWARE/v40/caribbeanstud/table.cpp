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

#include <iostream.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
//#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ToggleB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Dealer.h"
#include "Player.h"
#include "Table.h"

#define XtNbetUnit "betUnit"
#define XtCbetUnit "betUnit"
#define XtNminBet "minBet"
#define XtCminBet "minBet"
#define XtNbankroll "bankroll"
#define XtCbankroll "bankroll"
#define XtNjackpot "jackpot"
#define XtCjackpot "jackpot"
#define XtNminjackpot "minjackpot"
#define XtCminjackpot "minjackpot"
#define XtNplayers "players"
#define XtCplayers "players"

typedef struct {
  int bankroll;
  int betUnit;
  int minBet;
  int jackpot;
  int minjackpot;
  int players;
  } RValues, *RVptr;

RValues rv;

XtResource resources[] = {
   { XtNbankroll, XtCbankroll, XtRInt, sizeof(int),
     XtOffset(RVptr, bankroll), XtRImmediate, (XtPointer)100 },
   { XtNbetUnit, XtCbetUnit, XtRInt, sizeof(int),
     XtOffset(RVptr, betUnit), XtRImmediate, (XtPointer)1 },
   { XtNminBet, XtCminBet, XtRInt, sizeof(int),
     XtOffset(RVptr, minBet), XtRImmediate, (XtPointer)0 },
   { XtNjackpot, XtCjackpot, XtRInt, sizeof(int),
     XtOffset(RVptr, jackpot), XtRImmediate, (XtPointer)10000000 },
   { XtNminjackpot, XtCminjackpot, XtRInt, sizeof(int),
     XtOffset(RVptr, minjackpot), XtRImmediate, (XtPointer)1000000 },
   { XtNplayers, XtCplayers, XtRInt, sizeof(int),
     XtOffset(RVptr, players), XtRImmediate, (XtPointer)1 }
};

Widget Table::toplevel = (Widget) NULL;
Widget Table::Layout = (Widget) NULL;
Widget Table::PlayTable = (Widget) NULL;
Widget Table::ControlTable = (Widget) NULL;
Widget Table::DealerHand = (Widget) NULL;
Widget Table::DealButton = (Widget) NULL;
Widget Table::PlayerHand = (Widget) NULL;
Widget Table::AnteScroll = (Widget) NULL;
Widget Table::BetButton = (Widget) NULL;
Widget Table::FoldButton = (Widget) NULL;
Widget Table::ProgressiveButton = (Widget) NULL;
Widget Table::JackPotLabel = (Widget) NULL;
Widget Table::ExceptionDialog = (Widget) NULL;
Widget Table::YouHaveLabel[6] ;
Widget Table::YouHaveLabelValue[6];
Widget Table::YouWinLoseValue[6];
Widget Table::DealerTableLabel;

Widget Table::HelpRulesDialog;
Widget Table::HelpRulesButton;
Widget Table::HelpRulesScrolledText;
 
void Table::SetLabel(Widget w, char* label)
{
#ifdef DEBUG
  	 cout << "Calling Table::SetLabel: " << label <<endl;
#endif
   int i;
   Arg args[5];
   XmString s;

   assert(w != NULL);
   s = XmStringCreate(label, XmSTRING_DEFAULT_CHARSET);

   i = 0;
   XtSetArg(args[i], XmNlabelString, s);  i++;
   XtSetValues(w, args, i);
   XmStringFree(s);
}
 
void Table::Initialize()
{

#ifdef DEBUG
  	 cout << "BankRoll: " <<  _bankroll <<endl;
    cout << "BetUnit: " << _betUnit <<endl;
    cout << "Minimum Bet: " << _minBet <<endl;
    cout << "JackPot: " <<  _jackpot <<endl;
    cout << "JackPot: " <<  _minjackpot <<endl;
    cout << "Number Of Players: " <<  _totalSeats <<endl;
#endif
	 _dealerFace = 0;
	 _playerSeats[0] = new DealerSeat(this);
	 _dealer = new Dealer(_playerSeats[0]);
    _dealer->JackPot(_jackpot);
    _dealer->MinJackPot(_minjackpot);

    Player * player; 
    for(int i =1; i <= _players; i++)
	 {
	    _playerSeats[i] = new PlayerSeat(this, (i-1)*100/_players,
														    i*100/_players);
		 player = new Player(_dealer, _playerSeats[i]);
		 _dealer->Add(player);
		 player->BankRoll(_bankroll);
	 }
    CurrentSeat(0);
    Disable( BetButton ) ;
    Disable( FoldButton) ;
}

Table::Table(int argc, char **argv) :  _dealer(0)
{

#ifndef VMS
   try{
#endif
   XtSetLanguageProc ( (XtAppContext) NULL, (XtLanguageProc) NULL, (XtPointer) NULL );
   XtToolkitInitialize ();
   app_context = XtCreateApplicationContext ();
   _display = XtOpenDisplay (app_context, NULL, argv[0], "CaribbeanStud",
                            NULL, 0, &argc, argv);
   int scr = DefaultScreen(_display);
   if (!_display)
   {
       printf("%s: can't open display, exiting...\n", argv[0]);
       exit (-1);
   }

	CreateWidgets ( _display, argv[0], argc, argv);
	AddCallbacks();
	XtRealizeWidget (toplevel);
	Initialize();
#ifndef VMS
   }
   catch(const char *msg)
   {
      Table::DoExceptionDialog(msg);
#ifdef DEBUG
      cout << msg << endl;
#endif
   }
#endif  /* VMS */
}

Table::~Table()
{
	for(int i=0; i<_totalSeats; i++)
		delete _playerSeats[i];
	delete _dealer;
}

void Table::CreateWidgets(Display *display, char *app_name, int app_argc,
								  char **app_argv)
{
	Widget children[6];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	char  str[50];    /* temporary storage for XmStrings */

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
//   XtSetArg(al[ac], XmNargc, app_argc); ac++;
//   XtSetArg(al[ac], XmNargv, app_argv); ac++;


	toplevel = XtAppCreateShell ( app_name, "CaribbeanStud", applicationShellWidgetClass, display, al, ac );

   XtGetApplicationResources(toplevel, (unsigned char *)&rv, resources,
             XtNumber(resources), NULL, 0);
  
    _bankroll = rv.bankroll;
    _betUnit =  rv.betUnit;
    _minBet =   rv.minBet;
    _jackpot =  rv.jackpot;
    _minjackpot =  rv.minjackpot;
    _players =  (rv.players > 6 ) ? 6 : rv.players;
    _totalSeats = _players;

	ac = 0;
	XtSetArg(al[ac], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); ac++;
   ExceptionDialog = XmCreateMessageDialog(toplevel,
         "ExceptionDialog", al, ac);

	ac = 0;
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 0); ac++;
	XtSetArg(al[ac], XmNadjustLast, TRUE); ac++;
	XtSetArg(al[ac], XmNadjustMargin, TRUE); ac++;
	XtSetArg(al[ac], XmNrubberPositioning, TRUE); ac++;
	Layout = XmCreateForm ( toplevel, "Layout", al, ac );

   // control table
	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNrightPosition, 35); ac++;
	XtSetArg(al[ac], XmNrubberPositioning, TRUE); ac++;
	ControlTable = XmCreateForm ( Layout, "ControlTable", al, ac );

   // play Table 
	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 20); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 20); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 20); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftWidget, ControlTable); ac++;
	XtSetArg(al[ac], XmNrubberPositioning, TRUE); ac++;
	PlayTable = XmCreateForm ( Layout, "PlayTable", al, ac );
   // For all the contral Items

   // JackPot 

   ac = 0;  
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 20); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNrightOffset, 20); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 10); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
   XColor      color;
   Colormap cmap = DefaultColormap(_display, DefaultScreen(_display));
   color.red = 52428;
   color.green = color.blue = 0;
   XAllocColor(display, cmap, &color);
   XtSetArg(al[ac], XmNforeground, color.pixel);   ac++;
   JackPotLabel = XmCreateLabel(ControlTable, "JackPotLabel", al, ac);
	sprintf(str, "JACKPOT: %d", rv.jackpot);
   SetLabel(JackPotLabel, str);
   XtManageChild(JackPotLabel);

   ac = 0;
   XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
   XtSetArg(al[ac], XmNtopWidget, JackPotLabel); ac++;
   XtSetArg(al[ac], XmNtopOffset, 10); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 20); ac++;
   XtSetArg(al[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
   char name[50];
   XmString s;
   for(int j=0; j < rv.players; j++)
   {
      XtSetArg(al[ac], XmNleftPosition, j*100/rv.players);
      XtSetArg(al[ac+1], XmNrightPosition,(j+1)*100/rv.players);
      sprintf(name, "YouHaveLabel%i" , j+1);
      YouHaveLabel[j] = XmCreateLabel(ControlTable, name, al, ac +2);
      sprintf(name, " Player %i:  " , j+1);
      SetLabel(YouHaveLabel[j], name);
   }

   ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomPosition, 30); ac++;
   XtSetArg(al[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
   for(j=0; j < rv.players; j++) 
   { 
		XtSetArg(al[ac], XmNtopWidget, YouHaveLabel[j]);
      XtSetArg(al[ac+1], XmNleftPosition, j*100/rv.players); 
      XtSetArg(al[ac+2], XmNrightPosition,(j+1)*100/rv.players); 
      sprintf(name, "YouHaveLabelValue%i" , j+1);
      YouHaveLabelValue[j] = XmCreateLabel(ControlTable, name, al, ac +3); 
      sprintf(str, "$%d", rv.bankroll);
      SetLabel(YouHaveLabelValue[j], str);
   }

   ac = 0;
   XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 40); ac++;
   XtSetArg(al[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
   for(j=0; j < rv.players; j++)
   {
      XtSetArg(al[ac], XmNtopWidget, YouHaveLabelValue[j]);
      XtSetArg(al[ac+1], XmNleftPosition, j*100/rv.players);
      XtSetArg(al[ac+2], XmNrightPosition,(j+1)*100/rv.players);
      sprintf(name, "YouWinLoseValue%i" , j+1);
      YouWinLoseValue[j] = XmCreateLabel(ControlTable, name, al, ac +3);
      SetLabel(YouWinLoseValue[j], "");
   }

   // dealbutton
   ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 40); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 40); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNrightPosition, 65); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 50); ac++;
	DealButton = XmCreatePushButtonGadget ( ControlTable, "DealButton", al, ac );


   // player.progressiveButton
	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 52); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 15); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNrightPosition, 85); ac++;
	XtSetArg(al[ac], XmNmarginTop, 5); ac++;
	XtSetArg(al[ac], XmNmarginBottom, 5); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 62); ac++;
	ProgressiveButton = XmCreateToggleButton ( ControlTable, "ProgressiveButton", al, ac );

   // playerform.antescroll
	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 64); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 15); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNrightPosition, 85); ac++;
   XtSetArg(al[ac], XmNmaximum, rv.bankroll/3); ac++;
   if(rv.bankroll/3 >= rv.minBet)
   	XtSetArg(al[ac], XmNminimum, rv.minBet);
	else
   	XtSetArg(al[ac], XmNminimum, 0);
   ac++;
   XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
   XtSetArg(al[ac], XmNshowValue, True); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 77); ac++;
	AnteScroll = XmCreateScale ( ControlTable, "AnteScroll", al, ac );


   // player.betbutton
	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 80); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 15); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNrightPosition, 45); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 90); ac++;
	BetButton = XmCreatePushButtonGadget( ControlTable, "BetButton", al, ac );

   // player.foldbutton
   ac = 0;  
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 80); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 55); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNrightPosition, 85); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 90); ac++;
   FoldButton = XmCreatePushButtonGadget ( ControlTable, "FoldButton", al, ac );
 
   // For PlayTable

   ac = 0;
   XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNleftPosition, 85); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 8);   ac++;
   HelpRulesButton = XmCreatePushButtonGadget(PlayTable, "HelpRulesButton", al, ac);
   XtManageChild(HelpRulesButton);

   ac = 0;
   XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
   XtSetArg(al[ac], XmNrightWidget, HelpRulesButton); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNforeground, color.pixel);   ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
   XtSetArg(al[ac], XmNbottomPosition, 8);   ac++;
   DealerTableLabel = XmCreateLabel(PlayTable, "DealerLabel", al, ac);
   SetLabel(DealerTableLabel, "");
   XtManageChild(DealerTableLabel);

   // dealer draw area
   ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, DealerTableLabel); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomPosition, 55); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	DealerHand = XmCreateDrawingArea ( PlayTable, "DealerHand", al, ac );

   // player draw area
   ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, DealerHand); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	PlayerHand = XmCreateDrawingArea ( PlayTable, "PlayerHand", al, ac );

   // For help on Rules.

   HelpRulesDialog =  XmCreateFormDialog(toplevel,
            "HelpRulesDialog", NULL, 0);
 
 
   ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
   XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
   HelpRulesScrolledText = XmCreateScrolledText(HelpRulesDialog,
            "HelpRulesScrolledText", al , ac);
   XtManageChild(HelpRulesScrolledText);


   for(j=0; j< rv.players; j++)
	{
	  ac = 0;
	  children[ac++] = YouHaveLabel[j];
	  children[ac++] = YouHaveLabelValue[j];
	  children[ac++] = YouWinLoseValue[j];
	  XtManageChildren(children, ac);
	}

	ac = 0;
	children[ac++] = DealButton;
	XtManageChildren(children, ac);

	ac = 0;
	children[ac++] = AnteScroll;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = BetButton;
	children[ac++] = FoldButton;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = ProgressiveButton;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = PlayerHand;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = DealerHand;
	XtManageChildren(children, ac);
   ac = 0;
   children[ac++] = PlayTable;
   children[ac++] = ControlTable;
   XtManageChildren(children, ac);
	ac = 0;
	XtManageChild ( Layout );
}

void Table::AddCallbacks()
{
   XtAddCallback( DealerHand ,XmNexposeCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::DealerDrawingAreaCB,this);
   XtAddCallback( DealerHand, XmNresizeCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::DealerDrawingAreaCB, this);
   XtAddCallback( PlayerHand ,XmNresizeCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::PlayerDrawingAreaCB, this);
   XtAddCallback( PlayerHand ,XmNexposeCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::PlayerDrawingAreaCB, this);
   XtAddCallback( DealButton, XmNactivateCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::DealCB, this);
   XtAddCallback( BetButton,  XmNactivateCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::BetCB,  this);
   XtAddCallback( FoldButton, XmNactivateCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::FoldCB, this);
   XtAddCallback( AnteScroll,XmNvalueChangedCallback, 
					 (void(*)(_WidgetRec*, void*, void*))Table::AnteScrollCB,this);
   XtAddCallback(HelpRulesButton, XmNactivateCallback,
            	 (void(*)(_WidgetRec*, void*, void*))ManageHelpRulesDialogCB, this);
//   XtAddCallback(ExceptionDialog, XmNfocusCallback,
//                 (void(*)(_WidgetRec*, void*, void*))DoNothing, NULL);

}

void Table::DoExceptionDialog(const char *str)
{
   int i = 0;
   XmString s;
   Arg args[5];

   i = 0;
   s = XmStringCreate(str, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[i], XmNmessageString, s);   i++;
   XtSetValues(ExceptionDialog, args, i);
   XmStringFree(s);
   XtUnmanageChild(XmMessageBoxGetChild(ExceptionDialog,
               XmDIALOG_CANCEL_BUTTON));
   XtUnmanageChild(XmMessageBoxGetChild(ExceptionDialog,
               XmDIALOG_HELP_BUTTON));
   XtManageChild(ExceptionDialog);
   XtPopup(XtParent(ExceptionDialog), XtGrabNone);
}

void Table::CurrentSeat(int whichone)
{

   if(_players <= 1) return;

   Arg args[5];
   int i = 0;
   Pixel forg, back;
   char  label[50];

   
   XtSetArg(args[i], XmNforeground, &forg); i++;
   XtSetArg(args[i], XmNbackground, &back); i++;
   XtGetValues( YouHaveLabelValue[0], args, i);

   for(int j=0; j < _players; j++)
   {
      sprintf(label, "Player %d: ", j+1);
      i=0;
      if(j == whichone){
        XtSetArg(args[i], XmNbackground, forg);i++;
        XtSetArg(args[i], XmNforeground, back);i++;
      }
      else{
        XtSetArg(args[i], XmNforeground, forg);i++;
        XtSetArg(args[i], XmNbackground, back);i++;
      }
      XtSetValues(YouHaveLabel[j], args, i);
      SetLabel(YouHaveLabel[j], label);
   }
   if( whichone >= _players)
#ifndef __DECCXX
   throw(" The index for Table::CurrentSeat is too big. ");
#else
    cout << " The index for Table::CurrentSeat is too big. " << endl;
#endif
}

void Table::DealerDrawingAreaCB( Widget w, XtPointer client_data,
											XmDrawingAreaCallbackStruct *call_data)
{
#ifndef VMS
  try{
#endif
#ifdef DEBUG
   cout << "Calling DealerDrawingAreaCB" << endl;
#endif DEBUG
   if (call_data->window) {
     XEvent event;
     Table * table = (Table*) client_data;
     // drain Expose events 
     while (XCheckTypedWindowEvent(XtDisplay(table->DealerDrawingArea()),
            XtWindow(table->DealerDrawingArea()), Expose, &event));
     XClearWindow(XtDisplay(table->DealerDrawingArea()),
						XtWindow(table->DealerDrawingArea()));
     (table->_dealerFace == 0) ?
     ((DealerSeat*)table->GetDealerSeat())->ShowCardsBeforeBet(table->GetDealer()) :  
     table->GetDealerSeat()->ShowCards(table->GetDealer());
   }
#ifndef VMS
 }
 catch(const char *msg)
 {
    Table::DoExceptionDialog(msg);
#ifdef DEBUG
    cout << msg << endl;
#endif
 }
#endif /* VMS */
}
 
void Table::PlayerDrawingAreaCB( Widget w, XtPointer client_data,
                                 XmDrawingAreaCallbackStruct *call_data)
{
#ifndef VMS
  try{
#endif  
#ifdef DEBUG
   cout << "Calling PlayerDrawingAreaCB" << endl;
#endif DEBUG

   if (call_data->window) {
     XEvent event; 
     Table * table = (Table*) client_data; 
     Dealer* dealer = table->GetDealer() ; 
      // drain Expose events  
     while (XCheckTypedWindowEvent(table->GetDisplay(), 
            XtWindow(table->PlayerDrawingArea()), Expose, &event));
     XClearWindow(XtDisplay(table->PlayerDrawingArea()), XtWindow(table->PlayerDrawingArea()));
     int i = dealer->CurrentSpot();
     do {
        dealer->CurrentPlayer()->GetSeat()->ShowCards(dealer->CurrentPlayer());
		  if(!dealer->NextPlayer())
			 dealer->FirstPlayer();
     } while(dealer->CurrentSpot() != i);
   }
#ifndef VMS
 }
 catch(const char *msg)
 {
    Table::DoExceptionDialog(msg);
#ifdef DEBUG
    cout << msg << endl;
#endif
 }
#endif /* VMS */
}

void Table::AnteScrollCB( Widget w, XtPointer client_data,
								  XmScaleCallbackStruct *call_data)
{
#ifdef DEBUG
   cout << "Calling AnteScrollCB" << endl;
#endif DEBUG
   int i;
   char str[80];
   Arg  args[5];

   Table *table = (Table*)client_data;
   table->_dealerFace = 0;
   int   betPlaced = call_data->value;
   Dealer * dealer = table->GetDealer();

   if(table->_minBet > (dealer->CurrentPlayer()->BankRoll()-1)/3)
   {
      char str[80];
      sprintf(str, "Not enough money left for a bet.\nAdded another $%d!", table->
_bankroll);
      DoExceptionDialog(str);
      dealer->CurrentPlayer()->BankRoll(table->_bankroll+
                                        dealer->CurrentPlayer()->BankRoll());
      sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
      SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
   }

   if(betPlaced > (dealer->CurrentPlayer()->BankRoll()-1)/3)
   {
      sprintf(str, "Not enough money either for initial ante or additional bet.");
      DoExceptionDialog(str);
   }
}

void Table::ManageHelpRulesDialogCB( Widget w, XtPointer client_data, caddr_t call_data)
{
   XtManageChild(HelpRulesDialog);
}

void Table::DealCB( Widget w, XtPointer client_data, caddr_t call_data)
{
#ifdef DEBUG
   cout << "Calling DealCB" << endl;
#endif DEBUG
   Arg  args[5];
   char str[80];
	XmScaleCallbackStruct  data;
   Table *table = (Table*)client_data;

   SetLabel(table->DealerTableLabel, "");
   XtSetArg(args[0], XmNvalue, &(data.value));
   XtGetValues(table->AnteScroll, args, 1);
#ifndef VMS
   try{ 
#endif
   int i;

   table->_dealerFace = 0;
   int   betPlaced = data.value;
   Dealer * dealer = table->GetDealer();
   i = (int) XmToggleButtonGetState (table->ProgressiveButton);

   if(table->_minBet > (dealer->CurrentPlayer()->BankRoll()-1)/3)
   {
      char str[80];
      sprintf(str, "Not enough money left for a bet.\nAdded another $%d!", table->
_bankroll);
      DoExceptionDialog(str);
      dealer->CurrentPlayer()->BankRoll(table->_bankroll+
                                        dealer->CurrentPlayer()->BankRoll());
      sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
      SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
   }

   if(i != dealer->CurrentPlayer()->Progressive())
	   dealer->CurrentPlayer()->Progressive(i);
   dealer->CurrentPlayer()->Ante(betPlaced);
	sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
	SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
	if(dealer->NextPlayer())
	{
       i=0;
	    XtSetArg(args[i], XmNmaximum, (dealer->CurrentPlayer()->BankRoll()-1)/3);
	    betPlaced = (betPlaced > (dealer->CurrentPlayer()->BankRoll()-1)/3) ?
						  table->_minBet : betPlaced;
	    XtSetArg(args[i], XmNvalue, betPlaced); i++;
   	 XtSetValues(table->AnteScroll, args, i);
		 sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
		 SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
       table->CurrentSeat(dealer->CurrentSpot() -1);
	}else{
		 Disable( table->DealButton) ;
       Disable( table->AnteScroll) ;
   	 dealer->DealCards();
       XClearWindow(XtDisplay(table->PlayerDrawingArea()), XtWindow(table->PlayerDrawingArea()));
       XClearWindow(XtDisplay(table->DealerDrawingArea()), XtWindow(table->DealerDrawingArea()));
		 ((DealerSeat*)dealer->GetSeat())->ShowCardsBeforeBet(dealer);
       dealer->FirstPlayer();
		 do{
			dealer->CurrentPlayer()->Score();
			dealer->CurrentPlayer()->GetSeat()->ShowCards(dealer->CurrentPlayer());
		 }while(dealer->NextPlayer());
       Enable( table->BetButton ) ;
       Enable( table->FoldButton) ;
       dealer->FirstPlayer();
       table->CurrentSeat(0);
   }
#ifndef VMS
 }
 catch(const char *msg)
 {
    Table::DoExceptionDialog(msg);
#ifdef DEBUG
    cout << msg << endl;
#endif
 }
#endif /* VMS */
}

void Table::BetCB( Widget w, XtPointer client_data, caddr_t call_data)
{
#ifndef VMS
  try{
#endif
#ifdef DEBUG
   cout << "Calling BetCB" << endl;
#endif DEBUG
   char str[80];
   Arg  args[5];

   Table *table = (Table*)client_data;

   Dealer * dealer = table->GetDealer();
   dealer->CurrentPlayer()->Bet(dealer->CurrentPlayer()->Ante()*2);
	sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
	SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
	if(!dealer->NextPlayer())
		PayTime(table);
   else
      table->CurrentSeat(dealer->CurrentSpot() -1);
#ifndef VMS
 }
 catch(const char *msg)
 {
    Table::DoExceptionDialog(msg);
#ifdef DEBUG
    cout << msg << endl;
#endif
 }
#endif /* VMS */
}

void Table::FoldCB( Widget w, XtPointer client_data, caddr_t call_data)
{
#ifndef VMS
  try{
#endif
#ifdef DEBUG
   cout << "Calling FoldCB" << endl;
#endif DEBUG

   char str[80];
   Arg  args[5];
 
   Table *table = (Table*)client_data;
 
   Dealer * dealer = table->GetDealer();
   dealer->CurrentPlayer()->Fold();
   sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
   SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
   if(!dealer->NextPlayer())
      PayTime(table);
   else
      table->CurrentSeat(dealer->CurrentSpot() -1);
#ifndef VMS
 }
 catch(const char *msg)
 {
    Table::DoExceptionDialog(msg);
#ifdef DEBUG
    cout << msg << endl;
#endif
 }
#endif /* VMS */
}

void Table::PayTime(Table* table)
{
#ifndef VMS
  try{
#endif
#ifdef DEBUG
   cout << "Calling Table::PayTime" << endl;
#endif DEBUG

   char str[80];
   Arg  args[5];
   Dealer *dealer = table->GetDealer();

   Disable( table->BetButton ) ;
   Disable( table->FoldButton) ;
   table->_dealerFace = 1;
	dealer->Score();
   dealer->FirstPlayer();
   XClearWindow(XtDisplay(table->DealerDrawingArea()), XtWindow(table->DealerDrawingArea()));
	dealer->GetSeat()->ShowCards(dealer);
   if(dealer->Score() < 573496) // Does not have a King/Ace or higher
   {
	   strcpy(str,"Dealer does not have a ACE/KING or higher.");
      SetLabel(DealerTableLabel, str);
   }
	dealer->CollectBet();
	do
	{
	  sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
	  SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
     if(dealer->CurrentPlayer()->WinLost() >= 0)
		  sprintf(str, "Won: $%d", dealer->CurrentPlayer()->WinLost());
     else
		  sprintf(str, "Lost: $%d", -dealer->CurrentPlayer()->WinLost());
	  SetLabel(table->YouWinLoseValue[dealer->CurrentSpot() -1], str);
	}while(dealer->NextPlayer());
   
   // for the first player
   dealer->FirstPlayer();
	int  betPlaced;
   XtSetArg(args[0], XmNvalue, &betPlaced);
   XtGetValues(table->AnteScroll, args, 1);
   sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
   SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
   int i=0;
   if(table->_minBet > (dealer->CurrentPlayer()->BankRoll()-1)/3)
   {
      char str[80];
      sprintf(str, "Not enough money left for a bet.\nAdded another $%d!", table->_bankroll);
      DoExceptionDialog(str);
      dealer->CurrentPlayer()->BankRoll(table->_bankroll+
                                        dealer->CurrentPlayer()->BankRoll());
   	sprintf(str, "$%d", dealer->CurrentPlayer()->BankRoll());
   	SetLabel(table->YouHaveLabelValue[dealer->CurrentSpot() -1], str);
   }
   XtSetArg(args[i], XmNmaximum, (dealer->CurrentPlayer()->BankRoll()-1)/3);

	betPlaced = (betPlaced > (dealer->CurrentPlayer()->BankRoll()-1)/3) ?
                                     table->_minBet : betPlaced;
   XtSetArg(args[i], XmNvalue, betPlaced); i++;
   XtSetValues(table->AnteScroll, args, i);
	Enable( table->DealButton) ;
   Enable( table->AnteScroll) ;
   dealer->NewGame();
   table->CurrentSeat(0);
	sprintf(str, "JACKPOT: %d", dealer->JackPot());
   SetLabel(JackPotLabel, str);
#ifndef VMS
 }
 catch(const char *msg)
 {
    Table::DoExceptionDialog(msg);
#ifdef DEBUG
    cout << msg << endl;
#endif
 }
#endif /* VMS */
}

void Table::Enable ( Widget client_data )
{
      XtSetSensitive (client_data, TRUE);
}
 
void Table::Disable ( Widget client_data)
{
    XtSetSensitive (client_data, FALSE);
}
 
void Table::Start()
{
   XtAppMainLoop (app_context);
}
