
/*
 * solvers.c
 * @(#)solvers.c	1.29 (UCLA) 10/15/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include "xbomb.h"


#define MAX_SOLVERS 10
char *solvers[MAX_SOLVERS];
FILE *solver_rfps[MAX_SOLVERS], *solver_wfps[MAX_SOLVERS];
int solver_pids[MAX_SOLVERS];


	
void
revoke_solvers()   /* Kill them ALL!  Yeah!  Slaughter! */
{
	int i;
	for (i=0; i<MAX_SOLVERS; i++)
		if (solver_pids[i]) {
			fclose(solver_rfps[i]);
			fclose(solver_wfps[i]);
		};
}


void
dump_map(fp, column, row)
	FILE *fp;
	int column, row;
{
	int x, y;
	char ch;

	fprintf (fp, "numbombs %d\n", numbombs);
	fprintf (fp, "mouse %d %d\n", column, row);
	fprintf (fp, "map\n");
	for (y = 1; y <= height; y++) {
		for (x = 1; x <= width; x++) {
			if (map[x][y].status == KBOMB)
				ch = 'X';
			else if (map[x][y].flag)
				ch = 'f';
			else if (is_unknown(map[x][y].status))
				ch = '.';
			else ch = map[x][y].neighbors + '0';
			fprintf (fp, "%c", ch);
		};
		fprintf (fp, "\n");
	};
	fprintf (fp, "\n");   /* blank line==end of dataset */
	fflush(fp);
}


void
reap_children()
{
	int pid, status;
	int i;

	for (;;) {
		pid = waitpid(-1, &status, WNOHANG);
		if (pid==-1 || pid==0)
			return;  /* no children */
		for (i=0; i<MAX_SOLVERS; i++)
			if ((solvers[i] != NULL) && (solver_pids[i] == pid)) {
				fclose(solver_rfps[i]);
				fclose(solver_wfps[i]);
				solver_rfps[i] = solver_wfps[i] = NULL;
				solver_pids[i] = 0;
				break;
			};
		if (i >= MAX_SOLVERS)
			fprintf(stderr,"warning: %d reaped, but not sown\n",
				pid);
	};
}


		
int
exec_solver(s)
	char *s;
{
	char tmps[512];
	int error;
	if (-1==(error = execl (s, s, NULL))) {
		sprintf (tmps, "%s/%s", _PATH_SOLVERS, s);
		error = execl(tmps,tmps,NULL);
		if (error)
			fprintf (stderr, "Solver not found as either %s or %s.\n",
				 s, tmps);
	};
	return error;
}


int
read_response(fp)
	FILE *fp;
{
	int x, y, ret = 0;
	char action;
#define SLEN 80
	char s[SLEN];

	for (;;) {
		if (NULL==fgets (s, SLEN, fp))
			break;
		if (s[0] == 0 || s[0] =='\n')
			break;
		if (3 != sscanf (s, "%d %d %c", &x, &y, &action))
			break;
		ret = 1;
		switch (action) {
		case 'f':
			if (! map[x][y].flag)
				place_flag(x,y);
			break;
		case 'm':
			move_somewhere(x,y);
			break;
		case 'e':
			fprintf (stderr, "Flagging empties not yet implemented.\n");
			break;
		default:
			fprintf (stderr, "unknown action '%c'\n", action);
			break;
		};
		putpix(x,y);
	};

	return(ret);
	
}


int
invoke_solvers(n, column, row)
	int n;
	int column, row;
{
	int subproc;
	int rp[2], wp[2];
	FILE *rfp, *wfp;
	int ret;
	int error, status;

	if (solvers[n] == NULL) { 
		fprintf (stderr, "No solver #%d\n", n+1);
		return 0;
	};

	reap_children();

	/* put up a thinking light */
	XCopyPlane(dpy, pixmaps[THINK], puzzleWindow, gc,
			   0, 0, WIDTH, HEIGHT, 5, 5, 1);
	XFlush(dpy);

	if (solver_pids[n] == 0) {
		if (-1==pipe(rp))
			bail("pipe");
		if (-1==pipe(wp))
			bail("pipe");
		if (0==(subproc=fork())) {
			if (-1==dup2 (wp[0], 0))
				bail ("dup2-a");
			if (-1==dup2 (rp[1], 1))
				bail ("dup2-b");
			close (rp[0]);
			close (rp[1]);
			close (wp[0]);
			close (wp[1]);
			/*
			 * Plug the security hole.
			 */
			if (-1==seteuid(getuid()))
				bail("seteuid");
			if (-1==setegid(getgid()))
				bail("setegid");
			if (-1==exec_solver(solvers[n])) {
				char tmps[80];
				sprintf (tmps, "cannot invoke solver: %s\n", solvers[n]);
				bail (tmps);
			};
			/*NOTREACHED*/
		};
		if (subproc == -1)
			bail ("fork");
		solver_pids[n] = subproc;
		close (rp[1]);
		close (wp[0]);
		solver_rfps[n] = fdopen (rp[0], "r");
		solver_wfps[n] = fdopen (wp[1], "w");
	};
	dump_map(solver_wfps[n], column, row);
	ret = read_response(solver_rfps[n]);
	reap_children();

	/* take down thinking light */
	XCopyPlane(dpy, pixmaps[BLANK], puzzleWindow, gc,
			   0, 0, WIDTH, HEIGHT, 5, 5, 1);
	XFlush(dpy);

	return(ret);
}

