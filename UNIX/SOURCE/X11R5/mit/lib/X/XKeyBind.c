/* $XConsortium: XKeyBind.c,v 11.67 92/05/19 11:23:14 converse Exp $ */
/* Copyright 1985, 1987, Massachusetts Institute of Technology */

/*
Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.
*/

/* Beware, here be monsters (still under construction... - JG */

#define NEED_EVENTS
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_LATIN2
#define XK_LATIN3
#define XK_LATIN4
#include <X11/keysymdef.h>
#include "VendorKeysym.h"
#include <stdio.h>

#define AllMods (ShiftMask|LockMask|ControlMask| \
		 Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask)

/* jbb 890512, rev. 900402 */
static char *compose_tab[] = {
	/*   */	"''^^``~~ \240\"\250*\260",
	/* ! */	"!\241^\246S\247s\247P\266p\266",
	/* " */	" \250\"\250A\304E\313I\317O\326U\334a\344e\353i\357o\366u\374y\377",
	/* # */	"",
	/* $ */	"",
	/* % */	"",
	/* & */	"",
	/* ' */	" ''\264A\301E\311I\315O\323U\332Y\335a\341e\351i\355o\363u\372y\375",
	/* ( */	"([-{",
	/* ) */	")]-}",
	/* * */	" \260A\305a\345",
	/* + */	"+#-\261",
	/* , */	",\270C\307c\347",
	/* - */	"({)}-\255L\243l\243Y\245y\245,\254^\257+\261D\320d\360:\367",
	/* . */	"^\267",
	/* / */	"/\\<\\^|C\242c\242U\265u\265O\330o\370",

	/* 0 */	"X\244x\244S\247s\247C\251c\251R\256r\256^\260",
	/* 1 */	"^\2714\2742\275",
	/* 2 */	"^\262",
	/* 3 */	"^\2634\276",
	/* 4 */	"",
	/* 5 */	"",
	/* 6 */	"",
	/* 7 */	"",
	/* 8 */	"",
	/* 9 */	"",
	/* : */	"-\367",
	/* ; */	"",
	/* < */	"/\\<\253",
	/* = */	"L\243l\243Y\245y\245",
	/* > */	">\273",
	/* ? */	"?\277",

	/* @ */	"",
	/* A */	"A@a@_\252`\300'\301^\302~\303\"\304*\305E\306",
	/* B */	"",
	/* C */	"/\242|\2420\251O\251o\251,\307",
	/* D */	"-\320",
	/* E */	"`\310'\311^\312\"\313",
	/* F */	"",
	/* G */	"",
	/* H */	"",
	/* I */	"`\314'\315^\316\"\317",
	/* J */	"",
	/* K */	"",
	/* L */	"-\243=\243",
	/* M */	"",
	/* N */	"~\321",
	/* O */	"X\244x\244S\247s\247C\251c\251R\256r\256_\272`\322'\323^\324~\325\"\326/\330",

	/* P */	"!\266",
	/* Q */	"",
	/* R */	"0\256O\256o\256",
	/* S */	"!\2470\247O\247o\247S\337",
	/* T */	"H\336",
	/* U */	"`\331'\332^\333\"\334",
	/* V */	"",
	/* W */	"",
	/* X */	"0\244O\244o\244",
	/* Y */	"-\245=\245'\335",
	/* Z */	"",
	/* [ */	"",
	/* \ */	"",
	/* ] */	"",
	/* ^ */	" ^/|!\246-\257_\2570\2601\2712\2623\263.\267A\302E\312I\316O\324U\333a\342e\352i\356o\364u\373",
	/* _ */	"A\252a\252^\257O\272o\272",

	/* ` */	" `A\300E\310I\314O\322U\331a\340e\350i\354o\362u\371",
	/* a */	"A@a@_\252`\340'\341^\342~\343\"\344*\345e\346",
	/* b */	"",
	/* c */	"/\242|\2420\251O\251o\251,\347",
	/* d */	"-\360",
	/* e */	"`\350'\351^\352\"\353",
	/* f */	"",
	/* g */	"",
	/* h */	"",
	/* i */	"`\354'\355^\356\"\357",
	/* j */	"",
	/* k */	"",
	/* l */	"-\243=\243",
	/* m */	"",
	/* n */	"~\361",
	/* o */	"X\244x\244C\251c\251R\256r\256S\247s\247_\272`\362'\363^\364~\365\"\366/\370",

	/* p */	"!\266",
	/* q */	"",
	/* r */	"0\256O\256o\256",
	/* s */	"!\2470\247O\247o\247s\337",
	/* t */	"h\376",
	/* u */	"`\371'\372^\373\"\374",
	/* v */	"",
	/* w */	"",
	/* x */	"0\244O\244o\244x\327",
	/* y */	"-\245=\245'\375\"\377",
	/* z */	"",
	/* { */	"",
	/* | */	"C\242c\242|\246",
	/* } */	"",
	/* ~ */	" ~A\303N\321O\325a\343n\361o\365",
	/*DEL*/ ""
};

static ComputeMaskFromKeytrans();
static int Initialize();
static void XConvertCase();

struct _XKeytrans {
	struct _XKeytrans *next;/* next on list */
	char *string;		/* string to return when the time comes */
	int len;		/* length of string (since NULL is legit)*/
	KeySym key;		/* keysym rebound */
	unsigned int state;	/* modifier state */
	KeySym *modifiers;	/* modifier keysyms you want */
	int mlen;		/* length of modifier list */
};

static KeySym
KeyCodetoKeySym(dpy, keycode, col)
    register Display *dpy;
    KeyCode keycode;
    int col;
{
    register int per = dpy->keysyms_per_keycode;
    register KeySym *syms;
    KeySym lsym, usym;

    if ((col < 0) || ((col >= per) && (col > 3)) ||
	((int)keycode < dpy->min_keycode) || ((int)keycode > dpy->max_keycode))
      return NoSymbol;

    syms = &dpy->keysyms[(keycode - dpy->min_keycode) * per];
    if (col < 4) {
	if (col > 1) {
	    while ((per > 2) && (syms[per - 1] == NoSymbol))
		per--;
	    if (per < 3)
		col -= 2;
	}
	if ((per <= (col|1)) || (syms[col|1] == NoSymbol)) {
	    XConvertCase(dpy, syms[col&~1], &lsym, &usym);
	    if (!(col & 1))
		return lsym;
	    else if (usym == lsym)
		return NoSymbol;
	    else
		return usym;
	}
    }
    return syms[col];
}

#if NeedFunctionPrototypes
KeySym
XKeycodeToKeysym(Display *dpy,
#if NeedWidePrototypes
		 unsigned int kc,
#else
		 KeyCode kc,
#endif
		 int col)
#else
KeySym
XKeycodeToKeysym(dpy, kc, col)
    Display *dpy;
    KeyCode kc;
    int col;
#endif
{
    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return NoSymbol;
    return KeyCodetoKeySym(dpy, kc, col);
}

KeyCode
XKeysymToKeycode(dpy, ks)
    Display *dpy;
    KeySym ks;
{
    register int i, j;

    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return (KeyCode) 0;
    for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	for (i = dpy->min_keycode; i <= dpy->max_keycode; i++) {
	    if (KeyCodetoKeySym(dpy, (KeyCode) i, j) == ks)
		return i;
	}
    }
    return 0;
}

KeySym
XLookupKeysym(event, col)
    register XKeyEvent *event;
    int col;
{
    if ((! event->display->keysyms) && (! Initialize(event->display)))
	return NoSymbol;
    return KeyCodetoKeySym(event->display, event->keycode, col);
}

static int
InitModMap(dpy)
    Display *dpy;
{
    register XModifierKeymap *map;
    register int i, j, n;
    KeySym sym;
    register struct _XKeytrans *p;

    if (! (dpy->modifiermap = map = XGetModifierMapping(dpy)))
	return 0;
    dpy->free_funcs->modifiermap = XFreeModifiermap;
    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return 0;
    LockDisplay(dpy);
    /* If any Lock key contains Caps_Lock, then interpret as Caps_Lock,
     * else if any contains Shift_Lock, then interpret as Shift_Lock,
     * else ignore Lock altogether.
     */
    dpy->lock_meaning = NoSymbol;
    /* Lock modifiers are in the second row of the matrix */
    n = 2 * map->max_keypermod;
    for (i = map->max_keypermod; i < n; i++) {
	for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	    sym = KeyCodetoKeySym(dpy, map->modifiermap[i], j);
	    if (sym == XK_Caps_Lock) {
		dpy->lock_meaning = XK_Caps_Lock;
		break;
	    } else if (sym == XK_Shift_Lock) {
		dpy->lock_meaning = XK_Shift_Lock;
	    }
	}
    }
    /* Now find any Mod<n> modifier acting as the Group modifier */
    dpy->mode_switch = 0;
    n *= 4;
    for (i = 3*map->max_keypermod; i < n; i++) {
	for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	    sym = KeyCodetoKeySym(dpy, map->modifiermap[i], j);
	    if (sym == XK_Mode_switch)
		dpy->mode_switch |= 1 << (i / map->max_keypermod);
	}
    }
    for (p = dpy->key_bindings; p; p = p->next)
	ComputeMaskFromKeytrans(dpy, p);
    UnlockDisplay(dpy);
    return 1;
}

XRefreshKeyboardMapping(event)
    register XMappingEvent *event;
{

    if(event->request == MappingKeyboard) {
	/* XXX should really only refresh what is necessary
	 * for now, make initialize test fail
	 */
	LockDisplay(event->display);
	if (event->display->keysyms) {
	     Xfree ((char *)event->display->keysyms);
	     event->display->keysyms = NULL;
	}
	UnlockDisplay(event->display);
    }
    if(event->request == MappingModifier) {
	LockDisplay(event->display);
	if (event->display->modifiermap) {
	    XFreeModifiermap(event->display->modifiermap);
	    event->display->modifiermap = NULL;
	}
	UnlockDisplay(event->display);
	/* go ahead and get it now, since initialize test may not fail */
	(void) InitModMap(event->display);
    }
}

static int
Initialize(dpy)
    Display *dpy;
{
    int per, n;
    KeySym *keysyms;

    /* 
     * lets go get the keysyms from the server.
     */
    if (!dpy->keysyms) {
	n = dpy->max_keycode - dpy->min_keycode + 1;
	keysyms = XGetKeyboardMapping (dpy, (KeyCode) dpy->min_keycode,
				       n, &per);
	/* keysyms may be NULL */
	if (! keysyms) return 0;

	LockDisplay(dpy);
	dpy->keysyms = keysyms;
	dpy->keysyms_per_keycode = per;
	UnlockDisplay(dpy);
    }
    if (!dpy->modifiermap)
        return InitModMap(dpy);
    return 1;
}

/*ARGSUSED*/
static void
XConvertCase(dpy, sym, lower, upper)
    Display *dpy;
    register KeySym sym;
    KeySym *lower;
    KeySym *upper;
{
    *lower = sym;
    *upper = sym;
    switch(sym >> 8) {
    case 0:
	if ((sym >= XK_A) && (sym <= XK_Z))
	    *lower += (XK_a - XK_A);
	else if ((sym >= XK_a) && (sym <= XK_z))
	    *upper -= (XK_a - XK_A);
	else if ((sym >= XK_Agrave) && (sym <= XK_Odiaeresis))
	    *lower += (XK_agrave - XK_Agrave);
	else if ((sym >= XK_agrave) && (sym <= XK_odiaeresis))
	    *upper -= (XK_agrave - XK_Agrave);
	else if ((sym >= XK_Ooblique) && (sym <= XK_Thorn))
	    *lower += (XK_oslash - XK_Ooblique);
	else if ((sym >= XK_oslash) && (sym <= XK_thorn))
	    *upper -= (XK_oslash - XK_Ooblique);
	break;
#ifdef XK_LATIN2
    case 1:
	/* Assume the KeySym is a legal value (ignore discontinuities) */
	if (sym == XK_Aogonek)
	    *lower = XK_aogonek;
	else if (sym >= XK_Lstroke && sym <= XK_Sacute)
	    *lower += (XK_lstroke - XK_Lstroke);
	else if (sym >= XK_Scaron && sym <= XK_Zacute)
	    *lower += (XK_scaron - XK_Scaron);
	else if (sym >= XK_Zcaron && sym <= XK_Zabovedot)
	    *lower += (XK_zcaron - XK_Zcaron);
	else if (sym == XK_aogonek)
	    *upper = XK_Aogonek;
	else if (sym >= XK_lstroke && sym <= XK_sacute)
	    *upper -= (XK_lstroke - XK_Lstroke);
	else if (sym >= XK_scaron && sym <= XK_zacute)
	    *upper -= (XK_scaron - XK_Scaron);
	else if (sym >= XK_zcaron && sym <= XK_zabovedot)
	    *upper -= (XK_zcaron - XK_Zcaron);
	else if (sym >= XK_Racute && sym <= XK_Tcedilla)
	    *lower += (XK_racute - XK_Racute);
	else if (sym >= XK_racute && sym <= XK_tcedilla)
	    *upper -= (XK_racute - XK_Racute);
	break;
#endif
#ifdef XK_LATIN3
    case 2:
	/* Assume the KeySym is a legal value (ignore discontinuities) */
	if (sym >= XK_Hstroke && sym <= XK_Hcircumflex)
	    *lower += (XK_hstroke - XK_Hstroke);
	else if (sym >= XK_Gbreve && sym <= XK_Jcircumflex)
	    *lower += (XK_gbreve - XK_Gbreve);
	else if (sym >= XK_hstroke && sym <= XK_hcircumflex)
	    *upper -= (XK_hstroke - XK_Hstroke);
	else if (sym >= XK_gbreve && sym <= XK_jcircumflex)
	    *upper -= (XK_gbreve - XK_Gbreve);
	else if (sym >= XK_Cabovedot && sym <= XK_Scircumflex)
	    *lower += (XK_cabovedot - XK_Cabovedot);
	else if (sym >= XK_cabovedot && sym <= XK_scircumflex)
	    *upper -= (XK_cabovedot - XK_Cabovedot);
	break;
#endif
#ifdef XK_LATIN4
    case 3:
	/* Assume the KeySym is a legal value (ignore discontinuities) */
	if (sym >= XK_Rcedilla && sym <= XK_Tslash)
	    *lower += (XK_rcedilla - XK_Rcedilla);
	else if (sym >= XK_rcedilla && sym <= XK_tslash)
	    *upper -= (XK_rcedilla - XK_Rcedilla);
	else if (sym == XK_ENG)
	    *lower = XK_eng;
	else if (sym == XK_eng)
	    *upper = XK_ENG;
	else if (sym >= XK_Amacron && sym <= XK_Umacron)
	    *lower += (XK_amacron - XK_Amacron);
	else if (sym >= XK_amacron && sym <= XK_umacron)
	    *upper -= (XK_amacron - XK_Amacron);
	break;
#endif
    }
}

static int
XTranslateKey(dpy, keycode, modifiers, modifiers_return, keysym_return)
    register Display *dpy;
    KeyCode keycode;
    register unsigned int modifiers;
    unsigned int *modifiers_return;
    KeySym *keysym_return;
{
    int per;
    register KeySym *syms;
    KeySym sym, lsym, usym;

    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return 0;
    *modifiers_return = (ShiftMask|LockMask) | dpy->mode_switch;
    if (((int)keycode < dpy->min_keycode) || ((int)keycode > dpy->max_keycode))
    {
	*keysym_return = NoSymbol;
	return 1;
    }
    per = dpy->keysyms_per_keycode;
    syms = &dpy->keysyms[(keycode - dpy->min_keycode) * per];
    while ((per > 2) && (syms[per - 1] == NoSymbol))
	per--;
    if ((per > 2) && (modifiers & dpy->mode_switch)) {
	syms += 2;
	per -= 2;
    }
    if (!(modifiers & ShiftMask) &&
	(!(modifiers & LockMask) || (dpy->lock_meaning == NoSymbol))) {
	if ((per == 1) || (syms[1] == NoSymbol))
	    XConvertCase(dpy, syms[0], keysym_return, &usym);
	else
	    *keysym_return = syms[0];
    } else if (!(modifiers & LockMask) ||
	       (dpy->lock_meaning != XK_Caps_Lock)) {
	if ((per == 1) || ((usym = syms[1]) == NoSymbol))
	    XConvertCase(dpy, syms[0], &lsym, &usym);
	*keysym_return = usym;
    } else {
	if ((per == 1) || ((sym = syms[1]) == NoSymbol))
	    sym = syms[0];
	XConvertCase(dpy, sym, &lsym, &usym);
	if (!(modifiers & ShiftMask) && (sym != syms[0]) &&
	    ((sym != usym) || (lsym == usym)))
	    XConvertCase(dpy, syms[0], &lsym, &usym);
	*keysym_return = usym;
    }
    if (*keysym_return == XK_VoidSymbol)
	*keysym_return = NoSymbol;
    return 1;
}

static void
ComposeLed(dpy, state)
    Display *dpy;
    int state;
{
#define Compose_LED	16

    XKeyboardControl kbdctrl;

    kbdctrl.led_mode = state;
    kbdctrl.led = Compose_LED;	
    XChangeKeyboardControl(dpy, KBLed|KBLedMode, &kbdctrl);
}

/*
 * dead key and compose-character support (jbb 890909, rev. 900305, 910314, 910906)
 */
static void
ComposeHandler(dpy, keysym_in_out, modifiers, status)
    Display *dpy;
    KeySym *keysym_in_out;
    unsigned int modifiers;
    XComposeStatus *status;
{
    register KeySym symbol = *keysym_in_out;

    switch(symbol) {
    case XK_acute:	 symbol = XK_apostrophe; goto dead;
    case XK_diaeresis:   symbol = XK_quotedbl; goto dead;
    case XK_cedilla:     symbol = XK_comma; goto dead;
#ifdef SunXK_FA_Grave	/* Sun */
    case SunXK_FA_Grave:  symbol = XK_grave; goto dead;
    case SunXK_FA_Circum: symbol = XK_asciicircum; goto dead;
    case SunXK_FA_Tilde:  symbol = XK_asciitilde; goto dead;
    case SunXK_FA_Acute:  symbol = XK_apostrophe; goto dead;
    case SunXK_FA_Diaeresis: symbol = XK_quotedbl; goto dead;
    case SunXK_FA_Cedilla: symbol = XK_comma; goto dead;
#endif
#ifdef DXK_grave_accent	/* DEC */
    case DXK_grave_accent:
    case DXK_acute_accent:
    case DXK_circumflex_accent:
    case DXK_tilde:
    case DXK_diaeresis:
    case DXK_ring_accent:
    case DXK_cedilla_accent:
	symbol &= 0xFF; goto dead;
#endif
#ifdef hpXK_mute_grave	/* HP */
    case hpXK_mute_grave:	symbol = XK_grave; goto dead;
    case hpXK_mute_acute:	symbol = XK_apostrophe; goto dead;
    case hpXK_mute_asciicircum:	symbol = XK_asciicircum; goto dead;
    case hpXK_mute_asciitilde:	symbol = XK_asciitilde; goto dead;
    case hpXK_mute_diaeresis:	symbol = XK_quotedbl; goto dead;
#endif
#if 0
    /* These don't occur in ISO Latin-1 */
    case XK_macron:
    case XK_breve:
    case XK_abovedot:
    case XK_doubleacute:
    case XK_ogonek:
    case XK_caron:
#endif
dead:
	status->chars_matched = 1;
	ComposeLed(dpy, LedModeOn);
	break;
    case XK_Multi_key:
	status->chars_matched = 1;
	ComposeLed(dpy, LedModeOn);
	*keysym_in_out = NoSymbol;
	return;
    case XK_Delete:
	symbol = '\177';
	break;
    }

    /* non-Latin-1 keys and ctrl chars silently terminate compose */
    if (status->chars_matched != 0 &&
        ((symbol >> 8) != 0x0 || (modifiers & ControlMask))) {
	/* but modifiers have no effect on compose status! */
	if (! IsModifierKey(symbol)) {
	    status->chars_matched = 0;
	    ComposeLed(dpy, LedModeOff);
	}
	return;
    }

    if (status->chars_matched == 1) {
	/* DEL ends compose processing */
	if (symbol == '\177') {
	    status->chars_matched = 0;
	    ComposeLed(dpy, LedModeOff);
	} else {
	    if (symbol == XK_degree) symbol = XK_asterisk;
		/* degree behaves like asterisk for compose processing */
	    status->compose_ptr = compose_tab[(symbol&0x7f) - ' '];
	    status->chars_matched = 2;
	}
	*keysym_in_out = NoSymbol;
    }
    else if (status->chars_matched == 2) {
	register unsigned char *p;
	p = (unsigned char *)status->compose_ptr;
	ComposeLed(dpy, LedModeOff);
	if (symbol == '\177') {
	    *keysym_in_out = NoSymbol;
	    status->chars_matched = 0;
	} else {
	    if (symbol == XK_degree) symbol = XK_asterisk;
	    while (p && *p)
		if ((symbol&0x7f) == p[0]) {
		    *keysym_in_out = p[1];
		    status->chars_matched = 3;	/* 920304 for OW3 */
		    return;
		} else p+=2;
	    if (! (p && *p)) {
		XBell(dpy, 50);
		*keysym_in_out = NoSymbol;
		status->chars_matched = 0;
	    }
	}
    }
    else if (status->chars_matched == 3)
	status->chars_matched = 0;
}

static int
XTranslateKeySym(dpy, symbol, modifiers, buffer, nbytes)
    Display *dpy;
    register KeySym symbol;
    unsigned int modifiers;
    char *buffer;
    int nbytes;
{
    register struct _XKeytrans *p; 
    int length;
    unsigned long hiBytes;
    register unsigned char c;

    if (!symbol)
	return 0;
    /* see if symbol rebound, if so, return that string. */
    for (p = dpy->key_bindings; p; p = p->next) {
	if (((modifiers & AllMods) == p->state) && (symbol == p->key)) {
	    length = p->len;
	    if (length > nbytes) length = nbytes;
	    bcopy (p->string, buffer, length);
	    return length;
	}
    }
    /* try to convert to Latin-1, handling control */
    hiBytes = symbol >> 8;
    if (!(nbytes &&
	  ((hiBytes == 0) ||
	   ((hiBytes == 0xFF) &&
	    (((symbol >= XK_BackSpace) && (symbol <= XK_Clear)) ||
	     (symbol == XK_Return) ||
	     (symbol == XK_Escape) ||
	     (symbol == XK_KP_Space) ||
	     (symbol == XK_KP_Tab) ||
	     (symbol == XK_KP_Enter) ||
	     ((symbol >= XK_KP_Multiply) && (symbol <= XK_KP_9)) ||
	     (symbol == XK_KP_Equal) ||
	     (symbol == XK_Delete))))))
	return 0;

    /* if X keysym, convert to ascii by grabbing low 7 bits */
    if (symbol == XK_KP_Space)
	c = XK_space & 0x7F; /* patch encoding botch */
    else if (symbol == XK_hyphen)
	c = XK_minus & 0xFF; /* map to equiv character */
    else if (hiBytes == 0xFF)
	c = symbol & 0x7F;
    else
	c = symbol & 0xFF;
    /* only apply Control key if it makes sense, else ignore it */
    if (modifiers & ControlMask) {
	if ((c >= '@' && c < '\177') || c == ' ') c &= 0x1F;
	else if (c == '2') c = '\000';
	else if (c >= '3' && c <= '7') c -= ('3' - '\033');
	else if (c == '8') c = '\177';
	else if (c == '/') c = '_' & 0x1F;
    }
    buffer[0] = c;
    return 1;
}
  
/*ARGSUSED*/
int
XLookupString (event, buffer, nbytes, keysym, status)
    register XKeyEvent *event;
    char *buffer;	/* buffer */
    int nbytes;	/* space in buffer for characters */
    KeySym *keysym;
    XComposeStatus *status;	/* implemented jbb 890512 */
{
    unsigned int modifiers;
    KeySym symbol;

    if (! XTranslateKey(event->display, event->keycode, event->state,
		  &modifiers, &symbol))
	return 0;

    if (status && event->type == KeyPress)
	ComposeHandler(event->display, &symbol, event->state, status);

    if (keysym)
	*keysym = symbol;
    /* arguable whether to use (event->state & ~modifiers) here */
    return XTranslateKeySym(event->display, symbol, event->state,
			    buffer, nbytes);
}

static void
_XFreeKeyBindings (dpy)
    Display *dpy;
{
    register struct _XKeytrans *p, *np;

    for (p = dpy->key_bindings; p; p = np) {
	np = p->next;
	Xfree(p->string);
	Xfree((char *)p->modifiers);
	Xfree((char *)p);
    }   
}

#if NeedFunctionPrototypes
XRebindKeysym (
    Display *dpy,
    KeySym keysym,
    KeySym *mlist,
    int nm,		/* number of modifiers in mlist */
    _Xconst unsigned char *str,
    int nbytes)
#else
XRebindKeysym (dpy, keysym, mlist, nm, str, nbytes)
    Display *dpy;
    KeySym keysym;
    KeySym *mlist;
    int nm;		/* number of modifiers in mlist */
    unsigned char *str;
    int nbytes;
#endif
{
    register struct _XKeytrans *tmp, *p;
    int nb;

    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return;
    LockDisplay(dpy);
    tmp = dpy->key_bindings;
    nb = sizeof(KeySym) * nm;

    if ((! (p = (struct _XKeytrans *) Xmalloc( sizeof(struct _XKeytrans)))) ||
	((! (p->string = (char *) Xmalloc( (unsigned) nbytes))) && 
	 (nbytes > 0)) ||
	((! (p->modifiers = (KeySym *) Xmalloc( (unsigned) nb))) &&
	 (nb > 0))) {
	if (p) {
	    if (p->string) Xfree(p->string);
	    if (p->modifiers) Xfree((char *) p->modifiers);
	    Xfree((char *) p);
	}
	UnlockDisplay(dpy);
	return;
    }

    dpy->key_bindings = p;
    dpy->free_funcs->key_bindings = _XFreeKeyBindings;
    p->next = tmp;	/* chain onto list */
    bcopy ((char *) str, p->string, nbytes);
    p->len = nbytes;
    bcopy ((char *) mlist, (char *) p->modifiers, nb);
    p->key = keysym;
    p->mlen = nm;
    ComputeMaskFromKeytrans(dpy, p);
    UnlockDisplay(dpy);
    return;
}

/*
 * given a KeySym, returns the first keycode containing it, if any.
 */
static CARD8
FindKeyCode(dpy, code)
    register Display *dpy;
    register KeySym code;
{

    register KeySym *kmax = dpy->keysyms + 
	(dpy->max_keycode - dpy->min_keycode + 1) * dpy->keysyms_per_keycode;
    register KeySym *k = dpy->keysyms;
    while (k < kmax) {
	if (*k == code)
	    return (((k - dpy->keysyms) / dpy->keysyms_per_keycode) +
		    dpy->min_keycode);
	k += 1;
	}
    return 0;
}

	
/*
 * given a list of modifiers, computes the mask necessary for later matching.
 * This routine must lookup the key in the Keymap and then search to see
 * what modifier it is bound to, if any.  Sets the AnyModifier bit if it
 * can't map some keysym to a modifier.
 */
static
ComputeMaskFromKeytrans(dpy, p)
    Display *dpy;
    register struct _XKeytrans *p;
{
    register int i;
    register CARD8 code;
    register XModifierKeymap *m = dpy->modifiermap;

    p->state = AnyModifier;
    for (i = 0; i < p->mlen; i++) {
	/* if not found, then not on current keyboard */
	if ((code = FindKeyCode(dpy, p->modifiers[i])) == 0)
		return;
	/* code is now the keycode for the modifier you want */
	{
	    register int j = m->max_keypermod<<3;

	    while ((--j >= 0) && (code != m->modifiermap[j]))
		;
	    if (j < 0)
		return;
	    p->state |= (1<<(j/m->max_keypermod));
	}
    }
    p->state &= AllMods;
}
