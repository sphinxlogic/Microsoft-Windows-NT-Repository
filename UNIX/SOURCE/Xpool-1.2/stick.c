

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



extern char    *progname;
extern Display  disp;
extern Window   Pane;
extern GC       gc;
extern GC       XorGc;


extern void
RubberLineOnPane(int type, int x, int y, int xx, int yy, int color);
extern int
get_color(char *name);


static void
Stick_Show(Stick * this)
{
	RubberLineOnPane(this->type, this->New.x, this->New.y, this->New.xx, this->New.yy, this->color);
	this->Visible = YES;

};

static void
Stick_Hide(Stick * this)
{
	RubberLineOnPane(this->type, this->Old.x, this->Old.y, this->Old.xx, this->Old.yy, this->color);

};

static void
Stick_MoveTo(Stick * this, int x, int y, int xx, int yy)
{
	this->New.x = x;
	this->New.y = y;
	this->New.xx = xx;
	this->New.yy = yy;
	this->Hide(this);
	this->Show(this);
	this->Old.x = this->New.x;
	this->Old.xx = this->New.xx;
	this->Old.y = this->New.y;
	this->Old.yy = this->New.yy;

};

static void
Stick_GetColor(Stick * this, int color)
{
	this->color = color;
};
static void
Stick_Status(Stick * this, int status)
{
	this->Done = status;
	if (this->Done) {
		this->Old.x = this->Old.y = 0;
		this->Old.xx = this->Old.yy = 0;
	}
};

float
Stick_Lenght(Stick * this)
{
	return (sqrt((float) ((this->New.xx - this->New.x) * (this->New.xx - this->New.x)
			      +
		 (this->New.yy - this->New.y) * (this->New.yy - this->New.y)
			      )));
};


Stick          *
new__Stick(int type)
{

	Stick          *this;

	this = (Stick *) malloc(sizeof(Stick));
	if (!this) {
		printf("problem..\n");
		exit(0);
	}
	/* this is default stick color */
	this->color = get_color("red");
	this->Done = NO;
	this->Visible = NO;
	this->type = type;
	this->Old.x = this->New.x = 0;
	this->Old.xx = this->New.xx = 0;
	this->Old.y = this->New.y = 0;
	this->Old.yy = this->New.yy = 0;


	this->Show = Stick_Show;
	this->Hide = Stick_Hide;
	this->MoveTo = Stick_MoveTo;
	this->GetColor = Stick_GetColor;
	this->DoneWithTheStick = Stick_Status;
	this->Lenght = Stick_Lenght;


	return (this);


};
