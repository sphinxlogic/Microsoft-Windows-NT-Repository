/* DCDECL.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#include <ctype.h>
#include <stddef.h>
#include <string.h>	/* memcmp used */
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

#ifdef VMS
#pragma noinline (cleverSkipTok)
#pragma noinline (errMsngStati, fctInfo, getINDEXTYPEtype, tstNoNewTyp)
#endif

/* Miscelleanous */
#define BoolHCode	AdaptHCode(0x181C)
#define BoolTName	(Tname)"\4\0bool"
#define ErrDecl1	QuasiNULLval(TdeclList)
#define InfoDcl(wName, xmemberFl, ydeclFl, znoNewTypeFl)		       \
  static TinfoDecl wName = {{  /* 'static' to avoid local compiler	       \
							 gross inefficiency. */\
    0-0		   /*~ zif (! __member(_fldSize))			       \
			    "misplaced initialization for field '_fldSize'" */,\
    NoParQal+0-0   /*~ zif (! __member(_parQal))			       \
			     "misplaced initialization for field '_parQal'" */,\
    xmemberFl      /*~ zif (! __member(_memberFl))			       \
			   "misplaced initialization for field '_memberFl'" */,\
    ydeclFl        /*~ zif (! __member(_declFl))			       \
			     "misplaced initialization for field '_declFl'" */,\
    (bool)(0-0)    /*~ zif (! __member(_signedInt))			       \
			  "misplaced initialization for field '_signedInt'" */,\
    NoAttrib+0-0   /*~ zif (! __member(_attriba))			       \
			    "misplaced initialization for field '_attriba'" */,\
    (bool)(0-0)	   /*~ zif (! __member(_fldFl))				       \
			      "misplaced initialization for field '_fldFl'" */,\
    znoNewTypeFl   /*~ zif (! __member(_noNewTypeFl))			       \
			"misplaced initialization for field '_noNewTypeFl'" */,\
    (bool)(0-0)	   /*~ zif (! __member(_variousD1))			       \
			  "misplaced initialization for field '_variousD1'" */,\
    (bool)(0-0)||False /*~ zif (! __member(_variousD2))			       \
			  "misplaced initialization for field '_variousD2'" */,\
    /*~ zif ! __member()						       \
		"Structure '" ConvToString(wName) "' not fully initialized" */ \
  }}
#define MainHCode	AdaptHCode(0x1C01)
#define OutsideStrunInit QuasiNULLval(TdeclList)
#define Sgnd		(Tqualif)1
#define StartTagCh	'#'
#define TstBadIndexType	(kindEqv<CCheck && !((kindEqv < NoCheck)	       \
			 ? compatTypeR(x->BndType, y->BndType, VerifCastF)     \
			 : (x->BndType==&defaultIndexTypeElt || compatTypeR(   \
					   x->BndType, y->BndType, NoCheck))))
DefRepresType  /* define function 'represType' */

/* Type definitions */
typedef struct {
  Tname idName;
  ThCode hCode;
  TpTypeElt headType, tailType;
} TcreatType;

struct _TtagVisi;
typedef struct _TtagVisi TtagVisi;
struct _TtagVisi {
  uint braceLvl;
  TsemanElt *pTagId;
  TtagVisi *preced;
};

struct _tQlfdElt {
  TpTypeElt arrQlfdTypes[1<<NbBitsQualif - 1];
  TqlfdElt *bid;  /* useless */
};

/* Function profiles */
static TpTypeElt allocTypeElt(void);
static void accumulateQal(TpcTypeElt, TpcTypeElt), checkEndAccumQal(TpcTypeElt,
  TpcTypeElt), checkWrngHdrFile(const TsemanElt */*~MayModify*/), cleverSkipTok(
  void), computeSize(TpTypeElt), declObj(TdeclList), defFunc(TdeclList),
  declStrun(TsemanElt *, TtypeSort, Tname), declStrunU(TsemanElt *, TtypeSort,
  Tname), defTag(TsemanElt *, Tname), errMsngStati(Tname), freeDeclList(
  TdeclList), init(TpTypeElt, bool, bool, TmacExpNb), initDefTag(void),
  initOrSizFld(TdeclElt *, TsemanElt *), nbBits(TdeclElt *),
  processFctDPrags(TdeclList);
static Tattrib attrib(void);
static TdeclElt *allocDeclElt(void);
static TundfTagElt *allocUndfTagElt(void);
static TdeclList createDLElt(TcreatType /*~MayModify*/, TinfoDecl, TpTypeElt,
  Tqualif), decl1(TinfoDecl), decl3(TdeclList, TpTypeElt, Tqualif),
  freeDeclElt(TdeclList);
static Tqualif arrQal(TpcTypeElt, Tqualif), typeQualif(void);
static TcreatType declarator(TinfoDecl);
static TsemanElt *enterEnumTagName(Tname, TinfoSeman), *errAlrdDefId(Tname,
  TpTypeElt, Tkind);
static TtagVisi *allocTagListElt(void), *freeTagListElt(TtagVisi *);
static TpTypeElt nativeType(void);
static bool compatTypeR(TpTypeElt, TpTypeElt, TkTypeEquiv), rowUp(TpTypeElt,
  TpTypeElt), sameLgtDList(const TdeclElt *, const TdeclElt *),
  sameQal(TpcTypeElt, TpcTypeElt), tstNoNewTyp(TinfoDecl),
  tstQlfdVariants(TpcTypeElt, TpcTypeElt, bool);
static TqlfdElt *allocQlfdElt(void), *freeQlfdElt(TqlfdElt *);
static int depth(TpcTypeElt) /*~Utility*/;

/* Objects declarations */
static TdeclList curInitMember = OutsideStrunInit;
static bool declMayBeIgnd, declMayBeIgnd1;  /* to ignore functions not declared
				in local system header files, but indicated by
				corresponding (general) adjustment files. */
static size_t curInitArrayIndex = ArrLimErr;
static TpcTypeElt curInitArrayBndType = NULL;
static uint igndMacArgNbP1;
static bool msngAttribFl;
static Tname nameIdErrTyp;
InfoDcl(noNewTypeInfo, False /*memberFl*/, False /*declFl*/, True
							       /*noNewTypeFl*/);
static bool signSpecSeen;
static const TtypeElt valInitTypeElt = {(TtypeSort)0};  /* all fields either
							   False, NULL, or 0. */
static const Ttok rBraSCol[] = {RBRA, SCOL, EndSTok};
static const Ttok sColLBra[] = {SCOL, LBRA, EndSTok};
static const Ttok sColRBra[] = {SCOL, RBRA, EndSTok};
static const Ttok zRbraSCol[] = {NoSwallowTok, RBRA, SCOL, EndSTok};
static const Ttok zCommaRBraSCol[] = {NoSwallowTok, COMMA, RBRA, SCOL, EndSTok};
static const Ttok zCommaRParSCol[] = {NoSwallowTok, COMMA, RPAR, SCOL, EndSTok};
static const Ttok zCoScEdpRPRB[] = {NoSwallowTok, COMMA, SCOL, ENDDPRAG, RPAR,
								 RBRA, EndSTok};
/* External objects */
bool alrdAnalzdExp = False;
int difQual;
TundfTagElt *headUndfTagsList;
bool heedRootType = True;
TsemanElt *pIdInit;  /* identifier being initialized */
Tname rowXName, rowYName;

void prog(void)
{
  GetNxtTok();  /* get first token */
  if (IsDP(DOLLSIGN)) {  /* if this d-pragma */
    dollAlwdInId = True;
    GetNxtTok();}  /* must be called AFTER dollAlwdInId set */
  do {
    decl(D_LEGAL, sColLBra);
  } while (curTok.tok != ENDPROG);
  spaceCount = nestLvl*indentIncr;
  checkIndent();  /* to get possible 'scndMsg' */
}

void decl(DeclStatus declAlwd, const Ttok stopSkipToks[])
{
  TdeclList declList;
  Tattrib curAttrib;
  Tstring fileAtBegDecl = curFileName;
  TlineNb lineAtBegDecl = lineNb;
  InfoDcl(attribAndFalseFl, False /*memberFl*/, True /*declFl*/,
							 False /*noNewTypeFl*/);

  checkIndent();
  attribAndFalseFl.s11._attriba = curAttrib = attrib();
  declMayBeIgnd = declMayBeIgnd1 = (adjustFile && InsideInterval(curAttrib,
								 Extrn, Typdf));
  nameIdErrTyp = NULL;
  declList = decl1(attribAndFalseFl);
  if (declList != NULL) {  /* not function definition, nor 'VerySpecialRule' */
    if (declList == ErrDecl1) skipTok(stopSkipToks);
    else {
      do {
        if (declAlwd>D_LEGAL && declList->InitFl) err0(IlgInit1|Warn3|PossErr);
      } while ((declList = freeDeclElt(declList)) != NULL);
      if (insideHdrFile) { if (curAttrib == Stati /* else error detected in
								    declObj() */
#ifdef VMS
				        && !vmsNoStd
#endif
						    ) err0(ExtrnExptd | Warn2);}
      else if (curAttrib == Extrn) err0(ExtDeclBeInHdrFile|Warn1|Rdbl);
      if (ignoreErr = False, !Found(SCOL)) {
        errWFName(SColExptd1, lineAtBegDecl, fileAtBegDecl, NULL, NULL,
			    (fileAtBegDecl == curFileName)? errTxt[At0] : NULL);
        if (curTok.tok==IDENT && pDescTokId==NULL) {  /* may have been declared
								  in between. */
          curTok.PtrSem = searchSymTabHC(curTok.IdName, curTok.Hcod);
          if (curTok.PtrSem != NULL) pDescTokId = curTok.PtrSem->PdscId;}}}}
}

static Tattrib attrib(void)
{
  Tattrib w;

  w = (Tattrib) (curTok.tok - BAttrib);
  if ((uint)w <= (uint)(EAttrib - 1 - BAttrib)) {  /* if found legal attribute,
		 absorb token, and absorb any other (useless) 'attrib' token. */
    if (w==Stati && nestLvl!=0) w = StatiL;
    while (InsideInterval(NxtTok(), BAttrib, EAttrib - 1)) err0(
							    MoreThanOneAttrib);}
  else w = NoAttrib;
  return w;
}

static Tqualif typeQualif(void)
{
  Tqualif curQual, res = NoQualif;

  for (;; GetNxtTok()) {
    switch (curTok.tok) {
      case CONST: curQual = ConstQal; break;
      case VOLAT: curQual = VolatQal; break;
      default: return res;}
    if (res & curQual) err0(AlrdQual);
    res |= curQual;}
}

static TdeclList decl1(TinfoDecl infoDcl)
/* May return NULL (declaration of tag alone, or definition of function) */
{
  Tname tagName = NULL, tagName1 = NULL;
  TpTypeElt type;
  Tqualif qual;
  TsemanElt *ptrTagId;
  TtypeSort typS;

  qual = typeQualif();
  if ((qual & VolatQal)!=NoQualif && infoDcl.s11._attriba==Reg) err0(
						 RegVolatIncptbl|Warn2|PossErr);
  if (InsideInterval(curTok.tok, BNatTyp, SGND)) {
    type = nativeType();
    if (type->typeSort == Int) infoDcl.s11._signedInt = signSpecSeen;}
  else
    switch (curTok.tok) {
    case COLON:		/* in case of bit-field padding */
      if (! sysHdrFile) err0((qual == NoQualif)? BadIntgrlType :
						      BadIntgrlType|Warn1|Rdbl);
      type = NoConstTyp(&natTyp[UIntDpl]);
      break;
    case ENUM:
      if (NxtTok() == IDENT) {
        tagName1 = tagName = storeName(curTok.IdName, TagSpace);
        GetNxtTok();}
      if (curTok.tok == LBRA) {
        TcalcS enumCtr = -1;  /* value of last enum constant */
        TsemanElt *pIdEnumCst = QuasiNULLval(TsemanElt *), *pLastEnumCst = NULL;
        TpTypeElt enumCstType;
        DefSem(defndEnumTag, EnumTag, True /*defnd*/);

        /* It is a new enum, so declare it, that is declare its enum tag */
        if (tstNoNewTyp(infoDcl) || declMayBeIgnd) tagName = NULL; /* => stub */
        ptrTagId = enterEnumTagName(tagName, defndEnumTag);
        type = ptrTagId->type;
        /* Create a type element for associated enum constants */
        enumCstType = allocTypeEltIC(type, NoQualif);
        enumCstType->Shared = True;  /* owned by EnumTag */
        enumCstType->Generiq = True;  /* for enum constants to be usable by
							       derived types. */
        GetNxtTok();  /* here for good error position */
        /* Process enum constants list */
        do {
          Tname enumCName;
          ThCode enumCHCode;
          bool initByPrevECst = False;

          ignoreErr = False;
          if (curTok.tok != IDENT) {err0(IdExptd); skipTok(zCommaRBraSCol);}
          else {
            enumCName = storeName(curTok.IdName, ObjectSpace);
            enumCHCode = curTok.Hcod;
            if (NxtTok() == ASGNOP) {  /* if initialized constant */
              TkAsgn oper;

              if ((oper = (TkAsgn)curTok.Val) != SimplAsgn) err0(
								SimplAsgnExptd);
              GetNxtTok();
              pIdInit = NULL;  /* for managePointers()/modifPtdVal() */
              if (correctExprN(enumCstType, Int, True, txtAsgnTok[oper], False
									    )) {
                enumCtr = cExp.Sval;
                if (cExp.ptrId == pIdEnumCst) initByPrevECst = True;
                goto enterEnumCstL;}}
            if (! computeSigAdd(enumCtr, 1, (TcalcS */*~OddCast*/)&enumCtr)
#if defined(LONGLONG) || ULONG_MAXC>UINT_MAXC
 				   || enumCtr>INT_MAXC
#endif
						      ) err0(Overflow1 | Warn2);
enterEnumCstL:
            if (infoDcl.s11._noNewTypeFl
                || InsideDPragma
                || declMayBeIgnd
                || (pIdEnumCst = enterSymTabHC(enumCName, enumCHCode))==NULL
                   && (pIdEnumCst = errAlrdDefId(enumCName, NULL, EnumCst))==
					   NULL) pIdEnumCst = enterSymTab(NULL);
            {
              /* Chain enum constants */
              register TsemanElt *w = pLastEnumCst, *prevW = NULL;
              TenumCst iEnumCtr = (TenumCst)enumCtr;

              while (w!=NULL && iEnumCtr<w->EnumVal) {prevW = w; w = w->
								    NxtEnumCst;}
              if (initByPrevECst) (void)FoundDP(SAMEVAL);
              else if ((w!=NULL && iEnumCtr==w->EnumVal) != FoundDP(SAMEVAL)) {
                const Tchar *pBuf;

                if (w!=NULL && iEnumCtr==w->EnumVal) {
                  /* Search name of (last) same-valued enum constant */
                  const TsemanElt *pId;
                  TnameBuf buf;

                  (void)initGetNxtIdInCurBlk();
                  while (pId = nxtId(), pId->Kind!=EnumCst || pId->EnumVal!=
								    iEnumCtr) {}
                  bufNameToS(curIdName(), buf);
                  pBuf = &buf[0];}
                else pBuf = NULL;
                errWNSS((pBuf == NULL)? WrngSameVal|Warn1 : SameEnumCst|Warn2|
		 PossErr, enumCName, grstIntToS((TgreatestInt)iEnumCtr), pBuf);}
              pIdEnumCst->NxtEnumCst = w;
              if (prevW == NULL) pLastEnumCst = pIdEnumCst;
              else prevW->NxtEnumCst = pIdEnumCst;
              pIdEnumCst->EnumVal = iEnumCtr;}
            {
              DefSem(defndEnumCst, EnumCst, True /*defnd*/);
              DefSem1(notUsedEnumCst, EnumCst, False /*defnd*/, True /*used*/,
							      False /*initlz*/);

#ifdef _AIX /* for compiler does not accept next conditional expression */
              if (FoundDP(NOTUSED)) pIdEnumCst->InfoS = notUsedEnumCst;
              else pIdEnumCst->InfoS = defndEnumCst;
#else
              pIdEnumCst->InfoS = (FoundDP(NOTUSED))? notUsedEnumCst :
								   defndEnumCst;
#endif
            }
            if (insideHdrFile) {
              pIdEnumCst->Attribb = Extrn;
              if (sysAdjHdrFile) pIdEnumCst->SysElt = True;}
            pIdEnumCst->type = enumCstType;
            pIdEnumCst->defLineNb = lineNb;         /* record              */
            pIdEnumCst->defFileName = curFileName;} /*        birth place. */
        } while(Found(COMMA));
        type->FrstEnumCst = pLastEnumCst;
        if (pLastEnumCst == NULL) {  /* no enum constant found */
          enumCstType->Shared = False;
          (void)freeTypeElt(enumCstType);}
        else {
          enumCstType->FrstEnumCst = pLastEnumCst;
          type->size = IntSiz;  /* *doIt*, because standard says enum */
          type->Algn = Alig0;}  /* representation can be char<->int.  */
        if (sysAdjHdrFile && tagName1==NULL && infoDcl.s11._attriba!=Typdf)
          type->IntPoss = True;  /* 'enum' cst usable as 'int'. */
        if (! Found(RBRA)) {errWS(CommaOrXExptd, "}"); cleverSkipTok();}
        goto tstDeclaratorL;}
      {	/* Here if no left brace seen ; is tag already defined ? */
        DefSem(notDefndEnumTag, EnumTag, False /*defnd*/);

        if (tagName == NULL) err0(UndefEnum);
        else if ((ptrTagId = searchSymTab(tagName)) != NULL) {
          if (ptrTagId->Kind == EnumTag) goto tagProcessdL;}
        else errWN(NotDefTag, tagName);
        ptrTagId = enterEnumTagName(tagName, notDefndEnumTag);}  /* if unknown
						       enumTag, declare it... */
tagProcessdL:
      type = ptrTagId->type;
      /* "if (! InsideDPragma)" test removed because tag can be used only in
			      d-pragma, e.g. IndexType */ ptrTagId->Used = True;
      if (ptrTagId->Undf) errWN(UndfndId|Warn1|Rdbl,ptrTagId->PdscId->idName);
      break;
    case IDENT:
      if (adjustFile && isSameName(curTok.Hcod, curTok.IdName, BoolHCode,
					       BoolTName)) type = &boolTypeElt1;
      else {
        TsemanElt *ptrId = curTok.PtrSem;

        if (ptrId==NULL || ptrId->Kind!=Type) {
          Ttok peep;
          char frstCharOfIdent = (char)*(curTok.IdName + LgtHdrId);

          if (infoDcl.s11._prioToCast
              || adjustFile
              || (peep = peepNxtTok(),
		  peep==IDENT || peep==STAR || peep==LPAR && infoDcl.s11.
			    _attriba==Typdf)) {  /* should be type identifier */
            Tname w;

            if (ptrId == NULL) {
              if ((ptrId = searchSymTab(w = storeName(curTok.IdName, TagSpace)
				/* do not modify curTok.IdName, which may be in
				 macro storage, and so, remanent */)) != NULL) {
                errWN(TagQM, w);
                goto identL;}
              else if (declMayBeIgnd) {
                if ((int)declMayBeIgnd > 0) nameIdErrTyp = w;}
              else {
                DefSem1(artifDef, Type, True /*defnd*/, True /*used*/,
							      False /*initlz*/);

                errWN(UndeclId, w);
                if (!chkTypName || isupper(frstCharOfIdent) || frstCharOfIdent
					      =='_') (void)defineId(artifDef);}}
            else if (ptrId->type != NULL) errWN(NotTypeId, curTok.IdName);
            type = NULL;}
          else goto forceIntTypeL;}
        else
identL: {
          type = ptrId->NamedType;
          if (! InsideDPragma) ptrId->Used = True;}}
      GetNxtTok();
      break;
    case LBRA:
      if (qual==NoQualif && infoDcl.s11._declFl && infoDcl.s11._attriba
								   ==NoAttrib) {
        type = NoConstTyp(&natTyp[IntDpl]);  /* to handle 'cleanly blocks
					    declared (wrongly) at level zero. */
        break;}
      err0(MsngStrunEnum);
      typS = Struct;  /* could be Union (or Enum); just to have dcc behave the
						    less stupidly possible... */
      goto strunTrmtL;
    case STRUCT: GetNxtTok(); typS = Struct; goto strunTrmtL;
    case UNION: GetNxtTok(); typS = Union;
strunTrmtL: {
        TdeclList membList = NULL, lastEltMembList;

        if (curTok.tok == IDENT) {
          tagName1 = tagName = storeName(curTok.IdName, TagSpace);
          GetNxtTok();}
        if (curTok.tok == LBRA) {/* if left brace seen, definitely a new type */
          static uint braceLvl = 0;  /* +/- 1 at each strun opening/closing
							  brace inside strun. */
          static TtagVisi *pTagList = NULL;  /* head of list of strun tags to
						       be rendered invisible. */

          if (tstNoNewTyp(infoDcl)) tagName = NULL;  /* => stub */
          if (declMayBeIgnd /* adjustment file */ && tagName!=NULL) {
            ptrTagId = searchSymTab(tagName);
            if (ptrTagId!=NULL && ptrTagId->SysElt) goto strunAlrdDecldL;  /*
							  keep same semanElt. */
            nameIdErrTyp = tagName;
            tagName = NULL;}  /* make it stub */
          for (; (ptrTagId = enterSymTab(tagName)) == NULL; tagName = NULL) {
            /* Already defined tag; is it compatible ? */
            ptrTagId = searchSymTab(tagName);
            if (ptrTagId->type->typeSort==typS && !ptrTagId->Defnd) {
              /* Declared but not defined ('Very Special Rule') */
              if (ptrTagId->ListAlwdFiles!=NULL && !isFNameVisible(curFileName,
			  ptrTagId)) errId1(TagShdBeDefIn|Warn1|Rdbl, ptrTagId,
						    lastPrivFileName(ptrTagId));
              ptrTagId->defLineNb = lineNb;         /* record               */
              ptrTagId->defFileName = curFileName;  /*    definition place. */
              goto strunAlrdDecldL;}
            if (errAlrdDefId(tagName, NULL, StrunTag) != NULL) break;}
          declStrun(ptrTagId, typS, tagName);  /* uncomplete strun as yet */
strunAlrdDecldL:
          GetNxtTok();	/* here for good error position */
          ptrTagId->ListAlwdFiles = headListPrivTo;
          /* Explore member list */
          braceLvl++;
          while (curTok.tok!=RBRA && curTok.tok!=ENDPROG) {
            TdeclList newMembList, w;
            const TdeclElt *w1;
            InfoDcl(memberInfo, True /*memberFl*/, False /*declFl*/,
							 False /*noNewTypeFl*/);

            memberInfo.s11._attriba = attrib();
            newMembList = decl1(memberInfo);
            if (newMembList == ErrDecl1) {skipTok(sColRBra); continue;}
            if (ignoreErr = False, curTok.tok != SCOL) errMsngSCol();
            if (memberInfo.s11._attriba != NoAttrib) err0(NoAttribAlwd);  /*
				checked here for deceitless error position... */
            if (newMembList == NULL) err0(FctDeclDefNotAlwd);
            else {
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
                lastEltMembList->cdr = NULL;}
              if (ignoreErr = False, !Found(SCOL)) skipTok(sColRBra);}}
          /* Define corresponding baseType (no more incomplete) */
          for (;;) {  /* mock loop, just to avoid a backward branch (see the
								 'continue'). */
            type = ptrTagId->type;
            if (ptrTagId->Defnd) {
              if (! declMayBeIgnd) errId0(DblDefStrun, ptrTagId);  /* cannot be
					checked before, because tag may be
					defined by a member of the strun... */
              else if (sameLgtDList(type->MemberList, membList)) {
                freeTypes(ptrTagId);
                declStrun(ptrTagId, typS, tagName);
                continue;}  /* substitute to previous strun, keeping same
								    semanElt. */
              else declMayBeIgnd = declMayBeIgnd1 = (bool)-1;  /* if the propo-
					sed strun is not accepted, do not take
					this line as a declaration. */
              freeDeclList(membList);}
            else {
              size_t size = 0;
              Talgn algn = Alig0;

              type->MemberList = membList;
              if (membList == NULL) err0(EmptyStrun); 
              else {
                /* Compute size of strun (may be a minorand, because of align-
                   ment not taken into account, for some 'zif' expressions to
                   be portable). */
                bool fldBefore = False;

                do {
                  size_t membSize;

                  if (membList->FldFl) {  /* field */
                    if ((membSize = (size_t)membList->FldSize)==0 && membList->
				idName==NULL) size = (size + (CHAR_BITC - 1))/
				  CHAR_BITC*CHAR_BITC;  /* round to next byte */
                    else fldBefore = True;}
                  else {
                    if (fldBefore) {
                      size = ((size + (CHAR_BITC - 1))/CHAR_BITC)*CHAR_BITC;  /*
							  round to next byte. */
                      fldBefore = False;}
                    membSize = sizeOfTypeD(membList->type, IncplType,
						  membList->idName) * CHAR_BITC;
                    if (membList->type!=NULL && membList->type->Algn>algn) algn =
							  membList->type->Algn;}
                  if (typS == Struct) size += membSize;
                  else if (membSize > size) size = membSize;
                  /* Accumulate const/volatile qualifiers */
                  if (membList->type != NULL) {
                    type->SynthQualif |= membList->type->Qualif;
                    if (IsStrun(membList->type)) type->SynthQualif |=
				    BaseStrunType(membList->type)->SynthQualif;}
                  membList = membList->cdr;
                } while (membList != NULL);}
              if ((type->size = (size + (CHAR_BITC - 1)) / CHAR_BITC) == 0) type
		->ErrSiz = True;  /* in case of no correct field type, to
		      prevent sizeOfTypeI() errors, for would be superfluous. */
              type->Algn = algn;
              ptrTagId->Defnd = True;}
            break;}  /* exit mock "for (;;)" loop */
          /* Search if there are tags to be marked 'invisible' (from outside
             this declaration). */
          while ((pTagList != NULL) && (pTagList->braceLvl == braceLvl)) {
            if (! sysHdrFile) pTagList->pTagId->NotVisible = True;
            pTagList = freeTagListElt(pTagList);}
          if (--braceLvl > 0) {
            TtagVisi *w = allocTagListElt();

            w->braceLvl = braceLvl;
            w->pTagId = ptrTagId;
            w->preced = pTagList;
            pTagList = w;}
          GetNxtTok();  /* here for good error position */
          goto tstDeclaratorL;}
        /* Here if no left brace seen */
        if (tagName == NULL) {err0(UndefStrun); return ErrDecl1;}
        if (infoDcl.s11._declFl && curTok.tok==SCOL) {  /* no declarator =>
				'very special rule' (K&R p213) : declaration
				of incompletely specified 'strun'. */
          TpTypeElt w;

          if (infoDcl.s11._attriba != NoAttrib) err0(NoAttribAlwd);
          if (qual != NoQualif) err0(NoQualifAlwd);
          if ((ptrTagId = enterSymTab(tagName))!=NULL
              || (ptrTagId = errAlrdDefId(tagName, ((w = searchSymTab(tagName)->
		type)!=NULL && w->typeSort==typS)? w : NULL    /* to cater to
		  multiple use of the 'very special rule' in the same scope. */,
			  StrunTag))!=NULL) declStrunU(ptrTagId, typS, tagName);
          GetNxtTok();
          return NULL;}
        if ((ptrTagId = searchSymTab(tagName)) != NULL) {
          if (ptrTagId->type->typeSort==typS && !(declMayBeIgnd && !ptrTagId->
						   SysElt)) goto foundStrunTagL;
          if (! declMayBeIgnd) {
            if (errAlrdDefId(tagName, NULL, StrunTag) != NULL) goto
								  declStrunTagL;
            goto makeItStubL;}}
        if ((int)declMayBeIgnd > 0) {nameIdErrTyp = tagName; type = NULL
								       ; break;}
        if (declMayBeIgnd || tstNoNewTyp(infoDcl))
makeItStubL:
          tagName = NULL;
        ptrTagId = enterSymTab(tagName);
declStrunTagL:
        declStrunU(ptrTagId, typS, tagName);  /* if unknown strunTag, declare
									it... */
foundStrunTagL:
        if (ptrTagId->NotVisible) errWN(TagNotVisi|Warn1|Rdbl, tagName);
        goto tagProcessdL /*~BackBranch*/;}
tstDeclaratorL:
      if (curTok.tok==SCOL && tagName1!=NULL && infoDcl.s11._declFl && infoDcl.
				s11._attriba!=Typdf) {  /* no object declared */
        if (infoDcl.s11._attriba != NoAttrib) err0(UslAttrib|UWarn1|Rdbl);
        if (qual != NoQualif) err0(NoQualifAlwd|UWarn1|Rdbl);
        GetNxtTok();
        return NULL;}
      break;
    default:
      if (nestLvl==0 && curTok.tok==RBRA) {curTok.tok = SCOL; err0(ExtraRBra);}
      else if (qual==NoQualif && infoDcl.s11._attriba==NoAttrib && infoDcl.s11.
	_declFl) {if (cUnitFName!=NULL || curTok.tok!=ENDPROG) err0(DeclExptd);}
      else goto forceIntTypeL;
      return ErrDecl1;
forceIntTypeL:
      err0((qual!=NoQualif || infoDcl.s11._attriba!=NoAttrib || infoDcl.s11.
	    _declFl /* because function definition may have an empty baseType
		       and no qualif/attrib. */)
		? TypeExptd|Warn1|Rdbl
		: TypeExptd);
      type = NoConstTyp(&natTyp[IntDpl]);}
  {
    Tqualif postQal = typeQualif();

    if (qual & postQal) err0(AlrdQual);
    else if (qual!=NoQualif && postQal!=NoQualif) err0(GroupQual|Warn1|Rdbl);
    qual |= postQal;
  }
  if (qual!=NoQualif && type!=NULL && IsFct(type)) errWT(UslFctQual|UWarn1|Rdbl,
									  type);
  {
    TdeclList dList = createDLElt(declarator(infoDcl), infoDcl, type, qual);

    if (! infoDcl.s11._noNewTypeFl) dList = decl3(dList, type, qual);
    return dList;}
}

static TpTypeElt nativeType(void)
{
  int codNatTyp = -1;
  bool longSeen = False, shortSeen = False;
#ifdef LONGLONG
  bool longlongSeen;
#endif

  signSpecSeen = False;
  do {
    if (curTok.tok >= USGND) {  /* 'unsigned'/'signed' */
      if (signSpecSeen) goto uslTypSpeL;
      if (shortSeen || longSeen || codNatTyp>=0) err0(AdjectvFrst|Warn1|Rdbl);
      signSpecSeen = True;
      if (curTok.tok == SGND) signSpecSeen |= (bool) SIGN_BIT;
      goto correctL;}
    else if (curTok.tok==SHORT || curTok.tok==LONG) {
      if (shortSeen || longSeen) goto uslTypSpeL;
      if (codNatTyp >= 0) err0(AdjectvFrst|Warn1|Rdbl);
      if (curTok.tok == SHORT) {shortSeen = True; goto correctL;}
      longSeen = True;
      GetNxtTok();
#ifdef LONGLONG
      if (longlongSeen = (curTok.tok == LONG)) GetNxtTok();
#endif
      continue;}
    if (codNatTyp < 0) {codNatTyp = curTok.tok - BNatTyp; goto correctL;}
uslTypSpeL:
    err0(UslTypSpe);
correctL:
    GetNxtTok();
  } while (InsideInterval(curTok.tok, BNatTyp, SGND));
  if (shortSeen) {
    if (codNatTyp<0 || codNatTyp==IntDpl) codNatTyp = ShortDpl;
    else err0(UslTypSpe);}
  else if (longSeen) {
    if (codNatTyp<0 || codNatTyp==IntDpl) codNatTyp =
#ifdef LONGLONG
		(longlongSeen)? LLongDpl :
#endif
					   LongDpl;
    else if (codNatTyp == DoubleDpl) codNatTyp = LongDblDpl;
    else err0(UslTypSpe);}
  if (signSpecSeen) {
    if (codNatTyp < 0) {err0(BadIntgrlType|Warn2|PossErr); codNatTyp = IntDpl;}
    if (InsideInterval(codNatTyp, SByteDpl, LLongDpl)) {
      if ((int)signSpecSeen >= 0) codNatTyp += DeltaTyp;}
    else err0(UslTypSpe);}
  else if (codNatTyp == SByteDpl) codNatTyp = CharDpl;
  return NoConstTyp(&natTyp[codNatTyp]);
}

static void declStrun(TsemanElt *ptrTagId, TtypeSort typS, Tname tagName)
{
  DefSem(notDefndStrunTag, StrunTag, False /*defnd*/);

  ptrTagId->InfoS = notDefndStrunTag;
  ptrTagId->type = allocTypeEltID(typS,NoQualif);
#if NULLNotZero  /* otherwise, done by initialization */
  ptrTagId->type->MemberList = NULL; /* incomplete type */
#endif
  defTag(ptrTagId, tagName);
}

static void declStrunU(TsemanElt *ptrTagId, TtypeSort typS, Tname tagName)
{
  declStrun(ptrTagId, typS, tagName);
  if (insideHdrFile && tagName!=NULL && chkUCTags && !lastPrivFileIsUniq(
						     ptrTagId->ListAlwdFiles)) {
    /* Set up to check that tag will be defined before header file exit */
    TundfTagElt *w = allocUndfTagElt();

    w->pTagId = ptrTagId;
    w->prev = headUndfTagsList;
    headUndfTagsList = w;}
}

static TsemanElt *enterEnumTagName(Tname tagName, TinfoSeman sem)
{
  TsemanElt *ptrTagId;
  TpTypeElt newType;

  ptrTagId = enterSymTab(tagName);  /* named enum or anonymous stub (for
						    later automatic freeing). */
  if (ptrTagId == NULL) {  /* name already seen in this block */
    ptrTagId = searchSymTab(tagName);
    if (ptrTagId->Kind==EnumTag && !ptrTagId->Defnd) {ptrTagId->InfoS = sem
							     ; return ptrTagId;}
    if (errAlrdDefId(tagName, NULL, EnumTag) == NULL) {
      tagName = NULL;
      ptrTagId = enterSymTab(NULL);}}  /* stub */
  ptrTagId->InfoS = sem;
  ptrTagId->type = newType = allocTypeEltID(Enum, NoQualif);
  newType->size = IntSiz;  /* for enum constants */
  newType->Algn = Alig0;  /* will be computed once max-valued constant known. */
  defTag(ptrTagId, tagName);
  return ptrTagId;
}

static TcreatType declarator(TinfoDecl infoDcl)
/* if no identifier declared, 'idName' field == NULL */
{
  TpTypeElt pBefType = NULL, pEndBefType, newType;
  TcreatType curCreat;
  bool leftParenSeen = False;

  while (Found(STAR)) {		/* create 'before' part of type */
    newType = allocTypeEltID(Ptr, typeQualif());
    newType->size = PtrSiz;
    newType->BndType = (IsDP(INDEXTYPE))? getINDEXTYPEtype() :
	(moreIndexTypeChk)? NoConstTyp(&defaultIndexTypeElt) : NULL;
    if (pBefType == NULL) pEndBefType = newType;
    newType->NextTE = pBefType;
    pBefType = newType;}
  if (Found(LPAR)) {
    if ((curTok.tok == IDENT)
        ? curTok.PtrSem!=NULL && curTok.PtrSem->Kind==Type
        : curTok.tok!=STAR && curTok.tok!=LPAR) {
      curCreat.idName = NULL;
      curCreat.headType = NULL;
      leftParenSeen = True;}
    else {
      curCreat = declarator(infoDcl);
      if (! Found(RPAR)) errMsngRPar();}}
  else {
    if (curTok.tok == IDENT) {
      curCreat.idName = storeName(curTok.IdName, ObjectSpace);
      curCreat.hCode = curTok.Hcod;
      {
        const TsemanElt *ptrSem = curTok.PtrSem;
        uint saveIgndSysMacArgNbP1 = igndSysMacArgNbP1;

        GetNxtTok();
        if ((int)declMayBeIgnd>0 && infoDcl.s11._declFl) {
          bool addedFctFl = FoundDP(ADDDFCT);

          if (ptrSem!=NULL && !ptrSem->SysElt
              || ptrSem==NULL && !(saveIgndSysMacArgNbP1!=0 || addedFctFl)
	     ) declMayBeIgnd = (bool)-1;  /* MUST be ignored. */
          else igndMacArgNbP1 = saveIgndSysMacArgNbP1;}}}
    else curCreat.idName = NULL;
    curCreat.headType = NULL;}
  if ((int)declMayBeIgnd>0 && infoDcl.s11._declFl) {
    if (nameIdErrTyp != NULL) {errWN(UndeclId, nameIdErrTyp); nameIdErrTyp =
									  NULL;}
    declMayBeIgnd = False;}
  for (;;) {
    if (leftParenSeen || Found(LPAR)) {	/* function declarator */
      TtypeSort sortFct = Fct;
      TdeclList parList = NULL;

      mainFl = isSameName(curCreat.hCode, curCreat.idName, MainHCode,
							 ConvTname("\4\0main"));
      if (curTok.tok!=RPAR && curTok.tok!=LBRA) {  /* not empty parameter list
	     (LBRA checked just to forgive the sinner that forgets the RPAR). */
        TdeclList endList;
        bool resulPtrSeen = False, sizeofBlSeen = False;

        do {
          TdeclList newPar;
          Tattrib attrb;

          ignoreErr = False;
          if (Found(ELLIP)) {
            if (parList == NULL) err0(AtLeastOnePar);
            else {
              bool typeIsOK = False;

              if (   chkPortbl
                  && !endList->NotUsedFl
                  && (   endList->type!=NULL
                      && (   IsTypeSort(endList->type, Byte|UByte|Short|
								   UShort|Float)
                          || IsChar(endList->type)
                          || IsArrFct(endList->type))
                      || (typeIsOK = True, endList->Attriba==Reg))) errWT(
			BadLastPar | UWarn3, (typeIsOK)? NULL : endList->type);}
            sortFct = VFct;
            break;}
          if ((attrb = attrib())!=NoAttrib && attrb!=Reg) {
            err0(OnlyRegAttrib);			
            attrb = NoAttrib;}
          noNewTypeInfo.s11._attriba = attrb;
          noNewTypeInfo.s11._prioToCast = False;
          newPar = decl1(noNewTypeInfo);
          if (newPar == ErrDecl1) skipTok(zCommaRParSCol);
          else {
            if (attrb == NoAttrib) newPar->Attriba = Auto;
            if (newPar->type!=NULL && IsFct(newPar->type)) errWNTT(
			     PtrOnFctExptd, newPar->idName, newPar->type, NULL);
            if (parList == NULL) {  /* first parameter */
              if (newPar->type!=NULL && newPar->type->typeSort==Void) {
                if (newPar->idName!=NULL || attrb!=NoAttrib || curTok.tok!=RPAR
							   ) err0(VoidNotAlone);
                (void)freeDeclElt(newPar);}
              else parList = newPar;}
            else {
              if (newPar->type!=NULL && newPar->type->typeSort==Void) errWN(
						   VoidNotAlwd, newPar->idName);
              endList->cdr = newPar;}
            endList = newPar;
            {
              TpcTypeElt parType = newPar->type;
              bool sizeOKSeen = False;

              newPar->SavedFl = False;
              newPar->SizeofBlFl = False;  /* other flags already off */
              for (; curTok.tok == DPTOK; GetNxtTok()) {
                Tdprag curDP = (Tdprag)curTok.Val;

                switch (curDP) {
                  TparQal paramQal;

                  case GENERIC:  paramQal = GenericV; goto commonParQalL;
                  case MAYMODIFY: {
                      TresulTstPtd w;

                      if ((w = tstPtd(parType, False)) & NOTALLCONST &&
			  !hereOrAloneInMac()) continue;  /* ignore d-pragma
			   in this case if function declaration inside macro. */
                      if (w & (NOTPTR | NOTALLCONST)) break;
                      newPar->MayModifFl = True;
                      continue;}
                  case NOTUSED:
                    if (newPar->ResulPtrFl) break;
                    newPar->NotUsedFl = True;
                    continue;
                  case RESULPTR: 
                    if (parType!=NULL && !IsPtrArr(parType) || resulPtrSeen ||
						       newPar->NotUsedFl) break;
                    newPar->ResulPtrFl = True;
                    /*~NoBreak*/
                  case RESULTYP: paramQal = ResulTypV; goto commonParQalL;
                  case SAVED:
                    if (parType!=NULL && !IsPtr(parType)) break;
                    newPar->SavedFl = True;
                    continue;
                  case SIZEOFBL:
                    if (parType!=NULL && !(parType->typeSort & Who) ||
							    sizeofBlSeen) break;
                    newPar->SizeofBlFl = True;
                    continue;
                  case SIZEOK:
                    sizeOKSeen = True;
                    if (parType!=NULL && (!IsStrun(parType) || BaseStrunType(
			   parType)->size<=CoefMaxParStrunSize*sizeof(void *))
							      ) errUslDP(curDP);
                    continue;
                  case UTILITY:
                    if (parType!=NULL && !isRepreType(parType)) break;
                    paramQal = UtilityV;
commonParQalL:
                    if (newPar->ParQal == NoParQal) newPar->ParQal = paramQal;
                    /*else*/ if (newPar->ParQal != paramQal) break;
                    continue;
                  default: goto exitLoopL;}
                errIlgDP(curDP);}
exitLoopL:
              if (chkNmdTypParFl
                  && !sysAdjHdrFile
                  && newPar->ParQal==NoParQal
                  && isRepreType(parType)
                  && !mainFl) errWNT(ParRepreType | Warn1, NULL, parType);
              if (newPar->ResulPtrFl) resulPtrSeen = True;
              if (newPar->SizeofBlFl) sizeofBlSeen = True;
              if (!sizeOKSeen && parType!=NULL && IsStrun(parType) &&
			  BaseStrunType(parType)->size>CoefMaxParStrunSize*
					     sizeof(void *) && !sysAdjHdrFile) {
                Tverbo savVerbo = verbose;

                verbose = FullVerbo;
                errWT(BigStrun|Warn1|Effic, parType);
                verbose = savVerbo;}}}
        } while (Found(COMMA));}
      else if (chkEmptParList && !mainFl) err0(MsngVoid | Warn1);
      newType = allocTypeEltID(sortFct, NoQualif); /* Fct or VFct if variadic */
      newType->ParamList = parList;
      if (! Found(RPAR)) errWS((parList == NULL)? Msng :CommaOrXExptd, ")");
      leftParenSeen = False;}
    else if (Found(LSBR)) {	/* array declarator */
      size_t limArr;

      limArr = (curTok.tok!=RSBR && !IsDP(INDEXTYPE))
                ? (correctExprN(NoFreeExpType, WhoEnumBool, True, "[]", False))
                  ? (cExp.Sval==0 || cExp.Sval<0 && cExp.type->typeSort &
									 SigWho)
                    ? (errWS((cExp.ErrEvl || sysAdjHdrFile && cExp.Sval==0)?
		     NoErrMsg : IlgArraySize, grstIntToS(cExp.Sval)), ArrLimErr)
                    : (size_t)cExp.Uval
                  : ArrLimErr
                : (cExp.type = NoConstTyp(&defaultIndexTypeElt),
						 0);  /* if no size specified */
      newType = allocTypeEltID(Array, NoQualif);
      newType->Lim = limArr;
      if (limArr != 0) {
        newType->size = 1U;  /* forbids computation of bound by
						    potential initialization. */
        if (chkArrBnd && cExp.LitCst) {
          Tname nameToShow = curCreat.idName;
          TpcTypeElt w1 = curCreat.headType;

          while (w1 != NULL) {
            if (w1->typeSort != Array) {nameToShow = NULL; break;}
            w1 = w1->NextTE;}
          errWN((cExp.LitCstOutsMac)? BndShdBeNamed|Warn1|Rdbl :
				   NumCstShdBeNamed1|Warn1|Rdbl , nameToShow);}}
      if (IsDP(INDEXTYPE)) {
        TpTypeElt w = getINDEXTYPEtype();

        if (compatType(w ,cExp.type, VerifCastF)) errUslDP(INDEXTYPE);
        FreeExpType(cExp);
        newType->BndType = w;}
      else newType->BndType = (cExp.CstImpsd)
	     ? (FreeExpType(cExp), NoConstTyp(&defaultIndexTypeElt))
	     : cExp.type;
      if (newType->BndType!=NULL && newType->BndType->NoOwner) newType->BndType
							      ->NoOwner = False;
      if (! Found(RSBR)) errWS(Msng, "]");}
    else break;  /* exit "for(;;)" */
    if (curCreat.headType == NULL) curCreat.headType = newType;
    else curCreat.tailType->NextTE = newType;
    curCreat.tailType = newType;}
  /* splice eventual front end of type ('pointer' part) */
  if (pBefType != NULL) {
    if (curCreat.headType == NULL) curCreat.headType = pBefType;
    else curCreat.tailType->NextTE = pBefType;
    curCreat.tailType = pEndBefType;}
  return curCreat;
}

static TdeclList decl3(TdeclList x, TpTypeElt baseType, Tqualif qual)
{
  processFctDPrags(x);
  if (curTok.tok == LBRA) {defFunc(x); return NULL;}  /* function definition */
  {
    TdeclList endDL;
    TinfoDecl localIDL = x->InfoD;

    msngAttribFl = False;  /* this flag to prevent several instances of same
								       error. */
    endDL = x;  /* end of declaration list = beginning as of now */
    ((TinfoDecl1 * /*~OddCast*/)&localIDL)->s11._flags1 = 0;  /* reset these
								       flags. */
    localIDL.s11._declFl = False;  /* because of MsngObjTypName management */
    for (;;) {
      if (nestLvl!=0 && endDL->Attriba==NoAttrib && (endDL->type==NULL ||
				    !IsFct(endDL->type))) endDL->Attriba = Auto;
      if (x->DeclFl) {
        declObj(endDL);  /* if object to be declared (to be done immediatly,
			     because of case "struct _s a ={..., &a, ...};"). */
        declMayBeIgnd = declMayBeIgnd1;}
      else {
        initOrSizFld(endDL, NULL);
        if (x->InitFl) err0(MemberCantBeInit);}
      if (! Found(COMMA)) break;
      /* Chain current declaration in declaration list */
      endDL->cdr = createDLElt(declarator(x->InfoD), localIDL, baseType, qual);
      endDL = endDL->cdr;
      processFctDPrags(endDL);}}
  return x;
}
/* Code cleaning interrupted here Feb 06/2001 */

static void defFunc(TdeclList x)
{
  TpTypeElt defFctTyp, curFctRetTyp;
  Tname fctName;

  checkIndent();
  /* Search if type = (V)Fct; if not, try to find one in type chain */
  if ((defFctTyp = x->type) != NULL)
    do {
      if (IsFct(defFctTyp)) break;
    } while ((defFctTyp = freeTypeElt(defFctTyp)) != NULL);
  if (defFctTyp != x->type) err0((defFctTyp==NULL && x->idName!=NULL)?
							MsngFctParen : NotAFct);
  if (defFctTyp != NULL) {  /* function type found */
    TsemanElt *ptrIdFct;
    TdeclList defParList;
    Tattrib fctAttrib;
    bool keepDecl = False;  /* try hard not to keep declaration coordinates, to
			get function definition coordinates (otherwise, problem
			to either spot declaration coordinates (eg. if bad type)
			or definition coordinates (eg. in case of double
			definition). */

    if (defFctTyp->Shared) errWN(FctDefViaTpdf, x->idName);
    if ((fctAttrib = x->Attriba) == Extrn) fctAttrib = NoAttrib;
    else if (fctAttrib==NoAttrib || sysHdrFile && fctAttrib==Stati) fctAttrib
									= Extrn;
    if (insideHdrFile) errWS((adjustFile)? NoDefInHdrFile : NoDefInHdrFile|
							      Warn1|Rdbl, NULL);
    if (nestLvl != 0) err0(EmbdFctNotAlwd);
    if ((fctName = x->idName) == NULL) err0(MsngFctName);
    if ((curFctRetTyp = NxtTypElt(defFctTyp))!=NULL && curFctRetTyp->
	     typeSort!=Void && !IsArrFct(curFctRetTyp)) (void)sizeOfTypeD(
					   curFctRetTyp, IncplRetType, fctName);
    if (fctName!=NULL && (ptrIdFct = searchSymTabHC(fctName, x->hCode))!=
		      NULL) {  /* already existing identifier; is it correct? */
      if (ptrIdFct->type != NULL) {
        if (IsFct(ptrIdFct->type) && !ptrIdFct->Defnd) {
          /* Correct: now, check concordance */
          const TpTypeElt declFctTyp = ptrIdFct->type, declRetType =
							  NxtTypElt(declFctTyp);
          TdeclList declParList;

          if (ptrIdFct->Attribb != fctAttrib) errId1(IncptblAttrib|Warn2|
			    PossErr, ptrIdFct, (ptrIdFct->Attribb == Extrn)?
						    errTxt[Empty] : "'static'");
          if (declFctTyp->typeSort != defFctTyp->typeSort) {errId0(VariNotVari,
						    ptrIdFct); keepDecl = True;}
          if (! CompatType(declRetType, curFctRetTyp, StrictChk)) {
            errId2((compatType(curFctRetTyp, declRetType, CStrictChk))?
		 NotSameType|Warn2|PossErr : NotSameType, ptrIdFct, typeToS1(
					  curFctRetTyp), typeToS2(declRetType));
            keepDecl = True;}
          for (declParList = declFctTyp->ParamList,
					      defParList = defFctTyp->ParamList;
               declParList!=NULL && defParList!=NULL; 
                 declParList = declParList->cdr, defParList = defParList->cdr) {
            if (defParList->idName!=NULL && declParList->idName!=NULL &&
		memcmp(declParList->idName, defParList->idName, FullLgt(
						      defParList->idName))!=0) {
              TnameBuf buf;

              bufNameToS(defParList->idName, buf);
              errId1(ParNamesNotEq|Warn1|Rdbl, ptrIdFct, buf);}
            if (! CompatType(declParList->type, defParList->type, StrictChk)) {
              errId((compatType(declParList->type, defParList->type, 
		CStrictChk))? ParTypesNotEq|Warn2|PossErr : ParTypesNotEq,
		  ptrIdFct, defParList->idName, typeToS1(declParList->type),
						    typeToS2(defParList->type));
              keepDecl = True;}
            if (declParList->FlagsD != defParList->FlagsD) {
              errId0(NonCoheDPInFct | Warn2, ptrIdFct);
              keepDecl = True;}}
          if ((defParList!=NULL || declParList!=NULL) && (!sysHdrFile ||
		declFctTyp->ParamList!=NULL && defFctTyp->ParamList!=NULL) /*
		in system header files, empty parameter list => no check ? */) {
            errId0(NotSameNbOfPar, ptrIdFct);
            keepDecl = True;}
          if (defFctTyp->FlagsT != declFctTyp->FlagsT) {
            errId0(NonCoheDPInFct | Warn2, ptrIdFct);
            keepDecl = True;}
          if (fctAttrib == Extrn) checkWrngHdrFile(ptrIdFct);
          ptrIdFct->Defnd = True;
          if (! keepDecl) {
            freeTypeChain(declFctTyp);
            goto commonL;}}
        else (void)errAlrdDefId(fctName, defFctTyp, Obj);
        ptrIdFct = enterSymTab(NULL);}  /* stub (to manage correctly following
							      function body). */
      if (! mainFl) fctAttrib = Stati;}
    else {  /* yet undeclared identifier: declare it as a function */
      if (InsideInterval(x->Attriba, Extrn, Reg)) {errWN(IlgAttribForFct,
						   fctName); fctAttrib = Stati;}
      ptrIdFct = enterSymTabHC(fctName, x->hCode);}
    {
      DefSem1(defndObj, Obj, True /*defnd*/, False /*used*/, True /*initlz*/);

      ptrIdFct->InfoS = defndObj;}
    if (sysAdjHdrFile) ptrIdFct->SysElt = True;
    if ((ptrIdFct->Attribb = fctAttrib)==Extrn && !mainFl && !insideHdrFile &&
					      nestLvl==0) errMsngStati(fctName);
commonL:
    ptrIdFct->defLineNb = lineNb;		/* record              */
    ptrIdFct->defFileName = curFileName;	/*        birth place. */
    ptrIdFct->type = defFctTyp;
    /* Check for 'main' special treatment */
    if (mainFl) {  /* verify that legal form */
      register const TdeclElt *declParList = ptrIdFct->type->ParamList;

      if (fctAttrib != Extrn) err0(BadAttribForMain);
      if (curFctRetTyp!=NULL && curFctRetTyp->typeSort!=Int) err0(
							     BadRetTypeForMain);
      if (declParList != NULL) {
        if (declParList->type!=NULL && !(declParList->type->typeSort & WhoEnum)
		) errWNT(BadParForMain, declParList->idName, declParList->type);
        if ((declParList = declParList->cdr)!=NULL && declParList->type!=NULL) {
          scndMainParTypeElt.BndType = declParList->type->BndType;  /* for
					 no false error due to type of bound. */
          if (! compatType(declParList->type, &scndMainParTypeElt, FctCall)
		) errWNT(BadParForMain, declParList->idName, declParList->type);
          if (declParList->cdr!=NULL && declParList->cdr->cdr!=NULL) err0(
							  TooManyParForMain);}}}
    /* Enter parameters into symbol table */
    enterBlock();	/* so parameters are defined at function body level */
    for (defParList = defFctTyp->ParamList; defParList != NULL; defParList =
							     defParList->cdr) {
      TsemanElt *ptrId;
      TpTypeElt curType = defParList->type;

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
        if (defParList->NotUsedFl) ptrId->InfoS = notUsedPar;
        else ptrId->InfoS = defndPar;
#else
        ptrId->InfoS = (defParList->NotUsedFl)? notUsedPar : defndPar;
#endif
        if (defParList->MayModifFl) ptrId->MayModif = True;
        if (defParList->ResulPtrFl) ptrId->ResulPtr = True;
        if (defParList->SavedFl) ptrId->Saved = True;
        ptrId->type = curType;
        ptrId->Attribb = (curType!=NULL && IsArr(curType))? NoAttrib
			/* so that no LocalAdr d-pragma needed for &par[...] */
				     : defParList->Attriba;  /* auto/register */
/*        if (curType != NULL)
          if (curType->typeSort==Bool) ptrId->NotPureBoo = True;
	  * if so done, warning in 'constPtd()', on 'resultIfNULL' parameter *
          else */
        ptrId->defLineNb = lineNb;		/* record              */
        ptrId->defFileName = curFileName;}}}	/*        birth place. */
  else {fctName = NULL; enterBlock();}
  (void)freeDeclElt(x);
  {
    Tname oldCurFctName = curFctName;  /* necessary because of possible
					    (erroneously) embedded functions. */

    curFctName = fctName;
    enterFctBody(defFctTyp);
    curFctName = oldCurFctName;}
}

static void initOrSizFld(TdeclElt *x, TsemanElt *ptrId)
{
  if (Found(COLON)) nbBits(x);
  else {
    TpTypeElt objType = x->type;

    if (x->idName == NULL) err0((x->MemberFl)
				? MsngMembName
				: (x->DeclFl && curTok.tok==SCOL)
				  ? MsngObjTypName|Warn2|PossErr
				  : MsngObjTypName);
    if (curTok.tok == ASGNOP) {
      TmacExpNb locMacExpndNb = curMacExpNb;

      if ((TkAsgn)curTok.Val != SimplAsgn) err0(SimplAsgnExptd);
      paramTxt = txtAsgnTok[(TkAsgn)curTok.Val];
      if (x->Attriba == Typdf) err0(CantInitTpdf);
      else if (objType!=NULL && IsFct(objType)) {err0(FctCantBeInit); objType =
									  NULL;}
      else {
        if (objType!=NULL && IsStrun(objType)) (void)sizeOfTypeD(objType,
							  IncplType, x->idName);
        x->InitFl = True;}
      GetNxtTok();  /* here for good error position */
      {
        bool cmplxInit = (curTok.tok==LBRA || curTok.tok==CSTST);

        if (cmplxInit && objType!=NULL && IsStrun(objType) && ptrId!=NULL
	     ) ptrId->StrunMdfd = False;  /* to allow StrunShdBeConst warning */
        limErroTypes = NULL;  /* forget previous erroneous types */
        pIdInit = ptrId;  /* for correctExprN() */
        init(objType, !InsideInterval(x->Attriba, Extrn, Reg), chkNumCst,
								 locMacExpndNb);
        if (InsideInterval(x->Attriba, Auto, Reg) && objType!=NULL && IsCmpsd(
								     objType)) {
          if (cmplxInit) {
            if (! FoundDP(DYNINIT)) errWN(SlowingInit|Warn1|Effic, x->idName);}
          else if (ptrId != NULL) ptrId->DynInitCtr = NoDynInitWarn;}}}
    else if (ptrId!=NULL && ptrId->Attribb>=Auto && ptrId->type!=NULL &&
		 IsCmpsdObj(ptrId) && FoundDP(DYNINIT)) ptrId->DynInitCtr =
								 NoDynInitWarn;}
}

static void init(TpTypeElt objType, bool cstExp, bool chkIntCst,TmacExpNb macNb)
{
  bool msngBra, simulLBra = False;

  for (;;) {  /* mock loop, to avoid a back branch (see 'continue') */
    msngBra = alrdAnalzdExp;
    if (curTok.tok==LBRA || simulLBra) {
      TtypeSort ts = (objType == NULL)? Void : objType->typeSort;

      if (ts == Array) {
        size_t oldCurIArrayIndex = curInitArrayIndex;
        TpcTypeElt oldCurIArrayBndType = curInitArrayBndType;
        TpTypeElt eltType = NxtTypElt(objType);

        if (! msngBra) GetNxtTok();
        curInitArrayIndex = 0;
        curInitArrayBndType = objType->BndType;
        do {
          init(eltType, True, False, 0);
          curInitArrayIndex++;
        } while (!(msngBra && curInitArrayIndex==objType->Lim) &&
					   curTok.tok==COMMA && NxtTok()!=RBRA);
        if (objType->size == 0) {    /* if size not specified... */
          if (curInitArrayIndex > objType->Lim) objType->Lim =
	       curInitArrayIndex;}  /* then size of array defined by length
			    of longest initializer list (dimensionality > 1). */
        else if (curInitArrayIndex > objType->Lim) err0(TooManyIniz);
        curInitArrayIndex = oldCurIArrayIndex;
        curInitArrayBndType = oldCurIArrayBndType;}
      else if (IsStrunSort(ts)) {
        TdeclList oldCurIMember = curInitMember;
        TmacExpNb locMacExpNb = curMacExpNb;
        bool locLitCst = False;

        if (! msngBra) GetNxtTok();
        if (objType->TagId->ListAlwdFiles  != NULL) checkPrivVisible(objType);
        curInitMember = BaseStrunType(objType)->MemberList;
        while (curInitMember!=NULL && curInitMember->FldFl && curInitMember->
	   idName==NULL) curInitMember = curInitMember->cdr;  /* skip possible
							     initial padding. */
        while (curInitMember != NULL) {
          bool curTokNotLBra = (curTok.tok != LBRA);

          init(curInitMember->type, True, chkIntCst && !curTokNotLBra,
								   locMacExpNb);
          if (curTokNotLBra && cExp.LitCst) locLitCst = chkIntCst;
          do {curInitMember = curInitMember->cdr;} while (curInitMember!=NULL
		&& curInitMember->FldFl && curInitMember->idName==NULL);  /*
								skip padding. */
          if (ts == Union) curInitMember = NULL;
          if (msngBra && curInitMember==NULL || curTok.tok!=COMMA ||
					       NxtTok()==RBRA) goto checkRBraL;}
        alrdAnalzdExp = False;  /* for init() may not have been called again */
        err0(TooManyIniz);
        skipTok(zRbraSCol);
checkRBraL:
        curInitMember = oldCurIMember;
        if (locLitCst && (curMacExpNb==macNb || curMacExpNb!=locMacExpNb)) err0(
						     NotNamedStrunCst | Warn1);}
      else {
        err0(TooManyBraLvl|Warn1|Rdbl);
        GetNxtTok();
        init(objType, cstExp, chkIntCst, macNb);}
      if (!msngBra && !Found(RBRA)) {
        errWS((IsCmpsdSort(ts))? CommaOrXExptd : Exptd, "}");
        cleverSkipTok();}}
    else {
      if (correctExprN(objType, Void, cstExp, paramTxt, chkIntCst)) {
        if (objType!=NULL && IsArr(objType)) {
          if (! cExp.Revlbl) err0(CstExpExptd);
          else {  /* Initialization of character array by string literal */
            size_t lgtStr = cExp.type->size;

            if (objType->size == 0) {  /* limit not specified */
              if (objType->Lim < lgtStr) objType->Lim = lgtStr;}
            else if (lgtStr - 1 > objType->Lim) /* trick! (if erroneous, objTyp
		 ->Lim = -1U => greater than any size) */  err0(TooManyIniz);}}}
      else if (alrdAnalzdExp) {  /* elided '{' ? */
        err0((pIdInit==NULL || objType == pIdInit->type)
			? MsngLBra  /* init() called at level 0 */ 
			: (goodPrgmg)
			  ? MsngLBra|Warn1|Rdbl
			  : NoErrMsg);
        simulLBra = True;
        continue;}}
    break;}
}

static void nbBits(TdeclElt *x)
{
  TpcTypeElt fldType;

  if (! x->MemberFl) err0(FldNotAlwd);
  if (! correctExprN(NULL, WhoEnum, True, ":", chkNumCst)) cExp.Uval = 0;
  /* *doIt* Portability (field wdth < 16 = min sizeof(int) ) */
  else if (cExp.Uval>IntSiz*CHAR_BITC || (x->idName!=NULL &&
			      cExp.Uval==0)) {err0(FldSizIncor); cExp.Uval = 0;}
  x->FldSize = (uint)cExp.Uval;
  x->FldFl = True;
  if ((fldType = x->type) != NULL) {
    if (chkPortbl) 
      if (! (fldType->typeSort & (UInt | Int | Bool))) errWT((fldType->typeSort
		        ==Enum && fldType->TagId!=NULL)? UnsuitFldType | Warn1 :
							UnsuitFldType, fldType);
      else if (x->idName!=NULL && cExp.Uval!=0 && fldType->typeSort==Int &&
	       fldType->SynthQualif!=Sgnd) errWT(IntBitField | UWarn2, fldType);
    if (fldType->Shared) x->type = allocTypeEltIC(fldType, NoQualif);  /* to be
						    able to set 'size' field. */
    x->type->size = 1;}  /* *doIt* size in bits */
}

static void declObj(TdeclList pDeclElt)
{
  TsemanElt *ptrId = NULL;
  TpTypeElt objType = pDeclElt->type;
  Tattrib objAttrib = pDeclElt->Attriba;
  Tname objName;
  bool reDecl = False;
  Tstring declFile = curHdrFName;  /* even for 'extern's (wrongly) declared
				   in body file (=> no ExtObjNotDef warning). */

  if (declMayBeIgnd) goto freeObjTypeL;  /* ignore (re-)declaration (can happen
							in adjustment files). */
  /* Insert new declaration into symbol table */
  if ((objName = pDeclElt->idName) != NULL) { /* correct declar. of an object */
    if (adjustFile && igndMacArgNbP1!=0 && objType!=NULL && objType->typeSort==
									  Fct) {
      /* Check that system declaring macro (now inhibited) has same number of
         parameters as would-be redeclared function. */
      const TdeclElt *w = objType->ParamList;

      while (igndMacArgNbP1--, w != NULL) w = w->cdr;
      if (igndMacArgNbP1 != 0) goto freeObjTypeL;}
    if ((ptrId = enterSymTabHC(objName, pDeclElt->hCode)) == NULL) {  /* alrea-
						      dy declared identifier. */
      if (objAttrib != NoAttrib) {
        ptrId = searchSymTabHC(objName, pDeclElt->hCode);
        if (adjustFile) {
          if (objType==NULL || !InsideInterval(objAttrib, Extrn, Typdf) &&
	  	errAlrdDefId(objName, objType, ptrId->Kind /* because of
		CantChgMnng */)==NULL) goto freeObjTypeL;  /* not typedef nor
				 external object/function type to be amended. */
          if ((objAttrib == Typdf) != (ptrId->Kind == Type)) {
            errWNTT(CantChgMnng, objName, ptrId->type, objType);
            goto freeObjTypeL;}
          if (ptrId->type != NULL) {
            if (objType->typeSort!=ptrId->type->typeSort
		&& !(IsScalar(objType) && IsScalar(ptrId->type))) {
              if (! (IsFct(objType) && IsFct(ptrId->type)|| IsStrun(objType) &&
							  IsStrun(ptrId->type)))
                errWNTT(CantChgMnng, objName, ptrId->type, objType);
              goto freeObjTypeL;}
            if (IsFct(objType) && !sameLgtDList(objType->ParamList, ptrId->type
		->ParamList)) goto freeObjTypeL;}  /* Ignore replacing function
			     prototype if not same sort/number of parameters. */
          if (ptrId->Kind==Type && ptrId->NamedType!=NULL) {
            reDecl = True;
            freeTypeChain(NxtTypElt(ptrId->NamedType));}} /* keep first typeElt,
						       to redirect NamedType. */
        else {
          bool sameFile = !insideHdrFile || ptrId->Kind!=Obj || StrEq(ptrId->
						    DeclaringFile, curHdrFName);

          if (objAttrib==Extrn && ptrId->Attribb==Extrn && !sameFile &&
		ptrId->DeclaringFile!=nonFileName) {  /* 'extern' declarations
			may be duplicated only once, in different header files,
			the last of which being the header file corresponding
			to the current compilation unit. */
            if (! isBodyHdrFile(curHdrFName)) {
              if (! isBodyHdrFile(ptrId->DeclaringFile)) goto redeclL;
              errWNSS(IncldAfter|Warn1|Rdbl, objName, ptrId->DeclaringFile,
								  curHdrFName);}
            errQalTyp1 = NULL;
            if (!CompatType(ptrId->type, objType, ExtDcl) || errQalTyp1!= NULL){
              Tverbo savVerbo = verbose;

              if (verbose < HalfVerbo) verbose = HalfVerbo;
              errId2((compatType(ptrId->type, objType, CStrChkExtDcl))?
			MoreStrngType|Warn2|PossErr : MoreStrngType, ptrId,
				       typeToS1(ptrId->type),typeToS2(objType));
              verbose = savVerbo;
              goto freeL;}
            if (CompatType(ptrId->type, objType, StrictChk)) goto freeL;}
				     /* keep previous declaration coordinates */
          else
redeclL:
            if ((ptrId->Attribb!=Extrn || objAttrib<Auto) && (errAlrdDefId(
		objName, objType, (objAttrib == Typdf)? Type : Obj)==NULL ||
								objType==NULL))
freeL:      {
              if (! sameFile) ptrId->DeclaringFile = nonFileName;  /* see below
									 why. */
              goto freeObjTypeL;}
          declFile = (! sameFile)? nonFileName : ptrId->DeclaringFile;}  /* if
			several declaring files, which one to jot ? =>
			impossible file name (-> no 'ExtObjNotDef' message). */
        freeTypeChain(ptrId->type);}}
    else {  /* new identifier, initialize it */
      DefSem(notDefndObj, Obj, False /*defnd*/);

      ptrId->InfoS = notDefndObj;}
    /* Now, process object according to its attribute */
    if (objAttrib == Typdf) {
      /* Create type chain that will be shared by all users of defined type */
      if ((ptrId->type = objType) == NULL) ptrId->NamedType = NULL;
      else {  /* 'bool' ? */
        TpTypeElt newType = (reDecl)
                  ? ptrId->NamedType /* reuse old typeElt, because it may
						     already be pointed upon. */
                  : allocTypeElt();

        *newType = *objType;
        newType->NoFreeDpdt = True;
        newType->StopFreeing = True;
        newType->Shared = True;
        if (IsTypeSort(newType, NumEnumBool) && isSameName(pDeclElt->hCode,
					       objName, BoolHCode, BoolTName)) {
          if (chkBool) {
            if (newType->typeSort != UInt) err0(IlgBoolDef|Warn1|PossErr);
            newType->typeSort = Bool;}
          else newType->typeSort |= Bool;
          pDeclElt->ParalTypeFl = False;}
        else if (chkTypName && !sysAdjHdrFile) {
          char leadingChar = (char)*(objName + LgtHdrId);

          if (! (isupper(leadingChar) || leadingChar=='_')) errWN(
					    UpCaseTypName|Warn1|Rdbl, objName);}
        if (sysAdjHdrFile) newType->SysTpdf = True;
        else {
          if (! pDeclElt->ParalTypeFl) goto notParalL;  /* description bits
					   stay the same (qualified variant). */
          newType->ParalTyp = True;}
        newType->TypeId = ptrId;
        newType->RootTyp = False;
        newType->QlfdTyp = False;
notParalL:
        for (; curTok.tok == DPTOK; GetNxtTok()) {
          switch ((Tdprag)curTok.Val) {
            case LITCST:
              if (!newType->ParalTyp || !IsTypeSort(newType, Num)) goto
								      errIlgDPL;
              else newType->LitCsta = True;
              break;
            case ROOTTYP:
              if (newType->ParalTyp) {newType->RootTyp = True; break;}
errIlgDPL:
              errIlgDP((Tdprag)curTok.Val);
              break;
            default: goto exitForL;}}  /* exit 'for' loop */
exitForL:
        ptrId->NamedType = newType;}
      {
        DefSem(defndType, Type, True /*defnd*/);
        DefSem1(defndUsedType, Type, True /*defnd*/, True /*used*/, False
								    /*initlz*/);

#ifdef _AIX
        if (insideHdrFile) ptrId->InfoS = defndUsedType; /* for no
						      'UnusedTypeId' message. */
        else ptrId->InfoS = defndType;}
#else
        ptrId->InfoS = (insideHdrFile)? defndUsedType /* for no 'UnusedTypeId'
							message */ : defndType;}
#endif
      if (insideHdrInHdr) ptrId->DeclInInHdr = True;}
    else {		/* not type definition */
      if (objAttrib==NoAttrib && insideHdrFile && !msngAttribFl) {
        errWS(NoDefInHdrFile|Warn2|Rdbl, errTxt[MsngExtTpdf]);
        msngAttribFl = True;}
      /* Verify constraints on declaration/definition, taking specified
         attribute and current block level into account. */
      if (objType!=NULL && IsFct(objType)) {  /* function */
        if (objAttrib == NoAttrib) {
          if (ptrId==NULL && (ptrId = errAlrdDefId(objName, objType, Obj)) ==
							NULL) goto freeObjTypeL;
          if (!insideHdrFile && !msngAttribFl && !mainFl) {
            errWN((nestLvl == 0)? MsngFctAttrib|Warn1|Rdbl :
					    ShdBeInHdrFile|Warn1|Rdbl, objName);
            msngAttribFl = True;}
          objAttrib = (nestLvl==0 && !insideHdrFile && !mainFl)? Stati : Extrn;}
        else if (objAttrib > Extrn) errWN((objAttrib == StatiL)? OnlyExtrn :
							    IlgAttrib, objName);
        ptrId->Initlz = True;
        if (objType->NextTE==&boolTypeElt1) ptrId->NotPureBoo = True;}  /* for
				       pseudo-boolean functions ('feof' etc.) */
      else {		/* object not function */
        if (objAttrib == Extrn) {
/*          if (objType!=NULL && objType->typeSort==Bool) ptrId->NotPureBoo =
			  True;*/}  /* just in case True in other modules ... */
        else {
          if (objAttrib == NoAttrib) {	/* external definition
						    (necessarily at level 0). */
            if (ptrId == NULL) {/* identifier already seen (e.g, declaration) */
              ptrId = searchSymTabHC(objName, pDeclElt->hCode);
              if ((ptrId->Kind!=Obj || ptrId->Defnd) && errAlrdDefId(objName,
				       objType, Obj) == NULL) goto freeObjTypeL;
              checkWrngHdrFile(ptrId);
              if (! CompatType(ptrId->type, objType, StrictChkLsBnd)) {
                errId2((compatType(ptrId->type, objType, CStrChkExtDcl))?
			NotSameType|Warn2|PossErr : NotSameType, ptrId,
				      typeToS1(objType), typeToS2(ptrId->type));
                ptrId->Defnd = True; goto freeObjTypeL;}
              freeTypeChain(ptrId->type);}  /* to be able to replace int[] by
							 int[5], for example. */
            else if (!insideHdrFile && !msngAttribFl) {errMsngStati(objName)
							 ; msngAttribFl = True;}
            objAttrib = Extrn;}
          else if (objAttrib!=Stati && nestLvl==0 || objAttrib==Reg && objType
			    !=NULL && IsFct(objType)) errWN(IlgAttrib, objName);
          ptrId->Defnd = True;}}  /* defined */
      ptrId->DeclaringFile = declFile;
      if ((ptrId->Attribb = objAttrib)==Extrn && insideHdrInHdr) ptrId->
							     DeclInInHdr = True;
      if (insideHdrFile && objAttrib==Stati) ptrId->Used = True;  /* to prevent
								     warning. */
      ptrId->type = objType;}
    ptrId->defLineNb = lineNb;		/* record              */
    ptrId->defFileName = curFileName;	/*        birth place. */
    if (sysAdjHdrFile) ptrId->SysElt = True;
    initOrSizFld(pDeclElt, ptrId);
    ptrId->type = objType = pDeclElt->type;}  /* but ptrId->type must also be
	     set before calling 'initOrSizeFld', because of case "int i = i". */
  else
freeObjTypeL:
  {
    initOrSizFld(pDeclElt, NULL);  /* before pDeclElt->type freeing, to get
	      more interesting messages (for example, no 'NotStrunInit' msg). */
    freeTypeChain(pDeclElt->type);
    objType = NULL;}
  if (pDeclElt->InitFl) {
    computeSize(objType);
    if (ptrId != NULL) {
      if (cExp.LclAdr) ptrId->LclAd = True;  /* does also NotPureBool */
      ptrId->Initlz = True;}
    if (pDeclElt->Attriba /* 'objAttrib' may have been modified */ == Extrn
					  ) errWN(IlgInit|Warn1|Rdbl, objName);}
  if (objType==NULL || !IsFct(objType)) {
    if (FoundDP(NOTUSED) && ptrId!=NULL) {ptrId->Defnd = False; ptrId->Used =
									  True;}
    if (objType!=NULL && pDeclElt->Attriba!=Typdf && !(pDeclElt->Attriba==Extrn
	&& IsArr(objType))) (void)sizeOfTypeD(objType, (pDeclElt->Attriba ==
			 Extrn)? IncplType|Warn2|PossErr : IncplType, objName);}
}

/******************************************************************************/
/*			           UTILITIES				      */
/******************************************************************************/

static AllocXElt(allocDeclElt, TdeclElt, ctrDE, resul->cdr = NULL;)  /* setting
	       'cdr' field to NULL makes element a list (of one element... ). */

static AllocXElt(allocQlfdElt, TqlfdElt, ctrQL, ;)

static AllocXElt(allocTagListElt, TtagVisi, ctrTLE, resul->preced = NULL;)

static AllocXElt(allocTypeElt, TtypeElt, ctrTE, ;)

static AllocXElt(allocUndfTagElt, TundfTagElt, ctrUT, ;)

TpTypeElt allocTypeEltIC(TpcTypeElt x, Tqualif y)
/* New type element initialized by (amended) copy of x */
{
  TpTypeElt resul;

  resul = allocTypeElt();
  *resul = *x;
  resul->Qualif |= y;
  resul->Shared = False;
  resul->NoFreeDpdt = True;  /* because of shallow copy */
  resul->StopFreeing = True;  /* a priori, only the new element is freeable
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
  resul->Qualif = y;
  return resul;
}

bool checkFrstMember(TpcTypeElt x, TpTypeElt y)
{
  const TdeclElt *member;

  if (IsStrun(x))
    for (member = BaseStrunType(x)->MemberList; member != NULL; member =
								  member->cdr) {
      if (CompatType(y, member->type, Asgn) || checkFrstMember(member->type,
								y)) return True;
      if (x->typeSort == Struct) break;}  /* for unions, keep on */
  return False;
}

void checkPrivVisible(TpcTypeElt type)
{
  const TsemanElt *pTag = type->TagId;

  if (   !isFNameVisible(curFileName, pTag)
      && !(InsideMacro && visibleFromMac(pTag))) {
    Tverbo savVerbo = verbose;
    Tstring terseTypeRepre = typeToS1((verbose = Terse /* so that inside of
						      strun not seen */, type));

    verbose = savVerbo;
    errId(PrivNotVisi | Warn3, pTag, NULL, terseTypeRepre, typeToS2(type));}
}

static void checkWrngHdrFile(const TsemanElt *pId /*~MayModify*/)
{
  if (chkInclPos && *pId->DeclaringFile!='\0' /* not nonHdrFName */)
    if (isBodyHdrFile(pId->DeclaringFile)) {
      if (! pId->SysElt) *(TstringNC /*~OddCast*/)(pId->defFileName -
		PosCmpsgFlg) = (Tchar)False;}  /* if one object of a composing
			header is defined in this compilation unit, all must be
			(see next comment). Not done for system objects, to
			allow redefinition of e.g. a single system function. */
    else  /* this 'else' allows a header file (included in a 'right' header file
	     using *~ComposingHdr* d-pragma), to carry a name different from its
	     corresponding body file. */
      if (pId->DeclaringFile == nonFileName) errId1(WrngCUnit|Warn1|Rdbl, pId,
								    cUnitFName);
      else if (! isBodyHdrFile(IndicatedFName(pId->defFileName))) errId2(
		WrngHdrFile|Warn1|Rdbl, pId, cUnitFName, (isBodyHdrFile(
		       skipPath(pId->defFileName)))? errTxt[UseIOption] : NULL);
}

static void cleverSkipTok(void)
{
  if (curTok.tok==SCOL && peepNxtTok()==RBRA) GetNxtTok();  /* for
							   ExtraSColAtEndMac. */
  skipTok(rBraSCol);
}

/* "Local" global variables for managing array qualifiers (that in fact qualify
   arrays elements => propagation (managed this way because of the handling of
   parallel types). */

static bool arrayIsCurrent = False;
static Tqualif accumXQal, accumYQal;

bool compatType(TpTypeElt x, TpTypeElt y, TkTypeEquiv kindEqv)
/* 'x' = left/formal/declaration type; 'y' = right/actual/definition type.
   To be kept coherent with commonType() ("dcexp.c").
   Answers True (False for VerifCastF) if either (sub-)type is NULL.
   ~MayModify because rowUp may add bit 'QlfdTyp'. */
{
  bool notFrstTurn = False, notAllConst;

  arrayIsCurrent = False;
  for (;;) {
    TtypeSort tsx, tsy;

    if (x==NULL || y==NULL) return (kindEqv!=VerifCastF);
    tsx = x->typeSort;
    tsy = y->typeSort;
    if (x->SysTpdf!=y->SysTpdf && tsx&Who && kindEqv<CCheck1) return False;
    if (!(y->Generiq && InsideInterval(kindEqv, PrevCast, FctCall) &&
						NxtIsTypId(x) && rowUp(x, y))) {
      if (NxtIsTypId(y)) {
        if (kindEqv < NoCheck) {  /* VerifCastx/StrictChkx */
          if (x->TypeId != y->TypeId) return False;}
        else if (   !(kindEqv==FctCall && InsideInterval(tsy, Array, VFct) &&
	 			!notFrstTurn) /* because Array => &Array[0]
					     and operator '&' yields generic. */
                 && !rowUp(y, x)
                 && kindEqv<CCheck1 /* !(CCheck1/CCheck/CStrictChk/
					       CStrChkExtDcl) */) return False;}
      else if (x->ParalTyp && (
                      InsideInterval(kindEqv, StrictChkLsBnd, IlgIcp)
                   && !(y->Generiq && rowUp(x, y))
                   && tsy!=Array
                 || InsideInterval(kindEqv, VerifCast, VerifCastF))
								) return False;}
    if (tsx != tsy) {  /* not same type sort */
      if (kindEqv < NoCheck /* VerifCastx/StrictChkx */) return False;
      if (   kindEqv<LitString  /* NoCheck/EqCmp */
          || !(   tsx==Ptr
               && tsy==Array
               && (y->Generiq || kindEqv==FctCall && !notFrstTurn)
          || kindEqv==FctCall
           && tsx==Array
           && tsy==Ptr) /* not (Ptr <- String or, if FctCall, Array <->
								     Ptr). */) {
        /* Accept, for CCheck1 type of equivalence, pointer on numeric type
           to be equivalent to pointer on corresponding unsigned type (gua-
           ranteed by standard to be of same size and alignment). */
        {
          TtypeSort tsc;

          if (! (   kindEqv==CCheck
                 || kindEqv==CCheck1
                  && ((tsc = tsx | tsy)==(Int   | UInt  )
                                 || tsc==(Long  | ULong )
                                 || tsc==(Short | UShort)
                                 || tsc==(Byte  | UByte )
#ifdef LONGLONG
                                 || tsc==(LLong | ULLong)
#endif
                                                         ))) return False;}
        /* Manage C equivalence between all integral types, Ptr <=> &Array[],
							    Fct <=> Ptr/Fct. */
        if (! (tsx & NumEnumBool && tsy & NumEnumBool)) {
          if (IsPtrArrSort(tsx)) {
            if (IsPtrArrSort(tsy)) goto skipTstL;
            if (! IsFctSort(tsy)) return False;
            if ((x = NxtTypElt(x)) == NULL) return True;
            tsx = x->typeSort;}
          else {
            if (!IsFctSort(tsx) || tsy!=Ptr) return False;
            if ((y = NxtTypElt(y)) == NULL) return True;
            tsy = y->typeSort;}
          if (tsx != tsy) return False;
skipTstL:;}}
      if (tsx==Ptr && tsy==Array && y->Qualif!=NoQualif) accumulateQal(x,y);}
    if (tsx >= Enum)
      switch (tsx >> PosDelT) {
      case Enum>>PosDelT:
        if (x->TagId!=y->TagId && kindEqv!=CCheck) return False;
        break;
      case Array>>PosDelT:
        if (!y->Generiq /* not string literal */
            && (   TstBadIndexType
                || (x->Lim!=0 && tsy!=Ptr || kindEqv==StrictChk)
                  && x->Lim!=ArrLimErr
                  && y->Lim!=ArrLimErr
                  && x->Lim!=y->Lim
                  && (kindEqv!=CStrChkExtDcl || y->Lim!=0))) return False;
        if (x->Qualif!=NoQualif || y->Qualif!=NoQualif) accumulateQal(x, y);
        if (! InsideInterval(kindEqv, FctCall, CCheck1)) goto endPtrL;  /*
					    !(FctCall/ExtDcl/IlgIcp/CCheck1). */
        /*~NoBreak*/
      case Ptr>>PosDelT: {
          TpTypeElt nxtX;
          TpcTypeElt nxtY;

          if ((nxtX = NxtTypElt(x))==NULL || (nxtY = NxtTypElt(y))==NULL) break;
          if (x->Generiq && nxtX->typeSort==Void && (kindEqv==LitString ||
		  kindEqv==FctCall) && !(y->Generiq && nxtY->typeSort==Void)
								 ) return False;
          if (InsideInterval(kindEqv, PrevCast, CCheck1)) {/* PrevCast/Asgn/
					       FctCall/ExtDcl/IlgIcp/CCheck1. */
            if (tsx==Array && nxtY->typeSort==Array && nxtX->typeSort!=Array
								 ) return False;
            /* Check pointed element qualifiers, according to C++ Standard */
            {
              Tqualif qualNxtX = (arrayIsCurrent)? arrQal(nxtX, accumXQal) :
								   nxtX->Qualif,
                      qualNxtY = (arrayIsCurrent)? arrQal(nxtY, accumYQal) :
								   nxtY->Qualif;

              if (! notFrstTurn) notAllConst = False;
              if ((((kindEqv == ExtDcl)? qualNxtX & ~qualNxtY : qualNxtY &
				~qualNxtX)!=NoQualif /* compatible qualifiers */
                    || notAllConst && qualNxtX!=qualNxtY)
                  && errQalTyp1==NULL) {  /* keep only first warning */
                static TtypeElt wx, wy;

                errQalTyp1 = (arrayIsCurrent && accumXQal!=NoQualif)? (wx = *x,
					       wx.Qualif |= accumXQal, &wx) : x;
                errQalTyp2 = (arrayIsCurrent && accumYQal!=NoQualif)? (wy = *y,
					    wy.Qualif |= accumYQal, &wy) : y;}
              if (! (qualNxtX & ConstQal) && nxtX->typeSort!=Array /* array
			     always const by itself... */) notAllConst = True;}}
          /* Allow equivalences 'anyType *' => 'void *' and generic 'void *' =>
								 'anyType *'. */
          if (   !InsideInterval(kindEqv, StrictChkLsBnd, NoCheck) && kindEqv!=
			CStrChkExtDcl /* !(NoCheck/StrictChkx/CStrChkExtDcl). */
              && !notFrstTurn
              && (   nxtX->typeSort==Void && (!IsFct(nxtY) || kindEqv==CCheck)
                  || nxtY->typeSort==Void && (y->Generiq || kindEqv==CCheck ||
					       kindEqv==EqCmp && !IsFct(nxtX))))
            return (kindEqv>=NoCheck || ((arrayIsCurrent)
                    ? sameQal(x, y) && sameQal(nxtX, nxtY)
                    : x->Qualif==y->Qualif && nxtX->Qualif==nxtY->Qualif));
          /* Pointer on strun can always be stored in pointer on first member */
          if (InsideInterval(kindEqv, Asgn, FctCall) && checkFrstMember(nxtY,
							    nxtX)) return True;}
        if (TstBadIndexType) return False;
endPtrL:
        notFrstTurn = True;
        break;
      case Fct>>PosDelT: case VFct>>PosDelT: {
          /*TdeclList*/const TdeclElt *wx = x->ParamList, *wy = y->ParamList;

          if (InsideInterval(kindEqv, PrevCast, IlgIcp) && (x->PvNr && !y->PvNr
						   || x->Generiq && !y->Generiq)
	      || kindEqv<NoCheck /* VerifCastx/StrictChkx */ && x->FlagsT!=
							y->FlagsT) return False;
          while (wx!=NULL && wy!=NULL) {
            if (!(InsideInterval(kindEqv, PrevCast, FctCall) && wx->ParQal==
			    GenericV && CompatType(wx->type, wy->type, kindEqv))
                && (   !CompatType(wy->type, wx->type, kindEqv)
		    || wx->ParQal!=wy->ParQal
		     && (    InsideInterval(kindEqv, LitString, IlgIcp)
			  && (   kindEqv==IlgIcp
			      || wx->ParQal!=GenericV)
			 || kindEqv<NoCheck /* VerifCastx/StrictChkx */))
								 ) return False;
            wx = wx->cdr; wy = wy->cdr;}
          if (wx!=NULL && tsy!=VFct || wy!=NULL && tsx!=VFct) return False; 
					     /* not same number of parameters */
          notFrstTurn = False;  /* function result has nothing to do with
							       function call. */
          break;}
      case Struct>>PosDelT: case Union>>PosDelT:
        if (x->TagId != y->TagId) return False;
        break;
      /*~ NoDefault */}
    if (kindEqv<NoCheck /* VerifCastx/StrictChkx */ && ((arrayIsCurrent)?
			   !sameQal(x, y) : x->Qualif!=y->Qualif)) return False;
    if (arrayIsCurrent) checkEndAccumQal(x, y);
    if ((x = NxtTypElt(x))==(y = NxtTypElt(y)) && !arrayIsCurrent) return True;
    /* Stricter check for pointers, arrays ... */
    if (kindEqv == CCheck) kindEqv = CCheck1;}
}

static void accumulateQal(TpcTypeElt x, TpcTypeElt y)
{
  if (! arrayIsCurrent) {
    accumXQal = accumYQal = NoQualif;
    arrayIsCurrent = True;}
  if (IsArr(x)) accumXQal |= x->Qualif;
  if (IsArr(y)) accumYQal |= y->Qualif;
}

static Tqualif arrQal(TpcTypeElt t, Tqualif q)
{
  return (IsArr(t))? NoQualif : t->Qualif | q;
}

static void checkEndAccumQal(TpcTypeElt x, TpcTypeElt y)
{
  if (! IsArr(x)) accumXQal = NoQualif;
  if (! IsArr(y)) accumYQal = NoQualif;
  if (accumXQal==NoQualif && accumYQal==NoQualif) arrayIsCurrent = False;
}

static bool compatTypeR(TpTypeElt x, TpTypeElt y, TkTypeEquiv kindEqv)
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
  TpTypeElt nxtType;

  if (x==NULL || x->Shared || (nxtType = NxtTypElt(x))==NULL) return;
  computeSize(nxtType);
  if (IsArr(x)) x->size = sizeOfTypeD(nxtType, ArrOfIncplOrFctElt, NULL) *	
									 x->Lim;
  else if (IsFct(x) && IsArrFct(nxtType)) errWT(ArrOrFctCantBeRet, nxtType);
}

static TdeclList createDLElt(TcreatType dId /*~MayModify*/, TinfoDecl infoDcl,
						   TpTypeElt type, Tqualif qual)
{
  TdeclList dList;

  if (type != NULL) {
    if (type->Qualif & qual) err0(AlrdQual);
    /* If array (case "typedef int Tarr[]; Tarr a={1,2}, b={3};"), or added
       qualifiers, create copy of type element; copy owned by the declared
       object, so has to be freed when the object disappears. */
    if (qual & ~type->Qualif || type->size==0 && IsArr(type) || infoDcl.s11.
								   _signedInt) {
      TpTypeElt w = allocTypeEltIC(type, qual);

      type = w;
      if (infoDcl.s11._signedInt) w->SynthQualif = Sgnd;}}
  /* Connect baseType and type modifier */
  if (dId.headType != NULL) dId.tailType->NextTE = type;
  else {  /* no modifier */
    dId.headType = type;
    /* Parallel Type ? (no qualifier, no type modifier and type defined by
       typeId). */
    if (qual==NoQualif && !infoDcl.s11._noNewTypeFl) infoDcl.s11._paralTypeFl
									= True;}
  /* Allocate and fill new declList element */
  dList = (TdeclList)allocDeclElt();
  dList->idName = dId.idName;
  dList->hCode = dId.hCode;
  dList->type = dId.headType;	/* beginning of type chain */
  computeSize(dList->type);
  dList->InfoD = infoDcl;
  return dList;
}

TpTypeElt declType(void)
{
  TpTypeElt type;
  TdeclList typeName;

  noNewTypeInfo.s11._prioToCast = True;
  typeName = decl1(noNewTypeInfo);
  if (typeName == ErrDecl1) {skipTok(zCoScEdpRPRB); type = NULL;}
  else {
    if (typeName->idName != NULL) errWN(NoIdAlwdInType, typeName->idName);
    type = typeName->type;
    (void) freeDeclElt(typeName);
    if (type!=NULL && !type->Shared) type->NoOwner = True;}
  return type;
}

TsemanElt *defineId(TinfoSeman infoS)
/* Create a fake typeId to prevent further 'UndeclId' errors. */
{
  TsemanElt *ptrId;

  if (allErrFl) return NULL;
  ptrId = enterSymTabHC(storeName(curTok.IdName, ObjectSpace), curTok.Hcod);
  ptrId->type = NULL;
  ptrId->InfoS = infoS;
  if (infoS.s11._kind == Obj) ptrId->DeclaringFile = curHdrFName;
  else ptrId->NamedType = NULL;
  ptrId->defLineNb = lineNb;		/* record              */
  ptrId->defFileName = curFileName;	/*        birth place. */
  return ptrId;
}

static uint stubCtr = 0;

static void initDefTag(void)
{
  stubCtr = 0;
}

static void defTag(TsemanElt *ptrTagId, Tname x)
{
  ptrTagId->type->TagId = ptrTagId;  /* unique value for each tag */
  ptrTagId->type->Shared = True;  /* tag is only owner of this type element */
  if (x == NULL) {  /* create name only if stub */
    Tchar buf[sizeof(TnbBuf) + LgtHdrId + 1], *ptrBuf = &buf[LgtHdrId + 1];

    bufGrstIntToS((TgreatestInt)++stubCtr, ptrBuf);
    while (*ptrBuf++ != '\0') {;}
    buf[0]= (Tchar)(ptrBuf - &buf[LgtHdrId + 1]);
    buf[LgtHdrId] = StartTagCh;
    ptrTagId->PdscId->idName = storeName(ConvTname(&buf[0]), StubSpace);}
  else ptrTagId->PdscId->idName = x;  /* remember nameString of strun/enum */
  if (insideHdrFile) ptrTagId->Attribb = Extrn;
  ptrTagId->ListAlwdFiles = headListPrivTo;
  ptrTagId->defLineNb = lineNb;	      /* record              */
  ptrTagId->defFileName = curFileName;  /*        birth place. */
  if (sysAdjHdrFile) ptrTagId->SysElt = True;
}

/*~Undef stubCtr */

static int depth(TpcTypeElt x) /*~Utility*/
{
  int result = 0;

  while (x->ParalTyp) {result++; x = x->TypeId->type;}
  return result;
}

static TsemanElt *errAlrdDefId(Tname x, TpTypeElt y, Tkind z)
{
  TsemanElt *ptrId = searchSymTab(x);

  if (ptrId->type==NULL && (ptrId->Kind==z || ptrId->Kind==Obj && z==EnumCst)
					 ) return ptrId;  /* "forced defined" */
  {
    Tverbo savVerbo = verbose;

    verbose = FullVerbo;
    errId((!ptrId->Defnd && y!=NULL && (CompatType(ptrId->type, y, CStrChkExtDcl
	   ) || fctInfo(ptrId)<NOINFO))? AlrdDefId|Warn2|PossErr : AlrdDefId,
				  ptrId, x, typeToS1(ptrId->type), typeToS2(y));
    verbose = savVerbo;}
  return NULL;
}

static void errMsngStati(Tname x)
{
  register Tstring w;

  if (!goodPrgmg || (w = cUnitFName)==NULL) return;
  {
    register TstringNC w1;
    Tstring nameWoutDot, posDot = SearchDot(w);

    nameWoutDot = w1 = allocPermSto((size_t)(posDot - w + 1));
    while (w != posDot) {*w1++ = *w++;}
    *w1 = '\0';
    errWNSS(MsngStati|Warn1|Rdbl, x, nameWoutDot, NULL);
    freeLastPermSto();}
}

TfctInfo fctInfo(TsemanElt *ptrId)
/* Returns <0 (SYSFCT) if function declared by 'system' header file, else see
   definition of enumType TfctInfo in dcdecl.h */
{
  register TpTypeElt curType;

  if (ptrId == NULL) return NOTFCT;
  curType = ptrId->type;
  for (;;) {
    if (curType == NULL) return NOTFCT;
    if (IsFct(curType)) break;
    curType = NxtTypElt(curType);}
  {
    register const TdeclElt *param = curType->ParamList;
    TfctInfo result = (curType==ptrId->type && ptrId->SysElt)? SYSFCT : NOINFO;
    bool voidStarFct = compatType(NxtTypElt(curType), NoConstTyp(
						  &voidPtrTypeElt), VerifCastF);

    while (param != NULL) {
      if (param->ParQal == ResulTypV) result |= RESTYPFCT;
      if (voidStarFct && param->type!=NULL && IsTypeSort(param->type, UInt|
					 ULong|ULLong)) result |= MALLOCLIKEFCT;
      param = param->cdr;}
    return result;}
}

static FreeXElt(freeDeclElt, TdeclList, ctrDE, ;, cdr)

static void freeDeclList(TdeclList x)
{
  while (x != NULL) {freeTypeChain(x->type); x = freeDeclElt(x);}
}

static FreeXElt(freeQlfdElt, TqlfdElt *, ctrQL, ;, bid)

static FreeXElt(freeTagListElt, TtagVisi *, ctrTLE, ;, preced)

void freeTypeChain(TpTypeElt x)
{
  while (x != NULL) {x = freeTypeElt(x);}  /* x may be NULL at the outset
			     (case of undefined Id, or empty BndType, or...). */
}

FreeXElt(freeTypeElt, TpTypeElt, ctrTE, {
	  if (x->Shared) return NULL;  /* stop freeing immediately */
          x->Shared = True;  /* to prevent several freeing of the same typeElt
				(by remaining types pointing on it, because
				otherwise no more protected by 'Shared' bit). */
          if (x->StopFreeing) resul = NULL;  /* stop freeing after this one */
          if (! x->NoFreeDpdt) {
            switch (x->typeSort  >> PosDelT) {
            case Array>>PosDelT: {
              freeTypeChain(x->BndType);  /* BndType may be NULL, so don't
						  use freeTypeElt() directly. */
              break;}
            case Fct>>PosDelT: case VFct>>PosDelT: freeDeclList(x->ParamList);
									  break;
            case Struct>>PosDelT: case Union>>PosDelT: freeDeclList(x->
							     MemberList); break;
            /*~ NoDefault */}}
                                        }, NextTE)

void freeTypes(TsemanElt *x)
/* Beware of case "typedef unknId bool;" (type==NULL, NamedType!=NULL) */
{
  if (! InsideInterval(x->Kind, Obj, StrunTag)) return;  /* Param, EnumCst,
								       Label. */
  if (x->Kind != Obj) {  /* Type, Tags */
    if (x->Kind == Type) {
      TpTypeElt namedType = x->NamedType;

      if (namedType != NULL) {
        if (namedType->QlfdTyp) (void)freeQlfdElt(namedType->PqlfdTypes);
#if Debug
/*        if (! namedType->Shared) sysErr(ExCod5);*/
#endif
        namedType->Shared = False;  /* to allow freeing */
        freeTypeChain(namedType);}}
    else {  /* Tags */
      TpTypeElt tagType = x->type;

      if (x->type != NULL) {
        TsemanElt *pFrstCst;

#if Debug
/*        if (! tagType->Shared) sysErr(ExCod8);*/
#endif
        tagType->Shared = False;  /* to allow freeing */
        if (x->Kind==EnumTag && (pFrstCst = tagType->FrstEnumCst)!=NULL) {  /*
			       free type element associated to enum constant. */
#if Debug
/*          if (! pFrstCst->type->Shared) sysErr(ExCod8);*/
#endif
          pFrstCst->type->Shared = False;  /* to allow freeing */
          (void)freeTypeElt(pFrstCst->type);}}}}
  freeTypeChain(x->type);
}

TpTypeElt getINDEXTYPEtype(void)
{
  TpTypeElt resul;

  resul = (curTok.DpType == NULL)
            ? NULL
            : (curTok.DpType->typeSort & WhoEnumBool)
              ? curTok.DpType
              : (err0(IntgrlTypeExptd | Warn2), FreeExpType1(curTok.DpType),
									  NULL);
  GetNxtTok();
  return resul;
}

void initDecl(void)
{
  initDefTag();
}

bool isRepreType(TpcTypeElt x)
{
  return (x!=NULL && !NxtIsTypId(x) && IsTypeSort(x, Num) && !IsTypeSort(x,
									 Bool));
}

bool isSameName(ThCode xh, Tname xn, ThCode yh, Tname yn)
{
  return (xh==yh && xn!=NULL && memcmp(xn, yn, FullLgt(xn))==0);
}

bool isType(void)
{
  return IsDecl1(BDecl) || curTok.tok==IDENT && curTok.PtrSem!=NULL &&
                                                      curTok.PtrSem->Kind==Type;
}

void makeGeneric(TpTypeElt x)
/* Mark 'generic' non-void function of base type (or type derived from base
   type via qualifier/modifier; in such a case, each level of (pointer) modifier
   is also marked 'generic'). */
{
  TpTypeElt nxtX;
  bool sharedSeen;

  if (x->Shared) return;  /* typedef has already undergone makeGeneric */
  sharedSeen = False;
  while (!NxtIsTypId(x) && (nxtX = x->NextTE)!=NULL && nxtX->typeSort!=Void) {
    if (nxtX->Shared || sharedSeen) {  /* duplicate type chain */
      x->NextTE = nxtX = allocTypeEltIC(nxtX, NoQualif);
      x->StopFreeing = False;
      sharedSeen = True;}
    if (! IsArrFct(nxtX)) nxtX->Generiq = True;
    x = nxtX;}
}

static void processFctDPrags(TdeclList dl)
{
  TpTypeElt x = dl->type, retType;
  bool resultTypeSeen = False, resultTypePoss = False;

  if (   !insideHdrFile
      && (   !(dl->Attriba==NoAttrib && nestLvl==0) && !InsideInterval(dl->
							  Attriba, Extrn, Typdf)
          || dl->MemberFl)
      && chk1stCharVF
#ifdef VMS
      && !vmsNoStd
#endif
      && !InsideMacro) {
    char c;

    if (dl->idName!=NULL && (c = (char)*(dl->idName + LgtHdrId), isupper(c) ||
	 c=='_' && !dl->MemberFl)) errWN(LoCaseObjName|Warn1|Rdbl, dl->idName);}
  while (x != NULL) { /* search whether there is a function in the type chain */
    if (IsFct(x)) {  /* search whether possible ResultType or SizeOfMemBlk
			      parameter compatible with function return type. */
      TdeclElt *parElt;
      uint parCtr;

      retType = NxtTypElt(x);
      for (parElt = x->ParamList, parCtr = 0; parCtr++, parElt != NULL;
							 parElt = parElt->cdr) {
        if (parElt->ParQal == ResulTypV) {
          resultTypeSeen = True;
          if (! CompatType(retType, parElt->type, NoCheck)) {
            errWSTTS(IncohResultType | Warn2, grstIntToS((TgreatestInt)parCtr),
			    parElt->type, retType, dpName((parElt->ResulPtrFl)?
							  RESULPTR : RESULTYP));
            parElt->ParQal = NoParQal;}}
        else if (!sysAdjHdrFile && parElt->ParQal==NoParQal && !parElt->
		    NotUsedFl && CompatType(retType, parElt->type, NoCheck)
							) resultTypePoss = True;
        if (parElt->SizeofBlFl && retType!=NULL && !IsPtr(retType)) {errIlgDP(
					SIZEOFBL); parElt->SizeofBlFl = False;}}
      goto foundFctL;}
    x = NxtTypElt(x);}
  return;  /* no function found */
foundFctL:
  if (resultTypePoss && !resultTypeSeen) x->ResTypPoss = True;
  {
    bool pvNrSeen = False, genUtilSeen = False;

    for (; curTok.tok == DPTOK; GetNxtTok()) {
      Tdprag curDP = (Tdprag)curTok.Val;

      switch (curDP) {
        case PSEUDOVOID: case NEVRET:
          if (pvNrSeen) break;
          pvNrSeen = True;
          if (retType!=NULL && (retType->typeSort == Void)==(curDP==
							     PSEUDOVOID)) break;
          x->PvNr = True;
          continue;
        case UTILITY:
          if (retType!=NULL && !isRepreType(retType)) break;
          /*~NoBreak*/
        case GENERIC:
          if (genUtilSeen) break;
          genUtilSeen = True;
          if (retType!=NULL && retType->typeSort==Void) break;
          if (resultTypeSeen) errWS(CantBeGeneUtil | Warn2, dpName(curDP));
          else if (curDP == GENERIC) x->Generiq = True;
          continue;
        /*~NoDefault*/}
      errIlgDP(curDP);}
    if (x->Generiq) makeGeneric(x);
    else /* because of possible typedef used to define function */
      if (chkFctRetTyp && !genUtilSeen && !resultTypeSeen && !mainFl &&
		   isRepreType(retType)) errWT(RetRepreType | UWarn2, retType);}
}

void procExtent(void)
{
  TsemanElt *pSem = curTok.PtrSem;

  cExp.Sval = 0;
  if ((pSem != NULL)
       ? pSem->Kind != Type
       : (pSem = searchSymTab(storeName(curTok.IdName, TagSpace)))==NULL ||
		    pSem->Kind!=EnumTag) errWN(NotEnumTypeOrTag, curTok.IdName);
  else {
    TpcTypeElt idType;

    if ((idType = pSem->type) != NULL) {
      register const TsemanElt *w;

      if ((w = idType->FrstEnumCst) == NULL) {
        if (idType->TagId == NULL) cExp.Sval = UCHAR_MAXC;}
      else {
        cExp.Sval = (TcalcS)w->EnumVal;
        while (w->NxtEnumCst != NULL) {w = w->NxtEnumCst;}
        cExp.Sval -= (TcalcS)w->EnumVal;}}
    pSem->Used = True;}
  cExp.type = NoConstTyp(&intCstTypeElt);
}

void procIndex(void)
{
  if (curInitArrayIndex == ArrLimErr) {err0(NotArrInit); cExp.ErrEvl = True;}
  cExp.Uval = (TcalcU)curInitArrayIndex;
  cExp.type = NoConstTyp(curInitArrayBndType);
}

static void procMember(void)
{
  if (curInitMember == OutsideStrunInit) {err0(NotStrunInit); cExp.ErrEvl =
									  True;}
  cExp.type = &boolCstTypeElt;
}

void procMember1(void)
{
  cExp.Uval = (TcalcU)(GtPtr(curInitMember, OutsideStrunInit) && isSameName(
      curInitMember->hCode, curInitMember->idName, curTok.Hcod, curTok.IdName));
  procMember();
}

void procMember2(void)
{
  cExp.Uval = (TcalcU)(curInitMember==NULL);
  procMember();
}

static bool rowUp(TpTypeElt x, TpTypeElt y)
/* Returns True if 'y' is a (reachable if heedRootType True) ancestor of 'x' */
{
  TpcTypeElt curX = x, curY;

  do {
    const TsemanElt *xTypId;

    if ((xTypId = curX->TypeId) == y->TypeId) return True;/* ancestor reached */
    if (xTypId->NamedType->QlfdTyp) {
      const TpcTypeElt *w = &xTypId->NamedType->PqlfdTypes->arrQlfdTypes[0];

      do {
        if (tstQlfdVariants(*w++, y, False)) return True;
      } while (*w != NULL);}
    if (curX->RootTyp && heedRootType) {  /* "RootType" (non representation
								 type) found. */

      if (y->typeSort==Ptr && !NxtIsTypId(y) && ((curY = y->NextTE)==NULL ||
	     curY->typeSort==Void)) return True;  /* 'void*' always reachable */
      goto exitL;}
    curX = Parent(curX);  /* climb hierarchy */
  } while (NxtIsTypId(curX));
  /* Representation type reached; OK if 'y' non-parallel type */
  if (! y->ParalTyp) return True;
exitL:
  /* Find out if x and y are of (pseudo-)parallel types, that is descendants of
     differently qualified same types. */
  {
    int difDepth;

    if (! (y->ParalTyp && IsPtr(y)) || (difDepth = depth(x) - depth(y))<0
								 ) return False;
    /* Bring x to same hierarchy level than y */
    while (difDepth != 0) {x = Parent(x); difDepth--;}}
  /* Check that there is now a strict parallelism between x and y */
  curX = x; curY = y;
  do {
    if (!curY->ParalTyp || curX->RootTyp!=curY->RootTyp) return False;
    curX = Parent(curX); curY = Parent(curY);
  } while (curX->ParalTyp);
  if (curY->ParalTyp) return False;
  if ((curX = NxtTypElt(curX))==NULL || (curY = NxtTypElt(curY))==NULL) return
									   True;
  {
    Tqualif xQual, yQual;

    difQual = (int)((xQual = curX->Qualif) - (yQual = curY->Qualif)) &
							 ((MaxQualif << 1) + 1);
    if (difQual == 0) return False;
    rowXName = x->TypeId->PdscId->idName;
    rowYName = y->TypeId->PdscId->idName;
    {
      size_t lgtQlfd = Lgt(rowXName), lgtNqlfd = Lgt(rowYName);
      const TnameAtom *ptrQlfd = rowXName + LgtHdrId, *ptrNqlfd = rowYName +
								       LgtHdrId;

      if (difQual >= MaxQualif+1) {
        size_t w1 = lgtQlfd; const TnameAtom *w2 = ptrQlfd;

        lgtQlfd = lgtNqlfd; ptrQlfd = ptrNqlfd;
        lgtNqlfd = w1; ptrNqlfd = w2;}
      while (lgtNqlfd != 0) {
        if (lgtQlfd == 0) return False;  /* name of less qualified type not
						       included in the other. */
        if (*ptrNqlfd == *ptrQlfd++) {ptrNqlfd++; lgtNqlfd--;}
        lgtQlfd--;}}
    difQual |= SIGN_BIT;  /* => negative */
    if (x->TypeId->PdscId->nstLvl != y->TypeId->PdscId->nstLvl) {  /* because
			     then typeElts would not be freed at same time... */
      difQual |= (SIGN_BIT >> 1) & INT_MAX;
      return False;}
    /* Establish (crossed) link between x and y */
    x = x->TypeId->NamedType; y = y->TypeId->NamedType;  /* because of possible
								qualifiers... */
    {
      TpTypeElt *wx, *wy;

      if (! x->QlfdTyp) {
        x->PqlfdTypes = allocQlfdElt();
        x->QlfdTyp = True;
        wx = &x->PqlfdTypes->arrQlfdTypes[0];}
      else {
        wx = &x->PqlfdTypes->arrQlfdTypes[0];
        do {
          if (NxtTypElt(*wx++)->Qualif == yQual)  {  /* there exists already
					     a same-qualified parallel type . */
            rowYName = (*(wx - 1))->TypeId->PdscId->idName;
            return False;}
        } while (*wx != NULL);}
      if (! y->QlfdTyp) {
        y->PqlfdTypes = allocQlfdElt();
        y->QlfdTyp = True;
        wy = &y->PqlfdTypes->arrQlfdTypes[0];}
      else {
        wy = &y->PqlfdTypes->arrQlfdTypes[0];
        do {
          if (NxtTypElt(*wy++)->Qualif == xQual) {
            if (wx == &x->PqlfdTypes->arrQlfdTypes[0]) {(void)freeQlfdElt(x->
					       PqlfdTypes); x->QlfdTyp = False;}
            rowXName = (*(wy - 1))->TypeId->PdscId->idName;
            return False;}  /* there exists already a same-qualified parallel
								       type . */
        } while (*wy != NULL);}
      *wx++ = y; *wy++ = x;
      *wx = *wy = NULL;}}  /* end of table marker */
  return True;
}
      
static bool sameLgtDList(const TdeclElt *wx, const TdeclElt *wy)
{
  while (wx!=NULL && wy!=NULL) {wx = wx->cdr; wy = wy->cdr;}
  return (wx == wy);
}

size_t sizeOfTypeD(TpTypeElt type, Terr err, Tname name)
{
  TpcTypeElt type1 = type;

  if (type == NULL) return 0;
  if (IsStrun(type)) type = BaseStrunType(type);
  if (((IsArr(type))? type->Lim : type->size)==0 && !type->ErrSiz) {
    errWNT(err, name, type1);
    if (!(err & WarnMsk) && type!=&natTyp[VoidDpl] && !allErrFl)
      type->ErrSiz = True;}
  return type->size;
}

static bool tstNoNewTyp(TinfoDecl info)
{
  if (info.s11._noNewTypeFl || InsideDPragma) {
    err0(StrunEnumDeclNotAlwd|Warn3|PossErr);
    return True;}
  return False;
}

TresulTstPtd tstPtd(register TpcTypeElt w, bool onlyFrstLvl)
{
  if (w == NULL) return ERRTYPE;
  if (! IsPtrArr(w)) return (IsStrun(w))? STRUN : NOTPTR;
  {
    bool constArray = False, firstTurn = True;
    TtypeSort ts;

    do {
      if ((ts = w->typeSort) == Array) {if (w->Qualif & ConstQal) constArray =
									  True;}
      else {
        if (IsFctSort(ts)) break;
        if (! firstTurn) {
          if (! (w->Qualif & ConstQal || constArray)) return NOTALLCONST;
          if (onlyFrstLvl) return /*addInfo |*/ ALLCONST;
          constArray = False;}}
      firstTurn = False;
    } while ((w = NxtTypElt(w)) != NULL);
    return ((IsPtrArrSort(ts))? ERRTYPE : ALLCONST);}
}

static bool tstQlfdVariants(TpcTypeElt x, TpcTypeElt y, bool recursCall)
/* Returns True if 'y' is same parallel type as 'x', or is an (un)qualified
   variation of 'x'.
   Prevents infinite recursion on crossed qualified types. */
{
  static TpcTypeElt initialType;

  if (x->TypeId == y->TypeId) return True;
  if (recursCall) {if (x == initialType) return False;}  /* for no infinite
								   recursion. */
  else initialType = x;
  if (x->QlfdTyp) {
    const TpcTypeElt *w = &x->PqlfdTypes->arrQlfdTypes[0];

    do {
      if (tstQlfdVariants(*w++, y, True)) return True;
    } while (*w != NULL);}
  return False;
}

/* End DCDECL.C */
