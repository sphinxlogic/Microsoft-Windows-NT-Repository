/*
 * mdir.c:
 * Display an MSDOS directory
 */

#include "sysincludes.h"
#include "msdos.h"
#include "vfat.h"
#include "mtools.h"
#include "file.h"
#include "mainloop.h"
#include "fs.h"
#include "codepage.h"

static int concise;
static char newpath[MAX_PATH];
static char drive;

struct recurs_ptr {
	struct recurs_ptr *next;
	char *value;
} *recurs_ptr, **tail_ptr;


/*
 * Print an MSDOS directory date stamp.
 */
static void print_date(struct directory *dir)
{
	char year[5];
	char day[3];
	char month[3];
	char *p;

	sprintf(year, "%04d", DOS_YEAR(dir));
	sprintf(day, "%02d", DOS_DAY(dir));
	sprintf(month, "%02d", DOS_MONTH(dir));

	for(p=mtools_date_string; *p; p++) {
		if(!strncasecmp(p, "yyyy", 4)) {
			printf("%04d", DOS_YEAR(dir));
			p+= 3;
			continue;
		} else if(!strncasecmp(p, "yy", 2)) {
			printf("%02d", DOS_YEAR(dir) % 100);
			p++;
			continue;
		} else if(!strncasecmp(p, "dd", 2)) {
			printf("%02d", DOS_DAY(dir));
			p++;
			continue;
		} else if(!strncasecmp(p, "mm", 2)) {
			printf("%02d", DOS_MONTH(dir));
			p++;
			continue;
		}
		putchar(*p);
	}
}

/*
 * Print an MSDOS directory time stamp.
 */
static void print_time(struct directory *dir)
{
	char am_pm;
	int hour = DOS_HOUR(dir);
       
	if(!mtools_twenty_four_hour_clock) {
		am_pm = (hour >= 12) ? 'p' : 'a';
		if (hour > 12)
			hour = hour - 12;
		if (hour == 0)
			hour = 12;
	} else
		am_pm = ' ';

	printf("%2d:%02d%c", hour, DOS_MINUTE(dir), am_pm);
}

/*
 * Return a number in dotted notation
 */
static const char *dotted_num(unsigned long num, int width, char **buf)
{
	int      len;
	register char *srcp, *dstp;
	int size;

	size = width + width;
	*buf = malloc(size+1);

	if (*buf == NULL)
		return "";
	
	/* Create the number in maximum width; make sure that the string
	 * length is not exceeded (in %6ld, the result can be longer than 6!)
	 */
	sprintf(*buf, "%.*ld", size, num);

	for (srcp=*buf; srcp[1] != '\0'; ++srcp)
		if (srcp[0] == '0')
			srcp[0] = ' ';
		else
			break;
	
	len = strlen(*buf);
	srcp = (*buf)+len;
	dstp = (*buf)+len+1;

	for ( ; dstp >= (*buf)+4 && isdigit (srcp[-1]); ) {
		srcp -= 3;  /* from here we copy three digits */
		dstp -= 4;  /* that's where we put these 3 digits */
	}

	/* now finally copy the 3-byte blocks to their new place */
	while (dstp < (*buf) + len) {
		dstp[0] = srcp[0];
		dstp[1] = srcp[1];
		dstp[2] = srcp[2];
		if (dstp + 3 < (*buf) + len)
			/* use spaces instead of dots: they place both
			 * Americans and Europeans */
			dstp[3] = ' ';		
		srcp += 3;
		dstp += 4;
	}

	return (*buf) + len-width;
}

static void print_volume_label(Stream_t *Dir, char drive)
{
	Stream_t *Stream = GetFs(Dir);
	DeclareThis(FsPublic_t);
	Stream_t *RootDir;
	int entry;
	struct directory dir;
	char shortname[13];
	char longname[VBUFSIZE];

	if(concise)
		return;
	
	/* find the volume label */
	RootDir = open_root(Stream);
	entry = 0;
	if(vfat_lookup(RootDir, &dir, &entry, 0, 0,
		       ACCEPT_LABEL | MATCH_ANY,
		       NULL, shortname, longname) )
		printf(" Volume in drive %c has no label\n", drive);
	else if (*longname)
		printf(" Volume in drive %c is %s (abbr=%s)\n",
		       drive, longname, shortname);
	else
		printf(" Volume in drive %c is %s\n",
		       drive, shortname);
	if(This->serialized)
		printf(" Volume Serial Number is %04lX-%04lX\n",
		       (This->serial_number >> 16) & 0xffff, 
		       This->serial_number & 0xffff);
	FREE(&RootDir);
}


static int list_directory(Stream_t *Dir, char *newname, 
				 int wide, int all,
				 size_t *tot_size)
{
	int entry;
	struct directory dir;
	int files;
	size_t size;
	char shortname[13];
	char longname[VBUFSIZE];
	char outname[VBUFSIZE];
	int i;
	int Case;

	entry = 0;
	files = 0;

	if(!wide && !concise)
		printf("\n");

	while(vfat_lookup(Dir, &dir, &entry, 0, newname,
			  ACCEPT_DIR | ACCEPT_PLAIN,
			  outname, shortname, longname) == 0){
		if(!all && (dir.attr & 0x6))
			continue;
		if (wide) {
			if(files % 5)
				putchar(' ');				
			else
				putchar('\n');
		}
		files++;

		if(dir.attr & 0x10){
			size = 0;
		} else
			size = FILE_SIZE(&dir);

		Case = dir.Case;
		if(!(Case & (BASECASE | EXTCASE)) && mtools_ignore_short_case)
			Case |= BASECASE | EXTCASE;

		if(Case & EXTCASE){
			for(i=0; i<3;i++)
				dir.ext[i] = tolower(dir.ext[i]);
		}
		to_unix(dir.ext,3);
		if(Case & BASECASE){
			for(i=0; i<8;i++)
				dir.name[i] = tolower(dir.name[i]);
		}
		to_unix(dir.name,8);
		if(wide){
			if(dir.attr & 0x10)
				printf("[%s]%*s", shortname,
					(int) (15 - 2 - strlen(shortname)), "");
			else
				printf("%-15s", shortname);
		} else if(!concise) {				
			/* is a subdirectory */
			if(mtools_dotted_dir)
				printf("%-13s", shortname);
			else
				printf("%-8.8s %-3.3s ",dir.name, dir.ext);
			if(dir.attr & 0x10)
				printf("<DIR>    ");
			else
				printf(" %8ld", (long) size);
			printf(" ");
			print_date(&dir);
			printf("  ");
			print_time(&dir);

			if(*longname)
				printf(" %s", longname);
			printf("\n");
		} else {
			if(!strcmp(outname,".") || !strcmp(outname,".."))
				continue;
			if(newpath[0] && newpath[strlen(newpath)-1] == '/')
				printf("%c:%s%s", drive, newpath, outname);
			else
				printf("%c:%s/%s", drive, newpath, outname);
			if(dir.attr & 0x10) {
				(*tail_ptr) = New(struct recurs_ptr);
				(*tail_ptr)->next = 0;
				(*tail_ptr)->value = 
					malloc(strlen(newpath)+
					       strlen(outname)+4);
				(*tail_ptr)->value[0]=drive;
				(*tail_ptr)->value[1]=':';
				strcpy((*tail_ptr)->value+2,newpath);
				strcat((*tail_ptr)->value,"/");
				strcat((*tail_ptr)->value,outname);
				tail_ptr = &(*tail_ptr)->next;
				putchar('/');
			}
			putchar('\n');
			

		}
		*tot_size += size;
	}

	if(wide)
		putchar('\n');
	return files;
}

static void print_footer(char *drive, char *newname, int files,
			 size_t tot_size, size_t blocks)
{	
	char *s1,*s2;

	if(concise)
		return;

	if (*drive != '\0') {
		if (!files)
			printf("File \"%s\" not found\n"
			       "                     %s bytes free\n\n",
			       newname, dotted_num(blocks,13, &s1));
		else {
			printf("      %3d file(s)     %s bytes\n"
			       "                      %s bytes free\n\n",
			       files, dotted_num(tot_size,13, &s1),
			       dotted_num(blocks,13, &s2));
			if(s2)
				free(s2);
		}
		if(s1)
			free(s1);
	}
	*drive = '\0'; /* ensure the footer is only printed once */
}


static void usage(void)
{
		fprintf(stderr, "Mtools version %s, dated %s\n",
			mversion, mdate);
		fprintf(stderr, "Usage: %s: [-V] [-w] [-a] msdosdirectory\n",
			progname);
		fprintf(stderr,
			"       %s: [-V] [-w] [-a] msdosfile [msdosfiles...]\n",
			progname);
		exit(1);
}

void mdir(int argc, char **argv, int type)
{
	const char *arg;
	MainParam_t mp;
	int i, files, wide, all, faked;
	size_t blocks, tot_size=0;
	char last_drive;
	char newname[13];
	Stream_t *SubDir;
	Stream_t *Dir;
	int c;
	int fast=0;
	
	concise = 0;
	wide = all = files = blocks = 0;
					/* first argument */
	while ((c = getopt(argc, argv, "waXf")) != EOF) {
		switch(c) {
			case 'w':
				wide = 1;
				break;
			case 'a':
				all = 1;
				break;
			case 'X':
				concise = 1;
				break;
			case 'f':
				fast = 1;
				break;
			default:
				usage();
		}
	}

	/* fake an argument */
	faked = 0;
	if (optind == argc) {
		faked++;
		argc++;
	}

	tail_ptr = &recurs_ptr;

	init_mp(&mp);
	last_drive = '\0';
	for (i = optind; i < argc || recurs_ptr ; i++) {
		char *free_arg=0;
		if(i>=argc) {
			struct recurs_ptr *old;
			old = recurs_ptr;
			free_arg = recurs_ptr->value;
			arg = free_arg;
			recurs_ptr = recurs_ptr->next;
			if(!recurs_ptr)
				tail_ptr = &recurs_ptr;
			free(old);
		} else if (faked)
			arg="";
		else 
			arg = argv[i];
		Dir = open_subdir(&mp, arg, O_RDONLY, 0, 0);
		if(!Dir)
			continue;
		drive = mp.drivename;
		/* is this a new device? */
		if (drive != last_drive) {
			print_footer(&last_drive, newname, files, tot_size,
				     blocks);
			if(fast)
				blocks = 0;
			else
				blocks = getfree(Dir);
			files = 0;
			tot_size = 0;
			print_volume_label(Dir, drive);
		}
		last_drive = drive;

		/*
		 * Under MSDOS, wildcards that match directories don't
		 * display the contents of that directory.  So I guess I'll
		 * do that too.
		 */
		get_path(arg,newpath,mp.mcwd,  0);
		if (strpbrk(mp.filename, "*[?") == NULL &&
		    ((SubDir = descend(Dir, mp.filename, 0, 0, 0) )) ) {
			static char tmppath[MAX_PATH+5];

			/* Subdirectory */
			FREE(&Dir);
			Dir = SubDir;
#if 0
			if(*mp.filename){
				if (newpath[strlen(newpath) -1 ] != '/')
					strcat(newpath, "/");
				strcat(newpath, mp.filename);
			}
#endif
			strncpy(tmppath, arg, MAX_PATH+2);
			if(tmppath[0] != '\0' &&
			   (tmppath[1] !=':' || tmppath[2] != '\0'))
				strcat(tmppath,"/");
			get_path(tmppath,newpath,mp.mcwd,  0);	       
			*mp.filename='\0';
		}		
		
		if(concise && i<argc){
			printf("%c:%s",drive,newpath);
			if(newpath[0] != '/' || newpath[1])
				putchar('/');
			putchar('\n');
		}

		if(mp.filename[0]=='\0')
			strcpy(newname, "*");
		else
			strcpy(newname, mp.filename);

		if(!concise)
			printf(" Directory for %c:%s\n", drive, newpath);
		files += list_directory(Dir, newname, wide, all, &tot_size);
		FREE(&Dir);
		if(i>=argc)
			free(free_arg);
	}
	print_footer(&last_drive, newname, files, tot_size, blocks);
	exit(0);
}
