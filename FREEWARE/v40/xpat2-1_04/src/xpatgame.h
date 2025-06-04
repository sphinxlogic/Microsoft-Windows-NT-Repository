/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module xpatgame.h			     */
/*									     */
/*	Copyright (C) 1993, 1994 by Heiko Eissfeldt and Michael Bischoff     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*	This file is included by all interface-independent sources	     */
/*									     */
/*									     */
/*****************************************************************************/
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#include "xpat.h"

/* special values & defs for the "Move" type */
#define MOVE(srcind, dstpile)	((Move)(srcind) | (Move)((dstpile) << 9))
#define MOVE_TURNED	(Move)0x8000
#define DSTPILE(m)	(int)((m >> 9) & 63)
#define SRCIND(m)	(int)(m & 511)

/* special moves have bit 15 set and bit 31 also */
#define SPECIAL_MOVE	0x80008000UL
#define SPECIAL_MASK	0xffff8000UL
#define SPECIAL_ARGS	0x00007fffUL

#define COMPOUND_BEGIN	0xfff08000UL
#define COMPOUND_END	0xfff18000UL
#define ADD_CHEAT	0xfff28000UL	/* | value */
#define ROTATE_UP	0xfff38000UL	/* | srcind */
#define ROTATE_DOWN	0xfff48000UL	/* | srcind */
#define SHUFFLING	0xfff58000UL	/*  */
/* add others here */
#define NEW_CARDS_MOVE	0xffff8000UL
#define NO_MOVE		0xffffffffUL


#define DIFFERENT_COLOR(i, j)	(((i) ^ (j)) & 2)

Move RotateUp(Cardindex);
Move RotateDown(Cardindex);
Move SlotShuffle(int fwd);
void check_win_game(void);
void write_log_file(void);
Cardindex maxsequence(Pileindex ind, Cardindex cardind);

int std_good_hint(int srcindex, int dstpile);

void jumpto_movenr(int movenr);	/* without graphics! */


Cardindex complete_suit(Pileindex pile, Cardindex i);
void check_win_game(void);
int in_relaxed_sequence(int srccard, int dstcard);
int in_strong_sequence(int srccard, int dstcard);
Cardindex slotsequence(Pileindex ind, Cardindex start, Cardindex stop);
Cardindex relaxedslotsequence(Pileindex ind, Cardindex start, Cardindex stop);
int move_valid(int, int );
int default_movevalid(int, int );
int move_to_stack(Pileindex);
int check_new_cards(void);

Cardindex stackable(Pileindex);

/* rules.c */
void memo_alloc(int);
void store_move(Move);
int undo_move(void);		/* returns 1, if undo possible */
int redo_move(void);		/* returns 1, is redo possible */
Move do_move(Cardindex, Pileindex);
int all_to_stack(void);
Move give_new_cards(void);

/* hints.c */
void do_last_hint(void);
int generic_automove(Cardindex);

/* events.c */
void event_loop(void);
void resize_desktop(int w, int h, int request);


/* cards.c */
/* void redraw_buttons(int, int, int, int, Window, int, struct singlebutton *); */
void redraw_one_pile(int, int, int, int, int);


/* buttons.c */

/* commands.c */

/* loadsave.c */

/* tools.c */
#define PRANDMAX 1000000000L
void sprand(long);
long prand(void);
