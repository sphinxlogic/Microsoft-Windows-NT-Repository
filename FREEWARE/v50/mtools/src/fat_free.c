#include "sysincludes.h"
#include "msdos.h"
#include "fsP.h"

/*
 * Remove a string of FAT entries (delete the file).  The argument is
 * the beginning of the string.  Does not consider the file length, so
 * if FAT is corrupted, watch out!
 */

int fat_free(Stream_t *Dir, unsigned int fat)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);
	unsigned int next_no_step;
					/* a zero length file? */
	if (fat == 0)
		return(0);

	/* CONSTCOND */
	while (1) {
					/* get next cluster number */
		next_no_step = This->fat_decode(This,fat);
		/* mark current cluster as empty */
		if (This->fat_encode(This,fat, 0) || next_no_step == 1) {
			fprintf(stderr, "fat_free: FAT problem %d %d\n",
				fat,next_no_step);
			This->fat_error++;
			return(-1);
		}
		if (next_no_step >= This->last_fat)
			break;
		fat = next_no_step;
	}
	return(0);
}

int fatFreeWithDir(Stream_t *Dir, struct directory *dir)
{
	unsigned int first;

	if((!strncmp(dir->name,".      ",8) ||
	    !strncmp(dir->name,"..     ",8)) &&
	   !strncmp(dir->ext,"   ",3)) {
		fprintf(stderr,"Trying to remove . or .. entry\n");
		return -1;
	}

	first = START(dir);
  	if(fat32RootCluster(Dir))
		first |= STARTHI(dir) << 16;
	return fat_free(Dir, first);
}
