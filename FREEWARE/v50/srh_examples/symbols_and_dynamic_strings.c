#include <descrip.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <starlet.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stsdef.h>
#include <string.h>
main()
  {
  int RetStat;
  unsigned short int ValLen;

  $DESCRIPTOR( Symb, "FOO" );
  struct dsc$descriptor ValIn = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_D, NULL };
  struct dsc$descriptor ValOut = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_D, NULL };

  for ( ValLen = 128; ValLen < 1024; ValLen += 128 )
    {
    RetStat = lib$sget1_dd( &ValLen, &ValIn );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
      return RetStat;
    memset( ValIn.dsc$a_pointer, '*', ValLen );
    RetStat = lib$set_symbol( &Symb, &ValIn );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
      return RetStat;
    RetStat = lib$get_symbol( &Symb, &ValOut );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
      return RetStat;
    printf("\nNow serving number %d, length %d\n", 
      ValLen, ValOut.dsc$w_length );
    RetStat = lib$put_output( &ValOut );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
      return RetStat;
    }

  RetStat = lib$sfree1_dd( &ValIn );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;
  RetStat = lib$sfree1_dd( &ValOut );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
    return RetStat;

  return SS$_NORMAL;
  }
