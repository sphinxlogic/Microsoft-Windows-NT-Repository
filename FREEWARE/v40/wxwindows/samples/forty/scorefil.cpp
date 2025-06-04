//+-------------------------------------------------------------+
//| File:	scorfil.cpp											|
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

#ifdef wx_msw
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "scorefil.h"


ScoreFile::ScoreFile(const char* appName) :
	m_players(0)
{
	ostrstream os;
#ifdef wx_msw
	os << appName << ".ini" << '\0';
#else
	os << appName << "rc" << '\0';
#endif
	m_file = os.str();
}

ScoreFile::~ScoreFile()
{
	delete m_file;
	delete m_players;
}


void ScoreFile::GetPlayerList(char*** list, int& length)
{
	length = 0;
	delete m_players;
	m_players = 0;
	if (::wxGetResource("General", "Players", &m_players, m_file))
	{
		// first count the number of entries
		char* str = m_players;
		while (*str)
		{
			while (*str == '@') str++;
			if (*str)
			{
				length++;
				while (*str && *str != '@') str++;
			}
		}

		// then split them into a list
		*list = new char*[length];
		str = m_players;
		int i = 0;
		while (*str)
		{
			while (*str == '@') str++;
			if (*str)
			{
				(*list)[i++] = str;
				while (*str && *str != '@') str++;
				if (*str == '@') *str++ = '\0';
			}
		}
	}
}


// Calculate an encrypted check number to prevent tampering with
// score file
long ScoreFile::CalcCheck(const char* name, int p1, int p2, int p3)
{
    long check = 0;
    while (*name)
	{
		check = (check << 1) ^ (long)*name++;
		check = ((check >> 23) ^ check) & 0xFFFFFF;
	}
	check = (check << 1) ^ (long)p1;
	check = ((check >> 23) ^ check) & 0xFFFFFF;
	check = (check << 1) ^ (long)p2;
	check = ((check >> 23) ^ check) & 0xFFFFFF;
	check = (check << 1) ^ (long)p3;
	check = ((check >> 23) ^ check) & 0xFFFFFF;
    return check;
}



Bool ScoreFile::PlayerExists(const char* player)
{
	Bool exists = FALSE;
	char* players = 0;
	if (::wxGetResource("General", "Players", &players, m_file))
	{
		ostrstream os;
		os << '@' << player << '@' << '\0';
		char* str = os.str();
		exists = players && strstr(players, str);
		delete str;
	}
	delete [] players;
	return exists;
}


void ScoreFile::GetPreviousPlayer(char** player)
{
	*player = 0;
	wxGetResource("General", "LastPlayer", player, m_file);
}

void ScoreFile::ReadPlayersScore(
						const char* player,
						int& wins,
						int& games,
						int& score)
{
	long check;

	if (::wxGetResource("Score", player, &score, m_file) &&
		::wxGetResource("Games", player, &games, m_file) &&
		::wxGetResource("Wins",  player, &wins,  m_file) &&
		::wxGetResource("Check", player, &check, m_file))
	{
	    if (check != CalcCheck(player, games, wins, score))
		{
			wxMessageBox("Score file corrupted", "Warning",
						wxOK | wxICON_EXCLAMATION);
			games = wins = score = 0;
		}
	}
	else
	{
		games = wins = score = 0;
	}
	WritePlayersScore(player, wins, games, score);
}


void ScoreFile::WritePlayersScore(const char* player, int wins, int games, int score)
{
    if (player)
	{
		if (!PlayerExists(player))
		{
			ostrstream os;

			char* players = 0;
			if (::wxGetResource("General", "Players", &players, m_file))
			{
				os << players;
			}
			delete [] players;

			os << '@' << player << '@' << '\0';
			char* str = os.str();
			::wxWriteResource("General",  "Players", str,  m_file);
			delete str;
		}

		long check = CalcCheck(player, games, wins, score);

		if (!(::wxWriteResource("Score", player, score, m_file) &&
			  ::wxWriteResource("Games", player, games, m_file) &&
			  ::wxWriteResource("Wins",  player, wins,  m_file) &&
			  ::wxWriteResource("Check", player, check, m_file) &&
			  ::wxWriteResource("General", "LastPlayer", (char*)player, m_file)))
		{
    		wxMessageBox("Error writing score file", "Warning",
	    			   wxOK | wxICON_EXCLAMATION);
		}
	}
}
