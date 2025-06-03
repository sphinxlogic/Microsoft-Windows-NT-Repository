#module uaf_file_subs "X-2"


/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the routines necessary to manage the
**		authorization file.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:	18-Sep-1992
**
**  MODIFICATION HISTORY:
**
**	X-2	LMP		L. Mark Pilant,		26-FEB-1993  13:13
**		Convert to C from BLISS sources.
**
**	X-1	LMP		L. Mark Pilant,		18-Sep-1992
**		Original Version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include	descrip
#include	fab
#include	nam
#include	rab
#include	rmsdef
#include	string
#include	xab

#include	<decw$include:mrmappl>

#include	"uaf_header"

/*
**
**  FORWARD ROUTINES
**
*/

#pragma noinline (AUTHORIZE$ADD_LIST_ENTRY)
#pragma noinline (AUTHORIZE$BUILD_MASTER_LIST)
#pragma noinline (AUTHORIZE$BUILD_USER_LIST)
#pragma noinline (AUTHORIZE$CRE_USER_INFO)
#pragma noinline (AUTHORIZE$DEL_LIST_ENTRY)
#pragma noinline (AUTHORIZE$DEL_USER_INFO)
#pragma noinline (AUTHORIZE$FILE_PROCESS)
#pragma noinline (AUTHORIZE$FIND_USER_ENTRY)
#pragma noinline (AUTHORIZE$GET_USER_INFO)
#pragma noinline (AUTHORIZE$OPEN_UAF)
#pragma noinline (AUTHORIZE$RELEASE_UAF_RECORD)
#pragma noinline (AUTHORIZE$SET_USER_INFO)
#pragma noinline (AUTHORIZE$VALIDATE_USER_NAME)

/* RMS structure storage, used across several routines. */

static char		sysuaf_esn[NAM$C_MAXRSS];	/* Expanded name storage */
static struct FAB	sysuaf_fab;			/* Authorization file FAB */
static struct XABKEY	sysuaf_key0;			/* Authorization file primary lookup key (username) */
static struct XABKEY	sysuaf_key1;			/* Authorization file secondary lookup key (UIC) */
static struct XABKEY	sysuaf_key2;			/* Authorization file unused lookup key (extended UIC) */
static struct XABKEY	sysuaf_key3;			/* Authorization file unused lookup key (parent ID) */
static struct NAM	sysuaf_nam;			/* Authorization file NAMe block */
static struct RAB	sysuaf_rab;			/* Authorization file RAB */
static struct UAFDEF	sysuaf_record;			/* Authorization file record storage */
static char		sysuaf_rsn[NAM$C_MAXRSS];	/* Resultant name storage */
static struct XABPRO	sysuaf_xabpro;			/* Authorization file protection XAB; no world access */


extern void AUTHORIZE$ADD_LIST_ENTRY (username, uic, list_head, entry)

char			*username;
int			uic;
struct USRLSTDEF	*list_head;
struct USRLSTDEF	**entry;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine allocates a user list entry, fills it and adds it to the
**	list whose list head is supplied.
**
**	NOTE:	The new list entry is added to the list in alphabetical order.
**		(With an optimization to see if the supplied "list head" is already
**		positioned to the correct place.)  This only affects the in memory
**		list.  It DOES NOT affect the list in the main window.
**
**  FORMAL PARAMETERS:
**
**	USERNAME	- pointer to the username text
**	UIC		- (binary) UIC for new entry
**	LIST_HEAD	- list head for the list to which the entry will be added
**	ENTRY		- pointer to the entry just added
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines. */

extern unsigned int		AUTHORIZE$FIND_USER_ENTRY ();

/* Global storage. */

globalref struct USRLSTDEF	uaf_r_master_list;		/* Complete user list (from the file) */

/* Local storage. */

struct USRLSTDEF		*list_entry;			/* Current user list entry */
struct USRLSTDEF		*new_entry;			/* Pointer to new (created) entry */
struct USRLSTDEF		*next_entry;			/* Pointer to entry after queue head */
int				simple_uic_desc[2];		/* Simple descriptor for octal UIC */
int				simple_uic_id_desc[2];		/* Simple descriptor for identifier UIC */
char				simple_uic_text[sizeof uaf_r_master_list.uaf$t_usrlst_uic];	/* Octal UIC (text) storage */
char				simple_uic_id_text[sizeof uaf_r_master_list.uaf$t_usrlst_uic_id];	/* Identifer UIC storage */
unsigned int			status;				/* Routine exit status */

$DESCRIPTOR			(fao_uic, "!%U");		/* Format UIC with octal group and member */
$DESCRIPTOR			(fao_uic_id, "!%I");		/* Format UIC with identifiers */

/* Initialize needed storage. */

simple_uic_desc[0] = sizeof simple_uic_text;
simple_uic_desc[1] = &simple_uic_text;
simple_uic_id_desc[0] = sizeof simple_uic_id_text;
simple_uic_id_desc[1] = &simple_uic_id_text;

/* Allocate and clear the storage for the list entry. */

new_entry = calloc (1, uaf$c_usrlst_length);
if (new_entry == 0) LIB$SIGNAL (UAF$_NOPROCMEM);

/* Fill the newly allocated list entry. */

memmove (&new_entry->uaf$t_usrlst_name, username, strlen (username));

if (uic != 0)
    {
    new_entry->uaf$l_usrlst_own_id = uic;			/* Save binary UIC for later */

/* Convert the binary UIC value in the authorization record to two strings.
** The first is a straight numeric (to octal) conversion.  The second uses
** any identifiers available.  These are needed for UIC based filtering. */

    simple_uic_desc[0] = sizeof simple_uic_text;		/* Reset length */
    memset (simple_uic_text, 0, sizeof simple_uic_text);	/* Reset contents */
    simple_uic_id_desc[0] = sizeof simple_uic_id_text;		/* Reset length */
    memset (simple_uic_id_text, 0, sizeof simple_uic_id_text);	/* Reset contents */

    SYS$FAO (&fao_uic,
	     simple_uic_desc,
	     simple_uic_desc,
	     uic);
    SYS$FAO (&fao_uic_id,
	     simple_uic_id_desc,
	     simple_uic_id_desc,
	     uic);

    memmove (&new_entry->uaf$t_usrlst_uic, simple_uic_text, strlen (simple_uic_text));
    memmove (&new_entry->uaf$t_usrlst_uic_id, simple_uic_id_text, strlen (simple_uic_id_text));
    }

/* Now add the new entry to the list.  As a performance optimization,
** check to see if the "list head" is positioned such that the new entry
** can be added immediately after it.  If this is not the case, chase
** down the entire list to find the correct place to insert the new entry. */

list_entry = &list_head->uaf$l_usrlst_flink;	/* Get pointer to current entry */
next_entry = list_head->uaf$l_usrlst_flink;	/* Get pointer to next entry */

if (((strlen (list_entry->uaf$t_usrlst_name) != 0) &&			/* List entry name length != 0 */
     (strcmp (list_entry->uaf$t_usrlst_name, username) > 0)) ||		/* List entry name > new entry name */
    ((strlen (next_entry->uaf$t_usrlst_name) != 0) &&			/* Next entry name length != 0 */
     (strcmp (next_entry->uaf$t_usrlst_name, username) < 0)))		/* Next entry name < new entry name */
    {

/* Search the list to find the appropriate spot for insertion. */

    AUTHORIZE$FIND_USER_ENTRY (list_head,
			       username,
			       FIND_NAME_LESS,
			       0,
			       &list_entry);

/* Now get the address of the next entry. */

    next_entry = list_entry->uaf$l_usrlst_flink;
    }

/* Add the entry after the queue list entry.  This could have been done
** with an INSQUE instruction, but I didn't want to add explicit architecture
** dependencies. */

list_entry->uaf$l_usrlst_flink = &new_entry->uaf$l_usrlst_flink;	/* Forward link in list entry points to new entry */
next_entry->uaf$l_usrlst_blink = &new_entry->uaf$l_usrlst_flink;	/* Back link in next entry points to new entry */

new_entry->uaf$l_usrlst_flink = &next_entry->uaf$l_usrlst_flink;	/* Forward link points to old next entry */
new_entry->uaf$l_usrlst_blink = &list_entry->uaf$l_usrlst_flink;	/* Back link points to list entry */

/* Return a pointer to the newly created list entry. */

*entry = new_entry;
}

extern unsigned int AUTHORIZE$BUILD_MASTER_LIST ()
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine reads the authorization file and builds the master user
**	list.  This list is then used to build the user and group lists, using
**	the appropriate wildcard criteria.  This means that it is not necessary
**	to read every record in the file when the wildcard criteria is changed.
**	However, it does mean the list can become stale.
**
**  FORMAL PARAMETERS:
**
**	None
**
**  RETURN VALUE:
**
**	SS$_NORMAL if successful, error code otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines. */

extern void			AUTHORIZE$ADD_LIST_ENTRY ();
extern unsigned int		AUTHORIZE$VALIDATE_USER_NAME ();

/* Global storage. */

globalref struct USRLSTDEF	uaf_r_master_list;			/* Complete user list (from the file) */

/* Local storage. */

struct USRLSTDEF		*current_entry;				/* Current list entry */
int				index;					/* General index */
struct USRLSTDEF		*new_entry;				/* User list entry just added */
struct USRLSTDEF		*next_entry;				/* Next list entry */
struct USRLSTDEF		*prev_entry;				/* Previous list entry */
unsigned int			status;					/* Routine exit status */
char				username[MAX_USERNAME];			/* Username text storage */

/* Since a new master list is being built, clear out any existing master
** list entries. */

while (uaf_r_master_list.uaf$l_usrlst_flink != &uaf_r_master_list.uaf$l_usrlst_flink)
    {

/* Remove the first entry.  This could have been done with a REMQUE instruction,
** but I didn't want to add explicit architecture dependencies. */

    current_entry = uaf_r_master_list.uaf$l_usrlst_flink;
    prev_entry = current_entry->uaf$l_usrlst_blink;
    next_entry = current_entry->uaf$l_usrlst_flink;

    prev_entry->uaf$l_usrlst_flink = &next_entry->uaf$l_usrlst_flink;
    next_entry->uaf$l_usrlst_blink = &prev_entry->uaf$l_usrlst_flink;

    free (current_entry);
    }

/* Position the currently opened authorization file back to the beginning. */

CHECK_RETURN (SYS$REWIND (&sysuaf_rab));

/* Read each record in the authorization file and build the master user
** list.  The user list is simply an entry for each user record in the
** authorization file. */

sysuaf_rab.rab$l_ubf = &sysuaf_record;
sysuaf_rab.rab$w_usz = sizeof sysuaf_record;
sysuaf_rab.rab$b_rac = RAB$C_SEQ;

do
    {

/* Check the status of the $GET.  Only upon success do we actually process
** the record. */

    if (TRUE (status = SYS$GET (&sysuaf_rab)))
	{

/* Since the $GET succeeded, release the record so that it does not remain
** locked. */

	SYS$RELEASE (&sysuaf_rab);

/* Since C understands ASCIZ strings better than blank filled strings or
** character descriptors, convert the blank filled username from the authorization
** record to an ASCIZ string in a local variable. */

	memmove (username, &sysuaf_record.UAF$r_fill_0.UAF$T_USERNAME, sizeof username);	/* Copy the text */
	for (index = sizeof username - 1; index >= 0; --index)
	    if (username[index] != ' ') break;				/* Exit on first non-blank character */
	memset (&username[index + 1], 0, sizeof username - index - 1);	/* Zero fill remainder */

/* Becuse there are "username" entries in the UAF which are not for real users
** (notably the system password entry), validate the syntax of the entry.  If
** the syntax is valid, add it to the appropriate lists.  Otherwise, simply
** ignore it.  (This assumes the syntax of the name is checked prior to the
** creation of the new UAF entry. */

	if (TRUE (AUTHORIZE$VALIDATE_USER_NAME (username,
						0)))

/* Add the new entry to the master user list. */

	    AUTHORIZE$ADD_LIST_ENTRY (username,
				      sysuaf_record.UAF$r_fill_2.UAF$L_UIC,
				      uaf_r_master_list.uaf$l_usrlst_blink,	/* To the end */
				      &new_entry);
	}

/* If the $GET failed, check the failure status.  Certain errors can be
** considered benign, and are ignored.  Others will terminate processing. */

    if ((status == RMS$_RLK) ||		/* Record locked */
	(status == RMS$_RNF) ||		/* Record not found */
	(status == RMS$_RTB))		/* record too big for buffer */
	status = RMS$_NORMAL;
    }
while (TRUE (status));

/* Return success. */

return SS$_NORMAL;
}

extern void AUTHORIZE$BUILD_USER_LIST ()
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine reads the authorization file and builds two lists of usernames.
**	The first list is contains all the usernames in the authorization file.  The
**	second list contains only those usernames representing groups (i.e., the
**	member portion of the UIC is 177777 [octal]).  These lists are then used for
**	processing.  This is to allow the use of $GETUAI and $SETUAI which do not
**	currently understand how to perform wildcard operations.
**
**  FORMAL PARAMETERS:
**
**	None
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines. */

extern void			AUTHORIZE$ADD_LIST_ENTRY ();
extern unsigned int		AUTHORIZE$VALIDATE_USER_NAME ();
extern unsigned int		AUTHORIZE$MATCH_NAME ();

/* The following storage is defined first, to allow storage following to
** be correctly sized. */

globalref struct USRLSTDEF	uaf_r_group_list;			/* Group user name list head */
globalref struct USRLSTDEF	uaf_r_master_list;			/* Complete user list (from the file) */
globalref struct USRLSTDEF	uaf_r_user_list;			/* User name list head */

/* Remaining global storage. */

globalref unsigned int		uaf_l_current_group_max;		/* Count of entries in group list */
globalref unsigned int		uaf_l_current_user_max;			/* Count of entries in user list */
globalref Widget		uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */
globalref char			*uaf_t_default_username;		/* Default account username */
globalref char			uaf_t_username_filter[sizeof uaf_r_user_list.uaf$t_usrlst_name];    /* Current username filter string */

/* Local storage. */

struct USRLSTDEF		*current_entry;				/* Current list entry */
int				index;					/* General index */
struct USRLSTDEF		*master_entry;				/* Master user list entry */
XmString			master_entry_username;			/* Compound string for username from master list */
struct USRLSTDEF		*new_entry;				/* User list entry just added */
struct USRLSTDEF		*next_entry;				/* Next list entry */
struct USRLSTDEF		*prev_entry;				/* Previous list entry */
unsigned int			status;					/* Routine exit status */
char				username[MAX_USERNAME];			/* Username text storage */

/* Since a new user list is being built, clear out any existing user list
** entries; on both user lists. */

while (uaf_r_user_list.uaf$l_usrlst_flink != &uaf_r_user_list.uaf$l_usrlst_flink)
    {

/* Remove the first entry.  This could have been done with a REMQUE instruction,
** but I didn't want to add explicit architecture dependencies. */

    current_entry = uaf_r_user_list.uaf$l_usrlst_flink;
    prev_entry = current_entry->uaf$l_usrlst_blink;
    next_entry = current_entry->uaf$l_usrlst_flink;

    prev_entry->uaf$l_usrlst_flink = &next_entry->uaf$l_usrlst_flink;
    next_entry->uaf$l_usrlst_blink = &prev_entry->uaf$l_usrlst_flink;

    free (current_entry);
    }

while (uaf_r_group_list.uaf$l_usrlst_flink != &uaf_r_group_list.uaf$l_usrlst_flink)
    {

/* Remove the first entry.  This could have been done with a REMQUE instruction,
** but I didn't want to add explicit architecture dependencies. */

    current_entry = uaf_r_group_list.uaf$l_usrlst_flink;
    prev_entry = current_entry->uaf$l_usrlst_blink;
    next_entry = current_entry->uaf$l_usrlst_flink;

    prev_entry->uaf$l_usrlst_flink = &next_entry->uaf$l_usrlst_flink;
    next_entry->uaf$l_usrlst_blink = &prev_entry->uaf$l_usrlst_flink;

    free (current_entry);
    }

/* Delete all the items from the username and group name list boxes. */

XmListDeleteAllItems (uaf_r_widget_id_array[uaf$c_main_user_list]);
XmListDeleteAllItems (uaf_r_widget_id_array[uaf$c_main_group_list]);

/* Traverse the master user list looking for candidates for the user and
** group lists. */

master_entry = uaf_r_master_list.uaf$l_usrlst_flink;

while (master_entry != &uaf_r_master_list.uaf$l_usrlst_flink)
    {
    master_entry_username = XmStringCreateSimple (master_entry->uaf$t_usrlst_name);

/* Check each entry in the master user list and build the user and group
** lists.  The user list is simply an entry for each user record in the
** authorization file.  The group list is an entry for each record in the
** authorization file that has a member number of -1 (177777 octal).
**
** See if the entry matches the filter criteria.  Filtering is done in one
** of two ways:
**
**  1)	If the first character of the filter string is not a square bracket
**	or angle bracket, compare the supplied string with the username field.
**
**  2)	If the first character of the filter string is a suqare bracket or
**	an angle bracket, compare the supplied string with the coverted UIC
**	(octal form as well as the identifier form).
**
** Note this is only done for the username list.  The filter does not apply
** for candidates for the group list */

    if (
	(											/* First case */
	 ((uaf_t_username_filter[0] != '[') && (uaf_t_username_filter[0] != '<')) &&
	 (TRUE (AUTHORIZE$MATCH_NAME (strlen (master_entry->uaf$t_usrlst_name),
				      master_entry->uaf$t_usrlst_name,
				      strlen (uaf_t_username_filter),
				      uaf_t_username_filter)))
	) ||
	(											/* Second case */
	 ((uaf_t_username_filter[0] == '[') || (uaf_t_username_filter[0] == '<')) &&
	 (
	  ((TRUE (AUTHORIZE$MATCH_NAME (strlen (master_entry->uaf$t_usrlst_uic),
					master_entry->uaf$t_usrlst_uic,
					strlen (uaf_t_username_filter),
					uaf_t_username_filter))) ||
	   (TRUE (AUTHORIZE$MATCH_NAME (strlen (master_entry->uaf$t_usrlst_uic_id),
					master_entry->uaf$t_usrlst_uic_id,
					strlen (uaf_t_username_filter),
					uaf_t_username_filter)))
	  )
	 )
	)
       )
	{

/* Add the new entry to the username list. */

	AUTHORIZE$ADD_LIST_ENTRY (master_entry->uaf$t_usrlst_name,
				  0,
				  uaf_r_user_list.uaf$l_usrlst_blink,	/* To the end */
				  &new_entry);

/* Add the entry to the user list box. */

	XmListAddItem (uaf_r_widget_id_array[uaf$c_main_user_list],
		       master_entry_username,
		       0);
	++uaf_l_current_user_max;
	}

/* If this entry is a group entry, add it to the group name queue as well. */

    if (sysuaf_record.UAF$r_fill_2.UAF$r_fill_3.UAF$W_MEM == 0xffff)
	{

/* Add the new entry to the group list. */

	AUTHORIZE$ADD_LIST_ENTRY (master_entry->uaf$t_usrlst_name,
				  0,
				  uaf_r_group_list.uaf$l_usrlst_blink,	/* To the end */
				  &new_entry);

/* Add the entry to the group list box. */

	XmListAddItem (uaf_r_widget_id_array[uaf$c_main_group_list],
		       master_entry_username,
		       0);
	++uaf_l_current_group_max;
	}

/* In this entry is the DEFAULT entry, add it to the group queue and set it
** up as the default selected item. */

    if (strcmp (uaf_t_default_username, master_entry->uaf$t_usrlst_name) == 0)
	{

/* Add the new entry to the group list. */

	AUTHORIZE$ADD_LIST_ENTRY (master_entry->uaf$t_usrlst_name,
				  0,
				  uaf_r_group_list.uaf$l_usrlst_blink,	/* To the end */
				  &new_entry);

/* Add the entry to the group list box. */

	XmListAddItem (uaf_r_widget_id_array[uaf$c_main_group_list],
		       master_entry_username,
		       0);
	++uaf_l_current_group_max;

/* Select and highlight the DEFAULT item. */

	AUTHORIZE$SET_UP_GROUP (master_entry->uaf$t_usrlst_name,
				NULL);
	}
    XtFree (master_entry_username);

/* Set up for the next entry in the master user list. */

    master_entry = master_entry->uaf$l_usrlst_flink;
    }
}

extern unsigned int AUTHORIZE$CRE_USER_INFO (username, itmlst)

char			*username;
struct ITMDEF		(*itmlst)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine creates the user authorization entry for the specified
**	user, using the information supplied.
**
**  FORMAL PARAMETERS:
**
**	USERNAME	- pointer to the username text
**	ITMLST		- pointer to the dynamic item list
**
**  RETURN VALUE:
**
**	Status from $CREUAI
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Global storage. */

globalref unsigned short int	uaf_w_uai_context[2];		/* $xxxUAI context cell */

/* Local storage. */

unsigned int			username_desc[2];		/* Simple username descriptor */

/* Since $CREUAI wants the username as a descriptor, set it up. */

username_desc[0] = strlen (username);
username_desc[1] = username;

/* Create the specified user authorization entry. */

return SYS$CREUAI (NULL,
#ifdef NONSHARED_UAI_CONTEXT
		   NULL,
#else
		   uaf_w_uai_context,
#endif /* NONSHARED_UAI_CONTEXT */
		   username_desc,
		   itmlst,
		   NULL,
		   NULL,
		   NULL);
}

extern void AUTHORIZE$DEL_LIST_ENTRY (list_entry)

struct USRLSTDEF	*list_entry;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine removes user list entry and deallocates the
**	associated memory.  This could have been done with a REMQUE
**	instruction, but I didn't want to add explicit architecture
**	dependencies.
**
**  FORMAL PARAMETERS:
**
**	LIST_ENTRY	- list entry to delete
**
**  RETURN VALUE:
**
**	None.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Local storage. */

struct USRLSTDEF	*next_entry;		/* Pointer to entry after queue head */
struct USRLSTDEF	*prev_entry;		/* Pointer to entry before the queue head */

/* Get pointers to the previous and next entries. */

prev_entry = list_entry->uaf$l_usrlst_blink;	/* Get pointer to previous entry */
next_entry = list_entry->uaf$l_usrlst_flink;	/* Get pointer to next entry */

/* Adjust the link pointers to eliminate the list head entry. */

prev_entry->uaf$l_usrlst_flink = &next_entry->uaf$l_usrlst_flink;	/* Forward link points to old next entry */
next_entry->uaf$l_usrlst_blink = &prev_entry->uaf$l_usrlst_flink;	/* Back link points to old previous entry */

/* Return the memory from the list head entry. */

free (list_entry);
}

extern unsigned int AUTHORIZE$DEL_USER_INFO (username)

char			*username;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine deletes the user authorization entry for the specified
**	user.
**
**  FORMAL PARAMETERS:
**
**	USERNAME	- pointer to the username text
**
**  RETURN VALUE:
**
**	Status from $DELUAI
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Global storage. */

globalref unsigned short int	uaf_w_uai_context[2];		/* $xxxUAI context cell */

/* Local storage. */

unsigned int			username_desc[2];		/* Simple username descriptor */

/* Since $DELUAI wants the username as a descriptor, set it up. */

username_desc[0] = strlen (username);
username_desc[1] = username;

/* Create the specified user authorization entry. */

return SYS$DELUAI (NULL,
#ifdef NONSHARED_UAI_CONTEXT
		   NULL,
#else
		   uaf_w_uai_context,
#endif /* NONSHARED_UAI_CONTEXT */
		   username_desc,
		   NULL,
		   NULL,
		   NULL,
		   NULL);
}

extern void AUTHORIZE$FILE_PROCESS (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine process the various actions for the file selection
**	box widget.
**
**  FORMAL PARAMETERS:
**
**	LIST_HEAD	- list head for the list to which the entry will be added
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines. */

extern unsigned int	AUTHORIZE$BUILD_MASTER_LIST ();
extern void		AUTHORIZE$BUILD_USER_LIST ();
extern void		AUTHORIZE$DECW_ERROR ();
extern unsigned int	AUTHORIZE$OPEN_UAF ();
extern void		AUTHORIZE$SET_UP_USERNAME ();
extern void		AUTHORIZE$WATCH_CURSOR_ON ();
extern void		AUTHORIZE$WATCH_CURSOR_OFF ();

/* Global references. */

globalref Widget	uaf_r_file_window_widget;	/* File selection widget info */
globalref Widget	uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget	uaf_r_main_window_widget;	/* Main window widget info */
globalref Cursor	uaf_r_wait_cursor_main;		/* Main window wait cursor id */

/* Local storage. */

Widget			file_selection_box_text;	/* File selection box text widget id */
unsigned int		status;				/* Routine exit status */
char			*temp_string;			/* Temp string pointer */
int			widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("file process\n");
#endif /* DEBUGGING */

/* Call the appropriate routine, based upon the requested action. */

switch (widget_number)
    {
    case uaf$c_file_open_file:
	{

/* Get the name of the new file to open. */

	file_selection_box_text = XmFileSelectionBoxGetChild (uaf_r_file_window_widget,
							      XmDIALOG_TEXT);
	temp_string = XmTextGetString (file_selection_box_text);

/* Open the new authorization file. */

	if (FALSE (AUTHORIZE$OPEN_UAF (temp_string)))
	    {

/* If the open fails, note the error. */

	    AUTHORIZE$DECW_ERROR (UAF$_UAFOPENERR);
	    return;
	    }

/* Dismiss the file selection box. */

	XtUnmanageChild (uaf_r_file_window_widget);
	XFlush (XtDisplay (uaf_r_file_window_widget));	/* Now */

/* Since building the user list may take some time, set up the wait cursor. */

	AUTHORIZE$WATCH_CURSOR_ON (uaf_r_main_top_level_widget,
				   uaf_r_main_window_widget,
				   uaf_r_wait_cursor_main);

/* Get a list of the user's and groups. */

	if (FALSE (AUTHORIZE$BUILD_MASTER_LIST ()))
	    {

/* If the build fails, not the failure. */

	    AUTHORIZE$DECW_ERROR (UAF$_UAFREADERR);
	    return;
	    }
	AUTHORIZE$BUILD_USER_LIST ();

/* If possible, set up for the first username in the list. */

	AUTHORIZE$SET_UP_USERNAME (0,
				   1);

/* Reset the cursor. */

	AUTHORIZE$WATCH_CURSOR_OFF (uaf_r_main_top_level_widget,
				    uaf_r_main_window_widget);

/* Free up any storage allocated for the text string. */

	XtFree (temp_string);
	break;
	}

    case uaf$c_file_cancel:
	{
	XtUnmanageChild (uaf_r_file_window_widget);
	break;
	}
    }
}

extern unsigned int AUTHORIZE$FIND_USER_ENTRY (list_head, username, match_type, position, list_entry)

struct USRLSTDEF	*list_head;
char			*username;
int			match_type;
int			**position;
struct USRLSTDEF	**list_entry;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine checks the specified list and locates an entry that
**	meets the specified match criteria.
**
**	Note: There is an implicit assumption that the candidate entry
**	      will not exist in the specified list.
**
**  FORMAL PARAMETERS:
**
**	LIST_HEAD	- list head for the list to search
**	USERNAME	- pointer to the (candidate) username text
**	MATCH_TYPE	- type code that determines name matching criteria
**			  (less, equal, or greater)
**	POSITION	- address of a cell to contain the list position
**
**  RETURN VALUE:
**
**	1 if found, 0 otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Local storage. */

struct USRLSTDEF	*entry;			/* Current user list entry */
int			found_entry;		/* Flag to indicate entry found */
int			list_position = 1;	/* Position in the list; NOTE 1 bias */

/* Loop through each entry in the list looking for the specified username. */

entry = list_head->uaf$l_usrlst_flink;		/* Get the first entry */

while (entry != &list_head->uaf$l_usrlst_flink)
    {
    found_entry = 0;				/* Assume entry not found */

/* Do the check based upon the type of match requested. */

    switch (match_type)
	{
	case FIND_NAME_LESS:
	    {

/* Since we are looking for the target entry (just) less than the
** candidate entry, it is necessary to find the first entry (just)
** greater than the candidate entry, and position to the previous
** entry.  So the logic is similar to the FIND_NAME_GREATER case;
** but with a twist. */

	    if (entry->uaf$t_usrlst_name[0] >= username[0])				/* Check first character */
		{
		if (strcmp (&entry->uaf$t_usrlst_name, username) >= 0)			/* Then entire string */
		    {
		    found_entry = 1;							/* entry > username */
		    entry = entry->uaf$l_usrlst_blink;					/* Point to previous */
		    }
		}
	    break;
	    }

	case FIND_NAME_EQUAL:
	    {

/* If a check for equality is being made, a performance boost can be made
** by comparing the first byte of each string.  If they do not match, then
** there is no point comparing the entire string.  If they do match, then
** compare the entire strings. */

	    if (entry->uaf$t_usrlst_name[0] == username[0])				/* Check first character */
		{
		if (strcmp (&entry->uaf$t_usrlst_name, username) == 0)			/* Then entire string */
		    found_entry = 1;							/* username = entry*/
		}
	    break;
	    }

	case FIND_NAME_GREATER:
	    {

/* Since we are looking for the target entry (just) greater than the
** candidate entry, all target entries whose first byte is less than
** the first byte of the candidate entry can be excluded from the full
** comparison. */

	    if (entry->uaf$t_usrlst_name[0] >= username[0])				/* Check first character */
		{
		if (strcmp (&entry->uaf$t_usrlst_name, username) > 0)			/* Then entire string */
		    found_entry = 1;							/* entry > username */
		}
	    break;
	    }
	}

/* If the entry was found, post any information requested and return. */

    if (found_entry != 0)
	{
	if (position != 0) *position = list_position;
	if (list_entry != 0) *list_entry = entry;
	return 1;
	}

/* Otherwise try the next entry; and up the list position. */

    entry = entry->uaf$l_usrlst_flink;
    ++list_position;
    }

/* At this point, all the entries have been checked.  Since nothing was
** found, the entry "found" is the list head, and the position is the
** first position in the list. */

if (position != 0) *position = 1;
if (list_entry != 0) *list_entry = &list_head->uaf$l_usrlst_flink;

/* Return a failure. */

return 0;
}

extern unsigned int AUTHORIZE$GET_USER_INFO (username, itmlst)

char			*username;
struct ITMDEF		(**itmlst)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine obtains the user authorization information for the
**	specified user.  The information is returned in the form of a
**	dynamic item list.
**
**  FORMAL PARAMETERS:
**
**	USERNAME	- pointer to the username text
**	ITMLST		- address of a cell to contain the pointer to the
**			  dynamic item list
**
**  RETURN VALUE:
**
**	Status from $GETUAI
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines. */

extern void			AUTHORIZE$ITMLST_COPY ();
extern unsigned int		AUTHORIZE$RELEASE_UAF_RECORD ();

/* Global storage. */

globalref struct ITMDEF		uaf_r_user_auth_info[];	/* $xxxUAI item list */
globalref unsigned short int	uaf_w_uai_context[2];	/* $xxxUAI context cell */

/* Local storage. */

unsigned int			status;			/* Routine exit status */
unsigned int			username_desc[2];	/* Simple username descriptor */

/* Since $GETUAI wants the username as a descriptor, set it up. */

username_desc[0] = strlen (username);
username_desc[1] = username;

/* Get the information for the specified user.  If the NONSHARED_UAI_CONTEXT
** symbol is defined, the UAF is not kept open across $xxxUAI calls.
** This means when information is needed, the UAF must be opened and
** closed for the request (internal to $GETUAI). */

if (TRUE (status = SYS$GETUAI (NULL,
#ifdef NONSHARED_UAI_CONTEXT
			       NULL,
#else
			       uaf_w_uai_context,
#endif /* NONSHARED_UAI_CONTEXT */
			       username_desc,
			       uaf_r_user_auth_info,
			       NULL,
			       NULL,
			       NULL)))
    {

/* Release the RMS record lock. */

    AUTHORIZE$RELEASE_UAF_RECORD ();

/* If there is already a dynamic item list, delete it. */

    if (*itmlst != 0) AUTHORIZE$ITMLST_DELETE (itmlst);

/* Copy the information from the local item list to a dynamic item list. */

    AUTHORIZE$ITMLST_COPY (uaf_r_user_auth_info,
			   itmlst,
			   0);
    }

/* Return with the $GETUAI status. */

return status;
}

extern unsigned int AUTHORIZE$OPEN_UAF (uaf_file)

char			*uaf_file;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine opens the existing user authorization file specified
**	by the caller.  If an authorization file has been breviously opened,
**	it is closed.
**
**  FORMAL PARAMETERS:
**
**	UAF_FILE	- Pointer to the authorization file specification
**
**  RETURN VALUE:
**
**	SS$_NORMAL if successful, error code otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Global references. */

globalref char			*default_sysuaf_file_name;	/* Authorization file default file name */
globalref char			*default_sysuaf_file_type;	/* Authorization file default file type */
globalref char			*default_sysuaf_file_spec;	/* Authorization filed default spec */
globalref unsigned short int	uaf_w_uai_context[2];		/* $xxxUAI context cell */

/* Local storage. */

unsigned int			status;				/* Routine exit status */

/* If there is an open authorization file, as indicated by the uaf_w_uai_context cell
** being something other than -1, close it now. */

if (uaf_w_uai_context[0] != 0xFFFF)
    {

/* Initialize the RMS data structures with the information needed to close
** the open user authorization file. */

    memset (&sysuaf_fab, 0, sizeof sysuaf_fab);
    memset (&sysuaf_rab, 0, sizeof sysuaf_rab);

    sysuaf_fab.fab$b_bid = FAB$C_BID;
    sysuaf_fab.fab$b_bln = FAB$C_BLN;
    sysuaf_fab.fab$w_ifi = uaf_w_uai_context[0];

    sysuaf_rab.rab$b_bid = RAB$C_BID;
    sysuaf_rab.rab$b_bln = RAB$C_BLN;
    sysuaf_rab.rab$w_isi = uaf_w_uai_context[1];

    SYS$DISCONNECT (&sysuaf_rab);
    SYS$CLOSE (&sysuaf_rab);
    }

/* Initialize the RMS data structures needed to open the user authorization file. */

/* Unused lookup key - UAF$Q_PARENT_ID. */

sysuaf_key3 = cc$rms_xabkey;			/* Prototype information */
sysuaf_key3.xab$b_ref = 3;									/* Key of reference */
sysuaf_key3.xab$w_pos0 = (int)(&sysuaf_record.UAF$Q_PARENT_ID) - (int)(&sysuaf_record);		/* Position in record */
sysuaf_key3.xab$b_siz0 = sizeof sysuaf_record.UAF$Q_PARENT_ID;					/* Field size */
sysuaf_key3.xab$b_dtp = XAB$C_BN8;								/* Data type */
sysuaf_key3.xab$b_flg = XAB$M_CHG | XAB$M_DUP;							/* Key flags */

/* Unused lookup key - UAF$L_UIC and UAF$L_SUB_ID (assumed to be adjacent
** fields in the UAF record. */

sysuaf_key2 = cc$rms_xabkey;			/* Prototype information */
sysuaf_key2.xab$b_ref = 2;									/* Key of reference */
sysuaf_key2.xab$w_pos0 = (int)(&sysuaf_record.UAF$r_fill_2.UAF$L_UIC) - (int)(&sysuaf_record);	/* Position in record */
sysuaf_key2.xab$b_siz0 = sizeof sysuaf_record.UAF$r_fill_2.UAF$L_UIC + sizeof sysuaf_record.UAF$L_SUB_ID;	/* Field size */
sysuaf_key2.xab$b_dtp = XAB$C_BN8;								/* Data type */
sysuaf_key2.xab$b_flg = XAB$M_CHG | XAB$M_DUP;							/* Key flags */
sysuaf_key2.xab$l_nxt = &sysuaf_key3;								/* Next XAB */
	
/* Secondary lookup key - UAF$L_UIC. */

sysuaf_key1 = cc$rms_xabkey;			/* Prototype information */
sysuaf_key1.xab$b_ref = 1;									/* Key of reference */
sysuaf_key1.xab$w_pos0 = (int)(&sysuaf_record.UAF$r_fill_2.UAF$L_UIC) - (int)(&sysuaf_record);	/* Position in record */
sysuaf_key1.xab$b_siz0 = sizeof sysuaf_record.UAF$r_fill_2.UAF$L_UIC;				/* Field size */
sysuaf_key1.xab$b_dtp = XAB$C_BN4;								/* Data type */
sysuaf_key1.xab$b_flg = XAB$M_CHG | XAB$M_DUP;							/* Key flags */
sysuaf_key1.xab$l_nxt = &sysuaf_key2;								/* Next XAB */

/* Primary lookup key - UAF$T_USERNAME. */

sysuaf_key0 = cc$rms_xabkey;			/* Prototype information */
sysuaf_key0.xab$b_ref = 0;									/* Key of reference */
sysuaf_key0.xab$w_pos0 = (int)(&sysuaf_record.UAF$r_fill_0.UAF$T_USERNAME) - (int)(&sysuaf_record);	/* Position in record */
sysuaf_key0.xab$b_siz0 = sizeof sysuaf_record.UAF$r_fill_0.UAF$T_USERNAME;				/* Field size */
sysuaf_key0.xab$b_dtp = XAB$C_STG;								/* Data type */
sysuaf_key0.xab$l_nxt = &sysuaf_key2;								/* Next XAB */

/* Authorization file NAMe block.

/* Since the prototyping doesn't work for the NAMe block, the block id and
** block length fields have to be set in the usual way; after clearing out
** the block. */

/* sysuaf_nam = cc$rms_nam;			/* Prototype information */

memset (&sysuaf_nam, 0, sizeof sysuaf_nam);
sysuaf_nam.nam$b_bid = NAM$C_BID;
sysuaf_nam.nam$b_bln = NAM$C_BLN;

sysuaf_nam.nam$l_esa = sysuaf_esn;		/* Expanded name string storage */
sysuaf_nam.nam$b_ess = sizeof sysuaf_esn;	/* Size of expanded name string storage */
sysuaf_nam.nam$l_rsa = sysuaf_rsn;		/* Resultant name string storage */
sysuaf_nam.nam$b_rss = sizeof sysuaf_rsn;	/* Size of resultant name string storage */

/* Authorization File Access Block. */

sysuaf_fab = cc$rms_fab;			/* Prototype information */
sysuaf_fab.fab$l_fop = FAB$M_CBT;
sysuaf_fab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_DEL | FAB$M_UPD;
sysuaf_fab.fab$b_shr = FAB$M_GET | FAB$M_PUT | FAB$M_DEL | FAB$M_UPD;
sysuaf_fab.fab$b_org = FAB$C_IDX;
sysuaf_fab.fab$b_rfm = FAB$C_VAR;
sysuaf_fab.fab$w_mrs = UAF$C_LENGTH;
sysuaf_fab.fab$l_alq = 10;
sysuaf_fab.fab$w_deq = 10;
sysuaf_fab.fab$l_nam = &sysuaf_nam;
sysuaf_fab.fab$l_xab = &sysuaf_key0;

/* If a file name was supplied, use it.  Otherwise, use the default
** file name string. */

if (strlen (uaf_file) == 0)
    {

/* If the SYSUAF_LOGICAL symbol is defined, set up the FAB to allow
** the authorization file to be accessed via the appropriate logical
** name.  Otherwise, set up the FAB to allow the authorization file
** to be accessed via a full file specification. */

#ifdef	SYSUAF_LOGICAL
    sysuaf_fab.fab$l_fna = default_sysuaf_file_name;		/* For the logical name */
    sysuaf_fab.fab$b_fns = strlen (default_sysuaf_file_name);
    sysuaf_fab.fab$l_dna = default_sysuaf_file_type;
    sysuaf_fab.fab$b_dns = strlen (default_sysuaf_file_type);
#else
    sysuaf_fab.fab$l_fna = default_sysuaf_file_spec;		/* For a file spec */
    sysuaf_fab.fab$b_fns = strlen (default_sysuaf_file_spec);
#endif	/* SYSUAF_LOGICAL */
    }
else
    {
    sysuaf_fab.fab$l_fna = uaf_file;
    sysuaf_fab.fab$b_fns = strlen (uaf_file);
    }

/* Authorization file Record Access Block. */

sysuaf_rab = cc$rms_rab;			/* Prototype information */
sysuaf_rab.rab$l_fab = &sysuaf_fab;
sysuaf_rab.rab$b_krf = 0;
sysuaf_rab.rab$l_kbf = &sysuaf_record.UAF$r_fill_0.UAF$T_USERNAME;
sysuaf_rab.rab$b_ksz = sizeof sysuaf_record.UAF$r_fill_0.UAF$T_USERNAME;
sysuaf_rab.rab$w_usz = sizeof sysuaf_record;
sysuaf_rab.rab$b_rac = RAB$C_KEY;

/* Open the authorization file, and return any errors. */

CHECK_RETURN (SYS$OPEN (&sysuaf_fab));

/* Connect up a record stream to the authorization file. */

CHECK_RETURN (SYS$CONNECT (&sysuaf_rab));

/* Save the RMS IFI and ISI for later. */

uaf_w_uai_context[0] = sysuaf_fab.fab$w_ifi;
uaf_w_uai_context[1] = sysuaf_rab.rab$w_isi;

return SS$_NORMAL;
}

extern unsigned int AUTHORIZE$RELEASE_UAF_RECORD ()

{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to release the RMS locks associated with the
**	authorization record read by $GETUAI or written by $SETUAI.
**
**  FORMAL PARAMETERS:
**
**	None.
**
**  RETURN VALUE:
**
**	SS$_NORMAL if successful, error code otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Global storage. */

globalref unsigned short int	uaf_w_uai_context[2];		/* $xxxUAI context cell */

/* Initialize the RMS data structures with the information needed to close
** the open user authorization file. */

memset (&sysuaf_rab, 0, sizeof sysuaf_rab);

sysuaf_rab.rab$b_bid = RAB$C_BID;
sysuaf_rab.rab$b_bln = RAB$C_BLN;
sysuaf_rab.rab$w_isi = uaf_w_uai_context[1];

/* Release the record. */

SYS$RELEASE (&sysuaf_rab);

return SS$_NORMAL;
}

extern unsigned int AUTHORIZE$SET_USER_INFO (username, itmlst)

char			*username;
struct ITMDEF		(*itmlst)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine modifies the user authorization information for the
**	specified user.
**
**  FORMAL PARAMETERS:
**
**	USERNAME	- pointer to the username text
**	ITMLST		- pointer to the dynamic item list
**
**  RETURN VALUE:
**
**	Status from $SETUAI
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Global storage. */

globalref unsigned short int	uaf_w_uai_context[2];		/* $xxxUAI context cell */

/* Local storage. */

unsigned int			status;				/* Routine exit status */
unsigned int			username_desc[2];		/* Simple username descriptor */

/* Since $SETUAI wants the username as a descriptor, set it up. */

username_desc[0] = strlen (username);
username_desc[1] = username;

/* Create the specified user authorization entry.  If the NONSHARED_UAI_CONTEXT
** symbol is defined, the UAF is not kept open across $xxxUAI calls.
** This means when information is needed, the UAF must be opened and
** closed for the request (internal to $GETUAI). */

status = SYS$SETUAI (NULL,
#ifdef NONSHARED_UAI_CONTEXT
		     NULL,
#else
		     uaf_w_uai_context,
#endif /* NONSHARED_UAI_CONTEXT */
		     username_desc,
		     itmlst,
		     NULL,
		     NULL,
		     NULL);

/* Release RMS record lock. */

AUTHORIZE$RELEASE_UAF_RECORD ();

/* Return with $SETUAI status. */

return status;
}

extern unsigned int AUTHORIZE$VALIDATE_USER_NAME (username, wildcard)

char			*username;
int			wildcard;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine checks the syntax of the specified username.
**
**  FORMAL PARAMETERS:
**
**	USERNAME	- pointer to the username text
**	WILDCARD	- = 0 to disallow wildcard characters as legal
**			  = 1 to allow wildcard characters
**
**  RETURN VALUE:
**
**	SS$_NORMAL if valid, 0 otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Local storage. */

char			*character_set;							/* Character set to use */
char			*valid_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$_";	/* Valid non-wildcard characters */
char			*valid_wc_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$_*%";	/* Valid characters and wildcards */

/* Set up the character set to use for the validation.  This is based upon whether
** or not wildcard characters are allowed. */

if (wildcard == 0) character_set = valid_chars;
else character_set = valid_wc_chars;

/* If all of the characters in the supplied username are in the character set,
** return a generic success status. */

if (strspn (username, character_set) == strlen (username)) return 1;

/*  Otherwise return a generic failure status. */

return 0;
}
