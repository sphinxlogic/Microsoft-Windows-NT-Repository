
/************************************************************************/
/* 		Program to convert Wordstar Document-Mode		*/
/*	    files to plain-vanilla, non-document, ascii format.		*/
/*									*/
/*	by Paul Homchick, One Radnor Station #300, Radnor, PA 19087	*/
/* picked up by Richard McGee, BRL, from the SIMTEL archives under      */
/*                  micro:<cpm.txtutl>unsoft1c.c                        */
/* and severely edited to compile under UNIX and to delete extra CR's.  */
/* last edited  on  September 24, 1984                                  */
/* To Compile:   cc -o unsoft -O unsoft.c                               */
/************************************************************************/

#include	<stdio.h>
#define CR	0x0d
#define LF	0x0a
#define FF	0x0c		/* form feed ()			*/
#define TAB	0x09
#define CNTRL_O 0x0f		/* non-break space			*/
#define END_HYP 0x1f		/* soft hyphen at end of line		*/
#define TRUE	1
#define FALSE	0
#define BIGLINE	512		/* number of char in biggest line	*/

int startline;
FILE *inptr, *outptr, *fopen();
main(argc,argv)
int argc; char *argv[];
{
	int c;
	startline= TRUE;
	switch (argc)
	{
		case 1:
			usage();
			exit(0);
		case 2:
			if ( (!strcmp(argv[1],"h")) ||
			     (!strcmp(argv[1],"H")) )
				help();
			else
			{
				usage();
				error("Not enough arguments on command line.");
				exit(0);
			}
		case 3:
			break;
		default:
			usage();
			error("Too many arguments on command line.");
			exit(0);
	}

	if (strcmp(argv[1],argv[2])==0)
	{
		printf("Input and output filenames must differ.");
		printf("  Aborting...\007\n");
		exit(0);
	}


	if ((inptr= fopen(argv[1],"r")) == NULL)
	{
		printf("Can't open '%s' for input.\n",argv[1]);
		exit(0);
	}

	if ((outptr=fopen(argv[2],"w")) == NULL)

	{
		printf("Can't open '%s' for output.",argv[2]);
		exit(0);
	}

/************************************************************************/
/*			 	main loop				*/
/************************************************************************/

	printf("processing... ");
	while ((c=getc(inptr))!=EOF)
		putc(translate(c),outptr);
	printf("done.\n");
}

/************************************************************************/
/*			wordstar translation routine			*/
/************************************************************************/

translate(c)
int c;
{
char	buf[BIGLINE];

	c&= 0x7f;			/* strip high bit		*/
	if (startline)
	{
		switch (c)
		{
			case '.':	/* process dot commands		*/
				fgets(buf,BIGLINE,inptr);
					/* .pa to form feed		*/
				if ((cmdeq(buf,"PA")) || (cmdeq(buf,"pa")) )
					fputc(FF,outptr);
				startline= TRUE;
				return(translate(c= fgetc(inptr)));
			case LF:
				return(c);
			default:
				startline= FALSE;
		}
	}
	if (c < ' ')			/* check for control character	*/
	{
		switch (c)
		{
			case END_HYP:
				return('-');
			case CNTRL_O:
				return(' ');
			case LF:
			case FF:
			case TAB:
				return(c);
			case CR:
				startline= TRUE;
				return(NULL);
			default:
				c= fgetc(inptr);
				return(translate(c));
		}	
	}
	else
		return(c);
}

/************************************************************************/
/*			short usage prompt routine			*/
/************************************************************************/

usage()
{
	printf("usage: unsoft wordstar_input_name ascii_output_name\n");
	printf("   or: unsoft (h | H) for help.\n");
	printf("converts wordstar document-mode files ");
	printf("to plain text format.\n");
}

/************************************************************************/
/*			error print routine				*/
/************************************************************************/

error(s)
char *s;
{
	printf("\007Error: %s\n",s);
}

/************************************************************************/
/*			on-line program help routine			*/
/************************************************************************/

help()
{
	printf("\nUnsoft is a program to filter files prepared under Wordstar\n");
	printf("document mode.  Given a Wordstar document mode file as an\n");
	printf("input, unsoft will output a file having made the following\n");
	printf("transformations:\n\n");
	printf("\to High bits stripped from all characters.\n");
	printf("\to Converts '.pa' dot commands to form feed (^L).\n");
	printf("\to Removes all other dot command lines from file.\n");
	printf("\to Converts 'non-break-space' (^O) to a space.\n");
	printf("\to Converts soft hyphen at end of line (1F hex) to '-'.\n");
	printf("\to Passes through LF, FF, and TAB characters.\n");
	printf("\to Filters out all other control characters and CR.\n");
	printf("\nUsage: unsoft wordstar_input_name");
	printf(" ascii_output_name\n");
	printf("where (names) are input and output file names.  Unsoft h,\n");
	printf("or H will evoke this help message.\n");
	exit(0);
}

/************************************************************************/
/*			test input commands				*/
/************************************************************************/

cmdeq(s,p)
char *s, *p;
{
	while(*p)
		if(*s++ != *p++)
			return(FALSE);
	return(TRUE);
}
