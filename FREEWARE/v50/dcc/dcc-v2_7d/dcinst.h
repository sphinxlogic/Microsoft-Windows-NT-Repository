/* DCINST.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCINST_H
#define DCINST_H

#include "dc.th"

/* Functions profiles */
extern void checkIndent(void);
extern void checkNotInitVarAndSuppress(const TsemanElt *);
extern void enterFctBody(TpTypeElt);

#endif /* ifndef DCINST_H */
/* End DCINST.H */
