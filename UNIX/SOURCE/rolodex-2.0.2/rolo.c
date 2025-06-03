/* rolo.c */
#ifdef UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#ifdef BSD
#include <sys/file.h>
#else
#include <fcntl.h>
#endif
#endif

#ifdef VMS
#include <types.h>
#include <file.h>
#include <stat.h>
#include <string.h>
#endif

#ifdef MSDOS
#include <fcntl.h>
#	ifdef MSC
#	include <sys	ypes.h>
#	endif	/* MSC */
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#endif

#include <stdio.h>
#include <ctype.h>
/* #include <sgtty.h> */
#include <signal.h>


#ifdef TMC
#include <ctools.h>
#else
#include "ctools.h"
#endif
#include "args.h"
#include "menu.h"
#include "mem.h"

#include "rolofilz.h"
#include "rolodefs.h"
#include "datadef.h"

#ifndef BSD
/* BSD Unix gives us these values but AT&T Unix, VMS and MSDOS don't */
/* Used with 'access(2)' syscall */
#define R_OK 04
#define F_OK 00
#define W_OK 02
#endif

#ifdef UNIX
#define	DEF_PERM 0600	/* Default permissions: All, for owner only */
#endif

#ifdef VMS
#define	DEF_PERM 0600	/* Default permissions: All, for owner only */
#endif

#ifdef MSDOS
#define	DEF_PERM (S_IREAD | S_IWRITE)	/* Default permissions: All */
#endif

#define MAX_LINES	60		/* Max number of lines/page */

static struct stat stat_buf;

static char rolodir[DIRPATHLEN];        /* directory where rolo data is */
static char filebuf[DIRPATHLEN];        /* stores result of homedir() */

int changed = 0;
int name_changed = 0;
int reorder_file = 0;
int rololocked = 0;
int in_search_mode = 0;
int read_only = 0;

char *rolo_emalloc (size) int size;

/* error handling memory allocator */

{
  char *rval;        
  char *malloc();
  if (0 == (rval = malloc((unsigned)size))) {        
     fprintf(stderr,"Fatal error:  out of memory\n");
     save_and_exit(-1);                                               
  }
  return(rval);
}  

        
char *copystr (s) char *s;

/* memory allocating string copy routine */


{
 char *copy;        
 if (s == 0) return(0);
 copy = rolo_emalloc(strlen(s) + 1);
 strcpy(copy,s);
 return(copy);
}

 
char *timestring ()

/* returns a string timestamp */

{
  char *s;        
  long timeval;
  long time();

  time(&timeval);
  s = ctime(&timeval);  
  s[strlen(s) - 1] = '\0';
  return(copystr(s));
}  
  

user_interrupt ()

/* if the user hits C-C (we assume he does it deliberately) */

{
#ifdef VMS
  delete(homedir(ROLOLOCK));
  delete(homedir(ROLOTEMP));
#else
  unlink(homedir(ROLOLOCK));
  unlink(homedir(ROLOTEMP));
#endif
  fprintf(stderr,"\nAborting rolodex, no changes since last save recorded\n");
  exit(-1);  
}  


user_eof ()

/* if the user hits C-D */

{
#ifdef VMS
  delete(homedir(ROLOLOCK));
#else
  unlink(homedir(ROLOLOCK));        
#endif
  fprintf(stderr,"\nUnexpected EOF on terminal. Saving rolodex and exiting\n");
  save_and_exit(-1);        
}


roloexit (rval) int rval;
{
  /* In case it hasn't been cleaned up, also clobber ROLOTEMP. */
  clear_the_screen();
  if (rololocked)
#ifdef VMS
  delete(homedir(ROLOTEMP));
  delete(homedir(ROLOLOCK));
  exit(rval==0?1:rval);
#else
  unlink(homedir(ROLOTEMP));
  unlink(homedir(ROLOLOCK));
  exit(rval);
#endif
}  
      
                                      
save_to_disk ()

/* move the old rolodex to a backup, and write out the new rolodex and */
/* a copy of the new rolodex (just for safety) */

{
  FILE *tempfp,*copyfp;        
  char *strcpy();
  char d1[DIRPATHLEN], d2[DIRPATHLEN];

#ifdef VMS
  /*
   * Since VMS keeps versions of files around, clobber any that are still
   * hanging around.  If appropriate access permissions aren't granted,
   * well, they'll just have to purge their directory...
   */
  delete(homedir(ROLOTEMP));	/* There will be a copy of this from earlier */

  if(!access(homedir(ROLOCOPY),R_OK))
	if(!access(homedir(ROLOCOPY),W_OK))	/* Implies delete permission */
	delete(homedir(ROLOCOPY));
#endif
  tempfp = fopen(homedir(ROLOTEMP),"w");
  if(tempfp == (FILE *)NULL)
     perror(ROLOTEMP);

  copyfp = fopen(homedir(ROLOCOPY),"w");
  if(copyfp == (FILE *)NULL)
     perror(ROLOCOPY);

  if (tempfp == (FILE *)NULL || copyfp == (FILE *)NULL) {
     fprintf(stderr,"Unable to write rolodex...\n");
     fprintf(stderr,"Any changes made have not been recorded\n");
     roloexit(-1);
  }

  /*
   * Be sure to set proper permissions on the temp,copy files *before* putting
   * data in them...
   */
#ifndef MSDOS
  if(chmod(homedir(ROLOTEMP),(int)stat_buf.st_mode))
	fprintf(stderr,"WARNING: Couldn't set permissions on %s\n",
		homedir(ROLOTEMP));

  if(chmod(homedir(ROLOCOPY),(int)stat_buf.st_mode))
	fprintf(stderr,"WARNING: Couldn't set permissions on %s\n",
		homedir(ROLOCOPY));
#endif	/* MSDOS */

  write_rolo(tempfp,copyfp);
  
  fclose(tempfp);    
  fclose(copyfp);

#ifdef UNIX
  unlink(strcpy(d1,homedir(ROLOBAK)));              
  link(strcpy(d1,homedir(ROLODATA)),strcpy(d2,homedir(ROLOBAK)));
  unlink(strcpy(d1,homedir(ROLODATA)));

  if(link(strcpy(d1,homedir(ROLOTEMP)),strcpy(d2,homedir(ROLODATA))))
  {
    	fprintf(stderr,"Link failed.  Revised rolodex is in %s\n",ROLOCOPY);
	roloexit(-1);
  }else
  {
   	unlink(strcpy(d1,homedir(ROLOTEMP)));
   	unlink(strcpy(d1,homedir(ROLOCOPY)));
  }
#endif

#ifdef VMS
  delete(homedir(ROLOBAK));

  if ((rename(strcpy(d1,homedir(ROLODATA)),strcpy(d2,homedir(ROLOBAK))) != 1) ||
      (rename(strcpy(d1,homedir(ROLOTEMP)),strcpy(d2,homedir(ROLODATA)))) != 1) {
     fprintf(stderr,"Rename failed.  Revised rolodex is in %s\n",ROLOCOPY);
     roloexit(-1);
  }else
	delete(homedir(ROLOCOPY));	/* Don't really need this anymore */
#endif

#ifdef MSDOS
  unlink(homedir(ROLOBAK));

  if (rename(strcpy(d1,homedir(ROLODATA)),strcpy(d2,homedir(ROLOBAK))) ||
      rename(strcpy(d1,homedir(ROLOTEMP)),strcpy(d2,homedir(ROLODATA)))) {
     fprintf(stderr,"Rename failed.  Revised rolodex is in %s\n",ROLOCOPY);
     perror(homedir(ROLODATA));
     perror(homedir(ROLOBAK));
     perror(homedir(ROLOTEMP));
     roloexit(-1);
  }else
	unlink(homedir(ROLOCOPY));	/* Don't really need this anymore */
#endif

  printf("Rolodex saved\n");

  sleep(1);
  changed = 0;
  name_changed = 0;
}
                   

save_and_exit (rval) int rval;
{
  if (changed)
	save_to_disk();
  else
#ifdef VMS
	delete(homedir(ROLOTEMP));
#endif
#ifdef UNIX
	unlink(homedir(ROLOTEMP));
#endif
#ifdef MSDOS
	unlink(homedir(ROLOTEMP));
#endif

  roloexit(rval);        
}
                 
#ifdef UNIX
extern struct passwd *getpwnam();

char *home_directory (name) char *name;
{
  struct passwd *pwentry;
  if (0 == (pwentry = getpwnam(name))) return("");
  return(pwentry -> pw_dir);
}
#endif

#ifdef VMS
/*
 * This routine should never be called in VMS.
 */
char *home_directory(name) char *name;
{
	fprintf(stderr,"INTERNAL ERROR: Called home_directory under VMS.\n");
	exit(0);
}
#endif

#ifdef MSDOS
/*
 * This is rather simplistic. For drives A through C, try to find a directory
 * named 'name' under the root.  If not found on any of the drives, then
 * simply return the root directory of the current drive.
 */
char *home_directory (name) char *name;
{
	char *drvspec = "A:\\";

	static char dirbuff[128];
	int fid;

	for(;*drvspec <= 'C';(*drvspec)++)
	{
		strcpy(dirbuff,drvspec);
		strcat(dirbuff,name);
		
		if((fid = open(dirbuff,O_RDONLY)) >= 0)
		{
			close(fid);
			return(dirbuff);
		}else
			if(errno != ENOENT && errno != ENODEV)
				return(dirbuff);
	}

      	return("/");
}                       
#endif

char *homedir (filename) char *filename;

/* e.g., given "rolodex.dat", create "/u/massar/rolodex.dat" */
/* rolodir generally the user's home directory but could be someone else's */
/* home directory if the -u option is used. */

{
#ifdef UNIX
  nbuffconcat(filebuf,3,rolodir,"/",filename);
#endif

#ifdef MSDOS
  nbuffconcat(filebuf,3,rolodir,"\\",filename);
#endif

#ifdef VMS
  nbuffconcat(filebuf,2,rolodir,filename);
#endif

  return(filebuf);
}


char *libdir (filename) char *filename;

/* return a full pathname into the rolodex library directory */
/* the string must be copied if it is to be saved! */

{
#ifdef UNIX
  nbuffconcat(filebuf,3,ROLOLIB,"/",filename);
#endif

#ifdef MSDOS
  /*
   * If there's no drive specifier in ROLOLIB, then prepend the current
   * drive.
   */
  char *curdir;
  char *getcwd();

  curdir = ROLOLIB;

  if(curdir[2] != ':')
  {
  	curdir = getcwd((char *)NULL,128);
	curdir[3] = '\0';
	nbuffconcat(filebuf,4,curdir,ROLOLIB,"\\",filename);
	(void)free(curdir);
  }else
	nbuffconcat(filebuf,3,ROLOLIB,"\\",filename);
#endif

#ifdef VMS
  nbuffconcat(filebuf,2,ROLOLIB,filename);
#endif

                 
  return(filebuf);        
}


rolo_only_to_read () 
{
  return((option_present(READONLYFLAG) << 1) |
	(option_present(SUMMARYFLAG) || n_non_option_args()));
}


locked_action () 
{
  if (option_present(OTHERUSERFLAG)) {        
     fprintf(stderr,"Someone else is modifying that rolodex, sorry\n");
     exit(-1);
  }
  else {
     cathelpfile(LOCKINFO,"locked rolodex",0);
     exit(-1);
  }
}  
  

pretty_print()
{
	int elt_lines, index, tmp_line_cnt;
	int line_cnt = 0;
	char *tmpptr;
	char *strchr();
	FILE *lstfp;
	Ptr_Rolo_List rptr;
	Ptr_Rolo_Entry lentry;

	clear_the_screen();

	fputs("\nPretty printing the Rolodex...\n",stdout);
	fprintf(stdout,"Print the file %-30s\n", homedir(ROLOPRINT));

	lstfp = fopen(homedir(ROLOPRINT),"w");
	if(lstfp == (FILE *)NULL) {
		perror(homedir(ROLOPRINT));
		return;
	}

	rptr = Begin_Rlist;
	if(rptr == 0) {
		fputs("\n\nNo entries to print...\n",stderr);
		return;
	}

	while (rptr != 0) {
  		lentry = get_entry(rptr);

		/* Find the number of lines this entry requires to print */
		for(elt_lines = 0, index = 0; index < N_BASIC_FIELDS; index++) {
			tmpptr = lentry->basicfields[index];
			elt_lines++;	/* Even one for an empty line */

			while((tmpptr = strchr(tmpptr,';')) != (char *)NULL) {
				elt_lines++;
				tmpptr++;
			}
		}
		if(lentry->n_others) {
			for(index=0; index < lentry->n_others; index++) {
				elt_lines++;	/* As above, even if the entry is empty... */
				tmpptr = lentry->other_fields[index];
                        	
                        	while((tmpptr = strchr(tmpptr,';')) != (char *)NULL) {
                        		elt_lines++;
                        		tmpptr++;
                        	}
			}
		}

		/*
		 * Check to see if there is enough room on the current page
		 * to print this entry... Otherwise advance to the top of the
		 * next page
		 */
		tmp_line_cnt = line_cnt + elt_lines;
		if(tmp_line_cnt > MAX_LINES) {
			fputc('\f',lstfp);
			line_cnt = 0;
		}

		/*
		 * We can now start the printing of the actual entry
		 */
		fputs("Name:               ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_NAME,lentry),lstfp,20);
		fputs("Home address:       ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_HOME_ADDRESS,lentry),lstfp,20);
		fputs("Home phone:         ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_HOME_PHONE,lentry),lstfp,20);
		fputs("Company:            ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_COMPANY,lentry),lstfp,20);
		fputs("Work address:       ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_WORK_ADDRESS,lentry),lstfp,20);
		fputs("Work phone:         ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_WORK_PHONE,lentry),lstfp,20);
		fputs("Remarks:            ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_REMARKS,lentry),lstfp,20);

		/* Now print any user defined fields */
		if(lentry->n_others) {
			fputs("*** User defined fields ***\n",lstfp);
			line_cnt++;
			for(index = 0;index < lentry->n_others; index++)
				fprintf(lstfp,"%-30s\n",get_other_field(index,lentry));
		}

		/* Print the date this entry was last updated */
		fputs("				LAST UPDATED:       ",lstfp);
		prt_dump(get_basic_rolo_field((int)R_UPDATED,lentry),lstfp,20);

		/* Print two(2) blank lines, and increment the line count */
		fputs("\n\n",lstfp);
		line_cnt += elt_lines;
		line_cnt += 2;		/* For the two blank lines between entries */
		rptr = get_next_link(rptr);
	}
	fclose(lstfp);
}

prt_dump(strptr, stream, offset)
char *strptr;
FILE *stream;
int offset;
{
	int i;

	if(*strptr == (char)NULL)
		fputs("<<NONE>>\n",stream);
	else {
		for(; *strptr != '\0'; strptr++) {
			if(*strptr == ';') {
				fputc('\n',stream);
				for(i = 0; i < offset; i++)
					fputc(' ',stream);
			}
			else
				fputc(*strptr,stream);
		}
		fputc('\n',stream);
	}
}

main (argc,argv) int argc; char *argv[];

{
    int fd,in_use,rolofd;
    Bool not_own_rolodex;        
    char *user = "";
    FILE *tempfp;
    
    clearinit();
    clear_the_screen();
    
    /* parse the options and arguments, if any */
    
    switch (get_args(argc,argv,T,T)) {
        case ARG_ERROR : 
          roloexit(-1);
        case NO_ARGS :
          break;
        case ARGS_PRESENT :
          if (ALL_LEGAL != legal_options(LEGAL_OPTIONS)) {
                fprintf(stderr,"illegal option\nusage: %s\n",USAGE);
                roloexit(-1);
          }
    }
    
    /* find the directory in which the rolodex file we want to use is */
    
    not_own_rolodex = option_present(OTHERUSERFLAG);        
    if (not_own_rolodex) {
       if (NIL == (user = option_arg(OTHERUSERFLAG,1)) || 
           n_option_args(OTHERUSERFLAG) != 1) {
          fprintf(stderr,"Illegal syntax using -u option\nusage: %s\n",USAGE);
          roloexit(-1);
       }
    }        
#ifndef MSDOS
    else {
       if (0 == (user = getenv("HOME"))) {
          fprintf(stderr,"Cant find your home directory, no HOME\n");
          roloexit(-1);
       }
    }
#endif
    if (not_own_rolodex) {
#ifndef MSDOS
       strcpy(rolodir,user);
#else
       strcpy(rolodir,home_directory(user));
#endif
       if (*rolodir == '\0') {
          fprintf(stderr,"No user %s is known to the system\n",user);
          roloexit(-1);
       }
    }
    else strcpy(rolodir,user);
    
    /* is the rolodex readable? */
    
    if (0 != access(homedir(ROLODATA),R_OK)) {
        
       /* No.  if it exists and we cant read it, that's an error */
        
       if (0 == access(homedir(ROLODATA),F_OK)) { 
          fprintf(stderr,"Cant access rolodex data file to read\n");
          roloexit(-1);
       }
       
       /* if it doesn't exist, should we create one? */
       /* Under Unix, only if it's our own.  On other systems, it depends.*/

#ifdef UNIX       
       if (option_present(OTHERUSERFLAG)) {
          fprintf(stderr,"No rolodex file belonging to %s found\n",user);
          roloexit(-1);
       }
#endif
       /* try to create it, only if its our own */
       
       if (-1 == (fd = creat(homedir(ROLODATA),DEF_PERM))) {
         if(option_present(OTHERUSERFLAG))
	   fprintf(stderr,"couldn't create rolodex in %s\n",homedir(ROLODATA));
	 else
           fprintf(stderr,"couldn't create rolodex in your home directory\n");

         roloexit(-1);
       }      
       
       else {
	 stat_buf.st_mode = DEF_PERM;	/* For later mode set */
         close(fd);
         fprintf(stderr,"Creating empty rolodex...\n");
       }

    }
    
    /* see if someone else is using it */
    
    in_use = (0 == access(homedir(ROLOLOCK),F_OK));
    
    /* are we going to access the rolodex only for reading? */
    
    if (!(read_only = rolo_only_to_read())) {
    
       /* No.  Make sure no one else has it locked. */
        
       if (in_use) {
          locked_action();
       }
        
       /* create a lock file.  Catch interrupts so that we can remove */
       /* the lock file if the user decides to abort */
               
       if (!option_present(NOLOCKFLAG)) {
          if ((fd = open(homedir(ROLOLOCK),O_EXCL|O_CREAT,00200|00400)) < 0) {
             fprintf(stderr,"unable to create lock file...\n");
#ifdef VMS
	     exit(0);
#else
	     exit(1);
#endif
	  }
          rololocked = 1;
          close(fd);
#ifdef MSDOS
#	ifdef MSC
	  signal(SIGINT,user_interupt);
#	else
	  ssignal(SIGINT,user_interrupt);	/* Very Turbo-ish */
#	endif	/* MSC */
#else
          signal(SIGINT,user_interrupt);
#endif
       }
        
       /* open a temporary file for writing changes to make sure we can */
       /* write into the directory */
       
       /* when the rolodex is saved, the old rolodex is moved to */
       /* a '~' file (on Unix), the temporary is made to be the new rolodex, */
       /* and a copy of the new rolodex is made */
       
       if (NULL == (tempfp = fopen(homedir(ROLOTEMP),"w"))) {
           fprintf(stderr,"Can't open temporary file to write to\n");
           roloexit(-1);
       }        
       fclose(tempfp);
    
    }
       
    allocate_memory_chunk(CHUNKSIZE);
    
    if (NULL == (rolofd = open(homedir(ROLODATA),O_RDONLY))) {
        fprintf(stderr,"Can't open rolodex data file to read\n");
        roloexit(-1);
    }
    
    /* Get the current rolodex file's permissions */
    if(fstat(rolofd,&stat_buf))
    {
	fprintf(stderr,"Can't fstat rolodex data file\n");
	roloexit(-1);
    }

    /* read in the rolodex from disk */
    /* It should never be out of order since it is written to disk ordered */
    /* but just in case... */
    
    if (!(read_only & 1)) printf("Reading in rolodex from %s\n",homedir(ROLODATA));
    read_rolodex(rolofd);
    close(rolofd);
    if (!(read_only & 1)) printf("%d entries listed\n",rlength(Begin_Rlist));
    if (reorder_file && !read_only) {
       fprintf(stderr,"Reordering rolodex...\n");
       rolo_reorder();
       fprintf(stderr,"Saving reordered rolodex to disk...\n");
       save_to_disk();
    }
       
    /* the following routines live in 'options.c' */
    
    /* -s option.  Prints a short listing of people and phone numbers to */
    /* standard output */
    
    if (option_present(SUMMARYFLAG)) {
        print_short();
#ifdef VMS
        exit(1);
#else
    	exit(0);         
#endif 
    }
    
    /* rolo <name1> <name2> ... */
    /* print out info about people whose names contain any of the arguments */
    
    if (n_non_option_args() > 0) {
       print_people();
#ifdef VMS
       exit(1);
#else
       exit(0);
#endif
    }
    
    /* regular rolodex program */
    
    interactive_rolo();
#ifdef VMS
    exit(1);
#else
    exit(0);
#endif
    
}

#ifdef MSC
sleep(sec)
int sec;
{
	register long tloc;
	long time();

	tloc = time((long *)NULL);

	tloc += (long)sec;

	while(time((long *)NULL) <= tloc);

	return(0);
}
#endif
