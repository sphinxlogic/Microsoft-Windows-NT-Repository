#include "config.h"
#include <stdio.h>
#include "aalib.h"
int width, height;

static int stderr_init(struct aa_hardware_params *p, void *none)
{
    return 1;
}
static void stderr_uninit(aa_context * c)
{
}
static void stderr_getsize(aa_context * c, int *width, int *height)
{
}

static void stderr_flush(aa_context * c)
{
    int x, y;
    for (y = 0; y < aa_scrheight(c); y++) {
	for (x = 0; x < aa_scrwidth(c); x++) {
	    putc(c->textbuffer[x + y * aa_scrwidth(c)], stderr);
	}
	putc('\n', stderr);
    }
    putc('', stderr);
    putc('\n', stderr);
    fflush(stderr);
}
static void stderr_gotoxy(aa_context * c, int x, int y)
{
}
struct aa_driver stderr_d =
{
    "stderr", "Standard error driver",
    {NULL, AA_NORMAL_MASK | AA_EXTENDED},
    stderr_init,
    stderr_uninit,
    stderr_getsize,
    NULL,
    NULL,
    NULL,
    stderr_gotoxy,
    stderr_flush,
    NULL
};
