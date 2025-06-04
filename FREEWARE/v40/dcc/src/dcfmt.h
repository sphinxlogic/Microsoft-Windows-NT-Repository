/* DCFMT.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCFMT_H
#define DCFMT_H

#include <stddef.h>
#include "dc.th"

#define ScanfFl		(ioFctNb > 0)

extern void initFlwgSpe(void);
extern bool folwngSpe(TpcTypeElt);

extern int ioFctNb;  /* >=0 if 'scanf' family */

#endif /* ifndef DCFMT_H */
/* End DCFMT.H */
