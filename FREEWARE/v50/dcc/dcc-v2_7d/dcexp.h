/* DCEXP.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCEXP_H
#define DCEXP_H

#include "dc.th"

#define FreeExpType(x)							       \
  FreeExpType1((x).type)
#define FreeExpType1(x)							       \
  ((x != NULL)? FreeExpType2(x) : (void)0)
#define FreeExpType2(x)							       \
  ((x->NoOwner)? freeTypeChain(x): (void)0)

typedef enum {NonRchbl_Jmp=-1 /* so that Rchbl==0, for efficiency */, Rchbl,
                                                           NonRchbl_Fwd} Trchbl;

/* Function profiles */
extern void asgnExpr(void);
extern bool boolExp(Tstring);
extern void checkInit(void);
extern void cleanExprThings(void);
extern void commaExpr(void);
extern bool compatNumType(void);
extern bool computeSigAdd(TcalcS, TcalcS, TcalcS *);
extern bool correctExprN(TpTypeElt, TtypeSort, bool, Tstring, bool);
extern void manageRetValue(TpTypeElt);
extern void valueNotUsed(void);

/* External objects */
extern bool evaluateFl;
extern TpcTypeElt *limErroTypes;  /* to prevent multiple (same) errors in
							     initializations. */
extern Trchbl nxtStmtRchbl;  /* next statement reachable ? */

#endif /* ifndef DCEXP_H */
/* End DCEXP.H */
