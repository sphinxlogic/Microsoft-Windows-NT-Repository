/*
 * You will have to change these
 */
HOME		=/usr/X
BINDIR		=${HOME}/bin
MANDIR		=/opt/man
RESOURCES	=${HOME}/lib/X11/app-defaults

/*
 * Where do X includes and libraries live?
 */
INCROOT		=/usr/local/X11R5/include
XLIBDIR		=-L${XHOME}/lib

CDEBUGFLAGS=-O

/***********************************************************************/


#define XawClientDepLibs
#define XawClientLibs $(XLIBDIR) -lXaw -lXt -lXmu -lXext -lX11

SYS_LIBRARIES = $(XLIBDIR) -lXaw -lXt -lXmu -lXext -lX11 -lm
SYSLIBS	= $(SYS_LIBRARIES) 

CFLAGS=$(DEFINES) $(CDEBUGFLAGS) $(INCLUDES) $(SYS_INCLUDES)

#define YES 1
#define NO 0

#if defined(mips) || defined(ultrix)
#  define UltrixArchitecture
#endif
