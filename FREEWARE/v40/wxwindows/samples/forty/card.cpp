//+-------------------------------------------------------------+
//| File:	card.cpp											|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
//| Description:												|
//|	A class for drawing playing cards.							|
//|	Currently assumes that the card symbols have been			|
//|	loaded into hbmap_symbols and the pictures for the			|
//|	Jack, Queen and King have been loaded into					|
//|	hbmap_pictures.												|
//+-------------------------------------------------------------+

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "forty.h"
#include "card.h"

#ifdef wx_x
#include "pictures.xpm"
#include "symbols.xbm"
#endif

wxBitmap* Card::pictureBmap_ = 0;
wxBitmap* Card::symbolBmap_ = 0;


//+-------------------------------------------------------------+
//| Card::Card()												|
//+-------------------------------------------------------------+
//| Description:												|
//|	Constructor for a playing card.								|
//|	Checks that the value is in the range 1..52 and then		|
//|	initialises the suit, colour, pipValue and wayUp.			|
//+-------------------------------------------------------------+
Card::Card(int value, WayUp way_up) :
	wayUp_(way_up)
{
	if (!symbolBmap_)
	{
#if wx_msw
		symbolBmap_ = new wxBitmap("CardSymbols", wxBITMAP_TYPE_BMP_RESOURCE);
#else
		symbolBmap_ = new wxBitmap(Symbols_bits, Symbols_width, Symbols_height);
#endif
		if (!symbolBmap_->Ok())
		{
			::wxMessageBox("Failed to load bitmap CardSymbols", "Error");
		}
	}
	if (!pictureBmap_)
	{
#if wx_msw
		pictureBmap_ = new wxBitmap("CardPictures", wxBITMAP_TYPE_BMP_RESOURCE);
#else
		pictureBmap_ = new wxBitmap(Pictures);
#endif
		if (!pictureBmap_->Ok())
		{
			::wxMessageBox("Failed to load bitmap CardPictures", "Error");
		}
	}

    if (value >= 1 && value <= PackSize)
    {
		switch ((value - 1) / 13)
		{
		case 0:
			suit_ = clubs;
			colour_ = black;
			break;
		case 1:
			suit_ = diamonds;
			colour_ = red;
			break;
		case 2:
			suit_ = hearts;
			colour_ = red;
			break;
		case 3:
			suit_ = spades;
			colour_ = black;
			break;
		}
		pipValue_ = 1 + (value - 1) % 13;
		status_ = TRUE;
    }
    else
    {
        status_ = FALSE;
    }
} // Card::Card()


//+-------------------------------------------------------------+
//| Card::~Card()												|
//+-------------------------------------------------------------+
//| Description:												|
//|	Destructor - nothing to do at present.						|
//+-------------------------------------------------------------+
Card::~Card()
{
}


//+-------------------------------------------------------------+
//| Card::Erase()												|
//+-------------------------------------------------------------+
//| Description:												|
//|	Erase the card at (x, y) by drawing a rectangle in the		|
//|	background colour.											|
//+-------------------------------------------------------------+
void Card::Erase(wxDC* dc, int x, int y)
{
	wxPen* pen = wxThePenList->FindOrCreatePen(
						FortyApp::BackgroundColour(),
						1,
						wxSOLID
						);
	dc->SetPen(pen);
	dc->SetBrush(FortyApp::BackgroundBrush());
	dc->DrawRectangle(x, y, CardWidth, CardHeight);
} // Card::Erase()


//+-------------------------------------------------------------+
//| Card::Draw()												|
//+-------------------------------------------------------------+
//| Description:												|
//|	Draw the card at (x, y).									|
//|	If the card is facedown draw the back of the card.			|
//|	If the card is faceup draw the front of the card.			|
//|	Cards are not held in bitmaps, instead they are drawn		|
//|	from their constituent parts when required.					|
//|	hbmap_symbols contains large and small suit symbols and		|
//|	pip values. These are copied to the appropriate part of		|
//|	the card. Picture cards use the pictures defined in			|
//|	hbmap_pictures. Note that only one picture is defined		|
//|	for the Jack, Queen and King, unlike a real pack where		|
//|	each suit is different.										|
//|																|
//| WARNING:													|
//|	The locations of these symbols is 'hard-wired' into the		|
//|	code. Editing the bitmaps or the numbers below will			|
//|	result in the wrong symbols being displayed.				|
//+-------------------------------------------------------------+
void Card::Draw(wxDC* dc, int x, int y)
{
	wxBrush* backgroundBrush = dc->GetBackground();

	dc->SetBrush(wxWHITE_BRUSH);
	dc->SetPen(wxBLACK_PEN);
#ifdef wx_msw
	dc->DrawRoundedRectangle(x, y, CardWidth, CardHeight, 4);
#else
	dc->DrawRoundedRectangle(x, y, CardWidth - 1, CardHeight - 1, 4);
#endif
	if (wayUp_ == facedown)
	{
		dc->SetBackground(wxRED_BRUSH);
		dc->SetBackgroundMode(wxSOLID);
		wxBrush* brush = wxTheBrushList->FindOrCreateBrush(
							"BLACK", wxCROSSDIAG_HATCH
							);
		dc->SetBrush(brush);

		dc->DrawRoundedRectangle(
				x + 4, y + 4,
#ifdef wx_msw
				CardWidth - 8, CardHeight - 8,
#else
				CardWidth - 9, CardHeight - 9,
#endif
				2
				);
	}
	else
	{
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(symbolBmap_);

		dc->SetBackground(wxWHITE_BRUSH);
		dc->SetBackgroundMode(wxTRANSPARENT);

		switch (suit_)
		{
		case spades:
		case clubs:
			dc->SetPen(wxBLACK_PEN);
			break;
		case diamonds:
		case hearts:
			dc->SetPen(wxRED_PEN);
			break;
		}
			// Draw the value
		dc->Blit(x + 3, y + 3, 6, 7,
				&memoryDC, 6 * (pipValue_ - 1), 36, wxCOPY);
		dc->Blit(x + CardWidth - 9, y + CardHeight - 11, 6, 7,
				&memoryDC, 6 * (pipValue_ - 1), 43, wxCOPY);

			// Draw the pips
		dc->Blit(x + 11, y + 3, 7, 7,
				&memoryDC, 7 * suit_, 0, wxCOPY);
		dc->Blit(x + CardWidth - 17, y + CardHeight - 11, 7, 7,
				&memoryDC, 7 * suit_, 7, wxCOPY);
		switch (pipValue_)
		{
		case 1:
			dc->Blit(x - 5 + CardWidth / 2, y - 5 + CardHeight / 2, 11, 11,
	    		   &memoryDC, 11 * suit_, 14, wxCOPY);
			break;

		case 3:
			dc->Blit(x - 5 + CardWidth / 2, y - 5 + CardHeight / 2, 11, 11,
			   &memoryDC, 11 * suit_, 14, wxCOPY);
		case 2:
			dc->Blit(x - 5 + CardWidth / 2,
	    			y - 5 + CardHeight / 4, 11, 11,
	    	   		&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 2,
				y - 5 + 3 * CardHeight / 4, 11, 11,
	    	   		&memoryDC, 11 * suit_, 25, wxCOPY);
			break;

		case 5:
			dc->Blit(x - 5 + CardWidth / 2, y - 5 + CardHeight / 2, 11, 11,
			   &memoryDC, 11 * suit_, 14, wxCOPY);
		case 4:
			dc->Blit(x - 5 +  CardWidth / 4,
				y - 5 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 4,
				y - 5 + 3 * CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + 3 * CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			break;

		case 8:
			dc->Blit(x - 5 + 5 * CardWidth / 10,
				y - 5 + 5 * CardHeight / 8, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
		case 7:
			dc->Blit(x - 5 + 5 * CardWidth / 10,
				y - 5 + 3 * CardHeight / 8, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
		case 6:
			dc->Blit(x - 5 + CardWidth / 4,
				y - 5 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 4,
				y - 5 + CardHeight / 2, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 4,
				y - 5 + 3 * CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + CardHeight / 2, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + 3 * CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			break;

		case 10:
			dc->Blit(x - 5 + CardWidth / 2,
				y - 5 + 2 * CardHeight / 3, 11, 11,
	    	   		&memoryDC, 11 * suit_, 25, wxCOPY);
		case 9:
			dc->Blit(x - 5 + CardWidth / 4,
				y - 6 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 4,
				y - 6 + 5 * CardHeight / 12, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 4,
				y - 5 + 7 * CardHeight / 12, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 4,
				y - 5 + 3 * CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);

			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 6 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 6 + 5 * CardHeight / 12, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + 7 * CardHeight / 12, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			dc->Blit(x - 5 + 3 * CardWidth / 4,
				y - 5 + 3 * CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			dc->Blit(x - 5 + CardWidth / 2,
				y - 5 + CardHeight / 3, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			break;
		case 11:
		case 12:
		case 13:
			memoryDC.SelectObject(pictureBmap_);
			dc->Blit(x + 5, y - 5 + CardHeight / 4, 40, 45,
				&memoryDC, 40 * (pipValue_ - 11), 0, wxCOPY);
			memoryDC.SelectObject(symbolBmap_);
			dc->Blit(x + 32, y - 3 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 14, wxCOPY);
			dc->Blit(x + 7, y + 27 + CardHeight / 4, 11, 11,
				&memoryDC, 11 * suit_, 25, wxCOPY);
			break;
		}

		memoryDC.SelectObject(0);
	}
	dc->SetBackground(backgroundBrush);
} // Card:Draw()


//+-------------------------------------------------------------+
//| Card::DrawNullCard()										|
//+-------------------------------------------------------------+
//| Description:												|
//|	Draws the outline of a card at (x, y).						|
//|	Used to draw place holders for empty piles of cards.		|
//+-------------------------------------------------------------+
void Card::DrawNullCard(wxDC* dc, int x, int y)
{
	wxPen* pen = wxThePenList->FindOrCreatePen(FortyApp::TextColour(), 1, wxSOLID);
	dc->SetBrush(FortyApp::BackgroundBrush());
	dc->SetPen(pen);

#ifdef wx_msw
	dc->DrawRoundedRectangle(x, y, CardWidth, CardHeight, 4);
#else
	dc->DrawRoundedRectangle(x, y, CardWidth - 1, CardHeight - 1, 4);
#endif
} // Card::DrawNullCard()


