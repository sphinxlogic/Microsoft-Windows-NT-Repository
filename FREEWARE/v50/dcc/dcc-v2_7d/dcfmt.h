/* DCFMT.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCFMT_H
#define DCFMT_H

#include "dc.th"

#define ScanfFl		(ioFctNb > 0)

extern void initFlwgSpec(void);
extern bool folwngSpec(TpcTypeElt, TpcTypeElt);
extern void resetFlwgSpec(void);

extern int ioFctNb;

#endif /* ifndef DCFMT_H */
/* End DCFMT.H */
