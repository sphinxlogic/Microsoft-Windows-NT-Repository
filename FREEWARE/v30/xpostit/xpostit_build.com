$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc xpostit
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc confirm
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc dialog
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc menu
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc note
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc plaid
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc plaid2
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc remote
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc util
$ define/user sys sys$library:
$ define/user x11 decw$include:
$ cc vmsutil
$ link xpostit,confirm,dialog,menu,note,plaid,plaid2,-
	remote,util,vmsutil,decw.opt/opt
