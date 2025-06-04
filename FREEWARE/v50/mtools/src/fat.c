#include "sysincludes.h"
#include "msdos.h"
#include "stream.h"
#include "mtools.h"
#include "fsP.h"

extern Stream_t *default_drive;


#define SECT_PER_ENTRY (sizeof(int)*8)

static FatMap_t *GetFatMap(Fs_t *Stream)
{
	int nr_entries,i;
	FatMap_t *map;

	Stream->fat_error = 0;
	nr_entries = (Stream->fat_len + SECT_PER_ENTRY - 1) / SECT_PER_ENTRY;
	map = NewArray(nr_entries, FatMap_t);
	if(!map)
		return 0;

	for(i=0; i< nr_entries; i++) {
		map[i].data = 0;
		map[i].valid = 0;
		map[i].dirty = 0;
	}

	return map;
}

static int locate(Fs_t *Stream, int offset, int *slot, int *bit)
{
	if(offset > Stream->fat_len)
		return -1;
	*slot = offset / SECT_PER_ENTRY;
	*bit = offset % SECT_PER_ENTRY;
	return 0;
}


static int fatReadSector(Fs_t *This, int sector, int slot, int bit, int dupe)
{
	int fat_start;

	dupe = (dupe + This->primaryFat) % This->num_fat;
	fat_start = This->fat_start + This->fat_len * dupe;

	return force_read(This->Next,
			  (char *) 
			  (This->FatMap[slot].data + bit * This->sector_size),
			  (fat_start+sector) * This->sector_size,
			  This->sector_size);
}


static int fatWriteSector(Fs_t *This, int sector, int slot, int bit, int dupe)
{
	int fat_start;

	dupe = (dupe + This->primaryFat) % This->num_fat;
	if(dupe && !This->writeAllFats)
		return This->sector_size;

	fat_start = This->fat_start + This->fat_len * dupe;

	return force_write(This->Next,
			   (char *) 
			   (This->FatMap[slot].data + bit * This->sector_size),
			   (fat_start+sector)*This->sector_size,
			   This->sector_size);
}


enum mode_t { FAT_ACCESS_READ, FAT_ACCESS_WRITE };

static unsigned char *loadSector(Fs_t *This,
				 unsigned int sector, enum mode_t mode,
				 int recurs)
{
	int slot, bit, i, ret;

	if(locate(This,sector, &slot, &bit) < 0)
		return 0;
	if(!This->FatMap[slot].data) {
		/* allocate the storage space */
		This->FatMap[slot].data = 
			malloc(This->sector_size * SECT_PER_ENTRY);
		if(!This->FatMap[slot].data)
			return 0;		   
	}

	if(! (This->FatMap[slot].valid & (1 << bit))) {
		ret = -1;
		for(i=0; i< This->num_fat; i++) {
			/* read the sector */
			ret = fatReadSector(This, sector, slot, bit, i);

			if(ret != This->sector_size) {
				fprintf(stderr,
					"Error reading fat number %d\n", i);
				continue;
			}
			break;
		}

		/* all copies bad.  Return error */
		if(ret != This->sector_size)
			return 0;

		This->FatMap[slot].valid |= 1 << bit;
		/* do some prefetching  */
		if(!recurs)
			loadSector(This, sector+1, mode, 1);
	}

	if(mode == FAT_ACCESS_WRITE)
		This->FatMap[slot].dirty |= 1 << bit;
	return This->FatMap[slot].data + bit * This->sector_size;
}


static unsigned char *getAddress(Fs_t *Stream,
				 unsigned int num, enum mode_t mode)
{
	unsigned char *ret;
	int offset, sector;

	offset = num % Stream->sector_size;
	sector = num / Stream->sector_size;
	ret = loadSector(Stream, sector, mode, 0);
	if(!ret)
		return 0;
	else
		return ret+offset;
}


/*
 * Fat 12 encoding:
 *	|    byte n     |   byte n+1    |   byte n+2    |
 *	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|
 *	| | | | | | | | | | | | | | | | | | | | | | | | |
 *	| n+0.0 | n+0.5 | n+1.0 | n+1.5 | n+2.0 | n+2.5 |
 *	    \_____  \____   \______/________/_____   /
 *	      ____\______\________/   _____/  ____\_/
 *	     /     \      \          /       /     \
 *	| n+1.5 | n+0.0 | n+0.5 | n+2.0 | n+2.5 | n+1.0 |
 *	|      FAT entry k      |    FAT entry k+1      |
 */
 
 /*
 * Get and decode a FAT (file allocation table) entry.  Returns the cluster
 * number on success or 1 on failure.
 */

static unsigned int fat12_decode(Fs_t *Stream, unsigned int num)
{
	unsigned int start = num * 3 / 2;
	unsigned char *address0 = getAddress(Stream, start, FAT_ACCESS_READ);
	unsigned char *address1 = getAddress(Stream, start+1, FAT_ACCESS_READ);

	if (num < 2 || !address0 || !address1 || num > Stream->num_clus+1) {
		fprintf(stderr,"Bad address %d\n", num);
		return 1;
	}

	if (num & 1)
		return (((*address1) & 0xff) << 4) | (((*address0) & 0xf0)>>4);
	else
		return (((*address1) & 0xf) << 8) | ((*address0) & 0xff );
}




/*
 * Puts a code into the FAT table.  Is the opposite of fat_decode().  No
 * sanity checking is done on the code.  Returns a 1 on error.
 */
static int fat12_encode(Fs_t *Stream, unsigned int num, unsigned int code)
{
	DeclareThis(Fs_t);

	int start = num * 3 / 2;
	unsigned char *address0 = getAddress(Stream, start, FAT_ACCESS_WRITE);
	unsigned char *address1 = getAddress(Stream, start+1, FAT_ACCESS_WRITE);

	if (num < 2 || !address0 || !address1 || num >This->num_clus+1) {
		fprintf(stderr,"Bad address %d\n", num);
		return 1;
	}


	This->fat_dirty = 1;
	if (num & 1) {
		/* (odd) not on byte boundary */
		*address0 = (*address0 & 0x0f) | ((code << 4) & 0xf0);
		*address1 = (code >> 4) & 0xff;
	} else {
		/* (even) on byte boundary */
		*address0 = code & 0xff;
		*address1 = (*address1 & 0xf0) | ((code >> 8) & 0x0f);
	}
	return 0;
}


/*
 * Fat 16 encoding:
 *	|    byte n     |   byte n+1    |
 *	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|
 *	| | | | | | | | | | | | | | | | |
 *	|         FAT entry k           |
 */

static unsigned int fat16_decode(Fs_t *Stream, unsigned int num)
{
	unsigned int start = num * 2;
	unsigned char *address = getAddress(Stream, start, FAT_ACCESS_READ);
	/* always aligned */

	if (num < 2 || !address || num > Stream->num_clus+1) {
		fprintf(stderr,"Bad address %d\n", num);
		return 1;
	}
	return _WORD(address);
}

static int fat16_encode(Fs_t *Stream, unsigned int num, unsigned int code)
{       
	DeclareThis(Fs_t);

	int start = num * 2;
	unsigned char *address = getAddress(Stream, start, FAT_ACCESS_WRITE);

	if (num < 2 || !address || num >This->num_clus+1) {
		fprintf(stderr,"Bad address %d\n", num);
		return 1;
	}

	This->fat_dirty = 1;
	set_word(address, code);
	return 0;
}


/*
 * Fat 32 encoding
 */
static unsigned int fat32_decode(Fs_t *Stream, unsigned int num)
{
	unsigned int start = num * 4;
	unsigned char *address = getAddress(Stream, start, FAT_ACCESS_READ);
	/* always aligned */

	if (num < 2 || !address || num > Stream->num_clus+1) {
		fprintf(stderr,"Bad address %d\n", num);
		return 1;
	}
	return _DWORD(address);
}

static int fat32_encode(Fs_t *Stream, unsigned int num, unsigned int code)
{       
	DeclareThis(Fs_t);
	int oldcode;

	int start = num * 4;
	unsigned char *address = getAddress(Stream, start, FAT_ACCESS_WRITE);

	if (num < 2 || !address || num >This->num_clus+1) {
		fprintf(stderr,"Bad address %d\n", num);
		return 1;
	}
	This->fat_dirty = 1;
	oldcode = _DWORD(address);
	set_dword(address, code);

	if(Stream->infoSector) {
		int free;
		free = _DWORD(Stream->infoSector->count);
		if(code != MAX32)
			/* put the pointer on the sector actually allocated,
			 * and not on the previous one, unless we are at the
			 * end. */
			num = code;
		if(code)
			set_dword(Stream->infoSector->pos,num);
		if(free != MAX32) {
			if(oldcode)
				free++;
			if(code)
				free--;
			set_dword(Stream->infoSector->count, free);
		}
	}

	return 0;
}


/*
 * Write the FAT table to the disk.  Up to now the FAT manipulation has
 * been done in memory.  All errors are fatal.  (Might not be too smart
 * to wait till the end of the program to write the table.  Oh well...)
 */

void fat_write(Fs_t *This)
{
	int i, j, dups, ret, bit, slot;
	int fat_start;

	if (!This->fat_dirty)
		return;

	dups = This->num_fat;
	if (This->fat_error)
		dups = 1;


	for(i=0; i<dups; i++){
		j = 0;
		fat_start = This->fat_start + i*This->fat_len;
		for(slot=0;j<This->fat_len;slot++) {
			if(!This->FatMap[slot].dirty) {
				j += SECT_PER_ENTRY;
				continue;
			}
			for(bit=0; 
			    bit < SECT_PER_ENTRY && j<This->fat_len;
			    bit++,j++) {
				if(!(This->FatMap[slot].dirty & (1 << bit)))
					continue;
				ret = fatWriteSector(This,j,slot, bit, i);
				if (ret < This->sector_size){
					if (ret < 0 ){
						perror("error in fat_write");
						exit(1);
					} else {
						fprintf(stderr,
							"end of file in fat_write\n");
						exit(1);
					}
				}
				/* if last dupe, zero it out */
				if(i==dups-1)
					This->FatMap[slot].dirty &= ~(1<<bit);
			}
		}	 
	}
	/* write the info sector, if any */
	if(This->infoSector) {
		if(force_write(This->Next,
			       (char *)This->infoSector,
			       This->infoSectorLoc*This->sector_size,
			       This->sector_size) !=
		   This->sector_size)
			fprintf(stderr,"Trouble writing the info sector\n");
	}
	This->fat_dirty = 0;
}



/*
 * Zero-Fat
 * Used by mformat.
 */
int zero_fat(Fs_t *Stream, int media_descriptor)
{
	int i, j;
	int fat_start;
	unsigned char *buf;

	buf = safe_malloc(Stream->sector_size);
	for(i=0; i< Stream->num_fat; i++) {
		fat_start = Stream->fat_start + i*Stream->fat_len;
		for(j = 0; j < Stream->fat_len; j++) {
			if(j <= 1)
				memset(buf, 0, Stream->sector_size);
			if(!j) {
				buf[0] = media_descriptor;
				buf[2] = buf[1] = 0xff;
				if(Stream->fat_bits > 12)
					buf[3] = 0xff;
				if(Stream->fat_bits > 16) {
					buf[4] = 0xff;
					buf[5] = 0xff;
					buf[6] = 0xff;
					buf[7] = 0xff;
				}
			}

			if(force_write(Stream->Next,
				       (char *)buf,
				       (fat_start + j) * Stream->sector_size,
				       Stream->sector_size) !=
			   Stream->sector_size) {
				fprintf(stderr,
					"Trouble initializing a FAT sector\n");
				free(buf);
				return -1;
			}
		}
	}
	
	free(buf);
	Stream->FatMap = GetFatMap(Stream);
	if (Stream->FatMap == NULL) {
		perror("alloc fat map");
		return -1;
	}
	return 0;
}


void set_fat12(Fs_t *This)
{
	This->fat_bits = 12;
	This->end_fat = 0xfff;
	This->last_fat = 0xff6;
	This->fat_decode = fat12_decode;
	This->fat_encode = fat12_encode;
}

static int try_fat12(Fs_t *This)
{
	if (This->num_clus > FAT12)
		/* too many clusters */
		return -2;
	set_fat12(This);
	return 0;
}

void set_fat16(Fs_t *This)
{
	This->fat_bits = 16;
	This->end_fat = 0xffff;
	This->last_fat = 0xfff6;
	This->fat_decode = fat16_decode;
	This->fat_encode = fat16_encode;
}

static int try_fat16(Fs_t *This)
{
	unsigned char *address;

	set_fat16(This);
	if(This->fat_len < NEEDED_FAT_SIZE(This))
		return -2;
	address = getAddress(This, 3, FAT_ACCESS_READ);
	if(!address || address[3] != 0xff)
		return -1;
	return 0;
}

void set_fat32(Fs_t *This)
{
	This->fat_bits = 32;
	This->end_fat = 0xffffffff;
	This->last_fat = 0xfffffff6;
	
	This->fat_decode = fat32_decode;
	This->fat_encode = fat32_encode;       	
}


static int check_fat(Fs_t *This, int verbose)
{
	/* 
	 * This is only a sanity check.  For disks with really big FATs,
	 * there is no point in checking the whole FAT.
	 */

	int i, f, tocheck;
	if(mtools_skip_check)
		return 0;
	
	tocheck = This->num_clus;
	if(tocheck > 4096)
		tocheck = 4096;

	for ( i= 3 ; i < tocheck; i++){
		f = This->fat_decode(This,i);
		if (f < This->last_fat && f > This->num_clus){
			if(verbose){
				fprintf(stderr,
					"Cluster # at %d too big(%#x)\n", i,f);
				fprintf(stderr,"Probably non MS-DOS disk\n");
			}
			return -1;
		}
	}
	return 0;
}

static int try_fat(Fs_t *This, int bits, int verbose)
{
	int ret;

	if (bits == 12)
		ret = try_fat12(This);
	else
		ret = try_fat16(This);
	if(ret)
		return ret;
	else
		return check_fat(This, verbose);
}

/*
 * Read the first sector of FAT table into memory.  Crude error detection on
 * wrong FAT encoding scheme.
 */
static int check_media_type(Fs_t *This, struct bootsector *boot, 
			    unsigned int tot_sectors)
{
	unsigned char *address;

	This->num_clus = (tot_sectors - This->clus_start) / This->cluster_size;

	This->FatMap = GetFatMap(This);
	if (This->FatMap == NULL) {
		perror("alloc fat map");
		return -1;
	}

	address = getAddress(This, 0, FAT_ACCESS_READ);
	if(!address) {
		fprintf(stderr,
			"Could not read first FAT sector\n");
		return -1;
	}

	if (!mtools_skip_check && (address[0] || address[1] || address[2])) {
		if((address[0] != boot->descr && boot->descr >= 0xf0 &&
		    (address[0] != 0xf9 || boot->descr != 0xf0)) ||
		   address[0] < 0xf0){
			fprintf(stderr,
				"Bad media type, probably non-MSDOS disk\n");
			return -1;
		}
		if(address[1] != 0xff || address[2] != 0xff){
			fprintf(stderr,"Initial byte of fat is not 0xff\n");
			return -1;
		}
	}
	return 0;
}



static int fat_32_read(Fs_t *This, struct bootsector *boot, 
		       unsigned int tot_sectors)
{
	This->fat_len = DWORD(ext.fat32.bigFat);
	This->writeAllFats = !(boot->ext.fat32.extFlags[0] & 0x80);
	This->primaryFat = boot->ext.fat32.extFlags[0] & 0xf;
	This->rootCluster = DWORD(ext.fat32.rootCluster);
	This->clus_start = This->fat_start + This->num_fat * This->fat_len;

	/* read the info sector */
	This->infoSectorLoc = WORD(ext.fat32.infoSector);
	if(This->infoSectorLoc > 0) {
		This->infoSector = (InfoSector_t *) malloc(This->sector_size);
		if(This->infoSector) {
			if(force_read(This->Next,
				      (char *)This->infoSector,
				      This->infoSectorLoc*This->sector_size,
				      This->sector_size) !=
			   This->sector_size) {
				free(This->infoSector);
				This->infoSector = 0;
			}
			if(_DWORD(This->infoSector->signature) != 
			   INFOSECT_SIGNATURE){
				free(This->infoSector);
				This->infoSector = 0;
			}
		}
	}
	
	set_fat32(This);
	if(check_media_type(This,boot, tot_sectors))
		return -1;
	return 0;
}


static int old_fat_read(Fs_t *This, struct bootsector *boot, int fat_bits,
			unsigned int tot_sectors, int nodups)
{
	int ret, ret2;

	This->writeAllFats = 1;
	This->primaryFat = 0;
	This->dir_start = This->fat_start + This->num_fat * This->fat_len;
	This->clus_start = This->dir_start + This->dir_len;

	if(nodups)
		This->num_fat = 1;

	if(check_media_type(This,boot, tot_sectors))
		return -1;

	switch(fat_bits){
		case 12:
		case 16:
			ret = try_fat(This, fat_bits, 1);
			break;
		case -12:
		case -16:
			ret = try_fat(This, -fat_bits, 1);
			break;
		case 0:
			/* no size given in the configuration file.
			 * Figure out a first guess */
			
			if (boot->descr < 0xf0 || boot->ext.old.dos4 !=0x29)
				fat_bits = 12; /* old DOS */
			else if (!strncmp(boot->ext.old.fat_type, 
					  "FAT12   ", 8))
				fat_bits = 12;
			else if (!strncmp (boot->ext.old.fat_type, 
					   "FAT16   ", 8))
				fat_bits = 16;
			else
				fat_bits = 12;
			
			ret = try_fat(This, fat_bits, 1);
			if(!ret)
				break;

			fat_bits = 28 - fat_bits;
			ret2 = try_fat(This, fat_bits, 1);
			if(ret2 >= ret){ 
				/* second try didn't fail as badly as first */
				ret = ret2;
				break;
			}

			/* revert to first try because that one failed
			 * less badly */
			fat_bits = 28 - fat_bits;
			ret = try_fat(This, fat_bits, 1);
			break;
		default:
			fprintf(stderr,"%d fat bits not supported\n", fat_bits);
			return -1;
	}

	if(ret == -2) {
		/* the return value suggests a different fat,
		 * independently of the read data */
		if(fat_bits > 0 ){				
			fprintf(stderr,
				"%c: %d bit FAT. sure ? (Use -%d in the device configuration file to bypass.)\n",
				This->drive, fat_bits, fat_bits);
			return -2;
		} else if(fat_bits >= 0)
			return -1;
	}

	/*
	 * Let's see if the length of the FAT table is appropriate for
	 * the number of clusters and the encoding scheme.
	 * Older versions of mtools goofed this up. If the env var
	 * MTOOLS_FAT_COMPATIBILITY is defined, skip this check in order to read
	 * disks formatted by an old version.
	 */
	if(!mtools_fat_compatibility &&
	   This->fat_len > NEEDED_FAT_SIZE(This) + 1){
		fprintf(stderr,
			"fat_read: Wrong FAT encoding for drive %c."
			"(len=%d instead of %d?)\n",
			This->drive, This->fat_len, NEEDED_FAT_SIZE(This));
		fprintf(stderr,
			"Set MTOOLS_FAT_COMPATIBILITY to suppress"
			" this message\n");
		return -1;
	}
	return 0;
}



/*
 * Read the first sector of the  FAT table into memory and initialize 
 * structures.
 */
int fat_read(Fs_t *This, struct bootsector *boot, int fat_bits,
	     unsigned int tot_sectors, int nodups)
{
	This->fat_error = 0;
	This->fat_dirty = 0;
	This->infoSector = 0;

	if(This->fat_len)
		return old_fat_read(This, boot, fat_bits, tot_sectors, nodups);
	else
		return fat_32_read(This, boot, tot_sectors);
}


unsigned int get_next_free_cluster(Fs_t *This, unsigned int last)
{
	int i;

	if(This->infoSector)
		last = _DWORD(This->infoSector->pos);

	if ( last < 2)
		last = 1;

	for (i=last+1; i< This->num_clus+2; i++){
		if (!This->fat_decode(This, i))
			return i;
	}
	for(i=2; i < last+1; i++){
		if (!This->fat_decode(This, i))
			return i;
	}
	fprintf(stderr,"No free cluster\n");
	return 1;
}

int fat_error(Stream_t *Dir)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);

	if(This->fat_error)
		fprintf(stderr,"Fat error detected\n");

	return This->fat_error;
}

int fat32RootCluster(Stream_t *Dir)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);
	
	if(This->fat_bits == 32)
		return This->rootCluster;
	else
		return 0;
}


/*
 * Get the amount of free space on the diskette
 */

unsigned long getfree(Stream_t *Dir)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);
	register unsigned int i;
	size_t total;

	if(This->infoSector && 
	   (total = _DWORD(This->infoSector->count)) != MAX32)
		return total * This->sector_size * This->cluster_size;

	total = 0L;
	for (i = 2; i < This->num_clus + 2; i++) {
		/* if fat_decode returns zero */
		if (!This->fat_decode(This,i))
			total += This->cluster_size;
	}

	if(This->infoSector)
		set_dword(This->infoSector->count, total);

	return(total * This->sector_size);
}


/*
 * Ensure that there is a minimum of total sectors free
 */

unsigned long getfreeMin(Stream_t *Dir, size_t ref)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(Fs_t);
	register unsigned int i, last;
	size_t total, ref2;

	ref2 = ref  / (This->sector_size * This->cluster_size);
	if(ref % (This->sector_size * This->cluster_size))
		ref2++;

	if(This->infoSector && 
	   (total = _DWORD(This->infoSector->count)) != MAX32)
		return total >= ref2;

	total = 0L;

	/* we start at the same place where we'll start later to actually
	 * allocate the sectors.  That way, the same sectors of the FAT, which
	 * are already loaded during getfreeMin will be able to be reused 
	 * during get_next_free_cluster */

	if(This->infoSector)
		last = _DWORD(This->infoSector->pos);
	else
		last = 1;

	
	if ( last < 2)
		last = 1;
	for (i=last+1; i< This->num_clus+2; i++){
		if (!This->fat_decode(This, i))
			total++;
		if(total >= ref2)
			return 1;
	}
	for(i=2; i < last+1; i++){
		if (!This->fat_decode(This, i))
			total++;
		if(total >= ref2)
			return 1;
	}
	return 0;
}
