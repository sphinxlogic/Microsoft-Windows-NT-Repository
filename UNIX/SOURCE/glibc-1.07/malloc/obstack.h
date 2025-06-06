/* obstack.h - object stack macros
   Copyright (C) 1988, 1992 Free Software Foundation, Inc.

This file is part of the GNU C Library.  Its master source is NOT part of
the C library, however.  The master source lives in /gd/gnu/lib.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/* Summary:

All the apparent functions defined here are macros. The idea
is that you would use these pre-tested macros to solve a
very specific set of problems, and they would run fast.
Caution: no side-effects in arguments please!! They may be
evaluated MANY times!!

These macros operate a stack of objects.  Each object starts life
small, and may grow to maturity.  (Consider building a word syllable
by syllable.)  An object can move while it is growing.  Once it has
been "finished" it never changes address again.  So the "top of the
stack" is typically an immature growing object, while the rest of the
stack is of mature, fixed size and fixed address objects.

These routines grab large chunks of memory, using a function you
supply, called `obstack_chunk_alloc'.  On occasion, they free chunks,
by calling `obstack_chunk_free'.  You must define them and declare
them before using any obstack macros.

Each independent stack is represented by a `struct obstack'.
Each of the obstack macros expects a pointer to such a structure
as the first argument.

One motivation for this package is the problem of growing char strings
in symbol tables.  Unless you are "fascist pig with a read-only mind"
--Gosper's immortal quote from HAKMEM item 154, out of context--you
would not like to put any arbitrary upper limit on the length of your
symbols.

In practice this often means you will build many short symbols and a
few long symbols.  At the time you are reading a symbol you don't know
how long it is.  One traditional method is to read a symbol into a
buffer, realloc()ating the buffer every time you try to read a symbol
that is longer than the buffer.  This is beaut, but you still will
want to copy the symbol from the buffer to a more permanent
symbol-table entry say about half the time.

With obstacks, you can work differently.  Use one obstack for all symbol
names.  As you read a symbol, grow the name in the obstack gradually.
When the name is complete, finalize it.  Then, if the symbol exists already,
free the newly read name.

The way we do this is to take a large chunk, allocating memory from
low addresses.  When you want to build a symbol in the chunk you just
add chars above the current "high water mark" in the chunk.  When you
have finished adding chars, because you got to the end of the symbol,
you know how long the chars are, and you can create a new object.
Mostly the chars will not burst over the highest address of the chunk,
because you would typically expect a chunk to be (say) 100 times as
long as an average object.

In case that isn't clear, when we have enough chars to make up
the object, THEY ARE ALREADY CONTIGUOUS IN THE CHUNK (guaranteed)
so we just point to it where it lies.  No moving of chars is
needed and this is the second win: potentially long strings need
never be explicitly shuffled. Once an object is formed, it does not
change its address during its lifetime.

When the chars burst over a chunk boundary, we allocate a larger
chunk, and then copy the partly formed object from the end of the old
chunk to the beginning of the new larger chunk.  We then carry on
accreting characters to the end of the object as we normally would.

A special macro is provided to add a single char at a time to a
growing object.  This allows the use of register variables, which
break the ordinary 'growth' macro.

Summary:
	We allocate large chunks.
	We carve out one object at a time from the current chunk.
	Once carved, an object never moves.
	We are free to append data of any size to the currently
	  growing object.
	Exactly one object is growing in an obstack at any one time.
	You can run one obstack per control block.
	You may have as many control blocks as you dare.
	Because of the way we do it, you can `unwind' an obstack
	  back to a previous state. (You may remove objects much
	  as you would with a stack.)
*/


/* Don't do the contents of this file more than once.  */

#ifndef __OBSTACKS__
#define __OBSTACKS__

/* We use subtraction of (char *)0 instead of casting to int
   because on word-addressable machines a simple cast to int
   may ignore the byte-within-word field of the pointer.  */

#ifndef __PTR_TO_INT
#define __PTR_TO_INT(P) ((P) - (char *)0)
#endif

#ifndef __INT_TO_PTR
#define __INT_TO_PTR(P) ((P) + (char *)0)
#endif

/* We need the type of the resulting object.  In ANSI C it is ptrdiff_t
   but in traditional C it is usually long.  If we are in ANSI C and
   don't already have ptrdiff_t get it.  */

#if defined (__STDC__) && ! defined (offsetof)
#if defined (__GNUC__) && defined (IN_GCC)
/* On Next machine, the system's stddef.h screws up if included
   after we have defined just ptrdiff_t, so include all of gstddef.h.
   Otherwise, define just ptrdiff_t, which is all we need.  */
#ifndef __NeXT__
#define __need_ptrdiff_t
#endif

/* While building GCC, the stddef.h that goes with GCC has this name.  */
#include "gstddef.h"
#else
#include <stddef.h>
#endif
#endif

#ifdef __STDC__
#define PTR_INT_TYPE ptrdiff_t
#else
#define PTR_INT_TYPE long
#endif

struct _obstack_chunk		/* Lives at front of each chunk. */
{
  char  *limit;			/* 1 past end of this chunk */
  struct _obstack_chunk *prev;	/* address of prior chunk or NULL */
  char	contents[4];		/* objects begin here */
};

struct obstack		/* control current object in current chunk */
{
  long	chunk_size;		/* preferred size to allocate chunks in */
  struct _obstack_chunk* chunk;	/* address of current struct obstack_chunk */
  char	*object_base;		/* address of object we are building */
  char	*next_free;		/* where to add next char to current object */
  char	*chunk_limit;		/* address of char after current chunk */
  PTR_INT_TYPE temp;		/* Temporary for some macros.  */
  int   alignment_mask;		/* Mask of alignment for each object. */
  struct _obstack_chunk *(*chunkfun) (); /* User's fcn to allocate a chunk.  */
  void (*freefun) ();		/* User's function to free a chunk.  */
  char *extra_arg;		/* first arg for chunk alloc/dealloc funcs */
  unsigned use_extra_arg:1;	/* chunk alloc/dealloc funcs take extra arg */
  unsigned maybe_empty_object:1;/* There is a possibility that the current
				   chunk contains a zero-length object.  This
				   prevents freeing the chunk if we allocate
				   a bigger chunk to replace it. */
};

/* Declare the external functions we use; they are in obstack.c.  */

#ifdef __STDC__
extern void _obstack_newchunk (struct obstack *, int);
extern void _obstack_free (struct obstack *, void *);
extern void _obstack_begin (struct obstack *, int, int,
			    void *(*) (), void (*) ());
extern void _obstack_begin_1 (struct obstack *, int, int,
			      void *(*) (), void (*) (), void *);
#else
extern void _obstack_newchunk ();
extern void _obstack_free ();
extern void _obstack_begin ();
extern void _obstack_begin_1 ();
#endif

#ifdef __STDC__

/* Do the function-declarations after the structs
   but before defining the macros.  */

void obstack_init (struct obstack *obstack);

void * obstack_alloc (struct obstack *obstack, int size);

void * obstack_copy (struct obstack *obstack, void *address, int size);
void * obstack_copy0 (struct obstack *obstack, void *address, int size);

void obstack_free (struct obstack *obstack, void *block);

void obstack_blank (struct obstack *obstack, int size);

void obstack_grow (struct obstack *obstack, void *data, int size);
void obstack_grow0 (struct obstack *obstack, void *data, int size);

void obstack_1grow (struct obstack *obstack, int data_char);
void obstack_ptr_grow (struct obstack *obstack, void *data);
void obstack_int_grow (struct obstack *obstack, int data);

void * obstack_finish (struct obstack *obstack);

int obstack_object_size (struct obstack *obstack);

int obstack_room (struct obstack *obstack);
void obstack_1grow_fast (struct obstack *obstack, int data_char);
void obstack_ptr_grow_fast (struct obstack *obstack, void *data);
void obstack_int_grow_fast (struct obstack *obstack, int data);
void obstack_blank_fast (struct obstack *obstack, int size);

void * obstack_base (struct obstack *obstack);
void * obstack_next_free (struct obstack *obstack);
int obstack_alignment_mask (struct obstack *obstack);
int obstack_chunk_size (struct obstack *obstack);

#endif /* __STDC__ */

/* Non-ANSI C cannot really support alternative functions for these macros,
   so we do not declare them.  */

/* Pointer to beginning of object being allocated or to be allocated next.
   Note that this might not be the final address of the object
   because a new chunk might be needed to hold the final size.  */

#define obstack_base(h) ((h)->object_base)

/* Size for allocating ordinary chunks.  */

#define obstack_chunk_size(h) ((h)->chunk_size)

/* Pointer to next byte not yet allocated in current chunk.  */

#define obstack_next_free(h)	((h)->next_free)

/* Mask specifying low bits that should be clear in address of an object.  */

#define obstack_alignment_mask(h) ((h)->alignment_mask)

#define obstack_init(h) \
  _obstack_begin ((h), 0, 0, \
		  (void *(*) ()) obstack_chunk_alloc, (void (*) ()) obstack_chunk_free)

#define obstack_begin(h, size) \
  _obstack_begin ((h), (size), 0, \
		  (void *(*) ()) obstack_chunk_alloc, (void (*) ()) obstack_chunk_free)

#define obstack_specify_allocation(h, size, alignment, chunkfun, freefun) \
  _obstack_begin ((h), (size), (alignment), \
		    (void *(*) ()) (chunkfun), (void (*) ()) (freefun))

#define obstack_specify_allocation_with_arg(h, size, alignment, chunkfun, freefun, arg) \
  _obstack_begin_1 ((h), (size), (alignment), \
		    (void *(*) ()) (chunkfun), (void (*) ()) (freefun), (arg))

#define obstack_1grow_fast(h,achar) (*((h)->next_free)++ = achar)

#define obstack_blank_fast(h,n) ((h)->next_free += (n))

#if defined (__GNUC__) && defined (__STDC__)
#if __GNUC__ < 2
#define __extension__
#endif

/* For GNU C, if not -traditional,
   we can define these macros to compute all args only once
   without using a global variable.
   Also, we can avoid using the `temp' slot, to make faster code.  */

#define obstack_object_size(OBSTACK)					\
  __extension__								\
  ({ struct obstack *__o = (OBSTACK);					\
     (unsigned) (__o->next_free - __o->object_base); })

#define obstack_room(OBSTACK)						\
  __extension__								\
  ({ struct obstack *__o = (OBSTACK);					\
     (unsigned) (__o->chunk_limit - __o->next_free); })

/* Note that the call to _obstack_newchunk is enclosed in (..., 0)
   so that we can avoid having void expressions
   in the arms of the conditional expression.
   Casting the third operand to void was tried before,
   but some compilers won't accept it.  */
#define obstack_grow(OBSTACK,where,length)				\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   int __len = (length);						\
   ((__o->next_free + __len > __o->chunk_limit)				\
    ? (_obstack_newchunk (__o, __len), 0) : 0);				\
   bcopy (where, __o->next_free, __len);				\
   __o->next_free += __len;						\
   (void) 0; })

#define obstack_grow0(OBSTACK,where,length)				\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   int __len = (length);						\
   ((__o->next_free + __len + 1 > __o->chunk_limit)			\
    ? (_obstack_newchunk (__o, __len + 1), 0) : 0),			\
   bcopy (where, __o->next_free, __len),				\
   __o->next_free += __len,						\
   *(__o->next_free)++ = 0;						\
   (void) 0; })

#define obstack_1grow(OBSTACK,datum)					\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   ((__o->next_free + 1 > __o->chunk_limit)				\
    ? (_obstack_newchunk (__o, 1), 0) : 0),				\
   *(__o->next_free)++ = (datum);					\
   (void) 0; })

/* These assume that the obstack alignment is good enough for pointers or ints,
   and that the data added so far to the current object
   shares that much alignment.  */
   
#define obstack_ptr_grow(OBSTACK,datum)					\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   ((__o->next_free + sizeof (void *) > __o->chunk_limit)		\
    ? (_obstack_newchunk (__o, sizeof (void *)), 0) : 0),		\
   *((void **)__o->next_free)++ = ((void *)datum);			\
   (void) 0; })

#define obstack_int_grow(OBSTACK,datum)					\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   ((__o->next_free + sizeof (int) > __o->chunk_limit)			\
    ? (_obstack_newchunk (__o, sizeof (int)), 0) : 0),			\
   *((int *)__o->next_free)++ = ((int)datum);				\
   (void) 0; })

#define obstack_ptr_grow_fast(h,aptr) (*((void **)(h)->next_free)++ = (void *)aptr)
#define obstack_int_grow_fast(h,aint) (*((int *)(h)->next_free)++ = (int)aint)

#define obstack_blank(OBSTACK,length)					\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   int __len = (length);						\
   ((__o->chunk_limit - __o->next_free < __len)				\
    ? (_obstack_newchunk (__o, __len), 0) : 0);				\
   __o->next_free += __len;						\
   (void) 0; })

#define obstack_alloc(OBSTACK,length)					\
__extension__								\
({ struct obstack *__h = (OBSTACK);					\
   obstack_blank (__h, (length));					\
   obstack_finish (__h); })

#define obstack_copy(OBSTACK,where,length)				\
__extension__								\
({ struct obstack *__h = (OBSTACK);					\
   obstack_grow (__h, (where), (length));				\
   obstack_finish (__h); })

#define obstack_copy0(OBSTACK,where,length)				\
__extension__								\
({ struct obstack *__h = (OBSTACK);					\
   obstack_grow0 (__h, (where), (length));				\
   obstack_finish (__h); })

/* The local variable is named __o1 to avoid a name conflict
   when obstack_blank is called.  */
#define obstack_finish(OBSTACK)  					\
__extension__								\
({ struct obstack *__o1 = (OBSTACK);					\
   void *value = (void *) __o1->object_base;				\
   if (__o1->next_free == value)					\
     __o1->maybe_empty_object = 1;					\
   __o1->next_free							\
     = __INT_TO_PTR ((__PTR_TO_INT (__o1->next_free)+__o1->alignment_mask)\
		     & ~ (__o1->alignment_mask));			\
   ((__o1->next_free - (char *)__o1->chunk				\
     > __o1->chunk_limit - (char *)__o1->chunk)				\
    ? (__o1->next_free = __o1->chunk_limit) : 0);			\
   __o1->object_base = __o1->next_free;					\
   value; })

#define obstack_free(OBSTACK, OBJ)					\
__extension__								\
({ struct obstack *__o = (OBSTACK);					\
   void *__obj = (OBJ);							\
   if (__obj > (void *)__o->chunk && __obj < (void *)__o->chunk_limit)  \
     __o->next_free = __o->object_base = __obj;				\
   else (obstack_free) (__o, __obj); })

#else /* not __GNUC__ or not __STDC__ */

#define obstack_object_size(h) \
 (unsigned) ((h)->next_free - (h)->object_base)

#define obstack_room(h)		\
 (unsigned) ((h)->chunk_limit - (h)->next_free)

#define obstack_grow(h,where,length)					\
( (h)->temp = (length),							\
  (((h)->next_free + (h)->temp > (h)->chunk_limit)			\
   ? (_obstack_newchunk ((h), (h)->temp), 0) : 0),			\
  bcopy (where, (h)->next_free, (h)->temp),				\
  (h)->next_free += (h)->temp)

#define obstack_grow0(h,where,length)					\
( (h)->temp = (length),							\
  (((h)->next_free + (h)->temp + 1 > (h)->chunk_limit)			\
   ? (_obstack_newchunk ((h), (h)->temp + 1), 0) : 0),			\
  bcopy (where, (h)->next_free, (h)->temp),				\
  (h)->next_free += (h)->temp,						\
  *((h)->next_free)++ = 0)

#define obstack_1grow(h,datum)						\
( (((h)->next_free + 1 > (h)->chunk_limit)				\
   ? (_obstack_newchunk ((h), 1), 0) : 0),				\
  *((h)->next_free)++ = (datum))

#define obstack_ptr_grow(h,datum)					\
( (((h)->next_free + sizeof (char *) > (h)->chunk_limit)		\
   ? (_obstack_newchunk ((h), sizeof (char *)), 0) : 0),		\
  *((char **)(((h)->next_free+=sizeof(char *))-sizeof(char *))) = ((char *)datum))

#define obstack_int_grow(h,datum)					\
( (((h)->next_free + sizeof (int) > (h)->chunk_limit)			\
   ? (_obstack_newchunk ((h), sizeof (int)), 0) : 0),			\
  *((int *)(((h)->next_free+=sizeof(int))-sizeof(int))) = ((int)datum))

#define obstack_ptr_grow_fast(h,aptr) (*((char **)(h)->next_free)++ = (char *)aptr)
#define obstack_int_grow_fast(h,aint) (*((int *)(h)->next_free)++ = (int)aint)

#define obstack_blank(h,length)						\
( (h)->temp = (length),							\
  (((h)->chunk_limit - (h)->next_free < (h)->temp)			\
   ? (_obstack_newchunk ((h), (h)->temp), 0) : 0),			\
  (h)->next_free += (h)->temp)

#define obstack_alloc(h,length)						\
 (obstack_blank ((h), (length)), obstack_finish ((h)))

#define obstack_copy(h,where,length)					\
 (obstack_grow ((h), (where), (length)), obstack_finish ((h)))

#define obstack_copy0(h,where,length)					\
 (obstack_grow0 ((h), (where), (length)), obstack_finish ((h)))

#define obstack_finish(h)  						\
( ((h)->next_free == (h)->object_base					\
   ? (((h)->maybe_empty_object = 1), 0)					\
   : 0),								\
  (h)->temp = __PTR_TO_INT ((h)->object_base),				\
  (h)->next_free							\
    = __INT_TO_PTR ((__PTR_TO_INT ((h)->next_free)+(h)->alignment_mask)	\
		    & ~ ((h)->alignment_mask)),				\
  (((h)->next_free - (char *)(h)->chunk					\
    > (h)->chunk_limit - (char *)(h)->chunk)				\
   ? ((h)->next_free = (h)->chunk_limit) : 0),				\
  (h)->object_base = (h)->next_free,					\
  __INT_TO_PTR ((h)->temp))

#ifdef __STDC__
#define obstack_free(h,obj)						\
( (h)->temp = (char *)(obj) - (char *) (h)->chunk,			\
  (((h)->temp > 0 && (h)->temp < (h)->chunk_limit - (char *) (h)->chunk)\
   ? (int) ((h)->next_free = (h)->object_base				\
	    = (h)->temp + (char *) (h)->chunk)				\
   : (((obstack_free) ((h), (h)->temp + (char *) (h)->chunk), 0), 0)))
#else
#define obstack_free(h,obj)						\
( (h)->temp = (char *)(obj) - (char *) (h)->chunk,			\
  (((h)->temp > 0 && (h)->temp < (h)->chunk_limit - (char *) (h)->chunk)\
   ? (int) ((h)->next_free = (h)->object_base				\
	    = (h)->temp + (char *) (h)->chunk)				\
   : (_obstack_free ((h), (h)->temp + (char *) (h)->chunk), 0)))
#endif

#endif /* not __GNUC__ or not __STDC__ */

#endif /* not __OBSTACKS__ */
