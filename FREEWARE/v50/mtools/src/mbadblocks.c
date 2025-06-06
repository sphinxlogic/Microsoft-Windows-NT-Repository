/*
 * mbadblocks.c
 * Mark bad blocks on disk
 *
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "mainloop.h"
#include "fsP.h"

void mbadblocks(int argc, char **argv, int type)
{
	int i;
	char *in_buf;
	int in_len;
	off_t start;
	struct MainParam_t mp;
	Fs_t *Fs;
	Stream_t *Dir;
	int ret;

	if (argc != 2) {
		fprintf(stderr, "Mtools version %s, dated %s\n", 
			mversion, mdate);
		fprintf(stderr, "Usage: %s [-V] device\n", argv[0]);
		exit(1);
	}

	init_mp(&mp);

	Dir = open_subdir(&mp, argv[1], O_RDWR, 0, 0);
	if (!Dir) {
		fprintf(stderr,"%s: Cannot initialize drive\n", argv[0]);
		exit(1);
	}

	Fs = (Fs_t *)GetFs(Dir);
	in_len = Fs->cluster_size * Fs->sector_size;
	in_buf = safe_malloc(in_len);
	for(i=0; i < Fs->clus_start; i++ ){
		ret = READS(Fs->Next, in_buf, i * Fs->sector_size, 
			   Fs->sector_size);
		if( ret < 0 ){
			perror("early error");
			exit(1);
		}
		if(ret < Fs->sector_size){
			fprintf(stderr,"end of file in file_read\n");
			exit(1);
		}
	}
		
	in_len = Fs->cluster_size * Fs->sector_size;
	for(i=2; i< Fs->num_clus + 2; i++){
		if(Fs->fat_decode((Fs_t*)Fs,i))
			continue;
		start = (i - 2) * Fs->cluster_size + Fs->clus_start;
		ret = READS(Fs->Next, in_buf, start * Fs->sector_size, in_len);
		if(ret < in_len ){
			printf("Bad cluster %d found\n", i);
			Fs->fat_encode((Fs_t*)Fs, i, 0xfff7);
			continue;
		}
	}
	FREE(&Dir);
	exit(0);
}
