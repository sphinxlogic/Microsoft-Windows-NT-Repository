/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Seahaven.c			     */
/*									     */
/*	Characteristics of the ``Seahaven Towers'' rules		     */
/*	written by Michael Bischoff					     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

static int Seahaven_valid(Cardindex src, Pileindex dstpile) {
    Pileindex srcpile;
    if (!default_movevalid(src, dstpile))
	return 0;	/* move is not valid */
    /* if we use relaxed rules, the move is valid */
    if (rules.param[0])
	return 1;	/* relaxed rules */
    /* strict rules: for slot to slot moves, there must be a sufficient
       number of tmps free */
    srcpile = getpile(src);
    if (game.piletype[srcpile] == Slot && game.piletype[dstpile] == Slot) {
	int freetmps, cardsmoved;
	cardsmoved = INDEX_OF_LAST_CARD(srcpile) + 1 - src;
	freetmps = rules.numtmps - (INDEX_OF_FIRST_CARD(
            LAST_SLOT+1+rules.numtmps) - INDEX_OF_FIRST_CARD(LAST_SLOT+1));
	if (cardsmoved-1 > freetmps)
	    return 0;
    }
    return 1;
}

struct rules Seahaven_rules = {	/* Seahaven */
    "Seahaven",	/* shortname */
    "Seahaven Towers",	/* longname */
    "st",       /* abbrev */
    1,		/* layout_hints */
    NODEAL,	/* variant */
    CUSTOM_STD|CUSTOM_PARAM0,	/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    10,		/* numslots */
    4,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    5,		/* faceup */
    0,		/* newgame_bits */
    NULL,	/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_KINGS|US_RS|MG_RS|DC_ALWAYS|ST_ONE|STACK_SOURCE, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    Seahaven_valid,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    std_good_hint,/* good_hint */
    NULL,	/* automove */
    NULL,	/* score */
    0,		/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    NULL,	/* initfunc */
    NULL,	/* local keyboard bindings */
};

struct rules FreeCell_rules = {	/* FreeCell */
    "Free Cell",/* shortname */
    NULL,	/* longname */
    "fc",       /* abbrev */
    1,		/* layout_hints */
    NODEAL,	/* variant */
    CUSTOM_STD|CUSTOM_PARAM0,	/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    8,		/* numslots */
    4,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    6,		/* faceup */
    FORCE_SLOTS,/* newgame_bits */
    NULL,	/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_ALL|US_RA|MG_RA|DC_ALWAYS|ST_ONE|STACK_SOURCE, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    Seahaven_valid,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    std_good_hint,/* good_hint */
    NULL,	/* automove */
    NULL,	/* score */
    0,		/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    NULL,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
