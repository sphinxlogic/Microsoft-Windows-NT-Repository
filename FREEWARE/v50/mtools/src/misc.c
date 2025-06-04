/*
 * Miscellaneous routines.
 */

#include "sysincludes.h"
#include "msdos.h"
#include "stream.h"
#include "vfat.h"
#include "mtools.h"

/*
** 13-Mar-2000 Sergey Tikhonov
**	Added defines specific to OpenVMS
*/

#ifdef VMS
#define	LOC_MCWD    ".mcwd"
#define	TMP_MCWD    "sys$scratch:.mcwd"
#else
#define	LOC_MCWD    "/.mcwd"
#define	TMP_MCWD    "/.mcwd"
#endif


char *get_homedir(void)
{
	struct passwd *pw;
	int uid;
	char *homedir;
	char *username;
	
	homedir = getenv ("HOME");    
	/* 
	 * first we call getlogin. 
	 * There might be several accounts sharing one uid 
	 */
	if ( homedir )
		return homedir;
	
	pw = 0;
	
	username = getenv("LOGNAME");
	if ( !username )
		username = getlogin();
	if ( username )
		pw = getpwnam( username);
  
	if ( pw == 0 ){
		/* if we can't getlogin, look up the pwent by uid */
		uid = geteuid();
		pw = getpwuid(uid);
	}
	
	/* we might still get no entry */
	if ( pw )
		return pw->pw_dir;
	return 0;
}

FILE *open_mcwd(const char *mode)
{
    struct stat	sbuf;
    char	file[MAXPATHLEN+EXPAND_BUF];
    time_t	now;
    char	*mcwd_path;
    char    *homedir;

    mcwd_path = getenv("MCWD");
    if (mcwd_path == NULL || *mcwd_path == '\0')
    {
	homedir= get_homedir();
	if ( homedir )
	{
	    strncpy(file, homedir, MAXPATHLEN);
	    file[MAXPATHLEN]='\0';
	    strcat(file, LOC_MCWD);
	} else
	    strcpy(file, TMP_MCWD);
    } else
	expand(mcwd_path, file);

    if (*mode == 'r')
    {
	if (stat(file, &sbuf) < 0)
	    return NULL;
	/*
	 * Ignore the info, if the file is more than 6 hours old
	 */
	time(&now);
	if (now - sbuf.st_mtime > 6 * 60 * 60) {
	    fprintf(stderr, "Warning: \"%s\" is out of date, removing it\n",
		    file);
	    unlink(file);
	    return NULL;
	}
    }
    
    return  fopen(file, mode);
}
	

/* Fix the info in the MCWD file to be a proper directory name.
 * Always has a leading separator.  Never has a trailing separator
 * (unless it is the path itself).  */

const char *fix_mcwd(char *ans)
{
	FILE *fp;
	char *s;
	char buf[BUFSIZ];

	fp = open_mcwd("r");
	if(!fp){
		strcpy(ans, "A:/");
		return ans;
	}

	if (!fgets(buf, BUFSIZ, fp))
		return("A:/");

	buf[strlen(buf) -1] = '\0';
	fclose(fp);
					/* drive letter present? */
	s = buf;
	if (buf[0] && buf[1] == ':') {
		strncpy(ans, buf, 2);
		ans[2] = '\0';
		s = &buf[2];
	} else 
		strcpy(ans, "A:");
					/* add a leading separator */
	if (*s != '/' && *s != '\\') {
		strcat(ans, "/");
		strcat(ans, s);
	} else
		strcat(ans, s);

#if 0
					/* translate to upper case */
	for (s = ans; *s; ++s) {
		*s = toupper(*s);
		if (*s == '\\')
			*s = '/';
	}
#endif
					/* if only drive, colon, & separator */
	if (strlen(ans) == 3)
		return(ans);
					/* zap the trailing separator */
	if (*--s == '/')
		*s = '\0';
	return ans;
}

size_t maximize(size_t *target, long max)
{
	if(max < 0)
		max = 0;
	if (*target > max)
		*target = max;
	return *target;
}

size_t minimize(size_t *target, long min)
{
	if (*target < min)
		*target = min;
	return *target;
}


void *safe_malloc(size_t size)
{
	void *p;

	p = malloc(size);
	if(!p){
		fprintf(stderr,"Out of memory error\n");
		exit(1);
	}
	return p;
}

void print_sector(char *message, unsigned char *data)
{
	int col;
	int row;

	printf("%s:\n", message);
	
	for(row = 0; row < 32; row++){
		printf("%03x  ", row * 16);
		for(col = 0; col < 16; col++)
			printf("%02x ", data [row*16+col]);
		for(col = 0; col < 16; col++) {
			if(isprint(data [row*16+col]))
				printf("%c", data [row*16+col]);
			else
				printf(".");
		}
		printf("\n");
	}
}
