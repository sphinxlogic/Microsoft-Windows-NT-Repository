$ ! Procedure:	BUILD_IMAGE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ on error then goto bail_out
$ say = "write sys$output"
$ package = f$search("*.PACKAGE_NAME",9764)
$ if (package .nes. "")
$   then
$   package = f$parse(package,,,"name")
$   if (f$edit(P1,"collapse") .nes. "")
$     then
$     P2 = "''P1'"
$   endif !(f$edit(P1,"collapse") .nes. "")
$   P1 = package
$ endif !(f$search("*.PACKAGE_NAME",9764) .nes. "")
$ filespec = f$edit(P1, "collapse,upcase")
$ if (filespec .eqs. "") then -
$   filespec = f$trnlnm("image_to_build","lnm$process_table")
$ if (filespec .eqs. "") then goto bail_out
$ say "%Building ''filespec'"
$ definee/process/nolog image_to_build 'filespec'
$ other_build = f$search("BUILD_''filespec'.COM",61524)
$ if (other_build .nes. "")
$   then
$   other_build = f$element(0,";",other_build)
$   say "%Using ''other_build' for the build"
$   @'other_build'
$ else
$ link_only = "False"
$ debug_qual = ""
$ optimize = ""
$ link_it = "link/notrace"
$ save_objects = "False"
$ map_option = ""
$ option_list = f$edit(P2, "collapse,upcase")
$ count = 0
$options_loop:
$   option = f$element(count, ",", option_list)
$   count = count + 1
$   if (option .eqs. ",") then goto end_options_loop
$   if (option .eqs. "") then goto options_loop
$   option_len = f$length(option)
$   if (option .eqs. f$extract(0, option_len, "DEBUG"))
$     then
$     debug_qual = "/DEBUG"
$     optimize = "/NOOPTIMIZE"
$     link_it = "link/debug"
$   else
$   if (option .eqs. f$extract(0, option_len, "LINK_ONLY"))
$     then
$     link_only = "True"
$   else
$   if (option .eqs. f$extract(0, option_len, "SAVE_OBJECTS"))
$     then
$     save_objects = "True"
$   else
$   if (option .eqs. f$extract(0, option_len, "MAP"))
$     then
$     map_option = "/MAP"
$   endif !(option .eqs. f$extract(0, option_len, "MAP"))
$   endif !(option .eqs. f$extract(0, option_len, "SAVE_OBJECTS"))
$   endif !(option .eqs. f$extract(0, option_len, "LINK_ONLY"))
$   endif !(option .eqs. f$extract(0, option_len, "DEBUG"))
$ goto options_loop
$end_options_loop:
$ do_compile = (.not. link_only)
$ filename = f$parse(filespec,,,"name")
$ if (do_compile)
$   then
$   if (f$search("''filespec'.MAR") .nes. "")
$     then
$     say "$ macro ''filespec'''debug_qual'"
$     macro 'filespec''debug_qual'
$   else
$   if (f$search("''filespec'.FOR") .nes. "")
$     then
$     say "$ fortran ''filespec'''debug_qual'''optimize'"
$     fortran 'filespec''debug_qual''optimize'/extend_source
$   else
$   if (f$search("''filespec'.BAS") .nes. "")
$     then
$     say "$ basic ''filespec'''debug_qual'"
$     basic 'filespec''debug_qual'
$   else
$   say "%BUILD-I-NOFILE, no .MAR, .FOR or .BAS file found"
$   goto bail_out
$   endif !(f$search("''filespec'.BAS") .nes. "")
$   endif !(f$search("''filespec'.FOR") .nes. "")
$   endif !(f$search("''filespec'.MAR") .nes. "")
$ endif !(do_compile)
$ cld_file = "''filespec'_CLD"
$ if (f$search("''cld_file'.CLD") .nes. "")
$   then
$   if (do_compile)
$     then
$     say "$ set command/object ''cld_file'"
$     set command/object 'cld_file'
$   endif !(do_compile)
$   cld_file = ",''filename'_CLD"
$ else
$ cld_file = ""
$ endif !(f$search("''cld_file'.CLD") .nes. "")
$ msg_file = "''filespec'_MSG"
$ if (f$search("''msg_file'.MSG") .nes. "")
$   then
$   if (do_compile)
$     then
$     say "$ message ''msg_file'"
$     message 'msg_file'
$   endif !(do_compile)
$   msg_file = ",''filename'_MSG"
$ else
$ msg_file = ""
$ endif !(f$search("''msg_file'.MSG") .nes. "")
$ if (f$search("''filename'.OPT") .nes. "")
$   then
$   say "$! linker options file being used . . ."
$   opt_file = ",''filename'/OPTIONS"
$ else
$ opt_file = ""
$ endif !(f$search("''filename'.OPT") .nes. "")
$ say "$ ''link_it'''map_option' ''filename'''msg_file'''cld_file'''opt_file'"
$ 'link_it''map_option' 'filename''msg_file''cld_file''opt_file'
$ if (.not. save_objects)
$   then
$   say "$ delete ''filename'*.obj;*"
$   delete 'filename'*.obj;*
$ endif !(.not. save_objects)
$ endif !(other_build .nes. "")
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	BUILD_IMAGE.COM
$ !
$ ! Purpose:	To compile and link "simple" images.  It will look for a .MAR
$ !		then a .FOR source file and use that as the source to the
$ !		appropriate compiler.  CLD, MSG and OPT files are also
$ !		considered and the necessary commands are generated to include
$ !		all these bits in the image.
$ !		The following filenaming conventions are assumed:
$ !			filename.MAR	    (or .FOR as the main source file)
$ !			filename_CLD.CLD
$ !			filename_MSG.MSG
$ !			filename.OPT
$ !		All object files generated are deleted on the way out unless
$ !		LINK_ONLY is used.
$ !
$ ! Parameters:
$ !	 P1	The name of the source file.  No filetype is allowed.
$ !	[P2]	An optional comma separated list of the following...
$ !		DEBUG to generate a debuggable image.
$ !		LINK_ONLY to skip the compiles and do a link only.
$ !		SAVE_OBJECTS to prevent the deletion of object files.
$ !
$ ! History:
$ !		19-Sep-1991, DBS; Version V1-001
$ !	001 -	Original version.
$ !		15-Jun-1994, DBS; Version V1-002
$ !	002 -	Added the link_only and save_objects options.
$ !		15-Jul-1994, DBS; Version V1-003
$ !	003 -	Introduced use of <package>.PACKAGE_NAME to be used in place
$ !		of P1 being the package name and look for BUILD_<package>.COM
$ !		to be used instead of the code in here (if it exists).  Also
$ !		added MAP as a valid option.
$ !-==========================================================================
