/* DCPRAG.C */
/* Y.L. Noyelle, Supelec, France 1994 */
/* Management of d-pragmas */

#include <stdlib.h>
#ifdef EXIT_FAILURE
#  undef EXIT_FAILURE
#endif
#include <setjmp.h>
#include <string.h>
#include "dcprag.h"
#include "dcprag.ph"
#include "dcblk.h"
#include "dcdecl.h"
#include "dcdir.h"
#include "dcdir.ph"
#include "dcexp.h"
#include "dcext.h"
#include "dcinst.ph"
#include "dcmain.h"
#include "dcrec.h"
#include "dcrec.ph"
#include "dcrecdir.ph"

#ifdef VMS
#  pragma noinline (processPriv)
#endif

struct _tcBlk {
  const TsemanElt *lTypeId, *rTypeId;
  TpcTypeElt resulType;
  TlineNb defLineNb;
  Tstring defFileName;
  TCBlk *next;
};

struct _tMngtPriv {
  TmngtPriv *prev, *chain;
  const Tchar fileName[1];  /* stub for name, so must be at end */
};
/*~zif Offset(TmngtPriv, fileName) != sizeof(TmngtPriv)-1
				      "Field 'fileName' not at end of struct" */
DefRepresType

/* Function profiles */
static TCBlk *allocTCBlk(void), *freeTCBlk(TCBlk *);
static bool getTypeIdent(TpTypeElt *, bool);
static TtypeSort intPromotion(TpcTypeElt);
static void addTCB(TkOpTC, TpcTypeElt, TpcTypeElt, TpcTypeElt),
  initWarnMngt(void),
  manageTC(TkOpTC, TpcTypeElt, TpcTypeElt, TpcTypeElt, bool),
  processPriv(TlitString), skipToComma(void),
  verifyTC(TpcTypeElt, TpcTypeElt, TpcTypeElt);

/* Global variables */
static TmngtPriv *headPrivBlks = NULL;

/* External variables */
uint ctrTCB = 0;  /* count of TypeCombination blocks */
TdpNst dpragNst = 0;  /* level of imbrication of d-pragmas (imbrication possible
							  because of macros). */
TmngtPriv *headListPrivTo;
bool noWarn;	  /* do not report warnings */
TCBlk *(*pHeadTCBLists)[ModAsgn - AddAsgn + 1 /*~IndexType TkOpTC*/] = NULL;

static Tname tabDPragN[/*~IndexType Tdprag*/] = {
	/* Non-token, or token with possible argument (then '\1' at end (cf also
	   dpName() ). */
	/* Sorted by order of increasing h-codes (see tabDPragHC, just next; see
	   also enumerated type Tdprag in "dc.th"). */
	ConvTname("\x3\0zif\1")		/*~zif __index!=ZIF "Pb"*/,
	ConvTname("\x4\0Init\1"),
	ConvTname("\x4\0Warn\1"),
	ConvTname("\x5\0Saved"),
	ConvTname("\x5\0Undef\1"),
	ConvTname("\x6\0CastTo\1")	/*~zif __index!=CASTTO "Pb"*/,
	ConvTname("\x6\0Ignore"),
	ConvTname("\x6\0NoWarn\1"),
	ConvTname("\x6\0Public\1"),
	ConvTname("\x6\0SizeOK"),
	ConvTname("\x7\0Generic\1")	/*~zif __index!=GENERIC "Pb"*/,
	ConvTname("\x7\0DynInit"),
	ConvTname("\x7\0Masking"),
	ConvTname("\x7\0OddCast"),
	ConvTname("\x7\0NoBreak"),
	ConvTname("\x7\0NotUsed")	/*~zif __index!=NOTUSED "Pb"*/,
	ConvTname("\x7\0PopWarn\1"),
	ConvTname("\x7\0Private\1"),
	ConvTname("\x7\0Utility"),
	ConvTname("\x8\0AddedObj"),
	ConvTname("\x8\0ExactCmp"),
	ConvTname("\x8\0FullEnum")	/*~zif __index!=FULLENUM "Pb"*/,
	ConvTname("\x8\0LocalAdr"),
	ConvTname("\x8\0RootType\1"),
	ConvTname("\x8\0UndefTag\1"),
	ConvTname("\x9\0IndexType\1"),
	ConvTname("\x9\0MayModify")	/*~zif __index!=MAYMODIFY "Pb"*/,
	ConvTname("\x9\0NoDefault\1"),
	ConvTname("\x9\0SameValue"),
	ConvTname("\x9\0ResultPtr"),
	ConvTname("\x9\0PrivateTo\1"),
	ConvTname("\xA\0BackBranch")	/*~zif __index!=BCKBRCH "Pb"*/,
	ConvTname("\xA\0DollarSign"),
#ifdef TstAdjFiles
	ConvTname("\xA\0EndAdjFile\1"),
	ConvTname("\xA\0EndSysFile\1"),
#endif
	ConvTname("\xA\0LiteralCst")	/*~zif __index!=LITCST "Pb"*/,
	ConvTname("\xA\0PortableQM"),
	ConvTname("\xA\0ResultType"),
	ConvTname("\xA\0PseudoVoid\1"),
#ifdef TstAdjFiles
	ConvTname("\xA\0SimAdjFile\1"),
	ConvTname("\xA\0SimSysFile\1"),
#endif
	ConvTname("\xB\0NonConstExp")	/*~zif __index!=NONCONST "Pb"*/,
	ConvTname("\xB\0VoidToOther"),
	ConvTname("\xC\0DccCompliant\1"),
	ConvTname("\xC\0ComposingHdr"),
	ConvTname("\x12\0EndLongjmpCallable\1"),
	ConvTname("\xC\0NeverReturns\1")/*~zif __index!=NEVRET "Pb"*/,
	ConvTname("\xC\0SideEffectOK"),
	ConvTname("\xC\0SizeOfMemBlk"),
	ConvTname("\xF\0TypeCombination\1")
	/*~ zif __index != (Tdprag)__extent(Tdprag) "Bad 'tabDPragN'" */};

static ThCode tabDPragHC[/*~IndexType Tdprag*/] = {
        AdaptHCode(0x6FA)		/*~zif __index!=ZIF "Pb"*/,
        AdaptHCode(0xEA6),		/* Init */
        AdaptHCode(0x145E),		/* Warn */
        AdaptHCode(0x5C70),		/* Saved */
        AdaptHCode(0x65EB),		/* Undef */
        AdaptHCode(0x10AE0)		/*~zif __index!=CASTTO "Pb"*/,
        AdaptHCode(0x16044),		/* Ignore */
        AdaptHCode(0x1A733),		/* NoWarn */
        AdaptHCode(0x1D18F),		/* Public */
        AdaptHCode(0x1E375),		/* SizeOK */
        AdaptHCode(0x64A30)		/*~zif __index!=GENERIC "Pb"*/,
        AdaptHCode(0x67C28),		/* DynInit */
        AdaptHCode(0x795F1),		/* Masking */
        AdaptHCode(0x7FEC7),		/* OddCast */
        AdaptHCode(0x817AF),		/* NoBreak */
        AdaptHCode(0x8794B)		/*~zif __index!=NOTUSED "Pb"*/,
        AdaptHCode(0x8E91C),		/* PopWarn */
        AdaptHCode(0x908DA),		/* Private */
        AdaptHCode(0xA4E6B),		/* Utility */
        AdaptHCode(0x178C25),		/* AddedObj */
        AdaptHCode(0x20F2ED),		/* ExactCmp */
        AdaptHCode(0x21F8FC)		/*~zif __index!=FULLENUM "Pb"*/,
        AdaptHCode(0x27351A),		/* LocalAdr */
        AdaptHCode(0x2F194E),		/* RootType */
        AdaptHCode(0x31C772),		/* UndefTag */
        AdaptHCode(0xB18D4D),		/* IndexType */
        AdaptHCode(0xBDF8C6)		/*~zif __index!=MAYMODIFY "Pb"*/,
        AdaptHCode(0xCA3005),		/* NoDefault */
        AdaptHCode(0xDF8C02),		/* SameValue */
        AdaptHCode(0xE0BA94),		/* ResultPtr */
        AdaptHCode(0xE1DE0D),		/* PrivateTo */
        AdaptHCode(0x257CA39)		/*~zif __index!=BCKBRCH "Pb"*/,
        AdaptHCode(0x2F2FBD1),		/* DollarSign */
#ifdef TstAdjFiles
        AdaptHCode(0x2F8826E),		/* EndAdjFile */
        AdaptHCode(0x2FDE362),		/* EndSysFile */
#endif
        AdaptHCode(0x3C62E49)		/*~zif __index!=LITCST "Pb"*/,
        AdaptHCode(0x4618D4B),		/* PortableQM */
        AdaptHCode(0x463A773),		/* ResultType */
        AdaptHCode(0x46A6080),		/* PseudoVoid */
#ifdef TstAdjFiles
        AdaptHCode(0x486AB24),		/* SimAdjFile */
        AdaptHCode(0x48C0C18),		/* SimSysFile */
#endif
        AdaptHCode(0x1476A396)		/*~zif __index!=NONCONST "Pb"*/,
        AdaptHCode(0x191BE16A),		/* VoidToOther */
        AdaptHCode(0x40F23DAD),		/* DccCompliant */
        AdaptHCode(0x4718E851),		/* ComposingHdr */
        AdaptHCode(0x495D7AD2),		/* EndLongjmpCallable */
        AdaptHCode(0x621221EB)		/*~zif __index!=NEVRET "Pb"*/,
        AdaptHCode(0x70AFBE08),		/* SideEffectOK */
        AdaptHCode(0x734A69AC),		/* SizeOfMemBlk */
        AdaptHCode(0xAC6C71FA)		/* TypeCombination */
	/*~ zif __index != (Tdprag)__extent(Tdprag) "Bad 'tabDPragHC'" */};

static bool lastIsPopWarnAtMainLvl = True;  /* True because of possible
							       option '-zwa'. */
static uint warnStk = 0;

static void initWarnMngt(void)
{
  lastIsPopWarnAtMainLvl = True;
  warnStk = 0;
}

TvalTok manageDPrag(void)
/* Done that way because of possible macro-expansion inside d-pragma */
{
  TvalTok resulTok;

  dpragNst++;
  if (transmitDP()) resulTok.tok = DPRAG;
  else {  /* 'execute' d-pragma */
    register int i = 0, j = NbElt(tabDPragHC) - 1;
    register Tdprag curDP;
    TtokLvl savGetTokLvl = getTokLvl;
    bool savME = macroExpand, savNE = noExpand, savGArgC = gblArgColl;

    macroExpand = True; noExpand = False; gblArgColl = False;
    if (!dirLineFl && (getTokLvl = tokLvl0 - 1)==0) storeTokTxt(DPragPrefix);
    resulTok.tok = WHITESPACE;
    if (NxtTok() != IDENT) {
      if (curTok.tok == ENDDPRAG) goto exitL;
      err0(DPragNameExptd);
      analTok();
      goto exitL1;}
    /* Dichotomic search */
    do {
      curDP = (Tdprag)((i + j) >> 1);
      if (curTok.Hcod > tabDPragHC[curDP]) {
        i = (int)curDP + 1; continue;}
      if (curTok.Hcod<tabDPragHC[curDP] || memcmp(tabDPragN[curDP], curTok.
	  IdName, FullLgt(curTok.IdName))!=0 /* this case (same hCode for two
				different d.pragmas) put arbitrarily here. */) {
        j = (int)curDP - 1; continue;}
      /* Found */
      if (*(tabDPragN[curDP]+FullLgt(tabDPragN[curDP])) == 0) {  /* token-like
								    d-pragma. */
        resulTok.tok = DPTOK;
        resulTok.Val = (uint)curDP;}
      else {  /* d-pragma to be interpreted */
        switch (curDP) {
          case CASTTO: case INDEXTYPE:
            GetNxtTok();
            resulTok.DpType = declType();
            goto rootTypL;
          case DCCCOMPL:
            if (!sysHdrFile) err0(IlgDccCmpl | Warn1);
            else sysHdrFile = sysAdjHdrFile = False;
            break;
          case ENDLJCLLBL:
            if (setjmpBlkLvl == 0) errIlgDP(curDP);
            else setjmpBlkLvl = 0;
            break;
#ifdef TstAdjFiles
          case ENDADJ: adjustFile = False; sysAdjHdrFile = sysHdrFile; break;
          case ENDSYS: sysHdrFile = False; sysAdjHdrFile = adjustFile; break;
#endif
       /* case GENERIC: see PSEUDOVOID */
          case INIT:
            do {
              TsemanElt *ptrId;

              GetNxtTok();
              if (curTok.tok != IDENT) err0(IdExptd);
              else {
                if ((ptrId = curTok.PtrSem) == NULL) errWN(UndeclId, curTok.
									IdName);
                else if (ptrId->Kind!=Obj || ptrId->Attribb<Auto) errWN(
						       NotVarId, curTok.IdName);
                else if (ptrId->InitBefUsd || ptrId->Initlz || ptrId->Used
			   ) errWN(UslInitBU|Warn1|Rdbl, ptrId->PdscId->idName);
                else if (adjMacro) ptrId->Initlz = True;
                else ptrId->InitBefUsd = True;
                GetNxtTok();}
            } while (curTok.tok == COMMA);
            goto exitL;
       /* case INDEXTYPE: see CASTTO */
       /* case NEVRET: see PSEUDOVOID */
          case NODEFAULT: resulTok.tok = DEFAULT; resulTok.Hcod = 0; break;
       /* case NOWARN:  see WARN */
          case POPWARN:
            noWarn = warnStk & 1;
            warnStk >>= 1;
            if (hereOrAloneInMac() && !insideIncldFile()
						) lastIsPopWarnAtMainLvl = True;
            break;
          case GENERIC:
          case NEVRET:
          case PSEUDOVOID:
            if (NxtTok() != IDENT) goto rootTypL;
            do {
              TpTypeElt type;
              const TsemanElt *ptrId = curTok.PtrSem;

              if (! adjustFile) goto ilgDPL;
              if (curTok.tok != IDENT) {err0(IdExptd); continue;}
              else if (ptrId != NULL) {
                if (ptrId->Kind!=Obj || (type = ptrId->type)==NULL || !IsFct(
					  type)) errWN(NotFctId, curTok.IdName);
                else {
                  if (curDP == GENERIC) {
                    const TdeclElt *parElt;

                    for (parElt = type->ParamList; parElt != NULL; parElt =
    								    parElt->cdr)
                      if (parElt->ParQal == ResulTypV) goto ilgDPL;
                    type->Generiq = True;
                    makeGeneric(type);}
                  else {
                    TpcTypeElt retType = NxtTypElt(type);

                    if (retType==NULL || (retType->typeSort == Void)!=
						 (curDP == NEVRET)) goto ilgDPL;
                    type->PvNr = True;}}}
              goto skip1L;
ilgDPL:
              errIlgDP(curDP);
skip1L:
              GetNxtTok();
            } while (Found(COMMA));
            goto exitL;
          case PRIVATE:
          case PRIVATETO:
          case PUBLIC:
            if (! insideHdrFile) errWS(DPragNotAlwd | Warn2, dpName(curDP));
            headListPrivTo = NULL;  /* do not free preceding list, which is
			      probably refered to by 'ListAlwdFiles' members. */
            if (curDP == PUBLIC) break;
            if (curDP == PRIVATE) {
              if (insideHdrFile) {
                const Tchar *posDot;
                const Tchar *nakedFName = IndicatedFName(curFileName);

                enterBlock();  /* for string storage reclaiming purpose */
                for (posDot = NULL;; posDot = SearchDot(nakedFName)) {
                  register const Tchar *ptrC = nakedFName - 1;
                  TlitString fileName = ptrFreeIdSpace();

                  do {storeStrCh(*++ptrC);} while (ptrC!=posDot && *ptrC!='\0');
                  if (posDot != NULL) {  /* creates corresponding body file
									name. */
                    ptrC = &CSuffix[0];
                    do {storeStrCh(*++ptrC);} while (*ptrC != '\0');}
                  processPriv(fileName);
                  if (posDot != NULL) break;}
                exitBlock();}  /* because of 'getLitString()' */
              break;}
            /* else PRIVATETO */
            GetNxtTok();
            do {
              TlitString str;

              if ((str = getLitString())!=NULL && insideHdrFile) processPriv(
									   str);
              exitBlock();  /* because of 'getLitString()' */
            } while (Found(COMMA));
            goto exitL;
          case ROOTTYP:
            if (NxtTok() == IDENT) {
              do {
                TpTypeElt type;

                if (getTypeIdent(&type, False) && type!=NULL)
                  if (!NxtIsTypId(type) || !adjustFile) errIlgDP(ROOTTYP);
                  else type->RootTyp = True;
              } while Found(COMMA);
              goto exitL;}
rootTypL:
            resulTok.tok = DPTOK;
            resulTok.Val = (uint)curDP;
            goto exitL;
#ifdef TstAdjFiles
          case SIMADJ: sysAdjHdrFile = adjustFile = True; break;
          case SIMSYS: sysAdjHdrFile = sysHdrFile = True; break;
#endif
          case TYPECMBN:
            if (nestLvl != 0) err0(TCNotAtLvl0);
            do {  
              bool commut = True;
              TkOpTC oper;
              TpTypeElt lType, rType, resType;
	    
              if (getTypeIdent(&lType, True)) {
                if (curTok.tok==TILDE) {
                  commut = False;
                  GetNxtTok();}
                oper = (curTok.tok == ADDOP)
                       ? AddTC + ((TkAdd)curTok.Val - Add)
                       : (curTok.tok == STAR)
                         ? MulTC
                         : (curTok.tok == MULOP)
                           ? MulTC + ((TkMul)curTok.Val - Mul)
                           : (err0(IlgTCOptr), IlgTcOp);
                if (oper >= AddTC) {
                  if (! (oper==AddTC || oper==MulTC)) 
                    if  (! commut) err0(NonCommutOper);
                    else commut = False;
                  if (getTypeIdent(&rType, True)) {
                    if (curTok.tok != ARROW) err0(ArrowExptd);
                    else {
                      if (oper==DivTC && (CompatType(lType, rType, NoCheck) ||
		  	  CompatType(rType, lType, NoCheck))) {  /* one is
						      ascendant of the other. */
                        GetNxtTok();
                        if ((resType = declType())!=NULL && (resType->NoOwner ||
					       !IsTypeSort(resType, NumEnum))) {
                          errWT(IlgType, resType);
                          FreeExpType2(resType);
                          resType = NULL;}}
                      else if (! getTypeIdent(&resType, True)) goto skipTL;
                      manageTC(oper, lType, rType, resType, commut);
                      if (curTok.tok!=COMMA && curTok.tok!=ENDDPRAG) errWS(
						   CommaOr, errTxt[EndDPExptd]);
skipTL:;             }}}}
              skipToComma();
            } while (curTok.tok == COMMA);
            goto exitL; 
          case UNDEF: case UNDEFTAG:
            if (nestLvl != 0) err0(IllUndef);
            do {
              TsemanElt *pId;

              if (NxtTok() != IDENT) {
                err0(IdExptd);
                skipToComma();}
              else {
                if (nestLvl == 0) {
                  pId = (curDP == UNDEFTAG)? searchSymTab(storeName(curTok.
					     IdName, TagSpace)) : curTok.PtrSem;
                  if (pId == NULL) errWN(UnknId, curTok.IdName);
                  else if (pId->Undf) {if (curDP == UNDEFTAG) errWN(AlrdUndef |
							 Warn2, curTok.IdName);}
                  else pId->Undf = True;}
                GetNxtTok();}
            } while (curTok.tok == COMMA);
            goto exitL;
          case WARN: case NOWARN:
            warnStk = warnStk<<1 | (uint)noWarn;
            noWarn = (curDP == NOWARN);
            if (hereOrAloneInMac() && !insideIncldFile()) {
              if (!lastIsPopWarnAtMainLvl && !((warnStk & 1) ^ noWarn)) err0(
				(noWarn)? WarnAlrdOff|Warn2 : WarnAlrdOn|Warn2);
              lastIsPopWarnAtMainLvl = False;}
            break;
          case ZIF: {
              const TresulExp savCExp = cExp, savLExp = lExp;
              TlitString str;
              bool trueZif, savZifExp = zifExp, savIfDirExp = ifDirExp;
              Trchbl savNSR = nxtStmtRchbl;

              zifExp = True;
              ifDirExp = False;
              GetNxtTok();
              trueZif = correctExprN(NULL, mskBool & WhoEnumBool, True, "zif",
					 False) && !cExp.ErrEvl && cExp.Uval!=0;
              if ((str = getLitString())!=NULL && trueZif) {
                Tchar *ptrZifMsg, *w;

                w = ptrZifMsg = allocPermSto(initGetStrLit(str));
                while ((*w++ = nxtStrLitChar()) != '\0') {}
                errWS(ZifWarn | Warn3, ptrZifMsg);
                freeLastPermSto();
                resetGetStrLit();}
              exitBlock();  /* because of 'getLitString()' */
              zifExp = savZifExp;
              ifDirExp = savIfDirExp;
              cExp = savCExp; lExp = savLExp; nxtStmtRchbl = savNSR;}
            goto exitL;
          default: sysErr(ExCod7);}}
      goto exitL1;
    } while (i <= j);  /* end binary search loop */
    err0(UnknDPrag);
exitL1:
    GetNxtTok();
exitL:
    if (curTok.tok != ENDDPRAG) err0(EndDPExptd);
    {
      static const Ttok zEndDP[] = {NoSwallowTok, ENDDPRAG, ENDDIR, EndSTok};

      skipTok(zEndDP);  /* to be called anyway, to decrement dpragNst; this
		decrementation must be done here, NOT when recognizing ENDDPRAG,
		because of potential errors that have to be categorized
		'warnings', for we are inside a (pseudo-)comment. */
    }
    if (curTok.tok != ENDDPRAG) {
      if (curTok.tok == ENDDIR) {dirLineFl = True; backUp();}  /* to get
						   ENDDIR back as next token. */
      dpragNst--;}
    macroExpand = savME; noExpand = savNE; gblArgColl = savGArgC;
    getTokLvl = savGetTokLvl;}
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
  register const TsemanElt *lTypId = lType->TypeId;

  if (pHeadTCBLists == NULL) {
    /* Allocate and initialize table of TypeCombinations for each operator
       (Add -> Mod). */
    TCBlk **ptr;

    MyAlloc(pHeadTCBLists, sizeof(*pHeadTCBLists));
    for (ptr = &(*pHeadTCBLists)[AddTC]; ptr <= &(*pHeadTCBLists)[ModTC];)
      *ptr++ = NULL;}
  for (pcTCB = (*pHeadTCBLists)[op]; pcTCB != NULL; pcTCB = pcTCB->next) {
    if (pcTCB->lTypeId==lTypId && pcTCB->rTypeId==rType->TypeId) {
      static Tchar tCOperRepre[] = " ";
#include "dctxttok.h"

      tCOperRepre[0] = txtAsgnTok[AddAsgn + (int)op][0];
      errWFName(TCAlrdDef, pcTCB->defLineNb, pcTCB->defFileName, lTypId->
	   PdscId->idName, nameToS(rType->TypeId->PdscId->idName), tCOperRepre);
      longjmp(*curErrRet, 1);}
    prec = pcTCB;}
  {
    TCBlk *w = allocTCBlk();

    w->next = NULL;
    w->lTypeId = lTypId;
    w->rTypeId = rType->TypeId;
    w->resulType = resType;
    w->defLineNb = lineNb;
    w->defFileName = curFileName;
    if (prec == NULL) (*pHeadTCBLists)[op] = w;
    else prec->next = w;}
}

static AllocXElt(allocTCBlk, TCBlk, ctrTCB, ;)

Tstring dpName(Tdprag pragNo)
{
  static Tstring listStrings[] = {DPragPrefix, NULL, DPragSuffix};
  const Tstring *w;
  register Tchar *ptrBuf = &tokTxtBuf[0];

  listStrings[1] = (Tstring)(tabDPragN[pragNo] + LgtHdrId);
  for (w = &listStrings[0]; w != &listStrings[NbElt(listStrings)]; ) {
    register Tstring w1 = *w++;

    while (*w1 > '\1') *ptrBuf++ = *w1++;}
  *ptrBuf = '\0';
  return &tokTxtBuf[0];
}

void freeTypeCmbn(void)
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

static FreeXElt(freeTCBlk, TCBlk *, ctrTCB, ; , next)

Tstring lastPrivFileName(const TsemanElt *x)
{
  return &(x->ListAlwdFiles->fileName[0]);
} 

static bool getTypeIdent(TpTypeElt *pGottenType, bool fl)
/* 'fl' used to get better position of possible error messages */
{
  bool resul = False;
  const TsemanElt *ptrId;

  if (fl) GetNxtTok();
  *pGottenType = NULL;
  if (curTok.tok == IDENT) {
    if ((ptrId = curTok.PtrSem) == NULL) {
      DefSem1(artifDef, Type, True, True, False);

      errWN(UndeclId, curTok.IdName);
      (void)defineId(artifDef);
      resul = True;}
    else if (ptrId->Kind == Type) {
      if (fl && ptrId->NamedType!=NULL && (!IsTypeSort(ptrId->NamedType,
		NumEnum) || !ptrId->NamedType->ParalTyp)) errWT(TypeNotParal,
							      ptrId->NamedType);
      else *pGottenType = ptrId->NamedType;
      resul = True;}
    else errWN(NotTypeId, curTok.IdName);
    GetNxtTok();}
  else err0(IdExptd);
  return resul;
}

void initPrag(void)
{
  initWarnMngt();
  /* Free memory */
  while (headPrivBlks != NULL) {
    TmngtPriv *w = headPrivBlks;

    headPrivBlks = headPrivBlks->chain;
    free(w);}
  headListPrivTo = NULL;
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
  register const TmngtPriv *w = pTag->ListAlwdFiles;

  if (fileName == cUnitFName) fileName = strpdCUnitFName;
  do {
    if (match(w->fileName, fileName)) return True;
  } while ((w = w->prev) != NULL);
  return False;
}

bool lastPrivFileIsUniq(register const TmngtPriv *x)
{
  return (x!=NULL && strpbrk(x->fileName, "*%")==NULL);
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
  savErrRet = curErrRet;
  curErrRet = &localJmpBuf /*~LocalAdr*/;
  if (setjmp(*curErrRet) == 0) {
    addTCB(oper, lType, rType, resType);
    if (commut && rType!=lType) addTCB(oper, rType, lType, resType);
    if (resType != NULL)
      switch (oper) {
        case AddTC:
          if (resType == lType) addTCB(SubTC, lType, rType, resType);
							    /* 'generic' type */
          break;
        case MulTC:
          if (commut) {
            addTCB(DivTC, resType, lType, rType);
            if (rType != lType) addTCB(DivTC, resType, rType, lType);
            if (IsTypeSort(resType, WhoEnum)) {
              addTCB(ModTC, resType, lType, resType);
              if (rType != lType) addTCB(ModTC, resType, rType, resType);}}
          break;
        case DivTC:
          if (IsTypeSort(resType, WhoEnum)) addTCB(ModTC, lType, rType, lType);
          break;
        /*~NoDefault*/}}
  curErrRet = savErrRet;
}

static void processPriv(TlitString str)
{
  register Tchar *pc;
  TmngtPriv *w;

  MyAlloc(w, initGetStrLit(str) + Offset(TmngtPriv, fileName));
  pc = (Tchar * /*~OddCast*/)&w->fileName[0];
  while ((*pc++ = nxtStrLitChar()) != '\0') {};  /* note indicated file name */
  resetGetStrLit();
  w->prev = headListPrivTo;
  headListPrivTo = w;
  w->chain = headPrivBlks;  /* for memory		    */
  headPrivBlks = w;	    /*		  freeing purposes. */
}

TpcTypeElt searchTC(TkOpTC oper)
/* Returns usually QuasiNULLval(TpcTypeElt) if no combination found */
{
  TpcTypeElt lType, rType;
  register const TsemanElt *lTypeId, *rTypeId;
  const TCBlk *ptrTCoper = (*pHeadTCBLists)[oper];

  if (ptrTCoper==NULL || (lType = lExp.type)==NULL || !lType->ParalTyp ||
       cExp.type==NULL || !cExp.type->ParalTyp) return QuasiNULLval(TpcTypeElt);
  /* Prepare to row up hierarchy, in order to find possible combination between
     ancestors. */
  do {
    lTypeId = lType->TypeId;
    rType = cExp.type;
    do {
      register const TCBlk *pcTCB = ptrTCoper;

      rTypeId = rType->TypeId;
      do {
        if (lTypeId==pcTCB->lTypeId && rTypeId==pcTCB->rTypeId) return
							       pcTCB->resulType;
      } while ((pcTCB = pcTCB->next) != NULL);
    } while (!rType->RootTyp && (rType = rTypeId->type)!=NULL && rType->
								      ParalTyp);
  } while (!lType->RootTyp && (lType = lTypeId->type)!=NULL && lType->ParalTyp);
  if (InsideInterval(oper, MulTC, DivTC) &&
  /* Search whether one operand appears in a type combination involving opera-
     tor '*' ('normal' type combination for non dimensionless type). If so,
     complain about missing typeCombination (cf Volt*Volt/Ohm). */
		     ((ptrTCoper = (*pHeadTCBLists)[MulTC])!=NULL || (ptrTCoper
					    = (*pHeadTCBLists)[DivTC])!=NULL)) {
    lType = lExp.type; rType = cExp.type;
    do {
      register const TCBlk *pcTCB = ptrTCoper;

      if (lType != NULL) lTypeId = lType->TypeId;
      if (rType != NULL) rTypeId = rType->TypeId;
      do {
        if (lTypeId==pcTCB->lTypeId || rTypeId==pcTCB->rTypeId) {
          TnameBuf buf;

          bufNameToS(cExp.type->TypeId->PdscId->idName, buf);
          errWNSS(NoTCAllows | Warn1, lExp.type->TypeId->PdscId->idName,
								 paramTxt, buf);
          return NULL;}
      } while ((pcTCB = pcTCB->next) != NULL);
      if (lType!=NULL && (lType->RootTyp || (lType = Parent(lType))!=NULL 
					     && !lType->ParalTyp)) lType = NULL;
      if (rType!=NULL && (rType->RootTyp || (rType = Parent(rType))!=NULL 
					     && !rType->ParalTyp)) rType = NULL;
    } while (lType!=NULL || rType!=NULL);}
  return QuasiNULLval(TpcTypeElt);
}

static void skipToComma(void)
{
  static const Ttok zCommaEndDP[] = {NoSwallowTok, COMMA, ENDDPRAG, ENDDIR,
								       EndSTok};

  skipTok(zCommaEndDP);
  if (curTok.tok == ENDDPRAG) dpragNst++;
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
