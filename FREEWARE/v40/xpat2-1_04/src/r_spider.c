/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Spider.c			     */
/*									     */
/*	Characteristics of the ``Spider'' rules				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

/* calculate score according to Spider documentation */
static int Spider_score(void) {
    Pileindex i;
    Cardindex j;
    int score;
    int total_visible;
    int resolved_suits_in_slots=0;
    int resolved_suits_in_stacks=0;

    /* set score to the maximum of facedown cards turned */
    score = ((rules.numcards % rules.numslots) + 
	      rules.numslots * rules.facedown) * 10;

    for (i = FIRST_SLOT; i <= LAST_SLOT; i++) {
	total_visible = 1;
	for (j = game.ind[i]; j < INDEX_OF_LAST_CARD(i); j++) {
	    if (game.visible[j] &&
		in_strong_sequence(game.cards[j], game.cards[j+1])) {
		/* visible combined cards give 2 Points */
		score += 2;
	    }
	    if (!game.visible[j]) {
		/* each of the unvisible cards at start got 10 Points at first.
		   Now clear the points for each yet invisible card. */
		score -= 10;
		total_visible = 0;     /* something is hidden in this slot */
	    }
	}
	/* 15 points for each fully visible slot */
	score += total_visible * 15;

	/* check for completed suits (even if stacked on each other) */
	if (!EMPTY(i)) {
	    /* each full sequence gets 50 points total 
	       (12 * 2 points are already there) so add 26 points. */
	    Cardindex k;
	    for (k = INDEX_OF_LAST_CARD(i); k >= game.ind[i] + 12; --k) {
		if (game.visible[k-12] && complete_suit(i, k) != -1) {
		    score += 26;
		    ++resolved_suits_in_slots;
		}
	    }
	}
    }
    /* check the stacks for complete sequences */
    for (i = 0; i < rules.numstacks; ++i)
	if (!EMPTY(STACK(i))) {
	    score += 50;	/* each completed sequence got 50 points */
	    resolved_suits_in_stacks ++;
	}

    if ((resolved_suits_in_slots + resolved_suits_in_stacks) / 4 /*suits*/ == 
	rules.numdecks) {
	resolved_suits_in_slots -= 3;
	if (resolved_suits_in_slots > 0) 
	    score += resolved_suits_in_slots * 2;
    }
    return score;
}

static void Spider_init(void) {
    switch (rules.param[0]) {
    case 0:
	rules.move_bits |= DC_NOEMPTY;		/* std. Spider rules */
	break;
    case 1:
	rules.move_bits |= DC_STRONGOK;		/* Heiko's variant */
	break;
    case 2:
	rules.move_bits |= DC_RELAXEDOK;	/* Heiko's variant */
	break;
    default:
	fprintf(stderr, "Spider: option to -relaxed may be 0, 1, or 2. Found %d\n", rules.param[0]);
	exit(1);
    }
}


struct rules Spider_rules = {
    "Spider",	/* shortname */
    NULL,	/* longname */
    NULL,       /* abbrev */
    0,		/* layout_hints */
    0,		/* variant */
    CUSTOM_STD|CUSTOM_PARAM0,	/* customizable */
    0,		/* customized */
    104,	/* numcards */
    8,		/* numstacks */
    10,		/* numslots */
    0,		/* numtmps */
    2,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[0], param[1], param[2], param[3] */
    4,		/* facedown */
    1,		/* faceup */
    0,		/* newgame_bits */
    NULL,	/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    ES_ALL|US_R|MG_RS|ST_13, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    NULL,	/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    std_good_hint,	/* good_hint */
    NULL,	/* automove */
    Spider_score,/* score */
    1000,	/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    Spider_init,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
