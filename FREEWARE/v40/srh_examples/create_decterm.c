/*
* Usage is: mcr sys$login:create_decterm node::screen
*/
               
#include <descrip.h>     /* descriptor definitions */
#include <prcdef.h>      /* stsflg bits for creating process */
#include <ssdef.h>       /* system status codes */
#include <starlet.h>     /* system service definitions */
#include <stdio.h>       /* standard I/O library */
#include <stdlib.h>      /* standard library */

int DECwTermPort();      /* no prototype available */

main( int argc, char **argv )
  {
  char *display;
  int status, stsflg;
  short device_length;

  /* this must be exactly 50 characters */
  char device_name[50];

  $DESCRIPTOR( command, "SYS$SYSTEM:LOGINOUT.EXE" );
  $DESCRIPTOR( input_file, "" );
  $DESCRIPTOR( output_file, "" );

  /* first parameter is the display name */
  display = argv[1];

  /* send the message to the controller */
  status = DECwTermPort( display, 0, 0, device_name,
    &device_length );
  if ( status != SS$_NORMAL )
    printf( "DECterm creation failed, status is %x\n", status );
  else
    {
    /* create a process that is already logged in */
    /* input from TWn: */
    input_file.dsc$w_length = device_length;
    input_file.dsc$a_pointer = device_name;

    /* output to TWn: */
    output_file.dsc$w_length = device_length;
    output_file.dsc$a_pointer = device_name;

    /* make it detached, interactive, logged in */
    stsflg = PRC$M_DETACH | PRC$M_INTER | PRC$M_NOPASSWORD;

    /* create the process */
    status = sys$creprc( 0, &command, &input_file,
      &output_file, 0, 0, 0, 0, 0, 0, 0, stsflg );
    if ( status != SS$_NORMAL )
      printf( "Could not run LOGINOUT.EXE, status is %x\n", status );
    }
  }
