/*
 * File:      wb_gdi.cc
 * Purpose:     GDI (Graphics Device Interface) objects and functions
 * Author:      Julian Smart
 * Created:     1993
 * Updated:     August 1994
 * RCS_ID:      $Id: wb_gdi.cc,v 1.4 1994/08/14 22:59:35 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_list.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_gdi.h"
#endif

#ifdef wx_x
extern Colormap wxMainColormap;
#endif
#ifdef wx_xview
extern Xv_Server xview_server;
#endif

wxbFont::wxbFont (void)
{
  WXSET_TYPE(wxFont, wxTYPE_FONT)
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
wxbFont::wxbFont (int WXUNUSED(PointSize), int WXUNUSED(Family), int WXUNUSED(Style), int WXUNUSED(Weight), Bool WXUNUSED(Underline),
  const char *WXUNUSED(Face))
{
  WXSET_TYPE(wxFont, wxTYPE_FONT)
}

wxbFont::~wxbFont ()
{
}

char *wxbFont::GetFamilyString(void)
{
  char *fam = NULL;
  switch (GetFamily())
  {
    case wxDECORATIVE:
      fam = "wxDECORATIVE";
      break;
    case wxROMAN:
      fam = "wxROMAN";
      break;
    case wxSCRIPT:
      fam = "wxSCRIPT";
      break;
    case wxSWISS:
      fam = "wxSWISS";
      break;
    case wxMODERN:
      fam = "wxMODERN";
      break;
    case wxTELETYPE:
      fam = "wxTELETYPE";
      break;
    default:
      fam = "wxDEFAULT";
      break;
  }
  return fam;
}

/* New font system */
char *wxbFont::GetFaceName(void)
{
#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)
  return wxTheFontNameDirectory.GetFontName(fontid); 
#else
  return faceName;
#endif
}

char *wxbFont::GetStyleString(void)
{
  char *styl = NULL;
  switch (GetStyle())
  {
    case wxITALIC:
      styl = "wxITALIC";
      break;
    case wxSLANT:
      styl = "wxSLANT";
      break;
    default:
      styl = "wxNORMAL";
      break;
  }
  return styl;
}

char *wxbFont::GetWeightString(void)
{
  char *w = NULL;
  switch (GetWeight())
  {
    case wxBOLD:
      w = "wxBOLD";
      break;
    case wxLIGHT:
      w = "wxLIGHT";
      break;
    default:
      w = "wxNORMAL";
      break;
  }
  return w;
}

// Colour

wxColour::wxColour (void)
{
  WXSET_TYPE(wxColour, wxTYPE_COLOUR)
  
  isInit = FALSE;
#ifdef wx_x
  pixel = -1;
#endif
#ifdef wx_msw
  pixel = 0;
#endif
  red = blue = green = 0;
//  wxTheColourList->Append (this);
}

wxColour::wxColour (const unsigned char r, const unsigned char g, const unsigned char b)
{
  WXSET_TYPE(wxColour, wxTYPE_COLOUR)
  
  red = r;
  green = g;
  blue = b;
  isInit = TRUE;
#ifdef wx_x
  pixel = -1;
#endif
#ifdef wx_msw
  pixel = PALETTERGB (red, green, blue);
#endif
//  wxTheColourList->Append (this);
}

wxColour::wxColour (const wxColour& col)
{
  WXSET_TYPE(wxColour, wxTYPE_COLOUR)
  
  red = col.red;
  green = col.green;
  blue = col.blue;
  isInit = col.isInit;
  pixel = col.pixel;
}

wxColour& wxColour::operator =(const wxColour& col)
{
  WXSET_TYPE(wxColour, wxTYPE_COLOUR)
  
  red = col.red;
  green = col.green;
  blue = col.blue;
  isInit = col.isInit;
  pixel = col.pixel;
  return *this;
}

wxColour::wxColour (const char *col)
{
  WXSET_TYPE(wxColour, wxTYPE_COLOUR)
  
  wxColour *the_colour = wxTheColourDatabase->FindColour (col);
  if (the_colour)
    {
      red = the_colour->Red ();
      green = the_colour->Green ();
      blue = the_colour->Blue ();
      isInit = TRUE;
    }
  else
    {
      red = 0;
      green = 0;
      blue = 0;
      isInit = FALSE;
    }
#ifdef wx_x
  pixel = -1;
#endif
#ifdef wx_msw
  pixel = PALETTERGB (red, green, blue);
#endif
//  wxTheColourList->Append (this);
}

wxColour::~wxColour (void)
{
//  wxTheColourList->DeleteObject (this);
}

wxColour& wxColour::operator = (const char *col)
{
  wxColour *the_colour = wxTheColourDatabase->FindColour (col);
  if (the_colour)
    {
      red = the_colour->Red ();
      green = the_colour->Green ();
      blue = the_colour->Blue ();
      isInit = TRUE;
    }
  else
    {
      red = 0;
      green = 0;
      blue = 0;
      isInit = FALSE;
    }
#ifdef wx_x
  pixel = -1;
#endif
#ifdef wx_msw
  pixel = PALETTERGB (red, green, blue);
#endif
  return (*this);
}

void wxColour::Set (unsigned char r, unsigned char g, unsigned char b)
{
  red = r;
  green = g;
  blue = b;
  isInit = TRUE;
#ifdef wx_x
  pixel = -1;
#endif
#ifdef wx_msw
  pixel = PALETTERGB (red, green, blue);
#endif

}

void wxColour::Get (unsigned char *r, unsigned char *g, unsigned char *b)
{
  *r = red;
  *g = green;
  *b = blue;
}

wxColourDatabase::wxColourDatabase (int type):
wxList (type)
{
}

wxColourDatabase::~wxColourDatabase (void)
{
  // Cleanup Colour allocated in Initialize()
  wxNode *node = First ();
  while (node)
    {
      wxColour *col = (wxColour *) node->Data ();
      wxNode *next = node->Next ();
      delete col;
      node = next;
    }
}

// Colour database stuff
void wxColourDatabase::Initialize (void)
{
  // Don't initialize for X: colours are found
  // in FindColour below.
#ifdef wx_msw
  struct cdef {
   char *name;
   int r,g,b;
  };
  cdef cc;
  static cdef table[]={
   {"AQUAMARINE",112, 219, 147},
   {"BLACK",0, 0, 0},
   {"BLUE", 0, 0, 255},
   {"BLUE VIOLET", 159, 95, 159},
   {"BROWN", 165, 42, 42},
   {"CADET BLUE", 95, 159, 159},
   {"CORAL", 255, 127, 0},
   {"CORNFLOWER BLUE", 66, 66, 111},
   {"CYAN", 0, 255, 255},
   {"DARK GREY", 47, 47, 47},   // ?

   {"DARK GREEN", 47, 79, 47},
   {"DARK OLIVE GREEN", 79, 79, 47},
   {"DARK ORCHID", 153, 50, 204},
   {"DARK SLATE BLUE", 107, 35, 142},
   {"DARK SLATE GREY", 47, 79, 79},
   {"DARK TURQUOISE", 112, 147, 219},
   {"DIM GREY", 84, 84, 84},
   {"FIREBRICK", 142, 35, 35},
   {"FOREST GREEN", 35, 142, 35},
   {"GOLD", 204, 127, 50},
   {"GOLDENROD", 219, 219, 112},
   {"GREY", 128, 128, 128},
   {"GREEN", 0, 255, 0},
   {"GREEN YELLOW", 147, 219, 112},
   {"INDIAN RED", 79, 47, 47},
   {"KHAKI", 159, 159, 95},
   {"LIGHT BLUE", 191, 216, 216},
   {"LIGHT GREY", 192, 192, 192},
   {"LIGHT STEEL BLUE", 143, 143, 188},
   {"LIME GREEN", 50, 204, 50},
   {"LIGHT MAGENTA", 255, 0, 255},
   {"MAGENTA", 255, 0, 255},
   {"MAROON", 142, 35, 107},
   {"MEDIUM AQUAMARINE", 50, 204, 153},
   {"MEDIUM GREY", 100, 100, 100},
   {"MEDIUM BLUE", 50, 50, 204},
   {"MEDIUM FOREST GREEN", 107, 142, 35},
   {"MEDIUM GOLDENROD", 234, 234, 173},
   {"MEDIUM ORCHID", 147, 112, 219},
   {"MEDIUM SEA GREEN", 66, 111, 66},
   {"MEDIUM SLATE BLUE", 127, 0, 255},
   {"MEDIUM SPRING GREEN", 127, 255, 0},
   {"MEDIUM TURQUOISE", 112, 219, 219},
   {"MEDIUM VIOLET RED", 219, 112, 147},
   {"MIDNIGHT BLUE", 47, 47, 79},
   {"NAVY", 35, 35, 142},
   {"ORANGE", 204, 50, 50},
   {"ORANGE RED", 255, 0, 127},
   {"ORCHID", 219, 112, 219},
   {"PALE GREEN", 143, 188, 143},
   {"PINK", 188, 143, 234},
   {"PLUM", 234, 173, 234},
   {"PURPLE", 176, 0, 255},
   {"RED", 255, 0, 0},
   {"SALMON", 111, 66, 66},
   {"SEA GREEN", 35, 142, 107},
   {"SIENNA", 142, 107, 35},
   {"SKY BLUE", 50, 153, 204},
   {"SLATE BLUE", 0, 127, 255},
   {"SPRING GREEN", 0, 255, 127},
   {"STEEL BLUE", 35, 107, 142},
   {"TAN", 219, 147, 112},
   {"THISTLE", 216, 191, 216},
   {"TURQUOISE", 173, 234, 234},
   {"VIOLET", 79, 47, 79},
   {"VIOLET RED", 204, 50, 153},
   {"WHEAT", 216, 216, 191},
   {"WHITE", 255, 255, 255},
   {"YELLOW", 255, 255, 0},
   {"YELLOW GREEN", 153, 204, 50},
   {0,0,0,0}
  };
  int i;
  for (i=0;cc=table[i],cc.name!=0;i++)
  {
    Append(cc.name,new wxColour(cc.r,cc.g,cc.b));
  }
#endif
}

/*
 * Changed by Ian Brown, July 1994.
 *
 * When running under X, the Colour Database starts off empty. The X server
 * is queried for the colour first time after which it is entered into the
 * database. This allows our client to use the server colour database which
 * is hopefully gamma corrected for the display being used.
 */

wxColour *wxColourDatabase::FindColour(const char *colour)
{
  wxNode *node = Find(colour);
  if (node)
    return (wxColour *)node->Data();
#ifdef wx_msw
  else return NULL;
#else
  else {
    XColor xcolour;

#ifdef wx_motif
    Display *display = XtDisplay(wxTheApp->topLevel) ;
#endif
#ifdef wx_xview
    Xv_Screen screen = xv_get(xview_server, SERVER_NTH_SCREEN, 0);
    Xv_opaque root_window = xv_get(screen, XV_ROOT);
    Display *display = (Display *)xv_get(root_window, XV_DISPLAY);
#endif

    /* MATTHEW: [4] Use wxGetMainColormap */
    if (!XParseColor(display, wxGetMainColormap(display), colour,&xcolour))
      return NULL;

    unsigned char r = (unsigned char)(xcolour.red >> 8);
    unsigned char g = (unsigned char)(xcolour.green >> 8);
    unsigned char b = (unsigned char)(xcolour.blue >> 8);

    wxColour *col = new wxColour(r, g, b);
    Append(colour, col);

    return col;
  }
#endif
}

/* Old FindColour
wxColour *wxColourDatabase::FindColour (const char *colour)
{
  wxNode *node = Find (colour);
  return node ? (wxColour *) node->Data () : NULL ;
}
*/

char *wxColourDatabase::FindName (wxColour& colour)
{
  unsigned char red = colour.Red ();
  unsigned char green = colour.Green ();
  unsigned char blue = colour.Blue ();

  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxColour *col = (wxColour *) node->Data ();
      if (col->Red () == red && col->Green () == green && col->Blue () == blue)
	{
	  char *found = node->key.string;
	  if (found)
	    return found;
	}
    }
  return NULL;			// Not Found

}


void 
wxInitializeStockObjects (void)
{
  wxTheBrushList = new wxBrushList;
  wxThePenList = new wxPenList;
  wxTheFontList = new wxFontList;
  wxTheBitmapList = new wxGDIList;
  // wxTheColourList =  new wxGDIList;

#ifdef wx_motif
#endif
#ifdef wx_x
  wxFontPool = new XFontPool;
#endif

  wxNORMAL_FONT = new wxFont (12, wxMODERN, wxNORMAL, wxNORMAL);
  wxSMALL_FONT = new wxFont (10, wxSWISS, wxNORMAL, wxNORMAL);
  wxITALIC_FONT = new wxFont (12, wxROMAN, wxITALIC, wxNORMAL);
  wxSWISS_FONT = new wxFont (12, wxSWISS, wxNORMAL, wxNORMAL);

  wxRED_PEN = new wxPen ("RED", 1, wxSOLID);
  wxCYAN_PEN = new wxPen ("CYAN", 1, wxSOLID);
  wxGREEN_PEN = new wxPen ("GREEN", 1, wxSOLID);
  wxBLACK_PEN = new wxPen ("BLACK", 1, wxSOLID);
  wxWHITE_PEN = new wxPen ("WHITE", 1, wxSOLID);
  wxTRANSPARENT_PEN = new wxPen ("BLACK", 1, wxTRANSPARENT);
  wxBLACK_DASHED_PEN = new wxPen ("BLACK", 1, wxSHORT_DASH);
  wxGREY_PEN = new wxPen ("GREY", 1, wxSOLID);
  wxMEDIUM_GREY_PEN = new wxPen ("MEDIUM GREY", 1, wxSOLID);
  wxLIGHT_GREY_PEN = new wxPen ("LIGHT GREY", 1, wxSOLID);

  wxBLUE_BRUSH = new wxBrush ("BLUE", wxSOLID);
  wxGREEN_BRUSH = new wxBrush ("GREEN", wxSOLID);
  wxWHITE_BRUSH = new wxBrush ("WHITE", wxSOLID);
  wxBLACK_BRUSH = new wxBrush ("BLACK", wxSOLID);
  wxTRANSPARENT_BRUSH = new wxBrush ("BLACK", wxTRANSPARENT);
  wxCYAN_BRUSH = new wxBrush ("CYAN", wxSOLID);
  wxRED_BRUSH = new wxBrush ("RED", wxSOLID);
  wxGREY_BRUSH = new wxBrush ("GREY", wxSOLID);
  wxMEDIUM_GREY_BRUSH = new wxBrush ("MEDIUM GREY", wxSOLID);
  wxLIGHT_GREY_BRUSH = new wxBrush ("LIGHT GREY", wxSOLID);

  wxBLACK = new wxColour ("BLACK");
  wxWHITE = new wxColour ("WHITE");
  wxRED = new wxColour ("RED");
  wxBLUE = new wxColour ("BLUE");
  wxGREEN = new wxColour ("GREEN");
  wxCYAN = new wxColour ("CYAN");
  wxLIGHT_GREY = new wxColour ("LIGHT GREY");

  wxSTANDARD_CURSOR = new wxCursor (wxCURSOR_ARROW);
  wxHOURGLASS_CURSOR = new wxCursor (wxCURSOR_WAIT);
  wxCROSS_CURSOR = new wxCursor (wxCURSOR_CROSS);
}

void 
wxDeleteStockObjects (void)
{
  if (wxBLACK)
    delete wxBLACK;
  if (wxWHITE)
    delete wxWHITE;
  if (wxRED)
    delete wxRED;
  if (wxBLUE)
    delete wxBLUE;
  if (wxGREEN)
    delete wxGREEN;
  if (wxCYAN)
    delete wxCYAN;
  if (wxLIGHT_GREY)
    delete wxLIGHT_GREY;

  if (wxSTANDARD_CURSOR)
    delete wxSTANDARD_CURSOR;
  if (wxHOURGLASS_CURSOR)
    delete wxHOURGLASS_CURSOR;
  if (wxCROSS_CURSOR)
    delete wxCROSS_CURSOR;
}

// Pens

wxbPen::wxbPen (void)
{
  WXSET_TYPE(wxPen, wxTYPE_PEN)
}

wxbPen::~wxbPen ()
{
}

wxbPen::wxbPen (wxColour& WXUNUSED(col), int WXUNUSED(Width), int WXUNUSED(Style))
{
  WXSET_TYPE(wxPen, wxTYPE_PEN)
}

wxbPen::wxbPen (const char *WXUNUSED(col), int WXUNUSED(Width), int WXUNUSED(Style))
{
  WXSET_TYPE(wxPen, wxTYPE_PEN)
}

int wxbPen::GetWidth (void)
{
  return width;
}

int wxbPen::GetStyle (void)
{
  return style;
}

int wxbPen::GetJoin (void)
{
  return join;
}

wxBitmap *wxbPen::GetStipple (void)
{
  return stipple;
}

int wxbPen::GetCap (void)
{
  return cap;
}

int wxbPen::GetDashes (wxDash ** ptr)
{
  *ptr = dash;
  return nb_dash;
}

wxColour& wxbPen::GetColour (void)
{
  return colour;
}

void wxbPen::SetColour (wxColour& col)
{
  colour = col;
}

void wxbPen::SetColour (const char *col)
{
  colour = col;
}

void wxbPen::SetColour (char red, char green, char blue)
{
 colour.Set(red, green, blue);
}

void wxbPen::SetWidth (int Width)
{
  width = Width;
}

void wxbPen::SetCap (int Cap)
{
  cap = Cap;
}

void wxbPen::SetJoin (int Join)
{
  join = Join;
}

void wxbPen::SetStyle (int Style)
{
  style = Style;
}

void wxbPen::SetDashes (int nbDash, wxDash * Dash)
{
  nb_dash = nbDash;
  dash = Dash;
}

void wxbPen::SetStipple (wxBitmap * Stipple)
{
  stipple = Stipple;
}

// Brushes

wxbBrush::wxbBrush (void)
{
  WXSET_TYPE(wxBrush, wxTYPE_BRUSH)
}

wxbBrush::~wxbBrush ()
{
}

wxbBrush::wxbBrush (wxColour& WXUNUSED(col), int WXUNUSED(Style))
{
  WXSET_TYPE(wxBrush, wxTYPE_BRUSH)
}

wxbBrush::wxbBrush (char *WXUNUSED(col), int WXUNUSED(Style))
{
  WXSET_TYPE(wxBrush, wxTYPE_BRUSH)
}

int wxbBrush::GetStyle (void)
{
  return style;
}

wxBitmap *wxbBrush::GetStipple (void)
{
  return stipple;
}

wxColour& wxbBrush::GetColour (void)
{
  return colour;
}

void wxbBrush::SetColour (wxColour& col)
{
  colour = col;
}

void wxbBrush::SetColour (const char *col)
{
  colour = col;
}

void wxbBrush::SetColour (char red, char green, char blue)
{
  colour.Set(red, green, blue);
}

void wxbBrush::SetStyle (int Style)
{
  style = Style;
}

void wxbBrush::SetStipple (wxBitmap * Stipple)
{
  stipple = Stipple;
}

wxGDIList::wxGDIList (void)
{
}

wxGDIList::~wxGDIList (void)
{
// #ifndef wx_x
  wxNode *node = First ();
  while (node)
    {
      wxObject *object = (wxObject *) node->Data ();
      wxNode *next = node->Next ();
      delete object;
      node = next;
    }
// #endif
}

// Pen and Brush lists
wxPenList::~wxPenList (void)
{
  wxNode *node = First ();
  while (node)
    {
      wxPen *pen = (wxPen *) node->Data ();
      wxNode *next = node->Next ();
      delete pen;
      node = next;
    }
}

void wxPenList::AddPen (wxPen * pen)
{
  Append (pen);
}

void wxPenList::RemovePen (wxPen * pen)
{
  DeleteObject (pen);
}

wxPen *wxPenList::FindOrCreatePen (wxColour * colour, int width, int style)
{
  if (!colour)
    return NULL;

  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxPen *each_pen = (wxPen *) node->Data ();
      if (each_pen &&
	  each_pen->GetWidth () == width &&
	  each_pen->GetStyle () == style &&
	  each_pen->GetColour ().Red () == colour->Red () &&
	  each_pen->GetColour ().Green () == colour->Green () &&
	  each_pen->GetColour ().Blue () == colour->Blue ())
	return each_pen;
    }
  return new wxPen (*colour, width, style);
}

wxPen *wxPenList::FindOrCreatePen (char *colour, int width, int style)
{
  wxColour *the_colour = wxTheColourDatabase->FindColour (colour);
  if (the_colour)
    return FindOrCreatePen (the_colour, width, style);
  else
    return NULL;
}

wxBrushList::~wxBrushList (void)
{
  wxNode *node = First ();
  while (node)
    {
      wxBrush *brush = (wxBrush *) node->Data ();
      wxNode *next = node->Next ();
      delete brush;
      node = next;
    }
}

void wxBrushList::AddBrush (wxBrush * brush)
{
  Append (brush);
}

wxBrush *wxBrushList::FindOrCreateBrush (wxColour * colour, int style)
{
  if (!colour)
    return NULL;

  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxBrush *each_brush = (wxBrush *) node->Data ();
      if (each_brush &&
	  each_brush->GetStyle () == style &&
	  each_brush->GetColour ().Red () == colour->Red () &&
	  each_brush->GetColour ().Green () == colour->Green () &&
	  each_brush->GetColour ().Blue () == colour->Blue ())
	return each_brush;
    }
  return new wxBrush (*colour, style);
}

wxBrush *wxBrushList::FindOrCreateBrush (char *colour, int style)
{
  wxColour *the_colour = wxTheColourDatabase->FindColour (colour);
  if (the_colour)
    return FindOrCreateBrush (the_colour, style);
  else
    return NULL;
}


void wxBrushList::RemoveBrush (wxBrush * brush)
{
  DeleteObject (brush);
}

wxFontList::~wxFontList (void)
{
  wxNode *node = First ();
  while (node)
    {
      wxFont *font = (wxFont *) node->Data ();
      wxNode *next = node->Next ();
      delete font;
      node = next;
    }
}

void wxFontList::AddFont (wxFont * font)
{
  Append (font);
}

void wxFontList::RemoveFont (wxFont * font)
{
  DeleteObject (font);
}

wxFont *wxFontList::
FindOrCreateFont (int PointSize, int FamilyOrFontId, int Style, int Weight, Bool underline, const char *Face)
{
  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxFont *each_font = (wxFont *) node->Data ();
      if (each_font &&
	  each_font->GetPointSize () == PointSize &&
	  each_font->GetStyle () == Style &&
	  each_font->GetWeight () == Weight &&
	  each_font->GetUnderlined () == underline &&
#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)
	  each_font->GetFontId () == FamilyOrFontId) /* New font system */
#else
	  each_font->GetFamily () == FamilyOrFontId &&
          (!each_font->GetFaceName() || !Face || strcmp(each_font->GetFaceName(), Face) == 0))
#endif
	return each_font;
    }
  return new wxFont (PointSize, FamilyOrFontId, Style, Weight, underline, Face);
}

/* New font system */

/* Matthew: I'm trying to remove independence from the font name directory
 * for some plaforms e.g. Windows where you could get by without it.
 * So I'm removing this function. This needs more discussion.
 */

/*
wxFont *wxFontList::
FindOrCreateFont (int PointSize, const char *Face, int Family, int Style, int Weight, Bool underline)
{
  return FindOrCreateFont(PointSize,
			  wxTheFontNameDirectory.FindOrCreateFontId(Face, Family),
			  Style,
			  Weight,
			  underline);
}
*/

#if (!USE_TYPEDEFS)
wxPoint::wxPoint (void)
{
}

wxPoint::wxPoint (float the_x, float the_y)
{
  x = the_x;
  y = the_y;
}

wxPoint::~wxPoint (void)
{
}
#endif

#if (!USE_TYPEDEFS)

wxIntPoint::wxIntPoint (void)
{
}

wxIntPoint::wxIntPoint (int the_x, int the_y)
{
  x = the_x;
  y = the_y;
}

wxIntPoint::~wxIntPoint (void)
{
}
#endif

#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)

/* MATTHEW: Font name directory */

char *font_defaults[] = {
  /* Family map */
  "FamilyDefault", "Default",
  "FamilyRoman", "Roman",
  "FamilyDecorative", "Decorative",
  "FamilyModern", "Modern",
  "FamilyTeletype", "Teletype",
  "FamilySwiss", "Swiss",
  "FamilyScript", "Script",

  "AfmMedium", "",
#ifdef wx_x
  "AfmBold", "Bo",
#else
  "AfmBold", "bo",
#endif
  "AfmLight", "",
  "AfmStraight", "",
  "AfmItalic", "${AfmSlant}",
#ifdef wx_x
  "AfmSlant", "O",
#else
  "AfmSlant", "ob",
#endif
#ifdef wx_x
  "AfmRoman", "Ro",
#else
  "AfmRoman", "ro",
#endif

#ifdef wx_x
  "AfmTimes", "Times",
  "AfmHelvetica", "Helv",
  "AfmCourier", "Cour",
#else
  "AfmTimes", "time",
  "AfmHelvetica", "helv",
  "AfmCourier", "cour",  
#endif

  "Afm___", "${AfmTimes,$[weight],$[style]}",

  "AfmTimes__", "${AfmTimes}${Afm$[weight]}${Afm$[style]}",
  "AfmTimesMediumStraight", "${AfmTimes}${AfmRoman}",
  "AfmTimesLightStraight", "${AfmTimes}${AfmRoman}",
  "AfmTimes_Italic", "${AfmTimes}$[weight]${AfmItalic}",
  "AfmTimes_Slant", "${AfmTimes}$[weight]${AfmItalic}",

  "AfmSwiss__", "${AfmHelvetica}${Afm$[weight]}${Afm$[style]}",
  "AfmModern__", "${AfmCourier}${Afm$[weight]}${Afm$[style]}",

  "AfmTeletype__", "${AfmModern,$[weight],$[style]}",

  "PostScriptMediumStraight", "",
  "PostScriptMediumItalic", "-Oblique",
  "PostScriptMediumSlant", "-Oblique",
  "PostScriptLightStraight", "",
  "PostScriptLightItalic", "-Oblique",
  "PostScriptLightSlant", "-Oblique",
  "PostScriptBoldStraight", "-Bold",
  "PostScriptBoldItalic", "-BoldOblique",
  "PostScriptBoldSlant", "-BoldOblique",

#if WX_NORMALIZED_PS_FONTS
  "PostScript___", "${PostScriptTimes,$[weight],$[style]}",
#else
  "PostScriptRoman__", "${PostScriptTimes,$[weight],$[style]}",
  "PostScript___", "LucidaSans${PostScript$[weight]$[style]}",
#endif

  "PostScriptTimesMedium", "",
  "PostScriptTimesLight", "",
  "PostScriptTimesBold", "Bold",

  "PostScriptTimes__", "Times${PostScript$[weight]$[style]}",
  "PostScriptTimesMediumStraight", "Times-Roman",
  "PostScriptTimesLightStraight", "Times-Roman",
  "PostScriptTimes_Slant", "Times-${PostScriptTimes$[weight]}Italic",
  "PostScriptTimes_Italic", "Times-${PostScriptTimes$[weight]}Italic",

  "PostScriptSwiss__", "Helvetica${PostScript$[weight]$[style]}",
  "PostScriptModern__", "Courier${PostScript$[weight]$[style]}",

  "PostScriptTeletype__", "${PostScriptModern,$[weight],$[style]}",

#if !WX_NORMALIZED_PS_FONTS
  "PostScriptScript__", "Zapf-Chancery-MediumItalic",
#endif

#ifdef wx_x
  "ScreenMedium", "medium",
  "ScreenBold", "bold",
  "ScreenLight", "light",
  "ScreenStraight", "r",
  "ScreenItalic", "i",
  "ScreenSlant", "o",

  /* "Family" -> "Base" */
  "ScreenDefaultBase", "*-*",
  "ScreenRomanBase", "*-times",
  "ScreenDecorativeBase", "*-helvetica",
  "ScreenModernBase", "*-courier",
  "ScreenTeletypeBase", "*-lucidatypewriter",
  "ScreenSwissBase", "*-lucida",
  "ScreenScriptBase", "*-zapfchancery",

  /* Use ${ScreenStdSuffix} */
  "ScreenStdSuffix", "-${Screen$[weight]}-${Screen$[style]}"
    "-normal-*-*-%d-*-*-*-*-*-*",

  "Screen___",
  "-${ScreenDefaultBase}${ScreenStdSuffix}",
  "ScreenRoman__",
  "-${ScreenRomanBase}${ScreenStdSuffix}",
  "ScreenDecorative__",
  "-${ScreenDecorativeBase}${ScreenStdSuffix}",
  "ScreenModern__",
  "-${ScreenModernBase}${ScreenStdSuffix}",
  "ScreenTeletype__",
  "-${ScreenTeletypeBase}${ScreenStdSuffix}",
  "ScreenSwiss__",
  "-${ScreenSwissBase}${ScreenStdSuffix}",
  "ScreenScript__",
  "-${ScreenScriptBase}${ScreenStdSuffix}",
#else
  /* don't specify family, weight && style... */
  "ScreenDefault__", "MS Sans Serif",
  "ScreenRoman__", "Times New Roman",
  "ScreenDecorative__", "",
  "ScreenModern__", "Courier New",
  "ScreenTeletype__", "${ScreenModern$[weight];$[style]}",
  "ScreenSwiss__", "Arial",
  "ScreenScript__", "Script",
#endif
  NULL
};

extern wxFontNameDirectory wxTheFontNameDirectory;

enum {
  wxWEIGHT_NORMAL,
  wxWEIGHT_BOLD,
  wxWEIGHT_LIGHT,
  wxNUM_WEIGHTS
  };

enum {
  wxSTYLE_NORMAL,
  wxSTYLE_ITALIC,
  wxSTYLE_SLANT,
  wxNUM_STYLES
  };

class wxSuffixMap {
 public:
  char *map[wxNUM_WEIGHTS][wxNUM_STYLES];
  wxSuffixMap();
  ~wxSuffixMap();
  void Initialize(const char *, const char *, int weight, int style);
};

class wxFontNameItem : public wxObject
{
 public:
  wxFontNameItem(int fontid, int fam, const char *resname) {
    id = fontid;
    family = fam;
    name = copystring(resname);
  }
  ~wxFontNameItem() { delete[] name; }
  int id;
  int family;
  char *name;
  wxSuffixMap screen, printing, afm;
  Bool isroman;
};

static int WCoordinate(int w)
{
  switch (w) {
  case wxBOLD:
    return wxWEIGHT_BOLD;
  case wxLIGHT:
    return wxWEIGHT_LIGHT;
  case wxNORMAL:
  default:
    return wxWEIGHT_NORMAL;
  }
}

static int SCoordinate(int s)
{
  switch (s) {
  case wxITALIC:
    return wxSTYLE_ITALIC;
  case wxSLANT:
    return wxSTYLE_SLANT;
  case wxNORMAL:
  default:
    return wxSTYLE_NORMAL;
  }
}

wxFontNameDirectory::wxFontNameDirectory(void)
{
  table = new wxHashTable(wxKEY_INTEGER, 20);
  nextFontId = -1;
}

void
wxFontNameDirectory::CleanUp()
{
  wxNode *node;

  table->BeginFind();

  while (node = table->Next())
    delete (wxFontNameItem *)node->Data();
}

wxFontNameDirectory::~wxFontNameDirectory()
{
  delete table;
}

int wxFontNameDirectory::GetNewFontId(void)
{
  return (nextFontId--);
}

#if !USE_RESOURCES
#define wxGetResource(a, b, c) 0
#endif

static void SearchResource(const char *prefix, const char **names, int count, char **v)
{
  int k, i, j;
  char resource[1024], **defaults, *internal;

  k = 1 << count;

  *v = NULL;
  internal = NULL;

  for (i = 0; i < k; i++) {
    strcpy(resource, prefix);
    for (j = 0; j < count; j++) {
      if (!(i & (1 << j)))
	strcat(resource, names[j]);
      else
	strcat(resource, "_");
    }

    if (wxGetResource(wxTheApp->wx_class, (char *)resource, v))
      return;

    if (!internal) {
      defaults = font_defaults;
      while (*defaults) {
	if (!strcmp(*defaults, resource)) {
	  internal = defaults[1];
	  break;
	}
	defaults += 2;
      }
    }
  }

  if (internal)
    *v = copystring(internal);
}

void wxFontNameDirectory::Initialize()
{
  wxTheFontNameDirectory.Initialize(wxDEFAULT, wxDEFAULT, "Default");
  wxTheFontNameDirectory.Initialize(wxDECORATIVE, wxDECORATIVE, "Decorative");
  wxTheFontNameDirectory.Initialize(wxROMAN, wxROMAN, "Roman");
  wxTheFontNameDirectory.Initialize(wxMODERN, wxMODERN, "Modern");
  wxTheFontNameDirectory.Initialize(wxTELETYPE, wxTELETYPE, "Teletype");
  wxTheFontNameDirectory.Initialize(wxSWISS, wxSWISS, "Swiss");
  wxTheFontNameDirectory.Initialize(wxSCRIPT, wxSCRIPT, "Script");
}

typedef char *a_charptr;

wxSuffixMap::wxSuffixMap()
{
  int i, j;

  for(i=0; i<wxNUM_WEIGHTS; i++)
    for(j=0; j<wxNUM_STYLES; j++)
      map[i][j] = 0;
}

wxSuffixMap::~wxSuffixMap()
{
  int i, j;

  for(i=0; i<wxNUM_WEIGHTS; i++)
    for(j=0; j<wxNUM_STYLES; j++)
      if(map[i][j])
	delete[] map[i][j];
}

void wxSuffixMap::Initialize(const char *resname, const char *devresname, int wt, int st)
{
  const char *weight, *style;
  char *v;
  int i;
  const char *names[3];

  {
    switch (wt) {
    case wxWEIGHT_NORMAL:
      weight = "Medium";
      break;
    case wxWEIGHT_LIGHT:
      weight = "Light";
      break;
    case wxWEIGHT_BOLD:
      default:
      weight = "Bold";
    }
    {
      switch (st) {
      case wxSTYLE_NORMAL:
	style = "Straight";
	break;
      case wxSTYLE_ITALIC:
	style = "Italic";
	break;
      case wxSTYLE_SLANT:
      default:
	style = "Slant";
      }

      names[0] = resname;
      names[1] = weight;
      names[2] = style;
      
      SearchResource(devresname, names, 3, &v);

      /* Expand macros in the found string: */
    found:
      int len, closer = 0, startpos = 0;

      len = (v ? strlen(v) : 0);
      for (i = 0; i < len; i++)
	if (v[i] == '$' && ((v[i+1] == '[') || (v[i+1] == '{'))) {
	  startpos = i;
	  if (v[i+1] == '[')
	    closer = ']';
	  else
	    closer = '}';
	  i++;
	} else if (v[i] == closer) {
	  int newstrlen;
	  const char *r = NULL; /* MATTHEW: [4] const */
	  char *naya, *name;
	  
	  name = v + startpos + 2;
	  v[i] = 0;

	  if (closer == '}') {
	    int i, count, len;
	    char **names;

	    for (i = 0, count = 1; name[i]; i++)
	      if (name[i] == ',')
		count++;
	    
	    len = i;

	    names = new a_charptr[count];
	    names[0] = name;
	    for (i = 0, count = 1; i < len; i++)
	      if (name[i] == ',') {
		names[count++] = name + i + 1;
		name[i] = 0;
	      }

	    SearchResource("", (const char **)names, count, (char **)&r);
	    delete[] names;

	    if (!r) {
	      for (i = 0; i < len; i++)
		if (!name[i])
		  name[i] = ',';
	      r = "";
	      printf("Bad resource name \"%s\" in font lookup\n", name);
	    }
	  } else if (!strcmp(name, "weight")) {
	    r = weight;
	  } else if (!strcmp(name, "style")) {
	    r = style;
	  } else if (!strcmp(name, "family")) {
	    r = resname;
	  } else {
	    r = "";
	    printf("Bad font macro name \"%s\"\n", name);
	  }
	  newstrlen = strlen(r);

	  naya = new char[len + newstrlen + 1];
	  memcpy(naya, v, startpos);
	  memcpy(naya + startpos, r, newstrlen);
	  memcpy(naya + startpos + newstrlen, v + i + 1, len - i);

	  delete[] v;
	  v = naya;

	  goto found;
	}

#ifdef wx_msw
      /* MATTHEW: [4] For msw, there's a meaningful default */
      if (!v)
	v = copystring(resname);
#endif

      /* We have a final value: */
      map[wt][st] = v;
    }
  }

#if 0
  for (k = 0; k < wxNUM_WEIGHTS; k++) {
    switch (k) {
    case wxWEIGHT_NORMAL:
      weight = "Medium";
      break;
    case wxWEIGHT_LIGHT:
      weight = "Light";
      break;
    case wxWEIGHT_BOLD:
      default:
      weight = "Bold";
    }
    for (j = 0; j < wxNUM_STYLES; j++) {
      switch (j) {
      case wxSTYLE_NORMAL:
	style = "Straight";
	break;
      case wxSTYLE_ITALIC:
	style = "Italic";
	break;
      case wxSTYLE_SLANT:
      default:
	style = "Slant";
      }

      names[0] = resname;
      names[1] = weight;
      names[2] = style;
      
      SearchResource(devresname, names, 3, &v);

      /* Expand macros in the found string: */
    found:
      int len, closer = 0, startpos = 0;

      len = (v ? strlen(v) : 0);
      for (i = 0; i < len; i++)
	if (v[i] == '$' && ((v[i+1] == '[') || (v[i+1] == '{'))) {
	  startpos = i;
	  if (v[i+1] == '[')
	    closer = ']';
	  else
	    closer = '}';
	  i++;
	} else if (v[i] == closer) {
	  int newstrlen;
	  char *r = NULL;
          Bool delete_r = FALSE;
	  char *naya, *name;
	  
	  name = v + startpos + 2;
	  v[i] = 0;

	  if (closer == '}') {
	    int i, count, len;
	    char **names;

	    for (i = 0, count = 1; name[i]; i++)
	      if (name[i] == ',')
		count++;
	    
	    len = i;

	    names = new a_charptr[count];
	    names[0] = name;
	    for (i = 0, count = 1; i < len; i++)
	      if (name[i] == ',') {
		names[count++] = name + i + 1;
		name[i] = 0;
	      }

	    SearchResource("", (const char **)names, count, (char **)&r);
	    delete[] names;

	    if (!r) {
	      for (i = 0; i < len; i++)
		if (!name[i])
		  name[i] = ',';
	      r = "";
	      printf("Bad resource name \"%s\" in font lookup\n", name);
	    }
            else delete_r = TRUE;
	  } else if (!strcmp(name, "weight")) {
	    r = (char *)weight;
	  } else if (!strcmp(name, "style")) {
	    r = (char *)style;
	  } else if (!strcmp(name, "family")) {
	    r = (char *)resname;
	  } else {
	    r = "";
	    printf("Bad font macro name \"%s\"\n", name);
	  }
	  newstrlen = strlen(r);

	  naya = new char[startpos + newstrlen + len - i];
	  memcpy(naya, v, startpos);
	  memcpy(naya + startpos, r, newstrlen);
	  if(delete_r)
	    delete[] r;
	  memcpy(naya + startpos + newstrlen, v + i + 1, len - i);

	  delete[] v;
	  v = naya;

	  goto found;
	}

#ifdef wx_msw
      /* For msw, there's a meaningful default */
      if (!v)
	v = copystring(resname);
#endif

      /* We have a final value: */
      map[k][j] = v;
    }
  }
#endif
}

/* New font system */
void wxFontNameDirectory::Initialize(int fontid, int family, const char *resname)
{
  wxFontNameItem *item = new wxFontNameItem(fontid, family, resname);
  char *fam, resource[256];
  
  sprintf(resource, "Family%s", resname);
  fam = NULL;
  SearchResource((const char *)resource, NULL, 0, (char **)&fam);
  if (fam) {
    if (!strcmp(fam, "Default"))
      item->family = wxDEFAULT;
    else if (!strcmp(fam, "Roman"))
      item->family = wxROMAN;
    else if (!strcmp(fam, "Decorative"))
      item->family = wxDECORATIVE;
    else if (!strcmp(fam, "Modern"))
      item->family = wxMODERN;
    else if (!strcmp(fam, "Teletype"))
      item->family = wxTELETYPE;
    else if (!strcmp(fam, "Swiss"))
      item->family = wxSWISS;
    else if (!strcmp(fam, "Script"))
      item->family = wxSCRIPT;
    delete[] fam;
  }

#if 0
  item->screen.Initialize(resname, "Screen");
  item->printing.Initialize(resname, "PostScript");
  item->afm.Initialize(resname, "Afm");
#endif

  table->Put(fontid, item);
}

int wxFontNameDirectory::FindOrCreateFontId(const char *name, int family)
{
  int id;

  if (id = GetFontId(name))
    return id;

  id = GetNewFontId();
  Initialize(id, family, name);

  return id;
}

char *wxFontNameDirectory::GetScreenName(int fontid, int weight, int style)
{
  wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid);
  
  if (!item)
    return NULL;

  weight = WCoordinate(weight);
  style = SCoordinate(style);

  /* MATTHEW: [14] Check for init */
  if (!item->screen.map[weight][style])
    item->screen.Initialize(item->name, "Screen", weight, style);

  return item->screen.map[weight][style];
}

char *wxFontNameDirectory::GetPostScriptName(int fontid, int weight, int style)
{
  wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid);

  if (!item)
    return NULL;

  weight = WCoordinate(weight);
  style = SCoordinate(style);

  /* MATTHEW: [14] Check for init */
  if (!item->printing.map[weight][style])
    item->printing.Initialize(item->name, "PostScript", weight, style);

  return item->printing.map[weight][style];
}

char *wxFontNameDirectory::GetAFMName(int fontid, int weight, int style)
{
  wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid);

  if (!item)
    return NULL;

  weight = WCoordinate(weight);
  style = SCoordinate(style);

  /* MATTHEW: [14] Check for init */
  if (!item->afm.map[weight][style])
    item->afm.Initialize(item->name, "Afm", weight, style);

  return item->afm.map[weight][style];
}

char *wxFontNameDirectory::GetFontName(int fontid)
{
  wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid);
  
  if (!item)
    return NULL;

  return item->name;
}

int wxFontNameDirectory::GetFontId(const char *name)
{
  wxNode *node;

  table->BeginFind();

  while (node = table->Next()) {
    wxFontNameItem *item = (wxFontNameItem *)node->Data();
    if (!strcmp(name, item->name))
      return item->id;
  }

  return 0;
}

int wxFontNameDirectory::GetFamily(int fontid)
{
  wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid);
  
  if (!item)
    return wxDEFAULT;

  return item->family;
}

#endif
