


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




#include   <math.h>
#include    <X11/Xlib.h>
#include   <stdio.h>
#include    "definition.h"

int             WhoseTurn = MYTURN;


extern Display *disp;
extern GC       gc;
extern GC       XorGc;


extern Window   main_window, Pane, exitbutton, newgame, Yourscoreboard, Myscoreboard, WhoseTurnWindow;
extern Window   MyPlus, MyMinus, YourPlus, YourMinus;
extern Window   TraJectory;


extern int      WhiteBallOut, BlackBallOut;
extern int      YourScore, MyScore;
extern int      StartX, StartY, NewX, NewY;
extern void
PutIn(Window win, char *string, GC gc, int fg, int bg);
extern void
RubberLineOnPane(int type, int x, int y, int xx, int yy, int color);
extern void
line(int x, int y, int xx, int yy);
extern
get_color(char *name);
extern
setbkcolor(int color);
extern Distance *
DistanceChecker_Between(Ball * ThisBall, Ball * ThatBall);

extern void
show_button_pressed(Window the_win, GC gc, char *the_string);




extern Stick   *PoolStick;
extern Stick   *Trajectory;

int             WantsTrajectory = NO;


int             OldSx = 0, OldSy = 0, OldEx = 0, OldEy = 0;

extern unsigned long fg, bg, bd;

extern XFontStruct *fontstruct;	/* Font descriptor */


extern Coordinator *Dept_Head;




extern void
LayTheFloor(void);

int             ButtonPressedBefore = NO;
int             main_x, main_y, pane_x, pane_y;
unsigned int    buttons;
char            Message[20] = {
			       "Ready"
};				/* just for a short message */







int
MyBallsOut()
{
	int             i;
	int             InList = 0;
	for (i = 2; i < 9; i++)
		if (Dept_Head->Objects[i]->In)
			InList++;
	return ((InList) ? NO : YES);

}

int
YourBallsOut()
{
	int             i;
	int             InList = 0;
	for (i = 9; i < 16; i++)
		if (Dept_Head->Objects[i]->In)
			InList++;
	return ((InList) ? NO : YES);
}


void
ShowWhatHappenned(char *message)
{
	PutIn(WhoseTurnWindow, message, gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
};

void
handle_scratch(XEvent event);	/* defined down here somewhere */
/* need to use in handle_stick() */

handle_stick(XEvent event)
{

	float          hip;
	float          lenght;
	float          Speed;
	int             i;
	int             Trax, Tray, TraEndx, TraEndy;

	Trax = StartX = (int) Dept_Head->Objects[0]->CurrentL.x;
	Tray = StartY = (int) Dept_Head->Objects[0]->CurrentL.y;

	NewX = pane_x;
	NewY = pane_y;
	TraEndx = Trax + (Trax - NewX);
	TraEndy = Tray + (Tray - NewY);

	/* we don't want to draw on the ball */
	hip = sqrt((float) ((NewX - StartX) * (NewX - StartX) + (NewY -
								 StartY) *
			    (NewY - StartY)));

	StartX += (int) ((float) (NewX - StartX) / hip
			 * (Dept_Head->Objects[0]->radius + 10));

	StartY += (int) ((float) (NewY - StartY) / hip
			 * (Dept_Head->Objects[0]->radius + 10));



	/* 256 button1 */
	if (buttons == 256) {

		PoolStick->MoveTo(PoolStick, StartX, StartY, NewX, NewY);
		PoolStick->DoneWithTheStick(PoolStick, NO);
		if (WantsTrajectory == YES) {
			Trajectory->MoveTo(Trajectory, Trax, Tray, TraEndx, TraEndy);
			Trajectory->DoneWithTheStick(Trajectory, NO);
		}
		ButtonPressedBefore = YES;

	} else if (buttons == 0 && ButtonPressedBefore) {

		/*
		 * here we are done with the stick, hit the ball now 
		 */
		PoolStick->DoneWithTheStick(PoolStick, YES);
		PoolStick->Show(PoolStick);
		if (WantsTrajectory == YES) {
			Trajectory->DoneWithTheStick(Trajectory, YES);
			Trajectory->Show(Trajectory);
		}
		ButtonPressedBefore = NO;

		lenght = PoolStick->Lenght(PoolStick);

		Speed = lenght * (float) SPEEDLIMIT / (float) MAXSTICKLENGHT;
		Dept_Head->Objects[0]->CurrentV.x = -(float) ((NewX
							    - StartX) / hip)
			* Speed;
		Dept_Head->Objects[0]->CurrentV.y = -(float) ((NewY
							    - StartY) / hip)
			* Speed;
		Dept_Head->Objects[0]->Stopped = NO;
		sprintf(Message, "%s", "Running");
		ShowWhatHappenned(Message);
		setbkcolor(get_color(PANE_BG));	/* one more quick fix */
		Dept_Head->StartAction(Dept_Head);
		sprintf(Message, "%s", "Ready");
		ShowWhatHappenned(Message);


		/* let's see what the hell happenned  */
		if (WhiteBallOut) {
			sprintf(Message, "%s", "Scrach");
			ShowWhatHappenned(Message);
			handle_scratch(event);
			sprintf(Message, "%s", "Ready");
			ShowWhatHappenned(Message);
		}
		if (BlackBallOut) {
			Dept_Head->InitConfig(Dept_Head);
			setbkcolor(get_color(PANE_BG));
		}
	}
};


handle_white_ball(XEvent event)
{
	if (!ButtonPressedBefore) {
		if ((pane_x > WHITE_BALL_LIMIT) && (buttons == 256)) {
			Dept_Head->Objects[0]->Hide(Dept_Head->Objects[0]);

			Dept_Head->Objects[0]->CurrentL.x = pane_x;
			Dept_Head->Objects[0]->CurrentL.y = pane_y;

			Dept_Head->Objects[0]->Show(Dept_Head->Objects[0]);
		}
		/*
		 * we need this sucker to avoid fast movement of the mouse /*
		 * PoolStick -> Old.x = PoolStick -> New.x = 1000;/* some far
		 * place 
		 */
		PoolStick->Old.xx = PoolStick->New.xx = 1000;
		PoolStick->Old.y = PoolStick->New.y = 1000;
		PoolStick->Old.yy = PoolStick->New.yy = 1000;
	}
};


void
handle_scratch(XEvent event)
{

	int             j, Done = NO, i, TryAgain = YES;
	Distance       *theDistance;
	while (TryAgain) {
		TryAgain = NO;
		/*
		 * if random()  doesn't work you, fix something  for your
		 * machine 
		 */
		/*
		 * you only need to pick two random numbers for white ball
		 * position 
		 */

		srandom(random());	/* it looks like working on my
					 * machine ( DEC 3100 ) */

		do {		/* this thing sets the x position somewhere
				 * between 500 & 650 */
			Dept_Head->Objects[0]->CurrentL.x = (float) (random() & 0x02FF);
			if (Dept_Head->Objects[0]->CurrentL.x > 550 &&
			    Dept_Head->Objects[0]->CurrentL.x < 650)
				Done = YES;
		} while (!Done);

		Done = NO;	/* for the next pick */

		do {		/* this thing sets the y position somewhere
				 * between 20 & 400 */
			Dept_Head->Objects[0]->CurrentL.y = (float) (random() & 0x01FF);
			if (Dept_Head->Objects[0]->CurrentL.y > 20 &&
			    Dept_Head->Objects[0]->CurrentL.y < 400)
				Done = YES;
		} while (!Done);

		dprintf(" %f  %f   \n", Dept_Head->Objects[0]->CurrentL.x,
			Dept_Head->Objects[0]->CurrentL.y);

		/* let's see if this sucker overlaps */
		for (i = 1; i < Dept_Head->NumberOfObjects; i++) {
			theDistance = DistanceChecker_Between(Dept_Head->Objects[0], Dept_Head->Objects[i]);
			if (theDistance->amount > 0)
				TryAgain = YES;	/* this sucker overlaps, try
						 * again */
		}
	}

	/* time to refresh ourself and start movin' */
	Dept_Head->Objects[0]->In = YES;
	WhiteBallOut = NO;
	Dept_Head->Objects[0]->Stopped = NO;
	Dept_Head->Objects[0]->ShowOut = NO;
	Dept_Head->Objects[0]->Visible = NO;
	Dept_Head->Objects[0]->NeedOneMoreDrawing = YES;
	setbkcolor(get_color(PANE_BG));	/* another quick fix */
	Dept_Head->Objects[0]->Show(Dept_Head->Objects[0]);

};


take_event()
{
	XEvent          event;
	Window          win;
	Window          parent, current;
	XButtonPressedEvent *button_event;
	XExposeEvent   *expose_event;
	float          hip;
	float          lenght;
	float          Speed;
	char            buf[15];
	int             i;
	float           howFar;
	XNextEvent(disp, &event);
	switch (event.type) {

	case ButtonPress:
		{
			button_event = (XButtonPressedEvent *) & event;
			/* handle event here */
			/* check windows and determine which option wanted */
			press_handle(button_event);

		}
		break;


	case MotionNotify:
		{
			/* let's find where the mouse is.. */
			if (!XQueryPointer(disp, event.xmotion.window,
			       &parent, &current, &main_x, &main_y, &pane_x,
					   &pane_y, &buttons))
				break;
			/* let's see what the hell player wants to do.. */
			if (sqrt((double) ((pane_x - (int) Dept_Head->Objects[0]->CurrentL.x) *
			  (pane_x - (int) Dept_Head->Objects[0]->CurrentL.x)
					   +
			(pane_y - (int) Dept_Head->Objects[0]->CurrentL.y) *
			 (pane_y - (int) Dept_Head->Objects[0]->CurrentL.y))
				 ) < Dept_Head->Objects[0]->radius)
				handle_white_ball(event);	/* wants to move the
								 * white ball */
			else
				handle_stick(event);	/* wants to move stick */
			/*
			 * note that if you move your mouse fast, blt will
			 * not be able to catch up with you, so you are gonna
			 * end up moving the stick not the white ball. 
			 */
		}
		break;


	case ButtonRelease:
		/*
		 * now you are done with the stick one more MotionNotify
		 * needed to inform handle_stick()  
		 */
		if (ButtonPressedBefore) {
			event.type = MotionNotify;
			XSendEvent(disp, Pane, True, 0, &event);
		}
		break;

	case Expose:
		{
			expose_event = (XExposeEvent *) & event;
			expose_window(expose_event);
		}
		break;


	default:
		dprintf("unknown option..!\n");
		break;
	}			/* end of swithc */
}




press_handle(press_event)
	XButtonPressedEvent *press_event;
{
	Window          win;
	int             i;
	char            buf[15];

	win = press_event->window;
	if (win == exitbutton)
		exit(0);
	if (win == newgame) {
          show_button_pressed(newgame,gc,"New Game");
		YourScore = 0;
		MyScore = 0;
		Dept_Head->InitConfig(Dept_Head);
		for (i = 0; i < Dept_Head->NumberOfObjects; i++)
			Dept_Head->Objects[i]->Visible = NO;

	}
	/* for now you are supposed to enter the score by 'hand'. */

	if (win == MyPlus) {
                show_button_pressed(MyPlus,gc,"+" );
		MyScore += 1;
		sprintf(buf, "%d", MyScore);
		PutIn(Myscoreboard, buf, gc,get_color(BUTTON_FG) , get_color(BUTTON_COLOR));
	}
	if (win == MyMinus) {
                show_button_pressed(MyMinus,gc,"-");
		MyScore -= 1;
		sprintf(buf, "%d", MyScore);
		PutIn(Myscoreboard, buf, gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	}
	if (win == YourPlus) {
                show_button_pressed(YourPlus,gc,"+");
		YourScore += 1;
		sprintf(buf, "%d", YourScore);
		PutIn(Yourscoreboard, buf, gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	}
	if (win == YourMinus) {
                show_button_pressed(YourMinus,gc,"-");
		YourScore -= 1;
		sprintf(buf, "%d", YourScore);
		PutIn(Yourscoreboard, buf, gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	}
	if (win == TraJectory) {
		if (WantsTrajectory == YES) {
			WantsTrajectory = NO;
			PutIn(TraJectory, "No Cheat", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
                        show_button_pressed(TraJectory,gc,"No Cheat");
		} else {
			WantsTrajectory = YES;
			PutIn(TraJectory, "Cheat", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
                        show_button_pressed(TraJectory,gc,"Cheat");
		}
	}
	setbkcolor(get_color(PANE_BG));	/* one more quick fix */



};





expose_window(expose_event)
	XExposeEvent   *expose_event;
{
	char            buf[15];
	Window          win;
	int             i;
	win = expose_event->window;

	if (win == exitbutton)
		PutIn(exitbutton, "  Exit   ", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	if (win == newgame)
		PutIn(newgame, " New Game  ", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	if (win == Yourscoreboard) {
		sprintf(buf, " %d ", YourScore);
		PutIn(Yourscoreboard, buf, gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	}
	if (win == Myscoreboard) {
		sprintf(buf, " %d ", MyScore);
		PutIn(Myscoreboard, buf, gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	}
	if (win == main_window)
		Dept_Head->DrawPockets(Dept_Head);

	setbkcolor(get_color(PANE_BG));	/* quick fix */
	if (win == Pane) {
		for (i = 0; i < Dept_Head->NumberOfObjects; i++)
			Dept_Head->Objects[i]->ShowOut = NO;
		Dept_Head->ShowStaticPosition(Dept_Head);
	}
	if (win == WhoseTurnWindow)
		ShowWhatHappenned(Message);

	if (win == MyPlus)
		PutIn(MyPlus, "+", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	if (win == MyMinus)
		PutIn(MyMinus, "-", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));

	if (win == YourPlus)
		PutIn(YourPlus, "+", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	if (win == YourMinus)
		PutIn(YourMinus, "-", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));

	if (win == TraJectory) {
		if (WantsTrajectory == YES)
			PutIn(TraJectory, "Cheat", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
		else
			PutIn(TraJectory, "No Cheat", gc, get_color(BUTTON_FG), get_color(BUTTON_COLOR));
	}
}
