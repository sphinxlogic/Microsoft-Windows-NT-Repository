#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "aalib.h"
#include "aaint.h"

struct aa_hardware_params aa_defparams =
{
    &font16, AA_NORMAL_MASK | AA_DIM_MASK | AA_BOLD_MASK
};
void aa_uninitmouse(struct aa_context *c)
{
    if (c->mousedriver != NULL) {
	c->mousedriver->uninit(c);
	c->mousedriver = NULL;
    }
}
void aa_uninitkbd(struct aa_context *c)
{
    if (c->kbddriver != NULL) {
	if (c->mousedriver != NULL)
	    aa_uninitmouse(c);
	c->kbddriver->uninit(c);
	c->kbddriver = NULL;
    }
}

int aa_resize(aa_context * c)
{
    int width, height;
    width = abs(c->params.width);
    height = abs(c->params.height);
    c->driver->getsize(c, &width, &height);
    if (width <= 0 || height <= 0) {
	printf("Invalid buffer sizes!\n");
	exit(-1);
    }
    if (width != aa_scrwidth(c) || height != aa_imgheight(c)) {
	if (c->imagebuffer != NULL)
	    free(c->imagebuffer);
	if (c->textbuffer != NULL)
	    free(c->textbuffer);
	if (c->attrbuffer != NULL)
	    free(c->attrbuffer);
	c->params.width = width;
	c->params.height = height;
	c->imgwidth = width * c->mulx;
	c->imgheight = height * c->mulx;
	if ((c->imagebuffer = calloc(1, c->imgwidth * c->imgheight)) == NULL)
	    return 0;
	if ((c->textbuffer = calloc(1, aa_scrwidth(c) * aa_scrheight(c))) == NULL) {
	    free(c->imagebuffer);
	    return 0;
	}
	memset(c->textbuffer, ' ', c->params.width * c->params.height);
	if ((c->attrbuffer = calloc(1, c->params.width * c->params.height)) == NULL) {
	    free(c->imagebuffer);
	    free(c->textbuffer);
	    return 0;
	}
    }
    if (!c->driver->params.mmwidth)
	c->params.mmwidth = 290;
    else
	c->params.mmwidth = c->driver->params.mmwidth;
    if (!c->driver->params.mmheight)
	c->params.mmheight = 215;
    else
	c->params.mmheight = c->driver->params.mmheight;
    if (!c->driver->params.minwidth)
	c->params.minwidth = c->params.width;
    else
	c->params.minwidth = c->driver->params.minwidth;
    if (!c->driver->params.minheight)
	c->params.minheight = c->params.height;
    else
	c->params.minheight = c->driver->params.minheight;
    if (!c->driver->params.maxwidth)
	c->params.maxwidth = c->params.width;
    else
	c->params.maxwidth = c->driver->params.maxwidth;
    if (!c->driver->params.maxheight)
	c->params.maxheight = c->params.height;
    else
	c->params.maxheight = c->driver->params.maxheight;
    return 1;
}
aa_context *aa_init(struct aa_driver * driver, struct aa_hardware_params * defparams, void *driverdata)
{
    struct aa_context *c;
    if (!driver->init(defparams, driverdata))
	return NULL;
    c = calloc(1, sizeof(*c));
    if (c == NULL || !driver->params.supported) {
	driver->uninit(c);
	printf("no supported attributes available\n");
	return NULL;
    }
    c->driver = driver;
    c->kbddriver = NULL;
    c->mousedriver = NULL;
    c->driverdata = driverdata;
    c->params.supported = driver->params.supported & defparams->supported;
    c->params.font = driver->params.font;
    if (!c->params.font)
	c->params.font = defparams->font;
    if (!c->params.supported)
	c->params.supported = driver->params.supported;
    c->mulx = 2;
    c->muly = 2;
    c->cursorx = 0;
    c->cursory = 0;
    c->mousex = 0;
    c->mousey = 0;
    c->buttons = 0;
    c->table = NULL;
    c->filltable = NULL;
    c->parameters = NULL;
    if (defparams->width)
	c->params.width = defparams->width;
    else if (driver->params.width)
	c->params.width = driver->params.width;
    else if (defparams->recwidth)
	c->params.recwidth = defparams->recwidth;
    else if (driver->params.recwidth)
	c->params.recwidth = driver->params.recwidth;
    else
	c->params.width = 80;
    if (defparams->minwidth > c->params.width)
	c->params.width = defparams->minwidth;
    if (driver->params.minwidth > c->params.width)
	c->params.width = driver->params.minwidth;
    if (defparams->maxwidth && defparams->maxwidth > c->params.width)
	c->params.width = defparams->maxwidth;
    if (driver->params.maxwidth && driver->params.maxwidth > c->params.width)
	c->params.width = driver->params.maxwidth;

    if (defparams->height)
	c->params.height = defparams->height;
    else if (driver->params.height)
	c->params.height = driver->params.height;
    else if (defparams->recheight)
	c->params.recheight = defparams->recheight;
    else if (driver->params.recheight)
	c->params.recheight = driver->params.recheight;
    else
	c->params.height = 25;
    if (defparams->minheight > c->params.height)
	c->params.height = defparams->minheight;
    if (driver->params.minheight > c->params.height)
	c->params.height = driver->params.minheight;
    if (defparams->maxheight && defparams->maxheight > c->params.height)
	c->params.height = defparams->maxheight;
    if (driver->params.maxheight && driver->params.maxheight > c->params.height)
	c->params.height = driver->params.maxheight;
    c->params.width *= -1;
    c->params.height *= -1;
    c->params.dimmul = 5.3;
    c->params.boldmul = 2.7;
    if(c->driver->params.dimmul) c->params.dimmul=c->driver->params.dimmul;
    if(c->driver->params.boldmul) c->params.boldmul=c->driver->params.boldmul;
    if(defparams->dimmul) c->params.dimmul=defparams->dimmul;
    if(defparams->boldmul) c->params.boldmul=defparams->boldmul;
    c->imagebuffer = NULL;
    c->textbuffer = NULL;
    c->attrbuffer = NULL;
    c->resizehandler = NULL;
    if (!aa_resize(c)) {
	driver->uninit(c);
	free(c);
	printf("out of memory\n");
	return NULL;
    }
    if (!aa_validmode(aa_scrwidth(c), aa_scrheight(c), defparams)) {
	aa_close(c);
	return 0;
    }
    return (c);
}
static void aa_invalidate(aa_context * c)
{
    if (c->table != NULL)
	free(c->table);
    if (c->filltable != NULL)
	free(c->filltable);
    if (c->parameters != NULL)
	free(c->parameters);
    c->table = NULL;
    c->filltable = NULL;
    c->parameters = NULL;
}
void aa_setfont(aa_context * c, struct aa_font *font)
{
    c->params.font = font;
    aa_invalidate(c);
}
void aa_setsupported(aa_context * c, int supported)
{
    c->params.supported = c->driver->params.supported & supported;
    if (!c->params.supported)
	c->params.supported = c->driver->params.supported;
    aa_invalidate(c);
}
void aa_close(aa_context * c)
{
    if (c->cursorstate < 0 && c->driver->cursormode)
	c->driver->cursormode(c, 1);
    if (c->kbddriver != NULL)
	aa_uninitkbd(c);
    c->driver->uninit(c);
    aa_invalidate(c);
    if (c->imagebuffer != NULL)
	free(c->imagebuffer);
    if (c->textbuffer != NULL)
	free(c->textbuffer);
    if (c->attrbuffer != NULL)
	free(c->attrbuffer);
    free(c);
}
