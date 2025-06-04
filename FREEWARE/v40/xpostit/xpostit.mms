include	=   dir.h,plaid.h,plaidp.h,xpostit.h,xpostit.h

obj	=   xpostit.obj,confirm.obj,dialog.obj,menu.obj,note.obj,-
	    plaid.obj,plaid2.obj,remote.obj,util.obj,vmsutil.obj

#cdebug	=   /debug/noopt
#ldebug	=   /debug

.c.obj :
	define/user sys sys$library
	define/user x11 decw$include
	$(CC) $(CDEBUG) $(CFLAGS) $(MMS$SOURCE)

xpostit.exe : $(obj)
	link$(LDEBUG) $(obj), decw/opt

$(obj) : $(include)
