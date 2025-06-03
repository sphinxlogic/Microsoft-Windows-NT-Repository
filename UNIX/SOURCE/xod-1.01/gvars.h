#include	<stdio.h>
#include	<X11/X.h>

#ifdef XOD_MAIN
#	define Dextern
#else
#	define Dextern extern
#endif

Dextern FILE *datafp;
Dextern int  FileSize;
Dextern int  DataOffset;
Dextern int  DataMode;
Dextern int  BytesHoriz;
Dextern int  DataLeftCol;
Dextern int  OffsetClickedOn;
Dextern int  MatchAt;
Dextern int  NumberRows;
Dextern int  charwidth;
Dextern int  charheight;
Dextern int  SearchMode;
Dextern char *OffsetFmt;
Dextern int  OffsetMode;
Dextern int  MaxOffset;
Dextern int  CtwColumns;		/* actual width of CTW display, in chars */
