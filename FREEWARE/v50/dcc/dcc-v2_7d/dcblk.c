/* DCBLK.C */
/* Y.L. Noyelle, Supelec, France 1994 */
/* Manages blocks and associated identifiers; also symbol table and string
   literals. */

#include <stdlib.h>
#ifdef EXIT_FAILURE
#  undef EXIT_FAILURE
#endif
#include <string.h>
#include "dcblk.h"
#include "dcblk.ph"
#include "dcdecl.h"
#include "dcdir.ph"
#include "dcext.h"
#include "dcinst.h"
#include "dcinst.ph"  /* for 'condStmtLvl' */
#include "dcmain.h"
#include "dcprag.h"
#include "dcrec.h"
#include "dcrec.ph"
#include "dcrecdir.th"

#define IsStub(x)	((idName = x->idName)==NULL || idName[DispNSId]==      \
							(TnameAtom)StubSpace)
#define PosEndNameStoArr (Offset(TnameBlk, nameSto) + SizeNameStorageChunk     \
							  * sizeof(TnameAtom))
#define SizeNameStorageChunk ((SizeNameStorageBlk - Offset(struct _bid, bid))/ \
							    sizeof(TnameAtom))
DefRepresType

/* Structures definitions */
typedef struct _TblkStkElt TblkStkElt;
struct _TblkStkElt {
  TdescrId *headSameBlk;
  Tname pFreeNameStorage, endCurNameStoBlk;
  size_t lastStrLitLgt;
  TnstLvl baseNstLvl;
  TblkStkElt *prev;
};
			
typedef struct _TnameBlk TnameBlk;
struct _bid{  /* just to allow computation of SizeNameStorageChunk */
  TnameBlk *bid1, *bid2;
  TnameAtom bid;
};
struct _TnameBlk {
  TnameBlk *next, *prec;
  TnameAtom nameSto[SizeNameStorageChunk];
};
/*~zif (Offset(struct _bid, bid) != Offset(TnameBlk, nameSto))
				     "struct _bid non coherent with TnameBlk" */
/*~zif (SizeNameStorageBlk < sizeof(TnameBlk))
				    "incorrect struct _TnameBlk organization" */


typedef struct _savStrLitCtx TsavStrLitCtx;
struct _savStrLitCtx {
  const TnameAtom *litPtr, *endLitChunk;
  TsavStrLitCtx *prev;
};

/* Function profiles */
static TdescrId *allocDescrIdElt(void);
static TblkStkElt *allocBlkStkElt(void), *freeBlkStkElt(TblkStkElt *);
static TsemanElt *allocSemanElt(void), *freeSemanElt(TsemanElt *);
static void initSymTab(void), stats(void);

/* Global variables (should be hidable !!!) */
static const TnameAtom *litPtr, *endLitChunk;
static const TdescrId *pCurIdInBlk;
static TblkStkElt *pTopBlkStk;	/* top of (nested) blocks description stack */
static TsavStrLitCtx *pTopSavLitCtxStk = NULL;
static Tname pFreeLblSpace = NULL, endLblNameBlk = NULL;  /* to manage label
							   name string space. */

static AllocXElt(allocBlkStkElt, TblkStkElt, ctrBSE, ;)
static AllocXElt(allocDescrIdElt, TdescrId, ctrDIE, ;)
static AllocXElt(allocSavStrLitElt, TsavStrLitCtx, ctrSSL, ;)
static AllocXElt(allocSemanElt, TsemanElt, ctrSE, ;)

TnstLvl baseNstLvl(void)
{
  return pTopBlkStk->baseNstLvl;
}

static int cmpId(Tname id1, Tname id2) /*~Utility*/
{
  int w;

  if ((w = memcmp(id1 + LgtHdrId, id2 + LgtHdrId, *(((Lgt(id1)
			  < Lgt(id2))? id1 : id2) + DispLgtId))) != 0) return w;
  return (int)Lgt1(id1) - (int)Lgt1(id2);
}

Tname curIdName(void)
{
  return pCurIdInBlk->idName;
}

void enterBlock(void)
{
  register TblkStkElt *w = allocBlkStkElt();

  w->pFreeNameStorage = pTopBlkStk->pFreeNameStorage;
  w->endCurNameStoBlk = pTopBlkStk->endCurNameStoBlk;
  w->headSameBlk = NULL;
  w->baseNstLvl = ++nestLvl;
  w->prev = pTopBlkStk;	/* push 		     */
  pTopBlkStk = w;	/*      new blockDescriptor. */ 
}

TsemanElt *enterSymTab(Tname idName)
{
  return enterSymTabHC(idName, (idName != NULL)? hCodFct(idName) : 0);
}

TsemanElt *enterSymTabHC(Tname idName, ThCode hCode)
/* Returns NULL if symbol already exists in current block (same name space);
   else enters symbol into symbol table, and chains it to other symbols
   defined in same block. */
{
  TdescrId *pDescId;

  if ((pDescId = enterSymTabHC1(idName, hCode)) == NULL) return NULL; /* iden-
				   tifier already existing in the same block. */
  pDescId->SameBlk = pTopBlkStk->headSameBlk; /* record new identifier	      */
  pTopBlkStk->headSameBlk = pDescId;    /* (declared at current block level). */
  (pDescId->PidSeman = allocSemanElt())->PdscId = pDescId;
  return pDescId->PidSeman;
}

TdescrId *enterSymTabHC1(Tname idName, ThCode hCode)
/* 'idName' is supposed to have its own storage, that becomes owned by the new
   descriptionId element. */
{
  register TdescrId *pDescId, **bucket;
  TdescrId *pDescIdMacro = NULL;
  register TnstLvl realNstLvl = pTopBlkStk->baseNstLvl;

  if (idName != NULL) {	/* if name exist, try to enter it into symbol table */
    bucket = &symTabHeads[hCode & (SizeSymTab - 1)]; /* address of pointer on
				first element of corresponding 'small table'. */
    /* Search if identifier already there, in the same block ('pDescId' used
					    here as temporary roaming index). */
    for (pDescId = *bucket; pDescId != NULL; pDescId = pDescId->next) {
      if ((int)pDescId->nstLvl!=realNstLvl && pDescId->nstLvl>=0) break;  /*
			     exiting current block (macro names are skipped). */
      if (*(pDescId->idName + DispLgtId)==*(idName + DispLgtId) /* same length
		 ? */  && memcmp(pDescId->idName, idName, FullLgt(idName))==0) {
        if (pDescId->nstLvl >= 0) return NULL;  /* already there... */
        if (pDescId->nstLvl+1 == 0) break;  /* keyword (case '+zkwm') */
        pDescIdMacro = pDescId;}}  /* there exist a macro with the same name */
    /* Add identifier to symbol table */
    pDescId = allocDescrIdElt();
    if (pDescIdMacro == NULL) {  /* add new identifier at front of 'small
		       table', so symbol table as a whole organized as stack. */
      if ((pDescId->next = *bucket) != NULL) (*bucket)->prec = pDescId;  /* dou-
								ble chaining. */
      pDescId->prec = (TdescrId *)bucket;
      *bucket = pDescId;}
    else {  /* do not mask macro (and do not change its descriptor address !),
		       so add new identifier just after macro (of same name). */
      if ((pDescId->next = pDescIdMacro->next) != NULL) pDescId->next->prec =
									pDescId;
      pDescId->prec = pDescIdMacro;
      pDescIdMacro->next = pDescId;}}
  else pDescId = allocDescrIdElt();  /* stub */
  pDescId->idName = idName;
  pDescId->nstLvl = (long)realNstLvl;
  return pDescId;
}

void exitBlock(void)
{
  TdescrId *pDescId;
  Tname curEndNameSto;
  TsemanElt *pId;
#if Debug
/*  TnstLvl prevNstLvl = nestLvl;*/
#endif

  if (printStat && !noExpand) stats();
  nestLvl--;
  if ((uint)nestLvl < (uint)condStmtLvl) condStmtLvl = nestLvl;
  if ((uint)nestLvl < (uint)setjmpBlkLvl) setjmpBlkLvl = nestLvl;
  /* Get rid of all symbols and stubs declared in block being exited (except
     for labels if not exiting function; then move them to outer block). */
  curEndNameSto = pTopBlkStk->endCurNameStoBlk;  /* free their name storage */
  pDescId = pTopBlkStk->headSameBlk;
  pTopBlkStk = freeBlkStkElt(pTopBlkStk);
  while (pDescId != NULL) {
    Tkind kindId;

#if Debug
/*    if (pDescId->nstLvl != prevNstLvl) sysErr(ExCod4);*/
#endif
    ignoreErr = False;
    pId = pDescId->PidSeman;
    kindId = pId->Kind;
    if (kindId!=Label || nestLvl==0) {  /* not a label, or function exit */
      Tname idName;

      if (! IsStub(pDescId)) {
        /* Check various conditions */
        if (!pId->Used
            && (pId->Attribb!=Extrn
                || InsideInterval(kindId, EnumTag, StrunTag)
                   && isBodyHdrFile(IndicatedFName(pId->defFileName)))
            && (warnNUI || kindId==Obj && (pId->type==NULL || !IsFct(pId->
						  type)) && pId->Initlz)) errId(
			      (kindId == Param)
                              ? (warnNUP && !pId->ForceUsed)
                                ? UnusedPar|NoDispLine|Warn1
                                : NoErrMsg
                              : (kindId == Obj)
                                ? (pId->type!=NULL && IsFct(pId->type))
                                  ? UnusedFct|NoDispLine|Warn1|PossErr
                                  : (pId->Initlz)
                                    ? (pId->ForceUsed)
                                      ? NoErrMsg
                                      : UnusedVar|NoDispLine|Warn1|PossErr
                                    : UslObj|NoDispLine|Warn1|PossErr
                                : (kindId == Type)
                                  ? UnusedTypeId|NoDispLine|Warn1|PossErr
                                  : (kindId <= StrunTag)
                                    ? UnusedTag|NoDispLine|Warn1|PossErr
                                    : (kindId == EnumCst)
                                      ? UnusedEnumCst|NoDispLine|Warn1|PossErr
                                      : (kindId == Label)
                                        ? UnusedLabel|NoDispLine|Warn1|PossErr
                                        : /* ParamMac */ UnusedMacPar|Warn2|
									PossErr,
		pId, idName, nameToS(curFctName),
                (pId->PassedToFct)
                  ? errTxt[MsngSavedQ]
                  : (kindId == Param)
                    ? errTxt[MsngNotUsed]
                    : NULL);
        if (! pId->Defnd) {
          if (kindId == Label) errId(UndefLabel|NoDispLine, pId, idName,
						     nameToS(curFctName), NULL);
          else if (kindId == StrunTag)
            if (pId->Attribb != Extrn) {
              if (! pId->Used) errId0(UndefTag|NoDispLine|Warn1|PossErr, pId);}
            else {
              if (pId->ListAlwdFiles!=NULL && isBodyHdrFile(lastPrivFileName(
			    pId))) errId1(TagShdBeDefIn|NoDispLine|Warn1|Rdbl,
						   pId, lastPrivFileName(pId));}
          else if (nestLvl<0
                   && !pId->SysElt
                   && pId->Attribb==Extrn
                   && isBodyHdrFile(pId->DeclaringFile)
                   && !(bool)*(pId->defFileName - PosCmpsgFlg) 	/* not
			ComposingHdr */) errId0(ExtObjNotDef|NoDispLine|
							    Warn2|PossErr, pId);
		  /* tests 'declaringFile', and not 'defFileName', because of
		 possible inclusion of non-header file in (body) header file. */
          else if (pId->Attribb==Stati && pId->type!=NULL && IsFct(pId->type)
		  ) errId0((pId->Used)? StaFctNotDef|NoDispLine : StaFctNotDef|
						NoDispLine|Warn1|PossErr, pId);}
        if (pId->Used || pId->Initlz) {
          if (!pId->Dmodfd
              && (kindId==Param
                  || kindId==Obj
                     && (pId->CheckConst || verifAllPtrForConst)
                     && (pId->Attribb==Stati || pId->Attribb>=Auto))
              && askConstFl
              && !(mainFl && !goodPrgmg)) {
            if (pId->Defnd /* cf ~NotUsed */ && tstPtd(pId->type, False) &
								  NOTALLCONST) {
              TpTypeElt w = pId->type, w1;

              cExp.ptrId = pId;
              cExp.Einfo = 0;
              cExp.hist = H_EMPTY;
              while ((w1 = NxtTypElt(w)) != NULL) {
                if ((kindId==Param || IsPtr(w) /* only Ptr, for non-parameter
		  array stands for its elements. */) && !(IsArr(w) && (cExp.hist
		       & MskHistElt)==H_ARRAY) && tstPtd(w, True) & NOTALLCONST)
                  errId(
                    (IsPtr(w))? PtrShdBeConst|NoDispLine|Warn1|Rdbl :
					    ArrShdBeConst|NoDispLine|Warn1|Rdbl,
                    pId,
                    decoratedName(&cExp),
                    nameToS(curFctName),
                    (IsPtr(w1))? errTxt[EGConstPtr] : NULL);
                PushHist(((IsPtr(w))? H_PTR : H_ARRAY)); /* for
							     decoratedName(). */
                w = w1;}}}
          if (pId->ModifSJ) errId0(MsngVolatQual/* |NoDispLine 
				(because of message text */|Warn3|PossErr, pId);
          if (kindId == Obj) {
            if (pId->DynInitCtr==NoDynInitWarn-1  && pId->Attribb>=Auto &&
		  !pId->type->SysTpdf /* pId->type can't be NULL here */
			       ) errId0(StaticInit|NoDispLine|Warn1|Effic, pId);
            if (pId->type!=NULL && IsStrun(pId->type) && !pId->StrunMdfd &&
		!(pId->type->Qualif & ConstQal) && pId->Attribb!=Extrn) errId0(
				   StrunShdBeConst|NoDispLine|Warn1|Rdbl, pId);}
          else if (kindId==Param && pId->Saved && !pId->MayNeedSaved) errId2(
		 BadDPForFctPar|NoDispLine|Warn1, pId, nameToS(curFctName),
								dpName(SAVED));}
        checkNotInitVarAndSuppress(pId);
        /* Remove symbol from symbol table */
        pDescId->prec->next = pDescId->next;
        if (pDescId->next != NULL) pDescId->next->prec = pDescId->prec;}
      /* Stub or not, free associated type chain, semanElt, descriptor */
      freeTypes(pId);
      (void)freeSemanElt(pId);
      pDescId = freeDescrIdElt(pDescId);}
    else {		/* label, and not function exit */
      TdescrId *w = pDescId;

      pDescId = pDescId->SameBlk;  /* must be here, because field 'SameBlk'
							  is going to change. */
      w->nstLvl = pId->NstLvla = pTopBlkStk->baseNstLvl;/* move to           */
      w->SameBlk = pTopBlkStk->headSameBlk;		 /*        outer      */
      pTopBlkStk->headSameBlk = w;			 /*             block.*/
      pId->Inner = pId->Defnd;}}
  /* Free (non-label) name storage (keep one more block than needed at present,
     so that calls to 'malloc' are minimized). */
  if (curEndNameSto != pTopBlkStk->endCurNameStoBlk) {
    TnameBlk *w1, *w2, *nxtNameBlk = ((TnameBlk * /*~OddCast*/)(pTopBlkStk->
				    endCurNameStoBlk - PosEndNameStoArr))->next;

    w1 = (TnameBlk * /*~OddCast*/)(curEndNameSto - PosEndNameStoArr);
    while (w1->next != NULL) {w1 = w1->next;}  /* to reclaim waiting blocks */
    while (w1 != nxtNameBlk) {w2 = w1->prec; free(w1); ctrNSB--; w1 = w2;}
    nxtNameBlk->next = NULL;}
  /* If exiting from a function, free label name storage (keep one block, as for
     name storage). */
  if (nestLvl==0 && pFreeLblSpace!=NULL) {
    TnameBlk *precNameBlk, *curNameBlk = (TnameBlk * /*~OddCast*/)
					     (endLblNameBlk - PosEndNameStoArr);

    while ((precNameBlk = curNameBlk->prec) != NULL) {free(curNameBlk)
					  ; ctrNSB--; curNameBlk = precNameBlk;}
    pFreeLblSpace = &curNameBlk->nameSto[0];
    endLblNameBlk = &curNameBlk->nameSto[NbElt(curNameBlk->nameSto)];}
}

static FreeXElt(freeBlkStkElt, TblkStkElt *, ctrBSE, ; , prev)
FreeXElt(freeDescrIdElt, TdescrId *, ctrDIE, ; , SameBlk)
static FreeXElt(freeSavStrLitElt, TsavStrLitCtx *, ctrSSL, ; , prev)
static FreeXElt(freeSemanElt, TsemanElt *, ctrSE, ; , u3._bid)

TlitString getLitString(void)
/* 'exitBlock()' must be called when done with string */
{
  TlitString result;

  enterBlock();  /* for string storage reclaiming purpose */
  if (curTok.tok != CSTST) {err0(StrCstExptd); return NULL;}
  result = ptrFreeIdSpace();
  do {analStrCst(&storeStrCh);} while (NxtTok() == CSTST);
  storeStrCh('\0');
  return result;
}

void initBlk(void)
{
  register TnameBlk *frstBlk;
  static bool frstInit = True;

  if (frstInit) {
    initSymTab();
    /* Initialize blockStack and identifiers nameString storage */
    pTopBlkStk = allocBlkStkElt();
    pTopBlkStk->prev = NULL;	/* bottom of stack */
    pTopBlkStk->headSameBlk = NULL;
    pTopBlkStk->baseNstLvl = nestLvl;
    MyAlloc(frstBlk, sizeof(TnameBlk));
    /*~zif ((ptrdiff_t)SizeNameStorageChunk < MaxLgtId + LgtHdrId)
					       "SizeNameStorageBlk too small" */
    ctrNSB++;
    frstBlk->prec = frstBlk->next = NULL;
    pTopBlkStk->pFreeNameStorage = &frstBlk->nameSto[0];
    pTopBlkStk->endCurNameStoBlk = &frstBlk->nameSto[NbElt(frstBlk->nameSto)];
    frstInit = False;
  }
}

size_t initGetStrLit(TlitString x) /*~PseudoVoid*/
/* Does initializations for access to string literal 'x' (adjacent literals
   coalesced), and answers length (including ending '\0') of last seen string
   literal (in current block). When done with string literal, call
   'resetGetStrLit()', except if x==NULL. */
{
  if (x != NULL) {
    {
      TsavStrLitCtx *w = allocSavStrLitElt();

      w->litPtr = litPtr;
      w->endLitChunk = endLitChunk;
      w->prev = pTopSavLitCtxStk;
      pTopSavLitCtxStk = w;}
    endLitChunk = pTopBlkStk->endCurNameStoBlk;
    /* Search beginning block */
    while ((size_t)(endLitChunk - x) > sizeof(TnameBlk)) {
      const TnameBlk *precNameBlk = ((TnameBlk * /*~OddCast*/)(endLitChunk -
						       PosEndNameStoArr))->prec;

      endLitChunk = &precNameBlk->nameSto[SizeNameStorageChunk];}
    litPtr = x;}
  return pTopBlkStk->lastStrLitLgt;
}

TsemanElt *initGetNxtIdInCurBlk(void)
{
  pCurIdInBlk = pTopBlkStk->headSameBlk;
  return (pCurIdInBlk == NULL)? NULL : pCurIdInBlk->PidSeman;
}

static void initSymTab(void)
{
  TdescrId **ptr = &symTabHeads[0];

  do {*ptr++ = NULL;} while (ptr <= &symTabHeads[SizeSymTab - 1]);
}

TsemanElt *nxtId(void)
{
  pCurIdInBlk = pCurIdInBlk->SameBlk;
  return (pCurIdInBlk == NULL)? NULL : pCurIdInBlk->PidSeman;
}

char nxtStrLitChar(void) /*~Generic*/
{
  if (litPtr == endLitChunk) {
    const TnameBlk *nxtNameBlk = ((TnameBlk * /*~OddCast*/)(endLitChunk -
						       PosEndNameStoArr))->next;

    litPtr = &nxtNameBlk->nameSto[0];
    endLitChunk = litPtr + SizeNameStorageChunk;}
  return (char)*litPtr++;
}

TlitString ptrFreeIdSpace(void)
{
  pTopBlkStk->lastStrLitLgt = 0;
  return (TlitString)pTopBlkStk->pFreeNameStorage;
}

void resetGetStrLit(void)
{
  TsavStrLitCtx *w = pTopSavLitCtxStk;

  litPtr = w->litPtr;
  endLitChunk = w->endLitChunk;
  pTopSavLitCtxStk = freeSavStrLitElt(w);
}

TsemanElt *searchSymTab(Tname idName)
{
  return searchSymTabHC(idName, hCodFct(idName));
}

TsemanElt *searchSymTabHC(register Tname adBegName, ThCode hCode)
/* Returns NULL if symbol not found in the whole symbol table (same name
   space). */
{
  register TdescrId *pDescCurId;

  SearchSym(adBegName, MacroNotVisible)
  return (pDescCurId == NULL)? NULL : pDescCurId->PidSeman;
}

static void stats(void)
/* Print symbol table content for current block, in alphabetical order */
{
  Tname lowLimit = ConvTname("\1\0 ");
  uint lowLimitPos;
  bool identSeen = False;

  for(;;) {
    const TdescrId *pDescCurId, *pDescMinId;
    Tname curMinId, maxId = ConvTname("\1\0~");
    uint posit, minIdPos;

    curMinId = maxId;
    for (posit = 0, pDescCurId = pTopBlkStk->headSameBlk; pDescCurId != NULL;
				    pDescCurId = pDescCurId->SameBlk, posit++) {
      int w;
      Tname idName;

      if (IsStub(pDescCurId)) continue;
      if (((w = cmpId(pDescCurId->idName, lowLimit))>0 || w==0 && posit>
			lowLimitPos) && cmpId(pDescCurId->idName, curMinId)<0) {
        curMinId = pDescCurId->idName;
        minIdPos = posit;
        pDescMinId = pDescCurId;}}
    if (curMinId == maxId) break;
    lowLimit = curMinId;
    lowLimitPos = minIdPos;
    {
      const TsemanElt *pId = pDescMinId->PidSeman;

      if (pId->Kind!=Label || pId->Defnd && !pId->Inner) {  /* output label
						    only at definition level. */
        emitS((identSeen)? "\n" : (identSeen = True, errTxt[SeparMsg]));
        emitS("- ");
        emitS(nameToS(curMinId));
        {
          static const Tstring tabNS[/*~ IndexType TnameSpace */] = {"(GNS/",
						    "(TNS/", "(LNS/", "/", "/"};
          /*~ zif (NbEltGen(tabNS, ObjectSpace) != __extent(TnameSpace) + 1)
					      "array 'tabNS': incorrect size" */

          emitS(tabNS[(TnameSpace)*(curMinId + DispNSId)]);}
        putSeman(pId);}}}
  /* Explore all of symbol table, to evaluate its behavior */
  if (identSeen) {
    const TdescrId *const *bucket;
    uint maxNbIdBckt = 0, nbUsedBckts = 0, nbId = 0;

    for (bucket = &symTabHeads[0]; bucket != (const TdescrId *const *) /* VMS
		     compiler requires this cast */&symTabHeads[SizeSymTab]; ) {
      /* Explore each bucket */
      uint nbIdBckt = 0;
      const TdescrId *w;

      for (w = *bucket++; w != NULL; w = w->next) nbIdBckt++;
      if (nbIdBckt != 0) {
        if (nbIdBckt > maxNbIdBckt) maxNbIdBckt = nbIdBckt;
        nbId += nbIdBckt;
        nbUsedBckts++;}}
    emitS("\n"); emitS(grstIntToS((TgreatestInt)nbId)); emitS(" id(s)");
    emitS("; nb buckets used: "); emitS(grstIntToS((TgreatestInt)nbUsedBckts));
    emitS("; max bucket filling: "); emitS(grstIntToS((TgreatestInt)
								  maxNbIdBckt));
    emitS("; nbUnivEltChunks: "); emitS(grstIntToS((TgreatestInt)ctrUEC));
    emitS("\n");}
} 

Tname storeLabelName(Tname x)
/* Cannot use same name string storage as 'storeName', because labels may be
   moved from one block to an outer one; so storage must not be freed at block
   exit, but only at function exit. */
{
  TnameNC ptr;
  size_t size;

  size = FullLgt(x);
  if (pFreeLblSpace + size > endLblNameBlk) {
    TnameBlk *nxtNameBlk;

    MyAlloc(nxtNameBlk, sizeof(TnameBlk));
    ctrNSB++;
    nxtNameBlk->prec = (endLblNameBlk == NULL)? NULL :
		    (TnameBlk * /*~OddCast*/)(endLblNameBlk - PosEndNameStoArr);
    pFreeLblSpace = &nxtNameBlk->nameSto[0];
    endLblNameBlk = pFreeLblSpace + SizeNameStorageChunk;}
  ptr = (TnameNC /*~OddCast*/)pFreeLblSpace;
  pFreeLblSpace += size;
  memcpy(ptr, x, FullLgt(x));  /* copy name into new allocated space */
  *(ptr + DispNSId) = (TnameAtom)LabelSpace;  /* set name space */
  return (Tname)ptr;
}

Tname storeName(Tname x, TnameSpace y)
/* Allocates a memory block to store contiguously the name 'x', and returns
   pointer on stored name. Storage freed at block exit. */
{
  TnameNC ptr;
  size_t size;
  register TblkStkElt *pTBS;

  if (y==ObjectSpace && pDescTokId!=NULL) return pDescTokId->idName;
  pTBS = pTopBlkStk;
  size = FullLgt(x);
  if (pTBS->pFreeNameStorage + size > pTBS->endCurNameStoBlk) {
    TnameBlk *nxtNameBlk = ((TnameBlk * /*~OddCast*/)(pTBS->endCurNameStoBlk -
						       PosEndNameStoArr))->next;

    if (nxtNameBlk == NULL) {  /* needs a new block */
      MyAlloc(nxtNameBlk, sizeof(TnameBlk));
      ctrNSB++;
      nxtNameBlk->next = NULL;
      nxtNameBlk->prec = (TnameBlk * /*~OddCast*/)(pTBS->endCurNameStoBlk -
							      PosEndNameStoArr);
      nxtNameBlk->prec->next = nxtNameBlk;}	/* double chaining */
    pTBS->pFreeNameStorage = &nxtNameBlk->nameSto[0];
    pTBS->endCurNameStoBlk = pTBS->pFreeNameStorage + SizeNameStorageChunk;}
  ptr = (TnameNC /*~OddCast*/)pTBS->pFreeNameStorage;
  pTBS->pFreeNameStorage += size;
  memcpy(ptr, x, size);  /* copy name into new allocated space */
  *(ptr + DispNSId) = (TnameAtom)y;	/* set name space */
  return (Tname)ptr;
}

void storeStrCh(char x)
/* Strings are stored in the "name" area, freed only at block exit */
{
  register TblkStkElt *pTBS = pTopBlkStk;
  Tname ptr = pTBS->pFreeNameStorage;

  if (ptr == pTBS->endCurNameStoBlk) ptr = storeName((Tname)"", ObjectSpace +
				     1);  /* to force allocation of new block */
  *(TnameNC /*~OddCast*/)ptr++ = (TnameAtom)x;  /* store character */
  pTBS->lastStrLitLgt++;
  pTBS->pFreeNameStorage = ptr;
}

/* End DCBLK.C */
