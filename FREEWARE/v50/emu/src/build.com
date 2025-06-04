$ set noveri
$ver=F$ver()	!	--- This command file ---
$!	This is a build command file. The file works using two data files 
$!	defined by logical names SUB_FILES and PROG_FILES. These files
$!	contain the names of the subroutines/programs making up the system.
$!	In these files are the names of the files to be operated on. 
$!	The compiler is assumed to be FORTRAN unless a filetype is specified.
$!	The names within the data files can be ordered to suit the User. 
$!	Each input line can only contain one file name and comments are
$!	allowed indicated by a "!" mark. Blank lines or comment lines are 
$!	ignored.
$!	
$!	Thrown together by K. O'Brien on the 17th July 1985.
$!	Upgraded 22nd December 1987.
$!	Modified 08th April 1990 to add .SQLMOD
$!	Modified 09th Sep   1993 to add /R/(rest) to programs
$!				    and delete list files
$!				    deass exe_dir from previous usage
$!	Modified 12th April 1995 to put programs in object library
$!
$! Assign local directory logical names for
$! 	TARGET_DIR	source directory spec
$!	EXE_DIR		executable directory spec
$!	OBJECT_LIB	object library filename
$!	SUB_FILES	file containing subroutine filenames
$!	PROG_FILES	file containing program filenames
$	build_logs = f$search ("build_logicals.com")
$	if "''build_logs'" .nes. "" then goto set_logicals
$	write sys$output "Build_logicals.com file not present"
$	exit
$set_logicals:
$	write sys$output "Logical assignments taken from ''build_logs'"
$	if f$trnlnm("exe_dir").nes. "" then deass exe_dir
$	@build_logicals
$	sh log target_dir, exe_dir, object_lib, sub_files, prog_files
$	exe_direc = f$trnlnm ("EXE_DIR")
$! If a file exists on this directory called linkopts.opt then it is used 
$! in the link operation.
$	options := ""
$	if f$getsyi("NODE_HWTYPE") .eqs. "ALPH"
$	then
$	   link_opts = f$search ("linkopts.opt_axp")
$	else
$	   link_opts = f$search ("linkopts.opt")
$	endif
$	if "''link_opts'" .nes. "" then options := ",''link_opts'/opt"
$	write sys$output "Link options file will be ""''options'"""
$
$!
$		DBG 		:= "!!"
$		IF VER THEN dbg	:= write sys$output
$		_fnm		:== ""	!Filename
$		_fty		:== ""	!Filetype
$		_cmpr		:== ""
$		linkall		:= "N"
$		link_options	:= ""
$ if f$getsyi("NODE_HWTYPE") .eqs. "ALPH" then link_options = "/SYSEXE"
$		msg_1		:= " "
$		msg_2		:= "occured "
$		msg_3		:= " "
$		start_file	 = "''p1'"
$		start_file	:= 'start_file
$		all_specified	= "N"
$
$		on error then goto err
$		on severe then goto err
$		on warning then goto err
$		on control_y then goto conty
$
$		if start_file .nes. "/ALL" then goto part1
$		all_specified	= "Y"
$		start_file := ""
$		compileall := "YES"
$		goto comp_all_subs
$
$part1:		if start_file .nes. "" then goto ask_any_comps
$		read/prompt="Compile all subroutines (YES/n) >" -
			/end=cep/err=cep sys$command compileall
$		compileall = f$edit (compileall, "UPCASE,COMPRESS,TRIM")
$		if "''compileall'" .eqs. "YES" then goto comp_all_subs
$                                                 
$ask_any_comps:	compileall:="N"
$		if start_file .nes. "" then goto comp_all_subs
$		read/end=cep/err=cep/prompt="Compile any subroutines (y/n) >" -
			sys$command compile
$		compile  = f$edit (compile, "UPCASE,COMPRESS,TRIM")
$		if (.not. compile) then goto no_sub_comp
$
$comp_all_subs:	open/err=no_subs_file infile sub_files
$
$next:		read/end=comp_end infile record	! Search for next record
$		call extract_fnm		! Extract filename
$		if "''_fnm'" .eqs. "" then goto next	!  if there else skip rcd
$		if start_file .eqs. "" then goto no_p1
$		if 'f$loc(start_file,_fnm) .eq. 'f$len(_fnm) then goto next
$		start_file := ""
$
$no_p1:		call choose_compiler
$		msg_3:="before compiling "
$		if (compileall .eqs. "YES") then goto comp_next
$		read/end=cep/err=cep sys$command compile -
		/prompt="Compile ''_fnm' (y/n/r/p/l/q or ""Name"") >"
$		compile  = f$edit (compile, "UPCASE,COMPRESS,TRIM")
$		len='f$len(compile)
$		if len .le. 1 then goto part2
$		start_file = f$edit (compile, "UPCASE,COMPRESS,TRIM")
$		close infile			!Explicit Name given
$		goto comp_all_subs
$
$part2:		if ("''compile'" .eqs. "P") then goto comp_end
$		if ("''compile'" .eqs. "R") then goto comp_rest
$		if ("''compile'" .eqs. "L") then goto link_now
$		if ("''compile'" .eqs. "Q") then goto cep
$		if (.not. compile) then goto next
$		goto comp_next
$comp_rest:	write sys$output "Compiling remaining modules !"
$		compileall := "YES"
$comp_next:	write sys$output "Compiling >''_fnm'"
$		msg_3:="during compilation of "
$		if f$extract(0,1,_cmpr) .nes. "!"
$		then
$		   '_cmpr' '_fnm'
$		    library/replace/log/obj	object_lib '_fnm'
$		    delete '_fnm'.obj;/lo
$		    delete '_fnm'.lis;*/lo
$		else
$		   write sys$output f$fao("No compilation due to !AS", _cmpr)
$		endif
$		goto next
$
$link_now:	compileall:="N"
$		compprogs:="N"
$		compile:="N"
$		close infile		!subroutines
$		compile:="Y"
$		open/err=no_prog_file infile prog_files
$		goto next_prog		!link now
$comp_end:	write sys$output "Subroutine compilations ended... "
$		close infile
$
$no_sub_comp:	msg_3 := "before compiling "
$		open/err=no_prog_file infile prog_files
$		if all_specified .eqs. "Y"
$		then
$		   compileall = "YES"
$		else
$		   read /prompt="Compile all Programs (YES/n) >" -
			/end=cep/err=cep sys$command compileall
$		endif
$		compileall = f$edit (compileall, "UPCASE,COMPRESS,TRIM")
$		if (compileall .eqs. "YES")
$		then
$		   compprogs := "N"
$		   compile := "Y"
$		   goto comp_all_progs
$		endif
$		compileall:="N"
$		read/end=cep/err=cep/prompt="Compile any Programs (y/n) >" -
			sys$command compprogs
$		compile = f$edit (compprogs, "UPCASE,COMPRESS,TRIM")
$
$re_open_2:	if (.not. compprogs) then -
$		read/end=cep/err=cep/prompt="Link all programs (YES/n) >" -
			sys$command linkall
$		linkall = f$edit (linkall, "UPCASE,COMPRESS,TRIM")
$
$comp_all_progs:
$next_prog:	msg_3:="before compiling "
$		read/end=cep infile record	!Search for next record
$		call extract_fnm		! Extract filename
$		if "''_fnm'" .eqs. "" then goto next_prog !  if there else skip rcd
$		if start_file .eqs. "" then goto no_p1_2
$		if 'f$loc(start_file,_fnm) .eq. 'f$len(_fnm) then goto next_prog
$		start_file := ""
$no_p1_2:	if (.not. compile) then goto link_prog
$		call choose_compiler
$
$		msg_3:="before compiling "
$		if (compileall .eqs. "YES") then goto cnp
$
$		read/end=cep/err=cep sys$command compile -
		/prompt="Compile ''_fnm' (y/r/n/q or ""Name"") >"
$		compile = f$edit (compile, "UPCASE,COMPRESS,TRIM")
$		len='f$len(compile)
$		if len .le. 1 then goto part3
$		start_file = f$edit (compile, "UPCASE,COMPRESS,TRIM")
$		close infile
$		compile:="Y"
$		open/err=no_prog_file infile prog_files
$		goto re_open_2
$part3:		if ("''compile'" .eqs. "Q") then goto cep
$		if ("''compile'" .eqs. "R") then goto comp_rest_prg
$		if (.not. compile) then goto link_prog
$
$		goto cnp
$comp_rest_prg:	write sys$output "Compiling remaining programs !"
$		compileall := "YES"
$! comp_next_prog
$cnp:		write sys$output "Compiling >''_fnm'"
$		msg_3 := "during compilation of "
$		if f$extract(0,1,_cmpr) .nes. "!"
$		then
$		   '_cmpr' '_fnm'
$		    library/replace/log/obj	object_lib '_fnm'
$		    delete '_fnm'.obj;/lo
$		    delete '_fnm'.lis;*/lo
$		else
$		   write sys$output f$fao("No compilation due to !AS. Skipping Link.", _cmpr)
$		   goto next_prog
$		endif
$               
$link_prog:	msg_3 := "before linking "
$		if (compprogs) then compile := 'compprogs
$		if (compileall .eqs. "YES") then goto lnp
$		if (linkall .eqs. "YES") then goto lnp
$		read/end=cep/err=cep/prompt="Link ''_fnm' (y/n/q) >" -
			sys$command comp
$		comp = f$edit (comp, "UPCASE,COMPRESS,TRIM")
$		if ("''comp'" .eqs. "Q") then goto CEP
$		if (.not. comp) then goto next_prog
$                     
$! link_next_prog
$lnp:		write sys$output "Linking ''link_options' ''_fnm'"
$		msg_3 := "during linking of "
$ SET VERI
$		link 'link_options'/notrace /exe='exe_direc''_fnm' object_lib/inc=('_fnm',emumsg), object_lib/lib 'options'
$ SET NOVERI
$		goto next_prog
$
$! Comp_end_prog
$cep:		write sys$output "Program compile/link completed... "
$		goto close_if
$
$
$! Error handling and things
$
$conty:		msg_1	:= "Control_Y Abort "
$		goto issue_msg
$no_subs_file:	msg_2	:= "opening subroutines data file " 
$		_fnm	:== 'f$log("sub_files")
$		goto file_err
$no_prog_file:	msg_2	:= "opening programs data file "
$		_fnm	:== 'f$log("prog_files")
$		goto file_err
$file_err:	msg_3	:= ""
$err:		msg_1	:= "Error "
$issue_msg:	write sys$output "''msg_1'''msg_2'''msg_3'>''_fnm'"
$close_if:	log_tran:='f$log("infile")
$      		if log_tran .nes. "" then close infile
$Finished:	delete%/sym/glo _fnm
$		delete%/sym/glo _fty
$		delete%/sym/glo _cmpr
$		exit
$
$!*****************************************************************************
$choose_compiler: subroutine 
$		if _fty .eqs. ".MAR"
$		then
$		   if f$getsyi("NODE_HWTYPE") .eqs. "ALPH"
$		   then	!alpha
$		      _cmpr:=="macro/migration/list/flag=(noalign)/optim=none/unalign "
$		   else	!vax
$		      _cmpr:=="macro/list "
$		   endif
$		endif
$		if _fty .eqs. ".SQLMOD" then -
			_cmpr:=="SQLMOD/list "
$		if _fty .eqs. ".MSG" then -
			_cmpr:=="message/obj/list "
$		if _fty .eqs. ".CLD" then -
			_cmpr:=="set command/object/list "
$		if _fty .eqs. ".FOR"
$		then
$		   _cmpr:=="fortran/show=(noinclude,map)/noopt/list "
$		   if f$search("sys$system:fortran.exe;") .eqs. "" then _cmpr :=="!No fortran compiler"
$		endif
$ dbg "Compiler >''_cmpr'"
$		exit
$endsubroutine
$!*****************************************************************************
$extract_fnm:	subroutine
$ dbg "Record >''record'"
$		_fnm == ""
$		record = f$edit (record, "UNCOMMENT,COMPRESS,TRIM,UPCASE")
$		if 'f$len(record) .eq. 0 then goto exit_extract_fnm
$		_fnm == f$element(0," ",record)
$		_fty == f$parse(_fnm,".for",,"type")
$		_fnm == f$parse(_fnm,".for",,"name")
$ dbg "_fnm >''_fnm'"
$exit_extract_fnm:	exit
$endsubroutine
$!*****************************************************************************
$
