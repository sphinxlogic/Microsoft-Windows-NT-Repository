/*
 * Linux version of keymapping setup. The kernel (since 0.99.14) has support
 * for fully remapping the keyboard, but there are some differences between
 * the Linux map and the SVR4 map (esp. in the extended keycodes). We also
 * remove the restriction on what keycodes can be remapped.
 * Orest Zborowski.
 */
/*
 * $XFree86: mit/server/ddx/x386/common/xf86_KbdLnx.c,v 2.3 1994/03/11 03:04:58 dawes Exp $
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * $Header: /proj/X11/mit/server/ddx/x386/RCS/x386Kbd.c,v 1.1 1991/06/02 22:36:16 root Exp $
 */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "atKeynames.h"
#include "xf86_Config.h"

#include "x386Keymap.h"


/*
 * LegalModifier --
 *      determine whether a key is a legal modifier key, i.e send a
 *      press/release sequence.
 */

/*ARGSUSED*/
Bool
LegalModifier(key)
     int  key;
{
  return (TRUE);
}

/*
 * x386KbdGetMapping --
 *	Get the national keyboard mapping. The keyboard type is set, a new map
 *      and the modifiermap is computed.
 */

static void readKernelMapping(KeySymsPtr pKeySyms, CARD8 *pModMap);

void
x386KbdGetMapping (pKeySyms, pModMap)
     KeySymsPtr pKeySyms;
     CARD8      *pModMap;
{
  KeySym        *k;
  char          type;
  int           i, j;

  readKernelMapping(pKeySyms, pModMap);

  /*
   * Apply the special key mapping specified in Xconfig 
   */
  for (k = map, i = MIN_KEYCODE;
       i < (NUM_KEYCODES + MIN_KEYCODE);
       i++, k += 4) {
    switch (k[0]) {
      case XK_Alt_L:
        j = K_INDEX_LEFTALT;
        break;
      case XK_Alt_R:
        j = K_INDEX_RIGHTALT;
        break;
      case XK_Scroll_Lock:
        j = K_INDEX_SCROLLLOCK;
        break;
      case XK_Control_R:
        j = K_INDEX_RIGHTCTL;
        break;
      default:
        j = -1;
    }
    if (j >= 0)
      switch (x386Info.specialKeyMap[j]) {
        case K_META:
          if (k[0] == XK_Alt_R)
            k[1] = XK_Meta_R;
          else {
            k[0] = XK_Alt_L;
            k[1] = XK_Meta_L;
          }
          break;
        case K_COMPOSE:
          k[0] = XK_Multi_key;
          break;
        case K_MODESHIFT:
          k[0] = XK_Mode_switch;
          k[1] = NoSymbol;
          break;
        case K_MODELOCK:
          k[0] = XK_Mode_switch;
          k[1] = XK_Mode_Lock;
          break;
        case K_SCROLLLOCK:
          k[0] = XK_Scroll_Lock;
          break;
        case K_CONTROL:
          k[0] = XK_Control_R;
          break;
      }
  }

  /*
   * compute the modifier map
   */
  for (i = 0; i < MAP_LENGTH; i++)
    pModMap[i] = NoSymbol;  /* make sure it is restored */
  
  for (k = map, i = MIN_KEYCODE;
       i < (NUM_KEYCODES + MIN_KEYCODE);
       i++, k += 4)
    
    switch(*k) {
      
    case XK_Shift_L:
    case XK_Shift_R:
      pModMap[i] = ShiftMask;
      break;
      
    case XK_Control_L:
    case XK_Control_R:
      pModMap[i] = ControlMask;
      break;
      
    case XK_Caps_Lock:
      pModMap[i] = LockMask;
      break;
      
    case XK_Alt_L:
    case XK_Alt_R:
      pModMap[i] = AltMask;
      break;
      
    case XK_Num_Lock:
      if (!x386Info.serverNumLock) pModMap[i] = NumLockMask;
      break;

    case XK_Scroll_Lock:
      pModMap[i] = ScrollLockMask;
      break;

      /* kana support */
    case XK_Kana_Lock:
    case XK_Kana_Shift:
      pModMap[i] = KanaMask;
      break;

      /* alternate toggle for multinational support */
    case XK_Mode_switch:
      pModMap[i] = AltLangMask;
      break;

    }
  
  x386Info.kbdType =
    ioctl(x386Info.consoleFd, KDGKBTYPE, &type) != -1 ? type : KB_101;

  pKeySyms->map        = map;
  pKeySyms->mapWidth   = GLYPHS_PER_KEY;
  pKeySyms->minKeyCode = MIN_KEYCODE;
  pKeySyms->maxKeyCode = MAX_KEYCODE;
}

#undef K_COMPOSE
#include <linux/keyboard.h>

/*
 * As of Linux 0.99.15h, these are not defined in keyboard.h, but
 * Andries Brouwer (aeb@cwi.nl), who's done the work in getting key
 * mapping in the kernel, promised me they'd be in soon.
 */
#ifndef KG_SHIFTL

#define KG_SHIFTL        4
#define KG_SHIFTR        5
#define KG_CTRLL         6
#define KG_CTRLR         7

#define K_SHIFTL         K(KT_SHIFT,KG_SHIFTL)
#define K_SHIFTR         K(KT_SHIFT,KG_SHIFTR)
#define K_CTRLL          K(KT_SHIFT,KG_CTRLL)
#define K_CTRLR          K(KT_SHIFT,KG_CTRLR)

#endif /* KG_SHIFTL */

static KeySym linux_to_x[256] = {
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	XK_BackSpace,	XK_Tab,		XK_Linefeed,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	XK_Escape,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	XK_space,	XK_exclam,	XK_quotedbl,	XK_numbersign,
	XK_dollar,	XK_percent,	XK_ampersand,	XK_apostrophe,
	XK_parenleft,	XK_parenright,	XK_asterisk,	XK_plus,
	XK_comma,	XK_minus,	XK_period,	XK_slash,
	XK_0,		XK_1,		XK_2,		XK_3,
	XK_4,		XK_5,		XK_6,		XK_7,
	XK_8,		XK_9,		XK_colon,	XK_semicolon,
	XK_less,	XK_equal,	XK_greater,	XK_question,
	XK_at,		XK_A,		XK_B,		XK_C,
	XK_D,		XK_E,		XK_F,		XK_G,
	XK_H,		XK_I,		XK_J,		XK_K,
	XK_L,		XK_M,		XK_N,		XK_O,
	XK_P,		XK_Q,		XK_R,		XK_S,
	XK_T,		XK_U,		XK_V,		XK_W,
	XK_X,		XK_Y,		XK_Z,		XK_bracketleft,
	XK_backslash,	XK_bracketright,XK_asciicircum,	XK_underscore,
	XK_grave,	XK_a,		XK_b,		XK_c,
	XK_d,		XK_e,		XK_f,		XK_g,
	XK_h,		XK_i,		XK_j,		XK_k,
	XK_l,		XK_m,		XK_n,		XK_o,
	XK_p,		XK_q,		XK_r,		XK_s,
	XK_t,		XK_u,		XK_v,		XK_w,
	XK_x,		XK_y,		XK_z,		XK_braceleft,
	XK_bar,		XK_braceright,	XK_asciitilde,	XK_Delete,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
	XK_nobreakspace,XK_exclamdown,	XK_cent,	XK_sterling,
	XK_currency,	XK_yen,		XK_brokenbar,	XK_section,
	XK_diaeresis,	XK_copyright,	XK_ordfeminine,	XK_guillemotleft,
	XK_notsign,	XK_hyphen,	XK_registered,	XK_macron,
	XK_degree,	XK_plusminus,	XK_twosuperior,	XK_threesuperior,
	XK_acute,	XK_mu,		XK_paragraph,	XK_periodcentered,
	XK_cedilla,	XK_onesuperior,	XK_masculine,	XK_guillemotright,
	XK_onequarter,	XK_onehalf,	XK_threequarters,XK_questiondown,
	XK_Agrave,	XK_Aacute,	XK_Acircumflex,	XK_Atilde,
	XK_Adiaeresis,	XK_Aring,	XK_AE,		XK_Ccedilla,
	XK_Egrave,	XK_Eacute,	XK_Ecircumflex,	XK_Ediaeresis,
	XK_Igrave,	XK_Iacute,	XK_Icircumflex,	XK_Idiaeresis,
	XK_ETH,		XK_Ntilde,	XK_Ograve,	XK_Oacute,
	XK_Ocircumflex,	XK_Otilde,	XK_Odiaeresis,	XK_multiply,
	XK_Ooblique,	XK_Ugrave,	XK_Uacute,	XK_Ucircumflex,
	XK_Udiaeresis,	XK_Yacute,	XK_THORN,	XK_ssharp,
	XK_agrave,	XK_aacute,	XK_acircumflex,	XK_atilde,
	XK_adiaeresis,	XK_aring,	XK_ae,		XK_ccedilla,
	XK_egrave,	XK_eacute,	XK_ecircumflex,	XK_ediaeresis,
	XK_igrave,	XK_iacute,	XK_icircumflex,	XK_idiaeresis,
	XK_eth,		XK_ntilde,	XK_ograve,	XK_oacute,
	XK_ocircumflex,	XK_otilde,	XK_odiaeresis,	XK_division,
	XK_oslash,	XK_ugrave,	XK_uacute,	XK_ucircumflex,
	XK_udiaeresis,	XK_yacute,	XK_thorn,	XK_ydiaeresis
};

/*
 * Maps the AT keycodes to Linux keycodes
 */
static unsigned char at2lnx[] =
{
	0x01,	/* KEY_Escape */	0x02,	/* KEY_1 */
	0x03,	/* KEY_2 */		0x04,	/* KEY_3 */
	0x05,	/* KEY_4 */		0x06,	/* KEY_5 */
	0x07,	/* KEY_6 */		0x08,	/* KEY_7 */
	0x09,	/* KEY_8 */		0x0a,	/* KEY_9 */
	0x0b,	/* KEY_0 */		0x0c,	/* KEY_Minus */
	0x0d,	/* KEY_Equal */		0x0e,	/* KEY_BackSpace */
	0x0f,	/* KEY_Tab */		0x10,	/* KEY_Q */
	0x11,	/* KEY_W */		0x12,	/* KEY_E */
	0x13,	/* KEY_R */		0x14,	/* KEY_T */
	0x15,	/* KEY_Y */		0x16,	/* KEY_U */
	0x17,	/* KEY_I */		0x18,	/* KEY_O */
	0x19,	/* KEY_P */		0x1a,	/* KEY_LBrace */
	0x1b,	/* KEY_RBrace */	0x1c,	/* KEY_Enter */
	0x1d,	/* KEY_LCtrl */		0x1e,	/* KEY_A */
	0x1f,	/* KEY_S */		0x20,	/* KEY_D */
	0x21,	/* KEY_F */		0x22,	/* KEY_G */
	0x23,	/* KEY_H */		0x24,	/* KEY_J */
	0x25,	/* KEY_K */		0x26,	/* KEY_L */
	0x27,	/* KEY_SemiColon */	0x28,	/* KEY_Quote */
	0x29,	/* KEY_Tilde */		0x2a,	/* KEY_ShiftL */
	0x2b,	/* KEY_BSlash */	0x2c,	/* KEY_Z */
	0x2d,	/* KEY_X */		0x2e,	/* KEY_C */
	0x2f,	/* KEY_V */		0x30,	/* KEY_B */
	0x31,	/* KEY_N */		0x32,	/* KEY_M */
	0x33,	/* KEY_Comma */		0x34,	/* KEY_Period */
	0x35,	/* KEY_Slash */		0x36,	/* KEY_ShiftR */
	0x37,	/* KEY_KP_Multiply */	0x38,	/* KEY_Alt */
	0x39,	/* KEY_Space */		0x3a,	/* KEY_CapsLock */
	0x3b,	/* KEY_F1 */		0x3c,	/* KEY_F2 */
	0x3d,	/* KEY_F3 */		0x3e,	/* KEY_F4 */
	0x3f,	/* KEY_F5 */		0x40,	/* KEY_F6 */
	0x41,	/* KEY_F7 */		0x42,	/* KEY_F8 */
	0x43,	/* KEY_F9 */		0x44,	/* KEY_F10 */
	0x45,	/* KEY_NumLock */	0x46,	/* KEY_ScrollLock */
	0x47,	/* KEY_KP_7 */		0x48,	/* KEY_KP_8 */
	0x49,	/* KEY_KP_9 */		0x4a,	/* KEY_KP_Minus */
	0x4b,	/* KEY_KP_4 */		0x4c,	/* KEY_KP_5 */
	0x4d,	/* KEY_KP_6 */		0x4e,	/* KEY_KP_Plus */
	0x4f,	/* KEY_KP_1 */		0x50,	/* KEY_KP_2 */
	0x51,	/* KEY_KP_3 */		0x52,	/* KEY_KP_0 */
	0x53,	/* KEY_KP_Decimal */	0x54,	/* KEY_SysReqest */
	0x55,	/* unused */		0x56,	/* KEY_Less */
	0x57,	/* KEY_F11 */		0x58,	/* KEY_F12 */
	0x66,	/* KEY_Home */		0x67,	/* KEY_Up */
	0x68,	/* KEY_PgUp */		0x69,	/* KEY_Left */
	0x5d,	/* KEY_Begin */		0x6a,	/* KEY_Right */
	0x6b,	/* KEY_End */		0x6c,	/* KEY_Down */
	0x6d,	/* KEY_PgDown */	0x6e,	/* KEY_Insert */
	0x6f,	/* KEY_Delete */	0x60,	/* KEY_KP_Enter */
	0x61,	/* KEY_RCtrl */		0x77,	/* KEY_Pause */
	0x63,	/* KEY_Print */		0x62,	/* KEY_KP_Divide */
	0x64,	/* KEY_AltLang */	0x65	/* KEY_Break */
};
#define NUM_AT2LNX (sizeof(at2lnx) / sizeof(at2lnx[0]))

static void
readKernelMapping(KeySymsPtr pKeySyms, CARD8 *pModMap)
{
  KeySym        *k;
  char          type;
  int           i, j;
  static unsigned char tbl[GLYPHS_PER_KEY] =
  {
	0,	/* unshifted */
	1,	/* shifted */
	0,	/* modeswitch unshifted */
	0	/* modeswitch shifted */
  };

  for (k = map, i = GLYPHS_PER_KEY * NUM_KEYCODES; i--; )
	  *k++ = NoSymbol;

  /*
   * Read the mapping from the kernel.
   * Since we're still using the XFree86 scancode->AT keycode mapping
   * routines, we need to convert the AT keycodes to Linux keycodes,
   * then translate the Linux keysyms into X keysyms.
   *
   * First, figure out which tables to use for the modeswitch columns
   * above, from the Xconfig fields.
   */
  if (x386Info.specialKeyMap[K_INDEX_RIGHTCTL] == K_MODESHIFT ||
      x386Info.specialKeyMap[K_INDEX_RIGHTCTL] == K_MODELOCK)
    tbl[2] = 4;	/* control */
  else if (x386Info.specialKeyMap[K_INDEX_RIGHTALT] == K_MODESHIFT ||
           x386Info.specialKeyMap[K_INDEX_RIGHTALT] == K_MODELOCK)
    tbl[2] = 2;	/* AltGr */
  else
    tbl[2] = 8;	/* alt */
  tbl[3] = tbl[2] | 1;

  for (i = 0, k = map+GLYPHS_PER_KEY; i < NUM_AT2LNX; ++i)
  {
    struct kbentry kbe;
    int j;

    kbe.kb_index = at2lnx[i];
    for (j = 0; j < GLYPHS_PER_KEY; ++j, ++k)
    {
      unsigned short kval;

      kbe.kb_table = tbl[j];
      if (ioctl(x386Info.consoleFd, KDGKBENT, &kbe))
	continue;

      kval = KVAL(kbe.kb_value);
      switch (KTYP(kbe.kb_value))
      {
      case KT_LATIN:
      case KT_LETTER:
	*k = linux_to_x[kval];
	break;

      case KT_FN:
	if (kval <= 19)
	  *k = XK_F1 + kval;
	else switch (kbe.kb_value)
	{
	case K_FIND:
	  *k = XK_Home; /* or XK_Find */
	  break;
	case K_INSERT:
	  *k = XK_Insert;
	  break;
	case K_REMOVE:
	  *k = XK_Delete;
	  break;
	case K_SELECT:
	  *k = XK_End; /* or XK_Select */
	  break;
	case K_PGUP:
	  *k = XK_Prior;
	  break;
	case K_PGDN:
	  *k = XK_Next;
	  break;
	case K_HELP:
	  *k = XK_Help;
	  break;
	case K_DO:
	  *k = XK_Execute;
	  break;
	case K_PAUSE:
	  *k = XK_Pause;
	  break;
	case K_MACRO:
	  *k = XK_Menu;
	  break;
	default:
	  break;
	}
	break;

      case KT_SPEC:
	switch (kbe.kb_value)
	{
	case K_ENTER:
	  *k = XK_Return;
	  break;
	case K_BREAK:
	  *k = XK_Break;
	  break;
	case K_CAPS:
	  *k = XK_Caps_Lock;
	  break;
	case K_NUM:
	  *k = XK_Num_Lock;
	  break;
	case K_HOLD:
	  *k = XK_Scroll_Lock;
	  break;
	case K_COMPOSE:
          *k = XK_Multi_key;
	  break;
	default:
	  break;
	}
	break;

      case KT_PAD:
	switch (kbe.kb_value)
	{
	case K_PPLUS:
	  *k = XK_KP_Add;
	  break;
	case K_PMINUS:
	  *k = XK_KP_Subtract;
	  break;
	case K_PSTAR:
	  *k = XK_KP_Multiply;
	  break;
	case K_PSLASH:
	  *k = XK_KP_Divide;
	  break;
	case K_PENTER:
	  *k = XK_KP_Enter;
	  break;
	case K_PCOMMA:
	  *k = XK_KP_Separator;
	  break;
	case K_PDOT:
	  *k = XK_KP_Decimal;
	  break;
	default:
	  if (kval <= 9)
	    *k = XK_KP_0 + kval;
	  break;
	}
	break;

      /*
       * KT_DEAD keys are for accelerated diacritical creation.
       * We should generate "XK_Compose XK_xxx" but can only map to one
       * keysym.
       */
      case KT_DEAD:
	break;

      case KT_CUR:
	switch (kbe.kb_value)
	{
	case K_DOWN:
	  *k = XK_Down;
	  break;
	case K_LEFT:
	  *k = XK_Left;
	  break;
	case K_RIGHT:
	  *k = XK_Right;
	  break;
	case K_UP:
	  *k = XK_Up;
	  break;
	}
	break;

      case KT_SHIFT:
	switch (kbe.kb_value)
	{
	case K_ALTGR:
	  *k = XK_Alt_R;
	  break;
	case K_ALT:
	  *k = (kbe.kb_index == 0x64 ?
		XK_Alt_R : XK_Alt_L);
	  break;
	case K_CTRL:
	  *k = (kbe.kb_index == 0x61 ?
		XK_Control_R : XK_Control_L);
	  break;
        case K_CTRLL:
	  *k = XK_Control_L;
	  break;
        case K_CTRLR:
	  *k = XK_Control_R;
	  break;
	case K_SHIFT:
	  *k = (kbe.kb_index == 0x36 ?
		XK_Shift_R : XK_Shift_L);
	  break;
        case K_SHIFTL:
	  *k = XK_Shift_L;
	  break;
        case K_SHIFTR:
	  *k = XK_Shift_R;
	  break;
	default:
	  break;
	}
	break;

      /*
       * KT_ASCII keys accumulate a 3 digit decimal number that gets
       * emitted when the shift state changes. We can't emulate that.
       */
      case KT_ASCII:
	break;

      case KT_LOCK:
	if (kbe.kb_value == K_SHIFTLOCK)
	  *k = XK_Shift_Lock;
	break;

      default:
	break;
      }
    }

    if (k[-1] == k[-2]) k[-1] = NoSymbol;
    if (k[-2] == k[-3]) k[-2] = NoSymbol;
    if (k[-3] == k[-4]) k[-3] = NoSymbol;
    if (k[-4] == k[-2] && k[-3] == k[-1]) k[-2] = k[-1] = NoSymbol;
    if (k[-1] == k[-4] && k[-2] == k[-3] && k[-2] == NoSymbol) k[-1] =NoSymbol;
  }
}
