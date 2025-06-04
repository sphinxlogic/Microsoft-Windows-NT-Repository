/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module rules.c				     */
/*									     */
/*	General utility functions for all rule sets			     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"
#include <time.h>

Cardindex complete_suit(Pileindex pile, Cardindex i) {
    Rank r;
    Suit s;
    int c;
    if (IS_JOKER(game.cards[i]))
	return 0;
    r = RANK(game.cards[i]);
    s = SUIT(game.cards[i]);
    if (r != Ace)
	return 0;
    for (c = rules.cards_per_color-1; c > 0; --c) {
	if (--i < game.ind[pile] || !game.visible[i])
	    return -1;
	if (RANK(game.cards[i]) != ++r || SUIT(game.cards[i]) != s)
	    return -1;
    }
    return i;
}

/* Move cards to stack. */

int stackable(Pileindex pile) {
    if (rules.stackable) {
	return (*rules.stackable)(pile);
    } else {
	Cardindex srcindex;
	srcindex = INDEX_OF_LAST_CARD(pile);
	switch (rules.move_bits & ST_MASK) {
	case ST_NONE:
	    return -1;	/* user may not may cards to stack */
	case ST_ONE:
	    if (IS_JOKER(game.cards[srcindex]))     /* jokers not movable to stack */
		return -1;
	    break;
	case ST_13:
	    if ((srcindex = complete_suit(pile, srcindex)) == -1)
		return -1;
	    break;
	}
	return srcindex;
    }
}

int move_to_stack(Pileindex pile) {	/* pile is slot or tmp */
    Cardindex srcindex;
    Pileindex i;

    if (EMPTY(pile))
	return 0;

    if ((srcindex = stackable(pile)) == -1)
	return 0;
    for (i = 0; i < rules.numstacks; ++i) {
	if (SUIT(i) != SUIT(game.cards[srcindex]))
	    continue;
	if (move_valid(srcindex, i)) {
	    store_move(do_move(srcindex, i));
	    return 1;
	}
    }
    return 0;
}


void check_win_game(void) {
    if (game.finished)
	return;		/* don't win twice */
    if (rules.game_won) {
	if (!(*rules.game_won)())
	    return;
    } else {
	if (game.ind[FIRST_SLOT] != rules.cards_per_color * rules.numstacks)
	    return;
    }
    /* actually, win the game! */

    /* all cards on the stacks and not yet notified */
    game.finished = True;
    write_log_file();
    show_message(TXT_YOU_WIN);	/* overwrites OK */
    win_game();		/* hook for additional doodads */
    play_sound("success");
}



int in_relaxed_sequence(int srccard, int dstcard) {
    if (rules.relaxed_valid)
	return (*rules.relaxed_valid)(srccard, dstcard);
    switch (rules.move_bits & US_MASK) {
    case US_RS:
	if (IS_JOKER(dstcard) || IS_JOKER(srccard))
	    return 1;
	return RANK(dstcard) == 1 + RANK(srccard) && SUIT(dstcard) == SUIT(srccard);
    case US_R:
	if (IS_JOKER(dstcard) || IS_JOKER(srccard))
	    return 1;
	return RANK(dstcard) == 1 + RANK(srccard);
    case US_RA:
	if (IS_JOKER(dstcard) || IS_JOKER(srccard))
	    return 1;
	return RANK(dstcard) == 1 + RANK(srccard) && DIFFERENT_COLOR(dstcard, srccard);
    }
    return 0;
}
int in_strong_sequence(int srccard, int dstcard) {
    if (rules.valid)
	return (*rules.valid)(srccard, dstcard);
    switch (rules.move_bits & MG_MASK) {
    case MG_RS:
	if (IS_JOKER(dstcard) || IS_JOKER(srccard))
	    return 1;
	return RANK(dstcard) == 1 + RANK(srccard) && SUIT(dstcard) == SUIT(srccard);
    case MG_R:
	if (IS_JOKER(dstcard) || IS_JOKER(srccard))
	    return 1;
	return RANK(dstcard) == 1 + RANK(srccard);
    case MG_RA:
	if (IS_JOKER(dstcard) || IS_JOKER(srccard))
	    return 1;
	return RANK(dstcard) == 1 + RANK(srccard) && DIFFERENT_COLOR(dstcard, srccard);
    }
    return 0;
}

/* find maximum sequence in a nonempty slot for moving */

Cardindex slotsequence(Pileindex ind, Cardindex start, Cardindex stop) {
    while (start > game.ind[ind] && game.visible[start-1] && start != stop &&
	   in_strong_sequence(game.cards[start], game.cards[start-1]))
	--start;
    return start;
}

/* find maximum sequence in a nonempty slot for moving */

Cardindex relaxedslotsequence(Pileindex ind, Cardindex start, Cardindex stop) {
    while (start > game.ind[ind] && game.visible[start-1] && start != stop &&
	   in_relaxed_sequence(game.cards[start], game.cards[start-1]))
	--start;
    return start;
}

int default_movevalid(Cardindex src, Pileindex dstpile) {
    int srcpile, dstcard, srccard;
    srcpile = getpile(src);
    if (srcpile == dstpile)
	return 0;
    switch (game.piletype[srcpile]) {	/* filter out some invalid sources */
    case FacedownDeck:
	return 0;
    case Stack:
	if (!(rules.move_bits & STACK_SOURCE))
	    return 0;
	break;
    case FaceupDeck:			/* always valid sources */
    case Tmp:
    case Slot:
	break;
    }
    dstcard = EMPTY(dstpile) ? -1 : game.cards[game.ind[dstpile+1]-1];
    srccard = game.cards[src];
    switch (game.piletype[dstpile]) {
    case Stack:
	if (SUIT(dstpile) != SUIT(game.cards[src]))
	    return 0;
	if (rules.stackable) {
	    if (src != (*rules.stackable)(srcpile))
		return 0;		/* not possible to move to stack */
	} else {
	    switch (rules.move_bits & ST_MASK) {
	    case ST_NONE:
		return 0;	/* user may not may cards to stack */
	    case ST_ONE:
		if (src != INDEX_OF_LAST_CARD(srcpile) || IS_JOKER(game.cards[src]))
		    return 0;
		break;
	    case ST_13:
		if (src != complete_suit(srcpile, INDEX_OF_LAST_CARD(srcpile)))
		    return 0;
		break;
	    }
	}
	/* src is a block of cards without jokers */
	if (RANK(game.cards[INDEX_OF_LAST_CARD(srcpile)]) != (
	    dstcard == -1 ? Ace : (1 + RANK(dstcard))	     ))
	    return 0;
	/* top cards match */
	return 1;
    case Slot:
	if (dstcard == -1) {
	    switch (rules.move_bits & ES_MASK) {
		case ES_KINGS:  /* Klondike: only Kings are */
		if (IS_JOKER(game.cards[src]) || RANK(game.cards[src]) == King)
		    return 1;
		break;
	    case ES_ALL:
		return 1;
	    }
	} else {	/* move to non-empty slot */
	    return in_relaxed_sequence(srccard, dstcard);
	}
	return 0;	/* fall through: move not allowed */
    case Tmp:
	if (game.ind[srcpile+1] - src != 1)
	    return 0;		/* only one card can be moved */
	return dstcard == -1;
    default:
	return 0;
    }
}

int debugging = 0;
int move_valid(Cardindex src, Pileindex dstpile) {

    if (debugging)
	return 1;
    if (rules.movevalid)
	return (*rules.movevalid)(src, dstpile);
    else
	return default_movevalid(src, dstpile);
}


Cardindex maxsequence(Pileindex ind, Cardindex cardi) {
    if (!EMPTY(ind)) {	/* the pile may not be empty for this operation */
	switch (game.piletype[ind]) {
	case Stack:
	    if (!(rules.move_bits & STACK_SOURCE))
		break;		/* not allowed => break */
	    /* fall through: */
	case Tmp:
	    return INDEX_OF_LAST_CARD(ind);
	case Slot:
	    return slotsequence(ind, INDEX_OF_LAST_CARD(ind), cardi);
	case FaceupDeck:
	    if (rules.variant & DECK_SOURCE)
		return INDEX_OF_LAST_CARD(ind);
	    break;	/* not allowed => fall through */
	case FacedownDeck:
	    if ((rules.variant & DECK_VISIBLE) && !EMPTY(ind))
		return INDEX_OF_LAST_CARD(ind);
	    break;
	}
    }
    return -1;		/* this indicates that selection is not possible */
}




/* check, if new cards may be dealt */

int check_new_cards(void) {
    Pileindex i;
    Cardindex (*testfunc)(Pileindex, Cardindex, Cardindex) = NULL;

    if (rules.variant&NODEAL)
	return 0;
    if (rules.new_cards)
	return (*rules.new_cards)();	/* custom function */

    if (!CARDS_ON_PILE(IDECK))
	return 0;	/* no more cards */

    switch (rules.move_bits & DC_MASK) {
    case DC_ALWAYS:
	return 1;
    /* case DC_NOEMPTY: falls thru */
    case DC_STRONGOK:
	testfunc = slotsequence;
	break;
    case DC_RELAXEDOK:
	testfunc = relaxedslotsequence;
	break;
    }
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i)
	if (EMPTY(i))
	    goto require_check;
    return 1;	/* no empty slot */
require_check:
    if (!testfunc)
	return 0;
    /* may give cards if all nonempty slots are in some sequence */
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i)
	if (!EMPTY(i))
	    if ((*testfunc)(i, INDEX_OF_LAST_CARD(i), INDEX_OF_FIRST_CARD(i))
		!= INDEX_OF_FIRST_CARD(i))
		return 0;	/* test failed, pile unsorted */
    return 1;	/* OK, all nonempty pile are in order */
}

int std_good_hint(Cardindex srcindex, Pileindex dstpile) {
    Pileindex srcpile = getpile(srcindex);
    int value = 0;

    if (IS_STACK(srcpile))  	/* don't move from stack if not explicitly requested */
        value = 1;
    else if (IS_STACK(dstpile))	{ /* likes to move cards to stack */
	value = (rules.variant & ST_MASK) == ST_13 ? 10000 : 30;
    } else
	value = 15;		/* value of standard move */

    /* modifications */
    if (IS_SLOT(dstpile)) {
	switch (rules.move_bits & MG_MASK) {
	case MG_RS:
	    if (SUIT(game.cards[srcindex]) == SUIT(game.cards[INDEX_OF_LAST_CARD(dstpile)]))
		value += 15;
	    else if (RANK(game.cards[srcindex]) == RANK(game.cards[srcindex-1]) - 1)
		value -= 15;
	    else
		value += RANK(game.cards[srcindex]); /* higher ones first */
	    break;
	case MG_NONE:
	    if (RANK(game.cards[srcindex]) == RANK(game.cards[srcindex-1]) - 1)
		value -= 15;	/* doesn't make anything better */
	    else
		value -= 10;
	    break;
	}
    }

    if (IS_SLOT(srcpile)) {
	if (game.ind[srcpile] == srcindex) {
	    /* bonus for creating empty space */
	    switch (rules.move_bits & ES_MASK) {
	    case ES_NONE:
		value -= 10;	/* no bonus */
		break;
	    case ES_KINGS:
		value += 10;
		break;
	    case ES_ALL:
		value += 100;
		break;
	    }
	} else if (!game.visible[srcindex-1]) {
	    /* turn a card */
	    value += 10;
	}
    }
    if (value < 1)
	value = 1;
    return value;
}
