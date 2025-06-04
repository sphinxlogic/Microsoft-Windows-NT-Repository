/* DCEXP.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#pragma noinline (arrToPtr, checkNumCstNamed, checkPureBool, errCompoOf)
#pragma noinline (errExp, errOvfl, errOvfl1, errUdfl, errWrngType, finalizeCast)
#pragma noinline (freeAlloc, freePrev, ignorableDP, insertBool, manageLogOper)
#pragma noinline (modifPtdVal, transfOpnd, typeToS2NoQual, warnCstBool)
#pragma noinline (warnNotPureBool, warnOrErr)
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

#define ArbitAdr	QuasiNULLval(char *)  /* arbitrary */
#define CheckSink(pExp, sinkType)					       \
  ((pExp)->rEvlbl && sinkType!=NULL && sinkType->litCsta)
#define IsChar(x)							       \
  (IsTypeSort(x, Enum) && x->tagId==NULL)
#define IsCmpsdObj(x)	x->type->typeSort >= Array
#define IsDecl1(x)	InsideInterval(curTok.tok, x, EDecl - 1)
#define IsSideEffect(x)	InsideInterval(NakedTok(x.topOper), BSideEffTok,       \
							      ESideEffTok - 1)
#define IsWhoEnumNotChar(x)						       \
  (IsTypeSort(x, Who) || IsTypeSort(x, Enum) && x->tagId!=NULL)
#define NakedTok(x)	(x & ~(PARENSEEN | PRIMARY))
#define NoResulTyp	QuasiNULLval(TpTypeElt)
#define PtrdiffTHCode	0x180904D
#define PtrdiffTName	(Tname)"\x9\0ptrdiff_t"
#define PureUnsig(t)	(t->typeSort & UnsigWho)
#define SetInfoForCst							       \
  cExp.rEvlbl = True;							       \
  if (! evaluateFl) cExp.errEvl = True;  /* to prevent computation */
#define SizeTHCode	0x321B8
#define SizeTName	(Tname)"\6\0size_t"
#define SubOP		(ADDOP | NONASSOC)
#define XAllocSeen	QuasiNULLval(TpcTypeElt)

DefIsSameName  /* define isSameName function */
DefRepresType  /* define represType function */

typedef enum {Composite = -1, NotMember, Member, SingleMember} Tmember;

/* Function profiles */
static TpcTypeElt arrToPtr(TpcTypeElt);
static void condExpr(void), primQualif(void),
  term0(void), term1(void), term2(void), term3(void), term4(void), term5(void),
  term6(void), term7(void), term8(void), term9(void), term10(void);
static void computeCond(TpcTypeElt), computeShortCircuit(TpcTypeElt),
  computeIor(TpcTypeElt), computeXor(TpcTypeElt), computeAnd(TpcTypeElt),
  computeCmp(TpcTypeElt), computeShi(TpcTypeElt), computeAdd(TpcTypeElt),
  computeSub(TpcTypeElt), computeMul(TpcTypeElt), computeDiv(TpcTypeElt),
  computeMod(TpcTypeElt);
static bool authzdType(TpcTypeElt, TtypeSort, bool *, Terr), compatNumType(void
  ), errWrngType(TtypeSort) /*~PseudoVoid*/, frstMemberCompat(TpcTypeElt,
  TtypeSort), ignorableDP(TmacParCtr), isType(void), manageInitlzUsed(void)
  /*~PseudoVoid*/, manageTypCmbn(TkOpTC) /*~PseudoVoid*/, resulExp(TtypeSort,
  bool, void(*)(TpcTypeElt), TmacLvl, Ttok) /*~PseudoVoid*/;
static void checkIncldFiles(const TsemanElt *), checkInfoLoss(TpcTypeElt),
  checkPureBool(void), errCompoOf(bool), errExp(Terr, TpcTypeElt, Tstring),
  errOvfl(TpcTypeElt), errOvfl1(TpcTypeElt, Tstring), errUdfl(void),
  finalizeCast(TpcTypeElt /*~MayModify*/), freeAlloc(void), freePrev(void),
  indir(void), indirindex(const TresulExp * /*~MayModify*/), insertBool(
  TtypeSort), manageLogOper(void (*pf)(void), Ttok, bool, Tstring),
  manageNumCst(void), managePointers(TpcTypeElt),
  manageUsedForIncOp(void), modifPtdVal(Terr), relOpTrtmt(void), resulIncOp(
  TkInc,TmacParCtr), resulUnOp(TtypeSort, Tstring), specialCaseOp(TkAdd),
  transfOpnd(const TresulExp * /*~MayModify*/, TresulExp *), valueNotUsed(void),
  verifCstOvfl(void), warnCstBool(Tstring), warnNotPureBool(TresulExp *,
  Tstring);
static TpcTypeElt advInTypeChain(TpcTypeElt), commonType(TtypeSort);
static TtypeSort computeCstType(TcalcS, TtypeSort), computeLstType(TcalcS);
static TpTypeElt copyTypeEltNoOwn(TpcTypeElt /*~MayModify*/);
static Tmember isMember(void);
static size_t sizeOfTypeI(TpcTypeElt, bool, Terr, Tstring);
static Tstring typeToS2NoQual(TpcTypeElt);
static Terr warnOrErr(Terr);

/* Objects */
static TpcTypeElt allocType = NULL;  /* type of 'sizeof' argument for
					     ~SizeOfMemBlk function argument. */
static bool apsndOpnd = False;  /* True when inside operand of '&' operator */
static bool chkNumCst1 = True;
static Terr convToUnsgnd;
static bool cstExpG = False;
static bool errAsgn;
static bool evaluateFl = True;
static bool initDPFl = False;
static TsemanElt *lastCalledFct;
static TmacParCtr lastPrimMacParCtr;
static TtypeSort mskCmpat = NumEnumBool;  /* default value */
static TpcTypeElt preLastCastType = NULL;  /* type before previous cast */
static bool ptrdiffSeen = False;
static TpcTypeElt sizeofOpndType = NULL;
static bool sizeofSeen = False;
static bool stddefNotInFl = False;  /* flag to output message only once */

/* External objects */
TpcTypeElt *limErroTypes;  /* to prevent multiple (same) errors in
							     initializations. */
TtypeSort mskBool;


bool boolExp(Tstring operTxt)
/* Returns True if boolean expression correct and constant */
{
  TpcTypeElt eType = cExp.type;

  if (cExp.ValMltplDef) cExp.rEvlbl = False;
  checkInit();
  CheckNumCstNamed(cExp, eType);
  cleanExprThings();
  if (eType != NULL) {
    if (!IsTypeSort(eType, Bool) || eType->paralTyp && eType->typeSort==Bool) {
      if (NakedTok(cExp.topOper) == ASGNOP) err0(MsngEqual | Warn3);
      else {
        Terr n = (IsScalar(eType))? BoolExptd | CWarn1 : BoolExptd;

        if (chkBool || !(n & WarnMsk)) errWSS(n, operTxt, typeToS1(eType));}}
    else if (cExp.rEvlbl) warnCstBool(NULL);
    FreeExpType2(eType);}
  return cExp.rEvlbl;
}

bool correctExprN(TpcTypeElt exptdType, TtypeSort msk, bool cstExp, Tstring
						     operName, bool checkNumCst)
/* Gets a condExpr, and detects an error if type of gotten expression not com-
   patible with 'exptdType' and/or 'msk', or if 'cstExp' = True and expression
   not constant.
   Returns False if gotten expression incorrect, or if type mismatch (cExp.type
   == NULL in both cases).
   Frees expression type only if exptdType != NoFreeExpType.
   Does not exit via 'longjmp', either directly or indirectly.
   Can be called recursively (because of 'zif'). */
{
  bool resul, savEvalFl = evaluateFl, savCstExpG = cstExpG, savChkNumCst1 =
			 chkNumCst1, freeExpType = (exptdType != NoFreeExpType);
  const Tstring savParamTxt = paramTxt;  /* because of condExpr() */
  TsemanElt *savLCF = lastCalledFct, *savLEptrId = lExp.ptrId;
  TpcTypeElt eType;
  TpTypeElt newElt;

  ignoreErr = False;
  evaluateFl = True;
  cstExpG = cstExp;
  chkNumCst1 = checkNumCst;
  condExpr();
  if (cExp.ValMltplDef) cExp.rEvlbl = False;
  cstExpG = savCstExpG;
  paramTxt = operName;  /* also for managePointers() */
  eType = cExp.type;
  if (msk != Void) {
    if (exptdType > NoFreeExpType) {
      newElt = allocTypeEltIC(exptdType, NoQualif);
      newElt->typeSort |= msk;}
    else {
      newElt = allocTypeEltID(msk, NoQualif);
      newElt->tagId = (eType!=NULL && IsTypeSort(eType, Enum))? eType->tagId
			      : GenericEnum;}  /* in case msk includes 'Enum' */
    newElt->noOwner = True;
    exptdType = newElt;}
  else newElt = NULL;
  CheckNumCstNamed(cExp, exptdType);
  lExp.ptrId = savLEptrId;
  managePointers(exptdType);
  if (eType != NULL) {
    resul = True;
    if (exptdType != NULL) {
      lExp.type = exptdType;
      if (! freeExpType) heedRootType = False;  /* for then msk!=Void */
      if (exptdType->typeSort>=DelType) { /* type expansion not needed */
        errQalTyp1 = NULL;
        if (! CompatType(exptdType, eType, Asgn)) resul = False;
        else if (errQalTyp1 != NULL) {  /* report only one of several possible
				 error messages due to bad 'const' qualifier. */
#define MaxNbRmbrdTypes		10
          static TpcTypeElt rmbrdErroTypes[MaxNbRmbrdTypes];
          TpcTypeElt *ptrErroTypes;

          if (msk!=Void || limErroTypes==NULL) limErroTypes= &rmbrdErroTypes[0];
          else
            for (ptrErroTypes = &rmbrdErroTypes[0]; ptrErroTypes !=
							       limErroTypes; ) {
              if (*ptrErroTypes++ == errQalTyp1) goto noErrL;}
          errWSSSS(ConstQalNotHeeded1|ConstWarn|PossErr, paramTxt,
	      typeToS2NoQual(errQalTyp1), typeToS1(errQalTyp2), (errQalTyp1 ==
					     exptdType)? NULL : errTxt[SubPre]);
          if (limErroTypes <= &rmbrdErroTypes[MaxNbRmbrdTypes - 1]
						 ) *limErroTypes++ = errQalTyp1;
#undef MaxNbRmbrdTypes
noErrL:;}}
      else if (! compatNumType()) resul = False;}
    heedRootType = True;
    if (! resul) {
      Terr msg = warnOrErr((msk == Void)? BadForLftType|PossErr : NotExptdType
								      |PossErr);

      if (ifDirExp && cExp.rEvlbl && (msg & WarnMsk)) {resul = True; errWSSSS(
	      msg, paramTxt, typeToS1(eType), typeToS2NoQual(exptdType), NULL);}
      else {errExp(msg, eType, typeToS2NoQual(exptdType)); eType = NULL;}}
    else if (cstExp) {
      if (! cExp.rEvlbl) {
        err0(CstExpExptd);
        if (freeExpType) resul = False;
        cExp.errEvl = True;}
      else if (IsPtrArr(eType) && cExp.pseudoAttrib>=Auto) {err0(UncomputAd)
							      ; resul = False;}}
    if (! cExp.rEvlbl)
#if PtrGtLong
      cExp.pVal = NULL;
#else
      cExp.sVal = 0;
#endif
    if (resul && !freeExpType) {
      if (eType->generic) ((ModifType)(cExp.type = copyTypeEltNoOwn(eType)))->
		generic = False;}  /* for array bound (cf. index type
					   verification if of parallel type). */
    else {
      FreeExpType1(eType);
      if (! freeExpType) cExp.type = NULL;}}
  else resul = False;
  FreeExpType1(newElt);
  paramTxt = savParamTxt;
  evaluateFl = savEvalFl;
  lastCalledFct = savLCF;
  chkNumCst1 = savChkNumCst1;
  return resul;
}

void commaExpr(void)
/* Does not exit via 'longjmp', either directly or indirectly */
{
  bool commaSeen = False;

  while (asgnExpr(), curTok.tok == COMMA) {checkSideEffect(); GetNxtTok()
							    ; commaSeen = True;}
  if (commaSeen) cExp.lValFl = False;
}

void asgnExpr(void)
{
  TmacParCtr begAsgnMacParCtr = macParCtr;

  condExpr();
  if (curTok.tok == ASGNOP) {
    TkAsgn kindAsgn;
    TresulExp slExp;		/* left operand saving */
    register TsemanElt *ptrLId;
    TpcTypeElt lType, eType;
    TmacParCtr operMacParCtr = lastPrimMacParCtr;
    bool lclAdrSeen, ignorable;

    kindAsgn = (TkAsgn)curTok.val;
    if ((lType = cExp.type) != NULL) {  /* no type error seen in left operand */
      paramTxt = txtAsgnTok[kindAsgn];
      if (! cExp.lValFl) {errWSS(NotLVal, paramTxt, errTxt[Left]); cExp.ptrId
									= NULL;}
      else if (lType->typeSort==Void || IsArrFct(lType)) errWSTT(IlgLftType,
					     txtAsgnTok[kindAsgn], lType, NULL);
      else {
        bool isConst = False;

        if (lType->qualif & ConstQal) isConst = True;
        if (IsStrun(lType)) {
          if (BaseStrunType(lType)->SynthQualif & ConstQal) isConst = True;
          (void)sizeOfTypeI(lType, False, IncplStrunType, paramTxt);}
        if (isConst) errCompoOf(lType->qualif & ConstQal);
        else if (cExp.PointedByObj) modifPtdVal(WillM);}}
    CheckNumCstNamed(cExp, lType);
    valueNotUsed();
    slExp = cExp;
    GetNxtTok();
    asgnExpr();
    lExp = slExp;
    ptrLId = slExp.ptrId;
    CheckNumCstNamed(cExp, lType);
    if (NakedTok(cExp.topOper) == ASGNOP) valueNotUsed();  /* so that "i=j=1"
						      does not mark 'j' used. */
    paramTxt = txtAsgnTok[kindAsgn];
    if (lclAdrSeen = (curTok.tok == LOCALADR)) ignorable = ignorableDP(
							      begAsgnMacParCtr);
    if ((eType = cExp.type)!=NULL && lType!=NULL) {
      errAsgn = False;
      switch(kindAsgn) {
        case SimplAsgn:
/*          if (cExp.ptrId!=NULL && eType->qualif & VolatQal) errId3(CopyVolat |
					  Warn2, cExp, typeToS1(eType), NULL);*/
          if (IsPtr(eType)) {
            if (cExp.lclAdr) {
              if (InsideInterval(lExp.pseudoAttrib, StatiL, Extrn))  /* non-
								local object. */
                if (! lclAdrSeen) err0(AsgnGblWLclAd|Warn2|PossErr);
                else lclAdrSeen = False;
              else if (ptrLId != NULL) ptrLId->lclAd = True;}
            if (ignorable) lclAdrSeen = False;}
          if (compatNumType()) {
            if (eType->generic) {
              cleanExprThings();
              FreeExpType2(lType);  /* free left operand type (not right ope-
			  rand), to allow "int = unsigned short = float = 0". */
              if (cExp.rEvlbl) {lExp.rEvlbl = True;lExp.errEvl = cExp.errEvl;}
              goto exit1L;}}
          else if ((lType->typeSort==Ptr || IsStrun(lType)) && (errQalTyp1 =
					NULL, CompatType(lType, eType, Asgn))) {
            if (errQalTyp1 != NULL) errWSTTS(ConstQalNotHeeded1|ConstWarn|
		    PossErr, paramTxt, errQalTyp1, errQalTyp2, (errQalTyp1 ==
						lType)? NULL : errTxt[SubPre]);}
          else errAsgn = True;
          goto exitL;
        case AndAsgn:
          if (cExp.rEvlbl && cExp.sVal<0) cExp.uVal = ~cExp.uVal;  /* for no
					   untimely overflow error detection. */
          /*~NoBreak*/
        case XorAsgn:
          if (lType->typeSort == Bool) checkPureBool();
          goto commonBitOpL;
        case IorAsgn:
          if (lType->typeSort==Bool && cExp.notPureBool && ptrLId!=NULL) ptrLId
							    ->notPureBoo = True;
          if (lExp.notPureBool) cExp.notPureBool = True; /* does also .lclAdr */
commonBitOpL:
          mskCmpat = WhoEnumBool;
          break;
        case LShAsgn: case RShAsgn:
          if (lType->typeSort==Void || IsArrFct(lType)) lExp.type = NULL;  /*
	       these IlgLftType checked earlier (BEWARE, lType still used !). */
          if (! IsTypeSort(lType, WhoEnum)) errWrngType(WhoEnum);
          else if (! IsWhoEnumNotChar(eType)) errExp(IlgRhtType | ((IsTypeSort(
			  eType, WhoEnum))? Warn2 | PossErr: Err), eType, NULL);
          goto endCAsgnL;
        case AddAsgn: case SubAsgn:
          mskCmpat = NumEnum;
          goto commonArithOpL;
        case MulAsgn: case DivAsgn:
          mskCmpat = Num;
          goto commonMulOpL;
        case ModAsgn:
          mskCmpat = Who;
commonMulOpL:
          if (! eType->paralTyp) ((ModifType)(cExp.type = copyTypeEltNoOwn(
		eType)))->generic = True;  /* because multiplication =
						       sequence of additions. */
commonArithOpL: {
            TpcTypeElt w;

            if (pHeadTCBLists!=NULL && (w = searchTC(AddTC + (kindAsgn -
		 AddAsgn)))!=QuasiNULLval(TpcTypeElt) && (w==NULL || w->typeId
					      ==lType->typeId)) goto endCAsgnL;}
          if (eType->typeSort==Enum && !IsPtr(lType)) goto errAsgnL;
          break;}
      if (! compatNumType()) 
        if (InsideInterval(kindAsgn, AddAsgn, SubAsgn)) specialCaseOp(
								    AddSubAsgn);
        else
errAsgnL: errAsgn = True;
endCAsgnL:
      mskCmpat = NumEnumBool;  /* back to default value */
      cExp.rEvlbl = False;
exitL:
      if (errAsgn && !(lType->typeSort==Void || IsArrFct(lType)) /* IlgLftType
							     checked earlier */)
        errWSSSS(warnOrErr(BadForLftType|PossErr), paramTxt, typeToS1(eType),
						  typeToS2NoQual(lType), NULL);}
    managePointers(lType);
    FreeExpType(cExp); /* errExp() may have been called, so don't use 'eType' */
    cExp.type = lType;
exit1L:
    if (lclAdrSeen) errIlgDP(LOCALADR);
    cExp.ptrId = ptrLId;
    lExp.notPureBool = cExp.notPureBool;  /* keep that flag (also lclAdr) */
    cExp.Einfo = lExp.Einfo;
    /* Here so that done even if r/lType == NULL */
    if (kindAsgn == SimplAsgn) {
      cExp.hist = lExp.hist;
      if (manageInitlzUsed()) {
        cExp.oldUsed = False;
        if (ptrLId != NULL) {
          ptrLId->used = False;
          ptrLId->MsngConstQM = False;}}
      cExp.topOper = ASGNOP;}  /* top operator of the syntax tree */
    else manageUsedForIncOp();
    if (cExp.notPureBool && lType!=NULL && lType->typeSort==Bool && ptrLId!=
						  NULL && !ptrLId->notPureBoo) {
      if (ptrLId->pureBoolAskd && !cExp.inhibWaNPB) {errWSS(Outside01|Warn2|
		     PossErr, paramTxt, errTxt[Right]); cExp.inhibWaNPB = True;}
      ptrLId->notPureBoo = True;}
    if (operMacParCtr!=0 && lastPrimMacParCtr==operMacParCtr) err1(
						 SideEffInMacPar|Warn3|PossErr);
    cExp.lValFl= False;}  /* the value of an assignment is not an l-value */
}

static void condExpr(void)
{
  term0();
  if (curTok.tok == QMARK) {
    TresulExp expB, slExp, scExp;
    TmacLvl operMacLvl;
    Trchbl endFrstBrchRchbl;
    bool savEvalFl = evaluateFl, sideEffect;

    (void)boolExp("?:");
    if (! (cExp.topOper & PARENSEEN)) err0(ShdBePrntzBool|CWarn1|Rdbl);
    expB = cExp;
    if (expB.rEvlbl && expB.uVal==0) evaluateFl = False;
    condStmtLvl++;
    GetNxtTok();
    commaExpr();
    operMacLvl = macLvl;
    if (! Found(COLON)) err0(ColExptd);
    endFrstBrchRchbl = nxtStmtRchbl;
    nxtStmtRchbl = Rchbl;
    slExp = cExp;
    evaluateFl = (expB.rEvlbl && expB.uVal!=0)? False : savEvalFl;
    condExpr();
    evaluateFl = savEvalFl;
    {
      bool lSideEffect = IsSideEffect(slExp), rSideEffect = IsSideEffect(cExp);

      if (expB.rEvlbl) {
        condStmtLvl--;
        if (expB.uVal != 0) {
          nxtStmtRchbl = endFrstBrchRchbl;
          transfOpnd(&slExp, &cExp);
          rSideEffect = True;}
        else {transfOpnd(&cExp, &slExp); lSideEffect = True;}}
      sideEffect = lSideEffect && rSideEffect;}
    lExp = slExp;
    paramTxt = ":?";
    scExp = cExp;
    if (lExp.type == NULL) lExp.type = cExp.type;
    else if (cExp.type == NULL) cExp.type = lExp.type;
    if (! resulExp(NumEnumBool | PtrPoss | StrunPoss, False, &computeCond,
							   operMacLvl, QMARK)) {
      TpcTypeElt curLType = lExp.type, curRType = cExp.type;
      bool rhtHghrParal = False;

      if (!CompatType(curLType, curRType, LitString) && !(rhtHghrParal = True,
		      compatType(curRType, curLType, LitString))) errWrngType(
						 NumEnumBool|PtrPoss|StrunPoss);
      else {
        curLType = arrToPtr(curLType);  /* for string literal case */
        curRType = arrToPtr(curRType);  /* for string literal case */
        if (curLType != curRType) {
          if (IsPtr(curLType) && NxtTypElt(curLType)!=NxtTypElt(curRType)) {
            /* Create a result type chain that inherits qualifiers from both
               arms of the conditional; if one arm is string litteral,
               result is pointer. */
            TpTypeElt resType, lastNewElt = NULL;

            do {
              TpTypeElt newElt;

              if (rhtHghrParal || curLType->typeSort == Void) {
                /* Choose higher parallel type; also, possible 'void *' arm
                   may be shorter. */
                TpcTypeElt w1;

                w1 = curRType; curRType = curLType; curLType = w1
							; rhtHghrParal = False;}
              newElt = allocTypeEltIC(curLType, curRType->qualif);
              if (lastNewElt == NULL) {
                resType = newElt;
                resType->noOwner = True;}  /* for freeing purposes */
              else {
                lastNewElt->stopFreeing = False;  /* because full created
							   chain to be freed. */
                lastNewElt->nextTE = newElt;}
              lastNewElt = newElt;
            } while (!NxtIsTypeId(curLType) && (curLType = advInTypeChain(
			      curLType))!=NULL && (curRType = advInTypeChain(
					curRType))!=NULL && curLType!=curRType);
            FreeExpType1(curRType);
            cExp.type = resType;}
          else if (! rhtHghrParal) {
            cExp.type = curLType;
            curLType = curRType;}}}
      if (scExp.ptrId!=NULL && (cExp.ptrId==NULL || scExp.ptrId->type!=NULL &&
						  IsStrun(scExp.ptrId->type))) {
        cExp.ptrId = scExp.ptrId;
        cExp.hist = scExp.hist;
        cExp.PointingOnObj = scExp.PointingOnObj;}
      else cExp.PointedByObj = slExp.PointedByObj;
      if (slExp.ptrId != scExp.ptrId) {
        if (slExp.ptrId!=NULL && slExp.PointingOnObj) slExp.ptrId->used = True;
        if (scExp.ptrId!=NULL && scExp.PointingOnObj) scExp.ptrId->used = True;}
      if (curLType != cExp.type) FreeExpType1(curLType);}
    cExp.topOper = (sideEffect)? SIDEFF : QMARK;
    if (expB.rEvlbl)
      if (expB.errEvl) cExp.errEvl = True;
      else
        if (expB.uVal != 0) {
          cExp.ptrId = slExp.ptrId;
          cExp.PointedByObj = slExp.PointedByObj;}  /* other relevant fields
						   already set by 'resulExp'. */
        else {
          cExp.ptrId = scExp.ptrId;
          cExp.hist = scExp.hist;
          cExp.PointingOnObj = scExp.PointingOnObj;} 
    else {
      condStmtLvl--;
      cExp.ValMltplDef = True;
      if (lExp.notPureBool) cExp.notPureBool = True;  /* does also cExp.lclAdr
							      |= lExp.lclAdr. */
      if (lExp.inhibWaNPB) cExp.inhibWaNPB = True;
      if (lExp.FctCallSeen) cExp.FctCallSeen = True;
      if (cExp.type == &boolCstTypeElt) cExp.type = &boolTypeElt;
      if (endFrstBrchRchbl == Rchbl) nxtStmtRchbl = Rchbl;}
    if (expB.litCst) {
      cExp.litCst = True;
      if (expB.LitCstOutsMac) cExp.LitCstOutsMac = True;}}
}

static void computeCond(TpcTypeElt x)
/* Keep greatest absolute value, without loosing possible sign... */
{
  if (IsTypeSort(x, SigAri | Enum)) {
    bool negSeen = (lExp.sVal < 0);
#define Abs(x) ((x >= 0)? x : -x)

    if (Abs(lExp.sVal) > Abs(cExp.sVal)) {
      if (cExp.sVal < 0) negSeen = True;
      cExp.sVal = lExp.sVal;}
    if (cExp.sVal>0 && negSeen) cExp.sVal = - cExp.sVal;}
#undef Abs
  else if (lExp.uVal > cExp.uVal) cExp.uVal = lExp.uVal;
}

static void term0(void)
{
  term1();
  while (curTok.tok == LOGOR) manageLogOper(&term1, curTok.tok, True, "||");
}

static void term1(void)
{
  term2();
  while (curTok.tok == LOGAND) manageLogOper(&term2, curTok.tok, False, "&&");
}

static void computeShortCircuit(TpcTypeElt x /*~NotUsed*/)
{
  if (cExp.uVal != 0) cExp.uVal = 1;
}

static void term2(void)
{
  term3();
  while (curTok.tok == IOR) {
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;
    TtypeSort rExpTypeSort;

    GetNxtTok();
    term3();
    lExp = slExp;
    paramTxt = "|";
    if (cExp.type != NULL) rExpTypeSort = cExp.type->typeSort;
    resulExp(WhoEnumBool, False, &computeIor, operMacLvl, IOR);
    insertBool(rExpTypeSort);}
}

static void computeIor(TpcTypeElt x /*~NotUsed*/)
{
  cExp.uVal |= lExp.uVal;
}

static void term3(void)
{
  term4();
  while (curTok.tok == XOR) {
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;
    TtypeSort rExpTypeSort;

    GetNxtTok();
    term4();
    lExp = slExp;
    paramTxt = "^";
    checkPureBool();
    if (cExp.type != NULL) rExpTypeSort = cExp.type->typeSort;
    resulExp(WhoEnumBool, False, &computeXor, operMacLvl, XOR);
    insertBool(rExpTypeSort);}
}

static void computeXor(TpcTypeElt x /*~NotUsed*/)
{
  cExp.uVal ^= lExp.uVal;
}

static void term4(void)
{
  term5();
  while (curTok.tok == APSAND) {
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;
    TtypeSort rExpTypeSort;
    bool is01 = False;

    GetNxtTok();
    term5();
    lExp = slExp;
    paramTxt = "&";
    checkPureBool();
    if (cExp.type != NULL) rExpTypeSort = cExp.type->typeSort;
    if (cExp.rEvlbl)
      if (cExp.uVal<=1) is01 = True;
      else if (cExp.sVal < 0) cExp.uVal = ~ cExp.uVal;  /* for no wrong
						    overflow error detection. */
    resulExp(WhoEnumBool, True, &computeAnd, operMacLvl, AND);
    insertBool(rExpTypeSort);
    if (is01) cExp.notPureBool = False;}
}

static void computeAnd(TpcTypeElt x /*~NotUsed*/)
{
  cExp.uVal &= lExp.uVal;
}

static TkCmp operCmp;  /* temporary */

static void term5(void)
{
  term6();
  while (curTok.tok == EQUALOP) {
    TkCmp oper;
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;

    oper = (TkCmp)curTok.val;		/* operator in class EqualOp */
    GetNxtTok();
    term6();
    lExp = slExp;		/* left operand */
    operCmp = oper;
    paramTxt = txtCmpTok[oper];
    verifCstOvfl();
    if (cExp.rEvlbl && !cExp.errEvl && cExp.uVal!=0) checkPureBool();
    if (! resulExp(NumEnumBool | PtrPoss, True, &computeCmp, operMacLvl,
								     EQUALOP)) {
      const TpcTypeElt lType = lExp.type, rType = cExp.type;

      if (rType==NULL || lType->typeSort!=Ptr || rType->typeSort!=Ptr ||
	   !CompatType(lType, rType, Cmpar)) errWrngType(NumEnumBool | PtrPoss);
      else if (cExp.rEvlbl) {  /* compute result */
        cExp.uVal = (TcalcU)((oper == EQ)? lExp.pVal == cExp.pVal :
						       lExp.pVal != cExp.pVal);}
      FreeExpType2(lType);
      cExp.ptrId = NULL;}
    else if (cExp.type!=NULL && IsTypeSort(cExp.type, Flt) && !Found(EXACTCMP)
						 ) err1(FltNotEq|Warn2|PossErr);
    relOpTrtmt();
    cExp.topOper = EQUALOP | NONASSOC;}
}

static void term6(void)
{
  term7();
  while (curTok.tok == ORDEROP) {
    TkCmp oper;
    TresulExp slExp = cExp;		/* left operand saving */
    const TpcTypeElt lType = slExp.type;
    TmacLvl operMacLvl = macLvl;

    oper = (TkCmp)curTok.val;		/* operator in class OrderOp */
    GetNxtTok();
    term7();
    lExp = slExp;		/* left operand */
    operCmp = oper;
    paramTxt = txtCmpTok[oper];
    verifCstOvfl();
    if (cExp.rEvlbl && !cExp.errEvl) {
      if (cExp.uVal == 0) {
        if (lType!=NULL && PureUnsig(lType) && !lExp.unsigDiff && oper!=GT
						   ) err1(UnsigNonNeg | Warn2);}
      else checkPureBool();}
    if (! resulExp(NumEnumBool | PtrPoss, True, &computeCmp, operMacLvl,
								     ORDEROP)) {
      TpcTypeElt rType = cExp.type;

      if (rType==NULL || lType->typeSort!=Ptr || rType->typeSort!=Ptr ||
	 !CompatType(lType, rType, NoCheck)) errWrngType(NumEnumBool | PtrPoss);
      else {
        TpcTypeElt nType;

	if ((nType = NxtTypElt(lType))!=NULL && (nType->typeSort==Void || IsFct(
	    nType)) || (nType = NxtTypElt(rType))!=NULL && (nType->typeSort==
	      Void || IsFct(nType))) errExp(NoOrderRel, lType, typeToS2(rType));
        else if (cExp.rEvlbl) {  /* compute result */
#if PtrGtLong
          switch (oper) {
            case GT: cExp.uVal = (TcalcU)(lExp.pVal > cExp.pVal); break;
            case GE: cExp.uVal = (TcalcU)(lExp.pVal >= cExp.pVal); break;
            case LE: cExp.uVal = (TcalcU)(lExp.pVal <= cExp.pVal); break;
            case LT: cExp.uVal = (TcalcU)(lExp.pVal < cExp.pVal); break;
            /*~NoDefault*/}
#else
          lExp.uVal = (TcalcU)lExp.pVal;
          cExp.uVal = (TcalcU)cExp.pVal;
          computeCmp(&natTyp[_ULLONG]);
#endif
        }}
      FreeExpType1(lType);
      cExp.ptrId = NULL;}
    else if (cExp.unsigDiff) err1(UnsigDiffNotNeg|Warn2|PossErr);
    relOpTrtmt();
    cExp.topOper = ORDEROP | NONASSOC;}
}

#pragma noinline (sConvToType, uConvToType)
static TcalcS sConvToType(const TresulExp *pExp, TpcTypeElt x)
{
  return (x->typeSort < Long)
          ? (TcalcS)(int)pExp->sVal
#ifdef LONGLONG
          : (x->typeSort < LLong)
            ? (TcalcS)(long)pExp->sVal
#endif
            : pExp->sVal;
}

static TcalcU uConvToType(const TresulExp *pExp, TpcTypeElt x)
{
  return (x->typeSort < Long)
          ? (TcalcU)(uint)pExp->uVal
#ifdef LONGLONG
          : (x->typeSort < LLong)
            ? (TcalcU)(ulong)pExp->uVal
#endif
            : pExp->uVal;
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
  cExp.uVal = (cmpCod[operCmp] & cmpMsk)? 1 : 0;
  /* *doIt* float types */
#undef LtBit
#undef EqBit
#undef GtBit
}
/*~Undef operCmp */

static TkShi operShi;  /* temporary */

static void term7(void)
{
  term8();
  while (curTok.tok == SHIFTOP) {
    TkShi oper;
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;

    oper = (TkShi)curTok.val;		/* operator in class ShiftOp */
    GetNxtTok();
    term8();
    lExp = slExp;
    paramTxt = txtShiTok[oper];
    operShi = oper;
    {
      TpcTypeElt lType;

      /* Start by eliminating erroneous cases (else, funny messages from
         resulExp() because rType==lType...). */
      if ((lType = lExp.type)!=NULL && !IsTypeSort(lType, WhoEnum)) {
        errWrngType(WhoEnum);
        FreeExpType2(lType);
        lExp.type = lType = NULL;}
      else if (cExp.type!=NULL && !IsWhoEnumNotChar(cExp.type)) errExp(
	    IlgRhtType | ((IsTypeSort(cExp.type, WhoEnum))? Warn2 | PossErr :
							 Err), cExp.type, NULL);
      else {
        if (cExp.rEvlbl && !cExp.errEvl && (cExp.sVal<0 || lType!=NULL && cExp.
	     sVal>=(TcalcS)lType->size * CHAR_BITC)) { /* *doIt*: portability */
          errWSSSS(UndefResul|Warn2|PossErr, paramTxt, longToS(cExp.sVal),
							 typeToS1(lType), NULL);
          cExp.errEvl = True;
          cExp.sVal = 0;}
        FreeExpType(cExp);}
      cExp.type = lType;}
    cExp.cstImpsd = lExp.cstImpsd;
    cExp.unsigDiff = False;
    resulExp(WhoEnum, False, &computeShi, operMacLvl, SHIFTOP);
    cExp.topOper = SHIFTOP | NONASSOC;}
}

static void computeShi(TpcTypeElt x)
{
  TtypeSort tsl = x->typeSort;

  if (operShi == LSh) {
    register TcalcS w;

    w = lExp.sVal << cExp.sVal;
    if ((lExp.sVal>=0 && w<0 || lExp.sVal<0 && w>=0) && tsl & SigWhoEnum ||
				    (TcalcU)w>>cExp.sVal!=lExp.uVal) errOvfl(x);
    cExp.sVal = w;}
  else if (cExp.uVal == 0) cExp.sVal = lExp.sVal;
  else {
    if (tsl & SigWhoEnum) cExp.sVal = lExp.sVal >> (cExp.uVal - 1);
    else cExp.uVal = lExp.uVal >> (cExp.uVal - 1);
    if (cExp.sVal==0 && lExp.sVal!=0) errUdfl();
    if (tsl & SigWhoEnum) cExp.sVal >>= 1;
    else cExp.uVal >>= 1;}
}
/*~Undef operShi */

static void term8(void)
{
  term9();
  while (curTok.tok == ADDOP) {
    TkAdd oper;
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;
    bool foundTC = False;

    oper = (TkAdd)curTok.val;		/* operator in class AddOp */
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
      if (! resulExp(NumEnum | PtrPoss, False, &computeSub, operMacLvl, ADDOP))
        specialCaseOp(oper);
      else if (! foundTC) {  /* see whether "distance" obtained */
        TpcTypeElt eType = cExp.type;

        if (eType!=NULL && eType->typeSort==Enum) {
          FreeExpType2(eType);
          cExp.type = &natTyp[_INT];}}
      if (cExp.type!=NULL && PureUnsig(cExp.type) && !cExp.rEvlbl /* because
		    either no problem or already warned on overflow. */) cExp.
							       unsigDiff = True;
      cExp.topOper = SubOP;}}
}

static void computeAdd(TpcTypeElt x)
{
  if (x->typeSort & UnsigWho) {
    cExp.uVal += lExp.uVal;
    if (cExp.uVal < lExp.uVal) errOvfl(x);}
  else {  /* *doIt* float types */
    if (! computeSigAdd(lExp.sVal, cExp.sVal, &cExp.sVal)) errOvfl(x);}
}

static void computeSub(TpcTypeElt x)
{
  if (x->typeSort & UnsigWho) {
    if (cExp.uVal > lExp.uVal) errOvfl(x);
    cExp.uVal = lExp.uVal - cExp.uVal;}
  else {  /* *doIt* float types */
    TcalcS prevValCExp = cExp.sVal;

    if (!computeSigAdd(lExp.sVal, -cExp.sVal, &cExp.sVal)
#if LONG_MINC + LONG_MAXC != 0
			  || prevValCExp==LONGLONG_MINC && lExp.sVal>=0
#endif
								  ) errOvfl(x);}
}

static void term9(void)
{
  term10();
  while (curTok.tok==STAR || curTok.tok==MULOP) {
    TkMul oper;
    TresulExp slExp = cExp;		/* left operand saving */
    TmacLvl operMacLvl = macLvl;

    oper = (curTok.tok == STAR)? Mul : (TkMul)curTok.val;
    GetNxtTok();
    term10();
    lExp = slExp;		/* left operand */
    paramTxt = txtMulTok[oper];
    if (pHeadTCBLists!=NULL && manageTypCmbn(MulTC + (oper - Mul))) {}
    else if (cExp.type != NULL)  /* coefficient ? */
      if (! cExp.type->paralTyp) ((ModifType)(cExp.type = copyTypeEltNoOwn(
		cExp.type)))->generic = True;  /* because multiplication =
						       sequence of additions. */
      else if (oper==Mul && lExp.type!=NULL && !lExp.type->paralTyp) ((
	   ModifType)(lExp.type = copyTypeEltNoOwn(lExp.type)))->generic = True;
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
    if (lExp.uVal<=SqrtTwoPow32-1 && cExp.uVal<=SqrtTwoPow32) cExp.uVal *=
								      lExp.uVal;
#undef SqrtTwoPow32
    else if (cExp.uVal != 0) {
      register TcalcU w;

      w = lExp.uVal * cExp.uVal;
      if (w/cExp.uVal != lExp.uVal) errOvfl(x);
      cExp.uVal = w;}}
  else {  /* *doIt* float types */
#define SqrtTwoPow31	46340L  /* 46340 = (TcalcS)sqrt(2**31) */
    if ((TcalcU)(lExp.sVal+SqrtTwoPow31)<=SqrtTwoPow31+SqrtTwoPow31 && (TcalcU)
      (cExp.sVal+SqrtTwoPow31)<=SqrtTwoPow31+SqrtTwoPow31) cExp.sVal *=
								      lExp.sVal;
#undef SqrtTwoPow31
    else if (cExp.sVal != 0) {
      register TcalcS w;

      w = lExp.sVal * cExp.sVal;
      if (w/cExp.sVal != lExp.sVal) errOvfl(x);
      cExp.sVal = w;}}
}

static void computeDiv(TpcTypeElt x)
{
  if (cExp.sVal == 0) errOvfl(lExp.type);  /* divide by 0 */
  else {
    if (x->typeSort & UnsigWho) cExp.uVal = lExp.uVal / cExp.uVal;
    else cExp.sVal = lExp.sVal / cExp.sVal;  /* *doIt* float types */
    if (cExp.uVal==0 && lExp.uVal!=0) errUdfl();}
}

static void computeMod(TpcTypeElt x)
{
  if (cExp.sVal == 0) errOvfl(lExp.type);  /* divide by 0 */
  if (x->typeSort & UnsigWho) cExp.uVal = lExp.uVal % cExp.uVal;
  else cExp.sVal = lExp.sVal % cExp.sVal;
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
      static const Tstring txtUnAddTok[/*~ IndexType TkAdd */] = {"u+", "u-"};
      /*~ zif NbEltGen(txtUnAddTok, (TkAdd)0) != __extent(TkAdd)
					      "Array 'txtUnAddTok': bad size" */

      oper = (TkAdd)curTok.val;
      if (NxtTok() == CSTNU) {	/* special case for numeric constants, to be
						    able to mark them signed. */
        curTok.val = (oper == Sub)? SignSeen | NegSeen : SignSeen;
        goto cstL;}
      term10();
      resulUnOp(Num, txtUnAddTok[oper]);
      if (cExp.rEvlbl) {
        TpcTypeElt rType;

        if ((rType = cExp.type) != NULL) {
          TtypeSort tsr = rType->typeSort;

          if (tsr & SigWhoEnum) {
            if (oper == Sub) {
              register TcalcS w;

              w = -cExp.sVal;
              if (((w<0 && cExp.sVal<0) || (w>0 && cExp.sVal>0))) errOvfl(rType);
              cExp.sVal = w;}}
          else if (tsr & UnsigWho) {
            if (oper == Sub) {
              topOper = SubOP;
#if NO_GREATER_INT
              cExp.uVal = -cExp.uVal;
#else
              if (tsr < Long) cExp.uVal = (TcalcU)(-(uint)cExp.uVal);
						 /* because -(1U) = UINT_MAXC */
              else if (tsr < LLong) cExp.uVal = (TcalcU)(-(ulong)cExp.uVal);
					       /* because -(1UL) = ULONG_MAXC */
              else cExp.uVal = -cExp.uVal;
#endif
            }}}
        else cExp.sVal = - cExp.sVal;}
      goto defTopOperL;}
  case APSAND: {
      TpcTypeElt ptrAddendType = (NxtTok() == INDEXTYPE)? getINDEXTYPEtype() :
									   NULL;
      bool savAO = apsndOpnd;

      apsndOpnd = True;
      term10();
      if (! cExp.lValFl) {paramTxt = "u&"; errExp(NotLVal, NULL, NULL); cExp.
								  ptrId = NULL;}
      else {
        TpTypeElt newElt = allocTypeEltID(Ptr, NoQualif);

        if (cExp.pseudoAttrib == Reg) {err0(RegAttribForAmp); cExp.
							   pseudoAttrib = Auto;}
        newElt->noOwner = True;
        if (cExp.type != NULL) newElt->stopFreeing = ! cExp.type->noOwner;
        newElt->size = PtrSiz;
        if (ptrAddendType != NULL) newElt->indexType = ptrAddendType;
        else {
          Ttok topOper = NakedTok(cExp.topOper);

          newElt->indexType = ((topOper==LSBR || topOper==STAR) && cExp.type!=
	    NULL)? lExp.type : (moreIndexTypeChk)? &defaultIndexTypeElt : NULL;}
        newElt->nextTE = cExp.type;
        newElt->generic = True;  /* to allow '&buf[...]' to be compatible with
				   any reachable descendant of 'Ptr/TbufElt'. */
        cExp.type = newElt;
        cExp.rEvlbl = cExp.lEvlbl;
        cExp.errEvl = False;
        if (! cExp.PointedByObj) {
          TsemanElt *ptrId = cExp.ptrId;

          if (ptrId!=NULL && ptrId->initlz /* because of UslObj */) ptrId->
		used = cExp.oldUsed;  /* taking address of object is not
							      (yet) using it. */
          cExp.PointingOnObj = True;}
        else PopHist(cExp.hist);
        cExp.lValFl = False;
        cExp.lclAdr = cExp.pseudoAttrib == Auto;}
      apsndOpnd = savAO;
      goto defTopOperL;}
  case CASTTO: {
      TpcTypeElt castType = curTok.dpType, eType;

      GetNxtTok();
      term10();
      if (castType!=NULL && (eType = cExp.type)!=NULL && !compatType(
			  represType(castType), represType(eType), VerifCast)) {
        errWSTT(IlgCastTo|Warn2|PossErr, NULL, castType, eType);
        FreeExpType2(castType);}
      else {
        if (preLastCastType != NULL) freePrev();
        if (compatType(castType, eType, VerifCastF) && !cExp.unsigDiff && !cExp.
		 cstImpsd) errWSTT(UslCastTo|Warn1|Rdbl, NULL, castType, eType);
        finalizeCast(castType);}
      goto defTopOperL;}
  case CSTCH: case CSTCH1:	/* character constant */
    cExp.Einfo = 0;
    cExp.errEvl = curTok.tok == CSTCH1;
    cExp.uVal = (cExp.errEvl)? 0 : (TcalcU)curTok.val;
    cExp.type = &charCstTypeElt;
    goto cstL1;
  case CSTNU:			/* numeric constant */
cstL:
    manageNumCst();
    if (chkNumCst && !cExp.errEvl && (TcalcU)(cExp.sVal + 1)>1+1 /* *doIt* Flt */
	 && !sysAdjHdrFile && !aloneInNoParMac(True) && !(operMacLvl!=0 &&
	  NakedTok(oldTopOper)==LPAR && nxtCharOrMacTok()==-(TcharTok)RPAR)) {
      cExp.litCst = True;
      if (operMacLvl == 0) cExp.LitCstOutsMac = True;}
cstL1:
    cExp.ptrId = NULL;
cstL2:
    SetInfoForCst
    goto exitIdL;
  case CSTST: {			/* string constant */
      TpTypeElt newElt;

      curTok.error = False;
      cExp.pVal = (const char *)ptrFreeIdSpace();
      /* Concatenate adjacent string litterals */
      do {
        analStrCst(&storeStrCh);
        lastPrimMacParCtr = (insideMultUsedMacPar)? macParCtr : 0;
      } while (NxtTok() == CSTST);
      storeStrCh('\0');	/* terminating byte */
      cExp.Einfo = 0;  /* reset all flags */
      cExp.errEvl = curTok.error;  /* do not reset cExp.pVal, since one may
			      still want to use the erroneous string literal. */
      cExp.lValFl = True;
      cExp.lEvlbl = True;
      cExp.rEvlbl = True;  /* because "..." => &"..."[0] implicitely */;
      cExp.pseudoAttrib = Stati;
      cExp.ptrId = NULL;
      cExp.type = newElt = copyTypeEltNoOwn(&strCstTypeElt);
      newElt->lim = newElt->size = initGetStrLit(NULL);
      goto tstQualif1L;}
  case EMARK:
    GetNxtTok();
    term10();
    resulUnOp(mskBool, "!");
    if (cExp.rEvlbl) cExp.uVal = (TcalcU)(! (bool)cExp.uVal);
    relOpTrtmt();
    goto defTopOperL;
  case IDENT: {
      TsemanElt *ptrId;

      cExp.type = NULL;
      cExp.Einfo = 0;  /* reset pseudoAttrib and all flags (rEvlbl etc.) */
      if ((ifDirExp || zifExp) && checkSpeFct()) {  /* special preprocessor/
								zif function. */
        topOper |= PRIMARY;
        cExp.ptrId = NULL;
        SetInfoForCst
        goto defTopOperL;}
      if (ifDirExp) {  /* 'any identifiers remaining after macro expansion
							 are replaced by 0L'. */
        if (evaluateFl) errWN(UndefId|Warn3|PossErr, curTok.name);
        cExp.type = &longCstTypeElt;
        cExp.sVal = 0;
        goto cstL1 /*~BackBranch*/;}
      if ((ptrId = curTok.ptrSem) == NULL) {  /* unknown identifier */
        TcharTok w = nxtCharOrMacTok();
        DefSem1(artifDef, Obj, True /*defnd*/, True /*used*/, True /*initlz*/);

        errWN((w==(TcharTok)'(' || w==-(TcharTok)LPAR)
		 ? UndefFctId | CWarn3
                 : (isupper((char)*(curTok.name + LgtHdrId)))  /* symbol? =>
								    constant. */
		   ? (cExp.rEvlbl = True, cExp.errEvl = True, UndefId)
		   : UndefId, curTok.name);
        ptrId = defineId(artifDef);
        if (ptrId == NULL) goto exit1IdL;}
      if (ptrId->kind >= Type) {
        if (ptrId->kind == EnumCst) {
          if (ifDirExp) err0(IlgInIfExp);
          if (! ptrId->defnd) {
            errWN(NotUsdEnumCst|Warn1|Rdbl, curTok.name);
            if (! allErrFl) ptrId->defnd = True;}
          if (ptrId->type->tagId->listAlwdFiles != NULL) checkPrivVisible(
								   ptrId->type);
          cExp.ptrId = ptrId;
          cExp.type = ptrId->type;
          cExp.sVal = (TcalcS)ptrId->enumVal;
          ptrId->used = True;
          goto cstL2 /*~BackBranch*/;}
        errWN(IlgId, curTok.name);
        goto exit1IdL;}
      if ((cExp.type = ptrId->type)==NULL && isupper((char)*(curTok.name +
			   LgtHdrId))) {cExp.rEvlbl = True; cExp.errEvl = True;}
      if (ptrId->attribb == Extrn) {
        if (ptrId->declInInHdr) checkIncldFiles(ptrId);}
      else if (!ptrId->defnd && ptrId->type!=NULL && !IsFct(ptrId->type)) {
        errWN((ptrId->kind == Param)? NotUsdPar|Warn1|Rdbl : NotUsdObj|Warn1|
							     Rdbl, curTok.name);
        if (! allErrFl) ptrId->defnd = True;}
      cExp.pseudoAttrib = ptrId->attribb;
      cExp.oldUsed = ptrId->used;   /* save previous state of 'used' flag */
      if (! zifExp) ptrId->used = True;
      cExp.lclAdr = ptrId->lclAd;  /* in case type(ident) == Ptr  (or Bool: does
				 also "cExp.notPureBool = ptrId->notPureBoo;" */
exit1IdL:
      cExp.lValFl = True;
      cExp.lEvlbl = True;  /* because &(id) yields constant address */
      cExp.pVal = ArbitAdr;
      cExp.ptrId = ptrId;
      cExp.hist = H_EMPTY;}
exitIdL:
    lastPrimMacParCtr = (insideMultUsedMacPar)? macParCtr : 0;
    GetNxtTok();
tstQualif1L:
    cExp.topOper = topOper | PRIMARY;
    goto tstQualifL;
  case INCOP: {
      TkInc oper= (TkInc)curTok.val;
      TmacParCtr operMacParCtr = macParCtr;

      GetNxtTok();
      term10();
      resulIncOp(oper, operMacParCtr);
      goto positMacLvlL;}
  case LPAR: {  /* cast/sub-expression */
      TcharTok w;

      GetNxtTok();
      if (IsDecl1(BQualif)
       || curTok.tok==IDENT
        && (curTok.ptrSem!=NULL && curTok.ptrSem->kind==Type
        /* in an attempt to follow programmer idea, consider undefined paren-
           thetized uppercase identifier as would-be type identifier. */
         || curTok.ptrSem==NULL && ((w = nxtCharOrMacTok())==(TcharTok)'*' ||
		 w==(TcharTok)')' || w==-(TcharTok)STAR || w==-(TcharTok)RPAR))
        && (curTok.ptrSem!=NULL && curTok.ptrSem->namedType!=NULL
         || !ifDirExp /* cast meaningless in '#if' */ && isupper((char)*(
					curTok.name + LgtHdrId)))) {  /* cast */
        TpcTypeElt castType, eType;
        bool macParMet, oddCSeen = False, portQMSeen, ignorable = False;

        if (preLastCastType != NULL) freePrev();
        castType = declType();
        if (! (portQMSeen = Found(PORTQM)) && curTok.tok==ODDCAST) {
          ignorable = ignorableDP(macParCtr + 1);
          oddCSeen = True;}
        if (ifDirExp) err0(IlgInIfExp);
        if (! Found(RPAR)) err0(RParExptd);
        {
          TmacParCtr begExpMacParCtr = macParCtr;

          term10();
          macParMet = (macParCtr != begExpMacParCtr);}
        if (! macParMet) ignorable = False;
        eType = cExp.type;
        paramTxt = "cast";
        if (castType != NULL) {
          TtypeSort tsc = castType->typeSort;

          if ((uint)tsc >= (uint)Array) {
            errWT((!compatType(castType, eType, VerifCastF) && eType!=NULL &&
		compatType(represType(castType), represType(eType), VerifCast)
				   )? IlgCastTarget1 : IlgCastTarget, castType);
            FreeExpType2(castType);
            castType = NULL;}
          else {
            if (tsc == Bool) cExp.notPureBool = !(oddCSeen || cExp.rEvlbl
					      && (cExp.errEvl || cExp.uVal<=1));
            if (eType != NULL) {
              TtypeSort tse = eType->typeSort;
              Terr errNo;

              if (IsFctSort(tse) || (tse==Void || IsStrunSort(tse)) && tsc!=
					      Void) errWT(IlgCastSource, eType);
              else if (tsc != Void) {
                if (tse==Array && eType->generic) tse = Ptr;  /* for cast
					     purpose, string constant => Ptr. */
                if (CompatType(castType, eType, VerifCastF)) {
                  TpcTypeElt w;

                  if (   !macParMet
		      && (castType==eType || castType->sysTpdf==eType->sysTpdf)
		      && !(tsc==Ptr && eType->generic && ((w = NxtTypElt(eType)
			)==NULL || w->typeSort==Void) && ((w = NxtTypElt(
			  castType))==NULL || w->typeSort!=Void))  /* (void *)
				generally to be cast to other pointer, so do not
				   complain if people try to do things right. */
		      && !cExp.cstImpsd) {errNo = UslCast|CWarn1|Rdbl; goto
								       stdErrL;}
                  goto noErrL;}
                else if (tsc < DelType) {  /* cast to NumEnumBool */
                  switch(tse) {
                  case Ptr:  /* Ptr => NumEnumBool */
                    if (allocType != NULL) freeAlloc();  /* inhibitates
							      'Xalloc' check. */
                    if (cExp.rEvlbl && cExp.pseudoAttrib==NoAttrib) {  /* ad-
								 dress known. */
                      if (!cExp.errEvl && computeLstType((TcalcS)(size_t)cExp.
			  pVal) > ((tsc == Enum)? Int : tsc)) errOvfl(castType);
                      break;}  /* *doIt* break only if no overflow on any
								     machine. */
#if ! PtrGtLong
                    if (castType->sysTpdf && memcmp(castType->typeId->nameb,
			SizeTName, LitLen(SizeTName))==0 || castType->typeSort==
							    ULLong) goto noErrL;
#endif
                    errNo = PtrToNum|CWarn2|PossErr;
                    goto stdErrL;
                  case Array:  /* Array => NumEnumBool */
                    errNo = NotArrToNum|Warn1|Rdbl;
                    goto stdErrL;;
                  default:  /* NumEnumBool => NumEnumBool */
                    if (castType->litCsta!=eType->litCsta) CheckNumCstNamed(
								cExp, castType);
                    if (cExp.rEvlbl) checkInfoLoss(castType);}}
                else {  /* cast to Ptr */
                  bool sameTypButQual;

                  errQalTyp1 = NULL;
                  sameTypButQual = compatType(castType, eType, CCheck1);
                  if (errQalTyp1 != NULL) {
                    if (! oddCSeen) errWSTT(HzrdConv1|CWarn2|PossErr, NULL,
							       eType, castType);
                    else preLastCastType = eType;
                    ignorable = True;}
                  if (! sameTypButQual) {
                    TpcTypeElt castTypw = castType;

                    for(;;) {
                      TpcTypeElt nxtCType, nxtEType;

                      if ((nxtCType = NxtTypElt(castTypw))==NULL || (nxtEType
			  = NxtTypElt(eType))==NULL && IsPtrArrSort(tse)) goto
									noErr1L;
                      switch(tse) {
                      case Array:  /* Array => Ptr */
                        if (castTypw==castType /* first turn around */) errWSTT(
				    ArrToPtr|Warn1|Rdbl, NULL, eType, castTypw);
                        /*~NoBreak*/
                      case Ptr:  /* Ptr => Ptr */
                        if (IsFct(nxtCType) && IsFct(nxtEType)) goto hzdCastL;
                        if (castTypw == castType) { /* first turn around */
                          if (nxtEType->typeSort == Void) break;  /* because
					'void *' compatible with any pointer. */
                          if (nxtCType->typeSort == Void)
                            if (IsFct(nxtEType)) goto nonPortCastL;
                            else break;
                          if (nxtCType->typeSort == UByte) break;  /* raw
							     bytes container. */
                          if (IsFct(nxtCType)) {errWSTT(IncptblTypes, paramTxt,
						      castType, eType); break;}}
                        if ((uint)nxtCType->typeSort >= (uint)Fct) /* Fct, VFct,
					    Struc, Union */ goto nonPortCastQML;
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
                        if (nxtCType->algn != nxtEType->algn) {  /* nothing
					 to say if same alignment constraint. */
                            /*if (nxtCType->algn > nxtEType->algn) goto
				  nonPortCastQML;   because alignment cons-
				       traint may be respected by programmer. */
                          /* Cast to pointer on smaller integer sort a priori
                             never portable, because of big/little endians, but
                             see header management of dynamic arrays (dynarray
                             .h). */
                          goto nonPortCastQML;}
                        break;
                      default:  /* NumEnumBool => Ptr */
                        cExp.pseudoAttrib = NoAttrib;
                        if (cExp.sVal==0 && eType==&intCstTypeElt &&
		         NakedTok(cExp.topOper)==CSTNU) {  /* because 0 can
			  be cast to any pointer (idiom '&(struct *)0->...'). */
                          ((ModifType)castType)->generic = True;  /* NULL
								    generic ! */
                          break;}
                        if (eType->sysTpdf && memcmp(eType->typeId->nameb,
				      SizeTName, LitLen(SizeTName)) == 0) break;
                        if (cExp.rEvlbl && !cExp.errEvl && ((computeCstType(
			       cExp.sVal, UnsigWho)<=UInt)? IntSiz : LongSiz)>
						      PtrSiz) errOvfl(castType);
                        goto nonPortCastQML;}
                      break;}}}}  /* exit for(;;) loop */
              if (oddCSeen && !ignorable) errUslDP(ODDCAST);  /* no warning in
					macro if macro parameter met, because
					its type is a priori unknown. */
              goto noErrL;
hzdCastL:
              errNo = HzrdConv|CWarn2|PossErr;
stdErrL:
              if (!oddCSeen) errWSTT(errNo, NULL, cExp.type, castType);
noErrL:
              if (portQMSeen) errUslDP(PORTQM);
              goto noErr1L;
nonPortCastQML:
              /* Does not know whether really not portable */
              if (!oddCSeen && !portQMSeen && chkPortbl) errWSTT(
			     NonPortCastQM | CWarn1, NULL, cExp.type, castType);
              goto noErr1L;
nonPortCastL:
              if (!portQMSeen && chkPortbl) errWSTT(PortCastQM | CWarn1, NULL,
							   cExp.type, castType);
noErr1L: ;}}
          if (!macParMet || NakedTok(cExp.topOper)!=VFCTC) cExp.topOper = CAST; 
		  /* inside macro, pseudo-void function stays so through cast */
          if (! (tsc==Ptr && cExp.type!=NULL && IsPtrArr(cExp.type))) cExp.
								lValFl = False;}
        finalizeCast(castType);
        goto positMacLvlL;}
      /* Expression */
      commaExpr();
      if (! Found(RPAR)) err0(RParExptd);
      cExp.topOper |= PARENSEEN;  /* so that "(a=b)" => side-effect. */
      if (Found(NONCONST)) cExp.rEvlbl = False;}
tstQualifL:
    cExp.macLvl = operMacLvl;
    if (InsideInterval(curTok.tok, LPAR, DOT)) primQualif();
    break;
  case SIZEOF: {
      bool parenFl = False, savEvalFl = evaluateFl;

      if (ifDirExp) err0(IlgInIfExp);
      GetNxtTok();
      lastPrimMacParCtr = (insideMultUsedMacPar)? macParCtr : 0;
      evaluateFl = False;
      loopLvl++;  /* hack to prevent manageUsedForIncOp() from doing wrong */
      if (curTok.tok == LPAR) {		/* maybe type */
        parenFl = True;
        GetNxtTok();
        if (isType()) {cExp.type = declType(); cExp.topOper = WHITESPACE |
						    PRIMARY; cExp.ptrId = NULL;}
        else {commaExpr(); valueNotUsed();}
        if (curTok.tok != RPAR) err0(RParExptd);}
      else {term10(); valueNotUsed();}
      loopLvl--;
      evaluateFl = savEvalFl;
      {
        TREInfo savCEinfo;

        savCEinfo._eInfo = cExp.Einfo;
        cExp.Einfo = 0;  /* reset all flags */
        if (savCEinfo.s21._litCst) {
          cExp.litCst = True;
          if (savCEinfo.s21._litCstOutsMac) cExp.LitCstOutsMac = True;}}
      if ((cExp.uVal = (TcalcU)sizeOfTypeD(
		cExp.type,
		(cExp.ptrId!=NULL && cExp.ptrId->kind==Param && cExp.ptrId->
		       type!=NULL && IsArrFct(cExp.ptrId->type) && cExp.hist==
	 			  H_EMPTY)? UndefSize|Warn2|PossErr : UndefSize,
		(Tname)"\6\0sizeof"))
				   == 0) cExp.errEvl = True;  /* illegal size */
      cExp.cstImpsd = True;
      cExp.rEvlbl = True;
      if (! evaluateFl) cExp.errEvl = True;  /* to prevent computation */
      if (sizeofOpndType == XAllocSeen) sizeofOpndType = cExp.type;
      else {
        FreeExpType(cExp);
        if (sizeofOpndType != NULL) {FreeExpType2(sizeofOpndType)
						      ; sizeofOpndType = NULL;}}
      if (! sizeofSeen) {
        TsemanElt *ptrId;

        if ((ptrId = searchSymTabHC(SizeTName, SizeTHCode))==NULL ||
							    ptrId->type==NULL) {
          if (! stddefNotInFl) {
            err0(StddefNotIncl|Warn2|Rdbl|PossErr);
            stddefNotInFl = True;}}
        else {
          sizeofSeen = True;
          sizeofTypeElt = *ptrId->namedType;
          sizeofTypeElt.generic = True;}}
      cExp.type = &sizeofTypeElt;
      if (InsideInterval(NakedTok(cExp.topOper), FCTC, INCOP)) err0(
						  SizeofDontEval|Warn3|PossErr);
      else if (InsideInterval(NakedTok(cExp.topOper), CSTCH, CSTCH1)) {err0(
			      SizeofCharCst|Warn3|PossErr); cExp.uVal = IntSiz;}
      if (parenFl) GetNxtTok();  /* for good error position */
      cExp.ptrId = NULL;
      goto tstQualif1L /*~BackBranch*/;}
  case STAR: {
      GetNxtTok();
      term10();
      paramTxt = "u*";
      indir();
      goto defTopOperL;}
  case TILDE:
    GetNxtTok();
    term10();
    resulUnOp(WhoEnum, "~");
    if (cExp.rEvlbl) {
      if (cExp.type == NULL) cExp.uVal = ~cExp.uVal;
      else if (cExp.type->typeSort & SigWhoEnum) cExp.sVal = ~cExp.sVal;
      else {
        cExp.uVal = ~cExp.uVal;
#if ! NO_GREATER_INT
        if (cExp.type->typeSort < Long) cExp.uVal &= UINT_MAXC;
        else if (cExp.type->typeSort < LLong) cExp.uVal &= ULONG_MAXC;
#endif
      }}
    goto defTopOperL;
  case VOIDTOTHER: {
      TpcTypeElt w, eType;

      GetNxtTok();
      term10();
      if ((eType = cExp.type) != NULL) {
        if (IsPtr(eType)) {
          if ((w = NxtTypElt(eType)) == NULL) goto defTopOperL;
          if (w->typeSort == Void) {
            cExp.type = copyTypeEltNoOwn(eType);
            ((ModifType)cExp.type)->generic = True;  /* keep qualifiers */
            goto defTopOperL;}}
        errIlgDP(VOIDTOTHER);}}
defTopOperL:
    cExp.topOper = topOper;
    goto positMacLvlL;
  default:
    cExp.type = NULL;  /* because of FreeExpType in errExp() */
    errExp((oldTopOper == SCOL)? StmtExptd : MsngExp, NULL, NULL);
    cExp.Einfo = 0;  /* reset all flags */
    cExp.lValFl = True;
    cExp.lEvlbl = True;
    cExp.pVal = ArbitAdr;
    lastPrimMacParCtr = 0;
    cExp.ptrId = NULL;
positMacLvlL:
    cExp.macLvl = operMacLvl;}
  while (curTok.tok == INCOP) {
    lastPrimMacParCtr = (insideMultUsedMacPar)? operMacParCtr : 0;
    resulIncOp((TkInc)curTok.val, macParCtr);
    cExp.macLvl = macLvl;
    GetNxtTok();}
}

static void finalizeCast(TpcTypeElt castType /*~MayModify*/)
{
  if (castType==NULL || castType->typeSort!=Void) checkInit();
  else cExp.ptrId = NULL;  /* for no checkInit */
  if (castType!=NULL && cExp.rEvlbl && castType->rootTyp)  /* mark constants
						      of root type 'generic'. */
    ((ModifType)(castType = copyTypeEltNoOwn(castType)))->generic = True;
  if (preLastCastType != cExp.type) FreeExpType(cExp);
  cExp.type = castType;
  cExp.cstImpsd = False;  /* to get 'type mismatch' error on
							 "schar = (short)3;". */
  cExp.unsigDiff = False;
}

static void manageNumCst(void)
{
  analNumCst(NULL);
  cExp.Einfo = 0;  /* reset all flags */
  cExp.errEvl = curTok.error;
  if (curTok.val & FltSeen) {  /* floating constant */
    cExp.sVal = curTok.numVal;  /* until better mngt... *doIt* */
    if (curTok.val & NegSeen) cExp.sVal = -cExp.sVal;
    cExp.type = (curTok.val & FSeen)? &floatCstTypeElt :
	       (curTok.val & LDSeen)? &longDblCstTypeElt :
	       (cExp.cstImpsd = True, &doubleCstTypeElt);}
  else {
    cExp.sVal = curTok.numVal;
    if (curTok.val & NegSeen) cExp.sVal = -cExp.sVal;
    if (curTok.numVal>=0 && !(curTok.val & (LSeen | USeen))
#if ! NO_GREATER_INT
     && ((TcalcU)(cExp.sVal - INT_MINC) <= (TcalcU)((TcalcS)INT_MAXC -INT_MINC))
#endif
							   ) {
      cExp.type = &intCstTypeElt;  /* 'simple' integer constant */
      cExp.cstImpsd = True;}
    else {
      TpTypeElt newElt;
      TtypeSort cstSort;

      newElt = allocTypeEltID((TtypeSort)(0-0), NoQualif);
      newElt->noOwner = True; newElt->generic = True;
      if ((cstSort = computeLstType(cExp.sVal)) < Int) cstSort = Int;
      if (curTok.val & SignSeen)  /* possible overflow already detected by
								analNumCst(). */
#ifdef LONGLONG
        if (cstSort < Long) cstSort |= Long | LLong; else cstSort |= LLong;
#else
        cstSort |= Long;
#endif
      else {
        if (curTok.numVal < 0) cstSort =
#if NO_GREATER_INT  /* for numVal computed in Long */
					 UInt | ULong;
#else
					 ULLong;
#endif
        else if (cstSort < Long) cstSort |= UInt | Long | ULong | LLong |ULLong;
        else if (cstSort < LLong) cstSort |= ULong | LLong | ULLong;
        else cstSort |= ULLong;}
      if (curTok.val & LSeen) cstSort &= Long | ULong;
#ifdef LONGLONG
      else if (curTok.val & LLSeen) cstSort &= LLong | ULLong;
#endif
      else cExp.cstImpsd = True;
      if (curTok.val & USeen) cstSort &= UInt | ULong | ULLong;
      else if (! (curTok.val & OctHex)) cstSort &= ~ UInt;  /* because the
							    standard says so. */
      if (cstSort == Void) {err0(WrngNb); cExp.errEvl = True; cstSort = Int;}
      /* Keep only least type sort */
      {
        TtypeSort w;
        int i = 0;
        static const size_t tabSize[] = {IntSiz, IntSiz, LongSiz, LongSiz
#ifdef LONGLONG
						   , LongLongSiz, LongLongSiz
#endif
									      };

        for (w = Int; (w & cstSort) == Void; w <<= 1) {i++;}
        newElt->typeSort = w;
        newElt->size = tabSize[i];}
      cExp.type = newElt;}}
}

static void primQualif(void)
{
  CheckNumCstNamed(cExp, cExp.type);
  for (;;) {
    TpcTypeElt initialType = cExp.type;

    switch (curTok.tok) {
    case ARROW:
      paramTxt = "->";
      {
        TpcTypeElt w;

        if (initialType!=NULL && (w = NxtTypElt(initialType))!=NULL &&
		!IsStrun(w)) errExp(IlgLftType, initialType, NULL);}  /* has to
			be done that way (cannot use test of DOT case), because
			indir() destroys head of type chain (field 'nextTE'). */
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
        TpcTypeElt eType;

        cExp.topOper = curTok.tok | PRIMARY;
        if (preLastCastType != NULL) freePrev();  /* type of members are
					  completely unrelated to strun type. */
        if ((eType = cExp.type) != NULL) {
          if (eType->tagId->listAlwdFiles != NULL) checkPrivVisible(eType);
          member = BaseStrunType(eType)->memberList;
          if (member == NULL) {
            (void)sizeOfTypeI(eType, False, IncplStrunType, paramTxt);/* type */
            errExp(NoErrMsg, NULL, NULL);}}		/* not yet completed. */
        PushHist(H_DOT);
        if (NxtTok() != IDENT) {errExp(IdExptd, NULL, NULL); break;}
        if (cExp.type != NULL) {
          Tqualif qual = cExp.type->qualif;
          bool strucFl = (cExp.type->typeSort == Struc);

          do {
            if (isSameName(member->hCode, member->idName, curTok.hCod,
								 curTok.name)) {
              const TpcTypeElt mType = member->type;

              FreeExpType2(cExp.type);
              if (mType==NULL || !(qual & ~mType->qualif)) cExp.type = mType;
              else {  /* propagate qualifier */
                TpTypeElt newElt = copyTypeEltNoOwn(mType);

                newElt->qualif |= qual;
                cExp.type = newElt;}
              if (member->fldFl) cExp.pseudoAttrib = Reg;  /* to prevent taking
						  the address of a bit-field. */
              goto foundMember;}
            if (strucFl && member->type!=NULL) cExp.pVal += ((IsStrun(member->
		type))? BaseStrunType(member->type) : member->type)->size; /*
							     compute l-value. */
          } while ((member = member->cdr) != NULL);
          errExp(UnknMember, cExp.type, nameToS(curTok.name));}
foundMember:
        GetNxtTok();
        cExp.notPureBool = False;  /* struct/union fields independant */
        break;}  /* 'lValFl', 'pseudoAttrib' keep their preceding value (cf.
		 cast number to pointer "(.. *)0" ); 'cExp.ptrId' not reset,
		  so as to be able to take notice of possible initialization. */
    case LPAR: {		/* function call */
        const TdeclElt *formParList;
        bool badLgth = False, formatFl = False;
        TREInfo savCEinfo;
        TpcTypeElt resulType = NoResulTyp;
        TresulExp slExp = cExp;
        Tname lastCalledFctName = decoratedName(&cExp);
        TsemanElt *finalPtrId = NULL;  /* a priori, function result have
					nothing in common with function name. */
        TpcTypeElt locAllocType = allocType, saveSizeofOpndType= sizeofOpndType;

        sizeofOpndType = allocType = NULL;
        paramTxt = "()";
        if (macParCtr == lastPrimMacParCtr) errWN(IneffOrSideEff|
					      Warn2|PossErr, lastCalledFctName);
        if (initialType != NULL) {
          if (IsFct(initialType)) {
            if (initialType->typeSort==VFct && lastCalledFctName!=NULL) {
              static const Tname tabIOf[] = {PrintfFamily, ConvTname(""),
		      ScanfFamily};  /* 'printf' family must come first
		     (because more frequent; see also 'ioFctNb' computation). */
              const Tname *ptrTIOf;
              size_t lgtId = Lgt(lastCalledFctName);

              for (ptrTIOf = &tabIOf[0]; ptrTIOf <= AdLastElt(tabIOf); ) {
                if (memcmp(lastCalledFctName, *ptrTIOf++, lgtId) == 0) {  /*
					       found xscanf/xprintf function. */
                  formatFl = True;
                  ioFctNb = (int)(ptrTIOf - &tabIOf[SizePrintfFamily + 1 /* 
							   because of '++' */]);
                  break;}}}
/*            else {
              TpcTypeElt nxtType = NxtTypElt(initialType);

              if (nxtType!=NULL && nxtType->generic && nxtType->typeSort==
		   Double) fltDblType = True;}  .. for library math functions */
            goto correctL;}
          errExp(IlgLftType, initialType, NULL);}
        initialType = allocTypeEltID(VFct, NoQualif);  /* just to simplify
		     coming life ('nextTE' field = 'paramList' field = NULL). */
        ((ModifType)initialType)->noOwner = True;
correctL:
        if (cExp.macLvl==0 && macLvl!=0) err0(LParOutsideMac|Warn1|Rdbl);
	      /* case "exit EXIT_SUCCESS", with EXIT_SUCCESS defined as "(1)" */
        savCEinfo._eInfo = cExp.Einfo;
        formParList = initialType->paramList;
        if (NxtTok() != RPAR) {
          bool resulPtrSeen = False;

          do {
            if (formParList!=NULL && formParList->SizeofBlFl) {
              if (locAllocType == NULL) sizeofOpndType = XAllocSeen;
              else if (locAllocType != XAllocSeen) {FreeExpType2(locAllocType)
						  ; locAllocType = XAllocSeen;}}
            {
              int savIoFctNb = ioFctNb;

              asgnExpr();
              ioFctNb = savIoFctNb;}
            {
              const TpcTypeElt eType = cExp.type;

/*              if (loopLvl!=0 && cExp.ptrId!=NULL) cExp.ptrId->forceUsed = True;
/ case "a=f(a)" inside loop; but seems to denote bad coding, so warning left */
              paramTxt = "()";  /* paramTxt used in managePointers() */
              lExp.type = (formParList == NULL)  /* variadic part */
                ? (ScanfFl)
                  ? &charPtrTypeElt  /* NOTALLCONST type, to prevent xscanf
					 parameters from being asked 'const'. */
                  : NULL
                : formParList->type;
              /* xscanf will not change its parameters in case of input or match-
                 ing failure, so parameters cannot be marked 'not used'. */
/*              if (ScanfFl && cExp.ptrId!=NULL && (cExp.PointingOnObj || cExp.ptrId
		  ->type!=NULL && IsArr(cExp.ptrId->type) && cExp.ptrId->kind!=
		  Param && cExp.Drefncd==0)) cExp.ptrId->used = cExp.oldUsed;
		  * for UnusedVar1 detection (check not done for array para-
				 meter, because of possible 'UnusedPar' msg). */
              managePointers(lExp.type);
              if (formParList == NULL) {
                CheckNumCstNamed(cExp, eType);
                badLgth = True;
                if (formatFl && !folwngSpe(eType)) {err0(FmtExhstd|Warn2|
						   PossErr); formatFl = False;}}
              else {  /* non exhausted formal parameters list */
                TpcTypeElt formType = formParList->type;

                CheckNumCstNamed(cExp, formType);
                if (eType != NULL) {  /* correct actual parameter */
                  errQalTyp1 = NULL;
                  if (formParList->parQal == ResulTypV) heedRootType = False;
                  if (formParList->parQal==UtilityV && eType->paralTyp || !
		     CompatType(formType, eType, FctCall) && !compatNumType()) {
                    heedRootType = True;
                    /* Skip over bad manners of some system header files */
                    if (!(eType->typeSort==Enum && eType->intPoss && IsTypeSort(
							      formType, Who))) {
                      errId3(warnOrErr(IncptblPar|CWarn|PossErr), &slExp,
					 typeToS1(eType), typeToS2(formType));}}
                  else {
                    heedRootType = True;
                    if (IsStrun(eType) && formParList->MayModifFl) {
                      TsemanElt *ptrId = cExp.ptrId;

                      if (ptrId != NULL) {
                        if (ptrId->kind==Param && !ptrId->MayModif && (!ptrId->
					      Dmodfd || allErrFl) && askConstFl)
                          errId2(MayModify|Warn1|Rdbl, ptrId, nameToS(
						     curFctName), errTxt[MayM]);
                        ptrId->Dmodfd = True;}}
                    if (errQalTyp1 != NULL) errWSTT(ConstQalNotHeeded2|
			ConstWarn|PossErr, (errQalTyp1 == formType)? NULL :
					errTxt[SubPre], errQalTyp1, errQalTyp2);
                    if (IsArr(eType) && !cExp.lValFl) err0(NotLValP);
                    /*if (cExp.ptrId!=NULL && eType->qualif&VolatQal) errId1(
			      CopyVolat | Warn2, cExp.ptrId, typeToS1(eType));*/
                    if (formParList->SizeofBlFl) {locAllocType =
					 sizeofOpndType; sizeofOpndType = NULL;}
                    if (formParList->parQal == ResulTypV) {
                      if (resulType == NoResulTyp) resulType = arrToPtr(eType);
				 /* because array parameter passed by address */
                      else {
                        TpcTypeElt w;

                        if (CompatType(resulType, eType, FctCall)) /* 'FctCall'
				       because of "strcpy(array, array/ptr)". */
                          if (formParList->ResulPtrFl && !CompatType(eType,
							  resulType, FctCall)) {
                            errWSTT(ResulPtrNotHghsTyp|Warn2|PossErr, dpName(
						   RESULTYP), eType, resulType);
                            w = arrToPtr(eType);}
                          else goto noChangeL;
                        else
                          if (compatType(eType, resulType, FctCall)) {
                            if (resulPtrSeen) {
                              errWSTT(ResulPtrNotHghsTyp|Warn2|PossErr, dpName(
						   RESULPTR), eType, resulType);
                              goto noChangeL;}
                            w = arrToPtr(eType);}
                          else {errWSTT(NoHghrTyp|Warn2|PossErr, NULL,
						   resulType, eType); w = NULL;}
                        FreeExpType2(resulType);
                        resulType = w;
noChangeL:;}
                      if (formParList->ResulPtrFl) {
                        finalPtrId = cExp.ptrId;
                        resulPtrSeen = True;}}}}
                if ((formParList = formParList->cdr)==NULL && formatFl) {
                  if (eType!=NULL && IsArr(eType) && eType->generic && !cExp.
		     errEvl) initFlwgSpe();  /* correct literal format string */
                  else formatFl = False;}}
              if (eType != resulType) FreeExpType1(eType);}
          } while (Found(COMMA));}
        if (curTok.tok != RPAR) err0(RParExptd);  /* here to be before 
						   'BadActParListLgth' error. */
        if (formParList!=NULL || (badLgth && initialType->typeSort!=VFct)
				) errId3(BadActParListLgth, &slExp, NULL, NULL);
        if (formatFl && folwngSpe(NULL)) err0(TooManyFmtSpe|Warn2|PossErr);
        ioFctNb = 0;  /* reset ScanfFl/PrintfFl */
        cExp.Einfo = 0;  /* reset all flags */
        cExp.notPureBool = savCEinfo.s21._lclAdr;  /* same field */
        {
          bool pvNrFl = initialType->pvNr;  /* PseudoVoid/NeverReturns */

          cExp.type = advInTypeChain(initialType);
          if (resulType != NoResulTyp) {
            if (cExp.type!=NULL && (resulType==NULL || resulType->typeSort==
			   cExp.type->typeSort || IsTypeSort(cExp.type, Flt))) {
              FreeExpType(cExp);
              cExp.type = resulType;}
            else FreeExpType1(resulType);}
          cExp.topOper = (cExp.type != NULL)
                           ? (cExp.type->typeSort == Void)
                             ? (nxtStmtRchbl = (pvNrFl)? NonRchbl_Jmp : Rchbl,
								VFCTC | PRIMARY)
                             : (pvNrFl)
                               ? VFCTC | PRIMARY
                               : FCTC | PRIMARY
                           : VFCTC | PRIMARY;}
        (void)Found(RPAR);  /* here for good error position (also for good
						management of 'usrLvlParFl'). */
        cExp.ptrId = finalPtrId;
        cExp.FctCallSeen = True;
        lastCalledFct = slExp.ptrId;  /* for 'NotPureSideEffect1' error */
        allocType = locAllocType;
        sizeofOpndType = saveSizeofOpndType;
        break;}
    case LSBR: {		/* indexation */
        TresulExp slExp;
        TpcTypeElt lType;
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
        CheckNumCstNamed(cExp, (lType==NULL || lType->indexType==NULL)? cExp.
						       type : lType->indexType);
        cleanExprThings();
        apsndOpnd = savAO;
        cExp.pseudoAttrib = slExp.pseudoAttrib;
        cExp.FctCallSeen = False;
        if (lType != NULL) {
          const TpcTypeElt eType = cExp.type;
          Tqualif qual = NoQualif;  /* qualifier to be propagated */

          lExp.type = lType->indexType;
          if (eType != NULL) {
            if (! IsTypeSort(eType, WhoEnumBool)) errWNT(BadIndexType1,
						  decoratedName(&slExp), eType);
            else {
              if (lType->indexType != NULL) {  /* array with bound type
						   specified (without error). */
                if ((slExp.ptrId==NULL || !slExp.ptrId->noErrTypInd) &&
			!(NakedTok(cExp.topOper)==CSTNU && cExp.uVal==0 &&
						   cExp.type==&intCstTypeElt)) {
                  if (paramTxt = NULL, !compatNumType()) errId3(BadIndexType|
			Warn1|PossErr, &slExp, (lType->indexType ==
			   &defaultIndexTypeElt)? errTxt[Arithm] :
			     typeToS2NoQual(lType->indexType), typeToS1(eType));
                  else if (moreIndexTypeChk && eType->paralTyp && isRepreType(
			lType->indexType)) errId3((IsPtr(lType))?
			UseIndexTypForPtr | Warn1 : BndShdBeOfNamedType | Warn1,
			&slExp, (lType->indexType == &defaultIndexTypeElt)?
			errTxt[Arithm] : typeToS2NoQual(lType->indexType),
							       typeToS1(eType));
                  else goto noErrL;
                  if (!allErrFl && slExp.ptrId!=NULL) slExp.ptrId->noErrTypInd
									 = True;
noErrL:;}}
              if (IsPtr(lType)) indirindex(&slExp);
              else {
                if (lType->lim + 1U > 1U) {  /* array with size
						   specified (without error). */
                  const  TcalcS valBound = (TcalcS)lType->lim;

                  if (cExp.rEvlbl) {
                    if ((cExp.sVal>valBound || !apsndOpnd && cExp.sVal==
						    valBound) && !cExp.errEvl) {
                      TnbBuf w;

                      bufLongToS(valBound, w);
                      errWSS(IndexValTooBig|Warn3|PossErr, longToS(cExp.sVal),
									   w);}}
                  else if (eType!=NULL && eType->typeSort < computeLstType(
	 	       valBound - 1)) errId3(IndexTypeTooSmall | Warn2, &slExp,
					   typeToS1(eType), longToS(valBound));}
                if (cExp.rEvlbl && !cExp.errEvl && cExp.sVal<0) err0(
						     NegIndexVal|Warn3|PossErr);
                cExp.notPureBool = slExp.notPureBool;  /* also .lclAdr */
                cExp.PointedByObj = (slExp.ptrId!=NULL && slExp.ptrId->kind==
			Param && slExp.ptrId->type!=NULL && IsArr(slExp.ptrId->
					      type))? True : slExp.PointedByObj;
                cExp.FctCallSeen = slExp.FctCallSeen;
                qual = lType->qualif;}}
            FreeExpType2(eType);}
          cExp.type = advInTypeChain(lType);
          if (qual!=NoQualif && cExp.type!=NULL && qual & ~cExp.type->qualif) {
            /* Propagate qualifier */
            TpTypeElt w = copyTypeEltNoOwn(cExp.type);

            w->qualif |= qual;
            cExp.type = w;}}
        else {FreeExpType(cExp); cExp.type = NULL;}
        cExp.lEvlbl = slExp.lEvlbl & cExp.rEvlbl;
        if (cExp.lEvlbl && !cExp.errEvl) cExp.pVal = slExp.pVal +
						      cExp.sVal*(TcalcS)ptdSize;
        cExp.oldUsed = slExp.oldUsed;
        cExp.hist = slExp.hist;
        if (! slExp.PointingOnObj) PushHist(H_ARRAY);
        cExp.ptrId = slExp.ptrId;
        if (! Found(RSBR)) err0(RSBrExptd);
        cExp.lValFl = ! cExp.FctCallSeen;  /* case "&fct().tab[0]" */
        cExp.cstImpsd = False;
        cExp.rEvlbl = False;
        cExp.unsigDiff = False;
        cExp.PointingOnObj = False;
        cExp.topOper = LSBR | PRIMARY;
        break;}  /* cExp.ptrId not reset, for example to be able to take
				  notice of possible initialization of array. */
    default:
      return;}}
}

static void resulIncOp(TkInc oper, TmacParCtr ctrMP)
{
  TpcTypeElt eType;

  paramTxt = txtIncTok[oper];
  manageUsedForIncOp(); /* here because of 'usrLvlParFl' */
  if (ctrMP == lastPrimMacParCtr) err1(SideEffInMacPar|Warn3|PossErr);
  if ((eType = cExp.type) != NULL) {
    if (! cExp.lValFl) {errWSS(NotLVal, paramTxt, NULL); cExp.ptrId = NULL;}
    else {
      if (IsPtr(eType)) (void)sizeOfTypeI(eType, True, UndefPtdSize, paramTxt);
      else if (! IsTypeSort(eType, NumEnum)) errExp(IlgOpndType, eType, NULL);
      if (eType->qualif & ConstQal) errCompoOf(True);
      else if (cExp.PointedByObj) modifPtdVal(WillM);}}
  cExp.lValFl = False;
}

static void specialCaseOp(TkAdd oper)
/* If 'AddSubAsgn': does not change either cExp.ptrId or lExp.ptrId,
   else if 'pointer +/- nb' sets cExp.ptrId to lExp.ptrId (done in fact by
								   resulExp() ),
   else sets cExp.ptrId to NULL. */
{
  const TpcTypeElt lType = lExp.type, eType = cExp.type;
  const TtypeSort tsl = lType->typeSort;

  if (tsl==Enum /* enum +/- distance */
      || tsl & NumEnum && lType->rootTyp && lExp.rEvlbl &&
			       cExp.rEvlbl /* so that "(RootType)3 + 1" OK */) {
    /* Heterogeneous operands possible */
    if (eType != NULL) {
      if (!eType->paralTyp && (eType->typeSort & (Int | UInt) || cExp.cstImpsd
			     && eType->typeSort & (Byte|Short) /* cf '?:' */)) {
        if (eType->sysTpdf && !cExp.rEvlbl && chkPortbl) errWSTT(NonPortOpnd |
						CWarn2, paramTxt, eType, lType);
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
      TsemanElt *w = cExp.ptrId;

      cExp.ptrId = lExp.ptrId;
      ptdSize = sizeOfTypeI(lType, True, UndefPtdSize, paramTxt);
      cExp.ptrId = w;}
    if (eType != NULL) {
      if (oper==Sub && IsPtr(eType)) {
        if (!CompatType(lType, eType, NoCheck) || !CompatType(lType->indexType,
	      eType->indexType, VerifCast)) errWSTT(warnOrErr(DiffIncptblPtr|
						  PossErr), NULL, lType, eType);
        else if (lExp.lclAdr!=cExp.lclAdr && (loopLvl==0 || loopLvl>0 &&
	      condStmtLvl==initialLoopCondLvl || lExp.PointingOnObj && cExp.
				PointingOnObj)) err0(MnlsPtrDiff|Warn3|PossErr);
        FreeExpType2(eType);
        if (! ptrdiffSeen) {
          TsemanElt *ptrId;

          if ((ptrId = searchSymTabHC(PtrdiffTName, PtrdiffTHCode))==NULL ||
							    ptrId->type==NULL) {
            if  (! stddefNotInFl) {
              err0(StddefNotIncl|Warn2|Rdbl|PossErr);
              stddefNotInFl = True;}}
          else {
            ptrdiffSeen = True,
            ptrdiffTypeElt = *ptrId->namedType;}}
        cExp.type = &ptrdiffTypeElt;
        if (cExp.rEvlbl) {
          if (ptdSize!=0 && !cExp.errEvl) cExp.sVal = (TcalcS)((lExp.pVal -
						 cExp.pVal)/(ptrdiff_t)ptdSize);
          ((ModifType)(cExp.type = copyTypeEltNoOwn(cExp.type)))->generic =
									  True;}
        cExp.PointingOnObj = False;
        goto freeLTypeL;}
      if (! IsTypeSort(eType, WhoEnumBool)) errExp(IlgRhtType, eType, NULL);
      else {
        TsemanElt *ptrLId;

        lExp.type = (lType->indexType != NULL)? lType->indexType :
							   &defaultIndexTypeElt;
        if (((ptrLId = lExp.ptrId)==NULL || !ptrLId->noErrTypInd) &&
		!(NakedTok(cExp.topOper)==CSTNU && cExp.uVal<=1 && cExp.type==
							      &intCstTypeElt)) {
          Tstring savParamTxt = paramTxt;

          if (paramTxt = NULL, !compatNumType()) errId3(BadIndexType|Warn1|
		PossErr, &lExp, (lExp.type == &defaultIndexTypeElt)? errTxt[
			  Arithm] : typeToS2NoQual(lExp.type), typeToS1(eType));
          else if (moreIndexTypeChk && eType->paralTyp && isRepreType(lExp.
		  type)) errId3(UseIndexTypForPtr | Warn1, &lExp, (lExp.type
		    == &defaultIndexTypeElt)? errTxt[Arithm] : typeToS2NoQual(
						   lExp.type), typeToS1(eType));
          else goto noErrL;
          if (!allErrFl && ptrLId!=NULL) ptrLId->noErrTypInd = True;
noErrL:
          paramTxt = savParamTxt;}}}
    if (oper != AddSubAsgn) {
      if (cExp.rEvlbl && !cExp.errEvl) {
        cExp.pVal = (oper == Add)? lExp.pVal + cExp.sVal*(TcalcS)ptdSize :
					 lExp.pVal - cExp.sVal*(TcalcS)ptdSize;}
      FreeExpType(cExp); /* because of possible errExp() */
      if (tsl == Array) {  /* "tab + i" */
        errWSTT(IlgLftType|Warn1|Rdbl, paramTxt, lType, NULL);
						       /* not string constant */
        ((ModifType)(cExp.type = copyTypeEltNoOwn(lType)))->typeSort = Ptr;}
      else cExp.type = lType;
      cExp.hist = lExp.hist;}
    cExp.lclAdr = lExp.lclAdr;
    cExp.pseudoAttrib = lExp.pseudoAttrib;
    goto exitL;}
  if (oper==Add && eType!=NULL &&
       (!lType->paralTyp && (eType->typeSort==Enum && (tsl & Int || lExp.
				  cstImpsd && tsl & (Byte|Short) /* cf '?:' */))
        || (IsPtr(eType) || IsArr(eType) && eType->generic) && IsWhoEnumNotChar(
		  lType))) errExp(TryPermut|Warn2|Rdbl, lType, typeToS2(eType));
  else if (oper == AddSubAsgn) errAsgn = True;
  else errWrngType(NumEnum | PtrPoss);  /* 'PtrPoss' because Array<=>Ptr */
freeLTypeL:
  cExp.lclAdr = False;
  if (oper != AddSubAsgn) {FreeExpType2(lType); cExp.ptrId = NULL;}
exitL:
  cExp.cstImpsd = False;  /* well-defined type anyway */
  cExp.unsigDiff = False;
}

/******************************************************************************/
/*                                 UTILITIES                                  */
/******************************************************************************/

static TpcTypeElt advInTypeChain(TpcTypeElt x)
{
  TpcTypeElt nxtType = NxtTypElt(x);

  if (x->noOwner) {
    if (nxtType!=NULL && !nxtType->shared && !x->stopFreeing) ((ModifType)
			    nxtType)->noOwner = True;  /* propagate attribute */
    (void)freeTypeElt(x);}
  return nxtType;
}

static AllocXElt(allocNotInitVarElt, TnotInitVar *, ctrNI, ;)

static TpcTypeElt arrToPtr(TpcTypeElt type)
{
  if (! IsArr(type)) return type;
  {
    TpTypeElt nType = copyTypeEltNoOwn(type);

    nType->typeSort = Ptr;
    nType->size = PtrSiz;
    nType->generic = False;
    nType->nextTE = NxtTypElt(nType);
    nType->paralTyp = False;
    if (nType->qualif&ConstQal && nType->nextTE!=NULL && !(nType->nextTE->
			qualif & ConstQal)) {  /* propagate 'const' qualifier */
      TpTypeElt nNxtType = copyTypeEltNoOwn(nType->nextTE);

      nNxtType->qualif |= ConstQal;
      nType->nextTE = nNxtType;
      nType->stopFreeing = False;}
    return nType;}
}

static bool authzdType(TpcTypeElt type, TtypeSort msk, bool *pErrFl, Terr x)
{
  TtypeSort msk1, ts;

  if (type==NULL || (((ts = type->typeSort) < DelType)
                       ? ts
                       : (ts==Ptr || ts==Array && type->generic)
                         ? PtrPoss
                         : (IsStrunSort(ts))
                           ? StrunPoss
                           : Void) & msk) return True;  /* type authorized
								      by dcc. */
  msk1 = (msk & WhoEnumBool)? WhoEnumBool : Void;  /* 'real' C integral types */
  if (msk & (Flt | BoolOpnd)) msk1 |= Flt;
  errWSTT(x | (((IsPtrArrSort(ts))? msk&(PtrPoss | BoolOpnd) : ts&msk1)? Warn2 :
				  (*pErrFl = True, Err)), paramTxt, type, NULL);
  return False;
}

static void checkIncldFiles(const TsemanElt *ptrId)
{
  if (!headerFile && ptrId->declaringFile!=nonFileName && !ptrId->defnd &&
	(!ReallyInsideMacro || curMacDFName()==curFileName) && chkInclPos &&
	   !isBodyHdrFile(ptrId->declaringFile) && addLvl0InclFName(ptrId->
	     declaringFile)) errWNSS(BadlyIncldHdrFile|Warn1|Rdbl, ptrId->nameb,
		ptrId->declaringFile, NULL);  /* message only once for each
									file. */
}

static void checkInfoLoss(TpcTypeElt x)
/* Check for loss of information (constant expression too big for 'x' type) */
{
  if (! cExp.errEvl) {
    TtypeSort ts = x->typeSort;

    if (ts == Bool) ts = UInt;
    else if (ts == Enum) ts = (x->tagId == NULL)?
#if SignedChar
						  Byte
#else
						  UByte
#endif
							: Int; /* 'real' enum */
    if (computeCstType(cExp.sVal, ts) > ts) {
      errOvfl(x);
      /* Truncate value (making no assumption as to internal representation of
         numbers). */
      if (ts & UnsigWho) ts &= UnsigWho;
      switch (ts) {	/* *doIt* float/crossCompil */
      case Int:		cExp.sVal = (TcalcS)(int)cExp.sVal; break;
      case UInt:	cExp.uVal = (TcalcU)(uint)cExp.uVal; break;
      case Short:	cExp.sVal = (TcalcS)(short)cExp.sVal; break;
      case UShort:	cExp.uVal = (TcalcU)(unsigned short)cExp.uVal; break;
      case Byte:	cExp.sVal = (TcalcS)(char)cExp.sVal; break;
      case UByte:	cExp.uVal = (TcalcU)(ubyte)cExp.uVal; break;
      /*~ NoDefault */}}}
}

void checkInit(void)
{
  register TsemanElt *ptrId;

  if ((ptrId = cExp.ptrId)!=NULL  && evaluateFl) {
    ptrId->MsngConstQM = False;
    if (!ptrId->initlz && ptrId->kind==Obj && (ptrId->attribb>=Auto || ptrId->
				      attribb==StatiL) && !cExp.PointingOnObj) {
      if (loopLvl==0 || loopLvl>0 && condStmtLvl==initialLoopCondLvl) errInit(
			   ptrId, 0, NULL, initDPFl);  /* must be initialized */
      else {
        /* Might be initialized later, so add to list to be checked at end of
           loop. */
        register TnotInitVar *w = notInitVarList;

        while (w != NULL) {
          if (ptrId == w->ptrId) return;  /* already in list */
          w = w->prec;}
        w = allocNotInitVarElt();
        w->ptrId = ptrId;
        w->lineNb = lineNb;
        w->fileName = curFileName;
        w->initDPFl = initDPFl;
        w->prec = notInitVarList;
        notInitVarList = w;}}}
}

void checkNumCstNamed(TresulExp *pExp, TpcTypeElt sinkType)
{
  if ((pExp->type==NULL || !pExp->type->litCsta) && !CheckSink(pExp, sinkType)){
    if (! chkNumCst1) return;
    err0((pExp->LitCstOutsMac)? NumCstShdBeNamed|Warn1|Rdbl :
						 NumCstShdBeNamed1|Warn1|Rdbl);}
  pExp->litCst = False;
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
  const TpcTypeElt eType = cExp.type;

  CheckNumCstNamed(cExp, eType);
  if (! IsSideEffect(cExp)) {
    if (NakedTok(cExp.topOper) == FCTC) {
      cExp.ptrId = lastCalledFct;
      errWN(NotPureSideEffect1|Warn2|PossErr, decoratedName(&cExp));}
    else {
      if (eType!=NULL && eType->typeSort!=Void) err0((IsFct(eType))
			? MsngFctParen|Warn3|PossErr
			: NotPureSideEffect|Warn2|PossErr);}
    cExp.ptrId = NULL;}
  else {
    if (NakedTok(cExp.topOper) == VFCTC) cExp.ptrId = NULL;}  /* for ~PseudoVoid
						  functions with ~ResultType. */
  valueNotUsed();  /* for example, so that "a++;" => NotUsed */
  FreeExpType1(eType);
}

void cleanExprThings(void)
/* cleanExprThings (or managePointers) should be called at end of each expression,
   be it correct or not. */
{
  if (preLastCastType != NULL) freePrev();
  if (allocType != NULL) freeAlloc();
}

static TpcTypeElt commonType(TtypeSort msk)
/* Returns NULL if type incompatibility between lExp.type, cExp.type and 'msk';
   else resulting type.
   If changes made here, change also 'compatType()' (in file dcdecl.c) */
{
  TpcTypeElt lType = lExp.type, rType = cExp.type;
  TtypeSort tsl = lType->typeSort, tsr, resTS;
  bool rhtHghrParal, retRepreType;

  convToUnsgnd = NoErrMsg;
  if (lType==rType && tsl&msk) return lType;
  tsr = rType->typeSort;
  resTS = tsl & tsr & msk;
  rhtHghrParal = retRepreType = False;
  {
    TpcTypeElt lTyp1, rTyp1;

    if (lType->paralTyp) {
      lTyp1 = lType; rTyp1 = rType;}
    else {
      if (! rType->paralTyp)  /* none parallel type */
        if (rType->sysTpdf) goto rHiMayBeOkL;
        else goto mayBeOkL;
      lTyp1 = rType; rTyp1 = lType;}
    if (! rTyp1->paralTyp) {  /* only one is parallel type */
      /* Is it visible from the other one? */
      do {
        if (lTyp1->rootTyp && heedRootType) goto tstBoolViaInsertBool;  /* no */
        lTyp1 = lTyp1->typeId->type;
      } while (lTyp1->paralTyp);
      /* Is one typeSort subset of the other? */
      if (resTS == Void) {   /* no */
        if (rTyp1->generic) {
          if (lType->generic) {if (tsr & Flt) tsl = tsr;}
          else {if (tsl & Flt) tsr = tsl;}
          goto mayBeOkL;}
        retRepreType = True;
        goto rHiMayBeOkL;}
      if (lType->paralTyp) {
        if (rTyp1->generic) goto mayBeOkL;
        goto rHiMayBeOkL;}
      if (! rTyp1->generic) goto mayBeOkL;
      goto rHiMayBeOkL;}
    /* Both parallel type: find if one is higher and, if so, which one */
    if (lTyp1->typeId == rTyp1->typeId) goto mayBeOkL;  /* 'typeId' checked
		     (and not lTyp1 == rTyp1) because of possible qualifiers. */
    /* Row up hierarchy */
    while (!rTyp1->rootTyp || !heedRootType) {
      rTyp1 = rTyp1->typeId->type;
      if (! rTyp1->paralTyp) break;
      if (rTyp1->typeId == lTyp1->typeId) {  /* lType higher than rType */
        if (lType->generic) goto rHiMayBeOkL;
        goto mayBeOkL;}}
    while (!lTyp1->rootTyp || !heedRootType) {
      lTyp1 = lTyp1->typeId->type;
      if (! lTyp1->paralTyp) break;
      if (lTyp1->typeId == rType->typeId) {  /* rType higher than lType */
        if (! rType->generic) goto rHiMayBeOkL;
        goto mayBeOkL;}}
    /* no higher type */
tstBoolViaInsertBool:
    if (!(resTS & Bool) || tsl==Bool && tsr==Bool) return NULL;  /* if not
				  case "'bool' inserted" (via insertBool() ). */
    goto mayBeOkL;
rHiMayBeOkL:
    rhtHghrParal = True;
mayBeOkL: ;}
  if (!(resTS & Enum) || lType->tagId!=rType->tagId) {  /* not enums or not
								  same enums. */
    /* First, take care of 'bool' case (beware of 'insertBool()') */
    resTS &= Bool;
    msk &= Num;
    if ((tsl &= msk)==Void || (tsr &= msk)==Void) {
      if (resTS == Void) return NULL;}
    else {
      /* Mixing variable and constant (expression) ? */
      if (cExp.rEvlbl) {
        if (lExp.rEvlbl) {
          /* Parallel type constant op generic constant => parallel type */
          if (! lType->paralTyp) {
            if (rType->paralTyp) goto computeRepreTslL;
            goto noChangeL;}}
        else if (!cExp.cstImpsd /* not "multi-typed" constant */ || tsl>=tsr /*
		   then, computation of real typeSort useless*/) goto noChangeL;
        tsr = computeCstType(cExp.sVal, (tsr>Int && tsr>tsl)? tsr : tsl);
        if (tsr>tsl && computeLstType(cExp.sVal)<=tsl) cExp.cstImpsd = False;
						  /* case "short s = 32767u;" */
noChangeL:;}
      else if (lExp.rEvlbl && lExp.cstImpsd && tsr<tsl)
computeRepreTslL: {
        tsl = computeCstType(lExp.sVal, (tsl>Int && tsl>tsr)? tsl : tsr);
        if (tsl>tsr && computeLstType(lExp.sVal)<=tsr) lExp.cstImpsd = False;}
      /* 'Usual arithmetic conversions': find greater type */
      {
        TtypeSort gtTS = (tsl == tsr)? (cExp.cstImpsd &= lExp.cstImpsd, tsl) :
                        (tsl  > tsr)? (cExp.cstImpsd  = lExp.cstImpsd, tsl) :
                        tsr;

#if NO_GREATER_INT
        if (gtTS==Long && (tsl==UInt || tsr==UInt)) gtTS = ULong;
#endif
        if (gtTS & UnsigWho) {
          if (!(lType->typeSort & UnsigWho) && !(lExp.rEvlbl && (lExp.errEvl ||
					    lExp.sVal>=0))) convToUnsgnd = Left;
          if (!(rType->typeSort & UnsigWho) && !(lType->typeSort & Enum) && !(
	   cExp.rEvlbl && (cExp.errEvl || cExp.sVal>=0))) convToUnsgnd = Right;}
        resTS |= gtTS;}}}
  if (!rhtHghrParal && resTS==lType->typeSort && (!lType->generic || rType->
							 generic)) return lType;
  if (!retRepreType && resTS==rType->typeSort && (!rType->generic || lType->
							 generic)) return rType;
  /* Parallel type op generic => original parallel type (because of
							       insertBool()). */
  if (rType->generic && (lType->paralTyp || lType->sysTpdf) && resTS==lType->
		   typeId->namedType->typeSort) return lType->typeId->namedType;
  if (lType->generic && (rType->paralTyp || rType->sysTpdf) && resTS==rType->
		   typeId->namedType->typeSort) return rType->typeId->namedType;
  {  /* New type element needed for result */
    TpTypeElt newElt = allocTypeEltID(resTS, NoQualif);

    newElt->noOwner = True;
    newElt->size = (resTS & lType->typeSort)? lType->size : rType->size;
    newElt->tagId = lType->tagId;  /* == rType->tagId */
    if (lType->generic && rType->generic) newElt->generic = True;
    return newElt;}
}

static bool compatNumType(void)
{
  TpcTypeElt resType, lType = lExp.type, eType = cExp.type;
  bool resul;

  lExp.cstImpsd = cExp.cstImpsd; lExp.rEvlbl = False;
  if ((resType = commonType(mskCmpat)) == NULL) return False;
  resul = True;
  if (lType->sysTpdf!=eType->sysTpdf && lType->typeSort>=eType->typeSort &&
				  !cExp.rEvlbl && paramTxt!=NULL && chkPortbl &&
		(lType->sysTpdf && InsideInterval(eType->typeSort, Long, Num) ||
		 eType->sysTpdf && !InsideInterval(lType->typeSort, Long, Num)))
    errWSTT(NonPortCmbn | CWarn1, paramTxt, lType, eType);
  else if (resType!=lType && (!((resType->typeSort & lType->typeSort) || lType->
	    typeSort==Float && resType->typeSort==Double) || lType->paralTyp)) {
    if (paramTxt == NULL) resul = False;
    else if (cExp.cstImpsd && !(IsTypeSort(resType, Flt) && IsTypeSort(lType,
									    Who)
#if NO_GREATER_INT
    || lType->typeSort==Long && cExp.uVal<=INT_MAX
#endif
						   )) errOvfl1(lType, paramTxt);
    else if (convToUnsgnd != NoErrMsg) {
      if (! (cExp.rEvlbl && (cExp.errEvl || cExp.sVal>=0 && computeLstType(
		cExp.sVal)<=lType->typeSort))) errWSTT((convToUnsgnd == Left)?
		    PossOvfl|Warn2|PossErr : ConvToUnsig1|Warn2|PossErr,
						       paramTxt, lType, eType);}
    else resul = False;}
  else if (convToUnsgnd!=NoErrMsg && paramTxt!=NULL) errWSTT(ConvToUnsig1|Warn2
					      |PossErr, paramTxt, lType, eType);
  if (resType!=lType && resType!=eType) FreeExpType2(resType);
  return resul;
}

static TtypeSort computeCstType(TcalcS value, TtypeSort msk)
/* Returns the smallest type (compatible with 'msk') in which 'value' can be
   represented. */
{
  TtypeSort resTS = (InsideInterval(msk, Float, Flt))? Float : computeLstType(
					value);  /* *doIt* for floating types */

  if (resTS == Bool) resTS = Byte;
  msk = (msk & (SigAri | Enum))? SigAri : UnsigWho;
  return (resTS & msk)
		? resTS
                : (msk == UnsigWho)
		  ? resTS << 1
#if USHRT_MAXC == UINT_MAXC
		  : (resTS == UShort)
		    ? Long  /* because 40000=>UShort=>Long (not Int), if 'int'
				      is defined to be equivalent to 'short'. */
#elif NO_GREATER_INT
		  : (resTS == UInt)
		    ? ULong
#endif
		  : resTS << 1;
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
#if NO_GREATER_INT
  return Int;
#else
  if ((TcalcU)(value - INT_MINC) <= (TcalcU)((TcalcS)INT_MAXC - INT_MINC)
								   ) return Int;
  if ((TcalcU)value <= UINT_MAXC) return UInt;
#  ifndef LONGLONG
  return Long;
#  else
  if ((TcalcU)(value - LONG_MINC) <= (TcalcU)((TcalcS)LONG_MAXC - LONG_MINC)
								  ) return Long;
  if ((TcalcU)value <= ULONG_MAXC) return ULong;
  return LLong;
#  endif
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

static TpTypeElt copyTypeEltNoOwn(TpcTypeElt x /*~MayModify*/)
{
  if (x->noOwner) return (ModifType)x;
  {
    TpTypeElt res = allocTypeEltIC(x, NoQualif);

    res->noOwner = True;
    return res;}
}

static void errCompoOf(bool notStrun)
{
  Tmember w = isMember();
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
}

static void errOvfl(TpcTypeElt type)
{
  errOvfl1(type, paramTxt);
}

static void errOvfl1(TpcTypeElt type, Tstring x)
{
  if (! cExp.errEvl) {
    errWSTT(((type->typeSort & SigWhoEnum) != Void)? Overflow|Warn2|PossErr :
					Overflow2|Warn2|PossErr, x, type, NULL);
    cExp.errEvl = True;}
}

static void errUdfl(void)
{
  if (! cExp.errEvl) {
    err1(Underflow|Warn2|PossErr);
    cExp.errEvl = True;
    cExp.sVal = 0;}
}

static bool errWrngType(TtypeSort msk) /*~PseudoVoid*/
/* Returns True if 'real' error (that is, error for "regular C") or either type
   is NULL (type error occurred earlier), else False. */
{
  const TpcTypeElt lType = lExp.type, eType = cExp.type;
  bool resul = False, errMsg = False;

  if (! authzdType(lType, msk, &resul, IlgLftType|PossErr)) errMsg = True;
  if (! authzdType(eType, msk, &resul, IlgRhtType|PossErr)) errMsg = True;
  if (eType==NULL || lType==NULL) resul = True;
  if (!errMsg && !resul && (chkBool || !(msk & BoolOpnd))) {
    Terr n = warnOrErr((compatType(lType, eType, IlgIcp))? IlgTypes | PossErr :
							IncptblTypes | PossErr);

    if ((n & WarnMsk) == Err) resul = True;
    errWSSSS(n, paramTxt, typeToS1(lType), typeToS2(eType), (!resul && lType->
			   paralTyp && eType->paralTyp)? errTxt[UseTC] : NULL);}
  errExp(NoErrMsg, NULL, NULL);  /* cExp.type can be freed now */
  return resul;
}

static void freeAlloc(void)
{
  if (allocType != XAllocSeen) FreeExpType2(allocType);
  allocType = NULL;
}

static bool frstMemberCompat(TpcTypeElt type, TtypeSort castTSort)
{
  const TdeclElt *member;

  if ((member = BaseStrunType(type)->memberList) == NULL) return True;
  do {
    if (member->type==NULL || member->type->typeSort==castTSort || IsStrun(
	member->type) && frstMemberCompat(member->type, castTSort)) return True;
  } while (type->typeSort==Union && (member = member->cdr)!=NULL);
  return False;
}

static void freePrev(void)
{
  FreeExpType2(preLastCastType);
  preLastCastType = NULL;
}

static bool ignorableDP(TmacParCtr x)
{
  bool resul = ReallyInsideMacro && !InsideMacPar && macParCtr!=x;

  GetNxtTok();
  return resul;
}

static void indir(void)
{
  TpcTypeElt eType = cExp.type;

  if (eType != NULL) {
    if (! (IsPtr(eType) || IsArr(eType) && eType->generic)) {
      errExp(((paramTxt[0] == '-')? IlgLftType : IlgRhtType) |
			       ((IsArr(eType))? Warn1|Rdbl : Err), eType, NULL);
      goto noCheckInit;}
    lExp.type = eType->indexType;  /* for possible '&' operator applied on *x */
    cExp.type = advInTypeChain(eType);}
  checkInit();
noCheckInit:
  cExp.lEvlbl = cExp.rEvlbl;
  cExp.cstImpsd = False;
  cExp.unsigDiff = False;
  cExp.lValFl = True;
  cExp.FctCallSeen = False;
  indirindex(&cExp);
  if (! cExp.PointingOnObj) PushHist(H_PTR);
  else cExp.PointingOnObj = False;
}

static void indirindex(const TresulExp *x /*~MayModify*/)
{
  cExp.pseudoAttrib = (x->pseudoAttrib == NoAttrib)
			? NoAttrib
			: (x->lclAdr)
			  ? Auto
			  : Stati;
  cExp.rEvlbl = False;
  cExp.lclAdr = False;
  if (x->PointingOnObj) {if (x->ptrId != NULL) x->ptrId->used = True;}
  else cExp.PointedByObj = True;
}

void initExp(void)
{
  ptrdiffSeen = sizeofSeen = False;
}

static void insertBool(TtypeSort rExpTypeSort)
{
  if (cExp.type != NULL) {
    if (lExp.type==NULL || lExp.type->typeSort!=Bool || rExpTypeSort!=Bool) {
      cExp.type = copyTypeEltNoOwn(cExp.type);
      ((ModifType)cExp.type)->typeSort |= Bool;
      if (!cExp.rEvlbl || cExp.uVal>1) cExp.notPureBool = True;}
    else if (lExp.notPureBool) cExp.notPureBool = True;
    if (lExp.inhibWaNPB) cExp.inhibWaNPB = True;}
}

static Tmember isMember(void)
/* Returns positive non-nul if cExp describes a strun member (Member, or
   SingleMember if only member of struct, or member of union with no composed
   member). */
{
  if (cExp.ptrId==NULL || cExp.PointingOnObj) return NotMember;
  {
    TpcTypeElt w = cExp.ptrId->type;
    Thistory history = cExp.hist;

    if (history != H_EMPTY) {
      /* Explore history searching a 'dot' (access to a strun) */
      while ((history & (MskHistElt << (sizeof(history)*CHAR_BIT - SizeHistElt)
					 )) == H_EMPTY) history <<= SizeHistElt;
      while (history & ((H_PTR & H_ARRAY) << (sizeof(history)*CHAR_BIT -
	 SizeHistElt)) && w!=NULL) {w = NxtTypElt(w); history <<= SizeHistElt;}}
    if (cExp.type == w) return NotMember;
    if (history >> (sizeof(history)*CHAR_BIT - SizeHistElt) != H_DOT) return
								      Composite;
    {
      const TdeclElt *w1;

      if (w==NULL || (w1 = w->memberList)==NULL || w->typeSort==Struc && w1->
						       cdr!=NULL) return Member;
      do {
        if (w1->type!=NULL && IsCmpsdObj(w1)) return Member;
      } while ((w1 = w1->cdr) != NULL);}}
  return SingleMember;
}

static bool isType(void)
{
  return IsDecl1(BDecl) || curTok.tok==IDENT && curTok.ptrSem!=NULL &&
						      curTok.ptrSem->kind==Type;
}

static bool manageInitlzUsed(void) /*~PseudoVoid*/
/* Answers True if current object has to be marked 'not used' */
{
  bool resul = False;
  register TsemanElt *rPtrId;

  if (!cExp.PointedByObj && (rPtrId = cExp.ptrId)!=NULL && !zifExp) {
    if (rPtrId->initBefUsd && rPtrId->attribb!=Extrn) {
      if (!cExp.oldUsed || !rPtrId->defnd /* because of ~NotUsed */) {
        errWN(UslInitBU|Warn1|Rdbl, rPtrId->nameb);
        rPtrId->initBefUsd = False;}}  /* to prevent further error */
    if (rPtrId->attribb>=Auto || rPtrId->attribb==StatiL) {
      if (!rPtrId->used && rPtrId->initlz && condStmtLvl==0 && (rPtrId->type
	  ==NULL || !(IsArr(rPtrId->type) || IsStrun(rPtrId->type) && isMember()
	      ==Member))) errWNSS(UnusedVar1|Warn1|PossErr, rPtrId->nameb, NULL,
			      (rPtrId->MsngConstQM)? errTxt[MsngConstQ] : NULL);
      resul = (loopLvl==0 && rPtrId->attribb>=Auto);}
    rPtrId->initlz = True;}
  return resul;
}

static void manageLogOper(void (*pf)(void), Ttok x, bool y, Tstring z)
{
  TresulExp slExp;  /* left operand saving */
  TmacLvl operMacLvl = macLvl;
  bool savEvalFl = evaluateFl;

  if (cExp.ValMltplDef) cExp.rEvlbl = False;
  slExp = cExp;
  if (slExp.rEvlbl && (slExp.uVal != 0)==y) evaluateFl = False;
  condStmtLvl++;
  GetNxtTok();
  (*pf)();
  if (cExp.ValMltplDef) cExp.rEvlbl = False;
  evaluateFl = savEvalFl;
  /* Short-circuit evaluation */
  if (slExp.rEvlbl)
    if ((slExp.uVal != 0) == y) transfOpnd(&slExp, &cExp);
    else {if (! cExp.rEvlbl) warnCstBool(errTxt[SubPre]);}
  else if (cExp.rEvlbl)
    if ((cExp.uVal != 0) == y) transfOpnd(&cExp, &slExp);
    else warnCstBool(errTxt[SubPre]);
  checkInit();
  cExp.ptrId = NULL;  /* to prevent 2nd call to checkInit (in resulExp) */
  condStmtLvl--;
  lExp = slExp;
  paramTxt = z;
  if (! resulExp(mskBool, False, &computeShortCircuit, operMacLvl, x)) {
    /* Pointer(s) used as boolean */
    errWrngType(mskBool);
    FreeExpType(slExp);
    cExp.topOper = x;
    cExp.ptrId = NULL;}
  relOpTrtmt();
}

static void managePointers(TpcTypeElt sinkType)
/* managePointers()/cleanExprThings() should be called at end of each
   expression, be it correct or not. */
{
  {
    register TsemanElt *rPtrId = cExp.ptrId;
    bool flg = False;

    if (rPtrId != NULL) {
      /* Mark possible pointed object initialized */
      if (cExp.type!=NULL && IsPtrArr(cExp.type)) {
        switch (tstPtd((sinkType == NULL)? cExp.type : sinkType, True)) {
          case ALLCONST:
            if (cExp.PointingOnObj) {  /* pointer produced via '&' */
              rPtrId->used = True;
              if (paramTxt == txtAsgnTok[SimplAsgn]) rPtrId->forceUsed = True;
				/* if assignment to pointer, cannot trace usage
				   of pointed object's value any more. */
              if (paramTxt!=txtAsgnTok[SimplAsgn] || cExp.PointingOnObj &&
					NxtTypElt(cExp.type)->qualif&ConstQal) {
                if (rPtrId->initBefUsd) rPtrId->initlz = True;
                else {
                  cExp.PointingOnObj = False;  /* since receiving pointer/array
					is 'const', pointed object should be
					initialized (see next checkInit() ). */
                  initDPFl = True;}}}
            break;
          case NOTALLCONST:
            if (cExp.PointingOnObj /* produced via '&' */ && (rPtrId->used =
	       True, (paramTxt[0]=='r' && cExp.lclAdr || manageInitlzUsed()))) {
              if (paramTxt == txtAsgnTok[SimplAsgn] /* assignment */) rPtrId->
							       forceUsed = True;
              else if (sinkType != NULL) {  /* not variadic part */
                /* Pointed object supposed to be initialized/modified via recei-
	 	   ving pointer, except if 'return' and auto object; functions
		   may change value; objects can be choosed to be manipulated
		   only via pointers.
		   BEWARE: manageInitlzUsed causes side-effect. */
                if (paramTxt[0] == '(') {
                  rPtrId->used = False;
                  if (!ScanfFl) flg = True;}
                else rPtrId->used = cExp.oldUsed;}}
            else if (sinkType!=NULL && paramTxt[0]=='(' && cExp.hist==H_EMPTY
			&& rPtrId->type!=NULL && IsArr(rPtrId->type)
			&& manageInitlzUsed()) rPtrId->used = False;  /* array
						     represents its elements. */
            if (tstPtd(sinkType, False) & NOTALLCONST && !(rPtrId==lExp.ptrId
		&& paramTxt==txtAsgnTok[SimplAsgn]) /* not case "x = x->..." */
							    ) modifPtdVal(MayM);
            break;
          default:;}}
      checkInit();  /* must be here (because of earlier manageInitlzUsed) */
      initDPFl = False;
      if (flg) rPtrId->MsngConstQM = True;}}
  if (preLastCastType != NULL) {
    if ((errQalTyp1 = NULL, compatType(sinkType, preLastCastType, Asgn)) &&
	   errQalTyp1==NULL && sinkType!=NULL) errWSTT(PrevCastUsl|Warn1|Rdbl,
					      NULL, preLastCastType, cExp.type);
    freePrev();}
  if (allocType != NULL) {
    if (allocType != XAllocSeen) {
      if (sinkType!=NULL && IsPtrArr(sinkType)) {
        TpcTypeElt nxtType = NxtTypElt(sinkType), w = allocType;

        for (;; w = NxtTypElt(w)) {
          if (CompatType(w, nxtType, CStrictChk)) break;
          if (! IsArr(w)) {
            if (nxtType->typeSort != Void) errWSTT(WrngSizeof|Warn3|PossErr,
							      NULL, w, nxtType);
            break;}}}}
    freeAlloc();}
}

void manageRetValue(TpcTypeElt retType)
{
  TpcTypeElt eType;
  
  commaExpr();
  paramTxt = "return";  /* paramTxt used in managePointers() */
  CheckNumCstNamed(cExp, retType);
  managePointers(retType);
  if ((eType = cExp.type) != NULL) {
    errQalTyp1 = NULL;
    if (!CompatType(retType, eType, Asgn) && (lExp.type = retType,
							    !compatNumType())) {
      errWSTT(warnOrErr(BadRetType | PossErr), NULL, retType, eType);}
    else {
      if (errQalTyp1 != NULL) errWSTTS(ConstQalNotHeeded1|ConstWarn|PossErr,
		    paramTxt, errQalTyp1, errQalTyp2, (errQalTyp1 == retType)
						       ? NULL : errTxt[SubPre]);
      if (cExp.lclAdr && IsPtr(eType) && !Found(LOCALADR)) err0(
						   RetPtrOnLclAd|Warn3|PossErr);
      else if (retType!=NULL && retType->typeSort==Bool) warnNotPureBool(&cExp,
			  NULL);}  /* for function should behave as constant. */
    FreeExpType2(eType);}
}

static bool manageTypCmbn(TkOpTC oper) /*~PseudoVoid*/
{
  TpcTypeElt resulTC = searchTC(oper);

  if (resulTC == QuasiNULLval(TpcTypeElt)) return False;
  if (lExp.type->litCsta != cExp.type->litCsta) {
    CheckNumCstNamed(lExp, lExp.type);
    CheckNumCstNamed(cExp, cExp.type);}
  FreeExpType2(lExp.type);
  FreeExpType2(cExp.type);
  lExp.type = cExp.type = resulTC;
  return True;
}

static void manageUsedForIncOp(void)
{
  TsemanElt *rPtrId;

  checkInit();
  if (loopLvl==0 && !cExp.PointedByObj && (rPtrId = cExp.ptrId)!=NULL &&
					     rPtrId->attribb>=Auto && !zifExp) {
    /* Not inside loop: mark variable 'notUsed' (=> to be used after) */
    cExp.oldUsed = False;
    rPtrId->used = False;}
  cExp.topOper = INCOP;
}

static void modifPtdVal(Terr n)
{
  TsemanElt *ptrId = cExp.ptrId;

  if (ptrId != NULL) {
    if (isMember() != NotMember) {
      if (ptrId->kind != Param) return;
      if (!ptrId->MayModif && (!ptrId->Dmodfd || allErrFl) && tstPtd(ptrId->
			      type, False) & (ALLCONST | STRUN) && askConstFl) {
        errId2(MayModify|Warn1|Rdbl, ptrId, nameToS(curFctName), errTxt[n]);
        if (n==MayM && paramTxt==txtAsgnTok[SimplAsgn] && lExp.ptrId!=NULL &&
	   lExp.ptrId->kind==Obj) lExp.ptrId->CheckConst = True;}}  /* turn on
			   receiving pointer missing 'const' qualifier check. */
    ptrId->Dmodfd = True;}
}

void procSameType(void)
{
  TpcTypeElt lType;

  lType = (isType())? declType() : (asgnExpr(), valueNotUsed(), cExp.type);
  if (!Found(COMMA)) err0(CommaExptd);
  if (isType()) cExp.type = declType();
  else asgnExpr(), valueNotUsed();
  errQalTyp1 = NULL;
  cExp.uVal = (TcalcU)(compatType(lType, cExp.type, FctCall) && errQalTyp1 ==
		 NULL || (paramTxt = NULL, lExp.type = lType, compatNumType()));
  FreeExpType1(lType);
  FreeExpType(cExp);
  cExp.type = &boolCstTypeElt;
  cExp.Einfo = 0;  /* reset all flags */
}

static void relOpTrtmt(void)
{
  if (cExp.type != NULL) {
    FreeExpType2(cExp.type);
    cExp.type = (cExp.rEvlbl)? &boolCstTypeElt : &boolTypeElt;}
  cExp.notPureBool = False;
  cExp.unsigDiff = False;
  cExp.cstImpsd = False;
}

static bool resulExp(TtypeSort msk, bool cstOnRight, void (*ptrComputeResul)
	       (TpcTypeElt), TmacLvl operMacLvl, Ttok baseClass) /*~PseudoVoid*/
/* Returns False (without freeing operand types) if lExp.type is non NULL, but
   not compatible with cExp.type nor with 'msk', and there exist a "xPoss" bit
   in 'msk'. */
{
  const TpcTypeElt lType = lExp.type, eType = cExp.type;
  TpcTypeElt resType;

  if (lType!=NULL && eType!=NULL) resType = commonType(msk & NumEnumBool);
  else if (baseClass == MULOP) resType = NULL;
  else {  /* if one operand type NULL, assume result type is type of other */
    if (eType == NULL) {
      if (lType == NULL) resType = NULL;
      else {
        cExp.type = lType;
        resType = commonType(msk & NumEnumBool);
        cExp.type = NULL;}}
    else {
      lExp.type = eType;
      resType = commonType(msk & NumEnumBool);
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
  cExp.lValFl = False;
  if (cExp.litCst) {
    if (CheckSink(&cExp, lType)) {cExp.litCst = False; cExp.LitCstOutsMac 
							    = False; goto okL;}}
  else {
    if (!lExp.litCst || CheckSink(&lExp, eType)) goto okL;
    cExp.litCst = True;}
  if (lExp.LitCstOutsMac) cExp.LitCstOutsMac = True;
okL:
  if (lExp.unsigDiff) cExp.unsigDiff = True;
  if (lExp.ValMltplDef) cExp.ValMltplDef = True;
  if (! lExp.rEvlbl) cExp.rEvlbl = False;
  else if (lExp.errEvl) cExp.errEvl = True;
  if (resType == NULL) {
    if (lType!=NULL && msk>=PtrPoss) return False;
    if (errWrngType(msk) || !cExp.rEvlbl || !ifDirExp) goto exitL;
    resType = &longCstTypeElt;}  /* needed because of the poor coding of many
							 system header files. */
  if (cExp.rEvlbl) {
    if (eType==NULL || lType==NULL) cExp.errEvl = True;
    else if (! cExp.errEvl) {  /* evaluate result */
      (*ptrComputeResul)(resType);
      checkInfoLoss(resType);}}
  else if (cstOnRight && lExp.rEvlbl && lExp.topOper&PRIMARY) err1(
						       CstOnLftSide|Warn1|Rdbl);
exitL:
  if (lType != resType) FreeExpType1(lType);
  if (eType != resType) FreeExpType(cExp);  /* cExp.type may have been changed
						  by errWrngType => != eType. */
  cExp.type = resType;
  cExp.topOper = baseClass;
  cExp.ptrId = NULL;
  return True;
}

static void resulUnOp(TtypeSort msk, Tstring operTxt)
{
  bool err = False;

  checkInit();
  paramTxt = operTxt;
  if (! authzdType(cExp.type, msk, &err, IlgRhtType | PossErr))
    if (ifDirExp && !err && cExp.rEvlbl) {FreeExpType(cExp); cExp.type =
							       &longCstTypeElt;}
    else errExp(NoErrMsg, NULL, NULL);
  else if (cExp.type != NULL) {  /* compute result type (in case !(Bool | x),
								for example). */
    TpcTypeElt resType;

    lExp.type = cExp.type;
    lExp.Einfo = cExp.Einfo;
    resType = commonType(msk);
    if (resType != cExp.type) {FreeExpType(cExp); cExp.type = resType;}}
  cExp.lValFl = False;
  cExp.ptrId = NULL;
}

static size_t sizeOfTypeI(TpcTypeElt type, bool deref, Terr err, Tstring y)
{
  TpcTypeElt tr = (deref)? NxtTypElt(type) : type, t1 = tr;

  if (tr == NULL) return 0;
  if (IsStrun(tr)) tr = BaseStrunType(tr);
  if (tr->size==0 && !tr->errSiz) {
    errId3(err, &cExp, typeToS1(t1), y);
    if (deref && tr==&natTyp[_VOID]) ((ModifType)represType(type))->nextTE =
					      tr = allocTypeEltIC(tr, NoQualif);
    if (tr != &natTyp[_VOID]) ((ModifType)tr)->errSiz = True;}
  return tr->size;
}

static void transfOpnd(const TresulExp *x /*~MayModify*/, TresulExp *y)
{
  TREInfo savCEinfo;

  savCEinfo._eInfo = y->Einfo;
  y->Einfo = x->Einfo;
  y->hist = x->hist;
  if (savCEinfo.s21._litCst && (!ifDirExp || x==&cExp)) {
    y->litCst = True;
    if (savCEinfo.s21._litCstOutsMac) y->LitCstOutsMac = True;}
  y->unsigDiff = savCEinfo.s21._unsigDiff;
#if PtrGtLong
  y->pVal = x->pVal;
#else
  y->uVal = x->uVal;
#endif
  y->ptrId = x->ptrId;
  if (y->type == NULL) y->type = x->type;
}

static Tstring typeToS2NoQual(TpcTypeElt x)
{
  Tstring result;
  TpTypeElt x1;

  if (x == NULL) return NULL;
  x1 = allocTypeEltIC(x, NoQualif);
  x1->qualif = NoQualif;
  result = typeToS2(x1);
  (void)freeTypeElt(x1);
  return result;
}

static void valueNotUsed(void)
{
  register TsemanElt *rPtrId;

  if (!cExp.PointedByObj && (rPtrId = cExp.ptrId)!=NULL && !(rPtrId->kind==
	Param && (rPtrId->type==NULL || IsArr(rPtrId->type))) && !zifExp) rPtrId
	->used = cExp.oldUsed; /* side-effect is not use (except for externals,
				       global statics, and array parameters). */
  cleanExprThings();
}

static void verifCstOvfl(void)
{
  if (lExp.rEvlbl) {
    TcalcS savCexpVal = cExp.sVal;
    ulong savCexpFlg = cExp.Einfo;

    cExp.sVal = lExp.sVal;
    cExp.Einfo = lExp.Einfo;
    checkInfoLoss(lExp.type);
    cExp.sVal = savCexpVal;
    cExp.Einfo = savCexpFlg;}
  if (cExp.rEvlbl) checkInfoLoss(cExp.type);
}

static void warnCstBool(Tstring x)
{
  if (chkBool && !cstExpG && !ReallyInsideMacro) errWS(CstBoolExp | CWarn2, x);
}

static void warnNotPureBool(TresulExp *x, Tstring y)
{
  if (x->notPureBool && !x->inhibWaNPB && (x->ptrId==NULL || !x->ptrId->
	pureBoolAskd /* to minimize nb of warnings */)) {errWSS(Outside01|
			   Warn2|PossErr, paramTxt, y); cExp.inhibWaNPB = True;}
  if (x->ptrId != NULL) x->ptrId->pureBoolAskd = True;  /* limits also nb of
							      error messages. */
}

static Terr warnOrErr(Terr n)
{
  return (compatType(lExp.type, cExp.type, CCheck) || lExp.type->typeSort==Ptr
       && IsTypeSort(cExp.type, WhoEnumBool) && cExp.rEvlbl && !cExp.errEvl &&
						   cExp.sVal==0)? n | Warn2 : n;
}

/* End DCEXP.C */
