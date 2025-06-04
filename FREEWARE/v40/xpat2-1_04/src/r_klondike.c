/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Klondike.c			     */
/*									     */
/*	Characteristics of the ``Klondike'' rules			     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

static void Klondike_newgame(void) {
    int i;
    /* specific part: */
    for (i = 0; i < rules.numslots; ++i) {
	game.ind[rules.numstacks+i+1] = game.ind[rules.numstacks+i] + i + 1;
	game.visible[game.ind[rules.numstacks+i+1]-1] = 1; /* card is turned */
    }
    game.ind[IDECK] = game.ind[VDECK];

}

static void Klondike_init(void) {
    if (rules.param[0])
	rules.move_bits |= STACK_SOURCE;
}

struct rules Klondike_rules = {
    "Klondike",	/* shortname */
    NULL,	/* longname */
    NULL,       /* abbrev */
    0,		/* layout_hints */
    DECK_SOURCE|KLONDIKE_DEAL,/* variant */
    CUSTOM_PARAM0|CUSTOM_PARAM1|CUSTOM_PARAM2,/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    7,		/* numslots */
    0,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 3, 1, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    1,		/* faceup */
    0,		/* newgame_bits */
    Klondike_newgame,/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_KINGS|US_RA|MG_RA|ST_ONE|DC_ALWAYS, /* move_bits */
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
    {0, TXTI_FLIP, 0, 0}, /* paramstring blocks */
    0,		/* used */
    Klondike_init,/* initfunc */
    NULL,	/* local keyboard bindings */
};
