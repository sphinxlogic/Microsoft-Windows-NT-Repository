/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Bakers.c			     */
/*						       			     */
/*	Characteristics of the ``Baker's Dozen'' rules			     */
/*	written by Michael Bischoff					     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

static void Bakers_new_game(void) {
    int i, flag, onstack;
    /* all cards visible */
    for (i = 0; i < rules.numcards; ++i)
	game.visible[i] = 1;
    onstack = 13 - rules.numslots;
    /* perhaps cards on the stacks */
    for (i = 0; i <= FIRST_SLOT; ++i)
	game.ind[i] = i * onstack;

    /* cards on the slots */
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i)
	game.ind[i+1] = game.ind[i] + rules.facedown + rules.faceup;
    game.ind[IDECK+1] = rules.numcards;

    /* sort: 1) cards to the stacks */
    for (i = 0; i < rules.numcards; ++i)
	while (RANK(game.cards[i]) < onstack) {
	    /* card starts on its correct place */
	    Cardindex j;
	    Card c;
	    c = game.cards[i];
	    j = onstack * SUIT(c) + RANK(c);
	    if (j == i)	/* the correct place */
		break;
	    game.cards[i] = game.cards[j];
	    game.cards[j] = c;
	}

    /* sort: 2) kings to the bottom of the piles */
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	Cardindex j;
	do {	/* ugly bubble-sort */
	    flag = 0;
	    for (j = game.ind[i]; j < INDEX_OF_LAST_CARD(i); ++j)
		if (RANK(game.cards[j]) != King && RANK(game.cards[j+1]) == King) {
		    Card c;
		    flag = 1;
		    c = game.cards[j]; game.cards[j] = game.cards[j+1];
		    game.cards[j+1] = c;
		}
	} while (flag);
    }
}

static void Bakers_init(void) {
    if (rules.numslots < 2)
	rules.numslots = 2;
    else if (rules.numslots > 13)
	rules.numslots = 13;
}

struct rules Bakers_rules = {
    "Baker's Dozen",/* shortname */
    NULL,	/* longname */
    "bd",	/* abbrev */
    7,		/* layout_hints */
    NODEAL,	/* variant */
    CUSTOM_SLOTS,/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    13,		/* numslots */
    0,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    4,		/* faceup */
    0,		/* newgame_bits */
    Bakers_new_game,/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_NONE|US_R|MG_NONE|DC_ALWAYS|ST_ONE|STACK_SOURCE, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    NULL,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    std_good_hint,/* good_hint */
    NULL,	/* automove */
    NULL,	/* score */
    0,		/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    Bakers_init,/* initfunc */
    NULL,	/* local keyboard bindings */
};
