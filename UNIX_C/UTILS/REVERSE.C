10-Sep-85 11:17:20-MDT,4446;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 10 Sep 85 11:17:09-MDT
Received: from usenet by TGR.BRL.ARPA id a025785; 10 Sep 85 5:50 EDT
From: K A Gluck <kurt@pyuxhh.uucp>
Newsgroups: net.sources
Subject: program to totaly reverse input
Message-ID: <873@pyuxhh.UUCP>
Date: 9 Sep 85 20:48:55 GMT
To:       unix-sources@BRL-TGR.ARPA

rellik gub
# Program to super reverse input
# 
# this program will completly reverse its input.
# (with one exception - the output will have an extra line feed
# at its beginning, and the final trainling line feed will be deleted)
# for example, the folowing was a copy of the previous run through 
# this program
#
#-------------------------------------------------------------------------
# margorp siht
#  hguorht nur suoiverp eht fo ypoc a saw gniwolof eht ,elpmaxe rof
# )deteled eb lliw deef enil gnilniart lanif eht dna ,gninnigeb sti ta
# deef enil artxe na evah lliw tuptuo eht - noitpecxe eno htiw(
# .tupni sti esrever yltelpmoc lliw margorp siht
#
# tupni esrever repus ot margorP
#
#-------------------------------------------------------------------------
# This is a shell archive.  Remove anything before this line, then
# unpack it by saving it in a file and typing "sh file".  (Files
# unpacked will be owned by you and have default permissions.)
#
# This archive contains:
# reverse.c reverse.man

echo x - reverse.c
cat > "reverse.c" << '//E*O*F reverse.c//'
#include <stdio.h>
#include <fcntl.h>
#define strlen 256
 
main(argc,argv)  /* rev- lists file. */
		/* argc - number of args*/
		/* argv - pointer to array of char strings of args*/
int argc;
char *argv[];
{
 int fp, open(), creat() , strcmp() ;
 char template[30], *mktemp() , *strcpy() ;
 char *tmp;
char *tmpname() ;
 long pos;
 int n;
 
 if( argc == 1 ) /* no args - halt */
    { printf("rev- no argument given\n");
      printf("usage-   rev file1 [{filen}]\n");
	printf(" alt:	rev -    reads from stdin (pipes etc...) \n");
      exit(1); }
 else
	while( --argc > 0 )
		if( (*++argv)[0] == '-' )
		{
			if( ( fp = open(( tmp = ( mktemp("/tmp/revXXXXXX\0")) ),O_RDWR | O_CREAT , 0755)) == -1 )
			{	printf("rev- cant open %s <<<<\n",*template);
			} else {
				filecopy(0,fp);
				filerev(fp) ;
				close(fp);
				unlink(tmp);
			}
		} else {
			if (( fp = open(*argv,0)) == -1 ) {
				printf("rev- cant open %s ###################\n",*argv);
			} else {
				filerev(fp) ;
				close(fp);
			}
		}
 }
 filecopy(fpi,fpo)	/* list a file			*/
 int fpi , fpo;		/* the file			*/
{ char c ;
 while ( ( read(fpi,&c,1) ) > 0 ) 
 	write(fpo,&c,1) ; 
 }
 filerev(fp)	/* list a file			*/
 int fp;		/* the file			*/
{ char c ;
  long p ;
	int flag;
 long lseek() ;
 int whence;
 p=0L ;
 whence=2;
 flag=1;
  while ( ( lseek(fp,p,whence ) ) != -1L )  {
	p=0L-2L;
	whence=1;
 	read(fp,&c,1) ; 
	if( flag==0 ) 	write(1,&c,1) ; 
	flag=0;
	}
	printf("\n");
 }
//E*O*F reverse.c//

echo x - reverse.man
cat > "reverse.man" << '//E*O*F reverse.man//'
.tr ~
.de Na
.br
\\$1
..
.de Ex
.IP "" 5
.B \\$1 \\$2 \$3 \\$4 \\$5 \\$6 \\$7 \\$8 \\$9
.P
..
.de Es
.IP "\\$1" 5
.B \\$2 \\$3 \\$4 \\$5 \\$6 \\$7 \\$8 \\$9 
..
.de Ee
.br
.B \\$1 \\$2 \\$3 \\$4 \\$5 \\$6 \\$7 \\$8 \\$9 
..
.de Ez
.P
..
.tr ~
.TH reverse lman "Pics System Test"
.SH NAME
reverse - c program to reverse files.
.SH SYNOPSIS
.Na "reverse files"
.SH DESCRIPTION
.B reverse
is a filter to reverse files.
It is used in a manner similar to cat (see cat(1)).
.B reverse
will process each of its arguments in turn, reading the files backwards.
If any file has 
.B -
for its name, 
.B reverse
will create a temporary file, read standard input into that temporary file
and then reverse the temporary file.
.SH EXAMPLES
.Es "To reverse input from terminal" reverse -
.Ez
.Es "To create a confusing listing" ps -ef |reverse -
.Ez
.Es  "To reverse the date" date |reverse -
.Ez
.Es "To reverse a file" who >file
.Ee reverse file
.Ez
.SH "SEE ALSO"
cat(1)
.SH AUTHOR
Kurt Gluck, SPL 1c273a x2023
//E*O*F reverse.man//

exit 0
-- 
Kurt Gluck                              (201)-561-7100 x2023
SPL 1c273a                              ihnp4!rruxg!kurt
Bell Communications Research Inc        ihnp4!pyuxhh!kurt
50 Cragwood Road
South Plainfield, NJ 07080              
