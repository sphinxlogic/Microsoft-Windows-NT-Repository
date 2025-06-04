/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Delight.c			     */
/*									     */
/*	Characteristics of the ``Idiot's Delight'' rules		     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	31-Mar-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

#define ACE_HIGHEST

static int ID_won(void) {
    if (!EMPTY(IDECK))
	return 0;
    /* winning condition: */
    /* there must be exactly 4 cards left on the slots! */
    if (INDEX_OF_LAST_CARD(LAST_SLOT) - INDEX_OF_FIRST_CARD(FIRST_SLOT) != 3)
	return 0;
    return 1;
}

static int ID_move_valid(Cardindex src, Pileindex dstpile) {
    int i, thiscard;
    Pileindex srcpile;

    srcpile = getpile(src);
    if (game.piletype[srcpile] != Slot)
	return 0;
    if (game.piletype[dstpile] == Slot)
	return EMPTY(dstpile);
    if (game.piletype[dstpile] != Stack)
	return 0;
    /* slot to stack: must exist card of greater rank */
    thiscard = game.cards[src];
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	int thatcard;
	thatcard = game.cards[INDEX_OF_LAST_CARD(i)];
	if (SUIT(thatcard) == SUIT(thiscard) &&
#ifndef ACE_HIGHEST
	    RANK(thatcard) > RANK(thiscard)
#else
	    /* wrap order */
	    (12+RANK(thatcard))%13 > (12+RANK(thiscard))%13
#endif
	    )
	    return 1;
    }
    return 0;
}

struct rules IdiotsDelight_rules = {
    "Idiot's Delight",		/* shortname */
    NULL,	/* longname */
    "id",       /* abbrev */
    2,		/* layout_hints */
    0,		/* variant (DECK_SOURCE) */
    0,		/* customizable */
    0,		/* customized */
    52,		/* numcards */
    1,		/* numstacks */
    4,		/* numslots */
    0,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    1,		/* faceup */
    0,		/* newgame_bits */
    NULL,	/* new_game */
    ID_won,	/* game_won */
    NULL,	/* new_cards */
    ES_ALL|US_NONE|MG_NONE|DC_ALWAYS|ST_ONE, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    ID_move_valid,/* movevalid */
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
