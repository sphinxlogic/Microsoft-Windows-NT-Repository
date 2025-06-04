$ arch= f$getsyi ("arch_name")
$ cmd=p1
$ if cmd.eqs."" then cmd:= add
$ if cmd.eqs."ADD"
$ then	copy [.'arch']acc_ckpt.exe sys$common:[sys$ldr];/log
$ else	if p1.nes."REMOVE"
$	then	write sys$output "Syntaxe: @install [ADD/REMOVE]
$		exit
$	endif
$	set noon
$	delete sys$loadable_images:acc_ckpt.exe;*/log
$ endif
