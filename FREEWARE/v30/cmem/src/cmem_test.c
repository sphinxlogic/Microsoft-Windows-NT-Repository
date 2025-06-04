/*
**++
**  FACILITY:  CMEM  V1.1
**
**
**  MODULE DESCRIPTION:
**
**	This module contains a mainline routine to test the CMEM facility.
**
**	The following routines are defined in this module:
**
**	    main
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
**	Just a quick check to make sure things are working.
**
**
**  MODIFICATION HISTORY:
**
**      4-May-1995	B. Hunsaker	Initial implementation
**
**	This is a test program to check out the various CMEM routines.
**
**
**--
*/

/*
**
**  ENVIRONMENT SETTINGS
**
*/


/*
**
**  GLOBAL CONSTANTS
**
*/


/*
**
**  INCLUDE FILES
**
*/

#include <stdlib.h>
#include <stdio.h>


/*
**
**  GLOBAL STRUCTURES
**
*/


/*
**
**  GLOBAL VARIABLES
**
*/




main()
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

  extern void cmem_show_blocks( void );
  extern int cmem_collect_stack( unsigned char state );
  extern int cmem_collect_time( unsigned char state );
  extern int cmem_boundary_check( unsigned char state );
  extern int cmem_protect_freed( unsigned char state );
  extern int cmem_reset_display_flags( void );


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

  char * dummy_ptr;
  char * new_dummy_ptr;


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
	Actually, on a VAX, we will display the INIT message before this
	if we link with the object library and not the shareable image.
*/

  printf( "==> Display CMEM__INIT and CMEM__STACKENA messages...\n" );
  cmem_collect_stack( 1 );


/*
	We disable collection of time stamps.  This way, the output from
	this run can be compared with a previous run to see if they are the
	same.  If time stamp collection was enabled, the output would have
	different times displayed.
*/

  printf( "==> Display CMEM__TIMEDIS messages...\n" );
  cmem_collect_time( 0 );

  printf( "==> Display CMEM__BNDRYPRTDIS message...\n" );
  cmem_boundary_check( 0 );

  printf( "==> Display CMEM__RELMEMREUSE message...\n" );
  cmem_protect_freed( 0 );

/*
	On the VAX, we've probably had some interference from some of the
	C RTL initialization code.  So, mark them not to be displayed.
*/

  cmem_reset_display_flags();


  dummy_ptr = ( char * ) malloc( 12 );
  printf( "==> Show a 12 byte block...\n" );
  cmem_show_blocks( );

  printf( "==> Display CMEM__NOBNDYPRT message...\n" );
  cmem_protect_freed( 1 );

  printf( "==> Display CMEM__BNDRYPRTENA message...\n" );
  cmem_boundary_check( 1 );

  new_dummy_ptr = ( char * ) realloc( ( void * ) dummy_ptr, 24 );
  printf( "==> Show a 24 byte block...\n" );
  cmem_show_blocks( );

  free( new_dummy_ptr );
  printf( "==> Show all blocks have been released...\n" );
  cmem_show_blocks( );

  printf( "==> Report attempt to deallocate unallocated memory...\n" );
  free( new_dummy_ptr );
}
