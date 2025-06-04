/* DCREC.PH */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCREC_PH
#define DCREC_PH

#include "dcrecdir.th"

#define CurTokNotAnalz() (frstChOfNb)
#define ClrSmshCh	adSmshCh = NULL
#define RestoSmshCh do {                                                       \
  if (adSmshCh != NULL) {  /* put back in place possible characters smashed by \
                              preceding identifier length/nameSpace-code. */   \
    register char *w = adSmshCh;; *w = smshCh[0]; *(w + 1) = smshCh[1];}       \
    /*~zif LgtHdrId != 2 "Change lines using 'smshCh'"*/		       \
} while (False)

typedef struct {
  void (*savPGNT)(void);
  char *savSrcPtr;
  bool savME;
  char ppNbSuffixBuf[LgtHdrId];
} TppNbCtx;
/*~zif Offset(TppNbCtx, ppNbSuffixBuf) != sizeof(TppNbCtx)-LgtHdrId
				       "Field 'ppNbSuffixBuf' must come last" */

/* Functions profiles */

extern void backUp(void);
extern ThCode hCodFct(Tname);
extern void ilgCharF(register char *ptrS);
extern void initRec(void);
extern void storeTokTxt(Tstring);

/* Objects */
extern char *adSmshCh;  /* address of characters currently smashed in source
			   buffer by an identifier header (couple id. length,
							      id. nameSpace). */
extern Terr concatErr;
extern bool dirLineFl;
extern bool frstChOfNb;  /* used as flag 'analXXXCst() not yet called' */
extern bool mngConc;
extern TppNbCtx *pPPNbCtx;  /* context of current pp-number suffix */
extern char smshCh[LgtHdrId];  /* buffer to save characters smashed in source
						buffer 	by identifier header. */
extern TdescrId *symTabHeads[SizeSymTab]; /* symbol table (heads of 'small
								    tables'). */
#endif /* ifndef DCREC_PH */
/* End DCREC.PH */
