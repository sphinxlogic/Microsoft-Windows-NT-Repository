//+-------------------------------------------------------------+
//| File:	playerdg.h											|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
#ifndef _PLAYERDG_H_
#define _PLAYERDG_H_

class PlayerSelectionDialog : public wxDialogBox
{
public:
	PlayerSelectionDialog(wxWindow* parent, ScoreFile* file);
	virtual ~PlayerSelectionDialog();

	char* GetPlayersName();
	void ButtonCallback(wxButton& button);
	void SelectCallback(wxListBox& list);

protected:
	friend void ButtonCallback(wxButton&, wxCommandEvent&);
	friend void SelectCallback(wxListBox&, wxCommandEvent&);
	Bool OnClose();

private:
	ScoreFile*	m_scoreFile;
	char*		m_player;
	wxButton*	m_OK;
	wxButton*	m_cancel;
	wxText*		m_textField;
};

#endif
