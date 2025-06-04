/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_MonteCarlo.c			     */
/*									     */
/*	Characteristics of the ``Monte Carlo'' rules			     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	04-Apr-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

#define COLUMNS	rules.param[1]

static int MC_new_cards(void) {
    int i, hashole = 0;
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	switch (CARDS_ON_PILE(i)) {
	case 0:
	    hashole = 1;
	    break;
	case 1:
	    if (hashole)	/* 0 => 1 transition */
		return 1;
	    break;
	default:
	    return 1;
	}
    }
    return 0;
}

static Move MC_deal_cards(void) {
    Pileindex rd, wr;
    int emptybefore, turned;
    int remember_count = game.n_moves;
    store_move(COMPOUND_BEGIN);
    for (wr = rd = FIRST_SLOT; rd <= LAST_SLOT; ++rd) {
	switch (CARDS_ON_PILE(rd)) {
	case 1:
	    if (rd != wr) {
		/* shift cards to wr pile */
		store_move(do_move(INDEX_OF_FIRST_CARD(rd), wr++));
	    } else
		++wr;
	case 0:
	    break;	/* no action */
	default:
	    store_move(do_move(INDEX_OF_FIRST_CARD(rd), FIRST_STACK));
	}
    }
    /* now fill with remaining cards */
    emptybefore = EMPTY(IDECK);
    turned = 0;
    while (wr <= LAST_SLOT && !EMPTY(IDECK)) {
	store_move(do_move(INDEX_OF_LAST_CARD(IDECK), wr++));
	++turned;
    }
    if (turned)
	store_move(ADD_CHEAT | (Move)turned);

    if (EMPTY(IDECK) && !emptybefore)	/* there are no more cards now */
	draw_pileupdate(IDECK, 0);	/* force deck to be redrawn */

    game.n_moves = remember_count;
    return COMPOUND_END;	/* this will do the increment */
}

static int MC_move_valid(Cardindex src, Pileindex dstpile) {
    int i, thiscard, thatcard;
    Pileindex srcpile = getpile(src);

    if (CARDS_ON_PILE(srcpile) != 1 || CARDS_ON_PILE(dstpile) != 1)
	return 0;
    if (game.piletype[srcpile] != Slot || game.piletype[dstpile] != Slot)
	return 0;
    thiscard = game.cards[src];
    thatcard = game.cards[INDEX_OF_LAST_CARD(dstpile)];
    
    if (RANK(thiscard) != RANK(thatcard))
	return 0;

    i = (srcpile-FIRST_SLOT) % COLUMNS - (dstpile-FIRST_SLOT) % COLUMNS;
    if (i < -1 || i > 1)
	return 0;
    i = (srcpile-FIRST_SLOT) / COLUMNS - (dstpile-FIRST_SLOT) / COLUMNS;
    if (i < -1 || i > 1)
	return 0;
    return 1;
}

struct rules MonteCarlo_rules = {
    "Monte Carlo",/* shortname */
    NULL,	/* longname */
    "mc",       /* abbrev */
    3,		/* layout_hints */
    HINTS_LESSER,/* variant */
    0,		/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    25,		/* numslots */
    0,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 5, 0, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    1,		/* faceup */
    SLOTS_SAME,	/* newgame_bits */
    NULL,	/* new_game */
    NULL,	/* game_won */
    MC_new_cards,/* new_cards */
    ST_NONE|MG_NONE,	/* move_bits */
    MC_deal_cards,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    MC_move_valid,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    NULL,	/* good_hint */
    NULL,	/* automove */
    NULL,	/* score */
    0,		/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    NULL,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
