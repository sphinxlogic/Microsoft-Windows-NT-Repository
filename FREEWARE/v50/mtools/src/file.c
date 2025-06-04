#include "sysincludes.h"
#include "msdos.h"
#include "stream.h"
#include "mtools.h"
#include "fsP.h"
#include "file.h"
#include "htable.h"

typedef struct File_t {
	Class_t *Class;
	int refs;
	struct Fs_t *Fs;	/* Filesystem that this fat file belongs to */
	Stream_t *Buffer;

	int (*map)(struct File_t *this, off_t where, size_t *len, int mode);
	size_t FileSize;

	/* Absolute position of first cluster of file */
	unsigned int FirstAbsCluNr;

	/* Absolute position of previous cluster */
	unsigned int PreviousAbsCluNr;

	/* Relative position of previous cluster */
	unsigned int PreviousRelCluNr;
	struct directory dir;
	int locked;
	Stream_t *ParentDir;
} File_t;

static int normal_map(File_t *This, off_t where, size_t *len, int mode)
{
	int offset;
	int NrClu; /* number of clusters to read */
	unsigned int RelCluNr;
	unsigned int CurCluNr;
	unsigned int NewCluNr;
	unsigned int AbsCluNr;
	int clus_size;
	Fs_t *Fs = This->Fs;

	clus_size = Fs->cluster_size * Fs->sector_size;
	offset = where % clus_size;

	if (mode == MT_READ)
		maximize(len, This->FileSize - where);
	if (*len == 0 )
		return 0;

	if (This->FirstAbsCluNr < 2){
		if( mode == MT_READ || *len == 0){
			*len = 0;
			return 0;
		}
		NewCluNr = get_next_free_cluster(This->Fs, 1);
		if (NewCluNr == 1 ){
			errno = ENOSPC;
			return -2;
		}
		This->FirstAbsCluNr = NewCluNr;
		Fs->fat_encode(This->Fs, NewCluNr, Fs->end_fat);
	}

	RelCluNr = where / clus_size;
	
	if (RelCluNr >= This->PreviousRelCluNr){
		CurCluNr = This->PreviousRelCluNr;
		AbsCluNr = This->PreviousAbsCluNr;
	} else {
		CurCluNr = 0;
		AbsCluNr = This->FirstAbsCluNr;
	}


	NrClu = (offset + *len - 1) / clus_size;
	while (CurCluNr <= RelCluNr + NrClu){
		if (CurCluNr == RelCluNr){
			/* we have reached the beginning of our zone. Save
			 * coordinates */
			This->PreviousRelCluNr = RelCluNr;
			This->PreviousAbsCluNr = AbsCluNr;
		}
		NewCluNr = Fs->fat_decode(This->Fs, AbsCluNr);
		if (NewCluNr == 1 ){
			fprintf(stderr,"Fat problem while decoding %d\n", 
				AbsCluNr);
			exit(1);
		}
		if(CurCluNr == RelCluNr + NrClu)			
			break;
		if (NewCluNr > Fs->last_fat && mode == MT_WRITE){
			/* if at end, and writing, extend it */
			NewCluNr = get_next_free_cluster(This->Fs, AbsCluNr);
			if (NewCluNr == 1 ){ /* no more space */
				errno = ENOSPC;
				return -2;
			}
			Fs->fat_encode(This->Fs, AbsCluNr, NewCluNr);
			Fs->fat_encode(This->Fs, NewCluNr, Fs->end_fat);
		}

		if (CurCluNr < RelCluNr && NewCluNr > Fs->last_fat){
			*len = 0;
			return 0;
		}

		if (CurCluNr >= RelCluNr && NewCluNr != AbsCluNr + 1)
			break;
		CurCluNr++;
		AbsCluNr = NewCluNr;
	}

	maximize(len, (1 + CurCluNr - RelCluNr) * clus_size - offset);

	return ((This->PreviousAbsCluNr-2) * Fs->cluster_size+Fs->clus_start) *
			Fs->sector_size + offset;
}


static int root_map(File_t *This, off_t where, size_t *len, int mode)
{
	Fs_t *Fs = This->Fs;

	if(Fs->dir_len * Fs->sector_size < where) {
		*len = 0;
		errno = ENOSPC;
		return -2;
	}

	maximize(len, Fs->dir_len * Fs->sector_size - where);
	return Fs->dir_start * Fs->sector_size + where;
}
	

static int read_file(Stream_t *Stream, char *buf, off_t where, size_t len)
{
	DeclareThis(File_t);
	int pos;

	Stream_t *Disk = This->Fs->Next;
	
	pos = This->map(This, where, &len, MT_READ);
	if(pos < 0)
		return pos;
	return READS(Disk, buf, pos, len);
}

static int write_file(Stream_t *Stream, char *buf, off_t where, size_t len)
{
	DeclareThis(File_t);
	int pos;
	int ret;
	Stream_t *Disk = This->Fs->Next;

	pos = This->map(This, where, &len, MT_WRITE);
	if( pos < 0)
		return pos;
	ret = WRITES(Disk, buf, pos, len);
	if ( ret > 0 && where + ret > This->FileSize )
		This->FileSize = where + ret;
	return ret;
}


/*
 * Convert an MSDOS time & date stamp to the Unix time() format
 */

static int month[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
static long conv_stamp(struct directory *dir)
{
	struct tm *tmbuf;
	long tzone, dst;
	time_t accum;

	accum = DOS_YEAR(dir) - 1970; /* years past */

	/* days passed */
	accum = accum * 365L + month[DOS_MONTH(dir)-1] + DOS_DAY(dir);

	/* leap years */
	accum += (DOS_YEAR(dir) - 1972) / 4L;

	/* back off 1 day if before 29 Feb */
	if (!(DOS_YEAR(dir) % 4) && DOS_MONTH(dir) < 3)
	        accum--;
	accum = accum * 24L + DOS_HOUR(dir); /* hours passed */
	accum = accum * 60L + DOS_MINUTE(dir); /* minutes passed */
	accum = accum * 60L + DOS_SEC(dir); /* seconds passed */

	/* correct for Time Zone */
#ifdef HAVE_GETTIMEOFDAY
	{
		struct timeval tv;
		struct timezone tz;
		
		gettimeofday(&tv, &tz);
		tzone = tz.tz_minuteswest * 60L;
	}
#else
#ifdef HAVE_TZSET
	{
#ifndef OS_ultrix
		/* Ultrix defines this to be a different type */
		extern long timezone;
#endif
		tzset();
		tzone = (long) timezone;
	}
#else
	tzone = 0;
#endif /* HAVE_TZSET */
#endif /* HAVE_GETTIMEOFDAY */
	
	accum += tzone;

	/* correct for Daylight Saving Time */
	tmbuf = localtime(&accum);
	dst = (tmbuf->tm_isdst) ? (-60L * 60L) : 0L;
	accum += dst;
	
	return accum;
}


static int get_file_data(Stream_t *Stream, long *date, size_t *size,
			 int *type, int *address)
{
	DeclareThis(File_t);

	if(date)
		*date = conv_stamp(& This->dir);
	if(size)
		*size = This->FileSize;
	if(type)
		*type = This->dir.attr & 0x10;
	if(address)
		*address = This->FirstAbsCluNr;
	return 0;
}

T_HashTable *filehash;

static int free_file(Stream_t *Stream)
{
	DeclareThis(File_t);
	FREE(&This->ParentDir);
	return hash_remove(filehash, (void *) Stream, 0);
}

static Class_t FileClass = { 
	read_file, 
	write_file, 
	0, /* flush */
	free_file, /* free */
	0, /* get_geom */
	get_file_data 
};


static unsigned int func1(void *Stream)
{
	DeclareThis(File_t);

	return This->FirstAbsCluNr ^ (long) This->Fs;
}

static unsigned int func2(void *Stream)
{
	DeclareThis(File_t);

	return This->FirstAbsCluNr;
}

static int comp(void *Stream, void *Stream2)
{
	DeclareThis(File_t);

	File_t *This2 = (File_t *) Stream2;

	return This->Fs != This2->Fs ||
		This->FirstAbsCluNr != This2->FirstAbsCluNr;
}

static void init_hash(void)
{
	static int is_initialised=0;
	
	if(!is_initialised){
		make_ht(func1, func2, comp, 20, &filehash);
		is_initialised = 1;
	}
}


static Stream_t *OpenFileByNumAndSize(Stream_t *Dir, unsigned int first, 
				      size_t size, struct directory *dir);

Stream_t *open_root(Stream_t *Dir)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);
	File_t *File;
	File_t Pattern;
	unsigned int num;

	num = fat32RootCluster(Dir);

	if(num)		
		return OpenFileByNumAndSize(Dir, num, MAX_SIZE, 0);

	init_hash();
	This->refs++;
	Pattern.Fs = (Fs_t *) This;
	Pattern.FirstAbsCluNr = 0;
	if(!hash_lookup(filehash, (T_HashTableEl) &Pattern, 
			(T_HashTableEl **)&File, 0)){
		File->refs++;
		This->refs--;
		return (Stream_t *) File;
	}

	File = New(File_t);
	File->Fs = This;
	if (!File)
		return NULL;

	File->ParentDir = 0;
	File->FirstAbsCluNr = 0;	
	File->FileSize = -1;
	File->Class = &FileClass;
	File->map = root_map;
	File->refs = 1;
	File->Buffer = 0;
	hash_add(filehash, (void *) File);
	return (Stream_t *) File;
}

Stream_t *open_file(Stream_t *Dir, struct directory *dir)
{
	Stream_t *Stream = GetFs(Dir);
	unsigned int first;
	size_t size;

	first = START(dir);
	if(fat32RootCluster(Stream))
		first |= STARTHI(dir) << 16;

	if(!first && (dir->attr & 0x10))
		return open_root(Stream);
	
	if (dir->attr & 0x10 )
		size = MAX_SIZE;
	else 
		size = FILE_SIZE(dir);

	return OpenFileByNumAndSize(Dir, first, size, dir);
}

static Stream_t *OpenFileByNumAndSize(Stream_t *Dir, unsigned int first, 
				      size_t size, struct directory *dir)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);
	File_t Pattern;
	File_t *File;

	init_hash();
	This->refs++;
	if(first >= 2){
		/* if we have a zero-addressed file here, it is certainly
		 * _not_ the root directory, but rather a newly created
		 * file */
		Pattern.Fs = This;
		Pattern.FirstAbsCluNr = first;
		if(!hash_lookup(filehash, (T_HashTableEl) &Pattern, 
				(T_HashTableEl **)&File, 0)){
			File->refs++;
			This->refs--;
			return (Stream_t *) File;
		}
	}

	File = New(File_t);
	if (!File)
		return NULL;
	
	/* memorize dir for date and attrib */
	if(dir)
		File->dir = *dir;
	File->ParentDir = COPY(Dir);
	File->locked = 0;
	File->Class = &FileClass;
	File->Fs = This;
	File->map = normal_map;
	File->FirstAbsCluNr = first;
	File->PreviousRelCluNr = 0xffff;
	File->FileSize = size;
	File->refs = 1;
	File->Buffer = 0;
	hash_add(filehash, (void *) File);
	return (Stream_t *) File;
}

int FileIsLocked(Stream_t *Stream)
{
	DeclareThis(File_t);

	return This->locked;
}

void LockFile(Stream_t *Stream)
{
	DeclareThis(File_t);

	This->locked = 1;
}

	
