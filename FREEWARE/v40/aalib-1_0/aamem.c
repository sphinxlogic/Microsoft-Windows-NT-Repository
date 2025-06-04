#include "config.h"
#include "aalib.h"

static int mem_init(struct aa_hardware_params *p, void *none)
{
    return 1;
}
static void mem_uninit(aa_context * c)
{
}
static void mem_getsize(aa_context * c, int *width, int *height)
{
}

static void mem_flush(aa_context * c)
{
}
static void mem_gotoxy(aa_context * c, int x, int y)
{
}

struct aa_driver mem_d =
{
    "mem", "Dummy memory driver 1.0",
    {NULL, AA_DIM_MASK | AA_REVERSE_MASK | AA_NORMAL_MASK | AA_BOLD_MASK | AA_EXTENDED},
    mem_init,
    mem_uninit,
    mem_getsize,
    NULL,
    NULL,
    NULL,
    mem_gotoxy,
    mem_flush,
};
