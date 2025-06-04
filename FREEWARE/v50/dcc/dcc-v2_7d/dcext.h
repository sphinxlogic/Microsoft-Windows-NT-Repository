/* DCEXT.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCEXT_H
#define DCEXT_H

#include "dc.th"

/* Functions profiles */
extern void bufGrstIntToS(TgreatestInt, TstringNC), bufNameToS(Tname,
  TstringNC);
extern void err0(Terr), err1(Terr),
  errWN(Terr, Tname),
  errWNSS(Terr, Tname, Tstring, Tstring), errWNT(Terr, Tname, TpcTypeElt),
  errWNTT(Terr, Tname, TpcTypeElt, TpcTypeElt), errWS(Terr, Tstring),
  errWSS(Terr, Tstring, Tstring),
  errWSSSS(Terr, Tstring, Tstring, Tstring, Tstring),
  errWSTNSS(Terr, Tstring, TpcTypeElt, Tname, Tstring, Tstring),
  errWSTT(Terr, Tstring, TpcTypeElt, TpcTypeElt),
  errWSTTS(Terr, Tstring, TpcTypeElt, TpcTypeElt, Tstring),
  errWT(Terr, TpcTypeElt),
  errId(Terr, const TsemanElt *, Tname, Tstring, Tstring),
  errId0(Terr, const TsemanElt *),
  errId1(Terr, const TsemanElt *, Tstring),
  errId2(Terr, const TsemanElt *, Tstring, Tstring),
  errId3(Terr, const TresulExp *, Tstring, Tstring),
  errIlgDP(Tdprag),
  errMsngRPar(void),
  errMsngSCol(void),
  errPanic(Terr, const Tstring[]) /*~NeverReturns*/,
  errUslDP(Tdprag),
  errWFName(Terr, TlineNb, Tstring, Tname, Tstring, Tstring);
extern bool addLvl0InclFName(Tstring);
extern void *allocChunk(void) /*~Generic*/;
extern void *allocPermSto(size_t) /*~Generic*/;
extern Tstring charToHexS(char);
extern Tname decoratedName(const TresulExp *);
extern bool foundDP(Tdprag);
extern void freeLastPermSto(void);
extern bool isBodyHdrFile(Tstring);
extern Tstring grstIntToS(TgreatestInt);
extern bool match(Tstring, Tstring);
extern Tstring nameToS(Tname);
extern void putSeman(const TsemanElt *);
extern Tstring skipPath(Tstring);
extern Tstring typeToS1(TpcTypeElt), typeToS2(TpcTypeElt);

/* Objects */
extern bool adjustFile, allErrFl, askConstFl, chkArrBnd, chkBool, chkEffic,
	    chkEmptParList, chkFctRetTyp, chkInclPos, chkNmdTypParFl, chkNumCst,
	    chkPortbl, chkTypName, chkUCTags, chk1stCharVF, dollAlwdInId,
	    goodPrgmg, insideHdrFile, ifDirExp, ignoreErr, kwAsMacName,
	    macroExpand, moreIndexTypeChk, printStat, sysAdjHdrFile,
	    sysHdrFile, verifAllPtrForConst, warnNUI, warnNUP, zifExp;
#ifdef VMS
extern bool vmsNoStd;
#endif
extern TresulExp cExp;		/* information on current (sub-)expression */
extern Tstring cUnitFName;	/* main file name */
extern jmp_buf *curErrRet;	/* for 'panic' mode error recovery */
extern Tname curFctName;	/* current function name */
extern Tstring curFileName;	/* current file name */
extern Tstring curHdrFName;	/* current header file name */
extern TmacExpNb curMacExpNb;	/* current macro expansion number */
extern TvalTok curTok;		/* current token */
extern const Tstring errTxt[/*~ IndexType Terr */]; /* message texts */
extern TpcTypeElt errQalTyp1, errQalTyp2; /* erroneous (sub-)type in
						 case of 'const' incoherence. */
extern bool insideHdrInHdr;
extern TqElt6 *headQElt6;	/* head of queue of available 'universal'
								      blocks. */
extern uint igndSysMacArgNbP1;
extern TindentChk indentIncr;	/* for indentation checking */
extern TresulExp lExp;		/* left operand, in binary expression */
extern TlineNb lineNb;		/* current line nb */
extern bool mainFl;
extern TtypeSort mskBool;  /* depends on option 'zbo' */
extern TnstLvl nestLvl;
extern const Tstring nonFileName;
extern Tstring paramTxt;
extern void (*pCurGNT)(void);
extern TindentChk spaceCount, oldSpaceCount; /* for indentation checking */
extern Tstring strpdCUnitFName;	/* compilation unit name ((without directory/
							     version number). */
extern Tverbo verbose;
/* Description of predefined types */
extern
#ifndef NotInitUnions
       const
#endif
             TtypeElt strCstTypeElt, undeclFctTypeElt, voidPtrTypeElt,
  wstrCstTypeElt;
extern const TtypeElt defaultIndexTypeElt, /*fltDblTypeElt, */charCstTypeElt,
  intCstTypeElt, longCstTypeElt, floatCstTypeElt, doubleCstTypeElt,
  longDblCstTypeElt, natTyp[/*~IndexType int */], ptrdiffTypeElt,
  sizeofTypeElt, wcharCstTypeElt;
extern TtypeElt boolTypeElt, boolTypeElt1, boolCstTypeElt, scndMainParTypeElt;
extern uint ctrBSE;  /* number of blocks stack elements */
extern uint ctrCE;   /* number of caseElt currently allocated */
extern uint ctrDE;   /* number of declElt currently allocated */
extern uint ctrDIE;  /* number of descrId elements */
extern uint ctrMSB;  /* number of macro storage blocks */
extern uint ctrMSE;  /* number of macro stack elements */
extern uint ctrNI;   /* number of notInitVar blocks */
extern uint ctrNSB;  /* number of nameStorage blocks */
extern uint ctrQL;   /* number of qualifiedTypesElt currently allocated */
extern uint ctrSE;   /* number of semanElt currently allocated */
extern uint ctrSSL;  /* number of savStrLitCtxElt currently allocated */
extern uint ctrTE;   /* number of typeElt currently allocated */
extern uint ctrTLE;  /* number of tagListElt currently allocated */
extern uint ctrUEC;  /* number of 'universal' block chunks */
extern uint ctrUT;   /* number of undfTagElt currently allocated */

#endif /* ifndef DCEXT_H */
/* End DCEXT.H */
