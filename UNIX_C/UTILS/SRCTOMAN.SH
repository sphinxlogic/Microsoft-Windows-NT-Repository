: srctoman - see comment below

: process arguments

while :
do
    case $1 in
 [0-9]) SECT=$1;;
     -) LANG=$1; B='[#:]';;
  -awk) LANG=$1; B='#';;
    -c) LANG=$1; B='\/\*';;
    -f) LANG=$1; B='[Cc]';;
   -mk) LANG=$1; B='#';;
 -n|-t) LANG=$1; B='\\"';;
    -p) LANG=$1; B='{';;
    -r) LANG=$1; B='#';;
    -C) LANG=$1; B=$2; shift;;
    -*) ERROR="unknown option: $1"; break;;
    "") ERROR="missing file argument"; break;;
     *) break;;
    esac
    shift
done

: check error status

case $ERROR in
"") ;;
 *) echo "$0: $ERROR" 1>&2
    echo "usage: $0 [-|-awk|-c|-f|-mk|-n|-p|-t|-r] [section] file(s)" 1>&2; exit 1;;
esac

: set up for file suffix processing

case $LANG in
"") sh='[:#]';	r='#';	rh=$r;	awk='#'; mk='#';
    c='\/\*';	h=$c;	y=$c;	l=$c;
    f='[Cc]';	fh=$f;	p='{';	ph=$p;
    ms='\\"';	nr=$ms;	mn=$ms;	man=$ms;
esac

: extract comments

for i in $*
do
    case $LANG in
    "") eval B\="\$`expr $i : '^.*\.\([^.]*\)$'`"
	test "$B" || { echo "$0: unknown suffix: $i; assuming c" 1>&2; B=$c; }
    esac
    sed '
    /^'"$B"'++/,/^'"$B"'--/!d
    /^'"$B"'++/d
    /^'"$B"'--/d
    s/[ 	]*$//
    /^'"$B"' \([A-Z]\)/{
	s//\1/
	/^NAME/{
	    N
	    s/^.*\n'"$B"'[ 	]*//
	    h
	    y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/
	    s/^.*$/.TH & '"$SECT"'\
.ad\
.fi\
.SH NAME/
	    p
	    g
	    s/ [0-9]$//
	    a\
\\-
	    p
	    d
	}
	/^SUMMARY/d
	/^DESCRIPTION/s//.SH &\
.ad\
.fi/
	/^BUGS/s//.SH &\
.ad\
.fi/
	/^DIAGNOSTICS/s//.SH &\
.ad\
.fi/
	/^[A-Z][A-Z][A-Z][^a-z]*$/s//.SH &\
.na\
.nf/
	p
	d
    }
    s/^'"$B"'[ 	]*//
    s/^[ 	]*$//
' $i
done 

exit

:++
: NAME
:	srctoman 1
: SUMMARY
:	extract manual page from source file comment
: PACKAGE
:	source file maintentance tools
: SYNOPSIS
:	srctoman [-|-awk|-c|-f|-mk|-m|-n|-p|-t|-r] [section] file(s)
: DESCRIPTION
:	Srctoman converts comments in various programming languages to
:	UNIX-style manual pages.
:	The command processes comments in the style of newsource(1);
:	its standard output is suitable for formatting with nroff(1) or 
:	troff(1) using the "-man" macro package.  
:	Typically, srctoman is integrated with make(1) scripts.
:
:	Source files are processed in the indicated order; if no
:	files argument the command produces no output.
:
:	The source file language can be specified through a command-line
:	option, or can be implied by the filename suffix.
:	The expected start-of-comment symbol is shown in the last column.
:
: .nf
	option	language	comment

	-	shell		[:#]
	-awk	awk		#
	-c	c		/*
	-f	fortran		[Cc]
	-mk	make		#
	-n	nroff		\\"
	-p	pascal		{
	-t	troff		\\"
	-r	ratfor		#
	-C	any language	next argument
: .fi
:
: .nf
	suffix	language	comment

	.awk	awk		#
	.c	c		/*
	.f	fortran		[Cc]
	.fh	fortran		[Cc]
	.h	c		/*
	.l	lex		/*
	.man	nroff,troff	\\"
	.mk	make		#
	.me	nroff,troff	\\"
	.ms	nroff,troff	\\"
	.nr	nroff,troff	\\"
	.p	pascal		{
	.ph	pascal		{
	.r	ratfor		#
	.rh	ratfor		#
	.sh	shell		[:#]
	.y	yacc		/*
: .fi
:
:	The required format of comments is discussed below, where SOC
:	stands for the start-of-comment symbol of the language being used.
:
:	1) Start of manual: SOC, followed by `++'.
:
:	2) Section heading: SOC, blank, section name in upper case.
:
:	3) New paragraph: empty line or line with SOC only.
:
:	4) All other text: SOC and subsequent blanks or tabs are removed.
:	Lines that do not start with SOC are left unchanged (useful for 
:	inclusion of program text).
:
:	5) End of manual: SOC, followed by `--'.
:	An end-of-comment may follow if the source file language requires this.
:
:	The following manual sections receive a special treatment:
:	NAME and SUMMARY should appear at the beginning and in
:	this order; DESCRIPTION, DIAGNOSTICS and BUGS will be
:	right-margin adjusted.
:	Other sections may be added freely without confusing srctoman.
: COMMANDS
:	sh(1), sed(1), expr(1)
: SEE ALSO
:	newsource(1), modsource(1), xman(1)
:	The earlier commands new(1), mod(1), mkman(1) and dssman(1)
:	by Ruud Zwart and Ben Noordzij (Erasmus University, Rotterdam) 
: DIAGNOSTICS
:	The program complains if an unknown language is specified
:	or if the language cannot be deduced from the file suffix.
: AUTHOR(S)
:	W.Z. Venema
:	Eindhoven University of Technology
:	Department of Mathematics and Computer Science
:	Den Dolech 2, P.O. Box 513, 5600 MB Eindhoven, The Netherlands
: CREATION DATE
:	Fri Jan 17 22:59:27 MET 1986
: LAST MODIFICATION
:	Thu Mar 10 20:08:15 MET 1988
: VERSION/RELEASE
:	1.20
:--


