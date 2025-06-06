#if !defined(__ATTRLIST_H)
#define __ATTRLIST_H

#include <HText.h>
#include <HTMLDTD.h>

enum {
 ABS_OFF = 0,
 STACK_OFF = 0,
 STACK_ON,
 ABS_ON
};

#define STARTAT 8

enum {
 DSTYLE_LINK = HTML_A+STARTAT,
 DSTYLE_STATUS = HTML_ELEMENTS+STARTAT,
 DSTYLE_ALINK,		/* active link */
 DSTYLE_NORMAL,		/* default attributes */
 DSTYLE_OPTION, 	/* option on the option screen */
 DSTYLE_VALUE, 		/* value on the option screen */
 DSTYLE_HIGH,
 DSTYLE_CANDY,		/* possibly going to vanish */
 DSTYLE_WHEREIS,	/* whereis search target */
 DSTYLE_ELEMENTS
};

enum {
 MSTYLE_NORMAL = 0,
 MSTYLE_EM = 1,
 MSTYLE_STRONG = 2,
 MSTYLE_PHYSICAL = 3,
 MSTYLE_A = 4,
 MSTYLE_A_OFF = 4,
 MSTYLE_A_ON,
 MSTYLE_BOLD,
 MSTYLE_UL,
 MSTYLE_STATUS,
 MSTYLE_CANDY,
 MSTYLE_NOMORE
};

typedef struct {
 int color; /* color highlighting to be done */
 int mono; /* mono highlighting to be done */
 int cattr; /* attributes to go with the color */
} HTCharStyle;

#define HText_characterStyle CTRACE((tfp,"HTC called from %s/%d\n",__FILE__,__LINE__));_internal_HTC

#undef HText_characterStyle
#define HText_characterStyle _internal_HTC

#if defined(USE_COLOR_STYLE)
extern void _internal_HTC PARAMS((HText * text, int style, int dir));
#define TEMPSTRINGSIZE 256
extern char class_string[TEMPSTRINGSIZE];

/* stack of attributes during page rendering */
extern int last_styles[128];
extern int last_colorattr_ptr;

#endif

#endif
