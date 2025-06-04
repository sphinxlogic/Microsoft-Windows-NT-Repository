$   if f$getsyi("hw_model") .ge. 1024
$   then	    ! ALPHA
$	if f$search("guess_pass.obj_alpha") .eqs. ""
$	then
$	    write sys$output "Compiling for AXP..."
$	    cc/stand=vaxc [.src]guess_pass/obj=guess_pass.obj_alpha
$	endif
$	write sys$output "Linking for AXP..."
$	link/exe=guess_pass.exe_alpha guess_pass.obj_alpha
$	copy guess_pass.exe_alpha guess_pass.exe
$   else	    ! VAX
$	if f$search("guess_pass.obj_vax") .eqs. ""
$	then
$	    write sys$output "Compileing for VAX..."
$	    cc [.src]guess_pass/obj=guess_pass.obj_vax
$	endif
$	write sys$output "Linking for VAX..."
$	link/exe=guess_pass.exe_vax guess_pass.obj_vax,vaxc.opt/opt
$	copy guess_pass.exe_vax guess_pass.exe
$   endif
