#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *usage = "usage: ld: [-eg] [-f file] [options]";
/*
	only question here is:
		how many extra arguments do we add for microsoft
			link parameters not given in unix
		how do we map non-mappable unix parameters
*/

main(argc, argv) 
	int argc;
	char **argv;
{
	int ch;
	extern
		opterr,
		optind;
	extern char
		*optarg;
	register
		i;

	opterr = 0;
	fprintf (stderr, "link\n");
	for (i = 1; i < argc; i++)
		if (argv [i] [0] != '-')
			fprintf (stderr, "%s\n", argv [i]);
	while ((ch = getopt(argc, argv, "o:s:u:v:")) != EOF)
	{
		switch(ch) {
		/*
			output file
		*/
		case 'o':		
			fprintf (stderr, "/OUT:%s\n", optarg);
			break;
		/*
			stack size
		*/
		case 's':		
			fprintf (stderr, "/STACK:%s\n", optarg);
			break;
		/*
			subsystem
		*/
		case 'u':		
			fprintf (stderr, "/SUBSYSTEM:%s\n", optarg);
			break;
		/*
			output file
		*/
		case 'v':		
			fprintf (stderr, "/VERSION:%s\n", optarg);
			break;
		case '?':
		default:
			fprintf (stderr, "/%c\n", ch);
			break;
		}
	}
	exit(0);
}
