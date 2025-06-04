$! capture_reply.com -- capture the output from the REPLY/TO command
$! p1 -- operator to make request to , DEF=CENTRAL
$! p2 -- message text, no default
$! p3 -- Global Symbol to put reply text into, DEF=OPERATOR_REPLY
$! p4 -- Global Symbol to put the reply time into, DEF=OPERATOR_TIME
$! p5 -- Global Symbol to put the reply username into, DEF=OPERATOR_TERMINAL
$! p6 -- if present, and = "S" then p2 is a symbol with the message string
$!	 in it.  It is copied from the symbol
$ vfl = f$ver(0)
$ set noon
$ status = $status
$ null = ""
$ comma = ","
$ crlf = f$fao("!/")
$ cr = f$ext(0,1,crlf)
$ lf = f$ext(1,1,crlf)
$ show sym crlf
$ operator = p1
$ if operator.eqs.null then $ operator = "CENTRAL"
$ if f$edit(f$ext(0,1,p6),"upcase").eqs."S" then $ p2 = 'p2'
$ message = p2
$ if message.eqs.null then $ goto ERR_NOMESSAGE
$ reply_sym = p3
$ if reply_sym.eqs.null then $ reply_sym = "OPERATOR_REPLY"
$ time_sym = p4
$ if time_sym.eqs.null then $ time_sym = "OPERATOR_TIME"
$ name_sym = p5
$ if name_sym.eqs.null then $ name_sym = "OPERATOR_TERMINAL"
$ tmp_file = "sys$login:reply_"+(f$cvtime()-" "-":"-":"-".")+".tmp"
$ if vfl then $ show sym tmp_file
$ define/user sys$output 'tmp_file'
$ request/reply/to=('operator') "''message'"
$ status = $status
$ if f$trnlnm("message_file","lnm$process").nes.null then $ close message_file
$ open/read/error=ERR_FILE message_file 'tmp_file'
$ read/end=ERR_FILE message_file header
$ read/end=ERR_FILE message_file reply_text
$ show sym header
$ show sym reply_text
$ close message_file
$ 'reply_sym' == f$element(1,lf,f$element(1,cr,reply_text))
$ note_field == f$element(2,lf,reply_text)
$ 'time_sym' == f$element(0,comma,note_field)
$ 'name_sym' == f$extract(9+f$locate("operator",note_field),999,note_field)
$EXIT:
$ if f$trnlnm("message_file","lnm$process").nes.null then $ close message_file
$ delete 'tmp_file';*/nolog
$ if vfl then $ show sym 'reply_sym'
$ if vfl then $ show sym 'time_sym'
$ if vfl then $ show sym 'name_sym'
$ exit status ! 'f$ver(vfl)'
$ERR_NOMESSAGE:
$ STATUS == "%X100381F4"
$ say "CAPTURE-E-ABSENT, missing message text:  P2"
$ goto EXIT
$ERR_FILE:
$ status == $status
$ say "CAPTURE-E-FILE, error handling message file"
$ goto EXIT
$!Last Modified:  26-AUG-1987 13:38:09.69, By: RLB 
