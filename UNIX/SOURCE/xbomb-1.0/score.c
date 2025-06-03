
/*
 * score.c
 * @(#)score.c	1.29 (UCLA) 10/13/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 * This scoring code is completely Matthew's thieving fault
 * (thanks to the authors of ATC).
 */

#include "xbomb.h"

char	*name = NULL;	/* the player's name */
	
int
compar(a, b)
	struct score *a, *b;
{
	return (b->score - a->score);
}

print_score()
{
	int cf, d, ct;
	
	ct = emptylen;
	cf = numflags;
	if (cf < 0) cf = 0;

	while (ct > 1) {
		d = cf%10;
		if ((d == 0) && (cf == 0) && (numflags > 0)) {
			XCopyPlane(dpy, pixmaps[BLANK], puzzleWindow, gc,
			   0, 0, WIDTH, HEIGHT, ct*WIDTH, 0, 1);
		}
		else {
			XCopyPlane(dpy, pixmaps[d], puzzleWindow, gc,
			   0, 0, WIDTH, HEIGHT, ct*WIDTH, 0, 1);
		}
		ct--;
		cf = cf/10;
	}
}

int
figure_score()
{
	int s, x, y;
	s=0;
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status == KEMPTY)
				s += map[x][y].neighbors * map[x][y].neighbors;
	return s;
}

char *
formattime(t)
	time_t *t;
{
#define SLEN 80
	static char s[SLEN];
	strftime(s, SLEN, "%e-%b-%y", localtime(t));
	return s;

}

log_score()
{
	register int	c, r, place, change = 0, ct, i, fd, nnum_scores = 0, 
	num_scores = 0, good, nscoreid = -1;
	int botscore;
	struct passwd	*pw;
	FILE		*fp;
	char		*cp, *index(), *rindex();
	struct score	nscore[NUM_SCORES + 1], score[NUM_SCORES + 1], thisscore;
	char		*tmp, s[101];
	char		scorechar;
	struct score	ascore;
	
	umask(0);
	fd = open(_PATH_SCORE, O_CREAT|O_RDWR, 0644);
	if (fd < 0) {
		perror(_PATH_SCORE);
		return (-1);
	}
	/*
	 * This is done to take advantage of stdio, while still 
	 * allowing a O_CREAT during the open(2) of the log file.
	 */
	fp = fdopen(fd, "r+");
	if (fp == NULL) {
		perror(_PATH_SCORE);
		return (-1);
	}
#ifndef SVR4
	if (flock(fileno(fp), LOCK_EX) < 0)
	{
		perror("flock");
		return (-1);
	}
#endif
	
	for (;;) {
		if (fgets(s, 100, fp) == NULL) break;

#define REQFIELDS 8
		good = sscanf(s, "%c %s %d %d %d %d %d %d",
			      &scorechar,
			      ascore.name,
			      &ascore.size, 
			      &ascore.bombs,
			      &ascore.defused, 
			      &ascore.won,
			      &ascore.score,
			      &ascore.time);
		if (good == REQFIELDS-1)
			ascore.time = 717701220;   /* when the time field was added */
		ascore.mine = 0;
		if (scorechar == 'P') {
			nscore[nnum_scores] = ascore;
			if (ascore.bombs == numbombs)
				nscoreid = nnum_scores;
			if (good != REQFIELDS || ++nnum_scores >= NUM_SCORES)
				break;
		} else if (scorechar == 'H') {
			score[num_scores] = ascore;
			if (good != REQFIELDS || ++num_scores >= NUM_SCORES)
				break;
		} else {
			bail("Corrupt score file.");
		};
	};

	if (name == NULL) {
		if ((pw = (struct passwd *) getpwuid(getuid())) == NULL) {
			fprintf(stderr, 
				"getpwuid failed for uid %d.  Who are you?\n",
				getuid());
			return (-1);
		}
		strcpy(thisscore.name, pw->pw_name);
		}
	else {
		strncpy(thisscore.name, name, 8);
		thisscore.name[8] = '\0';
		}

	thisscore.size = width * height;
	thisscore.bombs = numbombs;
	thisscore.defused = numbombs - numflags;
	for (c = 1; c <= width; c++)
		for (r = 1; r <= height; r++)
			if (map[c][r].flag && 
			    ((map[c][r].status == EMPTY) || 
			     (map[c][r].status == KEMPTY)))
				thisscore.defused--;
	thisscore.won = dead?0:1;
	thisscore.score = figure_score();
/* Old scoring function:
 *	thisscore.score = 
 *		(thisscore.won + 1)*(thisscore.size * thisscore.defused);
 */
	thisscore.time = time(NULL);
	thisscore.mine = 1;
	
	ct = 0;
	botscore = 0;
	for (i = 0; i < num_scores; i++) {
		if (strcmp(thisscore.name, score[i].name) == 0) {
			botscore = score[i].score;
			ct++;
			place = i;
		}
	}
	
	if (ct >= MAX_PER_PLAYER) {
		if (thisscore.score > botscore) {
			bcopy(&thisscore, &score[place], sizeof (score[place]));
			qsort(score, num_scores, sizeof (*score), compar);
			change = 1;
		}
	}
	else {
		bcopy(&thisscore, &score[num_scores], sizeof (score[place]));
		qsort(score, num_scores + 1, sizeof (*score), compar);
		if (thisscore.score >= score[num_scores].score) change = 1;
		if (num_scores < NUM_SCORES) num_scores++;
	}
	
	if (nscoreid == -1) {
		bcopy(&thisscore, &nscore[nnum_scores++], sizeof (nscore[0]));
		change = 1;
		nscoreid = nnum_scores;
	}
	
	if (nscore[nscoreid].defused < thisscore.defused) {
		bcopy(&thisscore, &nscore[nscoreid], sizeof (nscore[nscoreid]));
		change = 1;
	}
	
	if (change && !onlyprint) {
		rewind(fp);
		for (i = 0; i < num_scores; i++)
			fprintf(fp, "H %s %d %d %d %d %d %d\n",
				score[i].name, score[i].size,
				score[i].bombs, score[i].defused,
				score[i].won, score[i].score,
				score[i].time);
		for (i = 0; i < nnum_scores; i++)
			fprintf(fp, "P %s %d %d %d %d %d %d\n",
				nscore[i].name, nscore[i].size,
				nscore[i].bombs, nscore[i].defused,
				nscore[i].won, nscore[i].score,
				nscore[i].time);
	}
	
#ifndef SVR4
	flock(fileno(fp), LOCK_UN);
#endif
	fclose(fp);

	if (showscore || change) {
		printf("%2s:  %-8s  %-9s  %7s  %4s  %5s  %5s  %3s\n",
		       "#", "name", "date",
		       "score", "size", "bombs", "flags", "won");
		printf("---------------------------------------------------------\n");
		for (i = 0; i < num_scores; i++) {
			if (i <= TOP_PRINT || 
			   (strcmp(thisscore.name, score[i].name) == 0) || onlyprint)
				printf("%2d:  %-8s  %-9s  %7d  %4d  %5d  %5d %3s %6s\n",
				       i + 1,
				       score[i].name, formattime(&score[i].time),
				       score[i].score, 
				       score[i].size, score[i].bombs, 
				       score[i].defused,
				       (score[i].won)?" Y":" N",
				       (score[i].mine)?"  <---":" ");
			}
		
		if (!onlyprint) {
			printf("\nYour stats:\n     %-8s  %-9s  %7d  %4d  %5d  %5d %3s %6s\n",
			       thisscore.name, 
			       formattime(&thisscore.time),
			       thisscore.score,  
			       thisscore.size, 
			       thisscore.bombs, 
			       thisscore.defused,
			       (thisscore.won)?" Y":" N", 
			       (thisscore.mine)?"  <---":" ");
			
			printf("\nMost defused with %d bombs\n     %-8s  %-9s  %7d  %4d  %5d  %5d %3s %6s\n",
			       nscore[nscoreid].bombs,
			       nscore[nscoreid].name, 
			       formattime(&nscore[nscoreid].time),
			       nscore[nscoreid].score,  
			       nscore[nscoreid].size, 
			       nscore[nscoreid].bombs, 
			       nscore[nscoreid].defused,
			       (nscore[nscoreid].won)?" Y":" N", 
			       (nscore[nscoreid].mine)?"  <---":" ");
			}
		}
	return (0);
}
