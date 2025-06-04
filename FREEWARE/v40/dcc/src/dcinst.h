/* DCINST.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCINST_H
#define DCINST_H

#include "dc.th"

typedef struct _TnotInitVar TnotInitVar;
extern TnotInitVar *notInitVarList;

/* Functions profiles */
extern void checkIndent(void);
extern void checkNotInitVarAndSuppress(const TsemanElt *);
extern void enterFctBody(TpcTypeElt);

#endif /* ifndef DCINST_H */
/* End DCINST.H */
