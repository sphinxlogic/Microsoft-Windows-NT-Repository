/* DCRECDIR.PH */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifndef DCRECDIR_PH
#define DCRECDIR_PH

#include "dcrecdir.th"

/* Common functions */

/* Common objects */
extern bool condDirSkip;
extern uint ctrCSE;  /* number of conditional directive stack elements */
extern uint ctrMSE;  /* number of macro stack elements */
extern TlineNb cumDeltaLineNb;  /* cumulated 'delta' introduced by #line dir. */
extern uint cumNbSkippedLines;
extern uint dpragNst;
extern uint getTokLvl;
extern bool listTok;
extern TcharStream (*pNxtCh)(void);

#endif /* ifndef DCRECDIR_PH */
/* End DCRECDIR.PH */
