/* DCMAIN.H */
/* Y.L. Noyelle, Supelec, France 1998 */

#ifndef DCMAIN_H
#define DCMAIN_H

#include "dc.th"

/* Function profiles */
extern void emitS(Tstring);
extern void err(Terr, const Tstring[]);
extern bool insideInclude(void);
extern TcharStream nxtChFromTxt(void);
extern void sysErr(Tstring) /*~NeverReturns*/;

/* External objects */
extern int tabSpacing;

#endif  /* ifndef DCMAIN_H */
/* End DCMAIN.H */
