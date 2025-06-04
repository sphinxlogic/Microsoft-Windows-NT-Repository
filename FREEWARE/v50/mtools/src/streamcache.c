/*
 * streamcache.c
 * Managing a cache of open disks
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "vfat.h"
#include "fs.h"
#include "mainloop.h"
#include "plain_io.h"

static int is_initialized = 0;
static Stream_t *Subdir;	
static char subdir_name[VBUFSIZE];
static Stream_t *fss[256]; /* open drives */
static char last_drive; /* last opened drive */	


static void finish_sc(void)
{
	int i;

	FREE(&Subdir);
	for(i=0; i<256; i++){
		if(fss[i] && fss[i]->refs != 1 )
			fprintf(stderr,"Streamcache allocation problem:%c %d\n",
				i, fss[i]->refs);
		FREE(&(fss[i]));
	}
}



static void init_streamcache(void)
{
	int i;

	if(is_initialized)
		return;
	is_initialized = 1;
	last_drive = '\0';
	for(i=0; i<256; i++)
		fss[i]=0;
	Subdir= NULL;
	subdir_name[0]='\0';
	atexit(finish_sc);
}


Stream_t *open_subdir(MainParam_t *mp, const char *arg, 
		      int flags,int mode, int lock)
{
	int drive;
	Stream_t *Fs;
	char pathname[MAX_PATH];

	init_streamcache();

	mp->drivename = drive = get_drive(arg, *(mp->mcwd));
	
	/* open the drive */
	if(fss[drive])
		Fs = fss[drive];
	else {
		Fs = fs_init(drive, flags);
		if (!Fs){
			fprintf(stderr, "Cannot initialize '%c:'\n", drive);
			return NULL;
		}

		fss[drive] = Fs;
	}

	get_name(arg, mp->filename, mp->mcwd);
	get_path(arg, pathname, mp->mcwd, mode);
	if(mp->pathname)
		strcpy(mp->pathname, pathname);

	if (last_drive != drive || 
	    strcasecmp(pathname,subdir_name)){
		FREE(&Subdir);
		
		Subdir = subdir(Fs, pathname, lock);
		last_drive = drive;
		strcpy(subdir_name, pathname);
	}
	return COPY(Subdir);
}
