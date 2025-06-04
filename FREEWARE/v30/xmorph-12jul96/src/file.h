/* file.h -- file handling routines header
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _FILE_H__INCLUDED_
#define _FILE_H__INCLUDED_

#include <stdio.h>

/* ====================================================================== */

/* get_byte : get from fio a byte
*/
#define GET_BYTE(byte, fio) \
{\
  int rv;\
  (byte)=(unsigned char)(rv=getc(fio));\
  if(rv==EOF) {\
    fprintf(stderr, "get_byte: EOF/read error\n");\
    return(EOF);\
  }\
}

/* ====================================================================== */

/* put_le_word : put into fio a 2-byte little-endian unsigned integer */
extern int put_le_word(short le_int, FILE *fio);

/* get_le_word : get from fio a 2-byte little-endian unsigned integer */
extern long get_le_word(FILE *fio);

/* get_block : get from fio an block of n bytes, and store it in buf */
extern short get_block(FILE *fio, char *buf, long n);

#endif /* _FILE_H__INCLUDED_ */
