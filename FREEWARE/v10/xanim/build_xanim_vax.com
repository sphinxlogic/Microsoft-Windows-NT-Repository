$! VAX build of Xanim
$!
$ cc   := cc
$ link := link
$
$ if P1 .EQS. "LINK" THEN GOTO LINK_IT
$
$ write sys$output "Compiling XANIM.C..."
$ cc xanim.c
$ write sys$output "Compiling XANIM_X11.C..."
$ cc xanim_x11.c
$ write sys$output "Compiling XANIM_FLI.C..."
$ cc xanim_fli.c
$ write sys$output "Compiling XANIM_IFF.C..."
$ cc xanim_iff.c
$ write sys$output "Compiling XANIM_GIF.C..."
$ cc xanim_gif.c
$ write sys$output "Compiling XANIM_TXT.C..."
$ cc xanim_txt.c
$ write sys$output "Compiling UNPACKER.C..."
$ cc unpacker.c
$ write sys$output "Compiling XANIM_UTILS.C..."
$ cc xanim_utils.c
$ write sys$output "Compiling XANIM_ACT.C..."
$ cc xanim_act.c
$ write sys$output "Compiling XTIMES.C..."
$ cc xtimes.c
$ write sys$output "Compiling TXTMERGE.C..."
$ cc txtmerge.c
$ write sys$output "Compiling FIND_BEST_WINDOW_POSITION..."
$ cc find_best_window_position.c
$ LINK_IT:
$ write sys$output "Linking XANIM..."
$ def/user sys$library sys$sysroot:[syslib]
$ def/user sys$share   sys$sysroot:[syslib]
$ link/exe=xanim_vax sys$input/option
        xanim
        xanim_x11
        xanim_fli
        xanim_iff
        xanim_gif
        xtimes
        xanim_txt
        unpacker
        xanim_utils
        xanim_act
	find_best_window_position
        sys$share:decw$xlibshr/shareable
        sys$share:decw$dwtlibshr/shareable
$ pur/nolog *.exe, *.lis, *.obj
$ exit
