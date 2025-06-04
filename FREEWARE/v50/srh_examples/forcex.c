#module wake "V1.0-000"
#pragma builtins

/*
** COPYRIGHT (c) 1992 BY
** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
** ALL RIGHTS RESERVED.
**
** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
** TRANSFERRED.
**
** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
** CORPORATION.
**
** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
*/

/*
**++
**  Facility:
**
**	Examples
**
**  Version: V1.0
**
**  Abstract:
**
**	Performs a $WAKE on the specified target.  Set up to
**	be called as a foreign command.
**
**  Author:
**	Steve Hoffman
**
**  Creation Date:  1-Jan-1990
**
**  Modification History:
**--
*/

/*
/*  Example/test program that performs a $FORCEX on the specified
/*  process.  Called as a foreign command, with one argument, the
/*  hexidecimal PID of the target process.  May require GROUP or
/*  WORLD privilege.
*/

#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>

main( int argc, char **argv )
    {
    unsigned long int retstat;
    unsigned long int pid;
    unsigned long int finalstat = SS$_BUGCHECK;
    char hex[12];

    if (( argc != 2 ) && ( argc != 3 ))
	{
	printf("this is a foreign command; usage: fx pid exitstat\n");
	return SS$_BADPARAM;
	}

    sscanf( argv[1], "%x", &pid );
    if ( argc == 3 )
	sscanf( argv[1], "%x", &finalstat );

    retstat = sys$forcex( &pid, 0, finalstat );

    return retstat;
    }

