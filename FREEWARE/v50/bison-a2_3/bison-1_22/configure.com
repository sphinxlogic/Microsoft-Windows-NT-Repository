  




$!# Guess values for system-dependent variables and create Makefiles.
$!# Generated automatically using autoconf version 1.11 
$!# Copyright (C) 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
$
$!# This configure script is free software; you can redistribute it and/or
$!# modify it under the terms of the GNU General Public License as published
$!# by the Free Software Foundation; either version 2, or (at your option)
$!# any later version.
$
$!# This script is distributed in the hope that it will be useful, but
$!# WITHOUT ANY WARRANTY; without even the implied warranty of
$!# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
$!# Public License for more details.
$
$!# You should have received a copy of the GNU General Public License
$!# along with this program; if not, write to the Free Software
$!# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
$
$!#
$ goto passed_subroutines
$
$!# This set of subroutines helps with parsing arguments GNU style
$!# 3 Symbols are needed to make these possible:
$!#
$!#   POSSIBLE_ARGS_n := switch_descriptors
$!#	switch_descriptors := switch_descriptor
$!#				| switch_descriptor "|" switch_descriptors
$!#	switch_descriptor := switches ":" lable [ ":" description ]
$!#	switches := switch
$!#			| switch "/" switches
$!#	switch := "-" letter
$!#		| "--" letters [ "*" letters ] [ "=" | "&" ]
$!#	lable := letters
$!#	letters := letter
$!#		| letter letters
$!#	letter := "A".."Z" | "_" | "-"
$!#     description := any chars | "\n"
$!#
$!#   N_POSSIBLE_ARGS := number of POSSIBLE_ARGS_n variables we have.
$!#
$!#   DEFAULT_LABLE	containing the lable to jump to when the next
$!#			argument is not a switch
$!#
$!#   ERROR_LABLE	containing the lable to jump to on error
$!#
$!# examples:
$!#    $ possible_args_0 = "-H/--H*ELP:A_HELP:--help	Gives this help"
$!#    $ possible_args_1 = "-M/--M*ACRODIR=:A_MACRODIR" +-
$!#    "|-V/--V*ERSION:A_VERSION|--:LOOPEND|-:A_KBDINP|-W-&/--WITH-&:A_WITH"
$!#    $ n_possible_args = 2
$!#    $ default_lable := A_NOMORE
$!#    $ error_lable := A_BADSWITCH
$count_args:
$ ac_count = 8
$count_args1:
$ if ac_count .eq. 0 then return
$ if p'ac_count' .nes. "" then return
$ ac_count = ac_count - 1
$ goto count_args1
$
$
$process_arg:
$ ac_label = default_lable
$ ac_optarg = "="
$ _n1 = -1
$process_arg0:
$ _n1 = _n1 + 1
$ if _n1 .ge. n_possible_args then return
$ _i1 = 0
$process_arg1:
$ _e1 = f$element(_i1,"|",possible_args_'_n1')
$ _i1 = _i1 + 1
$ if "''_e1'" .eqs. "|" then goto process_arg0
$ _s = f$element(0,":",_e1)
$ _l = f$element(1,":",_e1)
$ _i2 = 0
$process_arg2:
$ _e2 = f$element(_i2,"/","''_s'")
$ _i2 = _i2 + 1
$ if "''_e2'" .eqs. "/" then goto process_arg1
$ _s1 = f$element(0,"*",_e2)
$ _s2 = f$element(1,"*",_e2)
$ if "''_s2'" .eqs. "*" then _s2 = ""
$ _s3 = f$element(0,"&",_s1 + _s2)
$ _s4 = f$element(1,"&","''_s1'" + "''_s2'")
$ check_amp := true
$ if _s4 .eqs. "&" then check_amp := false
$ if _s4 .eqs. "&" then _s3 = f$element(0,"=",_s1 + _s2)
$ if _s4 .eqs. "&" then _s4 = f$element(1,"=",_s1 + _s2)
$ _p1 = f$element(0,"=",ac_option)
$ _p2 = f$element(1,"=",ac_option)
$ if (.not. check_amp) -
	.and. "''_p1'" .nes. f$extract(0,f$length(_p1),"''_s3'") -
	then goto process_arg2
$ if (check_amp) -
	.and. "''_s3'" .nes. f$extract(0,f$length(_s3),_p1) -
	then goto process_arg2
$ if f$length(_p1) .lt. f$length(_s1) then goto process_arg_err
$ if (_s4 .eqs. "=" .and. _p2 .nes. "=") then goto process_arg_err
$!# if (.not. check_amp) .and. (_p2 .eqs. "=") then gosub shift
$ ac_optarg = _p2
$ if check_amp then ac_package = f$extract(f$length(_s3), 999, _p1)
$ ac_label = _l
$ return
$process_arg_err:
$ ac_label = error_lable
$ return
$
$
$shift:
$ p1 = p2
$ p2 = p3
$ p3 = p4
$ p4 = p5
$ p5 = p6
$ p6 = p7
$ p7 = p8
$ p8 = ""
$ ac_count = ac_count - 1
$ return
$
$ac_usage:
$!# Only options tha might do something get documented
$ ac_w := write 'ac_usage_output'
$ ac_w "Usage: configure options host"
$ ac_w "Options: defaults in brackets after descriptions"
$ac_usage_arg:
$ _n1 = -1
$ac_usage_arg0:
$ _n1 = _n1 + 1
$ if _n1 .ge. n_possible_args then return
$ _i1 = 0
$ac_usage_arg1:
$ _e1 = f$element(_i1,"|",possible_args_'_n1')
$ _i1 = _i1 + 1
$ if "''_e1'" .eqs. "|" then goto ac_usage_arg0
$ _s = f$element(0,":",_e1)
$ _l = f$element(1,":",_e1)
$ _d = f$extract(f$length(_s)+f$length(_l)+2,f$length(_e1),_e1)
$ if _d .nes. ":" .and. _d .nes. "" then ac_w _d
$ goto ac_usage_arg1
$ return
$
$passed_subroutines:
$
$
$!# Save the original args to write them into config.status later.
$ configure_args = p1+" "+p2+" "+p3+" "+p4+" "+p5+" "+p6+" "+p7+" "+p8+" "
$
$!# A few defaults
$ prefix := SYS$SYSDEVICE:[GNU]
$ STARTUPDIR := SYS$COMMON:[SYS$STARTUP]
$
$!# Initialize some variables set by options.
$!# The variables have the same names as the options, with
$!# dashes changed to underlines.
$ build:=NONE
$ host:=NONE
$ no_create:=NO
$ nonopt:=NONE
$ norecursion:=NO
$! prefix:=
$ exec_prefix:=
$ program_prefix:=
$ program_suffix:=
$ program_transform_name:=
$ silent:=NO
$ srcdir:=
$ target:=NONE
$ verbose:=NO
$ x_includes:=
$ x_libraries:=
$
$ possible_args_0 = "-BUILD=/--B*UILD=:A_BUILD:"+-
"--build=BUILD		configure for building on BUILD BUILD=HOST"
$ possible_args_1 = "-DISABLE-&/--DISABLE-&:A_DISABLE:"+-
"--disable-FEATURE	do not include FEATURE (same as --enable-FEATURE=no)"
$ possible_args_2 = "-ENABLE-&=/--ENABLE-&=:A_ENABLE:"+-
"--enable-FEATURE=ARG	include FEATURE ARG=yes"
$ possible_args_3 = "-EXEC-PREFIX=/-EXEC_PREFIX=/--E*XEC-PREFIX=:A_EXEC_PREFIX:"+-
"--exec-prefix=PREFIX	install host dependent files in PREFIX /usr/local"
$ possible_args_4 = "-GAS=/--G*AS:A_GAS"
$ possible_args_5 = "-HELP=/--HE*LP:A_HELP:"+-
"--help			print this message"
$ possible_args_6 = "-HOST=/--HO*ST=:A_HOST:"+-
"--host=HOST		configure for HOST guessed"
$ possible_args_7 = "-NFP=/--NF*P:A_NFP"
$ possible_args_8 = "-NO-CREATE=/--NO-C*REATE:A_NO_CREATE"
$ possible_args_9 = "-NORECURSION=/--NOR*ECURSION:A_NORECURSION"
$ possible_args_10 = "-PREFIX/--P*REFIX=:A_PREFIX:"+-
"--prefix=PREFIX		install host independent files in PREFIX /usr/local"
$ possible_args_11 = "-PROGRAM-PREFIX/--PROGRAM-P*REFIX=:A_PROGRAM_PREFIX"
$ possible_args_12 = "-PROGRAM-SUFFIX/--PROGRAM-S*UFFIX=:A_PROGRAM_SUFFIX"
$ possible_args_13 = "-PROGRAM-TRANSFORM-NAME/--PROGRAM-T*RANSFORM-NAME=:A_PROGRAM_TRANSFORM_NAME"
$ possible_args_14 = "-Q/-QUIET/-SILENT/--Q*UIET/--SIL*ENT:A_SILENT:"+-
"--quiet, --silent	do not print `checking for...' messages"
$ possible_args_15 = "-SRCDIR=/--S*RCDIR=:A_SRCDIR:"+-
"--srcdir=DIR		find the sources in DIR configure dir or .."
$ possible_args_16 = "-TARGET=/--T*ARGET=:A_HOST:"+-
"--target=TARGET		configure for TARGET TARGET=HOST"
$ possible_args_17 = "-V/-VERBOSE/--VERB*OSE:A_VERBOSE:"+-
"--verbose		print results of checks"
$ possible_args_18 = "-VERSION/--VERS*ION:A_VERSION:"+-
"--version		print the version of autoconf that created configure"
$ possible_args_19 = "-WITH-&=/--WITH-&=:A_WITH:"+-
"--with-PACKAGE=ARG	use PACKAGE ARG=yes"
$ possible_args_20 = "-WITHOUT-&/--WITHOUT-&:A_WITHOUT:"+-
"--without-PACKAGE	do not use PACKAGE (same as --with-PACKAGE=no)"
$ possible_args_21 = "--X:A_X"
$ possible_args_22 = "-X-INCLUDES=/--X-I*NCLUDES=:A_X_INCLUDES:"+-
"--x-includes=DIR	X include files are in DIR"
$ possible_args_23 = "-X-LIBRARIES=/--X-L*IBRARIES=:A_X_LIBRARIES:"+-
"--x-libraries=DIR	X library files are in DIR"
$ n_possible_args = 24
$ possible_args_'n_possible_args' = "|-STARTUPDIR/--ST*ARTUPDIR=:A_STARTUPDIR:--startupdir=DIR	defines the directory for the startup database"
$ n_possible_args = n_possible_args + 1
$ startupdir = "SYS$COMMON:[SYS$STARTUP]"
$ possible_args_'n_possible_args' = "|-DCLTABLE/--D*CLTABLE=:A_DCLTABLE:--dcltable=DCLTABLE	defines the dcl table to put cmd.def. in."
$ n_possible_args = n_possible_args + 1
$ dcltable = ""
$ default_lable := A_SKIP
$ error_lable := A_ERROR
$ gosub count_args ! generates AC_COUNT
$ i = 0
$ ac_prev :=
$ goto ac_arg_file_init
$ac_arg_file_err:
$ ac_tmp = f$message($status)
$ write sys$error "configure: warning: ","could not open file ''ac_arg_file_name'"
$ write sys$error "configure:          ","''ac_tmp'"
$ got ac_arg_file_init
$ac_arg_file_end:
$ close ac_arg_file
$ac_arg_file_init:
$ ac_args_from_file := NO
$ ac_arg_line :=
$ac_arg_loop:
$ if i .ge. ac_count .and. .not. ac_args_from_file then goto ac_arg_loopend
$ if ac_args_from_file then goto ac_arg_file_loop
$ i = i + 1
$ ac_option = p'i'
$ ac_label = ""
$ goto ac_arg_prev
$ac_arg_file_loop:
$ if ac_arg_line .nes. "" then goto ac_arg_file_loop1
$  read/error=ac_arg_file_end/end=ac_arg_file_end ac_arg_file ac_arg_line
$  goto ac_arg_file_loop
$ac_arg_file_loop1:
$ ac_option :=
$ ac_arg_line = f$edit(ac_arg_line,"TRIM")
$ac_arg_file_loop2:
$ if ac_arg_line .eqs. "" then goto ac_arg_file_loop5
$  ac_i1 = f$locate(" ",ac_arg_line)
$ ac_arg_file_loop3:
$  ac_i2 = f$locate("""",ac_arg_line)
$  if ac_i2 .ge. ac_i1 then goto ac_arg_file_loop4
$   ac_option = ac_option + f$extract(0,ac_i2,ac_arg_line)
$   ac_arg_line = f$extract(ac_i2,f$length(ac_arg_line)-ac_i2,ac_arg_line)
$   ac_i1 = ac_i1 - ac_i2
$   ac_i2 = f$locate("""",ac_arg_line)
$   ac_option = ac_option + f$extract(0,ac_i2,ac_arg_line)
$   ac_arg_line = f$extract(ac_i2,f$length(ac_arg_line)-ac_i2,ac_arg_line)
$   ac_i1 = ac_i1 - ac_i2
$   if ac_i1 .lt. ac_i1 then goto ac_arg_file_loop2: ! The space was between quotes
$   goto ac_arg_file_loop3
$ ac_arg_file_loop4:
$  ac_option = ac_option + f$extract(0,ac_i1,ac_arg_line)
$  ac_arg_line = f$extract(ac_i1,f$length(ac_arg_line)-ac_i1,ac_arg_line)
$ac_arg_file_loop5:
$ ! This should make sure the argument is treated the same way as a command
$ ! line argument.
$ ac_option := 'ac_option'
$
$ac_arg_prev:
$!# If the previous option needs an argument, assign it
$ if ac_prev .eqs. "" then goto ac_arg_no_prev
$  'ac_prev' = ac_option
$  ac_prev = ""
$  goto ac_arg_loop
$
$ac_arg_no_prev:
$ if f$extract(0,1,ac_option) .nes. "+" then goto ac_arg_loop1
$ ac_arg_file_name = f$extract(1,999,ac_option)
$ open/read/error=ac_arg_file_err ac_arg_file 'ac_arg_file_name'
$ ac_args_from_file := YES
$ goto ac_arg_loop
$
$ac_arg_loop1:
$ gosub process_arg ! generates AC_LABEL and AC_OPTION
$ goto 'ac_label'
$A_STARTUPDIR:
$ if ac_optarg .eqs. "=" then ac_prev := startupdir
$ if ac_optarg .nes. "=" then startupdir = ac_optarg
$ goto ac_arg_loop
$A_DCLTABLE:
$ if ac_optarg .eqs. "=" then ac_prev := dcltable
$ if ac_optarg .nes. "=" then dcltable = ac_optarg
$ goto ac_arg_loop
$A_BUILD:
$A_EXEC_PREFIX:
$A_HOST:
$A_PREFIX:
$A_PROGRAM_PREFIX:
$A_PROGRAM_SUFFIX:
$A_PROGRAM_TRANSFORM_NAME:
$A_SRCDIR:
$A_TARGET:
$A_X_INCLUDES:
$A_X_LIBRARIES:
$ ac_varname = ac_label - "A_"
$ if ac_optarg .eqs. "=" then ac_prev := 'ac_varname'
$ if ac_optarg .nes. "=" then 'ac_varname' = ac_optarg
$ goto ac_arg_loop
$A_NO_CREATE:
$A_NORECURSION:
$A_SILENT:
$A_VERBOSE:
$ ac_varname = ac_label - "A_"
$ 'ac_varname' := YES
$ goto ac_arg_loop
$A_GAS:
$A_X:
$ ac_varname = ac_label - "A_"
$ with_'ac_varname' := YES
$ goto ac_arg_loop
$A_NFP:
$ with_fp := NO
$ goto ac_arg_loop
$A_ENABLE:
$A_DISABLE:
$ ac_name = ac_package
$ gosub A_check_name
$ if ac_check then goto ac_process_feature
$ write sys$error "configure: ","''ac_package': invalid feature name"
$ exit 2
$ goto ac_arg_loop
$ac_process_feature:
$ if ac_optarg .eqs. "=" then ac_optarg := YES
$ if ac_label .eqs. "A_DISABLE" then ac_optarg := NO
$ enable_'ac_name' = ac_optarg
$ goto ac_arg_loop
$A_WITH:
$A_WITHOUT:
$ ac_name = ac_package
$ gosub A_check_name
$ if ac_check then goto ac_process_package
$ write sys$error "configure: ","''ac_package': invalid package name"
$ exit 2
$ goto ac_arg_loop
$ac_process_package:
$ if ac_optarg .eqs. "=" then ac_optarg := YES
$ if ac_label .eqs. "A_WITHOUT" then ac_optarg := NO
$ with_'ac_name' = ac_optarg
$ goto ac_arg_loop
$A_HELP:
$ ac_usage_output := sys$output
$ gosub ac_usage
$ exit
$A_VERSION:
$ write sys$output "configure generated by autoconf version 1.11"
$ exit
$A_SKIP:
$ if f$extract(0,1,ac_option) .eqs. "-" then goto A_ERROR
$ ac_name = f$edit(ac_option,"LOWERCASE")
$ gosub A_check_name_no_convert
$ if ac_check then goto A_SKIP1
$ write sys$error "configure: warning: ","''ac_option': invalid host type"
$A_SKIP1:
$ if nonopt .eqs. "NONE" then goto A_SKIP2
$ write sys$error "configure: ","can only configure for one host and one target at a time"
$ exit 2
$A_SKIP2:
$ nonopt = ac_option
$ goto ac_arg_loop
$A_ERROR:
$ write sys$error "configure: ","''ac_option': invalid option; use --help to show usage"
$ exit 2
$ goto ac_arg_loop
$
$A_check_name:
$ ac_convert := yes
$ goto A_check_name1
$A_check_name_no_convert:
$ ac_convert := no
$A_check_name1:
$ ac_p = f$length(ac_name)
$ ac_check := YES
$ac_loop_name:
$ ac_p = ac_p - 1
$ if ac_p .eq. -1 then goto ac_loop_name_end
$ ac_e = f$extract(ac_p,1,ac_name)
$ if ac_e .eqs. "-" .and. ac_convert then -
	ac_nameac_p*8,8 = 95 ! an underscore
$ if ac_e .eqs. "-" .or. ac_e .eqs. "_" -
     .or. (ac_e .ges. "a" .and. ac_e .les. "z") -
     .or. (ac_e .ges. "A" .and. ac_e .les. "A") -
     .or. (ac_e .ges. "0" .and. ac_e .les. "9") then goto ac_loop_name
$ ac_check := NO
$ac_loop_name_end:
$ return
$ac_arg_loopend:
$ if ac_prev .eqs. "" then goto ac_arg_end
$
$ ac_p = f$length(ac_prev)
$ac_arg_loopend1:
$ ac_p = ac_p - 1
$ if ac_p .eq. -1 then goto ac_arg_loopend2
$ ac_e = f$extract(ac_p,1,ac_prev)
$ if ac_e .eqs. "_" then ac_prevac_p*8,8 = 45 ! a dash
$ goto ac_arg_loopend1
$ac_arg_loopend2:
$ write sys$error "configure: ","missing argument to --''ac_prev'"
$ exit 2
$ac_arg_end:
$
$ ac_DCL_message = f$environment("MESSAGE")
$ ac_DCL_verify = f$verify(0)
$ ! 'f$verify(ac_DCL_verify)
$ ac_DCL_original_directory = f$trnlnm("SYS$DISK")+f$directory()
$ on warning then continue
$ on error then goto ac_bail_out
$ on severe_error then goto ac_bail_out
$ on control_y then goto ac_bail_out
$ goto ac_passed_error_routines
$ac_bail_out:
$ set default 'ac_DCL_original_directory'
$ set message 'ac_DCL_message'
$!#
$ goto after_VMS_DELR
$VMS_DELR: subroutine
$ on control_y then goto VMS_DELR_STOP
$ on warning then goto VMS_DELR_exit
$ _VMS_DELR_def = f$trnlnm("SYS$DISK")+f$directory()
$ if f$parse(p1) .eqs. "" then exit
$ set default 'f$parse(p1,,,"DEVICE")''f$parse(p1,,,"DIRECTORY")'
$VMS_DELR_loop:
$ _fp = f$parse(".DIR",p1)
$ _f = f$search(_fp)
$ if _f .eqs. "" then goto VMS_DELR_loopend
$ call VMS_DELR [.'f$parse(_f,,,"NAME")']*.*
$ goto VMS_DELR_loop
$VMS_DELR_loopend:
$ _fp = f$parse(p1,".;*")
$ if f$search(_fp) .eqs. "" then goto VMS_DELR_exit
$ set noon
$ set file/prot=(S:RWED,O:RWED,G:RWED,W:RWED) '_fp'
$ set on
$ delete/nolog '_fp'
$VMS_DELR_exit:
$ set default '_VMS_DELR_def'
$ exit
$VMS_DELR_STOP:
$ set default '_VMS_DELR_def'
$ stop/id=""
$ exit
$ endsubroutine
$
$after_VMS_DELR:
$!
$ call VMS_DELR "conftest*.*"
$
$ call VMS_DELR "confdefs*.*"
$
$ exit 1 + 0 * f$verify(ac_DCL_verify)
$ac_passed_error_routines:
$
$!# Save the original args if we used an alternate arg parser.
$ if f$type(configure_args) .nes. "" then ac_configure_args = configure_args
$ if f$type(configure_args) .eqs. "" then -
	ac_configure_args = p1+" "+p2+" "+p3+" "+p4+" "+p5+" "+p6+" "+p7+" "+p8
$!# Strip out --no-create and --norecursion so they don't pile up.
$ configure_args = configure_args-" --NO-CREATE"-"--NO-CREATE "-"--NO-CREATE"
$ configure_args = configure_args-" --NORECURSION"-"--NORECURSION "-"--NORECURSION"
$
$ goto ac_prepare_after_subs
$ac_prepare_compile_params:
$ __cflags = ""
$ if f$type(CFLAGS) .nes. "" then __cflags = CFLAGS
$ __defs = ""
$ if f$type(DEFS) .nes. "" then __defs = "/defin=("+DEFS+")"
$ if f$type(ac_flags) .nes. "" then __cflags = __cflags + ac_flags
$ __incs = ""
$ if f$type(INCS) .nes. "" then __incs = "/include=("+INCS+")"
$ __compiler := cc
$ if f$type(ac_cc) .nes. "" then __compiler := 'ac_cc'
$ if f$type(CC) .nes. "" then __compiler := 'CC'
$ __preprocessor := cc '__cflags'/preprocess=
$ if f$type(ac_cpp) .nes. "" then __preprocessor := 'ac_cpp'
$ if f$type(CPP) .nes. "" then __preprocessor := 'CPP'
$ __extension := c
$ if f$type(ac_ext) .nes. "" then __extension = ac_ext
$ return
$ac_prepare_preprocess:
$ gosub ac_prepare_compile_params
$! '__preprocessor' CONFTEST.OUT '__defs' '__incs' conftest.'__extension'
$ '__preprocessor' CONFTEST.OUT conftest.'__extension'
$ return
$ac_prepare_compile:
$ gosub ac_prepare_compile_params
$ '__compiler' '__cflags' /object=conftest.obj '__defs' '__incs' conftest.'__extension'
$ return
$ac_prepare_link:
$ open/write link_opt conftest.opt
$ libi = 0
$ if f$type(LIBS) .eqs. "" then libs = ""
$ac_prepare_link_loop:
$ lib1 = f$element(libi," ",libs)
$ libi = libi + 1
$ if lib1 .nes. " "
$  then
$   if lib1 .nes. "" then write link_opt lib1
$   goto ac_prepare_link_loop
$  endif
$ close link_opt
$ libc = ","
$ if "''OPTS'" .eqs. "" then libc = ""
$ link 'ldflags' conftest.obj,conftest.opt/opt 'libc' 'OPTS'
$ return
$ac_prepare_after_subs:
$
$ ldflags := /nouserlibrary
$ ac_preprocess := gosub ac_prepare_preprocess
$ ac_compile := gosub ac_prepare_compile
$ ac_link := gosub ac_prepare_link
$
$ call VMS_DELR "conftest*.*"
$
$ if f$search("CONFDEFS.H.*") .nes. "" then delete/nolog confdefs.h.*
$
$ open/write foo confdefs.h
$ close foo
$
$!# A filename unique to this package, relative to the directory that
$!# configure is in, which we can look for to find out if srcdir is correct.
$ ac_unique_file:=reduce.c
$
$!# Find the source files, if location was not specified.
$ if "''srcdir'" .nes. "" then goto ac_prepare_srcdir_try
$  ac_srcdirdefaulted := yes
$!# Try the directory containing this script, then `..'.
$  ac_prog=f$environment("PROCEDURE")
$  _dummy = "''prog'" ! This is prog, for debugging
$  ac_confdir=f$parse(ac_prog,,,"NODE") + f$parse(ac_prog,,,"DEVICE") + f$parse(ac_prog,,,"DIRECTORY")
$  _dummy = "''ac_confdir'" ! This is confdir, for debugging
$  if ac_confdir .eqs. f$environment("DEFAULT") then ac_confdir := []
$  srcdir=ac_confdir
$  if f$search(srcdir+ac_unique_file) .eqs. "" then srcdir := [-]
$ac_prepare_srcdir_try:
$ if f$search(srcdir+ac_unique_file) .nes. "" then goto ac_prepare_srcdir2
$ if "''ac_srcdirdefaulted'" then -
  $ write sys$error "configure: ","can not find sources in `''ac_confdir'' or `-'."
$ exit 2
$ if .not. "''ac_srcdirdefaulted'" then -
  $ write sys$error "configure: ","can not find sources in `''srcdir''."
$ exit 2
$ exit 0
$ac_prepare_srcdir2:
$!# The following are quite usefull
$ if srcdir .eqs. "[-]" .or. srcdir .eqs. "[]"
$  then
$   srcdir_dev:=
$   srcdir_dir = srcdir - "[" - "]"
$  else
$   srcdir_dir = f$parse(srcdir,,,"DIRECTORY") - "[" - "]" - "<" - ">"
$   srcdir_dev = f$parse(srcdir,,,"DEVICE")
$  endif
$ ac_cc := cc
$ if "''CC'" .nes. "" then ac_cc = CC
$ ac_ext:=c
$ ac_cpp := 'ac_cc' 'CFLAGS' /preprocess=
$ if "''CPP'" .nes. "" then ac_cpp = CPP
$
$
$ if "''CC'" .eqs. ""
$  then ! We are taking a risk here, because we are really trying to start
$	! the program. I hope we don't get caught by this.
$   set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   !# Gasp...
$   ac_dummy = "cc"
$ if .not. silent then write sys$output "checking ","for ''ac_dummy'"
$   ac_dummy
$   _tmp = $status
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$   
$   set on
$   if _tmp .ne. %X038090 ! This is the code for an unknown verb
$    then
$     CC = "cc"
$    endif
$  endif
$ if "''CC'" .eqs. "" then CC="gcc"
$ if "''CC'" .nes. "" then $ if verbose then write sys$output "	","setting CC to ''CC'"
$
$!# Find out if we are using GNU C, under whatever name.
$ _GNU_C := no
$ _DEC_C := no
$ _VAX_C := no
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ type sys$input /out=conftest.c
#ifdef __GNUC__
conftest___ _GNU_C:=yes
#endif
#ifdef __DECC
conftest___ _DEC_C:=yes
#else
#ifdef VAXC
conftest___ _VAX_C:=yes
#endif
#endif
$!#
$ if f$type(CFLAGS) .eqs. "" then CFLAGS = ""
$ ac_preprocess
$ search conftest.out "conftest___" /out=conftest.out-stripped/noheader
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ open/read ac_prog_cc_in conftest.out-stripped
$ac_prog_cc_read_loop:
$ read/end=ac_prog_cc_stop_reading ac_prog_cc_in foo
$ foo = f$extract(12,999,foo) ! Get rid of "conftest___ "
$ 'foo'
$ goto ac_prog_cc_read_loop
$ac_prog_cc_stop_reading:
$ close ac_prog_cc_in
$ if f$type(LIBS) .eqs. "" then LIBS = ""
$ if f$type(OPTS) .eqs. "" then OPTS = ""
$ sys_includes = ""
$ if _GNU_C
$  then
$   GCC := yes
$   CFLAGS := 'CFLAGS' /NOCASE_HACK
$   if OPTS .nes. ""
$    then OPTS = OPTS + ","
$    endif
$   OPTS := 'OPTS'GNU_CC:000000OPTIONS_SHR.OPT/opt
$  else
$   if _DEC_C
$    then
$     CFLAGS := 'CFLAGS' /NESTED=INCLUDE
$    else
$     if LIBS .nes. ""
$      then LIBS = LIBS + " "
$      endif
$     LIBS = LIBS + "SYS$SHARE:VAXCRTL/SHARE"
$     sys_includes := vaxc$include:,sys$library:
$    endif
$   if sys_includes .nes. "" then defi/nolog sys 'sys_includes'
$  endif
$ ac_cflags = CFLAGS
$ set noon
$ delete conftest*.*.*
$
$ if .not. silent then write sys$output "checking ","how to run the C preprocessor"
$ if "''CPP'" .eqs. ""
$  then
$  !# This must be in double quotes, not single quotes, because CPP may get
$  !# substituted into the Makefile and ``${CC-cc}'' will simply confuse
$  !# make.  It must be expanded now.
$  CPP="''CC' ''CFLAGS'/PREPROCESS="
  $ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <stdio.h>"
$ write conftest_file "Syntax Error"
$ close conftest_file
$ set noon
$ def/user sys$output conftest.err
$ def/user sys$error conftest.err
$ 'ac_cpp' nla0: conftest.'ac_ext'
$ set on
$ ac_err := YES
$ if f$search("conftest.err") .nes. "" then -
	if f$file("conftest.err","EOF") .ne. 0 then -
	ac_err := NO
$ if ac_err
$  then
$!
$  else
$ delete/nolog conftest*.*.*
$ write sys$error "What do I do now?"
$ exit 0
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$  endif
$ if verbose then write sys$output "	","setting CPP to ''CPP'"
$
$ write sys$error "configure: warning: ","perversion allert!"
$! A VMS hack
$ tmp_quotes = """"
$ tmp_quotes = tmp_quotes + tmp_quotes
$ tmp_quotes = tmp_quotes + tmp_quotes
$ tmp_quotes = tmp_quotes + tmp_quotes
$ tmp_quotes = tmp_quotes + tmp_quotes
$ tmp_quotes2 = tmp_quotes + tmp_quotes + tmp_quotes + tmp_quotes + tmp_quotes
$ tmp_quotes = tmp_quotes + tmp_quotes + tmp_quotes
$! tmp_quotes = tmp_quotes + tmp_quotes
$! tmp_quotes = tmp_quotes + tmp_quotes
$! tmp_quotes = tmp_quotes + tmp_quotes
$ INSTALL="@"+(srcdir - "]") + ".VMS]COPY "+tmp_quotes+"/PROT=(S:RWED,O:RWED,G:RE,W:RE)"+tmp_quotes
$ INSTALL_PROGRAM="@"+(srcdir - "]") + ".VMS]COPY "+tmp_quotes+"/PROT=(S:RWED,O:RWED,G:RE,W:RE)"+tmp_quotes
$ INSTALL_DATA="@"+(srcdir - "]") + ".VMS]COPY "+tmp_quotes+"/PROT=(S:RWED,O:RWED,G:RE,W:RE)"+tmp_quotes
$
$ INSTALL_QUOTED="@"+(srcdir - "]") + ".VMS]COPY "+tmp_quotes2+"/PROT=(S:RWED,O:RWED,G:RE,W:RE)"+tmp_quotes2
$ INSTALL_PROGRAM_QUOTED="@"+(srcdir - "]") + ".VMS]COPY "+tmp_quotes2+"/PROT=(S:RWED,O:RWED,G:RE,W:RE)"+tmp_quotes2
$ INSTALL_DATA_QUOTED="@"+(srcdir - "]") + ".VMS]COPY "+tmp_quotes2+"/PROT=(S:RWED,O:RWED,G:RE,W:RE)"+tmp_quotes2
$
$ if f$type(INSTALL) .eqs. "" THEN -
	INSTALL:=copy/prot=(s:rwed,o:rwed,g:re,w:re)
$ if verbose then write sys$output "	","setting INSTALL to ",INSTALL
$ if f$type(INSTALL_PROGRAM) .eqs. "" THEN -
	INSTALL_PROGRAM:=copy/prot=(s:rwed,o:rwed,g:re,w:re)
$ if verbose then write sys$output "	","setting INSTALL_PROGRAM to ",INSTALL_PROGRAM
$ if f$type(INSTALL_DATA) .eqs. "" THEN -
	INSTALL_DATA:=copy/prot=(s:rwed,o:rwed,g:re,w:re)
$ if verbose then write sys$output "	","setting INSTALL_DATA to ",INSTALL_DATA
$
$ if f$type(INSTALL_QUOTED) .eqs. "" THEN -
	INSTALL_QUOTED:=copy/prot=(s:rwed,o:rwed,g:re,w:re)
$ if verbose then write sys$output "	","setting INSTALL_QUOTED to ",INSTALL_QUOTED
$ if f$type(INSTALL_PROGRAM_QUOTED) .eqs. "" THEN -
	INSTALL_PROGRAM_QUOTED:=copy/prot=(s:rwed,o:rwed,g:re,w:re)
$ if verbose then write sys$output "	","setting INSTALL_PROGRAM_QUOTED to ",INSTALL_PROGRAM_QUOTED
$ if f$type(INSTALL_DATA_QUOTED) .eqs. "" THEN -
	INSTALL_DATA_QUOTED:=copy/prot=(s:rwed,o:rwed,g:re,w:re)
$ if verbose then write sys$output "	","setting INSTALL_DATA_QUOTED to ",INSTALL_DATA_QUOTED
$
$ write sys$error "configure: warning: ","perversion is over now (pfewwww...)"
$ if .not. silent then write sys$output "checking ","for minix/config.h"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <minix/config.h>"
$ write conftest_file ""
$ close conftest_file
$ set noon
$ def/user sys$output conftest.err
$ def/user sys$error conftest.err
$ 'ac_cpp' nla0: conftest.'ac_ext'
$ set on
$ ac_err := YES
$ if f$search("conftest.err") .nes. "" then -
	if f$file("conftest.err","EOF") .ne. 0 then -
	ac_err := NO
$ if ac_err
$  then
$ delete/nolog conftest*.*.*
$ MINIX:=YES
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$!# The Minix shell can't assign to the same variable on the same line!
$ if "''MINIX'" .eqs. "YES"
$  then
  $!#
$ _ac_is_defined__POSIX_SOURCE = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining _POSIX_SOURCE"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define _POSIX_SOURCE 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """_POSIX_SOURCE""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
  $!#
$ _ac_is_defined__POSIX_1_SOURCE = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining _POSIX_1_SOURCE to be 2"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define _POSIX_1_SOURCE 2"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """_POSIX_1_SOURCE""=""2"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
  $!#
$ _ac_is_defined__MINIX = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining _MINIX"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define _MINIX 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """_MINIX""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$
$! ISC_POSIX is a noop on 
$ if .not. silent then write sys$output "checking ","for lack of working const"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { /* Ultrix mips cc rejects this.  */"
$ write conftest_file "typedef int charset[2]; const charset x;"
$ write conftest_file "/* SunOS 4.1.1 cc rejects this.  */"
$ write conftest_file "char const *const *ccp;"
$ write conftest_file "char **p;"
$ write conftest_file "/* AIX XL C 1.02.0.0 rejects this."
$ write conftest_file "   It does not let you subtract one const X* pointer from another in an arm"
$ write conftest_file "   of an if-expression whose if-part is not a constant expression */"
$ write conftest_file "const char *g = ""string"";"
$ write conftest_file "ccp = &g + (g ? g-g : 0);"
$ write conftest_file "/* HPUX 7.0 cc rejects these. */"
$ write conftest_file "++ccp;"
$ write conftest_file "p = (char**) ccp;"
$ write conftest_file "ccp = (char const *const *) p;"
$ write conftest_file "{ /* SCO 3.2v4 cc rejects this.  */"
$ write conftest_file "  char *t;"
$ write conftest_file "  char const *s = 0 ? (char *) 0 : (char const *) 0;"
$ write conftest_file ""
$ write conftest_file "  *t++ = 0;"
$ write conftest_file "}"
$ write conftest_file "{ /* Someone thinks the Sun supposedly-ANSI compiler will reject this.  */"
$ write conftest_file "  int x[] = {25,17};"
$ write conftest_file "  const int *foo = &x[0];"
$ write conftest_file "  ++foo;"
$ write conftest_file "}"
$ write conftest_file "{ /* Sun SC1.0 ANSI compiler rejects this -- but not the above. */"
$ write conftest_file "  typedef const int *iptr;"
$ write conftest_file "  iptr p = 0;"
$ write conftest_file "  ++p;"
$ write conftest_file "}"
$ write conftest_file "{ /* AIX XL C 1.02.0.0 rejects this saying"
$ write conftest_file "     ""k.c"", line 2.27: 1506-025 (S) Operand must be a modifiable lvalue. */"
$ write conftest_file "  struct s { int j; const int *ap[3]; };"
$ write conftest_file "  struct s *b; b->j = 5;"
$ write conftest_file "}"
$ write conftest_file "{ /* ULTRIX-32 V3.1 (Rev 9) vcc rejects this */"
$ write conftest_file "  const int foo = 10;"
$ write conftest_file "}"
$ write conftest_file "; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$!
$  else
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_const = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining const to be empty"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define const "
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """const""="""""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$ if .not. silent then write sys$output "checking ","for ANSI C header files"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <stdlib.h>"
$ write conftest_file "#include <stdarg.h>"
$ write conftest_file "#include <string.h>"
$ write conftest_file "#include <float.h>"
$ close conftest_file
$ set noon
$ def/user sys$output conftest.err
$ def/user sys$error conftest.err
$ 'ac_cpp' nla0: conftest.'ac_ext'
$ set on
$ ac_err := YES
$ if f$search("conftest.err") .nes. "" then -
	if f$file("conftest.err","EOF") .ne. 0 then -
	ac_err := NO
$ if ac_err
$  then
$ delete/nolog conftest*.*.*
$!# SunOS 4.x string.h does not declare mem*, contrary to ANSI.
$ open/write conftest_FILE conftest.'ac_ext'
$ write conftest_FILE "#include ""confdefs.h"""
$ write conftest_FILE "#include <string.h>"
$ close conftest_FILE
$ set noon
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ ac_preprocess
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ search/out=nla0: conftest.out "memchr"
$ _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ set on
$ if _tmp .eq. 1
$  then
$ delete/nolog conftest*.*.*
$!# SGI's /bin/cc from Irix-4.0.5 gets non-ANSI ctype macros unless using -ansi.
$   open/write conftest_file conftest.'ac_ext'
$   write conftest_file "#include ""confdefs.h"""
$   write conftest_file "#include <ctype.h>"
$   write conftest_file "#define ISLOWER(c) ('a' <= (c) && (c) <= 'z')"
$   write conftest_file "#define TOUPPER(c) (ISLOWER(c) ? 'A' + ((c) - 'a') : (c))"
$   write conftest_file "#define XOR(e,f) (((e) && !(f)) || (!(e) && (f)))"
$   write conftest_file "void exit();"
$   write conftest_file "int main () { int i; for (i = 0; i < 256; i++)"
$   write conftest_file "if (XOR (islower (i), ISLOWER (i)) || toupper (i) != TOUPPER (i)) exit(2);"
$   write conftest_file "exit (0); }"
$   write conftest_file ""
$   close conftest_file
$   ac_file_name := conftest.'ac_ext'
$   ac_default_name := conftest.'ac_ext'
$   if ac_file_name .nes. ac_default_name then copy 'ac_file_name 'ac_default_name
$   set noon
$   _tmp = -1 ! Because DCL can't jump into the middle of an IF stmt
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if f$search("conftest.obj") .eqs. "" then goto ac_tp_VMS_LABLE0
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .ne. 1 then goto ac_tp_VMS_LABLE0
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   run conftest
$   _tmp = $status
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   set on
$ ac_tp_VMS_LABLE0:
$   if _tmp .eq. 1 .or. _tmp .eq. 0 ! UNIXoid programs return 0 for success
$    then
$!# ISC 2.0.2 stdlib.h does not declare free, contrary to ANSI.
$ open/write conftest_FILE conftest.'ac_ext'
$ write conftest_FILE "#include ""confdefs.h"""
$ write conftest_FILE "#include <stdlib.h>"
$ close conftest_FILE
$ set noon
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ ac_preprocess
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ search/out=nla0: conftest.out "free"
$ _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ set on
$ if _tmp .eq. 1
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_STDC_HEADERS = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining STDC_HEADERS"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define STDC_HEADERS 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """STDC_HEADERS""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$  else
$!
$    endif
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$ ac_hh_l = "string.h stdlib.h memory.h"
$ ac_hh_i = 0
$ac_hh_VMS_LABLE0:
$ ac_hh_hdr_orig = f$element(ac_hh_i," ",ac_hh_l)
$ ac_hh_hdr = "HAVE_" + ac_hh_hdr_orig
$ ac_hh_i = ac_hh_i + 1
$ if ac_hh_hdr_orig .nes. " "
$  then
$   ac_hh_tr_hdr = ""
$   ac_hh_c1 = "."
$   ac_hh_c2 = "/"
$   ac_hh_c = 1
$   ac_hh_j = 0
$ac_hh_VMS_LABLE1:
$   ac_hh_e = f$element(ac_hh_j,ac_hh_c'ac_hh_c',ac_hh_hdr)
$   ac_hh_j = ac_hh_j + 1
$   if ac_hh_e .eqs. ac_hh_c'ac_hh_c'
$    then
$     if ac_hh_c .eq. 1
$      then
$       ac_hh_c = 2
$       ac_hh_hdr = ac_hh_tr_hdr
$       ac_hh_tr_hdr = ""
$       ac_hh_j = 0
$       goto ac_hh_VMS_LABLE1
$      endif
$    else
$     if ac_hh_tr_hdr .nes. "" then -
	ac_hh_tr_hdr = ac_hh_tr_hdr + "_"
$     ac_hh_tr_hdr = ac_hh_tr_hdr + f$edit(ac_hh_e,"UPCASE")
$     goto ac_hh_VMS_LABLE1
$    endif
$ if .not. silent then write sys$output "checking ","for ''ac_hh_hdr_orig'"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <''ac_hh_hdr_orig'>"
$ write conftest_file ""
$ close conftest_file
$ set noon
$ def/user sys$output conftest.err
$ def/user sys$error conftest.err
$ 'ac_cpp' nla0: conftest.'ac_ext'
$ set on
$ ac_err := YES
$ if f$search("conftest.err") .nes. "" then -
	if f$file("conftest.err","EOF") .ne. 0 then -
	ac_err := NO
$ if ac_err
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_'ac_hh_tr_hdr' = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining ''ac_hh_tr_hdr'"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define ''ac_hh_tr_hdr' 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """''ac_hh_tr_hdr'""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$   goto ac_hh_VMS_LABLE0
$  endif
$
$ ac_l = "strerror bcopy "+-
""
$ ac_i = 0
$ac_hf_VMS_LABLE0:
$ ac_func = f$element(ac_i," ",ac_l)
$ ac_i = ac_i + 1
$ if ac_func .nes. " "
$  then
$   if ac_func .nes. ""
$    then
$     !# Translates all lower case characters to upper case
$     ac_tr_func := HAVE_'ac_func'
$ if .not. silent then write sys$output "checking ","for ''ac_func'"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <ctype.h>"
$ write conftest_file ""
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { /* The GNU C library defines this for functions which it implements"
$ write conftest_file "    to always fail with ENOSYS.  Some functions are actually named"
$ write conftest_file "    something starting with __ and the normal name is an alias.  */"
$ write conftest_file "#if defined (__stub_''ac_func') || defined (__stub___''ac_func')"
$ write conftest_file "choke me"
$ write conftest_file "#else"
$ write conftest_file "/* Override any gcc2 internal prototype to avoid an error.  */"
$ write conftest_file "extern char ''ac_func'(); ''ac_func'();"
$ write conftest_file "#endif"
$ write conftest_file "; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_'ac_tr_func' = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining ''ac_tr_func'"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define ''ac_tr_func' 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """''ac_tr_func'""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$    endif
$   goto ac_hf_VMS_LABLE0
$  endif 
$
$!# The Ultrix 4.2 mips built in alloca declared by alloca.h only works
$!# for constant arguments.  Useless!
$ if .not. silent then write sys$output "checking ","for working alloca.h"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <alloca.h>"
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { char *p = alloca(2 * sizeof(int));; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_HAVE_ALLOCA_H = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining HAVE_ALLOCA_H"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define HAVE_ALLOCA_H 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """HAVE_ALLOCA_H""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$ if .not. silent then write sys$output "checking ","for alloca"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#ifdef __GNUC__"
$ write conftest_file "#define alloca __builtin_alloca"
$ write conftest_file "#else"
$ write conftest_file "#ifdef __DECC"
$ write conftest_file "#include <builtins.h>"
$ write conftest_file "#define alloca __ALLOCA"
$ write conftest_file "#else"
$ write conftest_file "#if HAVE_ALLOCA_H"
$ write conftest_file "#include <alloca.h>"
$ write conftest_file "#else"
$ write conftest_file "#ifdef _AIX"
$ write conftest_file " #pragma alloca"
$ write conftest_file "#else"
$ write conftest_file "char *alloca ();"
$ write conftest_file "#endif"
$ write conftest_file "#endif"
$ write conftest_file "#endif"
$ write conftest_file "#endif"
$ write conftest_file ""
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { char *p = (char *) alloca(1);; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_HAVE_ALLOCA = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining HAVE_ALLOCA"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define HAVE_ALLOCA 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """HAVE_ALLOCA""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  else
$ delete/nolog conftest*.*.*
$ ac_alloca_missing=1
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file ""
$ write conftest_file "#if defined(CRAY) && ! defined(CRAY2)"
$ write conftest_file "winnitude"
$ write conftest_file "#else"
$ write conftest_file "lossage"
$ write conftest_file "#endif"
$ write conftest_file ""
$ close conftest_file
$ set noon
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_preprocess
$ search/out=nla0: conftest.out "winnitude"
$ _tmp = $status
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ set on
$ if _tmp .eq. 1
$  then
$ delete/nolog conftest*.*.*
$ if .not. silent then write sys$output "checking ","for _getb67"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <ctype.h>"
$ write conftest_file ""
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { /* The GNU C library defines this for functions which it implements"
$ write conftest_file "    to always fail with ENOSYS.  Some functions are actually named"
$ write conftest_file "    something starting with __ and the normal name is an alias.  */"
$ write conftest_file "#if defined (__stub__getb67) || defined (__stub____getb67)"
$ write conftest_file "choke me"
$ write conftest_file "#else"
$ write conftest_file "#ifndef _getb67 /* It might be implemented as a macro, as with DEC C for AXP */"
$ write conftest_file "/* Override any gcc2 internal prototype to avoid an error.  */"
$ write conftest_file "extern char _getb67();"
$ write conftest_file "#endif"
$ write conftest_file "_getb67();"
$ write conftest_file "#endif"
$ write conftest_file "; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_CRAY_STACKSEG_END = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining CRAY_STACKSEG_END to be _getb67"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define CRAY_STACKSEG_END _getb67"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """CRAY_STACKSEG_END""=""_getb67"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  else
$ delete/nolog conftest*.*.*
$ if .not. silent then write sys$output "checking ","for GETB67"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <ctype.h>"
$ write conftest_file ""
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { /* The GNU C library defines this for functions which it implements"
$ write conftest_file "    to always fail with ENOSYS.  Some functions are actually named"
$ write conftest_file "    something starting with __ and the normal name is an alias.  */"
$ write conftest_file "#if defined (__stub_GETB67) || defined (__stub___GETB67)"
$ write conftest_file "choke me"
$ write conftest_file "#else"
$ write conftest_file "#ifndef GETB67 /* It might be implemented as a macro, as with DEC C for AXP */"
$ write conftest_file "/* Override any gcc2 internal prototype to avoid an error.  */"
$ write conftest_file "extern char GETB67();"
$ write conftest_file "#endif"
$ write conftest_file "GETB67();"
$ write conftest_file "#endif"
$ write conftest_file "; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_CRAY_STACKSEG_END = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining CRAY_STACKSEG_END to be GETB67"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define CRAY_STACKSEG_END GETB67"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """CRAY_STACKSEG_END""=""GETB67"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  else
$ delete/nolog conftest*.*.*
$ if .not. silent then write sys$output "checking ","for getb67"
$ open/write conftest_file conftest.'ac_ext'
$ write conftest_file "#include ""confdefs.h"""
$ write conftest_file "#include <ctype.h>"
$ write conftest_file ""
$ write conftest_file "int main() { return 1; }"
$ write conftest_file "int t() { /* The GNU C library defines this for functions which it implements"
$ write conftest_file "    to always fail with ENOSYS.  Some functions are actually named"
$ write conftest_file "    something starting with __ and the normal name is an alias.  */"
$ write conftest_file "#if defined (__stub_getb67) || defined (__stub___getb67)"
$ write conftest_file "choke me"
$ write conftest_file "#else"
$ write conftest_file "/* Override any gcc2 internal prototype to avoid an error.  */"
$ write conftest_file "extern char getb67(); getb67();"
$ write conftest_file "#endif"
$ write conftest_file "; return 0; }"
$ close conftest_file
$ _ac_cc_flag = "NO"
$!# def/user sys$error nla0:
$!# def/user sys$output nla0:
$ set noon
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$ ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$ if f$search("conftest.obj") .nes. ""
$  then
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .eq. 1
$    then
$     _ac_cc_flag = "YES"
$    endif
$  endif
$ set on
$ if _ac_cc_flag
$  then
$ delete/nolog conftest*.*.*
$!#
$ _ac_is_defined_CRAY_STACKSEG_END = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining CRAY_STACKSEG_END to be getb67"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define CRAY_STACKSEG_END getb67"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """CRAY_STACKSEG_END""=""getb67"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$  endif
$ dummy = f$search("foo.bar.1") ! To prevent f$search bug.
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$ if "''ac_alloca_missing'" .nes. ""
$  then
$   !# The SVR3 libPW and SVR4 libucb both contain incompatible functions
$   !# that cause trouble.  Some versions do not even contain alloca or
$   !# contain a buggy version.  If you still want to use their alloca,
$   !# use ar to extract alloca.o from them instead of compiling alloca.c.
$   ALLOCA=",alloca.obj"
$!#
$ _ac_is_defined_C_ALLOCA = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining C_ALLOCA"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define C_ALLOCA 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """C_ALLOCA""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$
$ if .not. silent then write sys$output "checking ","stack direction for C alloca"
$ if .not. silent then write sys$output "checking ","whether cross-compiling"
$!# If we cannot run a trivial program, we must be cross compiling.
$   open/write conftest_file conftest.'ac_ext'
$   write conftest_file "#include ""confdefs.h"""
$   write conftest_file "void exit();main(){exit(0);}"
$   close conftest_file
$   ac_file_name := conftest.'ac_ext'
$   ac_default_name := conftest.'ac_ext'
$   if ac_file_name .nes. ac_default_name then copy 'ac_file_name 'ac_default_name
$   set noon
$   _tmp = -1 ! Because DCL can't jump into the middle of an IF stmt
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if f$search("conftest.obj") .eqs. "" then goto ac_tp_VMS_LABLE1
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .ne. 1 then goto ac_tp_VMS_LABLE1
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   run conftest
$   _tmp = $status
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   set on
$ ac_tp_VMS_LABLE1:
$   if _tmp .eq. 1 .or. _tmp .eq. 0 ! UNIXoid programs return 0 for success
$    then
$!
$  else
$ cross_compiling=1
$    endif
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$
$ if "''cross_compiling'" .nes. ""
$  then
$!#
$ _ac_is_defined_STACK_DIRECTION = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining STACK_DIRECTION to be 0"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define STACK_DIRECTION 0"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """STACK_DIRECTION""=""0"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  else
$   open/write conftest_file conftest.'ac_ext'
$   write conftest_file "#include ""confdefs.h"""
$   write conftest_file "find_stack_direction ()"
$   write conftest_file "{"
$   write conftest_file "  static char *addr = 0;"
$   write conftest_file "  auto char dummy;"
$   write conftest_file "  if (addr == 0)"
$   write conftest_file "    {"
$   write conftest_file "      addr = &dummy;"
$   write conftest_file "      return find_stack_direction ();"
$   write conftest_file "    }"
$   write conftest_file "  else"
$   write conftest_file "    return (&dummy > addr) ? 1 : -1;"
$   write conftest_file "}"
$   write conftest_file "void exit();"
$   write conftest_file "main ()"
$   write conftest_file "{"
$   write conftest_file "  exit (find_stack_direction() < 0 ? 2 : 1); /* slightly changed for VMS */"
$   write conftest_file "}"
$   close conftest_file
$   ac_file_name := conftest.'ac_ext'
$   ac_default_name := conftest.'ac_ext'
$   if ac_file_name .nes. ac_default_name then copy 'ac_file_name 'ac_default_name
$   set noon
$   _tmp = -1 ! Because DCL can't jump into the middle of an IF stmt
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_compile
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if f$search("conftest.obj") .eqs. "" then goto ac_tp_VMS_LABLE2
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   ac_link
$   _tmp = $severity
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   if _tmp .ne. 1 then goto ac_tp_VMS_LABLE2
$ __debug = f$trnlnm("DEBUG_CONFIGURE")
$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment("MESSAGE")
$ if .not. __debug then set message/nofacility/noident/noseverity/notext
$ if .not. __debug then def/user sys$output nl:
$ if .not. __debug then def/user sys$error nl:
$   run conftest
$   _tmp = $status
$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'
$
$   set on
$ ac_tp_VMS_LABLE2:
$   if _tmp .eq. 1 .or. _tmp .eq. 0 ! UNIXoid programs return 0 for success
$    then
$!#
$ _ac_is_defined_STACK_DIRECTION = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining STACK_DIRECTION to be 1"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define STACK_DIRECTION 1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """STACK_DIRECTION""=""1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$  else
$!#
$ _ac_is_defined_STACK_DIRECTION = 1
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ if f$type(DEFS) .nes. "" then DEFS = DEFS + ","
$ if f$type(DEFS) .eqs. "" then DEFS:=
$ DEFS_COUNTER = DEFS_COUNTER + 1
$ if "''verbose'" then write sys$output -
"	defining STACK_DIRECTION to be -1"
$ open/append ac_define_file confdefs.h
$ write ac_define_file "#define STACK_DIRECTION -1"
$ close ac_define_file
$ DEFS'DEFS_COUNTER' = """STACK_DIRECTION""=""-1"""
$ DEFS = DEFS + DEFS'DEFS_COUNTER'
$
$    endif
$  endif
$ if f$search("CONFTEST*.*.*") .nes. "" then delete/nolog conftest*.*.*
$
$
$ endif
$
$ search 'srcdir'version.c "char *version"/output=config.ver_tmp
$ open/read foo config.ver_tmp
$ read foo line
$ close foo
$ delete config.ver_tmp;*
$ version = f$element(0,"\",f$element(1,"""",line))
$ version = f$extract(f$locate("version ",version)+8,f$length(version),version)
$
$ version_us = version
$ len = f$length(version_us)
$loopversion:
$ p = f$locate(".",version_us)
$ if p .lt. len
$  then
$   version_us = f$extract(0,p,version_us) + "_" + -
	f$extract(p+1,len-p-1,version_us)
$   goto loopversion
$  endif
$


$!# The preferred way to propogate these variables is regular @ substitutions.
$ if "''prefix'" .nes. ""
$  then
$   prefix_dir = f$parse(prefix,,,"DIRECTORY") - "[" - "]" - "<" - ">"
$   prefix_dev = f$parse(prefix,,,"DEVICE")
$  else
$   prefix_dev := sys$sysdevice:
$   prefix_dir := gnu
$   prefix := sys$sysdevice:[gnu]
$  endif
$ if "''exec_prefix'" .nes. ""
$  then
$   exec_prefix := 'exec_prefix'
$   exec_prefix_dir = f$parse(exec_prefix,,,"DIRECTORY")-"[" - "]" - "<" - ">"
$   exec_prefix_dev = f$parse(exec_prefix,,,"DEVICE")
$  else
$   exec_prefix_dev = prefix_dev
$   exec_prefix_dir = prefix_dir
$   exec_prefix = prefix
$  endif
$ ac_prsub_counter = 0
$ if "''startupdir'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""startupdir""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""startupdir""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='startupdir'"
$   ac_prsub'ac_prsub_counter'="res := """"startupdir"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''dcltable'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""dcltable""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""dcltable""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='dcltable'"
$   ac_prsub'ac_prsub_counter'="res := """"dcltable"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''prefix_dev'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""prefix_dev""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""prefix_dev""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='prefix_dev'"
$   ac_prsub'ac_prsub_counter'="res := """"prefix_dev"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''prefix_dir'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""prefix_dir""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""prefix_dir""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='prefix_dir'"
$   ac_prsub'ac_prsub_counter'="res := """"prefix_dir"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''prefix'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""prefix""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""prefix""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='prefix'"
$   ac_prsub'ac_prsub_counter'="res := """"prefix"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''exec_prefix_dev'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""exec_prefix_dev""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""exec_prefix_dev""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='exec_prefix_dev'"
$   ac_prsub'ac_prsub_counter'="res := """"exec_prefix_dev"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''exec_prefix_dir'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""exec_prefix_dir""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""exec_prefix_dir""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='exec_prefix_dir'"
$   ac_prsub'ac_prsub_counter'="res := """"exec_prefix_dir"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$ if "''exec_prefix'" .nes. ""
$  then
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""exec_prefix""""+ ((p_0_or_more_spc +""""="""")@r1))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r = 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(beginning_of (main_buffer));"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'= "r := search_quietly(LINE_BEGIN+(""""exec_prefix""""+ ((p_0_or_more_spc +""""="""")@r1)+scan(""""""""))@r0+LINE_END,FORWARD);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="if (r <> 0)"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="then"
$   ac_prsub_counter = ac_prsub_counter + 1
$! This is a piece of deep DCL magic.
$   ac_prsub_support'ac_prsub_counter'="__ac_prsub_tmp:='exec_prefix'"
$   ac_prsub'ac_prsub_counter'="res := """"exec_prefix"""" + """"=""""+""""'"+"'__ac_prsub_tmp'"""";"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="erase(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="position(r0);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="copy_text(res);"
$   ac_prsub_counter = ac_prsub_counter + 1
$   ac_prsub'ac_prsub_counter'="endif;"
$  endif
$!#
$
$
$ set noon
$ if f$search("CONFIG.STATUS.*") .nes. "" then delete/nolog config.status.*
$ set on
$! trap 'rm -f config.status; exit 1' 1 2 15
$ write sys$output "creating config.status"
$ open/write config_status_file config.status
$ write config_status_file "$! Generated automatically by CONFIGURE.COM."
$ write config_status_file "$! Run this file to recreate the current configuration."
$ write config_status_file "$! This directory was configured as follows,"
$ write config_status_file "$! on host "+f$getsyi("NODENAME")+":"
$ write config_status_file "$!"
$ __tmp_p = f$env("PROCEDURE")
$ __tmp_p = f$parse(__tmp_p,,,"DEVICE")+f$parse(__tmp_p,,,"DIRECTORY")+ -
	f$parse(__tmp_p,,,"NAME")+f$parse(__tmp_p,,,"TYPE")
$ write config_status_file "$! @"+__tmp_p+" "+configure_args
$ write config_status_file "$"
$ write config_status_file "$! A trick to read the whole file before processing..."
$ write config_status_file "$! This way, CONFIG.STATUS will not complain at the end of a --RECHECK."
$ write config_status_file "$ goto config_status_end"
$ write config_status_file "$config_status_begin:"
$ write config_status_file "$"
$ write config_status_file "$ ac_cs_usage = ""Usage: config.status [--recheck] [--version] [--help]"
$ write config_status_file "$ ac_will_recheck := no"
$ write config_status_file "$ ac_recheck_extra_args = """""
$ write config_status_file "$ ac_count = 8"
$ write config_status_file "$config_status_loop1:"
$ write config_status_file "$ if ac_count .gt. 0
$ write config_status_file "$  then
$ write config_status_file "$   if p'ac_count' .eqs. """""
$ write config_status_file "$    then"
$ write config_status_file "$     ac_count = ac_count - 1"
$ write config_status_file "$     goto config_status_loop1"
$ write config_status_file "$    endif"
$ write config_status_file "$  endif"
$ write config_status_file "$ i = 0"
$ write config_status_file "$config_status_loop2:"
$ write config_status_file "$ i = i + 1"
$ write config_status_file "$ if i .le. ac_count"
$ write config_status_file "$  then"
$ write config_status_file "$   if p'i' .eqs. ""-RECHECK"" -"
$ write config_status_file "	.or. p'i' .eqs. f$extract(0,f$length(p'i'),""--RECHECK"")"
$ write config_status_file "$    then"
$ write config_status_file "$     ac_will_recheck := yes"
$ write config_status_file "$     goto config_status_loop2"
$ write config_status_file "$    endif"
$ write config_status_file "$   if f$extract(0,f$locate(""="",p'i'),p'i') .eqs. f$extract(0,f$locate(""="",p'i'),""--NEW-ARGS"")"
$ write config_status_file "$    then"
$ write config_status_file "$     tmp = f$extract(f$locate(""="",p'i'),999,p'i')"
$ write config_status_file "$     if tmp .nes. """" then ac_recheck_extra_args = ac_recheck_extra_args + "" "" + (tmp - ""="")"
$ write config_status_file "$     goto config_status_loop2"
$ write config_status_file "$    endif"
$ write config_status_file "$   if p'i' .eqs. ""-VERSION"" -"
$ write config_status_file "	.or. p'i' .eqs. f$extract(0,f$length(p'i'),""--VERSION"")"
$ write config_status_file "$    then"
$ write config_status_file "$     write sys$output ""config.status generated by autoconf version 1.11"""
$ write config_status_file "$     exit 1"
$ write config_status_file "$    endif"
$ write config_status_file "$   if p'i' .eqs. ""-HELP"" -"
$ write config_status_file "	.or. p'i' .eqs. f$extract(0,f$length(p'i'),""--HELP"")"
$ write config_status_file "$    then"
$ write config_status_file "$     write sys$output ac_cs_usage"
$ write config_status_file "$     exit $status"
$ write config_status_file "$    endif"
$ write config_status_file "$   if p'i' .nes. """"
$ write config_status_file "$    then
$ write config_status_file "$     write sys$error ac_cs_usage"
$ write config_status_file "$     exit 0"
$ write config_status_file "$    endif"
$ write config_status_file "$   goto config_status_loop2"
$ write config_status_file "$  endif"
$ write config_status_file "$"
$ write config_status_file "$ if ac_will_recheck"
$ write config_status_file "$  then"
$ write config_status_file "$   write sys$output ""@"+__tmp_p+" "+configure_args+""",ac_recheck_extra_args"
$ write config_status_file "$   @"+__tmp_p+" "+f$edit(configure_args,"TRIM")+"'ac_recheck_extra_args'"
$ write config_status_file "$   exit $status"
$ write config_status_file "$  endif"
$ write config_status_file "$"
$ write config_status_file "$ startupdir=""''startupdir'""" ! Experimental
$ write config_status_file "$ dcltable=""''dcltable'""" ! Experimental
$ write config_status_file "$ LDFLAGS=""''LDFLAGS'""" ! Experimental
$ write config_status_file "$ CC=""''CC'""" ! Experimental
$ write config_status_file "$ sys_includes=""''sys_includes'""" ! Experimental
$ write config_status_file "$ OPTS=""''OPTS'""" ! Experimental
$ write config_status_file "$ LIBS=""''LIBS'""" ! Experimental
$ write config_status_file "$ CFLAGS=""''CFLAGS'""" ! Experimental
$ write config_status_file "$ CPP=""''CPP'""" ! Experimental
$ write config_status_file "$ INSTALL=""''INSTALL'""" ! Experimental
$ write config_status_file "$ INSTALL_PROGRAM=""''INSTALL_PROGRAM'""" ! Experimental
$ write config_status_file "$ INSTALL_DATA=""''INSTALL_DATA'""" ! Experimental
$ write config_status_file "$ INSTALL_QUOTED=""''INSTALL_QUOTED'""" ! Experimental
$ write config_status_file "$ INSTALL_PROGRAM_QUOTED=""''INSTALL_PROGRAM_QUOTED'""" ! Experimental
$ write config_status_file "$ INSTALL_DATA_QUOTED=""''INSTALL_DATA_QUOTED'""" ! Experimental
$ write config_status_file "$ ALLOCA=""''ALLOCA'""" ! Experimental
$ write config_status_file "$ version=""''version'""" ! Experimental
$ write config_status_file "$ version_us=""''version_us'""" ! Experimental
$ write config_status_file "$ srcdir_dev=""''srcdir_dev'""" ! Experimental
$ write config_status_file "$ srcdir_dir=""''srcdir_dir'""" ! Experimental
$ write config_status_file "$ srcdir=""''srcdir'""" ! Experimental
$ write config_status_file "$ top_srcdir_dev=""''top_srcdir_dev'""" ! Experimental
$ write config_status_file "$ top_srcdir_dir=""''top_srcdir_dir'""" ! Experimental
$ write config_status_file "$ top_srcdir=""''top_srcdir'""" ! Experimental
$ write config_status_file "$ prefix_dev=""''prefix_dev'""" ! Experimental
$ write config_status_file "$ prefix_dir=""''prefix_dir'""" ! Experimental
$ write config_status_file "$ prefix=""''prefix'""" ! Experimental
$ write config_status_file "$ exec_prefix_dev=""''exec_prefix_dev'""" ! Experimental
$ write config_status_file "$ exec_prefix_dir=""''exec_prefix_dir'""" ! Experimental
$ write config_status_file "$ exec_prefix=""''exec_prefix'""" ! Experimental
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ DEFSC = 0
$ DEFS_prefix = "$ DEFS:="
$DEFS_SUBST_VMS_LABLE2:
$ DEFSC = DEFSC + 1
$ if DEFSC .le. DEFS_COUNTER
$  then
$   DEFS = DEFS'DEFSC'
$
$   DEFS_ACC = f$element(0,"""",DEFS)
$   DEFSI = 1
$DEFS_SUBST_VMS_LABLE4:
$   DEFS_EL = f$element(DEFSI,"""",DEFS)
$   if DEFS_EL .nes. """"
$    then
$     DEFSI = DEFSI + 1
$     DEFS_ACC = DEFS_ACC + """""""""" + DEFS_EL
$     goto DEFS_SUBST_VMS_LABLE4
$    endif
$   if DEFSC .lt. DEFS_COUNTER
$    then
$     write config_status_file "''DEFS_prefix'""''DEFS_ACC'"",-"
$    else
$     write config_status_file "''DEFS_prefix'""''DEFS_ACC'"""
$    endif
$   DEFS_prefix = ""
$   goto DEFS_SUBST_VMS_LABLE2
$  endif
$
$ DEFSC = 0
$ QUOTED_DEFS_prefix = "$ QUOTED_DEFS:="
$DEFS_SUBST_VMS_LABLE3:
$ DEFSC = DEFSC + 1
$ if DEFSC .le. DEFS_COUNTER
$  then
$   DEFS = DEFS'DEFSC'
$
$   QUOTED_DEFS_ACC = f$element(0,"""",DEFS)
$   DEFSI = 1
$DEFS_SUBST_VMS_LABLE5:
$   DEFS_EL = f$element(DEFSI,"""",DEFS)
$   if DEFS_EL .nes. """"
$    then
$     DEFSI = DEFSI + 1
$     QUOTED_DEFS_ACC = QUOTED_DEFS_ACC + """""""""""""""""" + DEFS_EL
$     goto DEFS_SUBST_VMS_LABLE5
$    endif
$   if DEFSC .lt. DEFS_COUNTER
$    then
$     write config_status_file "''QUOTED_DEFS_prefix'""''QUOTED_DEFS_ACC'"",-"
$    else
$     write config_status_file "''QUOTED_DEFS_prefix'""''QUOTED_DEFS_ACC'"""
$    endif
$   QUOTED_DEFS_prefix = ""
$   goto DEFS_SUBST_VMS_LABLE3
$  endif
$ ac_i = 0
$ac_prsub_loop:
$ ac_i = ac_i + 1
$ if f$type(ac_prsub'ac_i') .nes. ""
$  then"
$   if f$type(ac_prsub_support'ac_i') .nes. ""
$    then
$     ac_tmp = ac_prsub_support'ac_i'
$     write config_status_file "$   ",ac_tmp
$    endif
$   ac_tmp = ac_prsub'ac_i'
$   write config_status_file "$   ac_prsub''ac_i'=""",ac_tmp,""""
$   goto ac_prsub_loop
$  endif
$
$ ac_i = 0
$ac_vpsub_loop:
$ ac_i = ac_i + 1
$ if f$type(ac_vpsub'ac_i') .nes. ""
$  then"
$   if f$type(ac_vpsub_support'ac_i') .nes. ""
$    then
$     ac_tmp = ac_vpsub_support'ac_i'
$     write config_status_file "$   ",ac_tmp
$    endif
$   ac_tmp = ac_vpsub'ac_i'
$   write config_status_file "$   ac_vpsub''ac_i'=""",ac_tmp,""""
$   goto ac_vpsub_loop
$  endif
$
$ ac_i = 0
$ac_extrasub_loop:
$ ac_i = ac_i + 1
$ if f$type(ac_extrasub'ac_i') .nes. ""
$  then"
$   ac_tmp = ac_extrasub'ac_i'
$   write config_status_file "$   ac_extrasub''ac_i'=""''ac_tmp'"""
$   goto ac_extrasub_loop
$  endif
$ write config_status_file "$ startupdir := 'startupdir'"
$ write config_status_file "$ dcltable := 'dcltable'"
$ write config_status_file "$ LDFLAGS := 'LDFLAGS'"
$ write config_status_file "$ CC := 'CC'"
$ write config_status_file "$ sys_includes := 'sys_includes'"
$ write config_status_file "$ OPTS := 'OPTS'"
$ write config_status_file "$ LIBS := 'LIBS'"
$ write config_status_file "$ CFLAGS := 'CFLAGS'"
$ write config_status_file "$ CPP := 'CPP'"
$ write config_status_file "$ INSTALL := 'INSTALL'"
$ write config_status_file "$ INSTALL_PROGRAM := 'INSTALL_PROGRAM'"
$ write config_status_file "$ INSTALL_DATA := 'INSTALL_DATA'"
$ write config_status_file "$ INSTALL_QUOTED := 'INSTALL_QUOTED'"
$ write config_status_file "$ INSTALL_PROGRAM_QUOTED := 'INSTALL_PROGRAM_QUOTED'"
$ write config_status_file "$ INSTALL_DATA_QUOTED := 'INSTALL_DATA_QUOTED'"
$ write config_status_file "$ ALLOCA := 'ALLOCA'"
$ write config_status_file "$ version := 'version'"
$ write config_status_file "$ version_us := 'version_us'"
$ write config_status_file "$ srcdir_dev := 'srcdir_dev'"
$ write config_status_file "$ srcdir_dir := 'srcdir_dir'"
$ write config_status_file "$ srcdir := 'srcdir'"
$ write config_status_file "$ top_srcdir_dev := 'top_srcdir_dev'"
$ write config_status_file "$ top_srcdir_dir := 'top_srcdir_dir'"
$ write config_status_file "$ top_srcdir := 'top_srcdir'"
$ write config_status_file "$ prefix_dev := 'prefix_dev'"
$ write config_status_file "$ prefix_dir := 'prefix_dir'"
$ write config_status_file "$ prefix := 'prefix'"
$ write config_status_file "$ exec_prefix_dev := 'exec_prefix_dev'"
$ write config_status_file "$ exec_prefix_dir := 'exec_prefix_dir'"
$ write config_status_file "$ exec_prefix := 'exec_prefix'"
$ write config_status_file "$"
$ write config_status_file "$ ac_given_srcdir=srcdir"
$ write config_status_file "$ ac_given_srcdir_dev=srcdir_dev"
$ write config_status_file "$ ac_given_srcdir_dir=srcdir_dir"
$ write config_status_file "$"
$ write config_status_file "$ if f$type(CONFIG_FILES) .eqs. """" then CONFIG_FILES = ""[-]"" +-"
$ write config_status_file "	"" descrip.mms"" +-"
$ write config_status_file "	"""""
$ write config_status_file "$! CONFIG_FILE_i = -1"
$ write config_status_file "$ CONFIG_FILE_i = 0"
$ write config_status_file "$config_status_loop3:"
$ write config_status_file "$! if CONFIG_FILE_i .eq. -1"
$ write config_status_file "$!  then"
$ write config_status_file "$!   ac_file = ""[-]"""
$ write config_status_file "$!  else"
$ write config_status_file "$   ac_file = f$element(CONFIG_FILE_i,"" "",CONFIG_FILES)"
$ write config_status_file "$!  endif"
$ write config_status_file "$ dummy = ""'"+"'ac_file'""" ! debug
$ write config_status_file "$ CONFIG_FILE_i = CONFIG_FILE_i + 1"
$ write config_status_file "$ if ac_file .eqs. ""[-]"" then goto config_status_loop3"
$ write config_status_file "$ if ac_file .nes. "" """
$ write config_status_file "$  then"
$ write config_status_file "$   ac_dir:="
$ write config_status_file "$   ac_dir_tmp = ac_file"
$ write config_status_file "$  ac_loop_dir1:"
$ write config_status_file "$   ac_dir_e = f$element(0,""]"",ac_dir_tmp)"
$ write config_status_file "$   if ac_dir_e .eqs. ac_dir_tmp then goto ac_loop_dir2"
$ write config_status_file "$   ac_dir_e = ac_dir_e + ""]"""
$ write config_status_file "$   ac_dir = ac_dir + ac_dir_e"
$ write config_status_file "$   ac_dir_tmp = ac_dir_tmp - ac_dir_e"
$ write config_status_file "$   goto ac_loop_dir1
$ write config_status_file "$  ac_loop_dir2:"
$ write config_status_file "$   ac_dir_e = f$element(0,"">"",ac_dir_tmp)"
$ write config_status_file "$   if ac_dir_e .eqs. ac_dir_tmp then goto ac_end_dir"
$ write config_status_file "$   ac_dir_e = ac_dir_e + "">"""
$ write config_status_file "$   ac_dir = ac_dir + ac_dir_e"
$ write config_status_file "$   ac_dir_tmp = ac_dir_tmp - ac_dir_e"
$ write config_status_file "$   goto ac_loop_dir2"
$ write config_status_file "$  ac_end_dir:"
$ write config_status_file "$   if ac_dir .nes. ac_file .and. ac_dir .nes. """" -"
$ write config_status_file "	.and. ac_dir .nes. ""[]"""
$ write config_status_file "$    then"
$ write config_status_file "$     !# The file is in a subdirectory"
$ write config_status_file "$     set noon"
$ write config_status_file "$     if f$parse(ac_dir) .eqs. """" then create/dir 'ac_dir'"
$ write config_status_file "$     set on"
$ write config_status_file "$     ac_dir_tmp = ac_dir - ""[."" - ""<."" - ""["" - ""<"" - ""]"" - "">"""
$ write config_status_file "$     ac_dir_suffix:=.'ac_dir_tmp'"
$ write config_status_file "$    else"
$ write config_status_file "$     ac_dir_suffix:="
$ write config_status_file "$    endif"
$ write config_status_file "$"
$ write config_status_file "$   ac_dashes:="
$ write config_status_file "$   ac_dir_tmp=ac_dir_suffix"
$ write config_status_file "$  ac_loop_dashes:"
$ write config_status_file "$   ac_dir_e = f$element(1,""."",ac_dir_tmp)"
$ write config_status_file "$   if ac_dir_e .eqs. ""."" then goto ac_end_dashes"
$ write config_status_file "$   ac_dashes = ac_dashes + ""-"""
$ write config_status_file "$   ac_dir_e = "".""+ac_dir_e"
$ write config_status_file "$   ac_dir_tmp = ac_dir_tmp - ac_dir_e"
$ write config_status_file "$   goto ac_loop_dashes"
$ write config_status_file "$  ac_end_dashes:"
$ write config_status_file "$   if ac_given_srcdir .eqs. ""[]"""
$ write config_status_file "$    then"
$ write config_status_file "$     srcdir := []"
$ write config_status_file "$     srcdir_dev :="
$ write config_status_file "$     srcdir_dir :="
$ write config_status_file "$     if ac_dir_suffix .eqs. """"
$ write config_status_file "$      then"
$ write config_status_file "$       top_srcdir := []"
$ write config_status_file "$       top_srcdir_dev :="
$ write config_status_file "$       top_srcdir_dir :="
$ write config_status_file "$      else"
$ write config_status_file "$       top_srcdir := ['ac_dashes']"
$ write config_status_file "$       top_srcdir_dev :="
$ write config_status_file "$       top_srcdir_dir = ac_dashes"
$ write config_status_file "$      endif"
$ write config_status_file "$    else"
$ write config_status_file "$     if f$extract(0,2,ac_given_srcdir_dir) .eqs. ""[."""
$ write config_status_file "$      then !# Relative path."
$ write config_status_file "$       if ac_dashes .nes. """" then ac_dashes = ac_dashes + ""."""
$ write config_status_file "$       srcdir_dir := 'ac_dashes'"+"'ac_given_srcdir_dir'"+"'ac_dir_suffix'"
$ write config_status_file "$       srcdir_dev := 'ac_given_srcdir_dev'"
$ write config_status_file "$       srcdir := 'srcdir_dev'['srcdir_dir']"
$ write config_status_file "$       top_srcdir_dev := 'ac_given_srcdir_dev'"
$ write config_status_file "$       top_srcdir_dir := 'ac_dashes'"+"'ac_given_srcdir_dir'"
$ write config_status_file "$       top_srcdir := 'top_srcdir_dev'['top_srcdir_dir']"
$ write config_status_file "$      else"
$ write config_status_file "$       srcdir_dir := 'ac_given_srcdir_dir'"+"'ac_dir_suffix'"
$ write config_status_file "$       srcdir_dev := 'ac_given_srcdir_dev'"
$ write config_status_file "$       srcdir := 'srcdir_dev'['srcdir_dir']"
$ write config_status_file "$       top_srcdir_dev := 'ac_given_srcdir_dev'"
$ write config_status_file "$       top_srcdir_dir := 'ac_given_srcdir_dir'"
$ write config_status_file "$       top_srcdir := 'top_srcdir_dev'['top_srcdir_dir']"
$ write config_status_file "$      endif"
$ write config_status_file "$    endif"
$ write config_status_file "$"
$ write config_status_file "$   write sys$output ""creating '"+"'ac_file'"""
$ write config_status_file "$   set noon"
$ write config_status_file "$   ac_tmp = f$parse(ac_file,""*.*;*"")"
$ write config_status_file "$!   if f$search(ac_tmp) .nes. """" then delete/nolog 'ac_tmp'"
$ write config_status_file "$   ac_input_file_type = f$parse(ac_file,,,""TYPE"")"
$ write config_status_file "$   ac_input_file := 'f$element(0,"";"",ac_file)'"
$ write config_status_file "$   ac_input_file_len = f$length(ac_input_file)-f$length(ac_input_file_type)"
$ write config_status_file "$   !sh sym ac_input_file*"
$ write config_status_file "$   if f$extract(ac_input_file_len,f$length(ac_input_file_type),ac_input_file) .nes. ac_input_file_type then ac_input_file_len = f$length(ac_input_file)"
$ write config_status_file "$   ac_input_file = f$extract(0,ac_input_file_len,ac_input_file)"
$ write config_status_file "$   ac_input_file_dir = f$extract(0,ac_input_file_len-f$length(f$parse(ac_input_file,,,""NAME"")),ac_input_file)"
$ write config_status_file "$   ac_input_file = ac_input_file-ac_input_file_dir"
$ write config_status_file "$   !sh sym ac_input_file*"
$ write config_status_file "$   !set verify"
$ write config_status_file "$   comment_str=""Generated automatically from ""+ac_input_file+ac_input_file_type+"" by configure."""
$ write config_status_file "$   open/write config_status_dest 'ac_file'"
$ write config_status_file "$   if ac_input_file_type .eqs. "".C"" -"
$ write config_status_file "      .or. ac_input_file_type .eqs. "".H"" -"
$ write config_status_file "      .or. ac_input_file_type .eqs. "".CC"" -"
$ write config_status_file "      .or. ac_input_file_type .eqs. "".MAR"""
$ write config_status_file "$    then"
$ write config_status_file "$     write config_status_dest ""/* "",comment_str,"" */"""
$ write config_status_file "$    else"
$ write config_status_file "$     if ac_input_file_type .eqs. "".COM"""
$ write config_status_file "$      then"
$ write config_status_file "$       write config_status_dest ""$! "",comment_str"
$ write config_status_file "$      else
$ write config_status_file "$       write config_status_dest ""# "",comment_str"
$ write config_status_file "$      endif"
$ write config_status_file "$    endif"
$ write config_status_file "$
$ write config_status_file "$   open/write config_status_tpu conftest.tpu"
$ write config_status_file "$   type sys$input:/out=config_status_tpu"
$ write config_status_file "procedure TPU_substitute(pat,value)"
$ write config_status_file "	local r;"
$ write config_status_file "	position (beginning_of (main_buffer));"
$ write config_status_file "	loop"
$ write config_status_file "		r := search_quietly (pat, FORWARD);"
$ write config_status_file "		EXITIF r = 0;"
$ write config_status_file "		erase (r);"
$ write config_status_file "		position (r);"
$ write config_status_file "		copy_text (value);"
$ write config_status_file "		position (end_of (r));"
$ write config_status_file "	endloop;"
$ write config_status_file "endprocedure;"
$ write config_status_file "! This is the main thing."
$ write config_status_file "input_file := GET_INFO (COMMAND_LINE, ""file_name"");"
$ write config_status_file "main_buffer := CREATE_BUFFER (""main"", input_file);"
$ write config_status_file "p_0_or_more_spc := (span("" ""+ASCII(9)) | """");"
$ write config_status_file "position (beginning_of (main_buffer));"
$ write config_status_file "! Here it is time to put the calls."
$ write config_status_file "$   !set noverify"
$ write config_status_file "$   ac_i = 0"
$ write config_status_file "$ac_prsub_write_loop:"
$ write config_status_file "$   ac_i = ac_i + 1"
$ write config_status_file "$   if f$type(ac_prsub'ac_i') .nes. ""
$ write config_status_file "$    then
$ write config_status_file "$     ac_tmp = ac_prsub'ac_i'"
$ write config_status_file "$     write config_status_tpu ac_tmp"
$ write config_status_file "$     goto ac_prsub_write_loop"
$ write config_status_file "$    endif"
$ write config_status_file "$   ac_i = 0"
$ write config_status_file "$ac_vpsub_write_loop:"
$ write config_status_file "$   ac_i = ac_i + 1"
$ write config_status_file "$   if f$type(ac_vpsub'ac_i') .nes. ""
$ write config_status_file "$    then
$ write config_status_file "$     ac_tmp = ac_vpsub'ac_i'"
$ write config_status_file "$     write config_status_tpu ac_tmp"
$ write config_status_file "$     goto ac_vpsub_write_loop"
$ write config_status_file "$    endif"
$ write config_status_file "$   ac_i = 0"
$ write config_status_file "$ac_extrasub_write_loop:"
$ write config_status_file "$   ac_i = ac_i + 1"
$ write config_status_file "$   if f$type(ac_extrasub'ac_i') .nes. ""
$ write config_status_file "$    then
$ write config_status_file "$     ac_tmp = ac_extrasub'ac_i'"
$ write config_status_file "$     write config_status_tpu ac_tmp"
$ write config_status_file "$     goto ac_extrasub_write_loop"
$ write config_status_file "$    endif"
$ write config_status_file "$ __config_status_tmp := 'startupdir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""startupdir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'dcltable'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""dcltable""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'LDFLAGS'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""LDFLAGS""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'CC'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""CC""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'sys_includes'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""sys_includes""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'OPTS'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""OPTS""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'LIBS'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""LIBS""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'CFLAGS'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""CFLAGS""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'CPP'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""CPP""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'INSTALL'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""INSTALL""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'INSTALL_PROGRAM'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""INSTALL_PROGRAM""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'INSTALL_DATA'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""INSTALL_DATA""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'INSTALL_QUOTED'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""INSTALL_QUOTED""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'INSTALL_PROGRAM_QUOTED'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""INSTALL_PROGRAM_QUOTED""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'INSTALL_DATA_QUOTED'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""INSTALL_DATA_QUOTED""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'ALLOCA'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""ALLOCA""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'version'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""version""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'version_us'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""version_us""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'srcdir_dev'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""srcdir_dev""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'srcdir_dir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""srcdir_dir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'srcdir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""srcdir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'top_srcdir_dev'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""top_srcdir_dev""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'top_srcdir_dir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""top_srcdir_dir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'top_srcdir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""top_srcdir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'prefix_dev'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""prefix_dev""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'prefix_dir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""prefix_dir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'prefix'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""prefix""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'exec_prefix_dev'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""exec_prefix_dev""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'exec_prefix_dir'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""exec_prefix_dir""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ write config_status_file "$ __config_status_tmp := 'exec_prefix'"
$ write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@""""+""""exec_prefix""""+""""@"""","""""",__config_status_tmp,"""""");""" ! Experimental
$ if f$type(DEFS_COUNTER) .eqs. "" then DEFS_COUNTER = 0
$ DEFSC = 0
$DEFS_SUBST_VMS_LABLE0:
$ DEFSC = DEFSC + 1
$ if DEFSC .le. DEFS_COUNTER
$  then
$   DEFS = DEFS'DEFSC'
$
$   DEFS_ACC = f$element(0,"""",DEFS)
$   QUOTED_DEFS_ACC = DEFS_ACC
$   DEFSI = 1
$DEFS_SUBST_VMS_LABLE1:
$   DEFS_EL = f$element(DEFSI,"""",DEFS)
$   if DEFS_EL .nes. """"
$    then
$     DEFSI = DEFSI + 1
$     DEFS_ACC = DEFS_ACC + """""""""""""""""" + DEFS_EL
$     QUOTED_DEFS_ACC = QUOTED_DEFS_ACC + """""""""""""""""""""""""""""""""" + DEFS_EL
$     goto DEFS_SUBST_VMS_LABLE1
$    endif
$   if DEFSC .lt. DEFS_COUNTER
$    then
$     write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@DEFS@"""",""""''DEFS_ACC',@DEFS@"""");"""
$     write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@QUOTED_DEFS@"""",""""""""""""''QUOTED_DEFS_ACC'"""""""",@QUOTED_DEFS@"""");"""
$    else
$     write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@DEFS@"""",""""''DEFS_ACC'"""");"""
$     write config_status_file "$ write config_status_tpu ""TPU_substitute(""""@QUOTED_DEFS@"""",""""""""""""''QUOTED_DEFS_ACC'"""""""""""");"""
$    endif
$   goto DEFS_SUBST_VMS_LABLE0
$  endif
$ write config_status_file "$   !set verify"
$ write config_status_file "$   type sys$input:/out=config_status_tpu"
$ write config_status_file "! Now let's save it all"
$ write config_status_file "WRITE_FILE(main_buffer, GET_INFO(COMMAND_LINE, ""output_file""));"
$ write config_status_file "quit;"
$ write config_status_file "$   close config_status_tpu"
$ write config_status_file "$   if ac_input_file_type .eqs. ""."""
$ write config_status_file "$    then ac_input_file = ac_input_file + "".IN"""
$ write config_status_file "$    else ac_input_file = ac_input_file + ac_input_file_type + ""_IN"""
$ write config_status_file "$    endif"
$ write config_status_file "$   save_def = f$environment(""DEFAULT"")"
$ write config_status_file "$   if ac_given_srcdir .nes. """" then set default 'ac_given_srcdir'"
$ write config_status_file "$   if ac_input_file_dir .nes. """" then set default 'ac_input_file_dir'"
$ write config_status_file "$   ac_input_file_dir = f$environment(""DEFAULT"")"
$ write config_status_file "$   set default 'save_def'
$ write config_status_file "$ __debug = f$trnlnm(""DEBUG_CONFIGURE"")"
$ write config_status_file "$ if .not. __debug then __save_mesg_VMS_LABLE0 = f$environment(""MESSAGE"")"
$ write config_status_file "$ if .not. __debug then set message/nofacility/noident/noseverity/notext"
$ write config_status_file "$ if .not. __debug then def/user sys$output nl:"
$ write config_status_file "$ if .not. __debug then def/user sys$error nl:"
$ write config_status_file "$   edit/tpu/nosection/command=conftest.tpu/nodisplay -"
$ write config_status_file "	'ac_input_file_dir'"+"'ac_input_file' /out=config_status_dest"
$ write config_status_file "$ if .not. __debug then set message '__save_mesg_VMS_LABLE0'"
$ write config_status_file "$"
$ write config_status_file "$   close config_status_dest"
$ write config_status_file "$   set noon"
$ write config_status_file "$   if f$search(""conftest.tpu"") then delete/nolog conftest.tpu.*"
$ write config_status_file "$! Description files are often check for dependencies, so don't check
$ write config_status_file "$! if they have changed.  Scripts on the ofter hand...
$ write config_status_file "$!$   if f$parse(file,,,""TYPE"") .eqs. "".COM"" -"
$ write config_status_file "$!	.and. f$search(file) .nes. """" then -"
$ write config_status_file "$!	diff conftest.out 'file'/out=nla0:"
$ write config_status_file "$!$   if $status .eq. %X06c8009 ! code for no change"
$ write config_status_file "$!$    then"
$ write config_status_file "$!$     ! The file exists and we would not be changing it."
$ write config_status_file "$!$     write sys$output ""'"+"'file' is unchanged."""
$ write config_status_file "$!$    else"
$ write config_status_file "$!$     copy conftest.out 'file'"
$ write config_status_file "$!$     purge/nolog 'file'"
$ write config_status_file "$!$    endif"
$ write config_status_file "$!$   if f$search(""conftest.out.*"") .nes. """" then delete/nolog conftest.out.*"
$ write config_status_file "$   set on"
$ write config_status_file "$   goto config_status_loop3"
$ write config_status_file "$  endif"
$
$ write config_status_file "$"
$ write config_status_file "$ srcdir=ac_given_srcdir"
$ write config_status_file "$ srcdir_dev=ac_given_srcdir_dev"
$ write config_status_file "$ srcdir_dir=ac_given_srcdir_dir"
$ write config_status_file "$"

$ write config_status_file "$ exit"
$ write config_status_file "$config_status_end:"
$ write config_status_file "$ goto config_status_begin"
$ close config_status_file
$ set file/prot=(s:rwed,o:rwed,g:re,w:re) config.status
$ if .not. no_create then @config.status
$
