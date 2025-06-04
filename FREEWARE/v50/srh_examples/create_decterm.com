$!To compile, link and run CREATE_DECTERM (in SYS$LOGIN:)
$ cc create_decterm
$ link create_decterm, sys$input/opt
sys$share:decw$xlibshr/share
sys$library:decw$dwtlibshr/share
sys$share:vaxcrtl/share
sys$share:decw$terminalshr/share
$! @remote_decterm mynode
