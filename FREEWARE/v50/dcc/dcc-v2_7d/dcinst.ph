/* DCINST.PH */
/* Y.L. Noyelle, Supelec, France 1998 */

#ifndef DCINST_PH
#define DCINST_PH

#include "dc.th"

/* Functions profiles */
extern void addNotInitVarList(TsemanElt *, bool);
extern void errInit(TsemanElt *, TlineNb, Tstring, bool);
extern void initInst(void);

/* External objects */
extern TnstLvl condStmtLvl;  /* zero as long as control flow has to go
								through here. */
extern TnstLvl initialLoopCondLvl;
extern bool longjmpTaken;
extern uint loopLvl;
extern TnstLvl setjmpBlkLvl;
extern bool setjmpSeen;

#endif /* ifndef DCINST_PH */
/* End DCINST.PH */
