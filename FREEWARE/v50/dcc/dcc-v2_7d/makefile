#Makefile for dcc

CC = gcc -w
CCNAME = `echo $(CC) | sed -e 's/ .*//g'`
CFLAGS = -g
#CFLAGS = -O6 -mpentium
#CFLAGS = -O2 -m486
OBJS = dcblk.o dcdecl.o dcdir.o dcexp.o dcext.o dcfmt.o dcinst.o dcmain.o \
       dcprag.o dcrec.o dcrecdir.o dctxttok.o
BASICINCLFILES = dc.th configdc.th locdcFil.mng


dcc : dccFiles.mng.orig DoAlways $(OBJS)
	$(CC) $(CFLAGS) -o dcc $(OBJS) 
	chmod +x createlocalexec tstdcc
	./createlocalexec

dccFiles.mng.orig: dccFiles.mng
#Done only after downloading
	chmod +x chooseMsgLanguage
	chmod +x adaptStarterFile chooseStarterFile createLocalFilesMngFile
	cp dccFiles.mng dccFiles.mng.orig

DoAlways:
	./chooseMsgLanguage
	./chooseStarterFile
	./createLocalFilesMngFile

dcblk.o: dcblk.c dcblk.h dcblk.ph dcdecl.h dcdir.ph dcext.h dcmain.h dcprag.h \
         dcrec.h dcrec.ph $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcblk.c

dcdecl.o: dcdecl.c dcdecl.h dcblk.h dcdir.h dcexp.h dcext.h dcinst.h dcmain.h \
          dcprag.h dcrec.h dctxttok.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcdecl.c

dcdir.o: dcdir.c dcdir.h dcdir.ph dcblk.h dcblk.ph dcdecl.h dcexp.h dcext.h \
         dcmain.h dcmain.ph dcprag.h dcprag.ph dcrec.h dcrec.ph dcrecdir.ph \
         dcrecdir.th $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcdir.c

dcexp.o: dcexp.c dcexp.h dcexp.ph dcblk.h dcdecl.h dcdir.h dcext.h dcfmt.h \
         dcinst.h dcprag.h dcrec.h dctxttok.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcexp.c

dcext.o: dcext.c dcmsg.txt dcext.h dcext.ph dcdecl.h dcinst.h dcmain.h \
         dcprag.h dcrec.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcext.c

dcfmt.o: dcfmt.c dcfmt.h dcblk.h dcext.h dcrec.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcfmt.c

dcinst.o: dcinst.c dcinst.h dcblk.h dcdecl.h dcdir.h dcexp.h dcexp.ph dcext.h \
          dcrec.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcinst.c

dcmain.o: dcmain.c dcmain.h dcmain.ph dcblk.h dcblk.ph dcdecl.h dcdir.h \
          dcdir.ph dcext.h dcext.ph dcprag.ph dcrec.h dcrec.ph dcrecdir.ph \
          dcrecdir.th $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcmain.c

dcprag.o: dcprag.c dcprag.h dcprag.ph dcblk.h dcdecl.h dcdir.h dcexp.h dcext.h \
          dcmain.h dcrec.h dcrec.ph dcrecdir.ph dcrecdir.th $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcprag.c

dcrec.o: dcrec.c dcrec.h dcrec.ph dcblk.h dcblk.ph dcdecl.h dcdir.ph dcexp.h \
         dcext.h dcmain.h dcmain.ph dcprag.ph dcrecdir.ph dcrecdir.th \
         $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcrec.c

dcrecdir.o: dcrecdir.c dcrecdir.ph dcrecdir.th dcmain.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dcrecdir.c

dctxttok.o: dctxttok.c dctxttok.h $(BASICINCLFILES)
	$(CC) $(CFLAGS) -c dctxttok.c

clean : 
	rm $(OBJS) *~

install:
	chmod +x installfile
	./installfile

#End dcc makefile
