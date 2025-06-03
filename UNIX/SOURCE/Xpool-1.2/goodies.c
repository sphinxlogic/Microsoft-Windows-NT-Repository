
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

#include "definition.h"


Distance        ThisMuch;


Distance       *
DistanceChecker_Between(Ball * ThisBall, Ball * ThatBall)
{

	if ((ThisBall->In) && (ThatBall->In)) {
		ThisMuch.x = ThisBall->CurrentL.x - ThatBall->CurrentL.x;
		ThisMuch.y = ThisBall->CurrentL.y - ThatBall->CurrentL.y;
		ThisMuch.amount = (ThisBall->radius + ThatBall->radius) -
			sqrt((ThisMuch.x * ThisMuch.x) + (ThisMuch.y * ThisMuch.y));
	} else {
		ThisMuch.x = 0.0000000;
		ThisMuch.y = 0.0000000;
		ThisMuch.amount = 0.000000;
	}
	/*
	 * ThisBall -> NeedOneMoreDrawing = ((ThisMuch.amount +6 )>0)? YES :
	 * NO; ThatBall -> NeedOneMoreDrawing = ((ThisMuch.amount +6 )>0)?
	 * YES : NO; 
	 */

	return (&ThisMuch);
};


Force           ForceToApply;

void
Force_ToObject(Ball * first, Distance * TheirDistance, int type)
{
	float          theForce, hip;
	float          radian;
        float temp;


	/* let 's create force normal to the contact surface */
	theForce = first->kConstant * TheirDistance->amount;
	/*
	 * dprintf (" total %f  k %f d %f \n", theForce, first -> kConstant,
	 * TheirDistance -> amount); dprintf (" d->x %f d->y %f \n",
	 * TheirDistance -> x, TheirDistance -> x); 
	 */
	/* let 's calculate x,y componets */
	hip = sqrt((TheirDistance->x * TheirDistance->x) +
		   (TheirDistance->y * TheirDistance->y));
        temp = theForce/hip;
	ForceToApply.x = temp * TheirDistance->x ;
	ForceToApply.y = temp * TheirDistance->y ;


	if (type == ACTION) {
		ForceToApply.x *= -1;
		/* reaction to the fists ball */
		ForceToApply.y *= -1;
	}
	/*
	 * dprintf (" # %d  x %f y %f  fx %f  fy %f  \n", first -> MyID,
	 * first -> CurrentL.x, first -> CurrentL.y, first -> CurrentF.x,
	 * first -> CurrentF.y); 
	 *
	 * dprintf ("  fx %f  fy %f  \n", ForceToApply.x, ForceToApply.y); 
	 */
	first->UpdateForce(first, &ForceToApply);



};
