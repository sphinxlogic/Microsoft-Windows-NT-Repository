/*
 * Initialize an MSDOS diskette.  Read the boot sector, and switch to the
 * proper floppy disk device to match the format on the disk.  Sets a bunch
 * of global variables.  Returns 0 on success, or 1 on failure.
 */

#include "sysincludes.h"
#include "msdos.h"
#include "stream.h"
#include "mtools.h"
#include "fsP.h"
#include "plain_io.h"
#include "xdf_io.h"
#include "buffer.h"

extern int errno;


#define FULL_CYL

unsigned int num_clus;			/* total number of cluster */

#ifndef OS_linux
#define BOOTSIZE 512
#else
#define BOOTSIZE 256
#endif


/*
 * Read the boot sector.  We glean the disk parameters from this sector.
 */
static int read_boot(Stream_t *Stream, struct bootsector * boot)
{	
	/* read the first sector, or part of it */
	if (force_read(Stream, (char *) boot, 0, BOOTSIZE) !=
	    BOOTSIZE)
		return -1;
	return 0;
}

static int fs_flush(Stream_t *Stream)
{
	DeclareThis(Fs_t);

	fat_write(This);
	return 0;
}

Class_t FsClass = {
	read_pass_through, /* read */
	write_pass_through, /* write */
	fs_flush, 
	0, /* free */
	0, /* set geometry */
	get_data_pass_through
};

static int get_media_type(Stream_t *St, struct bootsector *boot)
{
	int media;

	media = boot->descr;
	if(media < 0xf0){
		char temp[512];
		/* old DOS disk. Media descriptor in the first FAT byte */
		/* old DOS disk always have 512-byte sectors */
		if (force_read(St,temp,512,512) == 512)
			media = (unsigned char) temp[0];
		else
			media = 0;
	} else
		media += 0x100;
	return media;
}


Stream_t *GetFs(Stream_t *Fs)
{
	while(Fs && Fs->Class != &FsClass)
		Fs = Fs->Next;
	return Fs;
}

Stream_t *find_device(char drive, int mode, struct device *out_dev,
		      struct bootsector *boot,
		      char *name, int *media)
{
	char errmsg[80];
	Stream_t *Stream;
	struct device *dev;

	Stream = NULL;
	sprintf(errmsg, "Drive '%c:' not supported", drive);	
					/* open the device */
	for (dev=devices; dev->name; dev++) {
		FREE(&Stream);
		if (dev->drive != drive)
			continue;
		*out_dev = *dev;
		expand(dev->name,name);
#ifdef USING_NEW_VOLD
		strcpy(name, getVoldName(dev, name));
#endif


#ifdef USE_XDF
		Stream = XdfOpen(out_dev, name, mode, errmsg, 0);
#else
		Stream = 0;
#endif

		if (!Stream)
			Stream = SimpleFileOpen(out_dev, dev, name, mode,
						errmsg, 0);
		else
			out_dev->use_2m = 0x7f;

		if( !Stream)
			continue;

		/* read the boot sector */
		if (read_boot(Stream, boot)){
			sprintf(errmsg,
				"init %c: could not read boot sector",
				drive);
			continue;
		}

		if((*media= get_media_type(Stream, boot)) <= 0xf0 ){
			sprintf(errmsg,"init %c: unknown media type", drive);
			continue;
		}

		/* set new parameters, if needed */
		errno = 0;
		if(SET_GEOM(Stream, out_dev, dev, *media, boot)){
			sprintf(errmsg, "Can't set disk parameters for %c: %s", 
				drive, strerror(errno));
			continue;
		}
		break;
	}

	/* print error msg if needed */	
	if ( dev->drive == 0 ){
		FREE(&Stream);
		fprintf(stderr,"%s\n",errmsg);
		return NULL;
	}
	return Stream;
}


Stream_t *fs_init(char drive, int mode)
{
	int media,i;
	int nhs;
	int disk_size = 0;	/* In case we don't happen to set this below */
	size_t tot_sectors;
	char name[EXPAND_BUF];
	int cylinder_size;
	struct device dev;

	struct bootsector boot0;
#define boot (&boot0)
	Fs_t *This;

	This = New(Fs_t);
	if (!This)
		return NULL;

	This->Direct = NULL;
	This->Next = NULL;
	This->refs = 1;
	This->Buffer = 0;
	This->Class = &FsClass;
	This->drive = drive;

	This->Direct = find_device(drive, mode, &dev, &boot0, name, &media);
	if(!This->Direct)
		return NULL;
	
	This->sector_size = WORD(secsiz);
	if(This->sector_size > MAX_SECTOR){
		fprintf(stderr,"init %c: sector size too big\n", drive);
		return NULL;
	}

	cylinder_size = dev.heads * dev.sectors;
	This->serialized = 0;
	if ((media & ~7) == 0xf8){
		i = media & 3;
		This->cluster_size = old_dos[i].cluster_size;
		tot_sectors = cylinder_size * old_dos[i].tracks;
		This->fat_start = 1;
		This->fat_len = old_dos[i].fat_len;
		This->dir_len = old_dos[i].dir_len;
		This->num_fat = 2;
		This->sector_size = 512;
		This->fat_bits = 12;
		nhs = 0;
	} else {
		/*
		 * all numbers are in sectors, except num_clus 
		 * (which is in clusters)
		 */
		tot_sectors = WORD(psect);
		if(!tot_sectors) {
			tot_sectors = DWORD(bigsect);			
			nhs = DWORD(nhs);
		} else
			nhs = WORD(nhs);

		if(boot0.ext.old.dos4 == 0x29 && WORD(fatlen)) {
			This->serialized = 1;
			This->serial_number = DWORD(ext.old.serial);
		}

		This->cluster_size = boot0.clsiz; 		
		This->fat_start = WORD(nrsvsect);
		This->fat_len = WORD(fatlen);
		This->dir_len = WORD(dirents) * MDIR_SIZE / This->sector_size;
		This->num_fat = boot0.nfat;
	}	
	cylinder_size = dev.sectors * dev.heads;

	if(!mtools_skip_check && (tot_sectors % dev.sectors)){
		fprintf(stderr,
			"Total number of sectors not a multiple of"
			" sectors per track!\n");
		exit(1);
	}

	/* full cylinder buffering */
#ifdef FULL_CYL
	disk_size = (dev.tracks) ? cylinder_size : 512;
#else /* FULL_CYL */
	disk_size = (dev.tracks) ? dev.sectors : 512;
#endif /* FULL_CYL */

#if (defined OS_sysv4 && !defined OS_solaris)
	/*
	 * The driver in Dell's SVR4 v2.01 is unreliable with large writes.
	 */
        disk_size = 0;
#endif /* (defined sysv4 && !defined(solaris)) */

#ifdef OS_linux
	disk_size = cylinder_size;
#endif

#if 1
	if(disk_size > 256) {
		disk_size = dev.sectors;
		if(dev.sectors % 2)
			disk_size <<= 1;
	}
#endif
	if (disk_size % 2)
		disk_size *= 2;

	if (disk_size)
		This->Next = buf_init(This->Direct,
				      disk_size * This->sector_size,
				      This->sector_size,
				      disk_size * This->sector_size);
	else
		This->Next = This->Direct;

	if (This->Next == NULL) {
		perror("init: allocate buffer");
		This->Next = This->Direct;
	}

	/* read the FAT sectors */
	if(fat_read(This, &boot0, dev.fat_bits, tot_sectors, dev.use_2m&0x7f)){
		This->num_fat = 1;
		FREE(&This->Next);
		Free(This->Next);
		return NULL;
	}
	return (Stream_t *) This;
}
