$ !
$ ! LLVINSTAL.COM - Create VMS specific directories and logicals for language switching 
$ !
$ !	Execution call 
$ !
$ !		@VMI$KWD:LLVINSTAL.COM  [VMI$KWD:LLVINSTALLANGUAGE.DAT]
$ !
$ !	The option parameter specifies the filename of the .DAT file to be 
$ !	used by LLVINSTAL.COM. If omitted, the default filename used is  
$ !	VMI$KWD:LLVINSTAL.DAT
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
$
$
$ !
$ ! Process the local language database file. 
$ !
$ ! Define the installation symbols here so that they can be overridden
$ ! within the installation data file.
$ !
$I18N$LANG_CULTURE = ""
$ i18n$dir_list = -
"decw$defaults.system,decw$defaults.user,syshlp.examples,syshlp,syslib,sysmsg,vue$library.system,vue$library.user"
$i18n$vmi_list = -
"vmi$lv_decw_defaults_system,vmi$lv_decw_defaults_user,vmi$lv_sysexamples,vmi$lv_syshlp,vmi$lv_syslib,vmi$lv_sysmsg,vmi$lv_vue_library_system,vmi$lv_vue_library_user"
$
$ !
$ ! Allow translators to overide the name of the data file, so we can have 
$ ! multiple cultural information in each kit. eg: DE_DE, DE_CH etc
$ !
$ IF P1 .EQS. "" THEN P1 = "VMI$KWD:LLVINSTAL.DAT"
$ ! Open the data file.
$ !
$ OPEN/READ i18n$data 'P1
$
$ ! Get and process the next record in the file.
$ !
$next_data_record:
$ READ/END=close_data_file i18n$data i18n$record
$ i18n$record = F$EDIT(i18n$record,"COMPRESS,TRIM,UPCASE")
$
$ ! Ignore comments.
$ !
$ IF F$EXTRACT(0,1,i18n$record) .EQS. "!" THEN GOTO next_data_record
$
$ ! Process commands.  Most of these should simply by i18n$xxx symbol
$ ! definitions.
$ !
$ IF F$EXTRACT(0,1,i18n$record) .EQS. "$" 
$ THEN 
$ 	i18n$record = i18n$record - "$"
$ 	'i18n$record'
$ 	GOTO next_data_record
$ ENDIF
$
$ ! Close the file.
$ !
$close_data_file:
$ CLOSE i18n$data
$ !
$ ! Create the necessary directories for this language and culture.
$ !
$ i = 0
$next_dir:
$ i18n$dir = F$ELEMENT(i,",",i18n$dir_list)
$ i18n$vmi_symbol = F$ELEMENT(i,",",i18n$vmi_list)
$ IF i18n$dir .EQS. "," THEN GOTO last_dir
$ IF i18n$lang_culture .EQS. ""
$ THEN
$	i18n$cr_dir = "vmi$root:[''i18n$dir']" 
$ ELSE
$	i18n$cr_dir = "vmi$root:[''i18n$dir'.''i18n$lang_culture']"
$ ENDIF
$ IF i18n$lang_culture .NES. "" .AND. -
	F$SEARCH("vmi$root:[" + i18n$dir + "]" + i18n$lang_culture + ".dir;") .EQS. ""
$ THEN 
$	IF .NOT. F$TRNLNM("decw$testi18n")
$	THEN 
$		vmi$callback create_directory user vmi$root:['i18n$dir'.'i18n$lang_culture']
$	ELSE
$		WRITE SYS$OUTPUT "Create vmi$root:[''i18n$dir'.''i18n$lang_culture'] directory"
$	ENDIF
$ ENDIF
$ IF .NOT. F$TRNLNM("decw$testi18n") 
$ THEN 
$ 	define 'i18n$vmi_symbol' 'i18n$cr_dir'
$ ELSE
$ 	WRITE SYS$OUTPUT "Defining ''i18n$vmi_symbol' to be ''i18n$cr_dir'"
$ ENDIF
$ i = i + 1
$ GOTO next_dir
$last_dir:
$
$
$ !
$ ! Execute the DECW$STARTI18N.COM, in /JOB mode so the specific langauge 
$ ! logical names are defined for this process. Do this before moving the 
$ ! file incase we are no running safety mode.
$ !
$ IF i18n$lang_culture .NES. "" THEN -
	@VMI$KWD:DECW$STARTI18N.COM "''I18N$LANG_CULTURE'" "" "/JOB"
$ !
$ ! Provide the startup file and be done.
$ !
$ ! (Actually, we want to make this generic i18n startup file part
$ !  of the base kit.  That way it only ships once for the n variants.)
$ !
$ IF i18n$lang_culture .NES. "" .AND. -
	F$SEARCH("vmi$root:[sysmgr]decw$starti18n.com") .EQS. "" 
$ THEN
$	IF .NOT. F$TRNLNM("decw$testi18n")
$	THEN 
$		vmi$callback provide_file  i18n$ decw$starti18n.com vmi$root:[sysmgr]
$	ELSE
$		WRITE SYS$OUTPUT "DECW$STARTI18N.COM installed in VMI$ROOT:[SYSMGR]"
$	ENDIF
$ ENDIF
$ EXIT vmi$_success
