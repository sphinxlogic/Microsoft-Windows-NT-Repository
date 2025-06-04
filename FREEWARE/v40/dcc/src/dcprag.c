/* DCPRAG.C */
/* Y.L. Noyelle, Supelec, France 1994 */
/* Manages d-pragmas */

#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include "dcprag.h"
#include "dcprag.ph"
#include "dcblk.h"
#include "dcdecl.h"
#include "dcdir.h"
#include "dcexp.h"
#include "dcext.h"
#include "dcmain.h"
#include "dcrec.h"
#include "dcrec.ph"
#include "dcrecdir.ph"

#ifdef VMS
#  pragma noinline (checkEndDP, procPrivTo, skipToEndDP)
#endif

DefIsSameName

struct _TCBlk {
  const TsemanElt *lTypeId, *rTypeId;
  TpcTypeElt resulType;
  TlineNb defLineNb;
  Tstring defFileName;
  TCBlk *next;
};

struct _TmngtPriv {
  TmngtPriv *prec;
  const Tchar fileName[1];  /* stub for name, so must be at end */
};
/*~zif Offset(TmngtPriv, fileName) != sizeof(TmngtPriv)-1
				      "Field 'fileName' not at end of struct" */

/* Function profiles */
static TCBlk *allocTCBlk(void), *freeTCBlk(TCBlk *);
static bool getTypeIdent(TpcTypeElt *gottenType);
static TtypeSort intPromotion(TpcTypeElt);
static void addTCB(TkOpTC, TpcTypeElt, TpcTypeElt, TpcTypeElt), checkEndDP(void
  ), initWarnMngt(void), manageTC(TkOpTC, TpcTypeElt, TpcTypeElt, TpcTypeElt,
  bool), procPrivTo(void), skipToEndDP(void), verifyTC(TpcTypeElt, TpcTypeElt,
  TpcTypeElt);

/* Global variables */
static const Ttok zCommaEndDP[] = {NoSwallowTok, COMMA, ENDDPRAG, ENDDIR,
								       EndSTok};

/* External variables */
uint ctrTCB = 0;  /* count of TypeCombination blocks */
bool noWarn = False;
TCBlk *(*pHeadTCBLists)[ModAsgn - AddAsgn + 1 /*~IndexType TkOpTC*/] = NULL;

typedef enum {CastTo, DccCompl,
#ifdef TstAdjFiles
				EndAdj, EndSys,
#endif
						IndexType, Init, NoDefault,
  NoWarn, PopWarn, PrivTo, Public,
#ifdef TstAdjFiles
				   SimAdj, SimSys,
#endif
						   TypeCmbn, Undef, UndfTag,
  WWarn, Zif} TidDP;
static const struct _descPrag {Tname pragName; ThCode hCode;} tabDPrag[] =
	   /* Token-like */
          {{ConvTname("\xA\0BackBranch"),	0x257CA39},
           {ConvTname("\xC\0ComposingHdr"),	0x4718E851},
           {ConvTname("\xA\0DollarSign"),	0x2F2FBD1},
           {ConvTname("\x7\0DynInit"),		0x67C28},
           {ConvTname("\x8\0ExactCmp"),		0x20F2ED},
           {ConvTname("\x8\0FullEnum"),		0x21F8FC},
           {ConvTname("\x7\0Generic"),		0x64A30},
           {ConvTname("\xA\0LiteralCst"),	0x3C62E49},
           {ConvTname("\x8\0LocalAdr"),		0x27351A},
           {ConvTname("\x7\0Masking"),		0x795F1},
           {ConvTname("\x9\0MayModify"),	0xBDF8C6},
           {ConvTname("\xC\0NeverReturns"),	0x621221EB},
           {ConvTname("\x7\0NoBreak"),		0x817AF},
           {ConvTname("\xB\0NonConstExp"),	0x1476A396},
           {ConvTname("\x7\0NotUsed"),		0x8794B},
           {ConvTname("\x7\0OddCast"),		0x7FEC7},
           {ConvTname("\xA\0PortableQM"),	0x4618D4B},
           {ConvTname("\xA\0PseudoVoid"),	0x46A6080},
           {ConvTname("\x9\0ResultPtr"),	0xE0BA94},
           {ConvTname("\xA\0ResultType"),	0x463A773},
           {ConvTname("\x8\0RootType"),		0x2F194E},
           {ConvTname("\x9\0SameValue"),	0xDF8C02},
           {ConvTname("\xC\0SideEffectOK"),	0x70AFBE08},
           {ConvTname("\xC\0SizeOfMemBlk"),	0x734A69AC},
           {ConvTname("\x6\0SizeOK"),		0x1E375},
           {ConvTname("\x7\0Utility"),		0xA4E6B},
           {ConvTname("\xB\0VoidToOther"),	0x191BE16A},
           /*~ zif __index != EndDPragTok - BegDPragTok "Bad 'tabDPrag'1" */
	   /* Non-token, or token with argument */
           {ConvTname("\x6\0CastTo"),		0x10AE0},
           {ConvTname("\xC\0DccCompliant"),	0x40F23DAD},
#ifdef TstAdjFiles
           {ConvTname("\xA\0EndAdjFile"),	0x2F8826E},
           {ConvTname("\xA\0EndSysFile"),	0x2FDE362},
#endif
           {ConvTname("\x9\0IndexType"),	0xB18D4D},
           {ConvTname("\x4\0Init"),		0xEA6},
           {ConvTname("\x9\0NoDefault"),	0xCA3005},
           {ConvTname("\x6\0NoWarn"),		0x1A733},
           {ConvTname("\x7\0PopWarn"),		0x8E91C},
           {ConvTname("\x9\0PrivateTo"),	0xE1DE0D},
           {ConvTname("\x6\0Public"),		0x1D18F},
#ifdef TstAdjFiles
           {ConvTname("\xA\0SimAdjFile"),	0x486AB24},
           {ConvTname("\xA\0SimSysFile"),	0x48C0C18},
#endif
           {ConvTname("\xF\0TypeCombination"),	0xAC6C71FA},
           {ConvTname("\x5\0Undef"),		0x65EB},
           {ConvTname("\x8\0UndefTag"),		0x31C772},
           {ConvTname("\x4\0Warn"),		0x145E},
           {ConvTname("\x3\0zif"),		0x6FA}
           /*~ zif __index != EndDPragTok - BegDPragTok + __extent(TidDP)
							  "Bad 'tabDPrag'2" */};

void dpragMet(void)
{
  int savOldSpCnt = oldSpaceCount, savSpCnt = spaceCount;
  TvalTok resulTok;

  dpragNst++;
  if (--getTokLvl==0 && listTok) {curTok.tok = DPPREFIX; storeTokSource();}
  GetNxtTok();
  if (curTok.tok != IDENT) {
    if (curTok.tok != ENDDPRAG) err0(DPragNameExptd);}
  else {
    const struct _descPrag *ptrTDP;

    for (ptrTDP = &tabDPrag[0]; ptrTDP <= AdLastElt(tabDPrag); ptrTDP++) {
      if (isSameName(curTok.hCod, curTok.name, ptrTDP->hCode, ptrTDP->pragName)
					       ) {  /* found correct d-pragma */
        if (ptrTDP < &tabDPrag[EndDPragTok - BegDPragTok]) {  /* d-pragma
							      coded as token. */
          GetNxtTok();
          resulTok.tok = BegDPragTok + (int)(ptrTDP - &tabDPrag[0]);
          goto okL;}
        curTok.val = (uint)(ptrTDP - &tabDPrag[EndDPragTok -BegDPragTok]);
        resulTok = manageDPrag();
        goto okL1;}}
    err0(UnknDPrag);}
  skipToEndDP();
  resulTok.tok = WHITESPACE;
okL:
  checkEndDP();
okL1:
  curTok = resulTok;
  oldSpaceCount = savOldSpCnt; spaceCount = savSpCnt; getTokLvl++;  /* back
							   to previous state. */
  return;
}

static bool lastIsPopWarnAtMainLvl = True;  /* True because of possible
							       option '-zwa'. */
static uint warnStk = 0;

static void initWarnMngt(void)
{
  lastIsPopWarnAtMainLvl = True;
  warnStk = 0;
}

TvalTok manageDPrag(void)
{
  TvalTok resulTok;

  if (! macroExpand) {  /* do not heed d-pragma if inside macro
	     definition, macro arguments collection, or conditional skipping. */
    resulTok.tok = DPRAG;  
    resulTok.val = curTok.val;}  
  else {
    TidDP dp = (TidDP)curTok.val;

    resulTok.tok = WHITESPACE;
    switch (dp) {
      case CastTo: case IndexType:
        GetNxtTok();
        resulTok.dpType = declType();
        resulTok.tok = (dp == CastTo)? CASTTO : INDEXTYPE;
        goto exitL;
      case DccCompl:
        if (!sysHdrFile) err0(IlgDccCmpl | Warn1);
        else sysHdrFile = sysAdjHdrFile = False;
        break;
#ifdef TstAdjFiles
      case EndAdj: adjustFile = False; sysAdjHdrFile = sysHdrFile; break;
      case EndSys: sysHdrFile = False; sysAdjHdrFile = adjustFile; break;
#endif
      case Init:
        do {
          TsemanElt *ptrId;

          GetNxtTok();
          if (curTok.tok != IDENT) err0(IdExptd);
          else {
            if ((ptrId = curTok.ptrSem) == NULL) errWN(UndefId, curTok.name);
            else if (ptrId->kind!=Obj || ptrId->attribb<Auto && ptrId->attribb!=
					  StatiL)  errWN(NotVarId, curTok.name);
            else if (ptrId->initBefUsd || ptrId->initlz || ptrId->used) errWN(
					    UslInitBU|Warn1|Rdbl, ptrId->nameb);
            else if (adjMacro) ptrId->initlz = True;
            else ptrId->initBefUsd = True;
            GetNxtTok();}
        } while (curTok.tok == COMMA);
        goto exitL;
   /* case IndexType : see CastTo */
      case NoDefault: resulTok.tok = DEFAULT; break;
   /* case NoWarn:  see WWarn */
      case PopWarn:
        noWarn = warnStk & 1;
        warnStk >>= 1;
        if (! (ReallyInsideMacro || insideInclude())) lastIsPopWarnAtMainLvl =
									   True;
        break;
      case PrivTo:
        if (! headerFile) errWS(DPragNotAlwd | Warn2, "PrivateTo");
        headListPrivTo = NULL;  /* do not free preceding list, which is proba-
				   bly refered to by 'listAlwdFiles' members. */
        procPrivTo();
        goto exitL;
      case Public:
        if (! headerFile) errWS(DPragNotAlwd | Warn2, "Public");
        headListPrivTo = NULL;
        break;
#ifdef TstAdjFiles
      case SimAdj: sysAdjHdrFile = adjustFile = True; break;
      case SimSys: sysAdjHdrFile = sysHdrFile = True; break;
#endif
      case TypeCmbn:
        if (nestLvl != 0) err0(TCNotAtLvl0);
        do {  
          bool commut = True;
          TkOpTC oper;
          TpcTypeElt lTyp, rTyp, resTyp;
	    
          if (getTypeIdent(&lTyp)) {
            if (curTok.tok==TILDE) {
              commut = False;
              GetNxtTok();}
            oper = (curTok.tok == ADDOP)
                   ? AddTC + ((TkAdd)curTok.val - Add)
                   : (curTok.tok == STAR)
                     ? MulTC
                     : (curTok.tok == MULOP)
                       ? MulTC + ((TkMul)curTok.val - Mul)
                       : (err0(IlgTCOptr), IlgTcOp);
            if (oper >= AddTC) {
              if (! (oper==AddTC || oper==MulTC)) 
                if  (! commut) err0(NonCommutOper);
                else commut = False;
              if (getTypeIdent(&rTyp)) {
                if (curTok.tok != ARROW) err0(ArrowExptd);
                else {
                  if (getTypeIdent(&resTyp)) {
                    manageTC(oper, lTyp, rTyp, resTyp, commut);
                    if (curTok.tok == COMMA || curTok.tok == ENDDPRAG
								) continue;}}}}}
          skipTok(zCommaEndDP);
        } while (curTok.tok == COMMA);
        goto exitL; 
      case Undef : case UndfTag:
        if (nestLvl != 0) err0(IllUndef);
        do {
          TsemanElt *pId;

          if (NxtTok() != IDENT) {
            err0(IdExptd);
            skipTok(zCommaEndDP);}
          else {
            if (nestLvl == 0) {
              if (dp == UndfTag) {
                *(TnameNC /*~OddCast*/)(curTok.name + DispNSId) =
							    (TnameAtom)TagSpace;
                pId = searchSymTab(curTok.name);}
              else
                pId = curTok.ptrSem;
              if (pId == NULL) errWN(UnknId, curTok.name);
              else if (pId->undef && dp==UndfTag) errWN(AlrdUndef | Warn2,
								   curTok.name);
              else pId->undef = True;}
            GetNxtTok();}
        } while (curTok.tok == COMMA);
        goto exitL;
      case WWarn: case NoWarn:
        warnStk = warnStk<<1 | (uint)noWarn;
        noWarn = (dp == NoWarn);
        if (! (ReallyInsideMacro || insideInclude())) {
          if (!lastIsPopWarnAtMainLvl && !((warnStk & 1) ^ noWarn)) err0(
				(noWarn)? WarnAlrdOff|Warn2 : WarnAlrdOn|Warn2);
          lastIsPopWarnAtMainLvl = False;}
        break;
      case Zif: {
          TresulExp saveCExp = cExp, saveLExp = lExp;
          bool trueZif, saveZifExp = zifExp, saveIfDirExp = ifDirExp;
          TlitString str;

          zifExp = True;
          ifDirExp = False;
          GetNxtTok();
          trueZif = correctExprN(NULL, Bool, True, "zif", False) && !cExp.
							 errEvl && cExp.uVal!=0;
          if ((str = getLitString())!=NULL && trueZif) {
            Tchar *ptrZifMsg, *w;

            MyAlloc(w = ptrZifMsg, initGetStrLit(str));
            while ((*w++ = getNxtStrLitChar()) != '\0') {}
            errWS(ZifWarn | Warn3, ptrZifMsg);
            free(ptrZifMsg);}
          exitBlock();  /* because of getLitString() */
          zifExp = saveZifExp;
          ifDirExp = saveIfDirExp;
          cExp = saveCExp; lExp = saveLExp;}
        goto exitL;
    }
    GetNxtTok();  /* here for good error position */
exitL:
    checkEndDP();
    resulTok.hCod = 0;}  /* to differentiate d-pragma tokens from keywords */
  return resulTok;
}

/*~Undef lastIsPopWarnAtMainLvl,warnStk */

/******************************************************************************/
/*                                UTILITIES                                   */
/******************************************************************************/

static void addTCB(TkOpTC op, TpcTypeElt lType, TpcTypeElt rType, TpcTypeElt
									resType)
{
  TCBlk *pcTCB, *prec = NULL;
  register const TsemanElt *lTypId = lType->typeId;

  if (pHeadTCBLists == NULL) {
    /* Allocate and initialize table of TypeCombinations for each operator
       (Add -> Mod). */
    TCBlk **ptr;

    MyAlloc(pHeadTCBLists, sizeof(*pHeadTCBLists));
    for (ptr = &(*pHeadTCBLists)[AddTC]; ptr <= &(*pHeadTCBLists)[ModTC];)
      *ptr++ = NULL;}
  for (pcTCB = (*pHeadTCBLists)[op]; pcTCB != NULL; pcTCB = pcTCB->next) {
    if (pcTCB->lTypeId==lTypId && pcTCB->rTypeId==rType->typeId) {
      static Tchar tCOperRepre[] = " ";
#include "dctxttok.h"

      tCOperRepre[0] = txtAsgnTok[AddAsgn + (int)op][0];
      errWFName(TCAlrdDef, pcTCB->defLineNb, pcTCB->defFileName, lTypId->
			     nameb, nameToS(rType->typeId->nameb), tCOperRepre);
      longjmp(*curErrRet, 1);}
    prec = pcTCB;}
  {
    TCBlk *w = allocTCBlk();

    w->next = NULL;
    w->lTypeId = lTypId;
    w->rTypeId = rType->typeId;
    w->resulType = resType;
    w->defLineNb = lineNb;
    w->defFileName = curFileName;
    if (prec == NULL) (*pHeadTCBLists)[op] = w;
    else prec->next = w;}
}

static AllocXElt(allocTCBlk, TCBlk *, ctrTCB, ;)

static void checkEndDP(void)
{
  if (curTok.tok != ENDDPRAG) {
    err0(EndDPragLineExptd);
    skipToEndDP();}
  dpragNst--;  /* reset here only so that ERror->WArning inside d-pragma,
	    even if look-ahead (e.g. in call to declType() in __sameType() ). */
}

Tstring dpName(Ttok pragNb)
{
  static Tstring listStrings[] = {DPragPrefix, NULL, DPragSuffix};
  Tstring *w, *limW = &listStrings[(pragNb == DPRAG)? NbElt(listStrings) - 1 :
							    NbElt(listStrings)];
  register Tchar *ptrBuf = &tokTxtBuf[0];

  listStrings[1] = (Tstring)(tabDPrag[(pragNb == DPRAG)? (curTok.val +
	(EndDPragTok - BegDPragTok)) : (uint)(pragNb - BegDPragTok)].pragName +
								      LgtHdrId);
  for (w = &listStrings[0]; w != limW; ) {
    register Tstring w1 = *w++;

    while (*w1 != '\0') *ptrBuf++ = *w1++;}
  *ptrBuf = '\0';
  return &tokTxtBuf[0];
}

static FreeXElt(freeTCBlk, TCBlk *, ctrTCB, ; , next)

void freeTCB(void)
{
  if (pHeadTCBLists != NULL) {
    TCBlk **ptrInArr;

    for (ptrInArr = &(*pHeadTCBLists)[AddTC]; ptrInArr <= &(*pHeadTCBLists)[
								     ModTC]; ) {
      TCBlk *curTCB = *ptrInArr++;

      while (curTCB != NULL) curTCB = freeTCBlk(curTCB);}
    free(pHeadTCBLists);
    pHeadTCBLists = NULL;}
}

static bool getTypeIdent(TpcTypeElt *gottenType)
{
  bool resul = False;
  TsemanElt *ptrId;
  
  GetNxtTok();
  if (curTok.tok == IDENT) {
    if ((ptrId = curTok.ptrSem) == NULL) {
      DefSem1(artifDef, Type, True, True, False);
    
      errWN(UndefId, curTok.name);
      (void)defineId(artifDef);
      *gottenType = NULL;
      resul = True;}
    else if (ptrId->kind == Type) {
      *gottenType = (ptrId->namedType!=NULL && (!IsTypeSort(ptrId->namedType,
		NumEnum) || !ptrId->namedType->paralTyp))? (errWT(TypeNotParal,
				    ptrId->namedType), NULL) : ptrId->namedType;
      resul = True;}
    else errWN(NotTypeId, curTok.name);
    GetNxtTok();}
  else err0(IdExptd);
  return resul;
}

void initPrag(void)
{
  initWarnMngt();
}

static TtypeSort intPromotion(TpcTypeElt x)
{
  TtypeSort ts = x->typeSort;

  if (ts == Enum) return Int;
  if (ts < Int)
#if USHRT_MAXC == UINT_MAXC
    if (chkPortbl && ts==UShort) errWS(NonPortTC | Warn2, IntStr);
#endif
    return Int;
  return ts;
}

bool isFNameVisible(Tstring fileName, const TsemanElt *pTag)
{
  register const TmngtPriv *w = pTag->listAlwdFiles;

  fileName = skipPath(fileName);
  do {
    if (strJokerEq(w->fileName, fileName)) return True;
  } while ((w = w->prec) != NULL);
  return False;
}

static void manageTC(TkOpTC oper, TpcTypeElt lType, TpcTypeElt rType,
						TpcTypeElt resType, bool commut)
{
  jmp_buf localJmpBuf, *savErrRet;

  if (nestLvl!=0 || lType==NULL || rType==NULL) return; /* ignore combination */
  if (resType != NULL) {
    verifyTC(lType, rType, resType);
    if (oper == ModTC) {
      if (IsTypeSort(resType, Flt)) errWSTT(IlgTCModOpnd, NULL, lType, rType);
      if (lType != resType) err0(IncorTCModRes | Warn2 | PossErr);}}
  if (lType == rType) commut = False;
  savErrRet = curErrRet;
  curErrRet = &localJmpBuf /*~LocalAdr*/;
  if (setjmp(*curErrRet) == 0) {
    addTCB(oper, lType, rType, resType);
    if (commut) addTCB(oper, rType, lType, resType);
    if (resType != NULL)
      switch (oper) {
        case AddTC:
          if (resType == lType) addTCB(SubTC, lType, rType, resType);
							    /* 'generic' type */
          break;
        case MulTC:
          addTCB(DivTC, resType, lType, rType);
          if (IsTypeSort(resType, WhoEnum)) addTCB(ModTC, resType, lType,
								       resType);
          if (commut) {
            addTCB(DivTC, resType, rType, lType);
            if (IsTypeSort(resType, WhoEnum)) addTCB(ModTC, resType, rType,
								      resType);}
          break;
        case DivTC:
          if (IsTypeSort(resType, WhoEnum)) addTCB(ModTC, lType, rType, lType);
          break;
        /*~NoDefault*/}}
  curErrRet = savErrRet;
}

static void procPrivTo(void)
{
  GetNxtTok();
  do {
    TlitString str;

    if ((str = getLitString())!=NULL && headerFile) {
      register Tchar *pc;
      Tstring sp;
      TmngtPriv *w;

      MyAlloc(w, initGetStrLit(str) + Offset(TmngtPriv, fileName));
      pc = (Tchar * /*~OddCast*/)&w->fileName[0];
      while ((*pc++ = getNxtStrLitChar()) != '\0') {};  /* note indicated
								   file name. */
      /* Suppress possible path */
      if ((sp = skipPath(w->fileName)) != &w->fileName[0]) {
        size_t lgt = strlen(sp) + 1;
  
        memmove((Tchar * /*~OddCast*/)&w->fileName[0], sp, lgt);
        w = realloc(w, lgt + Offset(TmngtPriv, fileName));}
      w->prec = headListPrivTo;
      headListPrivTo = w;}
    exitBlock();  /* because of getLitString() */
  } while (Found(COMMA));
}

static void skipToEndDP(void)
{
  static const Ttok zEndDPrag[] = {NoSwallowTok, ENDDPRAG, ENDDIR, EndSTok};

  skipTok(zEndDPrag);
  if (curTok.tok == ENDDIR) {dirLineFl = True; backUp();}  /* to get back
							ENDDIR as next token. */
}

TpcTypeElt searchTC(TkOpTC oper)
/* Returns usually QuasiNULLval(TpcTypeElt) if no combination found */
{
  TpcTypeElt lType, rType;
  register TsemanElt *lTypeId, *rTypeId;
  TCBlk *ptrTCoper = (*pHeadTCBLists)[oper];

  if (ptrTCoper==NULL || (lType = lExp.type)==NULL || !lType->paralTyp ||
       cExp.type==NULL || !cExp.type->paralTyp) return QuasiNULLval(TpcTypeElt);
  /* Prepare to row up hierarchy, in order to find possible combination between
     ancestors. */
  do {
    lTypeId = lType->typeId;
    rType = cExp.type;
    do {
      register TCBlk *pcTCB = ptrTCoper;

      rTypeId = rType->typeId;
      do {
        if (lTypeId==pcTCB->lTypeId && rTypeId==pcTCB->rTypeId) return
							       pcTCB->resulType;
      } while ((pcTCB = pcTCB->next) != NULL);
    } while (!rType->rootTyp && (rType = rTypeId->type)!=NULL && rType->
								      paralTyp);
  } while (!lType->rootTyp && (lType = lTypeId->type)!=NULL && lType->paralTyp);
  if (InsideInterval(oper, MulTC, DivTC) &&
  /* Search whether one operand appears in a type combination involving opera-
     tor '*' ('normal' type combination for non dimensionless type). If so,
     complain about missing typeCombination (cf Volt*Volt/Ohm). */
		     ((ptrTCoper = (*pHeadTCBLists)[MulTC])!=NULL || (ptrTCoper
					    = (*pHeadTCBLists)[DivTC])!=NULL)) {
    lType = lExp.type;
    do {
      TsemanElt *typId = lType->typeId;
      register TCBlk *pcTCB = ptrTCoper;

      do {
        if (typId==pcTCB->lTypeId) goto noTCFoundL;
      } while ((pcTCB = pcTCB->next) != NULL);
    } while (!lType->rootTyp && (lType = lType->typeId->type)!=NULL && lType->
								      paralTyp);
    rType = cExp.type;
    if (rType != lExp.type) {  /* test to speed up */
      do {
        TsemanElt *typId = rType->typeId;
        register TCBlk *pcTCB = ptrTCoper;

        do {
          if (typId==pcTCB->rTypeId) goto noTCFoundL;
        } while ((pcTCB = pcTCB->next) != NULL);
      } while (!rType->rootTyp && (rType = rType->typeId->type)!=NULL &&
							     rType->paralTyp);}}
  return QuasiNULLval(TpcTypeElt);
noTCFoundL:
  {
    TnameBuf buf;
    static Tchar operBuf[] = " ";

    bufNameToS(cExp.type->typeId->nameb, buf);
    operBuf[0] = paramTxt[0];
    errWNSS(NoTCAllows | Warn1, lExp.type->typeId->nameb, operBuf, buf);}
  return NULL;
}

static void verifyTC(TpcTypeElt lType, TpcTypeElt rType, TpcTypeElt resType)
{
  TtypeSort tsl = intPromotion(lType), tsr = intPromotion(rType), tsRes =
							  intPromotion(resType);
    
#if UINT_MAXC == ULONG_MAXC
  if (tsl<tsr) {TtypeSort tsaux=tsr;; tsr=tsl; tsl=tsaux;}
  if (chkPortbl && tsl==Long && tsr==UInt) errWS(NonPortTC | Warn2, LongStr);
#else
  if (tsl<tsr) tsl = tsr;
#endif 
  if (tsRes!=tsl && !(tsl==Double && tsRes==Float)) {
    Tchar buf[SizTypTxtBuf + 1];

    strcpy(buf, typeToS1(resType));
    errWSTT(IncorTCResTyp, buf, lType, rType);}
}

/* End DCPRAG.C */
