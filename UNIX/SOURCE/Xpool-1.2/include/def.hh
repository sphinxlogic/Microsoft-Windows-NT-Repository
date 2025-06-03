
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


#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_OBJECTS 19
#define MENU_FG           "Black"
#define MENU_BG           "Gray"

#define MENU_PRESS_COLOR   "Wheat"

#define PANE_FG  "White"
#define PANE_BG    "SkyBlue"

#define MY_BALLS_COLOR  "Red"

#define YOUR_BALLS_COLOR "Blue"

#define STICK_COLOR  "white"	/* no matter what you define, it looks
				 * similar on screen with GXxor */
#define WHITE_BALL  "black"
#define BLACK_BALL  "Black"

#define BORDER_COLOR  "Brown"

#define MAIN_BG   "Wheat"
#define MAIN_FG   "Black"
#define RED      "Red"

#define THREEBALL   3
#define SIXTEENBALL 16
#define MINSPEED    2		/* if you are under 1.00, you die ( I mean
				 * 'stop') */
#define SPEEDLIMIT 50		/* don't worry, it's not for you */
#define PAYLOAD    20
#define GRAVITY    10
#define DROPDEADFACTOR .001	/* this is friction */
#define POCKETRADIOUS 25	/* this is not standard, I don't know the
				 * original size */
#define BALLRADIUS  14.0	/* for better graphics (actually bitmaps
				 * 12.5) */

#define MAXSTICKLENGHT   400
#define CHECKMOVEDENSITY 50
#define MYTURN   1
#define YOURTURN 0
#define WHITE_BALL_LIMIT 550	/* if you want to move the white ball, you
				 * can't pass this x limit */

#define BALL_RADIUS  13.5
#define INCR         0.01
#define FONT         "9x15"
#define BORDER       1
#define BUTTONCURSOR  30	/* I guess this is for cross ptr */
/*
 * #define BUTTONCURSOR  XC_center_ptr 
 */

#define STICK  1
#define TRAJECTORY 0



#define DEBUG   0
#define YES     1
#define NO      0
#define OKEY    0
#define FRAMEDENSITY 17		/* how often you want to redraw moving balls */
#define REACTION 1
#define ACTION  0
#define dprintf if(DEBUG)printf
#define bitmapMode YES		/* if you say NO, you will see circles on the
				 * screen not bitmaps */


/* if you want to refresh balls every time, comment out the following  */
#define REFRESH_EVERY_TIME




#define class typedef struct



typedef struct Segment {
	int             x, y, xx, yy;
}               Segment;

typedef struct Velocity {
	double          x, y
	               ;
}               Velocity;
typedef struct Location {
	double          x, y
	               ;
}               Location;
typedef struct Distance {
	double          amount, x, y;
}               Distance;

typedef struct Timing {
	double          currenttime, totaltime, dt;
}               Timing;


typedef struct Space {
	float           xmin, ymin, xmax, ymax;
}               Space;

typedef struct Force {
	double          x, y
	               ;
}               Force;



class           Stick
{

	unsigned int    color;
	int             Visible;
	int             Done;
	int             type;
	Segment         Old, New;

	void            (*Show) ();
	void            (*Hide) ();
	void            (*MoveTo) ();
	void            (*GetColor) ();
	void            (*DoneWithTheStick) ();
	float           (*Lenght) ();


}               Stick;



class           Ball
{

	double          mass, radius;
	int             MyID;
	double          Ip;

	Velocity        CurrentV, OneStepBackV;
	Location        CurrentL, OneStepBackL, TwoStepBackL;
	Location        LastDrawingL;
	Force           CurrentF, OneStepBackF;
	Timing          mytime;
	long            framecounter;
	int             ListCounter;
	int             mycolor, erasecolor;
	double          kConstant;
	int             In;
	int             Visible;
	int             Stopped;
	int             ShowOut;
	int             NeedOneMoreDrawing;

	void            (*MyColor_is) ();
	void            (*MyIDis) ();
	void            (*UpdateVelocity) ();
	void            (*UpdateLocation) ();
	void            (*UpdateForce) ();
	void            (*UpdateMe) ();
	void            (*MoveToNextLocation) ();
	void            (*Hide) ();
	void            (*Show) ();
	void            (*NewSpeed) ();



}               Ball;





class           Coordinator
{

	Ball           *Objects[MAX_OBJECTS];

	Ball           *Pocket[6];	/* I assume we have 6 pockets */

	Space           myspace;
	Timing          mytime;
	int             NumberOfObjects;
	int             ObjectCounter;
	double          AllowableDmax;
	double          kConstant;
	int             GameType;
	int             AllStopped;
	long            Counter;


	void            (*ShowStaticPosition) ();
	void            (*HideStaticPosition) ();
	void            (*DrawPockets) ();
	void            (*GetGameType) ();
	void            (*UpDateObjectsInitials) ();
	void            (*GetTime) ();
	void            (*OpenSpaceForObjects) ();
	int             (*RegisterThisObject) ();
	void            (*CheckInitialLocations) ();
	void            (*AdvanceOneStep) ();
	void            (*StartAction) ();
	void            (*GetBoundary) ();
	void            (*CheckSpeedsAndCalculateK) ();
	void            (*CheckObjectsForTheSpace) ();
	void            (*ShowObjects) ();
	void            (*AllowableBump) ();
	void            (*ArrangeBalls) ();
	void            (*CheckCondition) ();
	void            (*StillMoving) ();
	void            (*InitConfig) ();


}               Coordinator;
