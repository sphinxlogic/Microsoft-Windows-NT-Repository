/*
 *	Timer - A VMS version of waiting ...
 *
 *	NOTE: This is provided for use with X11 programs
 *	ported to VMS. It is NOT a UNIX system call.
 */

#include <time.h>
#include <timeb.h>
#include <ctype.h>
#include <lib$routines.h>

Timer(val)
long val;
{
    float seconds;

    seconds = (float) val;
    seconds = seconds / 1000000.0;
    lib$wait(&seconds);

} /*** End Timer() ***/

