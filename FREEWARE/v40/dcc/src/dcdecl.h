/* DCDECL.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCDECL_H
#define DCDECL_H

#include "dc.th"

typedef enum {ERRTYPE=1, NOTPTR=ERRTYPE<<1, STRUN=NOTPTR<<1, ALLCONST=STRUN<<1,
					  NOTALLCONST=ALLCONST<<1} TresulTstPtd;

/* Function profiles */
extern TpTypeElt allocTypeEltIC(TpcTypeElt, Tqualif), allocTypeEltID(TtypeSort,
  Tqualif);
extern void checkPrivVisible(TpcTypeElt type);
extern bool compatType(TpcTypeElt, TpcTypeElt, TkTypeEquiv);
extern void decl(void);
extern TpcTypeElt declType(void);
extern TsemanElt *defineId(TinfoSeman);
extern void freeTypeChain(TpcTypeElt);
extern TpcTypeElt freeTypeElt(TpcTypeElt /*~MayModify*/);
extern void freeTypes(TsemanElt *);
extern TpcTypeElt getINDEXTYPEtype(void);
extern bool isRepreType(TpcTypeElt);
extern void procExtent(void), procIndex(void), procMember1(void), procMember2(
  void);
extern void prog(void);
extern size_t sizeOfTypeD(TpcTypeElt /*~MayModify*/, Terr, Tname);
extern TresulTstPtd tstPtd(TpcTypeElt, bool);

/* External variables */
extern bool heedRootType;

#endif /* ifndef DCDECL_H */
/* End DCDECL.H */
