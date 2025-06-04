$ ! As distributed, this command file will build mxrn for 
$ ! DECwindows/Motif V1.0 or later.
$ !
$ ! This file is conditionalized to compile and link properly for
$ ! either VAX C on OpenVMS VAX or DEC C on OpenVMS AXP.
$ !
$ ! Include the following line if you're using a Motif Developer's kit
$ !@sys$common:[decw$motif]dxm_logicals
$ ! Otherwise, the following two lines
$ define x11 decw$include
$ define xm decw$include
$ define sys sys$Library
$ !
$ is_alpha = "false"
$ if f$getsyi("hw_model") .eq. 0 then is_alpha = "true"
$ if f$getsyi("hw_model") .gt. 1023 then is_alpha = "true"
$
$ CFLAGS = "/DEFINE=(MOTIF)"
$ if is_alpha 
$ then 
$   CFLAGS = CFLAGS + "/PREFIX=ALL"
$   CRTL = ""
$ endif
$!
$ if p1 .eqs. "LINK" then goto link
$ cc/nolist 'CFLAGS avl
$ cc/nolist 'CFLAGS buttons/opt=noinline	! Makes the compile faster
$ cc/nolist 'CFLAGS clientlib/define=(MOTIF,UCX)
$ cc/nolist 'CFLAGS compose
$ cc/nolist 'CFLAGS cursor
$ cc/nolist 'CFLAGS dialogs
$ cc/nolist 'CFLAGS error_hnds
$ cc/nolist 'CFLAGS internals
$ cc/nolist 'CFLAGS killfile
$ cc/nolist 'CFLAGS menus
$ cc/nolist 'CFLAGS mesg
$ cc/nolist 'CFLAGS newsrcfile
$ cc/nolist 'CFLAGS newsrc_parse
$ cc/nolist 'CFLAGS resources
$ cc/nolist 'CFLAGS save
$ cc/nolist 'CFLAGS server
$ cc/nolist 'CFLAGS slist
$ cc/nolist 'CFLAGS t6
$ cc/nolist 'CFLAGS t7
$ cc/nolist 'CFLAGS utils
$ cc/nolist 'CFLAGS xmisc
$ cc/nolist 'CFLAGS xrn
$ cc/nolist 'CFLAGS xthelper
$link:
$ if is_alpha then goto link_alpha
$ !
$ ! Link for OpenVMS VAX
$ !
$ if f$search("sys$share:decw$dxmlibshr12.exe") .nes. ""
$ then
$ link/exe=mxrn sys$input/opt
xrn,buttons,compose,cursor,dialogs,error_hnds,menus,mesg
newsrcfile,newsrc_parse,resources,internals,save,server,utils
xmisc,xthelper,avl,clientlib,t6,t7,slist,killfile
sys$share:decw$dxmlibshr12.exe/share
sys$share:decw$xmlibshr12.exe/share
sys$share:decw$xtlibshrr5.exe/share
sys$share:decw$xlibshr.exe/share
sys$share:vaxcrtl/share
!
! Remove this or put in your TCP/IP library
!
sys$share:ucx$ipc/library
!
! For example, to use Multinet:
!
!multinet_socket_library/share
$ else
$ link/exe=mxrn sys$input/opt
xrn,buttons,compose,cursor,dialogs,error_hnds,menus,mesg
newsrcfile,newsrc_parse,resources,internals,save,server,utils
xmisc,xthelper,avl,clientlib,t6,t7,slist,killfile
!use these libraries for Motif Developer's kit V1.0:
!Xm:DECW$MOTIF$XM.OLB/LIBRARY
!Xt:DECW$MOTIF$XT.OLB/LIBRARY
!Xt:CLIB.OLB/LIBRARY
!
!use these for Motif Developer's kit V1.1.
!
!sys$share:decw$motif$dxmshr/shareable
!sys$share:decw$motif$xmshr/shareable
!sys$share:decw$motif$xtshr/shareable
!
!use these for DECwindows/Motif V1.0
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
!
!use these for either version.
!
sys$share:decw$xlibshr/shareable
sys$share:vaxcrtl/share
!
! Remove this or put in your TCP/IP library
!
sys$share:ucx$ipc/library
!
! For example, to use Multinet:
!
!multinet_socket_library/share
$ endif
$ exit
$ !
$ ! Link for OpenVMS AXP
$ !
$link_alpha:
$ if f$search("sys$share:decw$dxmlibshr12.exe") .nes. ""
$ then
$ link/exe=mxrn sys$input/opt
xrn,buttons,compose,cursor,dialogs,error_hnds,menus,mesg
newsrcfile,newsrc_parse,resources,internals,save,server,utils
xmisc,xthelper,avl,clientlib,t6,t7,slist,killfile
sys$share:decw$dxmlibshr12.exe/share
sys$share:decw$xmlibshr12.exe/share
sys$share:decw$xtlibshrr5.exe/share
sys$share:decw$xlibshr.exe/share
!
! Remove this or put in your TCP/IP library
!
sys$share:ucx$ipc_shr/share
!
! For example, to use Multinet:
!
!multinet_socket_library/share
$ else
$ link/exe=mxrn sys$input/opt
xrn,buttons,compose,cursor,dialogs,error_hnds,menus,mesg
newsrcfile,newsrc_parse,resources,internals,save,server,utils
xmisc,xthelper,avl,clientlib,t6,t7,slist,killfile
sys$share:decw$dxmlibshr.exe/share
sys$share:decw$xmlibshr.exe/share
sys$share:decw$xtshr.exe/share
sys$share:decw$xlibshr.exe/share
!
! Remove this or put in your TCP/IP library
!
sys$share:ucx$ipc_shr/share
!
! For example, to use Multinet:
!
!multinet_socket_library/share
$ endif
$ exit
