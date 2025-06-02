/*
	undigest - reformats a mailing list digest for use with "mail -f"

	Modified 1/27/93 to increase temp def from 81 to 1024 to allow
	for headers with long lines.  kbp

	Modified 12/5/92 to use the dashed break lines in the digest,
	rather than the "Date:" lines of the member messages.  This
	makes the program immune to differences in the order of the
	headers in the member messages.  It also prevents false breaks
	caused by messages included within member messages.  Renamed to
	undigest.c to avoid confusion with the digest creator.

	                            Keith B. Petersen
	                            w8sdz@TACOM-EMH1.Army.Mil

	Modified 11/16/90 to fix use of an uninitialized variable
	(could cause a run time error on some machines)
	and a variable set but not used to eliminate the warnings
	from the Apollo c compiler.  jdb

	Modified 1/27/88 to handle more types of digests and
	a compile-time option added (LONGNAME) which will cause
	the default output file name to be <digest-name>.VOL.NUM
	rather than the VOL.NUM form described below.

	This has been tested with the Info-IBM, Info-Kermit, and
	Info-CPM Digest formats.

	This program should be called 'UNDIGEST' rather than 'DIGEST'
	as it is below since there is a companion program in the
	SIMTEL20 PD6:<UNIX-C.MAIL> directory (digest.c) that
	creates a Digest file from individual messages.

	The original documentation below has NOT been modified to
	reflect these changes.

					David Brown
					jdb@email.ncsc.navy.mil
*/
/*
DIGEST:  (version 3) November, 1984

NAME: 
	Digest  - reformats the ailist digest for use with "mail -f"

SYNOPSIS:
	digest file [file] 

DESCRIPTION:

digest takes the file name  given  in  first  argument  and  places  the
reformatted  file  in  the  second argument, if given. If no output file
(2nd argument) is given, the output will be placed into a  default  file
whose  name  is of the form VOL.NUM where VOL and NUM are the volume and
number of the ailist digest fed in (e.g. 2.144,etc.).

A few notes:
	(1) if only one argument is given, it is assumed to be the
	    input file.  If no args are given, you get prompted for
	    for the input file, and the output is sent to the default
	    file construction.

	(2) This has been tested only for use with the specific ailist/human-
	    nets format now in use.  I will soon get around to adding
	    code to this program to handle other formats.  When I do,
	    I will send it along.

	(3) The input to this program must be A SINGLE AILIST 
	    OR HUMAN-NETS DIGEST.  If you have been stuffing all 
	    your ailist digests into a single file, running this pgm 
	    on that file will yield  incorrect and unpredictable results.  
	    The pgm is best  used to manage the incoming stuff.

	(4) the input file is left untouched (i.e. is not removed)

	(5) digest does not work with piped input (a bug, sorry).
	    This has meant for me that I stick the day's ailist digest
	    into a temp file when I receive it over "mail", and then
	    later "digest" this temp file to get it into a suitable
	    form for "mail -f".

BUGS:
	If there are ailist sub-entries which do not have a DATE:
	field in the header, they will be appended to the entry
	prior.  

Any questions, suggestions or problems, etc. should be sent to 

douglas stumberger
department of computer science
111 Cumington Street
boston, ma. 02215

csnet: des@bostonu
bitnet: csc10304@bostonu

*/

#include <stdio.h>

/* the following defines the exact length of the break lines in the digest,
   which are used to determine where the member messages begin and end. */

/* a line of 70 hyphens separates the Preamble from the rest of the digest. */
char *endhead = "----------------------------------------------------------------------\n";

/* a line of 30 hyphens separates the messages in the digest. */
char *keyline = "------------------------------\n";

main(argc,argv)
	int argc; char *argv[] ;
{
	FILE *fpr, *fpw ;
	char *lead, *fromline, temp[1024], fname[81] ,
		digest[81],vol[50],num[5] ;
	register int done=0, gl ;

	if (argc > 3) {
		printf("Usage: %s file [file]\n",argv[0]) ;
		exit(0);
	}
	if (argc == 1) { 
		printf("What file is the digest in? > ") ;
		scanf("%s",fname) ;
	}
	else 
		strcpy(fname,argv[1]) ;
	
	if ((fpr = fopen(fname,"r")) == NULL) {
		printf("%s: No such file\n",fname) ;
		exit(0) ;
	}

#ifdef DEBUG
	printf(" input file name is <%s>\n",fname) ;
#endif

	lead = (char *) calloc(90,sizeof(char)) ;  

	get_line(fpr,lead) ;		/* get the first line of file */

	fromline = (char *) malloc(strlen(lead)+1) ;
	strcpy(fromline,lead) ;

	if (argc != 3) {   /* no output file given - 
				find out vol/num for filename */

		while ((strcmp(lead,endhead)) && (!done)) {

#ifdef DEBUG
printf("Scanning:%s",lead);
#endif
			sscanf(lead,"%s %s",digest,temp) ;
			if (!strcmp(temp,"Digest")) {
#ifdef DEBUG
printf("\nFound a match\n");
#endif
	               sscanf(lead,"%*s %*s %*s %*s %*s %*s %*s %s %*c %*s %s",
		   		vol,num) ;
		   	done++ ;
			}
			get_line(fpr,lead) ;
		}

		strcat(digest,".") ;		
#ifndef LONGNAME
		digest[0]='\0';
#endif
		strcat(digest,vol) ;
		strcat(digest,".") ;
		strcat(digest,num) ;
	}
	else 
		strcpy(digest,argv[2]) ;	/* output filename is third argument */


#ifdef DEBUG
		printf("output file is <%s>",digest) ;
#endif

	fclose(fpr) ;			

#ifdef DEBUG
		printf(" input file is <%s>\n",fname) ;
#endif

	if ((fpr = fopen(fname,"r")) == NULL) {
		printf("\nERROR: File will not rewind\n") ;
		exit(0) ;
	}

	if ((fpw = fopen(digest,"w")) == NULL) {
		printf("\nERROR: Output File will not open\n") ;
		exit(0) ;
	}

	get_line(fpr,lead) ;		/* copy the ailist header */

	while (strcmp (lead,endhead)) {	/* i.e.  Law's message of the topics */
		fprintf(fpw,"%s",lead) ;
		get_line(fpr,lead) ;
	}

	fprintf(fpw,"%s",fromline) ;	/* add first break line after header */
	fflush(fpw) ;
	gl = get_line(fpr,lead) ;	/* gobble up blank line */
	gl = get_line(fpr,lead) ;	/* do the body of the digest */

	while (gl != EOF) {

		if (!strcmp (lead,keyline))
			{
			fprintf(fpw,"%s",fromline) ;
			gl = get_line(fpr,lead) ;
			gl = get_line(fpr,lead) ;
			}

		fprintf(fpw,"%s",lead) ;
		gl = get_line(fpr,lead) ;
	}

	printf("Re-formatted digest now in file <%s>\n",digest) ;
	}	


get_line (fp,s)
	FILE *fp;	char *s;
{
	register int c=0,i=0 ;

	while ((c != '\n') && (c != EOF)) {
		c = getc(fp) ;
		*(s+i++) = c ;
	}
	*(s+i) = '\0' ;
	return(c) ;
}
