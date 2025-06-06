/*
 * A program to create a manifest (shipping list) that is a shell script
 * to return a Unix file name to it's original state after it has been
 * clobbered by MSDOS's file name restrictions.
 *
 *	This code also used in arc, mtools, and pcomm
 */

#include "sysincludes.h"
#include "msdos.h"
#include "patchlevel.h"

static char *dos_name2();

void
main(argc, argv)
int argc;
char *argv[];
{
	int i;
	char *name, *new_name;

	/* print the version */
	if(argc >= 2 && strcmp(argv[1], "-V") == 0) {
		printf("Mtools version %s, dated %s\n", VERSION, DATE);
		exit(0);
	}

	if (argc == 1) {
		fprintf(stderr, "Usage: mkmanifest [-V] <list-of-files>\n");
		exit(1);
	}

	for (i=1; i<argc; i++) {
					/* zap the leading path */
		if ((name = strrchr(argv[i], '/')))
			name++;
		else
			name = argv[i];
					/* create new name */
		new_name = dos_name2(name);

		if (strcasecmp(new_name, name))
			printf("mv %s %s\n", new_name, name);
	}
	exit(0);
}

static char *
dos_name2(name)
char *name;
{
	static const char *dev[9] = {"con", "aux", "com1", "com2", "lpt1", 
				     "prn", "lpt2", "lpt3", "nul"};
	char *s;
	char *ext,*temp;
	char buf[MAX_PATH];
	int i, dot;
	static char ans[13];

	strcpy(buf, name);
	temp = buf;
					/* separate the name from extension */
	ext = 0;
	dot = 0;
	for (i=strlen(buf)-1; i>=0; i--) {
		if (buf[i] == '.' && !dot) {
			dot = 1;
			buf[i] = '\0';
			ext = &buf[i+1];
		}
		if (isupper(buf[i]))
			buf[i] = tolower(buf[i]);
	}
					/* if no name */
	if (*temp == '\0')
		strcpy(ans, "x");
	else {
		/* if name is a device */
		for (i=0; i<9; i++) {
			if (!strcasecmp(temp, dev[i])) 
				*temp = 'x';
		}
		/* name too long? */
		if (strlen(temp) > 8)
			*(temp+8) = '\0';
		/* extension too long? */
		if (ext && strlen(ext) > 3)
			*(ext+3) = '\0';
		/* illegal characters? */
		while ((s = strpbrk(temp, "^+=/[]:',?*\\<>|\". ")))
			*s = 'x';

		while (ext && (s = strpbrk(ext, "^+=/[]:',?*\\<>|\". ")))
			*s = 'x';	      
		strcpy(ans, temp);
	}
	if (ext && *ext) {
		strcat(ans, ".");
		strcat(ans, ext);
	}
	return(ans);
}
