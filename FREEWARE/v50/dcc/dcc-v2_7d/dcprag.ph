/* DCPRAG.PH */
/* Y.L. Noyelle, Supelec, France 1994 */

#ifndef DCPRAG_PH
#define DCPRAG_PH

#include "dc.th"

#define DPragPrefix	"/*~"
#define DPragSuffix	"*/"

/* Functions profiles */
extern void freeTypeCmbn(void);
extern void initPrag(void);
extern TvalTok manageDPrag(void);

/* External objects */
extern uint ctrTCB;
extern bool noWarn;

#endif /* ifndef DCPRAG_PH */
/* End DCPRAG.PH */
