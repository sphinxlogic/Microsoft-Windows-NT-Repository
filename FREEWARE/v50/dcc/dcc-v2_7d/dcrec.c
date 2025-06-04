/* DCREC.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#include <ctype.h>
#include <stdlib.h>
#ifdef EXIT_FAILURE
#  undef EXIT_FAILURE
#endif
#include <string.h>
#include "dcrec.h"
#include "dcrec.ph"
#include "dcblk.h"
#include "dcblk.ph"
#include "dcdecl.h"
#include "dcdir.ph"
#include "dcexp.h"
#include "dcext.h"
#include "dcmain.h"
#include "dcmain.ph"
#include "dcprag.h"
#include "dcprag.ph"
#include "dcrecdir.ph"

#ifdef VMS
#  pragma noinline (semanErrNb, syntErrNb)
#endif

#define Base8		8
#define Base16		16
#define BitsOctDigit	3
#define CodeCorrectCharForId (cI = charInfo[(TcharStream)c])>=Aln ||	       \
			        cI==DollFct && (sysAdjHdrFile || dollAlwdInId)
#define ComputeHCode(x)							       \
  hCode = hCode*HCodeCst + (ThCode)(x - Aln)
#define DoWhatForNb(x) {						       \
  curTok.Val = 0;  /* reset flags for analNumCst() */			       \
  frstChOfNb = (bool)x;							       \
  curTok.tok = CSTNU;}
#define HCodeCst	5  /* changing this constant (that must stay odd for
		    good scrambling) means changing all hardcoded Hcodes... */
#define IncLineNb() {	 						       \
  nlPosP1 = ptrS; if (lineNb++ > 0) frstLineOfFile = False;}
#define LexErr(x, y)	srcPtr = ptrS, errWS(x, y)
#define LexErrPhase4(x, y) {if (! condDirSkip) LexErr(x, y);}
#define LexErrPx(x, y, z) srcPtr = ptrS+(z), errWS(x, y)
#define LimChar		(1<<NbBitsChar)
#define Lletter		(Uletter + ('z' - 'a' + 1))
#define ManageLitChar(x)						       \
  if (c=='\n' || c==EndCh && fileClosed) {LexErr(x, NULL); BackUp; break;}     \
  if (c == '\\') ptrS = getEscSeqChar(ptrS, &c);			       \
  else if (!IsVisibleChar(c) && c!='\t')				       \
    LexErr(NotVisiChar | Warn2, charToHexS(c));
#define ManageTab(x)	x = (x + tabSpacing)/tabSpacing*tabSpacing
#define MaxNbCharRepr	3
#define Uletter		(Aln + Base10)

/* Function profiles */
static char *getEscSeqChar(char *, char *);
typedef void TspeTokRecFct(char *);
static TspeTokRecFct nlF, ampersF, dollF, dotF, dQuoteF, eMarkF, eqF, greaTF,
  lessTF, minusF, percentF, plusF, quoteF, sharpF, slashF, starF, upArrowF,
  vBarF;
static void initKeyWords(void), initStoreTokTxt(void), putCh(TcharStream),
  putTokChar(char c), semanErrNb(void), storeTokTxtRepr(char), syntErrNb(Terr,
  Tstring);

/* Global variables */
static void (*pPutCh)(char /*~Generic*/);
static void (*pStoreChFct)(char /*~Generic*/) = &saveTokChar;
static TstringNC pTokTxt;  /* pointer for putTokChar() */
static bool wccSeen = False;

/* External variables */

char *adSmshCh = NULL;  /* address of characters currently smashed in source
	  buffer by last identifier header (tuple {id.length, id.nameSpace}). */
Terr concatErr = NoErrMsg;
bool dirLineFl = False;  /* True if currently processing directive line */
bool frstChOfNb = False;
bool mngConc = False;  /* True if currently doing '##' concatenation */
TdescrId *pDescTokId;
TppNbCtx *pPPNbCtx = NULL;  /* context of current pp-number suffix */
char smshCh[LgtHdrId];  /* buffer to save characters smashed in source
						 buffer by identifier header. */
TdescrId *symTabHeads[SizeSymTab];  /* symbol table (heads of 'small tables') */
Tchar tokTxtBuf[MaxLgtId + 1];  /* general use buffer */


static const Tname tabKeyWords[] = { 
    /* First byte: length; 2nd byte: name space */
    NULL,			/* NOATTRIB */
    ConvTname("\6\0static"), ConvTname("\6\0extern"), ConvTname("\7\0typedef"),
    ConvTname("\4\0auto"), ConvTname("\x8\0register"),
    ConvTname("\5\0" ConstStr), ConvTname("\x8\0" VolatileStr),
    ConvTname("\4\0" EnumStr), ConvTname("\6\0" StructStr),
    ConvTname("\5\0" UnionStr), ConvTname("\4\0" VoidStr),
    ConvTname("\5\0" FloatStr), ConvTname("\6\0" DoubleStr),
    ConvTname("\4\0" CharStr), ConvTname("\5\0" ShortStr),
    ConvTname("\3\0" IntStr), ConvTname("\4\0" LongStr),
    ConvTname("\x8\0" UnsignedStr), ConvTname("\6\0" SignedStr),
    ConvTname("\5\0break"), ConvTname("\4\0case"),
    ConvTname("\x8\0continue"), ConvTname("\7\0default"), ConvTname("\2\0do"),
    ConvTname("\4\0else"), ConvTname("\3\0for"), ConvTname("\4\0goto"),
    ConvTname("\2\0if"), ConvTname("\6\0return"), ConvTname("\6\0sizeof"),
    ConvTname("\6\0switch"), ConvTname("\5\0while"),};
  /*~ zif (ObjectSpace != (TnameSpace)0) "Table 'tabKeyWords': change second "
	"byte of each keyword to the new value of ObjectSpace" */ /* cf also
							 'lowLimit', 'maxId'. */
  /*~ zif NbElt(tabKeyWords) != (EKeyWords - BKeyWords)
					      "Table 'tabKeyWords': bad size" */

/* Action to do on characters; Aln must have highest value, cf working of
							     getTokFromTxt(). */
typedef enum {WhiteSpace = (int)ETok1Char, BegFct, IlgCharFct = BegFct,
  EndPrgFct, NlFct, AmpersFct, DollFct, DotFct, DQuoteFct, EMarkFct, EqualFct,
  GTFct, LTFct, MinusFct, PercentFct, PlusFct, QuoteFct, SharpFct, SlashFct,
  StarFct, UpArrowFct, VBarFct, EndFct, Aln = EndFct} TcodChar;
static const TcodChar charInfo[/*~ IndexType TcharStream */] = {   /* ASCII
			   coding assumed; otherwise, just shuffle entries... */
		EndPrgFct	  /* EndCh (to be left there, regardless of
							    character coding. */
		/*~zif __index!=(TcharStream)EndCh
						"Bad position for EndPrgFct" */,
		IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct,
		IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct,
		WhiteSpace,	  /* HT */
		NlFct,		  /* NL */
		WhiteSpace,	  /* VT */
		WhiteSpace,	  /* FF */
		IlgCharFct,	  /* CR */
		IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct,
		IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct,
		IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct, IlgCharFct,
		IlgCharFct, IlgCharFct, IlgCharFct,
		WhiteSpace,	  /* space */
		EMarkFct,	  /* ! */	
		DQuoteFct,	  /* " */
		SharpFct,	  /* # */
		DollFct,	  /* $ */
		PercentFct,	  /* % */
		AmpersFct,	  /* & */
		QuoteFct,	  /* ' */
		(TcodChar)LPAR,	  /* ( */
		(TcodChar)RPAR,	  /* ) */
		StarFct,	  /* * */
		PlusFct,	  /* + */
		(TcodChar)COMMA,  /* , */
		MinusFct,	  /* - */
		DotFct,		  /* . */
		SlashFct,	  /* / */
		Aln+0,		  /* 0 */
		Aln+1, Aln+2, Aln+3, Aln+4, Aln+5, Aln+6, Aln+7, Aln+8,
		Aln+9,		  /* 9 */
		(TcodChar)COLON,  /* : */
		(TcodChar)SCOL,	  /* ; */
		LTFct,		  /* < */
		EqualFct,	  /* = */
		GTFct,		  /* > */
		(TcodChar)QMARK,  /* ? */
		IlgCharFct,	  /* @ */
		Uletter+0,	  /* A */
		Uletter+1,  Uletter+2,  Uletter+3,  Uletter+4,  Uletter+5,
		Uletter+6,  Uletter+7,  Uletter+8,  Uletter+9,  Uletter+10,
		Uletter+11, Uletter+12, Uletter+13, Uletter+14, Uletter+15,
		Uletter+16, Uletter+17, Uletter+18, Uletter+19, Uletter+20,
		Uletter+21, Uletter+22, Uletter+23, Uletter+24,
		Uletter+25,	  /* Z */
		(TcodChar)LSBR,	  /* [ */
		IlgCharFct,	  /* \ */
		(TcodChar)RSBR,	  /* ] */
		UpArrowFct,	  /* ^ */
		Lletter+26,	  /* _ */
		IlgCharFct,	  /* ` */
		Lletter+0,	  /* a */
		Lletter+1,  Lletter+2,  Lletter+3,  Lletter+4,  Lletter+5,
		Lletter+6,  Lletter+7,  Lletter+8,  Lletter+9,  Lletter+10,
		Lletter+11, Lletter+12, Lletter+13, Lletter+14, Lletter+15,
		Lletter+16, Lletter+17, Lletter+18, Lletter+19, Lletter+20,
		Lletter+21, Lletter+22, Lletter+23, Lletter+24,
		Lletter+25,	  /* z */
		(TcodChar)LBRA,	  /* { */
		VBarFct,	  /* | */
		(TcodChar)RBRA,	  /* } */
		(TcodChar)TILDE,  /* ~ */
		IlgCharFct,};	  /* DEL */
/*~ zif NbElt(charInfo)!=LimChar "Array 'charInfo': incorrect size" */

void getTokFromTxt(void)
/* Gets next token */
{
  register char *ptrS;	/* local fast copy of source pointer */
  register char c;	/* current source character */
  register TcodChar cI; /* info on current character */
  static TspeTokRecFct *const tabTokFct[/*~IndexType int */] = {
    &ilgCharF, &endPrgF, &nlF, &ampersF, &dollF, &dotF, &dQuoteF, &eMarkF,
    &eqF, &greaTF, &lessTF, &minusF, &percentF, &plusF, &quoteF, &sharpF,
    &slashF, &starF, &upArrowF, &vBarF};
  /*~ zif NbElt(tabTokFct) != EndFct - BegFct "Array 'tabTokFct': bad size" */

  getTokLvl++;  /* to be able to detect calls at level 0 ('real' calls) */
  spaceCount = SetInGetTokFrmTxt;  /* a priori, no line beginning seen */
  for (;;) {  /* loop because of WHITESPACE and false #if/#elif/#else */
    ptrS = srcPtr;
    /* Put info on next non-blank char in cI */
    for (;;) {
      NxtCh
      if ((ubyte)c >= LimChar) LexErr(NotASCIIChar, charToHexS(c));
      else if ((cI = charInfo[(TcharStream)c]) != WhiteSpace) break;}
    if (cI < Aln) {		/* not an alphanumeric char */
      if (cI < (TcodChar)ETok1Char) {	/* direct token number */
        curTok.tok = (Ttok) cI;
        srcPtr = ptrS;
        goto exitL;}
      /* Else there is a specific function to be called */
      (*tabTokFct[cI- BegFct])(ptrS);
      if (curTok.tok == WHITESPACE) continue;  /* happens in particular
							after 'newline' seen. */
      if (curTok.tok == DOLLVAL) goto idL;
      goto exitL;}
    if (cI < Uletter) {	/* number */
      DoWhatForNb(c)
      srcPtr = ptrS;
      goto exitL;}
    /* Else identifier/keyword */
idL:
    {
      register TdescrId *pDescCurId;  /* position of identifier description in
								symbol table. */
      register char *adBegId = ptrS - 1;  /* address of first character of
				current identifier; identifiers are managed
				so that their name is always in one piece. */
      uint lgtId;		/* length of current identifier */
      ThCode hCode;  /* h-code for current identifier (in 'Object' nameSpace) */

      RestoSmshCh;
      hCode = (ThCode)ObjectSpace*HCodeCst + (ThCode)(cI - Aln);
      for (;;) {  /* collect identifier */
        if ((ubyte)(c = *ptrS++) >= LimChar) break;
        if (CodeCorrectCharForId) {
          ComputeHCode(cI);
          continue;}
        /* Next character not alphanumeric */
        if (c=='\\' && splice(ptrS)) {  /* splicing */
          ptrS--;
          if ((ubyte)(c = *ptrS)<LimChar && (CodeCorrectCharForId)) LexErrPx(
					   DontSplitIdent|Warn1|Rdbl, NULL, +1);
          continue;}
        if (! (c==EndCh && ptrS==posEndChP1)) break;  /* end of identifier */
        /* End of source buffer hit: load some more text and, if appropriate,
           copy already seen portion of identifier into fore buffer, so that
          identifier string always contiguous. */
        lgtId = (uint)(ptrS - adBegId) - 1; /* length of already seen portion */
        if ((ptrS = nxtChunkOfTxt(ptrS))==begSBuf) {  /* next chunk of source
					  text loaded at beginning of buffer. */
          if (lgtId > MaxLgtId) lgtId = MaxLgtId;  /* *doIt* if (adBegId <
	    begSBuf) err(TooBigId); err(TrnctdId) detected in case lgtId>SLI? */
          adBegId = memmove(ptrS - lgtId, adBegId, lgtId);}}
      BackUp;
      srcPtr = ptrS;
      if ((lgtId = (uint)(ptrS - adBegId))==1 && *adBegId=='L' && (c=='\'' ||
	    c=='\"')) {wccSeen = True; continue;}  /* wide-character constant */
      smshCh[1] = *--adBegId;	/* save character about to be smashed */
      *adBegId = (char) ObjectSpace;  /* qualify the name by the nameSpace */
      smshCh[0] = *--adBegId;
      adSmshCh = adBegId;  /* note that characters are smashed */
      curTok.IdName = ConvTname(adBegId);  /* remember address of identifier
								  nameString. */
      if (lgtId > MaxLgtId) {  /* truncate identifier if exceed storage
								    capacity. */
        *(TnameNC)adBegId = (TnameAtom)MaxLgtId;
        hCode = hCodFct(ConvTname(adBegId));}  /* recompute H-code, taking only
						kept characters into account. */
      else *(TnameNC)adBegId = (TnameAtom)lgtId;  /* store identifier length in
							   front of its name. */
      curTok.Hcod = hCode;  /* must be defined even for keywords, in order to
					   differentiate them from d-pragmas. */
      /* Search identifier in symbol table (Object Space) */
      SearchSym(ConvTname(adBegId), MacroVisible)
      curTok.tok = IDENT;
      igndSysMacArgNbP1 = 0;  /* reset flag */
      if ((pDescTokId = pDescCurId) == NULL) {  /* new identifier */
        if (lgtId>SignifLgtId && chkPortbl) err0(TrnctdId | Warn2);  /* warn
									once. */
        curTok.PtrSem = NULL;
        goto exitL;}
      if (pDescCurId->nstLvl < 0) {  /* keyword or macro */
        if (pDescCurId->nstLvl+1 == 0) {  /* keyword */
          if (!noExpand || !kwAsMacName || mngConc) {
            curTok.tok = pDescCurId->NoLex;
            goto exitL;}
          curTok.PtrSem = NULL;}
        expandIfMac();
        if (curTok.tok!=IDENT || curTok.PtrSem==NULL) goto exitL;}
      else curTok.PtrSem = pDescCurId->PidSeman;  /* 'regular' (and already
							existing) identifier. */
      if (curTok.PtrSem->Undf && getTokLvl==tokLvl0) errWN(UndfndId|Warn1|Rdbl,
								curTok.IdName);}
exitL:
    if (! condDirSkip) {
      if (--getTokLvl == 0) storeTokTxt(curTokTxt());  /* store token in "last
				tokens list" only if 'real' (level 0) call to
				GetNxtTok (and '+zlt' option on). */
      return;}
    analTok();}  /* skip token and loop if inside false '#if' arm */
}

static void ampersF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '&') {curTok.tok = LOGAND;}
  else if (c == '=') {curTok.Val = (uint)AndAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.tok = APSAND;}
  srcPtr = ptrS;
}

static void dollF(register char *ptrS)
{
  if (! (sysAdjHdrFile || dollAlwdInId)) ilgCharF(ptrS);
  else curTok.tok = DOLLVAL;
}

static void dotF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '.') {
    NxtCh
    if (c == '.') curTok.tok = ELLIP;
    else {BackUp; LexErr(IlgSynt, NULL); curTok.tok = DOT;}}
  else {
    BackUp;
    if (InsideInterval(charInfo[(TcharStream)c], Aln, Aln + ('9' - '0'))
							      ) DoWhatForNb('.')
    else curTok.tok = DOT;}
  srcPtr = ptrS;
}

static void dQuoteF(char *ptrS)
{
  curTok.tok = CSTST;
  curTok.Val = (uint)wccSeen;
  wccSeen = False;
  frstChOfNb = True;  /* analStrCst() not called yet */
  srcPtr = ptrS;
}

static void eMarkF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)NE; curTok.tok = EQUALOP;}
  else {BackUp; curTok.tok = EMARK;}
  srcPtr = ptrS;
}

static void eqF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)EQ; curTok.tok = EQUALOP;}
  else {BackUp; curTok.Val = (uint)SimplAsgn; curTok.tok = ASGNOP;}
  srcPtr = ptrS;
}

static void greaTF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)GE; curTok.tok = ORDEROP;}
  else if (c == '>') {
    NxtCh
    if (c == '=') {curTok.Val = (uint)RShAsgn; curTok.tok = ASGNOP;}
    else {BackUp; curTok.Val = (uint)RSh; curTok.tok = SHIFTOP;}}
  else {BackUp; curTok.Val = (uint)GT; curTok.tok = ORDEROP;}
  srcPtr = ptrS;
}

void ilgCharF(register char *ptrS)
{
  static Tchar errCh[] = " ";
  char badCh = *(ptrS - 1);

  srcPtr = ptrS;
  errCh[0] = (badCh!='\r' && badCh!='\b')? (Tchar)badCh : '\0';
  errWSS((badCh == '$')? IlgCharD : IlgChar, errCh, charToHexS(badCh));
  curTok.tok = WHITESPACE;
}

static void lessTF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)LE; curTok.tok = ORDEROP;}
  else if (c == '<') {
    NxtCh
    if (c == '=') {curTok.Val = (uint)LShAsgn; curTok.tok = ASGNOP;}
    else {BackUp; curTok.Val = (uint)LSh; curTok.tok = SHIFTOP;}}
  else {BackUp; curTok.Val = (uint)LT; curTok.tok = ORDEROP;}
  srcPtr = ptrS;
}

static void minusF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '-') {curTok.Val = (uint)Dec; curTok.tok = INCOP;}
  else if (c == '=') {curTok.Val = (uint)SubAsgn; curTok.tok = ASGNOP;}
  else if (c == '>') curTok.tok = ARROW;
  else {BackUp; curTok.Val = (uint)Sub; curTok.tok = ADDOP;}
  srcPtr = ptrS;
}

static void nlF(register char *ptrS)
{
  register char c;

  if (dirLineFl) {  /* end of preprocessor line */
    dirLineFl = False;
    condDirSkip = False;  /* will be re-set by 'manageDir()' */
    curTok.tok = ENDDIR;}
  else {
    if (spaceCount != CmtAtBegLine) oldSpaceCount = spaceCount; /* ignore
							       comment lines. */
    for (;;) {
      IncLineNb()
      spaceCount = 0;
      for (;;) {
        NxtCh
        if ((ubyte)c >= LimChar) LexErr(NotASCIIChar, charToHexS(c));
        else if (charInfo[(TcharStream)c] != WhiteSpace) break;
        else if (c == '\t') ManageTab(spaceCount);
       	else spaceCount++;}
      if (c == '#') {	/* beginning of preprocessor line */
        dirLineFl = True;
        srcPtr = ptrS;
        manageDir();
        ptrS = srcPtr;}
      else break;}
    BackUp;
    curTok.tok = WHITESPACE;}
  srcPtr = ptrS;
}

static void percentF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)ModAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.Val = (uint)Mod; curTok.tok = MULOP;}
  srcPtr = ptrS;
}

static void plusF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '+') {curTok.Val = (uint)Inc; curTok.tok = INCOP;}
  else if (c == '=') {curTok.Val = (uint)AddAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.Val = (uint)Add; curTok.tok = ADDOP;}
  srcPtr = ptrS;
}

static void quoteF(register char *ptrS)
{
  uint nbChar = 0;
  char c;

  curTok.ErrorT = False;
  for (;;) {
    NxtCh
    if (c == '\'') break;
    ManageLitChar(UnFnshCharCst)
    curTok.Val = (uint)c;
    nbChar++;}
  if (nbChar != 1) {LexErrPhase4(LgtCharCstNotOne|Warn3|PossErr, NULL); curTok.
								 ErrorT = True;}
  curTok.tok = (curTok.ErrorT)? CSTCH1 : CSTCH;
  if (wccSeen) {curTok.tok += CSTWCH - CSTCH; wccSeen = False;}
  srcPtr = ptrS;
}

static void sharpF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '#') curTok.tok = (mngConc)? CONCATOP1 : CONCATOP;
  else {BackUp; curTok.tok = QUOTEOP;}
  if (! (dirLineFl || mngConc)) {LexErrPhase4(LegalOnlyInMac, NULL); curTok.
			tok = (curTok.tok == CONCATOP)? CONCATOP1 : WHITESPACE;}
  srcPtr = ptrS;
}

static void slashF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '*') {	/* comment, or d-pragma */
    if (InsideDPragma) LexErrPhase4(UnclosedDP, NULL);
    NxtCh
    if (c=='~' && !InsideDPragma && !condDirSkip) {  /* d-pragma */
      TindentChk savOldSpCnt = oldSpaceCount, savSpCnt = spaceCount;

      srcPtr = ptrS;
      curTok = manageDPrag();
      oldSpaceCount = savOldSpCnt; spaceCount = savSpCnt;
      return;}
    if (mngConc) return;
    for (;;) {
      while (c == '/') {
        NxtCh
        if (c == '*') {
          NxtCh
          if (c == '/') goto exitL;  /* end comment */
          LexErrPx(UnclosedCmt|Warn2|PossErr, NULL, -1);}}
      while (c == '*') {
        NxtCh
        if (c == '/') goto exitL;}  /* end comment */
      if (fileClosed && c==EndCh) {LexErr(UnFnshCmt, NULL); BackUp; break;}
      if (c == '\n') IncLineNb()
      NxtCh}
exitL:
    curTok.tok = (InsideDPragma && !condDirSkip)? ENDDPRAG : WHITESPACE;
    if (spaceCount >= 0) spaceCount = CmtAtBegLine;}  /* for always correct
			indentation check after comment at beginning of line. */
  else if (c == '=') {curTok.Val = (uint)DivAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.Val = (uint)Div; curTok.tok = MULOP;}
  srcPtr = ptrS;
}

static void starF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)MulAsgn; curTok.tok = ASGNOP;}
  else if (InsideDPragma && c=='/') curTok.tok = ENDDPRAG;
  else {BackUp; curTok.tok = STAR;}
  srcPtr = ptrS;
}

static void upArrowF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.Val = (uint)XorAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.tok = XOR;}
  srcPtr = ptrS;
}

static void vBarF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '|') {curTok.tok = LOGOR;}
  else if (c == '=') {curTok.Val = (uint)IorAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.tok = IOR;}
  srcPtr = ptrS;
}

/******************************************************************************/
/*				    UTILITIES				      */
/******************************************************************************/

void analNumCst(void(*pPutC)(char /*~Generic*/))
/* The analysis of a constant is only done now so that it is not converted
   each time it is found (for example, during macro body storage or macro para-
   meter passing), but only when used (pPutC == Convert). */
{
#define StoreAndGetCh	{putCh(c); c=(*pNxtCh)();}
  uint curDigVal, base, base1;
  TcalcU value;
  TcharStream c;  /* 'TcharStream' for efficiency */
  bool eSeen;  /* exponent seen */

  if (! CurTokNotAnalz()) return;  /* if analNumCst() already called */
  pPutCh = pPutC;
  eSeen = False;  /* exponent seen */
  curTok.ErrorT = False;
  base1 = base = Base10;  /* 'base1' needed because 0 alone => base 10... */
  if ((c = (TcharStream)frstChOfNb) == (TcharStream)'.') {
    curTok.Val |= FltSeen;
    StoreAndGetCh
    frstChOfNb = False;}
  value = (TcalcU)(charInfo[c] - Aln);
  StoreAndGetCh
  frstChOfNb = False;
  if (value==0 && !(curTok.Val & FltSeen)) {
    if (c==(TcharStream)'x' || c==(TcharStream)'X') {base1 = Base16
		      ; StoreAndGetCh}  /* *doIt*  only hex nb if digit after */
    else base1 = Base8;}
  for (;;) {
    if ((uint)c >= LimChar) break;
    if ((curDigVal = (uint)(charInfo[c] - Aln)) >= base1) {
      if (base1==Base16 && InsideInterval(c, (TcharStream)'a', (TcharStream)'f'
					      )) curDigVal -= Lletter - Uletter;
      else if (curDigVal < Base10) syntErrNb(IlgOctDig, charToHexS((char)c));
      else {
        if (c == (TcharStream)'.') {if ((curTok.Val & FltSeen) != 0 || base!=
	    Base10) break; base1 = Base10; curTok.Val |= FltSeen; goto noConvL;}
        if (c==(TcharStream)'e' || c==(TcharStream)'E') {
          if (eSeen || base!=Base10) break;
	  eSeen = True;
          curTok.Val |= FltSeen;
          StoreAndGetCh
          if (c==(TcharStream)'+' || c==(TcharStream)'-') StoreAndGetCh
          if ((ubyte)c>=LimChar || !InsideInterval(charInfo[c], Aln, Aln +
				('9' - '0'))) {syntErrNb(WrngNb1, NULL); break;}
          base1 = Base10;
          goto noConvL;}
        break;}}
    base = base1;
    if (pPutC == Convert) {
      if (! (curTok.Val & FltSeen)) {  /* compute binary number */
        uint w = base >> 1;  /* for 'base' always even */
        TcalcU resulMult = 0, value1;

        do {  /* multiply bitwise by 'base' to detect possible overflow */
          value1 = value << 1;
          if (value1>>1 != value) semanErrNb();
          value = value1;
          if (w & 1) {
            TcalcU temp = resulMult + value1;

            if (temp < resulMult) semanErrNb();
            resulMult = temp;}
        } while ((w >>= 1) != 0);
        value = resulMult + (TcalcU)curDigVal;
        if (value < resulMult) semanErrNb();}
      else if (curDigVal != 0) value = 1 + 1;} /* to get NumCstShdBeNamed msg */
noConvL:
    StoreAndGetCh}
  /* Process possible suffixes */
  if (curTok.Val & FltSeen) {  /* floating constant */
    uint w = (c==(TcharStream)'f' || c==(TcharStream)'F')? FSeen : (c==
			    (TcharStream)'l' || c==(TcharStream)'L')? LDSeen : 0;

    curTok.NumVal = (TcalcS)value;  /* until better processing ... */
    if (w != 0) {curTok.Val |= w; StoreAndGetCh}}
  else {  /* integer constant */
    uint w;

    curTok.NumVal = (TcalcS)value;
    for (;;) {
      switch(c) {
      case (TcharStream)'u': case (TcharStream)'U': w = USeen; break;
      case (TcharStream)'l': case (TcharStream)'L':
#ifndef LONGLONG
        if (sysAdjHdrFile)  /* 'LL' always accepted in system header files */
#endif
        {
          if (curTok.Val & (LSeen | LLSeen)) {
            w = LLSeen;
            curTok.Val &= ~LSeen;
            break;}}
        w = LSeen; break;
      default:
        if (base != Base10) curTok.Val |= OctHex;
        if ((TcalcS)value < 0) {  /* a-priori, overflow */
          if (curTok.Val & OctHex) curTok.Val |= USeen;  /* if octal or hex,
							 convert to unsigned. */
          else if (pPutC==Convert && curTok.Val & SignSeen && (!(curTok.Val &
				  NegSeen) || -(TcalcS)value>=0)) semanErrNb();}
        goto exitNbL;}
      if (curTok.Val & w) syntErrNb(WrngNb, NULL);
      curTok.Val |= w;
      StoreAndGetCh}}
exitNbL:
  /* Manage possible pp-number suffix */
  {
    char *pPPNbChar = NULL;

    if (isAlnu((char)c) || c==(TcharStream)'.') {
      uint bufSiz;
      const char *endPPNbBuf = NULL;

      if ((int)insideDefineBody > 0) err0(MsngSpace|Warn1|Rdbl);
      if (pPPNbCtx != NULL) pPutC = NoConv;  /* do not store twice in pp-nb
					     buffer (e.g. in case "00.035."). */
      do {
        if (pPutC == Convert) {
          /* Copy pp-number suffix in contiguous buffer */
          if (pPPNbChar >= endPPNbBuf) {  /* buffer full */
            if (pPPNbChar == NULL) bufSiz = LgtHdrId;  /* LgtHdrId bytes to
			      be set aside at beginning of any source buffer. */
            else if (pPPNbChar > endPPNbBuf) bufSiz++;  /* storing sign
								  overflowed. */
#define PP_NB_DELTA	10
            MyRealloc(pPPNbCtx, (sizeof(TppNbCtx) + PP_NB_DELTA + 1 + 1 -
		LgtHdrId) + bufSiz);  /* +1 because of ending NUL; +1 because
					   of unprotected storing of sign. */
            pPPNbChar = &pPPNbCtx->ppNbSuffixBuf[bufSiz];
            endPPNbBuf = pPPNbChar + PP_NB_DELTA;
            bufSiz += PP_NB_DELTA;}
#undef PP_NB_DELTA
          *pPPNbChar++ = (char)c;}
        else putCh(c);
        if (c==(TcharStream)'e' || c==(TcharStream)'E') {
          c = (*pNxtCh)();
          if (c!=(TcharStream)'-' && c!=(TcharStream)'+') continue;
          if (pPPNbChar != NULL) *pPPNbChar++ = (char)c;
          else putCh(c);}
        c = (*pNxtCh)();
      } while (isAlnu((char)c) || c==(TcharStream)'.');}
    if (pNxtCh == &nxtChFromTxt) backUp();
    if (pPPNbChar != NULL) {
      *pPPNbChar = '\0';  /* ending NUL */
      pPPNbCtx->savSrcPtr = srcPtr;
      pPPNbCtx->savPGNT = pCurGNT;
      pPPNbCtx->savME = macroExpand;
      srcPtr = &pPPNbCtx->ppNbSuffixBuf[LgtHdrId];
      pCurGNT = &getTokFromTxt;
      pNxtCh = &nxtChFromTxt;
      macroExpand = False;}}
  restoPnxtChState();
  #undef StoreAndGetCh
}

void analStrCst(void(*pPutC)(char /*~Generic*/))
/* The analysis of a string constant is only done now, so that it can be stored
   at different places (for example, during macro body storage or for real
   use). */
{
  if (! CurTokNotAnalz()) return;  /* if analStrCst() already called */
  frstChOfNb = False;
  curTok.ErrorT = False;
  pPutCh = pPutC;
  if (! MacroTxt) {  /* not macro expansion */
    register char *ptrS = srcPtr;
    char c;

    for (;;) {
      NxtCh
      if (c == '\"') break;
      ManageLitChar(UnFnshStrCst)
      putCh((TcharStream)c);}
    srcPtr = ptrS;}
  else
    for (;;) {
      TcharStream c;

      if ((c = (*pNxtCh)()) < 0) break;
      putCh(c);}
  if (getTokLvl == 0) storeTokTxt("\"");
  restoPnxtChState();
}

void analTok(void)
{
  if (CurTokNotAnalz())
    if (curTok.tok == CSTST) analStrCst(NoConv);
    else analNumCst((getTokLvl == 0)? Convert /* to get space between consti-
					     tuents of pp-numbers. */ : NoConv);
}

void backUp(void)
{
  srcPtr--;
}

Tstring curTokTxt(void)
/* Yields the string representation of current token */
{
#include "dctxttok.h"
  switch (curTok.tok) {
  case ADDOP:    return txtAddTok[(TkAdd)curTok.Val];
  case ASGNOP:   return txtAsgnTok[(TkAsgn)curTok.Val];
  case CSTCH:
  case CSTCH1:
  case CSTWCH:
  case CSTWCH1:
    pTokTxt = &tokTxtBuf[0];  /* for putTokChar() */
    if (curTok.tok==CSTWCH || curTok.tok==CSTWCH1) *pTokTxt++ = 'L';
    *pTokTxt++ = '\'';
    if (curTok.tok < CSTCH1) {  /* not erroneous character */
      if ((char)curTok.Val == '\'') *pTokTxt++ = '\\';
      pStoreChFct = &putTokChar;
      storeTokTxtRepr((char)curTok.Val);
      pStoreChFct = &saveTokChar;}
    *pTokTxt++ = '\'';
    *pTokTxt = '\0';
    return &tokTxtBuf[0];
  case CSTNU:
  case CSTNU1:   return "";
  case CSTST:    return ((bool)curTok.Val)? "L\"" : "\"";
  case DPRAG:    return DPragPrefix;
/*case EQUALOP: see ORDEROP */
/*case FORCEMACEXP: see NOMACEXP */
  case IDENT:    return nameToS(curTok.IdName);
  case INCOP:    return txtIncTok[(TkInc)curTok.Val];
  case MULOP:    return txtMulTok[(TkMul)curTok.Val];
  case NOMACEXP: case FORCEMACEXP: return nameToS(pDescTokId->idName);
  case ORDEROP:  case EQUALOP: return txtCmpTok[(TkCmp)curTok.Val];
  case SHIFTOP:  return txtShiTok[(TkShi)curTok.Val];
  default: {
      static const Tstring tok1ChTxtRepr[/*~IndexType Ttok*/] = {"", ":", ",",
             "{", "(", "[", "->", ".", "?", "}", ")", "]", ";", "~", "#", "##"};
      /*~zif NbElt(tok1ChTxtRepr) != (int)ETok1Char+2
                                                  "Bad 'tok1ChTxtRepr' array" */
      static const Tstring tok2ChTxtRepr[] = {NULL, "&", "^", "|", "&", "...",
"!", "", NULL, NULL, "&&", "||", NULL, NULL, NULL, "*", DPragSuffix, NULL};
      /*~zif NbElt(tok2ChTxtRepr) != BSideEffTok-EKeyWords
                                                  "Bad 'tok2ChTxtRepr' array" */
      if (curTok.tok < BKeyWords) return tok1ChTxtRepr[curTok.tok];
      if (curTok.tok < EKeyWords) return (curTok.tok==DEFAULT && curTok.Val==0)
                ? ""
                : (Tstring)tabKeyWords[curTok.tok - BKeyWords] + LgtHdrId;
      if (curTok.tok < BSideEffTok) return tok2ChTxtRepr[curTok.tok -EKeyWords];
      return "";}}
}

void findIndentCnt(void)
{
  register char *ptrS = srcPtr;
  char c;
  TindentChk indentCtr = spaceCount + 1;

  for (;;) {
    NxtCh
    if (c == ' ') {indentCtr++; continue;}
    if (c == '\t') {ManageTab(indentCtr); continue;}
    BackUp;
    break;}
  srcPtr = ptrS;
  GetNxtTok();
  if (c != '\n') spaceCount = indentCtr;
}

void fullCurTokTxt(void(*pPutC)(Tchar))
{
  Tstring w = curTokTxt();

  while (*w != '\0') (*pPutC)(*w++);
  if (CurTokNotAnalz())
    if (curTok.tok == CSTST) {
      pStoreChFct = pPutC;  /* for storeTokTxtRepr() */
      analStrCst(&storeTokTxtRepr);
      pStoreChFct = &saveTokChar;
      (*pPutC)('"');}
    else analNumCst(pPutC);
}

static char *getEscSeqChar(register char *ptrS, char *pch)
/* Manages escape sequences for string or character constant */
{
  char c, errC;
  uint charVal;

  NxtCh
  if (InsideInterval(c, '0', '9')) {  /* octal constant */
    int i;

    charVal = 0;
    for (i = 0; i < MaxNbCharRepr; i++) {
      uint curDigVal;

      if ((curDigVal = (uint)(c - '0')) > (uint)('7' - '0')) {LexErrPhase4(
			       IlgOctDig, charToHexS(c)); curTok.ErrorT = True;}
      charVal = (charVal<<BitsOctDigit) + curDigVal;
      NxtCh
      if (! InsideInterval(c, '0', '9')) break;}
    BackUp;}
  else {
    const char *ptr;
    static const char speChar[] = {'n','t','\'','\"','\\','\?','v','b','r','f',
								       'a','x'};
    static const char correspChar[/*~IndexType ptrdiff_t */] = {'\n','\t','\'',
				  '\"','\\','\?','\v','\b','\r','\f','\a','\0'};
    /*~ zif (NbElt(speChar) != NbElt(correspChar)) "Pb 'speChar'" */

    for (ptr = &speChar[0]; ptr < AdLastEltP1(speChar); ) {
      if (c == *ptr++) {
        bool digitSeen;

        if ((charVal = (uint)correspChar[ptr - &speChar[1]]) != 0) goto okL;
        /* Hexadecimal constant */
        digitSeen = False;
        for (;;) {
          uint curDigVal;

          NxtCh
          if ((ubyte)c >= LimChar) break;
          if ((curDigVal = (uint)(charInfo[(TcharStream)c] - Aln))>=Base16) {
            if (! InsideInterval(c, 'a', 'f')) break;
            curDigVal -= Lletter - Uletter;}
          digitSeen = True;
          charVal = (charVal<<BitsHexDigit) + curDigVal;
          if (charVal > UCHAR_MAXC) charVal = UCHAR_MAXC + 1;}  /* to detect
								    overflow. */
        BackUp;
        if (digitSeen) goto okL;
        errC = '\0';
        goto errL;}}
    errC = c;
errL:
    LexErrPhase4(IlgEscSeq, charToHexS(errC));
    if (fileClosed && c==EndCh) BackUp;
    charVal = 1;  /* to avoid a fake NUL character */
    curTok.ErrorT = True;}
okL:
  if (charVal > UCHAR_MAXC) {LexErrPhase4(Overflow1 | Warn3, NULL); curTok.
								 ErrorT = True;}
  *pch = (char)charVal;
  return ptrS;
}

ThCode hCodFct(register Tname x)
/* Computed hash-code (=> curTok.hCod) guaranteed non zero */
{
  uint lgt;
  ThCode hCode;

  lgt = (uint)*x++;	/*~ zif (DispLgtId != 0) "Pb in hCodFct()" */
  hCode = (ThCode)*x++;	/*~ zif (DispNSId != 1) "Pb in hCodFct()" */
  do {ComputeHCode(charInfo[(TcharStream)*x++]);} while (--lgt != 0);
  return hCode;
}

static void initKeyWords(void)
{
  const Tname *pExplTabKW;
  TdescrId *ptrDescKW;

  for (pExplTabKW = &tabKeyWords[0]; pExplTabKW < AdLastEltP1(tabKeyWords);
								 pExplTabKW++) {
    if (*pExplTabKW != NULL) {
      ptrDescKW = enterSymTabHC1(*pExplTabKW, hCodFct(*pExplTabKW));
      ptrDescKW->NoLex = BKeyWords + (int)(pExplTabKW - &tabKeyWords[0]);}}
  ctrDIE = 0;
}

void initRec(void)
{
  static bool frstInit = True;

  if (frstInit) {initKeyWords(); frstInit = False;}
  initStoreTokTxt();
}

bool isAlnu(char c)
{
  return ((ubyte)c >= LimChar)
         ? False
         : (c == '$')
           ? sysAdjHdrFile || dollAlwdInId
           : (charInfo[(TcharStream)c] >= Aln);
}

static void putCh(TcharStream c)
{
  if (GtPtr(pPutCh, NoConv)) (*pPutCh)((char)c);
  if (getTokLvl==0 && !frstChOfNb) storeTokTxtRepr((char)c);
}

static void putTokChar(char c)
{
  if (pTokTxt < &tokTxtBuf[NbElt(tokTxtBuf) - 1]) *pTokTxt++ = (Tchar)c;
}

static void semanErrNb(void)
{
  if (! curTok.ErrorT) {
    err0(Overflow1 | Warn2);
    curTok.ErrorT = True;}
}

void skipTok(const Ttok stopTokenList[])
{
  for (; curTok.tok != ENDPROG; GetNxtTok()) {
    register const Ttok *ptrT;
    register Ttok currTok = curTok.tok;

    if (currTok == DPTOK) {
      switch ((Tdprag)curTok.Val) {
        case CASTTO: case INDEXTYPE:
          FreeExpType1(curTok.DpType);
          break;
        /*~NoDefault*/}}
    else if (currTok == ENDDPRAG) dpragNst--;
    analTok();
    /* Search whether current token inside 'stop tokens' list */
    ptrT = &stopTokenList[0];
    do {
      if (currTok == *ptrT++) {
        if (ptrT == &stopTokenList[1]) {
          if (currTok==LBRA || currTok==RBRA || currTok==SCOL || currTok==
						       COMMA) ignoreErr = False;
          GetNxtTok();}  /* 'swallow' first token */
        return;}
    } while (*ptrT != EndSTok);}
}

static Tchar prevChar = ' ';

static void initStoreTokTxt(void)
{
  prevChar = ' ';
}

void storeTokTxt(Tstring w)
{
  if (frstChOfNb > True) {  /* number */
    /* Put 1st character of constant in '+zlt' buffer */
    static Tchar skelStr[] = " ";

    skelStr[0] = (Tchar)frstChOfNb;
    w = &skelStr[0];}
  /* Insert separating space if necessary */
  if (   isAlnu(prevChar) && isAlnu(*w)
      || (*w==prevChar && strchr("+-<>=&|", *w)!=NULL || *w=='=')
         && strchr("+-<>=!&|*/%^.", prevChar)!=NULL) (*pStoreChFct)(' '); /* so
				that "==-(3)" or "+ =" outputted identically. */
  if (*w != '\0') { /* some token texts may be empty (for example, ? tokens). */
    do {
      (*pStoreChFct)(*w++);
    } while (*w != '\0');
    prevChar = *(w - 1);}
}

/*~Undef prevChar */

static void storeTokTxtRepr(char c)
{
  if (isprint(c)) {
    if (c=='\"' || c=='\\') (*pStoreChFct)('\\');
    (*pStoreChFct)(c);
    return;}
  {  /* Replace invisible characters */
    static const char tSpeChar[] = "\n\t\v\b\r\f\a";
    const char *ptr = &tSpeChar[0];

    (*pStoreChFct)('\\');
    /* Search if invisible character has a name */
    do {
      if (*ptr++ == c) {(*pStoreChFct)("ntvbrfa"[ptr - &tSpeChar[1]]); return;}
    } while (*ptr != '\0');}
  {  /* Else output hex coding (to draw attention on fact that not decimal) */
    Tstring w = charToHexS(c);

    if ((uint)c >= Base8) (*pStoreChFct)('x');
    do {(*pStoreChFct)(*w++);} while (*w != '\0');}
}

static void syntErrNb(Terr n, Tstring y)
{
  if (!MacroTxt && !condDirSkip) errWS(n, y);  /* msg only at macro collection
									time. */
  curTok.ErrorT = True;
}

/* End DCREC.C */
