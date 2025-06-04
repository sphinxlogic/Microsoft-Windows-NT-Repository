$ cc/nolist avl
$ cc/nolist buttons/opt=noinline	! makes the compile faster
$ cc/nolist clientlib
$ cc/nolist compose
$ cc/nolist cursor
$ cc/nolist dialogs
$ cc/nolist error_hnds
$ cc/nolist internals
$ cc/nolist killfile
$ cc/nolist menus
$ cc/nolist mesg
$ cc/nolist newsrcfile
$ cc/nolist newsrc_parse
$ cc/nolist pane
$ cc/nolist resources
$ cc/nolist save
$ cc/nolist server
$ cc/nolist slist
$ cc/nolist t6
$ cc/nolist t7
$ cc/nolist utils
$ cc/nolist xmisc
$ cc/nolist xrn
$ cc/nolist xthelper
$ link/exe=dxrn sys$input/opt
xrn,buttons,compose,cursor,dialogs,error_hnds,menus,mesg
newsrcfile,newsrc_parse,resources,internals,save,server,utils
xmisc,xthelper,avl,clientlib,t6,t7,pane,slist,killfile
sys$share:decw$dwtlibshr/share
sys$share:decw$xlibshr/share
sys$share:vaxcrtl/share
!
! Remove this or put in your TCP/IP library
!
sys$share:ucx$ipc/library
!
! For example, to use Multinet:
!
!multinet_socket_library/share
$ exit
