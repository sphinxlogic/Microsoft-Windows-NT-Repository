/*  Copyright (c) 1995 by Digital Equipment Corporation. All rights reserved.
 *
 *  File:   FASTIO.C
 *  Author: Thierry Lelegard
 *  Date:   10-OCT-1995
 *
 *  Abstract
 *  --------
 *
 *  This program demonstrates the usage of the new Fast I/O interface.
 *  This new interface is designed as a replacement to the traditional
 *  $QIO interface. As of OpenVMS V7.0, only the disk and tape driver
 *  support this interface. Using this interface is slightly more comples
 *  to develop, although faster to execute, than $QIO.
 *
 *  This example program creates a file and fills it with a binary pattern.
 *  By default, the data are written using Fast I/O (routine write_fastio).
 *  However, the equivalent $QIO work may be done (routine write_qio).
 *
 *  This program recognizes the following options:
 *
 *  -n file-name   : specify the file name (default: same as EXE with DAT type)
 *  -b block-count : specify the size of the file in blocks (default: 10)
 *  -q             : use $QIO instead of Fast I/O
 *
 *  Compilation Note
 *  ----------------
 *
 *  Since this program uses some routines of the DEC C Run Time Library
 *  which are not part of the ISO C standard, the compilation must use
 *  the following option: /PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES or simply
 *  /PREFIX=ALL
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unixio.h>
#include <descrip.h>
#include <ssdef.h>
#include <stsdef.h>
#include <psldef.h>
#include <iodef.h>
#include <iosadef.h>
#include <rms.h>
#include <starlet.h>
#include <lib$routines.h>


/*
 *  Miscellaneous OpenVMS constants.
 */

#define PAGELET_SIZE    512
#define DISK_BLOCK_SIZE 512
#define NO_EFN          128


/*
 *  OpenVMS status codes.
 */

typedef unsigned long Status;
#define Success(status) ((status) & STS$M_SUCCESS)
#define Failure(status) (~(status) & STS$M_SUCCESS)


/*
 *  OpenVMS I/O types.
 */

typedef unsigned short Channel;    /* I/O channel */
typedef unsigned __int64 Bufobj;   /* Buffer Object handle */
typedef unsigned __int64 Fandle;   /* File handle */

typedef struct {                   /* I/O Status Block */
    unsigned short status;
    unsigned short count;
    unsigned long info;
} Iosb;


/*
 *  This routine is a very basic error handler for OpenVMS routines. In case of
 *  error, it simply terminates the application with the appropriate status.
 */

static void Vcheck (Status status)
{
    if (Failure (status))
        sys$exit (status);
}


/*
 *  This routine allocates a number of CPU-specific pages.
 *  These pages will contain at least 'size' bytes.
 *  Upon return, *start contains the starting virtual address of
 *  the first allocated page and *count contains the number of
 *  allocated pagelets (not pages). When no longer used, the
 *  pagelets must be freed using lib$free_vm_page.
 *
 *  This routine is used to find space for buffer objects because
 *  they must be aligned on a CPU-specific page.
 */

static Status get_pages (size_t size, void** start, unsigned long* count)
{
    Status status;
    const unsigned long page_size = getpagesize ();
    const unsigned long pagelets_per_page = page_size / PAGELET_SIZE;
    unsigned long page_count;
    unsigned long free_count;
    char* base;
    char* used_base;

    if (size <= 0)
        return SS$_BADPARAM;

    /* Compute the required number of CPU-specific pages */

    page_count = (size + page_size - 1) / page_size;

    /* Compute the number of pagelets to allocate to ensure that the
     * required number of CPU-specific pages are there. */

    *count = (page_count + 1) * pagelets_per_page - 1;

    /* Allocate the pagelets */

    status = lib$get_vm_page (count, &base);

    if (Failure (status))
        return status;

    /* Compute the start of the first CPU-specific page inside the area */

    used_base = (char*)(page_size * ((long)(base + page_size - 1) / page_size));
    *start = used_base;

    /* Free unused pagelets before the first CPU-specific page */

    if ((free_count = (used_base - base) / PAGELET_SIZE) > 0) {
        lib$free_vm_page (&free_count, &base);
        *count -= free_count;
    }

    /* Free the unused pagelets after the last CPU-specific page */

    if ((free_count = *count - page_count * pagelets_per_page) > 0) {
        base = used_base + page_count * page_size;
        lib$free_vm_page (&free_count, &base);
        *count -= free_count;
    }

    return SS$_NORMAL;
}


/*
 *  This routine fills the file using the traditional $QIO interface.
 *  See routine write_fastio for a Fast I/O equivalent.
 */

static void write_qio (Channel channel, int number_of_blocks)
{
    int block;
    Iosb iosb;
    unsigned char buffer [DISK_BLOCK_SIZE];

    printf ("Writing %d blocks using the traditional $QIO interface\n",
        number_of_blocks);

    for (block = 1; block <= number_of_blocks; block++) {

        /* Fill the buffer with a pattern (VBN modulo 256) */

        memset (buffer, block % 256, sizeof (buffer));

        /* Write a block on the disk */

        Vcheck (sys$qiow (NO_EFN, channel, IO$_WRITEVBLK, &iosb, 0, 0,
            buffer, sizeof (buffer), block, 0, 0, 0));

        Vcheck (iosb.status);
    }
}


/*
 *  This routine fills the file using the new Fast I/O interface.
 *  See routine write_qio for an $QIO equivalent.
 *
 *  The Fast I/O uses a data buffer and an IOSA buffer. The IOSA (I/O
 *  Status Area) is the Fast I/O equivalent of the IOSB. The data buffer
 *  and the the IOSA must be located inside a Buffer Object. You can use
 *  the same Buffer Object or two different Buffer Objects.
 * 
 *  In this example, the two areas are grouped inside a user-defined
 *  data structure named MyFastIo. This structure will be allocated
 *  inside the same Buffer Object. For better performance, the data
 *  buffer is placed first and is consequently aligned on a page
 *  boundary. The required alignment for the IOSA is quadword (implicit
 *  in this example).
 */

static void write_fastio (Channel channel, int number_of_blocks)
{
    typedef struct {
        unsigned char data [DISK_BLOCK_SIZE];
        struct _iosa iosa;
    } MyFastIo;

    MyFastIo* io;
    Fandle fandle;
    Bufobj bufobj;
    int block;
    void* bufobj_range [2];
    void* bufobj_range_ret [2];
    unsigned long bufobj_pagelet_count;

    printf ("Writing %d blocks using the new Fast I/O interface\n",
        number_of_blocks);

    /* Create a buffer object for the data buffer and the IOSA. First,
     * we have to find a memory area which covers an exact number
     * of CPU-specific pages. */

    Vcheck (get_pages (sizeof (MyFastIo), (void**)&io, &bufobj_pagelet_count));

    /* Then, we may create a buffer object over this area */

    bufobj_range [0] = io;
    bufobj_range [1] = (char*)(io + 1) - 1;

    Vcheck (sys$create_bufobj (bufobj_range, bufobj_range_ret,
        PSL$C_USER, 0, &bufobj));

    /* Prepare the I/O structure. The same file handle (or fandle) will
     * be used on all subsequent I/O's. */

    Vcheck (sys$io_setup (IO$_WRITEVBLK, &bufobj, &bufobj, 0, 0, &fandle));

    /* Perform the Fast I/O on each block of the file */

    for (block = 1; block <= number_of_blocks; block++) {

        /* Fill the buffer with a pattern (VBN modulo 256) */

        memset (io->data, block % 256, sizeof (io->data));

        /* Write a block on the disk */

        Vcheck (sys$io_performw (fandle, channel, &io->iosa,
            io->data, sizeof (io->data), block));

        Vcheck (io->iosa.iosa$l_status);
    }

    /* Free all structures */

    Vcheck (sys$io_cleanup (fandle));
    Vcheck (sys$delete_bufobj (&bufobj));
    Vcheck (lib$free_vm_page (&bufobj_pagelet_count, &io));
}


/*
 *  Program entry point.
 */

int main (int argc, char** argv)
{
    int c;
    Channel channel;
    struct FAB fab = cc$rms_fab;

    /* Default values for options */

    int use_fastio = 1;
    int number_of_blocks = 10;
    char* file_spec = ".DAT;";

    /* Get the command line options. Note that getopt() is new in the
     * OpenVMS V7.0 C run time library. */

    while ((c = getopt (argc, argv, ":qn:b:")) != -1) {
        switch (c) {
            case 'q':
                use_fastio = 0;
                break;
            case 'n':
                file_spec = optarg;
                break;
            case 'b':
                if ((number_of_blocks = atoi (optarg)) <= 0) {
                    fprintf (stderr, "Invalid block count: %s\n", optarg);
                    exit (EXIT_FAILURE);
                }
                break;
            default:
                fprintf (stderr, "options: -n file-name -b block-count -q\n");
                exit (EXIT_FAILURE);
        }
    }

    /* Create a sequential file with fixed size record of 512 bytes.
     * Specify a "User File Open": the file file is only accessed using
     * ACP-QIO interface, returning a channel. No other RMS processing
     * will be done. */

    fab.fab$l_fna = file_spec;
    fab.fab$b_fns = strlen (file_spec);
    fab.fab$l_dna = argv [0];
    fab.fab$b_dns = strlen (argv [0]);
    fab.fab$l_alq = number_of_blocks;
    fab.fab$l_fop = FAB$M_MXV | FAB$M_UFO;
    fab.fab$b_org = FAB$C_SEQ;
    fab.fab$b_rfm = FAB$C_FIX;
    fab.fab$w_mrs = DISK_BLOCK_SIZE;

    Vcheck (sys$create (&fab));

    channel = fab.fab$l_stv;

    /* Writes the specified number of blocks using the selected method */

    if (use_fastio)
        write_fastio (channel, number_of_blocks);
    else
        write_qio (channel, number_of_blocks);

    /* Close the file */

    Vcheck (sys$dassgn (channel));
}
