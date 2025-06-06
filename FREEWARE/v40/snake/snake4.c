/* $Id: snake4.c,v 1.5 1996/10/09 16:11:48 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            snake4.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <sverrehu@ifi.uio.no>
 *
 **************************************************************************/

#include <stdlib.h>
#include <string.h>

#ifndef VMS
#include <shhmsg.h>
#include <shhopt.h>
#else
#define COMPILED_DATE "1998"
#define COMPILED_BY "DEC C"
#endif

#include "win.h"
#include "game.h"
#include "score.h"
#include "suid.h"

/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
version(void)
{
    printf(
      "snake4 " VERSION ", by Sverre H. Huseby "
      "(compiled " COMPILED_DATE " by " COMPILED_BY ")\n"
    );
    exit(0);
}

static void
usage(void)
{
#ifndef VMS
    printf(
      "usage: %s [options]\n"
      "\n"
      "  -h, --help                    display this help and exit\n"
      "  -H, --highscores              show highscores and exit\n"
      "  -M, --merge-highscores=FILE   merge highscores from FILE and exit\n"
      "  -V, --version                 output version information and exit\n"
      "\n"
      "In addition, `normal' X11 options are allowed.\n",
      msgGetName()
    );
#else
    printf(
      "usage: snake4 [options]\n"
      "\n"
      "  -h, --help                    display this help and exit\n"
      "  -H, --highscores              show highscores and exit\n"
      "  -M, --merge-highscores=FILE   merge highscores from FILE and exit\n"
      "  -V, --version                 output version information and exit\n"
      "\n"
      "In addition, `normal' X11 options are allowed.\n");
#endif
    exit(0);
}

static void
highscores(void)
{
    scoreDumpHighscores();
    exit(0);
}

static void
merge(char *file)
{
    scoreMergeScoreFile(file);
    scoreDumpHighscores();
    exit(0);
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

int
main(int argc, char *argv[])
{
#ifndef VMS
    optStruct opt[] = {
      /* short long           type        var/func    special       */
        { 'h', "help",        OPT_FLAG,   usage,      OPT_CALLFUNC },
        { 'H', "highscores",  OPT_FLAG,   highscores, OPT_CALLFUNC },
        { 'M', "merge-highscores",
                              OPT_STRING, merge,      OPT_CALLFUNC },
        { 'V', "version",     OPT_FLAG,   version,    OPT_CALLFUNC },
        { 0, 0, OPT_END, 0, 0 }  /* no more options */
    };
    msgSetName(argv[0]);
    suidInit();

    /* to allow showing highscores before connecting to X11 server */
    if (strcmp(msgGetName(), "snake4scores") == 0)
	highscores(); /* exits the program */
#endif

    winParseOptions(&argc, argv);
#ifndef VMS
    optParseOptions(&argc, argv, opt, 0);
#endif

    gameInit();

    winMainLoop();

    gameFinish();

    return 0;
}
