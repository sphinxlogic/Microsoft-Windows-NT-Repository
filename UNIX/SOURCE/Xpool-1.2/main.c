

/*
 * I. ARIT 1992 Hidirbeyli,AYDIN,TR.  09400 
 *              Golden,    CO,   USA. 80401 
 *
 *
 * Copyright (C) 1992 Ismail ARIT 
 *
 * This file is distributed in the hope that it will be useful,but without any
 * warranty.  No author or distributor accepts responsibility to anyone for
 * the consequences of using it or for whether it serves any particular
 * purpose or works at all. 
 *
 *
 * Everyone is granted permission to copy, modify and redistribute this file
 * under the following conditions: 
 *
 * Permission is granted to anyone to make or distribute copies of the source
 * code, either as received or modified, in any medium, provided that all
 * copyright notices, permission and nonwarranty notices are preserved, and
 * that the distributor grants the recipient permission for further
 * redistribution as permitted by this document. 
 *
 * No part of this program can be used in any commercial product. 
 */



#include <string.h>
#include <X11/Xlib.h>
#include "definition.h"


extern char    *progname;
extern Display  disp;
extern Window   Pane;
extern GC       gc;
extern int      file_type;
extern Ball    *
new__Ball(double Mass, double radious, float initX, float initY, double initVx, double initVy, double initVr);

extern Coordinator *
new__Coordinator(int HowManyObjects);
extern Stick   *
new__Stick(int type);


/* at the beginning */
int             WhiteBallOut = NO;
int             BlackBallOut = NO;

int             YourScore = 0, MyScore = 0;
/* these are for stick position */
int             StartX, StartY, NewX, NewY;


static char    *MY_BALL_COLOR[7] = {"Yellow", "Red", "Orange", "Maroon",
                                    "FireBrick", "Navy", "Violet"};
				    

static char    *YOUR_BALL_COLOR[7] = {"Yellow", "Red", "Orange", "Maroon",
                                      "FireBrick", "Navy", "Violet"};

				      







Space           PlayGround;
Timing          PlayTime;





/* this is the big guy, controlling everything */
Coordinator    *Dept_Head;

Stick          *PoolStick;
Stick          *Trajectory;

Ball           *WhiteBall, *BlackBall;
Ball           *DottedWhiteBall;/* we'll use it for 3-ball pool */
Ball           *MyBalls[7];
Ball           *YourBalls[7];


main(argc, argv)
	int             argc;
	char          **argv;
{
	int             i;
	int             gameType, HowManyBalls;


	PlayGround.xmin = 1.000000;
	PlayGround.ymin = 1.000000;
	PlayGround.xmax = 745.000000;
	PlayGround.ymax = 460.000000;
	PlayTime.currenttime = 0.000;
	PlayTime.totaltime = 100000.0000;	/* you can play max 100000
						 * sec. */
	/* seconds */
	PlayTime.dt = 0.040000;	/* don't change this value without adjusting
				 * others */


	/* here are the defaults */
	gameType = SIXTEENBALL;
	HowManyBalls = SIXTEENBALL;

	initgraph(argc, argv);
	init_colors();


	PoolStick = new__Stick(STICK);
	Trajectory = new__Stick(TRAJECTORY);

	PoolStick->GetColor(PoolStick, get_color(STICK_COLOR));
	Trajectory->GetColor(Trajectory, get_color(STICK_COLOR));



	Dept_Head = new__Coordinator(HowManyBalls);

	/* we need these balls anyway */
	WhiteBall = new__Ball(20, BALLRADIUS, 600, 230, 50, 0, 0);
	BlackBall = new__Ball(20, BALLRADIUS, 200, 230, 0.1, 0, 0);

	WhiteBall->MyColor_is(WhiteBall, WHITE_BALL);
	BlackBall->MyColor_is(BlackBall, BLACK_BALL);

	WhiteBall->MyIDis(WhiteBall, Dept_Head->RegisterThisObject(Dept_Head, WhiteBall));
	BlackBall->MyIDis(BlackBall, Dept_Head->RegisterThisObject(Dept_Head, BlackBall));




	/* now let's see what user wants to play */
	if (gameType == SIXTEENBALL) {
		for (i = 0; i < 7; i++) {
			MyBalls[i] = new__Ball(20, BALLRADIUS, 100 + i * 42, 400, 0.1, 0, 0);
			MyBalls[i]->MyColor_is(MyBalls[i], MY_BALL_COLOR[i]);
			MyBalls[i]->MyIDis(MyBalls[i], Dept_Head->RegisterThisObject(Dept_Head, MyBalls[i]));
		}
		for (i = 0; i < 7; i++) {
			YourBalls[i] = new__Ball(20, BALLRADIUS, 100 + i * 42, 300, 0.1, 0, 0);
			YourBalls[i]->MyColor_is(YourBalls[i], YOUR_BALL_COLOR[i]);
			YourBalls[i]->MyIDis(YourBalls[i], Dept_Head->RegisterThisObject(Dept_Head, YourBalls[i]));

		}
	} else {
		DottedWhiteBall = new__Ball(20, BALLRADIUS, 100, 250, 0.1, 0, 0);
		DottedWhiteBall->MyColor_is(DottedWhiteBall, YOUR_BALLS_COLOR);
		DottedWhiteBall->MyIDis(DottedWhiteBall, Dept_Head->RegisterThisObject(Dept_Head, DottedWhiteBall));
	}



	Dept_Head->GetGameType(Dept_Head, gameType);

	Dept_Head->GetBoundary(Dept_Head, PlayGround);

	Dept_Head->GetTime(Dept_Head, PlayTime);

	/* Dept_Head -> AllowableBump (Dept_Head, 4.000000); */

	Dept_Head->CheckSpeedsAndCalculateK(Dept_Head);


	while (1) {
		take_event();
	}

}
