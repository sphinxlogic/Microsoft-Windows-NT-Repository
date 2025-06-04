/*
 * mmd.c
 * Makes an MSDOS directory
 */


#define LOWERCASE

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "vfat.h"
#include "mainloop.h"
#include "plain_io.h"
#include "nameclash.h"
#include "file.h"
#include "fs.h"

/*
 * Preserve the file modification times after the fclose()
 */

typedef struct Arg_t {
	char *target;
	int nowarn;
	int interactive;
	int verbose;
	int silent;
	MainParam_t mp;

	Stream_t *SrcDir;
	int entry;
	ClashHandling_t ch;
	Stream_t *targetDir;
} Arg_t;


/*
 * Open the named file for read, create the cluster chain, return the
 * directory structure or NULL on error.
 */
static struct directory *makeeit(char *dosname,
				 char *longname,
				 void *arg0,
				 struct directory *dir)
{
	Stream_t *Target;
	time_t now;
	Arg_t *arg = (Arg_t *) arg0;
	int fat;

	/* will it fit? At least one sector must be free */
	if (! getfree(arg->targetDir)) {
		fprintf(stderr,"Disk full\n");
		return NULL;
	}
	
	/* find out current time */
	time(&now);
	mk_entry(".", 0x10, 1, 0, now, dir);
	Target = open_file(arg->targetDir, dir);	
	if(!Target){
		fprintf(stderr,"Could not open Target\n");
		return NULL;
	}

	dir_grow(Target, 0);
	/* this allocates the first cluster for our directory */

	GET_DATA(arg->targetDir, 0, 0, 0, &fat);
	mk_entry("..         ", 0x10, fat, 0, now, dir);
	dir_write(Target, 1, dir);

	GET_DATA(Target, 0, 0, 0, &fat);
	mk_entry(".          ", 0x10, fat, 0, now, dir);
	dir_write(Target, 0, dir);

	mk_entry(dosname, 0x10, fat, 0, now, dir);
	FREE(&Target);
	return dir;
}


static void usage(void)
{
	fprintf(stderr,
		"Mtools version %s, dated %s\n", mversion, mdate);
	fprintf(stderr,
		"Usage: %s [-itnmvV] file targetfile\n", progname);
	fprintf(stderr,
		"       %s [-itnmvV] file [files...] target_directory\n", 
		progname);
	exit(1);
}

void mmd(int argc, char **argv, int type)
{
	int ret = 0;
	Stream_t *Dir;
	Arg_t arg;
	int c;
	char filename[VBUFSIZE];
	int i;

	/* get command line options */

	init_clash_handling(& arg.ch);

	/* get command line options */
	arg.nowarn = 0;
	arg.interactive = 0;
	arg.silent = 0;
	arg.verbose = 0;
	while ((c = getopt(argc, argv, "XinvorsamORSAM")) != EOF) {
		switch (c) {
			case 'i':
				arg.interactive = 1;
				break;
			case 'n':
				arg.nowarn = 1;
				break;
			case 'v':
				arg.verbose = 1;
				break;
			case 'X':
				arg.silent = 1;
				break;
			case '?':
				usage();
			default:
				if(handle_clash_options(&arg.ch, c))
					usage();
				break;
		}
	}

	if (argc - optind < 1)
		usage();

	init_mp(&arg.mp);
	arg.mp.arg = (void *) &arg;
	arg.mp.openflags = O_RDWR;
	ret = 0;

	for(i=optind; i < argc; i++){
		if(got_signal)
			break;
		Dir = open_subdir(&arg.mp, argv[i], O_RDWR, 0, 0);
		if(!Dir){
			fprintf(stderr,"Could not open parent directory\n");
			exit(1);
		}
		arg.targetDir = Dir;
		get_name(argv[i], filename, arg.mp.mcwd);
		if(!filename[0]){
			if(!arg.silent)
				fprintf(stderr,"Empty filename\n");
			ret |= MISSED_ONE;
			FREE(&Dir);
			continue;
		}
		if(mwrite_one(Dir, filename,0,
			      makeeit, (void *)&arg, &arg.ch)<=0){
			if(!arg.silent)
				fprintf(stderr,"Could not create %s\n", 
					argv[i]);
			ret |= MISSED_ONE;
		} else
			ret |= GOT_ONE;
		FREE(&Dir);
	}

	if ((ret & GOT_ONE) && ( ret & MISSED_ONE))
		exit(2);
	if (ret & MISSED_ONE)
		exit(1);
	exit(0);
}
