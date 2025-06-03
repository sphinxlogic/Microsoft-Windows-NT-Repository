# Makefile modified by Greg Lehey, LEMIS, 16 May 1993

XLIBS   = -lXaw -lXmu -lXt -lXext -lX11 -lnsl

OBJECTS = pegbd.o xui.o xpeg.o strdup.o strtrim.o

DEBUG  = -O3 -g
CC	= gcc
CFLAGS = $(DEBUG) -Idynobj

default : 
	@echo "		make xpeg     - makes xpeg"
	@echo "		make dist     - makes an xpeg distribution"
	@echo "		make clean    - cleans up xpeg objects etc..."
	@echo "		make allclean - cleans up xpeg and dynobj..."
	@echo ""
	@echo "To test after making: xpeg sample_pegfile"
	@echo ""
xpeg : $(OBJECTS) dynobj/libdyn.a
	$(CC) $(CFLAGS) -o xpeg $(OBJECTS) dynobj/libdyn.a $(XLIBS)

dynobj/libdyn.a : 
	(cd dynobj; make)


dist :
	cp *.c ../xpeg1.0/
	cp *.h ../xpeg1.0/
	cp Makefile ../xpeg1.0/
	cp Xpeg.ad  ../xpeg1.0/
	cp README   ../xpeg1.0/
	cp COPYRIGHT ../xpeg1.0/
	cp sample_pegfile ../xpeg1.0/
	cp xpeg.icon ../xpeg1.0/
	cp Todo ../xpeg1.0/


install:
	cp xpeg /opt/bin
	cat README > /opt/man/cat1/xpeg.1

clean : 
	-rm *.o xpeg

allclean : clean
	(cd dynobj; make clean)
