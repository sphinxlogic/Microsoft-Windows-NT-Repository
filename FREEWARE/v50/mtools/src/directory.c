#include "sysincludes.h"
#include "msdos.h"
#include "stream.h"
#include "mtools.h"
#include "file.h"
#include "vfat.h"
#include "fs.h"

/* #define DEBUG */

/*
 * Read a directory entry into caller supplied buffer
 */
struct directory *dir_read(Stream_t *Dir,
			   struct directory *dir, 
			   int num,
			   struct vfat_state *v)
{
	if(force_read(Dir, (char *) dir, num * MDIR_SIZE, MDIR_SIZE) !=
	   MDIR_SIZE)
		return NULL;

	if (v && (dir->attr == 0x0f) && dir->name[0] != DELMARK) {
		struct vfat_subentry *vse;
		unsigned char id, last_flag;
		char *c;

		vse = (struct vfat_subentry *) dir;

		id = vse->id & VSE_MASK;
		last_flag = (vse->id & VSE_LAST);
		if (id > MAX_VFAT_SUBENTRIES) {
			fprintf(stderr, "dir_read: invalid VSE ID %d at %d.\n",
				id, num);
			return dir;
		}

/* 950819: This code enforced finding the VSEs in order.  Well, Win95
 * likes to write them in *reverse* order for some bizarre reason!  So
 * we pretty much have to tolerate them coming in any possible order.
 * So skip this check, we'll do without it (What does this do, Alain?).
 *
 * 950820: Totally rearranged code to tolerate any order but to warn if
 * they are not in reverse order like Win95 uses.
 *
 * 950909: Tolerate any order. We recognize new chains by mismatching
 * checksums. In the event that the checksums match, new entries silently
 * overwrite old entries of the same id. This should accept all valid
 * entries, but may fail to reject invalid entries in some rare cases.
 */

		/* bad checksum, begin new chain */
		if(v->sum != vse->sum) {
			clear_vfat(v);
			v->sum = vse->sum;
		}

#ifdef DEBUG
		if(v->status & (1 << (id-1)))
			fprintf(stderr,
				"dir_read: duplicate VSE %d\n", vse->id);
#endif
			
		v->status |= 1 << (id-1);
		if(last_flag)
			v->subentries = id;
			
#ifdef DEBUG
		if (id > v->subentries)
			/* simple test to detect entries preceding
			 * the "last" entry (really the first) */
			fprintf(stderr,
				"dir_read: new VSE %d sans LAST flag\n",
				vse->id);
#endif

		c = &(v->name[VSE_NAMELEN * (id-1)]);
		c += unicode_read(vse->text1, c, VSE1SIZE);
		c += unicode_read(vse->text2, c, VSE2SIZE);
		c += unicode_read(vse->text3, c, VSE3SIZE);
#ifdef DEBUG
		printf("Read VSE %d at %d, subentries=%d, = (%13s).\n",
		       id,num,v->subentries,&(v->name[VSE_NAMELEN * (id-1)]));
#endif		
		if (last_flag)
			*c = '\0';	/* Null terminate long name */
	}
	return dir;
}

/*
 * Make a subdirectory grow in length.  Only subdirectories (not root)
 * may grow.  Returns a 0 on success, 1 on failure (disk full), or -1
 * on error.
 */

int dir_grow(Stream_t *Dir, int size)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(FsPublic_t);
	int ret;
	int buflen;
	char *buffer;
	
	
	buflen = This->cluster_size * This->sector_size;

	if(! (buffer=malloc(buflen)) ){
		perror("dir_grow: malloc");
		return -1;
	}
		
	memset((char *) buffer, '\0', buflen);
	ret = force_write(Dir, buffer, size * MDIR_SIZE, buflen);
	if(ret < buflen)
		return -1;
	return 0;
}


void dir_write(Stream_t *Dir, int num, struct directory *dir)
{
	force_write(Dir, (char *) dir, num * MDIR_SIZE, MDIR_SIZE);
}


/*
 * Make a directory entry.  Builds a directory entry based on the
 * name, attribute, starting cluster number, and size.  Returns a pointer
 * to a static directory structure.
 */

struct directory *mk_entry(const char *filename, char attr,
			   unsigned int fat, size_t size, long date,
			   struct directory *ndir)
{
	struct tm *now;
	time_t date2 = date;
	unsigned char hour, min_hi, min_low, sec;
	unsigned char year, month_hi, month_low, day;

	now = localtime(&date2);
	strncpy(ndir->name, filename, 8);
	strncpy(ndir->ext, filename + 8, 3);
	ndir->attr = attr;
	ndir->ctime_ms = 0;
	hour = now->tm_hour << 3;
	min_hi = now->tm_min >> 3;
	min_low = now->tm_min << 5;
	sec = now->tm_sec / 2;
	ndir->ctime[1] = ndir->time[1] = hour + min_hi;
	ndir->ctime[0] = ndir->time[0] = min_low + sec;
	year = (now->tm_year - 80) << 1;
	month_hi = (now->tm_mon + 1) >> 3;
	month_low = (now->tm_mon + 1) << 5;
	day = now->tm_mday;
	ndir -> adate[1] = ndir->cdate[1] = ndir->date[1] = year + month_hi;
	ndir -> adate[0] = ndir->cdate[0] = ndir->date[0] = month_low + day;

	set_word(ndir->start, fat & 0xffff);
	set_word(ndir->startHi, fat >> 16);
	set_dword(ndir->size, size);
	return ndir;
}
