/***********************************************************************
Convert a stream format file to fixed binary, supplying trailing NUL's
for padding if necessary.

Usage:
  	cvt infile outfile
[22-Aug-87]
***********************************************************************/

#define PAD '\0'
#define FAILURE ((1 << 28) + 2)
#define SUCCESS 1

#include <stdio.h>

main(argc,argv)
int argc;
char* argv[];
{
	FILE* in;
	FILE* out;
	int c;
	long count;

	if (argc != 3)
	  {
	    fprintf(stderr,"Usage: cvf infile outfile\n");
	    exit(FAILURE);
	  }

	in = fopen(argv[1],"rb","rfm=stm","ctx=stm");
	if (in == (FILE*)NULL)
	  {
	    fprintf(stderr,"?Cannot open [%s] for input\n",argv[1]);
	    exit(FAILURE);
	  }

	out = fopen(argv[2],"wb","rfm=fix","bls=512","mrs=512");
	if (out == (FILE*)NULL)
	  {
	    fprintf(stderr,"?Cannot open [%s] for output\n",argv[1]);
	    exit(FAILURE);
	  }

	count = 0;
	while ((c = getc(in)) != EOF)
	  {
	    putc(c,out);
	    count++;
	  }
	fprintf(stderr,"File size = %ld\n",count);
	/* Now supply trailing padding to ensure output is a multiple of
	   512 characters; otherwise, RMS will not close the output file
	   correctly. */
	count %= 512;
	if (count > 0)
	    for (; count < 512; ++count)
	        putc(PAD,out);
	if (fclose(in) == EOF)
	  {
		fprintf(stderr,"?Close failure on [%s]\n",argv[1]);
		exit(FAILURE);
	  }
	if (fclose(out) == EOF)
	  {
		fprintf(stderr,"?Close failure on [%s]\n",argv[2]);
		exit(FAILURE);
	  }
	exit(SUCCESS);
}

