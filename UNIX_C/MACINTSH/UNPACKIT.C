31-Mar-86 22:42:01-PST,10912;000000000001
Return-Path: <werner@ngp.UTEXAS.EDU>
Received: from ngp.UTEXAS.EDU by SUMEX-AIM.ARPA with TCP; Mon 31 Mar 86 22:41:52-PST
Date: Tue, 1 Apr 86 00:22:55 cst
From: werner@ngp.UTEXAS.EDU (Werner Uhrig)
Posted-Date: Tue, 1 Apr 86 00:22:55 cst
Message-Id: <8604010622.AA04381@ngp.UTEXAS.EDU>
Received: by ngp.UTEXAS.EDU (4.22/4.22)
	id AA04381; Tue, 1 Apr 86 00:22:55 cst
To: info-mac-request@sumex-aim
Subject: from USENET: unpit2

[The Postman Strikes Twice .... (-: ---Werner]
From weber@brand.UUCP  Wed Mar 26 10:52:28 1986
Relay-Version: version B 2.10.2 9/18/84; site ut-ngp.UUCP
Path: ut-ngp!ut-sally!seismo!lll-crg!lll-lcc!qantel!hplabs!sdcrdcf!oberon!brand!weber
From: weber@brand.UUCP (Allan G. Weber)
Newsgroups: net.sources.mac
Subject: Unpit - Packit file unpacking program
Message-ID: <194@brand.UUCP>
Date: 26 Mar 86 16:52:28 GMT
Date-Received: 28 Mar 86 13:22:23 GMT
Distribution: na
Organization: U. of So. Calif., Los Angeles
Lines: 425

This is a Unix program for unpacking Mac Packit files into the component
files.  See message in net.micro.mac for more info.

					Allan Weber
					Arpa: Weber%Brand@USC-ECL
					Usenet: ...sdcrdcf!oberon!brand!weber

/*

		unpit - Macintosh PackIt file unpacker

		Version 2, for PackIt II

This program will unpack a Macintosh PackIt file into separate files.  The
data fork of a PackIt file contains both the data and resource forks of the
packed files.  The program will unpack each Mac file into separate .data,
.rsrc., and .info files that can be downloaded to a Mac using macput.

The program syntax is much like macput/macget:

	unpit [-rdu] packit-file.data

The  -r and -d flags will cause only the resource and data forks to be
written.  The -u flag will cause only the data fork to be written and
to have carriage return characters changed to Unix newline characters.

Some of the program is borrowed from the macput.c/macget.c programs.

	Author: Allan G. Weber, (Weber%Brand@USC-ECL)		
	Date:   September 30, 1985
	Revised: January 24, 1986 - added CRC checking
		 March 25, 1986 - support compressed mode of PackIt II,
				  check for illegal Unix file names

*/

/* There is some confusion as to what to do with the "inited" flag in the
   finder info bytes that are in the header of each file in the packit file.
   If this flag bit is copied to the .info file, it seems to confuse
   MacTerminal into placing the file icons in the upper left corner of the
   window on top of each other.  Setting this bit to zero in the .info file
   seems to fix that problem but may cause others.  I haven't been able to
   find any .info files that have this flag set so making it zero may be OK.
   Anyway, MacTerminal seems to set the flag when it create the file on the
   Mac.  The "#define INITED_BUG" can be used to try both settings.  */

/*
Format of a Packit file:

Repeat the following sequence for each file in the Packit file:

	4 byte identifier ("PMag" = not compressed, "Pma4" = compressed)
	variable length compression data (if compressed file)
	92 byte header (see struct pit_header below) *
	2 bytes CRC number *
	data fork (length from header) *
	resource fork (length from header) *
	2 bytes CRC number *

Last file is followed by the 4 byte Ascii string, "Pend", and then the EOF.

* these are in compressed form if compression is on for the file

*/

#define DEBUG

#include <stdio.h>

typedef char byte;
typedef short word;

struct pit_header {	/* Packit file header (92 bytes)
	byte nlen;	/* number of characters in packed file name */
	byte name[63];	/* name of packed file */
	byte type[4];	/* file type */
	byte auth[4];	/* file creator */
	word flags;	/* file flags (?) */
	word lock;	/* unknown */
	long dlen;	/* number of bytes in data fork */
	long rlen;	/* number of bytes in resource fork */
	long ctim;	/* file creation time */
	long mtim;	/* file modified time */
};

#define HDRBYTES  92
#define INFOBYTES 128

#define BYTEMASK 0xff

#define H_NAMELEN 63

#define H_NLENOFF 0
#define H_NAMEOFF 1
#define H_TYPEOFF 64
#define H_AUTHOFF 68
#define	H_LOCKOFF 70
#define H_FLAGOFF 72
#define H_DLENOFF 76
#define H_RLENOFF 80
#define H_CTIMOFF 84
#define H_MTIMOFF 88

#define I_NAMELEN 69	/* H_NAMELEN + strlen(".info") + 1 */

/* The following are copied out of macput.c/macget.c */
#define I_NLENOFF 1
#define I_NAMEOFF 2
/* 65 <-> 80 is the FInfo structure */
#define I_TYPEOFF 65
#define I_AUTHOFF 69
#define I_FLAGOFF 73
#define I_LOCKOFF 81
#define I_DLENOFF 83
#define I_RLENOFF 87
#define I_CTIMOFF 91
#define I_MTIMOFF 95

#define INITED_BUG
#define INITED_OFF	I_FLAGOFF	/* offset to byte with Inited flag */
#define INITED_MASK	(~1)		/* mask to '&' with byte to reset it */

#define TEXT 0
#define DATA 1
#define RSRC 2
#define FULL 3

struct node {
	int flag, byte;
	struct node *one, *zero;
} nodelist[512], *nodeptr, *read_tree();	/* 512 should be big enough */

char f_info[I_NAMELEN];
char f_data[I_NAMELEN];
char f_rsrc[I_NAMELEN];

char hdr[HDRBYTES];
char info[INFOBYTES];
char text[H_NAMELEN+1];

int mode, txtmode;
int datalen, rsrclen;
int decode, bit;

char usage[] = "usage: unpit [-rdu] filename\n";

main(ac, av)
int ac;
char **av;
{
	char temp[4], *name;
	int crc, data_crc;

	mode = FULL;
	name = "";
	ac--; av++;
	while (ac) {
		if (av[0][0] == '-') {
			switch (av[0][1]) {
			case 'r':
				mode = RSRC;
				break;
			case 'd':
				mode = DATA;
				break;
			case 'u':
				mode = TEXT;
				break;
			case '\0':
				name = av[0];
				break;
			default:
				fprintf(stderr, usage);
				exit(1);
			}
		}
		else {
			name = av[0];
		}
		ac--; av++;
	}

	if (strlen(name) == 0) {
		fprintf(stderr, usage);
		exit(1);
	}
	else {
		if (freopen(name,"r",stdin) == NULL) {
			fprintf(stderr,"Can't open input file \"%s\"\n",name);
			exit(1);
		}
	}

	while (1) {
		fread(temp, 1, 4, stdin);
		if (strncmp(temp, "PMag", 4) == 0 ||
		    strncmp(temp, "PMa4", 4) == 0) {
			if (temp[3] == '4') {
				nodeptr = nodelist;
				read_tree();
				decode = 1;
			}
			else
				decode = 0;
			data_crc = read_hdr();
			crc = getcrc();
			if (crc != data_crc) {
				fprintf(stderr, "File header CRC mismatch\n");
				exit(1);
			}
			txtmode = (mode == TEXT);
			data_crc = write_file(f_data, datalen, 0);
			txtmode = 0;
			data_crc = write_file(f_rsrc, rsrclen, data_crc);
			crc = getcrc();
			if (crc != data_crc) {
				fprintf(stderr,
					"File data/rsrc CRC mismatch\n");
				exit(1);
			}
			decode = 0;
			bit = 0;	/* flush unused bits */
		}
		else if (strncmp(temp, "PEnd", 4) == 0)
			break;
		else {
			fprintf(stderr, "Unrecognized Packit format\n");
			exit(1);
		}
	}
}

/* This routine recursively reads the compression decoding data.
   It appears to be Huffman compression. */
struct node *read_tree()
{
	struct node *np;
	np = nodeptr++;
	if (getbit() == 1) {
		np->flag = 1;
		np->byte = getbyte();
	}
	else {
		np->flag = 0;
		np->zero = read_tree();
		np->one  = read_tree();
	}
	return(np);
}

read_hdr()
{
	long get4();
	register int i, n, crc;
	FILE *fp;
	char *np;

	for (n = 0; n < INFOBYTES; n++)
		info[n] = '\0';
	for (n = 0; n < HDRBYTES; n++)
		hdr[n] = getbyte();
	crc = 0;
	for (n = 0; n < HDRBYTES; n++) {
		crc = crc ^ ((int)hdr[n] << 8);
		for (i = 0; i < 8; i++)
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc <<= 1;
	}

	n = hdr[H_NLENOFF] & BYTEMASK;
	if (n > H_NAMELEN)
		n = H_NAMELEN;
	info[I_NLENOFF] = n;
	copy(info + I_NAMEOFF, hdr + H_NAMEOFF, n);
	strncpy(text, hdr + H_NAMEOFF, n);
	text[n] = '\0';
	datalen = get4(hdr + H_DLENOFF);
	rsrclen = get4(hdr + H_RLENOFF);
#ifdef DEBUG
	printf("name=\"%s\", type=%4.4s, author=%4.4s, data=%ld, rsrc=%ld\n",
	text, hdr + H_TYPEOFF, hdr + H_AUTHOFF, datalen, rsrclen);
#endif
	/* check for illegal Unix characters in file name */
	for (np = text; *np; np++)
		if (*np <= ' ' || *np == '/' || *np > '~')
			*np = '_';

	if (mode == FULL) {
		sprintf(f_info, "%s.info", text);
		sprintf(f_data, "%s.data", text);
		sprintf(f_rsrc, "%s.rsrc", text);

		copy(info + I_TYPEOFF, hdr + H_TYPEOFF, 4);
		copy(info + I_AUTHOFF, hdr + H_AUTHOFF, 4);
		copy(info + I_FLAGOFF, hdr + H_FLAGOFF, 2);
#ifdef INITED_BUG
		info[INITED_OFF] &= INITED_MASK;	/* reset init bit */
#endif
		copy(info + I_LOCKOFF, hdr + H_LOCKOFF, 2);
		copy(info + I_DLENOFF, hdr + H_DLENOFF, 4);
		copy(info + I_RLENOFF, hdr + H_RLENOFF, 4);
		copy(info + I_CTIMOFF, hdr + H_CTIMOFF, 4);
		copy(info + I_MTIMOFF, hdr + H_MTIMOFF, 4);

		fp = fopen(f_info, "w");
		if (fp == NULL) {
			perror(f_info);
			exit(1);
		}
		fwrite(info, 1, INFOBYTES, fp);
		fclose(fp);
	}
	else if (mode == RSRC) {
			sprintf(f_data, "/dev/null");
			sprintf(f_rsrc, "%s.rsrc", text);
	}
	else {
			sprintf(f_data, "%s", text);
			sprintf(f_rsrc, "/dev/null");
	}
	return(crc & 0xffff);
}

write_file(fname, bytes, crc)
char *fname;
long bytes;
register int crc;
{
	register int b, i;
	FILE *outf;

	outf = fopen(fname, "w");
	if (outf == NULL) {
		perror(fname);
		exit(1);
	}
	while (bytes-- > 0) {
		b = getbyte();
		crc = crc ^ (b << 8);
		for (i = 0; i < 8; i++)
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc <<= 1;
		if (txtmode && (b & BYTEMASK) == '\r')
			b = '\n';
		putc(b, outf);
	}
	fclose(outf);
	return(crc & 0xffff);
}

long
get4(bp)
char *bp;
{
	register int i;
	long value = 0;

	for (i = 0; i < 4; i++) {
		value <<= 8;
		value |= (*bp & BYTEMASK);
		bp++;
	}
	return(value);
}

getcrc()
{
	int value;
	value = getbyte() & BYTEMASK;
	return( value << 8 | (getbyte() & BYTEMASK) );
}

copy(p1, p2, n)
char *p1, *p2;
int n;
{
	while (n-- > 0)
		*p1++ = *p2++;
}


/* This routine returns the next bit in the input stream (MSB first) */
getbit()
{
	static char b;
	if (bit == 0) {
		b = getchar() & 0xff;
		bit = 8;
	}
	bit--;
	return((b >> bit) & 1);
}

/* This routine returns the next 8 bits.  If decoding is on, it finds the
byte in the decoding tree based on the bits from the input stream.  If
decoding is not on, it either gets it directly from the input stream or
puts it together from 8 calls to getbit(), depending on whether or not we
are currently on a byte boundary
*/
getbyte()
{
	register struct node *np;
	register int i, b;
	if (decode) {
		np = nodelist;
		while (np->flag == 0)
			np = (getbit()) ? np->one : np->zero;
		b = np->byte;
	}
	else {
		if (bit == 0)	/* on byte boundary? */
			b = getc(stdin) & 0xff;
		else {		/* no, put a byte together */
			b = 0;
			for (i = 8; i > 0; i--) {
				b = (b << 1) + getbit();
			}
		}
	}
	return(b);
}


