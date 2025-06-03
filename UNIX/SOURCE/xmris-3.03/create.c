/* Copyright (C) 1992 Nathan Sidwell */
/*{{{  interesting color stuff*/
/*
 * color is spelt as it is, 'cos its just too complicated spelling it as
 * it should be, when X doesn't.
 * The colour allocation is done by first parsing all the colour names
 * to RGB values (using a type convertor from the resources). Each RGB
 * value is converted to a 3space coordinate in a colour 'cone' (the
 * colour circle, (well, hexagon in this case), and a height,
 * corresponding to brightness). With that we can work out the
 * perceived distances between different colours.
 * These are then allocated in turn, by picking the unallocated colour
 * farthest from the allocated colours. If the allocation fails, we pick
 * the allocated colour which is nearest the desired colour. This way
 * we try to get the most diverse set of colours possible with the
 * colormap that's being used. Remember, black and white have already
 * been allocated so we don't need to allocate those, and it gives us
 * a starting ix in the allocation loop.
 * The board background colours only need allocating, if it is a read
 * only visual (Direct Colour or something). For writable visuals,
 * we allocate two colorcells and set these as required, allowing
 * dynamic colour changes, and saving entries in the colormap.
 * You can see the order in which this is done by giving the -colours arg.
 * There is a bug (IMHO) in the way that X parses hex color names to XColor.
 * #FA5 is parsed as #F000A0005000, not #FFFFAAAA5555 as I would have
 * expected it to be. That's why I've specified colours accurately.
 * When the color #F000A0005000 is allocated, the rgb values are set,
 * as I expected, to #F0F0A0A05050, on an 8 bit visual. There doesn't
 * seem to be a way to get X to tell you what colour it would
 * allocate on a colormap, assuming that there's a space.
 */
/*}}}*/
#include "xmris.h"
#include "declare.h"
/*{{{  _tolower thing*/
#include <ctype.h>
#ifdef _tolower
#define lowercase(a) _tolower(a)
#else
#define lowercase(a) tolower(a)
#endif /* _tolower */
/*}}}*/
/*{{{  static tables*/
/*{{{  static char const *fallback[] =*/
static char const *fallback[] =
{
#if __STDC__
  "Xmris.font:" FONT_NAME,
#else
  "Xmris.font:%s",
#endif /* __STDC__ */
  NULL
};
/*}}}*/
/*{{{  static Arg toplevel_args[] =*/
static Arg toplevel_args[] =
{
  {XtNwidth, WINDOW_WIDTH + 2 + EDGING * 2},
  {XtNheight, WINDOW_HEIGHT + 2 + EDGING * 2},
  {XtNmaxWidth, WINDOW_WIDTH + 2 + EDGING * 2},
  {XtNmaxHeight, WINDOW_HEIGHT + 2 + EDGING * 2},
};
/*}}}*/
/*{{{  static XtConvertArgRec const string2colorargs[] =*/
static XtConvertArgRec const string2colorargs[] =
{
  {XtAddress, (XtPointer)&display.colormap, sizeof(display.colormap)},
};
/*}}}*/
/*}}}*/
/*{{{  prototypes*/
static Boolean convert_string2color PROTOARG((Display *, XrmValue *,
    Cardinal *, XrmValue *, XrmValue *, XtPointer *));
static Boolean convert_string2gender PROTOARG((Display *, XrmValue *,
    Cardinal *, XrmValue *, XrmValue *, XtPointer *));
static Boolean convert_string2keysym PROTOARG((Display *, XrmValue *,
    Cardinal *, XrmValue *, XrmValue *, XtPointer *));
static int error_handler PROTOARG((Display *, XErrorEvent *));
static VOIDFUNC make_color_image
    PROTOARG((SPRITE *, SPRITE_DEF *, Window, unsigned, Pixmap));
static VOIDFUNC make_mono_image
    PROTOARG((SPRITE *, SPRITE_DEF *, Window, unsigned, Pixmap));
static VOIDFUNC make_mask_image
    PROTOARG((SPRITE *, SPRITE_DEF *, Window, unsigned, Pixmap));
/*}}}*/
/*{{{  void close_toolkit()*/
extern VOIDFUNC close_toolkit FUNCARGVOID
{
  if(display.toplevel)
    XtDestroyWidget(display.toplevel);
  if(display.display)
    XtCloseDisplay(display.display);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  Boolean convert_string2color(display, args, num_args, from, to, data)*/
static Boolean convert_string2color
/* ARGSUSED */
FUNCARG((display, args, num_args, from, to, data),
	Display   *display
ARGSEP  XrmValue  *args
ARGSEP  Cardinal  *num_args
ARGSEP  XrmValue  *from
ARGSEP  XrmValue  *to
ARGSEP  XtPointer *data
)
/*
 * converts a string to a colordef
 */
{
  Boolean ok;
  
  ok = False;
  if(to->size == sizeof(XColor))
    {
      Status    status;
      
      assert(*num_args == 1 && args->size == sizeof(Colormap));
      status = XParseColor(display, *(Colormap *)args->addr,
	  (char const *)from->addr, (XColor *)to->addr);
      if(status == 1)
	{
	  ok = True;
	  ((XColor *)to->addr)->flags = DoRed | DoGreen | DoBlue;
	}
    }
  if(ok == False)
    fprintf(stderr, "Cannot convert '%s' to Color\n",
	(char const *)from->addr);
  return ok;
}
/*}}}*/
/*{{{  Boolean convert_string2gender(display, args, num_args, from, to, data)*/
static Boolean convert_string2gender
/* ARGSUSED */
FUNCARG((display, args, num_args, from, to, data),
	Display   *display
ARGSEP  XrmValue  *args
ARGSEP  Cardinal  *num_args
ARGSEP  XrmValue  *from
ARGSEP  XrmValue  *to
ARGSEP  XtPointer *data
)
/*
 * converts a string to a gender boolean (my new type XtRGender)
 */
{
  static char const *genders[] =
    {"he", "male", "mris", "she", "msit", "female"};
  Boolean ok;
  char const **ptr;
  
  ok = False;
  if(to->size == sizeof(Boolean))
    {
      for(ptr = genders; *ptr; ptr++)
	if(!strcmp(*ptr, (char const *)from->addr))
	  {
	    *(Boolean *)to->addr = (ptr - genders) > 2 ? True : False;
	    ok = True;
	    break;
	  }
    }
  if(ok == False)
    fprintf(stderr, "Cannot convert '%s' to gender\n",
	(char const *)from->addr);
  return ok;
}
/*}}}*/
/*{{{  Boolean convert_string2keysym(display, args, num_args, from, to, data)*/
static Boolean convert_string2keysym
/* ARGSUSED */
FUNCARG((display, args, num_args, from, to, data),
	Display   *display
ARGSEP  XrmValue  *args
ARGSEP  Cardinal  *num_args
ARGSEP  XrmValue  *from
ARGSEP  XrmValue  *to
ARGSEP  XtPointer *data
)
/*
 * converts a key name string to a key symbol
 */
{
  Boolean ok;
  KeySym symbol;
  
  ok = False;
  if(to->size == sizeof(KeySym))
    {
      symbol = XStringToKeysym((char const *)from->addr);
      if(symbol)
	{
	  *(KeySym *)to->addr = symbol;
	  ok = True;
	}
    }
  if(ok == False)
    fprintf(stderr, "Cannot convert '%s' to keysym\n",
	(char const *)from->addr);
  return ok;
}
/*}}}*/
/*{{{  void create_widget()*/
extern VOIDFUNC create_widget FUNCARGVOID
/*
 * create the game widget thing. This is a paned widget, with
 * a window inside.
 * Create the graphics contexts, sprites and colours and stuff.
 */
{
  Window    root;
  unsigned  depth;
  Widget    widget;
  
  display.screen = DefaultScreen(display.display);
  depth = DefaultDepth(display.display, display.screen);
  if(data.visual)
    /*{{{  get information about requested visual*/
    {
      XVisualInfo *list;
      XVisualInfo required;
      int       count;
      
      required.visualid = data.visual->visualid;
      list = XGetVisualInfo(display.display, VisualIDMask,
	  &required, &count);
      if(list)
	{
	  depth = list[0].depth;
	  display.screen = list[0].screen;
	  XFree((char *)list);
	}
      else
	data.visual = NULL;
    }
    /*}}}*/
  if(!data.visual)
    data.visual = DefaultVisual(display.display, display.screen);
  if(data.visual->map_entries < 16)
    data.mono = True;
  root = RootWindow(display.display, display.screen);
  display.colormap = DefaultColormap(display.display, display.screen);
  /*{{{  private colormap?*/
  if(data.private == True)
    {
      Colormap  colormap;
      
      colormap = XCreateColormap(display.display, root,
	  data.visual, AllocNone);
      if(colormap)
	{
	  display.colormap = colormap;
	  XtVaSetValues(display.toplevel,
	      XtNcolormap, display.colormap, NULL);
	  XtSetWMColormapWindows(display.toplevel, &display.toplevel, 1);
	}
      else
	{
	  fprintf(stderr, "Cannot create colormap, using default");
	  data.private = False;
	}
    }
  /*}}}*/
  /*{{{  show visual class?*/
  if(data.colors == True)
    {
      VISUAL_CLASS const *vptr;
      
      for(vptr = visual_class; vptr->name; vptr++)
	if(vptr->class == data.visual->class)
	  break;
      fprintf(stdout, "Using %s visual with %s colormap of %d entries\n",
	  vptr->name ? vptr->name : vptr->class & 1 ?
	  "UnknownDynamic" : "UnknownStatic",
	  data.private == True ? "private" : "shared",
	  data.visual->map_entries);
    }
  /*}}}*/
  /*{{{  make black and white*/
  {
    Status  status;
    XColor  color;
    
    color.red = color.green = color.blue = 0xFFFF;
    color.flags = DoRed | DoGreen | DoBlue;
    status = XAllocColor(display.display, display.colormap, &color);
    if(status == 1)
      {
	display.white = color.pixel;
	color.red = color.green = color.blue = 0x0000;
	color.flags = DoRed | DoGreen | DoBlue;
	status = XAllocColor(display.display, display.colormap, &color);
	display.black = color.pixel;
      }
    if(status != 1)
      fatal_error("Cannot get hold of black and white pixels");
    if(data.colors == True)
      {
	fprintf(stdout, "Color white pixel %lu\n", display.white);
	fprintf(stdout, "Color black pixel %lu\n", display.black);
      }
  }
  /*}}}*/
  /*{{{  XtVaSetValues*/
  {
    SPRITE_DEF const *sptr;
    
    sptr = &icons[(int)(data.gender != False)];
    XtVaSetValues(display.toplevel, XtNiconPixmap,
	XCreateBitmapFromData(display.display, root,
	(char const *)sptr->bitmap, sptr->size.x, sptr->size.y),
	XtNinput, True,
	XtNvisual, data.visual,
	NULL);
  }
  /*}}}*/
  XtAppSetTypeConverter(display.context, XtRString, XtRColor,
      convert_string2color, string2colorargs, XtNumber(string2colorargs),
      XtCacheNone, (void (*)PROTOARG((XtAppContext, XrmValue *, XtPointer,
	  XrmValue *, Cardinal *)))NULL);
  widget = XtVaCreateManagedWidget(data.gender == True ? "msit" : "mris",
      panedWidgetClass, display.toplevel, XtNbackground,
      data.swap == True ? display.black : display.white,
      NULL);
  if(!widget)
    fatal_error("Cannot create game widget");
  color_one = ((unsigned long)1 << depth) - (unsigned long)1;
  /*{{{  DEC stuff*/
  /*
   * This bit's from xchomp. I don't know if it's really required.
   * We want to suspend the game in case the window is iconified.
   * This is more difficult than it sounds. On the Sun, iconification
   * seems to produce an UnmapNotify event -- very nice. DECwindows,
   * however, informs the application by generating a PropertyNotify
   * event on a DEC-specific property -- very nasty. The atom is
   * not defined in any of the DECwindows headers, so we will try
   * to get its value from the server, and use it later. We are
   * hoping here that all non-DECwindows servers will return None
   * for this atom.
   */
  display.DEC_icon_atom =
      XInternAtom(display.display, "DEC_WM_ICON_STATE", True);
  /*}}}*/
  /*{{{  color?*/
  if(data.mono == False)
    {
      unsigned  ix;
      COLOR_DEF *nptr;
      XColor    *cptr;
      unsigned  count;
      unsigned  new;
      unsigned  share;
      
      /*{{{  malloc & get resources*/
      {
	XtResource *resources;
	size_t    length;
	char      *text;
	char      *tptr;
	XtResource *rptr;
	unsigned  source;
	unsigned  mask;
      
	colors = (XColor *)malloc(COLORS * sizeof(XColor));
	resources = (XtResource *)malloc((COLORS - 4) * sizeof(XtResource));
	length = COLORS - 4;
	for(nptr = &color_names[4], count = COLORS - 4; count--; nptr++)
	  length += strlen(nptr->name);
	text = malloc(length);
	if(!colors || !resources || !text)
	  fatal_error("Cannot malloc color allocation");
	for(cptr = colors, count = COLORS; count--; cptr++)
	  cptr->flags = 0;
	colors[COLOR_WHITE].flags = DoRed | DoGreen | DoBlue;
	colors[COLOR_WHITE].red = 0xFF00;
	colors[COLOR_WHITE].green = 0xFF00;
	colors[COLOR_WHITE].blue = 0xFF00;
	colors[COLOR_BLACK].flags = DoRed | DoGreen | DoBlue;
	colors[COLOR_BLACK].red = 0;
	colors[COLOR_BLACK].green = 0;
	colors[COLOR_BLACK].blue = 0;
	source = (data.gender == True ? 2 : 0) +
	    (data.swap == True ? 1 : 0);
	mask = (2 << source) - 1;
	if(source == 2)
	  mask ^= 2;
	/*{{{  build the resource table*/
	{
	  for(nptr = &color_names[4], rptr = resources, tptr = text, ix = 4;
	      ix != COLORS; ix++, nptr++, rptr++)
	    {
	      char const **sptr;
	      unsigned  bit;
	      
	      rptr->resource_name = tptr;
	      strcpy(tptr, nptr->name);
	      if(isupper(*tptr))
		*tptr = lowercase(*tptr);
	      tptr += strlen(tptr) + 1;
	      rptr->resource_class = nptr->name;
	      rptr->resource_type = XtRColor;
	      rptr->resource_size = sizeof(XColor);
	      rptr->resource_offset = sizeof(XColor) * ix;
	      rptr->default_type = XtRString;
	      for(sptr = &nptr->source[source], bit = 1 << source;
		  !*sptr || !(bit & mask); bit >>= 1, sptr--)
		assert(bit);
	      rptr->default_addr = (XtPointer)*sptr;
	    }
	  assert(tptr - text == length);
	}
	/*}}}*/
	if(data.swap == True)
	  XtGetSubresources(widget, colors, "swap", "Swap",
	      resources, COLORS - 4, NULL, 0);
	else
	  XtGetApplicationResources(widget, colors,
	      resources, COLORS - 4, NULL, 0);
	free(text);
	free(resources);
      }
      /*}}}*/
      display.dynamic = 0;
      /*{{{  dynamic colors?*/
      if(data.visual->class & 1)
	{
	  unsigned long pixels[2];
	  unsigned long planes;
	  
	  if(XAllocColorCells (display.display, display.colormap, False,
	      &planes, 0, pixels, 2))
	    {
	      display.dynamic = 1;
	      colors[0].pixel = pixels[0];
	      colors[1].pixel = pixels[1];
	      if(data.colors == True)
		fprintf(stdout, "Dynamic: background %lu, foreground %lu\n",
		    pixels[0], pixels[1]);
	    }
	}
      /*}}}*/
      count = new = 0;
      /*{{{  parse all the colors*/
      {
	unsigned  mask;
	
	mask = (data.gender == True ? 4 : 1) << (data.swap == True);
	for(cptr = &colors[2], nptr = &color_names[2], ix = COLORS - 2;
	    ix--; nptr++, cptr++)
	  if(cptr->flags)
	    {
	      nptr->coord[0] = RGB2X(cptr->red, cptr->green, cptr->blue);
	      nptr->coord[1] = RGB2Y(cptr->red, cptr->green, cptr->blue);
	      nptr->coord[2] = RGB2H(cptr->red, cptr->green, cptr->blue);
	      nptr->distance = 0;
	      nptr->nearest = NULL;
	      if(nptr->type & mask)
		{
		  if(nptr->type & 16 && display.dynamic)
		    nptr->alloc = 4;
		  else if(nptr->type & 32 && !display.dynamic)
		    nptr->alloc = 3;
		  else
		    {
		      nptr->alloc = 0;
		      count++;
		    }
		}
	      else
		nptr->alloc = 3;
	    }
	  else if(nptr->alloc & mask)
	    fatal_error("Have no color for %s", nptr->name);
      }
      /*}}}*/
      share = 2;
      /*{{{  allocate them in optimum order*/
      for(colors[COLOR_WHITE].pixel = display.white,
	  nptr = &color_names[COLOR_WHITE], nptr->alloc = 1;
	  --count;)
	{
	  if(nptr->alloc == 1)
	    /*{{{  work out distance of just allocated color*/
	    {
	      unsigned  ix;
	      COLOR_DEF *optr;
		  
	      for(optr = color_names, ix = COLORS; ix--; optr++)
		if(!optr->alloc)
		  {
		    unsigned long distance;
		    unsigned  ix;
		    
		    distance = 0;
		    for(ix = 3; ix--;)
		      /*
		       * must be very careful about type promotion and
		       * overflow here
		       */
		      {
			unsigned  delta;
			
			delta = nptr->coord[ix] < optr->coord[ix] ?
			    optr->coord[ix] - nptr->coord[ix] :
			    nptr->coord[ix] - optr->coord[ix];
			distance += ((unsigned long)delta *
			    (unsigned long)delta) / 4;
		      }
		    if(distance < optr->distance || !optr->nearest)
		      {
			optr->distance = distance;
			optr->nearest = nptr;
		      }
		  }
	    }
	    /*}}}*/
	  if(nptr == &color_names[COLOR_WHITE])
	    {
	      nptr = &color_names[COLOR_BLACK];
	      nptr->alloc = 1;
	      colors[COLOR_BLACK].pixel = display.black;
	    }
	  else
	    /*{{{  allocate the farthest one*/
	    {
	      unsigned long distance;
	      COLOR_DEF *optr;
	      Status    status;
	      unsigned  ix;
	      XColor    *fptr;
	    
	      nptr = NULL;
	      distance = 0;
	      for(optr = color_names, ix = COLORS; ix--; optr++)
		if(!optr->alloc && distance <= optr->distance)
		  {
		    nptr = optr;
		    distance = nptr->distance;
		  }
	      assert(nptr && nptr->nearest);
	      cptr = &colors[nptr - color_names];
	      fptr = &colors[nptr->nearest - color_names];
	      if(distance)
		/*{{{  different*/
		{
		  if(data.colors == True)
		    fprintf(stdout,
			"Color %s:#%04X%04X%04X near %s:#%04X%04X%04X (%lu)",
			nptr->name, cptr->red, cptr->green, cptr->blue,
			nptr->nearest->name,
			fptr->red, fptr->green, fptr->blue, distance);
		  if(data.distinct)
		    status = XAllocColor(display.display, display.colormap,
		      cptr);
		  else
		    status = 0;
		}
		/*}}}*/
	      else
		/*{{{  same*/
		{
		  if(data.colors == True)
		    fprintf(stdout,
			"Color %s:#%04X%04X%04X at %s:#%04X%04X%04X",
			nptr->name, cptr->red, cptr->green, cptr->blue,
			nptr->nearest->name,
			fptr->red, fptr->green, fptr->blue);
		  status = 0;
		}
		/*}}}*/
	      /*{{{  fixup*/
	      if(status == 1)
		{
		  new++;
		  nptr->alloc = 1;
		  distance = 0;
		  data.distinct--;
		}
	      else
		{
		  nptr->alloc = 2;
		  cptr->pixel = fptr->pixel;
		  share++;
		}
	      /*}}}*/
	      if(data.colors == True)
		fprintf(stdout, nptr->alloc == 1 ?
		    " pixel %lu\n" : " sharing %lu\n", cptr->pixel);
	      /*{{{  error message?*/
	      if(distance)
		{
		  fprintf(stderr,
		      "No color %s:#%04X%04X%04X used %s:#%04X%04X%04X\n",
		      nptr->name, cptr->red, cptr->green, cptr->blue,
		      nptr->nearest->name,
		      fptr->red, fptr->green, fptr->blue);
		}
	      /*}}}*/
	    }
	    /*}}}*/
	}
      /*}}}*/
      if(data.colors == True)
	fprintf(stdout, "%u new colours and %u shared\n", new, share);
      for(nptr = &color_names[2], count = COLORS - 2; count--; nptr++)
	assert(nptr->alloc);
    }
  /*}}}*/
  /*{{{  swap b & w?*/
  if(data.swap == True)
    {
      unsigned long temp;
    
      temp = display.black;
      display.black = display.white;
      display.white = temp;
    }
  /*}}}*/
  if(colors)
    {
      display.white = colors[COLOR_BACKGROUND].pixel;
      display.black = colors[COLOR_FOREGROUND].pixel;
    }
  display.xor = display.black ^ display.white;
  /*{{{  set foreground & background types*/
  {
    unsigned long set;
    unsigned long clear;
    unsigned  ix;
    
    set = color_zero;
    clear = color_one;
    /*{{{  set on and off*/
    if(data.mono == True)
      {
	set |= display.white;
	clear &= display.white;
	set |= display.black;
	clear &= display.black;
      }
    else
      {
	for(ix = COLORS; ix--;)
	  {
	    set |= colors[ix].pixel;
	    clear &= colors[ix].pixel;
	  }
      }
    /*}}}*/
    display.background = display.white == clear ? COLOUR_ZERO :
	display.white == set ? COLOUR_ONE : COLOUR_WEIRD;
    display.foreground = display.black == clear ? COLOUR_ZERO :
	display.black == set ? COLOUR_ONE : COLOUR_WEIRD;
  }
  /*}}}*/
  /*{{{  get a font*/
  {
    char const *string = "09AZhy";
    int       direction;
    XCharStruct chars;
	
    XQueryTextExtents(display.display, data.font, string, 6,
	    &direction, &font.ascent, &font.descent, &chars);
    font.width = chars.width / 6;
    font.center = (font.ascent - font.descent) / 2;
  }
  /*}}}*/
  /*{{{  create graphics contexts*/
  {
    XGCValues gcv;
    CONTEXT   *cptr;
    unsigned  count;
	
    gcv.plane_mask = AllPlanes;
    gcv.font       = data.font;
    gcv.graphics_exposures = False;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gcv.join_style = JoinMiter;
    gcv.fill_style = FillSolid;
    gcv.cap_style = CapNotLast;
    for(cptr = gcsdefine, count = 0; count != GCS; count++, cptr++)
      {
	gcv.function = cptr->function;
	gcv.foreground = *cptr->fgp;
	gcv.background = *cptr->bgp;
	GCN(count) = XCreateGC(display.display, root,
	    GCForeground | GCBackground | GCFunction |
	    GCFont | GCGraphicsExposures | GCPlaneMask | GCCapStyle |
	    GCLineStyle | GCLineWidth | GCJoinStyle | GCFillStyle, &gcv);
	if(!GCN(count))
	  fatal_error("Cannot create context %d", count);
      }
  }
  /*}}}*/
  display.back = XCreatePixmap(display.display, root,
      WINDOW_WIDTH, WINDOW_HEIGHT, depth);
  display.copy = XCreatePixmap(display.display, root,
      WINDOW_WIDTH, WINDOW_HEIGHT, depth);
  if(!display.back || !display.copy)
    fatal_error("Cannot create window copies");
  XFillRectangle(display.display, display.copy, GCN(GC_CLEAR),
      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  /*{{{  are we she?*/
  if(data.gender == True)
    {
      SPRITE_DEF *dptr;
      SPRITE_DEF const *sptr;
      
      for(sptr = she_nadger; sptr->copy; sptr++)
	{
	  dptr = &sprites_def[sptr->copy];
	  dptr->bitmap = sptr->bitmap;
	  dptr->planes = sptr->bitmap ? sptr->planes : 0;
	  dptr->flags = sptr->flags;
	  dptr->copy = sptr->bitmap ? 0 : sptr->planes;
	  memcpy(&dptr->size, &sptr->size, sizeof(sptr->size));
	  memcpy(dptr->colors, sptr->colors, sizeof(sptr->colors));
	}
      memcpy(ball_hold, she_hold, sizeof(ball_hold));
      lettering[0] = letter_msit;
    }
  /*}}}*/
  /*{{{  create sprites*/
  {
    unsigned  i;
    SPRITE    *dptr;
    SPRITE_DEF *sptr;
    unsigned  unknown;
    unsigned  changed;
	
    /*{{{  generate all the ones from bitmaps*/
    for(i = 0, sptr = sprites_def, dptr = sprites;
	i != SPRITES; i++, sptr++, dptr++)
      {
	/* check that its the size we expected */
	if(sptr->bitmap)
	  {
	    Pixmap    bitmap;
	    
	    assert(!sptr->expected.x || sptr->expected.x == sptr->size.x);
	    assert(!sptr->expected.y || sptr->expected.y * sptr->planes == sptr->size.y);
	    assert(sptr->size.x && sptr->size.y && !sptr->copy);
	    dptr->size.x = sptr->size.x;
	    dptr->size.y = sptr->size.y / sptr->planes;
	    bitmap = XCreatePixmapFromBitmapData(display.display,
		root, (char *)sptr->bitmap,
		sptr->size.x, sptr->size.y, color_one, color_zero, depth);
	    dptr->image = XCreatePixmap(display.display, root,
	      dptr->size.x, dptr->size.y, depth);
	    dptr->mask = XCreatePixmap(display.display, root,
	      dptr->size.x, dptr->size.y, depth);
	    if(!bitmap || !dptr->image || !dptr->mask)
	      fatal_error("Cannot create sprite %d", i);
	    if(!sptr->colors[0])
	      /*{{{  solid black*/
	      {
		XFillRectangle(display.display, dptr->image, GCN(GC_CLEAR),
		    0, 0, sptr->size.x, sptr->size.y);
	      }
	      /*}}}*/
	    else if(data.mono == True)
	      make_mono_image(dptr, sptr, root, depth, bitmap);
	    else
	      make_color_image(dptr, sptr, root, depth, bitmap);
	    make_mask_image(dptr, sptr, root, depth, bitmap);
	    XFreePixmap(display.display, bitmap);
	  }
      }
    /*}}}*/
    do
      {
	unknown = SPRITES;
	changed = 0;
	/*{{{  do what copies we can*/
	for(sptr = sprites_def, dptr = sprites, i = SPRITES;
	    i--; sptr++, dptr++)
	  if(!sptr->copy)
	    {
	      /*
	       * Sun's assert macro is broken, so I have to
	       * put it in a scope
	       */
	      assert(sptr->bitmap);
	    }
	  else if(!dptr->mask && !dptr->image)
	    {
	      SPRITE    *optr;
	
	      optr = &sprites[sptr->copy];
	      if(!optr->size.x || !optr->size.y)
		unknown = i;
	      else
		{
		  changed = 1;
		  dptr->size.x = optr->size.x;
		  dptr->size.y = optr->size.y;
		  switch(sptr->flags)
		  {
		    /*{{{  case REFLECT_ALIAS:*/
		    case REFLECT_ALIAS:
		      dptr->mask = optr->mask;
		      dptr->image = optr->image;
		      break;
		    /*}}}*/
		    /*{{{  case REFLECT_VERTICAL:*/
		    case REFLECT_VERTICAL:
		    {
		      int       i;
		    
		      dptr->mask = XCreatePixmap(display.display, root,
			  dptr->size.x, dptr->size.y, depth);
		      dptr->image = XCreatePixmap(display.display, root,
			  dptr->size.x, dptr->size.y, depth);
		      if(dptr->image && dptr->mask)
			for(i = dptr->size.x; i--;)
			  {
			    XCopyArea(display.display,
				optr->mask, dptr->mask,
				GCN(GC_COPY),
				i, 0, 1, dptr->size.y,
				(int)dptr->size.x - i - 1, 0);
			    XCopyArea(display.display,
				optr->image, dptr->image,
				GCN(GC_COPY),
				i, 0, 1, dptr->size.y,
				(int)dptr->size.x - i - 1, 0);
			  }
		      break;
		    }
		    /*}}}*/
		    /*{{{  case REFLECT_HORIZONTAL:*/
		    case REFLECT_HORIZONTAL:
		    {
		      int       i;
		    
		      dptr->mask = XCreatePixmap(display.display, root,
			  dptr->size.x, dptr->size.y, depth);
		      dptr->image = XCreatePixmap(display.display, root,
			  dptr->size.x, dptr->size.y, depth);
		      if(dptr->mask && dptr->image)
			for(i = dptr->size.x; i--;)
			  {
			    XCopyArea(display.display,
				optr->mask, dptr->mask,
				GCN(GC_COPY),
				0, i, dptr->size.x, 1,
				0, (int)dptr->size.y - i - 1);
			    XCopyArea(display.display,
				optr->image, dptr->image,
				GCN(GC_COPY),
				0, i, dptr->size.x, 1,
				0, (int)dptr->size.y - i - 1);
			  }
		      break;
		    }
		    /*}}}*/
		    /*{{{  case REFLECT_DIAGONAL:*/
		    case REFLECT_DIAGONAL:
		    {
		      int       x, y;
		    
		      assert(dptr->size.x == dptr->size.y);
		      dptr->mask = XCreatePixmap(display.display, root,
			  dptr->size.x, dptr->size.y, depth);
		      dptr->image = XCreatePixmap(display.display, root,
			  dptr->size.x, dptr->size.y, depth);
		      if(dptr->mask && dptr->image)
			for(x = dptr->size.x; x--;)
			  for(y = dptr->size.y; y--;)
			    {
			      XCopyArea(display.display,
				  optr->mask, dptr->mask,
				  GCN(GC_COPY), x, y, 1, 1, y, x);
			      XCopyArea(display.display,
				  optr->image, dptr->image,
				  GCN(GC_COPY), x, y, 1, 1, y, x);
			    }
		      break;
		    }
		    /*}}}*/
		    /*{{{  default*/
		    default:
		      assert(0);
		    /*}}}*/
		  }
		  if(!dptr->image || !dptr->mask)
		    fatal_error("Cannot generate sprite %d", SPRITES - 1 - i);
		}
	    }
	/*}}}*/
      }
    while(unknown != SPRITES && changed);
    assert(unknown == SPRITES); /* check no circular definitions */
  }
  /*}}}*/
  /*{{{  fiddle center & edge*/
  if(display.background == COLOUR_ZERO)
    {
      SPRITE  *sptr;
      unsigned ix;
      
      for(sptr = &sprites[SPRITE_CENTER_BASE], ix = 4; ix--; sptr++)
	XCopyArea(display.display, sptr->image, sptr->mask, GCN(GC_MASK),
	    0, 0, sptr->size.x, sptr->size.y, 0, 0);
    }
  /*}}}*/
  /*{{{  create fills*/
  {
    unsigned  i;
    SPRITE    *dptr;
    SPRITE_DEF *sptr;
	
    for(i = 0, sptr = fills_def, dptr = fills;
	i != BOARD_FILLS; i++, sptr++, dptr++)
      {
	int       x, y;
	char      c;
	unsigned char *ptr;
	
	assert(sptr->size.x && sptr->size.y);
	dptr->size.x = sptr->size.x;
	dptr->size.y = sptr->size.y;
	if(data.mono)
	  {
	    if(data.swap == True)
	      for(ptr = sptr->bitmap,
		  x = (sptr->size.x + 7) / 8 * sptr->size.y; x--; ptr++)
		*ptr ^= 0xFF;
	    for(ptr = sptr->bitmap, y = sptr->size.y; y--;)
	      for(c = 0x55 << (y & 1), x = (sptr->size.x + 7) / 8; x--; ptr++)
		*ptr |= c;
	  }
	dptr->mask = XCreateBitmapFromData(display.display, root,
	    (char const *)sptr->bitmap, sptr->size.x, sptr->size.y);
	if(!dptr->mask)
	  fatal_error("Cannot create fill %d", i);
      }
  }
  /*}}}*/
  /*{{{  create score pixmaps*/
  {
    unsigned  i;
	
    for(i = BOARD_SCORES; i--;)
      {
	update.score.list[i].image = XCreatePixmap(display.display,
	    root, DIGIT_WIDTH * 4, DIGIT_HEIGHT, depth);
	update.score.list[i].mask = XCreatePixmap(display.display,
	    root, DIGIT_WIDTH * 4, DIGIT_HEIGHT, depth);
	if(!update.score.list[i].image || !update.score.list[i].mask)
	  fatal_error("Cannot create score pixmap");
      }
  }
  /*}}}*/
  XtRealizeWidget(display.toplevel);
  /* this kind of stuff makes me nervous, but I'm not sure of the
   * correct way to do what I want */
  display.window = XCreateSimpleWindow(display.display, XtWindow(widget),
      EDGING, EDGING, WINDOW_WIDTH, WINDOW_HEIGHT,
      1, display.black, display.white);
  if(!display.window)
    fatal_error("Cannot create game window");
  XSelectInput(display.display, display.window, ExposureMask |
      KeyPressMask | KeyReleaseMask | EnterWindowMask |
      (display.DEC_icon_atom == None ?
      StructureNotifyMask : PropertyChangeMask) |
      (data.pause ? LeaveWindowMask : 0));
  assert(!(VEL_X_FAST * FAST_STEPS * 2 % VEL_X));
  assert(!(VEL_Y_FAST * FAST_STEPS * 2 % VEL_Y));
#ifndef NDEBUG
  /*{{{  check we used the colours that we allocated*/
  if(data.mono == False)
    {
      unsigned  ix;
      COLOR_DEF *cptr;
      
      color_names[COLOR_FOREGROUND].name = NULL;
      color_names[COLOR_BACKGROUND].name = NULL;
      for(cptr = &color_names[4], ix = COLORS - 4; ix--; cptr++)
	assert(cptr->alloc > 2 || !cptr->name ||
	    (cptr->type & 16 && !display.dynamic));
    }
  /*}}}*/
#ifndef RANDOM_APPLES
  /*{{{  check we know where to put the apples*/
  {
    BOARD const *bptr;
    unsigned  count;
	
    for(bptr = boards, count = BOARDS; count--; bptr++)
      {
	char const *cptr;
	unsigned  apples[4];
	unsigned  ix;
	unsigned  cell;
	
	for(ix = 4; ix--; apples[ix] = 0)
	  /* EMPTY */;
	for(cptr = (char const *)bptr->map,
	    cell = (CELLS_ACROSS + 1) * CELLS_DOWN; cell--; cptr++)
	  {
	    unsigned  mask;
	    char const *p;
	    
	    p = strchr(hex_text, *cptr);
	    if(p)
	      {
		mask = p - hex_text;
		for(ix = 4; ix--;)
		  if(mask & (1 << ix))
		    apples[ix]++;
	      }
	  }
	for(ix = 4; ix--;)
	  assert(apples[ix] == INITIAL_APPLES);
      }
  }
  /*}}}*/
#endif /* RANDOM_APPLES */
#endif /* NDEBUG */
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void destroy_widget()*/
extern VOIDFUNC destroy_widget FUNCARGVOID
/*
 * frees all the resources we have allocated by create_resources
 * (Do I need to do this?)
 */
{
  unsigned  i;
  SPRITE    *sptr;
  SPRITE_DEF *dptr;

  if(display.copy)
    XFreePixmap(display.display, display.copy);
  if(display.back)
    XFreePixmap(display.display, display.back);
  /*{{{  free gcs*/
  {
    GC        *gcptr;
    unsigned  count;
	
    for(gcptr = gcs, count = GCS; count--; gcptr++)
      if(*gcptr)
	{
	  XFreeGC(display.display, *gcptr);
	  *gcptr = 0;
	}
  }
  /*}}}*/
  for(i = SPRITES, dptr = sprites_def, sptr = sprites; i--; sptr++, dptr++)
    {
      if(!dptr->copy || dptr->flags)
	{
	  if(sptr->image)
	    XFreePixmap(display.display, sptr->image);
	  if(sptr->mask)
	    XFreePixmap(display.display, sptr->mask);
	}
    }
  for(i = BOARD_FILLS, sptr = fills; i--; sptr++)
    {
      if(sptr->mask)
	XFreePixmap(display.display, sptr->mask);
    }
  for(i = BOARD_SCORES; i--;)
    {
      if(update.score.list[i].image)
	XFreePixmap(display.display, update.score.list[i].image);
      if(update.score.list[i].mask)
	XFreePixmap(display.display, update.score.list[i].mask);
    }
  if(data.mono == False)
    {
      if(display.dynamic)
	{
	  XFreeColors(display.display, display.colormap,
	      &colors[0].pixel, 1, (unsigned long)0);
	  XFreeColors(display.display, display.colormap,
	      &colors[1].pixel, 1, (unsigned long)0);
	}
      for(i = COLORS; i--;)
	if(color_names[i].alloc == 1)
	  XFreeColors(display.display, display.colormap,
	      &colors[i].pixel, 1, (unsigned long)0);
    }
  if(data.private == True)
    XFreeColormap(display.display, display.colormap);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  int error_handler(display, event)*/
static int error_handler
FUNCARG((dpy, event),
	Display *dpy
ARGSEP  XErrorEvent *event
)
{
  fprintf(stderr, "X error %d, request %d:%d\n",
      event->error_code, event->request_code, event->minor_code);
  while(QLength(display.display))
    {
      XEvent  buffered;
      
      XNextEvent(display.display, &buffered);
      fprintf(stderr, "0x%x type %d\n", buffered.xany.window, buffered.type);
    }
  fflush(stderr);
  abort();
  return 0;
}
/*}}}*/
/*{{{  void list_help(name)*/
extern VOIDFUNC list_help
FUNCARG((name),
	char const *name
)
{
  if(data.colors == True)
    /*{{{  color resources*/
    {
      COLOR_DEF const *cptr;
      unsigned  count;
      
      fprintf(stderr, "Colour resources\n");
      fprintf(stderr, "Xmris.{Mris,Msit}.{Swap}.<name>:<color>\n");
      for(cptr = &color_names[4], count = COLORS - 4; count--; cptr++)
	{
	  unsigned  mask;
	  
	  mask = cptr->alloc & 15;
	  assert(mask && mask != 6 && mask != 9);
	  if(((mask + 1) | mask) == 0xF)
	    mask = 15;
	  fputs("Xmris", stderr);
	  if(mask == 15 || mask == 5)
	    fputc('*', stderr);
	  else
	    {
	      if(!(mask & 3))
		fputs(".Msit", stderr);
	      else if(!(mask & 12))
		fputs(".Mris", stderr);
	      if(!(mask & 10))
		fputc('.', stderr);
	      else if(mask == 12 || mask == 3)
		fputc('*', stderr);
	      else if(mask == 10)
		fputs("*Swap.", stderr);
	      else
		fputs(".Swap.", stderr);
	    }
	  fputs(cptr->name, stderr);
	  fputs(":\n", stderr);
	}
    }
    /*}}}*/
  else
    /*{{{  options*/
    {
      HELP const *hptr;
      TITLE const *tptr;
      
      fprintf(stderr, "%s [-option ...] [-toolkitoption ...]\n", name);
      for(tptr = title_text; tptr->text; tptr++)
	if(strchr(tptr->text, '('))
	  {
	    fprintf(stderr, "xmris %s %s\n", title_text[0].text, tptr->text);
	    break;
	  }
      fprintf(stderr, " Option    Resource         Argument\n");
      for(hptr = help; hptr->help; hptr++)
	fprintf(stderr, "-%-9s %-16s %-16s %s\n",
	    hptr->arg, hptr->resource, hptr->option, hptr->help);
    }
    /*}}}*/
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void make_color_image(dptr, sptr, root, depth, bitmap)*/
static VOIDFUNC make_color_image
FUNCARG((dptr, sptr, root, depth, bitmap),
	SPRITE    *dptr
ARGSEP  SPRITE_DEF *sptr
ARGSEP  Window    root
ARGSEP  unsigned  depth
ARGSEP  Pixmap    bitmap
)
/*
 * The colour sprite image is generated from the bitmap, which
 * is treated as a set of n planes vertically alligned.
 * The allows 2^n different colours per sprite (including the mask). The
 * mapping from the sprite planes to the colour map is
 * controlled by the sprite's colour table, which pointes the game's
 * colour table, which contains the pixel value to use.
 * We go through the bit planes, once for each colour, generating colour
 * stencils which we or together for the image. Rather like colour
 * printing really.
 */
{
  Pixmap    temp;
  
  temp = XCreatePixmap(display.display, root,
      dptr->size.x, dptr->size.y, depth);
  if(temp);
    {
      unsigned  ix;
  
      XSetForeground(display.display, GCN(GC_BOARD), color_zero);
      XFillRectangle(display.display, dptr->image, GCN(GC_BOARD),
	  0, 0, dptr->size.x, dptr->size.y);
      for(ix = 1 << sptr->planes; --ix;)
	{
	  unsigned  count;
	  
	  count = sptr->colors[data.swap == True][ix];
	  if(count != ~(unsigned long)0)
	    {
	      {
		/* stupid compilers with broken stringizizing
		 * and stupid compilers which don't understand \
		 * outside of #define */
		unsigned  alloc;
		
		alloc = color_names[count].alloc;
		assert(alloc == 1 || alloc == 2);
	      }
#ifndef NDEBUG
	      color_names[count].name = NULL;
#endif /* NDEBUG */
	      XSetForeground(display.display, GCN(GC_BOARD),
		  colors[count].pixel);
	      XFillRectangle(display.display, temp, GCN(GC_BOARD),
		  0, 0, dptr->size.x, dptr->size.y);
	      for(count = sptr->planes; count--;)
		XCopyArea(display.display, bitmap, temp,
		    GCN(ix & (1 << count) ? GC_AND : GC_MASK),
		    0, (int)(count * dptr->size.y),
		    dptr->size.x, dptr->size.y, 0, 0);
	      XCopyArea(display.display, temp, dptr->image, GCN(GC_OR),
		  0, 0, dptr->size.x, dptr->size.y, 0, 0);
	    }
	}
      XFreePixmap(display.display, temp);
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void make_mask_image(dptr, sptr, root, depth, bitmap)*/
static VOIDFUNC make_mask_image
FUNCARG((dptr, sptr, root, depth, bitmap),
	SPRITE  *dptr
ARGSEP  SPRITE_DEF *sptr
ARGSEP  Window    root
ARGSEP  unsigned  depth
ARGSEP  Pixmap    bitmap
)
/*
 * the mask is made from logical color ~0, ie or all the bitmap
 * planes together, and that's the mask.
 * the mask halo is generated from the mask by oring the mask ontop
 * of itself, offset by the 4 adjacent pixels. This grows the mask too.
 * the halo is the xor of the grown mask and the original mask.
 */
{
  unsigned  ix;
  
  XSetForeground(display.display, GCN(GC_BOARD), color_zero);
  XFillRectangle(display.display, dptr->mask, GCN(GC_BOARD),
      0, 0, dptr->size.x, dptr->size.y);
  for(ix = sptr->planes; ix--;)
    XCopyArea(display.display, bitmap, dptr->mask, GCN(GC_OR),
	0, (int)(dptr->size.y * ix), dptr->size.x, dptr->size.y, 0, 0);
  XCopyArea(display.display,
      dptr->mask, dptr->image, GCN(GC_AND),
      0, 0, dptr->size.x, dptr->size.y, 0, 0);
  if(sptr->flags & BORDER_HALO_MASK)
    /*{{{  make the halo*/
    {
      Pixmap    halo;
      
      halo = XCreatePixmap(display.display, root,
	  dptr->size.x, dptr->size.y, depth);
      if(halo)
	{
	  XFillRectangle(display.display, halo, GCN(GC_BOARD),
	      0, 0, dptr->size.x, dptr->size.y);
	  XCopyArea(display.display, dptr->mask, halo, GCN(GC_OR),
	      0, 0, dptr->size.x, dptr->size.y, 0, 1);
	  XCopyArea(display.display, dptr->mask, halo, GCN(GC_OR),
	      0, 0, dptr->size.x, dptr->size.y, 0, -1);
	  XCopyArea(display.display, dptr->mask, halo, GCN(GC_OR),
	      0, 0, dptr->size.x, dptr->size.y, 1, 0);
	  XCopyArea(display.display, dptr->mask, halo, GCN(GC_OR),
	      0, 0, dptr->size.x, dptr->size.y, -1, 0);
	  XCopyArea(display.display, dptr->mask, halo, GCN(GC_MASK),
	      0, 0, dptr->size.x, dptr->size.y, 0, 0);
	  XCopyArea(display.display, halo, dptr->mask, GCN(GC_OR),
	      0, 0, dptr->size.x, dptr->size.y, 0, 0);
	  XSetForeground(display.display, GCN(GC_BOARD),
	      data.swap == True && data.mono == True &&
	      sptr->flags & BORDER_OUTER_EDGE_MASK ?
	      display.black : display.white);
	  XSetFunction(display.display, GCN(GC_BOARD), GXand);
	  XFillRectangle(display.display, halo, GCN(GC_BOARD),
	      0, 0, dptr->size.x, dptr->size.y);
	  XSetFunction(display.display, GCN(GC_BOARD), GXcopy);
	  XCopyArea(display.display, halo, dptr->image, GCN(GC_OR),
	      0, 0, dptr->size.x, dptr->size.y, 0, 0);
	  XFreePixmap(display.display, halo);
	}
    }
    /*}}}*/
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void make_mono_image(dptr, sptr, root, depth, bitmap)*/
static VOIDFUNC make_mono_image
FUNCARG((dptr, sptr, root, depth, bitmap),
	SPRITE  *dptr
ARGSEP  SPRITE_DEF *sptr
ARGSEP  Window    root
ARGSEP  unsigned  depth
ARGSEP  Pixmap    bitmap
)
/*
 * As with the colour sprite image, the monochrome is generated from the
 * same bitmap and logical colors, but these either map to black or white
 * only.
 */
{
  Pixmap    temp;
  unsigned long colors;
  
  temp = XCreatePixmap(display.display, root,
      dptr->size.x, dptr->size.y, depth);
  if(temp);
    {
      unsigned  ix;
  
      XSetForeground(display.display, GCN(GC_BOARD), color_zero);
      XFillRectangle(display.display, dptr->image, GCN(GC_BOARD),
	  0, 0, dptr->size.x, dptr->size.y);
      colors = sptr->colors[data.swap == True][0];
      if(data.swap == True)
	colors = ~colors;
      for(ix = 1 << sptr->planes; ix; ix--)
	{
	  unsigned  count;
	  
	  if(sptr->colors[data.swap == True][ix] != ~(unsigned long)0)
	    {
	      XSetForeground(display.display, GCN(GC_BOARD),
		  colors & (unsigned long)1 << ix ?
		  display.black : display.white);
	      XFillRectangle(display.display, temp, GCN(GC_BOARD),
		  0, 0, dptr->size.x, dptr->size.y);
	      for(count = sptr->planes; count--;)
		XCopyArea(display.display, bitmap, temp,
		    GCN(ix & (1 << count) ? GC_AND : GC_MASK),
		    0, (int)(count * dptr->size.y),
		    dptr->size.x, dptr->size.y, 0, 0);
	      XCopyArea(display.display, temp, dptr->image, GCN(GC_OR),
		  0, 0, dptr->size.x, dptr->size.y, 0, 0);
	    }
	}
      XFreePixmap(display.display, temp);
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void open_toolkit(arc, argv)*/
extern VOIDFUNC open_toolkit
FUNCARG((argc, argv),
	int     argc
ARGSEP  char    **argv
)
{
#if !__STDC__
  /*{{{  K&R string fixups*/
  {
    char      *ptr;
    
    ptr = malloc(strlen(XMRISVERSION) + strlen(DATE) +
	strlen(title_text[0].text));
    sprintf(ptr, title_text[0].text, XMRISVERSION, DATE);
    title_text[0].text = ptr;
    ptr = malloc(strlen(FONT_NAME) + strlen(fallback[0]));
    sprintf(ptr, fallback[0], FONT_NAME);
    fallback[0] = ptr;
  }
  /*}}}*/
#endif /* __STDC__ */
  default_gender = GAME_GENDER;
  /*{{{  determine gender of the game*/
  if(*argv)
    {
      static char const *names[2] = {"xmris", "xmsit"};
      size_t    length;
      size_t    other;
      unsigned  ix;
	
      length = strlen(*argv);
      for(ix = 2; ix--;)
	{
	  other = strlen(&names[ix][1]);
	  if(length >= other &&
	      !strcmp(&(*argv)[length - other], &names[ix][1]))
	    default_gender = ix;
	}
    }
  /*}}}*/
  {
    XrmOptionDescRec const *ptr;
    
    for(ptr = options; *(char const **)ptr; ptr++)
      /* EMPTY */;
    display.toplevel = XtAppInitialize (&display.context, "Xmris",
	options, ptr - options, &argc, argv,
	fallback, toplevel_args, XtNumber(toplevel_args));
  }
  if(!display.toplevel)
    fatal_error("Cannot initialize toolkit");
  XtAppSetTypeConverter(display.context, XtRString, XtRGender,
      convert_string2gender, (XtConvertArgRec *)NULL, 0, XtCacheNone,
      (void (*)PROTOARG((XtAppContext, XrmValue *, XtPointer,
	  XrmValue *, Cardinal *)))NULL);
  XtAppSetTypeConverter(display.context, XtRString, XtRKeySym,
      convert_string2keysym, (XtConvertArgRec *)NULL, 0, XtCacheNone,
      (void (*)PROTOARG((XtAppContext, XrmValue *, XtPointer,
	  XrmValue *, Cardinal *)))NULL);
  display.display = XtDisplay(display.toplevel);
  display.screen = DefaultScreen(display.display);
  {
    XtResource const *ptr;
    
    for(ptr = resources; *(char const **)ptr; ptr++)
      /* EMPTY */;
    XtGetApplicationResources(display.toplevel, &data,
      resources, ptr - resources, (Arg *)NULL, 0);
  }
  if(argc > 1)
    {
      data.help = True;
      data.colors = False;
      fprintf(stderr, "%s: Unknown option '%s'\n", argv[0], argv[1]);
    }
  XSetErrorHandler(error_handler);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void set_colors(flag)*/
extern VOIDFUNC set_colors
FUNCARG((flag),
	int       flag
)
/*
 * set the board background colours to be what they ought. Either
 * the screen's specific, or the uniform colour for when the drones are
 * abroad. Or something else if we're stoned.
 */
{
  if(display.dynamic)
    {
      if(flag >= 0)
	/*{{{  set specific or drone*/
	{
	  unsigned  ix;
	  XColor const *sources[2];
	
	  if(flag || data.gender == True)
	    {
	      sources[0] = &colors[backgrounds[0]];
	      sources[1] = &colors[backgrounds[1]];
	    }
	  else
	    {
	      sources[0] = &colors[COLOR_DRONE_BACK];
	      sources[1] = &colors[COLOR_DRONE_FORE];
	    }
	  for(ix = 2; ix--;)
	    {
	      colors[ix].red = sources[ix]->red;
	      colors[ix].green = sources[ix]->green;
	      colors[ix].blue = sources[ix]->blue;
	      colors[ix].flags = DoRed | DoGreen | DoBlue;
	    }
	  flag = 1;
	}
	/*}}}*/
      else
	/*{{{  whizz*/
	{
	  /*
	   * XStoreColor overwrites the requested color with the best
	   * the visual can do, this may round back down to the start
	   * color, thus we must keep a copy of the desired color to
	   * avoid this rounding error
	   */
	  static unsigned store[6];
	  unsigned  ix;
	  
	  if(global.whizz == 1)
	    /*{{{  initialize*/
	    {
	      store[0] = colors[0].red;
	      store[1] = colors[0].green;
	      store[2] = colors[0].blue;
	      store[3] = colors[1].red;
	      store[4] = colors[1].green;
	      store[5] = colors[1].blue;
	      global.whizz = WHIZZ_CYCLES * WHIZZ_STEPS * 6 + 2;
	      flag = 0;
	    }
	    /*}}}*/
	  else if(global.whizz > 1 && (global.whizz - 2) % WHIZZ_CYCLES)
	    /*{{{  pause*/
	    {
	      if(global.whizz++ == WHIZZ_CYCLES * WHIZZ_STEPS * 6 +
		  WHIZZ_CYCLES + 1)
		global.whizz = WHIZZ_CYCLES + 1;
	      flag = 0;
	    }
	    /*}}}*/
	  else
	    /*{{{  whizz*/
	    {
	      unsigned  target[6];
	      unsigned  arrived;
	      
	      /*{{{  set target*/
	      if(global.whizz == 2)
		{
		  target[0] = colors[backgrounds[0]].red;
		  target[1] = colors[backgrounds[0]].blue;
		  target[2] = colors[backgrounds[0]].red;
		  target[3] = colors[backgrounds[1]].red;
		  target[4] = colors[backgrounds[1]].blue;
		  target[5] = colors[backgrounds[1]].red;
		}
	      else
		{
		  unsigned  cycle;
		  unsigned  a, b, c;
		  
		  ix = (global.whizz - 3) / WHIZZ_CYCLES % WHIZZ_STEPS *
		      WHIZZ_SIZE;
		  cycle = (global.whizz - 3) / WHIZZ_CYCLES / WHIZZ_STEPS;
		  if(cycle & 1)
		    a = 0xFFFF - ix, b = 0xFFFF, c = 0;
		  else
		    a = 0xFFFF, b = ix, c = 0;
		  if(cycle / 2 == 1)
		    c = b, b = a, a = 0;
		  else if(cycle / 2 == 2)
		    c = a, a = b, b = 0;
		  target[1] = a;
		  target[2] = b;
		  target[3] = c;
		  target[4] = 0xFFFF - a;
		  target[5] = 0xFFFF - b;
		  target[6] = 0xFFFF - c;
		  global.whizz++;
		}
	      /*}}}*/
	      arrived = 1;
	      /*{{{  move*/
	      for(ix = 6; ix--;)
		if(target[ix] > store[ix])
		  {
		    if(target[ix] - store[ix] <= WHIZZ_SIZE)
		      store[ix] = target[ix];
		    else
		      store[ix] += WHIZZ_SIZE;
		    arrived = 0;
		  }
		else if(target[ix] < store[ix])
		  {
		    if(store[ix] - target[ix] <= WHIZZ_SIZE)
		      store[ix] = target[ix];
		    else
		      store[ix] -= WHIZZ_SIZE;
		    arrived = 0;
		  }
	      /*}}}*/
	      colors[0].red = store[0];
	      colors[0].green = store[1];
	      colors[0].blue = store[2];
	      colors[1].red = store[3];
	      colors[1].green = store[4];
	      colors[1].blue = store[5];
	      if(arrived && global.whizz == 2)
		global.whizz = 0;
	    }
	    /*}}}*/
	}
	/*}}}*/
      /*{{{  store*/
      if(flag)
	{
	  XStoreColor(display.display, display.colormap, &colors[0]);
	  XStoreColor(display.display, display.colormap, &colors[1]);
	}
      /*}}}*/
    }
  return VOIDRETURN;
}
/*}}}*/
