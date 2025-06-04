/* DCFMT.C */
/* Y.L. Noyelle, Supelec, France 1995 */

#include <string.h>
#include "dcfmt.h"
#include "dcblk.h"
#include "dcext.h"
#include "dcinst.ph"

#ifdef VMS
#pragma noinline (errSpec)
#endif

#define MaxLgtSpecTxt	11
#define NbSpecScanf	13
#define VarWdth		-(2)

DefRepresType

typedef enum {EmptyD, StarB, MWB=StarB<<1, MPB=MWB<<1, HB=MPB<<1, LLB=HB<<1,
  ULB=LLB<<1, PtrB=ULB<<1, MiB=PtrB<<1, PlB=MiB<<1, ZrSpB=PlB<<1, NbB=ZrSpB<<1}
									 TdescB;
/* Local function signatures */
static void errSpec(Terr), record(TdescB), storeSpecTxt(void);
static int convNb(void) /*~Generic*/;

/* Global variables */
static TpcTypeElt argType;
static char curChar;
static TdescB descr;
static bool coRout1, coRout2, endSpec;
static const char tabConvCh[] = "[discgfeouxpnGEX";
static Tchar tSpecTxt[MaxLgtSpecTxt + 1 + 1 + 1] = {'%'}, *pSpecTxt;

/* External variables */
int ioFctNb = 0;

void initFlwgSpe(void)
{
  initGetStrLit((TlitString) cExp.pVal);
  coRout1 = coRout2 = endSpec = False;
}

bool folwngSpe(TpcTypeElt type)
{
  int minWdth;
  bool *pCoRout;

  argType = type;
  if (coRout1) {coRout1 = False; goto coRout1L;}  /* simulation of coroutine */
  if (coRout2) {coRout2 = False; goto coRout2L;}  /* simulation of coroutine */
  if (endSpec) return False;
nxtSpecL:
  for (;;) {  /* search next specification */
    if ((curChar = getNxtStrLitChar()) == '\0') {endSpec = True; return False;}
    if (curChar == '%') {if ((curChar = getNxtStrLitChar()) != '%') break;}}
  descr = EmptyD;  /* flags describing current specification */
  pSpecTxt = &tSpecTxt[1];  /* pointer to saved specification text ('1' because
							    of starting '%'). */
  if (ScanfFl) {if (curChar == '*') record(StarB);}
  else {
    TdescB oldDescr;

    do {
      oldDescr = descr;
      if (curChar == '-') record(MiB);
      else if (curChar=='0' || curChar==' ') record(ZrSpB);
      else if (curChar == '+') record(PlB);
      else if (curChar == '#') record(NbB);
    } while (descr != oldDescr);}  /* loop as long as new specif. flag seen */
  if ((minWdth = convNb()) >= 0) descr |= MWB;
  else if (minWdth == VarWdth) {descr |= MWB; pCoRout = &coRout1; goto
								      varWdthL;}
coRout1L:
  if (!ScanfFl && curChar=='.') {
    record(MPB);
    {
      int w;

      if ((w = convNb()) < 0) {
        if (w == VarWdth) {pCoRout = &coRout2; goto varWdthL;}
        errSpec(IlgPrecFld|Warn3|PossErr);}}}
coRout2L:
  if (curChar == 'h') record(HB);
  else if (curChar == 'l') record(LLB);
  else if (curChar == 'L') record(ULB);
  storeSpecTxt();
  if (curChar == '\0') endSpec = True;
  {
    const char *pos;

    if ((pos = strchr(tabConvCh, curChar))==NULL || ScanfFl && pos>=&tabConvCh[
		      NbSpecScanf] || !ScanfFl && pos==&tabConvCh[0] || endSpec
					     ) errSpec(IlgFmtSpe|Warn3|PossErr);
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

      if ((descr & pds->dBits) != descr) errSpec(IlgOptForSpe|Warn3|PossErr);
      if (descr & StarB) {
        errSpec(NoErrMsg);  /* empties error buffer */
        goto nxtSpecL /*~BackBranch*/;}
      if (type != NULL) {
        const TpcTypeElt initType = type;
        TtypeSort ts = type->typeSort;

        if (pds->dBits & PtrB || ScanfFl && curChar=='c' && minWdth<0) {
          if (! (ts==Ptr || ts==Array && curChar=='s')) {errSpec(PtrExptd|
						   Warn3|PossErr); goto exitL;}}
        else if (ScanfFl) {
          if (! IsPtrArrSort(ts)) {errSpec(ArrExptd|Warn3|PossErr); goto exitL;}
          if (minWdth<0 && (ioFctNb!=IndexSScanf || !Found(SIZEOK))) errSpec((
	     ioFctNb == IndexSScanf)? MsngFldWdth1|Warn1|PossErr : MsngFldWdth|
								 Warn1|PossErr);
          else if (ts==Array && (size_t)((curChar == 's')? minWdth + 1 :
			minWdth)>type->lim) errSpec(ArrTooSmall|Warn2|PossErr);}
        else goto noDeRefL;
        for (;;) {
          if ((type = NxtTypElt(type)) == NULL) goto exitL;
          if (ScanfFl) argType = type; /* so, less surprising (in error msg) */
          if (ScanfFl || curChar=='n') 
            if (type->qualif & ConstQal) errSpec(DontWriteIntoConst|Warn3|
								       PossErr);
            else {
              TsemanElt *rPtrId = cExp.ptrId;

              if (rPtrId!=NULL && (cExp.PointingOnObj || ts==Array && cExp.
		 ptrId->kind!=Param)) rPtrId->used = (loopLvl==0 && rPtrId->
					  attribb>=Auto)? False : cExp.oldUsed;}
          ts = type->typeSort;
          if (ts==Array && ScanfFl) err0(ShdntBePtrOnArr|Warn2|Rdbl);
          else break;}
noDeRefL:
        if (ScanfFl) {
          if (ts==Void && initType->generic) ts = pds->typeSort;  /* any type
			       is compatible with / *~VoidToOther* /(void *). */
          if (descr & HB) ts <<= GapNxtVarie;  /* short => int */
          if (descr & LLB) ts >>= (ts >= Float)? 1 : GapNxtVarie;  /* long =>
							int; double => float. */
          if (descr & ULB) ts >>= GapNxtVarie;}  /* long double => float */
        else {
          if (descr & LLB) ts = ((ts & (Long | ULong)) == Void)? Void
				     : ts>>GapNxtVarie;  /* (u)long => (u)int */
          if (descr & ULB) ts = (ts != LongDbl)? Void : Double;}
        if (pds->typeSort!=Void && (!(pds->typeSort & ts) || ts==Enum && type->
	    tagId!=NULL /* not 'char' */)) errSpec(IlgSpeForType|Warn3|PossErr);
        if (curChar == '[') {  /* read all set */
          curChar = getNxtStrLitChar();
          if (curChar == '^') curChar = getNxtStrLitChar();
          if (curChar == ']') curChar = getNxtStrLitChar();
          while (curChar != ']') curChar = getNxtStrLitChar();}}}}
  goto exitL;
varWdthL:
  if (type!=NULL && !(type->typeSort & Int) && !(type->typeSort & Enum &&
					   cExp.rEvlbl)) errSpec(IlgTypeForFld);
  *pCoRout = True;
exitL:
  errSpec(NoErrMsg);  /* output possible error message(s) */
  return True;
}

static void errSpec(Terr n)
{
#define MaxNbRmbdErr	3
  static struct _dErr {Terr errNo; TpcTypeElt type;} tabRmbdErr[MaxNbRmbdErr],
							      *pTRmbdErr = NULL;

  if (pTRmbdErr == NULL) pTRmbdErr = &tabRmbdErr[0];
  if (n != NoErrMsg) {  /* new error to save */
    if (pTRmbdErr > &tabRmbdErr[MaxNbRmbdErr - 1]) {tabRmbdErr[MaxNbRmbdErr - 1]
		       .errNo = n; tabRmbdErr[MaxNbRmbdErr - 1].type = argType;}
    else {pTRmbdErr->errNo = n; (*pTRmbdErr++).type = argType;}}
  else {  /* buffer to be emptied */
    register struct _dErr *w;

    *pSpecTxt = '\0';  /* end specification string */
    for (w = &tabRmbdErr[0]; w != pTRmbdErr; w++) errWSTT(w->errNo, tSpecTxt,
								 w->type, NULL);
    pTRmbdErr = NULL;}  /* reset to initial state */
#undef MaxNbRmbdErr
}

static void storeSpecTxt(void)
{
  if (pSpecTxt >= &tSpecTxt[MaxLgtSpecTxt]) {  /* buffer overflow */
    tSpecTxt[MaxLgtSpecTxt]  = '~';  /* to show overflow */
    tSpecTxt[MaxLgtSpecTxt + 1]  = (Tchar)curChar;
    pSpecTxt = &tSpecTxt[MaxLgtSpecTxt + 1 + 1];}
  else *pSpecTxt++ = (Tchar)curChar;
}

static void record(TdescB x)
{
  descr |= x;
  storeSpecTxt();
  curChar = getNxtStrLitChar();
}

static int convNb(void) /*~Generic*/
{
  int result;

  if (!ScanfFl && curChar=='*') {storeSpecTxt(); curChar = getNxtStrLitChar()
							      ; return VarWdth;}
  if (! InsideInterval(curChar, '0', '9')) return -1;
  result = curChar - '0';
  while (InsideInterval((storeSpecTxt(), curChar = getNxtStrLitChar()), '0',
				 '9')) result = result*Base10 + (curChar - '0');
  return result;
}
/* End DCFMT.C */
