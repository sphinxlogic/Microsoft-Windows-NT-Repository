/*
 * mdel.c
 * Delete an MSDOS file
 *
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "stream.h"
#include "mainloop.h"
#include "fs.h"
#include "file.h"

typedef struct Arg_t {
	int deltype;
	int verbose;
} Arg_t;

static int del_entry(Stream_t *Dir, MainParam_t *mp, int entry)
{
	Arg_t *arg=(Arg_t *) mp->arg;

	if (arg->verbose)
		fprintf(stderr,"Removing %s\n", mp->outname);
	if(got_signal)
		return MISSED_ONE;

	if ((mp->dir.attr & 0x05) &&
	    (ask_confirmation("%s: \"%s\" is read only, erase anyway (y/n) ? ",
			      progname,
			      mp->outname)))
		return MISSED_ONE;
	
	if (fatFreeWithDir(Dir,&mp->dir))
		return MISSED_ONE;
	mp->dir.name[0] = (char) DELMARK;
	dir_write(Dir,entry,& mp->dir);
	return GOT_ONE;
}

static int del_file(Stream_t *Dir, MainParam_t *mp, int entry)
{
	char shortname[13];
	int sub_entry = 0;
	Stream_t *SubDir;
	Arg_t *arg = (Arg_t *) mp->arg;
	MainParam_t sonmp;
	char outname[VBUFSIZE];
	int ret;

	sonmp = *mp;
	sonmp.outname = outname;
	sonmp.arg = mp->arg;

	if (mp->dir.attr & 0x10){
		/* a directory */
		SubDir = open_file(GetFs(Dir), & mp->dir);

		ret = 0;
		while(!vfat_lookup(SubDir, &sonmp.dir, &sub_entry, 
				   0, "*",
				   ACCEPT_DIR | ACCEPT_PLAIN,
				   outname, shortname, NULL) ){
			if(shortname[0] != DELMARK &&
			   shortname[0] &&
			   shortname[0] != '.' ){
				if(arg->deltype != 2){
					fprintf(stderr,
						"Directory %s non empty\n", 
						mp->outname);
					ret = MISSED_ONE;
					break;
				}
				if(got_signal) {
					ret = MISSED_ONE;
					break;
				}
				ret = del_file(SubDir, &sonmp, sub_entry - 1);
				if( ret & MISSED_ONE)
					break;
				ret = 0;
			}
		}
		FREE(&SubDir);
		if(ret)
			return ret;
	}
	return del_entry(Dir, mp, entry);
}


static void usage(void)
{
	fprintf(stderr, 
		"Mtools version %s, dated %s\n", mversion, mdate);
	fprintf(stderr, 
		"Usage: %s [-v] msdosfile [msdosfiles...]", progname);
	exit(1);
}

void mdel(int argc, char **argv, int deltype)
{
	int verbose=0;
	Arg_t arg;
	MainParam_t mp;
	char filename[VBUFSIZE];
	int c,i;

	arg.verbose = 0;

	while ((c = getopt(argc, argv, "v")) != EOF) {
		switch (c) {
			case 'v':
				arg.verbose = 1;
				break;
			default:
				usage();
		}
	}

	if(argc == optind)
		usage();

	init_mp(&mp);
	mp.callback = del_file;
	mp.outname = filename;
	mp.arg = (void *) &arg;
	mp.openflags = O_RDWR;
	arg.deltype = deltype;
	switch(deltype){
	case 0:
		mp.lookupflags = ACCEPT_PLAIN; /* mdel */
		break;
	case 1:
		mp.lookupflags = ACCEPT_DIR; /* mrd */
		break;
	case 2:
		mp.lookupflags = ACCEPT_DIR | ACCEPT_PLAIN; /* mdeltree */
		break;
	}
	mp.lookupflags |= NO_DOTS;
	for(i=optind;i<argc;i++) {
		int b,l;
		if(argv[i][0] && argv[i][1] == ':')
			b = 2;
		else
			b = 0;
		l = strlen(argv[i]+b);
		if(l > 1 && argv[i][b+l-1] == '/')
			argv[i][b+l-1] = '\0';
	}
		
	exit(main_loop(&mp, argv + optind, argc - optind));
}
