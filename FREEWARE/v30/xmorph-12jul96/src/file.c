/* file.c -- file handling routines
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include <string.h>

#include "file.h"

/* ====================================================================== */

/* put_le_word : put into fio a 2-byte little-endian unsigned integer
*/
int
put_le_word(short le_int, FILE *fio)
{
  unsigned char b1, b2;

  b1 = le_int & 0xff;

  b2 = (le_int >> 8) & 0xff;

  /* least significant byte comes first */
  if(putc(b1, fio)==EOF) return(EOF);

  /* most significant byte comes last */
  if(putc(b2, fio)==EOF) return(EOF);

  return(0);
}

/* ---------------------------------------------------------------------- */

/* get_le_word : get from fio a 2-byte little-endian unsigned integer
*/
long
get_le_word(FILE * fio)
{
  unsigned char b1, b2;

  /* least significant byte comes first */
  GET_BYTE(b1, fio);

  /* most significant byte comes last */
  GET_BYTE(b2, fio);

  return(b1 + b2*256);
}

/* ---------------------------------------------------------------------- */

/* get_block : get from fio an block of n bytes, and store it in buf
** return EOF if there is a read error, or 0 otherwise
*/
short
get_block(FILE * fio, char *buf, long n)
{
  int rv;

  if(feof(fio)) return(EOF);
  rv=fread(buf, 1, (size_t)n, fio);
  if(rv!=n) {
    if(rv) {
      fprintf(stderr, "get_block: EOF/read error reading byte %i/%i\n", rv, n);
    }
    return(EOF);
  }

  return(0);
}
