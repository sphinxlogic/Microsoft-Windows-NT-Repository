###
### Build the application "coloredit".
###

### Edit these variables.

# C compiler
CC = cc

# X11 and Motif headers and libraries
X_INC	=
X_LIBS	=

# compiler options
CCOPTS	= -O

# destination of installation
DESTDIR	= ${HOME}
DESTBIN	= ${DESTDIR}/bin
DESTAPP	= ${DESTDIR}/Xres

### No more edits are necessary!

DEST	= coloredit
CFLAGS	= ${CCOPTS} ${X_INC}

OBJS = cedit.o appwin.o colors.o tablewin.o

all: ${DEST}
	@echo "------------------------------------"
	@echo "XColorEdit build complete on"
	@date
	@echo "------------------------------------"

${DEST}: ${OBJS}
	@echo Building $@
	@${CC} ${CCOPTS} -o $@ $(OBJS) ${X_LIBS} -lXm -lXt -lX11

install: ${DEST}
	@echo Installing in ${DESTDIR}
	@cp `pwd`/${DEST} ${DESTBIN}
	@cp `pwd`/XColorEdit ${DESTAPP}

clean:
	rm -rf $(OBJS) ${DEST}

###

cedit.o:	cedit.h appwin.h

appwin.o:	appwin.h colors.h tablewin.h

colors.o:	colors.h

tablewin.o:	tablewin.h appwin.h colors.h
