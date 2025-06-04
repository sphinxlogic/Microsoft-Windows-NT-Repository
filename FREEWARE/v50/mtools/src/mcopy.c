/*
 * mcopy.c
 * Copy an MSDOS files to and from Unix
 *
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

static void set_mtime(const char *target, long mtime)
{
#ifdef HAVE_UTIMES
	struct timeval tv[2];

	if (mtime != 0L) {
		tv[0].tv_sec = mtime;
		tv[0].tv_usec = 0;
		tv[1].tv_sec = mtime;
		tv[1].tv_usec = 0;
		utimes(target, tv);
	}
#else
#ifdef HAVE_UTIME
	struct utimbuf utbuf;

	if (mtime != 0L) {
		utbuf.actime = mtime;
		utbuf.modtime = mtime;
		utime(target, &utbuf);
	}
#endif
#endif
	return;
}

typedef struct Arg_t {
	int preserve;
	char *target;
	int textmode;
	int needfilter;
	int nowarn;
	int single;
	int verbose;
	int type;
	MainParam_t mp;
	ClashHandling_t ch;
	Stream_t *sourcefile;
	Stream_t *targetDir;
} Arg_t;

static int read_file(Stream_t *Dir, MainParam_t *mp, int entry, 
		     int needfilter)
{
	Arg_t *arg=(Arg_t *) mp->arg;
	long mtime;
	Stream_t *File=mp->File;
	Stream_t *Target, *Source;
	struct stat stbuf;
	int ret;
	char errmsg[80];

	File->Class->get_data(File, &mtime, 0, 0, 0);
	
	if (!arg->preserve)
		mtime = 0L;

	/* if we are creating a file, check whether it already exists */
	if (!arg->nowarn && arg->target && strcmp(arg->target, "-") && 
	    !access(arg->target, 0)){
		if( ask_confirmation("File \"%s\" exists, overwrite (y/n) ? ",
				     arg->target,0))
			return MISSED_ONE;
		
		/* sanity checking */
		if (!stat(arg->target, &stbuf) && !S_ISREG(stbuf.st_mode)) {
			fprintf(stderr,"\"%s\" is not a regular file\n",
				arg->target);			
			return MISSED_ONE;
		}
	}

	if(!arg->type)
		fprintf(stderr,"Copying %s\n", mp->outname);
	if(got_signal)
		return MISSED_ONE;
	if ((Target = SimpleFileOpen(0, 0, arg->target,
				     O_WRONLY | O_CREAT | O_TRUNC,
				     errmsg, 0))) {
		ret = 0;
		if(needfilter && arg->textmode){
			Source = open_filter(COPY(File));
			if (!Source)
				ret = -1;
		} else
			Source = COPY(File);

		if (ret == 0 )
			ret = copyfile(Source, Target);
		FREE(&Source);
		FREE(&Target);
		if(ret < -1){
			unlink(arg->target);
			return MISSED_ONE;
		}
		if(arg->target && strcmp(arg->target,"-"))
			set_mtime(arg->target, mtime);
		return GOT_ONE;
	} else {
		fprintf(stderr,"%s\n", errmsg);
		return MISSED_ONE;
	}
}

static  int dos_read(Stream_t *Dir, MainParam_t *mp, int entry)
{
	return read_file(Dir, mp, entry, 1);
}


static  int unix_read(char *name, MainParam_t *mp)
{
	return read_file(0, mp, -1, 0);
}


/*
 * Open the named file for read, create the cluster chain, return the
 * directory structure or NULL on error.
 */
static struct directory *writeit(char *dosname,
				 char *longname,
				 void *arg0,
				 struct directory *dir)
{
	Stream_t *Target;
	time_t now;
	int type, fat, ret;
	long date;
	size_t filesize, newsize;
	Arg_t *arg = (Arg_t *) arg0;

	if (arg->mp.File->Class->get_data(arg->mp.File,
					  & date, &filesize, &type, 0) < 0 ){
		fprintf(stderr, "Can't stat source file\n");
		return NULL;
	}

	if (type){
		if (arg->verbose)
			fprintf(stderr, "\"%s\" is a directory\n", longname);
		return NULL;
	}

	if (!arg->single)
		fprintf(stderr,"Copying %s\n", longname);
	if(got_signal)
		return NULL;

	/* will it fit? */
	if (!getfreeMin(arg->targetDir, filesize)) {
		fprintf(stderr,"Disk full\n");
		return NULL;
	}
	
	/* preserve mod time? */
	if (arg->preserve)
		now = date;
	else
		time(&now);

	mk_entry(dosname, 0x20, 0, 0, now, dir);
	Target = open_file(arg->targetDir, dir);
	if(!Target){
		fprintf(stderr,"Could not open Target\n");
		exit(1);
	}
	if (arg->needfilter & arg->textmode)
		Target = open_filter(Target);
	ret = copyfile(arg->mp.File, Target);
	GET_DATA(Target, 0, &newsize, 0, &fat);
	FREE(&Target);
	if(ret < 0 ){
		fat_free(arg->targetDir, fat);
		return NULL;
	} else {
		mk_entry(dosname, 0x20, fat, newsize, now, dir);
		return dir;
	}
}


static int write_file(Stream_t *Dir, MainParam_t *mp, int entry,
		      int needfilter)
/* write a messy dos file to another messy dos file */
{
	int result;
	Arg_t * arg = (Arg_t *) (mp->arg);

	arg->needfilter = needfilter;
	if (arg->targetDir == Dir){
		arg->ch.ignore_entry = -1;
		arg->ch.source = entry;
	} else {
		arg->ch.ignore_entry = -1;
		arg->ch.source = -2;
	}
	result = mwrite_one(arg->targetDir, arg->target, 0,
			    writeit, (void *)arg, &arg->ch);
	if(result == 1)
		return GOT_ONE;
	else
		return MISSED_ONE;
}


static int dos_write(Stream_t *Dir, MainParam_t *mp, int entry)
{
	return write_file(Dir, mp, entry, 0);
}

static int unix_write(char *name, MainParam_t *mp)
/* write a Unix file to a messy DOS fs */
{
	return write_file(0, mp, 0, 1);
}


static void usage(void)
{
	fprintf(stderr,
		"Mtools version %s, dated %s\n", mversion, mdate);
	fprintf(stderr,
		"Usage: %s [-tnmvV] sourcefile targetfile\n", progname);
	fprintf(stderr,
		"       %s [-tnmvV] sourcefile [sourcefiles...] targetdirectory\n", 
		progname);
	exit(1);
}

void mcopy(int argc, char **argv, int mtype)
{
	Stream_t *SubDir,*Dir;
	int have_target;
	Arg_t arg;
	int c, ret, fastquit;
	char filename[VBUFSIZE];
	char spareOutname[VBUFSIZE];
	
	struct stat stbuf;

	/* get command line options */

	init_clash_handling(& arg.ch);

	/* get command line options */
	arg.preserve = 0;
	arg.nowarn = 0;
	arg.textmode = 0;
	arg.verbose = 0;
	arg.type = mtype;
	fastquit = 0;
	while ((c = getopt(argc, argv, "tnmvorsamQORSAM")) != EOF) {
		switch (c) {
			case 't':
				arg.textmode = 1;
				break;
			case 'n':
				arg.nowarn = 1;
				break;
			case 'm':
				arg.preserve = 1;
				break;
			case 'v':	/* dummy option for mcopy */
				arg.verbose = 1;
				break;
			case 'Q':
				fastquit = 1;
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

	/* only 1 file to copy... */
	arg.single = SINGLE;
	
	init_mp(&arg.mp);
	arg.mp.fast_quit = fastquit;
	arg.mp.arg = (void *) &arg;
	arg.mp.openflags = O_RDONLY;

	arg.targetDir = NULL;
	if(mtype){

		/* Mtype = copying to stdout */

		have_target = 0;
		arg.target = 0;
		arg.mp.outname = filename;
		arg.single = 0;		
		arg.mp.callback = dos_read;
		arg.mp.unixcallback = unix_read;
	} else if(argc - optind >= 2 && 
		  argv[argc-1][0] && argv[argc-1][1] == ':'){

		/* Copying to Dos */
		
		have_target = 1;
		Dir = open_subdir(&arg.mp, argv[argc-1], O_RDWR, 0, 0);
		if(!Dir){
			fprintf(stderr,"Bad target\n");
			exit(1);
		}
		get_name(argv[argc-1], filename, arg.mp.mcwd);
		SubDir = descend(Dir, filename, 0, 0, 0);
		if (!SubDir){
			/* the last parameter is a file */
			if (argc - optind != 2){
				fprintf(stderr,
					"%s: Too many arguments, or destination directory omitted\n",
					argv[0]);
				FREE(&Dir);
				exit(1);
			}
			arg.targetDir = Dir;
			arg.mp.outname = 0; /* toss away source name */
			arg.target = filename; /* store near name given as
						* target */
			arg.single = 1;
		} else {
			arg.targetDir = SubDir;
			FREE(&Dir);
			arg.mp.outname = filename;
			arg.target = filename;
			arg.single = 0;
		}
		arg.mp.callback = dos_write;
		arg.mp.unixcallback = unix_write;
	} else {
		/* Copying to Unix */
		have_target = 1;
		if (argc - optind == 1){
			/* one argument: copying to current directory */
			arg.single = 0;
			have_target = 0;
			arg.target = filename;
			arg.mp.outname = filename;
		} else if (strcmp(argv[argc-1],"-") && 
			   !stat(argv[argc-1], &stbuf) &&
			   S_ISDIR(stbuf.st_mode)){
			char *tmp;
			tmp = (char *) safe_malloc(VBUFSIZE + 1 + 
						   strlen(argv[argc-1]));
			strcpy(tmp, argv[argc-1]);
			strcat(tmp,"/");
			arg.mp.outname = tmp+strlen(tmp);
			arg.target = tmp;
		} else if (argc - optind != 2) {
			/* last argument is not a directory: we should only
			 * have one source file */
			fprintf(stderr,
				"%s: Too many arguments or destination directory omitted\n", 
				argv[0]);
			exit(1);
		} else {
			arg.target = argv[argc-1];
			arg.mp.outname = filename;
		}

		arg.mp.callback = dos_read;
		arg.mp.unixcallback = unix_read;
	}
	if(!arg.mp.outname)
		arg.mp.outname = spareOutname;

	arg.mp.lookupflags = ACCEPT_PLAIN | DO_OPEN | NO_DOTS | arg.single;
	ret=main_loop(&arg.mp, argv + optind, argc - optind - have_target);
	FREE(&arg.targetDir);

	exit(ret);
}
