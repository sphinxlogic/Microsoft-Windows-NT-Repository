
#include <stdio.h>
#include "aalib.h"

static void aa_display(aa_context * c, int x1, int y1, int x2, int y2)
{
    int x, y, pos, attr, p;
    unsigned char str[80];
    if (x2 < 0 || y2 < 0 || x1 > aa_scrwidth(c) || y1 > aa_scrheight(c))
	return;
    if (x2 >= aa_scrwidth(c))
	x2 = aa_scrwidth(c);
    if (y2 >= aa_scrheight(c))
	y2 = aa_scrheight(c);
    if (x1 < 0)
	x1 = 0;
    if (y1 < 0)
	y1 = 0;
    if (c->driver->update != NULL)
	c->driver->update(c, x1, y1, x2, y2);
    else {
	if (c->driver->print == NULL)
	    return;
	pos = 0;
	for (y = y1; y < y2; y++) {
	    pos = y * aa_scrwidth(c) + x1;
	    c->driver->gotoxy(c, x1, y);
	    for (x = x1; x < x2;) {
		p = 0;
		attr = c->attrbuffer[pos];
		while (p < 79 && x < x2 && c->attrbuffer[pos] == attr) {
		    str[p] = c->textbuffer[pos];
		    pos++;
		    p++;
		    x++;
		}
		str[p] = 0;
		c->driver->setattr(c, attr);
		c->driver->print(c, str);
	    }
	}
	c->driver->gotoxy(c, c->cursorx, c->cursory);
    }
}

void aa_flush(aa_context * c)
{
    if (c->driver->print != NULL)
	aa_display(c, 0, 0, aa_imgwidth(c), aa_imgheight(c));
    if (c->driver->flush != NULL)
	c->driver->flush(c);
}
