/*
**++
**  FACILITY:  CMEM  V1.1
**
**
**  MODULE DESCRIPTION:
**
**	This is the main module containing all user callable routines as well
**	as the replacements for the C RTL dynamic memory management routines.
**
**	The following routines are defined in this module:
**
**	    cmem___putmsg
**	    cmem___hash_index
**	    cmem___get_block_size
**	    cmem___allocate_info_cluster
**	    cmem___allocate_info_block
**	    cmem___display_block_info
**	    cmem___lookup_address
**	    cmem___condition_handler
**	    cmem___vm_allocate
**	    cmem___vm_free
**
**	    cmem_initialize
**	    cmem_reset_display_flags
**	    cmem_show_blocks
**	    cmem_check_address
**	    cmem_collect_stack
**	    cmem_collect_time
**	    cmem_boundary_check
**	    cmem_protect_freed
**
**	    cmem_malloc			(DECC: DECC$MALLOC;  VAXC: malloc)
**	    cmem_calloc			(DECC: DECC$CALLOC;  VAXC: calloc)
**	    cmem_free			(DECC: DECC$FREE;    VAXC: free)
**	    cmem_realloc		(DECC: DECC$REALLOC; VAXC: realloc)
**
**
**  AUTHORS:
**
**      Brett Hunsaker (hunsaker@eisner.decus.org)
**
**
**  CREATION DATE:  4 May 1995
**
**
**  DESIGN ISSUES:
**
**	We needed some way of tracking who was allocating memory and not
**	releasing it.
**
**
**  MODIFICATION HISTORY:
**
**      4-May-1995	B. Hunsaker	Initial implementation
**
**	Necessity is truly the mother of invention.
**
**	13-Sep-1995	B. Hunsaker	Add support for DEC C on VAX
**
**	Got a mail message suggesting the proper code to use DEC C on a
**	VAX.
**
**--
*/

/*
**
**  ENVIRONMENT SETTINGS
**
*/

/*
	For VAX C compatibility, we use the 'globalref' modifier to access
	message codes.  This will generate a warning message with DEC C, so
	disable that here.  If this module was only compiled with DEC C, you
	could get away with using 'extern int <condition_name>' rather than
	'globalref int <condition_name>'.
*/

#ifdef __DECC
#pragma message disable( GLOBALEXT )
#endif


/*
	We will access the frame pointer (R13) on a VAX to walk the stack
	when we allocate memory.  This is done using the VAX C '_READ_GPR'
	built-in function which requires the '#pragma builtins' statement.
*/

#pragma builtins


/*
**
**  GLOBAL CONSTANTS
**
*/

/*
	This is where we remap our routine names to thoses used by the RTL.
*/

#ifdef __DECC

#define		cmem_malloc		DECC$MALLOC
#define		cmem_calloc		DECC$CALLOC
#define		cmem_realloc		DECC$REALLOC
#define		cmem_free		DECC$FREE

#else

#define		cmem_malloc		malloc
#define		cmem_calloc		calloc
#define		cmem_realloc		realloc
#define		cmem_free		free

#endif

#define		END_OF_LIST		-1

#define		BYTES_PER_PAGELET	512

#define		FILL_PATTERN		0xAA

#define		MAX_CALL_DEPTH		20
#define		HASH_LIST_HEADERS	8191
#define		BLOCK_INFO_ENTRIES	50000


/*
**
**  INCLUDE FILES
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include starlet
#include lib$routines
#include ots$routines
#include prtdef
#include descrip
#include ssdef

/*
	Some VAX sites might not have re-installed VAX C to pick up the
	new $GETSYI codes which we use.  We'll define it here just to help
	those sites, but it will only help if they are running VAX/VMS V5.4?
	or better.
*/

#include syidef
#ifndef SYI$_PAGE_SIZE
#define SYI$_PAGE_SIZE 4452             /* Memory page size in bytes        */
#endif

#include varargs

#ifdef __ALPHA
#include libicb
#endif


/*
**
**  GLOBAL STRUCTURES
**
*/

typedef struct address_range_struct
{
  unsigned long start_address;
  unsigned long end_address;
} address_range_type;


/*
	We will need some information about each block of memory which is
	allocated.  This will be kept within a linked list.
*/

struct block_info_struct
{
  unsigned int				next_block;
  unsigned int				zone_id;
  void *				user_address;
  unsigned int				requested_size;
  unsigned int				call_stack[ MAX_CALL_DEPTH ];
  unsigned int				vms_time[ 2 ];
  struct
  {
    unsigned int			stack_good : 1;
    unsigned int			time_good : 1;
    unsigned int			do_not_display : 1;
  } flags;
};


/*
	We will initially allocate one 'conglomerated' structure so that
	we aren't constandtly fighting the user for dynamic memory.  This
	will be capable of holding a finite amount of information about all
	the memory blocks which have been allocated.  If the user allocates
	more blocks then this structure can hold, we'll create an extension
	for this to hold the extra info.  If that one fills too, then we
	will just keep appending more blocks as needed.  Once allocated,
	they will never be released -- just reused.
*/

struct cluster_info_struct
{
  struct cluster_info_struct *		next_cluster_ptr;
  int					list_head[ HASH_LIST_HEADERS ];
  int					available_block_count;
  int					next_available_block;
  unsigned char				block_in_use[ BLOCK_INFO_ENTRIES ];
  struct block_info_struct		block_info[ BLOCK_INFO_ENTRIES ];
};


/*
	Just for the heck of it we will store all our global data in one
	structure.
*/

struct global_values_struct
{
  unsigned long int			page_size;
  unsigned long int			pagelets_per_page;
  unsigned long int			protected_zone;
  unsigned long int			unprotected_zone;
  struct cluster_info_struct *		first_cluster_ptr;
  int					(*previous_exception_handler)();
  unsigned char				dostack;
  unsigned char				dotime;
  unsigned char				protect_allocated;
  unsigned char				protect_freed;
  unsigned char				initialized;
};


/*
**
**  GLOBAL VARIABLES
**
*/

static struct global_values_struct	cmem___globals
	= {
	    0,		/* Page size; set via $GETJPI			    */
	    0,		/* Pagelets/page: calculated via init routine	    */
	    0,		/* Protected zone id: from LIB$CREATE_VM_ZONE	    */
	    0,		/* Unprotected zone id: from LIB$CREATE_VM_ZONE	    */
	    NULL,	/* Cluster info pointer: set in init routine	    */
	    NULL,	/* Pointer to previous exception handler: not used  */

			/* ------------------------------------------------ */
			/*	       INITIAL MODE SETTINGS		    */
			/* ------------------------------------------------ */
	    TRUE,	/* Collect call stack info		    	    */
	    TRUE,	/* Collect allocation time			    */
	    FALSE,	/* Allocate with protected boundary pages	    */
	    FALSE,	/* Set memory to no-access upon free		    */
			/* ------------------------------------------------ */

	    FALSE	/* Global data initialized: set via init routine    */
	  };




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      cmem___putmsg:
**
**	Provides a convience routine for accessing the capabilities of the
**	SYS$PUTMSG system service.
**
**
**  FORMAL PARAMETERS:
**
**	The calling parameter list is the same as that used for LIB$SIGNAL.
**
**      condition_value:
**	    Type:	Unsigned long integer
**	    Access:	Input
**	    Mechanism:	By value
**
**	    Provides a VMS condition value to be converted to text.
**
**	fao_count:
**	    Type:	Signed integer
**	    Access:	Input
**	    Mechanism:	By value
**
**	    This is the number of FAO parameters associated with the message.
**	    If not specified, then no FAO parameters exist.
**
**	fao_param1:
**	    Type:	Unspecified
**	    Access:	Input
**	    Mechanism:	By value
**
**	    This is the first FAO parameter.
**
**	fao_param2:
**	    Type:	Unspecified
**	    Access:	Input
**	    Mechanism:	By value
**
**	    This is the second FAO parameter.
**
**
**  RETURN VALUE:
**
**      The status returned by the call to SYS$PUTMSG is propagated to the
**	calling routine.
**
**
**  SIDE EFFECTS:
**
**      None.
**
**
**  DESIGN:
**
**      Simply put the parameters into a structure on the heap and call the
**	SYS$PUTMSG system service.
**
**
**  PRECONDITIONS:
**   
**	None.
**
**
**  CALLING SEQUENCE:
**
**  	Here is an example calling sequence:
**
**	    cmem___putmsg( signaled_condition, 1, "burp!" );
**
**
**  EXCEPTIONS:
**   
**  	CMEM__GETVMFAIL:
**   
**	    We couldn't get the virtual memory we needed.
**
**
**--
*/

/* int cmem___putmsg( int condition_value, ... )			    */

int cmem___putmsg( va_alist )

va_dcl				/* Note that no ";" is needed.		    */

{

/*
**
**  INCLUDE FILES
**
*/


/*
**
**  CONSTANTS
**
*/


/*
**
**  STRUCTURES AND UNIONS
**
*/


/*
**
**  EXTERNAL ROUTINES
**
*/


/*
**
**  EXTERNAL CONSTANTS
**
*/

  globalref int CMEM__GETVMFAIL;


/*
**
**  LOCAL VARIABLES
**
*/

  int	    argument_count;
  int *	    putmsg_array;
  va_list   argument_pointer;
  int	    loop;
  int	    byte_size;
  int	    status;


/*
**
**  DECLARATIVE INITIALIZATION
**
*/


/*
**
**  CODE
**
*/

/*
	Get the count of arguments.  This is VMS specific.
*/

  va_count( argument_count );
  argument_count &= 0xFF;    /* Deal with a little problem in DEC C	    */

  byte_size = ( argument_count + 1 ) * sizeof( int );
  status = lib$get_vm( &byte_size, &putmsg_array, 0 );
  if ( ( status & 1 ) != 1 )
  {
    lib$signal( ( int ) &CMEM__GETVMFAIL, 1, "$PUTMSG", status );
  }
  else
  {
    putmsg_array[ 0 ] = argument_count;

    va_start( argument_pointer );
    for( loop = 1; loop <= argument_count; loop++ )
      putmsg_array[ loop ] = va_arg( argument_pointer, int );
    va_end( argument_pointer );

    status = sys$putmsg( putmsg_array, 0, 0, 0 );
    lib$free_vm( &byte_size, &putmsg_array, 0 );
  }

  return( status );
}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      cmem___hash_index:
**
**	Converts a user address value into an index into the hash table.
**
**
**  FORMAL PARAMETERS:
**
**	address:
**	    Type:	    Pointer
**	    Access:	    Input
**	    Mechanism:	    By value
**
**	    The user address which was returned from 'malloc', 'calloc', or
**	    'realloc'.
**
**
**  RETURN VALUE:
**
**	Index into hash table; ranges 0 to 'HASH_LIST_HEADERS' - 1.
**
**
**  SIDE EFFECTS:
**
**      None.
**
**
**  DESIGN:
**
**	Basically do a mod function on the value.  We make some adjustments
**	assuming that a user will normally be allocating quadword aligned
**	data structures.
**
**
**  PRECONDITIONS:
**   
**	None.
**
**
**  CALLING SEQUENCE:
**
**  	Here is an example calling sequence:
**
**	    block_index = cmem___hash_index( user_pointer );
**
**
**  EXCEPTIONS:
**   
**	None.
**
**
**--
*/

unsigned int cmem___hash_index(
	void *			address )
{

/*
**
**  INCLUDE FILES
**
*/


/*
**
**  CONSTANTS
**
*/


/*
**
**  STRUCTURES AND UNIONS
**
*/


/*
**
**  EXTERNAL ROUTINES
**
*/


/*
**
**  EXTERNAL CONSTANTS
**
*/


/*
**
**  LOCAL VARIABLES
**
*/


/*
**
**  DECLARATIVE INITIALIZATION
**
*/


/*
**
**  CODE
**
*/

/*
	This is a simple hash function.  We get rid of the bottom three
	bits as good programmers will be allocating quadword aligned data
	structures, and thus our hash function would produce very poorly
	distributed indexes.
*/

  return( ( ( unsigned int ) address >> 3 ) % HASH_LIST_HEADERS );
}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      cmem___get_block_size:
**
**	Returns the size of a memory segment associated with a specific
**	address.
**
**
**  FORMAL PARAMETERS:
**
**	user_address:
**	    Type:	    Pointer
**	    Access:	    Input
**	    Mechanism:	    By value
**
**	    The user address which was returned from 'malloc', 'calloc', or
**	    'realloc'.
**
**
**  RETURN VALUE:
**
**	The size of the associated memory segment in bytes.  If the address
**	could not be found, then we return zero.
**
**
**  SIDE EFFECTS:
**
**      None.
**
**
**  DESIGN:
**
**	The 'realloc' routine needs to know the size of the current block.
**	So, we wrote this routine to get that value.
**
**	Scan the appropriate hash linked list within each cluster until we
**	find the specified memory segment.  Return the size of the block or
**	zero if we can't find a match.
**
**
**  PRECONDITIONS:
**   
**	The 'cmem_initialize' routine must have been called.
**
**
**  CALLING SEQUENCE:
**
**  	Here is an example calling sequence:
**
**	    size = cmem___get_block_size( user_pointer );
**
**
**  EXCEPTIONS:
**   
**	None.
**
**
**--
*/

size_t cmem___get_block_size( void * user_address )
{

/*
**
**  INCLUDE FILES
**
*/


/*
**
**  CONSTANTS
**
*/


/*
**
**  STRUCTURES AND UNIONS
**
*/


/*
**
**  EXTERNAL ROUTINES
**
*/


/*
**
**  EXTERNAL CONSTANTS
**
*/


/*
**
**  LOCAL VARIABLES
**
*/

  unsigned char			found_it;
  struct cluster_info_struct *	cluster_ptr;
  int				block_index;


/*
**
**  DECLARATIVE INITIALIZATION
**
*/


/*
**
**  CODE
**
*/

/*
	Scan all the information blocks in all the clusters to see if we can
	find this address.
*/

  found_it = FALSE;
  cluster_ptr = cmem___globals.first_cluster_ptr;

  block_index = cluster_ptr->list_head[ cmem___hash_index( user_address ) ];

  while ( !found_it )
  {

/*
	If we're at the end of a linked list for a particular hash value, we
	need to move on to the next cluster (if any).
*/

    if ( block_index == END_OF_LIST )
    {
      if ( ( cluster_ptr = cluster_ptr->next_cluster_ptr ) == NULL )
	break;
      else
        block_index =
		cluster_ptr->list_head[ cmem___hash_index( user_address ) ];
    }


/*
	If the address in the info block matches what the user passed us,
	we've found the block.  Otherwise, keep on transversing the linked
	list.
*/

    else
    {
      if ( cluster_ptr->block_info[ block_index ].user_address == user_address )
	found_it = TRUE;
      else
	block_index = cluster_ptr->block_info[ block_index ].next_block;
    }
  }


/*
	If we have no entry for that address, then return zero as the size
	of the block.  Otherwise, return what is actually there.
*/

  return( found_it ?
		cluster_ptr->block_info[ block_index ].requested_size : 0 );
}




void cmem___allocate_info_cluster(
	struct cluster_info_struct * *	cluster_ptr )
{

  globalref int CMEM__INFOEXPFAIL;


  int				status;


  status = lib$get_vm( &( sizeof( struct cluster_info_struct ) ),
		cluster_ptr, 0 );
  if ( ( status & 1 ) != 1 )
  {
    lib$stop( ( int ) &CMEM__INFOEXPFAIL, 0, status );
    return;
  }

  (*cluster_ptr)->next_cluster_ptr = NULL;
  (*cluster_ptr)->available_block_count = BLOCK_INFO_ENTRIES;
  (*cluster_ptr)->next_available_block = 0;
  ots$move5( 0, NULL, END_OF_LIST,
	sizeof( (*cluster_ptr)->list_head ), (*cluster_ptr)->list_head );
  ots$move5( 0, NULL, FALSE,
	sizeof( (*cluster_ptr)->block_in_use ), (*cluster_ptr)->block_in_use );
  return;
}




struct block_info_struct * cmem___allocate_info_block(
	void *			user_address )
{


  globalref int CMEM__NOFREEBLK;


  int				status;
  struct cluster_info_struct *	cluster_ptr;
  int				blocks_scanned;
  int				block_index;
  int				hash_index;
  int				next_index;


/*
	Find a cluster which has an available block.
*/

  cluster_ptr = cmem___globals.first_cluster_ptr;
  while( cluster_ptr->available_block_count == 0 )
  {
    if ( cluster_ptr->next_cluster_ptr == NULL )
      cmem___allocate_info_cluster( &cluster_ptr->next_cluster_ptr );

    cluster_ptr = cluster_ptr->next_cluster_ptr;
  }


/*
	Go find an available block.  Although the cluster header says there
	is an avilable block, we will check to make sure we don't go into
	an infinite loop if there really is not one.
*/

  blocks_scanned = 0;
  block_index = cluster_ptr->next_available_block;


  while ( cluster_ptr->block_in_use[ block_index ] )
  {
    if ( ++blocks_scanned >= BLOCK_INFO_ENTRIES )
    {
      lib$stop( ( int ) &CMEM__NOFREEBLK );
      return( NULL );
    }

    if ( ++block_index >= BLOCK_INFO_ENTRIES )
      block_index = 0;
  }

  --cluster_ptr->available_block_count;
  cluster_ptr->next_available_block =
	( block_index == BLOCK_INFO_ENTRIES - 1 ? 0 : block_index + 1 );


/*
	We've found an open slot.  Now hook us into the appropriate linked
	list within this cluster.
*/

  hash_index = cmem___hash_index( user_address );
  if ( ( cluster_ptr->list_head[ hash_index ] ) == END_OF_LIST )
  {
    cluster_ptr->list_head[ hash_index ] = block_index;
  }
  else
  {
    next_index = cluster_ptr->list_head[ hash_index ];
    while( cluster_ptr->block_info[ next_index ].next_block != END_OF_LIST )
      next_index = cluster_ptr->block_info[ next_index ].next_block;
    cluster_ptr->block_info[ next_index ].next_block = block_index;
  }
	

/*
	Set the block so that it indicates it is now the end of the list
	and return a pointer to the block.  We can't return just the index,
	as the calling routine would not know what cluster is being used.
*/

  cluster_ptr->block_info[ block_index ].next_block = END_OF_LIST;
  cluster_ptr->block_info[ block_index ].user_address = user_address;

  cluster_ptr->block_in_use[ block_index ] = TRUE;
  return( &cluster_ptr->block_info[ block_index ] );
}




void cmem___display_block_info( struct block_info_struct * block_ptr )
{

/*
	We'll display the information to the screen even if the 'don't
	display this' flag is set.  It is up to the calling routine to
	check that flag.
*/

  globalref int CMEM__UNEXPERR;


  extern void cmem___symbolize(
	unsigned long int	address,
	char *			output_string,
	unsigned long int	output_string_length );


  int				status;
  int				loop;
  unsigned short int		return_string_length;
  struct dsc$descriptor_s	return_string_desc;
  char				return_string[ 200 ];


/*
	The user can turn off our collection of the call stack and time of
	allocation to improve performance.  In this case, we can only
	display the address and size of the allocated block of memory.
*/

  if ( block_ptr->flags.time_good )
  {
    return_string_desc.dsc$a_pointer = return_string;
    return_string_desc.dsc$b_dtype = DSC$K_DTYPE_T;
    return_string_desc.dsc$b_class = DSC$K_CLASS_S;
    return_string_desc.dsc$w_length = sizeof( return_string );

    status = sys$asctim( &return_string_length, &return_string_desc,
		&block_ptr->vms_time[ 0 ], 0 );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1,
				"$ASCTIM displaying block", status );

    return_string[ return_string_length ] = '\0';
    printf( "%08X: %d bytes allocated at %s",
		block_ptr->user_address, block_ptr->requested_size,
		return_string );
  }
  else
  {
    printf( "%08X: %d bytes allocated",
		block_ptr->user_address, block_ptr->requested_size );
  }


/*
	Display the stack if we collected that information.
*/

  if ( block_ptr->flags.stack_good )
  {
    printf( "; call stack:\n" );
    for( loop = 0; loop < MAX_CALL_DEPTH; loop++ )
    {
      if ( block_ptr->call_stack[ loop ] != 0 )
      {
	cmem___symbolize( block_ptr->call_stack[ loop ],
				return_string, sizeof( return_string ) );
	printf( "  %2d: PC = %08X%s\n", loop + 1,
			block_ptr->call_stack[ loop ], return_string );
      }
    }
  }
  printf( "\n" );

  return;
}




struct block_info_struct * cmem___lookup_address(
	void *			user_address )
{

/*
	This routine scans all allocated blocks to see if an address might
	be in one of the protected boundary pages.  We return a pointer to
	the info block if we find it, else we return NULL.
*/


  struct cluster_info_struct *	cluster_ptr;
  int				block_index;
  address_range_type		address_range;


/*
	Scan all allocated blocks in the protected zone to see if the
	address provided by the user is in one of the allocated segments.
*/

  for( cluster_ptr = cmem___globals.first_cluster_ptr;
	cluster_ptr != NULL; cluster_ptr = cluster_ptr->next_cluster_ptr )
  {
    for( block_index = 0; block_index < BLOCK_INFO_ENTRIES; block_index++ )
    {
      if ( ( cluster_ptr->block_in_use[ block_index ] )
	    && ( cluster_ptr->block_info[ block_index ].zone_id
		    == cmem___globals.protected_zone ) )

      {

/*
	Calculate the address range for this block.  If the address of
	the access violation is within the allocated space, then 
	return a pointer to the block info.
*/

	address_range.start_address = ( ( ( unsigned int )
		cluster_ptr->block_info[ block_index ].user_address
		/ cmem___globals.page_size )
		* cmem___globals.page_size ) - cmem___globals.page_size;
	address_range.end_address = ( unsigned int )
		cluster_ptr->block_info[ block_index ].user_address
		+ cluster_ptr->block_info[ block_index ].requested_size
		- 1 + cmem___globals.page_size;

	if ( ( ( unsigned int ) user_address >= address_range.start_address )
	    && ( ( unsigned int ) user_address <= address_range.end_address ) )
	  return( &cluster_ptr->block_info[ block_index ] );
      }
    }
  }

  return( NULL );
}




int cmem___condition_handler(
        int *           signal_arguments,
        int *           mechanism_arguments )
{

/*
	THIS ROUTINE IS NOT FULLY IMPLEMENTED.  IT DOESN'T WORK WITH THE
	DEBUGGER AS IT SHOULD.  THE INITIALIZATION CODE DOES NOT CURRENTLY
	DECLARE THIS AS A CONDITION HANDLER.
*/

  globalref int CMEM__BNDRYRDVIO;
  globalref int CMEM__BNDRYWRTVIO;


  int				    status;
  struct block_info_struct *	    block_ptr;


/*
	If we got an access violation, check to see if it was in one of our
	protected pages.  Display information about the associated block
	of memory.
*/

  if ( signal_arguments[ 1 ] == SS$_ACCVIO )
  {
    if ( ( block_ptr
	= cmem___lookup_address( ( void * ) signal_arguments[ 3 ] ) ) != NULL ) 
    {
      if ( ( signal_arguments[ 2 ] & 0x4 ) == 0 )
        cmem___putmsg( ( int ) &CMEM__BNDRYRDVIO, 1, signal_arguments[ 3 ] );
      else
        cmem___putmsg( ( int ) &CMEM__BNDRYWRTVIO, 1, signal_arguments[ 3 ] );
    }
    cmem___display_block_info( block_ptr );
  }


/*
	There can only be one routine using the exception vector.  So,
	if someone was there before us, we need to invoke them so they
	can do whatever they want.
*/

  if ( cmem___globals.previous_exception_handler != NULL )
  {
    status = cmem___globals.previous_exception_handler(
                                signal_arguments, mechanism_arguments );
    return( status );
  }
  else
    return( SS$_RESIGNAL );
}




int cmem___vm_allocate(
	int *			number_of_bytes,
	void * *		base_address,
	unsigned int *		user_argument )
{

  globalref int			CMEM__NORMAL;
  globalref int			CMEM__EXPREGFAIL;
  globalref int			CMEM__MALLOCFAIL;
  globalref int			CMEM__PROTCHGFAIL;
  globalref int			CMEM__UNEXPERR;


  int				status;
  int				user_pages;
  int				internal_pagelets;
  address_range_type		address_range;
  address_range_type		address_range2;


/*
	The VMS system service we use requires the size to be specified in
	pagelets.  We also need a page at the beginning and at the end of
	this space to try and detect reads and writes outside the allocated
	space.
*/

  user_pages = ( *number_of_bytes + cmem___globals.page_size - 1 )
				/ cmem___globals.page_size;
  internal_pagelets = ( user_pages + 2 )
				* cmem___globals.pagelets_per_page;


/*
	We allocate the number of pagelets.  VMS will round the number of
	pagelets to properly align with the page size of the system.
*/

  status = sys$expreg( internal_pagelets, &address_range, 0, 0 );
  if ( ( status & 1 ) != 1 )
  {
    lib$signal( ( int ) &CMEM__EXPREGFAIL,
			    2, number_of_bytes, internal_pagelets, status );
    base_address = NULL;
    goto error_return;
  }


/*
	Protect the starting and ending pages so that the code will incur
	an access violation when there is any attempt to read or write
	them.
*/

  address_range2.start_address = address_range.start_address;
  address_range2.end_address = address_range.start_address;

  status = sys$setprt( &address_range2, 0, 0, PRT$C_NA, 0 );
  if ( ( status & 1 ) != 1 )
  {
    lib$signal( ( int ) &CMEM__PROTCHGFAIL, 0, status );
    base_address = NULL;
    goto error_return;
  }

  address_range2.start_address = address_range.end_address;
  address_range2.end_address = address_range.end_address;

  status = sys$setprt( &address_range2, 0, 0, PRT$C_NA, 0 );
  if ( ( status & 1 ) != 1 )
  {
    lib$signal( ( int ) &CMEM__PROTCHGFAIL, 0, status );
    base_address = NULL;
    goto error_return;
  }


/*
	Determine the address to return to the user.  We want to make it
	so that the end of the space is next to the protected ending
	page.
*/

  *base_address = ( void * ) ( address_range.end_address
			- *number_of_bytes - cmem___globals.page_size + 1 );

/*
	We want to initialize any memory between the initial protected
	page and the 'base_address' with a value which will be checked when
	we free the memory.
*/

  ots$move5( 0, NULL, FILL_PATTERN,
		( unsigned int ) *base_address - ( address_range.start_address
			+ cmem___globals.page_size ),
		address_range.start_address + cmem___globals.page_size );

/*
	That's it.  Re-enable ASTs if that was how it was configured when we
	were called.
*/

  status = ( int ) &CMEM__NORMAL;


error_return:

  return( status );
}




int cmem___vm_free(
	int *				number_of_bytes,
	struct block_info_struct *	block_info_ptr,
	unsigned int *			user_argument )
{

  globalref int CMEM__NORMAL;
  globalref int CMEM__FREEWRTFAIL;
  globalref int CMEM__PROTCHGFAIL;
  globalref int CMEM__DELTVAFAIL;


  int				status;
  address_range_type		address_range;
  address_range_type		address_range2;
  char				single_character;
  struct dsc$descriptor_s	single_character_desc;
  struct dsc$descriptor_s	check_string_desc;


/*
	For the 'protected' zone, we expect the calling routine to pass us
	the address of the information block, rather than the address of
	allocated memory.
*/


/*
	Begin by calculating the address range of the blocks which were
	allocated.
*/

  address_range.start_address
	= ( ( ( unsigned int ) block_info_ptr->user_address
		/ cmem___globals.page_size )
		* cmem___globals.page_size ) - cmem___globals.page_size;
  address_range.end_address
	= ( unsigned int ) block_info_ptr->user_address
	    + block_info_ptr->requested_size - 1 + cmem___globals.page_size;


/*
	Check the memory preceding us to insure that it is the correct
	pattern.  If not, then somebody wrote into that space which is a
	no-no.
*/

  single_character = FILL_PATTERN;
  single_character_desc.dsc$a_pointer = &single_character;
  single_character_desc.dsc$b_dtype = DSC$K_DTYPE_T;
  single_character_desc.dsc$b_class = DSC$K_CLASS_S;
  single_character_desc.dsc$w_length = sizeof( single_character );

  check_string_desc.dsc$a_pointer
	= ( char * ) ( address_range.start_address + cmem___globals.page_size );
  check_string_desc.dsc$b_dtype = DSC$K_DTYPE_T;
  check_string_desc.dsc$b_class = DSC$K_CLASS_S;
  check_string_desc.dsc$w_length
	    = ( unsigned int ) block_info_ptr->user_address
		- ( unsigned int ) check_string_desc.dsc$a_pointer;

  if ( lib$skpc( &single_character_desc, &check_string_desc ) != 0 )
  {
    status = ( int ) &CMEM__FREEWRTFAIL;
    goto error_return;
  }


/*
	For this zone only, the user can tell us to actually release the
	memory or else just protect it.  The latter can be used to detect use
	of released memory at the cost of continually increasing use of
	virtual memory.
*/

  if ( cmem___globals.protect_freed )
  {
    status = sys$setprt(
		&address_range,
		0, 0, PRT$C_NA, 0 );
    if ( ( status & 1 ) != 1 )
    {
      lib$signal( ( int ) &CMEM__PROTCHGFAIL, 0, status );
      status = ( int ) &CMEM__PROTCHGFAIL;
      goto error_return;
    }
  }
  else
  {

/*
	We change the protection on the pages we accessed back to user-write.
	Although the $DELTVA system service probably does this...
*/

    address_range2.start_address = address_range.start_address;
    address_range2.end_address = address_range.start_address;

    status = sys$setprt( &address_range2, 0, 0, PRT$C_UW, 0 );
    if ( ( status & 1 ) != 1 )
    {
      lib$signal( ( int ) &CMEM__PROTCHGFAIL, 0, status );
      status = ( int ) &CMEM__PROTCHGFAIL;
      goto error_return;
    }

    address_range2.start_address = address_range.end_address;
    address_range2.end_address = address_range.end_address;

    status = sys$setprt( &address_range2, 0, 0, PRT$C_UW, 0 );
    if ( ( status & 1 ) != 1 )
    {
      lib$signal( ( int ) &CMEM__PROTCHGFAIL, 0, status );
      status = ( int ) &CMEM__PROTCHGFAIL;
      goto error_return;
    }


/*
	Release the memory.  The $DELTVA system service will work great
	unless the user's program allocates static structures after some
	dynamic stuff has enlarged the virtual address space.
*/

    status = sys$deltva( &address_range, 0, 0 );
    if ( ( status & 1 ) != 1 )
    {
      lib$signal( ( int ) &CMEM__DELTVAFAIL, 0, status );
      status = ( int ) &CMEM__DELTVAFAIL;
      goto error_return;
    }
  }

  status = ( int ) &CMEM__NORMAL;


error_return:

  return( status );
}




void cmem_initialize( void )
{

  struct item_list_struct
  {
    unsigned short int		buffer_length;
    unsigned short int		item_code;
    void *			buffer_address;
    unsigned short int *	return_length_address;
  };

  struct io_status_block_struct
  {
    unsigned int		condition_value;
    unsigned int		reserved;
  };


  globalref int CMEM__UNEXPERR;
  globalref int CMEM__INIT;


  extern int cmem___start_debugger( void );
  int cmem_collect_stack( unsigned char state );
  int cmem_collect_time( unsigned char state );
  int cmem_boundary_check( unsigned char state );
  int cmem_protect_freed( unsigned char state );


  int				status;
  int				temp;
  int				previous_ast_state;

  char *			equivalence_string;

  struct item_list_struct	getsyi_item_list[ 2 ];
  struct io_status_block_struct	iosb;


  $DESCRIPTOR( protected_zone_desc, "C Debug Protected Heap" );
  $DESCRIPTOR( unprotected_zone_desc, "C Debug Unprotected Heap" );


/*
	Most CMEM routines which call this initialization routine will have
	already disabled AST delivery.  But it is possible that the user may
	call us directly.  So, we need to make sure ASTs are disabled.

	There is a slight chance that an AST may fire after we were invoked
	but before we can disable ASTs and that the AST may have invoked this
	routine.  To allow for this, we check the status of the 'initialized'
	flag to see if we really need to run.

	Also, a user might call us twice, so it makes sense to see if we
	were initialized before.
*/

  previous_ast_state = sys$setast( 0 );
  if ( !cmem___globals.initialized )
  {

/*
	We will have two virtual memory zones defined.  The 'protected' zone
	has pages set to no-access both before and after each allocated
	chunk of memory.  The other is the standard heap with allocated
	chunks next to each other.

	Why not just use the protected memory scheme all the time?  Well, it
	has a tendancy to eat up a LOT of virtual address space.  On an
	Alpha with 8KByte pages, allocating 1 byte will require the use of 3
	pages of memory (24 KBytes).
*/

    status = lib$create_user_vm_zone(
		&cmem___globals.protected_zone,
		0,
		cmem___vm_allocate,
		cmem___vm_free,
		0,
		0,
		&protected_zone_desc );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR,
			    1, "LIB$CREATE_USER_VM_ZONE", status );

    status = lib$create_vm_zone(
		&cmem___globals.unprotected_zone,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&unprotected_zone_desc,
		0,
		0 );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR,
			    1, "LIB$CREATE_VM_ZONE", status );


/*
	With the 'protect_allocate' flag set, we will put no-access pages
	before and after the allocated memory.  For that we need some
	information about the system's memory management hardware.
*/

    getsyi_item_list[ 0 ].buffer_length = sizeof( cmem___globals.page_size );
    getsyi_item_list[ 0 ].item_code = SYI$_PAGE_SIZE;
    getsyi_item_list[ 0 ].buffer_address = &cmem___globals.page_size;
    getsyi_item_list[ 0 ].return_length_address = 0;
    getsyi_item_list[ 1 ].buffer_length = 0;
    getsyi_item_list[ 1 ].item_code = 0;

    status = sys$getsyiw( 0, 0, 0, &getsyi_item_list, &iosb, 0, 0 );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1, "$GETSYIW for page size", status );
    else if ( ( iosb.condition_value & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1, "$GETSYIW (IOSB) for page size",
							iosb.condition_value );

    cmem___globals.pagelets_per_page
			= cmem___globals.page_size / BYTES_PER_PAGELET;


/*
	We initially allocate a large chunk of memory to maintain all our
	information about the user's allocated memory.  Hopefully this will
	prevent us from fragmenting memory while the user code is running,
	but if need be, we can add more of these structures in a linked
	list.
*/

    cmem___allocate_info_cluster( &cmem___globals.first_cluster_ptr );


/*
	The format of the debug symbol table is undocumented.  So, rather
	than try to figure out the routine name associated with an address
	on the call stack, we'll let the debugger do it for us in a 
	subprocess.
*/

    cmem___start_debugger();


/*
	Declare a condition handler to interpret an access violation error.
	We'll use the primary exception vector.  But, if the user has
	already established one, then we'll call him after we've displayed
	our information.

	THIS ISN'T WORKING YET.
*/

/*
    status = sys$setexv( 0, cmem___condition_handler,
		0, &cmem___globals.previous_exception_handler );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1, "$SETEXV", status );
*/


/*
	Our global variables are set.  We'll never need to call this routine
	again so set the flag that all other routines check and return to
	the caller.
*/

    cmem___globals.initialized = TRUE;
    cmem___putmsg( ( int ) &CMEM__INIT );


/*
	Translate some logicals to see if the user wants to override our
	defaults for some of the features of this package.  The user can
	use the VMS Debugger CALL command to dynamically change a setting.

	We do this after the global 'we have been initialized' flag has
	been set as the routines we call to set the values will call this
	initialization routine if that flag is not set.  We use the
	routines rather than setting them directly so that they will
	display the fact that a logical is overriding the documented
	default.
*/

    if( ( equivalence_string = getenv( "CMEM_COLLECT_STACK" ) ) != NULL )
    {
      sscanf( equivalence_string, "%i", &temp );
      cmem_collect_stack( temp );
    }

    if( ( equivalence_string = getenv( "CMEM_COLLECT_TIME" ) ) != NULL )
    {
      sscanf( equivalence_string, "%i", &temp );
      cmem_collect_time( temp );
    }

/*
	By default, we have both boundary checking and protection of freed
	pages disabled.  In order to protect freed memory, you need to
	have enabled boundary checks.  So, translate the boundary check
	logical first to allow the user to enable that before enabling
	protection of freed memory.

        If the user fails to enable boundary checks before requesting
	protection of freed memory, he will get the 'NOBNDYPRT' warning
	when we invoke 'cmem_protect_freed'.
*/
	
    if( ( equivalence_string = getenv( "CMEM_BOUNDARY_CHECK" ) ) != NULL )
    {
      sscanf( equivalence_string, "%i", &temp );
      cmem_boundary_check( temp );
    }

    if( ( equivalence_string = getenv( "CMEM_PROTECT_FREED" ) ) != NULL )
    {
      sscanf( equivalence_string, "%i", &temp );
      cmem_protect_freed( temp );
    }
  }


/*
	Restore the setting of the AST enabled bit and get out of here.
*/

  if ( previous_ast_state == SS$_WASSET )
    sys$setast( 1 );

  return;
}




int cmem_reset_display_flags( void )
{

  struct cluster_info_struct *	cluster_ptr;
  int				block_index;
  int				total_block_count;


  if ( cmem___globals.initialized == FALSE )
    cmem_initialize();

  total_block_count = 0;

  for( cluster_ptr = cmem___globals.first_cluster_ptr;
	cluster_ptr != NULL; cluster_ptr = cluster_ptr->next_cluster_ptr )
  {
    for( block_index = 0; block_index < BLOCK_INFO_ENTRIES; block_index++ )
    {
      if ( cluster_ptr->block_in_use[ block_index ] )
      {
	cluster_ptr->block_info[ block_index ].flags.do_not_display = TRUE;
        total_block_count++;
      }
    }
  }

  printf( "%d blocks currently in use and will not be displayed\n",
							total_block_count );

  return( 1 );
}




void cmem_show_blocks( void )
{

  struct cluster_info_struct *	cluster_ptr;
  int				block_index;
  int				displayed_block_count;
  int				hidden_block_count;


  if ( cmem___globals.initialized == FALSE )
    cmem_initialize();

  displayed_block_count = 0;
  hidden_block_count = 0;

  printf( "\nAllocated blocks of memory:\n\n" );
  for( cluster_ptr = cmem___globals.first_cluster_ptr;
	cluster_ptr != NULL; cluster_ptr = cluster_ptr->next_cluster_ptr )
  {
    for( block_index = 0; block_index < BLOCK_INFO_ENTRIES; block_index++ )
    {
      if ( cluster_ptr->block_in_use[ block_index ] )
      {
	if ( cluster_ptr->block_info[ block_index ].flags.do_not_display )
	  hidden_block_count++;
	else
	{
	  cmem___display_block_info( &cluster_ptr->block_info[ block_index ] );
	  displayed_block_count++;
	}
      }
    }
  }

  if ( displayed_block_count == 0 )
    printf( "  -- All blocks were successfully released --\n\n" );

  return;
}




int cmem_check_address(
	void *		address )
{

  struct block_info_struct *	    block_ptr;

/*
	Check to see if the address is within one of our blocks.
*/

  if ( ( block_ptr = cmem___lookup_address( address ) ) != NULL ) 
  {
    printf( "Address %08X found:\n", address );
    cmem___display_block_info( block_ptr );
  }
  else
    printf( "Address %08X not found in any allocated block\n", address );

  return( SS$_NORMAL );
}




int cmem_collect_stack( unsigned char state )
{

/*
	This routine determines whether or not we collect the call stack when
	allocating a segment of memory.  You can call this from the debugger
	using the CALL command.  For example:

	    DBG> CALL cmem_collect_stack( 1 )
*/


  globalref int CMEM__STACKENA;
  globalref int CMEM__STACKDIS;


  int			    status;


  if ( cmem___globals.initialized == FALSE )
    cmem_initialize();

  cmem___globals.dostack = state;
  if ( cmem___globals.dostack )
    status = ( int ) &CMEM__STACKENA;
  else
    status = ( int ) &CMEM__STACKDIS;

  cmem___putmsg( status );
  return( status );
}




int cmem_collect_time( unsigned char state )
{

/*
	This routine determines whether or not we collect the time a block
	was allocated.  You can call this from the debugger using the CALL
	command.  For example:

	    DBG> CALL cmem_collect_time( 1 )
*/


  globalref int CMEM__TIMEENA;
  globalref int CMEM__TIMEDIS;


  int			    status;


  if ( cmem___globals.initialized == FALSE )
    cmem_initialize();

  cmem___globals.dotime = state;
  if ( cmem___globals.dotime )
    status = ( int ) &CMEM__TIMEENA;
  else
    status = ( int ) &CMEM__TIMEDIS;

  cmem___putmsg( status );
  return( status );
}




int cmem_boundary_check( unsigned char state )
{

/*
	This routine allows the user to decide if memory at both ends of
	an allocated block is to be protected.  You can call this routine
	from the debugger as shown:

	    DBG> CALL cmem_boundary_check( 1 )
*/


  globalref int CMEM__BNDRYPRTENA;
  globalref int CMEM__BNDRYPRTDIS;


  int				status;


  if ( cmem___globals.initialized == FALSE )
    cmem_initialize();

  cmem___globals.protect_allocated = state;
  if ( cmem___globals.protect_allocated )
    status = ( int ) &CMEM__BNDRYPRTENA;
  else
    status = ( int ) &CMEM__BNDRYPRTDIS;

  cmem___putmsg( status );
  return( status );
}




int cmem_protect_freed( unsigned char state )
{

/*
	Memory that was allocated with boundary checking can be set so that
	an attempt to access it after it is freed will result in an access
	violation.

	    DBG> CALL cmem_protect_freed( 1 )
*/


  globalref int CMEM__RELMEMPRT;
  globalref int CMEM__RELMEMREUSE;
  globalref int CMEM__NOBNDYPRT;


  int				status;


  if ( cmem___globals.initialized == FALSE )
    cmem_initialize();

  cmem___globals.protect_freed = state;
  if ( cmem___globals.protect_freed )
  {
    if ( cmem___globals.protect_allocated )
      status = ( int ) &CMEM__RELMEMPRT;
    else
      status = ( int ) &CMEM__NOBNDYPRT;
  }
  else
    status = ( int ) &CMEM__RELMEMREUSE;

  cmem___putmsg( status );
  return( status );
}




void * cmem_malloc( size_t size )
{

/*
	We use two ifdefs here instead of a 'ifdef __VAX && __DECC' because
	you get an error if either symbol is not defined.
*/

#ifdef __VAX

#ifdef __DECC

/*
	Lars Forsstrom (larfo@weald.air.saab.se) provided the following as
	the technique to use to get the frame pointer on the VAX when using
	DEC C.

	He said that this solution is found in EXC_HANDLING.H.
*/

  extern int *	    cma$exc_fetch_fp( void );

#endif

#endif


  globalref int			CMEM__MALLOCFAIL;
  globalref int			CMEM__UNEXPERR;


  int				status;
  int				previous_ast_state;
  void *			base_address;
  struct block_info_struct *	block_info_ptr;
  int				stack_index;

#ifdef __ALPHA

  struct invo_context_blk	invocation;

#else

  int *				frame_pointer;

#endif


/*
	This routine is AST reentrant.  We accomplish this by disabling ASTs
	while we are on the call stack.  The VAX C versions of 'malloc'
	always seem to have some AST related problems...
*/

  previous_ast_state = sys$setast( 0 );


/*
	The standard "C" library doesn't provide any initialization routine
	that the user needs to call.  So we need to make sure that we call
	the set-up routine in all user visible routines.
*/

  if ( !cmem___globals.initialized )
    cmem_initialize();


/*
	We can allocate from one of two zones depending upon what the user
	has selected.
*/

  if ( cmem___globals.protect_allocated )
    status = lib$get_vm( &size, &base_address,
					&cmem___globals.protected_zone );
  else
    status = lib$get_vm( &size, &base_address,
					&cmem___globals.unprotected_zone );
  if ( ( status & 1 ) != 1 )
  {
    lib$signal( ( int ) &CMEM__MALLOCFAIL, 1, size, status );
    base_address = NULL;
    goto error_return;
  }


/*
	Fill the allocated memory with 0xAA.
*/

  ots$move5( 0, NULL, FILL_PATTERN, size, base_address );


/*
	We need to record information about this block in our tables.
*/

  block_info_ptr = cmem___allocate_info_block( base_address );

  block_info_ptr->requested_size = size;
  block_info_ptr->flags.do_not_display = FALSE;
  block_info_ptr->flags.stack_good = cmem___globals.dostack;
  block_info_ptr->flags.time_good = cmem___globals.dotime;
  block_info_ptr->zone_id = ( cmem___globals.protect_allocated ?
	cmem___globals.protected_zone : cmem___globals.unprotected_zone );


/*
	Get the current time, if we are collecting stats.
*/

  if ( cmem___globals.dotime )
  {
    status = sys$gettim( &block_info_ptr->vms_time );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1, "$GETTIM", status );
  }


/*
	Record the call stack for later display.  Unused entries in the array
	are filled with zeros so that the display routine will know when to
	stop.
*/

  if ( cmem___globals.dostack )
  {
    stack_index = 0;

#ifdef __ALPHA						/* DEC C on ALPHA   */

    lib$get_curr_invo_context( &invocation );
    while ( lib$get_prev_invo_context( &invocation ) == 1 )
    {
      block_info_ptr->call_stack[ stack_index++ ]
		= invocation.libicb$q_program_counter[ 0 ];
      if ( stack_index >= MAX_CALL_DEPTH )
        break;
    }

#elif __DECC					    /* DEC C on VAX	    */

    frame_pointer = ( int * ) cma$exc_fetch_fp();

    while ( _PROBER( 3, sizeof( int ), frame_pointer ) == 1 )
    {
      block_info_ptr->call_stack[ stack_index++ ] = frame_pointer[ 4 ];
      frame_pointer = ( int * ) frame_pointer[ 3 ];
      if ( stack_index >= MAX_CALL_DEPTH )
        break;
    }

#else						    /* VAX C		    */

    frame_pointer = ( int * ) _READ_GPR( 13 );

    while ( _PROBER( 3, sizeof( int ), frame_pointer ) == 1 )
    {
      block_info_ptr->call_stack[ stack_index++ ] = frame_pointer[ 4 ];
      frame_pointer = ( int * ) frame_pointer[ 3 ];
      if ( stack_index >= MAX_CALL_DEPTH )
        break;
    }

#endif

    while ( stack_index < MAX_CALL_DEPTH )
      block_info_ptr->call_stack[ stack_index++ ] = 0;
  }
    

/*
	That's it.  Re-enable ASTs if that was how it was configured when we
	were called.
*/

error_return:

  if ( previous_ast_state == SS$_WASSET )
    sys$setast( 1 );

  return( ( void * ) base_address );
}




void * cmem_calloc( size_t nobj, size_t size )
{


  globalref int			CMEM__MALLOCFAIL;
  globalref int			CMEM__UNEXPERR;


  int				status;
  int				previous_ast_state;
  unsigned int			byte_size;
  void *			base_address;
  struct block_info_struct *	block_info_ptr;
  int				stack_index;

#ifdef __ALPHA

  struct invo_context_blk	invocation;

#endif

/*
	This routine is AST reentrant.  We accomplish this by disabling ASTs
	while we are on the call stack.  The VAX C versions of 'calloc'
	always seem to have some AST related problems...
*/

  previous_ast_state = sys$setast( 0 );


/*
	The standard "C" library doesn't provide any initialization routine
	that the user needs to call.  So we need to make sure that we call
	the set-up routine in all user visible routines.
*/

  if ( !cmem___globals.initialized )
    cmem_initialize();


/*
	We can allocate from one of two zones depending upon what the user
	has selected.
*/

  byte_size = size * nobj;
  if ( cmem___globals.protect_allocated )
    status = lib$get_vm( &byte_size, &base_address,
					&cmem___globals.protected_zone );
  else
    status = lib$get_vm( &byte_size, &base_address,
					&cmem___globals.unprotected_zone );
  if ( ( status & 1 ) != 1 )
  {
    lib$signal( ( int ) &CMEM__MALLOCFAIL, 1, byte_size, status );
    base_address = NULL;
    goto error_return;
  }


/*
	Fill the allocated memory with zeros.
*/

  ots$move5( 0, NULL, 0, byte_size, base_address );


/*

	With the value calculated that we will return to the user, we can
	allocate a block in our information table.
*/

  block_info_ptr = cmem___allocate_info_block( base_address );

  block_info_ptr->requested_size = byte_size;
  block_info_ptr->flags.do_not_display = FALSE;
  block_info_ptr->flags.stack_good = cmem___globals.dostack;
  block_info_ptr->flags.time_good = cmem___globals.dotime;
  block_info_ptr->zone_id = ( cmem___globals.protect_allocated ?
	cmem___globals.protected_zone : cmem___globals.unprotected_zone );


/*
	Go get the current time.
*/

  if ( cmem___globals.dotime )
  {
    status = sys$gettim( &block_info_ptr->vms_time );
    if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1, "$GETTIM", status );
  }


/*
	Record the call stack for later display.  Unused entries in the array
	are filled with zeros so that the display routine will know when to
	stop.
*/

  if ( cmem___globals.dostack )
  {
    stack_index = 0;

#ifdef __ALPHA

    lib$get_curr_invo_context( &invocation );
    while ( lib$get_prev_invo_context( &invocation ) == 1 )
    {
      block_info_ptr->call_stack[ stack_index++ ]
		= invocation.libicb$q_program_counter[ 0 ];
      if ( stack_index >= MAX_CALL_DEPTH )
        break;
    }

#else

/*	Follow the frame pointer... */

#endif

    while ( stack_index < MAX_CALL_DEPTH )
      block_info_ptr->call_stack[ stack_index++ ] = 0;
  }    


/*
	That's it.  Re-enable ASTs if that was how it was configured when we
	were called.
*/

error_return:

  if ( previous_ast_state == SS$_WASSET )
    sys$setast( 1 );

  return( ( void * ) base_address );
}




void cmem_free( void * base_address )
{

  globalref int CMEM__FREEWRTFAIL;
  globalref int CMEM__FREEUNALLO;
  globalref int CMEM__UNEXPERR;


  int				status;
  unsigned char			previous_ast_state;
  unsigned char			found_it;
  struct cluster_info_struct *	cluster_ptr;
  int				block_index;
  int				previous_block_index;


/*
	Scan all the information blocks in all the clusters to see if we can
	find this address.
*/

  found_it = FALSE;
  cluster_ptr = cmem___globals.first_cluster_ptr;

  previous_block_index = END_OF_LIST;
  block_index = cluster_ptr->list_head[ cmem___hash_index( base_address ) ];

  while ( !found_it )
  {

/*
	If we're at the end of a linked list for a particular hash value, we
	need to move on to the next cluster (if any).
*/

    if ( block_index == END_OF_LIST )
    {
      if ( ( cluster_ptr = cluster_ptr->next_cluster_ptr ) == NULL )
	break;
      else
      {
	previous_block_index = END_OF_LIST;
        block_index =
		cluster_ptr->list_head[ cmem___hash_index( base_address ) ];
      }
    }

/*
	If the address in the info block matches what the user passed us,
	we've found the block.  Otherwise, keep on transversing the linked
	list.
*/

    else
    {
      if ( cluster_ptr->block_info[ block_index ].user_address == base_address )
	found_it = TRUE;
      else
      {
	previous_block_index = block_index;
	block_index = cluster_ptr->block_info[ block_index ].next_block;
      }
    }
  }


/*
	If we didn't find an entry in the tables, then the user is trying to
	release memory that was never allocated.  Give him a trace back dump
	to work with.
*/

  if ( !found_it )
  {
    lib$signal( ( int ) &CMEM__FREEUNALLO, 1, base_address );
  }

  else
  {

/*
	If this block is at the head of the list, we need to change the
	list head.  Otherwise we need to unlink it from the middle.
*/

    if ( previous_block_index == END_OF_LIST )
    {
      cluster_ptr->list_head[ cmem___hash_index( base_address ) ]
			= cluster_ptr->block_info[ block_index ].next_block;
    }
    else
    {
      cluster_ptr->block_info[ previous_block_index ].next_block
			= cluster_ptr->block_info[ block_index ].next_block;
    }


/*
	Now let's release the memory.  If we are using the "protected"
	model, we pass the address of the information block rather than
	the address of the allocated memory.  This prevents our user 'free'
	routine from having to scan through all the clusters and linked
	lists to find the block again.
*/

    if ( cluster_ptr->block_info[ block_index ].zone_id
					== cmem___globals.protected_zone )
      status = lib$free_vm(
	&cluster_ptr->block_info[ block_index ].requested_size,
	&cluster_ptr->block_info[ block_index ],
	&cluster_ptr->block_info[ block_index ].zone_id );
    else
      status = lib$free_vm(
	&cluster_ptr->block_info[ block_index ].requested_size,
	&cluster_ptr->block_info[ block_index ].user_address,
	&cluster_ptr->block_info[ block_index ].zone_id );

    if ( status == ( int ) &CMEM__FREEWRTFAIL )
    {
      lib$signal( status );
      cmem___display_block_info( &cluster_ptr->block_info[ block_index ] );
    }
    else if ( ( status & 1 ) != 1 )
      lib$stop( ( int ) &CMEM__UNEXPERR, 1, "LIB$FREE_VM", status );


/*
	Mark the block as no longer in use.
*/

    if ( cluster_ptr->available_block_count++ == 0 )
      cluster_ptr->next_available_block = block_index;

    cluster_ptr->block_in_use[ block_index ] = FALSE;
  }


error_return:

/*
	Restore the AST setting before exiting.  Any path out of this routine
	had better get to this location or else ASTs might not be restored.
*/

  if ( previous_ast_state == SS$_WASSET )
    sys$setast( 1 );

  return;
}




void * cmem_realloc( void * ptr, size_t size )
{


  int				temp;
  void *			new_pointer;


/*
	Go get the new space using our allocation routine.  If we are unable
	to allocate space, then return a NULL pointer.  The old space is
	still valid and is still pointed to by the pointer passed as the
	first argument to this routine.
*/

  new_pointer = cmem_malloc( size );
  if ( new_pointer == NULL )
    return( NULL );


/*
	If the first argument is a NULL pointer, then this routine acts just
	like 'malloc'.  Return a pointer to the uninitialized memory.
*/

  if ( ptr == NULL )
    return( new_pointer );


/*
	Copy the old memory to the new.  If the new block is larger than the
	old, the 'cmem_malloc' routine will have fill the extra space with
	our 0xAA check pattern.
*/

  if ( ( temp = cmem___get_block_size( ptr ) ) < size )
    ots$move3( temp, ptr, new_pointer );
  else
    ots$move3( size, ptr, new_pointer );


/*
	Release the old memory.  This will generate a traceback if the
	pointer wasn't to a valid block.  Return a pointer to the new
	space.
*/

  cmem_free( ptr );
  return( new_pointer );
}
