/* DCMAIN.C */
/* Y.L. Noyelle, Supelec, France 1998 */
/* Entry module */
/* Manages command line, source files, character input/output, and errors */

#include <ctype.h>
#include <stdlib.h>
#ifndef EXIT_FAILURE
#  define EXIT_SUCCESS	0
#  define EXIT_FAILURE	1
#endif
#include <string.h>
#if defined(VMS) && !defined(_POSIX_C_SOURCE)
#  define _POSIX_C_SOURCE  0  /* for "fileno" function */
#  include <stdio.h>
#  include <unistd.h>
#  undef _POSIX_C_SOURCE
#elif defined(_MSC_VER) && !defined(_POSIX_)
#  define _POSIX_
#  include <stdio.h>
#  include <unistd.h>
#  undef _POSIX_
#elif defined(__hpux) && !defined(_INCLUDE_POSIX_SOURCE)
#  define _INCLUDE_POSIX_SOURCE
#  include <stdio.h>
#  include <unistd.h>
#  undef _INCLUDE_POSIX_SOURCE
#elif defined(__linux__) && !defined(__USE_POSIX)
#  define __USE_POSIX
#  include <stdio.h>
#  include <unistd.h>
#  undef __USE_POSIX
#else
#  include <stdio.h>
#  include <unistd.h>
#endif

#include "dcmain.h"
#include "dcmain.ph"
#include "dcblk.h"
#include "dcblk.ph"
#include "dcdecl.h"
#include "dcdecl.ph"
#include "dcdir.h"
#include "dcdir.ph"
#include "dcexp.ph"
#include "dcext.h"
#include "dcext.ph"
#include "dcinst.ph"
#include "dcprag.h"
#include "dcprag.ph"
#include "dcrec.h"
#include "dcrec.ph"
#include "dcrecdir.ph"

#ifdef VMS
#  pragma noinline (changeOutStreamTo, copyDirPrefix, createAdjFileName)
#  pragma noinline (dispUsg, errExit, fmtdMsg, foundOption)
#  pragma noinline (getNxtFName, intrnErr, nxtChunkOfTxt, mngOptListTok)
#  pragma noinline (mngOptMsgLimit, mngOptStopAfterMsg, mngOptVerbose)
#  pragma noinline (restoInclInfoAndPopStk)
#  pragma noinline (searchNxtNL, storeDMacTxt, waitAndAnalAnswer)
#endif

#define ComputeFileHCode(x) {						       \
  ThCode fileHCode = 0;							       \
									       \
  {register const char *p = x;; while (*p != '\0') {fileHCode +=	       \
						  (ThCode)RealChar(*p); p++;}}
#define DccOptPref	"+z"  /* could be "-z", just the same */
#define DccSymbol	"__dcc"
#define DefineStr	"define "
#define DfltOptVal	-1
#ifdef VMS
#define DDOpt		"/def="
#define DIOpt		"/incl="
#define DUOpt		"/undef="
#else
#define DDOpt		"-D"
#define DIOpt		"-I"
#define DUOpt		"-U"
#endif
#define EmptyDirName	((TdirName)"\0")
#define ErrDccOption(x) {						       \
  if (lastTrtdFName==NULL && !frstPass) fmtdMsg(x, parArr1);		       \
  argErrFl = True;}
#define IdentHeaderSpace "  "
/*~ zif LitLen(IdentHeaderSpace) != LgtHdrId "Bad 'IdentHeaderSpace' string" */
#define InitHdrFName(x)							       \
  *x++ = (Tchar)False;  /* reset ComposingHdr flag */			       \
  *x++ = (Tchar)0;  /* no added path */					       \
  /*~zif LgtHdrAddedPath != 2 "Problem" */
#define LgtD(x)		(size_t)*x
#define LS2		(LineSize + 1 + 1)  /* +1 for NL */
#define MinDccOptLgt	4
#define NbBitsWarn	2
#define NbSrcLinesKept	2
#define ResetSpaceAtEndOfLine spaceAtEndOfLine = (wrapFl)? LineSize : INT_MAX
#define SearchAdjFiles(fileName, x)                                            \
  ComputeFileHCode(fileName)						       \
    for (ptrAdjDesc = adjFilesArr; ptrAdjDesc < adjFilesArr +		       \
	 				      usedSizAdjArr; ptrAdjDesc++) {   \
      if (fileHCode==ptrAdjDesc->hCode && (StrEq(fileName,		       \
					      ptrAdjDesc->amendFName) || x)) {
#define SHIFTWARN	(INT_BIT - NbBitsWarn)
/*~zif (uint)WarnMsk>>SHIFTWARN != (1u << NbBitsWarn)-1 "Bad 'NbBitsWarn'"*/
#define SHRT_BIT	CHAR_BIT*sizeof(short)
#define SizeSrcBuffer (NbSrcLinesKept*(LineSize + TransfSize) + (3))
#define TxtOptions	"'" DDOpt "', '" DUOpt "', '" DIOpt "'"
#define UndefStr	"undef "

typedef FILE *Tstream;
typedef const TdirChar *TdirName;
typedef TdirChar *TdirNameNC;

typedef struct {
  ThCode hCode;
  bool noLoadSysFile;
  Tstring amendFName;
  Tstring adjFName;
} TadjFilesDesc;

typedef struct {
  const TdirName *curInclDir;
  TmngtPriv *listPriv;
  Tstream srcStream;
  char *ptrS, *oldMaxPtrS, *posEndChP1, *nxtPosEndChP1;
  TlineNb lineNb;
  char idPlusSrc[LgtHdrId + MaxLgtId + SizeSrcBuffer];
} TsrcMngtBlk;

typedef struct _inclStkElt TinclStkElt;
struct _inclStkElt {
  TsrcMngtBlk *srcMngtBlk;
  TundfTagElt *headUTList;
  Tstring curHdrFName;
  Tstring fileName;
  union {
    struct {
      bool _insideHdrFile:1;
      bool _insideHdrInHdr:1;
      bool _sysHdrFile:1;
      bool _diffLc:1;
      int :CHAR_BIT - 1 - 1 - 1 - 1;  /* 'portable' padding */
      int :CHAR_BIT;  /* cannot be regrouped with previous one */
      TindentChk _indentIncr:SHRT_BIT;
    } s1;
    TallocUnit bid;
  } inclInfo;
  TinclStkElt *prev;
};
/*~zif __bitoffset(((TinclStkElt *)0)->inclInfo.s1, _indentIncr) != SHRT_BIT
				"Inefficient layout for structure 'inclInfo'" */
#define InsideHdrFile	inclInfo.s1._insideHdrFile
#define InsideHdrInHdr	inclInfo.s1._insideHdrInHdr
#define ShdrFile	inclInfo.s1._sysHdrFile
#ifdef LcEqUc
#define DiffLc		inclInfo.s1._diffLc
#endif
#define IndentIncr	inclInfo.s1._indentIncr

typedef struct _incldInBlk TincldInBlk;
struct _incldInBlk {
  Tstring includedFName, includingFName;
  TlineNb lineNb;
  TincldInBlk *prev;
};

typedef struct {
  Tstring fileName;
  TlineNb lineNb;
} TposInSrc;

typedef uint TmsgCtr;

/* Function profiles */
static TincldInBlk *allocIncldFElt(void), *freeIncldFElt(TincldInBlk *);
static TinclStkElt *allocInclStkElt(void), *freeInclStkElt(TinclStkElt *);
static char *manageEndInclude(void), *searchNxtNL(char *, ptrdiff_t);
/****static char *trigraph(char *, char *);*/
static void changeOutStreamTo(Tstream), checks(void), closeSource(void),
  dispUsg(void), emitCstWdthCh(const char *, const char *), emitC(char),
  emitFName(Tstring), errExit(Terr, const Tstring *) /*~NeverReturns*/,
  flushAndExit(void) /*~NeverReturns*/, initAdjFiles(void), initAll(void),
  initEmitC(void), initMain(void), intrnErr(Tstring, uint *),
  openSrcFile(Tstring), resetArgPtr(void), resetOptions(void),
  restoInclInfoAndPopStk(TinclStkElt *), storeDMacTxt(Tchar);
static void mngOptListTok(void), mngOptMsgLimit(void), mngOptStopAfterMsg(void),
  mngOptVerbose(void);
static bool fmtdMsg(Terr n, const Tstring *parArr) /*~PseudoVoid*/,
  foundOption(Tstring, bool *), isDccOption(Tstring), isHeaderFile(Tstring),
  isSrcFName(Tstring), openSrcFile1(Tstring),
  prmtrzMsg(Tstring, const Tstring *) /*~PseudoVoid*/,
  processCUnit(Tstring, bool), waitAndAnalAnswer(void) /*~PseudoVoid*/;
static TdirName copyDirPrefix(Tstring, size_t);
static TundfTagElt *freeUndfTagElt(TundfTagElt *);
static Tstring getNxtArg(void);
#ifdef LcEqUc
static bool sameLowerCaseName(const char *, const char *, bool);
#endif
static TposInSrc srchPosInclLvl0(const TinclStkElt *);

/* Global variables */
static Tstring adjFiles;
static TadjFilesDesc *adjFilesArr = NULL;
static char *begLastTokBuf = NULL;
static bool callCompil;
static bool callCompilSeen = False;
static bool callCompilW;
static bool chkIndent;
static bool chkPossErr;		/* check various possible errors */
static bool chkRdbl;		/* check readability */
static int compilerStatus = EXIT_SUCCESS;
static TstringNC copDfltArgs, dfltArgsPtr;
static TmsgCtr ctrErr;
static uint ctrdI;
static TmsgCtr ctrIgndMsg;
static uint ctrIIB = 0;		/* number of header files included inside a
								       block. */
static uint ctrISE = 0;		/* number of '#include' stack elements */
static TmsgCtr ctrWarn;
static long cumNbLines;		/* cumulated number of lines processed */
static TsrcMngtBlk *curSrcMngtBlk = NULL;
static Tstring dccDir = DccExecFilesDir;
static uint dccDirLgt;
static int deltaPtrS = 0;	/* device to limit size of loaded chunk while
					searching next NL (cf searchNxtNL()). */
static Tstring dfltArgs;
static TdirName *dirArray;	/* array of directory prefixes for 'include'
								       files. */
static bool emitTrailer;
static Tchar *endDMacBuf;
static char *endSBuf;		/* end of source buffer */
static const char *endLastTokBuf;
static bool errSeen = False;
static bool existErr;
static bool frstPass = False;
static bool givAlwsFName;
static TincldInBlk *headListIIB = NULL; /* head of list of header files included
					inside a block (which may cause surprise
					if re-included thereafter...). */
static int initMsgLimit;
static bool initStopAfterMsg = True;
static bool initVerbose;
static bool interactivOutptDev;
static Tstring lastTrtdFName = NULL;
static size_t lgtCmdBuf;	/* length of command buffer */
static size_t lgtLastTokBuf;	/* length of "last tokens" buffer */
static bool listTok;		/* flag 'list last tokens on error' */
static uint lvlNoCompil;
static uint minMsgLvl;
static bool moreInfMsg;
static int msgLimit;
static bool msgLimitFl;
static bool notCompiled = False;
static char *nxtPosEndChP1;
static Tstring oldLastTrtdFName = NULL;
static char *oldMaxPtrS;	/* end of logical (circular) source buffer */
static int optionValue = DfltOptVal;
static Tstream outputStream;
static bool ovfldTokBuf;
static CreateParArr((1));
static const Tstring *pArg;
static char *pLastTokBuf;	/* pointer inside last tokens buffer */
static TposInSrc posLastInclWa;	/* position of last AlrdIncldInBlk warning */
static Tchar *pStoDMacTxt;
static TinclStkElt *pTopInclStk = NULL;
static bool reportHghsWaFl;
static bool screenMsgPass2;
static bool signalAllErr;
static int spaceAtEndOfLine;	/* number of remaining character slots at end
				      of current line (see 'emitC' function). */
static Tstream srcStream;
static bool stopAfterMsg;
static Tstring sysHdrDir = SysHdrDir;
static size_t totalSizAdjArr = 0;
static size_t usedSizAdjArr = 0;
static const Tstring *valInitParg;
static bool warnFl;
static bool warnSeen = False;
static bool wrapFl;
static const Ttok zSCol[] = {NoSwallowTok, SCOL, EndSTok};

/* External variables */
char *begSBuf;	/* beginning of source buffer (circular buffer) */
bool fileClosed;
bool frstLineOfFile;
char *nlPosP1;	/* 1 + position of last seen 'newline' character */
char *posEndChP1; /* 1 + position of EndCh character indicating end of current
								  text chunk. */
char *srcPtr;  /* source pointer */
Tstring starterFile;
TindentChk tabSpacing = DefaultTabSpacing;

int main(int argc, const Tstring *argv)
{
  Tstring lastFName = NULL;
  bool argErrFl = False;

  outputStream = SUMMARY_STREAM;
  initEmitC();
  valInitParg = argv;
  if (argc == 1) dispUsg();
  else {
    interactivOutptDev = isatty(fileno(MAIN_OUT_STREAM)) > 0;
    {
      TstringNC w, w1;

      if ((w = getenv("DCCFILES"))!=NULL && *w!='\0') {
        MyAlloc(dccDir = w1, strlen(w) + 1);
        strcpy(w1, w);
        if ((w = strchr(w1, ' ')) != NULL) {*w++ = '\0'; sysHdrDir = w;}}
      MyAlloc(starterFile = w, (dccDirLgt = (uint)strlen(dccDir)) + (LitLen(
							     StarterFile) + 1));
      strcpy(w, dccDir);
      strcat(w, StarterFile);
      MyAlloc(adjFiles = w, dccDirLgt + (LitLen(AdjFiles) + 1));
      strcpy(w, dccDir);
      strcat(w, AdjFiles);}
    if ((dfltArgs = getenv("DCCDFLTS")) == NULL) dfltArgs = "";
    MyAlloc(copDfltArgs, strlen(dfltArgs) + (LitLen(PrefixDCCDFLTS) + 1));
    /* Search position of last file name in command line, so as to process it
       with all indicated options, even ones met after that file name. */
    {
      Tstring w;

      resetArgPtr();
      while ((w = getNxtArg()) != NULL) if (isSrcFName(w)) lastFName = w;}
rescanOptL:  /* back here if more than one compilation unit to process; all
						previous options re-explored. */
    initAll();
    enterBlock();
    cUnitFName = NULL;  /* for starter file */
    openSrcFile(starterFile);
    dollAlwdInId = True;
    prog();
    closeSource();
    initAdjFiles();  /* here, because uses symbol defined in starter file */
    dollAlwdInId = False;
    cUnitFName = curFileName = "";  /* for -D, -U options processing (call to
				   'manageDir', which may note these values). */
    {
      Tstring arg;

      resetArgPtr();
      ctrdI = 0;  /* number of '-I' options */
      lgtCmdBuf = 0;  /* length of command buffer */
      while ((arg = getNxtArg()) != NULL) {
        if (isDccOption(arg)) {  /* take option into account immediatly,
		     because -D, -U, -I options management may create errors. */
          bool bidFl;

          parArr1[1] = arg;
          if (foundOption("ac", &askConstFl)) {}
          else if (foundOption("ae",  &signalAllErr)) {}
          else if (foundOption("afn", &givAlwsFName)) {}
          else if (foundOption("bo",  &chkBool)) {}
          else if (foundOption("cc",  &callCompil)) {}
          else if (foundOption("ctn", &chkTypName)) {}
          else if (foundOption("cw",  &callCompilW)) {}
          else if (foundOption("dol", &dollAlwdInId)) {}
          else if (foundOption("efi", &chkEffic)) {}
          else if (foundOption("epl", &chkEmptParList)) {}
          else if (foundOption("fci", &chk1stCharVF)) {
            chkTypName = chk1stCharVF;}
          else if (foundOption("frt", &chkFctRetTyp)) {}
          else if (foundOption("gpr", &goodPrgmg)) {
            chk1stCharVF = chkBool = chkFctRetTyp = chkInclPos = chkNumCst =
								     goodPrgmg;}
          else if (foundOption("inc", &chkInclPos)) {}
          else if (foundOption("ind", &chkIndent)) {}
          else if (foundOption("kwm", &kwAsMacName)) {}
          else if (foundOption("lt",  &listTok)) {
            if (optionValue >= MaxLgtId + 1) {lgtLastTokBuf = (size_t)
					 optionValue; optionValue = DfltOptVal;}
            else lgtLastTokBuf = LineSize - LitLen(Dots) - (strlen(errTxt[
			      LastToks]) - 1);  /* so that just one line used */
            mngOptListTok();}
          else if (foundOption("lvl", &bidFl)) {
            if (! bidFl) {
              if (optionValue>0 && optionValue<=(int)((uint)WarnMsk>>SHIFTWARN
									    )) {
                lvlNoCompil = (uint)optionValue;
                optionValue = DfltOptVal;}}
            else lvlNoCompil = ((uint)WarnMsk>>SHIFTWARN) + 1;}
          else if (foundOption("mcc", &verifAllPtrForConst)) {
            if (verifAllPtrForConst) askConstFl = True;}
          else if (foundOption("mic", &moreIndexTypeChk)) {}
          else if (foundOption("msg", &msgLimitFl)) {
            if (optionValue >= 0) {
              initMsgLimit = optionValue;
              optionValue = DfltOptVal;}
            mngOptMsgLimit();}
          else if (foundOption("nui", &warnNUI)) {}
          else if (foundOption("nup", &warnNUP)) {}
          else if (foundOption("nw",  &noWarn)) {}
          else if (foundOption("pe",  &chkPossErr)) {}
          else if (foundOption("pnt", &chkNmdTypParFl)) {}
          else if (foundOption("po",  &chkPortbl)) {}
          else if (foundOption("rd",  &chkRdbl)) {}
          else if (foundOption("rhw", &reportHghsWaFl)) {if (reportHghsWaFl
							) signalAllErr = False;}
          else if (foundOption("rst", &bidFl)) {if (bidFl) resetOptions();}
          else if (foundOption("sam", &initStopAfterMsg))
            mngOptStopAfterMsg();
          else if (foundOption("sy",  &printStat)) {}
          else if (foundOption("tab", &bidFl)) {
            if (bidFl) {
              if (optionValue > 0) {
                tabSpacing = (TindentChk)optionValue;
                optionValue = DfltOptVal;}}
            else tabSpacing = DefaultTabSpacing;}
          else if (foundOption("tr",  &emitTrailer)) {}
          else if (foundOption("uc",  &chkNumCst)) {chkArrBnd = chkNumCst;}
          else if (foundOption("udt", &chkUCTags)) {}
          else if (foundOption("usg", &bidFl)) {if (bidFl) dispUsg();}
          else if (foundOption("ve",  &moreInfMsg) || foundOption("vve",
						 &initVerbose)) mngOptVerbose();
          else if (foundOption("wa",  &warnFl)) {}
          else if (foundOption("wr",  &wrapFl)) {}
          else ErrDccOption(UnknOption)
          if (optionValue != DfltOptVal) {
            ErrDccOption(IgndCharsOpt)
            optionValue = DfltOptVal;}}
        else {
          int dOptionSeen;

          if (strncmp(arg, DIOpt, LitLen(DIOpt)) == 0) ctrdI++; /* count number
							     of '-I' options. */
          else if ((dOptionSeen = strncmp(arg, DDOpt, LitLen(DDOpt)))==0 ||
		  strncmp(arg, DUOpt, LitLen(DUOpt))==0) {  /* '-D/-U' option */
            for (;;) {  /* only there to avoid 'goto's */
              Tchar c, bufDef[MaxMacroLgtInCmd + LgtHdrId + 1 + 1];  /* +1 for
			EndCh, +1 for overflow detection; space for identifier
			header needed in front of any source buffer that can
			begin with an identifier and is processed via
			getTokFromTxt(). */
              register Tstring ptrInArg;

              if (dOptionSeen == 0) {
                strncpy(&bufDef[LgtHdrId], DefineStr, LitLen(DefineStr));
                pStoDMacTxt = &bufDef[LgtHdrId +LitLen(DefineStr)]/*~LocalAdr*/;
                ptrInArg = &arg[LitLen(DDOpt)];}
              else {
                strncpy(&bufDef[LgtHdrId], UndefStr, LitLen(UndefStr));
                pStoDMacTxt = &bufDef[LgtHdrId + LitLen(UndefStr)]/*~LocalAdr*/;
                ptrInArg = &arg[LitLen(DUOpt)];}
              endDMacBuf = &bufDef[NbElt(bufDef)] /*~LocalAdr*/;
              while (c = *ptrInArg++, isAlnu(c)) storeDMacTxt(c);  /* store
								  macro name. */
              if (dOptionSeen == 0) {
                storeDMacTxt(' ');
                if (c == '\0') storeDMacTxt('1');/* "1" is default macro text */
                else if (c != '=') {errWS(BadDOption | Warn2, arg); break;}
                else {  /* copy macro body */
                  while ((c = *ptrInArg++) != '\0') storeDMacTxt(c);}}
              else if (c != '\0') {errWS(BadDOption | Warn2, arg); break;}
              storeDMacTxt(EndCh);  /* 'fileClosed'=True here */
              posEndChP1 = NULL;  /* previous EndCh = end of pseudo-file */
              if (pStoDMacTxt >= endDMacBuf /* buffer overflow*/) {errWS(
					       BadDOption | Warn2, arg); break;}
              /* Prepare to print pseudo source line in case of error */
              dirLineFl = True; /* for 'manageDir()', but also allows printing
						     of source line on error. */
              begSBuf = nlPosP1 = srcPtr = &bufDef[LgtHdrId] /*~LocalAdr*/;
              frstLineOfFile = True;
              oldMaxPtrS = endDMacBuf;
              if (listTok) storeTokTxt(" #");
              manageDir();
              ClrSmshCh;
              break;}}
          lgtCmdBuf += strlen(arg) + 1;  /* +1 because of separating space; use-
			  less for already processed compilation units names. */
          if (isSrcFName(arg)) {
            if (arg != lastFName) {  /* because, for last compilation unit,
		    possible following options also to be taken into account. */
              if (lastTrtdFName != NULL) {  /* skip already processed
							   compilation units. */
                if (arg == lastTrtdFName) {
                  oldLastTrtdFName = lastTrtdFName;
                  lastTrtdFName = NULL;}}
              else {  /* process new compilation unit */
                if (! processCUnit(arg, False)) goto rescanOptL /*~BackBranch*/;
                if (notCompiled) goto exitL;
                lastTrtdFName = arg;
                if (emitTrailer) {
                  changeOutStreamTo(MAIN_OUT_STREAM);  /* separating pattern
							   more useful there. */
                  {
                    uint i;

                    for (i = 0; i < LineSize/LitLen(SeparPattern); i++)
                      emitS(SeparPattern);
                    emitC('\n');}
                  changeOutStreamTo(SUMMARY_STREAM);}
                goto rescanOptL /*~BackBranch*/;}}}}}
      if (! processCUnit(lastFName, True)) goto rescanOptL /*~BackBranch*/;}}
exitL:
  changeOutStreamTo(NULL);
  if (compilerStatus != EXIT_SUCCESS) return compilerStatus;
  if (argErrFl) return EXIT_ERRORS2;
  return (errSeen)
    ? (callCompilSeen)
      ? EXIT_ERRORS2
      : EXIT_ERRORS1
    : (warnSeen)
      ? (notCompiled)
        ? EXIT_WARNINGS2
        : EXIT_WARNINGS1
      : EXIT_SUCCESS;
}

static bool processCUnit(Tstring cUFName, bool lastFileFl)
/* Returns False iff first pass (for maximum warning level finding) */
{
  TdirName *ptrDirArray;  /* pointer in array of directories for 'include'
								       files. */
  char *cmdBuf, *ptrCmdBuf;
  register Tstring arg;
  bool fNameSeen = False;

  /* Determine if there is to be a 'first pass', and then if second pass is to
     report only highest level messages. */
  screenMsgPass2 = (frstPass && minMsgLvl!=0);
  frstPass = (frstPass || signalAllErr)? False : reportHghsWaFl;
  /* Get compiler name */
  {
    TlitString w;
    static char locBuf[] = IdentHeaderSpace DccSymbol ";";  /* ';' for
						     getLitString() workings. */

    getTokLvl++;  /* prevent tokens from showing */
    srcPtr = &locBuf[LgtHdrId];
    if (NxtTok()!=CSTST || (w = getLitString(), srcPtr != AdLastEltP1(locBuf)
						  - 1)) errExit(BadUUdcc, NULL);
    lgtCmdBuf += initGetStrLit(w);
    getTokLvl--;}
  /* Allocate array of directory prefixes for 'include' files */
  MyAlloc(dirArray, (ctrdI + (3))*sizeof(*dirArray));  /* +3 because of
			compilUnitFile directory, SysHdrDir, and ending NULL. */
  /* Allocate buffer for compile command (will be executed via the 'system'
								   function). */
  ptrCmdBuf = cmdBuf = allocPermSto(lgtCmdBuf);
  while ((*ptrCmdBuf++ = nxtStrLitChar()) != '\0') {} /* put compiler name */
  resetGetStrLit();
  exitBlock();  /* because of 'getLitString()' */
  ptrCmdBuf--;
  cUnitFName = NULL;
  resetArgPtr();
  ptrDirArray = dirArray + 1;
  while ((arg = getNxtArg()) != NULL) {

    if (isDccOption(arg)) continue;
    if (strncmp(arg, DIOpt, LitLen(DIOpt)) == 0) *ptrDirArray++ =
		copyDirPrefix(arg + LitLen(DIOpt), strlen(arg + LitLen(DIOpt)));
    else if (strncmp(arg, DDOpt, LitLen(DDOpt))==0 || strncmp(arg, DUOpt,
	     LitLen(DUOpt))==0) {}  /* '-D'/'-U' option already taken care of */
    else {  /* file name by default */
      fNameSeen = True;
      if (arg != cUFName) {
        if (isSrcFName(arg)) continue;}  /* ignore already processed
						       compilation unit name. */
      else {
        Tstring begOfFName;
        size_t lgtPath;

        begOfFName = skipPath(arg);
        lgtPath = (size_t)(begOfFName - arg);
        *dirArray = (lgtPath != 0)? copyDirPrefix(arg, lgtPath) :
							       &EmptyDirName[0];
        {
#ifdef VMS
          /* Add '.c' type suffix if none */
          size_t suffLgt = (SearchDot(begOfFName) == NULL)? LitLen(
					      CSuffix) /* no type suffix */ : 0;
          Tstring w;
          size_t posVersNb = (((w = strchr(begOfFName, ';')) == NULL)?
				 strlen(begOfFName) : (size_t)(w - begOfFName));
          TstringNC wm;

          wm = allocPermSto(strlen(arg) + suffLgt + (LgtHdrAddedPath + 1));
          InitHdrFName(wm)
          strncpy(wm, arg, lgtPath + posVersNb);
          if (suffLgt != 0) strcpy(wm + lgtPath + posVersNb, CSuffix);
          strcpy(wm + lgtPath + posVersNb + suffLgt, begOfFName +
					   posVersNb);  /* add version number */
          cUnitFName = wm;
          posVersNb += suffLgt;
          strpdCUnitFName = wm = allocPermSto(posVersNb + 1);
          strncpy(wm, cUnitFName + lgtPath, posVersNb);
          *(wm + posVersNb) = '\0';  /* remove possible version number (cf.
                                                          isFNameVisible() ). */
#else
          TstringNC wm;

          wm = allocPermSto(strlen(arg) + (LgtHdrAddedPath + 1));
          InitHdrFName(wm)
          strcpy(wm, arg);
          cUnitFName = wm;
          strpdCUnitFName = begOfFName;
#endif
        }}}
    /* Add argument to compiler command line */
    *ptrCmdBuf++ = ' ';  /* separating space */
    while (*arg!= '\0') *ptrCmdBuf++ = *arg++;
    if (cUnitFName!=NULL && !lastFileFl) break;}
  *ptrCmdBuf = '\0';  /* ending NUL */
  if (!fNameSeen || cUnitFName==NULL && !callCompil) errExit(NoCUName, NULL);
  *ptrDirArray++ = copyDirPrefix(sysHdrDir, strlen(sysHdrDir));
  *ptrDirArray = NULL;
  if (cUnitFName != NULL) {  /* if something to compile, do not skip to link */
    if (! chkIndent) indentIncr = 0;
    boolTypeElt.typeSort = boolCstTypeElt.typeSort = boolTypeElt1.typeSort = 
			  (chkBool)
			   ? (mskBool = Bool|BoolOpnd, Bool)
			   : (mskBool = NumEnumBool|PtrPoss|BoolOpnd, Int|Bool);
    {
      Terr err = CompHdrFile;

      if (isHeaderFile(cUnitFName) || (err = FileOpenFail, !openSrcFile1(
					   cUnitFName))) errWS(err, cUnitFName);
      else {
        cumNbLines = cumDeltaLineNb = cumNbSkippedLines = 0;  /* may have been
				    modified by 'starter' or 'initAdj' files. */
        posLastInclWa.lineNb = 0;
        changeOutStreamTo(MAIN_OUT_STREAM);
        prog();
        cumNbLines += lineNb - 1 + cumDeltaLineNb - cumNbSkippedLines;
        freeTypeCmbn();
        /* Reset list of header files included inside a block */
        while (headListIIB != NULL) headListIIB = freeIncldFElt(headListIIB);
        (void)ctrIIB;}}
    exitBlock();
    checkCondStkAndDeleteMacros();
    closeSource();  /* lineNb not used any more here */ 
    changeOutStreamTo(SUMMARY_STREAM);
    checks();
  }
  free(dirArray);
  if (ctrIgndMsg == 0) frstPass = False;
  if (frstPass) {
    if (ctrIgndMsg < FrstFewMsg) minMsgLvl = 0;  /* so as to report all
								    messages. */
    lastTrtdFName = oldLastTrtdFName;
    return False;}
  if (cUnitFName!=NULL && (emitTrailer || (ctrErr | ctrWarn)!=0)) {
    CreateParArr((4)) /*~DynInit*/;

    parArr4[1] = cUnitFName;
    parArr4[2] = grstIntToS(cumNbLines);
    prmtrzMsg(errTxt[EndMsg], parArr4);
    if ((ctrErr | ctrWarn) != 0) {
      TnbBuf buf, buf1;

      bufGrstIntToS((TgreatestInt)ctrErr, buf);
      parArr4[1] = &buf[0];
      parArr4[2] = grstIntToS((TgreatestInt)ctrWarn);
      parArr4[3] = (ctrIgndMsg == 0)? NULL : (bufGrstIntToS((TgreatestInt)
		    ctrIgndMsg, buf1), parArr4[4] = &buf1[0], errTxt[IgndMsgs]);
      prmtrzMsg(errTxt[ErrWarn], parArr4);}
    else emitS(errTxt[NoErrWarn]);}
  if (ctrWarn != 0) {warnSeen = True; if (! callCompilW) notCompiled = True;}
  if (ctrErr!=0 || existErr || minMsgLvl>=lvlNoCompil) {
    errSeen = True;
    if (callCompil) {
      callCompilSeen = True;
      if (ctrErr == 0) {
        parArr1[1] = grstIntToS((TgreatestInt)lvlNoCompil);
        prmtrzMsg(errTxt[NotCompild], parArr1);}}}
  else if (callCompil && (callCompilW || ctrWarn==0)) {
    int w;

    changeOutStreamTo(NULL);
    w = system(cmdBuf);  /* call compiler */
    if (compilerStatus == EXIT_SUCCESS) compilerStatus = w;}
  return True;
}

static AllocXElt(allocIncldFElt, TincldInBlk, ctrIIB, ;)
static AllocXElt(allocInclStkElt, TinclStkElt, ctrISE, ;)

static void changeOutStreamTo(Tstream x)
{
  static bool errSeen = False;

  if (spaceAtEndOfLine < 0) {emitC('\n'); ResetSpaceAtEndOfLine;}  /* flush
								     emitC(). */
  if (!errSeen && fflush(outputStream)!=0) {errSeen = True; sysErr(ExCod2);}
  if (x != NULL) outputStream = x;
}

static void checks(void)
{
  if (nestLvl != -1) {intrnErr("nestLvl", (uint *)&nestLvl); nestLvl = -1;}
  if (dpragNst != 0) intrnErr("dpragNst", &dpragNst);
  if (ctrBSE > 1)  intrnErr("ctrBlkStkElt",  &ctrBSE);
  if (ctrCE  != 0) intrnErr("ctrCaseElt",    &ctrCE);
  if (ctrCSE != 0) intrnErr("ctrCondStkElt", &ctrCSE);
  if (ctrDE  != 0) intrnErr("ctrDeclElt=",   &ctrDE);
  if (ctrDIE != 0) intrnErr("ctrDescIdElt",  &ctrDIE);
  if (ctrISE != 0) intrnErr("ctrInclStkElt", &ctrISE);
  if (ctrMSB != 0) intrnErr("ctrMacroBlk",   &ctrMSB);
  if (macLvl != 0) intrnErr("ctrMacStkElt",  &macLvl);
  if (ctrNI  != 0) intrnErr("ctrNotInitVarElt", &ctrNI);
  if (ctrNSB > (3) /* 3 because first name block never freed, and two other
			may be kept not freed (see exitBlock() ). */
		 ) intrnErr("ctrNameStoBlk", &ctrNSB);
  if (ctrQL  != 0) intrnErr("ctrQlfdTypesElt", &ctrQL);
  if (ctrSE  != 0) intrnErr("ctrSemanElt",   &ctrSE);
  if (ctrSSL != 0) intrnErr("ctrSavStrLitElt", &ctrSSL);
  if (ctrTCB != 0) intrnErr("ctrTypCombElt", &ctrTCB);
  if (ctrTE  != 0) intrnErr("ctrTypeElt",    &ctrTE);
  if (ctrTLE != 0) intrnErr("ctrTagListElt", &ctrTLE);
  if (ctrUT  != 0) intrnErr("ctrUndfTagElt", &ctrUT);
  intrnErr("", NULL);  /* flush buffer */
}

static void closeSource(void)
{
  free(curSrcMngtBlk); /* do not free curFileName, because name may still
					be used (eg. in a 'NotUsed' warning). */
  curSrcMngtBlk = NULL;
  lineNb = 0;  /* indicates "no valid source line" */
}

static TdirName copyDirPrefix(Tstring string, size_t lgt)
{
  TdirNameNC res;

  if (lgt >= UCHAR_MAXC) {parArr1[1] = string; errExit(StringTooLong, parArr1);}
  res = allocPermSto(lgt + (PosLgtAddedPath + 1));  /* +1 for ending '/' */
  memcpy(res + PosLgtAddedPath, string, lgt);
#ifdef VMS
  *res = (TdirChar)lgt;
#else
  *res = (TdirChar)(lgt + 1);
  *(res + lgt + PosLgtAddedPath) = (TdirChar)'/';
#endif
  return res;
}

static TstringNC createAdjFileName(Tstring fName)
{
  TstringNC fullAdjFileName;
  uint fnLgt = (uint)(SearchDot(fName) - fName) + 1;

  fullAdjFileName = allocPermSto(dccDirLgt + fnLgt + (LitLen(AdjSuffix) + 1));
  strcpy(fullAdjFileName, dccDir);
  strcpy(fullAdjFileName + dccDirLgt, fName);
  strcpy(fullAdjFileName + dccDirLgt + fnLgt, AdjSuffix);
  adjustFile = True;
  return fullAdjFileName;
}

static void dispUsg(void)
{
  TnbBuf buf;
  CreateParArr((4)) /*~DynInit*/;
  bool interactive;
  uint msgNb = 0;
  static Terr msgTbl[] = {Use1, Use2, Use3};

  bufGrstIntToS(LineSize, buf);
  parArr4[1] = *valInitParg;
  parArr4[2] = grstIntToS(DefaultTabSpacing);
  parArr4[3] = &buf[0];
  parArr4[4] = TxtOptions;
  interactive = (isatty(fileno(outputStream)) > 0);
  while (fmtdMsg(msgTbl[msgNb++], parArr4)) {
    if (interactive) {
      fmtdMsg(ProceedMsg, NULL);
      waitAndAnalAnswer();}}
}

#define HyphenDetec	2
static char bufChar[HyphenDetec + LineSize + 1 + 1] = "  ",  /* initialized
					because of hyphen detection; +1 for en-
					ding '\0', +1 for overflow detection. */
            *ptrBufCh;
static bool partWord = False;  /* flag "previous word not complete" */

static void initEmitC(void)
{
  bufChar[0] = bufChar[1] = ' ';  /* initialized because of hyphen detection */
  ptrBufCh = &bufChar[HyphenDetec];
  spaceAtEndOfLine = 0;
  partWord = False;
}

static void emitC(char x)
/* Manages line wrap-around (tries to wrap only when a space or hyphen character
   is seen, while avoiding lines too empty). */
{
#define MyFPuts(buf) {if (fputs(buf, outputStream) == EOF) sysErr(ExCod2);}
  bool ovfl = False;

  if (x != '\n') {
    *ptrBufCh++ = x;
    if (! (x==' ' || x=='-' && isalpha(*(ptrBufCh - HyphenDetec - 1)) &&
				isalpha(*(ptrBufCh - HyphenDetec)))) { /* dash
			supposed to be hyphen only if last but one character is
			letter (not only last because of word "d-pragma"). */
      if (ptrBufCh < AdLastEltP1(bufChar) - 1) return;
      ovfl = True;}}
  {
    char *begBuf = &bufChar[HyphenDetec];
    int nbChar;

    *((x==' ' && ptrBufCh - begBuf==spaceAtEndOfLine + 1)? --ptrBufCh :
			ptrBufCh) = '\0';  /* ending character (remove useless
						       space at end of line). */
    while ((nbChar = (int)(ptrBufCh - begBuf))>spaceAtEndOfLine || ovfl &&
		nbChar==spaceAtEndOfLine) {  /* in case of overflow, keep one
						 slot at end of line for '\'. */
      if (spaceAtEndOfLine>TolLostSpace || (partWord /* previous word not
			 completely written */ || ovfl) && spaceAtEndOfLine>0) {
        char *nxtBegBuf = begBuf + spaceAtEndOfLine - 1, c = *nxtBegBuf;

        *nxtBegBuf = '\0';  /* for fputs */
        if (*begBuf != '\0') {MyFPuts(begBuf)  MyFPuts("\\\n") }
        else if (partWord) MyFPuts("\\\n")
        else MyFPuts("\n")  /* case last character of line is space or hyphen */
        *nxtBegBuf = c;  /* restore smashed character */
        begBuf = nxtBegBuf;}
      else MyFPuts("\n")
      ResetSpaceAtEndOfLine;}
    if (*begBuf != '\0') MyFPuts(begBuf)  /* write (last part of) buffer */
    ptrBufCh = &bufChar[HyphenDetec];  /* buffer empty */
    if (x == '\n') spaceAtEndOfLine = -1;  /* to have NL inserted before next
								    emission. */
    else spaceAtEndOfLine -= nbChar;
    partWord = ovfl;}
#undef MyFPuts
#undef HyphenDetec
}

/*~Undef bufChar, ptrBufCh, partWord */

static void emitCstWdthCh(register const char *bStr, const char *eStr)
/* Emits all character between 'bStr' (included) and 'eStr' (excluded) as
   constant width characters (tabs and non-printable characters become spaces);
   otherwise, LineSize would not be identical to line length. */
{
  char c;

  while (bStr != eStr) {
    c = *bStr++;
    emitC((IsVisibleChar(c) || c == '\n')? c : ' ');}
}

void emitS(Tstring x)
{
  if (x != NULL) {while (*x != '\0') {emitC(*x++);}}
}

void endPrgF(register char *ptrS)
{
  RestoSmshCh;
  ClrSmshCh;
  if (pPPNbCtx != NULL) {  /* end of pp-number suffix */
    srcPtr = pPPNbCtx->savSrcPtr;
    pCurGNT = pPPNbCtx->savPGNT;
    if (pCurGNT != &getTokFromTxt) pNxtCh = &nxtChFromMac;
    macroExpand = pPPNbCtx->savME;
    free(pPPNbCtx);
    pPPNbCtx = NULL;
    GetNxtTok();}
  else {
    if (! fileClosed) ilgCharF(ptrS);
    else {
      BackUp;  /* do not go beyond EndCh */
      if (dirLineFl) {dirLineFl = False; curTok.tok = ENDDIR;}
      else {
        if (pTopInclStk != NULL) {ptrS = manageEndInclude(); curTok.tok =
								    WHITESPACE;}
        else {curTok.tok = ENDPROG; curTok.Val = 0; condDirSkip = False
							  ; noExpand = False;}}}
    srcPtr = ptrS;}
}

static void emitFName(Tstring fileName)
{
  CreateParArr((4)) /*~DynInit*/;

  parArr4[4] = fileName;
  prmtrzMsg(errTxt[File], parArr4);
}

void err(Terr n, const Tstring parArr[])
{
  Terr errNo = n & (EndErrInfoBits -1 -1);
  uint valWarn = (uint)(n & WarnMsk) >> SHIFTWARN;
  bool warn = (valWarn!=0 || InsideDPragma), noDispLine, savGAFN, savMIF;

  if (warn && (!warnFl
               || sysHdrFile  /* no warning in system header file */
               || sysMacro && n&UWarn  /* no Uwarning in system macro */
               || n&PossErr && !chkPossErr
               || n&Rdbl && !chkRdbl
               || n&Effic && !chkEffic)
      || errNo==NoErrMsg
      || valWarn!=(uint)Err && noWarn) return;
  if (ignoreErr) goto ignoreWarnL;
  if (mngConc) {  /* if inside ## operator */
    if (concatErr == NoConcErr) concatErr = errNo;  /* retain first error */
    return;}
  if (InsideMacro) {
    if (curFileName!=cUnitFName && !warn && errNo!=MacBefIncl)
      errMacBefIncl();
    if (curTok.tok==SCOL && (errNo==Msng  || errNo==Exptd || errNo==
				 CommaOrXExptd) && checkSColAtEndMac()) return;}
  if (! InsideDPragma) {
    uint msgLvl = (valWarn == (uint)Err)? (uint)WarnMsk >> SHIFTWARN : valWarn;

    if (msgLvl < minMsgLvl) {
      if (screenMsgPass2 || !allErrFl) goto ignoreWarnL;}  /* screen messages */
    else if (callCompil || frstPass) minMsgLvl = msgLvl;}
  if (frstPass)
ignoreWarnL:
    {ctrIgndMsg++; return;}
  if (ctrWarn + ctrErr >= FrstFewMsg) allErrFl = signalAllErr;
  if (msgLimit < 0) {
    if (! warn) existErr = True;
    goto ignoreWarnL /*~BackBranch*/;}
  if (ctrWarn + ctrErr == 0) {
    emitS(errTxt[WarnErrInFile]); emitS(cUnitFName); emitS("\" *****\n");
    if (stopAfterMsg) {emitS(errTxt[StopAftMsgBanner]); waitAndAnalAnswer();}
    emitC('\n');}
  else {
    emitS(errTxt[SeparMsg]);}
  if (!stopAfterMsg && msgLimit--==0) {
    emitS(errTxt[TooManyMsg]);
    goto ignoreWarnL /*~BackBranch*/;}
  if (warn) ctrWarn++;
  else {
    ctrErr++;
    ignoreErr = (!allErrFl && curTok.tok!=ENDPROG);}
  noDispLine = n & NoDispLine;
  savGAFN = givAlwsFName;
  savMIF = moreInfMsg;
explain1L:
  if (!noDispLine && (lineNb!=0 || dirLineFl)) {  /* not no printing of line */
    char *ptrS;
    const char *expl;
    ptrdiff_t distErrBegLine;  /* nb of characters between the beginning of the
				      line and the error position (included). */
    char savInfoId[LgtHdrId];

    if (adSmshCh != NULL) {   /* put back in place possible characters smashed
					  by an identifier length/name space. */
      register char *w = adSmshCh;

      savInfoId[0] = *w; *w = smshCh[0];
      savInfoId[1] = *(w + 1); *(w + 1) = smshCh[1];}
    ptrS = (pPPNbCtx != NULL)? pPPNbCtx->savSrcPtr : srcPtr;
    if ((distErrBegLine = ptrS - nlPosP1) < 0) distErrBegLine += oldMaxPtrS -
				       begSBuf; /* because of circular buffer */
    if (distErrBegLine <= LineSize) {
      /* Beginning of line not very far from error position, so (end of) prece-
         ding line, and (beginning of) current line, can be displayed. */
      if (frstLineOfFile) expl = nlPosP1;
      else {
        /* Search beginning of preceding line; 'expl' points on it */
        uint ctr;

        for (expl = nlPosP1 - 1, ctr = LS2 - 1; ctr != 0; ctr--) {
          if (expl == begSBuf) expl = oldMaxPtrS;
          if (*--expl == '\n') goto foundNLL;}
        /* Beginning of preceding line not visible */
        emitS(Dots);
        expl += LitLen(Dots);
foundNLL:
        if (++expl >= oldMaxPtrS) expl -= oldMaxPtrS - begSBuf;}
      ptrS = searchNxtNL(ptrS, distErrBegLine);}  /* (local copy of) ptrS
							moved to end of line. */
    else {
      /* Beginning of line too far from error position; only current line
         (broken into two physical lines, the first one being terminated by '\'
         to indicate logical continuation) can be displayed; 'expl' points on
         first char of second physical line. */
      const char *begErrLine;  /* pointer on 1st char of first physical line */

      if (distErrBegLine <= NbSrcLinesKept*LineSize - 1) {  /* - 1 because of
								  ending '\'. */
        /* Beginning of current line visible */
        distErrBegLine -= LineSize - 1;
        begErrLine = nlPosP1;
        expl = begErrLine + (LineSize - 1);
        ptrS = searchNxtNL(ptrS, distErrBegLine);}  /* (local copy of) ptrS
							moved to end of line. */
      else {      /* beginning of current line not visible : put error
				     position at end of second physical line. */
        emitS(Dots);
        distErrBegLine = LineSize;
        if ((begErrLine = ptrS - (NbSrcLinesKept*LineSize - (LitLen(Dots)) - 1
		    /* - 1 because of ending '\' */)) < begSBuf) begErrLine +=
							   oldMaxPtrS - begSBuf;
        expl = begErrLine + (LineSize - (LitLen(Dots)) - 1);}
      /* Print first part of source line */
      if (expl >= oldMaxPtrS) expl -= oldMaxPtrS - begSBuf;
      if (expl < begErrLine) {emitCstWdthCh(begErrLine, oldMaxPtrS); begErrLine
								     = begSBuf;}
      emitCstWdthCh(begErrLine, expl);
      emitS("\\\n");}
    /* Print (second part of) source line */
    if (ptrS < expl) {emitCstWdthCh(expl, oldMaxPtrS); expl = begSBuf;}
    emitCstWdthCh(expl, ptrS);
    /* Print caret under error position */
    emitC('\n');
    if (distErrBegLine != 0)
      while (--distErrBegLine != 0) {emitC('_');}
    emitS("^\n");
    if (adSmshCh != NULL) {   /* put back in place possible identifier
							   length/name space. */
      register char *w = adSmshCh;

      *w = savInfoId[0]; *(w + 1) = savInfoId[1];}}
explainL:
  if (warn) {
    emitS(errTxt[Warning]);
    emitC('0' + (int)((valWarn == (uint)Err)
	? ((uint)WarnMsk >> SHIFTWARN) + ((uint)Warn1 >> SHIFTWARN)
	: valWarn));
    emitC(')');}
  else emitS(errTxt[Error]);
  if (!noDispLine || givAlwsFName) {  /* print file name/line number */
    TnbBuf bufLineNo;

    emitS(errTxt[Line]);
    bufGrstIntToS(lineNb, bufLineNo);  /* so 'grstIntToS' buffer remains
							       uncorrupted... */
    emitS(bufLineNo);
    if (curFileName!=cUnitFName || givAlwsFName) emitFName(curFileName);
    if (pTopInclStk!=NULL && givAlwsFName) {  /* give including file(s) */
      const TinclStkElt *w = pTopInclStk;

      emitS(" (");
      do {
        emitS(errTxt[IncldBy]);
        emitS(errTxt[Line]);
        bufGrstIntToS(w->srcMngtBlk->lineNb, bufLineNo);  /* so 'grstIntToS'
						buffer remains uncorrupted... */
        emitS(bufLineNo);
        emitFName(w->fileName);
        emitS(((w = w->prev) == NULL)? ")" : ", ");
      } while (w != NULL);}}
  emitS(": ");
  prmtrzMsg(errTxt[errNo], parArr);  /* parametrized error message */
  if (listTok && !noDispLine && (!stopAfterMsg || moreInfMsg)) {  /* output
				 last tokens taken into account before error. */
    emitS(errTxt[LastToks]);
    if (ovfldTokBuf) {emitS(Dots); emitCstWdthCh(pLastTokBuf, endLastTokBuf);}
    emitCstWdthCh(begLastTokBuf, pLastTokBuf);}
  emitC('\n');
  if (stopAfterMsg && waitAndAnalAnswer()) {  /* explanation requested */
    givAlwsFName = moreInfMsg = True;
    if (! noDispLine) goto explainL /*~BackBranch*/;
    noDispLine = False;
    goto explain1L /*~BackBranch*/;}
  givAlwsFName = savGAFN;
  moreInfMsg = savMIF;
}

static void errExit(Terr n, const Tstring *parArr) /*~NeverReturns*/
{
  if (n != NoErrMsg) fmtdMsg(n, parArr);
  flushAndExit();
}

static void fatalErr(Terr n, Tstring x) /*~NeverReturns*/
{
  changeOutStreamTo(SUMMARY_STREAM);
  mngConc = ignoreErr = frstPass = False;
  errWS(n, x);
  flushAndExit();
}

static void flushAndExit(void) /*~NeverReturns*/
{
  changeOutStreamTo(NULL);
  exit(EXIT_FAILURE);
}

static bool fmtdMsg(Terr n, const Tstring *parArr) /*~PseudoVoid*/
{
  bool resul;
#ifndef VMS
  static bool frstTime = True;

  if (frstTime) {emitC('\r'); frstTime = False;}
#else

#endif
  resul = prmtrzMsg(errTxt[n], parArr);
  emitC('\n');
  return resul;
}

static bool foundOption(Tstring opt, bool *pFl)
{
  size_t lgtOpt = strlen(opt);

  if (*opt!=parArr1[1][LitLen(DccOptPref)] || strncmp(parArr1[1] + LitLen(
				     DccOptPref), opt, lgtOpt)!=0) return False;
  *pFl = (*parArr1[1] == '+');
  {
    register const Tchar *ptr = &parArr1[1][lgtOpt + LitLen(DccOptPref)];

    if (*ptr != '\0') {  /* there exist an 'appendix' to the option */
      optionValue = 0;
      do {
        if (! isdigit(*ptr)) {optionValue = DfltOptVal - 1; break;}
        optionValue = optionValue*Base10 + (*ptr++ - '0');
      } while (*ptr != '\0');}}
  return True;
}

static FreeXElt(freeIncldFElt, TincldInBlk *, ctrIIB, ; , prev)
static FreeXElt(freeInclStkElt, TinclStkElt *, ctrISE, ; , prev)
static FreeXElt(freeUndfTagElt, TundfTagElt *, ctrUT, ; , prev)

static Tstring getNxtArg(void)
/* Gives back pointer on next argument (NULL at end) */
{
  Tstring begArg;

  while (isspace(*dfltArgsPtr)) dfltArgsPtr++;  /* skip possible starting
								     spaces. */
  if (*dfltArgsPtr == '\0' /* environment string exhausted */) return *++pArg;
  begArg = dfltArgsPtr;
  while (*++dfltArgsPtr != '\0') {  /* search end of parameter */
    if (isspace(*dfltArgsPtr)) {
      *dfltArgsPtr++ = '\0';  /* put parameter ending */
      break;}}
  return begArg;
}

static Tstring getNxtFName(void)
{
  Tchar *w;
  Tstring ptrD;
  TlitString ptrO;

  if ((ptrO = getLitString()) == NULL) ptrD = NULL;
  else {
    MyAlloc(w, initGetStrLit(ptrO));
    ptrD = w;
    while ((*w++ = nxtStrLitChar()) != '\0') {}  /* save file name in
							     contiguous area. */
    resetGetStrLit();}
  exitBlock();  /* because of 'getLitString()' */
  return ptrD;
}

static void initAdjFiles(void)
{
  if (totalSizAdjArr != 0) return;  /* already initialized */
  if (! openSrcFile1(adjFiles)) {totalSizAdjArr = 1; return;}
  while (NxtTok() != ENDPROG) {
    if (usedSizAdjArr == totalSizAdjArr) {  /* array full; extend it */
#define IncrSizeAdjArr 5
      totalSizAdjArr += IncrSizeAdjArr;
#undef IncrSizeAdjArr
      MyRealloc(adjFilesArr, sizeof(TadjFilesDesc) * totalSizAdjArr);}
    {
      Tstring w;

      adjFilesArr[usedSizAdjArr].noLoadSysFile = Found(EMARK);
      if ((w = getNxtFName()) == NULL) skipTok(zSCol);
      else {
        adjFilesArr[usedSizAdjArr].amendFName = w;
        if (! Found(COMMA)) errWS(Exptd, ",");
        ComputeFileHCode(w);
          adjFilesArr[usedSizAdjArr].hCode = fileHCode;}
        if ((w = getNxtFName()) == NULL) skipTok(zSCol);
        else adjFilesArr[usedSizAdjArr++].adjFName = w;}}
    if (curTok.tok != SCOL) errWS(Exptd, ";");}
  closeSource();
}

static void initAll(void)
{
  initBlk();
  initDecl();
  initDir();
  initExp();
  initExt();
  initInst();
  initMain();
  initPrag();
  initRec();
}

static void initMain(void)
{
  static bool frstInit = True;

  if (frstInit) {
    if (setvbuf(MAIN_OUT_STREAM, NULL, (int)_IOFBF, BUFSIZ) != 0) sysErr(
									ExCod1);
    frstInit = False;}
  ctrIgndMsg = 0;
  listTok = False;  /* do not list starterFile tokens */
  allErrFl = True;
  if (! frstPass) {
    resetOptions();
    ctrErr = ctrWarn = 0;
    existErr = False;
    minMsgLvl = 0;}
}

bool insideIncldFile(void)
{
  return pTopInclStk != NULL;
}

static void intrnErr(Tstring x, uint *pCtr)
{
  static bool errSeen = False;

  if (pCtr == NULL) {
    if (errSeen) {emitC(' '); errSeen = False;}}  /* flush buffer */
  else {
    if (! frstPass) {
      emitS("\nINTERNAL PB: "); emitS(x); emitS("= "); emitS(grstIntToS(
							  (TgreatestInt)*pCtr));
      errSeen = True;}
    *pCtr = 0;}
}

static bool isDccOption(Tstring arg)
{
  return (*arg=='+' || *arg=='-') && *(arg + 1)=='z' && strlen(arg)>=
								   MinDccOptLgt;
}

static bool isHeaderFile(Tstring fileName)
{
  Tstring p = SearchDot(fileName);


  if (p == NULL) return False;
  if (strchr(p, 'h') != NULL) return True;
#ifdef LcEqUc
  if (strchr(p, 'H') != NULL) {
    if (chkPortbl) err0(LowerCaseH | Warn1);
    return True;}
#endif
  return False;
}

static bool isSrcFName(Tstring x)
{
  return !isDccOption(x) &&
#ifdef VMS
			    *x!='/';
#else
			    *x!='-' && StrEq(x + strlen(x) - LitLen(CSuffix),
								       CSuffix);
#endif
}

static const TdirName *savPtrDirArr = NULL;

static char *manageEndInclude(void)
{
  TinclStkElt *locPTInclS = pTopInclStk;

  if (! adjustFile) cumNbLines += lineNb - 1;
  if (isHeaderFile(curFileName)) {
    /* Check whether there remains undefined strun tags */
    while (headUndfTagsList != NULL) {
      if (! headUndfTagsList->pTagId->Defnd) errId1(TagShdBeDfnd|Warn1|Rdbl,
					 headUndfTagsList->pTagId, curFileName);
      headUndfTagsList = freeUndfTagElt(headUndfTagsList);}
    headUndfTagsList = locPTInclS->headUTList;}
  closeSource();
  /* Search if system header file to be 'amended' */
  if (sysHdrFile) {
    Tstring nakedFName = IndicatedFName(curFileName); /* skip prefix */

    /* Search if not recursive inclusion (because adjustFiles can only be
       included at end of system file). */
    {
      const TinclStkElt *w = locPTInclS;

      while (w->ShdrFile) {
        if (StrEq(IndicatedFName(w->fileName), nakedFName)) goto recursL;
        w = w->prev;}}
    sysHdrFile = False;
    /* First look in DccExecFilesDir directory for possible adjustment file */
    if (
#ifdef LcEqUc
        sameLowerCaseName(SysHdrDir, nakedFName, False)
#else
        strncmp(nakedFName, SysHdrDir, LitLen(SysHdrDir)) == 0
#endif
		 ) nakedFName += LitLen(SysHdrDir);  /* remove redundant path */
    if (openSrcFile1(createAdjFileName(nakedFName))) return srcPtr;
    freeLastPermSto();
    /* Then search if adjustment file not elsewhere */
    {
      const TadjFilesDesc *ptrAdjDesc;
#ifdef LcEqUc
      bool diffLC = False;

      SearchAdjFiles(nakedFName, (diffLC = sameLowerCaseName(nakedFName,
						 ptrAdjDesc->amendFName, True)))
            if (diffLC) locPTInclS->DiffLc = True;
#else

      SearchAdjFiles(nakedFName, False)
#endif
            openSrcFile(ptrAdjDesc->adjFName);
            return srcPtr;}}}}
recursL:;}
  adjustFile = False;
  /* Go back to previous source file */
  curFileName = locPTInclS->fileName;
  curHdrFName = locPTInclS->curHdrFName;
  insideHdrFile = locPTInclS->InsideHdrFile;
  insideHdrInHdr = locPTInclS->InsideHdrInHdr;
  curSrcMngtBlk = locPTInclS->srcMngtBlk;
  oldMaxPtrS = curSrcMngtBlk->oldMaxPtrS;
  posEndChP1 = curSrcMngtBlk->posEndChP1;
  nxtPosEndChP1 = curSrcMngtBlk->nxtPosEndChP1;
  nlPosP1 = srcPtr = curSrcMngtBlk->ptrS;  /* because of call to err()
						      later in this function. */
  lineNb = curSrcMngtBlk->lineNb;
  srcStream = curSrcMngtBlk->srcStream;
  headListPrivTo = curSrcMngtBlk->listPriv;
  savPtrDirArr = curSrcMngtBlk->curInclDir;
  begSBuf = &curSrcMngtBlk->idPlusSrc[LgtHdrId + MaxLgtId];
  endSBuf = begSBuf + SizeSrcBuffer;
  restoInclInfoAndPopStk(locPTInclS);
#ifdef LcEqUc
  if (locPTInclS->DiffLc && chkPortbl) err0(ChkUcLc | Warn1);  /* call to err()
					must work anew, so have to come last. */
#endif
  fileClosed = False;
  return srcPtr - 1;  /* -1 to get back to nlF() */
}

void processInclude(
#ifdef VMS
		   TstringNC
#else
		   Tstring
#endif
			     fileName, bool sysFl, bool cmpsgHdr, bool inclNxt)
{
  bool headerFile = isHeaderFile(fileName), prevFileSysHdr;
  TinclStkElt *newPTIS = allocInclStkElt();
  TnstLvl locNstLvl = nestLvl;

  /* Save previous context */
  curSrcMngtBlk->ptrS= srcPtr;
  curSrcMngtBlk->oldMaxPtrS = oldMaxPtrS;
  curSrcMngtBlk->posEndChP1 = posEndChP1;
  curSrcMngtBlk->nxtPosEndChP1 = nxtPosEndChP1;
  curSrcMngtBlk->lineNb = lineNb;
  curSrcMngtBlk->srcStream = srcStream;
  curSrcMngtBlk->listPriv = headListPrivTo;
  curSrcMngtBlk->curInclDir = savPtrDirArr;
  newPTIS->srcMngtBlk = curSrcMngtBlk;
  newPTIS->fileName = curFileName;
  newPTIS->curHdrFName = curHdrFName;
  newPTIS->InsideHdrFile = insideHdrFile;
  newPTIS->InsideHdrInHdr = insideHdrInHdr;
  newPTIS->ShdrFile = prevFileSysHdr = sysHdrFile;
#ifdef LcEqUc
  newPTIS->DiffLc = False;
#endif
  newPTIS->IndentIncr = indentIncr;
  newPTIS->prev = pTopInclStk;
  /* Test for errors/warnings while err() stills works */
  {
    Tstring p = SearchDot(fileName);

    if (p!=NULL && StrEq(p+1, "c")) errWS(DontInclBodyFile | Warn2, fileName);}
  if (cmpsgHdr) {
    if (!headerFile || !insideHdrFile) err0(IlgCmpsgHdr | Warn1);
    else if (isBodyHdrFile(fileName) && !isBodyHdrFile(curHdrFName)) errWS(
					     IlgCmpsgHdr1|Warn1|Rdbl, fileName);
    else goto OkL;
    cmpsgHdr = False;
OkL:;}
  if (headerFile) {
    /* Search whether file previously included, but inside a block */
    {
      register const TincldInBlk *w;

      for (w = headListIIB; w != NULL; w = w->prev) {
        if (StrEq(w->includedFName, fileName)) {
          TposInSrc wp = srchPosInclLvl0(newPTIS);

          if ((wp.lineNb!=w->lineNb || wp.fileName!=w->includingFName)
              && (wp.lineNb!=posLastInclWa.lineNb || wp.fileName!=
						      posLastInclWa.fileName)) {
            errWFName(AlrdIncldInBlk|Warn3|PossErr, w->lineNb, w->
					  includingFName, NULL, fileName, NULL);
            posLastInclWa.lineNb = wp.lineNb;
            posLastInclWa.fileName = wp.fileName;}  /* to prevent multiple
								      errors. */
          locNstLvl = 0;  /* to prevent adding again to list */
          break;}}}
    /* Search if circular chain of inclusion of header files */
    {
      const TinclStkElt *w;
      Tstring cmpsgHdrFName = NULL;

      for (w = newPTIS; *w->curHdrFName != '\0' /*not nonHdrFName */; w = w->
									 prev) {
        Tstring curFName = IndicatedFName(w->fileName);

        if (w->curHdrFName!=curFName && isHeaderFile(curFName)) cmpsgHdrFName =
				 curFName;  /* returning to ComposingHdr file */
        if (cmpsgHdrFName!=NULL || cmpsgHdr) {
          if (StrEq(curFName, fileName)) {
            errWSS(CmpsgHdrWithSelf | Warn1, fileName, cmpsgHdrFName);
            break;}}}}}
  if (inclNxt && savPtrDirArr==NULL) {err0(InclNxtIlg); inclNxt = False;}
  if (nestLvl != 0) indentIncr = 0;  /* to avoid 'BadIndent' errors in
						files included inside blocks. */
  if (sysFl) sysHdrFile = sysAdjHdrFile = True;
  if (headerFile && sysHdrFile) {
    /* Check if adjust file is to replace header file */
    const TadjFilesDesc *ptrAdjDesc;

#ifdef LcEqUc
    SearchAdjFiles(fileName, sameLowerCaseName(fileName, ptrAdjDesc->
							      amendFName, True))
#else
    SearchAdjFiles(fileName, False)
#endif
          if (ptrAdjDesc->noLoadSysFile) {
            adjustFile = True;
            openSrcFile(ptrAdjDesc->adjFName);
            goto openOkL;}
          break;}}}}
  /* Open included file */
#if defined(VMS) && defined(__ALPHA)
prefixRmvdL:
#endif
  {
    Tchar *fNameBuf;
    static const TdirName noPrefix[] = {EmptyDirName, EmptyDirName , NULL};  /* 'Emp-
			   tyDirName' twice, because of quoted include files. */
    const TdirName *ptrDirArray = (
#ifdef VMS
				   skipPath(fileName) != fileName
#else
				   *fileName == '/'
#endif
				  )? &noPrefix[0]
				   : (sysFl)
				     ? dirArray + 1
				     : dirArray;
    size_t lgtFileN = strlen(fileName) + 1;  /* '+1' for trailing '\0' */
    size_t lgtPrefix;
#ifdef VMS
    Tchar *pos;

    /* Manage 'dir/file' form (=>'dir:file') */
    if ((pos = strchr(fileName, '/')) != NULL) {
      *pos = ':';  /* replace '/' by ':' */
      ptrDirArray = &noPrefix[0];}
#endif

    if (inclNxt) ptrDirArray = savPtrDirArr;
    /* Try possible prefixes to open include file (and make a permanent copy
       of its name). */
    for (;;) {
      /* Create full file name */
      if (*ptrDirArray != NULL) {
        lgtPrefix = LgtD(*ptrDirArray) + PosLgtAddedPath;
        fNameBuf = allocPermSto(lgtPrefix + lgtFileN + (PosCmpsgFlg -
							      PosLgtAddedPath));
        memcpy(fNameBuf + (PosCmpsgFlg - PosLgtAddedPath), *ptrDirArray++,
								     lgtPrefix);
        strcpy(fNameBuf + lgtPrefix + (PosCmpsgFlg - PosLgtAddedPath),fileName);
        if (openSrcFile1(fNameBuf + LgtHdrAddedPath)) break;
        freeLastPermSto();}
      if (*ptrDirArray==NULL || !sysHdrFile && *(ptrDirArray + 1)==NULL /* do
		     not look in system directory for quoted include file. */) {
#if defined(VMS) && defined(__ALPHA)
        /* Remove possible part before '/' */
        if (pos != NULL) {fileName = pos + 1; goto prefixRmvdL /*~BackBranch*/;}
#endif
#ifdef VMS
        if (sysHdrFile) errWS(UnknSysHdrFile, fileName);
        else
#endif
/**/    errWS(FileOpenFail, fileName);
        restoInclInfoAndPopStk(newPTIS);
        return;}}  /* if no file can be opened, ignore '#include' */
#ifdef VMS
    if (pos != NULL) *((TstringNC/*~OddCast*/)curFileName + (pos - fileName))
					 = '/';  /* restore '/' replacing ':' */
#endif
    savPtrDirArr = ptrDirArray;
    *fNameBuf = (Tchar)cmpsgHdr;}
openOkL:
  if (headerFile) {
    if (locNstLvl != 0) {  /* header file included inside block: note it */
      register TincldInBlk *newIncldElt  = allocIncldFElt();
      TposInSrc wp = srchPosInclLvl0(newPTIS);

      newIncldElt->includedFName = IndicatedFName(curFileName);
      newIncldElt->lineNb = wp.lineNb;
      newIncldElt->includingFName = wp.fileName;
      newIncldElt->prev = headListIIB;
      headListIIB = newIncldElt;}
    if (! prevFileSysHdr) {  /* would-be header file (system header files
							  considered 'flat'). */
      if (! cmpsgHdr) {  /* real header file */
        curHdrFName = IndicatedFName(curFileName);
        if (insideHdrFile) {
          insideHdrInHdr = True;  /* header file included inside header file */
          goto notIncldAtLvl0L;}
        insideHdrFile = True;}
      else {  /* "composing" header */
        if (insideHdrInHdr) goto notIncldAtLvl0L;
        /* Composed header included at level 0 => composing header behaves as if
           included at level 0. */}
      (void)addLvl0InclFName(IndicatedFName(curFileName));  /* jot down files
							 included at level 0. */
notIncldAtLvl0L:
      headListPrivTo = NULL;}  /* reset PrivateTo */
    newPTIS->headUTList = headUndfTagsList;
    headUndfTagsList = NULL;}
  pTopInclStk = newPTIS;  /* only done here because of err() */
  lineNb--;  /* because of extra 'newline' put by end of directive processing */
}

/*~Undef savPtrDirArr */

static void mngOptListTok(void)
{
  if (begLastTokBuf != NULL) {free(begLastTokBuf); begLastTokBuf = NULL;}
  if (listTok) {
    MyAlloc(begLastTokBuf, lgtLastTokBuf);
    endLastTokBuf = begLastTokBuf + lgtLastTokBuf;
    pLastTokBuf = begLastTokBuf;
    ovfldTokBuf = False;
    getTokLvl =  0;}
  else getTokLvl = 1;  /* so, no storing */
  tokLvl0 = getTokLvl + 1;
}

static void mngOptMsgLimit(void)
{
  msgLimit = (msgLimitFl)
	     ? initMsgLimit
	     : (interactivOutptDev)
	       ? DefaultMsgLimit
	       : INT_MAX;
}

static void mngOptStopAfterMsg(void)
{
  if (stopAfterMsg = initStopAfterMsg) 
    if (! interactivOutptDev) stopAfterMsg = False;
    else verbose = HalfVerbo;
}

static void mngOptVerbose(void)
{
  verbose = (initVerbose)
		? (moreInfMsg = True /* 'zvve' implies 'zve' */, FullVerbo)
		: (moreInfMsg)
		  ? HalfVerbo
		  : Terse;
}

TcharStream nxtChFromTxt(void)
{
  char c;
  char *ptrS = srcPtr;

  NxtCh
  srcPtr = ptrS;
  return (TcharStream)c;
}

char *nxtChunkOfTxt(register char *ptrS)
/* Loads next chunk of source text, managing the circular source buffer.
   At entry, ptrS points just after ending EndCh. */
{
  int lgt;	/* (max) length of chunk of text to load (including
							     terminating \0). */
  size_t readLgt;	/* length of chunk of text really loaded */

  ptrS--;		/* eliminate previous terminating EndCh */
  if (ptrS == oldMaxPtrS) {  /* if next text chunk already loaded (by a
					preceding searchNxtNL() or splice()). */
    posEndChP1 = nxtPosEndChP1;
    return begSBuf;}
  lgt = (int)(ptrS - begSBuf) - deltaPtrS - (LineSize + LS2);  /* preserve
			   (maximum) number of characters needed before ptrS. */
  if (nlPosP1 <= ptrS) {  /* normal case (no 'circular buffer' effect) */
    int minLgt = (int)(nlPosP1 - begSBuf) - LS2;

    if (lgt < minLgt) lgt = minLgt;}  /* free space before beginning of
							      preceding line. */
  if (lgt >= 0) {
    if (endSBuf - ptrS > TransfSize) {  /* if enough space at end of buffer,  */
      lgt = (int)(endSBuf - ptrS);      /*   use it (in order to maximise
								 oldMaxPtrS). */
      oldMaxPtrS = endSBuf;}  /* to prevent oldMaxPtrS from indicating 'already
							  loaded' text chunk. */
    else {	/* load at beginning of buffer */
      oldMaxPtrS = ptrS;  /* remember (logical) end of circular buffer */
      if (nlPosP1 > ptrS) nlPosP1 = begSBuf + 1; /* to force subsequent reset,
				for nlPosP1 becomes meaningless in that case. */
      ptrS = begSBuf;}}
  else {  /* characters to be kept start at end of circular buffer */
    int minLgt;

    lgt += (int)(oldMaxPtrS - ptrS);
    minLgt = (int)(nlPosP1 - ptrS) - LS2;
    if (lgt < minLgt) lgt = minLgt;}
  if ((uint)(adSmshCh - ptrS) < (uint)lgt) ClrSmshCh;  /* if last identifier
							 will be overwritten. */
  if ((uint)(nlPosP1 - 1 - ptrS) < (uint)lgt) nlPosP1 = begSBuf - NbSrcLinesKept
		      *LineSize; /* if NL is overwritten, its position becomes
			  meaningless (see use of 'distErrBegLine' in err()). */
  readLgt = fread(ptrS, 1, (size_t)(lgt - 1), srcStream);
  if (readLgt==0 || ferror(srcStream)) {
    if (ferror(srcStream) | fclose(srcStream)!=0 /* '|' so that fclose always
		   called. */) {srcPtr = ptrS; errWS(InptFileErr, curFileName);}
    fileClosed = True;
    *ptrS = EndCh;
    posEndChP1 = NULL;}
  else {
    register char *endBuf = ptrS + readLgt;

    *endBuf++ = EndCh;  /* end of loaded text */
    posEndChP1 = endBuf;}
  return ptrS;
}

static void openSrcFile(Tstring fileName)
{
  if (! openSrcFile1(fileName)) fatalErr(FileOpenFail, fileName);
}

static bool openSrcFile1(Tstring fileName)
{
  Tstream w;

  if ((w = fopen(fileName, "r")) == NULL) return False;
  srcStream = w;
  fileClosed = False;
  curFileName = fileName;
  MyAlloc(curSrcMngtBlk, sizeof(TsrcMngtBlk));
  begSBuf = &curSrcMngtBlk->idPlusSrc[LgtHdrId + MaxLgtId];  /* reserve an
	identifier name buffer in front of the source buffer, buffer used in
	case an identifier straddles both ends of (circular) source buffer,
	because nameStrings must always be contiguous. */
  {
    register char *w;

    srcPtr = w = begSBuf + (SizeSrcBuffer - 1 - 1);
    *w++ = '\n';  /* simulate initial NL (to be able to manage preprocessor
						    directive on first line). */
    *w++ = EndCh;	/* simulate end of loaded text */
    oldMaxPtrS = posEndChP1 = endSBuf = w;}
  lineNb = 0;
  frstLineOfFile = True;
  return True;
}

static bool prmtrzMsg(Tstring x, const Tstring *tabPrm) /*~PseudoVoid*/
{
  register char c;
  static char lastChar;
  uint w;
  static bool skip;
  static int lvl = -1;
  bool resul = False;

  if (x != NULL) {
    if (++lvl == 0) skip = False;  /* start in no-skipping mode */
    while ((c = *x++) != '\0') {
      if (c == '\xFF') resul = True;
      else if (c=='@' && *x=='0') {
        if (! moreInfMsg) skip = !skip;
        x++;}
      else if (! skip)
        if (c=='@' && tabPrm!=NULL && (w = (uint)(*x - '1'))<(uint
	      /*~OddCast*/)*tabPrm) {prmtrzMsg(*(tabPrm + w + 1), tabPrm); x++;}
        else emitC(lastChar = c);}
    if (--lvl<0 && skip && lastChar!='?') emitC('.');}  /* message ending
								      period. */
  return resul;
}

static void resetArgPtr(void)
{
  dfltArgsPtr = strcpy(copDfltArgs, PrefixDCCDFLTS);  /* copied because modi-
							  fied (' ' => '\0'). */
  strcpy(copDfltArgs + LitLen(PrefixDCCDFLTS), dfltArgs);
  pArg = valInitParg;
}

static void resetOptions(void)
{
  askConstFl = callCompil = callCompilW = chk1stCharVF = chkBool = chkEffic =
   chkFctRetTyp = chkInclPos = chkIndent = chkNumCst = chkPortbl = chkPossErr =
   chkRdbl = chkTypName = emitTrailer = goodPrgmg = initStopAfterMsg = warnFl =
   warnNUI = warnNUP = wrapFl = True;
  chkEmptParList = chkNmdTypParFl = givAlwsFName = initVerbose = kwAsMacName =
  moreIndexTypeChk = moreInfMsg = msgLimitFl = noWarn = printStat = chkUCTags =
  reportHghsWaFl = verifAllPtrForConst = signalAllErr = False;
  tabSpacing = DefaultTabSpacing;
  initMsgLimit = DefaultMsgLimit;
  lvlNoCompil = ((uint)WarnMsk>>SHIFTWARN) + 1;
  mngOptListTok();
  mngOptMsgLimit();
  mngOptVerbose();
  mngOptStopAfterMsg();  /* to come after mngOptVerbose() */
}

static void restoInclInfoAndPopStk(TinclStkElt *locPTInclS)
{
  indentIncr = locPTInclS->IndentIncr;
  sysHdrFile = sysAdjHdrFile = locPTInclS->ShdrFile;
  pTopInclStk = freeInclStkElt(locPTInclS);
}

#ifdef LcEqUc
static bool sameLowerCaseName(const char *x, const char *y, bool identity)
{
  for (;;) {
    if (*x == '\0') return (!identity || *y=='\0');
    if (RealChar(*x) != RealChar(*y)) return False;
    x++; y++;}
}
#endif

void saveTokChar(char c)
{
  if (pLastTokBuf == endLastTokBuf) {pLastTokBuf = begLastTokBuf; ovfldTokBuf
									= True;}
  *pLastTokBuf++ = c;
}

static char *searchNxtNL(register char *ptrS, ptrdiff_t ctr)
/* Search until next NL, or until seen LineSize chars on error line; returns
   pointer on this character. */
{
  char c, *savPECP1 = NULL, *oldPECP1 = NULL;

  if (ctr == 0) {deltaPtrS = 1; ctr = 1;}
  else {
    if (ptrS == begSBuf) {ptrS = oldMaxPtrS; oldPECP1 = posEndChP1; posEndChP1
								    = ptrS + 1;}
    BackUp;}
  do {
    deltaPtrS++;  /* to limit length of loaded text chunk, so as not to destroy
		     last NL. Value = 1 too much, so can also serve as flag (in
		     nxtChunkOfTxt()). */
    for (;;) {  /* get next character; may cause (but only once) loading of
							     next text chunk. */
      if ((c = *ptrS++)==EndCh && ptrS==posEndChP1) {
        if (oldPECP1 == NULL) savPECP1 = posEndChP1;
        else {savPECP1 = oldPECP1; oldPECP1 = NULL;}
        ptrS = nxtChunkOfTxt(ptrS);
        if (ptrS != begSBuf) savPECP1 = NULL;  /* no circular effect */
        continue;}
/*    if (c == '\?') {          *//* trigraph? *//*
      while ((c = *ptrS++)==EndCh && ptrS==posEndChP1) {ptrS = nxtChunkOfTxt(
                                                                         ptrS);}
      if (c == '\?') ptrS = trigraph(ptrS, &c);*//* *doIt* c not ds reg *//*
      else {BackUp; c = '\?';}
      break;}*/
      if (c!='\\' || !splice(ptrS)) break; ptrS--;}  /* splicing? */
    if (c=='\n' || c==EndCh && fileClosed) break;  /* found NL (or end of
								source file). */
  } while (ctr++ <= LineSize);
  BackUp;
  deltaPtrS = 0;  /* back to normal case */
  if (savPECP1 != NULL) {
    nxtPosEndChP1 = posEndChP1;  /* because of possible splice() */
    posEndChP1 = savPECP1;}
  else if (oldPECP1 != NULL) posEndChP1 = oldPECP1;
  return ptrS;
}

bool splice(register char *ptrS)
/* On entry we have just read a backslash, and ptrS points at the next
   character. */
{
  if (*ptrS==EndCh && ptrS+1==posEndChP1) {  /* end of buffer */
    char *savPECP1 = posEndChP1;

    ptrS = nxtChunkOfTxt(ptrS + 1);
    if (ptrS == begSBuf) {nxtPosEndChP1 = posEndChP1; posEndChP1 = savPECP1;}}
							   /* circular effect */
  if (*ptrS != '\n') return False;
  /* Line splicing: remove '\\' and '\n' by shifting end of buffer */
  {
    char *savPECP1 = NULL;

    if (ptrS == begSBuf) {  /* circular effect */
      savPECP1 = posEndChP1;
      posEndChP1 = nxtPosEndChP1;
      nxtPosEndChP1 -= LitLen("\\\n");
      *(oldMaxPtrS - 1) = *++ptrS;}
    while (! (*ptrS++==EndCh && ptrS== posEndChP1)) {
      *(ptrS - LitLen("\\\n")) = *ptrS;}
    posEndChP1 = (savPECP1 != NULL)? savPECP1 : ptrS - LitLen("\\\n");}
  lineNb++;
  return True;
}

static TposInSrc srchPosInclLvl0(const TinclStkElt *w)
/* Search level 0 including file */
{
  TposInSrc result;

  while (*w->curHdrFName != '\0' /* not nonHdrFName */) {w = w->prev;}
  result.fileName = w->fileName;
  result.lineNb = w->srcMngtBlk->lineNb;
  return result;
}

static void storeDMacTxt(Tchar c)
{
  if (pStoDMacTxt < endDMacBuf) *pStoDMacTxt++ = c;
}

void sysErr(Tstring x) /*~NeverReturns*/
{
  fatalErr(SysError, x);
}

/****static char *trigraph(register char *ptrS, char *pEquivCh)
  a traiter non dans NxtCh (sauf ??/) 
{
  sysErr(55);
  *pEquivCh = *ptrS;
  return ptrS;
}*/

static bool waitAndAnalAnswer(void) /*~PseudoVoid*/
{
  char c;
  bool resul = False;

  (void)fflush(outputStream);
  while ((c = getchar()) != '\n') {
    if (c == '#') stopAfterMsg = False;
    if (tolower(c) == ExplainMsgChar) resul = True;};
#ifdef CrEchoedAsNewLine
  ResetSpaceAtEndOfLine;  /* kill previous '\n' */
#endif
  return resul;
}

/* End DCMAIN.C */
