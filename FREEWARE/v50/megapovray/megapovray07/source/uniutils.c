/*
 * File: uniutils.c
 *
 * Author: Jon A. Cruz
 *         joncruz@geocities.com
 *
 *
 * Module that contains various routines to handle Unicode strings.
 *
 *
 * Includes a simple implementation of UTF-8 to UCS-2 conversion routine.
 * ( described in RFC 2279 )
 *
 *
 *     Jan 23, 1999 - Initial version
 */

/*
 * Modifications For OpenVMS By Robert Alan Byer <byer@mail.ourservers.net>
 * Feb. 12, 2001
 */

#include <stdlib.h>
#include <string.h>

#include "frame.h"
#include "povray.h"
#include "uniutils.h"

#define MASK_1_BYTE 0x80
#define MASK_2_BYTE 0xe0
#define MASK_3_BYTE 0xf0
#define MASK_4_BYTE 0xf8
#define MASK_5_BYTE 0xfc
#define MASK_6_BYTE 0xfe
#define MASK_TRAIL_BYTE 0xc0

#define VAL_1_BYTE 0x00
#define VAL_2_BYTE 0xc0
#define VAL_3_BYTE 0xe0
#define VAL_4_BYTE 0xf0
#define VAL_5_BYTE 0xf8
#define VAL_6_BYTE 0xfc
#define VAL_TRAIL_BYTE 0x80

#define REPLACEMENT_CHARACTER 0xfffd
/*YS*/

/*
 * Like MacOS, OpenVMS dosen't appear to have a 'stricmp' function.
 */
#if defined(MACOS) || (defined(VMS) && defined(DECC))
	int pov_stricmp (char *s1,char *s2);
	#define STRICMP pov_stricmp
#else
	#ifdef UNIX
  		#define STRICMP strcasecmp
	#else
		#define STRICMP stricmp
	#endif
#endif	
/*YS*/

/*
Name: ECMA-cyrillic                                      [RFC1345,KXS2]
MIBenum: 77
Source: ECMA registry
Alias: iso-ir-111
Alias: csISO111ECMACyrillic
*/

#define ENCODING_UTF8       0   /* "UTF-8" - RFC2279 */
#define ENCODING_8859_1     1   /* "ISO-8859-1" - RFC1345 */
#define ENCODING_APPLEBYTE  2   /* "" */
#define ENCODING_CP1252     3   /* "windows-1251" ("Cp1252") guess. */
#define ENCODING_MAC_ROMAN  4   /* "macintosh" ("mac") - RFC1345 */

/*
 * Warning: Need to keep these in sync
 */
#define _UNIUTILS_FIRST_ENCODING ENCODING_UTF8
#define _UNIUTILS_LAST_ENCODING ENCODING_MAC_ROMAN


typedef struct _EncodingNamer {
    int encoding;
    char *name;
} EncodingNamer;

/*YS*/
int charToWide( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff );
int cp1252ToWide( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff );
int macRomanToWide( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff );
/*YS*/
/*

    IANA MIME prefered  Java
    "UTF-8"             "UTF8"
    "ISO-8859-1"        "ISO8859_1"
    "windows-1252"      "Cp1252"
    "macintosh"         "MacRoman"

*/
EncodingNamer _matchNames[] = {
    {ENCODING_UTF8, "utf8"},
    {ENCODING_UTF8, "utf-8"},
    {ENCODING_8859_1, "iso8859_1"},
    {ENCODING_8859_1, "iso-8859-1"},
/*    {ENCODING_APPLEBYTE, "applebyte"},    need to wait on this until font glyph cacheing is fixed
*/
    {ENCODING_CP1252, "cp1252"},
    {ENCODING_CP1252, "cp-1252"},
    {ENCODING_CP1252, "windows-1252"},
    {ENCODING_MAC_ROMAN, "macroman"},
    {ENCODING_MAC_ROMAN, "macintosh"},
    {ENCODING_MAC_ROMAN, "mac"},
    {ENCODING_INVALID, NULL},
};

/*
 * Converts UTF-8 to UCS-2 (16-bit Unicode)
 * 
 * Note: This code will currently drop all UCS-4 characters.
 * However, the code is started faily simply, and has some of the work
 * alredy done for those characters (UTF-8 sequences over 3 bytes in length)
 */
int uniUtf8ToWide( const unsigned char *pszUtf8, wchar_t *pwDstBuff, int sbDstBuff ) {
    int newChars = 0;
    unsigned char byteVal;
    unsigned char bitMask;
    wchar_t uniVal;
    int sequenceLength;
    int isGood;
    int i;
    const unsigned char *pszCurr;

    if ( pszUtf8 == NULL
        || ( ( sbDstBuff != 0 ) && !pwDstBuff)
        || sbDstBuff < 0
        ) {

        goto EXIT_ERR;
    }

    if ( pszUtf8 != NULL ) {
        pszCurr = pszUtf8;
        while( pszCurr && *pszCurr ) {
            byteVal = *pszCurr;
            pszCurr++;
            if ( byteVal & 0x80 ) {
                isGood = 1;

                /* is a sequence byte */
                if ( ( byteVal & MASK_2_BYTE ) == VAL_2_BYTE ) {
                    sequenceLength = 2;
                    bitMask = (unsigned char)~MASK_2_BYTE;
                } else if ( ( byteVal & MASK_3_BYTE ) == VAL_3_BYTE ) {
                    sequenceLength = 3;
                    bitMask = (unsigned char)~MASK_3_BYTE;
                } else if ( ( byteVal & MASK_4_BYTE ) == VAL_4_BYTE ) {
                    sequenceLength = 4;
                    bitMask = (unsigned char)~MASK_4_BYTE;
                } else if ( ( byteVal & MASK_5_BYTE ) == VAL_5_BYTE ) {
                    sequenceLength = 5;
                    bitMask = (unsigned char)~MASK_5_BYTE;
                } else if ( ( byteVal & MASK_6_BYTE ) == VAL_6_BYTE ) {
                    sequenceLength = 6;
                    bitMask = (unsigned char)~MASK_6_BYTE;
                } else {
                    /* 0xfe and 0xff are illegal UTF-8 chars .*/
                    sequenceLength = 0;
                }

                if ( sequenceLength < 2 || sequenceLength > 3 ) {
                    isGood = 0;
                } else {
                    for ( i = 0; i < sequenceLength - 1; i++ ) {
                        if ( (pszCurr[i] == 0)
                            || ( (pszCurr[i] & MASK_TRAIL_BYTE) != VAL_TRAIL_BYTE )
                            ) {
                            isGood = 0;
                            break;
                        }

                    }
                }

                if ( isGood ) {
                    uniVal = (wchar_t)(byteVal & bitMask);
                    for ( i = 0; i < sequenceLength - 1; i++ ) {
                        uniVal <<= 6;
                        uniVal |= ( pszCurr[i] & (~MASK_TRAIL_BYTE) );
                    }
                } else {
                    uniVal = REPLACEMENT_CHARACTER;
                }

                /* Now, skip to either the next lead byte, or the null terminator */
                while ( *pszCurr && ( ( (*pszCurr) & MASK_TRAIL_BYTE ) == VAL_TRAIL_BYTE ) ) {
                    pszCurr++;
                }
            } else {
                uniVal = (wchar_t)byteVal;
            }
            if ( newChars < sbDstBuff ) {
                pwDstBuff[ newChars ] = uniVal;
            }
            newChars++;
        }
    }

    return newChars;
EXIT_ERR:
    return -1;
}


/*
 * This might be for 8859-1 to UCS2, or bytes to Apple Font.
 */
int charToWide( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff ) {
    int newChars = 0;

    if ( pszString == NULL
        || ( ( sbDstBuff != 0 ) && !pwDstBuff)
        || sbDstBuff < 0
        ) {

        goto EXIT_ERR;
    }

    if ( pszString != NULL ) {
        while ( pszString[ newChars ] ) {
            if ( newChars < sbDstBuff ) {
                pwDstBuff[ newChars ] = 0x00ff & (wchar_t)pszString[ newChars ];
            }
            newChars++;
        }
    }

    return newChars;
EXIT_ERR:
    return -1;

}

int cp1252ToWide( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff ) {
    int newChars = 0;
    wchar_t trickyOnes[] = {
        0x20AC, 0xfffd, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
        0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0xfffd, 0x017D, 0xfffd,
        0xfffd, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
        0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0xfffd, 0x017E, 0x0178,
    };

    if ( pszString == NULL
        || ( ( sbDstBuff != 0 ) && !pwDstBuff)
        || sbDstBuff < 0
        ) {

        goto EXIT_ERR;
    }

    if ( pszString != NULL ) {
        while ( pszString[ newChars ] ) {
            if ( newChars < sbDstBuff ) {
                if ( pszString[ newChars ] < 0x80
                    || 0x9f < pszString[ newChars ] ) {
                    pwDstBuff[ newChars ] = 0x00ff & (wchar_t)pszString[ newChars ];
                } else {
                    pwDstBuff[ newChars ] = trickyOnes[ pszString[ newChars ] - 0x80 ];
                }
            }
            newChars++;
        }
    }

    return newChars;
EXIT_ERR:
    return -1;

}

/* Mac Roman mapping drawn from */
/* ftp://ftp.unicode.org/Public/MAPPINGS/VENDORS/APPLE/ROMAN.TXT updated 1998-Aug-18 */

int macRomanToWide( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff ) {
    int newChars = 0;
    wchar_t trickyOnes[] = {
        0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1,
        0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8,
        0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3,
        0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC,
        0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF,
        0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8,
        0x221E, 0x00B1, 0x2264, 0x2265, 0x00A5, 0x00B5, 0x2202, 0x2211,
        0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 0x00E6, 0x00F8,
        0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB,
        0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153,
        0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA,
        0x00FF, 0x0178, 0x2044, 0x20AC, 0x2039, 0x203A, 0xFB01, 0xFB02,
        0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1,
        0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4,
        0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC,
        0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7,
    };

    if ( pszString == NULL
        || ( ( sbDstBuff != 0 ) && !pwDstBuff)
        || sbDstBuff < 0
        ) {

        goto EXIT_ERR;
    }

    if ( pszString != NULL ) {
        while ( pszString[ newChars ] ) {
            if ( newChars < sbDstBuff ) {
                if ( pszString[ newChars ] < 0x80 ) {
                    pwDstBuff[ newChars ] = 0x00ff & (wchar_t)pszString[ newChars ];
                } else {
                    pwDstBuff[ newChars ] = trickyOnes[ pszString[ newChars ] - 0x80 ];
                }
            }
            newChars++;
        }
    }

    return newChars;
EXIT_ERR:
    return -1;

}


int uniGetEncodingByName( char *str ) {
    int encoding;
    int i;

    if ( str == NULL ) {
        encoding = ENCODING_INVALID;
    } else {
        for ( i = 0; _matchNames[i].encoding != ENCODING_INVALID; i++ ) {
		/*YS*/
		/*declared STRICMP for Mac platform*/ 
            if ( STRICMP( _matchNames[i].name, str ) == 0 ) {
        /*YS*/
                encoding = _matchNames[i].encoding;
                break;
            }
        }
    }

    return encoding;
}


int uniGetDefaultEncoding( void ) {
    return ENCODING_UTF8;
}

int uniIsEncodingValid( int encoding ) {
    int isGood;

    if ( _UNIUTILS_FIRST_ENCODING <= encoding && encoding <= _UNIUTILS_LAST_ENCODING ) {
        isGood = 1;
    } else {
        isGood = 0;
    }

    return isGood;
}

int uniIsAppleByte( int encoding ) {
    int retVal;

    if ( encoding == ENCODING_APPLEBYTE ) {
        retVal = 1;
    } else {
        retVal = 0;
    }

    return retVal;
}

int uniBytesToChars( const unsigned char *pszString, wchar_t *pwDstBuff, int sbDstBuff, int encoding ) {
    int retVal = -1;

    switch ( encoding ) {
        case ENCODING_UTF8:
            retVal = uniUtf8ToWide( pszString, pwDstBuff, sbDstBuff );
            break;
        case ENCODING_8859_1:
        case ENCODING_APPLEBYTE:
            retVal = charToWide( pszString, pwDstBuff, sbDstBuff );
            break;
        case ENCODING_CP1252:
            retVal = cp1252ToWide( pszString, pwDstBuff, sbDstBuff );
            break;
        case ENCODING_MAC_ROMAN:
            retVal = macRomanToWide( pszString, pwDstBuff, sbDstBuff );
            break;
        default:
            retVal = -1;
    }

    return retVal;
}
