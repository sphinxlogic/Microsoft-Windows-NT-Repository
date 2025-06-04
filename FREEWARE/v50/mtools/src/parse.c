#include "sysincludes.h"
#include "msdos.h"
#include "vfat.h"

/*
 * Get name component of filename.
 *
 * Formerly translated name to upper case; now preserves case.
 * This is because long DOS names are case sensitive, but also because we
 * want to preserve the user-specified case for the copied Unix files
 */

char *get_name(const char *filename, char *ans, char *mcwd)
{
	char *s;
	const char *temp;

	temp = filename;
					/* skip drive letter */
	if (temp[0] && temp[1] == ':')
		temp = &temp[2];
					/* find the last separator */
	if ((s = strrchr(temp, '/')))
		temp = s + 1;
	if ((s = strrchr(temp, '\\')))
		temp = s + 1;

	strncpy(ans, temp, VBUFSIZE-1);
	ans[VBUFSIZE-1]='\0';

	return ans;
}

static void strip_last_dir(char *begin, char **targetp)
{
	char *tmp;

	/* IN condition: begin stops with a slash */

	/* remove slash */
	(*targetp) --;
	**targetp = '\0';

	tmp = strrchr(begin,'/');
	if(tmp) {
		tmp++;
	} else {
		begin[0]='/';
		tmp=begin+1;
	}
	*tmp = '\0';
	*targetp = tmp;
}

static int handle_dot(char *begin, char **sourcep, char **targetp)
{
	char *source = *sourcep;

	switch(source[1]) {
		case '/':
			(*sourcep) += 2;
			return 1;
		case '.':
			switch(source[2]) {
				case '\0':
					strip_last_dir(begin, targetp);
					(*sourcep) += 2;
					return 1;
				case '/':
					strip_last_dir(begin, targetp);
					(*sourcep) += 3;
					return 1;
				default:
					return 0;
			}
		case '\0':
			(*sourcep) ++;
			return 1;
		default:
			return 0;
	}
}


static void canonize_path(char *begin)
{
	char tmp, *source, *target;

	/* transform all backslashes into slashes */
	for(source = begin; *source; source++)
		if(*source == '\\')
			*source ='/';

	/* prune off double slashes */
	source = begin;
	target = begin;
	while(*source) {
		tmp = *target++ = *source++;
		if(tmp == '/') {
			while(1) {				
				switch(source[0]) {
					case '/':
						source++;
						continue;
					case '.':
						if(handle_dot(begin, &source,
							      &target))
							continue;
						break;
				}
				break;
			}
			if(!*source)
				target--;
		}
	}

	*target='\0';
	if(!*begin) {
		begin[0] = '/';
		begin[1] = '\0';
	}
}

/*
 * Get the path component of the filename.
 * Doesn't alter leading separator, always strips trailing separator (unless
 * it is the path itself).
 *
 * Formerly translated name to upper case; now preserves case.
 * This is because long DOS names are case sensitive, but also because we
 * want to preserve the user-specified case for the copied Unix files
 */

char *get_path(const char *filename, char *ans, char *mcwd, int mode)
{
	char *s;
	const char *end;
	const char *begin;
	char drive;

	begin = filename;

	/* skip drive letter */
	drive = '\0';
	if (begin[0] && begin[1] == ':') {
		drive = toupper(begin[0]);
		begin += 2;
	}

	ans[0] = '/';
	ans[1] = '\0';
	if ((*begin != '/') && (!drive || drive == *mcwd))
		strcpy(ans+1, mcwd + 2);

	strcat(ans, "/");

	/* cut last separator (if needed) */
	if(mode) 
		end = begin + strlen(begin);
	else {
		if ((s = strrchr(begin, '/')))
			end = s;
		else
			end = begin;
	}

	if(strlen(ans)+end-begin+1 > MAX_PATH){
		fprintf(stderr,"Path too long\n");
		exit(1);
	}

	strncat(ans, begin, end - begin);
	ans[strlen(ans)+end-begin] = '\0';
	canonize_path(ans);
	return ans;
}

/*
 * get the drive letter designation
 */

char get_drive(const char *filename, char def)
{
	if (*filename && *(filename + 1) == ':')
		return(toupper(*filename));
	else
		return def;
}
