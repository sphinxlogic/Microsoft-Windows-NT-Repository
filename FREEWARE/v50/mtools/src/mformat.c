/*
 * mformat.c
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "mainloop.h"
#include "fsP.h"
#include "file.h"
#include "plain_io.h"
#include "nameclash.h"
#include "buffer.h"
#ifdef USE_XDF
#include "xdf_io.h"
#endif


#include <math.h>

#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

extern int errno;

static void init_geometry_boot(struct bootsector *boot, struct device *dev,
			       int sectors0, int rate_0, int rate_any,
			       int *tot_sectors, int keepBoot)
{
	int i;
	int nb_renum;
	int sector2;
	int size2;
	int j;
	int sum;

	set_word(boot->nsect, dev->sectors);
	set_word(boot->nheads, dev->heads);

	*tot_sectors = dev->heads * dev->sectors * dev->tracks - DWORD(nhs);

	if (*tot_sectors < 0x10000){
		set_word(boot->psect, *tot_sectors);
		set_dword(boot->bigsect, 0);
	} else {
		set_word(boot->psect, 0);
		set_dword(boot->bigsect, *tot_sectors);
	}

	if (dev->use_2m & 0x7f){
		strncpy(boot->banner, "2M-STV04", 8);
		boot->ext.old.res_2m = 0;
		boot->ext.old.fmt_2mf = 6;
		if ( dev->sectors % ( ((1 << dev->ssize) + 3) >> 2 ))
			boot->ext.old.wt = 1;
		else
			boot->ext.old.wt = 0;
		boot->ext.old.rate_0= rate_0;
		boot->ext.old.rate_any= rate_any;
		if (boot->ext.old.rate_any== 2 )
			boot->ext.old.rate_any= 1;
		i=76;

		/* Infp0 */
		set_word(boot->ext.old.Infp0, i);
		boot->jump[i++] = sectors0;
		boot->jump[i++] = 108;
		for(j=1; j<= sectors0; j++)
			boot->jump[i++] = j;

		set_word(boot->ext.old.InfpX, i);
		
		boot->jump[i++] = 64;
		boot->jump[i++] = 3;
		nb_renum = i++;
		sector2 = dev->sectors;
		size2 = dev->ssize;
		j=1;
		while( sector2 ){
			while ( sector2 < (1 << size2) >> 2 )
				size2--;
			boot->jump[i++] = 128 + j;
			boot->jump[i++] = j++;
			boot->jump[i++] = size2;
			sector2 -= (1 << size2) >> 2;
		}
		boot->jump[nb_renum] = ( i - nb_renum - 1 ) / 3;

		set_word(boot->ext.old.InfTm, i);

		sector2 = dev->sectors;
		size2= dev->ssize;
		while(sector2){
			while ( sector2 < 1 << ( size2 - 2) )
				size2--;
			boot->jump[i++] = size2;
			sector2 -= 1 << (size2 - 2 );
		}
		
		set_word(boot->ext.old.BootP,i);

		/* checksum */		
		for (sum=0, j=64; j<i; j++) 
			sum += boot->jump[j];/* checksum */
		boot->ext.old.CheckSum=-sum;
	} else {
		if(!keepBoot) {
			boot->jump[0] = 0xeb;
			boot->jump[1] = 0;
			boot->jump[2] = 0x90;
			strncpy(boot->banner, "MTOOLS38", 8);
			/* It looks like some versions of DOS are
			 * rather picky about this, and assume default
			 * parameters without this, ignoring any
			 * indication about cluster size et al. */
			set_word(boot->ext.old.BootP, OFFSET(ext.old.BootP)+2);
		}
	}
	return;
}


static int comp_fat_bits(Fs_t *Fs, int estimate, 
			 unsigned int tot_sectors, int fat32)
{
	int needed_fat_bits;

	needed_fat_bits = 12;
	if(tot_sectors > MAX_FAT12_SIZE(Fs->sector_size))
		needed_fat_bits = 16;       	
	if(fat32 || tot_sectors > MAX_FAT16_SIZE(Fs->sector_size))
		needed_fat_bits = 32;		
	if(abs(estimate) && abs(estimate) < needed_fat_bits) {
		if(fat32) {
			fprintf(stderr,
				"Contradiction between FAT size on command line and FAT size in conf file\n");
			exit(1);
		}		
		fprintf(stderr,
			"Device too big for a %d bit FAT\n",
			estimate);
		exit(1);
	}

	if(needed_fat_bits == 32 && !fat32 && abs(estimate) !=32){
		fprintf(stderr,"Warning: Using 32 bit FAT.  Drive will only be accessibly by Win95 OEM\n");		
	}

	if(!estimate) {
		if(needed_fat_bits > 12)
			return needed_fat_bits;
		else if(tot_sectors < MIN_FAT16_SIZE(Fs->sector_size))
			return 12;
		else if(tot_sectors >= 2* MIN_FAT16_SIZE(Fs->sector_size))
			return 16; /* heuristics */
	}

	return estimate;
}

static void calc_fat_bits2(Fs_t *Fs, unsigned int tot_sectors, int fat_bits)
{
	unsigned int rem_sect;

	/*
	 * the "remaining sectors" after directory and boot
	 * has been accounted for.
	 */
	rem_sect = tot_sectors - Fs->dir_len - Fs->fat_start;
	switch(abs(fat_bits)) {
		case 0:
			if((rem_sect - 32)/Fs->cluster_size > FAT12)
				/* big enough for FAT16:
				 * sectors minus 2 fats of 32
				 * bits */
				set_fat16(Fs);
			else if((rem_sect - 24)/Fs->cluster_size <= FAT12)
				set_fat12(Fs);
			else {
				/* "between two chairs",
				 * augment cluster size, and
				 * settle it */
				Fs->cluster_size <<= 1;
				set_fat12(Fs);
			}
			break;
		case 12:
			set_fat12(Fs);
			break;
		case 16:
			set_fat16(Fs);
			break;
		case 32:
			set_fat32(Fs);
			break;
	}
}

static inline void format_root(Fs_t *Fs, char *label, struct bootsector *boot)
{
	Stream_t *RootDir;
	char *buf;
	int i;
	struct ClashHandling_t ch;
	int dirlen;

	init_clash_handling(&ch);
	ch.name_converter = label_name;
	ch.ignore_entry = -2;

	buf = safe_malloc(Fs->sector_size);
	RootDir = open_root((Stream_t *)Fs);
	if(!RootDir){
		fprintf(stderr,"Could not open root directory\n");
		exit(1);
	}

	memset(buf, '\0', Fs->sector_size);

	if(Fs->fat_bits == 32)
		/* on a FAT32 system, we only write one sector,
		 * as the directory can be extended at will...*/
		dirlen = 1;
	else
		dirlen = Fs->dir_len; 
	for (i = 0; i < dirlen; i++)
		WRITES(RootDir, buf, i * Fs->sector_size,  Fs->sector_size);

	ch.ignore_entry = 1;
	if(label[0])
		mwrite_one(RootDir,label, 0, labelit, NULL,&ch);

	FREE(&RootDir);
	if(Fs->fat_bits == 32)
		set_word(boot->dirents, 0);
	else
		set_word(boot->dirents, Fs->dir_len * (Fs->sector_size / 32));
}


static void xdf_calc_fat_size(Fs_t *Fs, unsigned int tot_sectors, int fat_bits)
{
	unsigned int rem_sect;

	rem_sect = tot_sectors - Fs->dir_len - Fs->fat_start - 2 * Fs->fat_len;

	if(Fs->fat_len) {
		/* an XDF disk, we know the fat_size and have to find
		 * out the rest. We start with a cluster size of 1 and
		 * keep doubling until everything fits into the
		 * FAT. This will occur eventually, as our FAT has a
		 * minimal size of 1 */
		for(Fs->cluster_size = 1; 1 ; Fs->cluster_size <<= 1) {
			Fs->num_clus = rem_sect / Fs->cluster_size;
			if(abs(fat_bits) == 16 || Fs->num_clus > FAT12)
				set_fat16(Fs);
			else
				set_fat12(Fs);
			if (Fs->fat_len >= NEEDED_FAT_SIZE(Fs))
				return;
		}
	}
	fprintf(stderr,"Internal error while calculating Xdf fat size\n");
	exit(1);
}


static void calc_fat_size(Fs_t *Fs, unsigned int tot_sectors)
{
	unsigned int rem_sect;
	int tries;
	int occupied;
	
	tries=0;
	/* rough estimate of fat size */
	Fs->fat_len = 1;
	rem_sect = tot_sectors - Fs->dir_len - Fs->fat_start;
	while(1){
		Fs->num_clus = (rem_sect - 2 * Fs->fat_len ) /Fs->cluster_size;
		Fs->fat_len = NEEDED_FAT_SIZE(Fs);
		occupied = 2 * Fs->fat_len + Fs->cluster_size * Fs->num_clus;
		
		/* if we have used up more than we have,
		 * we'll have to reloop */
		
		if ( occupied > rem_sect )
			continue;


		/* if we have exactly used up all
		 * sectors, fine */
		if ( rem_sect - occupied < Fs->cluster_size )
			break;

		/* if we have not used up all our
		 * sectors, try again.  After the second
		 * try, decrease the amount of available
		 * space. This is to deal with the case of
		 * 344 or 345, ..., 1705, ... available
		 * sectors.  */
		
		switch(tries++){
			default:
				/* this should never happen */
				fprintf(stderr,
					"Internal error in cluster/fat repartition"
					" calculation.\n");
				exit(1);
			case 2:
				/* FALLTHROUGH */
			case 1:
				rem_sect-= Fs->cluster_size;
				Fs->dir_len += Fs->cluster_size;
			case 0:
				continue;
		}
	}

	if ( Fs->num_clus > FAT12 && Fs->fat_bits == 12 ){
		fprintf(stderr,"Too many clusters for this fat size."
			" Please choose a 16-bit fat in your /etc/mtools"
			" or .mtoolsrc file\n");
		exit(1);
	}
}


static unsigned char bootprog[]=
{0xfa, 0x31, 0xc0, 0x8e, 0xd8, 0x8e, 0xc0, 0xfc, 0xb9, 0x00, 0x01,
 0xbe, 0x00, 0x7c, 0xbf, 0x00, 0x80, 0xf3, 0xa5, 0xea, 0x00, 0x00,
 0x00, 0x08, 0xb8, 0x01, 0x02, 0xbb, 0x00, 0x7c, 0xba, 0x80, 0x00,
 0xb9, 0x01, 0x00, 0xcd, 0x13, 0x72, 0x05, 0xea, 0x00, 0x7c, 0x00,
 0x00, 0xcd, 0x19};

static inline void inst_boot_prg(struct bootsector *boot)
{
	int offset = WORD(ext.old.BootP);
	memcpy((char *) boot->jump + offset, 
	       (char *) bootprog, sizeof(bootprog) /sizeof(bootprog[0]));
	boot->jump[0] = 0xeb;
	boot->jump[1] = offset + 1;
	boot->jump[2] = 0x90;
	set_word(boot->jump + offset + 20, offset + 24);
}

static void calc_cluster_size(struct Fs_t *Fs, unsigned int tot_sectors,
			      int fat_bits)
			      
{
	unsigned int term, mult, rem_sect;

	switch(abs(fat_bits)) {
		case 12:			
			mult = FAT12;
			term = MAX_FAT12_FATLEN(Fs->sector_size);
			break;
		case 16:
		case 0: /* still hesititating between 12 and 16 */
			mult = FAT16;
			term = MAX_FAT16_FATLEN(Fs->sector_size);
			break;
		case 32:
			Fs->cluster_size = 1;
			return;
		default:
			fprintf(stderr,"Bad fat size\n");
			exit(1);
	}

	rem_sect = tot_sectors - Fs->dir_len - Fs->fat_start;
	while(Fs->cluster_size * mult  + term < rem_sect) {
		if(Fs->cluster_size > 64) {
			/* bigger than 64. Should fit */
			fprintf(stderr,
				"Internal error while calculating cluster size\n");
			exit(1);
		}
		Fs->cluster_size <<= 1;
	}
}


struct OldDos_t old_dos[]={
{   40,  9,  1, 4, 1, 2, 0xfc },
{   40,  9,  2, 7, 2, 2, 0xfd },
{   40,  8,  1, 4, 1, 1, 0xfe },
{   40,  8,  2, 7, 2, 1, 0xff },
{   80,  9,  2, 7, 2, 3, 0xf9 },
{   80, 15,  2,14, 1, 7, 0xf9 },
{   80, 18,  2,14, 1, 9, 0xf0 },
{   80, 36,  2,15, 2, 9, 0xf0 },
{    1,  8,  1, 1, 1, 1, 0xf0 },
};

static void calc_fs_parameters(struct device *dev, unsigned int tot_sectors,
			       struct Fs_t *Fs, struct bootsector *boot)
{
	int i;

	for(i=0; i < sizeof(old_dos) / sizeof(old_dos[0]); i++){
		if (dev->sectors == old_dos[i].sectors &&
		    dev->tracks == old_dos[i].tracks &&
		    dev->heads == old_dos[i].heads &&
		    (dev->fat_bits == 0 || abs(dev->fat_bits) == 12)){
			boot->descr = old_dos[i].media;
			Fs->cluster_size = old_dos[i].cluster_size;
			Fs->dir_len = old_dos[i].dir_len;
			Fs->fat_len = old_dos[i].fat_len;
			Fs->fat_bits = 12;
			break;
		}
	}
	if (i == sizeof(old_dos) / sizeof(old_dos[0]) ){
		/* a non-standard format */
		if(DWORD(nhs))
			boot->descr = 0xf8;
		  else
			boot->descr = 0xf0;


		if(!Fs->cluster_size) {
			if (dev->heads == 1)
				Fs->cluster_size = 1;
			else {
				Fs->cluster_size = (tot_sectors > 2000 ) ? 1:2;
				if (dev->use_2m & 0x7f)
					Fs->cluster_size = 1;
			}
		}
		
		if(!Fs->dir_len) {
			if (dev->heads == 1)
				Fs->dir_len = 4;
			else
				Fs->dir_len = (tot_sectors > 2000) ? 11 : 7;
		}			

		calc_cluster_size(Fs, tot_sectors, dev->fat_bits);
		if(Fs->fat_len)
			xdf_calc_fat_size(Fs, tot_sectors, dev->fat_bits);
		else {
			calc_fat_bits2(Fs, tot_sectors, dev->fat_bits);
			calc_fat_size(Fs, tot_sectors);
		}
	}

	set_word(boot->fatlen, Fs->fat_len);
}



static void calc_fs_parameters_32(unsigned int tot_sectors,
				  struct Fs_t *Fs, struct bootsector *boot)
{
	if(DWORD(nhs))
		boot->descr = 0xf8;
	else
		boot->descr = 0xf0;
	if(!Fs->cluster_size)
		/* FAT32 disks have a cluster size of 1 by default.
		 * After all, we can afford it, can't we? */		
		Fs->cluster_size = 1;
	
	Fs->dir_len = 0;
	Fs->num_clus = tot_sectors / Fs->cluster_size;
	set_fat32(Fs);
	calc_fat_size(Fs, tot_sectors);
	set_word(boot->fatlen, 0);
	set_dword(boot->ext.fat32.bigFat, Fs->fat_len);
}




static void usage(void)
{
	fprintf(stderr, 
		"Mtools version %s, dated %s\n", mversion, mdate);
	fprintf(stderr, 
		"Usage: %s [-V] [-t tracks] [-h heads] [-s sectors] "
		"[-l label] [-n serialnumber] "
		"[-S hardsectorsize] [-M softsectorsize] [-1]"
		"[-2 track0sectors] [-0 rate0] [-A rateany] [-a]"
		"device\n", progname);
	exit(1);
}

void mformat(int argc, char **argv, int dummy)
{
	Fs_t Fs;
	int hs, hs_set;
	int arguse_2m = 0;
	int sectors0=18; /* number of sectors on track 0 */
	int create = 0;
	int rate_0, rate_any;
	int mangled;
	int argssize=0; /* sector size */
	int msize=0;
	int fat32 = 0;

#ifdef USE_XDF
	int i;
	int format_xdf = 0;
	struct xdf_info info;
#endif
	struct bootsector *boot;
	char *bootSector=0;
	int c;
	int keepBoot = 0;
	struct device used_dev;
	int argtracks, argheads, argsectors;
	int tot_sectors;

	char drive, name[EXPAND_BUF];

	char label[VBUFSIZE], buf[MAX_SECTOR], shortlabel[13];
	struct device *dev;
	char errmsg[200];

	unsigned long serial;
 	int serial_set;
	int fsVersion;

	int Atari = 0; /* should we add an Atari-style serial number ? */
 
	hs = hs_set = 0;
	argtracks = 0;
	argheads = 0;
	argsectors = 0;
	arguse_2m = 0;
	argssize = 0x2;
	label[0] = '\0';
	serial_set = 0;
	serial = 0;
	fsVersion = 0;
	
	Fs.cluster_size = 0;
	Fs.refs = 1;
	Fs.dir_len = 0;
	Fs.fat_len = 0;
	Fs.Class = &FsClass;	
	rate_0 = mtools_rate_0;
	rate_any = mtools_rate_any;

	/* get command line options */
	while ((c = getopt(argc,argv,
			   "B:kr:IFCc:Xt:h:s:l:n:H:M:S:12:0Aaf:"))!= EOF) {
		switch (c) {
			case 'k':
				keepBoot = 1;
				break;
			case 'B':
				bootSector = optarg;
				break;
			case 'r': 
				Fs.dir_len = strtoul(optarg,0,0);
				break;
			case 'f':
				Fs.fat_len = strtoul(optarg,0,0);
				break;
			case 'F':
				fat32 = 1;
				break;
			case 'I':
				fsVersion = strtoul(optarg,0,0);
				break;
			case 'C':
				create = O_CREAT;
				break;
			case 'c':
				Fs.cluster_size = atoi(optarg);
				break;
			case 'H':
				hs = atoi(optarg);
				hs_set = 1;
				break;
#ifdef USE_XDF
			case 'X':
				format_xdf = 1;
				break;
#endif
			case 't':
				argtracks = atoi(optarg);
				break;
			case 'h':
				argheads = atoi(optarg);
				break;
			case 's':
				argsectors = atoi(optarg);
				break;
			case 'l':
				strncpy(label, optarg, VBUFSIZE-1);
				label[VBUFSIZE-1] = '\0';
				break;
 			case 'n':
 				serial = strtoul(optarg,0,0);
 				serial_set = 1;
 				break;
			case 'S':
				argssize = atoi(optarg) | 0x80;
				if(argssize < 0x81)
					usage();
				break;
			case 'M':
				msize = atoi(optarg);
				if (msize % 256 || msize > 8192 )
					usage();
				break;
			case '1':
				arguse_2m = 0x80;
				break;
			case '2':
				arguse_2m = 0xff;
				sectors0 = atoi(optarg);
				break;
			case '0': /* rate on track 0 */
				rate_0 = atoi(optarg);
				break;
			case 'A': /* rate on other tracks */
				rate_any = atoi(optarg);
				break;
			case 'a': /* Atari style serial number */
				Atari = 1;
				break;
			default:
				usage();
		}
	}

	if (argc - optind != 1 ||
	    !argv[optind][0] || argv[optind][1] != ':')
		usage();

#ifdef USE_XDF
	if(create && format_xdf) {
		fprintf(stderr,"Create and XDF can't be used together\n");
		exit(1);
	}
#endif
	
	drive = toupper(argv[argc -1][0]);

	/* check out a drive whose letter and parameters match */	
	sprintf(errmsg, "Drive '%c:' not supported", drive);	
	Fs.Direct = NULL;
	for(dev=devices;dev->drive;dev++) {
		FREE(&(Fs.Direct));
		/* drive letter */
		if (dev->drive != drive)
			continue;
		used_dev = *dev;

		SET_INT(used_dev.tracks, argtracks);
		SET_INT(used_dev.heads, argheads);
		SET_INT(used_dev.sectors, argsectors);
		SET_INT(used_dev.use_2m, arguse_2m);
		SET_INT(used_dev.ssize, argssize);
		if(hs_set)
			used_dev.hidden = hs;
		
		expand(dev->name, name);
#ifdef USING_NEW_VOLD
		strcpy(name, getVoldName(dev, name));
#endif

#ifdef USE_XDF
		if(!format_xdf)
#endif
			Fs.Direct = SimpleFileOpen(&used_dev, dev, name,
						   O_RDWR | create,
						   errmsg, 0);
#ifdef USE_XDF
		else {
			used_dev.use_xdf = 1;
			Fs.Direct = XdfOpen(&used_dev, name, O_RDWR,
					    errmsg, &info);
			if(Fs.Direct && !Fs.fat_len)
				Fs.fat_len = info.FatSize;
			if(Fs.Direct && !Fs.dir_len)
				Fs.dir_len = info.RootDirSize;
		}
#endif

		if (!Fs.Direct)
			continue;

		/* non removable media */
		if (!used_dev.tracks || !used_dev.heads || !used_dev.sectors){
			sprintf(errmsg, 
				"Non-removable media is not supported "
				"(You must tell the complete geometry "
				"of the disk, either in /etc/mtools or "
				"on the command line) ");
			continue;
		}

#if 0
		/* set parameters, if needed */
		if(SET_GEOM(Fs.Direct, &used_dev, 0xf0, boot)){
			sprintf(errmsg,"Can't set disk parameters: %s", 
				strerror(errno));
			continue;
		}
#endif
		Fs.sector_size = 512;
		if( !(used_dev.use_2m & 0x7f))
			Fs.sector_size = 128 << (used_dev.ssize & 0x7f);
		SET_INT(Fs.sector_size, msize);

		/* do a "test" read */
		if (!create &&
		    READS(Fs.Direct, (char *) buf, 0, Fs.sector_size) != 
		    Fs.sector_size) {
			sprintf(errmsg, 
				"Error reading from '%s', wrong parameters?",
				name);
			continue;
		}
		break;
	}


	/* print error msg if needed */	
	if ( dev->drive == 0 ){
		FREE(&Fs.Direct);
		fprintf(stderr,"%s: %s\n", argv[0],errmsg);
		exit(1);
	}

	/* the boot sector */
	boot = (struct bootsector *) buf;
	if(bootSector) {
		int fd;

		fd = open(bootSector, O_RDONLY);
		if(fd < 0) {
			perror("open boot sector");
			exit(1);
		}
		read(fd, buf, Fs.sector_size);
		keepBoot = 1;
	}
	if(!keepBoot)
		memset((char *)boot, '\0', Fs.sector_size);
	set_dword(boot->nhs, used_dev.hidden);

	Fs.Next = buf_init(Fs.Direct,
			   Fs.sector_size * used_dev.heads * used_dev.sectors,
			   Fs.sector_size,
			   Fs.sector_size * used_dev.heads * used_dev.sectors);

	boot->nfat = Fs.num_fat = 2;
	if(!keepBoot)
		set_word(boot->jump + 510, 0xaa55);
	
	/* get the parameters */
	tot_sectors = used_dev.tracks * used_dev.heads * used_dev.sectors - 
		DWORD(nhs);

	set_word(boot->nsect, dev->sectors);
	set_word(boot->nheads, dev->heads);

	dev->fat_bits = comp_fat_bits(&Fs,dev->fat_bits, tot_sectors, fat32);

	if(dev->fat_bits == 32) {
		Fs.primaryFat = 0;
		Fs.writeAllFats = 1;
		Fs.fat_start = 3;
		calc_fs_parameters_32(tot_sectors, &Fs, boot);

		Fs.clus_start = Fs.num_fat * Fs.fat_len + Fs.fat_start;

		/* extension flags: mirror fats, and use #0 as primary */
		set_word(boot->ext.fat32.extFlags,0);

		/* fs version.  What should go here? */
		set_word(boot->ext.fat32.fsVersion,fsVersion);

		/* root directory */
		set_dword(boot->ext.fat32.rootCluster, Fs.rootCluster = 2);

		/* info sector */
		set_word(boot->ext.fat32.infoSector, Fs.infoSectorLoc = 2);

		/* no backup boot sector */
		set_word(boot->ext.fat32.backupBoot, 1);
	} else {
		Fs.fat_start = 1;
		calc_fs_parameters(&used_dev, tot_sectors, &Fs, boot);
		boot->ext.old.physdrive = 0x00;
		boot->ext.old.reserved = 0;
		boot->ext.old.dos4 = 0x29;
		Fs.dir_start = Fs.num_fat * Fs.fat_len + Fs.fat_start;
		Fs.clus_start = Fs.dir_start + Fs.dir_len;

		if (!serial_set || Atari)
			srandom(time (0));
		if (!serial_set)
			serial=random();
		set_dword(boot->ext.old.serial, serial);	
		if(!label[0])
			strncpy(shortlabel, "NO NAME    ",11);
		else
			label_name(label, 0, &mangled, shortlabel);
		strncpy(boot->ext.old.label, shortlabel, 11);
		sprintf(boot->ext.old.fat_type, "FAT%2.2d   ", Fs.fat_bits);
	}

	set_word(boot->secsiz, Fs.sector_size);
	boot->clsiz = (unsigned char) Fs.cluster_size;
	set_word(boot->nrsvsect, Fs.fat_start);

	init_geometry_boot(boot, &used_dev, sectors0, rate_0, rate_any,
			   &tot_sectors, keepBoot);
	if(Atari) {
		boot->banner[4] = 0;
		boot->banner[5] = random();
		boot->banner[6] = random();
		boot->banner[7] = random();
	}		

	if (create) {
		WRITES(Fs.Direct, (char *) buf,
		       Fs.sector_size * (tot_sectors-1),
		       Fs.sector_size);
	}

	if(!keepBoot)
		inst_boot_prg(boot);
	if(dev->use_2m & 0x7f)
		Fs.num_fat = 1;
	zero_fat(&Fs, boot->descr);
	if(Fs.fat_bits == 32) {
		/* initialize info sector */
		Fs.infoSector = (InfoSector_t *) safe_malloc(Fs.sector_size);
		set_dword(Fs.infoSector->signature, INFOSECT_SIGNATURE);
		set_dword(Fs.infoSector->count, Fs.num_clus);
		set_dword(Fs.infoSector->pos, 2);

		/* this will trigger writing it */
		Fs.fat_dirty = 1;

		/* root directory: allocate one sector */
		Fs.fat_encode(&Fs, Fs.rootCluster, Fs.end_fat);
	} else
		Fs.infoSector = 0;

#ifdef USE_XDF
	if(format_xdf)
		for(i=0; 
		    i < (info.BadSectors+Fs.cluster_size-1)/Fs.cluster_size; 
		    i++)
			Fs.fat_encode(&Fs, i+2, 0xfff7);
#endif

	format_root(&Fs, label, boot);
	WRITES((Stream_t *)&Fs, (char *) boot, 0, Fs.sector_size);
	if(Fs.fat_bits == 32 && WORD(ext.fat32.backupBoot) != MAX32) {
		WRITES((Stream_t *)&Fs, (char *) boot, 
		       WORD(ext.fat32.backupBoot) * Fs.sector_size,
		       Fs.sector_size);
	}
	FLUSH((Stream_t *)&Fs); /* flushes Fs. 
				 * This triggers the writing of the FAT */
#ifdef USE_XDF
	if(format_xdf && isatty(0) && !getenv("MTOOLS_USE_XDF"))
		fprintf(stderr,
			"Note:\n"
			"Remember to set the \"MTOOLS_USE_XDF\" environmental\n"
			"variable before accessing this disk\n\n"
			"Bourne shell syntax (sh, ash, bash, ksh, zsh etc):\n"
			" export MTOOLS_USE_XDF=1\n\n"
			"C shell syntax (csh and tcsh):\n"
			" setenv MTOOLS_USE_XDF 1\n" );	
#endif
	exit(0);
}
