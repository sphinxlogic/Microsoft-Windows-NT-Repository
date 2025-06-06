 7-Sep-85 22:05:04-MDT,3181;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 7 Sep 85 22:04:57-MDT
Received: from usenet by TGR.BRL.ARPA id a001898; 7 Sep 85 16:07 EDT
From: Fred Toth #7252 <fpt@wgivax.uucp>
Newsgroups: net.sources
Subject: disappearing disk space (V7)
Message-ID: <126@wgivax.UUCP>
Date: 6 Sep 85 19:04:06 GMT
To:       unix-sources@BRL-TGR.ARPA

/*
 * This posting is for those interested in the 'disappearing disk space'
 * discussion in net.unix-wizards.
 *
 * Fred Toth
 * Washburn Graphics, Inc.
 * Charlotte, NC
 * decvax!mcnc!unccvax!wgivax!fpt
 *
 * scani.c
 *
 * This program scans a v7 file system, looking for inodes
 * that have more blocks allocated to them then they need
 * based on the size field.
 *
 * It reads it's standard input and reports inode numbers.
 * Ncheck can then be used to find the files.
 * Copying them and deleting the original will free the blocks.
 */
#include <stdio.h>
#include <sys/param.h>
#include <sys/filsys.h>
#include <sys/ino.h>

struct filsys sb;

struct dinode mynod;

daddr_t addr[13];	/* store converted disk addresses */

long ci, cii, ciii;

main()
{
	int i;
	unsigned toti, in;
	long has, need, countb(), extra, totex, totb;
	int files;

	files = 0;
	totb = totex = 0L;
	fseek(stdin, 512L, 0);
	fread(&sb, sizeof(struct filsys), 1, stdin);
	toti = (sb.s_isize - 2) * 8;
	printf("%u inodes\n", toti);
	fseek(stdin, 1024L, 0);
	in = 0;
	while (toti--) {
		in++;
		fseek(stdin, (((long) in - 1L) * 64L) + 1024L, 0);
		if (fread(&mynod, sizeof(struct dinode), 1, stdin) == NULL)
			break;
		if (mynod.di_mode) {
			files++;
			l3tol(addr, &mynod.di_addr[0], 13);
			need = (mynod.di_size + 511) / 512;
			if ((has = countb()) > need) {
				extra = has - need;
				totex += extra;
				printf("i %u, needs %D, has %D, %D extra\n", in, need, has, extra);
			}
			totb += has;
		}
	}
	printf("Did %u inodes, %d files\n", in, files);
	printf("%D extra allocated blocks\n", totex);
	printf("%D data, %D i, %D, ii, %D iii\n", totb, ci, cii, ciii);
}
long
countb()
{
	int i;
	long count, cindir(), cdoub(), ctrip();

	count = 0;
	for (i = 0; i < 10; i++)
		if (addr[i])
			count++;
	if (addr[10])
		count += cindir(addr[10]);
	if (addr[11])
		count += cdoub(addr[11]);
	if (addr[12])
		count += ctrip(addr[12]);
	return(count);
}
long
cindir(blk)
daddr_t blk;
{
	long count;
	daddr_t ib[128];
	int i;

	count = 0;
	getblk(ib, blk);
	ci++;
	for (i = 0; i < 128; i++)
		if (ib[i])
			count++;
	return(count);
}
getblk(p, no)
char *p;
daddr_t no;
{
	fseek(stdin, no * 512, 0);
	if (fread(p, 512, 1, stdin) <= 0)
		printf("bad read, b %D\n", no);
}
long
cdoub(blk)
daddr_t blk;
{
	daddr_t dib[128];
	long count;
	int i;

	count = 0;
	getblk(dib, blk);
	cii++;
	for (i = 0; i < 128; i++)
		if (dib[i])
			count += cindir(dib[i]);
	return(count);
}
long
ctrip(blk)
daddr_t blk;
{
	daddr_t tib[128];
	long count;
	int i;

	count = 0;
	getblk(tib, blk);
	ciii++;
	for (i = 0; i < 128; i++)
		if (tib[i])
			count += cdoub(tib[i]);
	return(count);
}
