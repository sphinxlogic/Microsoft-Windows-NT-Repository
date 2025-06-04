/*
//
//  Abstract:
//
//        Simple routine to act as an OBJECT module for input
//        as a debugger.  Its goal in life is to clear the debug
//        flags unless it finds the logical name of IMAGE$DEUG to
//        be set (to anything) in the LNM$PROCESS table.  Then it 
//        will invoke the real VMS image startup.
//
//	$ CC FAKE_DEBUG
//	$ LINK /DEBUG = FAKE_DEBUG object-code
//
*/

#include <descrip.h>
#include <lib$routines.h>
#include <lnmdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <stsdef.h>

static $DESCRIPTOR(table_name, "LNM$PROCESS");
static $DESCRIPTOR(logical, "IMAGE$DEBUG");

int debugger( int (*start)(), void *cli_coroutine, char *image_header,
                        int opt1, int lnk_flags, int cli_flags )
  {

  int status, (*image_startup)();

#ifdef __alpha
  /*
  //  The following is necessary to prevent the LINKER from making
  //  SYS$IMGSTA an entry point into the image -- we want to be the
  //  entry point.  Not necessary on OpenVMS Alpha LINKER.
  //
  //  OpenVMS Alpha: do the right relocation 
  */
  int sys$imgsta() ;
  image_startup = sys$imgsta;
#else
  /* 
  //  OpenVMS VAX: Fool the LINKER into NOT including trace 
  */
  image_startup = 0x7FFEDF68;
#endif

  /*
  //  See if the user defined the logical.
  */
  status = sys$trnlnm( 0, &table_name, &logical, 0, 0);
  if (!$VMS_STATUS_SUCCESS( status )) 
    {
    lnk_flags &= (~1);  /* Clear the link debug flag IHD$V_LNKDEBUG */
    cli_flags &= (~3);  /* Clear the CLI$V_DEBUG and DBGTRU flags */
    }

  /*
  //  Start up the image the OpenVMS way ...
  */
  return (*image_startup)( start, cli_coroutine, image_header,
     opt1, lnk_flags, cli_flags ) ;

  }
