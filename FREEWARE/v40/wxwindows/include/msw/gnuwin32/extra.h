/*
 * Some extra defines, types and functions that GNU-WIN32 doesn't yet
 * have (B18).
 */

/* WM_NCHITTEST message */
#ifndef HTMINBUTTON
#define HTMINBUTTON	(8)
#define HTMAXBUTTON	(9)
#endif

#ifndef TMPF_VECTOR
#define TMPF_VECTOR 0x02
#define TMPF_TRUETYPE 0x04
#endif

typedef char *NPSTR;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

HDDEDATA WINAPI DdeClientTransaction(LPBYTE, DWORD, HCONV, HSZ, UINT, UINT, DWORD, LPDWORD);
HDDEDATA WINAPI DdeCreateDataHandle(DWORD, LPBYTE, DWORD, DWORD, HSZ, UINT, UINT);

#ifdef __cplusplus
}
#endif /* __cplusplus */

