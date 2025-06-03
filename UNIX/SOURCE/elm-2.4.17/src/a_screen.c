
static char rcsid[] = "@(#)$Id: a_screen.c,v 5.1 1992/10/03 22:58:40 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: a_screen.c,v $
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/**  alias screen display routines for ELM program

**/

#include "headers.h"
#include "s_aliases.h"

char *show_status();
char *alias_type();

alias_screen()
{
	/* Stolen from showscreen() */

	ClearScreen();

	alias_title();

	last_header_page = -1;	 	/* force a redraw regardless */
	show_headers();

	if (mini_menu)
	  show_alias_menu();

	show_last_error();

	if (hp_terminal)
	  define_softkeys(MAIN);
}

alias_title()
{
	/** display a new title line, due to re-sync'ing the aliases **/
	/* Stolen from update_title() */

	char buffer[SLEN];

	if (selected)
	  MCsprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesSelect,
	      "Alias mode: Limited Display with %d shown out of %d [ELM %s]"),
	      selected, message_count, version_buff);
	else if (message_count == 1)
	  sprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesSingle,
	      "Alias mode: 1 alias [ELM %s]"), version_buff);
	else
	  MCsprintf(buffer, catgets(elm_msg_cat, AliasesSet, AliasesPlural,
	      "Alias mode: %d aliases [ELM %s]"), message_count, version_buff);

	ClearLine(1);

	Centerline(1, buffer);
}

show_alias_menu()
{
	/** write alias menu... **/
	/* Moved from alias.c */

	if (user_level == RANK_AMATEUR) {	/* Give less options  */
	  Centerline(LINES-7, catgets(elm_msg_cat, AliasesSet, AliasesRMenuLn1,
"You can use any of the following commands by pressing the first character;"));
	  Centerline(LINES-6, catgets(elm_msg_cat, AliasesSet, AliasesRMenuLn2,
"a)lias current message, n)ew alias, d)elete or u)ndelete an alias,"));
	  Centerline(LINES-5, catgets(elm_msg_cat, AliasesSet, AliasesRMenuLn3,
"or r)eturn to main menu.  To view an alias, press <return>."));
	  Centerline(LINES-4, catgets(elm_msg_cat, AliasesSet, AliasesRMenuLn4,
"j = move down, k = move up, ? = help"));
	}
	else {
	    Centerline(LINES-7, catgets(elm_msg_cat, AliasesSet, AliasesMenuLn1,
"Alias commands:  ?=help, <n>=set current to n, /=search pattern"));
	    Centerline(LINES-6, catgets(elm_msg_cat, AliasesSet, AliasesMenuLn2,
"a)lias current message, d)elete, e)dit aliases.text, f)ully expand alias,"));
	    Centerline(LINES-5, catgets(elm_msg_cat, AliasesSet, AliasesMenuLn3,
"l)imit display, m)ail, n)ew alias, r)eturn, t)ag, u)ndelete, or e(x)it"));
	}

}

build_alias_line(buffer, entry, message_number, highlight)
char *buffer;
struct alias_rec *entry;
int message_number, highlight;
{
	/** Build in buffer the alias header ... entry is the current
	    message entry, 'highlight' is either TRUE or FALSE,
	    and 'message_number' is the number of the message.
	**/

	char mybuffer[SLEN];

	/** Note: using 'strncpy' allows us to output as much of the
	    subject line as possible given the dimensions of the screen.
	    The key is that 'strncpy' returns a 'char *' to the string
	    that it is handing to the dummy variable!  Neat, eh? **/
	/* Stolen from build_header_line() */

	int name_width;

	/* Note that one huge sprintf() is too hard for some compilers. */

	sprintf(buffer, "%s%s%c%-3d ",
		(highlight && arrow_cursor)? "->" : "  ",
		show_status(entry->status),
		(entry->status & TAGGED?  '+' : ' '),
		message_number);

	/* Set the name display width. */
	name_width = COLUMNS-40;

	/* Put the name and associated comment in local buffer */
	if (strlen(entry->comment))
	  MCsprintf(mybuffer, "%s, %s", entry->name, entry->comment);
	else
	  sprintf(mybuffer, "%s", entry->name);

	/* complete line with name, type and alias. */
	sprintf(buffer + strlen(buffer), "%-*.*s %s %-18.18s",
		/* give max and min width parameters for 'name' */
		name_width, name_width, mybuffer,
		alias_type(entry->type),
		entry->alias);
}

char *alias_type(type)
int type;
{
	/** This routine returns a string showing the alias type,
	    'Person' or 'Group' aliases.  Additionally, a '(S)'
	    is appended if this is a system alias.
	**/

	static char mybuffer[10];
	extern char *a_group_name, *a_person_name, *a_system_flag;

	if (type & GROUP)	strcpy(mybuffer, a_group_name);
	else			strcpy(mybuffer, a_person_name);

	if (type & SYSTEM)	strcat(mybuffer, a_system_flag);
	else			strcat(mybuffer, "   ");

	return( (char *) mybuffer);
}
