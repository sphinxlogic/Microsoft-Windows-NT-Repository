#include <stdio.h>
#include "aalib.h"
#define NORMALPRINTS { "%c", "%c", "%c", "%c", "%c", }
#define NONE { "", "", "", "", "", }
struct aa_format aa_nhtml_format =
{
    79, 36,
    0, 0,
    AA_HTML_ESCAPED,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK | AA_DIM_MASK,
    &fontcourier,
    "Nestcapeized html",
    "html",
    "<HTML>\n <HEAD> <TITLE>Ascii arted image done using aalib</TITLE>\n</HEAD>\n<BODY BGCOLOR=\"#000000\" TEXT=\"#b2b2b2\" LINK=\"#FFFFFF\">\n<FONT SIZE=2><PRE>\n",
    "</PRE></FONT></BODY>\n</HTML>\n",
    "\n",
    NORMALPRINTS,
    {"",
     "<FONT COLOR=\"686868\">",
     "<FONT COLOR=\"ffffff\">",
     "",
     "<B>"
    },
    {"",
     "</FONT>",
     "</FONT>",
     "",
     "</B>"
    },
};
struct aa_format aa_html_format =
{
    79, 25,
    0, 0,
    AA_HTML_ESCAPED,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK,
    NULL,
    "Pure html",
    "html",
    "<HTML>\n <HEAD> <TITLE>Ascii arted image done using aalib</TITLE>\n</HEAD>\n<BODY><PRE>\n",
    "</PRE></BODY>\n</HTML>\n",
    "\n",
    NORMALPRINTS,
    {"",
     "",
     "<B>",
     "",
     "<B>"
    },
    {"",
     "",
     "</B>",
     "",
     "</B>"
    },
};
struct aa_format aa_ansi_format =
{
    80, 25,
    0, 0,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK | AA_REVERSE_MASK | AA_DIM_MASK,
    NULL,
    "ansi escape seqences",
    "ansi",
    "",
    "",
    "\n",
    NORMALPRINTS,
    {"",
     "\33[8m",
     "\33[1m",
     "\33[1m",
     "\33[7m"
    },
    {"",
     "\33[0;10m",
     "\33[0;10m",
     "\33[0;10m",
     "\33[0;10m",
    },
};
struct aa_format aa_text_format =
{
    80, 25,
    0, 0,
    0,
    AA_NORMAL_MASK,
    NULL,
    "Text file",
    "txt",
    "",
    "",
    "\n",
    NORMALPRINTS,
    NONE,
    NONE
};
struct aa_format aa_more_format =
{
    80, 25,
    0, 0,
    AA_NORMAL_SPACES,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK,
    NULL,
    "For more/less",
    "cat",
    "",
    "",
    "\n",
    {"%c", "%c", "%c\10%c", "%c\10%c", "%c"},
    NONE,
    NONE
};
struct aa_format aa_hp_format =
{
    130 * 2, 64 * 2 - 1,
    130, 64 * 2 - 1,
    AA_USE_PAGES,
    AA_NORMAL_MASK,
    &fontline,
    "HP laser jet - A4 small font",
    "hp",
    "\33(10U\33(s0p16.67h8.5v0s0b0T\33%%0A\33&/0U\33&/0Z\33&/24D",
    "\14",
    "\r\33=",
    NORMALPRINTS,
    NONE,
    NONE
};
struct aa_format aa_hp2_format =
{
    80 * 2, 64 - 1,
    80, 64 - 1,
    AA_USE_PAGES,
    AA_NORMAL_MASK,
    &font16,
    "HP laser jet - A4 big font",
    "hp",
    "\33(s7B\33*p0Y@",
    "\14",
    "\r\n",
    NORMALPRINTS,
    NONE,
    NONE
};
struct aa_format *aa_formats[] =
{
    &aa_text_format,
    &aa_html_format,
    &aa_nhtml_format,
    &aa_more_format,
    &aa_ansi_format,
    &aa_hp_format,
    &aa_hp2_format,
    NULL
};

struct aa_driver save_d;
#define FORMAT ((struct aa_savedata *)c->driverdata)->format
#define DATA ((struct aa_savedata *)c->driverdata)

static int save_init(struct aa_hardware_params *p, void *none)
{
    struct aa_savedata *d = (struct aa_savedata *) none;
    save_d.params.supported = d->format->supported;
#if 0
    if (p->width || p->height) {
	d->width = p->width;
	d->height = p->height;
    }
#endif
    return 1;
}
static void save_uninit(aa_context * c)
{
}
static void save_getsize(aa_context * c, int *width, int *height)
{
#if 0
    *width = DATA->width;
    *height = DATA->height;
#endif
}
static void save_gotoxy(aa_context * c, int x, int y)
{
}

static int lastattr;
static FILE *f;
static aa_context *c;
static void stop_tag()
{
    if (lastattr != -1)
	fputs(FORMAT->ends[lastattr], f);
    lastattr = -1;
}
static void start_tag(int attr)
{
    if (attr > AA_NATTRS)
	attr = AA_NATTRS;
    lastattr = attr;
    fputs(FORMAT->begin[lastattr], f);
}
static void encodechar(unsigned char attr, unsigned char ch)
{
    if (FORMAT->flags & AA_NORMAL_SPACES && ch == ' ' && (attr != AA_REVERSE))
	attr = AA_NORMAL;
    if (attr != lastattr) {
	stop_tag();
	start_tag(attr);
    }
    if (FORMAT->flags & AA_HTML_ESCAPED) {
	switch (ch) {
	case '<':
	    fputs("&lt;", f);
	    return;
	case '>':
	    fputs("&gt;", f);
	    return;
	case '&':
	    fputs("&amp;", f);
	    return;
	}
    }
    if (FORMAT->flags & AA_C_ESCAPED) {
	switch (ch) {
	case '"':
	    fputs("\\\"", f);
	    return;
	case '\\':
	    fputs("\\\\", f);
	    return;
	}
    }
    fprintf(f, FORMAT->prints[attr], ch, ch, ch, ch);
}
static void savearea(int x1, int y1, int x2, int y2)
{
    int x, y;
    fputs(FORMAT->head, f);
    lastattr = -1;
    for (y = y1; y < y2; y++) {
	for (x = x1; x < x2; x++) {
	    if (x < 0 || x >= aa_scrwidth(c) ||
		y < 0 || y >= aa_scrheight(c))
		encodechar(AA_NORMAL, ' ');
	    else {
		int pos = x + y * aa_scrwidth(c);
		encodechar(c->attrbuffer[pos], c->textbuffer[pos]);
	    }
	}
	stop_tag();
	fputs(FORMAT->newline, f);
    }
    fputs(FORMAT->end, f);
    fflush(f);
}

static void save_flush(aa_context * c1)
{
    char fname[1024];
    c = c1;
    if (FORMAT->flags & AA_USE_PAGES) {
	int xpages = (aa_scrwidth(c1) + FORMAT->pagewidth - 1) / FORMAT->pagewidth;
	int ypages = (aa_scrheight(c1) + FORMAT->pageheight - 1) / FORMAT->pageheight;
	int x, y;
	for (x = 0; x < xpages; x++)
	    for (y = 0; y < ypages; y++) {
		sprintf(fname, "%s_%i_%i.%s", DATA->name, x, y, FORMAT->extension);
		f = fopen(fname, "w");
		if (f == NULL)
		    return;
		savearea(x * FORMAT->pagewidth, y * FORMAT->pageheight, (x + 1) * FORMAT->pagewidth, (y + 1) * FORMAT->pageheight);
		fclose(f);
	    }
    } else {
	sprintf(fname, "%s.%s", DATA->name, FORMAT->extension);
	f = fopen(fname, "w");
	if (f == NULL)
	    return;
	savearea(0, 0, /*DATA->width, DATA->height */ aa_scrwidth(c1), aa_scrheight(c1));
	fclose(f);
    }
}

struct aa_driver save_d =
{
    "save", "Special driver for saving to files",
    {NULL, 0},
    save_init,
    save_uninit,
    save_getsize,
    NULL,
    NULL,
    NULL,
    save_gotoxy,
    save_flush,
};
