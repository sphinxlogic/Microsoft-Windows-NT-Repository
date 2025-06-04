//+-------------------------------------------------------------+
//| File:	scoredg.h											|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
#ifndef _SCOREDG_H_
#define _SCOREDG_H_

class ScoreDialog : public wxDialogBox
{
public:
	ScoreDialog(wxWindow* parent, ScoreFile* file);
	virtual ~ScoreDialog();

	void Display();
	void ButtonCallback(wxButton& button);
	void SelectCallback(wxListBox& list);

protected:
	friend void ButtonCallback(wxButton&, wxCommandEvent&);
	friend void SelectCallback(wxListBox&, wxCommandEvent&);
	Bool OnClose();

private:
	ScoreFile*	m_scoreFile;
	wxButton*	m_OK;
};

#endif
