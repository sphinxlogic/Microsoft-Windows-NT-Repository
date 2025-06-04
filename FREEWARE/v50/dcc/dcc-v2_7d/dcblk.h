/* DCBLK.H */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifndef DCBLK_H
#define DCBLK_H

#include "dcrecdir.th"

/* Function profiles */
extern TnstLvl baseNstLvl(void);
extern Tname curIdName(void);
extern void enterBlock(void);
extern TsemanElt *enterSymTab(Tname), *enterSymTabHC(Tname, ThCode);
extern void exitBlock(void);
extern TlitString getLitString(void);
extern TsemanElt *initGetNxtIdInCurBlk(void);
extern size_t initGetStrLit(TlitString) /*~PseudoVoid*/;
extern TsemanElt *nxtId(void);
extern char nxtStrLitChar(void) /*~Generic*/;
extern TlitString ptrFreeIdSpace(void);
extern void resetGetStrLit(void);
extern TsemanElt *searchSymTab(Tname), *searchSymTabHC(Tname, ThCode hCode);
extern Tname storeLabelName(Tname), storeName(Tname, TnameSpace);
extern void storeStrCh(char);

#endif  /* ifndef DCBLK_H */
/* End DCBLK.H */
