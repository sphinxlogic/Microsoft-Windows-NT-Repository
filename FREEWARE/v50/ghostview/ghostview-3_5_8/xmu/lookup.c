/* "$XConsortium: Lookup.c,v 1.12 90/12/11 13:19:10 rws Exp $"; */

/* 
 * Copyright 1988, 1989 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */

#ifdef VMS
#   include <X11_DIRECTORY/Xlib.h>
#   include <X11_DIRECTORY/Xresource.h>
#   include <X11_DIRECTORY/Xutil.h>
#else
#   include <X11/Xlib.h>
#   include <X11/Xutil.h>
#endif

#define XK_LATIN1
#define XK_PUBLISHING

#ifdef VMS
#   include <X11_DIRECTORY/keysymdef.h>
#   include <X11_DIRECTORY/Xutil.h>
#else
#   include <X11/keysymdef.h>
#   include <X11/Xutil.h>
#endif

#if __STDC__
#define Const const
#else
#define Const /**/
#endif

/* bit (1<<i) means character is in codeset i */
static unsigned short Const latin1[128] =
  {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x10ee, 0x0000, 0x1000, 0x1084, 0x102e, 0x1000, 0x1080, 0x108e, /* 10 */
   0x108e, 0x1080, 0x0000, 0x1080, 0x1080, 0x10ee, 0x1000, 0x1008,
   0x108e, 0x1080, 0x1084, 0x1084, 0x108e, 0x1004, 0x1000, 0x1084, /* 11 */
   0x100e, 0x1000, 0x0000, 0x1080, 0x1000, 0x1084, 0x1000, 0x0000,
   0x0004, 0x000e, 0x000e, 0x0008, 0x000e, 0x0008, 0x0008, 0x0006, /* 12 */
   0x0004, 0x000e, 0x0004, 0x000e, 0x0004, 0x000e, 0x000e, 0x0004,
   0x0000, 0x0004, 0x0004, 0x0006, 0x000e, 0x0008, 0x000e, 0x000e, /* 13 */
   0x0008, 0x0004, 0x000e, 0x000c, 0x000e, 0x0002, 0x0000, 0x000e,
   0x0004, 0x000e, 0x000e, 0x0008, 0x000e, 0x0008, 0x0008, 0x0006, /* 14 */
   0x0004, 0x000e, 0x0004, 0x000e, 0x0004, 0x000e, 0x000e, 0x0004,
   0x0000, 0x0004, 0x0004, 0x0006, 0x000e, 0x0008, 0x000e, 0x000e, /* 15 */
   0x0008, 0x0004, 0x000e, 0x000c, 0x000e, 0x0002, 0x0000, 0x0000};

/* bit (1<<i) means character is in codeset i */
static unsigned short Const latin2[128] =
  {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0008, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 10 */
   0x0000, 0x0008, 0x0004, 0x0000, 0x0000, 0x0000, 0x0008, 0x0004,
   0x0000, 0x0008, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0008, /* 11 */
   0x0000, 0x0008, 0x0004, 0x0000, 0x0000, 0x0000, 0x0008, 0x0004,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 12 */
   0x0008, 0x0000, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 13 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 14 */
   0x0008, 0x0000, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
   0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 15 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000c};

/* maps Cyrillic keysyms to 8859-5 */
static unsigned char Const cyrillic[128] =
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xf2, 0xf3, 0xf1, 0xf4, 0xf5, 0xf6, 0xf7, /* 10 */
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0x00, 0xfe, 0xff,
    0xf0, 0xa2, 0xa3, 0xa1, 0xa4, 0xa5, 0xa6, 0xa7, /* 11 */
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0x00, 0xae, 0xaf,
    0xee, 0xd0, 0xd1, 0xe6, 0xd4, 0xd5, 0xe4, 0xd3, /* 12 */
    0xe5, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde,
    0xdf, 0xef, 0xe0, 0xe1, 0xe2, 0xe3, 0xd6, 0xd2, /* 13 */
    0xec, 0xeb, 0xd7, 0xe8, 0xed, 0xe9, 0xe7, 0xea,
    0xce, 0xb0, 0xb1, 0xc6, 0xb4, 0xb5, 0xc4, 0xb3, /* 14 */
    0xc5, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe,
    0xbf, 0xcf, 0xc0, 0xc1, 0xc2, 0xc3, 0xb6, 0xb2, /* 15 */
    0xcc, 0xcb, 0xb7, 0xc8, 0xcd, 0xc9, 0xc7, 0xca};

/* maps Greek keysyms to 8859-7 */
static unsigned char Const greek[128] =
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xb6, 0xb8, 0xb9, 0xba, 0xda, 0x00, 0xbc, /* 10 */
    0xbe, 0xdb, 0x00, 0xbf, 0x00, 0x00, 0xb5, 0xaf,
    0x00, 0xdc, 0xdd, 0xde, 0xdf, 0xfa, 0xc0, 0xfc, /* 11 */
    0xfd, 0xfb, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* 12 */
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd3, 0x00, 0xd4, 0xd5, 0xd6, 0xd7, /* 13 */
    0xd8, 0xd9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* 14 */
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf3, 0xf2, 0xf4, 0xf5, 0xf6, 0xf7, /* 15 */
    0xf8, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define sLatin1		0
#define sLatin2		1
#define sLatin3		2
#define sLatin4		3
#define sKana		4
#define sX0201		0x01000004
#define sArabic		5
#define sCyrillic	6
#define sGreek		7
#define sAPL		11
#define sHebrew		12

int XmuLookupString (event, buffer, nbytes, keysym, status, keysymSet)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
    unsigned long keysymSet;
{
    int count;
    KeySym symbol;
    unsigned long kset;

    kset = keysymSet & 0xffffff;
    count = XLookupString(event, buffer, nbytes, &symbol, status);
    if (keysym) *keysym = symbol;
    if ((nbytes == 0) || (symbol == NoSymbol)) {
	/* nothing */
    } else if ((count == 0) && ((symbol >> 8) == kset)) {
	count = 1;
	switch (keysymSet) {
	case sKana:
	    buffer[0] = (symbol & 0xff);
	    if (buffer[0] == 0x7e)
		count = 0;
	    break;
	case sCyrillic:
	    buffer[0] = cyrillic[symbol & 0x7f];
	    break;
	case sGreek:
	    buffer[0] = greek[symbol & 0x7f];
	    if (!buffer[0])
		count = 0;
	    break;
	default:
	    buffer[0] = (symbol & 0xff);
	    break;
	}
    } else if ((keysymSet != 0) && (count == 1) &&
	       (((unsigned char *)buffer)[0] == symbol) &&
	       (symbol & 0x80) &&
	       !(latin1[symbol & 0x7f] & (1 << kset))) {
	if ((keysymSet == sHebrew) && (symbol == XK_multiply))
	    buffer[0] = 0xaa;
	else if ((keysymSet == sHebrew) && (symbol == XK_division))
	    buffer[0] = 0xba;
	else if ((keysymSet == sCyrillic) && (symbol == XK_section))
	    buffer[0] = 0xfd;
	else if ((keysymSet == sX0201) && (symbol == XK_yen))
	    buffer[0] = 0x5c;
	else
	    count = 0;
    } else if (count != 0) {
	if ((keysymSet == sX0201) &&
	    ((symbol == XK_backslash) || (symbol == XK_asciitilde)))
	    count = 0;
    } else if (((symbol >> 8) == sLatin2) &&
	       (symbol & 0x80) && (latin2[symbol & 0x7f] & (1 << kset))) {
	buffer[0] = (symbol & 0xff);
	count = 1;
    } else if ((keysymSet == sGreek) &&
	       ((symbol == XK_leftsinglequotemark) ||
		(symbol == XK_rightsinglequotemark))) {
	buffer[0] = symbol - (XK_leftsinglequotemark - 0xa1);
	count = 1;
    }
    return count;
}

/* produces ISO 8859-1 encoding plus ASCII control */
int XmuLookupLatin1 (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XLookupString(event, buffer, nbytes, keysym, status);
}

/* produces ISO 8859-2 encoding plus ASCII control */
int XmuLookupLatin2 (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sLatin2);
}

/* produces ISO 8859-3 encoding plus ASCII control */
int XmuLookupLatin3 (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sLatin3);
}

/* produces ISO 8859-4 encoding plus ASCII control */
int XmuLookupLatin4 (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sLatin4);
}

/* produces ISO 8859-1 GL plus Katakana plus ASCII control */
int XmuLookupKana (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sKana);
}

/* produces JIS X0201-1976 (8-bit) */
int XmuLookupJISX0201 (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sX0201);
}

/* produces ISO 8859-6 encoding plus ASCII control */
int XmuLookupArabic (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sArabic);
}

/* produces ISO/IEC 8859-5 encoding plus ASCII control */
int XmuLookupCyrillic (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sCyrillic);
}

/* produces ISO 8859-7 encoding plus ASCII control */
int XmuLookupGreek (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sGreek);
}

/* XXX this character set needs work */

int XmuLookupAPL (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sAPL);
}

/* produces ISO 8859-8 encoding plus ASCII control */
int XmuLookupHebrew (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;
    int nbytes;
    KeySym *keysym;
    XComposeStatus *status;
{
    return XmuLookupString(event, buffer, nbytes, keysym, status, sHebrew);
}
