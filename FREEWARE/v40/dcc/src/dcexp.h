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
  ((x->noOwner)? freeTypeChain(x): (void)0)

/* Function profiles */
extern void asgnExpr(void);
extern bool boolExp(Tstring);
extern void checkInit(void);
extern void cleanExprThings(void);
extern void commaExpr(void);
extern bool computeSigAdd(TcalcS, TcalcS, TcalcS *);
extern bool correctExprN(TpcTypeElt, TtypeSort, bool, Tstring,
							      bool checkNumCst);
extern void manageRetValue(TpcTypeElt);
extern void procSameType(void);

/* External objects */
extern TpcTypeElt *limErroTypes;  /* to prevent multiple (same) errors in
							     initializations. */
#endif /* ifndef DCEXP_H */
/* End DCEXP.H */
