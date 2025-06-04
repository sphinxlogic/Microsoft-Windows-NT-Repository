/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Michaels.c			     */
/*									     */
/*	Characteristics of the ``Michael's Fantasy'' rules		     */
/*	written by Michael Bischoff					     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"


struct rules Michaels_rules = {
    "Michael's Fantasy",/* shortname */
    NULL,	/* longname */
    "mf",       /* abbrev */
    5,		/* layout_hints */
    NODEAL,	/* variant */
    CUSTOM_PARAM3|CUSTOM_FACEUP|CUSTOM_FACEDOWN|CUSTOM_SLOTS,/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    6,		/* numslots */
    4,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 5},/* param[0], param[1], param[2], param[3] */
    3,		/* facedown */
    5,		/* faceup */
    FORCE_SLOTS,/* newgame_bits */
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
    NULL,	/* score */
    0,		/* maxscore */
    {0, 0, 0, TXTI_ROTATE}, /* paramstring blocks */
    0,		/* used */
    NULL,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
