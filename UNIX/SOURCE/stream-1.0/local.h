/*
        Some declarations I like,

                John Hughes, 24/10/90
*/

#ifndef PRIVATE
#define PRIVATE static
#endif

#ifndef EXPORT
#define EXPORT
#endif

#ifndef IMPORT
#define IMPORT extern
#endif

typedef enum BOOL {FALSE = 0, TRUE = !0} BOOL;

#ifdef __GNUC__

#undef MIN
#undef MAX

#ifdef __STDC__

PRIVATE __inline int MIN (int a, int b) { return a < b ? a : b; }
PRIVATE __inline int MAX (int a, int b) { return a > b ? a : b; }

#else

PRIVATE __inline int MIN (a, b) int a, b; { return a < b ? a : b; }
PRIVATE __inline int MAX (a, b) int a, b; { return a > b ? a : b; }

#endif
#else

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif
#endif

