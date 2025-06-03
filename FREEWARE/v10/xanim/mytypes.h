#ifdef ALPHA
#include "ints.h"
#define LONG int32
#define ULONG uint32
#define BYTE int8
#define UBYTE uint8
#define SHORT int16
#define USHORT uint16
#define WORD int16
#define UWORD uint16
#else
#define LONG long int
#define ULONG unsigned long int
#define BYTE char
#define UBYTE unsigned char
#define SHORT short int
#define USHORT unsigned short int
#define WORD short int
#define UWORD unsigned short int
#endif
#define TRUE 1
#define FALSE 0
