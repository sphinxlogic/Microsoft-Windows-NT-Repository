From evp@lewey.AIT.COM Sat Jan 24 00:27:42 1987
Path: beno!seismo!lll-lcc!pyramid!voder!lewey!evp
From: evp@lewey.AIT.COM (Ed Post)
Newsgroups: net.sources
Subject: cal.c for Unix and MSDOS (with uuencoded .EXE)
Keywords: calendar
Message-ID: <435@lewey.AIT.COM>
Date: 24 Jan 87 05:27:42 GMT
Organization: American Information Technology, Cupertino, CA
Lines: 1052


After all the discussion about a "bug" in unix cal, I decided to post
this version.  It compiles both in the unix environment and under
MSDOS using Lattice C (with a little .ASM assist).  It folds years
between 13 and 99 into the twentieth century, and recognizes months
either by name or number.

Included is the source and uuencoded MSDOS executable.

Usage:	cal [m] [y]
	'm' is 1 thru 12 or any reasonable month name.
	'y' is a year between 100 and 9999.
	Years 13-99 are abbreviations for 1913-1999.
	With no arguments, the current year is printed.
	With only a month given, the next instance of
		that month (this year or next) is printed.
	Year as a single argument gives that whole year.

------------------------cut here------------------------------
#! /bin/sh
# This is a shell archive, meaning:
# 1.  Remove everything above the #! /bin/sh line.
# 2.  Save the resulting test in a file
# 3.  Execute the file with /bin/sh (not csh) to create the files:
#
#		cal.c
#		time.asm
#		cal_exe_uu
#
# Created by evp (Ed Post) on Fri Jan 23 20:52:24 PST 1987
#
if test -f 'cal.c'
then
	echo shar: will not over-write existing file 'cal.c'
else
echo extracting 'cal.c'
sed 's/^X//' >cal.c <<'SHAR_EOF'
X/* #define LATTICE */
X#ifndef LATTICE
X#include <sys/time.h>			/* structures for time system calls */
X#endif
X#include <stdio.h>			/* buffered i/o package */
X#include <ctype.h>			/* upper/lower case macros */
X
X#ifdef LATTICE
Xstruct tm {
X	int	tm_sec;
X	int	tm_min;
X	int	tm_hour;
X	int	tm_mday;
X	int	tm_mon;
X	int	tm_year;
X	int	tm_wday;
X	int	tm_yday;
X	int	tm_isdst;
X};
X#endif
X
Xchar	string[432];
X	
X/*
X *	Day of week headers.
X */
X
Xstatic	char	dayw[] = { " S  M Tu  W Th  F  S" };
X
X/*
X *	Month of year headers.
X */
X
Xstatic	char	*smon[]= {
X	"January",	"February",	"March",	"April",
X	"May",		"June",		"July",		"August",
X	"September",	"October",	"November",	"December",
X};
X
Xmain(argc, argv)
Xint	argc;					/* argument count */
Xchar	*argv[];				/* argument vector */
X{
X	extern	int		exit();		/* terminate our process */
X	extern	int		fprintf();	/* print formatted to file */
X	extern	int		printf();	/* print formatted */
X	extern	long		time();		/* get current system time */
X	extern	struct	tm	*localtime();	/* convert sys to local time */
X
X	extern	pstr();				/* print calendar string */
X
X	register int	m;			/* month */
X	register int	y;			/* year */
X	register int	i;
X	register int	j;
X
X	long		systime;		/* system time */
X	struct	tm	*local;			/* local time */
X
X	/**
X	 *	Get the system time.
X	**/
X	
X	time(&systime);
X	
X	/**
X	 *	Convert it to local time.
X	**/
X	
X	local = localtime(&systime);
X	
X	/**
X	 *	Print the whole year if there was exactly one argument other
X	 *	than the invocation name, and that argument is a number greater
X	 *	than 13, or if there was no argument.
X	**/
X
X	if (argc == 1 || (argc == 2 && (y = number(argv[1])) > 12)) {
X
X		/**
X		 *	Print out the current year if
X		 *	no arguments are specified.
X		**/
X
X		if (argc == 1) {
X		
X			/**
X			 *	Extract the year and adjust it for this century.
X			**/
X
X			y = local->tm_year + 1900;
X
X		}
X	
X		/**
X		 *	Get the year from the command line.
X		**/
X
X		else {
X
X			/**
X			 *	Check for allowable years
X			**/
X
X			if (y < 1 || y > 9999) {
X				usage();
X			}
X
X			/**
X			 *	Allow abbreviations: 86 --> 1986.
X			**/
X
X			if (y < 100) {
X				y += 1900;
X			}
X		}
X
X		/**
X		 *	Print the year header.
X		**/
X
X		printf("\n\n\n				%u\n\n", y);
X
X		/**
X		 *	Cycle through the months.
X		**/
X
X		for (i = 0; i < 12; i += 3) {
X			for (j = 0; j < 6 * 72; j++)
X				string[j] = '\0';
X
X			printf("         %.3s", smon[i]);
X			printf("                    %.3s", smon[i + 1]);
X			printf("                    %.3s\n", smon[i + 2]);
X			printf("%s   %s   %s\n", dayw, dayw, dayw);
X
X			cal(i + 1, y, string, 72);
X			cal(i + 2, y, string + 23, 72);
X			cal(i + 3, y, string + 46, 72);
X
X			for (j = 0; j < 6 * 72; j += 72)
X				pstr(string + j, 72);
X		}
X		printf("\n\n\n");
X	}
X
X	else {
X
X		/**
X		 *	Print the current month if there was exactly one
X		 *	argument other than the invocation name, and that
X		 *	argument is a number less than 13.
X		**/
X
X		if (argc == 2 && (y = number(argv[1])) <= 12) {
X		
X			/**
X			 *	Extract the year and adjust it for this century.
X			**/
X
X			y = local->tm_year + 1900;
X
X			/**
X			 *	Get the month from the command line.
X			**/
X
X			m = number(argv[1]);
X
X			/**
X			 *	If the month has already passed, use
X			 *	next year.
X			**/
X
X			if (m < local->tm_mon+1) {
X				y++;
X			}
X
X		}
X
X		/**
X		 *	Print a specific month from the specified year if
X		 *	there was more than one argument other than the
X		 *	invocation name.
X		**/
X
X		else {
X			/**
X			 *	Get the month from the command line.
X			**/
X
X			m = number(argv[1]);
X	
X			/**
X			 *	Get the year from the command line.  Allow
X			 *	abbreviations of form nn -> 19nn.
X			**/
X
X			y = number(argv[2]);
X			if (y >0 && y < 100) {
X				y += 1900;
X			}
X		}
X
X		/**
X		 *	Generate an error if the month is illegal.
X		**/
X
X		if (m < 1 || m > 12) {
X			fprintf(stderr,
X				"cal:  month must be between 1 and 12.\n");
X			usage();
X		}
X
X		/**
X		 *	Generate an error if the year is illegal.
X		**/
X
X		if (y < 1 || y > 9999) {
X			fprintf(stderr,
X				"cal:  year must be between 1 and 9999.\n");
X			usage();
X		}
X
X		/**
X		 *	Print the month and year header.
X		**/
X
X		printf("   %s %u\n", smon[m - 1], y);
X
X		/**
X		 *	Print the day of week header.
X		**/
X
X		printf("%s\n", dayw);
X
X		/**
X		 *	Generate the calendar for the month and year.
X		**/
X
X		cal(m, y, string, 24);
X
X		/**
X		 *	Print out the month.
X		**/
X
X		for (i = 0; i < 6 * 24; i += 24)
X			pstr(string + i, 24);
X	}
X
X	/**
X	 *	All done.
X	**/
X
X	exit(0);
X}
X
Xint
Xnumber(str)
Xregister char	*str;				/* string to convert */
X{
X	int		cicmp();		/* case-insensitive compare */
X
X	register int	n;			/* number value of string */
X	register char	*s,*p;			/* loop pointers */
X
X	/**
X	 *	Convert the string to a number.
X	**/
X
X	for (n = 0, s = str; *s >= '0' && *s <= '9'; s++) {
X		n = n * 10 + *s - '0';
X	}
X	
X	if (*s == '\0') {
X		return (n);
X	}
X
X	/**
X	 *	If it's not a number, check if it's a month.
X	**/
X
X	for (n=0; n<12; n++) {
X		if (cicmp(str,smon[n]) == 0) {
X			return (n+1);
X		}
X	}
X
X	/**
X	 *	Otherwise, give up and return zero.
X	**/
X	
X	return (0);
X}
X
Xpstr(str, n)
Xchar	*str;
Xint	n;
X{
X	register int	i;
X	register char	*s;
X
X	s = str;
X	i = n;
X
X	while (i--)
X		if (*s++ == '\0')
X			s[-1] = ' ';
X
X	i = n + 1;
X
X	while (i--)
X		if (*--s != ' ')
X			break;
X
X	s[1] = '\0';
X	printf("%s\n", str);
X
X	return;
X}
X
Xcal(m, y, p, w)
Xint	m;						/* month */
Xint	y;						/* year */
Xchar	*p;
Xint	w;
X{
X	register int	d;
X	register int	i;
X	register char	*s;
X
X	/*
X	 *	Number of days per month table.
X	 */
X
X	static	char	mon[] = {
X		0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
X	};
X
X	s = p;
X
X	/**
X	 *	Get the day of the week for January 1 of this `y`ear.
X	**/
X
X	d = jan1(y);
X
X	/**
X	 *	Restore the days-per-month for February and September because
X	 *	they may have been fiddled with.
X	**/
X
X	mon[2] = 29;
X	mon[9] = 30;
X
X	switch ((jan1(y + 1) + 7 - d) % 7) {
X
X		/*
X		 *	non-leap year
X		 */
X	case 1:
X		mon[2] = 28;
X		break;
X
X		/*
X		 *	1752
X		 */
X	default:
X		mon[9] = 19;
X		break;
X
X		/*
X		 *	leap year
X		 */
X	case 2:
X		;
X	}
X
X	for (i = 1; i < m; i++)
X		d += mon[i];
X	d %= 7;
X	s += 3 * d;
X	for (i = 1; i <= mon[m]; i++) {
X		if (i == 3 && mon[m] == 19) {
X			i += 11;
X			mon[m] += 11;
X		}
X		if (i > 9)
X			*s = i / 10 + '0';
X		s++;
X		*s++ = i % 10 + '0';
X		s++;
X		if (++d == 7) {
X			d = 0;
X			s = p + w;
X			p = s;
X		}
X	}
X}
X
Xjan1(y)
Xregister int	y;					/* year */
X{
X	register int	d;				/* day */
X
X	/**
X	 *	Compute the number of days until the first of this year using
X	 *	the normal Gregorian calendar which has one extra day per four
X	 *	years.
X	**/
X
X	d = 4 + y + (y + 3) / 4;
X
X	/**
X	 *	Adjust for the Julian and Regular Gregorian calendars which
X	 *	have three less days per each 400.
X	**/
X
X	if (y > 1800) {
X		d -= (y - 1701) / 100;
X		d += (y - 1601) / 400;
X	}
X
X	/**
X	 *	Add three days if necessary to account for the great
X	 *	calendar changeover instant.
X	**/
X
X	if (y > 1752)
X		d += 3;
X	
X	/**
X	 *	Get the day of the week from the day count.
X	**/
X
X	return (d % 7);
X}
X
Xusage()
X{
X	fprintf(stderr,"Usage:\tcal [m] [y]\n");
X	fprintf(stderr,"\t'm' is 1 thru 12 or any reasonable month name.\n");
X	fprintf(stderr,"\t'y' is a year between 100 and 9999.\n");
X	fprintf(stderr,"\tYears 13-99 are abbreviations for 1913-1999.\n");
X	fprintf(stderr,"\tWith no arguments, the current year is printed.\n");
X	fprintf(stderr,"\tWith only a month given, the next instance of\n");
X	fprintf(stderr,"\t\tthat month (this year or next) is printed.\n");
X	fprintf(stderr,"\tYear as a single argument gives that whole year.\n");
X
X	exit(-1);
X}
X
Xint cicmp(s,pat)
Xchar *s,*pat;
X{
X	char	c1,c2;
X	while (*s) {
X		c1 = *s++;
X		c2 = *pat++;
X		if (isupper(c1)) {
X			c1 = tolower(c1);
X		}
X		if (isupper(c2)) {
X			c2 = tolower(c2);
X		}
X		if (c1 != c2) {
X			return (1);
X		}
X	}
X	return (0);
X}
X
X#ifdef LATTICE
Xlong time()
X{
X}
X
Xstruct tm *localtime()
X{
X	extern long time_bin(),date_bin();
X	long date,time;
X	static struct tm t;
X
X	time=time_bin();
X	date=date_bin();
X
X	t.tm_year=(date >> 16) & 0x0000FFFFL;
X	t.tm_year -= 1900;	/* conform to unix */
X	t.tm_mon=(date >> 8) & 0x000000FFL;
X	t.tm_mon -= 1;		/* unix months start at zero */
X	t.tm_day=date & 0x000000FFL;
X	t.tm_hr=(time >> 24) & 0x000000FFL;
X	t.tm_min=(time >> 16) & 0x000000FFL;
X	t.tm_sec=(time >> 8) & 0x000000FFL;
X
X	return (&t);
X}
X#endif
X
SHAR_EOF
if test 8116 -ne  ` wc -c < cal.c `
then
	echo shar: error transmitting 'cal.c' -- should have been 8116 characters
fi
fi
if test -f 'time.asm'
then
	echo shar: will not over-write existing file 'time.asm'
else
echo extracting 'time.asm'
sed 's/^X//' >time.asm <<'SHAR_EOF'
Xdgroup	group	data
Xdata	segment byte public 'DATA'
Xdata	ends
X
X_prog	segment byte
X	assume cs:_prog,ds:dgroup
X
X; long time_bin()
X;
X; Function: return the current time as a long in binary format:
X; top byte is hours (0-23), next is minutes (0-59), seconds (0-59),
X; and hundreths (0-99).
X;
X	public time_bin
Xtime_bin proc far
X	mov	ah,2Ch		; get-time command
X	int	21h		; BIOS request
X	mov	bx,dx		; ss,hu
X	mov	ax,cx		; hh,mm
X	ret
Xtime_bin endp
X
X; long date_bin()
X;
X; Function:  return the current date as a long: top two bytes are year,
X; followed by month and day in last two bytes.
X;
X	public date_bin
Xdate_bin proc far
X	mov	ah,2Ah		; get-date command
X	int	21h		; BIOS request
X	mov	bx,dx		; year
X	mov	ax,cx		; month and day
X	ret
Xdate_bin endp
X
X_prog	ends
X	end
SHAR_EOF
if test 754 -ne  ` wc -c < time.asm `
then
	echo shar: error transmitting 'time.asm' -- should have been 754 characters
fi
fi
if test -f 'cal_exe_uu'
then
	echo shar: will not over-write existing file 'cal_exe_uu'
else
echo extracting 'cal_exe_uu'
sed 's/^X//' >cal_exe_uu <<'SHAR_EOF'
Xbegin 640 cal.exe
XM35I@ ", K !  !  __\&!  !#T\"    '     0    )    - $   X %@"7
XM !8 VP 6 /@ %@ 5 18 *@$6 *P!%@!3 A8 =0(6 )D"%@"J A8 \ (6  8#
XM%@"K Q8 (P06 #D$%@!\!18 Z046 /H%%@ +!A8 ' 86 "T&%@ ^!A8 3P86
XM & &%@!K!A8 ?@86 #P'%@!#!Q8 3@<6 .T"6P/Q EL#]0); _D"6P/] EL#
XM 0-; P4#6P,) UL##0-; Q$#6P,5 UL#&0-; P\ E@#@ )8   &6  H!E@ 1
XM *< C "G *P IP 2 :< 1@&G !  O "$ +P HP"\ +0 O  & ;P *0&\ %X!
XMO "N ;P =@*\ )$"O "Y KP X0*\ !@ Z@"> .H O@#J "L!Z@!J >H %@ !
XM 3@  0%2  $!9P ! 0T " %A  @!A $( <<!" '; 0@! 0(( 3T"" %K @@!
XMGP(( 0(#" %U P@!@@,( 8@#" &< P@!H@,( 0@%" 'M!0@!+@8( 3P&" %*
XM!@@!Y@8( 70'" &P!P@!" @( 5,(" &-" @!Q0@( ? (" $/ *@!K "H 54!
XMJ $I J@!R@*H 9,#J &Z Z@!%0#H =X!Z 'Z >@!?@7H 1L <0)[ '$"F@!Q
XM LX <0(8 7$"3 %Q G(!<0*, 7$"H %Q K$!<0+@ 7$"(P)Q F\"<0*. G$"
XMR0)Q AL#<0(V W$"40-Q FX#<0*= W$"K@-Q N0#<0("!'$"7 1Q G($<0*R
XM!'$"#@5Q C<%<0)<!7$"H@5Q AH U (- -8"$0#< AP W (U -P"2P#< E8 
XMW )O -P"A0#< I8 W *Q -P"O #< M( W +C -P"_@#< @\!W (7 #H#+  Z
XM T\ .@-O #H#@P Z Z0 .@.T #H#R  Z ^( .@,1 %8#&0!6 RH 5@,     
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                              , ^KA; X[8N 8$
XMCM"\  '[N+ *HQ( C 96 ":A+ "C9@"^@  FB@PR[>,51B:*!#P]=! \('0$
XM/ EU!4E_[3/)ZT20,]M)="Y&)HH$/"!T)CP)="(L,'PH/ E_) /;<B"+TP/;
XM<AH#VW(6 ]IR$C+D ]AR#.O/"]MT!HD>X 7KN[IH .FT (L>X 71ZP/;@?L 
XM G<'NP "B1[@!2:+%@( C- KT/?" /!U"]'BT>+1XM'BZP20NO#_.]-W!KJ7
XM .MYD(D>$ "+XXO1H1  !0\ L033Z(S3 \.C6@"C7@ FBQX" "O8=M6Q!-/#
XMB\,E#P"!X_#_B1Y@ *-B  8SP%"+[(O:B\J#PP2!X_[_*^.+_#;&!6-'XP\V
XMQ@4@1R:*!#:(!49'XO8VQ@4 B\064!X'F@( E@"X $S-(;0)S2&X 4S-(:$0
XM "T$ (O@NJT M G-(;@!3,TAB^R+1@2T3,TA58/L%#LF$ !R!>I$ 0  B^P6
XMC48,4 [H#P>+Y1:-1@Q0#N@)!XOEBTX:@_D!B482B5X0="2#^0)T ^EU <1V
XM'";_= 8F_W0$#NBL HOE/0P B48&?P/I60&#?AH!=0[$=A FBP0%; >)1@;K
XM'HM&!CT! 'P%/0\G?@0.Z%H%BT8&/60 ?06!1@9L!_]V!AZX&P-0F@, IP"+
XMY<=&"   @WX(#'P#Z?L QT8*  "+1@H]L %]#(OPQH30  #_1@KK[(M&"-'@
XMT>"+\/^T[0+_M.L"'K@G U": P"G (OEBT8(T>#1X(OP_[3Q O^T[P(>N#4#
XM4)H# *< B^6+1@C1X-'@B_#_M/4"_[3S AZX3@-0F@, IP"+Y1ZX@ )0'E >
XM4!ZX: -0F@, IP"+Y8M&"$"[2 !3'KO0 %/_=@90#NCI HOEBT8(0$"[2 !3
XM'KOG %/_=@90#NC1 HOEBT8(!0, NT@ 4QZ[_@!3_W8&4 [HN *+Y<=&"@  
XMBT8*/; !?1J,V+O0  ->"KE( %%04P[H"0*+Y8-&"DCKWH-&" /I_/X>N'8#
XM4)H# *< B^7I-P&#?AH"=4K$=APF_W0&)O]T! [H,0&+Y3T, (E&!G\QQ'80
XM)HL$!6P'Q'8<)O]T!B;_= 2)1@8.Z P!B^7$=A FBUP"0SO#B48$?3O_1@;K
XM-L1V'";_= 8F_W0$#NCG (OEQ'8<)O]T"B;_= B)1@0.Z-, B^6%P(E&!GX*
XM/60 ?06!1@9L!XM&!#T! 'P%/0P ?A4>N'H#4!ZX% 90F@H Z@"+Y0[H?@.+
XM1@8] 0!\!3T/)WX5'KBA U >N!0&4)H* .H B^4.Z%P#BT8$T>#1X(OP_W8&
XM_[3I O^TYP(>N,D#4)H# *< B^4>N( "4!ZXTP-0F@, IP"+Y;@8 % >N-  
XM4/]V!O]V! [H:P&+Y<=&"   BT8(/9  ?1J,V+O0  ->"+D8 %%04P[HO "+
XMY8-&"!CKWC/ 4)H)  $!B^6#Q!1=RU6#[! [)A  <@7J1 $  (OLQT8&  "+
XM1AB+7A:)1@J)7@C$=@@FB@0PY#TP (E& 'P?/3D ?QJ+1@:["@#WZR:*'##_
XM \,M, ")1@;_1@CKT<1V"":*!##DA<!U"(M&!H/$$%W+QT8&  "+1@8]# !]
XM+-'@T>"+\/^T[0+_M.L"_W88_W86#NCN HOEA<!U"8M&!D"#Q!!=R_]&!NO,
XM,\"#Q!!=RU6#[ X[)A  <@7J1 $  (OLBT86BUX4BTX8B48,B4X(B5X*BT8(
XM_TX(A<!T&<1V"O]&"B:*!##DA<!UYXMV"B;&1/\@Z]V+1AA B48(BT8(_TX(
XMA<!T&(M&"DB.1@R+\":*'##_@_L@B48*=.#K ,1V"B;&1 $ _W86_W84'KC3
XM U": P"G (OE@\0.7<M5@^P0.R80 '(%ZD0!  "+[(M&'(M>&O]V&(E&#HE>
XM# [H'P&+Y<8&V0,=Q@;@ QZ+7AA#4XE&" [H!P&+Y04' "M&"+L' )GW^^L0
XMQ@;9 QSK%<8&X ,3ZP[K#(/Z G0'@_H!=.;KZ\=&"@$ BT8*BUX6.]A^$(OP
XMBH37 S#D 48(_T8*Z^:+1@B[!P"9]_N+PHE&"+L# /?KBUX, 48,QT8* 0"+
XM=A:*A-<#,.2+7@H[PWT#Z88 @_L#=1R*A-<#,.0]$P!U$8-&"@N*A-<#,.0%
XM"P"(A-<#BT8*/0D ?@^["@"9]_L%, #$=@PFB 2+1@Q CD8.B_")1@Q B48,
XMBT8*NPH F??[@\(P)H@4_T8,BT8(0#T' (E&"'4:QT8(  "+1AR+7AH#7AZ)
XM1AR)1@Z)7AJ)7@S_1@KI9_^#Q!!=RU6#[ 8[)A  <@7J1 $  (OLBT8,!0, 
XMNP0 F??[BUX,@\,$ ]B+1@P]" >)7@1^'RVE!KED )GW^2O8BT8,+4$&N9 !
XMF??YB5X$ ]B)7@2!?@S8!GX$@T8$ XM&!+L' )GW^XO"@\0&7<M5B^P>N.0#
XM4!ZX% 90F@H Z@"+Y1ZX^ -0'K@4!E":"@#J (OE'K@I!% >N!0&4)H* .H 
XMB^4>N$\$4!ZX% 90F@H Z@"+Y1ZX?@10'K@4!E":"@#J (OE'KBP!% >N!0&
XM4)H* .H B^4>N. $4!ZX% 90F@H Z@"+Y1ZX#@50'K@4!E":"@#J (OEN/__
XM4)H)  $!B^5=RU6#[ H[)A  <@7J1 $  (OLQ'80)H(\ '4#Z94 _T80)HH$
XMQ'84_T84)HH<B$8(,.2)1@! B_"*C& ',.V!X0$ B48"B%X)=!N*A& ',.0E
XM 0!T"(M&  4@ .L%BD8(,.2(1@B*1@DPY(E& $"+\(J<8 <P_X'C 0")1@)T
XM&XJ$8 <PY"4! '0(BT8 !2  ZP6*1@DPY(A&"8I&"3#DBEX(,/\[V'4#Z6?_
XMN $ @\0*7<LSP(/$"EW+58OL7<M5@^P,.R80 '(%ZD0!  "+[)H  )4 B48*
XMB5X(F@D E0"Y$ ")1@:)7@31^-';XOHSP(D>006!ZVP'N0@ BT8&BU8$T?C1
XMVN+Z,\"!XO\ B19#!4J+1@:+3@0SP('A_P")#D4%N1@ BT8*B1Y!!8M>"-'X
XMT=OB^C/ @>/_ +D0 (M&"HD>1P6+7@C1^-';XOHSP('C_P"Y" "+1@J)'DD%
XMBUX(T?C1V^+Z,\"!X_\ B19#!8D>2P6,V+M!!8/$#%W+M"S-(8O:B\'+M"K-
XM(8O:B\'+58/L!CLF$ !R!>I$ 0  B^S'!E %  "#/E %('P#Z7\ Q'8,)HH$
XM,.1 B_B*A6 ',.0E" !T!?]&#.OEQ'8,)HH$,.2%P'18H5 %_P90!='@T>"+
XM\(M&#HM>#(F$5 6)G%(%Q'8,)HH$,.2%P(E& '030(OXBH5@!S#D)0@ =07_
XM1@SKWL1V#":*!/]&#";&! "(1@0PY(7 = /I>?_K ,8&_04 L &S L8&( 8&
XMQ@9P",#'!G8( 0"QH,<&>@@" +H! %*B#P:B_ 6(#G@(B YT"(@>(0:('@X&
XMFO, F &+Y26  '0+H X&,.0-! "B#@8>N%(%4/\V4 6: 0 6 (OE,\!0F@D 
XM 0&+Y8/$!EW+58'L%@$[)A  <@7J1 $  (OLC-"-GB !B484B5X2Q+8< 2:*
XM!##DA<!U ^D< 8O&B48 0":*'##_@_LEB88< 8E>"G0#Z=  B_ FBAPP_X/[
XM)74[_X8< 2:*!##DBQX(!DN%VXE&"HD>" 9X#\0V @;_!@(&)H@$,.3KHHS8
XMNP(&4%/_=@J:2 &H 8OEZX\6C48,4!:-1A)0%HU&%E#_MAX!_[8< 9H( .@!
XMB^6)1A +PXE>#G4#Z63_BT80QT8(  ")AAX!B9X< 8M&"(M>##O8?P/I1_^+
XM'@@&2X7;B1X(!G@8BS8"!O\& @:+^(I#%HX&! 8FB 0PY.L7BW8(BD(6,.2,
XMV[D"!E-14)I( :@!B^7_1@CKLJ$(!DB%P*,(!G@3Q#8"!O\& @:+1@HFB 0P
XMY.GH_HS8NP(&4%/_=@J:2 &H 8OEZ=3^@<06 5W+58/L##LF$ !R!>I$ 0  
XMB^R,V+OP!8E&!HE>!(M&!+M8!SO#<Q6.1@:+\":*1 PPY(7 = :#1@02Z^&+
XM1@:+7@2,V;I8!SO!=0([VG4),\ SVX/$#%W+_W8&_W8$_W88_W86_W84_W82
XM#N@' (OE@\0,7<M5@^P..R80 '(%ZD0!  "+[,1V'":*1 R$P'0E,.0E @!T
XM#096N/__4)I( :@!B^7$=APFBD0-,.10FH (" &+Y<1V&":*1 $PY#TK '0$
XM,]OK S/;0R:*!##DB%X,Z04!H6 *#0@ B48 @GX, '0%N ( ZP.X 0"+7@ +
XMV('+  &XI %04_]V%O]V%)I4  @!B^6)1@9 =0DSP#/;@\0.7<NX @!0,\!0
XM,]M3_W8&FOL'" &+Y8)^# !T!;B  .L#N ( B48$Z;, @GX, '0%N ( ZP(S
XMP(L>8 H+V%/_=A;_=A2:5  ( 8OEB48&0'4),\ SVX/$#EW+@GX, '0%N(  
XMZP.X 0")1@3K<()^# !T!;@" .L#N $ BQY@"@O8@<L  8'+  *XI %04_]V
XM%O]V%)I4  @!B^6)1@9 =0DSP#/;@\0.7<N"?@P = 6X@ #K [@" (E&!.L@
XM,\ SVX/$#EW+/7< =*(]<@!U ^E7_SUA '4#Z>;^Z^"+1@;1X-'@B_""O'$(
XM '0?@4X$! "+1AZ+7AR#PQ".P(MV'":)1 J.P":)7 CK#\1V'";'1 H  ";'
XM1 @  (M&!L1V'":(1 TFBT0*)HM<"":)1 (FB1PSP":)1 8FB40$)HE$#HM&
XM!":(1 R,P(O>@\0.7<M5@^P$.R80 '(%ZD0!  "+[,1V"B:*1 PPY"4" '0-
XM!E:X__]0FD@!J &+Y<1V"B:*1 PPY"4, '4:)H-\#@!T$R;_= XF_W0*)O]T
XM")IA W$"B^7$=@HFQT0*   FQT0(   FQT0.   FQD0, ":*1 TPY%":@ @(
XM 8OE@\0$7<M5@>P6 3LF$ !R!>I$ 0  B^R,T(V>) &)1A2)7A+$MB !)HH$
XM,.2%P'4#Z3L!B\:)1@! )HH<,/^#^R6)AB !B5X*= /IX@"+\":*'##_@_LE
XM=4;_AB !)HH$,.3$MAP!)HM<!DLFB5P&A=N)1@IX%2:+7 (FBPPF_P2.PXOY
XM)H@%,.3KF/^V'@'_MAP!_W8*FD@!J &+Y>N$%HU&#% 6C4824!:-1A90_[8B
XM ?^V( &:" #H 8OEB480"\.)7@YU ^E9_XM&$,=&"   B88B 8F>( &+1@B+
XM7@P[V'\#Z3S_Q+8< 2:+7 9+)HE<!H7;>!HFBUP")HL,)O\$B_B*0Q:.PXOY
XM)H@%,.3K&(MV"(I"%C#D_[8> ?^V' %0FD@!J &+Y?]&".NKQ+8< 2:+1 9(
XM)HE$!H7 >!<FBT0")HL<)O\$BTX*CL FB \P[>G,_O^V'@'_MAP!_W8*FD@!
XMJ &+Y8O(Z;7^@<06 5W+58/L!CLF$ !R!>I$ 0  B^S'1@0  (M&!#T4 'T:
XMNQ( ]^N,V[GP!0/(4U&::0*\ (OE_T8$Z]['1@0  (-^!!1]#_]V!)J " @!
XMB^7_1@3KZ_]V$/]V#O]V#)H+ -0"B^6#Q 9=RU6#[ 0[)A  <@7J1 $  (OL
XMBT8*A<!X%CT4 'T1T>#1X(OPBH1P"##D)8  =0_'!H *"0 SP#/;@\0$7<N+
XM1@K1X-'@C-NY< @#R(O#B]F#Q 1=RU6#[ X[)A  <@7J1 $  (OLQT8&  "+
XM1@8]% !]%='@T>"+\(J$< @PY(7 = 7_1@;KXX-^!A1U#L<&@ H8 +C__X/$
XM#EW+BT8&T>#1X(S;N7 ( \B+\:&0"B4 @(M.&#/!B488)0" B5X,B78*= 6X
XM$ #K C/ #8  Q'8*)H@$BT88)0, ZU;$=@HFB@0PY U  ":(!.M7BT88)0@ 
XM= 6X" #K C/ #2  Q'8*)HH<,/\+V":('.LVQ'8*)HH$,.0-8  FB 3K)L1V
XM"B;&! #'!H *%@"X__^#Q Y=RST" '36/0$ =+ ]  !TF^O:Q'84)HH$,.2%
XMP'40Q'8*)L9$ 0&+1@:#Q Y=R\=&"   BT8(/1@ ?4*["@#WZXS;N< ( \A!
XM4U'_=A;_=A2:  #6 HOEA<!U'8M&"+L* /?KB_"*A, (Q'X*)HA% 8M&!H/$
XM#EW+_T8(Z[;$=@HFQD0! (M&&"4  G0-_W86_W84FKD F &+Y8M&&"4# %#_
XM=A;_=A2:( "8 8OEQ'8*)HE$ H,^0 H =#N+1A@E  -T%S/ 4/]V%O]V%)H$
XM )@!B^7$=@HFB40"@SY "@!T0\<&@ H" ,1V"B;&! "X__^#Q Y=RXM&&"4 
XM!3T !74CQ'8*)O]T IH\ )@!B^7'!H *$0#$=@HFQ@0 N/__@\0.7<N+1@:#
XMQ Y=RU6#[ 8[)A  <@7J1 $  (OLBT80)0" #0$#BUX0@>/_?U-0_W8._W8,
XM#NC)_8OE@\0&7<M5@^P<.R80 '(%ZD0!  "+[(M&*(7 >0['!H *%@"X__^#
XMQ!Q=R_]V(@[H0?V+Y8E&&@O#B5X8=0BX__^#Q!Q=R\1V&":*1 $PY.GQ HM&
XM*#T! '\#Z9< /8  ?@6X@ #K XM&*!Z[L E3HK )F@8 VP*+Y<=&"   QT8*
XM @"@L DPY(M>"#O#?E*+1@K_1@J+\(J$L F(1@8PY#T- '43_T8(BT8FBW8D
XM _..P";&! KK*8I&!C#D/1H =0?'1@@  .L8BT8(_T8(BUXFBW8D _"*1@:.
XMPR:(!.NBN H 4)IX -P"B^6+1@B#Q!Q=R[C< KL$ +G< KIX (E&$HE.%HE6
XM%(E>$.L8N-P"NZ0 N=P"NL4 B482B4X6B584B5X0Q'88)HH$,.0E$ !T-,=&
XM"   BT8(.T8H?1^+1@C_1@B+7B:+3B0#R(E. (E> O]>$,1V ":(!.O9BT8(
XM@\0<7<O'1@@  (M&"#M&*'P#Z>( _UX0B$8&,.3IP0"X#0!0_UX4B^6X"@!0
XM_UX4B^6+1@B)1@! BUXFBW8D W8 CL,FQ@0*B48(@\0<7<N#?@@ =+2X" !0
XM_UX4B^6X( !0_UX4B^6X" !0_UX4B^7KE[@- %#_7A2+Y;@* %#_7A2+Y3/ 
XM@\0<7<N+1@C_1@B+7B:+=B0#\(I&!H[#)H@$,.0]( !]&KA> %#_7A2+Y8I&
XM!C#D!4  4/]>%(OEZ4?_BD8&,.10_UX4B^7I.?\: &($"  _! T #P0*  \$
XMO@P +CN$P 1U!2[_I,($@^X$>>_KE8M&"(/$'%W+,\"#Q!Q=R_]V*/]V)O]V
XM),1V&";_= *:4P"8 8OEB48(@SY "@!T"+C__X/$'%W+Q'88)HH$,.0E$ !T
XM"(M&"(/$'%W+,\")1@J)1@R+1@J+7@@[V'YB_T8*BUXFBW8D _".PR:*!(A&
XM!C#DZS^+1@HK1@A(,]N%P'D!2[D! %%34/]V(@[HA *+Y8M&#(/$'%W+Z[B+
XM1@S_1@R+7B:+=B0#\(I&!H[#)H@$ZZ ]#0!TFST: '2WZ]R#?@P =0F#?@@ 
XM= /I/_^+1@R#Q!Q=R[C__X/$'%W+]03? NX$FP/N!(OP@_X%<P?1YB[_I,8%
XMZ]Y5@^P6.R80 '(%ZD0!  "+[(M&(H7 >0['!H *%@"X__^#Q!9=R_]V' [H
XM\_F+Y8E&% O#B5X2=0BX__^#Q!9=R\1V$B:*1 $PY.F^ ;C< KMX (E&"(E>
XM!NL:N-P"N_$ B48(B5X&ZPRXW *[Q0")1@B)7@;'1@H  (M&"HM>(CO8?CW_
XM1@J+7B"+=AX#\([#)HH$B$8$,.0]"@!U%L1V$B:*!##D)1  =0FX#0!0_UX&
XMB^6*1@0PY%#_7@:+Y>NYBT8*@\067<O$=A(FB@0PY"4( '03N ( 4#/ 4#/;
XM4_]V' [H- &+Y<1V$B:*!##D)1  ="[_=B+_=B#_=AXF_W0"FG0 F &+Y8E&
XM"H,^0 H = BX__^#Q!9=RXM&"H/$%EW+QD8% #/ B48*B48,B48.BT8*BUXB
XM.]A^>_]&"HM>((MV'@/PCL,FB@2(1@0PY#T* '41BD8%,.0]#0!T!\9&! W_
XM3@J+1@R)1@! BW8 BEX$B)RP"3V  (E&#(A>!7RO4!ZXL E0Q'82)O]T IIT
XM )@!B^6)1A"#/D * '0(BT8*@\067<N+1A !1@['1@P  .E[_X-^# !T)?]V
XM#!ZXL E0Q'82)O]T IIT )@!B^6)1A"#/D * '0%QT80  "+1@J#Q!9=RXM&
XM(H/$%EW+QP: "A, N/__@\067<NI!BT&.P9)!LL'B_"#_@5S!]'F+O^DX0?K
XMV%6#[ P[)A  <@7J1 $  (OL_W82#NCM]XOEB48*"\.)7@AU"[C__[O__X/$
XM#%W+Q'8()HI$ 3#DA<!T"3/ ,]N#Q Q=R_]V&/]V%O]V%,1V"";_= *:E0"8
XM 8OEB48&B5X$@SY "@!T$<<&@ H6 +C__[O__X/$#%W+BT8&BUX$@\0,7<M5
XM@^P*.R80 '(%ZD0!  "+[/]V$ [H:/>+Y8E&" O#B5X&=0BX__^#Q I=RS/ 
XMQ'8&)HI< 3#_A=N)1@1U%R;_= *:/ "8 8OE@SY "@!T!<=&!/__Q'8&)L8$
XM (M&!(/$"EW+58OL_W8(_W8&FKD F &+Y8,^0 H = 6X__]=RS/ 7<M5B^S'
XM!D *   >Q58&BTX*M#S-(1]S Z- "EW+58OLQP9 "@  'L56!HM&"K0]S2$?
XM<P.C0 I=RU6+[,<&0 H  (M>!K0^S2%S Z- "EW+58OLQP9 "@  BUX&BTX,
XM'L56"+0_S2$?<P6C0 HSP%W+58OLQP9 "@  BUX&BTX,'L56"+1 S2$?<P6C
XM0 HSP%W+58OLQP9 "@  BT8,M$*+7@:+3@J+5@C-(7,#HT *B]B+PEW+58OL
XMQP9 "@  'L56!K1!S2$?<P.C0 I=RU6+[,<&0 H  ![%5@:+1@JT0XM.#,TA
XM'W,#HT *B\%=RU6+[(M>!K@ 1,TAB\)=RU6#[ @[)A  <@7J1 $  (OLQ'8.
XM)HI$###D)3  = BX__^#Q A=R\1V#B:#? X =1\FBD0,,.0E! !U% 96#NA"
XM XOEA<!T"+C__X/$"%W+Q'8.)HI$###D)00 = ?'1@8! .LMQ'8.)HI$###D
XM)0( = BX__^#Q A=R\1V#B:*1 PPY T! ":(1 PFBT0.B48&Q'8.)HI$#3#D
XM_W8&)O]T"B;_= A0FI("" &+Y87 B48$>1#$=@XFBD0,,.0-(  FB$0,@WX$
XM '40Q'8.)HI$###D#1  )HA$#(M&!(7 ?A;$=@XFB40$)HM$"B:+7 @FB40"
XM)HD<Q'8.)HI$###D)3  = BX__^#Q A=R\1V#B:+1 1()HE$!(7 >!,FBT0"
XM)HL<)O\$CL FB@\P[>L._W80_W8.#NC%_HOEB\B+P8/$"%W+58/L#CLF$ !R
XM!>I$ 0  B^R+1A3$=A8FBEP,,/^!XS  B48)= BX__^#Q Y=R\1V%B:#? X 
XM=6XFBD0,,.0E! !U8P96#NCU 8OEA<!T"+C__X/$#EW+Q'86)HI$###D#0( 
XM)HA$#":+1 XFB40&2":)1 :%P'@6)HM$ B:+'";_!(M.%([ )H@/,.WK$?]V
XM&/]V%O]V% [H9?^+Y8O(B\&#Q Y=R\1V%B:*1 PPY"4$ '0\@WX4_W4',\"#
XMQ Y=RXM&%,1V%B:*7 TP_\=&"P$ _W8+%HU.!E%3B$8&FN %" &+Y<=&%/__
XMB48'Z:  Q'86)HI$###D)0$ = BX__^#Q Y=R\1V%B:*1 PPY T" ":(1 R#
XM?A3_=$ F@WP& 'XY)HM$!D@FB40&A<!X%B:+1 (FBQPF_P2+3A2.P":(#S#M
XMZQ'_=AC_=A;_=A0.Z*W^B^6+R,=&%/__Q'86)HL$)BM$"(E&"X7 =!XFBD0-
XM,.3_=@LF_W0*)O]T"%":X 4( 8OEB48'ZP7'1@<  (-^!_]U$L1V%B:*1 PP
XMY T@ ":(1 SK&(M&!SM&"W00Q'86)HI$###D#1  )HA$#,1V%B:+1 XFB40&
XM)HM$"B:+7 @FB40")HD<BT84/?__=#$FBUP&2R:)7 :%VW@5)HM< B:+#";_
XM!([#B_DFB 4PY.L/_W88_W86_W84#NCO_8OEQ'86)HI$###D)3  = BX__^#
XMQ Y=RX-^"?]U!S/ @\0.7<N+1@F#Q Y=RU6#[ 0[)A  <@7J1 $  (OLQ'8*
XM)H-\#@!T$B:*1 PPY"4( '4',\"#Q 1=R_\VH J:90%Q HOEQ'8*)HE$ B:)
XM'":)1 HFB5P("\-U#L<&@ H, +C__X/$!%W+H: *Q'8*)HE$#B:*1 PPY"7S
XM_R:(1 PSP":)1 8FB40$@\0$7<M5@^PZ.R80 '(%ZD0!  "+[#/ QT84___'
XM1A8@ ,1V0":*'##_@_LMB48.B488B480B482=0FX 0#_1D")1@[$=D FB@0P
XMY(E& $"+\(J$8 <PY"4$ '1-@WX ,'4%QT86, #$=D#_1D FB@0PY"4/ (E&
XM&,1V0":*!##D0(OXBH5@!S#D)00 =!N+1AB["@#WZ_]&0":*'##_@>,/  /#
XMB488Z\_$=D FB@0PY#TN '4Y_T9 QT84  #$=D FB@0PY$"+^(J%8 <PY"4$
XM '0;BT84NPH ]^O_1D FBAPP_X'C#P #PXE&%.O/Q'9 )HH$,.0]; !U";@!
XM /]&0(E&$,=&(   QT8>  #$=D FB@0PY.DR X-^$ !T(<1V2":+1 (FBQPF
XM@P0$CL FBT\"CL FBQ>)3B2)5B+K(,1V2":+1 (FBQPF@P0"CL FBP\STH7)
XM>0%*B4XBB58DBT8DA<!Y4_=6)/=>(H->)/_'1A(! .M"@WX0 '0AQ'9()HM$
XM B:+'":#! 2.P":+3P*.P":+%XE.)(E6(NL;Q'9()HM$ B:+'":#! *.P":+
XM#S/ B48DB4XBQT8("P"+1@A(B_")1@B+1B2+7B(SR;H* (EV )H- .T"@\(P
XM@]$ BW8 B%(FBT8DBUXB,\FZ"@":#0#M HE&)(E>(HM&) M&(G6Z@WX2 74-
XMBT8(2(OPQD(F+8E&"(S0C5XFBTX( ]FZ"P KT8E&((E6#(E>'NDN H-^$ !T
XM(<1V2":+1 (FBQPF@P0$CL FBT\"CL FBQ>)3B2)5B+K&\1V2":+1 (FBQPF
XM@P0"CL FBP\SP(E&)(E.(L=&" @ BT8(2(OPBUXDBTXB,]N!X0\ B_F*G5 *
XMB%HFN00 BUXDBU8BT?O1VN+ZB5XD@>/_#XE&"(E6(HE>)(M&) M&(G6^C-"-
XM7B:+3@@#V;H( "O1B48@B58,B5X>Z8H!@WX0 '0AQ'9()HM$ B:+'":#! 2.
XMP":+3P*.P":+%XE.)(E6(NL;Q'9()HM$ B:+'":#! *.P":+#S/ B48DB4XB
XMQT8("P"+1@A(B_"+7B2+3B(SVX'A!P"#P3"#TP"(2B:Y P"+7B2+5B+1^]':
XMXOJ)7B2!X_\?B48(B58BB5XDBT8D"T8B=;Z,T(U>)HM." /9N@L *]&)1B")
XM5@R)7A[IY@"#?A3_=07'1A3( ,=&#   BT8,BUX4.]A^*,1V2":+7 (FBSR.
XMPR:+10*.PR:+-0-V#([ )HH<,/^%VW0%_T8,Z\[$=D@FBT0")HL<)H,$!([ 
XM)HM/ H[ )HL7B4X@B58>Z8( C-"-7B;'1@P! ,1V2":+3 (FBQ0F@P0"B48@
XMCL&+\B:+!(A&)HE>'NM8QT8< @#K4<=&'   ZTK'1AP! .M#,\ SVX/$.EW+
XM9@ <!&4 %01G  X$8P#D W, @ -O -P">  X G4 ?0%D !X!OB  +CN$+ 1U
XM!2[_I"X$@^X$>>_KO8M&( M&'G4#Z=  BT88A<!T!3M&#'T&BT8,B488BT8,
XM*488QT8(  "#?@X =$R+1@Q(A<")1@QX'HM&"/]&"(M>1HMV1 /PQ'X>_T8>
XM)HH%CL,FB 3KUXM&&$B%P(E&&'ADBT8(_T8(BUY&BW9$ _"+1A:.PR:(!.O=
XMBT882(7 B488>!B+1@C_1@B+7D:+=D0#\(M&%H[#)H@$Z]V+1@Q(A<")1@QX
XM'HM&"/]&"(M>1HMV1 /PQ'X>_T8>)HH%CL,FB 3KUXM&",1V3":)!(M&0HM>
XM0$.#Q#I=RX-^%/]U!<=&% 8 @WX4%'P%N!, ZP.+1A2)1@Q %HU>)E,6C5X2
XM4Q:-7AI3_W8<4,1V2";_= (F_S2:  #Y HOEQ'9()H,$"(S3C4XFBU8:A=*)
XM1@R)3AZ)5@B)7B!Y _=>"(-^' )U%H-^# !T"H-^" 9\!#/ ZP,SP$")1AR#
XM?@P = /_3AHSP(E&"(-^$@!T _]&"(-^' !T(8M&%$"+7@@#PXM>&H7;B48(
XM> ,!7@B#?A0 =#'_1@CK+(M&% 4&  %&"(M&&H7 >03WV.L#BT8:/6, B48*
XM?@/_1@B!?@KG WX#_T8(@WX. '4JBT88.T8(?B*+1@@I1AB+1AA(A<")1AAX
XM$<1V1/]&1(M&%B:(!/]&".OD@WX2 '0*Q'9$_T9$)L8$+8-^' !U ^GX (M&
XM&H7 >6W$=D2+QHE& $ FQ@0PB_")1D1 )L8$+HE&1(M&%$B%P(E&%'D#Z;<!
XMBT8:0(7 B48:>0S$=D3_1D0FQ@0PZ]N+1@Q(A<")1@QX%XMV1/]&1,1^'O]&
XM'B:*!8Y&1B:(!.NYQ'9$_T9$)L8$,.NMBT8:_TX:A<!X+HM&#$B%P(E&#'@7
XMBW9$_T9$Q'X>_T8>)HH%CD9&)H@$Z]3$=D3_1D0FQ@0PZ\B#?A0 = K$=D3_
XM1D0FQ@0NBT842(7 B484>0/I' &+1@Q(A<")1@QX%XMV1/]&1,1^'O]&'B:*
XM!8Y&1B:(!.O0Q'9$_T9$)L8$,.O$BT8,2(7 B48,>!>+=D3_1D3$?A[_1AXF
XMB@6.1D8FB 3K"L1V1/]&1";&!##$=D3_1D0FQ@0NBT842(7 B484>"Z+1@Q(
XMA<")1@QX%XMV1/]&1,1^'O]&'B:*!8Y&1B:(!.O3Q'9$_T9$)L8$,.O'Q'9$
XMB\:)1@! )L8$18M>&H7;B49$>0Z+\/]&1";&!"WW7AKK"L1V1/]&1";&!"O'
XM1@P+ (M&#$B+\(E&#(M&&KL* )GW^X/",(A2)HM&&IGW^XE&&H-^# E_V8-^
XM&@!UTXM&##T+ 'T6BW9$_T9$_T8,B_B*0R:.1D8FB 3KXH-^#@%U*HM&&#M&
XM"'XBBT8(*488BT882(7 B488>!'$=D3_1D2+1A8FB 3_1@CKY(M&",1V3":)
XM!(M&0HM>0$.#Q#I=RU6#[ 0[)A  <@7J1 $  (OLH7(*BQYP"HL.=@J+%G0*
XMB0Y^"@O*HWH*B19\"HD>> IU"+C__X/$!%W+Q#9X"B;'1 (  ";'!   H7X*
XMBQY\"B:)1 8FB5P$,\"#Q 1=RU6#[ ([)A  <@7J1 $  (OL,\!0#N@' (OE
XM@\0"7<M5@^P*.R80 '(%ZD0!  "+[(M&$(7 >0BX__^#Q I=RS/ ,]O'1@B 
XM #/)4;H !%*C=@JC<@J)'G0*B1YP"IH* #H#B^6)1@8+PXE>!'4(N/__@\0*
XM7<N+1@:+7@2+3@@STH7)>0%*HW(*B0YT"HD6=@J)'G *BT802(E&$(7 ="8S
XMP%"[  13F@H .@.+Y0O#=!2+1@@SVX7 >0%+ 09T"A$>=@KKSP[HUOXSP(/$
XM"EW+58/L!#LF$ !R!>I$ 0  B^RY P"A?@J+'GP*T>/1T.+Z@\0$7<M5@^P(
XM.R80 '(%ZD0!  "+[(M&#C/;4U")1@2)7@8.Z P B^6:"0!) X/$"%W+58/L
XM&#LF$ !R!>I$ 0  B^R+1B"+7AXSR3/2F@< 3 -_"8O!B]J#Q!A=RXM&'@4(
XM (M>((/3 "T! (/; (E& (O#BUX ,\FZ" ":#0#M HS9OG@*CL$FBU0"B486
XMC$8.CL$FBP2)1@B)5@J)7A2)=@R+1@H+1@AU ^FF ,1V"":+1 8FBUP$BTX6
XMBU84F@< 3 -\;CO!=0([VG43)HM$ B:+',1V#":)1 (FB1SK-<1V"":+1 0K
XM1A0FBUP&&UX6)HE<!B:)1 2Y P#1X-'3XOJ+RXO0C,"+WIH$ $X#B48*B5X(
XMBT84*09\"HM>%AD>?@J+1@J+7@B:"0!) X/$&%W+BT8*BUX(CL FBT\"CL F
XMBQ>)1@Z)3@J)5@B)7@SI3_^Y P"+1A:+7A31X]'0XOI04YH* #H#B^6)1A*)
XM7A +PW4#Z7X H78*"P9T"G4:BT82BTX6BU84HW(*B0YV"HD6= J)'G *ZTNY
XM P"A=@J+'G0*T>/1T.+ZB\B+TZ%R"HL>< J:! !. XM.$HM6$(E&"HO!B5X(
XMB]J+3@J+5@B: @!1 W4.BT84 09T"HM>%A$>=@J+1A*+7A":"0!) X/$&%W+
XM,\ SVX/$&%W+58/L"#LF$ !R!>I$ 0  B^R+1A(SVU-0_W80_W8.B48$B5X&
XM#N@' (OE@\0(7<M5@^P@.R80 '(%ZD0!  "+[(M&+(M>*C/),]*:!P!, W\(
XMN/__@\0@7<N+1BB+7B:+3BJ#P0B+5BR#T@"#Z0&#V@")1A:+PHE>%(O9,\FZ
XM" ":#0#M KD# (E&'HE>'-'CT=#B^HO(B].+1A:+7A2:! !. XM.' $.? J+
XM5AX1%GX*C-F^> J.P2:+5 *)1AJ,1@Z.P2:+!(E&!(E6!HE>&(EV#(M&!HM>
XM! O#=0/IPP&Y P".1@8FBT<&)HM7!-'BT=#B^HO(C,":! !. XE&"HM&&HE>
XM"(M>&(S!BU8$F@( 40-S+<1V%":)3 (FB12+1AZ+7APFB40&)HE<!(S B][$
XM=@PFB40")HD<,\"#Q"!=RXM&!HM>!(M.&HM6&)H" %$#=4F.P":+3P*.P":+
XM%\1V%":)3 (FB12+1AR.1@8F T<$BUX>BWX$)A-=!HY&%B:)7 8FB40$C,"+
XMWL1V#":)1 (FB1PSP(/$(%W+BT86BUX4BTX*BU8(F@( 40-S%HM&'"D&? J+
XM7AX9'GX*N/__@\0@7<N+1A:+7A2+3@J+5@B: @!1 W0#Z9P Q'8$)HM$ B:+
XM' O#="J+1AJ+7A@FBTP")HL4F@( 40-V%HM&'"D&? J+7AX9'GX*N/__@\0@
XM7<N+1AS$=@0F 40$BUX>)A%<!B:+1 (FBQP+PW1!BT8:BUX8)HM, B:+%)H"
XM %$#=2V.P":+3P2.1@8F 4P$CL FBU<&CD8&)A%4!H[ )HM' B:+'XY&!B:)
XM1 (FB1PSP(/$(%W+BT8&BUX$BTX*BU8(B48.CL FBT<"B5X,)HL?B48&B4X2
XMB580B5X$Z3#^BT86BUX4Q'8,)HE$ B:)'([ )L=' @  CL FQP<  (M&'HM>
XM'(MV%":)1 8FB5P$,\"#Q"!=RU6+[/]V"O]V"/]V!II8 0  B^5=RU6#[ 0[
XM)A  <@7J1 $  (OLQ'8.)HH$,.3$=@HFBAPP_SO8=1@FB@0PY(7 =0<SP(/$
XM!%W+_T8*_T8.Z]3$=@XFB@0PY,1V"B:*'##_*]B+PX/$!%W+58OL'L56!K0*
XMS2$?7<M5@^P$.R80 '(%ZD0!  "+[+@( %":!P!5 XOEB$8"A,!T!S#D@\0$
XM7<NX" !0F@< 50.+Y8/$!%W+58/L!#LF$ !R!>I$ 0  B^RX 0!0F@< 50.+
XMY8A& H3 = <PY(/$!%W+N $ 4)H' %4#B^6#Q 1=RU6#[ ([)A  <@7J1 $ 
XM (OLBD8(,.10N ( 4)H' %4#B^6*1@@PY(/$ EW+58/L CLF$ !R!>I$ 0  
XMB^RX P!0F@< 50.+Y8/$ EW+58/L CLF$ !R!>I$ 0  B^R*1@@PY%"X! !0
XMF@< 50.+Y8I&"##D@\0"7<M5@^P".R80 '(%ZD0!  "+[(I&"##D4+@% %":
XM!P!5 XOEBD8(,.2#Q )=RU6#[ 2+[(E.  O*=0<SP#/;Z9H B48""\-U!S/)
XM,]+IC "+3@"+1@*+\(7V>1+WT/?;'?__>0DSP#/),]+K;Y S\8EV #/VA<EY
XM!_?1]]J#V?]U'872>!F_( #1X]'0T=8[\G(#*_)#3W7P,\F+UNLAOQ  T>/1
XMT-'6._%R"W4$.\)R!2O"&_%#3W7HB\Z+T#/ ]T8  (!T!_?0]]L=___W1@( 
XM@'0']]'WVH/9_X/$!%W+58/L*(OLQT8   #'1@P  ,1V+B:+1 :%P'D%@4X 
XM ( E_W]U%HOX)@M\!'4.)@M\ G4()@L\=0/I@ "Q!-/H+?X#B48.)HL,)HM<
XM B:+1 0FBW0&,]*_!0#1[M'8T=O1V=':3W7S#0" B48$B5X&B4X(B58*QT8,
XM  "+=@Z+SH7V=&EY$(/& W@3O@$ Z#L#_T8.Z^7HJ@+_1@SK#[X$  %V#N@E
XM ^@$ _].#/=&! " =<?_3@[HO0+K\3/V@4X  $ SR?=&  ! =2&+1@0+1@9U
XM"(M&" M&"G01Z,\"A<EU"H/^ 74%_TX,Z^^ P3"(2A1&@_X4?,K'1A ! (M&
XM-(MV,H7 = NY 0"+1@Q( _!X+;D4 (/^$GTEB\Z*0A4$!3PZ?!K&0A4P_D(4
XMBD(43GGO_T8,_TX0@WXT '0!0<1^/HMV$#/;BD(4)H@%0T<[V7T21H/^%'SN
XML# FB 5'0SM>,GSV,\#W1@  @'0(]T8  $!U 4C$=CHFB02+1@S$=C8FB03W
XM1@  0'0",\F+P8/$*%W+58/L*(OL,\")1@")1@R)1@*)1@Z)1@2)1@:)1@B)
XM1@KH-P(\,'4'@4X  "#K\CPM=0B!3@  @.@@ CPP?#@\.7\T@4X  "")1A"+
XM1@(I1@SW1@0 \'0%_T8,Z]OHN0&+1A E#P !1@J#5@@ @U8& (-6! #KP3PN
XM=0R+1@*%P'5"_T8"Z[$\170$/&5U->C) 3PK= D\+74(@4X  !#HN0$\,'P>
XM/#E_&B0/B480BT8.Z$X! T80B48./?\/<M_1Z.OT]T8  "!U$#/ Q'8V)HD$
XMBT82@\0H7<N+1@[W1@  $'0"]]@!1@R+1@0+1@8+1@@+1@IU ^F  ,=&#CX$
XM]T8$ (!U".C8 /].#NOQBT8,A<!T&W@1O@0  78.Z!8!Z/4 _TX,Z]?H@@#_
XM1@SKSXM&#H7 >$$]_P=_-[$$T^")1@Z+1@2+7@:+3@B+5@J*UHKQBLV*ZXK?
XMBOB*Q+\# -'HT=O1V=':3W7U)0\ "T8.ZPVX\'_K C/ ,]LSR3/2Q'8Z]T8 
XM (!T PT @":)1 8FB5P$)HE, B:)%+@! ,1V-B:)!(M&$H/$*%W+OD  ,\"+
XM7@2+3@:+5@B+?@K1Y]'2T='1T]'0/0H <@0M"@!'3G7JB5X$B4X&B58(B7X*
XMPXM&"M'@B48*BT8(T=")1@B+1@;1T(E&!HM&!-'0B48$PXO8,\G1X-'1T>#1
XMT0/#@]$ T>#1T<,SR;\& (M#!(E+!.C<_XM;! /#@]$ B4,$3T]YZ,.+1@2+
XM7@:+3@B+5@K1Z-';T=G1VDYU]8E&!(E>!HE."(E6"L.#?BX = F+1B['1BX 
XM ,/_7C*)1A+_3C!U!H%.   (P_=&   (= .X___#58/L!CLF$ !R!>I$ 0  
XMB^R+1@Z+7@R+#F( BQ9@ )H' $P#?@DSP#/;@\0&7<NA7@"+'EP BTX.BU8,
XMB48$B5X"F@0 3@.+3@PI#F  BU8.&19B *-> (D>7 "+1@2+7@*:"0!) X/$
XM!EW+58/L##LF$ !R!>I$ 0  B^R+1A(SVU-0#NAX_XOEB48*B5X("\-T#8M&
XM"IH) $D#@\0,7<NX__\SVY.:"0!) X/$#%W+58/L!#LF$ !R!>I$ 0  B^RX
XM 0!0H5X BQY< (L.6@"+%E@ F@D 5@,!'F  $09B (D.7@")%EP @\0$7<M1
XML033P(O()0\ @>'P_P/9%0  B\N!X0\ D]'KT=C1Z]'8T>O1V-'KT=B+V5G+
XM4#/!6%!X#RO!=0XSP#O:= AW TCK PW_?X7 6,M1L033P(O(@>'P_R4/  /9
XM%0  60/:$\$E#P"Q!-/(B]/3RH'B_P\+PH'C#P#+4%-14E8S]M'@T=;1X-'6
XMT>#1UM'@T=8#V(/6 (O&,_;1X='6T>'1UM'AT=;1X='6 ]&#U@"+SCO:&\%U
XM CO:7EI96UC+58OLBF8&BD8*BU8(S2&T %W+58OL45*: @!9 Y&'VIH" %D#
XMD8?:*]H;P8M6!C/)F@T [0):65W* @!1L033P(O(@>'P_R4/  /9%0  6<L 
XM             $QA='1I8V4@0R R+C P                            
XM                                                            
XM                             $EN=F%L:60@<W1A8VL@<VEZ90T*)$EN
XM=F%L:60@22]/(')E9&ER96-T:6]N#0HD26YS=69F:6-I96YT(&UE;6]R>0T*
XM)"HJ*B!35$%#2R!/5D521DQ/5R J*BH-"B0                         
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                        (%,@($T@5'4@(%<@5&@@($8@(%, 2F%N=6%R
XM>0!&96)R=6%R>0!-87)C: !!<')I; !-87D 2G5N90!*=6QY $%U9W5S= !3
XM97!T96UB97( 3V-T;V)E<@!.;W9E;6)E<@!$96-E;6)E<@"5 EL#G0); Z8"
XM6P.L EL#L@); [8"6P.[ EL#P ); \<"6P/1 EL#V0); ^("6P,*"@H)"0D)
XM)74*"@ @(" @(" @(" E+C-S " @(" @(" @(" @(" @(" @(" @)2XS<P @
XM(" @(" @(" @(" @(" @(" @("4N,W,* "5S(" @)7,@(" E<PH "@H* &-A
XM;#H@(&UO;G1H(&UU<W0@8F4@8F5T=V5E;B Q(&%N9" Q,BX* &-A;#H@('EE
XM87(@;75S="!B92!B971W965N(#$@86YD(#DY.3DN"@ @(" E<R E=0H )7,*
XM   ?'1\>'QX?'QX?'A]5<V%G93H)8V%L(%MM72!;>5T*  DG;2<@:7,@,2!T
XM:')U(#$R(&]R(&%N>2!R96%S;VYA8FQE(&UO;G1H(&YA;64N"@ ))WDG(&ES
XM(&$@>65A<B!B971W965N(#$P,"!A;F0@.3DY.2X*  E996%R<R Q,RTY.2!A
XM<F4@86)B<F5V:6%T:6]N<R!F;W(@,3DQ,RTQ.3DY+@H "5=I=&@@;F\@87)G
XM=6UE;G1S+"!T:&4@8W5R<F5N="!Y96%R(&ES('!R:6YT960N"@ )5VET:"!O
XM;FQY(&$@;6]N=&@@9VEV96XL('1H92!N97AT(&EN<W1A;F-E(&]F"@ )"71H
XM870@;6]N=&@@*'1H:7,@>65A<B!O<B!N97AT*2!I<R!P<FEN=&5D+@H "5EE
XM87(@87,@82!S:6YG;&4@87)G=6UE;G0@9VEV97,@=&AA="!W:&]L92!Y96%R
XM+@H                                                         
XM                                                            
XM                                                            
XM                                      @                     
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM                                                            
XM         " @(" @(" @("@H*"@H(" @(" @(" @(" @(" @(" @2! 0$! 0
XM$! 0$! 0$! 0$(2$A(2$A(2$A(00$! 0$! 0@8&!@8&! 0$! 0$! 0$! 0$!
XM 0$! 0$! 0$0$! 0$!""@H*"@H(" @(" @(" @(" @(" @(" @(" A 0$! @
XM(" @(" @(" @*"@H*"@@(" @(" @(" @(" @(" @("!($! 0$! 0$! 0$! 0
XM$! 0A(2$A(2$A(2$A! 0$! 0$!"!@8&!@8$! 0$! 0$! 0$! 0$! 0$! 0$!
XM 1 0$! 0$(*"@H*"@@(" @(" @(" @(" @(" @(" @("$! 0$"          
XM                                                            
XM                                                          %C
XM;VXZ       !0T]..@       G!R;CH       )04DXZ       ";'-T.@  
XM     DQ35#H       )L<'0Z       "3%!4.@       FQP=#$Z      ),
XM4%0Q.@     #875X.@       T%56#H       -C;VTZ       #0T]-.@  
XM     V-O;3$Z      -#3TTQ.@     #<F1R.@       U)$4CH       -P
XM=6XZ       #4%5..@      !&YU;#H       1.54PZ       $;G5L;#H 
XM    !$Y53$PZ                                                
XM                                                            
XM                                                            
XM                                                   P,3(S-#4V
XM-S@Y04)#1$5&                                                
XM                                       "                  !P
XM1                    P)#3 (   0!0P   @ $!EA#15A)5   6 $$!5A#
XM3U9&  !$ 00%7T)!4T5; Q( ! 9?24Y!345; Q0 ! 9?34)!4T5; U@ ! 9?
XM34Y%6%1; UP ! 9?35-)6D5; V  ! 9?3TY!345; S0 ! 1?4%-06P-4  0$
XM7U1/4%L#$  $!%]615); P(  P-#04P$!$U!24X6  $ !CT %@ 2  9# !8 
XM'0 &2P 6 "@ !E( %@!:  98 !8 8  &8  6 &P !F8 %@!N  9G !8 >P &
XM;@ 6 '\ !F\ %@"'  9W !8 C  &?0 6 )L !GX %@"I  9_ !8 M@ &@0 6
XM +T !H( %@#?  :# !8 _  &A  6 !D!!H8 %@ N 0:' !8 10$&B  6 %T!
XM!HH %@!V 0:+ !8 @P$&C  6 )<!!HT %@"= 0:0 !8 L $&F  6 +,!!IX 
XM%@#2 0:D !8 VP$&JP 6 .\!!JP %@#^ 0:W !8  0(&O  6  ,"!L, %@ 4
XM @;$ !8 * (&Q0 6 #0"!LT %@ Y @;/ !8 1@(&T  6 %<"!M< %@!; @;9
XM !8 : (&V@ 6 'D"!N$ %@!] @;G !8 G0(&[0 6 *X"!O, %@## @;T !8 
XMT (&]0 6 .0"!OL %@#J @;^ !8 ] ($!DY534)%4A8 ^0(&"P$6  H#!@P!
XM%@ P P8/ 18 10,&$ $6 %8#!A<!%@!> P88 18 :P,&&0$6 (D#!B$!%@"2
XM P0$4%-44A8 G@,&*P$6 *\#!BP!%@"U P8N 18 P0,&+P$6 ,L#!C !%@#:
XM P8R 18 X@,&- $6 .L#!C4!%@#U P8V 18 "P0&. $6  T$!CD!%@ 5! 8[
XM 18 )P0$ T-!3!8 + 0&4 $6 #T$!E8!%@!#! 9= 18 4@0&7@$6 %<$!F !
XM%@!<! 9F 18 > 0&9P$6 'T$!FT!%@!_! 9N 18 A 0&=P$6 (8$!G@!%@"C
XM! 9Y 18 K@0&>@$6 +P$!GL!%@#,! 9\ 18 Y 0&?0$6 /0$!GX!%@#X! : 
XM 18 !04&@0$6  T%!H(!%@ <!0:# 18 ( 4&A $6 #L%!H4!%@ ^!0:& 18 
XM2@4&AP$6 $\%!H@!%@!8!0:+ 18 9 4&C0$6 &H%! 1*04XQ%@!O!0:8 18 
XM@ 4&GP$6 )0%!J !%@"?!0:A 18 J@4&J0$6 +X%!JH!%@#%!0:P 18 R04$
XM!55304=%%@#9!0:U 18 W 4&M@$6 .T%!K<!%@#^!0:X 18 #P8&N0$6 " &
XM!KH!%@ Q!@:[ 18 0@8&O $6 %,&!KX!%@!D!@;! 18 ;P8$!4-)0TU0%@!Q
XM!@;% 18 @@8&Q@$6 (X&!L<!%@"4!@;( 18 G08&R0$6 +H&!LL!%@#5!@;,
XM 18 [P8&S@$6  H'!L\!%@ ;!P;2 18 (P<$!%1)3446 "H'!MH!%@ M!P0(
XM3$]#04Q424T6 "\'!N !%@! !P;A 18 10<&XP$6 % '!N0!%@!A!P;E 18 
XM:0<&Y@$6 'X'!N<!%@"#!P;H 18 CP<&Z0$6 *P'!NH!%@#%!P;L 18 W@<$
XM!E-44DE.1UL#T  #!%1)344$"$1!5$5?0DE.E0 )  0(5$E-15]"24Z5    
XM PA?34%)3E8R3 0%7TU!24Z6  ( ! 1!4D=#6P-0!00$05)'5EL#4@4#!5-4
XM04-+! 9?4U1!0TM; ^ % P904DE.5$8$!E!224Y41J<  P #"$9/4$5.5C),
XM! 5&3U!%3KP  P $!T9214]014Z\ '< ! 9&0TQ/4T6\ &D"! 1?24]"6P/P
XM!0,'1E!224Y41@0'1E!224Y41NH "@ #!$58250$!$58250! 0D  P5#5%E0
XM100&7T-465!%6P-@!P,'24]3,58R3 0$3U!%3@@!5  $!4-214%4" %> @0$
XM4D5!1 @!D@($!5=2251%" '@!00%3%-%14L( ?L'! 5#3$]310@!@ @$!E5.
XM3$E.2P@!Y @$!5]51D)36P-P" 0%7T1.0E-; \ ( P=)3U,P5C),! 9?1D-(
XM1TV8 =( ! =?1D-,3U-%F $\  0'7T9#4D5!5)@!!  $!5]&1T1)F 'S  0&
XM7T9/4$5.F $@  0&7T9214%$F %3  0%7T9235:8 ;D ! 9?1E-%14N8 94 
XM! =?1E=2251%F %T  0&7T]315)26P- "@,$24]3,@0&7T9)3$)&J $"  0&
XM7T9,4T)&J %( 0,%7U!&350$!5]01DU4Z $(  ,%1DU/1$4$!E]&34]$15L#
XM8 H#!$U%33($!E)35$U%37$"#@ $!D%,3$U%37$";@ $!D),1$U%37$"C0 $
XM!E-)6DU%37$"/P$$!D=%5$U%37$"90$$!4=%5$U,<0*3 00&4DQ3345-<0)A
XM P0%4DQ334QQ I #! 5?4$]/3%L#< H$!5]-14Q46P-X"@,%7T58250$!5]%
XM6$E4U (+  ,%15)23U($!4524DY/6P. "@,&24]-3T1%! =?24]-3T1%6P.0
XM"@,&4U120TU0! 935%)#35#6 @   P9?0T=%5%,$!E]#1T544]L"!@ #!$E/
XM4S $!5]#1T54W ($  0&7T-'151%W (^  0%7T-0553< G@ ! 5?04=%5-P"
XMI  $!5]!4%54W +%  0%7TQ0553< O$  P9"549325H$!U]"549325I; Z *
XM P5#6$0S,P0%0UA$,S/M @T  P5#6%9&1 0%0UA61$;Y I4!! 5#6%9&1/D"
XM   #!$U%33$$!4Q30E)+.@,*  0$4T)22SH#=@ $!%)"4DLZ [L  P5#6$Y-
XM. 0%0UA.33A) PD  P5#6$,S,P0%0UA#,S-, P<  P5#6$$S. 0%0UA!,SA.
XM P0  P5#6$,X. 0%0UA#.#A1 P(  P1"1$]3! 1"1$]350,'  ,%0UA3.#@$
XM!4-84S@X5@,)  ,%0UA6.#,$!4-85C@S60,"  ,'1$5&24Y%<P0&)%-44E0D
X(   "  <    )
X 
Xend
SHAR_EOF
if test 27318 -ne  ` wc -c < cal_exe_uu `
then
	echo shar: error transmitting 'cal_exe_uu' -- should have been 27318 characters
fi
fi
# end of shell archive
exit 0
-- 
Ed Post -- hplabs!lewey!evp
American Information Technology    (408)252-8713


