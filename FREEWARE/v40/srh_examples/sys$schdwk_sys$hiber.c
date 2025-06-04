#include <descrip.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>
#include <stsdef.h>
#include <lib$routines.h>


int main(int argc, char **argv )
  {
  int RetStat;
  $DESCRIPTOR( DateTimeText, "0 ::10");
  int DateTimeQuad[2];

  RetStat = sys$bintim( &DateTimeText, DateTimeQuad );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    lib$signal( RetStat );

  RetStat = sys$schdwk( 0, 0, DateTimeQuad, 0 );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    lib$signal( RetStat );

  printf("Commencing Hibernation\n");

  RetStat = sys$hiber();
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    lib$signal( RetStat );

  printf("Hibernation Completed\n");
          
  return SS$_NORMAL;
  }
