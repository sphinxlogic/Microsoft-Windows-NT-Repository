$! Compile and link Xsnow under VMS 
$!
$! Get compiler info
$!
$ XSNOW_VERSION = "Xsnow 1.41"
$ 
$ echo := write sys$output
$ echo " --- This is a command procedure to build ''XSNOW_VERSION' ---"
$ 
$ CFLAGS = "/OPTIMIZE"
$ decc = f$search("SYS$SYSTEM:DECC$COMPILER.EXE") .nes. "" -
        .and. CFLAGS - "/VAXC" .eqs. CFLAGS
$ all_cflags = CFLAGS
$ if decc then all_cflags = "/DECC/PREFIX=ALL " + all_cflags
$!
$!      Build the option-file
$!
$ open/write optf xsnow.opt
$ write optf "xsnow"
$ if .not.decc 
$ then 
$ 	if f$trnlmn("MULTINET").nes.""
$ 	then
$ 		! Try first the SHARE in MULTINET:
$		write optf "MULTINET:UCX$IPC_SHR/SHARE"
$ 	else
$ 		! Well, try the OLB then ...
$ 		if f$search("SYS$SHARE:UCX$IPC.OLB").nes."" 
$ 		then write optf "SYS$SHARE:ucx$ipc/lib"
$ 		endif
$ 	endif
$ endif
$ write optf "Identification=""''XSNOW_VERSION'""
$!
$!
$!  Find out which X-Version we're running.  This will fail for older
$!  VMS versions (i.e., v5.5-1).  Therefore, choose DECWindows XUI for
$!  default.
$!
$ On Error Then GoTo XUI
$ IF F$SEARCH("SYS$UPDATE:DECW$GET_IMAGE_VERSION.COM").NES.""
$ THEN
$  @sys$update:decw$get_image_version sys$share:decw$xlibshr.exe decw$version
$  if f$extract(4,3,decw$version).eqs."1.0"
$  then
$   write optf "Sys$share:DECW$DWTLIBSHR.EXE/Share"
$  endif
$  if f$extract(4,3,decw$version).eqs."1.1"
$  then
$   write optf "sys$share:decw$xmulibshr.exe/share"
$   write optf "sys$share:decw$xtshr.exe/share"
$  endif
$  if f$extract(4,3,decw$version).eqs."1.2"
$  then
$   write optf "sys$share:decw$xmulibshrr5.exe/share"
$   write optf "sys$share:decw$xtlibshrr5.exe/share"
$  endif
$  GoTo MAIN
$ ENDIF
$!
$XUI:
$!
$   write optf "Sys$share:DECW$DWTLIBSHR.EXE/Share"
$MAIN:
$!
$ set noon
$ write optf "sys$share:decw$xlibshr.exe/share"
$ if .not.decc then write optf "SYS$SHARE:VAXCRTL/SHARE"
$ close optf
$ echo " . Compiling Xsnow "
$ cc'all_cflags' xsnow.c
$ echo " . Linking Xsnow"
$ on error then goto ERR
$ link xsnow.opt/opt
$ echo ""
$ echo "Xsnow is ready."
$ echo "  XSNOW :== $''f$env("DEFAULT")'XSNOW"
$ echo ""
$ XSNOW :== $'f$env("DEFAULT")'XSNOW
$ exit
$ 
$ ERR:
$  echo ""
$  echo "An error occur while linking Xsnow - Sorry"
$ EXIT
