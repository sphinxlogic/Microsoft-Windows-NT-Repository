


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




#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include    <X11/Xlib.h>
#include    <X11/Xutil.h>

#include "definition.h"


extern void
line(int x, int y, int xx, int yy);
extern void
circle(int x, int y, int rad);
extern void
circleOut(int x, int y, int rad, int color);
extern int
get_color(char *name);
extern void
showBall(int x, int y, int number);
extern void
showBallOut(int x, int y, int color, int number);

extern int      WhiteBallOut;
extern int      BlackBallOut;


static void
Ball__MyColor_is(Ball * this, char *color)
{
	this->mycolor = get_color(color);
};


static void
Ball__MyIDis(Ball * this, int id)
{
	this->MyID = id;
};

static void
Ball__UpdateVelocity(Ball * this)
{
  float temp = this->mytime.dt / this->mass ;

	this->CurrentV.x += this->CurrentF.x * temp;
	this->CurrentV.y += this->CurrentF.y * temp;
	/* friction */
	this->CurrentV.x -= DROPDEADFACTOR * this->CurrentV.x;
	this->CurrentV.y -= DROPDEADFACTOR * this->CurrentV.y;

	/*
	 * dprintf ("problem with %d  %f %f \n", this -> MyID, this ->
	 * CurrentV.x, this -> CurrentV.y); 
	 */
	if ((fabs(this->CurrentV.x) < MINSPEED) &&
	    (fabs(this->CurrentV.y) < MINSPEED)) {
		this->CurrentV.x = 0.0;
		this->CurrentV.y = 0.0;
		this->Stopped = YES;
	} else
		this->Stopped = NO;


};


static void
Ball__Hide(Ball * this)
{
	setcolor(this->erasecolor);
	showBall(this->LastDrawingL.x - this->radius, this->LastDrawingL.y - this->radius, this->MyID);
	this->Visible = NO;
};


static void
Ball__Show(Ball * this)
{
	setcolor(this->mycolor);
	showBall(this->CurrentL.x - this->radius, this->CurrentL.y - this->radius, this->MyID);
	this->Visible = YES;
	this->LastDrawingL.x = this->CurrentL.x;
	this->LastDrawingL.y = this->CurrentL.y;

};




static void
Ball__MoveToNextLocation(Ball * this)
{
	int             x, y;
	if (this->In) {
		this->UpdateVelocity(this);
		this->UpdateLocation(this);

		if (((this->CurrentL.x != this->LastDrawingL.x) &&
		     (this->CurrentL.y != this->LastDrawingL.y))) {

			if (!(this->framecounter % FRAMEDENSITY)) {

				this->Hide(this);	/* delete the old one */
				this->Show(this);	/* put the new one */
			}
		} else if (this->NeedOneMoreDrawing) {
			this->Show(this);	/* put the new one */
			this->NeedOneMoreDrawing = NO;
		}
		this->UpdateMe(this);
	} else if (this->Visible == YES) {
		this->Stopped = YES;
		this->Hide(this);
		if (!this->ShowOut) {
			if (this->MyID > 1 && this->MyID < 9) {
				x = 60;
				y = 160 + (this->MyID - 2) * 30;
			}
			if (this->MyID > 8 && this->MyID < 16) {
				x = 10;
				y = 160 + (this->MyID - 9) * 30;
			}
			if (this->MyID == 0)
				WhiteBallOut = YES;
			if (this->MyID == 1)
				BlackBallOut = YES;

			this->ShowOut = YES;

			if (this->MyID > 1)	/* one of my/your balls is
						 * out */
				showBallOut(x, y, this->mycolor, this->MyID);

		}
	}
};



static void
Ball__UpdateLocation(Ball * this)
{
	this->CurrentL.x += this->CurrentV.x * this->mytime.dt;
	this->CurrentL.y += this->CurrentV.y * this->mytime.dt;

};



static void
Ball__UpdateMyself(Ball * this)
{

	this->OneStepBackL.x = this->CurrentL.x;
	this->OneStepBackL.y = this->CurrentL.y;

	/*
	 * this -> TwoStepBackL.x = this -> OneStepBackL.x; this ->
	 * TwoStepBackL.y = this -> OneStepBackL.y; 
	 */
	this->OneStepBackV.x = this->CurrentV.x;
	this->OneStepBackV.y = this->CurrentV.y;

	this->framecounter++;

};





static void
Ball__UpdateForce(Ball * this, Force * NewForce)
{
	this->CurrentF.x += NewForce->x;
	this->CurrentF.y += NewForce->y;
};

static void
Ball__NewSpeed(Ball * this, float Vx, float Vy)
{
	this->CurrentV.x = Vx;
	this->CurrentV.y = Vy;
	this->Stopped = NO;
};



Ball           *
new__Ball(double Mass, double radious, float initX, float initY, double initVx, double initVy, double initVr)
{
	Ball           *A_Ball;
	A_Ball = (Ball *) malloc(sizeof(Ball));

	if (!A_Ball) {
		printf("problem..\n");
		exit(0);
	};

	A_Ball->mass = Mass;
	A_Ball->radius = radious;
	A_Ball->In = YES;	/* everybody is in,  at the begining */
	A_Ball->kConstant = 10.00;
	A_Ball->Stopped = NO;
	A_Ball->ShowOut = NO;
	A_Ball->CurrentL.x = initX;
	A_Ball->CurrentL.y = initY;
	A_Ball->OneStepBackL.x = 0.00000000;
	A_Ball->OneStepBackL.y = 0.00000000;

	A_Ball->TwoStepBackL.x = initX;
	A_Ball->TwoStepBackL.y = initY;
	A_Ball->LastDrawingL.x = 2000.00000000;	/* so what? */
	A_Ball->LastDrawingL.y = 2000.00000000;
	A_Ball->CurrentV.x = initVx;
	A_Ball->CurrentV.y = initVy;
	A_Ball->Ip = A_Ball->mass * A_Ball->radius * A_Ball->radius / 2.;
	A_Ball->Visible = NO;
	A_Ball->NeedOneMoreDrawing = YES;
	A_Ball->framecounter = 0;

	A_Ball->mycolor = get_color(RED);
	A_Ball->erasecolor = get_color(PANE_BG);

	A_Ball->UpdateVelocity = Ball__UpdateVelocity;
	A_Ball->MyColor_is = Ball__MyColor_is;
	A_Ball->MyIDis = Ball__MyIDis;
	A_Ball->UpdateLocation = Ball__UpdateLocation;
	A_Ball->UpdateForce = Ball__UpdateForce;
	A_Ball->UpdateMe = Ball__UpdateMyself;
	A_Ball->MoveToNextLocation = Ball__MoveToNextLocation;
	A_Ball->Hide = Ball__Hide;
	A_Ball->Show = Ball__Show;
	A_Ball->NewSpeed = (void *)Ball__NewSpeed;

	dprintf("  %f %f \n", A_Ball->CurrentL.y, A_Ball->CurrentL.x);

	return (A_Ball);

};
