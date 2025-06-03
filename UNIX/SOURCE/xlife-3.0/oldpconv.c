#include <stdio.h>
#ifdef __svr4__
#include <string.h>
#else							    /* old way */
#include <strings.h>
#endif
#define MAXSTRING 256

/* Filter to make old #P files read in at the same coordinates
   as in version 2.0 */

main(argc,argv) 
int argc;
char *argv[];
{
static char convcomm[]="## converted to new format by oldpconv";
FILE *fptr,*wfptr;
char s[MAXSTRING];
char bakfile[MAXSTRING];
long x,y;
int linenum,i,converted;

  if (argc<2) {
      fprintf(stderr,
              "You must specify a list of files to be converted\n");
      exit(0);
  }


  for (i=1; i<argc; i++) {
     strcpy(bakfile,argv[i]); 
     strcat(bakfile,"~");

     if (fptr=fopen(bakfile,"r")) {
         fprintf(stderr,
                 "Can't create backup (would overwrite %s) -- exiting\n",bakfile);
         fclose(fptr); 
         exit(0);
     } else if (rename(argv[i],bakfile)) {
         fprintf(stderr, "Can't move %s to backup file -- exiting\n",argv[i]);
         exit(0);
     }

     if (!(fptr=fopen(bakfile,"r"))) {
         fprintf(stderr, "Can't open %s for reading -- exiting\n",bakfile);
         exit(0);
     }

     if (!(wfptr=fopen(argv[i],"w"))) {
         fprintf(stderr, "Can't open %s for writing -- exiting\n",argv[i]);
         fclose(fptr);
         exit(0);
     }

     converted=0;
     fprintf(wfptr,"%s\n",convcomm);
     linenum=0;
     while(!converted && fgets(s,MAXSTRING,fptr)) {
        if (s[0]=='#')
        {
          switch(s[1]) {
            case 'P':
              x=y=0; 
              sscanf(s+2,"%d %d",&x,&y);
              fprintf(wfptr,"#P %d %d\n",x,y+linenum); 
              break;
            case '#':
              if (!strncmp(s,convcomm,strlen(convcomm))) {
                 fprintf(stderr,"Looks like %s was already converted\n",argv[i]);
                 converted=1;
                 rename(bakfile,argv[i]);
              } 
             /* break omitted intentionally */
            default: 
              fprintf(wfptr,"%s",s); 
          }
        }
        else fprintf(wfptr,"%s",s); 

        linenum++;
     }
     fclose(fptr);
     fclose(wfptr);
  }
  exit(0);
}
