/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module sound-SUN.c			     */
/*									     */
/*	written by Michael Bischoff					     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

#ifndef AUDIO_DEVICE
#define AUDIO_DEVICE "/dev/audio"
#endif

void play_sound(const char *filename) {
    static int audio = 1;
    if (audio && checksound()) {
	char fullname[200];
	FILE *fp, *fsnd;
	int c;
	if (!(fsnd = fopen(AUDIO_DEVICE, "wb"))) {
	    audio = 0;
	    return;		/* cannot open /dev/audio */
	}
	XSync(dpy, 0);	/* text first! */
	sprintf(fullname, "%s/audio/%s.au", LIBDIR, filename);
	if (!(fp = fopen(fullname, "rb"))) {
	    fclose(fsnd);
	    return;
	}
	/* yeah, copy data */
	while ((c = getc(fp)) != EOF)
	    fputc(c, fsnd);
	fclose(fsnd);
	fclose(fp);
    }
}
