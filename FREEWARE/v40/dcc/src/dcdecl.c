/* DCDECL.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#pragma noinline (errMsngStati, getINDEXTYPEtype)
#pragma noinline (cleverSkipTok)
#endif

#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>	/* memcmp, strchr used */
#include "dcdecl.h"
#include "dcdecl.ph"
#include "dcblk.h"
#include "dcdir.h"
#include "dcexp.h"
#include "dcext.h"
#include "dcinst.h"
#include "dcmain.h"
#include "dcprag.h"
#include "dcrec.h"
#include "dctxttok.h"

/* Miscelleanous */
#define BoolHCode	0x181C		/* to be changed if hCodFct changes */
#define BoolTName	(Tname)"\4\0bool"
#define InfoDcl(wName, xmemberFl, ydeclFl, znoNewTypeFl)		       \
  static TinfoDecl wName = {  /* 'static' to avoid local compiler	       \
							 gross inefficiency. */\
    0-0		   /*~ zif (! __member(_fldSize))			       \
			    "misplaced initialization for field '_fldSize'" */,\
    NoAttrib+0-0   /*~ zif (! __member(_attriba))			       \
			    "misplaced initialization for field '_attriba'" */,\
    NoParQal+0-0   /*~ zif (! __member(_parQal))			       \
			     "misplaced initialization for field '_parQal'" */,\
    (bool)(0-0)	   /*~ zif (! __member(_fldFl))				       \
			      "misplaced initialization for field '_fldFl'" */,\
    xmemberFl	   /*~ zif (! __member(_memberFl))			       \
			   "misplaced initialization for field '_memberFl'" */,\
    ydeclFl	   /*~ zif (! __member(_declFl))			       \
			     "misplaced initialization for field '_declFl'" */,\
    znoNewTypeFl   /*~ zif (! __member(_noNewTypeFl))			       \
			"misplaced initialization for field '_noNewTypeFl'" */,\
    (bool)(0-0)	   /*~ zif (! __member(_signedInt))			       \
			  "misplaced initialization for field '_signedInt'" */,\
    (bool)(0-0)	   /*~ zif (! __member(_variousD))			       \
			   "misplaced initialization for field '_variousD'" */,\
    (bool)(0-0)||False /*~ zif (! __member(_variousD1))			       \
			  "misplaced initialization for field '_variousD1'" */,\
    /*~ zif ! __member()						       \
		"Structure '" ConvToString(wName) "' not fully initialized" */ \
  }
#define MainHCode	0x1C01		/* to be changed if hCodFct changes */
#define OutsideStrunInit QuasiNULLval(TdeclList)
#define Sgnd		(Tqualif)1
#define StartTagCh	'#'
#define VerySpclRule	QuasiNULLval(TdeclList)

DefRepresType  /* define represType function */
DefIsSameName  /* define isSameName function */

/* Type definitions */
typedef struct {
  Tname idName;
  ThCode hCode;
  TpcTypeElt headType, tailType;
} TcreatType;

struct _TtagVisi;
typedef struct _TtagVisi TtagVisi;
struct _TtagVisi {
  int brLvl;
  TsemanElt *pTagId;
  TtagVisi *preced;
};

/* Function profiles */
static TpTypeElt allocTypeElt(void);
static void accumulateQal(TpcTypeElt, TpcTypeElt), chkEndAccumQal(TpcTypeElt,
  TpcTypeElt), checkWrngHdrFile(const TsemanElt *), cleverSkipTok(void), 
  computeSize(TpTypeElt), declObj(TdeclList), defFunc(TdeclList), defStrun(
  TsemanElt *, TtypeSort, Tname), errMsngStati(Tname), freeDeclList(TdeclList),
  init(TpTypeElt, bool, bool, TmacExpnd), initCreateTagName(void), initOrSizFld(
  TdeclList, TsemanElt *), makeGeneric(TpTypeElt), manageFctDPrags(const
  TdeclElt *), nbBits(TdeclList);
static Tattrib attrib(void);
static TdeclList allocDeclElt(void), createDLElt(TcreatType /*~MayModify*/,
  TinfoDecl, TpcTypeElt, Tqualif), decl1(TinfoDecl), decl3(TdeclList,
  TpcTypeElt, Tqualif), freeDeclElt(TdeclList);
static Tqualif arrQal(TpcTypeElt, Tqualif), typeQualif(void);
static TcreatType declId(void);
static TsemanElt *enterEnumTagName(Tname, TinfoSeman), *errAlrdDefId(Tname,
  TpcTypeElt, Tkind);
static TtagVisi *allocTagListElt(void), *freeTagListElt(TtagVisi *);
static Tname createTagName(Tname);
static TpcTypeElt nativeType(void);
static bool compatTypeR(TpcTypeElt, TpcTypeElt, TkTypeEquiv), rowUp(TpcTypeElt,
  TpcTypeElt), sameQal(TpcTypeElt, TpcTypeElt);

/* Objects declarations */
static int braceLvl;		/* +/- 1 at each strun opening/closing brace
								inside strun. */
static TdeclList curInitMember = OutsideStrunInit;
static size_t curInitArrayIndex = ArrLimErr;
static TpcTypeElt curInitArrayIndexType = NULL;
static Tstring fileAtBegDecl;
static TlineNb lineAtBegDecl;
static bool mainFl;
static bool msngAttribFl;
InfoDcl(noNewTypeInfo, False/*memberFl*/, False /*declFl*/, True
							       /*noNewTypeFl*/);
static TtagVisi *pTagList;  /* head of list of strun to be rendered invisible */
static bool signSpecSeen;
static const TtypeElt valInitTypeElt;  /* static => initialized to 0 =>
					 all fields either False, NULL, or 0. */
static const Ttok rBraSCol[] = {RBRA, SCOL, EndSTok};
static const Ttok sColLBra[] = {SCOL, LBRA, EndSTok};
static const Ttok sColRBra[] = {SCOL, RBRA, EndSTok};
static const Ttok zRbraSCol[] = {NoSwallowTok, RBRA, SCOL, EndSTok};
static const Ttok zCommaRBraSCol[] = {NoSwallowTok, COMMA, RBRA, SCOL, EndSTok};
static const Ttok zCommaRParSCol[] = {NoSwallowTok, COMMA, RPAR, SCOL, EndSTok};
static const Ttok zCoScEdpRPRB[] = {NoSwallowTok, COMMA, SCOL, ENDDPRAG, RPAR,
								 RBRA, EndSTok};
/* External objects */
bool heedRootType = True;

void prog(void)
{
  jmp_buf mainErrRet;

  curErrRet = &mainErrRet /*~ LocalAdr */;
  if (setjmp(*curErrRet) != 0) {	/* return point in 'panic' mode */
    skipTok(sColLBra);
    if (curTok.tok == ENDPROG) return;}
  else {GetNxtTok(); if (Found(DOLLSIGN)) dollAlwdInId = True;}
  pTagList = NULL;
  braceLvl = 0;
  do {
    decl();
  } while (curTok.tok != ENDPROG);
}

void decl(void)
{
  TdeclList declList;
  Tattrib curAttrib;
  InfoDcl(attribAndFalseFl, False /*memberFl*/, True /*declFl*/,
							 False /*noNewTypeFl*/);

  checkIndent();
  fileAtBegDecl = curFileName; lineAtBegDecl = lineNb;
  attribAndFalseFl._attriba = curAttrib = attrib();
  declList = decl1(attribAndFalseFl);
  if (declList != NULL) {	/* not function definition */
    if (declList != VerySpclRule) {
      while ((declList = freeDeclElt(declList)) != NULL) {}
      if (headerFile) {if (curAttrib!=NoAttrib /* error detected in declObj() */
         && !InsideInterval(curAttrib, Extrn, Typdf)) err0(ExtrnExptd | Warn2);}
      else if (curAttrib==Extrn) err0(ExtDeclBeInHdrFile|Warn1|Rdbl);}
    if (ignoreErr = False, !Found(SCOL)) {
      errWFName(SColExptd1, lineAtBegDecl, fileAtBegDecl, NULL, errTxt[
	       CmplSColE1], (fileAtBegDecl == curFileName)? errTxt[At0] : NULL);
      errPanic(NoErrMsg, NULL);}}
}

static Tattrib attrib(void)
{
  Tattrib w;

  w = (Tattrib) (curTok.tok - BAttrib);
  if ((uint)w <= (uint)(EAttrib - 1 - BAttrib)) {  /* if found legal attribute,
		 absorb token, and absorb any other (useless) 'attrib' token. */
    while (InsideInterval(NxtTok(), BAttrib, EAttrib - 1)) err0(
							     MoreThanOneAttrib);
    w += StatiL;
    if (w==Stati && nestLvl!=0) w = StatiL;}
  else w = NoAttrib;
  return w;
}

static Tqualif typeQualif(void)
{
  Tqualif curQual, res = NoQualif;

  for (;;) {
    switch (curTok.tok) {
    case CONST: curQual = ConstQal; break;
    case VOLAT: curQual = VolatQal; break;
    default: return res;}
    if (res & curQual) err0(AlrdQual);
    res |= curQual;
    GetNxtTok();}
}

static TdeclList decl1(TinfoDecl infoDcl)
/* May return VerySpecialRule or NULL (declaration of tag alone, or definition
   of function). */
{
  Tname tagName = NULL;
  TpcTypeElt type;
  Tqualif qual;
  TtypeSort typS;
  TsemanElt *sPtrTagId = NULL;

  qual = typeQualif();
  if ((qual & VolatQal)!=NoQualif && infoDcl._attriba==Reg) err0(
						 RegVolatIncptbl|Warn2|PossErr);
  if (InsideInterval(curTok.tok, BNatTyp, SGND)) {
    type = nativeType();
    if (type->typeSort == Int) infoDcl._signedInt = signSpecSeen;}
  else
    switch (curTok.tok) {
    case COLON:		/* in case of bit-field padding */
      type = &natTyp[_UINT];
      break;
    case ENUM: {
        TsemanElt *ptrTagId;

        if (NxtTok() == IDENT) {
          tagName = storeName(curTok.name, TagSpace);
          GetNxtTok();}
        if (curTok.tok == LBRA) {
          volatile TcalcS enumCtr = -1;  /* value of last enum constant;
					      'volatile' because of setjmp(). */
          TsemanElt *volatile pLastEnumCst = NULL, *volatile pIdEnumCst =
						      QuasiNULLval(TsemanElt *);
          volatile bool firstTurn;
          jmp_buf localJmpBuf, *savErrRet = curErrRet;
          TpTypeElt enumCstType;
          DefSem(defndEnumTag, EnumTag, True /*defnd*/);

          /* It is a new enum, so declare it, that is declare its enum tag */
          if (infoDcl._noNewTypeFl) err0(StrunEnumDeclNotAlwd);
          ptrTagId = enterEnumTagName(tagName, defndEnumTag);
          /* Create a type element for associated enum constants */
          enumCstType = allocTypeEltIC(ptrTagId->type, NoQualif);
          enumCstType->shared = True;  /* owned by EnumTag */
          enumCstType->generic = True;  /* for enum constants to be usable by
							       derived types. */
          GetNxtTok();	/* here for good error position */
          /* Process enum constants list */
          curErrRet = &localJmpBuf /*~ LocalAdr */;
          if (setjmp(*curErrRet) == 0) firstTurn = True;
          else skipTok(zCommaRBraSCol);
          while (firstTurn || Found(COMMA)) {
            Tname enumCName;
            ThCode enumCHCode;
            bool initByPrevECst = False;

            ignoreErr = firstTurn = False;
            if (curTok.tok != IDENT) errPanic(IdExptd, NULL);
            enumCName = storeName(curTok.name, ObjectSpace);
            enumCHCode = curTok.hCod;
            if (NxtTok() == ASGNOP) {	/* if initialized constant */
              TkAsgn oper;

              if ((oper = (TkAsgn)curTok.val) != SimplAsgn) err0(
								SimplAsgnExptd);
              GetNxtTok();
              lExp.ptrId = NULL;  /* for managePointers()/modifPtdVal() */
              if (correctExprN(enumCstType, Int, True, txtAsgnTok[oper],
								       False)) {
                enumCtr = cExp.sVal;
                if (cExp.ptrId == pIdEnumCst) initByPrevECst = True;
                goto initlzEnumCstL;}}
            if (! computeSigAdd(enumCtr, 1, (TcalcS */*~OddCast*/)&enumCtr)
#if ! NO_GREATER_INT
				   || enumCtr>INT_MAXC
#endif
						      ) err0(Overflow1 | Warn2);
initlzEnumCstL:
            if ((pIdEnumCst = enterSymTabHC(enumCName, enumCHCode))==NULL &&
		   (pIdEnumCst = errAlrdDefId(enumCName, NULL, EnumCst))==NULL
					       ) pIdEnumCst = enterSymTab(NULL);
            {
              /* Chain enum constants */
              register TsemanElt *w = pLastEnumCst, *prevW = NULL;
              TenumCst iEnumCtr = (TenumCst)enumCtr;

              while (w!=NULL && iEnumCtr<w->enumVal) {prevW = w; w = w->
								    nxtEnumCst;}
              if (initByPrevECst) (void)Found(SAMEVAL);
              else if ((w!=NULL && iEnumCtr==w->enumVal) != Found(SAMEVAL)) {
                Tchar *pBuf;

                if (w!=NULL && iEnumCtr==w->enumVal) {
                  /* Search name of (last) same-valued enum constant */
                  TsemanElt *pId;
                  TnameBuf buf;

                  (void)initGetNxtIdInCurBlk();
                  while (pId = nxtId(), pId->kind!=EnumCst || pId->enumVal!=
								    iEnumCtr) {}
                  bufNameToS(curIdName(), buf);
                  pBuf = &buf[0];}
                else pBuf = NULL;
                errWNSS((pBuf == NULL)? WrngSameVal|Warn1 : SameEnumCst|Warn2|
		    PossErr, enumCName, longToS((TgreatestInt)iEnumCtr), pBuf);}
              pIdEnumCst->nxtEnumCst = w;
              if (prevW == NULL) pLastEnumCst = pIdEnumCst; else prevW->
							nxtEnumCst = pIdEnumCst;
              pIdEnumCst->enumVal = iEnumCtr;}
            {
              DefSem(defndEnumCst, EnumCst, True /*defnd*/);
              DefSem1(notUsedEnumCst, EnumCst, False /*defnd*/, True /*used*/,
							      False /*initlz*/);

#ifdef _AIX
              if (Found(NOTUSED)) pIdEnumCst->infoS = notUsedEnumCst;
              else pIdEnumCst->infoS = defndEnumCst;
#else
              pIdEnumCst->infoS = (Found(NOTUSED))? notUsedEnumCst :
								   defndEnumCst;
#endif
            }
            if (headerFile) pIdEnumCst->attribb = Extrn;
            pIdEnumCst->type = enumCstType;
            pIdEnumCst->defLineNb = lineNb;         /* record              */
            pIdEnumCst->defFileName = curFileName;} /*        birth place. */
          curErrRet = savErrRet;
          ((ModifType)ptrTagId->type)->frstEnumCst = pLastEnumCst;
          if (pLastEnumCst == NULL) {  /* no enum constant found */
            enumCstType->shared = False;
            (void)freeTypeElt(enumCstType);}
          else ((ModifType)enumCstType)->frstEnumCst = pLastEnumCst;
          if (sysAdjHdrFile && tagName==NULL && infoDcl._attriba!=Typdf) ((
		ModifType)ptrTagId->type)->intPoss = True;  /* 'enum' cst
							     usable as 'int'. */
          ptrTagId->listAlwdFiles = headListPrivTo;
          if (! Found(RBRA)) {err0(RBraOrCommaExptd); cleverSkipTok();}}
        else {	/* no left brace seen ; is tag already defined ? */
          DefSem(notDefndEnumTag, EnumTag, False /*defnd*/);

          if (tagName == NULL) err0(UndefEnum);
          else if ((ptrTagId = searchSymTab(tagName)) != NULL) {
            if (ptrTagId->kind == EnumTag) goto foundEnumTagL;}
          else errWN(NotDefTag, tagName);
          ptrTagId = enterEnumTagName(tagName, notDefndEnumTag);  /* if
					       unknown enumTag, declare it... */
foundEnumTagL:
          ptrTagId->used = True;
          tagName = NULL;}	/* enforce that variables has to be declared
				   (NULLing tagName can be done because name
				  storage freeing only occurs at block exit). */
        type = ptrTagId->type;
        sPtrTagId = ptrTagId;
        break;}
    case IDENT:
      if (adjustFile && isSameName(curTok.hCod, curTok.name, BoolHCode,
			   BoolTName)) type = &boolCstTypeElt;  /* => generic */
      else {
        TsemanElt *ptrId = curTok.ptrSem;

        if (ptrId==NULL || ptrId->kind!=Type) {
          TcharTok peep = nxtCharOrMacTok();

          if ((peep < 0)
		? peep==-(TcharTok)IDENT || peep==-(TcharTok)STAR ||
				    infoDcl._prioToCast && peep==-(TcharTok)RPAR
		: isAlfa((char)peep) || peep==(TcharTok)'*' || infoDcl.
			_prioToCast && peep==(TcharTok)')') {  /* should be
							     type identifier. */
            if (ptrId == NULL) {
              if ((ptrId = searchSymTab(storeName(curTok.name, TagSpace) /* do
			not modify curTok.name, which may be in macro storage,
			and so, remanent */)) != NULL) {errWN(TagQM, curTok.
						     name); ptrId->used = True;}
              else {
                DefSem1(artifDef, Type, True /*defnd*/, True /*used*/,
							      False /*initlz*/);

                errWN(UndefId, curTok.name);
                if (!chkTypName || isupper((char)*(curTok.name + LgtHdrId))
		      || (char)*(curTok.name + LgtHdrId)=='_') (void)defineId(
								    artifDef);}}
            else if (ptrId->type != NULL) errWN(NotTypeId, curTok.name);
            type = NULL;}
          else goto forceIntTypeL;}
        else {
          type = ptrId->namedType;
          ptrId->used = True;}}
      GetNxtTok();
      break;
    case LBRA:
      if (qual==NoQualif && infoDcl._declFl && infoDcl._attriba==NoAttrib) {
        type = &natTyp[_INT];  /* to handle 'cleanly'			    */
        break;}		       /*  blocks declared (wrongly) at level zero. */
      err0(MsngStrunEnum);
      typS = Struc;  /* could be Union (or Enum); just to have dcc behave the
						    less stupidly possible... */
      goto strunTrmt;
    case STRUC: GetNxtTok(); typS = Struc; goto strunTrmt;
    case UNION: GetNxtTok(); typS = Union;
strunTrmt: {
        TsemanElt *ptrTagId;
        volatile TdeclList membList = NULL, lastEltMembList;
        jmp_buf localJmpBuf, *savErrRet = curErrRet;
        InfoDcl(memberInfo, True /*memberFl*/, False /*declFl*/,
							 False /*noNewTypeFl*/);

        if (curTok.tok == IDENT) {
          tagName = storeName(curTok.name, TagSpace);
          GetNxtTok();}
        if (curTok.tok == LBRA) {/* if left brace seen, definitely a new type */
          if (infoDcl._noNewTypeFl) err0(StrunEnumDeclNotAlwd);
          braceLvl++;
          if ((ptrTagId = enterSymTab(tagName)) == NULL) {  /* already
					       defined tag; is it compatible? */
            ptrTagId = searchSymTab(tagName);
            if (ptrTagId->type->typeSort == typS) {
              if (! ptrTagId->defnd) {
                /* Declared but not defined ('Very Special Rule') */
                ptrTagId->defLineNb = lineNb;         /* record              */
                ptrTagId->defFileName = curFileName;  /*        birth place. */
                goto commonStrun;}
              if (adjustFile) {
                freeTypes(ptrTagId);
                tagName = ptrTagId->nameb;
                goto commonStrun1;}}
            if (errAlrdDefId(tagName, NULL, StrunTag) == NULL) ptrTagId =
						  enterSymTab(NULL);} /* stub */
          /* Yet undefined tag, or stub */
commonStrun1:
          defStrun(ptrTagId, typS, tagName);  /* uncomplete strun as yet */
commonStrun:
          type = ptrTagId->type;
          GetNxtTok();	/* here for good error position */
          /* Explore member list */
          curErrRet = &localJmpBuf /*~ LocalAdr */;
          if (setjmp(*curErrRet) != 0) skipTok(sColRBra);
          while (curTok.tok!=RBRA && curTok.tok!=ENDPROG) {
            TdeclList newMembList, w, w1;

            memberInfo._attriba = attrib();
            newMembList = decl1(memberInfo);
            if (ignoreErr = False, curTok.tok != SCOL) err0(SColExptd);
            if (memberInfo._attriba != NoAttrib) err0(NoAttribAlwd);  /* checked
					here for deceitless error position... */
            if (newMembList == NULL) err0(FctDeclDefNotAlwd);
            else if (newMembList != VerySpclRule) {
              /* Accumulate const/volatile qualifiers */
              if (newMembList->type != NULL) {
                ((ModifType)ptrTagId->type)->SynthQualif |= newMembList->
								   type->qualif;
                if (IsStrun(newMembList->type)) ((ModifType)ptrTagId->type)->
			     SynthQualif |= BaseStrunType(newMembList->type)->
								   SynthQualif;}
              /* Check that there are no double definition of member names, and
                 that members are allowed and of complete type; compute
                 qualifiers. */
              for (w = newMembList; w != NULL; ) {
                if (w->type!=NULL && IsFct(w->type)) err0(FctDeclDefNotAlwd);
                if (w->idName != NULL) {  /* not unnamed field (for non-fields,
				       naming is checked in 'initOrSizeFld'). */
                  for (w1 = membList; w1 != NULL; w1 = w1->cdr) {
                    if (isSameName(w1->hCode, w1->idName, w->hCode, w->idName)){
                      errWN(DblDefMember, w->idName);
                      break;}}}
                /* Insert new member into member list */
                if (membList == NULL) membList = w;
                else lastEltMembList->cdr = w;
                lastEltMembList = w;
                w = w->cdr;  /* to be left there (because of next statement) */
                lastEltMembList->cdr = NULL;}}
            if (ignoreErr = False, !Found(SCOL)) errPanic(NoErrMsg, NULL);}
          curErrRet = savErrRet;
          /* Define corresponding baseType (no more incomplete) */
          if (ptrTagId->defnd) errId1(DblDefStrun, ptrTagId, NULL);  /* cannot
				       be checked before, because the tag may
				       be defined by a member of the strun... */
          ((ModifType)type)->memberList = membList;
          if (membList == NULL) err0(EmptyStrun); 
          else {
            /* Compute size of strun (may be a minorand, because of alignment) */
            size_t size = 0;
            bool fldBefore = False;

            do {
              size_t membSize;

              if (membList->fldFl) {  /* field */
                if ((membSize = (size_t)membList->fldSize) == 0) size = ((size
			+ (CHAR_BITC - 1))/CHAR_BITC)*CHAR_BITC;  /* round to
								   next byte. */
                else fldBefore = True;}
              else {
                if (fldBefore) {
                  size = ((size + (CHAR_BITC - 1))/CHAR_BITC)*CHAR_BITC;  /*
							  round to next byte. */
                  fldBefore = False;}
                membSize = sizeOfTypeD(membList->type, IncplType,
						 membList->idName) * CHAR_BITC;}
              if (typS == Struc) size += membSize;
              else if (membSize > size) size = membSize;
              membList = membList->cdr;
            } while (membList != NULL);
            ((ModifType)type)->size = (size + (CHAR_BITC - 1)) / CHAR_BITC;}
          ((ModifType)type)->errSiz = True;  /* in case size = 0 (no correct
							      field type...). */
          ptrTagId->defnd = True;
          /* Search if there are tags to be marked 'invisible' (from outside
             this declaration). */
          while ((pTagList != NULL) && (pTagList->brLvl == braceLvl)) {
            pTagList->pTagId->notVisible = True;
            pTagList = freeTagListElt(pTagList);}
          if (--braceLvl > 0) {
            TtagVisi *w = allocTagListElt();

            w->brLvl = braceLvl;
            w->pTagId = ptrTagId;
            w->preced = pTagList;
            pTagList = w;}
          ptrTagId->listAlwdFiles = headListPrivTo;
          GetNxtTok();}  /* here for good error position */
        else {	/* no left brace seen */
          if (tagName == NULL) errPanic(UndefStrun, NULL);
          type = NULL;}  /* to be able to cope with the 'Very Special Rule' */
        break;}
    default:
      if (nestLvl==0 && curTok.tok==RBRA) {curTok.tok = SCOL; errPanic(
							      ExtraRBra, NULL);}
      if (qual==NoQualif && infoDcl._attriba==NoAttrib && infoDcl._declFl
	 ) errPanic((cUnitFName==NULL && curTok.tok==ENDPROG)? NoErrMsg :
							       DeclExptd, NULL);
forceIntTypeL:
      err0((qual!=NoQualif || infoDcl._attriba!=NoAttrib || infoDcl._declFl /*
		because function definition may have an empty baseType
		       and no qualif/attrib. */)
		? TypeExptd|Warn1|Rdbl
		: TypeExptd);
      type = &natTyp[_INT];}
  {
    Tqualif postQal = typeQualif();

    if (qual & postQal) err0(AlrdQual);
    else if (qual!=NoQualif && postQal!=NoQualif) err0(GroupQual|Warn1|Rdbl);
    qual |= postQal;
  }
  if (qual!=NoQualif && type!=NULL && IsFct(type)) errWT(UslFctQual|CWarn1|Rdbl,
									  type);
  {
    TcreatType firstId = declId();

    if (tagName != NULL) {  /* tag exists => Union/Struct/Enum */
      if (type == NULL) {
        /* structure or union ('strun') tag alone (no 'strun-decl-list') */
        TsemanElt *ptrTagId;

        if (firstId.idName==NULL && firstId.headType==NULL && !infoDcl.
	    _noNewTypeFl) {  /* no declaration 	of object => 'very special ru-
	      le' (K&R p213) : declaration of incompletely specified 'strun'. */
          TpcTypeElt w;

          if (infoDcl._attriba != NoAttrib) err0(NoAttribAlwd);
          if (qual != NoQualif) err0(NoQualifAlwd);
          if (   (ptrTagId = enterSymTab(tagName))!=NULL
              || (ptrTagId = errAlrdDefId(tagName, ((w = searchSymTab(tagName)
								->type) != NULL)
		 ? (w->typeSort == typS)
		   ? QuasiNULLval(TpcTypeElt)
		   : NULL
		  : NULL, StrunTag))!=NULL) defStrun(ptrTagId, typS, tagName);
          return VerySpclRule;}
        /* Utilization of strun (at least one object is declared,
							 or noNewType True). */
        if ((ptrTagId = searchSymTab(tagName)) != NULL) {
          if (ptrTagId->type->typeSort == typS) {sPtrTagId = ptrTagId; goto
								 foundStrunTag;}
          if (errAlrdDefId(tagName, NULL, StrunTag) != NULL) goto defStrunTag;
          tagName = NULL;}	/* stub */
        else if (infoDcl._noNewTypeFl) {err0(StrunEnumDeclNotAlwd); goto
								     ignoreTag;}
        ptrTagId = enterSymTab(tagName);
defStrunTag:
        defStrun(ptrTagId, typS, tagName);  /* if unknown strunTag, declare
									it... */
foundStrunTag:
        type = ptrTagId->type;
        ptrTagId->used = True;
        if (ptrTagId->notVisible) errWN(TagNotVisi|Warn1|Rdbl, tagName);
ignoreTag:;}
      else {	/* there exist a strun-decl-list */
        if (firstId.idName==NULL  /* no identifier declared */
            && infoDcl._attriba!=Typdf  /* not a  typedef */
            && firstId.headType==NULL	/* no type modifier seen */
            && !infoDcl._memberFl	/* not a strun member */
            && !infoDcl._noNewTypeFl) {
          /* Then declaration of a strun/enum tag alone */
          if (infoDcl._attriba != NoAttrib) err0(UslAttrib|CWarn1|Rdbl);
          if (ignoreErr = False, !Found(SCOL)) errWFName(IdOrSColExptd,
		    lineAtBegDecl, fileAtBegDecl, NULL, errTxt[CmplSColE1],
			    (fileAtBegDecl == curFileName)? errTxt[At0] : NULL);
          return NULL;}}}
    if (sPtrTagId!=NULL && sPtrTagId->undef) errWN(UndfndId|Warn1|Rdbl,
							      sPtrTagId->nameb);
    {
      TdeclList dList = createDLElt(firstId, infoDcl, type, qual);

      return (infoDcl._noNewTypeFl)? dList : decl3(dList, type, qual);}}
}

static TpcTypeElt nativeType(void)
{
  TpcTypeElt resulType;
  int codNatTyp;

  signSpecSeen = False;
  if (curTok.tok >= USGND) {  /* 'unsigned'/'signed' seen */
    signSpecSeen = True;
    codNatTyp = (curTok.tok == USGND)? DeltaTyp : 0;
    if (! InsideInterval(NxtTok(), CHAR, LONG)) {
      err0(BadIntgrlType|Warn2|PossErr);
      codNatTyp += _INT;}
    else {codNatTyp += curTok.tok - BNatTyp; GetNxtTok();}
#ifdef LONGLONG
    if (curTok.tok == LONG) {codNatTyp++; GetNxtTok();}
#endif
  }
  else {
    codNatTyp = (curTok.tok == CHAR)? _CHAR : curTok.tok - BNatTyp;
    GetNxtTok();
    if (codNatTyp == _LONG) {
      if (curTok.tok == DOUBLE) {codNatTyp = _LONGDBL; GetNxtTok();}
#ifdef LONGLONG
      else if (curTok.tok == LONG) {codNatTyp = _LLONG; GetNxtTok();}
#endif
    }}
  if ((resulType = &natTyp[codNatTyp])->typeSort & (Short | UShort | Long |
	  ULong | LLong | ULLong) && curTok.tok==INT) GetNxtTok();  /* short/
								   long int'. */
  while (InsideInterval(curTok.tok, BNatTyp, SGND)) {err0(UslTypSpe)
								 ; GetNxtTok();}
  return resulType;
}

static void defStrun(TsemanElt *ptrTagId, TtypeSort typS, Tname tagName)
{
  TpTypeElt newElt;
  DefSem(notDefndStrunTag, StrunTag, False /*defnd*/);

  ptrTagId->type = newElt = allocTypeEltID(typS,NoQualif);
  newElt->shared = True;   /* the tag is the only owner of this type element */
  newElt->tagId = ptrTagId;  /* remember tag */
  newElt->algn = Alig3;  /* *doIt* compute alignment from members */
  /* newElt->memberList = NULL; done by initialization */ /* incomplete type */
  ptrTagId->infoS = notDefndStrunTag;
  if (headerFile) ptrTagId->attribb = Extrn;
  ptrTagId->listAlwdFiles = NULL;  /* a priori, not private */
  ptrTagId->nameb = createTagName(tagName);  /* remember nameString of strun */
  ptrTagId->defLineNb = lineNb;	   	/* record              */
  ptrTagId->defFileName = curFileName;  /*        birth place. */
}

static TsemanElt *enterEnumTagName(Tname tagName, TinfoSeman sem)
{
  TsemanElt *ptrTagId;
  TpTypeElt newElt;

  ptrTagId = enterSymTab(tagName);  /* named enum or anonymous stub (for
						    later automatic freeing). */
  if (ptrTagId == NULL) {  /* name already seen in this block */
    ptrTagId = searchSymTab(tagName);
    if (ptrTagId->kind==EnumTag && !ptrTagId->defnd) {ptrTagId->infoS = sem
							     ; return ptrTagId;}
    if (errAlrdDefId(tagName, NULL, EnumTag) == NULL) ptrTagId = enterSymTab(
						   tagName = NULL);}  /* stub */
  ptrTagId->infoS = sem;
  if (headerFile) ptrTagId->attribb = Extrn;
  ptrTagId->listAlwdFiles  = NULL;  /* a priori, not private */
  ptrTagId->defLineNb = lineNb;		/* record              */
  ptrTagId->defFileName = curFileName;	/*        birth place. */
  ptrTagId->nameb = createTagName(tagName);  /* remember nameString of enum */
  ptrTagId->type = newElt = allocTypeEltID(Enum, NoQualif);
  newElt->tagId = ptrTagId;  /* unique value for each enum */
  newElt->shared = True;  /* the tag is the only owner of this type element */ 
  newElt->size = IntSiz;
  newElt->algn = Alig0;  /* because standard says enum representation can be
								 char<->long. */
  return ptrTagId;
}

static TcreatType declId(void)
/* if no identifier declared, 'idName' field == NULL */
{
  TpcTypeElt pBefType = NULL, pEndBefType;
  TpTypeElt newElt;
  TcreatType curCreat;
  bool leftParenSeen;

  while (Found(STAR)) {		/* create 'before' part of type */
    newElt = allocTypeEltID(Ptr, typeQualif());
    newElt->size = PtrSiz;
    newElt->indexType = (curTok.tok == INDEXTYPE)? getINDEXTYPEtype() :
				(moreIndexTypeChk)? &defaultIndexTypeElt : NULL;
    if (pBefType == NULL) pEndBefType = newElt;
    newElt->nextTE = pBefType;
    pBefType = newElt;}
  leftParenSeen = False;
  if (Found(LPAR)) {
    if ((curTok.tok == IDENT)
        ? curTok.ptrSem!=NULL && curTok.ptrSem->kind==Type
        : curTok.tok!=STAR && curTok.tok!=LPAR) {
      curCreat.idName = NULL;
      curCreat.headType = NULL;
      leftParenSeen = True;}
    else {curCreat = declId(); if (! Found(RPAR)) err0(RParExptd);}}
  else {
    if (curTok.tok == IDENT) {
      curCreat.idName = storeName(curTok.name, ObjectSpace);
      curCreat.hCode = curTok.hCod;
      GetNxtTok();}
    else curCreat.idName = NULL;
    curCreat.headType = NULL;}
  for (;;) {
    if (leftParenSeen || Found(LPAR)) {	/* function declarator */
      TtypeSort sortFct = Fct;
      volatile TdeclList parList = NULL;

      mainFl = curCreat.idName!=NULL && isSameName(curCreat.hCode, curCreat.
					  idName, MainHCode, (Tname)"\4\0main");
      {
        volatile TdeclList endList;
        jmp_buf localJmpBuf, *savErrRet = curErrRet;
        volatile bool firstTurn;
        bool resulPtrSeen, sizeofBlSeen;

        if (curTok.tok!=RPAR && curTok.tok!=LBRA) {  /* not empty parameter list (LBRA
		   checked just to forgive the sinner that forgets the RPAR). */
          curErrRet = &localJmpBuf /*~ LocalAdr */;
          if (setjmp(*curErrRet) == 0) {firstTurn = True; resulPtrSeen =
							  sizeofBlSeen = False;}
          else skipTok(zCommaRParSCol);
          while (firstTurn || Found(COMMA)) {
            TdeclList newPar;
            Tattrib attrb;

            ignoreErr = False;
            if (Found(ELLIP)) {
              if (parList == NULL) err0(AtLeastOnePar);
              sortFct = VFct;
              break;}
            firstTurn = False;
            if ((attrb = attrib())!=NoAttrib && attrb!=Reg) {
              err0(OnlyRegAttrib);			
              attrb = NoAttrib;}
            noNewTypeInfo._attriba = attrb;
            noNewTypeInfo._prioToCast = False;
            newPar = decl1(noNewTypeInfo);
            if (attrb == NoAttrib) newPar->attriba = Auto;
            if (newPar->type!=NULL && IsFct(newPar->type)) errWNTT(
			     PtrOnFctExptd, newPar->idName, newPar->type, NULL);
            if (parList == NULL) {  /* first parameter */
              if (newPar->type!=NULL && newPar->type->typeSort==Void) {
                if (newPar->idName!=NULL || attrb!=NoAttrib || curTok.tok!=
						       RPAR) err0(VoidNotAlone);
                (void)freeDeclElt(newPar);}
              else parList = newPar;}
            else {
              if (newPar->type!=NULL && newPar->type->typeSort==Void) errWN(
						   VoidNotAlwd, newPar->idName);
              endList->cdr = newPar;}
            endList = newPar;
            {
              TpcTypeElt parType = newPar->type;

              newPar->SizeofBlFl = False;  /* other flags already off */
              for (;; GetNxtTok()) {
                switch (curTok.tok) {
                  TparQal paramQal;

                  case GENERIC:  paramQal = GenericV; goto commonParQalL;
                  case MAYMODIFY: {
                      TresulTstPtd w;

                      if (newPar->MayModifFl) break;
                      if ((w = tstPtd(parType, False)) & NOTALLCONST &&
			  ReallyInsideMacro) continue;  /* ignore d-pragma
			   in this case if function declaration inside macro. */
                      if (w & (NOTPTR | NOTALLCONST)) goto ilgDPl;
                      newPar->MayModifFl = True;
                      continue;}
                  case NOTUSED:
                    if (newPar->notUsedFl) break;
                    if (newPar->ResulPtrFl) goto ilgDPl;
                    newPar->notUsedFl = True;
                    continue;
                  case RESULPTR: 
                    if (parType!=NULL && !IsPtrArr(parType) || resulPtrSeen ||
						 newPar->notUsedFl) goto ilgDPl;
                    newPar->ResulPtrFl = True;
                    /*~NoBreak*/
                  case RESULTYP: paramQal = ResulTypV; goto commonParQalL;
                  case SIZEOFBL:
                    if (newPar->SizeofBlFl) break;
                    if (parType!=NULL && !(parType->typeSort & WhoEnum) ||
						      sizeofBlSeen) goto ilgDPl;
                    sizeofBlSeen = True;
                    newPar->SizeofBlFl = True;
                    continue;
                  case UTILITY:
                    if (parType!=NULL && !isRepreType(parType)) goto ilgDPl;
                    paramQal = UtilityV;
commonParQalL:
                    if (newPar->parQal == NoParQal) {newPar->parQal = paramQal
								    ; continue;}
                    else if (newPar->parQal == paramQal) break;
                    goto ilgDPl;
                  default:
                    if (chkNmdTypParFl && !sysAdjHdrFile && newPar->parQal==
			NoParQal && isRepreType(parType) && !mainFl) errWNT(
					   ParRepreType | Warn1, NULL, parType);
                    goto noMoreDPL;}
                errUslDP(curTok.tok);
                continue;
ilgDPl:
                errIlgDP(curTok.tok);}
noMoreDPL:
              if (newPar->ResulPtrFl) resulPtrSeen = True;}}}
        else if (chkEmptParList && !mainFl) err0(MsngVoid | Warn1);
        curErrRet = savErrRet;
      }
      newElt = allocTypeEltID(sortFct, NoQualif); /* Fct or VFct if variadic */
      newElt->paramList = parList;
      if (! Found(RPAR)) err0((parList == NULL)? RParExptd :RParOrCommaExptd);
      leftParenSeen = False;}
    else if (Found(LSBR)) {	/* array declarator */
      size_t limArr;

      limArr = (curTok.tok!=RSBR && curTok.tok!=INDEXTYPE)
                ? (correctExprN(NoFreeExpType, WhoEnumBool, True, "[]", False))
                  ? (cExp.sVal==0 || cExp.sVal<0 && cExp.type->typeSort &
									 SigWho)
                    ? (errWS((cExp.errEvl)? NoErrMsg : IlgArraySize, longToS(
							 cExp.sVal)), ArrLimErr)
                    : (size_t)cExp.uVal
                  : ArrLimErr
                : (cExp.type = &defaultIndexTypeElt,
						 0);  /* if no size specified */
      newElt = allocTypeEltID(Array, NoQualif);
      newElt->lim = limArr;
      if (limArr != 0) {
        newElt->size = 1U;  /* forbids computation of bound by
						    potential initialization. */
        if (cExp.litCst) {
          Tname nameToShow = curCreat.idName;
          TpcTypeElt w1 = curCreat.headType;

          while (w1 != NULL) {
            if (w1->typeSort != Array) {nameToShow = NULL; break;}
            w1 = w1->nextTE;}
          errWN((cExp.LitCstOutsMac)? BndShdBeNamed | Warn1 :
				   NumCstShdBeNamed1|Warn1|Rdbl , nameToShow);}}
      newElt->indexType = (curTok.tok == INDEXTYPE)
                            ? (FreeExpType(cExp), getINDEXTYPEtype())
                            : (cExp.cstImpsd)
                              ? (FreeExpType(cExp), &defaultIndexTypeElt)
                              : cExp.type;
      if (newElt->indexType!=NULL && newElt->indexType->noOwner) ((ModifType)
					    newElt->indexType)->noOwner = False;
      if (! Found(RSBR)) err0(RSBrExptd);}
    else break;  /* exit "for(;;)" */
    if (curCreat.headType == NULL) curCreat.headType = newElt;
    else ((ModifType)curCreat.tailType)->nextTE = newElt;
    curCreat.tailType = newElt;}
  /* splice eventual front end of type ('pointer' part) */
  if (pBefType != NULL) {
    if (curCreat.headType == NULL) curCreat.headType = pBefType;
    else ((ModifType)curCreat.tailType)->nextTE = pBefType;
    curCreat.tailType = pEndBefType;}
  return curCreat;
}

static TdeclList decl3(TdeclList x, TpcTypeElt baseType, Tqualif qual)
{
  TdeclList endDL;
  InfoDcl(localIDL, (bool)(0-0), (bool)0, (bool)0);

  manageFctDPrags(x);
  if (curTok.tok == LBRA) {defFunc(x); return NULL;}  /* function definition */
  msngAttribFl = False;  /* this flag to prevent several instances of same
								       error. */
  endDL = x;  /* end of declaration list = beginning as of now */
  localIDL._attriba = x->attriba;
  localIDL._memberFl = x->memberFl;
  for (;;) {
    if (nestLvl!=0 && endDL->attriba==NoAttrib && (endDL->type==NULL ||
				    !IsFct(endDL->type))) endDL->attriba = Auto;
    if (x->declFl) declObj(endDL);  /* if object to be declared (to be done
	       immediatly, because of case "struct _s a ={..., &a, ...}..."). */
    else {
      initOrSizFld(endDL, NULL);
      if (x->initFl) err0(MemberCantBeInit);}
    if (! Found(COMMA)) break;
    {
      TdeclList newDlElt = createDLElt(declId(), localIDL, baseType, qual);

      endDL->cdr = newDlElt;	/* chain current declaration */
      endDL = newDlElt;}
    manageFctDPrags(endDL);}
  return x;
}

static void defFunc(TdeclList x)
{
  TpcTypeElt defFctTyp, curFctRetTyp;
  Tname fctName;

  checkIndent();
  /* Search if type = (V)Fct; if not, try to find one in type chain */
  if ((defFctTyp = x->type) != NULL)
    do {
      if (IsFct(defFctTyp)) break;
    } while ((defFctTyp = freeTypeElt(defFctTyp)) != NULL);
  if (defFctTyp != x->type) err0(NotAFct);
  if (defFctTyp != NULL) {	/* function type found */
    TsemanElt *ptrIdFct;
    TdeclList defParList;
    Tattrib fctAttrib;
    bool keepDecl = False;  /* try hard not to, to get function definition coor-
			dinates (otherwise, problem to either spot declaration
			coordinates (eg. if bad type) or definition coordinates
			(eg. in case of double definition). */

    if (defFctTyp->shared) errWN(FctDefViaTpdf, x->idName);
    if ((fctAttrib = x->attriba) == Extrn) fctAttrib = NoAttrib;
    else if (fctAttrib==NoAttrib || sysHdrFile && fctAttrib==Stati) fctAttrib
									= Extrn;
    if (headerFile) errWS(NoDefInHdrFile|Warn1|Rdbl, NULL);
    if (nestLvl != 0) err0(EmbdFctNotAlwd);
    if ((fctName = x->idName) == NULL) err0(MsngFctName);
    if ((curFctRetTyp = NxtTypElt(defFctTyp))!=NULL && curFctRetTyp->
	     typeSort!=Void && !IsArrFct(curFctRetTyp)) (void)sizeOfTypeD(
					   curFctRetTyp, IncplRetType, fctName);
    if (fctName!=NULL && (ptrIdFct = searchSymTabHC(fctName, x->hCode))!=
		      NULL) {  /* already existing identifier; is it correct? */
      if (ptrIdFct->type != NULL) {
        if (IsFct(ptrIdFct->type) && !ptrIdFct->defnd) {
          /* Correct: now, check concordance */
          const TpcTypeElt declFctTyp = ptrIdFct->type, declRetType =
							  NxtTypElt(declFctTyp);
          TdeclList declParList;

          if (ptrIdFct->attribb != fctAttrib) errId1(IncptblAttrib|Warn2|
			    PossErr, ptrIdFct, (ptrIdFct->attribb == Extrn)?
						    errTxt[Empty] : "'static'");
          if (declFctTyp->typeSort != defFctTyp->typeSort) {errId1(VariNotVari,
					      ptrIdFct, NULL); keepDecl = True;}
          if (! CompatType(declRetType, curFctRetTyp, StrictChk)) {
            errId2((compatType(curFctRetTyp, declRetType, CStrictChk))?
		 NotSameType|Warn2|PossErr : NotSameType, ptrIdFct, typeToS1(
					  curFctRetTyp), typeToS2(declRetType));
            keepDecl = True;}
          for (declParList = declFctTyp->paramList,
					      defParList = defFctTyp->paramList;
               declParList!=NULL && defParList!=NULL; 
                 declParList = declParList->cdr, defParList = defParList->cdr) {
            if (defParList->idName!=NULL && declParList->idName!=NULL &&
		memcmp(declParList->idName, defParList->idName, FullLgt(
						      defParList->idName))!=0) {
              TnameBuf buf;

              bufNameToS(defParList->idName, buf);
              errId1(ParNamesNotEq|Warn1|Rdbl, ptrIdFct, buf);}
            if (! CompatType(declParList->type, defParList->type, StrictChk)) {
              TpcTypeElt declParType = declParList->type;

              errId((compatType(declParType, defParList->type, CStrictChk))?
		ParTypesNotEq|Warn2|PossErr : ParTypesNotEq, ptrIdFct,
		    defParList->idName, typeToS1(declParType), typeToS2(
							     defParList->type));
              declParList->type = defParList->type;/* permute types, so that  */
              defParList->type = declParType;}	   /* declaration type stays. */
            if (declParList->parQal!=defParList->parQal || declParList->
		MayModifFl!=defParList->MayModifFl || declParList->ResulPtrFl
						     !=defParList->ResulPtrFl) {
              errId1(NonCoheDPInFct | Warn2, ptrIdFct, NULL);
              defParList->parQal = declParList->parQal;
              defParList->MayModifFl = declParList->MayModifFl;
              defParList->ResulPtrFl = declParList->ResulPtrFl;}}
          if ((defParList!=NULL || declParList!=NULL) && (!sysHdrFile ||
		declFctTyp->paramList!=NULL && defFctTyp->paramList!=NULL) /*
		in system header files, empty parameter list => no check ? */) {
            errId1(NotSameNbOfPar, ptrIdFct, NULL);
            keepDecl = True;}
          if (defFctTyp->pvNr!=declFctTyp->pvNr || defFctTyp->generic!=
							  declFctTyp->generic) {
            errId1(NonCoheDPInFct | Warn2, ptrIdFct, NULL);
            ((ModifType)defFctTyp)->pvNr = declFctTyp->pvNr;
            ((ModifType)defFctTyp)->generic = declFctTyp->generic;}
          if (fctAttrib == Extrn) checkWrngHdrFile(ptrIdFct);
          ptrIdFct->defnd = True;
          if (! keepDecl) {
            freeTypeChain(declFctTyp);
            goto commonL;}}
        else (void)errAlrdDefId(fctName, defFctTyp, Obj);
        ptrIdFct = enterSymTab(NULL);}  /* stub (to manage correctly following
							      function body). */
      if (! mainFl) fctAttrib = Stati;}
    else {  /* yet undeclared identifier: declare it as a function */
      if (x->attriba > Stati) {errWN(IlgAttribForFct, fctName); fctAttrib =
									 Stati;}
      ptrIdFct = enterSymTabHC(fctName, x->hCode);}
    {
      DefSem1(defndObj, Obj, True /*defnd*/, False /*used*/, True /*initlz*/);

      ptrIdFct->infoS = defndObj;}
    ptrIdFct->nameb = fctName;
    if ((ptrIdFct->attribb = fctAttrib)==Extrn && !mainFl && !headerFile &&
					      nestLvl==0) errMsngStati(fctName);
commonL:
    ptrIdFct->defLineNb = lineNb;		/* record              */
    ptrIdFct->defFileName = curFileName;	/*        birth place. */
    ptrIdFct->type = defFctTyp;
    /* Check for 'main' special treatment */
    if (mainFl) {  /* verify that legal form */
      register TdeclList declParList = ptrIdFct->type->paramList;

      if (fctAttrib != Extrn) err0(BadAttribForMain);
      if (curFctRetTyp!=NULL && curFctRetTyp->typeSort!=Int) err0(
							     BadRetTypeForMain);
      if (declParList != NULL) {
        if (declParList->type!=NULL && !(declParList->type->typeSort & WhoEnum)
		) errWNT(BadParForMain, declParList->idName, declParList->type);
        if ((declParList = declParList->cdr)!=NULL && declParList->type!=NULL) {
          scndMainParTypeElt.indexType = declParList->type->indexType;  /* for
					 no false error due to type of bound. */
          if (! compatType(declParList->type, &scndMainParTypeElt, FctCall)
		) errWNT(BadParForMain, declParList->idName, declParList->type);
          if (declParList->cdr!=NULL && declParList->cdr->cdr!=NULL) err0(
							  TooManyParForMain);}}}
    /* Enter parameters into symbol table */
    enterBlock();	/* so parameters are defined at function body level */
    for (defParList = defFctTyp->paramList; defParList != NULL; defParList =
							     defParList->cdr) {
      TsemanElt *ptrId;
      TpcTypeElt curType = defParList->type;

      if (defParList->idName == NULL) err0(ParMustBeNamed);
      if (curType!=NULL && !IsArrFct(curType)) (void)sizeOfTypeD(curType,
						 IncplType, defParList->idName);
      if ((ptrId = enterSymTabHC(defParList->idName, defParList->hCode)) ==
				   NULL) errWN(SameParName, defParList->idName);
      else {
        DefSem1(defndPar, Param, True /*defnd*/, False /*used*/,
							       True /*initlz*/);
        DefSem1(notUsedPar, Param, False  /*defnd*/, True /*used*/, True
	       /*initlz*/);  /* defnd = False for 'NotUsdPar' error detection */

#ifdef _AIX
        if (defParList->notUsedFl) ptrId->infoS = notUsedPar;
        else ptrId->infoS = defndPar;
#else
        ptrId->infoS = (defParList->notUsedFl)? notUsedPar : defndPar;
#endif
        if (defParList->MayModifFl) ptrId->MayModif = True;
        ptrId->type = curType;
        ptrId->attribb = (curType!=NULL && IsArr(curType))? NoAttrib
			/* so that no LocalAdr d-pragma needed for &par[...] */
				     : defParList->attriba;  /* auto/register */
/*        if (curType != NULL)
          if (curType->typeSort==Bool) ptrId->notPureBoo = True;
	  * if so done, warning in 'constPtd()', on 'resultIfNULL' parameter *
          else */
        ptrId->defLineNb = lineNb;		/* record              */
        ptrId->defFileName = curFileName;	/*        birth place. */
        ptrId->nameb = defParList->idName;}}}
  else {fctName = NULL; enterBlock();}
  (void)freeDeclElt(x);
  {
    Tname oldCurFctName = curFctName;  /* necessary because of possible
					    (erroneously) embedded functions. */

    curFctName = fctName;
    enterFctBody(defFctTyp);
    curFctName = oldCurFctName;}
}

static void initOrSizFld(TdeclList x, TsemanElt *ptrId)
{
  if (Found(COLON)) nbBits(x);
  else {
    TpcTypeElt objType = x->type;

    if (x->idName==NULL && (objType==NULL || !(objType->typeSort==Enum &&
	   objType->tagId!=NULL /* not 'char' */ && objType->tagId->type->
		  intPoss))) err0((x->memberFl)? MsngMembName : MsngObjTypName);
    if (curTok.tok == ASGNOP) {
      TmacExpnd locMacExpndNb = curMacExpNb;

      if ((TkAsgn)curTok.val != SimplAsgn) err0(SimplAsgnExptd);
      paramTxt = txtAsgnTok[(TkAsgn)curTok.val];
      if (x->attriba == Typdf) err0(CantInitTpdf);
      else if (objType!=NULL && IsFct(objType)) err0(FctCantBeInit);
      else {
        if (objType!=NULL && IsStrun(objType)) (void)sizeOfTypeD(objType,
							  IncplType, x->idName);
        x->initFl = True;}
      GetNxtTok();  /* here for good error position */
      {
        bool cmplxInit = curTok.tok==LBRA || curTok.tok==CSTST;

        limErroTypes = NULL;  /* forget previous erroneous types */
        lExp.ptrId = ptrId;  /* for managePointers()/modifPtdVal() */
        init((ModifType)objType, x->attriba < Extrn, True, locMacExpndNb);
        if (x->attriba>=Auto && objType!=NULL && objType->typeSort>=Array &&
		cmplxInit && !Found(DYNINIT)) err0(SlowingInit|Warn1|Rdbl|
								    PossErr);}}}
}

static void init(TpTypeElt objType, bool cstExp, bool chkIntCst, TmacExpnd
									  macNb)
{
  if (curTok.tok == LBRA) {
    TtypeSort ts = (objType == NULL)? Void : objType->typeSort;

    if (ts == Array) {
      size_t oldCurIArrayIndex = curInitArrayIndex;
      TpcTypeElt oldCurIArrayIndexType = curInitArrayIndexType;
      TpTypeElt eltType = (ModifType)NxtTypElt(objType);

      GetNxtTok();
      curInitArrayIndex = 0;
      curInitArrayIndexType = objType->indexType;
      do {
        init(eltType, True, False, 0);
        curInitArrayIndex++;
      } while ((curTok.tok == COMMA) && (NxtTok() != RBRA));
      if (objType->size == 0) {			  /* if size not specified... */
        if (curInitArrayIndex > objType->lim) objType->lim = curInitArrayIndex;}
			    /* then size of array defined by length of
			       longest initializer list (dimensionality > 1). */
      else if (curInitArrayIndex > objType->lim) err0(TooManyIniz);
      curInitArrayIndex = oldCurIArrayIndex;
      curInitArrayIndexType = oldCurIArrayIndexType;}
    else if (IsStrunSort(ts)) {
      TdeclList oldCurIMember = curInitMember;
      TmacExpnd locMacExpNb = curMacExpNb;
      bool locLitCst = False;

      GetNxtTok();
      if (objType->tagId->listAlwdFiles  != NULL) checkPrivVisible(objType);
      curInitMember = BaseStrunType(objType)->memberList;
      while (curInitMember != NULL) {
        bool nb = curTok.tok != LBRA;

        init((ModifType)curInitMember->type, True, chkIntCst && !nb,
								   locMacExpNb);
        if (nb && cExp.litCst) locLitCst = chkIntCst;
        do {curInitMember = curInitMember->cdr;} while (curInitMember!=NULL &&
		curInitMember->fldFl && curInitMember->idName==NULL);  /* skip
								     padding. */
        if (curTok.tok!=COMMA || NxtTok()==RBRA) goto checkRBraL;
        if (ts == Union) break;}
      err0(TooManyIniz);
      skipTok(zRbraSCol);
checkRBraL:
      curInitMember = oldCurIMember;
      if (locLitCst && (curMacExpNb==macNb || curMacExpNb!=locMacExpNb) &&
				     chkNumCst) err0(NotNamedStrunCst | Warn1);}
    else {
      err0(TooManyBraLvl|Warn1|Rdbl);
      GetNxtTok();
      init(objType, cstExp, chkIntCst, macNb);}
    if (! Found(RBRA)) {
      err0((ts >= Array)? RBraOrCommaExptd : RBraExptd);
      cleverSkipTok();}}
  else if (correctExprN(objType, Void, cstExp, paramTxt, chkIntCst)) {
    if (objType!=NULL && IsArr(objType)) {
      if (! cExp.rEvlbl) err0(CstExpExptd);
      else {  /* Initialization of character array by string literal */
        size_t lgtStr = cExp.type->size;

        if (objType->size == 0) {  /* limit not specified */
          if (objType->lim < lgtStr) objType->lim = lgtStr;}
        else if (/*~CastTo size_t*/ /* to avoid dcc warning on unsigned
	  difference */ (lgtStr - 1) > objType->lim) /* trick! (if erroneous,
	  objType->lim = -1U => greater than any size) */  err0(TooManyIniz);}}}
}

static void nbBits(TdeclList x)
{
  TpcTypeElt fldType;

  if (! x->memberFl) err0(FldNotAlwd);
  if (! correctExprN(NULL, WhoEnum, True, ":", True)) cExp.uVal = 0;
  /* *doIt* Portability (field wdth < 16 = min sizeof(int) ) */
  else if (cExp.uVal>IntSiz*CHAR_BITC || (x->idName!=NULL &&
			      cExp.uVal==0)) {err0(FldSizIncor); cExp.uVal = 0;}
  x->fldSize = (uint)cExp.uVal;
  x->fldFl = True;
  if ((fldType = x->type) != NULL) {
    if (chkPortbl) 
      if (! (fldType->typeSort & (UInt | Int | Bool))) errWT((fldType->typeSort
		        ==Enum && fldType->tagId!=NULL)? UnsuitFldType | Warn1 :
							UnsuitFldType, fldType);
      else if (cExp.uVal!=0 && fldType->typeSort==Int && fldType->SynthQualif
				   !=Sgnd) errWT(IntBitField | CWarn2, fldType);
    if (fldType->shared) x->type = allocTypeEltIC(fldType, NoQualif);  /* to be
						    able to set 'size' field. */
    ((ModifType)x->type)->size = 1;}  /* *doIt* size in bits */
}

static void declObj(TdeclList pDeclElt)
{
  TsemanElt *ptrId = NULL;
  TpcTypeElt objType = pDeclElt->type;
  Tattrib objAttrib = pDeclElt->attriba;
  Tname objName;
  bool reDecl = False;
  Tstring declFile = curHdrFName;  /* even for 'extern's (wrongly) declared
				   in body file (=> no ExtObjNotDef warning). */

  /* Insert new declaration into symbol table */
  if ((objName = pDeclElt->idName) != NULL) { /* correct declar. of an object */
    if ((ptrId = enterSymTabHC(objName, pDeclElt->hCode)) == NULL) {  /* alrea-
						      dy declared identifier. */
      if (objAttrib != NoAttrib) {
        ptrId = searchSymTabHC(objName, pDeclElt->hCode);
        if (adjustFile) {
          if (objType==NULL || ((!IsFct(objType) || objAttrib!=Extrn) &&
	    objAttrib!=Typdf && errAlrdDefId(objName, objType, ptrId->kind /*
		because of CantChgMnng */)==NULL)) goto freeObjTypeL;  /* not
				  typedef nor function profile to be amended. */
          if (objAttrib==Typdf && ptrId->kind!=Type || objAttrib!=Typdf &&
		ptrId->type!=NULL && objType->typeSort!=ptrId->type->typeSort) {
            err0(CantChgMnng);
            goto freeObjTypeL;}
          if (ptrId->kind==Type && ptrId->namedType!=NULL) {
            reDecl = True;
            freeTypeChain(NxtTypElt(ptrId->namedType));}} /* keep first typeElt,
						       to redirect namedType. */
        else {
          bool sameFile = !headerFile || StrEq(skipPath(ptrId->declaringFile),
							 skipPath(curHdrFName));

          if (objAttrib==Extrn && ptrId->attribb==Extrn && ptrId->kind==Obj
	      && ptrId->declaringFile!=nonFileName && !sameFile) {  /* 'extern'
			declarations may be duplicated only once, in different
			header files, the last of which being the header file
			corresponding to the current compilation unit. */
            if (! isBodyHdrFile(curHdrFName)) {
              if (! isBodyHdrFile(ptrId->declaringFile)) goto redeclL;
              errWNSS(IncldAfter|Warn1|Rdbl, objName, ptrId->declaringFile,
								  curHdrFName);}
            errQalTyp1 = NULL;
            if (!CompatType(ptrId->type, objType, ExtDcl) || errQalTyp1!= NULL
									     ) {
              Tverbo oldVerbo = verbose;

              if (verbose < HalfVerbo) verbose = HalfVerbo;
              errId2((compatType(ptrId->type, objType, CStrChkExtDcl))?
			MoreStrngType|Warn2|PossErr : MoreStrngType, ptrId,
				       typeToS1(ptrId->type),typeToS2(objType));
              verbose = oldVerbo;
              goto freeL;}
            if (CompatType(ptrId->type, objType, StrictChk)) goto freeL;}
				     /* keep previous declaration coordinates */
          else
redeclL:
            if ((ptrId->attribb!=Extrn || objAttrib<Auto) && (errAlrdDefId(
		objName, objType, (objAttrib == Typdf)? Type : Obj)==NULL ||
								objType==NULL))
freeL:      {
              if (ptrId->kind==Obj && !sameFile) ptrId->declaringFile =
					      nonFileName;  /* see below why. */
              goto freeObjTypeL;}
          declFile = (! sameFile)? nonFileName : ptrId->declaringFile;}  /* if
			several declaring files, which one to jot ? =>
			impossible file name (-> no 'ExtObjNotDef' message). */
        freeTypeChain(ptrId->type);}}
    else {  /* new identifier, initialize it */
      DefSem(notDefndObj, Obj, False /*defnd*/);

      ptrId->infoS = notDefndObj;}
    if (objAttrib == Typdf) {
      if (isSameName(pDeclElt->hCode, objName, BoolHCode, BoolTName)) {
        if (chkBool) {
          if (objType!=NULL && objType->typeSort!=UInt) err0(IlgBoolDef|Warn1|
								       PossErr);
          pDeclElt->paralTypeFl = False;
          objType = ptrId->namedType = allocTypeEltIC(&boolTypeElt, (objType ==
					     NULL)? NoQualif : objType->qualif);
          reDecl = True;}}
      else if (chkTypName && !(isupper((char)*(objName + LgtHdrId)) || (char)*
						 (curTok.name + LgtHdrId)=='_'))
        errWN(UpCaseTypName|Warn1|Rdbl, objName);
      /* Create type chain that will be shared by all users of defined type */
      if (objType == NULL) ptrId->namedType = NULL;
      else {
        TpTypeElt newElt;  /* typeElt for users of this typedef */

        newElt = (reDecl)
                  ? (ModifType)ptrId->namedType /* reuse old typeElt, because
					      it may already be pointed upon. */
                  : allocTypeElt();
        *newElt = *objType;
        newElt->shared = True; newElt->noFreeDepdt = True; newElt->stopFreeing
									 = True;
        if (sysAdjHdrFile) {
          newElt->paralTyp = False;
          newElt->sysTpdf = True;  /* pseudo parallel type (to have the indi-
			rection through the corresponding typeId, indirection
			that will allow possible redefinition of typeId in an
			adjust file, even after having been used. */
          newElt->typeId = ptrId;
          newElt->rootTyp = False;}
        else if (pDeclElt->paralTypeFl) {
          newElt->paralTyp = True;
          newElt->typeId = ptrId;
          newElt->rootTyp = False;
          for (;; GetNxtTok()) {
            switch (curTok.tok) {
              case ROOTTYP:
                if (newElt->rootTyp) errUslDP(curTok.tok);
                newElt->rootTyp = True;
                break;
              case LITCST:
                if (newElt->litCsta) errUslDP(curTok.tok);
                newElt->litCsta = True;
                break;
              default: goto exitLoopL;}}
exitLoopL: ;}
        /* else description bits stays the same (qualified variant) */
        ptrId->namedType = newElt;}
      {
        DefSem(defndType, Type, True /*defnd*/);
        DefSem1(defndUsedType, Type, True /*defnd*/, True /*used*/, False
								    /*initlz*/);

#ifdef _AIX
        if (headerFile) ptrId->infoS = defndUsedType; /* for no 'UnusedTypeId'
									 msg. */
        else ptrId->infoS = defndType;}
#else
        ptrId->infoS = (headerFile)? defndUsedType /* for no 'UnusedTypeId'
							    msg */ : defndType;}
#endif
      if (insideHdrInHdr) ptrId->declInInHdr = True;}
    else {		/* not type definition */
      if (objAttrib==NoAttrib && headerFile && !msngAttribFl) {
        errWS(NoDefInHdrFile|Warn2|Rdbl, errTxt[MsngExtTpdf]);
        msngAttribFl = True;}
      /* Verify constraints on declaration/definition, taking specified
         attribute and current block level into account. */
      if (objType!=NULL && IsFct(objType)) {  /* function */
        if (objAttrib == NoAttrib) {
          if (ptrId==NULL && (ptrId = errAlrdDefId(objName, objType, Obj)) ==
							NULL) goto freeObjTypeL;
          if (!headerFile && !msngAttribFl) {
            errWN((nestLvl == 0)? MsngFctAttrib|Warn1|Rdbl :
					    ShdBeInHdrFile|Warn1|Rdbl, objName);
            msngAttribFl = True;}
          objAttrib = (nestLvl==0 && !headerFile)? Stati : Extrn;}
        if (objAttrib == StatiL) errWN(OnlyExtrn, objName);
        else if (objAttrib > Extrn) errWN(IlgAttrib, objName);
        ptrId->initlz = True;
        if (objType->nextTE == &boolCstTypeElt) ptrId->notPureBoo = True;}
				/* for pseudo-boolean functions ('feof' etc.);
						   'generic' bit already set. */
      else {		/* object not function */
        if (objAttrib == Extrn) {
/*          if (objType!=NULL && objType->typeSort==Bool) ptrId->notPureBoo =
			  True;*/}  /* just in case True in other modules ... */
        else {
          if (objAttrib == NoAttrib) {	/* external definition
						    (necessarily at level 0). */
            if (ptrId == NULL) {/* identifier already seen (e.g, declaration) */
              ptrId = searchSymTabHC(objName, pDeclElt->hCode);
              if ((ptrId->kind!=Obj || ptrId->defnd) && errAlrdDefId(objName,
				       objType, Obj) == NULL) goto freeObjTypeL;
              checkWrngHdrFile(ptrId);
              if (! CompatType(ptrId->type, objType, StrictChk)) {
                errId2((compatType(ptrId->type, objType, CStrChkExtDcl))?
			NotSameType|Warn2|PossErr : NotSameType, ptrId,
				      typeToS1(objType), typeToS2(ptrId->type));
                ptrId->defnd = True; goto freeObjTypeL;}
              freeTypeChain(ptrId->type);}  /* to be able to replace int[] by
							 int[5], for example. */
            else if (!headerFile && !msngAttribFl) {errMsngStati(objName)
							 ; msngAttribFl = True;}
            objAttrib = Extrn;
/*            if (objType!=NULL && objType->typeSort==Bool) ptrId->notPureBoo
			= True;*/}  /* just in case True in other modules ... */
          else if (objAttrib!=Stati && nestLvl==0 || objAttrib==Reg && objType
		   !=NULL && objType->typeSort>Array) errWN(IlgAttrib, objName);
          ptrId->defnd = True;}}  /* defined */
      ptrId->declaringFile = declFile;
      if ((ptrId->attribb = objAttrib)==Extrn && insideHdrInHdr) ptrId->
							     declInInHdr = True;
      if (headerFile && objAttrib==Stati) ptrId->used = True;}  /* to prevent
								     warning. */
    ptrId->type = pDeclElt->type;
    ptrId->defLineNb = lineNb;		/* record              */
    ptrId->defFileName = curFileName;	/*        birth place. */
    ptrId->nameb = objName;
    initOrSizFld(pDeclElt, ptrId);
    ptrId->type = objType = pDeclElt->type;}  /* but ptrId->type must also be
	 defined before calling 'initOrSizeFld', because of case "int i = i". */
  else
freeObjTypeL:
  {
    initOrSizFld(pDeclElt, NULL);  /* before pDeclElt->type freeing, to get
	      more interesting messages (for example, no 'NotStrunInit' msg). */
    freeTypeChain(pDeclElt->type);
    objType = NULL;}
  if (pDeclElt->initFl) {
    computeSize((ModifType)objType);
    if (ptrId != NULL) {
      if (cExp.lclAdr) ptrId->lclAd = True;  /* does also notPureBool */
      ptrId->initlz = True;}
    if (pDeclElt->attriba==Extrn) errWN(IlgInit|Warn1|Rdbl, objName);}
  if (objType!=NULL && !IsFct(objType)) {
    if (Found(NOTUSED) && ptrId!=NULL) {ptrId->defnd = False; ptrId->used =
									  True;}
    if (pDeclElt->attriba!=Typdf && !(pDeclElt->attriba==Extrn && IsArr(
		objType))) (void)sizeOfTypeD(objType, (pDeclElt->attriba ==
			 Extrn)? IncplType|Warn2|PossErr : IncplType, objName);}
}

/******************************************************************************/
/*			           UTILITIES				      */
/******************************************************************************/

static AllocXElt(allocDeclElt, TdeclList, ctrDE, resul->cdr = NULL;)  /* setting
	       'cdr' field to NULL makes element a list (of one element... ). */

static AllocXElt(allocTagListElt, TtagVisi *, ctrTLE, resul->preced = NULL;)

static AllocXElt(allocTypeElt, TpTypeElt, ctrTE, ;)

TpTypeElt allocTypeEltIC(TpcTypeElt x, Tqualif y)
/* New type element initialized by (amended) copy of x */
{
  TpTypeElt resul;

  resul = allocTypeElt();
  *resul = *x;
  resul->qualif |= y;
  resul->shared = False;
  resul->noFreeDepdt = True;  /* because of shallow copy */
  resul->stopFreeing = True;  /* a priori, only the new element is freeable
					     (because of usual shallow copy). */
  return resul;
}

TpTypeElt allocTypeEltID(TtypeSort x, Tqualif y)
/* New type element initialized by default values */
{
  TpTypeElt resul;

  resul = allocTypeElt();
  *resul = valInitTypeElt;
  resul->typeSort = x;
  resul->qualif = y;
  return resul;
}

void checkPrivVisible(TpcTypeElt type)
{
  const TsemanElt *pTag = type->tagId;

  if (   !isFNameVisible(strpdCUnitFName, pTag)
      && !(InsideMacro && visibleFromMac(pTag))) {
    Tverbo savVerb = verbose;
    Tstring terseTypeRepre = typeToS1((verbose = Terse /* so that inside of
						      strun not seen */, type));

    verbose = savVerb;
    errId(PrivNotVisi | Warn1, pTag, NULL, terseTypeRepre, typeToS2(type));}
}

static void checkWrngHdrFile(const TsemanElt *pId)
{
  if (!isBodyHdrFile(pId->declaringFile) && *pId->declaringFile!='\0')
    if (pId->declaringFile == nonFileName) errId1(WrngCUnit|Warn1|Rdbl, pId,
								    cUnitFName);
    else if (! isBodyHdrFile(pId->defFileName)) errId1(WrngHdrFile|Warn1|Rdbl,
							       pId, cUnitFName);
}

/* "Local" global variables for managing array qualifiers (that in fact qualify
   arrays elements => propagation (managed this way because of the handling of
   parallel types). */

static bool arrayIsCurrent = False;
static Tqualif accumXQal, accumYQal;

bool compatType(TpcTypeElt x, TpcTypeElt y, TkTypeEquiv kindEqv)
/* 'x' = left/formal/declaration type; 'y' = right/actual/definition type.
   To be kept coherent with resulType() ("dcexp.c").
   Answers True (False for VerifCastF) if either (sub-)type is NULL */
{
  bool notFrstTurn = False, notAllConst;

  arrayIsCurrent = False;
  for (;;) {
    TtypeSort tsx, tsy;

    if (x==NULL || y==NULL) return (kindEqv!=VerifCastF);
    tsx = x->typeSort;
    tsy = y->typeSort;
    if (x->sysTpdf!=y->sysTpdf && tsx&Who && kindEqv<CCheck1) return False;
    if (!(y->generic && InsideInterval(kindEqv, Asgn, FctCall) && x->paralTyp
							      && rowUp(x, y))) {
      if (y->paralTyp) {
        if (kindEqv < NoCheck) {  /* VerifCast/StrictChk */
          if (x->typeId != y->typeId) return False;}
        else if (   !(kindEqv==FctCall && InsideInterval(tsy, Array, VFct) &&
	 			!notFrstTurn) /* because Array => &Array[0]
					     and operator '&' yields generic. */
                 && !rowUp(y, x)
                 && kindEqv<CCheck1 /* !(CCheck1/CCheck/CStrictChk/
					      CStrChkExtDecl) */) return False;}
      else if (x->paralTyp && (
                      InsideInterval(kindEqv, StrictChk, IlgIcp)
                   && !(y->generic && rowUp(x, y))
                   && tsy!=Array
                 || InsideInterval(kindEqv, VerifCast, VerifCastF))
								) return False;}
    if (tsx != tsy) {  /* not same type sort */
      if (kindEqv < NoCheck /* VerifCast/StrictChk */) return False;
      {
        if (   kindEqv<LitString  /* StrictChk/NoCheck/Cmpar */
            || !(   tsx==Ptr
                 && tsy==Array
                 && (y->generic || kindEqv==FctCall && !notFrstTurn)
            || kindEqv==FctCall
             && tsx==Array
             && tsy==Ptr) /* not (Ptr <- String or, if FctCall, Array <->
								     Ptr). */) {
          /* Accept, for CCheck1 type of equivalence, pointer on numeric type
             to be equivalent to pointer on corresponding unsigned type (gua-
		       ranteed by standard to be of same size and alignment). */
          if (! (kindEqv==CCheck || kindEqv==CCheck1 && ((tsx | tsy)==(Byte |
		UByte) || (tsx | tsy)==(Short | UShort) || (tsx | tsy)==(Int |
			   UInt) || (tsx | tsy)==(Long | ULong)))) return False;
          /* Manage C equivalence between all integral types, Ptr <=> &Array[],
							    Fct <=> Ptr/Fct. */
          if (! (tsx & NumEnumBool && tsy & NumEnumBool)) {
            if (IsPtrArrSort(tsx)) {
              if (IsPtrArrSort(tsy)) goto localSkipTstL;
              if (! IsFctSort(tsy)) return False;
              if ((x = NxtTypElt(x)) == NULL) return True;
              tsx = x->typeSort;}
            else {
              if (!IsFctSort(tsx) || tsy!=Ptr) return False;
              if ((y = NxtTypElt(y)) == NULL) return True;
              tsy = y->typeSort;}
            if (tsx != tsy) return False;
localSkipTstL:;}}
        if (tsx==Ptr && tsy==Array && y->qualif!=NoQualif
							) accumulateQal(x, y);}}
    if (tsx >= Enum)
      switch (tsx >> PosDelT) {
      case Enum>>PosDelT:
        if (x->tagId!=y->tagId && kindEqv!=CCheck) return False;
        break;
      case Array>>PosDelT:
        if (!y->generic /* not string literal */
            && (!((kindEqv < NoCheck)
		   ? compatTypeR(x->indexType, y->indexType, VerifCastF)
		   : (x->indexType==&defaultIndexTypeElt || compatTypeR(x->
			   indexType, y->indexType, NoCheck)))
                  && kindEqv<CCheck
                || (x->lim!=0 && tsy!=Ptr || kindEqv==StrictChk)
                  && x->lim!=ArrLimErr
                  && y->lim!=ArrLimErr
                  && x->lim!=y->lim
                  && (kindEqv!=CStrChkExtDcl || y->lim!=0))) return False;
        if (x->qualif!=NoQualif || y->qualif!=NoQualif) accumulateQal(x, y);
        if (! InsideInterval(kindEqv, ExtDcl, CCheck1)) goto endPtrL;  /*
					   !(ExtDecl/FctCall/IlgIcp/CCheck1). */
        /*~NoBreak*/
      case Ptr>>PosDelT: {
          TpcTypeElt nxtX, nxtY;

          if ((nxtX = NxtTypElt(x))==NULL || (nxtY = NxtTypElt(y))==NULL) break;
          if (InsideInterval(kindEqv, Asgn, CCheck1)) {/* Asgn/ExtDcl/
						      FctCall/IlgIcp/CCheck1. */
            if (tsx==Array && nxtY->typeSort==Array && nxtX->typeSort!=Array
								 ) return False;
            /* Check pointed element qualifiers, according to C++ Standard */
            {
              Tqualif qualNxtX = (arrayIsCurrent)? arrQal(nxtX, accumXQal) :
								   nxtX->qualif,
                      qualNxtY = (arrayIsCurrent)? arrQal(nxtY, accumYQal) :
								   nxtY->qualif;

              if (! notFrstTurn) notAllConst = False;
              if ((((kindEqv == ExtDcl)? qualNxtX & ~qualNxtY : qualNxtY &
				~qualNxtX)!=NoQualif /* compatible quelifiers */
                    || notAllConst && qualNxtX!=qualNxtY)
                  && errQalTyp1==NULL) {  /* keep only first warning */
                static TtypeElt wx, wy;

                errQalTyp1 = (arrayIsCurrent && accumXQal!=NoQualif)? (wx = *x,
					       wx.qualif |= accumXQal, &wx) : x;
                errQalTyp2 = (arrayIsCurrent && accumYQal!=NoQualif)? (wy = *y,
					    wy.qualif |= accumYQal, &wy) : y;}
              if (! (qualNxtX & ConstQal) && nxtX->typeSort!=Array /* array
			     always const by itself... */) notAllConst = True;}}
          /* Allow equivalences 'anyType *' => 'void *' and generic 'void *' =>
								 'anyType *'. */
          if (!InsideInterval(kindEqv, StrictChk, NoCheck) /* !(NoCheck/Strict-
		Chk) */ && kindEqv!=CStrChkExtDcl && !notFrstTurn
		&& (nxtX->typeSort==Void && (!IsFct(nxtY) || kindEqv==CCheck) ||
		nxtY->typeSort==Void && (y->generic || kindEqv==CCheck))) return
			(kindEqv>=NoCheck || ((arrayIsCurrent)? sameQal(x, y) &&
				sameQal(nxtX, nxtY) : x->qualif==y->qualif &&
						  nxtX->qualif==nxtY->qualif));}
        if (!((kindEqv < NoCheck)
	       ? compatTypeR(x->indexType, y->indexType, VerifCastF)
	       : (x->indexType==&defaultIndexTypeElt || compatTypeR(x->
	    indexType, y->indexType, NoCheck))) && kindEqv<CCheck) return False;
endPtrL:
        notFrstTurn = True;
        break;
      case Fct>>PosDelT: case VFct>>PosDelT: {
          /*TdeclList*/const TdeclElt *wx = x->paramList, *wy = y->paramList;

          if (InsideInterval(kindEqv, Asgn, IlgIcp) && (x->pvNr && !y->pvNr ||
						     x->generic && !y->generic)
	      || kindEqv<NoCheck /* VerifCast/StrictChk */ && (x->pvNr!=y->
				  pvNr || x->generic!=y->generic)) return False;
          while ((wx != NULL) && (wy != NULL)) {
            if (!CompatType(wy->type, wx->type, kindEqv) || wx->parQal!=
	     wy->parQal && (InsideInterval(kindEqv, Asgn, IlgIcp) ||
	      kindEqv<NoCheck /* VerifCast/StrictChk */)) return False;
            wx = wx->cdr; wy = wy->cdr;}
          if (wx!=NULL && tsy!=VFct || wy!=NULL && tsx!=VFct) return False; 
					     /* not same number of parameters */
          notFrstTurn = False;  /* function result has nothing to do with
							       function call. */
          break;}
      case Struc>>PosDelT: case Union>>PosDelT:
        if (x->tagId != y->tagId) return False;
        break;
      /*~ NoDefault */}
    if (kindEqv<NoCheck /* VerifCast/StrictChk */ && ((arrayIsCurrent)?
			   !sameQal(x, y) : x->qualif!=y->qualif)) return False;
    if (arrayIsCurrent) chkEndAccumQal(x, y);
    if ((x = NxtTypElt(x))==(y = NxtTypElt(y)) && !arrayIsCurrent) return True;
    /* Stricter check for pointers, arrays ... */
    if (kindEqv == CCheck) kindEqv = CCheck1;}
}

static void accumulateQal(TpcTypeElt x, TpcTypeElt y)
{
  if (! arrayIsCurrent) {
    accumXQal = accumYQal = NoQualif;
    arrayIsCurrent = True;}
  if (IsArr(x)) accumXQal |= x->qualif;
  if (IsArr(y)) accumYQal |= y->qualif;
}

static Tqualif arrQal(TpcTypeElt t, Tqualif q)
{
  return (IsArr(t))? NoQualif : t->qualif | q;
}

static void chkEndAccumQal(TpcTypeElt x, TpcTypeElt y)
{
  if (! IsArr(x)) accumXQal = NoQualif;
  if (! IsArr(y)) accumYQal = NoQualif;
  if (accumXQal==NoQualif && accumYQal==NoQualif) arrayIsCurrent = False;
}

static bool compatTypeR(TpcTypeElt x, TpcTypeElt y, TkTypeEquiv kindEqv)
{
  if (x == y) return True;
  {
    bool saveAIC = arrayIsCurrent, resul;
    Tqualif saveAXQ = accumXQal, saveAYQ = accumYQal;

    resul = CompatType(x, y, kindEqv);
    arrayIsCurrent = saveAIC;
    accumXQal = saveAXQ; accumYQal = saveAYQ;
    return resul;}
}

static bool sameQal(TpcTypeElt x, TpcTypeElt y)
{
  return arrQal(x, accumXQal) == arrQal(y, accumYQal);
}

/*~ Undef arrayIsCurrent, accumXQal, accumYQal */

static void computeSize(TpTypeElt x)
/* Computes size of all arrays in type chain 'x' */
{
  TpcTypeElt nxtType;

  if (x==NULL || x->shared || (nxtType = NxtTypElt(x))==NULL) return;
  computeSize((ModifType)nxtType);
  if (IsArr(x)) x->size = sizeOfTypeD(nxtType, ArrOfIncplOrFctElt, NULL) *	
									 x->lim;
  else if (IsFct(x) && IsArrFct(nxtType)) errWT(ArrOrFctCantBeRet, nxtType);
}

static TdeclList createDLElt(TcreatType dId /*~MayModify*/, TinfoDecl infoDcl,
						  TpcTypeElt type, Tqualif qual)
{
  TdeclList dList;

  if (type != NULL) {
    if (type->qualif & qual) err0(AlrdQual);
    /* If array (case "typedef int Tarr[]; Tarr a={1,2}, b={3};"), or added
       qualifiers, create copy of type element; copy owned by the declared
       object, so has to be freed when the object disappears. */
    if (qual & ~type->qualif || type->size==0 && IsArr(type) || infoDcl.
								   _signedInt) {
      TpTypeElt w = allocTypeEltIC(type, qual);

      type = w;
      if (infoDcl._signedInt) w->SynthQualif = Sgnd;}}
  /* Connect baseType and type modifier */
  if (dId.headType != NULL) ((ModifType)dId.tailType)->nextTE = type;
  else {  /* no modifier */
    dId.headType = type;
    /* Parallel Type ? (no qualifier, no type modifier and type defined by
       typeId). */
    if (qual==NoQualif && !infoDcl._noNewTypeFl) infoDcl._paralTypeFl = True;}
  /* Allocate and fill new declList element */
  dList = allocDeclElt();
  dList->idName = dId.idName;
  dList->hCode = dId.hCode;
  dList->type = dId.headType;	/* beginning of type chain */
  computeSize((ModifType)dList->type);
  dList->infoD = infoDcl;
  return dList;
}

static long stubCtr = 0;

static void initCreateTagName(void)
{
  stubCtr = 0;
}

static Tname createTagName(Tname x)
{
  if (x == NULL) {  /* create name only if stub */
    Tchar buf[sizeof(TnbBuf) + LgtHdrId + 1], *ptrBuf = &buf[LgtHdrId + 1];

    bufLongToS(++stubCtr, ptrBuf);
    while (*ptrBuf++ != '\0') {;}
    buf[0]= (Tchar)(ptrBuf - &buf[LgtHdrId + 1]);
    buf[LgtHdrId] = StartTagCh;
    x = storeName(ConvTname(&buf[0]), TagSpace);}
  return x;
}

/*~Undef stubCtr */

TpcTypeElt declType(void)
{
  TdeclList typeName;
  TpcTypeElt type;
  jmp_buf localJmpBuf, *savErrRet = curErrRet;

  curErrRet = &localJmpBuf /*~ LocalAdr */;
  if (setjmp(*curErrRet) == 0) {
    noNewTypeInfo._prioToCast = True;
    typeName = decl1(noNewTypeInfo);
    if (typeName->idName != NULL) errWN(NoIdAlwdInType, typeName->idName);
    type = typeName->type;
    (void) freeDeclElt(typeName);
    if (type!=NULL && !type->shared) ((ModifType)type)->noOwner = True;}
  else {skipTok(zCoScEdpRPRB); type = NULL;}
  curErrRet = savErrRet;
  return type;
}

TsemanElt *defineId(TinfoSeman infoS)
/* Create a fake typeId to prevent further 'UndefId' errors. */
{
  TsemanElt *ptrId;
  Tname w;

  if (allErrFl) return NULL;
  ptrId = enterSymTabHC((w = storeName(curTok.name, ObjectSpace)), curTok.hCod);
  ptrId->type = NULL;
  if (infoS._kind == Obj) ptrId->declaringFile = curHdrFName;
  else ptrId->namedType = NULL;
  ptrId->defLineNb = lineNb;		/* record              */
  ptrId->defFileName = curFileName;	/*        birth place. */
  ptrId->nameb = w;
  ptrId->infoS = infoS;
  return ptrId;
}

static TsemanElt *errAlrdDefId(Tname x, TpcTypeElt y, Tkind z)
{
  TsemanElt *ptrId = searchSymTab(x);

  if (ptrId->type==NULL && (ptrId->kind==z || ptrId->kind==Obj && z==EnumCst)
					 ) return ptrId;  /* "forced defined" */
  errId((y==QuasiNULLval(TpcTypeElt) || !ptrId->defnd && y!=NULL && CompatType(
	  ptrId->type, y, CStrChkExtDcl))? AlrdDefId|Warn2|PossErr : AlrdDefId,
							  ptrId, x, NULL, NULL);
  return NULL;
}

static void errMsngStati(Tname x)
{
  register TstringNC w;
  register Tstring w1 = cUnitFName;
  TstringNC w2;
  Tstring w3;

  MyAlloc(w, (size_t)((w3 = SearchDot(w1)) - w1 + 1));
  w2 = w;
  while (w1 != w3) {*w++ = *w1++;}
  *w = '\0';
  errWNSS(MsngStati|Warn1|Rdbl, x, w2, NULL);
  free(w2);
}

static FreeXElt(freeDeclElt, TdeclList, ctrDE, ;, cdr)

static void freeDeclList(TdeclList x)
{
  while (x != NULL) {freeTypeChain(x->type); x = freeDeclElt(x);}
}

static FreeXElt(freeTagListElt, TtagVisi *, ctrTLE, ;, preced)

void freeTypeChain(TpcTypeElt x)
{
  while (x != NULL) {x = freeTypeElt(x);}  /* x may be NULL at the outset
			   (case of undefined Id, or empty indexType, or...). */
}

FreeXElt(freeTypeElt, TpcTypeElt, ctrTE, {
	  if (x->shared) return NULL;  /* stop freeing immediately */
          ((ModifType)x)->shared = True;  /* to prevent several freeing of the
			same typeElt (by remaining types pointing on it, because
			otherwise no more protected by 'shared' bit). */
          if (x->stopFreeing) resul = NULL;  /* stop freeing after this one */
          if (! x->noFreeDepdt)
            switch (x->typeSort  >> PosDelT) {
            case Array>>PosDelT: {
              freeTypeChain(x->indexType);  /* indexType may be NULL, so don't
						  use freeTypeElt() directly. */
              break;}
            case Fct>>PosDelT: case VFct>>PosDelT: freeDeclList(x->paramList);
									  break;
            case Struc>>PosDelT: case Union>>PosDelT: freeDeclList(x->
							     memberList); break;
            /*~ NoDefault */}
                                        }, nextTE)

void freeTypes(TsemanElt *x)
/* Beware of case "typedef unknId bool;" (type==NULL, namedType!=NULL) */
{
  if (! InsideInterval(x->kind, Obj, StrunTag)) return;  /* Param, EnumCst,
								       Label. */
  if (x->kind != Obj) {  /* Type, Tags */
    if (x->kind == Type) {
      if (x->namedType != NULL) {
/*        if (! x->namedType->shared) sysErr(ExCod5);*/
        ((ModifType)x->namedType)->shared = False;  /* to allow freeing */
        freeTypeChain(x->namedType);}}
    else {  /* Tags */
      if (x->type != NULL) {
/*        if (! x->type->shared) sysErr(ExCod8);*/
        ((ModifType)x->type)->shared = False;  /* to allow freeing */
        if (x->kind==EnumTag && x->type->frstEnumCst!=NULL) {  /* free type
					 element associated to enum constant. */
          ((ModifType)x->type->frstEnumCst->type)->shared = False;
          (void)freeTypeElt(x->type->frstEnumCst->type);}}}}
  freeTypeChain(x->type);
}

TpcTypeElt getINDEXTYPEtype(void)
{
  TpcTypeElt resul;

  resul = (curTok.dpType == NULL)
            ? NULL
            : (curTok.dpType->typeSort & WhoEnumBool)
              ? curTok.dpType
              : (err0(IntgrlTypeExptd | Warn2), FreeExpType1(curTok.dpType),
									  NULL);
  GetNxtTok();
  return resul;
}

void initDecl(void)
{
  initCreateTagName();
}

bool isRepreType(TpcTypeElt x)
{
  if (x==NULL || !InsideInterval(x->typeSort, Byte, LongDbl)) return False;
  do {
    if (x->paralTyp || x->sysTpdf) return False;
    x = x->nextTE;
  } while (x != NULL);
  return True;
}

static void makeGeneric(TpTypeElt x)
/* Mark 'generic' non-void function of base type (or type derived from base
   type via qualifier/modifier; in such a case, each level of (pointer) modifier
   is also marked 'generic'). */
{
  TpTypeElt nxtX;
  bool sharedSeen;

  if (x->shared) return;  /* typedef has already undergone makeGeneric */
  sharedSeen = False;
  while (!NxtIsTypeId(x) && (nxtX = (ModifType)x->nextTE)!=NULL && nxtX->
							       typeSort!=Void) {
    if (nxtX->shared || sharedSeen) {  /* duplicate type chain */
      x->nextTE = nxtX = allocTypeEltIC(nxtX, NoQualif);
      x->stopFreeing = False;
      sharedSeen = True;}
    if (! IsArrFct(nxtX)) nxtX->generic = True;
    x = nxtX;}
}

static void manageFctDPrags(const TdeclElt *dl)
{
  TpcTypeElt x = dl->type, retType;
  bool resultTypeSeen = False;

  while (x != NULL) { /* search whether there is a function in the type chain */
    if (IsFct(x)) {  /* search whether possible *~ResultType* or *~SizeOfBlock*
			      parameter compatible with function return type. */
      TdeclList parList;
      int parCtr;

      retType = NxtTypElt(x);
      for (parList = x->paramList, parCtr = 0; parCtr++, parList != NULL;
						       parList = parList->cdr) {
        if (parList->parQal == ResulTypV) {
          resultTypeSeen = True;
          if (! CompatType(retType, parList->type, NoCheck)) {
            errWSTTS(IncohResultType | Warn2, longToS(parCtr), parList->type,
		   retType, dpName((parList->ResulPtrFl)? RESULPTR : RESULTYP));
            parList->parQal = NoParQal;}}
        if (parList->SizeofBlFl && retType!=NULL && !IsPtr(retType)) {errIlgDP(
				       SIZEOFBL); parList->SizeofBlFl = False;}}
      goto foundFctL;}
    x = NxtTypElt(x);}
  return;  /* no function found */
foundFctL:
  {
    bool pvNrSeen = False, genUtilSeen = False;

    for (;; GetNxtTok()) {
      switch (curTok.tok) {
        case PSEUDOVOID: case NEVRET:
          if (pvNrSeen) break;
          pvNrSeen = True;
          if (retType!=NULL && (retType->typeSort == Void)==(curTok.tok ==
							     PSEUDOVOID)) break;
          ((ModifType)x)->pvNr = True;
          continue;
        case UTILITY:
          if (retType!=NULL && !isRepreType(retType)) break;
          /*~NoBreak*/
        case GENERIC:
          if (genUtilSeen) break;
          genUtilSeen = True;
          if (retType!=NULL && retType->typeSort==Void) break;
          if (resultTypeSeen) errWS(CantBeGeneUtil | Warn2, dpName(curTok.tok));
          else if (curTok.tok == GENERIC) ((ModifType)x)->generic = True;
          continue;
        default: goto exitLoopL;}
      errIlgDP(curTok.tok);}
exitLoopL:
    if (x->generic) makeGeneric((ModifType)x);
    else if (chkFctRetTyp && !genUtilSeen && !resultTypeSeen && !x->generic /*
	  because of possible typedef used to define function */ && !mainFl 
		&& isRepreType(retType)) errWT(ResRepreType | CWarn2, retType);}
}

void procExtent(void)
{
  TsemanElt *pSem = curTok.ptrSem;

  cExp.sVal = 0;
  if ((pSem != NULL)
       ? pSem->kind!=Type
       : (*((TnameNC /*~OddCast*/)curTok.name + DispNSId) = (TnameAtom)TagSpace
		  , (pSem = searchSymTab(curTok.name)) == NULL) || pSem->kind!=
				  EnumTag) errWN(NotEnumTypeOrTag, curTok.name);
  else {
    TpcTypeElt idType;

    if ((idType = pSem->type) != NULL) {
      register TsemanElt *w;

      if ((w = idType->frstEnumCst) == NULL) {
        if (idType->tagId == NULL) cExp.sVal = UCHAR_MAX;}
      else {
        cExp.sVal = (TcalcS)w->enumVal;
        while (w->nxtEnumCst != NULL) {w = w->nxtEnumCst;}
        cExp.sVal -= (TcalcS)w->enumVal;}}
    pSem->used = True;}
  cExp.type = &intCstTypeElt;
}

void procIndex(void)
{
  if (curInitArrayIndex == ArrLimErr) {err0(NotArrInit); cExp.errEvl = True;}
  cExp.uVal = (TcalcU)curInitArrayIndex;
  cExp.type = curInitArrayIndexType;
}

static void procMember(void)
{
  if (curInitMember == OutsideStrunInit) {err0(NotStrunInit); cExp.errEvl =
									  True;}
  cExp.type = &boolCstTypeElt;
}

void procMember1(void)
{
  cExp.uVal = (TcalcU)(curInitMember>OutsideStrunInit && isSameName(curTok.hCod,
		     curTok.name, curInitMember->hCode, curInitMember->idName));
  procMember();
}

void procMember2(void)
{
  cExp.uVal = (TcalcU)(curInitMember==NULL);
  procMember();
}

static bool rowUp(TpcTypeElt x, TpcTypeElt y)
/* Returns True if 'y' is a (reachable if heedRootType True) ancestor of 'x' */
{
  do {
    if (x->typeId == y->typeId) return True;  /* ancestor reached */
    if (x->rootTyp && heedRootType) return y->typeSort==Ptr && !y->paralTyp &&
	  ((y = y->nextTE)==NULL || y->typeSort==Void);  /* "rootType" (non
			 representation type) found (void* always reachable). */
    x = x->typeId->type;  /* climb hierarchy */
  } while (x->paralTyp);
  /* Representation type reached; OK if 'y' non-parallel type */
  return ! y->paralTyp;
}

size_t sizeOfTypeD(TpcTypeElt t /*~MayModify*/, Terr e, Tname n)
{
  TpcTypeElt t1 = t;

  if (t == NULL) return 0;
  if (IsStrun(t)) t = BaseStrunType(t);
  if (((IsArr(t))? t->lim : t->size)==0 && !t->errSiz) {
    errWNT(e, n, t1);
    if (!(e & WarnMsk) && t!=&natTyp[_VOID] && !allErrFl)
      ((ModifType)t)->errSiz = True;}
  return t->size;
}

static void cleverSkipTok(void)
{
  TcharTok peep;

  if (curTok.tok==SCOL && (((peep = nxtCharOrMacTok()) < 0)? peep==-(TcharTok)
	  RBRA : peep==(TcharTok)'}')) GetNxtTok();  /* for ExtraSColAtEndMac */
  skipTok(rBraSCol);
}

TresulTstPtd tstPtd(register TpcTypeElt w, bool onlyFrstLvl)
{
/*  TresulTstPtd addInfo;
  TpcTypeElt w1;*/

  if (w == NULL) return ERRTYPE;
  if (IsStrun(w)) return STRUN;
/*    addInfo = STRUN;
  else {
  */  if (! IsPtrArr(w)) return NOTPTR;
/*    addInfo = (TresulTstPtd)0;}*/
  {
    bool constArray = False, firstTurn = True;
    TtypeSort ts;

    do {
/*      w1 = w;*/
      if ((ts = w->typeSort) == Array) {if (w->qualif & ConstQal) constArray =
									  True;}
      else {
        if (IsFctSort(ts)) break;
        if (! firstTurn) {
          if (! (w->qualif & ConstQal || constArray)) return /*addInfo |*/
								    NOTALLCONST;
          if (onlyFrstLvl) return /*addInfo |*/ ALLCONST;
          constArray = False;}}
      firstTurn = False;
    } while ((w = NxtTypElt(w)) != NULL);
#if False
    if (!onlyFrstLvl && IsStrunSort(ts)) {
      /* Search whether there exist non 'const' pointer fields */
      const TdeclElt *member = BaseStrunType(w1)->memberList;

      while (member != NULL) {
        if (tstPtd(member->type, False) & NOTALLCONST) return NOTALLCONST |
									addInfo;
        member = member->cdr;}}
#endif
    return ((IsPtrArrSort(ts))? ERRTYPE : ALLCONST) /*| addInfo*/;}
}

/* End DCDECL.C */
