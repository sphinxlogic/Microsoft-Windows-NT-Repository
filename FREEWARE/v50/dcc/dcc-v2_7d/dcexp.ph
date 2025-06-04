/* DCEXP.PH */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCEXP_PH
#define DCEXP_PH

#include "dc.th"

#define BoolOpnd	(StrunPoss << 1)
#define CheckNumCstNamed(x,y)						       \
  if (x.LitCst) checkNumCstNamed(&x,y)
#define PtrPoss		DelType
#define StrunPoss	(PtrPoss << 1)

/* Function profiles */
extern void checkNumCstNamed(TresulExp *, TpcTypeElt);
extern void checkSideEffect(void);
extern void initExp(void);

#endif /* ifndef DCEXP_PH */
/* End DCINST.PH */
