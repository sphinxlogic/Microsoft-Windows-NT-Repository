/*
 * mcd.c: Change MSDOS directories
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mainloop.h"
#include "mtools.h"

void mcd(int argc, char **argv, int type)
{
	Stream_t *Dir;
	FILE *fp;
	char pathname[MAX_PATH];
	struct MainParam_t mp;

	if (argc > 2) {
		fprintf(stderr, "Mtools version %s, dated %s\n", mversion, mdate);
		fprintf(stderr, "Usage: %s: [-V] msdosdirectory\n", argv[0]);
		exit(1);
	}

	init_mp(&mp);
	if (argc == 1) {
		printf("%s\n", mp.mcwd);
		exit(0);
	}
	mp.pathname = pathname;
	Dir = open_subdir(&mp, argv[1], O_RDONLY, 1, 0);
	if(!Dir){
		fprintf(stderr,"No such directory\n");
		exit(1);
	}

	FREE(&Dir);

	if (!(fp = open_mcwd("w"))){
		fprintf(stderr,"%s: Can't open mcwd file for write\n",argv[0]);
		exit(1);
	}
	fprintf(fp, "%c:%s\n", mp.drivename, pathname);
	fclose(fp);
	exit(0);
}
