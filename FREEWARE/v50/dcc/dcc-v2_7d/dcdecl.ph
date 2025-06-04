/* DCDECL.PH */
/* Y.L. Noyelle, Supelec, France 1998 */

#ifndef DCDECL_PH
#define DCDECL_PH

typedef struct _tUndfTagElt TundfTagElt;
struct _tUndfTagElt {
  TsemanElt *pTagId;
  TundfTagElt *prev;
};

extern void initDecl(void);

extern TundfTagElt *headUndfTagsList;

#endif /* ifndef DCDECL_PH */
/* End DCDECL.PH */
