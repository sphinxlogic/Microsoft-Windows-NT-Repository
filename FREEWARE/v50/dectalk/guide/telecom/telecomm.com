$!	save_verify = f$verify(0) + f$verify(0'basic_dtdemo_verify' .gt. 0)
$!
$! @telecomm.com P1
$! This command file starts the Telecommunications DECtalk demo on one 
$! or more terminal lines.  It reads the terminal lines from a configuration
$! file (P1) and creates an independent copy of the demo for each 
$! terminal in the file.  The configuration file contains one line per
$! terminal in the following (sample) format.
$!
$!      txa4: dictionary.user	! DECtalk connected to txa4:
$!      txa7:           	! DECtalk connected to txa7:
$!
$! where "txa4:" and "txa7:" are the terminal lines that DECtalk is 
$! connected to and "dictionary.user" is the name of the file containing 
$! the words to be loaded into DECtalk's user loadable dictionary.
$! Note, in the second example, no dictionary filename is specified.  
$! The terminal line must be specified, but the dictionary filename
$! is an optional parameter.  The text following the exclamation point 
$! is ignored.  If the P1 parameter is specified, it will override the 
$! name of the configuration file.
$!
$	on control_y then goto abnormal_exit
$	on warning then goto abnormal_exit
$!
$!
$! Now read in the available terminal lines and setup the command files.
$	if "''P1'" .eqs. "" then goto no_file
$	open/read/error=no_configuration cfg 'P1'
$ read_loop:
$	read/end=read_done cfg cmd
$	i = f$locate("!", cmd)			! Look for comment
$	cmd = f$extract(0, i, cmd)		! Remove the comment
$	cmd = f$edit(cmd, "COMPRESS, LOWERCASE") ! Remove multiple blanks/tabs
$	i = f$locate(" ", cmd)			 ! Separate tty and dictionary
$	tty = f$extract(0, i, cmd)		 ! Get name of tty and dict
$	dict = f$extract((i+1), (f$length(cmd)-i), cmd)
$	dict = f$edit(dict, "COLLAPSE")          ! Remove blanks/tabs
$       i = f$locate(":", tty)			 ! Remove any ':' that 
$	tty = f$extract(0, i, tty)		 ! might be hanging around
$	tty = f$edit(tty, "COLLAPSE, UPCASE")	 ! Remove blanks/tabs
$	if f$length(tty) .eq. 0 then goto read_loop
$! 
$! Create a command procedure that will start up the DECtalk demo
$! on the terminal line specified with the user dictionary file specified
$!
$	open/write/error=abnormal_exit com DEMO_'tty'.com
$       dir = f$environment("DEFAULT")		 ! Get default device/directory
$       write com "$ telecomm :== $''dir'telecomm.exe"
$	write com "$ telecomm ''tty': ''dict'"
$	close com
$	run/detach			-
	    /input=DEMO_'tty'.com	-
	    /output='tty'.err		-
	    /error='tty'.err		-
	    /process="DEMO_''tty'"	-
		sys$system:loginout
$	goto read_loop
$!
$ read_done:
$	close cfg
$	goto normal_exit
$!
$! Error exits
$!
$ no_file:
$	status = $status			!save status
$	write sys$error "No configuration file specified"
$	goto exit
$!
$ no_configuration:
$	status = $status
$	write sys$error "Can't open ''P1', ''f$message(STATUS)'"
$	goto exit
$!
$ abnormal_exit:
$	status = $status			!save failure status
$	if status then status = "%X08000002"	!force error if neccessary
$	goto exit
$!
$! Normal exit.
$!
$ normal_exit:
$	status = $status			!save success status
$!
$! Exit.
$!
$exit:
$!	exit ('status' .or. %x10000000) + f$verify(save_verify) * 0
