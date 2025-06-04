/* DCDIR.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#pragma noinline (errMac, errMacCall, errMacDef, errName)
#pragma noinline (evalIfExp, freeMac, peepNxtMacTok)
#pragma noinline (popMacStk, pushMacStk, skipParams, skipRestOfLinePhase4)
#pragma noinline (storeEndTok, transmitDP)
#endif

#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#ifdef EXIT_FAILURE
#  undef EXIT_FAILURE
#endif
#include <string.h>
#include "dcdir.h"
#include "dcdir.ph"
#include "dcblk.h"
#include "dcblk.ph"
#include "dcdecl.h"
#include "dcexp.h"
#include "dcext.h"
#include "dcmain.h"
#include "dcmain.ph"
#include "dcprag.h"
#include "dcprag.ph"
#include "dcrec.h"
#include "dcrec.ph"
#include "dcrecdir.ph"

#define BitOffsHCode	AdaptHCode(0x2C6F71B1)
#define CDfndHCode	AdaptHCode(0x3900A2)
#define DefineHCode	AdaptHCode(0x25729)
#define DefinedHCode	AdaptHCode(0xBB3F4)
#define	DefinedName	(Tname)"\7\0defined"
#define DispBegSto	(DispChain + 1)
#define DispChain	(TsTok)1
#define ElifHCode	AdaptHCode(0x1924)
#define ElseHCode	AdaptHCode(0x1955)
#define EndifHCode	AdaptHCode(0x7E69)
#define ErrorHCode	AdaptHCode(0x81E5)
#define ExecSundr							       \
  do {									       \
    if (curTok.Val == LineChng) lastSCOLline = 0;			       \
    else if (curTok.Val & ParBndary) macParCtr += Even;			       \
  } while (False)
#define Even		2
#define ExtentHCode	AdaptHCode(0x5B4956)
#define FillSkipTo(x)	memcpy(x, &pMacSto, sizeof(TsTok *))
#define GetTokAdr(x)							       \
  x = (x == NULL)							       \
       ? &(headChainMacBlk->tokSto[0])					       \
       : ((Ttok)*x == ENDBLK)						       \
         ? GoNxtBlk(x)							       \
         : x
#define GoNxtBlk(x)							       \
  &(((TmacBlk * /*~OddCast*/)(x - *(x + 1)))->next->tokSto[0])
#define IfHCode		AdaptHCode(0x105)
#define IfdefHCode	AdaptHCode(0x8431)
#define IfndefHCode	AdaptHCode(0x299E2)
#define InclNxtHCode	AdaptHCode(0xA2676C02)
#define IncludeHCode	AdaptHCode(0xD4DCD)
#define IncrMacParCtr	do {if (++macParCtr == 0) macParCtr = Even; /* to      \
					      keep parity. */} while (False)
#define IncrPMacT	2  /* sizeof(storage for standard macro token) */
#define IndexHCode	AdaptHCode(0x12452F)
#define IsConcFrame(x)	(x->tArg == NULL)
#define IsMacro(x)	(x!=NULL && x->nstLvl+1<0)
#define IsMacroFrame(x) (x->pPrevMac!=NULL && !IsConcFrame(x))
#define LineChng	(1 << PosLineChng)
#define LineHCode	AdaptHCode(0x1C5C)
#define LgtMacHdr	DispBegSto
#define MacActWNoPar	QuasiNULLval(const TsTok **)
#define ManageConcFl							       \
  if (concatFl != CNoBit)						       \
    if (! (concatFl & CParam)) concatFl |= CParam;			       \
    else concatFl |= CShield;  /* out of reach of current '##' operator */
#define MaxLineNb	32767
#define MemberHCode	AdaptHCode(0x5B78B6)
#define MPosFlags	CHAR_BIT
#define MPosLineNb	(MPosFlags + CHAR_BIT - 1) /* -1 because of LSIGN_BIT */
#define MRPosAdjFl	0
#define MRPosIgnFl	(MRPosAdjFl + 1)
#define MRPosPsdFl	(MRPosIgnFl + 1)
#define MRPosUsdFl	(MRPosPsdFl + 1)
#define MRPosSysFl	(MRPosUsdFl + 1)
#define ParBndary	(1 << PosParBnd)
#define PosLineChng	(PosParBnd + 1)
#define PosParBnd	(PosSysMac + 1)
#define PosSysMac	1  /* bit0 = stored value of sysMacro */
#define PragmaHCode	AdaptHCode(0x306B6)
#define PseudoName	ConvTname("\0\0")  /* name for pseudo-macro holding
				token string corresponding to each parameter. */
#define SamTypeHCode	AdaptHCode(0x8F50D2C)
#define SizOfCharBlkLgtFld 1
#define SizeMacroStorageChunk ((SizeMacroStorageBlk - Offset(struct _bid,      \
							bid))/sizeof(TsTok))
#define SkipRestOfLine	longjmp(*curErrRet, 1)
#define SkipTo(x)	memcpy(&x, x, sizeof(TsTok *))
#define SysMac		(1 << PosSysMac)
#define UndefHCode	AdaptHCode(0xA565)
#define WidthSundr	(PosLineChng + 1)

typedef struct _tCondStkElt TcondStkElt;
struct _tCondStkElt {
  TcondStkElt *prev;
  TindentChk savOldSpCnt;
  bool curIfArmActiv, trueIfPartSeen, elseSeen;
  bool lvlActiv:1, noExpand:1;
};

typedef uint TconcFl /*~RootType*/;
/* (Arbitrary) bit positions for type TconcFl */
#define CNoBit  (TconcFl)0
#define CShield (TconcFl)1
#define CArgColl (CShield << 1)
#define COpndSeen (CArgColl << 1)
#define COpnd1  (COpndSeen << 1)
#define CPsdOpnd1 (COpnd1 << 1)
#define COpnd2  (CPsdOpnd1 << 1)
#define CParam	(COpnd2 << 1)
#define CQuote	(CParam << 1)
#define MaxCMsk	CQuote

typedef struct _tMacBlk TmacBlk;
struct _bid {  /* just to allow computation of SizeMacroStorageChunk */
  TmacBlk *bid1, *bid2;
  TsTok bid3, bid;
};
struct _tMacBlk {
  TmacBlk *next, *prec;  /* 'next' must be first field (cf 'storeMacChunk()') */
  TsTok headChain, tokSto[(TsTok)SizeMacroStorageChunk];
};
/*~zif (Offset(struct _bid, bid) != Offset(TmacBlk, tokSto))
					"struct _bid incoherent with TmacBlk" */
/*~zif (SizeMacroStorageBlk < sizeof(TmacBlk))
				 "incorrect organization for struct _tMacBlk" */

typedef enum {EndMac, EndPar} TsortEndTok;

#define ConcFl		infoM.s11._concFl
#define InsideMUMP	infoM.s11._insideMUMP
#define InsidePar	MacExpnd
#define IsFME		infoM.s11._isFME
#define MacExpNb	infoM.s11._macExpNb
#define MacExpnd	infoM.s11._macExpnd
#define SysMacFl	infoM.s11._sysMacFl
#define NbBitsConcFl	8
/*~zif MaxCMsk >= (TconcFl)1<<NbBitsConcFl
				"Bad field size for storing 'TconcFl' values" */
typedef union {
  struct {
    TmacExpNb _macExpNb:SmallestWdthInt;  /* field needed because of embedded
	 			macro calls (will run into problem in the
				(unreasonable) case where there are more than
				2**16-1 macro expansions in a given compilation
				unit). */
    TconcFl _concFl:NbBitsConcFl;
#undef NbBitsConcFl
    bool _isFME:1;
    bool _sysMacFl:1;
    bool _macExpnd:1;
    bool _insideMUMP:1;
  } s11;
  TallocUnit u11;  /* for good alignment of following fields */
} TinfoM;
typedef struct _tMacStkElt TmacStkElt;
struct _tMacStkElt {
  const TsTok *pMacTok;/* token pointer in previous level */
  const TsTok **tArg;  /* because of case "Mac(Mac(3))"; NULL for '##' frame */
  TdescrId *pPrevMac;  /* previous macro (for embedded macro calls); NULL
							 for parameter frame. */
  TinfoM infoM;
  TmacStkElt *prev;
};
/*~zif Offset(TmacStkElt,infoM) > Offset(TmacStkElt,prev)
	"'prev' must be last field" */  /* because infoM may be used after
					  freeing (see end of manageConc() ). */

/* Context of GetNxtTok() */
typedef struct _tSavTokCtx TsavTokCtx;
struct _tSavTokCtx {
  TsavTokCtx *prev;
  TdescrId *newPDCI;
  void (*newPGNT)(void);
  TmacExpNb newCMEN;
  TvalTok newCurTok;
  TmacLvl newMacLvl;
  TmacParCtr newMPC;
  TindentChk newSpCnt, newOldSpCnt;
  bool newAdjMacro, newFrstChOfNb, newSysMacro, bigSave;
  ulong valSundries;
};

/* Functions profiles */
static void carryOutConc(void), checkDir(Tname),
  errMac(Terr, const TdescrId *), errMacCall(Terr, Tstring),
  errMacDef(Terr, Tstring)/*~NeverReturns*/, errName(Terr),
  freeMac(TdescrId *x), freeMacSto(const TsTok * /*~MayModify*/),
  getTokFromMac(void), manageConc(void), otherDir(void),
  popCondStk(void), popMacStk(void), procBitOffset(void), procCDefined(void),
  processDefined(void), procSameType(void), pushMacStk(void),
  restoPeepedTok(void), skipParams(uint),
  skipRestOfLinePhase4(void) /*~NeverReturns*/, skipToEndStNu(void),
  storeChInFName(Tchar), storeConcChar(char),
  storeEndTok(TdpNst, TsortEndTok), storeMacChar(char),
  storeMacChunk(Ttok, const TsTok *, size_t), storeTok(void);
static TcondStkElt *allocCondStkElt(void), *freeCondStkElt(TcondStkElt *);
static TmacStkElt *allocMacStkElt(void), *freeMacStkElt(TmacStkElt *);
static TsTok *createMacroHeader(Tname);
static bool evalIfExp(void), frstConcOpnd(void), macActive(bool);
static bool isMacPar(void);
static const TsTok *manageJmpTok(const TsTok * /*~ResultPtr*/);
static TcharStream nxtChFromConc(void);
static Ttok peepNxtMacTok(bool);

/* Global variables */
static Tchar *bufInclFName = NULL;
static char concatBuf[LgtHdrId + MaxLgtId + 1 + 1];  /* +1 for ending character,
						   +1 for overflow detection. */
static TconcFl concatFl = CNoBit;  /* flags for '##' operator */
static Tstring curFileN;
static bool curIfArmActiv = True;
static bool curMacIsFME;
static bool endOfDirLineGobld = False;
static bool elseSeen;
static const TsTok *endCurCharSubBlk = NULL;
static bool gotOutOfBody;
static bool incldSysFile;
static bool inclNxt;
static bool lvlActiv = True;
static bool nonPortFNSeen;
static TlineNb nLine;
static TmacExpNb nbNxtMac;
static char *pConcatBuf;
static const char *pConcTxt;
static TdescrId psdMacDcsId = {NULL, NULL, NULL, 0, {MacActWNoPar /*~zif
				   !__member(_tabArg) "Bad initialization" */}},
		*pCurMac = &psdMacDcsId;
static TsTok *pLgtCharSubBlk;  /* pointer on length field of currently filled
				      character (sub-)block in macro storage. */
static uint peepedTokVal;
static TdescrId *pMacBeingDfnd;
static const TsTok *pMacTok = NULL;  /* 'source' pointer when exploring macro */
static const TdescrId *pNxtMac = NULL;  /* non-NULL => collecting arguments for
					indicated macro (may collect them from
					expansion of an upper-level macro). */
static size_t posInBufIFN, sizBufIFN = 0;
static const TdescrId *prevMsngRParMac;
static TsTok *pseudoMacQuo;
static TcondStkElt *pTopCondStk;
static TmacStkElt *pTopMacStk = NULL;  /* pointer on top of macro stack */
static bool removingParMacFrame = False;
static bool savME;
static const TsTok *savPMTQuo = NULL;  /* not inside '#' operator */
static bool storingMacPar = False;
static bool trueIfPartSeen;
/* Variables to manage macro storage space */
static TsTok *pMacSto = NULL, *endCurMacSto = NULL, *pHeadChain;
static TmacBlk *curMacBlk,
	       *freeMacBlk = NULL,  /* available free block (for efficiency) */
	       *headChainMacBlk;    /* head of macro storage chunks chain */

/* External variables */
bool adjMacro = False;
TmacExpNb begMacExpNb;
uint ctrCSE = 0;  /* number of conditional directive stack elements */
bool gblArgColl = False;  /* somewhere in the macro stack, there is currently
			     at least one macro in the process of argument
			     collecting. */
bool insideDefineBody = False;
bool insideMultUsedMacPar = False;
TlineNb lastSCOLline;
TmacLvl macLvl = 0;
TmacParCtr macParCtr = Even;  /* non-zero because of 'lastPrimMacParCtr'
								   (dcexp.c). */
bool noExpand = False;  /* flag to prevent macro expansion, while keeping
			   first found 'pDescTokId', and permitting independant
			   life for 'macroExpand' (for example, in case of
			   missing ending parenthesis in macro call inside
			   macro body). */
bool sysMacro = False;  /* flag to prevent warnings inside system macros */

/* Pseudo-local variables */
static bool cmpsgHdr;

void manageDir(void)
{
  static TlineNb begLineNb;
  static Tstring begFileName;
  volatile bool oldCIAA = curIfArmActiv,
                          savGArgC = gblArgColl,
                          savMacroExpand = macroExpand,  /* macroExpand not
                                always True here, because of possible directive
                                line while collecting macro arguments. */
                          savNoExpand = noExpand;  /* noExpand not always False
				here, because of possible directive line while
				peeping for first '(' of macro call. */
  jmp_buf localJmpBuf, *savErrRet = curErrRet;

  curErrRet = &localJmpBuf  /*~ LocalAdr */;
  if (setjmp(*curErrRet) != 0) {  /* skip to end of directive line */
    static const Ttok zEndDir[]= {NoSwallowTok, ENDDIR, EndSTok};

    skipTok(zEndDir);}
  else {
    posInBufIFN = 0;  /* used also as '#include' flag */
    macroExpand = noExpand = True;
    gblArgColl = False;
    condDirSkip = False;  /* to de-inhibit error messages, and inhibit token
						     skipping in GetNxtTok(). */
    if (InsideDPragma) {
      curTok.tok = WHITESPACE;  /* destroy possible ENDDIR left in curTok.tok */
      err0(NoDirInDPrag);
      skipRestOfLinePhase4();}  /* see 'slashF()' */
    if (NxtTok()!=IDENT && curTok.tok!=IF && curTok.tok!=ELSE) errPanic(
       (curTok.tok == ENDDIR)? DirNameExptd|Warn2|PossErr : DirNameExptd, NULL);
    noExpand = False;
    switch (curTok.Hcod) {
    case IfHCode: checkDir(ConvTname("\2\0if")); goto ifL;
    case IfdefHCode: checkDir(ConvTname("\5\0ifdef")); goto ifL;
    case IfndefHCode: checkDir(ConvTname("\6\0ifndef"));
ifL:
      {
        TcondStkElt *w;

        w = allocCondStkElt();
        w->curIfArmActiv = curIfArmActiv;
        w->trueIfPartSeen = trueIfPartSeen;
        w->elseSeen = elseSeen;
        w->lvlActiv = lvlActiv;
        w->savOldSpCnt = oldSpaceCount;
        w->noExpand = savNoExpand;
        w->prev = pTopCondStk;
        pTopCondStk = w;}
      {
        bool w1;

        if (! curIfArmActiv) lvlActiv = False;
        trueIfPartSeen = elseSeen = False;
        if (! lvlActiv) skipRestOfLinePhase4();
        if (curTok.Hcod == IfHCode) w1 = evalIfExp();
        else {
          const TresulExp savCExp = cExp;

          w1 = (curTok.Hcod == IfndefHCode);
          noExpand = True;
          w1 = (NxtTok() == IDENT)
		? (processDefined(), (bool /*~OddCast*/)cExp.Uval ^ w1)
		: (errName(MacNameExptd), False);
          noExpand = False;
          cExp = savCExp;
          GetNxtTok();}
        curIfArmActiv = w1;}
      break;
    case ElifHCode:
      checkDir(ConvTname("\4\0elif"));
      goto elseL;
    case ElseHCode:
      checkDir(ConvTname("\4\0else"));
      if (elseSeen) err0(ElseAlrdSeen);
      elseSeen = True;
elseL:
      if (pTopCondStk == NULL) {err0(NoPrecIf); skipRestOfLinePhase4();}
      else if (oldCIAA) pTopCondStk->savOldSpCnt = oldSpaceCount;
      trueIfPartSeen |= curIfArmActiv;
      if (trueIfPartSeen || !lvlActiv) {
        curIfArmActiv = False;
        skipRestOfLinePhase4();}
      curIfArmActiv = (curTok.Hcod == ElifHCode)? evalIfExp() : (GetNxtTok(),
									  True);
      oldSpaceCount = pTopCondStk->savOldSpCnt;
      savNoExpand = pTopCondStk->noExpand;
      break;
    case EndifHCode:
      checkDir(ConvTname("\5\0endif"));
      if (pTopCondStk == NULL) err0(NoPrecIf);
      else if (oldCIAA) pTopCondStk->savOldSpCnt = oldSpaceCount;
      GetNxtTok();
      if (pTopCondStk != NULL) {
        savNoExpand = pTopCondStk->noExpand;
        popCondStk();}
      break;
    default:
      if (curIfArmActiv) otherDir();
      else skipRestOfLinePhase4();}
    if (curTok.tok != ENDDIR) errPanic(EndOfLineIgnd, NULL);}
  if (posInBufIFN != 0) processInclude(bufInclFName, incldSysFile, cmpsgHdr,
		inclNxt); /* '#include' (taken into account here, so that all
				     possible errors shown on previous file). */
  condDirSkip = (! curIfArmActiv);  /* condDirSkip must be set only after all
						   errors have been detected. */
  if (condDirSkip) {
    if (oldCIAA) {begLineNb = lineNb + cumDeltaLineNb; begFileName =
								   curFileName;}
    savNoExpand = True;}  /* to prevent macro-expansion while skipping */
  else if (!oldCIAA && !adjustFile && curFileName==begFileName
		 ) cumNbSkippedLines += lineNb - 1 + cumDeltaLineNb - begLineNb;
  ignoreErr = False;
  gblArgColl = savGArgC;
  macroExpand = savMacroExpand;
  noExpand = savNoExpand;
  curErrRet = savErrRet;
}

static void otherDir(void)
{
  switch (curTok.Hcod) {
  case DefineHCode : {
      uint paramCtr = 0;
      TlineNb oldLineNb;
      bool ignore, macIsSymb;

      checkDir(ConvTname("\6\0define"));
      noExpand = True;
      if (storingMacPar) {errMacCall(DccCantProcDef | Warn3, NULL)
							      ; SkipRestOfLine;}
      macIsSymb = False;
      GetNxtTok();
      {
        bool masking = FoundDP(MASKING);

        ignore = (cUnitFName==NULL && FoundDP(IGNORE));  /* d.p. allowed only
							     in starter file. */
        if (curTok.tok != IDENT) {
/*          if (!IsKW ||    memcmp(curTok.IdName, Const, FullLgt(Const))!=0
                       && memcmp(curTok.IdName, Volatile, FullLgt(Volatile))!=0)
			  ignore possible redefinition of 'const'/'volatile'. */
          errName(MacNameExptd);
          SkipRestOfLine;}
        if (pDescTokId != NULL) {
          if (pDescTokId->nstLvl+1 < 0) {errMac(MacAlrdDef|Warn3|PossErr,
						   pDescTokId); SkipRestOfLine;}
          else if (!masking && (curTok.PtrSem==NULL || curTok.PtrSem->type!=
		      NULL)) errId(NameAlrdInUse|Warn3|PossErr, curTok.PtrSem,
						    curTok.IdName, NULL, NULL);}
        else if (isSameName(curTok.Hcod, curTok.IdName, DefinedHCode,
				DefinedName)) errPanic(DefinedCantBeDef, NULL);}
      enterBlock();		/* just to store parameter names */
      pMacBeingDfnd = enterSymTabHC1((Tname)createMacroHeader(curTok.IdName),
								   curTok.Hcod);
      pMacBeingDfnd->nstLvl = IncplMacDef;  /* to remove immediately (in case of
	       erroneous macro definition) macro name from blocks name space. */
      if (nxtChFromTxt() == (TcharStream)'(') {  /* there is an (empty ?)
							      parameter list. */
        paramCtr++;	/* value 0 is for 'symbol' macro (no parenthesis
								 after name). */
        if (NxtTok() != RPAR) {
          do {
            TsemanElt *ptrParId;
            DefSem(defndMacPar, ParamMac, True);
            DefSem1(notUsdMacPar, ParamMac, False, True, False);

            if (curTok.tok != IDENT) {errName(ParNameExptd); errMacDef(NoErrMsg,
									 NULL);}
            if (paramCtr > UCHAR_MAX) errMacDef(TooManyMacPar, grstIntToS(
						      (TgreatestInt)UCHAR_MAX));
            if ((ptrParId = enterSymTabHC(storeName(curTok.IdName, ObjectSpace),
		     curTok.Hcod)) == NULL) errMacDef(ParAlrdExist, nameToS(
								curTok.IdName));
            ptrParId->NbPar = paramCtr++;  /* parameter number */
            ptrParId->defLineNb = lineNb;
            ptrParId->defFileName = curFileName;
            GetNxtTok();
#ifdef _AIX
            if (FoundDP(NOTUSED)) ptrParId->InfoS = notUsdMacPar;
            else ptrParId->InfoS = defndMacPar;
#else
            ptrParId->InfoS = (FoundDP(NOTUSED))? notUsdMacPar : defndMacPar;
#endif
            if (FoundDP(SIDEFFOK)) ptrParId->UsedMorThOnce = True;
          } while (Found(COMMA));}
        if (curTok.tok != RPAR) errMacDef(RParExptd, NULL);}
      else {
        backUp();
        if (curFileName!=starterFile && islower((char)*(curTok.IdName +
					  LgtHdrId))) macIsSymb = chk1stCharVF;}
      pMacBeingDfnd->nstLvl =
        (long)(((((ulong)lineNb<<(MPosLineNb - MPosFlags - MRPosSysFl) |
		  (ulong)sysHdrFile) << (MRPosSysFl - MRPosPsdFl) |
		  (ulong)ignore)     << (MRPosPsdFl - MRPosAdjFl) |
		  (ulong)adjustFile) << (MRPosAdjFl + MPosFlags) |
		  paramCtr |
		  LSIGN_BIT);  /* save definition line number, various flags,
						    and number of parameters. */
      pMacBeingDfnd->DfileName = curFileName;  /* birth place */
      pMacBeingDfnd->TabArg = NULL;  /* marks macro not active */
      /* Store macro body, in token form (except for identifiers, strings
         and numbers, because they can't be exactly decompiled). */
      insideDefineBody = True;
      oldLineNb = lineNb;
      if (NxtTok() != ENDDIR) {
        TsTok *pLastTok = NULL, *pLastNb = QuasiNULLval(TsTok *);
        bool lastTokIsConc = False, isNbAlone = (paramCtr == 0);

        do {
          if (lineNb != oldLineNb) {storeMacChunk(SUNDR, NULL, LineChng)
							  ; oldLineNb = lineNb;}
          if (curTok.tok == CONCATOP) {
            insideDefineBody = (bool)-1;  /* inhibits 'MsngSpace' warning */
            /* ## operator must be in prefix form */
            if (lastTokIsConc || pLastTok==NULL) {err0(MsngConcOpnd); continue;}
            lastTokIsConc = True;
            GetTokAdr(pLastTok);
            if (*pLastTok != (TsTok)IDSTNU) {  /* fixed length token => easy
					   to permute it with CONCATOP token. */
              curTok.tok = (Ttok)*pLastTok;
              curTok.Val = (uint)*(pLastTok + 1);
              storeTok();
              *(pLastTok + 1) = (TsTok)WHITESPACE;}  /* "normal" CONCATOP */
            *pLastTok = (TsTok)CONCATOP;  /* in case last token was IDSTNU,
				      extension is different from WHITESPACE. */
            curTok.tok = ENDARG1CONC;}  /* end marker for 1st arg. of ## */
          else {
            lastTokIsConc = False;
            pLastTok = pMacSto;
            if (isNbAlone && (curTok.tok!=ADDOP || pLastNb!=QuasiNULLval(
								      TsTok *)))
              if (curTok.tok==CSTNU && pLastNb==QuasiNULLval(TsTok *)
							   ) pLastNb = pLastTok;
              else isNbAlone = False;
            while (curTok.tok == QUOTEOP) {
              if (paramCtr != 0) {
                if (NxtTok()!=IDENT || !isMacPar()) {
                  err0(NotMacParForQuo);
                  if (curTok.tok == ENDDIR) goto endBodyL;
                  continue;}
                else {
                  curTok.Val = curTok.PtrSem->NbPar;
                  curTok.PtrSem->Used = True;
                  curTok.tok = QUOTEOP;}}
              else curTok.tok = QUOTEOP1;
              break;}
            if (macIsSymb && (curTok.tok==CSTNU || curTok.tok==CSTST ||
							     curTok.tok==DOT)) {
              errWN(NameSymbLoCase|Warn1|Rdbl, pMacBeingDfnd->idName);
              macIsSymb = False;}}
          storeTok();
          if (curTok.tok != ENDARG1CONC) insideDefineBody = True;
        } while (NxtTok() != ENDDIR);
endBodyL:
        if (isNbAlone && pLastNb!=QuasiNULLval(TsTok *)) {GetTokAdr(pLastNb)
					      ; *(pLastNb + 1) = (TsTok)CSTNU1;}
        if (lastTokIsConc) {err0(ConcOprIlgLast); storeMacChunk(WHITESPACE,
								     NULL, 0);}}
      storeEndTok(0, EndMac);
      exitBlock();
      insideDefineBody = False;}
    break;
  case ErrorHCode:
    checkDir(ConvTname("\5\0error"));
    err0(ErrorDir);
    skipRestOfLinePhase4();
  case InclNxtHCode:
    checkDir(ConvTname("\xC\0include_next"));
    inclNxt = True;
    goto mngInclL;
  case IncludeHCode:
    checkDir(ConvTname("\7\0include"));
    inclNxt = False;
mngInclL:
    nonPortFNSeen = False;
    noExpand = incldSysFile = True;
    if (NxtTok()==ORDEROP && (TkCmp)curTok.Val==LT) {  /* form '<' */
      Tchar c;

      while ((c = (Tchar)(*pNxtCh)()) != '>') {
        if (c=='\n' || c==EndCh && fileClosed) {errWS(Msng, ">"); backUp()
								       ; break;}
        if (! IsVisibleChar(c)) errWS(NotVisiChar|Warn3|PossErr, charToHexS(c));
        storeChInFName(c);}}
    else {  /* try macro expansion */
      noExpand = False;
      if (curTok.tok == IDENT) expandIfMac();
      if (curTok.tok == CSTST) {incldSysFile = False; analStrCst(
							      &storeChInFName);}
      else if (curTok.tok==ORDEROP && (TkCmp)curTok.Val==LT) {
        while (! (NxtTok()==ORDEROP && (TkCmp)curTok.Val==GT)) {
          if (curTok.tok == ENDDIR) {errWS(Msng, ">"); break;}
          if (InsideInterval(curTok.tok, CSTCH, CSTST)) goto ilgArgL;
          fullCurTokTxt(&storeChInFName);}}
#ifdef VMS
      else if (curTok.tok==IDENT && sysHdrFile) {
        fullCurTokTxt(&storeChInFName);
        storeChInFName('.'); storeChInFName('h');}
#endif
      else
ilgArgL:
      {
        err0(IlgInclArg);
        posInBufIFN = 0;
        skipRestOfLinePhase4();}}
    storeChInFName('\0');
    noExpand = False;
    if (curTok.tok != ENDDIR) GetNxtTok();
    cmpsgHdr = FoundDP(CMPSGHDR);
    break;
  case LineHCode:
    checkDir(ConvTname("\4\0line"));
    if (! InsideInterval(NxtTok(), CSTNU, CSTNU1)) err0(IntgrNbExptd);
    else {
      analNumCst(Convert);
      if (curTok.Val & FltSeen || curTok.NumVal==0 || curTok.NumVal>MaxLineNb
							   ) err0(IntgrNbExptd);
      else if (! curTok.ErrorT) {
        TlineNb newLineNb = (TlineNb)curTok.NumVal - 1;

        cumDeltaLineNb += lineNb - newLineNb;
        lineNb = newLineNb;}
      if (NxtTok() == CSTST) {
        Tchar *w;

        curFileName = w = allocPermSto(initGetStrLit(getLitString()));
        while ((*w++ = nxtStrLitChar()) != '\0') {}
        resetGetStrLit();
        exitBlock();}}  /* because of 'getLitString()' */
    break;
  case PragmaHCode:
    checkDir(ConvTname("\6\0pragma"));
#ifdef VMS
#  define NoStd	ConvTname("\xA\0nostandard")
#  define NoStd1 ConvTname("\xC\0__nostandard")
#  define YeStd	ConvTname("\x8\0standard")
#  define YeStd1 ConvTname("\xA\0__standard")
    noExpand = True;
    if (NxtTok() == IDENT) {
      if (memcmp(curTok.IdName, NoStd, FullLgt(NoStd))==0 ||
	     memcmp(curTok.IdName, NoStd1, FullLgt(NoStd1))==0) vmsNoStd = True;
      else if (memcmp(curTok.IdName, YeStd, FullLgt(YeStd))==0 ||
	    memcmp(curTok.IdName, YeStd1, FullLgt(YeStd1))==0) vmsNoStd = False;
      else goto exitL;
      GetNxtTok();
      break;}
exitL:;
#  undef NoStd
#  undef YeStd
#endif
    skipRestOfLinePhase4();  /* ignore line */
  case UndefHCode:
    checkDir(ConvTname("\5\0undef"));
    noExpand = True;
    if (NxtTok() != IDENT) {errName(MacNameExptd); skipRestOfLinePhase4();}
    if (! IsMacro(pDescTokId)) {if (!sysAdjHdrFile && lineNb!=0) errWN(
				       UnknMacro|Warn2|PossErr, curTok.IdName);}
    else {
      static const struct _s {Tname name; ThCode hc;} stickyMacros[] = {
		{(Tname)"\x8\0__LINE__", 0x59EB6C},
		{(Tname)"\x8\0__FILE__", 0x59A134},
		{(Tname)"\x8\0__DATE__", 0x59792A},
		{(Tname)"\x8\0__TIME__", 0x5A4C97},
		{(Tname)"\x8\0__STDC__", 0x5A56A6}};
      const struct _s *ptr;

      for (ptr = &stickyMacros[0]; ptr < AdLastEltP1(stickyMacros); ptr++)
        if (isSameName(curTok.Hcod, curTok.IdName, ptr->hc, ptr->name)) {errWN(
				   NonUndfnblMac, curTok.IdName); goto noFreeL;}
      freeMac(pDescTokId);
noFreeL: ;}
    GetNxtTok();
    break;
  default: errPanic(IlgDirName|Warn3|PossErr, NULL);}
}
/*~Undef cmpsgHdr */

void expandIfMac(void)
{
  TdescrId *calledMac = pDescTokId;
  uint argNbP1;
  const TsTok **newTabArg;  /* pointer on created argument table */
  bool isFMEMac, sysMacroFl, oldSysMac;
  TindentChk savSpcCnt;
  TmacExpNb nxtMacExpNb;

  if (! IsMacro(calledMac)) return;
  if (calledMac->nstLvl & (1 << (MPosFlags + MRPosPsdFl))) {  /* pseudo-macro
							       to be ignored. */
    if (NxtTok() == LPAR) {skipParams(-1u); GetNxtTok();}
    return;}
  if (curTok.tok == IDENT) {  /* not FORCEMACEXP/NOMACEXP */
    if (noExpand) {curTok.PtrSem = NULL; return;}
    if (! macroExpand) goto doNotExpandL;
    isFMEMac = False;}
  else isFMEMac = True;
  if (curTok.tok==NOMACEXP /* never expand NOMACEXP */ || calledMac->TabArg!=
						  NULL && macActive(isFMEMac)) {
    /* Avoid infinite recursion in macro expansion */
    if (gblArgColl) {curTok.tok = NOMACEXP; return;}  /* "Non-replaced 	macro
		      names are no longer available for further replacement". */
    goto doNotExpandL;}
  if (concatFl!=CNoBit && !(concatFl & (CShield | CArgColl))) {
    if (concatFl&CQuote || concatFl&COpnd2 && !(concatFl & COpndSeen) ||
            frstConcOpnd()) goto doNotExpandL;  /* do not expand argument(s)
                                                        of '#'/'##' operator. */
    isFMEMac = False;}
  argNbP1 = (uint)calledMac->nstLvl & ((1 << MPosFlags) - 1);
  sysMacroFl = (calledMac->nstLvl >> (MPosFlags + MRPosSysFl)) & 1;
  if (sysMacroFl && adjustFile && argNbP1!=0 && !dirLineFl && !sysMacro &&
								 !InsideDPragma)
    calledMac->nstLvl |= 1 << (MPosFlags + MRPosIgnFl);
  if (calledMac->nstLvl & (1 << (MPosFlags + MRPosIgnFl))) {
    igndSysMacArgNbP1 = argNbP1;
    goto doNotExpandL;}  /* ignore system macro tried to be expanded in adjust-
			    ment file (redefinition of function/object). */
  /* OK now for macro expansion */
  oldSysMac = sysMacro;
  savSpcCnt = spaceCount;
  {
    static TmacExpNb ctrMacExp = 0;  /* number of macro expansions */

    nxtMacExpNb = ++ctrMacExp;}  /* one more macro expansion ... (deemed
			      useless to check for (very unlikely) overflow). */
  if (argNbP1 == 0)  /* macro without argument */
    newTabArg = MacActWNoPar;
  else {  /* macro with (possibly empty) argument list */
    const TsTok **pNewTabArg, *const *endTabArg;
    TdpNst oldDPnst;
    TsTok *adPsdHdr;
    TindentChk savOSpCnt = oldSpaceCount;
    bool oldGblArgColl, oldSMP, oldSavME, oldGOOB = gotOutOfBody;
    TlineNb oldLineNb;
    const TdescrId *oldPNM = pNxtMac;
    TmacExpNb oldNNM = nbNxtMac;

    pNxtMac = calledMac;  /* must be set here (because of case where current
	   macro '#undef'ed on next line and next token only found after...). */
    nbNxtMac = nxtMacExpNb;
    gotOutOfBody = False;
    /* Check if really macro call ( '(' just after) */
    {
      bool exitFromParam = False, calledMacNotActiv;
      Ttok w;

      if (pMacTok!=NULL && peepNxtMacTok(False)==ENDDIR && (TsortEndTok)
				      peepedTokVal!=EndMac /* => == EndPar */) {
        exitFromParam = True;
        calledMacNotActiv = (calledMac != pCurMac);}
      {
        bool savNE = noExpand;

        noExpand = (! InsideMacro);  /* idem decCC, gcc */
        w = peepNxtTok();
        noExpand = savNE;}
      if (w == LPAR) {
        if (! exitFromParam) goto expandOKL;
        /* Case where name of macro alone passed in parameter */
        if (calledMacNotActiv) {
          if (! gblArgColl) isFMEMac = False;
          goto expandOKL;}}
      /* Macro not to be expanded */
      pNxtMac = oldPNM;
      nbNxtMac = oldNNM;
      goto doNotExpandL;
expandOKL: ;}
    nLine = lineNb;
    curFileN = curFileName;
    while (NxtTok() == SUNDR) ExecSundr;  /* skip LPAR */
    MyAlloc(pNewTabArg, argNbP1*sizeof(TsTok *));
    newTabArg = pNewTabArg;
    endTabArg = newTabArg + argNbP1;
    /* Simulate pseudo-macro body to store parameters (so as to free easily,
       at end of macro expansion, the space they use). */
    adPsdHdr = createMacroHeader(PseudoName);
    *pNewTabArg++ = (storingMacPar)? NULL : adPsdHdr;
    /* Argument collection */
    oldSavME = savME;
    savME = macroExpand;
    macroExpand = False;
    oldGblArgColl = gblArgColl;
    oldSMP = storingMacPar;
    storingMacPar = gblArgColl = True;
    oldLineNb = lineNb;
    oldDPnst = dpragNst;
    if (concatFl != CNoBit) concatFl |= CArgColl;  /* shield argument collec-
				     tion from possible '##' active operator. */
    if (NxtTok() != RPAR) {
      uint parenCtr = 0;  /* 'nested parenthesis' counter */

endArgL:
      if (pNewTabArg == endTabArg) {
        errMacCall(TooManyMacArg, NULL);
        if (! InsideMacro) skipParams(parenCtr);  /* idem decCC, gcc */
        goto endArgCollL;}
      *pNewTabArg++ = pMacSto;
      if (sysMacro != sysMacroFl) storeMacChunk(SUNDR, NULL, SysMac |
							      (size_t)sysMacro);
      for (;; GetNxtTok()) {
        if (lineNb != oldLineNb) {storeMacChunk(SUNDR, NULL, LineChng)
							  ; oldLineNb = lineNb;}
        switch (curTok.tok) {
          case COMMA:
            if (parenCtr == 0) {  /* "comma not protected by nested
								parenthesis." */
              storeEndTok(oldDPnst, EndPar);
              if (pNewTabArg != endTabArg) {sysMacro = oldSysMac; GetNxtTok();}
              goto endArgL /*~ BackBranch */;}
            break;		  /* else normal token */
          case ENDPROG:
            if (curTok.Val != 0) {
              storeEndTok(oldDPnst, EndPar);
              goto inFactFrstConcOpndL;}
            goto endEntityL;
          case ENDDIR:
            endOfDirLineGobld = dirLineFl = True;
endEntityL:
            errMacCall(UnFnshArgList, grstIntToS((TgreatestInt)(pNewTabArg -
							       newTabArg) - 1));
            goto endArgCollL;
          case IDENT:
            if (   parenCtr==0  /* identifier can be interpreted as macro name
				      only at level 0 of parenthesis nesting. */
                && IsMacro(pDescTokId)
		&& (concatFl==CNoBit || concatFl&(CShield | CArgColl)))
              curTok.tok = (pDescTokId->TabArg==NULL || !macActive(isFMEMac))?
				FORCEMACEXP : NOMACEXP;  /* if macro, remember
						    its current active state. */
            break;
          case LPAR: parenCtr++; break;
          case RPAR:
            if (parenCtr == 0) goto endArgCollL;
            parenCtr--;
            break;
          /*~ NoDefault */}
        storeTok();}}
endArgCollL:
    storeEndTok(oldDPnst, EndPar);
    if (pNewTabArg != endTabArg) {
      if (pNewTabArg-newTabArg !=1 || argNbP1!=1+1) errMac(MsngMacArg,
		      calledMac); /* case of empty argument for one parameter */
      do {
        *pNewTabArg++ = pMacSto;
        storeEndTok(oldDPnst, EndPar);
      } while (pNewTabArg != endTabArg);}
inFactFrstConcOpndL:
    gblArgColl = oldGblArgColl;
    storingMacPar = oldSMP;
    oldSpaceCount = savOSpCnt;  /* may have been modified during argument
								  collection. */
    macroExpand = savME;
    if (macroExpand) concatFl &= ~CArgColl;
    savME = oldSavME;
    gotOutOfBody = oldGOOB;
    pNxtMac = oldPNM;
    nbNxtMac = oldNNM;
    if (curTok.tok==ENDPROG && curTok.Val!=0 || frstConcOpnd()) {
      /* First operand of a '##' operator: do not expand => create pseudo macro
         body to exploit parameters as if they were not collected, and branch
	 on it. */
      Ttok savTok = curTok.tok;
      const TsTok *psdBody = pMacSto;
      size_t parCtr = 0;

      curTok.tok = LPAR;
      while (--pNewTabArg != newTabArg) {
        storeTok();
        storeMacChunk(MACPAR, NULL, ++parCtr);
        curTok.tok = COMMA;}
      if (savTok != ENDPROG) {curTok.tok = RPAR; storeTok();}
      storeMacChunk(SKIPTO, (const TsTok *)&pMacTok, sizeof(TsTok *) +
							      sizeof(TsTok **));
      *(pMacSto - (sizeof(TsTok *) + sizeof(TsTok **) + IncrPMacT - 1)) = 0;  /* special
								      SKIPTO. */
      memcpy(pMacSto - sizeof(TsTok **), &pCurMac->TabArg, sizeof(TsTok **));
      pCurMac->TabArg = newTabArg;
      pMacTok = psdBody;
      if (storingMacPar) FillSkipTo(adPsdHdr);  /* end of bulge */
      isFMEMac = True;  /* to have h-code recomputed */
      goto doNotExpandL;}
    if (storingMacPar) FillSkipTo(adPsdHdr);}  /* end of bulge */
  /* Enter macro */
  pushMacStk();
  ManageConcFl;
  macLvl++;
  {
    register TmacStkElt *pTMS = pTopMacStk;

    pTMS->MacExpNb = curMacExpNb;
    pTMS->SysMacFl = oldSysMac;
    pTMS->IsFME = curMacIsFME;
    pTMS->MacExpnd = macroExpand;}
  /* Entering a macro is a break in a parameter stream of tokens */
  if ((curMacIsFME = isFMEMac)) {  /* delayed expansion of macro, that should
		       have been performed before encountering possible current
		       '##' operator; so get out of reach of it (but remember
		       that inside it, to allow frstConcOpnd() to be called). */
    if (concatFl != CNoBit) concatFl |= CShield;}
  macroExpand = True;
  calledMac->nstLvl |= 1 << (MPosFlags + MRPosUsdFl);  /* marks macro used */
  adjMacro = (calledMac->nstLvl >> (MPosFlags + MRPosAdjFl)) & 1;
  sysMacro = sysMacroFl;
  pCurMac = calledMac;  /* macro becomes current */
  calledMac->TabArg = newTabArg;  /* note address of created table of pointers
				 on macro arguments; alse marks macro active. */
  pMacTok = (const TsTok *)calledMac->idName + FullLgt(calledMac->idName); /*
							     skip macro name. */
  if (! InsideMacro) {pCurGNT = &getTokFromMac; pNxtCh = &nxtChFromMac
						      ; prevMsngRParMac = NULL;}
  curMacExpNb = nxtMacExpNb;  /* BEWARE, used in InsideMacro... */
  if (gblArgColl && (concatFl==CNoBit || concatFl & (CShield | CArgColl)) &&
						        sysMacroFl!=oldSysMac) {
    curTok.tok = SUNDR;
    curTok.Val = (uint)sysMacroFl | SysMac;}
  else GetNxtTok();  /* get first token of macro */
  spaceCount = savSpcCnt;
  return;
doNotExpandL:
  {
    register TdescrId *pDescCurId;
    register Tname adBegName;
    ThCode hCode;

    adBegName = calledMac->idName;
    if (isFMEMac) {
      curTok.tok = IDENT;
      curTok.IdName = adBegName;
      curTok.Hcod = hCodFct(adBegName);}
    if (gblArgColl) {  /* token meant for macro argument */
      curTok.PtrSem = NULL;
      pDescTokId = calledMac;  /* keep its macro meaning */
      return;}
    hCode = curTok.Hcod;
    SearchSym(adBegName, MacroNotVisible)
    if ((pDescTokId = pDescCurId) == NULL) {curTok.PtrSem = NULL; return;}
    if (pDescCurId->nstLvl < 0) {curTok.tok = pDescCurId->NoLex; return;}
    curTok.PtrSem = pDescCurId->PidSeman;}
}

static void getTokFromMac(void)
{
  getTokLvl++;
  spaceCount = SetInGetTokFrmMac;
  for (;;) {  /* because some tokens are 'service' tokens, swallowed locally */
    register const TsTok *pMacTokL = pMacTok;
    Ttok token;

    curTok.tok = token = (Ttok)*pMacTokL++;
    curTok.Val = (uint)*pMacTokL++;
    /*~zif IncrPMacT != 2 "Pb"*/
    pMacTok = pMacTokL;
    if (token >= BegSpeMacTok) switch (token) {
      case CONCATOP:  /* '##' operator */
	/* Manage strings of concatenation from left to right, so that :
           - carryOutConc() not called recursively,
           - in case A##x##B , with x => 'U V', possible macro U not expanded. */
        concatFl = (concatFl != COpnd2)? (pushMacStk(), pTopMacStk->tArg = NULL
		/* marks new frame as ## frame */, COpnd1) : CPsdOpnd1 | COpnd2;
        if ((Ttok)curTok.Val != WHITESPACE) goto idStNuL;  /* if CONCATOP
						    condensed with IDENT etc. */
        continue;
      case DPRAG:
        if ((curTok = manageDPrag()).tok == WHITESPACE) continue;
        break;
      case ENDARG1CONC:
        if (concatFl == (COpnd1 | COpndSeen)) {  /* normal case */
          concatFl = COpnd2;
          continue;}  /* get 2nd operand */
        pMacTok -= IncrPMacT;
        curTok.tok = ENDPROG;
        curTok.Val = 1;
        break;
      case ENDBLK:
        pMacTokL -= IncrPMacT;
        pMacTok = GoNxtBlk(pMacTokL);
        continue;
      case ENDDIR: {  /* end of macro body or parameter */
          bool oldSysMac = sysMacro;

          {
            register TmacStkElt *pTMS = pTopMacStk;

            if ((TsortEndTok)curTok.Val == EndMac) {  /* end of macro */
              register TdescrId *pCurMacR = pCurMac;

              if (pNxtMac!=NULL && !macroExpand) {
                if (curMacExpNb<nbNxtMac && pNxtMac!=prevMsngRParMac) {
                  errMacCall(ArgCollGetsOutMacBody | Warn3, nameToS(pCurMac->
								       idName));
                  prevMsngRParMac = pNxtMac;}
                gotOutOfBody = True;}
              if (pCurMacR->TabArg != MacActWNoPar) {  /* if macro with parame-
					ters, free storage used by macro parame-
					ters and parameters table. */
                freeMacSto(pCurMacR->TabArg[0]);
                free(pCurMacR->TabArg);}
              pCurMacR->TabArg = NULL;  /* marks macro inactive */
              adjMacro = (pTMS->pPrevMac->nstLvl >> (MPosFlags + MRPosAdjFl))&1;
              pTMS->pPrevMac->TabArg = pTMS->tArg;
              pCurMac = pTMS->pPrevMac;
              curMacIsFME = pTMS->IsFME;
              if (! gotOutOfBody) macroExpand = pTMS->MacExpnd;
              else savME = pTMS->MacExpnd;
              curMacExpNb = pTMS->MacExpNb;
              if (curMacExpNb < begMacExpNb) lastSCOLline = 0;}
            else {  /* end of parameter */
              if (pTMS->InsidePar) macParCtr += Even;
              else IncrMacParCtr;  /* so that uninterrupted run of tokens in
							parameter detectable. */
              insideMultUsedMacPar = pTMS->InsideMUMP;}
            sysMacro = pTMS->SysMacFl;}
          macLvl--;
          popMacStk();
          if (concatFl != CNoBit) {
            if (removingParMacFrame) {getTokLvl--; return;}
            if (! (concatFl & CParam)) {  /* top frame is a '##' frame */
              if (! (concatFl & (COpndSeen | CArgColl))) {  /* no token gene-
					rated by parameter operand of a '##'
					operator => 'placemarker'. */
                curTok.tok = WHITESPACE;
                break;}
              if (concatFl == (COpnd2 | COpndSeen)) popMacStk();}}  /* exiting
		      from a '##' operator whose 2nd operand was a parameter. */
          /* Get next token */
          if (InsideMacro) {
            if (gblArgColl && (concatFl==CNoBit || concatFl & (CShield |
			CArgColl)) && ((TsortEndTok)curTok.Val!=EndMac /* i.e
					  ==EndPar */ || sysMacro!=oldSysMac)) {
              curTok.tok = SUNDR;
              curTok.Val = (SysMac | (uint)sysMacro) | (uint)(curTok.Val !=
						     (uint)EndMac) << PosParBnd;
              break;}
            continue;}}
        pCurGNT = &getTokFromTxt;
        pNxtCh = &nxtChFromTxt;
        getTokLvl--;
        if (endOfDirLineGobld) {endOfDirLineGobld = False; backUp() /* to get
		   ENDDIR back as next token; here for good error position. */;}
        GetNxtTok();
        return;
      case IDSTNU:
idStNuL:
        switch (curTok.tok = (Ttok)curTok.Val) {
        case CSTNU: case CSTNU1:
          curTok.Val = 0;
          frstChOfNb = (bool)nxtChFromMac();  /* True */
          break;
        case WHITESPACE + 1:  /* wide-character string (cf storeTok()) */
          curTok.tok = CSTST;
          goto cststL;
        case CSTST:
          curTok.Val = (uint)False;
cststL:
          frstChOfNb = True;
          break;
        case FORCEMACEXP: case NOMACEXP:
          memcpy(&pDescTokId, pMacTokL, sizeof(pDescTokId));
          pMacTok += sizeof(pDescTokId);
          goto expandL;
        case IDENT: {
            register TdescrId *pDescCurId;
            register Tname adBegName;
            ThCode hCode;

            curTok.IdName = adBegName = (Tname)pMacTokL;
            pMacTok += FullLgt(adBegName);
            curTok.Hcod = hCode = hCodFct(adBegName);
            SearchSym(adBegName, MacroVisible)
            igndSysMacArgNbP1 = 0;  /* reset flag */
            if ((pDescTokId = pDescCurId) == NULL) {curTok.PtrSem= NULL; break;}
            if (pDescCurId->nstLvl >= 0) {  /* identifier not macro */
              curTok.PtrSem = pDescCurId->PidSeman;
              goto tstUndefL;}
            if (pDescCurId->nstLvl+1 == 0) {curTok.tok = pDescCurId->NoLex
					; break;}}  /* keyword (case '+zkwm') */
expandL:
          expandIfMac();  /* macro */
          if (curTok.tok!=IDENT || curTok.PtrSem==NULL) break;
tstUndefL:
          if (curTok.PtrSem->Undf && getTokLvl==tokLvl0) errWN(UndfndId|Warn1|
							   Rdbl, curTok.IdName);
          break;
        /*~NoDefault*/}
        break;
      case MACPAR: case MACPAR1:
        pushMacStk();
        ManageConcFl;
        macLvl++;
        pTopMacStk->InsidePar = InsideMacPar;
        if (InsideMacPar) macParCtr += Even;  /* keep parity */
        else macParCtr++;  /* cannot overflow */
        pTopMacStk->InsideMUMP = insideMultUsedMacPar;
        pTopMacStk->SysMacFl = sysMacro;
        pTopMacStk->pPrevMac = NULL;  /* mark new frame as parameter frame */
        insideMultUsedMacPar = (curTok.tok == MACPAR1);
        pMacTok = pCurMac->TabArg[curTok.Val];  /* beginning of parameter
							    string of tokens. */
        if (gblArgColl && (concatFl==CNoBit || concatFl & (CShield |
								   CArgColl))) {
          curTok.tok = SUNDR;
          curTok.Val = ParBndary;
          break;}
        continue;
      case QUOTEOP: {
          /* Decode parameter tokens, and store generated string in string
             storage, pending better macro processing. */
          TsTok *pCrtdTok;

          concatFl |= CQuote;  /* to prevent macro-expansion (including
								FORCEMACEXP). */
          {
            TsTok w;  /* not initialized, since value later overwritten */

            pseudoMacQuo = createMacroHeader(PseudoName);  /* pseudo-macro to
		hold generated string token (for memory reclaiming purposes). */
            storeMacChunk(IDSTNU, /*~Init w*/&w, SizOfCharBlkLgtFld);  /* so
		    that length of 1st sub-block in same macro storage block. */
            pLgtCharSubBlk = pMacSto - SizOfCharBlkLgtFld;
            pCrtdTok = pLgtCharSubBlk - IncrPMacT;
            *(pCrtdTok + 1) = (TsTok)CSTST;  /* fill IDSTNU created token */
          }
          for (pMacTok = pCurMac->TabArg[curTok.Val]; (Ttok)*(manageJmpTok(
							 pMacTok)) != ENDDIR;) {
            if (NxtTok() == DPRAG) {
              dpragNst--;
              if (peepNxtMacTok(True) == ENDDPRAG) {storeMacChar(' ')
						      ; GetNxtTok(); continue;}}
            fullCurTokTxt(&storeMacChar);}
          *pLgtCharSubBlk = (TsTok)(pMacSto - pLgtCharSubBlk);  /* number
				    of characters (+1) in (ending) sub-block. */
          storeMacChunk((Ttok)0, NULL, 0); pMacSto--;  /* end marker = 0
						      (only one byte needed). */
          *pMacSto = *pMacTokL;  /* in case of 'peepNxtMacTok()' */
          if (storingMacPar) {FillSkipTo(pseudoMacQuo); pseudoMacQuo = NULL;}
          concatFl &= ~CQuote;
          savPMTQuo = pMacTokL;  /* also, flag to tell 'inside quoteOp' (if
								   non NULL). */
          pMacTok = pCrtdTok;
          continue;}  /* get created token */
      case SKIPTO:
        if (curTok.Val == 0) {  /* end of simulated macro body created after
					     late detection of concatenation. */
          const TsTok **w;

          memcpy(&w, pMacTok+sizeof(TsTok *), sizeof(TsTok **));
          SkipTo(pMacTok);
          freeMacSto(pCurMac->TabArg[0]);
          free(pCurMac->TabArg);
          pCurMac->TabArg = w;}  /* restore previous argument table */
        else SkipTo(pMacTok);
        continue;
      case SUNDR:
        if (curTok.Val & SysMac) {
          if (sysMacro==(curTok.Val & 1) && !(curTok.Val & (ParBndary |
					    LineChng))) continue;  /* useless */
          sysMacro = curTok.Val & 1;}
        if (gblArgColl && concatFl==CNoBit) break;  /* to be kept uninterpreted
			in case of argument collection (not inside '#'/'##'). */
        ExecSundr;
        continue;
      /*~NoDefault*/}
    if (concatFl!=CNoBit && frstConcOpnd()) {  /* first operand of '##'
								    operator. */
      manageConc();
      if (curTok.tok==WHITESPACE && !(concatFl&COpnd2 && !(concatFl & COpndSeen)
			)) continue;}  /* ignore possible empty token
						    created by concatenation. */
    break;}  /* normal exit of main "for(;;)" loop */
  if (--getTokLvl == 0) storeTokTxt(curTokTxt());
}

/******************************************************************************/
/*                                                                            */
/*                                  UTILITIES                                 */
/*                                                                            */
/******************************************************************************/

static AllocXElt(allocCondStkElt, TcondStkElt, ctrCSE, ;)
static AllocXElt(allocMacStkElt, TmacStkElt, ctrMSE, ;)

static bool aloneInMac(uint x)
{
  return (   pMacTok==NULL  /* because of peepNxtTok */
          || pMacTok==manageJmpTok((const TsTok *)pCurMac->idName + FullLgt(
		   pCurMac->idName))+x /* current token at beginning of macro */
           && peepNxtMacTok(False)==ENDDIR
           && (TsortEndTok)peepedTokVal==EndMac);  /* and nothing after. */
}

bool aloneInNoParMac(void)
{
  return (! (pCurMac->nstLvl & (1<<(MPosFlags + MRPosSysFl) |
	       ((1 <<MPosFlags) - 1)))  /* non system macro with no parameter */
          && aloneInMac(IncrPMacT));
}

static void carryOutConc(void)
{
#define BegConcBuf &concatBuf[LgtHdrId]

  /* Concatenate tokens text */
  pConcatBuf = BegConcBuf;
  for (;;) {
    fullCurTokTxt(&storeConcChar);
    concatFl |= COpndSeen;
    if (concatFl & COpnd2) break;  /* 2nd token seen */
    GetNxtTok();}
  if (pConcatBuf == &concatBuf[NbElt(concatBuf) - 1])
    concatErr = CrtdTokTooLong;
  else {
    concatErr = NoConcErr;  /* to intercept possible error */
    if (pConcatBuf == BegConcBuf) curTok.tok = WHITESPACE;  /* empty token */
    else {
      char *savSrcPtr = srcPtr;

      *pConcatBuf = ' ';  /* end of created token text */
      srcPtr = BegConcBuf;
      mngConc = True;
      {
        TindentChk savSpaceCnt = spaceCount;
        bool savNE = noExpand;

        noExpand = True;
        getTokFromTxt();  /* get created token */
        noExpand = savNE;
        spaceCount = savSpaceCnt;}
      pNxtCh = &nxtChFromTxt;
      analTok();
      pNxtCh = &nxtChFromMac;
      if (concatErr==NoConcErr /* no error seen yet */ && srcPtr!=pConcatBuf
							 ) concatErr = EmptyTxt;
      mngConc = False;
      srcPtr = savSrcPtr;}}
  if (concatErr != NoConcErr) {  /* error */
    *pConcatBuf = '\0';  /* in order to get a string */
    errWSS(IlgCrtdTok|Warn2|PossErr, (Tstring)BegConcBuf, errTxt[concatErr]);
    curTok.tok = WHITESPACE;}
  else if (InsideInterval(curTok.tok, CSTST, CSTNU)) {
    frstChOfNb = (bool)*BegConcBuf;
    pConcTxt = (curTok.tok == CSTNU)? BegConcBuf + 1 : BegConcBuf;
    pNxtCh = &nxtChFromConc;}  /* no lexical error can occur now in number/
	 							      string. */
  concatErr = NoErrMsg;
}

void checkCondStkAndDeleteMacros(void)
{
  if (pTopCondStk != NULL) {
    errWS(Msng, "#endif");
    do popCondStk(); while (pTopCondStk != NULL);}
  /* Free (possible) remaining macro zones */
  {
    TdescrId **w = &symTabHeads[0], *w1, *w2;

    do {
      for (w1 = *w++; w1 != NULL; w1 = w2) {
        w2 = w1->next;
        if ((w1->nstLvl)+1 < 0) freeMac(w1);}
    } while (w != &symTabHeads[SizeSymTab]);}
}

static void checkDir(Tname dirName)
{
  if (memcmp(curTok.IdName, dirName, FullLgt(dirName)) != 0) errPanic(
						IlgDirName|Warn3|PossErr, NULL);
}

bool checkSColAtEndMac(void)
{
  return (pMacTok==NULL || peepNxtMacTok(False)!=ENDDIR || (TsortEndTok)
     peepedTokVal!=EndMac) ? False : (errMac(ExtraSColAtEndMac, pCurMac), True);
}

bool checkSpeFct(void)
{
  enum _index{BegTab, Defined = BegTab, Member, Extent, CDefined,
					     Index, SamType, BitOffset, EndTab};
  static const struct _speFctDesc {ThCode fctHCode; Tname fctName;
	      void (*fctPtr)(void);} speFctTab[/*~ IndexType enum _index */] = {
	{DefinedHCode, DefinedName, &processDefined}		/*~ zif
		  __index!=Defined "Array 'speFctTab': element out of order" */,
	{MemberHCode, (Tname)"\x8\0__member", &procMember1}	/*~ zif
		   __index!=Member "Array 'speFctTab': element out of order" */,
	{ExtentHCode, (Tname)"\x8\0__extent", &procExtent}	/*~ zif
		   __index!=Extent "Array 'speFctTab': element out of order" */,
	{CDfndHCode, (Tname)"\x8\0cdefined", &procCDefined}	/*~ zif
		 __index!=CDefined "Array 'speFctTab': element out of order" */,
	{IndexHCode, (Tname)"\7\0__index", &procIndex}		/*~ zif
		    __index!=Index "Array 'speFctTab': element out of order" */,
	{SamTypeHCode, (Tname)"\xA\0__sametype", &procSameType}	/*~ zif
		  __index!=SamType "Array 'speFctTab': element out of order" */,
	{BitOffsHCode, (Tname)"\xB\0__bitoffset", &procBitOffset}/*~ zif
		 __index!=BitOffset"Array 'speFctTab': element out of order" */
	/*~ zif __index != EndTab - 1
				  "Array 'speFctTab' not fully initialized" */};
  register const struct _speFctDesc *ptrTab;

  for (ptrTab = &speFctTab[BegTab]; ptrTab <= &speFctTab[EndTab - 1];ptrTab++) {
    if (isSameName(curTok.Hcod, curTok.IdName, ptrTab->fctHCode,
							     ptrTab->fctName)) {
      bool rParExptd, savNE = noExpand;

      if (ptrTab == &speFctTab[Defined]) noExpand = True;
      else if (ptrTab == &speFctTab[CDefined]) {
        if (! adjustFile) return False;
        noExpand = True;}
      else if (! zifExp) return False;
      if (rParExptd = (NxtTok() == LPAR)) GetNxtTok();
      if (ptrTab >= &speFctTab[Index]) {
        bool savEvalFl = evaluateFl;

        evaluateFl = False;
        (*ptrTab->fctPtr)();  /* call function */
        evaluateFl = savEvalFl;}
      else if (ptrTab==&speFctTab[Member] && curTok.tok!=IDENT) procMember2();
      else {
        if (curTok.tok != IDENT) {
          if (ptrTab == &speFctTab[Defined]) errName(MacNameExptd);
          else err0(IdExptd);}
        else {
          (*ptrTab->fctPtr)();
          if (! rParExptd) noExpand = savNE;
          GetNxtTok();}}
      noExpand = savNE;
      if (rParExptd && !Found(RPAR)) errMsngRPar();
      return True;}}
  return False;
}

static TsTok *createMacroHeader(Tname x)
/* Returns address of created macro header */
{
  TsTok */*~IndexType TsTok*/ resul;

  if (storingMacPar) {  /* do not create (pseudo-)macro header if currently sto-
			  ring parameters, because already inside macro storage,
			  and this storage cannot nest; so create a 'bulge'. */
    storeMacChunk(SKIPTO, (const TsTok *)&pMacTok /* or any other pointer */,
								sizeof(void *));
    resul = pMacSto - sizeof(void *);}  /* remember position of address field
			of SKIPTO (to be able to fill it when address known). */
  else {
    storeMacChunk((Ttok)0, (const TsTok *)x, FullLgt(x));  /* store macro
					header and name in a contiguous area. */
    /*~zif LgtMacHdr != IncrPMacT "We have a big problem here" */
    resul = pMacSto - FullLgt(x);  /* here because new block may have started */
    /* Chain new zone to zones already held in current macro block, in such a
       manner that higher address zones are found earlier in chain (cf
							      freeMacSto() ). */
    *(resul - DispChain) = *pHeadChain;
    *(resul - DispBegSto) = *pHeadChain = (TsTok)(resul - &(curMacBlk->tokSto
									 [0]));}
  return resul;
}

Tstring curMacDFName(void)
{
  return pCurMac->DfileName;
}

static void errMac(Terr n, const TdescrId *x)
{
  errWFName(n, (TlineNb)(((ulong)x->nstLvl^LSIGN_BIT) >> MPosLineNb),
					   x->DfileName, x->idName, NULL, NULL);
}

void errMacBefIncl(void)
{
  const TmacStkElt *wpStk = pTopMacStk;
  const TdescrId *pCMac;

  if (wpStk != NULL) {  /* because of possible call to peepNxtTok() */
    pCMac = pCurMac;
    while (wpStk->prev != NULL) {
      if (IsMacroFrame(wpStk)) pCMac = wpStk->pPrevMac;
      wpStk = wpStk->prev;}
    if (curFileName!=pCMac->DfileName && pCMac->TabArg==MacActWNoPar)  /* if
		  macro defined in current file, or there exists parameters,
		  fortuitous error less probable. */ errMac(MacBefIncl, pCMac);}
}

static void errMacCall(Terr n, Tstring y)
{
  errWFName(n, nLine, curFileN, (pNxtMac == NULL)? NULL : pNxtMac->idName, y,
									  NULL);
}

static void errMacDef(Terr n, Tstring txt) /*~NeverReturns*/
/* Free storage used by the (aborted) macro name, and skip remaining of line */
{
  TnameBuf buf;

  bufNameToS(pMacBeingDfnd->idName, buf);  /* not errWNSS(), because of 'txt'
								   parameter. */
  errWSS(n, buf, txt);
  freeMac(pMacBeingDfnd);
  exitBlock();  /* may cause error messages */
  SkipRestOfLine;  /* errPanic() not used, to get correct sequencing
							   of error messages. */
}

static void errName(Terr n)
{
  errWS(n, (IsKW)? errTxt[IfKW] : NULL);
}

static bool evalIfExp(void)
{
  const TresulExp savCExp = cExp, savLExp = lExp;
  bool resul;
  Trchbl savNSR = nxtStmtRchbl;

  ifDirExp = True;
  GetNxtTok();
  resul = correctExprN(NULL, mskBool & WhoEnumBool, True, "#if", chkNumCst) &&
								   cExp.Uval!=0;
  ifDirExp = False;
  cExp = savCExp; lExp = savLExp; nxtStmtRchbl = savNSR;
  return resul;
}

static FreeXElt(freeCondStkElt, TcondStkElt *, ctrCSE, ;, prev)

static void freeMac(register TdescrId *x)
{
  if (pNxtMac != NULL)  /* cannot only test currently expanding macro, because
		      of macros passed as parameter, that store a pointer on a
		      DescrIdElt: e.g. // #define M(x) ... // #define M1 ... //
			      ... // M(M1 // #undef M // #undef M1 // ) // ...*/
    *((TnameNC/*~OddCast*/)x->idName + DispNSId) = (TnameAtom)DltdMac;  /*
			      then just render macro invisible to SearchSym;
			      will be freed by checkCondStkAndDeleteMacros(). */
  else {
    if (!(x->nstLvl & (1 << (MPosFlags + MRPosUsdFl))) && x->DfileName==
		   cUnitFName && warnNUI) errMac(UnusedMac|NoDispLine|Warn1, x);
    /* Free storage used by macro body */
    freeMacSto((const TsTok *)x->idName);
    /* Remove macro element from symbol table chaining */
    x->prec->next = x->next;
    if (x->next != NULL) x->next->prec = x->prec;
    /* Free element */
    (void)freeDescrIdElt(x);}
}

static FreeXElt(freeMacStkElt, TmacStkElt *, ctrMSE, ;, prev)

static void freeMacSto(const TsTok *macName /*~MayModify*/)
{
  TmacBlk *firstBlk, *nxtBlk, *precBlk;
  register TmacBlk *curBlk;

  if (macName == NULL) return;
  firstBlk = (TmacBlk * /*~OddCast*/)((const ubyte *)macName - *(macName -
	DispBegSto) - Offset(TmacBlk, tokSto[0]));  /* compute address of block
					where macro begins ('initial' block). */
  *((TsTok * /*~OddCast*/)macName - DispBegSto) = 0;  /* marks zone free */
  if (macName != &(firstBlk->tokSto[firstBlk->headChain])) return;  /* if
			    freed zone not last of block, nothing to be done. */
  for (curBlk = firstBlk->next; curBlk != NULL; curBlk = nxtBlk) {  /* find end
							       of freed zone. */
    TsTok w;

    for (w = curBlk->headChain; w != 0; w = curBlk->tokSto[w -
		     DispChain]) {  /* follow chain of macro headers in block */
      if (curBlk->tokSto[w - DispBegSto] != 0) {  /* found still used macro
							    in current block. */
        /* Patch chaining over all freed blocks */
        firstBlk->next = curBlk;
        curBlk->prec = firstBlk;
        return;}}
    /* Whole block is freeable; free it */
    nxtBlk = curBlk->next;
    if (freeMacBlk == NULL) freeMacBlk = curBlk;  /* keep one spare (for
								 efficiency). */
    else free(curBlk);
    ctrMSB--;}
  /* Freed zone includes last block, so reset pointers for storeMacChunk(),
     removing possible free blocks sitting before 'initial' one. */
  do {
    TsTok w, w1;

    for (w1 = firstBlk->headChain; w1 != 0; w1 = firstBlk->tokSto[w1 -
		     DispChain]) {  /* follow chain of macro headers in block */
      if (firstBlk->tokSto[w1 - DispBegSto] != 0) break;  /* found still
							   used macro header. */
      w = w1;}  /* keep position of end of current zone */
    if (   w1!=0 /* found still used macro header */
        || (precBlk = firstBlk->prec)!=(TmacBlk *)&headChainMacBlk
           && (   precBlk->headChain==0 /* if so, preceding zone can't be free*/
               || precBlk->tokSto[precBlk->headChain - DispBegSto]!=0  /* or
						preceding zone not free. */ )) {
      pMacSto = &(firstBlk->tokSto[w - LgtMacHdr]);
      firstBlk->headChain = w1;
      pHeadChain = &(firstBlk->headChain);
      curMacBlk = firstBlk;
      endCurMacSto = &(firstBlk->tokSto[(TsTok)SizeMacroStorageChunk -
								    IncrPMacT]);
      firstBlk->next = NULL;
      return;}
    free(firstBlk);
    ctrMSB--;
  } while ((firstBlk = precBlk) != (TmacBlk *)&headChainMacBlk);
  pMacSto = endCurMacSto = NULL; /* return to initial state */
}

static bool frstConcOpnd(void)
/* Answers True if current token is last token of first operand to a '##'
   operator (because cat(a b c, d e f) => a b cd e f). */
{
  Ttok nxtTok;

  if (!(concatFl & COpnd1) || concatFl & (CArgColl | CQuote)) return False;
  /* for case M(x##y), with x => a##b and y => c##d */
  if ((nxtTok = peepNxtMacTok(True)) == ENDDIR) {
    const TsTok *savPMacTok = pMacTok;
    register const TmacStkElt *w = pTopMacStk;

    do {
      /* Go to caller */
      while (IsConcFrame(w)) w = w->prev;  /* skip '##' frames */
      pMacTok = w->pMacTok;
      w = w->prev;
    } while ((nxtTok = peepNxtMacTok(False)) == ENDDIR);
    pMacTok = savPMacTok;}
  return (nxtTok == ENDARG1CONC);
}

bool hereOrAloneInMac(void)
{
  return (!InsideMacro || aloneInMac(0));
}

void initDir(void){
#ifdef VMS
  vmsNoStd = False;
#endif
}

static bool isMacPar(void)
{
  if (curTok.PtrSem == NULL) {  /* non-existant identifier, or macro name */
    register TdescrId *pDescCurId;
    register Tname adBegName;
    ThCode hCode;

    if (pDescTokId == NULL) return False;  /* non-existant identifier */
    hCode = curTok.Hcod;
    adBegName = curTok.IdName;
    SearchSym(adBegName, MacroNotVisible)
    if (pDescCurId==NULL || pDescCurId->nstLvl+1==0) return False;
    curTok.PtrSem = pDescCurId->PidSeman;}
  return (curTok.PtrSem->Kind == ParamMac);
}

static bool macActive(bool isFMEMac)
{
  /* See if irrelevant active status, due to expansion of macro parameter (that
     is itself a macro) after macro entering, though, in the direct text repla-
     cement scheme, it would have been expanded before entering. */
  register const TmacStkElt *wpStk = pTopMacStk;

  if (! isFMEMac) {
    if (pDescTokId == pCurMac) return True;
    if (curMacIsFME) return False;
    do {
      if (IsMacroFrame(wpStk)) {
        if (pDescTokId == wpStk->pPrevMac) {
          if (! wpStk->IsFME) return True;}  /* macro really active */
        else if (wpStk->IsFME) return False;}
    } while ((wpStk = wpStk->prev)->prev != NULL);
    return True;}
  {
    bool flg = False;

    do {
      if (IsMacroFrame(wpStk)) {
        if (pDescTokId==wpStk->pPrevMac && wpStk->MacExpnd && flg) return
						 flg;  /* macro really active */
        if (! wpStk->IsFME) flg = True;}
    } while ((wpStk = wpStk->prev)->prev != NULL);
    return False;}
}

static void manageConc(void)
{
  do {
    carryOutConc();
    /* Getting out of this '##' operator */
    /* First, see if string of concatenation; if so, prepare for next one */
    if (concatFl & CPsdOpnd1) {  /* string */
      register TmacStkElt *w = pTopMacStk;

      while (w->ConcFl & CPsdOpnd1) {
        w->ConcFl ^= COpnd1 | CPsdOpnd1 | COpnd2; /* replace COpnd2 by COpnd1 */
        w = w->prev;}
      concatFl ^= COpnd1 | CPsdOpnd1 | COpnd2 | COpndSeen;}  /* replace COpnd2
								   by COpnd1. */
    else {
      /* Exit of possible exhausted parameter/macro body (mainly to get correct
         value for 'macroExpand'). */
      while (concatFl & CParam) {
        if (peepNxtMacTok(False) != ENDDIR) goto expandQML;  /* 2nd operand
                        not yet exhausted: exiting reach of '##' operator done
                        when 2nd operand get exhausted (see ENDDIR management
                        in getTokFromMac() ). */
        removingParMacFrame = True;
        {
          const TvalTok savTok = curTok;

          getTokFromMac();  /* done that way so that possible SUNDR tokens
								      obeyed. */
          curTok = savTok;}
        removingParMacFrame = False;}
#if Debug
      if (! IsConcFrame(pTopMacStk)) sysErr(ExCod3);
#endif
      popMacStk();  /* remove '##' frame */
expandQML:;}
    if (curTok.tok == IDENT) expandIfMac();
  } while (frstConcOpnd());
}

static const TsTok *manageJmpTok(register const TsTok *x /*~ResultPtr*/)
/* Must only be used to peep */{
  for (;;) {
    switch ((Ttok)*x) {
      case DPRAG :
        if (transmitDP()) return x;
        x += IncrPMacT;
        for (;;) {
          x = manageJmpTok(x) + IncrPMacT;
          switch ((Ttok)*(x - IncrPMacT)) {
            case ENDDPRAG: goto exitL;
            case IDSTNU:
              switch ((Ttok)*(x - (IncrPMacT - 1))) {
                case FORCEMACEXP: case NOMACEXP: x += sizeof(pDescTokId); break;
                case IDENT: x += FullLgt((Tname)x); break;
                default: {
                    const TsTok *const savPMacTok = pMacTok;

                    pMacTok = x;
                    skipToEndStNu();
                    x = pMacTok;
                    pMacTok= savPMacTok;}}
            /*~NoDefault*/}}
exitL:
        continue;
      case ENDBLK: x = GoNxtBlk(x); continue;
      case SKIPTO: {const TsTok *w = x + IncrPMacT;;SkipTo(w); x = w; continue;}
      case SUNDR : x += IncrPMacT; continue;
      default: return x;}}
}

static TcharStream nxtChFromConc(void)
{
  return (TcharStream)*pConcTxt++;
}

TcharStream nxtChFromMac(void)
{
  for (;;) {
    if (endCurCharSubBlk == NULL) {
      endCurCharSubBlk = pMacTok + *pMacTok;
      if (*pMacTok != 0 /* not end marker */) pMacTok++;}
    if (pMacTok != endCurCharSubBlk) break;
    endCurCharSubBlk = NULL;
    if (*pMacTok != 0) pMacTok = GoNxtBlk(pMacTok);
    else {  /* end marker found */
      pMacTok++;  /* skip it */
      return -1;}}  /* end of character stream */
  return (TcharStream)*pMacTok++;
}

static Ttok peepNxtMacTok(bool mayBeInsideTok)
{
  const TsTok *const savPMacTok = pMacTok, *locPMactok;
  Ttok w;

  if (mayBeInsideTok && CurTokNotAnalz() && pNxtCh==&nxtChFromMac
							      ) skipToEndStNu();
  locPMactok = manageJmpTok(pMacTok);
  w = (Ttok)*locPMactok++;
  peepedTokVal = (uint)*locPMactok;
  pMacTok = savPMacTok;
  return w;
}

static TsavTokCtx dfltTokCtx, *pTopTokCtx = NULL; /* pointer on top of peeped
								tokens stack. */
static TdescrId *savCurPCM;
static const TsTok *savCurPMT;

Ttok peepNxtTok(void)
/* Peeps next token.
   Current token CANNOT be CSTNU(1) or CSTST (except if already analyzed). */
{
  ulong valSundries = 0;
  TdescrId *oldPDCI = pDescTokId;
  bool oldFrstChOfNb = frstChOfNb;
  TvalTok oldCurTok;
  TindentChk oldOldSpCnt = oldSpaceCount, oldSpCnt = spaceCount;
  TmacExpNb oldCMEN = curMacExpNb;
  TmacLvl oldMacLvl = macLvl;
  TmacParCtr oldMPC = macParCtr;
  bool oldSysMacro = sysMacro, oldAdjMacro = adjMacro;
  TdescrId *oldPCM = pCurMac;
  const TsTok *oldPMT = pMacTok;
  Ttok newCurTok;

#if Debug
  if (CurTokNotAnalz()) sysErr(ExCod6);
#endif
  if (curTok.tok == IDENT) curTok.IdName = storeName(curTok.IdName,ObjectSpace);
  oldCurTok = curTok;
  getTokLvl++;
  for (;;) {
    GetNxtTok();  /* may cause recursive call to peepNxtTok */
    if (curTok.tok != SUNDR) break;
    valSundries = valSundries>>WidthSundr | (ulong)curTok.Val<<
	((sizeof(valSundries)*CHAR_BIT/WidthSundr - 1) * WidthSundr);}  /* fill
				by the left to keep order of SUNDR values; no
				overflow checking deemed necessary. */
  getTokLvl--;
  newCurTok = curTok.tok;
  {
    TsavTokCtx *w;

    if (pTopTokCtx == NULL) {
      w = &dfltTokCtx;  /* for faster processing (most of the time, stack
								  depth < 2). */
      savCurPCM = pCurMac;   /* save furtherest point			   */
      savCurPMT = pMacTok;}  /* 		      attained in peeping. */
    else MyAlloc(w, sizeof(TsavTokCtx));
    w->newCurTok = curTok;
    w->newPDCI = pDescTokId;
    w->newFrstChOfNb = frstChOfNb;
    w->newOldSpCnt = oldSpaceCount, w->newSpCnt = spaceCount;
    w->valSundries = valSundries;
    pMacTok = oldPMT;
    if (curMacExpNb==oldCMEN && macParCtr==oldMPC)  /* no need for big save */
      w->bigSave = False;
    else {
      w->newCMEN = curMacExpNb;
      w->newMacLvl = macLvl;
      w->newMPC = macParCtr;
      w->newSysMacro = sysMacro, w->newAdjMacro = adjMacro;
      curMacExpNb = oldCMEN;
      macLvl = oldMacLvl;
      macParCtr = oldMPC;
      sysMacro = oldSysMacro, adjMacro = oldAdjMacro;
      if (pNxtMac == NULL) {
        pCurMac = oldPCM;
        if (pCurMac->TabArg==NULL && InsideMacPar) pMacTok = NULL;}
      w->bigSave = True;}
    w->newPGNT = pCurGNT;
    w->prev = pTopTokCtx;
    pTopTokCtx = w;}
  pCurGNT = &restoPeepedTok;
  curTok = oldCurTok;
  pDescTokId = oldPDCI;
  frstChOfNb = oldFrstChOfNb;
  oldSpaceCount = oldOldSpCnt, spaceCount = oldSpCnt;
  return newCurTok;
}

static void restoPeepedTok(void)
{
  TsavTokCtx *w = pTopTokCtx;

  if (w->valSundries != 0) {
    curTok.tok = SUNDR;
    do {
      curTok.Val = (uint)w->valSundries & ((1 << WidthSundr) - 1);
      w->valSundries >>= WidthSundr;
    } while (curTok.Val == 0);}  /* loop until first SUNDR value appears */
  else {
    curTok = w->newCurTok;
    pDescTokId = w->newPDCI;
    frstChOfNb = w->newFrstChOfNb;
    oldSpaceCount = w->newOldSpCnt; spaceCount = w->newSpCnt;
    pMacTok = savCurPMT;
    if (w->bigSave) {
      curMacExpNb = w->newCMEN;
      macLvl = w->newMacLvl;
      macParCtr = w->newMPC;
      sysMacro = w->newSysMacro; adjMacro = w->newAdjMacro;
      pCurMac = savCurPCM;}
    pCurGNT = w->newPGNT;
    if ((pTopTokCtx = w->prev) != NULL) free(w);
    if (curTok.tok == IDENT) {
      getTokLvl++;  /* to prevent possible macro parameters from showing */
      expandIfMac();
      if (--getTokLvl==tokLvl0 && curTok.tok==IDENT && curTok.PtrSem!=NULL
		   && curTok.PtrSem->Undf && !curTok.PtrSem->ModifSJ) errWN(
                                          UndfndId|Warn1|Rdbl, curTok.IdName);}}
  if (getTokLvl == 0) storeTokTxt(curTokTxt());
}

/*~Undef TsavTokCtx, dfltTokCtx, pTopTokCtx, savCurPCM, savCurPMT */
/*~UndefTag _tSavTokCtx */

static void popCondStk(void)
{
  curIfArmActiv = pTopCondStk->curIfArmActiv;
  trueIfPartSeen = pTopCondStk->trueIfPartSeen;
  elseSeen = pTopCondStk->elseSeen;
  lvlActiv = pTopCondStk->lvlActiv;
  oldSpaceCount = pTopCondStk->savOldSpCnt;
  pTopCondStk = freeCondStkElt(pTopCondStk);
}

static void popMacStk(void)
{
  register TmacStkElt *pTMS = pTopMacStk;

  if (! IsConcFrame(pTMS)) pMacTok = pTMS->pMacTok;  /* not exiting from a '##'
								    operator. */
  if (concatFl != CNoBit) {
    TconcFl oldConcFl = concatFl;

    concatFl = pTMS->ConcFl | oldConcFl&CArgColl;
    if (oldConcFl & CParam) concatFl |= oldConcFl & COpndSeen;}
  pTopMacStk = freeMacStkElt(pTMS);
}  

static void procBitOffset(void)
{
  TpTypeElt type = (isType())? declType() : (asgnExpr(), valueNotUsed(),
								     cExp.type);
  size_t offset = 0;
  bool end = False;

  cExp.Einfo = 0;  /* reset all flags */
  if (type != NULL) {
    if (type->typeSort != Struct) {err0(NotStrunType); cExp.ErrEvl = True;}}
  else cExp.ErrEvl = True;
  if (! Found(COMMA)) {end = (bool)-1; curTok.IdName = NULL;}
  else if (curTok.tok != IDENT) {err0(MsngMembName); cExp.ErrEvl = True; goto
								    noMbrNameL;}
  if (! cExp.ErrEvl) {
    const TdeclElt *membList;
    bool addSizeFl = True;

    for (membList = BaseStrunType(type)->MemberList; (int)end<=0 &&
				     membList!=NULL; membList = membList->cdr) {
      size_t membSize;

      if (isSameName(membList->hCode, membList->idName, curTok.Hcod, curTok.
								      IdName)) {
        end = True;
        if (NxtTok()==ADDOP && (TkAdd)curTok.Val==Add) GetNxtTok();
        else addSizeFl = False;}  /* not ending '+' */
      if (membList->FldFl) {
        size_t newOffset = offset + (((membSize = membList->FldSize)==0 &&
		  membList->idName==NULL /* forced padding ? */)? CHAR_BITC :
							     membList->FldSize);

        if ((newOffset - 1)/CHAR_BITC != offset/CHAR_BITC /* field will
			     not fit into current storage unit */) offset =
				(offset + (CHAR_BITC - 1))/CHAR_BITC*CHAR_BITC;}
      else if (membList->type != NULL) {
        size_t bitAlgn = (1 << membList->type->Algn)*CHAR_BITC;

        offset = ((offset + (CHAR_BITC - 1))/CHAR_BITC*CHAR_BITC + bitAlgn - 1)/
								bitAlgn*bitAlgn;
        membSize = membList->type->size*CHAR_BITC;}
      if (addSizeFl) offset += membSize;}
    if (! end) {
      errWSTNSS(UnknMember, "__bitoffset", type, curTok.IdName, NULL, NULL);
      cExp.ErrEvl = True;}}
  if (!end && NxtTok()==ADDOP && (TkAdd)curTok.Val==Add) GetNxtTok();
noMbrNameL:
  FreeExpType1(type);
  cExp.Uval = (TcalcU)offset;
  cExp.type = NoConstTyp(&natTyp[ULongDpl]);
}

static void procCDefined(void)
{
  const TsemanElt *w = curTok.PtrSem;

  cExp.Uval = (TcalcU)((
                w!=NULL
                || (w = searchSymTab(storeName(curTok.IdName, TagSpace)))!=NULL)
               && w->SysElt);
  cExp.type = &boolCstTypeElt;
}

static void processDefined(void)
{
  cExp.Uval = (TcalcU)(IsMacro(pDescTokId));
  if ((bool)cExp.Uval) pDescTokId->nstLvl |= 1 << (MPosFlags + MRPosUsdFl);
  cExp.type = NoConstTyp(((sysHdrFile && ifDirExp)?
					    &longCstTypeElt : &boolCstTypeElt));
}

static void procSameType(void)
{
  TpTypeElt lType = (isType())? declType() : (asgnExpr(), valueNotUsed(),
								     cExp.type);

  if (! Found(COMMA)) errWS(Exptd, ",");
  if (isType()) cExp.type = declType();
  else asgnExpr(), valueNotUsed();
  errQalTyp1 = NULL;
  cExp.Uval = (TcalcU)(compatType(lType, cExp.type, FctCall) && errQalTyp1 ==
                 NULL || (paramTxt = NULL, lExp.type = lType, compatNumType()));
  FreeExpType1(lType);
  FreeExpType(cExp);
  cExp.type = &boolCstTypeElt;
  cExp.Einfo = 0;  /* reset all flags */
}

static void pushMacStk(void)
{
  register TmacStkElt *w = allocMacStkElt();

  w->pPrevMac = pCurMac;
  w->tArg = pCurMac->TabArg;
  w->pMacTok = pMacTok;
  w->infoM.u11.bid2 = 0;  /* reset flags */
  w->ConcFl = concatFl;
  w->prev = pTopMacStk;
  pTopMacStk = w;
}

/* restoPeepedTok(void) : see after peepNxtTok(void) */

void restoPnxtChState(void)
{
  if (pNxtCh == &nxtChFromConc) pNxtCh = &nxtChFromMac;
  if (savPMTQuo != NULL) {pMacTok = savPMTQuo; savPMTQuo = NULL; freeMacSto(
								 pseudoMacQuo);}
}

static void skipParams(uint parenCtr)
/* Skip (excess) arguments */
{
  for (;; GetNxtTok()) {
    switch (curTok.tok) {
      case ENDDPRAG: dpragNst--; continue;
      case ENDPROG: return;
      case LPAR: parenCtr++; continue;
      case RPAR:
        if (parenCtr == 0) return;
        parenCtr--;
        continue;
      default:
        analTok();}}
}

static void skipRestOfLinePhase4(void) /*~NeverReturns*/
{
  condDirSkip = True;  /* to prevent detection of syntax errors that are not
					       errors in preprocessor-tokens. */
  noExpand = True;
  SkipRestOfLine;
}

static void skipToEndStNu(void)
{
  const TsTok *const savEndCCSB = endCurCharSubBlk;

  while (nxtChFromMac() >= 0) {}
  endCurCharSubBlk = savEndCCSB;
}

static void storeChInFName(Tchar x)
{
  if (isspace(x)) {
    if (chkPortbl && !nonPortFNSeen) {
      err0(NonPortFName|Warn1);
      nonPortFNSeen = True;}}
  else {
    if (posInBufIFN == sizBufIFN) {  /* buffer full */
      sizBufIFN += FILENAME_MAX;
      MyRealloc(bufInclFName, sizBufIFN);}
    bufInclFName[posInBufIFN++] = x;}
}

static void storeConcChar(char c)
{
  if (pConcatBuf < &concatBuf[NbElt(concatBuf) - 1]) *pConcatBuf++ = c;
}

static void storeEndTok(TdpNst oldDPnst, TsortEndTok sort)
{
  if (dpragNst != oldDPnst) {err0(EndDPExptd); storeMacChunk(ENDDPRAG, NULL,
						     0-0); dpragNst = oldDPnst;}
  storeMacChunk(ENDDIR, NULL, (size_t)sort);
}

static void storeMacChar(char c)
{
  if (pMacSto == endCurMacSto) {
    *pLgtCharSubBlk = (TsTok)(pMacSto - pLgtCharSubBlk);  /* number of character
						    (+1) in filled sub-block. */
    storeMacChunk((Ttok)0, NULL, 0);  /* to get another macro storage block */
    pMacSto -= IncrPMacT - SizOfCharBlkLgtFld;  /* to remove fake 0s */
    pLgtCharSubBlk = pMacSto - SizOfCharBlkLgtFld;}
  *pMacSto++ = (TsTok)c;
}

static void storeMacChunk(Ttok tok, const TsTok *x, size_t size)
/* 'size' is the size of the information pointed by 'x'. Stores 'tok', 'size'
   as TsToks, followed by (if 'x' non NULL) 'size' TsToks. The whole chunk is
   stored contiguously. */
{
  register TsTok *ptr = pMacSto;

  if (((x == NULL)? ptr : ptr + size) + IncrPMacT > endCurMacSto) {  /* not
			enough space at end of current block (there must always
			stay at least one slot for eventual ENDBLK). */
    TmacBlk *nxtBlk;

    if (freeMacBlk != NULL) {nxtBlk = freeMacBlk; freeMacBlk = NULL;}
    else MyAlloc(nxtBlk, sizeof(TmacBlk));
    /*~ zif ((ptrdiff_t)SizeMacroStorageChunk < IncrPMacT + (LgtHdrId +
	MaxLgtId) + IncrPMacT) "SizeMacroStorageBlk too small" */ /* a macro
				storage chunk must be capable of holding at
				once: IDENT, identifier Tname, ENDBLK. */
    ctrMSB++;
    if (ptr == NULL) curMacBlk = (TmacBlk *)&headChainMacBlk;
    /*~ zif Offset(TmacBlk, next) != 0
			    "field 'next' must be first of structure TmacBlk" */
    else {
      *ptr = (TsTok)ENDBLK;  /* marks end of used part of block */
      *(ptr + 1) = (TsTok)(ptr - (TsTok *)curMacBlk);}  /* displacement to
				    beginning of current macro storage block. */
    /*~zif (SizeMacroStorageBlk > (1 << sizeof(TsTok)*CHAR_BIT) + sizeof(TsTok))
					      "SizeMacroStorageBlk too large" */
    curMacBlk->next = nxtBlk;  /* chain new block */
    nxtBlk->prec = curMacBlk;
    nxtBlk->next = NULL;
    ptr = &(nxtBlk->tokSto[0]);
    endCurMacSto = &(nxtBlk->tokSto[(TsTok)SizeMacroStorageChunk - IncrPMacT]);
    curMacBlk = nxtBlk;
    pHeadChain = &(nxtBlk->headChain);
    *pHeadChain = 0;}  /* no macro header yet in new block */
  *ptr++ = (TsTok)tok;		/* store first cTok */
  *ptr++ = (TsTok)size;		/* 'size' used as second cTok */
  if (x != NULL) {memcpy(ptr, x, size); ptr += size;}
  pMacSto = ptr;
}

static void storeTok(void)
{
  Ttok currTok;

  switch (currTok = curTok.tok) {
    TsTok *oldPMS;

  case IDENT:
    if (insideDefineBody && isMacPar()) {  /* macro parameter */
      TsemanElt *pSem = curTok.PtrSem;

      storeMacChunk((!pSem->ReallyUsed || pSem->UsedMorThOnce || InsideDPragma)?
					   MACPAR : MACPAR1, NULL, pSem->NbPar);
      if (! pSem->Defnd) {
        errWN(NotUsdPar|Warn1|Rdbl, curTok.IdName);
        if (! allErrFl) pSem->Defnd = True;}
      else {
        if (! InsideDPragma) {
          if (pSem->ReallyUsed) pSem->UsedMorThOnce = True;
          else pSem->ReallyUsed = True;}
        pSem->Used = True;}
      return;}
    oldPMS = pMacSto;
    storeMacChunk(currTok, (const TsTok *)curTok.IdName,FullLgt(curTok.IdName));
    goto endCST;
  case CSTNU: if (sysHdrFile) currTok = CSTNU1;
    /*~NoBreak*/
  case CSTNU1: case CSTST: {
      TsTok w;

      oldPMS = pMacSto;
      storeMacChunk(currTok, /*~Init w*/&w, 1);  /* so that length of 1st
				   sub-block inside same macro storage block. */
      pLgtCharSubBlk = pMacSto - SizOfCharBlkLgtFld;
      if (currTok == CSTST) {
        analStrCst(&storeMacChar);
        if ((bool)curTok.Val) currTok = WHITESPACE + 1;}  /* wide-character
								      string. */
      else analNumCst(&storeMacChar);
      *pLgtCharSubBlk = (TsTok)(pMacSto - pLgtCharSubBlk);  /* number of
					 characters (+1) in ending sub-block. */
      storeMacChunk((Ttok)0, NULL, 0); pMacSto--;  /* end marker = 0 (only one
								byte needed). */
      /*~zif ENDBLK == (Ttok)0 "ENDBLK must be non-zero" */
      goto endCST;}
  case FORCEMACEXP: case NOMACEXP:
    oldPMS = pMacSto;
    storeMacChunk(currTok, (TsTok *)&pDescTokId, sizeof(pDescTokId));
endCST:
    /* Find beginning of stored token and replace its code by IDSTNU (in anti-
       cipation of possible '##' operator). */
    GetTokAdr(oldPMS);
    *oldPMS = (TsTok)IDSTNU;
    *(oldPMS + 1) = (TsTok)currTok;  /* real token value as extension */
    break;
  case ENDDPRAG: dpragNst--; /*~NoBreak*/
  default: storeMacChunk(currTok, NULL, curTok.Val);}
}

bool transmitDP(void)
{
  return (!(!gblArgColl && (concatFl==CNoBit || concatFl & (CShield | CArgColl)
					  )) && !dirLineFl || insideDefineBody);
}

bool visibleFromMac(const TsemanElt *pTag)
{
  if (isFNameVisible(pCurMac->DfileName, pTag)) return True;
  /* Find if possible calling macro(s) defined in visible file */
  {
    const TmacStkElt *w;

    if ((w = pTopMacStk) != NULL)  /* because of peepNxtTok() */
      for (; w->prev != NULL; w = w->prev)
        if (IsMacroFrame(w) && isFNameVisible(w->pPrevMac->DfileName, pTag)
								 ) return True;}
  return False;
}

/* End DCDIR.C */
