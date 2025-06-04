#define DEBUG   0

	/*
	 *	TEXT2RTF.C
	 *
	 *	This program converts text files to Rich Text Format (RTF).
         */
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<errno.h>
#include        <descrip.h>

#define	TRUE	                1
#define	FALSE	                0

#define MAX_FILENAME_LENGTH     256

#define MAX_LINE_LENGTH         256
#define LINE_BUFFER_LENGTH      MAX_LINE_LENGTH + 2

#define NUL                     '\000'
#define LF                      '\012'
#define CR                      '\013'
#define FF                      '\014'


int     main( argc,
	      argv )

int	argc;

char	*argv[];

{
char	*pInputFileName		= NULL,
	*pOutputFileName	= NULL,
	*pFlags			= NULL;

FILE	*pInputFile		= NULL,
	*pOutputFile		= NULL;

char	TextBuffer[ LINE_BUFFER_LENGTH ];
char	RTFBuffer[ LINE_BUFFER_LENGTH ];

char	OutputFileName[ MAX_FILENAME_LENGTH ];

char	*pPeriod;

char	OutputChar;

static char	ParaTag[] = "\n\\par ";
static char	PageTag[] = "\n\\page";

int	NewPage			= TRUE;
int	Landscape		= TRUE;
int	Verbose			= FALSE;
int	DisplayHelp		= FALSE;

int	ReadCount = 0;

int     n;


	/*
	 *	Get command line parameters.
	 *
	 *	Valid parameters are:
	 *
	 *	Input File - must be specified before Output File.  If 
	 *		     extension is not specified, ".TEXT" and ".TXT" 
	 *		     will be attempted.
	 *	Output File - must be specified after Input File.  If 
	 *		      Output File is not specified, Output File 
	 *		      will be Input File with ".RTF" as extension.
	 *	Flags - can be anywhere, but must be preceeded by "/" or "-".
	 *
	 *		L	Landscape orientation
	 *		P	Portrait orientation
	 *		Q	Quiet mode
	 *		V	Verbose mode
	 *		?	Display help
	 */

/*
	for ( n = 0; n < argc; n++ )
	{
		printf( "Arg %d: %s\n",
			n,
			argv[ n ] );
	}
*/

	/*
	 *	argv[ 0 ] = Command Line, so start checking
	 *	parameters at argv[ 1 ]...
	 */

	for ( n = 1; n < argc; n++ )
	{
		if ( ( *argv[ n ] == '-' ) ||
		     ( *argv[ n ] == '/' ) )
		{
			if ( pFlags == NULL )
			{
				pFlags = argv[ n ];
			} else	{
					printf( "More Than 1 Set Of Flags Specified! Aborting...\n" );
					goto Exit;
				}
		} else	{
				if ( pInputFileName == NULL )
				{
					pInputFileName = argv[ n ];
				} else	{
						pOutputFileName = argv[ n ];
					}
			}
	}

	if ( pFlags != NULL )
	{
		n = 1;

		while ( pFlags[ n ] != NUL )
		{
			switch( pFlags[ n ] )
			{
				/*
				 * Upper & lower case cases 'cause VAXC doesn't
				 * have a strupr( str ) function...
				 */

				case 'l':
				case 'L':

					Landscape = TRUE;
					break;

				case 'p':
				case 'P':

					Landscape = FALSE;
					break;

				case 'v':
				case 'V':

					Verbose = TRUE;
					break;

				case 'q':
				case 'Q':

					Verbose = FALSE;
					break;

				case '?':

					DisplayHelp = TRUE;
					break;

				default:

					printf( "Unexpected Flag Character = %c\n",
						pFlags[ n ] );
			}

			n++;
		}
	}

	if ( ( DisplayHelp == TRUE ) ||
	     ( argc < 2 ) ||
	     ( argc > 4 ) )
	{
		printf( "TEXT2RTF Usage:\n" );
		printf( "text2 rtf inputfile.ext [outputfile.ext] [{- | /}PLQV?]\n\n" );
		printf( "    inputfile.ext must be specified\n" );
		printf( "    if outputfile.ext is NOT specified, it will default to inputfile.RTF\n" );
		printf( "    valid flags (introduced by \"-\" or \"/\") are:\n" );
		printf( "      P - portrait orientation (80 column)\n" );
		printf( "      L - landscape orientation (default - 132 column)\n" );
		printf( "      Q - quiet mode (default - doesn't display informational stuff)\n" );
		printf( "      V - verbose mode (displays informational stuff)\n" );
		printf( "      ? - displays this message\n\n" );

 		if ( ( argc < 2 ) ||
		     ( argc > 4 ) )
		{
			if ( argc < 2 )
			{
				printf( "Too Few Parameters Sepcified.  Aborting...\n" );
			}

			if ( argc > 4 )
			{
				printf( "Too Many Parameters Sepcified.  Aborting...\n" );
			}

			goto Exit;
		}
	}

	if ( ( argc == 2 ) &&
	     ( DisplayHelp == TRUE ) )
	{
		goto Exit;
	}

	if ( pInputFileName == NULL )
	{
		printf( "Input FileName Must Be Specified! Aborting...\n" );
		goto Exit;
	}

	if ( pOutputFileName == NULL )
	{
		strcpy( OutputFileName,
			pInputFileName );

		pOutputFileName = OutputFileName;

		pPeriod = strrchr( pOutputFileName,
				   '.' );

		if ( pPeriod != NULL )
		{
			strcpy( pPeriod,
				".RTF" );
		} else	{
				strcat( pOutputFileName,
					".RTF" );
			}
	}

	if ( Verbose == TRUE )
	{
		printf( "InputFileName = %s\n",
			pInputFileName );
		printf( "OutputFileName = %s\n",
			pOutputFileName );

		if ( Landscape == TRUE )
		{
			printf( "Landscape Orientation\n" );
		} else	{
				printf( "Portrait Orientation\n" );
			}

		printf( "\n" );
	}

        /*
         *      Open input file.
         */
         
        if ( (pInputFile = fopen( pInputFileName, "r" )) == NULL )
        {
                perror( "Error opening input file.\n" );
		goto Exit;
        }

        /*
         *      Open output file.
         */
         
        if ( (pOutputFile = fopen( pOutputFileName, "w" )) == NULL )
        {
                perror( "Error opening output file.\n" );
		goto Exit;
        }

	/*
	 *	Write RTF prologue...
	 *
	 *	This proloque was developed by creating a document, in landscape and portrait
	 *	orientation, and using the resulting RTF.  Not sure if everything is needed,
	 *	but better safe than sorry 'cause I don't know exactly what all this stuff does...
	 */

	/*
	 *	RTF header & Font Table...
	 */

	strcpy( RTFBuffer,
		"{\\rtf1\\ansi \\deff4\\deflang1033{\\fonttbl{\\f3\\fmodern\\fcharset0\\fprq1 Courier;}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\f4\\froman\\fcharset0\\fprq2 Times New Roman;}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	

	/*
	 *	Color Table...
	 */

	strcpy( RTFBuffer,
		"{\\colortbl;\\red0\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green255\\blue255;\\red0\\green255\\blue0;\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"\\red255\\green0\\blue255;\\red255\\green0\\blue0;\\red255\\green255\\blue0;\\red255\\green255\\blue255;\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"\\red0\\green0\\blue128;\\red0\\green128\\blue128;\\red0\\green128\\blue0;\\red128\\green0\\blue128;\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"\\red128\\green0\\blue0;\\red128\\green128\\blue0;\\red128\\green128\\blue128;\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"\\red192\\green192\\blue192;}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	

	/*
	 * Style stuff...
	 */

	strcpy( RTFBuffer,
		"{\\stylesheet{\\widctlpar \\f4\\fs20 \\snext0 Normal;}{\\*\\cs10 \\additive Default Paragraph Font;}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	/*
	 *	Document information - slightly edited to get rid of my name...
	 */

	strcpy( RTFBuffer,
		"{\\info{\\author TEXT2RTF}{\\operator Text To RTF Converter}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\creatim\\yr1996\\mo8\\dy21\\hr10\\min7}{\\revtim\\yr1996\\mo8\\dy21\\hr10\\min13}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\version1}{\\edmins6}{\\nofpages1}{\\nofwords20}{\\nofchars118}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\company Digital Equipment of Canada}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\vern57431}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );

	/*
	 *	Paper dimensions and orientation...
	 */

	if ( Landscape == TRUE )
	{
		strcpy( RTFBuffer,
			"\\paperw15840\\paperh12240" );
		fputs( RTFBuffer,
		       pOutputFile );
	}
	
	strcpy( RTFBuffer,
		"\\margl360\\margr360\\margt360\\margb360 \\widowctrl\\ftnbj\\aenddoc" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	if ( Landscape == FALSE )
	{
		strcpy( RTFBuffer,
			"\\hyphcaps0" );
		fputs( RTFBuffer,
		       pOutputFile );
	}
	
	strcpy( RTFBuffer,
		"\\formshade \\fet0\\sectd \n" );
	fputs( RTFBuffer,
	       pOutputFile );

	if ( Landscape == TRUE )
	{
		strcpy( RTFBuffer,
			"\\lndscpsxn" );
		fputs( RTFBuffer,
		       pOutputFile );
	}

	strcpy( RTFBuffer,
		"\\psz1\\linex0\\headery0\\footery0\\colsx709\\endnhere \n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	/*
	 * Sections stuff...
	 */

	strcpy( RTFBuffer,
		"{\\*\\pnseclvl1\\pnucrm\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl2\\pnucltr\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl3\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl4\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxta )}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl5\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl6\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl7\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl8\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
	strcpy( RTFBuffer,
		"{\\*\\pnseclvl9\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	

	/*
	 *	Almost ready to do RTF...
	 */

	strcpy( RTFBuffer,
		"\\pard\\plain \\widctlpar \\f4\\fs20 " );
	fputs( RTFBuffer,
	       pOutputFile );
	
	/*
	 *	Set font & font size...
	 */

	if ( Landscape == TRUE )
	{
		strcpy( RTFBuffer,
			"{\\f3\\fs18 " );
		fputs( RTFBuffer,
		       pOutputFile );
	} else	{
			strcpy( RTFBuffer,
				"{\\f3\\fs24 " );
			fputs( RTFBuffer,
			       pOutputFile );
		}
	
	/*
	 *	Everything from here should be in a fixed font, sized
	 *	appropriately for page orientation.
	 */

	while ( feof( pInputFile ) == 0 )
	{
		fgets( TextBuffer,
		       LINE_BUFFER_LENGTH,
		       pInputFile );

		ReadCount = strlen( TextBuffer );

		if ( ReadCount > 0 )
		{
/*
			printf( "%d - %s",
				ReadCount,
				TextBuffer );
*/

			for ( n = 0; n < ReadCount; n++ )
			{
				OutputChar = TextBuffer[ n ];

/*
				printf( "%c",
					TextBuffer[ n ] );
*/

				switch( OutputChar )
				{
					case FF:

/*
						printf( "FF\n" );
*/

						fputs( PageTag,
						       pOutputFile );

						NewPage = TRUE;

						break;

					case LF:

/*
						printf( "LF\n" );
*/

						if ( NewPage == FALSE )
						{
							fputs( ParaTag,
							       pOutputFile );
						} else	{
								fputc( OutputChar,
								       pOutputFile );
							}

						NewPage	= FALSE;

						break;

					default:

/*
						printf( "Not FF or LF\n" );
*/

						if ( NewPage == TRUE )
						{
							fputc( LF,
							       pOutputFile );
						}

						fputc( OutputChar,
						       pOutputFile );

						NewPage	= FALSE;
				}
			}
		}
	}

	/*
	 *	Write RTF epilogue...
	 */

	strcpy( RTFBuffer,
		"}}\n" );
	fputs( RTFBuffer,
	       pOutputFile );
	
Exit:

	if ( pInputFile != NULL )
	{
		fclose( pInputFile );
	}

	if ( pOutputFile != NULL )
	{
		fclose( pOutputFile );
	}

	return( 1 );
}
