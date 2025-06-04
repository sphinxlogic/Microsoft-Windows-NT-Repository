/* DCDECL.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCDECL_H
#define DCDECL_H

#include "dc.th"

#define CompatType(x, y, z) ((x == y)? True : compatType(x, y, z))
#define IsDecl1(x)      InsideInterval(curTok.tok, x, EDecl - 1)

typedef enum {NOINFO, NOTFCT=1, RESTYPFCT=NOTFCT<<1, MALLOCLIKEFCT=RESTYPFCT<<1,
						      SYSFCT=SIGN_BIT} TfctInfo;
typedef enum {VerifCast = -4, VerifCastF, StrictChkLsBnd, StrictChk, NoCheck,
	EqCmp, LitString, PrevCast, Asgn, FctCall, ExtDcl, IlgIcp, CCheck1,
	CCheck, CStrictChk, CStrChkExtDcl} TkTypeEquiv;
/*~ zif NoCheck != (TkTypeEquiv)0 "NoCheck must be zero (cf 'compatType')" */

typedef enum {D_ILLEGAL = -1 /* for efficiency */, D_LEGAL, D_NOINITLZ}
								     DeclStatus;
typedef enum {ERRTYPE=1, NOTPTR=ERRTYPE<<1, STRUN=NOTPTR<<1, ALLCONST=STRUN<<1,
					  NOTALLCONST=ALLCONST<<1} TresulTstPtd;

/* Function profiles */
extern TpTypeElt allocTypeEltIC(TpcTypeElt, Tqualif), allocTypeEltID(TtypeSort,
  Tqualif);
extern bool checkFrstMember(TpcTypeElt, TpTypeElt);
extern void checkPrivVisible(TpcTypeElt type);
extern bool compatType(TpTypeElt, TpTypeElt, TkTypeEquiv);
extern void decl(DeclStatus, const Ttok[]);
extern TpTypeElt declType(void);
extern TsemanElt *defineId(TinfoSeman);
extern TfctInfo fctInfo(TsemanElt *);
extern void freeTypeChain(TpTypeElt);
extern TpTypeElt freeTypeElt(TpTypeElt);
extern void freeTypes(TsemanElt *);
extern TpTypeElt getINDEXTYPEtype(void);
extern bool isRepreType(TpcTypeElt);
extern bool isSameName(ThCode, Tname, ThCode, Tname);
extern bool isType(void);
extern void makeGeneric(TpTypeElt);
extern void procExtent(void), procIndex(void), procMember1(void), procMember2(
  void);
extern void prog(void);
extern size_t sizeOfTypeD(TpTypeElt, Terr, Tname);
extern TresulTstPtd tstPtd(TpcTypeElt, bool);

/* External variables */
extern bool alrdAnalzdExp;
extern int difQual;
extern bool heedRootType;
extern TsemanElt *pIdInit;
extern Tname rowXName, rowYName;

#endif /* ifndef DCDECL_H */
/* End DCDECL.H */
