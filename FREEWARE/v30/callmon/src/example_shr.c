/*  CALLMON Examples
 *
 *  File:     EXAMPLE_SHR.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: Content of a shareable image. The routine receives more
 *            than 6 arguments (both register and stack arguments).
 */


#include <stdio.h>


int shr_proc (
    int    a1,
    double a2,
    int    a3,
    double a4,
    int    a5,
    double a6,
    int    a7,
    double a8,
    int    a9,
    double a10)
{
    printf ("shr_proc: a1 = %d,  a2  = %f\n"
            "          a3 = %d,  a4  = %f\n"
            "          a5 = %d,  a6  = %f\n"
            "          a7 = %d,  a8  = %f\n"
            "          a9 = %d,  a10 = %f\n"
            "          return 5\n",
            a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);

    return 5;
}
