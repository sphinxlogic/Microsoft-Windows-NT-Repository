#
#   Makefile for ZYZGAUGE ('Gas Gauge Thingamajig') custom control.
#
#     (C) Copyright Microsoft Corp. 1991-1992.  All rights reserved.
#
#
#   to make a NON-DEBUG build, type the following line:
#       nmake DEBUG=NO
#   just typing 'nmake' will build a DEBUG build
#


NAME    =   testzyzg
MISC    =   makefile

INCS    =   $(NAME).h  
SRCS    =   $(NAME).c  
OBJ1    =   $(NAME).obj zyzgauge.obj zyz3d.obj
OBJ2    =
OBJ3    =
OBJS    =   $(OBJ1) $(OBJ2) $(OBJ3)
LIBS    =   libw mlibcew


!IF "$(DEBUG)" == "NO"

AS  =   masm -Mx
CC  =   cl -c -DRELEASE -AM -G2sw -Oxwt -W3 -Zpe
RC  =   rc -v -DRELEASE
LN  =   link /NOE/NOD/A:16/MAP

!ELSE

AS  =   masm -Mx -Zi
CC  =   cl -c -DDEBUG -AM -G2w -Oxwt -W3 -Zipe
RC  =   rc -v -DDEBUG
LN  =   link /NOD/NOE/A:16/MAP/CO

!ENDIF


.asm.obj:
            $(AS) $*;

.c.obj  :
            $(CC) -NT $* $*.c


#
#   RULES
#

all :   zyzgauge.lib $(NAME).exe


$(NAME).exe ::  $(OBJS) $(NAME).res $(NAME).def
                $(LN) @<<
                $(OBJ1)+
                $(OBJ2)+
                $(OBJ3),
                $(NAME).exe,
                $(NAME).map,
                $(LIBS),
                $(NAME).def
<<
                $(RC) -t $(NAME).res
		-mapsym $(NAME).map

$(NAME).exe ::  $(NAME).res
                $(RC) -t $(NAME).res

$(NAME).res :   $(NAME).rc $(NAME).h $(NAME).ico
                $(RC) -r $(NAME).rc

$(NAME).obj :   $(NAME).c $(NAME).h

zyzgauge.obj:   zyzgauge.c zyzgauge.h zyz3d.h

zyz3d.obj   :   zyz3d.c zyz3d.h

zyzgauge.lib:   $(OBJS)
                del zyzgauge.lib
                lib zyzgauge.lib +zyzgauge.obj zyz3d.obj ;


#
#   MISC. STUFF
#


clean   :
	    del *.lib
            del *.err
            del *.res
            del *.obj
            del *.map
            del *.sym
