/* DCINST.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#  pragma noinline (swallowColon, swallowLPar, swallowSCol)
#endif

#include <string.h>
#include <ctype.h>
#include "dcinst.h"
#include "dcinst.ph"
#include "dcblk.h"
#include "dcdecl.h"
#include "dcdir.h"
#include "dcexp.h"
#include "dcexp.ph"
#include "dcext.h"
#include "dcrec.h"
#include "dcrecdir.th"

#define SaveCSL		{savOCL = oldCondLvl; oldCondLvl = condStmtLvl;	       \
						       condStmtLvl = nestLvl;}

DefRepresType

typedef struct _tCaseElt TcaseElt;
struct _tCaseElt {  /* for checking non-duplication of case values */
  TcalcS valCase[(sizeof(TqElt6) - sizeof(TcaseElt *))/ sizeof(TcalcS)];
  TcaseElt *next;
};

typedef struct _TnotInitVar TnotInitVar;
struct _TnotInitVar {
  TsemanElt *ptrId;
  TlineNb lineNb;
  Tstring fileName;
  bool initDPFl;
  TnotInitVar *next;
};

typedef enum {PlainBlk, InsideSw = 1, InsideIf = InsideSw<<1} Tenvir;

/* Function profiles */
static TcaseElt *allocCaseElt(void), *freeCaseElt(TcaseElt *);
static TnotInitVar *allocNotInitVarElt(void), *freeNotInitVarElt(TnotInitVar *);
static void block(void), manageCondLvl(const TnstLvl *), manageLoop(Ttok),
  stmt(void);
static bool blockDecl(DeclStatus) /*~PseudoVoid*/, isLabel(void), prntzBoolExp(
  Tstring);
static void initCheckIndent(void), processLabel(void), subStmt(bool);
static void swallowColon(void), swallowLPar(void), swallowSCol(void);


/* Objects */
static const TsemanElt *bckBrch;
static TnstLvl befFwdBrchCondLvl;
static bool brkSeen, continueSeen;
static Tenvir curEnvir = PlainBlk;
static TpTypeElt curFctRetType;
static int fwdBrchCtrM1;  /* number of currently active forward branches (-1) */
static TnstLvl fwdBrchLvl;
static TlineNb lastElseLineNb = 0;
static uint loopStmtLvl = 0;
static TnotInitVar *notInitVarList = NULL;
static TnstLvl oldCondLvl;
static uint retCount;  /* number of 'return's in a function body */
static uint switchLvl = 0;
static const Ttok sColRBraLBra[] = {SCOL, RBRA, LBRA, EndSTok};
static const Ttok sColRBraLBraElse[] = {SCOL, RBRA, LBRA, ELSE, EndSTok};
static const Ttok sColRBraLBraCaseDefault[] = {SCOL, RBRA, LBRA, CASE,
							      DEFAULT, EndSTok};
static const Ttok sColRBraLBraCaseDefaultElse[] = {SCOL, RBRA, LBRA, CASE,
							DEFAULT, ELSE, EndSTok};
static const Ttok sColRParRBra[] = {SCOL, RPAR, RBRA, EndSTok};
static const Ttok *tokToBeSkpd[/*~IndexType Tenvir*/] = {
  &sColRBraLBra[0],
  &sColRBraLBraCaseDefault[0],
  &sColRBraLBraElse[0],
  &sColRBraLBraCaseDefaultElse[0]
};

/* External objects */
TnstLvl condStmtLvl;  /* zero as long as control flow has to go through here */
TnstLvl initialLoopCondLvl;
bool longjmpTaken;
uint loopLvl;
TnstLvl setjmpBlkLvl = 0;
bool setjmpSeen = False;


void enterFctBody(TpTypeElt fctType)
/* Prepares function body entry.
   Does not exit via 'longjmp', either directly or indirectly. */
{
  TmacExpNb prevCMEN = curMacExpNb;
  TpTypeElt oldRetType = curFctRetType;  /* in case of embedded function
								  definition. */

  curFctRetType = (fctType == NULL)? NULL : NxtTypElt(fctType);
  /* Indent count configuration */
  if (spaceCount >= 0 /* '{' at beginning of line */) findIndentCnt();
  else GetNxtTok();
  if (curTok.tok!=RBRA && indentIncr<0 && (!ReallyInsideMacro || curMacExpNb!=
	prevCMEN) /* beware of functions defined by macros (such as, for
				example, DefRepresType). */ && !insideHdrFile) {
    if (spaceCount <= 0) {err0(NoIndentIndct | Warn1); indentIncr = 0;} /* to
						    prevent further warnings. */
    else indentIncr = spaceCount;}  /* first indentation configurate
							   indentation count. */
  retCount = loopLvl = condStmtLvl = initialLoopCondLvl = fwdBrchLvl = 0;  /*
		       have to be reset because of possible previous 'goto's. */
  fwdBrchCtrM1 = oldCondLvl = -1;
  bckBrch = NULL;
  block();
  if (nxtStmtRchbl > Rchbl) err0(UnreachStmt|Warn2|PossErr);
  if (curFctRetType != NULL) {
    if (curFctRetType->typeSort != Void) {
      if (nxtStmtRchbl == Rchbl) errWN(NoRetAtEnd | Warn3, curFctName);}
    else
      if (retCount==0 && nxtStmtRchbl!=Rchbl) {
        if (!fctType->PvNr) errWN(MsngNevRet|Warn1|Rdbl, curFctName);}
      else {
        if (fctType->PvNr) errWN(IncorNevRet|Warn2|PossErr, curFctName);}}
  curFctRetType = oldRetType;
  GetNxtTok();	/* here for good error position */
}

static void block(void)
{
  blockDecl(D_LEGAL);
  /* Manage possible statements */
  nxtStmtRchbl = Rchbl;
  while (curTok.tok != RBRA) {
    if (curTok.tok == ENDPROG) {errWS(Msng, "}"); break;}   /* beware of case
				where RBRA is followed immediatly by ENDPROG. */
    stmt();}
  exitBlock();
  checkIndent();
}

static bool blockDecl(DeclStatus declAlwd) /*~PseudoVoid*/
/* Manages the declaration part of a block */
{
  Ttok w;
  bool declSeen = False;

  while (   IsDecl1(BDecl)
         || curTok.tok==IDENT
          && (    declAlwd>=D_LEGAL
	       && curTok.PtrSem==NULL  /* so that "toto var;int var1;" does not
				   get into 'stmt*' if toto "forced defined". */
               && ((w = peepNxtTok())==STAR || w==IDENT)
              || curTok.PtrSem!=NULL
               && curTok.PtrSem->Kind==Type
               && !(declAlwd>=D_LEGAL && isLabel()))) {
    if (! declSeen) {
      if (declAlwd < D_LEGAL) err0(IllPositDecl);
      declSeen = True;}
    decl(declAlwd, sColRBraLBra);}
  if (declSeen
      && spaceCount!=SetInGetTokFrmMac /* not inside macro, after 2nd token */
      && (oldSpaceCount<0 /* no blank line before */
          || spaceCount==SetInGetTokFrmTxt /* not at beginning of line */)
      && indentIncr>0
      && curTok.tok!=SCOL
      && declAlwd>=D_LEGAL) err0(SepDeclStmt|Warn1|Rdbl);
  return declSeen;
}

static void stmt(void)
{
  bool labldStmt = isLabel();

  if (nxtStmtRchbl != Rchbl) {
    if (! labldStmt) {
      if (curTok.tok == SCOL) goto begStmt;  /* empty statement */
      err0(UnreachStmt|Warn2|PossErr);}
    nxtStmtRchbl = Rchbl;}
begStmt:
  if (! (labldStmt && spaceCount<=nestLvl*indentIncr)) checkIndent();  /* labels
			     can be more on the left (for better visibility). */
  switch(curTok.tok) {
  case BREAK:
    if (switchLvl==0 && loopStmtLvl==0) err0(NotInsideLoopOrSwitch);
    brkSeen = True;
    goto termiByBrchL;
  case CASE: case DEFAULT: {
      const TvalTok prevTok = curTok;

      err0((curTok.Hcod == 0)? NotInsideSwitch1|Warn2|Rdbl : NotInsideSwitch1);
      GetNxtTok();
      if (prevTok.tok == CASE) (void)correctExprN(NULL, WhoEnum, True, "case",
								     chkNumCst);
      if (prevTok.Hcod != 0) swallowColon();
      goto tstLabL;}
  case CONTINUE:
    if (loopStmtLvl == 0) err0(NotInsideLoop);
    continueSeen = True;
termiByBrchL:
    nxtStmtRchbl = NonRchbl_Fwd;
    GetNxtTok();
    goto checkSCol;
  /* case DEFAULT: see case CASE */
  case DO:
    manageLoop(DO);
    goto checkSCol;
  case DPTOK:
    if ((Tdprag)curTok.Val == NOBRK) {
      if (switchLvl == 0) err0(NotInsideSwitch|Warn2|Rdbl);
      nxtStmtRchbl = NonRchbl_Fwd;  /* to prevent 'MsngBrk' warning */
      goto getNxtTokAndBreakL;}
    goto defaultL;
  case ELSE: err0(MsngIF);
begStmt1:
    GetNxtTok();
tstLabL:
    labldStmt = isLabel();
    goto begStmt /*~ BackBranch */;
  case FOR:
    GetNxtTok();
    swallowLPar();
    if (curTok.tok != SCOL) {commaExpr(); checkSideEffect();}
    swallowSCol();
    manageLoop(FOR);
    break;
  case GOTO: {
      Tname labelName;
      TsemanElt *ptrId;
      DefSem(notDefndLbl, Label, False);

      if (NxtTok() != IDENT) {err0(IdExptd); goto errPanicL;}
      labelName = storeLabelName(curTok.IdName);
      GetNxtTok();
      if ((ptrId = searchSymTab(labelName)) == NULL) {  /* new label */
        ptrId = enterSymTab(labelName);
        ptrId->InfoS = notDefndLbl;
        ptrId->defLineNb = lineNb;
        ptrId->defFileName = curFileName;
        ptrId->NstLvla = baseNstLvl();
        if (IsDP(BCKBRCH)) {errIlgDP(BCKBRCH); GetNxtTok();}
        if (++fwdBrchCtrM1 == 0) {
          befFwdBrchCondLvl = oldCondLvl;
          condStmtLvl = oldCondLvl = fwdBrchLvl = nestLvl;}}
      else { /* already seen label */
        if (ptrId->Inner)
          errId(NonVisiLabel|Warn1|Rdbl, ptrId, labelName, NULL, NULL);
        if (ptrId->Defnd) {
          if (! FoundDP(BCKBRCH)) err0(BackwdBranch|Warn1|Rdbl);
          if (ptrId == bckBrch) {
            checkNotInitVarAndSuppress(NULL);
            bckBrch = NULL;}}}
      ptrId->Used = True;
      nxtStmtRchbl = NonRchbl_Jmp;
      goto checkSCol;}
  case IF: {
      Trchbl endIfBrnchRchbl;
      bool ifBrnchBrkSeen, ifBrnchContSeen, oldBrkSeen = brkSeen,
		       oldContSeen = continueSeen, cstBoolExp, insideSJ = False;
      TnstLvl oldSetjmpBlkLvl = setjmpBlkLvl, savOCL;
      TcalcU boolExpVal;
      TmacLvl ifMacLvl = 0;
      Tenvir oldEnvir = curEnvir;

      if (curEnvir&InsideIf && ReallyInsideMacro) ifMacLvl = macLvl;
      cstBoolExp = prntzBoolExp("if");
      SaveCSL;
      if (cExp.topOper & SETJMPUSED) {
        setjmpSeen = False;
        insideSJ = True;
        if (longjmpTaken) {insideSJ = (bool)-1;}
        else setjmpBlkLvl = nestLvl;}
      cleanExprThings();
      boolExpVal = cExp.Uval;
      curEnvir |= InsideIf;
      subStmt(False);
      curEnvir = oldEnvir;
      ifBrnchBrkSeen = brkSeen;
      ifBrnchContSeen = continueSeen;
      endIfBrnchRchbl= nxtStmtRchbl;
      brkSeen = oldBrkSeen;		/* back to		   */
      continueSeen = oldContSeen;	/*         initial         */
      nxtStmtRchbl = Rchbl;		/*                 values. */
      if (curTok.tok == ELSE) {
        if (macLvl < ifMacLvl) errWS(IllParenMacro|Warn3|PossErr, "else");
        checkIndent();
        lastElseLineNb = lineNb;  /* prepare for "else if" on same line */
        setjmpBlkLvl = ((int)insideSJ < 0)? nestLvl : oldSetjmpBlkLvl;
        subStmt(False);}
      manageCondLvl(&savOCL);
      if (cstBoolExp) {
        if (boolExpVal != 0) {
          nxtStmtRchbl = endIfBrnchRchbl;
          brkSeen = ifBrnchBrkSeen;
          continueSeen = ifBrnchContSeen;}}
      else {
        if (endIfBrnchRchbl == Rchbl) nxtStmtRchbl = Rchbl;
        if (ifBrnchBrkSeen) brkSeen = True;
        if (ifBrnchContSeen) continueSeen = True;}
      if (insideSJ) setjmpBlkLvl = (oldSetjmpBlkLvl != 0)? oldSetjmpBlkLvl :
								       nestLvl;}
    break;
  case LBRA: GetNxtTok(); enterBlock(); block();
getNxtTokAndBreakL:
    GetNxtTok();
    break;
  case RBRA: goto checkSCol;  /* to avoid 'StmtExptd' error, i.e. after label */
  case RETURN:
    retCount++;
    if (NxtTok()==SCOL || curTok.tok==RBRA) {
      if (curFctRetType!=NULL && curFctRetType->typeSort!=Void) errWSTNSS(
			   BadRetType|Warn3|PossErr, typeToS2(curFctRetType),
					   &natTyp[VoidDpl], NULL, NULL, NULL);}
    else manageRetValue(curFctRetType);
    nxtStmtRchbl = NonRchbl_Jmp;
    goto checkSCol;
  case SCOL: goto checkSCol;  /* empty statement */
  case SWITCH: {
      TpTypeElt switchType;

      GetNxtTok();
      swallowLPar();
      commaExpr();
      checkInit();
      if ((switchType = cExp.type) != NULL) {
        if (! IsTypeSort(switchType, WhoEnum)) {
          errWSTT(IlgOpndType, "switch", switchType, NULL);
          FreeExpType2(switchType);
          switchType = NULL;}
        if (cExp.Revlbl) err0(CstSwitchExp|Warn2|PossErr);
        else CheckNumCstNamed(cExp, switchType);}
      cleanExprThings();
      if (! Found(RPAR)) errMsngRPar();
      if (curTok.tok != LBRA) err0(ShdBeBlk|Warn1|PossErr);
      else {
        TcaseElt *headCaseElt = NULL;
        uint nbCase = 0;
#define SizeValArray	(size_t)NbElt(((TcaseElt *)0)->valCase)
        TcalcS *lastPtrCase = &((TcaseElt *)0)->valCase[SizeValArray-1] 
					+ 1; /* pointer just after last filled
					   element of last array (of values). */
        bool oldBrkSeen, firstTurn = True;
        TnstLvl deltaIndent = -1;
        Tenvir oldEnvir = curEnvir;
        TnstLvl savOCL;

        checkIndent();
        GetNxtTok();
        switchLvl++;
        enterBlock();
        SaveCSL;
        blockDecl(D_NOINITLZ);
        oldBrkSeen = brkSeen;
        brkSeen = False;
        curEnvir = InsideSw;
        while (curTok.tok!=DEFAULT && curTok.tok!=RBRA && curTok.tok!=ENDPROG) {
          if (curTok.tok==CASE || isLabel()) {
            bool labelSeen = False;

            do {
              if (curTok.tok == IDENT) {labelSeen = True; processLabel();}
              else {
                if  (!firstTurn && nxtStmtRchbl==Rchbl) err0(MsngBrk|Warn2|
								       PossErr);
                firstTurn = False;
                if (spaceCount>=0 && indentIncr>0) {  /* decide current switch
		     indentation (see dc.tstInd before changing that code !). */
                  if (deltaIndent < 0) {
                    if (spaceCount == nestLvl*indentIncr) deltaIndent = 1;
                    else if (spaceCount == (nestLvl - 1)*indentIncr) {nestLvl--
							    ; deltaIndent = 0;}}
                  else nestLvl--;  /* for 'case' to stick out */
                  checkIndent();
                  if (deltaIndent >= 0) nestLvl++;}
                if (labelSeen) {
                  err0(MisplaLbl|Warn1|Rdbl);
                  labelSeen = False;}
                GetNxtTok();
                {
                  bool w;

                  if (((switchType == NULL)
                    ? (w = correctExprN(NoFreeExpType, WhoEnum, True, "case",
					  chkNumCst), switchType = cExp.type, w)
                    : correctExprN(switchType, switchType->typeSort /* non
			   Void */, True, "case", chkNumCst)) && !cExp.ErrEvl) {
                    /* Search if case constant already seen */
                    const TcaseElt *curBlk;

                    for (curBlk = headCaseElt; curBlk != NULL; curBlk =
								 curBlk->next) {
                      const TcalcS *ptrCase, *endPtrCase;

                      for (ptrCase = &curBlk->valCase[0], endPtrCase = ptrCase +
				NbElt(curBlk->valCase); ptrCase!=endPtrCase &&
						       ptrCase!=lastPtrCase; ) {
                        if (*ptrCase++ == cExp.Sval) {
                          paramTxt = grstIntToS(cExp.Sval);
                          err1(AlrdUsedCaseVal);
                          goto endSearchL;}}}
                    /* Note it */
                    if (lastPtrCase > &headCaseElt->valCase[SizeValArray - 1]) {
                      /* Current array full */
#undef SizeValArray
                      TcaseElt *newBlk = allocCaseElt();

                      newBlk->next = headCaseElt;
                      headCaseElt = newBlk;
                      lastPtrCase = &newBlk->valCase[0];}
                    *lastPtrCase++ = cExp.Sval;
                    nbCase++;
endSearchL: ;}}
                nxtStmtRchbl = NonRchbl_Fwd;}  /* to prevent multiple 'MsngBrk'
								      errors. */
              swallowColon();
            } while (curTok.tok==CASE || isLabel());
            nxtStmtRchbl = Rchbl;}  /* to prevent 'UnreachStmt' error */
          if (firstTurn) {errWS(Exptd|Warn3|PossErr, "case"); firstTurn= False;}
          if (curTok.tok == DEFAULT) err0((curTok.Hcod == 0)? StmtExptd :
							  StmtExptd|Warn1|Rdbl);
          else stmt();}
        if (firstTurn) errWS(Exptd|Warn3|PossErr, "case");
        curEnvir = oldEnvir;
        {
          uint nbEnumCst = 0;

          if (switchType!=NULL && switchType->typeSort==Enum) {
            /* Compute number of enum constants with differing values */
            if (switchType->TagId == NULL) nbEnumCst = UCHAR_MAX + 1; /* char */
            else if (switchType->FrstEnumCst != NULL) {
              register const TsemanElt *w = switchType->FrstEnumCst;
              TenumCst precVal = w->EnumVal;

              nbEnumCst++;
              while ((w = w->NxtEnumCst) != NULL) {
                if (w->EnumVal != precVal) {precVal = w->EnumVal
							      ; nbEnumCst++;}}}}
          if (curTok.tok == DEFAULT) {  /* real 'default' token, or NoDefault
								    d-pragma. */
            uint savDTok = curTok.Hcod;

            if (! firstTurn) {
              if (nxtStmtRchbl==Rchbl && savDTok!=0) err0(MsngBrk|Warn2|
								       PossErr);
              nestLvl--;
              checkIndent();
              nestLvl++;}
            GetNxtTok();
            if (savDTok != 0) {  /* not NoDefault d-pragma */
              swallowColon();
              if (IsDP(FULLENUM)) {
                if (nbCase != nbEnumCst) err0(WrngFullEnum|Warn1|Rdbl);
                GetNxtTok();}
              nxtStmtRchbl = Rchbl;
              do {
                stmt();
              } while (curTok.tok!=RBRA && curTok.tok!=ENDPROG);}
            else {  /* NoDefault d-pragma */
              if (nbCase == nbEnumCst) err0(UslNoDefault|Warn1|Rdbl);
              brkSeen = True;}}
          else if (nbCase != nbEnumCst) {
            if (brkSeen) err0(DefaultExptd|Warn1|Rdbl|PossErr);
            brkSeen = True;}}
        /* Free case constants list */
        while (headCaseElt != NULL) {headCaseElt = freeCaseElt(headCaseElt);}
        FreeExpType1(switchType);
        switchLvl--;
        if (deltaIndent > 0) nestLvl--;
        manageCondLvl(&savOCL);
        exitBlock();
        checkIndent();
        if (!Found(RBRA)) errWS(Msng, "}");
        if (brkSeen) nxtStmtRchbl = Rchbl;
        brkSeen = oldBrkSeen;}
      break;}
  case WHILE:
    manageLoop(WHILE);
    break;
  default:		/* label or expression */ 
defaultL:
    if (labldStmt) {processLabel(); goto begStmt1 /*~ BackBranch */;}
    if (blockDecl(D_ILLEGAL)) return;
    /* Else expression */
    cExp.topOper = SCOL;  /* to get 'StmtExptd' error msg */
    commaExpr();
    checkSideEffect();
checkSCol:
    lastSCOLline = lineNb;
    ignoreErr = False;
    if (Found(SCOL)) return;
    errMsngSCol();
errPanicL:
    skipTok(tokToBeSkpd[curEnvir]);}
}

/******************************************************************************/
/*                                 UTILITIES                                  */
/******************************************************************************/

static AllocXElt(allocCaseElt, TcaseElt, ctrCE, ;)
static AllocXElt(allocNotInitVarElt, TnotInitVar, ctrNI, ;)

void addNotInitVarList(TsemanElt *ptrId, bool initDPFl)
{
  register TnotInitVar *w = notInitVarList;

  while (w != NULL) {
    if (ptrId == w->ptrId) return;  /* already in list */
    w = w->next;}
  w = allocNotInitVarElt();
  w->ptrId = ptrId;
  w->lineNb = lineNb;
  w->fileName = curFileName;
  w->initDPFl = initDPFl;
  w->next = notInitVarList;
  notInitVarList = w;
}

static TlineNb frstBadIndentLine = 0;
static bool scndMsgFl = False;

static void initCheckIndent(void)
{
  frstBadIndentLine = 0;
  scndMsgFl = False;
}

void checkIndent(void)
{
  ignoreErr = False;
  if (spaceCount>=0 /* beginning of line */ && (indentIncr>0 || indentIncr<0 &&
		nestLvl==0) && !insideHdrFile /* because of remanent effect of
								 'scndMsg' */) {
    static Tstring savFileName;

    if (spaceCount != nestLvl * indentIncr) {
      if (frstBadIndentLine == 0) {
        TnbBuf buf1, buf2;

        bufGrstIntToS(spaceCount, buf1);
        bufGrstIntToS(nestLvl, buf2);
        errWSSSS(BadIndent | Warn1, buf1, buf2, grstIntToS((indentIncr < 0)? 0 :
							     indentIncr), NULL);
        savFileName = curFileName;
        frstBadIndentLine = lineNb;}
      else scndMsgFl = (lineNb!=frstBadIndentLine || curFileName!=savFileName);}
    else {
      if (scndMsgFl) {
        errWSSSS(BadIndent1|Warn1, grstIntToS(frstBadIndentLine), NULL,
		(curFileName == savFileName)? NULL : errTxt[File], savFileName);
        scndMsgFl = False;}
      frstBadIndentLine = 0;}}
}

/*~Undef frstBadIndentLine, scndMsgFl */

void checkNotInitVarAndSuppress(const TsemanElt *ptrId)
/* 'ptrId' == NULL => manage end of loop, else end of block */
{
  TnotInitVar *cur, *prev;

  if (ptrId == NULL) {if (--loopLvl != 0) return; initialLoopCondLvl = 0;}
  for (cur = notInitVarList, prev = (TnotInitVar * /*~OddCast*/)
			 ((ubyte *)&notInitVarList - Offset(TnotInitVar, next));

       cur != NULL;
       cur = prev->next) {
    TsemanElt *curPId;

    if ((curPId= cur->ptrId)==ptrId || ptrId==NULL && curPId->Attribb!=StatiL) {
      if (! curPId->Initlz) errInit(curPId, cur->lineNb, cur->fileName,
								 cur->initDPFl);
      prev->next = freeNotInitVarElt(cur);
      if (ptrId != NULL) return;}
    else prev = cur;}
}

void errInit(TsemanElt *ptrId, TlineNb line, Tstring file, bool initDPFl)
{
  Tstring cmpl = (initDPFl)? errTxt[InitDP] : NULL;

  if (file == NULL) errWNSS(NotInit|Warn3|PossErr, ptrId->PdscId->idName, cmpl,
									  NULL);
  else errWFName(NotInit1|Warn3|PossErr, line, file, ptrId->PdscId->idName,
								    cmpl, NULL);
  if (ptrId->InitBefUsd) {
    errWN(IlgInitBU|Warn2|PossErr, ptrId->PdscId->idName);
    ptrId->InitBefUsd = False;}  /* to prevent further messages */
  ptrId->Initlz = True;  /* to prevent further messages */
}

static FreeXElt(freeCaseElt, TcaseElt *, ctrCE, ;, next)
static FreeXElt(freeNotInitVarElt, TnotInitVar *, ctrNI, ;, next)

void initInst(void)
{
  initCheckIndent();
}

static bool isLabel(void)
{
  return (curTok.tok==IDENT && peepNxtTok()==COLON);
}

static void manageCondLvl(const TnstLvl *x)
{
  condStmtLvl = (oldCondLvl >= fwdBrchLvl)
		 ? oldCondLvl
		 : (fwdBrchLvl <= nestLvl)
		   ? fwdBrchLvl
		   : nestLvl;
  oldCondLvl = *x;
}

static void manageLoop(Ttok loopSort)
{
  bool infiniteLoop, oldContSeen, oldBrkSeen;
  TnstLvl savOCL;

  if (loopLvl++ /* incremented here because of 'UnusedVar' warning */ == 0
	) initialLoopCondLvl = condStmtLvl;  /* for case "int i; <loopHead> if
				     (i==..)" => immediate 'NotInit' warning. */
  if (loopSort != DO) {
    if (loopSort == FOR) {
      infiniteLoop = True;
      if (curTok.tok != SCOL) {  /* there exist a (continuation) test */
        commaExpr();
        if (!boolExp("for") || cExp.Uval==0) infiniteLoop = False;}
      swallowSCol();}
    else infiniteLoop = prntzBoolExp("while") && cExp.Uval!=0;
    if (! infiniteLoop) {
      SaveCSL;  /* for case "i=0; for (..;..;..) i=1;" => no 'UnusedVar1'
								     warning. */
      if (loopLvl == 1) initialLoopCondLvl = condStmtLvl;}
    if (loopSort == FOR) {
      if (curTok.tok != RPAR) {
        TnstLvl savCondLvl = condStmtLvl;

        condStmtLvl = INT_MAX;  /* force delayed check of initialization for
	    variables appearing in 3rd expression, and no 'UnusedVar1' check. */
        commaExpr();
        checkSideEffect();
        condStmtLvl= savCondLvl;}
      if (curTok.tok != RPAR) errMsngRPar();}}
  oldBrkSeen = brkSeen;
  brkSeen = False;
  oldContSeen = continueSeen;
  continueSeen = False;
  loopStmtLvl++;
  if (loopSort == DO) {
    subStmt(True);
    if (curTok.tok != WHILE) errWS(Exptd, "while");
    else if (spaceCount>=0 && indentIncr>0 && spaceCount<=nestLvl*indentIncr
					    ) err0(DWhileNotLinedUp|Warn1|Rdbl);
    infiniteLoop = (prntzBoolExp("do") && cExp.Uval!=0);
    if (curTok.tok==RPAR /* for nicer following error messages */) GetNxtTok();}
  else {
    subStmt(False);
    if (! infiniteLoop) manageCondLvl(&savOCL);}
  loopStmtLvl--;
  checkNotInitVarAndSuppress(NULL);
  if (brkSeen) nxtStmtRchbl = Rchbl;
  else if (infiniteLoop) nxtStmtRchbl = NonRchbl_Jmp;
  else if (continueSeen) nxtStmtRchbl = Rchbl;
  brkSeen = oldBrkSeen;
  continueSeen = oldContSeen;
}

static bool prntzBoolExp(Tstring operTxt)
/* Returns True if boolean expression correct and constant */
{
  bool res;

  if (curTok.tok!=SCOL && curTok.tok!=RBRA) GetNxtTok();
  if (! Found(LPAR)) errWS(Exptd, "(");  /* done that way (instead of checking
	      afterwards cExp.topOper) in order to get clearer error message. */
  commaExpr();
  res = boolExp(operTxt);
  if (curTok.tok != RPAR) errMsngRPar();
  return res;
}

static void processLabel(void)
{
  Tname labelName = storeLabelName(curTok.IdName);
  TsemanElt *ptrId;

  GetNxtTok();	/* COLON */
  if ((ptrId = searchSymTab(labelName)) == NULL) {  /* new label */
    DefSem(defndLbl, Label, True);

    ptrId = enterSymTab(labelName);
    ptrId->InfoS = defndLbl;
    if (bckBrch == NULL) {
      if (loopLvl++ == 0) initialLoopCondLvl = condStmtLvl;  /* backward branch
							    => probable loop. */
      bckBrch = ptrId;}}
  else if (ptrId->Defnd) {errId(AlrdDefLabel, ptrId, labelName, NULL, NULL)
								      ; return;}
  else {
    if (ptrId->NstLvla<baseNstLvl()) errId(TargLabelNotVisi|Warn1|Rdbl,
						  ptrId, labelName, NULL, NULL);
    ptrId->Defnd = True;
    if (--fwdBrchCtrM1 < 0) {  /* no more unterminated forward branch */
      if (oldCondLvl > befFwdBrchCondLvl) oldCondLvl = befFwdBrchCondLvl;  /*
			'if', 'switch', loop where 'goto' met not terminated. */
      else if (condStmtLvl > befFwdBrchCondLvl) condStmtLvl = befFwdBrchCondLvl;
      if (oldCondLvl < 0) condStmtLvl = 0;  /* 'goto' met outside of
								 conditional. */
      fwdBrchLvl = 0;}}
  ptrId->defLineNb = lineNb;		/* record              */
  ptrId->defFileName = curFileName;	/*        birth place. */
  ptrId->NstLvla = baseNstLvl();
}

static void subStmt(bool blockSeen)
/*  Detects if more than one statement on same line after an 'if'/'else'/loop.
    In case "if (...) Macro" => warning if more than one statement in Macro;
            "if (...) stmt;" as macro body, no warning when macro called;
	    "if (...) stmt1; \NL stmt2" => no warning. */
{
  TnstLvl deltaIndent = 0;

  begMacExpNb = curMacExpNb;
  if (curTok.tok==RPAR || blockSeen || lastElseLineNb!=0) /* for nicer
				        following error messages */ GetNxtTok();
  if (curTok.tok == SCOL) {
    err0(EmptyStmt|Warn2|PossErr);
    goto setBlockSeen;}  /* to prevent possible 'BlockQM' error message */
  if (curTok.tok == LBRA)
setBlockSeen:
    blockSeen = True;
  while (isLabel()) {processLabel(); GetNxtTok();}
  if (! ( curTok.tok==LBRA
          && (spaceCount<0
              || spaceCount==nestLvl*indentIncr
               && (!InsideMacro || macLvl==1 && aloneInNoParMac())) /* case
							    "#define BEGIN {" */
         || curTok.tok==IF
          && lastElseLineNb==lineNb)) {
    /* Force indentation */
    nestLvl++;
    deltaIndent = 1;}
  lastElseLineNb = 0;
  stmt();
  if (   !blockSeen
      && lineNb==lastSCOLline  /* no newline after ';' */
      && curTok.tok!=RBRA && curTok.tok!=ELSE && curTok.tok!=CASE && curTok.
					     tok!=DEFAULT && curTok.tok!=ENDPROG
      && nxtStmtRchbl==Rchbl
      && curMacExpNb>=begMacExpNb  /* inside same or higher macro level than at
						   beginning of statement. */) {
    err0(BlockQM|Warn2|PossErr);
    begMacExpNb = UINT_MAX;}  /* to prevent further error */
  nestLvl -= deltaIndent;
}

static void swallowColon(void)
{
  if (! Found(COLON)) errWS(Exptd, ":");
}

static void swallowLPar(void)
{
  if (! Found(LPAR)) errWS(Exptd, "(");
}

static void swallowSCol(void)
{
  ignoreErr = False;
  if (! Found(SCOL)) {errMsngSCol(); skipTok(sColRParRBra);}
}

/* End DCINST.C */
