//+-------------------------------------------------------------+
//| File:	scorefil.h											|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
#ifndef _SCOREFILE_H_
#define _SCOREFILE_H_

class ScoreFile {
public:
	ScoreFile(const char* appName);
	virtual ~ScoreFile();

	void GetPlayerList(char*** list, int& length);
	Bool PlayerExists(const char* player);
	void GetPreviousPlayer(char** player);

	void ReadPlayersScore(const char* player, int& wins, int& games, int &score);
	void WritePlayersScore(const char* player, int wins, int games, int score);

private:
	long CalcCheck(const char* name, int p1, int p2, int p3);
	char* m_file;
	char* m_players;
};

#endif
