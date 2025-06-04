/* DCPRAG.H */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifndef DCPRAG_H
#define DCPRAG_H

#include "dc.th"

typedef enum {IlgTcOp = -1, AddTC, SubTC, MulTC, DivTC, ModTC} TkOpTC;  /* or-
					      der important (cf 'searchTC()'. */
/*~zif ModTC-AddTC != ModAsgn-AddAsgn "Bad TkOpTC enum" */

typedef struct _TCBlk TCBlk;

/* External functions */
extern Tstring dpName(Ttok);
extern bool isFNameVisible(Tstring fileName, const TsemanElt *pTag);
extern TpcTypeElt searchTC(TkOpTC);

/* External variables */
extern TCBlk *(*pHeadTCBLists)[(TkOpTC)(ModTC - AddTC + 1)];

#endif  /* ifndef DCPRAG_H */
/* End DCPRAG.H */
