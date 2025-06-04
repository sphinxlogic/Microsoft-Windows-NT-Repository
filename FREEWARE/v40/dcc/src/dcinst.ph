/* DCINST.PH */
/* Y.L. Noyelle, Supelec, France 1998 */

#ifndef DCINST_PH
#define DCINST_PH

#include "dcinst.h"

typedef enum {NonRchbl_Jmp=-1 /* so that Rchbl==0, for efficiency */, Rchbl,
							   NonRchbl_Fwd} Trchbl;
struct _TnotInitVar {
  TsemanElt *ptrId;
  TnotInitVar *prec;
  TlineNb lineNb;
  Tstring fileName;
  bool initDPFl;
};

/* Functions profiles */
extern void errInit(TsemanElt *, TlineNb, Tstring, bool);
extern void initInst(void);

/* External objects */
extern uint condStmtLvl, initialLoopCondLvl, loopLvl;
extern Trchbl nxtStmtRchbl;  /* next statement reachable ? */

#endif /* ifndef DCINST_PH */
/* End DCINST.PH */
