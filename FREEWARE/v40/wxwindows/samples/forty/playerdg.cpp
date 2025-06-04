//+-------------------------------------------------------------+
//| File:	playerdg.cpp										|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
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

#include "scorefil.h"
#include "playerdg.h"

// Function		: browsePress 
// Description	: Processes the event generated when browse button pressed
// Comments		: This is a static function because it is a callback
static void btnCB (wxButton& b, wxCommandEvent& WXUNUSED(event))
{
	PlayerSelectionDialog* dialog = (PlayerSelectionDialog*)b.GetParent();
	dialog->ButtonCallback(b);
}

static void selectCB(wxListBox&		list,
					 wxCommandEvent&	WXUNUSED(event))
{
	PlayerSelectionDialog* dialog = (PlayerSelectionDialog*)list.GetParent();
	dialog->SelectCallback(list);
}

PlayerSelectionDialog::PlayerSelectionDialog(
							wxWindow* parent,
							ScoreFile* file
							) :
	wxDialogBox(parent, "Player Selection", TRUE, -1, -1, 200, 200),
	m_scoreFile(file),
	m_player(0)
{
	// enable constraints
	SetAutoLayout (TRUE);

	wxMessage* msg = new wxMessage(this, "Please select a name from the list");

	int numPlayers;
	char** players;
	m_scoreFile->GetPlayerList(&players, numPlayers);
	wxListBox* list = new wxListBox(
						this, (wxFunction)selectCB, 0, wxSINGLE,
						-1, -1, -1, -1,
						numPlayers, players,
						wxNEEDED_SB
						);
	if (numPlayers > 0)
	{
		delete [] players;
	}

	m_textField = new wxText(this, 0, "Name:");

	m_OK = new wxButton(this, (wxFunction)btnCB, "OK");
	m_cancel = new wxButton(this, (wxFunction)btnCB, "Cancel");

	wxLayoutConstraints* layout;

	// Constrain the msg at the top of the window
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->top.SameAs		(this,	wxTop,	10);
	layout->height.AsIs();
	layout->width.AsIs();
	msg->SetConstraints(layout);

	// Constrain the OK button
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->bottom.SameAs	(this,	 wxBottom,	10);
	layout->height.AsIs();
	layout->width.AsIs();
	m_OK->SetConstraints(layout);

	// Constrain the OK button
	layout = new wxLayoutConstraints;
	layout->left.RightOf	(m_OK,	10);
	layout->bottom.SameAs	(this,	wxBottom,	10);
	layout->height.AsIs();
	layout->width.AsIs();
	m_cancel->SetConstraints(layout);

	// Constrain the Name text entry field
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->right.SameAs	(this,	wxRight,	10);
	layout->bottom.SameAs	(m_OK,	wxTop,		10);
	layout->height.AsIs();
	m_textField->SetConstraints(layout);

	// Constrain the list of players
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->right.SameAs	(this,	wxRight,	10);
	layout->top.Below		(msg,	10);
	layout->bottom.SameAs	(m_textField,	wxTop,	10);
	list->SetConstraints(layout);

	char* prevPlayer;
	m_scoreFile->GetPreviousPlayer(&prevPlayer);
	if (prevPlayer)
	{
		list->SetStringSelection(prevPlayer);
		m_textField->SetValue(prevPlayer);
		delete prevPlayer;
	}
}

PlayerSelectionDialog::~PlayerSelectionDialog()
{
}

char* PlayerSelectionDialog::GetPlayersName()
{
	Show(TRUE);
	return m_player;
}

Bool PlayerSelectionDialog::OnClose()
{
	// hide the dialog
	// NB don't return TRUE otherwise delete is called
	m_player = 0;
	Show(FALSE);
	return FALSE;
}

void PlayerSelectionDialog::SelectCallback(wxListBox& list)
{
	m_textField->SetValue(list.GetStringSelection());
}

void PlayerSelectionDialog::ButtonCallback(wxButton& button)
{
	if (&button == m_OK)
	{
		char* name = m_textField->GetValue();
		if (*name && strlen(name) > 0)
		{
			if (strchr(name, '@'))
			{
				wxMessageBox("Names should not contain the '@' character", "Forty Thieves");
			}
			else
			{
				m_player = new char[strlen(name) + 1];
				strcpy(m_player, name);
				Show(FALSE);
			}
		}
		else
		{
			wxMessageBox("Please enter your name", "Forty Thieves");
		}
	}
	else
	{
		m_player = 0;
		Show(FALSE);
	}
}
