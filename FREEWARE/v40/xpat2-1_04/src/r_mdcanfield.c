/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Canfield.c			     */
/*									     */
/*	Characteristics of the ``Canfield'' rules			     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"


static int baserank;

static int Canfield_valid(Cardindex srcind, Pileindex dstpile) {
    int srcpile, dstcard, srccard;

    srcpile = getpile(srcind);
    if (srcpile == dstpile)
	return 0;
    if (game.piletype[srcpile] == Stack)
	return 0;
    dstcard = EMPTY(dstpile) ? -1 : game.cards[game.ind[dstpile+1]-1];
    srccard = game.cards[srcind];
    switch (game.piletype[dstpile]) {
    case Slot:	/* moves to Slot: depends on if slot is empty */
	if (EMPTY(dstpile))
	    return srcpile == IDECK || (EMPTY(IDECK) && srcpile == VDECK);
	/* dstpile not empty: rank and alternate colour! */
	if (!in_relaxed_sequence(srccard, dstcard))
	    return 0;
	if (game.piletype[srcpile] == Slot &&
	    srcind != INDEX_OF_FIRST_CARD(srcpile))
	    return 0;
	return 1;
    case Tmp:
    case FaceupDeck:
    case FacedownDeck:
	return 0;	/* only implicit moves allowable */
    case Stack:
	if (srcind != INDEX_OF_LAST_CARD(srcpile))
	    return 0;	/* only one card at a time */
	if (SUIT(srccard) != SUIT(dstpile))
	    return 0;
	if (EMPTY(dstpile))
	    return RANK(srccard) == baserank;
	return RANK(srccard) == (1 + RANK(
		  game.cards[INDEX_OF_LAST_CARD(dstpile)])) % 13;
    }
    return 0;
}

static void Canfield_newgame(void) {
    int i;
    int firstsuit;
    /* specific part: one card on a stack, 3 on the talon (VDECK) */
    for (i = 0; i < rules.numcards; ++i)
	game.visible[i] = 1;
    firstsuit = SUIT(game.cards[0]);
    baserank = RANK(game.cards[0]);
    for (i = 0; i <= firstsuit; ++i)
	game.ind[i] = 0;
    for (; i <= rules.numstacks; ++i)
	game.ind[i] = 1;
    for (; i <= rules.numstacks + rules.numslots; ++i)
	game.ind[i] = 1 + i - rules.numstacks;
    game.ind[VDECK] = 1 + rules.numslots;
    game.ind[IDECK] = 4 + rules.numslots;	/* => 3 on the talon */
    game.ind[IDECK+1] = rules.numcards;
}

static int Canfield_score(void) {
    return -52 + 5 * (game.ind[rules.numstacks] - game.counter[1]);
}

struct rules modCanfield_rules = {
    "modCanfield",	/* shortname */
    "modified Canfield",/* longname */
    "modcan",   /* abbrev */
    0,		/* layout_hints */
    DECK_SOURCE|DECK_VISIBLE|KLONDIKE_DEAL,/* variant */
    0,		/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    4,		/* numslots */
    0,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 999, 3, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    1,		/* faceup */
    0,		/* newgame_bits */
    Canfield_newgame,/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_ALL|US_RA|MG_RA|ST_ONE|DC_ALWAYS, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    Canfield_valid,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    NULL,	/* good_hint */
    NULL,	/* automove */
    Canfield_score,/* score */
    208,	/* maxscore */
    {0, TXTI_FLIP, 0, 0}, /* paramstring blocks */
    0,		/* used */
    NULL,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
