#ifndef lint
static char *rcsid = "$Id: you.c,v 1.1 1992/12/22 09:06:36 joke Exp $";
#endif

#include <stdio.h>
#include "youvegot.h"

main (argc, argv)
    int argc;
    char **argv;
{
    FILE *audio;

    if ((audio = fopen ("/dev/audio", "w")) != NULL) {
	fwrite (youvegot, YOUVESIZ, 1, audio);
	fclose (audio);
	return (0);
    } else {
	fprintf (stderr, "Sorry: No `/dev/audio' available.\n");
	return (-1);
    }
}
