/* DCMAIN.H */
/* Y.L. Noyelle, Supelec, France 1998 */

#ifndef DCMAIN_H
#define DCMAIN_H

#include "dc.th"

#define IndicatedFName(x)   (x+(ubyte)*(x - PosLgtAddedPath))  /* included files
				names are prefixed with length of added access
				path (cf. dirArray/copyDirPrefix() ). */
#define LgtHdrAddedPath	PosCmpsgFlg
#define PosCmpsgFlg	(PosLgtAddedPath + sizeof(TdirChar))
#define PosLgtAddedPath	sizeof(TdirChar)

typedef ubyte TdirChar;

/* Function profiles */
extern void emitS(Tstring);
extern void err(Terr, const Tstring[]);
extern bool insideIncldFile(void);
extern TcharStream nxtChFromTxt(void);
extern void sysErr(Tstring) /*~NeverReturns*/;

/* External objects */
extern TindentChk tabSpacing;

#endif  /* ifndef DCMAIN_H */
/* End DCMAIN.H */
