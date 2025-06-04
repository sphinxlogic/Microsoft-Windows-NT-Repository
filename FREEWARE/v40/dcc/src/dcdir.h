/* DCDIR.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCDIR_H
#define DCDIR_H
#include "dc.th"

#define InsideMacPar	(! (macParCtr & 1))
#define InsideMacro	(curMacExpNb != 0)
#define ReallyInsideMacro (InsideMacro && !aloneInNoParMac(False))

typedef uint TmacParCtr;

extern bool aloneInNoParMac(bool);
extern bool checkSColAtEndMac(void);
extern bool checkSpeFct(void);
extern Tstring curMacDFName(void);
extern bool embdMacro(void);
extern TcharTok nxtCharOrMacTok(void);
extern bool visibleFromMac(const TsemanElt *);

extern bool adjMacro;
extern TmacExpnd begMacExpNb;
extern bool insideMultUsedMacPar;
extern TlineNb lastSCOLline;
extern TmacLvl macLvl;
extern TmacParCtr macParCtr;
extern bool sysMacro;

#endif  /* ifndef DCDIR_H */
/* End DCDIR.H */
