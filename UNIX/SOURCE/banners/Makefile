#
# simple Makefile for banners collection
#

CC = gcc

CFLAGS = -O3 -g

# Banner 10 removed: we don't have a Pascal compiler (Greg Lehey, LEMIS, 9 September 1993)
BANNERS = bin/banner-01 bin/banner-02 bin/banner-03 bin/banner-04 \
          bin/banner-05 bin/banner-06 bin/banner-07 bin/banner-08 \
          bin/banner-09 bin/banner-12 \
          bin/banner-13 bin/cursive-01 bin/cursive-02 bin/gothic

all: bin $(BANNERS)
	@echo
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "+        Now look into the bin directory for the executables and           +"
	@echo "+        select the banner(s) you like most (and maybe install             +"
	@echo "+        them in a place like /usr/local/bin).                             +"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo

bin:
	mkdir bin

bin/banner-01: bin
	$(CC) -c $(CFLAGS) banner-01/banner.c -o bin/banner-01-1.o
	$(CC) -c $(CFLAGS) banner-01/font.c -o bin/banner-01-2.o
	$(CC) $(CFLAGS) bin/banner-01-1.o bin/banner-01-2.o -o bin/banner-01
	rm -f bin/banner-01-1.o bin/banner-01-2.o

bin/banner-02: bin
	$(CC) $(CFLAGS) banner-02/banner.c -o bin/banner-02

bin/banner-03: bin
	$(CC) $(CFLAGS) banner-03/banr.c -o bin/banner-03 -ltermcap

bin/banner-04: bin
	$(CC) $(CFLAGS) banner-04/sysvbanner.c -o bin/banner-04

bin/banner-05: bin
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/block.c -o bin/banner-05-1 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/cursive.c -o bin/banner-05-2 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/kban.c -o bin/banner-05-3 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/lban.c -o bin/banner-05-4 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/sban.c -o bin/banner-05-5 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/vban.c -o bin/banner-05-6 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/3db.c -o bin/banner-05-7 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/leb.c -o bin/banner-05-8 -L/usr/ucblib -lucb
	$(CC) $(CFLAGS) -Ibanner-05 banner-05/seb.c -o bin/banner-05-9 -L/usr/ucblib -lucb
	@touch bin/banner-05

bin/banner-06: bin
	$(CC) $(CFLAGS) banner-06/banner.c -o bin/banner-06

bin/banner-07: bin
	$(CC) $(CFLAGS) banner-07/banner.c -o bin/banner-07

bin/banner-08: bin
	$(CC) $(CFLAGS) banner-08/banner.c -o bin/banner-08

bin/banner-09: bin
	$(CC) -c $(CFLAGS) banner-09/block.c -o bin/banner-09-1.o
	$(CC) -c $(CFLAGS) banner-09/data.c -o bin/banner-09-2.o
	$(CC) $(CFLAGS) bin/banner-09-1.o bin/banner-09-2.o -o bin/banner-09
	rm -f bin/banner-09-1.o bin/banner-09-2.o

bin/banner-10: bin
	@echo "*** Please use your favourite pascal compiler to make banner-10. ***"
	@touch bin/banner-10

bin/banner-11: bin
	@cp banner-11/MB_Font.uu bin/MB_Font.uu
	@cd bin ; uudecode MB_Font.uu ; rm -f MB_Font.uu ; mv MB_Font banner-11.font
	$(CC) $(CFLAGS) banner-11/mb.c -o bin/banner-11
	@echo "*** Usage of banner-11: bin/banner-11 -o=bin/banner-11.font. ***"
	@echo "Or alternatively copy bin/banner-11.font to /etc/MB_Font."

bin/banner-12: bin
	$(CC) -c $(CFLAGS) -Ibanner-12 banner-12/main.c -o bin/banner-12-1.o
	$(CC) -c $(CFLAGS) -Ibanner-12 banner-12/banner.c -o bin/banner-12-2.o
	$(CC) -c $(CFLAGS) -Ibanner-12 banner-12/font.c -o bin/banner-12-3.o
	$(CC) $(CFLAGS) bin/banner-12-1.o bin/banner-12-2.o bin/banner-12-3.o -o bin/banner-12
	rm -f bin/banner-12-1.o bin/banner-12-2.o bin/banner-12-3.o

bin/banner-13: bin
	$(CC) $(CFLAGS) banner-13/cyrban.c -o bin/banner-13

bin/cursive-01: bin
	$(CC) -c $(CFLAGS) -Icursive-01 cursive-01/cursive.c -o bin/cursive-01-1.o
	$(CC) -c $(CFLAGS) -Icursive-01 cursive-01/font.c -o bin/cursive-01-2.o
	$(CC) $(CFLAGS) bin/cursive-01-1.o bin/cursive-01-2.o -o bin/cursive-01
	rm -f bin/cursive-01-1.o bin/cursive-01-2.o

bin/cursive-02: bin
	$(CC) -c $(CFLAGS) -Icursive-02 cursive-02/cursive.c -o bin/cursive-02-1.o
	$(CC) -c $(CFLAGS) -Icursive-02 cursive-02/font.c -o bin/cursive-02-2.o
	$(CC) $(CFLAGS) bin/cursive-02-1.o bin/cursive-02-2.o -o bin/cursive-02
	rm -f bin/cursive-02-1.o bin/cursive-02-2.o

bin/gothic: bin
	$(CC) $(CFLAGS) gothic/gothic.c -o bin/gothic

install:
	cp bin/* /opt/bin
	cp README /opt/man/cat6/banner.1

clean:
	rm bin/*
