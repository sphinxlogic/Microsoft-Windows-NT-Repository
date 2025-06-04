/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module hints.c				     */
/*									     */
/*	Functions for the ``display hint'' part				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

#define RESET_HINTS        (-2)
#define ALL_HINTS          (-1)



static Move lasthint;

void cmd_DoHint(void) {
    if (lasthint == NO_MOVE) {
	show_message(TXT_NOHINTGIVEN);
    } else {
	store_move(do_move(SRCIND(lasthint), DSTPILE(lasthint)));
    }
}

#define MAXHINTS 256
static struct hintstruct {
    int srcind, dstpile;
    int value;
} hinttab[MAXHINTS];
static int hints, showhint;	/* hints is number of hints in table (-1: unknown), showhint is index */

static int hintcmp(const void *a, const void *b)
{   return ((const struct hintstruct *)b)->value - ((const struct hintstruct *)a)->value;
}

/* ind == -1 => no marked cards, show all possible moves
    otherwise show move for block ind  */

static void collect_hints(Cardindex ind)
{
    int begsrcpile, endsrcpile;
    int pile;

    if (ind != ALL_HINTS) {
	begsrcpile = endsrcpile = getpile(ind);
    } else {
	begsrcpile = 0;
	endsrcpile = game.numpiles-1;
    }
    
    /* gather all possibilities, if necessary */
    hints = 0;
    for (pile = begsrcpile; pile <= endsrcpile; pile++) {
	int dstpile, uppercard, stacksrc, havestack;

	/* check all moves which go from this pile */
	/* if pile is empty, no moves are possible */
	if (EMPTY(pile))
	    continue;
	if (pile == IDECK && !(rules.variant & DECK_VISIBLE))
	    continue;
	/* if there is a block selected already, use it */
	/* otherwise, take the maximum number of movable cards */
	uppercard = (ind == ALL_HINTS) ? maxsequence(pile, INDEX_OF_FIRST_CARD(pile)) : ind;
	if (game.piletype[pile] == Stack)
	    stacksrc = -1;	/* avoid stack to stack moves */
        else {
	    stacksrc = stackable(pile);
	    if (ind != ALL_HINTS && stacksrc != uppercard)
		stacksrc = -1;	/* block differs => no moves to stack */
	}
	havestack = 0;
	for (dstpile = 0; dstpile < game.numpiles; ++dstpile) {
	    /* in a first step, reject some moves */
	    int srcindex;
	    if (rules.variant & HINTS_LESSER)
		if (pile <= dstpile)
		    break;
	    if (pile == dstpile)
		continue;
	    switch (game.piletype[dstpile]) {
	    case Slot:
		if (EMPTY(dstpile)) {
		    if ((rules.move_bits & ES_MASK) != ES_KINGS)
			continue;  /* don't fill empty slots, if allowed for all */
		    /* now: ugly code! */
		    /* kings may be moved, if they don't come from a stack */
		    /* and they don't leave an empty slot */
		    if (game.piletype[pile] == Stack && ind == ALL_HINTS)
			continue;
		    if (game.piletype[pile] == Slot && game.ind[pile] == uppercard)
			continue;
		    /* king's move allowed! */
		}
		if (IS_JOKER(game.cards[INDEX_OF_LAST_CARD(dstpile)]))
		    continue;	    /* don't move to jokers */
		/* fall through */
		srcindex = uppercard;
		break;
	    case Tmp:
	    case FaceupDeck:
	    case FacedownDeck:
		continue;
	    case Stack:	/* for stacks, choose another srcindex */
		if (stacksrc == -1 || havestack)
		    continue;
		srcindex = stacksrc;
		break;
	    default:
		srcindex = 0;	/* to keep compiler happy */
		exit(EXIT_FAILURE);	/* cannot happen! */
	    }
	    if (move_valid(srcindex, dstpile)) {
		/* hint found. store source and destination, if acceptable  */
		hinttab[hints].value = rules.good_hint ?
		    (*rules.good_hint)(srcindex, dstpile) : 100;	/* 100 is default value */
		if (hinttab[hints].value) {
		    if (hinttab[hints].value < 2 && ind == ALL_HINTS) {
			/* don't suggest bad moves */
			continue;
		    }
		    /* accept the hint */
		    if (game.piletype[dstpile] == Stack && !(rules.variant & STACKS_MULTI))
			havestack = 1;
		    hinttab[hints].srcind = srcindex;
		    hinttab[hints].dstpile = dstpile;
		    if (++hints == MAXHINTS)
			break;
		}
	    }
	}
    }
    /* sort hints by relevance */
    qsort(hinttab, (size_t)hints, sizeof(hinttab[0]), hintcmp);
    showhint = hints;	/* will show either first or last hint */
}

/* this function returns a value (the higher the better) of the move done (or 0, if no move) */

int generic_automove(Cardindex srcindex)	/* pile is "movable" */
{   Pileindex i, srcpile;

    cmd_ResetHints();
    collect_hints(srcindex);
    if (hints) {
	store_move(do_move(hinttab[0].srcind, hinttab[0].dstpile));
	return hinttab[0].value;
    }
    /* no move to non-empty piles possible, try to move to empty slot */
    srcpile = getpile(srcindex);
    for (i = game.numpiles - 1; i >= 0; --i) {
	if (i == srcpile)
	    continue;
	if (game.piletype[i] == Tmp && game.piletype[srcpile] == Tmp)
	    continue;
	if (move_valid(srcindex, i)) {
	    store_move(do_move(srcindex, i));
	    return 1;
	}
    }
    cmd_ResetHints();	/* MUST reset the hints! */
    return 0;	/* no move => extremely ugly */
}


static void mysprintf(char *p, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
	if (*fmt != '%')
	    *p++ = *fmt++;
	else {
	    int arg;
	    arg = va_arg(args, int);
	    switch (*++fmt) {
	    case 'R':
		strcpy(p, TXT_RANK[RANK(arg)]);
		p += strlen(p);
		break;
	    case 'S':
		strcpy(p, TXT_SUIT[SUIT(arg)]);
		p += strlen(p);
		break;
	    case 'P':
		switch(game.piletype[arg]) {
		case FaceupDeck:
		    strcpy(p, TXT_DECK);
		    break;
		case FacedownDeck:
		    strcpy(p, TXT_IDECK);
		    break;
		case Slot:
		    strcpy(p, TXT_SLOT);
		    p += strlen(p);
		    sprintf(p, " %d", arg - rules.numstacks + 1);
		    break;
		case Stack:
		    strcpy(p, TXT_STACK);
		    p += strlen(p);
		    sprintf(p, " %d", arg + 1);
		    break;
		case Tmp:
		    strcpy(p, TXT_TMP);
		    p += strlen(p);
		    sprintf(p, " %d", arg-rules.numstacks-rules.numslots + 1);
		    break;
		}
		p += strlen(p);
		break;
	    default:
		*p++ = *fmt;
	    }
	    ++fmt;
	}
    }
    va_end(args);
    *p = '\0';
}

static void display_hint(struct hintstruct *p) {
    int dstpile, srcpile, srcval;
    char buf[180];

    dstpile = p->dstpile;
    srcpile = getpile(p->srcind);
    srcval = game.cards[p->srcind];

    mysprintf(buf, TXT_CANMOVE, srcpile, srcval, srcval, dstpile);
    show_message(buf);
    show_arrow(True);
}


static int forward = 1;

static int hint(Cardindex ind, boolean dodisplay) {
    show_arrow(False);	/* if any arrow visible, clear it */

    if (ind == RESET_HINTS) {
	hints = showhint = -1;
	game.arrow_srcind = -1;
	game.arrow_dstpile = -1;
	lasthint = NO_MOVE;
	return 0;
    }
    /* printf("hints = %d\n", hints); */
    if (hints < 0)
	collect_hints(ind);

    if (!hints) {
        if (dodisplay)
	    show_message(TXT_NOHINTS);
	return 0;
    } else {
	showhint = (forward ? (showhint + 1) : (showhint + hints)) % (hints+1);
	if (showhint == hints)
	    show_message(TXT_WRAPPING);
	else {
	    struct hintstruct *p;
	    p = hinttab + showhint;
	    game.arrow_srcind = p->srcind;
	    game.arrow_dstpile = p->dstpile;
	    lasthint = MOVE(p->srcind, p->dstpile);
	    if (dodisplay)
		display_hint(p);
	}
	return 1;
    }
}

void cmd_ResetHints(void) {
    hint(RESET_HINTS, 0);
}

void cmd_AllMoves(void) {
    forward = 1;
    hint(RESET_HINTS, 0);
    while (hint(ALL_HINTS, 0)) {
	/* XFlush(dpy); */
	cmd_DoHint();
	/* XFlush(dpy); */
    }
}


void cmd_NextHint(void)	{	/* show next hint in queue */
    forward = 1;
    if (game.srcind < 0)
	hint(ALL_HINTS, 1);
    else
	hint(game.srcind, 1);
}

void cmd_PreviousHint(void) {	/* show previous hint */
    forward = 0;
    if (game.srcind < 0)
	hint(ALL_HINTS, 1);
    else
	hint(game.srcind, 1);
}
