/* DCEXT.C */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifdef VMS
#pragma noinline (nameToS, bufLongToS, bufNameToS)
#pragma noinline (putIdent, putList, putString)
#endif

#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "dcext.h"
#include "dcext.ph"
#include "dcdecl.h"  /* because of FreeExpType1 */
#include "dcinst.h"  /* because of FreeExpType1 */
#include "dcmain.h"
#include "dcprag.h"
#include "dcrec.h"

bool adjustFile = False;
bool allErrFl = False;	/* report all errors */
bool askConstFl = True;	/* ask for 'const' qualified pointer formal parameters*/
TresulExp cExp;	       /* information on current expression (type, value etc) */
bool chkBool = True;    /* check boolean type (control structures, '&&', etc) */
bool chkEmptParList = False; /* check empty parameter list */
bool chkFctRetTyp = True; /* check for named return type (functions) */
bool chkInclPos = True; /* check position of included files */
bool chkNmdTypParFl = False; /* check that function parameters are of named
									type. */
bool chkNumCst = True;  /* check literal integer constant */
bool chkPortbl = True;	/* check portability */
bool chkTypName = True;	/* check type name for uppercase 1st character */
uint ctrBSE = 0;	/* number of blocks stack elements */
uint ctrCE = 0;		/* number of caseElt currently allocated */
uint ctrDE = 0;		/* number of declElt currently allocated */
uint ctrDIE;		/* number of descrId elements */
uint ctrMSB = 0;	/* number of macro blocks currently allocated */
uint ctrNI = 0;		/* number of notInitVar blocks */
uint ctrNSB = 0;	/* number of nameStorage blocks */
uint ctrSE = 0;		/* number of semanElt currently allocated */
uint ctrTE = 0;		/* number of typeElt currently allocated */
uint ctrTLE = 0;	/* number of tagListElt currently allocated */
uint ctrUEC = 0;	/* number of 'universal' block chunks */
Tstring cUnitFName;	/* compilation unit name */
jmp_buf *curErrRet;	/* pointer on current 'longjmp' return information */
Tname curFctName;	/* current function body name */
Tstring curFileName;	/* current file name */
Tstring curHdrFName = NonHdrFName; /* if inside header file, current header
								   file name. */
TmacExpnd curMacExpNb = 0; /* number of current macro expansion */
TvalTok curTok;
bool dollAlwdInId;	/* flag: '$' allowed in identifiers */
const TtypeElt *errQalTyp1, *errQalTyp2;  /* non conformant (sub-)types for
				  'const'/'volatile' check (cf compatType()). */
bool headerFile = False; /* flag 'processing a header file' */
TmngtPriv *headListPrivTo;
TqElt6 *headQElt6 = NULL; /* head of queue of available 'universal' blocks. */
bool ifDirExp = False;	/* True if currently evaluating a '#if' expression */
bool ignoreErr = False;	/* do not output error(s) until end of statement/
								 declaration. */
int indentIncr;		/* < 0 => not yet initialized (1st block not yet met);
						     0 => no indent checking. */
bool insideHdrInHdr = False;  /* True when inside a header file included by
							 another header file. */
TresulExp lExp;		/* information on current left sub-expression */
bool macroExpand = True; /* flag 'macro expansion enabled' */
bool moreIndexTypeChk = False;
TlineNb lineNb;		/* current source line number */
TnstLvl nestLvl = -1;	/* +/- 1 at each block opening/closing brace */
const Tstring nonFileName = " ";
int oldSpaceCount;	/* space count before a NL */
Tstring paramTxt;
void (*pCurGNT)(void) = &getTokFromTxt;
bool printStat = False;	/* flag to print block statistics */
int spaceCount;		/* number of spaces at beginning of current logical
					line; <0 if not at beginning of line. */
Tstring strpdCUnitFName; /* compilation unit name (without possible path) */
bool sysAdjHdrFile = False; /* flag 'processing a system or adjust file' */
bool sysHdrFile = False; /* flag 'processing a system file' */
Tverbo verbose;		/* flag 'more information' */
bool verifAllPtrForConst = False;
bool warnNUI = True;	/* default option: warn on unused identifier */
bool warnNUP = True;	/* default option: warn on unused function parameter */
bool zifExp = False;	/* True if currently evaluating a 'zif' expression */

#define SharInfo(xqualif, ygen, zalign, tvari1) {			       \
    /* TinfoType used (implicitely) here */				       \
    xqualif	/*~ zif (! __member(_qualif))				       \
			"misplaced initialization for field '_qualif'" */,     \
    False	/*~ zif (! __member(_paralTyp))				       \
		        "misplaced initialization for field '_paralTyp'" */,   \
    False	/*~ zif (! __member(_sysTpdf))				       \
		        "misplaced initialization for field '_sysTpdf'" */,    \
    ygen	/*~ zif (! __member(_generic))				       \
			"misplaced initialization for field '_generic'" */,    \
    False	/*~ zif (! __member(_noOwner))				       \
			"misplaced initialization for field '_noOwner'" */,    \
    True	/*~ zif (! __member(_shared))				       \
			"misplaced initialization for field '_shared'" */,     \
    False	/*~ zif (! __member(_noFreeDepdt))			       \
		        "misplaced initialization for field '_noFreeDepdt'" */,\
    False	/*~ zif (! __member(_stopFreeing))			       \
			"misplaced initialization for field '_stopFreeing'" */,\
    False	/*~ zif (! __member(_rootTyp))				       \
			"misplaced initialization for field '_rootTyp'" */,    \
    False	/*~ zif (! __member(_variousT))				       \
			"misplaced initialization for field '_variousT'" */,   \
    tvari1	/*~ zif (! __member(_variousT1))			       \
			"misplaced initialization for field '_variousT1'" */,  \
    (Tqualif)(0-0) /*~ zif (! __member(_synthQualif))			       \
			"misplaced initialization for field '_synthQualif'" */,\
    zalign	/*~ zif (! __member(_algn))				       \
			"misplaced initialization for field '_algn'" */,       \
    /*~ zif ! __member() "'SharInfo' not fully initialized" */		       \
  }
#define SharNQInfo(x)	SharInfo(NoQualif, False, x, False)
#define SharNQGInfo(x)	SharInfo(NoQualif, True, x, False)
#define SharCInfo(x)	SharInfo(ConstQal, True, x, False)

const TtypeElt natTyp[/*~IndexType int */] = {
  {Void        /*~ zif (! __member(typeSort))
	      "misplaced initialization for field 'typeSort'" */,
   SharNQInfo(Alig0)  /*~ zif (! __member(infoT))
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
/*~ zif NbElt(natTyp)!=_ENDCNATTYP "Array 'natTyp': incorrect organization" */

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
               longDblCstTypeElt = {LongDbl, SharNQGInfo(Alig3), LongDblSiz};

#ifndef NotInitUnions
static const
#endif
TtypeElt charCst1TypeElt = {Enum, SharCInfo(Alig0), 1};

#ifndef NotInitUnions
const
#endif
TtypeElt charPtrTypeElt = {Ptr, SharNQGInfo(Alig0), PtrSiz
#ifndef NotInitUnions
  , {0}, {NULL}, {&charCstTypeElt /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'nextTE'" */}
#endif
},
         strCstTypeElt = {Array, SharNQGInfo(Alig0), 0
#ifndef NotInitUnions
  , {0}, {&defaultIndexTypeElt}, {&charCst1TypeElt /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'nextTE'" */}
#endif
};
TtypeElt boolTypeElt = {Bool, SharNQInfo(Alig2), IntSiz},
         boolCstTypeElt = {Bool, SharNQGInfo(Alig2), IntSiz},
         scndMainParTypeElt = {Array, SharNQInfo(Alig3), 0
#ifndef NotInitUnions
  , {0}, {NULL}, {&charPtrTypeElt /*~ zif (! __member(_nextTE))
			       "misplaced initialization for field 'nextTE'" */}
#endif
},
         ptrdiffTypeElt = {Byte, SharNQGInfo(Alig3), LongSiz},
         sizeofTypeElt = {UByte, SharNQGInfo(Alig3), LongSiz};
#undef SharInfo
#undef SharNQInfo
#undef SharNQGInfo
/*#undef SharNQGUInfo*/
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

typedef struct _lvl0InclBlk Lvl0InclBlk;
struct _lvl0InclBlk {
  Lvl0InclBlk *prec;
  Tstring inclFileName;
};
static Lvl0InclBlk *headListLvl0InclFiles = NULL;

static void initAddLvl0InclFName(void)
{
  headListLvl0InclFiles = NULL;
}

bool addLvl0InclFName(Tstring fileName)
{
  register Lvl0InclBlk *w;

  fileName = skipPath(fileName);
  /* Search in list of files included at level 0 */
  for (w = headListLvl0InclFiles; w != NULL; w = w->prec)
    if (StrEq(fileName, w->inclFileName)) return False;
  /* Add in list */
  MyAlloc(w, sizeof(Lvl0InclBlk));
  w->inclFileName = fileName;
  w->prec = headListLvl0InclFiles;
  headListLvl0InclFiles = w;
  return True;
}

/*~Undef Lvl0InclBlk, headListLvl0InclFiles */
/*~UndefTag _lvl0InclBlk */

void *allocChunk(void) /*~Generic*/
{
  static TqElt6 *adInCurChunk = NULL;
  static const TqElt6 *endCurChunk = NULL;

  if(adInCurChunk == endCurChunk) {	/* current chunk exhausted */
    MyAlloc(adInCurChunk, sizeof(TqElt6 [NbUnivEltChunk]));
    endCurChunk = adInCurChunk + NbUnivEltChunk;
    ctrUEC++;}
  return adInCurChunk++;
}

void bufLongToS(TgreatestInt n /*~Generic*/, register TstringNC ptrBuf)
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
   depending on the (expression-choosen) component of the object. */
{
  if (pExp->ptrId == NULL) return NULL;
  {
    Thistory history = pExp->hist;
    bool lastIsPtr = False;
#define DEUX	2
    static TnameAtom buf[MaxLgtId + LgtHdrId + MaxHistDescr*DEUX + (MaxHistDescr
	      + 1)/DEUX];  /* Worst case: five characters ("(*)[]") generated
	     for H_PTR followed by H_ARRAY => maximum expansion factor of 2.5 */
#undef DEUX
    register TnameAtom *ptrBuf = &buf[1];  /*~ zif (DispLgtId != 0)
						    "Pb into decoratedName()" */
    int posMsk = -SizeHistElt;
    register Tname name = pExp->ptrId->nameb;
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
    {
      Thistory w = history;

      while (w != H_EMPTY) {PopHist(w); posMsk += SizeHistElt;}}
    while (posMsk >= 0) {
      Thistory hElt = (history >> posMsk) & MskHistElt;

      /* Follow declaration as much as possible */
      if (idType != NULL) {
        if (IsArr(idType)) hElt = H_ARRAY;
        else if (IsPtr(idType)) hElt = H_PTR;
        idType = NxtTypElt(idType);}
      posMsk -= SizeHistElt;
      if (hElt == H_PTR) {
        if (posMsk>=0 && ((history >> posMsk) & MskHistElt)==H_DOT) {
          hElt = H_ARROW;
          posMsk -= SizeHistElt;}
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

void errId1(Terr n, const TsemanElt *ptrId, Tstring y)
/* Only to be called if ptrId->kind = {Type, Tag, Obj} (ptrId->nameb valid) */
{
  errId2(n, ptrId, y, NULL);
}

void errId2(Terr n, const TsemanElt *ptrId, Tstring y, Tstring z)
/* Only to be called if ptrId->kind = {Type, Tag, Obj} (ptrId->nameb valid) */
{
  errId(n, ptrId, (ptrId == NULL)? NULL : ptrId->nameb, y, z);
}

void errId3(Terr n, const TresulExp *pExp, Tstring y, Tstring z)
/* Only to be called if ptrId->kind = {Type, Tag, Obj} (ptrId->nameb valid) */
{
  errId(n, pExp->ptrId, decoratedName(pExp), y, z);
}

void errIlgDP(Ttok t)
{
  errWS(IlgDPrag|Warn1|Rdbl, dpName(t));
}

void errPanic(Terr n, const Tstring tabPrm[]) /*~NeverReturns*/
{
  err(n, tabPrm);
  longjmp(*curErrRet, 1);
}

void errUslDP(Ttok t)
{
  errWS(UslDPrag|Warn1|Rdbl, dpName(t));
}

void errWFName(Terr n, TlineNb line, Tstring fileName, Tname x, Tstring y,
								      Tstring z)
{
  CreateParArr(9) /*~DynInit*/;
  TnbBuf nbBuf;
  TnameBuf nameBuf;

  bufNameToS(x, nameBuf);
  parArr9[1] = &nameBuf[0]; parArr9[2] = y; parArr9[3] = z; parArr9[4] = "?";
  parArr9[5] = parArr9[6] = parArr9[7] = parArr9[8] = parArr9[9] = NULL;
  if (fileName != NULL) {
    bufLongToS((TgreatestInt)line, nbBuf);
    parArr9[4] = &nbBuf[0];
    if (line > 1) parArr9[8] = errTxt[OrJustBef];
    if (fileName != cUnitFName) {
      parArr9[6] = fileName;
      parArr9[5] = errTxt[File];
      parArr9[7] = errTxt[EndFileName];
      if (line==lineNb && StrEq(skipPath(fileName), skipPath(curFileName))
					  ) parArr9[9] = errTxt[DblIncldFile];}}
  err(n, parArr9);
}

void errWN(Terr n, Tname x)
{
  errWS(n, nameToS(x));
}

void errWNSS(Terr n, Tname x, Tstring y, Tstring z)
{
  errWSSSS(n, nameToS(x), y, z, NULL);
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
  CreateParArr(4) /*~DynInit*/;

  parArr4[1] = x; parArr4[2] = y; parArr4[3] = z; parArr4[4] = t;
  err(n, parArr4);
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

void initExt(void)
{
#ifdef NotInitUnions
  static bool frstInit = True;

  if (frstInit) {
    strCstTypeElt.indexType = &defaultIndexTypeElt;
    charPtrTypeElt.nextTE = &charCstTypeElt;
    strCstTypeElt.nextTE = &charCst1TypeElt;
    scndMainParTypeElt.nextTE = &charPtrTypeElt;
    frstInit = False;}
#endif
  cUnitFName = NULL;
  curFctName = NULL;
  headListPrivTo = NULL;
  indentIncr = oldSpaceCount = -1;
  initAddLvl0InclFName();
}


bool isBodyHdrFile(register Tstring fileName)
{
  register Tstring y = strpdCUnitFName;

  fileName = skipPath(fileName);
  while (RealChar(*fileName) == RealChar(*y)) {
    if (*fileName++ == '.') return True;
    y++;}
  return False;
}

Tstring longToS(TgreatestInt n /*~Generic*/)
{
  static TnbBuf buf;

  bufLongToS(n, buf);
  return &buf[0];
}

static Tstring longToS1(TgreatestInt n /*~Generic*/)  /* local buffer for
								    typeToSx. */
{
  static TnbBuf buf;

  bufLongToS(n, buf);
  return &buf[0];
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
						   {"", " ~RT", " ~GN", " ~UT"};
      /*~ zif (NbElt(dQalNames) != (int)(MaxParQal + 1))
				       "bad 'dQalNames' array initialization" */

      if (x->ResulPtrFl) putString(" ~RP");
      else putString(dQalNames[x->parQal]);}
    else if (x->fldFl) {putString(":"); putString(longToS1((TgreatestInt)x->
								    fldSize));}}
  if (strunFl) putString(";");
}

void putSeman(const TsemanElt *pId)
{
  static const Tstring tabKind[/*~ IndexType Tkind */] =
			    {"pA", "iD", "tY", "eT", "suT", "eC:", "lbL", "pA"};
  /*~ zif (NbEltGen(tabKind, (Tkind)0) != (int)MaxKind + 1)
					    "Array 'tabKind': incorrect size" */
  static const Tstring tabAttrib[/*~ IndexType Tattrib */] =
	      {"/none", "/static", "/static", "/extern", NULL, "/auto", "/reg"};
  /*~ zif (NbEltGen(tabAttrib, (Tattrib)0) != (int)MaxAttrib + 1)
					  "Array 'tabAttrib': incorrect size" */

  emitS(tabKind[pId->kind]);
  switch (pId->kind) {
    case EnumCst: emitS(longToS1(pId->enumVal)); break;
    case Label: emitS(")"); return;
    case EnumTag: case StrunTag: case Type: break;
    default: emitS(tabAttrib[pId->attribb]);}
  emitS(") ");
  emitS(typeToS1(pId->type));
}

static uint embdStrunLvl = 0;

static void putQualif(TpcTypeElt x)
{
  Tqualif qual = (embdStrunLvl==0 || !IsStrun(x))? x->qualif :
				      BaseStrunType(x)->SynthQualif | x->qualif;

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

bool strJokerEq(register Tstring x, register Tstring y)
/* Answers True if string x is equal to string y, matching '*'s in x with
   arbitrary (sub)strings in y. */
{
  Tstring savx = NULL;

  for (;;) {
    while (RealChar(*x) == RealChar(*y)) {if (*x++ == '\0') return True; y++;}
    if (*x == '*') {
      while (*++x == '*') {}  /* '***..*' -> '*' */
      if (*x == '\0') return True;
      savx = x;}
    else {if (savx == NULL) return False; y+= savx - x + 1; x = savx;}
    while (RealChar(*y) != RealChar(*x)) {if (*y++ == '\0') return False;}}
}

static void typeToS(TpcTypeElt x)
{
  TtypeSort tsx;
  static uint embdType = 0;
  static const Tstring tabSort[] =
	{"void", "bool", "byte", "unsigned byte", "short", "unsigned short",
	 IntStr, "unsigned "IntStr, LongStr, "unsigned " LongStr,
#ifdef LONGLONG
	 ConvToString(LONGLONG), "unsigned " ConvToString(LONGLONG),
#endif
	 "float", "double", "long double",
	 "enum ", "Ptr", "Array[", "Fct(", "VFct(", "struct ", "union "};

  while (x != NULL) {
    tsx = x->typeSort;  /* here to keep eventual 'Int' bit in 'Enum' parallel
									type. */
    if (NxtIsTypeId(x)) {
      bool spaceNeeded = False;

      do {
        if (x->typeId->namedType != x) putQualif(x);
        putString("<");
        if (! x->paralTyp) putString("<");  /* to emphasize 'pure' system
								    typedefs. */
        putIdent(x->typeId->nameb);
        putString(">");
        if (x->rootTyp) putString("RT");
        if (verbose<HalfVerbo || embdType!=0) return;
        spaceNeeded = True;
        x = x->typeId->type;
      } while (NxtIsTypeId(x));
      if (spaceNeeded) putString(" ");}
    embdType++;
    putQualif(x);
    /* Convert type sort(s) */
    if (tsx == Void) putString(tabSort[0]);
    else {
      int i;
      TtypeSort explorBit;

      if (tsx==Int && x->SynthQualif!=NoQualif) putString("signed ");
      for (i = 1, explorBit = Bool; explorBit!=DelType; i++) {
        if (! (tsx & explorBit)) explorBit <<= 1;
        else {
          putString((i==PosDelT && x->tagId==NULL)? "char" : tabSort[i]);
          if (tsx >= (explorBit <<= 1)) putString("/");  /* to separate from
								    next one. */
          else break;}}
      if (tsx >= DelType) putString(*((&tabSort[0] - 1) + i + (int)(tsx>>
								     PosDelT)));
      if (tsx >= Enum)
        switch (tsx >> PosDelT) {
        case Ptr>>PosDelT:
          if (verbose>=HalfVerbo && x->indexType!=NULL && x->indexType!=
	       &defaultIndexTypeElt) {putString("~IT "); typeToS(x->indexType);}
          putString("/");
          break;
        case Array>>PosDelT:
          if (verbose>=HalfVerbo && x->indexType!=NULL && x->indexType!=
							 &defaultIndexTypeElt) {
            typeToS(x->indexType);
            if (x->lim != 0) putString(" ");}
          if (x->lim != 0) {
            if (x->lim == ArrLimErr) putString("0");
            else {
              putString(longToS1((TgreatestInt)x->lim));
              if (verbose > HalfVerbo) {
                putString("*");
                putString(longToS1((TgreatestInt)(x->size/x->lim)));}}}
          putString("]:");
          break;
        case Fct>>PosDelT: case VFct>>PosDelT:
          if (x->paramList != NULL) {if (verbose > HalfVerbo) putList(x->
				      paramList, False); else putString("...");}
          putString(")");
          {
            TpcTypeElt w;

            if (x->pvNr && (w = NxtTypElt(x))!=NULL) putString(
					  (w->typeSort == Void)? "~NR" : "~PV");
            if (x->generic) putString("~GN");}
          putString("=>");
          break;
        case Enum>>PosDelT:   /* case Enum, Enum|Int */
          if (x->tagId > GenericEnum) putIdent(x->tagId->nameb);  /* not
				    'char' or generic enum (cf correctExprN). */
          break;
        case Struc>>PosDelT: case Union>>PosDelT:
          x = BaseStrunType(x);
          putIdent(x->tagId->nameb);
          if (verbose > HalfVerbo) {
            putString("{");
            if (x->memberList != NULL) {
              if (embdStrunLvl == 0) {
                /* Do not print embedded strun (to get shorter description; be-
                  sides, prevents infinite loop if a strun refers to itself). */
                embdStrunLvl++;
                putString(longToS1((TgreatestInt)x->size));
                putString("|");
                putList(x->memberList, True);
                embdStrunLvl--;}
              else putString("...");}
            else putString("??");
            putString("}");}
          break;
        /*~ NoDefault */}}
    embdType--;
    x = x->nextTE;}
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

#define putT(x, y)							       \
  Tstring x(TpcTypeElt pTyp)						       \
  {									       \
    ptrTxt = &y[0];							       \
    endBuf = &y[0] + NbElt(y);						       \
    typeToS(pTyp);							       \
    if (ptrTxt == endBuf) {ptrTxt -= NbElt(Dots); putString(Dots);}	       \
    *ptrTxt = '\0';							       \
    return &y[0];							       \
  }

static Tchar txtBuf1[SizTypTxtBuf + 1], txtBuf2[SizTypTxtBuf + 1]; /* +1
						      because of ending '\0'. */
putT(typeToS1, txtBuf1)  /* creates function 'typeToS1', which builds a string
			    representation of a type, and gives back a pointer
			    on the buffer holding the string. */
putT(typeToS2, txtBuf2)  /* creates function 'typeToS2 '(same as typeToS1,
					  except it uses a different buffer). */
/*~Undef ptrTxt, endBuf, txtBuf1, txtBuf2 */

/* End DCEXT.C */
