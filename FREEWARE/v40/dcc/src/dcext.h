/* DCEXT.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCEXT_H
#define DCEXT_H

#include <setjmp.h>
#include "dc.th"

/* Functions profiles */
extern bool addLvl0InclFName(Tstring);
extern void *allocChunk(void) /*~Generic*/;
extern bool isBodyHdrFile(Tstring);
extern void bufLongToS(TgreatestInt /*~Generic*/, TstringNC), bufNameToS(Tname,
  TstringNC);
extern void errIlgDP(Ttok), errUslDP(Ttok);
extern void err0(Terr), err1(Terr), errWN(Terr, Tname), errWNSS(Terr, Tname,
  Tstring, Tstring), errWNT(Terr, Tname, TpcTypeElt), errWNTT(Terr, Tname,
  TpcTypeElt, TpcTypeElt), errWS(Terr, Tstring), errWSS(Terr, Tstring, Tstring),
  errWSSSS(Terr, Tstring, Tstring, Tstring, Tstring), errWSTT(Terr, Tstring,
  TpcTypeElt, TpcTypeElt), errWSTTS(Terr, Tstring, TpcTypeElt, TpcTypeElt,
  Tstring), errWT(Terr, TpcTypeElt), errId(Terr, const TsemanElt *, Tname,
  Tstring, Tstring), errId1(Terr, const TsemanElt *, Tstring), errId2(Terr,
  const TsemanElt *, Tstring, Tstring), errId3(Terr, const TresulExp *,
  Tstring, Tstring), errPanic(Terr, const Tstring[]) /*~NeverReturns*/,
  errWFName(Terr, TlineNb, Tstring, Tname, Tstring, Tstring);
extern Tstring charToHexS(char), longToS(TgreatestInt /*~Generic*/), nameToS(
  Tname), typeToS1(TpcTypeElt), typeToS2(TpcTypeElt);
extern void putSeman(const TsemanElt *);
extern Tstring skipPath(Tstring);
extern Tname decoratedName(const TresulExp *);
extern bool strJokerEq(Tstring, Tstring);

/* Objects */
extern bool adjustFile, allErrFl, askConstFl, chkBool, chkEmptParList,
	    chkFctRetTyp, chkInclPos, chkNmdTypParFl, chkNumCst, chkPortbl,
	    chkTypName, dollAlwdInId, headerFile, ifDirExp, ignoreErr,
	    macroExpand, moreIndexTypeChk, printStat, sysAdjHdrFile, sysHdrFile,
	    verifAllPtrForConst, warnNUI, warnNUP, zifExp;
extern TresulExp cExp;		/* information on current (sub-)expression */
extern Tstring cUnitFName;	/* main file name */
extern jmp_buf *curErrRet;	/* for 'panic' mode error recovery */
extern Tname curFctName;	/* current function name */
extern Tstring curFileName;	/* current file name */
extern Tstring curHdrFName;	/* current header file name */
extern TmacExpnd curMacExpNb;	/* current macro expansion number */
extern TvalTok curTok;
extern const Tstring errTxt[/*~ IndexType Terr */]; /* message texts */
extern TpcTypeElt errQalTyp1, errQalTyp2; /* erroneous (sub-)type in
						 case of 'const' incoherence. */
extern bool insideHdrInHdr;
extern TmngtPriv *headListPrivTo;  /* current 'PrivateTo' list of body file
								       names. */
extern TqElt6 *headQElt6;	/* head of queue of available 'universal'
								      blocks. */
extern TresulExp lExp;		/* left operand, in binary expression */
extern TlineNb lineNb;		/* current line nb */
extern TnstLvl nestLvl;
extern const Tstring nonFileName;
extern Tstring paramTxt;
extern void (*pCurGNT)(void);
extern int spaceCount, indentIncr, oldSpaceCount; /* for indentation checking */
extern Tstring strpdCUnitFName;	/* compilation unit name ((without directory/
							     version number). */
extern Tverbo verbose;
/* Description of predefined types */
#ifdef NotInitUnions
extern TtypeElt charCst1TypeElt;
#endif
extern
#ifndef NotInitUnions
       const
#endif
             TtypeElt charPtrTypeElt, strCstTypeElt/*, NULLTypeElt*/;
extern const TtypeElt defaultIndexTypeElt, /*fltDblTypeElt, */charCstTypeElt,
  intCstTypeElt, longCstTypeElt, floatCstTypeElt, doubleCstTypeElt,
  longDblCstTypeElt, natTyp[/*~IndexType int */];
extern TtypeElt boolTypeElt, boolCstTypeElt, ptrdiffTypeElt, scndMainParTypeElt,
  sizeofTypeElt;
extern uint ctrBSE;		/* number of blocks stack elements */
extern uint ctrCE;		/* number of caseElt currently allocated */
extern uint ctrDE;		/* number of declElt currently allocated */
extern uint ctrDIE;		/* number of descrId elements */
extern uint ctrMSB;		/* number of macro storage blocks */
extern uint ctrNI;		/* number of notInitVar blocks */
extern uint ctrNSB;		/* number of nameStorage blocks */
extern uint ctrSE;		/* number of semanElt currently allocated */
extern uint ctrTE;		/* number of typeElt currently allocated */
extern uint ctrTLE;		/* number of tagListElt currently allocated */
extern uint ctrUEC;		/* number of 'universal' block chunks */

#endif /* ifndef DCEXT_H */
/* End DCEXT.H */
