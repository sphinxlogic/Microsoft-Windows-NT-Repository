/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
  crypt.h (full version) by Info-ZIP.   Last revised:  [see CR_VERSION_DATE]

  This encryption/decryption source code for Info-Zip software was
  originally written in Europe.  The whole source package can be
  freely distributed, including from the USA.  (Prior to January 2000,
  re-export from the US was a violation of US law.)

  NOTE on copyright history:
  Previous versions of this source package (up to version 2.8) were
  not copyrighted and put in the public domain.  If you cannot comply
  with the Info-Zip LICENSE, you may want to look for one of those
  public domain versions.
 */

#ifndef __crypt_h   /* don't include more than once */
#define __crypt_h

#ifdef CRYPT
#  undef CRYPT
#endif
/*
   Logic of selecting "full crypt" code:
   a) default behaviour:
      - dummy crypt code when used to compile Zip
        (because we do not distribute encrypting versions of Zip from US
        servers)
      - dummy crypt code when compiling UnZipSFX stub, to minimize size
      - full crypt code when used to compile UnZip and fUnZip
   b) USE_CRYPT defined:
      - always full crypt code
   c) NO_CRYPT defined:
      - never full crypt code
   NO_CRYPT takes precedence over USE_CRYPT
 */
#if defined(NO_CRYPT)
#  define CRYPT  0  /* dummy version */
#else
#if defined(USE_CRYPT)
#  define CRYPT  1  /* full version */
#else
#if (!defined(ZIP) && !defined(SFX))
#  define CRYPT  1  /* full version */
#else
#  define CRYPT  0  /* dummy version */
#endif
#endif /* ?USE_CRYPT */
#endif /* ?NO_CRYPT */

#if CRYPT
/* full version */

#ifdef CR_BETA
#  undef CR_BETA    /* this is not a beta release */
#endif

#define CR_MAJORVER        2
#define CR_MINORVER        9
#ifdef CR_BETA
#  define CR_BETA_VER      "a BETA"
#  define CR_VERSION_DATE  "05 May 2000"       /* last real code change */
#else
#  define CR_BETA_VER      ""
#  define CR_VERSION_DATE  "05 May 2000"       /* last public release date */
#  define CR_RELEASE
#endif

#ifndef __G         /* UnZip only, for now (DLL stuff) */
#  define __G
#  define __G__
#  define __GDEF
#  define __GPRO    void
#  define __GPRO__
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32)
#  ifndef DOS_OS2_W32
#    define DOS_OS2_W32
#  endif
#endif

#if defined(DOS_OS2_W32) || defined(__human68k__)
#  ifndef DOS_H68_OS2_W32
#    define DOS_H68_OS2_W32
#  endif
#endif

#if defined(VM_CMS) || defined(MVS)
#  ifndef CMS_MVS
#    define CMS_MVS
#  endif
#endif

/* To allow combining of Zip and UnZip static libraries in a single binary,
 * the Zip and UnZip versions of the crypt core functions have to be named
 * differently.
 */
#ifdef ZIP
#  ifdef REALLY_SHORT_SYMS
#    define decrypt_byte   zdcrby
#  else
#    define decrypt_byte   zp_decrypt_byte
#  endif
#  define  update_keys     zp_update_keys
#  define  init_keys       zp_init_keys
#else /* !ZIP */
#  ifdef REALLY_SHORT_SYMS
#    define decrypt_byte   dcrbyt
#  endif
#endif /* ?ZIP */

#define IZ_PWLEN  80    /* input buffer size for reading encryption key */
#ifndef PWLEN           /* for compatibility with previous zcrypt release... */
#  define PWLEN IZ_PWLEN
#endif
#define RAND_HEAD_LEN  12       /* length of encryption random header */

/* the crc_32_tab array has to be provided externally for the crypt calculus */
#ifndef CRC_32_TAB                   /* UnZip provides this in globals.h */
# if (!defined(USE_ZLIB) || defined(USE_OWN_CRCTAB))
   extern ZCONST ulg near *crc_32_tab;
# else
   extern ZCONST ulg Far *crc_32_tab;
# endif
#endif /* !CRC_32_TAB */

/* encode byte c, using temp t.  Warning: c must not have side effects. */
#define zencode(c,t)  (t=decrypt_byte(__G), update_keys(c), t^(c))

/* decode byte c in place */
#define zdecode(c)   update_keys(__G__ c ^= decrypt_byte(__G))

int  decrypt_byte OF((__GPRO));
int  update_keys OF((__GPRO__ int c));
void init_keys OF((__GPRO__ ZCONST char *passwd));

#ifdef ZIP
   void crypthead OF((ZCONST char *, ulg, FILE *));
#  ifdef UTIL
     int zipcloak OF((struct zlist far *, FILE *, FILE *, ZCONST char *));
     int zipbare OF((struct zlist far *, FILE *, FILE *, ZCONST char *));
#  else
     unsigned zfwrite OF((zvoid *, extent, extent, FILE *));
     extern char *key;
#  endif
#endif /* ZIP */

#if (defined(UNZIP) && !defined(FUNZIP))
   int  decrypt OF((__GPRO__ ZCONST char *passwrd));
#endif

#ifdef FUNZIP
   extern int encrypted;
#  ifdef NEXTBYTE
#    undef NEXTBYTE
#  endif
#  define NEXTBYTE \
   (encrypted? update_keys(__G__ getc(G.in)^decrypt_byte(__G)) : getc(G.in))
#endif /* FUNZIP */

#else /* !CRYPT */
/* dummy version */

#define zencode
#define zdecode

#define zfwrite  fwrite

#endif /* ?CRYPT */
#endif /* !__crypt_h */
