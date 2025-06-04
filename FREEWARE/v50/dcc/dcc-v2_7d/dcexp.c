/* DCEXP.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#if defined(__linux__) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE  /* pour LONG_LONG_MIN */
#endif
#include <string.h>
#include <ctype.h>
#include "dcexp.h"
#include "dcexp.ph"
#include "dcblk.h"
#include "dcdecl.h"
#include "dcdir.h"
#include "dcext.h"
#include "dcfmt.h"
#include "dcinst.h"
#include "dcinst.ph"
#include "dcprag.h"
#include "dcrec.h"
#include "dctxttok.h"

#ifdef VMS
#pragma noinline (arrToPtr, checkNumCstNamed, checkPureBool, errSinkType)
#pragma noinline (errCompoOf, errExp, errOvfl, errUdfl, errWrngType)
#pragma noinline (finalizeCast, freeAlloc, freePrevCast, ignorableDP)
#pragma noinline (managePredefTypes, manageSetjmp, processBitOper)
#pragma noinline (processLogOper, transfOpnd, typeToS2NoQual, warnCstBool)
#pragma noinline (warnNotPureBool, warnOrErr)
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
#  pragma noinline (manageLeftBits)
#  endif
#endif

#define ArbitAdr	QuasiNULLval(char *)  /* arbitrary */
#define CheckSink(pExp, sinkType)					       \
  ((pExp)->Revlbl && sinkType!=NULL && sinkType->LitCsta)
#define DefLastPrimMacPar(x)						       \
  do {									       \
    lastPrimMacParCtrIM = 0;						       \
    lastPrimMacParCtr = x;						       \
    if (insideMultUsedMacPar) lastPrimMacParCtrIM = x;			       \
  } while (False)
#define IsSideEffect(x)	InsideInterval(NakedTok(x.topOper), BSideEffTok,       \
							      ESideEffTok - 1)
#define IsWhoEnumNotChar(x)						       \
  (IsTypeSort(x, Who) || IsTypeSort(x, Enum) && x->TagId!=NULL)
#define NakedTok(x)	(x & ~(PARENSEEN | PRIMARY | SETJMPUSED))
#define NoResulTyp	QuasiNULLval(TpTypeElt)
#define PtrdiffTHCode	AdaptHCode(0x180904D)
#define PtrdiffTName	"\x9\0ptrdiff_t"
#define PureUnsig(t)	(t->typeSort & UnsigWho)
#define SetInfoForCst							       \
  cExp.Revlbl = True;							       \
  if (! evaluateFl) cExp.ErrEvl = True;  /* to prevent computation */
#define SizeTHCode	AdaptHCode(0x321B8)
#define SetjmpHCode	AdaptHCode(0x31539)
#define SetjmpTName	"\6\0setjmp"
#define SizeTName	"\6\0size_t"
#define SubOP		(ADDOP | NONASSOC)
#define TstAlwdInCstExp() if (cstExpAsked && evaluateFl) err0(OperIlgInCstExp)
#define WcharTHCode	AdaptHCode(0x102511)
#define WcharTName	"\7\0wchar_t"
#define XAllocSeen	QuasiNULLval(TpTypeElt)

DefRepresType  /* define function 'represType' */

typedef enum {Composite = -1, NotMember, Member, SingleMember} Tmember;
typedef enum {WCHAR_T, SIZE_T, PTRDIFF_T} TpredefTypes;

/* Function profiles */
static TpTypeElt arrToPtr(TpTypeElt);
static void condExpr(void), term0(void), term1(void), term2(void), term3(void),
  term4(void), term5(void), term6(void), term7(void), term8(void), term9(void),
  term10(void);
static void computeCond(TpcTypeElt), computeShortCircuit(TpcTypeElt),
  computeIor(TpcTypeElt), computeXor(TpcTypeElt), computeAnd(TpcTypeElt),
  computeCmp(TpcTypeElt), computeShi(TpcTypeElt), computeAdd(TpcTypeElt),
  computeSub(TpcTypeElt), computeMul(TpcTypeElt), computeDiv(TpcTypeElt),
  computeMod(TpcTypeElt);
static bool authzdType(TpcTypeElt, TtypeSort, bool *, Terr),
  errSinkType(Terr, Tstring) /*~PseudoVoid*/,
  errWrngType(TtypeSort) /*~PseudoVoid*/, frstMemberCompat(TpcTypeElt,
  TtypeSort), ignorableDP(TmacParCtr), manageInitlzUsed(void) /*~PseudoVoid*/,
  manageTypCmbn(TkOpTC) /*~PseudoVoid*/, resulExp(TtypeSort, bool,
  void(*)(TpcTypeElt), TmacLvl, Ttok) /*~PseudoVoid*/;
static void castValue(TtypeSort), checkIncldFiles(const TsemanElt *),
  checkInfoLoss(TpcTypeElt), checkPureBool(void),
  createBoolResult(void), errCompoOf(bool), errExp(Terr, TpcTypeElt, Tstring),
  errOvfl(TpcTypeElt), errUdfl(void), finalizeCast(TpTypeElt),
  freeAlloc(void), freePrevCast(void), indir(void),
  indirindex(const TresulExp * /*~MayModify*/), managePointers(TpTypeElt),
  manageSetjmp(void), manageUsedForIncOp(void), modifPtdVal(Terr, bool),
  primQualif(void),
  processBitOper(Tstring, void (*)(void), void (*)(TpcTypeElt), bool, Ttok),
  processCast(TmacParCtr),
  processLogOper(void (*pf)(void), bool, Tstring, Ttok), processNumCst(void),
  resulIncOp(TkInc,TmacParCtr), resulUnOp(TtypeSort, Tstring),
  specialCaseOp(TkAdd),
  transfOpnd(const TresulExp * /*~MayModify*/, TresulExp *),
  warnCstBool(Tstring), warnNotPureBool(TresulExp *, Tstring);
static TpTypeElt advInTypeChain(TpTypeElt), commonType(TtypeSort),
  copyGeneric(TpTypeElt), copyTypeEltNoOwn(TpTypeElt);
static TpcTypeElt managePredefTypes(TpredefTypes);
static TtypeSort computeCstType(TcalcS, TtypeSort, TtypeSort), computeTypeSort(
  TcalcS, TtypeSort, TtypeSort), computeLstType(TcalcS);
static Tmember isMember(const TresulExp *);
static size_t sizeOfTypeI(TpTypeElt, bool, Terr, Tstring);
static Tstring typeToS2NoQual(TpcTypeElt);
static Terr warnOrErr(Terr);
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
static void manageLeftBits(void);
#endif

/* Global objects */
static TpTypeElt allocType = NULL;  /* type of 'sizeof' argument for
					     ~SizeOfMemBlk function argument. */
static bool apsndOpnd = False;  /* True when inside operand of '&' operator */
static Terr convToUnsgnd;
static bool cstExpAsked = False;
static bool errAsgn;
static bool initDPFl = False;
static TsemanElt *lastCalledFct;
static TmacParCtr lastPrimMacParCtr, lastPrimMacParCtrIM;
static TtypeSort mskCmpat = NumEnumBool;  /* default value */
static TREInfo oldCEinfo;
static TmacLvl pLCMacLvl;
static TpTypeElt preLastCastType = NULL;  /* type before previous cast */
static TcalcU savCExpVal;
static TpTypeElt sizeofOpndType = NULL;
static bool stddefIncldFl;  /* flag to output message only once */

/* External objects */
bool evaluateFl = True;
TpcTypeElt *limErroTypes;  /* to prevent multiple (same) errors in
							     initializations. */
Trchbl nxtStmtRchbl;  /* next statement reachable ? */


bool boolExp(Tstring operTxt)
/* Returns True if boolean expression correct and constant */
{
  TpTypeElt eType = cExp.type;

  if (cExp.ValMltplDef) cExp.Revlbl = False;
  checkInit();
  CheckNumCstNamed(cExp, eType);
  if (*operTxt != 'i') cleanExprThings();  /* test "if" (because of setjmp
								 management). */
  if (eType != NULL) {
    bool b = False;

    if (NakedTok(cExp.topOper)==ASGNOP && !IsTypeSort(eType, Bool)) {
      err0(MsngEqual | Warn3);
      b = True;}
    if (! (IsTypeSort(eType, mskBool&NumEnumBool) || IsPtr(eType) &&
						      mskBool&PtrPoss)) errWSTT(
		(IsScalar(eType))
		? (b)
		  ? NoErrMsg
		  : BoolExptd | UWarn1
		: BoolExptd, operTxt, eType, NULL);
    else if (cExp.Revlbl) warnCstBool(NULL);
    FreeExpType2(eType);}
  return cExp.Revlbl;
}

bool correctExprN(TpTypeElt exptdType, TtypeSort msk, bool cstExp, Tstring
						      operName, bool chkNumCstP)
/* Gets an asgnExpr, and detects an error if type of gotten expression not com-
   patible with 'exptdType' and/or 'msk', or if 'cstExp' = True and expression
   not constant.
   Returns False if gotten expression incorrect, or if type mismatch (cExp.type
   == NULL in both cases).
   Frees expression type only if exptdType != NoFreeExpType.
   Does not exit via 'longjmp', either directly or indirectly.
   Can be called recursively (because of 'zif'). */
{
  bool resul, freeExpType = (exptdType != NoFreeExpType), savChkNumCst =
								      chkNumCst;
  const Tstring savParamTxt = paramTxt;
  TpTypeElt eType, newElt;

  chkNumCst = chkNumCstP;
  if (! alrdAnalzdExp) {
    bool savEvalFl = evaluateFl, savCstExpAskd = cstExpAsked;
    TsemanElt *savLCF = lastCalledFct;

    cstExpAsked = cstExp;
    evaluateFl = True;
    ignoreErr = False;
    asgnExpr();
    if (cExp.ValMltplDef) cExp.Revlbl = False;
    cstExpAsked = savCstExpAskd;
    evaluateFl = savEvalFl;
    lastCalledFct = savLCF;}
  else alrdAnalzdExp = False;
  paramTxt = operName;  /* also for managePointers() */
  eType = cExp.type;
  if (msk != Void) {
    if (GtPtr(exptdType, NoFreeExpType)) {
      newElt = allocTypeEltIC(exptdType, NoQualif);
      newElt->typeSort |= msk;}
    else {
      newElt = allocTypeEltID(msk, NoQualif);
      newElt->TagId = (eType!=NULL && IsTypeSort(eType, Enum))? eType->TagId
			      : GenericEnum;}  /* in case msk includes 'Enum' */
    newElt->NoOwner = True;
    exptdType = newElt;}
  else newElt = NULL;
  CheckNumCstNamed(cExp, exptdType);
  lExp.ptrId = pIdInit;   /* for		*/
  lExp.type = exptdType;  /*     modifPtdVal(). */
  difQual = 0;
  if (eType != NULL) {
    resul = True;
    if (exptdType != NULL) {
      if (! freeExpType) heedRootType = False;  /* for then msk!=Void */
      if (exptdType->typeSort >= DelType) { /* type expansion not needed */
        errQalTyp1 = NULL;
        if (! CompatType(exptdType, eType, Asgn)) resul = False;
        else {
          if (errQalTyp1 != NULL) {  /* report only one of many possible
				 error messages due to bad 'const' qualifier. */
#define MaxNbRmbrdTypes		10
            static TpcTypeElt rmbrdErroTypes[MaxNbRmbrdTypes];
            const TpcTypeElt *ptrErroTypes;

            if (msk!=Void || limErroTypes==NULL) limErroTypes =
							     &rmbrdErroTypes[0];
            else if (! allErrFl)
              for (ptrErroTypes = &rmbrdErroTypes[0]; ptrErroTypes !=
							       limErroTypes; ) {
                if (*ptrErroTypes++ == errQalTyp1) goto noErrL;}
            errWSSSS(ConstQalNotHeeded1|ConstWarn|PossErr, paramTxt,
	      typeToS2NoQual(errQalTyp1), typeToS1(errQalTyp2), (errQalTyp1 ==
					     exptdType)? NULL : errTxt[SubPre]);
            if (limErroTypes <= &rmbrdErroTypes[MaxNbRmbrdTypes - 1]
						 ) *limErroTypes++ = errQalTyp1;
#undef MaxNbRmbrdTypes
noErrL:;}
          if (!cExp.PointedByObj && cExp.ptrId!=NULL && cExp.ptrId->Kind==
				       Param) cExp.ptrId->MayNeedSaved = True;}}
      else if (! compatNumType()) resul = False;
      heedRootType = True;}
    if (! resul) {
      if (msk==Void
          && IsCmpsd(exptdType)
          && !IsCmpsd(eType)
          && (difQual = 0, warnOrErr(NoErrMsg))==NoErrMsg) {
        alrdAnalzdExp = True;  /* because of possible elided '{' in  */
        goto skipL1;}	       /*      initialization list => retry. */
      if (! errSinkType((msk == Void)? BadForLftType|PossErr|UWarn :
	     NotExptdType|PossErr|UWarn, typeToS2NoQual(exptdType))) goto skipL;
      if (ifDirExp && cExp.Revlbl) resul = True;}
    if (cstExp) {
      if (!cExp.Revlbl && !IsArr(eType)) {
        err0(CstExpExptd);
        if (freeExpType) resul = False;
        cExp.ErrEvl = True;}
      else if (IsPtrArr(eType) && InsideInterval(cExp.PseudoAttrib, Auto, Reg)
					   ) {err0(UncomputAd); resul = False;}}
skipL:
    managePointers(exptdType);
    if (! cExp.Revlbl)
#if PtrGtLong
      cExp.Pval = NULL;
#else
      cExp.Sval = 0;
#endif
    if (resul && !freeExpType) {
      if (eType->Generiq) (cExp.type = copyTypeEltNoOwn(eType))->Generiq =
				False;}  /* for array bound (cf. index type
					   verification if of parallel type). */
    else {
      FreeExpType2(eType);
      if (! freeExpType) cExp.type = NULL;}
skipL1:;}
  else resul = False;
  FreeExpType1(newElt);
  chkNumCst = savChkNumCst;
  paramTxt = savParamTxt;
  return resul;
}

void commaExpr(void)
/* Does not exit via 'longjmp', either directly or indirectly */
{
  bool commaSeen = False;

  while (asgnExpr(), curTok.tok == COMMA) {
    checkSideEffect();
    if (! commaSeen) {
      TstAlwdInCstExp();
      commaSeen = True;}
    GetNxtTok();}
  if (commaSeen) cExp.LvalFl = False;
}

void asgnExpr(void)
{
  TmacParCtr begAsgnMacParCtr = macParCtr;

  condExpr();
  if (curTok.tok == ASGNOP) {
    TkAsgn kindAsgn = (TkAsgn)curTok.Val;
    TresulExp slExp;  /* left operand saving */
    register TsemanElt *ptrLId = cExp.ptrId;
    TpTypeElt lType, eType;
    TmacParCtr operMacParCtr = lastPrimMacParCtrIM;
    bool lclAdrSeen, ignorable;

    TstAlwdInCstExp();
    if ((lType = cExp.type) != NULL) {  /* no type error seen in left operand */
      paramTxt = txtAsgnTok[kindAsgn];
      if (! cExp.LvalFl) {errWSS(NotLVal, paramTxt, errTxt[Left]); cExp.ptrId =
								 ptrLId = NULL;}
      else if (lType->typeSort==Void || IsArrFct(lType)) errWSTT(IlgLftType,
					     txtAsgnTok[kindAsgn], lType, NULL);
      else {
        bool isConst = lType->Qualif & ConstQal;

        if (IsStrun(lType)) {
          if (BaseStrunType(lType)->SynthQualif & ConstQal) isConst = True;
          (void)sizeOfTypeI(lType, False, IncplStrunType, paramTxt);}
        if (isConst) errCompoOf(lType->Qualif & ConstQal);
        else if (cExp.PointedByObj) modifPtdVal(WillM, False);
        else if (kindAsgn==SimplAsgn
                 && ptrLId!=NULL
                 && !ptrLId->Initlz
                 && ptrLId->PdscId->nstLvl>condStmtLvl
                 && ptrLId->Attribb==StatiL
                 && ptrLId->DynInitCtr!=NoDynInitWarn) errId0(UslStati|Warn2|
								 Effic, ptrLId);
        if (ptrLId!=NULL && IsStrun(ptrLId->type) && !cExp.PointedByObj
						   ) ptrLId->StrunMdfd = True;}}
    CheckNumCstNamed(cExp, lType);
    valueNotUsed();
    slExp = cExp;
    GetNxtTok();
    asgnExpr();
    lExp = slExp;
    CheckNumCstNamed(cExp, lType);
    if (NakedTok(cExp.topOper) == ASGNOP) valueNotUsed();  /* so that "i=j=1"
						      does not mark 'j' used. */
    paramTxt = txtAsgnTok[kindAsgn];
    if (lclAdrSeen = IsDP(LOCALADR)) ignorable = ignorableDP(begAsgnMacParCtr);
    if ((eType = cExp.type)!=NULL && lType!=NULL) {
      errAsgn = False;
      difQual = 0;
      switch(kindAsgn) {
        case SimplAsgn:
          if (! compatNumType()) {
            if ((IsPtr(lType) || IsStrun(lType)) && (errQalTyp1 = NULL,
					      CompatType(lType, eType, Asgn))) {
              if (IsPtr(lType)) {
                if (errQalTyp1 != NULL) errWSTTS(ConstQalNotHeeded1|ConstWarn|
		      PossErr, paramTxt, errQalTyp1, errQalTyp2, (errQalTyp1 ==
						 lType)? NULL : errTxt[SubPre]);
                if (cExp.LclAdr) {
                  if (! InsideInterval(lExp.PseudoAttrib, Auto, Reg)) {  /* non-
								local object. */
                    if (! lclAdrSeen) err0(AsgnGblWLclAd|Warn2|PossErr);
                    else lclAdrSeen = False;
                    cExp.LclAdr = False;}
                  else if (ptrLId != NULL) ptrLId->LclAd = True;}
                else if (ptrLId!=NULL && ptrLId->PdscId->nstLvl>condStmtLvl &&
			(IsPtr(ptrLId->type) || IsStrun(ptrLId->type) &&
			 isMember(&slExp)==SingleMember)) ptrLId->LclAd = False;
                if (ignorable) lclAdrSeen = False;
                if (!cExp.PointedByObj && cExp.ptrId!=NULL && cExp.ptrId->Kind
				     ==Param) cExp.ptrId->MayNeedSaved = True;}}
            else errAsgn = True;}
          if (cExp.Revlbl) {
            lExp.Revlbl = True;
            lExp.ErrEvl = cExp.ErrEvl;}
          if (ptrLId!=NULL && ptrLId->Kind==Obj) {
            if (ptrLId->Used || condStmtLvl>=ptrLId->PdscId->nstLvl) ptrLId->
						     DynInitCtr = NoDynInitWarn;
            else if (cExp.Revlbl && ptrLId->type!=lType /* composed object +
		access to one of its (sub-)components. */ && !lExp.PointedByObj
		&& !InsideMacro && !(IsPtr(eType) && InsideInterval(cExp.
		PseudoAttrib, Auto, Reg)) /* constant address */ && ptrLId->
			      DynInitCtr<NoDynInitWarn-1) ptrLId->DynInitCtr++;}
          if (eType->Generiq && !errAsgn) {
            if (eType->typeSort==lType->typeSort && (!IsPtr(eType) ||
			compatType(lType, eType, NoCheck)  /* to prevent
				 char* = int* = NULL from beeing correct. */)) {
              /* Keep right operand type, because of generic parallel types */
              managePointers(lType);
              FreeExpType2(lType);
              goto exit1L;}
            lExp.PseudoAttrib = cExp.PseudoAttrib;
            lType = copyGeneric(lType);}
          goto endCaseL;
        case AndAsgn:
          if (cExp.Revlbl
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
                         ) manageLeftBits();
#else
			  && cExp.Sval<0) cExp.Uval = ~cExp.Uval;  /* for no
	 wrong overflow error detection (cf. "ushort INDIC &= ~0u ^ 0x8000;". */
#endif
          /*~NoBreak*/
        case XorAsgn:
          if (lType->typeSort == Bool) checkPureBool();
          goto commonBitOpL;
        case IorAsgn:
          if (lType->typeSort==Bool && cExp.NotPureBool && ptrLId!=NULL &&
				!IsPtr(ptrLId->type)) ptrLId->NotPureBoo = True;
          if (lExp.NotPureBool) cExp.NotPureBool = True; /* does also .lclAdr */
commonBitOpL:
          mskCmpat = WhoEnumBool;
          break;
        case LShAsgn:
        case RShAsgn:
          if (lType->typeSort==Void || IsArrFct(lType)) lExp.type = NULL;  /*
	       these IlgLftType checked earlier (BEWARE, lType still used !). */
          if (! IsTypeSort(lType, WhoEnum)) errWrngType(WhoEnum);
          else if (! IsWhoEnumNotChar(eType)) errExp((IsTypeSort(eType,WhoEnum)
			 )? IlgRhtType|Warn2|PossErr : IlgRhtType, eType, NULL);
          goto exitL;
        case AddAsgn:
        case SubAsgn:
          mskCmpat = NumEnum;
          goto commonArithOpL;
        case MulAsgn:
        case DivAsgn:
          mskCmpat = Num;
          goto commonMulOpL;
        case ModAsgn:
          mskCmpat = Who;
commonMulOpL:
          if (! NxtIsTypId(eType)) cExp.type = copyGeneric(eType);  /* because
				      multiplication = sequence of additions. */
commonArithOpL: {
            TpcTypeElt w;

            if (pHeadTCBLists!=NULL && (w = searchTC(AddTC + (kindAsgn -
		 AddAsgn)))!=QuasiNULLval(TpcTypeElt) && (w==NULL || w->TypeId
					      ==lType->TypeId)) goto endCAsgnL;}
          if (eType->typeSort==Enum && !IsPtr(lType)) goto errAsgnL;
          break;}
      if (! compatNumType()) 
        if (InsideInterval(kindAsgn, AddAsgn, SubAsgn)) specialCaseOp(
								    AddSubAsgn);
        else
errAsgnL:
          errAsgn = True;
endCAsgnL:
      mskCmpat = NumEnumBool;  /* back to default value */
      lExp.CstImpsd = False;  /* see compatNumType */
endCaseL:
      if (errAsgn && !(lType->typeSort==Void || IsArrFct(lType)) /* IlgLftType
							      checked earlier */
	      ) errSinkType(BadForLftType|PossErr|UWarn, typeToS2NoQual(lType));
      if (IsTypeSort(eType, Bool) && !IsTypeSort(lType, Bool)) (lType =
				     copyTypeEltNoOwn(lType))->typeSort |= Bool;
exitL:;}
    managePointers(lType);
    FreeExpType(cExp); /* errExp() may have been called, so don't use 'eType' */
    cExp.type = lType;
exit1L:
    if (lclAdrSeen) errIlgDP(LOCALADR);
    cExp.ptrId = ptrLId;
    lExp.NotPureBool = cExp.NotPureBool;  /* keep that flag (also lclAdr) */
    cExp.Einfo = lExp.Einfo;
    /* Here so that done even if r/lType == NULL */
    if (kindAsgn == SimplAsgn) {
      cExp.hist = lExp.hist;
      if (ptrLId != NULL) {
        if (manageInitlzUsed()) {
          cExp.OldUsed = False;
          ptrLId->Used = False;}
        if (setjmpBlkLvl != 0) manageSetjmp();}
      cExp.topOper = ASGNOP;}  /* top operator of syntax tree */
    else manageUsedForIncOp();
    if (cExp.NotPureBool && lType!=NULL && lType->typeSort==Bool && ptrLId!=
			  NULL && !IsPtr(ptrLId->type) && !ptrLId->NotPureBoo) {
      if (ptrLId->PureBoolAskd && !cExp.InhibWaNPB) {errWSS(Outside01|Warn2|
		     PossErr, paramTxt, errTxt[Right]); cExp.InhibWaNPB = True;}
      ptrLId->NotPureBoo = True;}
    if (operMacParCtr!=0 && lastPrimMacParCtrIM==operMacParCtr) err1(
						 SideEffInMacPar|Warn3|PossErr);
    cExp.SideEff = True;
    cExp.LvalFl= False;}  /* value of assignment is not an l-value */
}

static void condExpr(void)
{
  term0();
  if (curTok.tok == QMARK) {
    TresulExp slExp;
    TresulExp expB, scExp;
    TmacLvl operMacLvl;
    Trchbl endFrstBrchRchbl;
    bool savEvalFl = evaluateFl, sideEffect;
    TnstLvl oldCondLvl = condStmtLvl;

    (void)boolExp("?");
    if (! (cExp.topOper & PARENSEEN)) err0(ShdBePrntzBool|UWarn1|Rdbl);
    expB = cExp;
    if (expB.Revlbl && expB.Uval==0) evaluateFl = False;
    condStmtLvl = nestLvl;
    GetNxtTok();
    commaExpr();
    operMacLvl = macLvl;
    if (! Found(COLON)) errWS(Exptd, ":");
    endFrstBrchRchbl = nxtStmtRchbl;
    nxtStmtRchbl = Rchbl;
    slExp = cExp;
    evaluateFl = (expB.Revlbl && expB.Uval!=0)? False : savEvalFl;
    condExpr();
    evaluateFl = savEvalFl;
    {
      bool lSideEffect = IsSideEffect(slExp), rSideEffect = IsSideEffect(cExp);

      if (expB.Revlbl) {
        condStmtLvl = oldCondLvl;
        if (expB.Uval != 0) {
          nxtStmtRchbl = endFrstBrchRchbl;
          transfOpnd(&slExp, &cExp);
          rSideEffect = True;}
        else {transfOpnd(&cExp, &slExp); lSideEffect = True;}}
      sideEffect = lSideEffect && rSideEffect;}
    lExp = slExp;
    paramTxt = ":";
    scExp = cExp;
    if (lExp.type == NULL) lExp.type = cExp.type;
    else if (cExp.type == NULL) cExp.type = lExp.type;
    oldCEinfo._eInfo = cExp.Einfo;
    if (! resulExp(NumEnumBool | PtrPoss | StrunPoss, False, &computeCond,
							   operMacLvl, QMARK)) {
      TpTypeElt curLType = lExp.type, curRType = cExp.type;
      bool rhtHghrParal = False;

      if (!CompatType(curLType, curRType, LitString) && !(rhtHghrParal = True,
		      compatType(curRType, curLType, LitString))) errWrngType(
						 NumEnumBool|PtrPoss|StrunPoss);
      else {
        curLType = arrToPtr(curLType);  /* for string literal case */
        curRType = arrToPtr(curRType);  /* for string literal case */
        if (curLType != curRType) {
          TpcTypeElt wL, wR;

          if (IsPtr(curLType) && (wL = NxtTypElt(curLType))!=(wR = NxtTypElt(
								   curRType))) {
            /* Create a result type chain that inherits qualifiers from both
               arms of the conditional; if one arm is string litteral, result
               is pointer. */
            TpTypeElt resType, lastNewElt = NULL;

            cExp.ArtifType =
		(   wL!=NULL && wL->typeSort==Void && !(lExp.Revlbl && lExp.
								      Uval==0)
		 || wR!=NULL && wR->typeSort==Void && !(cExp.Revlbl && cExp.
								      Uval==0));
            /* Choose higher parallel type or non-generic 'void *' arm */
            if (rhtHghrParal) {
              TpTypeElt w;;w = curRType; curRType = curLType; curLType = w;}
            do {
              TpTypeElt newElt;

              newElt = allocTypeEltIC(curLType, curRType->Qualif);
              if (lastNewElt == NULL) {
                resType = newElt;
                resType->NoOwner = True;}  /* for freeing purposes */
              else {
                lastNewElt->StopFreeing = False;  /* because full created
							   chain to be freed. */
                lastNewElt->NextTE = newElt;}
              lastNewElt = newElt;
            } while (!NxtIsTypId(curLType) && (curLType = advInTypeChain(
			      curLType))!=NULL && (curRType = advInTypeChain(
					curRType))!=NULL && curLType!=curRType);
            FreeExpType1(curRType);
            cExp.type = resType;}
          else if (! rhtHghrParal) {
            cExp.type = curLType;
            curLType = curRType;}}}
      if (! lExp.FctCallNoResTyp) cExp.FctCallNoResTyp = False;
      if (curLType != cExp.type) FreeExpType1(curLType);
      freePrevCast();}
    oldCEinfo._eInfo = 0;
    cExp.topOper = (sideEffect)? SIDEFF : QMARK;
    if (expB.LitCst) {
      cExp.LitCst = True;
      if (expB.LitCstOutsMac) cExp.LitCstOutsMac = True;}
    if (expB.Revlbl) {
      if (cExp.Revlbl && expB.ErrEvl) cExp.ErrEvl = True;}
    else {
      condStmtLvl = oldCondLvl;
      cExp.ValMltplDef = True;
      if (lExp.NotPureBool) cExp.NotPureBool = True;  /* does also cExp.LclAdr
							      |= lExp.LclAdr. */
      if (lExp.InhibWaNPB) cExp.InhibWaNPB = True;
      if (lExp.FctCallSeen1) {
        cExp.FctCallSeen1 = True;
        if (lExp.FctCallSeen) cExp.FctCallSeen = True;}
      if (cExp.type == &boolCstTypeElt) cExp.type = &boolTypeElt;
      if (endFrstBrchRchbl == Rchbl) nxtStmtRchbl = Rchbl;}
    {
      TresulExp *pExp;

      if (slExp.ptrId == NULL) goto keepRightL;
      if (scExp.ptrId == NULL) goto keepLeftL;
      if (! slExp.PointingOnObj) {
        TresulTstPtd w = tstPtd(slExp.ptrId->type, False);

        if (w==ALLCONST || w==STRUN && slExp.ptrId->Kind==Param) goto keepLeftL;}
keepRightL:
      cExp.ptrId = scExp.ptrId;
      cExp.PointingOnObj = scExp.PointingOnObj;
      pExp = &slExp;
      goto commonL;
keepLeftL:
      cExp.PointedByObj = slExp.PointedByObj;
      cExp.hist = slExp.hist;  /* other relevant fields already set by
								  'resulExp'. */
      pExp = &scExp;
commonL:
      if (pExp->ptrId!=NULL && slExp.ptrId!=scExp.ptrId) {
        if (pExp->PointingOnObj) pExp->ptrId->Used = True;
        else if (tstPtd(pExp->type, False) == NOTALLCONST) pExp->ptrId->Dmodfd
		   = True;}}}  /* to prevent possible 'PtrShdBeConst' warning */
}

static void computeCond(TpcTypeElt x)
/* Keep greatest absolute value, without loosing possible sign... */
{
  if (IsTypeSort(x, SigAri | Enum)) {
    bool negSeen = (lExp.Sval < 0);
#define Abs(x) ((x >= 0)? x : -x)

    if (Abs(lExp.Sval) > Abs(cExp.Sval)) {
      if (cExp.Sval < 0) negSeen = True;
      cExp.Sval = lExp.Sval;}
    if (cExp.Sval>0 && negSeen) cExp.Sval = - cExp.Sval;}
#undef Abs
  else if (lExp.Uval > cExp.Uval) cExp.Uval = lExp.Uval;
}

static void term0(void)
{
  term1();
  while (curTok.tok == LOGOR) processLogOper(&term1, True, "||", curTok.tok);
}

static void term1(void)
{
  term2();
  while (curTok.tok == LOGAND) processLogOper(&term2, False, "&&", curTok.tok);
}

static void computeShortCircuit(TpcTypeElt x /*~NotUsed*/)
{
  if (cExp.Uval != 0) cExp.Uval = 1;
}

static void term2(void)
{
  term3();
  while (curTok.tok == IOR) processBitOper("|", &term3, &computeIor, False,
								    curTok.tok);
}

static void computeIor(TpcTypeElt x /*~NotUsed*/)
{
  cExp.Uval |= lExp.Uval;
}

static void term3(void)
{
  term4();
  while (curTok.tok == XOR) processBitOper("^", &term4, &computeXor, False,
								    curTok.tok);
}

static void computeXor(TpcTypeElt x /*~NotUsed*/)
{
  cExp.Uval ^= lExp.Uval;
}

static void term4(void)
{
  term5();
  while (curTok.tok == APSAND) processBitOper("&", &term5, &computeAnd, True,
									   AND);
}

static void computeAnd(TpcTypeElt x /*~NotUsed*/)
{
  cExp.Uval = lExp.Uval & savCExpVal;
}

static TkCmp operCmp;		/* limited extent */
static void checkCmpdTypes(void);  /* limited extent */
#pragma noinline (checkCmpdTypes)

static void term5(void)
{
  term6();
  while (curTok.tok == EQUALOP) {
    TkCmp oper;
    const TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;
    Ttok topOper = EQUALOP | NONASSOC;

    oper = (TkCmp)curTok.Val;		/* operator in class EqualOp */
    GetNxtTok();
    term6();
    lExp = slExp;		/* left operand */
    operCmp = oper;
    paramTxt = txtCmpTok[oper];
    if (cExp.Revlbl) {
      oldCEinfo._eInfo = cExp.Einfo;
      if (cExp.Uval != 0) {
        if (! cExp.ErrEvl) {
          checkPureBool();
          if (NakedTok(cExp.topOper) == (EQUALOP|NONASSOC)) err0(UslCmp|Warn1|
								       Effic);}}
      else if (lExp.SjVal) {
        topOper |= SETJMPUSED;
        longjmpTaken = (oper != EQ);}}
    if (! resulExp(NumEnumBool | PtrPoss, True, &computeCmp, operMacLvl,
								     EQUALOP)) {
      checkCmpdTypes();
      if (cExp.Revlbl) cExp.Uval = (TcalcU)((oper == EQ)? lExp.Pval ==
		     cExp.Pval : lExp.Pval != cExp.Pval);}  /* compute result */
    else if (cExp.type!=NULL && IsTypeSort(cExp.type, Flt) && !FoundDP(EXACTCMP)
						 ) err1(FltNotEq|Warn2|PossErr);
    oldCEinfo._eInfo = 0;
    createBoolResult();
    cExp.topOper = topOper;}
}

static void term6(void)
{
  term7();
  while (curTok.tok == ORDEROP) {
    TkCmp oper;
    const TresulExp slExp = cExp;	/* left operand saving */
    TmacLvl operMacLvl = macLvl;

    oper = (TkCmp)curTok.Val;		/* operator in class OrderOp */
    GetNxtTok();
    term7();
    lExp = slExp;		/* left operand */
    operCmp = oper;
    paramTxt = txtCmpTok[oper];
    if (lExp.type!=NULL && PureUnsig(lExp.type) && cExp.Revlbl && !cExp.ErrEvl
			  && cExp.Uval==0 && !lExp.UnsigDiff && oper!=GT) err1(
						     UnsigNonNeg|Warn2|PossErr);
    if (! resulExp(NumEnumBool | PtrPoss, True, &computeCmp, operMacLvl,
								     ORDEROP)) {
      checkCmpdTypes();
      if (cExp.Revlbl) {  /* compute result */
#if PtrGtLong
        switch (oper) {
          case GT: cExp.Uval = (TcalcU)(lExp.Pval > cExp.Pval); break;
          case GE: cExp.Uval = (TcalcU)(lExp.Pval >= cExp.Pval); break;
          case LE: cExp.Uval = (TcalcU)(lExp.Pval <= cExp.Pval); break;
          case LT: cExp.Uval = (TcalcU)(lExp.Pval < cExp.Pval); break;
          /*~NoDefault*/}
#else
        lExp.Uval = (TcalcU)lExp.Pval;
        cExp.Uval = (TcalcU)cExp.Pval;
        computeCmp(&natTyp[ULLongDpl]);
#endif
      }}
    else if (cExp.UnsigDiff) err1(UnsigDiffNotNeg|Warn2|PossErr);
    createBoolResult();
    cExp.topOper = ORDEROP | NONASSOC;}
}

#pragma noinline (sConvToType, uConvToType)
static TcalcS sConvToType(const TresulExp *pExp, TpcTypeElt x)
{
  return (x->typeSort < Long)
          ? (TcalcS)(int)pExp->Sval
#ifdef LONGLONG
          : (x->typeSort < LLong)
            ? (TcalcS)(long)pExp->Sval
#endif
            : pExp->Sval;
}

static TcalcU uConvToType(const TresulExp *pExp, TpcTypeElt x)
{
  return (x->typeSort < Long)
          ? (TcalcU)(uint)pExp->Uval
#ifdef LONGLONG
          : (x->typeSort < LLong)
            ? (TcalcU)(ulong)pExp->Uval
#endif
            : pExp->Uval;
}

static void checkCmpdTypes(void)
{
  TpTypeElt lType = (lExp.ArtifType)? NoConstTyp(&voidPtrTypeElt)
	      : lExp.type, rType = (cExp.ArtifType)? NoConstTyp(&voidPtrTypeElt
								  ) : cExp.type;

  if (rType==NULL || lType->typeSort!=Ptr || rType->typeSort!=Ptr ||
					     !CompatType(lType, rType, EqCmp)) {
    errWrngType(NumEnumBool | PtrPoss);}
  else if (operCmp > NE) {
    TpcTypeElt type, nType;

    for (type = lType;; type = rType) {
      if ((nType = NxtTypElt(type))!=NULL && (nType->typeSort==Void || IsFct(
			     nType))) {errExp(NoOrderRel|Warn1|PossErr, lType,
						       typeToS2(rType)); break;}
      if (type == rType) break;}}
  FreeExpType2(lExp.type);
  cExp.ptrId = NULL;
}

static void computeCmp(TpcTypeElt x)
{
#define LtBit	(TcmpBit)1
#define EqBit	(LtBit << 1)
#define GtBit	(EqBit << 1)
  typedef uint TcmpBit;
  TcmpBit cmpMsk;
  static TcmpBit cmpCod[/*~IndexType TkCmp*/] = {EqBit, LtBit|GtBit, GtBit,
					       GtBit|EqBit, LtBit|EqBit, LtBit};

  if (IsTypeSort(x, (SigWhoEnum | Flt))) {
    TcalcS sCmpOpd1 = sConvToType(&lExp, x), sCmpOpd2 = sConvToType(&cExp, x);

    cmpMsk = (sCmpOpd1 > sCmpOpd2)
              ? GtBit
              : (sCmpOpd1 == sCmpOpd2)
                ? EqBit
                : LtBit;}
  else {
    TcalcU uCmpOpd1 = uConvToType(&lExp, x), uCmpOpd2 = uConvToType(&cExp, x);

    cmpMsk = (uCmpOpd1 > uCmpOpd2)
              ? GtBit
              : (uCmpOpd1 == uCmpOpd2)
                ? EqBit
                : LtBit;}
  cExp.Uval = (cmpCod[operCmp] & cmpMsk)? 1 : 0;
  /* *doIt* float types */
#undef LtBit
#undef EqBit
#undef GtBit
}
/*~Undef operCmp, checkCmpdTypes */

static TkShi operShi;  /* temporary */

static void term7(void)
{
  term8();
  while (curTok.tok == SHIFTOP) {
    TkShi oper;
    const TresulExp slExp = cExp;	/* left operand saving */
    TmacLvl operMacLvl = macLvl;

    oper = (TkShi)curTok.Val;		/* operator in class ShiftOp */
    GetNxtTok();
    term8();
    lExp = slExp;
    paramTxt = txtShiTok[oper];
    operShi = oper;
    {
      TpTypeElt lType;

      /* Start by eliminating erroneous cases (else, funny messages from
         resulExp() because rType==lType...). */
      if ((lType = lExp.type)!=NULL && !IsTypeSort(lType, WhoEnum)) {
        errWrngType(WhoEnum);
        FreeExpType2(lType);
        lExp.type = lType = NULL;}
      else if (cExp.type!=NULL && !IsWhoEnumNotChar(cExp.type)) errExp(
	    ((IsTypeSort(cExp.type, WhoEnum))? IlgRhtType|Warn2|PossErr :
						  IlgRhtType), cExp.type, NULL);
      else {
        if (cExp.Revlbl && !cExp.ErrEvl && (cExp.Sval<0 || lType!=NULL && cExp.
	     Sval>=(TcalcS)lType->size * CHAR_BITC)) { /* *doIt*: portability */
          errWSSSS(UndefResul|Warn2|PossErr, paramTxt, grstIntToS(cExp.Sval),
							 typeToS1(lType), NULL);
          cExp.ErrEvl = True;
          cExp.Sval = 0;}
        FreeExpType(cExp);}
      if (lType!=NULL && IsChar(lType) && chkPortbl) errWS(PrtblOper | UWarn2,
								      paramTxt);
      cExp.type = lType;}
    cExp.CstImpsd = lExp.CstImpsd;
    cExp.UnsigDiff = False;
    resulExp(WhoEnum, False, &computeShi, operMacLvl, SHIFTOP);
    if (!cExp.Revlbl && lExp.CstImpsd && lExp.Revlbl && lExp.Sval>=0) cExp.
		      ValMltplDef = True;  /* Prevent possible 'ConvToUnsgnd' */
    cExp.topOper = SHIFTOP | NONASSOC;}
}

static void computeShi(TpcTypeElt x)
{
  TtypeSort tsl = x->typeSort;

  if (operShi == LSh) {
    register TcalcS w;

    w = lExp.Sval << cExp.Sval;
    if ((lExp.Sval>=0 && w<0 || lExp.Sval<0 && w>=0) && tsl & SigWhoEnum ||
				    (TcalcU)w>>cExp.Sval!=lExp.Uval) errOvfl(x);
    cExp.Sval = w;}
  else if (cExp.Uval == 0) cExp.Sval = lExp.Sval;
  else {
    if (tsl & SigWhoEnum) cExp.Sval = lExp.Sval >> (cExp.Uval - 1);
    else cExp.Uval = lExp.Uval >> (cExp.Uval - 1);
    if (cExp.Sval==0 && lExp.Sval!=0) errUdfl();
    if (tsl & SigWhoEnum) cExp.Sval >>= 1;
    else cExp.Uval >>= 1;}
}
/*~Undef operShi */

static void term8(void)
{
  term9();
  while (curTok.tok == ADDOP) {
    TkAdd oper;
    const TresulExp slExp = cExp;	/* left operand saving */
    TmacLvl operMacLvl = macLvl;
    bool foundTC = False;

    oper = (TkAdd)curTok.Val;		/* operator in class AddOp */
    GetNxtTok();
    term9();
    lExp = slExp;		/* left operand */
    paramTxt = txtAddTok[oper];
    if (pHeadTCBLists != NULL) foundTC = manageTypCmbn(AddTC + (oper - Add));
    if (oper == Add) {
      if (! resulExp(Num | PtrPoss, False /* because of x0 + deltaX, x0 cst */,
					      &computeAdd, operMacLvl, ADDOP)) {
        specialCaseOp(oper);
        cExp.topOper = ADDOP;}}
    else {
      bool opnd2LE1 = cExp.Revlbl && cExp.Uval<=1;

      if (! resulExp(NumEnum | PtrPoss, False, &computeSub, operMacLvl, ADDOP))
        specialCaseOp(oper);
      else if (! foundTC) {  /* see whether "distance" obtained */
        TpTypeElt eType = cExp.type;

        if (eType!=NULL && eType->typeSort==Enum) {
          FreeExpType2(eType);
          cExp.type = NoConstTyp(&natTyp[IntDpl]);}}
      if (cExp.type!=NULL && PureUnsig(cExp.type) && !cExp.Revlbl /* because
	       either no problem or already warned on overflow. */ && !opnd2LE1)
        cExp.UnsigDiff = True;
      cExp.topOper = SubOP;}}
}

static void computeAdd(TpcTypeElt x)
{
  if (x->typeSort & UnsigWho) {
    cExp.Uval += lExp.Uval;
    if (cExp.Uval < lExp.Uval) errOvfl(x);}
  else {  /* *doIt* float types */
    if (! computeSigAdd(lExp.Sval, cExp.Sval, &cExp.Sval)) errOvfl(x);}
}

static void computeSub(TpcTypeElt x)
{
  if (x->typeSort & UnsigWho) {
    if (cExp.Uval > lExp.Uval) errOvfl(x);
    cExp.Uval = lExp.Uval - cExp.Uval;}
  else {  /* *doIt* float types */
    TcalcS prevValCExp = cExp.Sval;

#if LONG_MINC + LONG_MAXC != 0
    if ((!computeSigAdd(lExp.Sval, -cExp.Sval, &cExp.Sval)
	|| prevValCExp==LONGLONG_MINC && lExp.Sval>=0) && cExp.Sval!=0
#else
    if (! computeSigAdd(lExp.Sval, -cExp.Sval, &cExp.Sval)
#endif
								  ) errOvfl(x);}
}

static void term9(void)
{
  term10();
  while (curTok.tok==STAR || curTok.tok==MULOP) {
    TkMul oper;
    const TresulExp slExp = cExp;	/* left operand saving */
    TmacLvl operMacLvl = macLvl;

    oper = (curTok.tok == STAR)? Mul : (TkMul)curTok.Val;
    GetNxtTok();
    term10();
    lExp = slExp;		/* left operand */
    paramTxt = txtMulTok[oper];
    if (pHeadTCBLists==NULL || !manageTypCmbn(MulTC + (oper - Mul))) {
      if (cExp.type != NULL)  /* coefficient ? */
        if (! NxtIsTypId(cExp.type)) cExp.type = copyGeneric(cExp.type);  /*
			      because multiplication = sequence of additions. */
        else if (oper==Mul && lExp.type!=NULL && !NxtIsTypId(lExp.type)
					  ) lExp.type = copyGeneric(lExp.type);}
    if (oper == Mul) resulExp(Num, False, &computeMul, operMacLvl, MULOP);
    else {
      if (oper == Div) resulExp(Num, False, &computeDiv, operMacLvl, MULOP);
      else  /* Mod */  resulExp(Who, False, &computeMod, operMacLvl, MULOP);
      cExp.topOper = MULOP | NONASSOC;}}
}

static void computeMul(TpcTypeElt x)
{
  if (x->typeSort & UnsigWho) {
#define SqrtTwoPow32	65536L
    if (lExp.Uval<=SqrtTwoPow32-1 && cExp.Uval<=SqrtTwoPow32) cExp.Uval *=
								      lExp.Uval;
#undef SqrtTwoPow32
    else if (cExp.Uval != 0) {
      register TcalcU w;

      w = lExp.Uval * cExp.Uval;
      if (w/cExp.Uval != lExp.Uval) errOvfl(x);
      cExp.Uval = w;}}
  else {  /* *doIt* float types */
#define SqrtTwoPow31	46340L  /* 46340 = sqrt(2**31) */
    if ((TcalcU)(lExp.Sval+SqrtTwoPow31)<=SqrtTwoPow31+SqrtTwoPow31 && (TcalcU)
			    (cExp.Sval+SqrtTwoPow31)<=SqrtTwoPow31+SqrtTwoPow31)
      cExp.Sval *= lExp.Sval;
#undef SqrtTwoPow31
    else if (cExp.Sval != 0) {
      register TcalcS w;

      w = lExp.Sval * cExp.Sval;
      if (w/cExp.Sval != lExp.Sval) errOvfl(x);
      cExp.Sval = w;}}
}

static void computeDiv(TpcTypeElt x)
{
  if (cExp.Sval == 0) errOvfl(lExp.type);  /* divide by 0 */
  else {
    if (x->typeSort & UnsigWho) cExp.Uval = lExp.Uval / cExp.Uval;
    else cExp.Sval = lExp.Sval / cExp.Sval;  /* *doIt* float types */
    if (cExp.Uval==0 && lExp.Uval!=0) errUdfl();}
}

static void computeMod(TpcTypeElt x)
{
  if (cExp.Sval == 0) errOvfl(lExp.type);  /* divide by 0 */
  if (x->typeSort & UnsigWho) cExp.Uval = lExp.Uval % cExp.Uval;
  else cExp.Sval = lExp.Sval % cExp.Sval;
}

static void term10(void)
{
  Ttok topOper, oldTopOper = cExp.topOper;
  TmacLvl operMacLvl = macLvl;
  TmacParCtr operMacParCtr = macParCtr;

  cExp.topOper = topOper = curTok.tok;  /* 'curTok.tok' to be saved, because
						      of right associativity. */
  switch (curTok.tok) {
  case ADDOP: {
      TkAdd oper;

      oper = (TkAdd)curTok.Val;
      if (InsideInterval(NxtTok(), CSTNU, CSTNU1)) {  /* special case for
			   numeric constants, to be able to mark them signed. */
        curTok.Val = (oper != Add /* Sub */)? SignSeen | NegSeen : SignSeen;
        goto cstNuL;}
      term10();
      resulUnOp(Num, txtAddTok[oper]);
      if (cExp.Revlbl) {
        TpcTypeElt rType;

        if ((rType = cExp.type) != NULL) {
          TtypeSort tsr = rType->typeSort;

          if (tsr & SigWhoEnum) {
            if (oper != Add /* Sub */) {
              register TcalcS w;

              w = -cExp.Sval;
              if (((w<0 && cExp.Sval<0) || (w>0 && cExp.Sval>0))) errOvfl(rType);
              cExp.Sval = w;}}
          else if (tsr & UnsigWho) {
            if (oper != Add /* Sub */) {
              topOper = SubOP;
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
              if (tsr < Long) cExp.Uval = (- cExp.Uval) & UINT_MAXC;
						 /* because -(1U) = UINT_MAXC */
#  ifdef LONGLONG
              else if (tsr < LLong) cExp.Uval = (- cExp.Uval) & ULONG_MAXC;
					       /* because -(1UL) = ULONG_MAXC */
#  endif
              else cExp.Uval = -cExp.Uval;
#else
              cExp.Uval = -cExp.Uval;
#endif
            }}}
        else cExp.Sval = - cExp.Sval;}
      goto defTopOperL;}
  case APSAND: {
      TpTypeElt ptrAddendType = (GetNxtTok(), IsDP(INDEXTYPE))?
						      getINDEXTYPEtype() : NULL;
      bool savAO = apsndOpnd;

      apsndOpnd = True;
      term10();
      if (! cExp.LvalFl) {paramTxt = "u&"; errExp(NotLVal, NULL, NULL); cExp.
								  ptrId = NULL;}
      else {
        TpTypeElt newElt = allocTypeEltID(Ptr, NoQualif);

        if (cExp.PseudoAttrib == Reg) {err0(RegAttribForAmp); cExp.
							   PseudoAttrib = Auto;}
        newElt->NoOwner = True;
        if (cExp.type != NULL) newElt->StopFreeing = ! cExp.type->NoOwner;
        newElt->size = PtrSiz;
        if (ptrAddendType != NULL) newElt->BndType = ptrAddendType;
        else {
          Ttok topOper = NakedTok(cExp.topOper);

          newElt->BndType = ((topOper==LSBR || topOper==STAR) && cExp.type!=
	     NULL)? lExp.type : (moreIndexTypeChk)? NoConstTyp(
						  &defaultIndexTypeElt) : NULL;}
        newElt->NextTE = cExp.type;
        newElt->Generiq = True;  /* to allow '&buf[...]' to be compatible with
				   any reachable descendant of 'Ptr/TbufElt'. */
        cExp.type = newElt;
        cExp.Revlbl = cExp.Levlbl;
        cExp.ErrEvl = False;
        if (! cExp.PointedByObj) {
          TsemanElt *ptrId = cExp.ptrId;

          if (ptrId!=NULL && ptrId->Initlz /* because of UslObj */) ptrId->
		Used = cExp.OldUsed;  /* taking address of object is not
							      (yet) using it. */
          cExp.PointingOnObj = True;}
        else PopHist(cExp.hist);
        cExp.LvalFl = False;
        cExp.LclAdr = (cExp.PseudoAttrib == Auto);}
      apsndOpnd = savAO;
      goto defTopOperL;}
  case CSTCH: case CSTCH1: case CSTWCH: case CSTWCH1:  /* character constant */
    cExp.Einfo = 0;
    cExp.ErrEvl = (curTok.tok >= CSTCH1);
    cExp.Sval = (cExp.ErrEvl)? 0 : (TcalcS)(char) /* because of the rules for
					   sign of char constant. */ curTok.Val;
    cExp.type = NoConstTyp(((curTok.tok==CSTCH || curTok.tok==CSTCH1)
			     ? (sysHdrFile && ifDirExp)
			       ? &longCstTypeElt
			       : &charCstTypeElt
			     : managePredefTypes(WCHAR_T)));
    goto cstL1;
  case CSTNU: case CSTNU1:			/* numeric constant */
cstNuL:
    processNumCst();
    if (!cExp.ErrEvl && (TcalcU)(cExp.Sval + 1)>(2)/* *doIt* Flt */ &&
					 curTok.tok!=CSTNU1 && !sysAdjHdrFile) {
      cExp.LitCst = True;
      if (operMacLvl==0 || InsideMacPar) cExp.LitCstOutsMac = True;}
cstL1:
    cExp.ptrId = NULL;
cstL2:
    SetInfoForCst
    goto exitIdL;
  case CSTST: {			/* string constant */
      TpTypeElt newElt;
      uint initTok = curTok.Val;

      cExp.Einfo = 0;  /* reset all flags */
      cExp.Pval = (const char *)ptrFreeIdSpace();
      /* Concatenate adjacent string litterals */
      do {
        analStrCst(&storeStrCh);
        if (curTok.ErrorT) cExp.ErrEvl = True;  /* do not reset cExp.Pval,
		since one may still want to use the erroneous string literal. */
        DefLastPrimMacPar(macParCtr);
      } while (NxtTok()==CSTST && curTok.Val==initTok);
      storeStrCh('\0');	/* terminating byte */
      cExp.LvalFl = True;
      cExp.Levlbl = True;
      cExp.Revlbl = True;  /* because "..." => &"..."[0] implicitely (which
						 is a compile-time constant). */
      cExp.PseudoAttrib = Stati;
      cExp.ptrId = NULL;
      if ((bool)initTok) {
        newElt = copyTypeEltNoOwn(NoConstTyp(&wstrCstTypeElt));
        newElt->StopFreeing = False;
        newElt->NextTE = copyTypeEltNoOwn(NoConstTyp(managePredefTypes(WCHAR_T)
									     ));
        newElt->NextTE->Qualif = ConstQal;}
      else newElt = copyTypeEltNoOwn(NoConstTyp(&strCstTypeElt));
      newElt->Lim = newElt->size = initGetStrLit(NULL);
      cExp.type = newElt;
      goto tstQualif1L;}
  case DPTOK:
    if ((Tdprag)curTok.Val == CASTTO) {
      TpTypeElt castType = curTok.DpType, eType;

      GetNxtTok();
      term10();
      if (castType!=NULL && (eType = cExp.type)!=NULL && !compatType(
			  represType(castType), represType(eType), VerifCast)) {
        errWSTT(IlgCastTo|Warn2|PossErr, NULL, castType, eType);
        FreeExpType2(castType);}
      else {
        freePrevCast();
        if (compatType(castType, eType, VerifCastF) && !cExp.UnsigDiff && !cExp.
		 CstImpsd) errWSTT(UslCastTo|Warn1|Rdbl, NULL, castType, eType);
        finalizeCast(castType);}
      goto defTopOperL;}
    if ((Tdprag)curTok.Val == VOIDTOTHER) {
      TpTypeElt eType;
      TpcTypeElt w;

      GetNxtTok();
      term10();
      if ((eType = cExp.type) != NULL) {
        if (IsPtr(eType)) {
          if ((w = NxtTypElt(eType)) == NULL) goto defTopOperL;
          if (w->typeSort == Void) {
            cExp.type = copyGeneric(eType);  /* keep qualifiers */
            goto defTopOperL;}}
        errIlgDP(VOIDTOTHER);}
      goto defTopOperL;}
    goto defaultL;
  case EMARK:
    GetNxtTok();
    term10();
    resulUnOp(mskBool, "!");
    if (cExp.Revlbl) cExp.Uval = (TcalcU)(! (bool)cExp.Uval);
    createBoolResult();
    goto defTopOperL;
  case IDENT: {
      TsemanElt *ptrId;

      cExp.type = NULL;
      cExp.Einfo = 0;  /* reset PseudoAttrib and all flags (Revlbl etc.) */
      if ((ifDirExp || zifExp) && checkSpeFct()) {  /* special preprocessor/
								zif function. */
        topOper |= PRIMARY;
        cExp.ptrId = NULL;
        SetInfoForCst
        goto defTopOperL;}
      if (ifDirExp) {  /* 'any identifiers remaining after macro expansion
							 are replaced by 0L'. */
        if (evaluateFl) errWN(UndeclId|Warn3|PossErr, curTok.IdName);
        cExp.type = NoConstTyp(&longCstTypeElt);
        cExp.Sval = 0;
        goto cstL1 /*~BackBranch*/;}
      if ((ptrId = curTok.PtrSem) == NULL) {  /* unknown identifier */
        DefSem1(artifDef, Obj, True /*defnd*/, True /*used*/, True /*initlz*/);
        Ttok w = peepNxtTok();  /* because of side-effect on curTok.IdName */

        errWN((w == LPAR)
		? (cExp.type = NoConstTyp(&undeclFctTypeElt),
							   UndeclFctId | UWarn3)
                : (isupper((char)*(curTok.IdName + LgtHdrId)))  /* symbol? =>
								    constant. */
		  ? (cExp.Revlbl = True, cExp.ErrEvl = True, UndeclId)
		  : UndeclId, curTok.IdName);
        if ((ptrId = defineId(artifDef)) == NULL) goto exit1IdL;
        ptrId->type = cExp.type;}
      if (ptrId->Kind >= Type) {
        if (ptrId->Kind == EnumCst) {
          if (ifDirExp) err0(IlgInIfExp);
          if (! ptrId->Defnd) {
            errWN(NotUsdEnumCst|Warn1|Rdbl, curTok.IdName);
            if (! allErrFl) ptrId->Defnd = True;}
          if (ptrId->type->TagId->ListAlwdFiles != NULL) checkPrivVisible(
								   ptrId->type);
          cExp.ptrId = ptrId;
          cExp.type = ptrId->type;
          cExp.Sval = (TcalcS)ptrId->EnumVal;
          ptrId->Used = True;
          goto cstL2 /*~BackBranch*/;}
        errWN(IlgId, curTok.IdName);
        goto exit1IdL;}
      if ((cExp.type = ptrId->type)==NULL && isupper((char)*(curTok.IdName +
			   LgtHdrId))) {cExp.Revlbl = True; cExp.ErrEvl = True;}
      if (ptrId->Attribb == Extrn) {
        if (ptrId->DeclInInHdr) checkIncldFiles(ptrId);}
      else if (!ptrId->Defnd && ptrId->type!=NULL && !IsFct(ptrId->type)) {
        errWN((ptrId->Kind == Param)? NotUsdPar|Warn1|Rdbl : NotUsdObj|Warn1|
							   Rdbl, curTok.IdName);
        if (! allErrFl) ptrId->Defnd = True;}
      cExp.PseudoAttrib = ptrId->Attribb;
      cExp.OldUsed = ptrId->Used;   /* save previous state of 'Used' flag */
      if (!zifExp && (ptrId->PdscId->idName!=curFctName || ptrId->type==NULL ||
	    !IsFct(ptrId->type)) /* recursion is not use */) ptrId->Used = True;
      cExp.LclAdr = ptrId->LclAd;  /* in case type(ident) == Ptr  (or Bool:
			    does then "cExp.NotPureBool = ptrId->NotPureBoo;" */
exit1IdL:
      cExp.LvalFl = True;
      cExp.Levlbl = True;  /* because &(id) yields constant address */
      cExp.Pval = ArbitAdr;
      cExp.ptrId = ptrId;
      cExp.hist = H_EMPTY;}
exitIdL:
    DefLastPrimMacPar(macParCtr);
    GetNxtTok();
tstQualif1L:
    cExp.topOper = topOper | PRIMARY;
    goto tstQualifL;
  case INCOP: {
      TkInc oper = (TkInc)curTok.Val;

      GetNxtTok();
      term10();
      resulIncOp(oper, operMacParCtr);
      break;}
  case LPAR: {  /* cast/sub-expression */
      Ttok w;

      GetNxtTok();
      /* Decide whether cast or parenthetized expression */
      if (   IsDecl1(BQualif)
          || curTok.tok==IDENT
           && (   curTok.PtrSem!=NULL
                && curTok.PtrSem->Kind==Type  /* type identifier */
               ||  curTok.PtrSem==NULL  /* undefined identifier */
                && !ifDirExp /* cast forbidden in '#if' */
             /* In an attempt to follow programmer idea, consider undefined pa-
                renthetized uppercase identifier as would-be type identifier. */
                && isupper((char)*(curTok.IdName + LgtHdrId))
                /* Beware of interaction of peepNxtTok() and macro-expansion
					    (case "...(defined <symbol> ...") */
                && ((w = peepNxtTok())==STAR || w==RPAR))) {  /* cast */
        processCast(operMacParCtr);
        break;}
      /* Expression */
      commaExpr();
      if (cExp.topOper==(CSTNU | PRIMARY) && macLvl==operMacLvl &&
				  macParCtr==operMacParCtr) cExp.LitCst = False;
      if (! Found(RPAR)) errMsngRPar();
      cExp.topOper |= PARENSEEN;  /* so that "(a=b)" => side-effect. */
      if (FoundDP(NONCONST)) cExp.Revlbl = False;}
tstQualifL:
    cExp.macLvl = operMacLvl;
    if (curTok.tok == INCOP) {
      DefLastPrimMacPar(operMacParCtr);
      primQualif();}
    else if (InsideInterval(curTok.tok, LPAR, DOT))
      primQualif();
    return;
  case SIZEOF: {
      bool parenFl = False, savEvalFl = evaluateFl;

      if (ifDirExp) err0(IlgInIfExp);
      GetNxtTok();
      DefLastPrimMacPar(macParCtr);
      evaluateFl = False;
      loopLvl++;  /* hack to prevent manageUsedForIncOp() from doing wrong */
      if (curTok.tok == LPAR) {		/* maybe type */
        parenFl = True;
        GetNxtTok();
        if (isType()) {cExp.type = declType(); cExp.topOper = WHITESPACE |
			       PRIMARY; cExp.ptrId = NULL; cExp.Revlbl = False;}
        else {commaExpr(); valueNotUsed();}
        if (curTok.tok != RPAR) errMsngRPar();}
      else {term10(); valueNotUsed();}
      loopLvl--;
      evaluateFl = savEvalFl;
      if ((cExp.Uval = (TcalcU)sizeOfTypeD(
		  cExp.type,
		  (cExp.ptrId!=NULL && cExp.ptrId->Kind==Param && cExp.ptrId->
		        type!=NULL && IsArrFct(cExp.ptrId->type) && cExp.hist==
	 			  H_EMPTY)? UndefSize|Warn2|PossErr : UndefSize,
		  (Tname)"\6\0sizeof"))
				   == 0) cExp.ErrEvl = True;  /* illegal size */
      if (InsideInterval(NakedTok(cExp.topOper), FCTC, INCOP) || cExp.Revlbl &&
       cExp.type!=NULL && !IsArr(cExp.type)) err0(SizeofDontEval|Warn3|PossErr);
      if (sizeofOpndType == XAllocSeen) sizeofOpndType = cExp.type;
      else FreeExpType(cExp);
      cExp.type = NoConstTyp(managePredefTypes(SIZE_T));
      {
        TREInfo savCEinfo;

        savCEinfo._eInfo = cExp.Einfo;
        cExp.Einfo = 0;  /* reset all flags */
        if (savCEinfo.s21._litCst) {
          cExp.LitCst = True;
          if (savCEinfo.s21._litCstOutsMac) cExp.LitCstOutsMac = True;}}
      cExp.CstImpsd = True;
      cExp.Revlbl = True;
      if (! evaluateFl) cExp.ErrEvl = True;  /* to prevent computation */
      cExp.ptrId = NULL;
      if (parenFl) GetNxtTok();  /* for good error position */
      goto tstQualif1L /*~BackBranch*/;}
  case STAR:
    GetNxtTok();
    term10();
    paramTxt = "u*";
    indir();
    goto defTopOperL;
  case TILDE:
    GetNxtTok();
    term10();
    resulUnOp(WhoEnum, "~");
    if (cExp.Revlbl) {
      if (cExp.type == NULL) cExp.Uval = ~cExp.Uval;
      else if (cExp.type->typeSort & SigWhoEnum) cExp.Sval = ~cExp.Sval;
      else {
        cExp.Uval = ~cExp.Uval;
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
        if (cExp.type->typeSort < Long) cExp.Uval &= UINT_MAXC;
#  ifdef LONGLONG
        else if (cExp.type->typeSort < LLong) cExp.Uval &= ULONG_MAXC;
#  endif
#endif
      }}
defTopOperL:
    cExp.topOper = topOper;
    break;
  default:
defaultL:
    cExp.type = NULL;  /* because of FreeExpType in errExp() */
    errExp((oldTopOper == SCOL)? StmtExptd : MsngExp, NULL, NULL);
    cExp.Einfo = 0;  /* reset all flags, except followings */
    cExp.LvalFl = True;
    cExp.Levlbl = True;
    cExp.Revlbl = True;
    cExp.Pval = ArbitAdr;
    cExp.ptrId = NULL;
    lastPrimMacParCtrIM = 0;}
  cExp.macLvl = operMacLvl;
}

static void processCast(TmacParCtr operMacParCtr)
{
  TpTypeElt castType, eType;
  bool macParMet, oddCSeen = False, portQMSeen, ignorable = False, oldSysMac;

  castType = declType();
  oldSysMac = sysMacro;
  if (!(portQMSeen = FoundDP(PORTQM)) && IsDP(ODDCAST)) {
    ignorable = ignorableDP(operMacParCtr + 1);
    oddCSeen = True;}
  if (ifDirExp) err0(IlgInIfExp);
  pLCMacLvl = macLvl;
  if (! Found(RPAR)) errMsngRPar();
  term10();
  if (lastPrimMacParCtr != operMacParCtr) macParMet = True;
  else macParMet = ignorable = False;
  lastPrimMacParCtr = operMacParCtr;
  eType = cExp.type;
  paramTxt = "cast";
  freePrevCast();
  if (castType != NULL) {
    TtypeSort tsc = castType->typeSort;

    if (IsCmpsdSort(tsc)) {
      errWT((!compatType(castType, eType, VerifCastF) && eType!=NULL &&
	     compatType(represType(castType), represType(eType), VerifCast)
				   )? IlgCastTarget1 : IlgCastTarget, castType);
      FreeExpType2(castType);
      castType = NULL;}
    else {
      if (tsc == Bool) cExp.NotPureBool = !(oddCSeen || cExp.Revlbl
					      && (cExp.Uval<=1 || cExp.ErrEvl));
      if (eType != NULL) {
        TtypeSort tse = eType->typeSort;
        Terr errNo;
        bool savSysMac = sysMacro;

        sysMacro = oldSysMac;  /* for warnings management */
        if (IsFctSort(tse) || (tse==Void || IsStrunSort(tse)) && tsc!=
	   Void) {lExp.type = castType; errWT(warnOrErr(IlgCastSource), eType);}
        else if (tsc != Void) {
          if (tse==Array && eType->Generiq) tse = Ptr;  /* for cast purpose,
						      string constant => Ptr. */
          if (compatType(castType, eType, VerifCastF)) {
            TpcTypeElt w;

            if (macParMet
		|| castType!=eType && castType->SysTpdf!=eType->SysTpdf
		|| tsc==Ptr && eType->Generiq && ((w = NxtTypElt(eType))==NULL
		    || w->typeSort==Void) && ((w = NxtTypElt(castType))==NULL
		     || w->typeSort!=Void)  /* (void *) generally to be cast to
					    other pointer, so do not complain
					    if people try to do things right. */
		|| cExp.CstImpsd
		|| cExp.ArtifType) goto noErrL;
            errNo = UslCast|UWarn1|Rdbl;
            goto stdErrL;}
          else {
            if (! (macParMet || sysMacro || tsc<tse)) preLastCastType = eType;
            if (tsc < DelType) {  /* cast to NumEnumBool */
              if (tsc & Bool) oddCSeen = False;
              switch(tse) {
                case Ptr:  /* Ptr => NumEnumBool */
                  if (allocType != NULL) freeAlloc();  /* inhibitates 'Xalloc'
								       check. */
                  if (cExp.Revlbl) {
                    cExp.Uval = (TcalcU)cExp.Pval;/* because of big endians */
                    if (cExp.PseudoAttrib == NoAttrib) {  /* address computable
								at link time. */
                      if (!cExp.ErrEvl && computeLstType(cExp.Sval) >
				  ((tsc == Enum)? Int : tsc)) errOvfl(castType);
                      break;}}  /* *doIt* break only if no overflow on any
								     machine. */
#if ! PtrGtLong
                  if (castType->SysTpdf && memcmp(castType->TypeId->PdscId->
					idName, SizeTName, LitLen(SizeTName))==0
                      || castType->typeSort==ULLong) goto noErrL;
#endif
                  errNo = PtrToNum|UWarn2|PossErr;
                  goto stdErrL;
                case Array:  /* Array => NumEnumBool */
                  errNo = NotArrToNum|UWarn1|Rdbl;
                  goto stdErrL;
                default:  /* NumEnumBool => NumEnumBool */
                  if (castType->LitCsta!=eType->LitCsta) CheckNumCstNamed(cExp,
								      castType);
                  if (cExp.Revlbl) {
                    checkInfoLoss(castType);
                    if (cExp.Sval<0 && IsTypeSort(castType, UnsigWho)
					     ) castValue(castType->typeSort);}}}
            else {  /* cast to Ptr */
              bool sameTypButQual;

              errQalTyp1 = NULL;
              sameTypButQual = compatType(castType, eType, CCheck1);
              if (errQalTyp1 != NULL) {
                if (! oddCSeen) errWSTT(HzrdConv1|UWarn2|PossErr, NULL, eType,
								      castType);
                ignorable = True;}
              if (! sameTypButQual) {
                TpTypeElt castTypw = castType;

                for(;;) {
                  TpTypeElt nxtCType, nxtEType;

                  if ((nxtCType = NxtTypElt(castTypw))==NULL || (nxtEType =
		     NxtTypElt(eType))==NULL && IsPtrArrSort(tse)) goto noErrL1;
                  switch(tse) {
                    case Array:  /* Array => Ptr */
                      if (castTypw == castType /* first turn around */) errWSTT(
				   ArrToPtr|UWarn1|Rdbl, NULL, eType, castTypw);
                      /*~NoBreak*/
                    case Ptr:  /* Ptr => Ptr */
                      if (IsFct(nxtCType) && IsFct(nxtEType)) goto hzdCastL;
                      if (castTypw == castType) { /* first turn around */
                        if (nxtEType->typeSort == Void) break;  /* because
					'void *' compatible with any pointer. */
                        if (nxtCType->typeSort == Void)
                          if (IsFct(nxtEType)) goto nonPortCastL;
                          else break;
                        if (nxtCType->typeSort == UByte) break;}  /* raw bytes
								   container. */
                      if ((uint)nxtCType->typeSort >= (uint)Fct) {  /* Fct,
							 VFct, Struct, Union. */
                        /* Do not ask for OddCast if casting a pointer on
                           struct/union into a pointer on its first/any member
                           (or the other way around, which is a little more
                           risky because of possible alignment problems, but
                           will prevent too much use of OddCast). */
                        if (checkFrstMember(nxtEType, nxtCType) ||
				     checkFrstMember(nxtCType, nxtEType)) break;
                        goto hzdCastL;}
                      if ((uint)nxtCType->typeSort >= (uint)Ptr) {
                        /* Pointer on Ptr/Array : dereference and loop */
                        castTypw = nxtCType;
                        tsc = castTypw->typeSort;
                        if (IsPtrArr(nxtEType)) {
                          eType = nxtEType;
                          tse = eType->typeSort;
                          continue;}  /* dereferencing pointer chain */
                        goto hzdCastL;}
                      /* Here, casting type can only be pointer on num/enum */
                      if ((uint)nxtEType->typeSort >= (uint)DelType)
                        if (IsStrun(nxtEType) && frstMemberCompat(nxtEType,
						     nxtCType->typeSort)) break;
                        else goto hzdCastL;
                      if (nxtCType->typeSort==Void || nxtEType->typeSort==
								    Void) break;
                      /* Cast to pointer on floating type never portable;
                         cast from floating type to integral type may be
                         portable (see, for example, dynarray.h). */
                      if (IsTypeSort(nxtCType, Flt)) goto nonPortCastL;
                      if (IsTypeSort(nxtEType, Flt)) goto nonPortCastQML;
                      if (nxtCType->Algn != nxtEType->Algn) {  /* nothing
					 to say if same alignment constraint. */
/*                        if (nxtCType->Algn > nxtEType->Algn) goto
				  nonPortCastQML;   because alignment cons-
				traint may have been respected by programmer. */
                        /* Cast to pointer on smaller integer sort a priori ne-
                           ver portable, because of big/little endians, but see
                           header management of dynamic arrays (dynarray.h). */
                        goto nonPortCastQML;}
                      break;
                    default:  /* NumEnumBool => Ptr */
                      cExp.PseudoAttrib = NoAttrib;
                      if (cExp.Revlbl) {
                        TtypeSort w;

                        if (cExp.Uval==0 && IsTypeSort(eType, WhoEnumBool)) {
                          /* Integer 0 can be cast to any pointer (idiom
							'&(struct *)0->fld'). */
                          if (nxtCType->typeSort == Void) castType =
				     copyGeneric(castType); /* NULL generic ! */
                          break;}
                        if (!cExp.ErrEvl && (((w = computeCstType(cExp.Sval,
						tse, UnsigWho))<=UInt)? IntSiz :
#ifdef LONGLONG
				(w > ULong)? LongLongSiz :
#endif
				 ((void)w, LongSiz))>PtrSiz) errOvfl(castType);}
                      cExp.Pval = (char * /*~OddCast*/)cExp.Uval;  /* because
							      of big endians. */
                      if (eType->SysTpdf && memcmp(eType->TypeId->PdscId->
			      idName, SizeTName, LitLen(SizeTName)) == 0) break;
                      goto nonPortCastQML;}
                  break;}}}}}  /* exit for(;;) loop */
        if (oddCSeen && !ignorable) errUslDP(ODDCAST);  /* no warning in macro
		if macro parameter met, because its type is a priori unknown. */
        goto noErrL;
hzdCastL:
        errNo = HzrdConv|UWarn2|PossErr;
stdErrL:
        if (!oddCSeen) errWSTT(errNo, NULL, cExp.type, castType);
noErrL:
        if (portQMSeen) errUslDP(PORTQM);
        goto noErrL1;
nonPortCastQML:
        /* Does not know whether really not portable */
        if (!oddCSeen && !portQMSeen && chkPortbl) errWSTT(
			     NonPortCastQM | UWarn1, NULL, cExp.type, castType);
        goto noErrL1;
nonPortCastL:
        if (!portQMSeen && chkPortbl) errWSTT(PortCastQM | UWarn1, NULL,
							   cExp.type, castType);
noErrL1:
        sysMacro = savSysMac;}}
    if (!macParMet || NakedTok(cExp.topOper)!=VFCTC) cExp.topOper = CAST;
		  /* inside macro, pseudo-void function stays so through cast */
    if (! (tsc==Ptr && (cExp.type==NULL || IsPtrArr(cExp.type)))) cExp.
								LvalFl = False;}
  cExp.FctCallNoResTyp = False;
  cExp.ArtifType = False;
  finalizeCast(castType);
}

static void finalizeCast(TpTypeElt castType)
{
  if (castType==NULL || castType->typeSort!=Void) checkInit();
  else cExp.ptrId = NULL;  /* for no checkInit */
  if (cExp.Revlbl && castType!=NULL && castType->RootTyp)  /* mark constants
						      of root type 'generic'. */
    castType = copyGeneric(castType);
  if (preLastCastType != cExp.type) FreeExpType(cExp);
  cExp.type = castType;
  cExp.CstImpsd = False;  /* to get 'type mismatch' error on
							 "schar = (short)3;". */
  cExp.UnsigDiff = False;
}

static void processNumCst(void)
{
  analNumCst(Convert);
  cExp.Einfo = 0;  /* reset all flags */
  cExp.ErrEvl = curTok.ErrorT;
  cExp.Sval = (curTok.Val & NegSeen)? -curTok.NumVal : curTok.NumVal;
  if (curTok.Val & FltSeen) {  /* floating constant */
    cExp.type = (curTok.Val & FSeen)
			? NoConstTyp(&floatCstTypeElt)
			: (curTok.Val & LDSeen)
			  ? NoConstTyp(&longDblCstTypeElt)
			  : (cExp.CstImpsd = True, NoConstTyp(
							   &doubleCstTypeElt));}
  else {
    if (sysHdrFile && ifDirExp) curTok.Val |= LSeen;
    if (curTok.NumVal>=0 && !(curTok.Val & (LSeen | LLSeen | USeen))
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
     && ((TcalcU)(cExp.Sval - INT_MINC) <= (TcalcU)((TcalcS)INT_MAXC -INT_MINC))
#endif
							   ) {
      cExp.type = NoConstTyp(&intCstTypeElt);  /* 'simple' integer constant */
      cExp.CstImpsd = True;}
    else {
      TpTypeElt newElt;
      TtypeSort cstSort;

      newElt = allocTypeEltID((TtypeSort)(0-0), NoQualif);
      newElt->NoOwner = True; newElt->Generiq = True;
      if ((cstSort = computeLstType(cExp.Sval)) < Int) cstSort =
#if USHRT_MAXC == UINT_MAXC
						(cstSort == UShort)? UInt :
#endif
									    Int;
      if (!(curTok.Val & SignSeen) && curTok.NumVal<0) cstSort =
#if defined(LONGLONG) || ULONG_MAX>UINT_MAXC
					 ULLong;
#else
					 UInt | ULong;
#endif
      else if (cstSort < Long) cstSort |=
#if UINT_MAXC == ULONG_MAXC
				(cstSort == UInt)? ULong | LLong | ULLong :
#endif
					   UInt | Long | ULong | LLong | ULLong;
      else if (cstSort < LLong) cstSort |= ULong | LLong | ULLong;
      else cstSort |= ULLong;
      if (curTok.Val & LSeen) cstSort &= Long | ULong;
#ifdef LONGLONG
      else if (curTok.Val & LLSeen) cstSort &= LLong | ULLong;
#endif
      else cExp.CstImpsd = True;
      if (curTok.Val & USeen) cstSort &= UInt | ULong | ULLong;
      else if (! (curTok.Val & OctHex)) cstSort &= ~ UInt;  /* because the
							    standard says so. */
      /* Keep only least type sort */
      {
        TtypeSort w;
        int i = 0;
        static const size_t tabSize[] = {IntSiz, IntSiz, LongSiz, LongSiz
#ifdef LONGLONG
						   , LongLongSiz, LongLongSiz
#endif
									      };

        for (w = Int; ! (w & cstSort); w <<= 1) {i++;}
        newElt->typeSort = w;
        newElt->size = tabSize[i];}
      cExp.type = newElt;}}
}

static void primQualif(void)
{
  CheckNumCstNamed(cExp, cExp.type);
  for (;;) {
    TpTypeElt initialType = cExp.type;

    switch (curTok.tok) {
    case ARROW:
      paramTxt = "->";
      {
        TpcTypeElt w;

        if (initialType!=NULL && (w = NxtTypElt(initialType))!=NULL &&
		!IsStrun(w)) errExp(IlgLftType, initialType, NULL);}  /* has to
			be done that way (cannot use test of DOT case), because
			indir() destroys head of type chain (field 'NextTE'). */
      indir();
      goto dotL;
    case DOT:
      paramTxt = ".";
      if (initialType!=NULL && !IsStrun(initialType)) errExp(IlgLftType,
							     initialType, NULL);
dotL:
      /* Structure/union member access */
      {
        const TdeclElt *member;
        TpTypeElt eType;

        cExp.topOper = curTok.tok | PRIMARY;
        if ((eType = cExp.type) != NULL) {
          if (eType->TagId->ListAlwdFiles != NULL) checkPrivVisible(eType);
          member = BaseStrunType(eType)->MemberList;
          if (member == NULL) {
            (void)sizeOfTypeI(eType, False, IncplStrunType, paramTxt);/* type */
            errExp(NoErrMsg, NULL, NULL);}}		/* not yet completed. */
        if (NxtTok() != IDENT) {errExp(IdExptd, NULL, NULL); break;}
        if (cExp.type != NULL) {
          Tqualif qual = cExp.type->Qualif;
          bool strucFl = (cExp.type->typeSort == Struct);

          do {
            if (isSameName(member->hCode, member->idName, curTok.Hcod,
							       curTok.IdName)) {
              const TpTypeElt mType = member->type;

              FreeExpType2(cExp.type);
              if (mType==NULL || !(qual & ~mType->Qualif)) cExp.type = mType;
              else {  /* propagate qualifier */
                TpTypeElt newElt = copyTypeEltNoOwn(mType);

                newElt->Qualif |= qual;
                cExp.type = newElt;}
              if (member->FldFl) cExp.PseudoAttrib = Reg;  /* to prevent taking
						  the address of a bit-field. */
              goto foundMember;}
            if (strucFl && member->type!=NULL) cExp.Pval += ((IsStrun(member->
		type))? BaseStrunType(member->type) : member->type)->size; /*
							     compute l-value. */
          } while ((member = member->cdr) != NULL);
          errExp(UnknMember, cExp.type, nameToS(curTok.IdName));}
foundMember:
        GetNxtTok();
        freePrevCast();  /* type of members are completely unrelated to
								  strun type. */
        PushHist(H_DOT);
        cExp.NotPureBool = False;  /* struct/union fields independant */
        break;}  /* 'LvalFl', 'PseudoAttrib' keep their preceding value (cf.
		    cast number to pointer "(.. *)0" ); 'cExp.ptrId' not reset,
		    to be able to take notice of possible initialization. */
    case INCOP:
      resulIncOp((TkInc)curTok.Val, macParCtr);
      cExp.macLvl = macLvl;
      GetNxtTok();
      break;
    case LPAR: {		/* function call */
        const TdeclElt *formParList;
        bool exhstdFParList = False;
        TREInfo savCEinfo;
        TpTypeElt resulType = NoResulTyp, gnrcType = NULL;
        const TresulExp slExp = cExp;
        Tname lastCalledFctName = decoratedName(&cExp);
        TsemanElt *finalPtrId, *fctId = cExp.ptrId;
        TpTypeElt locAllocType = allocType, saveSizeofOpndType = sizeofOpndType;
        int savIoFctNb = ioFctNb;
        bool resulPtrSeen = False;

        ioFctNb = 0;  /* by default, not 'scanf'/'printf' family; BEWARE, used
							     by ScanFl macro. */
        TstAlwdInCstExp();
        sizeofOpndType = allocType = NULL;
        paramTxt = "()";
        if (macParCtr == lastPrimMacParCtrIM) errWN(IneffOrSideEff|
					      Warn2|PossErr, lastCalledFctName);
        if (initialType != NULL) {
          if (IsFct(initialType)) {
            if (lastCalledFctName != NULL) {
              if (initialType->typeSort == VFct) {
                static const Tname tabIOf[] = {PrintfFamily, ConvTname(""),
		      ScanfFamily};  /* 'printf' family must come first
		     (because more frequent; see also 'ioFctNb' computation). */
                const Tname *ptrTIOf;
                size_t lgtId = Lgt(lastCalledFctName);

                for (ptrTIOf = &tabIOf[0]; ptrTIOf < AdLastEltP1(tabIOf); ) {
                  if (memcmp(lastCalledFctName, *ptrTIOf++, lgtId) == 0) {  /*
					       found xscanf/xprintf function. */
                    ioFctNb = (int)(ptrTIOf - &tabIOf[SizePrintfFamily + 1 /* 
							   because of '++' */]);
                    break;}}}
              else if (curTok.Hcod==SetjmpHCode /* won't work if call of macro
		     seen for '(', but should never happen; else call directly
		    'memcmp'. */&& memcmp(lastCalledFctName, SetjmpTName,
				    LitLen(SetjmpTName))==0) cExp.SjVal = True;}
            goto correctL;}
          {
            TpcTypeElt w;

            if (IsPtr(initialType) && (w = NxtTypElt(initialType))!=NULL &&
								     IsFct(w)) {
              errWSTT(IlgLftType|Warn1|Rdbl, paramTxt, initialType, NULL);
              indir();
              initialType = cExp.type;
              goto correctL;}}
          errExp(IlgLftType, initialType, NULL);
          fctId = NULL;}
        initialType = allocTypeEltID(VFct, NoQualif);  /* just to simplify
		     coming life ('NextTE' field = 'ParamList' field = NULL). */
        initialType->NoOwner = True;
correctL:
        if (cExp.macLvl==0 && macLvl!=0) err0(LParOutsideMac|Warn1|Rdbl);
	      /* case "exit EXIT_SUCCESS", with EXIT_SUCCESS defined as "(1)" */
        savCEinfo._eInfo = cExp.Einfo;
        if (cExp.PointedByObj) fctId = NULL;
        finalPtrId = fctId;
        formParList = initialType->ParamList;
        if (NxtTok() != RPAR) {
          TpcTypeElt retType = NoResulTyp;
          int formatFl = 0;

          do {
            if (formParList!=NULL && formParList->SizeofBlFl) {
              if (locAllocType == NULL) sizeofOpndType = XAllocSeen;
              else if (locAllocType != XAllocSeen) {FreeExpType2(locAllocType)
						  ; locAllocType = XAllocSeen;}}
            asgnExpr();  /* actual parameter */
            {
              TpTypeElt eType = cExp.type;

/*              if (loopLvl!=0 && cExp.ptrId!=NULL) cExp.ptrId->ForceUsed = True;
/ case "a=f(a)" inside loop; but seems to denote bad coding, so warning left */
              paramTxt = "()";  /* paramTxt used in managePointers() */
              if (formParList == NULL) {  /* variadic part */
                CheckNumCstNamed(cExp, NxtTypElt(initialType));
                if (formatFl>0 && !folwngSpec(eType, (macLvl == pLCMacLvl)?
						      preLastCastType : NULL)) {
                  err0(FmtExhstd|Warn2|PossErr);
                  formatFl = -1;}  /* formatFl stays non-zero */
                freePrevCast();
                managePointers((ScanfFl)? NoConstTyp(&voidPtrTypeElt) /* type
					which is NOTALLCONST, to prevent xscanf
					parameters from being asked 'const'. */
					: NULL);
                exhstdFParList = True;}
              else {  /* non exhausted formal parameters list */
                TpTypeElt formType = formParList->type;

                if (formParList->ParQal >= GenericV) {freePrevCast()
							; heedRootType = False;}
                managePointers(formType);
                CheckNumCstNamed(cExp, formType);
                if (eType != NULL) {  /* correct actual parameter */
                  errQalTyp1 = NULL;
                  difQual = 0;
                  if (formParList->ParQal==UtilityV && eType->ParalTyp ||
			!CompatType(formType, eType, FctCall) && (lExp.type =
						  formType, !compatNumType())) {
                    heedRootType = True;
                    /* Skip over bad manners of some system header files */
                    if (!(eType->typeSort==Enum && eType->IntPoss &&
						   IsTypeSort(formType, Who))) {
                      errId3(warnOrErr(IncptblPar|UWarn|PossErr), &slExp,
					 typeToS1(eType), typeToS2(formType));}}
                  else {
                    heedRootType = True;
                    if (formType!=NULL && IsArr(formType) && cExp.
			PointingOnObj) errWSTT(ActParNotArray|Warn1|Rdbl,
							 NULL, formType, eType);
                    if (formParList->MayModifFl && (IsPtrArr(eType) || IsStrun(
					       eType))) modifPtdVal(MayM, True);
                    if (errQalTyp1 != NULL) errWSTT(ConstQalNotHeeded2|
			ConstWarn|PossErr, (errQalTyp1 == formType)? NULL :
					errTxt[SubPre], errQalTyp1, errQalTyp2);
                    if (IsArr(eType) && !cExp.LvalFl) err0(NotLValP);
/*                    if (cExp.ptrId!=NULL && eType->Qualif&VolatQal) errId1(
			      CopyVolat | Warn2, cExp.ptrId, typeToS1(eType));*/
                    if (formParList->SizeofBlFl) {
                      if ((locAllocType = sizeofOpndType)==XAllocSeen && cExp.
					  Revlbl) err0(ShdUseSizeof|Warn2|Rdbl);
                      sizeofOpndType = NULL;}
                    if (cExp.ptrId != NULL) {
                      if (formParList->SavedFl) {
                        if (cExp.LclAdr && !FoundDP(LOCALADR)) err0(
						   AsgnGblWLclAd|Warn2|PossErr);
                        if (cExp.PointingOnObj) cExp.ptrId->ForceUsed = True;
                        if (cExp.ptrId->Kind == Param) {
                          cExp.ptrId->MayNeedSaved = True;
                          if (!cExp.ptrId->Saved && !cExp.PointedByObj) {
                            errWN(FormParSaved|Warn1|Rdbl, cExp.ptrId->PdscId->
									idName);
                            cExp.ptrId->Saved = True;}}}  /* to prevent further
								    messages. */
                      else if (cExp.PointingOnObj) cExp.ptrId->PassedToFct =
									  True;}
                    if (formParList->ParQal == ResulTypV) {
                      if (IsTypeSort(eType, Num) && cExp.Revlbl &&
							   !NxtIsTypId(eType)) {
                        if (retType == NoResulTyp) retType = NxtTypElt(
								   initialType);
                        if (retType != NULL) {
                          FreeExpType2(eType);
                          eType = allocTypeEltID(computeCstType((cExp.ErrEvl)?
				      0 : cExp.Sval, eType->typeSort, retType->
							   typeSort), NoQualif);
                          eType->NoOwner = True;}}
                      if (formParList->ResulPtrFl) {
                        finalPtrId = cExp.ptrId;
                        savCEinfo.s21._lclAdr = cExp.LclAdr;
                        resulPtrSeen = True;}
                      if (resulType == NoResulTyp) resulType = arrToPtr(eType);
 				 /* because array parameter passed by address */
                      else {
                        TpTypeElt curRType, newRType = NULL, lastEltRType;

                        paramTxt = NULL;
                        if (CompatType(resulType, eType, FctCall) /* 'FctCall'
				   because of eg. "strcpy(array, array/ptr)". */
                            || (lExp.type = resulType, compatNumType()))
                          if (formParList->ResulPtrFl && !CompatType(eType,
				resulType, FctCall)) curRType = QuasiNULLval(
								     TpTypeElt);
                          else goto noChangeL;
                        else if (compatType(eType, resulType, FctCall)
                              || (lExp.type = eType, cExp.type = resulType,
				   compatNumType())) curRType = arrToPtr(eType);
                        else {
                          TpTypeElt curEType = eType;

                          curRType = resulType;
                          /* Create a copy of possible type modifiers */
                          while (! NxtIsTypId(curRType)) {
                            TpTypeElt w = allocTypeEltIC(curRType, NoQualif);

                            if (newRType == NULL) {newRType = w; w->NoOwner =
									  True;}
                            else {lastEltRType->StopFreeing = False
						    ; lastEltRType->NextTE = w;}
                            lastEltRType = w;
                            curEType = curEType->NextTE;
                            if (curRType->NextTE == NULL) break;
                            curRType = curRType->NextTE;}
                          while (NxtIsTypId(curRType) && !curRType->RootTyp) {
                            curRType = Parent(curRType);
                            if (compatType(curRType, curEType, FctCall)) {
                              if (resulPtrSeen) goto skipErrL;
                              goto foundCommonAncestorL;}}
                          errWNTT(IncompTypes|Warn2|PossErr, lastCalledFctName,
							      resulType, eType);
                          curRType = NULL;
skipErrL:;}
                        if (resulPtrSeen) {
                          if (curRType != NULL) errWSTTS(
				ResulPtrNotHghsTyp|Warn2|PossErr,
				dpName((formParList->ResulPtrFl)? RESULTYP :
								      RESULPTR),
				eType,
				resulType,
				(fctId!=NULL && fctId->SysElt)? NULL :
							  &"/*~ResultPtr*/"[0]);
                          if (formParList->ResulPtrFl) {
                            if (! GtPtr(curRType, QuasiNULLval(TpTypeElt))
						  ) curRType = arrToPtr(eType);}
                          else {TpTypeElt w = resulType;; resulType =
						       curRType; curRType = w;}}
                        if (newRType != NULL) {FreeExpType2(newRType)
							     ; newRType = NULL;}
foundCommonAncestorL:
                        FreeExpType1(resulType);
                        resulType = (newRType == NULL)? curRType :
				    (lastEltRType->NextTE = curRType, newRType);
noChangeL:;}}}
                  if (formParList->ParQal == GenericV) {
                    if (gnrcType != NULL) {
                      if (! CompatType(gnrcType, eType, FctCall)) {
                        if (compatType(eType, gnrcType, FctCall)) {
                          FreeExpType2(gnrcType);
                          goto changeTypL;}
                        errWNTT(IncompTypes|Warn2|PossErr, lastCalledFctName,
							     gnrcType, eType);}}
                    else
changeTypL:
                      gnrcType = arrToPtr(eType);}}
                if ((formParList = formParList->cdr)==NULL && ioFctNb!=0) {
                  if (eType!=NULL && IsArr(eType) && eType->Generiq && !cExp.
				  ErrEvl) {  /* correct literal format string */
                    initFlwgSpec();
                    formatFl = 1;}}}
              if (eType!=resulType && eType!=gnrcType) FreeExpType1(eType);}  /*
				check for type identity OK, for there could
				only be a problem with 'arrToPtr', but then
				type shared (cast to Array not possible). */
          } while (Found(COMMA));
          if (curTok.tok != RPAR) errMsngRPar();  /* here to be before 
						   'BadActParListLgth' error. */
          FreeExpType1(gnrcType);
          if (formatFl != 0) {  /* 'initFlwgSpec()' called */
            if (formatFl>0 && folwngSpec(NULL, NULL)) err0(TooManyFmtSpe|Warn2|
								       PossErr);
            resetFlwgSpec();}}
        if (formParList!=NULL
            || exhstdFParList
             && initialType->typeSort!=VFct
             && !(initialType->ParamList==NULL && fctId!=NULL && fctId->SysElt)
				) errId3(BadActParListLgth, &slExp, NULL, NULL);
        ioFctNb = savIoFctNb;
        cExp.Einfo = 0;  /* reset all flags */
        cExp.PseudoAttrib = Stati;  /* in case result is pointer */
        cExp.FctCallSeen1 = resulPtrSeen ^ True;
        cExp.hist = H_EMPTY;
        cExp.NotPureBool = savCEinfo.s21._lclAdr /* no _notPureBool field... */;
        if (cExp.SjVal = savCEinfo.s21._sjVal) setjmpSeen = True;
        cExp.FctCallNoResTyp = initialType->ResTypPoss;
        cExp.type = advInTypeChain(initialType);
        cExp.topOper = (cExp.type != NULL)
                         ? (cExp.type->typeSort == Void)
                           ? (nxtStmtRchbl = (initialType->PvNr /* NeverReturns
				     */)? NonRchbl_Jmp : Rchbl, VFCTC | PRIMARY)
                           : (initialType->PvNr /* PseudoVoid */)
                             ? VFCTC | PRIMARY
                             : FCTC | PRIMARY
                         : VFCTC | PRIMARY;
        if (resulType != NoResulTyp) {
          if (cExp.type!=NULL && (!IsTypeSort(cExp.type, Flt) || resulType==
					  NULL || IsTypeSort(resulType, Flt))) {
            if (resulType != NULL) {
              TpcTypeElt rType, eType;

              rType = (resulType->typeSort == Ptr)? (eType = NxtTypElt(cExp.
		  type), NxtTypElt(resulType)) : (eType = cExp.type, resulType);
              if (rType!=NULL && eType!=NULL && rType->typeSort!=eType->
					       typeSort) cExp.ArtifType = True;}
            FreeExpType2(cExp.type);
            cExp.type = resulType;}
          else FreeExpType1(resulType);}
        (void)Found(RPAR);  /* here for good error position (also for good
						management of 'usrLvlParFl'). */
        cExp.ptrId = finalPtrId;
        cExp.FctCallSeen = True;
        cExp.SideEff = True;  /* a priori */
        lastCalledFct = slExp.ptrId;  /* for 'NotPureSideEffect1' error */
        allocType = locAllocType;
        sizeofOpndType = saveSizeofOpndType;
        break;}
    case LSBR: {		/* indexation */
        TresulExp slExp;
        TpTypeElt lType;
        size_t ptdSize;
        bool savAO = apsndOpnd;

        apsndOpnd = False;
        if (initialType != NULL) {
          if (! IsPtrArr(initialType)) {
            paramTxt = "[]";
            errExp(IlgLftType, initialType, NULL);
            ptdSize = 0;}
          else {
            ptdSize = sizeOfTypeI(initialType, True, (IsArr(initialType))?
						   IncplArray : IncplPtr, NULL);
            if (IsPtr(initialType)) checkInit();}}  /* pointer represents
				       itself; array represents its elements. */
        slExp = cExp;
        GetNxtTok();
        commaExpr();
        checkInit();
        lType = slExp.type;
        CheckNumCstNamed(cExp, (lType==NULL || lType->BndType==NULL)? cExp.
							 type : lType->BndType);
        cleanExprThings();
        apsndOpnd = savAO;
        cExp.PseudoAttrib = slExp.PseudoAttrib;
        cExp.FctCallSeen = False;
        if (lType != NULL) {
          const TpTypeElt eType = cExp.type;
          Tqualif qual = NoQualif;  /* qualifier to be propagated */

          lExp.type = lType->BndType;
          if (eType != NULL) {
            if (! IsTypeSort(eType, WhoEnumBool)) errWNT(BadIndexType1,
						  decoratedName(&slExp), eType);
            else {
              if (lType->BndType != NULL) {  /* array with bound type
						   specified (without error). */
                if ((slExp.ptrId==NULL || !slExp.ptrId->NoErrTypInd) &&
		    !(cExp.Sval==0 && eType==&intCstTypeElt && InsideInterval(
			NakedTok(cExp.topOper), CSTNU, CSTNU1) /* plain 0 */)) {
                  if (paramTxt = NULL, !compatNumType()) errId3(BadIndexType|
			Warn1|PossErr, &slExp, (lType->BndType ==
			   &defaultIndexTypeElt)? errTxt[Arithm] :
			     typeToS2NoQual(lType->BndType), typeToS1(eType));
                  else if (moreIndexTypeChk && eType->ParalTyp && isRepreType(
			lType->BndType)) errId3((IsPtr(lType))?
			UseIndexTypForPtr | Warn1 : BndShdBeOfNamedType | Warn1,
			&slExp, (lType->BndType == &defaultIndexTypeElt)?
			errTxt[Arithm] : typeToS2NoQual(lType->BndType),
							       typeToS1(eType));
                  else goto noErrL;
                  if (!allErrFl && slExp.ptrId!=NULL) slExp.ptrId->NoErrTypInd
									 = True;
noErrL:;}}
              if (IsPtr(lType)) indirindex(&slExp);
              else {
                if (lType->Lim + 1U > 1U) {  /* array with size
						   specified (without error). */
                  TcalcS valBound = (TcalcS)lType->Lim;

                  if (cExp.Revlbl) {
                    if (!cExp.ErrEvl
                        && (IsTypeSort(eType, (SigAri | Enum)) && cExp.Sval>
									valBound
                         || IsTypeSort(eType, UnsigWho) && cExp.Uval>lType->Lim
                         || !apsndOpnd && cExp.Sval==valBound)) {
                      TnbBuf w;

                      bufGrstIntToS(valBound, w);
                      errWSS(IndexValTooBig|Warn3|PossErr,
						    grstIntToS(cExp.Sval), w);}}
                  else if (eType->typeSort < computeLstType(valBound - 1)
				) errId3(IndexTypeTooSmall | Warn2, &slExp,
					typeToS1(eType), grstIntToS(valBound));}
                if (cExp.Revlbl && !cExp.ErrEvl && cExp.Sval<0 && IsTypeSort(
		       eType, (SigAri | Enum))) err0(NegIndexVal|Warn3|PossErr);
                cExp.NotPureBool = slExp.NotPureBool;  /* also .LclAdr */
                cExp.PointedByObj = (slExp.ptrId!=NULL && slExp.ptrId->Kind==
			Param && slExp.ptrId->type!=NULL && IsArr(slExp.ptrId->
					      type))? True : slExp.PointedByObj;
                if (cExp.FctCallSeen1 = slExp.FctCallSeen1)
                  if (slExp.FctCallSeen) cExp.FctCallSeen = True;
                qual = lType->Qualif;}}
            FreeExpType2(eType);}
          cExp.type = advInTypeChain(lType);
          if (qual!=NoQualif && cExp.type!=NULL && qual & ~cExp.type->Qualif) {
            /* Propagate qualifier */
            TpTypeElt w = copyTypeEltNoOwn(cExp.type);

            w->Qualif |= qual;
            cExp.type = w;}}
        else {FreeExpType(cExp); cExp.type = NULL;}
        cExp.Levlbl = slExp.Levlbl & cExp.Revlbl;
        if (cExp.Levlbl && !cExp.ErrEvl) cExp.Pval = slExp.Pval +
						      cExp.Sval*(TcalcS)ptdSize;
        cExp.OldUsed = slExp.OldUsed;
        cExp.hist = slExp.hist;
        if (! slExp.PointingOnObj) PushHist(H_ARRAY);
        cExp.ptrId = slExp.ptrId;
        if (! Found(RSBR)) errWS(Msng, "]");
        cExp.LvalFl = ! cExp.FctCallSeen;  /* case "&fct().tab[0]" */
        cExp.CstImpsd = False;
        cExp.Revlbl = False;
        cExp.UnsigDiff = False;
        cExp.PointingOnObj = False;
        cExp.FctCallNoResTyp = False;
        if (slExp.SideEff) cExp.SideEff = True;
        cExp.topOper = LSBR | PRIMARY;
        break;}  /* cExp.ptrId not reset, for example to be able to take
				  notice of possible initialization of array. */
    default:
      return;}}
}

static void resulIncOp(TkInc oper, TmacParCtr ctrMP)
{
  TpTypeElt eType;

  TstAlwdInCstExp();
  paramTxt = txtIncTok[oper];
  manageUsedForIncOp(); /* here because of 'usrLvlParFl' */
  if (ctrMP == lastPrimMacParCtrIM) err1(SideEffInMacPar|Warn3|PossErr);
  if ((eType = cExp.type) != NULL) {
    if (! cExp.LvalFl) {errWSS(NotLVal, paramTxt, NULL); cExp.ptrId = NULL;}
    else {
      if (IsPtr(eType)) (void)sizeOfTypeI(eType, True, UndefPtdSize, paramTxt);
      else if (! IsTypeSort(eType, NumEnum)) errExp(IlgOpndType, eType, NULL);
      if (eType->Qualif & ConstQal) errCompoOf(True);
      else if (cExp.PointedByObj) modifPtdVal(WillM, False);
      if (cExp.ptrId!=NULL && IsStrun(cExp.ptrId->type)  && !cExp.PointedByObj
					       ) cExp.ptrId->StrunMdfd = True;}}
  cExp.SideEff = True;
  cExp.LvalFl = False;
}

static void specialCaseOp(TkAdd oper)
/* If 'AddSubAsgn': does not change either cExp.ptrId or lExp.ptrId,
   else if 'pointer +/- nb' sets cExp.ptrId to lExp.ptrId (done in fact by
								   resulExp() ),
   else sets cExp.ptrId to NULL. */
{
  const TpTypeElt lType = lExp.type, eType = cExp.type;
  const TtypeSort tsl = lType->typeSort;

  if (tsl==Enum /* enum +/- distance */
      || tsl & NumEnum && lType->RootTyp && lExp.Revlbl &&
			       cExp.Revlbl /* so that "(RootType)3 + 1" OK */) {
    /* Heterogeneous operands possible */
    if (eType != NULL) {
      if (!eType->ParalTyp && eType->typeSort & (Int | Short | Byte)) {
        if (eType->SysTpdf && !cExp.Revlbl && chkPortbl) errWSTT(NonPortOpnd |
						UWarn2, paramTxt, eType, lType);
        if (oper != AddSubAsgn) {
          FreeExpType2(eType);  /* in order to get left operand result type */
          cExp.type = lType;
          cExp.ptrId = NULL;
          lExp.macLvl = cExp.macLvl;  /* to prevent additional warnings */
          resulExp(NumEnum, False, (oper == Add)? &computeAdd : &computeSub,
							   cExp.macLvl, ADDOP);}
        goto exitL;}}}
  else if (IsPtrArrSort(tsl)) {
    size_t ptdSize;

    {
      const TresulExp w = cExp;

      cExp = lExp;
      ptdSize = sizeOfTypeI(lType, True, UndefPtdSize, paramTxt);
      cExp = w;}
    if (eType != NULL) {
      if (oper==Sub && IsPtr(eType)) {
        difQual = 0;
        if (!CompatType(lType, eType, NoCheck) && !compatType(eType, lType,
		NoCheck) || !CompatType(lType->BndType, eType->BndType,
				VerifCast)) errWSTT(warnOrErr(DiffIncptblPtr|
						  PossErr), NULL, lType, eType);
        else if (lExp.LclAdr!=cExp.LclAdr
                 && (loopLvl==0
                     || condStmtLvl==initialLoopCondLvl
                     || lExp.PointingOnObj
                      && cExp.PointingOnObj)) err0(MnlsPtrDiff|Warn3|PossErr);
        FreeExpType2(eType);
        cExp.type = NoConstTyp(managePredefTypes(PTRDIFF_T));
        if (cExp.Revlbl) {
          if (ptdSize!=0 && !cExp.ErrEvl) cExp.Sval = (TcalcS)((lExp.Pval -
						 cExp.Pval)/(ptrdiff_t)ptdSize);
          cExp.type = copyGeneric(cExp.type);}
        cExp.PointingOnObj = False;
        goto freeLTypeL;}
      if (! IsTypeSort(eType, WhoEnumBool)) errWSTT(IlgRhtType, paramTxt,
								   eType, NULL);
      else {
        TsemanElt *ptrLId;

        lExp.type = (lType->BndType != NULL)? lType->BndType :
					       NoConstTyp(&defaultIndexTypeElt);
        if (((ptrLId = lExp.ptrId)==NULL || !ptrLId->NoErrTypInd) &&
		  !(cExp.Uval<=1 && eType==&intCstTypeElt && InsideInterval(
		      NakedTok(cExp.topOper), CSTNU, CSTNU1) /* plain 0/1 */)) {
          Tstring savParamTxt = paramTxt;

          if (paramTxt = NULL, !compatNumType()) errId3(BadIndexType|Warn1|
		PossErr, &lExp, (lExp.type == &defaultIndexTypeElt)? errTxt[
			  Arithm] : typeToS2NoQual(lExp.type), typeToS1(eType));
          else if (moreIndexTypeChk && eType->ParalTyp && isRepreType(lExp.
		  type)) errId3(UseIndexTypForPtr | Warn1, &lExp, (lExp.type
		    == &defaultIndexTypeElt)? errTxt[Arithm] : typeToS2NoQual(
						   lExp.type), typeToS1(eType));
          else goto noErrL;
          if (!allErrFl && ptrLId!=NULL) ptrLId->NoErrTypInd = True;
noErrL:
          paramTxt = savParamTxt;}}}
    if (oper != AddSubAsgn) {
      if (cExp.Revlbl) cExp.Pval = (oper == Add)? lExp.Pval + cExp.Sval*
			(TcalcS)ptdSize : lExp.Pval - cExp.Sval*(TcalcS)ptdSize;
      if (compatType(preLastCastType, NoConstTyp(&voidPtrTypeElt), VerifCastF)
		   ) freePrevCast();  /* case "void *a;...;a = (int *)a + 1;" */
      FreeExpType1(eType);
      if (tsl == Array) {  /* "tab + i" */
        errWSTT(IlgLftType|Warn1|Rdbl, paramTxt, lType, NULL);
						       /* not string constant */
        (cExp.type = copyTypeEltNoOwn(lType))->typeSort = Ptr;}
      else cExp.type = lType;
      cExp.hist = lExp.hist;}
    cExp.LclAdr = lExp.LclAdr;
    cExp.FctCallNoResTyp = lExp.FctCallNoResTyp;
    cExp.PseudoAttrib = lExp.PseudoAttrib;
    goto exitL;}
  if (oper==Add && eType!=NULL &&
       (!lType->ParalTyp
         && (eType->typeSort==Enum || tsl & NumEnum && lType->RootTyp && lExp.
							  Revlbl && cExp.Revlbl)
         && tsl & (Int | Short | Byte)
        || (IsPtr(eType) || IsArr(eType) && eType->Generiq)
         && IsWhoEnumNotChar(lType)))
    errExp(TryPermut|Warn2|Rdbl, lType, typeToS2(eType));
  else if (oper == AddSubAsgn) errAsgn = True;
  else {
    if (oper == Sub) lExp.Revlbl = False;
    errWrngType(NumEnum | PtrPoss);}  /* 'PtrPoss' because Array<=>Ptr */
freeLTypeL:
  cExp.LclAdr = False;
  if (oper != AddSubAsgn) {FreeExpType2(lType); cExp.ptrId = NULL;}
exitL:
  cExp.CstImpsd = False;  /* well-defined type anyway */
  cExp.UnsigDiff = False;
}

/******************************************************************************/
/*                                 UTILITIES                                  */
/******************************************************************************/

static TpTypeElt advInTypeChain(TpTypeElt x)
{
  TpTypeElt nxtType = NxtTypElt(x);

  if (x->NoOwner) {
    if (nxtType!=NULL && !nxtType->Shared && !x->StopFreeing) nxtType->NoOwner 
				    = True;  /* propagate 'NoOwner' attribute */
    (void)freeTypeElt(x);}
  return nxtType;
}

static TpTypeElt arrToPtr(TpTypeElt type)
{
  if (! IsArr(type)) return type;
  {
    TpTypeElt nType = copyTypeEltNoOwn(type);

    nType->typeSort = Ptr;
    nType->size = PtrSiz;
    nType->Generiq = False;
    nType->ParalTyp = False;
    nType->SysTpdf = False;
    nType->NextTE = NxtTypElt(type);
    if (nType->Qualif&ConstQal && nType->NextTE!=NULL && !(nType->NextTE->
			Qualif & ConstQal)) {  /* propagate 'const' qualifier */
      TpTypeElt nNxtType = copyTypeEltNoOwn(nType->NextTE);

      nNxtType->Qualif |= ConstQal;
      nType->NextTE = nNxtType;
      nType->StopFreeing = False;}
    return nType;}
}

static bool authzdType(TpcTypeElt type, TtypeSort msk, bool *pErrFl, Terr x)
{
  TtypeSort msk1, ts;

  if (type==NULL || (((ts = type->typeSort) < DelType)
                       ? ts
                       : (ts==Ptr || ts==Array && type->Generiq)
                         ? PtrPoss
                         : (IsStrunSort(ts))
                           ? StrunPoss
                           : Void) & msk) return True;  /* type authorized
								      by dcc. */
  msk1 = (msk & WhoEnumBool)? WhoEnumBool : Void;  /* 'real' C integral types */
  if (msk & (Flt | BoolOpnd)) msk1 |= Flt;
  errWSTT(x | (((InsideInterval(ts, Ptr, VFct))? msk&(PtrPoss | BoolOpnd) :
		ts&msk1)? Warn2 : (*pErrFl = True, Err)), paramTxt, type, NULL);
  return False;
}

static void castValue(TtypeSort ts)
/* Truncate value (making no assumption as to internal representation of
   numbers). */
{
  if (ts & UnsigWho) ts &= UnsigWho;
  switch (ts) {	/* *doIt* float/crossCompil */
#if defined(LONGLONG) || ULONG_MAXC>UINT_MAXC
    case Long:	cExp.Sval = (TcalcS)(long)cExp.Sval; break;
    case ULong:	cExp.Uval = (TcalcU)(ulong)cExp.Uval; break;
#endif
    case Int:	cExp.Sval = (TcalcS)(int)cExp.Sval; break;
    case UInt:	cExp.Uval = (TcalcU)(uint)cExp.Uval; break;
    case Short:	cExp.Sval = (TcalcS)(short)cExp.Sval; break;
    case UShort:cExp.Uval = (TcalcU)(ushort)cExp.Uval; break;
    case Byte:	cExp.Sval = (TcalcS)(byte)cExp.Sval; break;
    case UByte:	cExp.Uval = (TcalcU)(ubyte)cExp.Uval; break;
    /*~ NoDefault */}
}

static void checkIncldFiles(const TsemanElt *ptrId)
{
  if (chkInclPos
      && !insideHdrFile
      && !ptrId->Defnd
      && (!ReallyInsideMacro || curMacDFName()==curFileName)
      && ptrId->DeclaringFile!=nonFileName
      && !isBodyHdrFile(ptrId->DeclaringFile)
      && addLvl0InclFName(ptrId->DeclaringFile)) errWNSS(BadlyIncldHdrFile|
	      Warn1|Rdbl, ptrId->PdscId->idName, ptrId->DeclaringFile, NULL);
					   /* message only once for each file */
}

static void checkInfoLoss(TpcTypeElt x)
/* Check for loss of information (constant expression described by cExp too
   big for 'x' type). */
{
  if (!cExp.ErrEvl && x!=NULL) {
    TtypeSort ts = x->typeSort;

    if (ts == Bool) ts = UInt;
    else if (ts == Enum) ts = (x->TagId == NULL)?
#if SignedChar
						  Byte
#else
						  UByte
#endif
							: Int; /* 'real' enum */
    if (cExp.type!=NULL && computeTypeSort(cExp.Sval, cExp.type->typeSort,
					   ts)>ts) {errOvfl(x); castValue(ts);}}
}

void checkInit(void)
{
  register TsemanElt *ptrId;

  if ((ptrId = cExp.ptrId)!=NULL && evaluateFl) {
    if (!ptrId->Initlz
        && ptrId->Kind==Obj
        && ptrId->Attribb>=Auto
        && (!cExp.PointingOnObj || initDPFl)
        && (!ScanfFl || cExp.type!=NULL && IsPtr(cExp.type))) {
      if (loopLvl==0 && ptrId->Attribb!=StatiL || condStmtLvl==
		initialLoopCondLvl) errInit(ptrId, 0, NULL, initDPFl);  /* must
							      be initialized. */
      else addNotInitVarList(ptrId, initDPFl);}}  /* Might be initialized later,
			   so add to list to be checked at end of loop/block. */
}

void checkNumCstNamed(TresulExp *pExp, TpcTypeElt sinkType)
{
  if ((pExp->type==NULL || !pExp->type->LitCsta) && !CheckSink(pExp, sinkType)){
    if (! chkNumCst) return;
    err0((pExp->LitCstOutsMac)? NumCstShdBeNamed|Warn1|Rdbl :
						 NumCstShdBeNamed1|Warn1|Rdbl);}
  pExp->LitCst = False;
  pExp->LitCstOutsMac = False;
}

static void checkPureBool(void)
{
  if (lExp.type!=NULL && lExp.type->typeSort==Bool || cExp.type!=NULL &&
						    cExp.type->typeSort==Bool) {
    warnNotPureBool(&lExp, errTxt[Left]);
    warnNotPureBool(&cExp, errTxt[Right]);}
}

void checkSideEffect(void)
/* Done that way so that warns even if bad expression type */
{
  const TpTypeElt eType = cExp.type;

  CheckNumCstNamed(cExp, eType);
  if (! IsSideEffect(cExp)) {
    if (NakedTok(cExp.topOper) == FCTC) {
      TfctInfo fctInf = fctInfo(lastCalledFct);

      if (goodPrgmg || fctInf>=NOINFO) {
        cExp.ptrId = lastCalledFct;
        errWNSS(NotPureSideEffect1|UWarn2|PossErr, decoratedName(&cExp),
		(lastCalledFct == NULL)
		   ? NULL
		   : errTxt[(fctInf < NOINFO)? CastToVoid : UsePsdVoidDP],
									NULL);}}
    else {
      if (eType!=NULL && eType->typeSort!=Void) err0((IsFct(eType))
			? MsngFctParen|UWarn3|PossErr
			: NotPureSideEffect|UWarn2|PossErr);}
    cExp.ptrId = NULL;}
  else {
    if (NakedTok(cExp.topOper) == VFCTC) cExp.ptrId = NULL;}  /* for ~PseudoVoid
						  functions with ~ResultType. */
  valueNotUsed();  /* so that, for example, "a++;" => NotUsed */
  FreeExpType1(eType);
}

void cleanExprThings(void)
/* cleanExprThings (or managePointers) should be called at end of each expression,
   be it correct or not. */
{
  if (preLastCastType != NULL) freePrevCast();
  if (allocType != NULL) freeAlloc();
  if (setjmpSeen) {err0(ChkSetjmpDsbld|Warn1|PossErr); setjmpSeen = False;}
}

static TpTypeElt commonType(TtypeSort msk)
/* Returns NULL if type incompatibility between lExp.type, cExp.type and 'msk';
   else resulting type.
   To be kept coherent with 'compatType()' ("dcdecl.c") */
{
  TpTypeElt lType = lExp.type, rType = cExp.type;
  TtypeSort tsl = lType->typeSort, tsr, resTS;
  bool rhtHghrParal, retRepreType;

  convToUnsgnd = NoErrMsg;
  msk &= NumEnumBool;
  if (lType==rType && tsl&msk) return lType;
  tsr = rType->typeSort;
  resTS = tsl & tsr & msk;
  rhtHghrParal = retRepreType = False;
  {
    TpcTypeElt lTyp1, rTyp1;

    if (NxtIsTypId(lType)) {
      lTyp1 = lType; rTyp1 = rType;}
    else {
      if (! NxtIsTypId(rType)) goto mayBeOkL;  /* none parallel type */
      lTyp1 = rType; rTyp1 = lType;}
    if (! NxtIsTypId(rTyp1)) {  /* only one is parallel type */
      TpcTypeElt oldLtyp1 = lTyp1;

      /* Is it visible from the other one ? */
      do {
        if (lTyp1->RootTyp && heedRootType) {rhtHghrParal = True; goto
						     tstInsertedBool;}  /* no */
        lTyp1 = Parent(lTyp1);
      } while (NxtIsTypId(lTyp1));  /* not ParalTyp, because of RootTyp */
      if (oldLtyp1->SysTpdf && !oldLtyp1->ParalTyp && !oldLtyp1->Generiq) {
        if (oldLtyp1 == lType) goto mayBeOkL;
        goto rHiMayBeOkL;}
      /* Is one typeSort subset of the other? */
      if (resTS == Void) {   /* no */
        if (rTyp1->Generiq) {
          if (lType->Generiq) {if (tsr & Flt) tsl = tsr;}
          else {if (tsl & Flt) tsr = tsl;}
          goto mayBeOkL;}
        retRepreType = True;
        goto rHiMayBeOkL;}  /* so as to get new typeElt (because "wider repre-
					     sentation type is higher type"). */
      if (lType->ParalTyp) {
        if (rTyp1->Generiq) goto mayBeOkL;
        goto rHiMayBeOkL;}
      if (! rTyp1->Generiq) goto mayBeOkL;
      goto rHiMayBeOkL;}
    /* Both parallel type: find if one is higher and, if so, which one */
    if (lTyp1->TypeId == rTyp1->TypeId) goto mayBeOkL;  /* 'TypeId' checked
		     (and not lTyp1 == rTyp1) because of possible qualifiers. */
    for (;; lTyp1 = rType, rTyp1 = lType, rhtHghrParal = True) {
      /* Row up hierarchy */
      while (!rTyp1->RootTyp || !heedRootType) {
        rTyp1 = Parent(rTyp1);
        if (! NxtIsTypId(rTyp1)) {  /* not ParalTyp, because of RootTyp */
          if (lTyp1->SysTpdf && !lTyp1->ParalTyp) goto mayBeOkL;
          break;}
        if (rTyp1->TypeId == lTyp1->TypeId) {  /* lTyp1 higher than rTyp1 */
          rhtHghrParal = (lTyp1->Generiq != rhtHghrParal);
          goto mayBeOkL;}}
      if (rhtHghrParal) break;}  /* exit loop */
    /* no higher type */
tstInsertedBool:
    if (!(resTS & Bool) || tsl==Bool && tsr==Bool) return NULL;  /* if not
			      case "'bool' inserted" (via processBitOper() ). */
rHiMayBeOkL:
    rhtHghrParal ^= True;
mayBeOkL: ;}
  if (!(resTS & Enum) || lType->TagId!=rType->TagId) {  /* not enums or not
								  same enums. */
    /* First, take care of 'bool' case (beware of inserted Bool) */
    resTS &= Bool;
    msk &= Num;
    if ((tsl &= msk)==Void || (tsr &= msk)==Void) {
      if (resTS == Void) return NULL;}
    else {
      /* Mixing variable and constant (expression) ? */
      if (cExp.Revlbl) {
        if (lExp.Revlbl) {
          /* Parallel type constant op generic constant => parallel type */
          if (! lType->ParalTyp) {
            if (rType->ParalTyp) goto computeRepreTslL;
            goto noChangeL;}}
        else if (!cExp.CstImpsd /* not 'multi-typed' constant */ || tsl>=tsr
	       /* then, computation of real typeSort useless */) goto noChangeL;
        tsr = computeCstType(cExp.Sval, tsr, tsl);
noChangeL:;}
      else if (lExp.Revlbl && lExp.CstImpsd && tsl>tsr)
computeRepreTslL:
        tsl = computeCstType(lExp.Sval, tsl, tsr);
      /* 'Usual arithmetic conversions': find greater type */
      {
        TtypeSort gtTS;

        if (tsl == tsr) {
          if (! lExp.CstImpsd) cExp.CstImpsd = False;
          if (! lExp.FctCallNoResTyp) cExp.FctCallNoResTyp = False;
          gtTS =  tsl;}
        else if (tsl > tsr) {
          cExp.CstImpsd  = lExp.CstImpsd;
          cExp.FctCallNoResTyp = lExp.FctCallNoResTyp;
          gtTS = tsl;}
        else gtTS = tsr;
#if UINT_MAXC == ULONG_MAXC
        if (gtTS==Long && (tsl==UInt || tsr==UInt)) gtTS = ULong;
#endif
        if (gtTS&UnsigWho && !((lType->typeSort | rType->typeSort) & Bool)) {
          if (! (IsTypeSort(lType, UnsigWho|Bool)
                 || lExp.Revlbl && (lExp.Sval>=0 || lExp.ErrEvl)
                 || !lExp.Revlbl && lExp.ValMltplDef)) convToUnsgnd = Left;
          else if (! (IsTypeSort(rType, UnsigWho|Bool)
                      || IsTypeSort(lType, Enum)
                      || cExp.Revlbl && (cExp.Sval>=0 || cExp.ErrEvl)
                      || !cExp.Revlbl && cExp.ValMltplDef)) convToUnsgnd =
									 Right;}
        resTS |= gtTS;}}}
  if (!rhtHghrParal && resTS==lType->typeSort && (!lType->Generiq || rType->
							 Generiq)) return lType;
  if (!retRepreType && resTS==rType->typeSort && (!rType->Generiq || lType->
							 Generiq)) return rType;
  /* Parallel type op generic => original parallel type (because of possible
							      inserted Bool). */
  if (rType->Generiq && NxtIsTypId(lType) && resTS==lType->TypeId->NamedType->
				      typeSort) return lType->TypeId->NamedType;
  if (lType->Generiq && NxtIsTypId(rType) && resTS==rType->TypeId->NamedType->
				      typeSort) return rType->TypeId->NamedType;
  {  /* New type element needed for result */
    TpTypeElt newElt = allocTypeEltID(resTS, NoQualif);

    newElt->NoOwner = True;
    newElt->size = (resTS & lType->typeSort)? lType->size : rType->size;
    newElt->TagId = lType->TagId;  /* == rType->TagId */
    if (lType->Generiq && rType->Generiq) newElt->Generiq = True;
    return newElt;}
}

bool compatNumType(void)
{
  TpcTypeElt lType = lExp.type, eType = cExp.type;
  TpTypeElt resType;
  bool resul;

  lExp.CstImpsd = cExp.CstImpsd; lExp.Revlbl = False;
  if ((resType = commonType(mskCmpat | (TtypeSort)SIGN_BIT)) == NULL) return
									  False;
  resul = True;
  if (lType->SysTpdf!=eType->SysTpdf && lType->typeSort>=eType->typeSort &&
				  !cExp.Revlbl && paramTxt!=NULL && chkPortbl &&
		(lType->SysTpdf && InsideInterval(eType->typeSort, Long, Num) ||
		 eType->SysTpdf && !InsideInterval(lType->typeSort, Long, Num)))
    errWSTT(NonPortCmbn | UWarn1, paramTxt, lType, eType);
  else if (resType!=lType
           && (lType->ParalTyp
               || !(resType->typeSort & lType->typeSort) && !(lType->typeSort==
					    Float && resType->typeSort==Double))
           && (chkBool || !IsTypeSort(eType, Bool))) {
    if (paramTxt == NULL) resul = False;
    else {
      if (cExp.CstImpsd && !(IsTypeSort(resType, Flt) && IsTypeSort(lType,
									Who))) {
        if (! (cExp.Revlbl && (cExp.ErrEvl || computeTypeSort(cExp.Sval,
	    eType->typeSort, lType->typeSort)<=lType->typeSort))) errOvfl(
									lType);}
      else if (convToUnsgnd != NoErrMsg) {
        if (! (cExp.Revlbl && (cExp.ErrEvl || cExp.Sval>=0 && computeLstType(
		cExp.Sval)<=lType->typeSort))) errWSTT((convToUnsgnd == Left)?
		 PossOvfl|Warn2|PossErr : ConvToUnsig1|Warn2|PossErr,
						       paramTxt, lType, eType);}
      else resul = False;}}
  else if (convToUnsgnd!=NoErrMsg && paramTxt!=NULL)
    errWSTT(ConvToUnsig1|Warn2|PossErr, paramTxt, lType, eType);
  if (resType!=lType && resType!=eType) FreeExpType2(resType);
  return resul;
}

static TtypeSort computeCstType(TcalcS value, TtypeSort tsCst, TtypeSort tsTgt)
/* Returns the smallest type (compatible with 'tsTgt') in which {'value', 'ts-
   Cst'} can be represented. */
{
  TtypeSort resTS;

#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
  if (value > 0) {
#  if defined(LONGLONG)
    if ((TcalcU)value & ~(ulong)LONG_MAXC && tsCst&ULong) value = (TcalcS)
				    ((TcalcU)value ^ (~(TcalcU)0 - ULONG_MAXC));
    else if
#  else
    if
#  endif
            ((TcalcU)value & ~(uint)INT_MAXC && tsCst&UInt) value = (TcalcS)
		 ((TcalcU)value ^ (~(TcalcU)0 - UINT_MAXC));}  /* sign-extend
							for 'computeLstType'. */
#endif
  tsTgt = (tsTgt & (SigAri | Enum))? SigAri : UnsigWho;
  if (tsCst & (SigAri | Enum)) tsCst = SigAri;
  else {
    if (tsTgt == SigAri) return tsCst;  /* do not loose fact that -1u >
								     INT_MAX. */
    tsCst = UnsigWho;}
  if ((resTS = computeLstType(value)) == Bool) resTS = Byte;
  if (! (resTS & tsCst & tsTgt)) {  /* keep 'signed'/'unsigned' property;
						  beware of case "i = -250u". */
    TtypeSort msk = (value < 0)? tsCst : resTS;

    for (;;) {
      if (! (resTS & msk)) resTS = 
#if USHRT_MAXC == UINT_MAXC
				    (resTS == UShort)
				      ? Long :  /* because 40000=>UShort=>Long
						(not Int), if 'int' is defined
						to be equivalent to 'short'. */
#elif UINT_MAXC == ULONG_MAXC
				    (resTS == UInt)
				      ? ULong :
#endif
				    resTS << 1;
      if (msk == tsTgt) break;
      msk = tsTgt;}}
  return resTS;
}

static TtypeSort computeTypeSort(TcalcS value, TtypeSort tsCst, TtypeSort tsTgt)
/* Returns the smallest type (compatible with 'tsTgt') in which {'value', 'ts-
   Cst'} can be represented. */
{
  TtypeSort resTS;

#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
  if (value > 0) {
#  if defined(LONGLONG)
    if ((TcalcU)value & ~(ulong)LONG_MAXC && tsCst&ULong) value = (TcalcS)
				    ((TcalcU)value ^ (~(TcalcU)0 - ULONG_MAXC));
    else if
#  else
    if
#  endif
            ((TcalcU)value & ~(uint)INT_MAXC && tsCst&UInt) value = (TcalcS)
		 ((TcalcU)value ^ (~(TcalcU)0 - UINT_MAXC));}  /* sign-extend
							for 'computeLstType'. */
#endif
  if (value>=0 || tsCst & (SigAri | Enum) || !(tsTgt & (SigAri | Enum))) {
    if ((resTS = computeLstType(value)) == Bool) resTS = Byte;
    return resTS;}
#if USHRT_MAXC  == UINT_MAXC
  if (tsCst == UShort) return UInt;
#endif
#if UINT_MAXC == ULONG_MAXC
  if (tsCst == UInt)   return ULong;
#endif
  return tsCst;
}

static TtypeSort computeLstType(TcalcS value)
/* Gives back first type large enough for value 'value' */
{
  /* *doIt* float */
  if ((TcalcU)value <= 1) return Bool;
  if ((TcalcU)(value - SCHAR_MINC) <= (TcalcU)(SCHAR_MAXC-SCHAR_MINC)) return
									   Byte;
  if ((TcalcU)value <= UCHAR_MAXC) return UByte;
  if ((TcalcU)(value - SHRT_MINC) <= (TcalcU)(SHRT_MAXC - SHRT_MINC)) return
									  Short;
  if ((TcalcU)value <= USHRT_MAXC) return UShort;
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
  if ((TcalcU)(value - INT_MINC) <= (TcalcU)((TcalcS)INT_MAXC - INT_MINC)
								   ) return Int;
  if ((TcalcU)value <= UINT_MAXC) return UInt;
#  ifdef LONGLONG
  if ((TcalcU)(value - LONG_MINC) <= (TcalcU)((TcalcS)LONG_MAXC - LONG_MINC)
								  ) return Long;
  if ((TcalcU)value <= ULONG_MAXC) return ULong;
  return LLong;
#  else
  return Long;
#  endif
#else
  return Int;
#endif
}

bool computeSigAdd(TcalcS x, TcalcS y, TcalcS *pRes)
{
  register TcalcS w = x + y;

  *pRes = w;
  return ! (x>0 && y>0 && w<0 || x<0 && y<0 && w>=0
#if LONG_MINC + LONG_MAXC == 0
						    || w-y!=x
#endif
							     );
}

static TpTypeElt copyGeneric(TpTypeElt x)
{
  TpTypeElt resul = copyTypeEltNoOwn(x);

  resul->Generiq = True;
  return resul;
}

static TpTypeElt copyTypeEltNoOwn(TpTypeElt x)
{
  if (x->NoOwner) return x;
  {
    TpTypeElt resul = allocTypeEltIC(x, NoQualif);

    resul->NoOwner = True;
    return resul;}
}

static void createBoolResult(void)
{
  FreeExpType(cExp);
  cExp.type = NoConstTyp((sysHdrFile && ifDirExp)? &intCstTypeElt : (cExp.
				       Revlbl)? &boolCstTypeElt : &boolTypeElt);
  cExp.NotPureBool = False;
  cExp.UnsigDiff = False;
  cExp.CstImpsd = False;
}

static void errCompoOf(bool notStrun)
{
  Tmember w = isMember(&cExp);
  Tstring compoOf = (w == NotMember)? NULL : errTxt[(w > NotMember)? ComponOf
								    : ComposOf];
  bool dotPopd = False;

  if ((cExp.hist & MskHistElt) == H_DOT) {dotPopd = True; PopHist(cExp.hist);}
  errId3((notStrun)? NotModfbl : ConstStrun, &cExp, (notStrun)? paramTxt :
						  typeToS1(cExp.type), compoOf);
  if (dotPopd) PushHist(H_DOT);
}

static void errExp(Terr x, TpcTypeElt t, Tstring str)
{
  errWSSSS(x, paramTxt, typeToS1(t), str, NULL);
  FreeExpType(cExp);
  cExp.type = NULL;
  if (! (x & WarnMsk)) cExp.ErrEvl = True;
}

static void errOvfl(TpcTypeElt type)
{
  if (! cExp.ErrEvl) {
    errWSTT((IsTypeSort(type, SigWhoEnum))? Overflow|Warn2|PossErr :
				 Overflow2|Warn2|PossErr, paramTxt, type, NULL);
    cExp.ErrEvl = True;}
}

static bool errSinkType(Terr msgNb, Tstring x) /*~PseudoVoid*/
{
  bool resul;
  TfctInfo fctInf = fctInfo(cExp.ptrId);

  msgNb = warnOrErr(msgNb);
  errWSTNSS(msgNb, x, cExp.type, decoratedName(&cExp), paramTxt,
            ((resul = (bool /*~OddCast*/)(msgNb & WarnMsk)) && !(fctInf &
							  (RESTYPFCT | SYSFCT)))
            ? (compatType(cExp.type, lExp.type, Asgn))
              ? (cExp.FctCallNoResTyp)
                ? errTxt[UseResTypOrGeneDP]
                : (cExp.FctCallSeen1)
                  ? (fctInf & MALLOCLIKEFCT)
                    ? errTxt[UseGeneAndSOMBlDP]
                    : errTxt[UseGenericDP]
                  : NULL
              : (cExp.FctCallNoResTyp)
                ? errTxt[UseResTypDP]
                : NULL
            : NULL);
  return resul;
}

static void errUdfl(void)
{
  if (! cExp.ErrEvl) {
    err1(Underflow|Warn2|PossErr);
    cExp.ErrEvl = True;
    cExp.Sval = 0;}
}

static bool errWrngType(TtypeSort msk) /*~PseudoVoid*/
/* Returns True if 'real' error (that is, error for "regular C") or either type
   is NULL (type error occurred earlier), else False. */
{
  const TpTypeElt lType = lExp.type, eType = cExp.type;
  bool resul = False, errMsg = False;

  if (! authzdType(lType, msk, &resul, IlgLftType|PossErr)) errMsg = True;
  if (! authzdType(eType, msk, &resul, IlgRhtType|PossErr)) errMsg = True;
  if (eType==NULL || lType==NULL) resul = True;
  if (!errMsg && !resul && (chkBool || !(msk & BoolOpnd))) {
    Terr n;

    difQual = 0;
    n = warnOrErr((compatType(lType, eType, IlgIcp))? IlgTypes | PossErr :
						    IncptblTypes|PossErr|UWarn);
    if (! (n & WarnMsk)) resul = True;
    errWSTTS(n, paramTxt, lType, eType, (!resul && lType->ParalTyp && eType->
		      ParalTyp && strchr(TxtTcOp, paramTxt[0])!=NULL)? errTxt[
							 UseTypCmbnDP] : NULL);}
  errExp(NoErrMsg, NULL, NULL);  /* cExp.type can be freed now */
  return resul;
}

static void freeAlloc(void)
{
  if (allocType != XAllocSeen) FreeExpType2(allocType);
  allocType = NULL;
}

static void freePrevCast(void)
{
  if (preLastCastType != NULL) {
    FreeExpType2(preLastCastType);
    preLastCastType = NULL;}
}

static bool frstMemberCompat(TpcTypeElt type, TtypeSort castTSort)
{
  const TdeclElt *member;

  if ((member = BaseStrunType(type)->MemberList) == NULL) return True;
  do {
    if (member->type==NULL || member->type->typeSort==castTSort || IsStrun(
	member->type) && frstMemberCompat(member->type, castTSort)) return True;
  } while (type->typeSort==Union && (member = member->cdr)!=NULL);
  return False;
}

static bool ignorableDP(TmacParCtr x)
{
  bool resul = /*!hereOrAloneInMac() &&*/ macParCtr!=x && !InsideMacPar;

  GetNxtTok();
  return resul;
}

static void indir(void)
{
  TpTypeElt eType = cExp.type;

  if (eType != NULL) {
    if (! (IsPtr(eType) || IsArr(eType) && eType->Generiq)) {
      errExp(((paramTxt[0] == '-')? IlgLftType : IlgRhtType) |
			       ((IsArr(eType))? Warn1|Rdbl : Err), eType, NULL);
      goto noCheckInit;}
    lExp.type = eType->BndType;  /* for possible '&' operator applied on *x */
    cExp.type = (cExp.ArtifType)? (FreeExpType2(eType), NoConstTyp(
				    &natTyp[VoidDpl])) : advInTypeChain(eType);}
  checkInit();
noCheckInit:
  cExp.Levlbl = cExp.Revlbl;
  cExp.CstImpsd = False;
  cExp.UnsigDiff = False;
  cExp.LvalFl = True;
  cExp.FctCallSeen = False;
  cExp.FctCallNoResTyp = False;
  indirindex(&cExp);
  if (! cExp.PointingOnObj) PushHist(H_PTR);
  else cExp.PointingOnObj = False;
}

static void indirindex(const TresulExp *x /*~MayModify*/)
{
  cExp.PseudoAttrib = (x->PseudoAttrib == NoAttrib)
			? NoAttrib
			: (x->LclAdr)
			  ? Auto
			  : Stati;
  cExp.Revlbl = False;
  cExp.LclAdr = False;
  if (x->PointingOnObj) {if (x->ptrId != NULL) x->ptrId->Used = True;}
  else cExp.PointedByObj = True;
}

void initExp(void)
{
  stddefIncldFl = False;
}

static Tmember isMember(const TresulExp *x)
/* Returns positive non-nul if *x describes a strun member (usually 'Member';
   'SingleMember' if sole member of struct, or member of union with no composed
   member). */
{
  if (x->ptrId==NULL || x->PointingOnObj) return NotMember;
  {
    TpcTypeElt w = x->ptrId->type;
    Thistory history = x->hist;

    if (history != H_EMPTY) {
      /* Explore history searching a 'dot' (access to a strun) */
      while ((history & (MskHistElt << (sizeof(history)*CHAR_BIT - SizeHistElt)
					 )) == H_EMPTY) history <<= SizeHistElt;
      while (history & ((H_PTR & H_ARRAY) << (sizeof(history)*CHAR_BIT -
	 SizeHistElt)) && w!=NULL) {w = NxtTypElt(w); history <<= SizeHistElt;}}
    if (x->type == w) return NotMember;
    if (history >> (sizeof(history)*CHAR_BIT - SizeHistElt) != H_DOT) return
								      Composite;
    {
      const TdeclElt *w1;

      if (w==NULL || (w1 = w->MemberList)==NULL || w->typeSort==Struct && w1->
						       cdr!=NULL) return Member;
      do {
        if (w1->type!=NULL && IsCmpsdObj(w1)) return Member;
      } while ((w1 = w1->cdr) != NULL);}}
  return SingleMember;
}

static bool manageInitlzUsed(void) /*~PseudoVoid*/
/* Answers True if current object has to be marked 'not used' */
{
  bool resul = False;
  register TsemanElt *rPtrId;

  if (!cExp.PointedByObj && (rPtrId = cExp.ptrId)!=NULL && !zifExp) {
    if (rPtrId->InitBefUsd && rPtrId->Attribb!=Extrn) {
      if (!cExp.OldUsed || !rPtrId->Defnd /* because of ~NotUsed */) {
        errWN(UslInitBU|Warn1|Rdbl, rPtrId->PdscId->idName);
        rPtrId->InitBefUsd = False;}}  /* to prevent further error */
    if (rPtrId->Attribb >= Auto) {
      if (!rPtrId->Used && rPtrId->Initlz && rPtrId->PdscId->nstLvl>condStmtLvl
          && (rPtrId->type==cExp.type
              || rPtrId->type==NULL
              || !(IsArr(rPtrId->type)
                   || IsStrun(rPtrId->type) && isMember(&cExp)==Member))
	) errWNSS(UnusedVar1|Warn2|PossErr,
                  rPtrId->PdscId->idName,
                  NULL,
                  (rPtrId->PassedToFct)
                    ? errTxt[MsngSavedQ]
                    : NULL);
      if (loopLvl==0 && rPtrId->Attribb!=StatiL) resul = True;}
    rPtrId->Initlz = True;}
  return resul;
}

#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
static void manageLeftBits(void)
/* For no wrong overflow error detection */
{
  if (cExp.Sval < 0) cExp.Uval = ~cExp.Uval;
  else if (lExp.type != NULL) {
    if (lExp.type->typeSort == UShort) cExp.Uval ^= UINT_MAXC ^ USHRT_MAXC;
    else if (lExp.type->typeSort == UByte) cExp.Uval ^= UINT_MAXC ^ UCHAR_MAXC;}
}
#endif

static void managePointers(TpTypeElt sinkType)
/* managePointers()/cleanExprThings() should be called at end of each
   expression, be it correct or not. */
{
  {
    register TsemanElt *rPtrId = cExp.ptrId;
    TpcTypeElt eType;

    if (rPtrId != NULL) {
      /* Mark possible pointed object initialized */
      if ((eType = cExp.type)!=NULL && IsPtrArr(eType)) {
        switch (tstPtd((sinkType == NULL)? eType : sinkType, True)) {
          case ALLCONST:
            if (cExp.PointingOnObj) {  /* pointer produced via '&' */
              TpcTypeElt w;

              rPtrId->Used = True;
              if (paramTxt == txtAsgnTok[SimplAsgn]) rPtrId->ForceUsed = True;
				/* if assignment to pointer, cannot trace usage
				   of pointed object's value any more. */
              if (paramTxt!=txtAsgnTok[SimplAsgn] || (w = NxtTypElt(eType))!=
						   NULL && w->Qualif&ConstQal) {
                if (rPtrId->InitBefUsd) rPtrId->Initlz = True;
                else initDPFl = True;}}  /* since receiving pointer/array
					 is 'const', pointed object should be
					 initialized (see next checkInit() ). */
            break;
          case NOTALLCONST:
            if (cExp.PointingOnObj /* produced via '&' */) {
              rPtrId->Used = True;
              if (rPtrId->type!=NULL && IsStrun(rPtrId->type)) rPtrId->
							       StrunMdfd = True;
              if (! (paramTxt[0]=='r' && cExp.LclAdr || manageInitlzUsed())
								     ) goto mpL;
              if (paramTxt == txtAsgnTok[SimplAsgn] /* assignment */) rPtrId->
							       ForceUsed = True;
              else if (sinkType != NULL) {  /* not variadic part */
                /* Pointed object supposed to be initialized/modified via recei-
	 	   ving pointer, except if 'return' and auto object; functions
		   may change value; objects can be chosen to be manipulated
		   only via pointers.
		   BEWARE: manageInitlzUsed causes side-effect. */
                if (paramTxt[0] == '(') {
                  if (ScanfFl) rPtrId->Used = False;}
                else rPtrId->Used = cExp.OldUsed;}}
            else
mpL:          if (sinkType!=NULL && paramTxt[0]=='(' && IsArr(eType)) {
                if (ioFctNb < 0) rPtrId->Used = cExp.OldUsed;  /* .printf() */
                if (manageInitlzUsed()) rPtrId->Used = False;}  /* array
						     represents its elements. */
            if (tstPtd(sinkType, False) & NOTALLCONST && !(rPtrId==lExp.ptrId
		&& paramTxt==txtAsgnTok[SimplAsgn]) /* not case "x = x->..." */
						     ) modifPtdVal(MayM, False);
            break;
          /*~NoDefault*/}}
      if (! (sinkType!=NULL && eType!=NULL && IsPtr(sinkType) && IsArr(eType))
	  || paramTxt[0]=='(') checkInit();  /* must be here (because of
						   earlier manageInitlzUsed). */
      initDPFl = False;}}
  if (preLastCastType!=NULL && pLCMacLvl==macLvl) {
    if (sinkType!=NULL && (errQalTyp1 = NULL, compatType(sinkType,
	preLastCastType, PrevCast)) && errQalTyp1==NULL) errWSTT(PrevCastUsl|
				  Warn1|Rdbl, NULL, preLastCastType, cExp.type);
    freePrevCast();}
  if (allocType != NULL) {
    if (allocType != XAllocSeen) {
      if (sinkType!=NULL && IsPtrArr(sinkType)) {
        TpTypeElt nxtType = NxtTypElt(sinkType), w = allocType;

        for (;; w = NxtTypElt(w)) {
          if (CompatType(w, nxtType, CStrictChk)) break;
          if (! IsArr(w)) {
            if (nxtType->typeSort != Void) errWSSSS(WrngSizeof|Warn3|PossErr,
			      NULL, typeToS1(w), typeToS2NoQual(nxtType), NULL);
            break;}}}}
    freeAlloc();}
  if (setjmpSeen) {err0(ChkSetjmpDsbld|Warn1|PossErr); setjmpSeen = False;}
}

static TpcTypeElt managePredefTypes(TpredefTypes typeNb)
{
  static struct _typesDes {Tname typeName; ThCode typeHCode; TpcTypeElt
       pDescType;}predefTypesDes[/*~IndexType TpredefTypes*/] =
    {{ConvTname(WcharTName), WcharTHCode, &wcharCstTypeElt},
     {ConvTname(SizeTName), SizeTHCode, &sizeofTypeElt},
     {ConvTname(PtrdiffTName), PtrdiffTHCode, &ptrdiffTypeElt}};
  /*~zif NbElt(predefTypesDes) != __extent(TpredefTypes)+1 "Bad size for array "
							   "'predefTypesDes'" */
  const struct _typesDes *const pTypeDesc= &predefTypesDes[typeNb];
  const TsemanElt *ptrId;

  if ((ptrId = searchSymTabHC(pTypeDesc->typeName, pTypeDesc->typeHCode))==NULL
							 || ptrId->type==NULL) {
    if (! stddefIncldFl) {
      err0(StddefNotIncl|UWarn2|Rdbl|PossErr);
      stddefIncldFl = True;}
    return pTypeDesc->pDescType;}
  return copyGeneric(ptrId->NamedType);
}

void manageRetValue(TpTypeElt fctRetType)
{
  TpTypeElt eType;
  
  commaExpr();
  paramTxt = "return";  /* paramTxt used in managePointers() */
  CheckNumCstNamed(cExp, fctRetType);
  managePointers(fctRetType);
  if ((eType = cExp.type) != NULL) {
    errQalTyp1 = NULL;
    difQual = 0;
    if (!CompatType(fctRetType, eType, Asgn) && (lExp.type = fctRetType,
							      !compatNumType()))
      errSinkType(BadRetType | PossErr, typeToS2(fctRetType));
    else {
      if (errQalTyp1 != NULL) errWSTTS(ConstQalNotHeeded1|ConstWarn|PossErr,
		    paramTxt, errQalTyp1, errQalTyp2, (errQalTyp1 == fctRetType)
						       ? NULL : errTxt[SubPre]);
      if (cExp.LclAdr && InsideInterval(eType->typeSort, Ptr, Union) &&
			  !FoundDP(LOCALADR)) err0(RetPtrOnLclAd|Warn3|PossErr);
      else if (fctRetType!=NULL && fctRetType->typeSort==Bool) warnNotPureBool(
		   &cExp, NULL);}  /* for function should behave as constant. */
    FreeExpType2(eType);}
}

static void manageSetjmp(void)
{
  if (! cExp.PointedByObj) {
    register TsemanElt *ptrId = cExp.ptrId;

    if (InsideInterval(ptrId->Attribb, Auto, Reg)
        && ptrId->type!=NULL && !(ptrId->type->Qualif & VolatQal)
        && ptrId->PdscId->nstLvl<=setjmpBlkLvl) ptrId->ModifSJ = True;}
}

static bool manageTypCmbn(TkOpTC oper) /*~PseudoVoid*/
{
  TpcTypeElt resulTC = searchTC(oper);

  if (resulTC == QuasiNULLval(TpcTypeElt)) return False;
  if (lExp.type->LitCsta != cExp.type->LitCsta) {
    CheckNumCstNamed(lExp, lExp.type);
    CheckNumCstNamed(cExp, cExp.type);}
  FreeExpType2(lExp.type);
  FreeExpType2(cExp.type);
  lExp.type = cExp.type = NoConstTyp(resulTC);
  return True;
}

static void manageUsedForIncOp(void)
{
  TsemanElt *rPtrId;

  checkInit();
  if (!cExp.PointedByObj && (rPtrId = cExp.ptrId)!=NULL) {
    if (loopLvl==0 && InsideInterval(rPtrId->Attribb, Auto, Reg) && !zifExp) {
      /* Not inside loop: mark variable 'notUsed' (=> to be used after) */
      cExp.OldUsed = False;
      rPtrId->Used = False;}
    if (setjmpBlkLvl != 0) manageSetjmp();}
  cExp.topOper = INCOP;
}

static void modifPtdVal(Terr n, bool x)
{
  TsemanElt *ptrId = cExp.ptrId;
  Tmember cExpS;

  if (ptrId!=NULL && !cExp.FctCallSeen1) {
    if (   ptrId->Kind==Param
        && (x || (cExpS = isMember(&cExp))!=NotMember)
        && !ptrId->MayModif
        && !(ptrId->ResulPtr && paramTxt[0]=='r' && cExp.hist==H_EMPTY)
        && (!ptrId->Dmodfd || allErrFl)
        && tstPtd(ptrId->type, False) & (ALLCONST | STRUN)
        && askConstFl) errId2((paramTxt[0]=='r' && cExp.hist==H_EMPTY &&
		ptrId->type!=NULL && IsPtr(ptrId->type))? ResulPtrM|Warn1|Rdbl
		 : MayModify|Warn1|Rdbl, ptrId, nameToS(curFctName), errTxt[n]);
    if (   n==MayM && paramTxt==txtAsgnTok[SimplAsgn]
        && (ptrId->Kind==Param || ptrId->CheckConst)
        && !cExp.PointingOnObj && cExpS<Member
        && lExp.ptrId!=NULL && lExp.ptrId->Kind==Obj && !lExp.ptrId->Dmodfd
        && isMember(&lExp)==NotMember) lExp.ptrId->CheckConst = True;  /* turn
			on receiving pointer missing 'const' qualifier check. */
    ptrId->Dmodfd = True;}
}

static void processBitOper(Tstring operName, void(*pTerm)(void), void(
			    *pCompute)(TpcTypeElt), bool cstOnRight, Ttok token)
{
  TmacLvl operMacLvl = macLvl;

  GetNxtTok();
  {
    const TresulExp slExp = cExp;  /* left operand saving */

    (*pTerm)();
    lExp = slExp;}
  {
    bool isAnd01 = False, opndAreRealBool;

    paramTxt = operName;
    if (token!=XOR && cExp.type==&boolTypeElt && lExp.type==&boolTypeElt &&
			       !cExp.SideEff) err1(NoShrtCircuit|Warn2|PossErr);
    opndAreRealBool = ((lExp.type==NULL || lExp.type->typeSort==Bool) &&
				(cExp.type==NULL || cExp.type->typeSort==Bool));
    if (token != IOR) {
      checkPureBool();
      if (token == AND)
        if (cExp.Revlbl)
          if ((savCExpVal = cExp.Uval)<=1 || cExp.ErrEvl) isAnd01 = True;
          else 
#if defined(LONGLONG) || ULONG_MAX /* not ULONG_MAXC ! */>UINT_MAXC
            manageLeftBits();
#else
            if (cExp.Sval < 0) cExp.Uval = ~cExp.Uval;  /* for no wrong over-
		   flow error detection (cf. "ushort INDIC &= ~0u ^ 0x8000;". */
#endif
    }
    resulExp(WhoEnumBool, cstOnRight, pCompute, operMacLvl, token);
    if (cExp.type!=NULL && !IsTypeSort(cExp.type, Bool)) {
      cExp.type = copyTypeEltNoOwn(cExp.type);
      cExp.type->typeSort |= Bool;}  /* result of bit operator can be used as
								     boolean. */
    if (isAnd01 || cExp.Revlbl && (cExp.Uval<=1 || cExp.ErrEvl)) cExp.
							    NotPureBool = False;
    else if (!opndAreRealBool || lExp.NotPureBool) cExp.NotPureBool = True;
    if (lExp.InhibWaNPB) cExp.InhibWaNPB = True;}
}

static void processLogOper(void (*pf)(void), bool y, Tstring z, Ttok token)
{
  TresulExp slExp;  /* left operand saving */
  TmacLvl operMacLvl = macLvl;
  bool savEvalFl = evaluateFl;
  TnstLvl oldCondLvl = condStmtLvl;

  if (cExp.ValMltplDef) cExp.Revlbl = False;
  slExp = cExp;
  if (slExp.Revlbl && (slExp.Uval != 0)==y) evaluateFl = False;
  condStmtLvl = nestLvl;
  GetNxtTok();
  (*pf)();
  if (cExp.ValMltplDef) cExp.Revlbl = False;
  evaluateFl = savEvalFl;
  /* Short-circuit evaluation */
  if (slExp.Revlbl)
    if ((slExp.Uval != 0) == y) transfOpnd(&slExp, &cExp);
    else {if (! cExp.Revlbl) warnCstBool(errTxt[SubPre]);}
  else if (cExp.Revlbl)
    if ((cExp.Uval != 0) == y) transfOpnd(&cExp, &slExp);
    else warnCstBool(errTxt[SubPre]);
  checkInit();
  cExp.ptrId = NULL;  /* to prevent 2nd call to checkInit (in resulExp) */
  condStmtLvl = oldCondLvl;
  lExp = slExp;
  paramTxt = z;
  {
    Ttok topOper = token | (cExp.topOper | lExp.topOper) & SETJMPUSED;

    if (! resulExp(mskBool, False, &computeShortCircuit, operMacLvl, token)) {
      /* Pointer(s) used as boolean */
      errWrngType(mskBool);
      FreeExpType(slExp);
      cExp.ptrId = NULL;}
    createBoolResult();
    cExp.topOper = topOper;}
}

static bool resulExp(TtypeSort msk, bool cstOnRight, void (*ptrComputeResul)
	       (TpcTypeElt), TmacLvl operMacLvl, Ttok baseClass) /*~PseudoVoid*/
/* Returns False (without freeing operand types) if lExp.type is non NULL, but
   not compatible with cExp.type nor with 'msk', and there exist a "xPoss" bit
   in 'msk'. */
{
  const TpTypeElt lType = lExp.type, eType = cExp.type;
  TpTypeElt resType;

  if (lType!=NULL && eType!=NULL) resType = commonType(msk);
  else if (baseClass == MULOP) resType = NULL;
  else {  /* if one operand type NULL, assume result type is type of other */
    if (eType == NULL) {
      if (lType == NULL) resType = NULL;
      else {
        cExp.type = lType;
        resType = commonType(msk);
        cExp.type = NULL;}}
    else {
      lExp.type = eType;
      resType = commonType(msk);
      lExp.type = NULL;}}
  if (convToUnsgnd != NoErrMsg) errWSTTS(ConvToUnsig|Warn2|PossErr, paramTxt,
					    lType, eType, errTxt[convToUnsgnd]);
  if (lExp.macLvl<operMacLvl && !(lExp.topOper & PRIMARY) && NakedTok(
	       lExp.topOper)!=baseClass /* NONASSOC (non-associativity) check */
    || cExp.macLvl<operMacLvl && !(cExp.topOper & PRIMARY) && NakedTok(
		    cExp.topOper)!=baseClass) err1(IllParenMacro|Warn3|PossErr);
  checkInit();
  cExp.PointingOnObj = lExp.PointingOnObj;
  if ((cExp.ptrId = lExp.ptrId) != NULL) checkInit();
  cExp.macLvl = operMacLvl;
  cExp.LvalFl = False;
  {
    if (cExp.LitCst) {
      if (CheckSink(&cExp, lType)) {cExp.LitCst = False; cExp.LitCstOutsMac 
							    = False; goto okL;}}
    else {
      if (!lExp.LitCst || CheckSink(&lExp, eType)) goto okL;
      cExp.LitCst = True;}
    if (lExp.LitCstOutsMac) cExp.LitCstOutsMac = True;
okL:;}
  if (lExp.UnsigDiff) cExp.UnsigDiff = True;
  if (cExp.ValMltplDef && !lExp.Revlbl) cExp.ValMltplDef = False;  /* done this
							  way for efficiency. */
  if (lExp.ValMltplDef && cExp.Revlbl && (lExp.Revlbl || cExp.Sval>=0 ||
					  cExp.ErrEvl)) cExp.ValMltplDef = True;
  if (lExp.SideEff) cExp.SideEff = True;
  if (! lExp.Revlbl) cExp.Revlbl = False;
  else {
    if (lExp.ErrEvl) cExp.ErrEvl = True;
    if (cstOnRight && !cExp.Revlbl) err1(CstOnLftSide|Warn1|Rdbl);}
  if (resType == NULL) {
    if (lType!=NULL && msk>=PtrPoss) return False;
    if (errWrngType(msk) || !cExp.Revlbl || !ifDirExp) goto exitL;
    resType = NoConstTyp(&longCstTypeElt);}  /* needed because of the poor
					  coding of many system header files. */
  if (cExp.Revlbl) {
    if (eType==NULL || lType==NULL) cExp.ErrEvl = True;
    (*ptrComputeResul)(resType);
    checkInfoLoss(resType);}
exitL:
  if (lType != resType) FreeExpType1(lType);
  if (eType != resType) FreeExpType(cExp);  /* cExp.type may have been changed
						  by errWrngType => != eType. */
  cExp.type = resType;
  cExp.topOper = baseClass;
  cExp.ptrId = NULL;
  if (preLastCastType != NULL) freePrevCast();
  return True;
}

static void resulUnOp(TtypeSort msk, Tstring operTxt)
{
  bool err = False;

  checkInit();
  paramTxt = operTxt;
  if (! authzdType(cExp.type, msk, &err, IlgRhtType | PossErr))
    if (ifDirExp && !err && cExp.Revlbl) {FreeExpType(cExp); cExp.type =
						   NoConstTyp(&longCstTypeElt);}
    else errExp(NoErrMsg, NULL, NULL);
  else if (cExp.type != NULL) {  /* compute result type (in case !(Bool | x),
								for example). */
    TpTypeElt resType;

    lExp.type = cExp.type;
    lExp.Einfo = cExp.Einfo;
    resType = commonType(msk);
    if (resType != cExp.type) {FreeExpType(cExp); cExp.type = resType;}}
  cExp.LvalFl = False;
  cExp.ptrId = NULL;
}

static size_t sizeOfTypeI(TpTypeElt type, bool deref, Terr err, Tstring y)
{
  TpTypeElt tr = (deref)? NxtTypElt(type) : type;
  TpcTypeElt t1 = tr;

  if (tr == NULL) return 0;
  if (cExp.ArtifType) {
    errId3(err, &cExp, typeToS1(&natTyp[VoidDpl]), y);
    return (&natTyp[VoidDpl])->size;}
  else {
    if (IsStrun(tr)) tr = BaseStrunType(tr);
    if (tr->size==0 && !tr->ErrSiz) {
      errId3(err, &cExp, typeToS1(t1), y);
      if (deref && tr==&natTyp[VoidDpl]) represType(type)->NextTE = tr =
						   allocTypeEltIC(tr, NoQualif);
      if (tr != &natTyp[VoidDpl]) tr->ErrSiz = True;}
    return tr->size;}
}

static void transfOpnd(const TresulExp *x /*~MayModify*/, TresulExp *y)
{
  TREInfo savCEinfo;

  savCEinfo._eInfo = y->Einfo;
  y->Einfo = x->Einfo;
  if (savCEinfo.s21._litCst && (!ifDirExp || x==&cExp)) {
    y->LitCst = True;
    if (savCEinfo.s21._litCstOutsMac) y->LitCstOutsMac = True;}
  y->UnsigDiff = savCEinfo.s21._unsigDiff;
#if PtrGtLong
  y->Pval = x->Pval;
#else
  y->Uval = x->Uval;
#endif
  y->hist = x->hist;
  y->ptrId = x->ptrId;
  if (y->type == NULL) y->type = x->type;
}

static Tstring typeToS2NoQual(TpcTypeElt x)
{
  Tstring result;
  TpTypeElt x1;

  if (x == NULL) return NULL;
  x1 = allocTypeEltIC(x, NoQualif);
  x1->Qualif = NoQualif;
  result = typeToS2(x1);
  (void)freeTypeElt(x1);
  return result;
}

void valueNotUsed(void)
{
  register TsemanElt *rPtrId;

  if (!cExp.PointedByObj && (rPtrId = cExp.ptrId)!=NULL && !(rPtrId->Kind==
	Param && (rPtrId->type==NULL || IsArr(rPtrId->type))) && !zifExp) rPtrId
	->Used = cExp.OldUsed; /* side-effect is not use (except for externals,
				       global statics, and array parameters). */
  cleanExprThings();
}

static void warnCstBool(Tstring x)
{
  if (!cstExpAsked && !ReallyInsideMacro) errWS(CstBoolExp|UWarn2|PossErr, x);
}

static void warnNotPureBool(TresulExp *x, Tstring y)
{
  if (x->NotPureBool && (allErrFl || !x->InhibWaNPB && (x->ptrId==NULL ||
	!x->ptrId->PureBoolAskd /* to minimize nb of warnings */))) {errWSS(
		 Outside01|Warn2|PossErr, paramTxt, y); cExp.InhibWaNPB = True;}
  if (x->ptrId != NULL) x->ptrId->PureBoolAskd = True;  /* limits also nb of
							      error messages. */
}

static Terr warnOrErr(Terr n)
{
#define LocalWarn Warn2

  TREInfo w;

  if (difQual != 0) {
    TnameBuf bufX;

    if ((difQual & ((MaxQualif << 1) + 1)) <= MaxQualif) {Tname w = rowXName
					   ;;rowXName = rowYName; rowYName = w;}
    bufNameToS(rowXName, bufX);
    errWNSS((difQual >= 0)? ShdContain | LocalWarn : (difQual & BITNXTSIGN)?
       NotSameBlk | LocalWarn : QlfdVariant | LocalWarn, rowYName, bufX, NULL);}
  return (compatType(lExp.type, cExp.type, CCheck)
          || lExp.type->typeSort==Ptr
           && IsTypeSort(cExp.type, WhoEnumBool)
           && (w._eInfo = (oldCEinfo._eInfo != 0)? oldCEinfo._eInfo :
						      cExp.Einfo, w.s21._rEvlbl)
           && !w.s21._errEvl
           && cExp.Uval==0
          || cExp.type->typeSort==Ptr
           && IsTypeSort(lExp.type, WhoEnumBool)
           && lExp.Revlbl
           && !lExp.ErrEvl
           && lExp.Uval==0)
          ? n | LocalWarn
          : n;
}

/* End DCEXP.C */
