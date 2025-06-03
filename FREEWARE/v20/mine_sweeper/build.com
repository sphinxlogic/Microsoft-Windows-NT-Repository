$   if f$getsyi("hw_model") .ge. 1024
$   then	    ! ALPHA
$	if f$search("ms_main.obj_alpha") .eqs. ""
$	then
$	    write sys$output "Compiling for AXP..."
$	    cc/stand=vaxc [.src]ms_main/obj=ms_main.obj_alpha
$	endif
$	write sys$output "Linking for AXP..."
$	link/exe=msweeper.exe_alpha ms_main.obj_alpha
$   else	    ! VAX
$	if f$search("ms_main.obj_vax") .eqs. ""
$	then
$	    write sys$output "Compileing for VAX..."
$	    cc [.src]ms_main/obj=ms_main.obj_vax
$	endif
$	write sys$output "Linking for VAX..."
$	link/exe=msweeper.exe_vax ms_main.obj_vax,vaxc.opt/opt
$   endif
