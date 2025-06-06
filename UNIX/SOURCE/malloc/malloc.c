/* Memory allocator `malloc'.
   Copyright 1989 Free Software Foundation
		  Written May 1989 by Mike Haertel.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   The author may be reached (Email) at the address mike@ai.mit.edu,
   or (US mail) as Mike Haertel c/o Free Software Foundation. */

#ifndef __ONEFILE
#include "ansidecl.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define _MALLOC_INTERNAL
#include "malloc.h"
#endif /* __ONEFILE */

/* How to really get more memory.  */
PTR EXFUN((*__morecore), (ptrdiff_t __size)) = __default_morecore;

/* Debugging hook for `malloc'.  */
PTR EXFUN((*__malloc_hook), (size_t __size));

/* Pointer to the base of the first block.  */
char *_heapbase;

/* Block information table.  */
malloc_info *_heapinfo;

/* Number of info entries.  */
static size_t heapsize;

/* Search index in the info table.  */
size_t _heapindex;

/* Limit of valid info table indices.  */
size_t _heaplimit;

/* Free lists for each fragment size.  */
struct list _fraghead[BLOCKLOG];

/* Instrumentation.  */
size_t _chunks_used;
size_t _bytes_used;
size_t _chunks_free;
size_t _bytes_free;

/* Are you experienced?  */
int __malloc_initialized;

/* Aligned allocation.  */
static PTR
DEFUN(align, (size), size_t size)
{
  PTR result;
  unsigned int adj;

  result = (*__morecore)(size);
  adj = (unsigned int) ((char *) result - (char *) NULL) % BLOCKSIZE;
  if (adj != 0)
    {
      adj = BLOCKSIZE - adj;
      (void) (*__morecore)(adj);
      result = (char *) result + adj;
    }
  return result;
}

/* Set everything up and remember that we have.  */
static int
DEFUN_VOID(initialize)
{
  heapsize = HEAP / BLOCKSIZE;
  _heapinfo = (malloc_info *) align(heapsize * sizeof(malloc_info));
  if (_heapinfo == NULL)
    return 0;
  memset(_heapinfo, 0, heapsize * sizeof(malloc_info));
  _heapinfo[0].free.size = 0;
  _heapinfo[0].free.next = _heapinfo[0].free.prev = 0;
  _heapindex = 0;
  _heapbase = (char *) _heapinfo;
  __malloc_initialized = 1;
  return 1;
}

/* Get neatly aligned memory, initializing or
   growing the heap info table as necessary. */
static PTR
DEFUN(morecore, (size), size_t size)
{
  PTR result;
  malloc_info *newinfo, *oldinfo;
  size_t newsize;

  result = align(size);
  if (result == NULL)
    return NULL;

  /* Check if we need to grow the info table.  */
  if (BLOCK((char *) result + size) > heapsize)
    {
      newsize = heapsize;
      while (BLOCK((char *) result + size) > newsize)
	newsize *= 2;
      newinfo = (malloc_info *) align(newsize * sizeof(malloc_info));
      if (newinfo == NULL)
	{
	  (*__morecore)(- size);
	  return NULL;
	}
      memset(newinfo, 0, newsize * sizeof(malloc_info));
      memcpy(newinfo, _heapinfo, heapsize * sizeof(malloc_info));
      oldinfo = _heapinfo;
      newinfo[BLOCK(oldinfo)].busy.type = 0;
      newinfo[BLOCK(oldinfo)].busy.info.size
	= BLOCKIFY(heapsize * sizeof(malloc_info));
      _heapinfo = newinfo;
      free(oldinfo);
      heapsize = newsize;
    }

  _heaplimit = BLOCK((char *) result + size);
  return result;
}

/* Allocate memory from the heap.  */
PTR
DEFUN(malloc, (size), size_t size)
{
  PTR result;
  size_t block, blocks, lastblocks, start;
  register size_t i;
  struct list *next;

  if (size == 0)
    return NULL;

  if (__malloc_hook != NULL)
    return (*__malloc_hook)(size);

  if (!__malloc_initialized)
    if (!initialize())
      return NULL;

  if (size < sizeof(struct list))
    size = sizeof(struct list);

  /* Determine the allocation policy based on the request size.  */
  if (size <= BLOCKSIZE / 2)
    {
      /* Small allocation to receive a fragment of a block.
	 Determine the logarithm to base two of the fragment size. */
      register size_t log = 1;
      --size;
      while ((size /= 2) != 0)
	++log;

      /* Look in the fragment lists for a
	 free fragment of the desired size. */
      next = _fraghead[log].next;
      if (next != NULL)
	{
	  /* There are free fragments of this size.
	     Pop a fragment out of the fragment list and return it.
	     Update the block's nfree and first counters. */
	  result = (PTR) next;
	  next->prev->next = next->next;
	  if (next->next != NULL)
	    next->next->prev = next->prev;
	  block = BLOCK(result);
	  if (--_heapinfo[block].busy.info.frag.nfree != 0)
	    _heapinfo[block].busy.info.frag.first = (unsigned int)
	      (((char *) next->next - (char *) NULL) % BLOCKSIZE) >> log;

	  /* Update the statistics.  */
	  ++_chunks_used;
	  _bytes_used += 1 << log;
	  --_chunks_free;
	  _bytes_free -= 1 << log;
	}
      else
	{
	  /* No free fragments of the desired size, so get a new block
	     and break it into fragments, returning the first.  */
	  result = malloc(BLOCKSIZE);
	  if (result == NULL)
	    return NULL;

	  /* Link all fragments but the first into the free list.  */
	  for (i = 1; i < BLOCKSIZE >> log; ++i)
	    {
	      next = (struct list *) ((char *) result + (i << log));
	      next->next = _fraghead[log].next;
	      next->prev = &_fraghead[log];
	      next->prev->next = next;
	      if (next->next != NULL)
		next->next->prev = next;
	    }

	  /* Initialize the nfree and first counters for this block.  */
	  block = BLOCK(result);
	  _heapinfo[block].busy.type = log;
	  _heapinfo[block].busy.info.frag.nfree = i - 1;
	  _heapinfo[block].busy.info.frag.first = i - 1;

	  _chunks_free += (BLOCKSIZE >> log) - 1;
	  _bytes_free += BLOCKSIZE - (1 << log);
	}
    }
  else
    {
      /* Large allocation to receive one or more blocks.
	 Search the free list in a circle starting at the last place visited.
	 If we loop completely around without finding a large enough
	 space we will have to get more memory from the system.  */
      blocks = BLOCKIFY(size);
      start = block = MALLOC_SEARCH_START;
      while (_heapinfo[block].free.size < blocks)
	{
	  block = _heapinfo[block].free.next;
	  if (block == start)
	    {
	      /* Need to get more from the system.  Check to see if
		 the new core will be contiguous with the final free
		 block; if so we don't need to get as much.  */
	      block = _heapinfo[0].free.prev;
	      lastblocks = _heapinfo[block].free.size;
	      if (_heaplimit != 0 && block + lastblocks == _heaplimit &&
		  (*__morecore)(0) == ADDRESS(block + lastblocks) &&
		  (morecore((blocks - lastblocks) * BLOCKSIZE)) != NULL)
		{
		  _heapinfo[block].free.size = blocks;
		  _bytes_free += (blocks - lastblocks) * BLOCKSIZE;
		  continue;
		}
	      result = morecore(blocks * BLOCKSIZE);
	      if (result == NULL)
		return NULL;
	      block = BLOCK(result);
	      _heapinfo[block].busy.type = 0;
	      _heapinfo[block].busy.info.size = blocks;
	      ++_chunks_used;
	      _bytes_used += blocks * BLOCKSIZE;
	      return result;
	    }
	}

      /* At this point we have found a suitable free list entry.
	 Figure out how to remove what we need from the list. */
      result = ADDRESS(block);
      if (_heapinfo[block].free.size > blocks)
	{
	  /* The block we found has a bit left over,
	     so relink the tail end back into the free list. */
	  _heapinfo[block + blocks].free.size
	    = _heapinfo[block].free.size - blocks;
	  _heapinfo[block + blocks].free.next
	    = _heapinfo[block].free.next;
	  _heapinfo[block + blocks].free.prev
	    = _heapinfo[block].free.prev;
	  _heapinfo[_heapinfo[block].free.prev].free.next
	    = _heapinfo[_heapinfo[block].free.next].free.prev
	      = _heapindex = block + blocks;
	}
      else
	{
	  /* The block exactly matches our requirements,
	     so just remove it from the list. */
	  _heapinfo[_heapinfo[block].free.next].free.prev
	    = _heapinfo[block].free.prev;
	  _heapinfo[_heapinfo[block].free.prev].free.next
	    = _heapindex = _heapinfo[block].free.next;
	  --_chunks_free;
	}

      _heapinfo[block].busy.type = 0;
      _heapinfo[block].busy.info.size = blocks;
      ++_chunks_used;
      _bytes_used += blocks * BLOCKSIZE;
      _bytes_free -= blocks * BLOCKSIZE;
    }

  return result;
}
