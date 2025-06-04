/*
//  Demonstrate CTRL/Y handling under OpenVMS, as well as some basic
//  dynamic string descriptor operations and a few other string-related 
//  operations...
//
//  Stephen Hoffman, Compaq Computer Corporation, 15-Dec-1998
//  
//  To build and use:
//    $ CC/DECC LIB$XXABLE_CTRL
//    $ LINK LIB$XXABLE_CTRL
//    $ RUN LIB$XXABLE_CTRL
*/
#include <descrip.h>
#include <iodef.h>
#include <libclidef.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>
#include <stsdef.h>


void CtrlYHandler()
  {
  int RetStat;
  $DESCRIPTOR( Y, "<CTRL/Y> was detected" );
  RetStat = lib$put_output( &Y );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return;
  RetStat = lib$enable_ctrl( &LIB$M_CLI_CTRLY );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return;
  return;
  }

main()
  {
  int RetStat;
  unsigned short int IOChan;
  unsigned short int GotLen;
  struct dsc$descriptor GotDsc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_D, NULL };
  $DESCRIPTOR( Prompt, "Enter <CTRL/Y>, or any characters and <RETURN>:" );
  $DESCRIPTOR( Exiting, "Exiting" );
  $DESCRIPTOR( TTDsc, "TT:");

  RetStat = lib$disable_ctrl( &LIB$M_CLI_CTRLY );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = sys$assign( &TTDsc, &IOChan, 0, 0 );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = sys$qiow( 0, IOChan, IO$_SETMODE|IO$M_CTRLYAST, 0, 0, 0,
    CtrlYHandler, 0, 0, 0, 0, 0 );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = lib$get_input( &GotDsc, &Prompt, &GotLen );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = sys$dassgn( IOChan );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = lib$enable_ctrl( &LIB$M_CLI_CTRLY );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = lib$put_output( &Exiting );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = lib$sfree1_dd( &GotDsc );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  return SS$_NORMAL;
  }

