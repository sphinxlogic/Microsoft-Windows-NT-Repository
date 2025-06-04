/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module xpat.h				     */
/*									     */
/*	Copyright (C) 1993, 1994 by Heiko Eissfeldt and Michael Bischoff     */
/*									     */
/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details. (The file COPYRIGHT.GNU)

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.		     */
/*									     */
/*									     */
/*	This file is included by all source files of xpat.		     */
/*	24-Feb-1993: First release (0.1)				     */
/*	04-May-1993: starting to work on 0.7				     */
/*			file renamed to xpat.h from gpat.h		     */
/*	29-Jan-1994: Release 1.0              				     */
/*									     */
/*	02-Apr-1994: Reworked and commented this file, with xpat version 2   */
/*			in mind.					     */
/*									     */
/*									     */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#ifndef EXIT_FAILURE	/* poor old SUN's */
#define EXIT_FAILURE (-1)
#endif

#ifndef max
#define max(a, b)     ((a) > (b) ? (a) : (b))
#define min(a, b)     ((a) < (b) ? (a) : (b))
#endif

typedef int boolean;            /* just one bit of information           */
typedef int Cardindex;		/* range from 0 to MAXCARDS-1            */
typedef int Pileindex;		/* range from 0 to MAXPILES              */
typedef int Card;		/* range from 0 to 51, plus CARDBACK...  */
typedef unsigned long Move;
typedef enum { Stack, Slot, FaceupDeck, FacedownDeck, Tmp } Pile;
typedef enum { Club, Spade, Heart, Diamond } Suit;
typedef enum { Ace, Deuce, Three, Four, Five, Six, Seven, Eight,
	       Nine, Ten, Jack, Queen, King } Rank;


struct key_action {
    char *string;
    void (*action)(void);
    struct key_action *next;
};

extern const char *langdir;	/* a slash, then the name of a subdir */

#define True		1
#define False		0

#define MAXCARDS	(Cardindex)504
#define MAXPILES	(Pileindex)63

/* The rules structure describes the current set of rules. The game.in this
   structure is not modified during a game, not even when selecting 'new game'.
   The game.is copied into the structure by the loadgame() and new_rules()
   functions. The major part is just a copy of the standard description of the
   rule set with the given name. Some entries are customised then. (For
   example, the number of slots.)
   Pointer entries marked as optional can be omitted (i.e. set to NULL) without
   making the game crash. */

extern struct rules {
    const char *shortname;	/* Identifier to select a rule set. It appears
				   in the menus and is used for the icon name.
				   */
    const char *longname;	/* Optional. This can be a longer description,
				   which is used for the window title in the
				   Xlib version. */
    const char *abbrev;		/* Optional. Abbreviation for `shortname', for
				   rule sets with spaces or very long names. */
    int layout_hints;		/* This is a classification of the games, which
				   is mainly used by the layout function to
				   find a nice setting. The current values are
				   0: Gypsy, Spider, Klondike
				   1: FreeCell, SeaHaven (no Deck, Tmp instead)
				   2: Idiot's Delight	 (Just one stack)
				   3: Monte Carlo	(layout must be fixed!)
				   4: Midnight Oil
				   5: Michael's Fantasy
				   6: Royal Cotillion
				   7: Baker's Dozen */
    int variant;		/* storage for some game parameters which don't
				   fit anywhere else. */
#define DECK_SOURCE	0x0001	/* has visible deck from which single cards can
				   be moved (Klondike) */
#define HINTS_LESSER	0x0002	/* only display hints, if their dst pile has
				   a lesser index than the srcpile (MonteCarlo)
				   */
#define STACKS_MULTI	0x0004	/* show hints for multiple move to stack */
#define NODEAL		0x0008	/* this type of game has no 'deal cards' or
				   equivalent */
#define DECK_VISIBLE	0x0010	/* topmost card of the deck is visible */
#define AUTOFILL_TMPS	0x0020	/* tmp spaces are filled from vdeck or ideck */
#define KLONDIKE_DEAL	0x0040	/* Klondike-type of dealing cards */

    int customizable;		/* These bits specify, which game parameters
				   may be customized. */
    int customized;		/* These bits specify, which game parameters
				   actually are customized. */
#define CUSTOM_DECKS	0x001
#define CUSTOM_SLOTS	0x002
#define CUSTOM_FACEUP	0x004
#define CUSTOM_FACEDOWN	0x008
#define CUSTOM_JOKERS	0x010
#define CUSTOM_TMPS	0x020
#define CUSTOM_STD	0x03f	/* everything above ORed together */
#define CUSTOM_PARAM0	0x040
#define CUSTOM_PARAM1	0x080
#define CUSTOM_PARAM2	0x100
#define CUSTOM_PARAM3	0x200

    /* Now comes a block with the game parameters */
    Cardindex numcards;		/* usually 52 or 104 (may be more, if jokers
				   are used) */
    Pileindex numstacks;	/* number of stacks, usually twice the number
				   of decks */
    Pileindex numslots;		/* number of slots */
    Pileindex numtmps;		/* number of temporary storages */
    int numdecks;		/* number of decks */
    int cards_per_color;	/* usually 13. This entry is not supported yet.
				   It will be used to support decks ranking
				   7,8,9,10,jack,queen,king,ace */
    int numjokers;		/* number of jokers. generally 0 */

    /* The following parameters may serve different purposes in different rule
       sets, so their name doesn't give too much info. With every parameter,
       there may be assigned a corresponding counter, in which case the
       parameter is used to initialise the counter. This feature can be used
       for special actions which are allowed only a limited number of times
       per game. Examples are flipping the deck in the Klondike game or
       shuffling the slots in Midnight Oil */
    int param[4];

    int facedown;		/* number of cards facedown in each slot at */
				/* beginning of game (rounded down) */
    int faceup;			/* number of cards faceup */
    int newgame_bits;		/* Directions to the generic newgame function,
				   in case you don't have your own function. */
#define SEQUENTIAL	0x0200	/* Fill slots from beginning to end */
#define SLOTS_SAME	0x0400	/* All slots same number of cards */
#define FORCE_SLOTS	0x0800	/* Extra cards on slots (FreeCell) */
    void (*new_game)(void);	/* Optional: Initialize a new game. In case
				   you supply your own function, you may
				   redefine the meaning of the newgame_bits
				   entry for your own. */
    int (*game_won)(void);	/* Optional: If the winning condition is
				   different from: `all non-jokers on the
				   stacks' */
    int (*new_cards)(void);	/* Optional: check if it is possible to deal
				   new cards or perform a similar action
				   (shuffle, Midnight Oil).
				   The default behaviour is to allow dealing
				   cards, if there are any left. */
    int move_bits;		/* hints about the validity of moves */
#define ES_MASK		0x0003	/* moves to empty slots */
#define ES_NONE		0x0000	/* no move to empty slots allowed.
				   (Midnight Oil) */
#define ES_KINGS	0x0001	/* only kings may go there (Klondike) */
#define ES_ALL		0x0002	/* all cards are allowed (Gypsy, Spider) */

#define US_MASK		0x000c	/* moves to used slots */
#define US_NONE		0x0000	/* no move to used slots allowed */
#define US_RS		0x0004	/* rank and suit must match
				   (Midnight Oil, Seahaven) */
#define US_RA		0x0008	/* rank and alternate colour
				   (Gypsy, FreeCell, Klondike) */
#define US_R		0x000c	/* rank and any suit (Spider) */

#define MG_MASK		0x0030	/* move groups */
#define MG_NONE		0x0000	/* only single cards may be moved */
#define MG_RS		0x0010	/* rank and suit (Spider) */
#define MG_RA		0x0020	/* rank and alternate colour
				   (Gypsy, Klondike) */
#define MG_R		0x0030	/* rank and any suit (currently not needed) */

#define DC_MASK		0x00c0	/* deal cards valid? */
#define DC_ALWAYS	0x0000	/* always deal cards possible, if cards are
				   are there */
#define DC_NOEMPTY	0x0040	/* only deal cards, if no empty slots (Spider)
				   */
#define DC_STRONGOK	0x0080	/* only deal, if all slots are very fine */
#define DC_RELAXEDOK	0x00c0	/* only deal, if all slots are nearly fine */

#define ST_MASK		0x0300
#define ST_NONE		0x0000	/* no cards explicitly movable to stack */
#define ST_ONE		0x0100	/* move single cards to stacks if matches */
#define ST_13		0x0200	/* only full sequence is stackable */

#define STACK_SOURCE	0x4000	/* stack can be source of move */


    Move (*deal_cards)(void);	/* Optional: custom card dealing function */
    void (*undeal_cards)(int);	/* Optional: custom card undealing function */
    Cardindex (*stackable)(Pileindex); /* Optional: custom function to select
				   a group of cards which may be movable to a
				   stack. The function should return the index
				   of the first card to be moved, or -1, if
				   no cards are stackable. */
    int (*movevalid)(Cardindex, Pileindex); /* Optional: In case the above
				   bits are not sufficient to decide about
				   a move, you can define your own function.
				   (Idiot's Delight, Monte Carlo) */
    int (*valid)(Card, Card);	/* Optional: A function to decide if a group
				   of cards may be moved in one turn */
    int (*relaxed_valid)(Card, Card); /* Optional: A function to decide if a
				   group of cards is in sequence in the relaxed
				   sense */
    int (*good_hint)(Cardindex, Pileindex); /* Optional. May return a value
				   of the specified move. Hints are sorted
				   in descending order of their estimated
				   value. If this function is not supplied,
				   all hints are considered to be of equal
				   value. The auto-play function stops, if
				   a move's value does not reach a given bound.
				   */
    int (*automove)(Cardindex);	/* Optional: a function to select a destination
				   pile for the given sequence of cards. should
				   return 0, if the specified card(s) cannot be
				   moved anywhere. The default function should
				   do in most cases */
    int (*score)(void);		/* Optional: a score routine */
    int maxscore;		/* Optional: maximum score reachable */
    int paramstring[4];		/* Optional: used to print out counter[1..3] */
    int inited;			/* set to nz when rules are used */
    void (*initfunc)(void);	/* function to be called if rules are inited */
    struct key_action *local_bindings;
} rules;



/* only parts of this structure are saved: */
extern struct game {
    long seed;			/* seed of this game			   */
    boolean finished;		/* was this game once finished?		   */
    boolean randomgame;		/* was the seed explicitly given	   */
    Card cards[MAXCARDS];	/* array of the cards (0..51, 60..62?)	   */
    Pile piletype[MAXPILES];	/* what sort of pile this is               */
    Cardindex ind[MAXPILES+1];	/* stacks + slots + deck + terminator      */
    boolean visible[MAXCARDS];  /* True, if a card is face-up              */
    int n_moves;		/* currently used moves                    */
    int stored_moves;		/* ptr, for redo and store_move            */
    int move_ptr;		/* ptr, points to current_move             */
    int cheat_count;		/* number of undos which turned back cards */
    int savecount;		/* How often was this game saved?	   */
    int counter[4];		/*  */
    /* here begins the old data structure */
    int numpiles;		/* number of piles in this game		   */
    int bookmark;		/* move number of bookmark		   */
    Cardindex srcind;
#define UNSELECTED	(-1)
#define ROTATE_UP_SEL	(-2)
#define ROTATE_DOWN_SEL	(-3)
    Cardindex arrow_srcind;
    Pileindex arrow_dstpile;
    boolean graphic;
    int pile_changed[MAXPILES];
    int numalloc;		/* space allocated for stored moves        */
    Move *move;			/* pointer to moves                        */
    int disable[MAXPILES];	/* disable bits		                   */
} game;
#define PILE_UNCHANGED	0x7fff

#define SUIT(i)			((i) & 3)
#define RANK(i)			((i) >> 2)
#define JOKER			60
#define IS_JOKER(i)		((i) >= JOKER)
#define INDEX_OF_LAST_CARD(i)	(game.ind[(i)+1]-1)
#define	INDEX_OF_FIRST_CARD(i)	game.ind[i]
#define STACK(i)		(i)
#define SLOT(i)			((i) + rules.numstacks)
#define IDECK			(game.numpiles-1)
#define VDECK			(game.numpiles-2)
#define IS_STACK(i)		((i) < rules.numstacks)
#define IS_SLOT(i)		(game.piletype[i] == Slot)
#define CARDS_ON_PILE(i)	(game.ind[(i)+1] - game.ind[(i)])
#define EMPTY(i)		(!CARDS_ON_PILE(i))
#define FIRST_SLOT		(rules.numstacks)
#define LAST_SLOT		(rules.numstacks+rules.numslots-1)
#define FIRST_STACK		0
#define LAST_STACK		(rules.numstacks-1)
#define CARDS_ON_DECK		(rules.numcards - game.ind[IDECK])


/* Prototypes: */

/* Prototypes of the Xlib interface / widget that are known to the game: */
/* (these should go into a different .h file) */
#if SOUND
int checksound(void);
void play_sound(const char *);
#else
#define play_sound(x)
#endif
void move_card_data(Cardindex dst, Cardindex src);	/* cards.c */
void draw_pileupdate(Pileindex pile, int delta);
boolean pile_resize(Pileindex);
void new_rules_notify(void);
void win_game(void);					/* ? */
void refresh_screen(void); /* request complete redraw */ /* ? */
void show_arrow(boolean);  /* show/hide the hint arrow */
void show_mark(boolean);	/* markcard.c */
void show_message(const char *, ...);
void request_confirm(void (*dofunc)(void), const char *prompt);

/* Prototypes of the game that are known to the Xlib interface / widget: */

typedef enum { Disable, Enable, EnableAndRedraw } GraphicsControl;
void graphics_control(GraphicsControl cmd);
void graphics_pile_control(GraphicsControl cmd, Pileindex pile);
Pileindex getpile(Cardindex);
void key_pressed(char *str);
void button_pressed(Pileindex pile, Cardindex card, int button);
void switch_uid(int);
void save_game(const char *);
void load_game(const char *);
void new_rules(const char *, int, Pileindex, int, int, int, int, int, int, int, int);
void newgame(long);
void change_rules(const char *new_rules_name);
void *malloc_(size_t);
char *strsav(const char *);

/* commands (which can be assigned to buttons */
void rq_LeavePat(void);
void rq_RestartGame(void);
void rq_AnotherGame(void);
void cmd_ResetHints(void);
void cmd_DoHint(void);
void cmd_NextHint(void);
void cmd_PreviousHint(void);
void cmd_CancelSelection(void);
void cmd_AllMoves(void);
void cmd_UndoMove(void);
void cmd_RedoMove(void);
void cmd_DealCards(void);
void cmd_LeavePat(void);
void cmd_ShowScore(void);
void cmd_AnotherGame(void);
void cmd_RestartGame(void);
void cmd_ReplayGame(void);
void cmd_Layout(void);
void cmd_SaveGame(void);
void cmd_ShowVersion(void);
void cmd_ToStack(void);
void cmd_OneToStack(void);	/* selected card(s) */
void cmd_AllToStack(void);	/* all possible */
void cmd_DropBookmark(void);
void cmd_GotoBookmark(void);
void cmd_Cancel(void);
void cmd_Confirm(void);
void cmd_Info(void);
void cmd_MinWindow(void);
void cmd_PreferredWindow(void);
void cmd_RotateUp(void);
void cmd_RotateDown(void);

/* Messages -- internationalisation */
void read_message_file(const char *filename);
void read_keyboard_file(const char *filename);
void add_keybinding(struct key_action **cp, const char *cmd, const char *function);
struct rules *getrules(const char *ruleset);

extern const char *US_rank_name[], *US_suit_name[];
extern const char *xpat_messages[];
#define TXT_QUIT_CONFIRM	(xpat_messages[0])
#define TXT_NEW_CONFIRM		(xpat_messages[1])
#define TXT_RESTART_CONFIRM	(xpat_messages[2])
#define TXT_SUIT		(xpat_messages+3)
#define TXT_SUIT0		(xpat_messages[3])
#define TXT_SUIT1		(xpat_messages[4])
#define TXT_SUIT2		(xpat_messages[5])
#define TXT_SUIT3		(xpat_messages[6])
#define TXT_JOKER		(xpat_messages[7])
#define TXT_RANK		(xpat_messages+8)
#define TXT_ACE			(xpat_messages[8])
#define TXT_DEUCE		(xpat_messages[9])
#define TXT_THREE		(xpat_messages[10])
#define TXT_FOUR		(xpat_messages[11])
#define TXT_FIVE		(xpat_messages[12])
#define TXT_SIX			(xpat_messages[13])
#define TXT_SEVEN		(xpat_messages[14])
#define TXT_EIGHT		(xpat_messages[15])
#define TXT_NINE		(xpat_messages[16])
#define TXT_TEN			(xpat_messages[17])
#define TXT_JACK		(xpat_messages[18])
#define TXT_QUEEN		(xpat_messages[19])
#define TXT_KING		(xpat_messages[20])
#define TXT_STACK		(xpat_messages[21])
#define TXT_SLOT		(xpat_messages[22])
#define TXT_DECK		(xpat_messages[23])
#define TXT_TMP			(xpat_messages[24])
#define TXT_IDECK		(xpat_messages[25])
#define TXT_NOHINTGIVEN		(xpat_messages[26])
#define TXT_CANMOVE		(xpat_messages[27])
#define TXT_NOHINTS		(xpat_messages[28])
#define TXT_WRAPPING		(xpat_messages[29])
#define TXT_BOOKMARK_SET	(xpat_messages[30])
#define TXT_YOU_WIN		(xpat_messages[31])
#define TXT_NONE_TO_STACK	(xpat_messages[32])
#define TXT_SAVE_ERR_BASIC	(xpat_messages[33])
#define TXT_LOAD_ERR_BASIC	(xpat_messages[34])
#define TXT_SAVE_ERR_OPEN	(xpat_messages[35])
#define TXT_LOAD_ERR_OPEN	(xpat_messages[36])
#define TXT_SAVE_ERR_HEADER	(xpat_messages[37])
#define TXT_LOAD_ERR_HEADER	(xpat_messages[38])
#define TXT_SAVE_ERR_MOVES	(xpat_messages[39])
#define TXT_LOAD_ERR_MOVES	(xpat_messages[40])
#define TXT_SAVE_OK		(xpat_messages[41])
#define TXT_LOAD_OK		(xpat_messages[42])
#define TXT_LOAD_ERR_BADMAGIC	(xpat_messages[43])

#define TXT_FLIPPING		(xpat_messages[45])
#define TXT_NEWCARDS		(xpat_messages[46])
#define TXT_NONEWCARDS		(xpat_messages[47])
#define TXT_NOSOURCEPILE	(xpat_messages[48])
#define TXT_MOVED_TO_STACK	(xpat_messages[49])
#define TXT_MOVENOTPOSSIBLE	(xpat_messages[50])
#define TXT_NOUNDO		(xpat_messages[51])
#define TXT_UNDO		(xpat_messages[52])
#define TXT_UNDOCHEAT		(xpat_messages[53])
#define TXT_NOREDO		(xpat_messages[54])
#define TXT_REDO		(xpat_messages[55])
#define TXT_REDOUNCHEAT		(xpat_messages[56])
#define TXT_INVALIDMOVE		(xpat_messages[57])
#define TXT_BADSRC		(xpat_messages[58])
#define TXT_SRCSELECTED		(xpat_messages[59])
#define TXT_BADSLOT		(xpat_messages[60])
#define TXT_ABORTED		(xpat_messages[61])
#define TXT_BADCMD		(xpat_messages[62])
#define TXT_NOMOVE		(xpat_messages[63])
#define TXT_VERSION		(xpat_messages[64])
#define TXT_INFO1		(xpat_messages[65])
#define TXT_SCORE		(xpat_messages[66])
#define TXT_NOSCORE		(xpat_messages[67])
#define TXT_CARDSLEFT		(xpat_messages[68])
#define TXT_HELP_KEYS		(xpat_messages[69])
#define TXT_HELP_RULES		(xpat_messages[70])
#define TXT_HELPFILES		(xpat_messages+71)



#define TXT_LOADING		(xpat_messages[92])
#define TXT_WELCOME		(xpat_messages[93])

#define TXT_ROTUP 		(xpat_messages[95])
#define TXT_ROTDN 		(xpat_messages[96])
#define TXT_SLOTREQUIRED	(xpat_messages[97])
#define TXT_CARDNOTVISIBLE	(xpat_messages[98])

#define TXTI_SHUFFLE		100	/* block of 4 messages */
#define TXTI_FLIP		104	/* " */
#define TXTI_ROTATE 		108	/* " */
#define TXTI_CARDS 		112	/* " */
#define TXT_NOFLIPLEFT		(xpat_messages[105])
#define TXT_NODEAL		(xpat_messages[112])
#define TXT_NOCARDSLEFT		(xpat_messages[113])
#define TXT_MOVEBLOCK		116
#define TXT_SCOREBLOCK		120

extern int rng;
