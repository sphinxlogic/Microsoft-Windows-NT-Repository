/* Copyright (C) 1992 Nathan Sidwell */
#include "xmris.h"
/*{{{  prototypes*/
static APPLE *apple_search PROTOARG((int, int, unsigned, unsigned, unsigned));
static VOIDFUNC squish_monster PROTOARG((MONSTER *));
/*}}}*/
/*{{{  APPLE *apple_search(x, y, width, height, found)*/
static APPLE *apple_search
FUNCARG((x, y, width, height, found),
	int       x       /* x coord start */
ARGSEP  int       y       /* y coord start */
ARGSEP  unsigned  width   /* width of box */
ARGSEP  unsigned  height  /* height of box */
ARGSEP  unsigned  found   /* already found ones */
)
/*
 * looks for an apple in the specified box
 * and returns a ptr to it, if found
 */
{
  unsigned  i;
  APPLE     *aptr;
  
  for(aptr = apple.list, i = 0; i != apple.apples; i++, aptr++, found >>= 1)
    if(!(found & 1) && INRANGE(aptr->pixel.x + aptr->push - x, 0, width) &&
	INRANGE(aptr->pixel.y - y, 0, height))
      return aptr;
  return NULL;
}
/*}}}*/
/*{{{  int apple_stop(mptr, cptr)*/
extern int apple_stop
FUNCARG((mptr, cptr),
	MONSTER   *mptr
ARGSEP  CELL      *cptr
)
/*
 * sees if the monster is about to walk into an apple
 * returns 0 if clear, 1 if stopped
 * sets mptr->pause & mptr->stop as required
 * the apples and other monsters are altered
 * if they're pushed about
 * called before we've moved the monster
 */
{
  unsigned  stop;
  unsigned  pause;
  
  pause = stop = 0;
  switch(mptr->dir)
  {
    /*{{{  case 0: (up)*/
    case 0:
    {
      unsigned  i;
      APPLE     *aptr;
      int       px, py;
    
      px = mptr->pixel.x;
      py = mptr->pixel.y;
      for(aptr = apple.list, i = apple.apples; i--; aptr++)
	{
	  if(INRANGE(aptr->pixel.x + aptr->push - px,
		1 - CELL_WIDTH, CELL_WIDTH) &&
	      INRANGE(py - aptr->pixel.y, CELL_HEIGHT / 2,
	      CELL_HEIGHT + VEL_Y) && !aptr->ghost)
	    {
	      stop = 1;
	      break;
	    }
	}
      break;
    }
    /*}}}*/
    /*{{{  case 1: (down)*/
    case 1:
    {
      unsigned  i;
      APPLE     *aptr;
      int       px, py;
	
      px = mptr->pixel.x;
      py = mptr->pixel.y;
      for(aptr = apple.list, i = apple.apples; i--; aptr++)
	{
	  if(INRANGE(aptr->pixel.x + aptr->push - px,
	      1 - CELL_WIDTH, CELL_WIDTH) &&
	      INRANGE(aptr->pixel.y + apple_sizes[aptr->state].offset.y - py,
	      CELL_HEIGHT / 2, CELL_HEIGHT + VEL_Y) && !aptr->ghost)
	    {
	      stop = 1;
	      break;
	    }
	}
      break;
    }
    /*}}}*/
    /*{{{  case 2: (left)*/
    case 2:
    {
      unsigned  found;
      int       x, y;
      int       width;
      APPLE     *final;
	  
      found = 0;
      final = NULL;
      width = VEL_X;
      x = mptr->pixel.x - CELL_WIDTH + 1 - width;
      y = mptr->pixel.y - CELL_HEIGHT + VEL_Y;
      /*{{{  look for apple*/
      for(;;)
      {
	APPLE     *aptr;
	CELL      *cptr;
	      
	aptr = apple_search(x, y, (unsigned)width + CELL_WIDTH / 2,
	    2 * (CELL_HEIGHT - VEL_Y), found);
	if(!aptr)
	  break;
	else if(aptr->push || aptr->pixel.x - width < PIXELX(0, 0))
	  {
	    stop = 1;
	    break;
	  }
	else if(aptr->state == 2 &&
	    ((mptr->type != 1 && mptr->type != 4) ||
	    (aptr->offset.x < (cptr = BOARDCELL(aptr->cell.x, aptr->cell.y +
		(aptr->offset.y <= 0)))->depths[2] &&
		  !cptr[-1].visit)))
	  break;
	else
	  {
	    final = aptr;
	    found |= 1 << (aptr - apple.list);
	    width = aptr->pixel.x - x + 1;
	    width = (width + APPLE_VEL_X - 1) / APPLE_VEL_X * APPLE_VEL_X;
	    aptr->maypush = width;
	    x = aptr->pixel.x - CELL_WIDTH + 1 - width;
	    if(aptr->state != 2 && aptr->offset.y > 0 &&
		apple_search(x, aptr->pixel.y - aptr->offset.y + CELL_HEIGHT,
		  (unsigned)width, (unsigned)aptr->offset.y, found))
	      {
		stop = 1;
		break;
	      }
	    y = aptr->pixel.y - CELL_HEIGHT + VEL_Y;
	    if(!width || aptr->state == 2)
	      break;
	  }
      }
      /*}}}*/
      if(found && !stop)
	{
	  /*{{{  check if against monster*/
	  if(width)
	    {
	      unsigned  i;
	      MONSTER   *mptr;
	      MONSTER   *list;
	  
	      list = NULL;
	      for(mptr = monster.list, i = monster.monsters; i--; mptr++)
		{
		  if(INRANGE(mptr->pixel.x - x, 0, width + CELL_WIDTH) &&
		      INRANGE(mptr->pixel.y - y, 0, 2 * (CELL_HEIGHT - VEL_Y)))
		    {
		      switch(mptr->type)
		      {
			/*{{{  case 0:*/
			case 0:
			{
			  CELL      *cptr;
			
			  cptr = BOARDCELL(mptr->cell.x, mptr->cell.y);
			  if(cptr->depths[2] <= mptr->offset.x - width ||
			      ((cptr->depths[1] || cptr->depths[0]) &&
			      mptr->offset.x >= 0))
			    {
			      mptr->list = list;
			      list = mptr;
			      mptr->push = -1;
			    }
			  else
			    stop = 1;
			  break;
			}
			/*}}}*/
			/*{{{  case 1:*/
			case 1:
			{
			  if(mptr->dir & 2 ||
			      mptr->pixel.x - x >= width + CELL_WIDTH / 2)
			    stop = 1;
			  break;
			  break;
			}
			/*}}}*/
			/*{{{  case 4:*/
			case 4:
			{
			  stop = 1;
			  break;
			}
			/*}}}*/
		      }
		    }
		}
	      if(final->state < 2)
		final->list = list;
	    }
	  /*}}}*/
	  if(!stop)
	  {
	    pause = 1;
	    /*{{{  push the apples*/
	    {
	      unsigned  i;
	      APPLE     *aptr;
	    
	      for(aptr = apple.list, i = apple.apples;
		  i--; aptr++, found >>= 1)
		if(found & 1)
		  aptr->push = -aptr->maypush;
	    }
	    /*}}}*/
	  }
	}
      break;
    }
    /*}}}*/
    /*{{{  case 3: (right)*/
    case 3:
    {
      unsigned  found;
      int       x, y;
      int       width;
      APPLE     *final;
	  
      found = 0;
      final = NULL;
      width = VEL_X;
      x = mptr->pixel.x + CELL_WIDTH;
      y = mptr->pixel.y - CELL_HEIGHT + VEL_Y;
      /*{{{  look for apple*/
      for(;;)
      {
	APPLE       *aptr;
	      
	aptr = apple_search(x - CELL_WIDTH / 2, y,
	    (unsigned)width + CELL_WIDTH / 2,
	    2 * (CELL_HEIGHT - VEL_Y), found);
	if(!aptr)
	  break;
	else if(aptr->push ||
	    aptr->pixel.x + width > PIXELX(CELLS_ACROSS - 1, 0))
	  {
	    stop = 1;
	    break;
	  }
	else if(aptr->state == 2 && ((mptr->type != 1 && mptr->type != 4) ||
	    (aptr->offset.x > (cptr = BOARDCELL(aptr->cell.x, aptr->cell.y +
		(aptr->offset.y <= 0)))->depths[3] &&
		  !cptr[1].visit)))
	  break;
	else
	  {
	    final = aptr;
	    found |= 1 << (aptr - apple.list);
	    width -= aptr->pixel.x - x;
	    width = (width + APPLE_VEL_X - 1) / APPLE_VEL_X * APPLE_VEL_X;
	    aptr->maypush = width;
	    x = aptr->pixel.x + CELL_WIDTH;
	    if(aptr->state != 2 && aptr->offset.y > 0 &&
		apple_search(x, aptr->pixel.y - aptr->offset.y + CELL_HEIGHT,
		    (unsigned)width, (unsigned)aptr->offset.y, found))
	      {
		stop = 1;
		break;
	      }
	    y = aptr->pixel.y - CELL_HEIGHT + VEL_Y;
	    if(!width || aptr->state == 2)
	      break;
	  }
      }
      /*}}}*/
      /*{{{  check if against monster*/
      if(found && width)
	{
	  unsigned  i;
	  MONSTER   *mptr;
	  MONSTER   *list;
	  
	  list = NULL;
	  for(mptr = monster.list, i = monster.monsters; i--; mptr++)
	    {
	      if(INRANGE(mptr->pixel.x - x, -CELL_WIDTH, width) &&
		  INRANGE(mptr->pixel.y - y, 0, 2 * (CELL_HEIGHT - VEL_Y)))
		{
		  switch(mptr->type)
		  {
		    /*{{{  case 0:*/
		    case 0:
		    {
		      CELL      *cptr;
		    
		      cptr = BOARDCELL(mptr->cell.x, mptr->cell.y);
		      if(cptr->depths[3] >= mptr->offset.x + width ||
			  ((cptr->depths[1] || cptr->depths[0]) &&
			  mptr->offset.x <= 0))
			{
			  mptr->list = list;
			  list = mptr;
			  mptr->push = 1;
			}
		      else
			stop = 1;
		      break;
		    }
		    /*}}}*/
		    /*{{{  case 1:*/
		    case 1:
		    {
		      if(mptr->dir & 2 ||
			  mptr->pixel.x - x < -CELL_WIDTH / 2)
			stop = 1;
		      break;
		    }
		    /*}}}*/
		    /*{{{  case 4:*/
		    case 4:
		    {
		      stop = 1;
		      break;
		    }
		    /*}}}*/
		  }
		}
	    }
	  if(final->state < 2)
	    final->list = list;
	}
      /*}}}*/
      if(found && !stop)
	{
	  pause = 1;
	  /*{{{  push the apples*/
	  {
	    unsigned  i;
	    APPLE     *aptr;
	  
	    for(aptr = apple.list, i = apple.apples; i--; aptr++, found >>= 1)
	      if(found & 1)
		aptr->push = aptr->maypush;
	  }
	  /*}}}*/
	}
      break;
    }
    /*}}}*/
  }
  mptr->stop = stop;
  mptr->pause = pause;
  return stop;
}
/*}}}*/
/*{{{  void move_apples()*/
extern VOIDFUNC move_apples FUNCARGVOID
/*
 * moves all the apples
 */
{
  APPLE     *aptr;
  unsigned  i;

  for(aptr = apple.list, i = apple.apples; i--; aptr++)
    {
      CELL      *cptr;
  
      cptr = BOARDCELL(aptr->cell.x, aptr->cell.y);
      if(aptr->chewed)
	{
	  assert(!aptr->monsters);
	  aptr->state = 6;
	}
      else
	{
	  if(aptr->ghost)
	    aptr->ghost--;
	  /*{{{  pushed?*/
	  if(aptr->push)
	    {
	      aptr->pixel.x += aptr->push;
	      aptr->offset.x += aptr->push;
	      assert(!(aptr->offset.x % APPLE_VEL_X));
	      if(aptr->offset.x < -(CELL_WIDTH / 2))
		{
		  aptr->offset.x += CELL_WIDTH + GAP_WIDTH;
		  aptr->cell.x -= 1;
		  cptr -= 1;
		}
	      else if(aptr->offset.x > (CELL_WIDTH / 2))
		{
		  aptr->offset.x -= CELL_WIDTH + GAP_WIDTH;
		  aptr->cell.x += 1;
		  cptr += 1;
		}
	    }
	  /*}}}*/
	  switch(aptr->state)
	  {
	    /*{{{  case 0: (static) case 1: (rock)*/
	    case 0: case 1:
	    {
	      unsigned  old;
	      unsigned  new;
	      int       offset;
	    
	      old = aptr->state;
	      new = 0;
	      offset = 0;
	      if(aptr->offset.y < cptr->depths[1])
		new = 2;
	      else if(aptr->cell.y == CELLS_DOWN - 1)
		{
		  new = 3;
		  aptr->ghost = 0;
		}
	      else if(cptr[CELL_STRIDE].visit)
		new = 1;
	      else if(aptr->offset.y - cptr[CELL_STRIDE*2].depths[0] >=
		  CELL_HEIGHT + GAP_HEIGHT)
		new = 1;
	      else if(cptr[CELL_STRIDE + 1].visit &&
		  cptr[CELL_STRIDE + 1].depths[2] - aptr->offset.x <
		  -(CELL_WIDTH / 2))
		{
		  new = 1;
		  offset = 1;
		}
	      else if(cptr[CELL_STRIDE - 1].visit &&
		  cptr[CELL_STRIDE - 1].depths[3] - aptr->offset.x >
		  CELL_WIDTH / 2)
		{
		  new = 1;
		  offset = -1;
		}
	      if(new == 1 && old == 1 && !aptr->count--)
		new = 2;
	      if(new)
		/*{{{  check for supporting monster*/
		{
		  MONSTER   *mptr;
		  unsigned  count;
		  int       x, y;
		
		  x = aptr->pixel.x;
		  y = aptr->pixel.y;
		  for(mptr = monster.list, count = monster.monsters;
		      count--; mptr++)
		    if((mptr->type == 4 || mptr->type == 1) && !mptr->shot &&
			INRANGE(mptr->pixel.x - x, 1 + VEL_X - CELL_WIDTH,
			  CELL_WIDTH - VEL_X) &&
			INRANGE(mptr->pixel.y - y, CELL_HEIGHT / 2,
			  CELL_HEIGHT + GAP_HEIGHT + 1))
		      {
			new = 0;
			break;
		      }
		    else if(mptr->type & 2 && !mptr->dir &&
			INRANGE(mptr->pixel.y - y, 0, CELL_HEIGHT + 1) &&
			INRANGE(mptr->pixel.x - x, 1 - CELL_WIDTH / 2,
			  CELL_WIDTH / 2))
		      {
			new = 0;
			break;
		      }
		}
		/*}}}*/
	      aptr->state = new;
	      if(new == 0)
		aptr->list = NULL;
	      else if(new == 1)
		/*{{{  rock*/
		{
		  aptr->list = NULL;
		  if(old != new)
		    aptr->count = APPLE_ROCK_DELAY;
		}
		/*}}}*/
	      else if(new == 2)
		/*{{{  start to fall*/
		{
		  MONSTER   *mptr;
		
		  aptr->count = APPLE_ACC;
		  aptr->distance = 0;
		  if(!cptr[offset].visit)
		    {
		      aptr->cell.y += 1;
		      aptr->offset.y -= CELL_HEIGHT + GAP_HEIGHT;
		    }
		  aptr->cell.x += offset;
		  aptr->offset.x -= offset * (CELL_WIDTH + GAP_WIDTH);
		  for(mptr = aptr->list; mptr; mptr = mptr->list)
		    {
		      squish_monster(mptr);
		      mptr->pixel.y += CELL_HEIGHT - CELL_HEIGHT / 4;
		      aptr->monsters++;
		      aptr->distance = APPLE_FALL_SPLIT;
		    }
		  panic_monsters(aptr->cell.x, aptr->cell.y,
		      BOARDCELL(aptr->cell.x, aptr->cell.y));
		}
		/*}}}*/
	      else
		aptr->count = APPLE_SPLIT_DELAY;
	      break;
	    }
	    /*}}}*/
	    /*{{{  case 2: (fall)*/
	    case 2:
	    {
	      unsigned  j;
	      APPLE     *optr;
	      COORD     pixel;
	    
	      /*{{{  horizontal movement*/
	      if(!aptr->push)
		{
		  int     shift;
		  int     x;
		  
		  shift = x = 0;
		  /*{{{  desired direction*/
		  if(aptr->offset.x < 0)
		    {
		      if(aptr->offset.x < cptr->depths[2])
		       {
			  shift = APPLE_VEL_X;
			  x = aptr->pixel.x + CELL_WIDTH;
			}
		    }
		  else if(aptr->offset.x > 0)
		    {
		      if(aptr->offset.x > cptr->depths[3])
			{
			  shift = -APPLE_VEL_X;
			  x = aptr->pixel.x - CELL_WIDTH - APPLE_VEL_X;
			}
		    }
		  /*}}}*/
		  if(shift)
		    /*{{{  move if not blocked*/
		    {
		      MONSTER   *mptr;
		      unsigned  count;
		      
		      for(mptr = monster.list, count = monster.monsters;
			  count--; mptr++)
			if((mptr->type == 1 || mptr->type == 2) &&
			    !mptr->shot && !mptr->squished &&
			    INRANGE(mptr->pixel.x - x, 0, APPLE_VEL_X) &&
			    INRANGE(mptr->pixel.y - aptr->pixel.y,
			      VEL_Y - CELL_HEIGHT + 1, CELL_HEIGHT - VEL_Y))
			  {
			    shift =  0;
			    break;
			  }
		      if(shift)
			{
			  aptr->offset.x += shift;
			  aptr->pixel.x += shift;
			}
		    }
		    /*}}}*/
		}
	      /*}}}*/
	      pixel.x = aptr->pixel.x - aptr->offset.x;
	      pixel.y = aptr->pixel.y - aptr->offset.y;
	      aptr->offset.y += aptr->count;
	      aptr->pixel.y += aptr->count;
	      aptr->distance += aptr->count;
	      aptr->count += APPLE_ACC;
	      if(aptr->count > APPLE_VEL_Y)
		aptr->count = APPLE_VEL_Y;
	      if(!aptr->ghost)
		/*{{{  bashes into another?*/
		{
		  int       x, y;
		
		  x = aptr->pixel.x;
		  y = aptr->pixel.y;
		  for(optr = apple.list, j = apple.apples; j--; optr++)
		    if(optr != aptr && !optr->ghost &&
			INRANGE(optr->pixel.x - x,
			  1 - CELL_WIDTH, CELL_WIDTH) &&
			INRANGE(optr->pixel.y +
			  apple_sizes[optr->state].offset.y - y, 1,
			  CELL_HEIGHT - APPLE_VEL_Y) &&
			(optr->state ||
			  BOARDCELL(optr->cell.x, optr->cell.y)->visit))
		      {
			if(optr->state < 2)
			  {
			    optr->state = 3;
			    optr->count = APPLE_SPLIT_DELAY;
			  }
			else if(optr->state == 2)
			  optr->distance = APPLE_FALL_SPLIT + 1;
			optr = NULL;
			break;
		      }
		}
		/*}}}*/
	      else
		optr = NULL;
	      if(!aptr->ghost && !optr)
		{
		  aptr->state = 3;
		  aptr->count = APPLE_SPLIT_DELAY;
		}
	      else
		{
		  CELL      *nptr;
	    
		  nptr = drop_apple(aptr, cptr);
		  if(nptr)
		    panic_monsters(aptr->cell.x, aptr->cell.y, nptr);
		  else if(aptr->offset.y <= cptr[0].depths[1])
		    /*{{{  not broken*/
		    {
		      if(aptr->offset.y > 0 && cptr[CELL_STRIDE].visit)
			{
			  aptr->cell.y += 1;
			  aptr->offset.y -= CELL_HEIGHT + GAP_HEIGHT;
			}
		    }
		    /*}}}*/
		  else
		    /*{{{  stop falling*/
		    {
		      aptr->pixel.y += cptr[0].depths[1] - aptr->offset.y;
		      aptr->offset.y = cptr[0].depths[1];
		      assert(aptr->offset.y < CELL_HEIGHT + GAP_HEIGHT);
		      if(aptr->distance > APPLE_FALL_SPLIT ||
			  aptr->cell.y == CELLS_DOWN - 1)
			{
			  aptr->state = 3;
			  aptr->count = APPLE_SPLIT_DELAY;
			  aptr->ghost = 0;
			}
		      else
			aptr->state = 0;
		    }
		    /*}}}*/
		}
	      if(aptr->state < 6)
		/*{{{  crushed someone?*/
		{
		  unsigned  i;
		  MONSTER   *mptr;
		  int       x, y;
		
		  x = aptr->pixel.x;
		  y = aptr->pixel.y;
		  for(mptr = monster.list, i = monster.monsters; i--; mptr++)
		    if(!mptr->squished && !mptr->shot && mptr->type < 5 &&
			(mptr->type != 4 ||
			(global.state != 4 && !aptr->ghost)) &&
			INRANGE(mptr->pixel.x - x, VEL_X - CELL_WIDTH,
			  CELL_WIDTH - VEL_X + 1) && INRANGE(mptr->pixel.y - y,
			    mptr->type & 2 && mptr->dir & 2 ?
			      -CELL_HEIGHT / 2 : VEL_Y,
			  CELL_HEIGHT / 2 + VEL_Y + 1))
		      {
			unsigned  squish;
			
			if(!(mptr->type & 2) || mptr->chew ||
			    aptr->monsters || mptr->dir == 1 ||
			    global.state == 4)
			  squish = mptr->pixel.y - y >= VEL_Y;
			else if(!mptr->dir)
			  /*{{{  going up*/
			  {
			    squish = 0;
			    if(mptr->pixel.y - y >= -APPLE_VEL_Y &&
				INRANGE(mptr->pixel.x - x,
				  -VEL_X * 2, VEL_X * 2 + 1))
			      {
				aptr->chewed = 1;
				mptr->chew = 1;
				break;
			      }
			  }
			  /*}}}*/
			else if(mptr->dir == 2)
			  /*{{{  going left*/
			  {
			    squish = mptr->pixel.x - x + y - mptr->pixel.y <
				  CELL_WIDTH / 2;
			    if(!squish && mptr->pixel.x - x <= 2 * VEL_X)
			      {
				aptr->chewed = 1;
				mptr->chew = 1;
				break;
			      }
			  }
			  /*}}}*/
			else
			  /*{{{  going right*/
			  {
			    squish = x - mptr->pixel.x + y - mptr->pixel.y <
				  CELL_WIDTH /2;
			    if(!squish && x - mptr->pixel.x <= 2 * VEL_X)
			      {
				aptr->chewed = 1;
				mptr->chew = 1;
				break;
			      }
			  }
			  /*}}}*/
			if(squish)
			  /*{{{  squish it*/
			  {
			    mptr->list = aptr->list;
			    aptr->list = mptr;
			    if(mptr != &monster.list[0])
			      aptr->monsters++;
			    aptr->distance = APPLE_FALL_SPLIT;
			    if(!aptr->state)
			      {
				aptr->state = 3;
				aptr->count = APPLE_SPLIT_DELAY;
			      }
			    squish_monster(mptr);
			  }
			  /*}}}*/
		      }
		}
		/*}}}*/
	      /*{{{  move the crushed monsters*/
	      {
		MONSTER   *mptr;
		  
		for(mptr = aptr->list; mptr; mptr = mptr->list)
		  mptr->pixel.y = aptr->pixel.y + CELL_HEIGHT;
	      }
	      /*}}}*/
	      break;
	    }
	    /*}}}*/
	    /*{{{  case 3: (split)*/
	    case 3:
	      if(!aptr->count--)
		{
		  aptr->state = 4;
		  aptr->count = APPLE_DECAY_DELAY;
		  /*{{{  delete the crushed monsters*/
		  {
		    MONSTER   *mptr;
		      
		    for(mptr = aptr->list; mptr; mptr = mptr->list)
		      if(mptr->type != 4)
			mptr->type = 5;
		  }
		  /*}}}*/
		  if(aptr->monsters)
		    add_score(squish_scores[aptr->monsters >= SQUISH_SCORES ?
			SQUISH_SCORES - 1 : aptr->monsters],
			aptr->pixel.x + CELL_WIDTH / 2,
			aptr->pixel.y + (int)(apple_sizes[4].size.y / 2));
		  if(!aptr->offset.x && !aptr->offset.y && !global.diamond &&
		      global.state != 4 && chaotic() < DIAMOND_PROB)
		    {
		      global.diamond = 1;
		      spawn_monster(6, 1, 1, aptr->cell.x, aptr->cell.y,
			  0, 0)->count = DIAMOND_DELAY;
		    }
		}
	      break;
	    /*}}}*/
	    /*{{{  case 4: (decay)*/
	    case 4:
	      if(!aptr->count--)
		{
		  aptr->state = 5;
		  aptr->count = APPLE_ROT_DELAY;
		}
	      break;
	    /*}}}*/
	    /*{{{  case 5: (rot)*/
	    case 5:
	      if(!aptr->count--)
		aptr->state = 6;
	      break;
	    /*}}}*/
	    /*{{{  default:*/
	    default:
	      assert(0);
	    /*}}}*/
	  }
	}
      aptr->push = 0;
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void panic_monsters(cx, cy, cptr)*/
extern VOIDFUNC panic_monsters
FUNCARG((cx, cy, cptr),
	int       cx
ARGSEP  int       cy
ARGSEP  CELL      *cptr
)
/*
 * looks underneath a newly falling apple and
 * sees if there are any normal monsters to panic
 */
{
  MONSTER   *mptr;
  unsigned  count;
  unsigned  by;
  
  for(by = ++cy; cptr->depths[1] >= GAP_HEIGHT; cptr++)
    by++;
  for(mptr = &monster.list[1], count = monster.monsters - 1; count--; mptr++)
    if(!mptr->squished && !mptr->shot && !mptr->type && !mptr->panic &&
	mptr->cell.x == cx && INRANGE(mptr->cell.y, cy, by) &&
	(mptr->offset.x >= 0 || mptr->dir != 2) &&
	(mptr->offset.x <= 0 || mptr->dir != 3))
      mptr->panic = 1;
  return VOIDRETURN;
}
/*}}}*/
/*{{{  APPLE *spawn_apple(cx, cy, ox, oy)*/
extern APPLE *spawn_apple
FUNCARG((cx, cy, ox, oy),
	int       cx
ARGSEP  int       cy
ARGSEP  int       ox
ARGSEP  int       oy
)
/*
 * spawns a new apple in state 0
 */
{
  APPLE     *aptr;

  assert(apple.apples != APPLES);
  aptr = &apple.list[apple.apples++];
  aptr->cell.x = cx;
  aptr->cell.y = cy;
  aptr->offset.x = ox = ox / APPLE_VEL_X * APPLE_VEL_X;
  aptr->offset.y = oy;
  aptr->old_pixel.x = aptr->pixel.x = PIXELX(cx, ox);
  aptr->old_pixel.y = aptr->pixel.y = PIXELY(cy, oy);
  aptr->ghost = aptr->state = 0;
  aptr->count = 1;
  aptr->distance = 0;
  aptr->monsters = 0;
  aptr->list = NULL;
  aptr->push = 0;
  aptr->chewed = 0;
  aptr->old_state = 7;
  aptr->back = 0;
  return aptr;
}
/*}}}*/
/*{{{  void squish_monster(mptr)*/
static VOIDFUNC squish_monster
FUNCARG((mptr),
	MONSTER   *mptr
)
/*
 * oh no, I've been squashed!
 */
{
  mptr->squished = 1;
  new_face(mptr);
  if(mptr == &monster.list[0])
    mptr->shot = 1;
  else
    {
      mptr->chew = 0;
      if(mptr->type < 2)
	monster.normals--;
      else if(mptr->type == 2)
	extra_dies();
      else if(mptr->type == 3)
	monster.drones--;
    }
  return VOIDRETURN;
}
/*}}}*/
