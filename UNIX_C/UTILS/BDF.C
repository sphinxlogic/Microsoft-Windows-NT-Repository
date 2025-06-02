Article 303 of comp.sources.misc:
Path: brl-smoke!brl-adm!cmcl2!husc6!necntc!ncoast!allbery
From: paul@vixie.UUCP (Paul Vixie Esq)
Newsgroups: comp.sources.misc
Subject: v01i014: A Berkeley-style DF fSystem V
Date: 26 Jan 88 03:48:30 GMT
Approved: allbery@ncoast.UUCP
X-Archive: comp.sources.misc/8801/15
Comment: Original subject made less likely to embarass archivists
Comp.sources.misc: Volume 2, Issue 15
Submitted-By: Paul Vixie Esq <paul@vixie.UUCP>
Archive-Name: bdf

Comp.sources.misc: Volume 2, Issue 15
Submitted-By: Paul Vixie Esq <paul@vixie.UUCP>
Archive-Name: bdf

[Someone remind me to post "space" -- better than Berzerk df, and doesn't
 have to parse someone else's grotty output.  ++bsa]

/* bdf - berkeley DF for system-V systems
 * vix 16jan88 [written - from scratch]
 *
 * This code is public-domain, but please leave this notice intact and give me
 * appropriate credit if you write a man page for it.  If you send me bug fixes
 * and enhancements, I will release new versions from time to time.
 *	Paul Vixie, paul%vixie@uunet.uu.net
 *
 * This is not nearly good enough for the obfuscated C contest.  Next time?
 *
 * Known to work on INTERACTIVE 386/ix, and should therefore work perfectly
 * on Microport/386 and whatever the AT&T 6386 calls its OS.  Should work okay
 * on 3B's.  Should work, in fact, anywhere where the output of 'df -t' is
 * the same as 'System V/386', which may or may not be all SysV.[23] systems.
 */

#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

static char *PROGNAME = "bdf";
static void usage() {
	fprintf(stderr, "usage:  %s [-i]\n", PROGNAME);
	exit(4);
}

main(argc, argv)
	int argc;
	char *argv[];
{
	void bdf();
	int iflag = FALSE;
	int optind = argc;
	char command_buf[300], *ptr = command_buf;

	if (!(PROGNAME = strrchr(argv[0], '/'))) PROGNAME = argv[0];
	if (argc > 1)
		if (argv[1][0] == '-')
			if (argv[1][1] == 'i') { iflag = TRUE; optind = 2; }
			else usage();
		else optind = 1;

	ptr += sprintf(ptr, "/bin/df -t");
	for (;  optind < argc;  optind++)
		ptr += sprintf(ptr, " %s", argv[optind]);

	bdf(iflag, command_buf);
}

static void
bdf(iflag, df_command)
	int iflag;
	char *df_command;
{
	void output_part1(), output_part2(), output_part3();
	void header_part1(), header_part2(), header_part3();
	char filesys[50], device[50];
	int fblocks, finodes, tblocks, tinodes;
	FILE *df;

	if (!(df = popen(df_command, "r"))) {
		fprintf(stderr, "error executing <%s> command\n", df_command);
		perror("popen");
		exit(2);
	}

	header_part1();
	if (iflag) header_part2();
	header_part3();

	while (EOF != fscanf(df, "%s (%s ): %d blocks %d i-nodes",
				filesys, device, &fblocks, &finodes)) {
		if (EOF == fscanf(df, " total: %d blocks %d i-nodes",
				&tblocks, &tinodes)) {
			perror("fscanf#2");
			exit(2);
		}

		output_part1(device, fblocks/2, tblocks/2);
		if (iflag)
			output_part2(finodes, tinodes);
		output_part3(filesys);
	}
}

/*************
Filesystem    kbytes    used   avail capacity iused   ifree  %iused  Mounted on
/dev/dsk/0s1 xxxxxxx xxxxxxx xxxxxxx   xxx%  xxxxxx  xxxxxx   xxx%   /foo/bar
*************/

static void
header_part1() {
	printf("Filesystem    kbytes    used   avail capacity");
}
static void
header_part2() {
	printf(" iused   ifree  %%iused");
}
static void
header_part3() {
	printf("  Mounted on\n");
}

static void
output_part1(device, free_kb, total_kb)
	char *device;
	int free_kb, total_kb;
{
	int used_kb = total_kb - free_kb;
	int capacity = (100 * used_kb) / total_kb;

	printf("%12s %7d %7d %7d   %3d%%  ",
		device, total_kb, used_kb, free_kb, capacity);
}

static void
output_part2(free_inodes, total_inodes)
	int free_inodes, total_inodes;
{
	int used_inodes = total_inodes - free_inodes;
	int percent_used = (100 * used_inodes) / total_inodes;

	printf("%6d  %6d   %3d%% ", used_inodes, free_inodes, percent_used);
}

static void
output_part3(filesys)
	char *filesys;
{
	printf("  %s\n", filesys);
}

-- 
Paul A Vixie Esq
paul%vixie@uunet.uu.net
{uunet,ptsfa,hoptoad}!vixie!paul
San Francisco, (415) 647-7023


