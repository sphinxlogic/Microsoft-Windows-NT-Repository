/* Copyright (C) 1992 Nathan Sidwell */
#include "xmris.h"
/*{{{  void bounce_ball()*/
extern VOIDFUNC bounce_ball FUNCARGVOID
/*
 * throws, bounces, explodes & implodes the ball
 * bouncing the ball is rather involved
 */
{
  COORD     tl;
  COORD     br;
  CELL      *cptr;
  CELL      *optr;
  
  cptr = optr = NULL;
  switch(player.ball.state)
  {
    /*{{{  case 0: (held)*/
    case 0:
      if(global.throw == 1 && global.state != 4)
	{
	  unsigned  face;
	  CELL      *nptr;
	
	  global.throw = 2;
	  player.ball.state = 1;
	  face = monster.list[0].face;
	  player.ball.cell.x = monster.list[0].cell.x;
	  player.ball.cell.y = monster.list[0].cell.y;
	  player.ball.offset.x =
	    monster.list[0].offset.x + ball_throw[face].x;
	  player.ball.offset.y =
	    monster.list[0].offset.y + ball_throw[face].y;
	  player.ball.count = ball_dir[face];
	  nptr = cptr = BOARDCELL(player.ball.cell.x, player.ball.cell.y);
	  switch(monster.list[0].dir)
	  {
	    /*{{{  case 0: (up)*/
	    case 0:
	      optr = cptr;
	      nptr = optr - CELL_STRIDE;
	      if(player.ball.offset.y > -(CELL_HEIGHT / 2))
		/* EMPTY */;
	      else if(cptr[-CELL_STRIDE].visit)
		{
		  player.ball.offset.y += CELL_HEIGHT + GAP_HEIGHT;
		  player.ball.cell.y--;
		}
	      else if(cptr[-CELL_STRIDE * 2].depths[1] -
		  player.ball.offset.y >
		  CELL_HEIGHT / 2 + CELL_HEIGHT + GAP_HEIGHT)
		{
		  player.ball.offset.y += (CELL_HEIGHT + GAP_HEIGHT) * 2;
		  player.ball.cell.y -= 2;
		}
	      else if(player.ball.offset.y + CELL_HEIGHT / 2 <
		  cptr[0].depths[0])
		player.ball.state = 0;
	      break;
	    /*}}}*/
	    /*{{{  case 1: (down)*/
	    case 1:
	      optr = cptr;
	      nptr = optr + CELL_STRIDE;
	      if(player.ball.offset.y < (CELL_HEIGHT / 2))
		/* EMPTY */;
	      else if(cptr[CELL_STRIDE].visit)
		{
		  player.ball.offset.y -= CELL_HEIGHT + GAP_HEIGHT;
		  player.ball.cell.y++;
		}
	      else if(player.ball.offset.y - cptr[CELL_STRIDE * 2].depths[0] >
		  CELL_HEIGHT / 2 + CELL_HEIGHT + GAP_HEIGHT)
		{
		  player.ball.offset.y -= (CELL_HEIGHT + GAP_HEIGHT) * 2;
		  player.ball.cell.y += 2;
		}
	      else if(player.ball.offset.y - CELL_HEIGHT / 2 >
		  cptr[0].depths[1])
		player.ball.state = 0;
	      break;
	    /*}}}*/
	    /*{{{  case 2: (left)*/
	    case 2:
	      optr = cptr;
	      nptr = optr - 1;
	      if(player.ball.offset.x > -(CELL_WIDTH / 2))
		/* EMPTY */;
	      else if(cptr[-1].visit)
		{
		  player.ball.offset.x += CELL_WIDTH + GAP_WIDTH;
		  player.ball.cell.x--;
		}
	      else if(cptr[-2].depths[3] - player.ball.offset.x >
		  CELL_WIDTH / 2 + CELL_WIDTH + GAP_WIDTH)
		{
		  player.ball.offset.x += (CELL_WIDTH + GAP_WIDTH) * 2;
		  player.ball.cell.x -= 2;
		}
	      else if(player.ball.offset.x + CELL_WIDTH / 2 <
		  cptr[0].depths[2])
		player.ball.state = 0;
	      break;
	    /*}}}*/
	    /*{{{  case 3: (right)*/
	    case 3:
	      optr = cptr;
	      nptr = optr + 1;
	      if(player.ball.offset.x < (CELL_HEIGHT / 2))
		/* EMPTY */;
	      else if(cptr[1].visit)
		{
		  player.ball.offset.x -= CELL_WIDTH + GAP_WIDTH;
		  player.ball.cell.x++;
		}
	      else if(player.ball.offset.x - cptr[2].depths[2] >
		  CELL_WIDTH / 2 + CELL_WIDTH + GAP_WIDTH)
		{
		  player.ball.offset.x -= (CELL_WIDTH + GAP_WIDTH) * 2;
		  player.ball.cell.x += 2;
		}
	      else if(player.ball.offset.x - CELL_HEIGHT / 2 >
		  cptr[0].depths[3])
		player.ball.state = 0;
	      break;
	    /*}}}*/
	  }
	  if(player.ball.state)
	    {
	      player.ball.pixel.x = PIXELX(player.ball.cell.x,
		  player.ball.offset.x + CELL_WIDTH / 2);
	      player.ball.pixel.y = PIXELY(player.ball.cell.y,
		  player.ball.offset.y + CELL_HEIGHT / 2);
	      br.x = player.ball.pixel.x;
	      br.y = player.ball.pixel.y;
	      tl.x = player.ball.pixel.x;
	      tl.y = player.ball.pixel.y;
	      player.ball.image = 2;
	      if(!optr->visit || !nptr->visit)
		optr = cptr = NULL;
	      else
		cptr = nptr;
	    }
	  else
	    optr = cptr = NULL;
	}
      break;
    /*}}}*/
    /*{{{  case 1: (bounce)*/
    case 1:
    {
      int       i;
	
      br.x = tl.x = player.ball.pixel.x;
      br.y = tl.y = player.ball.pixel.y;
      optr = cptr = BOARDCELL(player.ball.cell.x, player.ball.cell.y);
      for(i = BALL_STEPS; i--;)
	/*{{{  move one step*/
	{
	  int       walls;
	  /* bit vector tl, br, bl, tr, t, b, l, r */
	
	  /*{{{  moved to above or below cell?*/
	  if(player.ball.offset.y > CELL_HEIGHT / 2 &&
	      (cptr[CELL_STRIDE].depths[0] ||
	      (cptr->holes[3] &&
	      player.ball.offset.x + CELL_WIDTH / 2 >= cptr->holes[3]) ||
	      (cptr->holes[2] &&
	      player.ball.offset.x - CELL_WIDTH / 2 <= cptr->holes[2])))
	    {
	      player.ball.offset.y -= CELL_HEIGHT + GAP_HEIGHT;
	      player.ball.cell.y++;
	      cptr += CELL_STRIDE;
	    }
	  else if(player.ball.offset.y < -CELL_HEIGHT / 2 &&
	      (cptr[-CELL_STRIDE].depths[1] ||
	      (cptr->holes[1] &&
	      player.ball.offset.x + CELL_WIDTH / 2 >= cptr->holes[1]) ||
	      (cptr->holes[0] &&
	      player.ball.offset.x - CELL_WIDTH / 2 <= cptr->holes[0])))
	    {
	      player.ball.offset.y += CELL_HEIGHT + GAP_HEIGHT;
	      player.ball.cell.y--;
	      cptr -= CELL_STRIDE;
	    }
	  /*}}}*/
	  /*{{{  moved to left or right cell?*/
	  if(player.ball.offset.x > CELL_WIDTH / 2 && cptr[1].depths[2])
	    {
	      player.ball.offset.x -= CELL_WIDTH + GAP_WIDTH;
	      player.ball.cell.x++;
	      cptr++;
	    }
	  else if(player.ball.offset.x < -CELL_WIDTH / 2 &&
	      cptr[-1].depths[3])
	    {
	      player.ball.offset.x += CELL_WIDTH + GAP_WIDTH;
	      player.ball.cell.x--;
	      cptr--;
	    }
	  /*}}}*/
	  walls = 0x00;
	  /*{{{  set the maze walls*/
	  {
	    unsigned  quadrant;
	    /* quadrants to, bo, lo, ro, ti, bi, li, ri, t, b, l, r */
	  
	    /* work out where we are in the cell */
	    quadrant = 0x000;
	    /*{{{  y place*/
	    if(player.ball.offset.y < -CELL_HEIGHT / 2 - VEL_Y)
	      quadrant |= 0x008;
	    else if(player.ball.offset.y < -CELL_HEIGHT / 2)
	      quadrant |= 0x800;
	    else if(player.ball.offset.y < -CELL_HEIGHT / 2 + VEL_Y)
	      quadrant |= 0x080;
	    else if(player.ball.offset.y >= CELL_HEIGHT / 2 + VEL_Y)
	      quadrant |= 0x004;
	    else if(player.ball.offset.y >= CELL_HEIGHT / 2)
	      quadrant |= 0x400;
	    else if(player.ball.offset.y >= CELL_HEIGHT / 2 - VEL_Y)
	      quadrant |= 0x040;
	    /*}}}*/
	    /*{{{  x place*/
	    if(player.ball.offset.x < -CELL_WIDTH / 2 - VEL_X)
	      quadrant |= 0x002;
	    else if(player.ball.offset.x < -CELL_WIDTH / 2)
	      quadrant |= 0x200;
	    else if(player.ball.offset.x < -CELL_WIDTH / 2 + VEL_X)
	      quadrant |= 0x020;
	    else if(player.ball.offset.x >= CELL_WIDTH / 2 + VEL_X)
	      quadrant |= 0x001;
	    else if(player.ball.offset.x >= CELL_WIDTH / 2)
	      quadrant |= 0x100;
	    else if(player.ball.offset.x >= CELL_WIDTH / 2 - VEL_X)
	      quadrant |= 0x010;
	    /*}}}*/
	    if(quadrant & 0x888)
	      /*{{{  above cell*/
	      {
		if(player.ball.offset.y <=
		    cptr[0].depths[0] - CELL_HEIGHT / 2 + VEL_Y)
		  walls |= 0x98;
		if(quadrant & 0x808)
		  {
		    unsigned  mask;
	      
		    if(quadrant & 0x800)
		      mask = 0x93;
		    else
		      mask = 0xF3;
		    if(quadrant & 0x020)
		      walls |= mask & 0xA2;
		    if(quadrant & 0x010)
		      walls |= mask & 0x51;
		  }
		else
		  {
		    if(quadrant & 0x020)
		      {
			if(cptr[0].depths[0] >= -VEL_Y ||
			    cptr[0].depths[2] >= -VEL_X ||
			    cptr[-1].depths[0] >= -VEL_Y ||
			    cptr[-CELL_STRIDE].depths[2] >= -VEL_X)
			  walls |= 0x80;
		      }
		    else if(quadrant & 0x010)
		      {
			if(cptr[0].depths[0] >= -VEL_Y ||
			    cptr[0].depths[3] <= VEL_X ||
			    cptr[1].depths[0] >= -VEL_Y ||
			    cptr[-CELL_STRIDE].depths[3] <= VEL_X)
			  walls |= 0x10;
		      }
		  }
	      }
	      /*}}}*/
	    else if(quadrant & 0x444)
	      /*{{{  below cell*/
	      {
		if(player.ball.offset.y >=
		    cptr[0].depths[1] + CELL_HEIGHT / 2 - VEL_Y)
		  walls |= 0x64;
		if(quadrant & 0x404)
		  {
		    unsigned  mask;
	      
		    if(quadrant & 0x400)
		      mask = 0x63;
		    else
		      mask = 0xF3;
		    if(quadrant & 0x020)
		      walls |= mask & 0xA2;
		    if(quadrant & 0x010)
		      walls |= mask & 0x51;
		  }
		else
		  {
		    if(quadrant & 0x020)
		      {
			if(cptr[0].depths[1] <= VEL_Y ||
			    cptr[0].depths[2] >= -VEL_X ||
			    cptr[-1].depths[1] <= VEL_Y ||
			    cptr[CELL_STRIDE].depths[2] >= -VEL_X)
			  walls |= 0x20;
		      }
		    else if(quadrant & 0x010)
		      {
			if(cptr[0].depths[1] <= VEL_Y ||
			    cptr[0].depths[3] <= VEL_X ||
			    cptr[1].depths[1] <= VEL_Y ||
			    cptr[CELL_STRIDE].depths[3] <= VEL_X)
			  walls |= 0x40;
		      }
		  }
	      }
	      /*}}}*/
	    if(quadrant & 0x222)
	      /*{{{  left cell*/
	      {
		if(player.ball.offset.x <=
		    cptr[0].depths[2] - CELL_WIDTH / 2 + VEL_X)
		  walls |= 0xA2;
		if(quadrant & 0x202)
		  {
		    unsigned  mask;
	      
		    if(quadrant & 0x200)
		      mask = 0xAC;
		    else
		      mask = 0xFC;
		    if(quadrant & 0x080)
		      walls |= mask & 0x98;
		    if(quadrant & 0x040)
		      walls |= mask & 0x64;
		  }
	      }
	      /*}}}*/
	    else if(quadrant & 0x111)
	      /*{{{  right cell*/
	      {
		if(player.ball.offset.x >=
		    cptr[0].depths[3] + CELL_WIDTH / 2 - VEL_X)
		  walls |= 0x51;
		if(quadrant & 0x101)
		  {
		    unsigned  mask;
	      
		    if(quadrant & 0x100)
		      mask = 0x5C;
		    else
		      mask = 0xFC;
		    if(quadrant & 0x080)
		      walls |= mask & 0x98;
		    if(quadrant & 0x040)
		      walls |= mask & 0x64;
		  }
	      }
	      /*}}}*/
	    /*{{{  holes?*/
	    {
	      if(quadrant & 0x888)
		/*{{{  top*/
		{
		  if(cptr->holes[0] &&
		      player.ball.offset.x - CELL_WIDTH / 2 < cptr->holes[0])
		    walls &= ~0x99;
		  if(cptr->holes[1] &&
		      player.ball.offset.x + CELL_WIDTH / 2 > cptr->holes[1])
		    walls &= ~0x9A;
		}
		/*}}}*/
	      else if(quadrant & 0x444)
		/*{{{  bottom*/
		{
		  if(cptr->holes[2] &&
		      player.ball.offset.x - CELL_WIDTH / 2 < cptr->holes[2])
		    walls &= ~0x65;
		  if(cptr->holes[3] &&
		      player.ball.offset.x + CELL_WIDTH / 2 > cptr->holes[3])
		    walls &= ~0x66;
		}
		/*}}}*/
	    }
	    /*}}}*/
	  }
	  /*}}}*/
	  /*{{{  set apple walls*/
	  {
	    unsigned  i;
	    APPLE     *aptr;
	  
	    for(aptr = apple.list, i = apple.apples; i--; aptr++)
	      {
		int     x, y;
		int     edge;
		/* edge ot, ob, ol, or, t, b, l, r */
	  
		if(aptr->state < 4)
		  {
		    x = aptr->pixel.x;
		    y = aptr->pixel.y;
		    edge = 0;
		    /*{{{  Y edge*/
		    if(player.ball.pixel.y - y < -VEL_Y ||
			player.ball.pixel.y - y >= CELL_HEIGHT + VEL_Y)
		      /* EMPTY */;
		    else if(player.ball.pixel.y - y < 0)
		      edge |= 0X80;
		    else if(player.ball.pixel.y - y < CELL_HEIGHT / 2)
		      edge |= 0X08;
		    else if(player.ball.pixel.y - y < CELL_HEIGHT)
		      edge |= 0x04;
		    else
		      edge |= 0x40;
		    /*}}}*/
		    /*{{{  X edge*/
		    if(player.ball.pixel.x - x < -VEL_X ||
			player.ball.pixel.x - x >= CELL_WIDTH + VEL_X)
		      /* EMPTY */;
		    else if(player.ball.pixel.x - x < 0)
		      edge |= 0X20;
		    else if(player.ball.pixel.x - x < CELL_WIDTH / 2)
		      edge |= 0X02;
		    else if(player.ball.pixel.x - x < CELL_WIDTH)
		      edge |= 0x01;
		    else
		      edge |= 0x10;
		    /*}}}*/
		    if(edge)
		      {
			/*{{{  top*/
			if(edge & 0X88 && edge & 0X33)
			  {
			    if(edge & 0X03)
			      walls |= 0X04;
			    if(edge & 0X23)
			      walls |= 0X40;
			    if(walls & 0X13)
			      walls |= 0X20;
			  }
			/*}}}*/
			/*{{{  bottom*/
			if(edge & 0X44 && edge & 0X33)
			  {
			    if(edge & 0X03)
			      walls |= 0X08;
			    if(edge & 0X23)
			      walls |= 0X10;
			    if(walls & 0X13)
			      walls |= 0X80;
			  }
			/*}}}*/
			/*{{{  left*/
			if(edge & 0X22 && edge & 0X0C)
			  {
			    walls |= 0X01;
			    if(edge & 0X8C)
			      walls |= 0X40;
			    if(walls & 0X4C)
			      walls |= 0X10;
			  }
			/*}}}*/
			/*{{{  right*/
			if(edge & 0X11 && edge & 0X0C)
			  {
			    walls |= 0X02;
			    if(edge & 0X8C)
			      walls |= 0X20;
			    if(walls & 0X4C)
			      walls |= 0X80;
			  }
			/*}}}*/
		      }
		  }
	      }
	  }
	  /*}}}*/
	  if((walls & 0xC) == 0xC || (walls & 0x03) == 0x03)
	    /*{{{  trapped*/
	    {
	      player.count = 0;
	      player.ball.state = 2;
	      player.ball.count = 0;
	      break;
	    }
	    /*}}}*/
	  else
	    {
	      /*{{{  bounce?*/
	      switch(player.ball.count)
	      {
		/*{{{  case 0: (up left)*/
		case 0:
		  if(!(walls & 0X80))
		    /* EMPTY */;
		  else if((walls & 0X0A) == 0X08)
		    player.ball.count = 2;
		  else if((walls & 0X0A) == 0X02)
		    player.ball.count = 3;
		  else if((walls & 0X0A) == 0X0A)
		    player.ball.count = 1;
		  else
		    player.ball.count = chaotic() & 1 ? 2 : 3;
		  break;
		/*}}}*/
		/*{{{  case 1: (down right)*/
		case 1:
		  if(!(walls & 0X40))
		    /* EMPTY */;
		  else if((walls & 0X05) == 0X04)
		    player.ball.count = 3;
		  else if((walls & 0X05) == 0X01)
		    player.ball.count = 2;
		  else if((walls & 0X05) == 0X05)
		    player.ball.count = 0;
		  else
		    player.ball.count = chaotic() & 1 ? 3 : 2;
		  break;
		/*}}}*/
		/*{{{  case 2: (down left)*/
		case 2:
		  if(!(walls & 0X20))
		    /* EMPTY */;
		  else if((walls & 0X06) == 0X04)
		    player.ball.count = 0;
		  else if((walls & 0X06) == 0X02)
		    player.ball.count = 1;
		  else if((walls & 0X06) == 0X06)
		    player.ball.count = 3;
		  else
		    player.ball.count = chaotic() & 1 ? 0 : 1;
		  break;
		/*}}}*/
		/*{{{  case 3: (up right)*/
		case 3:
		  if(!(walls & 0X10))
		    /* EMPTY */;
		  else if((walls & 0X09) == 0X08)
		    player.ball.count = 1;
		  else if((walls & 0X09) == 0X01)
		    player.ball.count = 0;
		  else if((walls & 0x09) == 0x09)
		    player.ball.count = 2;
		  else
		    player.ball.count = chaotic() & 1 ? 1 : 0;
		  break;
		/*}}}*/
	      }
	      /*}}}*/
	      /*{{{  move ball*/
	      switch(player.ball.count)
	      {
		/*{{{  case 0: (up left)*/
		case 0:
		  player.ball.offset.x -= VEL_X;
		  player.ball.offset.y -= VEL_Y;
		  player.ball.pixel.x -= VEL_X;
		  player.ball.pixel.y -= VEL_Y;
		  break;
		/*}}}*/
		/*{{{  case 1: (down right)*/
		case 1:
		  player.ball.offset.x += VEL_X;
		  player.ball.offset.y += VEL_Y;
		  player.ball.pixel.x += VEL_X;
		  player.ball.pixel.y += VEL_Y;
		  break;
		/*}}}*/
		/*{{{  case 2: (down left)*/
		case 2:
		  player.ball.offset.x -= VEL_X;
		  player.ball.offset.y += VEL_Y;
		  player.ball.pixel.x -= VEL_X;
		  player.ball.pixel.y += VEL_Y;
		  break;
		/*}}}*/
		/*{{{  case 3: (up right)*/
		case 3:
		  player.ball.offset.x += VEL_X;
		  player.ball.offset.y -= VEL_Y;
		  player.ball.pixel.x += VEL_X;
		  player.ball.pixel.y -= VEL_Y;
		  break;
		/*}}}*/
	      }
	      /*}}}*/
	    }
	  if(player.ball.pixel.x < tl.x)
	    tl.x = player.ball.pixel.x;
	  else
	    br.x = player.ball.pixel.x;
	  if(player.ball.pixel.y < tl.y)
	    tl.y = player.ball.pixel.y;
	  else
	    br.y = player.ball.pixel.y;
	}
	/*}}}*/
      break;
    }
    /*}}}*/
    /*{{{  case 2: (explode)*/
    case 2:
      player.ball.count++;
      if(player.ball.count == BALL_EXPLODE)
	player.ball.state = 3;
      break;
    /*}}}*/
    /*{{{  case 3: (exploded)*/
    case 3:
      {
	if(!player.count--)
	  {
	    player.ball.state = 4;
	    player.ball.count--;
	  }
      }
      break;
    /*}}}*/
    /*{{{  case 4: (implode)*/
    case 4:
    {
      player.ball.count--;
      player.ball.pixel.x = monster.list[0].pixel.x + CELL_WIDTH / 2;
      player.ball.pixel.y = monster.list[0].pixel.y + CELL_HEIGHT / 2;
      if(!player.ball.count)
	player.ball.state = 0;
      break;
    }
    /*}}}*/
  }
  if(optr && cptr && optr != cptr)
    /*{{{  calc ball distances*/
    {
      CELL    **aptr, **sptr;
      CELL    *list[2][FLOOD_FILL];
      int     toggle;
      int     x, y;
      int     count;
      CELL    *pptr;
    
      for(y = CELLS_DOWN; y--;)
	{
	  CELL      *cptr;
	  
	  cptr = BOARDCELL(0, y);
	  for(x = CELLS_ACROSS; x--; cptr++)
	    cptr->ball = 0;
	}
      toggle = 0;
      pptr = BOARDCELL(monster.list[0].cell.x, monster.list[0].cell.y);
      pptr->ball = optr->ball = cptr->ball = count = BALL_DISTANCE;
      list[0][0] = cptr;
      list[0][1] = NULL;
      while(list[toggle][0] && --count)
      {
	sptr = list[toggle];
	toggle = !toggle;
	aptr = list[toggle];
	while((cptr = *sptr++) != NULL)
	  {
	    CELL      *tptr;
	    
	    /*{{{  go up?*/
	    if(cptr->depths[0])
	      {
		tptr = cptr - CELL_STRIDE;
		if(tptr->visit && !tptr->ball)
		  {
		    tptr->ball = count;
		    *aptr++ = tptr;
		  }
	      }
	    /*}}}*/
	    /*{{{  go down?*/
	    if(cptr->depths[1])
	      {
		tptr = cptr + CELL_STRIDE;
		if(tptr->visit && !tptr->ball)
		  {
		    tptr->ball = count;
		    *aptr++ = tptr;
		  }
	      }
	    /*}}}*/
	    /*{{{  go left?*/
	    if(cptr->depths[2])
	      {
		tptr = cptr - 1;
		if(tptr->visit && !tptr->ball)
		  {
		    tptr->ball = count;
		    *aptr++ = tptr;
		  }
	      }
	    /*}}}*/
	    /*{{{  go right?*/
	    if(cptr->depths[3])
	      {
		tptr = cptr + 1;
		if(tptr->visit && !tptr->ball)
		  {
		    tptr->ball = count;
		    *aptr++ = tptr;
		  }
	      }
	    /*}}}*/
	    assert(aptr - list[toggle] < FLOOD_FILL);
	  }
	*aptr = NULL;
      }
      pptr->ball = optr->ball = 0;
    }
    /*}}}*/
  if(player.ball.state == 1)
    /*{{{  hit a monster?*/
    {
      unsigned  i;
      MONSTER   *mptr;
	  
      tl.x -= CELL_WIDTH;
      tl.y -= CELL_HEIGHT;
      br.x += 1;
      br.y += 1;
      for(mptr = monster.list, i = monster.monsters; i--; mptr++)
	if(!mptr->shot && !mptr->squished &&
	    (mptr->type < 5 || mptr->type > 6) &&
	    (mptr != monster.list || !player.ball.image) &&
	    INRANGE(mptr->pixel.x, tl.x, br.x) &&
	    INRANGE(mptr->pixel.y, tl.y, br.y))
	  {
	    if(mptr->type == 4)
	      player.ball.state = 0;
	    else
	      {
		if(global.state == 2 || mptr->type == 2)
		  player.count = ball_returns[0];
		else
		  {
		    player.count = ball_returns[player.thrown];
		    if(player.thrown < BALL_RETURNS - 1)
		      player.thrown++;
		  }
		if(mptr->type < 6)
		  mptr->shot = 1;
		else if(INRANGE(mptr->type, SPRITE_MRIS + 4, SPRITE_MRIS + 8))
		  mptr->type -= 4;
		player.ball.state = 2;
		player.ball.count = 0;
	      }
	    break;
	  }
      if(player.ball.image)
	player.ball.image--;
    }
    /*}}}*/
  if(player.ball.state)
    global.throw = 2;
  else
    {
      player.ball.count = monster.list[0].face;
      player.ball.image = monster.list[0].image;
      player.ball.pixel.x = monster.list[0].pixel.x + CELL_WIDTH / 2;
      player.ball.pixel.y = monster.list[0].pixel.y + CELL_HEIGHT / 2;
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void move_player()*/
extern VOIDFUNC move_player FUNCARGVOID
/*
 * move the player
 * and deal with eating cherries etc
 * there is a wart to make it easier to stop just before a thin bit of
 * hedge and throw the ball through it.
 */
{
  unsigned  dir;
  unsigned  step;
  unsigned  stop;

  dir = monster.list[0].dir;
  stop = monster.list[0].stop;
  step = 0;
  /*{{{  key change?*/
  if(player.bashed || (player.pressed ^ global.pressed) & 0xF)
  /*
   * The keys have changed, so we need to select a new direction
   * if we're not an interchange to change dir, we keep going
   * in the current direction, if that is non-zero,
   * else we go to the nearest interchange
   */
  {
    unsigned  valid;
    unsigned  pressed;
	
    pressed = global.pressed & 0xF;
    if(!pressed)
      {
	if(dir == 0 && monster.list[0].offset.y <=
	    VEL_Y * STEP_OFF - CELL_HEIGHT - GAP_HEIGHT)
	  pressed = 0x1;
	else if(dir == 1 && monster.list[0].offset.y >=
	    CELL_HEIGHT + GAP_HEIGHT - VEL_Y * STEP_OFF)
	  pressed = 0x2;
	else if(dir == 2 && monster.list[0].offset.x <=
	    VEL_X * STEP_OFF - CELL_WIDTH - GAP_WIDTH)
	  pressed = 0x4;
	else if(dir == 3 && monster.list[0].offset.x >=
	    CELL_WIDTH + GAP_WIDTH - VEL_X * STEP_OFF)
	  pressed = 0x8;
	if(pressed)
	  step = 1;
      }
    pressed &= ~player.bashed;
    if(!pressed)
      pressed = global.pressed & 0xF;
    valid = 0xF;
    /*{{{  work out valid direction and the edges*/
    if(monster.list[0].offset.x)
      {
	valid &= 0xC;
	if(!monster.list[0].cell.y)
	  pressed &= 0xE;
	else if(monster.list[0].cell.y == CELLS_DOWN - 1)
	  pressed &= 0xD;
      }
    else if(monster.list[0].offset.y)
      {
	valid &= 0x3;
	if(!monster.list[0].cell.x)
	  pressed &= 0xB;
	else if(monster.list[0].cell.x == CELLS_ACROSS - 1)
	  pressed &= 0x7;
      }
    else
      {
	if(!monster.list[0].cell.x)
	  valid &= 0xB;
	else if(monster.list[0].cell.x == CELLS_ACROSS - 1)
	  valid &= 0x7;
	if(!monster.list[0].cell.y)
	  valid &= 0xE;
	else if(monster.list[0].cell.y == CELLS_DOWN - 1)
	  valid &= 0xD;
	pressed &= valid;
      }
    /*}}}*/
    if((pressed & 0x3) == 0x3)
      pressed &= 0xC;
    if((pressed & 0xC) == 0xC)
      pressed &= 0x3;
    if(!pressed)
      {
	step = 0;
	monster.list[0].stop = 1;
	player.next = 0;
      }
    else if((valid & pressed) == pressed)
      {
	unsigned  dir;
	
	if((pressed & 0x3) && (pressed & 0xC))
	  pressed &= monster.list[0].dir & 2 ? 0x3 : 0xC;
	for(dir = 0; !(pressed & 1); dir++, pressed >>= 1)
	  /* EMPTY */;
	player.next = 0;
	monster.list[0].dir = dir;
	monster.list[0].stop = 0;
      }
    else
      {
	unsigned  dir;
	
	pressed &= ~valid;
	for(dir = 0; !(pressed & 1); pressed >>= 1)
	  dir++;
	player.next = dir | 4;
	if(monster.list[0].stop)
	  {
	    /*{{{  select nearest cell*/
	    if(dir & 2)
	      {
		if(monster.list[0].offset.y <
		    -(CELL_HEIGHT + GAP_HEIGHT) * 3 / 4)
		  dir = 0;
		else if(monster.list[0].offset.y >
		    (CELL_HEIGHT + GAP_HEIGHT) * 3 / 4)
		  dir = 1;
		else if(monster.list[0].offset.y > 0 &&
		    monster.list[0].offset.y <=
		    (CELL_HEIGHT + GAP_HEIGHT) / 4)
		  dir = 0;
		else if(monster.list[0].offset.y < 0 &&
		    monster.list[0].offset.y >=
		    -(CELL_HEIGHT + GAP_HEIGHT) / 4)
		  dir = 1;
		else
		  dir = monster.list[0].dir;
	      }
	    else
	      {
		if(monster.list[0].offset.x <
		    -(CELL_WIDTH + GAP_WIDTH) * 3 / 4)
		  dir = 2;
		else if(monster.list[0].offset.x >
		    (CELL_WIDTH + GAP_WIDTH) * 3 / 4)
		  dir = 3;
		else if(monster.list[0].offset.x > 0 &&
		    monster.list[0].offset.x <=
		    (CELL_WIDTH + GAP_WIDTH) / 4)
		  dir = 2;
		else if(monster.list[0].offset.x < 0 &&
		    monster.list[0].offset.x >=
		    -(CELL_WIDTH + GAP_WIDTH) / 4)
		  dir = 3;
		else
		  dir = monster.list[0].dir;
	      }
	    /*}}}*/
	    monster.list[0].dir = dir;
	    monster.list[0].stop = 0;
	  }
      }
    if(monster.list[0].stop)
      player.bashed = 0;
    else if(player.bashed == 0x10)
      player.bashed = 0;
    else if(player.bashed & (1 << monster.list[0].dir))
      player.bashed = 0;
    else if(!(player.bashed & (3 << (monster.list[0].dir & 2))))
      player.bashed = 0x10;
  }
  /*}}}*/
  if(dir != monster.list[0].dir)
    {
      if(stop || dir == (monster.list[0].dir ^ 1))
	monster.list[0].pause = 1;
      new_face(&monster.list[0]);
    }
  if(monster.list[0].pause)
    {
      player.cherry = player.distance = 0;
      monster.list[0].pause = 0;
    }
  else if(!monster.list[0].stop)
    /*{{{  move*/
    {
      CELL      *cptr;
      
      if(!monster.list[0].cycle)
	{
	  monster.list[0].cycle = MONSTER_CYCLES;
	  monster.list[0].image++;
	  if(monster.list[0].image == MONSTER_IMAGES)
	    monster.list[0].image = 0;
	}
      monster.list[0].cycle--;
      cptr = move_muncher(&monster.list[0]);
      /*{{{  pushing?*/
      {
	unsigned  push;
	    
	push = monster.list[0].pause && (monster.list[0].dir & 2);
	if(push != monster.list[0].pushing)
	  {
	    monster.list[0].pushing = push;
	    new_face(&monster.list[0]);
	  }
      }
      /*}}}*/
      if(monster.list[0].stop)
	{
	  monster.list[0].pause = 0;
	  player.bashed = 1 << monster.list[0].dir;
	}
      if(cptr)
	{
	  monster.player = cptr;
	  global.stepped = 1;
	  if(cptr->sprite == SPRITE_CHERRY)
	    /*{{{  eat cherry*/
	    {
	      cptr->sprite = 0;
	      global.cherries--;
	      add_score(50 / SCORE_ROUND, 0, 0);
	      player.cherry++;
	      if(player.cherry == 8)
		{
		  add_score(500 / SCORE_ROUND,
		      monster.list[0].pixel.x + CELL_WIDTH / 2,
		      monster.list[0].pixel.y + CELL_HEIGHT / 2);
		  player.cherry = 0;
		}
	      else
		player.distance =
		    (CELL_WIDTH + GAP_WIDTH) / VEL_X;
	    }
	    /*}}}*/
	  else if(cptr->sprite >= SPRITE_PRIZE_BASE &&
	      cptr->sprite < SPRITE_PRIZE_BASE + SPRITE_PRIZES)
	    /*{{{  eat prize*/
	    {
	      unsigned  score;
	    
	      history.prize |= 1;
	      cptr->sprite = 0;
	      score = global.screen * (500 / SCORE_ROUND) + 500 / SCORE_ROUND;
	      add_score(score > 9500 / SCORE_ROUND ? 9500 / SCORE_ROUND :
		  score, monster.list[0].pixel.x + CELL_WIDTH / 2,
		  monster.list[0].pixel.y + CELL_HEIGHT / 2);
	      global.state = 2;
	      set_colors(0);
	      if(display.dynamic && global.screen > SPRITE_PRIZES - 1 &&
		  ((global.screen - 1) % SPRITE_PRIZES) ==
		  SPRITE_MUSHROOM && chaotic() < WHIZZ_PROB)
		global.whizz = 1;
	      if(!extra.escape)
		extra_escape();
	      else if(extra.escape == 2)
		{
		  extra.escape = 1;
		  extra.count = XTRA_HOME_DELAY;
		}
	      monster.den = 3;
	      monster.delay = 1;
	    }
	    /*}}}*/
	}
    }
    /*}}}*/
  if(!step)
    player.pressed = global.pressed;
  if(player.distance)
    player.distance--;
  else
    player.cherry = 0;
  /*{{{  player looping*/
  if(monster.list[0].stop ||
      monster.list[0].offset.x || monster.list[0].offset.y)
    /* EMPTY */;
  else if(player.next)
    {
      monster.list[0].dir = player.next & 3;
      player.next = 0;
      new_face(&monster.list[0]);
    }
  else if((!monster.list[0].cell.x && monster.list[0].dir == 2) ||
      (!monster.list[0].cell.y && monster.list[0].dir == 0) ||
      (monster.list[0].cell.x == CELLS_ACROSS - 1 &&
	  monster.list[0].dir == 3) ||
      (monster.list[0].cell.y == CELLS_DOWN - 1 &&
	  monster.list[0].dir == 1))
    {
      player.next = 0;
      monster.list[0].stop = 1;
      player.bashed = 1 << monster.list[0].dir;
    }
  else if(step)
    monster.list[0].stop = 1;
  /*}}}*/
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void killed_player()*/
extern VOIDFUNC killed_player FUNCARGVOID
/*
 * sees if the player has walked into a live monster
 */
{
  if(!monster.list[0].shot)
    {
      unsigned  i;
      MONSTER   *mptr;
      int       x, y;
	
      x = monster.list[0].pixel.x;
      y = monster.list[0].pixel.y;
      for(mptr = &monster.list[1], i = monster.monsters - 1; i--; mptr++)
	if(mptr->chew || mptr->shot || mptr->squished || mptr->type == 5)
	  /* EMPTY */;
	else if(mptr->type == 6)
	  {
	    if(mptr->pixel.x == x && mptr->pixel.y == y)
	      mptr->shot = 1;
	  }
	else
	  {
	    int     dx, dy;
	    
	    dx = mptr->pixel.x - x;
	    dy = mptr->pixel.y - y;
	    if(dx < 0)
	      dx = -dx;
	    if(dy < 0)
	      dy = -dy;
	    if(dx * CELL_HEIGHT + dy * CELL_WIDTH <
		CELL_WIDTH * CELL_HEIGHT)
	      {
		monster.list[0].shot = 1;
		break;
	      }
	  }
    }
  return VOIDRETURN;
}
/*}}}*/
