/* $XConsortium: t1stdio.h,v 1.4 91/10/10 11:19:49 rws Exp $ */
/* Copyright International Business Machines,Corp. 1991
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is
 * hereby granted, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * IBM PROVIDES THIS SOFTWARE "AS IS", WITHOUT ANY WARRANTIES
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT
 * LIMITED TO ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS.  THE ENTIRE RISK AS TO THE QUALITY AND
 * PERFORMANCE OF THE SOFTWARE, INCLUDING ANY DUTY TO SUPPORT
 * OR MAINTAIN, BELONGS TO THE LICENSEE.  SHOULD ANY PORTION OF
 * THE SOFTWARE PROVE DEFECTIVE, THE LICENSEE (NOT IBM) ASSUMES
 * THE ENTIRE COST OF ALL SERVICING, REPAIR AND CORRECTION.  IN
 * NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* T1IO FILE structure and related stuff */

#include "types.h"

#undef  getc
#undef  fclose
#undef  ungetc
#undef  fgetc
#undef  fread
#undef  feof
#undef  ferror
#undef  FILE

#define FILE F_FILE
typedef unsigned char F_char;
 
typedef struct F_FILE {
  F_char  *b_base;    /* Pointer to beginning of buffer */
  LONG    b_size;     /* Size of the buffer */
  F_char  *b_ptr;     /* Pointer to next char in buffer */
  LONG    b_cnt;      /* Number of chars remaining in buffer */
  F_char  flags;      /* other flags; != 0 means getc must call fgetc */
  F_char  ungotc;     /* Place for ungotten char; flag set if present */
  short   error;      /* error status */
  int     fd;         /* underlying file descriptor */
} F_FILE;
 
 
/* defines for flags */
#define UNGOTTENC (0x01)
#define FIOEOF    (0x80)
#define FIOERROR  (0x40)
 
#ifndef NULL
#define NULL 0       /* null pointer */
#endif
#define EOF (-1)     /* end of file */
#define F_BUFSIZ (512)
 
#define getc(f) \
  ( \
   ( ((f)->b_cnt > 0) && ((f)->flags == 0) ) ? \
   ( (f)->b_cnt--, (unsigned int)*( (f)->b_ptr++ ) ) : \
   T1Getc(f) \
  )
 
extern FILE *T1Open(), *T1eexec();
extern int T1Close(), T1Ungetc(), T1Read();

#define  fclose(f)          T1Close(f)
#define  fopen(name,mode)   T1Open(name,mode)
#define  ungetc(c,f)        T1Ungetc(c,f)
#define  fgetc(f)           T1Getc(f)
#define  fread(bufP,size,n,f) T1Read(bufP,size,n,f)
#define  feof(f)            (((f)->flags & FIOEOF) && ((f)->b_cnt==0))
#define  ferror(f)          (((f)->flags & FIOERROR)?(f)->error:0)
