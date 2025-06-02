/*
 *  DIR by Richard Conn
 *
 * Revision History --
 *  DIR Version 1.1 -- Modified by Frank Wancho
 *		Several changes made to provide operation with MORE
 *  DIR Version 1.0 by Richard Conn
 *
 *	DIR is an enhanced directory display utility for UNIX.
 * It produces a sorted listing of file names and sizes in two columns,
 * and the files are sorted down the columns.  If a file is a directory,
 * its name is prefixed with a "D".  File count and size totals are displayed
 * at the bottom of the listing.
 *
 */

/*  C Libraries  */
#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#define	BUFSIZE	256

/*  Structure of a Directory Element as Stored for DIR  */
struct dirnode {
	char name[DIRSIZ];	/* Name of File */
	int dir;		/* Dir or Not? */
	off_t size;		/* Size in Bytes */
	short mode;		/* R/W/X */
	struct dirnode *next;	/* Ptr to Next File */
};

/*  Global Environment */
struct environ {
	int showhid;		/* Show Hidden Files? */
	int showrwx;		/* Show Attributes? */
	int filecnt;		/* Number of Files in List */
	off_t totsize;		/* Sum of All File Sizes */
	struct dirnode *first;	/* Ptr to First Elt in List */
	struct dirnode *last;	/* Ptr to Last Elt in List */
};

FILE	*out;

main(argc,argv)
char *argv[];
{
	char buf[BUFSIZE];
	struct environ env;
	int i, first;

	/* Init Environment */
	env.first = NULL;	/* No First Entry */
	env.showhid = 0;	/* No Hidden Files */
	env.showrwx = 0;	/* No R/W/X */
	env.filecnt = 0;	/* No Files */
	env.totsize = 0;	/* Accumulated File Size */

	/* Build Linked List of DIR Elements */
	if (argc == 1) build(".",&env,0);	/* current dir */
	else {
		first = 1;				/* first file is 1 */
		if (*argv[1] == '-') {
			first = 2;			/* first file is 2 */
			opts(argv[1],&env);
			if (argc == 2) build(".",&env,0);	/* curr dir */
		}
		for (i=first; i<argc; i++) build(argv[i],&env,0);	/* ea */
	}

	/* Sort Linked List of DIR Elements */
	sort(&env);

	/* Display Results */
	out=popen("more","w");
	display(&env);
	pclose(out);
}

/*  Process Command Options  */
opts(cmdstr,env)
char *cmdstr;
struct environ *env;
{
	while (*cmdstr != '\0')
		switch (*cmdstr++) {
		case 'A' :
		case 'a' :
			env->showrwx = 1;	/* show file attributes */
			break;
		case 'H' :
		case 'h' :
			env->showhid = 1;	/* show hidden files */
		default :
			break;
		}
}

/*  Display Entries in Linked List  */
display(env)
struct environ *env;
{
	struct dirnode *lptr, *rptr, *elt();
	char *fname;
	int i;

	if (env->filecnt > 0) {
		fprintf(out,"  -- Filename --  - Size -");
		if (env->showrwx) fprintf(out,"  Atr");
	}
	if (env->filecnt > 1) {
		fprintf(out,"      -- Filename --  - Size -");
		if (env->showrwx) fprintf(out,"  Atr");
	}
	fprintf(out,"\n");
	lptr = elt(0,env);	/* Pt to first element in left col */
	rptr = elt(env->filecnt%2 ? env->filecnt/2+1 : env->filecnt/2, env);
	for (i=0; i < env->filecnt/2; i++) {
		prelt(lptr,env);	/* Print Left Element */
		prelt(rptr,env);	/* Print Right Element */
		fprintf(out,"\n");	/* New Line */
		lptr = lptr->next;	/* Pt to next */
		rptr = rptr->next;
	}
	if (env->filecnt%2) {
		prelt(lptr,env);	/* Print Odd Element */
		fprintf(out,"\n");
	}
	fprintf(out,"           -- %d Entries Displayed, %ld Bytes --\n",
	env->filecnt, env->totsize);
}

/*  Print Element Pointed To  */
prelt(ptr,env)
struct dirnode *ptr;
struct environ *env;
{
	char *fname;
	int j;

	if (ptr->dir) fprintf(out,"D ");		/* Print Dir Flag */
	else fprintf(out,"  ");
	fname = ptr->name;
	for (j=0; j < DIRSIZ; j++)			/* Print File Name */
		if (*fname == '\0') fputc(' ',out);
		else fputc(*fname++,out);
	fprintf(out,"  %8ld", ptr->size);		/* Print File Size */
	if (env->showrwx) fprintf(out,"  %c%c%c",	/* Print RWX Flags */
	(ptr->mode & S_IREAD)  ? 'r' : '-',
	(ptr->mode & S_IWRITE) ? 'w' : '-',
	(ptr->mode & S_IEXEC)  ? 'x' : '-');
	fprintf(out,"    ");
}

/*  Shell Sort Directory Entries (See Pg 108 of K&R for Algorithm)  */
sort(env)
struct environ *env;
{
	int gap, i, j;
	struct dirnode dir, *eltj, *eltjg, *elt();

	for (gap = env->filecnt/2; gap > 0; gap /= 2)
		for (i = gap; i < env->filecnt; i++)
			for (j = i-gap; j >= 0; j -= gap) {
				eltj = elt(j,env);	/* pt to elt j */
				eltjg = elt(j+gap,env);	/* pt to elt j+gap */
				if (strcmp(eltj->name,eltjg->name) <= 0)
					break;
				/* temp = v[j] */
				*dir.name = '\0';	/* clear str */
				strcat(dir.name,eltj->name);
				dir.dir = eltj->dir;
				dir.size = eltj->size;
				dir.mode = eltj->mode;
				/* v[j] = v[j+gap] */
				*eltj->name = '\0';	/* clear str */
				strcat(eltj->name,eltjg->name);
				eltj->dir = eltjg->dir;
				eltj->size = eltjg->size;
				eltj->mode = eltjg->mode;
				/* v[j+gap] = temp */
				*eltjg->name = '\0';	/* clear str */
				strcat(eltjg->name,dir.name);
				eltjg->dir = dir.dir;
				eltjg->size = dir.size;
				eltjg->mode = dir.mode;
			}
}

/* Point to Nth (Starting at 0) Element in Linked List */
struct dirnode *elt(n,env)
int n;
struct environ *env;
{
	struct dirnode *rover;
	int i;

	rover = env->first;	/* pt to first element */
	for (i=0; i<n; i++) rover = rover->next;	/* adv thru list */
	return (rover);
}

/* Build Linked List Structure Containing Directory Entries */
build(name,env,level)
char *name;
struct environ *env;
int level;
{
	struct stat stbuf;
	struct dirnode dir;
	struct dirnode *calloc();
	char *nameptr;
	int i, ccnt;

	/* Check for File Existence */
	if (stat(name,&stbuf) == -1) {
		fprintf(stderr,"Can't find %s\n", name);
		return;
	}

	/* Check to See if File is a Directory */
	if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {	/* we have a dir */
		directory(name,env,level);
		if (!level) return;
	}

	/*  Check Hidden Entries */
	if (*name == '.' && !(env->showhid)) return;

	/*  Store Entry in Memory  */
	if (env->first == NULL) {	/* First Entry Processing */
		env->first = calloc(1, sizeof(dir));
		env->last = env->first;
	}
	else {				/* Nth Entry Processing */
		(*env->last).next = calloc(1, sizeof(dir));
		env->last = (*env->last).next;
	}
	if (env->last == NULL) {
		fprintf(stderr, "Dynamic Memory Overflow\n");
		exit(0);
	}

	/* Store Entry Values */
	env->filecnt++;		/* Increment File Count */
	(*env->last).next = NULL;
	nameptr = name;		/* Pt to first char of file name */
	ccnt = strlen(name);	/* Number of chars in file name */
	for (i=0; i < ccnt; i++)	/* Find Last Part of Name */
		if (*name++ == '/') nameptr = name;
	strcat((*env->last).name,nameptr);	/* Save File Name */
	(*env->last).size = stbuf.st_size;	/* Save File Size */
	(*env->last).mode = stbuf.st_mode;	/* Save Mode Bits */
	env->totsize += stbuf.st_size;		/* Increment Total Sizes */
	if ((stbuf.st_mode & S_IFMT) == S_IFDIR) 	/* Set Dir Flag */
		(*env->last).dir = 1;
	else (*env->last).dir = 0;
}

/* Process All Entries in a Directory */
directory(name,env,level)
char *name;
struct environ *env;
int level;
{
	struct direct dirbuf;
	char *nbp, *nep;
	char dirname[BUFSIZE];
	char filename[DIRSIZ];
	int i,fd;

	if (level) return;	/* don't recurse */

	/* Build Name of Directory into DIRNAME */
	nep = dirname;
	nbp = name;
	while (*nbp != '\0') *nep++ = *nbp++;
	*nep = '\0';	/* terminate string */
	if (nep+DIRSIZ+2 >= dirname+BUFSIZE)	/* name too long */
		return;

	/* Log Into Directory */
	if (chdir(dirname)) {
		fprintf(stderr, "Directory %s Not Found\n", dirname);
		return;
	}

	/* Open Directory File */
	if ((fd = open(".",0)) == -1) return;

	/* Read Through the Elements in the Directory */
	while (read(fd, (char *)&dirbuf, sizeof(dirbuf)) > 0) {
		if (dirbuf.d_ino == 0)	/* slot not in use */
			continue;
		for (i=0, nep=filename; i<DIRSIZ; i++)	/* build file name */
			*nep++ = dirbuf.d_name[i];
		*nep++ = '\0';
		build(filename,env,level+1);	/* reenter build for new file */
	}
	close(fd);
}

