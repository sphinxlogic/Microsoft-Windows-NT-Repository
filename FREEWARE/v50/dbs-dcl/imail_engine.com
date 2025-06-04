$ ! Procedure:	IMAIL_ENGINE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$
$ !===========================================================================
$
$err: subroutine
$ set noon
$ severity = f$edit(P1,"COLLAPSE,UPCASE")
$ identification = f$edit(P2,"COLLAPSE,UPCASE")
$ text = f$edit(P3,"TRIM")
$ continuation = (f$edit(P4,"COLLAPSE,UPCASE") .nes. "")
$ percent = "%"
$ if (continuation) then percent = "-"
$ if (severity .eqs. "")
$ then say "%''facility'-F-NOSEVER, severity missing in call to ERR"
$ else if (identification .eqs. "")
$ then say "%''facility'-F-NOIDENT, identification missing in call to ERR"
$ else if (text .eqs. "")
$ then say "%''facility'-F-NOTEXT, text missing in call to ERR"
$ else say "''percent'''facility'-''severity'-''identification', ''text'"
$ endif !(text .eqs. "")
$ endif !(identification .eqs. "")
$ endif !(severity .eqs. "")
$exit_err:
$ exitt 1
$ endsubroutine
$
$ !===========================================================================
$
$imail_deliver: subroutine
$ set noon
$ if (f$edit(P1,"COLLAPSE") .eqs. "")
$   then
$   imail__status == ss$_badfilename
$   err E WHAT "deliver what?"
$ else
$ counter = 1
$loop0:
$   parameter = f$edit(P'counter',"COLLAPSE")
$   item = 0
$loop1:
$     filename = f$element(item,",",parameter)
$     item = item + 1
$     if (filename .eqs. "") then goto loop1
$     if (filename .eqs. ",") then goto end_loop1
$     if (imail_verbose) then err I DELIVER "delivering file <''filename'>"
$     call imail_deliver_file 'filename'
$    goto loop1
$end_loop1:
$   counter = counter + 1
$  if (counter .lt. 8) then goto loop0
$end_loop0:
$ endif !(f$edit(P1,"COLLAPSE") .eqs. "")
$exit_imail_deliver:
$ endsubroutine
$
$imail_deliver_file: subroutine
$ set noon
$ file_to_deliver = f$edit(P1,"COLLAPSE,UPCASE")
$ if (f$search(file_to_deliver) .eqs. "")
$   then
$   imail__status == rms$_fnf
$   err E LOSTIT "unable to locate file to deliver"
$   err I NAMEIS "the file to deliver was <''file_to_deliver'>" y
$ else
$ call imail_validate 'file_to_deliver'
$ if (imail__status)
$   then
$   imail_sff 'file_to_deliver'
$ endif !(imail__status)
$ endif !(f$search(file_to_deliver) .eqs. "")
$exit_imail_deliver_file:
$ endsubroutine
$
$ !===========================================================================
$
$imail_validate: subroutine
$ set noon
$ if (f$edit(P1,"COLLAPSE") .eqs. "")
$   then
$   imail__status == ss$_badfilename
$   err E WHAT "validate what?"
$ else
$ counter = 1
$loop0:
$   parameter = f$edit(P'counter',"COLLAPSE")
$   item = 0
$loop1:
$     filename = f$element(item,",",parameter)
$     item = item + 1
$     if (filename .eqs. "") then goto loop1
$     if (filename .eqs. ",") then goto end_loop1
$     if (imail_verbose) then err I VALIDATE "validating file <''filename'>"
$     call imail_validate_file 'filename'
$    goto loop1
$end_loop1:
$   counter = counter + 1
$  if (counter .lt. 8) then goto loop0
$end_loop0:
$ endif !(f$edit(P1,"COLLAPSE") .eqs. "")
$exit_imail_validate:
$ endsubroutine
$
$imail_validate_file: subroutine
$ set noon
$ file_to_check = f$edit(P1,"COLLAPSE,UPCASE")
$ if (f$search(file_to_check) .eqs. "")
$   then
$   imail__status == rms$_fnf
$   err E LOSTIT "unable to locate file to validate"
$   err I NAMEIS "the file to validate was <''file_to_check'>" y
$ else
$ imail__status == ss$_format			! assume it's bad
$ openn/read/error=all_done input 'file_to_check'
$ readd/error=all_done input input_text		! first line is MAIL FROM:...
$ input_text = f$edit(input_text,"UPCASE")
$ if ((f$extract(0,imail_from_len,input_text) .nes. imail_from_1) -
	.and. (f$extract(0,imail_from_len,input_text) .nes. imail_from_2)) then -
$   goto all_done
$ readd/error=all_done input input_text		! next line is RCPT TO:...
$ input_text = f$edit(input_text,"UPCASE")
$ if ((f$extract(0,imail_to_len,input_text) .nes. imail_to_1) -
	.and. (f$extract(0,imail_to_len,input_text) .nes. imail_to_2)) then -
$   goto all_done
$loop0:						! RCPT TO: or DATA
$   readd/error=end_loop0 input input_text
$   input_text = f$edit(input_text,"UPCASE")
$   if ((f$extract(0,imail_to_len,input_text) .eqs. imail_to_1) -
	.or. (f$extract(0,imail_to_len,input_text) .eqs. imail_to_2)) then -
$     goto loop0
$   if (input_text .eqs. imail_data) then imail__status == ss$_normal
$end_loop0:
$all_done:
$ closee/nolog input
$ if (.not. imail__status)
$   then
$   err E BADFILE "file <''file_to_check'> is bad"
$ else
$ if (imail_verbose) then -
$   err I GOODFILE "file <''file_to_check'> seems to be OK"
$ endif !(.not. imail__status)
$ endif !(f$search(file_to_check) .eqs. "")
$exit_imail_validate_file:
$ endsubroutine
$
$ !===========================================================================
$
$imail_encode: subroutine
$ set noon
$ if (f$edit(P1,"COLLAPSE") .eqs. "")
$   then
$   imail__status == ss$_badfilename
$   err E WHAT "encode what?"
$ else
$ counter = 1
$loop0:
$   parameter = f$edit(P'counter',"COLLAPSE")
$   item = 0
$loop1:
$     filename = f$element(item,",",parameter)
$     item = item + 1
$     if (filename .eqs. "") then goto loop1
$     if (filename .eqs. ",") then goto end_loop1
$     if (imail_verbose) then err I ENCODING "encoding file <''filename'>"
$     call imail_encode_file 'filename'
$    goto loop1
$end_loop1:
$   counter = counter + 1
$  if (counter .lt. 8) then goto loop0
$end_loop0:
$ endif !(f$edit(P1,"COLLAPSE") .eqs. "")
$exit_imail_encode:
$ endsubroutine
$
$imail_encode_file: subroutine
$ set noon
$ file_to_encode = f$edit(P1,"COLLAPSE,UPCASE")
$ if (f$search(file_to_encode) .eqs. "")
$   then
$   imail__status == rms$_fnf
$   err E LOSTIT "unable to locate file to encode"
$   err I NAMEIS "the file to encode was <''file_to_encode'>" y
$ else
$ encoded_file = f$parse(file_to_encode,,,"NAME") + ".UUE"
$ err I ENCODED "encoded file will be <''encoded_file'>"
$ imail_encode 'file_to_encode' 'encoded_file'
$ endif !(f$search(file_to_encode) .eqs. "")
$exit_imail_encode_file:
$ endsubroutine
$
$ !===========================================================================
$
$imail_send: subroutine
$ set noon
$ unique_tag = f$cvtime(,"COMPARISON") - "-" - "-" - " " - ":" - ":" - "."
$ unique_tag = unique_tag + "_''jpi_pid'"
$ if (imail_verbose) then err I UNIQUE "tag is <''unique_tag'>"
$ file_to_send = f$edit(P1,"COLLAPSE,UPCASE")
$ recipient = f$edit(P2,"COLLAPSE")
$ sender = f$edit(P3,"COLLAPSE")
$ file_qual = f$element(1,"/",file_to_send)
$ file_to_send = f$element(0,"/",file_to_send)
$ if (file_to_send .eqs. "")
$   then
$   err E WHAT "send what?"
$ else
$ if (f$search(file_to_send) .eqs. "")
$   then
$   imail__status == rms$_fnf
$   err E LOSTIT "unable to locate file to send"
$   err I NAMEIS "the file to send was <''file_to_send'>" y
$ else
$ if (sender .eqs. "") then sender = imail_default_sender
$ if (sender .eqs. "")
$   then
$   imail__status == ss$_ivaddr
$   err E NOSENDER "no sender/return address is available"
$ else
$ if (recipient .eqs. "")
$   then
$   imail__status == ss$_ivaddr
$   err E NOADDR "no recipient address was specified"
$ else
$ if (file_qual .eqs. "/") then file_qual = ""
$ if (file_qual .eqs. "") then file_qual = f$trnlnm("IMAIL_MIME_TYPE")
$ if (file_qual .eqs. "TEXT") then file_qual = "PLAIN"
$ if ((file_qual .nes. "PLAIN") .and. (file_qual .nes. "HTML")) then -
$   file_qual = "PLAIN"
$ sff_file = unique_tag + ".SFF"
$ imail_sff_file == "''sff_file'"
$ copyy/nolog nla0: 'sff_file'
$ openn/append sff 'sff_file'
$ writee sff "MAIL FROM:<''sender'>"
$ writee sff "RCPT TO:<''recipient'>"
$ writee sff "DATA"
$ writee sff "From: ''sender'"
$ writee sff "To: ''recipient'"
$ writee sff "Subject: File sent ""''file_to_send'"""
$ writee sff "MIME-Version: 1.0"
$ if (imail_organization .nes. "") then -
$   writee sff "Organization: ''imail_organization'"
$ if (file_qual .eqs. "HTML")
$   then
$   writee sff "Content-Type: text/html; charset=""US-ASCII"""
$   writee sff "Content-Transfer-Encoding: 7BIT"
$ else
$ writee sff "Content-Type: text/plain; charset=""US-ASCII"""
$ writee sff "Content-Transfer-Encoding: 7BIT"
$ endif !(file_qual .eqs. "HTML")
$ writee sff ""				! this ends the headers
$ closee/nolog sff
$ appendd/nolog 'file_to_send' 'sff_file'
$ if (imail_verbose) then err I SENDING "file being sent is <''sff_file'>"
$ imail_sff 'sff_file'
$ if (.not. imail_retain_sff)
$   then
$   if (imail_verbose) then err I CLEANUP "removing file <''sff_file'>"
$   deletee/nolog 'sff_file';
$ endif !(.not. imail_retain_sff)
$ endif !(recipient .eqs. "")
$ endif !(sender .eqs. "")
$ endif !(f$search(file_to_send) .eqs. "")
$ endif !(file_to_send .eqs. "")
$exit_imail_send:
$ endsubroutine
$
$ !===========================================================================
$
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ facility = procedure_name
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
		+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$ facility = "IMAIL"
$ imail_version = "X1-002"
$ imail__status == 1
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ say = "write sys$output"
$ err = "call err"
$ rms$_fnf = 98962
$ ss$_normal = 1
$ ss$_badparam = 20
$ ss$_badfilename = 2072
$ ss$_format = 188
$ ss$_ivaddr = 308
$ ss$_nopriv = 36
$ ss$_sysverdif = 9116
$ ss$_unsupported = 3658
$ syi_version = f$edit(f$getsyi("VERSION"),"COLLAPSE,UPCASE")
$ syi_version_type = f$extract(0,1,syi_version)
$ syi_version_number = syi_version - syi_version_type
$ syi_scsnode = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,LOWERCASE")
$ jpi_pid = f$getjpi("","PID")
$ jpi_username = f$edit(f$getjpi("","USERNAME"),"COLLAPSE,LOWERCASE")
$ imail_host = f$trnlnm("TCPIP$INET_HOST")
$ imail_domain = f$trnlnm("TCPIP$INET_DOMAIN")
$ if (imail_host .eqs. "") then imail_host = syi_scsnode
$ if (imail_domain .nes. "") then imail_domain = ".''imail_domain'"
$ imail_default_sender = f$trnlnm("IMAIL_SENDER")
$ if (imail_default_sender .eqs. "")
$   then
$   imail_default_sender = "''jpi_username'@''imail_host'''imail_domain'"
$ endif !(imail_default_sender .eqs. "")
$ imail_organization = f$trnlnm("IMAIL_ORGANIZATION")
$ if (imail_organization .eqs. "") then -
$   imail_organization = f$trnlnm("IMAIL_ORGANISATION")
$ imail_test = f$trnlnm("IMAIL_TEST")
$ imail_verbose = f$trnlnm("IMAIL_VERBOSE")
$ imail_retain_sff = f$trnlnm("IMAIL_RETAIN_SFF")
$ if (imail_retain_sff .eqs. "") then imail_retain_sff = "False"
$ if (imail_verbose)
$   then
$   say "''procedure_name'  ''imail_version'"
$   err I SENDER "current sender is ''imail_default_sender'"
$ endif !(imail_verbose)
$ imail_version_type = "V"
$ imail_min_vmsversion = "7.2"
$ imail_options = "/DELIVER/SEND/ENCODE/VALIDATE/"
$ imail_options_len = f$length(imail_options)
$ imail_encode_image = "SYS$SYSTEM:TCPIP$UUENCODE.EXE"
$ imail_encode = "$''imail_encode_image'"
$ imail_sff_image = "SYS$SYSTEM:TCPIP$SMTP_SFF.EXE"
$ imail_sff = "$''imail_sff_image'"
$ imail_from_1 == "MAIL FROM:"			! mail<sp>from:
$ imail_from_2 == "MAIL	FROM:"			! mail<tab>from:
$ imail_from_len == f$length(imail_from_1)
$ imail_to_1 == "RCPT TO:"			! rcpt<sp>to:
$ imail_to_2 == "RCPT	TO:"			! rcpt<tab>to:
$ imail_to_len == f$length(imail_to_1)
$ imail_data == "DATA"
$ imail_sff_file == ""
$ if (((syi_version_type .nes. imail_version_type) -
	.or. (syi_version_number .lts. imail_min_vmsversion)) -
	.and. (.not. imail_test))
$   then
$   imail__status == ss$_unsupported
$   err E BADVERSION "this version of VMS is not supported"
$   err I MINVERSION -
	"must be at least ''imail_version_type'''imail_min_vmsversion'" y
$ else
$ if ((f$search(imail_sff_image) .eqs. "") .and. (.not imail_test))
$   then
$   imail__status = rms$_fnf
$   err E IMAGELOST "unable to locate the SFF image"
$ else
$ if ((f$search(imail_encode_image) .eqs. "") .and. (.not. imail_test))
$   then
$   imail__status = rms$_fnf
$   err E IMAGELOST "unable to locate the UUENCODE image"
$ else
$ imail_option = f$edit(P1,"COLLAPSE,UPCASE")
$ if (imail_option .eqs. "")
$   then
$   imail__status == ss$_badparam
$   err E NOCHOICE "you have not specified an option"
$ else
$ if (f$locate("/''imail_option'/",imail_options) .eq. imail_options_len)
$   then
$   imail__status == ss$_badparam
$   err E BADCHOICE "you have chosen an invalid option <''imail_option'>"
$ else
$ call imail_'imail_option' "''P2'" -
				"''P3'" -
				"''P4'" -
				"''P5'" -
				"''P6'" -
				"''P7'" -
				"''P8'"
$ endif !(imail_option .eqs. "")
$ endif !(f$locate("/''imail_option'/",imail_options) .eq. imail_options_len)
$ endif !(f$search(imail_encode_image) .eqs. "")
$ endif !(f$search(imail_sff_image) .eqs. "")
$ endif !((syi_version_type .nes. imail_version_type) -
$bail_out:
$ exitt imail__status.or.f$verify(__vfy_saved)
$ !+==========================================================================
$ !
$ ! Procedure:	IMAIL_ENGINE.COM
$ !
$ ! Purpose:	To use the undocumented/unsupported SFF utility to send
$ !		internet mail.
$ !
$ ! Parameters:
$ !	P1	This defines the action to be taken.  The remaining parameters
$ !		are dependent on the action - read on.
$ !		For those options that allow multiple filenames, P2 though P8
$ !		can contain either a single filename or a comma separated list
$ !		of filenames; always bearing in mind the DCL command line
$ !		requirements.
$ !
$ !	DELIVER filename[,filename...] [filename[,filename...]]
$ !
$ !		The deliver option will simply pass the specified file(s) to
$ !		SFF on the assumption that each file is in the correct format
$ !		and that all required headers are in place.  Each file is
$ !		first checked to ensure a minimum of valid SMTP headers exist.
$ !
$ !	SEND <filename>[/<mimetype>] <recipient> [<sender>]
$ !
$ !		The send option will send a single file to the specified
$ !		recipient.  It is assumed that the file to send does NOT have
$ !		any MIME headers and a basic set will be generated based on
$ !		the command line parameters and the logicals described below.
$ !		The <mimetype> qualifier allows selection of either plain text
$ !		of HTML content.  Use "/PLAIN" or "/TEXT" for plain text or
$ !		"/HTML" for HTML (that wasn't hard now was it).  The default
$ !		is to use plain text unless the IMAIL_MIME_TYPE logical is
$ !		defined or the command line qualifier is used.
$ !		The <filename> specified in the command is NOT modified.
$ !		A unique filename is generated to contain the required headers
$ !		and the specified file is appended to this new file.
$ !		A symbol IMAIL_SFF_FILE will contain the name of the file.
$ !
$ !	ENCODE filename[,filename...] [filename[,filename...]]
$ !
$ !		The encode option will take the specified file(s) and encode
$ !		them using UUENCODE, which produces a BASE64 file suitable for
$ !		sending via SMTP.  The output filename is that of the input
$ !		file specification with a filetype of ".UUE".  This cannot be
$ !		changed.
$ !
$ ! Logicals:
$ !
$ !	IMAIL_MIME_TYPE  is used to define the default MIME type for files.
$ !		If not defined, the default is for plain text.  If defined,
$ !		the value must be "PLAIN", "TEXT" or "HTML".  Any other values
$ !		are invalid and will be ignored, the type will become "PLAIN".
$ !	IMAIL_SENDER  will be used as the <sender> if one is not specified
$ !		in the command line.  DO NOT include angle brackets, they will
$ !		be added automatically.
$ !	IMAIL_ORGANIZATION  will be used as the "Organization:" value in the
$ !		MIME headers if it is defined.  The logical can also be
$ !		spelled with an "S" rather than a "Z" for those that find this
$ !		necessary.
$ !
$ !		$ define imail_sender "dbsneddon@bigpond.com"
$ !		$ define imail_organization "Sneddo's all night bar & grille"
$ !
$ ! Unsupported options:
$ !
$ !	VALIDATE filename[,filename...] [filename[,filename...]]
$ !
$ ! Unsupported logicals:
$ !
$ !	IMAIL_RETAIN_SFF  This logical determines whether generated SFF files
$ !		are retained.  The default is to NOT retain these files.
$ !	IMAIL_TEST  If this logical is defined as "true", the initial checks
$ !		for VMS version and utility availablity are ignored.  Useful
$ !		for testing.
$ !	IMAIL_VERBOSE  If this logical is defined as "true", information
$ !		messages are displayed for various successful operations
$ !		rather than messages only being generated for errors.
$ !
$ ! History:
$ !		23-Oct-2000, DBS; Version X1-001
$ !	001 -	Original version.
$ !		24-Oct-2000, DBS; Version X1-002
$ !	002 -	Added IMAIL_RETAIN_SFF logical to control retention of
$ !		generated files.  The default is to delete them.
$ !-==========================================================================
