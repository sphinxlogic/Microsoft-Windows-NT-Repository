/* DCRECDIR.C */
/* Y.L. Noyelle, Supelec, France 1996 */

#include "dcrecdir.ph"
#include "dcmain.h"

bool condDirSkip = False;
TlineNb cumDeltaLineNb = 0;  /* due to '#line' directive */
TlineNb cumNbSkippedLines = 0;  /* count of lines skipped by #if/#endif */
TtokLvl getTokLvl;   /* counter to detect 'real' (level 0) calls to GetNxtTok */
TcharStream (*pNxtCh)(void) = &nxtChFromTxt;
TtokLvl tokLvl0;     /* level of 'real' (level 0) calls to GetNxtTok */

/* End DCRECDIR.C */
