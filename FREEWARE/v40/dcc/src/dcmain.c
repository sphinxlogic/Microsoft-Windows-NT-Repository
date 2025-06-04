/* DCMAIN.C */
/* Y.L. Noyelle, Supelec, France 1998 */
/* Manages command line, source files, character input/output, and errors */

#include <ctype.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#ifdef VMS
#  define _POSIX_C_SOURCE  0  /* for "fileno" function */
#  include <stdio.h>
#  include <unistd.h>
#  undef _POSIX_C_SOURCE
#elif defined _MSC_VER
/*#  include <io.h>
#  define isatty _isatty
#  define fileno _fileno*/
#  define _POSIX_
#  include <stdio.h>
#  include <unistd.h>
#  undef _POSIX_
#elif defined __hpux
#  define _INCLUDE_POSIX_SOURCE
#  include <stdio.h>
#  include <unistd.h>
#  undef _INCLUDE_POSIX_SOURCE
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
#include "dcprag.ph"
#include "dcrec.h"
#include "dcrec.ph"
#include "dcrecdir.ph"

#ifdef VMS
#  pragma noinline (changeStreamTo, copyDirPrefix, dispUsg, errExit, fileErr, fmtdMsg, foundOption)
#  pragma noinline (getNxtFName, intrnErr, nxtChunkOfTxt)
#  pragma noinline (mngOptListTok, mngOptMsgLimit, mngOptStopAfterMsg)
#  pragma noinline (mngOptVerbose)
#  pragma noinline (searchNxtNL, storeDMacTxt, waitAndAnalAnswer)
#endif

#define ComputeFileHCode(x) {						       \
  ThCode fileHCode = 0;							       \
									       \
  {register const char *p = x;; while (*p != '\0') {fileHCode +=	       \
						  (ThCode)RealChar(*p); p++;}}
#define CSuffix		".c"
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
#define EmptyDirName	((const TdirAtom *)"\0")
#define FinishSrcFile()	{						       \
  free(curSourceMngtBlk); /* do not free curFileName, because name may still   \
				    be used (i.e. in a 'NotUsed' warning). */  \
  lineNb = 0;  /* indicates "no valid source line" */}
#define IdentHeaderSpace "  "
/*~ zif LitLen(IdentHeaderSpace) != LgtHdrId "Bad 'IdentHeaderSpace' string" */
#define LgtD(x)		(size_t)*x
#define LimChar		(1<<NbBitsChar)
#define LS2		(LineSize + 1 + 1)  /* +1 for NL */
#define MinDccOptLgt	4
#define NakedFName(x)	(x+(ubyte)*(x - 1))
#define NbSrcLinesKept	2
#define ResetSpaceAtEndOfLine spaceAtEndOfLine = (wrapFl)? LineSize : INT_MAX
#define SearchAdjFiles(fileName, x)					       \
  ComputeFileHCode(fileName)						       \
    for (ptr = adjFilesArr; ptr < adjFilesArr + usedSizAdjArr; ptr++) {	       \
      if (fileHCode==ptr->hCode && (StrEq(fileName, ptr->amendFName) || x)) {
#define SeparPattern	"+"
#define SizeBufDef	(509) + 1  /* cf C standard, plus EndCh */
#define SizeSourceBuffer (NbSrcLinesKept*(LineSize + TransfSize) + 1 + 1 + 1)
#define TxtOptions	"'" DDOpt "', '" DUOpt "', '" DIOpt "'"
#define UndefStr	"undef "

typedef FILE *Stream;
typedef ubyte TdirAtom;
typedef const TdirAtom *TdirName;

typedef struct {
  ThCode hCode;
  bool noLoadSysFile;
  Tstring amendFName;
  Tstring adjFName;
} TadjFilesS;

typedef struct {
  Stream sourceStream;
  char *ptrS, *oldMaxPtrS, *posEndChP1, *nxtPosEndChP1;
  TlineNb lineNb;
  char idPlusSource[LgtHdrId + MaxLgtId + SizeSourceBuffer];
} TsourceMngtBlk;

typedef struct _TinclStkElt TinclStkElt;
struct _TinclStkElt {
  TsourceMngtBlk *sourceMngtBlk;
  TmngtPriv *listPriv;
  Tstring fileName;
  struct {
    bool _hdrFilPerSe:1;
    bool _sysHdrFile:1;
    bool _diffLc:1;
    signed int _indentIncra:INT_BIT - 1 - 1 - 1;
  } inclInfo;
  TinclStkElt *prec;
};
/*~zif sizeof(((TinclStkElt *)0)->inclInfo) != sizeof(int)
					       "Bad structure for 'inclInfo'" */
#define hdrFilPerSe	inclInfo._hdrFilPerSe
#define sHdrFile	inclInfo._sysHdrFile
#define diffLc		inclInfo._diffLc
#define indentIncra	inclInfo._indentIncra

/* Function profiles */
static TinclStkElt *allocInclStkElt(void), *freeInclStkElt(TinclStkElt *);
static char *manageEndInclude(void), *searchNxtNL(char *, ptrdiff_t);
/****static char *trigraph(char *, char *);*/
static void changeStreamTo(Stream), checks(void), dispUsg(void), emitCstWdthCh(
  const char *, const char *), emitC(char), errExit(Terr, const Tstring *)
  /*~NeverReturns*/, fileErr(Tstring) /*~NeverReturns*/, flushAndExit(void)
  /*~NeverReturns*/, fmtdMsg(Terr n, const Tstring *parArr), initAdjFiles(void),
  initAll(void), initEmitC(void), initErr(void), initMain(void), intrnErr(
  Tstring, uint *), openSourceFile(Tstring),
  processCUnit(Tstring, bool), prmtrzMsg(Tstring, const Tstring *),
  resetArgPtr(void), storeDMacTxt(Tchar);
static void mngOptListTok(void), mngOptMsgLimit(void), mngOptStopAfterMsg(void),
  mngOptVerbose(void);
static bool foundOption(Tstring, bool *), isDccOption(Tstring),
  isHeaderFile(Tstring), isSrcFName(Tstring), openSourceFile1(Tstring),
  waitAndAnalAnswer(void) /*~PseudoVoid*/;
static TdirName copyDirPrefix(Tstring, size_t);
static Tstring getNxtArg(void);
#ifdef LcEqUc
static bool sameLowerCaseName(const char *, const char *);
#endif

/* Global variables */
static TadjFilesS *adjFilesArr;
static Tstring adjFilesList, sysInclDir = SysInclDir;
static const Tstring *argvg, *argv1;
static bool callCompil = True, callCompilSeen = False, callCompilW = True,
  chkIndent = True, emitTrailer = True, errSeen = False, notCompiled = False,
  warnSeen = False;
static bool chkPossErr = True;	/* check various possible errors */
static bool chkRdbl = True;	/* check readability */
static TstringNC copDfltArgs, dfltArgsPtr;
static uint ctrdI;
static uint ctrISE = 0;		/* number of '#include' stack elements */
static uint ctrErr, ctrWarn;
static ulong cumNbLines = 0;	/* cumulated number of lines processed */
static TsourceMngtBlk *curSourceMngtBlk;
static int deltaPtrS = 0;  /* device to limit size of loaded chunk while
					searching next NL (cf searchNxtNL()). */
static TdirName *dirArray; /* array of directory prefixes for 'include' files */
static char *endSBuf;	/* end of source buffer */
static Tstring dfltArgs;
static bool givAlwsFName = False;
static bool hdrFilePerSe = False;  /* True if current file has a '.xhx' name,
			and is not included with a ComposingHdr d-pragma.
			Do not forget that "normal" files included in header
			files are considered as header files themselves. */
static int hfpsCtrM1 = -1;  /* count (-1) of real header files in 'include'
								       stack. */
static uint initMsgLimit = DefaultMessagesLimit;
static bool initStopAfterMsg = True;
static bool interactivOutptDev;
static bool initVerbose = False;
static Tstring lastTrtdFName = NULL;
static size_t lgtCmdBuf;  /* length of command buffer */
static size_t lgtLastTokBuf;  /* length of "last tokens" buffer */
static bool moreInfMsg = False;
static bool msgLimitFl = False;
static uint msgLimit;
static char *nxtPosEndChP1;
static char *oldMaxPtrS;  /* end of logical (circular) source buffer */
static int optionValue = DfltOptVal;
static Stream outputStream;
static bool ovfldTokBuf;
static CreateParArr(1);
static char *pLastTokBuf,  /* pointer inside last tokens buffer */
	    *begLastTokBuf = NULL,
	    *endLastTokBuf;
static Tchar *pStoDMacTxt, *endDMacBuf;
static TinclStkElt *pTopInclStk = NULL;
static bool screenMsg;
static Stream sourceStream;
static int spaceAtEndOfLine;  /* number of remaining character slots at end
				      of current line (see 'emitC' function). */
static bool stopAfterMsg;
static size_t usedSizAdjArr, totalSizAdjArr;
static bool warnFl = True;
static bool wrapFl = True;
static const Ttok zSCol[] = {NoSwallowTok, SCOL, EndSTok};

/* External variables */
char *begSBuf;	/* beginning of source buffer (circular buffer) */
bool fileClosed;
char *nlPosP1;	/* 1 + position of last seen 'newline' character */
char *posEndChP1; /* 1 + position of EndCh character indicating end of current
								  text chunk. */
int tabSpacing = 0;

int main(int argc, const Tstring *argv)
{
  Tstring lastFName = NULL, lastCUFName = NULL, starterFile;
  bool argErrFl = False, usgFl = False;

  outputStream = SUMMARY_STREAM;
  initEmitC();
  argvg = argv;
  if (argc == 1) dispUsg();
  else {
    interactivOutptDev = isatty(fileno(MAIN_OUT_STREAM)) > 0;
    {
      TstringNC w, w1;
      Tstring dccDir = DccDir;

      if ((w = getenv("DCCFILES"))!=NULL && *w!='\0') {
        MyAlloc(dccDir = w1, strlen(w) + 1);
        strcpy(w1, w);
        if ((w = strchr(w1, ' ')) != NULL) {*w++ = '\0'; sysInclDir = w;}}
      MyAlloc(starterFile = w, strlen(dccDir) + (LitLen(StarterFile) + 1));
      strcpy(w, dccDir);
      strcat(w, StarterFile);
      MyAlloc(adjFilesList = w, strlen(dccDir) + (LitLen(AdjustFilesList) + 1));
      strcpy(w, dccDir);
      strcat(w, AdjustFilesList);}
    if ((dfltArgs = getenv("DCCDFLTS")) == NULL) dfltArgs = "";
    MyAlloc(copDfltArgs, strlen(dfltArgs) + 1);
    /* Search position of last file name in command line, so that it is proces-
       sed with all indicated options, even ones met after that file name. */
    {
      Tstring w;

      resetArgPtr();
      while ((w = getNxtArg()) != NULL) if (isSrcFName(w)) lastFName = w;}
rescanOptL:  /* back here if more than one compilation unit to process */
    initAll();
    getTokLvl = 1;  /* don't keep tokens yet */
    mngOptListTok();
    mngOptMsgLimit();
    mngOptVerbose();
    mngOptStopAfterMsg();  /* to come after mngOptVerbose() */
    enterBlock();
    openSourceFile(starterFile);
    dollAlwdInId = True;
    prog();
    FinishSrcFile()
    initAdjFiles();  /* here, because uses symbol defined in starter file */
    cUnitFName = curFileName = "";  /* for -D, -U options processing (call to
				   'manageDir', which may note these values). */
    getTokLvl = 0;  /* start keeping tokens */
    {
      Tstring arg;

      resetArgPtr();
      ctrdI = 0;  /* number of '-I' options */
      lgtCmdBuf = 0;  /* length of command buffer */
      while ((arg = getNxtArg()) != NULL) {
        if (isDccOption(arg)) {
          static bool tabFl = False;

          parArr1[1] = arg;
          if (foundOption("ac", &askConstFl)) {}
          else if (foundOption("ae", &allErrFl)) {}
          else if (foundOption("afn", &givAlwsFName)) {}
          else if (foundOption("bo", &chkBool)) {}
          else if (foundOption("cc", &callCompil)) {}
          else if (foundOption("ctn", &chkTypName)) {}
          else if (foundOption("cw", &callCompilW)) {}
          else if (foundOption("epl", &chkEmptParList)) {}
          else if (foundOption("frt", &chkFctRetTyp)) {}
          else if (foundOption("inc", &chkInclPos)) {}
          else if (foundOption("ind", &chkIndent)) {}
          else if (foundOption("lt", &listTok)) {
            if (optionValue >= MaxLgtId + 1) {lgtLastTokBuf = (size_t)
					 optionValue; optionValue = DfltOptVal;}
            else lgtLastTokBuf = LineSize - LitLen(Dots) - (strlen(errTxt[
			      LastToks]) - 1);  /* so that just one line used */
            mngOptListTok();}
          else if (foundOption("mcc", &verifAllPtrForConst)) {
            if (verifAllPtrForConst) askConstFl = True;}
          else if (foundOption("mic", &moreIndexTypeChk)) {}
          else if (foundOption("msg", &msgLimitFl)) {
            if (optionValue >= 0) {
              initMsgLimit = (uint)optionValue;
              optionValue = DfltOptVal;}
            mngOptMsgLimit();}
          else if (foundOption("nui", &warnNUI)) {}
          else if (foundOption("nup", &warnNUP)) {}
          else if (foundOption("nw", &noWarn)) {}
          else if (foundOption("pe", &chkPossErr)) {}
          else if (foundOption("pnt", &chkNmdTypParFl)) {}
          else if (foundOption("po", &chkPortbl)) {}
          else if (foundOption("rd", &chkRdbl)) {}
          else if (foundOption("sam", &initStopAfterMsg))
            mngOptStopAfterMsg();
          else if (foundOption("sy", &printStat)) {}
          else if (foundOption("tab", &tabFl)) {
            if (optionValue > 0) {
              tabSpacing = optionValue;
              optionValue = DfltOptVal;}}
          else if (foundOption("tr", &emitTrailer)) {}
          else if (foundOption("uc", &chkNumCst)) {}
          else if (foundOption("usg", &usgFl)) {
            if (usgFl) {dispUsg(); usgFl = False;}}
          else if (foundOption("ve", &moreInfMsg)) mngOptVerbose();
          else if (foundOption("vve", &initVerbose)) mngOptVerbose();
          else if (foundOption("wa", &warnFl)) {}
          else if (foundOption("wr", &wrapFl)) {}
          else {
            if (lastTrtdFName == NULL) fmtdMsg(UnknOption, parArr1);
            argErrFl = True;}
          if (optionValue != DfltOptVal) {
            if (lastTrtdFName == NULL) fmtdMsg(IgndCharsOpt, parArr1);
            optionValue = DfltOptVal;
            argErrFl = True;}
          /* Take option into account immediatly, because -D, -U, -I options
						management may create errors. */
          screenMsg = !(allErrFl | chkEmptParList | moreIndexTypeChk |
		chkNmdTypParFl) & askConstFl & chkBool & callCompil &
		chkFctRetTyp & 	chkInclPos & warnNUI & warnNUP &
		chkPossErr & chkPortbl & chkRdbl & chkNumCst;}
        else {
          int dOptionSeen;

          if (strncmp(arg, DIOpt, LitLen(DIOpt)) == 0) ctrdI++; /* count number
							     of '-I' options. */
          else if ((dOptionSeen = strncmp(arg, DDOpt, LitLen(DDOpt)))==0 ||
		  strncmp(arg, DUOpt, LitLen(DUOpt))==0) {  /* '-D/-U' option */
            for (;;) {  /* only there to avoid 'goto's */
              Tchar bufDef[SizeBufDef + LgtHdrId], c;
              register Tstring ptrInArg;

              if (dOptionSeen == 0) {
                strncpy(&bufDef[LgtHdrId], DefineStr, LitLen(DefineStr));
                pStoDMacTxt = &bufDef[LitLen(DefineStr) + LgtHdrId]
								  /*~LocalAdr*/;
                ptrInArg = &(arg)[LitLen(DDOpt)];}
              else {
                strncpy(&bufDef[LgtHdrId], UndefStr, LitLen(UndefStr));
                pStoDMacTxt = &bufDef[LitLen(UndefStr) + LgtHdrId]
								  /*~LocalAdr*/;
                ptrInArg = &(arg)[LitLen(DUOpt)];}
              endDMacBuf = &bufDef[0] + sizeof(bufDef) /*~LocalAdr*/;
              while (c = *ptrInArg++, isAlfa(c)) storeDMacTxt(c);  /* store macro
									name. */
              if (dOptionSeen == 0) {
                storeDMacTxt(' ');
                if (c == '\0') storeDMacTxt('1');/* "1" is default macro text */
                else if (c != '=') {errWS(BadDOption | Warn2, arg); break;}
                else {  /* copy macro body */
                  while ((c = *ptrInArg++) != '\0') storeDMacTxt(c);}}
              else if (c != '\0') {errWS(BadDOption | Warn2, arg); break;}
              storeDMacTxt(EndCh);
              posEndChP1 = NULL;  /* previous EndCh = end of text */
              if (endDMacBuf == NULL /* buffer overflow */) {errWS(BadDOption |
							    Warn2, arg); break;}
              dirLineFl = True;
              srcPtr = &bufDef[LgtHdrId] /*~LocalAdr*/;
              /* Prepare to print pseudo source line in case of error */
              bufDef[LgtHdrId - 1 - 1] = EndCh;
              bufDef[LgtHdrId - 1] = '\n';
              nlPosP1 = srcPtr;
              begSBuf = &bufDef[0] /*~LocalAdr*/;
              oldMaxPtrS = &bufDef[NbElt(bufDef)] /*~LocalAdr*/;
              manageDir();
              RestoSmshCh;
              break;}}
          lgtCmdBuf += strlen(arg) + 1;  /* +1 because of separating space; use-
			  less for already processed compilation units names. */
          if (isSrcFName(arg)) {
            if (arg == lastFName) lastCUFName = lastFName;  /* for last compila-
	    				     tion unit, take also into account
					     possible following options. */
            else if (lastTrtdFName != NULL) {  /* skip already processed
							   compilation units. */
              if (arg == lastTrtdFName) lastTrtdFName = NULL;}
            else {  /* process new compilation unit */
              processCUnit(arg, False);
              lastTrtdFName = arg;
              changeStreamTo(MAIN_OUT_STREAM);  /* separating pattern more
								useful there. */
              {
                int i;

                for (i = 0; i < LineSize/LitLen(SeparPattern); i++)
                  emitS(SeparPattern);
                emitC('\n');}
              changeStreamTo(SUMMARY_STREAM);
              goto rescanOptL /*~BackBranch*/;}}}}
      processCUnit(lastCUFName, True);}}
  changeStreamTo(NULL);
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

static void processCUnit(Tstring cUFName, bool lastFNameFl)
{
  TdirName *ptrDirArray;  /* pointer in array of directories for 'include'
								       files. */
  char *cmdBuf, *ptrCmdBuf;
  register Tstring arg;
  bool fNameSeen = False;

  /* Get compiler name */
  {
    TlitString w;
    static char locBuf[] = IdentHeaderSpace DccSymbol ";";  /* ';' for
						     getLitString() workings. */

    getTokLvl = 1;  /* prevent tokens from showing */
    srcPtr = &locBuf[LgtHdrId];
    if (NxtTok()!=CSTST || (w = getLitString(), srcPtr != &locBuf[NbElt(
					 locBuf) - 1])) errExit(BadUUdcc, NULL);
    lgtCmdBuf += initGetStrLit(w);
    getTokLvl = 0;
    RestoSmshCh;}
  /* Allocate array of directory prefixes for 'include' files */
  MyAlloc(dirArray, (ctrdI + 1 + 1 + 1)*sizeof(*dirArray));  /* +3 because of
		       compilUnitFile directory, SysInclDir, and ending NULL. */
  /* Allocate compile command buffer (to be passed to system via the
							  'system' function). */
  MyAlloc(cmdBuf, lgtCmdBuf);
  ptrCmdBuf = cmdBuf;
  while ((*ptrCmdBuf++ = getNxtStrLitChar()) != '\0') {} /* put compiler name */
  exitBlock();  /* because of getLitString() */
  ptrCmdBuf--;
  ptrDirArray = dirArray + 1;
  cUnitFName = NULL;
  resetArgPtr();
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
#ifdef VMS
        size_t posVersNb;
#endif

        begOfFName = skipPath(arg);
        lgtPath = (size_t)(begOfFName - arg);
        *dirArray = (lgtPath != 0)? copyDirPrefix(arg, lgtPath) :
							       &EmptyDirName[0];
#ifdef VMS
        {
          /* Add '.c' type suffix if none; remove possible version number in
							     'stripped' name. */
          size_t suffLgt = (strchr(begOfFName, '.') == NULL)? LitLen(
					      CSuffix) /* no type suffix */ : 0;
          Tstring w;
          TstringNC wm;

          posVersNb = (((w = strchr(begOfFName, ';')) == NULL)?
				 strlen(begOfFName) : (size_t)(w - begOfFName));
          MyAlloc(cUnitFName = wm, strlen(arg) + (LitLen(CSuffix) + 1));
          strncpy(wm, arg, lgtPath + posVersNb);
          if (suffLgt != 0) strcpy(wm + lgtPath + posVersNb, CSuffix);
          strcpy(wm + lgtPath + posVersNb + suffLgt, begOfFName +
					   posVersNb);  /* add version number */
          posVersNb += suffLgt;
          MyAlloc(strpdCUnitFName = wm, posVersNb + 1);
          strncpy(wm, cUnitFName + lgtPath, posVersNb);
          *(wm + posVersNb) = '\0';}
#else
        cUnitFName = arg;
        strpdCUnitFName = begOfFName;
#endif
      }}
    /* Add argument to compiler command line */
    *ptrCmdBuf++ = ' ';  /* separating space */
    while (*arg!= '\0') *ptrCmdBuf++ = *arg++;
    if (cUnitFName!=NULL && !lastFNameFl) break;}
  *ptrCmdBuf = '\0';  /* ending NUL */
  if (!fNameSeen || cUnitFName==NULL && !callCompil) errExit(NoCUName, NULL);
  *ptrDirArray++ = copyDirPrefix(sysInclDir, strlen(sysInclDir));
  *ptrDirArray = NULL;
  if (cUnitFName != NULL) {  /* if something to compile, do not skip to link */
    if (tabSpacing == 0) tabSpacing = DefaultTabSpacing;
    if(! chkIndent) indentIncr = 0;
    dollAlwdInId = False;
    mskBool = (chkBool)? (boolTypeElt.typeSort = boolCstTypeElt.typeSort =
		Bool, Bool|BoolOpnd) : (boolTypeElt.typeSort = boolCstTypeElt.
				  typeSort = Int, NumEnumBool|PtrPoss|BoolOpnd);
    cumDeltaLineNb = 0; cumNbSkippedLines = 0;  /* may have been modified by
						'starter' or 'initAdj' files. */
    openSourceFile(cUnitFName);
    if (isHeaderFile(cUnitFName)) {cumNbLines = 1; errWS(CompHdrFile,
								   cUnitFName);}
    else {
      changeStreamTo(MAIN_OUT_STREAM);
      prog();}
    exitBlock();
    checkCondStkAndDeleteMacros();
    freeTCB();
    changeStreamTo(SUMMARY_STREAM);
    checks();
    if (emitTrailer || (ctrErr | ctrWarn)!=0) {
      CreateParArr(2) /*~DynInit*/;

      parArr2[1] = cUnitFName;
      parArr2[2] = longToS((long)(cumNbLines + lineNb - 1 +
					   cumDeltaLineNb - cumNbSkippedLines));
      prmtrzMsg(errTxt[EndMsg], parArr2);
      if ((ctrErr | ctrWarn) != 0) {
        TnbBuf buf;

        bufLongToS((long)ctrErr, buf);
        parArr2[1] = &buf[0];
        parArr2[2] = longToS((long)ctrWarn);
        prmtrzMsg(errTxt[ErrWarn], parArr2);}
      else emitS(errTxt[NoErrWarn]);}
    FinishSrcFile()  /* lineNb not used any more here */ 
#ifdef VMS
    free((TstringNC /*~OddCast*/)cUnitFName);
    free((TstringNC /*~OddCast*/)strpdCUnitFName);
#endif
  }
  if (ctrWarn != 0) {warnSeen = True; if (! callCompilW) notCompiled = True;}
  if (ctrErr != 0) {errSeen = True; if (callCompil) callCompilSeen = True;}
  else if (callCompil && (callCompilW || ctrWarn==0)) {
    changeStreamTo(NULL);
    (void)system(cmdBuf);}
  /* Free allocated memory */
  free(cmdBuf);
  if (*dirArray != &EmptyDirName[0]) free((TdirAtom * /*~OddCast*/)*dirArray);
  ptrDirArray = dirArray + 1;
  do {
    free((TdirAtom * /*~OddCast*/)*ptrDirArray++);
  } while (*ptrDirArray != NULL);
  free(dirArray);
}

static AllocXElt(allocInclStkElt, TinclStkElt *, ctrISE, ;)

static void changeStreamTo(Stream x)
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
  if (ctrBSE > 1)  intrnErr("ctrBlkStkElt", &ctrBSE);
  if (ctrCE != 0)  intrnErr("ctrCaseElt", &ctrCE);
  if (ctrCSE != 0) intrnErr("ctrCondStkElt", &ctrCSE);
  if (ctrDE != 0)  intrnErr("ctrDeclElt=", &ctrDE);
  if (ctrDIE != 0) intrnErr("ctrDescrIdElt", &ctrDIE);
  if (ctrISE != 0) intrnErr("ctrInclStkElt", &ctrISE);
  if (ctrMSB != 0) intrnErr("ctrMacroBlk", &ctrMSB);
  if (ctrMSE != 0) intrnErr("ctrMacStkElt", &ctrMSE);
  if (ctrNI != 0)  intrnErr("ctrNotInitVarElt", &ctrNI);
  if (ctrNSB > 1+1+1 /* 3 because first name block never freed, and two other
	may be kept not freed (see exitBlock() ) */) intrnErr("ctrNameStoBlk",
								       &ctrNSB);
  if (ctrSE != 0)  intrnErr("ctrSemanElt", &ctrSE);
  if (ctrTCB != 0) intrnErr("ctrTypCombElt", &ctrTCB);
  if (ctrTE != 0)  intrnErr("ctrTypeElt", &ctrTE);
  if (ctrTLE != 0) intrnErr("ctrTagListElt", &ctrTLE);
  intrnErr("", NULL);  /* flush buffer */
}

static TdirName copyDirPrefix(Tstring string, size_t lgt)
{
  TdirName res;

  if (lgt >= UCHAR_MAXC) {parArr1[1] = string; errExit(StringTooLong, parArr1);}
  MyAlloc(res, lgt + 1 + 1);  /* +1 for length field, +1 for ending '/' */
  memcpy((char * /*~OddCast*/)res + 1, string, lgt);
#ifdef VMS
  *(TdirAtom * /*~OddCast*/)res = (TdirAtom)lgt;
#else
  *(TdirAtom * /*~OddCast*/)(res + lgt + 1) = (TdirAtom)'/';
  *(TdirAtom * /*~OddCast*/)res = (TdirAtom)(lgt + 1);
#endif
  return res;
}

static void dispUsg(void)
{
  TnbBuf buf;
  CreateParArr(4) /*~DynInit*/;

  bufLongToS((long)LineSize, buf);
  parArr4[1] = *argvg;
  parArr4[2] = longToS((long)DefaultTabSpacing);
  parArr4[3] = &buf[0];
  parArr4[4] = TxtOptions;
  fmtdMsg(Use1, parArr4);
  if (isatty(fileno(outputStream)) > 0) {
    fmtdMsg(ProceedMsg, NULL);
    waitAndAnalAnswer();}
  fmtdMsg(Use2, parArr4);
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
      if (ptrBufCh < AdLastElt(bufChar)) return;
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
  if (! fileClosed) ilgCharF(ptrS);
  else {
    BackUp;  /* do not go beyond EndCh */
    if (dirLineFl) {dirLineFl = False; curTok.tok = ENDDIR;}
    else {
      RestoSmshCh;
      if (pTopInclStk != NULL) {ptrS = manageEndInclude(); curTok.tok =
								    WHITESPACE;}
      else {curTok.tok = ENDPROG; condDirSkip = False;}}}
  srcPtr = ptrS;
}

static uint minMsgLvl = 0;

static void initErr(void)
{
  minMsgLvl = 0;
}

void err(Terr n, const Tstring parArr[])
{
  Terr errNo = n & (EndErrInfoBits -1 -1);
  bool warn = n&WarnMsk || dpragNst!=0;

  if (mngConc) {  /* if inside ## operator */
    if (concatErr == NoConcErr) concatErr = errNo;  /* retain first error */
    return;}
  if (peepTok) return;  /* will be seen again... */
  if (InsideMacro && curTok.tok==SCOL) {
    static const Terr retainedErr[] = {RSBrExptd, RBraOrCommaExptd, RParOrCommaExptd,
							  RBraExptd, RParExptd};
    const Terr *ptr = &retainedErr[0];

    do {
      if (errNo == *ptr++)
        if (checkSColAtEndMac()) return;
        else break;
    } while (ptr < &retainedErr[NbElt(retainedErr)]);}
  if (warn && (!warnFl
               || sysHdrFile  /* no warning in system header file */
               || sysMacro && n&CWarn  /* no Cwarning in system macro */
               || n&PossErr && !chkPossErr
               || n&Rdbl && !chkRdbl)
      || errNo==NoErrMsg
      || n&WarnMsk && noWarn
      || ignoreErr) return;
  if (dpragNst==0 && screenMsg) {
    uint msgLvl = (warn)? (uint)(n & WarnMsk) : UINT_MAX;

    if (msgLvl < minMsgLvl) return;  /* screen messages */
    minMsgLvl = msgLvl;}
  if ((ctrWarn | ctrErr) == 0) {
    emitS(errTxt[WarnErrInFile]);
    emitS(cUnitFName);
    emitS("\" *****\n");
    if (stopAfterMsg) {emitS(errTxt[StopAftMsgBanner]); waitAndAnalAnswer();}
    emitC('\n');}
  else emitS(errTxt[SeparMsg]);
  if (!stopAfterMsg && msgLimit--==0) errExit(TooManyMsg, NULL);
  if (!(n & NoDispLine) && (lineNb!=0 || dirLineFl)) {  /* not no printing of
									line. */
    char *ptrS;
    const char *expl;
    ptrdiff_t distErrBegLine;  /* nb of characters between the beginning of the
				      line and the error position (included). */
    char savInfoId[LgtHdrId];

    if (adSmshCh != NULL) {   /* put back in place possible characters smashed
					  by an identifier length/name space. */
      savInfoId[0] = *adSmshCh; savInfoId[1]= *(adSmshCh + 1);
      *adSmshCh = smshCh[0]; *(adSmshCh + 1) = smshCh[1];}
    ptrS = srcPtr;
    if ((distErrBegLine = ptrS - nlPosP1) < 0) distErrBegLine += oldMaxPtrS -
				       begSBuf; /* because of circular buffer */
    if (distErrBegLine <= LineSize) {
      /* Beginning of line not very far from error position, so (end of) prece-
         ding line, and (beginning of) current line, can be displayed. */
      /* Search beginning of preceding line; 'expl' points on it */
      uint ctr;

      for (expl = nlPosP1 - 1, ctr = LS2 - 1; ctr != 0; ctr--) {
        if (expl == begSBuf) expl = oldMaxPtrS;
        if (*(--expl) == EndCh) {expl = begSBuf - 1; goto foundNLL;}  /* no line
				    before (testing lineNb == 1 is not correct,
				    because of possible splicing). */
        if (*expl == '\n') goto foundNLL;}
      /* Beginning of preceding line not visible */
      emitS(Dots);
      expl += LitLen(Dots);
foundNLL:
      if (++expl >= oldMaxPtrS) expl -= oldMaxPtrS - begSBuf;
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
        expl = begErrLine + (LineSize - (LitLen(Dots)) - 1);
/*        if ((ptrSM1 = ptrS) == begSBuf) ptrSM1 = oldMaxPtrS;
        if (*--ptrSM1=='\n' || ptrSM1==posEndChP1-1) ptrS = ptrSM1;*/}
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
      *adSmshCh = savInfoId[0]; *(adSmshCh + 1) = savInfoId[1];}}
  if (warn) ctrWarn++; else {ctrErr++; ignoreErr = !allErrFl && curTok.tok!=
								       ENDPROG;}
  {
    bool savGAFN = givAlwsFName, savMIF = moreInfMsg, explain = False;

explainL:
    if (warn) {
      emitS(errTxt[Warning]);
      emitC('0' + ((uint)(n & WarnMsk) >> INT_BIT - 1 - 1));
      /*~ zif (uint)WarnMsk != 3u<<(INT_BIT - 1 - 1)
						 "Change previous expression "*/
      emitC(')');}
    else emitS(errTxt[Error]);
    if (!(n & NoDispLine) || givAlwsFName) {  /* print file name/line
								      number. */
      TnbBuf bufLineNo;

      emitS(errTxt[Line]);
      bufLongToS((long)lineNb, bufLineNo); /* so 'longToS' buffer remains
							       uncorrupted... */
      emitS(bufLineNo);
      if (curFileName!=cUnitFName || givAlwsFName) {
        emitS(errTxt[File]);
        emitS(curFileName);
        emitS(errTxt[EndFileName]);}}
    emitS(": ");
    prmtrzMsg(errTxt[errNo], parArr);  /* parametrized error message */
    if (listTok && (!(n & NoDispLine) || explain) && (!stopAfterMsg || moreInfMsg)) { 
			/* output last tokens taken into account before error */
      emitS(errTxt[LastToks]);
      if (ovfldTokBuf) {emitS(Dots); emitCstWdthCh(pLastTokBuf, endLastTokBuf);}
      emitCstWdthCh(begLastTokBuf, pLastTokBuf);}
    emitC('\n');
    if (stopAfterMsg && (explain = waitAndAnalAnswer())) {givAlwsFName =
			True; moreInfMsg = True; goto explainL /*~BackBranch*/;}
    givAlwsFName = savGAFN;
    moreInfMsg = savMIF;}
}

/*~Undef minMsgLvl */

static void errExit(Terr n, const Tstring *parArr) /*~NeverReturns*/
{
  if (n != NoErrMsg) fmtdMsg(n, parArr);
  flushAndExit();
}

static void fatalErr(Terr n, Tstring x) /*~NeverReturns*/
{
  changeStreamTo(SUMMARY_STREAM);
  mngConc = ignoreErr = False;
  errWS(n, x);
  flushAndExit();
}

static void fileErr(Tstring fileName) /*~NeverReturns*/
{
  fatalErr(FileOpenFail, fileName);
}

static void flushAndExit(void) /*~NeverReturns*/
{
  changeStreamTo(NULL);
  exit(EXIT_FAILURE);
}

static void fmtdMsg(Terr n, const Tstring *parArr)
{
#ifndef VMS
  static bool frstTime = True;

  if (frstTime) {emitC('\r'); frstTime = False;}
#endif
  prmtrzMsg(errTxt[n], parArr);
  emitC('\n');
}

static bool foundOption(Tstring opt, bool *pFl)
{
  bool flagVal;
  size_t lgtOpt = strlen(opt);

  if (*opt!=parArr1[1][LitLen(DccOptPref)] || strncmp(parArr1[1] + LitLen(
				   DccOptPref), opt, lgtOpt) != 0) return False;
  flagVal = (*parArr1[1] == '+');
  lgtOpt += LitLen(DccOptPref);
  if (*pFl == flagVal) {
    if (lastTrtdFName == NULL) {  /* to not repeat already given message */
#define MaxSizDccOpt	6
      Tchar buf[MaxSizDccOpt + 1];

      if (lgtOpt > MaxSizDccOpt) lgtOpt = MaxSizDccOpt;
#undef MaxSizDccOpt
      strncpy(buf, parArr1[1], lgtOpt);
      buf[lgtOpt] = '\0';
      {
        CreateParArr(1) /*~DynInit*/;

        parArr1[1] = &buf[0];
        fmtdMsg(AlrdSeenOpt, parArr1);}}}
  else *pFl = flagVal;
  if (parArr1[1][lgtOpt] != '\0') {
    const Tchar *ptr = &parArr1[1][lgtOpt];

    optionValue = 0;
    do {
      if (! isdigit(*ptr)) {optionValue = DfltOptVal - 1; break;}
      optionValue = optionValue*Base10 + (*ptr++ - '0');
    } while (*ptr != '\0');}
  return True;
}

static FreeXElt(freeInclStkElt, TinclStkElt *, ctrISE, ; , prec)

static Tstring getNxtArg(void)
/* Gives back pointer on next argument (NULL at end) */
{
  Tstring begOpt;

  while (*dfltArgsPtr == ' ') dfltArgsPtr++;  /* skip possible starting spaces */
  if (*dfltArgsPtr == '\0' /* environment string exhausted */) return *++argv1;
  begOpt = dfltArgsPtr;
  while (*++dfltArgsPtr != '\0') {  /* search end of parameter */
    if (*dfltArgsPtr == ' ') {
      *dfltArgsPtr++ = '\0';  /* put parameter ending */
      break;}}
  return begOpt;
}

static Tstring getNxtFName(void)
{
  Tchar *ptrD, *w;
  TlitString ptrO;

  if ((ptrO = getLitString()) == NULL) ptrD = NULL;
  else {
    MyAlloc(ptrD, initGetStrLit(ptrO));
    w = ptrD;
    while ((*w++ = getNxtStrLitChar()) != '\0') {}}  /* save file name in
							     contiguous area. */
  exitBlock();  /* because of getLitString() */
  return ptrD;
}

static void initAdjFiles(void)
{
  openSourceFile(adjFilesList);
  while (NxtTok() != ENDPROG) {
#define IncrSizeAdjArr 5
    if (usedSizAdjArr == totalSizAdjArr) {  /* array full; extend it */
      if (totalSizAdjArr == 0) {
        totalSizAdjArr = IncrSizeAdjArr;
        MyAlloc(adjFilesArr, sizeof(TadjFilesS) * totalSizAdjArr);}
      else {
        totalSizAdjArr += IncrSizeAdjArr;
        if ((adjFilesArr = realloc(adjFilesArr, sizeof(TadjFilesS) *
			 totalSizAdjArr)) == NULL) sysErr(errTxt[RanOutOfMem]);}
#undef IncrSizeAdjArr
    }
    {
      Tstring w;

      adjFilesArr[usedSizAdjArr].noLoadSysFile = Found(EMARK);
      if ((w = getNxtFName()) == NULL) skipTok(zSCol);
      else {
        adjFilesArr[usedSizAdjArr].amendFName = w;
        if (! Found(COMMA)) err0(CommaExptd);
        ComputeFileHCode(w);
          adjFilesArr[usedSizAdjArr].hCode = fileHCode;}
        if ((w = getNxtFName()) == NULL) skipTok(zSCol);
        else adjFilesArr[usedSizAdjArr++].adjFName = w;}}
    if (curTok.tok != SCOL) err0(SColExptd);}
  FinishSrcFile()
}

static void initAll(void)
{
  initBlk();
  initDecl();
  initExp();
  initExt();
  initInst();
  initMain();
  initPrag();
  initRec();
}

static void initMain(void)
{
#ifndef VMS
  static bool frstInit = True;

  if (frstInit) {
    if (setvbuf(MAIN_OUT_STREAM, NULL, (int)_IOLBF, BUFSIZ) != 0) sysErr(
									ExCod1);
    frstInit = False;}
#endif
  ctrErr = ctrWarn = 0;
  cumNbLines = 0;
  initErr();
}

bool insideInclude(void)
{
  return pTopInclStk != NULL;
}

static void intrnErr(Tstring wrd, uint *pCtr)
{
  static bool errSeen = False;

  if (pCtr == NULL) {
    if (errSeen) {emitC(' '); errSeen = False;}}  /* flush buffer */
  else {
    emitS("\nINTERNAL PB: ");
    emitS(wrd);
    emitS("= ");
    emitS(longToS((long)*pCtr));
    *pCtr = 0;
    errSeen = True;}
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

static char *manageEndInclude(void)
{
  TinclStkElt *locPTInclS = pTopInclStk;

  if (! adjustFile) cumNbLines += lineNb - 1;
  free(curSourceMngtBlk); /* do not free curFileName, because name may still
	    be used after exit of include file (i.e. in a 'NotUsed' warning). */
  /* Search if system header file to be 'amended' */
  if (sysHdrFile) {
    TadjFilesS *ptr;
    Tstring nakedFName = NakedFName(curFileName); /* skip prefix */

    /* Search if not recursive inclusion (because adjustFiles can only be
       included at end of system file). */
    {
      TinclStkElt *w = locPTInclS;

      do {
        if (StrEq(NakedFName(w->fileName), nakedFName)) goto recursL;
      } while ((w = w->prec) != NULL);}
    {
#ifdef LcEqUc
      bool diffLC = False;

      SearchAdjFiles(nakedFName, (diffLC = sameLowerCaseName(nakedFName,
							      ptr->amendFName)))
            if (diffLC) locPTInclS->diffLc = True;
#else
      SearchAdjFiles(nakedFName, False)
#endif
            adjustFile = True;
            sysHdrFile = False;
            openSourceFile(ptr->adjFName);
            return srcPtr;}}}}
recursL:;}
  adjustFile = False;
  /* Go back to previous source file */
  curSourceMngtBlk = locPTInclS->sourceMngtBlk;
  begSBuf = &curSourceMngtBlk->idPlusSource[LgtHdrId + MaxLgtId];
  endSBuf = begSBuf + SizeSourceBuffer;
  oldMaxPtrS = curSourceMngtBlk->oldMaxPtrS;
  posEndChP1 = curSourceMngtBlk->posEndChP1;
  nxtPosEndChP1 = curSourceMngtBlk->nxtPosEndChP1;
  nlPosP1 = srcPtr = curSourceMngtBlk->ptrS;  /* because of call to err()
						      later in this function. */
  lineNb = curSourceMngtBlk->lineNb;
  sourceStream = curSourceMngtBlk->sourceStream;
  indentIncr = locPTInclS->indentIncra;
  curFileName = locPTInclS->fileName;
  headListPrivTo = locPTInclS->listPriv;
  fileClosed = False;
  if (hfpsCtrM1 < 0) {headerFile = False; curHdrFName = NonHdrFName;}
  else {
    TinclStkElt *w;

    if (hfpsCtrM1 == 0) insideHdrInHdr = False;
    /* Search in which header file we are back */
    for (w = locPTInclS;; w = w->prec) {
      if ((insideHdrInHdr)? w->hdrFilPerSe : isHeaderFile(w->fileName)) {
        curHdrFName = w->fileName;
        break;}}}
  sysHdrFile = sysAdjHdrFile = locPTInclS->sHdrFile;
  if (hdrFilePerSe = locPTInclS->hdrFilPerSe) hfpsCtrM1--;
#ifdef LcEqUc
  if (locPTInclS->diffLc && chkPortbl) err0(ChkUcLc | Warn1);
#endif
  pTopInclStk = freeInclStkElt(locPTInclS);
  return srcPtr - 1;  /* -1 to get back to nlF() */
}

void manageInclude(TstringNC fileName, bool sysFl, bool cmpsgHdr)
{
  /* Save previous context */
  RestoSmshCh;
  {
    TinclStkElt *w = allocInclStkElt();

    curSourceMngtBlk->ptrS= srcPtr;
    curSourceMngtBlk->oldMaxPtrS = oldMaxPtrS;
    curSourceMngtBlk->posEndChP1 = posEndChP1;
    curSourceMngtBlk->nxtPosEndChP1 = nxtPosEndChP1;
    curSourceMngtBlk->lineNb = lineNb;
    curSourceMngtBlk->sourceStream = sourceStream;
    w->sourceMngtBlk = curSourceMngtBlk;
    w->indentIncra = indentIncr;
    w->fileName = curFileName;
    w->listPriv = headListPrivTo;
    if (w->hdrFilPerSe = hdrFilePerSe) hfpsCtrM1++;
    w->sHdrFile = sysHdrFile;
#ifdef LcEqUc
    w->diffLc = False;
#endif
    w->prec = pTopInclStk;
    pTopInclStk = w;}
  /* Test for errors/warnings while err() stills works */
  {
    Tstring p = SearchDot(fileName);

    if (p!=NULL && StrEq(p+1, "c")) errWS(DontInclBodyFile | Warn2, fileName);
    for (p = fileName; *p !='\0'; p++) if (! IsVisibleChar(*p)) errWS(
					  NotVisiChar | Warn3, charToHexS(*p));}
  {
    bool isBodyHdrF;

    if (hdrFilePerSe = isHeaderFile(fileName)) {
      /* Search if circular chain of inclusion of header files */
      {
        TinclStkElt *w = pTopInclStk;
        Tstring cmpsgHdrFName = NULL, strpdFName = NULL;

        do {
          if (!w->hdrFilPerSe && isHeaderFile(w->fileName)) cmpsgHdrFName =
								    w->fileName;
          if (cmpsgHdrFName!=NULL || cmpsgHdr) {
            if (strpdFName == NULL) strpdFName = skipPath(fileName);
            if (StrEq(skipPath(w->fileName), strpdFName)) {
              errWSS(CmpsgHdrWithSelf | Warn1, strpdFName, cmpsgHdrFName);
              break;}}
        } while ((w = w->prec) != NULL);}
      isBodyHdrF = isBodyHdrFile(fileName);}
    if (cmpsgHdr) {
      if (!headerFile || !hdrFilePerSe) err0(IlgCmpsgHdr | Warn1);
      else if (isBodyHdrF) errWS(IlgCmpsgHdr1 | Warn1, fileName);
      else if (insideHdrInHdr && isBodyHdrFile(curHdrFName)) err0(IlgCmpsgHdr2|
								    Warn1|Rdbl);
      else goto OkL;
      cmpsgHdr = False;
OkL:;
    }}
  if (nestLvl != 0) indentIncr = 0;  /* to avoid 'BadIndent' errors in
						files included inside blocks. */
  if (sysFl) sysHdrFile = sysAdjHdrFile = True;
  if (hdrFilePerSe && sysHdrFile) {
    /* Check if adjust file to replace header file */
    TadjFilesS *ptr;

#ifdef LcEqUc
    SearchAdjFiles(fileName, sameLowerCaseName(fileName, ptr->amendFName))
#else
    SearchAdjFiles(fileName, False)
#endif
          if (ptr->noLoadSysFile) {
            adjustFile = True;
            openSourceFile(ptr->adjFName);
            goto openOK1;}
          break;}}}}
  /* Open included file */
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
			  )? &noPrefix[0] : (sysFl)? dirArray + 1 : dirArray;
    size_t lgtFileN = strlen(fileName) + 1 + 1;  /* prefix length, trailing
									'\0'. */
    size_t lgtPrefix;
#ifdef VMS
    Tchar *pos = NULL;

    /* Manage 'dir/file' form (=>'dir:file') for system header file name */
    if (hdrFilePerSe && sysHdrFile && (pos = strchr(fileName, '/'))!=NULL) {
      *pos = ':';  /* replace '/' by ':' */
      ptrDirArray = &noPrefix[0];}
#endif

    /* Try possible prefixes to open include file (and make a permanent copy
       of its name). */
    for (;;) {
      /* Create full file name */
      lgtPrefix = LgtD(*ptrDirArray);
      MyAlloc(fNameBuf, lgtPrefix + lgtFileN);
      *fNameBuf = (Tchar)lgtPrefix;
      memcpy(fNameBuf + 1, (*ptrDirArray++) + 1, lgtPrefix);
      strcpy(fNameBuf + 1 + lgtPrefix, fileName);
      if (openSourceFile1(fNameBuf + 1)) {
        if (*ptrDirArray == NULL) curFileName = NakedFName(curFileName);
				/* system directory => conceal its name (would
						 just puzzle casual user...). */
        *(TstringNC /*~OddCast*/)(curFileName - 1) = '\0';  /* no prefix */
        break;}
      if (*ptrDirArray==NULL || !sysHdrFile && *(ptrDirArray + 1)==NULL /* do
	     not look in system directory for quoted include file */) fileErr(
								      fileName);
      free(fNameBuf);}
#ifdef VMS
    if (pos != NULL) *((TstringNC/*~OddCast*/)curFileName + (pos - fileName))
					 = '/';  /* restore '/' replacing ':' */
#endif
  }
openOK1:
  if (hdrFilePerSe) {  /* would-be header file */
    if (! cmpsgHdr) {  /* real header file */
      curHdrFName = curFileName;
      if (headerFile) {  /* header file included inside header file */
        insideHdrInHdr = True;}
      else {
        headerFile = True;
        (void)addLvl0InclFName(curHdrFName);}}  /* jot down files included
								  at level 0. */
    else {  /* "composing" header */
      hdrFilePerSe = False;
      if (! insideHdrInHdr) {  /* composed header included at level 0 => compo-
			       sing header behaves as if included at level 0. */
        curHdrFName = curFileName;  /* to prevent ExtObjNotDef message */
        (void)addLvl0InclFName(curHdrFName);}}
    headListPrivTo = NULL;}  /* reset PrivateTo */
  free(fileName);
  lineNb--;  /* because of extra 'newline' put by end of directive processing */
}

static void mngOptListTok(void)
{
  if (begLastTokBuf != NULL) {free(begLastTokBuf); begLastTokBuf = NULL;}
  if (listTok) {
    MyAlloc(begLastTokBuf, lgtLastTokBuf);
    endLastTokBuf = begLastTokBuf + lgtLastTokBuf;
    pLastTokBuf = begLastTokBuf;
    ovfldTokBuf = False;}
}

static void mngOptMsgLimit(void)
{
  msgLimit = (msgLimitFl)
	     ? initMsgLimit
	     : (interactivOutptDev)
	       ? (uint)DefaultMessagesLimit
	       : UINT_MAX;
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
   ptrS points just after ending EndCh. */
{
  int lgt;	/* (max) length of chunk of text to load (including
							     terminating \0). */
  size_t readLgt;	/* length of chunk of text really loaded */

  ptrS--;		/* to eliminate previous terminating EndCh */
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
  if ((uint)(adSmshCh - ptrS) < (uint)lgt) adSmshCh = NULL;  /* if last
					      identifier will be overwritten. */
  if ((uint)(nlPosP1 - 1 - ptrS) < (uint)lgt) nlPosP1 = begSBuf - NbSrcLinesKept
		      *LineSize; /* if NL is overwritten, its position becomes
			  meaningless (see use of 'distErrBegLine' in err()). */
  readLgt = fread(ptrS, 1, (size_t)(lgt - 1), sourceStream);
  if (readLgt==0 || ferror(sourceStream)) {
    bool w = ferror(sourceStream);  /* so that ferror called before fclose */

    if (w | fclose(sourceStream)!=0 /* '|' so that fclose always called */
			     ) {srcPtr = ptrS; errWS(InptFileErr, curFileName);}
    fileClosed = True;
    *ptrS = EndCh;
    posEndChP1 = NULL;}
  else {
    register char *endBuf = ptrS + readLgt;

    *endBuf++ = EndCh;  /* end of loaded text */
    posEndChP1 = endBuf;}
  return ptrS;
}

static void openSourceFile(Tstring fileName)
{
  if (! openSourceFile1(fileName)) fileErr(fileName);
}

static bool openSourceFile1(Tstring fileName)
{
  if ((sourceStream = fopen(fileName, "r")) == NULL) return False;
  fileClosed = False;
  curFileName = fileName;
  MyAlloc(curSourceMngtBlk, sizeof(TsourceMngtBlk));
  begSBuf = &curSourceMngtBlk->idPlusSource[LgtHdrId + MaxLgtId];  /* reserve
	an identifier name buffer in front of the source buffer, buffer used
	in case an identifier straddles both ends of (circular) source buffer,
	because nameStrings must always be contiguous. */
  oldMaxPtrS = posEndChP1 = endSBuf = begSBuf + SizeSourceBuffer;
  *(endSBuf - (1 + 1 + 1)) = EndCh;  /* indicate first line (of current file) */
  *(endSBuf - (1 + 1)) = '\n';	/* simulate initial NL (to manage preprocessor
								 directives). */
  *(endSBuf - (1)) = EndCh;	/* simulate end of loaded text */
  srcPtr = endSBuf - (1 + 1);	/* initialize source pointer */
  lineNb = 0;
  return True;
}

static void prmtrzMsg(Tstring x, const Tstring *tabPrm)
{
  register char c;
  static char lastChar;
  uint w;
  static bool skip;
  static int lvl = -1;

  if (x != NULL) {
    if (++lvl == 0) skip = False;  /* start in no-skipping mode */
    while ((c = *x++) != '\0') {
      if (c=='@' && *x=='0') {
        if (! moreInfMsg) skip = !skip;
        x++;}
      else if (! skip)
        if (c=='@' && tabPrm!=NULL && (w = (uint)(*x - '1'))<(uint
	      /*~OddCast*/)*tabPrm) {prmtrzMsg(*(tabPrm + w + 1), tabPrm); x++;}
        else emitC(lastChar = c);}
    if (--lvl<0 && skip && lastChar!='?') emitC('.');}  /* message ending
								      period. */
}

static void resetArgPtr(void)
{
  dfltArgsPtr = strcpy(copDfltArgs, dfltArgs);  /* copied because modified
							       (' ' => '\0'). */
  argv1 = argvg;
}

#ifdef LcEqUc
static bool sameLowerCaseName(const char *x, const char *y)
{
  while (RealChar(*x) == RealChar(*y)) {if (*x++ == '\0') return True; y++;}
  return False;
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
   a pointer on this character. */
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

static void storeDMacTxt(Tchar c)
{
  if (pStoDMacTxt >= endDMacBuf) endDMacBuf = NULL;
  *pStoDMacTxt++ = c;
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
