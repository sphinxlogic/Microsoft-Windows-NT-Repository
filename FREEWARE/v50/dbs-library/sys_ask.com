$ ! Procedure:	SYS_ASK.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto _control_y_exit
$ say		= "write sys$output"
$ symbol	= f$edit(P1, "collapse,upcase")
$ prompt	= "''P2'"
$ default	= "''P3'"
$ options	= f$edit(P4, "collapse,upcase")
$ help_option	= "''P5'"
$ if (symbol .eqs. "") then symbol = "USER_RESPONSE"
$ if (prompt .eqs. "") then prompt = symbol
$ sys__control_y == 0
$ sys__control_z == 0
$ sys__backwards == 0
$ 'symbol' == ""
$ blank_line = " "
$ echo_off = "False"
$ ask_case = "UPPER"
$ if (f$locate("L",options) .ne. f$length(options)) then ask_case = "LOWER"
$ if (f$locate("U",options) .ne. f$length(options)) then ask_case = "UPPER"
$ if (f$locate("M",options) .ne. f$length(options)) then ask_case = "MAINTAIN"
$ if (f$locate("R",options) .ne. f$length(options)) then -
$	blank_line[0,16] = %x0707
$ if ((f$locate("D",options) .ne. f$length(options)) .or. -
      (f$locate("R",options) .ne. f$length(options))) then say blank_line
$ data_type = "S"
$ if (f$locate("B",options) .ne. f$length(options)) then data_type = "B"
$ if (f$locate("I",options) .ne. f$length(options)) then data_type = "I"
$ if (f$locate("E",options) .ne. f$length(options)) then echo_off = "True"
$ if (default .nes. "") then prompt = prompt + " [" + default + "]"
$ prompt = "* " + prompt + f$extract(f$locate(data_type,"BIS"),1,"?::") + " "
$ if (f$locate("H",options) .ne. f$length(options)) then goto provide_help
$start_ask:
$ if (echo_off)
$   then
$   set terminal/noecho
$   say ""
$ endif
$ read/end_of_file=check_control_z/prompt="''prompt'" sys$command answer
$ if (f$type(answer) .eqs. "") then goto start_ask
$ if (echo_off) then set terminal/echo
$ goto ask_'ask_case'
$ask_upper:
$ answer  = f$edit(answer,"COMPRESS,TRIM,UNCOMMENT,UPCASE")
$ default = f$edit(default,"COMPRESS,TRIM,UNCOMMENT,UPCASE")
$ goto check_answer
$ask_lower:
$ answer  = f$edit(answer,"COMPRESS,TRIM,UNCOMMENT,LOWERCASE")
$ default = f$edit(default,"COMPRESS,TRIM,UNCOMMENT,LOWERCASE")
$ goto check_answer
$ask_maintain:
$ answer  = f$edit(answer,"COMPRESS,TRIM,UNCOMMENT")
$ default = f$edit(default,"COMPRESS,TRIM,UNCOMMENT")
$check_answer:
$ if (answer .eqs. "?") then goto provide_help
$ if (answer .eqs. "") then answer = default
$ if ((answer .eqs. "") -
		.and. (f$locate("N",options) .eq. f$length(options))) then -
$	goto start_ask
$ if ((answer .eqs. "^") -
		.and. (f$locate("^",options) .eq. f$length(options))) then -
$	goto start_ask
$ if (answer .eqs. "^") then goto _backwards_exit
$ goto ask_'data_type'
$ask_B:
$ answer = f$edit(answer,"COMPRESS,TRIM,UNCOMMENT,UPCASE")
$ 'symbol' == (f$extract(0,1,answer) .eqs. "Y")
$ if ((f$locate(answer,"YES") .eq. 0) -
		.or. (f$locate(answer,"NO") .eq. 0)) then -
$	goto _success_exit
$ say "%SYS_ASK-I-YESNO, please enter YES or NO"
$ goto start_ask
$ask_I:
$ 'symbol' == f$integer(answer)
$ if (f$type(answer) .eqs. "INTEGER") then goto _success_exit
$ say "%SYS_ASK-I-INTEGER, please enter an integer number"
$ goto start_ask
$ask_S:
$ 'symbol' == answer
$ goto _success_exit
$check_control_z:
$ if (f$mode() .nes. "INTERACTIVE") then goto _control_y_exit
$ if (f$locate("Z",options) .eq. f$length(options)) then goto start_ask
$ 'symbol' == "^Z"
$ sys__control_z == 1
$ goto _success_exit
$provide_help:
$ if (help_option .eqs. "") then -
$	help_option = "%SYS_ASK-I-NOHELP, sorry, no help is available"
$ say ""
$ if (f$extract(0,1,help_option) .nes. "@") then say help_option
$ if (f$extract(0,1,help_option) .eqs. "@") then 'help_option'
$ goto start_ask
$
$_backwards_exit:
$ sys__backwards == 1
$_success_exit:
$ exitt %X10000001				!'f$verify(__vfy_saved)'
$_control_y_exit:
$ sys__control_y == 1
$ exitt %X10000004				!'f$verify(__vfy_saved)'
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_ASK.COM
$ !
$ ! Purpose:	To provide a common and consistent method of accepting user
$ !		input from procedures.
$ !
$ ! Parameters:
$ !	[P1]	The global symbol that will contain the user response.  If
$ !		not supplied, it will default to USER_RESPONSE.  All responses
$ !		are compressed, trimmed and have comments removed; any other
$ !		options are controlled by P4 (see below).
$ !	[P2]	A quoted string containing the question to be asked.  If you
$ !		omit this parameter, the symbol from P1 will be used.
$ !		Do not include trailing spaces, a backslash (\), a colon (:),
$ !		or a question mark (?) because these are added automatically.
$ !	[P3]	The default response, if any.  If you do not specify a default
$ !		response, this procedure will continue to prompt the user
$ !		until a valid response is entered.
$ !	[P4]	This specifies options which control various aspects of the
$ !		prompting and validation of responses.  Specify multiple options
$ !		without using spaces or commas e.g. BR will ring a bell before
$ !		displaying the prompt and will require a boolean data-type
$ !		response.
$ !		A CTRL/Y in response to a prompt will return a null string
$ !		and set the global symbol SYS__CONTROL_Y to TRUE; it is the
$ !		responsibility of the caller to handle this condition.
$ !		Valid options are:
$ !		  B -	The answer must be in Boolean form, YES or NO (or Y
$ !			or N); if it is not, the question repeats until it is
$ !		  D -	Generates a blank line before the prompt (double
$ !			spacing)
$ !		  E -	Disable echo during input.
$ !		  H -	Displays appropriate help text before displaying the
$ !			prompt (the help is based on P5)
$ !		  I -	The answer must be an integer; if it is not, the
$ !			question repeats until an integer is entered
$ !		  L -	Convert the response to LOWERCASE
$ !		  M -	Maintain the case of the response
$ !		  N -	A null string response is acceptable; applies to
$ !			situations where a string response is requested but
$ !			no default value is specified
$ !		  R -	Rings the bell before displaying the prompt
$ !		  S -	The response can be any character string; this is the
$ !			default response data-type
$ !		  U -	Convert the response to UPPERCASE
$ !		  Z -	Allow CTRL/Z in reponse to a prompt.  The return value
$ !			is the two-character string "^Z" and the global symbol
$ !			SYS__CONTROL_Z is set to TRUE.
$ !		  ^ -	Allow ^ (Shift/6) in reponse to a prompt.  The return
$ !			value is null and the global symbol SYS__BACKWARDS is
$ !			set to TRUE.
$ !	[P5]	Specifies help text to be used in conjunction with the prompt.
$ !		You can either enter actual help text, or you can enter a call
$ !		to a command procedure to provide the text by way of a help
$ !		callback.  A call to a procedure would generally be of the
$ !		following format -
$ !			"@LOCATION:HELP_PROCEDURE WHATEVER_TOPIC"
$ !
$ ! History:
$ !		12-Apr-1991, DBS; Version V1-001
$ !	001 -	Original version.  (Based on, taken from VMSINSTAL.)
$ !		13-Apr-1991, DBS; Version V1-002
$ !	002 -	Modified so that a ^Z in any non-interactive process will be
$ !		the same as a ^Y.
$ !		28-May-1992, DBS; Version V1-003
$ !	003 -	Fixed a few typos (in the documentation).
$ !		01-Dec-1994, DBS; Version V1-004
$ !	004 -	Added E option to disable echo during input.
$ !-==========================================================================
