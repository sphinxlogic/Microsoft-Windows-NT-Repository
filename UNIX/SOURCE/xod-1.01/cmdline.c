static char *RcsID = "$Id: cmdline.c,v 1.3 1993/03/02 00:44:36 rfs Exp $";

/*
 * $Log: cmdline.c,v $
 * Revision 1.3  1993/03/02  00:44:36  rfs
 * Enhanced for release.
 *
 * Revision 1.2  1993/02/13  14:34:03  rfs
 * *** empty log message ***
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
*/

#include <xod.h>
#include <xod.pt>

static int offsetType = HEX;
static int gotoOffset = 0;

void
CmdLineInit(void) {
	UpdateAll(offsetType, GetDataOffset());
	CursorToAbsByte(gotoOffset);
}

/* print usage summary */
static void
Usage() {
	fprintf(stderr, "Usage: xod [-options] filename\n");
}

/* pares the command line */
void
ParseCommandLine(int argc, char **argv) {
	char *program, *title, *filename;

	program = *argv;
	argv++;
	if (! *argv) {
		Usage();
		exit(1);
	}
	while (*argv) {
		char *arg = *argv;
		char *next;
		if (!strcmp(arg, "-v")) {
			printf("%s\n", XodVersion());
			exit(0);
		}
		else if (!strcmp(arg, "-x"))
			DataMode = HEX;
		else if (!strcmp(arg, "-o"))
			DataMode = OCTAL;
		else if (!strcmp(arg, "-d"))
			DataMode = DECIMAL;
		else if (!strcmp(arg, "-a"))
			DataMode = ASCII;
		/* change offset display format from hex default */
		else if (!strcmp(arg, "-odf")) {
			argv++;
			if (*(*argv) == 'o') {
				offsetType = OCTAL;
			}
			else if (*(*argv) == 'd') {
				offsetType = DECIMAL;
			}
			else if (*(*argv) == 'h') {
				offsetType = HEX;
			}
		}
		else if (!strcmp(arg, "-off")) {
			argv++;
			gotoOffset = AddressConvert(*argv);
		}
		else if (!strncmp(arg, "-", 1))
			;
		else
			filename = arg;
		argv++;
	}
	if (!FileOpen(filename)) {
		fprintf(stderr, "Error trying to open %s for reading\n", filename);
		exit(1);
	}
}
