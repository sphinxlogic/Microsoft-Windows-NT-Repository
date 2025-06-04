#ifndef MTOOLS_MAINLOOP_H
#define MTOOLS_MAINLOOP_H

//#include <sys/param.h>
#include "vfat.h"

typedef struct MainParam_t {
	/* stuff needing to be initialised by the caller */
	int (*callback)(Stream_t *Dir, struct MainParam_t *mp, int entry);
	int (*unixcallback)(char *name, struct MainParam_t *mp);
	int (*newdoscallback)(char *name, struct MainParam_t *mp);
	int (*newdrive_cb)(Stream_t *Dir, struct MainParam_t *mp);
	int (*olddrive_cb)(Stream_t *Dir, struct MainParam_t *mp);
	void *arg; /* to be passed to callback */

       	int openflags; /* flags used to open disk */
	int lookupflags; /* flags used to lookup up using vfat_lookup */
	int fast_quit; /* for commands manipulating multiple files, quit
			* as soon as even _one_ file has a problem */

	char *outname; /* where to put the matched file name */
	char *shortname; /* where to put the short name of the matched file */
	char *longname; /* where to put the long name of the matched file */
	char *pathname; /* path name of file */
	char filename[VBUFSIZE];
	char drivename;

	/* out parameter */
	Stream_t *File;
	struct directory dir;

	/* internal data */
	char mcwd[MAXPATHLEN];	
} MainParam_t;

void init_mp(MainParam_t *MainParam);
int main_loop(MainParam_t *MainParam, char **argv, int argc);

Stream_t *open_subdir(MainParam_t *MainParam, const char *arg, 
		      int flags, int mode, int lock);


#define NEXT_DISK 1
#define MISSED_ONE 2
#define GOT_ONE 4
#define IS_MATCH 8

#endif
