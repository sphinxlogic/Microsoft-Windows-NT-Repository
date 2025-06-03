$! Alpha build of Xanim
$ IF P1 .EQS. "LINK" THEN GOTO LINK_IT
$!
$ compile := cc/stand=vaxc/prefix=all/opt
$
$ compile xanim
$ compile xanim_x11
$ compile xanim_fli
$ compile xanim_iff
$ compile xanim_gif
$ compile xanim_txt
$ compile unpacker
$ compile xanim_utils
$ compile xanim_act
$ compile xtimes
$ compile txtmerge
$ compile find_best_window_position
$
$ library/repl/lo/noglo xanim *.obj
$ del *.obj;*
$
$LINK_IT:
$ write sys$output "Linking XANIM..."
$ link/alpha 'P2' /exe=xanim_alpha xanim/lib/include=(xanim,-
						xanim_x11,-
					        xanim_fli,-
					        xanim_iff,-
					        xanim_gif,-
					        xtimes,-
					        xanim_txt,-
					        unpacker,-
					        xanim_utils,-
					        xanim_act,-
						find_best_window_position-
						), sys$input/option
name="XANIM"
ident="V1"
SYS$LIBRARY:DECW$xmulibshrr5.EXE /SHARE
SYS$LIBRARY:DECW$xmlibshr12.EXE /SHARE
SYS$LIBRARY:DECW$DWTLIBSHR.EXE /SHARE
SYS$LIBRARY:DECW$XlibShr.EXE /SHARE
SYS$LIBRARY:DECW$dxmlibShr12.EXE /SHARE
SYS$LIBRARY:DECW$xtlibShrr5.EXE /SHARE
SYS$LIBRARY:DECW$xextlibShr.EXE /SHARE
SYS$LIBRARY:DECW$mrmlibShr12.EXE /SHARE
!SYS$LIBRARY:DECW$xtShr.EXE /SHARE
$ pur/nolog *.exe, *.lis
$ exit
