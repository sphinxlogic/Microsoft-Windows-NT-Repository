CC	      =	gcc
CFLAGS	      = -g -O3
BINDIR	      = ../bin
EXTHDRS	      = /usr/include/ctype.h \
		/usr/include/stdio.h
HDRS	      =
LDFLAGS	      =
LIBS	      =
LINKER	      = $(CC)
MAKEFILE      = Makefile
OBJS	      = cfc.o
PRINT	      = pr
PROGRAM	      = cfc
SRCS	      = cfc.c
#INSTALL	      = install -s
INSTALL	      = cp
all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

lint:
		lint cfc.c
clean:;		@rm -f $(OBJS) $(PROGRAM) core

depend:;	@mkmf -f $(MAKEFILE) "CFLAGS=$(CFLAGS)" PROGRAM=$(PROGRAM) BINDIR=$(BINDIR)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(BINDIR)
		${INSTALL} $(PROGRAM) $(BINDIR)

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

###
cfc.o: /usr/include/stdio.h /usr/include/ctype.h
