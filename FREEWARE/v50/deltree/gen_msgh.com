$! Program Name            : GEN_MSGH.COM
$!   Original Author       : JLAURET
$!   Date                  : 03-Jun-1999
$!   Program Description   : This script generates .H according
$!                         : to .MSG content. called by DECSRIP.MMS
$!
$!   Usage 		   : @GEN_MSGH filin filout Prefix
$! 
$! Note  This produced .H file autoinclude <ssdef.h> and <stsdef.h>
$! 
$! Revision History follows
$
$ open/read  filin  'P1'
$ open/write filout 'P2'
$
$ write filout "/*"
$ write filout "	AutoGenerated on ",f$time()
$ write filout ""
$ if f$search("COPYRIGHT.;").nes.""
$ then
$ 	open/read ftmp COPYRIGHT.;
$ 	LREAD:
$ 	read/end=EREAD ftmp ligne
$ 	write filout ligne
$ 	goto LREAD
$ 	EREAD:
$ endif
$ write filout "*/"
$ write filout ""
$ write filout "#include <ssdef.h>"
$ write filout "#include <stsdef.h>"
$ write filout ""
$ write filout "#ifdef __STDC__"
$ write filout "#define __GNU_HACKS_string(foo) #foo"
$ write filout "#define __GNU_HACKS_const __const"
$ write filout "#else   "
$ write filout "#define __GNU_HACKS_string(foo) ""foo"""
$ write filout "#define __GNU_HACKS_const"
$ write filout "#endif"
$ write filout ""
$ write filout "#ifdef __GNUC__"
$ write filout "# define GLOBALREF(TYPE,NAME) 		TYPE NAME __asm(""_$$PsectAttributes_GLOBALSYMBOL$$"" __GNU_HACKS_string(NAME))"
$ write filout "# define GLOBALDEF(TYPE,NAME,VALUE) 	TYPE NAME __asm(""_$$PsectAttributes_GLOBALSYMBOL$$"" __GNU_HACKS_string(NAME)) = VALUE"
$ write filout "# define GLOBALVALUEREF(TYPE,NAME) 	__GNU_HACKS_const TYPE NAME[1] __asm(""_$$PsectAttributes_GLOBALVALUE$$"" __GNU_HACKS_string(NAME))"
$ write filout "# define GLOBALVALUEDEF(TYPE,NAME,VALUE) __GNU_HACKS_const TYPE NAME[1] __asm(""_$$PsectAttributes_GLOBALVALUE$$"" __GNU_HACKS_string(NAME)) = {VALUE}"
$ write filout "#else"
$ write filout "# define GLOBALREF(TYPE,NAME)		globalref TYPE NAME"
$ write filout "# define GLOBALDEF(TYPE,NAME,VALUE)	globaldef TYPE NAME = VALUE"
$ write filout "# define GLOBALVALUEDEF(TYPE,NAME,VALUE) globalvalue TYPE NAME = VALUE"
$ write filout "# define GLOBALVALUEREF(TYPE,NAME)	globalvalue TYPE NAME"
$ write filout "#endif"
$ write filout ""
$ write filout "#pragma nostandard"
$
$ LOOP1:
$ 	read/end=ENDLOOP1 filin ligne
$ 	ligne	= f$edit(ligne,"COMPRESS")
$ 	ligne	= f$edit(ligne,"TRIM")
$ 	char	= f$extract(0,1,ligne)
$ 	symb	= f$element(0," ",ligne)
$
$ 	if char.nes."." .AND. char.nes."!" .AND. ligne.nes.""
$ 	then	write filout "	GLOBALVALUEREF(unsigned int,''p3'''symb');"
$ 	endif
$ GOTO LOOP1
$
$
$ ENDLOOP1:
$ close filin
$ open filin 'P1'
$ write filout "#pragma standard"
$ write filout ""
$ write filout ""
$ write filout "/*"
$ write filout " There are 2 ways one can implement the signal handler within both GCC and DEC-C :"
$ write filout " 1) Still use lib$signal() --> if you get it to work with GCC, let me know"
$ write filout " 2) Use hack$signal with the _msg_hack symbols."
$ write filout "*/"
$ write filout ""
$ write filout ""
$ write filout "#ifndef __GNUC__"
$ write filout "	/* Use lib$signal() and hack symbols	*/"
$ write filout "#	define hack$signal	 lib$signal"
$ write filout "#	define hack$establish(a) lib$establish(a)"
$ write filout "#	define hack$revert()	 lib$revert()"
$ write filout "#	define hack$exit(a) 	 exit(a)"
$ write filout ""
$
$ LOOP2:
$ 	read/end=ENDLOOP2 filin ligne
$ 	ligne	= f$edit(ligne,"COMPRESS")
$ 	ligne	= f$edit(ligne,"TRIM")
$ 	char	= f$extract(0,1,ligne)
$ 	symb	= f$element(0," ",ligne)
$
$ 	if char.nes."." .AND. char.nes."!" .AND. ligne.nes.""
$ 	then	write filout "#	define ''p3'''symb'_msg_hack ''p3'''symb'"
$ 	endif
$ GOTO LOOP2
$
$
$ ENDLOOP2:
$ close filin
$ open filin 'P1'
$ write filout "#else"
$ write filout "	/* Extraneous #define is for GCC support only	*/"
$ write filout "#	define hack$signal	''p3'signal"
$ write filout "#	define hack$exit(a)     {hack$signal(a); exit(SS$_NORMAL);}"
$ write filout "#	define hack$establish(a) /* a */"
$ write filout "#	define hack$revert() 	/* Missing $revert at link time on AXP */"
$ write filout "#if !defined($VMS_STATUS_SUCCESS)"
$ write filout "#	define $VMS_STATUS_SUCCESS(code) (((code)&STS$M_SUCCESS)  >>STS$V_SUCCESS)"
$ write filout "#endif"
$ write filout ""
$
$ LOOP3:
$ 	read/end=ENDLOOP3 filin ligne
$ 	ligne	= f$edit(ligne,"COMPRESS")
$ 	ligne	= f$edit(ligne,"TRIM")
$ 	char	= f$extract(0,1,ligne)
$ 	if char.eqs."."
$ 	then	severity = f$extract(0,1,f$element(1," ",ligne))
$ 		severity = f$edit(severity,"UPCASE")
$ 		goto LOOP3
$ 	endif
$
$ 	symb	= f$element(0," ",ligne)
$ 	value	= f$extract(f$locate("<",ligne),f$length(ligne),ligne)
$ 	value	= f$element(0,"/",value) - "<" - ">"
$ 	fao	= f$element(1,"=",f$extract(f$locate(">",ligne),f$length(ligne),ligne) - ">")
$ 	if fao.eqs."=" 
$ 	then fao = ",0"	! Fake this lib$signal() argument passing.
$ 	else fao = ""	! send nothing.
$ 	endif
$
$ 	SLOOP:
$ 		pos	= f$locate("!",value)
$ 		if pos.lt.f$length(value)
$ 		then	case = f$extract(pos,3,value)
$ 			if case.eqs."!AZ" then case = "%s"
$ 			if case.eqs."!UL" then case = "%ld"
$ 			if case.eqs."!SL" then case = "%d"
$ 			if case.eqs."!XL" then case = "%X"
$ 			if case.eqs."!%S" then case = "(s)"
$ 			value = f$extract(0,pos,value)+case+f$extract(pos+3,f$length(value),value)
$ 			goto SLOOP
$ 		endif
$
$ 	if char.nes."." .AND. char.nes."!" .AND. ligne.nes.""
$ 	then	write filout "#	define ''p3'''symb' ((unsigned int) ''p3'''symb')"
$ 		write filout "#	define ''p3'''symb'_msg_hack ""''severity'"",""''symb'"",""''value'""''fao'"
$ 	endif
$ GOTO LOOP3
$
$
$ ENDLOOP3:
$ nam= p3 - "_" - "_"
$ write filout ""
$ write filout ""
$ write filout "#ifndef ''nam'_SIGNAL_ROUTINE_HEADER"
$ write filout "#define ''nam'_SIGNAL_ROUTINE_HEADER"
$ write filout "/* provided ''p3'signal routine	*/"
$ write filout "#include <stdarg.h>"
$ write filout "#include <stdio.h>"
$ write filout "#include <stdlib.h>"
$ write filout "void	''p3'signal(char *sev,char *typ,char *mess,int id, ...)"
$ write filout "{"
$ write filout " char buf[2048];"
$ write filout " va_list args;"
$ write filout ""
$ write filout " va_start(args,id);"
$ write filout " (void) vsprintf(buf,mess,args);"
$ write filout " (void) fprintf(stderr,""%%''nam'-%s-%s, %s\n"",sev,typ,buf);"
$ write filout " if (*sev == 'E') abort();"
$ write filout " if (*sev == 'F') abort();"
$ write filout "}"
$ write filout "#endif"
$ write filout ""
$ write filout "#endif	/* GNUC or not	*/"
$ write filout ""
$ close filin
$ close filout
$ 
$! ----------------------- : 
$!   Date of Revision      : 30-JUN-1999 
$!   Change Author         : JLAURET
$!   Purpose of Revision   : Addded second hack$ possibility to circunvent GCC 
$!                         : lib$establish()/lib$revert()/lib$signal() problems.
$ 
$! ----------------------- : 
$!   Date of Revision      : 17-JUL-1999 
$!   Change Author         : JLAURET
$!   Purpose of Revision   : Added hack$exit() for consistency.
$!                         : 
