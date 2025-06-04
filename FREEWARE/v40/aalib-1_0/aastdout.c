#include "config.h"
#include <stdio.h>
#include "aalib.h"
int width, height;

static int stdout_init(struct aa_hardware_params *p, void *none)
{
    return 1;
}
static void stdout_uninit(aa_context * c)
{
}
static void stdout_getsize(aa_context * c, int *width, int *height)
{
}

static void stdout_flush(aa_context * c)
{
    int x, y;
    for (y = 0; y < aa_scrheight(c); y++) {
	for (x = 0; x < aa_scrwidth(c); x++) {
	    putc(c->textbuffer[x + y * aa_scrwidth(c)], stdout);
	}
	putc('\n', stdout);
    }
    putc('', stdout);
    putc('\n', stdout);
    fflush(stdout);
}
static void stdout_gotoxy(aa_context * c, int x, int y)
{
}
struct aa_driver stdout_d =
{
    "stdout", "Standard output driver",
    {NULL, AA_NORMAL_MASK | AA_EXTENDED},
    stdout_init,
    stdout_uninit,
    stdout_getsize,
    NULL,
    NULL,
    NULL,
    stdout_gotoxy,
    stdout_flush,
    NULL
};
