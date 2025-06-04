/* DCREC.PH */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCREC_PH
#define DCREC_PH

#include "dcrecdir.th"

#define RestoSmshCh {							       \
  if (adSmshCh != NULL) {  /* put back in place	possible characters smashed by \
			      preceding identifier length/name space code. */  \
    *adSmshCh = smshCh[0]; *(adSmshCh + 1) = smshCh[1]; adSmshCh = NULL;}}

/* Functions profiles */

extern void backUp(void);
extern ThCode hCodFct(Tname);
extern void ilgCharF(register char *ptrS);
extern void initRec(void);
extern void manageConc(void);
extern char peepNxtNonBlChFromTxt(void);
extern void storeTokSource(void);
extern void storeTokTxt(Tstring);
extern void storeTokTxtRepr(char);

/* Objects */
extern char *adSmshCh;  /* address of characters currently smashed in source
			   buffer by an identifier header (couple id. length,
							      id. nameSpace). */
extern Terr concatErr;
extern bool dirLineFl;
extern char frstChOfNb;
extern bool mngConc;
extern TdescrId *pDescCurId;
extern void (*pStoreChFct)(char);
extern char smshCh[LgtHdrId];  /* buffer to save characters smashed in source
						buffer 	by identifier header. */
extern char *srcPtr;	/* source pointer */
extern TdescrId *symTabHeads[SizeSymTab]; /* symbol table (heads of 'small
								    tables'). */
#endif /* ifndef DCREC_PH */
/* End DCREC.PH */
