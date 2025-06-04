#include <stdio.h>
#include "config.h"
#include "aalib.h"
#define MAXFONTS 246
struct aa_font *aa_fonts[MAXFONTS + 1] =
{
    &font8, &font9, &font14, &font16, &fontX13, &fontX13B, &fontX16, &fontline, &fontgl, &fontcourier, NULL
};

int aa_registerfont(struct aa_font *f)
{
    int i;
    for (i = 0; i < 256 && aa_fonts[i] != NULL; i++);
    if (i == 256)
	return 0;
    aa_fonts[i] = f;
    aa_fonts[i + 1] = 0;
    return (1);
}
