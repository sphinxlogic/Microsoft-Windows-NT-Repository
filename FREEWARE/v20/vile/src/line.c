/*
 * The functions in this file are a general set of line management utilities.
 * They are the only routines that touch the text. They also touch the buffer
 * and window structures, to make sure that the necessary updating gets done.
 * There are routines in this file that handle the kill register too. It isn't
 * here for any good reason.
 *
 * Note that this code only updates the dot and mark values in the window list.
 * Since all the code acts on the current window, the buffer that we are
 * editing must be being displayed, which means that "b_nwnd" is non zero,
 * which means that the dot and mark values in the buffer headers are nonsense.
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/line.c,v 1.96 1995/10/19 20:02:25 pgf Exp $
 *
 */

/* #define POISON */
#ifdef POISON
#define poison(p,s) (void)memset((char *)p, 0xdf, s)
#else
#define poison(p,s)
#endif

#include	"estruct.h"
#include	"edef.h"

#define roundlenup(n) ((n+NBLOCK-1) & ~(NBLOCK-1))

static	int	doput P(( int, int, int, REGIONSHAPE ));
static	int	ldelnewline P(( void ));
static	int	PutChar P(( int, REGIONSHAPE ));

#if OPT_SHOW_REGS && OPT_UPBUFF
static	void	relist_registers P(( void ));
#else
#define relist_registers()
#endif

#if OPT_MAP_MEMORY	/* patch: should use this in other places */
#if HAS_MEMMOVE
#define	MemMove(dst,src,len)	memmove(dst,src,len)
#else

static	char *	MemMove P(( char *, char *, SIZE_T ));

static char *
MemMove (dst, src, len)
char	*dst;
char	*src;
SIZE_T	len;
{
	/* copy from right to left, because src & dst overlap */
	while (len-- > 0)
		dst[len] = src[len];
	return dst;
}
#endif
#endif	/* patch */

#if OPT_MAP_MEMORY
static	int	Truncate P((int, SIZE_T));
static	int	WouldTruncate P((void));

static int
Truncate (want, have)
int	want;
SIZE_T	have;
{
	if (want > (int)have) {
		mlforce("[insertion truncated from %d to %d chars]", want, have);
		want = have;
	}
	return want;
}

static int
WouldTruncate()
{
	mlforce("[Line would be truncated]");
	return (FALSE);
}
#endif

/*
 * Test the 'report' threshold, returning true if the argument is above it.
 */
int
do_report (value)
L_NUM	value;
{
	if (value < 0)
		value = -value;
	return (global_g_val(GVAL_REPORT) > 0
	   &&   global_g_val(GVAL_REPORT) <= value);
}

/*
 * This routine allocates a block of memory large enough to hold a LINE
 * containing "used" characters. The block is always rounded up a bit. Return
 * a pointer to the new block, or NULL if there isn't any memory left. Print a
 * message in the message line if no space.
 */
/*ARGSUSED*/
LINEPTR
lalloc(used,bp)
register int	used;
BUFFER *bp;
{
	register LINE	*lp;
	register SIZE_T	size;

	/* lalloc(-1) is used by undo for placeholders */
	if (used < 0)  {
		size = 0;
	} else {
		size = roundlenup(used);
	}
#if OPT_MAP_MEMORY
	lp = l_ref(l_allocate(size));
	size = Truncate(size, lp->l_size);
#else
	/* see if the buffer LINE block has any */
	if ((lp = bp->b_freeLINEs) != NULL) {
		bp->b_freeLINEs = lp->l_nxtundo;
	} else if ((lp = typealloc(LINE)) == NULL) {
		(void)no_memory("LINE");
		return NULL;
	}
	lp->l_text = NULL;
	if (size && (lp->l_text = castalloc(char,size)) == NULL) {
		(void)no_memory("LINE text");
		poison(lp, sizeof(*lp));
		free((char *)lp);
		return NULL;
	}
	lp->l_size = size;
#endif
#if !SMALLER
	lp->l_number = 0;
#endif
	lp->l_used = used;
	lsetclear(lp);
	lp->l_nxtundo = null_ptr;
	return l_ptr(lp);
}

/*ARGSUSED*/
void
lfree(lp,bp)
fast_ptr LINEPTR lp;
fast_ptr BUFFER *bp;
{
#if OPT_MAP_MEMORY
	l_deallocate(lp);
#else
	if (lisreal(lp))
		ltextfree(lp,bp);

	/* if the buffer doesn't have its own block of LINEs, or this
		one isn't in that range, free it */
	if (!bp->b_LINEs || lp < bp->b_LINEs || lp >= bp->b_LINEs_end) {
		poison(lp, sizeof(*lp));
		free((char *)lp);
	} else {
		/* keep track of freed buffer LINEs here */
		lp->l_nxtundo = bp->b_freeLINEs;
		bp->b_freeLINEs = lp;
#ifdef POISON
		/* catch references hard */
		set_lback(lp, (LINE *)1);
		set_lforw(lp, (LINE *)1);
		lp->l_text = (char *)1;
		lp->l_size = lp->l_used = LINENOTREAL;
#endif
	}
#endif
}

#if !OPT_MAP_MEMORY
/*ARGSUSED*/
void
ltextfree(lp,bp)
register LINE *lp;
register BUFFER *bp;
{
	register UCHAR *ltextp;

	ltextp = (UCHAR *)lp->l_text;
	if (ltextp) {
		if (bp->b_ltext) { /* could it be in the big range? */
			if (ltextp < bp->b_ltext || ltextp >= bp->b_ltext_end) {
				poison(ltextp, lp->l_size);
				free((char *)ltextp);
			} /* else {
			could keep track of freed big range text here;
			} */
		} else {
			poison(ltextp, lp->l_size);
			free((char *)ltextp);
		}
		lp->l_text = NULL;
	} /* else nothing to free */
}
#endif

/*
 * Delete line "lp". Fix all of the links that might point at it (they are
 * moved to offset 0 of the next line. Unlink the line from whatever buffer it
 * might be in. The buffers are updated too; the magic
 * conditions described in the above comments don't hold here.
 * Memory is not released, so line can be saved in undo stacks.
 */
void
lremove(bp,lp)
register BUFFER *bp;
fast_ptr LINEPTR lp;
{
	register WINDOW *wp;
	fast_ptr LINEPTR point;

	point = lFORW(lp);

#if !WINMARK
	if (same_ptr(MK.l, lp)) {
		MK.l = point;
		MK.o = 0;
	}
#endif
	for_each_window(wp) {
		if (same_ptr(wp->w_line.l, lp))
			wp->w_line.l = point;
		if (same_ptr(wp->w_dot.l, lp)) {
			wp->w_dot.l  = point;
			wp->w_dot.o  = 0;
		}
#if WINMARK
		if (wp->w_mark.l == lp) {
			wp->w_mark.l = point;
			wp->w_mark.o = 0;
		}
#endif
#if 0
		if (wp->w_lastdot.l == lp) {
			wp->w_lastdot.l = point;
			wp->w_lastdot.o = 0;
		}
#endif
	}
	if (bp->b_nwnd == 0) {
		if (same_ptr(bp->b_dot.l, lp)) {
			bp->b_dot.l = point;
			bp->b_dot.o = 0;
		}
#if WINMARK
		if (same_ptr(bp->b_mark.l, lp)) {
			bp->b_mark.l = point;
			bp->b_mark.o = 0;
		}
#endif
#if 0
		if (same_ptr(bp->b_lastdot.l, lp)) {
			bp->b_lastdot.l = point;
			bp->b_lastdot.o = 0;
		}
#endif
	}
#if 0
	if (bp->b_nmmarks != NULL) { /* fix the named marks */
		int i;
		struct MARK *mp;
		for (i = 0; i < 26; i++) {
			mp = &(bp->b_nmmarks[i]);
			if (same_ptr(mp->p, lp)) {
				mp->p = point;
				mp->o = 0;
			}
		}
	}
#endif
#if OPT_VIDEO_ATTRS
	{
	    AREGION *ap = bp->b_attribs;
	    while (ap != NULL) {
		int samestart = same_ptr(ap->ar_region.r_orig.l, lp);
		int sameend   = same_ptr(ap->ar_region.r_end.l, lp);
		if (samestart && sameend) {
		    AREGION *tofree = ap;
		    ap = ap->ar_next;
		    free_attrib(bp, tofree);
		}
		else if (samestart) {
		    ap->ar_region.r_orig.l = point;
		    ap->ar_region.r_orig.o = 0;
		    ap = ap->ar_next;
		}
		else if (sameend) {
		    ap->ar_region.r_end.l = lBACK(lp);
		    ap->ar_region.r_end.o = lLength(ap->ar_region.r_end.l);
		    ap = ap->ar_next;
		}
		else
		    ap = ap->ar_next;
	    }
	}
#endif /* OPT_VIDEO_ATTRS */
	set_lForw(lBack(lp), lFORW(lp));
	set_lBack(lForw(lp), lBACK(lp));
}

int
insspace(f, n)	/* insert spaces forward into text */
int f, n;	/* default flag and numeric argument */
{
	if (!linsert(n, ' '))
		return FALSE;
	return backchar(f, n);
}

int
lstrinsert(s,len)	/* insert string forward into text */
char *s;	/* if NULL, treat as "" */
int len;	/* if non-zero, insert exactly this amount.  pad if needed */
{
	char *p = s;
	int n, b = 0;
	if (len <= 0)
		n = HUGE;
	else
		n = len;
	while (p && *p && n) {
		b++;
		if (!linsert(1, *p++))
			return FALSE;
		n--;
	}
	if (n && len > 0) {	/* need to pad? */
		if (!linsert(n, ' '))
			return FALSE;
		b += n;
	}

	DOT.o -= b;

	return TRUE;
}

/*
 * Insert "n" copies of the character "c" at the current location of dot. In
 * the easy case all that happens is the text is stored in the line. In the
 * hard case, the line has to be reallocated. When the window list is updated,
 * take special care; I screwed it up once. You always update dot in the
 * current window. You update mark, and a dot in another window, if it is
 * greater than the place where you did the insert. Return TRUE if all is
 * well, and FALSE on errors.
 */
int
linsert(n, c)
int n, c;
{
	register char	*cp1;
	register char	*cp2;
	register LINE	*tmp;
	fast_ptr LINEPTR lp1;
	fast_ptr LINEPTR lp2;
	fast_ptr LINEPTR lp3;
	register int	doto;
	register int	i;
	register WINDOW *wp;
#if !OPT_MAP_MEMORY
	register char	*ntext;
#endif
	SIZE_T	nsize;

	lp1 = DOT.l;				/* Current line 	*/
	if (same_ptr(lp1, buf_head(curbp))) {	/* At the end: special	*/
		if (DOT.o != 0) {
			mlforce("BUG: linsert");
			return (FALSE);
		}
		lp2 = lalloc(n, curbp);		/* Allocate new line	*/
		if (same_ptr(lp2, null_ptr))
			return (FALSE);

		lp3 = lBACK(lp1);		/* Previous line	*/
		set_lFORW(lp3, lp2);		/* Link in		*/
		set_lFORW(lp2, lp1);
		set_lBACK(lp1, lp2);
		set_lBACK(lp2, lp3);
		(void)memset(l_ref(lp2)->l_text, c, (SIZE_T)n);

		tag_for_undo(lp2);

		/* don't move DOT until after tagging for undo */
		/*  (it's important in an empty buffer) */
		DOT.l = lp2;
		DOT.o = n;
		chg_buff(curbp, WFINS|WFEDIT);
		return (TRUE);
	}
	doto = DOT.o;				/* Save for later.	*/
	tmp  = l_ref(lp1);
	nsize = llength(tmp) + n;
	if (nsize > tmp->l_size) {		/* Hard: reallocate	*/
		/* first, create the new image */
		nsize = roundlenup((int)nsize);
		copy_for_undo(lp1);
#if OPT_MAP_MEMORY
		if ((tmp = l_reallocate(DOT.l, nsize, curbp)) == 0)
			return (FALSE);
		lp1 = l_ptr(tmp);

		if (tmp->l_size == tmp->l_used) {
			chg_buff(curbp, WFEDIT);
			return WouldTruncate();
		} else {	/* assume (size > used) */
			n = Truncate(n, (SIZE_T)(tmp->l_size - tmp->l_used));
		}

		if (n > 0) {
			(void)MemMove(&tmp->l_text[doto+n], &tmp->l_text[doto], (SIZE_T)(tmp->l_used - doto));
			(void)memset(&tmp->l_text[doto],   c, (SIZE_T)n);
		}
		llength(tmp) += n;
#else
		if ((ntext=castalloc(char,nsize)) == NULL)
			return (FALSE);
		if (lp1->l_text) /* possibly NULL if l_size == 0 */
			(void)memcpy(&ntext[0], &lp1->l_text[0], (SIZE_T)doto);
		(void)memset(&ntext[doto],   c, (SIZE_T)n);
		if (lp1->l_text) {
			(void)memcpy(&ntext[doto+n], &lp1->l_text[doto],
					(SIZE_T)(lp1->l_used-doto ));
			ltextfree(lp1,curbp);
		}
		lp1->l_text = ntext;
		lp1->l_size = nsize;
		lp1->l_used += n;
#endif
	} else {		/* Easy: in place	*/
		copy_for_undo(lp1);
		chg_buff(curbp, WFEDIT);
		tmp = l_ref(lp1);
		/* don't use memcpy:  overlapping regions.... */
		llength(tmp) += n;
		if (tmp->l_used - n > doto) {
			cp2 = &tmp->l_text[tmp->l_used];
			cp1 = cp2-n;
			while (cp1 != &tmp->l_text[doto])
				*--cp2 = *--cp1;
		}
		for (i=0; i<n; ++i)		/* Add the characters	*/
			tmp->l_text[doto+i] = (char)c;
	}
	chg_buff(curbp, WFEDIT);
#if ! WINMARK
	if (same_ptr(MK.l, lp1)) {
		if (MK.o > doto)
			MK.o += n;
	}
#endif
	for_each_window(wp) {			/* Update windows	*/
		if (same_ptr(wp->w_dot.l, lp1)) {
			if (wp==curwp || wp->w_dot.o>doto)
				wp->w_dot.o += n;
		}
#if WINMARK
		if (same_ptr(wp->w_mark.l, lp1)) {
			if (wp->w_mark.o > doto)
				wp->w_mark.o += n;
		}
#endif
		if (same_ptr(wp->w_lastdot.l, lp1)) {
			if (wp->w_lastdot.o > doto)
				wp->w_lastdot.o += n;
		}
	}
	do_mark_iterate(mp,
			if (same_ptr(mp->l, lp1)) {
				if (mp->o > doto)
					mp->o += n;
			}
	);
	return (TRUE);
}

/*
 * Insert a newline into the buffer at the current location of dot in the
 * current window. The funny ass-backwards way it does things is not a botch;
 * it just makes the last line in the file not a special case. Return TRUE if
 * everything works out and FALSE on error (memory allocation failure). The
 * update of dot and mark is a bit easier then in the above case, because the
 * split forces more updating.
 */
int
lnewline()
{
	register char	*cp1;
	register char	*cp2;
	fast_ptr LINEPTR lp1;
	fast_ptr LINEPTR lp2;
	register int	doto;
	register WINDOW *wp;

	lp1  = DOT.l;			/* Get the address and	*/
	doto = DOT.o;			/* offset of "."	*/

	if (same_ptr(lp1, buf_head(curbp))
	 && same_ptr(lFORW(lp1), lp1)) {
		/* empty buffer -- just  create empty line */
		lp2 = lalloc(doto, curbp);
		if (same_ptr(lp2, null_ptr))
			return (FALSE);
		/* put lp2 in below lp1 */
		set_lFORW(lp2, lFORW(lp1));
		set_lFORW(lp1, lp2);
		set_lBACK(lFORW(lp2), lp2);
		set_lBACK(lp2, lp1);

		tag_for_undo(lp2);

		for_each_window(wp) {
			if (same_ptr(wp->w_line.l, lp1))
				wp->w_line.l = lp2;
			if (same_ptr(wp->w_dot.l, lp1))
				wp->w_dot.l = lp2;
		}

		chg_buff(curbp, WFHARD|WFINS);

		return lnewline();	/* vi really makes _2_ lines */
	}

	lp2 = lalloc(doto, curbp);	/* New first half line */
	if (same_ptr(lp2, null_ptr))
		return (FALSE);

	if (doto > 0) {
		register LINE *tmp;

		copy_for_undo(lp1);
		tmp = l_ref(lp1);
		cp1 = tmp->l_text;	/* Shuffle text around	*/
		cp2 = l_ref(lp2)->l_text;
		while (cp1 != &tmp->l_text[doto])
			*cp2++ = *cp1++;
		cp2 = tmp->l_text;
		while (cp1 != &tmp->l_text[tmp->l_used])
			*cp2++ = *cp1++;
		tmp->l_used -= doto;
	}
	/* put lp2 in above lp1 */
	set_lBACK(lp2, lBACK(lp1));
	set_lBACK(lp1, lp2);
	set_lFORW(lBACK(lp2), lp2);
	set_lFORW(lp2, lp1);

	tag_for_undo(lp2);
	dumpuline(lp1);

#if ! WINMARK
	if (same_ptr(MK.l, lp1)) {
		if (MK.o < doto)
			MK.l = lp2;
		else
			MK.o -= doto;
	}
#endif
	for_each_window(wp) {
		if (same_ptr(wp->w_line.l, lp1))
			wp->w_line.l = lp2;
		if (same_ptr(wp->w_dot.l, lp1)) {
			if (wp->w_dot.o < doto)
				wp->w_dot.l = lp2;
			else
				wp->w_dot.o -= doto;
		}
#if WINMARK
		if (same_ptr(wp->w_mark.l, lp1)) {
			if (wp->w_mark.o < doto)
				wp->w_mark.l = lp2;
			else
				wp->w_mark.o -= doto;
		}
#endif
		if (same_ptr(wp->w_lastdot.l, lp1)) {
			if (wp->w_lastdot.o < doto)
				wp->w_lastdot.l = lp2;
			else
				wp->w_lastdot.o -= doto;
		}
	}
	do_mark_iterate(mp,
			if (same_ptr(mp->l, lp1)) {
				if (mp->o < doto)
					mp->l = lp2;
				else
					mp->o -= doto;
			}
	);
	chg_buff(curbp, WFHARD|WFINS);
	return (TRUE);
}

/*
 * This function deletes "n" bytes, starting at dot. It understands how to deal
 * with end of lines, etc. It returns TRUE if all of the characters were
 * deleted, and FALSE if they were not (because dot ran into the end of the
 * buffer. The "kflag" is TRUE if the text should be put in the kill buffer.
 */
#if ANSI_PROTOS
int ldelete( B_COUNT n, int kflag)
#else
int
ldelete(n, kflag)
B_COUNT n; 	/* # of chars to delete */
int kflag;	/* put killed text in kill buffer flag */
#endif
{
	register char	*cp1;
	register char	*cp2;
	fast_ptr LINEPTR dotp;
	fast_ptr LINEPTR nlp;
	register int	doto;
	register int	chunk;
	register WINDOW *wp;
	register int i;
	register int s = TRUE;

	lines_deleted = 0;
	while (n > 0) {
		dotp = DOT.l;
		doto = DOT.o;
		if (same_ptr(dotp, buf_head(curbp))) { /* Hit end of buffer.*/
			s = FALSE;
			break;
		}
		chunk = l_ref(dotp)->l_used-doto; /* Size of chunk.	*/
		if (chunk > (int)n)
			chunk = (int)n;
		if (chunk == 0) {		/* End of line, merge.	*/
			/* first take out any whole lines below this one */
			nlp = lFORW(dotp);
			while (!same_ptr(nlp, buf_head(curbp))
			   &&  lLength(nlp)+1 < n) {
				if (kflag) {
					s = kinsert('\n');
					for (i = 0; i < lLength(nlp) &&
								s == TRUE; i++)
						s = kinsert(lGetc(nlp,i));
				}
				if (s != TRUE)
					break;
				lremove(curbp, nlp);
				lines_deleted++;
				toss_to_undo(nlp);
				n -= lLength(nlp)+1;
				nlp = lFORW(dotp);
			}
			if (s != TRUE)
				break;
			s = ldelnewline();
			chg_buff(curbp, WFHARD|WFKILLS);
			if (s != TRUE)
				break;
			if (kflag && (s = kinsert('\n')) != TRUE)
				break;
			--n;
			lines_deleted++;
			continue;
		}
		copy_for_undo(DOT.l);
		chg_buff(curbp, WFEDIT);

		cp1 = l_ref(dotp)->l_text + doto; /* Scrunch text.	*/
		cp2 = cp1 + chunk;
		if (kflag) {		/* Kill?		*/
			while (cp1 != cp2) {
				if ((s = kinsert(*cp1)) != TRUE)
					break;
				++cp1;
			}
			if (s != TRUE)
				break;
			cp1 = l_ref(dotp)->l_text + doto;
		}
		while (cp2 != l_ref(dotp)->l_text + l_ref(dotp)->l_used)
			*cp1++ = *cp2++;
		l_ref(dotp)->l_used -= chunk;
#if ! WINMARK
		if (same_ptr(MK.l, dotp) && MK.o > doto) {
			MK.o -= chunk;
			if (MK.o < doto)
				MK.o = doto;
		}
#endif
		for_each_window(wp) {		/* Fix windows		*/
			if (same_ptr(wp->w_dot.l, dotp)
			 && wp->w_dot.o > doto) {
				wp->w_dot.o -= chunk;
				if (wp->w_dot.o < doto)
					wp->w_dot.o = doto;
			}
#if WINMARK
			if (same_ptr(wp->w_mark.l, dotp)
			 && wp->w_mark.o > doto) {
				wp->w_mark.o -= chunk;
				if (wp->w_mark.o < doto)
					wp->w_mark.o = doto;
			}
#endif
			if (same_ptr(wp->w_lastdot.l, dotp)
			 && wp->w_lastdot.o > doto) {
				wp->w_lastdot.o -= chunk;
				if (wp->w_lastdot.o < doto)
					wp->w_lastdot.o = doto;
			}
		}
		do_mark_iterate(mp,
				if (same_ptr(mp->l, dotp)
				 && mp->o > doto) {
					mp->o -= chunk;
					if (mp->o < doto)
						mp->o = doto;
				}
		);
		n -= chunk;
	}
	return (s);
}

/* getctext:	grab and return a string with text from
		the current line, consisting of chars of type "type"
*/
#if OPT_EVAL
#if ANSI_PROTOS
char *getctext(CHARTYPE type)
#else
char *getctext(type)
CHARTYPE type;
#endif
{
	static char rline[NSTRING];	/* line to return */

	(void)screen_string(rline, NSTRING, type);
	return rline;
}
#endif

#if OPT_EVAL
/* putctext:	replace the current line with the passed in text	*/

int
putctext(iline)
char *iline;	/* contents of new line */
{
	register int status;

	/* delete the current line */
	DOT.o = w_left_margin(curwp); /* start at the beginning of the line */
	if ((status = deltoeol(TRUE, 1)) != TRUE)
		return(status);

	/* insert the new line */
	while (*iline) {
		if (*iline == '\n') {
			if (lnewline() != TRUE)
				return(FALSE);
		} else {
			if (linsert(1, *iline) != TRUE)
				return(FALSE);
		}
		++iline;
	}
	status = lnewline();
	backline(TRUE, 1);
	return(status);
}
#endif

/*
 * Delete a newline. Join the current line with the next line. If the next line
 * is the magic header line always return TRUE; merging the last line with the
 * header line can be thought of as always being a successful operation, even
 * if nothing is done, and this makes the kill buffer work "right". Easy cases
 * can be done by shuffling data around. Hard cases require that lines be moved
 * about in memory. Return FALSE on error and TRUE if all looks ok.
 */
static int
ldelnewline()
{
	fast_ptr LINEPTR lp1;
	fast_ptr LINEPTR lp2;
	register WINDOW *wp;
	int	len, add;

	lp1 = DOT.l;
	len = lLength(lp1);
	/* if the current line is empty, remove it */
	if (len == 0) {			/* Blank line.		*/
		toss_to_undo(lp1);
		lremove(curbp, lp1);
		return (TRUE);
	}
	lp2 = lFORW(lp1);
	/* if the next line is empty, that's "currline\n\n", so we
		remove the second \n by deleting the next line */
	/* but never delete the newline on the last non-empty line */
	if (same_ptr(lp2, buf_head(curbp)))
		return (TRUE);
	else if ((add = lLength(lp2)) == 0) {
		/* next line blank? */
		toss_to_undo(lp2);
		lremove(curbp, lp2);
		return (TRUE);
	}
	copy_for_undo(DOT.l);

	/* no room in line above, make room */
	if (add > l_ref(lp1)->l_size - len) {
#if !OPT_MAP_MEMORY
		char *ntext;
#endif
		SIZE_T nsize;
		/* first, create the new image */
		nsize = roundlenup(len + add);
#if OPT_MAP_MEMORY
		lp1 = l_ptr(l_reallocate(lp1, nsize, curbp));
		if (same_ptr(lp1, null_ptr))
			return (FALSE);
		if (len + add > l_ref(lp1)->l_size) {
			return WouldTruncate();
		}
#else
		if ((ntext=castalloc(char, nsize)) == NULL)
			return (FALSE);
		if (lp1->l_text) { /* possibly NULL if l_size == 0 */
			(void)memcpy(&ntext[0], &lp1->l_text[0], (SIZE_T)len);
			ltextfree(lp1,curbp);
		}
		lp1->l_text = ntext;
		lp1->l_size = nsize;
#endif
	}
	(void)memcpy(l_ref(lp1)->l_text + len, l_ref(lp2)->l_text, (SIZE_T)add);
#if ! WINMARK
	if (same_ptr(MK.l, lp2)) {
		MK.l  = lp1;
		MK.o += len;
	}
#endif
	/* check all windows for references to the deleted line */
	for_each_window(wp) {
		if (same_ptr(wp->w_line.l, lp2))
			wp->w_line.l = lp1;
		if (same_ptr(wp->w_dot.l, lp2)) {
			wp->w_dot.l  = lp1;
			wp->w_dot.o += len;
		}
#if WINMARK
		if (wp->w_mark.l == lp2) {
			wp->w_mark.l  = lp1;
			wp->w_mark.o += len;
		}
#endif
		if (same_ptr(wp->w_lastdot.l, lp2)) {
			wp->w_lastdot.l  = lp1;
			wp->w_lastdot.o += len;
		}
	}
	do_mark_iterate(mp,
			if (same_ptr(mp->l, lp2)) {
				mp->l  = lp1;
				mp->o += len;
			}
	);
	lLength(lp1) += add;
	set_lFORW(lp1, lFORW(lp2));
	set_lBACK(lFORW(lp2), lp1);
	dumpuline(lp1);
	toss_to_undo(lp2);
	return (TRUE);
}


static int kcharpending = -1;

/*
 * Delete all of the text saved in the kill buffer. Called by commands when a
 * new kill context is being created. The kill buffer array is released, just
 * in case the buffer has grown to immense size. No errors.
 */
void
ksetup()
{
	if ((kregflag & KAPPEND) != 0)
		kregflag = KAPPEND;
	else
		kregflag = KNEEDCLEAN;
	kchars = klines = 0;
	kregwidth = 0;
	kcharpending = -1;

}

/*
 * clean up the old contents of a kill register.
 * if called from other than kinsert, only does anything in the case where
 * nothing was yanked
 */

void
kdone()
{
	if ((kregflag & KNEEDCLEAN) && kbs[ukb].kbufh != NULL) {
		KILL *kp;	/* ptr to scan kill buffer chunk list */

		/* first, delete all the chunks */
		kbs[ukb].kbufp = kbs[ukb].kbufh;
		while (kbs[ukb].kbufp != NULL) {
			kp = kbs[ukb].kbufp->d_next;
			free((char *)(kbs[ukb].kbufp));
			kbs[ukb].kbufp = kp;
		}

		/* and reset all the kill buffer pointers */
		kbs[ukb].kbufh = kbs[ukb].kbufp = NULL;
		kbs[ukb].kused = 0;
		kbs[ukb].kbwidth = kregwidth = 0;
		kcharpending = -1;
	}
	kregflag &= ~KNEEDCLEAN;
	kbs[ukb].kbflag = kregflag;
	relist_registers();
}

int
kinsertlater(c)
int c;
{
    	int s = TRUE;
	if (kcharpending >= 0) {
		int oc = kcharpending;
		kcharpending = -1;
		s = kinsert(oc);
	}
	/* try to widen the rectangle, just in case */
	if (kregwidth > kbs[ukb].kbwidth)
		kbs[ukb].kbwidth = kregwidth;
	kcharpending = c;
	return s;
}

/*
 * Insert a character to the kill buffer, allocating new chunks as needed.
 * Return TRUE if all is well, and FALSE on errors.
 */
int
kinsert(c)
int c;		/* character to insert in the kill buffer */
{
	KILL *nchunk;	/* ptr to newly malloced chunk */
	KILLREG *kbp = &kbs[ukb];

	if (kcharpending >= 0) {
		int oc = kcharpending;
		kcharpending = -1;
		kinsert(oc);
	}

	kdone(); /* clean up the (possible) old contents */

	/* check to see if we need a new chunk */
	if (kbp->kused >= KBLOCK || kbp->kbufh == NULL) {
		if ((nchunk = typealloc(KILL)) == NULL)
			return(FALSE);
		if (kbp->kbufh == NULL)	/* set head ptr if first time */
			kbp->kbufh = nchunk;
		/* point the current to this new one */
		if (kbp->kbufp != NULL)
			kbp->kbufp->d_next = nchunk;
		kbp->kbufp = nchunk;
		kbp->kbufp->d_next = NULL;
		kbp->kused = 0;
	}

	/* and now insert the character */
	kbp->kbufp->d_chunk[kbp->kused++] = (char)c;
	kchars++;
	if (c == '\n') {
		klines++;
		if (kregwidth > kbp->kbwidth)
			kbp->kbwidth = kregwidth;
		kregwidth = 0;
	} else {
		kregwidth++;
	}
	return(TRUE);
}

/*
 * Translates the index of a register in kill-buffer list to its name.
 */
int
index2reg(c)
int	c;
{
	register int n;

	if (c >= 0 && c < 10)
		n = (c + '0');
	else if (c == KEYST_KREG)
		n = '<';
#if OPT_SELECTIONS
	else if (c == SEL_KREG)
		n = '.';
#endif
	else if (c >= 10 && c < TABLESIZE(kbs))
		n = (c - 10 + 'a');
	else
		n = '?';

	return n;
}

/*
 * Translates the name of a register into the index in kill-buffer list.
 */
int
reg2index(c)
int	c;
{
	register int n;

	if (c < 0)
		n = -1;
	else if (isdigit(c))
		n = c - '0';
	else if (islower(c))
		n = c - 'a' + 10;  /* named buffs are in 10 through 36 */
	else if (isupper(c))
		n = c - 'A' + 10;
#if OPT_SELECTIONS
	else if (c == '.')
		n = SEL_KREG;
#endif
	else if (c == '<')
		n = KEYST_KREG;
	else if (c == '"')
		n = 0;
	else
		n = -1;

	return n;
}

/*
 * Translates a kill-buffer index into the actual offset into the kill buffer,
 * handling the translation of "1 .. "9
 */
int
index2ukb(inx)
int	inx;
{
	if (inx >= 0 && inx < 10) {
		short save = ukb;
		ukb = (short)inx;
		kregcirculate(FALSE);
		inx = ukb;
		ukb = save;
	}
	return inx;
}

/* select one of the named registers for use with the following command */
/*  this could actually be handled as a command prefix, in kbd_seq(), much
	the way ^X-cmd and META-cmd are done, except that we need to be
	able to accept any of
		 3"adw	"a3dw	"ad3w
	to delete 3 words into register a.  So this routine gives us an
	easy way to handle the second case.  (The third case is handled in
	operators(), the first in main())
*/
int
usekreg(f,n)
int f,n;
{
	int c, i, status;
	char tok[NSTRING];		/* command incoming */
	static	char	cbuf[2];

	/* take care of incrementing the buffer number, if we're replaying
		a command via 'dot' */
	incr_dot_kregnum();

	if ((status = mlreply_reg("Use named register: ", cbuf, &c, -1)) != TRUE)
		return status;

	i = reg2index(c);
	if (kbm_started(i,FALSE))
		return FALSE;

	/* if we're playing back dot, let its kreg override */
	if (dotcmdmode == PLAY && dotcmdkreg != 0)
		ukb = dotcmdkreg;
	else
		ukb = (short)i;

	if (isupper(c))
		kregflag |= KAPPEND;

	if (clexec) {
		macarg(tok);	/* get the next token */
		status = execute(engl2fnc(tok), f, n);
	} else if (isnamedcmd) {
		status = namedcmd(f,n);
	} else {
		/* get the next command from the keyboard */
		c = kbd_seq();

		/* allow second chance for entering counts */
		do_repeats(&c,&f,&n);

		status = execute(kcod2fnc(c), f, n);
	}

	ukb = 0;
	kregflag = 0;

	return(status);
}

/* buffers 0 through 9 are circulated automatically for full-line deletes */
/* we re-use one of them until the KLINES flag is on, then we advance */
/* to the next */
void
kregcirculate(killing)
int killing;
{
	static	short	lastkb;	/* index of the real "0 */

	if (ukb >= 10) /* then the user specified a lettered buffer */
		return;

	/* we only allow killing into the real "0 */
	/* ignore any other buffer spec */
	if (killing) {
		if ((kbs[lastkb].kbflag & (KLINES|KRECT|KAPPEND)) &&
			! (kbs[lastkb].kbflag & KYANK)) {
			if (--lastkb < 0) lastkb = 9;
			kbs[lastkb].kbflag = 0;
		}
		ukb = lastkb;
	} else {
		/* let 0 pass unmolested -- it is the default */
		if (ukb == 0) {
			ukb = lastkb;
		} else {
		/* for the others, if the current "0 has lines in it, it
		    must be `"1', else "1 is `"1'.  get it? */
			if (kbs[lastkb].kbflag & (KLINES|KAPPEND))
				ukb = (lastkb + ukb - 1) % 10;
			else
				ukb = (lastkb + ukb) % 10;
		}
	}
}

int
putbefore(f,n)
int f,n;
{
	return doput(f, n, FALSE, EXACT);
}

int
putafter(f,n)
int f,n;
{
	return doput(f, n, TRUE, EXACT);
}

int
lineputbefore(f,n)
int f,n;
{
	return doput(f, n, FALSE, FULLLINE);
}

int
lineputafter(f,n)
int f,n;
{
	return doput(f, n, TRUE, FULLLINE);
}

int
rectputbefore(f,n)
int f,n;
{
	return doput(f, n, FALSE, RECTANGLE);
}

int
rectputafter(f,n)
int f,n;
{
	return doput(f, n, TRUE, RECTANGLE);
}

static int
doput(f,n,after,shape)
int f,n,after;
REGIONSHAPE shape;
{
	int s, oukb;

	if (!f)
		n = 1;

	oukb = ukb;
	kregcirculate(FALSE);	/* cf: 'index2ukb()' */
	if (kbs[ukb].kbufh == NULL) {
		if (ukb != 0)
			mlwarn("[Nothing in register %c]", index2reg(oukb));
		return(FALSE);
	}

	if (shape == EXACT) {
		if ((kbs[ukb].kbflag & (KLINES|KAPPEND)))
			shape = FULLLINE;
		else if (kbs[ukb].kbflag & KRECT)
			shape = RECTANGLE;
		else
			shape = EXACT;
	}

	if (shape == FULLLINE) {
		if (after && !is_header_line(DOT, curbp))
			DOT.l = lFORW(DOT.l);
		DOT.o = 0;
	} else {
		if (after && !is_at_end_of_line(DOT))
			forwchar(TRUE,1);
	}

	(void)setmark();
	s = PutChar(n, shape);
	if (s == TRUE)
		swapmark();
	if (is_header_line(DOT, curbp))
		DOT.l = lBACK(DOT.l);
	if (shape == FULLLINE)
		(void)firstnonwhite(FALSE,1);
	ukb = 0;
	return (s);
}

/* designed to be used with the result of "getoff()", which returns
 *	the offset of the character whose column is "close" to a goal.
 *	it may be to the left if the line is too short, or to the right
 *	if the column is spanned by a tab character.
 */
static int force_text_at_col P(( C_NUM, C_NUM ));

static int
force_text_at_col(goalcol, reached)
C_NUM goalcol, reached;
{
	int status = TRUE;
	if (reached < goalcol) {
		/* pad out to col */
		DOT.o = lLength(DOT.l);
		status = linsert(goalcol-reached, ' ');
	} else if (reached > goalcol) {
		/* there must be a tab there. */
		/* pad to hit column we want */
		DOT.o--;
		status = linsert(goalcol%curtabval, ' ');
	}
	return status;
}

static int next_line_at_col P(( C_NUM, C_NUM * ));

static int
next_line_at_col(col, reachedp)
C_NUM col;
C_NUM *reachedp;
{
	int s = TRUE;
	if (is_last_line(DOT,curbp)) {
		DOT.o = lLength(DOT.l);
		if (lnewline() != TRUE)
			return FALSE;
	} else {
		DOT.l = lFORW(DOT.l);
	}
	DOT.o = getoff(col, reachedp);
	return s;
}

/*
 * Put text back from the kill register.
 */
static int
PutChar(n,shape)
int n;
REGIONSHAPE shape;
{
	register int	c;
	register int	i;
	int status, wasnl, suppressnl;
	L_NUM before;
	C_NUM col = 0, width = 0;
	C_NUM reached = 0;
	int checkpad = FALSE;
	register char	*sp;	/* pointer into string to insert */
	KILL *kp;		/* pointer into kill register */

	if (n < 0)
		return FALSE;

	/* make sure there is something to put */
	if (kbs[ukb].kbufh == NULL)
		return TRUE;		/* not an error, just nothing */

	status = TRUE;
	before = line_count(curbp);
	suppressnl = FALSE;
	wasnl = FALSE;


	/* for each time.... */
	while (n--) {
		kp = kbs[ukb].kbufh;
		if (shape == RECTANGLE) {
			width = kbs[ukb].kbwidth;
			col = getcol(DOT, FALSE);
		}
#define SLOWPUT 0
#if SLOWPUT
		while (kp != NULL) {
			i = KbSize(ukb,kp);
			sp = (char *)kp->d_chunk;
			while (i--) {
				c = *sp++;
				if (shape == RECTANGLE) {
				    if (width == 0 || c == '\n') {
					    if (checkpad) {
						    status = force_text_at_col(
							    col, reached);
						    if (status != TRUE)
							    break;
						    checkpad = FALSE;
					    }
					    if (width && linsert(width, ' ') 
								!= TRUE) {
						    status = FALSE;
						    break;
					    }
					    if (next_line_at_col(col,&reached)
							    != TRUE) {
						    status = FALSE;
						    break;
					    }
					    checkpad = TRUE;
					    width = kbs[ukb].kbwidth;
				    }
				    if (c == '\n') {
					    continue; /* did it already */
				    } else {
					    if (checkpad) {
						status = force_text_at_col(
								col, reached);
						if (status != TRUE)
							break;
						checkpad = FALSE;
					    }
					    width--;

					    if (is_header_line(DOT,curbp))
						    suppressnl = TRUE;
					    if (linsert(1, c) != TRUE) {
						    status = FALSE;
						    break;
					    }
					    wasnl = FALSE;
				    }
				} else { /* not rectangle */
				    if (c == '\n') {
					    if (lnewline() != TRUE) {
						    status = FALSE;
						    break;
					    }
					    wasnl = TRUE;
				    } else {
					    if (is_header_line(DOT,curbp))
						    suppressnl = TRUE;
					    if (linsert(1, c) != TRUE) {
						    status = FALSE;
						    break;
					    }
					    wasnl = FALSE;
				    }
				}
			}
			if (status != TRUE)
				break;
			kp = kp->d_next;
		}
#else /* SLOWPUT */
		while (kp != NULL) {
			i = KbSize(ukb,kp);
			sp = (char *)kp->d_chunk;
			if (shape == RECTANGLE) {
				while (i--) {
				    c = *sp++;
				    if (width == 0 || c == '\n') {
					    if (checkpad) {
						    status = force_text_at_col(
							    col, reached);
						    if (status != TRUE)
							    break;
						    checkpad = FALSE;
					    }
					    if (width && linsert(width, ' ') 
								!= TRUE) {
						    status = FALSE;
						    break;
					    }
					    if (next_line_at_col(col,&reached)
							    != TRUE) {
						    status = FALSE;
						    break;
					    }
					    checkpad = TRUE;
					    width = kbs[ukb].kbwidth;
				    }
				    if (c == '\n') {
					    continue; /* did it already */
				    } else {
					    if (checkpad) {
						status = force_text_at_col(
								col, reached);
						if (status != TRUE)
							break;
						checkpad = FALSE;
					    }
					    width--;

					    if (is_header_line(DOT,curbp))
						    suppressnl = TRUE;
					    if (linsert(1, c) != TRUE) {
						    status = FALSE;
						    break;
					    }
					    wasnl = FALSE;
				    }
				}
			} else { /* not rectangle */
			    while (i-- > 0) {
				if (*sp == '\n') {
				    sp++;
				    if (lnewline() != TRUE) {
					status = FALSE;
					break;
				    }
				    wasnl = TRUE;
				} else {
				    register char *dp;
				    register char *ep = sp+1;
				    if (is_header_line(DOT,curbp))
					suppressnl = TRUE;
				    /* Find end of line or end of kill buffer */
				    while (i > 0 && *ep != '\n') {
					i--;
					ep++;
				    }
				    /* Open up space in current line */
				    status = linsert((int)(ep - sp), ' ');
				    if (status != TRUE)
					break;
				    dp = l_ref(DOT.l)->l_text 
					    + DOT.o - (int)(ep - sp);
				    /* Copy killbuf portion to the line */
				    while (sp < ep) {
					*dp++ = *sp++;
				    }
				    wasnl = FALSE;
				}
			    }
			}
			if (status != TRUE)
				break;
			kp = kp->d_next;
		}
#endif /* SLOWPUT */
		if (status != TRUE)
			break;
		if (wasnl) {
			if (suppressnl) {
				if (ldelnewline() != TRUE) {
					status = FALSE;
					break;
				}
			}
		} else {
			if (shape == FULLLINE && !suppressnl) {
				if (lnewline() != TRUE) {
					status = FALSE;
					break;
				}
			}
		}
	}
	curwp->w_flag |= WFHARD;
	(void)line_report(before);
	return status;
}

static int	lastreg = -1;

/* ARGSUSED */
int
execkreg(f,n)
int f,n;
{
	int c, j, jj, status;
	KILL *kp;		/* pointer into kill register */
	static	char	cbuf[2];
	int kbcount, whichkb;
	int i;
	char *sp;
	KILL *tkp;

	if (!f)
		n = 1;
	else if (n <= 0)
		return TRUE;

	if ((status = mlreply_reg("Execute register: ", cbuf, &c, lastreg)) != TRUE)
		return status;

	j = reg2index(c);
	if (kbm_started(j,TRUE))
		return FALSE;

	lastreg = c;
	relist_registers();

	/* make sure there is something to execute */
	jj = index2ukb(j);
	kp = kbs[jj].kbufh;
	if (kp == NULL)
		return TRUE;		/* not an error, just nothing */

	/* count the kchunks */
	kbcount = 0;
	tkp = kp;
	while (tkp != NULL) {
		kbcount++;
		tkp = tkp->d_next;
	}
	/* process them in reverse order */
	while (kbcount) {
		whichkb = kbcount;
		tkp = kp;
		while (--whichkb)
			tkp = tkp->d_next;
		i = KbSize(jj,tkp);
		sp = (char *)tkp->d_chunk+i-1;
		while (i--) {
			mapungetc((*sp--)|YESREMAP);
		}
		kbcount--;
	}
	return TRUE;
}

/* ARGSUSED */
int
loadkreg(f,n)
int f,n;
{
	int s;
	char respbuf[NFILEN];

	ksetup();
	s = mlreply_no_opts("Load register with: ", respbuf, sizeof(respbuf));
	if (s != TRUE)
		return FALSE;
	for (s = 0; s < NFILEN; s++) {
		if (!respbuf[s])
			break;
		if (!kinsert(respbuf[s]))
			break;
	}
	kdone();
	return TRUE;
}

/* Show the contents of the kill-buffers */
#if OPT_SHOW_REGS
#define	REGS_PREFIX	12	/* non-editable portion of the display */

static	void	makereglist P(( int, void * ));

#if OPT_UPBUFF
static	int	show_Registers P(( BUFFER * ));
static	int	show_all_chars;
#endif

/*ARGSUSED*/
static void
makereglist(iflag,dummy)
int iflag;	/* list nonprinting chars flag */
void *dummy;
{
	register KILL	*kp;
	register int	i, ii, j, c;
	register UCHAR	*p;
	int	any;

#if OPT_UPBUFF
	show_all_chars = iflag;
#endif
	b_set_left_margin(curbp, REGS_PREFIX);
	any = (reg2index(lastreg) >= 0);
	if (any)
		bprintf("last=%c", lastreg);

	for (i = 0; i < TABLESIZE(kbs); i++) {
		short	save = ukb;

		ii = index2ukb(i);
		if ((kp = kbs[ii].kbufh) != 0) {
			int first = FALSE;
			if (any++) {
				bputc('\n');
				lsettrimmed(lBack(DOT.l));
			}
			if (i > 0) {
				bprintf("%c:%*p",
					index2reg(i),
					REGS_PREFIX-2, ' ');
			} else {
				bprintf("%*S",
					REGS_PREFIX, "(unnamed)");
			}
			do {
				j = KbSize(ii,kp);
				p = kp->d_chunk;

				while (j-- > 0) {
					if (first) {
						first = FALSE;
						bprintf("%*p", REGS_PREFIX, ' ');
					}
					c = *p++;
					if (isprint(c) || !iflag) {
						bputc(c);
					} else if (c != '\n') {
						bputc('^');
						bputc(toalpha(c));
					}
					if (c == '\n') {
						first = TRUE;
						any = 0;
					} else
						any = 1;
				}
			} while ((kp = kp->d_next) != 0);
		}
		if (i < 10)
			ukb = save;
	}
	lsettrimmed(l_ref(DOT.l));
}

static int will_relist_regs;

/*ARGSUSED*/
int
showkreg(f,n)
int f,n;
{
	will_relist_regs = FALSE;
	return liststuff(REGISTERS_BufName, FALSE,
				makereglist, f, (void *)0);
}

#if OPT_UPBUFF


static int
show_Registers(bp)
BUFFER *bp;
{
	b_clr_obsolete(bp);
	return showkreg(show_all_chars, 1);
}

static void
relist_registers()
{

	if (will_relist_regs) 	/* have we already done this? */
		return;

	will_relist_regs = TRUE;

	update_scratch(REGISTERS_BufName, show_Registers);
}
#endif	/* OPT_UPBUFF */

#endif	/* OPT_SHOW_REGS */

/* For memory-leak testing (only!), releases all kill-buffer storage. */
#if NO_LEAKS
void	kbs_leaks()
{
	for (ukb = 0; ukb < TABLESIZE(kbs); ukb++) {
		ksetup();
		kdone();
	}
}
#endif
