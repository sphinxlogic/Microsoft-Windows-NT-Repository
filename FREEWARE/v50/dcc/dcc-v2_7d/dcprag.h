/* DCPRAG.H */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifndef DCPRAG_H
#define DCPRAG_H

#include "dc.th"

#define InsideDPragma	(dpragNst != 0)

typedef enum {IlgTcOp = -1, AddTC, SubTC, MulTC, DivTC, ModTC} TkOpTC;  /* or-
					      der important (cf 'searchTC()'. */
/*~zif ModTC-AddTC != ModAsgn-AddAsgn "Bad TkOpTC enum" */

#define TxtTcOp	"+-*/%"
/*~zif LitLen(TxtTcOp) != __extent(TkOpTC) "TxtTcOp not coherent with TkOpTC"*/

/*~Private*/
typedef struct _tcBlk TCBlk;
/*~Public*/

/* External functions */
extern Tstring dpName(Tdprag);
extern Tstring lastPrivFileName(const TsemanElt *);
extern bool isFNameVisible(Tstring fileName, const TsemanElt *pTag);
extern bool lastPrivFileIsUniq(const TmngtPriv *);
extern TpcTypeElt searchTC(TkOpTC);

/* External variables */
extern TdpNst dpragNst;  /* level of imbrication of d-pragmas (possible because
								  of macros). */
extern TmngtPriv *headListPrivTo;  /* current 'PrivateTo' list of body file
                                                                       names. */
extern TCBlk *(*pHeadTCBLists)[(TkOpTC)(ModTC - AddTC + 1)];

#endif  /* ifndef DCPRAG_H */
/* End DCPRAG.H */
