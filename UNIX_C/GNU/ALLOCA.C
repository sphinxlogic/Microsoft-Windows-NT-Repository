/*
	alloca -- (mostly) portable public-domain implementation

	last edit:	86/02/01	D A Gwyn

	This implementation of the PWB library alloca() function,
	which is used to allocate space off the run-time stack so
	that it is automatically reclaimed upon procedure exit, 
	was inspired by discussions with J. Q. Johnson of Cornell.

	It should work under any C implementation that uses an
	actual procedure stack (as opposed to a linked list of
	frames).  There are some preprocessor constants that can
	be defined when compiling for your specific system, for
	improved efficiency; however, the defaults should be okay.

	The general concept of this implementation is to keep
	track of all alloca()-allocated blocks, and reclaim any
	that are found to be deeper in the stack than the current
	invocation.  This heuristic does not reclaim storage as
	soon as it becomes invalid, but it will do so eventually.

	As a special case, alloca(0) reclaims storage without
	allocating any.  It is a good idea to use alloca(0) in
	your main control loop, etc. to force garbage collection.
*/
#ifndef lint
static char	SCCSid[] = "@(#)alloca.c	1.2";	/* for the "what" utility */
#endif

#ifdef X3J11
typedef void	*pointer;		/* generic pointer type */
#else
typedef char	*pointer;		/* generic pointer type */
#endif

#define	NULL	0			/* null pointer constant */

extern void	free();
extern pointer	xmalloc();

/*
	Define STACK_DIRECTION if you know the direction of stack
	growth for your system; otherwise it will be automatically
	deduced at run-time.

	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
*/

#ifndef STACK_DIRECTION
#define	STACK_DIRECTION	0		/* direction unknown */
#endif

#if STACK_DIRECTION != 0

#define	STK_DIR	STACK_DIRECTION		/* known at compile-time */

#else	/* STACK_DIRECTION == 0; need run-time code */

static int	stack_dir = 0;		/* 1 or -1 once known */
#define	STK_DIR	stack_dir

static void
find_stack_direction( /* void */ )
	{
	static char	*addr = NULL;	/* address of first
					   `dummy', once known */
	auto char	dummy;		/* to get stack address */

	if ( addr == NULL )
		{			/* initial entry */
		addr = &dummy;

		find_stack_direction();	/* recurse once */
		}
	else				/* second entry */
		if ( &dummy > addr )
			stack_dir = 1;	/* stack grew upward */
		else
			stack_dir = -1;	/* stack grew downward */
	}

#endif	/* STACK_DIRECTION == 0 */

/*
	An "alloca header" is used to:
	(a) chain together all alloca()ed blocks;
	(b) keep track of stack depth.

	It is very important that sizeof(header) agree with malloc()
	alignment chunk size.  The following default should work okay.
*/

#ifndef	ALIGN_SIZE
#define	ALIGN_SIZE	sizeof(double)
#endif

typedef union hdr
	{
	char	align[ALIGN_SIZE];	/* to force sizeof(header) */
	struct	{
		union hdr	*next;	/* for chaining headers */
		char		*deep;	/* for stack depth measure */
		}	h;
	}	header;

/*
	alloca( size ) returns a pointer to at least `size' bytes of
	storage which will be automatically reclaimed upon exit from
	the procedure that called alloca().  Originally, this space
	was supposed to be taken from the current stack frame of the
	caller, but that method cannot be made to work for some
	implementations of C, for example under Gould's UTX/32.
*/

pointer
alloca( size )				/* returns pointer to storage */
	unsigned	size;		/* # bytes to allocate */
	{
	static header	*last = NULL;	/* -> last alloca header */
	auto char	probe;		/* probes stack depth: */
	register char	*depth = &probe;

#if STACK_DIRECTION == 0
	if ( STK_DIR == 0 )		/* unknown growth direction */
		find_stack_direction();
#endif

	/* Reclaim garbage, defined as all alloca()ed storage that
	   was allocated from deeper in the stack than currently. */

	{
	register header	*hp;		/* traverses linked list */

	for ( hp = last; hp != NULL; )
		if ( STK_DIR > 0 && hp->h.deep > depth
		  || STK_DIR < 0 && hp->h.deep < depth
		   )	{
			register header	*np = hp->h.next;

			free( (pointer)hp );	/* collect garbage */

			hp = np;	/* -> next header */
			}
		else
			break;		/* rest are not deeper */

	last = hp;			/* -> last valid storage */
	}

	if ( size == 0 )
		return NULL;		/* no allocation required */

	/* Allocate combined header + user data storage. */

	{
	register pointer	new = xmalloc( sizeof(header) + size );
					/* address of header */

	if ( new == NULL )
		return NULL;		/* abort() is traditional */

	((header *)new)->h.next = last;
	((header *)new)->h.deep = depth;

	last = (header *)new;

	/* User storage begins just after header. */

	return (pointer)((char *)new + sizeof(header));
	}
	}
