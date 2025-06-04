/*
 * mattrib.c
 * Change MSDOS file attribute flags
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "mainloop.h"

typedef struct Arg_t {
	char add;
	unsigned char remove;
	struct MainParam_t mp;
} Arg_t;

static int attrib_file(Stream_t *Dir, MainParam_t *mp, int entry)
{
	Arg_t *arg=(Arg_t *) mp->arg;

	mp->dir.attr = (mp->dir.attr & arg->remove) | arg->add;
	dir_write(Dir,entry,& mp->dir);
	return GOT_ONE;
}


static int view_attrib(Stream_t *Dir, MainParam_t *mp, int entry)
{
	printf("  ");
	if(mp->dir.attr & 0x20)
		putchar('A');
	else
		putchar(' ');
	fputs("  ",stdout);
	if(mp->dir.attr & 0x4)
		putchar('S');
	else
		putchar(' ');
	if(mp->dir.attr & 0x2)
		putchar('H');
	else
		putchar(' ');
	if(mp->dir.attr & 0x1)
		putchar('R');
	else
		putchar(' ');
	printf("     %c:%s", mp->drivename, mp->pathname);
	if(strlen(mp->pathname) != 1 )
		putchar('/');	
	puts(mp->outname);
	return GOT_ONE;
}

static void usage(void) NORETURN;
static void usage(void)
{
	fprintf(stderr, "Mtools version %s, dated %s\n", 
		mversion, mdate);
	fprintf(stderr, 
		"Usage: %s [-p] [-a|+a] [-h|+h] [-r|+r] [-s|+s] msdosfile [msdosfiles...]\n",
		progname);
	exit(1);
}

static int letterToCode(int letter)
{
	switch (toupper(letter)) {
		case 'A':
			return 0x20;
		case 'H':
			return 0x2;
		case 'R':
			return 0x1;
		case 'S':
			return 0x4;
		default:
			usage();
	}
}


void mattrib(int argc, char **argv, int type)
{
	Arg_t arg;
	int view;
	int c;
	char filename[VBUFSIZE];
	char pathname[MAX_PATH];
	char *ptr;

	arg.add = 0;
	arg.remove = 0xff;
	view = 0;

	while ((c = getopt(argc, argv, "ahrsAHRS")) != EOF) {
		switch (c) {
			default:
				arg.remove &= ~letterToCode(c);
				break;
			case '?':
				usage();
		}
	}

	for(;optind < argc;optind++) {
		switch(argv[optind][0]) {
			case '+':
				for(ptr = argv[optind] + 1; *ptr; ptr++)
					arg.add |= letterToCode(*ptr);
				continue;
			case '-':
				for(ptr = argv[optind] + 1; *ptr; ptr++)
					arg.remove &= ~letterToCode(*ptr);
				continue;
		}
		break;
	}

	if(arg.remove == 0xff && !arg.add)
		view = 1;

	if (optind >= argc)
		usage();

	init_mp(&arg.mp);
	if(view){
		arg.mp.callback = view_attrib;
		arg.mp.openflags = O_RDONLY;
	} else {
		arg.mp.callback = attrib_file;
		arg.mp.openflags = O_RDWR;
	}

	arg.mp.outname = filename;
	arg.mp.arg = (void *) &arg;
	arg.mp.lookupflags = ACCEPT_PLAIN | ACCEPT_DIR;
	arg.mp.pathname = pathname;
	exit(main_loop(&arg.mp, argv + optind, argc - optind));
}
