/* DCREC.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#include <ctype.h>
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
#include "dcprag.ph"
#include "dcrecdir.ph"

#ifdef VMS
#  pragma noinline (putTokChar, semanErrNb, storeTokSource, syntErrNb)
#endif

#define Base8		8
#define Base16		16
#define BitsOctDigit	3
#define ComputeHCode(x) hCode = hCode*HCodeCst + (ThCode)(x - Aln)
#define DfltOptVal	-1
#define DoWhatForNb(x) {						       \
  curTok.val = 0;  /* reset flags for analNumCst() */			       \
  frstChOfNb = x;							       \
  curTok.tok = CSTNU;}
#define HCodeCst	5  /* changing this constant (that must stay odd for
		      good scrambling) means changing all hardwired Hcodes... */
#define LexErr(x, y)	srcPtr = ptrS, errWS(x, y)
#define LexErrPhase4(x, y) {if (! condDirSkip) LexErr(x, y);}
#define LgtD(x)		(size_t)*x
#define LimChar		(1<<NbBitsChar)
#define Lletter		(Uletter + ('z' - 'a' + 1))
#define ManageLitChar(x)						       \
  if (c=='\n' || c==EndCh && fileClosed) {LexErr(x, NULL); BackUp; break;}     \
  if (c == '\\') ptrS = getEscSeqChar(ptrS, &c);			       \
  else if (!IsVisibleChar(c) && c!='\t')				       \
    LexErr(NotVisiChar | Warn2, charToHexS(c));
#define MaxNbCharRepr	3
#define ResetSpaceAtEndOfL spaceAtEndOfLine = (wrapFl)? LineSize : INT_MAX
#define SizeBufDef	LineSize
#define SizeSourceBuffer (NbSrcLinesKept*(LineSize + TransfSize) + 1 + 1 + 1)
#define Uletter		(Aln + Base10)

/* Function profiles */
static char *getEscSeqChar(char *, char *);
static TcharStream nxtChFromConc(void);
typedef void TspeTokRecFct(char *);
static TspeTokRecFct nlF, ampersF, dollF, dotF, dQuoteF, eMarkF, eqF, greaTF,
  lessTF, minusF, nbSignF, percentF, plusF, quoteF, slashF, starF, upArrowF,
  vBarF;
static void initKeyWords(void), initStoreTokTxt(void), putCh(char), putTokChar(
  char), restoNormState(void), semanErrNb(void), syntErrNb(Terr, Tstring);

/* Global variables */
static char *pConcTxt;
static void (*pPutCh)(char);
static TstringNC pTokTxt;
static Ttok white = WHITESPACE;

/* External variables */

char *adSmshCh;  /* address of characters currently smashed in source buffer by
		     an identifier header (couple id. length, id. nameSpace). */
Terr concatErr = NoErrMsg;
bool dirLineFl = False;  /* True if currently processing directive line */
char frstChOfNb = '\0';
bool mngConc = False;  /* True if currently doing ## concatenation */
TdescrId *pDescCurId;
void (*pStoreChFct)(char) = &saveTokChar;
char smshCh[LgtHdrId];  /* buffer to save characters smashed in source
						buffer 	by identifier header. */
char *srcPtr;  /* source pointer */
TdescrId *symTabHeads[SizeSymTab];  /* symbol table (heads of 'small tables') */
Tchar tokTxtBuf[MaxLgtId + 1];  /* general use buffer */


static const Tname tabKeyWords[] = { 
    /* First byte: length; 2nd byte: name space */
    NULL,			/* NOATTRIB */
    ConvTname("\6\0static"),
    ConvTname("\6\0extern"), ConvTname("\7\0typedef"),
    ConvTname("\4\0auto"), ConvTname("\x8\0register"), ConvTname("\5\0const"),
    ConvTname("\x8\0volatile"), ConvTname("\4\0enum"), ConvTname("\6\0struct"),
    ConvTname("\5\0union"), ConvTname("\4\0void"), ConvTname("\5\0float"),
    ConvTname("\6\0double"), ConvTname("\4\0char"), ConvTname("\5\0short"),
    ConvTname("\3\0int"), ConvTname("\4\0long"), ConvTname("\x8\0unsigned"),
    ConvTname("\6\0signed"), ConvTname("\5\0break"), ConvTname("\4\0case"),
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
typedef enum {WhiteSpace = (int)ETok1Char, BegFct,
  IlgCharFct = BegFct, EndPrgFct, NlFct, AmpersFct, DollFct, DotFct, DQuoteFct,
  EMarkFct, EqualFct, GTFct, LTFct, MinusFct, NbSignFct, PercentFct, PlusFct,
  QuoteFct, SlashFct, StarFct, UpArrowFct, VBarFct, EndFct, Aln = EndFct
} TcodChar;
static const TcodChar charInfo[/*~ IndexType char */] = {   /* ASCII coding
				  assumed; otherwise, just shuffle entries... */
		EndPrgFct	  /* EndCh (to be left there, regardless of
							    character coding. */
		/*~zif __index!=EndCh "Bad position for EndPrgFct" */,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		WhiteSpace,	  /* HT */
		NlFct,		  /* NL */
		WhiteSpace,	  /* VT */
		WhiteSpace,	  /* FF */
		IlgCharFct,	  /* CR */
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		IlgCharFct,
		WhiteSpace,	  /* space */
		EMarkFct,	  /* ! */	
		DQuoteFct,	  /* " */
		NbSignFct,	  /* # */
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
		Aln+1,
		Aln+2,
		Aln+3,
		Aln+4,
		Aln+5,
		Aln+6,
		Aln+7,
		Aln+8,
		Aln+9,		  /* 9 */
		(TcodChar)COLON,  /* : */
		(TcodChar)SCOL,	  /* ; */
		LTFct,		  /* < */
		EqualFct,	  /* = */
		GTFct,		  /* > */
		(TcodChar)QMARK,  /* ? */
		IlgCharFct,	  /* @ */
		Uletter+0,	  /* A */
		Uletter+1,
		Uletter+2,
		Uletter+3,
		Uletter+4,
		Uletter+5,
		Uletter+6,
		Uletter+7,
		Uletter+8,
		Uletter+9,
		Uletter+10,
		Uletter+11,
		Uletter+12,
		Uletter+13,
		Uletter+14,
		Uletter+15,
		Uletter+16,
		Uletter+17,
		Uletter+18,
		Uletter+19,
		Uletter+20,
		Uletter+21,
		Uletter+22,
		Uletter+23,
		Uletter+24,
		Uletter+25,	  /* Z */
		(TcodChar)LSBR,	  /* [ */
		IlgCharFct,	  /* \ */
		(TcodChar)RSBR,	  /* ] */
		UpArrowFct,	  /* ^ */
		Lletter+26,	  /* _ */
		IlgCharFct,	  /* ` */
		Lletter+0,	  /* a */
		Lletter+1,
		Lletter+2,
		Lletter+3,
		Lletter+4,
		Lletter+5,
		Lletter+6,
		Lletter+7,
		Lletter+8,
		Lletter+9,
		Lletter+10,
		Lletter+11,
		Lletter+12,
		Lletter+13,
		Lletter+14,
		Lletter+15,
		Lletter+16,
		Lletter+17,
		Lletter+18,
		Lletter+19,
		Lletter+20,
		Lletter+21,
		Lletter+22,
		Lletter+23,
		Lletter+24,
		Lletter+25,	  /* z */
		(TcodChar)LBRA,	  /* { */
		VBarFct,	  /* | */
		(TcodChar)RBRA,	  /* } */
		(TcodChar)TILDE,  /* ~ */
		IlgCharFct,};	  /* DEL */
/*~ zif NbEltGen(charInfo, '\0')!=LimChar "Array 'charInfo': incorrect size" */

void getTokFromTxt(void)
/* Gets next token */
{
  register char *ptrS;	/* local fast copy of source pointer */
  register char c;	/* current source character */
  register TcodChar cI; /* info on current character */
  static TspeTokRecFct *const tabTokFct[/*~IndexType int */] = {
    &ilgCharF, &endPrgF, &nlF, &ampersF, &dollF, &dotF, &dQuoteF, &eMarkF,
    &eqF, &greaTF, &lessTF, &minusF, &nbSignF, &percentF, &plusF, &quoteF,
    &slashF, &starF, &upArrowF, &vBarF};
  /*~ zif NbElt(tabTokFct) != EndFct - BegFct "Array 'tabTokFct': bad size" */

  getTokLvl++;  /* to be able to detect calls at level 0 ('real' calls) */
  spaceCount = SpaceCntNoBegLine;  /* a priori, no line beginning seen */
  for (;;) {  /* because of WHITESPACE and false #if/#elif/#else */
    ptrS = srcPtr;
    /* Put info on next non-blank char in cI */
    for (;;) {
      NxtCh
      if ((ubyte)c >= LimChar) LexErr(NotASCIIChar, charToHexS(c));
      else if ((cI = charInfo[c]) != WhiteSpace) break;}
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
      register char *adBegId = ptrS - 1;  /* address of first character of
				current identifier; identifiers are managed
				so that their name is always in one piece. */
      int lgtId;		/* length of current identifier */
      ThCode hCode;  /* h-code for current identifier (in 'Object' nameSpace) */
      register TdescrId *pCurId;  /* position of identifier description in
								symbol table. */
      RestoSmshCh;
      hCode = (ThCode)ObjectSpace*HCodeCst + (ThCode)(cI - Aln);
      for (;;) {  /* collect identifier */
        if ((ubyte)(c = *ptrS++) >= LimChar) break;
        if ((cI = charInfo[c])>=Aln || (c=='$' && (sysAdjHdrFile ||
							       dollAlwdInId))) {
          ComputeHCode(cI);
          continue;}
        /* Next character not alphanumeric */
        if (c=='\\' && splice(ptrS)) {ptrS--; continue;}  /* splicing */
        if (c!=EndCh || ptrS!=posEndChP1) break;  /* end of identifier */
        /* End of source buffer hit: load some more text and, if appropriate,
           copy already seen portion of identifier into fore buffer, so that
          identifier string always contiguous. */
        lgtId = (int)(ptrS - adBegId) - 1;  /* length of already seen portion */
        if ((ptrS = nxtChunkOfTxt(ptrS))==begSBuf) {  /* next chunk of source
					  text loaded at beginning of buffer. */
          if (lgtId > MaxLgtId) lgtId = MaxLgtId;  /* *doIt* if (adBegId <
	    begSBuf) err(TooBigId); err(TrnctdId) detected in case lgtId>SLI? */
          adBegId = memmove(begSBuf - lgtId, adBegId, (size_t)lgtId);}}
      BackUp;
      srcPtr = ptrS;
      lgtId = (int)(ptrS - adBegId);	/* identifier length */
      smshCh[1] = *--adBegId;	/* save character about to be smashed */
      *adBegId = (char) ObjectSpace;  /* qualify the name by the name space */
      smshCh[0] = *--adBegId;
      adSmshCh = adBegId;  /* note that characters are smashed */
      curTok.name = ConvTname(adBegId);  /* remember address of identifier
		nameString (used also for directive keywords, such as '#if'). */
      if (lgtId > MaxLgtId) {  /* truncate identifier if exceed storage
								    capacity. */
        *(TnameNC)adBegId = (TnameAtom)MaxLgtId;
        hCode = hCodFct(ConvTname(adBegId));}  /* recompute H-code, taking only
						 kept character into account. */
      else *(TnameNC)adBegId = (TnameAtom)lgtId;  /* store identifier length in
							   front of its name. */
      curTok.hCod = hCode;  /* must be defined even for keywords, in order to
					   differentiate them from d-pragmas. */
      /* Search identifier in symbol table (Object Space) */
      SearchSym(ConvTname(adBegId), MacroVisible)
      curTok.tok = IDENT;
      if ((pDescCurId = pCurId) == NULL) {  /* new identifier */
        if (lgtId>SignifLgtId && chkPortbl) err0(TrnctdId | Warn2);  /* warn
									once. */
        curTok.ptrSem = NULL;}
      else if (pCurId->nstLvl < 0) {    /* keyword or macro */
        if (pCurId->nstLvl + 1 == 0) {  /* keyword */
          curTok.tok = pCurId->noLex;
          goto exitL;}
        else if (macroExpand) {expandMac(); if (curTok.tok!= IDENT) goto exitL;}
        else curTok.ptrSem = NULL;}  /* if inside argument collection for
				       macros, do as if identifier undefined. */
      else curTok.ptrSem = pCurId->pIdSeman;  /* 'regular' (and already
							existing) identifier. */
      if (curTok.ptrSem!=NULL && curTok.ptrSem->undef && getTokLvl==1) errWN(
					     UndfndId|Warn1|Rdbl, curTok.name);}
exitL:
    if (! condDirSkip) {
      if (--getTokLvl==0 && listTok) storeTokSource();  /* only keep token in
		    "last tokens list" if 'real' (level 0) call to GetNxtTok. */
      return;}
    if (InsideInterval(curTok.tok, CSTNU, CSTST))
      if (curTok.tok - CSTNU == 0) analNumCst(NoConv);
      else analStrCst(NoConv);}  /* end "for(;;)" */
}

static void ampersF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '&') {curTok.tok = LOGAND;}
  else if (c == '=') {curTok.val = (uint)AndAsgn; curTok.tok = ASGNOP;}
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
    if (InsideInterval(charInfo[c], Aln, Aln + ('9' - '0'))) DoWhatForNb('.')
    else curTok.tok = DOT;}
  srcPtr = ptrS;
}

static void dQuoteF(char *ptrS)
{
  curTok.tok = CSTST;
  srcPtr = ptrS;
}

static void eMarkF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)NE; curTok.tok = EQUALOP;}
  else {BackUp; curTok.tok = EMARK;}
  srcPtr = ptrS;
}

static void eqF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)EQ; curTok.tok = EQUALOP;}
  else {BackUp; curTok.val = (uint)SimplAsgn; curTok.tok = ASGNOP;}
  srcPtr = ptrS;
}

static void greaTF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)GE; curTok.tok = ORDEROP;}
  else if (c == '>') {
    NxtCh
    if (c == '=') {curTok.val = (uint)RShAsgn; curTok.tok = ASGNOP;}
    else {BackUp; curTok.val = (uint)RSh; curTok.tok = SHIFTOP;}}
  else {BackUp; curTok.val = (uint)GT; curTok.tok = ORDEROP;}
  srcPtr = ptrS;
}

void ilgCharF(register char *ptrS)
{
  static Tchar errCh[] = " ";

  srcPtr = ptrS;
  errCh[0] = (Tchar)*(ptrS - 1);
  errWSS((errCh[0] == '$')? IlgCharD : IlgChar, errCh, charToHexS((char)
								     errCh[0]));
  curTok.tok = WHITESPACE;
}

static void lessTF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)LE; curTok.tok = ORDEROP;}
  else if (c == '<') {
    NxtCh
    if (c == '=') {curTok.val = (uint)LShAsgn; curTok.tok = ASGNOP;}
    else {BackUp; curTok.val = (uint)LSh; curTok.tok = SHIFTOP;}}
  else {BackUp; curTok.val = (uint)LT; curTok.tok = ORDEROP;}
  srcPtr = ptrS;
}

static void minusF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '-') {curTok.val = (uint)Dec; curTok.tok = INCOP;}
  else if (c == '=') {curTok.val = (uint)SubAsgn; curTok.tok = ASGNOP;}
  else if (c == '>') curTok.tok = ARROW;
  else {BackUp; curTok.val = (uint)Sub; curTok.tok = ADDOP;}
  srcPtr = ptrS;
}

static void nbSignF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '#') curTok.tok = CONCATOP;
  else {BackUp; curTok.tok = QUOTEOP;}
  if (! dirLineFl) {LexErrPhase4(LegalOnlyInMac, NULL); curTok.tok =
								    WHITESPACE;}
  srcPtr = ptrS;
}

static void nlF(register char *ptrS)
{
  register char c;

  if (dirLineFl) {  /* end of preprocessor line */
    dirLineFl = False;
    curTok.tok = ENDDIR;
    condDirSkip = False;}  /* will be re-set by 'manageDir()' */
  else {
    if (spaceCount != -1) oldSpaceCount = spaceCount; /* ignore comment lines */
    for (;;) {
      nlPosP1 = ptrS;
      lineNb++;
      spaceCount = 0;
      for (;;) {
        NxtCh
        if ((ubyte)c >= LimChar) LexErr(NotASCIIChar, charToHexS(c));
        else if (charInfo[c] != WhiteSpace) break;
        else if (c == '\t') spaceCount = (spaceCount + tabSpacing)/tabSpacing*
								     tabSpacing;
       	else spaceCount++;}
      if (c == '#') {	/* beginning of preprocessor line */
        dirLineFl = True;
        srcPtr = ptrS;
        manageDir();
        ptrS = srcPtr;}
      else {
        BackUp;
        curTok.tok = white;  /* WHITESPACE in general; WHITESPACE+1 in
				peepNxtNonBlChFromTxt(), to prevent reading
				next token after conditional skipping. */
        break;}}}
  srcPtr = ptrS;
}

static void percentF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)ModAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.val = (uint)Mod; curTok.tok = MULOP;}
  srcPtr = ptrS;
}

static void plusF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '+') {curTok.val = (uint)Inc; curTok.tok = INCOP;}
  else if (c == '=') {curTok.val = (uint)AddAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.val = (uint)Add; curTok.tok = ADDOP;}
  srcPtr = ptrS;
}

static void quoteF(register char *ptrS)
{
  uint nbChar = 0;
  char c;

  curTok.error = False;
  for (;;) {
    NxtCh
    if (c == '\'') break;
    ManageLitChar(UnFnshCharCst)
    curTok.val = (uint)c;
    nbChar++;}
  if (nbChar != 1) {LexErrPhase4(LgtCharCstNotOne|Warn3|PossErr, NULL); curTok.
								  error = True;}
  curTok.tok = (curTok.error)? CSTCH1 : CSTCH;
  srcPtr = ptrS;
}

static void slashF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '*') {	/* comment, or d-pragma */
    if (dpragNst != 0) LexErrPhase4(UnclosedDP, NULL);
    NxtCh
    if (c=='~' && dpragNst==0 && !condDirSkip) {srcPtr = ptrS; dpragMet()
						      ; return;}  /* d-pragma */
    for (;;) {
      if (c == '/') {
        NxtCh
        if (c == '*') {LexErr(UnclosedCmt|Warn2|PossErr, NULL); NxtCh}}
      while (c == '*') {
        NxtCh
        if (c == '/') goto exitL;}  /* end comment */
      if (fileClosed && c==EndCh) {LexErr(UnFnshCmt, NULL); BackUp; break;}
      if (c == '\n') {nlPosP1 = ptrS; lineNb++;}
      NxtCh}
exitL:
    curTok.tok = (dpragNst!=0 && !condDirSkip)? ENDDPRAG : WHITESPACE;
    if (spaceCount >= 0) spaceCount = -1;}  /* for always correct
			indentation check after comment at beginning of line. */
  else if (c == '=') {curTok.val = (uint)DivAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.val = (uint)Div; curTok.tok = MULOP;}
  srcPtr = ptrS;
}

static void starF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)MulAsgn; curTok.tok = ASGNOP;}
  else if (dpragNst!=0 && c=='/') curTok.tok = ENDDPRAG;
  else {BackUp; curTok.tok = STAR;}
  srcPtr = ptrS;
}

static void upArrowF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '=') {curTok.val = (uint)XorAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.tok = XOR;}
  srcPtr = ptrS;
}

static void vBarF(register char *ptrS)
{
  register char c;

  NxtCh
  if (c == '|') {curTok.tok = LOGOR;}
  else if (c == '=') {curTok.val = (uint)IorAsgn; curTok.tok = ASGNOP;}
  else {BackUp; curTok.tok = IOR;}
  srcPtr = ptrS;
}

/******************************************************************************/
/*				    UTILITIES				      */
/******************************************************************************/

void analNumCst(void(*pPutC)(char))
/* The analysis of a constant is only done now so that it is not converted
   each time it is found (for example, during macro body storage or macro para-
   meter passing), but only when used. */
{
#define StoreAndGetCh	{putCh(c); c=(char)NxtChar();}
  uint curDigVal, base, base1;
  TcalcU value;
  char c;
  bool eSeen = False;  /* exponent seen */

  pPutCh = pPutC;
  curTok.error = False;
  base1 = base = Base10;  /* 'base1' needed because 0 alone => base 10... */
  if ((c = frstChOfNb) == '.') {curTok.val |= FltSeen
					     ; StoreAndGetCh frstChOfNb = '\0';}
  value = (TcalcU)(charInfo[c] - Aln);
  StoreAndGetCh frstChOfNb = '\0';
  if (value==0 && !(curTok.val & FltSeen)) {
    if (c=='x' || c=='X') {base1 = Base16; StoreAndGetCh}  /* *doIt* 
						   only hex nb if digit after */
    else base1 = Base8;}
  for (;;) {
    if ((ubyte)c >= LimChar) break;
    if ((curDigVal = (uint)(charInfo[c] - Aln)) >= base1) {
      if (base1==Base16 && InsideInterval(c, 'a', 'f')) curDigVal -=
							      Lletter - Uletter;
      else if (curDigVal < Base10) syntErrNb(IlgOctDig, charToHexS(c));
      else {
        if (c == '.') {if ((curTok.val & FltSeen) != 0 || base!=Base10) break
			 ; base1 = Base10; curTok.val |= FltSeen; goto noConvL;}
        if (c=='e' || c=='E') {
          if (eSeen || base!=Base10) break;
	  eSeen = True;
          curTok.val |= FltSeen;
          StoreAndGetCh
          if (c=='+' || c=='-') StoreAndGetCh
          if ((ubyte)c>=LimChar || !InsideInterval(charInfo[c], Aln, Aln +
				('9' - '0'))) {syntErrNb(WrngNb1, NULL); break;}
          base1 = Base10;
          goto noConvL;}
        break;}}
    base = base1;
    if (pPutC == NULL) {
      if (! (curTok.val & FltSeen)) {  /* compute binary number */
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
  if (curTok.val & FltSeen) {  /* floating constant */
    uint w = (c=='f' || c=='F')? FSeen : (c=='l' || c=='L')? LDSeen : 0;

    curTok.numVal = (TcalcS)value;  /* until better processing ... */
    if (w != 0) {curTok.val |= w; StoreAndGetCh}}
  else {  /* integer constant */
    uint w;

    curTok.numVal = (TcalcS)value;
    for (;;) {
      switch(c) {
      case 'u': case 'U': w = USeen; break;
      case 'l': case 'L':
#ifndef LONGLONG
        if (sysAdjHdrFile)  /* 'LL' always accepted in system header files */
#endif
        {
          if (curTok.val & (LSeen | LLSeen)) {
            w = LLSeen;
            curTok.val &= ~LSeen;
            break;}}
        w = LSeen; break;
      default:
        if (base != Base10) curTok.val |= OctHex;
        if ((TcalcS)value < 0) {  /* a-priori, overflow */
          if (curTok.val & OctHex) curTok.val |= USeen;  /* if octal or hex,
							 convert to unsigned. */
          else if (pPutC==NULL && curTok.val & SignSeen && (!(curTok.val &
				  NegSeen) || -(TcalcS)value>=0)) semanErrNb();}
        goto exitNbL;}
      if (curTok.val & w) syntErrNb(WrngNb, NULL);
      curTok.val |= w;
      StoreAndGetCh}}
exitNbL:
  if (pNxtCh == &nxtChFromTxt) srcPtr--;
  restoNormState();
#undef StoreAndGetCh
}

void analStrCst(void(*pPutC)(char))
/* The analysis of a string constant is only done now, so that it can be stored
   at different places (for example, during macro body storage or for real
   use). */
{
  pPutCh = pPutC;
  if (! MacroTxt) {  /* not macro expansion */
    register char *ptrS = srcPtr;
    char c;

    for (;;) {
      NxtCh
      if (c == '\"') break;
      ManageLitChar(UnFnshStrCst)
      putCh(c);}
    srcPtr = ptrS;}
  else
    for (;;) {
      int c;

      if ((c = NxtChar()) < 0) break;
      putCh((char)c);}
  if (listTok && getTokLvl==0) saveTokChar('"');
  restoNormState();
  restoQuoState();
}

void backUp(void)
{
  srcPtr--;
}

static Tstring curTokTxt(void)
/* Yields the string representation of current token */
{
#include "dctxttok.h"
  switch (curTok.tok) {
  case IDENT: return nameToS(curTok.name);
  case FORCEMACEXP: case FORCEMACEXP1: case NOMACEXP: return nameToS(
							    pDescCurId->idName);
  case CSTCH: case CSTCH1: {
      void (*savPSCF)(char) = pStoreChFct;

      pStoreChFct = &putTokChar;
      tokTxtBuf[0] = '\'';
      pTokTxt = &tokTxtBuf[1];
      if (curTok.tok != CSTCH1) {  /* not erroneous character */
        if ((char)curTok.val == '\'') *pTokTxt++ = '\\';
        storeTokTxtRepr((char)curTok.val);}
      *pTokTxt++ = '\'';
      *pTokTxt = '\0';
      pStoreChFct = savPSCF;
      return &tokTxtBuf[0];}
  case CSTNU:    return "";
  case CSTST:    return "\"";
  case ADDOP:    return txtAddTok[(TkAdd)curTok.val];
  case ASGNOP:   return txtAsgnTok[(TkAsgn)curTok.val];
  case EQUALOP: case ORDEROP:  return txtCmpTok[(TkCmp)curTok.val];
  case INCOP:    return txtIncTok[(TkInc)curTok.val];
  case MULOP:    return txtMulTok[(TkMul)curTok.val];
  case SHIFTOP:  return txtShiTok[(TkShi)curTok.val];
  default: {
      static const Tstring tok1ChTxtRepr[/*~IndexType Ttok*/] = {"", ":", ",",
			"{", "(", "[", "->", ".", "?", "}", ")", "]", ";", "~"};
      static const Tstring tok2ChTxtRepr[] = {"", "&", "^", "|", "&", "...",
	"!", "", "", "", "&&", "||", "", "", "", "*", DPragPrefix, DPragSuffix};

      if (curTok.tok < ETok1Char) return tok1ChTxtRepr[curTok.tok];
      if (curTok.tok < EKeyWords) return (curTok.tok==DEFAULT && curTok.val==0)
		? ""
		: (Tstring)tabKeyWords[curTok.tok - BKeyWords] + LgtHdrId;
      if (curTok.tok < BSideEffTok) return tok2ChTxtRepr[curTok.tok -EKeyWords];
      return "";}}
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
				IlgOctDig, charToHexS(c)); curTok.error = True;}
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

    for (ptr = &speChar[0]; ptr <= AdLastElt(speChar); ) {
      if (c == *ptr++) {
        bool digitSeen;

        if ((charVal = (uint)correspChar[ptr - &speChar[1]]) != 0) goto okL;
        /* Hexadecimal constant */
        digitSeen = False;
        for (;;) {
          uint curDigVal;

          NxtCh
          if ((ubyte)c >= LimChar) break;
          if ((curDigVal = (uint)(charInfo[c] - Aln))>=Base16) {
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
    curTok.error = True;}
okL:
  if (charVal > UCHAR_MAXC) {LexErrPhase4(Overflow1 | Warn3, NULL); curTok.
								  error = True;}
  *pch = (char)charVal;
  return ptrS;
}

ThCode hCodFct(register Tname x)
/* hash-code (=> curTok.hCod) guaranteed non zero */
{
  uint lgt;
  ThCode hCode;

  lgt = (uint)*x++;	/*~ zif (DispLgtId != 0) "Pb in hCodFct()" */
  hCode = (ThCode)*x++;	/*~ zif (DispNSId != 1) "Pb in hCodFct()" */
  do {ComputeHCode(charInfo[(char)*x++]);} while (--lgt != 0);
  return hCode;
}

static void initKeyWords(void)
{
  const Tname *pExplTabKW;
  TdescrId *ptrDescKW;

  for (pExplTabKW = &tabKeyWords[0]; pExplTabKW <= AdLastElt(tabKeyWords);
								 pExplTabKW++) {
    if (*pExplTabKW != NULL) {
      ptrDescKW = enterSymTabHC1(*pExplTabKW, hCodFct(*pExplTabKW));
      ptrDescKW->noLex = BKeyWords + (int)(pExplTabKW - &tabKeyWords[0]);}}
  ctrDIE = 0;
}

void initRec(void)
{
  static bool frstInit = True;

  if (frstInit) {initKeyWords(); frstInit = False;}
  initStoreTokTxt();
}

bool isAlfa(char c)
{
  return ((ubyte)c >= LimChar)
         ? False
         : (c == '$')
           ? sysAdjHdrFile || dollAlwdInId
           : charInfo[c]>=Aln;
}

void manageConc(void)
/* BEWARE of recursive case (e.g. a##b##c => ## a ## b c) */
{
  static char concatBuf[LgtHdrId + MaxLgtId + 1 + MaxConcatLvl],
					     *pConcatBuf = &concatBuf[LgtHdrId];
  char *oldPConcBuf = pConcatBuf, *concSavSrcPtr;
  bool errTokTooLong = False, scndTurn = False;
  void (*rmbPGNT)(void) = NULL;

  for (;;) {
    Tstring ptrTokTxt;
    size_t lgtTokTxt;

    ptrTokTxt = curTokTxt();
    if (InsideInterval(curTok.tok, CSTNU, CSTST)) {
      ptrTokTxt = pTokTxt = &tokTxtBuf[0];
      if (curTok.tok - CSTNU == 0) analNumCst(&putTokChar);
      else {
        void (*savPSCF)(char) = pStoreChFct;

        *pTokTxt++ = '"';
        pStoreChFct = &putTokChar;
        analStrCst(&storeTokTxtRepr);
        putTokChar('"');
        pStoreChFct = savPSCF;}  /* back to initial value */
      *pTokTxt = '\0';}
    RestoSmshCh
    if (pConcatBuf + (lgtTokTxt = strlen(ptrTokTxt)) > AdLastElt(concatBuf) +
									    1) {
      errTokTooLong = True;
      lgtTokTxt = (size_t)(AdLastElt(concatBuf) + 1 - pConcatBuf);}
    memcpy(pConcatBuf, ptrTokTxt, lgtTokTxt);
    pConcatBuf += lgtTokTxt;
    if (scndTurn) break;
    GetNxtTok();
    scndTurn = True;}
  if (errTokTooLong) concatErr = CrtdTokTooLong;
  else if (pConcatBuf == oldPConcBuf) concatErr = EmptyTxt;  /* empty token */
  else {  /* no error up to now */
    concatErr = NoConcErr;  /* to intercept possible error */
    *pConcatBuf = ' ';  /* end of created token text */
    concSavSrcPtr = srcPtr;
    srcPtr = oldPConcBuf;
    mngConc = True;
    macroExpand = False;
    {
      int savSpaceCnt = spaceCount;

      getTokFromTxt();  /* get created token */
      spaceCount = savSpaceCnt;}
    macroExpand = True;
    if (InsideInterval(curTok.tok, CSTNU, CSTST)) {
      rmbPGNT = pCurGNT;
      pNxtCh = &nxtChFromTxt;
      if (curTok.tok - CSTNU == 0) analNumCst(NoConv);
      else analStrCst(NoConv);  /* end checking phase */
      pNxtCh = &nxtChFromMac;}
    if (concatErr==NoConcErr /* no error seen yet */ && srcPtr!=pConcatBuf
							 ) concatErr = EmptyTxt;
    mngConc = False;
    srcPtr = concSavSrcPtr;}
  if (concatErr != NoConcErr) {
    *pConcatBuf = '\0';  /* in order to get a string */
    errWSS(IlgCrtdTok, (Tstring)oldPConcBuf, errTxt[concatErr]);
    curTok.tok = SCOL;}  /* arbitrary; but ## must always return a token,
					because of a##b##c and error in b##c. */
  else if (rmbPGNT != NULL) {
    pConcTxt = (curTok.tok == CSTNU)? (frstChOfNb = *oldPConcBuf, oldPConcBuf +
							       1) : oldPConcBuf;
    pNxtCh = &nxtChFromConc;
    pCurGNT = rmbPGNT;}  /* no lexical error can occur now in number/string */
  concatErr = NoErrMsg;
  pConcatBuf = oldPConcBuf;
}

static TcharStream nxtChFromConc(void)
{
  return (TcharStream)*pConcTxt++;
}

char peepNxtNonBlChFromTxt(void)
/* See prerequisites in nxtCharOrMacTok().
   Go also through newlines, so to be used with care because of checkIndent() */
{
  for (;;) {
    char c;

    {
      register char *ptrS = srcPtr;

      for (;;) {
        NxtCh
        if ((ubyte)c>=LimChar || charInfo[c]!=WhiteSpace) break;}
      BackUp;
      srcPtr = ptrS;
    }
    if (c == '\n') {
      TdescrId *savPDCI, *savPNxtMac = pNxtMac;
      TvalTok savCurTok;
      int savOldSpCnt = oldSpaceCount, savSpCnt = spaceCount;
      uint savDPNst = dpragNst;

      if (curTok.tok == IDENT) curTok.name = ((savPDCI = pDescCurId) == NULL)?
		       storeName(curTok.name, ObjectSpace) : pDescCurId->idName;
      savCurTok = curTok;
      getTokLvl++;
      nlF(srcPtr + 1);  /* may be a directive "white line" */
      getTokLvl--;
      if (condDirSkip) {
        white = WHITESPACE+1;  /* anything but WHITESPACE */
        GetNxtTok();
        white = WHITESPACE;}
      pDescCurId = savPDCI;
      curTok = savCurTok;
      oldSpaceCount = savOldSpCnt; spaceCount = savSpCnt;
      dpragNst = savDPNst;
      pNxtMac = savPNxtMac;}
    else return c;}
}

static void putCh(char c)
{
  if ((size_t)pPutCh > (size_t)NoConv) (*pPutCh)(c);
  if (listTok && getTokLvl==0 && frstChOfNb=='\0') storeTokTxtRepr(c);
}

static void putTokChar(char c)
{
  if (pTokTxt < &tokTxtBuf[NbElt(tokTxtBuf) - 1]) *pTokTxt++ = (Tchar)c;
}

static void restoNormState(void)
{
  if (pNxtCh == &nxtChFromConc) pNxtCh = &nxtChFromMac;
}

static void semanErrNb(void)
{
  if (! curTok.error) {
    err0(Overflow1 | Warn2);
    curTok.error = True;}
}

void skipTok(const Ttok stopTokenList[])
{
  for (; curTok.tok != ENDPROG; GetNxtTok()) {
    register const Ttok *ptrT;
    register Ttok currTok = curTok.tok;

    if (currTok==CASTTO || currTok==INDEXTYPE) FreeExpType1(curTok.dpType);
    if (InsideInterval(currTok, CSTNU, CSTST))
      if (currTok - CSTNU == 0) analNumCst(NoConv);
      else analStrCst(NoConv);
    /* Search whether current token inside 'stop tokens' list */
    ptrT = &stopTokenList[0];
    do {
      if (currTok == *ptrT++) {
        if (ptrT == &stopTokenList[1]) {
          if (currTok==LBRA || currTok==RBRA || currTok==SCOL || currTok==
						       COMMA) ignoreErr = False;
          else if (currTok == ENDDPRAG) dpragNst--;
          GetNxtTok();}  /* 'swallow' first token */
        return;}
    } while (*ptrT != EndSTok);
    if (currTok == ENDDPRAG) dpragNst--;}
}

void storeTokSource(void)
{
  storeTokTxt(curTokTxt());
}

static Tchar prevChar = ' ';

static void initStoreTokTxt(void)
{
  prevChar = ' ';
}

void storeTokTxt(Tstring w)
{
  if (pStoreChFct == &saveTokChar) {
    if (curTok.tok==CSTNU && pStoreChFct==&saveTokChar) {
      /* Put 1st character of constant in '+zlt' buffer */
      static Tchar skelStr[] = "\0";

      skelStr[0] = (Tchar)frstChOfNb;
      w = &skelStr[0];}
    /* Insert separating space if necessary */
    if (isAlfa(prevChar) && isAlfa(*w)
        || (*w==prevChar && strchr("+-<>=&|", *w)!=NULL
        || *w=='=') && strchr("+-<>=!&|*/%^.", prevChar)!=NULL) (*pStoreChFct)(
				' ');}  /* so that "==-(3)" outputted as
						"==-(3)", and "+ =" as "+ =". */
  if (*w != '\0') { /* some token texts may be empty (for example, ? tokens). */
    do {
      (*pStoreChFct)(*w++);
    } while (*w != '\0');
    prevChar = *(w - 1);}
}

/*~Undef prevChar */

void storeTokTxtRepr(char c)
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

    if (((int)c & UCHAR_MAXC) >= Base8) (*pStoreChFct)('x');
    do {(*pStoreChFct)(*w++);} while (*w != '\0');}
}

static void syntErrNb(Terr n, Tstring y)
{
  if (!MacroTxt && !condDirSkip) errWS(n, y);  /* msg only at macro collection
									time. */
  curTok.error = True;
}

/* End DCREC.C */
