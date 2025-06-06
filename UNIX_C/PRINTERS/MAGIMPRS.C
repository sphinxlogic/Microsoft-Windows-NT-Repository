12-Jan-86 08:58:08-MST,8176;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sun 12 Jan 86 08:57:59-MST
Received: from usenet by TGR.BRL.ARPA id a021853; 11 Jan 86 16:34 EST
From: Geoffrey Cooper <geof@imagen.uucp>
Newsgroups: net.sources
Subject: Pgm to manip. IMPRESS bitmaps - good for screen dumps
Message-ID: <173@imagen.UUCP>
Date: 8 Jan 86 21:22:45 GMT
To:       unix-sources@BRL-TGR.ARPA

/*
 * Magbitmap.c - Program to change the magnification of an IMPRESS bitmap.
 * 
 * This program searches the argument file for IMPRESS magnification or
 * bitmap commands.  It interprets the commands and produces an output
 * impress file which contains a compatible magnification command and
 * a set of bitmap commands that divide the bitmap across multiple pages
 * of output, as necessary.
 * 
 * An argument allows the magnification of the bitmap to be changed.  In
 * this mode, the program makes it possible to scale a bitmap to a size
 * larger than 8.5X11 inches.  One use of the program is to scale screen
 * dumps to magnification level 2 (four times the original size).
 * 
 * The program was written for Apollo DOMAIN/IX 4.2 emulation.  It relies
 * only on the stdio package, and should run under any C runtime system.
 * The input file is scanned in multiple passes.  This is a feature,
 * since it allows the program to run on machines with a small address
 * space.
 * 
 * Written January 8, 1985 by Geoffrey H. Cooper of IMAGEN
 * 
 * Copyright (C) 1985, IMAGEN Corporation
 * This software may be copied, modified, and put to any non-commercial
 * use so long as the copyright notice is preserved.
 *
 * Bugs: - Should be interpreting impress to find the bitmap
 *       - Will only find one bitmap in a file - not self recursive
 *       - Should allow for cropping of the bitmap on patch boundaries
 */

#include <stdio.h>

int columns, rows;              /* # columns and rows in the bitmap */
int magnification;              /* bitmap magnification */
int operation;                  /* bitmap operation */
long bitmap_pos;                /* fseek position of bitmap */
int ask = 1;                    /* interactive mode if -1 */
int overlap = 1;                /* # patches of overlap, default = 1 */

main( argc, argv )
    int argc;
    char **argv;
{
    char *ms;
    char **av = argv;
    FILE *f, *outf;
    char *opt;
    int mag = -1;

    while ( argv[1] && argv[1][0] == '-' ) {
        opt = &argv[1][1];
        while ( *opt ) {
            switch ( *opt ) {
              case 'i':
                ask = -1;
                break;

              case 'n':
                ask = 0;
                break;

              case 'm':
                if ( opt[1] )
                    mag = atoi(opt+1);
                else {
                        argv++;
                        argc--;
                        mag = atoi(argv[1]);
                }
                goto nextarg;

              case 'o':
                if ( opt[1] )
                    overlap = atoi(opt+1);
                else {
                        argv++;
                        argc--;
                        overlap = atoi(argv[1]);
                }
                goto nextarg;

              default:
                printf("unknown flag '%c'\n", *opt);
                goto usage;
            }
            opt++;
        }
nextarg:
        argc--; argv++;
    }

    if ( ! ((argc == 3) || (ask == 0 && argc == 2)) ) {
usage:  printf("usage: %s [-n -i -m magnification -o overlap_patches] input_fname output_fname\n", av[0]);
        exit(1);
    }
    if ( mag >= 0 ) 
        printf("Forcing magnification to %d\n", mag);
    f = fopen(argv[1], "r+");
    if ( f == NULL ) {
        perror( argv[1] );
        exit(2);
    }
    if ( ask == 0 ) argv[2] = "/dev/null";
    outf = fopen(argv[2], "w");
    if ( outf == NULL ) {
        perror( argv[2] );
        exit(3);
    }
    ScaleBM(f, outf, mag);
    fclose(f);
    fclose(outf);
    exit(0);
}

ScaleBM(inf, outf, use_magnification)
    FILE *inf, *outf;
{
    register c, i, j, r;
    int columnsPerPage, rowsPerPage;
    int page_num;
    float inches;

    magnification = 1;
    i = 0;
    for (;;) {
        /* Find the impress magnification command */
        while ( (c = getc(inf)) != EOF && c != 236 && c != 235 );
        switch (c) {
          case EOF:
            printf("Early EOF\n");
            return;
          case 236:
            magnification = getc(inf);
            break;
          case 235:
            operation = getc(inf);
            columns = getc(inf);
            rows = getc(inf);
            goto break2;
        }
    }
break2:
    bitmap_pos = ftell(inf);
    printf("Bitmap command found at byte %d.\n", ftell(inf));
    printf("Operation: %d, Horizontal patches: %d, Vertical Patches: %d\n",
            operation, columns, rows);

    if ( use_magnification >= 0 )
        magnification = use_magnification;

    columnsPerPage = 70 >> magnification;
    rowsPerPage =    94 >> magnification;

    Setup(outf);
    page_num = 1;
    for ( i = 0; i < rows; i += rowsPerPage ) {
        r = rows - i;
        if ( r > rowsPerPage ) r = rowsPerPage + overlap;
        for ( j = 0; j < columns; j += columnsPerPage ) {
            c = columns - j;
            if ( c > columnsPerPage ) c = columnsPerPage + overlap;
            printf("Page %d, patches (%d,%d) -> (%d,%d)\n",
                    page_num++, j, i, j+c, i+r);
            if ( Confirm("Produce page") )
                PadBitmap(inf, outf, j, c, i, r);
        }
    }
    putc(255, outf);    /* EOF */
    fflush(outf);
}

PadBitmap(inf, outf, start_h, nh, start_v, nv)
    FILE *inf, *outf;
    int start_h, nh, start_v, nv;
{
    register int h, v, i, c;
    long seek;

    Bitmap(outf, nh, nv);

    for ( v = 0; v < nv; v++ ) {
        seek = bitmap_pos + 
                ((long)(v + start_v) * columns * (32L*32L/8L)) +
                  ((long)(start_h * (32*32/8)));
        fseek(inf, seek, 0);
        for ( h = 0; h < nh; h++ )
            for ( i = 0; i < (32*32/8); i++ ) {
                if ( (c = getc(inf)) == EOF ) c = 0;
                putc(c, outf);
            }
    }
    putc(219, outf);    /* ENDPAGE */
}


Setup(f)
    FILE *f;
{
    fprintf (f, "@document(language impress)");
}

Confirm(s)
    char *s;
{
    int c, x;

    if ( ask != -1 ) return ( ask );
    printf("%s? [yngq] ", s);
    for (;;) {
        c = getchar();
        for ( x = c; x != '\n'; x = getchar() );
        switch (c) {
          case 'Y':   case 'y':
          case '\r':  case '\n':
            return ( 1 );

          case 'n':   case 'N':
            return ( 0 );

          case 'g':
            ask = 1;
            printf("OK, will assume YES from now on.\n");
            return ( 1 );

          case 'q':
            ask = 0;
            printf("OK, will assume NO from now on.\n");
            return ( 0 );

          default:
            printf("You must specify yes (y), no (n), quit (q), or go (g).\n");
        }
    }
}

Bitmap(f, h, v)
    FILE *f;
{
    putc(213, f);       /* PAGE message- sets up parameters for Impress pg. */

    putc(135, f);       /* SET-ABS-H: horiz. offset in points-req. 2 bytes */
    putc(00, f) ;           /*offset currently set to 150/300 of in. in */
    putc(150, f);           /*consideration of printer's margin error.  */

    putc(137, f);       /* SET-ABS-V: horiz. offset in points-req. 2 bytes */
    putc(00, f) ;
    putc(150, f);           /* offset set to 150/300 of an in. */

    putc(236, f);      /* Magnify following bitmap by power of 2 */
    putc(magnification, f);

    putc(235, f);       /* Impress code for BITMAP */
    putc(operation, f); /* operation-type = 'OR' mapping of bits */
    putc(h, f);         /* HPATCHES = no. of patches horizontally */
    putc(v, f);         /* no. of vertical lines of HPATCHES */
}
