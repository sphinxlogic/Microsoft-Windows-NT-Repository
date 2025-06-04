$!------------------------------------------------------------------------------
$!
$!                       © Yezerski Roper Ltd. 1997
$!
$!------------------------------------------------------------------------------
$!
$!DOC	Command file name	: FREEWARE_DEMO.COM
$!
$!DOC	Author(s)       	: John A Fotheringham
$!DOC 
$!DES	Command file description
$!DES	------------------------
$!DES	Command file to run the freeware demo of AscToHTM
$!DES
$!DES	Parameters
$!DES	----------
$!DES	p1	- None
$! 
$!DOC	HISTORY
$!DOC	-------
$!DOC
$!DOC	Version		Name/description			Date
$!DOC	-------		----------------			----
$!DOC	V01-001		John A Fotheringham			18-Aug-1998
$!
$!------------------------------------------------------------------------------
$!
$	wsay = "write sys$output"
$!
$!----- work out the architecture
$!
$ 	if F$GETSYI("NODE_HWTYPE").EQS."ALPH"
$ 	then
$    	  platform = "ALPHA"
$ 	else
$    	  platform = "VAX"
$ 	endif
$!
$!----- work out the directory
$!
$	command_file = f$environment ("PROCEDURE")
$	command_dir  = f$element (0,"]",command_file) + ".EXE]"
$	program      = Command_dir + "ASCTOHTM.EXE_''platform'_NOSHARE"
$!
$	! define a foreign command
$	a2h :== $'program'
$!
$ convert_loop:
$!
$	wsay ""
$	wsay "Enter filespec of file(s) to be converted to HTML"
$	inquire/nopunc filespec "[CR=exit] > "
$	if filespec.eqs."" then goto end_convert_loop
$!
$	file = f$search (filespec)
$!
$	if file.eqs."" 
$	then
$	  wsay ""
$	  wsay "Sorry.  I can't find ''filespec'"
$	  wsay ""
$	  goto convert_loop
$	endif
$!
$	! if multiple files, create an index for them
$	index_flag = ""
$	if f$locate ("*",filespec).ne.f$length(filespec) then index_flag ="/index"
$	if f$locate ("%",filespec).ne.f$length(filespec) then index_flag ="/index"
$!
$	wsay ""
$	! Okay, let's blow their minds :-)))
$	a2h 'filespec''index_flag'
$!
$	goto convert_loop
$	wsay "command_file = ",program
$!
$ end_convert_loop:
$!
$	wsay ""
$	wsay ""
$	wsay ""
$	wsay ""
$	wsay ""
$	wsay "Thank you for flying YEZERSKI ROPER's AscToHTM !!"
$	wsay ""
$	inquire/nopunc goon "Press RETURN to Exit > "
$!	
$exit
