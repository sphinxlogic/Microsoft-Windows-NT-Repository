/* DCBLK.C */
/* Y.L. Noyelle, Supelec, France 1994 */
/* Manages blocks and associated identifiers; also symbol table and string
   literals. */

#include <stdlib.h>
#include <string.h>
#include "dcblk.h"
#include "dcblk.ph"
#include "dcdecl.h"
#include "dcdir.ph"
#include "dcext.h"
#include "dcinst.h"
#include "dcmain.h"
#include "dcprag.h"
#include "dcrec.h"
#include "dcrec.ph"

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
  TblkStkElt *prec;
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


/* Function profiles */
static TdescrId *allocDescrIdElt(void);
static TblkStkElt *allocBlkStkElt(void), *freeBlkStkElt(TblkStkElt *);
static TsemanElt *allocSemanElt(void), *freeSemanElt(TsemanElt *);
static void initSymTab(void), stats(void);

/* Global variables (should be hidable !!!) */
static const TnameAtom *litPtr, *endLitBuf;
static TdescrId *pCurIdInBlk;
static TblkStkElt *pTopBlkStk;	/* top of (nested) blocks description stack */
static Tname pFreeLblSpace = NULL, endLblNameBlk = NULL;  /* to manage label
							   name string space. */

static AllocXElt(allocBlkStkElt, TblkStkElt *, ctrBSE, ;)
static AllocXElt(allocDescrIdElt, TdescrId *, ctrDIE, ;)
static AllocXElt(allocSemanElt, TsemanElt *, ctrSE, ;)

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
  register TblkStkElt *w;

  w = allocBlkStkElt();
  w->pFreeNameStorage = pTopBlkStk->pFreeNameStorage;
  w->endCurNameStoBlk = pTopBlkStk->endCurNameStoBlk;
  w->headSameBlk = NULL;
  w->baseNstLvl = ++nestLvl;
  w->prec = pTopBlkStk;	/* push 		     */
  pTopBlkStk = w;	/*      new blockDescriptor. */
 
}

TsemanElt *enterSymTab(Tname idName)
{
  return enterSymTabHC(idName, (idName != NULL)? hCodFct(idName) : 0);
}

TsemanElt *enterSymTabHC(Tname idName, ThCode hCode)
/* Returns NULL if symbol already exist in current block (same name space);
   else enters symbol into symbol table, and chains it to other symbols
   defined in same block. */
{
  TdescrId *pId;

  if ((pId = enterSymTabHC1(idName, hCode)) == NULL) return NULL; /* identifier
					  already existing in the same block. */
  pId->sameBlk = pTopBlkStk->headSameBlk; /* record new identifier	      */
  pTopBlkStk->headSameBlk = pId;     /*	   (declared at current block level). */
  return (pId->pIdSeman = allocSemanElt());
}

TdescrId *enterSymTabHC1(Tname idName, ThCode hCode)
/* 'idName' is supposed to have its own storage, that becomes owned by the new
   descriptionId element. */
{
  register TdescrId *pId, **w;
  TdescrId *pIdMacro = NULL;
  register TnstLvl realNstLvl = pTopBlkStk->baseNstLvl;

  if (idName != NULL) {	/* if name exist, try to enter it into symbol table */
    w = &symTabHeads[hCode & (SizeSymTab - 1)]; /* address of pointer on first
				      element of corresponding 'small table'. */
    /* Search if identifier already there, in the same block ('pId' used here
						 as temporary roaming index). */
    for (pId = *w; pId != NULL; pId = pId->next) {
      if ((int)pId->nstLvl!=realNstLvl && pId->nstLvl>=0) break;  /* exiting
				     current block (macro names are skipped). */
      if (*(pId->idName + DispLgtId)==*(idName + DispLgtId)  /* same
	      length ? */  && memcmp(pId->idName, idName, FullLgt(idName))==0) {
        if (pId->nstLvl >= 0) return NULL;  /* already there... */
        pIdMacro = pId;}}  /* there exist a macro with the same name */
    pId = allocDescrIdElt();
    if (pIdMacro == NULL) {  /* add new identifier at front of 'small table',
			       so symbol table as a whole organized as stack. */
      if ((pId->next = *w) != NULL) (*w)->prec = pId;  /* double chaining */
      pId->prec = (TdescrId * /*~OddCast*/)w;
      *w = pId;}
    else {  /* do not mask macro (and do not change its descriptor address !),
		       so add new identifier just after macro (of same name). */
      if ((pId->next = pIdMacro->next) != NULL) pId->next->prec = pId;
      pId->prec = pIdMacro;
      pIdMacro->next = pId;}}
  else pId = allocDescrIdElt();  /* stub */
  pId->idName = idName;
  pId->nstLvl = (long)realNstLvl;
  return pId;
}

void exitBlock(void)
{
  TdescrId *w;
  Tname curEndNameSto;
  TsemanElt *pId;
#ifdef debug
  TnstLvl prevNstLvl = nestLvl;
#endif

  if (printStat && macroExpand) stats();  /* !macroExpand => exitBlock
						called in #define processing. */
  nestLvl--;
  /* Get rid of all symbols and stubs declared in block being exited (except
     for labels if not exiting function; then move them to outer block). */
  curEndNameSto = pTopBlkStk->endCurNameStoBlk;  /* free their name storage */
  w = pTopBlkStk->headSameBlk;
  pTopBlkStk = freeBlkStkElt(pTopBlkStk);
  while (w != NULL) {
#ifdef debug
    if (w->nstLvl != prevNstLvl)
      sysErr(ExCod4);
#endif
    ignoreErr = False;
    pId = w->pIdSeman;
    if (pId->kind!=Label || nestLvl==0) {  /* not a label, or function exit */
      Tname symName;

      if ((symName = w->idName) != NULL) {  /* not a stub */
        /* Check various conditions */
        if (!pId->used
            && pId->attribb!=Extrn
            && (warnNUI || pId->kind==Obj && (pId->type==NULL || !IsFct(pId->
						  type)) && pId->initlz)) errId(
			      (pId->kind == Param)
                              ? (warnNUP && !pId->forceUsed)
                                ? UnusedPar|NoDispLine|Warn1
                                : NoErrMsg
                              : (pId->kind == Obj)
                                ? (pId->type!=NULL && IsFct(pId->type))
                                  ? UnusedFct|NoDispLine|Warn1|PossErr
                                  : (pId->initlz)
                                    ? (pId->forceUsed)
                                      ? NoErrMsg
                                      : UnusedVar|NoDispLine|Warn1|PossErr
                                    : UslObj|NoDispLine|Warn1|PossErr
                                : (pId->kind == Type)
                                  ? UnusedTypeId|NoDispLine|Warn1|PossErr
                                  : (pId->kind <= StrunTag)
                                    ? UnusedTag|NoDispLine|Warn1|PossErr
                                    : (pId->kind == EnumCst)
                                      ? UnusedEnumCst|NoDispLine|Warn1|PossErr
                                      : (pId->kind == Label)
                                        ? UnusedLabel|NoDispLine|Warn1|PossErr
                                        : (pId->kind == ParamMac)
                                          ? UnusedMacPar|Warn2|PossErr
                                          : UnusedId|NoDispLine|Warn1|PossErr,
		pId, symName, nameToS(curFctName), (pId->MsngConstQM)? errTxt[
							    MsngConstQ] : NULL);
        if (! pId->defnd) {
          if (pId->kind == Label) errId(UndefLabel|NoDispLine, pId, symName,
						     nameToS(curFctName), NULL);
          else if (pId->kind == StrunTag) {if (pId->attribb != Extrn) errId1(
		(pId->used)? UndefTag|NoDispLine : UndefTag|NoDispLine|Warn1|
							   PossErr, pId, NULL);}
          else if (nestLvl<0 && pId->attribb==Extrn && isBodyHdrFile(pId->
		declaringFile)) errId1(ExtObjNotDef|NoDispLine|Warn2|PossErr,
								     pId, NULL);
		  /* tests 'declaringFile', and not 'defFileName', because of
		 possible inclusion of non-header file in (body) header file. */
          else if (pId->attribb==Stati && pId->type!=NULL && IsFct(pId->type)
		  ) errId1((pId->used)? StaFctNotDef|NoDispLine : StaFctNotDef|
					  NoDispLine|Warn1|PossErr, pId, NULL);}
        if (   !pId->Dmodfd
            && (pId->kind==Param
             || pId->kind==Obj
              && (pId->CheckConst || verifAllPtrForConst)
              && pId->attribb>NoAttrib && pId->attribb!=Extrn)
            && askConstFl) {
          if (tstPtd(pId->type, False) & NOTALLCONST && pId->defnd /* cf
								  ~NotUsed */) {
            TpcTypeElt w = pId->type, w1;

            cExp.ptrId = pId;
            cExp.Einfo = 0;
            cExp.hist = H_EMPTY;
            while ((w1 = NxtTypElt(w)) != NULL) {
              if ((pId->kind==Param || IsPtr(w) /* only Ptr, for non-parameter
		  array stands for its elements. */) && !(IsArr(w) && (cExp.hist
		       & MskHistElt)==H_ARRAY) && tstPtd(w, True) & NOTALLCONST)
                errId(
                  (IsPtr(w))? PtrShdBeConst|NoDispLine|Warn1|Rdbl :
					    ArrShdBeConst|NoDispLine|Warn1|Rdbl,
                  pId,
                  decoratedName(&cExp),
                  nameToS(curFctName),
                  (IsPtr(w1))? errTxt[EGConstPtr] : NULL);
              PushHist(((IsPtr(w))? H_PTR : H_ARRAY));  /* for
							     decoratedName(). */
              w = w1;}}
          else if (pId->kind==Param && pId->MayModif) errId2(BadDPForFctPar|
		NoDispLine|Warn1, pId, nameToS(curFctName), dpName(MAYMODIFY));}
        /* Remove symbol from symbol table */
        w->prec->next = w->next;
        if (w->next != NULL) w->next->prec = w->prec;}
      if (notInitVarList != NULL) checkNotInitVarAndSuppress(pId);
      /* Stub or not, free associated type chain, semanElt, descriptor */
      freeTypes(pId);
      (void)freeSemanElt(pId);
      w = freeDescrIdElt(w);}
    else {		/* label, and not function exit */
      TdescrId *w1;

      w1 = w;
      w = w->sameBlk;	/* must be here, because field 'sameBlk' is going
								   to change. */
      w1->nstLvl = pId->nstLvla = pTopBlkStk->baseNstLvl;/* move to           */
      w1->sameBlk = pTopBlkStk->headSameBlk;		 /*        outer      */
      pTopBlkStk->headSameBlk = w1;			 /*             block.*/
      pId->inner = pId->defnd;}}
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
    pFreeLblSpace = (Tname)&curNameBlk->nameSto[0];
    endLblNameBlk = (Tname)curNameBlk + PosEndNameStoArr;}
}

static FreeXElt(freeBlkStkElt, TblkStkElt *, ctrBSE, ; , prec)
FreeXElt(freeDescrIdElt, TdescrId *, ctrDIE, ; , sameBlk)
static FreeXElt(freeSemanElt, TsemanElt *, ctrSE, ; , u1._bid)

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

char getNxtStrLitChar(void) /*~Generic*/
{
  if (litPtr == endLitBuf) {
    TnameBlk *nxtNameBlk = ((TnameBlk * /*~OddCast*/)(endLitBuf -
						       PosEndNameStoArr))->next;

    litPtr = &nxtNameBlk->nameSto[0];
    endLitBuf = litPtr + SizeNameStorageChunk;}
  return (char)*litPtr++;
}

void initBlk(void)
{
  register TnameBlk *w;
  static bool frstInit = True;

  if (frstInit) {
    initSymTab();
    /* Initialize blockStack and identifiers nameString storage */
    pTopBlkStk = allocBlkStkElt();
    pTopBlkStk->prec = NULL;	/* bottom of stack */
    pTopBlkStk->headSameBlk = NULL;
    pTopBlkStk->baseNstLvl = nestLvl;
    MyAlloc(w, sizeof(TnameBlk));
    /*~zif ((ptrdiff_t)SizeNameStorageChunk < MaxLgtId + LgtHdrId)
					       "SizeNameStorageBlk too small" */
    ctrNSB++;
    w->prec = w->next = NULL;
    pTopBlkStk->pFreeNameStorage = ConvTname(&w->nameSto[0]);
    pTopBlkStk->endCurNameStoBlk = ConvTname(w) + PosEndNameStoArr;
    frstInit = False;
  }
}

size_t initGetStrLit(TlitString x) /*~PseudoVoid*/
/* Does initializations for access to string literal 'x' (adjacent literals
   coalesced), and answers length (including ending '\0') of last seen string
   literal. */
{
  if (x != NULL) {
    endLitBuf = pTopBlkStk->endCurNameStoBlk;
    /* Search beginning block */
    while ((size_t)(endLitBuf - x) > sizeof(TnameBlk)) {
      TnameBlk *precNameBlk = ((TnameBlk * /*~OddCast*/)(endLitBuf -
						       PosEndNameStoArr))->prec;

      endLitBuf = &precNameBlk->nameSto[0] + SizeNameStorageChunk;}
    litPtr = x;}
  return pTopBlkStk->lastStrLitLgt;
}

TsemanElt *initGetNxtIdInCurBlk(void)
{
  pCurIdInBlk = pTopBlkStk->headSameBlk;
  return (pCurIdInBlk == NULL)? NULL : pCurIdInBlk->pIdSeman;
}

static void initSymTab(void)
{
  TdescrId **ptr = &symTabHeads[0];

  do {*ptr++ = NULL;} while (ptr <= &symTabHeads[SizeSymTab - 1]);
}

TsemanElt *nxtId(void)
{
  pCurIdInBlk = pCurIdInBlk->sameBlk;
  return (pCurIdInBlk == NULL)? NULL : pCurIdInBlk->pIdSeman;
}

TlitString ptrFreeIdSpace(void)
{
  pTopBlkStk->lastStrLitLgt = 0;
  return (TlitString)pTopBlkStk->pFreeNameStorage;
}

TsemanElt *searchSymTab(Tname idName)
{
  return searchSymTabHC(idName, hCodFct(idName));
}

TsemanElt *searchSymTabHC(register Tname adBegName, ThCode hCode)
/* Returns NULL if symbol not found in the whole symbol table (same name
   space). */
{
  TdescrId *pCurId;

  SearchSym(adBegName, MacroNotVisible)
  return (pCurId == NULL)? NULL : pCurId->pIdSeman;
}

static void stats(void)
/* Print symbol table content for current block, in alphabetical order */
{
  Tname lowLimit = ConvTname("\1\0 ");
  uint lowLimitPos;
  bool identSeen = False;

  for(;;) {
    TdescrId *pCurId, *pMinId;
    Tname curMinId, maxId = ConvTname("\1\0~");
    uint posit, minIdPos;

    curMinId = maxId;
    for (posit = 0, pCurId = pTopBlkStk->headSameBlk; pCurId != NULL; pCurId
						   = pCurId->sameBlk, posit++) {
      int w;

      if (pCurId->idName == NULL) continue;  /* stub */
      if (((w = cmpId(pCurId->idName, lowLimit))>0 || w==0 && posit>
			    lowLimitPos) && cmpId(pCurId->idName, curMinId)<0) {
        curMinId = pCurId->idName;
        minIdPos = posit;
        pMinId = pCurId;}}
    if (curMinId == maxId) break;
    lowLimit = curMinId;
    lowLimitPos = minIdPos;
    {
      TsemanElt *pId = pMinId->pIdSeman;

      if (pId->kind!=Label || pId->defnd && !pId->inner) {  /* output label
						    only at definition level. */
        emitS((identSeen)? "\n" : (identSeen = True, errTxt[SeparMsg]));
        emitS("- ");
        emitS(nameToS(curMinId));
        {
          static const Tstring tabNS[/*~ IndexType TnameSpace */] = {"(GNS/",
							 "(TNS/", "(LNS/", "/"};
          /*~ zif (NbEltGen(tabNS, ObjectSpace) != __extent(TnameSpace) + 1)
					      "array 'tabNS': incorrect size" */

          emitS(tabNS[(TnameSpace)*(curMinId + DispNSId)]);}
        putSeman(pId);}}}
  /* Explore all of symbol table, to evaluate its behavior */
  if (identSeen) {
    TdescrId **w;
    uint maxNbIdBckt = 0, nbUsedBckts = 0, nbId = 0;

    for (w = &symTabHeads[0]; w <= &symTabHeads[SizeSymTab - 1]; ) {
      /* Explore each bucket */
      uint nbIdBckt = 0;
      TdescrId *w1;

      for (w1 = *w++; w1 != NULL; w1 = w1->next) {nbIdBckt++; nbId++;}
      if (nbIdBckt != 0) {
        if (nbIdBckt > maxNbIdBckt) maxNbIdBckt = nbIdBckt;
        nbUsedBckts++;}}
    emitS("\n"); emitS(longToS((long)nbId)); emitS(" id(s)");
    emitS("; nb buckets used: "); emitS(longToS((long)nbUsedBckts));
    emitS("; max bucket filling: "); emitS(longToS((long)maxNbIdBckt));
    emitS("; nbUnivEltChunks: "); emitS(longToS((long)ctrUEC));
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
  register TblkStkElt *pTBS = pTopBlkStk;

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
  memcpy(ptr, x, FullLgt(x));  /* copy name into new allocated space */
  *(ptr + DispNSId) = (TnameAtom)y;	/* set name space */
  return (Tname)ptr;
}

void storeStrCh(char x)
/* Strings are stored in the "name" area, freed only at block exit */
{
  TblkStkElt *pTBS = pTopBlkStk;
  Tname ptr = pTBS->pFreeNameStorage;

  if (ptr == pTBS->endCurNameStoBlk) ptr = storeName((Tname)"", (TnameSpace)0);
					/* to force allocation of a new block */
  *(TnameNC /*~OddCast*/)ptr++ = (TnameAtom)x;  /* store character */
  pTBS->lastStrLitLgt++;
  pTBS->pFreeNameStorage = ptr;
}

/* End DCBLK.C */
