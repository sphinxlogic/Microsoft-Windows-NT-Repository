$	VFY = 'f$verify()	!'f$verify(VFY$LNGCOPY)
$ !+++
$ !
$ ! LNGCOPY	Command file to copy DECspell V1.1 kits on the ENET
$ !
$ ! X01.00   	Nick Tatham	1-AUG-84
$ !
$ !---
$
$	uk_loc = "VIA::DISK$CBRLCI:[DECSPELL.V11KIT]"	! Location of UK kit storage
$	us_loc = "VIA::DISK$CBRLCI:[DECSPELL.V11KIT]"	! Location of UK kit storage
$	location = us_loc				! Default location for this node 
$	version = "V1.1-23"				! Current DECspell version number
$
$	write sys$output ""
$	write sys$output "VAX DECspell ''version' kit copy procedure"
$	write sys$output ""
$
$get_language:
$	default = "AMERICAN"
$	read/prompt="Do you want the default dictionary to be AMERICAN or BRITISH [''default']:" /end=exit sys$command answer
$	answer := 'answer'
$	if answer .eqs. "" then answer = default
$	american = ""
$	if f$locate(answer,"AMERICAN") .eq. 0 then american = "1"
$	if f$locate(answer,"BRITISH") .eq. 0 then american = "0"
$	if american .nes. "" then goto got_language
$	write sys$output "Sorry the ''answer' language is not available in this release!"
$	goto get_language
$got_language:
$	if 'american' then other = "BRITISH"
$	if .not. 'american' then other = "AMERICAN"
$	if 'american' then kitname = "LNGVCA011.A"
$	if .not. 'american' then kitname = "LNGVCB011.A"
$	if 'american' then othername = "LNGALB011.A"
$	if .not. 'american' then othername = "LNGALA011.A"
$	if .not. 'american' then location = uk_loc
$	node = f$extract(0,f$locate("::",location),location)
$
$get_extradic:
$	default = "NO"
$	read/prompt="Do you want the alternate ''other' dictionary [''default']:"/end=exit sys$command answer
$	answer := 'answer'
$	if answer .eqs. "" then answer = default
$	extradic = ""
$	if f$locate(answer,"YES") .eq. 0 then extradic = "1"
$	if f$locate(answer,"NO") .eq. 0 then extradic = "0"
$	if extradic .nes. "" then goto got_extradic
$	write sys$output "Please answer YES or NO"
$	goto get_extradic
$got_extradic:
$
$
$	write sys$output ""
$	write sys$output "VAX DECspell documentation now being copied from ''location'"
$	write sys$output ""
$
$	copy 'location'spluser.txt * /log
$	copy 'location'splinst.txt * /log
$
$	write sys$output ""
$	write sys$output "VAX DECspell kits now being copied from ''location'"
$	write sys$output ""
$
$	copy 'location''kitname' * /log
$	if 'extradic' then copy 'location''othername' * /log
$	define /user sys$output: nl:
$exit:
$	write sys$output ""
$	write sys$output "VAX DECspell copy procedure completed"
$	write sys$output ""
$
$	set noverify
$	if VFY then set verify
