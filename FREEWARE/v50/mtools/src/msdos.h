#ifndef MTOOLS_MSDOS_H
#define MTOOLS_MSDOS_H

/*
 * msdos common header file
 */

#define MAX_SECTOR	8192   		/* largest sector size */
#define MDIR_SIZE	32		/* MSDOS directory entry size in bytes*/
#define MAX_CLUSTER	8192		/* largest cluster size */
#define MAX_PATH	128		/* largest MSDOS path length */
#define MAX_DIR_SECS	64		/* largest directory (in sectors) */
#define MSECTOR_SIZE    msector_size

#define NEW		1
#define OLD		0

#define _WORD(x) ((unsigned char)(x)[0] + (((unsigned char)(x)[1]) << 8))
#define _DWORD(x) (_WORD(x) + (_WORD((x)+2) << 16))

#define DELMARK ((char) 0xe5)

struct directory {
	char name[8];			/* file name */
	char ext[3];			/* file extension */
	unsigned char attr;		/* attribute byte */
	unsigned char Case;		/* case of short filename */
	unsigned char ctime_ms;		/* creation time, milliseconds (?) */
	unsigned char ctime[2];		/* creation time */
	unsigned char cdate[2];		/* creation date */
	unsigned char adate[2];		/* last access date */
	unsigned char startHi[2];	/* start cluster, Hi */
	unsigned char time[2];		/* time stamp */
	unsigned char date[2];		/* date stamp */
	unsigned char start[2];		/* starting cluster number */
	unsigned char size[4];		/* size of the file */
};

#define EXTCASE 0x10
#define BASECASE 0x8

#define MAX32 0xffffffff
#define MAX_SIZE 0x7fffffff

#define FILE_SIZE(dir)  (_DWORD((dir)->size))
#define START(dir) (_WORD((dir)->start))
#define STARTHI(dir) (_WORD((dir)->startHi))

/* ASSUMPTION: long is at least 32 bits */
//UNUSED(static inline void set_dword(unsigned char *data, unsigned long value))
static void set_dword(unsigned char *data, unsigned long value)
{
	data[3] = (value >> 24) & 0xff;
	data[2] = (value >> 16) & 0xff;
	data[1] = (value >>  8) & 0xff;
	data[0] = (value >>  0) & 0xff;
}


/* ASSUMPTION: short is at least 16 bits */
//UNUSED(static inline void set_word(unsigned char *data, unsigned short value))
static void set_word(unsigned char *data, unsigned short value)
{
	data[1] = (value >>  8) & 0xff;
	data[0] = (value >>  0) & 0xff;
}


/*
 *	    hi byte     |    low byte
 *	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|
 *      | | | | | | | | | | | | | | | | |
 *      \   7 bits    /\4 bits/\ 5 bits /
 *         year +80      month     day
 */
#define	DOS_YEAR(dir) (((dir)->date[1] >> 1) + 1980)
#define	DOS_MONTH(dir) (((((dir)->date[1]&0x1) << 3) + ((dir)->date[0] >> 5)))
#define	DOS_DAY(dir) ((dir)->date[0] & 0x1f)

/*
 *	    hi byte     |    low byte
 *	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|
 *      | | | | | | | | | | | | | | | | |
 *      \  5 bits /\  6 bits  /\ 5 bits /
 *         hour      minutes     sec*2
 */
#define	DOS_HOUR(dir) ((dir)->time[1] >> 3)
#define	DOS_MINUTE(dir) (((((dir)->time[1]&0x7) << 3) + ((dir)->time[0] >> 5)))
#define	DOS_SEC(dir) (((dir)->time[0] & 0x1f) * 2)


typedef struct InfoSector_t {
	unsigned char signature[4];
	unsigned char count[4];
	unsigned char pos[4];
} InfoSector_t;

#define INFOSECT_SIGNATURE 0x61417272


/* FAT32 specific info in the bootsector */
typedef struct fat32_t {
	unsigned char bigFat[4];	/* 36 nb of sectors per FAT */
	unsigned char extFlags[2];     	/* 40 extension flags */
	unsigned char fsVersion[2];	/* 42 ? */
	unsigned char rootCluster[4];	/* 44 start cluster of root dir */
	unsigned char infoSector[2];	/* 48 changeable global info */
	unsigned char backupBoot[2];	/* 50 back up boot sector */
	unsigned char reserved[6];	/* 52 ? */
} fat32; /* ends at 58 */

typedef struct oldboot_t {
	unsigned char physdrive;	/* 36 physical drive ? */
	unsigned char reserved;		/* 37 reserved */
	unsigned char dos4;		/* 38 dos > 4.0 diskette */
	unsigned char serial[4];       	/* 39 serial number */
	char label[11];			/* 43 disk label */
	char fat_type[8];		/* 54 FAT type */
			
	unsigned char res_2m;		/* 62 reserved by 2M */
	unsigned char CheckSum;		/* 63 2M checksum (not used) */
	unsigned char fmt_2mf;		/* 64 2MF format version */
	unsigned char wt;		/* 65 1 if write track after format */
	unsigned char rate_0;		/* 66 data transfer rate on track 0 */
	unsigned char rate_any;		/* 67 data transfer rate on track<>0 */
	unsigned char BootP[2];		/* 68 offset to boot program */
	unsigned char Infp0[2];		/* 70 T1: information for track 0 */
	unsigned char InfpX[2];		/* 72 T2: information for track<>0 */
	unsigned char InfTm[2];		/* 74 T3: track sectors size table */
	unsigned char DateF[2];		/* 76 Format date */
	unsigned char TimeF[2];		/* 78 Format time */
	unsigned char junk[512 - 80];	/* 80 remaining data */
} oldboot_t;

struct bootsector {
	unsigned char jump[3];		/* 0  Jump to boot code */
	char banner[8] PACKED;	       	/* 3  OEM name & version */
	unsigned char secsiz[2] PACKED;	/* 11 Bytes per sector hopefully 512 */
	unsigned char clsiz;    	/* 13 Cluster size in sectors */
	unsigned char nrsvsect[2];	/* 14 Number of reserved (boot) sectors */
	unsigned char nfat;		/* 16 Number of FAT tables hopefully 2 */
	unsigned char dirents[2] PACKED;/* 17 Number of directory slots */
	unsigned char psect[2] PACKED; 	/* 19 Total sectors on disk */
	unsigned char descr;		/* 21 Media descriptor=first byte of FAT */
	unsigned char fatlen[2];	/* 22 Sectors in FAT */
	unsigned char nsect[2];		/* 24 Sectors/track */
	unsigned char nheads[2];	/* 26 Heads */
	unsigned char nhs[4];		/* 28 number of hidden sectors */
	unsigned char bigsect[4];	/* 32 big total sectors */

	union {
		struct fat32_t fat32;
		struct oldboot_t old;
	} ext;
};

#define CHAR(x) (boot->x[0])
#define WORD(x) (_WORD(boot->x))
#define DWORD(x) (_DWORD(boot->x))
#define OFFSET(x) (((char *) (boot->x)) - ((char *)(boot->jump)))


extern struct OldDos_t {
	int tracks;
	int sectors;
	int heads;
	
	int dir_len;
	int cluster_size;
	int fat_len;

	int media;
} old_dos[];

#define FAT12 4085 /* max. number of clusters described by a 12 bit FAT */
#define FAT16 65525

#define NEEDED_FAT_SIZE(x) ((((x)->num_clus+2) * ((x)->fat_bits/4) -1 )/ 2 / (x)->sector_size + 1)

#define MAX_FAT12_FATLEN(sec_siz) (2*((FAT12+2)*3+(2*sec_siz)-1)/(sec_siz)/2)
#define MAX_FAT16_FATLEN(sec_siz) (2*((FAT16+2)*2+(sec_siz)-1)/(sec_siz))
/* The maximal fat length (both fats) for a given drive is the number
 * of slots times the size of one slot divided by the size of a sector */

#define MAX_FAT12_SIZE(sec_siz) (64 * FAT12 + MAX_FAT12_FATLEN(sec_siz) + 2)
#define MAX_FAT16_SIZE(sec_siz) (64 * FAT16 + MAX_FAT16_FATLEN(sec_siz) + 2)
/* the maximal size of a drive for a given fat size is FATn clusters
 * of max 64* sectors each, plus the two fats plus a boot sector and
 * at least one dir sector.  This doesn't give an absolute maximum,
 * but a very close approximation.
 * Experimentally, it turns out that DOS only wants powers of two, and 
 * less than 128 (else it gets a divide overflow) */


#define MIN_FAT16_SIZE(sec_siz) (FAT12+MAX_FAT12_FATLEN(sec_siz)+2)
/* minimal size for which a 16 bit FAT makes sense.  With less
 * sectors, the drive could be set up as a FAT12 drive with 1 sector
 * clusters. 16 bit would only be a waste of space */

extern const char *mversion;
extern const char *mdate;

/*
 * Function Prototypes */

int ask_confirmation(const char *, const char *, const char *);
char *get_homedir(void);
#define EXPAND_BUF 2048
const char *expand(const char *, char *);
const char *fix_mcwd(char *);
FILE *open_mcwd(const char *mode);

char *get_name(const char *, char *, char *mcwd);
char *get_path(const char *, char *, char *mcwd, int mode);
char get_drive(const char *, char);

int init(char drive, int mode);



#define MT_READ 1
#define MT_WRITE 2

#endif

