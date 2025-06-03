/*
 * table.c - set up translation tables for texi2roff
 *	     Release 1.0a	August 1988
 *
 * Copyright 1988  Beverly A.Erlebacher
 * erlebach@csri.toronto.edu	...uunet!utai!utcsri!erlebach
 *
 * When adding more commands: 
 *
 * - be sure that gettoken() can recognize not just the ending token
 *   (texend) but also the end of the starting token (texstart) for
 *   the command, if it doesnt already occur in a table.
 *
 * - keep the tables sorted
 *
 * - keep all troff output strings in the table macro header files
 *
 * - strive diligently to keep the program table-driven
 */

#include <stdio.h>	/* just to get NULL */
#include "texi2roff.h"
#include "tablems.h"
#include "tableme.h"
#include "tablemm.h"

struct misccmds * cmds;
struct tablerecd * table;
int tablesize;

void
initialize(macropkg, showInfo)
int macropkg;
int showInfo;
{
    extern void patchtable();

    switch (macropkg) {
    case MS:
	table = mstable;
	tablesize = sizeof mstable;
	cmds = &mscmds;
	if (showInfo == YES)
	    (void) patchtable( &msnodeentry, &msmenuentry, &msifinfoentry);
	puts(msinit);
	break;
    case MM:
	table = mmtable;
	tablesize = sizeof mmtable;
	cmds = &mmcmds;
	if (showInfo == YES)
	  (void) patchtable( &mmnodeentry, &mmmenuentry, &mmifinfoentry);
	puts(mminit);
	break;
    case ME:
	table = metable;
	tablesize = sizeof metable;
	cmds = &mecmds;
	if (showInfo == YES)
	   (void) patchtable( &menodeentry, &memenuentry, &meifinfoentry);
	puts( meinit);
	break;
    }
}

/*
 * real Texinfo has a sort of hypertext feature called Info files,
 * using menus, nodes and 'ifinfo' sections. Although i can't simulate
 * this here, and the material would not normally be printed by Texinfo,
 * it could be useful to the user searching through a machine-readable
 * manual. Specifying the -I option patches the table with alternative
 * troff commands to display rather than discard this material.
 */

void
patchtable(nodeentry, menuentry, ifinfoentry)
struct tablerecd * nodeentry;
struct tablerecd * menuentry;
struct tablerecd * ifinfoentry;
{
    struct tablerecd * tp;
    extern struct tablerecd * lookup();

    /* not everybody has structure assignment */
    if ((tp = lookup("@menu")) != NULL) {
	tp->trfstart = menuentry->trfstart;
	tp->trfend = menuentry->trfend;
	tp->type = menuentry->type;
    }
    if ((tp = lookup("@node")) != NULL) {
	tp->trfstart = nodeentry->trfstart;
	tp->trfend = nodeentry->trfend;
	tp->type = nodeentry->type;
    }
    if ((tp = lookup("@ifinfo")) != NULL) {
	tp->trfstart = ifinfoentry->trfstart;
	tp->trfend = ifinfoentry->trfend;
	tp->type = ifinfoentry->type;
    }
}
