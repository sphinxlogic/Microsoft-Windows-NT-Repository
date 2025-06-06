/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Code for menus.  Used for the GUI and 'wildmenu'.
 */

#include "vim.h"

#ifdef WANT_MENU

#define MENUDEPTH   10		/* maximum depth of menus */

#ifdef USE_GUI_WIN32
static int add_menu_path __ARGS((char_u *, int, int *, void (*)(), char_u *, int, int));
#else
# ifdef USE_GUI
static int add_menu_path __ARGS((char_u *, int, int *, void (*)(), char_u *, int));
# else
static int add_menu_path __ARGS((char_u *, int, int *, char_u *, int));
# endif
#endif
static int remove_menu __ARGS((VimMenu **, char_u *, int, int silent));
static void free_menu __ARGS((VimMenu **menup));
static void free_menu_string __ARGS((VimMenu *, int));
static int show_menus __ARGS((char_u *, int));
static void show_menus_recursive __ARGS((VimMenu *, int, int));
static int menu_name_equal __ARGS((char_u *name, VimMenu *menu));
static int menu_namecmp __ARGS((char_u *name, char_u *mname));
static int get_menu_cmd_modes __ARGS((char_u *, int, int *, int *));
static char_u *popup_mode_name __ARGS((char_u *name, int idx));
static char_u *menu_text __ARGS((char_u *text, int *mnemonic, char_u **actext));
#ifdef USE_GUI
static int get_menu_mode __ARGS((void));
static void gui_update_menus_recurse __ARGS((VimMenu *, int));
#endif

#ifdef USE_GUI_WIN32
static void gui_create_tearoffs_recurse __ARGS((VimMenu *menu, const char_u *pname, int *pri_tab, int pri_idx));
static void gui_add_tearoff __ARGS((char_u *tearpath, int *pri_tab, int pri_idx));
static void gui_destroy_tearoffs_recurse __ARGS((VimMenu *menu));
static int s_tearoffs = FALSE;
#endif

/* The character for each menu mode */
static char_u	menu_mode_chars[] = {'n', 'v', 'o', 'i', 'c', 't'};

/*
 * Do the :menu command and relatives.
 */
    void
do_menu(eap)
    EXARG	*eap;		    /* Ex command arguments */
{
    char_u	*menu_path;
    int		modes;
    char_u	*map_to;
    int		noremap;
    int		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    int		i;
#if defined(USE_GUI) && !defined(USE_GUI_GTK)
    int		old_menu_height;
#endif
    int		pri_tab[MENUDEPTH + 1];

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    /* fill in the priority table */
    for (p = arg; *p; ++p)
	if (!isdigit(*p) && *p != '.')
	    break;
    if (vim_iswhite(*p))
    {
	for (i = 0; i < MENUDEPTH && !vim_iswhite(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    if (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    if (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    else if (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    else
	i = 0;
    while (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		/* mark end of the table */

    menu_path = arg;
    if (*menu_path == NUL)
    {
	show_menus(menu_path, modes);
	return;
    }

    /* skip the menu name, and translate <Tab> into a real TAB */
    while (*arg && !vim_iswhite(*arg))
    {
	if ((*arg == '\\' || *arg == Ctrl('V')) && arg[1] != NUL)
	    arg++;
	else if (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    mch_memmove(arg + 1, arg + 5, STRLEN(arg + 4));
	}
	arg++;
    }
    if (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);
    map_to = arg;
    if (*map_to == NUL && !unmenu)
    {
	show_menus(menu_path, modes);
	return;
    }
    else if (*map_to != NUL && unmenu)
    {
	EMSG("Trailing characters");
	return;
    }
#if defined(USE_GUI) && !defined(USE_GUI_GTK)
    old_menu_height = gui.menu_height;
#endif
    if (unmenu)
    {
	if (STRCMP(menu_path, "*") == 0)	/* meaning: remove all menus */
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	if (popup_menu(menu_path))
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		if (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    if (p != NULL)
		    {
			remove_menu(&root_menu, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	/* Careful: remove_menu() changes menu_path */
	remove_menu(&root_menu, menu_path, modes, FALSE);
    }
    else
    {
	/* Replace special key codes */
	map_to = replace_termcodes(map_to, &map_buf, FALSE, TRUE);
	add_menu_path(menu_path, modes, pri_tab,
#ifdef USE_GUI
		gui_menu_cb,
#endif
		map_to, noremap
#ifdef USE_GUI_WIN32
		, TRUE
#endif
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	if (popup_menu(menu_path))
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		if (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    if (p != NULL)
		    {
			/* Include all modes, to make ":amenu" work */
			add_menu_path(p, modes, pri_tab,
#ifdef USE_GUI
				gui_menu_cb,
#endif
				map_to, noremap
#ifdef USE_GUI_WIN32
				, TRUE
#endif
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#if defined(USE_GUI) && !defined(USE_GUI_GTK)
    /* If the menubar height changed, resize the window */
    if (gui.menu_height != old_menu_height)
	gui_set_winsize(FALSE);
#endif
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    static int
add_menu_path(menu_path, modes, pri_tab,
#ifdef USE_GUI
	call_back,
#endif
	call_data, noremap
#ifdef USE_GUI_WIN32
	, addtearoff
#endif
	)
    char_u	*menu_path;
    int		modes;
    int		*pri_tab;
#ifdef USE_GUI
    void	(*call_back)();
#endif
    char_u	*call_data;
    int		noremap;
#ifdef USE_GUI_WIN32
    int		addtearoff;	/* may add tearoff item */
#endif
{
    char_u	*path_name;
    VimMenu	**menup;
    VimMenu	*menu = NULL;
    VimMenu	*parent;
    VimMenu	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    int		i;
    int		c;
#ifdef USE_GUI
    int		idx;
    int		new_idx;
#endif
    int		pri_idx = 0;
    int		old_modes = 0;
    int		amenu;

    /* Make a copy so we can stuff around with it, since it could be const */
    path_name = vim_strsave(menu_path);
    if (path_name == NULL)
	return FAIL;
    menup = &root_menu;
    parent = NULL;
    name = path_name;
    while (*name)
    {
	/* Get name of this element in the menu hierarchy, and the simplified
	 * name (without mnemonic and accelerator text). */
	next_name = menu_name_skip(name);
	dname = menu_text(name, NULL, NULL);

	/* See if it's already there */
	lower_pri = menup;
#ifdef USE_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	while (menu != NULL)
	{
	    if (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		if (*next_name == NUL && menu->children != NULL)
		{
		    if (!sys_menu)
			EMSG("Menu path must not lead to a sub-menu");
		    goto erret;
		}
		if (*next_name != NUL && menu->children == NULL
#ifdef USE_GUI_WIN32
			&& addtearoff
#endif
			)
		{
		    if (!sys_menu)
			EMSG("Part of menu-item path is not sub-menu");
		    goto erret;
		}
		break;
	    }
	    menup = &menu->next;

	    /* Count menus, to find where this one needs to be inserted.
	     * Ignore menus that are not in the menubar (PopUp and Toolbar) */
	    if (parent != NULL || menubar_menu(menu->name))
	    {
#ifdef USE_GUI
		++idx;
#endif
		if (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef USE_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	if (menu == NULL)
	{
	    if (*next_name == NUL && parent == NULL)
	    {
		EMSG("Must not add menu items directly to menu bar");
		goto erret;
	    }

	    if (is_menu_separator(dname) && *next_name != NUL)
	    {
		EMSG("Separator cannot be part of a menu path");
		goto erret;
	    }

	    /* Not already there, so lets add it */
	    menu = (VimMenu *)alloc_clear((unsigned)sizeof(VimMenu));
	    if (menu == NULL)
		goto erret;

	    menu->modes = modes;
	    menu->name = vim_strsave(name);
	    /* separate mnemonic and accelerator text from actual menu name */
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
	    menu->priority = pri_tab[pri_idx];

	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->next = *lower_pri;
	    *lower_pri = menu;

#ifdef USE_GUI
	    if (gui.in_use)  /* Otherwise it will be added when GUI starts */
	    {
		if (*next_name == NUL)
		{
		    /* Real menu item, not sub-menu */
		    gui_mch_add_menu_item(menu, parent, new_idx);

		    /* Want to update menus now even if mode not changed */
		    force_menu_update = TRUE;
		}
		else
		{
		    /* Sub-menu (not at end of path yet) */
		    gui_mch_add_menu(menu, parent, new_idx);
		}
	    }
#endif

#ifdef USE_GUI_WIN32
	    /* When adding a new submenu, may add a tearoff item */
	    if (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != NULL
		    && menubar_menu(name))
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_path) + TEAR_LEN + 2);
		if (tearpath != NULL)
		{
		    char_u  *s;
		    int	    idx;

		    STRCPY(tearpath, menu_path);
		    idx = next_name - path_name - 1;
		    for (s = tearpath; *s && s < tearpath + idx; ++s)
			if ((*s == '\\' || *s == Ctrl('V')) && s[1])
			{
			    ++idx;
			    ++s;
			}
		    tearpath[idx] = NUL;
		    gui_add_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
#endif
	    old_modes = 0;
	}
	else
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     */
#ifdef USE_GUI_WIN32
	    /* If adding a tearbar (addtearoff == FALSE) don't update modes */
	    if (addtearoff)
#endif
		menu->modes |= modes;
	}

	menup = &menu->children;
	parent = menu;
	name = next_name;
	vim_free(dname);
	if (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    if (sys_menu)
	modes &= ~old_modes;

    if (menu != NULL && modes)
    {
#ifdef USE_GUI
	menu->cb = call_back;
#endif
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	/* loop over all modes, may add more than one */
	for (i = 0; i < MENU_MODES; ++i)
	{
	    if (modes & (1 << i))
	    {
		/* free any old menu */
		free_menu_string(menu, i);

		/* For "amenu", may insert an extra character */
		/* Don't do this if adding a tearbar (addtearoff == FALSE) */
		c = 0;
		if (amenu
#ifdef USE_GUI_WIN32
		       && addtearoff
#endif
				       )
		{
		    switch (1 << i)
		    {
			case MENU_VISUAL_MODE:
			case MENU_OP_PENDING_MODE:
			case MENU_CMDLINE_MODE:
			    c = Ctrl('C');
			    break;
			case MENU_INSERT_MODE:
			    c = Ctrl('O');
			    break;
		    }
		}

		if (c)
		{
		    menu->strings[i] = alloc((unsigned)(STRLEN(call_data) + 2));
		    if (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			STRCPY(menu->strings[i] + 1, call_data);
		    }
		}
		else
		    menu->strings[i] = p;
		menu->noremap[i] = noremap;
	    }
	}
    }
    return OK;

erret:
    vim_free(path_name);
    vim_free(dname);
    return FAIL;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static int
remove_menu(menup, name, modes, silent)
    VimMenu	**menup;
    char_u	*name;
    int		modes;
    int		silent;		/* don't give error messages */
{
    VimMenu	*menu;
    VimMenu	*child;
    char_u	*p;

    if (*menup == NULL)
	return OK;		/* Got to bottom of hierarchy */

    /* Get name of this element in the menu hierarchy */
    p = menu_name_skip(name);

    /* Find the menu */
    while ((menu = *menup) != NULL)
    {
	if (*name == NUL || menu_name_equal(name, menu))
	{
	    if (*p != NUL && menu->children == NULL)
	    {
		if (!silent)
		    EMSG("Part of menu-item path is not sub-menu");
		return FAIL;
	    }
	    if ((menu->modes & modes) != 0x0)
	    {
#ifdef USE_GUI_WIN32
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		if (*p == NUL && modes == MENU_ALL_MODES)
		{
		    if (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		if (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    return FAIL;
	    }
	    else if (*name != NUL)
	    {
		if (!silent)
		    EMSG("Menu only exists in another mode");
		return FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    if (*name != NUL)
		break;

#ifdef USE_GUI_ATHENA
	    if (((menu->modes & ~modes) & MENU_ALL_MODES) == 0)
	    {
		EMSG("Sorry, deleting a menu is not possible in the Athena version");
		return FAIL;
	    }
#endif

	    /* Remove the menu item for the given mode[s].  If the menu item
	     * is no longer valid in ANY mode, delete it */
	    menu->modes &= ~modes;
	    if (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    if ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    else
		menup = &menu->next;
	}
	else
	    menup = &menu->next;
    }
    if (*name != NUL)
    {
	if (menu == NULL)
	{
	    if (!silent)
		EMSG("No menu of that name");
	    return FAIL;
	}


	/* Recalculate modes for menu based on the new updated children */
	menu->modes &= ~modes;
#ifdef USE_GUI_WIN32
	if ((s_tearoffs) && (menu->children != NULL)) /* there's a tear bar.. */
	    child = menu->children->next; /* don't count tearoff bar */
	else
#endif
	    child = menu->children;
	for ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	if (modes & MENU_TIP_MODE)
	    free_menu_string(menu, MENU_INDEX_TIP);
	if ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    /* The menu item is no longer valid in ANY mode, so delete it */
#ifdef USE_GUI_WIN32
	    if (s_tearoffs && menu->children != NULL) /* there's a tear bar.. */
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    return OK;
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    static void
free_menu(menup)
    VimMenu **menup;
{
    int	    i;
    VimMenu *menu;

    menu = *menup;
    *menup = menu->next;

#ifdef USE_GUI
    /* Free machine specific menu structures (only when already created) */
    /* Also may rebuild a tearoff'ed menu */
    if (gui.in_use)
	gui_mch_destroy_menu(menu);
#endif
    vim_free(menu->name);
    vim_free(menu->dname);
    vim_free(menu->actext);
    for (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#ifdef USE_GUI
    /* Want to update menus now even if mode not changed */
    force_menu_update = TRUE;
#endif
}

/*
 * Free the menu->string with the given index.
 */
    static void
free_menu_string(menu, idx)
    VimMenu *menu;
    int	    idx;
{
    int	    count = 0;
    int	    i;

    for (i = 0; i < MENU_MODES; i++)
	if (menu->strings[i] == menu->strings[idx])
	    count++;
    if (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static int
show_menus(path_name, modes)
    char_u  *path_name;
    int	    modes;
{
    char_u  *p;
    char_u  *name;
    VimMenu *menu;
    VimMenu *parent = NULL;

    menu = root_menu;
    name = path_name = vim_strsave(path_name);
    if (path_name == NULL)
	return FAIL;

    /* First, find the (sub)menu with the given name */
    while (*name)
    {
	p = menu_name_skip(name);
	while (menu != NULL)
	{
	    if (menu_name_equal(name, menu))
	    {
		/* Found menu */
		if (*p != NUL && menu->children == NULL)
		{
		    EMSG("Part of menu-item path is not sub-menu");
		    vim_free(path_name);
		    return FAIL;
		}
		else if ((menu->modes & modes) == 0x0)
		{
		    EMSG("Menu only exists in another mode");
		    vim_free(path_name);
		    return FAIL;
		}
		break;
	    }
	    menu = menu->next;
	}
	if (menu == NULL)
	{
	    EMSG("No menu of that name");
	    vim_free(path_name);
	    return FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }

    /* Now we have found the matching menu, and we list the mappings */
						    /* Highlight title */
    MSG_PUTS_TITLE("\n--- Menus ---");

    show_menus_recursive(parent, modes, 0);
    return OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
show_menus_recursive(menu, modes, depth)
    VimMenu *menu;
    int	    modes;
    int	    depth;
{
    int	    i;
    int	    bit;

    if (menu != NULL && (menu->modes & modes) == 0x0)
	return;

    if (menu != NULL)
    {
	msg_putchar('\n');
	if (got_int)		/* "q" hit for "--more--" */
	    return;
	for (i = 0; i < depth; i++)
	    MSG_PUTS("  ");
	if (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    MSG_PUTS(" ");
	}
				/* Same highlighting as for directories!? */
	msg_puts_attr(menu->name, hl_attr(HLF_D));
    }

    if (menu != NULL && menu->children == NULL)
    {
	for (bit = 0; bit < MENU_MODES; bit++)
	    if ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		if (got_int)		/* "q" hit for "--more--" */
		    return;
		for (i = 0; i < depth + 2; i++)
		    MSG_PUTS("  ");
		msg_putchar(menu_mode_chars[bit]);
		if (menu->noremap[bit])
		    msg_putchar('*');
		else
		    msg_putchar(' ');
		MSG_PUTS("  ");
		msg_outtrans_special(menu->strings[bit], FALSE);
	    }
    }
    else
    {
	if (menu == NULL)
	{
	    menu = root_menu;
	    depth--;
	}
	else
	    menu = menu->children;

	/* recursively show all children.  Skip PopUp[nvoci]. */
	for (; menu != NULL && !got_int; menu = menu->next)
	    if (!popup_menu(menu->dname) || menu->dname[5] == NUL)
		show_menus_recursive(menu, modes, depth + 1);
    }
}

#ifdef CMDLINE_COMPL

/*
 * Used when expanding menu names.
 */
static VimMenu	*expand_menu = NULL;
static int	expand_modes = 0x0;

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(cmd, arg, forceit)
    char_u  *cmd;
    char_u  *arg;
    int	    forceit;
{
    char_u  *after_dot;
    char_u  *p;
    char_u  *path_name = NULL;
    char_u  *name;
    int	    unmenu;
    VimMenu *menu;

    expand_context = EXPAND_UNSUCCESSFUL;

    after_dot = arg;
    for (p = arg; *p && !vim_iswhite(*p); ++p)
    {
	if ((*p == '\\' || *p == Ctrl('V')) && p[1] != NUL)
	    p++;
	else if (*p == '.')
	    after_dot = p + 1;
    }
    if (*cmd == 'e' && vim_iswhite(*p))
	return NULL;	/* TODO: check for next command? */
    if (*p == NUL)		/* Complete the menu name */
    {
	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	if (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	if (after_dot != arg)
	{
	    path_name = alloc((unsigned)(after_dot - arg));
	    if (path_name == NULL)
		return NULL;
	    STRNCPY(path_name, arg, after_dot - arg - 1);
	    path_name[after_dot - arg - 1] = NUL;
	}
	name = path_name;
	while (name != NULL && *name)
	{
	    p = menu_name_skip(name);
	    while (menu != NULL)
	    {
		if (menu_name_equal(name, menu))
		{
		    /* Found menu */
		    if ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			return NULL;
		    }
		    break;
		}
		menu = menu->next;
	    }
	    if (menu == NULL)
	    {
		/* No menu found with the name we were looking for */
		vim_free(path_name);
		return NULL;
	    }
	    name = p;
	    menu = menu->children;
	}

	expand_context = (*cmd == 'e') ? EXPAND_MENUNAMES : EXPAND_MENUS;
	expand_pattern = after_dot;
	expand_menu = menu;
    }
    else			/* We're in the mapping part */
	expand_context = EXPAND_NOTHING;
    return NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_menu_name(idx)
    int	    idx;
{
    static VimMenu  *menu = NULL;
    static int	    get_dname = FALSE;	/* return menu->dname next time */
    char_u	    *str;

    if (idx == 0)	    /* first call: start at first item */
    {
	menu = expand_menu;
	get_dname = FALSE;
    }

    /* Skip PopUp[nvoci]. */
    while (menu != NULL && popup_menu(menu->dname) && menu->dname[5])
	menu = menu->next;

    if (menu == NULL)	    /* at end of linked list */
	return NULL;

    if (menu->modes & expand_modes)
    {
	if (get_dname)
	{
	    str = menu->dname;
	    get_dname = FALSE;
	}
	else
	{
	    str = menu->name;
	    if (STRCMP(menu->name, menu->dname))
		get_dname = TRUE;
	}
    }
    else
    {
	str = (char_u *)"";
	get_dname = FALSE;
    }

    /* Advance to next menu entry. */
    if (!get_dname)
	menu = menu->next;

    return str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_menu_names(idx)
    int	    idx;
{
    static VimMenu  *menu = NULL;
    static char_u   tbuffer[256]; /*hack*/
    char_u	    *str;

    if (idx == 0)	    /* first call: start at first item */
	menu = expand_menu;

    /* Skip Browse-style entries, popup menus and separators. */
    while (menu != NULL
	    && (   popup_menu(menu->dname)
		|| is_menu_separator(menu->dname)
#ifndef USE_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
	menu = menu->next;

    if (menu == NULL)	    /* at end of linked list */
	return NULL;

    if (menu->modes & expand_modes)
    {
	if (menu->children != NULL)
	{
	    STRCPY(tbuffer, menu->dname);
	    /* hack on menu separators:  use a 'magic' char for the separator
	     * so that '.' in names gets escaped properly */
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	else
	    str = menu->dname;
    }
    else
	str = (char_u *)"";

    /* Advance to next menu entry. */
    menu = menu->next;

    return str;
}
#endif /* CMDLINE_COMPL */

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 */
    char_u *
menu_name_skip(name)
    char_u  *name;
{
    char_u  *p;

    for (p = name; *p && *p != '.'; p++)
    {
	if (*p == '\\' || *p == Ctrl('V'))
	{
	    mch_memmove(p, p + 1, STRLEN(p));
	    if (*p == NUL)
		break;
	}
#ifdef MULTI_BYTE
	if (is_dbcs && IsLeadByte(*p) && p[1] != NUL)
	    ++p;	/* skip multibyte char */
#endif
    }
    if (*p)
	*p++ = NUL;
    return p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static int
menu_name_equal(name, menu)
    char_u	*name;
    VimMenu	*menu;
{
    return (menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname));
}

    static int
menu_namecmp(name, mname)
    char_u	*name;
    char_u	*mname;
{
    int		i;

    for (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	if (name[i] != mname[i])
	    break;
    return ((name[i] == NUL || name[i] == TAB)
	    && (mname[i] == NUL || mname[i] == TAB));
}

/*
 * Return the modes specified by the given menu command (eg :menu! returns
 * MENU_CMDLINE_MODE | MENU_INSERT_MODE).  If noremap is not NULL, then the
 * flag it points to is set according to whether the command is a "nore"
 * command.  If unmenu is not NULL, then the flag it points to is set
 * according to whether the command is an "unmenu" command.
 */
    static int
get_menu_cmd_modes(cmd, forceit, noremap, unmenu)
    char_u  *cmd;
    int	    forceit;	    /* Was there a "!" after the command? */
    int	    *noremap;
    int	    *unmenu;
{
    int	    modes;

    switch (*cmd++)
    {
	case 'v':			/* vmenu, vunmenu, vnoremenu */
	    modes = MENU_VISUAL_MODE;
	    break;
	case 'o':			/* omenu */
	    modes = MENU_OP_PENDING_MODE;
	    break;
	case 'i':			/* imenu */
	    modes = MENU_INSERT_MODE;
	    break;
	case 't':
	    modes = MENU_TIP_MODE;	/* tmenu */
	    break;
	case 'c':			/* cmenu */
	    modes = MENU_CMDLINE_MODE;
	    break;
	case 'a':			/* amenu */
	    modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_OP_PENDING_MODE;
	    break;
	case 'n':
	    if (cmd[1] != 'o')		/* nmenu */
	    {
		modes = MENU_NORMAL_MODE;
		break;
	    }
	    /* FALLTHROUGH */
	default:
	    --cmd;
	    if (forceit)		/* menu!! */
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    else			/* menu */
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE
						       | MENU_OP_PENDING_MODE;
    }

    if (noremap != NULL)
	*noremap = (*cmd == 'n');
    if (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    return modes;
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(name, idx)
    char_u	*name;
    int		idx;
{
    char_u	*p;
    int		len = STRLEN(name);

    p = vim_strnsave(name, len + 1);
    if (p != NULL)
    {
	mch_memmove(p + 6, p + 5, (size_t)(len - 4));
	p[5] = menu_mode_chars[idx];
    }
    return p;
}

/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    int
get_menu_index(menu, state)
    VimMenu *menu;
    int	    state;
{
    int	    idx;

    if (VIsual_active)
	idx = MENU_INDEX_VISUAL;
    else if ((state & INSERT))
	idx = MENU_INDEX_INSERT;
    else if ((state & CMDLINE))
	idx = MENU_INDEX_CMDLINE;
    else if (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    else if ((state & NORMAL))
	idx = MENU_INDEX_NORMAL;
    else
	idx = MENU_INDEX_INVALID;

    if (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    return idx;
}

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    static char_u *
menu_text(str, mnemonic, actext)
    char_u	*str;
    int		*mnemonic;
    char_u	**actext;
{
    char_u	*p;
    char_u	*text;

    /* Locate accelerator text, after the first TAB */
    p = vim_strchr(str, TAB);
    if (p != NULL)
    {
	if (actext != NULL)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, (int)(p - str));
    }
    else
	text = vim_strsave(str);
    if (text != NULL)
    {
	p = vim_strchr(text, '&');
	if (p != NULL)
	{
	    if (mnemonic != NULL)
		*mnemonic = p[1];
	    mch_memmove(p, p + 1, STRLEN(p));
	}
    }
    return text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    int
menubar_menu(name)
    char_u	*name;
{
    return (!popup_menu(name)
	    && !toolbar_menu(name)
	    && *name != MNU_HIDDEN_CHAR);
}

    int
popup_menu(name)
    char_u	*name;
{
    return (STRNCMP(name, "PopUp", 5) == 0);
}

    int
toolbar_menu(name)
    char_u	*name;
{
    return (STRNCMP(name, "ToolBar", 7) == 0);
}

#ifdef USE_GUI

    static int
get_menu_mode()
{
    if (VIsual_active)
	return MENU_INDEX_VISUAL;
    if (State & INSERT)
	return MENU_INDEX_INSERT;
    if (State & CMDLINE)
	return MENU_INDEX_CMDLINE;
    if (finish_op)
	return MENU_INDEX_OP_PENDING;
    if (State & NORMAL)
	return MENU_INDEX_NORMAL;
    return MENU_INDEX_INVALID;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu, NULL);
 */
    void
gui_create_initial_menus(menu, parent)
    VimMenu	*menu;
    VimMenu	*parent;
{
    int		idx = 0;

    while (menu)
    {
	if (menu->children != NULL)
	{
	    gui_mch_add_menu(menu, parent, idx);
	    gui_create_initial_menus(menu->children, menu);
	}
	else
	    gui_mch_add_menu_item(menu, parent, idx);
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    static void
gui_update_menus_recurse(menu, mode)
    VimMenu *menu;
    int	    mode;
{
    int	    i;

    while (menu)
    {
	if (menu->modes & mode)
	    i = FALSE;
	else
	    i = TRUE;
#ifdef USE_GUI_ATHENA
	/* Hiding menus doesn't work for Athena, it can cause a crash. */
       gui_mch_menu_grey(menu, i);
#else
	if (vim_strchr(p_go, GO_GREY) != NULL)
	    gui_mch_menu_grey(menu, i);
	else
	    gui_mch_menu_hidden(menu, i);
#endif
	gui_update_menus_recurse(menu->children, mode);
	menu = menu->next;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    void
gui_update_menus(modes)
    int	    modes;
{
    static int	    prev_mode = -1;
    int		    mode = 0;

    if (modes != 0x0)
	mode = modes;
    else
    {
	mode = get_menu_mode();
	if (mode == MENU_INDEX_INVALID)
	    mode = 0;
	else
	    mode = (1 << mode);
    }

    if (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
#ifdef USE_GUI_WIN32
	/* This can leave a tearoff as active window - make sure we
	 * have the focus <negri>*/
	gui_mch_activate_window();
#endif
    }
}

#if defined(USE_GUI_MSWIN) || defined(USE_GUI_MOTIF) || defined(USE_GUI_GTK) \
    || defined(PROTO)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    int
gui_is_menu_shortcut(key)
    int		key;
{
    VimMenu	*menu;

    if (key < 256)
	key = TO_LOWER(key);
    for (menu = root_menu; menu != NULL; menu = menu->next)
	if (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TO_LOWER(menu->mnemonic) == key))
	    return TRUE;
    return FALSE;
}
#endif

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    void
gui_show_popupmenu()
{
    VimMenu	*menu;
    int		mode;

    mode = get_menu_mode();
    if (mode == MENU_INDEX_INVALID)
	return;
    mode = menu_mode_chars[mode];

    for (menu = root_menu; menu != NULL; menu = menu->next)
	if (STRNCMP("PopUp", menu->name, 5) == 0 && menu->name[5] == mode)
	    break;

    /* Only show a popup when it is defined and has entries */
    if (menu != NULL && menu->children != NULL)
	gui_mch_show_popupmenu(menu);
}
#endif /* USE_GUI */

#if defined(USE_GUI_WIN32) || defined(PROTO)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(int enable)
{
    int		pri_tab[MENUDEPTH + 1];
    int		i;

    if (enable)
    {
	for (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    else
	gui_destroy_tearoffs_recurse(root_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    static void
gui_create_tearoffs_recurse(menu, pname, pri_tab, pri_idx)
    VimMenu		*menu;
    const char_u	*pname;
    int			*pri_tab;
    int			pri_idx;
{
    char_u	*newpname = NULL;
    int		len;
    char_u	*s;
    char_u	*d;

    if (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    while (menu != NULL)
    {
	if (menu->children != NULL && menubar_menu(menu->name))
	{
	    /* Add the menu name to the menu path.  Insert a backslash before
	     * dots (it's used to separate menu names). */
	    len = STRLEN(pname) + STRLEN(menu->name);
	    for (s = menu->name; *s; ++s)
		if (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    if (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		for (s = menu->name; *s; ++s)
		{
		    if (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		/* check if tearoff already exists */
		if (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_add_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			/* remove TEAR_STRING */
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->next;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    static void
gui_add_tearoff(tearpath, pri_tab, pri_idx)
    char_u	*tearpath;
    int		*pri_tab;
    int		pri_idx;
{
    char_u	*tbuf;
    int		t;

    tbuf = alloc(5 + STRLEN(tearpath));
    if (tbuf != NULL)
    {
	tbuf[0] = K_SPECIAL;
	tbuf[1] = K_SECOND(K_TEAROFF);
	tbuf[2] = K_THIRD(K_TEAROFF);
	STRCPY(tbuf + 3, tearpath);
	STRCAT(tbuf + 3, "\r");

	STRCAT(tearpath, ".");
	STRCAT(tearpath, TEAR_STRING);

	/* Priority of tear-off is always 1 */
	t = pri_tab[pri_idx + 1];
	pri_tab[pri_idx + 1] = 1;

	add_menu_path(tearpath, MENU_ALL_MODES, pri_tab,
		gui_menu_cb, tbuf, TRUE, FALSE);

	add_menu_path(tearpath, MENU_TIP_MODE, pri_tab,
		gui_menu_cb, (char_u *)"Tear off this menu", TRUE, FALSE);

	pri_tab[pri_idx + 1] = t;
	vim_free(tbuf);
    }
}

/*
 * Recursively destroy tearoff items
 */
    static void
gui_destroy_tearoffs_recurse(menu)
    VimMenu *menu;
{
    while (menu)
    {
	if (menu->children)
	{
	    /* check if tearoff exists */
	    if (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		/* Disconnect the item and free the memory */
		free_menu(&menu->children);
	    }
	    if (menu->children != NULL) /* if not the last one */
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->next;
    }
}

#endif /*USE_GUI_WIN32*/

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    void
execute_menu(path_name)
    char_u	*path_name;
{
    VimMenu	*menu;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;
    int		idx;

    saved_name = vim_strsave(path_name);
    if (saved_name == NULL)
	return;

    menu = root_menu;
    name = saved_name;
    while (*name)
    {
	/* Find in the menu hierarchy */
	p = menu_name_skip(name);

	while (menu != NULL)
	{
	    if (menu_name_equal(name, menu))
	    {
		if (*p == NUL && menu->children != NULL)
		{
		    EMSG("Menu path must lead to a menu item");
		    menu = NULL;
		}
		else if (*p != NUL && menu->children == NULL)
		{
		    EMSG("Part of menu-item path is not sub-menu");
		    menu = NULL;
		}
		break;
	    }
	    menu = menu->next;
	}
	if (menu == NULL || *p == NUL)
	    break;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    if (menu == NULL)
    {
	EMSG("Menu not found - check menu names");
	return;
    }

    /* Found the menu, so execute. */
    idx = get_menu_index(menu, NORMAL);
    if (idx != MENU_INDEX_INVALID)
	ins_typebuf(menu->strings[idx], menu->noremap[idx] ? -1 : 0, 0, TRUE);
    else
	EMSG("Menu not defined for Normal mode");
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    int
is_menu_separator(name)
    char_u *name;
{
    return (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

#endif /* WANT_MENU */
