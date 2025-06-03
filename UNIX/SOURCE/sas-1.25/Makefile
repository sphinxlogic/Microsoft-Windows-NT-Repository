
#
# define SVR4 on system V r 4+
# Define SVR42 on SVR4.2
#
# Mods by Greg Lehey, LEMIS, 22 January 1994

CC	= gcc
CFLAGS	= -g -O3
DEFINES = -DSVR4 -DINKERNEL -D_KERNEL

Driver.o: sas.c sys/sas.h
	${CC} ${CFLAGS} ${DEFINES} -I. -c sas.c
	mv sas.o Driver.o

install: Driver.o
	cp sys/sas.h /usr/include/sys
	/etc/conf/bin/idinstall -d -k sas
	/etc/conf/bin/idinstall -a -k sas

clean:
	rm -f *~ *.o
