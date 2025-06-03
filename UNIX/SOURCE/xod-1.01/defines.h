#define XOD_OK 1
#define XOD_CANCEL 2

/* foreground & background escape sequences */
#define FGBLACK "\033[30m"
#define FGRED "\033[31m"
#define FGGREEN "\033[32m"
#define FGYELLOW "\033[33m"
#define FGBLUE "\033[34m"
#define FGMAGENTA "\033[35m"
#define FGCYAN "\033[36m"
#define FGWHITE "\033[37m"

#define BGBLACK "\033[40m"
#define BGRED "\033[41m"
#define BGGREEN "\033[42m"
#define BGYELLOW "\033[43m"
#define BGBLUE "\033[44m"
#define BGMAGENTA "\033[45m"
#define BGCYAN "\033[46m"
#define BGWHITE "\033[47m"

#define REVVID "\033[7m"
#define UNDERLINE "\033[4m"
#define DATANORMAL "\033[0m\033[34m\033[47m"

/* various decoding modes */
#define HEX 		0
#define OCTAL 		1
#define DECIMAL 	2
#define ASCII 		3
#define PACKASCII 	4

#define SHORT 1
#define USHORT 2
#define LONG 3
#define ULONG 4
#define FLOAT 5
#define DOUBLE 6
#define dASCII 7

/* what user types in search box means */
#define SmGrep 0
#define SmLine 1
#define SmAdvance 2
#define SmRetard 3

#define MAXLEN 256
