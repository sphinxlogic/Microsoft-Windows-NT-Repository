$ ver = 'f$verify(0)'
$ say := write sys$output
$ gindex := $gopher$root:[v1216]build_index
$ GR := "GOPHER_ROOT"
$ if P1 .nes. "" then goto got_doc
$get_doc:
$ inquire/nopunc P1 "Specify document/file to be indexed: "
$ if f$length(P1) .eq. 0 then goto done
$ if f$extract(0,f$length(GR),"''f$parse(P1,,,"device")'") .nes. GR
$	then
$	say "Must be within GOPHER_ROOT:[...]"
$	goto get_doc
$	endif
$ if f$search(P1) .nes. "" then goto got_doc
$ say "Can't find file ''P1'"
$ goto get_doc
$got_doc:
$ if P2 .nes. "" then goto got_parms
$ gindex
$ say " /CANCEL         cancel this whole thing"
$ inquire/nopunc P2 "Specify parameters (/'s required) : "
$ if f$length(P2) .eq. 0 then goto got_doc
$ if f$locate("/CA",f$edit(P2,"TRIM, UPCASE")) .lt. f$length(P2) then goto done
$got_parms:
$ status = 0
$ gindex 'P1 'P2
$ gst = status
$ if gst .eq. 7
$	then
$	say "Illegal syntax"
$	goto get_parms
$	endif 
$ if gst .eq. 1 
$	then
$	wait 00:00:05
$	goto get_doc
$	endif
$ if gst .eq. 3 then say "Can't open file ''P1'"
$ if gst .eq. 5 then say "This requires programming changes at LCS"
$ if (gst .and. 1) .eq. gst then wait 00:00:10
$ ! Optimize -- not quite yet....
$ ! index = P1 - f$parse(P1,,,"version") - f$parse(P1,,,"type") + ".idx"
$ ! @gopher$root:[v1216]optimize_index 'index
$done:
$ ver = f$verify(ver)
