/* DCREC.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCREC_H
#define DCREC_H

#include "dc.th"

/* Function profiles */
extern void analNumCst(void(*)(char)), analStrCst(void(*)(char));
extern void getTokFromTxt(void);
extern bool isAlfa(char);
extern void skipTok(const Ttok[]);

extern Tchar tokTxtBuf[MaxLgtId + 1];  /* general use buffer */

#endif  /* ifndef DCREC_H */
/* End DCREC.H */
