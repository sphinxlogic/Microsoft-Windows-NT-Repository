#ifndef __TPADEF_LOADED
#define __TPADEF_LOADED	1

/*** MODULE $tpadef ***/
/*                                                                          */
/* TPARSE CONTROL BLOCK. THIS BLOCK IS ONE OF THE ARGUMENTS USED TO CALL TPARSE, */
/* AND BECOMES THE ARGUMENT LIST TO ACTION ROUTINES.                        */
/*                                                                          */
#define TPA$K_COUNT0 8                  /* ZERO LEVEL VALUE IS 8            */
#define TPA$M_BLANKS 1
#define TPA$V_BLANKS 0
#define TPA$M_ABBREV 2
#define TPA$V_ABBREV 1
#define TPA$M_ABBRFM 4
#define TPA$V_ABBRFM 2
#define TPA$M_AMBIG 65536
#define TPA$V_AMBIG  16
#define TPA$K_LENGTH0 36                /* LENGTH OF ZERO LEVEL PARAMETER BLOCK  */
#define TPA$C_LENGTH0 36                /* LENGTH OF ZERO LEVEL PARAMETER BLOCK  */

struct tpadef {
    unsigned int  tpa$l_count;      /* ARGUMENT COUNT (NUMBER OF LONGWORDS)  */
    unsigned int  tpa$l_options; /* OPTIONS LONGWORD                */
    unsigned int  tpa$l_stringcnt;  /* BYTE COUNT OF STRING BEING PARSED  */
             char *tpa$l_stringptr;  /* ADDRESS OF STRING BEING PARSED   */
    unsigned int  tpa$l_tokencnt;   /* BYTE COUNT OF CURRENT MATCHING TOKEN  */
             char *tpa$l_tokenptr;   /* ADDRESS OF MATCHING TOKEN        */
             char tpa$b_char;
    char tpadef$$_fill_3[3];
    unsigned int  tpa$l_number;     /* NUMERIC VALUE OF NUMERIC TOKEN   */
    unsigned int  tpa$l_param;      /* PARAMETER LONGWORD FROM STATE TABLE  */
    } ;
 
#endif					/* __TPADEF_LOADED */
