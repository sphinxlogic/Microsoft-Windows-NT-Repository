/* DCDIR.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#pragma noinline (errMac, errMacCall, errMacDef)
#pragma noinline (frstConcFrame)
#pragma noinline (popMacStk)
#endif

#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
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

#define CDfndHCode	0x3900A2
#define DefineHCode	0x25729
#define DefinedHCode	0xBB3F4
#define DispBegSto	(DispChain + 1)
#define DispChain	(TsTok)1
#define ElifHCode	0x1924
#define ElseHCode	0x1955
#define EndifHCode	0x7E69
#define ErrorHCode	0x81E5
#define ExtentHCode	0x5B4956
#define FreeArg(x)	{freeMacSto(*x); free(x);}
#define GetTokAdr(x, y)							       \
  if (x == NULL) x = &(headChainMacBlk->tokSto[0]);			       \
  else if (*x == (TsTok)ENDBLK) x = &(y->next->tokSto[0]);
#define GoNxtBlk(x)							       \
  x = &(((TmacBlk * /*~OddCast*/)(x - *(x + 1)))->next->tokSto[0])
#define IfHCode		0x105
#define IfdefHCode	0x8431
#define IfndefHCode	0x299E2
#define IncludeHCode	0xD4DCD
#define IncrMacParCtr	if (++macParCtr == 0) macParCtr = (2)  /* 2 to keep
								      parity. */
#define IncrPMacT	2
#define IndexHCode	0x12452F
#define LineChng	0
#define LineHCode	0x1C5C
#define LgtMacHdr	DispBegSto
#define MacActWNoPar	QuasiNULLval(TsTok **)
#define MRPosAdjFl	(MRPosSysFl + 1)
#define MRPosConcFl	(MRPosAdjFl + 1)
#define MRPosSysFl	0
#define MPosFlags	CHAR_BIT
#define MPosLineNb	(MPosFlags + CHAR_BIT - 1) /* -1 because of LSIGN_BIT */
#define MemberHCode	0x5B78B6
#define NewArg								       \
  *pNewTabArg++ = pMacSto,						       \
		   *(pMacSto - 1) = (TsTok)(pMacSto - &(curMacBlk->tokSto[0])) \
  /*~ zif (DispNSId != LgtHdrId - 1)  "Watch out ! Where is displacement needed\
 to compute address of beginning of block stored?" */
#define PragmaHCode	0x306B6
#define PseudoName	ConvTname("\0\0")  /* name for pseudo-macro holding
				token string corresponding to each parameter. */
#define SamTypeHCode	0x8F50D2C
#define SizOfCharBlkLgtFld 1
#define SizeMacroStorageChunk ((SizeMacroStorageBlk - Offset(struct _bid,      \
							bid))/sizeof(TsTok))
#define SkipRestOfLine	longjmp(*curErrRet, 1)
#define SkipTo(x)	memcpy(&x, x, sizeof(void *))
#define UndefHCode	0xA565
#define UsrLvlPar	(LineChng + 1)

DefIsSameName

typedef uint TconcMsq /*~RootType*/;
/* Bit positions for type TconcMsq (arbitrary) */
#define CNoBit	(TconcMsq)0
#define COpnd1	(TconcMsq)1
#define COpnd2	(COpnd1 << 1)
#define CParam	(COpnd2 << 1)
#define CActiv	(CParam << 1)
#define CFme1	(CActiv << 1)
#define CISN	(CFme1  << 1)
#define MaxCMsq	CISN

typedef struct _TmacBlk TmacBlk;
struct _bid{  /* just to allow computation of SizeMacroStorageChunk */
  TmacBlk *bid1, *bid2;
  TsTok bid3, bid;
};
struct _TmacBlk {
  TmacBlk *next, *prec;  /* 'next' must be first field (cf 'storeMacChunk()') */
  TsTok headChain, tokSto[(TsTok)SizeMacroStorageChunk];
};
/*~zif (Offset(struct _bid, bid) != Offset(TmacBlk, tokSto))
				      "struct _bid non coherent with TmacBlk" */
/*~zif (SizeMacroStorageBlk < sizeof(TmacBlk))
				     "incorrect struct _TmacBlk organization" */

#define concFl		infoM.s11._concFl
#define fme1		infoM.s11._fme1
#define concAsPar	infoM.s11._concAsPar
#define usrLvlPFl	infoM.s11._usrLvlPFl
#define noFreeArg	infoM.s11._noFreeArg
#define macExpNb	infoM.s11._macExpNb
#define insideMUMP	infoM.s11._insideMUMP
#define NbBitsConcMsq	6
/*~zif MaxCMsq >= (TconcMsq)1<<NbBitsConcMsq
			       "Bad field size for storing 'TconcMsq' values" */
typedef union {
  struct {
    TmacExpnd _macExpNb:SmallestWdthInt;  /* field needed because of embedded
	 			macro calls (will run into problem in the
				(unreasonable) case where there are more than
				2**16-1 macro expansions in a given compilation
				unit). */
    TconcMsq _concFl:NbBitsConcMsq;  /* flags for '##' operator */
#undef NbBitsConcMsq
    bool _fme1:1;
    bool _concAsPar:1;
    bool _usrLvlPFl:1;
    bool _noFreeArg:1;
    bool _insideMUMP:1;
  } s11;
  TallocUnit u11;  /* for good alignment of following fields */
} TinfoM;
typedef struct _TmacStkElt TmacStkElt;
struct _TmacStkElt {
  const TsTok *pMacTok;
  TsTok **pArg;  /* because of case 'Mac(Mac(3))'; NULL for '##' frame */
  TdescrId *pMac;  /* preceding macro (for embedded macro calls); NULL for
							     parameter frame. */
  TinfoM infoM;
  const TmacStkElt *actFloor;
  TmacStkElt *prec;
};
/*~ zif Offset(TmacStkElt, prec) != Offset(TqElt6, next) "Beware, freed '##'\
 frames may still be referred to" */ /* see management of ptrConcFrameBef */

struct _TcondStkElt {
  bool curIfArmActiv, trueIfPartSeen, elseSeen, activLvl;
  int savOldSpCnt;
  TcondStkElt *prec;
};

/* Functions profiles */
static void checkConc(void), checkDir(Tname), errMac(Terr, const TdescrId *),
  errMacCall(Terr, Tstring),
  errMacDef(Terr, TdescrId *, Tstring)/*~NeverReturns*/, exitMacro(void),
  exitParam(void), freeMac(TdescrId *x), freeMacSto(TsTok *), getTokFromMac(
  void), otherDir(void), popCondStk(void), popMacStk(void), procCDefined(void),
  procDefined(void), pushMacStk(void), skipRestOfLinePhase4(void)
  /*~NeverReturns*/, storeFileName(Tchar), storeMacChar(char), storeMacChunk(
  Ttok, const TsTok *, size_t), storeTok(void);
static TmacStkElt *allocMacStkElt(void), *freeMacStkElt(TmacStkElt *);
static TcondStkElt *allocCondStkElt(void), *freeCondStkElt(TcondStkElt *);
static TsTok *createMacroHeader(Tname);
static bool evalIfExp(void), frstConcOpnd(void), macActive(bool);
static TmacStkElt *frstConcFrame(void);
static bool isMacPar(void);
static const TsTok *manageJmpTok(const TsTok *);
static Ttok peepNxtMacTok(bool);

/* Global variables */
static const TmacStkElt *activityFloor = NULL;  /* from this level up, the 
		macro call stack is used to expand a macro that would, in the
		direct text replacement scheme, have been expanded before. */
static bool activLvl = True;
static bool argCollect = False;  /* flag 'currently collecting macro param' */
static Tchar *begNameZone;
static bool concArgSeen;  /* either argument to '##' operator generated */
static TconcMsq concatFl = CNoBit;  /* flags for '##' operator */
static Tstring curFileN;
static bool curIfArmActiv = True;
static uint decrGetTokLvl = 1;
static bool elseSeen;
static const TsTok *endCurCharSubBlk = NULL;
static bool gotOutOfBody;
static bool ilgInclArgSeen;
static bool incldSysFile;
static TlineNb nLine;
static TdescrId *pCurMac;
static const TsTok *pCurMacNb;
static TsTok *pLgtCharSubBlk;  /* pointer on length field of currently filled
				      character (sub-)block in macro storage. */
static const TsTok *pMacTok;  /* 'source' pointer when exploring macro */
static size_t posNameZone, sizNameZone;
static TsTok *pseudoMacQuo;
static TcondStkElt *pTopCondStk;
static TmacStkElt *pTopMacStk = NULL;  /* pointer on top of macro stack */
static const TsTok *savPMTQuo = NULL;  /* not inside '#' operator */
static bool trueIfPartSeen;
static bool usrLvlParFl = True;  /* macro parameter comes from main level */
/* Variables to manage macro storage space */
static TsTok *pMacSto = NULL, *endCurMacSto = NULL, *pHeadChain;
static TmacBlk *curMacBlk,
	       *freeMacBlk = NULL,  /* available free block (for efficiency) */
	       *headChainMacBlk;    /* head of macro storage chunks chain */

/* External variables */
bool adjMacro = False;
TmacExpnd begMacExpNb;
bool insideMultUsedMacPar = False;
TlineNb lastSCOLline;
TmacLvl macLvl = 0;
TmacParCtr macParCtr = 1;  /* 1 because of 'lastIdMacParCtr' (dcexp.c) */
bool peepTok = False;
TdescrId *pNxtMac = NULL;
bool sysMacro = False;

/* Pseudo-local variables */
static bool cmpsgHdr;

void manageDir(void)
{
  static uint begLineNb;
  static Tstring begFileName;
  bool oldCIAA = curIfArmActiv;
  bool savMacroExpand = macroExpand;
  jmp_buf localJmpBuf, *savErrRet = curErrRet;

  curErrRet = &localJmpBuf  /*~ LocalAdr */;
  if (setjmp(*curErrRet) != 0) {  /* skip to end of directive line */
    static const Ttok zEndDir[]= {NoSwallowTok, ENDDIR, EndSTok};

    skipTok(zEndDir);}
  else {
    begNameZone = NULL;  /* for use as '#include' flag */
    macroExpand = False;
    condDirSkip = False;  /* to de-inhibit error messages, and inhibit token
						     skipping in GetNxtTok(). */
    if (dpragNst != 0) {
      curTok.tok = WHITESPACE;  /* destroy possible ENDDIR left in curTok.tok */
      err0(NoDirInDPrag);
      skipRestOfLinePhase4();}  /* see 'slashF()' */
    if (NxtTok()!=IDENT && curTok.tok!=IF && curTok.tok!=ELSE) errPanic(
							    DirNameExptd, NULL);
    switch (curTok.hCod) {
      bool trueCond;

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
        w->activLvl = activLvl;
        w->savOldSpCnt = oldSpaceCount;
        w->prec = pTopCondStk;
        pTopCondStk = w;}
      {
        bool w1;

        if (! curIfArmActiv) activLvl = False;
        trueIfPartSeen = elseSeen = False;
        if (! activLvl) skipRestOfLinePhase4();
        if (curTok.hCod == IfHCode) w1 = evalIfExp();
        else {
          w1 = (curTok.hCod == IfndefHCode);
          w1 = (NxtTok() == IDENT)? (pDescCurId!=NULL &&
		 pDescCurId->nstLvl<0) ^ w1 : (err0(MacNameExptd), False);
          GetNxtTok();}
        curIfArmActiv = w1;}
      break;
    case ElifHCode:
      checkDir(ConvTname("\4\0elif"));
      if (pTopCondStk == NULL) err0(NoPrecIf);
      trueCond = evalIfExp();
      goto elseL;
    case ElseHCode:
      checkDir(ConvTname("\4\0else"));
      if (pTopCondStk == NULL) err0(NoPrecIf);
      if (elseSeen) err0(ElseAlrdSeen);
      elseSeen = True;
      GetNxtTok();
      trueCond = True;
elseL:
      if (!activLvl || pTopCondStk==NULL) break;
      trueIfPartSeen |= curIfArmActiv;
      curIfArmActiv = trueCond & !trueIfPartSeen;
      oldSpaceCount = pTopCondStk->savOldSpCnt;
      break;
    case EndifHCode:
      checkDir(ConvTname("\5\0endif"));
      if (pTopCondStk == NULL) err0(NoPrecIf);
      GetNxtTok();
      if (pTopCondStk == NULL) break;
      popCondStk();
      break;
    default:
      if (curIfArmActiv) otherDir();
      else skipRestOfLinePhase4();}
    while (curTok.tok != ENDDIR) {
      if (curTok.tok == DPRAG) {
        macroExpand = True;
        if ((curTok = manageDPrag()).tok == WHITESPACE) GetNxtTok();}
      else errPanic(EndOfLineIgnd, NULL);}}
  if (begNameZone != NULL) manageInclude(begNameZone, incldSysFile,
			cmpsgHdr); /* '#include' (taken into account here,
			so that all possible errors shown on previous file). */
  condDirSkip = ! curIfArmActiv;  /* condDirSkip must be set only after all
						   errors have been detected. */
  if (! curIfArmActiv) {if (oldCIAA) {begLineNb = lineNb + cumDeltaLineNb
						  ; begFileName = curFileName;}}
  else if (!oldCIAA && !adjustFile && curFileName==begFileName
		 ) cumNbSkippedLines += lineNb - 1 + cumDeltaLineNb - begLineNb;
  ignoreErr = False;
  macroExpand = savMacroExpand;
  curErrRet = savErrRet;
}

static void otherDir(void)
{
  switch (curTok.hCod) {
  case DefineHCode : {
      TdescrId *pDescMacId;
      uint parCtr = 0;
      bool masking;
      TlineNb oldLineNb;

      checkDir(ConvTname("\6\0define"));
      if (argCollect) {errMacCall(DccCantProcDef | Warn3, NULL)
							      ; SkipRestOfLine;}
      GetNxtTok();
      masking = Found(MASKING);
      if (curTok.tok != IDENT) errPanic(MacNameExptd, NULL);
      if (pDescCurId != NULL) {
        if (pDescCurId->nstLvl < 0) {errMac(MacAlrdDef|Warn3|PossErr,
						   pDescCurId); SkipRestOfLine;}
        else if (!masking && curTok.ptrSem->type!=NULL) errId(NameAlrdInUse|
			Warn3|PossErr, curTok.ptrSem, curTok.name, NULL, NULL);}
      enterBlock();		/* just to store parameter names */
      pDescMacId = enterSymTabHC1((Tname)createMacroHeader(curTok.name),
								   curTok.hCod);
      pDescMacId->nstLvl = IncplMacDef;  /* to remove immediately (in case of
	       erroneous macro definition) macro name from blocks name space. */
      if (nxtChFromTxt() == (TcharStream)'(') {  /* there is an (empty ?)
							      parameter list. */
        parCtr++;	/* value 0 is for symbol (no parenthesis after name) */
        if (NxtTok() != RPAR) {
          do {
            TsemanElt *ptrParId;
            DefSem(defndMacPar, ParamMac, True);
            DefSem1(notUsdMacPar, ParamMac, False, True, False);

            if (curTok.tok != IDENT) errMacDef(ParNameExptd, pDescMacId,NULL);
            if (parCtr > UCHAR_MAX) errMacDef(TooManyMacPar, pDescMacId,
						      longToS((long)UCHAR_MAX));
            if ((ptrParId = enterSymTabHC(storeName(curTok.name, ObjectSpace),
		     curTok.hCod)) == NULL) errMacDef(ParAlrdExist, pDescMacId,
							  nameToS(curTok.name));
            ptrParId->noPar = parCtr++;  /* parameter number */
            ptrParId->defLineNb = lineNb;
            ptrParId->defFileName = curFileName;
            GetNxtTok();
#ifdef _AIX
            if (Found(NOTUSED)) ptrParId->infoS = notUsdMacPar;
            else ptrParId->infoS = defndMacPar;
#else
            ptrParId->infoS = (Found(NOTUSED))? notUsdMacPar : defndMacPar;
#endif
            if (Found(SIDEFFOK)) ptrParId->usedMorThOnce = True;
          } while (Found(COMMA));}
        if (curTok.tok != RPAR) errMacDef(RParExptd, pDescMacId, NULL);}
      else backUp();
      pDescMacId->nstLvl =
	(long)((((ulong)lineNb<<(MPosLineNb - MPosFlags - MRPosAdjFl) |
		  (ulong)adjustFile) << MRPosAdjFl |
		  (ulong)sysHdrFile) << (MRPosSysFl + MPosFlags) |
		  parCtr |
		  LSIGN_BIT);  /* save definition line number, system header
				 file flag, and number of parameters. */
      pDescMacId->dFileName = curFileName;  /* birth place */
      pDescMacId->pTabArg = NULL;  /* marks macro not active */
      /* Store macro body, in token form (except for identifiers, strings
         and numbers, because they can't be exactly decompiled). */
      oldLineNb = lineNb;
      if (NxtTok() != ENDDIR) {
        TsTok *pLastTok = NULL;
        TmacBlk *oldCurMacBlk;
        bool lastTokIsConc = False;

        do {
          if (lineNb != oldLineNb) {storeMacChunk(SUNDR, NULL, LineChng)
							  ; oldLineNb = lineNb;}
          if (curTok.tok == CONCATOP) {
            /* Prefix form for ## operator */
            if (lastTokIsConc || pLastTok==NULL) {err0(MsngConcOpnd); continue;}
            lastTokIsConc = True;
            GetTokAdr(pLastTok, oldCurMacBlk)
            if (*pLastTok == (TsTok)IDSTNU) *pLastTok = (TsTok)CONCATOP;
            else {
              curTok.tok = (Ttok)*pLastTok;
              curTok.val = (uint)*(pLastTok + 1);
              storeTok();
              *pLastTok = (TsTok)CONCATOP;
              *(pLastTok + 1) = (TsTok)WHITESPACE;}  /* "normal" CONCATOP */
            curTok.tok = ENDARG1CONC;}
          else {
            lastTokIsConc = False;
            pLastTok = pMacSto;
            oldCurMacBlk = curMacBlk;
            if (curTok.tok == QUOTEOP) {
              if (NxtTok()!=IDENT || !isMacPar()) err0(NotMacParForQuo);
              else {curTok.tok = QUOTEOP; curTok.val = curTok.ptrSem->noPar
						; curTok.ptrSem->used = True;}}}
          storeTok();
        } while (NxtTok() != ENDDIR);
        if (lastTokIsConc) {err0(ConcOprIlgLast); curTok.tok = SCOL
								  ; storeTok();}
        if (dpragNst != 0) {err0(EndDPragLineExptd); curTok.tok =
							  ENDDPRAG; storeTok();}
        curTok.tok = ENDDIR;}
      storeTok();
      pMacSto--;	/* only one useful TsTok for ENDDIR */
      exitBlock();}
    break;
  case ErrorHCode:
    checkDir(ConvTname("\5\0error"));
    err0(ErrorDir);
    skipRestOfLinePhase4();
  case IncludeHCode:
    checkDir(ConvTname("\7\0include"));
/*    macroExpand = True;*/
    ilgInclArgSeen = False;
    posNameZone = sizNameZone = 0;
    if (NxtTok()==ORDEROP && (TkCmp)curTok.val==LT || curTok.tok==CSTST) {
      Tchar c;
      static Tchar cArr[] = "\0";

      cArr[0] = (curTok.tok == CSTST)? (incldSysFile = False, '"') :
						     (incldSysFile = True, '>');
      while ((c = (Tchar)NxtChar()) != cArr[0]) {
        if (c=='\n' || c==EndCh && fileClosed) {errWS(UnFnshFileName, cArr)
							     ; backUp(); break;}
        storeFileName(c);}}
#ifdef VMS
    else if (curTok.tok == IDENT) {
      int lgtId = (int)*curTok.name;
      Tstring ptr = (Tstring)curTok.name + LgtHdrId;

      do storeFileName(*ptr++); while (--lgtId != 0);
      if (! sysHdrFile) goto ilgArgL;;
      storeFileName('.'); storeFileName('h');
      incldSysFile = True;}
    else
ilgArgL:
#else
    else
#endif
    {
      err0(IlgInclArg);
      free(begNameZone);
      begNameZone = NULL;}
    if (begNameZone != NULL) storeFileName('\0');
    GetNxtTok();
    cmpsgHdr = Found(CMPSGHDR);
    break;
  case LineHCode:
    checkDir(ConvTname("\4\0line"));
    macroExpand = True;
    if (NxtTok() != CSTNU) err0(IntgrNbExptd);
    else {
      analNumCst(NULL);
      if (curTok.val & FltSeen) err0(IntgrNbExptd);
      else if (! curTok.error) {
        TlineNb newLineNb = (TlineNb)curTok.numVal - 1;

        cumDeltaLineNb += lineNb - newLineNb;
        lineNb = newLineNb;}
      if (NxtTok() == CSTST) {
        Tchar *w;

        MyAlloc(w, initGetStrLit(getLitString()));
        curFileName = w;
        while ((*w++ = getNxtStrLitChar()) != '\0') {}
        exitBlock();}}  /* because of getLitString() */
    break;
  case PragmaHCode:
    checkDir(ConvTname("\6\0pragma"));
    skipRestOfLinePhase4();  /* ignore line */
  case UndefHCode:
    checkDir(ConvTname("\5\0undef"));
    if (NxtTok() != IDENT) errPanic(MacNameExptd, NULL);
    if (pDescCurId==NULL || pDescCurId->nstLvl>=0) {if (! sysAdjHdrFile) errWN(
					 UnknMacro|Warn2|PossErr, curTok.name);}
    else {
      static const struct _s {Tname name; ThCode hc;} permMac[] = {
				{(Tname)"\x8\0__LINE__", 0x59EB6C},
				{(Tname)"\x8\0__FILE__", 0x59A134},
				{(Tname)"\x8\0__DATE__", 0x59792A},
				{(Tname)"\x8\0__TIME__", 0x5A4C97},
				{(Tname)"\x8\0__STDC__", 0x5A56A6}};
      const struct _s *ptr;

      for (ptr = &permMac[0]; ptr < &permMac[NbElt(permMac)]; ptr++)
        if (isSameName(curTok.hCod, curTok.name, ptr->hc, ptr->name)) {errWN(
				     NonUndfnblMac, curTok.name); goto noFreeL;}
      freeMac(pDescCurId);
noFreeL: ;}
    GetNxtTok();
    break;
  default: errPanic(IlgDirName, NULL);}
}
/*~Undef cmpsgHdr */

void expandMac(void)
{
  TdescrId *x = pDescCurId;
  uint argNbP1;
  TsTok **newTabArg;  /* pointer on created argument table */
  Ttok macIdSort = curTok.tok;
  bool flagConcAct, fmeX = (bool)(macIdSort-IDENT)  /* True for FORCEMACEXPx */;
  bool sysMacroFl;

  if (condDirSkip
      || x->pTabArg!=NULL && macActive(fmeX)  /* avoid infinite recursion in
							     macro expansion. */
      || macIdSort==NOMACEXP
      || macIdSort!=FORCEMACEXP1 && concatFl!=CNoBit  /* concatenation */
         && (   !(concatFl&CFme1) && x->nstLvl&(1<<(MPosFlags + MRPosConcFl)) /*
							      concatActivity. */
             || !(concatFl & CActiv)
                && (   concatFl&COpnd1 && frstConcOpnd()
                    || concatFl&COpnd2 && !concArgSeen))
				) goto identNotMacroL;  /* not to be expanded */
  flagConcAct = False;
  sysMacroFl = (x->nstLvl >> (MPosFlags + MRPosSysFl)) & 1;
  if ((argNbP1 = (uint)x->nstLvl & ((1 << MPosFlags) - 1)) == 0)  /* macro
							    without argument. */
    newTabArg = MacActWNoPar;  /* mark macro active */
  else {  /* macro with argument list */
    /* OK now for macro expansion */
    {
      TsTok **pNewTabArg, **endTabArg;
      int savSpcCnt = spaceCount, savOSpCnt = oldSpaceCount;
      TconcMsq savConcFl = concatFl;
      bool savConcArgSeen = concArgSeen;
      TlineNb oldLineNb;

      pNxtMac = x;  /* must be set here (because of case where current macro
		 '#undef'ed on next line and next token only found after...). */
      /* Check if really macro call ( '(' just after) */
      {
        TcharTok w = nxtCharOrMacTok();

        if (w!=(TcharTok)'(' && w!=-(TcharTok)LPAR) {pNxtMac = NULL; goto
							       identNotMacroL;}}
      nLine = lineNb;
      curFileN = curFileName;
      GetNxtTok();  /* skip LPAR */
      MyAlloc(pNewTabArg, argNbP1*sizeof(TsTok *));
      newTabArg = pNewTabArg;
      endTabArg = newTabArg + argNbP1;
      /* Simulate pseudo-macro body to store parameters (so as to easily free
			      the space they use, at end of macro expansion). */
      *pNewTabArg++ = createMacroHeader(PseudoName);
      /* Argument collection */
      argCollect = True;
      gotOutOfBody = True;  /* set here to prevent warning in case 
		      // #define A B C ) // #define B() ... // #define C ( // */
      macroExpand = False;
      concatFl = CNoBit;  /* shield argument collection from possible '##'
							     active operator. */
      oldLineNb = lineNb;
      GetNxtTok();
      gotOutOfBody = False;
      if (curTok.tok != RPAR) {
        int parenCtr = 0;  /* 'nested parenthesis' counter */

endArg:
        if (pNewTabArg == endTabArg) {errMacCall(TooManyMacArg, NULL); goto
								    endArgColl;}
        NewArg;
        if (sysMacroFl && usrLvlParFl) storeMacChunk(SUNDR, NULL, UsrLvlPar); /*
			if parameter comes from user level (from outside system
			macro), note it (in order to allow warning report). */
        for (;; GetNxtTok()) {
          if (lineNb != oldLineNb) {storeMacChunk(SUNDR, NULL, LineChng)
							  ; oldLineNb = lineNb;}
          switch (curTok.tok) {
          case COMMA:
            if (parenCtr == 0) {  /* "comma not protected by nested
								parenthesis." */
              storeMacChunk(ENDPAR, NULL, 0-0);
              if (pNewTabArg != endTabArg) GetNxtTok();
              goto endArg /*~ BackBranch */;}
            break;		  /* else normal token */
          case CONCATOP: /*if (pTopMacStk->prec->pMac!=NULL && pTopMacStk->prec->
			    pArg!=NULL) */{  /* operator coming from macro body */
              flagConcAct = True;
              pCurMac->nstLvl |= 1<<(MPosFlags + MRPosConcFl);}  /* set 'concat
			activity' bit (telling that macro is active anyway,
			because parameters surrounding '##' operator only
			expanded after macro has become active). */
            /*~NoBreak*/
          case ENDARG1CONC: storeTok(); continue;
          case ENDPROG: errMacCall(UnFnshArgList, longToS((long)(pNewTabArg -
					      newTabArg) - 1)); goto endArgColl;
          case FORCEMACEXP:
            if (concatFl==CNoBit && (savConcFl==CNoBit || fmeX ||savConcFl&CFme1
	       )) curTok.tok = FORCEMACEXP1;  /* macro name inside argument =>
					    to be expanded in all cases (barring
					    rescanning / ## operator). */
            /*~NoBreak*/
          case FORCEMACEXP1:
            if (pDescCurId->pTabArg!=NULL && macActive(fmeX)) curTok.tok =
			  NOMACEXP;  /* because of 'rescanning' / ## operator */
            break;
          case IDENT:
            if (parenCtr==0
		&& pDescCurId!=NULL && pDescCurId->nstLvl<0  /* macro */
		&& (savConcFl==CNoBit || fmeX /* see case MY1(MY2 tstTOTO...) */
		    || savConcFl&CActiv)) curTok.tok = ((pDescCurId->pTabArg==
		      NULL || !macActive(fmeX)))? FORCEMACEXP : NOMACEXP;  /* if
				    macro, remember its current active state. */
            break;
          case LPAR: parenCtr++; break;
          case RPAR:
            if (parenCtr == 0) goto endArgColl;
            parenCtr--;
            break;
          case WHITESPACE:  /* incomplete parameter (may happen because of
							      '##' operator). */
            if (gotOutOfBody) {errMac(IncplPar, x); goto endArgColl;}
          /*~ NoDefault */
          }
          storeTok();
          if (concatFl != CNoBit) {
            concArgSeen = True;
            while ((concatFl & COpnd2) && !(concatFl & CParam)) popMacStk();}}}
endArgColl:
      storeMacChunk(ENDPAR, NULL, 0-0);
      if (pNewTabArg != endTabArg) {
        if (pNewTabArg-newTabArg!=1 || argNbP1!=1+1) errMac(MsngMacArg, x); /*
				    case of empty argument for one parameter. */
        do {
          NewArg;
          storeMacChunk(ENDPAR, NULL, 0-0);
        } while (pNewTabArg != endTabArg);}
      macroExpand = True;
      argCollect = False;
      pNxtMac = NULL;  /* allow freeing of macro body storage */
      if (! gotOutOfBody) concatFl = savConcFl;  /* if not got out of
							 caller's macro body. */
      spaceCount = savSpcCnt;  /* restore 'spaceCount' (which may have been
					modified during argument collection). */
      oldSpaceCount = savOSpCnt;      
      concArgSeen = savConcArgSeen;}}
  /* Enter macro */
  pushMacStk();
  if (flagConcAct) pTopMacStk->concAsPar = True;
  if (fmeX) {  /* delayed expansion of macro, that should have been performed
		before calling the other currently called macros, or encoun-
		tering current '##' operator). */
    activityFloor = pTopMacStk;  /* for no false infinite recursion detection */
    /* Get out of reach of possible current '##' operator (but remember that
       inside it, to allow frstConcOpnd() to be called). */
    if (concatFl != CNoBit) concatFl |= CActiv;
    if (macIdSort == FORCEMACEXP1) {pTopMacStk->fme1 = True; if (concatFl !=
						    CNoBit) concatFl |= CFme1;}}
  pTopMacStk->macExpNb = curMacExpNb;
  pTopMacStk->usrLvlPFl = usrLvlParFl;
  sysMacro = sysMacroFl;  /* set 'sysMacro' flag for system macros, to
						prevent warnings inside them. */
  usrLvlParFl = ! sysMacro;
  adjMacro = (x->nstLvl >> (MPosFlags + MRPosAdjFl)) & 1;
  x->pTabArg = newTabArg;  /* note address of created table of pointers on
				     macro arguments, and marks macro active. */
  pCurMac = x;  /* macro becomes current */
  {
    static TmacExpnd ctrMacExp = 0;  /* number of macro expansions */

    curMacExpNb = ++ctrMacExp;}  /* one more macro expansion ... */
  pMacTok = (const TsTok *)x->idName + FullLgt(x->idName); /* skip macro name */
  decrGetTokLvl++;
  GetNxtTok();
  decrGetTokLvl--;
  return;
identNotMacroL:
  {
    register TdescrId *pCurId;
    register Tname adBegName;
    ThCode hCode;

    curTok.name = adBegName = pDescCurId->idName;
    curTok.hCod = hCode = hCodFct(adBegName);
    SearchSym(adBegName, MacroNotVisible)
    curTok.ptrSem = (pCurId == NULL)? NULL : (pDescCurId = pCurId, pCurId->
								      pIdSeman);
    curTok.tok = IDENT;}
}

static void getTokFromMac(void)
{
  getTokLvl++;
  for (;;) {
    curTok.tok = (Ttok)*pMacTok++;
    curTok.val = (uint)*pMacTok++;
    /*~zif IncrPMacT != 2 "Pb"*/
    if (curTok.tok < BegDPragTok) break;
    switch (curTok.tok) {
      case CONCATOP:  /* '##' operator */
        pushMacStk();
        pTopMacStk->pArg = NULL;  /* marks that frame is a ## frame */
        concatFl = COpnd1;  /* ready for 1st argument of operator */
        concArgSeen = False;
        if (curTok.val != (uint)WHITESPACE) {  /* if CONCATOP condensed with
								   IDENT etc. */
          if (argCollect) concatFl = COpnd1 | CISN;  /* to not loose ##
						  operator (see storeTok() ). */
          goto idStNuL;}
        goto endConcOpL;
      case ENDARG1CONC:
        concatFl = COpnd2;
        concArgSeen = False;
endConcOpL:
        if (argCollect) break;  /* argument collection: save operator */
        continue;  /* get 1st/2nd operand */
      case ENDBLK: pMacTok -= IncrPMacT; GoNxtBlk(pMacTok); continue;
      case ENDDIR:  /* end of macro body */
        if (argCollect && !gotOutOfBody) {
          if (pNxtMac != NULL)  /* may be null after several levels of
								   'peeping'. */
            errMacCall(ArgCollGetsOutMacBody | Warn3, nameToS(pCurMac->idName));
          concatFl = (TconcMsq)1;  /* non zero, to enable popMacStk() to get
						    back to stacked concatFl. */
          gotOutOfBody = True;}  /* to prevent further errors */
        exitMacro();
        goto endParL;
      case ENDPAR:
        exitParam();
endParL:
        popMacStk();
        if (concatFl!=CNoBit && pTopMacStk->pArg==NULL) {  /* back to a '##'
								       frame. */
          if (!concArgSeen || argCollect && gotOutOfBody) { 
            /* No token generated by parameter/macro operand of a '##' operator;
               too late for concatenating previous token (if 1st operand) =>
               generate an empty token... Generate also an empty token for 2nd
	       operand, for symmetry reasons, though should be able to concate-
	       nate to next token. */
            curTok.tok = WHITESPACE;
            break;}
          if ((concatFl & COpnd2) && !(concatFl & CParam)) popMacStk();}  /* ex-
				iting from a '##' operator whose 2nd operand
				was a parameter of length > 1. */
        /* Get next token */
        if (pTopMacStk != NULL) continue;
        if (peepTok) {curTok.tok = WHITESPACE; break;}
        pCurGNT = &getTokFromTxt;
        pNxtCh = &nxtChFromTxt;
        getTokLvl--;
        GetNxtTok();
        return;
      case IDSTNU:
idStNuL:
        switch (curTok.tok = (Ttok)curTok.val) {
        case CSTNU:
          pCurMacNb = pMacTok;
          curTok.val = 0;
          frstChOfNb = (char)nxtChFromMac();
          break;
        case FORCEMACEXP: case FORCEMACEXP1: case NOMACEXP:
          memcpy(&pDescCurId, pMacTok, sizeof(pDescCurId));
          pMacTok += sizeof(pDescCurId);
          if (macroExpand) {
            expandMac();
            if (curTok.tok == IDENT) goto tstUndefL;}
          break;
        case IDENT: {
            register TdescrId *pCurId;
            register Tname adBegName;
            ThCode hCode;

            curTok.name = adBegName = (Tname)pMacTok;
            pMacTok += FullLgt(adBegName);
            curTok.hCod = hCode = hCodFct(adBegName);
            SearchSym(adBegName, MacroVisible)
            if ((pDescCurId = pCurId) == NULL) curTok.ptrSem = NULL;
            else if (pCurId->nstLvl < 0) {  /* macro (keyword not possible
								       here). */
              if (macroExpand) {expandMac(); if (curTok.tok != IDENT) break;}
              else curTok.ptrSem = NULL;}
            else curTok.ptrSem = pCurId->pIdSeman;}
tstUndefL:
          if (curTok.ptrSem!=NULL && curTok.ptrSem->undef && getTokLvl==1)
            errWN(UndfndId|Warn1|Rdbl, curTok.name);
          break;
        /*~NoDefault*/}
        break;
      case MACPAR: case MACPAR1:
        IncrMacParCtr;
        pushMacStk();
        pTopMacStk->insideMUMP = insideMultUsedMacPar;
        insideMultUsedMacPar = curTok.tok == MACPAR1;
        pTopMacStk->pMac = NULL;  /* to mark frame as parameter frame */
        activityFloor = pTopMacStk;   /* for infinite recursion detection
								     purpose. */
        pMacTok = *(pCurMac->pTabArg + curTok.val);  /* beginning of
						  parameter string of tokens. */
        continue;
      case SUNDR:
        if (argCollect) break;  /* to be kept uninterpreted in that case */
        if (curTok.val == LineChng) lastSCOLline = 0;
        else /* UsrLvlPar */ {
          sysMacro = False;  /* warnings just as at main level */
          usrLvlParFl = True;}
        continue;
      case SKIPTO: SkipTo(pMacTok); continue;
      default :
        if (! peepTok) {
          if (InsideInterval(curTok.tok, BegDPragTok, EndDPragTok)) {  /* in-
								cludes DPRAG! */
            uint localDecrGetTokLvl = decrGetTokLvl;

            if ((getTokLvl -= localDecrGetTokLvl)==0 && listTok) storeTokTxt(
							    dpName(curTok.tok));
            if (curTok.tok == DPRAG) {dpragNst++; curTok = manageDPrag();}
            getTokLvl += localDecrGetTokLvl;
            if (curTok.tok == WHITESPACE) continue;}
          else if (curTok.tok == QUOTEOP) {
            /* Decode parameter tokens, and store generated string in string
               storage, pending better macro processing. */
            const TsTok *savPMT = pMacTok;
            TlitString generStr;

            enterBlock();  /* to be able to reclaim string storage */
            generStr = ptrFreeIdSpace();
            {
              bool savME = macroExpand;
              TmacStkElt *initialPTMS = pTopMacStk;

              for (pMacTok = *(pCurMac->pTabArg + curTok.val);;) {
                void (*savPSCF)(char);

                pMacTok = manageJmpTok(pMacTok);
                if (InsideInterval((Ttok)*pMacTok, ENDDIR, ENDPAR)) {
                  if ((Ttok)*pMacTok == ENDDIR) exitMacro();
                  else {
                    if (pTopMacStk == initialPTMS) break;  /* end of '#'
								    argument. */
                    exitParam();}
                  popMacStk();
                  continue;}
                macroExpand = ((Ttok)*pMacTok==IDSTNU && (Ttok)*(pMacTok + 1)
							       == FORCEMACEXP1);
                GetNxtTok();
                savPSCF = pStoreChFct;
                pStoreChFct = &storeStrCh;
                storeTokSource();
                if (InsideInterval(curTok.tok, CSTNU, CSTST))
                  if (curTok.tok - CSTNU == 0) analNumCst(pStoreChFct);
                  else {analStrCst(&storeTokTxtRepr); storeStrCh('"');}
                pStoreChFct= savPSCF;}
              storeStrCh('\0');
              macroExpand = savME;}
            {
              TsTok w;  /* not initialized, since value later overwritten. */

              pseudoMacQuo = createMacroHeader(PseudoName);  /* pseudo-macro to
		      hold generated string (for memory reclaiming purposes). */
              storeMacChunk(IDSTNU, /*~Init w*/&w, SizOfCharBlkLgtFld);}  /* so
		    that length of 1st sub-block in same macro storage block. */
            {
              TsTok *pw;

              pMacTok = pw = pMacSto - (IncrPMacT + SizOfCharBlkLgtFld);
						   /* points on created token */
              *(pw + 1) = (TsTok)CSTST;  /* fill IDSTNU TsTok */
            }
            pLgtCharSubBlk = pMacSto - SizOfCharBlkLgtFld;
            {
              /* Create (macro) string token */
              char w;

              initGetStrLit(generStr);
              while ((w = getNxtStrLitChar()) != '\0') storeMacChar(w);}
            exitBlock();  /* reclaim temporary string storage */
            *pLgtCharSubBlk = (TsTok)(pMacSto - pLgtCharSubBlk);  /* number
				    of characters (+1) in (ending) sub-block. */
            storeMacChunk((Ttok)0, NULL, 0); pMacSto--;  /* end marker = 0
						      (only one byte needed). */
            savPMTQuo = savPMT;  /* also, flag to tell 'inside quoteOp' (if
								   non NULL). */
            if (argCollect) {memcpy(pseudoMacQuo, &pMacSto, sizeof(void *))
				 ;/* fill SKIPTO address*/ pseudoMacQuo = NULL;}
            continue;}}}  /* get created token */
    break;}
  if (concatFl != CNoBit) checkConc();  /* check if first operand to '##'
								    operator. */
  if (--getTokLvl==0 && listTok && !peepTok) storeTokSource();
}

static void checkConc(void)
{
  if (! argCollect)
    while (concatFl!=CNoBit && (concArgSeen = True, frstConcOpnd())) {
      TmacStkElt *ptrConcFrameBef = frstConcFrame();

      manageConc();
      /* Getting out of this '##' operator */
      /* First, exit of possible exhausted parameter/macro body (so then, '##'
         frame will be removed from stack, and call to 'frstConcFrame' for pos-
         sible next call to 'manageConc' will be correct). */
      while ((concatFl & COpnd2) && (concatFl & CParam)) {
        Ttok w;

        if (! InsideInterval((w = peepNxtMacTok(False)), ENDDIR, ENDPAR)) break;
        if (w == ENDDIR) exitMacro();
        else exitParam();
        popMacStk();}
      /* Else, if 2nd operand not yet exhausted, exiting reach of '##' opera-
				     tor done when 2nd operand get exhausted. */
      {
        TmacStkElt *ptrCurConcFrame;

        if ((concatFl & COpnd2) && !(concatFl  & CParam)) popMacStk();
        else if (ptrConcFrameBef != (ptrCurConcFrame = frstConcFrame())) {  /*
		  concatenating at an outer level => remove inner '##' frame. */
         /* See case MZA(MZA(NT,), MZA(, NT)), with:
            #define MZA(x, y) MZ(x, y)
            #define MZ(x, y) x##V##y
            #define V 22 33
            #define NT */
          register TmacStkElt *w = pTopMacStk;

          while (w->prec != ptrCurConcFrame) w = w->prec;
          w->prec = freeMacStkElt(w->prec);}}
      if ((ptrConcFrameBef->concFl==CNoBit || (ptrConcFrameBef->concFl & CActiv))
         /* outside of a possible chain of '##' operators */ && curTok.tok==
		IDENT && pDescCurId!=NULL && pDescCurId->nstLvl<0) expandMac();}
}


/******************************************************************************/
/*                                                                            */
/*                                  UTILITIES                                 */
/*                                                                            */
/******************************************************************************/

bool aloneInNoParMac(bool isNb)
{
  const TsTok *pBegMac;

  if (pTopMacStk==NULL || !isNb && pTopMacStk->prec!=NULL /* not level 1 macro,
			    that is, ReallyInsideMacro == True */) return False;
  if (pCurMac->nstLvl & 1<<(MPosFlags + MRPosSysFl)) return True;  /* system
								       macro. */
  if (pCurMac->pTabArg!=MacActWNoPar) return False;
  pBegMac = manageJmpTok((const TsTok *)pCurMac->idName + FullLgt(pCurMac->
								       idName));
  if (isNb && (Ttok)*pBegMac==ADDOP) pBegMac = manageJmpTok(pBegMac +
					   IncrPMacT);  /* skip possible sign */
  return (pBegMac==((isNb)? pCurMacNb : pMacTok)-IncrPMacT && peepNxtMacTok(
								False)==ENDDIR);
}

void checkCondStkAndDeleteMacros(void)
{
  if (pTopCondStk != NULL) {
    err0(MsngEndif);
    do popCondStk(); while (pTopCondStk != NULL);}
  /* Free (possible) remaining macro zones */
  {
    TdescrId **w = &symTabHeads[0], *w1, *w2;

    do {
      for (w1 = *w++; w1 != NULL; w1 = w2) {
        w2 = w1->next;
        if ((w1->nstLvl)+1 < 0) freeMac(w1);}
    } while (w <= &symTabHeads[SizeSymTab - 1]);}
}

static void checkDir(Tname dirName)
{
  if (memcmp(curTok.name, dirName, FullLgt(dirName)) != 0) errPanic(IlgDirName,
									  NULL);
}

bool checkSColAtEndMac(void)
{
  if (peepNxtMacTok(False) != ENDDIR) return False;
  errMac(ExtraSColAtEndMac, pCurMac);
  return True;
}

bool checkSpeFct(void)
{
  enum _index{BegTab, Defined = BegTab /*~SameValue*/, Member, Extent, Index,
    SamType, CDefined, EndTab};
  static const struct _s {ThCode fctHCode; Tname fctName; void (*fctPtr)(void);}
				     speFctTab[/*~ IndexType enum _index */] = {
	{DefinedHCode, (Tname)"\7\0defined", &procDefined}	/*~ zif
		  __index!=Defined "Array 'speFctTab': element out of order" */,
	{MemberHCode, (Tname)"\x8\0__member", &procMember1}/*~ zif
		   __index!=Member "Array 'speFctTab': element out of order" */,
	{ExtentHCode, (Tname)"\x8\0__extent", &procExtent}	/*~ zif
		   __index!=Extent "Array 'speFctTab': element out of order" */,
	{IndexHCode, (Tname)"\7\0__index", &procIndex}		/*~ zif
		    __index!=Index "Array 'speFctTab': element out of order" */,
	{SamTypeHCode, (Tname)"\xA\0__sametype", &procSameType}/*~ zif
		  __index!=SamType "Array 'speFctTab': element out of order" */,
	{CDfndHCode, (Tname)"\x8\0cdefined", &procCDefined}	/*~ zif
		 __index!=CDefined "Array 'speFctTab': element out of order" */
	/*~ zif __index != EndTab - 1
				 "Array 'speFctTab': not fully initialized" */};
  register const struct _s *ptrTab;

  for (ptrTab = &speFctTab[BegTab]; ptrTab <= &speFctTab[EndTab - 1];ptrTab++) {
    if (isSameName(curTok.hCod, curTok.name, ptrTab->fctHCode,
							     ptrTab->fctName)) {
      bool rParExptd, savME = macroExpand;

      if (ptrTab == &speFctTab[Defined]) macroExpand = False;
      else if (ptrTab == &speFctTab[CDefined]) {
        if (! adjustFile) return False;
        macroExpand = False;}
      else if (! zifExp) return False;
      if (rParExptd = (NxtTok() == LPAR)) GetNxtTok();
      if (ptrTab==&speFctTab[Index] || ptrTab==&speFctTab[SamType]) (
							     *ptrTab->fctPtr)();
      else if (ptrTab==&speFctTab[Member] && curTok.tok!=IDENT) procMember2();
      else {
        if (curTok.tok != IDENT) err0(IdExptd);
        else {
          (*ptrTab->fctPtr)();
          if (! rParExptd) macroExpand = savME;
          GetNxtTok();}}
      macroExpand = savME;
      if (rParExptd && !Found(RPAR)) err0(RParExptd);
      return True;}}
  return False;
}

static TsTok *createMacroHeader(Tname x)
/* Returns address of created macro header */
{
  TsTok */*~IndexType TsTok*/ resul;

  if (argCollect) {
    /* No need to create a macro header if inside a macro body (used space will
       be freed when body is freed). */
    storeMacChunk(SKIPTO, (const TsTok *)x, sizeof(void *));
    resul = pMacSto - sizeof(void *);}  /* remember address of address field of
				   SKIPTO (to be able to fill it when known). */
  else {
    storeMacChunk((Ttok)0, (const TsTok *)x, FullLgt(x));  /* store macro
					header and name in a contiguous area. */
    /*~zif LgtMacHdr != IncrPMacT "We have a big problem here" */
    resul = pMacSto - FullLgt(x);
    /* Chain new zone to zones already held in current macro block, in such a
       manner that higher address zones are found earlier in chain (cf
       'freeMacSto()'. */
    *(resul - DispChain) = *pHeadChain;
    *(resul - DispBegSto) = *pHeadChain = (TsTok)(resul - &(curMacBlk->tokSto
									 [0]));}
  return resul;
}

Tstring curMacDFName(void)
{
  return pCurMac->dFileName;
}

bool embdMacro(void)
{
  return pTopMacStk!=NULL && pTopMacStk->prec!=NULL;
}

static void errMac(Terr n, const TdescrId *x)
{
  errWFName(n, (TlineNb)(((ulong)x->nstLvl^LSIGN_BIT) >> MPosLineNb),
					   x->dFileName, x->idName, NULL, NULL);
}

static void errMacCall(Terr n, Tstring y)
{
  errWFName(n, nLine, curFileN, pNxtMac->idName, y, NULL);
}

static void errMacDef(Terr n, TdescrId *pDescMacId, Tstring txt)
							       /*~NeverReturns*/
/* Free storage used by the (aborted) macro name, and skip remaining of line */
{
  TnameBuf buf;

  freeMac(pDescMacId);
  bufNameToS(pDescMacId->idName, buf);
  errWSS(n, buf, txt);
  exitBlock();  /* may cause error messages */
  SkipRestOfLine;  /* errPanic() not used, to get correct sequencing
							   of error messages. */
}

static bool evalIfExp(void)
{
  TresulExp saveCExp = cExp, saveLExp = lExp;
  bool resul;

  ifDirExp = macroExpand = True;
  GetNxtTok();
  resul = correctExprN(NULL, Bool, True, "#if", True) && cExp.uVal!=0;
  ifDirExp = False;
  cExp = saveCExp; lExp = saveLExp;
  return resul;
}

static void exitMacro(void)
{
  if (! pTopMacStk->noFreeArg) {
    if (pCurMac->pTabArg != MacActWNoPar) FreeArg(pCurMac->pTabArg)  /* if
				 macro with parameters, free parameter table. */
    usrLvlParFl = pTopMacStk->usrLvlPFl;
    sysMacro = (pTopMacStk->prec == NULL)? False : ((pTopMacStk->pMac->nstLvl >>
						 (MPosFlags + MRPosSysFl)) & 1);
    adjMacro = (pTopMacStk->prec == NULL)? False : ((pTopMacStk->pMac->nstLvl >>
						 (MPosFlags + MRPosAdjFl)) & 1);
    if (pTopMacStk->concAsPar) {
      pTopMacStk->concAsPar = False;
      pTopMacStk->pMac->nstLvl &= ~(1<<(MPosFlags + MRPosConcFl));}  /* reset
						      'concat activity' flag. */
    pCurMac->pTabArg = NULL;  /* marks macro inactive */
    if (pTopMacStk->prec != NULL) pTopMacStk->pMac->pTabArg = pTopMacStk->pArg;
    curMacExpNb = pTopMacStk->macExpNb;
    if (curMacExpNb < begMacExpNb) lastSCOLline = 0;}
}

static void exitParam(void)
{
  IncrMacParCtr;  /* so that uninterrupted run of tokens in parameter
								  detectable. */
  insideMultUsedMacPar = pTopMacStk->insideMUMP;
  sysMacro = (pCurMac->nstLvl >> (MPosFlags + MRPosSysFl)) & 1;
  usrLvlParFl = ! sysMacro;
}

static void freeMac(TdescrId *x)
{
  if (x==pNxtMac /* deleting currently expanding macro: case
	   // #define M(x) ... // ... // M // #undef M // (param_of_M) ... // */
      || argCollect) /* in case of deleting macro passed as parameter: case
				// #define M(x) ... // #define M1 ... //
				... // 	M(M1 // #undef M // #undef M1 // ) // */
    *((TnameNC/*~OddCast*/)x->idName + DispNSId) = (TnameAtom)DeltdMac;  /*
					then just render macro invisible to
					SearchSym; will be freed by checks(). */
  else {
    /* Free storage used by macro body */
    freeMacSto((TsTok * /*~OddCast*/)x->idName);
    /* Remove macro element from symbol table chaining */
    x->prec->next = x->next;
    if (x->next != NULL) x->next->prec = x->prec;
    /* Free element */
    (void)freeDescrIdElt(x);}
}

static void freeMacSto(TsTok *macName)
{
  TmacBlk *firstBlk, *nxtBlk, *precBlk;
  register TmacBlk *curBlk;

  if (macName == NULL) return;  /* see argCollect effect in '#' operator */
  firstBlk = (TmacBlk * /*~OddCast*/)((ubyte *)macName - *(macName - DispBegSto)
		 - Offset(TmacBlk, tokSto[0]));  /* compute address of block
					where macro begins ('initial' block). */
  *(macName - DispBegSto) = 0;  /* marks zone free */
  if (macName != &(firstBlk->tokSto[0]) + firstBlk->headChain) return;  /* if
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
    if (freeMacBlk == NULL) freeMacBlk = curBlk;
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
    if (w1!=0		/* found still used macro header, */
        || ((precBlk = firstBlk->prec)!=(TmacBlk * /*~OddCast*/)&headChainMacBlk
         && (precBlk->headChain==0  /* (if so, preceding zone can't be free) */
          || precBlk->tokSto[precBlk->headChain - DispBegSto]!=0  /* or
					       preceding zone not free. */ ))) {
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
  } while ((firstBlk = precBlk) != (TmacBlk * /*~OddCast*/)&headChainMacBlk);
  pMacSto = endCurMacSto = NULL; /* return to initial state */
}

static TmacStkElt *frstConcFrame(void)
/* Returns address of first '##' frame found in stack (used to be able to
   manage cases such that a##x##b (with x => 'u v') => 'au' 'vb'. */
{
  register TmacStkElt *w = pTopMacStk;

  while (w->pArg != NULL) w = w->prec;
  return w;
}

static bool frstConcOpnd(void)
/* Answers True if current token is last token of first operand to a '##'
   operator (remember that cat(a b c, d e f) => a b cd e f). */
{
  if (! (concatFl & COpnd1)) return False;
  /* for case M(x##y), with x => a##b and y => c##d */
  {
    const TsTok *savPMacTok = pMacTok;
    register TmacStkElt *w = pTopMacStk;
    bool firstTurn = True;
    Ttok nxtTok;

    while (InsideInterval((nxtTok= peepNxtMacTok(firstTurn)), ENDDIR, ENDPAR)) {
      /* Go to caller */
      while (w->pArg == NULL) w = w->prec;  /* skip '##' frames */
      pMacTok = w->pMacTok;
      w = w->prec;
      firstTurn = False;}
    pMacTok = savPMacTok;
    return nxtTok == ENDARG1CONC;}
}

static bool isMacPar(void)
{
  return pDescCurId!=NULL && ((curTok.ptrSem==NULL)? /* macro name: hiding
	      parameter name? */ (curTok.ptrSem = searchSymTabHC(curTok.name,
		   curTok.hCod))!=NULL : True) && curTok.ptrSem->kind==ParamMac;
}

static bool macActive(bool fmeX)
{
  /* See if irrelevant active status, due to evaluation of macro parameter
        after macro calling. */
  register TmacStkElt *wpStk = pTopMacStk;
  register const TmacStkElt *locActFloor = activityFloor;

  if (fmeX || curTok.tok!=IDENT) {
    /* If at end of macro body or parameter, go back to previous activity level
       (to find M active in case M(M4(M)), where M as parameter has gotten to
       FORCEMACEXP1 status). */
    const TsTok *wpTok = pMacTok;

    while (InsideInterval((Ttok)*wpTok, ENDBLK, ENDPAR) && wpStk->prec!=NULL) {
      if ((Ttok)*wpTok == ENDBLK) GoNxtBlk(wpTok);
      else {
        wpTok = wpStk->pMacTok;
        locActFloor = wpStk->actFloor;
        while ((wpStk = wpStk->prec)->pArg == NULL) {
          if (wpStk->prec == NULL) break;}}}} /*skip all '##' frames */
  else {  /* go back to activity floor of last seen FORCEMACEXPx macro call
				(cf F(G(F(a))) with F(x)=>x and G(x)=>F(x) ). */
    while (locActFloor != NULL) {
      if (wpStk->pMac != NULL) {  /* not parameter frame */
        if (locActFloor != wpStk->actFloor) {  /* 'FMEx' frame */
          if (concatFl==CNoBit || concatFl&CActiv || wpStk->fme1) break;}} /*
			inside reach of '##' operator, macro expansion only
			done after parameter insertion, that is, when entered
			macro has gone to "active" status (rescanning). */
      locActFloor = wpStk->actFloor;
      wpStk = wpStk->prec;}}
  if (pTopMacStk == locActFloor) return False;
  if (pDescCurId == pCurMac) return True;
  wpStk = pTopMacStk;
  do {  /* search whether macro called before activity floor */
    if (wpStk->pMac == pDescCurId) return True;  /* macro really active */
  } while ((wpStk = wpStk->prec) != locActFloor);
  return False;
}

static const TsTok *manageJmpTok(const TsTok *x)
{
  for (;;) {
    switch ((Ttok)*x) {
      case ENDBLK: GoNxtBlk(x); continue;
      case SKIPTO: x += IncrPMacT; SkipTo(x); continue;
      default: return x;}}
}

TcharStream nxtChFromMac(void)
{
  for (;;) {
    if (endCurCharSubBlk == NULL) {
      endCurCharSubBlk = pMacTok + *pMacTok;
      if (*pMacTok != 0) pMacTok++;}
    if (pMacTok != endCurCharSubBlk) break;
    endCurCharSubBlk = NULL;
    if (*pMacTok == (TsTok)ENDBLK) GoNxtBlk(pMacTok);
    else {
      pMacTok++;  /* skip ending 0 */
      return -1;}}  /* end of character stream */
  return (TcharStream)*pMacTok++;
}

TcharTok nxtCharOrMacTok(void)
/* peeps next character (if outside macro, >0), or next macro token (<0).
   BEWARE, next source character may be on next line, with directive "white
   lines" in between.
   Current token CANNOT be CSTNU or CSTST (except if already analysed). */
{
  if (InsideInterval(curTok.tok, CSTNU, CSTST)) {
    if (frstChOfNb!='\0' || curTok.tok==CSTST) sysErr(ExCod6);}
  if (pTopMacStk == NULL) return (TcharTok)peepNxtNonBlChFromTxt();
  /* Get (non-destructively) next macro token */
  if (! (concatFl & COpnd1)) {
    pMacTok = manageJmpTok(pMacTok);
    if (! InsideInterval((Ttok)*pMacTok, BegServTok, EndServTok - 1)) return
							   -(TcharTok)*pMacTok;}
  if (frstConcOpnd()) return -(TcharTok)ENDARG1CONC;
  {
    const TmacStkElt *savActiFlo = activityFloor;
    TmacStkElt *savPTopMacStk;
    TdescrId *savPCurMac = pCurMac, *savPNxtMac = pNxtMac;
    bool savConcArgS = concArgSeen;
    const TsTok *savPMacTok = pMacTok;
    TconcMsq savConcFl = concatFl;
    bool savPeepTok = peepTok;
    TdescrId *savPDCI = pDescCurId;
    TvalTok savCurTok = curTok;
    TcharTok result;
    register TmacStkElt *wpStk, *wp;
    TmacLvl savMacLvl = macLvl;
    TmacParCtr savMacParCtr = macParCtr;
    bool savSysMacro = sysMacro, savAdjMacro = adjMacro;

    /* Save current context, in order to restore it after getting next token */
    for (wpStk = savPTopMacStk = pTopMacStk, pTopMacStk = NULL; wpStk != NULL;
				   wpStk = wpStk->prec) {  /* duplicate stack */
      register TmacStkElt *w = allocMacStkElt();

      *w = *wpStk;
      w->noFreeArg = True; /* do not free argument table (for not original) */
      if (pTopMacStk == NULL) pTopMacStk = w; else wp->prec = w;
      wp = w;}
    peepTok = True;
    GetNxtTok();
    peepTok = savPeepTok;
    result = (pTopMacStk == NULL)? (TcharTok)peepNxtNonBlChFromTxt() :
							  -(TcharTok)curTok.tok;
    if (curTok.tok == CSTNU) frstChOfNb = '\0';
    /* Free temporary stack used while 'peeping' */
    while (pTopMacStk != NULL) {
      if (pTopMacStk->pArg != NULL)  /* not '##' frame */
        if (pTopMacStk->pMac == NULL) exitParam();
        else exitMacro();
      popMacStk();}
    sysMacro = savSysMacro; adjMacro = savAdjMacro;
    macParCtr = savMacParCtr;
    macLvl = savMacLvl;
    pTopMacStk = savPTopMacStk;
    activityFloor = savActiFlo;
    pCurMac = savPCurMac;
    pNxtMac = savPNxtMac;
    pMacTok = savPMacTok;
    concatFl = savConcFl;
    concArgSeen = savConcArgS;
    curTok = savCurTok;
    pDescCurId = savPDCI;
    return result;}
}

static Ttok peepNxtMacTok(bool mayBeInsideTok)
{
  const TsTok *const savPMacTok = pMacTok;

  if (mayBeInsideTok && InsideInterval(curTok.tok, CSTNU, CSTST) && pNxtCh==
								&nxtChFromMac) {
    const TsTok *const savEndCCSB = endCurCharSubBlk;

    while (nxtChFromMac() >= 0) {};  /* go to next token */
    endCurCharSubBlk = savEndCCSB;}
  {
    Ttok w = (Ttok)*manageJmpTok(pMacTok);

    pMacTok = savPMacTok;
    return w;}
}

static void popCondStk(void)
{
  curIfArmActiv = pTopCondStk->curIfArmActiv;
  trueIfPartSeen = pTopCondStk->trueIfPartSeen;
  elseSeen = pTopCondStk->elseSeen;
  activLvl = pTopCondStk->activLvl;
  oldSpaceCount = pTopCondStk->savOldSpCnt;
  pTopCondStk = freeCondStkElt(pTopCondStk);
}

static void popMacStk(void)
{
  register TmacStkElt *pTMS = pTopMacStk;

  if (pTMS->prec!=NULL && pTMS->pArg!=NULL) {  /* not exiting from a '##'
								    operator. */
    if (pTMS->pMac != NULL) pCurMac = pTMS->pMac;  /* not exiting
							      from parameter. */
    pMacTok = pTMS->pMacTok;
    activityFloor = pTMS->actFloor;}
  if (concatFl != CNoBit) concatFl = pTMS->concFl;
  pTopMacStk = freeMacStkElt(pTMS);
  macLvl--;
}  

static void procCDefined(void)
{
  cExp.uVal = (TcalcU)(pDescCurId!=NULL && pDescCurId->nstLvl>=0);
  cExp.type = &boolCstTypeElt;
}

static void procDefined(void)
{
  cExp.uVal = (TcalcU)(pDescCurId!=NULL && pDescCurId->nstLvl<0);
  cExp.type = &boolCstTypeElt;
}

static void pushMacStk(void)
{
  register TmacStkElt *w = allocMacStkElt();

  if (pTopMacStk == NULL) {
    pCurGNT = &getTokFromMac;
    pNxtCh = &nxtChFromMac;
    w->pMac = QuasiNULLval(TdescrId *);}  /* non NULL (see CONCATOP in
							       expandMac() ). */
  else {
    w->pMacTok = pMacTok;
    w->actFloor = activityFloor;
    w->pMac = pCurMac;
    w->pArg = pCurMac->pTabArg;}
  w->infoM.u11.bid2 = 0;  /* reset flags */
  w->concFl = concatFl;
  if (concatFl != CNoBit)
    if (! (concatFl & CParam)) concatFl |= CParam;
    else concatFl |= CActiv;
  w->prec = pTopMacStk;
  pTopMacStk = w;
  macLvl++;
}

void restoQuoState(void)
{
  if (savPMTQuo != NULL) {pMacTok = savPMTQuo; savPMTQuo = NULL; freeMacSto(
								 pseudoMacQuo);}
}

static void skipRestOfLinePhase4(void) /*~NeverReturns*/
{
  condDirSkip = True;  /* to prevent detection of syntax errors that are not
					       errors in preprocessor-tokens. */
  SkipRestOfLine;
}

static void storeFileName(Tchar x)
{
  if (isspace(x)) {
    if (! ilgInclArgSeen) {
      err0(IlgInclArg|Warn3|Rdbl|PossErr);
      ilgInclArgSeen = True;}}
  else {
    if (posNameZone == sizNameZone) {  /* buffer full */
      sizNameZone += FILENAME_MAX + 1;
      if ((begNameZone = realloc(begNameZone, sizNameZone)) == NULL) sysErr(
							  errTxt[RanOutOfMem]);}
    begNameZone[posNameZone++] = x;}
}

static void storeMacChar(char x)
{
  if (pMacSto == endCurMacSto) {
    *pLgtCharSubBlk = (TsTok)(pMacSto - pLgtCharSubBlk);  /* number of character
						    (+1) in filled sub-block. */
    storeMacChunk((Ttok)0, NULL, 0);  /* to get another macro storage block */
    pMacSto -= IncrPMacT - SizOfCharBlkLgtFld;  /* to remove fake 0s */
    pLgtCharSubBlk = pMacSto - SizOfCharBlkLgtFld;}
  *pMacSto++ = (TsTok)x;
}

static void storeMacChunk(Ttok tok, const TsTok *x, size_t size)
/* 'size' is the size of the information pointed by 'x'. Stores 'tok', 'size'
   as TsToks, followed by (if 'x' non NULL) 'size' TsToks. The whole chunk is
   stored contiguously. */
{
  register TsTok *ptr = pMacSto;

  if (((x == NULL)? ptr : ptr + size) + IncrPMacT > endCurMacSto) {  /* not
			enough room at end of current block (there must always
			stay at least one slot for eventual ENDBLK). */
    TmacBlk *nxtBlk;

    if (freeMacBlk != NULL) {nxtBlk = freeMacBlk; freeMacBlk = NULL;}
    else MyAlloc(nxtBlk, sizeof(TmacBlk));
    /*~ zif ((ptrdiff_t)SizeMacroStorageChunk < IncrPMacT + (LgtHdrId +
	MaxLgtId) + IncrPMacT) "SizeMacroStorageBlk too small" */ /* a macro
	storage chunk must be capable of holding at once: IDENT, identifier
							       Tname, ENDBLK. */
    ctrMSB++;
    if (ptr == NULL) curMacBlk = (TmacBlk * /*~OddCast*/)&headChainMacBlk;
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
  TsTok *oldPMS = pMacSto;
  TmacBlk *oldCMB = curMacBlk;

  switch (curTok.tok) {
  case IDENT:
    if (isMacPar()) {  /* macro parameter */
      storeMacChunk((!curTok.ptrSem->reallyUsed || curTok.ptrSem->usedMorThOnce
		 || dpragNst!=0)? MACPAR : MACPAR1, NULL, curTok.ptrSem->noPar);
      if (! curTok.ptrSem->defnd) {
        errWN(NotUsdPar|Warn1|Rdbl, curTok.name);
        if (! allErrFl) curTok.ptrSem->defnd = True;}
      else {
        if (dpragNst == 0) {
          if (curTok.ptrSem->reallyUsed) curTok.ptrSem->usedMorThOnce = True;
          else curTok.ptrSem->reallyUsed = True;}
        curTok.ptrSem->used = True;}
      return;}
    storeMacChunk(curTok.tok, (const TsTok *)curTok.name, FullLgt(curTok.name));
    goto endCST;
  case CSTNU: case CSTST: {
      TsTok w = 0;  /* initialized just to quiet dcc down, since value
							   later overwritten. */

      storeMacChunk(curTok.tok, &w, 1);  /* so that length of 1st sub-block in
						    same macro storage block. */
      pLgtCharSubBlk = pMacSto - SizOfCharBlkLgtFld;
      if (curTok.tok == CSTNU) analNumCst(&storeMacChar);
      else analStrCst(&storeMacChar);
      *pLgtCharSubBlk = (TsTok)(pMacSto - pLgtCharSubBlk);  /* number of
					 characters (+1) in ending sub-block. */
      storeMacChunk((Ttok)0, NULL, 0); pMacSto--;  /* end marker = 0 (only one
								byte needed). */
      /*~zif ENDBLK == (Ttok)0 "ENDBLK must be non-zero" */
      goto endCST;}
  case FORCEMACEXP: case FORCEMACEXP1: case NOMACEXP:
    storeMacChunk(curTok.tok, (TsTok *)&pDescCurId, sizeof(pDescCurId));
endCST:
    /* Find beginning of stored token and put 'head' pseudo in it (in anticipa-
       tion of possible '##' operator). */
    GetTokAdr(oldPMS, oldCMB)
    *(oldPMS + 1) = *oldPMS;  /* real token value */
    *oldPMS = (TsTok)((concatFl == (COpnd1 | CISN))? (concatFl = COpnd1,
							    CONCATOP) : IDSTNU);
    break;
  case ENDDPRAG: dpragNst--; /*~NoBreak*/
  default: storeMacChunk(curTok.tok, NULL, curTok.val);}
}

bool visibleFromMac(const TsemanElt *pTag)
{
  TmacStkElt *w;

  if (isFNameVisible(pCurMac->dFileName, pTag)) return True;
  /* Find if possible calling macro(s) defined in visible file */
  for (w = pTopMacStk; w->prec != NULL; w = w->prec)
    if (w->pMac!=NULL && w->pArg!=NULL && isFNameVisible(w->pMac->dFileName,
							     pTag)) return True;
  return False;
}

static AllocXElt(allocMacStkElt, TmacStkElt *, ctrMSE, ;)
static FreeXElt(freeMacStkElt, TmacStkElt *, ctrMSE, ;, prec)
static AllocXElt(allocCondStkElt, TcondStkElt *, ctrCSE, ;)
static FreeXElt(freeCondStkElt, TcondStkElt *, ctrCSE, ;, prec)

/* End DCDIR.C */
