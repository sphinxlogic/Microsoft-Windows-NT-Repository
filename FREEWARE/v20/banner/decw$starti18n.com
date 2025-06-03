$ !
$ ! DECW$STARTI18N.COM - Set up logical names for translated VMS
$ !
$ !****************************************************************************
$ !*									      *
$ !*  COPYRIGHT (c) 1989 BY						      *
$ !*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		      *
$ !*  ALL RIGHTS RESERVED.						      *
$ !* 									      *
$ !*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
$ !*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
$ !*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
$ !*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
$ !*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
$ !*  TRANSFERRED.							      *
$ !* 									      *
$ !*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
$ !*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
$ !*  CORPORATION.							      *
$ !* 									      *
$ !*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
$ !*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		      *
$ !* 									      *
$ !*									      *
$ !****************************************************************************
$ !
$ ! Create a logical name for each of the VMS language specific directories
$ ! that is being used for translation.
$ !
$ !	P1 = comma separated list of languages (e.g., "FR_FR, DE_DE")
$ !	P2 = default system language (other than the implicit American English)
$ !	P3 = optional qualifiers to use for logical name definition
$ !	     (e.g., use "/PROCESS" or "/JOB" to override "SYSTEM")
$ !
$ IF p1 .EQS. ""
$ THEN
$ 	WRITE SYS$OUTPUT "No language was specified - DECwindows internationalization startup aborted."
$ 	EXIT
$ ENDIF
$ p1 = F$EDIT(p1,"COLLAPSE,UPCASE")
$
$ IF p3 .EQS. "" THEN p3 = "/SYSTEM"
$
$ IF F$TRNLNM("DECW$TESTI18N")
$ THEN
$ 	WRITE SYS$OUTPUT p1," language(s) specified."
$ 	WRITE SYS$OUTPUT "DEFINE",p3," command will be used."
$ ENDIF
$
$ ! Set up the default system language
$ !
$ IF p2 .NES. ""
$ THEN
$ 	IF F$TRNLNM("DECW$TESTI18N")
$ 	THEN
$ 		WRITE SYS$OUTPUT "Defining XNL$LANG to be: ",p2
$ 	ELSE
$ 		define/nolog 'p3' xnl$lang 'p2'
$ 	ENDIF
$ ENDIF
$
$ ! Do each language in the list
$ !
$ lang_index = 0
$next_language:
$ lang = F$ELEMENT(lang_index,",",p1)
$ IF lang .EQS. "," THEN GOTO last_lang
$
$ ! These are the logical names that need to be modified by adding these
$ ! language-specific directory specs as first values in the search list.
$ !
$ dir_list = "SYSHLP.EXAMPLES,SYSHLP,SYSLIB,SYSMSG"
$ name_list = "SYS$EXAMPLES,SYS$HELP,SYS$LIBRARY,SYS$MESSAGE"
$
$ ! Do each logical name.
$ !
$ name_index = 0
$next_name:
$ dir = F$ELEMENT(name_index ,",",dir_list)
$ name = F$ELEMENT(name_index ,",",name_list)
$ IF dir .EQS. "," THEN GOTO last_name
$ value = "SYS$COMMON:[" + dir + "." + lang + "]"
$ name = name + "_" + lang
$ IF F$TRNLNM("DECW$TESTI18N")
$ THEN
$ 	WRITE SYS$OUTPUT "Defining ",name," to be:"
$	WRITE SYS$OUTPUT "    ",value
$ ELSE
$ 	define/nolog 'p3' 'name' 'value'
$ ENDIF
$
$skip_logical:
$ name_index  = name_index  + 1
$ GOTO next_name
$last_name:
$
$ ! Handle DECW$SYSTEM_DEFAULTS as a special case for now.
$ !
$ name = "DECW$SYSTEM_DEFAULTS_" + lang
$ value = "SYS$COMMON:[DECW$DEFAULTS.USER." + lang + "]," + -
	"SYS$COMMON:[DECW$DEFAULTS.SYSTEM." + lang + "]"
$ IF F$TRNLNM("DECW$TESTI18N")
$ THEN
$ 	WRITE SYS$OUTPUT "Defining ",name," to be:"
$	WRITE SYS$OUTPUT "    ",value
$ ELSE
$ 	define/nolog 'p3' 'name' 'value'
$ ENDIF
$
$ ! Handle VUE$LIBRARY as a special case for now.
$ !
$ name = "VUE$LIBRARY_" + lang
$ value = "SYS$COMMON:[VUE$LIBRARY.USER." + lang + "]," + -
	"SYS$COMMON:[VUE$LIBRARY.USER]," + -
	"SYS$COMMON:[VUE$LIBRARY.SYSTEM." + lang + "]," + -
	"SYS$COMMON:[VUE$LIBRARY.SYSTEM]"
$ IF F$TRNLNM("DECW$TESTI18N")
$ THEN
$ 	WRITE SYS$OUTPUT "Defining ",name," to be:"
$	WRITE SYS$OUTPUT "    ",value
$ ELSE
$ 	define/nolog 'p3' 'name' 'value'
$ ENDIF
$
$ lang_index = lang_index + 1
$ GOTO next_language
$last_lang:
$ EXIT 
