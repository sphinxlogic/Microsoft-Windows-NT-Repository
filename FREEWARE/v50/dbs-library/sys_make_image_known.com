$ ! Procedure:	SYS_MAKE_IMAGE_KNOWN.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	saved_message = f$environment("message")
$	set message/facility/severity/identification/text
$	param_count = 1
$param_loop_1:
$	P'param_count' = f$edit(P'param_count', "upcase, collapse")
$	param_count = param_count + 1
$ if (param_count .le. 8) then goto param_loop_1
$	say = "write sys$output"
$	reqparmis = "%MAKEKNOWN-E-REQPARMIS, required parameter is missing"
$	notfound = "%MAKEKNOWN-E-NOTFOUND, image !AS not found"
$	operation = "add"
$	options = "/open/header"
$	image_name = f$element(0, "/", P1)
$	if (image_name .eqs. "")
$		then
$		say f$fao(reqparmis)
$	else
$	P1 = P1 - image_name
$	image_name = f$element(0, ";" -
			,f$parse(image_name, "SYS$SYSTEM:.EXE",,,"syntax_only"))
$	if (f$search(image_name) .eqs. "")
$		then
$		!say f$fao(notfound, image_name)
$	else
$	if (f$file_attributes(image_name, "known")) then operation = "replace"
$	options = "''options'''P1'''P2'''P3'''P4'''P5'''P6'''P7'''P8'"
$	install 'operation' 'image_name' 'options'
$	endif !(f$search(image_name) .eqs. "")
$	endif !(image_name .eqs. "")
$	set message'saved_message'
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_MAKE_IMAGE_KNOWN.COM
$ !
$ ! Purpose:	To install an image if it is not already installed or to
$ !		replace the installed image if it is already installed and
$ !		generally give some nicer error messages than one normally
$ !		gets from the system.
$ !		The default install is for /open/header.
$ !
$ ! Parameters:
$ !	 P1	The name of the image, defaults are SYS$SYSTEM:.EXE.
$ !   [P2..P8]	Optional parameters for the install, /open and /header are
$ !		supplied by default and therefore not required.
$ !
$ ! History:
$ !		12-Oct-1989, DBS; Version V1-001
$ !	001 -	Original version.
$ !-==========================================================================
