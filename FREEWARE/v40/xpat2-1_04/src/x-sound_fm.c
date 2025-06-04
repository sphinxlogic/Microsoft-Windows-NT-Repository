/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-sound-FM.c			     */
/*									     */
/*	written by Michael Bischoff					     */
/*	based on fmtest.c by Rob Hooft					     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

/* FMTEST by Rob Hooft (hooft@chem.ruu.nl) */
/* modified by M. Bischoff */

#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <getopt.h>
#include <sys/soundcard.h>

static int sb;
static char sbbuf[404];
static int sbptr = 0;

static void sbflush(void) {
    if (!sbptr) return;
    
    if (write(sb, sbbuf, sbptr) == -1) {
	perror("write sb");
	exit(EXIT_FAILURE);
    }
    
    sbptr=0;
}

static void sbwrite(char *msg) {
    if (sbptr>400) sbflush();
    
    memcpy(&sbbuf[sbptr], msg, 4);
    sbptr +=4;
}

static void midich(char c) {
    char buf[4];
    
    buf[0] = 5;
    buf[1] = c;
    sbwrite(buf);
}

static void noteon(int chan,int pitch,int vol) {
    char buf[4];
#ifdef DEBUG
    printf("Note on, chan=%d pitch=%d vol=%d\n",chan+1,pitch,vol);
#endif
    
    if (chan >= 0) {
	buf[0] = SEQ_FMNOTEON;
	buf[1] = chan;
	buf[2] = pitch;
	buf[3] = vol;
	sbwrite(buf);	
    } else {
	midich(0x90+chan);
	midich(pitch);
	midich(vol);
    }
}

static void noteoff(int chan,int pitch,int vol) {
    char buf[4];
#ifdef DEBUG
    printf("Note off, chan=%d pitch=%d vol=%d\n",chan+1,pitch,vol);	
#endif
    
    if (chan >= 0) {
	buf[0] = SEQ_FMNOTEOFF;
	buf[1] = chan;
	buf[2] = pitch;
	buf[3] = vol;
	sbwrite(buf);	
    } else {
	midich(0x80+chan);
	midich(pitch);
	midich(vol);
    }
}

static void wait(int delay) {
    int jiffies;
    jiffies = (delay << 8) | SEQ_WAIT;
    sbwrite((char*)&jiffies);
}

#define CHANNELS	4

static void do_music(XEvent *xev) {
    static long mask = KeyPressMask | ButtonPressMask | ExposureMask;
	/* | StructureNotifyMask  */
    char buf[100];
    int channel;
    struct dong {
	int off;
	int volume;
	int note;
    } dong[CHANNELS];

    fprintf(stderr, "do_music called, audio = %d\n", audio);
    XSync(dpy, 0);		/* wait for graphic to be finished */
    if (!audio) {
	wait_event(xev);
	return;
    }

start:
    while (XCheckWindowEvent(dpy, table, mask, xev)) {
	/* printf("xev->type is %d\n", xev->type); */
	if (xev->xany.type == Expose)
	    redraw_table((XExposeEvent *)xev);
	else if (xev->xany.type != NoExpose)
	    return;
    }
    if ((sb=open("/dev/sequencer", O_WRONLY, 0))==-1) {
	sleep(1);
	goto start;	/* retry the open */
    }

    for (channel = 0; channel < CHANNELS; ++channel) {
	buf[0] = SEQ_FMPGMCHANGE;
	buf[1] = channel;
	buf[2] = 9; /* nr_instr; */
	sbwrite(buf);
	dong[channel].off = 0;
    }
    channel = 0;
    {   int tm, tt;
	struct dong *ch;
	tt = tm = 3;

	for (;;) {
	    int delay;
	    /* select next channel */
	    channel = (channel + 1) % CHANNELS;
	    ch = dong + channel;
  	    wait(tm);
	    if (dong[channel].off) {
		noteoff(channel, ch->note, ch->volume);
		tt += 3;
		wait (tm+=3);
	    }
	    ch->off = 1;
	    ch->note = 5 + rand() % 30;
	    ch->volume = 32 + rand() % 32;
 	    noteon(channel, ch->note, ch->volume);
	    delay = 5 + rand() % 40;
	    tt += delay;
	    tm += delay;
 	    while (tt > 200) {
 	        sbflush();
	        while (XCheckWindowEvent(dpy, table, mask, xev)) {
		    /* printf("xev->type is %d\n", xev->type); */
		    if (xev->xany.type == Expose)
			redraw_table((XExposeEvent *)xev);
		    else if (xev->type != NoExpose) {	/* abort */
			for (channel = 0; channel < CHANNELS; ++channel)
			    if (dong[channel].off)
				noteoff(channel, dong[channel].note, dong[channel].volume);
			sbflush();
			close(sb);
			return;
		    }
		}
		tt -= 100;	/* 100 jiffies = 1 second */
		sleep(1);
	    }
        }
    }
}

void play_sound(const char *filename) {
    if (!strcmp(filename, "success") && checksound()) {
	XEvent xev;
	do_music(&xev);
    }
}
