/* ed/vi/ex style global commands, where first the file is scanned for
 *	matching lines, then for each such line, an action is performed.
 *	written for vile: Copyright (c) 1990, 1995 by Paul Fox
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/globals.c,v 1.35 1995/09/12 13:27:44 pgf Exp $
 *
 */

#include	"estruct.h"
#include        "edef.h"

extern	CMDFUNC f_godotplus; 	

static	int	globber P(( int, int, int ));

int
globals(f,n)
int f,n;
{
	return globber(f,n,'g');
}

int
vglobals(f,n)
int f,n;
{
	return globber(f,n,'v');
}

/* ARGSUSED */
static int
globber(f, n, g_or_v)
int f, n, g_or_v;
{
	int c, s;
	register LINE *lp;
	register char *fnp;	/* ptr to the name of the cmd to exec */
	char	cmd[NLINE];
	CMDFUNC *cfp;
	int foundone;
	WINDOW *wp, *sw_wp;
	L_NUM	before;
	int	save_report;

	c = kbd_delimiter();
	if (readpattern("global pattern: ", &pat[0], &gregexp, c, FALSE) != TRUE) {
		mlforce("[No pattern.]");
		return FALSE;
	}

	/* in some sense, it would be nice to search first, before
                making the user answer the next question, but the
                searching delay is too long, and unexpected in the
                middle of a command.  */

	fnp = kbd_engl("action to perform on each matching line: ", cmd);
	/* get the name of, and then the function to execute */
	if (!fnp) {
	        mlforce("[No function]");
		return FALSE;
	} else if ((cfp = engl2fnc(fnp)) == 0) {
	        mlforce("[No such function]");
		return FALSE;
	} else if ((cfp->c_flags & GLOBOK) == 0) {
	        mlforce("[Function not allowed]");
		return FALSE;
	}
	
	
	/* call the searcher, telling it to do line marking */
	s = fsearch(FALSE,0,TRUE,FALSE);
	if (s != TRUE)
		return s;
	
	calledbefore = FALSE;
	
	if (g_or_v == 'v') {  /* invert the sense of all the matches */
		for_each_line(lp, curbp)
			lflipmark(lp);
	}
	/* loop through the buffer -- we must clear the marks no matter what */
	s = TRUE;
	lp = lForw(buf_head(curbp));
	wp = sw_wp = curwp;
	/* loop until there are no marked lines in the buffer */
	foundone = FALSE;
	before = line_count(curbp);
	save_report = global_g_val(GVAL_REPORT);
	for_ever {
		if (lp == l_ref(win_head(wp))) {
			/* at the end -- only quit if we found no 
				marks on the last pass through. otherwise,
				go through again */
			if (foundone)
				foundone = FALSE;
			else
				break;
			lsetnotmarked(lp); /* always unmark the header line */
		}
		if (lismarked(lp)) {
			foundone = TRUE;
			lsetnotmarked(lp);
			/* call the function, if there is one, and results
				have been ok so far */
			if (cfp && s) {
				if (!calledbefore && (cfp->c_flags & UNDO)) {
					if (b_val(wp->w_bufp,MDVIEW))
						return(rdonly());
					mayneedundo();
					set_global_g_val(GVAL_REPORT,0);
				}
				havemotion = &f_godotplus;
				wp->w_dot.l = l_ptr(lp);
				wp->w_dot.o = 0;
				s = (cfp->c_func)(FALSE, 1);
				if (curwp != wp) {
				    /* function may have switched on us */
				    sw_wp = curwp;
				    (void)set_curwp(wp);
				}
				lp = l_ref(wp->w_dot.l);
				havemotion = NULL;
				calledbefore = TRUE;
			}
		}
		lp = lforw(lp);
	}
	set_global_g_val(GVAL_REPORT,save_report);
	(void)line_report(before);
	/* if it tried to switch, do it now */
	(void)set_curwp(sw_wp);

	return s;
}
