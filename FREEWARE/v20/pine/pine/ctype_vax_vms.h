#ifndef __CTYPE_LOADED
#define __CTYPE_LOADED	1

/*	CTYPE - V3.0 - Character Type Classification Macros	*/

#define _U	0x1
#define _L	0x2
#define _D	0x4
#define _S	0x8
#define _P	0x10
#define _C	0x20
#define _X	0x40
#define _B	0x80

#pragma nostandard
#ifdef CC$VAXCSHR
#define	_ctype_	(*vaxc$ga__ctype_)
globalref char _ctype_[];
#else
globalvalue c$v_ctypedefs;

#undef	readonly	/*~~~~*/
extern char  readonly _ctype_[];
#define	readonly	ReadOnly	/*~~~~*/
#endif

#undef isupper
#undef islower
#undef isalpha
#undef isdigit
#undef isalnum
#undef isxdigit
#undef isspace
#undef ispunct
#undef isgraph
#undef isprint
#undef iscntrl
#undef isascii

#pragma standard

#define isupper(c)	(_ctype_[(c) & 0x7F] & _U)
#define islower(c)	(_ctype_[(c) & 0x7F] & _L)
#define isalpha(c)	(_ctype_[(c) & 0x7F] & (_U | _L))
#define isdigit(c)	(_ctype_[(c) & 0x7F] & _D)
#define isalnum(c)	(_ctype_[(c) & 0x7F] & (_U | _L | _D))
#define isxdigit(c)	(_ctype_[(c) & 0x7F] & _X)
#define isspace(c)	(_ctype_[(c) & 0x7F] & _S)
#define ispunct(c)	(_ctype_[(c) & 0x7F] & _P)
#define isgraph(c)	(_ctype_[(c) & 0x7F] & (_P | _U | _L | _D))
#define isprint(c)	(_ctype_[(c) & 0x7F] & (_P | _U | _L | _D | _B))
#define iscntrl(c)	(_ctype_[(c) & 0x7F] & _C)
#define isascii(c)	((unsigned)(c) <= 0x7F)

#define _toupper(c)	((c) >= 'a' && (c) <= 'z' ? (c) & 0x5F:(c))
#define _tolower(c)	((c) >= 'A' && (c) <= 'Z' ? (c) | 0x20:(c))

#define toascii(c)	((c) & 0x7F)

int toupper(char c);

int tolower(char c);

#endif					/* __CTYPE_LOADED */
