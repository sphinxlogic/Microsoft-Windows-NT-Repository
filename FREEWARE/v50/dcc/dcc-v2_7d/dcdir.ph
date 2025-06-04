/* DCDIR.PH */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCDIR_PH
#define DCDIR_PH
#include "dcrecdir.th"

/* Functions profiles */
extern void checkCondStkAndDeleteMacros(void);
extern void expandIfMac(void);
extern void initDir(void);
extern void manageDir(void);
extern TcharStream nxtChFromMac(void);
extern void restoPnxtChState(void);
extern bool transmitDP(void);

/* External variables */
extern uint ctrCSE;  /* number of conditional directive stack elements */
extern bool gblArgColl;
extern bool insideDefineBody;
extern bool noExpand;

#endif  /* ifndef DCDIR_PH */
/* End DCDIR.PH */
