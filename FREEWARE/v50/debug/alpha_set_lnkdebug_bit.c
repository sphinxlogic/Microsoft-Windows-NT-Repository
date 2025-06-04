/*  This tool creates a copy of an Alpha VMS image, setting the LNKDEBUG    */
/*  bit in the image header.						    */
/*									    */
/*  P1 is the file spec of the original file.				    */
/*  P2 is the file spec of the copy of the file with the LNKDEBUG bit set.  */
/*  									    */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define buffer_length 512

FILE	    *ifile;
FILE	    *ofile;

char	    iline[ buffer_length ];
char	    oline[ buffer_length ];

char ifile_spec[ buffer_length ];
char ofile_spec[ buffer_length ];

int main(int argc, char *argv[])
{
    int n;

    if (argc != 3)
	{
	printf( "specify two arguments" );
	return 1;
	}

    ifile = fopen( argv[1], "r" );
    if (ifile == NULL)
	{
	printf( "cannot open %s\n", argv[1] );
	return 1;
	}
    
    ofile = fopen( argv[2], "w" );
    if (ofile == NULL)
	{
	printf( "cannot open %s\n", argv[2] );
	return 1;
	}

    if (n = fread( iline, buffer_length, 1, ifile  ) > 0)
	{
        iline[ 80 ] = iline[ 80 ] | 1;
	fwrite( iline, buffer_length, 1, ofile );
	}
	
    while (n = fread( iline, buffer_length, 1, ifile  ) > 0)
	{
	fwrite( iline, buffer_length, 1, ofile );
	}
	
    return 0;
}
