all:
	rayshade < dna.script > dna.rle
	convert +alpha dna.rle miff:dna
	/bin/mv dna dna.0
	mogrify -verbose -colors 256 -dither dna.[0-9]*

clean:
	/bin/rm -f dna.[0-9]* dna.rle dna.log
