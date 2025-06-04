/* DCMAIN.PH */
/* Y.L. Noyelle, Supelec, France 1998 */

#ifndef DCMAIN_PH
#define DCMAIN_PH

#include "dc.th"

#define BackUp		ptrS--  /* to be used at most once (after a call to
					 NxtCh), because of circular buffer. */
#define NxtCh	/* gives back, in variable 'c', next source character;         \
		  manages trigraphs, line splicing, and buffer (re-)filling. */\
  for (;;) {								       \
    while ((c = *ptrS++)==EndCh && ptrS==posEndChP1) ptrS = nxtChunkOfTxt(     \
								       ptrS);  \
/*    if (c == '\?') {		*//* trigraph? *//*			       \
      while ((c = *ptrS++)==EndCh && ptrS==posEndChP1) {ptrS = nxtChunkOfTxt(  \
								       ptrS);} \
      if (c == '\?') ptrS = trigraph(ptrS, &c);*//* *doIt* c not ds reg *//*   \
      else {BackUp; c = '\?';}						       \
      break;}*/								       \
    if (c!='\\' || !splice(ptrS)) break; ptrS--;}	/* splicing? */

/* Function profiles */
extern void endPrgF(register char *ptrS);
extern void processInclude(
#ifdef VMS
	                   TstringNC
#else
	                   Tstring
#endif
		 		    , bool, bool, bool);
extern char *nxtChunkOfTxt(char *);
extern void saveTokChar(char);
extern bool splice(char *);

/* Objects */
extern char *begSBuf;	/* beginning of source buffer (circular buffer) */
extern bool fileClosed;
extern bool frstLineOfFile;
extern char *nlPosP1;	/* 1 + position of last seen 'newline' char */
extern char *posEndChP1;/* 1 + position of EndCh character indicating end of
							  current text chunk. */
extern char *srcPtr;    /* source pointer */
extern Tstring starterFile;

#endif  /* ifndef DCMAIN_PH */
/* End DCMAIN.PH */
