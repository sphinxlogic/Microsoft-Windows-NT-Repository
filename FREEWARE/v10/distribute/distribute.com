$	verify = 'f$verify(0)'
$
$! DISTRIBUTE.COM V2.1
$! Dave Porter  26-Aug-1985
$!
$! This procedure allows the recipient list for a document to be
$! included in the document itself. The recipient addresses must
$! be enclosed between "(*D:" and ")", for example: 
$!		Dave Porter (*D: SMAUG::PORTER)
$! The recipient list is parsed out and the document is then sent,
$! using Nmail if it is present, otherwise using vanilla MAIL.
$!
$! Revision history:
$!
$! V1.1	16-Oct-1985
$!	Tidy up for inclusion in Nmail V8.1 kit
$! V2.0 23-Jul-1986
$!	Unbundle from Nmail kit
$! V2.1 11-Mar-1987
$!	Use SET SYMBOL to avoid problems with symbol clashes
$!
$	set noon 
$	set symbol/scope=(nolocal,noglobal)
$	on control_y then goto the_end
$	utag = f$cvtime(f$time()) - "-" - "-" - " " - ":" - ":" - "."
$
$	nmail = 0
$	if f$search("sys$share:nm_mailshr.exe") .nes. "" then -
	  if f$file_attrib("sys$share:nm_mailshr.exe","known") then -
	    nmail = 1
$
$	mail_prefix = ""
$	if nmail then mail_prefix = "NM%"
$	prefix_length  = f$length(mail_prefix)
$	address_opener = "(*D:"
$	opener_length  = f$length(address_opener)
$	address_closer = ")"
$	closer_length  = f$length(address_closer)
$
$	distlist       = "SYS$SCRATCH:''utag'.TMP;"
$
$! Open message file 
$	msgfile = p1
$10:	if msgfile .nes. "" then goto 20
$	read /error=the_end /prompt="Message file: " sys$command msgfile
$	msgfile = f$edit(msgfile,"upcase,trim,compress")
$	goto 10
$20:	tmp = f$parse(msgfile,".TXT")
$	if tmp .eqs. "" then goto opnin
$	msgfile = tmp
$	open /read /error=opnin  $$in  'msgfile'
$
$! Get message subject (drop quotes and apostrophes as they cause problems)
$	subject = p2
$30:	if subject .nes. "" then goto 40
$	read /error=the_end /prompt="Subject:      " sys$command subject
$	subject = f$edit(subject,"trim")
$	goto 30
$40:	if f$locate("""",subject) .ge. f$length(subject) then goto 50
$	subject = subject - """"
$	goto 40
$50:	if f$locate("'",subject) .ge. f$length(subject) then goto 60
$	subject = subject - "'"
$	goto 50
$60:	if subject .eqs. "" then goto 30
$
$! Create distribution list file 
$	open /write /error=opnout  $$out  'distlist'
$
$! Read next line from file, upcase and so forth
$110:	read /error=inerr /end=140  $$in  line  
$	line = f$edit(line,"trim,upcase,compress")
$
$! Extract next address from within line
$120:	a0   = f$locate(address_opener,line) + opener_length
$	line = f$extract(a0,999,line)
$	a1   = f$locate(address_closer,line)
$	addr = f$edit(f$extract(0,a1,line),"trim")
$	if addr .eqs. "" then goto 130
$	if addr .eqs. "ENDLIST" then goto 140
$
$! Write address to distribution list
$	if f$extract(0,prefix_length,addr) .nes. mail_prefix then addr = mail_prefix + addr
$	write/error=outerr  $$out addr
$
$! Move on to rest of line, read new line if necessary
$130:	line = f$extract(a1+closer_length,9999,line)
$	if line .nes. "" then goto 120
$	goto 110
$
$! End-of-file or else "endlist" marker seen
$140:	close $$in
$	close $$out
$
$! Send the mail
$	assign/user "@''distlist'" distribution
$	mail 'msgfile' "distribution" /subject="''subject'"
$
$the_end:
$	on control_y then continue
$	if f$trnlnm("$$in")  .nes. "" then close $$in
$	if f$trnlnm("$$out") .nes. "" then close $$out
$	if f$search(distlist) .nes. "" then delete/nolog/noconfirm 'distlist'
$	exit 1 + (0 * f$verify(verify))
$	exit
$
$! File I/O errors
$inerr:
$	write sys$output "%DISTRIBUTE-E-READERR, error reading from ",msgfile
$	goto the_end
$outerr:
$	write sys$output "%DISTRIBUTE-E-WRITERR, error writing to ",distlist
$	goto the_end
$opnin:
$	write sys$output "%DISTRIBUTE-E-OPENIN, unable to open ",msgfile
$	if p1 .nes. "" then goto the_end
$	msgfile = ""
$	goto 10
$opnout:
$	write sys$output "%DISTRIBUTE-E-OPENOUT, unable to open ",distlist
$	goto the_end
$!
$! End of DISTRIBUTE.COM
$!
