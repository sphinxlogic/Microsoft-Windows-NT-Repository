/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module commands.c			     */
/*									     */
/*	Most of the entries for commands assignable to keys		     */
/*	written by Michael Bischoff					     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"
#include "version.h"

void change_rules(const char *new_rules_name) {
    cmd_CancelSelection();
    new_rules(new_rules_name, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);	/* std rules */
    new_rules_notify();
    newgame(-1L);			/* new game with random seed */
}

void rq_LeavePat(void) {
    if (game.finished || !game.n_moves)
	cmd_LeavePat();
    else
	request_confirm(cmd_LeavePat, TXT_QUIT_CONFIRM);
}

void rq_AnotherGame(void) {
    if (game.finished || !game.n_moves)
	cmd_AnotherGame();
    else
	request_confirm(cmd_AnotherGame, TXT_NEW_CONFIRM);
}

/* unused, since this can be undone */
void rq_RestartGame(void) {
    request_confirm(cmd_RestartGame, TXT_RESTART_CONFIRM);
}

void cmd_AnotherGame(void) {
    if (!game.finished) {
	play_sound("giveup");
    }
    newgame(-1L);
    refresh_screen();
}

void graphics_control(GraphicsControl cmd) {
    Pileindex i;
    switch (cmd) {
    case Disable:
	game.graphic = False;
	for (i = 0; i < game.numpiles; ++i)
	    game.pile_changed[i] = PILE_UNCHANGED;
	break;
    case Enable:
	game.graphic = True;
	break;
    case EnableAndRedraw:
	game.graphic = True;
	for (i = 0; i < game.numpiles; ++i)
	    if (game.pile_changed[i] != PILE_UNCHANGED) {
		/* printf("pile_changed(%d) = %d\n", i, game.pile_changed[i]); */
		draw_pileupdate(i, game.pile_changed[i]);
	    }
	break;
    }
}
void graphics_pile_control(GraphicsControl cmd, Pileindex pile) {
    switch (cmd) {
    case Disable:
	game.disable[pile] = True;
	break;
    case Enable:
	game.disable[pile] = False;
	break;
    case EnableAndRedraw:
	game.disable[pile] = False;
	draw_pileupdate(pile, 0);
	break;
    }
}

void cmd_ReplayGame(void) {
    int i, oldcheatcount = game.cheat_count;
    int movenum = game.n_moves;
    cmd_RestartGame();	/* with graphics! */
    /* explicitly paint all the piles! */
    for (i = 0; i < game.numpiles; ++i)
	draw_pileupdate(i, 0);
    while (game.n_moves < movenum) {
	/* XSync(dpy, 0); */
	redo_move();
    }
    game.cheat_count = oldcheatcount;
    /* problem: we should ignore the following expose events! */
}

void cmd_DropBookmark(void) {
    game.bookmark = game.move_ptr;	/* easy, isn't it? */
    show_message(TXT_BOOKMARK_SET);
}

void jumpto_movenr(int move_ptr) {
    int remgraphic = game.graphic;
    if (move_ptr == game.move_ptr)
	return;
    assert(move_ptr <= game.stored_moves);
    if (remgraphic)		/* graphic was on */
	graphics_control(Disable);
    if (move_ptr > game.move_ptr)	/* move forward */
	while (move_ptr > game.move_ptr)
	    redo_move();
    else
	while (game.move_ptr > move_ptr)
	    undo_move();
    if (remgraphic)
	graphics_control(EnableAndRedraw);
}

void cmd_RestartGame(void) {
    jumpto_movenr(0);
}

void cmd_GotoBookmark(void) {
    jumpto_movenr(game.bookmark);
}

void cmd_SaveGame(void) {
    save_game(NULL);
}

void cmd_ShowVersion(void) {
    show_message("%s %s", TXT_VERSION, VERSION);
}

static void infosub(char *buff, int n, int txtindex) {
    switch (n) {
    case 0:
	strcpy(buff, xpat_messages[txtindex+0]);
	break;
    case 1:
	strcpy(buff, xpat_messages[txtindex+1]);
	break;
    default:
	sprintf(buff, xpat_messages[txtindex+2], n);
	break;
    }
}

void cmd_ShowScore(void) {
    char buf[256];
    sprintf(buf, TXT_INFO1, game.seed, game.cheat_count);
    strcat(buf, " ");
    infosub(buf+strlen(buf), game.n_moves, TXT_MOVEBLOCK);
    strcat(buf, " ");
    if (rules.score) {
	infosub(buf+strlen(buf), (*rules.score)(), TXT_SCOREBLOCK);
	sprintf(buf+strlen(buf), " %d.", rules.maxscore);
    } else
	sprintf(buf+strlen(buf), TXT_NOSCORE);
    show_message(buf);
}

void cmd_Info(void) {
    char buf[256];
    int i;
    buf[0] = '\0';
    for (i = 0; i < 4; ++i) {
	if (rules.paramstring[i]) {
	    infosub(buf+strlen(buf), rules.param[i]-game.counter[i], rules.paramstring[i]+1);
	    strcat(buf, "  ");
	}
    }
    if (!(rules.variant & NODEAL) && CARDS_ON_DECK)
	infosub(buf+strlen(buf), CARDS_ON_DECK, TXTI_CARDS+1);
    show_message(buf);
}

void cmd_DealCards(void) {
    cmd_CancelSelection();
    if (rules.variant & NODEAL) {
	show_message(TXT_NODEAL);
	return;
    }
    if (EMPTY(IDECK) && (rules.variant & DECK_SOURCE)) {
	int i;
	for (i = 0; i < 4; ++i)
	    if (rules.paramstring[i] == TXTI_FLIP) {
		/* try a flip */
		if (game.counter[i] == rules.param[i])
		    show_message(TXT_NOFLIPLEFT);
		else {
		    store_move(give_new_cards());
		    show_message(TXT_FLIPPING);
		}
		return;
	    }
    }
    if (check_new_cards()) {
	show_message(TXT_NEWCARDS);
	store_move(give_new_cards());
    } else
	show_message(TXT_NONEWCARDS);
}

void cmd_AllToStack(void) {
    cmd_CancelSelection();
    cmd_ToStack();
}

void cmd_OneToStack(void) {
    if (game.srcind < 0) {
	game.srcind = UNSELECTED;
	show_message(TXT_NOSOURCEPILE);
    } else
	cmd_ToStack();
}

void cmd_ToStack(void) {
    if (game.srcind < 0) {	/* all to stack */
	game.srcind = UNSELECTED;
	if (!all_to_stack())
	    show_message(TXT_NONE_TO_STACK);
	else
	    show_message(TXT_MOVED_TO_STACK);
    } else {
	show_mark(False);
	if (!move_to_stack(getpile(game.srcind)))
	    show_message(TXT_MOVENOTPOSSIBLE);
	game.srcind = UNSELECTED;
    }
}

void cmd_UndoMove(void) {
    cmd_CancelSelection();
    switch (undo_move()) {
    case 0:	show_message(TXT_NOUNDO);
		break;
    case 1:	show_message(TXT_UNDO);
		break;
    case 2:	show_message(TXT_UNDOCHEAT);
		break;
    }
}

void cmd_RedoMove(void) {
    cmd_CancelSelection();
    switch (redo_move()) {
    case 0:	show_message(TXT_NOREDO);
		break;
    case 1:	show_message(TXT_REDO);
		break;
    case 2:	show_message(TXT_REDOUNCHEAT);
		break;
    }
}

static void hit_card(Pileindex pile, Cardindex card) {
    static int srcpile;		/* temp. variable to hold getpile(srcind) */
    
    /* printf("hit_card(%d,%d) called\n", pile, card); */

    cmd_ResetHints();
    if (game.srcind >= 0) {
	Cardindex h;
	h = game.srcind;
	game.srcind = -1;
	/* do move */
	show_mark(False);
	if (pile < 0) {
	    show_message("");
	    return;
	}
	if (pile != srcpile) {	/* else just unselect slot */
	    if (game.piletype[pile] == FacedownDeck && !(rules.variant & DECK_VISIBLE))
		cmd_DealCards();
	    else {
		if (move_valid(h, pile)) {
		    store_move(do_move(h, pile));
		    show_message("");
		} else
		    show_message(TXT_INVALIDMOVE);
	    }
	}
    } else {		/* no mark to clear, set srcpile and game.srcind */
	if (pile < 0) {
	    show_message("");
	    game.srcind = UNSELECTED;
	    return;
	}
	if (game.piletype[pile] == FacedownDeck && !(rules.variant & DECK_VISIBLE))
	    cmd_DealCards();
	else {
	    game.srcind = card;
	    srcpile = pile;
	    if (card == -1)
		show_message(TXT_BADSRC);
	    else {
		show_message(TXT_SRCSELECTED);
		show_mark(True);
	    }
	}
    }
}

void cmd_CancelSelection(void) {
    hit_card(-1, -1);
}
void cmd_RotateUp(void) {
    int i;
    hit_card(-1, -1);
    for (i = 0; i < 4; ++i)
	if (rules.paramstring[i] == TXTI_ROTATE) {
	    if (rules.param[i] > game.counter[i]) {
		game.srcind = ROTATE_UP_SEL;
		show_message(TXT_ROTUP);
		return;
	    } else {
		show_message(xpat_messages[TXTI_ROTATE+1]);
		return;
	    }
	}
    show_message(xpat_messages[TXTI_ROTATE]);
}
void cmd_RotateDown(void) {
    int i;
    hit_card(-1, -1);
    for (i = 0; i < 4; ++i)
	if (rules.paramstring[i] == TXTI_ROTATE) {
	    if (rules.param[i] > game.counter[i]) {
		game.srcind = ROTATE_DOWN_SEL;
		show_message(TXT_ROTDN);
		return;
	    } else {
		show_message(xpat_messages[TXTI_ROTATE+1]);
		return;
	    }
	}
    show_message(xpat_messages[TXTI_ROTATE]);
}

void button_pressed(Pileindex i, Cardindex card, int button) {
    /*printf("press %d %d %d called\n", i, card, button); */
    if (i == -1) {	/* no pile hit => cancel */
	cmd_CancelSelection();
	return;
    }
    if (game.srcind == ROTATE_UP_SEL || game.srcind == ROTATE_DOWN_SEL) {
	if (game.piletype[i] != Slot)
	    show_message(TXT_SLOTREQUIRED);
	else if (!game.visible[card])
	    show_message(TXT_CARDNOTVISIBLE);
	else
	    store_move(game.srcind == ROTATE_UP_SEL ? RotateUp(card) : RotateDown(card));
	game.srcind = UNSELECTED;
	return;
    }

    switch (button) {
    case 1:
	cmd_CancelSelection();	/* calls cmd_ResetHints(); */
	switch (game.piletype[i]) {
	case FacedownDeck:
	    if (!(rules.variant & DECK_VISIBLE)) {
		hit_card(i, 0);
		return;
	    }
	    /* else fall through */
	case FaceupDeck:
	case Slot:
	case Stack:
	case Tmp:
	    {   Cardindex ind;
		if ((ind = maxsequence(i, card)) < 0)
		    show_message(TXT_BADSRC);
		else if (rules.automove ? !(*rules.automove)(ind) : !generic_automove(ind))
		    show_message(TXT_NOMOVE);
	    }
	    break;	/* no action */
	}
	break;
    case 2:
	hit_card(i, maxsequence(i, card));
	break;
    }
}
