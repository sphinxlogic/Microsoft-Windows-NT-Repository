#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "vfat.h"
#include "file.h"
#include "buffer.h"

/*
 * Find the directory and load a new dir_chain[].  A null directory
 * is OK.  Returns a 1 on error.
 */


static void bufferize(Stream_t **Dir)
{
	Stream_t *BDir;

#if 1    
	if(!*Dir)
		return;
	BDir = buf_init(*Dir, 16384, MDIR_SIZE, MDIR_SIZE);
	if(!BDir){
		FREE(Dir);
		*Dir = NULL;
	} else
		*Dir = BDir;
#endif
}
	
	
Stream_t *descend(Stream_t *Dir, char *path, int barf,char *outname, int lock)
{
	/* this function makes its own copy of the Next pointers */
	int entry;
	struct directory dir;
	Stream_t *SubDir;
	int ret;

	if(path[0] == '\0' || !strcmp(path, "."))
		/* don't waste timing scanning through the directory if
		 * we look for dot anyways */
		return COPY(Dir);

	entry = 0;
	ret = vfat_lookup(Dir, &dir, &entry, 0, path,
			  ACCEPT_DIR | SINGLE | DO_OPEN | (barf ? 0 : NO_MSG),
			  outname, 0, 0);

	if(ret == 0){
		SubDir = open_file(Dir, &dir);
		if(lock)
			LockFile(SubDir);
		bufferize(&SubDir);
		return SubDir;
	}

	/*
	 * If path is '..', but it wasn't found, then you must be
	 * at root.
	 */
	if (!strcmp(path, "..")){
		SubDir = open_root(Dir);
		bufferize(&SubDir);
		if(lock)
			LockFile(SubDir);
		return SubDir;
	}
	if (barf)
		fprintf(stderr, "Path component \"%s\" is not a directory\n",
			path);
	return NULL;
}


/*
 * Descends the directory tree.  Returns 1 on error.  Attempts to optimize by
 * remembering the last path it parsed
 */
Stream_t *subdir(Stream_t *Fs, char *pathname, int lock)
{
	/* this function makes its own copy of the Next pointers */

	char *s, *tmp, tbuf[MAX_PATH], *path, terminator;
	Stream_t *Dir, *NewDir;

	/* FIXME: path caching */

	strcpy(tbuf, pathname);

	/* start at root */
	Dir = open_root(Fs);
	bufferize(&Dir);
	if (!Dir){
		FREE(&Fs);
		return Dir;
	}

	/* separate the parts */
	tmp = &tbuf[1];
	for (s = tmp; ; ++s) {
		if (*s == '/' || *s == '\0') {
			path = tmp;
			terminator = *s;
			*s = '\0';
			if (s != tmp && strcmp(path,".") && path[0]){
				NewDir = descend(Dir, path, 1, 0, lock);
				FREE(&Dir);
				if(!NewDir)
					return NewDir;
				Dir = NewDir;
			}
			if (!terminator)
				break;
			tmp = s + 1;
		}
	}
	return Dir;
}
