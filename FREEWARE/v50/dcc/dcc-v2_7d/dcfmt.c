/* DCFMT.C */
/* Y.L. Noyelle, Supelec, France 1995 */

#include <stdlib.h>
#ifdef EXIT_FAILURE
#  undef EXIT_FAILURE
#endif
#include <string.h>
#include "dcfmt.h"
#include "dcblk.h"
#include "dcext.h"
#include "dcinst.ph"
#include "dcmain.h"

#ifdef VMS
#pragma noinline (errSpec)
#endif

#define MaxLgtSpecTxt	11
#define NbSpecScanf	13
#define VarWdth		-2

typedef enum {EmptyD, StarB, MWB=StarB<<1, MPB=MWB<<1, HB=MPB<<1, LLB=HB<<1,
  ULB=LLB<<1, PtrB=ULB<<1, MiB=PtrB<<1, PlB=MiB<<1, ZrSpB=PlB<<1, NbB=ZrSpB<<1
} TdescB;

typedef struct {
  Tchar tSpecTxt[MaxLgtSpecTxt + 1 + 1 + 1], *pSpecTxt;
  TdescB descr;
  bool coRout1, coRout2;
  char curChar;
} SaveReentInfo;
#define _coRout1	buf.coRout1
#define _coRout2	buf.coRout2
#define _curChar	buf.curChar
#define _descr		buf.descr
#define _pSpecTxt	buf.pSpecTxt
#define _tSpecTxt	buf.tSpecTxt

typedef struct _save Save;
struct _save {
  SaveReentInfo sav;
  Save *prev;
};

DefRepresType

/* Local function signatures */
static void errSpec(Terr), record(TdescB), storeSpecTxt(void);
static int convNb(void) /*~Generic*/;

/* Global variables */
static TpcTypeElt argType;
static SaveReentInfo buf = {"%"};
static bool castUseful;
static Save *pTopSavStk = NULL;
static int reentCtr = -1;
static const char tabConvCh[] = "[discgfeouxpnGEX";
static bool tstCastFl = False;

/* External variables */
int ioFctNb = 0;  /* ioFctNb >0 if 'scanf' family, <0 if 'printf' family */

void initFlwgSpec(void)
{
  if (++reentCtr != 0) {
    Save *w;

    MyAlloc(w, sizeof(Save));
    w->sav = buf;
    w->prev = pTopSavStk;
    pTopSavStk = w;}
  initGetStrLit((TlitString) cExp.Pval);
  _coRout1 = _coRout2 = False;
  _curChar = '\1';  /* non-NUL */
}

void resetFlwgSpec(void)
{
  if (--reentCtr >= 0) {
    Save *w = pTopSavStk;

    buf = w->sav;
    pTopSavStk = w->prev;
    free(w);}
  resetGetStrLit();
}

bool folwngSpec(TpcTypeElt type, TpcTypeElt preLastCastType)
{
  int minWdth;
  TpcTypeElt initType;

  argType = type;
  if (_coRout1) {_coRout1 = False; goto coRout1L;} /* simulation of coroutine */
  if (_coRout2) {_coRout2 = False; goto coRout2L;} /* simulation of coroutine */
  if (_curChar == '\0') return False;
nxtSpecL:
  for (;;) {  /* search next specification */
    if ((_curChar = nxtStrLitChar()) == '\0') return False;
    if (_curChar == '%') {if ((_curChar = nxtStrLitChar()) != '%') break;}}
  _descr = EmptyD;  /* flags describing current specification */
  _pSpecTxt = &_tSpecTxt[1];  /* pointer to saved specification text ('1' be-
						      cause of starting '%'). */
  if (ScanfFl) {if (_curChar == '*') record(StarB);}
  else {
    TdescB oldDescr;

    do {
      oldDescr = _descr;
      if (_curChar == '-') record(MiB);
      else if (_curChar=='0' || _curChar==' ') record(ZrSpB);
      else if (_curChar == '+') record(PlB);
      else if (_curChar == '#') record(NbB);
    } while (_descr != oldDescr);}  /* loop as long as new specif. flag seen */
  if ((minWdth = convNb()) >= 0) _descr |= MWB;
  else if (minWdth == VarWdth) {_descr |= MWB; _coRout1 = True; goto varWdthL;}
coRout1L:
  if (!ScanfFl && _curChar=='.') {
    record(MPB);
    {
      int w;

      if ((w = convNb()) < 0) {
        if (w == VarWdth) {_coRout2 = True; goto varWdthL;}
        errSpec(IlgPrecFld|Warn3|PossErr);}}}
coRout2L:
  if (_curChar == 'h') record(HB);
  else if (_curChar == 'l') record(LLB);
  else if (_curChar == 'L') record(ULB);
  storeSpecTxt();
  {
    const char *pos;

    if (   (pos = strchr(tabConvCh, _curChar))==NULL
        || ScanfFl && pos>=&tabConvCh[NbSpecScanf]
        || !ScanfFl && pos==&tabConvCh[0]
        || _curChar=='\0') errSpec(IlgFmtSpe|Warn3|PossErr);
    else {
      static const struct _descSpec{TdescB dBits; TtypeSort typeSort;}
				      tabDescSpec[/*~IndexType ptrdiff_t */] = {
        /* Scanf Family specifications */
        {StarB|MWB                , Enum    },	/* [ ('char' = Enum...) */
        {StarB|MWB|HB|LLB|    PtrB, Int     },	/* d */
        {StarB|MWB|HB|LLB|    PtrB, Int     },	/* i */
        {StarB|MWB                , Enum    },	/* s */
        {StarB|MWB                , Enum    },	/* c */
        {StarB|MWB|   LLB|ULB|PtrB, Float   },	/* g */
        {StarB|MWB|   LLB|ULB|PtrB, Float   },	/* f */
        {StarB|MWB|   LLB|ULB|PtrB, Float   },	/* e */
        {StarB|MWB|HB|LLB|    PtrB, UInt    },	/* o */
        {StarB|MWB|HB|LLB|    PtrB, UInt    },	/* u */
        {StarB|MWB|HB|LLB|    PtrB, UInt    },	/* x */
        {StarB|MWB|           PtrB, Void    },	/* p */
        {StarB|    HB|LLB|    PtrB, Int|UInt},	/* n */
        /*~zif __index() != NbSpecScanf "Inadequate count of 'scanf' "
							     "specifications" */
        /* Printf Family specifications (must be in same order than identical
           Scanf Family specifications, cf 'pds' computation). */
        {MiB|ZrSpB|PlB|    MWB|MPB|HB|LLB         , Byte|Short|Int   },	/* d */
        {MiB|ZrSpB|PlB|    MWB|MPB|HB|LLB         , Byte|Short|Int   },	/* i */
        {MiB|              MWB|MPB|HB|LLB|    PtrB, Enum             },	/* s */
        {MiB|              MWB                    , Enum             },	/* c */
        {MiB|ZrSpB|PlB|NbB|MWB|MPB|       ULB     , Float|Double     },	/* g */
        {MiB|ZrSpB|PlB|NbB|MWB|MPB|       ULB     , Float|Double     },	/* f */
        {MiB|ZrSpB|PlB|NbB|MWB|MPB|       ULB     , Float|Double     },	/* e */
        {MiB|ZrSpB|    NbB|MWB|MPB|HB|LLB         , UByte|UShort|UInt},	/* o */
        {MiB|ZrSpB|        MWB|MPB|HB|LLB         , UByte|UShort|UInt},	/* u */
        {MiB|ZrSpB|    NbB|MWB|MPB|HB|LLB         , UByte|UShort|UInt},	/* x */
        {MiB|ZrSpB|        MWB|MPB|           PtrB, Void             },	/* p */
        {                                     PtrB, Int|UInt         },	/* n */
        {MiB|ZrSpB|PlB|NbB|MWB|MPB|       ULB     , Float|Double     },	/* G */
        {MiB|ZrSpB|PlB|NbB|MWB|MPB|       ULB     , Float|Double     },	/* E */
        {MiB|ZrSpB|    NbB|MWB|MPB|HB|LLB         , UByte|UShort|UInt},	/* X */
        /*~zif __index() != NbSpecScanf + NbElt(tabConvCh) - 2
		"Inadequate count of 'printf' specifications (ignore this "
		"message if there were a previous message complaining about "
		"'scanf')." */
      };

      const struct _descSpec *pds = &tabDescSpec[pos - &tabConvCh[0] +
					      ((ScanfFl)? 0 : NbSpecScanf - 1)];

      if ((_descr & pds->dBits) != _descr) errSpec(IlgOptForSpe|Warn3|PossErr);
      if (_descr & StarB) {
        errSpec(NoErrMsg);  /* empties error buffer */
        goto nxtSpecL /*~BackBranch*/;}
      if (type != NULL) {
        initType = type;
        for (;;) {  /* do loop for type, then for preLastCastType (if non-
			     NULL), just to check whether cast really useful. */
          TtypeSort ts = type->typeSort;

          if (pds->dBits & PtrB || ScanfFl && _curChar=='c' && minWdth<0) {
            if (! (ts==Ptr || ts==Array && _curChar=='s')) {errSpec(PtrExptd|
						   Warn3|PossErr); goto exitL;}}
          else if (ScanfFl) {  /* cases "%s", "%[", "%xc" */
            if (! IsPtrArrSort(ts)) {errSpec(ArrExptd|Warn3|PossErr); goto
									 exitL;}
            if (minWdth < 0) {
              if (! (ioFctNb==IndexSScanf && !tstCastFl && FoundDP(SIZEOK))
			) errSpec((ioFctNb == IndexSScanf)? MsngFldWdth1|Warn1|
					  PossErr : MsngFldWdth|Warn1|PossErr);}
            else if (ts==Array && (size_t)((_curChar == 's')? minWdth + 1 :
			minWdth)>type->Lim) errSpec(ArrTooSmall|Warn2|PossErr);}
          else goto noDeRefL;
          if ((type = NxtTypElt(type)) == NULL) goto exitL;
          if (ScanfFl) {
            argType = type; /* so, less surprising (in error msg) */
            if (cExp.PointingOnObj && ts==Ptr && type->typeSort==Enum && (
	       minWdth>1 || _curChar=='s')) errSpec(ArrTooSmall|Warn2|PossErr);}
          if (ScanfFl || _curChar=='n') 
            if (type->Qualif & ConstQal) errSpec(DontWriteIntoConst|Warn3|
								       PossErr);
            else {
              TsemanElt *rPtrId = cExp.ptrId;

              if (rPtrId!=NULL && (cExp.PointingOnObj || ts==Array && cExp.
		  ptrId->Kind!=Param)) rPtrId->Used = (loopLvl==0 &&
			InsideInterval(rPtrId->Attribb, Auto, Reg))? False :
								  cExp.OldUsed;}
          ts = type->typeSort;
          if (ts==Array && ScanfFl) {
            errSpec(ShdntBePtrOnArr|Warn1|Rdbl);
            if ((argType = type = NxtTypElt(type)) == NULL) goto exitL;
            ts = type->typeSort;}
noDeRefL:
          if (ScanfFl) {
            if (ts==Void && initType->Generiq) ts = pds->typeSort;  /* any type
			       is compatible with / *~VoidToOther* /(void *). */
            if (_descr & HB) ts <<= GapNxtVarie;  /* short => int */
            if (_descr & LLB) ts >>= (ts >= Float)? 1 : GapNxtVarie;  /* long =>
							int; double => float. */
            if (_descr & ULB) ts >>= GapNxtVarie;}  /* long double => float */
          else {
            if (_descr & LLB) ts = (ts & (Long | ULong))? ts>>GapNxtVarie :
						  Void;  /* (u)long => (u)int */
            if (_descr & ULB) ts = (ts != LongDbl)? Void : Double;}
          if (pds->typeSort!=Void && (!(pds->typeSort & ts) || ts==Enum &&
		  type->TagId!=NULL /* not 'char' */)) errSpec(IlgSpeForType|
								 Warn3|PossErr);
          if (preLastCastType == NULL || tstCastFl) break;
          type = preLastCastType;
          castUseful = False;
          tstCastFl = True;}
        if (_curChar == '[') {  /* read set */
          _curChar = nxtStrLitChar();
          if (_curChar == '^') _curChar = nxtStrLitChar();
          if (_curChar == ']') _curChar = nxtStrLitChar();
          while (_curChar != ']') _curChar = nxtStrLitChar();}}}}
  goto exitL;
varWdthL:
  if (type!=NULL && !(type->typeSort & (Int|Byte|UByte|Short|UShort)) && !(type
	->typeSort & Enum && cExp.Revlbl)) errSpec(IlgTypeForFld|Warn3|PossErr);
exitL:
  errSpec(NoErrMsg);  /* output possible error message(s) */
  if (tstCastFl) {
    tstCastFl = False;
    if (! castUseful) errWSTT(PrevCastUsl|Warn1|Rdbl, NULL, preLastCastType,
								     initType);}
  return True;
}

/******************************************************************************/
/*				   UTILITIES				      */
/******************************************************************************/
static int convNb(void) /*~Generic*/
{
  int result;

  if (!ScanfFl && _curChar=='*') {storeSpecTxt(); _curChar = nxtStrLitChar()
							      ; return VarWdth;}
  if (! InsideInterval(_curChar, '0', '9')) return -1;
  result = _curChar - '0';
  while (InsideInterval((storeSpecTxt(), _curChar = nxtStrLitChar()), '0', '9')
				    ) result = result*Base10 + (_curChar - '0');
  return result;
}

static void errSpec(Terr n)
{
#define MaxNbRmbdErr	3
  static struct _dErr {Terr errNo; TpcTypeElt type;} tabRmbdErr[MaxNbRmbdErr],
						*pTRmbdErr = &tabRmbdErr[0] - 1;

  if (n != NoErrMsg) {  /* new error to save */
    if (tstCastFl) {castUseful = True; return;}
    else {
      if (pTRmbdErr != &tabRmbdErr[MaxNbRmbdErr - 1]) pTRmbdErr++;
      pTRmbdErr->errNo = n;
      pTRmbdErr->type = argType;}}
  else {  /* buffer to be emptied */
    register const struct _dErr *w;

    *_pSpecTxt = '\0';  /* end specification string */
    for (w = &tabRmbdErr[0]; w <= pTRmbdErr; w++) errWSTT(w->errNo, _tSpecTxt,
								 w->type, NULL);
    pTRmbdErr = &tabRmbdErr[0] - 1;}  /* reset to initial state */
#undef MaxNbRmbdErr
}

static void record(TdescB x)
{
  _descr |= x;
  storeSpecTxt();
  _curChar = nxtStrLitChar();
}

static void storeSpecTxt(void)
{
  if (_pSpecTxt >= &_tSpecTxt[MaxLgtSpecTxt]) {  /* buffer overflow */
    _tSpecTxt[MaxLgtSpecTxt]  = '~';  /* to show overflow */
    _tSpecTxt[MaxLgtSpecTxt + 1]  = (Tchar)_curChar;
    _pSpecTxt = &_tSpecTxt[MaxLgtSpecTxt + 1 + 1];}
  else *_pSpecTxt++ = (Tchar)_curChar;
}

/* End DCFMT.C */
