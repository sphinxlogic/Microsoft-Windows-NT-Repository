/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module loadsave.c			     */
/*									     */
/*	Functions for game logging and loading/saving games.		     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*	24-Feb-1993: First release (0.1)				     */
/*	19-Mar-1993: POSIX.1 now optional				     */
/*	24-Mar-1993: changed filename "log" to "xpat.log"		     */
/*									     */
/*									     */
/*****************************************************************************/
#include "xpatgame.h"

#define NARGS	32	/* twice the size of xpat 1 */
#define BUFSIZE	32	/* at least length of longest shortname + 1 */

#define MAGIC1	0x7419	/* magic of xpat version 1 */
#define MAGIC2	0x741a	/* magic of xpat version 2 */

#include <limits.h>

#ifndef _POSIX_NAME_MAX
#  undef _POSIX_SOURCE		/* seems that we have no POSIX system! */
#  define NAME_MAX	14	/* every UNIX should have at least this */
#else
#  include <unistd.h>		/* for pathconf() */
#  include <sys/utsname.h>	/* for uname() */
#endif

#include <time.h>
#include "version.h"


/* code to switch between real and effective user id */
/* we must use the real user id when saving files */

void switch_uid(int to_real) {
#ifdef _POSIX_SAVED_IDS
    static int uid_state = -1; /* -1 = unknown, 1 = real, 0 = effective */
    static uid_t real_uid, effective_uid;
    if (uid_state < 0) {
	real_uid = getuid();
	effective_uid = geteuid();
	uid_state = 0;
    }
    if (to_real != uid_state && real_uid != effective_uid) {
	setuid(to_real ? real_uid : effective_uid);
	uid_state = to_real;
    }
#endif
}

static void read_err(const char *msg) {
    if (game.graphic) {
	show_message("%s %s", TXT_LOAD_ERR_BASIC, msg);
	cmd_LeavePat();						/* make it better! */
    }
    fprintf(stderr, "%s %s\n", TXT_LOAD_ERR_BASIC, msg);
    exit(EXIT_FAILURE);
}

static void portable_to_internal(long *args, unsigned char *p, int num) {
    do {
	int j;
	*args = 0;
	for (j = 0; j < 4; ++j)
	    *args += (long)p[3-j] << (j << 3);
	++args;
	p += 4;
    } while (--num);
}

static void internal_to_portable(unsigned char *p, long *args, int num) {
    do {
	int j;
	memset(p, (*args < 0 ? -1 : 0), 4);
	for (j = 0; j < 4; ++j)
	    p[3-j] = (unsigned char)(*args >> (j << 3));
	++args;
	p += 4;
    } while (--num);
}

void load_game(const char *file) {
    FILE *fp;
    char buffer[BUFSIZE];
    long args[NARGS];
    int i;
    unsigned char p[NARGS * 4];

    if (!(fp = fopen(file, "rb")))
	read_err(TXT_LOAD_ERR_OPEN);
    if (fread(buffer, 1, BUFSIZE, fp) != BUFSIZE ||
	fread(p, 4, 16, fp) != 16)
	read_err(TXT_LOAD_ERR_HEADER);
    portable_to_internal(args, p, 16);

    switch ((int)args[15]) {
    case MAGIC1:	/* read xpat version 1 game.file */
	for (i = 16; i < NARGS; ++i)	/* no further game.*/
	    args[i] = -1;
	break;
    case MAGIC2:	/* must read extended header */
	if (fread(p, 4, NARGS-16, fp) != NARGS-16)
	    read_err(TXT_LOAD_ERR_HEADER);
	portable_to_internal(args+16, p, NARGS-16);
	break;
    default:
	read_err(TXT_LOAD_ERR_BADMAGIC);
    }
    new_rules(buffer, args[0], args[1], args[2], args[3], args[4], args[18], args[19], args[5], args[6], args[7]);
    if (game.numalloc < args[9]) {
	if (game.numalloc)
	    free(game.move);
	memo_alloc((args[9]|15)+1);
    }
    newgame(args[8]);
    game.stored_moves = args[9];
    game.finished = args[12];
    for (i = 0; i < game.stored_moves; i += 16) {
	if (fread(p, 4, 16, fp) != 16) {
	    read_err(TXT_LOAD_ERR_MOVES);
	    break;
	}
	portable_to_internal((long *)(game.move+i), p, 16);
    }
    fclose(fp);
    if (args[15] == MAGIC1)
	/* convert moves to version 2 */
	for (i = 0; i < game.stored_moves; ++i)
	    if ((game.move[i] & SPECIAL_MASK) != NEW_CARDS_MOVE)
		/* reset MOVE_TURNED flag in the upper part */
		game.move[i] &= 0x7fffffffUL;
    ++game.savecount;
    game.bookmark = args[10];		/* preset bookmark to load point */
    if (game.graphic) {
	graphics_control(Disable);
	cmd_GotoBookmark();
	graphics_control(Enable);
	refresh_screen();		/* draw complete screen */
    } else
	cmd_GotoBookmark();		/* graphics disabled anyway */
    assert(game.move_ptr == args[10]);
    assert(game.counter[1] == args[13]);	/* same for 2..4 */
    game.cheat_count = args[11];
}


void save_game(const char *file) {
    FILE *fp;
    char buffer[32];
    char filename[64];	/* must be 15 at least */
    long args[24];
    int i;
    unsigned char p[4 * 24];

    if (!file) {
	/* compute the filename to use */
	long name_max;

#ifdef NAME_MAX	
	name_max = NAME_MAX;
#else
	if ((name_max = pathconf(".", _PC_NAME_MAX)) == -1L)
	    name_max = _POSIX_NAME_MAX;	/* error => use 14 chars */
#endif
	if (name_max >= sizeof(filename))
	    name_max = sizeof(filename) - 1;
	/* copy characters to the filename string */
	/* POSIX allows digits, letters, '.', '-', '_' */
	{   char *wr; const char *rd;
	    rd = rules.shortname;
	    for (wr = filename; *rd && wr - filename <= name_max - 10; ++rd)
		if (isalnum(0xff & *rd) || *rd == '-')
		    *wr++ = *rd;
	    sprintf(wr, ".%09ld", game.seed);
	}
	file = filename;
    }

    switch_uid(1);	/* saves in users home directory */
    if (!(fp = fopen(file, "wb"))) {
	show_message("%s %s", TXT_SAVE_ERR_BASIC, TXT_SAVE_ERR_OPEN);
	goto werr2;
    }
    strcpy(buffer, rules.shortname);
    for (i = 0; i < NARGS; ++i)
	args[i] = -1;
    if (rules.customized & CUSTOM_DECKS)
	args[0] = rules.numdecks;
    if (rules.customized & CUSTOM_SLOTS)
	args[1] = rules.numslots;
    if (rules.customized & CUSTOM_FACEUP)
	args[2] = rules.faceup;
    if (rules.customized & CUSTOM_FACEDOWN)
	args[3] = rules.facedown;
    if (rules.customized & CUSTOM_JOKERS)
	args[4] = rules.numjokers;
    if (rules.customized & CUSTOM_TMPS)
	args[18] = rules.numtmps;
    if (rules.customized & CUSTOM_PARAM0)
	args[19] = rules.param[0];
    if (rules.customized & CUSTOM_PARAM1)
	args[5] = rules.param[1];
    if (rules.customized & CUSTOM_PARAM2)
	args[6] = rules.param[2];
    if (rules.customized & CUSTOM_PARAM3)
	args[7] = rules.param[3];
    args[ 8] = game.seed;
    args[ 9] = game.stored_moves;
    args[10] = game.move_ptr;
    args[11] = game.cheat_count;
    args[12] = game.finished;
    args[13] = game.counter[1];
    args[14] = game.counter[2];
    args[15] = MAGIC2;
    args[16] = game.counter[3];
    args[17] = game.counter[0];
    internal_to_portable(p, args, NARGS);
    if (fwrite(buffer, 1, BUFSIZE, fp) != BUFSIZE ||
	fwrite(p, 4, NARGS, fp) != NARGS) {
	show_message("%s %s", TXT_SAVE_ERR_BASIC, TXT_SAVE_ERR_HEADER);
	goto werr;
    }
    for (i = 0; i < game.stored_moves; i += 16) {
	internal_to_portable(p, (long *)(game.move+i), 16);
	if (fwrite(p, 4, 16, fp) != 16) {
	    show_message("%s %s", TXT_SAVE_ERR_BASIC, TXT_SAVE_ERR_MOVES);
	    goto werr;
	}
    }
    fclose(fp);
    switch_uid(0);
    show_message(TXT_SAVE_OK);
    play_sound("ok");	/* found no sound file for this. maybe later */
    return;

werr:
    fclose(fp);
werr2:
    switch_uid(0);
    play_sound("cannotsave");
}

void write_log_file(void) {
    time_t t;
    FILE *fp;

#ifdef _POSIX_SOURCE
    struct utsname utsname;
    const char *username;
#endif

    t = time((time_t *)0);

    if (!(fp = fopen(SCOREFILE, "a"))) {
	fprintf(stderr, "xpat: warning: cannot write to log file "
		SCOREFILE "\n");
	return;		/* cannot write to file */
    }
    fprintf(fp, "\n%s", ctime(&t));

#ifdef _POSIX_SOURCE
    if ((username = getlogin()) != NULL
	/* cuserid has been removed in the 1990 POSIX revision. */
	/* Thus we make the following code optional (default: use it) */
#ifndef NO_CUSERID
    	/* if started directly under a window-manager menu, getlogin()
    	   returns NULL, so get username with cuserid() (ThMO) */
	|| (username = cuserid(NULL)) != NULL
#endif
	)
	fprintf(fp, "  \"%s\"", username);
    else
#endif
	fprintf(fp, "  someone");

#ifdef _POSIX_SOURCE
    if (uname(&utsname) >= 0)
	fprintf(fp, " on %s\n  running %s Version %s (%s)\n ",
		utsname.nodename,
		utsname.sysname, utsname.release, utsname.version);
#endif

    fprintf(fp, " finished patience %s\n", rules.shortname);
    if (rules.customized & CUSTOM_DECKS)
	fprintf(fp, "  with %d decks\n", rules.numdecks);
    if (rules.customized & CUSTOM_SLOTS)
	fprintf(fp, "  with %d slots\n", rules.numslots);
    if (rules.customized & CUSTOM_TMPS)
	fprintf(fp, "  with %d tmps\n", rules.numtmps);
    if (rules.customized & CUSTOM_FACEUP)
	fprintf(fp, "  with %d cards faceup\n", rules.faceup);
    if (rules.customized & CUSTOM_FACEDOWN)
	fprintf(fp, "  with %d cards facedown\n", rules.facedown);
    if (rules.customized & CUSTOM_JOKERS)
	fprintf(fp, "  with %d jokers\n", rules.numjokers);

    if (rules.customized & CUSTOM_PARAM0)
	fprintf(fp, "  param0 = %d\n", rules.param[0]);
    if (rules.customized & CUSTOM_PARAM1)
	fprintf(fp, "  param1 = %d\n", rules.param[1]);
    if (rules.customized & CUSTOM_PARAM2)
	fprintf(fp, "  param2 = %d\n", rules.param[2]);
    if (rules.customized & CUSTOM_PARAM3)
	fprintf(fp, "  param3 = %d\n", rules.param[3]);
    if (game.counter[0])
	fprintf(fp, "  counter0 = %d\n", game.counter[0]);
    if (game.counter[1])
	fprintf(fp, "  counter1 = %d\n", game.counter[1]);
    if (game.counter[2])
	fprintf(fp, "  counter2 = %d\n", game.counter[2]);
    if (game.counter[3])
	fprintf(fp, "  counter3 = %d\n", game.counter[3]);


    fprintf(fp, "  using xpat2 version %s, game seed %9ld with %d moves\n",
	    VERSION, game.seed, game.n_moves);
    if (game.cheat_count)
	fprintf(fp, "  and cheatet! (count = %d)\n", game.cheat_count);
    else
	fprintf(fp, "  in a truly noble manner!\n");
    fclose(fp);
}
