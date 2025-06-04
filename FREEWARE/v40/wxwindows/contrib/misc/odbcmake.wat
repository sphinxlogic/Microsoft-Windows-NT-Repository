CC	= wcc386
CXX	= wpp386
LIB     = wlib
IFLAGS  = -i=..\\..\\include\\base;..\\..\\include\\msw
CFLAGS  = $(IFLAGS) -zq -zW -w1 -os -3 -dwx_msw

WXDIR   = ..\\..
LIBTARGET = $(WXDIR)\\lib\\wx.lib

C_SRCS = odbc32.c

OBJECTS = $(C_SRCS:.c=.obj) $(CC_SRCS:.cc=.obj)

.c.obj:
	$(CC) $(CFLAGS) $< 

.cc.obj:
	$(CXX) $(CFLAGS) $< 

all: $(OBJECTS) $(LIBTARGET)

$(LIBTARGET): $(OBJECTS)
	$(LIB) /P=256 $(LIBTARGET) @odbc.lnk
