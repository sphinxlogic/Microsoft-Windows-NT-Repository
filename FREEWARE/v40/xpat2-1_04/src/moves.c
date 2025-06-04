/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module moves.c				     */
/*									     */
/*	General utility functions for all rule sets			     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

#include <time.h>
#include <limits.h>	/* for INT_MAX */
#include <math.h>

/* RNG patch: */
int rng = 1;
static int myrand(int limit) {
    /* select one of the RNGs. */
    return rng ?
	(int)(((double)(prand()) / (double)PRANDMAX) * (double)limit)
	    : prand() % limit;
}

static Move Klondike_deal_cards(void) {
    int rem = CARDS_ON_PILE(IDECK);
    
    if (rem > rules.param[2])
	rem = rules.param[2];
    graphics_pile_control(Disable, IDECK);
    graphics_pile_control(Disable, VDECK);
    if (rem) {
	int i;
	/* without graphics: */
	for (i = 0; i < rem; ++i)
	    do_move(INDEX_OF_LAST_CARD(IDECK), VDECK);
    } else {
	while (!EMPTY(VDECK))
	    do_move(INDEX_OF_LAST_CARD(VDECK), IDECK);
	++game.counter[1];
    }
    graphics_pile_control(EnableAndRedraw, IDECK);
    graphics_pile_control(EnableAndRedraw, VDECK);
    return NEW_CARDS_MOVE | MOVE(0, rem);
}

static void Klondike_undeal_cards(int num) {
    graphics_pile_control(Disable, IDECK);
    graphics_pile_control(Disable, VDECK);
    if (!num) {
	/* undo flip */
	while (!EMPTY(IDECK))
	    do_move(INDEX_OF_LAST_CARD(IDECK), VDECK);
	--game.counter[1];
    } else {
	int i;
	for (i = 0; i < num; ++i)
	    do_move(INDEX_OF_LAST_CARD(VDECK), IDECK);
    }
    graphics_pile_control(EnableAndRedraw, IDECK);
    graphics_pile_control(EnableAndRedraw, VDECK);
}

Move SlotShuffle(int fwd) {
    int i, tab[MAXCARDS];
    int num = INDEX_OF_FIRST_CARD(LAST_SLOT+1)
	- INDEX_OF_FIRST_CARD(FIRST_SLOT);
    Cardindex start = INDEX_OF_FIRST_CARD(FIRST_SLOT);

    sprand(game.seed);
    for (i = 0; i < num; ++i) {
	int j;
	j = myrand(num);
	if (j < i)	/* some correction */
	    j = i;
	tab[i] = j;
    }
    if (fwd) {
	for (i = 0; i < num; ++i)
	    if (tab[i] != i) {
		/* swap card */
		Card c;
		c = game.cards[start+i];
		game.cards[start+i] = game.cards[start+tab[i]];
		game.cards[start+tab[i]] = c;
	    }
    } else {
	for (i = num-1; i >= 0; --i)
	    if (tab[i] != i) {
		/* swap card */
		Card c;
		c = game.cards[start+i];
		game.cards[start+i] = game.cards[start+tab[i]];
		game.cards[start+tab[i]] = c;
	    }
    }
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i)
	draw_pileupdate(i, 0);
    return SHUFFLING;
}

#ifdef DEBUG
static int type[200];

static void count(void) {
    int i;
    memset(type, 0, sizeof(type));
    for (i = 0; i < rules.numcards; ++i)
	++type[game.cards[i]];
    for (i = 0; i < 52; ++i)
	if (type[i] != rules.numdecks)
	    fprintf(stderr, "Card %d: Number = %d\n", i, type[i]);
}
#endif

void memo_alloc(int num) {
    game.numalloc = ((num - 1) | 0xf) + 1;	/* multiple of 16 */
    if (!(game.move = malloc(game.numalloc * sizeof(Move)))) {
	fprintf(stderr, "out of memory\n");
	exit(EXIT_FAILURE);
    }
}


static void makespace(int n) {
    if (game.move_ptr + n > game.numalloc) {
	Move *tmpnew;
	if (!(tmpnew = realloc(game.move, (game.numalloc+256) * sizeof(Move)))) {
	    fprintf(stderr, "out of memory. saving game in xpat.tmp\n");
	    save_game("xpat.tmp");
	    exit(EXIT_FAILURE);
	}
	game.numalloc += 256;
	game.move = tmpnew;
    }
}

/* A new move is done. This resets the limit of redo to the current point. */
/* Also, the Bookmark is possibly taken back */
/* if the AUTOFILL_TMPS feature is set, empty tmps are filled from the deck */
void store_move(Move m) {
    makespace(1);
    if (game.bookmark > game.move_ptr)	/* reset Bookmark position */
	game.bookmark = game.move_ptr;
    if ((rules.variant & AUTOFILL_TMPS) && (m & SPECIAL_MOVE) != SPECIAL_MOVE) {
	Pileindex srcpile = DSTPILE(m >> 16);
	if (game.piletype[srcpile] == Tmp && (!EMPTY(IDECK) || !EMPTY(VDECK))) {
	    /* this is the case we have to deal with */
	    makespace(3);
	    game.move[game.move_ptr++] = COMPOUND_BEGIN;
	    game.move[game.move_ptr++] = m;
	    game.move[game.move_ptr++] = !EMPTY(VDECK) ?
		do_move(INDEX_OF_LAST_CARD(VDECK), srcpile) :
		do_move(INDEX_OF_LAST_CARD(IDECK), srcpile);
	    m = COMPOUND_END;
	}
    }
    game.move[game.move_ptr] = m;
    game.stored_moves = ++game.move_ptr;
    ++game.n_moves;

    /* now that the work is done, check for finished game */
    check_win_game();
}

static void undo_give_cards(int num) {
    /* num = number of cards */
    /* printf("undo give %d cards\n", num); */
    if (rules.undeal_cards)
	(*rules.undeal_cards)(num);
    else if (rules.variant & KLONDIKE_DEAL)
	Klondike_undeal_cards(num);
    else {
	int i;
	graphics_pile_control(Disable, IDECK);
	for (i = FIRST_SLOT + num; i != FIRST_SLOT; --i)
	    /* slot i-1 back to deck */
	    do_move(game.ind[i]-1, IDECK);
	graphics_pile_control(EnableAndRedraw, IDECK);
    }
}


int undo_move(void) {
    Move m;
    int retval = 1;	/* no cheat */

    if (!game.move_ptr) {
	/* possibly undo restart game? */
	if (game.stored_moves) {
	    jumpto_movenr(game.stored_moves);	/* must replay the game */
	    return 1;
	}
	return 0;
    }
    m = game.move[--game.move_ptr];
    --game.n_moves;
    if ((m & SPECIAL_MOVE) == SPECIAL_MOVE) {
	switch (m & SPECIAL_MASK) {
	case COMPOUND_END:
	    {
		int remember_count, remgraphic;
		if ((remgraphic = game.graphic))
		    graphics_control(Disable);
		/* undo multiple moves */
		remember_count = game.n_moves;
		while (game.move[game.move_ptr-1] != COMPOUND_BEGIN) {
		    if (undo_move() == 2)
			retval = 2;
		}
		--game.move_ptr;
		game.n_moves = remember_count;
		if (remgraphic)
		    graphics_control(EnableAndRedraw);
	    }
	    return retval;	
	case ADD_CHEAT:
	    game.cheat_count += (int)(m & SPECIAL_ARGS);
	    return 2;
	case NEW_CARDS_MOVE:
	    undo_give_cards(DSTPILE(m));
	    if (DSTPILE(m)) {
		game.cheat_count += DSTPILE(m);
		return 2;
	    }
	    return 1;
	case ROTATE_UP:
	    /* change counter */
	    {   int c;
		for (c = 0; c < 4; ++c)
		    if (rules.paramstring[c] == TXTI_ROTATE)
			game.counter[c] -= 2;
	    }
	    RotateDown(SRCIND(m));
	    return 1;
	case ROTATE_DOWN:
	    /* change counter */
	    {   int c;
		for (c = 0; c < 4; ++c)
		    if (rules.paramstring[c] == TXTI_ROTATE)
			game.counter[c] -= 2;
	    }
	    RotateUp(SRCIND(m));
	    return 1;
	case SHUFFLING:
	    SlotShuffle(0);
	    return 1;
	default:
	    /* printf("Move in error is %08lx\n", m); */
	    assert(0);
	}
	return 0;
    }
    /* standard move follows */
    if (m & MOVE_TURNED) {
	m >>= 16;	/* upper part = undo information */
	++game.cheat_count;
	game.visible[game.ind[DSTPILE(m)+1]-1] = 0;	/* invisible again! */
	retval = 2;
    } else
        m >>= 16;	/* upper part = undo information */

    (void)do_move(SRCIND(m), DSTPILE(m));
    if (retval == 2)
	draw_pileupdate(DSTPILE(m), 0);			/* hide card again! */
    return retval;
}

int redo_move(void) {
    Move m;
    int retval = 1;
    if (game.move_ptr == game.stored_moves)
	return 0;	/* no redo possible */
    m = game.move[game.move_ptr++];
    ++game.n_moves;
    if ((m & SPECIAL_MOVE) == SPECIAL_MOVE) {
	switch (m & SPECIAL_MASK) {
	case COMPOUND_BEGIN:
	    {
		int remember_count, remgraphic;
		if ((remgraphic = game.graphic))
		    graphics_control(Disable);
		/* redo multiple moves */
		remember_count = game.n_moves;
		while (game.move[game.move_ptr] != COMPOUND_END) {
		    if (redo_move() == 2)
			retval = 2;
		}
		++game.move_ptr;
		game.n_moves = remember_count;
		if (remgraphic)
		    graphics_control(EnableAndRedraw);
	    }
	    return retval;
	case ADD_CHEAT:
	    game.cheat_count -= (int)(m & SPECIAL_ARGS);
	    return 2;
	case ROTATE_UP:
	    RotateUp(SRCIND(m));
	    return 1;
	case ROTATE_DOWN:
	    RotateDown(SRCIND(m));
	    return 1;
	case NEW_CARDS_MOVE:
	    retval = DSTPILE(give_new_cards());
	    if (retval) {
		game.cheat_count -= retval;
		return 2;
	    }
	    return 1;
	case SHUFFLING:
	    SlotShuffle(1);
	    return 1;
	default:
	    assert(0);
	}
    }
    if (m & MOVE_TURNED) {
	--game.cheat_count;
	retval = 2;
    }
    (void)do_move(SRCIND(m), DSTPILE(m));
    return retval;
}


static void shuffle(void) {
    int i, d, v, c;
    int tmp[MAXCARDS];

    if (game.graphic)
	show_message(" ");
    i = 0;
    for (d = 0; d < rules.numdecks; ++d)
	for (v = 0; v < rules.cards_per_color; ++v)
	    for (c = 0; c < 4; ++c)
		tmp[i++] = c + (v << 2);
    assert(i <= rules.numcards);
    c = 0;
    while (i < rules.numcards)
	tmp[i++] = JOKER + c++;
    while (c < rules.numjokers)
	tmp[myrand(rules.numcards)] = JOKER + c++;	/* subst old cards */

    for (i = rules.numcards; i; --i) {
	v = myrand(i);
	/* printf("rand() = %d\n", v); */
	/* look for the vth non-empty card in tmp */
	c = -1;
	do {
	    while (tmp[++c] == -1)
		;			/* skip card */
	} while (v--);
	game.cards[i-1] = tmp[c];
	/* printf("c = %d, card = %d\n", c, game.cards[i-1]); */
	tmp[c] = -1;
    }
}

Pileindex getpile(Cardindex ind) {
    int i;
    assert(ind < rules.numcards);
    i = 0;
    while (ind >= game.ind[i+1])
	++i;
    return i;
}

/* These are the central card moving routines, i.e. the only functions which */
/* change the contents of the game.cards array within a game.		     */
/* Every time anything is changed, one of these functions is called.  	     */
/* We can reset hint tables at this point. 				     */

Move RotateUp(Cardindex src) {
    Pileindex p = getpile(src);
    int c;
    Cardindex i;

    cmd_CancelSelection();
    i = INDEX_OF_LAST_CARD(p);
    c = game.cards[i];
    while (--i >= src)
	game.cards[i+1] = game.cards[i];
    game.cards[i+1] = c;
    draw_pileupdate(p, 0);
    /* change counter */
    for (c = 0; c < 4; ++c)
	if (rules.paramstring[c] == TXTI_ROTATE)
	    ++game.counter[c];
    return ROTATE_UP | MOVE(src, 0);
}
Move RotateDown(Cardindex src) {
    Pileindex p = getpile(src);
    int c;
    Cardindex i;

    cmd_CancelSelection();
    c = game.cards[src];
    for (i = src; i < INDEX_OF_LAST_CARD(p); ++i)
	game.cards[i] = game.cards[i+1];
    game.cards[i] = c;
    draw_pileupdate(p, 0);
    /* change counter */
    for (c = 0; c < 4; ++c)
	if (rules.paramstring[c] == TXTI_ROTATE)
	    ++game.counter[c];
    return ROTATE_DOWN | MOVE(src, 0);
}
Move do_move(Cardindex srcindex, Pileindex dstpile) {
    int srcpile, numcards;
    int tmp[MAXCARDS];
    int vis;
    Move m;

    cmd_CancelSelection();	/* calls cmd_ResetHints() */
    if (srcindex == -1)
	return NO_MOVE;

    srcpile = getpile(srcindex);
    numcards = game.ind[srcpile+1] - srcindex;
    m = MOVE(srcindex, dstpile);
    if (srcindex > game.ind[srcpile] && !game.visible[srcindex-1] &&
	(srcpile != IDECK || rules.variant & DECK_VISIBLE)) {
	game.visible[srcindex-1] = 1;
	m |= MOVE_TURNED;
    }

    memcpy(tmp, game.cards+srcindex, numcards * sizeof(int));
    /* moved cards are normally visible */
    vis = dstpile != IDECK || rules.variant & DECK_VISIBLE;
    /* printf("move %d to %d, %d cards\n", srcindex, dstpile, numcards); */
    assert(srcpile != dstpile);
    if (srcpile < dstpile) {
	/* ldir */
	int i;
	m |= MOVE(game.ind[dstpile+1]-numcards, srcpile) << 16;
	for (i = srcindex; i < game.ind[dstpile+1]-numcards; ++i)
	    move_card_data(i, i+numcards);
	memcpy(game.cards+i, tmp, numcards * sizeof(int));
	for (; i < game.ind[dstpile+1]; ++i)
	    game.visible[i] = vis;
	for (i = srcpile + 1; i <= dstpile; ++i)
	    game.ind[i] -= numcards;
    } else {
	/* lddr ; shift a block backwards; begin at the tail */
	int i;
	m |= MOVE(game.ind[dstpile+1], srcpile) << 16;
	for (i = srcindex - 1; i >= game.ind[dstpile+1]; --i)
	    move_card_data(i+numcards, i);
	memcpy(game.cards + game.ind[dstpile+1], tmp, numcards * sizeof(int));
	for (i = 0; i < numcards; ++i)
	    game.visible[game.ind[dstpile+1] + i] = vis;
	for (i = dstpile+1; i <= srcpile; ++i)
	    game.ind[i] += numcards;
    }
#ifdef DEBUG
    count();
#endif
    /* inform graphics interface of change */
    draw_pileupdate(srcpile, -numcards);
    draw_pileupdate(dstpile, numcards);
    /* printf("Move done is %08lx\n", m); */
    return m;
}


int all_to_stack(void) {
    int i, flag, anymove;

    anymove = 0;
    do {
	flag = 0;
	for (i = 0; i < game.numpiles; ++i) {
	    switch (game.piletype[i]) {
	    case Slot:
	    case Tmp:
	    case FaceupDeck:
		if (move_to_stack(i)) {
		    flag = 1;
		    anymove = 1;
		}
		break;
	    default:
		;
	    }
	}
    } while (flag);
    return anymove;
}

static void init_seed(long seed) {
    if (seed < 0) {
	game.seed = -1L;	/* to guarantee a mismatch later */
	seed = (long)time(NULL);
    }
    seed %= PRANDMAX;
    if (seed < 0L)
	seed += PRANDMAX;	/* I think this shouldn't happen */
    if (seed == game.seed) {	/* restart game */
	/* stored_moves stays valid */
	game.cheat_count += 1000;
    } else {
	game.seed = seed;
	game.cheat_count = game.stored_moves = 0;
	game.finished = False;	/* this is really a new game */
    }
    game.n_moves = 0;
    game.move_ptr = 0;
    game.counter[0] = 0;
    game.counter[1] = 0;
    game.counter[2] = 0;
    game.counter[3] = 0;
    sprand(seed);
}

static void distribute(char *xx, int rest, int piles)
{   int i, k, invert = 0;

    if (!piles)
	return;
    if (2 * rest > piles+1) {
	rest = piles - rest;
	invert = 1;
    }
    memset(xx, invert, piles);
    if (rest) {
	k = (piles-1) / rest + 1;	/* ceil(slots/rest) */
	for (i = 0; i < piles; i += k) {
	    xx[i] = 1-invert;
	    if (!--rest)
		break;
	}
	/* the rest is distributed quite ugly */
	for (i = 0; rest; ++i)
	    if (xx[i] == invert) {
		xx[i] = 1-invert;
		--rest;
	    }
    }
}

static void gen_newgame(void)
{   int i, rest;
    char xx[MAXPILES];

    memset(xx, 0, MAXPILES);
    if (rules.facedown && !rules.faceup) {
	fprintf(stderr, "newgame(): topmost card must be face-up, correcting it\n");
	--rules.facedown;
	++rules.faceup;
    }
    if (rules.newgame_bits & SLOTS_SAME)
	rest = 0;
    else
	rest = rules.numcards % rules.numslots;	/* cards that are too much */

    if (rules.numcards < rest + rules.numslots * (rules.faceup + rules.facedown)) {
	fprintf(stderr, "newgame(): too many cards specified, resetting to min values\n");
	rules.faceup = 1;
	rules.facedown = 0;
    }

    /* generate nice distribution of rest cards */
    if (rest > rules.numtmps || (rules.newgame_bits & FORCE_SLOTS))
 	/* distribute on slots */
	distribute(xx+FIRST_SLOT, rest, rules.numslots);
    else
	/* distribute on tmps */
	distribute(xx+LAST_SLOT+1, rest, rules.numtmps);

    for (i = 0; i <= FIRST_SLOT; ++i)
	game.ind[i] = 0;
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	int j;
	game.ind[i+1] = game.ind[i] + rules.facedown + rules.faceup + xx[i];
	for (j = game.ind[i+1] - rules.faceup; j < game.ind[i+1]; ++j)
	    game.visible[j] = 1;		/* card is turned */
    }
    while (++i < game.numpiles)
	game.ind[i] = game.ind[i-1] + xx[i-1];
    /* all cards on the tmps are faceup */
    for (i = game.ind[LAST_SLOT+1]; i < game.ind[IDECK]; ++i)
	game.visible[i] = 1;
    if (rules.variant & DECK_VISIBLE)
	game.visible[rules.numcards-1] = 1;	/* topmost card on deck */
    if (!rules.facedown)
	for (i = game.ind[FIRST_SLOT]; i < game.ind[LAST_SLOT+1]; ++i)
	     game.visible[i] = 1;	/* all cards on the slots visible */
}


static void MO_newgame(void) {
    int i, remcards = rules.numcards;
    /* specific part: */
    for (i = 0; i < rules.numslots; ++i) {
	int here;
	here = min(remcards, rules.faceup + rules.facedown);
	remcards -= here;
	game.ind[rules.numstacks+i+1] = game.ind[rules.numstacks+i] + here;
	if (here) {
	    Cardindex j;
	    j = game.ind[rules.numstacks+i+1] - 1;
	    do
		game.visible[j--] = 1;
	    while (j >= game.ind[rules.numstacks+i] + rules.facedown);
	}
    }
}

void newgame(long seed) {
    int i;

    game.randomgame = (seed == -1L);
    game.savecount = 0;
    game.bookmark = 0;
    init_seed(seed);
    shuffle();	/* initialize game.cards */

    for (i = 0; i < rules.numcards; ++i)
	game.visible[i] = 0;
    for (i = 0; i < game.numpiles; ++i)
	game.ind[i] = 0;	/* no cards on the piles */
    game.ind[i] = rules.numcards;	/* rest on the deck */

    if (rules.new_game)
	(*rules.new_game)();
    else if (rules.newgame_bits & SEQUENTIAL)
	MO_newgame();
    else
	gen_newgame();

    if (game.graphic)
	for (i = 0; i < game.numpiles; ++i)
	    pile_resize(i);
    /* (void)do_move(-1, -1); */		/* reset game.*/
    cmd_ResetHints();
    game.srcind = UNSELECTED;
}

/* generic give-cards routine */

Move give_new_cards(void) {
    if (rules.deal_cards)
	return (*rules.deal_cards)();
    else if (rules.variant & KLONDIKE_DEAL)
	return Klondike_deal_cards();
    else {
	int i, rem;
	
	graphics_pile_control(Disable, IDECK);
	rem = CARDS_ON_PILE(IDECK);
	if (rem > rules.numslots)
	    rem = rules.numslots;
	for (i = 0; i < rem; ++i)
	    do_move(INDEX_OF_LAST_CARD(IDECK), rules.numstacks + i);
	graphics_pile_control(EnableAndRedraw, IDECK);
	return NEW_CARDS_MOVE | MOVE(0, rem);
    }
}





extern struct rules Spider_rules, Gypsy_rules, Klondike_rules, modSpider_rules;
extern struct rules Seahaven_rules, FreeCell_rules, IdiotsDelight_rules,
    MonteCarlo_rules, MidnightOil_rules, Calculation_rules, Michaels_rules,
    Canfield_rules, modCanfield_rules, Royal_rules, Bakers_rules;

struct game game;
struct rules rules;

struct rules *rulepool[] = {
    &Spider_rules,
    &Gypsy_rules,
    &Klondike_rules,
    &Seahaven_rules,
    &FreeCell_rules,
    &IdiotsDelight_rules,
    &MonteCarlo_rules,
    &MidnightOil_rules,
    &Calculation_rules,
    &modCanfield_rules,
    &Michaels_rules,
    &Canfield_rules,
    &Royal_rules,
    &Bakers_rules,
    NULL
};

struct rules *getrules(const char *ruleset) {
    int i;
    for (i = 0; rulepool[i]; ++i)
	if (!strcmp(ruleset, rulepool[i]->shortname) ||
	    (rulepool[i]->abbrev && !strcmp(ruleset, rulepool[i]->abbrev))) {
	    return rulepool[i];
	}
    return NULL;
}

void new_rules(const char *ruleset, int decks, int slots, int faceup, int facedown,
    int jokers, int tmps, int param0, int param1, int param2, int param3) {
    int i;
    struct rules *rp;
    if (!game.numalloc)
	memo_alloc(512);
    game.seed = -1L;		/* no replay is valid */
				/* that will set finished to False */

    if (!(rp = getrules(ruleset))) {
	fprintf(stderr, "Unknown rule set. Known rules are:\n");
	for (i = 0; rulepool[i]; ++i) {
	    char buff[20];
	    if (rulepool[i]->abbrev)
		sprintf(buff, "(Abbrev %-2s)", rulepool[i]->abbrev);
	    else
		strcpy(buff, "           ");
	    fprintf(stderr, "%-15s %s -- %s\n", rulepool[i]->shortname, buff,
		    rulepool[i]->longname ? rulepool[i]->longname : "");
	}
	exit(EXIT_FAILURE);
    }
    rules = *rp;	/* copy std game.*/

    /* rule customization: */
    if (!(rules.customizable & CUSTOM_DECKS )) decks = -1;
    if (!(rules.customizable & CUSTOM_SLOTS )) slots = -1;
    if (!(rules.customizable & CUSTOM_JOKERS)) jokers = -1;
    if (!(rules.customizable & CUSTOM_TMPS  )) tmps = -1;
    if (!(rules.customizable & CUSTOM_FACEUP)) faceup = -1;
    if (!(rules.customizable & CUSTOM_FACEDOWN)) facedown = -1;
    if (!(rules.customizable & CUSTOM_PARAM0)) param0 = -1;
    if (!(rules.customizable & CUSTOM_PARAM1)) param1 = -1;
    if (!(rules.customizable & CUSTOM_PARAM2)) param2 = -1;
    if (!(rules.customizable & CUSTOM_PARAM3)) param3 = -1;

    if (jokers >= 0 && jokers != rules.numjokers) {
	rules.numjokers = jokers;
	rules.customized |= CUSTOM_JOKERS;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (param0 >= 0 && param0 != rules.param[0]) {
	rules.param[0] = param0;
	rules.customized |= CUSTOM_PARAM0;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (param1 >= 0 && param1 != rules.param[1]) {
	rules.param[1] = param1;
	rules.customized |= CUSTOM_PARAM1;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (param2 > 0 && param2 != rules.param[2]) {
	rules.param[2] = param2;
	rules.customized |= CUSTOM_PARAM2;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (param3 >= 0 && param3 != rules.param[3]) {
	rules.param[3] = param3;
	rules.customized |= CUSTOM_PARAM3;
    }
    if (faceup >= 0 && faceup != rules.faceup) {
	rules.faceup = faceup;
	rules.customized |= CUSTOM_FACEUP;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (facedown >= 0 && facedown != rules.facedown) {
	rules.facedown = facedown;
	rules.customized |= CUSTOM_FACEDOWN;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (slots > 0 && slots != rules.numslots) {
	rules.numslots = slots;
	rules.customized |= CUSTOM_SLOTS;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (decks > 0 && decks != rules.numdecks) {
	rules.numdecks = decks;
	rules.customized |= CUSTOM_DECKS;
	rules.maxscore = 0;	/* custom: maxscore unknown */
    }
    if (tmps >= 0 && tmps != rules.numtmps) {
	rules.numtmps = tmps;
	rules.customized |= CUSTOM_TMPS;
    }

    if (rules.numstacks >= 4)
	rules.numstacks = rules.numdecks * 4;
    rules.numcards = rules.cards_per_color * 4 * rules.numdecks + rules.numjokers;
    if (rules.numcards > MAXCARDS) {
	fprintf(stderr, "new_rules(): parameters give too many cards\n");
	exit(EXIT_FAILURE);
    }
    if (rules.numslots + rules.numstacks + 1 > MAXPILES) {
	fprintf(stderr, "new_rules(): parameters give too many slots\n");
	exit(EXIT_FAILURE);
    }
    for (i = 0; i < rules.numstacks; ++i)
	game.piletype[i] = Stack;
    for (; i < rules.numstacks + rules.numslots; ++i)
	game.piletype[i] = Slot;
    for (; i < rules.numstacks + rules.numslots + rules.numtmps; ++i)
	game.piletype[i] = Tmp;
    if (rules.variant & DECK_SOURCE)
	game.piletype[i++] = FaceupDeck;
    game.piletype[i++] = FacedownDeck;
    game.numpiles = i;
    for (i = 0; i <= game.numpiles; ++i)/* this fixes coredump-bug */
	game.ind[i] = 0;		/* no card currently there */

    if (rules.initfunc)    		/* initrules-hook */
	(*rules.initfunc)();
    rp->inited = 1;
}
