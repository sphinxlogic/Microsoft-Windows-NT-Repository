$   if f$getsyi("hw_model") .ge. 1024
$   then	    ! ALPHA
$	if f$search("show_users_v12.obj_alpha") .eqs. ""
$	then
$	    write sys$output "Compiling for AXP..."
$	    fortran/align=all/warn=nousage [.src]show_users_v12/obj=show_users_v12.obj_alpha
$	endif
$	write sys$output "Linking for AXP..."
$	link/exe=show_users_v12.exe_alpha show_users_v12.obj_alpha
$   else	    ! VAX
$	if f$search("show_users_v12.obj_vax") .eqs. ""
$	then
$	    write sys$output "Compileing for VAX..."
$	    fortran/warn=nogen [.src]show_users_v12/obj=show_users_v12.obj_vax
$	endif
$	write sys$output "Linking for VAX..."
$	link/exe=show_users_v12.exe_vax show_users_v12.obj_vax
$   endif
