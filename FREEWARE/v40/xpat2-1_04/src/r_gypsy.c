/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Gypsy.c			     */
/*									     */
/*	Characteristics of the ``Gypsy'' rules				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

static int Gypsy_score(void) {
    Pileindex i;
    Cardindex j;
    int score;

    score = 5 * game.ind[rules.numstacks];	/* 2 points for each card on stack */
					/* offset -4 to have 100 points total */
    for (i = 0; i < rules.numstacks; ++i)
	if (CARDS_ON_PILE(i) == 13)
	    score += 60;
    for (i = 0; i < rules.numslots; ++i)
	for (j = game.ind[SLOT(i)]; j < INDEX_OF_LAST_CARD(SLOT(i)); ++j)
	    if (game.visible[j] && in_strong_sequence(game.cards[j], game.cards[j+1]))
		score += 2;
    return score >= 0 ? score : 0;
}


struct rules Gypsy_rules = {	/* Heiko's & Michael's rules */
    "Gypsy",	/* shortname */
    "Gypsy - Heiko's and Michael's patience",/* longname */
    NULL,       /* abbrev */
    0,		/* layout_hints */
    0,		/* variant */
    CUSTOM_STD, /* customizable */
    0,		/* customized */
    104,	/* numcards */
    8,		/* numstacks */
    8,		/* numslots */
    0,		/* numtmps */
    2,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[1], param[2], param[3] */
    2,		/* facedown */
    1,		/* faceup */
    0,		/* newgame_bits */
    NULL,	/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_ALL|US_RA|MG_RA|DC_ALWAYS|ST_ONE|STACK_SOURCE, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    NULL,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    std_good_hint,/* good_hint */
    NULL,	/* automove */
    Gypsy_score,/* score */
    1000,	/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    NULL,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
