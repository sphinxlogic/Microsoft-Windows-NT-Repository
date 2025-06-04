/* DCDIR.PH */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCDIR_PH
#define DCDIR_PH
#include "dcrecdir.th"

/* Functions profiles */
extern void checkCondStkAndDeleteMacros(void);
extern void expandMac(void);
extern void manageDir(void);
extern TcharStream nxtChFromMac(void);
extern void restoQuoState(void);

/* Common objects */
extern bool peepTok;
extern TdescrId *pNxtMac;

#endif  /* ifndef DCDIR_PH */
/* End DCDIR.PH */
