Date: Thursday, 16 June 1988  16:55-MDT
From: attcan!utzoo!dciem!nrcaer!cognos!bruce at uunet.uu.net (Dwayne Bruce)
Re:   N.B.S. Time Service

Well kids, after reading about the NBS telephone time service, and
having read about an expensive commerical system from Precision
Standard Time, Inc., I just had to tell you about the system I cooked
up.

We've been keeping our VAXen in sync with a simple system that uses
the AFSK time code produced by CHU.  CHU is Canada's provider of
brodcast time, and it has a signal that can be heard regularly all
over the western hemisphere.  During seconds 31..39 of each minute,
they transmit a time code in 103-type AFSK.  I built a modem, plugged
it into an HF receiver, and wrote a piece of code to handle the
output. The code is enclosed.

If you want to know more about the hardware, consult the article I
wrote in the April, 1988 edition of "The Canadian Amateur" (published
by the Canadian Amateur Radio Federation).

Anyway, here's the code.  It runs under UNIX and VMS.

-------------------------
/* CHU      Marcus Leech  VE3MDL   Aug 1987 */
/* This program reads and understands the timecode format
 *  produced by the CHU broadcast signal.  This signal provides
 *  an atomic time standard to most places in North and South
 *  America on 3.330 7.335 and 14.670 MHZ, mode A3. During
 *  seconds 31 thru 39 the time is broadcast using Bell 103 AFSK.
 *  The time code looks like this:
 *  6.d|d.d|h.h|m.m|s.s|6.d|d.d|h.h|m.m|s.s
 *  The time code is repeated twice for error detection. Each . represents
 *    one nibble boundary.  The nibbles get transmitted reversed, so you
 *    have to flip them to make sense of the time code.  Each nibble is a BCD
 *    digit.
 * It takes one argument, the input device, and some optional flags:
 *  chu [-adst] <device>
 *  -
 *  a adjust  attempt to adjust the system clock to CHU time
 *  d daemon  run continuously as a daemon
 *  s show    show semi-raw timecode
 *  t tune    show chars that wouldn't get considered for time code
 *
 * When used as a system-time-adjuster, you need to set the system time
 *  to within 2mins of the correct time. The program will drag the
 *  clock into exact synchronism.
 */
#include <stdio.h>
#ifndef VMS
#include <sys/types.h>
#include <sgtty.h>
#include <time.h>
#else VMS
#include <types.h>
#include <time.h>
#endif VMS
/* Macros to fetch individual nibbles. */
#define hinib(x) (((x) >> 4) & 0x0F)
#define lonib(x) ((x) & 0x0F)
/* Macro to restore correct ordering within a byte. */
#define byte(h,l) (((l) << 4)|(h))
#define ADJINT 27
#define TWEAK 50 + 12     /* Fudge factor in centiseconds. */
                          /* CHU code is skewed by 0.5 seconds. */
#define iabs(x) ((x < 0) ? -x : x)
char sample1[5];  /* The first time code. */
char sample2[5];  /* The second (error checking) time code. */
main (argc, argv)
int argc;
char **argv;
{
    char c, *p;
#ifndef VMS
    struct sgttyb sgb;            /* For fiddling with tty line params. */
#endif VMS
    int line;                     /* Fd for the tty line.*/
    int i;
    int adjcnt;                     /* Number of samples before we adjust.*/
    struct tm *localtime(), *ltp, *gmtime();  /* To break out the time. */
    time_t now, time();
#ifdef VMS
    long t0[2], t1[2], t2[2];
#endif VMS
    int amm, ass, mmss, diff;
    char *devstr;
    int adjust;
    int show;
    int tune;
    int daemon;

    setbuf (stdout, NULL);
    adjust = show = tune = 0;
    devstr = "/dev/null";
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            p = &argv[i][1];
            while (*p)
            {
                switch (*p)
                {
                case 'a':
                    adjust++;
                    break;
                case 't':
                    tune++;
                    break;
                case 's':
                    show++;
                    break;
                case 'd':
                    daemon++;
                    break;
                default:
                    fprintf (stdout, "unknown flag '%c'\n", *p);
                    exit (1);
                }
                *p++;
            }
        }
        else
        {
            strcpy (devstr, argv[i]);
        }
    }
    line = open (devstr, 0);
#ifndef VMS
    /* Set up 8-bit datapath, at 30CPS. */
    gtty (line, &sgb);
    sgb.sg_ispeed = sgb.sg_ospeed = B300;
    sgb.sg_flags |= RAW;
    stty (line, &sgb);
#endif VMS
    adjcnt = 0;
    if (!daemon)
    {
        adjcnt = 19;
    }
    /* Read forever, waiting for the synchronizing BCD 6 digit to appear. */
    for (;;)
    {
        read (line, &c, 1);
        /* We have a syncronizing digit. Grab two samples and compare. */
        if (lonib(c) == 6)
        {
            /* Get first sample. */
            sample1[0] = byte(hinib(c),lonib(c));
            for (i = 1; i < 5; i++)
            {
                read (line, &c, 1);
                sample1[i] = byte(hinib(c),lonib(c));
            }
            /* Get second sample. */
            for (i = 0; i < 5; i++)
            {
                read (line, &c, 1);
                sample2[i] = byte(hinib(c),lonib(c));
            }
            /* If samples match, we have a valid time code. */
            if (compare (sample1, sample2, 5) == 0)
            {
                /* Show the code (if -s).  The high-order nibble in the
                 *  first byte is the synch digit, so it gets masked out
                 *  for printing.
                 */
                if (show)
                {
                    fprintf (stdout, "TC: ");
                    for (i = 0; i < 5; i++)
                    {
                        fprintf (stdout, "%02x", sample1[i]);
                    }
                    fprintf (stdout, "\n");
                }
                if (adjcnt++ >= ADJINT)
                {
                    adjcnt = 0;
                    /* Fetch UTC (GMT). */
                    time (&now);
                    ltp = gmtime (&now);
                    /* Convert time code minutes and seconds into
                     *   binary.
                     */
                    amm = (hinib(sample1[3]) * 10) + lonib(sample1[3]);
                    ass = (hinib(sample1[4]) * 10) + lonib(sample1[4]);
                    /* Convert minutes and seconds portion of system time. */
                    mmss = (ltp->tm_min * 60) + ltp->tm_sec;
                    /* Compute the difference. */
                    diff = ((amm * 60) + ass) - mmss;
                    /* Adjust the system time. */
                    now += (long)diff;
                    if (iabs(diff) > 120)
                    {
                        fprintf (stdout, "%02d-%02d-%02d %02d:%02d:%02d ",
                            ltp->tm_year, ltp->tm_mon + 1, ltp->tm_mday,
                            ltp->tm_hour, ltp->tm_min, ltp->tm_sec);
                        fprintf (stdout, "TERROR %d\n", diff);
                        if (!daemon)
                        {
                            break;
                        }
                        continue;
                    }
                    /* Only do it if there IS a (reasonable) difference. */
                    if ((diff != 0) && (adjust))
                    {
                        ltp = localtime (&now);
                        fprintf (stdout, "%02d-%02d-%02d %02d:%02d:%02d ",
                            ltp->tm_year, ltp->tm_mon + 1, ltp->tm_mday,
                            ltp->tm_hour, ltp->tm_min, ltp->tm_sec);
                        fprintf (stdout, "ADJUST %d\n", diff);
                        /* What we'd REALLY like here is a system call of
                         * the form:   stime (*time, ticks)
                         * that would allow you to set the system tick
                         * counter to <ticks> in centiseconds.  Too bad.
                         *-->stime (&now, TWEAK);<--
                         */
#ifndef VMS
#ifndef EUNICE
                        stime (&now);
#endif EUNICE
#else VMS
                        t1[0] = 100000 * ((diff * 100) + TWEAK);
                        t1[1] = 0;
                        if (diff < 0)
                        {
                            t1[1] = -1;
                        }
                        sys$gettim (t0);
                        lib$addx (t0, t1, t2);
                        sys$setime (t2);
#endif VMS
                    }
                    ltp = localtime (&now);
                    fprintf (stdout, "%02d-%02d-%02d %02d:%02d:%02d TVALID\n",
                        ltp->tm_year, ltp->tm_mon + 1, ltp->tm_mday,
                        ltp->tm_hour, ltp->tm_min, ltp->tm_sec);
                    if (!daemon)
                    {
                        break;
                    }
                }
            }
        }
        else if (tune)
        {
                fprintf (stdout, "FT: %c (%02x)\n", c, (unsigned)c);
        }
    }
}
/* Compare two byte-arrays (s1, s2) of length cnt. */
compare (s1, s2, cnt)
char *s1;
char *s2;
int cnt;
{
    int i;
    for (i = 0; i < cnt; i++)
    {
        if (*s1++ != *s2++)
        {
            return (1);
        }
    }
    return (0);
}
#ifdef VMS
struct tm *
gmtime (tod)
time_t *tod;
{
    int hh;
    int mm;
    char s;
    int secdiff, sgn;
    time_t utc;
    char *p, *getenv ();
    p = getenv ("UTCDISP");
    sscanf (p, "%c %02d:%02d", &s, &hh, &mm);
    utc = *tod;
    secdiff = (hh * 3600) + (mm * 60);
    sgn = (s == '+') ? 1 : -1;
    secdiff *= sgn;
    utc += secdiff;
    return (localtime (&utc));
}
#endif VMS

