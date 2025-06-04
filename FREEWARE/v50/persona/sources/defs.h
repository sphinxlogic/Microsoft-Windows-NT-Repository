
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |              PERSONA              %%   \
 *                %% |           defs.h   c2001          %%    \
 *                %% |            Lyle W. West           %%    |
 *                %% |                                   %%    |
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
 *                \                                        \   |
 *                 \                                        \  |
 *                  \                                        \ |
 *                   \________________________________________\|
 *
 *
 *
 *  Copyright (C) 2001 Lyle W. West, All Rights Reserved.
 *  Permission is granted to copy and use this program so long as [1] this
 *  copyright notice is preserved, and [2] no financial gain is involved
 *  in copying the program.  This program may not be sold as "shareware"
 *  or "public domain" software without the express, written permission
 *  of the author.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct  IOSB {              /* Standard I/O status block */
    short   status;
    short   byte_cnt;
    int     unused;
};

struct  ITEM {              /* VMS item list */
    short   buf_size;
    short   item_code;
    int     *buf_addr;
    int     *buf_len;
};

#define RMS$_RNF 98994

#define OLDDEV 1
#define NEWDEV 0


typedef struct _buff {      /* pseudoterm read/write buffers */
    short status;
    short length;
    char dat[508];
} buff;


