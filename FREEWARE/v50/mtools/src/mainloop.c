/*
 * mainloop.c
 * Iterating over all the command line parameters, and matching patterns
 * where needed
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "vfat.h"
#include "fs.h"
#include "mainloop.h"
#include "plain_io.h"
#include "file.h"

static int unix_loop(MainParam_t *mp, char *arg)
{
	char *tmp;
	int ret;
	int isdir;

	mp->File = NULL;
	if((mp->lookupflags & DO_OPEN)){
		mp->File = SimpleFileOpen(0, 0, arg, O_RDONLY, 0, 0);

		if(!mp->File){

			perror(arg);
#if 0
			tmp = strrchr(arg,'/');
			if(tmp)
				tmp++;
			else
				tmp = arg;
			strncpy(mp->filename, tmp, VBUFSIZE);
			mp->filename[VBUFSIZE-1] = '\0';
#endif
			return MISSED_ONE;
		}
		GET_DATA(mp->File, 0, 0, &isdir, 0);
		if(isdir)
			return IS_MATCH;
	}

	if(mp->outname){
		tmp = strrchr(arg,'/');
		if(tmp)
			tmp++;
		else
			tmp = arg;
		strncpy(mp->outname, tmp, VBUFSIZE-1);
		mp->outname[VBUFSIZE-1]='\0';
	}
	ret = mp->unixcallback(arg,mp) | IS_MATCH;
	FREE(&mp->File);
	return ret;
}

static int checkForDot(MainParam_t *mp, const char *name)
{
	if((mp->lookupflags & NO_DOTS) &&
	   (!name[0] ||
	    !strcmp(name,".") || 
	    !strcmp(name,"..")))
		return 1;
	return 0;
}
		
		
static int dos_loop(MainParam_t *mp, char *arg)
{
	Stream_t *Dir;
	int ret;
	int have_one;
	int entry;
	const char *filename;

	have_one = MISSED_ONE;
	ret = 0;

	Dir =  open_subdir(mp, arg, mp->openflags, 0, 0);
	if(!Dir)
		return MISSED_ONE;
	entry = 0;

	filename = mp->filename;
	if(!filename[0])
		filename="*";
	if(checkForDot(mp, filename)){
		fprintf(stderr,
			"This command cannot be applied to \".\" or \"..\"\n");
		FREE(&Dir);
		return MISSED_ONE;
	}

	while(entry >= 0){
		if(got_signal)
			break;
		mp->File = NULL;
		if(vfat_lookup(Dir,
			       & (mp->dir), &entry, 0,
			       filename,
			       mp->lookupflags,
			       mp->outname,
			       mp->shortname,
			       mp->longname) == 0 ){
			if(checkForDot(mp,mp->outname))
				ret |= MISSED_ONE;
			else {
				if(mp->lookupflags & DO_OPEN)
					mp->File = open_file(Dir, &mp->dir);
				if(got_signal)
					break;
				have_one = IS_MATCH;
				ret |= mp->callback(Dir, mp, entry - 1);
				FREE(&mp->File);
			}
		}		
		if (fat_error(Dir))
			ret |= MISSED_ONE;
		if(ret & NEXT_DISK)
			break;
		if(mp->fast_quit && (ret & MISSED_ONE))
			break;
		
	}
	FREE(&Dir);
	return ret | have_one;
}

int main_loop(MainParam_t *mp, char **argv, int argc)
{
	int i;
	int ret, Bret;
	
	Bret = 0;

	for (i = 0; i < argc; i++) {
		if ( got_signal )
			break;
		if (mp->unixcallback && (!argv[i][0] || argv[i][1] != ':' ))
			ret = unix_loop(mp, argv[i]);
		else if (mp->newdoscallback)
			ret = mp->newdoscallback(argv[i], mp) ;
		else
			ret = dos_loop(mp, argv[i]);
		
		if (! (ret & IS_MATCH) ) {
			fprintf(stderr, "%s: File \"%s\" not found\n",
				progname, argv[i]);
			ret |= MISSED_ONE;
		}
		Bret |= ret;
		if(mp->fast_quit && (Bret & MISSED_ONE))
			break;
	}
	if ((Bret & GOT_ONE) && ( Bret & MISSED_ONE))
		return 2;
	if (Bret & MISSED_ONE)
		return 1;
	return 0;
}


void init_mp(MainParam_t *mp)
{
	fix_mcwd(mp->mcwd);

	mp->unixcallback = NULL;
	mp->newdoscallback = NULL;
	mp->pathname = mp->outname = mp->shortname = mp->longname = 0;
	mp->newdrive_cb = mp->olddrive_cb = 0;
	mp->fast_quit = 0;
}
