$!	ARCH.COM
$!	V1.0
$!+
$! Rename files
$!-
$ save_ver = f$verify(0)
$ if f$getsyi("HW_MODEL") .lt. 1024
$ then
$       new = "VAX"
$	old = "ALPHA"
$ else
$       new = "ALPHA"
$	old = "VAX"
$ endif
$ set noon
$!
$!-----------------------------------------------
$ a=f$search("XXX")
$ if f$search("*.OBJ") .nes. "" then delete/noconfirm/log *.obj;*
$ call rename *.exe
$!-----------------------------------------------
$ goto exit
$!
$ rename:
$ subroutine
$	set noon
$	file = p1
$	name = f$parse(p1,,,"NAME")
$	ext = f$parse(p1,,,"TYPE") - "."
$	old_file = name + "." + old + "_" + ext
$	new_file = name + "." + new + "_" + ext
$	if f$search(file) .eqs. ""
$	then
$		write sys$output -
"%ARCH-W-NOTFOUND, ''file' not found."
$		file_exists = "F"
$	else
$		file_exists = "T"
$	endif
$	if f$search(old_file) .nes. ""
$	then
$		write sys$output -
"%ARCH-W-EXISTS, ''old_file' already exists - not renamed."
$		exit
$	endif
$	if file_exists then rename/log 'file' 'old_file'
$	if f$search(new_file) .eqs. ""
$	then
$		write sys$output -
"%ARCH-W-NOTFOUND, ''new_file' not found."
$		new_file_exists = "F"
$	else
$		new_file_exists = "T"
$	endif
$	if new_file_exists then rename/log 'new_file' 'file'
$ endsubroutine
$!
$ exit:
$!'f$verify(save_ver)'
