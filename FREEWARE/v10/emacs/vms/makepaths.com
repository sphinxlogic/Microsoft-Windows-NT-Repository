$! Generated automatically from MAKEPATHS.COM_IN by configure.
$! MAKEPATHS.COM_IN -- The template for a script to install the binaries.
$!
$! This file does the exact same thing as if you would have written
$! MMS [.SRC]PATHS.H,[.VMS]VMSLINK.OPT,[.VMS]EMACS_STARTUP.DAT,-
$!	[.VMS]REBUILD.COM
$
$ save_default = f$environment("DEFAULT")
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ set def 'proc_dir'
$
$ version := 19.22
$ version_us := 19_22
$ configuration := ALPHA-DEC-VMS6-1
$ libsrc_libs := SYS$SHARE:DECC$SHR/SHARE
$
$ prefix_dev := EMACS_LIBRARY:
$ prefix_dir := 000000
$ prefix := EMACS_LIBRARY:[000000]
$ exec_prefix_dev := EMACS_LIBRARY:
$ exec_prefix_dir := 000000
$ exec_prefix := EMACS_LIBRARY:[000000]
$
$ datadir_dev := EMACS_LIBRARY:
$ datadir_dir := 000000.COMMON
$ datadir := EMACS_LIBRARY:[000000.COMMON]
$ statedir_dev := EMACS_LIBRARY:
$ statedir_dir := 000000.COMMON
$ statedir := EMACS_LIBRARY:[000000.COMMON]
$ libdir_dev := EMACS_LIBRARY:
$ libdir_dir := 000000.BIN.ALPHA-DEC-VMS6-1
$ libdir := EMACS_LIBRARY:[000000.BIN.ALPHA-DEC-VMS6-1]
$
$ bindir := EMACS_LIBRARY:[000000.BIN.ALPHA-DEC-VMS6-1]
$ mandir :=EMACS_LIBRARY:[000000.HELP]
$ infodir := EMACS_LIBRARY:[000000.INFO]
$
$ lispdir := EMACS_LIBRARY:[000000.COMMON.LISP]
$ lisppath := EMACS_LIBRARY:[000000.COMMON.SITE-LISP],EMACS_LIBRARY:[000000.COMMON.LISP]
$ locallisppath := EMACS_LIBRARY:[000000.COMMON.SITE-LISP]
$ archlibdir := EMACS_LIBRARY:[000000.BIN.ALPHA-DEC-VMS6-1.ETC]
$ vmslibdir := EMACS_LIBRARY:[000000.BIN.ALPHA-DEC-VMS6-1.VMS]
$ etcdir := EMACS_LIBRARY:[000000.COMMON.DATA]
$ lockdir := EMACS_LIBRARY:[000000.COMMON.LOCK]
$ startupdir := EMACS_LIBRARY:[SYSCOMMON.SYS$STARTUP]
$ vuelibdir := EMACS_LIBRARY:[SYSCOMMON.VUE$LIBRARY.USER]
$ LOADLIBES := SYS$SHARE:DECC$SHR/SHARE
$
$ vmssrcdir_dev := EMACS_LIBRARY:
$ vmssrcdir_dir := 000000.VMS
$ vmssrcdir := EMACS_LIBRARY:[000000.VMS]
$
$ __result = ""
$ delete/symbol/local __result
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-.src] __result
$ srcdir = __result
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-] __result
$ rootdir = __result
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-.lisp] __result
$ buildlisppath  = "[-.lisp]," + __result
$ lispsrcdir = __result
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-.etc] __result
$ etcsrcdir = __result
$
$! This does the same as MMS [.VMS]ROUTINES.TPU$SECTION
$!
$ save_message = f$environment("MESSAGE")
$ set message/nofacility/noidentification/noseverity/notext
$ edit/tpu/nosect/nodisp -
	/command='vmssrcdir'routines.tpu -
	/out=sys$disk:[]routines.tpu$section
$ set message 'save_message'
$
$! This does the same as MMS [.SRC]PATHS.H
$!
$ write sys$output "Producing `[-.src]paths.h' from `",srcdir,"paths.h_in'."
$ open/write foo paths.tpu_tmp
$ write foo "lisppath := removenullpaths (""",lisppath,""");"
$ write foo "buildlisppath := removenullpaths (""",buildlisppath,""");"
$ write foo "input_file:=GET_INFO(COMMAND_LINE, ""file_name"");"
$ write foo "main_buffer := CREATE_BUFFER (""main"", input_file);
$ write foo "p_0_or_more_spc := (span("" ""+ASCII(9)) | """");"
$ write foo "position (beginning_of (main_buffer));
$ write foo "TPU_substitute(LINE_BEGIN + ""#"" + unanchor +"
$ write foo "               ""PATH_LOADSEARCH"","
$ write foo "               unanchor, LINE_END,"
$ write foo "               "" """""" + lisppath + """""""");"
$ write foo "TPU_substitute(LINE_BEGIN + ""#"" + unanchor +"
$ write foo "               ""PATH_DUMPLOADSEARCH"","
$ write foo "               unanchor, LINE_END,"
$ write foo "               "" """""" + buildlisppath + """""""");"
$ write foo "TPU_substitute(LINE_BEGIN + ""#"" + unanchor +"
$ write foo "               ""PATH_EXEC"","
$ write foo "               unanchor, LINE_END,"
$ write foo "               "" """""" + """,archlibdir,""" + """""""");"
$ write foo "TPU_substitute(LINE_BEGIN + ""#"" + unanchor +"
$ write foo "               ""PATH_INFO"","
$ write foo "               unanchor, LINE_END,"
$ write foo "               "" """""" + """,infodir,""" + """""""");"
$ write foo "TPU_substitute(LINE_BEGIN + ""#"" + unanchor +"
$ write foo "               ""PATH_DATA"","
$ write foo "               unanchor, LINE_END,"
$ write foo "               "" """""" + """,etcdir,""" + """""""");"
$ write foo "TPU_substitute(LINE_BEGIN + ""#"" + unanchor +"
$ write foo "               ""PATH_LOCK"","
$ write foo "               unanchor, LINE_END,"
$ write foo "               "" """""" + """,lockdir,""" + """""""");"
$ write foo "write_file(main_buffer, get_info (command_line, ""output_file""));"
$ write foo "quit;"
$ close foo
$ save_mesg = f$environment("MESSAGE")
$ set message/nofacility/noidentification/noseverity/notext
$ edit/tpu/section=sys$disk:[]routines.tpu$section/nodisplay -
	/command=sys$disk:[]paths.tpu_tmp/out=[-.src]paths.h_tmp -
	'srcdir'paths.h_in
$ delete paths.tpu_tmp;*
$ set message 'save_mesg'
$ @'rootdir'move-if-change [-.src]paths.h_tmp [-.src]paths.h
$
$! This does the same as MMS [.VMS]RUNTEMACS.COM
$!
$ write sys$output "Producing `[.vms]runtemacs.com'."
$ open/write foo [.vms]runtemacs.com_tmp
$ write foo "$! This is a subcommand to testemacs.com."
$ write foo "$! It was automatically generated."
$ write foo "$! Do NOT run this directly.  Instead, run testemacs.com"
$ write foo "$! and use the symbols `runtemacs' and `runtemacs_d'."
$ write foo "$ proc_dir = f$parse(""A.;0"",f$environment(""PROCEDURE"")) - ""A.;0"""
$ lisp_dir = lispsrcdir
$ termcap_dir = etcsrcdir
$ a = f$environment("DEFAULT")
$ set default [.lib-src]
$ lib_src_dir = f$environment("DEFAULT")
$ set default [-.etc]
$ etc_dir = f$environment("DEFAULT")
$ set default [-.info]
$ info_dir = f$environment("DEFAULT")
$ set default 'a'
$ write foo "$ args = """""
$ write foo "$ if args .nes. """" .or. p8 .nes """" then args = """""""" + p8 + """""" "" + args"
$ write foo "$ if args .nes. """" .or. p7 .nes """" then args = """""""" + p7 + """""" "" + args"
$ write foo "$ if args .nes. """" .or. p6 .nes """" then args = """""""" + p6 + """""" "" + args"
$ write foo "$ if args .nes. """" .or. p5 .nes """" then args = """""""" + p5 + """""" "" + args"
$ write foo "$ if args .nes. """" .or. p4 .nes """" then args = """""""" + p4 + """""" "" + args"
$ write foo "$ if args .nes. """" .or. p3 .nes """" then args = """""""" + p3 + """""" "" + args"
$ write foo "$ if args .nes. """" .or. p2 .nes """" then args = """""""" + p2 + """""" "" + args"
$ write foo "$ define/user EMACSLOADPATH ""''lisp_dir',''lisppath'"""
$ write foo "$ define/user EMACSPATH ""''lib_src_dir'"""
$ write foo "$ define/user EMACSDATA ""''etc_dir'"""
$ write foo "$ define/user TERMCAP ""''termcap_dir'TERMCAP.DAT"""
$ write foo "$ define/user INFOPATH ""''info_dir'"""
$ write foo "$ define/user SYS$INPUT SYS$COMMAND"
$ write foo "$ mcr 'proc_dir'temacs'p1' -map 'proc_dir'temacs'p1'.dump 'args'"
$ write foo "$ exit"
$ close foo
$ @$(srcdir)move-if-change [.vms]runtemacs.com_tmp [.vms]runtemacs.com
$
$! This does the same as MMS [.VMS]VMSLINK.OPT
$!
$ write sys$output "Producing `vmslink.opt'."
$ open/write foo vmslink.opt_tmp
$ @'vmssrcdir'loop_args lib "''LOADLIBES'" " " "write foo lib" ";"
$ close foo
$ @'rootdir'move-if-change vmslink.opt_tmp vmslink.opt
$
$! This does the same as MMS [.VMS]CONFIG.DAT
$!
$ write sys$output "Producing `config.dat'."
$ open/write foo config.dat_tmp
$ write foo "@"+"version"+"@"
$ write foo version
$ write foo "@"+"configuration"+"@"
$ write foo configuration
$ write foo "@"+"libsrc_libs"+"@"
$ write foo libsrc_libs
$ write foo "@"+"bindir"+"@"
$ write foo bindir
$ write foo "@"+"datadir"+"@"
$ write foo datadir
$ write foo "@"+"statedir"+"@"
$ write foo statedir
$ write foo "@"+"libdir"+"@"
$ write foo libdir
$ write foo "@"+"mandir"+"@"
$ write foo mandir
$ write foo "@"+"infodir"+"@"
$ write foo infodir
$ write foo "@"+"scrdir"+"@"
$ write foo srcdir
$ write foo "@"+"vmssrcdir"+"@"
$ write foo vmssrcdir
$ write foo "@"+"lispdir"+"@"
$ write foo lispdir
$ write foo "@"+"locallisppath"+"@"
$ write foo locallisppath
$ write foo "@"+"lisppath"+"@"
$ write foo lisppath
$ write foo "@"+"buildlisppath"+"@"
$ write foo buildlisppath
$ write foo "@"+"etcdir"+"@"
$ write foo etcdir
$ write foo "@"+"lockdir"+"@"
$ write foo lockdir
$ write foo "@"+"archlibdir"+"@"
$ write foo archlibdir
$ write foo "@"+"vmslibdir"+"@"
$ write foo vmslibdir
$ write foo "@"+"startupdir"+"@"
$ write foo startupdir
$ write foo "@"+"vuelibdir"+"@"
$ write foo vuelibdir
$ write foo "@"+"all_logicals"+"@"
$ __result = ""
$ __mode = ""
$ delete/symbol/local __result
$ delete/symbol/local __mode
$ lnm = f$parse(bindir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(datadir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(statedir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(libdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(mandir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(infodir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(srcdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(vmssrcdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(lispdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(etcdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(lockdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(archlibdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(vmslibdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(startupdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ lnm = f$parse(vuelibdir,,,"DEVICE")-":"
$ @'vmssrcdir'decomplnm 'lnm' __result DIRS,ATTRIBUTES __mode
$ flag = (__result .nes. "")
$ if flag then write foo " D\",lnm,"\",__mode,"\ -"
$ if flag then @'vmssrcdir'loop_sym v __result "," "if next_v .nes. "","" then write foo ""    "",v,"",-"";if next_v .eqs. "","" then write foo ""    "",v" ";" next_v
$ close foo
$ @'rootdir'move-if-change config.dat_tmp config.dat
$
$! This does the same as MMS [.VMS]EMACS_STARTUP.DAT
$!
$ write sys$output "Producing `emacs_startup.dat' from `",vmssrcdir,"emacs_startup.dat_in' and config.dat."
$ @'vmssrcdir'gnu_generate_data 'vmssrcdir'emacs_startup.dat_in -
		config.dat emacs_startup.dat_tmp
$ @'rootdir'move-if-change emacs_startup.dat_tmp -
		emacs_startup.dat
$
$! This does the same as MMS [.VMS]REBUILD.COM
$!
$ write sys$output "Producing `rebuild.com'."
$ sdir = f$environment("DEFAULT")
$ sdev = f$parse(sdir,,,"DEVICE")
$ __result = ""
$ __mode = ""
$ delete/symbol/local __result
$ delete/symbol/local __mode
$ @'vmssrcdir'decomplnm 'sdev' __result DIRS,ATTRIBUTES __mode
$ open/write foo rebuild.com_tmp
$ write foo "$ ! VMS command file to run `temacs.exe' and dump the data file `temacs.dump'."
$ write foo "$ define ",sdev-":",__mode,"/PROCESS -"
$ @'vmssrcdir'loop_sym e __result "," "if newe .eqs. "","" then write foo e;if newe .nes. "","" then write foo e,"",-""" ";" newe
$ write foo "$ objdir := ",sdir
$ write foo "$ proc = f$environment(""PROCEDURE"")"
$ write foo "$ proc_dir = f$parse(proc,,,""NODE"")+f$parse(proc,,,""DEVICE"")+f$parse(proc,,,""DIRECTORY"")"
$ write foo "$ save_default = f$environment(""DEFAULT"")"
$ write foo "$ set def 'objdir'"
$ write foo "$ temacs :== $",bindir,"emacs -batch"
$ write foo "$ temacs -l loadup.el dump nodoc"
$ write foo "$ rename ",bindir,"emacs.dump emacs-",version_us,".dump"
$ write foo "$ purge/keep=2 ",bindir,"emacs-",version_us,".dump"
$ write foo "$ if f$search(""",bindir,"emacs.dump"") .nes. """" then -"
$ write foo "	set file/remove ",bindir,"emacs.dump;*"
$ write foo "$ set file/enter=",bindir,"emacs.dump ",bindir,"emacs-",version_us,".dump"
$ write foo "$ set default 'save_default'"
$ write foo "$ exit"
$ close foo
$ @'rootdir'move-if-change rebuild.com_tmp rebuild.com
$
$ set default 'save_default'
$ exit
