CFLAGS = -O2 -Wall
LDFLAGS = -s -N

all: Suits.xpm Ranks.xpm

Suits.xpm: suit
	./suit

Ranks.xpm: rank
	./rank

suit: rank
	cp rank suit

rank: rank.c largerank.bm largediamond.bm largespade.bm largeclub.bm largeheart.bm
	$(CC) -o rank $(CFLAGS) $(LDFLAGS) rank.c -lXpm -lX11

combine: combine.c
	$(CC) -O2 -c -Wall combine.c
	$(CC) -s -N -o combine combine.o -lXpm -lXaw -lXmu -lXt -lX11

clean:
	rm -f suit rank *.o *~ *.xpm.gz *.xpm