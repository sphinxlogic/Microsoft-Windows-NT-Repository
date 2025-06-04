/*
 *   xmcd - Motif(tm) CD Audio Player
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef LINT
static char *_hotkey_c_ident_ = "@(#)hotkey.c	6.21 98/10/27";
#endif

#include "common_d/appenv.h"
#include "xmcd_d/xmcd.h"
#include "xmcd_d/widget.h"
#include "xmcd_d/cdfunc.h"
#include "xmcd_d/hotkey.h"


#define TOTAL_GRABLISTS	2
#define MAIN_LIST	0
#define KEYPAD_LIST	1

#define MAX_MODSTR_LEN	12
#define MAX_KEYSTR_LEN	12
#define MAX_BTNSTR_LEN	48


typedef struct grablist {
	Widget		gr_button;
	KeyCode		gr_keycode;
	Modifiers	gr_modifier;
	bool_t		gr_domnemonic;
	KeySym		gr_keysym;
	char		gr_keystr[MAX_KEYSTR_LEN];
	struct grablist	*next;
} grablist_t;


typedef struct {
	char		*name;
	Modifiers	mask;
} modtab_t;


extern appdata_t	app_data;
extern widgets_t	widgets;
extern FILE		*errfp;

STATIC modtab_t		modtab[8] = {
	{ "Shift",	ShiftMask	},
	{ "Lock",	LockMask	},
	{ "Ctrl",	ControlMask	},
	{ "Mod1",	Mod1Mask	},
	{ "Mod2",	Mod2Mask	},
	{ "Mod3",	Mod3Mask	},
	{ "Mod4",	Mod4Mask	},
	{ "Mod5",	Mod5Mask	},
};

STATIC grablist_t	*grablists[TOTAL_GRABLISTS] = {
	NULL, NULL
};


/***********************
 *  internal routines  *
 ***********************/


/*
 * hotkey_override_mnemonic
 *	Apply override mnemonic rules, if any.
 *
 * Args:
 *	g - Pointer to the button's associated grablist structure.
 *
 * Return:
 *	Nothing.
 */
STATIC void
hotkey_override_mnemonic(grablist_t *g)
{
	char		*lstr;
	XmString	xs;

	if (g->gr_button == NULL) {
		/* No associated button */
		g->gr_keysym = XK_VoidSymbol;
		return;
	}

	XtVaGetValues(g->gr_button, XmNlabelString, &xs, NULL);

	if (XmStringGetLtoR(xs, XmSTRING_DEFAULT_CHARSET, &lstr)) {
		/* No need to set mnemonic if the first
		 * button character is a digit.
		 */
		if (isdigit(lstr[0])) {
			g->gr_keysym = XK_VoidSymbol;
			return;
		}

		/* Make the first letter of the button label
		 * match if possible, even if the
		 * capitalization is wrong.
		 */
		if (toupper(g->gr_keystr[0]) == toupper(lstr[0]) &&
		    !(g->gr_modifier & ~ShiftMask)) {
			char	s[2];

			s[0] = lstr[0];
			s[1] = '\0';

			g->gr_keysym = XStringToKeysym(s);
			return;
		}
	}
}


/*
 * hotkey_set_mnemonics
 *	Set the mnemonics of pushbuttons with hotkey support.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
STATIC void
hotkey_set_mnemonics(void)
{
	int		i;
	grablist_t	*p;
	Widget		prev_btn = (Widget) NULL;

	/* Set mnemonics on button faces */
	for (i = 0; i < TOTAL_GRABLISTS; i++) {
		for (p = grablists[i]; p != NULL; p = p->next) {
			if (!p->gr_domnemonic)
				continue;
				
			if (p->gr_button != (Widget) NULL) {
				if (prev_btn == p->gr_button)
					continue;

				if (p->gr_keysym != XK_VoidSymbol) {
					XtVaSetValues(p->gr_button,
						XmNmnemonic, p->gr_keysym,
						NULL
					);
				}
			}
			prev_btn = p->gr_button;
		}
	}
}


/*
 * hotkey_parse_xlat_line
 *	A limited translation string parser.
 *
 * Args:
 *	line - The translation text string line to be parsed
 *	modp - The key modifier return string
 *	keyp - The key event return string
 *	btnp - The button event return string
 *
 * Return:
 *	Nothing.
 */
STATIC bool_t
hotkey_parse_xlat_line(
	char *line,
	char **modp,
	char **keyp,
	char **btnp
)
{
	char		*p,
			*q;
	static char	modstr[MAX_MODSTR_LEN],
			keystr[MAX_KEYSTR_LEN],
			btnstr[MAX_BTNSTR_LEN];

	keystr[0] = modstr[0] = btnstr[0] = '\0';

	/* Get modifier specification */
	p = line;
	q = strchr(p, '<');

	if (q == NULL)
		return FALSE;
	else if (q > p) {
		*q = '\0';
		(void) strncpy(modstr, p, MAX_MODSTR_LEN-1);
		modstr[MAX_MODSTR_LEN-1] = '\0';
		*modp = modstr;
		*q = '<';
	}

	/* Get event specification */
	p = q + 1;
	q = strchr(p, '>');

	if (q == NULL || q == p)
		return FALSE;
	else {
		*q = '\0';

		/* We are interested only in key events here */
		if (strncmp(p, "Key", 3) != 0)
			return FALSE;

		*q = '>';
	}

	/* Get key specification */
	p = q + 1;
	q = strchr(p, ':');

	if (q == NULL || q == p)
		return FALSE;
	else {
		*q = '\0';
		(void) strncpy(keystr, p, MAX_KEYSTR_LEN-1);
		keystr[MAX_KEYSTR_LEN-1] = '\0';
		*keyp = keystr;
		*q = ':';
	}

	/* Get associated button */
	p = q + 1;
	q = strchr(p, '(');

	if (q == NULL || q == p)
		return FALSE;
	else {
		p = q + 1;
		q = strchr(p, ',');

		if (q == NULL || q == p)
			return FALSE;
		else {
			*q = '\0';
			(void) strncpy(btnstr, p, MAX_BTNSTR_LEN-1);
			btnstr[MAX_BTNSTR_LEN-1] = '\0';
			*btnp = btnstr;
			*q = ',';
		}
	}

	return TRUE;
}


/*
 * hotkey_build_grablist
 *	Build a linked list of widgets which have associated
 *	hotkey support, and information about the hotkey.  These
 *	keys will be grabbed when the parent form window has input
 *	focus.
 *
 * Args:
 *	form - The parent form widget
 *	str - The translation string specifying the hotkey
 *	do_mnemonic - Whether to configure the widget's label for mnemonic
 *	listhead - The list head (return)
 *
 * Return:
 *	Nothing.
 */
STATIC void
hotkey_build_grablist(
	Widget		form,
	char		*str,
	bool_t		do_mnemonic,
	grablist_t	**listhead
)
{
	int		i;
	char		*p,
			*q,
			*end,
			*modstr,
			*keystr,
			*btnstr;
	grablist_t	*g = NULL;

	p = str;
	end = p + strlen(p);

	do {
		while (isspace(*p))
			p++;
		q = strchr(p, '\n');

		if (p >= end)
			break;

		if (q == NULL) {
			if (q > end)
				break;
		}
		else
			*q = '\0';

		modstr = keystr = btnstr = NULL;

		/* Parse translation line */
		if (*p != '#' &&
		    hotkey_parse_xlat_line(p, &modstr, &keystr, &btnstr) &&
		    keystr != NULL) {

			/* Allocate new list element */
			if (*listhead == NULL) {
				*listhead = g = (grablist_t *)(void *)
					MEM_ALLOC(
						"hotkey_listhead",
						sizeof(grablist_t)
					);

				if (g == NULL) {
					CD_FATAL(app_data.str_nomemory);
					return;
				}
			}
			else {
				g->next = (grablist_t *)(void *)
					MEM_ALLOC(
						"g->next",
						sizeof(grablist_t)
					);

				if (g->next == NULL) {
					CD_FATAL(app_data.str_nomemory);
					return;
				}

				g = g->next;
			}
			g->next = NULL;
			g->gr_domnemonic = do_mnemonic;

			strncpy(g->gr_keystr, keystr, MAX_KEYSTR_LEN-1);
			g->gr_keystr[MAX_KEYSTR_LEN-1] = '\0';

			g->gr_keysym = XStringToKeysym(keystr);
			g->gr_keycode = XKeysymToKeycode(
				XtDisplay(widgets.toplevel),
				g->gr_keysym
			);

			g->gr_modifier = 0;
			if (modstr != NULL) {
				for (i = 0; i < 8; i++) {
					if (strcmp(modtab[i].name,
						   modstr) == 0) {
						g->gr_modifier =
							modtab[i].mask;
						break;
					}
				}
			}

			g->gr_button = (Widget) NULL;
			if (btnstr != NULL)
				g->gr_button = XtNameToWidget(form, btnstr);

			/* Special mnemonic handling */
			hotkey_override_mnemonic(g);
		}

		if (q != NULL) {
			*q = '\n';
			p = q + 1;
		}
		else
			p = end;

	} while (p < end);
}


/***********************
 *   public routines   *
 ***********************/


/*
 * hotkey_init
 *	Top level setup function for the hotkey subsystem.  Called
 *	once at program startup.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
void
hotkey_init(void)
{
	char		*xlat_str;
	widgets_t	*m;

	m = &widgets;

	xlat_str = (char *) MEM_ALLOC("xlat_str", MAX_TRANSLATIONS_SZ);
	if (xlat_str == NULL) {
		CD_FATAL(app_data.str_nomemory);
		return;
	}

	/* Translations for the main window form */
	(void) sprintf(xlat_str,
		"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		app_data.mode_key,
		app_data.lock_key,
		app_data.repeat_key,
		app_data.shuffle_key,
		app_data.eject_key,
		app_data.quit_key,
		app_data.dbprog_key,
		app_data.help_key,
		app_data.options_key,
		app_data.time_key,
		app_data.ab_key,
		app_data.sample_key,
		app_data.keypad_key,
		app_data.playpause_key,
		app_data.stop_key,
		app_data.prevdisc_key,
		app_data.nextdisc_key,
		app_data.prevtrk_key,
		app_data.nexttrk_key,
		app_data.previdx_key,
		app_data.nextidx_key,
		app_data.rew_key,
		app_data.ff_key);

	XtOverrideTranslations(
		m->main.form,
		XtParseTranslationTable(xlat_str)
	);

	hotkey_build_grablist(
		m->main.form,
		xlat_str,
		FALSE,
		&grablists[MAIN_LIST]
	);

	/* Translations for the keypad window form */
	(void) sprintf(xlat_str,
		"%s%s%s%s%s%s%s%s%s%s%s%s%s",
		app_data.keypad0_key,
		app_data.keypad1_key,
		app_data.keypad2_key,
		app_data.keypad3_key,
		app_data.keypad4_key,
		app_data.keypad5_key,
		app_data.keypad6_key,
		app_data.keypad7_key,
		app_data.keypad8_key,
		app_data.keypad9_key,
		app_data.keypadclear_key,
		app_data.keypadenter_key,
		app_data.keypadcancel_key);

	XtOverrideTranslations(
		m->keypad.form,
		XtParseTranslationTable(xlat_str)
	);

	hotkey_build_grablist(
		m->keypad.form,
		xlat_str,
		TRUE,
		&grablists[KEYPAD_LIST]
	);

	/* Set button label mnemonics */
	hotkey_set_mnemonics();

	MEM_FREE(xlat_str);
}


/*
 * hotkey_grabkeys
 *	Grab all keys used as hotkeys in the specified window form.
 *
 * Args:
 *	form - The parent form widget.
 *
 * Return:
 *	Nothing.
 */
void
hotkey_grabkeys(Widget form)
{
	grablist_t	*list,
			*p;

	if (form == widgets.main.form)
		list = grablists[MAIN_LIST];
	else if (form == widgets.keypad.form)
		list = grablists[KEYPAD_LIST];
	else
		list = NULL;

	for (p = list; p != NULL; p = p->next) {
		XtGrabKey(
			form,
			p->gr_keycode,
			p->gr_modifier,
			True,
			GrabModeAsync,
			GrabModeAsync
		);
	}
}


/*
 * hotkey_ungrabkeys
 *	Ungrab all keys used as hotkeys in the specified window form.
 *
 * Args:
 *	form - The parent form widget.
 *
 * Return:
 *	Nothing.
 */
void
hotkey_ungrabkeys(Widget form)
{
	grablist_t	*list,
			*p;

	if (form == widgets.main.form)
		list = grablists[MAIN_LIST];
	else if (form == widgets.keypad.form)
		list = grablists[KEYPAD_LIST];
	else
		list = NULL;

	for (p = list; p != NULL; p = p->next)
		XtUngrabKey(form, p->gr_keycode, p->gr_modifier);
}


/*
 * hotkey_tooltip_mnemonic
 *	Set the tooltip mnemonic for the specified pushbutton
 *
 * Args:
 *	btn - The button for which the tooltip is being displayed
 *
 * Return:
 *	Nothing.
 */
void
hotkey_tooltip_mnemonic(Widget btn)
{
	grablist_t	*p;

	for (p = grablists[MAIN_LIST]; p != NULL; p = p->next) {
		if (p->gr_button == btn) {
			XtVaSetValues(widgets.tooltip.tooltip_lbl,
				XmNmnemonic, p->gr_keysym,
				NULL
			);
			return;
		}
	}

	/* If not found, simply unset the mnemonic */
	XtVaSetValues(widgets.tooltip.tooltip_lbl,
		XmNmnemonic, XK_VoidSymbol,
		NULL
	);
}


/*
 * hotkey
 *	Widget action routine to handle hotkey events
 */
void
hotkey(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	int	i;
	Widget	action_widget;

	if ((int) *num_args <= 0)
		return;	/* Error: should have at least one arg */

	if ((action_widget = XtNameToWidget(w, args[0])) == (Widget) NULL)
		return;	/* Can't find widget */

	/* Switch keyboard focus to the widget of interest */
	XmProcessTraversal(action_widget, XmTRAVERSE_CURRENT);

	for (i = 1; i < (int) *num_args; i++)
		/* Invoke the named action of the specified widget */
		XtCallActionProc(action_widget, args[i], ev, NULL, 0);
}


