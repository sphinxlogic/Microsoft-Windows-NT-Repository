/*
 * select.c		-- selection handling code for vile.
 *
 * Author: Kevin Buettner, Paul Fox
 * Creation: 2/26/94
 *
 * Description:  The following code is an attempt to improve the selection
 * mechanism for vile/xvile.  While my initial goal is to improve selection
 * handling for xvile, there is no reason that this code can not be used on
 * other platforms with some kind of pointing device.  In addition, the
 * mechanism is general enough to be used for other kinds of persisent
 * attributed text.
 *
 * For the purposes of this code, a selection is considered to be a region of
 * text which most applications highlight in some manner.  The user may
 * transfer selected text from one application to another (or even to the
 * same application) in some platform dependent way.  The mechanics of
 * transfering the selection are not dealt with in this file.  Procedures
 * for dealing with the representation are maintained in this file.
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/select.c,v 1.46 1995/10/01 14:44:59 pgf Exp $
 *
 */

#include	"estruct.h"
#include	"edef.h"

#if OPT_SELECTIONS

#define SAME(a,b) (same_ptr(a.l,b.l) && a.o == b.o)

extern REGION *haveregion;
extern CMDFUNC f_multimotion;

static	void	detach_attrib		P(( BUFFER *, AREGION * ));
static	void	attach_attrib		P(( BUFFER *, AREGION * ));

static	int	attribute_cntl_a_sequences P(( void ));
static	int	dot_vs_mark		P(( void ));
static	void	fix_dot			P(( void ));
static	void	output_selection_position_to_message_line P(( int ));

#if DISP_X11 && XTOOLKIT || SYS_WINNT
static	WINDOW *push_fake_win		P(( BUFFER * ));
static	void	pop_fake_win		P(( WINDOW * ));
#endif

static	int	selectregion		P(( void ));
static	REGION *extended_region		P(( void ));

/*
 * startbufp and startregion are used to represent the start of a selection
 * prior to any highlighted text being displayed.  The start and end of
 * the region will both be where the selection is to start.  Although
 * startregion is attached to the buffer indicated by startbufp, nothing
 * will be displayed since the ar_vattr field is zero.  The reason for
 * attaching it to the buffer is to force the MARKs which represent the
 * start of the region to be updated.
 *
 * selbufp and selregion are used to represent a highlighted selection.
 *
 * When starbufp or selbufp are NULL, the corresponding AREGION (startregion or
 * selregion) is not attached to any buffer and is invalid.
 */

static MARK orig_region;
static MARK plus_region;
static BUFFER *	startbufp = NULL;
static AREGION	startregion;
static BUFFER *	selbufp = NULL;
static AREGION	selregion;

typedef enum { ORIG_FIXED, END_FIXED, UNFIXED } WHICHEND;

static WHICHEND whichend;

void
free_attribs(bp)
    BUFFER *bp;
{
    AREGION *p, *q;
    p = bp->b_attribs;
    while (p != NULL) {
	q = p->ar_next;
	if (p == &selregion)
	    selbufp = NULL;
	else if (p == &startregion)
	    startbufp = NULL;
	else
	    free((char *) p);
	p = q;
    }
    bp->b_attribs = NULL;
}

void
free_attrib(bp, ap)
    BUFFER *bp;
    AREGION *ap;
{
    detach_attrib(bp, ap);
    if (ap == &selregion)
	selbufp = NULL;
    else if (ap == &startregion)
	startbufp = NULL;
    else
	free((char *) ap);
}

static void
detach_attrib(bp, arp)
    BUFFER *bp;
    AREGION *arp;
{
    if (bp != NULL) {
	WINDOW *wp;
	AREGION **rpp;
	for_each_window(wp) {
	    if (wp->w_bufp == bp)
		wp->w_flag |= WFHARD;
	}
	rpp = &bp->b_attribs;
	while (*rpp != NULL) {
	    if (*rpp == arp) {
		*rpp = (*rpp)->ar_next;
    		arp->ar_region.r_attr_id = 0;
		break;
	    }
	    else
		rpp = &(*rpp)->ar_next;
	}
    }
}

void find_release_attr(bp,rp)
BUFFER *bp;
REGION *rp;
{
    if (bp != NULL) {
	AREGION **rpp;
	rpp = &bp->b_attribs;
	while (*rpp != NULL) {
	    if ((*rpp)->ar_region.r_attr_id == rp->r_attr_id) {
		free_attrib(bp, *rpp);
		break;
	    }
	    else
		rpp = &(*rpp)->ar_next;
	}
    }
}

int
assign_attr_id()
{
    static int attr_id;
    return attr_id++;
}

static void
attach_attrib(bp, arp)
    BUFFER *bp;
    AREGION *arp;
{
    WINDOW *wp;
    arp->ar_next = bp->b_attribs;
    bp->b_attribs = arp;
    for_each_window(wp)
	if (wp->w_bufp == bp)
	    wp->w_flag |= WFHARD;
    arp->ar_region.r_attr_id = (unsigned short) assign_attr_id();
}

/*
 * Adjusts dot to last char of last line if dot is past end of buffer.  This
 * can happen when selecting with the mouse.
 */

static void
fix_dot()
{
    if (is_header_line(DOT, curwp->w_bufp)) {
	DOT.l = lBACK(DOT.l);
	DOT.o = lLength(DOT.l);
    }
}

/*
 * Output positional information regarding the selection to the message line.
 */

static void
output_selection_position_to_message_line(yanked)
    int yanked;
{
#ifdef WMDTERSELECT
    if (!w_val(curwp, WMDTERSELECT)) {
	mlwrite("(%d,%d) thru (%d,%d) %s",
		line_no(selbufp, selregion.ar_region.r_orig.l),
		getcol(selregion.ar_region.r_orig, FALSE) + 1,
		line_no(selbufp, selregion.ar_region.r_end.l),
		getcol(selregion.ar_region.r_end, FALSE),
		yanked ? "yanked" : "selected");

    }
#endif /* WMDTERSELECT */
}

/* Start a selection at dot */
int
sel_begin()
{
    fix_dot();
    detach_attrib(startbufp, &startregion);
    plus_region =
    orig_region =
    startregion.ar_region.r_orig = 
    startregion.ar_region.r_end  = DOT;
    plus_region.o += 1;
    startregion.ar_vattr = 0;
    startregion.ar_shape = EXACT;
    startbufp = curwp->w_bufp;
    attach_attrib(startbufp, &startregion);
    whichend = UNFIXED;
    return TRUE;
}

static int
dot_vs_mark()
{
	int	cmp = line_no(curbp, DOT.l) - line_no(curbp, MK.l);
	if (cmp == 0)
		cmp = DOT.o - MK.o;
	return cmp;
}

/* Extend the current selection to dot */
int
sel_extend(wiping, include_dot)
int wiping;
int include_dot;
{
	REGIONSHAPE save_shape = regionshape;
	REGION a,b;
	WINDOW *wp;
	MARK saved_dot;
	MARK working_dot;

	saved_dot = DOT;
	if (startbufp != NULL) {
		detach_attrib(selbufp, &selregion);
		selbufp = startbufp;
		selregion = startregion;
		attach_attrib(selbufp, &selregion);
		detach_attrib(startbufp, &startregion);
		startbufp = NULL;
	}

	if (curwp->w_bufp != selbufp)
		return FALSE;		/* handles NULL case also */

	fix_dot();
	regionshape = selregion.ar_shape;

	if (wiping && whichend == END_FIXED)
		MK = selregion.ar_region.r_end;
	else
		MK = selregion.ar_region.r_orig;

	/* 
	 * If we're extending in the positive direction, we want to include DOT
	 * in the selection.  To include DOT, we must advance it one char since
	 * a region runs from r_orig up to but not including r_end.
	 */
	working_dot = DOT;
	if (include_dot
	 && (selregion.ar_shape == EXACT)
	 && dot_vs_mark() >= 0) {
		if (SAME(MK, orig_region)) {
			DOT.o += 1;
		} else if (SAME(MK, plus_region)) {
			DOT.o += 1;
			MK = orig_region;
		}
	}
	getregion(&a);
	DOT = working_dot;

	/*
	 * Build a second region in the "opposite" direction.
	 */
	if (wiping && whichend == ORIG_FIXED)
		MK = selregion.ar_region.r_orig;
	else
		MK = selregion.ar_region.r_end;

	if (include_dot) {
		if (selregion.ar_shape == EXACT) {
			if (dot_vs_mark() <= 0) {
				if (SAME(MK, orig_region))
					MK.o += 1;
			}
		} else if (selregion.ar_shape == RECTANGLE) {
			if (SAME(MK, DOT)) {	/* avoid making empty-region */
				MK =  orig_region;
				DOT = plus_region;
			}
		}
	}
	getregion(&b);

	/*
	 * The two regions, 'a' and 'b' are _usually_ identical, except for the
	 * special case where we've extended one to the right to include the
	 * right endpoint of the region.
	 *
	 * For EXACT selections, setting 'whichend' to ORIG_FIXED means that
	 * we're selecting from the anchor point right/down.  Conversely,
	 * setting it to END_FIXED means that we selecting left/up.
	 *
	 * Rectangles are specified by making MK the opposite corner from DOT. 
	 * If DOT is below MK, we'll say that the selection region is
	 * ORIG_FIXED so that the next call on this function will build the
	 * regions a/b consistently.
	 *
	 * If the regions a/b are empty, we've made a mistake; this will cause
	 * the selection to be dropped in xvile.
	 */

	if (a.r_size > b.r_size) {
		whichend = ORIG_FIXED;
		selregion.ar_region = a;
	}
	else {
		if (selregion.ar_shape == RECTANGLE) {
			if (dot_vs_mark() < 0)
				whichend = END_FIXED;
			else
				whichend = ORIG_FIXED;
		} else {	/* exact or full-line */
			whichend = END_FIXED;
		}
		selregion.ar_region = b;
	}

	rls_region();
	rls_region();

	selregion.ar_vattr = VASEL | VOWN_SELECT;
	for_each_window(wp) {
		if (wp->w_bufp == selbufp)
			wp->w_flag |= WFHARD;
	}

	output_selection_position_to_message_line(FALSE);

	regionshape = save_shape;
	DOT = saved_dot;
	OWN_SELECTION();
	return TRUE;
}

/*
 * Detach current selection (if attached) and null the associated buffer
 * pointer.
 */
void
sel_release()
{
    detach_attrib(selbufp, &selregion);
    selbufp = NULL;
}

/*
 * Assert/reassert ownership of selection if appropriate.  This is necessary
 * in order to paste a selection after it's already been pasted once and then
 * modified.
 */

void
sel_reassert_ownership(bp)
    BUFFER *bp;
{
    if (selbufp == bp) {
	OWN_SELECTION();
    }
}

/*
 * Allocate a fake window so that we can yank a selection even if the buffer
 * containing the selection is not attached to any window.
 *
 * curwp is set to the new fake window.  A pointer to the old curwp is returned
 * for a later call to pop_fake_win() which will restore curwp.
 *
 * FIXME: These two functions should maybe be moved to window.c so that they are
 * together with the rest of the window manipulation code.
 */

#if DISP_X11 && XTOOLKIT || SYS_WINNT
static WINDOW *
push_fake_win(bp)
    BUFFER *bp;
{
    WINDOW *oldwp = curwp;
    WINDOW *wp;
    if ((wp = typealloc(WINDOW)) == NULL) {
	    (void)no_memory("WINDOW");
	    return NULL;
    }
    curwp = wp;
    curwp->w_bufp = bp;
    curwp->w_bufp->b_nwnd++;
    if ((wp = bp2any_wp(bp)) == NULL)
	copy_traits(&(curwp->w_traits), &(bp->b_wtraits));
    else
	copy_traits(&(curwp->w_traits), &(wp->w_traits));
    curwp->w_flag  = 0;
    curwp->w_force = 0;
    curwp->w_toprow = wheadp->w_toprow - 2;	/* should be negative */
    curwp->w_ntrows = 1;
    curwp->w_wndp = wheadp;
    wheadp = curwp;
    return oldwp;
}

/*
 * kill top fake window allocated by alloc_fake_win
 */
static void
pop_fake_win(oldwp)
    WINDOW *oldwp;
{
    WINDOW *wp;
    curwp = oldwp;

    wp = wheadp;
    if (wp->w_toprow >= 0)
	return;					/* not a fake window */
    /* 
     * Decrement the window count, but don't update the traits.  We want
     * to give as little indication as possible that a fake window was
     * created.  In particular, should the user go back to a buffer
     * which is not currently displayed, DOT should be where he last 
     * left it.
     */
    --wp->w_bufp->b_nwnd;
    /* unlink and free the fake window */
    wheadp = wp->w_wndp;
    free((char *)wp);
}

/* 
 * Yank the selection.  Return TRUE if selection could be yanked, FALSE
 * otherwise.  Note that this code will work even if the buffer being
 * yanked from is not attached to any window since it creates its own
 * fake window in order to perform the yanking.
 */

int
sel_yank(reg)
int reg;
{
    REGIONSHAPE save_shape;
    WINDOW *save_wp;

    if (selbufp == NULL)
	return FALSE;			/* No selection to yank */

    if ((save_wp = push_fake_win(selbufp)) == NULL)
	return FALSE;

    save_shape = regionshape;
    ukb = reg;
    kregflag = 0;
    haveregion = &selregion.ar_region;
    regionshape = selregion.ar_shape;
    yankregion();
    haveregion = NULL;
    regionshape = save_shape;
    pop_fake_win(save_wp);
    output_selection_position_to_message_line(TRUE);

    /* put cursor back on screen...is there a cheaper way to do this?  */
    (void)update(FALSE);
    return TRUE;
}

#if NEEDED
int
sel_attached()
{
    return startbufp == NULL;
}
#endif  /* NEEDED */

BUFFER *
sel_buffer()
{
    return (startbufp != NULL) ? startbufp : selbufp;
}
#endif  /* DISP_X11 && XTOOLKIT || SYS_WINNT */

int
sel_setshape(shape)
    REGIONSHAPE shape;
{
    if (startbufp != NULL) {
	startregion.ar_shape = shape;
	return TRUE;
    }
    else if (selbufp != NULL) {
	selregion.ar_shape = shape;
	return TRUE;
    }
    else {
	return FALSE;
    }
}

/* return a region which goes from DOT to the far end of the current
	selection region.  shape is maintained.  returns pointer to static
	region struct.
*/
static REGION *
extended_region()
{
    REGION *rp = NULL;
    static REGION a, b;
    MARK savemark;

    savemark = MK;
    regionshape = selregion.ar_shape;
    MK = selregion.ar_region.r_orig;
    DOT.o += 1;
    if (getregion(&a) == TRUE) {
        DOT.o -= 1;
	MK = selregion.ar_region.r_end;
	if (regionshape == FULLLINE)
	    MK.l = lBACK(MK.l);
	/* region b is to the end of the selection */
	if (getregion(&b) == TRUE) {
	    /* if a is bigger, it's the one we want */
	    if (a.r_size > b.r_size)
		rp = &a;
	    else
		rp = &b;
	}
	rls_region();
    } else {
	DOT.o -= 1;
    }
    rls_region();
    MK = savemark;
    return rp;
}

static	int	doingopselect;

/* ARGSUSED */
int
sel_motion(f,n)
int f,n;
{
    if (selbufp == NULL) {
	mlwrite("[No selection exists.]");
	return FALSE;
    }

    if (selbufp != curbp) {
	/* FIXME -- sure would be nice if we could do non-destructive
		things to other buffers, mainly yank. */
	mlwrite("[Selection not in current buffer.]");
	return FALSE;
    }

    curwp->w_flag |= WFMOVE;

    /* if this is happening on behalf of an operator, we're pretending
     * that the motion took us from one end of the selection to the
     * other, unless we're trying to select to the selection, in which
     * case that would be self-defeating
     */
    if (doingopcmd && !doingopselect) {
	pre_op_dot = selregion.ar_region.r_orig;  /* move us there */
	haveregion = &selregion.ar_region;
	regionshape = selregion.ar_shape;
	return TRUE;
    }

    if (!doingopcmd) { /* it's a simple motion -- go to the top of selection */
	/* remember -- this can never be used with an operator, as in
	 * "delete to the selection", since that case is taken care
	 * of above, and is really the whole reason for this
	 * "motion" in the first place.  */
	DOT = selregion.ar_region.r_orig;  /* move us there */
	return TRUE;
    }

    /* we must be doing an extension */
    haveregion = extended_region();
    return haveregion ? TRUE:FALSE;

}

static int
selectregion()
{
	register int    status;
	REGION          region;
	MARK		savedot;
	MARK		savemark;
	int		hadregion = FALSE;

	savedot = DOT;
	savemark = MK;
	if (haveregion) {	/* getregion() will clear this, so 
					we need to save it */
		region = *haveregion;
		hadregion = TRUE;
	}
	status = yankregion();
	DOT = savedot;
	MK = savemark;
	if (status != TRUE)
		return status;
	if (hadregion || ((status = getregion(&region)) == TRUE)) {
	    detach_attrib(startbufp, &startregion);
	    detach_attrib(selbufp, &selregion);
	    selbufp = curbp;
	    selregion.ar_region = region;
	    selregion.ar_vattr = VASEL | VOWN_SELECT;
	    selregion.ar_shape = regionshape;
	    attach_attrib(selbufp, &selregion);
	    OWN_SELECTION();
	}
	rls_region();
	return status;
}

int
multimotion(f,n)
int f,n;
{
	CMDFUNC	*cfp;
	int s,c,waserr;
	REGIONSHAPE shape;
	MARK savedot;
	MARK savemark;
	MARK realdot;
	char	temp[NLINE];
	static int wassweephack = FALSE;

	/* Use the repeat-count as a shortcut to specify the type of selection. 
	 * I'd use int-casts of the enum value, but declaring enums with
	 * specific values isn't 100% portable.
	 */
	if (!f || n <= 0)
		n = 1;
	if (n == 3)
		regionshape = RECTANGLE;
	else if (n == 2)
		regionshape = FULLLINE;
	else
		regionshape = EXACT;
	shape = regionshape;

	sweephack = FALSE;
	savedot = DOT;
	if (doingsweep) { /* the same command terminates as starts the sweep */
		doingsweep = FALSE;
		mlforce("[Sweeping: Completed]");
		regionshape = shape;
		/* since the terminating 'q' is executed as a motion, we have
		   now lost the value of sweephack we were interested in, the
		   one that tells us to include DOT.o in the selection.
		   so we preserved it in wassweephack, and restore it here.
		 */
		if (wassweephack)
			sweephack = wassweephack;
		return TRUE;
	} else {
		kcod2pstr(fnc2kcod(&f_multimotion), temp);
		doingsweep = TRUE;
		mlwrite("[Begin cursor sweep... (end with %*S)]",*temp,temp+1);
		(void)sel_begin();
		(void)sel_setshape(shape);
	}

	waserr = TRUE; /* to force message "state-machine" */

	while (doingsweep) {

		/* Fix up the screen	*/
		s = update(FALSE);

		/* get the next command from the keyboard */
		c = kbd_seq();

		if (ABORTED(c)) {
			doingsweep = FALSE;
			mlforce("[Sweeping: Aborted]");
			sel_release();
			return FALSE;
		}

		f = FALSE;
		n = 1;

		do_repeats(&c,&f,&n);

		/* and execute the command */
		cfp = kcod2fnc(c);
		if ( (cfp != NULL)
		 && ((cfp->c_flags & MOTION) != 0)) {
			wassweephack = sweephack;
			sweephack = FALSE;
			s = execute(cfp, f, n);
			if (s != TRUE) {
				mlforce(
				"[Sweeping: Motion failed. (end with %*S)]",*temp,temp+1);
				waserr = TRUE;
			} else {
				if (waserr && doingsweep) {
					mlforce("[Sweeping... (end with %*S)]",*temp,temp+1);
					waserr = FALSE;
				}
				realdot = DOT;
				DOT = savedot;
				(void)sel_begin();
				DOT = realdot;
				(void)sel_setshape(shape);
				/* we sometimes want to include DOT.o in the
				   selection (unless it's a rectangle, in
				   which case it's taken care of elsewhere)
				 */
				sel_extend(TRUE,(regionshape != RECTANGLE &&
					sweephack));
			}
		 } else {
			mlforce(
			"[Sweeping: Only motions permitted (end with %*S)]",*temp,temp+1);
			waserr = TRUE;
		 }

	}
	regionshape = shape;
	/* if sweephack is set here, it's because the last motion had
		it set */
    	if (doingopcmd)
		pre_op_dot = savedot;
	savedot = DOT;
	savemark = MK;
	if ((regionshape != RECTANGLE) && sweephack) {
		if (dot_vs_mark() < 0)
			MK.o += 1;
		else
    			DOT.o += 1;
	}
	s = yankregion();
	DOT = savedot;
	MK = savemark;
	sweephack = wassweephack = FALSE;
	return s;
}

/*ARGSUSED*/
int
multimotionfullline(f,n)
int f,n;
{
	return multimotion(TRUE,2);
}

/*ARGSUSED*/
int
multimotionrectangle(f,n)
int f,n;
{
	return multimotion(TRUE,3);
}

int
attributeregion()
{
	register int    status;
	REGION          region;
	AREGION *	arp;

	if ((status = getregion(&region)) == TRUE) {
	    if (VATTRIB(videoattribute) != 0) {	/* add new attribute-region */
		if ((arp = typealloc(AREGION)) == NULL) {
		    (void)no_memory("AREGION");
		    rls_region();
		    return FALSE;
		}
	    	arp->ar_region = region;
	    	arp->ar_vattr = videoattribute; /* include ownership */
	    	arp->ar_shape = regionshape;
	    	attach_attrib(curbp, arp);
	    } else { /* purge attributes in this region */
		L_NUM first = line_no(curbp, region.r_orig.l);
		L_NUM last  = line_no(curbp, region.r_end.l);
		AREGION *p, *q;
		int owner;

		owner = VOWNER(videoattribute);

		for (p = curbp->b_attribs, q = 0; p != 0; p = q) {
		    L_NUM f0, l0;

		    q = p->ar_next;

		    if (owner != 0 && owner != VOWNER(p->ar_vattr))
		    	continue;

		    f0 = line_no(curbp, p->ar_region.r_orig.l);
		    l0 = line_no(curbp, p->ar_region.r_end.l);

		    if (l0 < first || f0 > last)
		    	continue;	/* no overlap */

		    /* FIXME: this removes the whole of an overlapping region;
		     * we really only want to remove the overlapping portion...
		     */
		    detach_attrib(curbp, p);
		}
	    }
	}
	rls_region();
	return status;
}

int
operselect(f,n)
int f,n;
{
	int s;
	opcmd = OPOTHER;
	doingopselect = TRUE;
	s = operator(f,n,selectregion,"Select");
	doingopselect = FALSE;
	return s;
}

int
operattrbold(f,n)
int f,n;
{
      opcmd = OPOTHER;
      videoattribute = VABOLD | VOWN_OPERS;
      return operator(f,n,attributeregion,"Set bold attribute");
}

int
operattrital(f,n)
int f,n;
{
      opcmd = OPOTHER;
      videoattribute = VAITAL | VOWN_OPERS;
      return operator(f,n,attributeregion,"Set italic attribute");
}

int
operattrno(f,n)
int f,n;
{
      opcmd = OPOTHER;
      videoattribute = 0;	/* clears no matter who "owns" */
      return operator(f,n,attributeregion,"Set normal attribute");
}

int
operattrul(f,n)
int f,n;
{
      opcmd = OPOTHER;
      videoattribute = VAUL | VOWN_OPERS;
      return operator(f,n,attributeregion,"Set underline attribute");
}
  

int
operattrcaseq(f,n)
int f,n;
{
      opcmd = OPOTHER;
      videoattribute = VAUL | VOWN_CTLA;
      return operator(f,n,attribute_cntl_a_sequences,
                      "Attribute ^A sequences");
}
  
/*
 * attribute_cntl_a_sequences can take quite a while when processing a region
 * with a large number of attributes.  The reason for this is that the number
 * of marks to check for fixing (from ldelete) increases with each attribute
 * that is added.  It is not really necessary to check the attributes that
 * we are adding in attribute_cntl_a_sequences due to the order in which
 * they are added (i.e, none of them ever need to be fixed up when ldelete
 * is called from within attribute_cntl_a_sequences).
 *
 * It is still necessary to update those attributes which existed (if any)
 * prior to calling attribute_cntl_a_sequences.
 *
 * We define EFFICIENCY_HACK to be 1 if we want to enable the code which
 * will prevent ldelete from doing unnecessary work.  Note that we are
 * depending on the fact that attach_attrib() adds new attributes to the
 * beginning of the list.  It is for this reason that I consider this
 * code to be somewhat hacky.
 */
#define EFFICIENCY_HACK 1

static int
attribute_cntl_a_sequences()
{
    register int c;		/* current char during scan */
    fast_ptr LINEPTR pastline;	/* pointer to line just past EOP */
    C_NUM offset;		/* offset in cur line of place to attribute */

#if EFFICIENCY_HACK
    AREGION *orig_attribs;
    AREGION *new_attribs;
    orig_attribs = new_attribs = curbp->b_attribs;
#endif

    if (!sameline(MK, DOT)) {
	REGION region;
	if (getregion(&region) != TRUE)
	    return FALSE;
	if (sameline(region.r_orig, MK))
	    swapmark();
	rls_region();
    }
    pastline = MK.l;
    if (!same_ptr(pastline, win_head(curwp)))
	    pastline = lFORW(pastline);
    DOT.o = 0;
    regionshape = EXACT;
    while (!same_ptr(DOT.l, pastline)) {
	while (DOT.o < lLength(DOT.l)) {
	    if (char_at(DOT) == CONTROL_A) {
		int count = 0;
		offset = DOT.o+1;
start_scan:
		while (offset < lLength(DOT.l)) {
		    c = lGetc(DOT.l, offset);
		    if (isdigit(c)) {
			count = count * 10 + c - '0';
			offset++;
		    }
		    else
			break;
		}
		if (count == 0)
		    count = 1;
		videoattribute = VOWN_CTLA;
		while (offset < lLength(DOT.l)) {
		    c = lGetc(DOT.l, offset);
		    switch (c) {
			case 'C' :
			    /* We have color. Get color value */
			    offset++;
			    c = lGetc(DOT.l, offset);
			    if (isdigit(c))
				videoattribute |= VCOLORATTR(c - '0');
			    else if ('A' <= c && c <= 'F')
				videoattribute |= VCOLORATTR(c - 'A' + 10);
			    else if ('a' <= c && c <= 'f')
				videoattribute |= VCOLORATTR(c - 'a' + 10);
			    else
				offset--; /* Invalid attribute */
			    break;
			case 'U' : videoattribute |= VAUL;   break;
			case 'B' : videoattribute |= VABOLD; break;
			case 'R' : videoattribute |= VAREV;  break;
			case 'I' : videoattribute |= VAITAL; break;
			case ':' : offset++;
			    if (lGetc(DOT.l, offset) == CONTROL_A) {
				count = 0;
				offset++;
				goto start_scan; /* recover from filter-err */
			    }
			    /* FALLTHROUGH */
			default  : goto attribute_found;
		    }
		    offset++;
		}
attribute_found:
#if EFFICIENCY_HACK
		new_attribs = curbp->b_attribs;
		curbp->b_attribs = orig_attribs;
		ldelete((B_COUNT)(offset - DOT.o), FALSE);
		curbp->b_attribs = new_attribs;
#else
		ldelete((B_COUNT)(offset - DOT.o), FALSE);
#endif
		MK = DOT;
		MK.o += count;
		if (MK.o > lLength(DOT.l))
		    MK.o = lLength(DOT.l);
		if (VATTRIB(videoattribute))
		    (void) attributeregion();
	    }
	    DOT.o++;
	}
	DOT.l = lFORW(DOT.l);
	DOT.o = 0;
    }
    return TRUE;
}
#endif /* OPT_SELECTIONS */
