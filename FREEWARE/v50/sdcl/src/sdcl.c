/* 
**  Copyright Dick Munroe, 1985-2000.
**  Rights to distribution and modification are granted as specified in the GNU
**  Public License.
**
** 	This file contains the main driver for sdcl preprocessor.
**
**	This program accepts as input a file written in sdcl.  This 
**	language is an enhanced version of DCL.  It includes however
**	the for, while, do while, if, if-else, next, and break. The syntax
**	for these statements is almost exactly as in C.  The ouput
**	of this program is a file that is standard DCL.  This language 
**	can be summarized in BNF as follows:
**
**		program		| statement
**				| program statement
**
**		statement	| if ( condition ) statement
**				| if ( condition ) statement else statement
**				| while ( condition ) statement
**				| for ( initialize; condition; reinitialize ) 
**					statement
**				| do statment while (condition)
**				| break
**				| next
**				| { program }
**				| other
**
**
**	All the main driver function does is perform an initial call to
**	the lexical scanner (to "prime the pump") and then repeatedly call
**	function statement().  This function calls one of a series of semantic
**	functions depending on what the first token on an input line is.
**	The execution ends when the EOF token is encountered.
**
**	The seven files that make up the sdcl processor are
**
**		sdcl.c		- main driver and non-terminal routines.
**		statements.c	- handles statements such as if, while, etc.
**		scan_lex.c	- does all scanning for input tokens.
**		output.c	- holds all code generation functions
**		stack.c		- provides stack and manipulation functions
**		defs.h		- token code and character class definitions.
**		tcodes.h	- nextstate and output tables for scanner.
*/

#include <stdio.h>
#include "defs.h"
#ifdef vax11c
#include descrip
#endif

extern int tokencode;
FILE *infile, *outfile;

main(argc, argv)
    int argc;
    char *argv[];
    {
	int extern_files = FALSE;
	int xeq_flag     = FALSE;
	extern char *make_out_file();
	extern FILE *efopen();
	extern int statement(int);
#ifdef vax11c
	char dcl_command[80];
	$DESCRIPTOR ( cmd, dcl_command ); /* struct for lib$docomman */
#endif
	/*
	 *	If the user has specified input and output files (signified
	 *	by argc equaling 3) open them for reading and writing.  Also
	 *	set a boolean indicating these files exist.  This boolean will 
	 *	be used at program end to close these files.
	 *
	 */
	infile = stdin;
	outfile = stdout;
	if (argc == 4 ){
		infile = efopen(argv[1], "r");
		outfile = efopen(argv[2], "w");
		argv[1] = argv[2];
		extern_files = TRUE;
		if (strcmp( argv[3], "-x" ) == 0 )
		    xeq_flag = TRUE;
	}
	else if (argc == 3 ){
		infile = efopen(argv[1], "r");
		if (strcmp( argv[2], "-x" ) == 0 ) {
		    xeq_flag = TRUE;
		    argv[1] = make_out_file( argv[1] );
		    outfile = efopen(argv[1], "w");
		}
		else{
		    outfile = efopen(argv[2], "w");
		    argv[1] = argv[2];
		}
		extern_files = TRUE;
	}
	else if (argc == 2){  /* infile only */
		if (strcmp( argv[1], "-x" ) != 0 ) {
		    infile = efopen(argv[1], "r");
		    argv[1] = make_out_file( argv[1] );
		    outfile = efopen(argv[1], "w");
		    extern_files = TRUE;
		}
		else
		    fprintf(stderr,"Cannot execute from sys$output\n");
	}
	/*
	 *	Keep going until EOF but first, get the
	 *	first nonblank token via a call to scan().
	 */
	scan();
	while( tokencode != FILEEND )
		statement(1);

	/* If the user specified input and output files close them. */
	if (extern_files) {
		fclose(infile);
		fclose(outfile);
	}
#ifdef vax11c
	if ( xeq_flag ){
		strcpy( dcl_command, "@" );
		strcat( dcl_command, argv[1] );
		exit ( lib$do_command ( &cmd ) );
	}
#endif
}

FILE *efopen( name, mode )
    char *name, *mode;
    {
	FILE *fp;
	fp = fopen( name, mode );
	if (fp == NULL ){
	    fprintf(stderr, "Trouble opening %s, abort\n", name );
	    exit( 1 );
	}
	return( fp );
    }

char *make_out_file ( s )
    char *s;
    /*
    make .com filename out of filename in s.
    */
    {
	int i;
	char *new;
	for (i = 0; s[i]; i++ )
	    if ( s[i] == '.' ){
		s[i] = '\0';
		break;
	    }
	new = malloc( strlen(s) + 6 );
	strcpy( new, s );
	strcat( new, ".com" );
	return ( new );
    }
