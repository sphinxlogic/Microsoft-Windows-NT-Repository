/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module messages.c			     */
/*									     */
/*	Internationalisation and keyboard translation/customisation	     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	04-Apr-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

static const char
    n_ace[] = "Ace", n_deuce[] = "Deuce", n_three[] = "Three",
    n_four[] = "Four", n_five[] = "Five", n_six[] = "Six", n_seven[] = "Seven",
    n_eight[] = "Eight", n_nine[] = "Nine", n_ten[] = "Ten", n_jack[] = "Jack",
    n_queen[] = "Queen", n_king[] = "King", n_joker[] = "Joker",
    n_diamonds[] = "Diamonds", n_hearts[] = "Hearts", n_spades[] = "Spades",
    n_clubs[] = "Clubs";

const char *US_rank_name[] = {
    n_ace, n_deuce, n_three, n_four, n_five, n_six, n_seven, n_eight, n_nine,
    n_ten, n_jack, n_queen, n_king };
const char *US_suit_name[] = {
    n_clubs, n_spades, n_hearts, n_diamonds };

const char *xpat_messages[] = {
    "Quit game?",
    "Another game?",
    "Restart game?",
    n_clubs,
    n_spades,
    n_hearts,
    n_diamonds,
    n_joker,
    n_ace,
    n_deuce,
    n_three,
    n_four,
    n_five,
    n_six,
    n_seven,
    n_eight,
    n_nine,
    n_ten,
    n_jack,
    n_queen,
    n_king,
    "stack",
    "slot",
    "the talon",
    "tmp",
    "the stock",
    "No hint was given!",
    "You can move %P (%R of %S) to %P.",
    "No hints!",
    "(wrapping)",
    "Bookmark set.",
    "You won!",
    "No moves to stack possible.",
    "Saving of game FAILED.",
    "Loading the game FAILED.",
    "Could not open file.",
    "Could not open file.",
    "Could not write header.",
    "Could not read header.",
    "Could not write moves.",
    "Could not read moves.",
    "Saving the game succeeded.",
    "Loading the game succeeded.",
    "Magic match failed.",
    "No flip left.",
    "flipping",
    "new cards",
    "No new cards!",
    "No source pile selected.",
    "Cards moved to stack.",
    "Move not possible.",
    "Undo not possible.",
    "Undo!",
    "Undo (cheat!)",
    "Redo not possible.",
    "Redo!",
    "Redo (& uncheat)",
    "Invalid move",
    "Empty or invalid source pile",
    "source pile selected",
    "not so many slots",
    "aborted",
    "Unknown command:",
    "no move possible",
    "Xpat2 version",
    "Seed is %ld, cheat count %d. ",
    "Score %d of %d.",
    "(No score for these rules)",
    "",
    "Help on keys",
    "Help on %s rules",
    "help.keys",
    "",
    "",
    "",
    "",
    "help.Spider",	/* we keep the names to 14 characters. Not nice, but POSIX */
    "help.Gypsy",
    "help.Klondike",
    "help.Seahaven",
    "help.FreeCell",	/* 80 */
    "help.Delight",
    "hlpMonteCarlo",
    "hlpMidnightOil",
    "hlpCalculation",
    "hlpmodCanfield",
    "help.Michaels",
    "help.Canfield",
    "help.Cotillion",
    "hlpBakersDozen",
    "",
    "",
    "Please wait, loading cards...",
    "Welcome to XPat2 version %s!",
    "",
    "Rotate up selected.",
    "Rotate down selected.",
    "",
    "Card must be visible.",
    "For this action, a slot is required.",
    "In these rules, the cards cannot be shuffled.",	/* 100 */
    "No shuffles left.",
    "One shuffle left.",
    "%d shuffles left.",
    "In these rules, the deck cannot be flipped.",
    "No flip left.",
    "One flip left.",
    "%d flips left.",
    "This function is not available in these rules.",
    "No slot rotations left.",
    "One slot rotation left.",
    "%d slot rotations left.",
    "No ``deal cards'' in these rules.",
    "No more cards on the deck.",
    "One last card on the deck.",
    "%d cards left on deck.",
    "Starting position.",
    "One move done.",
    "%d moves done.",
    "",
    "Not a single point of",
    "One point of",
    "Score: %d points of",
};

void *malloc_(size_t n) {
    void *p;
    if (!(p = malloc(n))) {
	fprintf(stderr, "out of memory\n");
	exit(EXIT_FAILURE);
    }
    return p;
}

char *strsav(const char *txt) {
    char *p = malloc_(1 + strlen(txt));
    strcpy(p, txt);
    return p;
}

void read_message_file(const char *filename) {
    FILE *fp;
    int i;
    char line[256];
    if (*filename != '/') {
#ifndef VMS
	sprintf(line, "%s/%s/%s", LIBDIR, langdir, filename);
#else
        if (strlen(langdir) == 0) langdir = "000000";
	sprintf(line, "%s[%s]%s", LIBDIR, langdir, filename);
#endif
	filename = line;
printf ("Read message file: %s\n",filename);
    }
    if (!(fp = fopen(filename, "r")))
	return;		/* Xt philosophy: ignore error */
    for (i = 0; i < sizeof(xpat_messages) / sizeof(const char *); ++i) {
	char *p;
    again:
	if (!fgets(line, sizeof(line), fp))
	    break;	/* EOF */
	if (*line == '#')
	    goto again;
	if ((p = strrchr(line, '\n')))
	    *p = '\0';
	if (*line)	/* empty lines => keep old text */
	    xpat_messages[i] = strsav(line);
    }
    fclose(fp);
}

static void cmd_None(void) {}

static void replace_binding(struct key_action *p, const char *name) {
    int i;
    static struct translator {
	const char *name;
	void (*func)(void);
    } translator[] = {
	{ "None",		cmd_None },
	{ "rq_LeavePat",	rq_LeavePat },
	{ "rq_RestartGame",	rq_RestartGame },
	{ "rq_AnotherGame",	rq_AnotherGame },
	{ "ResetHints",		cmd_ResetHints },
	{ "DoHint",		cmd_DoHint },
	{ "NextHint",		cmd_NextHint },
	{ "PreviousHint",	cmd_PreviousHint },
	{ "CancelSelection",	cmd_CancelSelection },
	{ "AllMoves",		cmd_AllMoves },
	{ "UndoMove",		cmd_UndoMove },
	{ "RedoMove",		cmd_RedoMove },
	{ "DealCards",		cmd_DealCards },
	{ "LeavePat",		cmd_LeavePat },
	{ "ShowScore",		cmd_ShowScore },
	{ "AnotherGame",	cmd_AnotherGame },
	{ "RestartGame",	cmd_RestartGame },
	{ "ReplayGame",		cmd_ReplayGame },
	{ "Layout",		cmd_Layout },
	{ "SaveGame",		cmd_SaveGame },
	{ "ShowVersion",	cmd_ShowVersion },
	{ "MinWindow",		cmd_MinWindow },
	{ "PreferredWindow",	cmd_PreferredWindow },
	{ "Info",		cmd_Info },
	{ "ToStack",		cmd_ToStack },
	{ "OneToStack",		cmd_OneToStack },	/* selected card(s) */
	{ "AllToStack",		cmd_AllToStack },	/* all possible */
	{ "DropBookmark",	cmd_DropBookmark },
	{ "GotoBookmark",	cmd_GotoBookmark },
	{ "RotateUp",		cmd_RotateUp },
	{ "RotateDown",		cmd_RotateDown },
	{ "Cancel",		cmd_Cancel },
	{ "Confirm",		cmd_Confirm }
    };
    for (i = 0; i < sizeof(translator) / sizeof(translator[0]); ++i)
	if (!strcmp(name, translator[i].name)) {
	    p->action = translator[i].func;
	    return;
	}
    fprintf(stderr, "WARNING: no function corresponds to \"%s\"\n", name);
    p->action = cmd_None;
}

static struct key_action *global_bindings = NULL;
const char *langdir = "";

void add_keybinding(struct key_action **cp, const char *cmd, const char *function) {
    /* a NULL pointer for cmd is a wildcard */
    int done = 0;
    struct key_action *p;

    if (!cp)
	return;		/* bindings for unimplemented rulesets */
    while (*cp) {
	p = *cp;
	if (!cmd || !strcmp(p->string, cmd)) {
	    /* replace this! */
	    replace_binding(p, function);
	    done = 1;
	}
	cp = &(p->next);
    }
    if (!done && cmd) {
	/* didn't find previous command */
	/* add a new entry */
	p = *cp = malloc_(sizeof(struct key_action));
	p->next = NULL;
	p->string = strsav(cmd);
	replace_binding(p, function);
    }
}


void read_keyboard_file(const char *filename) {
    FILE *fp;
    char line[256];
    char buff[32], cmd[2];
    struct key_action **cp = &global_bindings;

    cmd[1] = '\0';	/* 1-char commands currently */
    if (*filename != '/') {
#ifndef VMS
	sprintf(line, "%s/%s/%s", LIBDIR, langdir, filename);
#else
        if (strlen(langdir) == 0) langdir = "000000";
	sprintf(line, "%s[%s]%s", LIBDIR, langdir, filename);
#endif
	filename = line;
printf ("Read keyboard file: %s\n",filename);
    }
    if (!(fp = fopen(filename, "r"))) {
	fprintf(stderr, "FAILED to open keyboard file \"%s\"\n", filename);
	return;		/* Xt philosophy: ignore error */
    }
    while (fgets(line, sizeof(line), fp)) {
	char *p;
	if ((p = strrchr(line, '\n')))
	    *p = '\0';
	if (!*line)
	    continue;
	if (!strncmp(line, "#c", 2))	/* comment */
	    continue;
	if (!strncmp(line, "#rules ", 7)) {	/* switch to other rules */
	    struct rules *rp;
	    p = line+7;
	    if (!(rp = getrules(p)))
		cp = NULL;
	    else
		cp = &(rp->local_bindings);
	    continue;
	}
	if (!strncmp(line, "#include ", 9)) {	/* include */
	    read_keyboard_file(line+9);
	    continue;
	}
	if (!strncmp(line, "#x", 2) && strlen(line) < 32) { /* hex number */
	    int c;
	    sscanf(line+2, "%x %s", &c, buff);
	    cmd[0] = c;
	    add_keybinding(cp, cmd, buff);
	    continue;
	}
	/* else assume string<space>commandname */
	if (strlen(line) < 32 && (p = strchr(line+1, ' '))) {
	    *p = '\0';
	    add_keybinding(cp, line, p+1);
	    continue;
	}
	fprintf(stderr, "Warning: cannot parse line in keys file:%s\n", line);
    }
    fclose(fp);
}


void key_pressed(char *str) {
    struct key_action *p;

    /* search for local binding */
    for (p = rules.local_bindings; p; p = p->next)
	if (!strcmp(p->string, str)) {
	    (*p->action)();
	    return;
	}

    /* search for global binding */
    for (p = global_bindings; p; p = p->next)
	if (!strcmp(p->string, str)) {
	    (*p->action)();
	    return;
	}

    /* not found. break up the string to smaller pieces */
    if (strlen(str) > 1) {
	char s[2];
	while (*str) {
	    s[0] = *str++;
	    s[1] = '\0';
	    key_pressed(s);
	}
    } else	/* last resort: */
	/* use hardcoded entries. Note: These can be overridden by assigning them "None" before. */

    switch (str[0]) {
    case '\014':
	cmd_ResetHints();	/* complicated to save */
	refresh_screen();
	break;
    case 27:	/* abort cmd */
	cmd_CancelSelection();
	show_message(TXT_ABORTED);
	return;
    }
    if (str[0]) {
	const char *rulechange = "FGHKSIMOBCR";
	const char *rulename[] = { "Free Cell", "Gypsy", "Seahaven", "Klondike",
				       "Spider", "id", "mc", "mo", "bd", "Canfield", "rc" };
	const char *s;

	if ((s = strchr(rulechange, str[0]))) {
	    change_rules(rulename[s-rulechange]);
	    return;
	}
    }
}
