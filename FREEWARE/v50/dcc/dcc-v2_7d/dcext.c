/* DCEXT.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#pragma noinline (nameToS, bufGrstIntToS, bufNameToS)
#pragma noinline (putIdent, putList, putString)
#endif

#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#ifdef EXIT_FAILURE
#  undef EXIT_FAILURE
#endif
#include <string.h>
#include "dcext.h"
#include "dcext.ph"
#include "dcdecl.h"  /* because of FreeExpType1 */
#include "dcinst.h"  /* because of FreeExpType1 */
#include "dcmain.h"
#include "dcprag.h"
#include "dcrec.h"
#include "dcrecdir.th"

/* External objects */
bool adjustFile = False;
bool allErrFl;		/* report all errors */
bool askConstFl;	/* ask for 'const' qualified pointer formal parameters*/
TresulExp cExp;		/* information on current expression (type, value etc)*/
bool chk1stCharVF;	/* check 1st character of identifiers */
bool chkArrBnd = True;	/* check array bound for named constants */
bool chkBool;		/* check boolean type (control structures, '&&', etc) */
bool chkEffic;		/* check inefficient constructs */
bool chkEmptParList;	/* check empty parameter list */
bool chkFctRetTyp;	/* check for named return type (functions) */
bool chkInclPos;	/* check position of included files */
bool chkNmdTypParFl;	/* check that function parameters are of named type */
bool chkNumCst;		/* check literal integer constant */
bool chkPortbl;		/* check portability */
bool chkTypName;	/* check 1st character of (created) type names */
bool chkUCTags;		/* check still incomplete tags at header file exit */
uint ctrBSE = 0;	/* number of blocks stack elements */
uint ctrCE = 0;		/* number of caseElt currently allocated */
uint ctrDE = 0;		/* number of declElt currently allocated */
uint ctrDIE;		/* number of descrId elements */
uint ctrMSB = 0;	/* number of macro blocks currently allocated */
uint ctrMSE = 0;	/* number of macro stack elements */
uint ctrNI = 0;		/* number of notInitVar blocks */
uint ctrNSB = 0;	/* number of nameStorage blocks */
uint ctrQL = 0;		/* number of qualifiedTypesElt currently allocated */
uint ctrSE = 0;		/* number of semanElt currently allocated */
uint ctrSSL = 0;	/* number of savStrLitCtxElt currently allocated */
uint ctrTE = 0;		/* number of typeElt currently allocated */
uint ctrTLE = 0;	/* number of tagListElt currently allocated */
uint ctrUEC = 0;	/* number of 'universal' block chunks */
uint ctrUT = 0;		/* number of undfTagElt currently allocated */
Tstring cUnitFName;	/* compilation unit name */
jmp_buf *curErrRet;	/* pointer on current 'longjmp' return information */
Tname curFctName = NULL;/* current function body name */
Tstring curFileName;	/* current file name */
Tstring curHdrFName = ""; /* current header file name */
TmacExpNb curMacExpNb = 0; /* number of current macro expansion */
TvalTok curTok;		/* current token */
bool dollAlwdInId;	/* flag: '$' allowed in identifiers */
const TtypeElt *errQalTyp1, *errQalTyp2;  /* non conformant (sub-)types for
				  'const'/'volatile' check (cf compatType()). */
bool goodPrgmg;		/* flag 'trying to enforce good programming' */
uint igndSysMacArgNbP1;	/* flag 'system macro not expanded' (kept as ident.) */
bool insideHdrFile = False; /* flag 'inside a header file' */
TqElt6 *headQElt6 = NULL; /* head of queue of available 'universal' blocks. */
bool ifDirExp = False;	/* True if currently evaluating a '#if' expression */
bool ignoreErr = False;	/* do not output error(s) until end of statement/
								 declaration. */
TindentChk indentIncr;	/* < 0 => not yet initialized (1st block not yet met);
						     0 => no indent checking. */
bool insideHdrInHdr = False; /* True when inside a header file included by
							 another header file. */
bool kwAsMacName;	/* True if keywords can be redefined */
TresulExp lExp;		/* information on current left sub-expression */
TlineNb lineNb = 0;	/* current source line number */
bool macroExpand = True;/* flag 'macro expansion enabled' */
bool mainFl;		/* flag 'current function is "main"' */
bool moreIndexTypeChk;
TtypeSort mskBool;	/* depends on option 'zbo' */
TnstLvl nestLvl = -1;	/* +/- 1 at each block opening/closing brace */
const Tstring nonFileName = " ";
TindentChk oldSpaceCount;/* space count before previous NL */
Tstring paramTxt;
void (*pCurGNT)(void) = &getTokFromTxt;
bool printStat;		/* flag to print block statistics */
TindentChk spaceCount;	/* number of spaces at beginning of current logical
					line; <0 if not at beginning of line. */
Tstring strpdCUnitFName = "";/* compilation unit name (without possible path) */
bool sysAdjHdrFile = False; /* flag 'processing a system or adjust file' */
bool sysHdrFile = False; /* flag 'processing a system file' */
Tverbo verbose;		/* flag 'more information' */
bool verifAllPtrForConst;
#ifdef VMS
bool vmsNoStd;		/* for #pragma (no)standard */
#endif
bool warnNUI;		/* default option: warn on unused identifier */
bool warnNUP;		/* default option: warn on unused function parameter */
bool zifExp = False;	/* True if currently evaluating a 'zif' expression */

#define SharInfo(xqualif, ygen, zalign, tvari) {{{			       \
    /* TinfoType used (implicitely) here */				       \
    xqualif	/*~ zif (! __member(_qualif))				       \
			"misplaced initialization for field '_qualif'" */,     \
    False	/*~ zif (! __member(_paralTyp))				       \
		        "misplaced initialization for field '_paralTyp'" */,   \
    False	/*~ zif (! __member(_sysTpdf))				       \
		        "misplaced initialization for field '_sysTpdf'" */,    \
    ygen	/*~ zif (! __member(_generic))				       \
			"misplaced initialization for field '_generic'" */,    \
    tvari	/*~ zif (! __member(_variousT))				       \
			"misplaced initialization for field '_variousT'" */,   \
    False	/*~ zif (! __member(_noOwner))				       \
			"misplaced initialization for field '_noOwner'" */,    \
    True	/*~ zif (! __member(_shared))				       \
			"misplaced initialization for field '_shared'" */,     \
    False	/*~ zif (! __member(_noFreeDpdt))			       \
		        "misplaced initialization for field '_noFreeDpdt'" */, \
    False	/*~ zif (! __member(_stopFreeing))			       \
			"misplaced initialization for field '_stopFreeing'" */,\
    False	/*~ zif (! __member(_rootTyp))				       \
			"misplaced initialization for field '_rootTyp'" */,    \
    False	/*~ zif (! __member(_qlfdTyp))				       \
			"misplaced initialization for field '_qlfdTyp'" */,    \
    False	/*~ zif (! __member(_variousT1))			       \
			"misplaced initialization for field '_variousT1'" */,  \
    (Tqualif)(0-0) /*~ zif (! __member(_synthQualif))			       \
			"misplaced initialization for field '_synthQualif'" */,\
    zalign	/*~ zif (! __member(_algn))				       \
			"misplaced initialization for field '_algn'" */,       \
    /*~ zif ! __member() "'SharInfo' not fully initialized" */		       \
  }}}
#define SharNQInfo(x)	SharInfo(NoQualif, False, x, False)
#define SharNQGInfo(x)	SharInfo(NoQualif, True, x, False)
#define SharCInfo(x)	SharInfo(ConstQal, True, x, False)

const TtypeElt natTyp[/*~IndexType int */] = {
  {Void        /*~ zif (! __member(typeSort))
	      "misplaced initialization for field 'typeSort'" */,
   SharNQInfo(Alig0)  /*~ zif (! __member(u1))
	      "misplaced initialization for field 'infoT'" */,
   0           /*~ zif (! __member(size))
	      "misplaced initialization for field 'size'" */}, 
  {Float, SharNQInfo(Alig3), FloatSiz},
  {Double, SharNQInfo(Alig3), DoubleSiz},
  {Byte, SharNQInfo(Alig0), 1},
  {Short, SharNQInfo(Alig1), ShortSiz},
  {Int, SharNQInfo(Alig2), IntSiz},
  {Long, SharNQInfo(Alig3), LongSiz}  /*~ zif __index() + 1 != ENatTyp - BNatTyp
				    "Array 'natTyp': incorrect organization" */,
#ifdef LONGLONG
  {LLong, SharNQInfo(Alig3), LongLongSiz},
#endif
  {UByte, SharNQInfo(Alig0), 1},
  {UShort, SharNQInfo(Alig1), ShortSiz},
  {UInt, SharNQInfo(Alig2), IntSiz},
  {ULong, SharNQInfo(Alig3), LongSiz},
#ifdef LONGLONG
  {ULLong, SharNQInfo(Alig3), LongLongSiz},
#endif
  {Enum, SharNQInfo(Alig0), 1},  /* 'char' */
  {LongDbl, SharNQInfo(Alig3), LongDblSiz},
};
/*~ zif NbElt(natTyp)!=EndCNatTypDpl "Array 'natTyp': incorrect organization" */

const TtypeElt
#ifdef LONGLONG
               defaultIndexTypeElt = {ULLong, SharNQInfo(Alig3), LongLongSiz},
#else
               defaultIndexTypeElt = {ULong, SharNQInfo(Alig3), LongSiz},
#endif
               charCstTypeElt = {Enum, SharNQGInfo(Alig0), 1},
               intCstTypeElt = {Int, SharNQGInfo(Alig2), IntSiz},
               longCstTypeElt = {Long, SharNQGInfo(Alig3), LongSiz},
               floatCstTypeElt = {Float, SharNQGInfo(Alig3), FloatSiz},
               doubleCstTypeElt = {Double, SharNQGInfo(Alig3), DoubleSiz},
               longDblCstTypeElt = {LongDbl, SharNQGInfo(Alig3), LongDblSiz},
               ptrdiffTypeElt = {Byte, SharNQGInfo(Alig3), LongSiz},
               sizeofTypeElt = {UByte, SharNQGInfo(Alig3), LongSiz},
	       wcharCstTypeElt = {UInt, SharNQGInfo(Alig2), IntSiz};

static const TtypeElt charCst1TypeElt = {Enum, SharCInfo(Alig0), 1};

static
#ifndef NotInitUnions
const
#endif
TtypeElt charPtrTypeElt = {Ptr, SharNQGInfo(Alig0), PtrSiz
#ifndef NotInitUnions
  , {0}, {NULL}, {NoConstTyp(&charCstTypeElt) /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'NextTE'" */}
#endif
};

#ifndef NotInitUnions
const
#endif
TtypeElt strCstTypeElt = {Array, SharNQGInfo(Alig0), 0
#ifndef NotInitUnions
  , {0}, {NoConstTyp(&defaultIndexTypeElt)}, {NoConstTyp(&charCst1TypeElt)
	 /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'NextTE'" */}
#endif
},
	       undeclFctTypeElt = {VFct, SharInfo(NoQualif, False, Alig0, True
							    /* PseudoVoid */), 0
#ifndef NotInitUnions
  , {0}, {NULL}, {NoConstTyp(&natTyp[IntDpl]) /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'NextTE'" */}
#endif
},
	       voidPtrTypeElt = {Ptr, SharNQInfo(Alig0), PtrSiz
#ifndef NotInitUnions
  , {0}, {NULL}, {NoConstTyp(&natTyp[VoidDpl]) /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'NextTE'" */}
#endif
},
	       wstrCstTypeElt = {Array, SharNQGInfo(Alig0), 0
#ifndef NotInitUnions
  , {0}, {NoConstTyp(&defaultIndexTypeElt) /*~ zif (! __member(_bndType))
			      "misplaced initialization for field 'BndType'" */}
#endif
};
TtypeElt boolTypeElt =    {Bool, SharNQInfo(Alig2), IntSiz},
         boolTypeElt1 =   {Bool, SharNQInfo(Alig2), IntSiz},
         boolCstTypeElt = {Bool, SharNQGInfo(Alig2), IntSiz},
         scndMainParTypeElt = {Array, SharNQInfo(Alig3), 0
#ifndef NotInitUnions
  , {0}, {NULL}, {NoConstTyp(&charPtrTypeElt) /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'NextTE'" */}
#endif
};

#undef SharInfo
#undef SharNQInfo
#undef SharNQGInfo
#undef SharCInfo

const Tstring errTxt[/*~ IndexType Terr */] = {
#include "dcmsg.txt"
		       };
/*~ zif NbEltGen(errTxt, (Terr)0) != EndErr - BegErr
			     "File 'dcmsg.txt': incorrect number of messages" */


/* Local functions profiles */
static void putList(const TdeclElt *, bool), typeToS(TpcTypeElt), putIdent(
						     Tname), putString(Tstring);

/* General purpose functions */
DefRepresType

typedef struct _lvl0InclBlk Tlvl0InclBlk;
struct _lvl0InclBlk {
  Tlvl0InclBlk *prev;
  Tstring inclFileName;
};

static Tlvl0InclBlk *headListLvl0InclFiles = NULL;

bool addLvl0InclFName(Tstring fileName)
{
  register Tlvl0InclBlk *w;

  /* Search in list of files included at level 0 */
  for (w = headListLvl0InclFiles; w != NULL; w = w->prev)
    if (StrEq(fileName, w->inclFileName)) return False;
  /* Add in list */
  MyAlloc(w, sizeof(Tlvl0InclBlk));
  w->inclFileName = fileName;
  w->prev = headListLvl0InclFiles;
  headListLvl0InclFiles = w;
  return True;
}

static void initAddLvl0InclFName(void)
{
  while (headListLvl0InclFiles != NULL) {
    Tlvl0InclBlk *w = headListLvl0InclFiles;

    headListLvl0InclFiles = headListLvl0InclFiles->prev;
    free(w);}
}

/*~Undef Tlvl0InclBlk, headListLvl0InclFiles */
/*~UndefTag _lvl0InclBlk */

void *allocChunk(void) /*~Generic*/
{
  static TqElt6 *adInCurChunk = NULL;
  static const TqElt6 *endCurChunk = NULL;

  if(adInCurChunk == endCurChunk) {	/* current chunk exhausted */
    MyAlloc(adInCurChunk, sizeof(TqElt6 [NbUnivEltInChunk]));
    endCurChunk = adInCurChunk + NbUnivEltInChunk;
    ctrUEC++;}
  return adInCurChunk++;
}

typedef struct _permSto TpermSto;
struct _permSto {
  TpermSto *prev;
  ubyte permZone[1];
};
/*~zif Offset(TpermSto, permZone) != sizeof(TpermSto)-1
                                      "Field 'permZone' not at end of struct" */

static TpermSto *headPermSto = NULL;
static ubyte *begPermSto, *endPermSto, *oldBegPermSto;

void *allocPermSto(size_t size) /*~Generic*/
/* Allocate a block of bytes, of length < sizeof(TqElt6 [NbUnivEltInChunk]).
   No alignment guaranteed. */
{
  while (begPermSto + size > endPermSto) {  /* current chunk exhausted (loop
				    so that exhausts memory if size too big). */
    TpermSto *w;

    MyAlloc(w, sizeof(TpermSto) - sizeof(TpermSto) +
					     sizeof(TqElt6 [NbUnivEltInChunk]));
    ctrUEC++;
    w->prev = headPermSto;
    headPermSto = w;
    begPermSto = &w->permZone[0];
    endPermSto = begPermSto + (sizeof(TqElt6 [NbUnivEltInChunk]) - Offset(
							  TpermSto, permZone));}
  oldBegPermSto = begPermSto;
  begPermSto += size;
  return oldBegPermSto;
}

void freeLastPermSto(void)
/* Frees last allocated permanent storage */
{
  begPermSto = oldBegPermSto;
}

static void initPermSto(void)
{
  while (headPermSto != NULL) {
    TpermSto *w = headPermSto;

    headPermSto = headPermSto->prev;
    free(w);
    ctrUEC--;}
  begPermSto = endPermSto = NULL;
}

/*~Undef TpermSto, headPermSto */
/*~UndefTag _permSto */
/*~Undef begPermSto, endPermSto, oldBegPermSto */

void bufGrstIntToS(TgreatestInt n, register TstringNC ptrBuf)
/* converts to 9999999999 maximum, plus sign, plus '\0' ending */
{
  unsigned TgreatestInt divsr = MaxPowOfTenInGtstInt, c, nb;
  bool digitSeen = False;

  nb = (n < 0)? (*ptrBuf++ = '-', (unsigned TgreatestInt)(-(n + 1)) + 1) :
						       (unsigned TgreatestInt)n;
  do {
    if ((c = nb/divsr)!=0 || digitSeen || divsr==1) {
      *ptrBuf++ = "0123456789"[c];
      digitSeen = True;
      nb -= c*divsr;}
  } while ((divsr /= Base10) != 0);
  *ptrBuf = '\0';
}

void bufNameToS(Tname x, register TstringNC ptrBuf)
{
  uint lgt;

  if (x != NULL) {
    lgt = (uint)*x++;	/*~ zif (DispLgtId != 0) "Pb into bufNameToS()" */
#ifdef Debug
    if (lgt==0 || lgt>MaxLgtId) sysErr(ExCod4);
#endif
    x++;		/*~ zif (DispNSId != 1)  "Pb into bufNameToS()" */
    do {*ptrBuf++ = (Tchar)*x++; lgt--;} while (lgt != 0);}
  *ptrBuf = '\0';  /* end of string */
}

Tstring charToHexS(char c)
{
  static Tchar hexRepre[(CHAR_BITC + BitsHexDigit - 1)/BitsHexDigit + 1];
  Tchar *ptr = &hexRepre[0];
  int i = ((CHAR_BITC + BitsHexDigit - 1)/BitsHexDigit  - 1) * BitsHexDigit;

  do {
    uint w = ((uint)c >> i) & ((1 << BitsHexDigit) - 1);

    if (w!=0 || ptr!=&hexRepre[0]) /* no '0' prefix */ *ptr++ =
							  "0123456789ABCDEF"[w];
  } while ((i -= BitsHexDigit) >= 0);
  if (ptr == &hexRepre[0]) *ptr++ = '0';  /* if no character generated */
  *ptr = '\0';
  return &hexRepre[0];
}

Tname decoratedName(const TresulExp *pExp)
/* Answers 'decorated' name of object associated to pointed (sub-)expression;
   decorates the name with (possibly parenthethized) '*', '[]', '.' and/or '->',
   depending on the (expression-chosen) component of the object. */
{
  if (pExp->ptrId ==NULL || pExp->ptrId->Kind==EnumCst) return NULL;
  {
    Thistory history = pExp->hist, invertedHist = H_EMPTY;
    bool lastIsPtr = False;
#define DEUX	2
    static TnameAtom buf[MaxLgtId + LgtHdrId + MaxHistDescr*DEUX + (MaxHistDescr
	      + 1)/DEUX];  /* Worst case: five characters ("(*)[]") generated
	     for H_PTR followed by H_ARRAY => maximum expansion factor of 2.5 */
#undef DEUX
    register TnameAtom *ptrBuf = &buf[1];  /*~ zif (DispLgtId != 0)
						    "Pb into decoratedName()" */
    register Tname name = pExp->ptrId->PdscId->idName;
    TpcTypeElt idType = pExp->ptrId->type;
    uint lgtName = (uint)*name++;
#define InsertAtFront(c)						       \
  {									       \
    register TnameAtom *w = ptrBuf;					       \
									       \
    while (w != &buf[LgtHdrId]) {*w = *(w - 1); w--;}			       \
    *w = (TnameAtom)c;							       \
    ptrBuf++;}

    *ptrBuf++ = *name++;  /*~ zif (DispNSId != 1) "Pb into decoratedName()" */
    if (pExp->PointingOnObj) *ptrBuf++ = (TnameAtom)'&';
    while (history != H_EMPTY) {invertedHist = invertedHist<<SizeHistElt |
					  history&MskHistElt; PopHist(history);}
    while (invertedHist != H_EMPTY) {
      Thistory hElt = invertedHist & MskHistElt;

      /* Follow declaration as much as possible */
      if (idType != NULL) {
        if (IsArr(idType)) hElt = H_ARRAY;
        else if (IsPtr(idType)) hElt = H_PTR;
        idType = NxtTypElt(idType);}
      invertedHist >>= SizeHistElt;
      if (hElt == H_PTR) {
        if ((invertedHist & MskHistElt)==H_DOT) {
          hElt = H_ARROW;
          invertedHist >>= SizeHistElt;}
        else {
          InsertAtFront('*');
          lastIsPtr = True;
          continue;}}
      if (lastIsPtr) InsertAtFront('(');
      while (lgtName != 0) {*ptrBuf++ = *name++; lgtName--;}
      if (lastIsPtr) {*ptrBuf++ = (TnameAtom)')'; lastIsPtr = False;}
      switch (hElt) {
        case H_ARRAY: *ptrBuf++ = (TnameAtom)'['; *ptrBuf++ = (TnameAtom)']'
									; break;
        case H_DOT: *ptrBuf++ = (TnameAtom)'.'; break;
        case H_ARROW: *ptrBuf++ = (TnameAtom)'-'; *ptrBuf++ = (TnameAtom)'>'
									; break;
        /*~NoDefault */}}
    while (lgtName != 0) {*ptrBuf++ = *name++; lgtName--;}
    buf[0] = (TnameAtom)(ptrBuf - &buf[LgtHdrId]);
    return &buf[0];}
#undef InsertAtFront
}

void err0(Terr n)
{
  err(n, NULL);
}

void err1(Terr n)
{
  errWS(n, paramTxt);
}

void errId(Terr n, const TsemanElt *ptrId, Tname x, Tstring y, Tstring z)
{
  errWFName(n, (ptrId != NULL)? ptrId->defLineNb : 0, (ptrId != NULL)? ptrId->
						   defFileName : NULL, x, y, z);
}

void errId0(Terr n, const TsemanElt *ptrId)
/* Only to be called if ptrId->Kind = {Obj, Param, xTag, Type} */
{
  errId1(n, ptrId, NULL);
}

void errId1(Terr n, const TsemanElt *ptrId, Tstring y)
/* Only to be called if ptrId->Kind = {Obj, Param, xTag, Type} */
{
  errId2(n, ptrId, y, NULL);
}

void errId2(Terr n, const TsemanElt *ptrId, Tstring y, Tstring z)
/* Only to be called if ptrId->Kind = {Obj, Param, xTag, Type} */
{
  errId(n, ptrId, (ptrId == NULL) ? NULL : ptrId->PdscId->idName, y, z);
}

void errId3(Terr n, const TresulExp *pExp, Tstring y, Tstring z)
/* Only to be called if ptrId->Kind = {Obj, Param} */
{
  errId(n, pExp->ptrId, decoratedName(pExp), y, z);
}

void errIlgDP(Tdprag t)
{
  errWS(IlgDPrag|Warn1|Rdbl, dpName(t));
}

void errMsngRPar(void)
{
  errWS(Msng, ")");
}

void errMsngSCol(void)
{
  errWS(Exptd, ";");
}

void errPanic(Terr n, const Tstring tabPrm[]) /*~NeverReturns*/
{
  err(n, tabPrm);
  longjmp(*curErrRet, 1);
}

void errUslDP(Tdprag t)
{
  errWS(UslDPrag|Warn1|Rdbl, dpName(t));
}

void errWFName(Terr n, TlineNb line, Tstring fileName, Tname x, Tstring y,
								      Tstring z)
{
  CreateParArr((8)) /*~DynInit*/;
  TnbBuf nbBuf;
  TnameBuf nameBuf;

  bufNameToS(x, nameBuf);
  parArr8[1] = &nameBuf[0]; parArr8[2] = y;
  parArr8[4] = fileName; parArr8[5] = z; parArr8[6] = "?";
  parArr8[3] = parArr8[7] = parArr8[8] = NULL;
  if (fileName != NULL) {
    bufGrstIntToS(line, nbBuf);
    parArr8[6] = &nbBuf[0];
    if (line > 1) parArr8[7] = errTxt[OrJustBef];
    if (fileName != cUnitFName) {
      parArr8[3] = errTxt[File];
      if (line==lineNb && StrEq(skipPath(fileName), skipPath(curFileName))
					  ) parArr8[8] = errTxt[DblIncldFile];}}
  err(n, parArr8);
}

void errWN(Terr n, Tname x)
{
  errWNSS(n, x, NULL, NULL);
}

void errWNSS(Terr n, Tname x, Tstring y, Tstring z)
{
  errWSSSS(n, nameToS(x), (y!=NULL && strncmp(y, SysHdrDir, LitLen(SysHdrDir))
				       == 0)? y+LitLen(SysHdrDir) : y, z, NULL);
}

void errWNT(Terr n, Tname x, TpcTypeElt y)
{
  errWNTT(n, x, y, NULL);
}

void errWNTT(Terr n, Tname x, TpcTypeElt y, TpcTypeElt z)
{
  errWSTT(n, nameToS(x), y, z);
}

void errWS(Terr n, Tstring x)
{
  errWSSSS(n, x, NULL, NULL, NULL);
}

void errWSS(Terr n, Tstring x, Tstring y)
{
  errWSSSS(n, x, y, NULL, NULL);
}

void errWSSSS(Terr n, Tstring x, Tstring y, Tstring z, Tstring t)
{
  CreateParArr((4)) /*~DynInit*/;

  parArr4[1] = x; parArr4[2] = y; parArr4[3] = z; parArr4[4] = t;
  err(n, parArr4);
}

void errWSTNSS(Terr n, Tstring x, TpcTypeElt y, Tname z, Tstring t, Tstring  u)
{
  CreateParArr((5)) /*~DynInit*/;

  parArr5[1] = x, parArr5[2] = typeToS1(y), parArr5[3] = nameToS(z),
						 parArr5[4] = t, parArr5[5] = u;
  err(n, parArr5);
}

void errWSTT(Terr n, Tstring x, TpcTypeElt y, TpcTypeElt z)
{
  errWSTTS(n, x, y, z, NULL);
}

void errWSTTS(Terr n, Tstring x, TpcTypeElt y, TpcTypeElt z, Tstring t)
{
  errWSSSS(n, x, typeToS1(y), typeToS2(z), t);
}

void errWT(Terr n, const TtypeElt *x)
{
  errWS(n, typeToS1(x));
}

bool foundDP(Tdprag dp)
{
  return (dp == (Tdprag)curTok.Val)? (GetNxtTok(), True) : False;
}

void initExt(void)
{
#ifdef NotInitUnions
  static bool frstInit = True;

  if (frstInit) {
    strCstTypeElt.BndType = NoConstTyp(&defaultIndexTypeElt);
    wstrCstTypeElt.BndType = NoConstTyp(&defaultIndexTypeElt);
    charPtrTypeElt.NextTE = NoConstTyp(&charCstTypeElt);
    strCstTypeElt.NextTE = NoConstTyp(&charCst1TypeElt);
    scndMainParTypeElt.NextTE = &charPtrTypeElt;
    undeclFctTypeElt.NextTE = NoConstTyp(&natTyp[IntDpl]);
    voidPtrTypeElt.NextTE = NoConstTyp(&natTyp[VoidDpl]);
    frstInit = False;}
#endif
  indentIncr = oldSpaceCount = -1;
  initAddLvl0InclFName();
  initPermSto();
}


bool isBodyHdrFile(register Tstring fileName)
{
  register Tstring y = strpdCUnitFName;

  while (RealChar(*fileName) == RealChar(*y)) {
    if (*fileName++ == '.') return True;
    y++;}
  return False;
}

Tstring grstIntToS(TgreatestInt n)
{
  static TnbBuf buf;

  bufGrstIntToS(n, buf);
  return &buf[0];
}

static Tstring grstIntToS1(TgreatestInt n)  /* local buffer for typeToSx */
{
  static TnbBuf buf;

  bufGrstIntToS(n, buf);
  return &buf[0];
}

bool match(register Tstring x, register Tstring y)
/* Answers True if string x is equal to string y, matching '*'s in x with
   arbitrary (sub)strings in y, and '%'s in x with arbitrary characters in y. */
{
  Tstring savx = NULL;

  for (;;) {
    while (RealChar(*x)==RealChar(*y) || *x=='%') {
      if (*y++ == '\0') return (*x != '%'); x++;}
    if (*x == '*') {
      while (*++x == '*') {}  /* '***..*' -> '*' */
      if (*x == '\0') return True;
      savx = x;}
    else {if (savx == NULL) return False; y+= savx - x + 1; x = savx;}
    if (*x != '%') {
      while (RealChar(*y) != RealChar(*x)) {if (*y++ == '\0') return False;}}}
}

Tstring nameToS(Tname x)
{
  static TnameBuf buf;

  bufNameToS(x, buf);
  return &buf[0];
}

static void putList(const TdeclElt *x, bool strunFl)
{
  bool firstElt = True;
  TpcTypeElt last = QuasiNULLval(TpcTypeElt);

  for (; x != NULL; x = x->cdr) {
    if (!strunFl || x->type!=last) {
      if (firstElt) firstElt = False; else putString((strunFl)? "; " : ", "); /*
						     list elements separator. */
      typeToS(last = x->type);
      if (x->idName != NULL) putString(" ");}
    else putString(", ");  /* for struct/union, do not print twice same type */
    if (x->idName != NULL) putIdent(x->idName);
    if (! strunFl) {
      static const Tstring dQalNames[/*~IndexType TparQal*/] =
						   {"", " ~UT", " ~GN", " ~RT"};
      /*~ zif (NbElt(dQalNames) != (int)(MaxParQal + 1))
				       "bad 'dQalNames' array initialization" */

      if (x->ResulPtrFl) putString(" ~RP");
      else putString(dQalNames[x->ParQal]);}
    else if (x->FldFl) {putString(":"); putString(grstIntToS1((TgreatestInt)x->
								    FldSize));}}
  if (strunFl) putString(";");
}

void putSeman(const TsemanElt *pId)
{
  static const Tstring tabKind[/*~ IndexType Tkind */] =
			    {"pA", "iD", "tY", "eT", "suT", "eC:", "lbL", "pA"};
  /*~ zif (NbEltGen(tabKind, (Tkind)0) != (int)MaxKind + 1)
					    "Array 'tabKind': incorrect size" */
  static const Tstring tabAttrib[/*~ IndexType Tattrib */] =
	      {"/none", "/static", "/extern", NULL, "/auto", "/reg", "/static"};
  /*~ zif (NbEltGen(tabAttrib, (Tattrib)0) != (int)MaxAttrib + 1)
					  "Array 'tabAttrib': incorrect size" */

  emitS(tabKind[pId->Kind]);
  switch (pId->Kind) {
    case EnumCst: emitS(grstIntToS1(pId->EnumVal)); break;
    case Label: emitS(")"); return;
    case EnumTag: case StrunTag: case Type: break;
    default: emitS(tabAttrib[pId->Attribb]);}
  emitS(") ");
  emitS(typeToS1(pId->type));
}

static uint embdStrunLvl = 0;

static void putQualif(TpcTypeElt x)
{
  Tqualif qual = (embdStrunLvl==0 || !IsStrun(x))? x->Qualif :
				      BaseStrunType(x)->SynthQualif | x->Qualif;

  if (qual & ConstQal) putString("const ");
  if (qual & VolatQal) putString("volatile ");
}

Tstring skipPath(Tstring fileName)
{
  Tstring posEndPath;

#ifdef VMS
  if ((posEndPath = strrchr(fileName, ']')) == NULL) posEndPath = 
							 strrchr(fileName, ':');
  if (posEndPath == NULL) {
#else
  {
#endif
    posEndPath = strrchr(fileName, '/');}
  return (posEndPath != NULL)? posEndPath + 1 : fileName;
}

static void typeToS(TpcTypeElt x)
{
  TtypeSort tsx;
  static uint embdType = 0;
  static const Tstring tabSort[] =
	{VoidStr, "bool", "byte", UnsignedStr " byte", ShortStr,
	 UnsignedStr " " ShortStr, IntStr, UnsignedStr " " IntStr,
	 LongStr, UnsignedStr " " LongStr,
#ifdef LONGLONG
	 ConvToString(LONGLONG), UnsignedStr " " ConvToString(LONGLONG),
#endif
	 FloatStr, DoubleStr, LongStr " " DoubleStr,
	 EnumStr " ", "Ptr", "Array[", "Fct(", "VFct(", StructStr " ",
	 UnionStr " "};

  while (x != NULL) {
    tsx = x->typeSort;  /* here to keep possible 'Int' bit in 'Enum' parallel
									type. */
    if (NxtIsTypId(x)) {
      do {
        if (x->TypeId->NamedType != x) putQualif(x);
        putString("<");
        if (! x->ParalTyp) putString("<");  /* to emphasize 'pure' system
								    typedefs. */
        putIdent(x->TypeId->PdscId->idName);
        putString(">");
        if (x->RootTyp) putString("RT");
        if (verbose<HalfVerbo || embdType!=0) return;
        x = Parent(x);
      } while (NxtIsTypId(x));
      putString(" ");}
    embdType++;
    putQualif(x);
    /* Convert type sort(s) */
    if (tsx == Void) putString(tabSort[0]);
    else {
      int i;
      TtypeSort explorBit;

      if (tsx==Int && x->SynthQualif!=NoQualif) putString(SignedStr " ");
      else if (! chkBool) tsx &= ~Bool;
      for (i = 1, explorBit = Bool; explorBit!=DelType; i++) {
        if (! (tsx & explorBit)) explorBit <<= 1;
        else {
          putString((i==PosDelT && x->TagId==NULL)? CharStr : tabSort[i]);
          if (tsx >= (explorBit <<= 1)) putString("/");  /* to separate from
								    next one. */
          else break;}}
      if (tsx >= Enum) {
        if (tsx >= DelType) putString(*((&tabSort[0] - 1) + i + (int)(tsx>>
								     PosDelT)));
        switch (tsx >> PosDelT) {
        case Ptr>>PosDelT:
          if (verbose>=HalfVerbo && x->BndType!=NULL && x->BndType!=
		 &defaultIndexTypeElt) {putString("~IT "); typeToS(x->BndType);}
          putString("/");
          break;
        case Array>>PosDelT:
          if (verbose>=HalfVerbo && x->BndType!=NULL && x->BndType!=
							 &defaultIndexTypeElt) {
            typeToS(x->BndType);
            if (x->Lim != 0) putString(" ");}
          if (x->Lim != 0) {
            if (x->Lim == ArrLimErr) putString("0");
            else {
              putString(grstIntToS1((TgreatestInt)x->Lim));
              if (verbose > HalfVerbo) {
                putString("*");
                putString(grstIntToS1((TgreatestInt)(x->size/x->Lim)));}}}
          putString("]:");
          break;
        case Fct>>PosDelT: case VFct>>PosDelT:
          if (x->ParamList != NULL) {if (verbose > HalfVerbo) putList(x->
				      ParamList, False); else putString("...");}
          putString(")");
          {
            TpcTypeElt w;

            if (x->PvNr && (w = NxtTypElt(x))!=NULL) putString(
					  (w->typeSort == Void)? "~NR" : "~PV");
            if (x->Generiq) putString("~GN");}
          putString("=>");
          break;
        case Enum>>PosDelT:   /* case Enum, Enum|Int */
          if (GtPtr(x->TagId, GenericEnum)) putIdent(x->TagId->PdscId->idName);
			      /* not 'char' or generic enum (cf correctExprN) */
          break;
        case Struct>>PosDelT:
        case Union>>PosDelT:
          x = BaseStrunType(x);
          putIdent(x->TagId->PdscId->idName);
          if (verbose>HalfVerbo || x->MemberList==NULL) {
            putString("{");
            if (x->MemberList != NULL) {
              if (embdStrunLvl == 0) {
                /* Do not print embedded strun (to get shorter description; be-
                  sides, prevents infinite loop if a strun refers to itself). */
                embdStrunLvl++;
                putString(grstIntToS1((TgreatestInt)x->size));
                putString("|");
                putList(x->MemberList, True);
                embdStrunLvl--;}
              else putString("...");}
            else putString("??");
            putString("}");}
          break;
        /*~ NoDefault */}}}
    embdType--;
    x = x->NextTE;}
}
/*~Undef embdStrunLvl */

static Tchar *ptrTxt;
static const Tchar *endBuf;

static void putIdent(Tname x)
/* used so that 'nameToS' buffer not destroyed */
{
  uint lgt;

  lgt = (uint)*x++;	/*~ zif (DispLgtId != 0) "Pb into putIdent()" */
  x++;			/*~ zif (DispNSId != 1)  "Pb into putIdent()" */
  while (ptrTxt!=endBuf && lgt!=0) {lgt--; *ptrTxt++ = (Tchar)*x++;}
}

static void putString(register Tstring x)
{
  while (ptrTxt!=endBuf && *x!='\0') {*ptrTxt++ = *x++;}
}

#define PutT(x, y)							       \
  Tstring x(TpcTypeElt pTyp)						       \
  {									       \
    ptrTxt = &y[0];							       \
    endBuf = AdLastEltP1(y);						       \
    typeToS(pTyp);							       \
    if (ptrTxt == endBuf) {ptrTxt -= NbElt(Dots); putString(Dots);}	       \
    *ptrTxt = '\0';							       \
    return &y[0];							       \
  }

static Tchar txtBuf1[SizTypTxtBuf + 1], txtBuf2[SizTypTxtBuf + 1]; /* +1
						      because of ending '\0'. */
PutT(typeToS1, txtBuf1)  /* creates function 'typeToS1', which builds a string
			    representation of a type, and gives back a pointer
			    on the buffer holding the string. */
PutT(typeToS2, txtBuf2)  /* creates function 'typeToS2 '(same as typeToS1,
					  except it uses a different buffer). */
/*~Undef ptrTxt, endBuf, txtBuf1, txtBuf2 */

/* End DCEXT.C */
