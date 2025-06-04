/* DCRECDIR.C */
/* Y.L. Noyelle, Supelec, France 1996 */

#include "dcrecdir.ph"
#include "dcmain.h"

bool condDirSkip = False;
uint ctrCSE = 0;	/* number of conditional directive stack elements */
uint ctrMSE = 0;	/* number of macro stack elements */
TlineNb cumDeltaLineNb = 0;  /* due to '#line' directive */
uint cumNbSkippedLines = 0;  /* count of lines skipped by #if/#endif */
uint dpragNst = 0;	/* if non zero, currently processing a d-pragma */
uint getTokLvl;	     /* counter to detect 'real' (level 0) calls to GetNxtTok */
bool listTok = False;	/* flag 'list last tokens' on error */
TcharStream (*pNxtCh)(void) = &nxtChFromTxt;

/* End DCRECDIR.C */
