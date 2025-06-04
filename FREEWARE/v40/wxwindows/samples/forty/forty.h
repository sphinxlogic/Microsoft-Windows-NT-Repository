//+-------------------------------------------------------------+
//| File:	forty.h												|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
#ifndef _FORTY_H_
#define _FORTY_H_

class FortyApp: public wxApp
{
public:
	wxFrame* OnInit();
	static wxColour* BackgroundColour();
	static wxColour* TextColour();
	static wxBrush*  BackgroundBrush();

private:
	static wxColour* m_backgroundColour;
	static wxColour* m_textColour;
	static wxBrush*  m_backgroundBrush;
};

#endif
