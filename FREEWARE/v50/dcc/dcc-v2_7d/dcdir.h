/* DCDIR.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCDIR_H
#define DCDIR_H
#include "dc.th"

#define InsideMacPar	(macParCtr & 1)
#define InsideMacro	(curMacExpNb != 0)
#define ReallyInsideMacro (InsideMacro && !aloneInNoParMac())

typedef uint TmacParCtr;

extern bool aloneInNoParMac(void);
extern bool checkSColAtEndMac(void);
extern bool checkSpeFct(void);
extern Tstring curMacDFName(void);
extern void errMacBefIncl(void);
extern bool hereOrAloneInMac(void);
extern Ttok peepNxtTok(void);
extern bool visibleFromMac(const TsemanElt *);

extern bool adjMacro;
extern TmacExpNb begMacExpNb;
extern bool insideMultUsedMacPar;
extern TlineNb lastSCOLline;
extern TmacLvl macLvl;
extern TmacParCtr macParCtr;
extern bool sysMacro;

#endif  /* ifndef DCDIR_H */
/* End DCDIR.H */
