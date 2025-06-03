#
# Makefile for cpus.
# 
# You should change the INCLUDES line to point to the directory
# where your X include files can be found.
#
CC = gcc
INCLUDES = -I/usr/X/include/X11
LIBS = -lXaw -lXmu -lXext -lXt -lX11 -lnsl
CFLAGS = -g -O3 $(INCLUDES)

cpus : cpus.o
	$(CC) -o cpus cpus.o $(LIBS)

install: cpus cpus.1
	cp cpus /opt/bin
	nroff -man cpus.1 > /opt/man/cat1/cpus.1
	cp Cpus.ad /usr/X/lib/app-defaults/Cpus
	cp cpus.sh /opt/bin/cpu

clean:
	rm -f cpus.cat cpus *.o *~