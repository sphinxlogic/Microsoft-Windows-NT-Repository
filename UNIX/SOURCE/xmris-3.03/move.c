/* Copyright (C) 1992 Nathan Sidwell */
#include "xmris.h"
/*{{{  prototypes*/
static VOIDFUNC connect_hole PROTOARG((CELL *));
static int apple_hole PROTOARG((APPLE *, CELL *));
static VOIDFUNC munch_back
    PROTOARG((int, int, unsigned, unsigned, int, int, SPRITE *));
static VOIDFUNC munch_edge_above PROTOARG((CELL *, int, int, unsigned));
static VOIDFUNC munch_edge_below PROTOARG((CELL *, int, int, int, unsigned));
static VOIDFUNC munch_edge_left PROTOARG((CELL *, int, int, unsigned));
static VOIDFUNC munch_edge_right PROTOARG((CELL *, int, int, unsigned));
/*}}}*/
/*{{{  int apple_hole(aptr, cptr)*/
static int apple_hole
FUNCARG((aptr, cptr),
	APPLE   *aptr
ARGSEP  CELL    *cptr
)
/*
 * An apple has fallen, this makes the hole it
 * leaves behind in the hedge, but only in the board table
 * remember to get it drawn too.
 */
{
  int     offset;
  
  offset = aptr->offset.x / VEL_X * VEL_X;
  if(offset < 0)
    {
      int     max;
      
      max = cptr[0].depths[2] < cptr[CELL_STRIDE].depths[2] ?
	  cptr[CELL_STRIDE].depths[2] : cptr[0].depths[2];
      if(max >= offset)
	offset = max + VEL_X;
      if(offset < 0)
	{
	  assert(offset > -(CELL_WIDTH + GAP_WIDTH));
	  cptr[CELL_STRIDE].holes[0] = cptr[0].holes[2] = offset;
	  cptr[CELL_STRIDE - 1].holes[1] =
	      cptr[-1].holes[3] = CELL_WIDTH + GAP_WIDTH + offset;
	}
      else
	offset = 0;
    }
  else if(offset > 0)
    {
      int     min;
      
      min = cptr[0].depths[3] > cptr[CELL_STRIDE].depths[3] ?
	  cptr[CELL_STRIDE].depths[3] : cptr[0].depths[3];
      if(min <= offset)
	offset = min - VEL_X;
      if(offset > 0)
	{
	  assert(offset < (CELL_WIDTH + GAP_WIDTH));
	  cptr[CELL_STRIDE].holes[1] = cptr[0].holes[3] = offset;
	  cptr[CELL_STRIDE + 1].holes[0] =
	      cptr[1].holes[2] = -(CELL_WIDTH + GAP_WIDTH) + offset;
	}
      else
	offset = 0;
    }
  return offset;
}
/*}}}*/
/*{{{  void back_sprite(ix, flag, x, y)*/
extern VOIDFUNC back_sprite
FUNCARG((ix, flag, x, y),
	unsigned  ix
ARGSEP  unsigned  flag
ARGSEP  int       x
ARGSEP  int       y
)
/*
 * copy a sprite onto the background
 */
{
  SPRITE const *sptr;
  
  sptr = &sprites[ix];
  if(display.background != COLOUR_ZERO || flag)
    XCopyArea(display.display, sptr->mask, display.back, GCN(GC_MASK),
	0, 0, sptr->size.x, sptr->size.y, x, y);
  XCopyArea(display.display, sptr->image, display.back, GCN(GC_OR),
      0, 0, sptr->size.x, sptr->size.y, x, y);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned choose_direction(valid)*/
extern unsigned choose_direction
FUNCARG((valid),
	unsigned  valid
)
/*
 * pick a direction at chaotic from the valid ones
 */
{
  unsigned  choices;
  unsigned  temp;
  unsigned  choice;
  
  assert(valid && !(valid & ~0xF));
  for(choices = 0, temp = valid; temp; choices++)
    temp ^= temp & -temp;
  if(choices == 1)
    choice = 0;
  else if(choices == 3)
    choice = chaotic() % 3;
  else
    choice = chaotic() & (choices - 1);
  do
    {
      temp = valid & -valid;
      valid ^= temp;
    }
  while(choice--);
  assert(temp);
  for(valid = 0; !(temp & 1); valid++)
    temp >>= 1;
  return valid;
}
/*}}}*/
/*{{{  void connect_hole(cptr)*/
static VOIDFUNC connect_hole
FUNCARG((cptr),
	CELL      *cptr
)
/*
 * connect the current cell up to its neighbours
 */
{
  cptr[0].visit = 1;
  if(cptr[-CELL_STRIDE].depths[1] >= GAP_HEIGHT)
    {
      cptr[0].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
      cptr[-CELL_STRIDE].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
    }
  if(cptr[CELL_STRIDE].depths[0] <= -GAP_HEIGHT)
    {
      cptr[0].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
      cptr[CELL_STRIDE].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
    }
  if(cptr[-1].depths[3] >= GAP_WIDTH)
    {
      cptr[0].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
      cptr[-1].depths[3] = CELL_WIDTH + GAP_WIDTH;
    }
  if(cptr[1].depths[2] <= -GAP_WIDTH)
    {
      cptr[0].depths[3] = CELL_WIDTH + GAP_WIDTH;
      cptr[1].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  CELL *drop_apple(aptr, cptr)*/
extern CELL *drop_apple
FUNCARG((aptr, cptr),
	APPLE   *aptr
ARGSEP  CELL    *cptr
)
/*
 * deals with apples which break through to the cell below
 * the apple has already been moved to the new coordinate
 * returns NULL if we stay in the same cell, or a pointer
 * to the new cell
 */
{
  CELL      *new;
  COORD     pixel;
  CELL      *cherry;

  update.set = 0;
  pixel.x = aptr->pixel.x - aptr->offset.x;
  pixel.y = aptr->pixel.y - aptr->offset.y;
  cherry = NULL;
  if(aptr->offset.y <= cptr[0].depths[1])
    {
      if(INRANGE(aptr->offset.y, 1, APPLE_VEL_Y + 1))
	{
	  int     offset;
      
	  offset = apple_hole(aptr, cptr);
	  if(offset)
	    munch_edge_below(cptr, pixel.x + offset, pixel.y, offset, 0);
	}
      new = NULL;
    }
  else if(!cptr[0].visit)
    {
      aptr->offset.y -= CELL_HEIGHT + GAP_HEIGHT;
      aptr->cell.y += 1;
      pixel.y += CELL_HEIGHT + GAP_HEIGHT;
      new = cptr + CELL_STRIDE;
    }
  else if(cptr[CELL_STRIDE].visit)
    /*{{{  break through below*/
    {
      int   offset;
      
      offset = apple_hole(aptr, cptr);
      /*{{{  munch*/
      {
	munch_edge_below(cptr, pixel.x + offset, pixel.y,
	    offset, (unsigned)!offset);
	if(!offset && !cptr[0].depths[2])
	  {
	    munch_back(0, 0, EDGE_WIDTH >> 1, GAP_HEIGHT,
		pixel.x + (CELL_WIDTH >> 1) - (EDGE_WIDTH >> 1),
		pixel.y + CELL_HEIGHT - GAP_HEIGHT,
		&sprites[SPRITE_EDGE_BASE + 1]);
	    cherry = cptr;
	  }
	if(!offset && !cptr[0].depths[3])
	  {
	    munch_back(EDGE_WIDTH >> 1, 0,
		EDGE_WIDTH >> 1, GAP_HEIGHT,
		pixel.x + (CELL_WIDTH >> 1),
		pixel.y + CELL_HEIGHT - GAP_HEIGHT,
		&sprites[SPRITE_EDGE_BASE + 1]);
	    cherry = cptr;
	  }
      }
      /*}}}*/
      aptr->offset.y -= CELL_HEIGHT + GAP_HEIGHT;
      aptr->cell.y += 1;
      pixel.y += CELL_HEIGHT + GAP_HEIGHT;
      global.broken = 1;
      new = cptr + CELL_STRIDE;
      if(!offset)
	{
	  cptr[0].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
	  cptr[CELL_STRIDE].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
	}
    }
    /*}}}*/
  else if(cptr[0].depths[1] - cptr[CELL_STRIDE * 2].depths[0] >=
      CELL_HEIGHT + GAP_HEIGHT)
    /*{{{  breakthrough 2 below*/
    {
      aptr->offset.y -= CELL_HEIGHT + GAP_HEIGHT;
      aptr->cell.y += 1;
      pixel.y += CELL_HEIGHT + GAP_HEIGHT;
      global.broken = 1;
      cptr[CELL_STRIDE].visit = 1;
      cptr[0].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
      cptr[CELL_STRIDE].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
      cptr[CELL_STRIDE].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
      cptr[CELL_STRIDE * 2].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
      if(cptr[CELL_STRIDE - 1].depths[3])
	{
	  cptr[CELL_STRIDE].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
	  cptr[CELL_STRIDE - 1].depths[3] = CELL_WIDTH + GAP_WIDTH;
	}
      if(cptr[CELL_STRIDE + 1].depths[2])
	{
	  cptr[CELL_STRIDE].depths[3] = CELL_WIDTH + GAP_WIDTH;
	  cptr[CELL_STRIDE + 1].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
	}
      munch_hole(cptr + CELL_STRIDE, pixel.x, pixel.y);
      cherry = new = cptr + CELL_STRIDE;
    }
    /*}}}*/
  else
    new = NULL;
  if(cherry && cherry->sprite)
    back_sprite(new->sprite, 0, pixel.x, pixel.y);
  if(update.set)
    {
      bounding_box(aptr->pixel.x, aptr->pixel.y, CELL_WIDTH, CELL_HEIGHT);
      bounding_box(aptr->old_pixel.x, aptr->old_pixel.y,
	CELL_WIDTH, CELL_HEIGHT);
      aptr->back = 1;
      add_background(update.tl.x, update.tl.y,
	  (unsigned)(update.br.x - update.tl.x),
	  (unsigned)(update.br.y - update.tl.y));
    }
  return new;
}
/*}}}*/
/*{{{  CELL *move_diamond(mptr, cptr)*/
extern CELL *move_diamond
FUNCARG((mptr, cptr),
	MONSTER   *mptr
ARGSEP  CELL      *cptr
)
/*
 * move the diamond downwards, munching away as we go
 */
{
  CELL      *nptr;
  
  assert(!mptr->offset.x && mptr->dir == 1);
  nptr = NULL;
  if(mptr->offset.y)
    /* EMPTY */;
  else if(cptr[CELL_STRIDE].visit || cptr[CELL_STRIDE].sprite ||
      mptr->cell.y == CELLS_DOWN - 1)
    mptr->stop = 1;
  else
    {
      APPLE     *aptr;
      unsigned  count;
      
      for(aptr = apple.list, count = apple.apples; count--; aptr++)
	if(aptr->state < 3 && aptr->cell.y == mptr->cell.y + 1 &&
	    INRANGE(aptr->pixel.x - mptr->pixel.x,
	      VEL_X - CELL_WIDTH + 1, CELL_WIDTH - VEL_X))
	  {
	    mptr->stop = 1;
	    break;
	  }
    }
  if(!mptr->stop)
    {
      mptr->offset.y += DIAMOND_VEL_Y;
      mptr->pixel.y += DIAMOND_VEL_Y;
      update.set = 0;
      cptr[0].depths[1] = mptr->offset.y;
      if(mptr->offset.y < CELL_HEIGHT + GAP_HEIGHT)
	{
	  munch_back(0, CELL_HEIGHT - DIAMOND_VEL_Y - INTERNAL_HEIGHT,
	      EDGE_WIDTH, DIAMOND_VEL_Y + INTERNAL_HEIGHT,
	      mptr->pixel.x, mptr->pixel.y + CELL_HEIGHT -
	      DIAMOND_VEL_Y - INTERNAL_HEIGHT, &sprites[SPRITE_CENTER_BASE]);
	}
      else
	{
	  nptr = cptr + CELL_STRIDE;
	  nptr[0].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
	  connect_hole(nptr);
	  munch_hole(nptr, mptr->pixel.x, mptr->pixel.y);
	  if(nptr->sprite)
	    back_sprite(nptr->sprite, 0, mptr->pixel.x, mptr->pixel.y);
	  mptr->offset.y = 0;
	  mptr->cell.y += 1;
	  global.broken = 1;
	}
      assert(update.set);
      bounding_box(mptr->pixel.x, mptr->pixel.y, CELL_WIDTH, CELL_HEIGHT);
      bounding_box(mptr->old_pixel.x, mptr->old_pixel.y,
	  CELL_WIDTH, CELL_HEIGHT);
      add_background(update.tl.x, update.tl.y,
	  (unsigned)(update.br.x - update.tl.x),
	  (unsigned)(update.br.y - update.tl.y));
    }
  return nptr;
}
/*}}}*/
/*{{{  CELL *move_movable(mptr, cptr)*/
extern CELL *move_movable
FUNCARG((mptr, cptr),
	MONSTER   *mptr
ARGSEP  CELL      *cptr
)
/*
 * move a non muncher down a path
 * we should never leave known territory
 */
{
  unsigned  delta;
  
  switch(mptr->dir)
  {
    /*{{{  case 0: (up)*/
    case 0:
      if(!mptr->fast)
	delta = VEL_Y;
      else if(mptr->offset.y >
	  (CELL_HEIGHT + GAP_HEIGHT) - (FAST_STEPS * VEL_Y_FAST))
	delta = VEL_Y_FAST;
      else if(mptr->offset.y > (VEL_Y_FAST * FAST_STEPS))
	delta = VEL_Y;
      else if(mptr->offset.y > -(VEL_Y_FAST * FAST_STEPS))
	delta = VEL_Y_FAST;
      else if(mptr->offset.y >
	  (VEL_Y_FAST * FAST_STEPS) - (CELL_HEIGHT + GAP_HEIGHT))
	delta = VEL_Y;
      else
	delta = VEL_Y_FAST;
      mptr->pixel.y -= delta;
      mptr->offset.y -= delta;
      if(mptr->offset.y == -(CELL_HEIGHT + GAP_HEIGHT) && mptr->cell.y)
	{
	  mptr->offset.y = 0;
	  mptr->cell.y -= 1;
	  cptr -= CELL_STRIDE;
	  if(!cptr->visit)
	    {
	      if(mptr->offset.x < 0)
		{
		  mptr->offset.x += CELL_WIDTH + GAP_WIDTH;
		  mptr->cell.x -= 1;
		  cptr -= 1;
		}
	      else if(mptr->offset.x > 0)
		{
		  mptr->offset.x -= CELL_WIDTH + GAP_WIDTH;
		  mptr->cell.x += 1;
		  cptr += 1;
		}
	      else
		{
		  mptr->offset.y = -(CELL_HEIGHT + GAP_HEIGHT);
		  mptr->cell.y += 1;
		  cptr += CELL_STRIDE;
		}
	    }
	}
      assert(cptr->visit && mptr->cell.y >= 0);
      assert(mptr->offset.y >= -(CELL_HEIGHT + GAP_HEIGHT));
      break;
    /*}}}*/
    /*{{{  case 1: (down)*/
    case 1:
      if(!mptr->fast)
	delta = VEL_Y;
      else if(mptr->offset.y <
	  (FAST_STEPS * VEL_Y_FAST) - (CELL_HEIGHT + GAP_HEIGHT))
	delta = VEL_Y_FAST;
      else if(mptr->offset.y < -(VEL_Y_FAST * FAST_STEPS))
	delta = VEL_Y;
      else if(mptr->offset.y < (VEL_Y_FAST * FAST_STEPS))
	delta = VEL_Y_FAST;
      else if(mptr->offset.y <
	  (CELL_HEIGHT + GAP_HEIGHT) - (VEL_Y_FAST * FAST_STEPS))
	delta = VEL_Y;
      else
	delta = VEL_Y_FAST;
      mptr->pixel.y += delta;
      mptr->offset.y += delta;
      if(mptr->offset.y == CELL_HEIGHT + GAP_HEIGHT)
	{
	  mptr->offset.y = 0;
	  mptr->cell.y += 1;
	  cptr += CELL_STRIDE;
	  if(!cptr->visit)
	    {
	      if(mptr->offset.x < 0)
		{
		  mptr->offset.x += CELL_WIDTH + GAP_WIDTH;
		  mptr->cell.x -= 1;
		  cptr -= 1;
		}
	      else if(mptr->offset.x > 0)
		{
		  mptr->offset.x -= CELL_WIDTH + GAP_WIDTH;
		  mptr->cell.x += 1;
		  cptr += 1;
		}
	      else
		{
		  mptr->offset.y = CELL_HEIGHT + GAP_HEIGHT;
		  mptr->cell.y -= 1;
		  cptr -= CELL_STRIDE;
		}
	    }
	}
      assert(cptr->visit && mptr->cell.y < CELLS_DOWN);
      assert(mptr->offset.y <= CELL_HEIGHT + GAP_HEIGHT);
      break;
    /*}}}*/
    /*{{{  case 2: (left)*/
    case 2:
      if(!mptr->fast)
	delta = VEL_X;
      else if(mptr->offset.x >
	  (CELL_WIDTH + GAP_WIDTH) - (VEL_X_FAST * FAST_STEPS))
	delta = VEL_X_FAST;
      else if(mptr->offset.x > (VEL_X_FAST * FAST_STEPS))
	delta = VEL_X;
      else if(mptr->offset.x > -(VEL_X_FAST * FAST_STEPS))
	delta = VEL_X_FAST;
      else if(mptr->offset.x >
	  (FAST_STEPS * VEL_X_FAST) - (CELL_WIDTH + GAP_WIDTH))
	delta = VEL_X;
      else
	delta = VEL_X_FAST;
      mptr->pixel.x -= delta;
      mptr->offset.x -= delta;
      if(mptr->offset.x == -(CELL_WIDTH + GAP_WIDTH) && cptr[-1].visit)
	{
	  mptr->offset.x = 0;
	  mptr->cell.x -= 1;
	  cptr -= 1;
	}
      assert(cptr->visit && !mptr->offset.y && mptr->cell.x >= 0);
      assert(mptr->offset.x >= -(CELL_WIDTH + GAP_WIDTH));
      break;
    /*}}}*/
    /*{{{  case 3: (right)*/
    case 3:
      if(!mptr->fast)
	delta = VEL_X;
      else if(mptr->offset.x <
	  (FAST_STEPS * VEL_X_FAST) - (CELL_WIDTH + GAP_WIDTH))
	delta = VEL_X_FAST;
      else if(mptr->offset.x < -(VEL_X_FAST * FAST_STEPS))
	delta = VEL_X;
      else if(mptr->offset.x < (VEL_X_FAST * FAST_STEPS))
	delta = VEL_X_FAST;
      else if(mptr->offset.x <
	  (CELL_WIDTH + GAP_WIDTH) - (VEL_X_FAST * FAST_STEPS))
	delta = VEL_X;
      else
	delta = VEL_X_FAST;
      mptr->pixel.x += delta;
      mptr->offset.x += delta;
      if(mptr->offset.x == CELL_WIDTH + GAP_WIDTH && cptr[1].visit)
	{
	  mptr->offset.x = 0;
	  mptr->cell.x += 1;
	  cptr += 1;
	}
      assert(cptr->visit && !mptr->offset.y && mptr->cell.x < CELLS_ACROSS);
      assert(mptr->offset.x <= CELL_WIDTH + GAP_WIDTH);
      break;
    /*}}}*/
  }
  return cptr;
}
/*}}}*/
/*{{{  CELL *move_muncher(mptr)*/
extern CELL *move_muncher
FUNCARG((mptr),
	MONSTER   *mptr /* the object to move */
)
/*
 * moves and munches the board for an object which can munch
 * apple checking is performed here too
 * (ie the man, or a munch monster)
 * the board array is updated as required
 * returns a pointer to the new cell, if we have arrived elsewhere
 * or NULL if we stayed on the same cell
 */
{
  unsigned  broke;
  CELL      *nptr;
  CELL      *cherry;
  CELL      *cptr;
  COORD     pixel;
  COORD     cell;
  SPRITE    *sptr;
  int       knocked;

  assert(!mptr->stop && !mptr->pause);
  broke = 0;
  nptr = NULL;
  cherry = NULL;
  update.set = 0;
  cell.x = mptr->cell.x;
  cell.y = mptr->cell.y;
  cptr = BOARDCELL(cell.x, cell.y);
  pixel.x = PIXELX(cell.x, 0);
  pixel.y = PIXELY(cell.y, 0);
  knocked = 0;
  if(!apple_stop(mptr, cptr))
    {
      switch(mptr->dir)
      {
	/*{{{  case 0: (up)*/
	case 0:
	  /*
	   * if the depth upwards is less than the future depth,
	   * then we have to do some munching
	   */
	  mptr->offset.y -= VEL_Y;
	  mptr->pixel.y = pixel.y + mptr->offset.y;
	  if(cptr[0].depths[0] > mptr->offset.y)
	    /*{{{  munch*/
	    {
	      cptr[0].depths[0] = mptr->offset.y;
	      sptr = &sprites[SPRITE_CENTER_BASE];
	      munch_back(0, 0, CELL_WIDTH, VEL_Y + INTERNAL_HEIGHT,
		  pixel.x, pixel.y + cptr->depths[0], sptr);
	      if(cptr->sprite && mptr->offset.y + VEL_Y + INTERNAL_HEIGHT > 0)
		back_sprite(cptr->sprite, 0, pixel.x, pixel.y);
	      if(mptr->offset.y < -GAP_HEIGHT)
		cherry = cptr - CELL_STRIDE;
	      if(INRANGE(mptr->offset.y,
		  1 - (GAP_HEIGHT + VEL_Y), 1 - GAP_HEIGHT) &&
		  cptr[-CELL_STRIDE].visit)
		{
		  cptr[0].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
		  cptr[-CELL_STRIDE].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
		  munch_edge_above(cptr, pixel.x, pixel.y, 1);
		  broke = 1;
		}
	      if(INRANGE(mptr->offset.y,
		  1 - (INTERNAL_HEIGHT + EXTERNAL_HEIGHT) - VEL_Y,
		  1 - (INTERNAL_HEIGHT + EXTERNAL_HEIGHT)))
		{
		  sptr = &sprites[SPRITE_EDGE_BASE + 1];
		  /*{{{  round top left corner?*/
		  if(cptr[0].depths[2] <= -(INTERNAL_WIDTH + EXTERNAL_WIDTH))
		    munch_back((EDGE_WIDTH >> 1) - (CELL_WIDTH >> 1) -
			EXTERNAL_WIDTH, 3 * GAP_HEIGHT - EXTERNAL_HEIGHT,
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x - EXTERNAL_WIDTH, pixel.y - EXTERNAL_HEIGHT,
			sptr);
		  /*}}}*/
		  /*{{{  round top right corner?*/
		  if(cptr[0].depths[3] >= (INTERNAL_WIDTH + EXTERNAL_WIDTH))
		    munch_back((EDGE_WIDTH >> 1) + (CELL_WIDTH >> 1),
			3 * GAP_HEIGHT - EXTERNAL_HEIGHT,
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x + CELL_WIDTH,
			pixel.y - EXTERNAL_HEIGHT, sptr);
		  /*}}}*/
		}
	      /*{{{  knocked through?*/
	      /*
	       * have we bumped into any of the following ?
	       * path from 2 above me
	       * path from above left
	       * path from above right
	       */
	      if(!cptr[-CELL_STRIDE].visit &&
		  ((cptr[-CELL_STRIDE * 2].depths[1] - cptr[0].depths[0] >=
		  CELL_HEIGHT + GAP_HEIGHT * 2) ||
		  (cptr[-CELL_STRIDE - 1].depths[3] >= GAP_WIDTH &&
		    cptr[-CELL_STRIDE - 1].depths[3] - cptr[0].depths[0] >=
		    KNOCK_THROUGH) ||
		  (cptr[-CELL_STRIDE + 1].depths[2] <= -GAP_WIDTH &&
		    cptr[-CELL_STRIDE + 1].depths[2] + cptr[0].depths[0] <=
		    -KNOCK_THROUGH)))
		{
		  knocked = -CELL_STRIDE;
		  cell.y -= 1;
		}
	      /*}}}*/
	      if(cptr->depths[0] == -(CELL_HEIGHT + GAP_HEIGHT))
		{
		  cptr[-CELL_STRIDE].visit = 1;
		  cptr[-CELL_STRIDE].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
		  broke = 1;
		}
	    }
	    /*}}}*/
	  pixel.y -= CELL_HEIGHT + GAP_HEIGHT;
	  if(mptr->offset.y == -(CELL_HEIGHT + GAP_HEIGHT))
	    {
	      mptr->offset.y = 0;
	      mptr->cell.y--;
	      nptr = cptr - CELL_STRIDE;
	    }
	  break;
	/*}}}*/
	/*{{{  case 1: (down)*/
	case 1:
	{
	  mptr->offset.y += VEL_Y;
	  mptr->pixel.y = pixel.y + mptr->offset.y;
	  if(cptr->depths[1] < mptr->offset.y)
	    /*{{{  munch*/
	    {
	      cptr->depths[1] = mptr->offset.y;
	      sptr = &sprites[SPRITE_CENTER_BASE];
	      munch_back(0, CELL_HEIGHT - VEL_Y - INTERNAL_HEIGHT,
		  CELL_WIDTH, VEL_Y + INTERNAL_HEIGHT,
		  pixel.x, pixel.y + cptr[0].depths[1] +
		  CELL_HEIGHT - VEL_Y - INTERNAL_HEIGHT, sptr);
	      if(cptr->sprite && mptr->offset.y < VEL_Y + INTERNAL_HEIGHT)
		back_sprite(cptr->sprite, 0, pixel.x, pixel.y);
	      if(mptr->offset.y > GAP_HEIGHT)
		cherry = cptr + CELL_STRIDE;
	      if(INRANGE(mptr->offset.y, GAP_HEIGHT, GAP_HEIGHT + VEL_Y) &&
		  cptr[CELL_STRIDE].visit)
		{
		  cptr[0].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
		  cptr[CELL_STRIDE].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
		  munch_edge_below(cptr, pixel.x, pixel.y, 0, 1);
		  broke = 1;
		}
	      if(INRANGE(mptr->offset.y, INTERNAL_HEIGHT + EXTERNAL_HEIGHT,
		  INTERNAL_HEIGHT + EXTERNAL_HEIGHT + VEL_Y))
		{
		  sptr = &sprites[SPRITE_EDGE_BASE + 1];
		  /*{{{  round bottom left corner?*/
		  if(cptr[0].depths[2] <= -(INTERNAL_WIDTH + EXTERNAL_WIDTH))
		    munch_back((EDGE_WIDTH >> 1) - (CELL_WIDTH >> 1) -
			EXTERNAL_WIDTH, GAP_HEIGHT,
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x - EXTERNAL_WIDTH, pixel.y + CELL_HEIGHT,
			sptr);
		  /*}}}*/
		  /*{{{  round bottom right corner?*/
		  if(cptr[0].depths[3] >= (INTERNAL_WIDTH + EXTERNAL_WIDTH))
		    munch_back((EDGE_WIDTH >> 1) + (CELL_WIDTH >> 1),
			GAP_HEIGHT,
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x + CELL_WIDTH, pixel.y + CELL_HEIGHT, sptr);
		  /*}}}*/
		}
	      /*{{{  knocked through?*/
	      /*
	       * have we bumped into any of the following ?
	       * path from 2 below me
	       * path from below left
	       * path from below right
	       */
	      if(!cptr[CELL_STRIDE].visit &&
		  ((cptr[0].depths[1] - cptr[CELL_STRIDE * 2].depths[0] >=
		  CELL_HEIGHT + GAP_HEIGHT * 2) ||
		  (cptr[CELL_STRIDE - 1].depths[3] >= GAP_WIDTH &&
		    cptr[CELL_STRIDE - 1].depths[3] + cptr[0].depths[1] >=
		    KNOCK_THROUGH) ||
		  (cptr[CELL_STRIDE + 1].depths[2] <= -GAP_WIDTH &&
		    cptr[0].depths[1] - cptr[CELL_STRIDE + 1].depths[2] >=
		    KNOCK_THROUGH)))
		{
		  knocked = CELL_STRIDE;
		  cell.y += 1;
		}
	      /*}}}*/
	      if(cptr->depths[1] == (CELL_HEIGHT + GAP_HEIGHT))
		{
		  cptr[CELL_STRIDE].visit = 1;
		  cptr[CELL_STRIDE].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
		  broke = 1;
		}
	    }
	    /*}}}*/
	  pixel.y += CELL_HEIGHT + GAP_HEIGHT;
	  if(mptr->offset.y == (CELL_HEIGHT + GAP_HEIGHT))
	    {
	      mptr->offset.y = 0;
	      mptr->cell.y++;
	      nptr = cptr + CELL_STRIDE;
	    }
	  break;
	}
	/*}}}*/
	/*{{{  case 2: (left)*/
	case 2:
	{
	  mptr->offset.x -= VEL_X;
	  mptr->pixel.x = pixel.x + mptr->offset.x;
	  if(cptr[0].depths[2] > mptr->offset.x)
	    /*{{{  munch*/
	    {
	      cptr[0].depths[2] = mptr->offset.x;
	      sptr = &sprites[SPRITE_CENTER_BASE];
	      munch_back(0, 0, VEL_X + INTERNAL_WIDTH, CELL_HEIGHT,
		  pixel.x + cptr[0].depths[2], pixel.y, sptr);
	      if(cptr->sprite && mptr->offset.x + VEL_X + INTERNAL_WIDTH > 0)
		back_sprite(cptr->sprite, 0, pixel.x, pixel.y);
	      if(mptr->offset.x < -GAP_WIDTH)
		cherry = cptr - 1;
	      if(INRANGE(mptr->offset.x, 1 - (GAP_WIDTH + VEL_X),
		  1 - GAP_WIDTH) && cptr[-1].visit)
		{
		  cptr[0].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
		  cptr[-1].depths[3] = CELL_WIDTH + GAP_WIDTH;
		  munch_edge_left(cptr, pixel.x, pixel.y, 1);
		  broke = 1;
		}
	      if(INRANGE(mptr->offset.x,
		  1 - (INTERNAL_WIDTH + EXTERNAL_WIDTH) - VEL_X,
		  1 - (INTERNAL_WIDTH + EXTERNAL_WIDTH)))
		{
		  sptr = &sprites[SPRITE_EDGE_BASE + 0];
		  /*{{{  round left top corner?*/
		  if(cptr[0].depths[0] <=
		      -(INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
		    munch_back(3 * GAP_WIDTH - EXTERNAL_WIDTH,
			(EDGE_HEIGHT >> 1) - (CELL_HEIGHT >> 1) -
			EXTERNAL_HEIGHT, EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x - EXTERNAL_WIDTH, pixel.y - EXTERNAL_HEIGHT,
			sptr);
		  /*}}}*/
		  /*{{{  round left bottom corner?*/
		  if(cptr[0].depths[1] >= (INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
		    munch_back(3 * GAP_HEIGHT - EXTERNAL_WIDTH,
			(EDGE_HEIGHT >> 1) + (CELL_HEIGHT >> 1),
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x - EXTERNAL_WIDTH,
			pixel.y + CELL_HEIGHT, sptr);
		  /*}}}*/
		}
	      /*{{{  knocked through?*/
	      /*
	       * have we bumped into any of the following ?
	       * path from 2 left me
	       * path from left above
	       * path from left below
	       */
	      if(!cptr[-1].visit &&
		  ((cptr[-2].depths[3] - cptr[0].depths[2] >=
		    CELL_WIDTH + GAP_WIDTH * 2) ||
		  (cptr[-CELL_STRIDE - 1].depths[1] >= GAP_HEIGHT &&
		    cptr[-CELL_STRIDE - 1].depths[1] - cptr[0].depths[2] >=
		    KNOCK_THROUGH) ||
		  (cptr[CELL_STRIDE - 1].depths[0] <= -GAP_HEIGHT &&
		    cptr[CELL_STRIDE - 1].depths[0] + cptr[0].depths[2] <=
		    -KNOCK_THROUGH)))
		{
		  knocked = -1;
		  cell.x -= 1;
		}
	      /*}}}*/
	      if(cptr->depths[2] == -(CELL_WIDTH + GAP_WIDTH))
		{
		  cptr[-1].visit = 1;
		  cptr[-1].depths[3] = CELL_WIDTH + GAP_WIDTH;
		  broke = 1;
		}
	    }
	    /*}}}*/
	  pixel.x -= CELL_WIDTH + GAP_WIDTH;
	  if(mptr->offset.x == -(CELL_WIDTH + GAP_WIDTH))
	    {
	      mptr->offset.x = 0;
	      mptr->cell.x--;
	      nptr = cptr - 1;
	    }
	  break;
	}
	/*}}}*/
	/*{{{  case 3: (right)*/
	case 3:
	{
	  mptr->offset.x += VEL_X;
	  mptr->pixel.x = pixel.x + mptr->offset.x;
	  if(cptr->depths[3] < mptr->offset.x)
	    /*{{{  munch*/
	    {
	      cptr->depths[3] = mptr->offset.x;
	      sptr = &sprites[SPRITE_CENTER_BASE];
	      munch_back(CELL_WIDTH - VEL_X - INTERNAL_WIDTH, 0,
		  VEL_X + INTERNAL_WIDTH, CELL_HEIGHT,
		  pixel.x + cptr->depths[3] +
		  CELL_WIDTH - VEL_X - INTERNAL_WIDTH, pixel.y, sptr);
	      if(cptr->sprite && mptr->offset.x < VEL_X + INTERNAL_WIDTH)
		back_sprite(cptr->sprite, 0, pixel.x, pixel.y);
	      if(mptr->offset.x > GAP_WIDTH)
		cherry = cptr + 1;
	      if(INRANGE(mptr->offset.x, GAP_WIDTH, GAP_WIDTH + VEL_X) &&
		  cptr[1].visit)
		{
		  cptr[0].depths[3] = CELL_WIDTH + GAP_WIDTH;
		  cptr[1].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
		  munch_edge_right(cptr, pixel.x, pixel.y, 1);
		  broke = 1;
		}
	      if(INRANGE(mptr->offset.x, INTERNAL_WIDTH + EXTERNAL_WIDTH,
		  INTERNAL_WIDTH + EXTERNAL_WIDTH + VEL_X))
		{
		  sptr = &sprites[SPRITE_EDGE_BASE + 0];
		  /*{{{  round right top corner?*/
		  if(cptr[0].depths[0] <=
		      -(INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
		    munch_back(GAP_HEIGHT, (EDGE_HEIGHT >> 1) -
			(CELL_HEIGHT >> 1) - EXTERNAL_HEIGHT,
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x + CELL_WIDTH, pixel.y - EXTERNAL_HEIGHT,
			sptr);
		  /*}}}*/
		  /*{{{  round right bottom corner?*/
		  if(cptr[0].depths[1] >= (INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
		    munch_back(GAP_WIDTH,
			(EDGE_HEIGHT >> 1) + (CELL_WIDTH >> 1),
			EXTERNAL_WIDTH, EXTERNAL_HEIGHT,
			pixel.x + CELL_WIDTH, pixel.y + CELL_HEIGHT, sptr);
		  /*}}}*/
		}
	      /*{{{  knocked through?*/
	      /*
	       * have we bumped into any of the following ?
	       * path from 2 right me
	       * path from right above
	       * path from right below
	       */
	      if(!cptr[1].visit && ((cptr[0].depths[3] - cptr[2].depths[2] >=
		  CELL_WIDTH + GAP_WIDTH * 2) ||
		  (cptr[-CELL_STRIDE + 1].depths[1] >= GAP_HEIGHT &&
		    cptr[-CELL_STRIDE + 1].depths[1] + cptr[0].depths[3] >=
		    KNOCK_THROUGH) ||
		  (cptr[CELL_STRIDE + 1].depths[0] <= -GAP_HEIGHT &&
		    cptr[0].depths[3] - cptr[CELL_STRIDE + 1].depths[0] >=
		    KNOCK_THROUGH)))
		{
		  knocked = 1;
		  cell.x += 1;
		}
	      /*}}}*/
	      if(cptr->depths[3] == (CELL_WIDTH + GAP_WIDTH))
		{
		  cptr[1].visit = 1;
		  cptr[1].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
		  broke = 1;
		}
	    }
	    /*}}}*/
	  pixel.x += CELL_WIDTH + GAP_WIDTH;
	  if(mptr->offset.x == (CELL_WIDTH + GAP_WIDTH))
	    {
	      mptr->offset.x = 0;
	      mptr->cell.x++;
	      nptr = cptr + 1;
	    }
	  break;
	}
	/*}}}*/
      }
    }
  /*{{{  knocked through?*/
  /*
   * if we knocked through to an adjoining cell
   * we clear out the specified cell and check all the corners
   * note, cell has already been altered correctly
   * we must also check to se if this launches an apple
   */
  if(knocked)
    {
      broke = 1;
      assert(!cherry || cherry == cptr + knocked);
      cherry = cptr += knocked;
      connect_hole(cptr);
      munch_hole(cptr, pixel.x, pixel.y);
    }
  /*}}}*/
  /*{{{  redraw or blank prize?*/
  if(nptr && nptr->sprite)
    {
      assert(!cherry || cherry == nptr);
      if((mptr->type != 4 && nptr->sprite >= SPRITE_PRIZE_BASE) ||
	  (nptr->sprite != SPRITE_CHERRY && nptr->sprite < SPRITE_PRIZE_BASE))
	back_sprite(nptr->sprite, 0, pixel.x, pixel.y);
      else
	munch_back(0, 0, CELL_WIDTH, CELL_HEIGHT,
	    pixel.x, pixel.y, &sprites[SPRITE_CENTER_BASE +
	    (nptr->sprite != SPRITE_CHERRY)]);
    }
  else if(cherry && cherry->sprite)
    back_sprite(cherry->sprite, 0, pixel.x, pixel.y);
  /*}}}*/
  if(update.set)
    {
      bounding_box(mptr->pixel.x, mptr->pixel.y, CELL_WIDTH, CELL_HEIGHT);
      bounding_box(mptr->old_pixel.x, mptr->old_pixel.y,
	  CELL_WIDTH, CELL_HEIGHT);
      add_background(update.tl.x, update.tl.y,
	  (unsigned)(update.br.x - update.tl.x),
	  (unsigned)(update.br.y - update.tl.y));
      mptr->back = 1;
    }
  if(broke)
    global.broken = 1;
  /* check we haven't wandered off the board */
  assert(!nptr || nptr->visit);
  assert(INRANGE(mptr->cell.y, 0, CELLS_DOWN));
  assert(INRANGE(mptr->cell.x, 0, CELLS_ACROSS));
  assert(mptr->cell.x || mptr->offset.x >= 0);
  assert(mptr->cell.y || mptr->offset.y >= 0);
  assert(mptr->cell.x < CELLS_ACROSS - 1 || mptr->offset.x <= 0);
  assert(mptr->cell.y < CELLS_DOWN - 1 || mptr->offset.y <= 0);
  assert(!mptr->offset.x || !mptr->offset.y);
  return nptr;
}
/*}}}*/
/*{{{  void munch_back(sx, sy, width, height, dx, dy, sprite)*/
static VOIDFUNC munch_back
FUNCARG((sx, sy, width, height, dx, dy, sprite),
	int       sx
ARGSEP  int       sy
ARGSEP  unsigned  width
ARGSEP  unsigned  height
ARGSEP  int       dx
ARGSEP  int       dy
ARGSEP  SPRITE    *sprite
)
/*
 * munches the background image with the specified sprite
 */
{
  if(display.background != COLOUR_ONE)
    XCopyArea(display.display, sprite->mask, display.back, GCN(GC_MASK),
	sx, sy, width, height, dx, dy);
  if(display.background != COLOUR_ZERO)
    XCopyArea(display.display, sprite->image, display.back, GCN(GC_OR),
	sx, sy, width, height, dx, dy);
  bounding_box(dx, dy, width, height);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void munch_hole(cptr, x, y)*/
extern VOIDFUNC munch_hole
FUNCARG((cptr, x, y),
	CELL      *cptr
ARGSEP  int       x
ARGSEP  int       y
)
/*
 * cut out the background for a whole cell, and
 * deal with connections to the adjoining cells
 * don't forget to replot the cherry
 */
{
  /*{{{  cut out the center*/
  {
    SPRITE    *sptr;
    
    sptr = &sprites[SPRITE_CENTER_BASE];
    munch_back(0, 0,
	CELL_WIDTH >> 1, CELL_HEIGHT >> 1,
	x, y,
	&sptr[!!(cptr[0].depths[0] || cptr[0].depths[2])]);
    munch_back(CELL_WIDTH >> 1, 0,
	CELL_WIDTH >> 1, CELL_HEIGHT >> 1,
	x + (CELL_WIDTH >> 1), y,
	&sptr[!!(cptr[0].depths[0] || cptr[0].depths[3])]);
    munch_back(CELL_WIDTH >> 1, CELL_HEIGHT >> 1,
	CELL_WIDTH >> 1, CELL_HEIGHT >> 1,
	x + (CELL_WIDTH >> 1), y + (CELL_HEIGHT >> 1),
	&sptr[!!(cptr[0].depths[1] || cptr[0].depths[3])]);
    munch_back(0, CELL_HEIGHT >> 1,
	CELL_WIDTH >> 1, CELL_HEIGHT >> 1,
	x, y + (CELL_HEIGHT >> 1),
	&sptr[!!(cptr[0].depths[1] || cptr[0].depths[2])]);
  }
  /*}}}*/
  if(cptr[0].depths[0] <= -GAP_HEIGHT && cptr[-CELL_STRIDE].visit)
    munch_edge_above(cptr, x, y, 0);
  if(cptr[0].depths[1] >= GAP_HEIGHT && cptr[CELL_STRIDE].visit)
    munch_edge_below(cptr, x, y, 0, 0);
  if(cptr[0].depths[2] <= -GAP_WIDTH && cptr[-1].visit)
    munch_edge_left(cptr, x, y, 0);
  if(cptr[0].depths[3] >= GAP_WIDTH && cptr[1].visit)
    munch_edge_right(cptr, x, y, 0);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void munch_edge_above(cptr, x, y, flag)*/
static VOIDFUNC munch_edge_above
FUNCARG((cptr, x, y, flag),
	CELL      *cptr
ARGSEP  int       x
ARGSEP  int       y
ARGSEP  unsigned  flag
)
/*
 * munches an edge above the cell, replots the cherry above if needed
 */
{
  unsigned  check;
    /* remember kiddies, never leave signed cheques lying about */
  unsigned  type;
  SPRITE    *sptr;

  sptr = &sprites[SPRITE_EDGE_BASE + 1];
  check = 0;
  type = 0;
  if(cptr[0].depths[2] <= -(INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type = 2 * GAP_HEIGHT;
  if(cptr[-CELL_STRIDE].depths[2] <= -(INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type += GAP_HEIGHT;
  if(type == 3 * GAP_HEIGHT && (cptr[-CELL_STRIDE - 1].depths[1] -
      cptr[-1].depths[0] >= GAP_HEIGHT))
    type = 4 * GAP_HEIGHT;
  munch_back(0, (int)type, EDGE_WIDTH >> 1, GAP_HEIGHT,
      x + (CELL_WIDTH >> 1) - (EDGE_WIDTH >> 1), y - GAP_HEIGHT, sptr);
  if(flag && !cptr[-CELL_STRIDE].depths[2])
    {
      check = 1;
      munch_back(0, 0, EDGE_WIDTH >> 1, GAP_HEIGHT,
	  x + (CELL_WIDTH >> 1) - (EDGE_WIDTH >> 1),
	  y - GAP_HEIGHT * 2, sptr);
    }
  type = 0;
  if(cptr[0].depths[3] >= (INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type = 2 * GAP_HEIGHT;
  if(cptr[-CELL_STRIDE].depths[3] >= (INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type += GAP_HEIGHT;
  if(type == 3 * GAP_HEIGHT && (cptr[-CELL_STRIDE + 1].depths[1] -
      cptr[1].depths[0] >= GAP_HEIGHT))
    type = 4 * GAP_HEIGHT;
  munch_back(EDGE_WIDTH >> 1, (int)type, EDGE_WIDTH >> 1, GAP_HEIGHT,
      x + (CELL_WIDTH >> 1), y - GAP_HEIGHT, sptr);
  if(flag && !cptr[-CELL_STRIDE].depths[3])
    {
      check = 1;
      munch_back(EDGE_WIDTH >> 1, 0, EDGE_WIDTH >> 1, GAP_HEIGHT,
	  x + (CELL_WIDTH >> 1), y - GAP_HEIGHT * 2, sptr);
    }
  if(check && cptr[-CELL_STRIDE].sprite)
      back_sprite(cptr[-CELL_STRIDE].sprite, 0,
	  x, y - CELL_HEIGHT - GAP_HEIGHT);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void munch_edge_below(cptr, x, y, offset, flag)*/
static VOIDFUNC munch_edge_below
FUNCARG((cptr, x, y, offset, flag),
	CELL      *cptr
ARGSEP  int       x
ARGSEP  int       y
ARGSEP  int       offset
ARGSEP  unsigned  flag
)
/*
 * munches an edge below the cell, replots the cherry above if needed
 * the edge can be offset, in the case of non-aligned apples
 */
{
  unsigned  check;
  unsigned  type;
  SPRITE    *sptr;

  sptr = &sprites[SPRITE_EDGE_BASE + 1];
  check = 0;
  type = 0;
  if(cptr[0].depths[2] - offset <= -(INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type = GAP_HEIGHT;
  if(cptr[CELL_STRIDE].depths[2] - offset <=
      -(INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type += 2 * GAP_HEIGHT;
  if(!offset && type == 3 * GAP_HEIGHT && (cptr[-1].depths[1] -
      cptr[CELL_STRIDE - 1].depths[0] >= GAP_HEIGHT))
    type = 4 * GAP_HEIGHT;
  munch_back(0, (int)type, EDGE_WIDTH >> 1, GAP_HEIGHT,
      x + (CELL_WIDTH >> 1) - (EDGE_WIDTH >> 1), y + CELL_HEIGHT, sptr);
  if(flag && !cptr[CELL_STRIDE].depths[2])
    {
      check = 1;
      munch_back(0, 0, EDGE_WIDTH >> 1, GAP_HEIGHT,
	  x + (CELL_WIDTH >> 1) - (EDGE_WIDTH >> 1),
	  y + CELL_HEIGHT + GAP_HEIGHT, sptr);
    }
  type = 0;
  if(cptr[0].depths[3] - offset >= (INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type = GAP_HEIGHT;
  if(cptr[CELL_STRIDE].depths[3] - offset >=
      (INTERNAL_WIDTH + EXTERNAL_WIDTH))
    type += 2 * GAP_HEIGHT;
  if(!offset && type == 3 * GAP_HEIGHT &&
      (cptr[1].depths[1] -
      cptr[CELL_STRIDE + 1].depths[0] >= GAP_HEIGHT))
    type = 4 * GAP_HEIGHT;
  munch_back(EDGE_WIDTH >> 1, (int)type, EDGE_WIDTH >> 1, GAP_HEIGHT,
      x + (CELL_WIDTH >> 1), y + CELL_HEIGHT, sptr);
  if(flag && !cptr[CELL_STRIDE].depths[3])
    {
      check = 1;
      munch_back(EDGE_WIDTH >> 1, 0, EDGE_WIDTH >> 1, GAP_HEIGHT,
	  x + (CELL_WIDTH >> 1), y + CELL_HEIGHT + GAP_HEIGHT, sptr);
    }
  if(check && cptr[CELL_STRIDE].sprite)
      back_sprite(cptr[CELL_STRIDE].sprite, 0,
	  x, y + CELL_HEIGHT + GAP_HEIGHT);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void munch_edge_left(cptr, x, y, flag)*/
static VOIDFUNC munch_edge_left
FUNCARG((cptr, x, y, flag),
	CELL      *cptr
ARGSEP  int       x
ARGSEP  int       y
ARGSEP  unsigned  flag
)
/*
 * munches an edge left of the cell, replots the cherry above if needed
 */
{
  unsigned  check;
  unsigned  type;
  SPRITE    *sptr;

  sptr = &sprites[SPRITE_EDGE_BASE + 0];
  check = 0;
  type = 0;
  if(cptr[0].depths[0] <= -(INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type = 2 * GAP_WIDTH;
  if(cptr[-1].depths[0] <= -(INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type += GAP_WIDTH;
  if(type == 3 * GAP_WIDTH && (cptr[-CELL_STRIDE - 1].depths[3] -
      cptr[-CELL_STRIDE].depths[2] >= GAP_WIDTH))
    type = 4 * GAP_WIDTH;
  munch_back((int)type, 0, GAP_HEIGHT, EDGE_HEIGHT >> 1,
      x - GAP_WIDTH, y + (CELL_HEIGHT >> 1) - (EDGE_HEIGHT >> 1), sptr);
  if(flag && !cptr[-1].depths[0])
    {
      check = 1;
      munch_back(0, 0, GAP_WIDTH, EDGE_HEIGHT >> 1, x - GAP_WIDTH * 2,
	  y + (CELL_HEIGHT >> 1) - (EDGE_HEIGHT >> 1), sptr);
    }
  type = 0;
  if(cptr[0].depths[1] >= (INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type = 2 * GAP_WIDTH;
  if(cptr[-1].depths[1] >= (INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type += GAP_WIDTH;
  if(type == 3 * GAP_WIDTH && (cptr[CELL_STRIDE - 1].depths[3] -
      cptr[CELL_STRIDE].depths[2] >= GAP_WIDTH))
    type = 4 * GAP_WIDTH;
  munch_back((int)type, EDGE_HEIGHT >> 1, GAP_WIDTH, EDGE_HEIGHT >> 1,
      x - GAP_WIDTH, y + (CELL_HEIGHT >> 1), sptr);
  if(flag && !cptr[-1].depths[1])
    {
      check = 1;
      munch_back(0, EDGE_HEIGHT >> 1, GAP_WIDTH, EDGE_HEIGHT >> 1,
	  x - GAP_WIDTH * 2, y + (CELL_HEIGHT >> 1), sptr);
    }
  if(check && cptr[-1].sprite)
      back_sprite(cptr[-1].sprite, 0, x - CELL_WIDTH - GAP_WIDTH, y);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void munch_edge_right(cptr, x, y, flag)*/
static VOIDFUNC munch_edge_right
FUNCARG((cptr, x, y, flag),
	CELL      *cptr
ARGSEP  int       x
ARGSEP  int       y
ARGSEP  unsigned  flag
)
/*
 * munches an edge right of the cell, replots the cherry above if needed
 */
{
  unsigned  check;
  unsigned  type;
  SPRITE    *sptr;

  sptr = &sprites[SPRITE_EDGE_BASE + 0];
  check = 0;
  type = 0;
  if(cptr[0].depths[0] <= -(INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type = GAP_WIDTH;
  if(cptr[1].depths[0] <= -(INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type += 2 * GAP_WIDTH;
  if(type == 3 * GAP_WIDTH &&
      (cptr[-CELL_STRIDE].depths[3] -
      cptr[-CELL_STRIDE + 1].depths[2] >= GAP_WIDTH))
    type = 4 * GAP_WIDTH;
  munch_back((int)type, 0, GAP_WIDTH, EDGE_HEIGHT >> 1,
      x + CELL_WIDTH, y + (CELL_HEIGHT >> 1) - (EDGE_HEIGHT >> 1), sptr);
  if(flag && !cptr[1].depths[0])
    {
      check = 1;
      munch_back(0, 0, GAP_WIDTH, EDGE_HEIGHT >> 1,
	  x + CELL_WIDTH + GAP_WIDTH,
	  y + (CELL_HEIGHT >> 1) - (EDGE_HEIGHT >> 1), sptr);
    }
  type = 0;
  if(cptr[0].depths[1] >= (INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type = GAP_WIDTH;
  if(cptr[1].depths[1] >= (INTERNAL_HEIGHT + EXTERNAL_HEIGHT))
    type += 2 * GAP_WIDTH;
  if(type == 3 * GAP_WIDTH &&
      (cptr[CELL_STRIDE].depths[3] -
      cptr[CELL_STRIDE + 1].depths[2] >= GAP_WIDTH))
    type = 4 * GAP_WIDTH;
  munch_back((int)type, EDGE_HEIGHT >> 1, GAP_WIDTH, EDGE_HEIGHT >> 1,
      x + CELL_WIDTH, y + (CELL_HEIGHT >> 1), sptr);
  if(flag && !cptr[1].depths[1])
    {
      check = 1;
      munch_back(0, EDGE_HEIGHT >> 1, GAP_WIDTH, EDGE_HEIGHT >> 1,
	  x + CELL_WIDTH + GAP_WIDTH, y + (CELL_HEIGHT >> 1), sptr);
    }
  if(check && cptr[1].sprite)
      back_sprite(cptr[1].sprite, 0, x + CELL_WIDTH + GAP_WIDTH, y);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void new_face(mptr)*/
extern VOIDFUNC new_face
FUNCARG((mptr),
	MONSTER *mptr
)
/*
 * calculates the required face for the direction,
 * given the old face
 */
{
  int       dir;
  int       face;

  if(mptr->push)
    dir = mptr->dir ^ 1;
  else
    dir = mptr->dir;
  face = mptr->face < 6 ? mptr->face : 2 + (mptr->face & 1);
  if(mptr->squished)
    mptr->face = 16 + (face > 2);
  else if(dir & 2)
    mptr->face = dir;
  else if(face == 2)
    mptr->face = dir;
  else if(face == 3)
    mptr->face = dir + 4;
  else if((dir ^ face) & 1)
    mptr->face ^= 5;
  if(mptr->face & 2 && mptr->pushing)
    mptr->face += 4;
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned valid_directions(mptr, cptr)*/
extern unsigned valid_directions
FUNCARG((mptr, cptr),
	MONSTER   *mptr
ARGSEP  CELL      *cptr
)
/*
 * sets the valid and nearer direction bits
 * these are nr, nl, nd, nu, r, l, d, u
 */
{
  unsigned  answer;
  int       offset;

  answer = 0;
  assert(cptr->visit);
  if((offset = mptr->offset.y) != 0)
    /*{{{  up down only*/
    {
      if(!mptr->cell.y && offset < 0)
	answer |= 2;
      else if(mptr->offset.x)
	answer |= 0x3;
      else
	{
	  if(offset > cptr->depths[0])
	    answer |= 0x1;
	  if(offset < cptr->depths[1])
	    answer |= 0x2;
	}
      if(cptr == monster.player)
	answer |= monster.list[0].offset.y > offset ? 0x20 : 0x10;
      else if(offset < 0 ? cptr[0].distance < cptr[-CELL_STRIDE].distance :
	  cptr[0].distance >= cptr[CELL_STRIDE].distance)
	answer |= 0x20;
      else
	answer |= 0x10;
    }
    /*}}}*/
  else if((offset = mptr->offset.x) != 0)
    /*{{{  left right only*/
    {
      if(offset > cptr->depths[2])
	answer |= 0x4;
      if(offset < cptr->depths[3])
	answer |= 0x8;
      if(cptr == monster.player)
	answer |= monster.list[0].offset.x > offset ? 0x80 : 0x40;
      else if(offset < 0 ? cptr[0].distance < cptr[-1].distance :
	  cptr[0].distance >= cptr[1].distance)
	answer |= 0x80;
      else
	answer |= 0x40;
      if(offset == cptr->holes[0] || offset == cptr->holes[1])
	{
	  answer |= 0x1;
	  if(cptr[0].distance > cptr[-CELL_STRIDE].distance)
	    answer |= 0x10;
	}
      if(offset == cptr->holes[2] || offset == cptr->holes[3])
	{
	  answer |= 0x2;
	  if(cptr[0].distance > cptr[CELL_STRIDE].distance)
	    answer |= 0x20;
	}
    }
    /*}}}*/
  else
    /*{{{  at intersection*/
    {
      int       distance;
	  
      if(cptr->depths[0])
	answer |= 0x1;
      if(cptr->depths[1])
	answer |= 0x2;
      if(cptr->depths[2])
	answer |= 0x4;
      if(cptr->depths[3])
	answer |= 0x8;
      distance = cptr->distance;
      if(cptr == monster.player)
	{
	  if(monster.list[0].offset.x < 0)
	    answer |= 0x40;
	  else if(monster.list[0].offset.x > 0)
	    answer |= 0x80;
	  else if(monster.list[0].offset.y < 0)
	    answer |= 0x10;
	  else if(monster.list[0].offset.y > 0)
	    answer |= 0x20;
	}
      else
	{
	  if(distance > cptr[-CELL_STRIDE].distance)
	    answer |= 0x10;
	  if(distance > cptr[CELL_STRIDE].distance)
	    answer |= 0x20;
	  if(distance > cptr[-1].distance)
	    answer |= 0x40;
	  if(distance > cptr[1].distance)
	    answer |= 0x80;
	}
    }
    /*}}}*/
  answer &= answer << 4 | 0xF;
  assert(answer & 0xF);
  return answer;
}
/*}}}*/
