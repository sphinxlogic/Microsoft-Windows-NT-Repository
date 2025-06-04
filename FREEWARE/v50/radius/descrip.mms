CFLAGS		=	/noWARN /noMEMB /preffix=all 

SERVER_OBJS	=	radiusd.oBJ,dict.oBJ,users.oBJ,util.oBJ,-
			md5.OBJ,attrprint.OBJ,acct.obj,version.obj,-
			verf-user.obj

SERVER_SRCS	=	radiusd.c,dict.c,users.c,util.c,md5.c,attrprint.c,-
			acct.c,version.c

INCLUDES	=	radius.h,conf.h

TARGET		=	radiusd.exe

!________________________________________________________________

All	:$(TARGET)


radiusd.exe	: $(SERVER_OBJS)
	link /exe=radiusd.exe $(SERVER_OBJS) /nodeb/notrace

!________________________________________________________________
radiusd.obj	: radiusd.c, $(INCLUDES)
acct.obj	: acct.c, $(INCLUDES)
attrprint.obj	: attrprint.c, $(INCLUDES)
dict.obj	: dict.c, $(INCLUDES)
users.obj	: users.c $(INCLUDES)
usersdbm.obj	: users.c, $(INCLUDES)
util.obj	: util.c,$(INCLUDES)
version.obj	: version.c,$(INCLUDES)
md5.obj		: md5.c,md5.h
!________________________________________________________________

clean		:
	del *.obj;*,*.exe;*
