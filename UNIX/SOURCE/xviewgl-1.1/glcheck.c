#include "patchlevel.h"
#include <stdio.h>
#ifdef STDLIB
#include <stdlib.h>
#endif
#include <string.h>
#include "my_types.h"
#include "libutils.h"

MY_FILE *program_fp;
char program_fname[FILENAME_MAX];
#define ABS(x) (((x)<0)?-(x):(x))

main(int argc, char **argv)
{
int i,j,numu;
char err_buf[132];
struct { unsigned start,end; char name[32]; } *used;
MY_FILE *fp;
    if (argc != 2) {
	fprintf(stderr,"Usage: %s file\n",argv[0]);
	exit(1);
    }
    if (!init_lib(argv[1],NULL,err_buf)) {
	fprintf(stderr,"%s: %s\n", argv[0], err_buf);
    }
    if (lib_files==NULL) {
	fprintf(stderr,"%s: %s is not a GRASP library\n",argv[0],argv[1]);
	exit(1);
    }
    used = malloc((lib_filecnt+1)*sizeof(*used));
    used[0].start = 0;
    used[0].end = (lib_filecnt+1)*17+2;
    strcpy(used[0].name,"header info");
    numu=1;
    printf("%d files in library %s\n",lib_filecnt,argv[1]);
    printf("Program (.txt) file name is %s.\n",program_fname);
    for (i=0;i<lib_filecnt;i++) {
	printf("File %s: starts at %d, size ",
	       lib_files[i].name,lib_files[i].offset);
	fp = find_file(lib_files[i].name);
	printf("%d\n",fp->bytesleft);
	for (j=numu;j>0;j--) {
	    if (used[j-1].start < lib_files[i].offset) {
		used[j].start = lib_files[i].offset;
		used[j].end = used[j].start + fp->bytesleft + 4;
		strcpy(used[j].name,lib_files[i].name);
		break;
	    }
	    used[j].start = used[j-1].start;
	    used[j].end = used[j-1].end;
	    strcpy(used[j].name,used[j-1].name);
	}
	numu++;
	my_fclose(fp);
    }
    for (i=0;i<(numu-1);i++) {
	if (used[i].end != used[i+1].start) {
	    printf("%s of %d bytes between %s and %s\n",
		   (used[i].end < used[i+1].start)?"Gap":"Overlap",
		   ABS((int)(used[i].end - used[i+1].start)),
		   used[i].name, used[i+1].name);
	}
    }
    exit(0);
}
