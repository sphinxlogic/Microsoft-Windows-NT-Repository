/************************************************************************
 *
 *  (C) Copyright 1992-1993 The Trustees of Indiana University
 *
 *   Permission to use, copy, modify, and distribute this program for
 *   non-commercial use and without fee is hereby granted, provided that
 *   this copyright and permission notice appear on all copies and
 *   supporting documentation, the name of Indiana University not be used
 *   in advertising or publicity pertaining to distribution of the program
 *   without specific prior permission, and notice be given in supporting
 *   documentation that copying and distribution is by permission of
 *   Indiana University.
 *
 *   Indiana University makes no representations about the suitability of
 *   this software for any purpose. It is provided "as is" without express
 *   or implied warranty.
 *
 **************************************************************************
 *
 * Module:
 *     ucx_compatibility.h - compensate for UCX's API shortcomings.
 *
 ************************************************************************/

typedef long fd_mask;

#define howmany(x, y)   (((x)+((y)-1))/(y))
#define NBBY 8                                  /* number of bits in a byte */
#define FD_SETSIZE 256
#define NFDBITS (sizeof (fd_mask) * NBBY)       /* bits per mask */

typedef struct fd_set 
{
  fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define FD_ZERO(fds)     ((fds)->fds_bits[0] = 0)
#define FD_SET(n, fds)   ((fds)->fds_bits[0] |= (1 << (n)))
#define FD_ISSET(n, fds) ((fds)->fds_bits[0] &  (1 << (n)))

