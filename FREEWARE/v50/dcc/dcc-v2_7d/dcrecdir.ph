/* DCRECDIR.PH */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifndef DCRECDIR_PH
#define DCRECDIR_PH

#include "dcrecdir.th"

/* Common functions */

/* Common objects */
extern bool condDirSkip;
extern TlineNb cumDeltaLineNb;  /* cumulated 'delta' introduced by #line dir. */
extern TlineNb cumNbSkippedLines;
extern TtokLvl getTokLvl;
extern TcharStream (*pNxtCh)(void);
extern TtokLvl tokLvl0;

#endif /* ifndef DCRECDIR_PH */
/* End DCRECDIR.PH */
