$ save_verify = 'f$verify(0)'
$ alpha = f$getsyi("HW_MODEL").ge.1024
$!
$!  File to build Ehud Gavron's INVISIBLE
$!
$!  Author:	Hunter Goatley
$!
$ say := write sys$output
$ on error then goto common_exit
$ on contrl_y then goto common_exit
$ say "Extracting $JIBDEF and $PCBDEF from LIB.MLB...."
$ library/macro/extr=$JIBDEF/out=jibdef.mar sys$library:lib.mlb
$ library/macro/extr=$PCBDEF/out=pcbdef.mar sys$library:lib.mlb
$ say "Converting $*DEF macros to C .H files...."
$ call convert_to_h jibdef.mar
$ call convert_to_h pcbdef.mar
$ say "Compiling INVISIBLE...."
$ cc invisible
$ say "Linking INVISIBLE...."
$ sysexe = ",sys$system:sys.stb/selective_search"
$ if alpha then sysexe = "/sysexe"
$ link/notrace invisible'sysexe'
$ say "INVISIBLE build completed"
$ common_exit:
$	exit f$verify(save_verify).or.1
$ convert_to_h: subroutine
$ name = f$parse(p1,"","","NAME")
$ open/read tmp 'p1'
$ create 'name'.H
$ open/append tmph 'name'.H
$ cvt_loop:
$    read/error=cvt_fin tmp line
$    if f$extract(0,4,line).nes."$EQU" then goto cvt_loop
$    hex = f$locate("^X",line)
$    if hex.nes.f$length(line) then -
	line = f$extract(0,hex,line)+"0x"+f$extract(hex+2,255,line)
$    line = line - "<" - ">"
$    write tmph "#define ",f$extract(4,255,line)
$    goto cvt_loop
$ cvt_fin:
$    close tmp
$    close tmph
$ write sys$output "C header file ''name'.H created"
$ exit
$ endsubroutine
