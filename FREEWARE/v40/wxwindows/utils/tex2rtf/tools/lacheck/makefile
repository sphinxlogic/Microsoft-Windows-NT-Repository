BINDIR = $(DESTDIR)/usr/local/tex82/bin
MANDIR = $(DESTDIR)/usr/local/tex82/man

#CC = gcc -traditional
CFLAGS = -O # -DNEED_STRSTR
LEX = flex -8

lacheck:	lacheck.o
	$(CC) $(CFLAGS) -o $@ lacheck.o -ll

lacheck.c:	lacheck.lex
	$(LEX) lacheck.lex
	mv lex.yy.c $@

install:	lacheck
	install -c -s lacheck $(BINDIR)
	install -c lacheck.1 $(MANDIR)/man1

clean:
	rm -f lacheck lacheck.c *.o
