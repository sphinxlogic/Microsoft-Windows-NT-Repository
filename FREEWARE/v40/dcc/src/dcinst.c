/* DCINST.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#  pragma noinline (errInit1)
#endif

#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include "dcinst.h"
#include "dcinst.ph"
#include "dcblk.h"
#include "dcdir.h"
#include "dcdecl.h"
#include "dcexp.h"
#include "dcexp.ph"
#include "dcext.h"
#include "dcrec.h"

#define IsDecl1(x)	InsideInterval(curTok.tok, x, EDecl - 1)
#define IsSideEffect(x)	InsideInterval(NakedTok(x.topOper), BSideEffTok,       \
							      ESideEffTok - 1)
#define SubOP		(ADDOP | NONASSOC)

DefRepresType

typedef struct _TcaseElt TcaseElt;
struct _TcaseElt {  /* for checking non-duplication of case values */
  TcalcS valCase[(sizeof(TqElt6) - sizeof(TcaseElt *))/ sizeof(TcalcS)];
  TcaseElt *next;
};

typedef enum {PlainBlk, InsideSw, InsideIf = InsideSw<<1} Tenvir;

/* Function profiles */
static void block(void), checkNotInitVar(void), manageLoop(Ttok), stmt(void);
static bool blockDecl(bool) /*~PseudoVoid*/, isLabel(void), prntzBoolExp(
  Tstring);
static void errInit1(TnotInitVar *), initCheckIndent(void), processLabel(void),
  subStmt(bool);
static TcaseElt *allocCaseElt(void), *freeCaseElt(TcaseElt *);
static TnotInitVar *freeNotInitVarElt(TnotInitVar *);

/* Objects */
static TsemanElt *bckBrch = NULL;
static bool brkSeen, continueSeen;
static Tenvir curEnvir = PlainBlk;
static TpcTypeElt curFctRetType;
static TsemanElt *fwdBrch;
static TlineNb lastElseLineNb = 0;
static uint loopStmtLvl = 0;
static uint retCount;  /* number of 'return's in a function body */
static uint switchLvl = 0;
static const Ttok sColRBraLBra[] = {SCOL, RBRA, LBRA, EndSTok};
static const Ttok sColRBraLBraElse[] = {SCOL, RBRA, LBRA, ELSE, EndSTok};
static const Ttok sColRBraLBraCaseDefault[] = {SCOL, RBRA, LBRA, CASE,
							      DEFAULT, EndSTok};
static const Ttok sColRBraLBraCaseDefaultElse[] = {SCOL, RBRA, LBRA, CASE,
							DEFAULT, ELSE, EndSTok};
static const Ttok sColRParRBra[] = {SCOL, RPAR, RBRA, EndSTok};
static const Ttok *tokToBeSkpd[/*~IndexType Tenvir*/] = {&sColRBraLBra[0],
					    &sColRBraLBraCaseDefault[0],
					    &sColRBraLBraElse[0],
					    &sColRBraLBraCaseDefaultElse[0]};

/* External objects */
uint condStmtLvl;
uint initialLoopCondLvl;
uint loopLvl;
TnotInitVar *notInitVarList;
Trchbl nxtStmtRchbl;  /* next statement reachable ? */


void enterFctBody(TpcTypeElt fctType)
/* Prepares function body entry.
   Does not exit via 'longjmp', either directly or indirectly. */
{
  TmacExpnd prevCMEN = curMacExpNb;
  TpcTypeElt oldRetType = curFctRetType;  /* in case of embedded function
								  definition. */

  curFctRetType = (fctType == NULL)? NULL : NxtTypElt(fctType);
  /* Indent count configuration */
  if (NxtTok()!=RBRA && indentIncr<0 && (!ReallyInsideMacro || curMacExpNb!=
	prevCMEN) /* beware of functions defined by macros (such as, for
				   example, DefRepresType). */ && !headerFile) {
    if (spaceCount <= 0) {err0(NoIndentIndct | Warn1); indentIncr = 0;} /* to
						    prevent further warnings. */
    else indentIncr = spaceCount;}  /* first indentation configurate
							   indentation count. */
  retCount = loopLvl = condStmtLvl = 0; fwdBrch = NULL; notInitVarList = NULL;
  block();
  if (nxtStmtRchbl > Rchbl) err0(UnreachStmt|Warn2|PossErr);
  if (curFctRetType != NULL) {
    if (curFctRetType->typeSort != Void) {
      if (nxtStmtRchbl == Rchbl) errWN(NoRetAtEnd | Warn3, curFctName);}
    else
      if (retCount==0 && nxtStmtRchbl!=Rchbl) {
        if (!fctType->pvNr) errWN(MsngNevRet|Warn1|Rdbl, curFctName);}
      else {
        if (fctType->pvNr) errWN(IncorNevRet|Warn2|PossErr, curFctName);}}
  curFctRetType = oldRetType;
  GetNxtTok();	/* here for good error position */
/*  if (sizeofOpndType!=NULL || allocType!=NULL) sysErr(ExCod3);*/
}

static void block(void)
{
  blockDecl(True);
  /* Manage possible statements */
  nxtStmtRchbl = Rchbl;
  while (curTok.tok != RBRA) {
    if (curTok.tok == ENDPROG) {err0(RBraExptd); break;}   /* beware of case
				where RBRA is followed immediatly by ENDPROG. */
    stmt();}
  exitBlock();
  checkIndent();
}

static bool blockDecl(bool legal) /*~PseudoVoid*/
/* Manages the declaration part of a block */
{
  TcharTok w;
  bool declSeen = False;
  jmp_buf localJmpBuf, *saveErrRet;

  while (   IsDecl1(BDecl)
         || curTok.tok==IDENT
          && (    legal
	       && curTok.ptrSem==NULL  /* so that "toto var;int var1;" does not
				   get into 'stmt*' if toto "forced defined". */
               && (w = nxtCharOrMacTok(),
                        w>=0 && (w==(TcharTok)'*' || isAlfa((char)w))
 	             || w<0  && (w==-(TcharTok)STAR || w==-(TcharTok)IDENT))
              || curTok.ptrSem!=NULL
               && curTok.ptrSem->kind==Type
               && !(legal && isLabel()))) {
    if (! declSeen) {
      if (! legal) err0(IllPositDecl);
      saveErrRet = curErrRet;
      curErrRet = &localJmpBuf /*~ LocalAdr */;
      declSeen = True;
      if (setjmp(*curErrRet) != 0) {
        skipTok(sColRBraLBra);  /* return point in 'panic' mode */
        continue;}}
    decl();}
  if (declSeen) {
    curErrRet = saveErrRet;
    if ((oldSpaceCount+1<0 || spaceCount+1<0) && indentIncr>0  &&
      !ReallyInsideMacro && curTok.tok!=SCOL && legal) err0(SepDeclStmt|Warn1);}
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
  if (labldStmt) {
    if (spaceCount <= nestLvl*indentIncr) spaceCount = SpaceCntNoBegLine;}  /*
		      labels can be more on the left (for better visibility). */
  checkIndent();
  switch(curTok.tok) {
  case BREAK:
    if (switchLvl==0 && loopStmtLvl==0) err0(NotInsideLoopOrSwitch);
    brkSeen = True;
    goto termiByBrL;
  case CASE: case DEFAULT: {
      TvalTok prevTok = curTok;

      err0((curTok.hCod == 0)? NotInsideSwitch1|Warn2|Rdbl : NotInsideSwitch1);
      GetNxtTok();
      if (prevTok.tok == CASE) (void)correctExprN(NULL, WhoEnum, True, "case",
									  True);
      if (prevTok.hCod!=0 && !Found(COLON)) err0(ColExptd);
      goto tstLabL;}
  case CONTINUE:
    if (loopStmtLvl == 0) err0(NotInsideLoop);
    continueSeen = True;
termiByBrL:
    nxtStmtRchbl = NonRchbl_Fwd;
    GetNxtTok();
    goto checkSCol;
  /* case DEFAULT: see case CASE */
  case DO:
    manageLoop(DO);
    goto checkSCol;
  case ELSE: err0(MsngIF);
begStmt1:
    GetNxtTok();
tstLabL:
    labldStmt = isLabel();
    goto begStmt /*~ BackBranch */;
  case FOR:
    GetNxtTok();
    if (! Found(LPAR)) err0(LParExptd);
    if (curTok.tok != SCOL) {commaExpr(); checkSideEffect();}
    if (ignoreErr = False, !Found(SCOL)) {err0(SColExptd); skipTok(
								 sColRParRBra);}
    manageLoop(FOR);
    break;
  case GOTO: {
      Tname labelName;
      TsemanElt *ptrId;
      DefSem(notDefndLbl, Label, False);

      if (NxtTok() != IDENT) {err0(IdExptd); goto errPanicL;}
      labelName = storeLabelName(curTok.name);
      GetNxtTok();
      if ((ptrId = searchSymTab(labelName)) == NULL) {  /* new label */
        ptrId = enterSymTab(labelName);
        ptrId->infoS = notDefndLbl;
        ptrId->defLineNb = lineNb;
        ptrId->defFileName = curFileName;
        ptrId->nstLvla = baseNstLvl();
        if (curTok.tok == BCKBRCH) {errIlgDP(BCKBRCH); GetNxtTok();}
        if (condStmtLvl!=0 && fwdBrch!=ptrId) {
          fwdBrch = (fwdBrch == NULL)? ptrId : QuasiNULLval(TsemanElt *);
          condStmtLvl++;}}
      else 						/* already seen label */
        if (ptrId->inner) {
          errId(NonVisiLabel|Warn1|Rdbl, ptrId, labelName, NULL, NULL);}
        else if (ptrId->defnd) {
          if (! Found(BCKBRCH)) err0(BackwdBranch|Warn1|Rdbl);
          if (ptrId == bckBrch) {checkNotInitVar(); bckBrch = NULL;}}
      ptrId->used = True;
      nxtStmtRchbl = NonRchbl_Jmp;
      goto checkSCol;}
  case IF: {
      Trchbl endIfBrnchRchbl;
      bool ifBrnchBrkSeen, ifBrnchContSeen, oldBrkSeen = brkSeen,
					 oldContSeen = continueSeen, cstBoolExp;
      TcalcU boolExpVal;
      TmacLvl ifMacLvl = macLvl;
      Tenvir oldEnvir = curEnvir;

      cstBoolExp = prntzBoolExp("if");
      boolExpVal = cExp.uVal;
      condStmtLvl++;
      curEnvir |= InsideIf;
      subStmt(False);
      curEnvir = oldEnvir;
      condStmtLvl--;
      endIfBrnchRchbl= nxtStmtRchbl;
      ifBrnchBrkSeen = brkSeen;
      ifBrnchContSeen = continueSeen;
      brkSeen = oldBrkSeen;		/* back to		   */
      continueSeen = oldContSeen;	/*         initial         */
      nxtStmtRchbl = Rchbl;		/*                 values. */
      if (curTok.tok == ELSE) {
        if (macLvl < ifMacLvl) errWS(IllParenMacro|Warn3|PossErr, "else");
        checkIndent();
        condStmtLvl++;
        lastElseLineNb = lineNb;  /* prepare for "else if" on same line */
        subStmt(False);
        condStmtLvl--;}
      if (cstBoolExp) {
        if (boolExpVal != 0) {
          nxtStmtRchbl = endIfBrnchRchbl;
          brkSeen = ifBrnchBrkSeen;
          continueSeen = ifBrnchContSeen;}}
      else {
        if (endIfBrnchRchbl == Rchbl) nxtStmtRchbl = Rchbl;
        if (ifBrnchBrkSeen) brkSeen = True;
        if (ifBrnchContSeen) continueSeen = True;}}
    break;
  case LBRA: GetNxtTok(); enterBlock(); block(); goto getNxtTokAndBreakL;
  case NOBRK:
    if (switchLvl == 0) err0(NotInsideSwitch|Warn2|Rdbl);
    nxtStmtRchbl = NonRchbl_Fwd;  /* to prevent 'MsngBrk' warning */
getNxtTokAndBreakL:
    GetNxtTok();
    break;
  case RBRA: goto checkSCol;  /* to avoid 'StmtExptd' error, i.e. after label */
  case RETURN:
    retCount++;
    if (NxtTok()==SCOL || curTok.tok==RBRA) {
      if (curFctRetType!=NULL && curFctRetType->typeSort!=Void) errWSTT(
		BadRetType|Warn3|PossErr, NULL, curFctRetType, &natTyp[_VOID]);}
    else manageRetValue(curFctRetType);
    nxtStmtRchbl = NonRchbl_Jmp;
    goto checkSCol;
  case SCOL: goto checkSCol;  /* empty statement */
  case SWITCH: {
      TpcTypeElt switchType;

      GetNxtTok();
      if (! Found(LPAR)) err0(LParExptd);
      commaExpr();
      checkInit();
      if ((switchType = cExp.type)!=NULL && !IsTypeSort(switchType, WhoEnum)) {
        errWSTT(IlgOpndType, "switch", switchType, NULL);
        FreeExpType2(switchType);
        switchType = NULL;}
      if (cExp.rEvlbl) err0(CstSwitchExp|Warn2|PossErr);
      else CheckNumCstNamed(cExp, switchType);
      cleanExprThings();
      if (! Found(RPAR)) err0(RParExptd);
      if (curTok.tok != LBRA) err0(ShdBeBlk|Warn1|PossErr);
      else {
        TcaseElt *headCaseElt = NULL;
        uint nbCase = 0;
#define SizeValArray	(size_t)NbElt(((TcaseElt *)0)->valCase)
        TcalcS *lastPtrCase = &((TcaseElt *)0)->valCase[SizeValArray-1] 
					+ 1; /* pointer just after last filled
					   element of last array (of values). */
        bool oldBrkSeen;
        bool firstTurn = True, deltaIndent = False, deltaISet = False;
        Tenvir oldEnvir = curEnvir;

        checkIndent();
        GetNxtTok();
        switchLvl++;
        condStmtLvl++;
        enterBlock();
        blockDecl(True);
        oldBrkSeen = brkSeen;
        brkSeen = False;
        curEnvir = InsideSw;
        while (curTok.tok!=DEFAULT && curTok.tok!=RBRA && curTok.tok!=ENDPROG) {
          if (curTok.tok==CASE || isLabel()) {
            bool labelSeen = False;

            do {
              if (curTok.tok == IDENT) {labelSeen = True; processLabel();}
              else {
                if  (!firstTurn && nxtStmtRchbl==Rchbl) err0(MsngBrk|Warn3|
								       PossErr);
                firstTurn = False;
                if (spaceCount>=0 && indentIncr>0) {  /* decide current switch
		     indentation (see dc.tstInd before changing that code !). */
                  if (! deltaISet) {
                    if (spaceCount == nestLvl*indentIncr) {
                      deltaIndent = True;
                      deltaISet = True;}
                    else if (spaceCount == (nestLvl - 1)*indentIncr) {
                      nestLvl--;
                      deltaISet = True;}}
                  else nestLvl--;  /* for 'case' to stick out */
                  checkIndent();
                  if (deltaISet) nestLvl++;}
                if (labelSeen) {
                  err0(MisplaLbl|Warn1|Rdbl);
                  labelSeen = False;}
                GetNxtTok();
                {
                  bool w;

                  if (((switchType == NULL)
                    ? (w = correctExprN(NoFreeExpType, WhoEnum, True, "case",
					       True), switchType = cExp.type, w)
                    : correctExprN(switchType, switchType->typeSort /* non
				Void */, True, "case", True)) && !cExp.errEvl) {
                    /* Search if case constant already seen */
                    TcaseElt *curBlk;

                    for (curBlk = headCaseElt; curBlk != NULL; curBlk =
								 curBlk->next) {
                      TcalcS *ptrCase, *endPtrCase;

                      for (ptrCase = &curBlk->valCase[0], endPtrCase = ptrCase +
				NbElt(curBlk->valCase); ptrCase!=endPtrCase &&
						       ptrCase!=lastPtrCase; ) {
                        if (*ptrCase++ == cExp.sVal) {
                          paramTxt = longToS(cExp.sVal);
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
                    *lastPtrCase++ = cExp.sVal;
                    nbCase++;
endSearchL: ;}}
                nxtStmtRchbl = NonRchbl_Fwd;}  /* to prevent multiple 'MsngBrk'
								      errors. */
              if (! Found(COLON)) err0(ColExptd);
            } while (curTok.tok==CASE || isLabel());
            nxtStmtRchbl = Rchbl;}  /* to prevent 'UnreachStmt' error */
          if (firstTurn) {err0(CaseExptd|Warn3|PossErr); firstTurn = False;}
          if (curTok.tok != DEFAULT) stmt();
          else err0(StmtExptd);}
        if (firstTurn) err0(CaseExptd|Warn3|PossErr);
        curEnvir = oldEnvir;
        {
          uint nbEnumCst = 0;

          if (switchType!=NULL && switchType->typeSort==Enum) {
            /* Compute number of enum constants with differing values */
            if (switchType->tagId == NULL) nbEnumCst = UCHAR_MAX + 1; /* char */
            else if (switchType->frstEnumCst!=NULL) {
              register TsemanElt *w = switchType->frstEnumCst;
              TenumCst precVal = w->enumVal;

              nbEnumCst++;
              while ((w = w->nxtEnumCst) != NULL) {
                if (w->enumVal != precVal) {precVal = w->enumVal
							      ; nbEnumCst++;}}}}
          if (curTok.tok == DEFAULT) {  /* real 'default' token, or NoDefault
								    d-pragma. */
            uint savCurVal = curTok.hCod;

            if (! firstTurn) {
              if  (nxtStmtRchbl==Rchbl && savCurVal!=0) err0(MsngBrk|Warn3|
								       PossErr);
              nestLvl--;
              checkIndent();
              nestLvl++;}
            GetNxtTok();
            if (savCurVal != 0) {  /* not NoDefault d-pragma */
              if (! Found(COLON)) err0(ColExptd);
              if (curTok.tok == FULLENUM) {
                if (nbCase != nbEnumCst) err0(WrngFullEnum|Warn1|Rdbl);
                GetNxtTok();}
              nxtStmtRchbl = Rchbl;
              stmt();
              while (curTok.tok!=RBRA && curTok.tok!=ENDPROG) stmt();}
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
        condStmtLvl--;
        if (deltaIndent) nestLvl--;
        exitBlock();
        checkIndent();
        if (!Found(RBRA)) err0(RBraExptd);
        if (brkSeen) nxtStmtRchbl = Rchbl;
        brkSeen = oldBrkSeen;}
      break;}
  case WHILE:
    manageLoop(WHILE);
    break;
  default:		/* label or expression */ 
    if (labldStmt) {processLabel(); goto begStmt1 /*~ BackBranch */;}
    if (blockDecl(False)) return;
    /* Else expression */
    cExp.topOper = SCOL;  /* to get 'StmtExptd' error msg */
    commaExpr();
    checkSideEffect();
checkSCol:
    lastSCOLline = lineNb;
    ignoreErr = False;
    if (Found(SCOL)) return;
    err0(SColExptd);
errPanicL:
    skipTok(tokToBeSkpd[curEnvir]);}
}

/******************************************************************************/
/*                                 UTILITIES                                  */
/******************************************************************************/

static AllocXElt(allocCaseElt, TcaseElt *, ctrCE, ;)

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
  if (spaceCount>=0 /* beginning of line */ && (indentIncr>0 || indentIncr<0
	       && nestLvl==0) && !headerFile /* because of remanent effect of
								 'scndMsg' */) {
    static Tstring savFileName;

    if (spaceCount != nestLvl * indentIncr) {
      if (frstBadIndentLine == 0) {
        TnbBuf buf1, buf2;

        bufLongToS(spaceCount, buf1);
        bufLongToS(nestLvl, buf2);
        errWSSSS(BadIndent | Warn1, buf1, buf2, longToS((indentIncr < 0)? 0 :
							     indentIncr), NULL);
        savFileName = curFileName;
        frstBadIndentLine = lineNb;}
      else scndMsgFl = True;}
    else {
      if (scndMsgFl) {
        errWSS((curFileName == savFileName)? BadIndent1|Warn1 : BadIndent2|
			  Warn1, longToS((long)frstBadIndentLine), savFileName);
        scndMsgFl = False;}
      frstBadIndentLine = 0;}
    spaceCount = SpaceCntNoBegLine;}  /* because of macros */
}

/*~Undef frstBadIndentLine, scndMsgFl */

static void checkNotInitVar(void)
{
  if (--loopLvl == 0) {
    while (notInitVarList != NULL) {
      if (! notInitVarList->ptrId->initlz) errInit1(notInitVarList);
      notInitVarList = freeNotInitVarElt(notInitVarList);}}
}

void checkNotInitVarAndSuppress(const TsemanElt *ptrId)
{
  TnotInitVar *cur = notInitVarList, *prec = (TnotInitVar * /*~OddCast*/)
			 ((ubyte *)&notInitVarList - Offset(TnotInitVar, prec));

  do {
    if (cur->ptrId == ptrId) {
      if (! cur->ptrId->initlz) errInit1(cur);
      prec->prec = freeNotInitVarElt(cur);
      return;}
    prec = cur;
  } while ((cur = cur->prec) != NULL);
}

void errInit(TsemanElt *ptrId, TlineNb line, Tstring file, bool initDPFl)
{
  Tstring cmpl = (initDPFl)? errTxt[InitDP] : NULL;

  if (file == NULL) errWNSS(NotInit|Warn3|PossErr, ptrId->nameb, cmpl, NULL);
  else errWFName(NotInit1|Warn3|PossErr, line, file, ptrId->nameb, cmpl, NULL);
  if (ptrId->initBefUsd) {
    errWN(IlgInitBU|Warn2|PossErr, ptrId->nameb);
    ptrId->initBefUsd = False;}  /* to prevent further messages */
  ptrId->initlz = True;  /* to prevent further messages */
}

static void errInit1(TnotInitVar *x)
{
  errInit(x->ptrId, x->lineNb, x->fileName, x->initDPFl);
}

static FreeXElt(freeCaseElt, TcaseElt *, ctrCE, ;, next)
static FreeXElt(freeNotInitVarElt, TnotInitVar *, ctrNI, ;, prec)

void initInst(void)
{
  initCheckIndent();
}

static bool isLabel(void)
{
  int w;

  return curTok.tok==IDENT && (w=nxtCharOrMacTok(), w==(int)':' || w==-(int)
									 COLON);
}

static void manageLoop(Ttok loopSort)
{
  bool infiniteLoop, oldContSeen, oldBrkSeen;

  if (loopLvl++ /* incremented here because of 'UnusedVar' warning */ == 0
	) initialLoopCondLvl = condStmtLvl;  /* for case "int i; <loopHead> if
				     (i==..)" => immediate 'NotInit' warning. */
  if (loopSort != DO) {
    if (loopSort == FOR) {
      infiniteLoop = True;
      if (curTok.tok != SCOL) {  /* there exist a (continuation) test */
        commaExpr();
        if (!boolExp("for") || cExp.uVal==0) infiniteLoop = False;}
      if (ignoreErr = False, !Found(SCOL)) {err0(SColExptd); skipTok(
								sColRParRBra);}}
    else infiniteLoop = prntzBoolExp("while") && cExp.uVal!=0;
    if (! infiniteLoop) {
      condStmtLvl++;  /* for case "i=0; for (..;..;..) i=1;"
						  => no 'UnusedVar1' warning. */
      if (loopLvl == 1) initialLoopCondLvl = condStmtLvl;}
    if (loopSort == FOR) {
      if (curTok.tok != RPAR) {
        condStmtLvl++;  /* force delayed check of initialization for variables
		      appearing in 3rd expression, and no 'UnusedVar1' check. */
        commaExpr();
        checkSideEffect();
        condStmtLvl--;}
      if (curTok.tok != RPAR) err0(RParExptd);}}
  oldBrkSeen = brkSeen;
  brkSeen = False;
  oldContSeen = continueSeen;
  continueSeen = False;
  loopStmtLvl++;
  if (loopSort == DO) {
    subStmt(True);
    if (curTok.tok != WHILE) err0(WhileExptd);
    else if (spaceCount>=0 && indentIncr>0 && spaceCount<=nestLvl*indentIncr
					       ) err0(DWhileNotLinedUp | Warn1);
    infiniteLoop = (prntzBoolExp("do") && cExp.uVal!=0);
    if (curTok.tok==RPAR /* for nicer following error messages */) GetNxtTok();}
  else {
    subStmt(False);
    if (! infiniteLoop) condStmtLvl--;}
  loopStmtLvl--;
  checkNotInitVar();
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
  if (! Found(LPAR)) err0(LParExptd);  /* done that way (instead of checking
	      afterwards cExp.topOper) in order to get clearer error message. */
  commaExpr();
  res = boolExp(operTxt);
  if (curTok.tok != RPAR) err0(RParExptd);
  return res;
}

static void processLabel(void)
{
  Tname labelName = storeLabelName(curTok.name);
  TsemanElt *ptrId;

  GetNxtTok();	/* COLON */
  if ((ptrId = searchSymTab(labelName)) == NULL) {  /* new label */
    DefSem(defndLbl, Label, True);

    ptrId = enterSymTab(labelName);
    ptrId->infoS = defndLbl;
    if (bckBrch == NULL) {
      if (loopLvl++ == 0) initialLoopCondLvl = condStmtLvl;  /* backward branch
							    => probable loop. */
      bckBrch = ptrId;}}
  else if (ptrId->defnd) {errId(AlrdDefLabel, ptrId, labelName, NULL, NULL)
								      ; return;}
  else {
    if (ptrId->nstLvla<baseNstLvl()) errId(TargLabelNotVisi|Warn1|Rdbl,
						  ptrId, labelName, NULL, NULL);
    ptrId->defnd = True;
    if (ptrId == fwdBrch) {fwdBrch = NULL; condStmtLvl--;}} /* end goto reach */
  ptrId->defLineNb = lineNb;		/* record              */
  ptrId->defFileName = curFileName;	/*        birth place. */
  ptrId->nstLvla = baseNstLvl();
}

static void subStmt(bool blockSeen)
/*  Detects if more than one statement on same line after an 'if'/'else'/loop.
    In case "if (...) Macro" => warning if more than one statement in Macro;
            "if (...) stmt;" as macro body, no warning when macro called;
	    "if (...) stmt1; \NL stmt2" => no warning. */
{
  bool incrSubStmt = False;

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
  if (! (curTok.tok==LBRA && (spaceCount<0 || spaceCount==nestLvl*indentIncr)
				 || curTok.tok==IF && lastElseLineNb==lineNb)) {
    /* Force indentation */
    nestLvl++;
    incrSubStmt = True;}
  lastElseLineNb = 0;
  stmt();
  if (   !blockSeen
      && lineNb==lastSCOLline  /* no newline after ';' */
      && curTok.tok!=RBRA && curTok.tok!=ELSE && curTok.tok!=CASE && curTok.
								    tok!=DEFAULT
      && nxtStmtRchbl==Rchbl
      && curMacExpNb>=begMacExpNb  /* inside same or higher macro level than at
						   beginning of statement. */) {
    err0(BlockQM|Warn2|PossErr);
    begMacExpNb = UINT_MAX;}  /* to prevent further error */
  if (incrSubStmt) nestLvl--;
}

/* End DCINST.C */
