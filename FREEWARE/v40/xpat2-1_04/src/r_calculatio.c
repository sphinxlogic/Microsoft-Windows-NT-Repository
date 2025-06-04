/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module r_Calculation.c			     */
/*									     */
/*	Characteristics of the ``Calculation'' rules			     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	04-Apr-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

#define A 1
#define J 11
#define Q 12
#define K 13

static int order[4][13] = {
    { A,2,3,4,5,6,7,8,9,10,J,Q,K },
    { 2,4,6,8,10,Q,A,3,5,7,9,J,K },
    { 3,6,9,Q,2,5,8,J,A,4,7,10,K },
    { 4,8,Q,3,7,J,2,6,10,A,5,9,K } };

static void Calc_newgame(void) {
    int i;
    /* specific part: all cards on the vdeck, except 4 starting cards */
    for (i = 0; i <= VDECK; ++i)
	game.ind[i] = rules.numstacks * rules.faceup;
    game.ind[IDECK] = rules.numcards;
    for (i = 0; i < rules.numstacks; ++i)
	game.ind[i] = i * rules.faceup;
    for (i = 0; i < rules.numstacks; ++i) {
	/* swap cards such that first cards are OK */
	int j, rem;
	for (j = 0; j < rules.faceup; ++j) {
	    int h;
	    rem = rules.numcards;
	    do
		--rem;
	    while (RANK(game.cards[rem]) != order[i&3][j] - 1);
	    h = game.cards[i*rules.faceup+j];
	    game.cards[i*rules.faceup+j] = game.cards[rem];
	    game.cards[rem] = h;
	}
    }
    for (i = 0; i < rules.numcards; ++i)
	game.visible[i] = 1;
}

static int Calc_movevalid(Cardindex src, Pileindex dst) {
    switch (game.piletype[dst]) {
    case Stack:
	return RANK(game.cards[src]) + 1 == order[dst&3][CARDS_ON_PILE(dst)];
    case Slot:
	return game.piletype[getpile(src)] == FaceupDeck;
    default:
	return 0;
    }
}

static int Calc_good_hint(Cardindex src, Pileindex dst) {
    switch (game.piletype[dst]) {
    case Stack:
	return 100;
    case Slot:
	return 1;	/* do not display */
    default:
	return 0;
    }
}

static void Calc_check(void) {
    if (rules.faceup >= 13) {
	fprintf(stderr, "Calculation cannot be played with more than 12 faceup cards!\n");
	exit(EXIT_FAILURE);
    }
}

struct rules Calculation_rules = {
    "Calculation",/* shortname */
    NULL,	/* longname */
    "calc",     /* abbrev */
    0,		/* layout_hints */
    DECK_SOURCE|STACKS_MULTI|NODEAL,/* variant */
    CUSTOM_FACEUP,/* customizable */
    0,		/* customized */
    52,		/* numcards */
    4,		/* numstacks */
    4,		/* numslots */
    0,		/* numtmps */
    1,		/* numdecks */
    13,		/* cards_per_color */
    0,		/* numjokers */
    {0, 0, 0, 0},/* param[0], param[1], param[2], param[3] */
    0,		/* facedown */
    1,		/* faceup */
    0,		/* newgame_bits */
    Calc_newgame,/* new_game */
    NULL,	/* game_won */
    NULL,	/* new_cards */
    MG_NONE|ST_ONE|DC_ALWAYS, /* move_bits */
    NULL,	/* deal_cards */
    NULL,	/* undeal_cards */
    NULL,	/* stackable */
    Calc_movevalid,/* movevalid */
    NULL,	/* valid */
    NULL,	/* relaxed_valid */
    Calc_good_hint,/* good_hint */
    NULL,	/* automove */
    NULL,	/* score */
    0,		/* maxscore */
    {0, 0, 0, 0}, /* paramstring blocks */
    0,		/* used */
    Calc_check,	/* initfunc */
    NULL,	/* local keyboard bindings */
};
