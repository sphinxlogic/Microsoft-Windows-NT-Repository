/* DCREC.H */
/* Y.L. Noyelle, Supelec, France 1996 */

#ifndef DCREC_H
#define DCREC_H

#include "dc.th"

#define CmtAtBegLine	-1
#define Convert		NULL
#define IsKW		InsideInterval(curTok.tok, BKeyWords+1, EKeyWords-1)
#define NoConv          QuasiNULLval(void (*)(char))
#define SetInGetTokFrmMac (CmtAtBegLine - 1)
#define SetInGetTokFrmTxt (SetInGetTokFrmMac - 1)

/* Function profiles */
extern void analNumCst(void(*)(char /*~Generic*/));
extern void analStrCst(void(*)(char /*~Generic*/));
extern void analTok(void);
extern Tstring curTokTxt(void);
extern void findIndentCnt(void);
extern void fullCurTokTxt(void(*)(Tchar));
extern void getTokFromTxt(void);
extern bool isAlnu(char);
extern void skipTok(const Ttok[]);

/* External objects */
extern TdescrId *pDescTokId;
extern Tchar tokTxtBuf[MaxLgtId + 1];  /* general use buffer */

#endif  /* ifndef DCREC_H */
/* End DCREC.H */
