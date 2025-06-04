/*
 * mlabel.c
 * Make an MSDOS volume label
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mainloop.h"
#include "vfat.h"
#include "mtools.h"
#include "nameclash.h"


static void usage(void)
{
	fprintf(stderr, 
		"Mtools version %s, dated %s\n", mversion, mdate);
	fprintf(stderr, 
		"Usage: %s [-v] drive\n", progname);
	exit(1);
}


static void displayInfosector(Stream_t *Stream, struct bootsector *boot)
{
	InfoSector_t *infosec;

	if(WORD(ext.fat32.infoSector) == MAX32)
		return;

	infosec = (InfoSector_t *) safe_malloc(WORD(secsiz));
	force_read(Stream, (char *) infosec, 
		   WORD(secsiz) * WORD(ext.fat32.infoSector),
		   WORD(secsiz));
	printf("\nInfosector:\n");
	printf("signature=0x%08x\n", _DWORD(infosec->signature));
	if(_DWORD(infosec->count) != MAX32)
		printf("free clusters=%u\n", _DWORD(infosec->count));
	if(_DWORD(infosec->pos) != MAX32)
		printf("last allocated cluster=%u\n", _DWORD(infosec->pos));
}


void minfo(int argc, char **argv, int type)
{
	struct bootsector boot0;
#define boot (&boot0)
	char name[EXPAND_BUF];
	int media;
	int tot_sectors;
	struct device dev;
	char drive;
	int verbose=0;
	int c;
	Stream_t *Stream;
	
	while ((c = getopt(argc, argv, "v")) != EOF) {
		switch (c) {
			case 'v':
				verbose = 1;
				break;
			default:
				usage();
		}
	}

	if(argc == optind)
		usage();

	for(;optind < argc; optind++) {
		if(!argv[optind][0] || argv[optind][1] != ':')
			usage();
		drive = toupper(argv[optind][0]);

		if(! (Stream = find_device(drive, O_RDONLY, &dev, boot, 
					   name, &media)))
			exit(1);

		tot_sectors = DWORD(bigsect);
		SET_INT(tot_sectors, WORD(psect));
		printf("device information:\n");
		printf("===================\n");
		printf("sectors per track: %d\n", dev.sectors);
		printf("heads: %d\n", dev.heads);
		printf("cylinders: %d\n\n", dev.tracks);
		printf("mformat command line: mformat -t %d -h %d -s %d ",
		       dev.tracks, dev.heads, dev.sectors);
		if(DWORD(nhs))
			printf("-H %d ", DWORD(nhs));
		printf("%c:\n", tolower(drive));
		printf("\n");
		
		printf("bootsector information\n");
		printf("======================\n");
		printf("filename=\"%s\"\n", name);
		printf("banner:\"%8s\"\n", boot->banner);
		printf("sector size: %d bytes\n", WORD(secsiz));
		printf("cluster size: %d sectors\n", boot->clsiz);
		printf("reserved (boot) sectors: %d\n", WORD(nrsvsect));
		printf("fats: %d\n", boot->nfat);
		printf("max available root directory slots: %d\n", 
		       WORD(dirents));
		printf("small size: %d sectors\n", WORD(psect));
		printf("media descriptor byte: 0x%x\n", boot->descr);
		printf("sectors per fat: %d\n", WORD(fatlen));
		printf("sectors per track: %d\n", WORD(nsect));
		printf("heads: %d\n", WORD(nheads));
		printf("hidden sectors: %d\n", DWORD(nhs));
		printf("big size: %d sectors\n", DWORD(bigsect));

		if(WORD(fatlen)){
			printf("physical drive id: 0x%x\n", 
			       boot->ext.old.physdrive);
			printf("reserved=0x%x\n", 
			       boot->ext.old.reserved);
			printf("dos4=0x%x\n", 
			       boot->ext.old.dos4);
			printf("serial number: %X\n", 
			       DWORD(ext.old.serial));
			printf("disk label=\"%11.11s\"\n", 
			       boot->ext.old.label);
			printf("disk type=\"%8.8s\"\n", 
			       boot->ext.old.fat_type);
		} else {
			printf("Big fatlen=%u\n",
			       DWORD(ext.fat32.bigFat));
			printf("Extended flags=0x%04x\n",
			       WORD(ext.fat32.extFlags));
			printf("FS version=0x%04x\n",
			       WORD(ext.fat32.fsVersion));
			printf("rootCluster=%u\n",
			       DWORD(ext.fat32.rootCluster));
			if(WORD(ext.fat32.infoSector) != MAX32)
				printf("infoSector location=%d\n",
				       WORD(ext.fat32.infoSector));
			if(WORD(ext.fat32.backupBoot) != MAX32)
				printf("backup boot sector=%d\n",
				       WORD(ext.fat32.backupBoot));
			displayInfosector(Stream,boot);
		}

		if(verbose) {
			printf("\n");
			print_sector("Boot sector hexdump", 
				     (unsigned char*) boot);
		}
	}

	exit(0);
}
