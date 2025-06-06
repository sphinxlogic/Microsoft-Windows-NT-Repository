/*		Character grid hypertext object
**		===============================
*/

#include <HTUtils.h>
#include <HTString.h>
#include <HTAccess.h>
#include <HTAnchor.h>
#include <HTParse.h>
#include <HTTP.h>
#include <HTAlert.h>
#include <HTCJK.h>
#include <HTFile.h>
#include <UCDefs.h>
#include <UCAux.h>

#include <assert.h>

#include <GridText.h>
#include <LYCurses.h>
#include <LYUtils.h>
#include <LYStrings.h>
#include <LYStructs.h>
#include <LYGlobalDefs.h>
#include <LYGetFile.h>
#include <LYClean.h>
#include <LYMail.h>
#include <LYList.h>
#include <LYCharSets.h>
#include <LYCharUtils.h>	/* LYUCTranslateBack... */
#include <UCMap.h>
#include <LYEdit.h>
#include <LYPrint.h>
#include <LYPrettySrc.h>
#include <TRSTable.h>
#ifdef EXP_CHARTRANS_AUTOSWITCH
#include <UCAuto.h>
#endif /* EXP_CHARTRANS_AUTOSWITCH */

#include <LYexit.h>
#include <LYLeaks.h>

#ifdef SH_EX	/* for DEBUG (1997/10/10 (Fri) 07:58:47) */
#define NOTUSED_BAD_FOR_SCREEN
#endif

#undef DEBUG_APPCH

#ifdef SOURCE_CACHE
#include <HTFile.h>
#endif

#ifdef USE_COLOR_STYLE
#include <AttrList.h>
#include <LYHash.h>

unsigned int cached_styles[CACHEH][CACHEW];

#endif

#include <LYJustify.h>


#ifdef USE_COLOR_STYLE_UNUSED
void LynxClearScreenCache NOARGS
{
    int i,j;

    CTRACE((tfp, "GridText: flushing cached screen styles\n"));
    for (i=0;i<CACHEH;i++)
	for (j=0;j<CACHEW;j++)
	    cached_styles[i][j]=s_a;
}
#endif
#ifdef USE_COLOR_STYLE
PRIVATE void LynxResetScreenCache NOARGS
{
    int i,j;

    for (i=1; (i<CACHEH && i <= display_lines); i++) {
	for (j=0;j<CACHEW;j++)
	    cached_styles[i][j]=0;
    }
}
#endif /* USE_COLOR_STYLE */

struct _HTStream {			/* only know it as object */
    CONST HTStreamClass *       isa;
    /* ... */
};

#define TITLE_LINES  1
#define IS_UTF_EXTRA(ch) (text->T.output_utf8 && \
			  (UCH((ch))&0xc0) == 0x80)
/* a test in compact form: how many extra UTF-8 chars after initial? - kw */
#define UTF8_XNEGLEN(c) (c&0xC0? 0 :c&32? 1 :c&16? 2 :c&8? 3 :c&4? 4 :c&2? 5:0)
#define UTF_XLEN(c) UTF8_XNEGLEN(((char)~(c)))

extern BOOL HTPassHighCtrlRaw;

#ifdef KANJI_CODE_OVERRIDE
PUBLIC HTkcode last_kcode = NOKANJI;	/* 1997/11/14 (Fri) 09:09:26 */
#endif
#ifdef CJK_EX
#define CHAR_WIDTH 6
#else
#define CHAR_WIDTH 1
#endif

/*	Exports
*/
PUBLIC HText * HTMainText = NULL;		/* Equivalent of main window */
PUBLIC HTParentAnchor * HTMainAnchor = NULL;	/* Anchor for HTMainText */

PUBLIC char * HTAppName = LYNX_NAME;		/* Application name */
PUBLIC char * HTAppVersion = LYNX_VERSION;	/* Application version */

PUBLIC int HTFormNumber = 0;
PUBLIC int HTFormFields = 0;
PUBLIC char * HTCurSelectGroup = NULL;		/* Form select group name */
PRIVATE int HTCurSelectGroupCharset = -1;	/* ... and name's charset */
PUBLIC int HTCurSelectGroupType = F_RADIO_TYPE;	/* Group type */
PUBLIC char * HTCurSelectGroupSize = NULL;	/* Length of select */
PRIVATE char * HTCurSelectedOptionValue = NULL;	/* Select choice */

PUBLIC char * checked_box = "[X]";
PUBLIC char * unchecked_box = "[ ]";
PUBLIC char * checked_radio = "(*)";
PUBLIC char * unchecked_radio = "( )";

PUBLIC BOOLEAN underline_on = OFF;
PUBLIC BOOLEAN bold_on      = OFF;

#ifdef SOURCE_CACHE
PUBLIC int LYCacheSource = SOURCE_CACHE_NONE;
PUBLIC int LYCacheSourceForAborted = SOURCE_CACHE_FOR_ABORTED_DROP;
#endif

#ifdef USE_SCROLLBAR
PUBLIC int LYsb = FALSE;
PUBLIC int LYsb_arrow = TRUE;
PUBLIC int LYsb_begin = -1;
PUBLIC int LYsb_end = -1;
#endif

#if defined(USE_COLOR_STYLE)
#define MAX_STYLES_ON_LINE 64

#ifdef OLD_HTSTYLECHANGE
typedef struct _stylechange {
	int	horizpos;	/* horizontal position of this change */
	int	style;		/* which style to change to */
	int	direction;	/* on or off */
	int	previous;	/* previous style */
} HTStyleChange;
#else
    /*try to fit in 2 shorts*/
typedef struct _stylechange {
	unsigned int	direction:2;	/* on or off */
	unsigned int	horizpos: (sizeof(short)*CHAR_BIT-2);
	    /* horizontal position of this change */
	unsigned short	style;		/* which style to change to */
} HTStyleChange;
#endif
#endif

typedef struct _line {
	struct _line	*next;
	struct _line	*prev;
	unsigned	offset;		/* Implicit initial spaces */
	unsigned	size;		/* Number of characters */
	BOOL	split_after;		/* Can we split after? */
	BOOL	bullet;			/* Do we bullet? */
	BOOL	expansion_line;		/* TEXTAREA edit new line flag */
#if defined(USE_COLOR_STYLE)
#ifdef OLD_HTSTYLECHANGE
	HTStyleChange	styles[MAX_STYLES_ON_LINE];
#else
	HTStyleChange* styles;
#endif
	int	numstyles;
#endif
	char	data[1];		/* Space for terminator at least! */
} HTLine;

#if defined(USE_COLOR_STYLE) && !defined(OLD_HTSTYLECHANGE)
typedef struct _HTStyleChangePool {
	HTStyleChange	data[4092];
	struct _HTStyleChangePool* next;
	int free_items;
} HTStyleChangePool;

/*these are used when current line is being aggregated. */
HTStyleChange stylechanges_buffers[2][MAX_STYLES_ON_LINE];
int stylechanges_buffers_free;/*this is an index of the free buffer.
    Can be 0 or 1*/

/* These are generic macors for any pools (provided those structures have the
same members as HTStyleChangePool).  Pools are used for allocation of groups of
objects of the same type T.  Pools are represented as a list of structures of
type P (called pool chunks here).  Structure P has an array of N objects of
type T named 'data' (the number N in the array can be chosen arbitrary),
pointer to the next pool chunk named 'pool', and the number of free items in
that pool chunk named 'free_items'.  Here is a definition of the structure P:
	struct P
	{
	    T data[N];
	    struct P* next;
	    int free_items;
	};
 It's recommended that sizeof(P) be memory page size minus 32 in order malloc'd
chunks to fit in machine page size.
 Allocation of 'n' items in the pool is implemented by decrementing member
'free_items' by 'n' if 'free_items' >= 'n', or allocating a new pool chunk and
allocating 'n' items in that new chunk.  It's the task of the programmer to
assert that 'n' is <= N.  Only entire pool may be freed - this limitation makes
allocation algorithms trivial and fast - so the use of pools is limited to
objects that are freed in batch, that are not deallocated not in the batch, and
not reallocated.
 Pools greatly reduce memory fragmentation and memory allocation/deallocation
speed due to the simple algorithms used.  Due to the fact that memory is
'allocated' in array, alignment overhead is minimal.  Allocating strings in a
pool provided their length will never exceed N and is much smaller than N seems
to be very efficient.

 Pool are referenced by pointer to the chunk that contains free slots. Macros
that allocate memory in pools update that pointer if needed.
 There are 3 macros that deal with pools - POOL_NEW, POOL_FREE and
ALLOC_IN_POOL.
 Here is a description of those macros as C++ functions (with names mentioned
above and with use of C++ references)

void ALLOC_IN_POOL( P*& pool, pool_type, int toalloc, T*& ptr)
    - allocates 'toalloc' items in the pool of type 'pool_type' pointed by
    'pool', sets the pointer 'ptr' to the "allocated" memory and updates 'pool'
    if necessary. Sets 'ptr' to NULL if fails.

void POOL_NEW( pool_type  , P*& ptr)
    Initializes a pool of type 'pool_type' pointed by 'ptr', updating 'ptr'.
    Sets 'ptr' to NULL if fails.

void POOL_FREE( pool_type , P* ptr)
    Frees a pool of type 'pool_type' pointed by ptr.

      - VH */

/*
void ALLOC_IN_POOL( P*& pool, pool_type, int toalloc, T*& ptr)
    - allocates 'toalloc' items in the pool of type 'pool_type' pointed by
    'pool', sets the pointer 'ptr' to the "allocated" memory and updates 'pool'
    if necessary. Sets 'ptr' to NULL if fails.
*/
#define ALLOC_IN_POOL(pool,pool_type,toalloc,ptr)     \
if (!pool)  \
    ptr = NULL; \
else { \
    if ((pool)->free_items > toalloc) { \
	(pool)->free_items -= toalloc; \
	ptr = (pool)->data + (pool)->free_items; \
    } else { \
	pool_type* newpool = (pool_type*)malloc(sizeof(pool_type)); \
	if (!newpool) { \
	    ptr = NULL; \
	} else { \
	    newpool->next = pool; \
	    newpool->free_items = sizeof newpool->data/ \
		    sizeof newpool->data[0] - toalloc; \
	    pool = newpool; \
	    ptr = newpool->data + sizeof newpool->data/sizeof newpool->data[0] - toalloc; \
	} \
    } \
}
/*
void POOL_NEW( pool_type  , P*& ptr)
    Initializes a pool of type 'pool_type' pointed by 'ptr', updating 'ptr'.
    Sets 'ptr' to NULL if fails.
*/
#define POOL_NEW(pool_type,ptr) \
    { \
	pool_type* newpool = (pool_type*)malloc(sizeof(pool_type)); \
	if (!newpool) { \
	    ptr = NULL; \
	} else { \
	    newpool->next = NULL; \
	    newpool->free_items = sizeof newpool->data/sizeof newpool->data[0]; \
	    ptr = newpool; \
	} \
    }
/*
void POOL_FREE( pool_type , P* ptr)
    Frees a pool of type 'pool_type' pointed by ptr.
*/
#define POOL_FREE(pool_type,xptr) \
    { \
	pool_type* ptr = xptr; \
	do { \
	    pool_type* prevpool = ptr; \
	    ptr = ptr->next; \
	    FREE(prevpool); \
	} while (ptr); \
    }
#endif

#define LINE_SIZE(l) (sizeof(HTLine)+(l))	/* Allow for terminator */
#define allocHTLine(l) (HTLine *)calloc(1, LINE_SIZE(l))

typedef struct _TextAnchor {
	struct _TextAnchor *	next;
	int			number;		/* For user interface */
	int			start;		/* Characters */
	int			line_pos;	/* Position in text */
	int			extent;		/* Characters */
	int			line_num;	/* Place in document */
	char *			hightext;	/* The link text */
	char *			hightext2;	/* A second line*/
	int			hightext2offset;/* offset from left */
	int			link_type;	/* Normal, internal, or form? */
	FormInfo *		input_field;	/* Info for form links */
	BOOL			show_anchor;	/* Show the anchor? */
	BOOL			inUnderline;	/* context is underlined */
	BOOL			expansion_anch; /* TEXTAREA edit new anchor */
	HTChildAnchor *		anchor;
} TextAnchor;

typedef struct _HTTabID {
	char *			name;		/* ID value of TAB */
	int			column;		/* Zero-based column value */
} HTTabID;


/*	Notes on struct _Htext:
**	next_line is valid if stale is false.
**	top_of_screen line means the line at the top of the screen
**			or just under the title if there is one.
*/
struct _HText {
	HTParentAnchor *	node_anchor;
#ifdef SOURCE_CACHE
	/*
	 * Parse settings when this HText was generated.
	 */
	BOOLEAN			clickable_images;
	BOOLEAN			pseudo_inline_alts;
	BOOLEAN			verbose_img;
	BOOLEAN			raw_mode;
	BOOLEAN			historical_comments;
	BOOLEAN			minimal_comments;
	BOOLEAN			soft_dquotes;
	BOOLEAN			old_dtd;
	int			keypad_mode;
	int			disp_lines;		/* Screen size */
	int			disp_cols;
#endif
	HTLine *		last_line;
	int			Lines;		/* Number of them */
	int			chars;		/* Number of them */
	TextAnchor *		first_anchor;	/* Singly linked list */
	TextAnchor *		last_anchor;
	TextAnchor *		last_anchor_before_stbl;
	HTList *		forms;		/* also linked internally */
	int			last_anchor_number;	/* user number */
	BOOL			source;		/* Is the text source? */
	BOOL			toolbar;	/* Toolbar set? */
	HTList *		tabs;		/* TAB IDs */
	HTList *		hidden_links;	/* Content-less links ... */
	int			hiddenlinkflag; /*  ... and how to treat them */
	BOOL			no_cache;	/* Always refresh? */
	char			LastChar;	/* For absorbing white space */
	BOOL			IgnoreExcess;	/* Ignore chars at wrap point */

/* For Internal use: */
	HTStyle *		style;			/* Current style */
	int			display_on_the_fly;	/* Lines left */
	int			top_of_screen;		/* Line number */
	HTLine *		top_of_screen_line;	/* Top */
	HTLine *		next_line;		/* Bottom + 1 */
	unsigned		permissible_split;	/* in last line */
	BOOL			in_line_1;		/* of paragraph */
	BOOL			stale;			/* Must refresh */
	BOOL			page_has_target; /* has target on screen */
	BOOL			has_utf8; /* has utf-8 on screen or line */
	BOOL			had_utf8; /* had utf-8 when last displayed */
#ifdef DISP_PARTIAL
	int			first_lineno_last_disp_partial;
	int			last_lineno_last_disp_partial;
#endif
	STable_info *		stbl;

	HTkcode			kcode;			/* Kanji code? */
	HTkcode			specified_kcode;	/* Specified Kanji code */
#ifdef USE_TH_JP_AUTO_DETECT
	enum _detected_kcode  { DET_SJIS, DET_EUC, DET_NOTYET, DET_MIXED }
				detected_kcode;		/* Detected Kanji code */
	enum _SJIS_status     { SJIS_state_neutral, SJIS_state_in_kanji,
				SJIS_state_has_bad_code } SJIS_status;
	enum _EUC_status      { EUC_state_neutral, EUC_state_in_kanji,
				EUC_state_in_kana, EUC_state_has_bad_code }
				EUC_status;
#endif
	enum grid_state       { S_text, S_esc, S_dollar, S_paren,
				S_nonascii_text, S_dollar_paren,
				S_jisx0201_text }
				state;			/* Escape sequence? */
	int			kanji_buf;		/* Lead multibyte */
	int			in_sjis;		/* SJIS flag */
	int			halted;			/* emergency halt */

	BOOL			have_8bit_chars;   /* Any non-ASCII chars? */
	LYUCcharset *		UCI;		   /* node_anchor UCInfo */
	int			UCLYhndl;	   /* charset we are fed */
	UCTransParams		T;

	HTStream *		target;			/* Output stream */
	HTStreamClass		targetClass;		/* Output routines */
#if defined(USE_COLOR_STYLE) && !defined(OLD_HTSTYLECHANGE)
	HTStyleChangePool*	styles_pool;
#endif
};

PRIVATE void HText_AddHiddenLink PARAMS((HText *text, TextAnchor *textanchor));


#ifdef EXP_JUSTIFY_ELTS
PUBLIC BOOL can_justify_here;
PUBLIC BOOL can_justify_here_saved;

PUBLIC BOOL can_justify_this_line;/* =FALSE if line contains form objects */
PUBLIC int wait_for_this_stacked_elt;/* -1 if can justify contents of the
    element on the op of stack. If positive - specifies minimal stack depth
    plus 1 at which we can justify element (can be MAX_LINE+2 if
    ok_justify ==FALSE or in psrcview. */
PUBLIC BOOL form_in_htext;/*to indicate that we are in form (since HTML_FORM is
  not stacked in the HTML.c */
PUBLIC BOOL in_DT = FALSE;
#ifdef DEBUG_JUSTIFY
PUBLIC BOOL can_justify_stack_depth;/* can be 0 or 1 if all code is correct*/
#endif


typedef struct ht_run_info_ {
    int byte_len;		/*length in bytes*/
    int cell_len;		/*length in cells*/
} ht_run_info;

static int justify_start_position;/* this is an index of char from which
    justification can start (eg after "* " preceeding <li> text) */

static int ht_num_runs;/*the number of runs filled*/
static ht_run_info ht_runs[MAX_LINE];
static BOOL this_line_was_split;
static TextAnchor* last_anchor_of_previous_line;
static int justified_text_map[MAX_LINE]; /* this is a map - for each index i
    it tells to which position j=justified_text_map[i] in justified text
    i-th character is mapped - it's used for anchor positions fixup and for
    color style's positions adjustment. */
static BOOL have_raw_nbsps = FALSE;

PUBLIC void ht_justify_cleanup NOARGS
{
    wait_for_this_stacked_elt = !ok_justify
#  ifdef USE_PRETTYSRC
	|| psrc_view
#  endif
	? 30000/*MAX_NESTING*/+2 /*some unreachable value*/ : -1;
    can_justify_here = TRUE;
    can_justify_this_line = TRUE;
    form_in_htext = FALSE;

    last_anchor_of_previous_line = NULL;
    this_line_was_split = FALSE;
    in_DT = FALSE;
    have_raw_nbsps = FALSE;
}

PUBLIC void mark_justify_start_position ARGS1(void*,text)
{
    if (text && ((HText*)text)->last_line)
	justify_start_position = ((HText*)text )->last_line->size;
}


#define REALLY_CAN_JUSTIFY(text) ( (wait_for_this_stacked_elt<0) && \
	( text->style->alignment == HT_LEFT     || \
	  text->style->alignment == HT_JUSTIFY) && \
	HTCJK == NOCJK && !in_DT && \
	can_justify_here && can_justify_this_line && !form_in_htext )

#endif /* EXP_JUSTIFY_ELTS */



/*
 *  Boring static variable used for moving cursor across
 */
#define UNDERSCORES(n) \
 ((n) >= MAX_LINE ? underscore_string : &underscore_string[(MAX_LINE-1)] - (n))

/*
 *	Memory leak fixed.
 *	05-29-94 Lynx 2-3-1 Garrett Arch Blythe
 *	Changed to arrays.
 */
PRIVATE char underscore_string[MAX_LINE + 1];
PUBLIC char star_string[MAX_LINE + 1];

PRIVATE int ctrl_chars_on_this_line = 0; /* num of ctrl chars in current line */
PRIVATE int utfxtra_on_this_line = 0; /* num of UTF-8 extra bytes in line,
				       they *also* count as ctrl chars. */

PRIVATE HTStyle default_style =
	{ 0,  "(Unstyled)", "",
	(HTFont)0, 1, HT_BLACK,		0, 0,
	0, 0, 0, HT_LEFT,		1, 0,	0,
	NO, NO, 0, 0,			0 };



PRIVATE HTList * loaded_texts = NULL;	 /* A list of all those in memory */
PUBLIC  HTList * search_queries = NULL;  /* isindex and whereis queries   */
#ifdef LY_FIND_LEAKS
PRIVATE void free_all_texts NOARGS;
#endif
PRIVATE int HText_TrueLineSize PARAMS((
	HTLine *	line,
	HText *		text,
	BOOL		IgnoreSpaces));

#ifndef VMS			/* VMS has a better way - right? - kw */
#define CHECK_FREE_MEM
#endif


#ifdef CHECK_FREE_MEM

/*
 *  text->halted = 1: have set fake 'Z' and output a message
 *		   2: next time when HText_appendCharacter is called
 *		      it will append *** MEMORY EXHAUSTED ***, then set
 *		      to 3.
 *		   3: normal text output will be suppressed (but not anchors,
 *		      form fields etc.)
 */
PRIVATE void HText_halt NOARGS
{
    if (HTFormNumber > 0)
	HText_DisableCurrentForm();
    if (!HTMainText)
	return;
    if (HTMainText->halted < 2)
	HTMainText->halted = 2;
}

#define MIN_NEEDED_MEM 5000

/*
 *  Check whether factor*min(bytes,MIN_NEEDED_MEM) is available,
 *  or bytes if factor is 0.
 *  MIN_NEEDED_MEM and factor together represent a security margin,
 *  to take account of all the memory allocations where we don't check
 *  and of buffers which may be emptied before HTCheckForInterupt()
 *  is (maybe) called and other things happening, with some chance of
 *  success.
 *  This just tries to malloc() the to-be-checked-for amount of memory,
 *  which might make the situation worse depending how allocation works.
 *  There should be a better way... - kw
 */
PRIVATE BOOL mem_is_avail ARGS2(
    size_t,	factor,
    size_t,	bytes)
{
    void *p;
    if (bytes < MIN_NEEDED_MEM && factor > 0)
	bytes = MIN_NEEDED_MEM;
    if (factor == 0)
	factor = 1;
    p = malloc(factor * bytes);
    if (p) {
	FREE(p);
	return YES;
    } else {
	return NO;
    }
}

/*
 *  Replacement for calloc which checks for "enough" free memory
 *  (with some security margins) and tries various recovery actions
 *  if deemed necessary. - kw
 */
PRIVATE void * LY_check_calloc ARGS2(
    size_t,	nmemb,
    size_t,	size)
{
    int i, n;
    if (mem_is_avail(4, nmemb * size)) {
	return (calloc(nmemb, size));
    }
    n = HTList_count(loaded_texts);
    for (i = n - 1; i > 0; i--) {
	HText * t = HTList_objectAt(loaded_texts, i);
	if (t == HTMainText)
	    t = NULL;		/* shouldn't happen */
	{
	CTRACE((tfp, "\r *** Emergency freeing document %d/%d for '%s'%s!\n",
		    i + 1, n,
		    ((t && t->node_anchor &&
		      t->node_anchor->address) ?
		     t->node_anchor->address : "unknown anchor"),
		    ((t && t->node_anchor &&
		      t->node_anchor->post_data) ?
		     " with POST data" : "")));
	}
	HTList_removeObjectAt(loaded_texts, i);
	HText_free(t);
	if (mem_is_avail(4, nmemb * size)) {
	    return (calloc(nmemb, size));
	}
    }
    LYFakeZap(YES);
    if (!HTMainText || HTMainText->halted <= 1) {
	if (!mem_is_avail(2, nmemb * size)) {
	    HText_halt();
	    if (mem_is_avail(0, 700)) {
		HTAlert(gettext("Memory exhausted, display interrupted!"));
	    }
	} else {
	    if ((!HTMainText || HTMainText->halted == 0) &&
		mem_is_avail(0, 700)) {
		HTAlert(gettext("Memory exhausted, will interrupt transfer!"));
		if (HTMainText)
		    HTMainText->halted = 1;
	    }
	}
    }
    return (calloc(nmemb, size));
}

#define LY_CALLOC LY_check_calloc

#else  /* CHECK_FREE_MEM */

  /* using the regular calloc */
#define LY_CALLOC calloc

#endif /* CHECK_FREE_MEM */

PRIVATE void HText_getChartransInfo ARGS1(
	HText *,	me)
{
    me->UCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor, UCT_STAGE_HTEXT);
    if (me->UCLYhndl < 0) {
	int chndl = current_char_set;
	HTAnchor_setUCInfoStage(me->node_anchor, chndl,
				UCT_STAGE_HTEXT, UCT_SETBY_STRUCTURED);
	me->UCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					    UCT_STAGE_HTEXT);
    }
    me->UCI = HTAnchor_getUCInfoStage(me->node_anchor, UCT_STAGE_HTEXT);
}

PRIVATE void PerFormInfo_free ARGS1(
    PerFormInfo *,	form)
{
    if (form) {
	FREE(form->accept_cs);
	FREE(form->thisacceptcs);
	FREE(form);
    }
}

PRIVATE void FormList_delete ARGS1(
    HTList *,		forms)
{
    HTList *cur = forms;
    PerFormInfo *form;
    while ((form = (PerFormInfo *)HTList_nextObject(cur)) != NULL)
	PerFormInfo_free(form);
    HTList_delete(forms);
}

/*			Creation Method
**			---------------
*/
PUBLIC HText *	HText_new ARGS1(
	HTParentAnchor *,	anchor)
{
#if defined(VMS) && defined(VAXC) && !defined(__DECC)
#include <lib$routines.h>
    int status, VMType=3, VMTotal;
#endif /* VMS && VAXC && !__DECC */
    HTLine * line = NULL;
    HText * self = typecalloc(HText);
    if (!self)
	return self;

    CTRACE((tfp, "GridText: start HText_new\n"));

#if defined(VMS) && defined (VAXC) && !defined(__DECC)
    status = lib$stat_vm(&VMType, &VMTotal);
    CTRACE((tfp, "GridText: VMTotal = %d\n", VMTotal));
#endif /* VMS && VAXC && !__DECC */

    /*
     *  If the previously shown text had UTF-8 characters on screen,
     *  remember this in the newly created object.  Do this now, before
     *  the previous object may become invalid. - kw
     */
    if (HTMainText) {
	if (HText_hasUTF8OutputSet(HTMainText) &&
	    HTLoadedDocumentEightbit() &&
	    LYCharSet_UC[current_char_set].enc == UCT_ENC_UTF8) {
	    self->had_utf8 = HTMainText->has_utf8;
	} else {
	    self->had_utf8 = HTMainText->has_utf8;
	}
	HTMainText->has_utf8 = NO;
    }

    if (!loaded_texts)	{
	loaded_texts = HTList_new();
#ifdef LY_FIND_LEAKS
	atexit(free_all_texts);
#endif
    }

    /*
     *  Links between anchors & documents are a 1-1 relationship.  If
     *  an anchor is already linked to a document we didn't call
     *  HTuncache_current_document(), e.g., for the showinfo, options,
     *  download, print, etc., temporary file URLs, so we'll check now
     *  and free it before reloading. - Dick Wesseling (ftu@fi.ruu.nl)
     */
    if (anchor->document) {
	HTList_removeObject(loaded_texts, anchor->document);
	CTRACE((tfp, "GridText: Auto-uncaching\n")) ;
	((HText *)anchor->document)->node_anchor = NULL;
	HText_free((HText *)anchor->document);
	anchor->document = NULL;
    }

    HTList_addObject(loaded_texts, self);
#if defined(VMS) && defined(VAXC) && !defined(__DECC)
    while (HTList_count(loaded_texts) > HTCacheSize &&
	   VMTotal > HTVirtualMemorySize)
#else
    if (HTList_count(loaded_texts) > HTCacheSize)
#endif /* VMS && VAXC && !__DECC */
    {
	CTRACE((tfp, "GridText: Freeing off cached doc.\n"));
	HText_free((HText *)HTList_removeFirstObject(loaded_texts));
#if defined(VMS) && defined (VAXC) && !defined(__DECC)
	status = lib$stat_vm(&VMType, &VMTotal);
	CTRACE((tfp, "GridText: VMTotal reduced to %d\n", VMTotal));
#endif /* VMS && VAXC && !__DECC */
    }

    line = self->last_line = allocHTLine(MAX_LINE);
    if (line == NULL)
	outofmem(__FILE__, "HText_New");
    line->next = line->prev = line;
    line->offset = line->size = 0;
#ifdef USE_COLOR_STYLE
    line->numstyles = 0;
#ifndef  OLD_HTSTYLECHANGE
    POOL_NEW(HTStyleChangePool,self->styles_pool);
    if (!self->styles_pool)
	outofmem(__FILE__, "HText_New");
    stylechanges_buffers_free = 0;
    line->styles = stylechanges_buffers[0];
#endif
#endif
    self->Lines = self->chars = 0;
    self->first_anchor = self->last_anchor = NULL;
    self->style = &default_style;
    self->top_of_screen = 0;
    self->node_anchor = anchor;
    self->last_anchor_number = 0;	/* Numbering of them for references */
    self->stale = YES;
    self->toolbar = NO;
    self->tabs = NULL;
#ifdef SOURCE_CACHE
    /*
     * Remember the parse settings.
     */
    self->clickable_images = clickable_images;
    self->pseudo_inline_alts = pseudo_inline_alts;
    self->verbose_img = verbose_img;
    self->raw_mode = LYUseDefaultRawMode;
    self->historical_comments = historical_comments;
    self->minimal_comments = minimal_comments;
    self->soft_dquotes = soft_dquotes;
    self->old_dtd = Old_DTD;
    self->keypad_mode = keypad_mode;
    self->disp_lines = LYlines;
    self->disp_cols = LYcols;
#endif
    /*
     *  If we are going to render the List Page, always merge in hidden
     *  links to get the numbering consistent if form fields are numbered
     *  and show up as hidden links in the list of links.
     *  If we are going to render a bookmark file, also always merge in
     *	hidden links, to get the link numbers consistent with the counting
     *  in remove_bookmark_link().  Normally a bookmark file shouldn't
     *	contain any entries with empty titles, but it might happen. - kw
     */
    if (anchor->bookmark ||
	LYIsUIPage3(anchor->address, UIP_LIST_PAGE, 0) ||
	LYIsUIPage3(anchor->address, UIP_ADDRLIST_PAGE, 0))
	self->hiddenlinkflag = HIDDENLINKS_MERGE;
    else
	self->hiddenlinkflag = LYHiddenLinks;
    self->hidden_links = NULL;
    self->no_cache = ((anchor->no_cache || anchor->post_data) ?
							  YES : NO);
    self->LastChar = '\0';
    self->IgnoreExcess = FALSE;

#ifndef USE_PRETTYSRC
    if (HTOutputFormat == WWW_SOURCE)
	self->source = YES;
    else
	self->source = NO;
#else
    /*  mark_htext_as_source == TRUE if we are parsing html file (and psrc_view is
     *	    set temporary to false at creation time)
     *	psrc_view == TRUE if source of the text produced by some lynx module
     *	    (like ftp browsers) is requested). - VH
     */
    self->source = (BOOL) (LYpsrc ? mark_htext_as_source || psrc_view : HTOutputFormat == WWW_SOURCE);
    mark_htext_as_source = FALSE;
#endif
    HTAnchor_setDocument(anchor, (HyperDoc *)self);
    HTFormNumber = 0;  /* no forms started yet */
    HTMainText = self;
    HTMainAnchor = anchor;
    self->display_on_the_fly = 0;
    self->kcode = NOKANJI;
    self->specified_kcode = NOKANJI;
#ifdef USE_TH_JP_AUTO_DETECT
    self->detected_kcode = DET_NOTYET;
    self->SJIS_status = SJIS_state_neutral;
    self->EUC_status = EUC_state_neutral;
#endif
    self->state = S_text;
    self->kanji_buf = '\0';
    self->in_sjis = 0;
    self->have_8bit_chars = NO;
    HText_getChartransInfo(self);
    UCSetTransParams(&self->T,
		     self->UCLYhndl, self->UCI,
		     current_char_set,
		     &LYCharSet_UC[current_char_set]);

    /*
     *  Check the kcode setting if the anchor has a charset element. - FM
     */
    HText_setKcode(self, anchor->charset,
		   HTAnchor_getUCInfoStage(anchor, UCT_STAGE_HTEXT));

    /*
     *	Memory leak fixed.
     *  05-29-94 Lynx 2-3-1 Garrett Arch Blythe
     *	Check to see if our underline and star_string need initialization
     *		if the underline is not filled with dots.
     */
    if (underscore_string[0] != '.') {
	/*
	 *  Create an array of dots for the UNDERSCORES macro. - FM
	 */
	memset(underscore_string, '.', (MAX_LINE-1));
	underscore_string[(MAX_LINE-1)] = '\0';
	underscore_string[MAX_LINE] = '\0';
	/*
	 *  Create an array of underscores for the STARS macro. - FM
	 */
	memset(star_string, '_', (MAX_LINE-1));
	star_string[(MAX_LINE-1)] = '\0';
	star_string[MAX_LINE] = '\0';
    }

    underline_on = FALSE; /* reset */
    bold_on = FALSE;

#ifdef DISP_PARTIAL
    /*
     * By this function we create HText object
     * so we may start displaying the document while downloading. - LP
     */
    if (display_partial_flag) {
	display_partial = TRUE;	 /* enable HTDisplayPartial() */
	NumOfLines_partial = 0;	 /* initialize */
    }

    /*
     *  These two fields should only be set to valid line numbers
     *  by calls of display_page during partial displaying.  This
     *  is just so that the FIRST display_page AFTER that can avoid
     *  repainting the same lines on the screen. - kw
     */
    self->first_lineno_last_disp_partial =
	self->last_lineno_last_disp_partial = -1;
#endif

#ifdef EXP_JUSTIFY_ELTS
    ht_justify_cleanup();
#endif
    return self;
}

/*			Creation Method 2
**			---------------
**
**      Stream is assumed open and left open.
*/
PUBLIC HText *  HText_new2 ARGS2(
	HTParentAnchor *,	anchor,
	HTStream *,		stream)

{
    HText * this = HText_new(anchor);

    if (stream) {
	this->target = stream;
	this->targetClass = *stream->isa;	/* copy action procedures */
    }
    return this;
}

/*	Free Entire Text
**	----------------
*/
PUBLIC void HText_free ARGS1(
	HText *,	self)
{
    if (!self)
	return;

    HTAnchor_setDocument(self->node_anchor, (HyperDoc *)0);
#if defined(USE_COLOR_STYLE) && !defined(OLD_HTSTYLECHANGE)
    POOL_FREE(HTStyleChangePool,self->styles_pool);
#endif
    while (YES) {	/* Free off line array */
	HTLine * l = self->last_line;
	if (l) {
	    l->next->prev = l->prev;
	    l->prev->next = l->next;	/* Unlink l */
	    self->last_line = l->prev;
	    if (l != self->last_line) {
		FREE(l);
	    } else {
		free(l);
	    }
	}
	if (l == self->last_line) {	/* empty */
	    l = self->last_line = NULL;
	    break;
	}
    }

    while (self->first_anchor) {		/* Free off anchor array */
	TextAnchor * l = self->first_anchor;
	self->first_anchor = l->next;

	if (l->link_type == INPUT_ANCHOR && l->input_field) {
	    /*
	     *  Free form fields.
	     */
	    if (l->input_field->type == F_OPTION_LIST_TYPE &&
		l->input_field->select_list != NULL) {
		/*
		 *  Free off option lists if present.
		 *  It should always be present for F_OPTION_LIST_TYPE
		 *  unless we had invalid markup which prevented
		 *  HText_setLastOptionValue from finishing its job
		 *  and left the input field in an insane state. - kw
		 */
		OptionType *optptr = l->input_field->select_list;
		OptionType *tmp;
		while (optptr) {
		    tmp = optptr;
		    optptr = tmp->next;
		    FREE(tmp->name);
		    FREE(tmp->cp_submit_value);
		    FREE(tmp);
		}
		l->input_field->select_list = NULL;
		/*
		 *  Don't free the value field on option
		 *  lists since it points to a option value
		 *  same for orig value.
		 */
		l->input_field->value = NULL;
		l->input_field->orig_value = NULL;
		l->input_field->cp_submit_value = NULL;
		l->input_field->orig_submit_value = NULL;
	    } else {
		FREE(l->input_field->value);
		FREE(l->input_field->orig_value);
		FREE(l->input_field->cp_submit_value);
		FREE(l->input_field->orig_submit_value);
	    }
	    FREE(l->input_field->name);
	    FREE(l->input_field->submit_action);
	    FREE(l->input_field->submit_enctype);
	    FREE(l->input_field->submit_title);

	    FREE(l->input_field->accept_cs);

	    FREE(l->input_field);
	}

	FREE(l->hightext);
	FREE(l->hightext2);

	FREE(l);
    }
    FormList_delete(self->forms);

    /*
     *  Free the tabs list. - FM
     */
    if (self->tabs) {
	HTTabID * Tab = NULL;
	HTList * cur = self->tabs;

	while (NULL != (Tab = (HTTabID *)HTList_nextObject(cur))) {
	    FREE(Tab->name);
	    FREE(Tab);
	}
	HTList_delete(self->tabs);
	self->tabs = NULL;
    }

    /*
     *  Free the hidden links list. - FM
     */
    if (self->hidden_links) {
	char * href = NULL;
	HTList * cur = self->hidden_links;

	while (NULL != (href = (char *)HTList_nextObject(cur)))
	    FREE(href);
	HTList_delete(self->hidden_links);
	self->hidden_links = NULL;
    }

    /*
     *  Invoke HTAnchor_delete() to free the node_anchor
     *  if it is not a destination of other links. - FM
     */
    if (self->node_anchor) {
	HTAnchor_resetUCInfoStage(self->node_anchor, -1, UCT_STAGE_STRUCTURED,
				  UCT_SETBY_NONE);
	HTAnchor_resetUCInfoStage(self->node_anchor, -1, UCT_STAGE_HTEXT,
				  UCT_SETBY_NONE);
#ifdef SOURCE_CACHE
	/* Remove source cache files and chunks always, even if the
	 * HTAnchor_delete call does not actually remove the anchor.
	 * Keeping them would just be a waste of space - they won't
	 * be used any more after the anchor has been disassociated
	 * from a HText structure. - kw
	 */
	HTAnchor_clearSourceCache(self->node_anchor);
#endif
	if (HTAnchor_delete(self->node_anchor))
	    /*
	     *  Make sure HTMainAnchor won't point
	     *  to an invalid structure. - KW
	     */
	    HTMainAnchor = NULL;
    }

    FREE(self);
}

/*		Display Methods
**		---------------
*/


/*	Output a line
**	-------------
*/
PRIVATE int display_line ARGS4(
	HTLine *,	line,
	HText *,	text,
	int,		scrline GCC_UNUSED,
	CONST char*,	target GCC_UNUSED)
{
    register int i, j;
    char buffer[7];
    char *data;
    size_t utf_extra = 0;
    char LastDisplayChar = ' ';
#ifdef USE_COLOR_STYLE
    int current_style = 0;
#define inunderline NO
#define inbold NO
#else
    BOOL inbold=NO, inunderline=NO;
#endif
#if defined(SHOW_WHEREIS_TARGETS) && !defined(USE_COLOR_STYLE)
    CONST char *cp_tgt;
    int i_start_tgt=0, i_after_tgt;
    int HitOffset, LenNeeded;
    BOOL intarget=NO;
#else
#define intarget NO
#endif /* SHOW_WHEREIS_TARGETS && !USE_COLOR_STYLE */

#ifndef NCURSES_VERSION
    text->has_utf8 = NO; /* use as per-line flag, except with ncurses */
#endif

    /*
     *  Set up the multibyte character buffer,
     *  and clear the line to which we will be
     *  writing.
     */
    buffer[0] = buffer[1] = buffer[2] = '\0';
    clrtoeol();

    /*
     *  Add offset, making sure that we do not
     *  go over the COLS limit on the display.
     */
    j = (int)line->offset;
    if (j > (int)LYcols - 1)
	j = (int)LYcols - 1;
#ifdef USE_SLANG
    SLsmg_forward (j);
    i = j;
#else
#ifdef USE_COLOR_STYLE
    if (line->size == 0)
	i = j;
    else
#endif
    for (i = 0; i < j; i++)
	addch (' ');
#endif /* USE_SLANG */

    /*
     *  Add the data, making sure that we do not
     *  go over the COLS limit on the display.
     */
    data = line->data;
    i++;

#ifndef USE_COLOR_STYLE
#if defined(SHOW_WHEREIS_TARGETS)
    /*
     *  If the target is on this line, it will be emphasized.
     */
    i_after_tgt = i;
    if (target) {
	if (case_sensitive)
	    cp_tgt = LYno_attr_mbcs_strstr(data,
					   target,
					   text->T.output_utf8, YES,
					   &HitOffset,
					   &LenNeeded);
	else
	    cp_tgt = LYno_attr_mbcs_case_strstr(data,
						target,
						text->T.output_utf8, YES,
						&HitOffset,
						&LenNeeded);
	if (cp_tgt) {
	    if (((int)line->offset + LenNeeded) >= LYcols) {
		cp_tgt = NULL;
	    } else {
		text->page_has_target = YES;
		i_start_tgt = i + HitOffset;
		i_after_tgt = i + LenNeeded;
	    }
	}
    } else {
	cp_tgt = NULL;
    }
#endif /* SHOW_WHEREIS_TARGETS */
#endif /* USE_COLOR_STYLE */

    while ((i < LYcols) && ((buffer[0] = *data) != '\0')) {

#ifndef USE_COLOR_STYLE
#if defined(SHOW_WHEREIS_TARGETS)
	if (cp_tgt && i >= i_after_tgt) {
	    if (intarget) {

		if (case_sensitive)
		    cp_tgt = LYno_attr_mbcs_strstr(data,
						target,
						text->T.output_utf8, YES,
						&HitOffset,
						&LenNeeded);
		else
		    cp_tgt = LYno_attr_mbcs_case_strstr(data,
						     target,
						text->T.output_utf8, YES,
						&HitOffset,
						&LenNeeded);
		if (cp_tgt) {
		    i_start_tgt = i + HitOffset;
		    i_after_tgt = i + LenNeeded;
		}
		if (!cp_tgt || i_start_tgt != i) {
		    LYstopTargetEmphasis();
		    intarget = NO;
		    if (inbold)		start_bold();
		    if (inunderline)	start_underline();
		}
	    }
	}
#endif /* SHOW_WHEREIS_TARGETS */
#endif /* USE_COLOR_STYLE */

	data++;

#if defined(USE_COLOR_STYLE) || defined(SLSC)
#define CStyle line->styles[current_style]

	while (current_style < line->numstyles &&
	       i >= (int) (CStyle.horizpos + line->offset + 1))
	{
		LynxChangeStyle (CStyle.style,CStyle.direction,CStyle.previous);
		current_style++;
	}
#endif
	switch (buffer[0]) {

#ifndef USE_COLOR_STYLE
	    case LY_UNDERLINE_START_CHAR:
		if (dump_output_immediately && use_underscore) {
		    addch('_');
		    i++;
		} else {
		    inunderline = YES;
		    if (!intarget) {
#if (defined(DOSPATH) || defined(WIN_EX)) && !defined(USE_SLANG)
			if (LYShowColor == SHOW_COLOR_NEVER)
			    start_bold();
			else
			    start_underline();
#else
			start_underline();
#endif	/* DOSPATH ... */
		    }
		}
		break;

	    case LY_UNDERLINE_END_CHAR:
		if (dump_output_immediately && use_underscore) {
		    addch('_');
		    i++;
		} else {
		    inunderline = NO;
		    if (!intarget) {
#if (defined(DOSPATH) || defined(WIN_EX)) && !defined(USE_SLANG)
		    if (LYShowColor == SHOW_COLOR_NEVER)
			stop_bold();
		    else
			stop_underline();
#else
		    stop_underline();
#endif	/* DOSPATH ... */
		    }
		}
		break;

	    case LY_BOLD_START_CHAR:
		inbold = YES;
		if (!intarget)
		    start_bold();
		break;

	    case LY_BOLD_END_CHAR:
		inbold = NO;
		if (!intarget)
		    stop_bold();
		break;

#endif
	    case LY_SOFT_NEWLINE:
		if (!dump_output_immediately) {
		    addch('+');
		    i++;
		}
		break;

	    case LY_SOFT_HYPHEN:
		if (*data != '\0' ||
		    isspace(UCH(LastDisplayChar)) ||
		    LastDisplayChar == '-') {
		    /*
		     *  Ignore the soft hyphen if it is not the last
		     *  character in the line.  Also ignore it if it
		     *  first character following the margin, or if it
		     *  is preceded by a white character (we loaded 'M'
		     *  into LastDisplayChar if it was a multibyte
		     *  character) or hyphen, though it should have
		     *  been excluded by HText_appendCharacter() or by
		     *  split_line() in those cases. - FM
		     */
		    break;
		} else {
		    /*
		     *  Make it a hard hyphen and fall through. - FM
		     */
		    buffer[0] = '-';
		}
		/* FALLTHRU */

	    default:
#ifndef USE_COLOR_STYLE
#if defined(SHOW_WHEREIS_TARGETS)
		if (!intarget && cp_tgt && i >= i_start_tgt) {
		    /*
		     *  Start the emphasis.
		     */
		    if (data > cp_tgt) {
			LYstartTargetEmphasis();
			intarget = YES;
		    }
		}
#endif /* SHOW_WHEREIS_TARGETS */
#endif /* USE_COLOR_STYLE */
		i++;
		if (text->T.output_utf8 && !isascii(UCH(buffer[0]))) {
		    text->has_utf8 = YES;
		    if ((*buffer & 0xe0) == 0xc0) {
			utf_extra = 1;
		    } else if ((*buffer & 0xf0) == 0xe0) {
			utf_extra = 2;
		    } else if ((*buffer & 0xf8) == 0xf0) {
			utf_extra = 3;
		    } else if ((*buffer & 0xfc) == 0xf8) {
			utf_extra = 4;
		    } else if ((*buffer & 0xfe) == 0xfc) {
			utf_extra = 5;
		    } else {
			 /*
			  *  Garbage.
			  */
			utf_extra = 0;
		    }
		    if (strlen(data) < utf_extra) {
			/*
			 *  Shouldn't happen.
			 */
			utf_extra = 0;
		    }
		    LastDisplayChar = 'M';
		}
		if (utf_extra) {
		    strncpy(&buffer[1], data, utf_extra);
		    buffer[utf_extra+1] = '\0';
		    addstr(buffer);
		    buffer[1] = '\0';
		    data += utf_extra;
		    utf_extra = 0;
		} else if (HTCJK != NOCJK && !isascii(UCH(buffer[0]))
#ifndef CONV_JISX0201KANA_JISX0208KANA
		    && kanji_code != SJIS
#endif
		) {
		    /*
		     *  For CJK strings, by Masanobu Kimura.
		     */
		    if (i >= LYcols) goto after_while;

		    buffer[1] = *data;
		    buffer[2] = '\0';
		    data++;
		    i++;
		    addstr(buffer);
		    buffer[1] = '\0';
		    /*
		     *  For now, load 'M' into LastDisplayChar,
		     *  but we should check whether it's white
		     *  and if so, use ' '.  I don't know if
		     *  there actually are white CJK characters,
		     *  and we're loading ' ' for multibyte
		     *  spacing characters in this code set,
		     *  but this will become an issue when
		     *  the development code set's multibyte
		     *  character handling is used. - FM
		     */
		    LastDisplayChar = 'M';
		} else {
#if 0	/* last-ditch attempt to prevent 0x9B to screen - disabled  */
#if defined(UNIX) || defined(VMS)
		    if (!dump_output_immediately &&
			UCH(buffer[0]) == 128+27) {
			addstr("~^");
			buffer[0] ^= 0xc0;
		    }
#endif
#endif
		    addstr(buffer);
		    LastDisplayChar = buffer[0];
		}
	} /* end of switch */
    } /* end of while */

after_while:
#if !defined(NCURSES_VERSION)
    if (text->has_utf8) {
	LYtouchline(scrline);
	text->has_utf8 = NO;	/* we had some, but have dealt with it. */
    }
#endif
    /*
     *  Add the return.
     */
    addch('\n');

#if defined(SHOW_WHEREIS_TARGETS) && !defined(USE_COLOR_STYLE)
    if (intarget)
	LYstopTargetEmphasis();
#else
#undef intarget
#endif /* SHOW_WHEREIS_TARGETS && !USE_COLOR_STYLE */
#ifndef USE_COLOR_STYLE
    stop_underline();
    stop_bold();
#else
    while (current_style < line->numstyles)
    {
	LynxChangeStyle (CStyle.style, CStyle.direction, CStyle.previous);
	current_style++;
    }
#undef CStyle
#endif
    return(0);
}

/*	Output the title line
**	---------------------
*/
PRIVATE void display_title ARGS1(
	HText *,	text)
{
    char *title = NULL;
    char percent[20];
    char *cp = NULL;
    unsigned char *tmp = NULL;
    int i = 0, j = 0;

    /*
     *  Make sure we have a text structure. - FM
     */
    if (!text)
	return;

    lynx_start_title_color ();
#ifdef USE_COLOR_STYLE
/* turn the TITLE style on */
    if (last_colorattr_ptr > 0) {
	LynxChangeStyle(s_title, STACK_ON, 0);
    } else {
	LynxChangeStyle(s_title, ABS_ON, 0);
    }
#endif /* USE_COLOR_STYLE */

    /*
     *  Load the title field. - FM
     */
    StrAllocCopy(title,
		 (HTAnchor_title(text->node_anchor) ?
		  HTAnchor_title(text->node_anchor) : " "));	/* "" -> " " */

    /*
     *  There shouldn't be any \n in the title field,
     *  but if there is, lets kill it now.  Also trim
     *  any trailing spaces. - FM
     */
    if ((cp = strchr(title,'\n')) != NULL)
	*cp = '\0';
    i = (*title ? (strlen(title) - 1) : 0);
    while ((i >= 0) && title[i] == ' ')
	title[i--] = '\0';

    /*
     *  Generate the page indicator (percent) string.
     */
    if (LYcols < 10) {
	percent[0] = '\0';	/* Null string */
    } else if ((display_lines) <= 0 && LYlines > 0 &&
	text->top_of_screen <= 99999 && text->Lines <= 999999) {
	sprintf(percent, " (l%d of %d)",
		text->top_of_screen, text->Lines);
    } else if ((text->Lines + 1) > (display_lines) &&
	(display_lines) > 0) {
	/*
	 *  In a small attempt to correct the number of pages counted....
	 *    GAB 07-14-94
	 *
	 *  In a bigger attempt (hope it holds up 8-)....
	 *    FM 02-08-95
	 */
	int total_pages =
		(((text->Lines + 1) + (display_lines - 1))/(display_lines));
	int start_of_last_page =
		((text->Lines + 1) < display_lines) ? 0 :
		((text->Lines + 1) - display_lines);

	sprintf(percent, " (p%d of %d)",
		((text->top_of_screen >= start_of_last_page) ?
						 total_pages :
		    ((text->top_of_screen + display_lines)/(display_lines))),
		total_pages);
    } else {
	percent[0] = '\0';	/* Null string */
    }

    /*
     *  Generate and display the title string, with page indicator
     *  if appropriate, preceded by the toolbar token if appropriate,
     *  and truncated if necessary. - FM & KW
     */
    if (HTCJK != NOCJK) {
	if (*title &&
	    (tmp = typecallocn(unsigned char, (strlen(title) + 256)))) {
	    if (kanji_code == EUC) {
		TO_EUC((unsigned char *)title, tmp);
	    } else if (kanji_code == SJIS) {
		TO_SJIS((unsigned char *)title, tmp);
	    } else {
		for (i = 0, j = 0; title[i]; i++) {
		    if (title[i] != CH_ESC) {  /* S/390 -- gil -- 1487 */
			tmp[j++] = title[i];
		    }
		}
		tmp[j] = '\0';
	    }
	    StrAllocCopy(title, (CONST char *)tmp);
	    FREE(tmp);
	}
    }
    move(0, 0);
    clrtoeol();
#if defined(SH_EX) && defined(KANJI_CODE_OVERRIDE)
    addstr(str_kcode(last_kcode));
#endif
    if (text->top_of_screen > 0 && HText_hasToolbar(text)) {
	addch('#');
    }
    i = (LYcols - 1) - strlen(percent) - strlen(title);
    if (i >= CHAR_WIDTH) {
	move(0, i);
    } else {
	/*
	 *  Note that this truncation is not taking into
	 *  account the possibility that multibyte
	 *  characters might be present. - FM
	 */
#ifdef SH_EX	/* 1999/06/15 (Tue) 10:17:28 */
	int last;
	last = (int)strlen(percent) + CHAR_WIDTH;
	if (LYcols - 3 >= last) {
	    title[(LYcols - 3) - last] = '.';
	    title[(LYcols - 2) - last] = '.';
	    title[(LYcols - 1) - last] = '\0';
	} else {
	    title[(LYcols - 1) - last] = '\0';
	}
#else
	if ((i = ((LYcols - 2) - strlen(percent)) - CHAR_WIDTH) >= 0)
	    title[i] = '\0';
#endif
	move(0, CHAR_WIDTH);
    }
    addstr(title);
    if (percent[0] != '\0')
	addstr(percent);
    addch('\n');
    FREE(title);

#ifdef USE_COLOR_STYLE
/* turn the TITLE style off */
    LynxChangeStyle(s_title, STACK_OFF, 0);
#endif /* USE_COLOR_STYLE */
    lynx_stop_title_color ();

    return;
}

/*	Output the scrollbar
**	---------------------
*/
#ifdef USE_SCROLLBAR
PRIVATE void display_scrollbar ARGS1(
	HText *,	text)
{
    int i;
    int h = display_lines - 2 * (LYsb_arrow!=0); /* Height of the scrollbar */
    int off = (LYsb_arrow != 0);		 /* Start of the scrollbar */
    int top_skip, bot_skip, sh, shown;

    LYsb_begin = LYsb_end = -1;
    if (!LYsb || !text || h <= 2
	|| (text->Lines + 1) <= display_lines)
	return;

    if (text->top_of_screen >= text->Lines + 1 - display_lines) {
	/* Only part of the screen shows actual text */
	shown = text->Lines + 1 - text->top_of_screen;

	if (shown <= 0)
	    shown = 1;
    } else
	shown = display_lines;
    /* Each cell of scrollbar represents text->Lines/h lines of text. */
    /* Always smaller than h */
    sh = (shown*h + text->Lines/2)/(text->Lines + 1);
    if (sh <= 0)
	sh = 1;
    if (sh >= h - 1)
	sh = h - 2;		/* Position at ends indicates BEG and END */

    if (text->top_of_screen == 0)
	top_skip = 0;
    else if (text->Lines - (text->top_of_screen + display_lines - 1) <= 0)
	top_skip = h - sh;
    else {
	/* text->top_of_screen between 1 and text->Lines - display_lines
	   corresponds to top_skip between 1 and h - sh - 1 */
	/* Use rounding to get as many positions into top_skip==h - sh - 1
	   as into top_skip == 1:
	   1--->1, text->Lines - display_lines + 1--->h - sh. */
	top_skip = 1 +
	    1. * (h - sh - 1) * text->top_of_screen
		/(text->Lines - display_lines + 1);
    }
    bot_skip = h - sh - top_skip;

    LYsb_begin = top_skip;
    LYsb_end = h - bot_skip;

    if (LYsb_arrow) {
#ifdef USE_COLOR_STYLE
	int s = top_skip ? s_sb_aa : s_sb_naa;

	if (last_colorattr_ptr > 0) {
	    LynxChangeStyle(s, STACK_ON, 0);
	} else {
	    LynxChangeStyle(s, ABS_ON, 0);
	}
#endif /* USE_COLOR_STYLE */
	move(1, LYcols - 1);
	addch_raw(ACS_UARROW);
#ifdef USE_COLOR_STYLE
	LynxChangeStyle(s, STACK_OFF, 0);
#endif /* USE_COLOR_STYLE */
    }
#ifdef USE_COLOR_STYLE
    if (last_colorattr_ptr > 0) {
	LynxChangeStyle(s_sb_bg, STACK_ON, 0);
    } else {
	LynxChangeStyle(s_sb_bg, ABS_ON, 0);
    }
#endif /* USE_COLOR_STYLE */

    for (i=1; i <= h; i++) {
#ifdef USE_COLOR_STYLE
	if (i-1 <= top_skip && i > top_skip)
	    LynxChangeStyle(s_sb_bar, STACK_ON, 0);
	if (i-1 <= h - bot_skip && i > h - bot_skip)
	    LynxChangeStyle(s_sb_bar, STACK_OFF, 0);
#endif /* USE_COLOR_STYLE */
	move(i + off, LYcols - 1);
	if (i > top_skip && i <= h - bot_skip)
	    addch(ACS_BLOCK);
	else
	    addch(ACS_CKBOARD);
    }
#ifdef USE_COLOR_STYLE
    LynxChangeStyle(s_sb_bg, STACK_OFF, 0);
#endif /* USE_COLOR_STYLE */

    if (LYsb_arrow) {
#ifdef USE_COLOR_STYLE
	int s = bot_skip ? s_sb_aa : s_sb_naa;

	if (last_colorattr_ptr > 0) {
	    LynxChangeStyle(s, STACK_ON, 0);
	} else {
	    LynxChangeStyle(s, ABS_ON, 0);
	}
#endif /* USE_COLOR_STYLE */
	move(h + 2, LYcols - 1);
	addch_raw(ACS_DARROW);
#ifdef USE_COLOR_STYLE
	LynxChangeStyle(s, STACK_OFF, 0);
#endif /* USE_COLOR_STYLE */
    }
    return;
}
#else
#define display_scrollbar(text) /*nothing*/
#endif /* USE_SCROLLBAR */

/*	Output a page
**	-------------
*/
PRIVATE void display_page ARGS3(
	HText *,	text,
	int,		line_number,
	char *,		target)
{
    HTLine * line = NULL;
    int i;
#if defined(USE_COLOR_STYLE) && defined(SHOW_WHEREIS_TARGETS)
    char *cp;
#endif
    char tmp[7];
    int last_screen;
    TextAnchor *Anchor_ptr = NULL;
    int stop_before_for_anchors;
    FormInfo *FormInfo_ptr;
    BOOL display_flag = FALSE;
    HTAnchor *link_dest;
    HTAnchor *link_dest_intl = NULL;
    static int last_nlinks = 0;
    static int charset_last_displayed = -1;
#ifdef DISP_PARTIAL
    int last_disp_partial = -1;
#endif

    lynx_mode = NORMAL_LYNX_MODE;

    if (text == NULL) {
	/*
	 *  Check whether to force a screen clear to enable scrollback,
	 *  or as a hack to fix a reverse clear screen problem for some
	 *  curses packages. - shf@access.digex.net & seldon@eskimo.com
	 */
	if (enable_scrollback) {
	    addch('*');
	    refresh();
	    clear();
	}
	addstr("\n\nError accessing document!\nNo data available!\n");
	refresh();
	nlinks = 0;  /* set number of links to 0 */
	return;
    }

#ifdef DISP_PARTIAL
    if (display_partial || recent_sizechange || text->stale) {
	/*  Reset them, will be set near end if all is okay. - kw */
	text->first_lineno_last_disp_partial =
	    text->last_lineno_last_disp_partial = -1;
    }
#endif /* DISP_PARTIAL */

    tmp[0] = tmp[1] = tmp[2] = '\0';
    if (target && *target == '\0') target = NULL;
    text->page_has_target = NO;
    if (display_lines <= 0) {
	/*  No screen space to display anything!
	 *  returning here makes it more likely we will survive if
	 *  an xterm is temporarily made very small. - kw */
	return;
    }
    last_screen = text->Lines - (display_lines - 2);
    line = text->last_line->prev;

    /*
     *  Constrain the line number to be within the document.
     */
    if (text->Lines < (display_lines))
	line_number = 0;
    else if (line_number > text->Lines)
	line_number = last_screen;
    else if (line_number < 0)
	line_number = 0;

    for (i = 0, line = text->last_line->next;		/* Find line */
	 i < line_number && (line != text->last_line);
	 i++, line = line->next) {			/* Loop */
#ifndef VMS
	if (!LYNoCore) {
	    assert(line->next != NULL);
	} else if (line->next == NULL) {
	    if (enable_scrollback) {
		addch('*');
		refresh();
		clear();
	    }
	    addstr("\n\nError drawing page!\nBad HText structure!\n");
	    refresh();
	    nlinks = 0;  /* set number of links to 0 */
	    return;
	}
#else
	assert(line->next != NULL);
#endif /* !VMS */
    } /* Loop */

    if (LYlowest_eightbit[current_char_set] <= 255 &&
	(current_char_set != charset_last_displayed) &&
	/*
	 *  current_char_set has changed since last invocation,
	 *  and it's not just 7-bit.
	 *  Also we don't want to do this for -dump and -source etc.
	 */
	LYCursesON) {
	charset_last_displayed = current_char_set;
#ifdef EXP_CHARTRANS_AUTOSWITCH
#ifdef LINUX
	/*
	 *  Currently implemented only for LINUX
	 */
	UCChangeTerminalCodepage(current_char_set,
				 &LYCharSet_UC[current_char_set]);
#endif /* LINUX */
#endif /* EXP_CHARTRANS_AUTOSWITCH */
    }

    /*
     *  Check whether to force a screen clear to enable scrollback,
     *  or as a hack to fix a reverse clear screen problem for some
     *  curses packages. - shf@access.digex.net & seldon@eskimo.com
     */
    if (enable_scrollback) {
	addch('*');
	refresh();
	clear();
    }

#ifdef USE_COLOR_STYLE
    /*
     *  Reset stack of color attribute changes to avoid color leaking,
     *  except if what we last displayed from this text was the previous
     *  screenful, in which case carrying over the state might be beneficial
     *  (although it shouldn't generally be needed any more). - kw
     */
    if (text->stale ||
	line_number != text->top_of_screen + (display_lines)) {
	last_colorattr_ptr = 0;
    }
#endif

    text->top_of_screen = line_number;
    text->top_of_screen_line = line;
    display_title(text);  /* will move cursor to top of screen */
    display_flag=TRUE;

#ifdef USE_COLOR_STYLE
#ifdef DISP_PARTIAL
    if (display_partial ||
	line_number != text->first_lineno_last_disp_partial ||
	line_number > text->last_lineno_last_disp_partial)
#endif /* DISP_PARTIAL */
    LynxResetScreenCache();
#endif /* USE_COLOR_STYLE */

#ifdef DISP_PARTIAL
    if (display_partial && text->stbl) {
	stop_before_for_anchors = Stbl_getStartLine(text->stbl);
	if (stop_before_for_anchors > line_number+(display_lines))
	    stop_before_for_anchors = line_number+(display_lines);
    } else
#endif
	stop_before_for_anchors = line_number+(display_lines);

    /*
     *  Output the page.
     */
    if (line) {
#if defined(USE_COLOR_STYLE) && defined(SHOW_WHEREIS_TARGETS)
	char *data;
	int offset, LenNeeded;
#endif
#ifdef DISP_PARTIAL
	if (display_partial ||
	    line_number != text->first_lineno_last_disp_partial)
	    text->has_utf8 = NO;
#else
	text->has_utf8 = NO;
#endif
	for (i = 0; i < (display_lines); i++)  {
	    /*
	     *  Verify and display each line.
	     */
#ifndef VMS
	    if (!LYNoCore) {
		assert(line != NULL);
	    } else if (line == NULL) {
		if (enable_scrollback) {
		    addch('*');
		    refresh();
		    clear();
		}
		addstr("\n\nError drawing page!\nBad HText structure!\n");
		refresh();
		nlinks = 0;  /* set number of links to 0 */
		return;
	    }
#else
	    assert(line != NULL);
#endif /* !VMS */

#ifdef DISP_PARTIAL
	    if (!display_partial &&
		line_number == text->first_lineno_last_disp_partial &&
		i + line_number <= text->last_lineno_last_disp_partial)
		move((i + 2), 0);
	    else
#endif
	    display_line(line, text, i+1, target);

#if defined(SHOW_WHEREIS_TARGETS)
#ifdef USE_COLOR_STYLE		/* otherwise done in display_line - kw */
	    /*
	     *  If the target is on this line, recursively
	     *  seek and emphasize it. - FM
	     */
	    data = (char *)line->data;
	    offset = (int)line->offset;
	    while ((target && *target) &&
		   (case_sensitive ?
		    (cp = LYno_attr_mbcs_strstr(data,
						target,
						text->T.output_utf8, YES,
						NULL,
						&LenNeeded)) != NULL :
		    (cp = LYno_attr_mbcs_case_strstr(data,
						     target,
						text->T.output_utf8, YES,
						NULL,
						&LenNeeded)) != NULL) &&
		   ((int)line->offset + LenNeeded) < LYcols) {
		int itmp = 0;
		int written = 0;
		int x_pos = offset + (int)(cp - data);
		int len = strlen(target);
		size_t utf_extra = 0;
		int y;

		text->page_has_target = YES;

		/*
		 *  Start the emphasis.
		 */
		LYstartTargetEmphasis();

		/*
		 *  Output the target characters.
		 */
		for (;
		     written < len && (tmp[0] = data[itmp]) != '\0';
		     itmp++)  {
		    if (IsSpecialAttrChar(tmp[0]) && tmp[0] != LY_SOFT_NEWLINE) {
			/*
			 *  Ignore special characters.
			 */
			x_pos--;

		    } else if (&data[itmp] >= cp) {
			if (cp == &data[itmp]) {
			    /*
			     *  First printable character of target.
			     */
			    move((i + 1), x_pos);
			}
			/*
			 *  Output all the printable target chars.
			 */
			if (text->T.output_utf8 && !isascii(UCH(tmp[0]))) {
			    if ((*tmp & 0xe0) == 0xc0) {
				utf_extra = 1;
			    } else if ((*tmp & 0xf0) == 0xe0) {
				utf_extra = 2;
			    } else if ((*tmp & 0xf8) == 0xf0) {
				utf_extra = 3;
			    } else if ((*tmp & 0xfc) == 0xf8) {
				utf_extra = 4;
			    } else if ((*tmp & 0xfe) == 0xfc) {
				utf_extra = 5;
			    } else {
				/*
				 *  Garbage.
				 */
				utf_extra = 0;
			    }
			    if (strlen(&line->data[itmp+1]) < utf_extra) {
				/*
				 *  Shouldn't happen.
				 */
				utf_extra = 0;
			    }
			}
			if (utf_extra) {
			    strncpy(&tmp[1], &line->data[itmp+1], utf_extra);
			    tmp[utf_extra+1] = '\0';
			    itmp += utf_extra;
			    addstr(tmp);
			    tmp[1] = '\0';
			    written += (utf_extra + 1);
			    utf_extra = 0;
			} else if (HTCJK != NOCJK && !isascii(UCH(tmp[0]))) {
			    /*
			     *  For CJK strings, by Masanobu Kimura.
			     */
			    tmp[1] = data[++itmp];
			    addstr(tmp);
			    tmp[1] = '\0';
			    written += 2;
			} else {
#if 0	/* last-ditch attempt to prevent 0x9B to screen - disabled  */
#if defined(UNIX) || defined(VMS)
			    if (!dump_output_immediately &&
				UCH(tmp[0]) == 128+27) {
				addstr("~^");
				tmp[0] ^= 0xc0;
			    }
#endif
#endif
			    addstr(tmp);
			    written++;
			}
		    }
		}

		/*
		 *  Stop the emphasis, and reset the offset and
		 *  data pointer for our current position in the
		 *  line. - FM
		 */
		LYstopTargetEmphasis();
		LYGetYX(y, offset);
		data = (char *)&data[itmp];

		/*
		 *  Adjust the cursor position, should we be at
		 *  the end of the line, or not have another hit
		 *  in it. - FM
		 */
		move((i + 2), 0);
	    } /* end while */
#endif /* USE_COLOR_STYLE */
#endif /* SHOW_WHEREIS_TARGETS */

	    /*
	     *  Stop if this is the last line.  Otherwise, make sure
	     *  display_flag is set and process the next line. - FM
	     */
	    if (line == text->last_line) {
		/*
		 *  Clear remaining lines of display.
		 */
		for (i++; i < (display_lines); i++) {
		    move((i + 1), 0);
		    clrtoeol();
		}
		break;
	    }
#ifdef DISP_PARTIAL
	    if (display_partial) {
		/*
		 *  Remember as fully shown during last partial display,
		 *  if it was not the last text line. - kw
		 */
		last_disp_partial = i + line_number;
	    }
#endif /* DISP_PARTIAL */
	    display_flag = TRUE;
	    line = line->next;
	} /* end of "Verify and display each line." loop */
    } /* end "Output the page." */

    text->next_line = line;	/* Line after screen */
    text->stale = NO;		/* Display is up-to-date */

    /*
     *  Add the anchors to Lynx structures.
     */
    nlinks = 0;
    for (Anchor_ptr=text->first_anchor;  Anchor_ptr != NULL &&
		Anchor_ptr->line_num <= stop_before_for_anchors;
					    Anchor_ptr = Anchor_ptr->next) {

	if (Anchor_ptr->line_num >= line_number &&
		Anchor_ptr->line_num < stop_before_for_anchors) {
	    /*
	     *  Load normal hypertext anchors.
	     */
	    if (Anchor_ptr->show_anchor && Anchor_ptr->hightext &&
			strlen(Anchor_ptr->hightext) > 0 &&
			(Anchor_ptr->link_type & HYPERTEXT_ANCHOR)) {

		links[nlinks].hightext	= Anchor_ptr->hightext;
		links[nlinks].hightext2 = Anchor_ptr->hightext2;
		links[nlinks].hightext2_offset = Anchor_ptr->hightext2offset;
		links[nlinks].inUnderline = Anchor_ptr->inUnderline;

		links[nlinks].anchor_number = Anchor_ptr->number;
		links[nlinks].anchor_line_num = Anchor_ptr->line_num;

		link_dest = HTAnchor_followMainLink(
					     (HTAnchor *)Anchor_ptr->anchor);
		{
		    /*
		     *	Memory leak fixed 05-27-94
		     *	Garrett Arch Blythe
		     */
		    auto char *cp_AnchorAddress = NULL;
		    if (traversal)
			cp_AnchorAddress = stub_HTAnchor_address(link_dest);
		    else {
#ifndef DONT_TRACK_INTERNAL_LINKS
			if (Anchor_ptr->link_type == INTERNAL_LINK_ANCHOR) {
			    link_dest_intl = HTAnchor_followTypedLink(
				(HTAnchor *)Anchor_ptr->anchor, LINK_INTERNAL);
			    if (link_dest_intl && link_dest_intl != link_dest) {

				CTRACE((tfp,
		    "GridText: display_page: unexpected typed link to %s!\n",
					    link_dest_intl->parent->address));
				link_dest_intl = NULL;
			    }
			} else
			    link_dest_intl = NULL;
			if (link_dest_intl) {
			    char *cp2 = HTAnchor_address(link_dest_intl);
			    cp_AnchorAddress = cp2;
			} else
#endif
			    cp_AnchorAddress = HTAnchor_address(link_dest);
		    }
		    FREE(links[nlinks].lname);

		    if (cp_AnchorAddress != NULL)
			links[nlinks].lname = cp_AnchorAddress;
		    else
			StrAllocCopy(links[nlinks].lname, empty_string);
		}

		links[nlinks].lx = Anchor_ptr->line_pos;
		links[nlinks].ly = ((Anchor_ptr->line_num + 1) - line_number);
		if (link_dest_intl)
		    links[nlinks].type = WWW_INTERN_LINK_TYPE;
		else
		    links[nlinks].type = WWW_LINK_TYPE;
		links[nlinks].target = empty_string;
		links[nlinks].form = NULL;

		nlinks++;
		display_flag = TRUE;

	    } else if (Anchor_ptr->link_type == INPUT_ANCHOR
			&& Anchor_ptr->input_field->type != F_HIDDEN_TYPE) {
		/*
		 *  Handle form fields.
		 */
		lynx_mode = FORMS_LYNX_MODE;

		FormInfo_ptr = Anchor_ptr->input_field;

		links[nlinks].anchor_number = Anchor_ptr->number;
		links[nlinks].anchor_line_num = Anchor_ptr->line_num;

		links[nlinks].form = FormInfo_ptr;
		links[nlinks].lx = Anchor_ptr->line_pos;
		links[nlinks].ly = ((Anchor_ptr->line_num + 1) - line_number);
		links[nlinks].type = WWW_FORM_LINK_TYPE;
		links[nlinks].inUnderline = Anchor_ptr->inUnderline;
		links[nlinks].target = empty_string;
		StrAllocCopy(links[nlinks].lname, empty_string);

		if (FormInfo_ptr->type == F_RADIO_TYPE) {
		    if (FormInfo_ptr->num_value)
			links[nlinks].hightext = checked_radio;
		    else
			links[nlinks].hightext = unchecked_radio;

		} else if (FormInfo_ptr->type == F_CHECKBOX_TYPE) {
		    if (FormInfo_ptr->num_value)
			links[nlinks].hightext = checked_box;
		    else
			links[nlinks].hightext = unchecked_box;

		} else if (FormInfo_ptr->type == F_PASSWORD_TYPE) {
		    links[nlinks].hightext = STARS(strlen(FormInfo_ptr->value));

		} else {  /* TEXT type */
		    links[nlinks].hightext = FormInfo_ptr->value;
		}

		/*
		 *  Never a second line on form types.
		 */
		links[nlinks].hightext2 = NULL;
		links[nlinks].hightext2_offset = 0;

		nlinks++;
		/*
		 *  Bold the link after incrementing nlinks.
		 */
		highlight(OFF, (nlinks - 1), target);

		display_flag = TRUE;

	    } else {
		/*
		 *  Not showing anchor.
		 */
		if (Anchor_ptr->hightext && *Anchor_ptr->hightext)
		    CTRACE((tfp,
			    "\nGridText: Not showing link, hightext=%s\n",
			    Anchor_ptr->hightext));
	    }
	}

	if (Anchor_ptr == text->last_anchor)
	    /*
	     *  No more links in document. - FM
	     */
	    break;

	if (nlinks == MAXLINKS) {
	    /*
	     *  Links array is full.  If interactive, tell user
	     *  to use half-page or two-line scrolling. - FM
	     */
	    if (LYCursesON) {
		HTAlert(MAXLINKS_REACHED);
	    }
	    CTRACE((tfp, "\ndisplay_page: MAXLINKS reached.\n"));
	    break;
	}
    } /* end of loop "Add the anchors to Lynx structures." */

    /*
     *  Free any un-reallocated links[] entries
     *  from the previous page draw. - FM
     */
    for (i = nlinks; i < last_nlinks; i++)
	FREE(links[i].lname);
    last_nlinks = nlinks;

    /*
     *  If Anchor_ptr is not NULL and is not pointing to the last
     *  anchor, then there are anchors farther down in the document,
     *  and we need to flag this for traversals.
     */
    more_links = FALSE;
    if (traversal && Anchor_ptr) {
	if (Anchor_ptr->next)
	    more_links = TRUE;
    }

    if (!display_flag) {
	/*
	 *  Nothing on the page.
	 */
	addstr("\n     Document is empty");
    }
    display_scrollbar(text);

#ifdef DISP_PARTIAL
    if (display_partial && display_flag &&
	last_disp_partial >= text->top_of_screen &&
	!enable_scrollback &&
	!recent_sizechange) {	/*  really remember them if ok - kw  */
	text->first_lineno_last_disp_partial = text->top_of_screen;
	text->last_lineno_last_disp_partial = last_disp_partial;
    } else {
	text->first_lineno_last_disp_partial =
	    text->last_lineno_last_disp_partial = -1;
    }
#endif /* DISP_PARTIAL */

    if (text->has_utf8 || text->had_utf8) {
	/*
	 *  For other than ncurses, repainting is taken care of
	 *  by touching lines in display_line and highlight. - kw 1999-10-07
	 */
	text->had_utf8 = text->has_utf8;
	clearok(curscr, TRUE);
    } else if (HTCJK != NOCJK) {
	/*
	 *  For non-multibyte curses.
	 *
	 *  Full repainting is necessary, otherwise only part of a multibyte 
	 *  character sequence might be written because of curses output 
	 *  optimizations. 
	 */
	clearok(curscr, TRUE); 
    }
    refresh();
}


/*			Object Building methods
**			-----------------------
**
**	These are used by a parser to build the text in an object
*/
PUBLIC void HText_beginAppend ARGS1(
	HText *,	text)
{
    text->permissible_split = 0;
    text->in_line_1 = YES;

}


/* LYcols_cu is the notion that the display library has of the screen
   width.  Normally it is the same as LYcols, but there may be a
   difference via SLANG_MBCS_HACK.  Checks of the line length (as the
   non-UTF-8-aware display library would see it) against LYcols_cu are
   is used to try to prevent that lines with UTF-8 chars get wrapped
   by the library when they shouldn't.
   If there is no display library involved, i.e. dump_output_immediately,
   no such limit should be imposed.  LYcols*6 should be just as good
   as any other large value.  (But don't use INT_MAX or something close
   to it to, avoid over/underflow.) - kw */
#ifdef USE_SLANG
#define LYcols_cu (dump_output_immediately ? LYcols*6 : SLtt_Screen_Cols)
#else
#define LYcols_cu (dump_output_immediately ? LYcols*6 : LYcols)
#endif

/*	Add a new line of text
**	----------------------
**
** On entry,
**
**	split	is zero for newline function, else number of characters
**		before split.
**	text->display_on_the_fly
**		may be set to indicate direct output of the finished line.
** On exit,
**		A new line has been made, justified according to the
**		current style.  Text after the split (if split nonzero)
**		is taken over onto the next line.
**
**		If display_on_the_fly is set, then it is decremented and
**		the finished line is displayed.
*/
#define new_line(text) split_line(text, 0)

#define DEBUG_SPLITLINE

#ifdef DEBUG_SPLITLINE
#define CTRACE_SPLITLINE(p)	CTRACE(p)
#else
#define CTRACE_SPLITLINE(p)	/*nothing*/
#endif

PRIVATE void split_line ARGS2(
	HText *,	text,
	unsigned,	split)
{
    HTStyle * style = text->style;
    HTLine * temp;
    int spare;
#if defined(USE_COLOR_STYLE)
    int inew;
#endif
    int indent = text->in_line_1 ?
	  text->style->indent1st : text->style->leftIndent;
    short alignment;
    TextAnchor * a;
    int CurLine = text->Lines;
    int HeadTrim = 0;
    int SpecialAttrChars = 0;
    int TailTrim = 0;
    int s;

    /*
     *  Make new line.
     */
    HTLine * previous = text->last_line;
    int ctrl_chars_on_previous_line = 0;
    int utfxtra_on_previous_line = utfxtra_on_this_line;
    char * cp;
    /* can't wrap in middle of multibyte sequences, so allocate 2 extra */
    HTLine * line = (HTLine *)LY_CALLOC(1, LINE_SIZE(MAX_LINE)+2);
    if (line == NULL)
	outofmem(__FILE__, "split_line_1");
#if defined(USE_COLOR_STYLE) && !defined(OLD_HTSTYLECHANGE)
    line->styles = stylechanges_buffers[stylechanges_buffers_free = (stylechanges_buffers_free + 1) &1];
#endif
    ctrl_chars_on_this_line = 0; /*reset since we are going to a new line*/
    utfxtra_on_this_line = 0;	/*reset too, we'll count them*/
    text->LastChar = ' ';

#ifdef DEBUG_APPCH
    CTRACE((tfp,"GridText: split_line(%p,%d) called\n", text, split));
    CTRACE((tfp,"   bold_on=%d, underline_on=%d\n", bold_on, underline_on));
#endif

    cp = previous->data;
    if (cp[0] == LY_BOLD_START_CHAR
     || cp[0] == LY_UNDERLINE_START_CHAR) {
	switch (cp[1]) {
	    case LY_SOFT_NEWLINE:
		cp[1] = cp[0];
		cp[0] = LY_SOFT_NEWLINE;
		break;
	    case LY_BOLD_START_CHAR:
	    case LY_UNDERLINE_START_CHAR:
		if (cp[2] == LY_SOFT_NEWLINE) {
		    cp[2] = cp[1];
		    cp[1] = cp[0];
		    cp[0] = LY_SOFT_NEWLINE;
		}
		break;
	}
    }
    if (split > previous->size) {
	CTRACE((tfp,
	       "*** split_line: split==%d greater than last_line->size==%d !\n",
	       split, previous->size));
	if (split > MAX_LINE) {
	    split = previous->size;
	    if ((cp = strrchr(previous->data, ' ')) &&
		cp - previous->data > 1)
		split = cp - previous->data;
	    CTRACE((tfp, "                split adjusted to %d.\n", split));
	}
    }

    text->Lines++;

    previous->next->prev = line;
    line->prev = previous;
    line->next = previous->next;
    previous->next = line;
    text->last_line = line;
    line->size = 0;
    line->offset = 0;
    text->permissible_split = 0;  /* 12/13/93 */
    line->data[0] = '\0';

    /*
     *  If we are not splitting and need an underline char, add it now. - FM
     */
    if ((split < 1) &&
	!(dump_output_immediately && use_underscore) && underline_on) {
	line->data[line->size++] = LY_UNDERLINE_START_CHAR;
	line->data[line->size] = '\0';
	ctrl_chars_on_this_line++;
    }
    /*
     *  If we are not splitting and need a bold char, add it now. - FM
     */
    if ((split < 1) && bold_on) {
	line->data[line->size++] = LY_BOLD_START_CHAR;
	line->data[line->size] = '\0';
	ctrl_chars_on_this_line++;
    }

    alignment = style->alignment;
    /*
     *  Split at required point
     */
    if (split > 0) {	/* Delete space at "split" splitting line */
	char *p, *prevdata = previous->data, *linedata = line->data;
	unsigned plen;
	int i;

	/*
	 *  Split the line. - FM
	 */
	prevdata[previous->size] = '\0';
	previous->size = split;

	/*
	 *  Trim any spaces or soft hyphens from the beginning
	 *  of our new line. - FM
	 */
	p = prevdata + split;
	while ((
#ifdef EXP_JUSTIFY_ELTS
		/* if justification is allowed for prev line, then raw
		 * HT_NON_BREAK_SPACE are still present in data[] (they'll be
		 * substituted at the end of this function with ' ') - VH
		 */
		(*p == ' ' || *p == HT_NON_BREAK_SPACE ) &&
#else
		(*p == ' ') &&
#endif

		(HeadTrim || text->first_anchor ||
		 underline_on || bold_on ||
		 alignment != HT_LEFT ||
		 style->wordWrap || style->freeFormat ||
		 style->spaceBefore || style->spaceAfter)) ||
		*p == LY_SOFT_HYPHEN) {
	    p++;
	    HeadTrim++;
	}
	plen = strlen(p);

	/*
	 *  Add underline char if needed. - FM
	 */
	if (!(dump_output_immediately && use_underscore)) {
	    /*
	     * Make sure our global flag is correct. - FM
	     */
	    underline_on = NO;
	    for (i = (split-1); i >= 0; i--) {
		if (prevdata[i] == LY_UNDERLINE_END_CHAR) {
		    break;
		}
		if (prevdata[i] == LY_UNDERLINE_START_CHAR) {
		    underline_on = YES;
		    break;
		}
	    }
	    /*
	     *  Act on the global flag if set above. - FM
	     */
	    if (underline_on && *p != LY_UNDERLINE_END_CHAR) {
		linedata[line->size++] = LY_UNDERLINE_START_CHAR;
		linedata[line->size] = '\0';
		ctrl_chars_on_this_line++;
		SpecialAttrChars++;
	    }
	    if (plen) {
		for (i = (plen - 1); i >= 0; i--) {
		    if (p[i] == LY_UNDERLINE_START_CHAR) {
			underline_on = YES;
			break;
		    }
		    if (p[i] == LY_UNDERLINE_END_CHAR) {
			underline_on = NO;
			break;
		    }
		}
		for (i = (plen - 1); i >= 0; i--) {
		    if (p[i] == LY_UNDERLINE_START_CHAR ||
			p[i] == LY_UNDERLINE_END_CHAR) {
			ctrl_chars_on_this_line++;
		    }
		}
	    }
	}

	/*
	 *  Add bold char if needed, first making
	 *  sure that our global flag is correct. - FM
	 */
	bold_on = NO;
	for (i = (split - 1); i >= 0; i--) {
	    if (prevdata[i] == LY_BOLD_END_CHAR) {
		break;
	    }
	    if (prevdata[i] == LY_BOLD_START_CHAR) {
		bold_on = YES;
		break;
	    }
	}
	/*
	 *  Act on the global flag if set above. - FM
	 */
	if (bold_on && *p != LY_BOLD_END_CHAR) {
	    linedata[line->size++] = LY_BOLD_START_CHAR;
	    linedata[line->size] = '\0';
	    ctrl_chars_on_this_line++;
	    SpecialAttrChars++;
	}
	if (plen) {
	    for (i = (plen - 1); i >= 0; i--) {
		if (p[i] == LY_BOLD_START_CHAR) {
		    bold_on = YES;
		    break;
		}
		if (p[i] == LY_BOLD_END_CHAR) {
		    bold_on = NO;
		    break;
		}
	    }
	    for (i = (plen - 1); i >= 0; i--) {
		if (p[i] == LY_BOLD_START_CHAR ||
		    p[i] == LY_BOLD_END_CHAR ||
		    p[i] == LY_SOFT_HYPHEN) {
		    ctrl_chars_on_this_line++;
		} else if (IS_UTF_EXTRA(p[i])) {
		    utfxtra_on_this_line++;
		}
		if (p[i] == LY_SOFT_HYPHEN && (int)text->permissible_split < i) {
		    text->permissible_split = i + 1;
		}
	    }
	    ctrl_chars_on_this_line += utfxtra_on_this_line;
	}

	/*
	 *  Add the data to the new line. - FM
	 */
	strcat(linedata, p);
	line->size += plen;
    }

    /*
     *  Economize on space.
     */
    while ((previous->size > 0) &&
#ifdef EXP_JUSTIFY_ELTS
	    /* if justification is allowed for prev line, then raw
	     * HT_NON_BREAK_SPACE are still present in data[] (they'll be
	     * substituted at the end of this function with ' ') - VH
	     */
	   ((previous->data[previous->size-1] == ' ') ||
	    (previous->data[previous->size-1] == HT_NON_BREAK_SPACE)) &&
#else
	   (previous->data[previous->size-1] == ' ') &&
#endif
#ifdef USE_PRETTYSRC
	    !psrc_view && /*don't strip trailing whites - since next line can
		start with LY_SOFT_NEWLINE - so we don't lose spaces when
		'p'rinting this text to file -VH */
#endif
	   (ctrl_chars_on_this_line || HeadTrim || text->first_anchor ||
	    underline_on || bold_on ||
	    alignment != HT_LEFT ||
	    style->wordWrap || style->freeFormat ||
	    style->spaceBefore || style->spaceAfter)) {
	/*
	 *  Strip trailers.
	 */
	previous->data[previous->size-1] = '\0';
	previous->size--;
	TailTrim++;
    }
    /*
     *  s is the effective split position, given by either a non-zero
     *  value of split or by the size of the previous line before
     *  trimming. - kw
     */
    if (split == 0) {
	s = previous->size + TailTrim; /* the original size */
    } else {
	s = split;
    }
#ifdef DEBUG_SPLITLINE
#ifdef DEBUG_APPCH
    if (s != (int)split)
#endif
	CTRACE((tfp,"GridText: split_line(%d [now:%d]) called\n", split, s));
#endif

#if defined(USE_COLOR_STYLE)
#define LastStyle (previous->numstyles-1)
    line->numstyles = 0;
    inew = MAX_STYLES_ON_LINE - 1;
    /*
     *  Color style changes after the split position + possible trimmed
     *  head characters are transferred to the new line.  Ditto for changes
     *  within the trimming region, but be stop when we reach an OFF change.
     *  The second while loop below may then handle remaining changes. - kw
     */
    while (previous->numstyles && inew >= 0) {
	if (previous->styles[LastStyle].horizpos > s + HeadTrim) {
	    line->styles[inew].horizpos =
		previous->styles[LastStyle].horizpos
		- (s + HeadTrim) + SpecialAttrChars;
	    line->styles[inew].direction = previous->styles[LastStyle].direction;
	    line->styles[inew].style = previous->styles[LastStyle].style;
	    inew --;
	    line->numstyles ++;
	    previous->numstyles --;
	} else if (previous->styles[LastStyle].horizpos > s - TailTrim &&
		   (previous->styles[LastStyle].direction == STACK_ON ||
		    previous->styles[LastStyle].direction == ABS_ON)) {
	    line->styles[inew].horizpos =
		(previous->styles[LastStyle].horizpos < s) ?
		0 : SpecialAttrChars;
	    line->styles[inew].direction = previous->styles[LastStyle].direction;
	    line->styles[inew].style = previous->styles[LastStyle].style;
	    inew --;
	    line->numstyles ++;
	    previous->numstyles --;
	} else
	    break;
    }
    spare = previous->numstyles;
    while (previous->numstyles && inew >= 0) {
	if (previous->numstyles >= 2 &&
	    previous->styles[LastStyle].style
	    == previous->styles[previous->numstyles-2].style &&
	    previous->styles[LastStyle].horizpos
	    == previous->styles[previous->numstyles-2].horizpos &&
	    ((previous->styles[LastStyle].direction == STACK_OFF &&
	      previous->styles[previous->numstyles-2].direction == STACK_ON) ||
	     (previous->styles[LastStyle].direction == ABS_OFF &&
	      previous->styles[previous->numstyles-2].direction == ABS_ON) ||
	     (previous->styles[LastStyle].direction == ABS_ON &&
	      previous->styles[previous->numstyles-2].direction == ABS_OFF)
		)) {
	    /*
	     *  Discard pairs of ON/OFF for the same color style, but only
	     *  if they appear at the same position. - kw
	     */
	    previous->numstyles -= 2;
	    if (spare > previous->numstyles)
		spare = previous->numstyles;
	} else if (spare > 0 && previous->styles[spare - 1].direction &&
		   previous->numstyles < MAX_STYLES_ON_LINE) {
	    /*
	     *  The index variable spare walks backwards through the
	     *  list of color style changes on the previous line, trying
	     *  to find an ON change which isn't followed by a
	     *  corresponding OFF.  When it finds one, the missing OFF
	     *  change is appended to the end, and an ON change is added
	     *  at the beginning of the current line.  The OFF change
	     *  appended to the previous line may get removed again in
	     *  the next iteration. - kw
	     */
	    line->styles[inew].horizpos = 0;
	    line->styles[inew].direction = ON;
	    line->styles[inew].style = previous->styles[spare - 1].style;
	    inew --;
	    line->numstyles ++;
	    previous->styles[previous->numstyles].style = line->styles[inew + 1].style;

	    previous->styles[previous->numstyles].direction = ABS_OFF;
	    previous->styles[previous->numstyles].horizpos = previous->size;
	    previous->numstyles++;
	    spare --;
	} else if (spare >= 2 &&
		   previous->styles[spare - 1].style == previous->styles[spare - 2].style &&
		   ((previous->styles[spare - 1].direction == STACK_OFF &&
		     previous->styles[spare - 2].direction == STACK_ON) ||
		    (previous->styles[spare - 1].direction == ABS_OFF &&
		     previous->styles[spare - 2].direction == ABS_ON) ||
		    (previous->styles[spare - 1].direction == STACK_ON &&
		     previous->styles[spare - 2].direction == STACK_OFF) ||
		    (previous->styles[spare - 1].direction == ABS_ON &&
		     previous->styles[spare - 2].direction == ABS_OFF)
		       )) {
		/*
		 *  Skip pairs of adjacent ON/OFF or OFF/ON changes.
		 */
	    spare -= 2;
	} else if (spare && !previous->styles[spare - 1].direction) {
	    /*
	     *  Found an OFF change not part of an adjacent matched pair.
	     *  Walk backward looking for the corresponding ON change.
	     *  If we find it, skip the ON/OFF and everything in between.
	     *  This can only work correctly if all changes are correctly
	     *  nested!  If this fails, assume it is safer to leave whatever
	     *  comes before the OFF on the previous line alone.  Setting
	     *  spare to 0 ensures that it won't be used in a following
	     *  iteration. - kw
	     */
	    int level=-1;
	    int itmp;
	    for (itmp = spare-1; itmp > 0; itmp--) {
		if (previous->styles[itmp - 1].style
		    == previous->styles[spare - 1].style) {
		    if (previous->styles[itmp - 1].direction == STACK_OFF) {
			level--;
		    } else if (previous->styles[itmp - 1].direction == STACK_ON) {
			if (++level == 0)
			    break;
		    } else
			break;
		}
	    }
	    if (level == 0)
		spare = itmp - 1;
	    else
		spare = 0;
	} else {
	    /*
	     *  Nothing applied, so we are done with the loop. - kw
	     */
	    break;
	}
    }
    if (previous->numstyles > 0 && previous->styles[LastStyle].direction) {

	CTRACE((tfp, "%s\n%s%s\n",
		    "........... Too many character styles on line:",
		    "........... ", previous->data));
    }
    if (line->numstyles > 0 && line->numstyles < MAX_STYLES_ON_LINE) {
	int n;
	inew ++;
	for (n = 0; n < line->numstyles; n++)
		line->styles[n] = line->styles[n + inew];
    } else if (line->numstyles == 0) {
	line->styles[0].horizpos = ~0;
    }
    if (previous->numstyles == 0)
	previous->styles[0].horizpos = ~0;
#endif /*USE_COLOR_STYLE*/

    temp = (HTLine *)LY_CALLOC(1, LINE_SIZE(previous->size));
    if (temp == NULL)
	outofmem(__FILE__, "split_line_2");
    memcpy(temp, previous, LINE_SIZE(previous->size));
#if defined(USE_COLOR_STYLE) && !defined(OLD_HTSTYLECHANGE)
    ALLOC_IN_POOL((text->styles_pool),HTStyleChangePool,previous->numstyles,temp->styles);
    memcpy(temp->styles, previous->styles, sizeof(HTStyleChange)*previous->numstyles);
    if (!temp->styles)
	outofmem(__FILE__, "split_line_2");
#endif
    FREE(previous);
    previous = temp;

    previous->prev->next = previous;	/* Link in new line */
    previous->next->prev = previous;	/* Could be same node of course */

    /*
     *  Terminate finished line for printing.
     */
    previous->data[previous->size] = '\0';

    /*
     *  Align left, right or center.
     */
    spare = 0;
    if (
#ifdef EXP_JUSTIFY_ELTS
	this_line_was_split ||
#endif
	(alignment == HT_CENTER ||
	 alignment == HT_RIGHT) || text->stbl) {
	/* Calculate spare character positions if needed */
	for (cp = previous->data; *cp; cp++) {
	    if (*cp == LY_UNDERLINE_START_CHAR ||
		*cp == LY_UNDERLINE_END_CHAR ||
		*cp == LY_BOLD_START_CHAR ||
		*cp == LY_BOLD_END_CHAR ||
		IS_UTF_EXTRA(*cp) ||
		*cp == LY_SOFT_HYPHEN)
		ctrl_chars_on_previous_line++;
	}
	if ((previous->size > 0) &&
		(int)(previous->data[previous->size-1] == LY_SOFT_HYPHEN))
	    ctrl_chars_on_previous_line--;

	/* @@ first line indent */
	spare =  (LYcols-1) -
	    (int)style->rightIndent - indent +
	    ctrl_chars_on_previous_line - previous->size;

	if (spare > 0 && !dump_output_immediately &&
	    text->T.output_utf8 && ctrl_chars_on_previous_line) {
	    utfxtra_on_previous_line -= utfxtra_on_this_line;
	    if (utfxtra_on_previous_line) {
		int spare_cu = (LYcols_cu-1) -
		    utfxtra_on_previous_line - indent +
		    ctrl_chars_on_previous_line - previous->size;
		    /*
		     *  Shift non-leftaligned UTF-8 lines that would be
		     *  mishandled by the display library towards the left
		     *  if this would make them fit.  The resulting display
		     *  will not be as intended, but this is better than
		     *  having them split by curses.  (Curses cursor movement
		     *  optimization may still cause wrong positioning within
		     *  the line, in particular after a sequence of spaces).
		     * - kw
		     */
		if (spare_cu < spare) {
		    if (spare_cu >= 0) {
			if (alignment == HT_CENTER &&
			    (int)(previous->offset + indent + spare/2 +
				  previous->size)
			    - ctrl_chars_on_previous_line
			    + utfxtra_on_previous_line <= (LYcols_cu - 1))
			    /* do nothing - it still fits - kw */;
			else {
			    spare = spare_cu;
			    if (alignment == HT_CENTER) {
				/*
				 *  Can't move towars center all the way,
				 *  but at least make line contents appear
				 *  as far right as possible. - kw
				 */
				alignment = HT_RIGHT;
			    }
			}
		    } else if (indent + (int)previous->offset + spare_cu >= 0)
		    { /* subtract overdraft from effective indentation */
			indent += (int)previous->offset + spare_cu;
			previous->offset = 0;
			spare = 0;
		    }
		}
	    }
	}
    }

    if (text->stbl)
	/*
	 *  Notify simple table stuff of line split, so that it can
	 *  set the last cell's length.  The last cell should and
	 *  its row should really end here, or on one of the following
	 *  lines with no more characters added after the break.
	 *  We don't know whether a cell has been started, so ignore
	 *  errors here.
	 *  This call is down here because we need the
	 *  ctrl_chars_on_previous_line, which have just been re-
	 *  counted above. - kw
	 */
	Stbl_lineBreak(text->stbl,
		       text->Lines - 1,
		       previous->size - ctrl_chars_on_previous_line);

    switch (style->alignment) {
	case HT_CENTER :
	    previous->offset = previous->offset + indent + spare/2;
	    break;
	case HT_RIGHT :
	    previous->offset = previous->offset + indent + spare;
	    break;
	case HT_LEFT :
	case HT_JUSTIFY :		/* Not implemented */
	default:
	    previous->offset = previous->offset + indent;
	    break;
    } /* switch */

    text->chars = text->chars + previous->size + 1;	/* 1 for the line */
    text->in_line_1 = NO;		/* unless caller sets it otherwise */

    /*
     *  If we split the line, adjust the anchor
     *  structure values for the new line. - FM
     */

    if (split > 0 || s > 0) {		/* if not completely empty */
	TextAnchor * prev_a = NULL;
	for (a = text->first_anchor; a; prev_a = a, a = a->next) {
	    if (a->line_num == CurLine) {
		int old_e = a->extent;
		int a0 = a->line_pos, a1 = a->line_pos + a->extent;
		int S, d, new_pos, new_ext;
		if (a->link_type == INPUT_ANCHOR) {
		    if (a->line_pos >= s) {
			a->start += (1 + SpecialAttrChars - HeadTrim - TailTrim);
			a->line_pos -= (s - SpecialAttrChars + HeadTrim);
			a->line_num = text->Lines;
		    }
		    continue;
		}
		if (a0 < s - TailTrim && a1 <= s - TailTrim) {
		    continue;	/* unaffected, leave it where it is. */
		}
		S = s + a->start - a->line_pos;
		d = S - s;	/* == a->start - a->line_pos */
		new_ext = old_e = a->extent;

		if (!old_e &&
		    (!a->number || a->show_anchor) &&
		    a0 <= s + HeadTrim) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,1));
		    /*
		     *  It is meant to be empty, and/or endAnchor
		     *  has seen it and recognized it as empty.
		     */
		    new_pos = (a0 <= s) ? s - TailTrim :
			s - TailTrim + 1;
		    if (prev_a && new_pos + d < prev_a->start) {
			if (prev_a->start <= S - TailTrim + 1 + SpecialAttrChars)
			    new_pos = prev_a->start - d;
			else
			    new_pos = s - TailTrim + 1 + SpecialAttrChars;
		    }
		} else if (old_e &&
		     a0 >= s - TailTrim && a0 <= s + HeadTrim &&
		     a1 <= s + HeadTrim) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,2));
		    /*
		     *  endAnchor has seen it, it is effectively empty
		     *  after our trimming, but endAnchor has for some
		     *  reason not recognized this.  In other words,
		     *  this should not happen.
		     *  Should we not adjust the extent and let it "leak"
		     *  into the new line?
		     */
		    new_pos = (a0 < s) ? s - TailTrim :
			s - TailTrim + 1;
		    if (prev_a && new_pos + d < prev_a->start) {
			if (prev_a->start <= S - TailTrim + 1 + SpecialAttrChars)
			    new_pos = prev_a->start - d;
			else
			    new_pos = s - TailTrim + 1 + SpecialAttrChars;
		    }
		    new_ext = 0;
		} else if (a0 >= s + HeadTrim) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,3));
		    /*
		     *  Completely after split, just shift.
		     */
		    new_pos = a0 - TailTrim + 1 - HeadTrim + SpecialAttrChars;
		} else if (!old_e) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,4));
		    /*
		     *  No extent set, we may still be growing it.
		     */
		    new_pos = s - TailTrim + 1 + SpecialAttrChars;

		    /*
		     *  Ok, it's neither empty, nor is it completely
		     *  before or after the split region (including trimmed
		     *  stuff).  So the anchor is either being split in
		     *  the middle, with stuff remaining on both lines,
		     *  or something is being nibbled off, either at
		     *  the end (anchor stays on previous line) or at
		     *  the beginning (anchor is on new line).  Let's
		     *  try in that order.
		     */
		} else if (a0 < s - TailTrim &&
			   a1 > s + HeadTrim) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,5));
		    new_pos = a0;
		    new_ext = old_e - TailTrim - HeadTrim + SpecialAttrChars;
		} else if (a0 < s - TailTrim) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,6));
		    new_pos = a0;
		    new_ext = s - TailTrim - a0;
		} else if (a1 > s + HeadTrim) {
		    CTRACE_SPLITLINE((tfp, "anchor %d case %d: ", a->number,7));
		    new_pos = s - TailTrim + 1 + SpecialAttrChars;
		    new_ext = old_e - (s + HeadTrim - a0);
		} else {
		    CTRACE((tfp, "split_line anchor %d line %d: This should not happen!\n",
			   a->number, a->line_num));
		    CTRACE((tfp,
			   "anchor %d: (T,H,S)=(%d,%d,%d); (line,start,pos,ext):(%d,%d,%d,%d)!\n",
			   a->number,
			   TailTrim,HeadTrim,SpecialAttrChars,
			   a->line_num,a->start,a->line_pos,a->extent));
		    continue;
		}
		CTRACE_SPLITLINE((tfp, "(T,H,S)=(%d,%d,%d); (line,start,pos,ext):(%d,%d,%d,%d",
		       TailTrim,HeadTrim,SpecialAttrChars,
		       a->line_num,a->start,a->line_pos,a->extent));
		if (new_pos != a->line_pos)
		    a->start = new_pos + d;
		if (new_pos > s - TailTrim) {
		    new_pos -= s - TailTrim + 1;
		    a->line_num = text->Lines;
		}
		a->line_pos = new_pos;
		a->extent = new_ext;

		CTRACE_SPLITLINE((tfp, "))->(%d,%d,%d,%d)\n",
		       a->line_num,a->start,a->line_pos,a->extent));
	    }
	}
    }

#ifdef EXP_JUSTIFY_ELTS
    /* now perform justification - by VH */

    if (this_line_was_split
     && spare
     && justify_max_void_percent > 0
     && justify_max_void_percent <= 100
     && justify_max_void_percent >= ((100*spare)
				  / ((LYcols - 1)
				   - (int)style->rightIndent
				   - indent
				   + ctrl_chars_on_previous_line))) {
	/* this is the only case when we need justification*/
	char* jp = previous->data + justify_start_position;
	ht_run_info* r = ht_runs;
	char c;
	int total_byte_len = 0, total_cell_len = 0;
	int d_, r_, i, j, cur_byte_num, *m;
	HTLine * jline;
	char *jdata;
	char *prevdata = previous->data;

	ht_num_runs = 0;
	r->byte_len = r->cell_len = 0;

	for(; (c = *jp) != 0; ++jp) {
	    if (c == ' ') {
		total_byte_len += r->byte_len;
		total_cell_len += r->cell_len;
		++r;
		++ht_num_runs;
		r->byte_len = r->cell_len = 0;
		continue;
	    }
	    ++r->byte_len;
	    if ( IsSpecialAttrChar(c) )
		continue;

	    ++r->cell_len;
	    if (c == HT_NON_BREAK_SPACE) {
		*jp = ' ';	/* substitute it */
		continue;
	    }
	    if (text->T.output_utf8 && !isascii(UCH(c))) {
		int utf_extra = 0;
		if ((c & 0xe0) == 0xc0) {
		    utf_extra = 1;
		} else if ((c & 0xf0) == 0xe0) {
		    utf_extra = 2;
		} else if ((c & 0xf8) == 0xf0) {
		    utf_extra = 3;
		} else if ((c & 0xfc) == 0xf8) {
		    utf_extra = 4;
		} else if ((c & 0xfe) == 0xfc) {
		    utf_extra = 5;
		} else
		    utf_extra = 0;
		if ( (int) strlen(jp+1) < utf_extra)
		    utf_extra = 0;
		r->byte_len += utf_extra;
		jp += utf_extra;
	    }
	}
	total_byte_len += r->byte_len;
	total_cell_len += r->cell_len;
	++ht_num_runs;

	if (ht_num_runs != 1) {

	    jline = allocHTLine(previous->size+spare);
	    if (jline == NULL)
		outofmem(__FILE__, "split_line_1");
#if defined(USE_COLOR_STYLE) && !defined(OLD_HTSTYLECHANGE)
	    jline->styles = previous->styles;
#endif
	    jdata = jline->data;

	    /*
	     * we have to spread num_spaces among (ht_num_runs-1) runs - we
	     * fill justified_text_map in order to apply changes caused by
	     * justification to anchor data and color styles, and justify
	     * original string on the fly
	     */
	    d_ = spare/(ht_num_runs-1);
	    r_ = spare % (ht_num_runs-1);

	    m = justified_text_map;
	    for(jp = previous->data, i = 0; i < justify_start_position; ++i) {
		*m++ = i;
		*jdata++ = ( *prevdata == HT_NON_BREAK_SPACE ? ' ' : *prevdata);
		++prevdata;
	    }

	    cur_byte_num = i;

	    for (r = ht_runs; r < ht_runs + ht_num_runs; ++r ) {

		/* copy the reference to run content */
		for(i=0; i < r->byte_len;  ++i) {
		    *m++ = cur_byte_num++;
		    *jdata++ = *prevdata++;
		}
		if ( r - ht_runs  == ht_num_runs - 1 ) { /* nop on last run */
		    *jdata++ = '\0';
		    break;
		}

		/* the space that was in original string */
		*m++ = cur_byte_num++;
		*jdata++ = ' ';
		prevdata++;/* skip that space */

		cur_byte_num += j=( d_ + ( r_--  > 0 ) );
		for (i=0; i<j; ++i)
		    *jdata++ = ' ';
	    }
	    *m++ = previous->size + spare; /*map the end */

	    text->chars += spare;

	    jline->offset = previous->offset;
	    jline->size = previous->size + spare;
	    jline->split_after = previous->split_after;
	    jline->bullet = previous->bullet;
	    jline->expansion_line = previous->expansion_line;

	    jline->prev = previous->prev;
	    jline->next = previous->next;
	    previous->next->prev = jline;
	    previous->prev->next = jline;

#if defined(USE_COLOR_STYLE)
	    jline->numstyles = previous->numstyles;

	    /* now copy and fix colorstyles */
	    for(i = 0; i < jline->numstyles; ++i) {
		int hpos = previous->styles[i].horizpos;

		jline->styles[i].style = previous->styles[i].style;
		jline->styles[i].direction = previous->styles[i].direction;
#ifdef OLD_HTSTYLECHANGE
		jline->styles[i].previous = previous->styles[i].previous;
#endif
		/*there are stylechanges with hpos > line length */
		jline->styles[i].horizpos = (hpos > previous->size)
			? previous->size + spare
			: justified_text_map[hpos];
	    }
#endif
	    /* we have to fix anchors*/
	    {
		/*a2 is the last anchor on the line preceding 'previous'*/
		TextAnchor* a2 = last_anchor_of_previous_line;

		if (!a2)
		    a2 = text->first_anchor;
		else if (a2 == text->last_anchor)
		    a2 = NULL;
		else
		    a2 = a2->next; /*1st anchor on line we justify */

		if (a2) {
		    for (; a2 /*&& a2->line_num == text->Lines-1*/;
			    last_anchor_of_previous_line = a2, a2 = a2->next) {
			int oldpos = a2->line_pos,
			    newpos = justified_text_map[a2->line_pos],
			    shift = newpos - oldpos;

			if (a2->line_num == text->Lines)
			    break;/*new line not yet completed*/

			if (a2->line_num == text->Lines-1) {
			    a2->line_pos = newpos;
			    a2->start += shift;

			    if (!a2->extent && a2->number &&
				(a2->link_type & HYPERTEXT_ANCHOR) &&
				!a2->show_anchor &&
				a2->number == text->last_anchor_number)
				/* seems endAnchor wasn't called for it */ {
				a2 = a2->next; /*don't allow .start to be incremented
				    by 'spare' once more */
				break;
			    }

			    if ( a2->extent + oldpos > (int) previous->size)
				/*anchor content wrapped to new line */
				a2->extent += (jline->size - newpos) -
				    (previous->size - oldpos);
			    else
				a2->extent = justified_text_map[oldpos+a2->extent]
				    - newpos;

			} else {
			    /* This is the anchor that was started on previous
			     * line.  Its .line_pos and .start were updated.
			     * So we have to update only extent.  If anchor
			     * text is longer than two lines, we don't bother
			     * setting it to correct value.
			     */
			    if (a2->line_num != text->Lines-2)
				continue; /* don't bother */
			    if (!a2->extent && a2->number &&
				(a2->link_type & HYPERTEXT_ANCHOR) &&
				!a2->show_anchor &&
				a2->number == text->last_anchor_number)
				/* seems endAnchor wasn't called for it */
				continue;
			    /* anchor is started at text->Lines-2, and there
			     * are two cases - either it was wrapped to newline
			     * or it ended in previous text->Lines-1.
			     */
			    {
				int p2sz = previous->prev->size,
				    p1sz = previous->size,
				    onp2sz = p2sz - a2->line_pos,
				    onp1sz = a2->extent - 1 - onp2sz;

				if (onp1sz >= p1sz)
				    /* this anchor will be skipped at the next
				     * split_line here, since its line_num will
				     * be text->Lines-3
				     */
				    a2->extent += spare;
				else if (onp2sz < a2->extent)
				    a2->extent += justified_text_map[onp1sz-1]
					 - onp1sz + 1;
			    }

			}

		    }

		    /* iterate on anchors in the last line */
		    for (; a2; a2 = a2->next)
			a2->start += spare;
		}
	    }

	    FREE(previous);

	} else { /* (ht_num_runs==1) */
	    /* keep maintaining 'last_anchor_of_previous_line' */
	    TextAnchor* a2 = last_anchor_of_previous_line;
	    if (justify_start_position) {
		char* p = previous->data;
		for( ; p < previous->data + justify_start_position; ++p)
		    *p = (*p == HT_NON_BREAK_SPACE ? ' ' : *p);
	    }

	    if (!a2)
		a2 = text->first_anchor;
	    else if (a2 == text->last_anchor)
		a2 = NULL;
	    else
		a2 = a2->next; /* 1st anchor on line we justify */

	    if (a2)
		for (; a2 && a2->line_num <= text->Lines-1;
		    last_anchor_of_previous_line = a2, a2 = a2->next);
	}
    } else {
	if (REALLY_CAN_JUSTIFY(text) ) {
	    char* p;

	    /* it was permitted to justify line, but this function was called
	     * to end paragraph - we must substitute HT_NON_BREAK_SPACEs with
	     * spaces in previous line
	     */
	    if (line->size) {
		  CTRACE((tfp,"justification: shouldn't happen - new line is not empty!\n"));
	    }

	    for (p=previous->data;*p;++p)
		if (*p == HT_NON_BREAK_SPACE)
		    *p = ' ';
	} else if (have_raw_nbsps) {
	    /* this is very rare case, that can happen in forms placed in
	       table cells*/
	    int i;

	    for (i = 0; i< previous->size; ++i)
		if (previous->data[i] == HT_NON_BREAK_SPACE)
		    previous->data[i] = ' ';

	    /*next line won't be justified, so substitute nbsps in it too */
	    for (i = 0; i< line->size; ++i)
		if (line->data[i] == HT_NON_BREAK_SPACE)
		    line->data[i] = ' ';
	}

	/* else HT_NON_BREAK_SPACEs were substituted with spaces in
	   HText_appendCharacter */
    }
	/* cleanup */
    can_justify_this_line = TRUE;
    justify_start_position = 0;
    this_line_was_split = FALSE;
    have_raw_nbsps = FALSE;
#endif /* EXP_JUSTIFY_ELTS */
} /* split_line */


/*	Allow vertical blank space
**	--------------------------
*/
PRIVATE void blank_lines ARGS2(
	HText *,	text,
	int,		newlines)
{
    BOOL IgnoreSpaces = FALSE;

    if (!HText_LastLineSize(text, IgnoreSpaces)) { /* No text on current line */
	HTLine * line = text->last_line->prev;
	while ((line != text->last_line) &&
	       (HText_TrueLineSize(line, text, IgnoreSpaces) == 0)) {
	    if (newlines == 0)
		break;
	    newlines--;		/* Don't bother: already blank */
	    line = line->prev;
	}
    } else {
	newlines++;			/* Need also to finish this line */
    }

    for (; newlines; newlines--) {
	new_line(text);
    }
    text->in_line_1 = YES;
}


/*	New paragraph in current style
**	------------------------------
** See also: setStyle.
*/
PUBLIC void HText_appendParagraph ARGS1(
	HText *,	text)
{
    int after = text->style->spaceAfter;
    int before = text->style->spaceBefore;
    blank_lines(text, ((after > before) ? after : before));
}


/*	Set Style
**	---------
**
**	Does not filter unnecessary style changes.
*/
PUBLIC void HText_setStyle ARGS2(
	HText *,	text,
	HTStyle *,	style)
{
    int after, before;

    if (!style)
	return;				/* Safety */
    after = text->style->spaceAfter;
    before = style->spaceBefore;

    CTRACE((tfp, "GridText: Change to style %s\n", style->name));

    blank_lines (text, ((after > before) ? after : before));

    text->style = style;
}

/*	Append a character to the text object
**	-------------------------------------
*/
PUBLIC void HText_appendCharacter ARGS2(
	HText *,	text,
	char,		ch)
{
    HTLine * line;
    HTStyle * style;
    int indent, utfx;

#ifdef DEBUG_APPCH
#ifdef CJK_EX
    static unsigned char save_ch = 0;
#endif

    if (TRACE) {
	char * special = NULL;  /* make trace a little more readable */
	switch(ch) {
	case HT_NON_BREAK_SPACE:
		special = "HT_NON_BREAK_SPACE";
		break;
	case HT_EN_SPACE:
		special = "HT_EN_SPACE";
		break;
	case LY_UNDERLINE_START_CHAR:
		special = "LY_UNDERLINE_START_CHAR";
		break;
	case LY_UNDERLINE_END_CHAR:
		special = "LY_UNDERLINE_END_CHAR";
		break;
	case LY_BOLD_START_CHAR:
		special = "LY_BOLD_START_CHAR";
		break;
	case LY_BOLD_END_CHAR:
		special = "LY_BOLD_END_CHAR";
		break;
	case LY_SOFT_HYPHEN:
		special = "LY_SOFT_HYPHEN";
		break;
	case LY_SOFT_NEWLINE:
		special = "LY_SOFT_NEWLINE";
		break;
	default:
		special = NULL;
		break;
	}

	if (special != NULL) {
	    CTRACE((tfp, "add(%s %d special char) %d/%d\n", special, ch,
		   HTisDocumentSource(), HTOutputFormat != WWW_SOURCE));
	} else {
#ifdef CJK_EX	/* 1998/08/30 (Sun) 13:26:23 */
	    if (save_ch == 0) {
		if (IS_SJIS_HI1(ch) || IS_SJIS_HI2(ch)) {
		    save_ch = ch;
		} else {
		    CTRACE((tfp, "add(%c) %d/%d\n", ch,
			HTisDocumentSource(), HTOutputFormat != WWW_SOURCE));
		}
	    } else {
		CTRACE((tfp, "add(%c%c) %d/%d\n", save_ch, ch,
			HTisDocumentSource(), HTOutputFormat != WWW_SOURCE));
		save_ch = 0;
	    }
#else
	    if (ch < 0x80) {
		CTRACE((tfp, "add(%c) %d/%d\n", ch,
		    HTisDocumentSource(), HTOutputFormat != WWW_SOURCE));
	    } else {
		CTRACE((tfp, "add(%02x) %d/%d\n", ch,
		    HTisDocumentSource(), HTOutputFormat != WWW_SOURCE));
	    }
#endif	/* CJK_EX */
	}
    } /* trace only */
#endif /* DEBUG_APPCH */

    /*
     *  Make sure we don't crash on NULLs.
     */
    if (!text)
	return;

    if (text->halted > 1) {
	/*
	 *  We should stop outputting more text, because low memory was
	 *  detected.  - kw
	 */
	if (text->halted == 2) {
	    /*
	     *  But if we haven't done so yet, first append a warning.
	     *  We should still have a few bytes left for that :).
	     *  We temporarily reset test->halted to 0 for this, since
	     *  this function will get called recursively. - kw
	     */
	    text->halted = 0;
	    text->kanji_buf = '\0';
	    HText_appendText(text, gettext(" *** MEMORY EXHAUSTED ***"));
	}
	text->halted = 3;
	return;
    }
#ifdef USE_TH_JP_AUTO_DETECT
    if ((HTCJK == JAPANESE) && (text->detected_kcode != DET_MIXED) &&
	(text->specified_kcode != SJIS) && (text->specified_kcode != EUC)) {
	unsigned char c;
	enum _detected_kcode save_d_kcode;

	c = ch;
	save_d_kcode = text->detected_kcode;
	switch (text->SJIS_status) {
	case SJIS_state_has_bad_code:
	    break;
	case SJIS_state_neutral:
	    if (IS_SJIS_HI1(c) || IS_SJIS_HI2(c)) {
		text->SJIS_status = SJIS_state_in_kanji;
	    }
	    else if ((c & 0x80) && !IS_SJIS_X0201KANA(c)) {
		text->SJIS_status = SJIS_state_has_bad_code;
		if (text->EUC_status == EUC_state_has_bad_code)
		    text->detected_kcode = DET_MIXED;
		else
		    text->detected_kcode = DET_EUC;
	    }
	    break;
	case SJIS_state_in_kanji:
	    if (IS_SJIS_LO(c)) {
		text->SJIS_status = SJIS_state_neutral;
	    }
	    else {
		text->SJIS_status = SJIS_state_has_bad_code;
		if (text->EUC_status == EUC_state_has_bad_code)
		    text->detected_kcode = DET_MIXED;
		else
		    text->detected_kcode = DET_EUC;
	    }
	    break;
	}
	switch (text->EUC_status) {
	case EUC_state_has_bad_code:
	    break;
	case EUC_state_neutral:
	    if (IS_EUC_HI(c)) {
		text->EUC_status = EUC_state_in_kanji;
	    }
	    else if (c == 0x8e) {
		text->EUC_status = EUC_state_in_kana;
	    }
	    else if (c & 0x80) {
		text->EUC_status = EUC_state_has_bad_code;
		if (text->SJIS_status == SJIS_state_has_bad_code)
		    text->detected_kcode = DET_MIXED;
		else
		    text->detected_kcode = DET_SJIS;
	    }
	    break;
	case EUC_state_in_kanji:
	    if (IS_EUC_LOX(c)) {
		text->EUC_status = EUC_state_neutral;
	    }
	    else {
		text->EUC_status = EUC_state_has_bad_code;
		if (text->SJIS_status == SJIS_state_has_bad_code)
		    text->detected_kcode = DET_MIXED;
		else
		    text->detected_kcode = DET_SJIS;
	    }
	    break;
	case EUC_state_in_kana:
	    if ((0xA1<=c)&&(c<=0xDF)) {
		text->EUC_status = EUC_state_neutral;
	    }
	    else {
		text->EUC_status = EUC_state_has_bad_code;
		if (text->SJIS_status == SJIS_state_has_bad_code)
		    text->detected_kcode = DET_MIXED;
		else
		    text->detected_kcode = DET_SJIS;
	    }
	    break;
	}
	if (save_d_kcode != text->detected_kcode) {
	    switch (text->detected_kcode) {
	    case DET_SJIS:
		CTRACE((tfp, "TH_JP_AUTO_DETECT: This document's kcode seems SJIS.\n"));
		break;
	    case DET_EUC:
		CTRACE((tfp, "TH_JP_AUTO_DETECT: This document's kcode seems EUC.\n"));
		break;
	    case DET_MIXED:
		CTRACE((tfp, "TH_JP_AUTO_DETECT: This document's kcode seems mixed!\n"));
		break;
	    default:
		CTRACE((tfp, "TH_JP_AUTO_DETECT: This document's kcode is unexpected!\n"));
		break;
	    }
	}
    }
#endif /* USE_TH_JP_AUTO_DETECT */
    /*
     *  Make sure we don't hang on escape sequences.
     */
    if (ch == CH_ESC && HTCJK == NOCJK)			/* decimal 27  S/390 -- gil -- 1504 */
	return;
#ifndef USE_SLANG
    /*
     *  Block 8-bit chars not allowed by the current display character
     *  set if they are below what LYlowest_eightbit indicates.
     *  Slang used its own replacements, so for USE_SLANG blocking here
     *  is not necessary to protect terminals from those characters.
     *  They should have been filtered out or translated by an earlier
     *  processing stage anyway. - kw
     */
#ifndef   EBCDIC  /* S/390 -- gil -- 1514 */
    if (UCH(ch) >= 128 && HTCJK == NOCJK &&
	!text->T.transp && !text->T.output_utf8 &&
	UCH(ch) < LYlowest_eightbit[current_char_set])
	return;
#endif /* EBCDIC */
#endif /* !USE_SLANG */
    if (UCH(ch) == 155 && HTCJK == NOCJK) {	/* octal 233 */
	if (!HTPassHighCtrlRaw &&
	    !text->T.transp && !text->T.output_utf8 &&
	    (155 < LYlowest_eightbit[current_char_set])) {
	    return;
	}
    }

    line = text->last_line;
    style = text->style;

    indent = text->in_line_1 ? (int)style->indent1st : (int)style->leftIndent;
    utfx = utfxtra_on_this_line;

    if (HTCJK != NOCJK) {
	switch(text->state) {
	    case S_text:
		if (ch == CH_ESC) {  /* S/390 -- gil -- 1536 */
		    /*
		    **  Setting up for CJK escape sequence handling (based on
		    **  Takuya ASADA's (asada@three-a.co.jp) CJK Lynx). - FM
		    */
		    text->state = S_esc;
		    text->kanji_buf = '\0';
		    return;
		}
		break;

	    case S_esc:
		/*
		 *  Expecting '$'or '(' following CJK ESC.
		 */
		if (ch == '$') {
		    text->state = S_dollar;
		    return;
		} else if (ch == '(') {
		    text->state = S_paren;
		    return;
		} else {
		    text->state = S_text;
		}
		/* FALLTHRU */

	    case S_dollar:
		/*
		 *  Expecting '@', 'B', 'A' or '(' after CJK "ESC$".
		 */
		if (ch == '@' || ch == 'B' || ch=='A') {
		    text->state = S_nonascii_text;
		    if (ch == '@' || ch == 'B')
			text->kcode = JIS;
		    return;
		} else if (ch == '(') {
		    text->state = S_dollar_paren;
		    return;
		} else {
		    text->state = S_text;
		}
		break;

	    case S_dollar_paren:
		/*
		 * Expecting 'C' after CJK "ESC$(".
		 */
		if (ch == 'C') {
		    text->state = S_nonascii_text;
		    return;
		} else {
		    text->state = S_text;
		}
		break;

	    case S_paren:
		/*
		 *  Expecting 'B', 'J', 'T' or 'I' after CJK "ESC(".
		 */
		if (ch == 'B' || ch == 'J' || ch == 'T')  {
		    /*
		     *  Can split here. - FM
		     */
		    text->permissible_split = text->last_line->size;
		    text->state = S_text;
		    return;
		} else if (ch == 'I')  {
		    text->state = S_jisx0201_text;
		    /*
		     *  Can split here. - FM
		     */
		    text->permissible_split = text->last_line->size;
		    text->kcode = JIS;
		    return;
		} else {
		    text->state = S_text;
		}
		break;

	    case S_nonascii_text:
		/*
		 *  Expecting CJK ESC after non-ASCII text.
		 */
		if (ch == CH_ESC) {  /* S/390 -- gil -- 1553 */
		    text->state = S_esc;
		    text->kanji_buf = '\0';
		    if (HTCJK == JAPANESE) {
			text->kcode = NOKANJI;
		    }
		    return;
		} else if ((0 <= ch) && (ch < 32)) {
		    text->state = S_text;
		    text->kanji_buf = '\0';
		    if (HTCJK == JAPANESE) {
			text->kcode = NOKANJI;
		    }
		} else {
		    ch |= 0200;
		}
		break;

		/*
		 *  JIS X0201 Kana in JIS support. - by ASATAKU
		 */
	    case S_jisx0201_text:
		if (ch == CH_ESC) {  /* S/390 -- gil -- 1570 */
		    text->state = S_esc;
		    text->kanji_buf = '\0';
		    text->kcode = NOKANJI;
		    return;
		} else {
		    text->kanji_buf = '\216';
		    ch |= 0200;
#if 0 /* This conversion is done after. --TH */
#ifdef SH_EX
		    /**** Add Next Line by patakuti ****/
		    text->permissible_split = (int)text->last_line->size;
		    {
			unsigned char hi, low;
			JISx0201TO0208_EUC(0x8e, ch, &hi, &low);
			text->kanji_buf = hi;
			ch = low;
		}
#endif
#endif
	}
		break;
	} /* end switch */

	if (!text->kanji_buf) {
	    if ((ch & 0200) != 0) {
		/*
		 *  JIS X0201 Kana in SJIS support. - by ASATAKU
		 */
		if ((text->kcode != JIS)
		 && (
#ifdef KANJI_CODE_OVERRIDE
		    (last_kcode == SJIS) ||
		     ((last_kcode == NOKANJI) &&
#endif
		      ((text->kcode == SJIS) ||
#ifdef USE_TH_JP_AUTO_DETECT
		       ((text->detected_kcode == DET_SJIS) &&
			(text->specified_kcode == NOKANJI)) ||
#endif
		       ((text->kcode == NOKANJI) &&
			(text->specified_kcode == SJIS)) )
#ifdef KANJI_CODE_OVERRIDE
		     )
#endif
		    ) &&
		    (UCH(ch) >= 0xA1) &&
		    (UCH(ch) <= 0xDF)) {
#ifdef CONV_JISX0201KANA_JISX0208KANA
		    unsigned char c = UCH(ch);
		    unsigned char kb = UCH(text->kanji_buf);
		    JISx0201TO0208_SJIS(c,
					(unsigned char *)&kb,
					(unsigned char *)&c);
		    ch = (char)c;
		    text->kanji_buf = kb;
#endif
		    /* 1998/01/19 (Mon) 09:06:15 */
		    text->permissible_split = (int)text->last_line->size;
		} else {
		    text->kanji_buf = ch;
		    /*
		     *  Can split here. - FM
		     */
		    text->permissible_split = text->last_line->size;
		    return;
		}
	    }
	} else {
	    goto check_WrapSource;
	}
    } else if (ch == CH_ESC) {  /* S/390 -- gil -- 1587 */
	return;
    }

#ifdef CJK_EX	/* MOJI-BAKE Fix! 1997/10/12 -- 10/31 (Fri) 00:22:57 - JH7AYN */
    if (HTCJK != NOCJK &&	/* added condition - kw */
	(ch == LY_BOLD_START_CHAR || ch == LY_BOLD_END_CHAR)) {
	text->permissible_split = (int)line->size;	/* Can split here */
	if (HTCJK == JAPANESE)
	    text->kcode = NOKANJI;
    }
#endif

    if (IsSpecialAttrChar(ch) && ch != LY_SOFT_NEWLINE) {
#if !defined(USE_COLOR_STYLE) || !defined(NO_DUMP_WITH_BACKSPACES)
	if (line->size >= (MAX_LINE-1)) return;
#if defined(USE_COLOR_STYLE) && !defined(NO_DUMP_WITH_BACKSPACES)
	if (with_backspaces && HTCJK==NOCJK && !text->T.output_utf8) {
#endif
	if (ch == LY_UNDERLINE_START_CHAR) {
	    line->data[line->size++] = LY_UNDERLINE_START_CHAR;
	    line->data[line->size] = '\0';
	    underline_on = ON;
	    if (!(dump_output_immediately && use_underscore))
		ctrl_chars_on_this_line++;
	    return;
	} else if (ch == LY_UNDERLINE_END_CHAR) {
	    line->data[line->size++] = LY_UNDERLINE_END_CHAR;
	    line->data[line->size] = '\0';
	    underline_on = OFF;
	    if (!(dump_output_immediately && use_underscore))
		ctrl_chars_on_this_line++;
	    return;
	} else if (ch == LY_BOLD_START_CHAR) {
	    line->data[line->size++] = LY_BOLD_START_CHAR;
	    line->data[line->size] = '\0';
	    bold_on = ON;
	    ctrl_chars_on_this_line++;
	    return;
	} else if (ch == LY_BOLD_END_CHAR) {
	    line->data[line->size++] = LY_BOLD_END_CHAR;
	    line->data[line->size] = '\0';
	    bold_on = OFF;
	    ctrl_chars_on_this_line++;
	    return;
	} else if (ch == LY_SOFT_HYPHEN) {
	    int i;

	    /*
	     *  Ignore the soft hyphen if it is the first character
	     *  on the line, or if it is preceded by a space or
	     *  hyphen. - FM
	     */
	    if (line->size < 1 || text->permissible_split >= line->size)
		return;

	    for (i = (text->permissible_split + 1); line->data[i]; i++) {
		if (!IsSpecialAttrChar(UCH(line->data[i])) &&
		    !isspace(UCH(line->data[i])) &&
		    UCH(line->data[i]) != '-' &&
		    UCH(line->data[i]) != HT_NON_BREAK_SPACE &&
		    UCH(line->data[i]) != HT_EN_SPACE) {
		    break;
		}
	    }
	    if (line->data[i] == '\0') {
		return;
	    }
	}
#if defined(USE_COLOR_STYLE) && !defined(NO_DUMP_WITH_BACKSPACES)
	} /* if (with_backspaces && HTCJK==HTNOCJK && !text->T.output_utf8) */
	 else
	     return;
#endif

#else
	return;
#endif
    } else if (ch == LY_SOFT_NEWLINE) {
	line->data[line->size++] = LY_SOFT_NEWLINE;
	line->data[line->size] = '\0';
	return;
    }

    if (text->T.output_utf8) {
	/*
	 *  Some extra checks for UTF-8 output here to make sure
	 *  memory is not overrun.  For a non-first char, append
	 *  to the line here and return. - kw
	 */
	if (IS_UTF_EXTRA(ch)) {
	    if ((line->size > (MAX_LINE-1))
		|| (indent + (int)(line->offset + line->size) +
		    utfxtra_on_this_line - ctrl_chars_on_this_line +
		    ((line->size > 0) &&
		     (int)(line->data[line->size-1] ==
				LY_SOFT_HYPHEN ?
					     1 : 0)) >= (LYcols_cu-1))
		) {
		if (!text->permissible_split || text->source) {
		    text->permissible_split = line->size;
		    while (text->permissible_split > 0 &&
			   IS_UTF_EXTRA(line->data[text->permissible_split-1]))
			text->permissible_split--;
		    if (text->permissible_split &&
			(line->data[text->permissible_split-1] & 0x80))
			text->permissible_split--;
		    if (text->permissible_split == line->size)
			text->permissible_split = 0;
		}
		split_line(text, text->permissible_split);
		line = text->last_line;
		if (text->source && line->size - ctrl_chars_on_this_line
		    + utfxtra_on_this_line == 0)
		    HText_appendCharacter (text, LY_SOFT_NEWLINE);
	    }
	    line->data[line->size++] = ch;
	    line->data[line->size] = '\0';
	    utfxtra_on_this_line++;
	    ctrl_chars_on_this_line++;
	    return;
	} else if (ch & 0x80) {	/* a first char of UTF-8 sequence - kw */
	    if ((line->size > (MAX_LINE-7))
#if 0	/* the equivalent check should already happen below */
		|| (indent + (int)(line->offset + line->size) +
		    utfxtra_on_this_line - ctrl_chars_on_this_line +
		    ((line->size > 0) &&
		     (int)(line->data[line->size-1] ==
				LY_SOFT_HYPHEN ?
					     1 : 0)) >= (LYcols_cu))
#endif /* 0 */
		) {
		if (!text->permissible_split || text->source) {
		    text->permissible_split = line->size;
		    while (text->permissible_split > 0 &&
			   (line->data[text->permissible_split-1] & 0x80)
			   == 0xC0) {
			text->permissible_split--;
		    }
		    if (text->permissible_split == line->size)
			text->permissible_split = 0;
		}
		split_line(text, text->permissible_split);
		line = text->last_line;
		if (text->source && line->size - ctrl_chars_on_this_line
		    + utfxtra_on_this_line == 0)
		    HText_appendCharacter (text, LY_SOFT_NEWLINE);
	    }
	}
    }

    /*
     *  New Line.
     */
    if (ch == '\n') {
	    new_line(text);
	    text->in_line_1 = YES;	/* First line of new paragraph */
	    /*
	     *  There are some pages written in
	     *  different kanji codes. - TA & kw
	     */
	    if (HTCJK == JAPANESE)
		text->kcode = NOKANJI;
	    return;
    }

    /*
     *  Convert EN_SPACE to a space here so that it doesn't get collapsed.
     */
    if (ch == HT_EN_SPACE)
	ch = ' ';

#ifdef SH_EX	/* 1997/11/01 (Sat) 12:08:54 */
    if (ch == 0x0b) {	/* ^K ??? */
	ch = '\r';
    }
    if (ch == 0x1a) {	/* ^Z ??? */
	ch = '\r';
    }
#endif

    /*
     *  I'm going to cheat here in a BIG way.  Since I know that all
     *  \r's will be trapped by HTML_put_character I'm going to use
     *  \r to mean go down a line but don't start a new paragraph.
     *  i.e., use the second line indenting.
     */
    if (ch == '\r') {
	new_line(text);
	text->in_line_1 = NO;
	/*
	 *  There are some pages written in
	 *  different kanji codes. - TA & kw
	 */
	if (HTCJK == JAPANESE)
	    text->kcode = NOKANJI;
	return;
    }


    /*
     *  Tabs.
     */
    if (ch == '\t') {
	CONST HTTabStop * Tab;
	int target, target_cu;	/* Where to tab to */
	int here, here_cu;	/* in _cu we try to guess what curses thinks */

	if (line->size > 0 && line->data[line->size-1] == LY_SOFT_HYPHEN) {
	    /*
	     *  A tab shouldn't follow a soft hyphen, so
	     *  if one does, we'll dump the soft hyphen. - FM
	     */
	    line->data[--line->size] = '\0';
	    ctrl_chars_on_this_line--;
	}
	here = ((int)(line->size + line->offset) + indent)
		- ctrl_chars_on_this_line; /* Consider special chars GAB */
	here_cu = here + utfxtra_on_this_line;
	if (style->tabs) {	/* Use tab table */
	    for (Tab = style->tabs;
		Tab->position <= here;
		Tab++) {
		if (!Tab->position) {
		    new_line(text);
		    return;
		}
	    }
	    target = Tab->position;
	} else if (text->in_line_1) {	/* Use 2nd indent */
	    if (here >= (int)style->leftIndent) {
		new_line(text); /* wrap */
		return;
	    } else {
		target = (int)style->leftIndent;
	    }
	} else {		/* Default tabs align with left indent mod 8 */
#ifdef DEFAULT_TABS_8
	    target = (((int)line->offset + (int)line->size + 8) & (-8))
			+ (int)style->leftIndent;
#else
	    new_line(text);
	    return;
#endif
	}

	if (target >= here)
	    target_cu = target;
	else
	    target_cu = target + (here_cu - here);

	if (target > (LYcols-1) - (int)style->rightIndent &&
	    HTOutputFormat != WWW_SOURCE) {
	    new_line(text);
	} else {
	    /*
	     *  Can split here. - FM
	     */
	    text->permissible_split = line->size;
	    if (target_cu > (LYcols-1))
		target -= target_cu - (LYcols-1);
	    if (line->size == 0) {
		line->offset = line->offset + target - here;
	    } else {
		for (; here<target; here++) {
		    /* Put character into line */
		    line->data[line->size++] = ' ';
		    line->data[line->size] = '\0';
		}
	    }
	}
	return;
    } /* if tab */

check_WrapSource:
    if ( (text->source || dont_wrap_pre) && text == HTMainText) {
	/*
	 * If we're displaying document source, wrap long lines to keep all of
	 * the source visible.
	 */
	int target = (int)(line->offset + line->size) - ctrl_chars_on_this_line;
	int target_cu = target + utfxtra_on_this_line;
	if (target >= (LYcols-1) - style->rightIndent -
	    (((HTCJK != NOCJK) && text->kanji_buf) ? 1 : 0) ||
	    (text->T.output_utf8 &&
	     target_cu + UTF_XLEN(ch) >= (LYcols_cu-1))
	    ) {
	    int saved_kanji_buf;
	    int saved_state;

	    new_line(text);
	    line = text->last_line;

	    saved_kanji_buf = text->kanji_buf;
	    saved_state = text->state;
	    text->kanji_buf = '\0';
	    text->state = S_text;
	    HText_appendCharacter (text, LY_SOFT_NEWLINE);
	    text->kanji_buf = saved_kanji_buf;
	    text->state = saved_state;
	}
    }

    if (ch == ' ') {
	/*
	 *  Can split here. - FM
	 */
	text->permissible_split = text->last_line->size;
	/*
	 *  There are some pages written in
	 *  different kanji codes. - TA
	 */
	if (HTCJK == JAPANESE)
	    text->kcode = NOKANJI;
    }

    /*
     *  Check if we should ignore characters at the wrap point.
     */
    if (text->IgnoreExcess &&
	(((indent + (int)line->offset + (int)line->size) +
	  (int)style->rightIndent - ctrl_chars_on_this_line) >= (LYcols-1) ||
	 ((indent + (int)line->offset + (int)line->size) +
	  utfxtra_on_this_line - ctrl_chars_on_this_line) >= (LYcols_cu-1)))
	return;

    /*
     *  Check for end of line.
     */
    if (((indent + (int)line->offset + (int)line->size) +
	 (int)style->rightIndent - ctrl_chars_on_this_line +
	 (((HTCJK != NOCJK) && text->kanji_buf) ? 1 : 0) +
	 ((line->size > 0) &&
	  (int)(line->data[line->size-1] ==
				LY_SOFT_HYPHEN ?
					     1 : 0))) >= (LYcols - 1) ||
	(text->T.output_utf8 &&
	 (((indent + (int)line->offset + (int)line->size) +
	   utfxtra_on_this_line - ctrl_chars_on_this_line +
	   UTF_XLEN(ch) +
	   ((line->size > 0) &&
	    (int)(line->data[line->size-1] ==
				LY_SOFT_HYPHEN ?
					     1 : 0))) >= (LYcols_cu - 1)))) {

	if (style->wordWrap && HTOutputFormat != WWW_SOURCE) {
#ifdef EXP_JUSTIFY_ELTS
	    if (REALLY_CAN_JUSTIFY(text))
		this_line_was_split = TRUE;
#endif
	    split_line(text, text->permissible_split);
	    if (ch == ' ') return;	/* Ignore space causing split */

	}  else if (HTOutputFormat == WWW_SOURCE) {
		 /*
		  *  For source output we don't want to wrap this stuff
		  *  unless absolutely necessary. - LJM
		  *  !
		  *  If we don't wrap here we might get a segmentation fault.
		  *  but let's see what happens
		  */
		if ((int)line->size >= (int)(MAX_LINE-1))
		   new_line(text);  /* try not to linewrap */
	} else {
		/*
		 *  For normal stuff like pre let's go ahead and
		 *  wrap so the user can see all of the text.
		 */

		if ( (dump_output_immediately|| (crawl && traversal) )
		     && dont_wrap_pre) {
		    if ((int)line->size >= (int)(MAX_LINE-1))
			new_line(text);
		} else
		    new_line(text);

	}
    } else if ((int)line->size >= (int)(MAX_LINE-1)) {
	/*
	 *  Never overrun memory if LYcols is set to a large value - KW
	 */
	new_line(text);
    }

    /*
     *  Insert normal characters.
     */
    if (ch == HT_NON_BREAK_SPACE
#ifdef EXP_JUSTIFY_ELTS
     && !REALLY_CAN_JUSTIFY(text)
#endif
     )
	ch = ' ';
#ifdef EXP_JUSTIFY_ELTS
    else
	have_raw_nbsps = TRUE;
#endif

    /* we leave raw HT_NON_BREAK_SPACE otherwise (we'll substitute it later) */

    if (ch & 0x80)
	text->have_8bit_chars = YES;

    /*
     * Kanji charactor handling.
     */
    {
	HTFont font = style->font;
	unsigned char hi, lo, tmp[2];

	line = text->last_line; /* May have changed */

	if (HTCJK != NOCJK && text->kanji_buf) {
	    hi = UCH(text->kanji_buf);
	    lo = UCH(ch);

	    if (HTCJK == JAPANESE) {
		if (text->kcode != JIS) {
		    if (IS_SJIS_2BYTE(hi, lo)) {
			if (IS_EUC(hi, lo)) {
#ifdef KANJI_CODE_OVERRIDE
			    if (last_kcode != NOKANJI)
				text->kcode = last_kcode;
			    else
#endif
				if (text->specified_kcode != NOKANJI)
				    text->kcode = text->specified_kcode;
#ifdef USE_TH_JP_AUTO_DETECT
				else if (text->detected_kcode == DET_EUC)
				    text->kcode = EUC;
				else if (text->detected_kcode == DET_SJIS)
				    text->kcode = SJIS;
#endif
				else if (IS_EUC_X0201KANA(hi, lo) && (text->kcode != EUC))
				    text->kcode = SJIS;
			}
			else
			    text->kcode = SJIS;
		    }
		    else if (IS_EUC(hi, lo))
			text->kcode = EUC;
		    else
			text->kcode = NOKANJI;
		}

#if 0 /* This judgement routine is replaced by above one. -- TH */
		if (text->kcode == NOKANJI)
		{
		    if (IS_SJIS(hi, lo, text->in_sjis) && IS_EUC(hi, lo)) {
			text->kcode = NOKANJI;
		    } else if (IS_SJIS(hi, lo, text->in_sjis)) {
			text->kcode = SJIS;
		    } else if (IS_EUC(hi, lo)) {
			text->kcode = EUC;
		    }
		}
#endif

		switch (kanji_code) {
		case EUC:
		    if (text->kcode == SJIS) {
			SJIS_TO_EUC1(hi, lo, tmp);
			line->data[line->size++] = tmp[0];
			line->data[line->size++] = tmp[1];
		    } else if (IS_EUC(hi, lo)) {
#ifdef CONV_JISX0201KANA_JISX0208KANA
			JISx0201TO0208_EUC(hi, lo, &hi, &lo);
#endif
			line->data[line->size++] = hi;
			line->data[line->size++] = lo;
		    } else {
			CTRACE((tfp, "This character (%X:%X) doesn't seem Japanese\n", hi, lo));
			line->data[line->size++] = '=';
			line->data[line->size++] = '=';
		    }
		    break;

		case SJIS:
		    if ((text->kcode == EUC) || (text->kcode == JIS))
		    {
#ifndef CONV_JISX0201KANA_JISX0208KANA
			if (IS_EUC_X0201KANA(hi, lo))
			    line->data[line->size++] = lo;
			else
#endif
			{
			    EUC_TO_SJIS1(hi, lo, tmp);
			    line->data[line->size++] = tmp[0];
			    line->data[line->size++] = tmp[1];
			}
		    } else if (IS_SJIS_2BYTE(hi, lo)) {
			line->data[line->size++] = hi;
			line->data[line->size++] = lo;
		    } else {
			line->data[line->size++] = '=';
			line->data[line->size++] = '=';
			CTRACE((tfp, "This character (%X:%X) doesn't seem Japanese\n", hi, lo));
		    }
		    break;

		default:
		    break;
		}
	    } else {
		line->data[line->size++] = hi;
		line->data[line->size++] = lo;
	    }
	    text->kanji_buf = 0;
	}
#if 0
	if (HTCJK != NOCJK && text->kanji_buf) {
	    hi = UCH(text->kanji_buf), lo = UCH(ch);
	    if (HTCJK == JAPANESE && text->kcode == NOKANJI) {
		if (IS_SJIS(hi, lo, text->in_sjis) && IS_EUC(hi, lo)) {
		    text->kcode = NOKANJI;
		} else if (IS_SJIS(hi, lo, text->in_sjis)) {
		    text->kcode = SJIS;
		} else if (IS_EUC(hi, lo)) {
		    text->kcode = EUC;
		}
	    }
	    if (HTCJK == JAPANESE &&
		(kanji_code == EUC) && (text->kcode == SJIS)) {
		SJIS_TO_EUC1(hi, lo, tmp);
		line->data[line->size++] = tmp[0];
		line->data[line->size++] = tmp[1];
	    } else if (HTCJK == JAPANESE &&
		       (kanji_code == EUC) && (text->kcode == EUC)) {
#ifdef CONV_JISX0201KANA_JISX0208KANA
		JISx0201TO0208_EUC(hi, lo, &hi, &lo);
#endif
		line->data[line->size++] = hi;
		line->data[line->size++] = lo;
	    } else if (HTCJK == JAPANESE &&
		       (kanji_code == SJIS) && (text->kcode == EUC)) {
		EUC_TO_SJIS1(hi, lo, tmp);
		line->data[line->size++] = tmp[0];
		line->data[line->size++] = tmp[1];
	    } else {
		line->data[line->size++] = hi;
		line->data[line->size++] = lo;
	    }
	    text->kanji_buf = 0;
	}
#endif
#ifndef CONV_JISX0201KANA_JISX0208KANA
	else if ((HTCJK == JAPANESE) && IS_SJIS_X0201KANA(UCH((ch))) &&
		 (kanji_code == EUC)) {
	    line->data[line->size++] = UCH(0x8e);
	    line->data[line->size++] = ch;
	}
#endif
	else if (HTCJK != NOCJK) {
	    line->data[line->size++] = (char) (
				       (kanji_code != NOKANJI) ?
							    ch :
					  (font & HT_CAPITALS) ?
						   TOUPPER(ch) : ch);
	} else {
	    line->data[line->size++] =	/* Put character into line */
		(char) (font & HT_CAPITALS ? TOUPPER(ch) : ch);
	}
	line->data[line->size] = '\0';
	if (font & HT_DOUBLE)		/* Do again if doubled */
	    HText_appendCharacter(text, HT_NON_BREAK_SPACE);
	    /* NOT a permissible split */

	if (ch == LY_SOFT_HYPHEN) {
	    ctrl_chars_on_this_line++;
	    /*
	     *  Can split here. - FM
	     */
	    text->permissible_split = text->last_line->size;
	}
	if (ch == LY_SOFT_NEWLINE) {
	    ctrl_chars_on_this_line++;
	}
    }
}

#ifdef USE_COLOR_STYLE
/*  Insert a style change into the current line
**  -------------------------------------------
*/
PUBLIC void _internal_HTC ARGS3(HText *,text, int,style, int,dir)
{
    HTLine* line;

    /* can't change style if we have no text to change style with */
    if (text != 0) {

	line = text->last_line;

	if (line->numstyles > 0 && dir == 0 &&
	    line->styles[line->numstyles].direction &&
	    line->styles[line->numstyles].style == style &&
	    (int) line->styles[line->numstyles].horizpos
	    == (int)line->size - ctrl_chars_on_this_line) {
	    /*
	     *  If this is an OFF change directly preceded by an
	     *	ON for the same style, just remove the previous one. - kw
	     */
	    line->numstyles--;
	} else if (line->numstyles < MAX_STYLES_ON_LINE) {
	    line->styles[line->numstyles].horizpos  = line->size;
	    /*
	     *  Special chars for bold and underlining usually don't
	     *  occur with color style, but soft hyphen can.
	     *  And in UTF-8 display mode all non-initial bytes are
	     *  counted as ctrl_chars. - kw
	     */
	    if (line->styles[line->numstyles].horizpos >= ctrl_chars_on_this_line)
		line->styles[line->numstyles].horizpos -= ctrl_chars_on_this_line;
	    line->styles[line->numstyles].style     = style;
	    line->styles[line->numstyles].direction = dir;
	    line->numstyles++;
	}
    }
}
#endif



/*	Set LastChar element in the text object.
**	----------------------------------------
*/
PUBLIC void HText_setLastChar ARGS2(
	HText *,	text,
	char,		ch)
{
    if (!text)
	return;

    text->LastChar = ch;
}

/*	Get LastChar element in the text object.
**	----------------------------------------
*/
PUBLIC char HText_getLastChar ARGS1(
	HText *,	text)
{
    if (!text)
	return('\0');

    return((char)text->LastChar);
}

/*	Set IgnoreExcess element in the text object.
**	--------------------------------------------
*/
PUBLIC void HText_setIgnoreExcess ARGS2(
	HText *,	text,
	BOOL,		ignore)
{
    if (!text)
	return;

    text->IgnoreExcess = ignore;
}

/*		Simple table handling - private
**		-------------------------------
*/

/*
 *  Given a line and two int arrays of old/now position, this function
 *  does the actual work of creating a new line where spaces have been
 *  inserted in appropriate places - so that characters at/after the old
 *  position end up at/after the new position, for each pair, if possible.
 *  Some necessary changes for anchors starting on this line are also done
 *  here if needed.
 *  Returns a newly allocated HTLine* if changes were made
 *    (caller has to free the old one).
 *  Returns NULL if no changes needed.
 * - kw
 */
PRIVATE HTLine * insert_blanks_in_line ARGS6(
    HTLine *,		line,
    int,		line_number,
    HText *,		text,
    int,		ninserts,
    int *,		oldpos,
    int *,		newpos)
{
    int i;
    int ioldb, inewb;		/* count bytes */
    int ioldc = 0, inewc = 0;	/* count visible characters (positions) */
    int ip = 0;			/* count oldpos,newpos insertion pairs */
    int acurshift, ashift = 0;	/* for shifting of anchors in this line */
#if defined(USE_COLOR_STYLE)
    int stcurshift, stshift = 0; /* for shifting of styles in this line */
    int istyle;
#endif
    int added_chars = 0;
    HTLine * mod_line;
    char * newdata;
    TextAnchor * a;
    if (!(line && line->size && ninserts))
	return NULL;
    for (i = 0; i < ninserts; i++)
	if (newpos[i] > oldpos[i] &&
	    (newpos[i] - oldpos[i]) > added_chars)
	    added_chars = newpos[i] - oldpos[i];
    if (line->size + added_chars > MAX_LINE - 2)
	return NULL;
    if (line == text->last_line)
	mod_line = allocHTLine(MAX_LINE);
    else
	mod_line = allocHTLine(line->size + added_chars);
    if (!mod_line)
	return NULL;
    memcpy(mod_line, line, LINE_SIZE(1));
    newdata = mod_line->data;
    for (ioldb = 0, inewb = 0; ioldb < (int)line->size; ioldb++) {
	if ((line->data[ioldb] == LY_BOLD_START_CHAR ||
	     line->data[ioldb] == LY_UNDERLINE_START_CHAR ||
	     !IsSpecialAttrChar(line->data[ioldb])) &&
	    (!(text && text->T.output_utf8) ||
	     UCH(line->data[ioldb]) < 128 ||
	     (UCH((line->data[ioldb] & 0xc0)) == 0xc0))) {
	    /*
	     *  A new displayable character starts here.  Time to check
	     *  whether this is a position to insert spaces.
	     */
	    while (ip < ninserts && oldpos[ip] <= ioldc) {
		if (inewc < newpos[ip]) {
		    /*
		     *  Yup, spaces to insert.  We also have to update
		     *  anchor positions for anchors that start on the
		     *  same line, this is a pain.  Let's do it first.
		     *  Note: we rely on a->line_pos counting bytes, not
		     *  characters.  That's one reason why HText_trimHightext
		     *  has to be prevented from acting on these anchors in
		     *  partial display mode before we get a chance to
		     *  deal with them here.
		     */
		    acurshift = 0;
		    for (a = text->last_anchor_before_stbl ?
			     text->last_anchor_before_stbl->next :
			     text->first_anchor;
			 a && a->line_num <= line_number; a = a->next) {
			if (a->line_num == line_number)
			    if (a->line_pos - ashift >= ioldb) {
				acurshift = newpos[ip] - inewc;
				a->line_pos += acurshift;
				a->start += acurshift;
			    }
		    }
		    ashift += acurshift;

		    /*  doing something for color styles here.
		     *  we rely on horizpos counting characters
		     *  (display positions), not bytes. */
#if defined(USE_COLOR_STYLE)
#define NStyle mod_line->styles[istyle]
		    stcurshift = 0;
		    for (istyle = 0; istyle < line->numstyles;
			 istyle++) {
			if (NStyle.horizpos - stshift >= ioldc) {
			    stcurshift = newpos[ip] - inewc;
			    NStyle.horizpos += stcurshift;
			}
		    }
		    stshift += stcurshift;
#endif
		    /*
		     *  Now insert the spaces.
		     */
		    for (; inewc < newpos[ip]; inewc++) {
			newdata[inewb++] = ' ';
		    }
		}
		ip++;	/* done with this oldpos[ip],newpos[ip] pair. */
	    }
	    /*
	     *  Copy character data over, advance position pointers.
	     */
	    newdata[inewb++] = line->data[ioldb];
	    if (!(line->data[ioldb] == LY_BOLD_START_CHAR ||
		  line->data[ioldb] == LY_UNDERLINE_START_CHAR)) {
		ioldc++;
		inewc++;
	    }
	} else {
	    /*
	     *  Just copy special attribute chars and utf-8 additional
	     *  bytes over, don't advance position pointers.
	     */
	    newdata[inewb++] = line->data[ioldb];
	}
    }
    newdata[inewb] = '\0';
    mod_line->size = inewb;
    return mod_line;
}

/*
 *  HText_insertBlanksInStblLines fixes up table lines when simple table
 *  processing is closed, by calling insert_blanks_in_line for lines
 *  that need fixup.  Also recalculates alignment for those lines,
 *  does additional updating of anchor positions, and makes sure the
 *  display of the lines on screen will be updated after partial display
 *  upon return to mainloop. - kw
 */
PRIVATE int HText_insertBlanksInStblLines ARGS2(
    HText *,		me,
    int,		ncols)
{
    HTLine *line;
    HTLine *mod_line, *first_line = NULL;
    int *	oldpos;
    int *	newpos;
    int		ninserts, lineno;
    int		first_lineno, last_lineno, first_lineno_pass2;
    int		added_chars_before = 0;
    TextAnchor * a;
    int lines_changed = 0;
    int max_width = 0, indent, spare, table_offset;
    HTStyle *style;
    short alignment;
    int i = 0;

    lineno = first_lineno = Stbl_getStartLine(me->stbl);
    if (lineno < 0 || lineno > me->Lines)
	return -1;
    /*
     *  oldpos, newpos: allocate space for two int arrays.
     */
    oldpos = typecallocn(int, 2 * ncols);
    if (!oldpos)
	return -1;
    else
	newpos = oldpos + ncols;
    for (line = me->last_line->next; i < lineno; line = line->next, i++) {
	if (!line) {
	    free(oldpos);
	    return -1;
	}
    }
    first_lineno_pass2 = last_lineno = me->Lines;
    for (; line && lineno <= last_lineno; line = line->next, lineno++) {
	if (added_chars_before) {
	    for (a = me->last_anchor_before_stbl ?
		     me->last_anchor_before_stbl->next : me->first_anchor;
		 a && a->line_num <= lineno; a = a->next) {
		if (a->line_num == lineno)
		    a->start += added_chars_before;
	    }
	}
	ninserts = Stbl_getFixupPositions(me->stbl, lineno, oldpos, newpos);
	if (ninserts < 0)
	    continue;
	if (!first_line) {
	    first_line = line;
	    first_lineno_pass2 = lineno;
	    if (TRACE) {
		int ip;
		CTRACE((tfp, "line %d first to adjust  --  newpos:",
		       lineno));
		for (ip = 0; ip < ncols; ip++)
		    fprintf(tfp, " %d", newpos[ip]);
		fprintf(tfp, "\r\n");
	    }
	}
	if (line == me->last_line) {
	    if (line->size == 0 || !HText_TrueLineSize(line, me, FALSE))
		continue;
	    /*
	     *  Last ditch effort to end the table with a line break,
	     *  if HTML_end_element didn't do it. - kw
	     */
	    if (first_line == line) /* obscure: all table on last line... */
		first_line = NULL;
	    new_line(me);
	    line = me->last_line->prev;
	    if (first_line == NULL)
		first_line = line;
	}
	if (ninserts == 0) {
	    /*  Do it also for no positions (but not error) */
	    int width = HText_TrueLineSize(line, me, FALSE);
	    if (width > max_width)
		max_width = width;
	    CTRACE((tfp, "line %d true/max width:%d/%d oldpos: NONE\r\n",
		   lineno, width, max_width));
	    continue;
	}
	mod_line = insert_blanks_in_line(line, lineno, me,
					 ninserts, oldpos, newpos);
	if (mod_line) {
	    if (line == me->last_line) {
		me->last_line = mod_line;
	    } else {
		me->chars += (mod_line->size - line->size);
		added_chars_before += (mod_line->size - line->size);
	    }
	    line->prev->next = mod_line;
	    line->next->prev = mod_line;
	    lines_changed++;
	    if (line == first_line)
		first_line = mod_line;
	    free(line);
	    line = mod_line;
#ifdef DISP_PARTIAL
	    /*
	     *  Make sure modified lines get fully re-displayed after
	     *  loading with partial display is done.
	     */
	    if (me->first_lineno_last_disp_partial >= 0) {
		if (me->first_lineno_last_disp_partial >= lineno) {
		    me->first_lineno_last_disp_partial =
			me->last_lineno_last_disp_partial = -1;
		} else if (me->last_lineno_last_disp_partial >= lineno) {
		    me->last_lineno_last_disp_partial = lineno - 1;
		}
	    }
#endif
	}
	{
	    int width = HText_TrueLineSize(line, me, FALSE);
	    if (width > max_width)
		max_width = width;
	    if (TRACE) {
		int ip;
		CTRACE((tfp, "line %d true/max width:%d/%d oldpos:",
		       lineno, width, max_width));
		for (ip = 0; ip < ninserts; ip++)
		    fprintf(tfp, " %d", oldpos[ip]);
		fprintf(tfp, "\r\n");
	    }
	}
    }
    /*
     *  Line offsets have been set based on the paragraph style, and
     *  have already been updated for centering or right-alignment
     *  for each line in split_line.  Here we want to undo all that, and
     *  align the table as a whole (i.e. all lines for which
     *  Stbl_getFixupPositions returned >= 0).  All those lines have to
     *  get the same offset, for the simple table formatting mechanism
     *  to make sense, and that may not actually be the case at this point.
     *
     *  What indentation and alignment do we want for the table as
     *  a whole?  Let's take most style properties from me->style.
     *  With some luck, it is the appropriate style for the element
     *  enclosing the TABLE.  But let's take alignment from the attribute
     *  of the TABLE itself instead, if it was specified.
     *
     *  Note that this logic assumes that all lines have been finished
     *  by split_line.  The order of calls made by HTML_end_element for
     *  HTML_TABLE should take care of this.
     */
    style = me->style;
    alignment = Stbl_getAlignment(me->stbl);
    if (alignment == HT_ALIGN_NONE)
	alignment = style->alignment;
    indent = style->leftIndent;
    /* Calculate spare character positions */
    spare = (LYcols-1) -
	(int)style->rightIndent - indent - max_width;
    if (spare < 0 && (int)style->rightIndent + spare >= 0) {
	/*
	 *  Not enough room!  But we can fit if we ignore right indentation,
	 *  so let's do that.
	 */
	spare = 0;
    } else if (spare < 0) {
	spare += style->rightIndent; /* ignore right indent, but need more */
    }
    if (spare < 0 && indent + spare >= 0) {
	/*
	 *  Still not enough room.  But we can move to the left.
	 */
	indent += spare;
	spare = 0;
    } else if (spare < 0) {
	/*
	 *  Still not enough.  Something went wrong.  Try the best we
	 *  can do.
	 */
	CTRACE((tfp, "insertBlanks: resulting table too wide by %d positions!",
	       -spare));
	indent = spare = 0;
    }
    /*
     *  Align left, right or center.
     */
    switch (alignment) {
	case HT_CENTER :
	    table_offset = indent + spare/2;
	    break;
	case HT_RIGHT :
	    table_offset = indent + spare;
	    break;
	case HT_LEFT :
	case HT_JUSTIFY :
	default:
	    table_offset = indent;
	    break;
    } /* switch */

    CTRACE((tfp, "changing offsets"));
    for (line = first_line, lineno = first_lineno_pass2;
	 line && lineno <= last_lineno && line != me->last_line;
	 line = line->next, lineno++) {
	ninserts = Stbl_getFixupPositions(me->stbl, lineno, oldpos, newpos);
	if (ninserts >= 0 && (int) line->offset != table_offset) {
#ifdef DISP_PARTIAL
	    /*  As above make sure modified lines get fully re-displayed */
	    if (me->first_lineno_last_disp_partial >= 0) {
		if (me->first_lineno_last_disp_partial >= lineno) {
		    me->first_lineno_last_disp_partial =
			me->last_lineno_last_disp_partial = -1;
		} else if (me->last_lineno_last_disp_partial >= lineno) {
		    me->last_lineno_last_disp_partial = lineno - 1;
		}
	    }
#endif
	    CTRACE((tfp, " %d:%d", lineno, table_offset - line->offset));
	    line->offset = table_offset;
	}
    }
    CTRACE((tfp, " %d:done\r\n", lineno));
    free(oldpos);
    return lines_changed;
}

/*		Simple table handling - public functions
**		----------------------------------------
*/

/*	Cancel simple table handling
*/
PUBLIC void HText_cancelStbl ARGS1(
	HText *,	me)
{
    if (!me || !me->stbl) {
	CTRACE((tfp, "cancelStbl: ignored.\n"));
	return;
    }
    CTRACE((tfp, "cancelStbl: ok, will do.\n"));
    Stbl_free(me->stbl);
    me->stbl = NULL;
}

/*	Start simple table handling
*/
PUBLIC void HText_startStblTABLE ARGS2(
	HText *,	me,
	short,		alignment)
{
    if (!me)
	return;
    if (me->stbl)
	HText_cancelStbl(me);	/* auto cancel previously open table */
    me->stbl = Stbl_startTABLE(alignment);
    if (me->stbl) {
	CTRACE((tfp, "startStblTABLE: started.\n"));
	me->last_anchor_before_stbl = me->last_anchor;
    } else {
	CTRACE((tfp, "startStblTABLE: failed.\n"));
    }
}

/*	Finish simple table handling
*/
PUBLIC void HText_endStblTABLE ARGS1(
	HText *,	me)
{
    int ncols, lines_changed = 0;
    if (!me || !me->stbl) {
	CTRACE((tfp, "endStblTABLE: ignored.\n"));
	return;
    }
    CTRACE((tfp, "endStblTABLE: ok, will try.\n"));
    ncols = Stbl_finishTABLE(me->stbl);
    CTRACE((tfp, "endStblTABLE: ncols = %d.\n", ncols));
    if (ncols > 0) {
	lines_changed = HText_insertBlanksInStblLines(me, ncols);
	CTRACE((tfp, "endStblTABLE: changed %d lines, done.\n", lines_changed));
#ifdef DISP_PARTIAL
	/* allow HTDisplayPartial() to redisplay the changed lines.
	 * There is no harm if we got several stbl in the document, hope so.
	 */
	NumOfLines_partial -= lines_changed;  /* fake */
#endif  /* DISP_PARTIAL */
    }
    Stbl_free(me->stbl);
    me->stbl = NULL;
}

/*	Start simple table row
*/
PUBLIC void HText_startStblTR ARGS2(
	HText *,	me,
	short,		alignment)
{
    if (!me || !me->stbl)
	return;
    if (Stbl_addRowToTable(me->stbl, alignment, me->Lines) < 0)
	HText_cancelStbl(me);	/* give up */
}

/*	Finish simple table row
*/
PUBLIC void HText_endStblTR ARGS1(
	HText *,	me)
{
    if (!me || !me->stbl)
	return;
    /* should this do something?? */
}

/*	Start simple table cell
*/
PUBLIC void HText_startStblTD ARGS5(
	HText *,	me,
	int,		colspan,
	int,		rowspan,
	short,		alignment,
	BOOL,		isheader)
{
    if (!me || !me->stbl)
	return;
    if (colspan < 0)
	colspan = 1;
    if (colspan > TRST_MAXCOLSPAN) {
	CTRACE((tfp, "*** COLSPAN=%d is too large, ignored!\n", colspan));
	colspan = 1;
    }
    if (rowspan > TRST_MAXROWSPAN) {
	CTRACE((tfp, "*** ROWSPAN=%d is too large, ignored!\n", rowspan));
	rowspan = 1;
    }
    if (Stbl_addCellToTable(me->stbl, colspan, rowspan, alignment, isheader,
			    me->Lines, HText_LastLineSize(me,FALSE)) < 0)
	HText_cancelStbl(me);	/* give up */
}

/*	Finish simple table cell
*/
PUBLIC void HText_endStblTD ARGS1(
	HText *,	me)
{
    if (!me || !me->stbl)
	return;
    if (Stbl_finishCellInTable(me->stbl, YES,
			       me->Lines, HText_LastLineSize(me,FALSE)) < 0)
	HText_cancelStbl(me);	/* give up */
}

/*	Remember COL info / Start a COLGROUP and remember info
*/
PUBLIC void HText_startStblCOL ARGS4(
	HText *,	me,
	int,		span,
	short,		alignment,
	BOOL,		isgroup)
{
    if (!me || !me->stbl)
	return;
    if (span <= 0)
	span = 1;
    if (span > TRST_MAXCOLSPAN) {
	CTRACE((tfp, "*** SPAN=%d is too large, ignored!\n", span));
	span = 1;
    }
    if (Stbl_addColInfo(me->stbl, span, alignment, isgroup) < 0)
	HText_cancelStbl(me);	/* give up */
}

/*	Finish a COLGROUP
*/
PUBLIC void HText_endStblCOLGROUP ARGS1(
	HText *,	me)
{
    if (!me || !me->stbl)
	return;
    if (Stbl_finishColGroup(me->stbl) < 0)
	HText_cancelStbl(me);	/* give up */
}

/*	Start a THEAD / TFOOT / TBODY - remember its alignment info
*/
PUBLIC void HText_startStblRowGroup ARGS2(
	HText *,	me,
	short,		alignment)
{
    if (!me || !me->stbl)
	return;
    if (Stbl_addRowGroup(me->stbl, alignment) < 0)
	HText_cancelStbl(me);	/* give up */
}

/*		Anchor handling
**		---------------
*/

/*	Start an anchor field
*/
PUBLIC int HText_beginAnchor ARGS3(
	HText *,		text,
	BOOL,			underline,
	HTChildAnchor *,	anc)
{
    char marker[32];

    TextAnchor * a = typecalloc(TextAnchor);

    if (a == NULL)
	outofmem(__FILE__, "HText_beginAnchor");
    a->hightext  = NULL;
    a->hightext2 = NULL;
    a->start = text->chars + text->last_line->size;
    a->inUnderline = underline;

    a->line_num = text->Lines;
    a->line_pos = text->last_line->size;
    if (text->last_anchor) {
	text->last_anchor->next = a;
    } else {
	text->first_anchor = a;
    }
    a->next = 0;
    a->anchor = anc;
    a->extent = 0;
    a->link_type = HYPERTEXT_ANCHOR;
    text->last_anchor = a;

#ifndef DONT_TRACK_INTERNAL_LINKS
    if (HTAnchor_followTypedLink((HTAnchor*)anc, LINK_INTERNAL)) {
	a->number = ++(text->last_anchor_number);
	a->link_type = INTERNAL_LINK_ANCHOR;
    } else
#endif
    if (HTAnchor_followMainLink((HTAnchor*)anc)) {
	a->number = ++(text->last_anchor_number);
    } else {
	a->number = 0;
    }

    /*
     *  If we are doing link_numbering add the link number.
     */
    if ((a->number > 0) &&
#ifdef USE_PRETTYSRC
	(text->source ? !psrcview_no_anchor_numbering : 1 ) &&
#endif
	(keypad_mode == LINKS_ARE_NUMBERED ||
	 keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED)) {
	char saved_lastchar = text->LastChar;
	int saved_linenum = text->Lines;
	sprintf(marker,"[%d]", a->number);
	HText_appendText(text, marker);
	if (saved_linenum && text->Lines && saved_lastchar != ' ')
	    text->LastChar = ']'; /* if marker not after space caused split */
	a->start = text->chars + text->last_line->size;
	a->line_num = text->Lines;
	a->line_pos = text->last_line->size;
    }

    return(a->number);
}

/*
    This returns whether the given anchor has blank content. Shamelessly copied
    from HText_endAnchor. The values returned are meaningful only for "normal"
    links - like ones produced by <a href=".">foo</a>, no inputs, etc. - VH
*/
#ifdef MARK_HIDDEN_LINKS
PUBLIC BOOL HText_isAnchorBlank ARGS2(
	HText *,	text,
	int,		number)
{
    TextAnchor *a;

    /*
     *  The number argument is set to 0 in HTML.c and
     *  LYCharUtils.c when we want to end the anchor
     *  for the immediately preceding HText_beginAnchor()
     *  call.  If it's greater than 0, we want to handle
     *  a particular anchor.  This allows us to set links
     *  for positions indicated by NAME or ID attributes,
     *  without needing to close any anchor with an HREF
     *  within which that link might be embedded. - FM
     */
    if (number <= 0 || number == text->last_anchor->number) {
	a = text->last_anchor;
    } else {
	for (a = text->first_anchor; a; a = a->next) {
	    if (a->number == number) {
		break;
	    }
	}
	if (a == NULL) {
	    /*
	     *  There's no anchor with that number,
	     *  so we'll default to the last anchor,
	     *  and cross our fingers. - FM
	     */
	    a = text->last_anchor;
	}
    }

    CTRACE((tfp, "GridText:HText_isAnchorBlank: number:%d link_type:%d\n",
			a->number, a->link_type));
    if (a->link_type == INPUT_ANCHOR) {
	/*
	 *  Shouldn't happen, but put test here anyway to be safe. - LE
	 */

	CTRACE((tfp,
	   "HText_isAnchorBlank: internal error: last anchor was input field!\n"));
	return 0;
    }
    if (a->number) {
	/*
	 *  If it goes somewhere...
	 */
	int i, j, k;
	HTLine *last = text->last_line;
	HTLine *prev = text->last_line->prev;
	HTLine *start = last;
	int CurBlankExtent = 0;
	int BlankExtent = 0;
	
	int extent_adjust = (text->chars + last->size) - a->start -
		     (text->Lines - a->line_num);

	/*
	 *  Check if the anchor content has only
	 *  white and special characters, starting
	 *  with the content on the last line. - FM
	 */
	a->extent += extent_adjust;
	if (a->extent > (int)last->size) {
	    /*
	     *  The anchor extends over more than one line,
	     *  so set up to check the entire last line. - FM
	     */
	    i = last->size;
	} else {
	    /*
	     *  The anchor is restricted to the last line,
	     *  so check from the start of the anchor. - FM
	     */
	    i = a->extent;
	}
	k = j = (last->size - i);
	while (j < (int)last->size) {
	    if (!IsSpecialAttrChar(last->data[j]) &&
		!isspace(UCH(last->data[j])) &&
		last->data[j] != HT_NON_BREAK_SPACE &&
		last->data[j] != HT_EN_SPACE)
		break;
	    i--;
	    j++;
	}
	if (i == 0) {
	    if (a->extent > (int)last->size) {
		/*
		 *  The anchor starts on a preceding line, and
		 *  the last line has only white and special
		 *  characters, so declare the entire extent
		 *  of the last line as blank. - FM
		 */
		CurBlankExtent = BlankExtent = last->size;
	    } else {
		/*
		 *  The anchor starts on the last line, and
		 *  has only white or special characters, so
		 *  declare the anchor's extent as blank. - FM
		 */
		CurBlankExtent = BlankExtent = a->extent;
	    }
	}
	/*
	 *  While the anchor starts on a line preceding
	 *  the one we just checked, and the one we just
	 *  checked has only white and special characters,
	 *  check whether the anchor's content on the
	 *  immediately preceding line also has only
	 *  white and special characters. - FM
	 */
	while (i == 0 &&
	       (a->extent > CurBlankExtent ||
		(a->extent == CurBlankExtent &&
		 k == 0 &&
		 prev != text->last_line &&
		 (prev->size == 0 ||
		  prev->data[prev->size - 1] == ']')))) {
	    start = prev;
	    k = j = prev->size - a->extent + CurBlankExtent;
	    if (j < 0) {
		/*
		 *  The anchor starts on a preceding line,
		 *  so check all of this line. - FM
		 */
		j = 0;
		i = prev->size;
	    } else {
		/*
		 *  The anchor starts on this line. - FM
		 */
		i = a->extent - CurBlankExtent;
	    }
	    while (j < (int)prev->size) {
		if (!IsSpecialAttrChar(prev->data[j]) &&
		    !isspace(UCH(prev->data[j])) &&
		    prev->data[j] != HT_NON_BREAK_SPACE &&
		    prev->data[j] != HT_EN_SPACE)
		    break;
		i--;
		j++;
	    }
	    if (i == 0) {
		if (a->extent > (CurBlankExtent + (int)prev->size) ||
		    (a->extent == CurBlankExtent + (int)prev->size &&
		     k == 0 &&
		     prev->prev != text->last_line &&
		     (prev->prev->size == 0 ||
		      prev->prev->data[prev->prev->size - 1] == ']'))) {
		    /*
		     *  This line has only white and special
		     *  characters, so treat its entire extent
		     *  as blank, and decrement the pointer for
		     *  the line to be analyzed. - FM
		     */
		    CurBlankExtent += prev->size;
		    BlankExtent = CurBlankExtent;
		    prev = prev->prev;
		} else {
		    /*
		     *  The anchor starts on this line, and it
		     *  has only white or special characters, so
		     *  declare the anchor's extent as blank. - FM
		     */
		    BlankExtent = a->extent;
		    break;
		}
	    }
	}
	a->extent -= extent_adjust;
	return i==0;
    } else
	return 0;
}
#endif /* MARK_HIDDEN_LINKS */


PUBLIC void HText_endAnchor ARGS2(
	HText *,	text,
	int,		number)
{
    TextAnchor *a;

    /*
     *  The number argument is set to 0 in HTML.c and
     *  LYCharUtils.c when we want to end the anchor
     *  for the immediately preceding HText_beginAnchor()
     *  call.  If it's greater than 0, we want to handle
     *  a particular anchor.  This allows us to set links
     *  for positions indicated by NAME or ID attributes,
     *  without needing to close any anchor with an HREF
     *  within which that link might be embedded. - FM
     */
    if (number <= 0 || number == text->last_anchor->number) {
	a = text->last_anchor;
    } else {
	for (a = text->first_anchor; a; a = a->next) {
	    if (a->number == number) {
		break;
	    }
	}
	if (a == NULL) {
	    /*
	     *  There's no anchor with that number,
	     *  so we'll default to the last anchor,
	     *  and cross our fingers. - FM
	     */
	    a = text->last_anchor;
	}
    }

    CTRACE((tfp, "GridText:HText_endAnchor: number:%d link_type:%d\n",
			a->number, a->link_type));
    if (a->link_type == INPUT_ANCHOR) {
	/*
	 *  Shouldn't happen, but put test here anyway to be safe. - LE
	 */

	CTRACE((tfp,
	   "HText_endAnchor: internal error: last anchor was input field!\n"));
	return;
    }
    if (a->number) {
	/*
	 *  If it goes somewhere...
	 */
	int i, j, k, l;
	BOOL remove_numbers_on_empty = (BOOL) (
	    ((keypad_mode == LINKS_ARE_NUMBERED ||
	      keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) &&
	     (text->hiddenlinkflag != HIDDENLINKS_MERGE ||
	      (LYNoISMAPifUSEMAP &&
	       !(text->node_anchor && text->node_anchor->bookmark) &&
	       HTAnchor_isISMAPScript(
		   HTAnchor_followMainLink((HTAnchor *)a->anchor))))));
	HTLine *last = text->last_line;
	HTLine *prev = text->last_line->prev;
	HTLine *start = last;
	int CurBlankExtent = 0;
	int BlankExtent = 0;

	/*
	 *  Check if the anchor content has only
	 *  white and special characters, starting
	 *  with the content on the last line. - FM
	 */
	a->extent += (text->chars + last->size) - a->start -
		     (text->Lines - a->line_num);
	if (a->extent > (int)last->size) {
	    /*
	     *  The anchor extends over more than one line,
	     *  so set up to check the entire last line. - FM
	     */
	    i = last->size;
	} else {
	    /*
	     *  The anchor is restricted to the last line,
	     *  so check from the start of the anchor. - FM
	     */
	    i = a->extent;
	}
	k = j = (last->size - i);
	while (j < (int)last->size) {
	    if (!IsSpecialAttrChar(last->data[j]) &&
		!isspace(UCH(last->data[j])) &&
		last->data[j] != HT_NON_BREAK_SPACE &&
		last->data[j] != HT_EN_SPACE)
		break;
	    i--;
	    j++;
	}
	if (i == 0) {
	    if (a->extent > (int)last->size) {
		/*
		 *  The anchor starts on a preceding line, and
		 *  the last line has only white and special
		 *  characters, so declare the entire extent
		 *  of the last line as blank. - FM
		 */
		CurBlankExtent = BlankExtent = last->size;
	    } else {
		/*
		 *  The anchor starts on the last line, and
		 *  has only white or special characters, so
		 *  declare the anchor's extent as blank. - FM
		 */
		CurBlankExtent = BlankExtent = a->extent;
	    }
	}
	/*
	 *  While the anchor starts on a line preceding
	 *  the one we just checked, and the one we just
	 *  checked has only white and special characters,
	 *  check whether the anchor's content on the
	 *  immediately preceding line also has only
	 *  white and special characters. - FM
	 */
	while (i == 0 &&
	       (a->extent > CurBlankExtent ||
		(a->extent == CurBlankExtent &&
		 k == 0 &&
		 prev != text->last_line &&
		 (prev->size == 0 ||
		  prev->data[prev->size - 1] == ']')))) {
	    start = prev;
	    k = j = prev->size - a->extent + CurBlankExtent;
	    if (j < 0) {
		/*
		 *  The anchor starts on a preceding line,
		 *  so check all of this line. - FM
		 */
		j = 0;
		i = prev->size;
	    } else {
		/*
		 *  The anchor starts on this line. - FM
		 */
		i = a->extent - CurBlankExtent;
	    }
	    while (j < (int)prev->size) {
		if (!IsSpecialAttrChar(prev->data[j]) &&
		    !isspace(UCH(prev->data[j])) &&
		    prev->data[j] != HT_NON_BREAK_SPACE &&
		    prev->data[j] != HT_EN_SPACE)
		    break;
		i--;
		j++;
	    }
	    if (i == 0) {
		if (a->extent > (CurBlankExtent + (int)prev->size) ||
		    (a->extent == CurBlankExtent + (int)prev->size &&
		     k == 0 &&
		     prev->prev != text->last_line &&
		     (prev->prev->size == 0 ||
		      prev->prev->data[prev->prev->size - 1] == ']'))) {
		    /*
		     *  This line has only white and special
		     *  characters, so treat its entire extent
		     *  as blank, and decrement the pointer for
		     *  the line to be analyzed. - FM
		     */
		    CurBlankExtent += prev->size;
		    BlankExtent = CurBlankExtent;
		    prev = prev->prev;
		} else {
		    /*
		     *  The anchor starts on this line, and it
		     *  has only white or special characters, so
		     *  declare the anchor's extent as blank. - FM
		     */
		    BlankExtent = a->extent;
		    break;
		}
	    }
	}
	if (i == 0) {
	    /*
	     *  It's an invisible anchor probably from an ALT=""
	     *  or an ignored ISMAP attribute due to a companion
	     *  USEMAP. - FM
	     */
	    a->show_anchor = NO;

	    CTRACE((tfp,
		   "HText_endAnchor: hidden (line,start,pos,ext,BlankExtent):(%d,%d,%d,%d,%d)",
		   a->line_num,a->start,a->line_pos,a->extent,
		   BlankExtent));

	    /*
	     *  If links are numbered, then try to get rid of the
	     *  numbered bracket and adjust the anchor count. - FM
	     *
	     * Well, let's do this only if -hiddenlinks=merged is not in
	     * effect, or if we can be reasonably sure that
	     * this is the result of an intentional non-generation of
	     * anchor text via NO_ISMAP_IF_USEMAP.  In other cases it can
	     * actually be a feature that numbered links alert the viewer
	     * to the presence of a link which is otherwise not selectable -
	     * possibly caused by HTML errors. - kw
	     */
	    if (remove_numbers_on_empty) {
		int NumSize = 0;
		TextAnchor *anc;

		/*
		 *  Set start->data[j] to the close-square-bracket,
		 *  or to the beginning of the line on which the
		 *  anchor start. - FM
		 */
		if (start == last) {
		    /*
		     *  The anchor starts on the last line. - FM
		     */
		    j = (last->size - a->extent - 1);
		} else {
		    /*
		     *  The anchor starts on a previous line. - FM
		     */
		    prev = start->prev;
		    j = (start->size - a->extent + CurBlankExtent - 1);
		}
		if (j < 0)
		    j = 0;
		i = j;

		/*
		 *  If start->data[j] is a close-square-bracket, verify
		 *  that it's the end of the numbered bracket, and if so,
		 *  strip the numbered bracket.  If start->data[j] is not
		 *  a close-square-bracket, check whether we had a wrap
		 *  and the close-square-bracket is at the end of the
		 *  previous line.  If so, strip the numbered bracket
		 *  from that line. - FM
		 */
		if (start->data[j] == ']') {
		    j--;
		    NumSize++;
		    while (j >= 0 && isdigit(UCH(start->data[j]))) {
			j--;
			NumSize++;
		    }
		    while (j < 0) {
			j++;
			NumSize--;
		    }
		    if (start->data[j] == '[') {
			/*
			 *  The numbered bracket is entirely
			 *  on this line. - FM
			 */
			NumSize++;
			if (start==last && (int)text->permissible_split > j) {
			    if ((int)text->permissible_split - NumSize < j)
				text->permissible_split = j;
			    else
				text->permissible_split -= NumSize;
			}
			k = j + NumSize;
			while (k < (int)start->size)
			    start->data[j++] = start->data[k++];
			if (start != last)
			    text->chars -= NumSize;
			for (anc = a; anc; anc = anc->next) {
			    anc->start -= NumSize;
			    if (anc->line_num == a->line_num &&
				anc->line_pos >= NumSize) {
			    anc->line_pos -= NumSize;
			    }
			}
			start->size = j;
			start->data[j++] = '\0';
			while (j < k)
			     start->data[j++] = '\0';
		    } else if (prev && prev->size > 1) {
			k = (i + 1);
			j = (prev->size - 1);
			while ((j >= 0) && IsSpecialAttrChar(prev->data[j]))
			    j--;
			i = (j + 1);
			while (j >= 0 &&
			       isdigit(UCH(prev->data[j]))) {
			    j--;
			    NumSize++;
			}
			while (j < 0) {
			    j++;
			    NumSize--;
			}
			if (prev->data[j] == '[') {
			    /*
			     *  The numbered bracket started on the
			     *  previous line, and part of it was
			     *  wrapped to this line. - FM
			     */
			    NumSize++;
			    l = (i - j);
			    while (i < (int)prev->size)
				prev->data[j++] = prev->data[i++];
			    text->chars -= l;
			    for (anc = a; anc; anc = anc->next) {
				anc->start -= l;
			    }
			    prev->size = j;
			    prev->data[j] = '\0';
			    while (j < i)
				prev->data[j++] = '\0';
			    if (start == last && text->permissible_split > 0) {
				if ((int)text->permissible_split < k)
				    text->permissible_split = 0;
				else
				    text->permissible_split -= k;
			    }
			    j = 0;
			    i = k;
			    while (k < (int)start->size)
				start->data[j++] = start->data[k++];
			    if (start != last)
				text->chars -= i;
			    for (anc = a; anc; anc = anc->next) {
				anc->start -= i;
				if (anc->line_num == a->line_num &&
				    anc->line_pos >= i) {
				    anc->line_pos -= i;
				}
			    }
			    start->size = j;
			    start->data[j++] = '\0';
			    while (j < k)
				start->data[j++] = '\0';
			} else {
			    /*
			     *  Shucks!  We didn't find the
			     *  numbered bracket. - FM
			     */
			    a->show_anchor = YES;
			}
		    } else {
			/*
			 *  Shucks!  We didn't find the
			 *  numbered bracket. - FM
			 */
			a->show_anchor = YES;
		    }
		} else if (prev && prev->size > 2) {
		    j = (prev->size - 1);
		    while ((j >= 0) && IsSpecialAttrChar(prev->data[j]))
			j--;
		    if (j < 0)
			j = 0;
		    i = (j + 1);
		    if ((j >= 2) &&
			(prev->data[j] == ']' &&
			 isdigit(UCH(prev->data[j - 1])))) {
			j--;
			NumSize++;
			k = (j + 1);
			while (j >= 0 &&
			       isdigit(UCH(prev->data[j]))) {
			    j--;
			    NumSize++;
			}
			while (j < 0) {
			    j++;
			    NumSize--;
			}
			if (prev->data[j] == '[') {
			    /*
			     *  The numbered bracket is all on the
			     *  previous line, and the anchor content
			     *  was wrapped to the last line. - FM
			     */
			    NumSize++;
			    k = j + NumSize;
			    while (k < (int)prev->size)
				prev->data[j++] = prev->data[k++];
			    text->chars -= NumSize;
			    for (anc = a; anc; anc = anc->next) {
				anc->start -= NumSize;
			    }
			    prev->size = j;
			    prev->data[j++] = '\0';
			    while (j < k)
				prev->data[j++] = '\0';
			} else {
			    /*
			     *  Shucks!  We didn't find the
			     *  numbered bracket. - FM
			     */
			    a->show_anchor = YES;
			}
		    } else {
			/*
			 *  Shucks!  We didn't find the
			 *  numbered bracket. - FM
			 */
			a->show_anchor = YES;
		    }
		} else {
		    /*
		     *  Shucks!  We didn't find the
		     *  numbered bracket. - FM
		     */
		    a->show_anchor = YES;
		}
	    }
	} else {
	    /*
	     *  The anchor's content is not restricted to only
	     *  white and special characters, so we'll show it
	     *  as a link. - FM
	     */
	    a->show_anchor = YES;
	    if (BlankExtent) {
		CTRACE((tfp,
		   "HText_endAnchor: blanks (line,start,pos,ext,BlankExtent):(%d,%d,%d,%d,%d)",
		   a->line_num,a->start,a->line_pos,a->extent,
		   BlankExtent));
	    }
	}
	if (a->show_anchor == NO) {
	    /*
	     *  The anchor's content is restricted to white
	     *  and special characters, so set it's number
	     *  and extent to zero, decrement the visible
	     *  anchor number counter, and add this anchor
	     *  to the hidden links list. - FM
	     */
	    a->extent = 0;
	    if (text->hiddenlinkflag != HIDDENLINKS_MERGE) {
		a->number = 0;
		text->last_anchor_number--;
		HText_AddHiddenLink(text, a);
	    }
	} else {
	    /*
	     *  The anchor's content is not restricted to white
	     *  and special characters, so we'll display the
	     *  content, but shorten it's extent by any trailing
	     *  blank lines we've detected. - FM
	     */
	    a->extent -= ((BlankExtent < a->extent) ?
					BlankExtent : 0);
	}
	if (BlankExtent || a->extent <= 0 || a->number <= 0) {
	    CTRACE((tfp,
		   "->[%d](%d,%d,%d,%d,%d)\n",
		   a->number,
		   a->line_num,a->start,a->line_pos,a->extent,
		   BlankExtent));
	}
    } else {
	/*
	 *  It's a named anchor without an HREF, so it
	 *  should be registered but not shown as a
	 *  link. - FM
	 */
	a->show_anchor = NO;
	a->extent = 0;
    }
}


PUBLIC void HText_appendText ARGS2(
	HText *,	text,
	CONST char *,	str)
{
    CONST char *p;

    if (str == NULL)
	return;

    if (text->halted == 3)
	return;

    for (p = str; *p; p++) {
	HText_appendCharacter(text, *p);
    }
}


PRIVATE int remove_special_attr_chars ARGS1(
	char *,		buf)
{
    register char *cp;
    register int soft_newline_count = 0;

    for (cp = buf; *cp != '\0' ; cp++) {
	/*
	 *  Don't print underline chars.
	 */
	soft_newline_count += (*cp == LY_SOFT_NEWLINE);
	if (!IsSpecialAttrChar(*cp)) {
	   *buf = *cp,
	   buf++;
	}
    }
    *buf = '\0';
    return soft_newline_count;
}


/*
**  This function trims blank lines from the end of the document, and
**  then gets the hightext from the text by finding the char position,
**  and brings the anchors in line with the text by adding the text
**  offset to each of the anchors.
*/
PUBLIC void HText_endAppend ARGS1(
	HText *,	text)
{
    HTLine *line_ptr;

    if (!text)
	return;

    CTRACE((tfp,"Gridtext: Entering HText_endAppend\n"));

    /*
     *  Create a  blank line at the bottom.
     */
    new_line(text);

    if (text->halted) {
	if (text->stbl)
	    HText_cancelStbl(text);
	/*
	 *  If output was stopped because memory was low, and we made
	 *  it to the end of the document, reset those flags and hope
	 *  things are better now. - kw
	 */
	LYFakeZap(NO);
	text->halted = 0;
    } else if (text->stbl) {
	/*
	 *  Could happen if TABLE end tag was missing.
	 *  Alternatively we could cancel in this case. - kw
	 */
	HText_endStblTABLE(text);
    }

    /*
     *  Get the first line.
     */
    line_ptr = text->last_line->next;

    /*
     *  Remove the blank lines at the end of document.
     */
    while (text->last_line->data[0] == '\0' && text->Lines > 2) {
	HTLine *next_to_the_last_line = text->last_line->prev;

	CTRACE((tfp, "GridText: Removing bottom blank line: `%s'\n",
			    text->last_line->data));
	/*
	 *  line_ptr points to the first line.
	 */
	next_to_the_last_line->next = line_ptr;
	line_ptr->prev = next_to_the_last_line;
	FREE(text->last_line);
	text->last_line = next_to_the_last_line;
	text->Lines--;
	CTRACE((tfp, "GridText: New bottom line: `%s'\n",
			    text->last_line->data));
    }

    /*
     *  Fix up the anchor structure values and
     *  create the hightext strings. - FM
     */
    HText_trimHightext(text, TRUE, -1);
}


/*
**  This function gets the hightext from the text by finding the char
**  position, and brings the anchors in line with the text by adding the text
**  offset to each of the anchors.
**
**  `Forms input' fields cannot be displayed properly without this function
**  to be invoked (detected in display_partial mode).
**
**  If final is set, this is the final fixup; if not set, we don't have
**  to do everything because there should be another call later.
**
**  BEFORE this function has treated a TextAnchor, its line_pos and
**  extent fields are counting bytes in the HTLine data, including
**  invisible special attribute chars and counting UTF-8 multibyte
**  characters as multiple bytes.
**  AFTER the adjustment, the anchor line_pos (and hightext2offset
**  if applicable) fields indicate x positions in terms of displayed
**  character cells, and the extent field apparently is unimportant;
**  the anchor text has been copied to the hightext (and possibly
**  hightext2) fields (which should have been NULL up to that point),
**  with special attribute chars removed.
**  This needs to be done so that display_page finds the anchors in the
**  form it expects when it sets the links[] elements.
*/
PUBLIC void HText_trimHightext ARGS3(
	HText *,	text,
	BOOLEAN,	final,
	int,		stop_before)
{
    int cur_line, cur_char, cur_shift;
    TextAnchor *anchor_ptr;
    TextAnchor *prev_a = NULL;
    HTLine *line_ptr;
    unsigned char ch;

    if (!text)
	return;

    if (final) {
	CTRACE((tfp, "Gridtext: Entering HText_trimHightext (final)\n"));
    } else {
	if (stop_before < 0 || stop_before > text->Lines)
	    stop_before = text->Lines;
	CTRACE((tfp,
	       "Gridtext: Entering HText_trimHightext (partial: 0..%d/%d)\n",
	       stop_before, text->Lines));
    }

    /*
     *  Get the first line.
     */
    line_ptr = text->last_line->next;
    cur_char = line_ptr->size;
    cur_line = 0;

    /*
     *  Fix up the anchor structure values and
     *  create the hightext strings. - FM
     */
    for (anchor_ptr = text->first_anchor;
	anchor_ptr;
	prev_a = anchor_ptr, anchor_ptr=anchor_ptr->next) {
	int have_soft_newline_in_1st_line=0;
re_parse:
	/*
	 *  Find the right line.
	 */
	for (; anchor_ptr->start > cur_char;
	       line_ptr = line_ptr->next,
	       cur_char += line_ptr->size+1,
	       cur_line++) {
	    ; /* null body */
	}

	if (!final) {
	    /*
	     *  If this is not the final call, stop when we have reached
	     *  the last line, or the very end of preceding line.
	     *  The last line is probably still not finished. - kw
	     */
	    if (cur_line >= stop_before)
		break;
	    if (anchor_ptr->start >= text->chars - 1)
		break;
	    /*
	     *  Also skip this anchor if it looks like HText_endAnchor
	     *  is not yet done with it. - kw
	     */
	    if (!anchor_ptr->extent && anchor_ptr->number &&
		(anchor_ptr->link_type & HYPERTEXT_ANCHOR) &&
		!anchor_ptr->show_anchor &&
		anchor_ptr->number == text->last_anchor_number)
		continue;
	}

	/*
	 *  If hightext has already been set, then we must have already
	 *  done the trimming & adjusting for this anchor, so avoid
	 *  doing it a second time. - kw
	 */
	if (anchor_ptr->hightext)
	    continue;

	if (anchor_ptr->start == cur_char) {
	    anchor_ptr->line_pos = line_ptr->size;
	} else {
	    anchor_ptr->line_pos = anchor_ptr->start -
				   (cur_char - line_ptr->size);
	}
	if (anchor_ptr->line_pos < 0) {
	    anchor_ptr->start -= anchor_ptr->line_pos;
	    anchor_ptr->line_pos = 0;
	    anchor_ptr->line_num = cur_line;
	}
	CTRACE((tfp,
	       "Gridtext: Anchor found on line:%d col:%d [%d] ext:%d\n",
	       cur_line, anchor_ptr->line_pos,
	       anchor_ptr->number, anchor_ptr->extent));

	cur_shift = 0;
	/*
	 *  Strip off any spaces or SpecialAttrChars at the beginning,
	 *  if they exist, but only on HYPERTEXT_ANCHORS.
	 */
	if (anchor_ptr->link_type & HYPERTEXT_ANCHOR) {
	    ch = UCH(line_ptr->data[anchor_ptr->line_pos]);
	    while (isspace(ch) ||
		   IsSpecialAttrChar(ch)) {
		anchor_ptr->line_pos++;
		anchor_ptr->extent--;
		cur_shift++;
		ch = UCH(line_ptr->data[anchor_ptr->line_pos]);
	    }
	}
	if (anchor_ptr->extent < 0) {
	    anchor_ptr->extent = 0;
	}
	anchor_ptr->start += cur_shift;

	CTRACE((tfp, "anchor text: '%s'\n", line_ptr->data));
	/*
	 *  If the link begins with an end of line and we have more
	 *  lines, then start the highlighting on the next line. - FM
	 *  But if an empty anchor is at the end of line and empty,
	 *  keep it where it is, unless the previous anchor in the list
	 *  (if any) already starts later. - kw
	 */
	if ((unsigned)anchor_ptr->line_pos >= strlen(line_ptr->data)) {
	    if (cur_line < text->Lines &&
		(anchor_ptr->extent ||
		 anchor_ptr->line_pos != (int)line_ptr->size ||
		 (prev_a && prev_a->start > anchor_ptr->start))) {
		anchor_ptr->start++;
		CTRACE((tfp, "found anchor at end of line\n"));
		goto re_parse;
	    } else {
		CTRACE((tfp, "found anchor at end of line, leaving it there\n"));
	    }
	}

	/*
	 *  Copy the link name into the data structure.
	 */
	if (line_ptr->data &&
	    anchor_ptr->extent > 0 && anchor_ptr->line_pos >= 0) {
	    StrnAllocCopy(anchor_ptr->hightext,
			  &line_ptr->data[anchor_ptr->line_pos],
			  anchor_ptr->extent);
	} else {
	    StrAllocCopy(anchor_ptr->hightext, "");
	}

	/*
	 *  If true the anchor extends over two lines,
	 *  copy that into the data structure.
	 */
	if ((unsigned)anchor_ptr->extent > strlen(anchor_ptr->hightext)) {
	    HTLine *line_ptr2 = line_ptr->next;

	    if (!final) {
		if (cur_line + 1 >= stop_before) {
		    FREE(anchor_ptr->hightext); /* bail out */
		    break;
		}
	    }
	    /*
	     *  Double check that we have a line pointer,
	     *  and if so, copy into hightext2.
	     */
	    if (line_ptr2) {
		StrnAllocCopy(anchor_ptr->hightext2,
			      line_ptr2->data,
			      (anchor_ptr->extent -
			       strlen(anchor_ptr->hightext)));
		anchor_ptr->hightext2offset = line_ptr2->offset;
		/*handle LY_SOFT_NEWLINEs -VH */
		anchor_ptr->hightext2offset +=
			remove_special_attr_chars(anchor_ptr->hightext2);

		if (anchor_ptr->link_type & HYPERTEXT_ANCHOR) {
		    LYTrimTrailing(anchor_ptr->hightext2);
		    if (anchor_ptr->hightext2[0] == '\0') {
			FREE(anchor_ptr->hightext2);
			anchor_ptr->hightext2offset = 0;
		    }
		}
	    }
	}
	remove_special_attr_chars(anchor_ptr->hightext);
	if (anchor_ptr->link_type & HYPERTEXT_ANCHOR) {
	    LYTrimTrailing(anchor_ptr->hightext);
	}

	/*
	 *  Subtract any formatting characters from the x position
	 *  of the link.
	 */
	if (anchor_ptr->line_pos > 0) {
	    register int offset = 0, i = 0;
	    for (; i < anchor_ptr->line_pos; i++) {
		if (IS_UTF_EXTRA(line_ptr->data[i]) ||
		    IsSpecialAttrChar(line_ptr->data[i])) {
		    offset++;
		    have_soft_newline_in_1st_line += (line_ptr->data[i] == LY_SOFT_NEWLINE);
		}
	    }
	    anchor_ptr->line_pos -= offset;
	}

	/*
	 *  Add the offset, and set the line number.
	 */
	anchor_ptr->line_pos += line_ptr->offset;
	anchor_ptr->line_num  = cur_line;

	/*handle LY_SOFT_NEWLINEs -VH */
	anchor_ptr->line_pos += have_soft_newline_in_1st_line;

	CTRACE((tfp, "GridText:     add link on line %d col %d [%d] %s\n",
	       cur_line, anchor_ptr->line_pos,
	       anchor_ptr->number, "in HText_trimHightext"));

	/*
	 *  If this is the last anchor, we're done!
	 */
	if (anchor_ptr == text->last_anchor)
	    break;
    }
}


/*	Dump diagnostics to tfp
*/
PUBLIC void HText_dump ARGS1(
	HText *,	text GCC_UNUSED)
{
    fprintf(tfp, "HText: Dump called\n");
}


/*	Return the anchor associated with this node
*/
PUBLIC HTParentAnchor * HText_nodeAnchor ARGS1(
	HText *,	text)
{
    return text->node_anchor;
}

/*				GridText specials
**				=================
*/
/*
 *  HTChildAnchor() returns the anchor with index N.
 *  The index corresponds to the [number] we print for the anchor.
 */
PUBLIC HTChildAnchor * HText_childNumber ARGS1(
	int,		number)
{
    TextAnchor * a;

    if (!(HTMainText && HTMainText->first_anchor) || number <= 0)
	return (HTChildAnchor *)0;	/* Fail */

    for (a = HTMainText->first_anchor; a; a = a->next) {
	if (a->number == number)
	    return(a->anchor);
    }
    return (HTChildAnchor *)0;	/* Fail */
}

/*
 *  HText_FormDescNumber() returns a description of the form field
 *  with index N.  The index corresponds to the [number] we print
 *  for the field. - FM & LE
 */
PUBLIC void HText_FormDescNumber ARGS2(
	int,		number,
	char **,	desc)
{
    TextAnchor * a;

    if (!desc)
	return;

    if (!(HTMainText && HTMainText->first_anchor) || number <= 0) {
	 *desc = gettext("unknown field or link");
	 return;
    }

    for (a = HTMainText->first_anchor; a; a = a->next) {
	if (a->number == number) {
	    if (!(a->input_field && a->input_field->type)) {
		*desc = gettext("unknown field or link");
		return;
	    }
	    break;
	}
    }

    switch (a->input_field->type) {
	case F_TEXT_TYPE:
	    *desc = gettext("text entry field");
	    return;
	case F_PASSWORD_TYPE:
	    *desc = gettext("password entry field");
	    return;
	case F_CHECKBOX_TYPE:
	    *desc = gettext("checkbox");
	    return;
	case F_RADIO_TYPE:
	    *desc = gettext("radio button");
	    return;
	case F_SUBMIT_TYPE:
	    *desc = gettext("submit button");
	    return;
	case F_RESET_TYPE:
	    *desc = gettext("reset button");
	    return;
	case F_OPTION_LIST_TYPE:
	    *desc = gettext("popup menu");
	    return;
	case F_HIDDEN_TYPE:
	    *desc = gettext("hidden form field");
	    return;
	case F_TEXTAREA_TYPE:
	    *desc = gettext("text entry area");
	    return;
	case F_RANGE_TYPE:
	    *desc = gettext("range entry field");
	    return;
	case F_FILE_TYPE:
	    *desc = gettext("file entry field");
	    return;
	case F_TEXT_SUBMIT_TYPE:
	    *desc = gettext("text-submit field");
	    return;
	case F_IMAGE_SUBMIT_TYPE:
	    *desc = gettext("image-submit button");
	    return;
	case F_KEYGEN_TYPE:
	    *desc = gettext("keygen field");
	    return;
	default:
	    *desc = gettext("unknown form field");
	    return;
    }
}

/* HTGetRelLinkNum returns the anchor number to which follow_link_number()
 *    is to jump (input was 123+ or 123- or 123+g or 123-g or 123 or 123g)
 * num is the number specified
 * rel is 0 or '+' or '-'
 * cur is the current link
 */
PUBLIC int HTGetRelLinkNum ARGS3(
	int,	num,
	int,	rel,
	int,	cur)
{
    TextAnchor *a, *l = 0;
    int scrtop = HText_getTopOfScreen(); /*XXX +1? */
    int curline = links[cur].anchor_line_num;
    int curpos = links[cur].lx;
    int on_screen = ( curline >= scrtop && curline < (scrtop + display_lines) );
    /* curanchor may or may not be the "current link", depending whether it's
     * on the current screen
     */
    int curanchor = links[cur].anchor_number;

    CTRACE((tfp, "HTGetRelLinkNum(%d,%d,%d) -- HTMainText=%p\n",
	   num, rel, cur, HTMainText));
    CTRACE((tfp,"  scrtop=%d, curline=%d, curanchor=%d, display_lines=%d, %s\n",
	   scrtop, curline, curanchor, display_lines,
	   on_screen ? "on_screen" : "0"));
    if (!HTMainText) return 0;
    if ( rel==0 ) return num;

    /* if cur numbered link is on current page, use it */
    if ( on_screen && curanchor ) {
	CTRACE((tfp,"curanchor=%d at line %d on screen\n",curanchor,curline));
	if ( rel == '+' ) return curanchor + num;
	else if ( rel == '-' ) return curanchor - num;
	else return num; /* shouldn't happen */
    }

    /* no current link on screen, or current link is not numbered
     * -- find previous closest numbered link
     */
    for (a = HTMainText->first_anchor; a; a = a->next) {
	CTRACE((tfp,"  a->line_num=%d, a->number=%d\n",a->line_num,a->number));
	if ( a->line_num >= scrtop ) break;
	if ( a->number == 0 ) continue;
	l = a;
	curanchor = l->number;
    }
    CTRACE((tfp,"  a=%p, l=%p, curanchor=%d\n",a,l,curanchor));
    if ( on_screen ) { /* on screen but not a numbered link */
	for ( ;  a;  a = a->next ) {
	    if ( a->number ) { l = a; curanchor = l->number; }
	    if ( curline == a->line_num && curpos == a->line_pos ) break;
	}
    }
    if ( rel == '+' ) {
	return curanchor + num;
    } else if ( rel == '-' ) {
	if ( l )
	    return curanchor + 1 - num;
	else {
	    for ( ;  a && a->number==0;  a = a->next ) ;
	    return a ? a->number - num : 0;
	}
    }
    else return num; /* shouldn't happen */
}

/*
 *  HTGetLinkInfo returns some link info based on the number.
 *
 *  If want_go is not 0, caller requests to know a line number for
 *  the link indicated by number.  It will be returned in *go_line, and
 *  *linknum will be set to an index into the links[] array, to use after
 *  the line in *go_line has been made the new top screen line.
 *  *hightext and *lname are unchanged. - KW
 *
 *  If want_go is 0 and the number doesn't represent an input field, info
 *  on the link indicated by number is deposited in *hightext and *lname.
 */
PUBLIC int HTGetLinkInfo ARGS6(
	int,		number,
	int,		want_go,
	int *,		go_line,
	int *,		linknum,
	char **,	hightext,
	char **,	lname)
{
    TextAnchor *a;
    HTAnchor *link_dest;
#ifndef DONT_TRACK_INTERNAL_LINKS
    HTAnchor *link_dest_intl = NULL;
#endif
    int anchors_this_line = 0, anchors_this_screen = 0;
    int prev_anchor_line = -1, prev_prev_anchor_line = -1;

    if (!HTMainText)
	return(NO);

    for (a = HTMainText->first_anchor; a; a = a->next) {
	/*
	 *  Count anchors, first on current line if there is more
	 *  than one.  We have to count all links, including form
	 *  field anchors and others with a->number == 0, because
	 *  they are or will be included in the links[] array.
	 *  The exceptions are hidden form fields and anchors with
	 *  show_anchor not set, because they won't appear in links[]
	 *  and don't count towards nlinks. - KW
	 */
	if ((a->show_anchor) &&
	    (a->link_type != INPUT_ANCHOR ||
	     a->input_field->type != F_HIDDEN_TYPE)) {
	    if (a->line_num == prev_anchor_line) {
		anchors_this_line++;
	    } else {
		/*
		 *  This anchor is on a different line than the previous one.
		 *  Remember which was the line number of the previous anchor,
		 *  for use in screen positioning later. - KW
		 */
		anchors_this_line = 1;
		prev_prev_anchor_line = prev_anchor_line;
		prev_anchor_line = a->line_num;
	    }
	    if (a->line_num >= HTMainText->top_of_screen) {
		/*
		 *  Count all anchors starting with the top line of the
		 *  currently displayed screen.  Just keep on counting
		 *  beyond this screen's bottom line - we'll know whether
		 *  a found anchor is below the current screen by a check
		 *  against nlinks later. - KW
		 */
		anchors_this_screen++;
	    }
	}

	if (a->number == number) {
	    /*
	     *  We found it.  Now process it, depending
	     *  on what kind of info is requested. - KW
	     */
	    if (want_go || a->link_type == INPUT_ANCHOR) {
		if (a->show_anchor == NO) {
		    /*
		     *  The number requested has been assigned to an anchor
		     *  without any selectable text, so we cannot position
		     *  on it.  The code for suppressing such anchors in
		     *  HText_endAnchor() may not have applied, or it may
		     *  have failed.  Return a failure indication so that
		     *  the user will notice that something is wrong,
		     *  instead of positioning on some other anchor which
		     *  might result in inadvertent activation. - KW
		     */
		    return(NO);
		}
		if (anchors_this_screen > 0 &&
		    anchors_this_screen <= nlinks &&
		    a->line_num >= HTMainText->top_of_screen &&
		    a->line_num < HTMainText->top_of_screen+(display_lines)) {
		    /*
		     *  If the requested anchor is within the current screen,
		     *  just set *go_line so that the screen window won't move
		     *  (keep it as it is), and set *linknum to the index of
		     *  this link in the current links[] array. - KW
		     */
		    *go_line = HTMainText->top_of_screen;
		    if (linknum)
			*linknum = anchors_this_screen - 1;
		} else {
		    /*
		     *  if the requested anchor is not within the currently
		     *  displayed screen, set *go_line such that the top line
		     *  will be either
		     *  (1) the line immediately below the previous
		     *      anchor, or
		     *  (2) about one third of a screenful above the line
		     *      with the target, or
		     *  (3) the first line of the document -
		     *  whichever comes last.  In all cases the line with our
		     *  target will end up being the first line with any links
		     *  on the new screen, so that we can use the
		     *  anchors_this_line counter to point to the anchor in
		     *  the new links[] array.  - kw
		     */
		    int max_offset = SEARCH_GOAL_LINE - 1;
		    if (max_offset < 0)
			max_offset = 0;
		    else if (max_offset >= display_lines)
			max_offset = display_lines - 1;
		    *go_line = prev_anchor_line - max_offset;
		    if (*go_line <= prev_prev_anchor_line)
			*go_line = prev_prev_anchor_line + 1;
		    if (*go_line < 0)
			*go_line = 0;
		    if (linknum)
			*linknum = anchors_this_line - 1;
		}
		return(LINK_LINE_FOUND);
	    } else {
		*hightext= a->hightext;
		link_dest = HTAnchor_followMainLink((HTAnchor *)a->anchor);
		{
		    char *cp_freeme = NULL;
		    if (traversal) {
			cp_freeme = stub_HTAnchor_address(link_dest);
		    } else {
#ifndef DONT_TRACK_INTERNAL_LINKS
			if (a->link_type == INTERNAL_LINK_ANCHOR) {
			    link_dest_intl = HTAnchor_followTypedLink(
				(HTAnchor *)a->anchor, LINK_INTERNAL);
			    if (link_dest_intl && link_dest_intl != link_dest) {

				CTRACE((tfp, "HTGetLinkInfo: unexpected typed link to %s!\n",
					    link_dest_intl->parent->address));
				link_dest_intl = NULL;
			    }
			}
			if (link_dest_intl) {
			    char *cp2 = HTAnchor_address(link_dest_intl);
			    FREE(*lname);
			    *lname = cp2;
			    return(WWW_INTERN_LINK_TYPE);
			} else
#endif
			    cp_freeme = HTAnchor_address(link_dest);
		    }
		    StrAllocCopy(*lname, cp_freeme);
		    FREE(cp_freeme);
		}
		return(WWW_LINK_TYPE);
	    }
	}
    }
    return(NO);
}

PRIVATE BOOLEAN same_anchor_or_field ARGS5(
    int,		numberA,
    FormInfo *,		formA,
    int,		numberB,
    FormInfo *,		formB,
    BOOLEAN,		ta_same)
{
    if (numberA > 0 || numberB > 0) {
	if (numberA == numberB)
	    return(YES);
	else if (!ta_same)
	    return(NO);
    }
    if (formA || formB) {
	if (formA == formB) {
	    return(YES);
	} else if (!ta_same) {
	    return(NO);
	} else if (!(formA && formB)) {
	    return(NO);
	}
    } else {
	return(NO);
    }
    if (formA->type != formB->type ||
	formA->type != F_TEXTAREA_TYPE || formB->type != F_TEXTAREA_TYPE) {
	return(NO);
    }
    if (formA->number != formB->number)
	return(NO);
    if (!formA->name || !formB->name)
	return(YES);
    return (BOOL) (strcmp(formA->name, formB->name) == 0);
}

#define same_anchor_as_link(i,a,ta_same) (i >= 0 && a &&\
		same_anchor_or_field(links[i].anchor_number,\
		(links[i].type == WWW_FORM_LINK_TYPE) ? links[i].form : NULL,\
		a->number,\
		(a->link_type == INPUT_ANCHOR) ? a->input_field : NULL,\
		ta_same))
#define same_anchors(a1,a2,ta_same) (a1 && a2 &&\
		same_anchor_or_field(a1->number,\
		(a1->link_type == INPUT_ANCHOR) ? a1->input_field : NULL,\
		a2->number,\
		(a2->link_type == INPUT_ANCHOR) ? a2->input_field : NULL,\
		ta_same))

/*
 *  Are there more textarea lines belonging to the same textarea before
 *  (direction < 0) or after (direction > 0) the current one?
 *  On entry, curlink must be the index in links[] of a textarea field. - kw
 */
PUBLIC BOOL HText_TAHasMoreLines ARGS2(
	int,		curlink,
	int,		direction)
{
    TextAnchor *a;
    TextAnchor *prev_a = NULL;

    if (!HTMainText)
	return(NO);
    if (direction < 0) {
	for (a = HTMainText->first_anchor; a; prev_a = a, a = a->next) {
	    if (a->link_type == INPUT_ANCHOR &&
		links[curlink].form == a->input_field) {
		return same_anchors(a, prev_a, TRUE);
	    }
	    if (links[curlink].anchor_number &&
		a->number >= links[curlink].anchor_number)
		break;
	}
	return NO;
    } else {
	for (a = HTMainText->first_anchor; a; a = a->next) {
	    if (a == HTMainText->last_anchor)
		break;
	    if (a->link_type == INPUT_ANCHOR &&
		links[curlink].form == a->input_field) {
		return same_anchors(a, a->next, TRUE);
	    }
	    if (links[curlink].anchor_number &&
		a->number >= links[curlink].anchor_number)
		break;
	}
	return NO;
    }
}

/*
 *  HTGetLinkOrFieldStart - moving to previous or next link or form field.
 *
 *  On input,
 *	curlink: current link, as index in links[] array (-1 if none)
 *	direction: whether to move up or down (or stay where we are)
 *	ta_skip: if FALSE, input fields belonging to the same textarea are
 *		 are treated as different fields, as usual;
 *		 if TRUE, fields of the same textarea are treated as a
 *		 group for skipping.
 *  The caller wants a information for positioning on the new link to be
 *  deposited in *go_line and (if linknum is not NULL) *linknum.
 *
 *  On failure (no more links in the requested direction) returns NO
 *  and doesn't change *go_line or *linknum.  Otherwise, LINK_DO_ARROWUP
 *  may be returned, and *go_line and *linknum not changed, to indicate that
 *  the caller should use a normal PREV_LINK or PREV_PAGE mechanism.
 *  Otherwise:
 *  The number (0-based counting) for the new top screen line will be returned
 *  in *go_line, and *linknum will be set to an index into the links[] array,
 *  to use after the line in *go_line has been made the new top screen
 *  line. - kw
 */
PUBLIC int HTGetLinkOrFieldStart ARGS5(
	int,		curlink,
	int *,		go_line,
	int *,		linknum,
	int,		direction,
	BOOLEAN,	ta_skip)
{
    TextAnchor *a;
    int anchors_this_line = 0;
    int prev_anchor_line = -1, prev_prev_anchor_line = -1;

    struct agroup {
	TextAnchor *anc;
	int prev_anchor_line;
	int anchors_this_line;
	int anchors_this_group;
    } previous, current;
    struct agroup *group_to_go = NULL;

    if (!HTMainText)
	return(NO);

    previous.anc = current.anc = NULL;
    previous.prev_anchor_line = current.prev_anchor_line = -1;
    previous.anchors_this_line = current.anchors_this_line = 0;
    previous.anchors_this_group = current.anchors_this_group = 0;

    for (a = HTMainText->first_anchor; a; a = a->next) {
	/*
	 *  Count anchors, first on current line if there is more
	 *  than one.  We have to count all links, including form
	 *  field anchors and others with a->number == 0, because
	 *  they are or will be included in the links[] array.
	 *  The exceptions are hidden form fields and anchors with
	 *  show_anchor not set, because they won't appear in links[]
	 *  and don't count towards nlinks. - KW
	 */
	if ((a->show_anchor) &&
	    (a->link_type != INPUT_ANCHOR ||
	     a->input_field->type != F_HIDDEN_TYPE)) {
	    if (a->line_num == prev_anchor_line) {
		anchors_this_line++;
	    } else {
		/*
		 *  This anchor is on a different line than the previous one.
		 *  Remember which was the line number of the previous anchor,
		 *  for use in screen positioning later. - KW
		 */
		anchors_this_line = 1;
		prev_prev_anchor_line = prev_anchor_line;
		prev_anchor_line = a->line_num;
	    }

	    if (!same_anchors(current.anc, a, ta_skip)) {
		previous.anc = current.anc;
		previous.prev_anchor_line = current.prev_anchor_line;
		previous.anchors_this_line = current.anchors_this_line;
		previous.anchors_this_group = current.anchors_this_group;
		current.anc = a;
		current.prev_anchor_line = prev_prev_anchor_line;
		current.anchors_this_line = anchors_this_line;
		current.anchors_this_group = 1;
	    } else {
		current.anchors_this_group++;
	    }
	    if (curlink >= 0) {
		if (same_anchor_as_link(curlink,a, ta_skip)) {
		    if (direction == -1) {
			group_to_go = &previous;
			break;
		    } else if (direction == 0) {
			group_to_go = &current;
			break;
		    }
		} else if (direction > 0 &&
			   same_anchor_as_link(curlink,previous.anc, ta_skip)) {
		    group_to_go = &current;
		    break;
		}
	    } else {
		if (a->line_num >= HTMainText->top_of_screen) {
		    if (direction < 0) {
			group_to_go = &previous;
			break;
		    } else if (direction == 0) {
			if (previous.anc) {
			    group_to_go = &previous;
			    break;
			} else {
			    group_to_go = &current;
			    break;
			}
		    } else {
			group_to_go = &current;
			break;
		    }
		}
	    }
	}
    }
    if (!group_to_go && curlink < 0 && direction <= 0) {
	group_to_go = &current;
    }
    if (group_to_go) {
	a = group_to_go->anc;
	if (a) {
	    int max_offset;
	    /*
	     *  We know where to go; most of the stuff below is just
	     *  tweaks to try to position the new screen in a specific
	     *  way.
	     *
	     *  In some cases going to a previous link can be done
	     *  via the normal LYK_PREV_LINK action, which may give
	     *  better positioning of the new screen. - kw
	     */
	    if (a->line_num < HTMainText->top_of_screen &&
		a->line_num >= HTMainText->top_of_screen-(display_lines)) {
		if ((curlink < 0 &&
		     group_to_go->anchors_this_group == 1) ||
		    (direction < 0 &&
		     group_to_go != &current &&
		     current.anc &&
		     current.anc->line_num >= HTMainText->top_of_screen &&
		     group_to_go->anchors_this_group == 1) ||
		    (a->next &&
		     a->next->line_num >= HTMainText->top_of_screen)) {
		    return(LINK_DO_ARROWUP);
		}
	    }
	    /*
	     *  The fundamental limitation of the current anchors_this_line
	     *  counter method is that we only can set *linknum to the right
	     *  index into the future links[] array if the line with our link
	     *  ends up being the first line with any links (that count) on
	     *  the new screen.  Subject to that restriction we still have
	     *  some vertical liberty (sometimes), and try to make the best
	     *  of it.  It may be a question of taste though. - kw
	     */
	    if (a->line_num <= (display_lines)) {
		max_offset = 0;
	    } else if (a->line_num < HTMainText->top_of_screen) {
		int screensback =
		    (HTMainText->top_of_screen - a->line_num + (display_lines) - 1)
		    / (display_lines);
		max_offset = a->line_num - (HTMainText->top_of_screen -
					    screensback * (display_lines));
	    } else if (HTMainText->Lines - a->line_num <= (display_lines)) {
		max_offset = a->line_num - (HTMainText->Lines + 1
					    - (display_lines));
	    } else if (a->line_num >=
		       HTMainText->top_of_screen+(display_lines)) {
		int screensahead =
		    (a->line_num - HTMainText->top_of_screen) / (display_lines);
		max_offset = a->line_num - HTMainText->top_of_screen -
		    screensahead * (display_lines);
	    } else {
		max_offset = SEARCH_GOAL_LINE - 1;
	    }

	    /* Stuff below should remain unchanged if line positioning
	       is tweaked. - kw */
	    if (max_offset < 0)
		max_offset = 0;
	    else if (max_offset >= display_lines)
		max_offset = display_lines - 1;
	    *go_line = a->line_num - max_offset;
	    if (*go_line <= group_to_go->prev_anchor_line)
		*go_line = group_to_go->prev_anchor_line + 1;

#if 0
	    if (*go_line > HTMainText->top_of_screen &&
		*go_line < HTMainText->top_of_screen+(display_lines) &&
		HTMainText->top_of_screen+(display_lines) <= a->line_num &&
		HTMainText->top_of_screen+2*(display_lines) <= HTMainText->Lines)
		*go_line = HTMainText->top_of_screen+(display_lines);
#endif
	    if (*go_line < 0)
		*go_line = 0;
	    if (linknum)
		*linknum = group_to_go->anchors_this_line - 1;
	    return(LINK_LINE_FOUND);
	}
    }
    return(NO);
}

/*
 *  This function finds the line indicated by line_num in the
 *  HText structure indicated by text, and searches that line
 *  for the first hit with the string indicated by target.  If
 *  there is no hit, FALSE is returned.  If there is a hit, then
 *  a copy of the line starting at that first hit is loaded into
 *  *data with all IsSpecial characters stripped, it's offset and
 *  the printable target length (without IsSpecial, or extra CJK
 *  or utf8 characters) are loaded into *offset and *tLen, and
 *  TRUE is returned. - FM
 */
PUBLIC BOOL HText_getFirstTargetInLine ARGS7(
	HText *,	text,
	int,		line_num,
	BOOL,		utf_flag,
	int *,		offset,
	int *,		tLen,
	char **,	data,
	CONST char *,	target)
{
    HTLine *line;
    char *LineData;
    int LineOffset, HitOffset, LenNeeded, i;
    char *cp;

    /*
     *  Make sure we have an HText structure, that line_num is
     *  in its range, and that we have a target string. - FM
     */
    if (!(text && line_num >= 0 && line_num <= text->Lines &&
	  target && *target))
	return(FALSE);

    /*
     *  Find the line and set up its data and offset - FM
     */
    for (i = 0, line = text->last_line->next;
	 i < line_num && (line != text->last_line);
	 i++, line = line->next) {
	if (line->next == NULL) {
	    return(FALSE);
	}
    }
    if (!line && line->data[0])
	return(FALSE);
    LineData = (char *)line->data;
    LineOffset = (int)line->offset;

    /*
     *  If the target is on the line, load the offset of
     *  its first character and the subsequent line data,
     *  strip any special characters from the loaded line
     *  data, and return TRUE. - FM
     */
    if ((case_sensitive ?
	 (cp = LYno_attr_mbcs_strstr(LineData,
				     target,
				     utf_flag, YES,
				     &HitOffset,
				     &LenNeeded)) != NULL :
	 (cp = LYno_attr_mbcs_case_strstr(LineData,
				     target,
				     utf_flag, YES,
				     &HitOffset,
				     &LenNeeded)) != NULL) &&
	(LineOffset + LenNeeded) < LYcols) {
	/*
	 *  We had a hit so load the results,
	 *  remove IsSpecial characters from
	 *  the allocated data string, and
	 *  return TRUE. - FM
	 */
	*offset = (LineOffset + HitOffset);
	*tLen = (LenNeeded - HitOffset);
	 StrAllocCopy(*data, cp);
	 remove_special_attr_chars(*data);
	 return(TRUE);
    }

    /*
     *  The line does not contain the target. - FM
     */
    return(FALSE);
}

/*
 *  HText_getNumOfLines returns the number of lines in the
 *  current document.
 */
PUBLIC int HText_getNumOfLines NOARGS
{
    return(HTMainText ? HTMainText->Lines : 0);
}

/*
 *  HText_getTitle returns the title of the
 *  current document.
 */
PUBLIC CONST char * HText_getTitle NOARGS
{
    return(HTMainText ?
	  HTAnchor_title(HTMainText->node_anchor) : 0);
}

#ifdef USE_HASH
PUBLIC CONST char *HText_getStyle NOARGS
{
   return(HTMainText ?
	  HTAnchor_style(HTMainText->node_anchor) : 0);
}
#endif

/*
 *  HText_getSugFname returns the suggested filename of the current
 *  document (normally derived from a Content-Disposition header with
 *  attachment; filename=name.suffix). - FM
 */
PUBLIC CONST char * HText_getSugFname NOARGS
{
    return(HTMainText ?
	  HTAnchor_SugFname(HTMainText->node_anchor) : 0);
}

/*
 *  HTCheckFnameForCompression receives the address of an allocated
 *  string containing a filename, and an anchor pointer, and expands
 *  or truncates the string's suffix if appropriate, based on whether
 *  the anchor indicates that the file is compressed.  We assume
 *  that the file was not uncompressed (as when downloading), and
 *  believe the headers about whether it's compressed or not. - FM
 *
 *  Added third arg - if strip_ok is FALSE, we don't trust the anchor
 *  info enough to remove a compression suffix if the anchor object
 *  does not indicate compression. - kw
 */
PUBLIC void HTCheckFnameForCompression ARGS3(
	char **,		fname,
	HTParentAnchor *,	anchor,
	BOOL,			strip_ok)
{
    char *fn = *fname;
    char *dot = NULL, *cp = NULL;
    char *suffix;
    CONST char *ct = NULL;
    CONST char *ce = NULL;
    CompressFileType method = cftNone;
    CompressFileType second;

    /*
     *  Make sure we have a string and anchor. - FM
     */
    if (!(fn && anchor))
	return;

    /*
     *  Make sure we have a file, not directory, name. -FM
     */
    if (*(fn = LYPathLeaf(fn)) == '\0')
	return;

    /*
     *  Check the anchor's content_type and content_encoding
     *  elements for a gzip or Unix compressed file. - FM
     */
    ct = HTAnchor_content_type(anchor);
    ce = HTAnchor_content_encoding(anchor);
    if (ce == NULL && ct != 0) {
	/*
	 *  No Content-Encoding, so check
	 *  the Content-Type. - FM
	 */
	if (!strncasecomp(ct, "application/gzip", 16) ||
	    !strncasecomp(ct, "application/x-gzip", 18)) {
	    method = cftGzip;
	} else if (!strncasecomp(ct, "application/compress", 20) ||
		   !strncasecomp(ct, "application/x-compress", 22)) {
	    method = cftCompress;
	} else if (!strncasecomp(ct, "application/bzip2", 17) ||
		   !strncasecomp(ct, "application/x-bzip2", 19)) {
	    method = cftBzip2;
	}
    } else if (ce != 0) {
	if (!strcasecomp(ce, "gzip") ||
	    !strcasecomp(ce, "x-gzip")) {
	    method = cftGzip;
	} else if (!strcasecomp(ce, "compress") ||
		   !strcasecomp(ce, "x-compress")) {
	    method = cftCompress;
	} else if (!strcasecomp(ce, "bzip2") ||
		   !strcasecomp(ce, "x-bzip2")) {
	    method = cftBzip2;
	}
    }

    /*
     *  If no Content-Encoding has been detected via the anchor
     *  pointer, but strip_ok is not set, there is nothing left
     *  to do. - kw
     */
    if ((method == cftNone) && !strip_ok)
	return;

    /*
     * Treat .tgz specially
     */
    if ((dot = strrchr(fn, '.')) != NULL
     && !strcasecomp(dot, ".tgz")) {
	if (method == cftNone) {
	    strcpy(dot, ".tar");
	}
	return;
    }

    /*
     *  Seek the last dot, and check whether
     *  we have a gzip or compress suffix. - FM
     */
    if ((dot = strrchr(fn, '.')) != NULL) {
	if (HTCompressFileType(fn, ".", &cp) != cftNone) {
	    if (method == cftNone) {
		/*
		 *  It has a suffix which signifies a gzipped
		 *  or compressed file for us, but the anchor
		 *  claims otherwise, so tweak the suffix. - FM
		 */
		*dot = '\0';
	    }
	    return;
	}
	if ((second = HTCompressFileType(fn, "-_", &cp)) != cftNone) {
	    if (method == cftNone) {
		/*
		 *  It has a tail which signifies a gzipped
		 *  file for us, but the anchor claims otherwise,
		 *  so tweak the suffix. - FM
		 */
		if (cp == dot+1)
		    cp--;
		*cp = '\0';
	    } else {
		/*
		 *  The anchor claims it's gzipped, and we
		 *  believe it, so force this tail to the
		 *  conventional suffix. - FM
		 */
#ifdef VMS
		*cp = '-';
#else
		*cp = '.';
#endif /* VMS */
		if (second == cftCompress)
		    LYUpperCase(cp);
		else
		    LYLowerCase(cp);
	    }
	    return;
	}
    }

    switch (method) {
    default:
	suffix = "";
	break;
    case cftCompress:
	suffix = ".Z";
	break;
    case cftGzip:
	suffix = ".gz";
	break;
    case cftBzip2:
	suffix = ".bz2";
	break;
    }

    /*
     *  Add the appropriate suffix. - FM
     */
    if (*suffix) {
	if (!dot) {
	    StrAllocCat(*fname, suffix);
	} else if (*++dot == '\0') {
	    StrAllocCat(*fname, suffix + 1);
	} else {
	    StrAllocCat(*fname, suffix);
#ifdef VMS
	    (*fname)[strlen(*fname) - strlen(suffix)] = '-';
#endif /* !VMS */
	}
    }
}

/*
 *  HText_getLastModified returns the Last-Modified header
 *  if available, for the current document. - FM
 */
PUBLIC CONST char * HText_getLastModified NOARGS
{
    return(HTMainText ?
	  HTAnchor_last_modified(HTMainText->node_anchor) : 0);
}

/*
 *  HText_getDate returns the Date header
 *  if available, for the current document. - FM
 */
PUBLIC CONST char * HText_getDate NOARGS
{
    return(HTMainText ?
	  HTAnchor_date(HTMainText->node_anchor) : 0);
}

/*
 *  HText_getServer returns the Server header
 *  if available, for the current document. - FM
 */
PUBLIC CONST char * HText_getServer NOARGS
{
    return(HTMainText ?
	  HTAnchor_server(HTMainText->node_anchor) : 0);
}

/*
 *  HText_pageDisplay displays a screen of text
 *  starting from the line 'line_num'-1.
 *  This is the primary call for lynx.
 */
PUBLIC void HText_pageDisplay ARGS2(
	int,		line_num,
	char *,		target)
{
#ifdef DISP_PARTIAL
    if (debug_display_partial || (LYTraceLogFP != NULL)) {
	CTRACE((tfp, "GridText: HText_pageDisplay at line %d started\n", line_num));
    }

    if (display_partial) {
	int stop_before = -1;
	/*
	**  Garbage is reported from forms input fields in incremental mode.
	**  So we start HText_trimHightext() to forget this side effect.
	**  This function was split-out from HText_endAppend().
	**  It may not be the best solution but it works. - LP
	**
	**  (FALSE =  indicate that we are in partial mode)
	**  Multiple calls of HText_trimHightext works without problem now.
	*/
	if (HTMainText && HTMainText->stbl)
	    stop_before = Stbl_getStartLine(HTMainText->stbl);
	HText_trimHightext(HTMainText, FALSE, stop_before);
    }
#endif

    display_page(HTMainText, line_num-1, target);

#ifdef DISP_PARTIAL
    if (display_partial && debug_display_partial)
	LYSleepMsg();
#endif

    is_www_index = HTAnchor_isIndex(HTMainAnchor);

#ifdef DISP_PARTIAL
    if (debug_display_partial || (LYTraceLogFP != NULL)) {
	CTRACE((tfp, "GridText: HText_pageDisplay finished\n"));
    }
#endif
}

/*
 *  Return YES if we have a whereis search target on the displayed
 *  page. - kw
 */
PUBLIC BOOL HText_pageHasPrevTarget NOARGS
{
    if (!HTMainText)
	return NO;
    else
	return HTMainText->page_has_target;
}

/*
 *  HText_LinksInLines returns the number of links in the
 *  'Lines' number of lines beginning with 'line_num'-1. - FM
 */
PUBLIC int HText_LinksInLines ARGS3(
	HText *,	text,
	int,		line_num,
	int,		Lines)
{
    int total = 0;
    int start = (line_num - 1);
    int end = (start + Lines);
    TextAnchor *Anchor_ptr = NULL;

    if (!text)
	return total;

    for (Anchor_ptr = text->first_anchor;
		Anchor_ptr != NULL && Anchor_ptr->line_num <= end;
			Anchor_ptr = Anchor_ptr->next) {
	if (Anchor_ptr->line_num >= start &&
	    Anchor_ptr->line_num < end &&
	    Anchor_ptr->show_anchor &&
	    (Anchor_ptr->link_type != INPUT_ANCHOR ||
	     Anchor_ptr->input_field->type != F_HIDDEN_TYPE))
	    ++total;
	if (Anchor_ptr == text->last_anchor)
	    break;
    }

    return total;
}

PUBLIC void HText_setStale ARGS1(
	HText *,	text)
{
    text->stale = YES;
}

PUBLIC void HText_refresh ARGS1(
	HText *,	text)
{
    if (text->stale)
	display_page(text, text->top_of_screen, "");
}

PUBLIC int HText_sourceAnchors ARGS1(
	HText *,	text)
{
    return (text ? text->last_anchor_number : -1);
}

PUBLIC BOOL HText_canScrollUp ARGS1(
	HText *,	text)
{
    return (BOOL) (text->top_of_screen != 0);
}

PUBLIC BOOL HText_canScrollDown NOARGS
{
    HText * text = HTMainText;

    return (BOOL) ((text != 0)
     && ((text->top_of_screen + display_lines) < text->Lines+1));
}

/*		Scroll actions
*/
PUBLIC void HText_scrollTop ARGS1(
	HText *,	text)
{
    display_page(text, 0, "");
}

PUBLIC void HText_scrollDown ARGS1(
	HText *,	text)
{
    display_page(text, text->top_of_screen + display_lines, "");
}

PUBLIC void HText_scrollUp ARGS1(
	HText *,	text)
{
    display_page(text, text->top_of_screen - display_lines, "");
}

PUBLIC void HText_scrollBottom ARGS1(
	HText *,	text)
{
    display_page(text, text->Lines - display_lines, "");
}


/*		Browsing functions
**		==================
*/

/* Bring to front and highlight it
*/
PRIVATE int line_for_char ARGS2(
	HText *,	text,
	int,		char_num)
{
    int line_number = 0;
    int characters = 0;
    HTLine * line = text->last_line->next;
    for (;;) {
	if (line == text->last_line) return 0;	/* Invalid */
	characters = characters + line->size + 1;
	if (characters > char_num) return line_number;
	line_number ++;
	line = line->next;
    }
}

PUBLIC BOOL HText_select ARGS1(
	HText *,	text)
{
    if (text != HTMainText) {
	/*
	 *  Reset flag for whereis search string - cannot be true here
	 *  since text is not our HTMainText. - kw
	 */
	if (text)
	    text->page_has_target = NO;

#ifdef DISP_PARTIAL
	/* Reset these for the previous and current text. - kw */
	text->first_lineno_last_disp_partial =
	    text->last_lineno_last_disp_partial = -1;
	if (HTMainText) {
	    HTMainText->first_lineno_last_disp_partial =
		HTMainText->last_lineno_last_disp_partial = -1;
	}
#endif /* DISP_PARTIAL */

	if (HTMainText) {
	    if (HText_hasUTF8OutputSet(HTMainText) &&
		HTLoadedDocumentEightbit() &&
		LYCharSet_UC[current_char_set].enc == UCT_ENC_UTF8) {
		text->had_utf8 = HTMainText->has_utf8;
	    } else {
		text->had_utf8 = NO;
	    }
	    HTMainText->has_utf8 = NO;
	    text->has_utf8 = NO;
	}

	HTMainText = text;
	HTMainAnchor = text->node_anchor;

	/*
	 *  Make this text the most current in the loaded texts list. - FM
	 */
	if (loaded_texts && HTList_removeObject(loaded_texts, text))
	    HTList_addObject(loaded_texts, text);
	    /* let lynx do it */
	    /* display_page(text, text->top_of_screen, ""); */
    }
    return YES;
}

/*
 *  This function returns TRUE if doc's post_data, address
 *  and isHEAD elements are identical to those of a loaded
 *  (memory cached) text. - FM
 */
PUBLIC BOOL HText_POSTReplyLoaded ARGS1(
	document *,	doc)
{
    HText *text = NULL;
    HTList *cur = loaded_texts;
    char *post_data, *address;
    BOOL is_head;

    /*
     *  Make sure we have the structures. - FM
     */
    if (!cur || !doc)
	return(FALSE);

    /*
     *  Make sure doc is for a POST reply. - FM
     */
    if ((post_data = doc->post_data) == NULL ||
	(address = doc->address) == NULL)
	return(FALSE);
    is_head = doc->isHEAD;

    /*
     *  Loop through the loaded texts looking for a
     *  POST reply match. - FM
     */
    while (NULL != (text = (HText *)HTList_nextObject(cur))) {
	if (text->node_anchor &&
	    text->node_anchor->post_data &&
	    !strcmp(post_data, text->node_anchor->post_data) &&
	    text->node_anchor->address &&
	    !strcmp(address, text->node_anchor->address) &&
	    is_head == text->node_anchor->isHEAD) {
	    return(TRUE);
	}
    }

    return(FALSE);
}

PUBLIC BOOL HTFindPoundSelector ARGS1(
	char *,		selector)
{
    TextAnchor * a;

    for (a=HTMainText->first_anchor; a; a=a->next) {

	if (a->anchor && a->anchor->tag)
	    if (!strcmp(a->anchor->tag, selector)) {

		www_search_result = a->line_num+1;

		CTRACE((tfp,
		       "HText: Selecting anchor [%d] at character %d, line %d\n",
				     a->number, a->start, www_search_result));
		if (!strcmp(selector, LYToolbarName)) {
		    --www_search_result;
		}
		return(YES);
	    }
    }

    return(NO);

}

PUBLIC BOOL HText_selectAnchor ARGS2(
	HText *,		text,
	HTChildAnchor *,	anchor)
{
    TextAnchor * a;

/* This is done later, hence HText_select is unused in GridText.c
   Should it be the contrary ? @@@
    if (text != HTMainText) {
	HText_select(text);
    }
*/

    for (a=text->first_anchor; a; a=a->next) {
	if (a->anchor == anchor) break;
    }
    if (!a) {
	CTRACE((tfp, "HText: No such anchor in this text!\n"));
	return NO;
    }

    if (text != HTMainText) {		/* Comment out by ??? */
	HTMainText = text;		/* Put back in by tbl 921208 */
	HTMainAnchor = text->node_anchor;
    }

    {
	 int l = line_for_char(text, a->start);
	 CTRACE((tfp,
	    "HText: Selecting anchor [%d] at character %d, line %d\n",
	    a->number, a->start, l));

	if ( !text->stale &&
	     (l >= text->top_of_screen) &&
	     ( l < text->top_of_screen + display_lines+1))
		 return YES;

	www_search_result = l - (display_lines/3); /* put in global variable */
    }

    return YES;
}


/*		Editing functions		- NOT IMPLEMENTED
**		=================
**
**	These are called from the application.  There are many more functions
**	not included here from the original text object.
*/

/*	Style handling:
*/
/*	Apply this style to the selection
*/
PUBLIC void HText_applyStyle ARGS2(
	HText *,	me GCC_UNUSED,
	HTStyle *,	style GCC_UNUSED)
{

}


/*	Update all text with changed style.
*/
PUBLIC void HText_updateStyle ARGS2(
	HText *,	me GCC_UNUSED,
	HTStyle *,	style GCC_UNUSED)
{

}


/*	Return style of  selection
*/
PUBLIC HTStyle * HText_selectionStyle ARGS2(
	HText *,		me GCC_UNUSED,
	HTStyleSheet *,		sheet GCC_UNUSED)
{
    return 0;
}


/*	Paste in styled text
*/
PUBLIC void HText_replaceSel ARGS3(
	HText *,	me GCC_UNUSED,
	CONST char *,	aString GCC_UNUSED,
	HTStyle *,	aStyle GCC_UNUSED)
{
}


/*	Apply this style to the selection and all similarly formatted text
**	(style recovery only)
*/
PUBLIC void HTextApplyToSimilar ARGS2(
	HText *,	me GCC_UNUSED,
	HTStyle *,	style GCC_UNUSED)
{

}


/*	Select the first unstyled run.
**	(style recovery only)
*/
PUBLIC void HTextSelectUnstyled ARGS2(
	HText *,		me GCC_UNUSED,
	HTStyleSheet *,		sheet GCC_UNUSED)
{

}


/*	Anchor handling:
*/
PUBLIC void HText_unlinkSelection ARGS1(
	HText *,	me GCC_UNUSED)
{

}

PUBLIC HTAnchor * HText_referenceSelected ARGS1(
	HText *,	me GCC_UNUSED)
{
     return 0;
}


PUBLIC int HText_getTopOfScreen NOARGS
{
    HText * text = HTMainText;
    return text != 0 ? text->top_of_screen : 0;
}

PUBLIC int HText_getLines ARGS1(
	HText *,	text)
{
    return text->Lines;
}

PUBLIC HTAnchor * HText_linkSelTo ARGS2(
	HText *,	me GCC_UNUSED,
	HTAnchor *,	anchor GCC_UNUSED)
{
    return 0;
}

/*
 *  Utility for freeing the list of previous isindex and whereis queries. - FM
 */
PUBLIC void HTSearchQueries_free NOARGS
{
    char *query;
    HTList *cur = search_queries;

    if (!cur)
	return;

    while (NULL != (query = (char *)HTList_nextObject(cur))) {
	FREE(query);
    }
    HTList_delete(search_queries);
    search_queries = NULL;
    return;
}

/*
 *  Utility for listing isindex and whereis queries, making
 *  any repeated queries the most current in the list. - FM
 */
PUBLIC void HTAddSearchQuery ARGS1(
	char *,		query)
{
    char *new_query = NULL;
    char *old;
    HTList *cur;

    if (!(query && *query))
	return;

    StrAllocCopy(new_query, query);

    if (!search_queries) {
	search_queries = HTList_new();
#ifdef LY_FIND_LEAKS
	atexit(HTSearchQueries_free);
#endif
	HTList_addObject(search_queries, new_query);
	return;
    }

    cur = search_queries;
    while (NULL != (old = (char *)HTList_nextObject(cur))) {
	if (!strcmp(old, new_query)) {
	    HTList_removeObject(search_queries, old);
	    FREE(old);
	    break;
	}
    }
    HTList_addObject(search_queries, new_query);

    return;
}

PUBLIC int do_www_search ARGS1(
	document *,	doc)
{
    char searchstring[256], temp[256], *cp, *tmpaddress = NULL;
    int ch, recall;
    int QueryTotal;
    int QueryNum;
    BOOLEAN PreviousSearch = FALSE;

    /*
     *  Load the default query buffer
     */
    if ((cp=strchr(doc->address, '?')) != NULL) {
	/*
	 *  This is an index from a previous search.
	 *  Use its query as the default.
	 */
	PreviousSearch = TRUE;
	LYstrncpy(searchstring, ++cp, sizeof(searchstring)-1);
	for (cp=searchstring; *cp; cp++)
	    if (*cp == '+')
		*cp = ' ';
	HTUnEscape(searchstring);
	strcpy(temp, searchstring);
	/*
	 *  Make sure it's treated as the most recent query. - FM
	 */
	HTAddSearchQuery(searchstring);
    } else {
	/*
	 *  New search; no default.
	 */
	searchstring[0] = '\0';
	temp[0] = '\0';
    }

    /*
     *  Prompt for a query string.
     */
    if (searchstring[0] == '\0') {
	if (HTMainAnchor->isIndexPrompt)
	    _statusline(HTMainAnchor->isIndexPrompt);
	else
	    _statusline(ENTER_DATABASE_QUERY);
    } else
	_statusline(EDIT_CURRENT_QUERY);
    QueryTotal = (search_queries ? HTList_count(search_queries) : 0);
    recall = (((PreviousSearch && QueryTotal >= 2) ||
	       (!PreviousSearch && QueryTotal >= 1)) ? RECALL : NORECALL);
    QueryNum = QueryTotal;
get_query:
    if ((ch=LYgetstr(searchstring, VISIBLE,
		     sizeof(searchstring), recall)) < 0 ||
	*searchstring == '\0' || ch == UPARROW || ch == DNARROW) {
	if (recall && ch == UPARROW) {
	    if (PreviousSearch) {
		/*
		 *  Use the second to last query in the list. - FM
		 */
		QueryNum = 1;
		PreviousSearch = FALSE;
	    } else {
		/*
		 *  Go back to the previous query in the list. - FM
		 */
		QueryNum++;
	    }
	    if (QueryNum >= QueryTotal)
		/*
		 *  Roll around to the last query in the list. - FM
		 */
		QueryNum = 0;
	    if ((cp=(char *)HTList_objectAt(search_queries,
					    QueryNum)) != NULL) {
		LYstrncpy(searchstring, cp, sizeof(searchstring)-1);
		if (*temp && !strcmp(temp, searchstring)) {
		    _statusline(EDIT_CURRENT_QUERY);
		} else if ((*temp && QueryTotal == 2) ||
			   (!(*temp) && QueryTotal == 1)) {
		    _statusline(EDIT_THE_PREV_QUERY);
		} else {
		    _statusline(EDIT_A_PREV_QUERY);
		}
		goto get_query;
	    }
	} else if (recall && ch == DNARROW) {
	    if (PreviousSearch) {
		/*
		 *  Use the first query in the list. - FM
		 */
		QueryNum = QueryTotal - 1;
		PreviousSearch = FALSE;
	    } else {
		/*
		 *  Advance to the next query in the list. - FM
		 */
		QueryNum--;
	    }
	    if (QueryNum < 0)
		/*
		 *  Roll around to the first query in the list. - FM
		 */
		QueryNum = QueryTotal - 1;
	    if ((cp=(char *)HTList_objectAt(search_queries,
					    QueryNum)) != NULL) {
		LYstrncpy(searchstring, cp, sizeof(searchstring)-1);
		if (*temp && !strcmp(temp, searchstring)) {
		    _statusline(EDIT_CURRENT_QUERY);
		} else if ((*temp && QueryTotal == 2) ||
			   (!(*temp) && QueryTotal == 1)) {
		    _statusline(EDIT_THE_PREV_QUERY);
		} else {
		    _statusline(EDIT_A_PREV_QUERY);
		}
		goto get_query;
	    }
	}

	/*
	 *  Search cancelled.
	 */
	HTInfoMsg(CANCELLED);
	return(NULLFILE);
    }

    /*
     *  Strip leaders and trailers. - FM
     */
    LYTrimLeading(searchstring);
    if (!(*searchstring)) {
	HTInfoMsg(CANCELLED);
	return(NULLFILE);
    }
    LYTrimTrailing(searchstring);

    /*
     *  Don't resubmit the same query unintentionally.
     */
    if (!LYforce_no_cache && 0 == strcmp(temp, searchstring)) {
	HTUserMsg(USE_C_R_TO_RESUB_CUR_QUERY);
	return(NULLFILE);
    }

    /*
     *  Add searchstring to the query list,
     *  or make it the most current. - FM
     */
    HTAddSearchQuery(searchstring);

    /*
     *  Show the URL with the new query.
     */
    if ((cp=strchr(doc->address, '?')) != NULL)
	*cp = '\0';
    StrAllocCopy(tmpaddress, doc->address);
    StrAllocCat(tmpaddress, "?");
    StrAllocCat(tmpaddress, searchstring);
    user_message(WWW_WAIT_MESSAGE, tmpaddress);
#if !defined(VMS) && defined(SYSLOG_REQUESTED_URLS)
    LYSyslog(tmpaddress);
#endif /* !VMS && SYSLOG_REQUESTED_URLS */
    FREE(tmpaddress);
    if (cp)
	*cp = '?';

    /*
     *  OK, now we do the search.
     */
    if (HTSearch(searchstring, HTMainAnchor)) {
	/*
	 *	Memory leak fixed.
	 *	05-28-94 Lynx 2-3-1 Garrett Arch Blythe
	 */
	auto char *cp_freeme = NULL;
	if (traversal)
	    cp_freeme = stub_HTAnchor_address((HTAnchor *)HTMainAnchor);
	else
	    cp_freeme = HTAnchor_address((HTAnchor *)HTMainAnchor);
	StrAllocCopy(doc->address, cp_freeme);
	FREE(cp_freeme);

	CTRACE((tfp,"\ndo_www_search: newfile: %s\n",doc->address));

	/*
	 *  Yah, the search succeeded.
	 */
	return(NORMAL);
    }

    /*
     *  Either the search failed (Yuk), or we got redirection.
     *  If it's redirection, use_this_url_instead is set, and
     *  mainloop() will deal with it such that security features
     *  and restrictions are checked before acting on the URL, or
     *  rejecting it. - FM
     */
    return(NOT_FOUND);
}

PRIVATE void write_offset ARGS2(
	FILE *,		fp,
	HTLine *,	line)
{
    int i;

    if (line->data[0]) {
	for (i = 0; i < (int)line->offset; i++) {
	     fputc(' ', fp);
	}
    }
}

PRIVATE void write_hyphen ARGS1(
	FILE *,		fp)
{
    if (dump_output_immediately &&
	LYRawMode &&
	LYlowest_eightbit[current_char_set] <= 173 &&
	(LYCharSet_UC[current_char_set].enc == UCT_ENC_8859 ||
	 (LYCharSet_UC[current_char_set].like8859 & UCT_R_8859SPECL)) != 0) {
	fputc(0xad, fp); /* the iso8859 byte for SHY */
    } else {
	fputc('-', fp);
    }
}

/*
 *  Print the contents of the file in HTMainText to
 *  the file descriptor fp.
 *  If is_reply is TRUE add ">" to the beginning of each
 *  line to specify the file is a reply to message.
 */
PUBLIC void print_wwwfile_to_fd ARGS2(
	FILE *,		fp,
	BOOLEAN,	is_reply)
{
    register int i;
    int first = TRUE;
    HTLine * line;
#ifndef NO_DUMP_WITH_BACKSPACES
    HText* text = HTMainText;
    BOOL in_b = FALSE;
    BOOL in_u = FALSE;
    BOOL bs = (BOOL)(!is_reply
		&& text != 0
		&& with_backspaces
		&& HTCJK == NOCJK
		&& !text->T.output_utf8);
#endif

    if (!HTMainText)
	return;

    line = HTMainText->last_line->next;
    for (;; line = line->next) {
	if (!first && line->data[0] != LY_SOFT_NEWLINE) {
	    fputc('\n',fp);
	    /*
	     *  Add news-style quotation if requested. - FM
	     */
	    if (is_reply) {
		fputc('>',fp);
	    }
	}

	first = FALSE;
	write_offset(fp, line);

	/*
	 *  Add data.
	 */
	for (i = 0; line->data[i] != '\0'; i++) {
	    if (!IsSpecialAttrChar(line->data[i])) {
#ifndef NO_DUMP_WITH_BACKSPACES
		if (in_b) {
		    fputc(line->data[i], fp);
		    fputc('\b',fp);
		    fputc(line->data[i], fp);
		} else if (in_u) {
		    fputc('_',fp);
		    fputc('\b',fp);
		    fputc(line->data[i], fp);
		} else
#endif
		fputc(line->data[i], fp);
	    } else if (line->data[i] == LY_SOFT_HYPHEN &&
		line->data[i + 1] == '\0') { /* last char on line */
		write_hyphen(fp);
	    } else if (dump_output_immediately && use_underscore) {
		switch (line->data[i]) {
		    case LY_UNDERLINE_START_CHAR:
		    case LY_UNDERLINE_END_CHAR:
			fputc('_', fp);
			break;
		    case LY_BOLD_START_CHAR:
		    case LY_BOLD_END_CHAR:
			break;
		}
	    }
#ifndef NO_DUMP_WITH_BACKSPACES
	    else if (bs) {
		switch (line->data[i]) {
		    case LY_UNDERLINE_START_CHAR:
			if (!in_b)
			    in_u = TRUE; /*favor bold over underline*/
			break;
		    case LY_UNDERLINE_END_CHAR:
			in_u = FALSE;
			break;
		    case LY_BOLD_START_CHAR:
			if (in_u)
			    in_u = FALSE; /* turn it off*/
			in_b = TRUE;
			break;
		    case LY_BOLD_END_CHAR:
			in_b = FALSE;
			break;
		}
	    }
#endif
	}

	if (line == HTMainText->last_line)
	    break;

#ifdef VMS
	if (HadVMSInterrupt)
	    break;
#endif /* VMS */
    }
    fputc('\n',fp);

}

/*
 *  Print the contents of the file in HTMainText to
 *  the file descriptor fp.
 *  First output line is "thelink", ie, the URL for this file.
 */
PUBLIC void print_crawl_to_fd ARGS3(
	FILE *,		fp,
	char *,		thelink,
	char *,		thetitle)
{
    register int i;
    int first = TRUE;
    HTLine * line;

    if (!HTMainText)
	return;

    line = HTMainText->last_line->next;
    fprintf(fp, "THE_URL:%s\n", thelink);
    if (thetitle != NULL) {
	fprintf(fp, "THE_TITLE:%s\n", thetitle);
    }

    for (;; line = line->next) {
	if (!first && line->data[0] != LY_SOFT_NEWLINE)
	    fputc('\n',fp);
	first = FALSE;
	write_offset(fp, line);

	/*
	 *  Add data.
	 */
	for (i = 0; line->data[i] != '\0'; i++) {
	    if (!IsSpecialAttrChar(line->data[i])) {
		fputc(line->data[i], fp);
	    } else if (line->data[i] == LY_SOFT_HYPHEN &&
		line->data[i + 1] == '\0') { /* last char on line */
		write_hyphen(fp);
	    }
	}

	if (line == HTMainText->last_line) {
	    break;
	}
    }
    fputc('\n',fp);

    /*
     *  Add the References list if appropriate
     */
    if ((nolist == FALSE) &&
	(keypad_mode == LINKS_ARE_NUMBERED ||
	 keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED)) {
	printlist(fp,FALSE);
    }

#ifdef VMS
    HadVMSInterrupt = FALSE;
#endif /* VMS */
}

PRIVATE void adjust_search_result ARGS3(
    document *,	doc,
    int,	tentative_result,
    int,	start_line)
{
    if (tentative_result > 0) {
	int anch_line = -1;
	TextAnchor * a;
	int nl_closest = -1;
	int goal = SEARCH_GOAL_LINE;
	int max_offset;
	BOOL on_screen = (BOOL) (tentative_result > HTMainText->top_of_screen &&
	    tentative_result <= HTMainText->top_of_screen+display_lines);
	if (goal < 1)
	    goal = 1;
	else if (goal > display_lines)
	    goal = display_lines;
	max_offset = goal - 1;

	if (on_screen && nlinks > 0) {
	    int i;
	    for (i = 0; i < nlinks; i++) {
		if (doc->line + links[i].ly - 1 <= tentative_result)
		    nl_closest = i;
		if (doc->line + links[i].ly - 1 >= tentative_result)
		    break;
	    }
	    if (nl_closest >= 0 &&
		doc->line + links[nl_closest].ly - 1 == tentative_result) {
		www_search_result = doc->line;
		doc->link = nl_closest;
		return;
	    }
	}

	/* find last anchor before or on target line */
	for (a = HTMainText->first_anchor;
	     a && a->line_num <= tentative_result-1; a = a->next) {
	    anch_line = a->line_num + 1;
	}
	/* position such that the anchor found is on first screen line,
	   if it is not too far above the target line; but also try to
	   make sure we move forward. */
	if (anch_line >= 0 &&
	    anch_line >= tentative_result - max_offset &&
	    (anch_line > start_line ||
		tentative_result <= HTMainText->top_of_screen)) {
	    www_search_result = anch_line;
	} else
	if (tentative_result - start_line > 0 &&
	    tentative_result - (start_line + 1) <= max_offset) {
	    www_search_result = start_line + 1;
	} else
	if (tentative_result > HTMainText->top_of_screen &&
	    tentative_result <= start_line && /* have wrapped */
	    tentative_result <= HTMainText->top_of_screen+goal) {
	    www_search_result = HTMainText->top_of_screen + 1;
	} else
	if (tentative_result <= goal)
	    www_search_result = 1;
	else
	    www_search_result = tentative_result - max_offset;
	if (www_search_result == doc->line) {
	    if (nl_closest >= 0) {
		doc->link = nl_closest;
		return;
	    }
	}
    }
}

/*
   John Bley, April 1, 1999 (No joke)
   www_user_search_internals was spawned from www_user_search to
   remove a cut-n-paste coding hack: basically, this entire function
   was duplicated at the two points that www_user_search now calls it.
   And, because www_user_search has a return value defined as modification
   of the screen and some global values, and since it used an awkward for(;;)
   construct, this method has to distinguish between when it's "really"
   returning and when it's just falling through via a break; in the
   infinite-for-loop.  So, basically, we have a large amount of arguments
   since this loop used to be directly in www_user_search, and we return
   1 to say we're "really" returning and 0 to indicate we fell through.
   Also, due to exactly one difference between the first pass of this
   code and the second pass, we have the "firstpass" argument, which is
   true iff it's the first pass.

   I hate cut-n-paste coding.
 */
PRIVATE int www_user_search_internals ARGS8(
	int,		firstpass,
	int,		start_line,
	document *,	doc,
	char *,		target,
	TextAnchor *,	a,
	HTLine *,	line,
	int *,		count,
	int *,		tentative_result)
{
    OptionType * option;
    char *stars = NULL, *cp;

    for (;;) {
	while ((a != NULL) && a->line_num == (*count - 1)) {
	    if (a->show_anchor &&
		(a->link_type != INPUT_ANCHOR ||
		 a->input_field->type != F_HIDDEN_TYPE)) {
		if (((a->hightext != NULL && case_sensitive == TRUE) &&
		     LYno_attr_char_strstr(a->hightext, target)) ||
		    ((a->hightext != NULL && case_sensitive == FALSE) &&
		     LYno_attr_char_case_strstr(a->hightext, target))) {
		    adjust_search_result(doc, *count, start_line);
		    return 1;
		}
		if (((a->hightext2 != NULL && case_sensitive == TRUE) &&
		     LYno_attr_char_strstr(a->hightext2, target)) ||
		    ((a->hightext2 != NULL && case_sensitive == FALSE) &&
		     LYno_attr_char_case_strstr(a->hightext2, target))) {
		    adjust_search_result(doc, *count, start_line);
		    return 1;
		}

		/*
		 *  Search the relevant form fields, taking the
		 *  case_sensitive setting into account. - FM
		 */
		if ((a->input_field != NULL && a->input_field->value != NULL) &&
		    a->input_field->type != F_HIDDEN_TYPE) {
		    if (a->input_field->type == F_PASSWORD_TYPE) {
			/*
			 *  Check the actual, hidden password, and then
			 *  the displayed string. - FM
			 */
			if (((case_sensitive == TRUE) &&
			     LYno_attr_char_strstr(a->input_field->value,
						   target)) ||
			    ((case_sensitive == FALSE) &&
			     LYno_attr_char_case_strstr(a->input_field->value,
							target))) {
			    adjust_search_result(doc, *count, start_line);
			    return 1;
			}
			StrAllocCopy(stars, a->input_field->value);
			for (cp = stars; *cp != '\0'; cp++)
			    *cp = '*';
			if (((case_sensitive == TRUE) &&
			     LYno_attr_char_strstr(stars, target)) ||
			    ((case_sensitive == FALSE) &&
			     LYno_attr_char_case_strstr(stars, target))) {
			    FREE(stars);
			    adjust_search_result(doc, *count, start_line);
			    return 1;
			}
			FREE(stars);
		   } else if (a->input_field->type == F_OPTION_LIST_TYPE) {
			/*
			 *  Search the option strings that are displayed
			 *  when the popup is invoked. - FM
			 */
			option = a->input_field->select_list;
			while (option != NULL) {
			    if (((option->name != NULL &&
				  case_sensitive == TRUE) &&
				 LYno_attr_char_strstr(option->name,
						       target)) ||
				((option->name != NULL &&
				  case_sensitive == FALSE) &&
				 LYno_attr_char_case_strstr(option->name,
							    target))) {
				adjust_search_result(doc, *count, start_line);
				return 1;
			    }
			    option = option->next;
			}
		    } else if (a->input_field->type == F_RADIO_TYPE) {
			/*
			 *  Search for checked or unchecked parens. - FM
			 */
			if (a->input_field->num_value) {
			    cp = checked_radio;
			} else {
			    cp = unchecked_radio;
			}
			if (((case_sensitive == TRUE) &&
			     LYno_attr_char_strstr(cp, target)) ||
			    ((case_sensitive == FALSE) &&
			     LYno_attr_char_case_strstr(cp, target))) {
			    adjust_search_result(doc, *count, start_line);
			    return 1;
			}
		    } else if (a->input_field->type == F_CHECKBOX_TYPE) {
			/*
			 *  Search for checked or unchecked
			 *  square brackets. - FM
			 */
			if (a->input_field->num_value) {
			    cp = checked_box;
			} else {
			    cp = unchecked_box;
			}
			if (((case_sensitive == TRUE) &&
			     LYno_attr_char_strstr(cp, target)) ||
			    ((case_sensitive == FALSE) &&
			     LYno_attr_char_case_strstr(cp, target))) {
			    adjust_search_result(doc, *count, start_line);
			    return 1;
			}
		    } else {
			/*
			 *  Check the values intended for display.
			 *  May have been found already via the
			 *  hightext search, but make sure here
			 *  that the entire value is searched. - FM
			 */
			if (((case_sensitive == TRUE) &&
			     LYno_attr_char_strstr(a->input_field->value,
						   target)) ||
			    ((case_sensitive == FALSE) &&
			     LYno_attr_char_case_strstr(a->input_field->value,
							target))) {
			    adjust_search_result(doc, *count, start_line);
			    return 1;
			}
		    }
		}
	    }
	    a = a->next;
	}
	if (a != NULL && a->line_num <= (*count - 1)) {
	    a = a->next;
	}

	if (case_sensitive && LYno_attr_char_strstr(line->data, target)) {
	    *tentative_result = *count;
	    break;
	} else if (!case_sensitive &&
		   LYno_attr_char_case_strstr(line->data, target)) {
	    *tentative_result = *count;
	    break;
	/* Note: this is where the two passes differ */
	} else if (firstpass && line == HTMainText->last_line) {
	    /* next line */
	    break;
	} else if (!firstpass && *count > start_line) {
	    HTUserMsg2(STRING_NOT_FOUND, target);
	    return 1;			/* end */
	} else {			/* end */
	    line = line->next;
	    (*count)++;
	}
    }
    /* No, man, we just fell through.  You want to call us again. */
    return 0;
}

PUBLIC void www_user_search ARGS3(
	int,		start_line,
	document *,	doc,
	char *,		target)
{
    HTLine * line;
    int count;
    int tentative_result = -1;
    TextAnchor *a;

    if (!HTMainText) {
	return;
    }

    /*
     *  Advance to the start line.
     */
    line = HTMainText->last_line->next;
    for (count = 1; count <= start_line; line = line->next, count++) {
	if (line == HTMainText->last_line) {
	    line = HTMainText->last_line->next; /* set to first line */
	    count = 1;
	    break;
	}
    }
    a = HTMainText->first_anchor;
    while (a && a->line_num < count - 1) {
	a = a->next;
    }
    if (www_user_search_internals(1, start_line, doc, target,
	a, line, &count, &tentative_result) == 1) {
	return; /* Return the www_user_search_internals result */
    }

    if (tentative_result > 0) {
	adjust_search_result(doc, tentative_result, start_line);
	return;
    }
    /* That didn't work, search from the beginning instead */
    line = HTMainText->last_line->next; /* set to first line */
    count = 1;
    a = HTMainText->first_anchor;
    while (a && a->line_num < count - 1) {
	a = a->next;
    }
    if (www_user_search_internals(0, start_line, doc, target,
	a, line, &count, &tentative_result) == 1) {
	return; /* Return the www_user_search_internals result */
    }
    if (tentative_result > 0) {
	adjust_search_result(doc, tentative_result, start_line);
    }
}

PUBLIC void user_message ARGS2(
	CONST char *,	message,
	CONST char *,	argument)
{
    char *temp = NULL;

    if (message == NULL) {
	mustshow = FALSE;
	return;
    }

    HTSprintf0(&temp, message, (argument == 0) ? "" : argument);

    statusline(temp);

    FREE(temp);
    return;
}

/*
 *  HText_getOwner returns the owner of the
 *  current document.
 */
PUBLIC CONST char * HText_getOwner NOARGS
{
    return(HTMainText ?
	   HTAnchor_owner(HTMainText->node_anchor) : 0);
}

/*
 *  HText_setMainTextOwner sets the owner for the
 *  current document.
 */
PUBLIC void HText_setMainTextOwner ARGS1(
	CONST char *,	owner)
{
    if (!HTMainText)
	return;

    HTAnchor_setOwner(HTMainText->node_anchor, owner);
}

/*
 *  HText_getRevTitle returns the RevTitle element of the
 *  current document, used as the subject for mailto comments
 *  to the owner.
 */
PUBLIC CONST char * HText_getRevTitle NOARGS
{
    return(HTMainText ?
	   HTAnchor_RevTitle(HTMainText->node_anchor) : 0);
}

/*
 *  HText_getContentBase returns the Content-Base header
 *  of the current document.
 */
PUBLIC CONST char * HText_getContentBase NOARGS
{
    return(HTMainText ?
	   HTAnchor_content_base(HTMainText->node_anchor) : 0);
}

/*
 *  HText_getContentLocation returns the Content-Location header
 *  of the current document.
 */
PUBLIC CONST char * HText_getContentLocation NOARGS
{
    return(HTMainText ?
	   HTAnchor_content_location(HTMainText->node_anchor) : 0);
}

/*
 *  HText_getMessageID returns the Message-ID of the
 *  current document.
 */
PUBLIC CONST char * HText_getMessageID NOARGS
{
    return(HTMainText ?
	   HTAnchor_messageID(HTMainText->node_anchor) : NULL);
}

PUBLIC void HTuncache_current_document NOARGS
{
    /*
     *  Should remove current document from memory.
     */
    if (HTMainText) {
	HTParentAnchor * htmain_anchor = HTMainText->node_anchor;

	if (HText_HaveUserChangedForms()) {
	    /*
	     * Issue a warning.  User forms content will be lost.
	     */
	    HTAlert(RELOADING_FORM);
	}

	if (htmain_anchor) {
	    if (!(HTOutputFormat && HTOutputFormat == WWW_SOURCE)) {
		FREE(htmain_anchor->UCStages);
	    }
	}
	CTRACE((tfp, "\nHTuncache.. freeing document for '%s'%s\n",
			    ((htmain_anchor &&
			      htmain_anchor->address) ?
			       htmain_anchor->address : "unknown anchor"),
			    ((htmain_anchor &&
			      htmain_anchor->post_data) ?
				      " with POST data" : "")));
	HTList_removeObject(loaded_texts, HTMainText);
	HText_free(HTMainText);
	HTMainText = NULL;
    } else {
	CTRACE((tfp, "HTuncache.. HTMainText already is NULL!\n"));
    }
}

#ifdef SOURCE_CACHE

PRIVATE HTProtocol scm = { "source-cache-mem", 0, 0 }; /* dummy - kw */

PUBLIC BOOLEAN HTreparse_document NOARGS
{
    BOOLEAN ok = FALSE;
#if 0
    char *source_url = NULL;	/* unused - see comments below */
#endif

    if (!HTMainAnchor || LYCacheSource == SOURCE_CACHE_NONE ||
	(LYCacheSource == SOURCE_CACHE_FILE &&
	!HTMainAnchor->source_cache_file) ||
	(LYCacheSource == SOURCE_CACHE_MEMORY &&
	!HTMainAnchor->source_cache_chunk))
	return FALSE;

    if (LYCacheSource == SOURCE_CACHE_FILE && HTMainAnchor->source_cache_file) {
	FILE * fp;
	HTFormat format;
	int ret;

	CTRACE((tfp, "SourceCache: Reparsing file %s\n",
	      HTMainAnchor->source_cache_file));

	/*
	 * This magic FREE(anchor->UCStages) call
	 * stolen from HTuncache_current_document() above.
	 */
	if (!(HTOutputFormat && HTOutputFormat == WWW_SOURCE)) {
	    FREE(HTMainAnchor->UCStages);
	}

	/*
	 * This is more or less copied out of HTLoadFile(), except we don't
	 * get a content encoding.  This may be overkill.  -dsb
	 */
	if (HTMainAnchor->content_type) {
	    format = HTAtom_for(HTMainAnchor->content_type);
	} else {
	    format = HTFileFormat(HTMainAnchor->source_cache_file, NULL, NULL);
	    format = HTCharsetFormat(format, HTMainAnchor,
					     UCLYhndl_for_unspec);
	    /* not UCLYhndl_HTFile_for_unspec - we are talking about remote
	     * documents...
	     */
	}
	CTRACE((tfp, "  Content type is \"%s\"\n", format->name));

	fp = fopen(HTMainAnchor->source_cache_file, "r");
	if (!fp) {
	    CTRACE((tfp, "  Cannot read file %s\n", HTMainAnchor->source_cache_file));
	    LYRemoveTemp(HTMainAnchor->source_cache_file);
	    FREE(HTMainAnchor->source_cache_file);
	    return FALSE;
	}

	if (HText_HaveUserChangedForms()) {
	    /*
	     * Issue a warning.  Will not restore changed forms, currently.
	     */
	    HTAlert(RELOADING_FORM);
	}
	/* Set HTMainAnchor->protocol or HTMainAnchor->physical to convince
	 * the SourceCacheWriter to not regenerate the cache file (which
	 * would be an unnecessary "loop"). - kw
	 */
#if 0 /* If cache writer looked at physical not protocol, we could use this: */
	LYLocalFileToURL(&source_url, HTMainAnchor->source_cache_file);
	HTAnchor_setPhysical(HTMainAnchor, source_url);
	FREE(source_url);
#endif /* 0 */
	HTAnchor_setProtocol(HTMainAnchor, &HTFile);
	ret = HTParseFile(format, HTOutputFormat, HTMainAnchor, fp, NULL);
	LYCloseInput(fp);
	if (ret == HT_PARTIAL_CONTENT) {
	    HTInfoMsg(gettext("Loading incomplete."));
	    CTRACE((tfp, "SourceCache: `%s' has been accessed, partial content.\n",
		    HTLoadedDocumentURL()));
	}
	ok = (BOOL) (ret == HT_LOADED || ret == HT_PARTIAL_CONTENT);
    }

    if (LYCacheSource == SOURCE_CACHE_MEMORY &&
	HTMainAnchor->source_cache_chunk) {
	HTFormat format = WWW_HTML;
	int ret;

	CTRACE((tfp, "SourceCache: Reparsing from memory chunk %p\n",
		    (void *)HTMainAnchor->source_cache_chunk));

	/*
	 * This magic FREE(anchor->UCStages) call
	 * stolen from HTuncache_current_document() above.
	 */
	if (!(HTOutputFormat && HTOutputFormat == WWW_SOURCE)) {
	    FREE(HTMainAnchor->UCStages);
	}

	if (HTMainAnchor->content_type) {
	    format = HTAtom_for(HTMainAnchor->content_type);
	} else {
	/*
	 * This is only done to make things aligned with SOURCE_CACHE_NONE and
	 * SOURCE_CACHE_FILE when switching to source mode since the original
	 * document's charset will be LYPushAssumed() and then LYPopAssumed().
	 * See LYK_SOURCE in mainloop if you change something here.  No
	 * user-visible benefits, seems just '=' Info Page will show source's
	 * effective charset as "(assumed)".
	 */
	    format = HTCharsetFormat(format, HTMainAnchor,
					     UCLYhndl_for_unspec);
	}
	/* not UCLYhndl_HTFile_for_unspec - we are talking about remote documents... */

	if (HText_HaveUserChangedForms()) {
	    /*
	     * Issue a warning.  Will not restore changed forms, currently.
	     */
	    HTAlert(RELOADING_FORM);
	}
	/* Set HTMainAnchor->protocol or HTMainAnchor->physical to convince
	 * the SourceCacheWriter to not regenerate the cache chunk (which
	 * would be an unnecessary "loop"). - kw
	 */
	HTAnchor_setProtocol(HTMainAnchor, &scm); /* cheating -
				   anything != &HTTP or &HTTPS would do - kw */
#if 0 /* If cache writer looked at physical not protocol, we could use this: */
	HTSprintf0(&source_url, "source-cache-mem:%p",
		   HTMainAnchor->source_cache_chunk);
	HTAnchor_setPhysical(HTMainAnchor, source_url);
	FREE(source_url);
#endif /* 0 */
	ret = HTParseMem(format, HTOutputFormat, HTMainAnchor,
			HTMainAnchor->source_cache_chunk, NULL);
	ok = (BOOL) (ret == HT_LOADED);
    }

    CTRACE((tfp, "Reparse %s\n", (ok ? "succeeded" : "failed")));

    return ok;
}

PUBLIC BOOLEAN HTcan_reparse_document NOARGS
{
    if (!HTMainAnchor || LYCacheSource == SOURCE_CACHE_NONE ||
	(LYCacheSource == SOURCE_CACHE_FILE &&
	!HTMainAnchor->source_cache_file) ||
	(LYCacheSource == SOURCE_CACHE_MEMORY &&
	!HTMainAnchor->source_cache_chunk))
	return FALSE;

    if (LYCacheSource == SOURCE_CACHE_FILE && HTMainAnchor->source_cache_file) {
	return LYCanReadFile(HTMainAnchor->source_cache_file);
    }

    if (LYCacheSource == SOURCE_CACHE_MEMORY &&
	HTMainAnchor->source_cache_chunk) {
	return TRUE;
    }

    return FALSE;  /* if came to here */
}

PRIVATE void trace_setting_change ARGS3(
	CONST char *,	name,
	BOOLEAN,	prev_setting,
	BOOLEAN,	new_setting)
{
    if (prev_setting != new_setting)
	CTRACE((tfp, "HTdocument_settings_changed: %s setting has changed (was %s, now %s)\n",
	       name, prev_setting ? "ON" : "OFF", new_setting ? "ON" : "OFF"));
}

PUBLIC BOOLEAN HTdocument_settings_changed NOARGS
{
    /*
     * Annoying Hack(TM):  If we don't have a source cache, we can't
     * reparse anyway, so pretend the settings haven't changed.
     */
    if (!HTMainAnchor || !HTMainText || LYCacheSource == SOURCE_CACHE_NONE ||
	(LYCacheSource == SOURCE_CACHE_FILE &&
	!HTMainAnchor->source_cache_file) ||
	(LYCacheSource == SOURCE_CACHE_MEMORY &&
	!HTMainAnchor->source_cache_chunk))
	return FALSE;

    if (TRACE) {
	/*
	 * If we're tracing, note everying that has changed.
	 */
	trace_setting_change("CLICKABLE_IMAGES",
			     HTMainText->clickable_images, clickable_images);
	trace_setting_change("PSEUDO_INLINE_ALTS",
			     HTMainText->pseudo_inline_alts,
			     pseudo_inline_alts);
	trace_setting_change("VERBOSE_IMG",
			    HTMainText->verbose_img,
			    verbose_img);
	trace_setting_change("RAW_MODE", HTMainText->raw_mode,
			     LYUseDefaultRawMode);
	trace_setting_change("HISTORICAL_COMMENTS",
			     HTMainText->historical_comments,
			     historical_comments);
	trace_setting_change("MINIMAL_COMMENTS",
			     HTMainText->minimal_comments, minimal_comments);
	trace_setting_change("SOFT_DQUOTES",
			     HTMainText->soft_dquotes, soft_dquotes);
	trace_setting_change("OLD_DTD", HTMainText->old_dtd, Old_DTD);
	trace_setting_change("KEYPAD_MODE",
			     HTMainText->keypad_mode, keypad_mode);
	if (HTMainText->disp_lines != LYlines || HTMainText->disp_cols != LYcols)
	    CTRACE((tfp,
		   "HTdocument_settings_changed: Screen size has changed (was %dx%d, now %dx%d)\n",
		   HTMainText->disp_cols, HTMainText->disp_lines, LYcols, LYlines));
    }

    return (HTMainText->clickable_images != clickable_images ||
	    HTMainText->pseudo_inline_alts != pseudo_inline_alts ||
	    HTMainText->verbose_img != verbose_img ||
	    HTMainText->raw_mode != LYUseDefaultRawMode ||
	    HTMainText->historical_comments != historical_comments ||
	    (HTMainText->minimal_comments != minimal_comments &&
	     !historical_comments) ||
	    HTMainText->soft_dquotes != soft_dquotes ||
	    HTMainText->old_dtd != Old_DTD ||
	    HTMainText->keypad_mode != keypad_mode ||
	    HTMainText->disp_cols != LYcols);
}
#endif

PUBLIC int HTisDocumentSource NOARGS
{
    return (HTMainText != 0) ? HTMainText->source : FALSE;
}

PUBLIC char * HTLoadedDocumentURL NOARGS
{
    if (!HTMainText)
	return ("");

    if (HTMainText->node_anchor && HTMainText->node_anchor->address)
	return(HTMainText->node_anchor->address);
    else
	return ("");
}

PUBLIC char * HTLoadedDocumentPost_data NOARGS
{
    if (!HTMainText)
	return ("");

    if (HTMainText->node_anchor && HTMainText->node_anchor->post_data)
	return(HTMainText->node_anchor->post_data);
    else
	return ("");
}

PUBLIC char * HTLoadedDocumentTitle NOARGS
{
    if (!HTMainText)
	return ("");

    if (HTMainText->node_anchor && HTMainText->node_anchor->title)
	return(HTMainText->node_anchor->title);
    else
	return ("");
}

PUBLIC BOOLEAN HTLoadedDocumentIsHEAD NOARGS
{
    if (!HTMainText)
	return (FALSE);

    if (HTMainText->node_anchor && HTMainText->node_anchor->isHEAD)
	return(HTMainText->node_anchor->isHEAD);
    else
	return (FALSE);
}

PUBLIC BOOLEAN HTLoadedDocumentIsSafe NOARGS
{
    if (!HTMainText)
	return (FALSE);

    if (HTMainText->node_anchor && HTMainText->node_anchor->safe)
	return(HTMainText->node_anchor->safe);
    else
	return (FALSE);
}

PUBLIC char * HTLoadedDocumentCharset NOARGS
{
    if (!HTMainText)
	return (NULL);

    if (HTMainText->node_anchor && HTMainText->node_anchor->charset)
	return(HTMainText->node_anchor->charset);
    else
	return (NULL);
}

PUBLIC BOOL HTLoadedDocumentEightbit NOARGS
{
    if (!HTMainText)
	return (NO);
    else
	return (HTMainText->have_8bit_chars);
}

PUBLIC void HText_setNodeAnchorBookmark ARGS1(
	CONST char *,	bookmark)
{
    if (!HTMainText)
	return;

    if (HTMainText->node_anchor)
	HTAnchor_setBookmark(HTMainText->node_anchor, bookmark);
}

PUBLIC char * HTLoadedDocumentBookmark NOARGS
{
    if (!HTMainText)
	return (NULL);

    if (HTMainText->node_anchor && HTMainText->node_anchor->bookmark)
	return(HTMainText->node_anchor->bookmark);
    else
	return (NULL);
}

PUBLIC int HText_LastLineSize ARGS2(
	HText *,	text,
	BOOL,		IgnoreSpaces)
{
    if (!text || !text->last_line || !text->last_line->size)
	return 0;
    return HText_TrueLineSize(text->last_line, text, IgnoreSpaces);
}

PUBLIC int HText_PreviousLineSize ARGS2(
	HText *,	text,
	BOOL,		IgnoreSpaces)
{
    HTLine * line;

    if (!text || !text->last_line)
	return 0;
    if (!(line = text->last_line->prev))
	return 0;
    return HText_TrueLineSize(line, text, IgnoreSpaces);
}

PRIVATE int HText_TrueLineSize ARGS3(
	HTLine *,	line,
	HText *,	text,
	BOOL,		IgnoreSpaces)
{
    size_t i;
    int true_size = 0;

    if (!(line && line->size))
	return 0;

    if (IgnoreSpaces) {
	for (i = 0; i < line->size; i++) {
	    if (!IsSpecialAttrChar(UCH(line->data[i])) &&
		(!(text && text->T.output_utf8) ||
		 UCH(line->data[i]) < 128 ||
		 (UCH((line->data[i] & 0xc0)) == 0xc0)) &&
		!isspace(UCH(line->data[i])) &&
		UCH(line->data[i]) != HT_NON_BREAK_SPACE &&
		UCH(line->data[i]) != HT_EN_SPACE) {
		true_size++;
	    }
	}
    } else {
	for (i = 0; i < line->size; i++) {
	    if (!IsSpecialAttrChar(line->data[i]) &&
		(!(text && text->T.output_utf8) ||
		 UCH(line->data[i]) < 128 ||
		 (UCH(line->data[i] & 0xc0) == 0xc0))) {
		true_size++;
	    }
	}
    }
    return true_size;
}

PUBLIC void HText_NegateLineOne ARGS1(
	HText *,	text)
{
    if (text) {
	text->in_line_1 = NO;
    }
    return;
}

PUBLIC BOOL HText_inLineOne ARGS1(
	HText *,	text)
{
    if (text) {
	return text->in_line_1;
    }
    return YES;
}

/*
 *  This function is for removing the first of two
 *  successive blank lines.  It should be called after
 *  checking the situation with HText_LastLineSize()
 *  and HText_PreviousLineSize().  Any characters in
 *  the removed line (i.e., control characters, or it
 *  wouldn't have tested blank) should have been
 *  reiterated by split_line() in the retained blank
 *  line. - FM
 */
PUBLIC void HText_RemovePreviousLine ARGS1(
	HText *,	text)
{
    HTLine *line, *previous;
    char *data;

    if (!(text && text->Lines > 1))
	return;

    line = text->last_line->prev;
    data = line->data;
    previous = line->prev;
    previous->next = text->last_line;
    text->last_line->prev = previous;
    text->chars -= ((data && *data == '\0') ?
					  1 : strlen(line->data) + 1);
    text->Lines--;
    FREE(line);
}

/*
 *  NOTE: This function presently is correct only if the
 *	  alignment is HT_LEFT.  The offset is still zero,
 *	  because that's not determined for HT_CENTER or
 *	  HT_RIGHT until subsequent characters are received
 *	  and split_line() is called. - FM
 */
PUBLIC int HText_getCurrentColumn ARGS1(
	HText *,	text)
{
    int column = 0;
    BOOL IgnoreSpaces = FALSE;

    if (text) {
	column = (text->in_line_1 ?
		  (int)text->style->indent1st : (int)text->style->leftIndent)
		  + HText_LastLineSize(text, IgnoreSpaces)
		  + (int)text->last_line->offset;
    }
    return column;
}

PUBLIC int HText_getMaximumColumn ARGS1(
	HText *,	text)
{
    int column = (LYcols-2);
    if (text) {
	column = ((int)text->style->rightIndent ? (LYcols-2) :
		  ((LYcols-1) - (int)text->style->rightIndent));
    }
    return column;
}

/*
 *  NOTE: This function uses HText_getCurrentColumn() which
 *	  presently is correct only if the alignment is
 *	  HT_LEFT. - FM
 */
PUBLIC void HText_setTabID ARGS2(
	HText *,	text,
	CONST char *,	name)
{
    HTTabID * Tab = NULL;
    HTList * cur = text->tabs;
    HTList * last = NULL;

    if (!text || !name || !*name)
	return;

    if (!cur) {
	cur = text->tabs = HTList_new();
    } else {
	while (NULL != (Tab = (HTTabID *)HTList_nextObject(cur))) {
	    if (Tab->name && !strcmp(Tab->name, name))
		return; /* Already set.  Keep the first value. */
	    last = cur;
	}
	if (last)
	    cur = last;
    }
    if (!Tab) { /* New name.  Create a new node */
	Tab = typecalloc(HTTabID);
	if (Tab == NULL)
	    outofmem(__FILE__, "HText_setTabID");
	HTList_addObject(cur, Tab);
	StrAllocCopy(Tab->name, name);
    }
    Tab->column = HText_getCurrentColumn(text);
    return;
}

PUBLIC int HText_getTabIDColumn ARGS2(
	HText *,	text,
	CONST char *,	name)
{
    int column = 0;
    HTTabID * Tab;
    HTList * cur = text->tabs;

    if (text && name && *name && cur) {
	while (NULL != (Tab = (HTTabID *)HTList_nextObject(cur))) {
	    if (Tab->name && !strcmp(Tab->name, name))
		break;
	}
	if (Tab)
	    column = Tab->column;
    }
    return column;
}

/*
 *  This function is for saving the address of a link
 *  which had an attribute in the markup that resolved
 *  to a URL (i.e., not just a NAME or ID attribute),
 *  but was found in HText_endAnchor() to have no visible
 *  content for use as a link name.  It loads the address
 *  into text->hidden_links, whose count can be determined
 *  via HText_HiddenLinks(), below.  The addresses can be
 *  retrieved via HText_HiddenLinkAt(), below, based on
 *  count. - FM
 */
PRIVATE void HText_AddHiddenLink ARGS2(
	HText *,	text,
	TextAnchor *,	textanchor)
{
    HTAnchor *dest;

    /*
     *  Make sure we have an HText structure and anchor. - FM
     */
    if (!(text && textanchor && textanchor->anchor))
	return;

    /*
     *  Create the hidden links list
     *  if it hasn't been already. - FM
     */
    if (text->hidden_links == NULL)
	text->hidden_links = HTList_new();

    /*
     *  Store the address, in reverse list order
     *  so that first in will be first out on
     *  retrievals. - FM
     */
    if ((dest = HTAnchor_followMainLink((HTAnchor *)textanchor->anchor)) &&
	(text->hiddenlinkflag != HIDDENLINKS_IGNORE ||
	 HTList_isEmpty(text->hidden_links)))
	HTList_appendObject(text->hidden_links, HTAnchor_address(dest));

    return;
}

/*
 *  This function returns the number of addresses
 *  that are loaded in text->hidden_links. - FM
 */
PUBLIC int HText_HiddenLinkCount ARGS1(
	HText *,	text)
{
    int count = 0;

    if (text && text->hidden_links)
	count = HTList_count((HTList *)text->hidden_links);

    return(count);
}

/*
 *  This function returns the address, corresponding to
 *  a hidden link, at the position (zero-based) in the
 *  text->hidden_links list of the number argument. - FM
 */
PUBLIC char * HText_HiddenLinkAt ARGS2(
	HText *,	text,
	int,		number)
{
    char *href = NULL;

    if (text && text->hidden_links && number >= 0)
	href = (char *)HTList_objectAt((HTList *)text->hidden_links, number);

    return(href);
}


/*
 *  Form methods
 *    These routines are used to build forms consisting
 *    of input fields
 */
PRIVATE int HTFormMethod;
PRIVATE char * HTFormAction = NULL;
PRIVATE char * HTFormEnctype = NULL;
PRIVATE char * HTFormTitle = NULL;
PRIVATE char * HTFormAcceptCharset = NULL;
PRIVATE BOOLEAN HTFormDisabled = FALSE;
PRIVATE PerFormInfo * HTCurrentForm;

PUBLIC void HText_beginForm ARGS5(
	char *,		action,
	char *,		method,
	char *,		enctype,
	char *,		title,
	CONST char *,	accept_cs)
{
    PerFormInfo * newform;
    HTFormMethod = URL_GET_METHOD;
    HTFormNumber++;
    HTFormFields = 0;
    HTFormDisabled = FALSE;

    /*
     *  Check the ACTION. - FM
     */
    if (action != NULL) {
	if (!strncmp(action, "mailto:", 7)) {
	    HTFormMethod = URL_MAIL_METHOD;
	}
	StrAllocCopy(HTFormAction, action);
    }
    else
	StrAllocCopy(HTFormAction, HTLoadedDocumentURL());

    /*
     *  Check the METHOD. - FM
     */
    if (method != NULL && HTFormMethod != URL_MAIL_METHOD)
	if (!strcasecomp(method,"post") || !strcasecomp(method,"pget"))
	    HTFormMethod = URL_POST_METHOD;

    /*
     *  Check the ENCTYPE. - FM
     */
    if ((enctype != NULL) && *enctype) {
	StrAllocCopy(HTFormEnctype, enctype);
	if (HTFormMethod != URL_MAIL_METHOD &&
	    !strncasecomp(enctype, "multipart/form-data", 19))
	    HTFormMethod = URL_POST_METHOD;
    } else {
	FREE(HTFormEnctype);
    }

    /*
     *  Check the TITLE. - FM
     */
    if ((title != NULL) && *title)
	StrAllocCopy(HTFormTitle, title);
    else
	FREE(HTFormTitle);

    /*
     *  Check for an ACCEPT_CHARSET.  If present, store it and
     *  convert to lowercase and collapse spaces. - kw
     */
    if (accept_cs != NULL) {
	StrAllocCopy(HTFormAcceptCharset, accept_cs);
	LYRemoveBlanks(HTFormAcceptCharset);
	LYLowerCase(HTFormAcceptCharset);
    }

    /*
     *  Create a new "PerFormInfo" structure to hold info on the current
     *  form.  The HTForm* variables could all migrate there, currently
     *  this isn't done (yet?) but it might be less confusing.
     *  Currently the only data saved in this structure that will actually
     *  be used is the accept_cs string.
     *  This will be appended to the forms list kept by the HText object
     *  if and when we reach a HText_endForm. - kw
     */
    newform = typecalloc(PerFormInfo);
    if (newform == NULL)
	outofmem(__FILE__,"HText_beginForm");
    newform->number = HTFormNumber;

    PerFormInfo_free(HTCurrentForm); /* shouldn't happen here - kw */
    HTCurrentForm = newform;

    CTRACE((tfp, "BeginForm: action:%s Method:%d%s%s%s%s%s%s\n",
		HTFormAction, HTFormMethod,
		(HTFormTitle ? " Title:" : ""),
		(HTFormTitle ? HTFormTitle : ""),
		(HTFormEnctype ? " Enctype:" : ""),
		(HTFormEnctype ? HTFormEnctype : ""),
		(HTFormAcceptCharset ? " Accept-charset:" : ""),
		(HTFormAcceptCharset ? HTFormAcceptCharset : "")));
}

PUBLIC void HText_endForm ARGS1(
	HText *,	text)
{
    if (HTFormFields == 1 && text && text->first_anchor) {
	/*
	 *  Support submission of a single text input field in
	 *  the form via <return> instead of a submit button. - FM
	 */
	TextAnchor * a = text->first_anchor;
	/*
	 *  Go through list of anchors and get our input field. - FM
	 */
	while (a) {
	    if (a->link_type == INPUT_ANCHOR &&
		a->input_field->number == HTFormNumber &&
		a->input_field->type == F_TEXT_TYPE) {
		/*
		 *  Got it.  Make it submitting. - FM
		 */
		a->input_field->submit_action = NULL;
		StrAllocCopy(a->input_field->submit_action, HTFormAction);
		if (HTFormEnctype != NULL)
		    StrAllocCopy(a->input_field->submit_enctype,
				 HTFormEnctype);
		if (HTFormTitle != NULL)
		    StrAllocCopy(a->input_field->submit_title, HTFormTitle);
		a->input_field->submit_method = HTFormMethod;
		a->input_field->type = F_TEXT_SUBMIT_TYPE;
		if (HTFormDisabled)
		    a->input_field->disabled = TRUE;
		break;
	    }
	    if (a == text->last_anchor)
		break;
	    a = a->next;
	}
    }
    /*
     *  Append info on the current form to the HText object's list of
     *  forms.
     *  HText_beginInput call will have set some of the data in the
     *  PerFormInfo structure (if there were any form fields at all),
     *  we also fill in the ACCEPT-CHARSET data now (this could have
     *  been done earlier). - kw
     */
    if (HTCurrentForm) {
	if (HTFormDisabled)
	    HTCurrentForm->disabled = TRUE;
	HTCurrentForm->accept_cs = HTFormAcceptCharset;
	HTFormAcceptCharset = NULL;
	if (!text->forms)
	    text->forms = HTList_new();
	HTList_appendObject(text->forms, HTCurrentForm);
	HTCurrentForm = NULL;
    } else {
	CTRACE((tfp, "endForm:    HTCurrentForm is missing!\n"));
    }

    FREE(HTCurSelectGroup);
    FREE(HTCurSelectGroupSize);
    FREE(HTCurSelectedOptionValue);
    FREE(HTFormAction);
    FREE(HTFormEnctype);
    FREE(HTFormTitle);
    FREE(HTFormAcceptCharset);
    HTFormFields = 0;
    HTFormDisabled = FALSE;
}

PUBLIC void HText_beginSelect ARGS4(
	char *,		name,
	int,		name_cs,
	BOOLEAN,	multiple,
	char *,		size)
{
    /*
     *  Save the group name.
     */
    StrAllocCopy(HTCurSelectGroup, name);
    HTCurSelectGroupCharset = name_cs;

    /*
     *  If multiple then all options are actually checkboxes.
     */
    if (multiple)
	HTCurSelectGroupType = F_CHECKBOX_TYPE;
    /*
     *  If not multiple then all options are radio buttons.
     */
    else
	HTCurSelectGroupType = F_RADIO_TYPE;

    /*
     *  Length of an option list.
     */
    StrAllocCopy(HTCurSelectGroupSize, size);

    CTRACE((tfp,"HText_beginSelect: name=%s type=%d size=%s\n",
	       ((HTCurSelectGroup == NULL) ?
				  "<NULL>" : HTCurSelectGroup),
		HTCurSelectGroupType,
	       ((HTCurSelectGroupSize == NULL) ?
				      "<NULL>" : HTCurSelectGroupSize)));
    CTRACE((tfp,"HText_beginSelect: name_cs=%d \"%s\"\n",
		HTCurSelectGroupCharset,
		(HTCurSelectGroupCharset >= 0 ?
		 LYCharSet_UC[HTCurSelectGroupCharset].MIMEname : "<UNKNOWN>")));
}

/*
**  This function returns the number of the option whose
**  value currently is being accumulated for a select
**  block. - LE && FM
*/
PUBLIC int HText_getOptionNum ARGS1(
	HText *,	text)
{
    TextAnchor *a;
    OptionType *op;
    int n = 1; /* start count at 1 */

    if (!(text && text->last_anchor))
	return(0);

    a = text->last_anchor;
    if (!(a->link_type == INPUT_ANCHOR && a->input_field &&
	  a->input_field->type == F_OPTION_LIST_TYPE))
	return(0);

    for (op = a->input_field->select_list; op; op = op->next)
	n++;
    CTRACE((tfp, "HText_getOptionNum: Got number '%d'.\n", n));
    return(n);
}

/*
**  This function checks for a numbered option pattern
**  as the prefix for an option value.  If present, and
**  we are in the correct keypad mode, it returns a
**  pointer to the actual value, following that prefix.
**  Otherwise, it returns the original pointer.
*/
PRIVATE char * HText_skipOptionNumPrefix ARGS1(
	char *,		opname)
{
    /*
     *  Check if we are in the correct keypad mode.
     */
    if (keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) {
	/*
	 *  Skip the option number embedded in the option name so the
	 *  extra chars won't mess up cgi scripts processing the value.
	 *  The format is (nnn)__ where nnn is a number and there is a
	 *  minimum of 5 chars (no underscores if (nnn) exceeds 5 chars).
	 *  See HTML.c.  If the chars don't exactly match this format,
	 *  just use all of opname.  - LE
	 */
	char *cp = opname;

	if ((cp && *cp && *cp++ == '(') &&
	    *cp && isdigit(UCH(*cp++))) {
	    while (*cp && isdigit(UCH(*cp)))
		++cp;
	    if (*cp && *cp++ == ')') {
		int i = (cp - opname);

		while (i < 5) {
		    if (*cp != '_')
			break;
		    i++;
		    cp++;
		}
		if (i < 5 ) {
		    cp = opname;
		}
	    } else {
		cp = opname;
	    }
	} else {
	    cp = opname;
	}
	return(cp);
    }

    return(opname);
}

/*
**  We couldn't set the value field for the previous option
**  tag so we have to do it now.  Assume that the last anchor
**  was the previous options tag.
*/
PUBLIC char * HText_setLastOptionValue ARGS7(
	HText *,	text,
	char *,		value,
	char*,		submit_value,
	int,		order,
	BOOLEAN,	checked,
	int,		val_cs,
	int,		submit_val_cs)
{
    char *cp, *cp1;
    char *ret_Value = NULL;
    unsigned char *tmp = NULL;
    int number = 0, i, j;

    if (!(text && text->last_anchor &&
	  text->last_anchor->link_type == INPUT_ANCHOR)) {
	CTRACE((tfp, "HText_setLastOptionValue: invalid call!  value:%s!\n",
		    (value ? value : "<NULL>")));
	return NULL;
    }

    CTRACE((tfp, "Entering HText_setLastOptionValue: value:%s, checked:%s\n",
		value, (checked ? "on" : "off")));

    /*
     *  Strip end spaces, newline is also whitespace.
     */
    if (*value) {
	cp = &value[strlen(value)-1];
	while ((cp >= value) && (isspace(UCH(*cp)) ||
				 IsSpecialAttrChar(UCH(*cp))))
	    cp--;
	*(cp+1) = '\0';
    }

    /*
     *  Find first non space
     */
    cp = value;
    while (isspace(UCH(*cp)) ||
	   IsSpecialAttrChar(UCH(*cp)))
	cp++;
    if (HTCurSelectGroupType == F_RADIO_TYPE &&
	LYSelectPopups &&
	keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) {
	/*
	 *  Collapse any space between the popup option
	 *  prefix and actual value. - FM
	 */
	if ((cp1 = HText_skipOptionNumPrefix(cp)) > cp) {
	    i = 0, j = (cp1 - cp);
	    while (isspace(UCH(cp1[i])) ||
		   IsSpecialAttrChar(UCH(cp1[i]))) {
		i++;
	    }
	    if (i > 0) {
		while (cp1[i] != '\0')
		    cp[j++] = cp1[i++];
		cp[j] = '\0';
	    }
	}
    }

    if (HTCurSelectGroupType == F_CHECKBOX_TYPE) {
	StrAllocCopy(text->last_anchor->input_field->value, cp);
	text->last_anchor->input_field->value_cs = val_cs;
	/*
	 *  Put the text on the screen as well.
	 */
	HText_appendText(text, cp);

    } else if (LYSelectPopups == FALSE) {
	StrAllocCopy(text->last_anchor->input_field->value,
		     (submit_value ? submit_value : cp));
	text->last_anchor->input_field->value_cs = (submit_value ?
						    submit_val_cs : val_cs);
	/*
	 *  Put the text on the screen as well.
	 */
	HText_appendText(text, cp);

    } else {
	/*
	 *  Create a linked list of option values.
	 */
	OptionType * op_ptr = text->last_anchor->input_field->select_list;
	OptionType * new_ptr = NULL;
	BOOLEAN first_option = FALSE;

	/*
	 *  Deal with newlines or tabs.
	 */
	convert_to_spaces(value, FALSE);

	if (!op_ptr) {
	    /*
	     *  No option items yet.
	     */
	    if (text->last_anchor->input_field->type != F_OPTION_LIST_TYPE) {
		CTRACE((tfp, "HText_setLastOptionValue: last input_field not F_OPTION_LIST_TYPE (%d)\n",
			    F_OPTION_LIST_TYPE));
		CTRACE((tfp, "                          but %d, ignoring!\n",
			    text->last_anchor->input_field->type));
		return NULL;
	    }

	    new_ptr = text->last_anchor->input_field->select_list =
				typecalloc(OptionType);
	    if (new_ptr == NULL)
		outofmem(__FILE__, "HText_setLastOptionValue");

	    first_option = TRUE;
	} else {
	    while (op_ptr->next) {
		number++;
		op_ptr=op_ptr->next;
	    }
	    number++;  /* add one more */

	    op_ptr->next = new_ptr = typecalloc(OptionType);
	    if (new_ptr == NULL)
		outofmem(__FILE__, "HText_setLastOptionValue");
	}

	new_ptr->name = NULL;
	new_ptr->cp_submit_value = NULL;
	new_ptr->next = NULL;
	/*
	 *  Find first non-space again, convert_to_spaces above may have
	 *  changed the string. - kw
	 */
	cp = value;
	while (isspace(UCH(*cp)) ||
	       IsSpecialAttrChar(UCH(*cp)))
	    cp++;
	for (i = 0, j = 0; cp[i]; i++) {
	    if (cp[i] == HT_NON_BREAK_SPACE ||
		cp[i] == HT_EN_SPACE) {
		cp[j++] = ' ';
	    } else if (cp[i] != LY_SOFT_HYPHEN &&
		       !IsSpecialAttrChar(UCH(cp[i]))) {
		cp[j++] = cp[i];
	    }
	}
	cp[j] = '\0';
	if (HTCJK != NOCJK) {
	    if (cp &&
		(tmp = (unsigned char *)calloc(1, strlen(cp)+1))) {
#ifdef SH_EX
		if (tmp == NULL)
		    outofmem(__FILE__, "HText_setLastOptionValue");
#endif
		if (kanji_code == EUC) {
		    TO_EUC((unsigned char *)cp, tmp);
		    val_cs = current_char_set;
		} else if (kanji_code == SJIS) {
		    TO_SJIS((unsigned char *)cp, tmp);
		    val_cs = current_char_set;
		} else {
		    for (i = 0, j = 0; cp[i]; i++) {
			if (cp[i] != CH_ESC) {	/* S/390 -- gil -- 1604 */
			    tmp[j++] = cp[i];
			}
		    }
		}
		StrAllocCopy(new_ptr->name, (CONST char *)tmp);
		FREE(tmp);
	    }
	} else {
	    StrAllocCopy(new_ptr->name, cp);
	}
	StrAllocCopy(new_ptr->cp_submit_value,
		     (submit_value ? submit_value :
		      HText_skipOptionNumPrefix(new_ptr->name)));
	new_ptr->value_cs = (submit_value ? submit_val_cs : val_cs);

	if (first_option) {
	    StrAllocCopy(HTCurSelectedOptionValue, new_ptr->name);
	    text->last_anchor->input_field->num_value = 0;
	    /*
	     *  If this is the first option in a popup select list,
	     *  HText_beginInput may have allocated the value and
	     *  cp_submit_value fields, so free them now to avoid
	     *  a memory leak. - kw
	     */
	    FREE(text->last_anchor->input_field->value);
	    FREE(text->last_anchor->input_field->cp_submit_value);

	    text->last_anchor->input_field->value =
		text->last_anchor->input_field->select_list->name;
	    text->last_anchor->input_field->orig_value =
		text->last_anchor->input_field->select_list->name;
	    text->last_anchor->input_field->cp_submit_value =
		text->last_anchor->input_field->select_list->cp_submit_value;
	    text->last_anchor->input_field->orig_submit_value =
		text->last_anchor->input_field->select_list->cp_submit_value;
	    text->last_anchor->input_field->value_cs =
		new_ptr->value_cs;
	} else {
	    int newlen = strlen(new_ptr->name);
	    int curlen = strlen(HTCurSelectedOptionValue);
		/*
		 *  Make the selected Option Value as long as
		 *  the longest option.
		 */
	    if (newlen > curlen)
		StrAllocCat(HTCurSelectedOptionValue,
			    UNDERSCORES(newlen-curlen));
	}

	if (checked) {
	    int curlen = strlen(new_ptr->name);
	    int newlen = strlen(HTCurSelectedOptionValue);
	    /*
	     *  Set the default option as this one.
	     */
	    text->last_anchor->input_field->num_value = number;
	    text->last_anchor->input_field->value = new_ptr->name;
	    text->last_anchor->input_field->orig_value = new_ptr->name;
	    text->last_anchor->input_field->cp_submit_value =
				   new_ptr->cp_submit_value;
	    text->last_anchor->input_field->orig_submit_value =
				   new_ptr->cp_submit_value;
	    text->last_anchor->input_field->value_cs =
		new_ptr->value_cs;
	    StrAllocCopy(HTCurSelectedOptionValue, new_ptr->name);
	    if (newlen > curlen)
		StrAllocCat(HTCurSelectedOptionValue,
			    UNDERSCORES(newlen-curlen));
	}

	/*
	 *  Return the selected Option value to be sent to the screen.
	 */
	if (order == LAST_ORDER) {
	    /*
	     *  Change the value.
	     */
	    text->last_anchor->input_field->size =
				strlen(HTCurSelectedOptionValue);
	    ret_Value = HTCurSelectedOptionValue;
	}
    }

    if (TRACE) {
	fprintf(tfp,"HText_setLastOptionValue:%s value=%s",
		(order == LAST_ORDER) ? " LAST_ORDER" : "",
		value);
	fprintf(tfp,"            val_cs=%d \"%s\"",
			val_cs,
			(val_cs >= 0 ?
			 LYCharSet_UC[val_cs].MIMEname : "<UNKNOWN>"));
	if (submit_value) {
	    fprintf(tfp, " (submit_val_cs %d \"%s\") submit_value%s=%s\n",
		    submit_val_cs,
		    (submit_val_cs >= 0 ?
		     LYCharSet_UC[submit_val_cs].MIMEname : "<UNKNOWN>"),
		    (HTCurSelectGroupType == F_CHECKBOX_TYPE) ?
						  "(ignored)" : "",
		    submit_value);
	}
	else {
	    fprintf(tfp,"\n");
	}
    }
    return(ret_Value);
}

/*
 * Count the number of anchors on the current line so we can allow for the
 * length of numbered fields.
 */
PRIVATE int AnchorsOnThisLine ARGS2(
	HText *,	txt,
	TextAnchor *,	ank)
{
    TextAnchor *chk = txt->first_anchor;
    int count = 1;

    while (chk != 0
    	&& chk != txt->last_anchor
	&& chk != ank) {
	if (chk->line_num == ank->line_num)
	    count++;
	chk = chk->next;
    }
    return count;
}

/*
 *  Assign a form input anchor.
 *  Returns the number of characters to leave
 *  blank so that the input field can fit.
 */
PUBLIC int HText_beginInput ARGS3(
	HText *,		text,
	BOOL,			underline,
	InputFieldData *,	I)
{
    TextAnchor * a = typecalloc(TextAnchor);
    FormInfo * f = typecalloc(FormInfo);
    CONST char *cp_option = NULL;
    char *IValue = NULL;
    unsigned char *tmp = NULL;
    int i, j;

    CTRACE((tfp, "GridText: Entering HText_beginInput\n"));

    if (a == NULL || f == NULL)
	outofmem(__FILE__, "HText_beginInput");

    a->start = text->chars + text->last_line->size;
    a->inUnderline = underline;
    a->line_num = text->Lines;
    a->line_pos = text->last_line->size;


    /*
     *  If this is a radio button, or an OPTION we're converting
     *  to a radio button, and it's the first with this name, make
     *  sure it's checked by default.  Otherwise, if it's checked,
     *  uncheck the default or any preceding radio button with this
     *  name that was checked. - FM
     */
    if (I->type != NULL && !strcmp(I->type,"OPTION") &&
	HTCurSelectGroupType == F_RADIO_TYPE && LYSelectPopups == FALSE) {
	I->type = "RADIO";
	I->name = HTCurSelectGroup;
	I->name_cs = HTCurSelectGroupCharset;
    }
    if (I->name && I->type && !strcasecomp(I->type, "radio")) {
	if (!text->last_anchor) {
	    I->checked = TRUE;
	} else {
	    TextAnchor * b = text->first_anchor;
	    int i2 = 0;
	    while (b) {
		if (b->link_type == INPUT_ANCHOR &&
		    b->input_field->type == F_RADIO_TYPE &&
		    b->input_field->number == HTFormNumber) {
		    if (!strcmp(b->input_field->name, I->name)) {
			if (I->checked && b->input_field->num_value) {
			    b->input_field->num_value = 0;
			    StrAllocCopy(b->input_field->orig_value, "0");
			    break;
			}
			i2++;
		    }
		}
		if (b == text->last_anchor) {
		    if (i2 == 0)
		       I->checked = TRUE;
		    break;
		}
		b = b->next;
	    }
	}
    }

    a->next = 0;
    a->anchor = NULL;
    a->link_type = INPUT_ANCHOR;
    a->show_anchor = YES;

    a->hightext = NULL;
    a->extent = 2;

    a->input_field = f;

    f->select_list = 0;
    f->number = HTFormNumber;
    f->disabled = (HTFormDisabled ? TRUE : I->disabled);
    f->no_cache = NO;

    HTFormFields++;


    /*
     *  Set the no_cache flag if the METHOD is POST. - FM
     */
    if (HTFormMethod == URL_POST_METHOD)
	f->no_cache = TRUE;

    /*
     *  Set up VALUE.
     */
    if (I->value)
	StrAllocCopy(IValue, I->value);
    if (IValue && HTCJK != NOCJK) {
	if ((tmp = (unsigned char *)calloc(1, (strlen(IValue) + 1)))) {
#ifdef SH_EX
	    if (tmp == NULL)
		outofmem(__FILE__, "HText_beginInput");
#endif
	    if (kanji_code == EUC) {
		TO_EUC((unsigned char *)IValue, tmp);
		I->value_cs = current_char_set;
	    } else if (kanji_code == SJIS) {
		TO_SJIS((unsigned char *)IValue, tmp);
		I->value_cs = current_char_set;
	    } else {
		for (i = 0, j = 0; IValue[i]; i++) {
		    if (IValue[i] != CH_ESC) {  /* S/390 -- gil -- 1621 */
			tmp[j++] = IValue[i];
		    }
		}
	    }
	    StrAllocCopy(IValue, (CONST char *)tmp);
	    FREE(tmp);
	}
    }

    /*
     *  Special case of OPTION.
     *  Is handled above if radio type and LYSelectPopups is FALSE.
     */
    /* set the values and let the parsing below do the work */
    if (I->type != NULL && !strcmp(I->type,"OPTION")) {
	cp_option = I->type;
	if (HTCurSelectGroupType == F_RADIO_TYPE)
	    I->type = "OPTION_LIST";
	else
	    I->type = "CHECKBOX";
	I->name = HTCurSelectGroup;
	I->name_cs = HTCurSelectGroupCharset;

	/*
	 *  The option's size parameter actually gives the length and not
	 *    the width of the list.  Perform the conversion here
	 *    and get rid of the allocated HTCurSelect....
	 *  0 is ok as it means any length (arbitrary decision).
	 */
	if (HTCurSelectGroupSize != NULL) {
	    f->size_l = atoi(HTCurSelectGroupSize);
	    FREE(HTCurSelectGroupSize);
	}
    }

    /*
     *  Set SIZE.
     */
    if (I->size != NULL) {
	f->size = atoi(I->size);
	/*
	 *  Leave at zero for option lists.
	 */
	if (f->size == 0 && cp_option == NULL) {
	    f->size = 20;  /* default */
	}
    } else {
	f->size = 20;  /* default */
    }

    /*
     *  Set MAXLENGTH.
     */
    if (I->maxlength != NULL) {
	f->maxlength = atoi(I->maxlength);
    } else {
	f->maxlength = 0;  /* 0 means infinite */
    }

    /*
     *  Set CHECKED
     *  (num_value is only relevant to check and radio types).
     */
    if (I->checked == TRUE)
	f->num_value = 1;
    else
	f->num_value = 0;

    /*
     *  Set TYPE.
     */
    if (I->type != NULL) {
	if (!strcasecomp(I->type,"password")) {
	    f->type = F_PASSWORD_TYPE;
	} else if (!strcasecomp(I->type,"checkbox")) {
	    f->type = F_CHECKBOX_TYPE;
	} else if (!strcasecomp(I->type,"radio")) {
	    f->type = F_RADIO_TYPE;
	} else if (!strcasecomp(I->type,"submit")) {
	    f->type = F_SUBMIT_TYPE;
	} else if (!strcasecomp(I->type,"image")) {
	    f->type = F_IMAGE_SUBMIT_TYPE;
	} else if (!strcasecomp(I->type,"reset")) {
	    f->type = F_RESET_TYPE;
	} else if (!strcasecomp(I->type,"OPTION_LIST")) {
	    f->type = F_OPTION_LIST_TYPE;
	} else if (!strcasecomp(I->type,"hidden")) {
	    f->type = F_HIDDEN_TYPE;
	    HTFormFields--;
	    f->size = 0;
	} else if (!strcasecomp(I->type,"textarea")) {
	    f->type = F_TEXTAREA_TYPE;
	} else if (!strcasecomp(I->type,"range")) {
	    f->type = F_RANGE_TYPE;
	} else if (!strcasecomp(I->type,"file")) {
	    f->type = F_FILE_TYPE;
	    CTRACE((tfp, "ok, got a file uploader\n"));
	} else if (!strcasecomp(I->type,"keygen")) {
	    f->type = F_KEYGEN_TYPE;
	} else {
	    /*
	     *  Note that TYPE="scribble" defaults to TYPE="text". - FM
	     */
	    f->type = F_TEXT_TYPE; /* default */
	}
    } else {
	f->type = F_TEXT_TYPE;
    }

    /*
     *  Set NAME.
     */
    if (I->name != NULL) {
	StrAllocCopy(f->name,I->name);
	f->name_cs = I->name_cs;
    } else {
	if (f->type == F_RESET_TYPE ||
	    f->type == F_SUBMIT_TYPE ||
	    f->type == F_IMAGE_SUBMIT_TYPE) {
	    /*
	     *  Set name to empty string.
	     */
	    StrAllocCopy(f->name, "");
	} else {
	    /*
	     *  Error!  NAME must be present.
	     */
	    CTRACE((tfp,
		  "GridText: No name present in input field; not displaying\n"));
	    FREE(a);
	    FREE(f);
	    FREE(IValue);
	    return(0);
	}
    }

    /*
     *  Add this anchor to the anchor list
     */
    if (text->last_anchor) {
	text->last_anchor->next = a;
    } else {
	text->first_anchor = a;
    }

    /*
     *  Set VALUE, if it exists.  Otherwise, if it's not
     *  an option list make it a zero-length string. - FM
     */
    if (IValue != NULL) {
	/*
	 *  OPTION VALUE is not actually the value to be seen but is to
	 *    be sent....
	 */
	if (f->type == F_OPTION_LIST_TYPE ||
	    f->type == F_CHECKBOX_TYPE) {
	    /*
	     *  Fill both with the value.  The f->value may be
	     *  overwritten in HText_setLastOptionValue....
	     */
	    StrAllocCopy(f->value, IValue);
	    StrAllocCopy(f->cp_submit_value, IValue);
	} else {
	    StrAllocCopy(f->value, IValue);
	}
	f->value_cs = I->value_cs;
    } else if (f->type != F_OPTION_LIST_TYPE) {
	StrAllocCopy(f->value, "");
	/*
	 *  May be an empty INPUT field.  The text entered will then
	 *  probably be in the current display character set. - kw
	 */
	f->value_cs = current_char_set;
    }

    /*
     *  Run checks and fill in necessary values.
     */
    if (f->type == F_RESET_TYPE) {
	if (f->value && *f->value != '\0') {
	    f->size = strlen(f->value);
	} else {
	    StrAllocCopy(f->value, "Reset");
	    f->size = 5;
	}
    } else if (f->type == F_IMAGE_SUBMIT_TYPE ||
	       f->type == F_SUBMIT_TYPE) {
	if (f->value && *f->value != '\0') {
	    f->size = strlen(f->value);
	} else if (f->type == F_IMAGE_SUBMIT_TYPE) {
	    StrAllocCopy(f->value, "[IMAGE]-Submit");
	    f->size = 14;
	} else {
	    StrAllocCopy(f->value, "Submit");
	    f->size = 6;
	}
	f->submit_action = NULL;
	StrAllocCopy(f->submit_action, HTFormAction);
	if (HTFormEnctype != NULL)
	    StrAllocCopy(f->submit_enctype, HTFormEnctype);
	if (HTFormTitle != NULL)
	    StrAllocCopy(f->submit_title, HTFormTitle);
	f->submit_method = HTFormMethod;

    } else if (f->type == F_RADIO_TYPE || f->type == F_CHECKBOX_TYPE) {
	f->size=3;
	if (IValue == NULL)
	    StrAllocCopy(f->value, (f->type == F_CHECKBOX_TYPE ? "on" : ""));

    }
    FREE(IValue);

    /*
     *  Set original values.
     */
    if (f->type == F_RADIO_TYPE || f->type == F_CHECKBOX_TYPE ) {
	if (f->num_value)
	    StrAllocCopy(f->orig_value, "1");
	else
	    StrAllocCopy(f->orig_value, "0");
    } else if (f->type == F_OPTION_LIST_TYPE) {
	f->orig_value = NULL;
    } else {
	StrAllocCopy(f->orig_value, f->value);
    }

    /*
     *  Store accept-charset if present, converting to lowercase
     *  and collapsing spaces. - kw
     */
    if (I->accept_cs) {
	StrAllocCopy(f->accept_cs, I->accept_cs);
	LYRemoveBlanks(f->accept_cs);
	LYLowerCase(f->accept_cs);
    }

    /*
     *  Add numbers to form fields if needed. - LE & FM
     */
    switch (f->type) {
	/*
	 *  Do not supply number for hidden fields, nor
	 *  for types that are not yet implemented.
	 */
	case F_HIDDEN_TYPE:
#ifndef EXP_FILE_UPLOAD
	case F_FILE_TYPE:
#endif
	case F_RANGE_TYPE:
	case F_KEYGEN_TYPE:
	    a->number = 0;
	    break;

	default:
	    if (keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED)
		a->number = ++(text->last_anchor_number);
	    else
		a->number = 0;
	    break;
    }
    if (keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED && a->number > 0) {
	char marker[16];

	if (f->type != F_OPTION_LIST_TYPE)
	    /*
	     *  '[' was already put out for a popup menu
	     *  designator.  See HTML.c.
	     */
	    HText_appendCharacter(text, '[');
	sprintf(marker,"%d]", a->number);
	HText_appendText(text, marker);
	if (f->type == F_OPTION_LIST_TYPE)
	    /*
	     *  Add option list designation char.
	     */
	    HText_appendCharacter(text, '[');
	a->start = text->chars + text->last_line->size;
	a->line_num = text->Lines;
	a->line_pos = text->last_line->size;
    }

    /*
     *  Restrict SIZE to maximum allowable size.
     */
    switch (f->type) {
	int MaximumSize;

	case F_SUBMIT_TYPE:
	case F_IMAGE_SUBMIT_TYPE:
	case F_RESET_TYPE:
	case F_TEXT_TYPE:
	case F_TEXTAREA_TYPE:
	    /*
	     *  For submit and reset buttons, and for text entry
	     *  fields and areas, we limit the size element to that
	     *  of one line for the current style because that's
	     *  the most we could highlight on overwrites, and/or
	     *  handle in the line editor.  The actual values for
	     *  text entry lines can be long, and will be scrolled
	     *  horizontally within the editing window. - FM
	     */
	    MaximumSize = (LYcols - 1) -
			  (int)text->style->leftIndent -
			  (int)text->style->rightIndent;

	    /*
	     *  If we are numbering form links, take that into
	     *  account as well. - FM
	     */
	    if (keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED)
		MaximumSize -= AnchorsOnThisLine(text, a) / 10 + 3;
	    if (f->size > MaximumSize)
		f->size = MaximumSize;

	    /*
	     *  Save value for submit/reset buttons so they
	     *  will be visible when printing the page. - LE
	     */
	    I->value = f->value;
	    break;


	default:
	    /*
	     *  For all other fields we limit the size element to
	     *  10 less than the screen width, because either they
	     *  are types with small placeholders, and/or are a
	     *  type which is handled via a popup window. - FM
	     */
	    if (f->size > LYcols-10)
		f->size = LYcols-10;  /* maximum */
	    break;
    }

    /*
     *  Add this anchor to the anchor list
     */
    text->last_anchor = a;

    if (HTCurrentForm) {	/* should always apply! - kw */
	if (!HTCurrentForm->first_field) {
	    HTCurrentForm->first_field = f;
	}
	HTCurrentForm->last_field = f;
	HTCurrentForm->nfields++; /* will count hidden fields - kw */
	/*
	 *  Propagate form field's accept-charset attribute to enclosing
	 *  form if the form itself didn't have an accept-charset - kw
	 */
	if (f->accept_cs && !HTFormAcceptCharset) {
	    StrAllocCopy(HTFormAcceptCharset, f->accept_cs);
	}
	if (!text->forms) {
	    text->forms = HTList_new();
	}
    } else {
	CTRACE((tfp, "beginInput: HTCurrentForm is missing!\n"));
    }

    CTRACE((tfp, "Input link: name=%s\nvalue=%s\nsize=%d\n",
			f->name,
			((f->value != NULL) ? f->value : ""),
			f->size));
    CTRACE((tfp, "Input link: name_cs=%d \"%s\" (from %d \"%s\")\n",
			f->name_cs,
			(f->name_cs >= 0 ?
			 LYCharSet_UC[f->name_cs].MIMEname : "<UNKNOWN>"),
			I->name_cs,
			(I->name_cs >= 0 ?
			 LYCharSet_UC[I->name_cs].MIMEname : "<UNKNOWN>")));
    CTRACE((tfp, "            value_cs=%d \"%s\" (from %d \"%s\")\n",
			f->value_cs,
			(f->value_cs >= 0 ?
			 LYCharSet_UC[f->value_cs].MIMEname : "<UNKNOWN>"),
			I->value_cs,
			(I->value_cs >= 0 ?
			 LYCharSet_UC[I->value_cs].MIMEname : "<UNKNOWN>")));

    /*
     *  Return the SIZE of the input field.
     */
    return(f->size);
}

/*
 *  Get a translation (properly: transcoding) quality, factoring in
 *  our ability to translate (an UCTQ_t) and a possible q parameter
 *  on the given charset string, for cs_from -> givenmime.
 *  The parsed input string will be mutilated on exit(!).
 *  Note that results are not normalised to 1.0, but results from
 *  different calls of this function can be compared. - kw
 *
 *  Obsolete, it was planned to use here a quality parametr UCTQ_t,
 *  which is boolean now.
 */
PRIVATE double get_trans_q ARGS2(
    int,		cs_from,
    char *,		givenmime)
{
    double df = 1.0;
    BOOL tq;
    char *p;
    if (!givenmime || !(*givenmime))
	return 0.0;
    if ((p = strchr(givenmime,';')) != NULL) {
	*p++ = '\0';
    }
    if (!strcmp(givenmime, "*"))
	tq = UCCanTranslateFromTo(cs_from,
				  UCGetLYhndl_byMIME("utf-8"));
    else
	tq = UCCanTranslateFromTo(cs_from,
				  UCGetLYhndl_byMIME(givenmime));
    if (!tq)
	return 0.0;
    if (p && *p) {
	char *pair, *field = p, *pval, *ptok;
	/* Get all the parameters to the Charset */
	while ((pair = HTNextTok(&field, ";", "\"", NULL)) != NULL) {
	    if ((ptok = HTNextTok(&pair, "= ", NULL, NULL)) != NULL &&
		(pval = HTNextField(&pair)) != NULL) {
		if (0==strcasecomp(ptok,"q")) {
		    df = strtod(pval, NULL);
		    break;
		}
	    }
	}
	return (df * tq);
    } else {
	return tq;
    }
}

/*
 *  Find the best charset for submission, if we have an ACCEPT_CHARSET
 *  list.  It factors in how well we can translate (just as guess, and
 *  not a very good one..) and possible  ";q=" factors.  Yes this is
 *  more general than it needs to be here.
 *
 *  Input is cs_in and acceptstring.
 *
 *  Will return charset handle as int.
 *  best_csname will point to a newly allocated MIME string for the
 *  charset corresponding to the return value if return value >= 0.
 *  - kw
 */
PRIVATE int find_best_target_cs ARGS3(
    char **,		best_csname,
    int,		cs_from,
    CONST char *,	acceptstring)
{
    char *paccept = NULL;
    double bestq = -1.0;
    char *bestmime = NULL;
    char *field, *nextfield;
    StrAllocCopy(paccept, acceptstring);
    nextfield = paccept;
    while ((field = HTNextTok(&nextfield, ",", "\"", NULL)) != NULL) {
	double q;
	if (*field != '\0') {
	    /* Get the Charset*/
	    q = get_trans_q(cs_from, field);
	    if (q > bestq) {
		bestq = q;
		bestmime = field;
	    }
	}
    }
    if (bestmime) {
	if (!strcmp(bestmime, "*")) /* non-standard for HTML attribute.. */
	    StrAllocCopy(*best_csname, "utf-8");
	else
	    StrAllocCopy(*best_csname, bestmime);
	FREE(paccept);
	if (bestq > 0)
	    return (UCGetLYhndl_byMIME(*best_csname));
	else
	    return (-1);
    }
    FREE(paccept);
    return (-1);
}

/*
 *  HText_SubmitForm - generate submit data from form fields.
 *  For mailto forms, send the data.
 *  For other methods, set fields in structure pointed to by doc
 *  appropriately for next request.
 *  Returns 1 if *doc set appropriately for next request,
 *	    0 otherwise. - kw
 */
PUBLIC int HText_SubmitForm ARGS4(
	FormInfo *,	submit_item,
	document *,	doc,
	char *,		link_name,
	char *,		link_value)
{
    TextAnchor *anchor_ptr;
    int form_number = submit_item->number;
    FormInfo *form_ptr;
    PerFormInfo *thisform;
    char *query = NULL;
    char *escaped1 = NULL, *escaped2 = NULL;
    int first_one = 1;
    char *last_textarea_name = NULL;
    int textarea_lineno = 0;
    char *previous_blanks = NULL;
    BOOLEAN PlainText = FALSE;
    BOOLEAN SemiColon = FALSE;
    char *Boundary = NULL;
    char *MultipartContentType = NULL;
    char *content_type_out = NULL;
    int target_cs = -1;
    CONST char *out_csname;
    CONST char *target_csname = NULL;
    char *name_used = "";
    BOOL form_has_8bit = NO, form_has_special = NO;
    BOOL field_has_8bit = NO, field_has_special = NO;
    BOOL name_has_8bit = NO, name_has_special = NO;
    BOOL have_accept_cs = NO;
    BOOL success;
    BOOL had_chartrans_warning = NO;
    char *val_used = "";
    char *copied_val_used = NULL;
    char *copied_name_used = NULL;

    CTRACE((tfp, "FIXME:SubmitForm\n"));
    if (!HTMainText)
	return 0;

    thisform = HTList_objectAt(HTMainText->forms, form_number - 1);
    /*  Sanity check */
    if (!thisform) {
	CTRACE((tfp, "SubmitForm: form %d not in HTMainText's list!\n",
		    form_number));
    } else if (thisform->number != form_number) {
	CTRACE((tfp, "SubmitForm: failed sanity check, %d!=%d !\n",
		    thisform->number, form_number));
	thisform = NULL;
    }

    if (submit_item->submit_action) {
	/*
	 *  If we're mailing, make sure it's a mailto ACTION. - FM
	 */
	if ((submit_item->submit_method == URL_MAIL_METHOD) &&
	    strncmp(submit_item->submit_action, "mailto:", 7)) {
	    HTAlert(BAD_FORM_MAILTO);
	    return 0;
	}
    } else {
	return 0;
    }

    /*
     *  Check the ENCTYPE and set up the appropriate variables. - FM
     */
    if (submit_item->submit_enctype &&
	!strncasecomp(submit_item->submit_enctype, "text/plain", 10)) {
	/*
	 *  Do not hex escape, and use physical newlines
	 *  to separate name=value pairs. - FM
	 */
	PlainText = TRUE;
    } else if (submit_item->submit_enctype &&
	       !strncasecomp(submit_item->submit_enctype,
			     "application/sgml-form-urlencoded", 32)) {
	/*
	 *  Use semicolons instead of ampersands as the
	 *  separators for name=value pairs. - FM
	 */
	SemiColon = TRUE;
    } else if (submit_item->submit_enctype &&
	       !strncasecomp(submit_item->submit_enctype,
			     "multipart/form-data", 19)) {
	/*
	 *  Use the multipart MIME format.  We should generate
	 *  a boundary string which we are sure doesn't occur
	 *  in the content, but for now we'll just assume that
	 *  this string doesn't. - FM
	 */
	Boundary = "xnyLAaB03X";
    }

    /*
     *  Determine in what character encoding (aka. charset) we should
     *  submit.  We call this target_cs and the MIME name for it
     *  target_csname.
     *  TODO:   - actually use ACCEPT-CHARSET stuff from FORM
     *  TODO:   - deal with list in ACCEPT-CHARSET, find a "best"
     *		  charset to submit
     */

    /* Look at ACCEPT-CHARSET on the submitting field if we have one. */
    if (thisform && submit_item->accept_cs &&
	strcasecomp(submit_item->accept_cs, "UNKNOWN")) {
	have_accept_cs = YES;
	target_cs = find_best_target_cs(&thisform->thisacceptcs,
					current_char_set,
					submit_item->accept_cs);
    }
    /* Look at ACCEPT-CHARSET on form as a whole if submitting field
     * didn't have one. */
    if (thisform && !have_accept_cs && thisform->accept_cs &&
	strcasecomp(thisform->accept_cs, "UNKNOWN")) {
	have_accept_cs = YES;
	target_cs = find_best_target_cs(&thisform->thisacceptcs,
					current_char_set,
					thisform->accept_cs);
    }
    if (have_accept_cs && (target_cs >= 0) && thisform->thisacceptcs) {
	target_csname = thisform->thisacceptcs;
    }

    if (target_cs < 0 &&
	HTMainText->node_anchor->charset &&
	*HTMainText->node_anchor->charset) {
	target_cs = UCGetLYhndl_byMIME(HTMainText->node_anchor->charset);
	if (target_cs >= 0) {
	    target_csname = HTMainText->node_anchor->charset;
	} else {
	    target_cs = UCLYhndl_for_unspec; /* always >= 0 */
	    target_csname = LYCharSet_UC[target_cs].MIMEname;
	}
    }
    if (target_cs < 0) {
	target_cs = UCLYhndl_for_unspec;  /* always >= 0 */
    }

    /*
     *  Go through list of anchors and get size first.
     */
    /*
     *  also get a "max." charset parameter - kw
     */
    anchor_ptr = HTMainText->first_anchor;
    while (anchor_ptr) {
	if (anchor_ptr->link_type == INPUT_ANCHOR) {
	    if (anchor_ptr->input_field->number == form_number &&
			!anchor_ptr->input_field->disabled) {

		char *p;
		char * val;
		form_ptr = anchor_ptr->input_field;
		val = form_ptr->cp_submit_value != NULL ?
				    form_ptr->cp_submit_value : form_ptr->value;
		field_has_8bit = NO;
		field_has_special = NO;

		for (p = val;
		     p && *p && !(field_has_8bit && field_has_special);
		     p++)
		    if ((*p == HT_NON_BREAK_SPACE) ||
		       (*p == HT_EN_SPACE) ||
			(*p == LY_SOFT_HYPHEN)) {
			field_has_special = YES;
		    } else if ((*p & 0x80) != 0) {
			field_has_8bit = YES;
		    }
		for (p = form_ptr->name;
		     p && *p && !(name_has_8bit && name_has_special);
		     p++)
		    if ((*p == HT_NON_BREAK_SPACE) ||
			(*p == HT_EN_SPACE) ||
			(*p == LY_SOFT_HYPHEN)) {
			name_has_special = YES;
		    } else if ((*p & 0x80) != 0) {
			name_has_8bit = YES;
		    }

		if (field_has_8bit || name_has_8bit)
		    form_has_8bit = YES;
		if (field_has_special || name_has_special)
		    form_has_special = YES;

		if (!field_has_8bit && !field_has_special) {
		    /* already ok */
		} else if (target_cs < 0) {
		    /* already confused */
		} else if (!field_has_8bit &&
		    (LYCharSet_UC[target_cs].enc == UCT_ENC_8859 ||
		     (LYCharSet_UC[target_cs].like8859 & UCT_R_8859SPECL))) {
		    /* those specials will be trivial */
		} else if (UCNeedNotTranslate(form_ptr->value_cs, target_cs)) {
		    /* already ok */
		} else if (UCCanTranslateFromTo(form_ptr->value_cs, target_cs)) {
		    /* also ok */
		} else if (UCCanTranslateFromTo(target_cs, form_ptr->value_cs)) {
		    target_cs = form_ptr->value_cs;	/* try this */
		    target_csname = NULL; /* will be set after loop */
		} else {
		    target_cs = -1; /* don't know what to do */
		}

		/*  Same for name */
		if (!name_has_8bit && !name_has_special) {
		    /* already ok */
		} else if (target_cs < 0) {
		    /* already confused */
		} else if (!name_has_8bit &&
		    (LYCharSet_UC[target_cs].enc == UCT_ENC_8859 ||
		     (LYCharSet_UC[target_cs].like8859 & UCT_R_8859SPECL))) {
		    /* those specials will be trivial */
		} else if (UCNeedNotTranslate(form_ptr->name_cs, target_cs)) {
		    /* already ok */
		} else if (UCCanTranslateFromTo(form_ptr->name_cs, target_cs)) {
		    /* also ok */
		} else if (UCCanTranslateFromTo(target_cs, form_ptr->name_cs)) {
		    target_cs = form_ptr->value_cs;	/* try this */
		    target_csname = NULL; /* will be set after loop */
		} else {
		    target_cs = -1; /* don't know what to do */
		}

	    } else if (anchor_ptr->input_field->number > form_number) {
		break;
	    }
	}

	if (anchor_ptr == HTMainText->last_anchor)
	    break;

	anchor_ptr = anchor_ptr->next;
    }

    if (target_csname == NULL && target_cs >= 0) {
	if (form_has_8bit) {
	    target_csname = LYCharSet_UC[target_cs].MIMEname;
	} else if (form_has_special) {
	    target_csname = LYCharSet_UC[target_cs].MIMEname;
	} else {
	    target_csname = "us-ascii";
	}
    }

    if (submit_item->submit_method == URL_GET_METHOD && Boundary == NULL) {
	StrAllocCopy(query, submit_item->submit_action);
	/*
	 *  Method is GET.  Clip out any anchor in the current URL.
	 */
	strtok (query, "#");
	/*
	 *  Clip out any old query in the current URL.
	 */
	strtok (query, "?");
	/*
	 *  Add the lead question mark for the new URL.
	 */
	StrAllocCat(query,"?");
    } else {
	/*
	 *  We are submitting POST content to a server,
	 *  so load content_type_out.  This will be put in
	 *  the post_content_type element if all goes well. - FM, kw
	 */
	if (SemiColon == TRUE) {
	    StrAllocCopy(content_type_out,
			 "application/sgml-form-urlencoded");
	} else if (PlainText == TRUE) {
	    StrAllocCopy(content_type_out,
			 "text/plain");
	} else if (Boundary != NULL) {
	    StrAllocCopy(content_type_out,
			 "multipart/form-data; boundary=");
	    StrAllocCat(content_type_out, Boundary);
	} else {
	    StrAllocCopy(content_type_out,
			 "application/x-www-form-urlencoded");
	}

	/*
	 *  If the ENCTYPE is not multipart/form-data, append the
	 *  charset we'll be sending to the post_content_type, IF
	 *  (1) there was an explicit accept-charset attribute, OR
	 *  (2) we have 8-bit or special chars, AND the document had
	 *      an explicit (recognized and accepted) charset parameter,
	 *	AND it or target_csname is different from iso-8859-1,
	 *      OR
	 *  (3) we have 8-bit or special chars, AND the document had
	 *      no explicit (recognized and accepted) charset parameter,
	 *	AND target_cs is different from the currently effective
	 *	assumed charset (which should have been set by the user
	 *	so that it reflects what the server is sending, if the
	 *	document is rendered correctly).
	 *  For multipart/form-data the equivalent will be done later,
	 *  separately for each form field. - kw
	 */
	if (have_accept_cs ||
	    (form_has_8bit || form_has_special)) {
	    if (target_cs >= 0 && target_csname) {
		if (Boundary == NULL) {
		    if ((HTMainText->node_anchor->charset &&
			 (strcmp(HTMainText->node_anchor->charset,
				 "iso-8859-1") ||
			  strcmp(target_csname, "iso-8859-1"))) ||
			(!HTMainText->node_anchor->charset &&
			 target_cs != UCLYhndl_for_unspec)) {
			StrAllocCat(content_type_out, "; charset=");
			StrAllocCat(content_type_out, target_csname);
		    }
		}
	    } else {
		had_chartrans_warning = YES;
		_user_message(
		    CANNOT_TRANSCODE_FORM,
		    target_csname ? target_csname : "UNKNOWN");
		LYSleepAlert();
	    }
	}
    }


    out_csname = target_csname;

    /*
     *  Reset anchor->ptr.
     */
    anchor_ptr = HTMainText->first_anchor;
    /*
     *  Go through list of anchors and assemble URL query.
     */
    while (anchor_ptr) {
	if (anchor_ptr->link_type == INPUT_ANCHOR) {
	    if (anchor_ptr->input_field->number == form_number &&
			!anchor_ptr->input_field->disabled) {
		char *p;
		int out_cs;
		form_ptr = anchor_ptr->input_field;

		if (form_ptr->type != F_TEXTAREA_TYPE)
		    textarea_lineno = 0;

		switch(form_ptr->type) {
		case F_RESET_TYPE:
		    break;
		case F_SUBMIT_TYPE:
		case F_TEXT_SUBMIT_TYPE:
		case F_IMAGE_SUBMIT_TYPE:
		    if (!(form_ptr->name && *form_ptr->name != '\0' &&
			  !strcmp(form_ptr->name, link_name))) {
			CTRACE((tfp,
				    "SubmitForm: skipping submit field with "));
			CTRACE((tfp, "name \"%s\" for link_name \"%s\", %s.\n",
				    form_ptr->name ? form_ptr->name : "???",
				    link_name ? link_name : "???",
				    (form_ptr->name && *form_ptr->name) ?
				    "not current link" : "no field name"));
			break;
		    }
		    if (!(form_ptr->type == F_TEXT_SUBMIT_TYPE ||
			(form_ptr->value && *form_ptr->value != '\0' &&
			 !strcmp(form_ptr->value, link_value)))) {
			CTRACE((tfp,
				"SubmitForm: skipping submit field with "));
			CTRACE((tfp,
				"name \"%s\" for link_name \"%s\", %s!\n",
				form_ptr->name ? form_ptr->name : "???",
				link_name ? link_name : "???",
				"values are different"));
			break;
		    }
		    /* FALLTHRU */

#ifdef EXP_FILE_UPLOAD
		case F_FILE_TYPE:
		    CTRACE((tfp, "I'd submit %s (from %s), but you've not finished it\n", form_ptr->value, form_ptr->name));
		    name_used = (form_ptr->name ? form_ptr->name : "");
		    val_used = (form_ptr->value ? form_ptr->value : "");
		    break;
#endif

		    /*  fall through  */
		case F_RADIO_TYPE:
		case F_CHECKBOX_TYPE:
		case F_TEXTAREA_TYPE:
		case F_PASSWORD_TYPE:
		case F_TEXT_TYPE:
		case F_OPTION_LIST_TYPE:
		case F_HIDDEN_TYPE:
		    /*
		     *	Be sure to actually look at the option submit value.
		     */
		    if (form_ptr->cp_submit_value != NULL) {
			val_used = form_ptr->cp_submit_value;
		    } else {
			val_used = form_ptr->value;
		    }

		    /*
		     *  Charset-translate value now, because we need
		     *  to know the charset parameter for multipart
		     *  bodyparts. - kw
		     */
		    field_has_8bit = NO;
		    field_has_special = NO;
		    for (p = val_used;
			 p && *p && !(field_has_8bit && field_has_special);
			 p++) {
			if ((*p == HT_NON_BREAK_SPACE) ||
			    (*p == HT_EN_SPACE) ||
			    (*p == LY_SOFT_HYPHEN)) {
			    field_has_special = YES;
			} else if ((*p & 0x80) != 0) {
			    field_has_8bit = YES;
			}
		    }

		    if (field_has_8bit || field_has_special) {
			/*  We should translate back. */
			StrAllocCopy(copied_val_used, val_used);
			success = LYUCTranslateBackFormData(&copied_val_used,
							form_ptr->value_cs,
							target_cs, PlainText);
			CTRACE((tfp, "SubmitForm: field \"%s\" %d %s -> %d %s %s\n",
				    form_ptr->name ? form_ptr->name : "",
				    form_ptr->value_cs,
				    form_ptr->value_cs >= 0 ?
				    LYCharSet_UC[form_ptr->value_cs].MIMEname :
									  "???",
				    target_cs,
				    target_csname ? target_csname : "???",
				    success ? "OK" : "FAILED"));
			if (success) {
			    val_used = copied_val_used;
			}
		    } else {  /* We can use the value directly. */
			CTRACE((tfp, "SubmitForm: field \"%s\" %d %s OK\n",
				    form_ptr->name ? form_ptr->name : "",
				    target_cs,
				    target_csname ? target_csname : "???"));
			success = YES;
		    }
		    if (!success) {
			if (!had_chartrans_warning) {
			    had_chartrans_warning = YES;
			    _user_message(
				CANNOT_TRANSCODE_FORM,
				target_csname ? target_csname : "UNKNOWN");
			    LYSleepAlert();
			}
			out_cs = form_ptr->value_cs;
		    } else {
			out_cs = target_cs;
		    }
		    if (out_cs >= 0)
			out_csname = LYCharSet_UC[out_cs].MIMEname;
		    if (Boundary) {
			if (!success && form_ptr->value_cs < 0) {
			    /*  This is weird. */
			    StrAllocCopy(MultipartContentType,
					 "\r\nContent-Type: text/plain; charset=");
			    StrAllocCat(MultipartContentType, "UNKNOWN-8BIT");
			} else if (!success) {
			    StrAllocCopy(MultipartContentType,
					 "\r\nContent-Type: text/plain; charset=");
			    StrAllocCat(MultipartContentType, out_csname);
			    target_csname = NULL;
			} else {
			    if (!target_csname) {
				target_csname = LYCharSet_UC[target_cs].MIMEname;
			    }
			    StrAllocCopy(MultipartContentType,
					 "\r\nContent-Type: text/plain; charset=");
			    StrAllocCat(MultipartContentType, out_csname);
			}
		    }

		    /*
		     *  Charset-translate name now, because we need
		     *  to know the charset parameter for multipart
		     *  bodyparts. - kw
		     */
		    if (form_ptr->type == F_TEXTAREA_TYPE) {
			textarea_lineno++;
			if (textarea_lineno > 1 &&
			    last_textarea_name && form_ptr->name &&
			    !strcmp(last_textarea_name, form_ptr->name)) {
			    break;
			}
		    }
		    name_used = (form_ptr->name ?
				 form_ptr->name : "");

		    name_has_8bit = NO;
		    name_has_special = NO;
		    for (p = name_used;
			 p && *p && !(name_has_8bit && name_has_special);
			 p++) {
			if ((*p == HT_NON_BREAK_SPACE) ||
			    (*p == HT_EN_SPACE) ||
			    (*p == LY_SOFT_HYPHEN)) {
			    name_has_special = YES;
			} else if ((*p & 0x80) != 0) {
			    name_has_8bit = YES;
			}
		    }

		    if (name_has_8bit || name_has_special) {
			/*  We should translate back. */
			StrAllocCopy(copied_name_used, name_used);
			success = LYUCTranslateBackFormData(&copied_name_used,
							form_ptr->name_cs,
							target_cs, PlainText);
			CTRACE((tfp, "SubmitForm: name \"%s\" %d %s -> %d %s %s\n",
				    form_ptr->name ? form_ptr->name : "",
				    form_ptr->name_cs,
				    form_ptr->name_cs >= 0 ?
				    LYCharSet_UC[form_ptr->name_cs].MIMEname :
									  "???",
				    target_cs,
				    target_csname ? target_csname : "???",
				    success ? "OK" : "FAILED"));
			if (success) {
			    name_used = copied_name_used;
			}
			if (Boundary) {
			    if (!success) {
				StrAllocCopy(MultipartContentType, "");
				target_csname = NULL;
			    } else {
				if (!target_csname)
				    target_csname = LYCharSet_UC[target_cs].MIMEname;
			    }
			}
		    } else {  /* We can use the name directly. */
			CTRACE((tfp, "SubmitForm: name \"%s\" %d %s OK\n",
				    form_ptr->name ? form_ptr->name : "",
				    target_cs,
				    target_csname ? target_csname : "???"));
			success = YES;
			if (Boundary) {
			    StrAllocCopy(copied_name_used, name_used);
			}
		    }
		    if (!success) {
			if (!had_chartrans_warning) {
			    had_chartrans_warning = YES;
			    _user_message(
				CANNOT_TRANSCODE_FORM,
				target_csname ? target_csname : "UNKNOWN");
			    LYSleepAlert();
			}
		    }
		    if (Boundary) {
			/*
			 *  According to RFC 1867, Non-ASCII field names
			 *  "should be encoded according to the prescriptions
			 *  of RFC 1522 [...].  I don't think RFC 1522 actually
			 *  is meant to apply to parameters like this, and it
			 *  is unknown whether any server would make sense of
			 *  it, so for now just use some quoting/escaping and
			 *  otherwise leave 8-bit values as they are.
			 *  Non-ASCII characters in form field names submitted
			 *  as multipart/form-data can only occur if the form
			 *  provider specifically asked for it anyway. - kw
			 */
			HTMake822Word(&copied_name_used);
			name_used = copied_name_used;
		    }

		    break;
		default:
		    CTRACE((tfp, "SubmitForm: What type is %d?\n",
				form_ptr->type));
		}

		switch(form_ptr->type) {

		case F_RESET_TYPE:
		    break;

#ifdef EXP_FILE_UPLOAD
		case F_FILE_TYPE:
		{
		    int cdisp_name_startpos = 0;
		    FILE *fd;
		    int bytes;
		    char buffer[257];

		    CTRACE((tfp, "Ok, about to convert %s to mime/thingy\n", form_ptr->value));
		    if (first_one) {
			if (Boundary) {
			    HTSprintf(&query, "--%s\r\n", Boundary);
			}
			first_one = FALSE;
		    } else {
			if (PlainText) {
			    HTSprintf(&query, "\n");
			} else if (SemiColon) {
			    HTSprintf(&query, ";");
			} else if (Boundary) {
			    HTSprintf(&query, "\r\n--%s\r\n", Boundary);
			} else {
			    HTSprintf(&query, "&");
			}
		    }

		    if (PlainText) {
			StrAllocCopy(escaped1, name_used);
		    } else if (Boundary) {
			StrAllocCopy(escaped1,
				"Content-Disposition: form-data; name=");
			cdisp_name_startpos = strlen(escaped1);
			StrAllocCat(escaped1, name_used);
			StrAllocCat(escaped1, "; filename=\"");
			StrAllocCat(escaped1, val_used);
			StrAllocCat(escaped1, "\"");
			if (MultipartContentType) {
			    StrAllocCat(escaped1, MultipartContentType);
			    StrAllocCat(escaped1, "\r\nContent-Transfer-Encoding: base64");
			}
			StrAllocCat(escaped1, "\r\n\r\n");
		    } else {
			escaped1 = HTEscapeSP(name_used, URL_XALPHAS);
		    }

		    if ((fd = fopen(val_used, "rb")) == 0) {
			/* We can't open the file, what do we do? */
			HTAlert("Can't open file for uploading");
			goto exit_disgracefully;
		    }
		    StrAllocCopy(escaped2, "");
		    while ((bytes = fread(buffer, sizeof(char), 45, fd)) != 0) {
			char base64buf[128];
			base64_encode(base64buf, buffer, bytes);
			StrAllocCat(escaped2, base64buf);
		    }
		    if (ferror(fd)) {
			/* We got an error reading the file, what do we do? */
			HTAlert("Short read from file, problem?");
			LYCloseInput(fd);
			goto exit_disgracefully;
		    }
		    LYCloseInput(fd);
		    /* we need to modify the mime-type here - rp */
		    /* Note: could use LYGetFileInfo for that and for
		       other headers that should be transmitted - kw */

		    HTSprintf(&query,
				   "%s%s%s%s%s",
				   escaped1,
				   (Boundary ? "" : "="),
				   (PlainText ? "\n" : ""),
					escaped2,
				   ((PlainText && *escaped2) ?
							 "\n" : ""));
		    FREE(escaped1);
		    FREE(escaped2);
		}
		break;
#endif /* EXP_FILE_UPLOAD */

		case F_SUBMIT_TYPE:
		case F_TEXT_SUBMIT_TYPE:
		case F_IMAGE_SUBMIT_TYPE:
		    /*
		     *  If it has a non-zero length name (e.g., because
		     *  it's IMAGE_SUBMIT_TYPE to be handled homologously
		     *  to an image map, or a SUBMIT_TYPE in a set of
		     *  multiple submit buttons, or a single type="text"
		     *  that's been converted to a TEXT_SUBMIT_TYPE),
		     *  include the name=value pair, or fake name.x=0 and
		     *  name.y=0 pairs for IMAGE_SUBMIT_TYPE. - FM
		     */
		    if ((form_ptr->name && *form_ptr->name != '\0' &&
			!strcmp(form_ptr->name, link_name)) &&
		       (form_ptr->type == F_TEXT_SUBMIT_TYPE ||
			(form_ptr->value && *form_ptr->value != '\0' &&
			 !strcmp(form_ptr->value, link_value)))) {
			int cdisp_name_startpos = 0;
			if (first_one) {
			    if (Boundary) {
				HTSprintf(&query, "--%s\r\n", Boundary);
			    }
			    first_one=FALSE;
			} else {
			    if (PlainText) {
				StrAllocCat(query, "\n");
			    } else if (SemiColon) {
				StrAllocCat(query, ";");
			    } else if (Boundary) {
				HTSprintf(&query, "\r\n--%s\r\n", Boundary);
			    } else {
				StrAllocCat(query, "&");
			    }
			}

			if (PlainText) {
			    StrAllocCopy(escaped1, name_used);
			} else if (Boundary) {
			    StrAllocCopy(escaped1,
				    "Content-Disposition: form-data; name=");
			    cdisp_name_startpos = strlen(escaped1);
			    StrAllocCat(escaped1, name_used);
			    if (MultipartContentType)
				StrAllocCat(escaped1, MultipartContentType);
			    StrAllocCat(escaped1, "\r\n\r\n");
			} else {
			    escaped1 = HTEscapeSP(name_used, URL_XALPHAS);
			}

			if (PlainText || Boundary) {
			    StrAllocCopy(escaped2,
					 (val_used ?
					  val_used : ""));
			} else {
			    escaped2 = HTEscapeSP(val_used, URL_XALPHAS);
			}

			if (form_ptr->type == F_IMAGE_SUBMIT_TYPE) {
			    /*
			     *  It's a clickable image submit button.
			     *  Fake a 0,0 coordinate pair, which
			     *  typically returns the image's default. - FM
			     */
			    if (Boundary) {
				escaped1[cdisp_name_startpos] = '\0';
				HTSprintf(&query,
					"%s.x\r\n\r\n0\r\n--%s\r\n%s.y\r\n\r\n0",
					escaped1,
					Boundary,
					escaped1);
			    } else {
				HTSprintf(&query,
					"%s.x=0%s%s.y=0%s",
					escaped1,
					(PlainText ?
					      "\n" : (SemiColon ?
							    ";" : "&")),
					escaped1,
					((PlainText && *escaped1) ?
							     "\n" : ""));
			    }
			} else {
			    /*
			     *  It's a standard submit button.
			     *  Use the name=value pair. = FM
			     */
			    HTSprintf(&query,
				    "%s%s%s%s%s",
				    escaped1,
				    (Boundary ?
					   "" : "="),
				    (PlainText ?
					  "\n" : ""),
				    escaped2,
				    ((PlainText && *escaped2) ?
							 "\n" : ""));
			}
			FREE(escaped1);
			FREE(escaped2);
		    }
		    FREE(copied_name_used);
		    FREE(copied_val_used);
		    break;

		case F_RADIO_TYPE:
		case F_CHECKBOX_TYPE:
		    /*
		     *  Only add if selected.
		     */
		    if (form_ptr->num_value) {
			if (first_one) {
			    if (Boundary) {
				HTSprintf(&query,
					"--%s\r\n", Boundary);
			    }
			    first_one=FALSE;
			} else {
			    if (PlainText) {
				StrAllocCat(query, "\n");
			    } else if (SemiColon) {
				StrAllocCat(query, ";");
			    } else if (Boundary) {
				HTSprintf(&query, "\r\n--%s\r\n", Boundary);
			    } else {
				StrAllocCat(query, "&");
			    }
			}

			if (PlainText) {
			    StrAllocCopy(escaped1, name_used);
			} else if (Boundary) {
			    StrAllocCopy(escaped1,
				     "Content-Disposition: form-data; name=");
			    StrAllocCat(escaped1,
					name_used);
			    if (MultipartContentType)
				StrAllocCat(escaped1, MultipartContentType);
			    StrAllocCat(escaped1, "\r\n\r\n");
			} else {
			    escaped1 = HTEscapeSP(name_used, URL_XALPHAS);
			}
			if (PlainText || Boundary) {
			    StrAllocCopy(escaped2,
					 (val_used ?
					  val_used : ""));
			} else {
			    escaped2 = HTEscapeSP(val_used, URL_XALPHAS);
			}

			HTSprintf(&query,
				"%s%s%s%s%s",
				escaped1,
				(Boundary ?
				       "" : "="),
				(PlainText ?
				      "\n" : ""),
				escaped2,
				((PlainText && *escaped2) ?
						     "\n" : ""));
			FREE(escaped1);
			FREE(escaped2);
		    }
		    FREE(copied_name_used);
		    FREE(copied_val_used);
		    break;

		case F_TEXTAREA_TYPE:
		    if (PlainText || Boundary) {
			StrAllocCopy(escaped2,
				     (val_used ?
				      val_used : ""));
		    } else {
			escaped2 = HTEscapeSP(val_used, URL_XALPHAS);
		    }

		    if (!last_textarea_name ||
			strcmp(last_textarea_name, form_ptr->name)) {
			textarea_lineno = 1;
			/*
			 *  Names are different so this is the first
			 *  textarea or a different one from any before
			 *  it.
			 */
			if (Boundary) {
			    StrAllocCopy(previous_blanks, "\r\n");
			} else {
			    FREE(previous_blanks);
			}
			if (first_one) {
			    if (Boundary) {
				HTSprintf(&query, "--%s\r\n", Boundary);
			    }
			    first_one=FALSE;
			} else {
			    if (PlainText) {
				StrAllocCat(query, "\n");
			    } else if (SemiColon) {
				StrAllocCat(query, ";");
			    } else if (Boundary) {
				HTSprintf(&query, "\r\n--%s\r\n", Boundary);
			    } else {
				StrAllocCat(query, "&");
			    }
			}
			if (PlainText) {
			    StrAllocCopy(escaped1, name_used);
			} else if (Boundary) {
			    StrAllocCopy(escaped1,
				    "Content-Disposition: form-data; name=");
			    StrAllocCat(escaped1, name_used);
			    if (MultipartContentType)
				StrAllocCat(escaped1, MultipartContentType);
			    StrAllocCat(escaped1, "\r\n\r\n");
			} else {
			    escaped1 = HTEscapeSP(name_used, URL_XALPHAS);
			}
			HTSprintf(&query,
				"%s%s%s%s%s",
				escaped1,
				(Boundary ?
				       "" : "="),
				(PlainText ?
				      "\n" : ""),
				escaped2,
				((PlainText && *escaped2) ?
						     "\n" : ""));
			FREE(escaped1);
			last_textarea_name = form_ptr->name;
		    } else {
			/*
			 *  This is a continuation of a previous textarea
			 *  add %0d%0a (\r\n) and the escaped string.
			 */
			if (escaped2[0] != '\0') {
			    if (previous_blanks) {
				StrAllocCat(query, previous_blanks);
				FREE(previous_blanks);
			    }
			    if (PlainText) {
				HTSprintf(&query, "%s\n", escaped2);
			    } else if (Boundary) {
				HTSprintf(&query, "%s\r\n", escaped2);
			    } else {
				HTSprintf(&query, "%%0d%%0a%s", escaped2);
			    }
			} else {
			    if (PlainText) {
				StrAllocCat(previous_blanks, "\n");
			    } else if (Boundary) {
				StrAllocCat(previous_blanks, "\r\n");
			    } else {
				StrAllocCat(previous_blanks, "%0d%0a");
			    }
			}
		    }
		    FREE(escaped2);
		    FREE(copied_val_used);
		    break;

		case F_PASSWORD_TYPE:
		case F_TEXT_TYPE:
		case F_OPTION_LIST_TYPE:
		case F_HIDDEN_TYPE:
		    if (first_one) {
			if (Boundary) {
			    HTSprintf(&query, "--%s\r\n", Boundary);
			}
			first_one=FALSE;
		    } else {
			if (PlainText) {
			    StrAllocCat(query, "\n");
			} else if (SemiColon) {
			    StrAllocCat(query, ";");
			} else if (Boundary) {
			    HTSprintf(&query, "\r\n--%s\r\n", Boundary);
			} else {
			    StrAllocCat(query, "&");
			}
		    }

		    if (PlainText) {
		       StrAllocCopy(escaped1, name_used);
		    } else if (Boundary) {
			StrAllocCopy(escaped1,
				    "Content-Disposition: form-data; name=");
			StrAllocCat(escaped1, name_used);
			if (MultipartContentType)
			    StrAllocCat(escaped1, MultipartContentType);
			StrAllocCat(escaped1, "\r\n\r\n");
		    } else {
			escaped1 = HTEscapeSP(name_used, URL_XALPHAS);
		    }

		    if (PlainText || Boundary) {
			StrAllocCopy(escaped2,
				     (val_used ?
				      val_used : ""));
		    } else {
			escaped2 = HTEscapeSP(val_used, URL_XALPHAS);
		    }

		    HTSprintf(&query,
			    "%s%s%s%s%s",
			    escaped1,
			    (Boundary ?
				   "" : "="),
			    (PlainText ?
				  "\n" : ""),
			    escaped2,
			    ((PlainText && *escaped2) ?
						 "\n" : ""));
		    FREE(escaped1);
		    FREE(escaped2);
		    FREE(copied_name_used);
		    FREE(copied_val_used);
		    break;
		}
	    } else if (anchor_ptr->input_field->number > form_number) {
		break;
	    }
	}

	if (anchor_ptr == HTMainText->last_anchor)
	    break;

	anchor_ptr = anchor_ptr->next;
    }
    FREE(copied_name_used);
    if (Boundary) {
	FREE(MultipartContentType);
	HTSprintf(&query, "\r\n--%s--\r\n", Boundary);
    } else if (!query) {
	StrAllocCopy(query, "");
    }
    FREE(previous_blanks);

    CTRACE((tfp, "QUERY (%d) >> \n%s\n", strlen(query), query));

    if (submit_item->submit_method == URL_MAIL_METHOD) {
	HTUserMsg2(gettext("Submitting %s"), submit_item->submit_action);
	CTRACE((tfp, "\nGridText - mailto_address: %s\n",
			    (submit_item->submit_action+7)));
	CTRACE((tfp, "GridText - mailto_subject: %s\n",
			    ((submit_item->submit_title &&
			      *submit_item->submit_title) ?
			      (submit_item->submit_title) :
					(HText_getTitle() ?
					 HText_getTitle() : ""))));
	CTRACE((tfp,"GridText - mailto_content: %s\n",query));
	mailform((submit_item->submit_action+7),
		 ((submit_item->submit_title &&
		   *submit_item->submit_title) ?
		   (submit_item->submit_title) :
			     (HText_getTitle() ?
			      HText_getTitle() : "")),
		 query,
		 content_type_out);
	FREE(query);
	FREE(content_type_out);
	return 0;
    } else {
	_statusline(SUBMITTING_FORM);
    }

    if (submit_item->submit_method == URL_POST_METHOD || Boundary) {
	StrAllocCopy(doc->post_data, query);
	FREE(doc->post_content_type);
	doc->post_content_type = content_type_out; /* don't free c_t_out */
	CTRACE((tfp,"GridText - post_data: %s\n",doc->post_data));
	StrAllocCopy(doc->address, submit_item->submit_action);
	FREE(query);
	return 1;
    } else { /* GET_METHOD */
	StrAllocCopy(doc->address, query);
	FREE(doc->post_data);
	FREE(doc->post_content_type);
	FREE(content_type_out);
	FREE(query);
	return 1;
    }
#ifdef EXP_FILE_UPLOAD
exit_disgracefully:
    FREE(escaped1);
    FREE(escaped2);
    FREE(previous_blanks);
    FREE(copied_name_used);
    FREE(copied_val_used);
    FREE(MultipartContentType);
    FREE(query);
    FREE(content_type_out);
    return 0;
#endif
}

PUBLIC void HText_DisableCurrentForm NOARGS
{
    TextAnchor * anchor_ptr;

    HTFormDisabled = TRUE;
    if (!HTMainText)
	return;

    /*
     *  Go through list of anchors and set the disabled flag.
     */
    anchor_ptr = HTMainText->first_anchor;
    while (anchor_ptr) {
	if (anchor_ptr->link_type == INPUT_ANCHOR &&
	    anchor_ptr->input_field->number == HTFormNumber) {

	    anchor_ptr->input_field->disabled = TRUE;
	}

	if (anchor_ptr == HTMainText->last_anchor)
	    break;

	anchor_ptr = anchor_ptr->next;
    }

    return;
}

PUBLIC void HText_ResetForm ARGS1(
	FormInfo *,	form)
{
    TextAnchor * anchor_ptr;

    _statusline(RESETTING_FORM);
    if (HTMainText == 0)
	return;

    /*
     *  Go through list of anchors and reset values.
     */
    anchor_ptr = HTMainText->first_anchor;
    while (anchor_ptr != 0) {
	if (anchor_ptr->link_type == INPUT_ANCHOR) {
	    if (anchor_ptr->input_field->number == form->number) {

		if (anchor_ptr->input_field->type == F_RADIO_TYPE ||
		    anchor_ptr->input_field->type == F_CHECKBOX_TYPE) {

		    if (anchor_ptr->input_field->orig_value[0] == '0')
			anchor_ptr->input_field->num_value = 0;
		    else
			anchor_ptr->input_field->num_value = 1;

		} else if (anchor_ptr->input_field->type ==
			   F_OPTION_LIST_TYPE) {
		    anchor_ptr->input_field->value =
				anchor_ptr->input_field->orig_value;

		    anchor_ptr->input_field->cp_submit_value =
				anchor_ptr->input_field->orig_submit_value;

		} else {
		    StrAllocCopy(anchor_ptr->input_field->value,
				 anchor_ptr->input_field->orig_value);
		}
	    } else if (anchor_ptr->input_field->number > form->number) {
		break;
	    }
	}

	if (anchor_ptr == HTMainText->last_anchor)
	    break;

	anchor_ptr = anchor_ptr->next;
    }
}

/*
 * This function is called before reloading/reparsing current document to find
 * whether any forms content was changed by user so any information will be
 * lost.
 */
PUBLIC BOOLEAN HText_HaveUserChangedForms NOARGS
{
    TextAnchor * anchor_ptr;

    if (HTMainText == 0)
	return FALSE;

    /*
     *  Go through list of anchors to check if any value was changed.
     *  This code based on HText_ResetForm()
     */
    anchor_ptr = HTMainText->first_anchor;
    while (anchor_ptr != 0) {
	if (anchor_ptr->link_type == INPUT_ANCHOR) {

	    if (anchor_ptr->input_field->type == F_RADIO_TYPE ||
		anchor_ptr->input_field->type == F_CHECKBOX_TYPE) {

		if ((anchor_ptr->input_field->orig_value[0] == '0' &&
		     anchor_ptr->input_field->num_value == 1) ||
		    (anchor_ptr->input_field->orig_value[0] != '0' &&
		     anchor_ptr->input_field->num_value == 0))
		    return TRUE;

	    } else if (anchor_ptr->input_field->type == F_OPTION_LIST_TYPE) {
		if (strcmp(anchor_ptr->input_field->value,
			   anchor_ptr->input_field->orig_value))
		    return TRUE;

		if (strcmp(anchor_ptr->input_field->cp_submit_value,
			   anchor_ptr->input_field->orig_submit_value))
		    return TRUE;

	    } else {
		if (strcmp(anchor_ptr->input_field->value,
			   anchor_ptr->input_field->orig_value))
		    return TRUE;
	    }
	}
	if (anchor_ptr == HTMainText->last_anchor)
	   break;

	anchor_ptr = anchor_ptr->next;
    }
    return FALSE;
}

PUBLIC void HText_activateRadioButton ARGS1(
	FormInfo *,	form)
{
    TextAnchor * anchor_ptr;
    int form_number = form->number;

    if (!HTMainText)
	return;
    anchor_ptr = HTMainText->first_anchor;
    while (anchor_ptr) {
	if (anchor_ptr->link_type == INPUT_ANCHOR &&
		anchor_ptr->input_field->type == F_RADIO_TYPE) {

	    if (anchor_ptr->input_field->number == form_number) {

		    /* if it has the same name and its on */
		 if (!strcmp(anchor_ptr->input_field->name, form->name) &&
		     anchor_ptr->input_field->num_value) {
		    anchor_ptr->input_field->num_value = 0;
		    break;
		 }
	    } else if (anchor_ptr->input_field->number > form_number) {
		    break;
	    }

	}

	if (anchor_ptr == HTMainText->last_anchor)
	    break;

	anchor_ptr = anchor_ptr->next;
   }

   form->num_value = 1;
}

#ifdef LY_FIND_LEAKS
/*
 *	Purpose:	Free all currently loaded HText objects in memory.
 *	Arguments:	void
 *	Return Value:	void
 *	Remarks/Portability/Dependencies/Restrictions:
 *		Usage of this function should really be limited to program
 *			termination.
 *	Revision History:
 *		05-27-94	created Lynx 2-3-1 Garrett Arch Blythe
 */
PRIVATE void free_all_texts NOARGS
{
    HText *cur = NULL;

    if (!loaded_texts)
	return;

    /*
     *  Simply loop through the loaded texts list killing them off.
     */
    while (loaded_texts && !HTList_isEmpty(loaded_texts)) {
	if ((cur = (HText *)HTList_removeLastObject(loaded_texts)) != NULL) {
	    if (cur->node_anchor && cur->node_anchor->underway)
		cur->node_anchor->underway = FALSE;
	    HText_free(cur);
	}
    }

    /*
     *  Get rid of the text list.
     */
    if (loaded_texts) {
	HTList_delete(loaded_texts);
    }

    /*
     *  Insurance for bad HTML.
     */
    FREE(HTCurSelectGroup);
    FREE(HTCurSelectGroupSize);
    FREE(HTCurSelectedOptionValue);
    FREE(HTFormAction);
    FREE(HTFormEnctype);
    FREE(HTFormTitle);
    FREE(HTFormAcceptCharset);
    PerFormInfo_free(HTCurrentForm);

    return;
}
#endif /* LY_FIND_LEAKS */

/*
**  stub_HTAnchor_address is like HTAnchor_address, but it returns the
**  parent address for child links.  This is only useful for traversal's
**  where one does not want to index a text file N times, once for each
**  of N internal links.  Since the parent link has already been taken,
**  it won't go again, hence the (incorrect) links won't cause problems.
*/
PUBLIC char * stub_HTAnchor_address ARGS1(
	HTAnchor *,	me)
{
    char *addr = NULL;
    if (me)
	StrAllocCopy (addr, me->parent->address);
    return addr;
}

PUBLIC void HText_setToolbar ARGS1(
	HText *,	text)
{
    if (text)
	text->toolbar = TRUE;
    return;
}

PUBLIC BOOL HText_hasToolbar ARGS1(
	HText *,	text)
{
    return (BOOL) ((text && text->toolbar) ? TRUE : FALSE);
}

PUBLIC void HText_setNoCache ARGS1(
	HText *,	text)
{
    if (text)
	text->no_cache = TRUE;
    return;
}

PUBLIC BOOL HText_hasNoCacheSet ARGS1(
	HText *,	text)
{
    return (BOOL) ((text && text->no_cache) ? TRUE : FALSE);
}

PUBLIC BOOL HText_hasUTF8OutputSet ARGS1(
	HText *,	text)
{
    return (BOOL) ((text && text->T.output_utf8) ? TRUE : FALSE);
}

/*
**  Check charset and set the kcode element. - FM
**  Info on the input charset may be passed in in two forms,
**  as a string (if given explicitly) and as a pointer to
**  a LYUCcharset (from chartrans mechanism); either can be NULL.
**  For Japanese the kcode will be reset at a space or explicit
**  line or paragraph break, so what we set here may not last for
**  long.  It's potentially more important not to set HTCJK to
**  NOCJK unless we are sure. - kw
*/
PUBLIC void HText_setKcode ARGS3(
	HText *,	text,
	CONST char *,	charset,
	LYUCcharset *,	p_in)
{
    BOOL explicit;

    if (!text)
	return;

    /*
    **  Check whether we have some kind of info. - kw
    */
    if (!charset && !p_in) {
	return;
    }
    explicit = charset ? TRUE : FALSE;
    /*
    **  If no explicit charset string, use the implied one. - kw
    */
    if (!charset || *charset == '\0') {
	charset = p_in->MIMEname;
    }
    /*
    **  Check whether we have a specified charset. - FM
    */
    if (!charset || *charset == '\0') {
	return;
    }

    /*
    **  We've included the charset, and not forced a download offer,
    **  only if the currently selected character set can handle it,
    **  so check the charset value and set the text->kcode element
    **  appropriately. - FM
    */
    /*  If charset isn't specified explicitely nor assumed,
     *  p_in->MIMEname would be set as display charset.
     *  So text->kcode sholud be set as SJIS or EUC here only if charset
     *  is specified explicitely, otherwise text->kcode would cause
     *  mishandling Japanese strings. -- TH
     */
    if (explicit && (!strcmp(charset, "shift_jis") ||
	!strcmp(charset, "x-sjis") ||		/* 1997/11/28 (Fri) 18:11:33 */
	!strcmp(charset, "x-shift-jis")))
    {
	text->kcode = SJIS;
    } else if (explicit && ((p_in && (p_in->enc == UCT_ENC_CJK)) ||
	       !strcmp(charset, "x-euc") ||	/* 1997/11/28 (Fri) 18:11:24 */
	       !strcmp(charset, "euc-jp") ||
	       !strncmp(charset, "x-euc-", 6) ||
#if 0 /* iso-2022-jp* shouldn't be treated as euc-jp */
	       !strcmp(charset, "iso-2022-jp") ||
	       !strcmp(charset, "iso-2022-jp-2") ||
#endif
	       !strcmp(charset, "euc-kr") ||
	       !strcmp(charset, "iso-2022-kr") ||
	       !strcmp(charset, "big5") ||
	       !strcmp(charset, "cn-big5") ||
	       !strcmp(charset, "euc-cn") ||
	       !strcmp(charset, "gb2312") ||
	       !strncmp(charset, "cn-gb", 5) ||
	       !strcmp(charset, "iso-2022-cn"))) {
	text->kcode = EUC;
    } else {
	/*
	**  If we get to here, it's not CJK, so disable that if
	**  it is enabled.  But only if we are quite sure. - FM & kw
	*/
	text->kcode = NOKANJI;
	if (HTCJK != NOCJK) {
	    if (!p_in || p_in->enc != UCT_ENC_CJK)
		HTCJK = NOCJK;
	}
    }
    if (explicit)
	text->specified_kcode = text->kcode;
    else {
	if (UCAssume_MIMEcharset) {
	    if (!strcmp(UCAssume_MIMEcharset, "euc-jp"))
		text->kcode = text->specified_kcode = EUC;
	    else if (!strcmp(UCAssume_MIMEcharset, "shift_jis"))
		text->kcode = text->specified_kcode = SJIS;
	}
    }

    return;
}

/*
**  Set a permissible split at the current end of the last line. - FM
*/
PUBLIC void HText_setBreakPoint ARGS1(
	HText *,	text)
{
    if (!text)
	return;

    /*
     *  Can split here. - FM
     */
    text->permissible_split = text->last_line->size;

    return;
}

/*
**  This function determines whether a document which
**  would be sought via the a URL that has a fragment
**  directive appended is otherwise identical to the
**  currently loaded document, and if so, returns
**  FALSE, so that any no_cache directives can be
**  overridden "safely", on the grounds that we are
**  simply acting on the equivalent of a paging
**  command.  Otherwise, it returns TRUE, i.e, that
**  the target document might differ from the current,
**  based on any caching directives or analyses which
**  claimed or suggested this. - FM
*/
PUBLIC BOOL HText_AreDifferent ARGS2(
	HTParentAnchor *,	anchor,
	CONST char *,		full_address)
{
    HTParentAnchor *MTanc;
    char *MTaddress;
    char *MTpound;
    char *TargetPound;

    /*
     *  Do we have a loaded document and both
     *  arguments for this function?
     */
    if (!(HTMainText && anchor && full_address))
	return TRUE;

    /*
     *  Do we have both URLs?
     */
    MTanc = HTMainText->node_anchor;
    if (!(MTanc->address && anchor->address))
	return (TRUE);

    /*
     *  Do we have a fragment associated with the target?
     */
    if ((TargetPound = strchr(full_address, '#')) == NULL)
	return (TRUE);

    /*
     *  Always treat client-side image map menus
     *  as potentially stale, so we'll create a
     *  fresh menu from the LynxMaps HTList.
     */
    if (!strncasecomp(anchor->address, "LYNXIMGMAP:", 11))
	return (TRUE);

    /*
     *  Do the docs differ in the type of request?
     */
    if (MTanc->isHEAD != anchor->isHEAD)
	return (TRUE);

    /*
     *  Are the actual URLs different, after factoring
     *  out a "LYNXIMGMAP:" leader in the MainText URL
     *  and its fragment, if present?
     */
    MTaddress = (strncasecomp(MTanc->address,
			      "LYNXIMGMAP:", 11) ?
				  MTanc->address : (MTanc->address + 11));
    if ((MTpound = strchr(MTaddress, '#')) != NULL)
	*MTpound = '\0';
    if (strcmp(MTaddress, anchor->address)) {
	if (MTpound != NULL) {
	    *MTpound = '#';
	}
	return(TRUE);
    }
    if (MTpound != NULL) {
	*MTpound = '#';
    }

    /*
     *  If the MainText is not an image map menu,
     *  do the docs have different POST contents?
     */
    if (MTaddress == MTanc->address) {
	if (MTanc->post_data) {
	    if (anchor->post_data) {
		if (strcmp(MTanc->post_data, anchor->post_data)) {
		    /*
		     *  Both have contents, and they differ.
		     */
		    return(TRUE);
		}
	    } else {
		/*
		 *  The loaded document has content, but the
		 *  target doesn't, so they're different.
		 */
		return(TRUE);
	    }
	} else if (anchor->post_data) {
	    /*
	     *  The loaded document does not have content, but
	     *  the target does, so they're different.
	     */
	    return(TRUE);
	}
    }

    /*
     *  We'll assume the target is a position in the currently
     *  displayed document, and thus can ignore any header, META,
     *  or other directives not to use a cached rendition. - FM
     */
    return(FALSE);
}


/*
 *  Cleanup new lines coming into a TEXTAREA from an external editor, or a
 *  file, such that they are in a suitable format for TEXTAREA rendering,
 *  display, and manipulation.  That means trimming off trailing whitespace
 *  from the line, expanding TABS into SPACES, and substituting a printable
 *  character for control chars, and the like.
 *
 *  --KED  02/24/99
 */
PRIVATE void cleanup_line_for_textarea ARGS2(
	     char *,  line,
	     int,     len)
{
    char  tbuf[MAX_LINE];

    char *cp;
    char *p;
    char *s;
    int   i;
    int   n;


    /*
     *  Whack off trailing whitespace from the line.
     */
    for (i = len, p = line + (len - 1); i != 0; p--, i--) {
	if (isspace(UCH(*p)))
	    *p = '\0';
	else
	    break;
    }

    if (strlen (line) != 0) {
	/*
	 *  Expand any tab's, since they won't render properly in a TEXTAREA.
	 *
	 *  [Is that "by spec", or just a "lynxism"?  As may be, it seems that
	 *   such chars may cause other problems, too ... with cursor movement,
	 *   submit'ing, etc.  Probably needs looking into more deeply.]
	 */
	p = line;
	s = tbuf;

	while (*p) {
	    if ((cp = strchr (p, '\t')) != 0) {
		i  = cp - p;
		s  = (strncpy (s, p, i))      + i;
		n  = TABSTOP - (i % TABSTOP);
		s  = (strncpy (s, SPACES, n)) + n;
		p += (i + 1);

	    } else {

		strcpy (s, p);
		break;
	    }
	}

	/*
	 *  Replace control chars with something printable.  Note that char
	 *  substitution above 0x7f is dependent on the charset being used,
	 *  and then only applies to the contiguous run of char values that
	 *  are between 0x80, and the 1st real high-order-bit-set character,
	 *  as specified by the charset.  In general (ie, for many character
	 *  sets), that usually means the so-called "C1 control chars" that
	 *  range from 0x80 thru 0x9f.  For EBCDIC machines, we only trim the
	 *  (control) chars below a space (x'40').
	 *
	 *  The assumption in all this is that the charset used in the editor,
	 *  is compatible with the charset specified in lynx.
	 *
	 *  [At some point in time, when/if lynx ever supports multibyte chars
	 *   internally (eg, UCS-2, UCS-4, UTF-16, etc), this kind of thing may
	 *   well cause problems.  But then, supporting such char sets will
	 *   require massive changes in (most) all parts of the lynx code, so
	 *   until then, we do the rational thing with char values that would
	 *   otherwise foul the display, if left alone.  If you're implementing
	 *   multibyte character set support, consider yourself to have been
	 *   warned.]
	 */
	for (p = line, s = tbuf; *s != '\0'; p++, s++) {
#ifndef EBCDIC
	    *p = ((UCH(*s)  < UCH(' '))       ||
		  (UCH(*s) == UCH('\177'))    ||
		  ((UCH(*s) > UCH('\177')) &&
		   (UCH(*s) <
		    UCH(LYlowest_eightbit[current_char_set]))))
		 ? (char) SPLAT : *s;
#else
	    *p = (UCH(*s) < UCH(' ')) ? SPLAT : *s;
#endif
	}
	*p = '\0';
    }

    return;
}


/*
 *  Re-render the text of a tagged ("[123]") HTLine (arg1), with the tag
 *  number incremented by some value (arg5).  The re-rendered string may
 *  be allowed to expand in the event of a tag width change (eg, 99 -> 100)
 *  as controlled by arg6 (CHOP or NOCHOP).  Arg4 is either (the address
 *  of) a value which must match, in order for the tag to be incremented,
 *  or (the address of) a 0-value, which will match any value, and cause
 *  any valid tag to be incremented.  Arg2 is a pointer to the first/only
 *  anchor that exists on the line; we may need to adjust their position(s)
 *  on the line.  Arg3 when non-0 indicates the number of new digits that
 *  were added to the 2nd line in a line crossing pair.
 *
 *  All tags fields in a line which individually match an expected new value,
 *  are incremented.  Line crossing [tags] are handled (PITA).
 *
 *  Untagged or improperly tagged lines are not altered.
 *
 *  Returns the number of chars added to the original string's length, if
 *  any.
 *
 *  --KED  02/03/99
 */
PRIVATE int increment_tagged_htline ARGS6(
	HTLine *,	ht,
	TextAnchor *,   a,
	int *,		lx_val,
	int *,		old_val,
	int,		incr,
	int,		mode)
{
    char    buf[MAX_LINE];
    char  lxbuf[MAX_LINE * 2];

    TextAnchor *st_anchor  = a;
    TextAnchor *nxt_anchor;

    char *p   = ht->data;
    char *s   = buf;
    char *lx  = lxbuf;
    char *t;

    BOOLEAN   plx    = FALSE;
    BOOLEAN   valid;

    int   val;
    int   n;
    int   new_n;
    int   pre_n;
    int   fixup = 0;


    /*
     *  Cleanup for the 2nd half of a line crosser, whose number of tag
     *  digits grew by some number of places (usually 1 when it does
     *  happen, though it *could* be more).  The tag chars were already
     *  rendered into the 2nd line of the pair, but the positioning and
     *  other effects haven't been rippled through any other anchors on
     *  the (2nd) line.  So we do that here, as a special case, since
     *  the part of the tag that's in the 2nd line of the pair, will not
     *  be found by the tag string parsing code.  Double PITA.
     *
     *  [see comments below on line crosser caused problems]
     */
    if (*lx_val != 0) {
	nxt_anchor = st_anchor;
	while ((nxt_anchor) && (nxt_anchor->line_num == a->line_num)) {
	    nxt_anchor->line_pos += *lx_val;
	    nxt_anchor = nxt_anchor->next;
	}
	fixup  = *lx_val;
	*lx_val = 0;
	if (st_anchor) st_anchor = st_anchor->next;
    }

    /*
     *  Walk thru the line looking for tags (ie, "[nnn]" strings).
     */
    while  (*p != '\0') {
	if (*p != '[')	{
	    *s++ = *p++;
	    continue;

	} else {
	    *s++  = *p++;
	    t     = p;
	    n     = 0;
	    valid = TRUE;   /* p = t = byte after '[' */

	    /*
	     *  Make sure there are only digits between "[" and "]".
	     */
	    while  (*t != ']')  {
		if (*t == '\0') { /* uhoh - we have a potential line crosser */
		    valid = FALSE;
		    plx   = TRUE;
		    break;
		}
		if (isdigit(UCH(*t++)) != 0) {
		    n++;
		    continue;
		} else {
		    valid = FALSE;
		    break;
		}
	    }

	    /*
	     *  If the format is OK, we check to see if the value is what
	     *  we expect.  If not, we have a random [nn] string in the text,
	     *  and leave it alone.
	     *
	     *  [It is *possible* to have a false match here, *if* there are
	     *   two identical [nn] strings (including the numeric value of
	     *   nn), one of which is the [tag], and the other being part of
	     *   a document.  In such a case, the 1st [nn] string will get
	     *   incremented; the 2nd one won't, which makes it a 50-50 chance
	     *   of being correct, if and when such an unlikely juxitposition
	     *   of text ever occurs.  Further validation tests of the [nnn]
	     *   string are probably not possible, since little of the actual
	     *   anchor-associated-text is retained in the TextAnchor or the
	     *   FormInfo structs.  Fortunately, I think the current method is
	     *   more than adequate to weed out 99.999% of any possible false
	     *   matches, just as it stands.  Caveat emptor.]
	     */
	    if ((valid) && (n > 0)) {
		val = atoi (p);
		if ((val == *old_val) || (*old_val == 0)) { /* 0 matches all */
		    if (*old_val != 0)
			(*old_val)++;
		    val += incr;
		    sprintf (s, "%d", val);
		    new_n = strlen (s);
		    s += new_n;
		    p += n;

		    /*
		     *  If the number of digits in an existing [tag] increased
		     *  (eg, [99] --> [100], etc), we need to "adjust" its
		     *  horizontal position, and that of all subsequant tags
		     *  that may be on the same line.  PITA.
		     *
		     *  [This seems to work as long as a tag isn't a line
		     *   crosser; when it is, the position of anchors on either
		     *   side of the split tag, seem to "float" and try to be
		     *   as "centered" as possible.  Which means that simply
		     *   incrementing the line_pos by the fixed value of the
		     *   number of digits that got added to some tag in either
		     *   line doesn't work quite right, and the text for (say)
		     *   a button may get stomped on by another copy of itself,
		     *   but offset by a few chars, when it is selected (eg,
		     *   "Box Office" may end up looking like "BoBox Office" or
		     *   "Box Officece", etc.
		     *
		     *   Dunno how to fix that behavior ATT, but at least the
		     *   tag numbers themselves are correct.  -KED  /\oo/\ ]
		     */
		    if ((new_n -= n) != 0) {
			nxt_anchor = st_anchor;
			while ((nxt_anchor)			      &&
			       (nxt_anchor->line_num == a->line_num)) {
			    nxt_anchor->line_pos += new_n;
			    nxt_anchor = nxt_anchor->next;
			}
			if (st_anchor) st_anchor = st_anchor->next;
		    }
		}
	   }

	    /*
	     *  Unfortunately, valid [tag] strings *can* be split across two
	     *  lines.  Perhaps it would be best to just prevent that from
	     *  happening, but a look into that code, makes me wonder.  Anyway,
	     *  we can handle such tags without *too* much trouble in here [I
	     *  think], though since such animals are rather rare, it makes it
	     *  a bit difficult to test thoroughly (ie, Beyond here, there be
	     *  Dragons).
	     *
	     *  We use lxbuf[] to deal with the two lines involved.
	     */
	    if (plx) {
		strcpy (lx, p);      /* <- 1st part of a possible lx'ing tag */
		pre_n = strlen (p);  /* count of 1st part chars in this line */
		strcat (lx, ht->next->data);   /* tack on NEXT line	     */

		t     = lx;
		n     = 0;
		valid = TRUE;

		/*
		 *  Go hunting again for just digits, followed by tag end ']'.
		 */
		while (*t != ']') {
		    if (isdigit(UCH(*t++)) != 0) {
			n++;
			continue;
		    } else {
			valid = FALSE;
			break;
		    }
		}

		/*
		 *  It *looks* like a line crosser; now we value test it to
		 *  find out for sure [but see the "false match" warning,
		 *  above], and if it matches, increment it into the buffer,
		 *  along with the 2nd line's text.
		 */
		if ((valid) && (n > 0)) {
		    val = atoi (lx);
		    if ((val == *old_val) || (*old_val == 0)) {
			if (*old_val != 0)
			    (*old_val)++;
			val += incr;
			sprintf (lx, "%d", val);
			new_n = strlen (lx);
			strcat (lx, strchr (ht->next->data, ']'));

			/*
			 *  We keep the the same number of chars from the
			 *  adjusted tag number in the current line; any
			 *  extra chars due to a digits increase, will be
			 *  stuffed into the next line.
			 *
			 *  Keep track of any digits added, for the next
			 *  pass through.
			 */
			s   = strncpy (s, lx, pre_n) + pre_n;
			lx += pre_n;
			strcpy (ht->next->data, lx);

			*lx_val = new_n - n;
		    }
		}
		break;	/* had an lx'er, so we're done with this line */
	    }
	}
    }

    *s = '\0';

    n = strlen (ht->data);
    if (mode == CHOP)
	*(buf + n) = '\0';
    strcpy (ht->data, buf);

    return (strlen (buf) - n + fixup);
}


/*
 *  Creates a new anchor and associated struct's appropriate for a form
 *  TEXTAREA, and links them into the lists following the current anchor
 *  position (as specified by arg1).
 *
 *  Exits with arg1 now pointing at the new TextAnchor, and arg2 pointing
 *  at the new, associated HTLine.
 *
 *  --KED  02/13/99
 */
PRIVATE void insert_new_textarea_anchor ARGS2(
	TextAnchor **,   curr_anchor,
	HTLine **,       exit_htline)
{
    TextAnchor *anchor = *curr_anchor;
    HTLine     *htline;

    TextAnchor *a = 0;
    FormInfo   *f = 0;
    HTLine     *l = 0;

    int curr_tag  = 0;   /* 0 ==> match any [tag] number */
    int lx	  = 0;	 /* 0 ==> no line crossing [tag]; it's a new line */
    int i;


    /*
     *  Find line in the text that matches ending anchorline of
     *  the TEXTAREA.
     *
     *  [Yes, Virginia ... we *do* have to go thru this for each
     *   anchor being added, since there is NOT a 1-to-1 mapping
     *   between anchors and htlines.  I suppose we could create
     *   YAS (Yet Another Struct), but there are too many structs{}
     *   floating around in here, as it is.  IMNSHO.]
     */
    for (htline = HTMainText->last_line->next, i = 0;
	 anchor->line_num != i;                i++) {
	htline = htline->next;
	if (htline == HTMainText->last_line)
	    break;
    }

    /*
     *  Clone and initialize the struct's needed to add a new TEXTAREA
     *  anchor.
     */
    if (((a = typecalloc(TextAnchor)) == 0) ||
	((f = typecalloc(FormInfo)) == 0) ||
	((l = allocHTLine(MAX_LINE)) == 0))
	outofmem(__FILE__, "insert_new_textarea_anchor");

    /*  Init all the fields in the new TextAnchor.                 */
    /*  [anything "special" needed based on ->show_anchor value ?] */
    a->next	       = anchor->next;
    a->number	       = anchor->number;
    a->start	       = anchor->start + anchor->input_field->size + 1;
    a->line_pos	       = anchor->line_pos;
    a->extent	       = anchor->extent;
    a->line_num	       = anchor->line_num + 1;
    StrAllocCopy (a->hightext,  anchor->hightext);
    StrAllocCopy (a->hightext2, anchor->hightext2);
    a->hightext2offset = anchor->hightext2offset;
    a->link_type       = anchor->link_type;
    a->input_field     = f;
    a->show_anchor     = anchor->show_anchor;
    a->inUnderline     = anchor->inUnderline;
    a->expansion_anch  = TRUE;
    a->anchor	       = NULL;

    /*  Just the (seemingly) relevant fields in the new FormInfo.  */
    /*  [do we need to do anything "special" based on ->disabled]  */
    StrAllocCopy (f->name, anchor->input_field->name);
    f->number	       = anchor->input_field->number;
    f->type	       = anchor->input_field->type;
    StrAllocCopy (f->orig_value, "");
    f->size	       = anchor->input_field->size;
    f->maxlength       = anchor->input_field->maxlength;
    f->no_cache        = anchor->input_field->no_cache;
    f->disabled        = anchor->input_field->disabled;
    f->value_cs        = current_char_set; /* use current setting - kw */

    /*  Init all the fields in the new HTLine (but see the #if).   */
    l->next	       = htline->next;
    l->prev	       = htline;
    l->offset	       = htline->offset;
    l->size	       = htline->size;
    l->split_after     = htline->split_after;
    l->bullet	       = htline->bullet;
    l->expansion_line  = TRUE;
#if defined(USE_COLOR_STYLE)
    /* dup styles[] if needed [no need in TEXTAREA (?); leave 0's] */
    l->numstyles       = htline->numstyles;
#ifndef OLD_HTSTYLECHANGE
    /*we fork the pointers!*/
    l->styles = htline->styles;
#endif
#endif
    strcpy (l->data,     htline->data);
    if (keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) {
	a->number++;
	increment_tagged_htline (l, a, &lx, &curr_tag, 1, CHOP);
    }

    /*
     *  If we're at the tail end of the TextAnchor or HTLine list(s),
     *  the new node becomes the last node.
     */
    if (anchor == HTMainText->last_anchor)
	HTMainText->last_anchor = a;
    if (htline == HTMainText->last_line)
	HTMainText->last_line = l;

    /*
     *  Link in the new TextAnchor and point the entry anchor arg at it;
     *  link in the new HTLine and point the entry htline arg at it, too.
     */
    anchor->next = a;
   *curr_anchor  = a;

    htline->next->prev = l;
    htline->next       = l;
   *exit_htline        = l->next;

    return;
}


/*
 *  If new anchors were added to expand a TEXTAREA, we need to ripple the
 *  new line numbers [and char counts ?] thru the subsequent anchors.
 *
 *  If form lines are getting [nnn] tagged, we need to update the displayed
 *  tag values to match (which means rerendering them ... sigh).
 *
 *  Finally, we need to update various HTMainText and other counts, etc.
 *
 *  [dunno if the char counts really *need* to be done, or if we're using
 *   the exactly proper values/algorithms ... seems to be OK though ...]
 *
 *  --KED  02/13/99
 */
PRIVATE void update_subsequent_anchors ARGS4(
	int,		 n,
	TextAnchor *,	 start_anchor,
	HTLine *,        start_htline,
	int,		 start_tag)
{
    TextAnchor *anchor;
    HTLine     *htline = start_htline;

    int form_chars_added = (start_anchor->input_field->size + 1) * n;
    int		line_adj = 0;
    int		tag_adj	 = 0;
    int		lx	 = 0;
    int	     hang	 = 0;  /* for HANG detection of a nasty intermittent */
    int      hang_detect = 100000;  /* ditto */


    CTRACE((tfp, "GridText: adjusting struct's to add %d new line(s)\n", n));

    /*
     *  Update numeric fields of the rest of the anchors.
     *
     *  [We bypass bumping ->number if it has a value of 0, which takes care
     *   of the ->input_field->type == F_HIDDEN_TYPE (as well as any other
     *   "hidden" anchors, if such things exist).  Seems like the "right
     *   thing" to do.  I think.]
     */
    anchor = start_anchor->next;   /* begin updating with the NEXT anchor */
    while (anchor) {
	if ((keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) &&
	    (anchor->number != 0))
	    anchor->number += n;
	anchor->line_num  += n;
	anchor->start	  += form_chars_added;
	anchor = anchor->next;
    }

    /*
     *  Update/rerender anchor [tags], if they are being numbered.
     *
     *  [If a number tag (eg, "[177]") is itself broken across a line
     *   boundary, this fixup only partially works.  While the tag
     *   numbering is done properly across the pair of lines, the
     *   horizontal positioning on *either* side of the split, can get
     *   out of sync by a char or two when it gets selected.  See the
     *   [comments] in  increment_tagged_htline()  for some more detail.
     *
     *   I suppose THE fix is to prevent such tag-breaking in the first
     *   place (dunno where yet, though).  Ah well ... at least the tag
     *   numbers themselves are correct from top to bottom now.
     *
     *   All that said, about the only time this will be a problem in
     *   *practice*, is when a page has near 1000 links or more (possibly
     *   after a TEXTAREA expansion), and has line crossing tag(s), and
     *   the tag numbers in a line crosser go from initially all 3 digit
     *   numbers, to some mix of 3 and 4 digits (or all 4 digits) as a
     *   result of the expansion process.  Oh, you also need a "clump" of
     *   anchors all on the same lines.
     *
     *   Yes, it *can* happen, but in real life, it probably won't be
     *   seen very much ...]
     *
     *  [This may also be an artifact of bumping into the right hand
     *   screen edge (or RHS margin), since we don't even *think* about
     *   relocating an anchor to the following line, when [tag] digits
     *   expansion pushes things too far in that direction.]
     */
    if (keypad_mode == LINKS_AND_FIELDS_ARE_NUMBERED) {
	anchor = start_anchor->next;
	while (htline != HTMainText->last_line->next) {

	    while (anchor) {
		if ((anchor->number - n) == start_tag)
		    break;

		/*** A HANG (infinite loop) *has* occurred here, with */
		/*** the values of anchor and anchor->next being the  */
		/*** the same, OR with anchor->number "magically" and */
		/*** suddenly taking on an anchor-pointer-like value. */
		/***                                                  */
		/*** The same code and same doc have both passed and  */
		/*** failed at different times, which indicates some  */
		/*** sort of content/html dependency, or some kind of */
		/*** a "race" condition, but I'll be damned if I can  */
		/*** find it after tons of CTRACE's, printf()'s, gdb  */
		/*** breakpoints and watchpoints, etc.                */
		/***                                                  */
		/*** I have added a hang detector (with error msg and */
		/*** beep) here, to break the loop and warn the user, */
		/*** until it can be isolated and fixed.              */
		/***                                                  */
		/*** [One UGLY intermittent .. gak ..!  02/22/99 KED] */

		hang++;
		if ((anchor == anchor->next) || (hang >= hang_detect))
		    goto hang_detected;

		anchor = anchor->next;
	    }

	    if (anchor) {
		line_adj = increment_tagged_htline (htline,  anchor,  &lx,
						    &start_tag, n,  NOCHOP);
		htline->size += line_adj;
		tag_adj      += line_adj;

	    } else {

		break;	/* out of anchors ... we're done */
	    }

	    htline = htline->next;
	}
    }

exit:
    /*
     *  Fixup various global variables.
     */
    nlinks                         += n;
    HTMainText->Lines              += n;
    HTMainText->last_anchor_number += n;
    HTMainText->chars              += (form_chars_added + tag_adj);

    more = HText_canScrollDown();

    CTRACE((tfp, "GridText: TextAnchor and HTLine struct's adjusted\n"));

    return;

hang_detected:  /* uglyness has happened; inform user and do the best we can */

    printf ("\007");  /* beep the user */
    fflush (NULL);
    HTAlert ("Hang Detect: TextAnchor struct corrupted - suggest aborting!");
    HTAlert ("Hang Detect: TextAnchor struct corrupted - suggest aborting!");
    goto exit;
}


/*
 *  Transfer the initial contents of a TEXTAREA to a temp file, invoke the
 *  user's editor on that file, then transfer the contents of the resultant
 *  edited file back into the TEXTAREA (expanding the size of the area, if
 *  required).
 *
 *  Returns the number of lines that the cursor should be moved so that it
 *  will end up on the 1st blank line of whatever number of trailing blank
 *  lines there are in the TEXTAREA (there will *always* be at least one).
 *
 *  --KED  02/01/99
 */
PUBLIC int HText_ExtEditForm ARGS1(
	   struct link *,  form_link)
{
    struct stat stat_info;
    size_t	size;

    char       *ed_temp;
    FILE       *fp;
    int		rv;

    TextAnchor *anchor_ptr;
    TextAnchor *start_anchor  = NULL;
    TextAnchor *end_anchor    = NULL;
    BOOLEAN	firstanchor   = TRUE;
    BOOLEAN wrapalert = FALSE;

    char	ed_offset[10];
    int		start_line    = 0;
    int		entry_line    = form_link->anchor_line_num;
    int		exit_line     = 0;
    int		orig_cnt      = 0;
    int		line_cnt      = 1;

    FormInfo   *form	 = form_link->form;
    char       *areaname = form->name;
    int		form_num = form->number;

    HTLine     *htline	 = NULL;

    char       *ebuf;
    char       *tbuf = NULL;
    char       *line;
    char       *lp;
    char       *cp;
    int         match_tag = 0;
    int		newlines  = 0;
    int		len, len0, len_in;
    int		wanted_fieldlen_wrap = -1; /* not yet asked; 0 means don't. */
    char       *skip_at = NULL;
    int		skip_num = 0, i;

    CTRACE((tfp, "GridText: entered HText_ExtEditForm()\n"));

    ed_temp = (char *) malloc (LY_MAXPATH);
    if ((fp = LYOpenTemp (ed_temp, "", "w")) == 0) {
	FREE(ed_temp);
	return (0);
    }

    /*
     *	Begin at the beginning, to find 1st anchor in the TEXTAREA, then
     *	write all of its lines (anchors) out to the edit temp file.
     *
     *	[Finding the TEXTAREA we're actually *in* with these attributes
     *	 isn't foolproof.  The form_num isn't unique to a given TEXTAREA,
     *	 and there *could* be TEXTAREA's with the same "name".	If that
     *	 should ever be true, we'll actually get the data from the *1st*
     *	 TEXTAREA in the page that matches.  We should probably assign
     *	 a unique id to each TEXTAREA in a page, and match on that, to
     *	 avoid this (potential) problem.
     *
     *	 Since the odds of "false matches" *actually* happening in real
     *	 life seem rather small though, we'll hold off doing this, for a
     *	 rainy day ...]
     */
    anchor_ptr = HTMainText->first_anchor;

    while (anchor_ptr) {

	if ((anchor_ptr->link_type	     == INPUT_ANCHOR)    &&
	    (anchor_ptr->input_field->type   == F_TEXTAREA_TYPE) &&
	    (anchor_ptr->input_field->number == form_num)	 &&
	    !strcmp (anchor_ptr->input_field->name, areaname))   {

	    if (firstanchor) {
		firstanchor  = FALSE;
		start_anchor = anchor_ptr;
		start_line   = anchor_ptr->line_num;
	    }
	    orig_cnt++;

	    /*
	     *  Write the anchors' text to the temp edit file.
	     */
	    fputs (anchor_ptr->input_field->value, fp);
	    fputc ('\n', fp);

	 } else {

	    if (!firstanchor)
		break;
	 }
	 anchor_ptr = anchor_ptr->next;
    }
    LYCloseTempFP (fp);

    CTRACE((tfp, "GridText: TEXTAREA name=|%s| dumped to tempfile\n", areaname));
    CTRACE((tfp, "GridText: invoking editor (%s) on tempfile\n", editor));

    /*
     *	Go edit the TEXTAREA temp file, with the initial editor line
     *  corresponding to the TEXTAREA line the cursor is on (if such
     *  positioning is supported by the editor [as lynx knows it]).
     */
    ed_offset[0] = 0; /* pre-ANSI compilers don't initialize aggregates - TD */
    if (((entry_line - start_line) > 0) && editor_can_position())
#ifdef VMS
	sprintf (ed_offset, "-%d", ((entry_line - start_line) + 1));
    HTSprintf0 (&tbuf, "%s %s %s", editor, ed_temp, ed_offset);
#else
	sprintf (ed_offset, "+%d", ((entry_line - start_line) + 1));
    HTSprintf0 (&tbuf, "%s %s %s", editor, ed_offset, ed_temp);
#endif

#ifdef UNIX
    set_errno(0);
#endif
    rv = LYSystem (tbuf);	/* finally the editor is called */
    if (rv) {
	/*
	 *  If something went wrong, we should probably return soon;
	 *  currently we don't, but at least put out a message. - kw
	 */
#ifdef UNIX
	int rvhi = (rv >> 8);
	CTRACE((tfp, "ExtEditForm: system() returned %d (0x%x), %s\n",
	       rv, rv, errno ? LYStrerror(errno) : "reason unknown"));
	LYFixCursesOn("show error warning:");
	if (rv != -1 && (rv && 0xff) && !rvhi) {
	    HTAlwaysAlert(NULL, gettext("Editor killed by signal"));
	} else if (!(rv == -1 || (rvhi == 127 && errno))) {
	    HTUserMsg2(gettext("Editor returned with error status, %s"),
		       errno ? LYStrerror(errno) : gettext("reason unknown."));
	} else
#endif
	    HTAlwaysAlert(NULL, ERROR_SPAWNING_EDITOR);
    }

#ifdef UNIX
    /*
     *  Delete backup file, if that's your style.
     */
    HTSprintf0 (&tbuf, "%s~", ed_temp);
    if (stat (tbuf, &stat_info) == 0)
	remove (tbuf);
#endif
    FREE(tbuf);

    CTRACE((tfp, "GridText: returned from editor (%s)\n", editor));

    /*
     *	Read back the edited temp file into our buffer.
     */
    if ((stat (ed_temp, &stat_info) < 0)   ||
	!S_ISREG(stat_info.st_mode)        ||
	((size = stat_info.st_size) == 0)) {
	size = 0;
	ebuf = typecalloc (char);
	if (!ebuf)
	    outofmem(__FILE__, "HText_ExtEditForm");
    } else {
	ebuf = typecallocn(char, size + 1);
	if (!ebuf) {
	    /*
	     *  This could be huge - don't exit if we don't have enough
	     *  memory for it.  With some luck, the user may be even able
	     *  to recover the file manually from the temp space while
	     *  the lynx session is not over. - kw
	     */
	    free(ed_temp);
	    HTAlwaysAlert(NULL, MEMORY_EXHAUSTED_FILE);
	    return 0;
	}

	fp = fopen (ed_temp, "r");
	size = fread (ebuf, 1, size, fp);
	LYCloseInput (fp);
	ebuf[size] = '\0';	/* Terminate! - kw */
    }

    /*
     *	Nuke any blank lines from the end of the edited data.
     */
    while ((size != 0) && (isspace(UCH(ebuf[size-1])) || (ebuf[size-1] == '\0')))
	ebuf[--size] = '\0';

    /*
     *	Copy each line from the temp file into the corresponding anchor
     *  struct.  Add new lines to the TEXTAREA if needed.  (Always leave
     *  the user with a blank line at the end of the TEXTAREA.)
     */
    if ((line = (char *) malloc (MAX_LINE)) == 0)
	outofmem(__FILE__, "HText_ExtEditForm");

    anchor_ptr = start_anchor;
    if (anchor_ptr->input_field->size <= 4 ||
	anchor_ptr->input_field->size >= MAX_LINE)
	wanted_fieldlen_wrap = 0;

    len = len_in = 0;
    lp  = ebuf;

    while ((line_cnt <= orig_cnt) || (*lp) || ((len != 0) && (*lp == '\0'))) {

	if (skip_at) {
	    len0 = skip_at - lp;
	    strncpy(line, lp, len0);
	    line[len0] = '\0';
	    lp = skip_at + skip_num;
	    skip_at = NULL;
	    skip_num = 0;
	} else {
	    len0 = 0;
	}
	line[len0] = '\0';

	if ((cp = strchr (lp, '\n')) != 0)
	   len = len_in = cp - lp;
	else
	   len = len_in = strlen (lp);


	if (wanted_fieldlen_wrap < 0 && !wrapalert &&
	    len0+len >= start_anchor->input_field->size &&
	    (cp = strchr(lp, ' ')) != NULL &&
	    (cp-lp) < start_anchor->input_field->size - 1) {
	    LYFixCursesOn("ask for confirmation:");
	    erase();		/* don't show previous state */
	    if (HTConfirmDefault(gettext("Wrap lines to fit displayed area?"),
				 NO)) {
		wanted_fieldlen_wrap = start_anchor->input_field->size - 1;
	    } else {
		wanted_fieldlen_wrap = 0;
	    }
	}
	if (wanted_fieldlen_wrap > 0 && len0+len > wanted_fieldlen_wrap) {
	    for (i = wanted_fieldlen_wrap-len0;
		 i+len0 >= wanted_fieldlen_wrap/4; i--) {
		if (isspace(UCH(lp[i]))) {
		    len = i + 1;
		    cp = lp + i;
		    if (cp[1] != '\n' &&
			isspace(UCH(cp[1])) &&
			!isspace(UCH(cp[2]))) {
			len++;
			cp++;
		    }
		    if (!isspace(UCH(cp[1]))) {
			while (*cp && *cp != '\r' && *cp != '\n' &&
			       (cp - lp) <= len + (3 * wanted_fieldlen_wrap/4))
			    cp++;	/* search for next line break */
			if (*cp == '\r' && cp[1] == '\n')
			    cp++;
			if (*cp == '\n' &&
			    (cp[1] == '\r' || cp[1] == '\n' ||
			     !isspace(UCH(cp[1])))) {
			    *cp = ' ';
			    while (isspace(UCH(*(cp-1)))) {
				skip_num++;
				cp--;
			    }
			    skip_at = cp;
			}
		    }
		    break;
		}
	    }
	}
	if (wanted_fieldlen_wrap > 0 && len0+len > wanted_fieldlen_wrap) {
	    i = len-1;
	    while (len0+i+1 > wanted_fieldlen_wrap &&
		   isspace(UCH(lp[i])))
		i--;
	    if (len0+i+1 > wanted_fieldlen_wrap)
		len = wanted_fieldlen_wrap - len0;
	}

	if (len0+len >= MAX_LINE) {
	    if (!wrapalert) {
		LYFixCursesOn("show alert:");
		HTAlert(gettext("Very long lines have been wrapped!"));
		wrapalert = TRUE;
	    }
	    /*
	     *  First try to find a space character for wrapping - kw
	     */
	    for (i = MAX_LINE - len0 - 1; i > 0; i--) {
		if (isspace(UCH(lp[i]))) {
		    len = i;
		    break;
		}
	    }
	    if (len0+len >= MAX_LINE)
		len = MAX_LINE - len0 - 1;
	}

	strncat (line, lp, len);
	*(line + len0+len) = '\0';

	cleanup_line_for_textarea (line, len0+len);

	/*
	 *  If there are more lines in the edit buffer than were in the
	 *  original TEXTAREA, we need to add a new line/anchor, continuing
	 *  on until the edit buffer is empty.
	 */
	if (line_cnt > orig_cnt) {
	    insert_new_textarea_anchor (&end_anchor, &htline);
	    anchor_ptr = end_anchor;   /* make the new anchor current */
	    newlines++;
	}

	/*
	 *  Finally copy the new line from the edit buffer into the anchor.
	 */
	StrAllocCopy(anchor_ptr->input_field->value, line);

	/*
	 *  Keep track of 1st blank line in any trailing blank lines, for
	 *  later cursor repositioning.
	 */
	if (len0+len > 0)
	    exit_line = 0;
	else if (exit_line == 0)
	    exit_line = anchor_ptr->line_num;

	/*
	 *  And do the next line of edited text, for the next anchor ...
	 */
	lp += len;
	if (*lp && isspace(UCH(*lp))) lp++;

	end_anchor = anchor_ptr;
	anchor_ptr = anchor_ptr->next;

	if (anchor_ptr)
	    match_tag = anchor_ptr->number;

	line_cnt++;
    }

    CTRACE((tfp, "GridText: edited text inserted into lynx struct's\n"));

    /*
     *	If we've added any new lines/anchors, we need to adjust various
     *  things in all anchor-bearing lines following the last newly added
     *  line/anchor.  The fun stuff starts here ...
     */
    if (newlines > 0)
	update_subsequent_anchors (newlines, end_anchor, htline, match_tag);

    /*
     *  Cleanup time.
     */
    FREE(line);
    FREE(ebuf);
    LYRemoveTemp (ed_temp);
    FREE(ed_temp);

    CTRACE((tfp, "GridText: exiting HText_ExtEditForm()\n"));

    /*
     *  Return the offset needed to move the cursor from its current
     *  (on entry) line number, to the 1st blank line of the trailing
     *  (group of) blank line(s), which is where we want to be.  Let
     *  the caller deal with moving us there, however ... :-) ...
     */
    return (exit_line - entry_line);
}


/*
 *  Expand the size of a TEXTAREA by a fixed number of lines (as specified
 *  by arg2).
 *
 *  --KED  02/14/99
 */
PUBLIC void HText_ExpandTextarea ARGS2(
	    struct link *,  form_link,
	    int,            newlines)
{
    TextAnchor *anchor_ptr;
    TextAnchor *end_anchor    = NULL;
    BOOLEAN	firstanchor   = TRUE;

    FormInfo *form      = form_link->form;
    char     *areaname  = form->name;
    int       form_num  = form->number;

    HTLine   *htline    = NULL;

    int       match_tag = 0;
    int       i;


    CTRACE((tfp, "GridText: entered HText_ExpandTextarea()\n"));

    if (newlines < 1) return;

    /*
     *	Begin at the beginning, to find the TEXTAREA, then on to find
     *	the last line (anchor) in it.
     *
     *	[Finding the TEXTAREA we're actually *in* with these attributes
     *	 isn't foolproof.  The form_num isn't unique to a given TEXTAREA,
     *	 and there *could* be TEXTAREA's with the same "name".	If that
     *	 should ever be true, we'll actually expand the *1st* TEXTAREA
     *	 in the page that matches.  We should probably assign a unique
     *	 id to each TEXTAREA in a page, and match on that, to avoid this
     *	 (potential) problem.
     *
     *	 Since the odds of "false matches" *actually* happening in real
     *	 life seem rather small though, we'll hold off doing this, for a
     *	 rainy day ...]
     */
    anchor_ptr = HTMainText->first_anchor;

    while (anchor_ptr) {

	if ((anchor_ptr->link_type	     == INPUT_ANCHOR)    &&
	    (anchor_ptr->input_field->type   == F_TEXTAREA_TYPE) &&
	    (anchor_ptr->input_field->number == form_num)	 &&
	    !strcmp (anchor_ptr->input_field->name, areaname))   {

	    if (firstanchor)
		firstanchor = FALSE;

	    end_anchor = anchor_ptr;

	 } else {

	    if (!firstanchor)
		break;
	 }
	 anchor_ptr = anchor_ptr->next;
    }

    for (i = 1; i <= newlines; i++) {
	insert_new_textarea_anchor (&end_anchor, &htline);

	/*
	 *  Make the new line blank.
	 */
	StrAllocCopy(end_anchor->input_field->value, "");

	/*
	 *  And go add another line ...
	 */
	if (end_anchor->next)
	   match_tag = end_anchor->next->number;
    }

    CTRACE((tfp, "GridText: %d blank line(s) added to TEXTAREA name=|%s|\n",
		newlines, areaname));

    /*
     *	We need to adjust various things in all anchor bearing lines
     *	following the last newly added line/anchor.  Fun stuff.
     */
    update_subsequent_anchors (newlines, end_anchor, htline, match_tag);

    CTRACE((tfp, "GridText: exiting HText_ExpandTextarea()\n"));

    return;
}


/*
 *  Insert the contents of a file into a TEXTAREA between the cursorline,
 *  and the line preceeding it.
 *
 *  Returns the number of lines that the cursor should be moved so that it
 *  will end up on the 1st line in the TEXTAREA following the inserted file
 *  (if we decide to do that).
 *
 *  --KED  02/21/99
 */
PUBLIC int HText_InsertFile ARGS1(
	   struct link *,  form_link)
{
    struct stat stat_info;
    size_t	size;

    FILE       *fp;
    char       *fn;

    TextAnchor *anchor_ptr;
    TextAnchor *prev_anchor = NULL;
    TextAnchor *end_anchor  = NULL;
    BOOLEAN	firstanchor = TRUE;
    BOOLEAN truncalert = FALSE;

    FormInfo   *form	 = form_link->form;
    char       *areaname = form->name;
    int		form_num = form->number;

    HTLine     *htline	 = NULL;

    TextAnchor *a = 0;
    FormInfo   *f = 0;
    HTLine     *l = 0;

    char       *fbuf;
    char       *line;
    char       *lp;
    char       *cp;
    int		entry_line = form_link->anchor_line_num;
    int		file_cs;
    int		match_tag  = 0;
    int		newlines   = 0;
    int		len;
    int		i;


    CTRACE((tfp, "GridText: entered HText_InsertFile()\n"));

    /*
     * Get the filename of the insert file.
     */
    if (!(fn = GetFileName())) {
	HTInfoMsg (FILE_INSERT_CANCELLED);
	CTRACE((tfp, "GridText: file insert cancelled - no filename provided\n"));
	return (0);
    }
    if (no_dotfiles || !show_dotfiles) {
	if (*LYPathLeaf(fn) == '.') {
	    HTUserMsg(FILENAME_CANNOT_BE_DOT);
	    return (0);
	}
    }

    /*
     *	Read it into our buffer (abort on 0-length file).
     */
    if ((stat (fn, &stat_info) < 0)        ||
	((size = stat_info.st_size) == 0)) {
	HTInfoMsg (FILE_INSERT_0_LENGTH);
	CTRACE((tfp, "GridText: file insert aborted - file=|%s|- was 0-length\n",
		    fn));
	FREE(fn);
	return (0);

    } else {

	if ((fbuf = typecallocn(char, size + 1)) == NULL) {
	    /*
	     *  This could be huge - don't exit if we don't have enough
	     *  memory for it. - kw
	     */ /*outofmem(__FILE__, "HText_InsertFile");*/
	    free(fn);
	    HTAlert(MEMORY_EXHAUSTED_FILE);
	    return 0;
	}

	/* Try to make the same assumption for the charset of the inserted
	 * file as we would for normal loading of that file, i.e. taking
	 * assume_local_charset and suffix mappings into account.
	 * If there is a mismatch with the display character set, characters
	 * may be displayed wrong, too bad; but the user has a chance to
	 * correct this by editing the lines, which will update f->value_cs
	 * again. - kw
	 */
	LYGetFileInfo(fn, 0, 0, 0, 0, 0, &file_cs);

	fp   = fopen (fn, "r");
	if (!fp) {
	    free(fbuf);
	    free(fn);
	    HTAlert(FILE_CANNOT_OPEN_R);
	    return 0;
	}
	size = fread (fbuf, 1, size, fp);
	LYCloseInput (fp);
	FREE(fn);
	fbuf[size] = '\0';	/* Terminate! - kw */
    }


    /*
     *	Begin at the beginning, to find the TEXTAREA we're in, then
     *	the current cursorline.
     *
     *	[Finding the TEXTAREA we're actually *in* with these attributes
     *	 isn't foolproof.  The form_num isn't unique to a given TEXTAREA,
     *	 and there *could* be TEXTAREA's with the same "name".	If that
     *	 should ever be true, we'll actually insert data into the *1st*
     *	 TEXTAREA in the page that matches.  We should probably assign
     *	 a unique id to each TEXTAREA in a page, and match on that, to
     *	 avoid this (potential) problem.
     *
     *	 Since the odds of "false matches" *actually* happening in real
     *	 life seem rather small though, we'll hold off doing this, for a
     *	 rainy day ...]
     */
    anchor_ptr = HTMainText->first_anchor;

    while (anchor_ptr) {

	if ((anchor_ptr->link_type	     == INPUT_ANCHOR)    &&
	    (anchor_ptr->input_field->type   == F_TEXTAREA_TYPE) &&
	    (anchor_ptr->input_field->number == form_num)	 &&
	    !strcmp (anchor_ptr->input_field->name, areaname))   {

	   if (anchor_ptr->line_num == entry_line)
	      break;
	}
	prev_anchor = anchor_ptr;
	anchor_ptr  = anchor_ptr->next;
    }


    /*
     *  Clone a new TEXTAREA line/anchor using the cursorline anchor as
     *  a template, but link it in BEFORE the cursorline anchor/htline.
     *
     *  [We can probably combine this with insert_new_textarea_anchor()
     *   along with a flag to indicate "insert before" as we do here,
     *   or the "normal" mode of operation (add after "current" anchor/
     *   line).  Beware of the differences ... some are a bit subtle to
     *   notice.]
     */
    for (htline = HTMainText->last_line->next, i = 0;
	 anchor_ptr->line_num != i;            i++) {
	htline = htline->next;
	if (htline == HTMainText->last_line)
	    break;
    }

    if (((a = typecalloc(TextAnchor)) == 0) ||
	((f = typecalloc(FormInfo)) == 0) ||
	((l = allocHTLine(MAX_LINE)) == 0))
	outofmem(__FILE__, "HText_InsertFile");

    /*  Init all the fields in the new TextAnchor.                 */
    /*  [anything "special" needed based on ->show_anchor value ?] */
    a->next	       = anchor_ptr;
    a->number	       = anchor_ptr->number;
    a->start	       = anchor_ptr->start;
    a->line_pos	       = anchor_ptr->line_pos;
    a->extent	       = anchor_ptr->extent;
    a->line_num	       = anchor_ptr->line_num;
    StrAllocCopy (a->hightext,  anchor_ptr->hightext);
    StrAllocCopy (a->hightext2, anchor_ptr->hightext2);
    a->hightext2offset = anchor_ptr->hightext2offset;
    a->link_type       = anchor_ptr->link_type;
    a->input_field     = f;
    a->show_anchor     = anchor_ptr->show_anchor;
    a->inUnderline     = anchor_ptr->inUnderline;
    a->expansion_anch  = TRUE;
    a->anchor	       = NULL;

    /*  Just the (seemingly) relevant fields in the new FormInfo.  */
    /*  [do we need to do anything "special" based on ->disabled]  */
    StrAllocCopy (f->name, anchor_ptr->input_field->name);
    f->number	       = anchor_ptr->input_field->number;
    f->type	       = anchor_ptr->input_field->type;
    StrAllocCopy (f->orig_value, "");
    f->size	       = anchor_ptr->input_field->size;
    f->maxlength       = anchor_ptr->input_field->maxlength;
    f->no_cache        = anchor_ptr->input_field->no_cache;
    f->disabled        = anchor_ptr->input_field->disabled;
    f->value_cs        = (file_cs >= 0) ? file_cs : current_char_set;

    /*  Init all the fields in the new HTLine (but see the #if).   */
    l->offset	       = htline->offset;
    l->size	       = htline->size;
    l->split_after     = htline->split_after;
    l->bullet	       = htline->bullet;
    l->expansion_line  = TRUE;
#if defined(USE_COLOR_STYLE)
    /* dup styles[] if needed [no need in TEXTAREA (?); leave 0's] */
    l->numstyles       = htline->numstyles;
#ifndef OLD_HTSTYLECHANGE
    /*we fork the pointers!*/
    l->styles = htline->styles;
#endif
#endif
    strcpy (l->data,     htline->data);

    /*
     *  If we're at the head of the TextAnchor list, the new node becomes
     *  the first node.
     */
    if (anchor_ptr == HTMainText->first_anchor)
	HTMainText->first_anchor = a;

    /*
     *  Link in the new TextAnchor, and corresponding HTLine.
     */
    if (prev_anchor)
	prev_anchor->next = a;

    htline             = htline->prev;
    l->next	       = htline->next;
    l->prev	       = htline;
    htline->next->prev = l;
    htline->next       = l;

    /*
     *  update_subsequent_anchors() expects htline to point to 1st potential
     *  line needing fixup; we need to do this just in case the inserted file
     *  was only a single line (yes, it's pathological ... ).
     */
    htline = htline->next; /* ->new (current) htline, for 1st inserted line  */
    htline = htline->next; /* ->1st potential (following) [tag] fixup htline */

    anchor_ptr = a;
    newlines++;


    /*
     *	Copy each line from the insert file into the corresponding anchor
     *  struct.
     *
     *  Begin with the new line/anchor we just added (above the cursorline).
     */
    if ((line = (char *) malloc (MAX_LINE)) == 0)
	outofmem(__FILE__, "HText_InsertFile");

    match_tag = anchor_ptr->number;

    len = 0;
    lp  = fbuf;

    while (*lp) {

	if ((cp = strchr (lp, '\n')) != 0)
	   len = cp - lp;
	else
	   len = strlen (lp);

	if (len >= MAX_LINE) {
	    if (!truncalert) {
		HTAlert(gettext("Very long lines have been truncated!"));
		truncalert = TRUE;
	    }
	    len = MAX_LINE - 1;
	    if (lp[len])
		lp[len+1] = '\0'; /* prevent next iteration */
	}
	strncpy (line, lp, len);
	*(line + len) = '\0';

	cleanup_line_for_textarea (line, len);

	/*
	 *  If not the first line from the insert file, we need to add
	 *  a new line/anchor, continuing on until the buffer is empty.
	 */
	if (!firstanchor) {
	   insert_new_textarea_anchor (&end_anchor, &htline);
	   anchor_ptr = end_anchor;   /* make the new anchor current */
	   newlines++;
	}

	/*
	 *  Copy the new line from the buffer into the anchor.
	 */
	StrAllocCopy(anchor_ptr->input_field->value, line);

	/*
	 *  insert_new_textarea_anchor always uses current_char_set,
	 *  we may want something else, so fix it up. - kw
	 */
	 if (file_cs >= 0)
	     anchor_ptr->input_field->value_cs = file_cs;

	/*
	 *  And do the next line of insert text, for the next anchor ...
	 */
	lp += len;
	if (*lp) lp++;

	firstanchor = FALSE;
	end_anchor  = anchor_ptr;
	anchor_ptr  = anchor_ptr->next;
    }

    CTRACE((tfp, "GridText: file inserted into lynx struct's\n"));

    /*
     *	Now adjust various things in all anchor-bearing lines following the
     *  last newly added line/anchor.  Some say this is the fun part ...
     */
    update_subsequent_anchors (newlines, end_anchor, htline, match_tag);


    /*
     *  Cleanup time.
     */
    FREE(line);
    FREE(fbuf);

    CTRACE((tfp, "GridText: exiting HText_InsertFile()\n"));

    return (newlines);
}

/*
 * This function draws the part of line 'line', pointed by 'str' (which can be
 * non terminated with null - i.e., is line->data+N) drawing 'len' bytes (not
 * characters) of it.  It doesn't check whether the 'len' bytes crosses a
 * character boundary (if multibyte chars are in string).  Assumes that the
 * cursor is positioned in the place where the 1st char of string should be
 * drawn.  Currently used only in redraw_lines_of_link when
 *    defined(USE_COLOR_STYLE) && !defined(NO_HILIT_FIX)
 * This code is based on display_line.  This code was tested with ncurses only
 * (since no support for lss is availble for Slang) and with
 * defined(USE_COLOR_STYLE).
 * -HV.
 */
#if defined(USE_COLOR_STYLE) && !defined(NO_HILIT_FIX)
PRIVATE void redraw_part_of_line ARGS4(
	HTLine *,	line,
	char*,		str,
	int,		len,
	HText *,	text)
{
    register int i;
    char buffer[7];
    char *data,*end_of_data;
    size_t utf_extra = 0;
#ifdef USE_COLOR_STYLE
    int current_style = 0;
#endif
    char LastDisplayChar = ' ';
    int YP,XP;

    LYGetYX(YP,XP);

    i = XP;

    /* Set up the multibyte character buffer  */
    buffer[0] = buffer[1] = buffer[2] = '\0';

    data = str;
    end_of_data = data + len;
    i++;

    /* this assumes that the part of line to be drawn fits in the screen*/
    while (  data < end_of_data ) {
	buffer[0] = *data;
	data++;

#if defined(USE_COLOR_STYLE) || defined(SLSC)
#define CStyle line->styles[current_style]

	while (current_style < line->numstyles &&
	       i >= (int) (CStyle.horizpos + line->offset + 1))
	{
		LynxChangeStyle (CStyle.style,CStyle.direction,CStyle.previous);
		current_style++;
	}
#endif
	switch (buffer[0]) {

#ifndef USE_COLOR_STYLE
	    case LY_UNDERLINE_START_CHAR:
		if (dump_output_immediately && use_underscore) {
		    addch('_');
		    i++;
		} else {
		    start_underline();
		}
		break;

	    case LY_UNDERLINE_END_CHAR:
		if (dump_output_immediately && use_underscore) {
		    addch('_');
		    i++;
		} else {
		    stop_underline();
		}
		break;

	    case LY_BOLD_START_CHAR:
		start_bold();
		break;

	    case LY_BOLD_END_CHAR:
		stop_bold ();
		break;

#endif
	    case LY_SOFT_NEWLINE:
		if (!dump_output_immediately)
		    addch('+');
		break;

	    case LY_SOFT_HYPHEN:
		if (*data != '\0' ||
		    isspace(UCH(LastDisplayChar)) ||
		    LastDisplayChar == '-') {
		    /*
		     *  Ignore the soft hyphen if it is not the last
		     *  character in the line.  Also ignore it if it
		     *  first character following the margin, or if it
		     *  is preceded by a white character (we loaded 'M'
		     *  into LastDisplayChar if it was a multibyte
		     *  character) or hyphen, though it should have
		     *  been excluded by HText_appendCharacter() or by
		     *  split_line() in those cases. - FM
		     */
		    break;
		} else {
		    /*
		     *  Make it a hard hyphen and fall through. - FM
		     */
		    buffer[0] = '-';
		    i++;
		}
		/* FALLTHRU */

	    default:
		i++;
		if (text->T.output_utf8 && !isascii(UCH(buffer[0]))) {
		    if ((*buffer & 0xe0) == 0xc0) {
			utf_extra = 1;
		    } else if ((*buffer & 0xf0) == 0xe0) {
			utf_extra = 2;
		    } else if ((*buffer & 0xf8) == 0xf0) {
			utf_extra = 3;
		    } else if ((*buffer & 0xfc) == 0xf8) {
			utf_extra = 4;
		    } else if ((*buffer & 0xfe) == 0xfc) {
			utf_extra = 5;
		    } else {
			 /*
			  *  Garbage.
			  */
			utf_extra = 0;
		    }
		    if (strlen(data) < utf_extra) {
			/*
			 *  Shouldn't happen.
			 */
			utf_extra = 0;
		    }
		    LastDisplayChar = 'M';
		}
		if (utf_extra) {
		    strncpy(&buffer[1], data, utf_extra);
		    buffer[utf_extra+1] = '\0';
		    addstr(buffer);
		    buffer[1] = '\0';
		    data += utf_extra;
		    utf_extra = 0;
		} else if (HTCJK != NOCJK && !isascii(UCH(buffer[0]))) {
		    /*
		     *  For CJK strings, by Masanobu Kimura.
		     */
		    buffer[1] = *data;
		    data++;
		    addstr(buffer);
		    buffer[1] = '\0';
		    /*
		     *  For now, load 'M' into LastDisplayChar,
		     *  but we should check whether it's white
		     *  and if so, use ' '.  I don't know if
		     *  there actually are white CJK characters,
		     *  and we're loading ' ' for multibyte
		     *  spacing characters in this code set,
		     *  but this will become an issue when
		     *  the development code set's multibyte
		     *  character handling is used. - FM
		     */
		    LastDisplayChar = 'M';
		} else {
#if 0	/* last-ditch attempt to prevent 0x9B to screen - disabled  */
#if defined(UNIX) || defined(VMS)
		    if (!dump_output_immediately &&
			UCH(buffer[0]) == 128+27) {
			addstr("~^");
			buffer[0] ^= 0xc0;
		    }
#endif
#endif
		    addstr(buffer);
		    LastDisplayChar = buffer[0];
		}
	} /* end of switch */
    } /* end of while */

#ifndef USE_COLOR_STYLE
    stop_underline();
    stop_bold();
#else

    while (current_style < line->numstyles)
    {
	LynxChangeStyle (CStyle.style, CStyle.direction, CStyle.previous);
	current_style++;
    }

#undef CStyle
#endif
    return;
}
#endif /* defined(USE_COLOR_STYLE) && !defined(NO_HILIT_FIX)  */

#ifndef USE_COLOR_STYLE
/*
 *  Function move_to_glyph is called from LYMoveToLink and does all
 *  the real work for it.
 *  The pair LYMoveToLink()/move_to_glyph() is similar to the pair
 *  redraw_lines_of_link()/redraw_part_of_line(), some key differences:
 *   LYMoveToLink/move_to_glyph		redraw_*
 *   -----------------------------------------------------------------
 *   - used without color style         - used with color style
 *   - handles showing WHEREIS target	- WHEREIS handled elsewhere
 *   - handles only one line		- handles first two lines for
 *					  hypertext anchors
 *   - right columns position for UTF-8
 *     by redrawing as necessary
 *   - currently used for highlight	- currently used for highlight
 *     ON and OFF			  OFF
 *
 *  Eventually the two sets of function should be unified, and should handle
 *  UTF-8 positioning, both lines of hypertext anchors, and WHEREIS in all
 *  cases.  If possible.  The complex WHEREIS target logic in highlight()
 *  could then be completely removed. - kw
 */
PRIVATE void move_to_glyph ARGS10(
	int,		YP,
	int,		XP,
	int,		XP_draw_min,
	char *,		data,
	int,		datasize,
	unsigned,	offset,
	CONST char *,	target,
	char *,		hightext,
	int,		flags,
	BOOL,		utf_flag)
{
    register int i;
    char buffer[7];
    CONST char *end_of_data;
    size_t utf_extra = 0;
#if defined(SHOW_WHEREIS_TARGETS)
    CONST char *cp_tgt;
    int i_start_tgt=0, i_after_tgt;
    int HitOffset, LenNeeded;
#endif /* SHOW_WHEREIS_TARGETS */
    BOOL intarget = NO, inunderline = NO, inbold = NO;
    BOOL drawing = NO, inU = NO, hadutf8 = NO;
    BOOL incurlink = NO, drawingtarget = NO, flag = NO;
    char *sdata = data;
    char LastDisplayChar = ' ';
    int XP_link = XP;
    int linkvlen;

    int	len;

    if (flags & 1)
	flag = YES;
    if (flags & 2)
	inU = YES;
    /* Set up the multibyte character buffer  */
    buffer[0] = buffer[1] = buffer[2] = '\0';
    /*
     *  Add offset, making sure that we do not
     *  go over the COLS limit on the display.
     */
    i = (int)offset;
    if (i > (int)LYcols - 1)
	i = (int)LYcols - 1;

    linkvlen = hightext ? LYmbcsstrlen(hightext, utf_flag, YES) : 0;

    /*
     *  Scan through the data, making sure that we do not
     *  go over the COLS limit on the display etc.
     */
    len = datasize;
    end_of_data = data + len;

#if defined(SHOW_WHEREIS_TARGETS)
    /*
     *  If the target overlaps with the part of this line that
     *  we are drawing, it will be emphasized.
     */
    i_after_tgt = i;
    if (target) {
	if (case_sensitive)
	    cp_tgt = LYno_attr_mbcs_strstr(sdata,
					   target,
					   utf_flag, YES,
					   &HitOffset,
					   &LenNeeded);
	else
	    cp_tgt = LYno_attr_mbcs_case_strstr(sdata,
						target,
						utf_flag, YES,
						&HitOffset,
						&LenNeeded);
	if (cp_tgt) {
	    if ((int)offset + LenNeeded >= LYcols ||
		((int)offset + HitOffset >= XP + linkvlen)) {
		cp_tgt = NULL;
	    } else {
		i_start_tgt = i + HitOffset;
		i_after_tgt = i + LenNeeded;
	    }
	}
    } else {
	cp_tgt = NULL;
    }
#endif /* SHOW_WHEREIS_TARGETS */


    /*
     *  Iterate through the line data from the start, keeping track of
     *  the display ("glyph") position in i.  Drawing will be turned
     *  on when either the first UTF-8 sequence (that occurs after
     *  XP_draw_min) is found, or when we reach the link itself (if
     *  highlight is non-NULL). - kw
     */
    while ((i < LYcols - 1) && data < end_of_data && (*data != '\0')) {

	if (data && hightext && i >= XP && !incurlink) {

	/*
	 *  We reached the position of link itself, and hightext is
	 *  non-NULL.  We switch data from being a pointer into the HTLine
	 *  to being a pointer into hightext.  Normally (as long as this
	 *  routine is applied to normal hyperlink anchors) the text in
	 *  hightext will be identical to that part of the HTLine that
	 *  data was already pointing to, except that special attribute
	 *  chars LY_BOLD_START_CHAR etc. have been stripped out (see
	 *  HText_trimHightext).  So the switching should not result in
	 *  any different display, but it ensures that it doesn't go
	 *  unnoticed if somehow hightext got messed up somewhere else.
	 *  This is also useful in preparation for using this function
	 *  for something else than normal hyperlink anchors, i.e. form
	 *  fields.
	 *  Turn on drawing here or make sure it gets turned on before the
	 *  next actual normal character is handled. - kw
	 */
	    data = hightext;
	    len = strlen(hightext);
	    end_of_data = hightext + len;
	    XP += linkvlen;	/* from now on XP includes hightext chars */
	    incurlink = YES;
#ifdef SHOW_WHEREIS_TARGETS
	    if (cp_tgt) {
		if (flag && i_after_tgt >= XP)
		    i_after_tgt = XP - 1;
	    }
#endif
	    /*
	     *  The logic of where to set intarget drawingtarget etc.
	     *  and when to react to it should be cleaned up (here and
	     *  further below).  For now this seems to work but isn't
	     *  very clear.  The complications arise from reproducing
	     *  the behavior (previously done in highlight()) for target
	     *  strings that fall into or overlap a link: use target
	     *  emphasis for the target string, except for the first
	     *  and last character of the anchor text if the anchor is
	     *  highlighted as "current link". - kw
	     */
	    if (!drawing) {
#ifdef SHOW_WHEREIS_TARGETS
		if (intarget) {
		    if (i_after_tgt > i) {
			move(YP, i);
			if (flag) {
			    drawing = YES;
			    drawingtarget = NO;
			    if (inunderline)	inU = YES;
			    lynx_start_link_color (flag, inU);
			} else {
			    drawing = YES;
			    drawingtarget = YES;
			    LYstartTargetEmphasis();
			}
		    }
#if 0
		} else {
		    if (inunderline)	inU = YES;
		    lynx_start_link_color (flag, inU);
#endif
		}
#endif /* SHOW_WHEREIS_TARGETS */
	    } else {
#ifdef SHOW_WHEREIS_TARGETS
		if (intarget && i_after_tgt > i) {
		    if (flag && (data == hightext)) {
			drawingtarget = NO;
			LYstopTargetEmphasis();
		    }
		} else if (!intarget)
#endif /* SHOW_WHEREIS_TARGETS */
		{
		    if (inunderline)	inU = YES;
		    if (inunderline)	stop_underline();
		    if (inbold)		stop_bold();
		    lynx_start_link_color (flag, inU);
		}

	    }
	}
	if (i >= XP || data >= end_of_data)
	    break;
	if ((buffer[0] = *data) == '\0')
	    break;


#if defined(SHOW_WHEREIS_TARGETS)
	/*
	 *  Look for a subsequent occurrence of the target string,
	 *  if we had a previous one and have now stepped past it. - kw
	 */
	if (cp_tgt && i >= i_after_tgt) {
	    if (intarget) {

		if (incurlink && flag && i == XP - 1)
		    cp_tgt = NULL;
		else if (case_sensitive)
		    cp_tgt = LYno_attr_mbcs_strstr(sdata,
						   target,
						   utf_flag, YES,
						   &HitOffset,
						   &LenNeeded);
		else
		    cp_tgt = LYno_attr_mbcs_case_strstr(sdata,
							target,
							utf_flag, YES,
							&HitOffset,
							&LenNeeded);
		if (cp_tgt) {
		    i_start_tgt = i + HitOffset;
		    i_after_tgt = i + LenNeeded;
		    if (incurlink) {
			if (flag && i_start_tgt == XP_link)
			    i_start_tgt++;
			if (flag && i_start_tgt == XP - 1)
			    i_start_tgt++;
			if (flag && i_after_tgt >= XP)
			    i_after_tgt = XP - 1;
			if (flag && i_start_tgt >= XP)
			    cp_tgt = NULL;
		    } else if (i_start_tgt == XP) {
			if (flag)
			    i_start_tgt++;
		    }
		}
		if (!cp_tgt || i_start_tgt != i) {
		    intarget = NO;
		    if (drawing) {
			if (drawingtarget) {
			    drawingtarget = NO;
			    LYstopTargetEmphasis();
			    if (incurlink) {
				lynx_start_link_color (flag, inU);
			    }
			}
			if (!incurlink) {
			    if (inbold)		start_bold();
			    if (inunderline)	start_underline();
			}
		    }
		}
	    }
	}
#endif /* SHOW_WHEREIS_TARGETS */

	/*
	 *  Advance data to point to the next input char (for the
	 *  next round).  Advance sdata, used for searching for a
	 *  target string, so that they stays in synch.  As long
	 *  as we are not within the highlight text, data and sdata
	 *  have identical values.  After we have switched data to
	 *  point into hightext, sdata remains a pointer into the
	 *  HTLine (so that we don't miss a partial target match at
	 *  the end of the anchor text).  So sdata has to sometimes
	 *  skip additional special attribute characters that are
	 *  not present in highlight in order to stay in synch. - kw
	 */
	data++;
	if (*sdata) {
	    do sdata++;
		while (incurlink && *sdata && sdata != data &&
		       IsSpecialAttrChar(*(sdata-1)));
	}

	switch (buffer[0]) {

	    case LY_UNDERLINE_START_CHAR:
		if (!drawing || !incurlink) inunderline = YES;
		if (drawing && !intarget && !incurlink)
		    start_underline();
		break;

	    case LY_UNDERLINE_END_CHAR:
		inunderline = NO;
		if (drawing && !intarget && !incurlink)
		    stop_underline();
		break;

	    case LY_BOLD_START_CHAR:
		if (!drawing || !incurlink) inbold = YES;
		if (drawing && !intarget && !incurlink)
		    start_bold();
		break;

	    case LY_BOLD_END_CHAR:
		inbold = NO;
		if (drawing && !intarget && !incurlink)
		    stop_bold();
		break;

	    case LY_SOFT_NEWLINE:
		if (drawing) {
		    addch('+');
		}
		i++;
		break;

	    case LY_SOFT_HYPHEN:
		if (*data != '\0' ||
		    isspace(UCH(LastDisplayChar)) ||
		    LastDisplayChar == '-') {
		    /*
		     *  Ignore the soft hyphen if it is not the last
		     *  character in the line.  Also ignore it if it
		     *  first character following the margin, or if it
		     *  is preceded by a white character (we loaded 'M'
		     *  into LastDisplayChar if it was a multibyte
		     *  character) or hyphen, though it should have
		     *  been excluded by HText_appendCharacter() or by
		     *  split_line() in those cases. - FM
		     */
		    break;
		} else {
		    /*
		     *  Make it a hard hyphen and fall through. - FM
		     */
		    buffer[0] = '-';
		}
		/* FALLTHRU */

	    default:
		/*
		 *  We have got an actual normal displayable character, or
		 *  the start of one.  Before proceeding check whether
		 *  drawing needs to be turned on now. - kw
		 */
#if defined(SHOW_WHEREIS_TARGETS)
		if (incurlink && intarget && flag && i_after_tgt > i) {
		    if (i == XP - 1) {
			i_after_tgt = i;
		    } else if (i == XP - 2 && HTCJK != NOCJK &&
			       !isascii(UCH(buffer[0]))) {
			i_after_tgt = i;
			cp_tgt = NULL;
			if (drawing) {
			    if (drawingtarget) {
				LYstopTargetEmphasis();
				drawingtarget = NO;
				lynx_start_link_color (flag, inU);
			    }
			}
		    }
		}
		if (cp_tgt && i >= i_start_tgt && sdata > cp_tgt) {
		    if (!intarget ||
			(intarget && incurlink && !drawingtarget)) {

			if (incurlink && drawing &&
			    !(flag &&
			      (i == XP_link || i == XP - 1))) {
			    lynx_stop_link_color (flag, inU);
			}
			if (incurlink && !drawing) {
			    move(YP, i);
			    if (inunderline)	inU = YES;
			    if (flag && (i == XP_link || i == XP - 1)) {
				lynx_start_link_color (flag, inU);
				drawingtarget = NO;
			    } else {
				LYstartTargetEmphasis();
				drawingtarget = YES;
			    }
			    drawing = YES;
			} else if (incurlink && drawing &&
				   intarget && !drawingtarget &&
				   (flag &&
				    (i == XP_link))) {
			    if (inunderline)	inU = YES;
			    lynx_start_link_color (flag, inU);
			} else if (drawing &&
				   !(flag &&
				     (i == XP_link || (incurlink && i == XP - 1)))) {
			    LYstartTargetEmphasis();
			    drawingtarget = YES;
			}
			intarget = YES;
		    }
		} else
#endif /* SHOW_WHEREIS_TARGETS */
		    if (incurlink) {
			if (!drawing) {
			    move(YP, i);
			    if (inunderline)	inU = YES;
			    lynx_start_link_color (flag, inU);
			    drawing = YES;
			}
		    }

		i++;
		if (utf_flag && !isascii(UCH(buffer[0]))) {
		    hadutf8 = YES;
		    if ((*buffer & 0xe0) == 0xc0) {
			utf_extra = 1;
		    } else if ((*buffer & 0xf0) == 0xe0) {
			utf_extra = 2;
		    } else if ((*buffer & 0xf8) == 0xf0) {
			utf_extra = 3;
		    } else if ((*buffer & 0xfc) == 0xf8) {
			utf_extra = 4;
		    } else if ((*buffer & 0xfe) == 0xfc) {
			utf_extra = 5;
		    } else {
			 /*
			  *  Garbage.
			  */
			utf_extra = 0;
		    }
		    if (strlen(data) < utf_extra) {
			/*
			 *  Shouldn't happen.
			 */
			utf_extra = 0;
		    }
		    LastDisplayChar = 'M';
		}
		if (utf_extra) {
		    strncpy(&buffer[1], data, utf_extra);
		    buffer[utf_extra+1] = '\0';
		    if (!drawing && i >= XP_draw_min) {
			move(YP, i - 1);
			drawing = YES;
#if defined(SHOW_WHEREIS_TARGETS)
			if (intarget) {
			    drawingtarget = YES;
			    LYstartTargetEmphasis();
			} else
#endif /* SHOW_WHEREIS_TARGETS */
			{
			    if (inbold)
				start_bold();
			    if (inunderline)
				start_underline();
			}
		    }
		    addstr(buffer);
		    buffer[1] = '\0';
		    sdata += utf_extra; data += utf_extra;
		    utf_extra = 0;
		} else if (HTCJK != NOCJK && !isascii(UCH(buffer[0]))) {
		    /*
		     *  For CJK strings, by Masanobu Kimura.
		     */
		    if (drawing && (i < LYcols - 1)) {
			buffer[1] = *data;
			addstr(buffer);
			buffer[1] = '\0';
		    }
		    i++;
		    sdata++; data++;
		    /*
		     *  For now, load 'M' into LastDisplayChar,
		     *  but we should check whether it's white
		     *  and if so, use ' '.  I don't know if
		     *  there actually are white CJK characters,
		     *  and we're loading ' ' for multibyte
		     *  spacing characters in this code set,
		     *  but this will become an issue when
		     *  the development code set's multibyte
		     *  character handling is used. - FM
		     */
		    LastDisplayChar = 'M';
		} else {
		    if (drawing) {
#if 0	/* last-ditch attempt to prevent 0x9B to screen - disabled  */
#if defined(UNIX) || defined(VMS)
			if (!dump_output_immediately &&
			    UCH(buffer[0]) == 128+27) {
			    addstr("~^");
			    buffer[0] ^= 0xc0;
			}
#endif
#endif
			addstr(buffer);
		    }
		    LastDisplayChar = buffer[0];
		}
	} /* end of switch */
    } /* end of while */

    if (!drawing) {
	move(YP, i);
	lynx_start_link_color (flag, inU);
    } else {
#if defined(SHOW_WHEREIS_TARGETS)
	if (drawingtarget) {
	    LYstopTargetEmphasis();
	    lynx_start_link_color (flag, inU);
	}
#endif /* SHOW_WHEREIS_TARGETS */
	if (hadutf8) {
	    LYtouchline(YP);
	}
    }
    return;
}
#endif /* !USE_COLOR_STYLE */

#ifndef USE_COLOR_STYLE
/*
 *  Move cursor position to a link's place in the display.
 *  The "moving to" is done by scanning through the line's
 *  character data in the corresponding HTLine of HTMainText,
 *  and starting to draw when a UTF-8 encoded non-ASCII character
 *  is encountered before the link (with some protection against
 *  overwriting form fields).  This refreshing of preceding data is
 *  necessary for preventing curses's or slang's display logic from
 *  getting too clever; their logic counts character positions wrong
 *  since they don't know about multi-byte characters that take up
 *  only one screen position.  So we have to make them forget their
 *  idea of what's in a screen line drawn previously.
 *  If hightext is non-NULL, it should be the anchor text for a normal
 *  link as stored in a links[] element, and the anchor text will be
 *  drawn too, with appropriate attributes. - kw
 */
PUBLIC void LYMoveToLink ARGS6(
	int,		cur,
	CONST char *,	target,
	char *,		hightext,
	int,		flag,
	BOOL,		inU,
	BOOL,		utf_flag)
{
#define pvtTITLE_HEIGHT 1
    HTLine* todr;
    int i, n=0;
    int XP_draw_min = 0;
    int flags = ((flag == ON) ? 1 : 0) | (inU ? 2 : 0);

    /*
     *  We need to protect changed form text fields preceding this
     *  link on the same line against overwriting. - kw
     */
    for (i = cur-1; i >= 0; i++) {
	if (links[i].ly < links[cur].ly)
	    break;
	if (links[i].type == WWW_FORM_LINK_TYPE) {
	    XP_draw_min = links[i].ly + links[i].form->size;
	    break;
	}
    }

    /*  Find the right HTLine. */
    if (!HTMainText) {
	todr = NULL;
    } else if (HTMainText->stale) {
	todr = HTMainText->last_line->next;
	n = links[cur].ly - pvtTITLE_HEIGHT + HTMainText->top_of_screen;
    } else {
	todr = HTMainText->top_of_screen_line;
	n = links[cur].ly - pvtTITLE_HEIGHT;
    }
    for (i = 0; i < n && todr; i++) {
	todr = (todr == HTMainText->last_line) ? NULL : todr->next;
    }
    if (todr) {
	if (target && *target == '\0') target = NULL;
	move_to_glyph(links[cur].ly, links[cur].lx, XP_draw_min,
		      todr->data, todr->size, todr->offset,
		      target, hightext, flags, utf_flag);
    } else {
	/*  This should not happen. */
	move_to_glyph(links[cur].ly, links[cur].lx, XP_draw_min,
		      "", 0, links[cur].lx,
		      target, hightext, flags, utf_flag);
	/* move(links[cur].ly, links[cur].lx); */
    }
}
#endif /* !USE_COLOR_STYLE */

/*
  This is used only if compiled with lss support. It's called to draw
  regular link (1st two lines of link) when it's being unhighlighted in
  highlight:LYUtils.
*/

PUBLIC void redraw_lines_of_link ARGS1(
	int,		cur GCC_UNUSED)
{
#if defined(USE_COLOR_STYLE) && !defined(NO_HILIT_FIX)
#define pvtTITLE_HEIGHT 1
    HTLine* todr1, *todr2;
    int lines_back;

    if (HTMainText->next_line == HTMainText->last_line) {
    /* we are at the last page - that is partially filled */
	lines_back = HTMainText->Lines - ( links[cur].ly-pvtTITLE_HEIGHT+
	HTMainText->top_of_screen);
    } else {
	lines_back = display_lines - (links[cur].ly-pvtTITLE_HEIGHT);
    }
    todr1 = HTMainText->next_line;
    while (lines_back-- > 0)
	todr1 = todr1->prev;
    todr2 = (links[cur].hightext2 && links[cur].ly < display_lines) ?
	    todr1->next : 0;

    move(links[cur].ly,  links[cur].lx);
    redraw_part_of_line (todr1, links[cur].hightext,
			 strlen(links[cur].hightext),  HTMainText);
    if (todr2) {
	move(links[cur].ly+1,links[cur].hightext2_offset);
	redraw_part_of_line (todr2, links[cur].hightext2,
			     strlen(links[cur].hightext2),  HTMainText);
    }

#undef pvtTITLE_HEIGHT
#else
    /* no dead code !*/
#endif
    return;
}

#ifdef USE_PRETTYSRC
PUBLIC void HTMark_asSource NOARGS
{
    if (HTMainText)
	HTMainText->source = TRUE;
}
#endif

PUBLIC HTkcode HText_getKcode ARGS1(
	HText *,	text)
{
    return text->kcode;
}

PUBLIC void HText_updateKcode ARGS2(
	HText *,	text,
	HTkcode,	kcode)
{
    text->kcode = kcode;
}

PUBLIC HTkcode HText_getSpecifiedKcode ARGS1(
	HText *,	text)
{
    return text->specified_kcode;
}

PUBLIC void HText_updateSpecifiedKcode ARGS2(
	HText *,	text,
	HTkcode,	kcode)
{
    text->specified_kcode = kcode;
}

PUBLIC int HTMainText_Get_UCLYhndl NOARGS
{
    return (HTMainText ?
	    HTAnchor_getUCLYhndl(HTMainText->node_anchor, UCT_STAGE_MIME)
	    : -1);
}
