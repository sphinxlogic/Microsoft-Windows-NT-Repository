
/*
 * I. ARIT 1992 Hidirbeyli,AYDIN,TR.  09400 
 *              Golden,    CO,   USA. 80401 
 *
 *
 * Copyright (C) 1992 Ismail ARIT 
 *
 * This file  is distributed in the hope that it will be useful, but without any
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



#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include    <X11/Xlib.h>
#include    <X11/Xutil.h>

#include "definition.h"

extern int      WhiteBallOut, BlackBallOut;
extern Display  disp;
extern Window   Pane;
extern GC       gc;
extern void
ClearTable(void);
extern Distance ThisMuch;


extern void
Force_ToObject(Ball * first, Distance * TheirDistance, int type);
extern Distance *
DistanceChecker_Between(Ball * ThisBall, Ball * ThatBall);
extern
setbkcolor(int color);
extern int
get_color(char *name);
extern void
setcolor(int color);
extern
Fillcircle(int x, int y, int rad);
extern void
Fillrectangle(int x, int y, int xx, int yy);
extern void
SyncDisplay(void);
extern Ball    *
new__Ball(double Mass, double radious, float initX, float initY, double initVx, double initVy, double initVr);
extern void
LayTheFloor(void);







static void
Coordinator__ShowStaticPosition(Coordinator * this)
{
	int             i;
	for (i = 0; i < this->NumberOfObjects; i++)
		this->Objects[i]->Show(this->Objects[i]);
}

static void
Coordinator__HideStaticPosition(Coordinator * this)
{
	int             i;
	for (i = 0; i < this->NumberOfObjects; i++)
		this->Objects[i]->Hide(this->Objects[i]);
}





static void
Coordinator__OpenSpaceForObjects(Coordinator * this)
{
	/* eliminate this for this version */
	/*
	 * this -> Objects = (Ball *) malloc (this -> NumberOfObjects *
	 * sizeof (Ball)); 
	 */
};


static int
Coordinator__RegisterThisObject(Coordinator * this, Ball * AnotherBall)
{
	this->ObjectCounter++;
	if (this->ObjectCounter > this->NumberOfObjects) {
		printf("Too many Objects\n");
		exit(0);
	}
	this->Objects[this->ObjectCounter] = AnotherBall;
	this->Objects[this->ObjectCounter]->MyID = this->ObjectCounter;
	return (this->ObjectCounter);
};

static void
Coordinator__GetTime(Coordinator * this, Timing time)
{
	int             i;
	this->mytime.currenttime = 0.000000000;
	this->mytime.totaltime = time.totaltime;
	this->mytime.dt = time.dt;

	for (i = 0; i < this->NumberOfObjects; i++) {
		this->Objects[i]->mytime.currenttime = this->mytime.currenttime;
		this->Objects[i]->mytime.totaltime = this->mytime.totaltime;
		this->Objects[i]->mytime.dt = this->mytime.dt;
	}

};





static void
Coordinator__AllowableBump(Coordinator * this, float bump)
{
	this->AllowableDmax = bump;
};



static void
Coordinator__UpDateObjectsInitials(Coordinator * this)
{
	int             i;

	/* dprintf ("Coordinator::UpDateObjectsInitials()..\n"); */
	for (i = 0; i < this->NumberOfObjects; i++) {
		this->Objects[i]->CurrentF.x = 0.00000000;
		this->Objects[i]->CurrentF.y = 0.00000000;
	}
};



static void
Coordinator__GetBoundary(Coordinator * this, Space limits)
{
	this->myspace.xmin = limits.xmin;
	this->myspace.ymin = limits.ymin;
	this->myspace.xmax = limits.xmax;
	this->myspace.ymax = limits.ymax;



	/* I 'll just use ball class to create pockets */
	this->Pocket[0] = new__Ball(0, POCKETRADIOUS, this->myspace.xmin, this->myspace.ymin, 0, 0, 0);
	this->Pocket[1] = new__Ball(0, POCKETRADIOUS, this->myspace.xmin, this->myspace.ymax, 0, 0, 0);

	this->Pocket[2] = new__Ball(0, POCKETRADIOUS, this->myspace.xmax, this->myspace.ymin, 0, 0, 0);
	this->Pocket[3] = new__Ball(0, POCKETRADIOUS, this->myspace.xmax, this->myspace.ymax, 0, 0, 0);
	this->Pocket[4] = new__Ball(0, POCKETRADIOUS, this->myspace.xmin + (this->myspace.xmax - this->myspace.xmin) / 2, this->myspace.ymin, 0, 0, 0);
	this->Pocket[5] = new__Ball(0, POCKETRADIOUS, this->myspace.xmin + (this->myspace.xmax - this->myspace.xmin) / 2, this->myspace.ymax, 0, 0, 0);

	/* these are for debuging , they don't mean anything */
	this->Pocket[0]->MyID = 20;
	this->Pocket[1]->MyID = 21;
	this->Pocket[2]->MyID = 22;
	this->Pocket[3]->MyID = 23;
	this->Pocket[4]->MyID = 24;
	this->Pocket[5]->MyID = 25;


};

static void
Coordinator__DrawPockets(Coordinator * this)
{
	int             i;
	LayTheFloor();
	/* let's draw table */
	Fillrectangle(this->Pocket[0]->CurrentL.x - POCKETRADIOUS,
		      this->Pocket[0]->CurrentL.y - POCKETRADIOUS,
		      this->Pocket[3]->CurrentL.x + POCKETRADIOUS * 2,
		      this->Pocket[3]->CurrentL.y + POCKETRADIOUS * 2);

	for (i = 0; i < 6; i++)
		Fillcircle(this->Pocket[i]->CurrentL.x, this->Pocket[i]->CurrentL.y, this->Pocket[i]->radius);
    /* do I have to do this here....??!!!!%$$%&$#$ */
	SyncDisplay();

};


static void
Coordinator__CheckPockets(Coordinator * this)
{
	int             i, j;
	Distance       *theDistance;
	for (i = 0; i < 6; i++)	/* we got 6 pockets */
		for (j = 0; j < this->NumberOfObjects; j++) {
			theDistance = DistanceChecker_Between(this->Pocket[i], this->Objects[j]);

			/* this is where we check the balls with the pockets */
			if (theDistance->amount > this->Objects[j]->radius) {
				this->Objects[j]->In = NO;	/* you are out Pal!.. */
				/* just put it away */
				this->Objects[j]->CurrentL.x = this->Objects[j]->MyID * 40 + 1000;
				this->Objects[j]->CurrentL.y = 1000;
				this->Objects[j]->CurrentV.x = 0.000;
				this->Objects[j]->CurrentV.y = 0.000;


			}
		}
};



static void
Coordinator__DummyFunction(Coordinator * this)
{
	;			/* nothing  in this version*/
};

static void
Coordinator__CheckHits(Coordinator * this)
{
	;			/* nothing in this version */
};


static void
Coordinator__GetGameType(Coordinator * this, int Type)
{
	this->GameType = Type;
	if (this->GameType == SIXTEENBALL) {
		this->CheckCondition = Coordinator__CheckPockets;
		this->DrawPockets = Coordinator__DrawPockets;
	} else {
		this->CheckCondition = Coordinator__CheckHits;
		this->DrawPockets = Coordinator__DummyFunction;
	}


};






static void
Coordinator__CheckInitialLocations(Coordinator * this)
{
	int             j, i;
	Distance       *theDistance;
  /* we don't wanna check the same object with itself */
	for (i = 0; i < this->NumberOfObjects - 1; i++)
		for (j = i + 1; j < this->NumberOfObjects; j++) {
			theDistance = DistanceChecker_Between(this->Objects[i], this->Objects[j]);
			if (theDistance->amount > 0) {
				printf("Overlaping objects %d and %d \n", i, j);
				exit(0);
			}
		}

};


static void
Coordinator__AdvanceOneStep(Coordinator * this)
{

	int             j = 0, i;
	Distance       *theDistance;

    /* I commented out some dprintf(..)'s , 'cause we dont wanna spend time 
       doing if(DEBUG) here, anything we can save is good for us..*/
	/* dprintf ("Coordinator::AdvanceOneStep()..\n"); */

	for (i = 0; i < this->NumberOfObjects - 1; i++)
		for (j = i + 1; j < this->NumberOfObjects; j++) {
			theDistance = DistanceChecker_Between(this->Objects[i], this->Objects[j]);
			if (theDistance->amount > 0) {
                         /* for this version, we are gonna use fixed max overlap
                            value, which is 5 units. */ 
				if (theDistance->amount > 5)
					printf("too much overlap %d %d \n", i, j);

				/* apply reaction */
				Force_ToObject(this->Objects[i], theDistance, REACTION);
				/* apply action */
				Force_ToObject(this->Objects[j], theDistance, ACTION);

			}
		}



};



static void
Coordinator__CheckObjectsForTheSpace(Coordinator * this)
{
	int             i;
	/* don't get confused, this is just to confine balls on the table */

	for (i = 0; i < this->NumberOfObjects; i++) {
		if (
		    (this->Objects[i]->CurrentL.x + this->Objects[i]->radius) >= this->myspace.xmax ||
		    (this->Objects[i]->CurrentL.x - this->Objects[i]->radius) < this->myspace.xmin
			)
			this->Objects[i]->CurrentV.x *= -1;
		if (
		    (this->Objects[i]->CurrentL.y + this->Objects[i]->radius) >= this->myspace.ymax ||
		    (this->Objects[i]->CurrentL.y - this->Objects[i]->radius) < this->myspace.ymin
			)
			this->Objects[i]->CurrentV.y *= -1;

	}


};





static void
Coordinator__CheckSpeedsAndCalculateK(Coordinator * this)
{
	float          MaxSpeed = 0.00000, CorrespondingMass = 0.00000;
	int             i;
       /* this is where we calculate spring const. for the balls */

	if (this->AllowableDmax == 0.0000000) {
		printf("\n Missing Dmax, Not able to calculate K, Exiting....\n");
		exit(0);
	}
	MaxSpeed = SPEEDLIMIT;
	CorrespondingMass = PAYLOAD;

    /* we use max speed that we allowed, and the payload for that speed */
	this->kConstant = CorrespondingMass * pow(MaxSpeed, 2.) / pow(this->AllowableDmax, 2.);

	for (i = 0; i < this->NumberOfObjects; i++)
		this->Objects[i]->kConstant = this->kConstant;


	dprintf("Max Speed = %f  kKons %f Dmax %f \n", MaxSpeed,
		this->kConstant,
		this->AllowableDmax);

};



static void
Coordinator__ShowObjects(Coordinator * this)
{
	int             i;
	for (i = 0; i < this->NumberOfObjects; i++)
		this->Objects[i]->MoveToNextLocation(this->Objects[i]);
};

/* another quick fix to arrange  the balls */
int             which[15] = {2, 15, 3, 14, 4, 13, 5, 12, 1, 11, 7, 10, 8, 9, 0};

static void
Coordinator__ArrangeBalls(Coordinator * this)
{
	int             i, j, counter = 0, ii;


	/* this is what happens when you click on 'newgame' */

	/* for white ball */
	this->Objects[0]->CurrentL.x = 600;
	this->Objects[0]->CurrentL.y = 230;
	/* black ball  will be somewhere in the middle */
	this->Objects[6]->CurrentL.x = 200;
	this->Objects[6]->CurrentL.y = 230;

	/* for the others */
	for (j = 0; j < 2; j++)
		for (i = 0; i < 4; i++) {

			ii = (j == 0) ? (i + 1) : (-i - 1);
			this->Objects[which[counter]]->CurrentL.x = 200 - (i + 1) * 27;
			this->Objects[which[counter]]->CurrentL.y = 230 - (ii) * 20;
			counter++;

		}

	this->Objects[which[counter]]->CurrentL.x = 200 - 54;
	this->Objects[which[counter]]->CurrentL.y = 230;
	counter++;

	for (j = 0; j < 2; j++)
		for (i = 0; i < 2; i++) {

			ii = (j == 0) ? (i + 1) : (-i - 1);
			this->Objects[which[counter]]->CurrentL.x = 145 - (i + 1) * 27;
			this->Objects[which[counter]]->CurrentL.y = 230 - (ii) * 20;
			counter++;

		}

	this->Objects[which[counter]]->CurrentL.x = 90;
	this->Objects[which[counter]]->CurrentL.y = 230;





};

static void
Coordinator__StillMoving(Coordinator * this)
{
	int             i;
	int             Movelist = 0;
	for (i = 0; i < this->NumberOfObjects; i++) {
		if (this->Objects[i]->Stopped == NO)
			Movelist++;
	}

	if (Movelist)
		this->AllStopped = NO;
	else
		this->AllStopped = YES;

};

static void
Coordinator__InitConfig(Coordinator * this)
{
	int             i;
	this->DrawPockets(this);
	ClearTable();
	setbkcolor(get_color(PANE_BG));
	if (this->GameType == SIXTEENBALL) /* this is the only game in town, for NOW!!!*/
		this->ArrangeBalls(this);
	this->CheckInitialLocations(this);
	this->ShowStaticPosition(this);
	for (i = 0; i < this->NumberOfObjects; i++) {
           /* init every ball */
		this->Objects[i]->Stopped = NO;
		this->Objects[i]->In = YES;
		this->Objects[i]->ShowOut = NO;
		this->Objects[i]->NeedOneMoreDrawing = YES;
	}
	WhiteBallOut = BlackBallOut = NO;

};



static void
Coordinator__StartAction(Coordinator * this)
{

	int             Counter = 0;
        int             CounterForPocket = 0;

	this->mytime.currenttime = 0.000;
	this->AllStopped = NO;

	/* dprintf ("Coordinator::StartAction()..\n"); */
	while (!this->AllStopped) {
		this->CheckObjectsForTheSpace(this);	/* table boundaries */
		this->UpDateObjectsInitials(this);	/* init forces */
		this->AdvanceOneStep(this);	/* do checkups and advance */
		this->mytime.currenttime += this->mytime.dt;
		this->ShowObjects(this);	/* show balls */
                CounterForPocket++;
                 if (!(CounterForPocket % CHECKPOCKETDENSITY))
		        this->CheckCondition(this);	/* new locations in pockets ? */
		Counter++;
		if (!(Counter % CHECKMOVEDENSITY))
			this->StillMoving(this);	/* make sure everybody
							 * stopped */

	}

};





Coordinator    *
new__Coordinator(int HowManyObjects)
{
	Coordinator    *A_Coordinator;

	if ((A_Coordinator = (Coordinator *) malloc(sizeof(Coordinator))) == NULL) {
		printf("problem..\n");
		exit(0);
	};

	A_Coordinator->NumberOfObjects = HowManyObjects;
	A_Coordinator->AllStopped = NO;
	A_Coordinator->ObjectCounter = -1;
	A_Coordinator->Counter = 0;


	A_Coordinator->AllowableDmax = 2.000000;
	A_Coordinator->GetGameType = Coordinator__GetGameType;
	A_Coordinator->UpDateObjectsInitials = Coordinator__UpDateObjectsInitials;
	A_Coordinator->GetTime = Coordinator__GetTime;
	A_Coordinator->OpenSpaceForObjects = Coordinator__OpenSpaceForObjects;
	A_Coordinator->RegisterThisObject = Coordinator__RegisterThisObject;
	A_Coordinator->CheckInitialLocations = Coordinator__CheckInitialLocations;
	A_Coordinator->AdvanceOneStep = Coordinator__AdvanceOneStep;
	A_Coordinator->StartAction = Coordinator__StartAction;
	A_Coordinator->GetBoundary = Coordinator__GetBoundary;
	A_Coordinator->CheckSpeedsAndCalculateK = Coordinator__CheckSpeedsAndCalculateK;
	A_Coordinator->CheckObjectsForTheSpace = Coordinator__CheckObjectsForTheSpace;
	A_Coordinator->ShowObjects = Coordinator__ShowObjects;
	A_Coordinator->AllowableBump = (void *)Coordinator__AllowableBump;
	A_Coordinator->ArrangeBalls = Coordinator__ArrangeBalls;
	A_Coordinator->ShowStaticPosition = Coordinator__ShowStaticPosition;
	A_Coordinator->HideStaticPosition = Coordinator__HideStaticPosition;
	A_Coordinator->DrawPockets = Coordinator__DrawPockets;
	A_Coordinator->CheckCondition = Coordinator__CheckPockets;
	A_Coordinator->StillMoving = Coordinator__StillMoving;
	A_Coordinator->InitConfig = Coordinator__InitConfig;


	return (A_Coordinator);

};
