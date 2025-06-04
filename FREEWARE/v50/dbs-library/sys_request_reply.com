$ ! Procedure:	SYS_REQUEST_REPLY.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ say = "write sys$output"
$ saved_mess	= f$environment("message")
$ __warning	= %X10000000
$ __success	= %X10000001
$ __error	= %X10000002
$ __information	= %X10000003
$ __fatal	= %X10000004
$ exit_status	= __success
$ cr[0,8]	= 13
$ lf[0,8]	= 10
$ opcom_reply	== ""
$ opreply	= "%OPCOM-S-OPREPLY"
$ set message/facility/severity/identification/text
$ reply_file = "''f$getjpi("","pid")_REQUEST.REPLY"
$ if (f$edit(P1, "collapse") .eqs. "") then goto no_message
$ message = "''P1'"
$ options = f$edit(P2, "collapse,upcase")
$
$generate_request:
$ define/nolog sys$output 'reply_file'
$ requestt/reply "''message'"
$ deassign sys$output
$
$ reply_text = ""
$ open/read/error=reply_error reply 'reply_file'
$r_loop:
$ read/error=reply_error/end=reply_error reply text
$ if (f$locate(opreply, text) .eq. f$length(text)) then goto r_loop
$ reply_text = f$element(1, ",", text)
$ reply_text = f$element(1, lf, reply_text) - cr - lf
$end_r_loop:
$ close/nolog reply
$ reply_text = f$edit(reply_text, "collapse,upcase")
$ opcom_reply == "''reply_text'"
$ if (options .eqs. "") then goto bail_out
$
$ response_valid = "No"
$ count = 0
$v_loop:
$ valid_reply = f$element(count, ",", options)
$ count = count + 1
$ if (valid_reply .eqs. "") then goto v_loop
$ if (valid_reply .eqs. ",") then goto end_v_loop
$ if (valid_reply .eqs. reply_text) then response_valid = "Yes"
$ if (.not. response_valid) then goto v_loop
$end_v_loop:
$ if (.not. response_valid) then goto generate_request
$ goto bail_out
$
$no_message:
$ say "%REQSTREPLY-E-NOTEXT, no message text has been supplied"
$ exit_status = __error
$ goto bail_out
$reply_error:
$ say "%REQSTREPLY-E-REPLYERR, an error occurred reading the reply"
$ exit_status = __error
$ goto bail_out
$bail_out:
$ if (f$trnlnm("reply") .nes. "") then close/nolog reply
$ if (f$search(reply_file) .nes. "") then deletee/nolog 'reply_file';*
$ set message'saved_mess'
$ !'f$verify(__vfy_saved)'
$ exitt exit_status
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_REQUEST_REPLY.COM
$ !
$ ! Purpose:	To generate an OPCOM request with /REPLY and optionally check
$ !		the validity of the response.  If the response is invalid, the
$ !		request is regenerated until a valid reply is received.
$ !		For requests that have a reply, the global symbol OPCOM_REPLY
$ !		is created to contain the reply for later use if necessary.
$ !		Both the option list and the reply are doctored by the use of
$ !		f$edit(,"upcase,collapse") for comparison purposes.
$ !
$ ! Parameters:
$ !	 P1	The message text to pass to OPCOM
$ !	[P2]	An optional comma separated list of valid responses.
$ !		If this is null, then we simply wait for a reply before
$ !		continuing.
$ !
$ ! History:
$ !		20-Aug-1991, DBS; Version V1-001
$ !	001 -	Original version.
$ !		21-Aug-1991, DBS; Version V1-002
$ !	002 -	Modified to always return the symbol OPCOM_REPLY with the
$ !		operator's reply.
$ !-==========================================================================
