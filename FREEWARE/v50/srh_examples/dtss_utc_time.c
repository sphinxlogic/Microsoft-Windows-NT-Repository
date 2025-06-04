#include <stdio.h>
//
//  An example of returning the current Zulu/GMT/UTC time in text format
//
//  Universal Coordinated Time (UTC) can effectively be treated as GMT.
//  (UTC reportedly apparently has a slightly different definition than 
//  GMT, but I'll ignore the details of the difference(s) here.)
//
//  The TDF is the Time Differential Factor, and is the difference between 
//  the local time and UTC.
//
//  This example uses the UTC calls in the DTSS$SHR image -- these routines
//  are present on recent versions of OpenVMS; Documentation is included with 
//  DECnet-Plus DTSS/DECdtss -- to retrieve the current time in UTC format 
//  is attached below, as well as the output from an example run and a 
//  subsequent (manual) SHOW TIME and supporting information.  
//
//  This was built on a DECnet Phase IV system running OpenVMS Alpha V7.2-1
//  and Compaq C V6.2.
//
//  Build and invocation instructions, and related commands:
//
//    $ cc x
//    $ link x,sys$input:/opt
//    sys$share:dtss$shr/share
//    $ run x
//    Formatted UTC time is 2000-10-12-22:14:43.980I-----
//    $ sho time
//      12-OCT-2000 18:14:45
//    $ show logical SYS$TIMEZONE_DIFFERENTIAL
//       "SYS$TIMEZONE_DIFFERENTIAL" = "-14400" (LNM$SYSTEM_TABLE)
//    $ cc/vers
//    Compaq C V6.2-006 on OpenVMS Alpha V7.2-1
//    $
//
//  Consider using the utc.h header file, if you have it.
//  #include <utc.h>
//  As utc.h might not be available, I'll define the necessary parts here.
//
//
typedef struct utc
  {
  char char_array[16];
  } utc_t;
extern utc_gettime(void*);
extern utc_mkasctime(void*,void*);
extern utc_ascgmtime(void*,int,void*);
main()
  {
  utc_t Utc;
  char AscTim[35];
  utc_gettime(&Utc);
  utc_ascgmtime(AscTim, 35, &Utc );
  printf("Formatted UTC time is %s\n",AscTim);
  return 1;
  }
