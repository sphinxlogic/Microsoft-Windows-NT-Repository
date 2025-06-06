/*		Structured stream to Rich hypertext converter
**		============================================
**
**	This generates a hypertext object.  It converts from the
**	structured stream interface of HTML events into the style-
**	oriented interface of the HText.h interface.  This module is
**	only used in clients and should not be linked into servers.
**
**	Override this module if making a new GUI browser.
**
**   Being Overidden
**
*/

#include <HTUtils.h>

#define Lynx_HTML_Handler
#include <HTChunk.h>
#include <HText.h>
#include <HTStyle.h>
#include <HTML.h>

#include <HTCJK.h>
#include <HTAtom.h>
#include <HTAnchor.h>
#include <HTMLGen.h>
#include <HTParse.h>
#include <HTList.h>
#include <UCMap.h>
#include <UCDefs.h>
#include <UCAux.h>

#include <LYGlobalDefs.h>
#include <LYCharUtils.h>
#include <LYCharSets.h>

#include <HTAlert.h>
#include <HTForms.h>
#include <HTNestedList.h>
#include <GridText.h>
#include <LYStrings.h>
#include <LYUtils.h>
#include <LYMap.h>
#include <LYList.h>
#include <LYBookmark.h>

#ifdef VMS
#include <LYCurses.h>
#endif /* VMS */

#ifdef USE_PRETTYSRC
#include <LYPrettySrc.h>
#endif

#ifdef USE_COLOR_STYLE
#include <SGML.h>
#include <AttrList.h>
#include <LYHash.h>
#include <LYStyle.h>
#undef SELECTED_STYLES
#define pHText_changeStyle(X,Y,Z) {}

#define OPT_SCN 1
#define OMIT_SCN_KEEPING 0 /* whether to omit keeping of Style_className
    when lss support is on. 1 to increase performance. The value must
    correspond to one in LYCurses.c. Should be 0 if OPT_SCN=0 */

#if OMIT_SCN_KEEPING
# define HCODE_TO_STACK_OFF(x) /*(CSHASHSIZE+1)*/ 88888  /*special value.*/
#else
# define HCODE_TO_STACK_OFF(x) x /*pass computed value*/
#endif

#endif /* USE_COLOR_STYLE */

#ifdef SOURCE_CACHE
#include <HTAccess.h>
#endif

#include <LYJustify.h>

#include <LYexit.h>
#include <LYLeaks.h>

#define STACKLEVEL(me) ((me->stack + MAX_NESTING - 1) - me->sp)

extern BOOL HTPassEightBitRaw;

extern BOOLEAN HT_Is_Gopher_URL;

/* from Curses.h */
extern int LYcols;

struct _HTStream {
    CONST HTStreamClass *	isa;
#ifdef SOURCE_CACHE
    HTParentAnchor *		anchor;
    FILE *			fp;
    char *			filename;
    HTChunk *			chunk;
    CONST HTStreamClass *	actions;
    HTStream *			target;
    int				status;
#else
    /* .... */
#endif
};

PRIVATE HTStyleSheet * styleSheet = NULL;	/* Application-wide */

/*	Module-wide style cache
*/
PRIVATE HTStyle *styles[HTML_ELEMENTS+LYNX_HTML_EXTRA_ELEMENTS];
					   /* adding 24 nested list styles  */
					   /* and 3 header alignment styles */
					   /* and 3 div alignment styles    */
PRIVATE HTStyle *default_style = NULL;

PUBLIC char *LYToolbarName = "LynxPseudoToolbar";

/* used to turn off a style if the HTML author forgot to
PRIVATE int i_prior_style = -1;
 */

/*
 *	Private function....
 */
PRIVATE int HTML_end_element PARAMS((HTStructured *me,
				      int element_number,
				      char **include));

PRIVATE int HTML_start_element PARAMS((
	HTStructured *		me,
	int			element_number,
	CONST BOOL*		present,
	CONST char **		value,
	int			tag_charset,
	char **			include));

/*
 * If we have verbose_img set, display labels for images.
 */
#define VERBOSE_IMG(value,src_type,string) \
      ((verbose_img) ? (newtitle = MakeNewTitle(value,src_type)): string)

PRIVATE char* MakeNewTitle PARAMS((CONST char ** value, int src_type));
PRIVATE char* MakeNewImageValue PARAMS((CONST char ** value));
PRIVATE char* MakeNewMapValue PARAMS((CONST char ** value, CONST char* mapstr));

/*	Set an internal flag that the next call to a stack-affecting method
**	is only internal and the stack manipulation should be skipped. - kw
*/
#define SET_SKIP_STACK(el_num) if (HTML_dtd.tags[el_num].contents != SGML_EMPTY) \
						{ me->skip_stack++; }

extern int hash_code PARAMS((char* i));

PUBLIC void strtolower ARGS1(char*, i)
{
    if (!i) return;
    while (*i) { *i=(char)tolower(*i); i++; }
}

/*		Flattening the style structure
**		------------------------------
**
On the NeXT, and on any read-only browser, it is simpler for the text to have
a sequence of styles, rather than a nested tree of styles.  In this
case we have to flatten the structure as it arrives from SGML tags into
a sequence of styles.
*/

/*
**  If style really needs to be set, call this.
*/
PUBLIC void actually_set_style ARGS1(HTStructured *, me)
{
    if (!me->text) {			/* First time through */
	LYGetChartransInfo(me);
	UCSetTransParams(&me->T,
		     me->UCLYhndl, me->UCI,
			 HTAnchor_getUCLYhndl(me->node_anchor,
					      UCT_STAGE_HTEXT),
			 HTAnchor_getUCInfoStage(me->node_anchor,
						 UCT_STAGE_HTEXT));
	me->text = HText_new2(me->node_anchor, me->target);
	HText_beginAppend(me->text);
	HText_setStyle(me->text, me->new_style);
	me->in_word = NO;
	LYCheckForContentBase(me);
    } else {
	HText_setStyle(me->text, me->new_style);
    }

    me->old_style = me->new_style;
    me->style_change = NO;
}

/*
**  If you THINK you need to change style, call this.
*/
PRIVATE void change_paragraph_style ARGS2(HTStructured *, me, HTStyle *,style)
{
    if (me->new_style != style) {
	me->style_change = YES;
	me->new_style = style;
    }
    me->in_word = NO;
}

PUBLIC BOOL LYBadHTML ARGS1(
    HTStructured *,	me)
{
    if (!TRACE && !me->inBadHTML) {
	HTUserMsg(BAD_HTML_USE_TRACE);
	me->inBadHTML = TRUE;
	return FALSE;
    }
    return TRUE;
}

/*_________________________________________________________________________
**
**			A C T I O N	R O U T I N E S
*/

/* FIXME:  this should be amended to do the substitution only when not in a
 * multibyte stream.
 */
#ifdef EXP_JAPANESE_SPACES
#define FIX_JAPANESE_SPACES \
	(HTCJK == CHINESE || HTCJK == JAPANESE || HTCJK == TAIPEI)
	/* don't replace '\n' with ' ' if Chinese or Japanese - HN
	 */
#else
#define FIX_JAPANESE_SPACES 0
#endif

/*	Character handling
**	------------------
*/
PUBLIC void HTML_put_character ARGS2(HTStructured *, me, char, c)
{
#if 0 /* def KANJI_CODE_OVERRIDE */
    static unsigned char save_ch1 = 0;
    static unsigned char save_ch2 = 0;
#endif
    /*
     *	Ignore all non-MAP content when just
     *	scanning a document for MAPs. - FM
     */
    if (LYMapsOnly && me->sp[0].tag_number != HTML_OBJECT)
	return;

    /*
     *	Do EOL conversion if needed. - FM
     *
     *	Convert EOL styles:
     *	 macintosh:  cr    --> lf
     *	 ascii:      cr-lf --> lf
     *	 unix:	     lf    --> lf
     */
    if ((me->lastraw == '\r') && c == '\n') {
	me->lastraw = -1;
	return;
    }
    me->lastraw = c;
    if (c == '\r')
	c = '\n';

    /*
     *	Handle SGML_LITTERAL tags that have HTChunk elements. - FM
     */
    switch (me->sp[0].tag_number) {

    case HTML_COMMENT:
	return; /* Do Nothing */

    case HTML_TITLE:
	if (c == LY_SOFT_HYPHEN)
	    return;
	if (c != '\n' && c != '\t' && c != '\r') {
	    HTChunkPutc(&me->title, c);
	} else if (FIX_JAPANESE_SPACES) {
	    if (c == '\t') {
		HTChunkPutc(&me->title, ' ');
	    } else {
		return;
	    }
	} else {
	    HTChunkPutc(&me->title, ' ');
	}
	return;

    case HTML_STYLE:
	HTChunkPutc(&me->style_block, c);
	return;

    case HTML_SCRIPT:
	HTChunkPutc(&me->script, c);
	return;

    case HTML_OBJECT:
	HTChunkPutc(&me->object, c);
	return;

    case HTML_TEXTAREA:
	HTChunkPutc(&me->textarea, c);
	return;

    case HTML_SELECT:
    case HTML_OPTION:
	HTChunkPutc(&me->option, c);
	return;

    case HTML_MATH:
	HTChunkPutc(&me->math, c);
	return;

    default:
	if (me->inSELECT) {
	    /*
	     *	If we are within a SELECT not caught by the cases
	     *	above - HTML_SELECT or HTML_OPTION may not be the
	     *	last element pushed on the style stack if there were
	     *	invalid markup tags within a SELECT element.  For error
	     *	recovery, treat text as part of the OPTION text, it is
	     *	probably meant to show up as user-visible text.
	     *	Having A as an open element while in SELECT is really sick,
	     *	don't make anchor text part of the option text in that case
	     *	since the option text will probably just be discarded. - kw
	     */
	    if (me->sp[0].tag_number == HTML_A)
		break;
	    HTChunkPutc(&me->option, c);
	    return;
	}
	break;
    } /* end first switch */

    /*
     *	Handle all other tag content. - FM
     */
    switch (me->sp[0].tag_number) {

    case HTML_PRE:				/* Formatted text */
	/*
	 *  We guarantee that the style is up-to-date in begin_litteral
	 *  But we still want to strip \r's
	 */
	if (c != '\r' &&
	    !(c == '\n' && me->inLABEL && !me->inP) &&
	    !(c == '\n' && !me->inPRE)) {
	    me->inP = TRUE;
	    me->inLABEL = FALSE;
	    HText_appendCharacter(me->text, c);
	}
	me->inPRE = TRUE;
	break;

    case HTML_LISTING:				/* Literal text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
	/*
	 *  We guarantee that the style is up-to-date in begin_litteral
	 *  But we still want to strip \r's
	 */
	if (c != '\r')	{
	    me->inP = TRUE;
	    me->inLABEL = FALSE;
	    HText_appendCharacter(me->text, c);
	}
	break;

    default:
	/*
	 *  Free format text.
	 */
	if (!strcmp(me->sp->style->name,"Preformatted")) {
	    if (c != '\r' &&
		!(c == '\n' && me->inLABEL && !me->inP) &&
		!(c == '\n' && !me->inPRE)) {
		me->inP = TRUE;
		me->inLABEL = FALSE;
		HText_appendCharacter(me->text, c);
	    }
	    me->inPRE = TRUE;

	} else if (!strcmp(me->sp->style->name,"Listing") ||
		   !strcmp(me->sp->style->name,"Example")) {
	    if (c != '\r') {
		me->inP = TRUE;
		me->inLABEL = FALSE;
		HText_appendCharacter(me->text, c);
	    }

	} else {
	    if (me->style_change) {
		if ((c == '\n') || (c == ' '))
		    return;	/* Ignore it */
		UPDATE_STYLE;
	    }
	    if (c == '\n') {
		if (!FIX_JAPANESE_SPACES) {
		    if (me->in_word) {
			if (HText_getLastChar(me->text) != ' ') {
			    me->inP = TRUE;
			    me->inLABEL = FALSE;
			    HText_appendCharacter(me->text, ' ');
			}
			me->in_word = NO;
		    }
		}

	    } else if (c == ' ' || c == '\t') {
		if (HText_getLastChar(me->text) != ' ') {
		    me->inP = TRUE;
		    me->inLABEL = FALSE;
		    HText_appendCharacter(me->text, ' ');
		}

	    } else if (c == '\r') {
	       /* ignore */

	    } else {
		me->inP = TRUE;
		me->inLABEL = FALSE;
#if 0 /* Should this check be done in HText_appendCharacter? */
		if (last_kcode == EUC) {
		    if (save_ch1 && !save_ch2) {
			if (UCH(c) & 0x80) {
			    save_ch2 = c;
			}
			HText_appendCharacter(me->text, save_ch1);
			HText_appendCharacter(me->text, save_ch2);
			save_ch1 = save_ch2 = '\0';
		    } else if (UCH(c) & 0x80) {
			save_ch1 = c;
			save_ch2 = '\0';
		    } else {
			HText_appendCharacter(me->text, c);
		    }
		} else {
		    HText_appendCharacter(me->text, c);
		}
#endif
		HText_appendCharacter(me->text, c);
		me->in_word = YES;
	    }
	}
    } /* end second switch */

    if (c == '\n' || c == '\t') {
	HText_setLastChar(me->text, ' '); /* set it to a generic separator */

	/*
	 *  \r's are ignored.  In order to keep collapsing spaces
	 *  correctly we must default back to the previous
	 *  separator if there was one
	 */
    } else if (c == '\r' && HText_getLastChar(me->text) == ' ') {
	HText_setLastChar(me->text, ' '); /* set it to a generic separator */
    } else {
	HText_setLastChar(me->text, c);
    }
}

/*	String handling
**	---------------
**
**	This is written separately from put_character because the loop can
**	in some cases be promoted to a higher function call level for speed.
*/
PUBLIC void HTML_put_string ARGS2(HTStructured *, me, CONST char *, s)
{
#ifdef USE_PRETTYSRC
    char* translated_string = NULL;
#endif

    if (s == NULL || (LYMapsOnly && me->sp[0].tag_number != HTML_OBJECT))
	return;
#ifdef USE_PRETTYSRC
    if (psrc_convert_string) {
	StrAllocCopy(translated_string,s);
	TRANSLATE_AND_UNESCAPE_ENTITIES(&translated_string, TRUE, FALSE);
	s = (CONST char *) translated_string;
    }
#endif

    switch (me->sp[0].tag_number) {

    case HTML_COMMENT:
	break;					/* Do Nothing */

    case HTML_TITLE:
	HTChunkPuts(&me->title, s);
	break;

    case HTML_STYLE:
	HTChunkPuts(&me->style_block, s);
	break;

    case HTML_SCRIPT:
	HTChunkPuts(&me->script, s);
	break;

    case HTML_PRE:				/* Formatted text */
    case HTML_LISTING:				/* Literal text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
	/*
	 *  We guarantee that the style is up-to-date in begin_litteral
	 */
	HText_appendText(me->text, s);
	break;

    case HTML_OBJECT:
	HTChunkPuts(&me->object, s);
	break;

    case HTML_TEXTAREA:
	HTChunkPuts(&me->textarea, s);
	break;

    case HTML_SELECT:
    case HTML_OPTION:
	HTChunkPuts(&me->option, s);
	break;

    case HTML_MATH:
	HTChunkPuts(&me->math, s);
	break;

    default:					/* Free format text? */
	if (!me->sp->style->freeFormat) {
	    /*
	     *	If we are within a preformatted text style not caught
	     *	by the cases above (HTML_PRE or similar may not be the
	     *	last element pushed on the style stack). - kw
	     */
#ifdef USE_PRETTYSRC
	    if (psrc_view) {
		/*
		 * We do this so that a raw '\r' in the string will not be
		 * interpreted as an internal request to break a line - passing
		 * '\r' to HText_appendText is treated by it as a request to
		 * insert a blank line - VH
		 */
		for(; *s; ++s)
		    HTML_put_character(me, *s);
	    } else
#endif
	    HText_appendText(me->text, s);
	    break;
	} else {
	    CONST char *p = s;
	    char c;
	    if (me->style_change) {
		for (; *p && ((*p == '\n') || (*p == '\r') ||
			      (*p == ' ') || (*p == '\t')); p++)
		    ;	/* Ignore leaders */
		if (!*p)
		    break;
		UPDATE_STYLE;
	    }
	    for (; *p; p++) {
		if (*p == 13 && p[1] != 10) {
		    /*
		     *	Treat any '\r' which is not followed by '\n'
		     *	as '\n', to account for macintosh lineend in
		     *	ALT attributes etc. - kw
		     */
		    c = '\n';
		} else {
		    c = *p;
		}
		if (me->style_change) {
		    if ((c == '\n') || (c == ' ') || (c == '\t'))
			continue;  /* Ignore it */
		    UPDATE_STYLE;
		}
		if (c == '\n') {
		    if (!FIX_JAPANESE_SPACES) {
			if (me->in_word) {
			    if (HText_getLastChar(me->text) != ' ')
				HText_appendCharacter(me->text, ' ');
			    me->in_word = NO;
			}
		    }

		} else if (c == ' ' || c == '\t') {
		   if (HText_getLastChar(me->text) != ' ')
			HText_appendCharacter(me->text, ' ');

		} else if (c == '\r') {
			/* ignore */
		} else {
		    HText_appendCharacter(me->text, c);
		    me->in_word = YES;
		}

		/* set the Last Character */
		if (c == '\n' || c == '\t') {
		    /* set it to a generic separator */
		    HText_setLastChar(me->text, ' ');
		} else if (c == '\r' &&
			   HText_getLastChar(me->text) == ' ') {
		    /*
		     *	\r's are ignored.  In order to keep collapsing
		     *	spaces correctly, we must default back to the
		     *	previous separator, if there was one.  So we
		     *	set LastChar to a generic separator.
		     */
		    HText_setLastChar(me->text, ' ');
		} else {
		    HText_setLastChar(me->text, c);
		}

	    } /* for */
	}
    } /* end switch */
#ifdef USE_PRETTYSRC
    if (psrc_convert_string) {
	psrc_convert_string = FALSE;
	FREE(translated_string);
    }
#endif
}

/*	Buffer write
**	------------
*/
PUBLIC void HTML_write ARGS3(HTStructured *, me, CONST char*, s, int, l)
{
    CONST char* p;
    CONST char* e = s+l;

    if (LYMapsOnly && me->sp[0].tag_number != HTML_OBJECT)
	return;

    for (p = s; p < e; p++)
	HTML_put_character(me, *p);
}

/*
 *  "Internal links" are hyperlinks whose source and destination are
 *  within the same document, and for which the destination is given
 *  as a URL Reference with an empty URL, but possibly with a non-empty
 *  #fragment.	(This terminology re URL-Reference vs. URL follows the
 *  Fielding URL syntax and semantics drafts).
 *  Differences:
 *  (1) The document's base (in whatever way it is given) is not used for
 *	resolving internal link references.
 *  (2) Activating an internal link should not result in a new retrieval
 *	of a copy of the document.
 *  (3) Internal links are the only way to refer with a hyperlink to a document
 *	(or a location in it) which is only known as the result of a POST
 *	request (doesn't have a URL from which the document can be retrieved
 *	with GET), and can only be used from within that document.
 *
 * *If DONT_TRACK_INTERNAL_LINKS is not defined, we keep track of whether a
 *  link destination was given as an internal link.  This information is
 *  recorded in the type of the link between anchor objects, and is available
 *  to the HText object and the mainloop from there.  URL References to
 *  internal destinations are still resolved into an absolute form before
 *  being passed on, but using the current stream's retrieval address instead
 *  of the base URL.
 *  Examples:  (replace [...] to have a valid absolute URL)
 *  In document retrieved from [...]/mypath/mydoc.htm w/ base [...]/otherpath/
 *  a. HREF="[...]/mypath/mydoc.htm"	  -> [...]/mypath/mydoc.htm
 *  b. HREF="[...]/mypath/mydoc.htm#frag" -> [...]/mypath/mydoc.htm#frag
 *  c. HREF="mydoc.htm"			  -> [...]/otherpath/mydoc.htm
 *  d. HREF="mydoc.htm#frag"		  -> [...]/otherpath/mydoc.htm#frag
 *  e. HREF=""		      -> [...]/mypath/mydoc.htm      (marked internal)
 *  f. HREF="#frag"	      -> [...]/mypath/mydoc.htm#frag (marked internal)
 *
 * *If DONT_TRACK_INTERNAL_LINKS is defined, URL-less URL-References are
 *  resolved differently from URL-References with a non-empty URL (using the
 *  current stream's retrieval address instead of the base), but we make no
 *  further distinction.  Resolution is then as in the examples above, execept
 *  that there is no "(marked internal)".
 *
 * *Note that this doesn't apply to form ACTIONs (always resolved using base,
 *  never marked internal).  Also other references encountered or generated
 *  are not marked internal, whether they have a URL or not, if in a given
 *  context an internal link makes no sense (e.g., IMG SRC=).
 */

#ifndef DONT_TRACK_INTERNAL_LINKS
/* A flag is used to keep track of whether an "URL reference" encountered
   had a real "URL" or not.  In the latter case, it will be marked as
   "internal".	The flag is set before we start messing around with the
   string (resolution of relative URLs etc.).  This variable only used
   locally here, don't confuse with LYinternal_flag which is for
   overriding non-caching similar to LYoverride_no_cache. - kw */
#define CHECK_FOR_INTERN(flag,s) flag = (s && (*s=='#' || *s=='\0')) ? TRUE : FALSE

/* Last argument to pass to HTAnchor_findChildAndLink() calls,
   just an abbreviation. - kw */
#define INTERN_LT (HTLinkType *)(intern_flag ? LINK_INTERNAL : NULL)

#else  /* !DONT_TRACK_INTERNAL_LINKS */

#define CHECK_FOR_INTERN(flag,s)  /* do nothing */ ;
#define INTERN_LT (HTLinkType *)NULL

#endif /* DONT_TRACK_INTERNAL_LINKS */

#ifdef USE_COLOR_STYLE
# if !OPT_SCN
static char* Style_className = NULL;
static char myHash[128];
# else
PRIVATE char Style_className[4096];/* i hope it's enough :) HV */
PRIVATE char* Style_className_end=Style_className;/*this points to the
		    end of string in Style_className (it points to '\0') */
# endif
static int hcode;
#endif


#ifdef USE_PRETTYSRC

PRIVATE void HTMLSRC_apply_markup ARGS4(
	    HTStructured *,   context,
	    HTlexeme,	      lexeme,
	    BOOL,	      start,
	    int,	      tag_charset)
{
    HT_tagspec* ts = *( ( start ? lexeme_start : lexeme_end ) + lexeme);

    while (ts) {
#ifdef USE_COLOR_STYLE
	if (ts->start) {
	    current_tag_style = ts->style;
	    force_current_tag_style = TRUE;
	    forced_classname = ts->class_name;
	    force_classname = TRUE;
	}
#endif
	CTRACE((tfp,ts->start ? "SRCSTART %d\n" : "SRCSTOP %d\n",(int)lexeme));
	if (ts->start)
	    HTML_start_element(
		context,
		ts->element,
		ts->present,
		(CONST char **)ts->value,
		tag_charset,
		NULL);
	else
	    HTML_end_element(
		context,
		ts->element,
		NULL);
	ts = ts->next;
    }
}
#  define START TRUE
#  define STOP FALSE

#if defined(__STDC__) || _WIN_CC
#  define PSRCSTART(x)	HTMLSRC_apply_markup(me,HTL_##x,START,tag_charset)
#  define PSRCSTOP(x)  HTMLSRC_apply_markup(me,HTL_##x,STOP,tag_charset)
#else
#  define PSRCSTART(x)	HTMLSRC_apply_markup(me,HTL_/**/x,START,tag_charset)
#  define PSRCSTOP(x)  HTMLSRC_apply_markup(me,HTL_/**/x,STOP,tag_charset)
#endif

#  define PUTC(x) HTML_put_character(me,x)
#  define PUTS(x) HTML_put_string(me,x)

#endif /* USE_PRETTYSRC*/

PRIVATE void LYStartArea ARGS5(
	HTStructured *,		obj,
	CONST char *,		href,
	CONST char *,		alt,
	CONST char *,		title,
	int,			tag_charset)
{
    BOOL		new_present[HTML_AREA_ATTRIBUTES];
    CONST char *		new_value[HTML_AREA_ATTRIBUTES];
    int i;

    for (i = 0; i < HTML_AREA_ATTRIBUTES; i++)
	 new_present[i] = NO;

    if (alt) {
	new_present[HTML_AREA_ALT] = YES;
	new_value[HTML_AREA_ALT] = (CONST char *)alt;
    }
    if (title && *title) {
	new_present[HTML_AREA_TITLE] = YES;
	new_value[HTML_AREA_TITLE] = (CONST char *)title;
    }
    if (href) {
	new_present[HTML_AREA_HREF] = YES;
	new_value[HTML_AREA_HREF] = (CONST char *)href;
    }

    (*obj->isa->start_element)(obj, HTML_AREA, new_present, new_value,
			       tag_charset, 0);
}

PRIVATE void LYHandleFIG ARGS10(
	HTStructured *,		me,
	CONST BOOL*,		present,
	CONST char **,		value,
	BOOL,			isobject,
	BOOL,			imagemap,
	CONST char *,		id,
	CONST char *,		src,
	BOOL,			convert,
	BOOL,			start,
	BOOL *,			intern_flag GCC_UNUSED)
{
    if (start == TRUE) {
	me->inFIG = TRUE;
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, NULL);
	}
	if (!isobject) {
	    LYEnsureDoubleSpace(me);
	    LYResetParagraphAlignment(me);
	    me->inFIGwithP = TRUE;
	} else {
	    me->inFIGwithP = FALSE;
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	}
	if (id && *id) {
	    if (present && convert) {
		CHECK_ID(HTML_FIG_ID);
	    } else
		LYHandleID(me, id);
	}
	me->in_word = NO;
	me->inP = FALSE;

	if (clickable_images && src && src != '\0') {
	    char *href = NULL;
	    StrAllocCopy(href, src);
	    CHECK_FOR_INTERN(*intern_flag,href);
	    LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    if (*href) {
		char *temp = NULL;
		/*
		 *  Check whether a base tag is in effect. - FM
		 */
		if ((me->inBASE && *href != '#') &&
		    (temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		    *temp != '\0')
		    /*
		     *	Use reference related to the base.
		     */
		    StrAllocCopy(href, temp);
		FREE(temp);

		/*
		 *  Check whether to fill in localhost. - FM
		 */
		LYFillLocalFileURL(&href,
				   ((*href != '#' &&
				     me->inBASE) ?
				   me->base_href : me->node_anchor->address));

		me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					INTERN_LT);		/* Type */
		HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		HTML_put_string(me, (isobject ?
		      (imagemap ? "(IMAGE)" : "(OBJECT)") : "[FIGURE]"));
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, 0);
		HTML_put_character(me, '-');
		HTML_put_character(me, ' '); /* space char may be ignored */
		me->in_word = NO;
	    }
	    FREE(href);
	}
    } else {			/* handle end tag */
	if (me->inFIGwithP) {
	    LYEnsureDoubleSpace(me);
	} else {
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	}
	LYResetParagraphAlignment(me);
	me->inFIGwithP = FALSE;
	me->inFIG = FALSE;
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	if (me->List_Nesting_Level >= 0) {
	    UPDATE_STYLE;
	    HText_NegateLineOne(me->text);
	}
    }
}

PRIVATE void clear_objectdata ARGS1(
	HTStructured *,		me)
{
    if (me) {
	HTChunkClear(&me->object);
	me->object_started = FALSE;
	me->object_declare = FALSE;
	me->object_shapes = FALSE;
	me->object_ismap = FALSE;
	FREE(me->object_usemap);
	FREE(me->object_id);
	FREE(me->object_title);
	FREE(me->object_data);
	FREE(me->object_type);
	FREE(me->object_classid);
	FREE(me->object_codebase);
	FREE(me->object_codetype);
	FREE(me->object_name);
    }
}

/*	Start Element
**	-------------
*/
PRIVATE int HTML_start_element ARGS6(
	HTStructured *,		me,
	int,			element_number,
	CONST BOOL*,		present,
	CONST char **,		value,
	int,			tag_charset,
	char **,		include)
{
    char *alt_string = NULL;
    char *id_string = NULL;
    char *newtitle = NULL;
    char **pdoctitle = NULL;
    char *href = NULL;
    char *map_href = NULL;
    char *title = NULL;
    char *I_value = NULL;
    char *I_name = NULL;
    char *temp = NULL;
    int dest_char_set = -1;
    HTParentAnchor *dest = NULL;	     /* An anchor's destination */
    BOOL dest_ismap = FALSE;		     /* Is dest an image map script? */
    BOOL UseBASE = TRUE;		     /* Resolved vs. BASE if present? */
    HTChildAnchor *ID_A = NULL;		     /* HTML_foo_ID anchor */
    int url_type = 0, i = 0;
    char *cp = NULL;
    HTMLElement ElementNumber = element_number;
    BOOL intern_flag = FALSE;
    short stbl_align = HT_ALIGN_NONE;
    int status = HT_OK;
#ifdef USE_COLOR_STYLE
    char* class_name;
    int class_used = 0;
#  if OPT_SCN
#    if !OMIT_SCN_KEEPING
    char* Style_className_end_was = Style_className_end+1;
#    endif
    /* assume ';' will be appended*/
#  endif
#endif

#ifdef USE_PRETTYSRC
    if (psrc_view && !sgml_in_psrc_was_initialized) {
	if (!psrc_nested_call) {
	    HTTag * tag = &HTML_dtd.tags[element_number];
	    char buf[200];
	    CONST char* p;
	    if (psrc_first_tag) {
		psrc_first_tag=FALSE;
		/* perform the special actions on the begining of the document.
		   It's assumed that all lynx modules start generating html
		   from tag (ie not a text) so we are able to trap this moment
		   and initialize.
		*/
		psrc_nested_call=TRUE;
		HTML_start_element(me,HTML_BODY, NULL,NULL,tag_charset,NULL);
		HTML_start_element(me,HTML_PRE, NULL,NULL,tag_charset,NULL);
		PSRCSTART(entire);
		psrc_nested_call=FALSE;
	    }

	    psrc_nested_call=TRUE;
	    /*write markup for tags and exit*/
	    PSRCSTART(abracket); PUTC('<'); PSRCSTOP(abracket);
	    PSRCSTART(tag);
	    if (tagname_transform!=0)
		PUTS(tag->name);
	    else {
		LYstrncpy(buf, tag->name, sizeof(buf)-1);
		LYLowerCase(buf);
		PUTS(buf);
	    }
	    if (present) {
		for (i = 0; i < tag->number_of_attributes; i++)
		    if (present[i]) {
			PUTC(' ');
			PSRCSTART(attrib);
			if (attrname_transform!=0)
			    PUTS(tag->attributes[i].name);
			else {
			    LYstrncpy(buf, tag->attributes[i].name, sizeof(buf)-1);
			    LYLowerCase(buf);
			    PUTS(buf);
			}
			if (value[i]) {
			    char q='"';
				/*0 in dquotes, 1 - in quotes, 2 mixed*/
			    char kind= (char) ( !strchr(value[i], '"') ?
					 0 :
					 !strchr(value[i], '\'') ?
					 q='\'',1 :
					 2);
			    PUTC('=');
			    PSRCSTOP(attrib);
			    PSRCSTART(attrval);
			    PUTC(q);
			    /*is it special ? */
			    if (tag->attributes[i].type == HTMLA_ANAME) {
				HTStartAnchor(me,value[i],NULL);
				HTML_end_element(me,HTML_A,NULL);
			    } else if (tag->attributes[i].type == HTMLA_HREF) {
				PSRCSTART(href);
				HTStartAnchor(me,NULL,value[i]);
			    }
			    if (kind!=2)
				PUTS(value[i]);
			    else
				for (p = value[i]; *p; p++)
				    if (*p != '"')
					PUTC(*p);
				    else
					PUTS("&#34;");
			    /*is it special ? */
			    if (tag->attributes[i].type == HTMLA_HREF) {
				HTML_end_element(me,HTML_A,NULL);
				PSRCSTOP(href);
			    }
			    PUTC(q);
			    PSRCSTOP(attrval);
			} /* if value */
		    } /* if present[i] */
	    }/* if present*/
	    PSRCSTOP(tag);
	    PSRCSTART(abracket); PUTC('>'); PSRCSTOP(abracket);
	    psrc_nested_call=FALSE;
	    return HT_OK;
	} /*if (!psrc_nested_call) */
	/*fall through*/
    }
#endif /* USE_PRETTYSRC */

    if (LYMapsOnly) {
	if (!(ElementNumber == HTML_MAP || ElementNumber == HTML_AREA ||
	      ElementNumber == HTML_BASE || ElementNumber == HTML_OBJECT ||
	      ElementNumber == HTML_A)) {
	    return HT_OK;
	}
    } else if (!me->text) {
	UPDATE_STYLE;
    }

    {
	/*  me->tag_charset  is charset for attribute values.  */
	int j = ((tag_charset < 0) ? me->UCLYhndl : tag_charset);

	if ((me->tag_charset != j) || (j < 0  /* for trace entry */)) {
	    CTRACE((tfp, "me->tag_charset: %d -> %d", me->tag_charset, j ));
	    CTRACE((tfp, " (me->UCLYhndl: %d, tag_charset: %d)\n",
		   me->UCLYhndl, tag_charset));
	    me->tag_charset = j;
	}
    }

/* this should be done differently */
#if defined(USE_COLOR_STYLE)

#if !OPT_SCN
    HTSprintf (&Style_className, ";%s", HTML_dtd.tags[element_number].name);
#else
# if !OMIT_SCN_KEEPING
    *Style_className_end=';';
    memcpy(Style_className_end+1,
	   HTML_dtd.tags[element_number].name,
	   HTML_dtd.tags[element_number].name_len+1);
    Style_className_end += HTML_dtd.tags[element_number].name_len+1;
# endif
#endif
    class_name = (force_classname ? forced_classname : class_string);
    force_classname = FALSE;

    if (force_current_tag_style == FALSE) {
	current_tag_style = class_name[0]
			  ? -1
			  : cached_tag_styles[element_number];
    } else {
	force_current_tag_style = FALSE;
    }

    CTRACE((tfp, "CSS.elt:<%s>\n", HTML_dtd.tags[element_number].name));

    if (current_tag_style == -1) {	/* Append class_name */
#if !OPT_SCN
	strcpy (myHash, HTML_dtd.tags[element_number].name);
#else
	hcode = hash_code_lowercase_on_fly(HTML_dtd.tags[element_number].name);
#endif
	if (class_name[0])
	{
	    int ohcode = hcode;
	    char *oend = Style_className_end;
#if !OPT_SCN
	    int len = strlen(myHash);
	    sprintf(myHash, ".%.*s", (int)sizeof(myHash) - len - 2, class_name);
	    HTSprintf (&Style_className, ".%s", class_name);
#else
#   if !OMIT_SCN_KEEPING
	    int l = strlen(class_name);
	    *Style_className_end = '.';
	    memcpy(Style_className_end+1, class_name, l+1 );
	    Style_className_end += l+1;
#   endif

	    hcode = hash_code_aggregate_char('.', hcode);
	    hcode = hash_code_aggregate_lower_str(class_name, hcode);
#endif
	    if (!hashStyles[hcode].name) { /* None such -> classless version */
		hcode = ohcode;
		*oend = '\0';
		CTRACE((tfp, "STYLE.start_element: <%s> (class <%s> not configured), hcode=%d.\n",
			HTML_dtd.tags[element_number].name, class_name, hcode));
	    } else {
		CTRACE((tfp, "STYLE.start_element: <%s>.<%s>, hcode=%d.\n",
			HTML_dtd.tags[element_number].name, class_name, hcode));
		class_used = 1;
	    }
	}

#if !OPT_SCN
	strtolower(myHash);
	hcode = hash_code(myHash);
#endif
	class_string[0] = '\0';

#if !OPT_SCN
    if (TRACE)
    {
	fprintf(tfp, "CSSTRIM:%s -> %d", myHash, hcode);
	if (hashStyles[hcode].code!=hcode)
	{
	    char *rp = strrchr(myHash, '.');
	    fprintf(tfp, " (undefined) %s\n", myHash);
	    if (rp)
	    {
		int hcd;
		*rp='\0'; /* trim the class */
		hcd = hash_code(myHash);
		fprintf(tfp, "CSS:%s -> %d", myHash, hcd);
		if (hashStyles[hcd].code!=hcd)
		    fprintf(tfp, " (undefined) %s\n", myHash);
		else
		    fprintf(tfp, " ca=%d\n", hashStyles[hcd].color);
	    }
	}
	else
	    fprintf(tfp, " ca=%d\n", hashStyles[hcode].color);
    }
#endif
    } else { /* (current_tag_style!=-1)	 */
	if (class_name[0]) {
#if !OPT_SCN
	    int len = strlen(myHash);
	    sprintf(myHash, ".%.*s", (int)sizeof(myHash) - len - 2, class_name);
	    HTSprintf (&Style_className, ".%s", class_name);
#else
#     if !OMIT_SCN_KEEPING
	    int l = strlen(class_name);
	    *Style_className_end = '.';
	    memcpy(Style_className_end+1,class_name, l + 1 );
	    Style_className_end += l + 1;
#     endif
#endif
	    class_string[0] = '\0';
	}
	hcode = current_tag_style;
	CTRACE((tfp, "STYLE.start_element: <%s>, hcode=%d.\n",
		HTML_dtd.tags[element_number].name, hcode));
	current_tag_style = -1;
    }

#if !OPT_SCN
    strtolower(Style_className);
#else
#  if !OMIT_SCN_KEEPING
    strtolower(Style_className_end_was);/*only the part that wasn't
					  lowercased yet*/
#   endif
#endif

#if OPT_SCN && !OMIT_SCN_KEEPING	/* Can be done in other cases too... */
    if (!class_used && ElementNumber == HTML_INPUT) { /* For some other too? */
	char *type = "", *oend = Style_className_end;
	int l, ohcode = hcode;

	if (present && present[HTML_INPUT_TYPE] && value[HTML_INPUT_TYPE])
	    type = (char *)value[HTML_INPUT_TYPE];
	l = strlen(type);

	*Style_className_end = '.';
	memcpy(Style_className_end+1, "type.", 5 );
	memcpy(Style_className_end+6, type, l+1 );
	Style_className_end += l+6;
	hcode = hash_code_aggregate_lower_str(".type.", hcode);
	hcode = hash_code_aggregate_lower_str(type, hcode);
	if (!hashStyles[hcode].name) { /* None such -> classless version */
	    hcode = ohcode;
	    *oend = '\0';
	    CTRACE((tfp, "STYLE.start_element: type <%s> not configured.\n", type));
	} else {
	    CTRACE((tfp, "STYLE.start_element: <%s>.type.<%s>, hcode=%d.\n",
		    HTML_dtd.tags[element_number].name, type, hcode));
	}
    }
#endif	/* OPT_SCN && !OMIT_SCN_KEEPING */

    HText_characterStyle(me->text, hcode, 1);
#endif /* USE_COLOR_STYLE */

    /*
     *	Handle the start tag. - FM
     */
    switch (ElementNumber) {

    case HTML_HTML:
	break;

    case HTML_HEAD:
	break;

    case HTML_BASE:
	if (present && present[HTML_BASE_HREF] && !local_host_only &&
	    value[HTML_BASE_HREF] && *value[HTML_BASE_HREF]) {
	    char *base = NULL;
	    char *related = NULL;

	    StrAllocCopy(base, value[HTML_BASE_HREF]);
	    if (!(url_type = LYLegitimizeHREF(me, &base, TRUE, TRUE))) {
		CTRACE((tfp, "HTML: BASE '%s' is not an absolute URL.\n",
			    (base ? base : "")));
		if (me->inBadBASE == FALSE)
		    HTAlert(BASE_NOT_ABSOLUTE);
		me->inBadBASE = TRUE;
	    }

	    if (url_type == LYNXIMGMAP_URL_TYPE) {
		/*
		 *  These have a non-standard form, basically
		 *  strip the prefix or the code below would insert
		 *  a nonsense host into the pseudo URL.  These
		 *  should never occur where they would be used for
		 *  resolution of relative URLs anyway.  We can
		 *  also strip the #map part. - kw
		 */
		temp = HTParse(base + 11, "",
			       PARSE_ACCESS+PARSE_HOST+PARSE_PATH
			       +PARSE_PUNCTUATION);
		if (temp) {
		    FREE(base);
		    base = temp;
		    temp = NULL;
		}
	    }

	    /*
	     *	Get parent's address for defaulted fields.
	     */
	    StrAllocCopy(related, me->node_anchor->address);

	    /*
	     *	Create the access field.
	     */
	    if ((temp = HTParse(base, related,
				PARSE_ACCESS+PARSE_PUNCTUATION)) &&
		*temp != '\0') {
		StrAllocCopy(me->base_href, temp);
	    } else {
		FREE(temp);
		StrAllocCopy(me->base_href, (temp = HTParse(related, "",
					 PARSE_ACCESS+PARSE_PUNCTUATION)));
	    }
	    FREE(temp);

	    /*
	     *	Create the host[:port] field.
	     */
	    if ((temp = HTParse(base, "",
				PARSE_HOST+PARSE_PUNCTUATION)) &&
		!strncmp(temp, "//", 2)) {
		StrAllocCat(me->base_href, temp);
		if (!strcmp(me->base_href, "file://")) {
		    StrAllocCat(me->base_href, "localhost");
		}
	    } else {
		if (!strcmp(me->base_href, "file:")) {
		    StrAllocCat(me->base_href, "//localhost");
		} else if (strcmp(me->base_href, "news:")) {
		    FREE(temp);
		    StrAllocCat(me->base_href, (temp = HTParse(related, "",
					    PARSE_HOST+PARSE_PUNCTUATION)));
		}
	    }
	    FREE(temp);
	    FREE(related);

	    /*
	     *	Create the path field.
	     */
	    if ((temp = HTParse(base, "",
				PARSE_PATH+PARSE_PUNCTUATION)) &&
		*temp != '\0') {
		StrAllocCat(me->base_href, temp);
	    } else if (!strcmp(me->base_href, "news:")) {
		StrAllocCat(me->base_href, "*");
	    } else if (!strncmp(me->base_href, "news:", 5) ||
		       !strncmp(me->base_href, "nntp:", 5) ||
		       !strncmp(me->base_href, "snews:", 6)) {
		StrAllocCat(me->base_href, "/*");
	    } else {
		StrAllocCat(me->base_href, "/");
	    }
	    FREE(temp);
	    FREE(base);

	    me->inBASE = TRUE;
	    StrAllocCopy(me->node_anchor->content_base, me->base_href);
	}
	break;

    case HTML_META:
	if (present)
	    LYHandleMETA(me, present, value, include);
	break;

    case HTML_TITLE:
	HTChunkClear(&me->title);
	break;

    case HTML_LINK:
#ifndef DONT_TRACK_INTERNAL_LINKS
	intern_flag = FALSE;
#endif
	if (present && present[HTML_LINK_HREF]) {
	    CHECK_FOR_INTERN(intern_flag,value[HTML_LINK_HREF]);
	    /*
	     *	Prepare to do housekeeping on the reference. - FM
	     */
	    if (!value[HTML_LINK_HREF]) {
		if (me->inBASE && me->base_href && *me->base_href) {
		    StrAllocCopy(href, me->base_href);
		} else {
		    StrAllocCopy(href, me->node_anchor->address);
		}
	    } else {
		StrAllocCopy(href, value[HTML_LINK_HREF]);
		url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    }

	    /*
	     *	Check whether a base tag is in effect. - FM
	     */
	    if ((me->inBASE && *href != '\0' && *href != '#') &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((*href != '\0' && *href != '#' &&
				 me->inBASE) ?
			       me->base_href : me->node_anchor->address));

	    /*
	     *	Handle links with a REV attribute. - FM
	     */
	    if (present &&
		present[HTML_LINK_REV] && value[HTML_LINK_REV]) {
		/*
		 *  Handle REV="made" or REV="owner". - LM & FM
		 */
		if (!strcasecomp("made", value[HTML_LINK_REV]) ||
		    !strcasecomp("owner", value[HTML_LINK_REV])) {
		    /*
		     *	Load the owner element. - FM
		     */
		    if (!is_url(href)) {
			temp = HTParse(href,
				       (me->inBASE ?
				     me->base_href : me->node_anchor->address),
					PARSE_ALL);
			StrAllocCopy(href, temp);
			FREE(temp);
			LYFillLocalFileURL(&href,
					   (me->inBASE ?
					 me->base_href :
					 me->node_anchor->address));
		    }
		    HTAnchor_setOwner(me->node_anchor, href);
		    CTRACE((tfp, "HTML: DOC OWNER '%s' found\n", href));
		    FREE(href);

		    /*
		     *	Load the RevTitle element if a TITLE attribute
		     *	and value are present. - FM
		     */
		    if (present && present[HTML_LINK_TITLE] &&
			value[HTML_LINK_TITLE] &&
			*value[HTML_LINK_TITLE] != '\0') {
			StrAllocCopy(title, value[HTML_LINK_TITLE]);
			TRANSLATE_AND_UNESCAPE_ENTITIES(&title, TRUE, FALSE);
			LYTrimHead(title);
			LYTrimTail(title);
			if (*title != '\0')
			    HTAnchor_setRevTitle(me->node_anchor, title);
			FREE(title);
		    }
		    break;
		}
	    }

	    /*
	     *	Handle REL links. - FM
	     */

	    if (present &&
		present[HTML_LINK_REL] && value[HTML_LINK_REL]) {
		/*
		 *  Ignore style sheets, for now. - FM
		 */
/*  lss and css has different syntax - lynx shouldn't try to
    parse them now (it tries to parse them as lss, so it exits with
    error message on the 1st non-empty line) - VH
*/
#ifndef USE_COLOR_STYLE
		if (!strcasecomp(value[HTML_LINK_REL], "StyleSheet") ||
		    !strcasecomp(value[HTML_LINK_REL], "Style")) {
		    CTRACE((tfp, "HTML: StyleSheet link found.\n"));
#ifdef LINKEDSTYLES
		    if (href && *href != '\0')
		    {
			int res = -999;
			if ((url_type = is_url(href)) == 0 ||
			    (url_type == FILE_URL_TYPE && LYisLocalFile(href))) {
			    if (url_type == FILE_URL_TYPE) {
				temp = HTParse(href, "", PARSE_PATH+PARSE_PUNCTUATION);
				HTUnEscape(temp);
				if (temp && *temp != '\0') {
				    res = style_readFromFile(temp);
				    if (res != 0)
					StrAllocCopy(href, temp);
				}
				FREE(temp);
			    } else {
				res = style_readFromFile(href);
			    }
			}
			CTRACE((tfp, "CSS: StyleSheet=%s %d\n", href, res));
			if (res == 0)
			    HTAnchor_setStyle (me->node_anchor, href);
		    }
		    else {
			CTRACE((tfp,
				"        non-local StyleSheets not yet implemented.\n"));
		    }
#else
		    CTRACE((tfp,
				"        StyleSheets not yet implemented.\n"));
#endif
		    FREE(href);
		    break;
		}
#endif /* ! USE_COLOR_STYLE */

		/*
		 *  Ignore anything not registered in the the 28-Mar-95
		 *  IETF HTML 3.0 draft and W3C HTML 3.2 draft, or not
		 *  appropriate for Lynx banner links in the expired
		 *  Maloney and Quin relrev draft.  We'll make this more
		 *  efficient when the situation stabilizes, and for now,
		 *  we'll treat "Banner" as another toolbar element. - FM
		 */
		if (!strcasecomp(value[HTML_LINK_REL], "Home") ||
		    !strcasecomp(value[HTML_LINK_REL], "ToC") ||
		    !strcasecomp(value[HTML_LINK_REL], "Contents") ||
		    !strcasecomp(value[HTML_LINK_REL], "Index") ||
		    !strcasecomp(value[HTML_LINK_REL], "Glossary") ||
		    !strcasecomp(value[HTML_LINK_REL], "Copyright") ||
		    !strcasecomp(value[HTML_LINK_REL], "Help") ||
		    !strcasecomp(value[HTML_LINK_REL], "Search") ||
		    !strcasecomp(value[HTML_LINK_REL], "Bookmark") ||
		    !strcasecomp(value[HTML_LINK_REL], "Banner") ||
		    !strcasecomp(value[HTML_LINK_REL], "Top") ||
		    !strcasecomp(value[HTML_LINK_REL], "Origin") ||
		    !strcasecomp(value[HTML_LINK_REL], "Navigator") ||
		    !strcasecomp(value[HTML_LINK_REL], "Disclaimer") ||
		    !strcasecomp(value[HTML_LINK_REL], "Author") ||
		    !strcasecomp(value[HTML_LINK_REL], "Editor") ||
		    !strcasecomp(value[HTML_LINK_REL], "Publisher") ||
		    !strcasecomp(value[HTML_LINK_REL], "Trademark") ||
		    !strcasecomp(value[HTML_LINK_REL], "Hotlist") ||
		    !strcasecomp(value[HTML_LINK_REL], "Begin") ||
		    !strcasecomp(value[HTML_LINK_REL], "First") ||
		    !strcasecomp(value[HTML_LINK_REL], "End") ||
		    !strcasecomp(value[HTML_LINK_REL], "Last") ||
		    !strcasecomp(value[HTML_LINK_REL], "Documentation") ||
		    !strcasecomp(value[HTML_LINK_REL], "Biblioentry") ||
		    !strcasecomp(value[HTML_LINK_REL], "Bibliography") ||
		    !strcasecomp(value[HTML_LINK_REL], "Start") ||
		    !strcasecomp(value[HTML_LINK_REL], "Appendix")) {
		    StrAllocCopy(title, value[HTML_LINK_REL]);
		    pdoctitle = &title;	/* for setting HTAnchor's title */
		} else
		if (!strcasecomp(value[HTML_LINK_REL], "Up") ||
		    !strcasecomp(value[HTML_LINK_REL], "Next") ||
		    !strcasecomp(value[HTML_LINK_REL], "Previous") ||
		    !strcasecomp(value[HTML_LINK_REL], "Prev") ||
		    !strcasecomp(value[HTML_LINK_REL], "Child") ||
		    !strcasecomp(value[HTML_LINK_REL], "Sibling") ||
		    !strcasecomp(value[HTML_LINK_REL], "Parent") ||
		    !strcasecomp(value[HTML_LINK_REL], "Meta") ||
		    !strcasecomp(value[HTML_LINK_REL], "URC") ||
		    !strcasecomp(value[HTML_LINK_REL], "Pointer") ||
		    !strcasecomp(value[HTML_LINK_REL], "Translation") ||
		    !strcasecomp(value[HTML_LINK_REL], "Definition") ||
		    !strcasecomp(value[HTML_LINK_REL], "Alternate") ||
		    !strcasecomp(value[HTML_LINK_REL], "Section") ||
		    !strcasecomp(value[HTML_LINK_REL], "Subsection") ||
		    !strcasecomp(value[HTML_LINK_REL], "Chapter")) {
		    StrAllocCopy(title, value[HTML_LINK_REL]);
		    /* not setting target HTAnchor's title, for these
		       links of highly relative character.  Instead,
		       try to remember the REL attribute as a property
		       of the link (but not the destination), in the
		       (otherwise underused) link type in a special format;
		       the LIST page generation code may later use it. - kw */
		    if (!intern_flag) {
			StrAllocCopy(temp, "RelTitle: ");
			StrAllocCat(temp, value[HTML_LINK_REL]);
		    }
		} else {
		    CTRACE((tfp, "HTML: LINK with REL=\"%s\" ignored.\n",
				 value[HTML_LINK_REL]));
		    FREE(href);
		    break;
		}
	    }
	} else if (present &&
		   present[HTML_LINK_REL] && value[HTML_LINK_REL]) {
	    /*
	     *	If no HREF was specified, handle special REL links
	     *	with self-designated HREFs. - FM
	     */
	    if (!strcasecomp(value[HTML_LINK_REL], "Home")) {
		StrAllocCopy(href, LynxHome);
	    } else if (!strcasecomp(value[HTML_LINK_REL], "Help")) {
		StrAllocCopy(href, helpfile);
	    } else if (!strcasecomp(value[HTML_LINK_REL], "Index")) {
		StrAllocCopy(href, indexfile);
	    } else {
		CTRACE((tfp, "HTML: LINK with REL=\"%s\" and no HREF ignored.\n",
			    value[HTML_LINK_REL]));
		break;
	    }
	    StrAllocCopy(title, value[HTML_LINK_REL]);
	    pdoctitle = &title;
	}
	if (href) {
	    /*
	     *	Create a title (link name) from the TITLE value,
	     *	if present, or default to the REL value that was
	     *	loaded into title. - FM
	     */
	    if (present && present[HTML_LINK_TITLE] &&
		value[HTML_LINK_TITLE] && *value[HTML_LINK_TITLE] != '\0') {
		StrAllocCopy(title, value[HTML_LINK_TITLE]);
		TRANSLATE_AND_UNESCAPE_ENTITIES(&title, TRUE, FALSE);
		LYTrimHead(title);
		LYTrimTail(title);
		pdoctitle = &title;
		FREE(temp); /* forget about recording RelTitle - kw */
	    }
	    if (!(title && *title)) {
		FREE(href);
		FREE(title);
		break;
	    }

	    if (me->inA) {
		/*
		 *  Ugh!  The LINK tag, which is a HEAD element,
		 *  is in an Anchor, which is BODY element.  All
		 *  we can do is close the Anchor and cross our
		 *  fingers. - FM
		 */
		SET_SKIP_STACK(HTML_A);
		HTML_end_element(me, HTML_A, include);
	    }

	    /*
	     *	Create anchors for the links that simulate
	     *	a toolbar. - FM
	     */
	    me->CurrentA = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				NULL,			/* Tag */
				href,			/* Addresss */
				temp
				   ? (HTLinkType*)HTAtom_for(temp)
				   : INTERN_LT);	/* Type */
	    FREE(temp);
	    if ((dest = HTAnchor_parent(
			    HTAnchor_followMainLink((HTAnchor*)me->CurrentA)
				      )) != NULL) {
		if (pdoctitle && !HTAnchor_title(dest))
		    HTAnchor_setTitle(dest, *pdoctitle);

		/* Don't allow CHARSET attribute to change *this* document's
		   charset assumption. - kw */
		if (dest == me->node_anchor)
		    dest = NULL;
		if (present[HTML_LINK_CHARSET] &&
		    value[HTML_LINK_CHARSET] && *value[HTML_LINK_CHARSET] != '\0') {
		    dest_char_set = UCGetLYhndl_byMIME(value[HTML_LINK_CHARSET]);
		    if (dest_char_set < 0)
			dest_char_set = UCLYhndl_for_unrec;
		}
		if (dest && dest_char_set >= 0)
		    HTAnchor_setUCInfoStage(dest, dest_char_set,
					    UCT_STAGE_PARSER,
					    UCT_SETBY_LINK);
	    }
	    UPDATE_STYLE;
	    if (!HText_hasToolbar(me->text) &&
		(ID_A = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					LYToolbarName,		/* Tag */
					NULL,			/* Addresss */
					(HTLinkType*)0))) {	/* Type */
		HText_appendCharacter(me->text, '#');
		HText_setLastChar(me->text, ' ');  /* absorb white space */
		HText_beginAnchor(me->text, me->inUnderline, ID_A);
		HText_endAnchor(me->text, 0);
		HText_setToolbar(me->text);
	    } else {
		/*
		 *  Add collapsible space to separate link from previous
		 *  generated links. - kw
		 */
		HTML_put_character(me, ' ');
	    }
	    HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
#ifdef USE_COLOR_STYLE
	    if (present && present[HTML_LINK_CLASS] &&
	    value && *value[HTML_LINK_CLASS]!='\0')
	    {
		char *tmp = 0;
		HTSprintf0(&tmp, "link.%s.%s", value[HTML_LINK_CLASS], title);
		CTRACE((tfp, "STYLE.link: using style <%s>\n", tmp));

		HText_characterStyle(me->text, hash_code(tmp), 1);
		HTML_put_string(me, title);
		HTML_put_string(me, " (");
		HTML_put_string(me, value[HTML_LINK_CLASS]);
		HTML_put_string(me, ")");
		HText_characterStyle(me->text, hash_code(tmp), 0);
		FREE(tmp);
	    }
	    else
#endif
	    HTML_put_string(me, title);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
	    HText_endAnchor(me->text, 0);
	}
	FREE(href);
	FREE(title);
	break;

    case HTML_ISINDEX:
	if (((present)) &&
	    ((present[HTML_ISINDEX_HREF] && value[HTML_ISINDEX_HREF]) ||
	     (present[HTML_ISINDEX_ACTION] && value[HTML_ISINDEX_ACTION]))) {
	    char * action = NULL;
	    char * isindex_href = NULL;

	    /*
	     *	Lynx was supporting ACTION, which never made it into
	     *	the HTML 2.0 specs.  HTML 3.0 uses HREF, so we'll
	     *	use that too, but allow use of ACTION as an alternate
	     *	until people have fully switched over. - FM
	     */
	    if (present[HTML_ISINDEX_HREF] && value[HTML_ISINDEX_HREF])
		StrAllocCopy(isindex_href, value[HTML_ISINDEX_HREF]);
	    else
		StrAllocCopy(isindex_href, value[HTML_ISINDEX_ACTION]);
	    url_type = LYLegitimizeHREF(me, &isindex_href, TRUE, TRUE);

	    /*
	     *	Check whether a base tag is in effect.
	     */
	    if (me->inBASE && *isindex_href != '\0' && *isindex_href != '#')
		action = HTParse(isindex_href, me->base_href, PARSE_ALL);
	    if (!(action && *action))
		action = HTParse(isindex_href,
				 me->node_anchor->address, PARSE_ALL);
	    FREE(isindex_href);

	    if (action && *action) {
		HTAnchor_setIndex(me->node_anchor, action);
	    } else {
		HTAnchor_setIndex(me->node_anchor, me->node_anchor->address);
	    }
	    FREE(action);

	} else {
	    if (me->inBASE)
		/*
		 *  Use base.
		 */
		HTAnchor_setIndex(me->node_anchor, me->base_href);
	    else
		/*
		 *  Use index's address.
		 */
		HTAnchor_setIndex(me->node_anchor, me->node_anchor->address);
	}
	/*
	 *  Support HTML 3.0 PROMPT attribute. - FM
	 */
	if (present &&
	    present[HTML_ISINDEX_PROMPT] &&
	    value[HTML_ISINDEX_PROMPT] && *value[HTML_ISINDEX_PROMPT]) {
	    StrAllocCopy(temp, value[HTML_ISINDEX_PROMPT]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&temp, TRUE, FALSE);
	    LYTrimHead(temp);
	    LYTrimTail(temp);
	    if (*temp != '\0') {
		StrAllocCat(temp, " ");
		HTAnchor_setPrompt(me->node_anchor, temp);
	    } else {
		HTAnchor_setPrompt(me->node_anchor, ENTER_DATABASE_QUERY);
	    }
	    FREE(temp);
	} else {
	    HTAnchor_setPrompt(me->node_anchor, ENTER_DATABASE_QUERY);
	}
	break;

    case HTML_NEXTID:
	/* if (present && present[NEXTID_N] && value[NEXTID_N])
		HText_setNextId(me->text, atoi(value[NEXTID_N])); */
	break;

    case HTML_STYLE:
	/*
	 *  We're getting it as Literal text, which, for now,
	 *  we'll just ignore. - FM
	 */
	HTChunkClear(&me->style_block);
	break;

    case HTML_SCRIPT:
	/*
	 *  We're getting it as Literal text, which, for now,
	 *  we'll just ignore. - FM
	 */
	HTChunkClear(&me->script);
	break;

    case HTML_BODY:
	CHECK_ID(HTML_BODY_ID);
	if (HText_hasToolbar(me->text))
	    HText_appendParagraph(me->text);
	break;

    case HTML_FRAMESET:
	break;

    case HTML_FRAME:
	if (present && present[HTML_FRAME_NAME] &&
	    value[HTML_FRAME_NAME] && *value[HTML_FRAME_NAME]) {
	    StrAllocCopy(id_string, value[HTML_FRAME_NAME]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&id_string, TRUE, FALSE);
	    LYTrimHead(id_string);
	    LYTrimTail(id_string);
	}
	if (present && present[HTML_FRAME_SRC] &&
	    value[HTML_FRAME_SRC] && *value[HTML_FRAME_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_FRAME_SRC]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);

	    /*
	     *	Check whether a base tag is in effect. - FM
	     */
	    if ((me->inBASE && *href != '\0' && *href != '#') &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((*href != '\0' && *href != '#' &&
				 me->inBASE) ?
			       me->base_href : me->node_anchor->address));

	    if (me->inA) {
		SET_SKIP_STACK(HTML_A);
		HTML_end_element(me, HTML_A, include);
	    }
	    me->CurrentA = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				NULL,			/* Tag */
				href,			/* Addresss */
				INTERN_LT);		/* Type */
	    CAN_JUSTIFY_PUSH(FALSE);
	    LYEnsureSingleSpace(me);
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    HTML_put_string(me, "FRAME:");
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    HTML_put_character(me, ' ');

	    me->in_word = NO;
	    CHECK_ID(HTML_FRAME_ID);
	    HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
	    HTML_put_string(me, (id_string ? id_string : href));
	    FREE(href);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
	    HText_endAnchor(me->text, 0);
	    LYEnsureSingleSpace(me);
	    CAN_JUSTIFY_POP;
	} else {
	    CHECK_ID(HTML_FRAME_ID);
	}
	FREE(id_string);
	break;

    case HTML_NOFRAMES:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	break;

    case HTML_IFRAME:
	if (present && present[HTML_IFRAME_NAME] &&
	    value[HTML_IFRAME_NAME] && *value[HTML_IFRAME_NAME]) {
	    StrAllocCopy(id_string, value[HTML_IFRAME_NAME]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&id_string, TRUE, FALSE);
	    LYTrimHead(id_string);
	    LYTrimTail(id_string);
	}
	if (present && present[HTML_IFRAME_SRC] &&
	    value[HTML_IFRAME_SRC] && *value[HTML_IFRAME_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_IFRAME_SRC]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);

	    /*
	     *	Check whether a base tag is in effect. - FM
	     */
	    if ((me->inBASE && *href != '\0' && *href != '#') &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((*href != '\0' && *href != '#' &&
				 me->inBASE) ?
			       me->base_href : me->node_anchor->address));

	    if (me->inA)
		HTML_end_element(me, HTML_A, include);
	    me->CurrentA = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				NULL,			/* Tag */
				href,			/* Addresss */
				INTERN_LT);		/* Type */
	    LYEnsureDoubleSpace(me);
	    CAN_JUSTIFY_PUSH_F
	    LYResetParagraphAlignment(me);
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    HTML_put_string(me, "IFRAME:");
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    HTML_put_character(me, ' ');

	    me->in_word = NO;
	    CHECK_ID(HTML_IFRAME_ID);
	    HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
	    HTML_put_string(me, (id_string ? id_string : href));
	    FREE(href);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
	    HText_endAnchor(me->text, 0);
	    LYEnsureSingleSpace(me);
	    CAN_JUSTIFY_POP;
	} else {
	    CHECK_ID(HTML_IFRAME_ID);
	}
	FREE(id_string);
	break;

    case HTML_BANNER:
    case HTML_MARQUEE:
	change_paragraph_style(me, styles[HTML_BANNER]);
	UPDATE_STYLE;
	if (me->sp->tag_number == (int) ElementNumber)
	    LYEnsureDoubleSpace(me);
	/*
	 *  Treat this as a toolbar if we don't have one
	 *  yet, and we are in the first half of the
	 *  first page. - FM
	 */
	if ((!HText_hasToolbar(me->text) &&
	     HText_getLines(me->text) < (display_lines/2)) &&
	    (ID_A = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					LYToolbarName,		/* Tag */
					NULL,			/* Addresss */
					(HTLinkType*)0))) {	/* Type */
	    HText_beginAnchor(me->text, me->inUnderline, ID_A);
	    HText_endAnchor(me->text, 0);
	    HText_setToolbar(me->text);
	}
	CHECK_ID(HTML_GEN_ID);
	break;

    case HTML_CENTER:
    case HTML_DIV:
	if (me->Division_Level < (MAX_NESTING - 1)) {
	    me->Division_Level++;
	} else {
	    CTRACE((tfp,
		"HTML: ****** Maximum nesting of %d divisions exceeded!\n",
		MAX_NESTING));
	}
	if (me->inP)
	    LYEnsureSingleSpace(me); /* always at least break line - kw */
	if (ElementNumber == HTML_CENTER) {
	    me->DivisionAlignments[me->Division_Level] = HT_CENTER;
	    change_paragraph_style(me, styles[HTML_DCENTER]);
	    UPDATE_STYLE;
	    me->current_default_alignment = styles[HTML_DCENTER]->alignment;
	} else if (me->List_Nesting_Level >= 0 &&
		   !(present && present[HTML_DIV_ALIGN] &&
		     value[HTML_DIV_ALIGN] &&
		     (!strcasecomp(value[HTML_DIV_ALIGN], "center") ||
		      !strcasecomp(value[HTML_DIV_ALIGN], "right")))) {
	    if (present && present[HTML_DIV_ALIGN])
		me->current_default_alignment = HT_LEFT;
	    else if (me->Division_Level == 0)
		me->current_default_alignment = HT_LEFT;
	    else if (me->sp[0].tag_number == HTML_UL ||
		     me->sp[0].tag_number == HTML_OL ||
		     me->sp[0].tag_number == HTML_MENU ||
		     me->sp[0].tag_number == HTML_DIR ||
		     me->sp[0].tag_number == HTML_LI ||
		     me->sp[0].tag_number == HTML_LH ||
		     me->sp[0].tag_number == HTML_DD)
		me->current_default_alignment = HT_LEFT;
	    LYHandlePlike(me, present, value, include, HTML_DIV_ALIGN, TRUE);
	    me->DivisionAlignments[me->Division_Level] = (short)
		me->current_default_alignment;
	} else if (present && present[HTML_DIV_ALIGN] &&
		   value[HTML_DIV_ALIGN] && *value[HTML_DIV_ALIGN]) {
	    if (!strcasecomp(value[HTML_DIV_ALIGN], "center")) {
		me->DivisionAlignments[me->Division_Level] = HT_CENTER;
		change_paragraph_style(me, styles[HTML_DCENTER]);
		UPDATE_STYLE;
		me->current_default_alignment = styles[HTML_DCENTER]->alignment;
	    } else if (!strcasecomp(value[HTML_DIV_ALIGN], "right")) {
		me->DivisionAlignments[me->Division_Level] = HT_RIGHT;
		change_paragraph_style(me, styles[HTML_DRIGHT]);
		UPDATE_STYLE;
		me->current_default_alignment = styles[HTML_DRIGHT]->alignment;
	    } else {
		me->DivisionAlignments[me->Division_Level] = HT_LEFT;
		change_paragraph_style(me, styles[HTML_DLEFT]);
		UPDATE_STYLE;
		me->current_default_alignment = styles[HTML_DLEFT]->alignment;
	    }
	} else {
	    me->DivisionAlignments[me->Division_Level] = HT_LEFT;
	    change_paragraph_style(me, styles[HTML_DLEFT]);
	    UPDATE_STYLE;
	    me->current_default_alignment = styles[HTML_DLEFT]->alignment;
	}
	CHECK_ID(HTML_DIV_ID);
	break;

    case HTML_H1:
    case HTML_H2:
    case HTML_H3:
    case HTML_H4:
    case HTML_H5:
    case HTML_H6:
	/*
	 *  Close the previous style if not done by HTML doc.
	 *  Added to get rid of core dumps in BAD HTML on the net.
	 *		GAB 07-07-94
	 *  But then again, these are actually allowed to nest.  I guess
	 *  I have to depend on the HTML writers correct style.
	 *		GAB 07-12-94
	if (i_prior_style != -1) {
	    HTML_end_element(me, i_prior_style);
	}
	i_prior_style = ElementNumber;
	 */

	/*
	 *  Check whether we have an H# in a list,
	 *  and if so, treat it as an LH. - FM
	 */
	if ((me->List_Nesting_Level >= 0) &&
	    (me->sp[0].tag_number == HTML_UL ||
	     me->sp[0].tag_number == HTML_OL ||
	     me->sp[0].tag_number == HTML_MENU ||
	     me->sp[0].tag_number == HTML_DIR ||
	     me->sp[0].tag_number == HTML_LI)) {
	    if (HTML_dtd.tags[HTML_LH].contents == SGML_EMPTY) {
		ElementNumber = HTML_LH;
	    } else {
		me->new_style = me->sp[0].style;
		ElementNumber = me->sp[0].tag_number;
		UPDATE_STYLE;
	    }
	    /*
	     *	Some authors use H# headers as a substitute for
	     *	FONT, so check if this one immediately followed
	     *	an LI.	If so, both me->inP and me->in_word will
	     *	be FALSE (though the line might not be empty due
	     *	to a bullet and/or nbsp) and we can assume it is
	     *	just for a FONT change.  We thus will not create
	     *	another line break nor add to the current left
	     *	indentation. - FM
	     */
	    if (!(me->inP == FALSE && me->in_word == NO)) {
		HText_appendParagraph(me->text);
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		HText_setLastChar(me->text, ' ');
		me->in_word = NO;
		me->inP = FALSE;
	    }
	    CHECK_ID(HTML_H_ID);
	    break;
	}

	if (present && present[HTML_H_ALIGN] &&
	    value[HTML_H_ALIGN] && *value[HTML_H_ALIGN]) {
	    if (!strcasecomp(value[HTML_H_ALIGN], "center"))
		change_paragraph_style(me, styles[HTML_HCENTER]);
	    else if (!strcasecomp(value[HTML_H_ALIGN], "right"))
		change_paragraph_style(me, styles[HTML_HRIGHT]);
	    else if (!strcasecomp(value[HTML_H_ALIGN], "left") ||
		     !strcasecomp(value[HTML_H_ALIGN], "justify"))
		change_paragraph_style(me, styles[HTML_HLEFT]);
	    else
		change_paragraph_style(me, styles[ElementNumber]);
	} else if (me->Division_Level >= 0) {
	    if (me->DivisionAlignments[me->Division_Level] == HT_CENTER) {
		change_paragraph_style(me, styles[HTML_HCENTER]);
	    } else if (me->DivisionAlignments[me->Division_Level] == HT_LEFT) {
		change_paragraph_style(me, styles[HTML_HLEFT]);
	    } else if (me->DivisionAlignments[me->Division_Level] == HT_RIGHT) {
		change_paragraph_style(me, styles[HTML_HRIGHT]);
	    }
	} else {
	    change_paragraph_style(me, styles[ElementNumber]);
	}
	UPDATE_STYLE;
	CHECK_ID(HTML_H_ID);

	if ((bold_headers == TRUE ||
	     (ElementNumber == HTML_H1 && bold_H1 == TRUE)) &&
	    (styles[ElementNumber]->font&HT_BOLD)) {
	    if (me->inBoldA == FALSE && me->inBoldH == FALSE) {
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
	    }
	    me->inBoldH = TRUE;
	}
	break;

    case HTML_P:
	LYHandlePlike(me, present, value, include, HTML_P_ALIGN, TRUE);
	CHECK_ID(HTML_P_ID);
	break;

    case HTML_BR:
	UPDATE_STYLE;
	CHECK_ID(HTML_GEN_ID);
	  /* Add a \r (new line) if these three conditions are true:
	   *   1. We are not collapsing BR's, and
	   *   2. The previous line has text on it, or
	   *   3. This line has text on it.
	   * Otherwise, don't do anything. -DH 980814, TD 980827
	   */
	if ((LYCollapseBRs == FALSE &&
	     HText_PreviousLineSize(me->text, FALSE)) ||
	    HText_LastLineSize(me->text, FALSE)) {
	    HText_setLastChar(me->text, ' ');  /* absorb white space */
	    HText_appendCharacter(me->text, '\r');
	}
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_WBR:
	UPDATE_STYLE;
	CHECK_ID(HTML_GEN_ID);
	HText_setBreakPoint(me->text);
	break;

    case HTML_HY:
    case HTML_SHY:
	UPDATE_STYLE;
	CHECK_ID(HTML_GEN_ID);
	HText_appendCharacter(me->text, LY_SOFT_HYPHEN);
	break;

    case HTML_HR:
	{
	    int width;

	    /*
	     *	Start a new line only if we had printable
	     *	characters following the previous newline,
	     *	or remove the previous line if both it and
	     *	the last line are blank. - FM
	     */
	    UPDATE_STYLE;
	    if (HText_LastLineSize(me->text, FALSE)) {
		HText_setLastChar(me->text, ' ');  /* absorb white space */
		HText_appendCharacter(me->text, '\r');
	    } else if (!HText_PreviousLineSize(me->text, FALSE)) {
		HText_RemovePreviousLine(me->text);
	    }
	    me->in_word = NO;
	    me->inP = FALSE;

	    /*
	     *	Add an ID link if needed. - FM
	     */
	    CHECK_ID(HTML_HR_ID);

	   /*
	    *  Center lines within the current margins, if
	    *  a right or left ALIGNment is not specified.
	    *  If WIDTH="#%" is given and not garbage,
	    *  use that to calculate the width, otherwise
	    *  use the default width. - FM
	    */
	    if (present && present[HTML_HR_ALIGN] && value[HTML_HR_ALIGN]) {
		if (!strcasecomp(value[HTML_HR_ALIGN], "right")) {
		    me->sp->style->alignment = HT_RIGHT;
		} else if (!strcasecomp(value[HTML_HR_ALIGN], "left")) {
		    me->sp->style->alignment = HT_LEFT;
		} else {
		    me->sp->style->alignment = HT_CENTER;
		}
	    } else {
		me->sp->style->alignment = HT_CENTER;
	    }
	    width = LYcols - 1 -
		    me->new_style->leftIndent - me->new_style->rightIndent;
	    if (present && present[HTML_HR_WIDTH] && value[HTML_HR_WIDTH] &&
		isdigit(UCH(*value[HTML_HR_WIDTH])) &&
		value[HTML_HR_WIDTH][strlen(value[HTML_HR_WIDTH])-1] == '%') {
		char *percent = NULL;
		int Percent, Width;
		StrAllocCopy(percent, value[HTML_HR_WIDTH]);
		percent[strlen(percent)-1] = '\0';
		Percent = atoi(percent);
		if (Percent > 100 || Percent < 1)
		    width -= 5;
		else {
		    Width = (width * Percent) / 100;
		    if (Width < 1)
			width = 1;
		    else
			width = Width;
		}
		FREE(percent);
	    } else {
		width -= 5;
	    }
	    for (i = 0; i < width; i++)
		HTML_put_character(me, '_');
	    HText_appendCharacter(me->text, '\r');
	    me->in_word = NO;
	    me->inP = FALSE;

	    /*
	     *	Reset the alignment appropriately
	     *	for the division and/or block. - FM
	     */
	    if (me->List_Nesting_Level < 0 &&
		me->Division_Level >= 0) {
		me->sp->style->alignment =
				me->DivisionAlignments[me->Division_Level];
	    } else if (!strcmp(me->sp->style->name, "HeadingCenter") ||
		       !strcmp(me->sp->style->name, "Heading1")) {
		me->sp->style->alignment = HT_CENTER;
	    } else if (!strcmp(me->sp->style->name, "HeadingRight")) {
		me->sp->style->alignment = HT_RIGHT;
	    } else {
		me->sp->style->alignment = HT_LEFT;
	    }

	    /*
	     *	Add a blank line and set the second line
	     *	indentation for lists and addresses, or a
	     *	paragraph separator for other blocks. - FM
	     */
	    if (me->List_Nesting_Level >= 0 ||
		me->sp[0].tag_number == HTML_ADDRESS) {
		HText_setLastChar(me->text, ' ');  /* absorb white space */
		HText_appendCharacter(me->text, '\r');
	    } else {
		HText_appendParagraph(me->text);
	    }
	}
	break;

    case HTML_TAB:
	if (!present) { /* Bad tag.  Must have at least one attribute. - FM */
	    CTRACE((tfp, "HTML: TAB tag has no attributes.  Ignored.\n"));
	    break;
	}
	/*
	 *  If page author is using TAB within a TABLE, it's probably
	 *  formatted specifically to work well for Lynx without simple
	 *  table tracking code.  Cancel tracking, it would only make
	 *  things worse. - kw
	 */
	HText_cancelStbl(me->text);
	UPDATE_STYLE;

	CANT_JUSTIFY_THIS_LINE;
	if (present[HTML_TAB_ALIGN] && value[HTML_TAB_ALIGN] &&
	    (strcasecomp(value[HTML_TAB_ALIGN], "left") ||
	     !(present[HTML_TAB_TO] || present[HTML_TAB_INDENT]))) {
	    /*
	     *	Just ensure a collapsible space, until we have
	     *	the ALIGN and DP attributes implemented. - FM
	     */
	    HTML_put_character(me, ' ');
	    CTRACE((tfp, "HTML: ALIGN not 'left'.  Using space instead of TAB.\n"));

	} else if (!LYoverride_default_alignment(me) &&
		   me->current_default_alignment != HT_LEFT) {
	    /*
	     *	Just ensure a collapsible space, until we
	     *	can replace HText_getCurrentColumn() in
	     *	GridText.c with code which doesn't require
	     *	that the alignment be HT_LEFT. - FM
	     */
	    HTML_put_character(me, ' ');
	    CTRACE((tfp, "HTML: Not HT_LEFT.  Using space instead of TAB.\n"));

	} else if ((present[HTML_TAB_TO] &&
		    value[HTML_TAB_TO] && *value[HTML_TAB_TO]) ||
		   (present[HTML_TAB_INDENT] &&
		    value[HTML_TAB_INDENT] &&
		    isdigit(UCH(*value[HTML_TAB_INDENT])))) {
	    int column, target = -1;
	    int enval = 2;

	    column = HText_getCurrentColumn(me->text);
	    if (present[HTML_TAB_TO]) {
		/*
		 *  TO has priority over INDENT if both are present. - FM
		 */
		StrAllocCopy(temp, value[HTML_TAB_TO]);
		TRANSLATE_AND_UNESCAPE_TO_STD(&temp);
		if (*temp) {
		    target = HText_getTabIDColumn(me->text, temp);
		}
	    } else if (!(temp && *temp) && present[HTML_TAB_INDENT] &&
		       value[HTML_TAB_INDENT] &&
		       isdigit(UCH(*value[HTML_TAB_INDENT]))) {
		/*
		 *  The INDENT value is in "en" (enval per column) units.
		 *  Divide it by enval, rounding odd values up. - FM
		 */
		target =
		   (int)(((1.0 * atoi(value[HTML_TAB_INDENT]))/enval)+(0.5));
	    }
	    FREE(temp);
	    /*
	     *	If we are being directed to a column too far to the left
	     *	or right, just add a collapsible space, otherwise, add the
	     *	appropriate number of spaces. - FM
	     */

	    if (target < column ||
		target > HText_getMaximumColumn(me->text)) {
		HTML_put_character(me, ' ');
		CTRACE((tfp, "HTML: Column out of bounds.  Using space instead of TAB.\n"));
	    } else {
		for (i = column; i < target; i++)
		    HText_appendCharacter(me->text, ' ');
		HText_setLastChar(me->text, ' ');  /* absorb white space */
	    }
	}
	me->in_word = NO;

	/*
	 *  If we have an ID attribute, save it together
	 *  with the value of the column we've reached. - FM
	 */
	if (present[HTML_TAB_ID] &&
	    value[HTML_TAB_ID] && *value[HTML_TAB_ID]) {
	    StrAllocCopy(temp, value[HTML_TAB_ID]);
	    TRANSLATE_AND_UNESCAPE_TO_STD(&temp);
	    if (*temp)
		HText_setTabID(me->text, temp);
	    FREE(temp);
	}
	break;

    case HTML_BASEFONT:
	break;

    case HTML_FONT:

	/*
	 *  FONT *may* have been declared SGML_EMPTY in HTMLDTD.c, and
	 *  SGML_character() in SGML.c *may* check for a FONT end
	 *  tag to call HTML_end_element() directly (with a
	 *  check in that to bypass decrementing of the HTML
	 *  parser's stack).  Or this may have been really a </FONT>
	 *  end tag, for which some incarnations of SGML.c would fake
	 *  a <FONT> start tag instead. - fm & kw
	 *
	 *  But if we have an open FONT, DON'T close that one now,
	 *  since FONT tags can be legally nested AFAIK, and Lynx
	 *  currently doesn't do anything with them anyway... - kw
	 */
#ifdef NOTUSED_FOTEMODS
	if (me->inFONT == TRUE)
	    HTML_end_element(me, HTML_FONT, &include);
#endif /* NOTUSED_FOTEMODS */

	/*
	 *  Set flag to know we are in a FONT container, and
	 *  add code to do something about it, someday. - FM
	 */
	me->inFONT = TRUE;
	break;

    case HTML_B:			/* Physical character highlighting */
    case HTML_BLINK:
    case HTML_I:
    case HTML_U:

    case HTML_CITE:			/* Logical character highlighting */
    case HTML_EM:
    case HTML_STRONG:
	UPDATE_STYLE;
	me->Underline_Level++;
	CHECK_ID(HTML_GEN_ID);
	/*
	 *  Ignore this if inside of a bold anchor or header.
	 *  Can't display both underline and bold at same time.
	 */
	if (me->inBoldA == TRUE || me->inBoldH == TRUE) {
	    CTRACE((tfp, "Underline Level is %d\n", me->Underline_Level));
	    break;
	}
	if (me->inUnderline == FALSE) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    me->inUnderline = TRUE;
	    CTRACE((tfp,"Beginning underline\n"));
	} else {
	    CTRACE((tfp,"Underline Level is %d\n", me->Underline_Level));
	}
	break;

    case HTML_ABBREV:	/* Miscellaneous character containers */
    case HTML_ACRONYM:
    case HTML_AU:
    case HTML_AUTHOR:
    case HTML_BIG:
    case HTML_CODE:
    case HTML_DFN:
    case HTML_KBD:
    case HTML_SAMP:
    case HTML_SMALL:
    case HTML_TT:
    case HTML_VAR:
	CHECK_ID(HTML_GEN_ID);
	break; /* ignore */

    case HTML_SUP:
	if (isxdigit(UCH(HText_getLastChar(me->text)))) {
	    HText_appendCharacter(me->text, '^');
	}
	CHECK_ID(HTML_GEN_ID);
	break;

    case HTML_SUB:
	HText_appendCharacter(me->text, '[');
	CHECK_ID(HTML_GEN_ID);
	break;

    case HTML_DEL:
    case HTML_S:
    case HTML_STRIKE:
	CHECK_ID(HTML_GEN_ID);
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, "[DEL:");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	me->in_word = NO;
	break;

    case HTML_INS:
	CHECK_ID(HTML_GEN_ID);
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, "[INS:");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	me->in_word = NO;
	break;

    case HTML_Q:
	CHECK_ID(HTML_GEN_ID);
	/*
	 *  Should check LANG and/or DIR attributes, and the
	 *  me->node_anchor->charset and/or yet to be added
	 *  structure elements, to determine whether we should
	 *  use chevrons, but for now we'll always use double-
	 *  or single-quotes. - FM
	 */
	if (!(me->Quote_Level & 1))
	    HTML_put_character(me, '"');
	else
	    HTML_put_character(me, '`');
	me->Quote_Level++;
	break;

    case HTML_PRE:				/* Formatted text */
	/*
	**  Set our inPRE flag to FALSE so that a newline
	**  immediately following the PRE start tag will
	**  be ignored.  HTML_put_character() will set it
	**  to TRUE when the first character within the
	**  PRE block is received. - FM
	*/
	me->inPRE = FALSE;
	/* FALLTHRU */
    case HTML_LISTING:				/* Literal text */
	/* FALLTHRU */
    case HTML_XMP:
	/* FALLTHRU */
    case HTML_PLAINTEXT:
	change_paragraph_style(me, styles[ElementNumber]);
	UPDATE_STYLE;
	CHECK_ID(HTML_GEN_ID);
	if (me->comment_end)
	    HText_appendText(me->text, me->comment_end);
	break;

    case HTML_BLOCKQUOTE:
    case HTML_BQ:
	change_paragraph_style(me, styles[ElementNumber]);
	UPDATE_STYLE;
	if (me->sp->tag_number == (int) ElementNumber)
	    LYEnsureDoubleSpace(me);
	CHECK_ID(HTML_BQ_ID);
	break;

    case HTML_NOTE:
	change_paragraph_style(me, styles[ElementNumber]);
	UPDATE_STYLE;
	if (me->sp->tag_number == (int) ElementNumber)
	    LYEnsureDoubleSpace(me);
	CHECK_ID(HTML_NOTE_ID);
	{
	    char *note = NULL;

	    /*
	     *	Indicate the type of NOTE.
	     */
	    if (present && present[HTML_NOTE_CLASS] &&
		value[HTML_NOTE_CLASS] &&
		(!strcasecomp(value[HTML_NOTE_CLASS], "CAUTION") ||
		 !strcasecomp(value[HTML_NOTE_CLASS], "WARNING"))) {
		StrAllocCopy(note, value[HTML_NOTE_CLASS]);
		LYUpperCase(note);
		StrAllocCat(note, ":");
	    } else if (present && present[HTML_NOTE_ROLE] &&
		       value[HTML_NOTE_ROLE] &&
		       (!strcasecomp(value[HTML_NOTE_ROLE], "CAUTION") ||
			!strcasecomp(value[HTML_NOTE_ROLE], "WARNING"))) {
		StrAllocCopy(note, value[HTML_NOTE_ROLE]);
		LYUpperCase(note);
		StrAllocCat(note, ":");
	    } else {
		StrAllocCopy(note, "NOTE:");
	    }
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    HTML_put_string(me, note);
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    HTML_put_character(me, ' ');
	    CAN_JUSTIFY_START;
	    FREE(note);
	}
	CAN_JUSTIFY_START;
	me->inLABEL = TRUE;
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_ADDRESS:
	change_paragraph_style(me, styles[ElementNumber]);
	UPDATE_STYLE;
	if (me->sp->tag_number == (int) ElementNumber)
	    LYEnsureDoubleSpace(me);
	CHECK_ID(HTML_ADDRESS_ID);
	break;

    case HTML_DL:
	me->List_Nesting_Level++;  /* increment the List nesting level */
	if (me->List_Nesting_Level <= 0) {
	    change_paragraph_style(me, present && present[HTML_DL_COMPACT]
				      ? styles[HTML_DLC] : styles[HTML_DL]);

	} else if (me->List_Nesting_Level >= 6) {
	    change_paragraph_style(me, present && present[HTML_DL_COMPACT]
				      ? styles[HTML_DLC6] : styles[HTML_DL6]);

	} else {
	    change_paragraph_style(me, present && present[HTML_DL_COMPACT]
		 ? styles[(HTML_DLC1 - 1) + me->List_Nesting_Level]
		 : styles[(HTML_DL1 - 1) + me->List_Nesting_Level]);
	}
	UPDATE_STYLE;	  /* update to the new style */
	CHECK_ID(HTML_DL_ID);

	break;

    case HTML_DLC:
	me->List_Nesting_Level++;  /* increment the List nesting level */
	if (me->List_Nesting_Level <= 0) {
	    change_paragraph_style(me, styles[HTML_DLC]);

	} else if (me->List_Nesting_Level >= 6) {
	    change_paragraph_style(me, styles[HTML_DLC6]);

	} else {
	    change_paragraph_style(me,
			    styles[(HTML_DLC1 - 1) + me->List_Nesting_Level]);
	}
	UPDATE_STYLE;	  /* update to the new style */
	CHECK_ID(HTML_DL_ID);
	break;

    case HTML_DT:
	CHECK_ID(HTML_GEN_ID);
	if (!me->style_change) {
	    BOOL in_line_1 = HText_inLineOne(me->text);
	    HTCoord saved_spaceBefore = me->sp->style->spaceBefore;
	    HTCoord saved_spaceAfter = me->sp->style->spaceAfter;
	    /*
	     *  If there are several DT elements and this is not the first,
	     *  and the preceding DT element's first (and normally only) line
	     *  has not yet been ended, suppress intervening blank line by
	     *  temporarily modifying the paragraph style in place.  Ugly
	     *  but there's ample precedence. - kw
	     */
	    if (in_line_1) {
		me->sp->style->spaceBefore = 0;	/* temporary change */
		me->sp->style->spaceAfter = 0;	/* temporary change */
	    }
	    HText_appendParagraph(me->text);
	    me->sp->style->spaceBefore = saved_spaceBefore; /* undo */
	    me->sp->style->spaceAfter = saved_spaceAfter; /* undo */
	    me->in_word = NO;
	    me->sp->style->alignment = HT_LEFT;
	}
	me->inP = FALSE;
	break;

    case HTML_DD:
	CHECK_ID(HTML_GEN_ID);
	HText_setLastChar(me->text, ' ');  /* absorb white space */
	if (!me->style_change)	{
	    if (HText_LastLineSize(me->text, FALSE)) {
		HText_appendCharacter(me->text, '\r');
	    } else {
		HText_NegateLineOne(me->text);
	    }
	} else {
	    UPDATE_STYLE;
	    HText_appendCharacter(me->text, '\t');
	}
	me->sp->style->alignment = HT_LEFT;
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_OL:
	/*
	 * Set the default TYPE.
	 */
	 me->OL_Type[(me->List_Nesting_Level < 11 ?
			 me->List_Nesting_Level+1 : 11)] = '1';

	/*
	 *  Check whether we have a starting sequence number,
	 *  or want to continue the numbering from a previous
	 *  OL in this nest. - FM
	 */
	if (present && (present[HTML_OL_SEQNUM] || present[HTML_OL_START])) {
	    int seqnum;

	    /*
	     *	Give preference to the valid HTML 3.0 SEQNUM attribute name
	     *	over the Netscape START attribute name (too bad the Netscape
	     *	developers didn't read the HTML 3.0 specs before re-inventing
	     *	the "wheel" as "we'll"). - FM
	     */
	    if (present[HTML_OL_SEQNUM] &&
		value[HTML_OL_SEQNUM] && *value[HTML_OL_SEQNUM]) {
		seqnum = atoi(value[HTML_OL_SEQNUM]);
	    } else if (present[HTML_OL_START] &&
		       value[HTML_OL_START] && *value[HTML_OL_START]) {
		seqnum = atoi(value[HTML_OL_START]);
	    } else {
		seqnum = 1;
	    }

	    /*
	     *	Don't allow negative numbers less than
	     *	or equal to our flags, or numbers less
	     *	than 1 if an Alphabetic or Roman TYPE. - FM
	     */
	    if (present[HTML_OL_TYPE] && value[HTML_OL_TYPE]) {
		if (*value[HTML_OL_TYPE] == 'A') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'A';
		    if (seqnum < 1)
			seqnum = 1;
		} else if (*value[HTML_OL_TYPE] == 'a') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'a';
		    if (seqnum < 1)
			seqnum = 1;
		} else if (*value[HTML_OL_TYPE] == 'I') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'I';
		    if (seqnum < 1)
			seqnum = 1;
		} else if (*value[HTML_OL_TYPE] == 'i') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'i';
		    if (seqnum < 1)
			seqnum = 1;
		} else {
		  if (seqnum <= OL_VOID)
		      seqnum = OL_VOID + 1;
		}
	    } else if (seqnum <= OL_VOID) {
		seqnum = OL_VOID + 1;
	    }

	    me->OL_Counter[(me->List_Nesting_Level < 11 ?
			       me->List_Nesting_Level+1 : 11)] = seqnum;

	} else if (present && present[HTML_OL_CONTINUE]) {
	    me->OL_Counter[me->List_Nesting_Level < 11 ?
			      me->List_Nesting_Level+1 : 11] = OL_CONTINUE;

	} else {
	    me->OL_Counter[(me->List_Nesting_Level < 11 ?
			       me->List_Nesting_Level+1 : 11)] = 1;
	    if (present && present[HTML_OL_TYPE] && value[HTML_OL_TYPE]) {
		if (*value[HTML_OL_TYPE] == 'A') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'A';
		} else if (*value[HTML_OL_TYPE] == 'a') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'a';
		} else if (*value[HTML_OL_TYPE] == 'I') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'I';
		} else if (*value[HTML_OL_TYPE] == 'i') {
		    me->OL_Type[(me->List_Nesting_Level < 11 ?
				    me->List_Nesting_Level+1 : 11)] = 'i';
		}
	    }
	}
	me->List_Nesting_Level++;

	if (me->List_Nesting_Level <= 0) {
	    change_paragraph_style(me, styles[ElementNumber]);

	} else if (me->List_Nesting_Level >= 6) {
	    change_paragraph_style(me, styles[HTML_OL6]);

	} else {
	    change_paragraph_style(me,
			  styles[HTML_OL1 + me->List_Nesting_Level - 1]);
	}
	UPDATE_STYLE;  /* update to the new style */
	CHECK_ID(HTML_OL_ID);
	break;

    case HTML_UL:
	me->List_Nesting_Level++;

	if (me->List_Nesting_Level <= 0) {
	    if (!(present && present[HTML_UL_PLAIN]) &&
		!(present && present[HTML_UL_TYPE] &&
		  value[HTML_UL_TYPE] &&
		  0==strcasecomp(value[HTML_UL_TYPE], "PLAIN"))) {
		change_paragraph_style(me, styles[ElementNumber]);
	    } else {
		change_paragraph_style(me, styles[HTML_DIR]);
		ElementNumber = HTML_DIR;
	    }

	} else if (me->List_Nesting_Level >= 6) {
	    if (!(present && present[HTML_UL_PLAIN]) &&
		!(present && present[HTML_UL_TYPE] &&
		  value[HTML_UL_TYPE] &&
		  0==strcasecomp(value[HTML_UL_TYPE], "PLAIN"))) {
		change_paragraph_style(me, styles[HTML_OL6]);
	    } else {
		change_paragraph_style(me, styles[HTML_MENU6]);
		ElementNumber = HTML_DIR;
	    }

	} else {
	    if (!(present && present[HTML_UL_PLAIN]) &&
		!(present && present[HTML_UL_TYPE] &&
		  value[HTML_UL_TYPE] &&
		  0==strcasecomp(value[HTML_UL_TYPE], "PLAIN"))) {
		change_paragraph_style(me,
			  styles[HTML_OL1 + me->List_Nesting_Level - 1]);
	    } else {
		change_paragraph_style(me,
			  styles[HTML_MENU1 + me->List_Nesting_Level - 1]);
		ElementNumber = HTML_DIR;
	    }
	}
	UPDATE_STYLE;  /* update to the new style */
	CHECK_ID(HTML_UL_ID);
	break;

    case HTML_MENU:
    case HTML_DIR:
	me->List_Nesting_Level++;

	if (me->List_Nesting_Level <= 0) {
	    change_paragraph_style(me, styles[ElementNumber]);

	} else if (me->List_Nesting_Level >= 6) {
	    change_paragraph_style(me, styles[HTML_MENU6]);

	} else {
	    change_paragraph_style(me,
			  styles[HTML_MENU1 + me->List_Nesting_Level - 1]);
	}
	UPDATE_STYLE;  /* update to the new style */
	CHECK_ID(HTML_UL_ID);
	break;

    case HTML_LH:
	UPDATE_STYLE;  /* update to the new style */
	HText_appendParagraph(me->text);
	CHECK_ID(HTML_GEN_ID);
	HTML_put_character(me, HT_NON_BREAK_SPACE);
	HText_setLastChar(me->text, ' ');
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_LI:
	UPDATE_STYLE;  /* update to the new style */
	HText_appendParagraph(me->text);
	me->sp->style->alignment = HT_LEFT;
	CHECK_ID(HTML_LI_ID);
	{
	    int surrounding_tag_number = me->sp[0].tag_number;
	    /*
	     *  No, a LI should never occur directly within another LI,
	     *  but this may result from incomplete error recovery.
	     *  So check one more surrounding level in this case. - kw
	     */
	    if (surrounding_tag_number == HTML_LI &&
		me->sp < (me->stack + MAX_NESTING - 1))
		surrounding_tag_number = me->sp[1].tag_number;
	    if (surrounding_tag_number == HTML_OL) {
		char number_string[20];
		int counter, seqnum;
		char seqtype;

		counter = me->List_Nesting_Level < 11 ?
		    me->List_Nesting_Level : 11;
		if (present && present[HTML_LI_TYPE] && value[HTML_LI_TYPE]) {
		    if (*value[HTML_LI_TYPE] == '1') {
			me->OL_Type[counter] = '1';
		    } else if (*value[HTML_LI_TYPE] == 'A') {
			me->OL_Type[counter] = 'A';
		    } else if (*value[HTML_LI_TYPE] == 'a') {
			me->OL_Type[counter] = 'a';
		    } else if (*value[HTML_LI_TYPE] == 'I') {
			me->OL_Type[counter] = 'I';
		    } else if (*value[HTML_LI_TYPE] == 'i') {
			me->OL_Type[counter] = 'i';
		    }
		}
		if (present && present[HTML_LI_VALUE] &&
		    ((value[HTML_LI_VALUE] != NULL) &&
		     (*value[HTML_LI_VALUE] != '\0')) &&
		    ((isdigit(UCH(*value[HTML_LI_VALUE]))) ||
		     (*value[HTML_LI_VALUE] == '-' &&
		      isdigit(UCH(*(value[HTML_LI_VALUE] + 1)))))) {
		    seqnum = atoi(value[HTML_LI_VALUE]);
		    if (seqnum <= OL_VOID)
			seqnum = OL_VOID + 1;
		    seqtype = me->OL_Type[counter];
		    if (seqtype != '1' && seqnum < 1)
			seqnum = 1;
		    me->OL_Counter[counter] = seqnum + 1;
		} else if (me->OL_Counter[counter] >= OL_VOID) {
		    seqnum = me->OL_Counter[counter]++;
		    seqtype = me->OL_Type[counter];
		    if (seqtype != '1' && seqnum < 1) {
			seqnum = 1;
			me->OL_Counter[counter] = seqnum + 1;
		    }
		} else {
		    seqnum = me->Last_OL_Count + 1;
		    seqtype = me->Last_OL_Type;
		    for (i = (counter - 1); i >= 0; i--) {
			if (me->OL_Counter[i] > OL_VOID) {
			    seqnum = me->OL_Counter[i]++;
			    seqtype = me->OL_Type[i];
			    i = 0;
			}
		    }
		}
		if (seqtype == 'A') {
		    sprintf(number_string, LYUppercaseA_OL_String(seqnum));
		} else if (seqtype == 'a') {
		    sprintf(number_string, LYLowercaseA_OL_String(seqnum));
		} else if (seqtype == 'I') {
		    sprintf(number_string, LYUppercaseI_OL_String(seqnum));
		} else if (seqtype == 'i') {
		    sprintf(number_string, LYLowercaseI_OL_String(seqnum));
		} else {
		    sprintf(number_string, "%2d.", seqnum);
		}
		me->Last_OL_Count = seqnum;
		me->Last_OL_Type = seqtype;
		/*
		 *	Hack, because there is no append string!
		 */
		for (i = 0; number_string[i] != '\0'; i++)
		    if (number_string[i] == ' ')
			HTML_put_character(me, HT_NON_BREAK_SPACE);
		    else
			HTML_put_character(me, number_string[i]);

		/*
		 *	Use HTML_put_character so that any other spaces
		 *	coming through will be collapsed.  We'll use
		 *	nbsp, so it won't break at the spacing character
		 *	if there are no spaces in the subsequent text up
		 *	to the right margin, but will declare it as a
		 *	normal space to ensure collapsing if a normal
		 *	space does immediately follow it. - FM
		 */
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		HText_setLastChar(me->text, ' ');
	    } else if (surrounding_tag_number == HTML_UL) {
		/*
		 *	Hack, because there is no append string!
		 */
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		switch(me->List_Nesting_Level % 7) {
		case 0:
		    HTML_put_character(me, '*');
		    break;
		case 1:
		    HTML_put_character(me, '+');
		    break;
		case 2:
		    HTML_put_character(me, 'o');
		    break;
		case 3:
		    HTML_put_character(me, '#');
		    break;
		case 4:
		    HTML_put_character(me, '@');
		    break;
		case 5:
		    HTML_put_character(me, '-');
		    break;
		case 6:
		    HTML_put_character(me, '=');
		    break;

		}
		/*
		 *	Keep using HTML_put_character so that any other
		 *	spaces coming through will be collapsed.  We use
		 *	nbsp, so we won't wrap at the spacing character
		 *	if there are no spaces in the subsequent text up
		 *	to the right margin, but will declare it as a
		 *	normal space to ensure collapsing if a normal
		 *	space does immediately follow it. - FM
		 */
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		HText_setLastChar(me->text, ' ');
	    } else {
		/*
		 *	Hack, because there is no append string!
		 */
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		HTML_put_character(me, HT_NON_BREAK_SPACE);
		HText_setLastChar(me->text, ' ');
	    }
	}
	CAN_JUSTIFY_START;
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_SPAN:
	CHECK_ID(HTML_GEN_ID);
	/*
	 *  Should check LANG and/or DIR attributes, and the
	 *  me->node_anchor->charset and/or yet to be added
	 *  structure elements, and do something here. - FM
	 */
	break;

    case HTML_BDO:
	CHECK_ID(HTML_GEN_ID);
	/*
	 *  Should check DIR (and LANG) attributes, and the
	 *  me->node_anchor->charset and/or yet to be added
	 *  structure elements, and do something here. - FM
	 */
	break;

    case HTML_SPOT:
	CHECK_ID(HTML_GEN_ID);
	break;

    case HTML_FN:
	change_paragraph_style(me, styles[ElementNumber]);
	UPDATE_STYLE;
	if (me->sp->tag_number == (int) ElementNumber)
	    LYEnsureDoubleSpace(me);
	CHECK_ID(HTML_FN_ID);
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, "FOOTNOTE:");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	CAN_JUSTIFY_START
	me->inLABEL = TRUE;
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_A:
	    /*
	     *  If we are looking for client-side image maps,
	     *  then handle an A within a MAP that has a COORDS
	     *  attribute as an AREA tag.  Unfortunately we lose
	     *  the anchor text this way for the LYNXIMGMAP, we
	     *  would have to do much more parsing to collect it.
	     *  After potentially handling the A as AREA, always return
	     *  immediately if only looking for image maps, without
	     *  pushing anything on the style stack. - kw
	     */
	if (me->map_address && present && present[HTML_A_COORDS])
	    LYStartArea(me,
			present[HTML_A_HREF] ? value[HTML_A_HREF] : NULL,
			NULL,
			present[HTML_A_TITLE] ? value[HTML_A_TITLE] : NULL,
			tag_charset);
	if (LYMapsOnly) {
	    return HT_OK;
	}
	/*
	 *  A may have been declared SGML_EMPTY in HTMLDTD.c, and
	 *  SGML_character() in SGML.c may check for an A end
	 *  tag to call HTML_end_element() directly (with a
	 *  check in that to bypass decrementing of the HTML
	 *  parser's stack), so if we have an open A, close
	 *  that one now. - FM & kw
	 */
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	/*
	 *  Set to know we are in an anchor.
	 */
	me->inA = TRUE;

	/*
	 *  Load id_string if we have an ID or NAME. - FM
	 */
	if (present && present[HTML_A_ID] &&
	    value[HTML_A_ID] && *value[HTML_A_ID]) {
	    StrAllocCopy(id_string, value[HTML_A_ID]);
	} else if (present && present[HTML_A_NAME] &&
		   value[HTML_A_NAME] && *value[HTML_A_NAME]) {
	    StrAllocCopy(id_string, value[HTML_A_NAME]);
	}
	if (id_string) {
	    TRANSLATE_AND_UNESCAPE_TO_STD(&id_string);
	    if (*id_string == '\0') {
		FREE(id_string);
	    }
	}

	/*
	 *  Handle the reference. - FM
	 */
	if (present && present[HTML_A_HREF]) {
#ifndef DONT_TRACK_INTERNAL_LINKS
	    if (present[HTML_A_ISMAP])
		intern_flag = FALSE;
	    else
		CHECK_FOR_INTERN(intern_flag,value[HTML_A_HREF]);
#endif
	    /*
	     *	Prepare to do housekeeping on the reference. - FM
	     */
	    if (!value[HTML_A_HREF] || *value[HTML_A_HREF] == '\0') {
		StrAllocCopy(href, me->node_anchor->address);
	    } else if (*value[HTML_A_HREF] == '#') {
		StrAllocCopy(href, me->node_anchor->address);
		if (strlen(value[HTML_A_HREF]) > 1) {
		    StrAllocCat(href, value[HTML_A_HREF]);
		}
	    } else {
		StrAllocCopy(href, value[HTML_A_HREF]);
	    }
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);

	    /*
	     *	Deal with our ftp gateway kludge. - FM
	     */
	    if (!url_type && !strncmp(href, "/foo/..", 7) &&
		(!strncmp(me->node_anchor->address, "ftp:", 4) ||
		 !strncmp(me->node_anchor->address, "file:", 5))) {
		for (i = 0; (href[i] = href[i+7]) != 0; i++)
		    ;
	    }

	    /*
	     *	Set to know we are making the content bold.
	     */
	    me->inBoldA = TRUE;

	    /*
	     *	Check whether a base tag is in effect. - FM
	     */
	    if ((me->inBASE && *href != '\0' && *href != '#') &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((*href != '\0' && *href != '#' &&
				 me->inBASE) ?
			       me->base_href : me->node_anchor->address));
	} else {
	    if (bold_name_anchors == TRUE) {
		me->inBoldA = TRUE;
	    }
	}
#ifndef DONT_TRACK_INTERNAL_LINKS
	if (present && present[HTML_A_TYPE] && value[HTML_A_TYPE]) {
	    StrAllocCopy(temp, value[HTML_A_TYPE]);
	    if (!intern_flag && href &&
		!strcasecomp(value[HTML_A_TYPE], HTAtom_name(LINK_INTERNAL)) &&
		!LYIsUIPage3(me->node_anchor->address, UIP_LIST_PAGE, 0) &&
		!LYIsUIPage3(me->node_anchor->address, UIP_ADDRLIST_PAGE, 0) &&
		0 != strncmp(me->node_anchor->address, "LYNXIMGMAP:", 11)) {
		/* Some kind of spoof?
		** Found TYPE="internal link" but not in a valid context
		** where we have written it. - kw
		*/
		CTRACE((tfp, "HTML: Found invalid HREF=\"%s\" TYPE=\"%s\"!\n",
			    href, temp));
		FREE(temp);
	    }
	}
#endif /* DONT_TRACK_INTERNAL_LINKS */

	me->CurrentA = HTAnchor_findChildAndLink(
			me->node_anchor,			/* Parent */
			id_string,				/* Tag */
			href,					/* Address */
			temp ?
		(HTLinkType*)HTAtom_for(temp) : INTERN_LT);  /* Type */
	FREE(temp);
	FREE(id_string);

	if (me->CurrentA && present) {
	    if (present[HTML_A_TITLE] &&
		value[HTML_A_TITLE] && *value[HTML_A_TITLE] != '\0') {
		StrAllocCopy(title, value[HTML_A_TITLE]);
		TRANSLATE_AND_UNESCAPE_ENTITIES(&title, TRUE, FALSE);
		LYTrimHead(title);
		LYTrimTail(title);
		if (*title == '\0') {
		    FREE(title);
		}
	    }
	    if (present[HTML_A_ISMAP])
		dest_ismap = TRUE;
	    if (present[HTML_A_CHARSET] &&
		value[HTML_A_CHARSET] && *value[HTML_A_CHARSET] != '\0') {
		/*
		**  Set up to load the anchor's chartrans structures
		**  appropriately for the current display character
		**  set if it can handle what's claimed. - FM
		*/
		StrAllocCopy(temp, value[HTML_A_CHARSET]);
		TRANSLATE_AND_UNESCAPE_TO_STD(&temp);
		dest_char_set = UCGetLYhndl_byMIME(temp);
		if (dest_char_set < 0) {
			dest_char_set = UCLYhndl_for_unrec;
		}
	    }
	    if (title != NULL || dest_ismap == TRUE || dest_char_set >= 0) {
		dest = HTAnchor_parent(
			HTAnchor_followMainLink((HTAnchor*)me->CurrentA)
				      );
	    }
	    if (dest && title != NULL && HTAnchor_title(dest) == NULL)
		HTAnchor_setTitle(dest, title);
	    if (dest && dest_ismap)
		dest->isISMAPScript = TRUE;
	    /* Don't allow CHARSET attribute to change *this* document's
	       charset assumption. - kw */
	    if (dest && dest != me->node_anchor && dest_char_set >= 0) {
		/*
		**  Load the anchor's chartrans structures.
		**  This should be done more intelligently
		**  when setting up the structured object,
		**  but it gets the job done for now. - FM
		*/
		HTAnchor_setUCInfoStage(dest, dest_char_set,
					UCT_STAGE_MIME,
					UCT_SETBY_DEFAULT);
		HTAnchor_setUCInfoStage(dest, dest_char_set,
					UCT_STAGE_PARSER,
					UCT_SETBY_LINK);
	    }
	    FREE(temp);
	    dest = NULL;
	    dest_ismap = FALSE;
	    FREE(title);
	}
	me->CurrentANum = HText_beginAnchor(me->text,
					    me->inUnderline, me->CurrentA);
	if (me->inBoldA == TRUE && me->inBoldH == FALSE)
	    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
#if defined(NOTUSED_FOTEMODS)
	/*
	 *  Close an HREF-less NAMED-ed now if we aren't making their
	 *  content bold, and let the check in HTML_end_element() deal
	 *  with any dangling end tag this creates. - FM
	 */
	if (href == NULL && me->inBoldA == FALSE) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
#else
	/*Close an HREF-less NAMED-ed now if force_empty_hrefless_a was
	    requested - VH*/
	if (href == NULL && force_empty_hrefless_a) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
#endif
	FREE(href);
	break;

    case HTML_IMG:			/* Images */
	/*
	 *  If we're in an anchor, get the destination, and if it's a
	 *  clickable image for the current anchor, set our flags for
	 *  faking a 0,0 coordinate pair, which typically returns the
	 *  image's default. - FM
	 */
	if (me->inA && me->CurrentA) {
	    if ((dest = HTAnchor_parent(
			HTAnchor_followMainLink((HTAnchor*)me->CurrentA)
				      )) != NULL) {
		if (dest->isISMAPScript == TRUE) {
		    dest_ismap = TRUE;
		    CTRACE((tfp, "HTML: '%s' is an ISMAP script\n",
				dest->address));
		} else if (present && present[HTML_IMG_ISMAP]) {
		    dest_ismap = TRUE;
		    dest->isISMAPScript = TRUE;
		    CTRACE((tfp, "HTML: Designating '%s' as an ISMAP script\n",
				dest->address));
		}
	    }
	}

#ifndef DONT_TRACK_INTERNAL_LINKS
	intern_flag = FALSE;	/* unless set below - kw */
#endif
	/*
	 *  If there's a USEMAP, resolve it. - FM
	 */
	if (present && present[HTML_IMG_USEMAP] &&
	    value[HTML_IMG_USEMAP] && *value[HTML_IMG_USEMAP]) {
	    StrAllocCopy(map_href, value[HTML_IMG_USEMAP]);
	    CHECK_FOR_INTERN(intern_flag,map_href);
	    url_type = LYLegitimizeHREF(me, &map_href, TRUE, TRUE);
	    /*
	     *	If map_href ended up zero-length or otherwise doesn't
	     *	have a hash, it can't be valid, so ignore it. - FM
	     */
	    if (strchr(map_href, '#') == NULL) {
		FREE(map_href);
	    }
	}

	/*
	 *  Handle a MAP reference if we have one at this point. - FM
	 */
	if (map_href) {
	    /*
	     *	If the MAP reference doesn't yet begin with a scheme,
	     *	check whether a base tag is in effect. - FM
	     */
	    if (!url_type && me->inBASE) {
		/*
		 *  If the
		 *  USEMAP value is a lone fragment and LYSeekFragMAPinCur
		 *  is set, we'll use the current document's URL for
		 *  resolving.	Otherwise use the BASE. - kw
		 */
		if ((*map_href == '#' &&
		     LYSeekFragMAPinCur == TRUE)) {
		    /*
		     *	Use reference related to the current stream. - FM
		     */
		    temp = HTParse(map_href, me->node_anchor->address,
				    PARSE_ALL);
		    StrAllocCopy(map_href, temp);
		    UseBASE = FALSE;
		} else {
		    /*
		     *	Use reference related to the base. - FM
		     */
		    temp = HTParse(map_href, me->base_href, PARSE_ALL);
		    StrAllocCopy(map_href, temp);
		    UseBASE = TRUE;
		}
		FREE(temp);
	    }

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&map_href,
			       ((UseBASE && me->inBASE) ?
			  me->base_href : me->node_anchor->address));
	    UseBASE = TRUE;

	    /*
	     *	If it's not yet a URL, resolve versus
	     *	the current document's address. - FM
	     */
	    if (!(url_type = is_url(map_href))) {
		temp = HTParse(map_href, me->node_anchor->address, PARSE_ALL);
		StrAllocCopy(map_href, temp);
		FREE(temp);
	    }

	    /*
	     *	Prepend our client-side MAP access field. - FM
	     */
	    StrAllocCopy(temp, "LYNXIMGMAP:");
	    StrAllocCat(temp, map_href);
	    StrAllocCopy(map_href, temp);
	    FREE(temp);
	}

	/*
	 *  Check whether we want to suppress the server-side
	 *  ISMAP link if a client-side MAP is present. - FM
	 */
	if (LYNoISMAPifUSEMAP && map_href && dest_ismap) {
	    dest_ismap = FALSE;
	    dest = NULL;
	}

	/*
	 *  Check for a TITLE attribute. - FM
	 */
	if (present && present[HTML_IMG_TITLE] &&
	    value[HTML_IMG_TITLE] && *value[HTML_IMG_TITLE]) {
	    StrAllocCopy(title, value[HTML_IMG_TITLE]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&title, TRUE, FALSE);
	    LYTrimHead(title);
	    LYTrimTail(title);
	    if (*title == '\0') {
		FREE(title);
	    }
	}

	/*
	 *  If there's an ALT string, use it, unless the ALT string
	 *  is zero-length or just spaces and we are making all SRCs
	 *  links or have a USEMAP link. - FM
	 */
	if (((present) &&
	     (present[HTML_IMG_ALT] && value[HTML_IMG_ALT])) &&
	    (!clickable_images ||
	     ((clickable_images || map_href) &&
	      *value[HTML_IMG_ALT] != '\0'))) {
	    StrAllocCopy(alt_string, value[HTML_IMG_ALT]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&alt_string,
					    me->UsePlainSpace, me->HiddenValue);
	    /*
	     *	If it's all spaces and we are making SRC or
	     *	USEMAP links, treat it as zero-length. - FM
	     */
	    if (clickable_images || map_href) {
		LYTrimHead(alt_string);
		LYTrimTail(alt_string);
		if (*alt_string == '\0') {
		    if (map_href) {
			StrAllocCopy(alt_string, (title ? title :
				     (temp = MakeNewMapValue(value,"USEMAP"))));
			FREE(temp);
		    } else if (dest_ismap) {
			StrAllocCopy(alt_string, (title ? title :
				     (temp = MakeNewMapValue(value,"ISMAP"))));
			FREE(temp);

		    } else if (me->inA == TRUE && dest) {
			StrAllocCopy(alt_string, (title ?
						  title :
						  VERBOSE_IMG(value, HTML_IMG_SRC, "[LINK]")));

		    } else {
			StrAllocCopy(alt_string,
					     (title ? title :
						  ((present &&
						    present[HTML_IMG_ISOBJECT]) ?
						    "(OBJECT)" :
						    VERBOSE_IMG(value, HTML_IMG_SRC, "[INLINE]"))));
		    }
		}
	    }

	} else if (map_href) {
	    StrAllocCopy(alt_string, (title ? title :
				      (temp = MakeNewMapValue(value,"USEMAP"))));
	    FREE(temp);

	} else if ((dest_ismap == TRUE) ||
		   (me->inA && present && present[HTML_IMG_ISMAP])) {
	    StrAllocCopy(alt_string, (title ? title :
				      (temp = MakeNewMapValue(value,"ISMAP"))));
	    FREE(temp);

	} else if (me->inA == TRUE && dest) {
	    StrAllocCopy(alt_string, (title ?
				      title :
				      VERBOSE_IMG(value, HTML_IMG_SRC, "[LINK]")));

	} else {
	    if (pseudo_inline_alts || clickable_images)
		StrAllocCopy(alt_string, (title ? title :
			  ((present &&
			    present[HTML_IMG_ISOBJECT]) ?
					     "(OBJECT)" :
					     VERBOSE_IMG(value, HTML_IMG_SRC, "[INLINE]"))));
	    else
		StrAllocCopy(alt_string, (title ?
					  title : ""));
	}
	if (*alt_string == '\0' && map_href) {
	    StrAllocCopy(alt_string, (temp = MakeNewMapValue(value,"USEMAP")));
	    FREE(temp);
	}

	CTRACE((tfp, "HTML IMG: USEMAP=%d ISMAP=%d ANCHOR=%d PARA=%d\n",
		    map_href ? 1 : 0,
		    (dest_ismap == TRUE) ? 1 : 0,
		    me->inA, me->inP));

	/*
	 *  Check for an ID attribute. - FM
	 */
	if (present && present[HTML_IMG_ID] &&
	    value[HTML_IMG_ID] && *value[HTML_IMG_ID]) {
	    StrAllocCopy(id_string, value[HTML_IMG_ID]);
	    TRANSLATE_AND_UNESCAPE_TO_STD(&id_string);
	    if (*id_string == '\0') {
		FREE(id_string);
	    }
	}

	/*
	 *  Create links to the SRC for all images, if desired. - FM
	 */
	if (clickable_images &&
	    present && present[HTML_IMG_SRC] &&
	    value[HTML_IMG_SRC] && *value[HTML_IMG_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_IMG_SRC]);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);

	    /*
	     *	Check whether a base tag is in effect. - FM
	     */
	    if ((me->inBASE && *href != '\0' && *href != '#') &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((*href != '\0' && *href != '#' &&
				 me->inBASE) ?
			       me->base_href : me->node_anchor->address));

	    /*
	     *	If it's an ISMAP and/or USEMAP, or graphic for an
	     *	anchor, end that anchor and start one for the SRC. - FM
	     */
	    if (me->inA) {
		/*
		 *  If we have a USEMAP, end this anchor and
		 *  start a new one for the client-side MAP. - FM
		 */
		if (map_href) {
		    if (dest_ismap) {
			HTML_put_character(me, ' ');
			me->in_word = NO;
			HTML_put_string(me, (temp = MakeNewMapValue(value,"ISMAP")));
			FREE(temp);
		    } else if (dest) {
			HTML_put_character(me, ' ');
			me->in_word = NO;
			HTML_put_string(me, "[LINK]");
		    }
		    if (me->inBoldA == TRUE && me->inBoldH == FALSE) {
			HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		    }
		    me->inBoldA = FALSE;
		    HText_endAnchor(me->text, me->CurrentANum);
		    me->CurrentANum = 0;
		    if (dest_ismap || dest)
			HTML_put_character(me, '-');
		    if (id_string) {
			if ((ID_A = HTAnchor_findChildAndLink(
				  me->node_anchor,	/* Parent */
				  id_string,		/* Tag */
				  NULL,			/* Addresss */
				  (HTLinkType*)0)) != NULL) {	/* Type */
			    HText_beginAnchor(me->text, me->inUnderline, ID_A);
			    HText_endAnchor(me->text, 0);
			}
		    }
		    me->CurrentA = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				NULL,			/* Tag */
				map_href,		/* Addresss */
				INTERN_LT);		/* Type */
		    if (me->CurrentA && title) {
			if ((dest = HTAnchor_parent(
				HTAnchor_followMainLink((HTAnchor*)me->CurrentA)
						  )) != NULL) {
			    if (!HTAnchor_title(dest))
				HTAnchor_setTitle(dest, title);
			}
		    }
		    me->CurrentANum = HText_beginAnchor(me->text,
							me->inUnderline,
							me->CurrentA);
		    if (me->inBoldA == FALSE && me->inBoldH == FALSE) {
			HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		    }
		    me->inBoldA = TRUE;
		} else {
		    HTML_put_character(me, ' ');/* space char may be ignored */
		    me->in_word = NO;
		}
		HTML_put_string(me, alt_string);
		if (me->inBoldA == TRUE && me->inBoldH == FALSE) {
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		}
		me->inBoldA = FALSE;
		HText_endAnchor(me->text, me->CurrentANum);
		me->CurrentANum = 0;
		HTML_put_character(me, '-');
		FREE(newtitle);
		StrAllocCopy(alt_string,
			     ((present &&
			       present[HTML_IMG_ISOBJECT]) ?
		   ((map_href || dest_ismap) ?
				   "(IMAGE)" : "(OBJECT)") :
				   VERBOSE_IMG(value, HTML_IMG_SRC, "[IMAGE]")));
		if (id_string && !map_href) {
		    if ((ID_A = HTAnchor_findChildAndLink(
				  me->node_anchor,	/* Parent */
				  id_string,		/* Tag */
				  NULL,			/* Addresss */
				  (HTLinkType*)0)) != NULL) {	/* Type */
			HText_beginAnchor(me->text, me->inUnderline, ID_A);
			HText_endAnchor(me->text, 0);
		    }
		}
	    } else if (map_href) {
		HTML_put_character(me, ' ');  /* space char may be ignored */
		me->in_word = NO;
		if (id_string) {
		    if ((ID_A = HTAnchor_findChildAndLink(
				  me->node_anchor,	/* Parent */
				  id_string,		/* Tag */
				  NULL,			/* Addresss */
				  (HTLinkType*)0)) != NULL) {	/* Type */
			HText_beginAnchor(me->text, me->inUnderline, ID_A);
			HText_endAnchor(me->text, 0);
		    }
		}
		me->CurrentA = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				NULL,			/* Tag */
				map_href,		/* Addresss */
				INTERN_LT);		/* Type */
		if (me->CurrentA && title) {
		    if ((dest = HTAnchor_parent(
				HTAnchor_followMainLink((HTAnchor*)me->CurrentA)
					      )) != NULL) {
			if (!HTAnchor_title(dest))
			    HTAnchor_setTitle(dest, title);
		    }
		}
		me->CurrentANum = HText_beginAnchor(me->text,
						    me->inUnderline,
						    me->CurrentA);
		if (me->inBoldA == FALSE && me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		me->inBoldA = TRUE;
		HTML_put_string(me, alt_string);
		if (me->inBoldA == TRUE && me->inBoldH == FALSE) {
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		}
		me->inBoldA = FALSE;
		HText_endAnchor(me->text, me->CurrentANum);
		me->CurrentANum = 0;
		HTML_put_character(me, '-');
		FREE(newtitle);
		StrAllocCopy(alt_string,
			     ((present &&
			       present[HTML_IMG_ISOBJECT]) ?
						 "(IMAGE)" :
						 VERBOSE_IMG(value, HTML_IMG_SRC, "[IMAGE]")));
	    } else {
		HTML_put_character(me, ' ');  /* space char may be ignored */
		me->in_word = NO;
		if (id_string) {
		    if ((ID_A = HTAnchor_findChildAndLink(
				  me->node_anchor,	/* Parent */
				  id_string,		/* Tag */
				  NULL,			/* Addresss */
				  (HTLinkType*)0)) != NULL) {	/* Type */
			HText_beginAnchor(me->text, me->inUnderline, ID_A);
			HText_endAnchor(me->text, 0);
		    }
		}
	    }

	    /*
	     *	Create the link to the SRC. - FM
	     */
	    me->CurrentA = HTAnchor_findChildAndLink(
			me->node_anchor,		/* Parent */
			NULL,				/* Tag */
			href,				/* Addresss */
			(HTLinkType*)0);		/* Type */
	    FREE(href);
	    me->CurrentANum = HText_beginAnchor(me->text,
						me->inUnderline,
						me->CurrentA);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
	    HTML_put_string(me, alt_string);
	    if (!me->inA) {
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, me->CurrentANum);
		me->CurrentANum = 0;
		HTML_put_character(me, ' ');  /* space char may be ignored */
		me->in_word = NO;
	    } else {
		HTML_put_character(me, ' ');  /* space char may be ignored */
		me->in_word = NO;
		me->inBoldA = TRUE;
	    }
	} else if (map_href) {
	    if (me->inA) {
		/*
		 *  We're in an anchor and have a USEMAP, so end the anchor
		 *  and start a new one for the client-side MAP. - FM
		 */
		if (dest_ismap) {
		    HTML_put_character(me, ' ');/* space char may be ignored */
		    me->in_word = NO;
		    HTML_put_string(me, (temp = MakeNewMapValue(value,"ISMAP")));
		    FREE(temp);
		} else if (dest) {
		    HTML_put_character(me, ' ');/* space char may be ignored */
		    me->in_word = NO;
		    HTML_put_string(me, "[LINK]");
		}
		if (me->inBoldA == TRUE && me->inBoldH == FALSE) {
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		}
		me->inBoldA = FALSE;
		HText_endAnchor(me->text, me->CurrentANum);
		me->CurrentANum = 0;
		if (dest_ismap || dest) {
		    HTML_put_character(me, '-');
		}
	    } else {
		HTML_put_character(me, ' ');
		me->in_word = NO;
	    }
	    me->CurrentA = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				NULL,			/* Tag */
				map_href,		/* Addresss */
				INTERN_LT);		/* Type */
	    if (me->CurrentA && title) {
		if ((dest = HTAnchor_parent(
				HTAnchor_followMainLink((HTAnchor*)me->CurrentA)
					  )) != NULL) {
		    if (!HTAnchor_title(dest))
			HTAnchor_setTitle(dest, title);
		}
	    }
	    me->CurrentANum = HText_beginAnchor(me->text,
						me->inUnderline,
						me->CurrentA);
	    if (me->inBoldA == FALSE && me->inBoldH == FALSE) {
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
	    }
	    me->inBoldA = TRUE;
	    HTML_put_string(me, alt_string);
	    if (!me->inA) {
		if (me->inBoldA == TRUE && me->inBoldH == FALSE) {
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		}
		me->inBoldA = FALSE;
		HText_endAnchor(me->text, me->CurrentANum);
		me->CurrentANum = 0;
	    }
	} else {
	    /*
	     *	Just put in the ALT or pseudo-ALT string
	     *	for the current anchor or inline, with an
	     *	ID link if indicated. - FM
	     */
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	    me->in_word = NO;
	    if (id_string) {
		if ((ID_A = HTAnchor_findChildAndLink(
				  me->node_anchor,	/* Parent */
				  id_string,		/* Tag */
				  NULL,			/* Addresss */
				  (HTLinkType*)0)) != NULL) {	/* Type */
		    HText_beginAnchor(me->text, me->inUnderline, ID_A);
		    HText_endAnchor(me->text, 0);
		}
	    }
	    HTML_put_string(me, alt_string);
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	    me->in_word = NO;
	}
	FREE(map_href);
	FREE(alt_string);
	FREE(id_string);
	FREE(title);
	FREE(newtitle);
	dest = NULL;
	dest_ismap = FALSE;
	break;

    case HTML_MAP:
	/*
	 *  Load id_string if we have a NAME or ID. - FM
	 */
	if (present && present[HTML_MAP_NAME] &&
	    value[HTML_MAP_NAME] && *value[HTML_MAP_NAME]) {
	    StrAllocCopy(id_string, value[HTML_MAP_NAME]);
	} else if (present && present[HTML_MAP_ID] &&
		   value[HTML_MAP_ID] && *value[HTML_MAP_ID]) {
	    StrAllocCopy(id_string, value[HTML_MAP_ID]);
	}
	if (id_string) {
	    TRANSLATE_AND_UNESCAPE_TO_STD(&id_string);
	    if (*id_string == '\0') {
		FREE(id_string);
	    }
	}

	/*
	 *  Generate a target anchor in this place in the containing
	 *  document.  MAP can now contain block markup, if it doesn't
	 *  contain any AREAs (or A anchors with COORDS converted to AREAs)
	 *  the current location can be used as a fallback for following
	 *  a USEMAP link. - kw
	 */
	 if (!LYMapsOnly)
	     LYHandleID(me, id_string);

	/*
	 *  Load map_address. - FM
	 */
	if (id_string) {
	    /*
	     *	The MAP must be in the current stream, even if it
	     *	had a BASE tag, so we'll use its address here, but
	     *	still use the BASE, if present, when resolving the
	     *	AREA elements in it's content, unless the AREA's
	     *	HREF is a lone fragment and LYSeekFragAREAinCur is
	     *	set. - FM && KW
	     */
	    StrAllocCopy(me->map_address, me->node_anchor->address);
	    if ((cp = strrchr(me->map_address, '#')) != NULL)
		*cp = '\0';
	    StrAllocCat(me->map_address, "#");
	    StrAllocCat(me->map_address, id_string);
	    FREE(id_string);
	    if (present && present[HTML_MAP_TITLE] &&
		value[HTML_MAP_TITLE] && *value[HTML_MAP_TITLE] != '\0') {
		StrAllocCopy(title, value[HTML_MAP_TITLE]);
		TRANSLATE_AND_UNESCAPE_ENTITIES(&title, TRUE, FALSE);
		LYTrimHead(title);
		LYTrimTail(title);
		if (*title == '\0') {
		    FREE(title);
		}
	    }
	    LYAddImageMap(me->map_address, title, me->node_anchor);
	    FREE(title);
	}
	break;

    case HTML_AREA:
	if (me->map_address &&
	    present && present[HTML_AREA_HREF] &&
	    value[HTML_AREA_HREF] && *value[HTML_AREA_HREF]) {
	    /*
	     *	Resolve the HREF. - FM
	     */
	    StrAllocCopy(href, value[HTML_AREA_HREF]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);

	    /*
	     *	Check whether a BASE tag is in effect, and use it
	     *	for resolving, even though we used this stream's
	     *	address for locating the MAP itself, unless the
	     *	HREF is a lone fragment and LYSeekFragAREAinCur
	     *	is set. - FM
	     */
	    if (((me->inBASE && *href != '\0') &&
		 !(*href == '#' && LYSeekFragAREAinCur == TRUE)) &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((((me->inBASE && *href != '\0') &&
				  !(*href == '#' &&
				    LYSeekFragAREAinCur == TRUE)))
						?
				  me->base_href : me->node_anchor->address));
	    if (!(url_type = is_url(href))) {
		temp = HTParse(href, me->node_anchor->address, PARSE_ALL);
		if (!(temp && *temp)) {
		   FREE(href);
		   FREE(temp);
		   break;
		}
		StrAllocCopy(href, temp);
		FREE(temp);
	    }

	    /*
	     *	Check for an ALT. - FM
	     */
	    if (present[HTML_AREA_ALT] &&
		value[HTML_AREA_ALT] && *value[HTML_AREA_ALT]) {
		StrAllocCopy(alt_string, value[HTML_AREA_ALT]);
	    } else if (present[HTML_AREA_TITLE] &&
		value[HTML_AREA_TITLE] && *value[HTML_AREA_TITLE]) {
		/*
		 *  Use the TITLE as an ALT. - FM
		 */
		StrAllocCopy(alt_string, value[HTML_AREA_TITLE]);
	    }
	    if (alt_string != NULL) {
		TRANSLATE_AND_UNESCAPE_ENTITIES(&alt_string,
						me->UsePlainSpace,
						me->HiddenValue);
		/*
		 *  Make sure it's not just space(s). - FM
		 */
		LYTrimHead(alt_string);
		LYTrimTail(alt_string);
		if (*alt_string == '\0') {
		    StrAllocCopy(alt_string, href);
		}
	    } else {
		/*
		 *  Use the HREF as an ALT. - FM
		 */
		StrAllocCopy(alt_string, href);
	    }

	    LYAddMapElement(me->map_address, href, alt_string,
			    me->node_anchor, intern_flag);
	    FREE(href);
	    FREE(alt_string);
	}
	break;

    case HTML_PARAM:
	/*
	 *  We may need to look at this someday to deal with
	 *  MAPs, OBJECTs or APPLETs optimally, but just ignore
	 *  it for now. - FM
	 */
	break;

    case HTML_BODYTEXT:
	CHECK_ID(HTML_BODYTEXT_ID);
	/*
	 *  We may need to look at this someday to deal with
	 *  OBJECTs optimally, but just ignore it for now. - FM
	 */
	break;

    case HTML_TEXTFLOW:
	CHECK_ID(HTML_BODYTEXT_ID);
	/*
	 *  We may need to look at this someday to deal with
	 *  APPLETs optimally, but just ignore it for now. - FM
	 */
	break;

    case HTML_FIG:
	if (present)
	    LYHandleFIG(me, present, value,
			present[HTML_FIG_ISOBJECT],
			present[HTML_FIG_IMAGEMAP],
			present[HTML_FIG_ID] ? value[HTML_FIG_ID] : NULL,
			present[HTML_FIG_SRC] ? value[HTML_FIG_SRC] : NULL,
			YES, TRUE, &intern_flag);
	else
	    LYHandleFIG(me, NULL, NULL,
			0,
			0,
			NULL,
			NULL, YES, TRUE, &intern_flag);
#if 0
	me->inFIG = TRUE;
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	if (!present ||
	    (present && !present[HTML_FIG_ISOBJECT])) {
	    LYEnsureDoubleSpace(me);
	    LYResetParagraphAlignment(me);
	    me->inFIGwithP = TRUE;
	} else {
	    me->inFIGwithP = FALSE;
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	}
	CHECK_ID(HTML_FIG_ID);
	me->in_word = NO;
	me->inP = FALSE;

	if (clickable_images && present && present[HTML_FIG_SRC] &&
	    value[HTML_FIG_SRC] && *value[HTML_FIG_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_FIG_SRC]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    if (*href) {
		/*
		 *  Check whether a base tag is in effect. - FM
		 */
		if ((me->inBASE && *href != '#') &&
		    (temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		    *temp != '\0')
		    /*
		     *	Use reference related to the base.
		     */
		    StrAllocCopy(href, temp);
		FREE(temp);

		/*
		 *  Check whether to fill in localhost. - FM
		 */
		LYFillLocalFileURL(&href,
				   ((*href != '#' &&
				     me->inBASE) ?
				   me->base_href : me->node_anchor->address));

		me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					INTERN_LT);		/* Type */
		HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		HTML_put_string(me, (present[HTML_FIG_ISOBJECT] ?
		      (present[HTML_FIG_IMAGEMAP] ?
					"(IMAGE)" : "(OBJECT)") : "[FIGURE]"));
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, 0);
		HTML_put_character(me, '-');
		HTML_put_character(me, ' '); /* space char may be ignored */
		me->in_word = NO;
	    }
	    FREE(href);
	}
#endif
	break;

    case HTML_OBJECT:
	if (!me->object_started) {
	    /*
	     *	This is an outer OBJECT start tag,
	     *	i.e., not a nested OBJECT, so save
	     *	it's relevant attributes. - FM
	     */
	    if (present) {
		if (present[HTML_OBJECT_DECLARE])
		    me->object_declare = TRUE;
		if (present[HTML_OBJECT_SHAPES])
		    me->object_shapes = TRUE;
		if (present[HTML_OBJECT_ISMAP])
		    me->object_ismap = TRUE;
		if (present[HTML_OBJECT_USEMAP] &&
		    value[HTML_OBJECT_USEMAP] && *value[HTML_OBJECT_USEMAP]) {
		    StrAllocCopy(me->object_usemap, value[HTML_OBJECT_USEMAP]);
		    TRANSLATE_AND_UNESCAPE_TO_STD(&me->object_usemap);
		    if (*me->object_usemap == '\0') {
			FREE(me->object_usemap);
		    }
		}
		if (present[HTML_OBJECT_ID] &&
		    value[HTML_OBJECT_ID] && *value[HTML_OBJECT_ID]) {
		    StrAllocCopy(me->object_id, value[HTML_OBJECT_ID]);
		    TRANSLATE_AND_UNESCAPE_TO_STD(&me->object_id);
		    if (*me->object_id == '\0') {
			FREE(me->object_id);
		    }
		}
		if (present[HTML_OBJECT_TITLE] &&
		    value[HTML_OBJECT_TITLE] && *value[HTML_OBJECT_TITLE]) {
		    StrAllocCopy(me->object_title, value[HTML_OBJECT_TITLE]);
		    TRANSLATE_AND_UNESCAPE_ENTITIES(&me->object_title, TRUE, FALSE);
		    LYTrimHead(me->object_title);
		    LYTrimTail(me->object_title);
		    if (me->object_title == '\0') {
			FREE(me->object_title);
		    }
		}
		if (present[HTML_OBJECT_DATA] &&
		    value[HTML_OBJECT_DATA] && *value[HTML_OBJECT_DATA]) {
		    StrAllocCopy(me->object_data, value[HTML_OBJECT_DATA]);
		    TRANSLATE_AND_UNESCAPE_TO_STD(&me->object_data);
		    if (*me->object_data == '\0') {
			FREE(me->object_data);
		    }
		}
		if (present[HTML_OBJECT_TYPE] &&
		    value[HTML_OBJECT_TYPE] && *value[HTML_OBJECT_TYPE]) {
		    StrAllocCopy(me->object_type, value[HTML_OBJECT_TYPE]);
		    TRANSLATE_AND_UNESCAPE_ENTITIES(&me->object_type, TRUE, FALSE);
		    LYTrimHead(me->object_type);
		    LYTrimTail(me->object_type);
		    if (me->object_type == '\0') {
			FREE(me->object_type);
		    }
		}
		if (present[HTML_OBJECT_CLASSID] &&
		    value[HTML_OBJECT_CLASSID] &&
		    *value[HTML_OBJECT_CLASSID]) {
		    StrAllocCopy(me->object_classid,
				 value[HTML_OBJECT_CLASSID]);
		    TRANSLATE_AND_UNESCAPE_ENTITIES(&me->object_classid, TRUE, FALSE);
		    LYTrimHead(me->object_classid);
		    LYTrimTail(me->object_classid);
		    if (me->object_classid == '\0') {
			FREE(me->object_classid);
		    }
		}
		if (present[HTML_OBJECT_CODEBASE] &&
		    value[HTML_OBJECT_CODEBASE] &&
		    *value[HTML_OBJECT_CODEBASE]) {
		    StrAllocCopy(me->object_codebase,
				 value[HTML_OBJECT_CODEBASE]);
		    TRANSLATE_AND_UNESCAPE_TO_STD(&me->object_codebase);
		    if (*me->object_codebase == '\0') {
			FREE(me->object_codebase);
		    }
		}
		if (present[HTML_OBJECT_CODETYPE] &&
		    value[HTML_OBJECT_CODETYPE] &&
		    *value[HTML_OBJECT_CODETYPE]) {
		    StrAllocCopy(me->object_codetype,
				 value[HTML_OBJECT_CODETYPE]);
		    TRANSLATE_AND_UNESCAPE_ENTITIES(&me->object_codetype, TRUE, FALSE);
		    LYTrimHead(me->object_codetype);
		    LYTrimTail(me->object_codetype);
		    if (me->object_codetype == '\0') {
			FREE(me->object_codetype);
		    }
		}
		if (present[HTML_OBJECT_NAME] &&
		    value[HTML_OBJECT_NAME] && *value[HTML_OBJECT_NAME]) {
		    StrAllocCopy(me->object_name, value[HTML_OBJECT_NAME]);
		    TRANSLATE_AND_UNESCAPE_ENTITIES(&me->object_name, TRUE, FALSE);
		    LYTrimHead(me->object_name);
		    LYTrimTail(me->object_name);
		    if (me->object_name == '\0') {
			FREE(me->object_name);
		    }
		}
	    }
	    /*
	     *  If we can determine now that we are not going to do anything
	     *  special to the OBJECT element's SGML contents, like skipping
	     *  it completely or collecting it up in order to add something
	     *  after it, then generate any output that should be emitted in the
	     *  place of the OBJECT start tag NOW, then don't initialize special
	     *  handling but return, letting our SGML parser know that further
	     *  content is to be parsed normally not literally.  We could defer
	     *  this until we have collected the contents and then recycle the
	     *  contents (as was previously always done), but that has a higher
	     *  chance of completely losing content in case of nesting errors
	     *  in the input, incomplete transmissions, etc. - kw
	     */
	    if ((!present ||
		 (me->object_declare == FALSE && me->object_name == NULL &&
		  me->object_shapes == FALSE && me->object_usemap == NULL))) {
		if (!LYMapsOnly) {
		    if (!clickable_images || me->object_data == NULL ||
			!(me->object_data != NULL &&
			  me->object_classid == NULL &&
			  me->object_codebase == NULL &&
			  me->object_codetype == NULL))
			FREE(me->object_data);
		    if (me->object_data) {
			HTStartAnchor5(me,
				       me->object_id ? value[HTML_OBJECT_ID]
					: NULL,
				       value[HTML_OBJECT_DATA],
				       value[HTML_OBJECT_TYPE],
				       tag_charset);
			if ((me->object_type != NULL) &&
			    !strncasecomp(me->object_type, "image/", 6))
			    HTML_put_string(me, "(IMAGE)");
			else
			    HTML_put_string(me, "(OBJECT)");
			HTML_end_element(me,HTML_A,NULL);
		    } else if (me->object_id)
			LYHandleID(me, me->object_id);
		}
		clear_objectdata(me);
		/*
		 *  We do NOT want the HTML_put_* functions that are going
		 *  to be called for the OBJECT's character content to
		 *  add to the chunk, so we don't push on the stack.
		 *  Instead we keep a counter for open OBJECT tags that
		 *  are treated this way, so HTML_end_element can skip
		 *  handling the corresponding end tag that is going to
		 *  arrive unexpectedly as far as our stack is concerned.
		 */
		status = HT_PARSER_OTHER_CONTENT;
		if (me->sp[0].tag_number == HTML_FIG &&
		    me->objects_figged_open > 0) {
		    ElementNumber = HTML_OBJECT_M;
		} else {
		    me->objects_mixed_open++;
		    SET_SKIP_STACK(HTML_OBJECT);
		}
	    } else if (me->object_declare == FALSE && me->object_name == NULL &&
		       me->object_shapes == TRUE) {
		LYHandleFIG(me, present, value,
		    1,
		    1 || me->object_ismap,
		    me->object_id,
		    (me->object_data && !me->object_classid) ? value[HTML_OBJECT_DATA] : NULL,
		    NO, TRUE, &intern_flag);
		clear_objectdata(me);
		status = HT_PARSER_OTHER_CONTENT;
		me->objects_figged_open++;
		ElementNumber = HTML_FIG;

	    } else {
		/*
		 *	Set flag that we are accumulating OBJECT content. - FM
		 */
		me->object_started = TRUE;
	    }
	}
	break;

    case HTML_OVERLAY:
	if (clickable_images && me->inFIG &&
	    present && present[HTML_OVERLAY_SRC] &&
	    value[HTML_OVERLAY_SRC] && *value[HTML_OVERLAY_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_OVERLAY_SRC]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    if (*href) {
		/*
		 *  Check whether a base tag is in effect. - FM
		 */
		if ((me->inBASE && *href != '#') &&
		    (temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		    *temp != '\0')
		    /*
		     *	Use reference related to the base.
		     */
		    StrAllocCopy(href, temp);
		FREE(temp);

		/*
		 *  Check whether to fill in localhost. - FM
		 */
		LYFillLocalFileURL(&href,
				   ((*href != '#' &&
				     me->inBASE) ?
				   me->base_href : me->node_anchor->address));

		if (me->inA) {
		    SET_SKIP_STACK(HTML_A);
		    HTML_end_element(me, HTML_A, include);
		}
		me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					INTERN_LT);		/* Type */
		HTML_put_character(me, ' ');
		HText_appendCharacter(me->text, '+');
		me->CurrentANum = HText_beginAnchor(me->text,
						    me->inUnderline,
						    me->CurrentA);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		HTML_put_string(me, "[OVERLAY]");
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, me->CurrentANum);
		HTML_put_character(me, ' ');
		me->in_word = NO;
	    }
	    FREE(href);
	}
	break;

    case HTML_APPLET:
	me->inAPPLET = TRUE;
	me->inAPPLETwithP = FALSE;
	HTML_put_character(me, ' ');  /* space char may be ignored */
	/*
	 *  Load id_string if we have an ID or NAME. - FM
	 */
	if (present && present[HTML_APPLET_ID] &&
	    value[HTML_APPLET_ID] && *value[HTML_APPLET_ID]) {
	    StrAllocCopy(id_string, value[HTML_APPLET_ID]);
	} else if (present && present[HTML_APPLET_NAME] &&
		   value[HTML_APPLET_NAME] && *value[HTML_APPLET_NAME]) {
	    StrAllocCopy(id_string, value[HTML_APPLET_NAME]);
	}
	if (id_string) {
	    TRANSLATE_AND_UNESCAPE_TO_STD(&id_string);
	    LYHandleID(me, id_string);
	    FREE(id_string);
	}
	me->in_word = NO;

	/*
	 *  If there's an ALT string, use it, unless the ALT string
	 *  is zero-length and we are making all sources links. - FM
	 */
	if (present && present[HTML_APPLET_ALT] && value[HTML_APPLET_ALT] &&
	    (!clickable_images ||
	     (clickable_images && *value[HTML_APPLET_ALT] != '\0'))) {
	    StrAllocCopy(alt_string, value[HTML_APPLET_ALT]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&alt_string,
						   me->UsePlainSpace, me->HiddenValue);
	    /*
	     *	If it's all spaces and we are making sources links,
	     *	treat it as zero-length. - FM
	     */
	    if (clickable_images) {
		LYTrimHead(alt_string);
		LYTrimTail(alt_string);
		if (*alt_string == '\0') {
		    StrAllocCopy(alt_string, "[APPLET]");
		}
	    }

	} else {
	    if (clickable_images)
		StrAllocCopy(alt_string, "[APPLET]");
	    else
		StrAllocCopy(alt_string, "");
	}

	/*
	 *  If we're making all sources links, get the source. - FM
	 */
	if (clickable_images && present && present[HTML_APPLET_CODE] &&
	    value[HTML_APPLET_CODE] && *value[HTML_APPLET_CODE] != '\0') {
	    char * base = NULL;
	    char * code = NULL;

	    /*
	     *	Check for a CODEBASE attribute. - FM
	     */
	    if (present[HTML_APPLET_CODEBASE] &&
		value[HTML_APPLET_CODEBASE] && *value[HTML_APPLET_CODEBASE]) {
		StrAllocCopy(base, value[HTML_APPLET_CODEBASE]);
		LYRemoveBlanks(base);
		TRANSLATE_AND_UNESCAPE_TO_STD(&base);
		/*
		 *  Force it to be a directory. - FM
		 */
		if (*base == '\0')
		    StrAllocCopy(base, "/");
		LYAddHtmlSep(&base);
		url_type = LYLegitimizeHREF(me, &base, TRUE, FALSE);

		/*
		 *  Check whether to fill in localhost. - FM
		 */
		LYFillLocalFileURL(&base,
				   (me->inBASE ?
				 me->base_href : me->node_anchor->address));

		if (!(url_type = is_url(base))) {
		    /*
		     *	Check whether a base tag is in effect.
		     */
		    if (me->inBASE) {
			temp = HTParse(base, me->base_href, PARSE_ALL);
		    } else {
			temp = HTParse(base, me->node_anchor->address,
							PARSE_ALL);
		    }
		    StrAllocCopy(base, temp);
		    FREE(temp);
		}
	    } else {
		if (me->inBASE) {
		    StrAllocCopy(base, me->base_href);
		} else {
		    StrAllocCopy(base, me->node_anchor->address);
		}
	    }

	    StrAllocCopy(code, value[HTML_APPLET_CODE]);
	    url_type = LYLegitimizeHREF(me, &code, TRUE, FALSE);
	    href = HTParse(code, base, PARSE_ALL);
	    FREE(base);
	    FREE(code);

	    if (href && *href) {
		if (me->inA) {
		    if (me->inBoldA == TRUE && me->inBoldH == FALSE)
			HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		    HText_endAnchor(me->text, me->CurrentANum);
		    HTML_put_character(me, '-');
		}
		me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					(HTLinkType*)0);	/* Type */
		me->CurrentANum = HText_beginAnchor(me->text,
						    me->inUnderline,
						    me->CurrentA);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		HTML_put_string(me, alt_string);
		if (me->inA == FALSE) {
		    if (me->inBoldH == FALSE)
			HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		    HText_endAnchor(me->text, me->CurrentANum);
		    me->CurrentANum = 0;
		}
		HTML_put_character(me, ' ');  /* space char may be ignored */
		me->in_word = NO;
	    }
	    FREE(href);
	} else if (*alt_string) {
	    /*
	     *	Just put up the ALT string, if non-zero. - FM
	     */
	    HTML_put_string(me, alt_string);
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	    me->in_word = NO;
	}
	FREE(alt_string);
	FREE(id_string);
	break;

    case HTML_BGSOUND:
	/*
	 *  If we're making all sources links, get the source. - FM
	 */
	if (clickable_images && present && present[HTML_BGSOUND_SRC] &&
	    value[HTML_BGSOUND_SRC] && *value[HTML_BGSOUND_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_BGSOUND_SRC]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    if (*href == '\0') {
		FREE(href);
		break;
	    }

	    /*
	     *	Check whether a base tag is in effect. - FM
	     */
	    if ((me->inBASE && *href != '#') &&
		(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		*temp != '\0')
		/*
		 *  Use reference related to the base.
		 */
		StrAllocCopy(href, temp);
	    FREE(temp);

	    /*
	     *	Check whether to fill in localhost. - FM
	     */
	    LYFillLocalFileURL(&href,
			       ((*href != '#' &&
				 me->inBASE) ?
			       me->base_href : me->node_anchor->address));

	    if (me->inA) {
		if (me->inBoldA == TRUE && me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, me->CurrentANum);
		HTML_put_character(me, '-');
	    } else {
		HTML_put_character(me, ' ');  /* space char may be ignored */
		me->in_word = NO;
	    }
	    me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					INTERN_LT);		/* Type */
	    me->CurrentANum = HText_beginAnchor(me->text,
						me->inUnderline,
						me->CurrentA);
	    if (me->inBoldH == FALSE)
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
	    HTML_put_string(me, "[BGSOUND]");
	    if (me->inA == FALSE) {
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, me->CurrentANum);
		me->CurrentANum = 0;
	    }
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	    me->in_word = NO;
	    FREE(href);
	}
	break;

    case HTML_EMBED:
	if (pseudo_inline_alts || clickable_images)
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	/*
	 *  Load id_string if we have an ID or NAME. - FM
	 */
	if (present && present[HTML_EMBED_ID] &&
	    value[HTML_EMBED_ID] && *value[HTML_EMBED_ID]) {
	    StrAllocCopy(id_string, value[HTML_EMBED_ID]);
	} else if (present && present[HTML_EMBED_NAME] &&
		   value[HTML_EMBED_NAME] && *value[HTML_EMBED_NAME]) {
	    StrAllocCopy(id_string, value[HTML_EMBED_NAME]);
	}
	if (id_string) {
	    TRANSLATE_AND_UNESCAPE_TO_STD(&id_string);
	    LYHandleID(me, id_string);
	    FREE(id_string);
	}
	if (pseudo_inline_alts || clickable_images)
	    me->in_word = NO;

	/*
	 *  If there's an ALT string, use it, unless the ALT string
	 *  is zero-length and we are making all sources links. - FM
	 */
	if (present && present[HTML_EMBED_ALT] && value[HTML_EMBED_ALT] &&
	    (!clickable_images ||
	     (clickable_images && *value[HTML_EMBED_ALT] != '\0'))) {
	    StrAllocCopy(alt_string, value[HTML_EMBED_ALT]);
	    TRANSLATE_AND_UNESCAPE_ENTITIES(&alt_string,
						   me->UsePlainSpace, me->HiddenValue);
	    /*
	     *	If it's all spaces and we are making sources links,
	     *	treat it as zero-length. - FM
	     */
	    if (clickable_images) {
		LYTrimHead(alt_string);
		LYTrimTail(alt_string);
		if (*alt_string == '\0') {
		    StrAllocCopy(alt_string, "[EMBED]");
		}
	    }
	} else {
	    if (pseudo_inline_alts || clickable_images)
		StrAllocCopy(alt_string, "[EMBED]");
	    else
		StrAllocCopy(alt_string, "");
	}

	/*
	 *  If we're making all sources links, get the source. - FM
	 */
	if (clickable_images && present && present[HTML_EMBED_SRC] &&
	    value[HTML_EMBED_SRC] && *value[HTML_EMBED_SRC] != '\0') {
	    StrAllocCopy(href, value[HTML_EMBED_SRC]);
	    CHECK_FOR_INTERN(intern_flag,href);
	    url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    if (*href != '\0') {
		/*
		 *  Check whether a base tag is in effect. - FM
		 */
		if ((me->inBASE && *href != '#') &&
		    (temp = HTParse(href, me->base_href, PARSE_ALL)) &&
		    *temp != '\0')
		    /*
		     *	Use reference related to the base.
		     */
		    StrAllocCopy(href, temp);
		FREE(temp);

		/*
		 *  Check whether to fill in localhost. - FM
		 */
		LYFillLocalFileURL(&href,
				   ((*href != '#' &&
				     me->inBASE) ?
				   me->base_href : me->node_anchor->address));

		if (me->inA) {
		    if (me->inBoldA == TRUE && me->inBoldH == FALSE)
			HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		    HText_endAnchor(me->text, me->CurrentANum);
		    HTML_put_character(me, '-');
		}
		me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					INTERN_LT);		/* Type */
		me->CurrentANum = HText_beginAnchor(me->text,
						    me->inUnderline,
						    me->CurrentA);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		HTML_put_string(me, alt_string);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		if (me->inA == FALSE) {
		    if (me->inBoldH == FALSE)
			HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		    HText_endAnchor(me->text, me->CurrentANum);
		    me->CurrentANum = 0;
		}
		HTML_put_character(me, ' ');
		me->in_word = NO;
	    }
	    FREE(href);
	} else if (*alt_string) {
	    /*
	     *	Just put up the ALT string, if non-zero. - FM
	     */
	    HTML_put_string(me, alt_string);
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	    me->in_word = NO;
	}
	FREE(alt_string);
	FREE(id_string);
	break;

    case HTML_CREDIT:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	me->inCREDIT = TRUE;
	CHECK_ID(HTML_CREDIT_ID);
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, "CREDIT:");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	CAN_JUSTIFY_START;

	if (me->inFIG)
	    /*
	     *	Assume all text in the FIG container is intended
	     *	to be paragraphed. - FM
	     */
	    me->inFIGwithP = TRUE;

	if (me->inAPPLET)
	    /*
	     *	Assume all text in the APPLET container is intended
	     *	to be paragraphed. - FM
	     */
	    me->inAPPLETwithP = TRUE;

	me->inLABEL = TRUE;
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_CAPTION:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	me->inCAPTION = TRUE;
	CHECK_ID(HTML_CAPTION_ID);
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, "CAPTION:");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	CAN_JUSTIFY_START;

	if (me->inFIG)
	    /*
	     *	Assume all text in the FIG container is intended
	     *	to be paragraphed. - FM
	     */
	    me->inFIGwithP = TRUE;

	if (me->inAPPLET)
	    /*
	     *	Assume all text in the APPLET container is intended
	     *	to be paragraphed. - FM
	     */
	    me->inAPPLETwithP = TRUE;

	me->inLABEL = TRUE;
	me->in_word = NO;
	me->inP = FALSE;
	break;

    case HTML_FORM:
	{
	    char * action = NULL;
	    char * method = NULL;
	    char * enctype = NULL;
	    CONST char * accept_cs = NULL;

	    HTChildAnchor * source;
	    HTAnchor *link_dest;

	    /*
	     *	FORM may have been declared SGML_EMPTY in HTMLDTD.c, and
	     *	SGML_character() in SGML.c may check for a FORM end
	     *	tag to call HTML_end_element() directly (with a
	     *	check in that to bypass decrementing of the HTML
	     *	parser's stack), so if we have an open FORM, close
	     *	that one now. - FM
	     */
	    if (me->inFORM) {
		CTRACE((tfp, "HTML: Missing FORM end tag.  Faking it!\n"));
		SET_SKIP_STACK(HTML_FORM);
		HTML_end_element(me, HTML_FORM, include);
	    }

	    /*
	     *	Set to know we are in a new form.
	     */
	    me->inFORM = TRUE;
	    EMIT_IFDEF_EXP_JUSTIFY_ELTS(form_in_htext = TRUE);

	    if (present && present[HTML_FORM_ACCEPT_CHARSET]) {
		accept_cs = value[HTML_FORM_ACCEPT_CHARSET] ?
			    value[HTML_FORM_ACCEPT_CHARSET] : "UNKNOWN";
	    }
	    if (present && present[HTML_FORM_ACTION] &&
		value[HTML_FORM_ACTION])  {
		/*
		 *  Prepare to do housekeeping on the reference. - FM
		 */
		StrAllocCopy(action, value[HTML_FORM_ACTION]);
		url_type = LYLegitimizeHREF(me, &action, TRUE, TRUE);

		/*
		 *  Check whether a base tag is in effect.  Note that
		 *  actions always are resolved w.r.t. to the base,
		 *  even if the action is empty. - FM
		 */
		if ((me->inBASE && me->base_href && *me->base_href) &&
		    (temp = HTParse(action, me->base_href, PARSE_ALL)) &&
		    *temp != '\0') {
		    /*
		     *	Use action related to the base.
		     */
		    StrAllocCopy(action, temp);
		} else if ((temp = HTParse(action,
					   me->node_anchor->address,
					   PARSE_ALL)) &&
		    *temp != '\0') {
		    /*
		     *	Use action related to the current document.
		     */
		    StrAllocCopy(action, temp);
		} else {
		    FREE(action);
		}
		FREE(temp);
	    }
	    if (!(action && *action)) {
		if (me->inBASE && me->base_href && *me->base_href) {
		     StrAllocCopy(action, me->base_href);
		} else {
		     StrAllocCopy(action, me->node_anchor->address);
		}
	    }
	    if (action) {
		source = HTAnchor_findChildAndLink(me->node_anchor,
						   NULL,
						   action,
						   (HTLinkType*)0);
		if ((link_dest = HTAnchor_followMainLink((HTAnchor *)source)) != NULL) {
		    /*
		     *	Memory leak fixed.
		     *	05-28-94 Lynx 2-3-1 Garrett Arch Blythe
		     */
		    auto char *cp_freeme = HTAnchor_address(link_dest);
		    if (cp_freeme != NULL) {
			StrAllocCopy(action, cp_freeme);
			FREE(cp_freeme);
		    } else {
			StrAllocCopy(action, "");
		    }
		}
	    }

	    if (present && present[HTML_FORM_METHOD])
		StrAllocCopy(method, value[HTML_FORM_METHOD] ?
				     value[HTML_FORM_METHOD] : "GET");

	    if (present && present[HTML_FORM_ENCTYPE] &&
		value[HTML_FORM_ENCTYPE] && *value[HTML_FORM_ENCTYPE]) {
		StrAllocCopy(enctype, value[HTML_FORM_ENCTYPE]);
		LYLowerCase(enctype);
	    }

	    if (present) {
		/*
		 *  Check for a TITLE attribute, and if none is present,
		 *  check for a SUBJECT attribute as a synonym. - FM
		 */
		if (present[HTML_FORM_TITLE] &&
		    value[HTML_FORM_TITLE] &&
		    *value[HTML_FORM_TITLE] != '\0') {
		    StrAllocCopy(title, value[HTML_FORM_TITLE]);
		} else if (present[HTML_FORM_SUBJECT] &&
			   value[HTML_FORM_SUBJECT] &&
			   *value[HTML_FORM_SUBJECT] != '\0') {
		    StrAllocCopy(title, value[HTML_FORM_SUBJECT]);
		}
		if (title != NULL && *title != '\0') {
		    TRANSLATE_AND_UNESCAPE_ENTITIES(&title, TRUE, FALSE);
		    LYTrimHead(title);
		    LYTrimTail(title);
		    if (*title == '\0') {
			FREE(title);
		    }
		}
	    }

	    HText_beginForm(action, method, enctype, title, accept_cs);

	    FREE(action);
	    FREE(method);
	    FREE(enctype);
	    FREE(title);
	}
	CHECK_ID(HTML_FORM_ID);
	break;

    case HTML_FIELDSET:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	CHECK_ID(HTML_FIELDSET_ID);
	break;

    case HTML_LEGEND:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	CHECK_ID(HTML_LEGEND_ID);
	break;

    case HTML_LABEL:
	CHECK_ID(HTML_LABEL_ID);
	break;

    case HTML_KEYGEN:
	CHECK_ID(HTML_KEYGEN_ID);
	break;

    case HTML_BUTTON:
	{
	    InputFieldData I;
	    int chars;

	    /* init */
	    I.align=NULL; I.accept=NULL; I.checked=NO; I.class=NULL;
	    I.disabled=NO; I.error=NULL; I.height= NULL; I.id=NULL;
	    I.lang=NULL; I.max=NULL; I.maxlength=NULL; I.md=NULL;
	    I.min=NULL; I.name=NULL; I.size=NULL; I.src=NULL;
	    I.type=NULL; I.value=NULL; I.width=NULL;
	    I.accept_cs = NULL;
	    I.name_cs = ATTR_CS_IN;
	    I.value_cs = ATTR_CS_IN;

	    UPDATE_STYLE;
	    if ((present && present[HTML_BUTTON_TYPE] &&
		 value[HTML_BUTTON_TYPE]) &&
		(!strcasecomp(value[HTML_BUTTON_TYPE], "submit") ||
		 !strcasecomp(value[HTML_BUTTON_TYPE], "reset"))) {
		/*
		 *  It's a button for submitting or resetting a form. - FM
		 */
		I.type = value[HTML_BUTTON_TYPE];
	    } else {
		/*
		 *  Ugh, it's a button for a script. - FM
		 */
		HTML_put_string(me," [BUTTON] ");
		break;
	    }

	    /*
	     *	Make sure we're in a form.
	     */
	    if (!me->inFORM) {
		if (LYBadHTML(me))
		    CTRACE((tfp,
			    "Bad HTML: BUTTON tag not within FORM tag\n"));
		/*
		 *  We'll process it, since the chances of a crash are
		 *  small, and we probably do have a form started. - FM
		 *
		break;
		 */
	    }

	    /*
	     *	Before any input field, add a collapsible space if
	     *	we're not in a PRE block, to promote a wrap there
	     *	for any long values that would extend past the right
	     *	margin from our current position in the line.  If
	     *	we are in a PRE block, start a new line if the last
	     *	line already is within 6 characters of the wrap point
	     *	for PRE blocks. - FM
	     */
	    if (me->sp[0].tag_number != HTML_PRE && !me->inPRE &&
		me->sp->style->freeFormat) {
		HTML_put_character(me, ' ');
		me->in_word = NO;
	    } else if (HText_LastLineSize(me->text, FALSE) > (LYcols - 7)) {
		HTML_put_character(me, '\n');
		me->in_word = NO;
	    }
	    HTML_put_character(me, '(');

	    if (!(present && present[HTML_BUTTON_NAME] &&
		  value[HTML_BUTTON_NAME])) {
		I.name = "";
	    } else if (strchr(value[HTML_BUTTON_NAME], '&') == NULL) {
		I.name = value[HTML_BUTTON_NAME];
	    } else {
		StrAllocCopy(I_name, value[HTML_BUTTON_NAME]);
		UNESCAPE_FIELDNAME_TO_STD(&I_name);
		I.name = I_name;
	    }

	    if (present && present[HTML_BUTTON_VALUE] &&
		value[HTML_BUTTON_VALUE] && *value[HTML_BUTTON_VALUE]) {
		/*
		 *  Convert any HTML entities or decimal escaping. - FM
		 */
		int len;

		StrAllocCopy(I_value, value[HTML_BUTTON_VALUE]);
		me->UsePlainSpace = TRUE;
		TRANSLATE_AND_UNESCAPE_ENTITIES(&I_value, TRUE, me->HiddenValue);
		me->UsePlainSpace = FALSE;
		I.value = I_value;
		/*
		 *  Convert any newlines or tabs to spaces,
		 *  and trim any lead or trailing spaces. - FM
		 */
		convert_to_spaces(I.value, FALSE);
		while (I.value && I.value[0] == ' ')
		    I.value++;
		len = strlen(I.value) - 1;
		while (len > 0 && I.value[len] == ' ')
		    I.value[len--] = '\0';
	    }

	    if (present && present[HTML_BUTTON_DISABLED])
		I.disabled = YES;

	    if (present && present[HTML_BUTTON_CLASS] && /* Not yet used. */
		value[HTML_BUTTON_CLASS] && *value[HTML_BUTTON_CLASS])
		I.class = value[HTML_BUTTON_CLASS];

	    if (present && present[HTML_BUTTON_ID] &&
		value[HTML_BUTTON_ID] && *value[HTML_BUTTON_ID]) {
		I.id = value[HTML_BUTTON_ID];
		CHECK_ID(HTML_BUTTON_ID);
	    }

	    if (present && present[HTML_BUTTON_LANG] && /* Not yet used. */
		value[HTML_BUTTON_LANG] && *value[HTML_BUTTON_LANG])
		I.lang = value[HTML_BUTTON_LANG];

	    chars = HText_beginInput(me->text, me->inUnderline, &I);
	    /*
	     *	Submit and reset buttons have values which don't change,
	     *	so HText_beginInput() sets I.value to the string which
	     *	should be displayed, and we'll enter that instead of
	     *	underscore placeholders into the HText structure to
	     *	see it instead of underscores when dumping or printing.
	     *	We also won't worry about a wrap in PRE blocks, because
	     *	the line editor never is invoked for submit or reset
	     *	buttons. - LE & FM
	     */
	    if (me->sp[0].tag_number == HTML_PRE ||
		    !me->sp->style->freeFormat) {
		/*
		 *  We have a submit or reset button in a PRE block,
		 *  so output the entire value from the markup.  If
		 *  it extends to the right margin, it will wrap
		 *  there, and only the portion before that wrap will
		 *  be hightlighted on screen display (Yuk!) but we
		 *  may as well show the rest of the full value on
		 *  the next or more lines. - FM
		 */
		while (I.value[i])
		    HTML_put_character(me, I.value[i++]);
	    } else {
		/*
		 *  The submit or reset button is not in a PRE block.
		 *  Note that if a wrap occurs before outputting the
		 *  entire value, the wrapped portion will not be
		 *  highlighted or clearly indicated as part of the
		 *  link for submission or reset (Yuk!).
		 *  We'll replace any spaces in the submit or reset
		 *  button value with nbsp, to promote a wrap at the
		 *  space we ensured would be present before the start
		 *  of the string, as when we use all underscores
		 *  instead of the INPUT's actual value, but we could
		 *  still get a wrap at the right margin, instead, if
		 *  the value is greater than a line width for the
		 *  current style.  Also, if chars somehow ended up
		 *  longer than the length of the actual value
		 *  (shouldn't have), we'll continue padding with nbsp
		 *  up to the length of chars. - FM
		 */
		for (i = 0; I.value[i]; i++) {
		    HTML_put_character(me,
				       (I.value[i] ==  ' ' ?
					HT_NON_BREAK_SPACE : I.value[i]));
		}
		while (i++ < chars) {
		    HTML_put_character(me, HT_NON_BREAK_SPACE);
		}
	    }
	    HTML_put_character(me, ')');
	    if (me->sp[0].tag_number != HTML_PRE &&
		me->sp->style->freeFormat) {
		HTML_put_character(me, ' ');
		me->in_word = NO;
	    }
	    FREE(I_value);
	    FREE(I_name);
	}
	break;

    case HTML_INPUT:
	{
	    InputFieldData I;
	    int chars;
	    BOOL UseALTasVALUE = FALSE;
	    BOOL HaveSRClink = FALSE;
	    char* ImageSrc = NULL;
	    BOOL IsSubmitOrReset = FALSE;
	    HTkcode kcode = 0;
	    HTkcode specified_kcode = 0;
	    /* init */
	    I.align=NULL; I.accept=NULL; I.checked=NO; I.class=NULL;
	    I.disabled=NO; I.error=NULL; I.height= NULL; I.id=NULL;
	    I.lang=NULL; I.max=NULL; I.maxlength=NULL; I.md=NULL;
	    I.min=NULL; I.name=NULL; I.size=NULL; I.src=NULL;
	    I.type=NULL; I.value=NULL; I.width=NULL;
	    I.accept_cs = NULL;
	    I.name_cs = ATTR_CS_IN;
	    I.value_cs = ATTR_CS_IN;

	    UPDATE_STYLE;

	    /*
	     *	Before any input field, add a collapsible space if
	     *	we're not in a PRE block, to promote a wrap there
	     *	for any long values that would extend past the right
	     *	margin from our current position in the line.  If
	     *	we are in a PRE block, start a new line if the last
	     *	line already is within 6 characters of the wrap point
	     *	for PRE blocks. - FM
	     */
	    if (me->sp[0].tag_number != HTML_PRE && !me->inPRE &&
		me->sp->style->freeFormat) {
		HTML_put_character(me, ' ');
		me->in_word = NO;
	    } else if (HText_LastLineSize(me->text, FALSE) > (LYcols - 7)) {
		HTML_put_character(me, '\n');
		me->in_word = NO;
	    }

	    /*
	     *	Get the TYPE and make sure we can handle it. - FM
	     */
	    if (present && present[HTML_INPUT_TYPE] &&
		value[HTML_INPUT_TYPE] && *value[HTML_INPUT_TYPE]) {
		I.type = value[HTML_INPUT_TYPE];

		if (!strcasecomp(I.type, "range")) {
		    if (present[HTML_INPUT_MIN])
			I.min = value[HTML_INPUT_MIN];
		    if (present[HTML_INPUT_MAX])
			I.max = value[HTML_INPUT_MAX];
		    /*
		     *	Not yet implemented.
		     */
		    HTML_put_string(me,"[RANGE Input] (Not yet implemented.)");
#ifdef NOTDEFINED
		    if (me->inFORM)
			HText_DisableCurrentForm();
#endif /* NOTDEFINED */
		    CTRACE((tfp, "HTML: Ignoring TYPE=\"range\"\n"));
		    break;

		} else if (!strcasecomp(I.type, "file")) {
		    if (present[HTML_INPUT_ACCEPT])
			I.accept = value[HTML_INPUT_ACCEPT];
#ifdef EXP_FILE_UPLOAD
		    /*
		     *	Not yet implemented.
		     */
		    if (me->inUnderline == FALSE) {
			HText_appendCharacter(me->text,
					      LY_UNDERLINE_START_CHAR);
		    }
		    HTML_put_string(me,"[FILE Input] (Not yet implemented.)");
		    if (me->inUnderline == FALSE) {
			HText_appendCharacter(me->text,
					      LY_UNDERLINE_END_CHAR);
		    }
#else
		    CTRACE((tfp, "Attempting to fake as: %s\n", I.type));
#endif /* EXP_FILE_UPLOAD */
#ifdef NOTDEFINED
		    if (me->inFORM)
			HText_DisableCurrentForm();
#endif /* NOTDEFINED */
		    CTRACE((tfp, "HTML: Ignoring TYPE=\"file\"\n"));

		} else if (!strcasecomp(I.type, "button")) {
		    /*
		     *	Ugh, a button for a script.
		     */
		    HTML_put_string(me,"[BUTTON] ");
		    break;
		}
	    }

	    /*
	     *	Check if we're in a form. - FM
	     */
	    if (!me->inFORM) {
		if (LYBadHTML(me))
		    CTRACE((tfp,
			    "Bad HTML: INPUT tag not within FORM tag\n"));
		/*
		 *  We'll process it, since the chances of a crash are
		 *  small, and we probably do have a form started. - FM
		 *
		break;
		 */
	    }

	    CTRACE((tfp, "Ok, we're trying type=[%s]\n", NONNULL(I.type)));

	    /*
	     *	Check for an unclosed TEXTAREA.
	     */
	    if (me->inTEXTAREA) {
		if (LYBadHTML(me))
		    CTRACE((tfp, "Bad HTML: Missing TEXTAREA end tag.\n"));
	    }

	    /*
	     *	Check for an unclosed SELECT, try to close it if found.
	     */
	    if (me->inSELECT) {
		CTRACE((tfp, "HTML: Missing SELECT end tag, faking it...\n"));
		if (me->sp->tag_number != HTML_SELECT) {
		    SET_SKIP_STACK(HTML_SELECT);
		}
		HTML_end_element(me, HTML_SELECT, include);
	    }

	    /*
	     *	Handle the INPUT as for a FORM. - FM
	     */
	    if (!(present && present[HTML_INPUT_NAME] &&
		  value[HTML_INPUT_NAME])) {
		I.name = "";
	    } else if (strchr(value[HTML_INPUT_NAME], '&') == NULL) {
		I.name = value[HTML_INPUT_NAME];
	    } else {
		StrAllocCopy(I_name, value[HTML_INPUT_NAME]);
		UNESCAPE_FIELDNAME_TO_STD(&I_name);
		I.name = I_name;
	    }
	    if ((present && present[HTML_INPUT_ALT] &&
		 value[HTML_INPUT_ALT] && *value[HTML_INPUT_ALT] &&
		 I.type && !strcasecomp(I.type, "image")) &&
		!(present && present[HTML_INPUT_VALUE] &&
		  value[HTML_INPUT_VALUE] && *value[HTML_INPUT_VALUE])) {
		/*
		 *  This is a TYPE="image" using an ALT rather than
		 *  VALUE attribute to indicate the link string for
		 *  text clients or GUIs with image loading off, so
		 *  set the flag to use that as if it were a VALUE
		 *  attribute. - FM
		 */
		UseALTasVALUE = TRUE;
	    }
	    if (verbose_img && !clickable_images &&
		present && present[HTML_INPUT_SRC] &&
		value[HTML_INPUT_SRC] && *value[HTML_INPUT_SRC] &&
		I.type && !strcasecomp(I.type, "image")) {
		ImageSrc = MakeNewImageValue(value);
	    } else if (clickable_images == TRUE &&
		present && present[HTML_INPUT_SRC] &&
		value[HTML_INPUT_SRC] && *value[HTML_INPUT_SRC] &&
		I.type && !strcasecomp(I.type, "image")) {
		StrAllocCopy(href, value[HTML_INPUT_SRC]);
		/*
		 *  We have a TYPE="image" with a non-zero-length SRC
		 *  attribute and want clickable images.  Make the
		 *  SRC's value a link if it's still not zero-length
		 *  legitimizing it. - FM
		 */
		url_type = LYLegitimizeHREF(me, &href, TRUE, TRUE);
		if (*href) {
		    /*
		     *	Check whether a base tag is in effect. - FM
		     */
		    if ((me->inBASE && *href != '#') &&
			(temp = HTParse(href, me->base_href, PARSE_ALL)) &&
			*temp != '\0')
			/*
			 *  Use reference related to the base.
			 */
			StrAllocCopy(href, temp);
		    FREE(temp);

		    /*
		     *	Check whether to fill in localhost. - FM
		     */
		    LYFillLocalFileURL(&href,
				       ((*href != '#' &&
					 me->inBASE) ?
				       me->base_href :
				       me->node_anchor->address));

		    if (me->inA) {
			SET_SKIP_STACK(HTML_A);
			HTML_end_element(me, HTML_A, include);
		    }
		    me->CurrentA = HTAnchor_findChildAndLink(
					me->node_anchor,	/* Parent */
					NULL,			/* Tag */
					href,			/* Addresss */
					(HTLinkType*)0);	/* Type */
		    HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
		    if (me->inBoldH == FALSE)
			HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		    HTML_put_string(me, VERBOSE_IMG(value,HTML_INPUT_SRC,"[IMAGE]"));
		    FREE(newtitle);
		    if (me->inBoldH == FALSE)
			HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		    HText_endAnchor(me->text, 0);
		    HTML_put_character(me, '-');
		    HaveSRClink = TRUE;
		}
		FREE(href);
	    }
	    CTRACE((tfp, "2.Ok, we're trying type=[%s] (present=%p)\n", NONNULL(I.type), present));
	    /* text+file don't go in here */
	    if ((UseALTasVALUE == TRUE) ||
		(present && present[HTML_INPUT_VALUE] &&
		 value[HTML_INPUT_VALUE] &&
		(*value[HTML_INPUT_VALUE] ||
		 (I.type && (!strcasecomp(I.type, "checkbox") ||
			     !strcasecomp(I.type, "radio")))))) {

		/*
		 *  Convert any HTML entities or decimal escaping. - FM
		 */
		int CurrentCharSet = current_char_set;
		BOOL CurrentEightBitRaw = HTPassEightBitRaw;
		BOOLEAN CurrentUseDefaultRawMode = LYUseDefaultRawMode;
		HTCJKlang CurrentHTCJK = HTCJK;
		int len;

		if (I.type && !strcasecomp(I.type, "hidden")) {
		    me->HiddenValue = TRUE;
		    current_char_set = LATIN1;	/* Default ISO-Latin1 */
		    LYUseDefaultRawMode = TRUE;
		    HTMLSetCharacterHandling(current_char_set);
		}

		CTRACE((tfp, "3.Ok, we're trying type=[%s]\n", NONNULL(I.type)));
		if (!I.type)
		    me->UsePlainSpace = TRUE;
		else if (!strcasecomp(I.type, "text") ||
#ifdef EXP_FILE_UPLOAD
			 !strcasecomp(I.type, "file") ||
#endif
			 !strcasecomp(I.type, "submit") ||
			 !strcasecomp(I.type, "image") ||
			 !strcasecomp(I.type, "reset")) {
		    CTRACE((tfp, "normal field type: %s\n", NONNULL(I.type)));
		    me->UsePlainSpace = TRUE;
		}

		StrAllocCopy(I_value,
			     ((UseALTasVALUE == TRUE) ?
				value[HTML_INPUT_ALT] :
				value[HTML_INPUT_VALUE]));
		if (me->UsePlainSpace && !me->HiddenValue) {
		    I.value_cs = current_char_set;
		}
		CTRACE((tfp, "4.Ok, we're trying type=[%s]\n", NONNULL(I.type)));
		TRANSLATE_AND_UNESCAPE_ENTITIES6(
		    &I_value,
		    ATTR_CS_IN,
		    I.value_cs,
		    (me->UsePlainSpace && !me->HiddenValue),
		    me->UsePlainSpace,
		    me->HiddenValue);
		I.value = I_value;
		if (me->UsePlainSpace == TRUE) {
		    /*
		     *	Convert any newlines or tabs to spaces,
		     *	and trim any lead or trailing spaces. - FM
		     */
		    convert_to_spaces(I.value, FALSE);
		    while (I.value && I.value[0] == ' ')
			I.value++;
		    len = strlen(I.value) - 1;
		    while (len > 0 && I.value[len] == ' ')
			I.value[len--] = '\0';
		}
		me->UsePlainSpace = FALSE;


		if (I.type && !strcasecomp(I.type, "hidden")) {
		    me->HiddenValue = FALSE;
		    current_char_set = CurrentCharSet;
		    LYUseDefaultRawMode = CurrentUseDefaultRawMode;
		    HTMLSetCharacterHandling(current_char_set);
		    HTPassEightBitRaw = CurrentEightBitRaw;
		    HTCJK = CurrentHTCJK;
		}
	    } else if (HaveSRClink == TRUE) {
		/*
		 *  We put up an [IMAGE] link and '-' for a TYPE="image"
		 *  and didn't get a VALUE or ALT string, so fake a
		 *  "Submit" value.  If we didn't put up a link, then
		 *  HText_beginInput() will use "[IMAGE]-Submit". - FM
		 */
		StrAllocCopy(I_value, "Submit");
		I.value = I_value;
	    } else if ( ImageSrc ) {
		/* [IMAGE]-Submit with verbose images and not clickable images.
		 * Use ImageSrc if no other alt or value is supplied. --LE
		 */
		I.value = ImageSrc;
	    }
	    if (present && present[HTML_INPUT_CHECKED])
		I.checked = YES;
	    if (present && present[HTML_INPUT_SIZE] &&
		value[HTML_INPUT_SIZE] && *value[HTML_INPUT_SIZE])
		I.size = value[HTML_INPUT_SIZE];
	    if (present && present[HTML_INPUT_MAXLENGTH] &&
		value[HTML_INPUT_MAXLENGTH] && *value[HTML_INPUT_MAXLENGTH])
		I.maxlength = value[HTML_INPUT_MAXLENGTH];
	    if (present && present[HTML_INPUT_DISABLED])
		I.disabled = YES;

	    if (present && present[HTML_INPUT_ACCEPT_CHARSET]) { /* Not yet used. */
		I.accept_cs = value[HTML_INPUT_ACCEPT_CHARSET] ?
			      value[HTML_INPUT_ACCEPT_CHARSET] : "UNKNOWN";
	    }
	    if (present && present[HTML_INPUT_ALIGN] && /* Not yet used. */
		value[HTML_INPUT_ALIGN] && *value[HTML_INPUT_ALIGN])
		I.align = value[HTML_INPUT_ALIGN];
	    if (present && present[HTML_INPUT_CLASS] && /* Not yet used. */
		value[HTML_INPUT_CLASS] && *value[HTML_INPUT_CLASS])
		I.class = value[HTML_INPUT_CLASS];
	    if (present && present[HTML_INPUT_ERROR] && /* Not yet used. */
		value[HTML_INPUT_ERROR] && *value[HTML_INPUT_ERROR])
		I.error = value[HTML_INPUT_ERROR];
	    if (present && present[HTML_INPUT_HEIGHT] && /* Not yet used. */
		value[HTML_INPUT_HEIGHT] && *value[HTML_INPUT_HEIGHT])
		I.height = value[HTML_INPUT_HEIGHT];
	    if (present && present[HTML_INPUT_WIDTH] && /* Not yet used. */
		value[HTML_INPUT_WIDTH] && *value[HTML_INPUT_WIDTH])
		I.width = value[HTML_INPUT_WIDTH];
	    if (present && present[HTML_INPUT_ID] &&
		value[HTML_INPUT_ID] && *value[HTML_INPUT_ID]) {
		I.id = value[HTML_INPUT_ID];
		CHECK_ID(HTML_INPUT_ID);
	    }
	    if (present && present[HTML_INPUT_LANG] && /* Not yet used. */
		value[HTML_INPUT_LANG] && *value[HTML_INPUT_LANG])
		I.lang = value[HTML_INPUT_LANG];
	    if (present && present[HTML_INPUT_MD] && /* Not yet used. */
		value[HTML_INPUT_MD] && *value[HTML_INPUT_MD])
		I.md = value[HTML_INPUT_MD];

	    chars = HText_beginInput(me->text, me->inUnderline, &I);
#ifndef EXP_FILE_UPLOAD
	    CTRACE((tfp, "I.%s have %d chars, or something\n", NONNULL(I.type), chars));
#endif
	    /*
	     *	Submit and reset buttons have values which don't change,
	     *	so HText_beginInput() sets I.value to the string which
	     *	should be displayed, and we'll enter that instead of
	     *	underscore placeholders into the HText structure to
	     *	see it instead of underscores when dumping or printing.
	     *	We also won't worry about a wrap in PRE blocks, because
	     *	the line editor never is invoked for submit or reset
	     *	buttons. - LE & FM
	     */
	    if (I.type &&
		(!strcasecomp(I.type,"submit") ||
		 !strcasecomp(I.type,"reset") ||
		 !strcasecomp(I.type,"image")))
		IsSubmitOrReset = TRUE;

	    if (I.type && chars == 3 &&
		!strcasecomp(I.type, "radio")) {
		/*
		 *  Put a (_) placeholder, and one space
		 *  (collapsible) before the label that is
		 *  expected to follow. - FM
		 */
		HTML_put_string(me, "(_)");
		chars = 0;
		me->in_word = YES;
		if (me->sp[0].tag_number != HTML_PRE &&
		    me->sp->style->freeFormat) {
		    HTML_put_character(me, ' ');
		    me->in_word = NO;
		}
	    } else if (I.type && chars == 3 &&
		!strcasecomp(I.type, "checkbox")) {
		/*
		 *  Put a [_] placeholder, and one space
		 *  (collapsible) before the label that is
		 *  expected to follow. - FM
		 */
		HTML_put_string(me, "[_]");
		chars = 0;
		me->in_word = YES;
		if (me->sp[0].tag_number != HTML_PRE &&
		    me->sp->style->freeFormat) {
		    HTML_put_character(me, ' ');
		    me->in_word = NO;
		}
	    } else if ((me->sp[0].tag_number == HTML_PRE ||
			!me->sp->style->freeFormat)
		       && chars > 6 &&
		       IsSubmitOrReset == FALSE) {
		/*
		 *  This is not a submit or reset button, and we are
		 *  in a PRE block with a field intended to exceed 6
		 *  character widths.  The code inadequately handles
		 *  INPUT fields in PRE tags if wraps occur (at the
		 *  right margin) for the underscore placeholders.
		 *  We'll put up a minimum of 6 underscores, since we
		 *  should have wrapped artificially, above, if the
		 *  INPUT begins within 6 columns of the right margin,
		 *  and if any more would exceed the wrap column, we'll
		 *  ignore them.  Note that if we somehow get tripped
		 *  up and a wrap still does occur before all 6 of the
		 *  underscores are output, the wrapped ones won't be
		 *  treated as part of the editing window, nor be
		 *  highlighted when not editing (Yuk!). - FM
		 */
		for (i = 0; i < 6; i++) {
		    HTML_put_character(me, '_');
		    chars--;
		}
		HText_setIgnoreExcess(me->text, TRUE);
	    }
#ifndef EXP_FILE_UPLOAD
	    CTRACE((tfp, "I.%s, %d\n", NONNULL(I.type), IsSubmitOrReset));
#endif
	    if (IsSubmitOrReset == FALSE) {
		/*
		 *  This is not a submit or reset button,
		 *  so output the rest of the underscore
		 *  placeholders, if any more are needed. - FM
		 */
		for (; chars > 0; chars--)
		    HTML_put_character(me, '_');
	    } else {
		if (HTCJK == JAPANESE) {
		    kcode = HText_getKcode(me->text);
		    HText_updateKcode(me->text, kanji_code);
		    specified_kcode = HText_getSpecifiedKcode(me->text);
		    HText_updateSpecifiedKcode(me->text, kanji_code);
		}
		if (me->sp[0].tag_number == HTML_PRE ||
		    !me->sp->style->freeFormat) {
		    /*
		     *	We have a submit or reset button in a PRE block,
		     *	so output the entire value from the markup.  If
		     *	it extends to the right margin, it will wrap
		     *	there, and only the portion before that wrap will
		     *	be hightlighted on screen display (Yuk!) but we
		     *	may as well show the rest of the full value on
		     *	the next or more lines. - FM
		     */
		    while (I.value[i])
			HTML_put_character(me, I.value[i++]);
		} else {
		    /*
		     *	The submit or reset button is not in a PRE block.
		     *	Note that if a wrap occurs before outputting the
		     *	entire value, the wrapped portion will not be
		     *	highlighted or clearly indicated as part of the
		     *	link for submission or reset (Yuk!).
		     *	We'll replace any spaces in the submit or reset
		     *	button value with nbsp, to promote a wrap at the
		     *	space we ensured would be present before the start
		     *	of the string, as when we use all underscores
		     *	instead of the INPUT's actual value, but we could
		     *	still get a wrap at the right margin, instead, if
		     *	the value is greater than a line width for the
		     *	current style.	Also, if chars somehow ended up
		     *	longer than the length of the actual value
		     *	(shouldn't have), we'll continue padding with nbsp
		     *	up to the length of chars. - FM
		     */
		    for (i = 0; I.value[i]; i++)
			HTML_put_character(me,
					   (I.value[i] ==  ' ' ?
					    HT_NON_BREAK_SPACE : I.value[i]));
		    while (i++ < chars)
			HTML_put_character(me, HT_NON_BREAK_SPACE);
		}
		if (HTCJK == JAPANESE) {
		    HText_updateKcode(me->text, kcode);
		    HText_updateSpecifiedKcode(me->text, specified_kcode);
		}
	    }
	    HText_setIgnoreExcess(me->text, FALSE);
	    FREE(ImageSrc);
	    FREE(I_value);
	    FREE(I_name);
	}
	break;

    case HTML_TEXTAREA:
	/*
	 *  Make sure we're in a form.
	 */
	if (!me->inFORM) {
	    if (LYBadHTML(me))
		CTRACE((tfp, "Bad HTML: TEXTAREA start tag not within FORM tag\n"));
	    /*
	     *	Too likely to cause a crash, so we'll ignore it. - FM
	     */
	    break;
	}

	/*
	 *  Set to know we are in a textarea.
	 */
	me->inTEXTAREA = TRUE;

	/*
	 *  Get ready for the value.
	 */
	HTChunkClear(&me->textarea);
	if (present && present[HTML_TEXTAREA_NAME] &&
	    value[HTML_TEXTAREA_NAME]) {
	    StrAllocCopy(me->textarea_name, value[HTML_TEXTAREA_NAME]);
	    me->textarea_name_cs = ATTR_CS_IN;
	    if (strchr(value[HTML_TEXTAREA_NAME], '&') != NULL) {
		UNESCAPE_FIELDNAME_TO_STD(&me->textarea_name);
	    }
	} else {
	    StrAllocCopy(me->textarea_name, "");
	}

	if (present && present[HTML_TEXTAREA_ACCEPT_CHARSET]) {
	    if (value[HTML_TEXTAREA_ACCEPT_CHARSET]) {
		StrAllocCopy(me->textarea_accept_cs, value[HTML_TEXTAREA_ACCEPT_CHARSET]);
		TRANSLATE_AND_UNESCAPE_TO_STD(&me->textarea_accept_cs);
	    } else {
		StrAllocCopy(me->textarea_accept_cs, "UNKNOWN");
	    }
	} else {
	    FREE(me->textarea_accept_cs);
	}

	if (present && present[HTML_TEXTAREA_COLS] &&
	    value[HTML_TEXTAREA_COLS] &&
	    isdigit(UCH(*value[HTML_TEXTAREA_COLS])))
	    StrAllocCopy(me->textarea_cols, value[HTML_TEXTAREA_COLS]);
	else {
	    int width;
	    width = LYcols - 1 -
		    me->new_style->leftIndent - me->new_style->rightIndent;
	    if (dump_output_immediately) /* don't waste too much for this */
		width = HTMIN(width, 60);
	    if (width > 1 && (width-1)*6 < MAX_LINE - 3 -
		me->new_style->leftIndent - me->new_style->rightIndent)
		HTSprintf0(&me->textarea_cols, "%d", width);
	    else
		StrAllocCopy(me->textarea_cols, "60");
	}

	if (present && present[HTML_TEXTAREA_ROWS] &&
	    value[HTML_TEXTAREA_ROWS] &&
	    isdigit(UCH(*value[HTML_TEXTAREA_ROWS])))
	    me->textarea_rows = atoi(value[HTML_TEXTAREA_ROWS]);
	else
	    me->textarea_rows = 4;

	if (present && present[HTML_TEXTAREA_DISABLED])
	    me->textarea_disabled = YES;
	else
	    me->textarea_disabled = NO;

	if (present && present[HTML_TEXTAREA_ID]
	    && value[HTML_TEXTAREA_ID] && *value[HTML_TEXTAREA_ID]) {
	    StrAllocCopy(id_string, value[HTML_TEXTAREA_ID]);
	    TRANSLATE_AND_UNESCAPE_TO_STD(&id_string);
	    if ((id_string != '\0') &&
		(ID_A = HTAnchor_findChildAndLink(
				me->node_anchor,	/* Parent */
				id_string,		/* Tag */
				NULL,			/* Addresss */
				(HTLinkType*)0))) {	/* Type */
		HText_beginAnchor(me->text, me->inUnderline, ID_A);
		HText_endAnchor(me->text, 0);
		StrAllocCopy(me->textarea_id, id_string);
	    } else {
		FREE(me->textarea_id);
	    }
	    FREE(id_string);
	} else {
	    FREE(me->textarea_id);
	}
	break;

    case HTML_SELECT:
	/*
	 *  Check for an already open SELECT block. - FM
	 */
	if (me->inSELECT) {
	    if (LYBadHTML(me))
		CTRACE((tfp, "Bad HTML: SELECT start tag in SELECT element.  Faking SELECT end tag. *****\n"));
	    if (me->sp->tag_number != HTML_SELECT) {
		SET_SKIP_STACK(HTML_SELECT);
	    }
	    HTML_end_element(me, HTML_SELECT, include);
	}

	/*
	 * Start a new SELECT block. - FM
	 */
	LYHandleSELECT(me,
		       present, (CONST char **)value,
		       include,
		       TRUE);
	break;

    case HTML_OPTION:
	{
	    /*
	     *	An option is a special case of an input field.
	     */
	    InputFieldData I;

	    /*
	     *	Make sure we're in a select tag.
	     */
	    if (!me->inSELECT) {
		if (LYBadHTML(me))
		    CTRACE((tfp, "Bad HTML: OPTION tag not within SELECT tag\n"));

		/*
		 *  Too likely to cause a crash, so we'll ignore it. - FM
		 */
		break;
	    }

	    if (!me->first_option) {
		/*
		 *  Finish the data off.
		 */
		HTChunkTerminate(&me->option);

		/*
		 *  Finish the previous option @@@@@
		 */
		HText_setLastOptionValue(me->text,
					 me->option.data,
					 me->LastOptionValue,
					 MIDDLE_ORDER,
					 me->LastOptionChecked,
					 me->UCLYhndl,
					 ATTR_CS_IN);
	    }

	    /*
	     *	If its not a multiple option list and select popups
	     *	are enabled, then don't use the checkbox/button method,
	     *	and don't put anything on the screen yet.
	     */
	    if (me->first_option ||
		HTCurSelectGroupType == F_CHECKBOX_TYPE ||
		LYSelectPopups == FALSE) {
		if (HTCurSelectGroupType == F_CHECKBOX_TYPE ||
		    LYSelectPopups == FALSE) {
		    /*
		     *	Start a newline before each option.
		     */
		    LYEnsureSingleSpace(me);
		} else {
		    /*
		     *	Add option list designation character.
		     */
		    HText_appendCharacter(me->text, '[');
		    me->in_word = YES;
		}

		/*
		 *  Inititialize.
		 */
		I.align=NULL; I.accept=NULL; I.checked=NO; I.class=NULL;
		I.disabled=NO; I.error=NULL; I.height= NULL; I.id=NULL;
		I.lang=NULL; I.max=NULL; I.maxlength=NULL; I.md=NULL;
		I.min=NULL; I.name=NULL; I.size=NULL; I.src=NULL;
		I.type=NULL; I.value=NULL; I.width=NULL;
		I.accept_cs = NULL;
		I.name_cs = -1;
		I.value_cs = current_char_set;

		I.type = "OPTION";

		if ((present && present[HTML_OPTION_SELECTED]) ||
		    (me->first_option && LYSelectPopups == FALSE &&
		     HTCurSelectGroupType == F_RADIO_TYPE))
		    I.checked=YES;

		if (present && present[HTML_OPTION_VALUE] &&
		    value[HTML_OPTION_VALUE]) {
		    /*
		     *	Convert any HTML entities or decimal escaping. - FM
		     */
		    StrAllocCopy(I_value, value[HTML_OPTION_VALUE]);
		    me->HiddenValue = TRUE;
		    TRANSLATE_AND_UNESCAPE_ENTITIES6(&I_value,
						       ATTR_CS_IN,
						       ATTR_CS_IN,
							NO,
						       me->UsePlainSpace, me->HiddenValue);
		    I.value_cs = ATTR_CS_IN;
		    me->HiddenValue = FALSE;

		    I.value = I_value;
		}

		if (me->select_disabled ||
		   (present && present[HTML_OPTION_DISABLED]))
		    I.disabled=YES;

		if (present && present[HTML_OPTION_ID]
		    && value[HTML_OPTION_ID] && *value[HTML_OPTION_ID]) {
		    if ((ID_A = HTAnchor_findChildAndLink(
				    me->node_anchor,	   /* Parent */
				    value[HTML_OPTION_ID], /* Tag */
				    NULL,		   /* Addresss */
				    (HTLinkType*)0)) != NULL) {    /* Type */
			HText_beginAnchor(me->text, me->inUnderline, ID_A);
			HText_endAnchor(me->text, 0);
			I.id = value[HTML_OPTION_ID];
		    }
		}

		HText_beginInput(me->text, me->inUnderline, &I);

		if (HTCurSelectGroupType == F_CHECKBOX_TYPE) {
		    /*
		     *	Put a "[_]" placeholder, and one space
		     *	(collapsible) before the label that is
		     *	expected to follow. - FM
		     */
		    HText_appendCharacter(me->text, '[');
		    HText_appendCharacter(me->text, '_');
		    HText_appendCharacter(me->text, ']');
		    HText_appendCharacter(me->text, ' ');
		    HText_setLastChar(me->text, ' ');  /* absorb white space */
		    me->in_word = NO;
		} else if (LYSelectPopups == FALSE) {
		    /*
		     *	Put a "(_)" placeholder, and one space
		     *	(collapsible) before the label that is
		     *	expected to follow. - FM
		     */
		    HText_appendCharacter(me->text, '(');
		    HText_appendCharacter(me->text, '_');
		    HText_appendCharacter(me->text, ')');
		    HText_appendCharacter(me->text, ' ');
		    HText_setLastChar(me->text, ' ');  /* absorb white space */
		    me->in_word = NO;
		}
	    }

	    /*
	     *	Get ready for the next value.
	     */
	    HTChunkClear(&me->option);
	    if ((present && present[HTML_OPTION_SELECTED]) ||
		(me->first_option && LYSelectPopups == FALSE &&
		 HTCurSelectGroupType == F_RADIO_TYPE))
		me->LastOptionChecked = TRUE;
	    else
		me->LastOptionChecked = FALSE;
	    me->first_option = FALSE;


	    if (present && present[HTML_OPTION_VALUE] &&
		value[HTML_OPTION_VALUE]) {
		if (!I_value) {
		    /*
		     *	Convert any HTML entities or decimal escaping. - FM
		     */
		    StrAllocCopy(I_value, value[HTML_OPTION_VALUE]);
		    me->HiddenValue = TRUE;
		    TRANSLATE_AND_UNESCAPE_ENTITIES6(&I_value,
						       ATTR_CS_IN,
						       ATTR_CS_IN,
							NO,
						       me->UsePlainSpace, me->HiddenValue);
		    me->HiddenValue = FALSE;
		}
		StrAllocCopy(me->LastOptionValue, I_value);
	    } else {
		StrAllocCopy(me->LastOptionValue, me->option.data);
	    }

	    /*
	     *	If this is a popup option, print its option
	     *	for use in selecting option by number. - LE
	     */
	    if (HTCurSelectGroupType == F_RADIO_TYPE &&
		LYSelectPopups &&
		keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) {
		char marker[8];
		int opnum = HText_getOptionNum(me->text);

		if (opnum > 0 && opnum < 100000) {
		    sprintf(marker,"(%d)", opnum);
		    HTML_put_string(me, marker);
		    for (i = strlen(marker); i < 5; ++i) {
			HTML_put_character(me, '_');
		    }
		}
	    }
	    FREE(I_value);
	}
	break;

    case HTML_TABLE:
	/*
	 *  Not fully implemented.  Just treat as a division
	 *  with respect to any ALIGN attribute, with
	 *  a default of HT_LEFT, or leave as a PRE
	 *  block if we are presently in one. - FM
	 *
	 *  Also notify simple table tracking code unless
	 *  in a preformatted section, or (currently) non-left
	 *  alignment.  But first cancel tracking any already
	 *  open (enclosing) table.
	 *
	 *  If page author is using a TABLE within PRE, it's probably
	 *  formatted specifically to work well for Lynx without simple
	 *  table tracking code.  Cancel tracking, it would only make
	 *  things worse. - kw
	 */
	HText_cancelStbl(me->text);
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	if (me->Underline_Level > 0) {
	    SET_SKIP_STACK(HTML_U);
	    HTML_end_element(me, HTML_U, include);
	}
	me->inTABLE = TRUE;
	if (!strcmp(me->sp->style->name, "Preformatted")) {
	    UPDATE_STYLE;
	    CHECK_ID(HTML_TABLE_ID);
	    break;
	}
	if (me->Division_Level < (MAX_NESTING - 1)) {
	    me->Division_Level++;
	} else {
	    CTRACE((tfp,
	    "HTML: ****** Maximum nesting of %d divisions/tables exceeded!\n",
		    MAX_NESTING));
	}
	if (present && present[HTML_TABLE_ALIGN] &&
	    value[HTML_TABLE_ALIGN] && *value[HTML_TABLE_ALIGN]) {
	    if (!strcasecomp(value[HTML_TABLE_ALIGN], "center")) {
#ifdef SH_EX	/* 1998/10/09 (Fri) 15:20:09 */
		if (no_table_center) {
		    me->DivisionAlignments[me->Division_Level] = HT_LEFT;
		    change_paragraph_style(me, styles[HTML_DLEFT]);
		    UPDATE_STYLE;
		    me->current_default_alignment =
				styles[HTML_DLEFT]->alignment;
		} else {
		    me->DivisionAlignments[me->Division_Level] = HT_CENTER;
		    change_paragraph_style(me, styles[HTML_DCENTER]);
		    UPDATE_STYLE;
		    me->current_default_alignment =
					styles[HTML_DCENTER]->alignment;
		}
#else
		me->DivisionAlignments[me->Division_Level] = HT_CENTER;
		change_paragraph_style(me, styles[HTML_DCENTER]);
		UPDATE_STYLE;
		me->current_default_alignment = styles[HTML_DCENTER]->alignment;
#endif
		stbl_align = HT_CENTER;

	    } else if (!strcasecomp(value[HTML_TABLE_ALIGN], "right")) {
		me->DivisionAlignments[me->Division_Level] = HT_RIGHT;
		change_paragraph_style(me, styles[HTML_DRIGHT]);
		UPDATE_STYLE;
		me->current_default_alignment = styles[HTML_DRIGHT]->alignment;
		stbl_align = HT_RIGHT;
	    } else {
		me->DivisionAlignments[me->Division_Level] = HT_LEFT;
		change_paragraph_style(me, styles[HTML_DLEFT]);
		UPDATE_STYLE;
		me->current_default_alignment = styles[HTML_DLEFT]->alignment;
		if (!strcasecomp(value[HTML_TABLE_ALIGN], "left") ||
		    !strcasecomp(value[HTML_TABLE_ALIGN], "justify"))
		    stbl_align = HT_LEFT;
	    }
	} else {
	    me->DivisionAlignments[me->Division_Level] = HT_LEFT;
	    change_paragraph_style(me, styles[HTML_DLEFT]);
	    UPDATE_STYLE;
	    me->current_default_alignment = styles[HTML_DLEFT]->alignment;
	    /* stbl_align remains HT_ALIGN_NONE */
	}
	CHECK_ID(HTML_TABLE_ID);
	HText_startStblTABLE(me->text, stbl_align);
	break;

    case HTML_TR:
	/*
	 *  Not fully implemented.  Just start a new row,
	 *  if needed, act on an ALIGN attribute if present,
	 *  and check for an ID link. - FM
	 *  Also notify simple table tracking code. - kw
	 */
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	if (me->Underline_Level > 0) {
	    SET_SKIP_STACK(HTML_U);
	    HTML_end_element(me, HTML_U, include);
	}
	UPDATE_STYLE;
	if (HText_LastLineSize(me->text, FALSE)) {
	    HText_setLastChar(me->text, ' ');  /* absorb white space */
	    HText_appendCharacter(me->text, '\r');
	}
	me->in_word = NO;

	if (!strcmp(me->sp->style->name, "Preformatted")) {
	    CHECK_ID(HTML_TR_ID);
	    me->inP = FALSE;
/*	    HText_cancelStbl(me->text);  seems unnecessary here - kw */
	    break;
	}
	if (LYoverride_default_alignment(me)) {
	    me->sp->style->alignment = styles[me->sp[0].tag_number]->alignment;
	} else if (me->List_Nesting_Level >= 0 ||
		   ((me->Division_Level < 0) &&
		    (!strcmp(me->sp->style->name, "Normal") ||
		     !strcmp(me->sp->style->name, "Preformatted")))) {
		me->sp->style->alignment = HT_LEFT;
	} else {
	    me->sp->style->alignment = (short) me->current_default_alignment;
	}
	if (present && present[HTML_TR_ALIGN] && value[HTML_TR_ALIGN]) {
	    if (!strcasecomp(value[HTML_TR_ALIGN], "center") &&
		!(me->List_Nesting_Level >= 0 && !me->inP)) {
#ifdef SH_EX
		if (no_table_center)
		    me->sp->style->alignment = HT_LEFT;
		else
		    me->sp->style->alignment = HT_CENTER;
#else
		me->sp->style->alignment = HT_CENTER;
#endif
		stbl_align = HT_CENTER;
	    } else if (!strcasecomp(value[HTML_TR_ALIGN], "right") &&
		       !(me->List_Nesting_Level >= 0 && !me->inP)) {
		me->sp->style->alignment = HT_RIGHT;
		stbl_align = HT_RIGHT;
	    } else if (!strcasecomp(value[HTML_TR_ALIGN], "left") ||
		       !strcasecomp(value[HTML_TR_ALIGN], "justify")) {
		me->sp->style->alignment = HT_LEFT;
		stbl_align = HT_LEFT;
	    }
	}

	CHECK_ID(HTML_TR_ID);
	me->inP = FALSE;
	HText_startStblTR(me->text, stbl_align);
	break;

    case HTML_THEAD:
    case HTML_TFOOT:
    case HTML_TBODY:
	HText_endStblTR(me->text);
	/*
	 *  Not fully implemented.  Just check for an ID link. - FM
	 */
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	if (me->Underline_Level > 0) {
	    SET_SKIP_STACK(HTML_U);
	    HTML_end_element(me, HTML_U, include);
	}
	UPDATE_STYLE;
	if (me->inTABLE) {
	    if (present && present[HTML_TR_ALIGN] && value[HTML_TR_ALIGN]) {
		if (!strcasecomp(value[HTML_TR_ALIGN], "center")) {
		    stbl_align = HT_CENTER;
		} else if (!strcasecomp(value[HTML_TR_ALIGN], "right")) {
		    stbl_align = HT_RIGHT;
		} else if (!strcasecomp(value[HTML_TR_ALIGN], "left") ||
			   !strcasecomp(value[HTML_TR_ALIGN], "justify")) {
		    stbl_align = HT_LEFT;
		}
	    }
	    HText_startStblRowGroup(me->text, stbl_align);
	}
	CHECK_ID(HTML_TR_ID);
	break;

    case HTML_COL:
    case HTML_COLGROUP:
	/*
	 *  Not fully implemented.  Just check for an ID link. - FM
	 */
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	if (me->Underline_Level > 0) {
	    SET_SKIP_STACK(HTML_U);
	    HTML_end_element(me, HTML_U, include);
	}
	UPDATE_STYLE;
	if (me->inTABLE) {
	    int span = 1;
	    if (present && present[HTML_COL_SPAN] &&
		value[HTML_COL_SPAN] &&
		isdigit(UCH(*value[HTML_COL_SPAN])))
		span = atoi(value[HTML_COL_SPAN]);
	    if (present && present[HTML_COL_ALIGN] && value[HTML_COL_ALIGN]) {
		if (!strcasecomp(value[HTML_COL_ALIGN], "center")) {
		    stbl_align = HT_CENTER;
		} else if (!strcasecomp(value[HTML_COL_ALIGN], "right")) {
		    stbl_align = HT_RIGHT;
		} else if (!strcasecomp(value[HTML_COL_ALIGN], "left") ||
			   !strcasecomp(value[HTML_COL_ALIGN], "justify")) {
		    stbl_align = HT_LEFT;
		}
	    }
	    HText_startStblCOL(me->text, span, stbl_align,
			       (ElementNumber == HTML_COLGROUP));
	}
	CHECK_ID(HTML_COL_ID);
	break;

    case HTML_TH:
    case HTML_TD:
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, include);
	}
	if (me->Underline_Level > 0) {
	    SET_SKIP_STACK(HTML_U);
	    HTML_end_element(me, HTML_U, include);
	}
	UPDATE_STYLE;
	CHECK_ID(HTML_TD_ID);
	/*
	 *  Not fully implemented.  Just add a collapsible space and break. - FM
	 *  Also notify simple table tracking code. - kw
	 */
	HTML_put_character(me, ' ');
	{
	    int colspan = 1, rowspan = 1;
	    if (present && present[HTML_TD_COLSPAN] &&
		value[HTML_TD_COLSPAN] &&
		isdigit(UCH(*value[HTML_TD_COLSPAN])))
		colspan = atoi(value[HTML_TD_COLSPAN]);
	    if (present && present[HTML_TD_ROWSPAN] &&
		value[HTML_TD_ROWSPAN] &&
		isdigit(UCH(*value[HTML_TD_ROWSPAN])))
		rowspan = atoi(value[HTML_TD_ROWSPAN]);
	    if (present && present[HTML_TD_ALIGN] && value[HTML_TD_ALIGN]) {
		if (!strcasecomp(value[HTML_TD_ALIGN], "center")) {
		    stbl_align = HT_CENTER;
		} else if (!strcasecomp(value[HTML_TD_ALIGN], "right")) {
		    stbl_align = HT_RIGHT;
		} else if (!strcasecomp(value[HTML_TD_ALIGN], "left") ||
			   !strcasecomp(value[HTML_TD_ALIGN], "justify")) {
		    stbl_align = HT_LEFT;
		}
	    }
	    HText_startStblTD(me->text, colspan, rowspan, stbl_align,
			      (ElementNumber == HTML_TH));
	}
	me->in_word = NO;
	break;

    case HTML_MATH:
	/*
	 *  We're getting it as Literal text, which, until we can process
	 *  it, we'll display as is, within brackets to alert the user. - FM
	 */
	HTChunkClear(&me->math);
	CHECK_ID(HTML_GEN_ID);
	break;

    default:
	break;

    } /* end switch */

    if (ElementNumber >= HTML_ELEMENTS ||
	HTML_dtd.tags[ElementNumber].contents != SGML_EMPTY) {
	if (me->skip_stack > 0) {
	    CTRACE((tfp,
    "HTML:begin_element: internal call (level %d), leaving on stack - `%s'\n",
			me->skip_stack, NONNULL(me->sp->style->name)));
	    me->skip_stack--;
	    return status;
	}
	if (me->sp == me->stack) {
	    if (me->stack_overrun == FALSE) {
		HTAlert(HTML_STACK_OVERRUN);
		CTRACE((tfp,
			"HTML: ****** Maximum nesting of %d tags exceeded!\n",
			MAX_NESTING));
		me->stack_overrun = TRUE;
	    }
	    return HT_ERROR;
	}

	CTRACE((tfp,"HTML:begin_element[%d]: adding style to stack - %s\n",
							(int) STACKLEVEL(me),
							NONNULL(me->new_style->name)));
	(me->sp)--;
	me->sp[0].style = me->new_style;	/* Stack new style */
	me->sp[0].tag_number = ElementNumber;
#ifdef EXP_JUSTIFY_ELTS
	if (wait_for_this_stacked_elt < 0 &&
		HTML_dtd.tags[ElementNumber].can_justify == FALSE)
	    wait_for_this_stacked_elt = me->stack - me->sp + MAX_NESTING;
#endif
    }

#ifdef EXP_JUSTIFY_ELTS
    if (in_DT && ElementNumber == HTML_DD)
	in_DT = FALSE;
    else if (ElementNumber == HTML_DT)
	in_DT = TRUE;
#endif

#if defined(USE_COLOR_STYLE)
/* end really empty tags straight away */

    if (ReallyEmptyTagNum(element_number))
    {
	CTRACE((tfp, "STYLE.begin_element:ending \"EMPTY\" element style\n"));
#if !defined(USE_HASH)
	HText_characterStyle(me->text, element_number+STARTAT, STACK_OFF);
#else
	HText_characterStyle(me->text, HCODE_TO_STACK_OFF(hcode), STACK_OFF);
#endif /* USE_HASH */

#if !OPT_SCN
	TrimColorClass(HTML_dtd.tags[element_number].name,
		       Style_className, &hcode);
#else
#  if !OMIT_SCN_KEEPING
	FastTrimColorClass(HTML_dtd.tags[element_number].name,
			   HTML_dtd.tags[element_number].name_len,
			   Style_className,
			   &Style_className_end, &hcode);
#  endif
#endif
    }
#endif /* USE_COLOR_STYLE */
    return status;
}

/*		End Element
**		-----------
**
**	When we end an element, the style must be returned to that
**	in effect before that element.	Note that anchors (etc?)
**	don't have an associated style, so that we must scan down the
**	stack for an element with a defined style. (In fact, the styles
**	should be linked to the whole stack not just the top one.)
**	TBL 921119
**
**	We don't turn on "CAREFUL" check because the parser produces
**	(internal code errors apart) good nesting.  The parser checks
**	incoming code errors, not this module.
*/
PRIVATE int HTML_end_element ARGS3(
	HTStructured *,		me,
	int,			element_number,
	char **,		include)
{
    int i = 0;
    int status = HT_OK;
    char *temp = NULL, *cp = NULL;
    BOOL BreakFlag = FALSE;
    BOOL intern_flag = FALSE;
    BOOL skip_stack_requested = FALSE;
    EMIT_IFDEF_EXP_JUSTIFY_ELTS(BOOL reached_awaited_stacked_elt = FALSE);

#ifdef USE_PRETTYSRC
    if (psrc_view && !sgml_in_psrc_was_initialized) {
	if (!psrc_nested_call) {
	    HTTag * tag = &HTML_dtd.tags[element_number];
	    char buf[200];
	    int tag_charset=0;

	    psrc_nested_call=TRUE;
	    PSRCSTART(abracket); PUTS("</"); PSRCSTOP(abracket);
	    PSRCSTART(tag);
	    if (tagname_transform!=0)
		PUTS(tag->name);
	    else {
		LYstrncpy(buf, tag->name, sizeof(buf)-1);
		LYLowerCase(buf);
		PUTS(buf);
	    }
	    PSRCSTOP(tag);
	    PSRCSTART(abracket); PUTC('>'); PSRCSTOP(abracket);
	    psrc_nested_call=FALSE;
	    return HT_OK;
	}
	/*fall through*/
    }
#endif

    if ((me->sp >= (me->stack + MAX_NESTING - 1) ||
	 element_number != me->sp[0].tag_number) &&
	HTML_dtd.tags[element_number].contents != SGML_EMPTY) {
	CTRACE((tfp,
		"HTML: end of element %s when expecting end of %s\n",
		HTML_dtd.tags[element_number].name,
	       (me->sp == me->stack + MAX_NESTING - 1) ? "none" :
	       (me->sp->tag_number < 0) ? "*invalid tag*" :
	       (me->sp->tag_number >= HTML_ELEMENTS) ? "special tag" :
		HTML_dtd.tags[me->sp->tag_number].name));
#ifdef CAREFUL			/* parser assumed to produce good nesting */
		/* panic */
#endif /* CAREFUL */
    }

    /*
     *	If we're seeking MAPs, skip everything that's
     *	not a MAP or AREA tag. - FM
     */
    if (LYMapsOnly) {
	if (!(element_number == HTML_MAP || element_number == HTML_AREA ||
	      element_number == HTML_OBJECT)) {
	    return HT_OK;
	}
    }

    /*
     *	Pop state off stack if we didn't declare the element
     *	SGML_EMPTY in HTMLDTD.c. - FM & KW
     */
    if (HTML_dtd.tags[element_number].contents != SGML_EMPTY) {
	skip_stack_requested = me->skip_stack > 0;
	if ((element_number != me->sp[0].tag_number) &&
	    me->skip_stack <= 0 &&
	    HTML_dtd.tags[HTML_LH].contents != SGML_EMPTY &&
	    (me->sp[0].tag_number == HTML_UL ||
	     me->sp[0].tag_number == HTML_OL ||
	     me->sp[0].tag_number == HTML_MENU ||
	     me->sp[0].tag_number == HTML_DIR ||
	     me->sp[0].tag_number == HTML_LI) &&
	    (element_number == HTML_H1 ||
	     element_number == HTML_H2 ||
	     element_number == HTML_H3 ||
	     element_number == HTML_H4 ||
	     element_number == HTML_H6 ||
	     element_number == HTML_H6)) {
	    /*
	     *	Set the break flag if we're popping
	     *	a dummy HTML_LH substituted for an
	     *	HTML_H# encountered in a list.
	     */
	    BreakFlag = TRUE;
	}
	if (me->skip_stack == 0 && element_number == HTML_OBJECT &&
	    me->sp[0].tag_number == HTML_OBJECT_M &&
	    (me->sp < (me->stack + MAX_NESTING - 1)))
	    me->sp[0].tag_number = HTML_OBJECT;
	if (me->skip_stack > 0) {
	    CTRACE((tfp, "HTML:end_element: Internal call (level %d), leaving on stack - %s\n",
		    me->skip_stack, NONNULL(me->sp->style->name)));
	    me->skip_stack--;
	} else if (element_number == HTML_OBJECT &&
		   me->sp[0].tag_number != HTML_OBJECT &&
		   me->sp[0].tag_number != HTML_OBJECT_M &&
		   me->objects_mixed_open > 0 &&
		   !(me->objects_figged_open > 0 &&
		       me->sp[0].tag_number == HTML_FIG)) {
	    /*
	     *	Ignore non-corresponding OBJECT tags that we
	     *	didn't push because the SGML parser was supposed
	     *  to go on parsing the contents non-literally. - kw
	     */
	    CTRACE((tfp,
		   "HTML:end_element[%d]: %s (level %d), %s - %s\n",
		   (int) STACKLEVEL(me),
		   "Special OBJECT handling", me->objects_mixed_open,
		   "leaving on stack",
		   NONNULL(me->sp->style->name)));
	    me->objects_mixed_open--;
	} else if (me->stack_overrun == TRUE &&
	    element_number != me->sp[0].tag_number) {
	    /*
	     *	Ignore non-corresponding tags if we had
	     *	a stack overrun.  This is not a completely
	     *	fail-safe strategy for protection against
	     *	any seriously adverse consequences of a
	     *	stack overrun, and the rendering of the
	     *	document will not be as intended, but we
	     *	expect overruns to be rare, and this should
	     *	offer reasonable protection against crashes
	     *	if an overrun does occur. - FM
	     */
	    return HT_OK; /* let's pretend... */
	} else if (element_number == HTML_SELECT &&
	    me->sp[0].tag_number != HTML_SELECT) {
	    /*
	     *	Ignore non-corresponding SELECT tags, since we
	     *	probably popped it and closed the SELECT block
	     *	to deal with markup which amounts to a nested
	     *	SELECT, or an out of order FORM end tag. - FM
	     */
	    return HT_OK;
	} else if ((element_number != me->sp[0].tag_number) &&
	    HTML_dtd.tags[HTML_LH].contents == SGML_EMPTY &&
	    (me->sp[0].tag_number == HTML_UL ||
	     me->sp[0].tag_number == HTML_OL ||
	     me->sp[0].tag_number == HTML_MENU ||
	     me->sp[0].tag_number == HTML_DIR ||
	     me->sp[0].tag_number == HTML_LI) &&
	    (element_number == HTML_H1 ||
	     element_number == HTML_H2 ||
	     element_number == HTML_H3 ||
	     element_number == HTML_H4 ||
	     element_number == HTML_H6 ||
	     element_number == HTML_H6)) {
	    /*
	     *	It's an H# for which we substituted
	     *	an HTML_LH, which we've declared as
	     *	SGML_EMPTY, so just return. - FM
	     */
	    return HT_OK;
	} else if (me->sp < (me->stack + MAX_NESTING - 1)) {
#ifdef EXP_JUSTIFY_ELTS
	    if (wait_for_this_stacked_elt == me->stack - me->sp + MAX_NESTING)
		reached_awaited_stacked_elt = TRUE;
#endif
	    if (element_number == HTML_OBJECT) {
		if (me->sp[0].tag_number == HTML_FIG &&
		    me->objects_figged_open > 0) {
		    /*
		     *  It's an OBJECT for which we substituted a FIG,
		     *  so pop the FIG and pretend that's what we are
		     *  being called for. - kw
		     */
		    CTRACE((tfp,
			"HTML:end_element[%d]: %s (level %d), %s - %s\n",
			(int) STACKLEVEL(me),
			"Special OBJECT->FIG handling", me->objects_figged_open,
			"treating as end FIG",
			NONNULL(me->sp->style->name)));
		    me->objects_figged_open--;
		    element_number = HTML_FIG;
		}
	    }
	    (me->sp)++;
	    CTRACE((tfp, "HTML:end_element[%d]: Popped style off stack - %s\n",
			(int) STACKLEVEL(me),
			NONNULL(me->sp->style->name)));
	} else {
	    CTRACE((tfp,
  "Stack underflow error!  Tried to pop off more styles than exist in stack\n"));
	}
    }
    if (BreakFlag == TRUE) {
#ifdef EXP_JUSTIFY_ELTS
	    if (reached_awaited_stacked_elt)
		wait_for_this_stacked_elt=-1;
#endif
	return HT_OK; /* let's pretend... */
    }

    /*
     *	Check for unclosed TEXTAREA. - FM
     */
    if (me->inTEXTAREA && element_number != HTML_TEXTAREA) {
	if (LYBadHTML(me))
	    CTRACE((tfp, "Bad HTML: Missing TEXTAREA end tag\n"));
    }

    if (!me->text && !LYMapsOnly) {
	UPDATE_STYLE;
    }

    /*
     *	Handle the end tag. - FM
     */
    switch(element_number) {

    case HTML_HTML:
	if (me->inA || me->inSELECT || me->inTEXTAREA) {
	    if (LYBadHTML(me))
		CTRACE((tfp,
			"Bad HTML: %s%s%s%s%s not closed before HTML end tag *****\n",
			me->inSELECT ? "SELECT" : "",
			(me->inSELECT && me->inTEXTAREA) ? ", " : "",
			me->inTEXTAREA ? "TEXTAREA" : "",
			((me->inSELECT || me->inTEXTAREA) && me->inA) ? ", " : "",
			me->inA ? "A" : ""));
	}
	break;

    case HTML_HEAD:
	if (me->inBASE &&
	    (LYIsUIPage3(me->node_anchor->address, UIP_LIST_PAGE, 0) ||
	     LYIsUIPage3(me->node_anchor->address, UIP_ADDRLIST_PAGE, 0))) {
	    /*	If we are parsing the List Page, and have a BASE after
	     *	we are done with the HEAD element, propagate it back
	     *	to the node_anchor object.  The base should have been
	     *	inserted by showlist() to record what document the List
	     *	Page is about, and other functions may later look for it
	     *	in the anchor. - kw
	     */
	    StrAllocCopy(me->node_anchor->content_base, me->base_href);
	}
	if (HText_hasToolbar(me->text))
	    HText_appendParagraph(me->text);
	break;

    case HTML_TITLE:
	HTChunkTerminate(&me->title);
	HTAnchor_setTitle(me->node_anchor, me->title.data);
	HTChunkClear(&me->title);
	/*
	 *  Check if it's a bookmark file, and if so, and multiple
	 *  bookmark support is on, or it's off but this isn't the
	 *  default bookmark file (e.g., because it was on before,
	 *  and this is another bookmark file that has been retrieved
	 *  as a previous document), insert the current description
	 *  string and filepath for it.  We pass the strings back to
	 *  the SGML parser so that any 8 bit or multibyte/CJK
	 *  characters will be handled by the parser's state and
	 *  charset routines. - FM
	 */
	if (me->node_anchor->bookmark && *me->node_anchor->bookmark) {
	    if ((LYMultiBookmarks == TRUE) ||
		((bookmark_page && *bookmark_page) &&
		 strcmp(me->node_anchor->bookmark, bookmark_page))) {
		if (!include)
		    include = &me->xinclude;
		for (i = 0; i <= MBM_V_MAXFILES; i++) {
		    if (MBM_A_subbookmark[i] &&
			!strcmp(MBM_A_subbookmark[i],
				me->node_anchor->bookmark)) {
			StrAllocCat(*include, "<H2><EM>");
			StrAllocCat(*include, gettext("Description:"));
			StrAllocCat(*include, "</EM> ");
			StrAllocCopy(temp,
				     ((MBM_A_subdescript[i] &&
				       *MBM_A_subdescript[i]) ?
					 MBM_A_subdescript[i] : gettext("(none)")));
			LYEntify(&temp, TRUE);
			StrAllocCat(*include, temp);
			StrAllocCat(*include, "<BR><EM>&nbsp;&nbsp;&nbsp;");
			StrAllocCat(*include, gettext("Filepath:"));
			StrAllocCat(*include, "</EM> ");
			StrAllocCopy(temp,
				     ((MBM_A_subbookmark[i] &&
				       *MBM_A_subbookmark[i]) ?
					 MBM_A_subbookmark[i] : gettext("(unknown)")));
			LYEntify(&temp, TRUE);
			StrAllocCat(*include, temp);
			FREE(temp);
			StrAllocCat(*include, "</H2>");
			break;
		    }
		}
	    }
	}
	break;

    case HTML_STYLE:
	/*
	 *  We're getting it as Literal text, which, for now,
	 *  we'll just ignore. - FM
	 */
	HTChunkTerminate(&me->style_block);
	CTRACE((tfp, "HTML: STYLE content =\n%s\n",
			    me->style_block.data));
	HTChunkClear(&me->style_block);
	break;

    case HTML_SCRIPT:
	/*
	 *  We're getting it as Literal text, which, for now,
	 *  we'll just ignore. - FM
	 */
	HTChunkTerminate(&me->script);
	CTRACE((tfp, "HTML: SCRIPT content =\n%s\n",
			    me->script.data));
	HTChunkClear(&me->script);
	break;

    case HTML_BODY:
	if (me->inA || me->inSELECT || me->inTEXTAREA) {
	    if (LYBadHTML(me))
		CTRACE((tfp,
			"Bad HTML: %s%s%s%s%s not closed before BODY end tag *****\n",
			me->inSELECT ? "SELECT" : "",
			(me->inSELECT && me->inTEXTAREA) ? ", " : "",
			me->inTEXTAREA ? "TEXTAREA" : "",
			((me->inSELECT || me->inTEXTAREA) && me->inA) ? ", " : "",
			me->inA ? "A" : ""));
	}
	break;

    case HTML_FRAMESET:
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_NOFRAMES:
    case HTML_IFRAME:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_BANNER:
    case HTML_MARQUEE:
    case HTML_BLOCKQUOTE:
    case HTML_BQ:
    case HTML_ADDRESS:
		/*
		 *  Set flag to know that style has ended.
		 *  Fall through.
		i_prior_style = -1;
		 */
	change_paragraph_style(me, me->sp->style);
	UPDATE_STYLE;
	if (me->sp->tag_number == element_number)
	    LYEnsureDoubleSpace(me);
	if (me->List_Nesting_Level >= 0)
	    HText_NegateLineOne(me->text);
	break;

    case HTML_CENTER:
    case HTML_DIV:
	if (me->Division_Level >= 0)
	    me->Division_Level--;
	if (me->Division_Level >= 0) {
	    if (me->sp->style->alignment !=
		me->DivisionAlignments[me->Division_Level]) {
		if (me->inP)
		    LYEnsureSingleSpace(me);
		me->sp->style->alignment =
				me->DivisionAlignments[me->Division_Level];
	    }
	}
	change_paragraph_style(me, me->sp->style);
	if (me->style_change) {
	    actually_set_style(me);
	    if (me->List_Nesting_Level >= 0)
		HText_NegateLineOne(me->text);
	} else if (me->inP)
	    LYEnsureSingleSpace(me);
	me->current_default_alignment = me->sp->style->alignment;
	break;

    case HTML_H1:			/* header styles */
    case HTML_H2:
    case HTML_H3:
    case HTML_H4:
    case HTML_H5:
    case HTML_H6:
	if (me->Division_Level >= 0) {
	    me->sp->style->alignment =
				me->DivisionAlignments[me->Division_Level];
	} else if (!strcmp(me->sp->style->name, "HeadingCenter") ||
		   !strcmp(me->sp->style->name, "Heading1")) {
	    me->sp->style->alignment = HT_CENTER;
	} else if (!strcmp(me->sp->style->name, "HeadingRight")) {
	    me->sp->style->alignment = HT_RIGHT;
	} else {
	    me->sp->style->alignment = HT_LEFT;
	}
	change_paragraph_style(me, me->sp->style);
	UPDATE_STYLE;
	if (styles[element_number]->font & HT_BOLD) {
	    if (me->inBoldA == FALSE && me->inBoldH == TRUE) {
		HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
	    }
	    me->inBoldH = FALSE;
	}
	if (me->List_Nesting_Level >= 0)
	    HText_NegateLineOne(me->text);
	if (me->Underline_Level > 0 && me->inUnderline == FALSE) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    me->inUnderline = TRUE;
	}
	break;

    case HTML_P:
	LYHandlePlike(me,
		 (CONST BOOL*)0, (CONST char **)0,
		 include, 0,
		 FALSE);
	break;

    case HTML_FONT:
	me->inFONT = FALSE;
	break;

    case HTML_B:			/* Physical character highlighting */
    case HTML_BLINK:
    case HTML_I:
    case HTML_U:

    case HTML_CITE:			/* Logical character highlighting */
    case HTML_EM:
    case HTML_STRONG:
	/*
	 *  Ignore any emphasis end tags if the
	 *  Underline_Level is not set. - FM
	 */
	if (me->Underline_Level <= 0)
	    break;

	/*
	 *  Adjust the Underline level counter, and
	 *  turn off underlining if appropriate. - FM
	 */
	me->Underline_Level--;
	if (me->inUnderline && me->Underline_Level < 1) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    me->inUnderline = FALSE;
	    CTRACE((tfp, "Ending underline\n"));
	} else {
	    CTRACE((tfp, "Underline Level is %d\n", me->Underline_Level));
	}
	break;

    case HTML_ABBREV:	/* Miscellaneous character containers */
    case HTML_ACRONYM:
    case HTML_AU:
    case HTML_AUTHOR:
    case HTML_BIG:
    case HTML_CODE:
    case HTML_DFN:
    case HTML_KBD:
    case HTML_SAMP:
    case HTML_SMALL:
    case HTML_SUP:
    case HTML_TT:
    case HTML_VAR:
	break;

    case HTML_SUB:
	HText_appendCharacter(me->text, ']');
	break;

    case HTML_DEL:
    case HTML_S:
    case HTML_STRIKE:
	HTML_put_character(me, ' ');
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, ":DEL]");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	me->in_word = NO;
	break;

    case HTML_INS:
	HTML_put_character(me, ' ');
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	HTML_put_string(me, ":INS]");
	if (me->inUnderline == FALSE)
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	HTML_put_character(me, ' ');
	me->in_word = NO;
	break;

    case HTML_Q:
	if (me->Quote_Level > 0)
	    me->Quote_Level--;
	/*
	 *  Should check LANG and/or DIR attributes, and the
	 *  me->node_anchor->charset and/or yet to be added
	 *  structure elements, to determine whether we should
	 *  use chevrons, but for now we'll always use double-
	 *  or single-quotes. - FM
	 */
	if (!(me->Quote_Level & 1))
	    HTML_put_character(me, '"');
	else
	    HTML_put_character(me, '\'');
	break;

    case HTML_PRE:				/* Formatted text */
	/*
	 *  Set to know that we are no longer in a PRE block.
	 */
	me->inPRE = FALSE;
	/* FALLTHRU */
    case HTML_LISTING:				/* Literal text */
	/* FALLTHRU */
    case HTML_XMP:
	/* FALLTHRU */
    case HTML_PLAINTEXT:
	if (me->comment_start)
	    HText_appendText(me->text, me->comment_start);
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	if (me->List_Nesting_Level >= 0) {
	    UPDATE_STYLE;
	    HText_NegateLineOne(me->text);
	}
	break;

    case HTML_NOTE:
    case HTML_FN:
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	UPDATE_STYLE;
	if (me->sp->tag_number == element_number)
	    LYEnsureDoubleSpace(me);
	if (me->List_Nesting_Level >= 0)
	    HText_NegateLineOne(me->text);
	me->inLABEL = FALSE;
	break;

    case HTML_OL:
	me->OL_Counter[me->List_Nesting_Level < 11 ?
			    me->List_Nesting_Level : 11] = OL_VOID;
	/* FALLTHRU */
    case HTML_DL:
	/* FALLTHRU */
    case HTML_UL:
	/* FALLTHRU */
    case HTML_MENU:
	/* FALLTHRU */
    case HTML_DIR:
	me->List_Nesting_Level--;
	CTRACE((tfp, "HTML_end_element: Reducing List Nesting Level to %d\n",
		    me->List_Nesting_Level));
#ifdef EXP_JUSTIFY_ELTS
	if (element_number == HTML_DL)
	    in_DT = FALSE; /*close the term that was without definition. */
#endif
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	UPDATE_STYLE;
	if (me->List_Nesting_Level >= 0)
	    LYEnsureSingleSpace(me);
	break;

    case HTML_SPAN:
	/*
	 *  Should undo anything we did based on LANG and/or DIR
	 *  attributes, and the me->node_anchor->charset and/or
	 *  yet to be added structure elements. - FM
	 */
	break;

    case HTML_BDO:
	/*
	 *  Should undo anything we did based on DIR (and/or LANG)
	 *  attributes, and the me->node_anchor->charset and/or
	 *  yet to be added structure elements. - FM
	 */
	break;

    case HTML_A:
	/*
	 *  Ignore any spurious A end tags. - FM
	 */
	if (me->inA == FALSE)
	    break;
	/*
	 *  Set to know that we are no longer in an anchor.
	 */
	me->inA = FALSE;
#ifdef MARK_HIDDEN_LINKS
	if (hidden_link_marker && *hidden_link_marker &&
		HText_isAnchorBlank(me->text, me->CurrentANum) ) {
	    HText_appendText(me->text,hidden_link_marker);
	}
#endif
	UPDATE_STYLE;
	if (me->inBoldA == TRUE && me->inBoldH == FALSE)
	    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
	HText_endAnchor(me->text, me->CurrentANum);
	me->CurrentANum = 0;
	me->inBoldA = FALSE;
	if (me->Underline_Level > 0 && me->inUnderline == FALSE) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    me->inUnderline = TRUE;
	}
	break;

    case HTML_MAP:
	FREE(me->map_address);
	break;

    case HTML_BODYTEXT:
	/*
	 *  We may need to look at this someday to deal with
	 *  OBJECTs optimally, but just ignore it for now. - FM
	 */
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_TEXTFLOW:
	/*
	 *  We may need to look at this someday to deal with
	 *  APPLETs optimally, but just ignore it for now. - FM
	 */
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_FIG:
	LYHandleFIG(me, NULL, NULL,
		    0,
		    0,
		    NULL,
		    NULL, NO, FALSE, &intern_flag);
	break;

    case HTML_OBJECT:
	/*
	 *  Finish the data off.
	 */
	{
	    int s = 0, e = 0;
	    char *start = NULL, *first_end = NULL, *last_end = NULL;
	    char *first_map = NULL, *last_map = NULL;
	    BOOL have_param = FALSE;
	    char *data = NULL;

	    HTChunkTerminate(&me->object);
	    data = me->object.data;
	    while ((cp = strchr(data, '<')) != NULL) {
		/*
		 *  Look for nested OBJECTs.  This procedure
		 *  could get tripped up if invalid comments
		 *  are present in the content, or if an OBJECT
		 *  end tag is present in a quoted attribute. - FM
		 */
		if (!strncmp(cp, "<!--", 4)) {
		    data = LYFindEndOfComment(cp);
		    cp = data;
		} else if (s == 0 && !strncasecomp(cp, "<PARAM", 6) &&
		    !IsNmChar(cp[6])) {
		    have_param = TRUE;
		} else if (!strncasecomp(cp, "<OBJECT", 7) &&
		    !IsNmChar(cp[7])) {
		    if (s == 0)
			start = cp;
		    s++;
		} else if (!strncasecomp(cp, "</OBJECT", 8) &&
		    !IsNmChar(cp[8])) {
		    if (e == 0)
			first_end = cp;
		    last_end = cp;
		    e++;
		} else if (!strncasecomp(cp, "<MAP", 4) &&
		    !IsNmChar(cp[4])) {
		    if (!first_map)
			first_map = cp;
		    last_map = cp;
		} else if (!strncasecomp(cp, "</MAP", 5) &&
		    !IsNmChar(cp[5])) {
		    last_map = cp;
		}
		data = ++cp;
	    }
	    if (s < e) {
		/*
		 *  We had more end tags than start tags, so
		 *  we have bad HTML or otherwise misparsed. - FM
		 */
		if (LYBadHTML(me))
		    CTRACE((tfp, "Bad HTML: Unmatched OBJECT start and end tags.  Discarding content:\n%s\n",
			    me->object.data));
		goto End_Object;
	    }
	    if (s > e) {
		if (!me->object_declare && !me->object_name &&
		    !(me->object_shapes && !LYMapsOnly) &&
		    !(me->object_usemap != NULL && !LYMapsOnly) &&
		    !(clickable_images && !LYMapsOnly &&
		      me->object_data != NULL &&
		      !have_param &&
		      me->object_classid == NULL &&
		      me->object_codebase == NULL &&
		      me->object_codetype == NULL)) {
		    /*
		     *  We have nested OBJECT tags, and not yet all of the
		     *  end tags, but have a case where the content needs
		     *  to be parsed again (not dropped) and where we don't
		     *  want to output anything special at the point when we
		     *  *do* have accumulated all the end tags.  So recycle
		     *  the incomplete contents now, and signal the SGML
		     *  parser that it should not regard the current OBJECT
		     *  ended but should treat its contents as mixed.
		     *  Normally these cases would have already handled
		     *  in the real start_element call, so this block may
		     *  not be necessary. - kw
		     */
		    CTRACE((tfp, "%s:\n%s\n",
			   "HTML: Nested OBJECT tags.  Recycling incomplete contents",
			   me->object.data));
		    status = HT_PARSER_OTHER_CONTENT;
		    me->object.size--;
		    HTChunkPuts(&me->object, "</OBJECT>");
		    if (!include)	/* error, should not happen */
			include = &me->xinclude;
		    StrnAllocCat(*include, me->object.data, me->object.size);
		    clear_objectdata(me);
		    /* an internal fake call to keep our stack happy: */
		    HTML_start_element(me, HTML_OBJECT, NULL,NULL,
				       me->tag_charset, include);
		    break;
		}
		/*
		 *  We have nested OBJECT tags, and not yet all of the
		 *  end tags, and we want the end tags.  So restore an
		 *  end tag to the content, and signal to the SGML parser
		 *  that it should resume the accumulation of OBJECT content
		 *  (after calling back to start_element) in a way that
		 *  is equivalent to passing it a dummy start tag. - FM, kw
		 */
		CTRACE((tfp, "HTML: Nested OBJECT tags.  Recycling.\n"));
		status = HT_PARSER_REOPEN_ELT;
		me->object.size--;
		HTChunkPuts(&me->object, "</OBJECT>");
		if (!LYMapsOnly)
		    change_paragraph_style(me, me->sp->style);
		break;
	    }

	    /*
	     *	OBJECT start and end tags are fully matched,
	     *	assuming we weren't tripped up by comments
	     *	or quoted attributes. - FM
	     */
	    CTRACE((tfp, "HTML:OBJECT content:\n%s\n", me->object.data));

	    /*
	     *	OBJECTs with DECLARE should be saved but
	     *	not instantiated, and if nested, can have
	     *	only other DECLAREd OBJECTs.  Until we have
	     *	code to handle these, we'll just create an
	     *	anchor for the ID, if present, and discard
	     *	the content (sigh 8-). - FM
	     */
	    if (me->object_declare == TRUE) {
		if (me->object_id && *me->object_id && !LYMapsOnly)
		    LYHandleID(me, me->object_id);
		CTRACE((tfp, "HTML: DECLAREd OBJECT.  Ignoring!\n"));
		goto End_Object;
	    }

	    /*
	     *	OBJECTs with NAME are for FORM submissions.
	     *	We'll just create an anchor for the ID, if
	     *	present, and discard the content until we
	     *	have code to handle these. (sigh 8-). - FM
	     */
	    if (me->object_name != NULL && !LYMapsOnly) {
		if (me->object_id && *me->object_id)
		    LYHandleID(me, me->object_id);
		CTRACE((tfp, "HTML: NAMEd OBJECT.  Ignoring!\n"));
		goto End_Object;
	    }

	    /*
	     *	Deal with any nested OBJECTs by descending
	     *	to the inner-most OBJECT. - FM
	     */
	    if (s > 0) {
		if (start != NULL &&
		    first_end != NULL && first_end > start) {
		    /*
		     *	Minumum requirements for the ad hoc parsing
		     *	to have succeeded are met.  We'll hope that
		     *	it did succeed. - FM
		     */
		    if (LYMapsOnly) {
			/*
			 *  Well we don't need to do this any more,
			 *  nested objects should either not get here
			 *  any more at all or can be handled fine by
			 *  other code below.  Leave in place for now
			 *  as a special case for LYMapsOnly. - kw
			 */
			if (LYMapsOnly && (!last_map || last_map < first_end))
			    *first_end = '\0';
			else
			    e = 0;
			data = NULL;
			if (LYMapsOnly && (!first_map || first_map > start))
			    StrAllocCopy(data, start);
			else
			    StrAllocCopy(data, me->object.data);
			if (e > 0) {
			    for (i = e; i > 0; i--) {
				StrAllocCat(data, "</OBJECT>");
			    }
			}
			if (!include)	/* error, should not happen */
			    include = &me->xinclude;
			StrAllocCat(*include, data);
			CTRACE((tfp, "HTML: Recycling nested OBJECT%s.\n",
			       (s > 1) ? "s" : ""));
			FREE(data);
			goto End_Object;
		    }
		} else {
		    if (LYBadHTML(me))
			CTRACE((tfp, "Bad HTML: Unmatched OBJECT start and end tags.  Discarding content.\n"));
		    goto End_Object;
		}
	    }

	    /*
	     *	If its content has SHAPES, convert it to FIG. - FM
	     *
	     *  This is now handled in our start_element without using
	     *  include if the SGML parser cooperates, so this block
	     *  may be unnecessary. - kw
	     */
	    if (me->object_shapes == TRUE && !LYMapsOnly) {
		CTRACE((tfp, "HTML: OBJECT has SHAPES.  Converting to FIG.\n"));
		if (!include)	/* error, should not happen */
		    include = &me->xinclude;
		StrAllocCat(*include, "<FIG ISOBJECT IMAGEMAP");
		if (me->object_ismap == TRUE)
		    StrAllocCat(*include, " IMAGEMAP");
		if (me->object_id != NULL) {
		    StrAllocCat(*include, " ID=\"");
		    StrAllocCat(*include, me->object_id);
		    StrAllocCat(*include, "\"");
		}
		if (me->object_data != NULL &&
		    me->object_classid == NULL) {
		    StrAllocCat(*include, " SRC=\"");
		    StrAllocCat(*include, me->object_data);
		    StrAllocCat(*include, "\"");
		}
		StrAllocCat(*include, ">");
		me->object.size--;
		HTChunkPuts(&me->object, "</FIG>");
		HTChunkTerminate(&me->object);
		StrAllocCat(*include, me->object.data);
		goto End_Object;
	    }

	    /*
	     *	If it has a USEMAP attribute and didn't have SHAPES,
	     *	convert it to IMG. - FM
	     */
	    if (me->object_usemap != NULL && !LYMapsOnly) {
		CTRACE((tfp, "HTML: OBJECT has USEMAP.  Converting to IMG.\n"));

		if (!include)	/* error, should not happen */
		    include = &me->xinclude;
		StrAllocCat(*include, "<IMG ISOBJECT");
		if (me->object_id != NULL) {
		    /*
		     *	Pass the ID. - FM
		     */
		    StrAllocCat(*include, " ID=\"");
		    StrAllocCat(*include, me->object_id);
		    StrAllocCat(*include, "\"");
		}
		if (me->object_data != NULL &&
		    me->object_classid == NULL) {
		    /*
		     *	We have DATA with no CLASSID, so let's
		     *	hope it' equivalent to an SRC. - FM
		     */
		    StrAllocCat(*include, " SRC=\"");
		    StrAllocCat(*include, me->object_data);
		    StrAllocCat(*include, "\"");
		}
		if (me->object_title != NULL) {
		    /*
		     *	Use the TITLE for both the MAP
		     *	and the IMGs ALT. - FM
		     */
		    StrAllocCat(*include, " TITLE=\"");
		    StrAllocCat(*include, me->object_title);
		    StrAllocCat(*include, "\" ALT=\"");
		    StrAllocCat(*include, me->object_title);
		    StrAllocCat(*include, "\"");
		}
		/*
		 *  Add the USEMAP, and an ISMAP if present. - FM
		 */
		if (me->object_usemap != NULL) {
		    StrAllocCat(*include, " USEMAP=\"");
		    StrAllocCat(*include, me->object_usemap);
		    if (me->object_ismap == TRUE)
			StrAllocCat(*include, "\" ISMAP>");
		    else
			StrAllocCat(*include, "\">");
		} else {
		    StrAllocCat(*include, ">");
		}
		/*
		 *  Add the content if it has <MAP, since that may
		 *  be the MAP this usemap points to.  But if we have
		 *  nested objects, try to eliminate portions that
		 *  cannot contribute to the quest for MAP.  This is
		 *  not perfect, we may get too much content; this seems
		 *  preferable over losing too much. - kw
		 */
		if (first_map) {
		    if (s == 0) {
			StrAllocCat(*include, me->object.data);
			CTRACE((tfp, "HTML: MAP found, recycling object contents.\n"));
			goto End_Object;
		    }
		    /* s > 0 and s == e */
		    data = NULL;
		    if (last_map < start) {
			*start = '\0';
			i = 0;
		    } else if (last_map < first_end) {
			*first_end = '\0';
			i = e;
		    } else if (last_map < last_end) {
			*last_end = '\0';
			i = 1;
		    } else {
			i = 0;
		    }
		    if (first_map > last_end) {
			/* fake empty object to keep stacks stack happy */
			StrAllocCopy(data, "<OBJECT><");
			StrAllocCat(data, last_end + 1);
			i = 0;
		    } else if (first_map > start) {
			StrAllocCopy(data, start);
		    } else {
			StrAllocCopy(data, me->object.data);
		    }
		    for (; i > 0; i--) {
			StrAllocCat(data, "</OBJECT>");
		    }
		    CTRACE((tfp, "%s:\n%s\n",
			   "HTML: MAP and nested OBJECT tags.  Recycling parts",
			   data));
		    StrAllocCat(*include, data);
		    FREE(data);
		}
		goto End_Object;
	    }

	    /*
	     *	Add an ID link if needed. - FM
	     */
	    if (me->object_id && *me->object_id && !LYMapsOnly)
		LYHandleID(me, me->object_id);

	    /*
	     *	Add the OBJECTs content if not empty. - FM
	     */
	    if (me->object.size > 1) {
		if (!include)	/* error, should not happen */
		    include = &me->xinclude;
		StrAllocCat(*include, me->object.data);
	    }

	    /*
	     *	Create a link to the DATA, if desired, and
	     *	we can rule out that it involves scripting
	     *	code.  This a risky thing to do, but we can
	     *	toggle clickable_images mode off if it really
	     *	screws things up, and so we may as well give
	     *	it a try. - FM
	     */
	    if (clickable_images) {
		if (!LYMapsOnly &&
		    me->object_data != NULL &&
		    !have_param &&
		    me->object_classid == NULL &&
		    me->object_codebase == NULL &&
		    me->object_codetype == NULL) {
		    /*
		     *	We have a DATA value and no need for scripting
		     *	code, so close the current Anchor, if one is
		     *	open, and add an Anchor for this source.  If
		     *	we also have a TYPE value, check whether it's
		     *	an image or not, and set the link name
		     *	accordingly. - FM
		     */
		    if (!include)	/* error, should not happen */
			include = &me->xinclude;
		    if (me->inA)
			StrAllocCat(*include, "</A>");
		    StrAllocCat(*include, " -<A HREF=\"");
		    StrAllocCat(*include, me->object_data);
		    StrAllocCat(*include, "\">");
		    if ((me->object_type != NULL) &&
			!strncasecomp(me->object_type, "image/", 6)) {
			StrAllocCat(*include, "(IMAGE)");
		    } else {
			StrAllocCat(*include, "(OBJECT)");
		    }
		    StrAllocCat(*include, "</A> ");
		}
	    }
	}

	/*
	 *  Re-intialize all of the OBJECT elements. - FM
	 */
End_Object:
	clear_objectdata(me);

	if (!LYMapsOnly)
	    change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_APPLET:
	if (me->inAPPLETwithP) {
	    LYEnsureDoubleSpace(me);
	} else {
	    HTML_put_character(me, ' ');  /* space char may be ignored */
	}
	LYResetParagraphAlignment(me);
	me->inAPPLETwithP = FALSE;
	me->inAPPLET = FALSE;
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_CAPTION:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	me->inCAPTION = FALSE;
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	me->inLABEL = FALSE;
	break;

    case HTML_CREDIT:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	me->inCREDIT = FALSE;
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	me->inLABEL = FALSE;
	break;

    case HTML_FORM:
	/*
	 *  Check if we had a FORM start tag, and issue a
	 *  message if not, but fall through to check for
	 *  an open SELECT and ensure that the FORM-related
	 *  globals in GridText.c are initialized. - FM
	 */
	if (!me->inFORM) {
	    if (LYBadHTML(me))
		CTRACE((tfp, "Bad HTML: Unmatched FORM end tag\n"));
	}
	EMIT_IFDEF_EXP_JUSTIFY_ELTS(form_in_htext = FALSE);

	/*
	 *  Check if we still have a SELECT element open.
	 *  FORM may have been declared SGML_EMPTY in HTMLDTD.c,
	 *  and in that case SGML_character() in SGML.c is
	 *  not able to ensure correct nesting; or it may have
	 *  failed to enforce valid nesting.  If a SELECT is open,
	 *  issue a message, then
	 *  call HTML_end_element() directly (with a
	 *  check in that to bypass decrementing of the HTML
	 *  parser's stack) to close the SELECT. - kw
	 */
	if (me->inSELECT) {
	    if (LYBadHTML(me))
		CTRACE((tfp, "Bad HTML: Open SELECT at FORM end. Faking SELECT end tag. *****\n"));
	    if (me->sp->tag_number != HTML_SELECT) {
		SET_SKIP_STACK(HTML_SELECT);
	    }
	    HTML_end_element(me, HTML_SELECT, include);
	}

	/*
	 *  Set to know that we are no longer in an form.
	 */
	me->inFORM = FALSE;

	HText_endForm(me->text);
	/*
	 *  If we are in a list and are on the first line
	 *  with no text following a bullet or number,
	 *  don't force a newline.  This could happen if
	 *  we were called from HTML_start_element() due
	 *  to a missing FORM end tag. - FM
	 */
	if (!(me->List_Nesting_Level >= 0 && !me->inP))
	    LYEnsureSingleSpace(me);
	break;

    case HTML_FIELDSET:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_LEGEND:
	LYEnsureDoubleSpace(me);
	LYResetParagraphAlignment(me);
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    case HTML_LABEL:
	break;

    case HTML_BUTTON:
	break;

    case HTML_TEXTAREA:
	{
	    InputFieldData I;
	    int chars;
	    char *data;

	    /*
	     *	Make sure we had a textarea start tag.
	     */
	    if (!me->inTEXTAREA) {
		if (LYBadHTML(me))
		    CTRACE((tfp, "Bad HTML: Unmatched TEXTAREA end tag\n"));
		break;
	    }

	    /*
	     *	Set to know that we are no longer in a textarea tag.
	     */
	    me->inTEXTAREA = FALSE;

	    /*
	     *	Initialize.
	     */
	    I.align=NULL; I.accept=NULL; I.checked=NO; I.class=NULL;
	    I.disabled=NO; I.error=NULL; I.height= NULL; I.id=NULL;
	    I.lang=NULL; I.max=NULL; I.maxlength=NULL; I.md=NULL;
	    I.min=NULL; I.name=NULL; I.size=NULL; I.src=NULL;
	    I.type=NULL; I.value=NULL; I.width=NULL;
	    I.value_cs = current_char_set;

	    UPDATE_STYLE;
	    /*
	     *	Before any input field add a space if necessary.
	     */
	    HTML_put_character(me, ' ');
	    me->in_word = NO;
	    /*
	     *	Add a return.
	     */
	    HText_appendCharacter(me->text, '\r');

	    /*
	     *	Finish the data off.
	     */
	    HTChunkTerminate(&me->textarea);
	    FREE(temp);

	    I.type = "textarea";
	    I.size = me->textarea_cols;
	    I.name = me->textarea_name;
	    I.name_cs = me->textarea_name_cs;
	    I.accept_cs = me->textarea_accept_cs;
	    me->textarea_accept_cs = NULL;
	    I.disabled = me->textarea_disabled;
	    I.id = me->textarea_id;

	    /*
	     *	Transform the TEXTAREA content as needed, then parse
	     *	it into individual lines to be handled as a series
	     *  series of INPUT fields (ugh!).
	     *	Any raw 8-bit or multibyte characters already have been
	     *	handled in relation to the display character set
	     *	in SGML_character().
	     *
	     *  If TEXTAREA is handled as SGML_LITTERAL (the old way),
	     *	we need to SGML-unescape any character references and NCRs
	     *  here.  Otherwise this will already have happened in the
	     *  SGML.c parsing. - kw
	     */
	    me->UsePlainSpace = TRUE;

	    if (HTML_dtd.tags[element_number].contents == SGML_LITTERAL) {
		TRANSLATE_AND_UNESCAPE_ENTITIES6(&me->textarea.data,
						 me->UCLYhndl,
						 current_char_set,
						 NO,
						 me->UsePlainSpace, me->HiddenValue);
	    } else {
		/*
		 *  This shouldn't have anything to do, normally, but
		 *  just in case...
		 *  There shouldn't be lynx special character codes in
		 *  the chunk ("DTD" flag Tgf_nolyspcl tells SGML.c not
		 *  to generate them).  If there were, we could set the
		 *  last parameter ('Back') below to YES, which would
		 *  take them out of the data.
		 *  The data may however contain non break space, soft
		 *  hyphen, or en space etc., in the me->UCLYhndl character
		 *  encoding.  If that's a problem, perhaps for the (line
		 *  or other) editor, setting 'Back' to YES should also
		 *  help to always convert them to plain spaces (or drop
		 *  them). - kw
		 */
		TRANSLATE_HTML7(&me->textarea.data,
						    me->UCLYhndl,
						    current_char_set,
						    NO,
						    me->UsePlainSpace, me->HiddenValue,
						    NO);
	    }
	    data = me->textarea.data;

	    /*
	     *	Trim any trailing newlines and
	     *	skip any lead newlines. - FM
	     */
	    if (*data != '\0') {
		cp = (data + strlen(data)) - 1;
		while (cp >= data && *cp == '\n') {
		    *cp-- = '\0';
		}
		while (*data == '\n') {
		    data++;
		}
	    }
	    /*
	     *	Load the first text line, or set
	     *	up for all blank rows. - FM
	     */
	    if ((cp = strchr(data, '\n')) != NULL) {
		*cp = '\0';
		StrAllocCopy(temp, data);
		*cp = '\n';
		data = (cp + 1);
	    } else {
		if (*data != '\0') {
		    StrAllocCopy(temp, data);
		} else {
		    FREE(temp);
		}
		data = "";
	    }
	    /*
	     *	Display at least the requested number
	     *	of text lines and/or blank rows. - FM
	     */
	    for (i = 0; i < me->textarea_rows; i++) {
		int j;
		for (j = 0; temp && temp[j]; j++) {
		    if (temp[j] == '\r')
			temp[j] = (char) (temp[j+1] ? ' ' : '\0');
		}
		I.value = temp;
		chars = HText_beginInput(me->text, me->inUnderline, &I);
		for (; chars > 0; chars--)
		    HTML_put_character(me, '_');
		HText_appendCharacter(me->text, '\r');
		if (*data != '\0') {
		    if (*data == '\n') {
			FREE(temp);
			data++;
		    } else if ((cp = strchr(data, '\n')) != NULL) {
			*cp = '\0';
			StrAllocCopy(temp, data);
			*cp = '\n';
			data = (cp + 1);
		    } else {
			StrAllocCopy(temp, data);
			data = "";
		    }
		} else {
		    FREE(temp);
		}
	    }
	    /*
	     *	Check for more data lines than the rows attribute.
	     *	We add them to the display, because we support only
	     *	horizontal and not also vertical scrolling. - FM
	     */
	    while (*data != '\0' || temp != NULL) {
		int j;
		for (j = 0; temp && temp[j]; j++) {
		    if (temp[j] == '\r')
			temp[j] = (char) (temp[j+1] ? ' ' : '\0');
		}
		I.value = temp;
		chars = HText_beginInput(me->text, me->inUnderline, &I);
		for (chars = atoi(me->textarea_cols); chars > 0; chars--)
		    HTML_put_character(me, '_');
		HText_appendCharacter(me->text, '\r');
		if (*data == '\n') {
		    FREE(temp);
		    data++;
		} else if ((cp = strchr(data, '\n')) != NULL) {
		    *cp = '\0';
		    StrAllocCopy(temp, data);
		    *cp = '\n';
		    data = (cp + 1);
		} else if (*data != '\0') {
		    StrAllocCopy(temp, data);
		    data = "";
		} else {
		    FREE(temp);
		}
	    }
	    FREE(temp);
	    cp = NULL;
	    me->UsePlainSpace = FALSE;

	    HTChunkClear(&me->textarea);
	    FREE(me->textarea_name);
	    me->textarea_name_cs = -1;
	    FREE(me->textarea_cols);
	    FREE(me->textarea_id);
	    break;
	}

    case HTML_SELECT:
	{
	    char *ptr;

	    /*
	     *	Make sure we had a select start tag.
	     */
	    if (!me->inSELECT) {
		if (LYBadHTML(me))
		    CTRACE((tfp, "Bad HTML: Unmatched SELECT end tag *****\n"));
		break;
	    }

	    /*
	     *	Set to know that we are no longer in a select tag.
	     */
	    me->inSELECT = FALSE;

	    /*
	     *	Clear the disable attribute.
	     */
	    me->select_disabled = FALSE;

	    /*
	     *	Make sure we're in a form.
	     */
	    if (!me->inFORM) {
		if (LYBadHTML(me))
		    CTRACE((tfp,
			    "Bad HTML: SELECT end tag not within FORM element *****\n"));
		/*
		 *  Hopefully won't crash, so we'll ignore it. - kw
		 */
	    }

	    /*
	     *	Finish the data off.
	     */
	    HTChunkTerminate(&me->option);
	    /*
	     *	Finish the previous option.
	     */
	    ptr = HText_setLastOptionValue(me->text,
					   me->option.data,
					   me->LastOptionValue,
					   LAST_ORDER,
					   me->LastOptionChecked,
					   me->UCLYhndl,
					   ATTR_CS_IN);
	    FREE(me->LastOptionValue);

	    me->LastOptionChecked = FALSE;

	    if (HTCurSelectGroupType == F_CHECKBOX_TYPE ||
		LYSelectPopups == FALSE) {
		    /*
		     *	Start a newline after the last checkbox/button option.
		     */
		    LYEnsureSingleSpace(me);
	    } else {
		/*
		 *  Output popup box with the default option to screen,
		 *  but use non-breaking spaces for output.
		 */
		if (ptr &&
		    (me->sp[0].tag_number == HTML_PRE || me->inPRE == TRUE ||
		     !me->sp->style->freeFormat) &&
		    strlen(ptr) > 6) {
		    /*
		     *	The code inadequately handles OPTION fields in PRE tags.
		     *	We'll put up a minimum of 6 characters, and if any
		     *	more would exceed the wrap column, we'll ignore them.
		     */
		    for (i = 0; i < 6; i++) {
			if (*ptr == ' ')
			    HText_appendCharacter(me->text,HT_NON_BREAK_SPACE);
			else
			    HText_appendCharacter(me->text,*ptr);
			ptr++;
		    }
		    HText_setIgnoreExcess(me->text, TRUE);
		}
		for (; ptr && *ptr != '\0'; ptr++) {
		    if (*ptr == ' ')
			HText_appendCharacter(me->text,HT_NON_BREAK_SPACE);
		    else {
			HTkcode kcode = 0;
			HTkcode specified_kcode = 0;
			if (HTCJK == JAPANESE) {
			    kcode = HText_getKcode(me->text);
			    HText_updateKcode(me->text, kanji_code);
			    specified_kcode = HText_getSpecifiedKcode(me->text);
			    HText_updateSpecifiedKcode(me->text, kanji_code);
			}
			HText_appendCharacter(me->text,*ptr);
			if (HTCJK == JAPANESE) {
			    HText_updateKcode(me->text, kcode);
			    HText_updateSpecifiedKcode(me->text, specified_kcode);
			}
		    }
		}
		/*
		 *  Add end option character.
		 */
		if (!me->first_option) {
		    HText_appendCharacter(me->text, ']');
		    HText_setLastChar(me->text, ']');
		    me->in_word = YES;
		}
		HText_setIgnoreExcess(me->text, FALSE);
	    }
	    HTChunkClear(&me->option);

	    if (me->Underline_Level > 0 && me->inUnderline == FALSE) {
		HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
		me->inUnderline = TRUE;
	    }
	    if (me->needBoldH == TRUE && me->inBoldH == FALSE) {
		HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		me->inBoldH = TRUE;
		me->needBoldH = FALSE;
	    }
	}
	break;

    case HTML_TABLE:
	me->inTABLE = FALSE;
	if (!strcmp(me->sp->style->name, "Preformatted")) {
	    break;
	}
	if (me->Division_Level >= 0)
	    me->Division_Level--;
	if (me->Division_Level >= 0)
	    me->sp->style->alignment =
				me->DivisionAlignments[me->Division_Level];
	change_paragraph_style(me, me->sp->style);
	UPDATE_STYLE;
	HText_endStblTABLE(me->text);
	me->current_default_alignment = me->sp->style->alignment;
	if (me->List_Nesting_Level >= 0)
	    HText_NegateLineOne(me->text);
	break;

/* These TABLE related elements may now not be SGML_EMPTY. - kw */
    case HTML_TR:
	HText_endStblTR(me->text);
	if (HText_LastLineSize(me->text, FALSE)) {
	    HText_setLastChar(me->text, ' ');  /* absorb next white space */
	    HText_appendCharacter(me->text, '\r');
	}
	me->in_word = NO;
	break;

    case HTML_THEAD:
    case HTML_TFOOT:
    case HTML_TBODY:
	break;

    case HTML_COLGROUP:
	if (me->inTABLE)
	    HText_endStblCOLGROUP(me->text);
	break;

    case HTML_TH:
    case HTML_TD:
	HText_endStblTD(me->text);
	break;

/* More stuff that may now not be SGML_EMPTY any more: */
    case HTML_DT:
    case HTML_DD:
    case HTML_LH:
    case HTML_LI:
    case HTML_OVERLAY:
	break;

    case HTML_MATH:
	/*
	 *  We're getting it as Literal text, which, until we can process
	 *  it, we'll display as is, within brackets to alert the user. - FM
	 */
	HTChunkPutc(&me->math, ' ');
	HTChunkTerminate(&me->math);
	if (me->math.size > 2) {
	    LYEnsureSingleSpace(me);
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    HTML_put_string(me, "[MATH:");
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    HTML_put_character(me, ' ');
	    HTML_put_string(me, me->math.data);
	    HText_appendCharacter(me->text, LY_UNDERLINE_START_CHAR);
	    HTML_put_string(me, ":MATH]");
	    if (me->inUnderline == FALSE)
		HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    LYEnsureSingleSpace(me);
	}
	HTChunkClear(&me->math);
	break;

    default:
	change_paragraph_style(me, me->sp->style);  /* Often won't really change */
	break;

    } /* switch */

#ifdef EXP_JUSTIFY_ELTS
	    if (reached_awaited_stacked_elt)
		wait_for_this_stacked_elt=-1;
#endif

    if (me->xinclude) {
	HText_appendText(me->text, " *** LYNX ERROR ***\rUnparsed data:\r");
	HText_appendText(me->text, me->xinclude);
	FREE(me->xinclude);
    }

#ifdef USE_COLOR_STYLE
    if (!skip_stack_requested) { /*don't emit stylechanges if skipped stack element - VH*/
#if !OPT_SCN
	TrimColorClass(HTML_dtd.tags[element_number].name,
		       Style_className, &hcode);
#else
# if !OMIT_SCN_KEEPING
	FastTrimColorClass(HTML_dtd.tags[element_number].name,
			   HTML_dtd.tags[element_number].name_len,
			   Style_className,
			   &Style_className_end, &hcode);
#  endif
#endif

	if (!ReallyEmptyTagNum(element_number))
	{
	    CTRACE((tfp, "STYLE.end_element: ending non-\"EMPTY\" style <%s...>\n",
		    HTML_dtd.tags[element_number].name));
#if !defined(USE_HASH)
	    HText_characterStyle(me->text, element_number+STARTAT, STACK_OFF);
#else
	    HText_characterStyle(me->text, HCODE_TO_STACK_OFF(hcode), STACK_OFF);
#endif /* USE_HASH */
	}
    }
#endif /* USE_COLOR_STYLE */
    return status;
}

/*		Expanding entities
**		------------------
*/
/*	(In fact, they all shrink!)
*/
PUBLIC int HTML_put_entity ARGS2(HTStructured *, me, int, entity_number)
{
    int nent = HTML_dtd.number_of_entities;

    if (entity_number < nent) {
	HTML_put_string(me, p_entity_values[entity_number]);
	return HT_OK;
    }
    return HT_CANNOT_TRANSLATE;
}

/*	Free an HTML object
**	-------------------
**
**	If the document is empty, the text object will not yet exist.
**	So we could in fact abandon creating the document and return
**	an error code.	In fact an empty document is an important type
**	of document, so we don't.
**
**	If non-interactive, everything is freed off.   No: crashes -listrefs
**	Otherwise, the interactive object is left.
*/
PRIVATE void HTML_free ARGS1(HTStructured *, me)
{
    char *include = NULL;

    if (LYMapsOnly && !me->text) {
	/*
	 *  We only handled MAP, AREA and BASE tags, and didn't
	 *  create an HText structure for the document nor want
	 *  one now, so just make sure we free anything that might
	 *  have been allocated. - FM
	 */
	FREE(me->base_href);
	FREE(me->map_address);
	clear_objectdata(me);
	FREE(me->xinclude);
	FREE(me);
	return;
    }

    UPDATE_STYLE;		/* Creates empty document here! */
    if (me->comment_end)
	HTML_put_string(me, me->comment_end);
    if (me->text) {
	/*
	 *  Emphasis containers, A, FONT, and FORM may be declared
	 *  SGML_EMPTY in HTMLDTD.c, and SGML_character() in SGML.c
	 *  may check for their end tags to call HTML_end_element()
	 *  directly (with a check in that to bypass decrementing
	 *  of the HTML parser's stack).  So if we still have the
	 *  emphasis (Underline) on, or any open A, FONT, or FORM
	 *  containers, turn it off or close them now. - FM & kw
	 *
	 *  IF those tags are not declared SGML_EMPTY, but we let
	 *  the SGML.c parser take care of correctly stacked ordering,
	 *  and of correct wind-down on end-of-stream (in SGML_free
	 *  SGML_abort),
	 *  THEN these and other checks here in HTML.c should not be
	 *  necessary.	Still it can't hurt to include them. - kw
	 */
	if (me->inUnderline) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    me->inUnderline = FALSE;
	    me->Underline_Level = 0;
	    CTRACE((tfp, "HTML_free: Ending underline\n"));
	}
	if (me->inA) {
	    HTML_end_element(me, HTML_A, &include);
	    me->inA = FALSE;
	    CTRACE((tfp, "HTML_free: Ending HTML_A\n"));
	}
	if (me->inFONT) {
	    HTML_end_element(me, HTML_FONT, &include);
	    me->inFONT = FALSE;
	}
	if (me->inFORM) {
	    HTML_end_element(me, HTML_FORM, &include);
	    me->inFORM = FALSE;
	}
	if (me->option.size > 0) {
	    /*
	     *	If we still have data in the me->option chunk after
	     *	forcing a close of a still-open form, something must
	     *	have gone very wrong. - kw
	     */
	    if (LYBadHTML(me))
		CTRACE((tfp,
			"Bad HTML: SELECT or OPTION not ended properly *****\n"));
	    HTChunkTerminate(&me->option);
	    /*
	     *	Output the left-over data as text, maybe it was invalid
	     *	markup meant to be shown somewhere. - kw
	     */
	    CTRACE((tfp, "HTML_free: ***** leftover option data: %s\n",
			me->option.data));
	    HTML_put_string(me, me->option.data);
	    HTChunkClear(&me->option);
	}
	if (me->textarea.size > 0) {
	    /*
	     *	If we still have data in the me->textarea chunk after
	     *	forcing a close of a still-open form, something must
	     *	have gone very wrong. - kw
	     */
	    if (LYBadHTML(me))
		CTRACE((tfp,
			"Bad HTML: TEXTAREA not used properly *****\n"));
	    HTChunkTerminate(&me->textarea);
	    /*
	     *	Output the left-over data as text, maybe it was invalid
	     *	markup meant to be shown somewhere. - kw
	     */
	    CTRACE((tfp, "HTML_free: ***** leftover textarea data: %s\n",
			me->textarea.data));
	    HTML_put_string(me, me->textarea.data);
	    HTChunkClear(&me->textarea);
	}
	/*
	 *  If we're interactive and have hidden links but no visible
	 *  links, add a message informing the user about this and
	 *  suggesting use of the 'l'ist command. - FM
	 */
	if (!dump_output_immediately &&
	    HText_sourceAnchors(me->text) < 1 &&
	    HText_HiddenLinkCount(me->text) > 0) {
	    HTML_start_element(me, HTML_P, 0, 0, -1, &include);
	    HTML_put_character(me, '[');
	    HTML_start_element(me, HTML_EM, 0, 0, -1, &include);
	    HTML_put_string(me,
		gettext("Document has only hidden links.  Use the 'l'ist command."));
	    HTML_end_element(me, HTML_EM, &include);
	    HTML_put_character(me, ']');
	    HTML_end_element(me, HTML_P, &include);
	}
	if (me->xinclude) {
	    HText_appendText(me->text, " *** LYNX ERROR ***\rUnparsed data:\r");
	    HText_appendText(me->text, me->xinclude);
	    FREE(me->xinclude);
	}

	/*
	 *  Now call the cleanup function. - FM
	 */
	HText_endAppend(me->text);
    }
    if (me->option.size > 0) {
	/*
	 *  If we still have data in the me->option chunk after
	 *  forcing a close of a still-open form, something must
	 *  have gone very wrong. - kw
	 */
	if (LYBadHTML(me))
	    CTRACE((tfp,
		    "Bad HTML: SELECT or OPTION not ended properly *****\n"));
	if (TRACE) {
	    HTChunkTerminate(&me->option);
	    fprintf(tfp, "HTML_free: ***** leftover option data: %s\n",
		    me->option.data);
	}
	HTChunkClear(&me->option);
    }
    if (me->textarea.size > 0) {
	/*
	 *  If we still have data in the me->textarea chunk after
	 *  forcing a close of a still-open form, something must
	 *  have gone very wrong. - kw
	 */
	if (LYBadHTML(me))
	    CTRACE((tfp,
		    "Bad HTML: TEXTAREA not used properly *****\n"));
	if (TRACE) {
	    HTChunkTerminate(&me->textarea);
	    fprintf(tfp, "HTML_free: ***** leftover textarea data: %s\n",
		    me->textarea.data);
	}
	HTChunkClear(&me->textarea);
    }

    if (me->target) {
	(*me->targetClass._free)(me->target);
    }
    if (me->sp && me->sp->style && me->sp->style->name) {
	if (!strcmp(me->sp->style->name, "DivCenter") ||
	    !strcmp(me->sp->style->name, "HeadingCenter") ||
	    !strcmp(me->sp->style->name, "Heading1")) {
	    me->sp->style->alignment = HT_CENTER;
	} else if (!strcmp(me->sp->style->name, "DivRight") ||
		   !strcmp(me->sp->style->name, "HeadingRight")) {
	    me->sp->style->alignment = HT_RIGHT;
	} else	{
	    me->sp->style->alignment = HT_LEFT;
	}
	styles[HTML_PRE]->alignment = HT_LEFT;
    }
#ifdef USE_COLOR_STYLE
# if !OPT_SCN
    FREE(Style_className);
# endif
#endif
    FREE(me->base_href);
    FREE(me->map_address);
    FREE(me->LastOptionValue);
    clear_objectdata(me);
    FREE(me);
}

PRIVATE void HTML_abort ARGS2(HTStructured *, me, HTError, e)
{
    char *include = NULL;

    if (me->text) {
	/*
	 *  If we have emphasis on, or open A, FONT, or FORM
	 *  containers, turn it off or close them now. - FM
	 */
	if (me->inUnderline) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    me->inUnderline = FALSE;
	    me->Underline_Level = 0;
	}
	if (me->inA) {
	    HTML_end_element(me, HTML_A, &include);
	    me->inA = FALSE;
	}
	if (me->inFONT) {
	    HTML_end_element(me, HTML_FONT, &include);
	    me->inFONT = FALSE;
	}
	if (me->inFORM) {
	    HTML_end_element(me, HTML_FORM, &include);
	    me->inFORM = FALSE;
	}

	/*
	 *  Now call the cleanup function. - FM
	 */
	HText_endAppend(me->text);
    }

    if (me->option.size > 0) {
	/*
	 *  If we still have data in the me->option chunk after
	 *  forcing a close of a still-open form, something must
	 *  have gone very wrong. - kw
	 */
	if (TRACE) {
	    fprintf(tfp,
		    "HTML_abort: SELECT or OPTION not ended properly *****\n");
	    HTChunkTerminate(&me->option);
	    fprintf(tfp, "HTML_abort: ***** leftover option data: %s\n",
		    me->option.data);
	}
	HTChunkClear(&me->option);
    }
    if (me->textarea.size > 0) {
	/*
	 *  If we still have data in the me->textarea chunk after
	 *  forcing a close of a still-open form, something must
	 *  have gone very wrong. - kw
	 */
	if (TRACE) {
	    fprintf(tfp,
		    "HTML_abort: TEXTAREA not used properly *****\n");
	    HTChunkTerminate(&me->textarea);
	    fprintf(tfp, "HTML_abort: ***** leftover textarea data: %s\n",
		    me->textarea.data);
	}
	HTChunkClear(&me->textarea);
    }

    if (me->target) {
	(*me->targetClass._abort)(me->target, e);
    }
    if (me->sp && me->sp->style && me->sp->style->name) {
	if (!strcmp(me->sp->style->name, "DivCenter") ||
	    !strcmp(me->sp->style->name, "HeadingCenter") ||
	    !strcmp(me->sp->style->name, "Heading1")) {
	    me->sp->style->alignment = HT_CENTER;
	} else if (!strcmp(me->sp->style->name, "DivRight") ||
		   !strcmp(me->sp->style->name, "HeadingRight")) {
	    me->sp->style->alignment = HT_RIGHT;
	} else	{
	    me->sp->style->alignment = HT_LEFT;
	}
	styles[HTML_PRE]->alignment = HT_LEFT;
    }
#ifdef USE_COLOR_STYLE
# if !OPT_SCN
    FREE(Style_className);
# endif
#endif
    FREE(me->base_href);
    FREE(me->map_address);
    FREE(me->textarea_name);
    FREE(me->textarea_accept_cs);
    FREE(me->textarea_cols);
    FREE(me->textarea_id);
    FREE(me->LastOptionValue);
    FREE(me->xinclude);
    clear_objectdata(me);
    FREE(me);
}

/*	Get Styles from style sheet
**	---------------------------
*/
PRIVATE void get_styles NOARGS
{
    styleSheet = DefaultStyle();
    default_style =		HTStyleNamed(styleSheet, "Normal");

    styles[HTML_H1] =		HTStyleNamed(styleSheet, "Heading1");
    styles[HTML_H2] =		HTStyleNamed(styleSheet, "Heading2");
    styles[HTML_H3] =		HTStyleNamed(styleSheet, "Heading3");
    styles[HTML_H4] =		HTStyleNamed(styleSheet, "Heading4");
    styles[HTML_H5] =		HTStyleNamed(styleSheet, "Heading5");
    styles[HTML_H6] =		HTStyleNamed(styleSheet, "Heading6");
    styles[HTML_HCENTER] =	HTStyleNamed(styleSheet, "HeadingCenter");
    styles[HTML_HLEFT] =	HTStyleNamed(styleSheet, "HeadingLeft");
    styles[HTML_HRIGHT] =	HTStyleNamed(styleSheet, "HeadingRight");

    styles[HTML_DCENTER] =	HTStyleNamed(styleSheet, "DivCenter");
    styles[HTML_DLEFT] =	HTStyleNamed(styleSheet, "DivLeft");
    styles[HTML_DRIGHT] =	HTStyleNamed(styleSheet, "DivRight");

    styles[HTML_DL] =		HTStyleNamed(styleSheet, "Glossary");
	/* nested list styles */
    styles[HTML_DL1] =		HTStyleNamed(styleSheet, "Glossary1");
    styles[HTML_DL2] =		HTStyleNamed(styleSheet, "Glossary2");
    styles[HTML_DL3] =		HTStyleNamed(styleSheet, "Glossary3");
    styles[HTML_DL4] =		HTStyleNamed(styleSheet, "Glossary4");
    styles[HTML_DL5] =		HTStyleNamed(styleSheet, "Glossary5");
    styles[HTML_DL6] =		HTStyleNamed(styleSheet, "Glossary6");

    styles[HTML_UL] =
    styles[HTML_OL] =		HTStyleNamed(styleSheet, "List");
	/* nested list styles */
    styles[HTML_OL1] =		HTStyleNamed(styleSheet, "List1");
    styles[HTML_OL2] =		HTStyleNamed(styleSheet, "List2");
    styles[HTML_OL3] =		HTStyleNamed(styleSheet, "List3");
    styles[HTML_OL4] =		HTStyleNamed(styleSheet, "List4");
    styles[HTML_OL5] =		HTStyleNamed(styleSheet, "List5");
    styles[HTML_OL6] =		HTStyleNamed(styleSheet, "List6");

    styles[HTML_MENU] =
    styles[HTML_DIR] =		HTStyleNamed(styleSheet, "Menu");
	/* nested list styles */
    styles[HTML_MENU1] =	HTStyleNamed(styleSheet, "Menu1");
    styles[HTML_MENU2] =	HTStyleNamed(styleSheet, "Menu2");
    styles[HTML_MENU3] =	HTStyleNamed(styleSheet, "Menu3");
    styles[HTML_MENU4] =	HTStyleNamed(styleSheet, "Menu4");
    styles[HTML_MENU5] =	HTStyleNamed(styleSheet, "Menu5");
    styles[HTML_MENU6] =	HTStyleNamed(styleSheet, "Menu6");

    styles[HTML_DLC] =		HTStyleNamed(styleSheet, "GlossaryCompact");
	/* nested list styles */
    styles[HTML_DLC1] =		HTStyleNamed(styleSheet, "GlossaryCompact1");
    styles[HTML_DLC2] =		HTStyleNamed(styleSheet, "GlossaryCompact2");
    styles[HTML_DLC3] =		HTStyleNamed(styleSheet, "GlossaryCompact3");
    styles[HTML_DLC4] =		HTStyleNamed(styleSheet, "GlossaryCompact4");
    styles[HTML_DLC5] =		HTStyleNamed(styleSheet, "GlossaryCompact5");
    styles[HTML_DLC6] =		HTStyleNamed(styleSheet, "GlossaryCompact6");

    styles[HTML_ADDRESS] =	HTStyleNamed(styleSheet, "Address");
    styles[HTML_BANNER] =	HTStyleNamed(styleSheet, "Banner");
    styles[HTML_BLOCKQUOTE] =	HTStyleNamed(styleSheet, "Blockquote");
    styles[HTML_BQ] =		HTStyleNamed(styleSheet, "Bq");
    styles[HTML_FN] =		HTStyleNamed(styleSheet, "Footnote");
    styles[HTML_NOTE] =		HTStyleNamed(styleSheet, "Note");
    styles[HTML_PLAINTEXT] =
    styles[HTML_XMP] =		HTStyleNamed(styleSheet, "Example");
    styles[HTML_PRE] =		HTStyleNamed(styleSheet, "Preformatted");
    styles[HTML_LISTING] =	HTStyleNamed(styleSheet, "Listing");
}

/*
 * If we're called from another module, make sure we've initialized styles
 * array first.
 */
PUBLIC  HTStyle *LYstyles ARGS1(int, style_number)
{
    if (styles[style_number] == 0)
	get_styles();
    return styles[style_number];
}

/*				P U B L I C
*/

/*	Structured Object Class
**	-----------------------
*/
PUBLIC CONST HTStructuredClass HTMLPresentation = /* As opposed to print etc */
{
	"Lynx_HTML_Handler",
	HTML_free,
	HTML_abort,
	HTML_put_character,	HTML_put_string,  HTML_write,
	HTML_start_element,	HTML_end_element,
	HTML_put_entity
};

/*		New Structured Text object
**		--------------------------
**
**	The structured stream can generate either presentation,
**	or plain text, or HTML.
*/
PUBLIC HTStructured* HTML_new ARGS3(
	HTParentAnchor *,	anchor,
	HTFormat,		format_out,
	HTStream*,		stream)
{

    HTStructured * me;

    if (format_out != WWW_PLAINTEXT && format_out != WWW_PRESENT) {
	HTStream * intermediate = HTStreamStack(WWW_HTML, format_out,
						stream, anchor);
	if (intermediate)
	    return HTMLGenerator(intermediate);
	fprintf(stderr, "\n** Internal error: can't parse HTML to %s\n",
		HTAtom_name(format_out));
	exit_immediately (-1);
    }

    me = (HTStructured*) calloc(sizeof(*me),1);
    if (me == NULL)
	outofmem(__FILE__, "HTML_new");

    /*
     * This used to call 'get_styles()' only on the first time through this
     * function.  However, if the user reloads a page with ^R, the styles[]
     * array is not necessarily the same as it was from 'get_styles()'.  So
     * we reinitialize the whole thing.
     */
    get_styles();

    me->isa = &HTMLPresentation;
    me->node_anchor = anchor;

    me->CurrentA = NULL;
    me->CurrentANum = 0;
    me->base_href = NULL;
    me->map_address = NULL;

    me->title.size = 0;
    me->title.growby = 128;
    me->title.allocated = 0;
    me->title.data = NULL;

    me->object.size = 0;
    me->object.growby = 128;
    me->object.allocated = 0;
    me->object.data = NULL;
    me->object_started = FALSE;
    me->object_declare = FALSE;
    me->object_shapes = FALSE;
    me->object_ismap = FALSE;
    me->object_id = NULL;
    me->object_title = NULL;
    me->object_data = NULL;
    me->object_type = NULL;
    me->object_classid = NULL;
    me->object_codebase = NULL;
    me->object_codetype = NULL;
    me->object_usemap = NULL;
    me->object_name = NULL;

    me->option.size = 0;
    me->option.growby = 128;
    me->option.allocated = 0;
    me->option.data = NULL;
    me->first_option = TRUE;
    me->LastOptionValue = NULL;
    me->LastOptionChecked = FALSE;
    me->select_disabled = FALSE;

    me->textarea.size = 0;
    me->textarea.growby = 128;
    me->textarea.allocated = 0;
    me->textarea.data = NULL;
    me->textarea_name = NULL;
    me->textarea_name_cs = -1;
    me->textarea_accept_cs = NULL;
    me->textarea_cols = NULL;
    me->textarea_rows = 4;
    me->textarea_disabled = NO;
    me->textarea_id = NULL;

    me->math.size = 0;
    me->math.growby = 128;
    me->math.allocated = 0;
    me->math.data = NULL;

    me->style_block.size = 0;
    me->style_block.growby = 128;
    me->style_block.allocated = 0;
    me->style_block.data = NULL;

    me->script.size = 0;
    me->script.growby = 128;
    me->script.allocated = 0;
    me->script.data = NULL;

    me->text = 0;
    me->style_change = YES;	/* Force check leading to text creation */
    me->new_style = default_style;
    me->old_style = 0;
    me->current_default_alignment = HT_LEFT;
    me->sp = (me->stack + MAX_NESTING - 1);
    me->skip_stack = 0;
    me->sp->tag_number = -1;				/* INVALID */
    me->sp->style = default_style;			/* INVALID */
    me->sp->style->alignment = HT_LEFT;
    me->stack_overrun = FALSE;

    me->Division_Level = -1;
    me->Underline_Level = 0;
    me->Quote_Level = 0;

    me->UsePlainSpace = FALSE;
    me->HiddenValue = FALSE;
    me->lastraw = -1;

    /*
     *	Used for nested lists. - FM
     */
    me->List_Nesting_Level = -1; /* counter for list nesting level */
    LYZero_OL_Counter(me);	 /* Initializes OL_Counter[] and OL_Type[] */
    me->Last_OL_Count = 0;	 /* last count in ordered lists */
    me->Last_OL_Type = '1';	 /* last type in ordered lists */

    me->inA = FALSE;
    me->inAPPLET = FALSE;
    me->inAPPLETwithP = FALSE;
    me->inBadBASE = FALSE;
    me->inBadHREF = FALSE;
    me->inBadHTML = FALSE;
    me->inBASE = FALSE;
    me->inBoldA = FALSE;
    me->inBoldH = FALSE;
    me->inCAPTION = FALSE;
    me->inCREDIT = FALSE;
    me->inFIG = FALSE;
    me->inFIGwithP = FALSE;
    me->inFONT = FALSE;
    me->inFORM = FALSE;
    me->inLABEL = FALSE;
    me->inP = FALSE;
    me->inPRE = FALSE;
    me->inSELECT = FALSE;
    me->inTABLE = FALSE;
    me->inUnderline = FALSE;

    me->needBoldH = FALSE;

    me->comment_start = NULL;
    me->comment_end = NULL;

#ifdef USE_COLOR_STYLE
# if !OPT_SCN
    FREE(Style_className);
# else
    Style_className_end = Style_className;
# endif
    class_string[0] = '\0';
#endif

#ifdef NOTUSED_FOTEMODS
    /*
    **	If the anchor already has stage info, make sure that it is
    **	appropriate for the current display charset.  HTMIMEConvert()
    **	does this for the http and https schemes, and HTCharsetFormat()
    **	does it for the file and and ftp schemes, be we need to do it,
    **	if necessary, for the gateway schemes. - FM
    */
    if (me->node_anchor->UCStages) {
	if (HTAnchor_getUCLYhndl(me->node_anchor,
				 UCT_STAGE_STRUCTURED) != current_char_set) {
	    /*
	    **	We are reloading due to a change in the display character
	    **	set.  Free the stage info and let the stage info creation
	    **	mechanisms create a new UCStages structure appropriate for
	    **	the current display character set. - FM
	    */
	    FREE(anchor->UCStages);
	} else if (HTAnchor_getUCLYhndl(me->node_anchor,
					UCT_STAGE_MIME) == current_char_set) {
	    /*
	    **	The MIME stage is set to the current display character
	    **	set.  If it is CJK, and HTCJK does not point to a CJK
	    **	character set, assume we are reloading due to a raw
	    **	mode toggle and reset the MIME and PARSER stages to
	    **	an ISO Latin 1 default. - FM
	    */
	    LYUCcharset *p_in = HTAnchor_getUCInfoStage(me->node_anchor,
							UCT_STAGE_MIME);
	    if (p_in->enc == UCT_ENC_CJK && HTCJK == NOCJK) {
		HTAnchor_resetUCInfoStage(me->node_anchor, LATIN1,
					  UCT_STAGE_MIME,
					  UCT_SETBY_DEFAULT);
		HTAnchor_setUCInfoStage(me->node_anchor, LATIN1,
					UCT_STAGE_MIME,
					UCT_SETBY_DEFAULT);
		HTAnchor_resetUCInfoStage(me->node_anchor, LATIN1,
					  UCT_STAGE_PARSER,
					  UCT_SETBY_DEFAULT);
		HTAnchor_setUCInfoStage(me->node_anchor, LATIN1,
					UCT_STAGE_PARSER,
					UCT_SETBY_DEFAULT);
	    }
	}
    }
#endif /* NOTUSED_FOTEMODS */

    /*
    **	Create a chartrans stage info structure for the anchor,
    **	if it does not exist already (in which case the default
    **	MIME stage info will be loaded as well), and load the
    **	HTML stage info into me->UCI and me->UCLYhndl. - FM
    */
    LYGetChartransInfo(me);
    UCTransParams_clear(&me->T);

    /*
    **	Load the existing or default input charset info
    **	into the holding elements.  We'll believe what
    **	is indicated for UCT_STAGE_PARSER. - FM
    */
    me->inUCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					  UCT_STAGE_PARSER);
    if (me->inUCLYhndl < 0) {
	me->inUCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					      UCT_STAGE_MIME);
	me->inUCI = HTAnchor_getUCInfoStage(me->node_anchor,
					    UCT_STAGE_MIME);
    } else {
	me->inUCI = HTAnchor_getUCInfoStage(me->node_anchor,
					    UCT_STAGE_PARSER);
    }

    /*
    **	Load the existing or default output charset info
    **	into the holding elements, UCT_STAGE_STRUCTURED
    **	should be the same as UCT_STAGE_TEXT at this point,
    **	but we could check, perhaps. - FM
    */
    me->outUCI = HTAnchor_getUCInfoStage(me->node_anchor,
					 UCT_STAGE_STRUCTURED);
    me->outUCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					   UCT_STAGE_STRUCTURED);
#ifdef NOTUSED_FOTEMODS
    UCSetTransParams(&me->T,
		     me->inUCLYhndl, me->inUCI,
		     me->outUCLYhndl, me->outUCI);
#endif

    me->target = stream;
    if (stream)
	me->targetClass = *stream->isa;			/* Copy pointers */

    return (HTStructured*) me;
}

#ifdef SOURCE_CACHE

/*
 *  A flag set by a file write error.  Used for only generating an alert
 *  the first time such an error happens, since Lynx should still be usable
 *  if the temp space becomes full, and an alert each time a cache file
 *  cannot be written would be annoying.  Reset when  lynx.cfg is being
 *  reloaded (user may change SOURCE_CACHE setting). - kw
 */
PUBLIC BOOLEAN source_cache_file_error = FALSE;

/*
 * Pass-thru cache HTStream
 */

PRIVATE void CacheThru_do_free ARGS1(
	HTStream *,	me)
{
    if (me->anchor->source_cache_file) {
	CTRACE((tfp, "SourceCacheWriter: Removing previous file %s\n",
		me->anchor->source_cache_file));
	LYRemoveTemp(me->anchor->source_cache_file);
	FREE(me->anchor->source_cache_file);
    }
    if (me->anchor->source_cache_chunk) {
	CTRACE((tfp, "SourceCacheWriter: Removing previous memory chunk %p\n",
		(void *)me->anchor->source_cache_chunk));
	HTChunkFree(me->anchor->source_cache_chunk);
	me->anchor->source_cache_chunk = NULL;
    }
    if (me->fp) {
	fflush(me->fp);
	if (ferror(me->fp))
	    me->status = HT_ERROR;
	LYCloseTempFP(me->fp);
	if (me->status == HT_OK) {
	    me->anchor->source_cache_file = me->filename;
	    CTRACE((tfp,
		    "SourceCacheWriter: Committing file %s for URL %s to anchor\n",
		    me->filename, HTAnchor_address((HTAnchor *)me->anchor)));
	} else {
	    if (source_cache_file_error == FALSE) {
		HTAlert(gettext("Source cache error - disk full?"));
		source_cache_file_error = TRUE;
	    }
	    LYRemoveTemp(me->filename);
	    me->anchor->source_cache_file = NULL;
	}
    } else if (me->status != HT_OK) {
	if (me->chunk) {
	    CTRACE((tfp, "SourceCacheWriter: memory chunk %p had errors.\n",
		    me->chunk));
	    HTChunkFree(me->chunk);
	    me->chunk = NULL;
	}
	HTAlert(gettext("Source cache error - not enough memory!"));
    }
    if (me->chunk) {
	me->anchor->source_cache_chunk = me->chunk;
	CTRACE((tfp,
		"SourceCacheWriter: Committing memory chunk %p for URL %s to anchor\n",
		(void *)me->chunk, HTAnchor_address((HTAnchor *)me->anchor)));
    }
}

PRIVATE void CacheThru_free ARGS1(
	HTStream *,	me)
{
    CacheThru_do_free(me);
    (*me->actions->_free)(me->target);
    FREE(me);
}

PRIVATE void CacheThru_abort ARGS2(
	HTStream *,	me,
	HTError,	e)
{
    if (me->fp)
	LYCloseTempFP(me->fp);
    if (LYCacheSourceForAborted == SOURCE_CACHE_FOR_ABORTED_DROP) {
	if (me->filename) {
	    CTRACE((tfp, "SourceCacheWriter: Removing active file %s\n",
		    me->filename));
	    LYRemoveTemp(me->filename);
	    FREE(me->filename);
	}
	if (me->chunk) {
	    CTRACE((tfp, "SourceCacheWriter: Removing active memory chunk %p\n",
		    (void *)me->chunk));
	    HTChunkFree(me->chunk);
	}
    } else {
	me->status = HT_OK;	/*fake it*/
	CacheThru_do_free(me);
    }
    (*me->actions->_abort)(me->target, e);
    FREE(me);
}

PRIVATE void CacheThru_put_character ARGS2(
	HTStream *,	me,
	char,		c_in)
{
    if (me->status == HT_OK) {
	if (me->fp) {
	    fputc(c_in, me->fp);
	} else if (me->chunk) {
	    HTChunkPutc(me->chunk, c_in);
	    if (me->chunk->allocated == 0)
		me->status = HT_ERROR;
	}
    }
    (*me->actions->put_character)(me->target, c_in);
}

PRIVATE void CacheThru_put_string ARGS2(
	HTStream *,	me,
	CONST char *,	str)
{
    if (me->status == HT_OK) {
	if (me->fp) {
	    fputs(str, me->fp);
	} else if (me->chunk) {
	    HTChunkPuts(me->chunk, str);
	    if (me->chunk->allocated == 0 && *str)
		me->status = HT_ERROR;
	}
    }
    (*me->actions->put_string)(me->target, str);
}

PRIVATE void CacheThru_write ARGS3(
	HTStream *,	me,
	CONST char *,	str,
	int,		l)
{
    if (me->status == HT_OK) {
	if (me->fp) {
	    fwrite(str, 1, l, me->fp);
	    if (ferror(me->fp))
		me->status = HT_ERROR;
	} else if (me->chunk) {
	    HTChunkPutb(me->chunk, str, l);
	    if (me->chunk->allocated == 0 && l != 0)
		me->status = HT_ERROR;
	}
    }
    (*me->actions->put_block)(me->target, str, l);
}

PRIVATE CONST HTStreamClass PassThruCache =
{
    "PassThruCache",
    CacheThru_free,
    CacheThru_abort,
    CacheThru_put_character,
    CacheThru_put_string,
    CacheThru_write
};

PRIVATE HTStream* CacheThru_new ARGS2(
	HTParentAnchor *,	anchor,
	HTStream *,		target)
{
    char filename[LY_MAXPATH];
    HTStream *stream = NULL;
    HTProtocol *p = (HTProtocol *)anchor->protocol;

    /*
     * Neatly and transparently vanish if source caching is disabled.
     */
    if (LYCacheSource == SOURCE_CACHE_NONE)
	return target;

#ifndef DEBUG_SOURCE_CACHE
    /*  Only remote HTML documents may benefits from HTreparse_document(), */
    /*  oh, assume http protocol:                                          */
    if (strcmp(p->name, "http") != 0) {
	CTRACE((tfp, "SourceCacheWriter: Protocol is \"%s\"; not caching\n", p->name));
	return target;
    }
#else
    /* all HTStreams will be cached */
#endif

    stream = (HTStream *) malloc(sizeof(*stream));
    if (!stream)
	outofmem(__FILE__, "CacheThru_new");

    stream->isa = &PassThruCache;
    stream->anchor = anchor;
    stream->fp = NULL;
    stream->filename = NULL;
    stream->chunk = NULL;
    stream->target = target;
    stream->actions = target->isa;
    stream->status = HT_OK;

    if (LYCacheSource == SOURCE_CACHE_FILE) {
	if (anchor->source_cache_file) {
	    CTRACE((tfp, "SourceCacheWriter: If successful, will replace source cache file %s\n",
		    anchor->source_cache_file));
#if 0 /* No, let's NOT do this. - kw 1999-12-05 */
	    FREE(stream);
	    return target;
#endif
	}

	/*
	 * We open the temp file in binary mode to make sure that
	 * end-of-line stuff and high-bit Latin-1 (or other) characters
	 * don't get munged; this way, the file should (knock on wood)
	 * contain exactly what came in from the network.
	 */
	if (!(stream->fp = LYOpenTemp(filename, HTML_SUFFIX, "wb"))) {
	    CTRACE((tfp, "SourceCacheWriter: Cannot open source cache file for URL %s\n",
		   HTAnchor_address((HTAnchor *)anchor)));
	    FREE(stream);
	    return target;
	}

	StrAllocCopy(stream->filename, filename);

	CTRACE((tfp, "SourceCacheWriter: Caching source for URL %s in file %s\n",
		     HTAnchor_address((HTAnchor *)anchor), filename));
    }

    if (LYCacheSource == SOURCE_CACHE_MEMORY) {
	if (anchor->source_cache_chunk) {
	    CTRACE((tfp,
		    "SourceCacheWriter: If successful, will replace memory chunk %p\n",
		    (void *)anchor->source_cache_chunk));
#if 0 /* No, let's NOT do this. - kw 1999-12-05 */
	    FREE(stream);
	    return target;
#endif
	}

#ifdef SAVE_TIME_NOT_SPACE
	stream->chunk = HTChunkCreateMayFail(4096, 1);
#else
	stream->chunk = HTChunkCreateMayFail(128, 1);
#endif
	if (!stream->chunk)	/* failed already? pretty bad... - kw */
	    stream->status = HT_ERROR;

	CTRACE((tfp, "SourceCacheWriter: Caching source for URL %s in memory chunk %p\n",
	       HTAnchor_address((HTAnchor *)anchor), (void *)stream->chunk));

    }

    return stream;
}
#else
#define CacheThru_new(anchor, target) target
#endif

/*	HTConverter for HTML to plain text
**	----------------------------------
**
**	This will convert from HTML to presentation or plain text.
**
**	It is registered in HTInit.c, but never actually used by lynx.
**	- kw 1999-03-15
*/
PUBLIC HTStream* HTMLToPlain ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor,
				  HTML_new(anchor, pres->rep_out, sink)));
}

/*	HTConverter for HTML source to plain text
**	-----------------------------------------
**
**	This will preparse HTML and convert back to presentation or plain text.
**
**	It is registered in HTInit.c and used by lynx if invoked with
**	-preparsed.  The stream generated here will be fed with HTML text,
**	It feeds that to the SGML.c parser, which in turn feeds an HTMLGen.c
**	structured stream for regenerating flat text; the latter should
**	end up being handled as text/plain. - kw
*/
PUBLIC HTStream* HTMLParsedPresent ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    HTStream * intermediate = sink;
    if (!intermediate) {
	/*
	 *  Trick to prevent HTPlainPresent from translating again.
	 *  Temporarily change UCT_STAGE_PARSER setting in anchor
	 *  while the HTPlain stream is initialized, so that HTPlain
	 *  sees its input and output charsets as the same.  - kw
	 */
	int old_parser_cset = HTAnchor_getUCLYhndl(anchor,UCT_STAGE_PARSER);
	int structured_cset = HTAnchor_getUCLYhndl(anchor,UCT_STAGE_STRUCTURED);
	if (structured_cset < 0)
	    structured_cset = HTAnchor_getUCLYhndl(anchor,UCT_STAGE_HTEXT);
	if (structured_cset < 0)
	    structured_cset = current_char_set;
	HTAnchor_setUCInfoStage(anchor, structured_cset,
				UCT_STAGE_PARSER, UCT_SETBY_MIME);
	if (pres->rep_out == WWW_SOURCE) {
		/*  same effect as
	    intermediate = HTPlainPresent(pres, anchor, NULL);
		    just written in a more general way:
		 */
	    intermediate = HTStreamStack(WWW_PLAINTEXT, WWW_PRESENT,
					 NULL, anchor);
	} else {
		/*  this too should amount to calling HTPlainPresent: */
	    intermediate = HTStreamStack(WWW_PLAINTEXT, pres->rep_out,
					 NULL, anchor);
	}
	if (old_parser_cset != structured_cset) {
	    HTAnchor_resetUCInfoStage(anchor, old_parser_cset,
				      UCT_STAGE_PARSER, UCT_SETBY_NONE);
	    if (old_parser_cset >= 0) {
		HTAnchor_setUCInfoStage(anchor, old_parser_cset,
					UCT_STAGE_PARSER,
					UCT_SETBY_DEFAULT+1);
	    }
	}
    }
    if (!intermediate)
	return NULL;
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor,
				  HTMLGenerator(intermediate)));
}

/*	HTConverter for HTML to C code
**	------------------------------
**
**	C code is like plain text but all non-preformatted code
**	is commented out.
**	This will convert from HTML to presentation or plain text.
**
**	It is registered in HTInit.c, but normally not used by lynx.
**	- kw 1999-03-15
*/
PUBLIC HTStream* HTMLToC ARGS3(
	HTPresentation *,	pres GCC_UNUSED,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    HTStructured * html;
#if 0
    if (!sink)
	sink = HTStreamStack(WWW_SOURCE, HTAtom_for("www/dump"),
	HTOutputStream, anchor);
#endif
    if (sink)
	(*sink->isa->put_string)(sink, "/* ");	/* Before even title */
    html = HTML_new(anchor, WWW_PLAINTEXT, sink);
    html->comment_start = "/* ";
    html->comment_end = " */\n";	/* Must start in col 1 for cpp */
    if (!sink)
	HTML_put_string(html,html->comment_start);
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor, html));
}

/*	Presenter for HTML
**	------------------
**
**	This will convert from HTML to presentation or plain text.
**
** (Comment from original libwww:)
**	Override this if you have a windows version
*/
#ifndef GUI
PUBLIC HTStream* HTMLPresent ARGS3(
	HTPresentation *,	pres GCC_UNUSED,
	HTParentAnchor *,	anchor,
	HTStream *,		sink GCC_UNUSED)
{
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor,
				  HTML_new(anchor, WWW_PRESENT, NULL)));
}
#endif /* !GUI */

/* (Comments from original libwww:) */
/*	Record error message as a hypertext object
**	------------------------------------------
**
**	The error message should be marked as an error so that
**	it can be reloaded later.
**	This implementation just throws up an error message
**	and leaves the document unloaded.
**	A smarter implementation would load an error document,
**	marking at such so that it is retried on reload.
**
** On entry,
**	sink	is a stream to the output device if any
**	number	is the HTTP error number
**	message is the human readable message.
**
** On exit,
**	returns a negative number to indicate lack of success in the load.
*/
/* (We don't actually do any of that hypertext stuff for errors,
   the trivial implementation for lynx just generates a message
   and returns. - kw 1999-03-15)
*/
PUBLIC int HTLoadError ARGS3(
	HTStream *,	sink GCC_UNUSED,
	int,		number,
	CONST char *,	message)
{
    HTAlert(message);		/* @@@@@@@@@@@@@@@@@@@ */
    return -number;
}

PRIVATE char * MakeNewTitle ARGS2(CONST char **, value, int, src_type)
{
    char *ptr;
    char *newtitle = NULL;

    StrAllocCopy(newtitle, "[");
    if (value != 0 && value[src_type] != 0)
	ptr = strrchr(value[src_type], '/');
    else
	ptr = 0;
    if (!ptr) {
	StrAllocCat(newtitle, value[src_type]);
    } else {
	StrAllocCat(newtitle, ptr + 1);
    }
#ifdef SH_EX	/* 1998/04/02 (Thu) 16:02:00 */

    /* for proxy server 1998/12/19 (Sat) 11:53:30 */
    if (AS_casecomp(newtitle + 1, "internal-gopher-menu") == 0) {
	StrAllocCopy(newtitle, "+");
    } else if (AS_casecomp(newtitle + 1, "internal-gopher-unknown") == 0) {
	StrAllocCopy(newtitle, " ");
    } else {
	/* normal title */
	ptr = strrchr(newtitle, '.');
	if (ptr) {
	  if (AS_casecomp(ptr, ".gif") == 0)
	    *ptr = '\0';
	  else if (AS_casecomp(ptr, ".jpg") == 0)
	    *ptr = '\0';
	  else if (AS_casecomp(ptr, ".jpeg") == 0)
	    *ptr = '\0';
	}
	StrAllocCat(newtitle, "]");
    }
#else
    StrAllocCat(newtitle, "]");
#endif
    return newtitle;
}

PRIVATE char * MakeNewImageValue ARGS1(CONST char **, value)
{
    char *ptr;
    char *newtitle = NULL;

    StrAllocCopy(newtitle, "[");
    ptr = strrchr(value[HTML_INPUT_SRC], '/');
    if (!ptr) {
	StrAllocCat(newtitle, value[HTML_INPUT_SRC]);
    } else {
	StrAllocCat(newtitle, ptr + 1);
    }
    StrAllocCat(newtitle, "]-Submit");
    return newtitle;
}

PRIVATE char * MakeNewMapValue ARGS2(CONST char **, value, CONST char*, mapstr)
{
    char *ptr;
    char *newtitle = NULL;

    StrAllocCopy(newtitle, "[");
    StrAllocCat(newtitle,mapstr); /* ISMAP or USEMAP */
    if ( verbose_img && value[HTML_IMG_SRC] && *value[HTML_IMG_SRC] ) {
	StrAllocCat(newtitle,":");
	ptr = strrchr(value[HTML_IMG_SRC], '/');
	if (!ptr) {
	    StrAllocCat(newtitle, value[HTML_IMG_SRC]);
	} else {
	    StrAllocCat(newtitle, ptr + 1);
	}
    }
    StrAllocCat(newtitle, "]");
    return newtitle;
}
