#include <X11/keysym.h>

/* various keys on keyboard, Sun4 & Linux tested */
#ifdef linux
#define PgUp XK_Prior
#define PgDn XK_Next
#define Home XK_Home
#define End  XK_End
#else
#define PgUp XK_R9
#define PgDn XK_R15
#define Home XK_R7
#define End  XK_R13
#endif
#define UpArrow XK_Up
#define DownArrow XK_Down
#define LeftArrow XK_Left
#define RightArrow XK_Right
