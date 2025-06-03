
/*
 * infer_solver
 * %W% (UCLA) %G%
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>
#include "solver_io.h"

#ifndef _PATH_SOLVERS
#ifdef DEBUG
#define     _PATH_SOLVERS     "."
#else
#define     _PATH_SOLVERS     "/usr/dist/games/lib/xbomb"
#endif
#endif


#define KNOWN(s) ((s == empty) || (s == bomb) || (s == edge))

struct point {
	int x, y;
	};

struct neighbor_set {
	int size;
	struct point points[8];
	};

struct neighbor_set mymap[MAXWIDTH][MAXHEIGHT];

struct act {
        int x, y;
	char action;
	struct act *next;
      };

struct act *acts[8];

int solver_pid = 0;
FILE *solver_rfps, *solver_wfps;


add_em(i, x, y, action)
int i, x, y;
char action;
{
        struct act *newact;

	newact = (struct act *) (malloc(sizeof(struct act)));
	
	newact->x = x;
	newact->y = y;
	newact->action = action;
	newact->next = acts[i];
	acts[i] = newact;
#ifdef DEBUG
	fprintf(stderr,"    adding... %d %d %c\n", x, y, action);
#endif DEBUG
      }

compare_em(n) 
int n;
{
  	int i;
  	struct act *cur, *cp;
 	int same, found;
  
	for (cur = acts[0]; cur != NULL; cur = cur->next) {
        	same = 1;
          	for (i = 1; same && (i < n); i++) {
			found = 0;
	          	for (cp = acts[i]; !found && cp != NULL; cp = cp->next) {
		          	if ((cur->x == cp->x)  &&
		              	    (cur->y == cp->y)  &&
		                    (cur->action == cp->action)) found = 1;
				}
			same = found;
			}
	  	if (same) 
			fprintf(stderr, "%d %d %c\n", cur->x, cur->y, cur->action);
		}
 
	for (i = 0; i < n; i++) {
  		cur = acts[i]; 
		while (cur != NULL) {
			cp = cur->next;
			free(cur);
			cur = cp;
			}
		}
}
    

	
int
apply_empty_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	if (map[x][y].status==unknown) {
		printf ("%d %d e\n", x, y);
	};
}

int 
apply_flag_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	if (map[x][y].status==unknown) {
		map[x][y].status = bomb_flag;
		printf ("%d %d f\n", x, y);
		return(1);
	};
	return(0);
}

int
run4(x, y, dx, dy)
{
	int ct = 0;

	if (map[x][y].status == unknown) ct++;
	if (map[x+dx][y].status == unknown) ct++;
	if (map[x][y+dy].status == unknown) ct++;
	if (map[x+dx][y+dy].status == unknown) ct++;

	return(ct);
}

void
add_neighbor(x, y, dx, dy)
int x, y, dx, dy;
{
	int s;

	s = mymap[x][y].size;
	mymap[x][y].points[s].x = x+dx;
	mymap[x][y].points[s].y = y+dy;
	mymap[x][y].size ++;
	}

void
make_neighbors(status, x, y, dx, dy)
int status, x, y;
{
	int i, j, fix;

	for (j = 0; j < mymap[x+dx][y+dy].size; j++) {
		fix = 1;
		for (i = 0; fix && (i < mymap[x][y].size); i++) {
			if ((mymap[x][y].points[i].x == mymap[x+dx][y+dy].points[j].x) &&
			    (mymap[x][y].points[i].y == mymap[x+dx][y+dy].points[j].y)) {
				fix = 0;
				}
			}
		if (fix) {
			printf ("%d %d %c\n", mymap[x+dx][y+dy].points[j].x, mymap[x+dx][y+dy].points[j].y, (status == empty)?'m':'f');
			}
		}
}

int
subset_of(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	int i, j, found;

	for (i = 0; i < mymap[x1][y1].size; i++) {
		found = 0;
		for (j = 0; !found && j < mymap[x2][y2].size; j++) {
			if ((mymap[x1][y1].points[i].x == mymap[x2][y2].points[j].x) &&
			    (mymap[x1][y1].points[i].y == mymap[x2][y2].points[j].y))
				found = 1;
				}
		if (!found) return(0);
		}
	
	return(1);
	}

int
place_em()
{
	int x, y, i, ret = 0, dx, dy, nx, ny;

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			mymap[x][y].size = 0;
			if (map[x][y].status == empty &&
			    map[x][y].ted >= 1)
				for (dx = -1; dx <= 1; dx++)
					for (dy = -1; dy <= 1; dy++)
						if (((dx != 0) || (dy != 0)) &&
						    (! KNOWN(map[x+dx][y+dy].status)))
							add_neighbor(x, y, dx, dy);
						}


	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			if (map[x][y].status == empty && 
			    map[x][y].ted >= 1) 
			    {
#ifdef DEBUG
				fprintf(stderr, "Inferring %d, %d\n", x, y);
#endif DEBUG
			        for (i = 0; i < mymap[x][y].size; i++) {
#ifdef DEBUG
				      fprintf(stderr," start of %d\n", i);
#endif DEBUG
				      acts[i] = NULL;
				      nx = mymap[x][y].points[i].x;
				      ny = mymap[x][y].points[i].y;
				      map[nx][ny].status = bomb;
				      invoke_solver(i);
				      map[nx][ny].status = empty;
#ifdef DEBUG
				      fprintf(stderr," end of %d\n", i);
#endif DEBUG
				    }
				compare_em(mymap[x][y].size);
			      }
		      }
	
	return(ret);
		   
      }




void
revoke_solvers()   /* Kill them ALL!  Yeah!  Slaughter! */
{
	int i;

		if (solver_pid) {
			fclose(solver_rfps);
			fclose(solver_wfps);
		};
}


void
dump_map(fp)
	FILE *fp;
{
	int x, y;
	char ch;

	fprintf (fp, "%d\n", numbombs);
	for (y = 1; y <= height; y++) {
		for (x = 1; x <= width; x++) {
			if (map[x][y].status == bomb)
				ch = 'X';
			else if (map[x][y].status == bomb_flag)
				ch = 'f';
			else if (! KNOWN(map[x][y].status))
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
		if (pid==-1 || pid==0) return;  /* no children */

		if (solver_pid == pid) {
			fclose(solver_rfps);
			fclose(solver_wfps);
			solver_rfps = solver_wfps = NULL;
			solver_pid = 0;
			break;
			};
	}
}


int
invoke_solver(i)
        int i;
{
	int subproc;
	int rp[2], wp[2];
	FILE *rfp, *wfp;
	int ret;
	int error, status;

	reap_children();

	if (solver_pid == 0) {
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
			if (-1==exec_solver("pattern_solver")) {
				char tmps[80];
				sprintf (tmps, "cannot invoke solver\n");
				bail (tmps);
			};
			/*NOTREACHED*/
		};
		if (subproc == -1)
			bail ("fork");
		solver_pid = subproc;
		close (rp[1]);
		close (wp[0]);
		solver_rfps = fdopen (rp[0], "r");
		solver_wfps = fdopen (wp[1], "w");
	};
	dump_map(solver_wfps);
	ret = read_response(i, solver_rfps);
	reap_children();
	return(ret);
}



int
read_response(i, fp)
        FILE *fp;
        int i;
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
                
		add_em(i, x, y, action);
                };
                

        return(ret);

}

exec_solver(s)
        char *s;
{
        char tmps[512];
        int error;
        if (-1==(error = execl (s, s, NULL))) {
                sprintf (tmps, "%s/%s", _PATH_SOLVERS, s);
                error = execl(tmps,tmps,NULL);
                if (error)
                        fprintf (stderr, "Solver not found as either %s or %s.\n", s, tmps);
        };
        return error;
}


	
main()
{
	int ret; int x, y;

	while (!feof(stdin)) {
		read_data();
		if (counts[bomb_flag])
			exit (1);   /* we don't allow flags, yet */
		calc_em();
		ret = place_em();
		printf ("\n");   /* terminate output */
		fflush(stdout);
	};
	exit (0);
}
