$!
$! Procedure to build the MBX kits in a single directory environment
$! (derived from the MMS script used for development)
$!
$! © Marc Van Dyck, 01-JUN-1999
$!
$!==============================================================================
$!
$ arch = f$element ( f$getsyi ( "ARCH_TYPE" ) , "|" , "|VAX|AXP|" )
$ vax = arch .eqs. "VAX"
$ axp = arch .eqs. "AXP"
$!
$ here = f$element ( 0 , "]" , f$environment ( "PROCEDURE" ) ) + "]"
$ define int 'here'
$ write sys$output "Building MBX V1.2 for OpenVMS ''arch' in ''here'"
$!
$ macro :== macro /lis /cross /diag /obj /anal /nodeb
$ pascal_pen :== pascal /lis /cross /show=all /nodiag /env /noobj /anal /nocheck /nodeb /optim /nowarn
$ pascal_obj :== pascal /lis /cross /show=all /nodiag /obj /noenv /anal /nocheck /nodeb /optim /nowarn
$ link_exe :== link /exec /map /full /nodebug /notraceback
$ link_shr :== link /shar /map /full /nodebug /notraceback
$!
$ message /nosymbol /object mbx_messages.msg
$ message /file_name = mbx_messages /object = mbx_msgptrs.obj mbx_messages.msg
$ message /noobject /sdl = mbx_messages.sdl mbx_messages.msg
$!
$ sdl /langage = pascal mbx_messages.sdl
$ edit /tpu /nodisplay /command = add_ident.tpu /noinitialize mbx_messages.pas
$ delete mbx_messages.pas;-1
$ pascal_pen mbx_messages.pas
$!
$ sdl /language = pascal mbx_declarations.sdl
$ edit /tpu /nodisplay /command = add_ident.tpu /noinitialize mbx_declarations.pas
$ delete mbx_declarations.pas;-1
$ if vax then edit /tpu /nodisplay /command = change_integer_to_unsigned.tpu /noinitialize mbx_declarations.pas
$ if vax then delete mbx_declarations.pas;-1
$ pascal_pen mbx_declarations.pas
$!
$ pascal_obj mbx_cli.pas
$ pascal_obj mbx_routines.pas
$ if vax then macro mbx_xfr_vect.mar
$!
$ if vax then link_shr mbx_routines.obj + mbx_routines_vax.opt /options
$ if axp then link_shr mbx_routines.obj + mbx_routines_axp.opt /options
$ link_exe mbx_cli.obj + mbx_msgptrs.obj + mbx_cli.opt /options
$ link_shr mbx_messages.obj + mbx_messages.opt /options
