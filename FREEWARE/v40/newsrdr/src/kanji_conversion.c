/*
!++
!
! MODULE:   	    KANJI_CONVERSION
!
! FACILITY: 	    NEWSRDR
!
! ABSTRACT: 	    JIS/DEC Kanji character conversion routines.
!
! MODULE DESCRIPTION:
!
!   This module contains routines for use by NEWSRDR that convert
!   character strings between DEC Kanji and JIS Kanji character sets.
!   To use these routines with NEWSRDR, compile this module and link
!   it with the commands:
!
!   	$ link/share/notrace kanji_conversion,sys$input:/opt
!   	    universal=network_to_local,local_to_network            (VAX)
!           symbol_vector=(network_to_local=procedure,-
!                          local_to_network=procedure)             (Alpha)
!   	    <ctrl/Z>
!
!   The UNIVERSAL= linker directive is used only on VAX systems; the
!   SYMBOL_VECTOR= directive is used only on Alpha systems.
!
!   Then copy it to SYS$LIBRARY and make it available with the commands:
!
!   	$ copy kanji_conversion.exe sys$common:[syslib]/protection=w:re
!   	$ install create sys$share:kanji_conversion/share/open/header
!   	$ define/system/exec newsrdr_character_conversion -
!   	    	sys$library:kanji_conversion
!
!   (You need a suitably privileged account to do this.)
!
! AUTHOR:   	    M. Madison
!   	    	    COPYRIGHT © 1992, MADGOAT SOFTWARE.
!
! CREATION DATE:    12-NOV-1990
!
! MODIFICATION HISTORY:
!
!   12-NOV-1990	V1.0	Madison	    Initial coding.
!   07-MAR-1991	V1.0-1	Togawa	    Fix handling of control characters.
!   12-SEP-1992	V2.0	Madison	    Update for NEWSRDR 4.0.
!--
*/

#include descrip
#include str$routines

#define STATE_ASCII 0
#define STATE_AK    1
#define STATE_AKK   2
#define STATE_KANJI 3
#define STATE_KK    4
#define STATE_KA    6
#define STATE_KAA   7
#define STATE_STOP  9

#define append(c)   {if (outc-out < outsize-2) *outc++ = (c);}

void
network_to_local (char *in, char *out, int outsize) {

    char *inc, *outc;
    int state;

    state = STATE_ASCII;
    inc = in;
    outc = out;

    for (inc = in; *inc; inc++) {
    	switch (state) {
    	    case STATE_ASCII:
    	    	if (*inc == 27) state = STATE_AK;
    	    	else append(*inc);
    	    	break;
    	    case STATE_AK:
    	    	if (*inc == 36) state = STATE_AKK;
    	    	else {append(27); append(*inc); state = STATE_ASCII;}
    	    	break;
    	    case STATE_AKK:
    	    	if (*inc == 66 || *inc == 64) state = STATE_KANJI;
    	    	else {state = STATE_ASCII; append(27); append(36); append(*inc);}
    	    	break;
    	    case STATE_KANJI:
    	    	if (*inc == 27) state = STATE_KA;
    	    	else if (*inc <= 32) {append(*inc);}	/* added by H.Togawa */
    	    	else {state = STATE_KK; append(*inc | 0x80);}
    	    	break;
    	    case STATE_KK:
    	    	append (*inc | 0x80);
    	    	state = STATE_KANJI;
    	    	break;
    	    case STATE_KA:
    	    	if (*inc == 40) state = STATE_KAA;
    	    	else {state = STATE_KANJI; append(27); append(*inc);}
    	    	break;
    	    case STATE_KAA:
    	    	if (*inc == 72 || *inc == 66 || *inc == 74) state = STATE_ASCII;
    	    	else {append(27); append(40); append(*inc); state = STATE_KANJI;}
    	    	break;
    	}
    }

    *outc = '\0';

} /* network_to_local */

void
local_to_network (char *in, char *out, int outsize) {

    char *inc, *outc;
    int state;

    state = STATE_ASCII;
    inc = in;
    outc = out;

    for (inc = in; *inc; inc++) {
    	if (state == STATE_ASCII && *inc < 0) {
    	    state = STATE_KANJI;
    	    append(27); append(36); append(64);
    	} else if (state == STATE_KANJI && *inc > 0) {
    	    state = STATE_ASCII;
    	    append(27); append(40); append(74);
    	}
    	append(*inc & 0x7f);
    }

    if (state == STATE_KANJI) {			/* added by H.Togawa	    */
    	state = STATE_ASCII;			/* The mode should be ASCII */
    	append(27); append(40); append(74);	/* when terminates.	    */
    }						/*			    */

    *outc = '\0';

} /* local_to_network */
