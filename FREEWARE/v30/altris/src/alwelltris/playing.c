/*
# GENERIC X-BASED WELLTRIS
#
#	playing.c
#
###
#
#  Copyright (c) 1993 - 95	David A. Bagley, bagleyd@perry.njit.edu
#
#  Taken from GENERIC X-WINDOW-BASED TETRIS
#
#  Copyright (c) 1992 - 95	 	Q. Alex Zhao, azhao@cc.gatech.edu
#
#			All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and
#  its documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and
#  that both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  This program is distributed in the hope that it will be "playable",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
*/

#include	"welltris.h"

#ifdef VMS
    unsigned long int statvms;
    unsigned long int LIB$WAIT();     /* VMS timer routine */
    float seconds;
#endif /* VMS */

static Bool     paused = False;
static Bool     firstFall = False;

static int      speeds[NUM_LEVELS] = {
    100, 92, 84, 76, 68, 60, 53, 46, 39, 32, 26, 20, 15, 11, 8, 5, 3, 1, 0};

static struct timeval nextFall, now, delay, oldsample;
static struct timezone tzone = {0, 0};

static Bool     evGotNewThing();
static Bool     moveOne();
static void     redrawAll();
static void     gameOver();

/* ------------------------------------------------------------------ */

void
playing()
{
    Bool            resetTime = True;
    int             conNum = ConnectionNumber(display);

    drawTitle();
    drawStatus();
    drawField();
    XFlush(display);
    (void) sleep(1);

    (void) gettimeofday(&oldsample, &tzone);
    score = 0;
    while (True) {
	if (resetTime) {
	    (void) gettimeofday(&nextFall, &tzone);
	    nextFall.tv_usec += 10000 * speeds[level];
	    realTime(&nextFall);
	} else {

	    fd_set          writefd, exceptfd, readfd;

  	    FD_ZERO(&writefd);
	    FD_ZERO(&exceptfd);
	    FD_ZERO(&readfd);
	    FD_SET(conNum, &readfd);

	    (void) gettimeofday(&now, &tzone);
	    delay.tv_sec = nextFall.tv_sec - now.tv_sec;
	    delay.tv_usec = nextFall.tv_usec - now.tv_usec;
	    realTime(&delay);
	    if (((long) delay.tv_sec > 0) ||
		    (((long) delay.tv_sec == 0) &&
		    ((long) delay.tv_usec > 0))) {
		/* sleep */
#ifndef VMS
		(void) select(conNum + 1, &readfd, &writefd, &exceptfd, &delay);
#else
             seconds = ((float)delay.tv_sec);
/*+((float)delay.tv_usec)/1000000.0);*/
             statvms = LIB$WAIT(&seconds);
#endif /* VMS */

	    }
	}

	(void) gettimeofday(&now, &tzone);
        delay.tv_sec = now.tv_sec - oldsample.tv_sec;
        delay.tv_usec = now.tv_usec - oldsample.tv_usec - SAMPLERATE;
        realTime(&delay);

        if (((long) delay.tv_sec > 0) ||
                (((long) delay.tv_sec == 0) &&
                 ((long) delay.tv_usec > 0))) {
            oldsample.tv_sec = now.tv_sec;
            oldsample.tv_usec = now.tv_usec;
	    /*
	     * have to make big scores in very short time, so if someone
	     * is cheating by pausing the game, he/she will not be able
	     * to get into the bonus mode.
	     */
	     if ((long) delay.tv_sec > SAMPLERATE / MILLION)
	       bonus = False;
        }

	if ((now.tv_sec > nextFall.tv_sec) ||
		((now.tv_sec == nextFall.tv_sec) &&
		 (now.tv_usec > nextFall.tv_usec))) {
	    (void) evGotNewThing(True);
	    resetTime = True;
	    firstFall = True;
	} else {
	    resetTime = evGotNewThing(False);
	}
    }
    /* never come to here */
}

/* ------------------------------------------------------------------ */

static Bool
evGotNewThing(falldown)
    Bool            falldown;
{
    XEvent          ev;
    Bool            gotNew = False, flag;
    char            buf[4];

    if (!paused && falldown)
	gotNew = moveOne(FALL) || gotNew;

    while (XPending(display)) {
	XNextEvent(display, &ev);
	flag = True;
	while (flag) {
	    switch (ev.type) {
	    case KeyPress:
		if (!XLookupString(&ev.xkey, buf, 4, NULL, NULL))
		    break;

		switch (buf[0]) {
		case ' ':
		case '\n':
		    if (!paused)
			gotNew = moveOne(DROP) || gotNew;
		    break;

		case 'j':
		case 's':
		    if (!paused)
			gotNew = moveOne(LEFT) || gotNew;
		    break;

		case 'l':
		case 'f':
		    if (!paused)
			gotNew = moveOne(RIGHT) || gotNew;
		    break;

		case 'k':
		case 'd':
		    if (!paused)
			gotNew = moveOne(ROTATE) || gotNew;
		    break;

		case 'i':
		case 'e':
		    if (!paused)
			gotNew = moveOne(REFLECT) || gotNew;
		    break;

		case 'q':
		case 'Q':
		    gameOver();
		    break;

		case '+':
		case '=':
		    if (!paused)
			if (level < NUM_LEVELS - 1) {
			    level++;
			    drawStatus();
			}
		    break;

                /* This can lead to 'cheating' right? */
		case '-':
		case '_':
		    if (!paused)
			if (level > 0) {
			    level--;
			    drawStatus();
			}
                    bonus = False;
		    break;

		case 'b':
		case 'B':
		    beep = !beep;
		    if (beep)
			XBell(display, BVOLUME);
		    break;

		case 'p':
		case 'P':
		case '\03': /* My instinct is Control-C when I see my boss */
		    if (beep)
			XBell(display, BVOLUME);
		    paused = !paused;
		    if (paused) {
			/* "... Hi boss, I'm working hard as usual ..." */
			banner(MSG_PAUSED);
			(void) XIconifyWindow(display, mainWin, screen_num);
		    } else {
			clearNext();
			if (showNext)
			    drawNext();
		    }
		    XSync(display, False);
		    break;

		case '\014':
		    XClearWindow(display, mainWin);
		    XClearWindow(display, blockWin);
		    redrawAll();
		    if (paused)
			banner(MSG_PAUSED);
		    XSync(display, False);
		    break;

		case 'n':
		case 'N':
		    if (!paused) {
			showNext = !showNext;
			if (showNext) {
			    drawNext();
			} else {
			    clearNext();
			}
			XSync(display, False);
		    }
		    break;
		
		case '>':
		case '.':
		    if (!paused && LEGAL(squares + 1, diagonal, mixed, bonus)) {
			squares++;
			redoNext();
			if (showNext) {
			    clearNext();
			    drawNext();
			}
			XSync(display, False);
		    }
		    break;

		case '<':
		case ',':
		    if (!paused && LEGAL(squares - 1, diagonal, mixed, bonus)) {
			squares--;
			redoNext();
			if (showNext) {
			    clearNext();
			    drawNext();
			}
			XSync(display, False);
		    }
		    break;

		case 'g':
		case 'G':
		    if (!paused && LEGAL(squares, !diagonal, mixed, bonus)) {
			diagonal = !diagonal;
			redoNext();
			if (showNext) {
			    clearNext();
			    drawNext();
			}
			XSync(display, False);
		    }
		    break;


		case 'm':
		case 'M':
		    if (!paused && LEGAL(squares, diagonal, !mixed, bonus)) {
			mixed = !mixed;
			redoNext();
			if (showNext) {
			    clearNext();
			    drawNext();
			}
			XSync(display, False);
		    }
		    break;

		case 'o':
		case 'O':
		    if (!paused && LEGAL(squares, diagonal, mixed, !bonus)) {
			bonus = !bonus;
			redoNext();
			if (showNext) {
			    clearNext();
			    drawNext();
			}
			XSync(display, False);
		    }
		    break;

                case 'r':
                case 'R':
                    if (!paused) {
                        grid = !grid;
                        drawGrid();
                        XSync(display, False);
                    }
                    break;

		default:
		    XBell(display, 0);
		    XSync(display, False);
		    break;
		}

		break;

	    case UnmapNotify:
		paused = True;
		break;

	    case FocusOut:
		if (firstFall) {
		    paused = True;
		    banner(MSG_PAUSED);
		}
		break;

	    case Expose:
		if (ev.xexpose.count == 0) {
		    redrawAll();
		    if (paused)
			banner(MSG_PAUSED);
		}
		break;

	    case ClientMessage:
		if ((Atom) ev.xclient.data.l[0] == delw)
		    gameOver();
		break;

	    case DestroyNotify:
		exit(0);
		break;
	    }
	    if (paused) {
		XNextEvent(display, &ev);
		flag = True;
		gotNew = True;		/* for reseting time */
                (void) gettimeofday(&oldsample, &tzone);
	    } else
		flag = False;
	}
    }

    XSync(display, False);
    return gotNew;
}

/* ------------------------------------------------------------------ */

static void
redrawAll()
{
    clearScreen();
    drawTitle();
    drawStatus();
    drawField();
    drawThing();
    if (showNext)
	drawNext();
    if (grid)
	drawGrid();
}

/* ------------------------------------------------------------------ */

static Bool
moveOne(move)
    move_t          move;
{
    int             lines;

    if ((move == DROP) || ((move == FALL) && atBottom())) {
	tryMove(move);
	putBox();
	lines = checkLines();
	score += lines + !gradualAppear + !grid;
	score += (showNext ? 3 : 5) * level * level * lines * lines *
          squares * squares * (bonus + diagonal + 1) / (16 * (1 + mixed));
	rows += lines;
	if (rows > THRESHOLD(level))
        {
	    level++;
            if (bonus)
              bonusNow = True;
        }
	drawStatus();
	newThing();
	if (showNext) {
	    clearNext();
	    drawNext();
	}
	XSync(display, True);	/* discard all events */
	if (allFrozen())
	    gameOver();
        checkFreeze();
	if (overlapping())
	    gameOver();
	drawThing();
	return True;
    } else {
	tryMove(move);
	if (rows > THRESHOLD(level)) {
	    level++;
	    drawStatus();
            if (bonus)
              bonusNow = True;
	}
	return False;
    }
}

/* ------------------------------------------------------------------ */

static void
addScore()
{
    time_t          tloc;
    char            buff[2][SCORESIZE];
    char            lockfile[FILENAMELEN];
    int             fd, lfd;
    int             tmp, ptmp, s1;
    int             mycount = 0;
    Bool            saved = False, trickle = False;

    (void) time(&tloc);
    (void) strcpy(myscore.mydate, asctime(localtime(&tloc)));
    (void) sprintf(myscore.score, "%9d", score);
    (void) sprintf(myscore.level, "%3d", level);
    (void) sprintf(myscore.rows, "%4d", rows);

    (void) fprintf(stderr, "\n- %s", myscore.mydate);
    (void) fprintf(stderr, "- Your final score is %d,", score);
    if (rows == 1)
      (void) fprintf(stderr, " at level %d with 1 row.\n\n", level);
    else
      (void) fprintf(stderr, " at level %d with %d rows.\n\n", level, rows);

    if ((fd = open(scorefile, O_CREAT | O_RDWR, 0644)) < 0) {
	(void) fprintf(stderr, "Cannot write the score-file!\n");
	return;
    }

    /* lock */
    (void) strcpy(lockfile, scorefile);
#ifndef VMS
    (void) strcat(lockfile, ".lock");
#else
    (void) strcat(lockfile, "_lock");
#endif /* VMS */

    while (((lfd = open(lockfile, O_CREAT | O_EXCL, 0644)) < 0) &&
	    errno == EEXIST)
	(void) sleep(1);

    if (lfd < 0) {
	(void) perror("Error in creating the score-file lock-file");
	(void) fprintf(stderr, "Score not recorded - sorry.\n");
	return;
    }
    tmp = 0;
    ptmp = 1;
    bcopy((char *) &myscore, buff[1], SCORESIZE);

    while (read(fd, buff[tmp], SCORESIZE) == SCORESIZE) {
	(void) sscanf(((score_t *) buff[tmp])->score, " %d", &s1);
	if (!saved && (s1 <= score)) {
	    trickle = True;
	    saved = True;
	    mycount++;
	}
	if (!strncmp(myscore.myname, ((score_t *) buff[tmp])->myname,
		     NAMELEN)) {
	    mycount++;
	}
	/* Then check if we should trickle the score */
	if (trickle) {
	    (void) lseek(fd, (off_t) - SCORESIZE, SEEK_CUR);
	    (void) write(fd, buff[ptmp], SCORESIZE);
	    ptmp = tmp;
	    tmp = (tmp + 1) % 2;
	}

	/*
	 * As we trickle, we add up records owned by me. Once we hit max, we
	 * throw it away, and stop trickling.
	 */
	if ((mycount > MAXSCORES) || ((mycount == MAXSCORES) && !trickle)) {
	    trickle = False;
	    break;
	}
    }				/* while */

    if (trickle) {
	(void) write(fd, buff[ptmp], SCORESIZE);
    }
    if (!saved && (mycount < MAXSCORES)) {
	(void) write(fd, (char *) &myscore, SCORESIZE);
    }
    /* unlock */
    (void) close(lfd);
    (void) unlink(lockfile);
    (void) close(fd);
}

/* ------------------------------------------------------------------ */

static void
gameOver()
{
    banner(MSG_END);
    fillLines();
    addScore();
    showScores(SHOWSCORES);
    XCloseDisplay(display);
    exit(0);
}

/* ------------------------------------------------------------------ */

void
showScores(num)
    int             num;
{
    int             fd, i = 0;
    score_t         curs;

    if ((fd = open(scorefile, O_RDONLY, 0644)) < 0)
	return;

    (void) fprintf(stderr, "            GENERIC WELLTRIS  HALL OF FAME\n\n");
    (void) fprintf(stderr,
		"   # USER            SCORE   L    R  HOST         DATE\n");

    while (read(fd, (char *) &curs, SCORESIZE) == SCORESIZE) {
	i++;
	if ((num == 0) || (i <= num))
	    (void) fprintf(stderr, "%4d %-12s%9s %3s %4s  %-12s %-s",
			   i, curs.myname, curs.score, curs.level, curs.rows,
			   curs.myhost, curs.mydate);
    }
    (void) close(fd);
    if (i == 1)
      (void) fprintf(stderr, "There is 1 score to date.\n");
    else
      (void) fprintf(stderr, "There are %d scores to date.\n", i);
}

/* ------------------------------------------------------------------ */
