/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                                   X   X                                     %
%                                    X X                                      %
%                                     X                                       %
%                                    X X                                      %
%                                   X   X                                     %
%                                                                             %
%                    X11 Utility Routines for ImageMagick.                    %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                  July 1992                                  %
%                                                                             %
%                                                                             %
%  Copyright 1994 E. I. du Pont de Nemours and Company                        %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. du Pont de Nemours    %
%  and Company not be used in advertising or publicity pertaining to          %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. du Pont de Nemours and Company makes no representations %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. du Pont de Nemours and Company disclaims all warranties with regard  %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. du Pont de Nemours and Company be     %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortuous action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "image.h"
#include "utility.h"
#include "compress.h"
#include "X.h"
#include "widget.h"

/*
  State declarations.
*/
#define ControlState  0x0001
#define DefaultState  0x0000
#define ExitState  0x0002

/*
  Forward declarations.
*/
int
  Latin1Compare _Declare((char *,char *));

static void
  XMakeImageLSBFirst _Declare((XResourceInfo *,XWindowInfo *,Image *, XImage *,
    XImage *)),
  XMakeImageMSBFirst _Declare((XResourceInfo *,XWindowInfo *,Image *, XImage *,
    XImage *));

static Window
  XWindowByProperty _Declare((Display *,Window,Atom));

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T r u e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function IsTrue returns True if the boolean is "true", "on", "yes" or "1".
%
%  The format of the IsTrue routine is:
%
%      option=IsTrue(boolean)
%
%  A description of each parameter follows:
%
%    o option: either True or False depending on the boolean parameter.
%
%    o boolean: Specifies a pointer to a character array.
%
%
*/
unsigned int IsTrue(boolean)
char
  *boolean;
{
  if (boolean == (char *) NULL)
    return(False);
  if (Latin1Compare(boolean,"true") == 0)
    return(True);
  if (Latin1Compare(boolean,"on") == 0)
    return(True);
  if (Latin1Compare(boolean,"yes") == 0)
    return(True);
  if (Latin1Compare(boolean,"1") == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a t i n 1 C o m p a r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Latin1Compare compares two null terminated Latin-1 strings,
%  ignoring case differences, and returns an integer greater than, equal
%  to, or less than 0, according to whether first is lexicographically
%  greater than, equal to, or less than second.  The two strings are
%  assumed to be encoded using ISO 8859-1.
%
%  The format of the Latin1Compare routine is:
%
%      Latin1Compare(first,second)
%
%  A description of each parameter follows:
%
%    o first: A pointer to the string to convert to Latin1 string.
%
%    o second: A pointer to the string to convert to Latin1 string.
%
%
*/
int Latin1Compare(first,second)
char
  *first,
  *second;
{
  register unsigned char
   *p,
   *q;

  p=(unsigned char *) first;
  q=(unsigned char *) second;
  while ((*p != '\0') && (*q != '\0'))
  {
    register unsigned char
      c,
      d;

    c=(*p);
    d=(*q);
    if (c != d)
      {
        /*
          Try lowercasing and try again.
        */
        if ((c >= XK_A) && (c <= XK_Z))
          c+=(XK_a-XK_A);
        else
          if ((c >= XK_Agrave) && (c <= XK_Odiaeresis))
            c+=(XK_agrave-XK_Agrave);
          else
            if ((c >= XK_Ooblique) && (c <= XK_Thorn))
              c+=(XK_oslash-XK_Ooblique);
        if ((d >= XK_A) && (d <= XK_Z))
          d+=(XK_a-XK_A);
        else
          if ((d >= XK_Agrave) && (d <= XK_Odiaeresis))
            d+=(XK_agrave-XK_Agrave);
          else if ((d >= XK_Ooblique) && (d <= XK_Thorn))
            d+=(XK_oslash-XK_Ooblique);
        if (c != d)
          return(((int) c)-((int) d));
      }
    p++;
    q++;
  }
  return(((int) *p)-((int) *q));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a t i n 1 U p p e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Latin1Upper copies a null terminated string from source to
%  destination (including the null), changing all Latin-1 lowercase letters
%  to uppercase.  The string is assumed to be encoded using ISO 8859-1.
%
%  The format of the Latin1Upper routine is:
%
%      Latin1Upper(string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to upper-case Latin1.
%
%
*/
static void Latin1Upper(string)
char
  *string;
{
  unsigned char
    c;

  c=(*string);
  while (c != '\0')
  {
    if ((c >= XK_a) && (c <= XK_z))
      *string=c-(XK_a-XK_A);
    else
      if ((c >= XK_agrave) && (c <= XK_odiaeresis))
        *string=c-(XK_agrave-XK_Agrave);
      else
        if ((c >= XK_oslash) && (c <= XK_thorn))
          *string=c-(XK_oslash-XK_Ooblique);
    string++;
    c=(*string);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n n o t a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnnotateImage annotates the image with text.
%
%  The format of the XAnnotateImage routine is:
%
%    status=XAnnotateImage(display,pixel_info,annotate_info,background,image)
%
%  A description of each parameter follows:
%
%    o status: Function XAnnotateImage returns True if the image is
%      successfully annotated with text.  False is returned is there is a
%      memory shortage.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o annotate_info: Specifies a pointer to a XAnnotateInfo structure.
%
%    o background: Specifies whether the background color is included in
%      the annotation.  Must be either True or False;
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
unsigned int XAnnotateImage(display,pixel_info,annotate_info,background,image)
Display
  *display;

XPixelInfo
  *pixel_info;

XAnnotateInfo
  *annotate_info;

unsigned int
  background;

Image
  *image;
{
  GC
    annotate_context;

  Image
    *annotate_image;

  int
    x,
    y;

  Pixmap
    annotate_pixmap;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    depth,
    height,
    width;

  Window
    root_window;

  XGCValues
    context_values;

  XImage
    *annotate_ximage;

  /*
    Initialize annotated image.
  */
  if (!UncompressImage(image))
    return(False);
  /*
    Initialize annotated pixmap.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  depth=XDefaultDepth(display,XDefaultScreen(display));
  annotate_pixmap=XCreatePixmap(display,root_window,annotate_info->width,
    annotate_info->height,(int) depth);
  if (annotate_pixmap == (Pixmap) NULL)
    return(False);
  /*
    Initialize graphics info.
  */
  context_values.background=0;
  context_values.foreground=(unsigned long) (~0);
  context_values.font=annotate_info->font_info->fid;
  annotate_context=XCreateGC(display,root_window,GCBackground | GCFont |
    GCForeground,&context_values);
  if (annotate_context == (GC) NULL)
    return(False);
  /*
    Draw text to pixmap.
  */
  XDrawImageString(display,annotate_pixmap,annotate_context,0,
    (int) annotate_info->font_info->ascent,annotate_info->text,
    strlen(annotate_info->text));
  XFreeGC(display,annotate_context);
  /*
    Initialize annotated X image.
  */
  annotate_ximage=XGetImage(display,annotate_pixmap,0,0,annotate_info->width,
    annotate_info->height,AllPlanes,ZPixmap);
  if (annotate_ximage == (XImage *) NULL)
    return(False);
  XFreePixmap(display,annotate_pixmap);
  /*
    Initialize annotated image.
  */
  annotate_image=AllocateImage((ImageInfo *) NULL);
  if (annotate_image == (Image *) NULL)
    return(False);
  annotate_image->columns=annotate_info->width;
  annotate_image->rows=annotate_info->height;
  annotate_image->packets=annotate_image->columns*annotate_image->rows;
  annotate_image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if (annotate_image->pixels == (RunlengthPacket *) NULL)
    {
      DestroyImage(annotate_image);
      return(False);
    }
  /*
    Transfer annotated X image to image.
  */
  q=annotate_image->pixels;
  for (y=0; y < annotate_image->rows; y++)
    for (x=0; x < annotate_image->columns; x++)
    {
      q->index=(unsigned short) XGetPixel(annotate_ximage,x,y);
      if (q->index == 0)
        {
          /*
            Set this pixel to the background color.
          */
          q->red=ColorShift(pixel_info->background_color.red);
          q->green=ColorShift(pixel_info->background_color.green);
          q->blue=ColorShift(pixel_info->background_color.blue);
        }
      else
        {
          /*
            Set this pixel to the pen color.
          */
          q->red=ColorShift(pixel_info->annotate_color.red);
          q->green=ColorShift(pixel_info->annotate_color.green);
          q->blue=ColorShift(pixel_info->annotate_color.blue);
        }
      q->length=0;
      q++;
    }
  XDestroyImage(annotate_ximage);
  /*
    Determine annotate geometry.
  */
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  if ((width != annotate_image->columns) || (height != annotate_image->rows))
    {
      Image
        *sampled_image;

      /*
        Sample image.
      */
      sampled_image=SampleImage(annotate_image,width,height);
      if (sampled_image == (Image *) NULL)
        return(False);
      DestroyImage(annotate_image);
      annotate_image=sampled_image;
    }
  if (annotate_info->degrees != 0.0)
    {
      ColorPacket
        border_color;

      Image
        *rotated_image;

      /*
        Rotate image.
      */
      border_color.red=ColorShift(pixel_info->border_color.red);
      border_color.green=ColorShift(pixel_info->border_color.green);
      border_color.blue=ColorShift(pixel_info->border_color.blue);
      border_color.index=0;
      rotated_image=
        RotateImage(annotate_image,annotate_info->degrees,&border_color,False);
      if (rotated_image == (Image *) NULL)
        return(False);
      DestroyImage(annotate_image);
      annotate_image=rotated_image;
    }
  /*
    Paste annotated image to image.
  */
  image->class=DirectClass;
  p=annotate_image->pixels;
  q=image->pixels+y*image->columns+x;
  for (y=0; y < annotate_image->rows; y++)
  {
    for (x=0; x < annotate_image->columns; x++)
    {
      if (p->index != 0)
        {
          /*
            Set this pixel to the pen color.
          */
          *q=(*p);
          q->index=pixel_info->annotate_index;
        }
      else
        if (background)
          {
            /*
              Set this pixel to the background color.
            */
            *q=(*p);
            q->index=pixel_info->background_index;
          }
      p++;
      q++;
    }
    q+=image->columns-annotate_image->columns;
  }
  DestroyImage(annotate_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t F o n t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestFont returns the "best" font.  "Best" is defined as a font
%  specified in the X resource database or a font such that the text width
%  displayed with the font does not exceed the specified maximum width.
%
%  The format of the XBestFont routine is:
%
%      font=XBestFont(display,resource_info,text_font)
%
%  A description of each parameter follows:
%
%    o font: XBestFont returns a pointer to a XFontStruct structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o text_font:  True is font should be mono-spaced (typewriter style).
%
%
*/
XFontStruct *XBestFont(display,resource_info,text_font)
Display
  *display;

XResourceInfo
  *resource_info;

unsigned int
  text_font;
{
  static char
    *Fonts[]=
    {
      "-adobe-helvetica-medium-r-normal-*-14-*",
      "-*-helvetica-medium-r-*-*-14-*",
      "-*-lucida-medium-r-*-*-14-*",
      "8x13",
      "6x13",
      "fixed",
      "variable",
      (char *) NULL
    },
    *TextFonts[]=
    {
      "-adobe-courier-medium-r-*-*-14-*",
      "-*-fixed-medium-r-normal-*-14-*",
      "-*-fixed-medium-r-*-*-14-*",
      "8x13",
      "6x13",
      "fixed",
      (char *) NULL
    };

  char
    *font_name,
    **p;

  XFontStruct
    *font_info;

  font_info=(XFontStruct *) NULL;
  font_name=resource_info->font;
  if (text_font)
    font_name=resource_info->text_font;
  if (font_name != (char *) NULL)
    {
      /*
        Load preferred font specified in the X resource database.
      */
      font_info=XLoadQueryFont(display,font_name);
      if (font_info == (XFontStruct *) NULL)
        Warning("Unable to load font",font_name);
    }
  /*
    Load fonts from list of fonts until one is found.
  */
  p=Fonts;
  if (text_font)
    p=TextFonts;
  while (*p != (char *) NULL)
  {
    if (font_info != (XFontStruct *) NULL)
      break;
    font_info=XLoadQueryFont(display,*p);
    p++;
  }
  return(font_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t I c o n S i z e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestIconSize returns the "best" icon size.  "Best" is defined as
%  an icon size that maintains the aspect ratio of the image.  If the window
%  manager has preferred icon sizes, one of the preferred sizes is used.
%
%  The format of the XBestIconSize routine is:
%
%      XBestIconSize(display,window,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
void XBestIconSize(display,window,image)
Display
  *display;

XWindowInfo
  *window;

Image
  *image;
{
#define MaxIconSize  96

  int
    i,
    number_sizes;

  unsigned int
    height,
    icon_height,
    icon_width,
    width;

  unsigned long
    scale_factor;

  Window
    root_window;

  XIconSize
    *icon_size,
    *size_list;

  /*
    Determine if the window manager has specified preferred icon sizes.
  */
  window->width=MaxIconSize;
  window->height=MaxIconSize;
  icon_size=(XIconSize *) NULL;
  number_sizes=0;
  root_window=XRootWindow(display,window->screen);
  if (XGetIconSizes(display,root_window,&size_list,&number_sizes) != 0)
    if ((number_sizes > 0) && (size_list != (XIconSize *) NULL))
      icon_size=size_list;
  if (icon_size == (XIconSize *) NULL)
    {
      /*
        Window manager does not restrict icon size.
      */
      icon_size=XAllocIconSize();
      if (icon_size == (XIconSize *) NULL)
        {
          Warning("Unable to choose best icon size","Memory allocation failed");
          return;
        }
      icon_size->min_width=1;
      icon_size->max_width=MaxIconSize;
      icon_size->min_height=1;
      icon_size->max_height=MaxIconSize;
      icon_size->width_inc=1;
      icon_size->height_inc=1;
    }
  /*
    Determine aspect ratio of image.
  */
  width=image->columns;
  height=image->rows;
  if (window->crop_geometry)
    (void) XParseGeometry(window->crop_geometry,&i,&i,&width,&height);
  /*
    Look for an icon size that maintains the aspect ratio of image.
  */
  scale_factor=UpShift(icon_size->max_width)/width;
  if (scale_factor > (UpShift(icon_size->max_height)/height))
    scale_factor=UpShift(icon_size->max_height)/height;
  icon_width=icon_size->min_width;
  while (icon_width < icon_size->max_width)
  {
    if (icon_width >= (DownShift(width*scale_factor)))
      break;
    icon_width+=icon_size->width_inc;
  }
  icon_height=icon_size->min_height;
  while (icon_height < icon_size->max_height)
  {
    if (icon_height >= (DownShift(height*scale_factor)))
      break;
    icon_height+=icon_size->height_inc;
  }
  XFree((void *) icon_size);
  window->width=icon_width;
  window->height=icon_height;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t P i x e l                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestPixel returns a pixel from an array of pixels that is closest
%  to the requested color.  If the color array is NULL, the colors are obtained
%  from the X server.
%
%  The format of the XBestPixel routine is:
%
%      pixel=XBestPixel(display,colormap,colors,number_colors,color)
%
%  A description of each parameter follows:
%
%    o pixel: XBestPixel returns the pixel value closest to the requested
%      color.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o colormap: Specifies the ID of the X server colormap.
%
%    o colors: Specifies an array of XColor structures.
%
%    o number_colors: Specifies the number of XColor structures in the
%      color definition array.
%
%    o color: Specifies the desired RGB value to find in the colors array.
%
%
*/
void XBestPixel(display,colormap,colors,number_colors,color)
Display
  *display;

Colormap
  colormap;

XColor
  *colors;

unsigned int
  number_colors;

XColor
  *color;
{
  int
    j,
    query_server,
    status;

  register int
    blue_distance,
    green_distance,
    i,
    red_distance;

  unsigned long
    distance,
    min_distance;

  /*
    Find closest representation for the requested RGB color.
  */
  status=XAllocColor(display,colormap,color);
  if (status != 0)
    return;
  query_server=colors == (XColor *) NULL;
  if (query_server)
    {
      /*
        Read X server colormap.
      */
      colors=(XColor *) malloc(number_colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("Unable to read X server colormap","Memory allocation failed");
      for (i=0; i < number_colors; i++)
        colors[i].pixel=(unsigned long) i;
      if (number_colors > 256)
        number_colors=256;
      XQueryColors(display,colormap,colors,number_colors);
    }
  min_distance=(unsigned long) (~0);
  color->pixel=0;
  j=0;
  for (i=0; i < number_colors; i++)
  {
    red_distance=(int)
      (ColorShift(colors[i].red))-(int) (ColorShift(color->red));
    green_distance=(int)
      (ColorShift(colors[i].green))-(int) (ColorShift(color->green));
    blue_distance=(int)
      (ColorShift(colors[i].blue))-(int) (ColorShift(color->blue));
    distance=red_distance*red_distance+green_distance*green_distance+
      blue_distance*blue_distance;
    if (distance < min_distance)
      {
        min_distance=distance;
        color->pixel=colors[i].pixel;
        j=i;
      }
  }
  (void) XAllocColor(display,colormap,&colors[j]);
  if (query_server)
    (void) free((char *) colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t V i s u a l I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestVisualInfo returns visual information for a visual that is
%  the "best" the server supports.  "Best" is defined as:
%
%    1. Restrict the visual list to those supported by the default screen.
%
%    2. If a visual type is specified, restrict the visual list to those of
%       that type.
%
%    3. If a map type is specified, choose the visual that matches the id
%       specified by the Standard Colormap.
%
%    4  From the list of visuals, choose one that can display the most
%       simultaneous colors.  If more than one visual can display the same
%       number of simultaneous colors, one is choosen based on a rank.
%
%  The format of the XBestVisualInfo routine is:
%
%      visual_info=XBestVisualInfo(display,map_info,resource_info)
%
%  A description of each parameter follows:
%
%    o visual_info: XBestVisualInfo returns a pointer to a X11 XVisualInfo
%      structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
XVisualInfo *XBestVisualInfo(display,map_info,resource_info)
Display
  *display;

XStandardColormap
  *map_info;

XResourceInfo
  *resource_info;
{
#define MaxStandardColormaps  7
#define XVisualColormapSize(visual_info) \
  ((visual_info->class == TrueColor) || (visual_info->class == DirectColor) ? \
    visual_info->red_mask | visual_info->green_mask | visual_info->blue_mask : \
    visual_info->colormap_size)

  char
    *map_type,
    *visual_type;

  int
    number_visuals;

  register int
    i;

  unsigned int
    visual_mask;

  XVisualInfo
    *visual_info,
    *visual_list,
    visual_template;

  /*
    Restrict visual search by screen number.
  */
  map_type=resource_info->map_type;
  visual_type=resource_info->visual_type;
  visual_mask=VisualScreenMask;
  visual_template.screen=XDefaultScreen(display);
  if (visual_type != (char *) NULL)
    {
      /*
        Restrict visual search by class or visual id.
      */
      if (Latin1Compare("staticgray",visual_type) == 0)
        {
          visual_mask|=VisualClassMask;
          visual_template.class=StaticGray;
        }
      else
        if (Latin1Compare("grayscale",visual_type) == 0)
          {
            visual_mask|=VisualClassMask;
            visual_template.class=GrayScale;
          }
        else
          if (Latin1Compare("staticcolor",visual_type) == 0)
            {
              visual_mask|=VisualClassMask;
              visual_template.class=StaticColor;
            }
          else
            if (Latin1Compare("pseudocolor",visual_type) == 0)
              {
                visual_mask|=VisualClassMask;
                visual_template.class=PseudoColor;
              }
            else
              if (Latin1Compare("truecolor",visual_type) == 0)
                {
                  visual_mask|=VisualClassMask;
                  visual_template.class=TrueColor;
                }
              else
                if (Latin1Compare("directcolor",visual_type) == 0)
                  {
                    visual_mask|=VisualClassMask;
                    visual_template.class=DirectColor;
                  }
                else
                  if (Latin1Compare("default",visual_type) == 0)
                    {
                      visual_mask|=VisualIDMask;
                      visual_template.visualid=XVisualIDFromVisual(
                        XDefaultVisual(display,XDefaultScreen(display)));
                    }
                  else
                    if (isdigit(*visual_type))
                      {
                        visual_mask|=VisualIDMask;
                        visual_template.visualid=
                          strtol(visual_type,(char **) NULL,0);
                      }
                    else
                      Warning("Invalid visual specifier",visual_type);
    }
  /*
    Get all visuals that meet our criteria so far.
  */
  number_visuals=0;
  visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  visual_mask=VisualScreenMask | VisualIDMask;
  if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
    {
      /*
        Failed to get visual;  try using the default visual.
      */
      Warning("Unable to get visual",visual_type);
      visual_template.visualid=
        XVisualIDFromVisual(XDefaultVisual(display,XDefaultScreen(display)));
      visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
        &number_visuals);
      if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
        return((XVisualInfo *) NULL);
      Warning("Using default visual",XVisualClassName(visual_list->class));
    }
  resource_info->color_recovery=False;
  if (map_type != (char *) NULL)
    {
      Atom
        map_property;

      char
        map_name[MaxTextLength];

      int
        j,
        number_maps,
        status;

      Window
        root_window;

      XStandardColormap
        *map_list;

      /*
        Choose a visual associated with a standard colormap.
      */
      root_window=XRootWindow(display,XDefaultScreen(display));
      status=0;
      if (strcmp(map_type,"list") != 0)
        {
          /*
            User specified Standard Colormap.
          */
          (void) sprintf((char *) map_name,"RGB_%s_MAP",map_type);
          Latin1Upper(map_name);
          map_property=XInternAtom(display,(char *) map_name,True);
          if (map_property == (Atom) NULL)
            Error("Unable to get Standard Colormap",map_type);
          status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
            map_property);
        }
      else
        {
          static char
            *colormap[]=
            {
              "_HP_RGB_SMOOTH_MAP_LIST",
              "RGB_BEST_MAP",
              "RGB_DEFAULT_MAP",
              "RGB_GRAY_MAP",
              "RGB_RED_MAP",
              "RGB_GREEN_MAP",
              "RGB_BLUE_MAP",
            };

          /*
            Choose a standard colormap from a list.
          */
          for (i=0; i < MaxStandardColormaps; i++)
          {
            map_property=XInternAtom(display,colormap[i],True);
            if (map_property == (Atom) NULL)
              continue;
            status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
              map_property);
            if (status != 0)
              break;
          }
          resource_info->color_recovery=(i == 0);  /* _HP_RGB_SMOOTH_MAP_LIST */
        }
      if (status == 0)
        Error("Unable to get Standard Colormap",map_type);
      /*
        Search all Standard Colormaps and visuals for ids that match.
      */
      *map_info=map_list[0];
#ifndef PRE_R4_ICCCM
      visual_template.visualid=XVisualIDFromVisual(visual_list[0].visual);
      for (i=0; i < number_maps; i++)
        for (j=0; j < number_visuals; j++)
          if (map_list[i].visualid ==
              XVisualIDFromVisual(visual_list[j].visual))
            {
              *map_info=map_list[i];
              visual_template.visualid=
                XVisualIDFromVisual(visual_list[j].visual);
              break;
            }
      if (map_info->visualid != visual_template.visualid)
        Error("Unable to match visual to Standard Colormap",map_type);
#endif
      if (map_info->colormap == (Colormap) NULL)
        Error("Standard Colormap is not initialized",map_type);
      XFree((void *) map_list);
    }
  else
    {
      static unsigned int
        rank[]=
          {
            StaticGray,
            GrayScale,
            StaticColor,
            DirectColor,
            TrueColor,
            PseudoColor
          };

      XVisualInfo
        *p;

      /*
        Pick one visual that displays the most simultaneous colors.
      */
      visual_info=visual_list;
      p=visual_list;
      for (i=1; i < number_visuals; i++)
      {
        p++;
        if (XVisualColormapSize(p) > XVisualColormapSize(visual_info))
          visual_info=p;
        else
          if (XVisualColormapSize(p) == XVisualColormapSize(visual_info))
            if (rank[p->class] > rank[visual_info->class])
              visual_info=p;
      }
      visual_template.visualid=XVisualIDFromVisual(visual_info->visual);
    }
  XFree((void *) visual_list);
  /*
    Retrieve only one visual by its screen & id number.
  */
  visual_info=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  if ((number_visuals == 0) || (visual_info == (XVisualInfo *) NULL))
    return((XVisualInfo *) NULL);
  return(visual_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C h e c k R e f r e s h W i n d o w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XCheckRefreshWindow checks the X server for exposure events for
%  a particular window and updates the area associated withe exposure event.
%
%  The format of the XCheckRefreshWindow routine is:
%
%      XCheckRefreshWindow(display,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
void XCheckRefreshWindow(display,window)
Display
  *display;

XWindowInfo
  *window;
{
  XEvent
    event;

  XDelay(display,SuspendTime);
  while (XCheckTypedWindowEvent(display,window->id,Expose,&event))
    XRefreshWindow(display,window,&event);
  XDelay(display,SuspendTime << 1);
  while (XCheckTypedWindowEvent(display,window->id,Expose,&event))
    XRefreshWindow(display,window,&event);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i e n t M e s s a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XClientMessage sends a message to a window with XSendEvent.  The
%  message is initialized with a particular protocol type and atom.
%
%  The format of the XClientMessage function is:
%
%      XClientMessage(display,window,protocol,message,timestamp)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o protocol: Specifies an atom value.
%
%    o message: Specifies an atom value which is the message to send.
%
%    o timestamp: Specifies a value of type Time.
%
%
*/
void XClientMessage(display,window,protocol,message,timestamp)
Display
  *display;

Window
   window;

Atom
  protocol,
  message;

Time
  timestamp;
{
  XClientMessageEvent
    client_event;

  client_event.type=ClientMessage;
  client_event.window=window;
  client_event.message_type=protocol;
  client_event.format=32;
  client_event.data.l[0]=message;
  client_event.data.l[1]=timestamp;
  XSendEvent(display,window,False,0L,(XEvent *) &client_event);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i e n t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XClientWindow finds a window, at or below the specified window,
%  which has a WM_STATE property.  If such a window is found, it is returned,
%  otherwise the argument window is returned.
%
%  The format of the XClientWindow function is:
%
%      client_window=XClientWindow(display,target_window)
%
%  A description of each parameter follows:
%
%    o client_window: XClientWindow returns a window, at or below the specified
%      window, which has a WM_STATE property otherwise the argument
%      target_window is returned.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o target_window: Specifies the window to find a WM_STATE property.
%
%
*/
Window XClientWindow(display,target_window)
Display
  *display;

Window
  target_window;
{
  Atom
    state,
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned long
    after,
    number_items;

  Window
    client_window;

  state=XInternAtom(display,"WM_STATE",True);
  if (state == (Atom) NULL)
    return(target_window);
  type=(Atom) NULL;
  status=XGetWindowProperty(display,target_window,state,0L,0L,False,
    (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
  if ((status == Success) && (type != (Atom) NULL))
    return(target_window);
  client_window=XWindowByProperty(display,target_window,state);
  if (client_window == (Window) NULL)
    return(target_window);
  return(client_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D e l a y                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDelay suspends program execution for the number of milliseconds
%  specified.
%
%  The format of the Delay routine is:
%
%      XDelay(display,milliseconds)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o milliseconds: Specifies the number of milliseconds to delay before
%      returning.
%
%
*/
void XDelay(display,milliseconds)
Display
  *display;

unsigned long
  milliseconds;
{
#ifndef vms
#ifdef SYSV
#include <sys/poll.h>

  struct pollfd
    descriptor;

  (void) XFlush(display);
  if (milliseconds == 0)
    return;
  (void) poll((struct pollfd *) NULL,0,(int) milliseconds);
#else
  struct timeval
    timer;

  (void) XFlush(display);
  if (milliseconds == 0)
    return;
  timer.tv_sec=milliseconds/1000;
  timer.tv_usec=(milliseconds % 1000)*1000;
  (void) select(0,(XFD_SET *) NULL,(XFD_SET *) NULL,(XFD_SET *) NULL,&timer);
#endif
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D e s t r o y W i n d o w C o l o r s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDestroyWindowColors frees X11 color resources previously saved on
%  a window by XRetainWindowColors or programs like xsetroot.
%
%  The format of the XDestroyWindowColors routine is:
%
%      XDestroyWindowColors(display,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%
*/
void XDestroyWindowColors(display,window)
Display
  *display;

Window
  window;
{
  Atom
    property,
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned long
    after,
    length;

  /*
    If there are previous resources on the root window, destroy them.
  */
  property=XInternAtom(display,"_XSETROOT_ID",False);
  if (property == (Atom) NULL)
    {
      Warning("Unable to create X property","_XSETROOT_ID");
      return;
    }
  status=XGetWindowProperty(display,window,property,0L,1L,True,
    (Atom) AnyPropertyType,&type,&format,&length,&after,&data);
  if (status != Success)
    return;
  if ((type == XA_PIXMAP) && (format == 32) && (length == 1) && (after == 0))
    {
      XKillClient(display,(XID) (*((Pixmap *) data)));
      XDeleteProperty(display,window,property);
    }
  if (type != None)
    XFree((void *) data);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y I m a g e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayImageInfo displays information about an X image.
%
%  The format of the XDisplayImageInfo routine is:
%
%      XDisplayImageInfo(display,resource_info,windows,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
void XDisplayImageInfo(display,resource_info,windows,image)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *windows;

Image
  *image;
{
  char
    *text,
    **textlist,
    title[MaxTextLength];

  int
    length;

  Image
    *p;

  unsigned int
    count;

  /*
    Display information about the image in the Text View widget.
  */
  length=11*MaxTextLength;
  if (image->directory != (char *) NULL)
    length+=strlen(image->directory);
  if (image->comments != (char *) NULL)
    length+=strlen(image->comments);
  text=(char *) malloc(length*sizeof(char));
  if (text == (char *) NULL)
    {
      XNoticeWidget(display,windows,"Unable to display image info:",
        "Memory allocation failed");
      return;
    }
  (void) sprintf(title," Image Info: %s",image->filename);
  (void) sprintf(text,"X\n  visual: %s\n",
    XVisualClassName(windows->image.class));
  (void) sprintf(text,"%s  depth: %d\n",text,windows->image.depth);
  (void) sprintf(text,"%s  geometry: %dx%d\n",text,
    windows->image.ximage->width,windows->image.ximage->height);
  if (windows->image.crop_geometry != (char *) NULL)
    (void) sprintf(text,"%s  crop geometry: %s\n",text,
      windows->image.crop_geometry);
  if (windows->image.pixmap == (Pixmap) NULL)
    (void) strcat(text,"  type: X Image\n");
  else
    (void) strcat(text,"  type: Pixmap\n");
  if (windows->image.shared_memory)
    (void) strcat(text,"  shared memory: True\n");
  else
    (void) strcat(text,"  shared memory: False\n");
  if (resource_info->colormap== SharedColormap)
    (void) strcat(text,"  colormap type: Shared\n");
  else
    (void) strcat(text,"  colormap type: Private\n");
  (void) strcat(text,"\n");
  (void) sprintf(text,"%sImage\n  file: %s\n",text,image->filename);
  (void) sprintf(text,"%s  format: %s\n",text,image->magick);
  if (image->filesize != 0)
    (void) sprintf(text,"%s  bytes: %ld\n",text,image->filesize);
  if (image->class == DirectClass)
    (void) strcat(text,"  class: DirectClass\n");
  else
    (void) strcat(text,"  class: PseudoClass\n");
  if (image->matte)
    (void) strcat(text,"  matte: True\n");
  else
    (void) strcat(text,"  matte: False\n");
  (void) sprintf(text,"%s  geometry: %ux%u\n",text,
    image->columns,image->rows);
  if (image->class == PseudoClass)
    if (image->colors != 0)
      (void) sprintf(text,"%s  colors: %u\n",text,image->colors);
  if (image->signature != (char *) NULL)
    (void) sprintf(text,"%s  signature: %s\n",text,image->signature);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) sprintf(text,"%s  scene: %u of %u\n",text,image->scene,count);
  else
    if (image->scene != 0)
      (void) sprintf(text,"%s  scene: %u\n",text,image->scene);
  if (image->montage != (char *) NULL)
    (void) sprintf(text,"%s  montage: %s\n",text,image->montage);
  if (image->directory != (char *) NULL)
    (void) sprintf(text,"%s  directory:\n\n%s\n",text,image->directory);
  if (image->label != (char *) NULL)
    (void) sprintf(text,"%s  label: %s\n",text,image->label);
  if (image->comments != (char *) NULL)
    (void) sprintf(text,"%s  comments:\n\n%s",text,image->comments);
  textlist=StringToList(text);
  if (textlist != (char **) NULL)
    {
      register int
        i;

      XTextViewWidget(display,resource_info,windows,True,title,textlist);
      for (i=0; textlist[i] != (char *) NULL; i++)
        (void) free((char *) textlist[i]);
      (void) free((char *) textlist);
    }
  (void) free((char *) text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y I n f o S t r i n g                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayInfoString first clears the info window and draws a
%  text string justifed left in the x-direction and centered within the
%  y-direction.
%
%  The format of the XDisplayInfoString function is:
%
%      XDisplayInfoString(display,window_info,text)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window_info: Specifies a pointer to a XWindowInfo structure.
%
%    o text: Specifies a pointer to a text string.
%
%
*/
void XDisplayInfoString(display,window_info,text)
Display
  *display;

XWindowInfo
  *window_info;

char
  *text;
{
  int
    length,
    x,
    y;

  unsigned int
    width;

  XFontStruct
    *font_info;

  /*
    Insure text will fit within the window.
  */
  font_info=window_info->font_info;
  width=window_info->width-font_info->max_bounds.width;
  for (length=strlen(text); length > 0; length--)
    if (XTextWidth(font_info,text,length) <= width)
      break;
  /*
    Clear the info window then draw the text.
  */
  XClearWindow(display,window_info->id);
  x=font_info->max_bounds.width >> 1;
  y=font_info->ascent+(window_info->height >> 3);
  XDrawString(display,window_info->id,window_info->annotate_context,x,y,
    text,length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     X D i t h e r I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDitherImage dithers the reference image as required by the HP
%  Color Recovery algorithm.  The color values are quantized to 3 bits of red
%  and green, and 2 bits of blue (3/3/2) and can be used as indices into a
%  8-bit X standard colormap.
%
%  The format of the XDitherImage routine is:
%
%      XDitherImage(image,ximage)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
static void XDitherImage(image,ximage)
Image
  *image;

XImage
  *ximage;
{
  static short int
    dither_red[2][16]=
    {
      {-16,  4, -1, 11,-14,  6, -3,  9,-15,  5, -2, 10,-13,  7, -4,  8},
      { 15, -5,  0,-12, 13, -7,  2,-10, 14, -6,  1,-11, 12, -8,  3, -9}
    },
    dither_green[2][16]=
    {
      { 11,-15,  7, -3,  8,-14,  4, -2, 10,-16,  6, -4,  9,-13,  5, -1},
      {-12, 14, -8,  2, -9, 13, -5,  1,-11, 15, -7,  3,-10, 12, -6,  0}
    },
    dither_blue[2][16]=
    {
      { -3,  9,-13,  7, -1, 11,-15,  5, -4,  8,-14,  6, -2, 10,-16,  4},
      {  2,-10, 12, -8,  0,-12, 14, -6,  3, -9, 13, -7,  1,-11, 15, -5}
    };

  ColorPacket
    color;

  int
    value,
    y;

  register char
    *q;

  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  unsigned char
    *blue_map[2][16],
    *green_map[2][16],
    *red_map[2][16];

  unsigned int
    scanline_pad;

  register unsigned long
    pixel;

  if (!UncompressImage(image))
    return;
  /*
    Allocate and initialize dither maps.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
    {
      red_map[i][j]=(unsigned char *) malloc(256*sizeof(unsigned char));
      green_map[i][j]=(unsigned char *) malloc(256*sizeof(unsigned char));
      blue_map[i][j]=(unsigned char *) malloc(256*sizeof(unsigned char));
      if ((red_map[i][j] == (unsigned char *) NULL) ||
          (green_map[i][j] == (unsigned char *) NULL) ||
          (blue_map[i][j] == (unsigned char *) NULL))
        {
          Warning("Unable to dither image","Memory allocation failed");
          return;
        }
    }
  /*
    Initialize dither tables.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
      for (x=0; x < 256; x++)
      {
        value=x-16;
        if (x < 48)
          value=x/2+8;
        value+=dither_red[i][j];
        if (value < 0)
          value=0;
        else
          if (value > MaxRGB)
            value=MaxRGB;
        red_map[i][j][x]=value;
        value=x-16;
        if (x < 48)
          value=x/2+8;
        value+=dither_green[i][j];
        if (value < 0)
          value=0;
        else
          if (value > MaxRGB)
            value=MaxRGB;
        green_map[i][j][x]=value;
        value=x-32;
        if (x < 112)
          value=x/2+24;
        value+=(dither_blue[i][j] << 1);
        if (value < 0)
          value=0;
        else
          if (value > MaxRGB)
            value=MaxRGB;
        blue_map[i][j][x]=value;
      }
  /*
    Dither image.
  */
  scanline_pad=ximage->bytes_per_line-
    ((ximage->width*ximage->bits_per_pixel) >> 3);
  i=0;
  j=0;
  p=image->pixels;
  q=ximage->data;
  for (y=0; y < image->rows; y++)
  {
    for (x=0; x < image->columns; x++)
    {
      color.red=red_map[i][j][p->red];
      color.green=green_map[i][j][p->green];
      color.blue=blue_map[i][j][p->blue];
      pixel=((color.red & 0xe0) |
        ((unsigned char) (color.green & 0xe0) >> 3) |
        ((unsigned char) (color.blue & 0xc0) >> 6));
      *q++=(unsigned char) pixel;
      p++;
      j++;
      if (j == 16)
        j=0;
    }
    q+=scanline_pad;
    i++;
    if (i == 2)
      i=0;
  }
  /*
    Free allocated memory.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
    {
      (void) free((char *) green_map[i][j]);
      (void) free((char *) blue_map[i][j]);
      (void) free((char *) red_map[i][j]);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X E r r o r                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XError ignores BadWindow errors for XQueryTree and
%  XGetWindowAttributes, and ignores BadDrawable errors for XGetGeometry, and
%  ignores BadValue errors for XQueryColor.  It returns False in those cases.
%  Otherwise it returns True.
%
%  The format of the XError function is:
%
%      XError(display,error)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o error: Specifies the error event.
%
%
*/
int XError(display,error)
Display
  *display;

XErrorEvent
  *error;
{
  switch (error->request_code)
  {
    case X_GetGeometry:
    {
      if (error->error_code == BadDrawable)
        return(False);
      break;
    }
    case X_GetWindowAttributes:
    case X_QueryTree:
    {
      if (error->error_code == BadWindow)
        return(False);
      break;
    }
    case X_QueryColors:
    {
      if (error->error_code == BadValue)
        return(False);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X F r e e R e s o u r c e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XFreeResources frees X11 resources.
%
%  The format of the XFreeResources routine is:
%
%      XFreeResources(display,visual_info,map_info,pixel_info,font_info,
%        resource_info,window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o font_info: Specifies a pointer to a XFontStruct structure.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window_info: Specifies a pointer to a X11 XWindowInfo structure.
%
%
*/
void XFreeResources(display,visual_info,map_info,pixel_info,font_info,
  resource_info,window_info)
Display
  *display;

XVisualInfo
  *visual_info;

XStandardColormap
  *map_info;

XPixelInfo
  *pixel_info;

XFontStruct
  *font_info;

XResourceInfo
  *resource_info;

XWindowInfo
  *window_info;
{
  if (window_info != (XWindowInfo *) NULL)
    {
      /*
        Free X image.
      */
      if (window_info->ximage != (XImage *) NULL)
        XDestroyImage(window_info->ximage);
      if (window_info->id != (Window) NULL)
        {
          /*
            Free destroy window and free cursors.
          */
          if (window_info->id != XRootWindow(display,visual_info->screen))
            XDestroyWindow(display,window_info->id);
          if (window_info->annotate_context != (GC) NULL)
            XFreeGC(display,window_info->annotate_context);
          if (window_info->highlight_context != (GC) NULL)
            XFreeGC(display,window_info->highlight_context);
          if (window_info->widget_context != (GC) NULL)
            XFreeGC(display,window_info->widget_context);
          XFreeCursor(display,window_info->cursor);
          XFreeCursor(display,window_info->busy_cursor);
        }
    }
  /*
    Free font.
  */
  if (font_info != (XFontStruct *) NULL)
    XFreeFont(display,font_info);
  if (map_info != (XStandardColormap *) NULL)
    {
      /*
        Free X Standard Colormap.
      */
      if (resource_info->map_type == (char *) NULL)
        XFreeStandardColormap(display,visual_info,map_info,pixel_info);
      XFree((void *) map_info);
    }
  /*
    Free X visual info.
  */
  if (visual_info != (XVisualInfo *) NULL)
    XFree((void *) visual_info);
  XCloseDisplay(display);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X F r e e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XFreeStandardColormap frees an X11 colormap.
%
%  The format of the XFreeStandardColormap routine is:
%
%      XFreeStandardColormap(display,visual_info,map_info,pixel_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%
*/
void XFreeStandardColormap(display,visual_info,map_info,pixel_info)
Display
  *display;

XVisualInfo
  *visual_info;

XStandardColormap
  *map_info;

XPixelInfo
  *pixel_info;
{
  /*
    Free colormap.
  */
  XFlush(display);
  if (map_info->colormap != (Colormap) NULL)
    if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
      XFreeColormap(display,map_info->colormap);
    else
      if (pixel_info != (XPixelInfo *) NULL)
        if ((visual_info->class != TrueColor) &&
            (visual_info->class != DirectColor))
          XFreeColors(display,map_info->colormap,pixel_info->pixels,
            (int) pixel_info->colors,0);
  map_info->colormap=(Colormap) NULL;
  if (pixel_info != (XPixelInfo *) NULL)
    {
      if (pixel_info->pixels != (unsigned long *) NULL)
        (void) free((char *) pixel_info->pixels);
      pixel_info->pixels=(unsigned long *) NULL;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t A n n o t a t e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetAnnotateInfo initializes the AnnotateInfo structure.
%
%  The format of the GetAnnotateInfo routine is:
%
%      XGetAnnotateInfo(image_info)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to a XAnnotateInfo structure.
%
%
*/
void XGetAnnotateInfo(annotate_info)
XAnnotateInfo
  *annotate_info;
{
  /*
    Initialize annotate structure.
  */
  annotate_info->x=0;
  annotate_info->y=0;
  annotate_info->width=0;
  annotate_info->height=0;
  annotate_info->degrees=0.0;
  annotate_info->font_info=(XFontStruct *) NULL;
  annotate_info->text=(char *) NULL;
  *annotate_info->geometry='\0';
  annotate_info->previous=(XAnnotateInfo *) NULL;
  annotate_info->next=(XAnnotateInfo *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t M a p I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetMapInfo initializes the XStandardColormap structure.
%
%  The format of the XStandardColormap routine is:
%
%      XGetMapInfo(visual_info,colormap,map_info)
%
%  A description of each parameter follows:
%
%    o colormap: Specifies the ID of the X server colormap.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: Specifies a pointer to a X11 XStandardColormap structure.
%
%
*/
void XGetMapInfo(visual_info,colormap,map_info)
XVisualInfo
  *visual_info;

Colormap
  colormap;

XStandardColormap
  *map_info;
{
  /*
    Initialize map info.
  */
  map_info->colormap=colormap;
  map_info->red_max=visual_info->red_mask;
  map_info->red_mult=map_info->red_max != 0 ? 1 : 0;
  if (map_info->red_max != 0)
    while ((map_info->red_max & 0x01) == 0)
    {
      map_info->red_max>>=1;
      map_info->red_mult<<=1;
    }
  map_info->green_max=visual_info->green_mask;
  map_info->green_mult=map_info->green_max != 0 ? 1 : 0;
  if (map_info->green_max != 0)
    while ((map_info->green_max & 0x01) == 0)
    {
      map_info->green_max>>=1;
      map_info->green_mult<<=1;
    }
  map_info->blue_max=visual_info->blue_mask;
  map_info->blue_mult=map_info->blue_max != 0 ? 1 : 0;
  if (map_info->blue_max != 0)
    while ((map_info->blue_max & 0x01) == 0)
    {
      map_info->blue_max>>=1;
      map_info->blue_mult<<=1;
    }
  map_info->base_pixel=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t M o n t a g e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetMontageInfo initializes the MontageInfo structure.
%
%  The format of the XGetMontageInfo routine is:
%
%      XGetMontageInfo(montage_info)
%
%  A description of each parameter follows:
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%
*/
void XGetMontageInfo(montage_info)
XMontageInfo
  *montage_info;
{
  montage_info->number_tiles=0;
  montage_info->frame=True;
  montage_info->shadow=True;
  montage_info->compose=ReplaceCompositeOp;
  montage_info->tile="5x4";
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t P i x e l I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetPixelInfo initializes the PixelInfo structure.
%
%  The format of the XGetPixelInfo routine is:
%
%      XGetPixelInfo(display,visual_info,map_info,resource_info,image,
%        pixel_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%
*/
void XGetPixelInfo(display,visual_info,map_info,resource_info,image,pixel_info)
Display
  *display;

XVisualInfo
  *visual_info;

XStandardColormap
  *map_info;

XResourceInfo
  *resource_info;

Image
  *image;

XPixelInfo
  *pixel_info;
{
  static char
    *PenColors[MaxNumberPens]=
    {
      Pen1Color,
      Pen2Color,
      Pen3Color,
      Pen4Color,
      Pen5Color,
      Pen6Color,
      Pen7Color,
      Pen8Color,
      Pen9Color,
      Pen0Color,
    };

  Colormap
    colormap;

  int
    blue,
    green,
    red,
    status;

  register int
    i;

  unsigned int
    packets;

  /*
    Initialize pixel info.
  */
  pixel_info->colors=0;
  if (image != (Image *) NULL)
    if (image->class == PseudoClass)
      pixel_info->colors=image->colors;
  packets=Max(pixel_info->colors,visual_info->colormap_size)+MaxNumberPens;
  pixel_info->pixels=(unsigned long *) malloc(packets*sizeof(unsigned long));
  if (pixel_info->pixels == (unsigned long *) NULL)
    Error("Unable to get pixel info","Memory allocation failed");
  /*
    Set foreground color.
  */
  colormap=map_info->colormap;
  (void) XParseColor(display,colormap,ForegroundColor,
    &pixel_info->foreground_color);
  status=XParseColor(display,colormap,resource_info->foreground_color,
    &pixel_info->foreground_color);
  if (status == 0)
    Warning("Color is not known to X server",resource_info->foreground_color);
  pixel_info->foreground_color.pixel=
    XStandardPixel(map_info,pixel_info->foreground_color,16);
  pixel_info->foreground_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set background color.
  */
  (void) XParseColor(display,colormap,BackgroundColor,
    &pixel_info->background_color);
  status=XParseColor(display,colormap,resource_info->background_color,
    &pixel_info->background_color);
  if (status == 0)
    Warning("Color is not known to X server",resource_info->background_color);
  pixel_info->background_color.pixel=
    XStandardPixel(map_info,pixel_info->background_color,16);
  pixel_info->background_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set border color.
  */
  (void) XParseColor(display,colormap,BorderColor,&pixel_info->border_color);
  status=XParseColor(display,colormap,resource_info->border_color,
    &pixel_info->border_color);
  if (status == 0)
    Warning("Color is not known to X server",resource_info->border_color);
  pixel_info->border_color.pixel=
    XStandardPixel(map_info,pixel_info->border_color,16);
  pixel_info->border_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set matte color.
  */
  pixel_info->matte_color=pixel_info->background_color;
  if (resource_info->matte_color != (char *) NULL)
    {
      /*
        Matte color is specified as a X resource or command line argument.
      */
      status=XParseColor(display,colormap,resource_info->matte_color,
        &pixel_info->matte_color);
      if (status == 0)
        Warning("Color is not known to X server",resource_info->matte_color);
      pixel_info->matte_color.pixel=
        XStandardPixel(map_info,pixel_info->matte_color,16);
      pixel_info->matte_color.flags=DoRed | DoGreen | DoBlue;
    }
  /*
    Set highlight color.
  */
  red=ColorShift(309*pixel_info->matte_color.red);
  pixel_info->highlight_color.red=(red > 65535 ? 65535 : red);
  green=ColorShift(309*pixel_info->matte_color.green);
  pixel_info->highlight_color.green=(green > 65535 ? 65535 : green);
  blue=ColorShift(309*pixel_info->matte_color.blue);
  pixel_info->highlight_color.blue=(blue > 65535 ? 65535 : blue);
  pixel_info->highlight_color.pixel=
    XStandardPixel(map_info,pixel_info->highlight_color,16);
  pixel_info->highlight_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set shadow color.
  */
  pixel_info->shadow_color.red=ColorShift(148*pixel_info->matte_color.red);
  pixel_info->shadow_color.green=ColorShift(148*pixel_info->matte_color.green);
  pixel_info->shadow_color.blue=ColorShift(148*pixel_info->matte_color.blue);
  pixel_info->shadow_color.pixel=
    XStandardPixel(map_info,pixel_info->shadow_color,16);
  pixel_info->shadow_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set depth color.
  */
  pixel_info->depth_color.red=ColorShift(209*pixel_info->matte_color.red);
  pixel_info->depth_color.green=ColorShift(209*pixel_info->matte_color.green);
  pixel_info->depth_color.blue=ColorShift(209*pixel_info->matte_color.blue);
  pixel_info->depth_color.pixel=
    XStandardPixel(map_info,pixel_info->depth_color,16);
  pixel_info->depth_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set trough color.
  */
  pixel_info->trough_color.red=ColorShift(134*pixel_info->matte_color.red);
  pixel_info->trough_color.green=ColorShift(134*pixel_info->matte_color.green);
  pixel_info->trough_color.blue=ColorShift(134*pixel_info->matte_color.blue);
  pixel_info->trough_color.pixel=
    XStandardPixel(map_info,pixel_info->trough_color,16);
  pixel_info->trough_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set pen color.
  */
  for (i=0; i < MaxNumberPens; i++)
  {
    (void) XParseColor(display,colormap,PenColors[i],&pixel_info->pen_color[i]);
    status=XParseColor(display,colormap,resource_info->pen_color[i],
      &pixel_info->pen_color[i]);
    if (status == 0)
      Warning("Color is not known to X server",resource_info->pen_color[i]);
    pixel_info->pen_color[i].pixel=
      XStandardPixel(map_info,pixel_info->pen_color[i],16);
    pixel_info->pen_color[i].flags=DoRed | DoGreen | DoBlue;
  }
  pixel_info->annotate_color=pixel_info->foreground_color;
  if (image != (Image *) NULL)
    if (image->class == PseudoClass)
      {
        /*
          Initialize pixel array for images of type PseudoClass.
        */
        for (i=0; i < image->colors; i++)
          pixel_info->pixels[i]=XStandardPixel(map_info,image->colormap[i],8);
        for (i=0; i < MaxNumberPens; i++)
          pixel_info->pixels[image->colors+i]=pixel_info->pen_color[i].pixel;
        pixel_info->colors+=MaxNumberPens;
      }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e C l a s s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResourceClass queries the X server for the specified resource
%  name or class.  If the resource name or class is not defined in the
%  database, the supplied default value is returned.
%
%  The format of the XGetResourceClass routine is:
%
%      value=XGetResourceClass(database,client_name,keyword,resource_default)
%
%  A description of each parameter follows:
%
%    o value: Function XGetResourceClass returns the resource value associated
%      with the name or class.  If none is found, the supplied default value is
%      returned.
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve resource
%      info from the X server database.
%
%    o keyword: Specifies the keyword of the value being retrieved.
%
%    o resource_default: Specifies the default value to return if the query
%      fails to find the specified keyword/class.
%
%
*/
char *XGetResourceClass(database,client_name,keyword,resource_default)
XrmDatabase
  database;

char
  *client_name,
  *keyword,
  *resource_default;
{
  char
    *resource_type,
    resource_class[MaxTextLength],
    resource_name[MaxTextLength];

  int
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return(resource_default);
  *resource_name='\0';
  *resource_class='\0';
  if (keyword != (char *) NULL)
    {
      unsigned char
        c,
        k;

      /*
        Initialize resource keyword and class.
      */
      (void) sprintf(resource_name,"%s.%s",client_name,keyword);
      c=(*client_name);
      if ((c >= XK_a) && (c <= XK_z))
        c-=(XK_a-XK_A);
      else
        if ((c >= XK_agrave) && (c <= XK_odiaeresis))
          c-=(XK_agrave-XK_Agrave);
        else
          if ((c >= XK_oslash) && (c <= XK_thorn))
            c-=(XK_oslash-XK_Ooblique);
      k=(*keyword);
      if ((k >= XK_a) && (k <= XK_z))
        k-=(XK_a-XK_A);
      else
        if ((k >= XK_agrave) && (k <= XK_odiaeresis))
          k-=(XK_agrave-XK_Agrave);
        else
          if ((k >= XK_oslash) && (k <= XK_thorn))
            k-=(XK_oslash-XK_Ooblique);
      (void) sprintf(resource_class,"%c%s.%c%s",c,client_name+1,k,keyword+1);
    }
  status=XrmGetResource(database,resource_name,resource_class,&resource_type,
    &resource_value);
  if (status == False)
    return(resource_default);
  return(resource_value.addr);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResourceInfo initializes the ResourceInfo structure.
%
%  The format of the XGetResourceInfo routine is:
%
%      XGetResourceInfo(resource_database,client_name,resource_info)
%
%  A description of each parameter follows:
%
%    o resource_database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
void XGetResourceInfo(resource_database,client_name,resource_info)
XrmDatabase
  resource_database;

char
  *client_name;

XResourceInfo
  *resource_info;
{
  char
    *resource_value;

  register char
    *p;

  if (client_name != (char *) NULL)
    {
      /*
        Get basename of client.
      */
      p=client_name+(strlen(client_name)-1);
      while ((p > client_name) && (*p != '/'))
        p--;
      if (*p == '/')
        client_name=p+1;
    }
  /*
    Initialize resource info fields.
  */
  resource_info->resource_database=resource_database;
  resource_value=
    XGetResourceClass(resource_database,client_name,"backdrop","False");
  resource_info->backdrop=IsTrue(resource_value);
  resource_info->background_color=XGetResourceInstance(resource_database,
    client_name,"background",BackgroundColor);
  resource_info->border_color=XGetResourceInstance(resource_database,
    client_name,"borderColor",BorderColor);
  resource_value=
    XGetResourceClass(resource_database,client_name,"borderWidth","2");
  resource_info->border_width=atoi(resource_value);
  resource_value=
    XGetResourceClass(resource_database,client_name,"colormap","shared");
  resource_info->colormap=UndefinedColormap;
  if (Latin1Compare("private",resource_value) == 0)
    resource_info->colormap=PrivateColormap;
  if (Latin1Compare("shared",resource_value) == 0)
    resource_info->colormap=SharedColormap;
  if (resource_info->colormap == UndefinedColormap)
    Warning("Unrecognized colormap type",resource_value);
  resource_info->color_recovery=False;
  resource_value=XGetResourceClass(resource_database,client_name,"colors","0");
  resource_info->number_colors=atoi(resource_value);
  resource_value=
    XGetResourceClass(resource_database,client_name,"colorspace","rgb");
  resource_info->colorspace=UndefinedColorspace;
  if (Latin1Compare("gray",resource_value) == 0)
    resource_info->colorspace=GRAYColorspace;
  if (Latin1Compare("rgb",resource_value) == 0)
    resource_info->colorspace=RGBColorspace;
  if (Latin1Compare("ohta",resource_value) == 0)
    resource_info->colorspace=OHTAColorspace;
  if (Latin1Compare("xyz",resource_value) == 0)
    resource_info->colorspace=XYZColorspace;
  if (Latin1Compare("yiq",resource_value) == 0)
    resource_info->colorspace=YIQColorspace;
  if (Latin1Compare("yuv",resource_value) == 0)
    resource_info->colorspace=YUVColorspace;
  if (resource_info->colorspace == UndefinedColorspace)
    Warning("Unrecognized colorspace type",resource_value);
  resource_info->editor_command=XGetResourceClass(resource_database,client_name,
    "editorCommand",EditorCommand);
  resource_value=
    XGetResourceClass(resource_database,client_name,"debug","False");
  resource_info->debug=IsTrue(resource_value);
  resource_value=XGetResourceClass(resource_database,client_name,"delay","0");
  resource_info->delay=atoi(resource_value);
  resource_value=
    XGetResourceClass(resource_database,client_name,"dither","True");
  resource_info->dither=IsTrue(resource_value);
  resource_info->font=
    XGetResourceClass(resource_database,client_name,"fontList",(char *) NULL);
  resource_info->font_name[0]=
    XGetResourceClass(resource_database,client_name,"font1","fixed");
  resource_info->font_name[1]=
    XGetResourceClass(resource_database,client_name,"font2","variable");
  resource_info->font_name[2]=
    XGetResourceClass(resource_database,client_name,"font3","5x8");
  resource_info->font_name[3]=
    XGetResourceClass(resource_database,client_name,"font4","6x10");
  resource_info->font_name[4]=
    XGetResourceClass(resource_database,client_name,"font5","7x13bold");
  resource_info->font_name[5]=
    XGetResourceClass(resource_database,client_name,"font6","8x13bold");
  resource_info->font_name[6]=
    XGetResourceClass(resource_database,client_name,"font7","9x15bold");
  resource_info->font_name[7]=
    XGetResourceClass(resource_database,client_name,"font8","10x20");
  resource_info->font_name[8]=
    XGetResourceClass(resource_database,client_name,"font9","12x24");
  resource_info->font_name[9]=
    XGetResourceClass(resource_database,client_name,"font0","fixed");
  resource_info->foreground_color=XGetResourceInstance(resource_database,
    client_name,"foreground",ForegroundColor);
  resource_info->icon_geometry=XGetResourceClass(resource_database,client_name,
    "iconGeometry",(char *) NULL);
  resource_value=
    XGetResourceClass(resource_database,client_name,"gravity","North");
  resource_info->gravity=(-1);
  if (Latin1Compare("Forget",resource_value) == 0)
    resource_info->gravity=ForgetGravity;
  if (Latin1Compare("NorthWest",resource_value) == 0)
    resource_info->gravity=NorthWestGravity;
  if (Latin1Compare("North",resource_value) == 0)
    resource_info->gravity=NorthGravity;
  if (Latin1Compare("NorthEast",resource_value) == 0)
    resource_info->gravity=NorthEastGravity;
  if (Latin1Compare("West",resource_value) == 0)
    resource_info->gravity=WestGravity;
  if (Latin1Compare("Center",resource_value) == 0)
    resource_info->gravity=CenterGravity;
  if (Latin1Compare("East",resource_value) == 0)
    resource_info->gravity=EastGravity;
  if (Latin1Compare("SouthWest",resource_value) == 0)
    resource_info->gravity=SouthWestGravity;
  if (Latin1Compare("South",resource_value) == 0)
    resource_info->gravity=SouthGravity;
  if (Latin1Compare("SouthEast",resource_value) == 0)
    resource_info->gravity=SouthEastGravity;
  if (Latin1Compare("Static",resource_value) == 0)
    resource_info->gravity=StaticGravity;
  if (resource_info->gravity == (-1))
    {
      Warning("Unrecognized gravity type",resource_value);
      resource_info->gravity=CenterGravity;
    }
  resource_value=
    XGetResourceClass(resource_database,client_name,"iconic","False");
  resource_info->iconic=IsTrue(resource_value);
  resource_info->image_geometry=XGetResourceClass(resource_database,
    client_name,"imageGeometry",(char *) NULL);
  resource_value=XGetResourceClass(resource_database,client_name,"magnify","3");
  resource_info->magnify=atoi(resource_value);
  resource_info->map_type=
    XGetResourceClass(resource_database,client_name,"map",(char *) NULL);
  resource_info->matte_color=XGetResourceInstance(resource_database,
    client_name,"mattecolor",(char *) NULL);
  resource_value=
    XGetResourceClass(resource_database,client_name,"monochrome","False");
  resource_info->monochrome=IsTrue(resource_value);
  resource_info->name=
    XGetResourceClass(resource_database,client_name,"name",(char *) NULL);
  resource_info->pan_geometry=XGetResourceClass(resource_database,client_name,
    "panGeometry",(char *) NULL);
  resource_info->pen_color[0]=
    XGetResourceClass(resource_database,client_name,"pen1","black");
  resource_info->pen_color[1]=
    XGetResourceClass(resource_database,client_name,"pen2","blue");
  resource_info->pen_color[2]=
    XGetResourceClass(resource_database,client_name,"pen3","cyan");
  resource_info->pen_color[3]=
    XGetResourceClass(resource_database,client_name,"pen4","green");
  resource_info->pen_color[4]=
    XGetResourceClass(resource_database,client_name,"pen5","gray");
  resource_info->pen_color[5]=
    XGetResourceClass(resource_database,client_name,"pen6","red");
  resource_info->pen_color[6]=
    XGetResourceClass(resource_database,client_name,"pen7","magenta");
  resource_info->pen_color[7]=
    XGetResourceClass(resource_database,client_name,"pen8","yellow");
  resource_info->pen_color[8]=
    XGetResourceClass(resource_database,client_name,"pen9","white");
  resource_info->pen_color[9]=
    XGetResourceClass(resource_database,client_name,"pen0","gray");
  resource_info->print_command=XGetResourceClass(resource_database,client_name,
    "printCommand",PrintCommand);
  resource_info->server_name=
    XGetResourceClass(resource_database,client_name,"serverName",(char *) NULL);
  resource_value=
    XGetResourceClass(resource_database,client_name,"sharedMemory","True");
  resource_info->use_shared_memory=IsTrue(resource_value);
  resource_info->text_font=
    XGetResourceClass(resource_database,client_name,"textFont",(char *) NULL);
  resource_info->title=
    XGetResourceClass(resource_database,client_name,"title",(char *) NULL);
  resource_value=
    XGetResourceClass(resource_database,client_name,"treeDepth","0");
  resource_info->tree_depth=atoi(resource_value);
  resource_value=
    XGetResourceClass(resource_database,client_name,"update","False");
  resource_info->update=IsTrue(resource_value);
  resource_value=
    XGetResourceClass(resource_database,client_name,"usePixmap","False");
  resource_info->use_pixmap=IsTrue(resource_value);
  resource_info->visual_type=
    XGetResourceClass(resource_database,client_name,"visual",(char *) NULL);
  resource_info->window_id=
    XGetResourceClass(resource_database,client_name,"window",(char *) NULL);
  resource_info->write_filename=XGetResourceClass(resource_database,
    client_name,"writeFilename",(char *) NULL);
  /*
    Handle side-effects.
  */
  if (resource_info->monochrome)
    {
      resource_info->number_colors=2;
      resource_info->tree_depth=8;
      resource_info->dither=True;
      resource_info->colorspace=GRAYColorspace;
    }
  if (resource_info->colorspace == GRAYColorspace)
    {
      resource_info->number_colors=256;
      resource_info->tree_depth=8;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e I n s t a n c e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResourceInstance queries the X server for the specified
%  resource name.  If the resource name is not defined in the database, the
%  supplied default value is returned.
%
%  The format of the XGetResourceInstance routine is:
%
%      value=XGetResourceInstance(database,client_name,keyword,resource_default)
%
%  A description of each parameter follows:
%
%    o value: Function XGetResourceInstance returns the resource value
%      associated with the name or class.  If none is found, the supplied
%      default value is returned.
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o keyword: Specifies the keyword of the value being retrieved.
%
%    o resource_default: Specifies the default value to return if the query
%      fails to find the specified keyword/class.
%
%
*/
char *XGetResourceInstance(database,client_name,keyword,resource_default)
XrmDatabase
  database;

char
  *client_name,
  *keyword,
  *resource_default;
{
  char
    *resource_type,
    resource_name[MaxTextLength];

  int
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return(resource_default);
  *resource_name='\0';
  if (keyword != (char *) NULL)
    (void) sprintf(resource_name,"%s.%s",client_name,keyword);
  status=XrmGetResource(database,resource_name,(char *) NULL,&resource_type,
    &resource_value);
  if (status == False)
    return(resource_default);
  return(resource_value.addr);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w C o l o r                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetWindowColor returns the color of a pixel interactively choosen
%  from the X server.
%
%  The format of the XGetWindowColor routine is:
%
%      status=XGetWindowColor(display,color)
%
%  A description of each parameter follows:
%
%    o status: Function XGetWindowColor returns True if the color is obtained
%      from the X server.  False is returned if any errors occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o color: The color of the pixel choosen is returned in this XColor
%      structure.
%
%
*/
unsigned int XGetWindowColor(display,color)
Display
  *display;

XColor
  *color;
{
  int
    x,
    y;

  RectangleInfo
    crop_info;

  unsigned int
    status;

  Window
    child,
    client_window,
    root_window,
    target_window;

  XImage
    *ximage;

  XWindowAttributes
    window_attributes;

  /*
    Choose a pixel from the X server.
  */
  target_window=XSelectWindow(display,&crop_info);
  root_window=XRootWindow(display,XDefaultScreen(display));
  client_window=target_window;
  if (target_window != root_window)
    {
      unsigned int
        d;

      /*
        Get client window.
      */
      status=XGetGeometry(display,target_window,&root_window,&x,&x,&d,&d,&d,&d);
      if (status != 0)
        {
          client_window=XClientWindow(display,target_window);
          target_window=client_window;
        }
    }
  /*
    Verify window is viewable.
  */
  status=XGetWindowAttributes(display,target_window,&window_attributes);
  if ((status == False) || (window_attributes.map_state != IsViewable))
    return(False);
  /*
    Get window X image.
  */
  XTranslateCoordinates(display,root_window,target_window,crop_info.x,
    crop_info.y,&x,&y,&child);
  ximage=XGetImage(display,target_window,x,y,1,1,AllPlanes,ZPixmap);
  if (ximage == (XImage *) NULL)
    return(False);
  color->pixel=XGetPixel(ximage,0,0);
  XDestroyImage(ximage);
  /*
    Query X server for pixel color.
  */
  XQueryColor(display,window_attributes.colormap,color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetWindowImage reads an image from the target X window and returns
%  it.  XGetWindowImage optionally descends the window hierarchy and overlays
%  the target image with each subwindow image.
%
%  The format of the XGetWindowImage routine is:
%
%      image=XGetWindowImage(display,window,borders,level)
%
%  A description of each parameter follows:
%
%    o image: Function XGetWindowImage returns a MIFF image if it can be
%      successfully read from the X window.  A null image is returned if
%      any errors occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the window to obtain the image from.
%
%    o borders: Specifies whether borders pixels are to be saved with
%      the image.
%
%    o level: Specifies an unsigned integer representing the level of
%      decent in the window hierarchy.  This value must be zero or one on
%      the initial call to XGetWindowImage.  A value of zero returns after
%      one call.  A value of one causes the function to descend the window
%      hierarchy and overlays the target image with each subwindow image.
%
%
*/
Image *XGetWindowImage(display,window,borders,level)
Display
  *display;

Window
  window;

unsigned int
  borders,
  level;
{
  typedef struct _ColormapList
  {
    Colormap
      colormap;

    XColor
      *colors;

    struct _ColormapList
      *next;
  } ColormapList;

  GC
    annotate_context;

  Image
    *image;

  int
    display_height,
    display_width,
    number_colors,
    status,
    x_offset,
    y_offset;

  RectangleInfo
    crop_info;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *p;

  register unsigned long
    pixel;

  static ColormapList
    *colormap_list = (ColormapList *) NULL;

  Window
    child,
    root_window;

  XColor
    *colors;

  XGCValues
    context_values;

  XImage
    *ximage;

  XWindowAttributes
    window_attributes;

  /*
    Verify window is viewable.
  */
  status=XGetWindowAttributes(display,window,&window_attributes);
  if ((status == False) || (window_attributes.map_state != IsViewable))
    return((Image *) NULL);
  /*
    Cropping rectangle is relative to root window.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  XTranslateCoordinates(display,window,root_window,0,0,&x_offset,&y_offset,
    &child);
  crop_info.x=x_offset;
  crop_info.y=y_offset;
  crop_info.width=window_attributes.width;
  crop_info.height=window_attributes.height;
  if (borders)
    {
      /*
        Include border in image.
      */
      crop_info.x-=window_attributes.border_width;
      crop_info.y-=window_attributes.border_width;
      crop_info.width+=window_attributes.border_width << 1;
      crop_info.height+=window_attributes.border_width << 1;
    }
  /*
    Crop to root window.
  */
  if (crop_info.x < 0)
    {
      if ((crop_info.x+(int) crop_info.width) < 0)
        return((Image *) NULL);
      crop_info.width+=crop_info.x;
      crop_info.x=0;
    }
  if (crop_info.y < 0)
    {
      if ((crop_info.y+(int) crop_info.height) < 0)
        return((Image *) NULL);
      crop_info.height+=crop_info.y;
      crop_info.y=0;
    }
  display_width=XDisplayWidth(display,XDefaultScreen(display));
  if ((crop_info.x+(int) crop_info.width) > display_width)
    {
      if (crop_info.x >= display_width)
        return((Image *) NULL);
      crop_info.width=display_width-crop_info.x;
    }
  display_height=XDisplayHeight(display,XDefaultScreen(display));
  if ((crop_info.y+(int) crop_info.height) > display_height)
    {
      if (crop_info.y >= display_height)
        return((Image *) NULL);
      crop_info.height=display_height-crop_info.y;
    }
  crop_info.x-=x_offset;
  crop_info.y-=y_offset;
  /*
    Alert user we are about to get an X region by flashing a border.
  */
  context_values.background=XBlackPixel(display,XDefaultScreen(display));
  context_values.foreground=XWhitePixel(display,XDefaultScreen(display));
  context_values.function=GXinvert;
  context_values.plane_mask=
    context_values.background ^ context_values.foreground;
  context_values.subwindow_mode=IncludeInferiors;
  annotate_context=XCreateGC(display,window,GCBackground | GCForeground |
    GCFunction | GCPlaneMask | GCSubwindowMode,&context_values);
  if (annotate_context != (GC) NULL)
    {
      XHighlightRegion(display,window,annotate_context,&crop_info);
      XDelay(display,(unsigned long) (SuspendTime << 2));
      XHighlightRegion(display,window,annotate_context,&crop_info);
    }
  /*
    Get window X image.
  */
  ximage=XGetImage(display,window,crop_info.x,crop_info.y,crop_info.width,
    crop_info.height,AllPlanes,ZPixmap);
  if (ximage == (XImage *) NULL)
    return((Image *) NULL);
  number_colors=0;
  colors=(XColor *) NULL;
  if (window_attributes.colormap != (Colormap) NULL)
    {
      ColormapList
        *p;

      /*
        Search colormap list for window colormap.
      */
      number_colors=window_attributes.visual->map_entries;
      for (p=colormap_list; p != (ColormapList *) NULL; p=p->next)
        if (p->colormap == window_attributes.colormap)
          break;
      if (p == (ColormapList *) NULL)
        {
          /*
            Get the window colormap.
          */
          colors=(XColor *) malloc(number_colors*sizeof(XColor));
          if (colors == (XColor *) NULL)
            {
              XDestroyImage(ximage);
              return((Image *) NULL);
            }
          if ((window_attributes.visual->class != DirectColor) &&
              (window_attributes.visual->class != TrueColor))
            for (i=0; i < number_colors; i++)
            {
              colors[i].pixel=i;
              colors[i].pad=0;
            }
          else
            {
              unsigned long
                blue,
                blue_bit,
                green,
                green_bit,
                red,
                red_bit;

              /*
                DirectColor or TrueColor visual.
              */
              red=0;
              green=0;
              blue=0;
              red_bit=window_attributes.visual->red_mask &
                (~(window_attributes.visual->red_mask)+1);
              green_bit=window_attributes.visual->green_mask &
                (~(window_attributes.visual->green_mask)+1);
              blue_bit=window_attributes.visual->blue_mask &
                (~(window_attributes.visual->blue_mask)+1);
              for (i=0; i < number_colors; i++)
              {
                colors[i].pixel=red | green | blue;
                colors[i].pad=0;
                red+=red_bit;
                if (red > window_attributes.visual->red_mask)
                  red=0;
                green+=green_bit;
                if (green > window_attributes.visual->green_mask)
                  green=0;
                blue+=blue_bit;
                if (blue > window_attributes.visual->blue_mask)
                  blue=0;
              }
            }
          XQueryColors(display,window_attributes.colormap,colors,
           (int) number_colors);
          /*
            Append colormap to colormap list.
          */
          p=(ColormapList *) malloc(sizeof(ColormapList));
          p->colormap=window_attributes.colormap;
          p->colors=colors;
          p->next=colormap_list;
          colormap_list=p;
        }
      colors=p->colors;
    }
  /*
    Allocate image structure.
  */
  image=AllocateImage((ImageInfo *) NULL);
  if (image == (Image *) NULL)
    {
      XDestroyImage(ximage);
      return((Image *) NULL);
    }
  /*
    Convert X image to MIFF format.
  */
  if ((window_attributes.visual->class != TrueColor) &&
      (window_attributes.visual->class != DirectColor))
    image->class=PseudoClass;
  image->columns=ximage->width;
  image->rows=ximage->height;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      XDestroyImage(ximage);
      DestroyImage(image);
      return((Image *) NULL);
    }
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      register unsigned long
        color,
        index;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=window_attributes.visual->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=window_attributes.visual->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=window_attributes.visual->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if ((number_colors > 0) &&
          (window_attributes.visual->class == DirectColor))
        for (y=0; y < image->rows; y++)
        {
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            index=(pixel >> red_shift) & red_mask;
            p->red=ColorShift(colors[index].red);
            index=(pixel >> green_shift) & green_mask;
            p->green=ColorShift(colors[index].green);
            index=(pixel >> blue_shift) & blue_mask;
            p->blue=ColorShift(colors[index].blue);
            p->index=0;
            p->length=0;
            p++;
          }
        }
      else
        for (y=0; y < image->rows; y++)
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            color=(pixel >> red_shift) & red_mask;
            p->red=ColorShift(((unsigned long) color*65535)/red_mask);
            color=(pixel >> green_shift) & green_mask;
            p->green=ColorShift(((unsigned long) color*65535)/green_mask);
            color=(pixel >> blue_shift) & blue_mask;
            p->blue=ColorShift(((unsigned long) color*65535)/blue_mask);
            p->index=0;
            p->length=0;
            p++;
          }
      break;
    }
    case PseudoClass:
    {
      /*
        Create colormap.
      */
      image->colors=number_colors;
      image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        {
          XDestroyImage(ximage);
          DestroyImage(image);
          return((Image *) NULL);
        }
      for (i=0; i < image->colors; i++)
      {
        image->colormap[colors[i].pixel].red=ColorShift(colors[i].red);
        image->colormap[colors[i].pixel].green=ColorShift(colors[i].green);
        image->colormap[colors[i].pixel].blue=ColorShift(colors[i].blue);
      }
      /*
        Convert X image to PseudoClass packets.
      */
      for (y=0; y < image->rows; y++)
        for (x=0; x < image->columns; x++)
        {
          pixel=XGetPixel(ximage,x,y);
          p->index=(unsigned short) pixel;
          p->length=0;
          p++;
        }
      SyncImage(image);
      break;
    }
  }
  XDestroyImage(ximage);
  if (annotate_context != (GC) NULL)
    {
      /*
        Alert user we got the X region by flashing a border.
      */
      XHighlightRegion(display,window,annotate_context,&crop_info);
      XFlush(display);
      XHighlightRegion(display,window,annotate_context,&crop_info);
      XFreeGC(display,annotate_context);
    }
  if (level != 0)
    {
      unsigned int
        number_children;

      Window
        *children,
        parent;

      /*
        Descend the window hierarchy and overlay with each subwindow image.
      */
      status=XQueryTree(display,window,&root_window,&parent,&children,
        &number_children);
      if ((status == True) && (number_children != 0))
        {
          Image
            *child_image;

          /*
            Composite any children in back-to-front order.
          */
          for (i=0; i < number_children; i++)
          {
            child_image=XGetWindowImage(display,children[i],False,level+1);
            if (child_image != (Image *) NULL)
              {
                /*
                  Composite child window image.
                */
                XTranslateCoordinates(display,children[i],window,0,0,&x_offset,
                  &y_offset,&child);
                x_offset-=crop_info.x;
                if (x_offset < 0)
                  x_offset=0;
                y_offset-=crop_info.y;
                if (y_offset < 0)
                  y_offset=0;
                CompositeImage(image,ReplaceCompositeOp,child_image,x_offset,
                  y_offset);
                DestroyImage(child_image);
              }
          }
          XFree((void *) children);
        }
    }
  if (level <= 1)
    {
      ColormapList
        *next;

      /*
        Free resources.
      */
      while (colormap_list != (ColormapList *) NULL)
      {
        next=colormap_list->next;
        (void) free((char *) colormap_list->colors);
        (void) free((char *) colormap_list);
        colormap_list=next;
      }
      if (image->class == PseudoClass)
        CompressColormap(image);
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetWindowInfo initializes the XWindowInfo structure.
%
%  The format of the XGetWindowInfo routine is:
%
%      XGetWindowInfo(display,visual_info,map_info,pixel_info,font_info,
%        resource_info,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o font_info: Specifies a pointer to a XFontStruct structure.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
void XGetWindowInfo(display,visual_info,map_info,pixel_info,font_info,
  resource_info,window)
Display
  *display;

XVisualInfo
  *visual_info;

XStandardColormap
 *map_info;

XPixelInfo
  *pixel_info;

XFontStruct
  *font_info;

XResourceInfo
  *resource_info;

XWindowInfo
  *window;
{
  /*
    Initialize window info.
  */
  if (window->id != (Window) NULL)
    {
      XFreeCursor(display,window->cursor);
      XFreeCursor(display,window->busy_cursor);
      if (window->highlight_stipple != (Pixmap) NULL)
        XFreePixmap(display,window->highlight_stipple);
      if (window->shadow_stipple != (Pixmap) NULL)
        XFreePixmap(display,window->shadow_stipple);
    }
  else
    {
      int
        x;

      window->id=(Window) NULL;
      window->ximage=(XImage *) NULL;
      window->matte_image=(XImage *) NULL;
      window->pixmap=(Pixmap) NULL;
      window->matte_pixmap=(Pixmap) NULL;
      window->mapped=False;
      window->stasis=False;
      window->shared_memory=False;
#ifdef HasSharedMemory
      window->shared_memory=False;
      if (resource_info->use_shared_memory)
        (void) XShmQueryExtension(display,&x,&x,&window->shared_memory);
      window->ximage_segment_info.shmid=(-1);
      window->pixmap_segment_info.shmid=(-1);
#endif
    }
  window->screen=visual_info->screen;
  window->visual=visual_info->visual;
  window->class=visual_info->class;
  window->depth=visual_info->depth;
  window->visual_info=visual_info;
  window->map_info=map_info;
  window->pixel_info=pixel_info;
  window->font_info=font_info;
  window->cursor=XCreateFontCursor(display,XC_left_ptr);
  window->busy_cursor=XCreateFontCursor(display,XC_watch);
  window->name=(char *) NULL;
  window->geometry=(char *) NULL;
  window->icon_name=(char *) NULL;
  window->icon_geometry=resource_info->icon_geometry;
  window->crop_geometry=(char *) NULL;
  window->flags=PSize;
  window->x=0;
  window->y=0;
  window->x_origin=0;
  window->y_origin=0;
  window->width=1;
  window->height=1;
  window->min_width=1;
  window->min_height=1;
  window->width_inc=1;
  window->height_inc=1;
  window->border_width=resource_info->border_width;
  window->annotate_context=pixel_info->annotate_context;
  window->highlight_context=pixel_info->highlight_context;
  window->widget_context=pixel_info->widget_context;
  window->shadow_stipple=(Pixmap) NULL;
  window->highlight_stipple=(Pixmap) NULL;
  window->immutable=False;
  window->mask=CWBackingStore | CWBackPixel | CWBackPixmap | CWBitGravity |
    CWBorderPixel | CWColormap | CWCursor | CWDontPropagate | CWEventMask |
    CWOverrideRedirect | CWSaveUnder | CWWinGravity;
  window->attributes.background_pixel=pixel_info->background_color.pixel;
  window->attributes.background_pixmap=(Pixmap) NULL;
  window->attributes.backing_store=NotUseful;
  window->attributes.bit_gravity=ForgetGravity;
  window->attributes.border_pixel=pixel_info->border_color.pixel;
  window->attributes.colormap=map_info->colormap;
  window->attributes.cursor=window->cursor;
  window->attributes.do_not_propagate_mask=NoEventMask;
  window->attributes.event_mask=NoEventMask;
  window->attributes.override_redirect=False;
  window->attributes.save_under=False;
  window->attributes.win_gravity=NorthWestGravity;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X H i g h l i g h t L i n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XHighlightLine puts a border on the X server around a region
%  defined by highlight_info.
%
%  The format of the XHighlightLine routine is:
%
%    XHighlightLine(display,window,annotate_context,highlight_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o annotate_context: Specifies a pointer to a GC structure.
%
%    o highlight_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any highlighting rectangle.
%
%
*/
void XHighlightLine(display,window,annotate_context,highlight_info)
Display
  *display;

Window
  window;

GC
  annotate_context;

XSegment
  *highlight_info;
{
  XDrawLine(display,window,annotate_context,highlight_info->x1,
    highlight_info->y1,highlight_info->x2,highlight_info->y2);
/*  XDrawLine(display,window,annotate_context,highlight_info->x1+1,
    highlight_info->y1+1,highlight_info->x2+1,highlight_info->y2+1);*/
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X H i g h l i g h t R e g i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XHighlightRegion puts a border on the X server around a region
%  defined by highlight_info.
%
%  The format of the XHighlightRegion routine is:
%
%    XHighlightRegion(display,window,annotate_context,highlight_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o annotate_context: Specifies a pointer to a GC structure.
%
%    o highlight_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any highlighting rectangle.
%
%
*/
void XHighlightRegion(display,window,annotate_context,highlight_info)
Display
  *display;

Window
  window;

GC
  annotate_context;

RectangleInfo
  *highlight_info;
{
  if ((highlight_info->width < 4) || (highlight_info->height < 4))
    return;
  XDrawRectangle(display,window,annotate_context,highlight_info->x,
    highlight_info->y,highlight_info->width-1,highlight_info->height-1);
  XDrawRectangle(display,window,annotate_context,highlight_info->x+1,
    highlight_info->y+1,highlight_info->width-3,highlight_info->height-3);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e C u r s o r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeCursor creates a crosshairs X11 cursor.
%
%  The format of the XMakeCursor routine is:
%
%      XMakeCursor(display,window,colormap,background_color,foreground_color)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the ID of the window for which the cursor is
%      assigned.
%
%    o colormap: Specifies the ID of the colormap from which the background
%      and foreground color will be retrieved.
%
%    o background_color: Specifies the color to use for the cursor background.
%
%    o foreground_color: Specifies the color to use for the cursor foreground.
%
%
*/
Cursor XMakeCursor(display,window,colormap,background_color,foreground_color)
Display
  *display;

Window
  window;

Colormap
  colormap;

char
  *background_color,
  *foreground_color;
{
#define scope_height 17
#define scope_x_hot 8
#define scope_y_hot 8
#define scope_width 17

  static unsigned char
    scope_bits[] =
    {
      0x80, 0x03, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02,
      0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x7f,
      0xfc, 0x01, 0x01, 0x00, 0x01, 0x7f, 0xfc, 0x01, 0x80, 0x02, 0x00,
      0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02,
      0x00, 0x80, 0x02, 0x00, 0x80, 0x03, 0x00
    };

  static unsigned char
    scope_mask_bits[] =
    {
      0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06,
      0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xff, 0xfe, 0x01, 0x7f,
      0xfc, 0x01, 0x03, 0x80, 0x01, 0x7f, 0xfc, 0x01, 0xff, 0xfe, 0x01,
      0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06,
      0x00, 0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00
    };

  Cursor
    cursor;

  Pixmap
    mask,
    source;

  XColor
    background,
    foreground;

  source=XCreateBitmapFromData(display,window,(char *) scope_bits,scope_width,
    scope_height);
  mask=XCreateBitmapFromData(display,window,(char *) scope_mask_bits,
    scope_width,scope_height);
  if ((source == (Pixmap) NULL) || (mask == (Pixmap) NULL))
    Error("Unable to create pixmap",(char *) NULL);
  XParseColor(display,colormap,background_color,&background);
  XParseColor(display,colormap,foreground_color,&foreground);
  cursor=XCreatePixmapCursor(display,source,mask,&foreground,&background,
    scope_x_hot,scope_y_hot);
  XFreePixmap(display,source);
  XFreePixmap(display,mask);
  return(cursor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImage creates an X11 image.  If the image size differs from
%  the X11 image size, the image is first resized.
%
%  The format of the XMakeImage routine is:
%
%      status=XMakeImage(display,resource_info,window,image,width,height)
%
%  A description of each parameter follows:
%
%    o status: Function XMakeImage returns True if the X image is
%      successfully created.  False is returned is there is a memory shortage.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o width: Specifies the width in pixels of the rectangular area to
%      display.
%
%    o height: Specifies the height in pixels of the rectangular area to
%      display.
%
%
*/
unsigned int XMakeImage(display,resource_info,window,image,width,height)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *window;

Image
  *image;

unsigned int
  width,
  height;
{
  Image
    *transformed_image;

  int
    format;

#ifdef HasSharedMemory
  static XShmSegmentInfo
    segment_info;
#endif

  XImage
    *matte_image,
    *ximage;

  if ((window->width == 0) || (window->height == 0))
    return(False);
  /*
    Display busy cursor.
  */
  XDefineCursor(display,window->id,window->busy_cursor);
  XFlush(display);
  transformed_image=image;
  if (image != (Image *) NULL)
    {
      /*
        Apply user transforms to the image.
      */
      if (window->crop_geometry)
        {
          Image
            *cropped_image;

          RectangleInfo
            crop_info;

          /*
            Crop image.
          */
          (void) XParseGeometry(window->crop_geometry,&crop_info.x,&crop_info.y,
            &crop_info.width,&crop_info.height);
          transformed_image->orphan=True;
          cropped_image=CropImage(transformed_image,&crop_info);
          transformed_image->orphan=False;
          if (transformed_image != image)
            DestroyImage(transformed_image);
          if (cropped_image == (Image *) NULL)
            return(False);
          transformed_image=cropped_image;
        }
      if ((width != transformed_image->columns) ||
          (height != transformed_image->rows))
        {
          Image
            *scaled_image;

          /*
            Scale image.
          */
          transformed_image->orphan=True;
          if (window->pixel_info->colors != 0)
            scaled_image=SampleImage(transformed_image,width,height);
          else
            scaled_image=ScaleImage(transformed_image,width,height);
          transformed_image->orphan=False;
          if (transformed_image != image)
            DestroyImage(transformed_image);
          if (scaled_image == (Image *) NULL)
            return(False);
          transformed_image=scaled_image;
        }
      width=transformed_image->columns;
      height=transformed_image->rows;
    }
  /*
    Create X image.
  */
  format=(window->depth == 1) ? XYBitmap : ZPixmap;
#ifdef HasSharedMemory
  if (window->shared_memory)
    {
      ximage=XShmCreateImage(display,window->visual,window->depth,format,0,
        &segment_info,width,height);
      segment_info.shmid=shmget(IPC_PRIVATE,ximage->bytes_per_line*
        ximage->height,IPC_CREAT | 0777);
      window->shared_memory=segment_info.shmid >= 0;
      if (window->shared_memory)
        segment_info.shmaddr=(char *) shmat(segment_info.shmid,0,0);
    }
#endif
  if (!window->shared_memory)
    ximage=XCreateImage(display,window->visual,window->depth,format,0,
      (char *) NULL,width,height,XBitmapPad(display),0);
  if (ximage == (XImage *) NULL)
    {
      /*
        Unable to create X image.
      */
      XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"XImage:\n");
      (void) fprintf(stderr,"  width, height: %dx%d\n",ximage->width,
        ximage->height);
      (void) fprintf(stderr,"  format: %d\n",ximage->format);
      (void) fprintf(stderr,"  byte order: %d\n",ximage->byte_order);
      (void) fprintf(stderr,"  bitmap unit, bit order, pad: %d %d %d\n",
        ximage->bitmap_unit,ximage->bitmap_bit_order,ximage->bitmap_pad);
      (void) fprintf(stderr,"  depth: %d\n",ximage->depth);
      (void) fprintf(stderr,"  bytes per line: %d\n",ximage->bytes_per_line);
      (void) fprintf(stderr,"  bits per pixel: %d\n",ximage->bits_per_pixel);
      (void) fprintf(stderr,"  red, green, blue masks: 0x%lx 0x%lx 0x%lx\n",
        ximage->red_mask,ximage->green_mask,ximage->blue_mask);
    }
  /*
    Allocate X image pixel data.
  */
#ifdef HasSharedMemory
  if (window->shared_memory)
    {
      segment_info.readOnly=False;
      XShmAttach(display,&segment_info);
      XSync(display,False);
      shmctl(segment_info.shmid,IPC_RMID,0);
      ximage->data=segment_info.shmaddr;
    }
#endif
  if (!window->shared_memory)
    if (ximage->format == XYBitmap)
      ximage->data=(char *)
        malloc(ximage->bytes_per_line*ximage->height*ximage->depth);
    else
      ximage->data=(char *) malloc(ximage->bytes_per_line*ximage->height);
  if (ximage->data == (char *) NULL)
    {
      /*
        Unable to allocate pixel data.
      */
      XDestroyImage(ximage);
      XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  if (window->ximage != (XImage *) NULL)
    {
#ifdef HasSharedMemory
      if (window->shared_memory)
        {
          XShmDetach(display,&window->ximage_segment_info);
          XDestroyImage(window->ximage);
          shmdt(window->ximage_segment_info.shmaddr);
        }
#endif
      if (!window->shared_memory)
        XDestroyImage(window->ximage);
    }
#ifdef HasSharedMemory
  window->ximage_segment_info=segment_info;
#endif
  window->ximage=ximage;
  matte_image=(XImage *) NULL;
  if (transformed_image != (Image *) NULL)
    if (transformed_image->matte)
      {
        /*
          Create matte image.
        */
        matte_image=XCreateImage(display,window->visual,1,XYBitmap,0,
          (char *) NULL,width,height,XBitmapPad(display),0);
        if (matte_image != (XImage *) NULL)
          {
            /*
              Allocate matte image pixel data.
            */
            matte_image->data=(char *) malloc(matte_image->bytes_per_line*
              matte_image->height*matte_image->depth);
            if (matte_image->data == (char *) NULL)
              {
                XDestroyImage(matte_image);
                matte_image=(XImage *) NULL;
              }
          }
      }
  if (window->matte_image != (XImage *) NULL)
    XDestroyImage(window->matte_image);
  window->matte_image=matte_image;
  window->stasis=False;
  if (image == (Image *) NULL)
    {
      XDefineCursor(display,window->id,window->cursor);
      return(True);
    }
  /*
    Convert runlength-encoded pixels to X image data.
  */
  if ((ximage->byte_order == LSBFirst) ||
      ((ximage->format == XYBitmap) && (ximage->bitmap_bit_order == LSBFirst)))
    XMakeImageLSBFirst(resource_info,window,transformed_image,ximage,
      matte_image);
  else
    XMakeImageMSBFirst(resource_info,window,transformed_image,ximage,
      matte_image);
  if (window->matte_image != (XImage *) NULL)
    {
      /*
        Create matte pixmap.
      */
      if (window->matte_pixmap != (Pixmap) NULL)
        XFreePixmap(display,window->matte_pixmap);
      window->matte_pixmap=XCreatePixmap(display,window->id,width,height,1);
      if (window->matte_pixmap != (Pixmap) NULL)
        {
          GC
            graphics_context;

          XGCValues
            context_values;

          /*
            Copy matte image to matte pixmap.
          */
          context_values.background=1;
          context_values.foreground=0;
          graphics_context=XCreateGC(display,window->matte_pixmap,GCBackground |
            GCForeground,&context_values);
          XPutImage(display,window->matte_pixmap,graphics_context,
            window->matte_image,0,0,0,0,width,height);
          XFreeGC(display,graphics_context);
#ifdef HasShape
          XShapeCombineMask(display,window->id,ShapeBounding,0,0,
            window->matte_pixmap,ShapeSet);
          XWithdrawWindow(display,window->id,window->screen);
          XMapRaised(display,window->id);
#endif
        }
    }
  if (transformed_image != image)
    DestroyImage(transformed_image);
  /*
    Restore cursor.
  */
  XDefineCursor(display,window->id,window->cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e L S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImageLSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in least-significant bit and byte first
%  order.  The server's scanline pad is respected.  Rather than using one or
%  two general cases, many special cases are found here to help speed up the
%  image conversion.
%
%  The format of the XMakeImageLSBFirst routine is:
%
%      XMakeImageLSBFirst(resource_info,window,image,ximage,matte_image)
%
%  A description of each parameter follows:
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%    o matte_image: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
*/
static void XMakeImageLSBFirst(resource_info,window,image,ximage,matte_image)
XResourceInfo
  *resource_info;

XWindowInfo
  *window;

Image
  *image;

XImage
  *ximage,
  *matte_image;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    bit,
    byte,
    *q;

  register unsigned long
    pixel;

  unsigned int
    scanline_pad;

  unsigned long
    *pixels;

  XStandardColormap
    *map_info;

  scanline_pad=ximage->bytes_per_line-
    ((ximage->width*ximage->bits_per_pixel) >> 3);
  map_info=window->map_info;
  pixels=window->pixel_info->pixels;
  p=image->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned short
        polarity;

      unsigned char
        background,
        foreground;

      /*
        Convert image to big-endian bitmap.
      */
      background=(Intensity(window->pixel_info->foreground_color) <
        Intensity(window->pixel_info->background_color) ? 0x80 : 0x00);
      foreground=(Intensity(window->pixel_info->background_color) <
        Intensity(window->pixel_info->foreground_color) ? 0x80 : 0x00);
      polarity=Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte>>=1;
          if (p->index == polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == ximage->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q=byte >> (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
  else
    if (window->pixel_info->colors != 0)
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 2 bit color-mapped X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index] & 0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) (pixel << 6);
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit color-mapped X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index] & 0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit color-mapped X image.
          */
          if (resource_info->color_recovery && resource_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index];
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=(unsigned char) pixel;
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        default:
        {
          register int
            k;

          register unsigned int
            bytes_per_pixel;

          unsigned char
            channel[sizeof(unsigned long)];

          /*
            Convert to multi-byte color-mapped X image.
          */
          bytes_per_pixel=ximage->bits_per_pixel >> 3;
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index];
            for (k=0; k < bytes_per_pixel; k++)
            {
              channel[k]=(unsigned char) pixel;
              pixel>>=8;
            }
            for (j=0; j <= ((int) p->length); j++)
            {
              for (k=0; k < bytes_per_pixel; k++)
                *q++=channel[k];
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
      }
    else
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to contiguous 2 bit continuous-tone X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=XStandardPixel(map_info,*p,8);
            pixel&=0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) (pixel << 6);
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to contiguous 4 bit continuous-tone X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=XStandardPixel(map_info,*p,8);
            pixel&=0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to contiguous 8 bit continuous-tone X image.
          */
          if (resource_info->color_recovery && resource_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (i=0; i < image->packets; i++)
          {
            pixel=XStandardPixel(map_info,*p,8);
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=(unsigned char) pixel;
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        default:
        {
          if ((ximage->bits_per_pixel == 32) &&
              (map_info->red_max == 255) &&
              (map_info->green_max == 255) &&
              (map_info->blue_max == 255) &&
              (map_info->red_mult == 65536) &&
              (map_info->green_mult == 256) &&
              (map_info->blue_mult == 1))
            {
              /*
                Convert to 32 bit continuous-tone X image.
              */
              for (i=0; i < image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  *q++=p->blue;
                  *q++=p->green;
                  *q++=p->red;
                  *q++=0;
                }
                p++;
              }
            }
          else
            if ((ximage->bits_per_pixel == 32) &&
                (map_info->red_max == 255) &&
                (map_info->green_max == 255) &&
                (map_info->blue_max == 255) &&
                (map_info->red_mult == 1) &&
                (map_info->green_mult == 256) &&
                (map_info->blue_mult == 65536))
              {
                /*
                  Convert to 32 bit continuous-tone X image.
                */
                for (i=0; i < image->packets; i++)
                {
                  for (j=0; j <= ((int) p->length); j++)
                  {
                    *q++=p->red;
                    *q++=p->green;
                    *q++=p->blue;
                    *q++=0;
                  }
                  p++;
                }
              }
            else
              {
                register int
                  k;

                register unsigned int
                  bytes_per_pixel;

                unsigned char
                  channel[sizeof(unsigned long)];

                /*
                  Convert to multi-byte continuous-tone X image.
                */
                bytes_per_pixel=ximage->bits_per_pixel >> 3;
                for (i=0; i < image->packets; i++)
                {
                  pixel=XStandardPixel(map_info,*p,8);
                  for (k=0; k < bytes_per_pixel; k++)
                  {
                    channel[k]=(unsigned char) pixel;
                    pixel>>=8;
                  }
                  for (j=0; j <= ((int) p->length); j++)
                  {
                    for (k=0; k < bytes_per_pixel; k++)
                      *q++=channel[k];
                    x++;
                    if (x == ximage->width)
                      {
                        x=0;
                        q+=scanline_pad;
                      }
                  }
                  p++;
                }
              }
          break;
        }
      }
  if (matte_image != (XImage *) NULL)
    {
      /*
        Initialize matte image.
      */
      scanline_pad=matte_image->bytes_per_line-
        ((matte_image->width*matte_image->bits_per_pixel) >> 3);
      p=image->pixels;
      q=(unsigned char *) matte_image->data;
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte>>=1;
          if (p->index == Transparent)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == matte_image->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q=byte >> (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e M S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImageMSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in most-significant bit and byte first order.
%  The server's scanline pad is also resprected. Rather than using one or two
%  general cases, many special cases are found here to help speed up the image
%  conversion.
%
%  The format of the XMakeImageMSBFirst routine is:
%
%      XMakeImageMSBFirst(resource_info,window,image,ximage,matte_image)
%
%  A description of each parameter follows:
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%    o matte_image: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
static void XMakeImageMSBFirst(resource_info,window,image,ximage,matte_image)
XResourceInfo
  *resource_info;

XWindowInfo
  *window;

Image
  *image;

XImage
  *ximage,
  *matte_image;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    bit,
    byte,
    *q;

  register unsigned long
    pixel;

  unsigned int
    scanline_pad;

  unsigned long
    *pixels;

  XStandardColormap
    *map_info;

  scanline_pad=ximage->bytes_per_line-
    ((ximage->width*ximage->bits_per_pixel) >> 3);
  map_info=window->map_info;
  pixels=window->pixel_info->pixels;
  p=image->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned short
        polarity;

      unsigned char
        background,
        foreground;

      /*
        Convert image to big-endian bitmap.
      */
      background=(Intensity(window->pixel_info->foreground_color) <
        Intensity(window->pixel_info->background_color) ? 0x01 : 0x00);
      foreground=(Intensity(window->pixel_info->background_color) <
        Intensity(window->pixel_info->foreground_color) ? 0x01 : 0x00);
      polarity=Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte<<=1;
          if (p->index == polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == ximage->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q=byte << (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
  else
    if (window->pixel_info->colors != 0)
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 2 bit color-mapped X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index] & 0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 6);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit color-mapped X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index] & 0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit color-mapped X image.
          */
          if (resource_info->color_recovery && resource_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index];
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=(unsigned char) pixel;
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        default:
        {
          register int
            k;

          register unsigned int
            bytes_per_pixel;

          unsigned char
            channel[sizeof(unsigned long)];

          /*
            Convert to 8 bit color-mapped X image.
          */
          bytes_per_pixel=ximage->bits_per_pixel >> 3;
          for (i=0; i < image->packets; i++)
          {
            pixel=pixels[p->index];
            for (k=bytes_per_pixel-1; k >= 0; k--)
            {
              channel[k]=(unsigned char) pixel;
              pixel>>=8;
            }
            for (j=0; j <= ((int) p->length); j++)
            {
              for (k=0; k < bytes_per_pixel; k++)
                *q++=channel[k];
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
      }
    else
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit continuous-tone X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=XStandardPixel(map_info,*p,8);
            pixel&=0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 6);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit continuous-tone X image.
          */
          nibble=0;
          for (i=0; i < image->packets; i++)
          {
            pixel=XStandardPixel(map_info,*p,8);
            pixel&=0xf;
            for (j=0; j <= ((int) p->length); j++)
            {
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  nibble=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit continuous-tone X image.
          */
          if (resource_info->color_recovery && resource_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (i=0; i < image->packets; i++)
          {
            pixel=XStandardPixel(map_info,*p,8);
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=(unsigned char) pixel;
              x++;
              if (x == ximage->width)
                {
                  x=0;
                  q+=scanline_pad;
                }
            }
            p++;
          }
          break;
        }
        default:
        {
          if ((ximage->bits_per_pixel == 32) &&
              (map_info->red_max == 255) &&
              (map_info->green_max == 255) &&
              (map_info->blue_max == 255) &&
              (map_info->red_mult == 65536) &&
              (map_info->green_mult == 256) &&
              (map_info->blue_mult == 1))
            {
              /*
                Convert to 32 bit continuous-tone X image.
              */
              for (i=0; i < image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  *q++=0;
                  *q++=p->red;
                  *q++=p->green;
                  *q++=p->blue;
                }
                p++;
              }
            }
          else
            if ((ximage->bits_per_pixel == 32) &&
                (map_info->red_max == 255) &&
                (map_info->green_max == 255) &&
                (map_info->blue_max == 255) &&
                (map_info->red_mult == 1) &&
                (map_info->green_mult == 256) &&
                (map_info->blue_mult == 65536))
              {
                /*
                  Convert to 32 bit continuous-tone X image.
                */
                for (i=0; i < image->packets; i++)
                {
                  for (j=0; j <= ((int) p->length); j++)
                  {
                    *q++=0;
                    *q++=p->blue;
                    *q++=p->green;
                    *q++=p->red;
                  }
                  p++;
                }
              }
            else
              {
                register int
                  k;

                register unsigned int
                  bytes_per_pixel;

                unsigned char
                  channel[sizeof(unsigned long)];

                /*
                  Convert to multi-byte continuous-tone X image.
                */
                bytes_per_pixel=ximage->bits_per_pixel >> 3;
                for (i=0; i < image->packets; i++)
                {
                  pixel=XStandardPixel(map_info,*p,8);
                  for (k=bytes_per_pixel-1; k >= 0; k--)
                  {
                    channel[k]=(unsigned char) pixel;
                    pixel>>=8;
                  }
                  for (j=0; j <= ((int) p->length); j++)
                  {
                    for (k=0; k < bytes_per_pixel; k++)
                      *q++=channel[k];
                    x++;
                    if (x == ximage->width)
                      {
                        x=0;
                        q+=scanline_pad;
                      }
                  }
                  p++;
                }
              }
          break;
        }
      }
  if (matte_image != (XImage *) NULL)
    {
      /*
        Initialize matte image.
      */
      scanline_pad=matte_image->bytes_per_line-
        ((matte_image->width*matte_image->bits_per_pixel) >> 3);
      p=image->pixels;
      q=(unsigned char *) matte_image->data;
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte<<=1;
          if (p->index == Transparent)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == matte_image->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q=byte << (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e M a g n i f y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeMagnifyImage magnifies a region of an X image and displays it.
%
%  The format of the XMakeMagnifyImage routine is:
%
%      XMakeMagnifyImage(display,windows)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%
*/
void XMakeMagnifyImage(display,windows)
Display
  *display;

XWindows
  *windows;
{
  register int
    x,
    y;

  register unsigned char
    *p,
    *q;

  register unsigned int
    j,
    k,
    l;

  static char
    text[MaxTextLength];

  static unsigned int
    previous_magnify=0;

  static XWindowInfo
    magnify_window;

  unsigned int
    height,
    i,
    magnify,
    scanline_pad,
    width;

  XColor
    color;

  XImage
    *ximage;

  /*
    Check boundry conditions.
  */
  magnify=1;
  for (i=1; i < windows->magnify.data; i++)
    magnify<<=1;
  while ((magnify*windows->image.ximage->width) < windows->magnify.width)
    magnify<<=1;
  while ((magnify*windows->image.ximage->height) < windows->magnify.height)
    magnify<<=1;
  while (magnify > windows->magnify.width)
    magnify>>=1;
  while (magnify > windows->magnify.height)
    magnify>>=1;
  if (magnify != previous_magnify)
    {
      unsigned int
        status;

      XTextProperty
        window_name;

      /*
        New magnify factor:  update magnify window name.
      */
      i=0;
      while ((1 << i) <= magnify)
        i++;
      (void) sprintf(windows->magnify.name,"Magnify %uX",i);
      status=XStringListToTextProperty(&windows->magnify.name,1,&window_name);
      if (status != 0)
        XSetWMName(display,windows->magnify.id,&window_name);
    }
  previous_magnify=magnify;
  ximage=windows->image.ximage;
  width=windows->magnify.ximage->width;
  height=windows->magnify.ximage->height;
  x=windows->magnify.x-((width/magnify) >> 1);
  if (x < 0)
    x=0;
  else
    if (x > (ximage->width-(width/magnify)))
      x=ximage->width-width/magnify;
  y=windows->magnify.y-((height/magnify) >> 1);
  if (y < 0)
    y=0;
  else
    if (y > (ximage->height-(height/magnify)))
      y=ximage->height-height/magnify;
  q=(unsigned char *) windows->magnify.ximage->data;
  scanline_pad=windows->magnify.ximage->bytes_per_line-
    ((width*windows->magnify.ximage->bits_per_pixel) >> 3);
  if (ximage->bits_per_pixel < 8)
    {
      register unsigned char
        background,
        byte,
        foreground,
        p_bit,
        q_bit;

      register unsigned int
        plane;

      XPixelInfo
        *pixel_info;

      pixel_info=windows->magnify.pixel_info;
      switch (ximage->bitmap_bit_order)
      {
        case LSBFirst:
        {
          /*
            Magnify little-endian bitmap.
          */
          background=0x00;
          foreground=0x80;
          if (ximage->format == XYBitmap)
            {
              background=(Intensity(pixel_info->foreground_color) <
                Intensity(pixel_info->background_color) ? 0x80 : 0x00);
              foreground=(Intensity(pixel_info->background_color) <
                Intensity(pixel_info->foreground_color) ? 0x80 : 0x00);
            }
          for (i=0; i < height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; plane < ximage->bits_per_pixel; plane++)
                  {
                    byte>>=1;
                    if (*p & (0x01 << (p_bit+plane)))
                      byte|=foreground;
                    else
                      byte|=background;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte >> (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
        case MSBFirst:
        default:
        {
          /*
            Magnify big-endian bitmap.
          */
          background=0x00;
          foreground=0x01;
          if (ximage->format == XYBitmap)
            {
              background=(Intensity(pixel_info->foreground_color) <
                Intensity(pixel_info->background_color) ? 0x01 : 0x00);
              foreground=(Intensity(pixel_info->background_color) <
                Intensity(pixel_info->foreground_color) ? 0x01 : 0x00);
            }
          for (i=0; i < height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; plane < ximage->bits_per_pixel; plane++)
                  {
                    byte<<=1;
                    if (*p & (0x80 >> (p_bit+plane)))
                      byte|=foreground;
                    else
                      byte|=background;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte << (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
      }
    }
  else
    switch (ximage->bits_per_pixel)
    {
      case 6:
      case 8:
      {
        /*
          Magnify 8 bit X image.
        */
        for (i=0; i < height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                *q++=(*p);
              p++;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
      default:
      {
        register unsigned int
          bytes_per_pixel,
          m;

        /*
          Magnify multi-byte X image.
        */
        bytes_per_pixel=ximage->bits_per_pixel >> 3;
        for (i=0; i < height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                for (m=0; m < bytes_per_pixel; m++)
                  *q++=(*(p+m));
              p+=bytes_per_pixel;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
    }
  /*
    Copy X image to magnify pixmap.
  */
  x=windows->magnify.x-((width/magnify) >> 1);
  if (x < 0)
    x=(width >> 1)-windows->magnify.x*magnify;
  else
    if (x > (ximage->width-(width/magnify)))
      x=(ximage->width-windows->magnify.x)*magnify-(width >> 1);
    else
      x=0;
  y=windows->magnify.y-((height/magnify) >> 1);
  if (y < 0)
    y=(height >> 1)-windows->magnify.y*magnify;
  else
    if (y > (ximage->height-(height/magnify)))
      y=(ximage->height-windows->magnify.y)*magnify-(height >> 1);
    else
      y=0;
  if ((x != 0) || (y != 0))
    XFillRectangle(display,windows->magnify.pixmap,
      windows->magnify.annotate_context,0,0,width,height);
  XPutImage(display,windows->magnify.pixmap,windows->magnify.annotate_context,
    windows->magnify.ximage,0,0,x,y,width-x,height-y);
  if ((magnify > 1) && ((magnify <= (width >> 1)) &&
      (magnify <= (height >> 1))))
    {
      RectangleInfo
        highlight_info;

      /*
        Highlight center pixel.
      */
      highlight_info.x=windows->magnify.width >> 1;
      highlight_info.y=windows->magnify.height >> 1;
      highlight_info.width=magnify;
      highlight_info.height=magnify;
      XDrawRectangle(display,windows->magnify.pixmap,
        windows->magnify.highlight_context,highlight_info.x,highlight_info.y,
        highlight_info.width-1,highlight_info.height-1);
      if (magnify > 2)
        XDrawRectangle(display,windows->magnify.pixmap,
          windows->magnify.annotate_context,highlight_info.x+1,
          highlight_info.y+1,highlight_info.width-3,highlight_info.height-3);
    }
  /*
    Show center pixel color.
  */
  color.pixel=
    XGetPixel(windows->image.ximage,windows->magnify.x,windows->magnify.y);
  XQueryColor(display,windows->image.map_info->colormap,&color);
  if (windows->magnify.depth > 12)
    (void) sprintf(text,"%+d%+d  (%3u,%3u,%3u)",windows->magnify.x,
      windows->magnify.y,ColorShift(color.red),ColorShift(color.green),
      ColorShift(color.blue));
  else
    (void) sprintf(text,"%+d%+d  (%3u,%3u,%3u)  %lu ",windows->magnify.x,
      windows->magnify.y,ColorShift(color.red),ColorShift(color.green),
      ColorShift(color.blue),color.pixel);
  height=windows->magnify.font_info->ascent+windows->magnify.font_info->descent;
  x=(3*windows->magnify.font_info->max_bounds.width) >> 2;
  y=windows->magnify.font_info->ascent+(height >> 2);
  XDrawImageString(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,x,y,text,strlen(text));
  /*
    Refresh magnify window.
  */
  magnify_window=windows->magnify;
  magnify_window.x=0;
  magnify_window.y=0;
  XRefreshWindow(display,&magnify_window,(XEvent *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e P i x m a p                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakePixmap creates an X11 pixmap.
%
%  The format of the XMakePixmap routine is:
%
%      status=XMakePixmap(display,resource_info,window)
%
%  A description of each parameter follows:
%
%    o status: Function XMakePixmap returns True if the X pixmap is
%      successfully created.  False is returned is there is a memory shortage.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
unsigned int XMakePixmap(display,resource_info,window)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *window;
{
  unsigned int
    height,
    width;

  if (window->ximage == (XImage *) NULL)
    return(False);
  /*
    Display busy cursor.
  */
  XDefineCursor(display,window->id,window->busy_cursor);
  XFlush(display);
  /*
    Create pixmap.
  */
  if (window->pixmap != (Pixmap) NULL)
    {
      XFreePixmap(display,window->pixmap);
      window->pixmap=(Pixmap) NULL;
#ifdef HasSharedMemory
      if (window->shared_memory)
        if (window->pixmap_segment_info.shmid >= 0)
          {
            XShmDetach(display, &window->pixmap_segment_info);
            XSync(display,False);
            shmdt(window->pixmap_segment_info.shmaddr);
            window->pixmap_segment_info.shmid=(-1);
          }
#endif
    }
  width=window->ximage->width;
  height=window->ximage->height;
#ifdef HasSharedMemory
  if (window->shared_memory)
    {
      XShmSegmentInfo
        segment_info;

      segment_info.shmid=(int) shmget(IPC_PRIVATE,
        window->ximage->bytes_per_line*height,IPC_CREAT | 0777);
      if (segment_info.shmid >= 0)
        {
          segment_info.shmaddr=(char *) shmat(segment_info.shmid,0,0);
          segment_info.readOnly=False;
          if (XShmAttach(display,&segment_info))
            window->pixmap=XShmCreatePixmap(display,window->id,
              segment_info.shmaddr,&segment_info,width,height,window->depth);
          if (window->pixmap != (Pixmap) NULL)
            window->pixmap_segment_info=segment_info;
          else
            {
              shmdt(segment_info.shmaddr);
              XShmDetach(display,&segment_info);
            }
          XSync(display,False);
          shmctl(segment_info.shmid,IPC_RMID,0);
        }
    }
#endif
  if (window->pixmap == (Pixmap) NULL)
    window->pixmap=XCreatePixmap(display,window->id,width,height,window->depth);
  if (window->pixmap == (Pixmap) NULL)
    {
      /*
        Unable to allocate pixmap.
      */
      XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  /*
    Copy X image to pixmap.
  */
#ifdef HasSharedMemory
  if (window->shared_memory)
    XShmPutImage(display,window->pixmap,window->annotate_context,window->ximage,
      0,0,0,0,width,height,True);
#endif
  if (!window->shared_memory)
    XPutImage(display,window->pixmap,window->annotate_context,window->ximage,
      0,0,0,0,width,height);
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"Pixmap:\n");
      (void) fprintf(stderr,"  width, height: %dx%d\n",width,height);
    }
  /*
    Restore cursor.
  */
  XDefineCursor(display,window->id,window->cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeStandardColormap creates an X11 Standard Colormap.
%
%  The format of the XMakeStandardColormap routine is:
%
%      XMakeStandardColormap(display,visual_info,resource_info,image,
%        map_info,pixel_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o map_info: If a Standard Colormap type is specified, this structure is
%      initialized with info from the Standard Colormap.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%
*/
static int IntensityCompare(x,y)
const void
  *x,
  *y;
{
  DiversityPacket
    *color_1,
    *color_2;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) Intensity(*color_2)-(int) Intensity(*color_1));
}

static int PopularityCompare(x,y)
const void
  *x,
  *y;
{
  DiversityPacket
    *color_1,
    *color_2;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) color_2->count-(int) color_1->count);
}

void XMakeStandardColormap(display,visual_info,resource_info,image,map_info,
  pixel_info)
Display
  *display;

XVisualInfo
  *visual_info;

XResourceInfo
  *resource_info;

Image
  *image;

XStandardColormap
  *map_info;

XPixelInfo
  *pixel_info;
{
  Colormap
    colormap;

  int
    status;

  register int
    i;

  unsigned int
    gray_value,
    number_colors,
    retain_colors;

  XColor
    color,
    *colors,
    *p;

  if (resource_info->map_type != (char *) NULL)
    {
      /*
        Standard Colormap is already defined (i.e. xstdcmap).
      */
      if (pixel_info->pixels != (unsigned long *) NULL)
        (void) free((char *) pixel_info->pixels);
      number_colors=(unsigned int) ((map_info->red_max*map_info->red_mult)+
        (map_info->green_max*map_info->green_mult)+
        (map_info->blue_max*map_info->blue_mult)+1);
      if (image->matte || !resource_info->dither ||
          resource_info->color_recovery || (number_colors > MaxColormapSize))
        XGetPixelInfo(display,visual_info,map_info,resource_info,image,
          pixel_info);
      else
        {
          Image
            *map_image;

          register RunlengthPacket
            *p;

          /*
            Improve image appearance with error diffusion.
          */
          map_image=AllocateImage((ImageInfo *) NULL);
          if (map_image == (Image *) NULL)
            Error("Unable to dither image","Memory allocation failed");
          map_image->columns=number_colors;
          map_image->rows=1;
          map_image->packets=map_image->columns*map_image->rows;
          map_image->pixels=(RunlengthPacket *)
            malloc(map_image->packets*sizeof(RunlengthPacket));
          if (map_image->pixels == (RunlengthPacket *) NULL)
            Error("Unable to dither image","Memory allocation failed");
          /*
            Initialize colormap image.
          */
          p=map_image->pixels;
          for (i=0; i < number_colors; i++)
          {
            p->red=0;
            if (map_info->red_max != 0)
              p->red=(unsigned char)
                (((i/map_info->red_mult)*MaxRGB)/map_info->red_max);
            p->green=0;
            if (map_info->green_max != 0)
              p->green=(unsigned char) ((((i/map_info->green_mult) %
                (map_info->green_max+1))*MaxRGB)/map_info->green_max);
            p->blue=0;
            if (map_info->blue_max != 0)
              p->blue=(unsigned char)
                (((i % map_info->green_mult)*MaxRGB)/map_info->blue_max);
            p->index=0;
            p->length=0;
            p++;
          }
          MapImage(image,map_image,True);
          XGetPixelInfo(display,visual_info,map_info,resource_info,image,
            pixel_info);
          image->class=DirectClass;
          DestroyImage(map_image);
        }
      if (resource_info->debug)
        {
          (void) fprintf(stderr,"Standard Colormap:\n");
          (void) fprintf(stderr,"  colormap id: 0x%lx\n",map_info->colormap);
          (void) fprintf(stderr,"  red, green, blue max: %lu %lu %lu\n",
            map_info->red_max,map_info->green_max,map_info->blue_max);
          (void) fprintf(stderr,"  red, green, blue mult: %lu %lu %lu\n",
            map_info->red_mult,map_info->green_mult,map_info->blue_mult);
        }
      return;
    }
  if ((visual_info->class != DirectColor) && (visual_info->class != TrueColor))
    if ((image->class == DirectClass) ||
        (image->colors > visual_info->colormap_size))
      {
        /*
          Image has more colors than the visual supports.
        */
        QuantizeImage(image,(unsigned int) visual_info->colormap_size,
          resource_info->tree_depth,resource_info->dither,
          resource_info->colorspace);
        image->class=DirectClass;  /* promote to DirectClass */
      }
  /*
    Free previous and create new colormap.
  */
  XFreeStandardColormap(display,visual_info,map_info,pixel_info);
  colormap=XDefaultColormap(display,visual_info->screen);
  if (visual_info->visual != XDefaultVisual(display,visual_info->screen))
    colormap=XCreateColormap(display,XRootWindow(display,visual_info->screen),
      visual_info->visual,visual_info->class == DirectColor ?
      AllocAll : AllocNone);
  if (colormap == (Colormap) NULL)
    Error("Unable to create colormap",(char *) NULL);
  /*
    Initialize the map and pixel info structures.
  */
  XGetMapInfo(visual_info,colormap,map_info);
  XGetPixelInfo(display,visual_info,map_info,resource_info,image,pixel_info);
  /*
    Allocating colors in server colormap is based on visual class.
  */
  switch (visual_info->class)
  {
    case StaticGray:
    case StaticColor:
    {
      /*
        Define Standard Colormap for StaticGray or StaticColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *) malloc(visual_info->colormap_size*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("Unable to create colormap","Memory allocation failed");
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (visual_info->class == StaticColor)
        for (i=0; i < image->colors; i++)
        {
          color.red=(unsigned short) (image->colormap[i].red << 8);
          color.green=(unsigned short) (image->colormap[i].green << 8);
          color.blue=(unsigned short) (image->colormap[i].blue << 8);
          status=XAllocColor(display,colormap,&color);
          if (status == 0)
            {
              colormap=XCopyColormapAndFree(display,colormap);
              XAllocColor(display,colormap,&color);
            }
          pixel_info->pixels[i]=color.pixel;
          *p++=color;
        }
      else
        for (i=0; i < image->colors; i++)
        {
          gray_value=Intensity(image->colormap[i]);
          color.red=(unsigned short) (gray_value << 8);
          color.green=(unsigned short) (gray_value << 8);
          color.blue=(unsigned short) (gray_value << 8);
          status=XAllocColor(display,colormap,&color);
          if (status == 0)
            {
              colormap=XCopyColormapAndFree(display,colormap);
              XAllocColor(display,colormap,&color);
            }
          pixel_info->pixels[i]=color.pixel;
          *p++=color;
        }
      break;
    }
    case GrayScale:
    case PseudoColor:
    {
      unsigned int
        colormap_type;

      /*
        Define Standard Colormap for GrayScale or PseudoColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *) malloc(visual_info->colormap_size*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("Unable to create colormap","Memory allocation failed");
      /*
        Preallocate our GUI colors.
      */
      (void) XAllocColor(display,colormap,&pixel_info->foreground_color);
      (void) XAllocColor(display,colormap,&pixel_info->background_color);
      (void) XAllocColor(display,colormap,&pixel_info->border_color);
      (void) XAllocColor(display,colormap,&pixel_info->matte_color);
      (void) XAllocColor(display,colormap,&pixel_info->highlight_color);
      (void) XAllocColor(display,colormap,&pixel_info->shadow_color);
      (void) XAllocColor(display,colormap,&pixel_info->depth_color);
      (void) XAllocColor(display,colormap,&pixel_info->trough_color);
      for (i=0; i < MaxNumberPens; i++)
        (void) XAllocColor(display,colormap,&pixel_info->pen_color[i]);
      /*
        Determine if image colors will "fit" into X server colormap.
      */
      colormap_type=resource_info->colormap;
      status=XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
        pixel_info->pixels,image->colors);
      if (status != 0)
        colormap_type=PrivateColormap;
      if (colormap_type == SharedColormap)
        {
          DiversityPacket
            *diversity;

          register RunlengthPacket
            *q;

          unsigned short
            index;

          /*
            Define Standard colormap for shared GrayScale or PseudoColor visual:
          */
          diversity=(DiversityPacket *)
            malloc(image->colors*sizeof(DiversityPacket));
          if (diversity == (DiversityPacket *) NULL)
            Error("Unable to create colormap","Memory allocation failed");
          for (i=0; i < image->colors; i++)
          {
            diversity[i].red=image->colormap[i].red;
            diversity[i].green=image->colormap[i].green;
            diversity[i].blue=image->colormap[i].blue;
            diversity[i].index=(unsigned short) i;
            diversity[i].count=0;
          }
          q=image->pixels;
          for (i=0; i < image->packets; i++)
          {
            diversity[q->index].count+=(q->length+1);
            q++;
          }
          /*
            Sort colors by decreasing intensity.
          */
          (void) qsort((void *) diversity,image->colors,sizeof(DiversityPacket),
            (int (*) _Declare((const void *, const void *))) IntensityCompare);
          for (i=0; i < image->colors; i+=Max(image->colors >> 4,2))
            diversity[i].count<<=4;  /* increase this colors popularity */
          diversity[image->colors-1].count<<=4;
          (void) qsort((void *) diversity,image->colors,sizeof(DiversityPacket),
            (int (*) _Declare((const void *, const void *))) PopularityCompare);
          /*
            Allocate colors.
          */
          p=colors;
          color.flags=DoRed | DoGreen | DoBlue;
          if (visual_info->class == PseudoColor)
            for (i=0; i < image->colors; i++)
            {
              index=diversity[i].index;
              color.red=(unsigned short) (image->colormap[index].red << 8);
              color.green=(unsigned short) (image->colormap[index].green << 8);
              color.blue=(unsigned short) (image->colormap[index].blue << 8);
              status=XAllocColor(display,colormap,&color);
              if (status == 0)
                break;
              pixel_info->pixels[index]=color.pixel;
              *p++=color;
            }
          else
            for (i=0; i < image->colors; i++)
            {
              index=diversity[i].index;
              gray_value=Intensity(image->colormap[index]);
              color.red=(unsigned short) (gray_value << 8);
              color.green=(unsigned short) (gray_value << 8);
              color.blue=(unsigned short) (gray_value << 8);
              status=XAllocColor(display,colormap,&color);
              if (status == 0)
                break;
              pixel_info->pixels[index]=color.pixel;
              *p++=color;
            }
          if (i < image->colors)
            {
              register int
                j;

              XColor
                *server_colors;

              /*
                Read X server colormap.
              */
              server_colors=(XColor *)
                malloc(visual_info->colormap_size*sizeof(XColor));
              if (server_colors == (XColor *) NULL)
                Error("Unable to create colormap","Memory allocation failed");
              for (j=0; j < visual_info->colormap_size; j++)
                server_colors[j].pixel=(unsigned long) j;
              XQueryColors(display,colormap,server_colors,
                (int) Min(visual_info->colormap_size,256));
              /*
                Select remaining colors from X server colormap.
              */
              if (visual_info->class == PseudoColor)
                for (; i < image->colors; i++)
                {
                  index=diversity[i].index;
                  color.red=(unsigned short) (image->colormap[index].red << 8);
                  color.green=(unsigned short)
                    (image->colormap[index].green << 8);
                  color.blue=(unsigned short)
                    (image->colormap[index].blue << 8);
                  XBestPixel(display,colormap,server_colors,
                    (unsigned int) visual_info->colormap_size,&color);
                  pixel_info->pixels[index]=color.pixel;
                  *p++=color;
                }
              else
                for (; i < image->colors; i++)
                {
                  index=diversity[i].index;
                  gray_value=Intensity(image->colormap[index]);
                  color.red=(unsigned short) (gray_value << 8);
                  color.green=(unsigned short) (gray_value << 8);
                  color.blue=(unsigned short) (gray_value << 8);
                  XBestPixel(display,colormap,server_colors,
                    (unsigned int) visual_info->colormap_size,&color);
                  pixel_info->pixels[index]=color.pixel;
                  *p++=color;
                }
              if (image->colors < visual_info->colormap_size)
                {
                  /*
                    Fill up colors array-- more choices for pen colors.
                  */
                  retain_colors=
                    Min(visual_info->colormap_size-image->colors,256);
                  for (i=0; i < retain_colors; i++)
                    *p++=server_colors[i];
                  number_colors+=retain_colors;
                }
              (void) free((char *) server_colors);
            }
          (void) free((char *) diversity);
          break;
        }
      /*
        Define Standard colormap for private GrayScale or PseudoColor visual.
      */
      if (status == 0)
        {
          /*
            Not enough colormap entries in the colormap-- Create a new colormap.
          */
          colormap=XCreateColormap(display,
            XRootWindow(display,visual_info->screen),visual_info->visual,
            AllocNone);
          if (colormap == (Colormap) NULL)
            Error("Unable to create colormap",(char *) NULL);
          map_info->colormap=colormap;
          if (image->colors < visual_info->colormap_size)
            {
              /*
                Retain colors from the default colormap to help lessens the
                effects of colormap flashing.
              */
              retain_colors=Min(visual_info->colormap_size-image->colors,256);
              p=colors+image->colors;
              for (i=0; i < retain_colors; i++)
              {
                p->pixel=(unsigned long) i;
                p++;
              }
              XQueryColors(display,
                XDefaultColormap(display,visual_info->screen),
                colors+image->colors,(int) retain_colors);
              /*
                Transfer colors from default to private colormap.
              */
              XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
                pixel_info->pixels,retain_colors);
              p=colors+image->colors;
              for (i=0; i < retain_colors; i++)
              {
                p->pixel=pixel_info->pixels[i];
                p++;
              }
              XStoreColors(display,colormap,colors+image->colors,retain_colors);
              number_colors+=retain_colors;
            }
          XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
            pixel_info->pixels,image->colors);
        }
      /*
        Store the image colormap.
      */
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (visual_info->class == PseudoColor)
        for (i=0; i < image->colors; i++)
        {
          color.red=(unsigned short) (image->colormap[i].red << 8);
          color.green=(unsigned short) (image->colormap[i].green << 8);
          color.blue=(unsigned short) (image->colormap[i].blue << 8);
          color.pixel=pixel_info->pixels[i];
          *p++=color;
        }
      else
        for (i=0; i < image->colors; i++)
        {
          gray_value=Intensity(image->colormap[i]);
          color.red=(unsigned short) (gray_value << 8);
          color.green=(unsigned short) (gray_value << 8);
          color.blue=(unsigned short) (gray_value << 8);
          color.pixel=pixel_info->pixels[i];
          *p++=color;
        }
      XStoreColors(display,colormap,colors,image->colors);
      break;
    }
    case TrueColor:
    case DirectColor:
    default:
    {
      unsigned int
        linear_colormap;

      /*
        Define Standard Colormap for TrueColor or DirectColor visual.
      */
      number_colors=(unsigned int) ((map_info->red_max*map_info->red_mult)+
        (map_info->green_max*map_info->green_mult)+
        (map_info->blue_max*map_info->blue_mult)+1);
      linear_colormap=(number_colors > 4096) ||
        (((map_info->red_max+1) == visual_info->colormap_size) &&
         ((map_info->green_max+1) == visual_info->colormap_size) &&
         ((map_info->blue_max+1) == visual_info->colormap_size));
      if (linear_colormap)
        number_colors=visual_info->colormap_size;
      /*
        Allocate color array.
      */
      colors=(XColor *) malloc(number_colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("Unable to create colormap","Memory allocation failed");
      /*
        Initialize linear color ramp.
      */
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (linear_colormap)
        for (i=0; i < number_colors; i++)
        {
          color.blue=(unsigned short) 0;
          if (map_info->blue_max != 0)
            color.blue=(unsigned short)
              (((i % map_info->green_mult)*65535)/map_info->blue_max);
          color.green=color.blue;
          color.red=color.blue;
          color.pixel=XStandardPixel(map_info,color,16);
          *p++=color;
        }
      else
        for (i=0; i < number_colors; i++)
        {
          color.red=(unsigned short) 0;
          if (map_info->red_max != 0)
            color.red=(unsigned short)
              (((i/map_info->red_mult)*65535)/map_info->red_max);
          color.green=(unsigned short) 0;
          if (map_info->green_max != 0)
            color.green=(unsigned short) ((((i/map_info->green_mult) %
              (map_info->green_max+1))*65535)/map_info->green_max);
          color.blue=(unsigned short) 0;
          if (map_info->blue_max != 0)
            color.blue=(unsigned short)
              (((i % map_info->green_mult)*65535)/map_info->blue_max);
          color.pixel=XStandardPixel(map_info,color,16);
          *p++=color;
        }
      if ((visual_info->class == DirectColor) &&
          (colormap != XDefaultColormap(display,visual_info->screen)))
        XStoreColors(display,colormap,colors,number_colors);
      else
        for (i=0; i < number_colors; i++)
          XAllocColor(display,colormap,&colors[i]);
      break;
    }
  }
  if ((visual_info->class != DirectColor) && (visual_info->class != TrueColor))
    {
      /*
        Set foreground, background, border, etc. pixels.
      */
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->foreground_color);
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->background_color);
      if (pixel_info->background_color.pixel ==
          pixel_info->foreground_color.pixel)
        {
          /*
            Foreground and background colors must differ.
          */
          pixel_info->background_color.red=(~pixel_info->foreground_color.red);
          pixel_info->background_color.green=
            (~pixel_info->foreground_color.green);
          pixel_info->background_color.blue=
            (~pixel_info->foreground_color.blue);
          XBestPixel(display,colormap,colors,number_colors,
            &pixel_info->background_color);
        }
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->border_color);
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->matte_color);
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->highlight_color);
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->shadow_color);
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->depth_color);
      XBestPixel(display,colormap,colors,number_colors,
        &pixel_info->trough_color);
      for (i=0; i < MaxNumberPens; i++)
      {
        XBestPixel(display,colormap,colors,number_colors,
          &pixel_info->pen_color[i]);
        pixel_info->pixels[image->colors+i]=pixel_info->pen_color[i].pixel;
      }
      pixel_info->colors=image->colors+MaxNumberPens;
    }
  (void) free((char *) colors);
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"Standard Colormap:\n");
      (void) fprintf(stderr,"  colormap id: 0x%lx\n",map_info->colormap);
      (void) fprintf(stderr,"  red, green, blue max: %lu %lu %lu\n",
        map_info->red_max,map_info->green_max,map_info->blue_max);
      (void) fprintf(stderr,"  red, green, blue mult: %lu %lu %lu\n",
        map_info->red_mult,map_info->green_mult,map_info->blue_mult);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e W i n d o w                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeWindow creates an X11 window.
%
%  The format of the XMakeWindow routine is:
%
%      XMakeWindow(display,parent,argv,argc,class_hint,manager_hints,
%        window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o parent: Specifies the parent window_info.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o class_hint: Specifies a pointer to a X11 XClassHint structure.
%
%    o manager_hints: Specifies a pointer to a X11 XWMHints structure.
%
%    o window_info: Specifies a pointer to a X11 XWindowInfo structure.
%
%
*/
void XMakeWindow(display,parent,argv,argc,class_hint,manager_hints,window_info)
Display
  *display;

Window
  parent;

char
  **argv;

int
  argc;

XClassHint
  *class_hint;

XWMHints
  *manager_hints;

XWindowInfo
  *window_info;
{
#define MinWindowSize  64

  Atom
    atom_list[2];

  int
    gravity,
    status;

  XSizeHints
    *size_hints;

  XTextProperty
    icon_name,
    window_name;

  /*
    Set window_info hints.
  */
  size_hints=XAllocSizeHints();
  if (size_hints == (XSizeHints *) NULL)
    Error("Unable to make X window","Memory allocation failed");
  size_hints->flags=window_info->flags;
  size_hints->x=window_info->x;
  size_hints->y=window_info->y;
  size_hints->width=window_info->width;
  size_hints->height=window_info->height;
  size_hints->flags|=PMinSize;
  if (window_info->immutable)
    {
      /*
        Window size cannot be changed.
      */
      size_hints->min_width=size_hints->width;
      size_hints->min_height=size_hints->height;
      size_hints->max_width=size_hints->width;
      size_hints->max_height=size_hints->height;
      size_hints->flags|=PMaxSize;
    }
  else
    {
      /*
        Window size can be changed.
      */
      size_hints->min_width=window_info->min_width;
      size_hints->min_height=window_info->min_height;
      size_hints->flags|=PResizeInc;
      size_hints->width_inc=window_info->width_inc;
      size_hints->height_inc=window_info->height_inc;
#ifndef PRE_R4_ICCCM
      size_hints->flags|=PBaseSize;
      size_hints->base_width=size_hints->width_inc;
      size_hints->base_height=size_hints->height_inc;
#endif
    }
  gravity=NorthWestGravity;
  if (window_info->geometry != (char *) NULL)
    {
      char
        default_geometry[MaxTextLength],
        geometry[MaxTextLength];

      int
        flags;

      register char
        *p;

      /*
        User specified geometry.
      */
      (void) sprintf(default_geometry,"%dx%d",size_hints->width,
        size_hints->height);
      (void) strcpy(geometry,window_info->geometry);
      p=geometry;
      while ((int) strlen(p) > 0)
      {
        if (!isspace(*p) && (*p != '%'))
          p++;
        else
          (void) strcpy(p,p+1);
      }
      flags=XWMGeometry(display,window_info->screen,geometry,default_geometry,
        window_info->border_width,size_hints,&size_hints->x,&size_hints->y,
        &size_hints->width,&size_hints->height,&gravity);
      if ((flags & WidthValue) && (flags & HeightValue))
        size_hints->flags|=USSize;
      if ((flags & XValue) && (flags & YValue))
        {
          size_hints->flags|=USPosition;
          window_info->x=size_hints->x;
          window_info->y=size_hints->y;
        }
    }
#ifndef PRE_R4_ICCCM
  size_hints->win_gravity=gravity;
  size_hints->flags|=PWinGravity;
#endif
  if (window_info->id == (Window) NULL)
    window_info->id=XCreateWindow(display,parent,window_info->x,window_info->y,
      window_info->width,window_info->height,window_info->border_width,
      window_info->depth,InputOutput,window_info->visual,window_info->mask,
      &window_info->attributes);
  else
    {
      unsigned int
        mask;

      XEvent
        sans_event;

      XWindowChanges
        window_changes;

      /*
        Window already exists;  change relevant attributes.
      */
      XChangeWindowAttributes(display,window_info->id,window_info->mask,
        &window_info->attributes);
      mask=ConfigureNotify;
      while (XCheckTypedWindowEvent(display,window_info->id,mask,&sans_event));
      window_changes.x=window_info->x;
      window_changes.y=window_info->y;
      window_changes.width=window_info->width;
      window_changes.height=window_info->height;
      mask=CWWidth | CWHeight;
      if (window_info->flags & USPosition)
        mask|=CWX | CWY;
      XReconfigureWMWindow(display,window_info->id,window_info->screen,mask,
        &window_changes);
    }
  if (window_info->id == (Window) NULL)
    Error("Unable to create window",window_info->name);
  status=XStringListToTextProperty(&window_info->name,1,&window_name);
  if (status == 0)
    Error("Unable to create text property",window_info->name);
  status=XStringListToTextProperty(&window_info->icon_name,1,&icon_name);
  if (status == 0)
    Error("Unable to create text property",window_info->icon_name);
  if (window_info->icon_geometry != (char *) NULL)
    {
      int
        flags,
        gravity,
        height,
        width;

      /*
        User specified icon geometry.
      */
      size_hints->flags|=USPosition;
      flags=XWMGeometry(display,window_info->screen,window_info->icon_geometry,
        (char *) NULL,0,size_hints,&manager_hints->icon_x,
        &manager_hints->icon_y,&width,&height,&gravity);
      if ((flags & XValue) && (flags & YValue))
        manager_hints->flags|=IconPositionHint;
    }
  XSetWMProperties(display,window_info->id,&window_name,&icon_name,argv,argc,
    size_hints,manager_hints,class_hint);
  atom_list[0]=XInternAtom(display,"WM_DELETE_WINDOW",False);
  atom_list[1]=XInternAtom(display,"WM_TAKE_FOCUS",False);
  XSetWMProtocols(display,window_info->id,atom_list,2);
  XFree((void *) size_hints);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M o n t a g e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMontageImage creates a composite image by combining several
%  separate images.
%
%  The format of the XMontageImage routine is:
%
%      XMontageImage(display,resource_info,montage_info,filename,images)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%    o filename: Specifies the name of the montage image.
%
%    o image: Specifies a pointer to an array of Image structures.
%
%
*/

static int SceneCompare(x,y)
const void
  *x,
  *y;
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) (*image_1)->scene-(int) (*image_2)->scene);
}

Image *XMontageImage(display,resource_info,montage_info,filename,images)
Display
  *display;

XResourceInfo
  *resource_info;

XMontageInfo
  *montage_info;

char
  *filename;

Image
  **images;
{
  ColorPacket
    border_color,
    highlight_color,
    matte_color,
    shadow_color;

  Image
    *image,
    *montage_image;

  int
    x,
    x_offset,
    y,
    y_offset;

  register char
    *q;

  register int
    i;

  register RunlengthPacket
    *p;

  RectangleInfo
    label_info,
    tile_info;

  unsigned int
    border_width,
    bevel_width,
    count,
    max_width,
    number_images,
    number_tiles,
    status,
    tile,
    tiles_per_column,
    tiles_per_row;

  XAnnotateInfo
    annotate_info;

  XColor
    background_color;

  XFontStruct
    *font_info;

  XPixelInfo
    pixel_info;

  XStandardColormap
    *map_info;

  XVisualInfo
    *visual_info;

  /*
    Sort images by increasing tile number.
  */
  number_tiles=montage_info->number_tiles;
  count=0;
  for (tile=0; tile < number_tiles; tile++)
    count+=images[tile]->scene;
  if (count != 0)
    (void) qsort((void *) images,number_tiles,sizeof(Image *),
      (int (*) _Declare((const void *, const void *))) SceneCompare);
  /*
    Determine tiles per row and column.
  */
  tiles_per_row=1;
  tiles_per_column=1;
  while ((tiles_per_row*tiles_per_column) < number_tiles)
  {
    tiles_per_row++;
    tiles_per_column++;
  }
  if (montage_info->tile != (char *) NULL)
    {
      tiles_per_column=montage_info->number_tiles;
      XParseGeometry(montage_info->tile,&x,&y,&tiles_per_row,&tiles_per_column);
    }
  /*
    Determine tile sizes.
  */
  border_width=0;
  if (display != (Display *) NULL)
    {
      border_width=resource_info->border_width;
      if (montage_info->frame)
        {
          bevel_width=(border_width >> 2)+1;
          border_width+=bevel_width << 1;
        }
    }
  tile_info.x=resource_info->border_width;
  tile_info.y=resource_info->border_width;
  if (resource_info->image_geometry != (char *) NULL)
    {
      XParseGeometry(resource_info->image_geometry,&tile_info.x,&tile_info.y,
        &tile_info.width,&tile_info.height);
      if (tile_info.x < 0)
        tile_info.x=0;
      if (tile_info.y < 0)
        tile_info.y=0;
    }
  tile_info.width=images[0]->columns;
  tile_info.height=images[0]->rows;
  for (tile=1; tile < montage_info->number_tiles; tile++)
  {
    if (images[tile]->columns > tile_info.width)
      tile_info.width=images[tile]->columns;
    if (images[tile]->rows > tile_info.height)
      tile_info.height=images[tile]->rows;
  }
  /*
    Initialize tile colors.
  */
  background_color.red=0;
  background_color.green=0;
  background_color.blue=0;
  border_color.red=0;
  border_color.green=0;
  border_color.blue=0;
  highlight_color=border_color;
  shadow_color=border_color;
  XGetAnnotateInfo(&annotate_info);
  if (display != (Display *) NULL)
    {
      /*
        Initialize visual info.
      */
      map_info=XAllocStandardColormap();
      if (map_info == (XStandardColormap *) NULL)
        {
          Warning("Unable to create standard colormap",
            "Memory allocation failed");
          return((Image *) NULL);
        }
      visual_info=XBestVisualInfo(display,map_info,resource_info);
      map_info->colormap=(Colormap) NULL;
      pixel_info.pixels=(unsigned long *) NULL;
      /*
        Initialize font info.
      */
      font_info=XBestFont(display,resource_info,False);
      annotate_info.text=(char *) malloc(MaxTextLength*sizeof(char));
      if ((visual_info == (XVisualInfo *) NULL) ||
          (font_info == (XFontStruct *) NULL) ||
          (annotate_info.text == (char *) NULL))
        {
          Warning("Unable to montage images","Memory allocation failed");
          return((Image *) NULL);
        }
      annotate_info.font_info=font_info;
      annotate_info.height=font_info->ascent+font_info->descent;
      /*
        Determine background and border colors.
      */
      XGetMapInfo(visual_info,XDefaultColormap(display,visual_info->screen),
        map_info);
      XGetPixelInfo(display,visual_info,map_info,resource_info,(Image *) NULL,
        &pixel_info);
      background_color=pixel_info.background_color;
      border_color.red=ColorShift(pixel_info.border_color.red);
      border_color.green=ColorShift(pixel_info.border_color.green);
      border_color.blue=ColorShift(pixel_info.border_color.blue);
      matte_color.red=ColorShift(pixel_info.matte_color.red);
      matte_color.green=ColorShift(pixel_info.matte_color.green);
      matte_color.blue=ColorShift(pixel_info.matte_color.blue);
      highlight_color.red=ColorShift(pixel_info.highlight_color.red);
      highlight_color.green=ColorShift(pixel_info.highlight_color.green);
      highlight_color.blue=ColorShift(pixel_info.highlight_color.blue);
      shadow_color.red=ColorShift(pixel_info.shadow_color.red);
      shadow_color.green=ColorShift(pixel_info.shadow_color.green);
      shadow_color.blue=ColorShift(pixel_info.shadow_color.blue);
    }
  /*
    Allocate image structure.
  */
  montage_image=AllocateImage((ImageInfo *) NULL);
  if (montage_image == (Image *) NULL)
    {
      Warning("Unable to montage images","Memory allocation failed");
      return((Image *) NULL);
    }
  montage_image->scene=1;
  number_images=
    (montage_info->number_tiles-1)/(tiles_per_row*tiles_per_column)+1;
  for (i=0; i < number_images; i++)
  {
    /*
      Initialize Image structure.
    */
    (void) strcpy(montage_image->filename,filename);
    montage_image->columns=
      (tile_info.width+(tile_info.x+border_width)*2)*tiles_per_row;
    number_tiles=Min(montage_info->number_tiles,tiles_per_row*tiles_per_column);
    montage_image->rows=(tile_info.height+(tile_info.y+border_width)*2+
      (display ? annotate_info.height+4 : 0))*
      (number_tiles/tiles_per_row+ ((number_tiles % tiles_per_row) != 0));
    if (resource_info->title != (char *) NULL)
      montage_image->rows+=((annotate_info.height+4) << 1)+(tile_info.y << 1);
    montage_image->montage=(char *) malloc(MaxTextLength*sizeof(char));
    count=1;
    for (tile=0; tile < number_tiles; tile++)
      count+=strlen(images[tile]->filename)+1;
    montage_image->directory=(char *) malloc(count*sizeof(char));
    montage_image->packets=montage_image->columns*montage_image->rows;
    montage_image->pixels=(RunlengthPacket *)
      malloc((unsigned int) montage_image->packets*sizeof(RunlengthPacket));
    if ((montage_image->montage == (char *) NULL) ||
        (montage_image->directory == (char *) NULL) ||
        (montage_image->pixels == (RunlengthPacket *) NULL))
      {
        Warning("Unable to montage images","Memory allocation failed");
        DestroyImages(montage_image);
        return((Image *) NULL);
      }
    /*
      Set montage geometry.
    */
    x_offset=0;
    y_offset=0;
    if (resource_info->title != (char *) NULL)
      y_offset+=((annotate_info.height+4) << 1)+(tile_info.y << 1);
    *montage_image->directory='\0';
    (void) sprintf(montage_image->montage,"%dx%d%+d%+d",
      (int) (tile_info.width+(tile_info.x+border_width)*2),
      (int) (tile_info.height+(tile_info.y+border_width)*2+
      (display ? annotate_info.height+4 : 0)),x_offset,y_offset);
    /*
      Initialize montage image to background color.
    */
    p=montage_image->pixels;
    for (x=0; x < montage_image->packets; x++)
    {
      p->red=ColorShift(background_color.red);
      p->green=ColorShift(background_color.green);
      p->blue=ColorShift(background_color.blue);
      p->index=0;
      p->length=0;
      p++;
    }
    if ((display != (Display *) NULL) &&
        (resource_info->title != (char *) NULL))
      {
        /*
          Copy title to the composite image.
        */
        (void) strcpy(annotate_info.text,resource_info->title);
        annotate_info.width=
          XTextWidth(font_info,annotate_info.text,strlen(annotate_info.text));
        max_width=montage_image->columns;
        if (((annotate_info.width+tile_info.x*2) << 1) >= max_width)
          {
            /*
              Label is too wide-- shorten.
            */
            q=annotate_info.text+strlen(annotate_info.text);
            do
            {
              *--q='\0';
              if ((int) strlen(annotate_info.text) > 2)
                (void) strcpy(q-2,"...");
              annotate_info.width=XTextWidth(font_info,annotate_info.text,
                strlen(annotate_info.text));
            } while ((2*(annotate_info.width+(tile_info.x << 1))) >= max_width);
          }
        label_info.width=(annotate_info.width << 1)-1;
        label_info.height=(annotate_info.height << 1)-1;
        label_info.x=tile_info.x+(int) (montage_image->columns >> 1)-
          (int) annotate_info.width;
        label_info.y=tile_info.y+4;
        (void) sprintf(annotate_info.geometry,"%ux%u%+d%+d",
          label_info.width,label_info.height,label_info.x,label_info.y);
        (void) XAnnotateImage(display,&pixel_info,&annotate_info,True,
          montage_image);
      }
    /*
      Copy tile images to the composite image.
    */
    x_offset=tile_info.x;
    y_offset=tile_info.y;
    if (display != (Display *) NULL)
      if (resource_info->title != (char *) NULL)
        y_offset+=((annotate_info.height+4) << 1)+(tile_info.y << 1);
    *montage_image->directory='\0';
    for (tile=0; tile < number_tiles; tile++)
    {
      /*
        Copy this tile to the composite image.
      */
      image=images[tile];
      (void) strcat(montage_image->directory,image->filename);
      (void) strcat(montage_image->directory,"\n");
      status=RunlengthDecodeImage(image);
      if (status == False)
        {
          Warning("Unable to unpack image",image->filename);
          DestroyImages(montage_image);
          return((Image *) NULL);
        }
      if ((display != (Display *) NULL) && (border_width != 0))
        {
          ColorPacket
            black;

          Image
            *bordered_image;

          RectangleInfo
            border_info;

          /*
            Put a border around the image.
          */
          border_info.width=border_width-1;
          border_info.height=border_width-1;
          if (montage_info->frame)
            {
              border_info.width=(tile_info.width-image->columns+1) >> 1;
              border_info.height=(tile_info.height-image->rows+1) >> 1;
            }
          bordered_image=BorderImage(image,&border_info,&border_color);
          if (bordered_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=bordered_image;
            }
          border_info.width=1;
          border_info.height=1;
          black.red=0;
          black.green=0;
          black.blue=0;
          bordered_image=BorderImage(image,&border_info,&black);
          if (bordered_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=bordered_image;
            }
        }
      /*
        Gravitate image as specified by the tile gravity.
      */
      switch (resource_info->gravity)
      {
        case NorthWestGravity:
        {
          x=0;
          y=0;
          break;
        }
        case NorthGravity:
        {
          x=((tile_info.width+(border_width << 1))-image->columns) >> 1;
          y=0;
          break;
        }
        case NorthEastGravity:
        {
          x=(tile_info.width+(border_width << 1))-image->columns;
          y=0;
          break;
        }
        case WestGravity:
        {
          x=0;
          y=((tile_info.height+(border_width << 1))-image->rows) >> 1;
          break;
        }
        case ForgetGravity:
        case StaticGravity:
        case CenterGravity:
        default:
        {
          x=((tile_info.width+(border_width << 1))-image->columns) >> 1;
          y=((tile_info.height+(border_width << 1))-image->rows) >> 1;
          break;
        }
        case EastGravity:
        {
          x=(tile_info.width+(border_width << 1))-image->columns;
          y=((tile_info.height+(border_width << 1))-image->rows) >> 1;
          break;
        }
        case SouthWestGravity:
        {
          x=0;
          y=(tile_info.height+(border_width << 1))-image->rows;
          break;
        }
        case SouthGravity:
        {
          x=((tile_info.width+(border_width << 1))-image->columns) >> 1;
          y=(tile_info.height+(border_width << 1))-image->rows;
          break;
        }
        case SouthEastGravity:
        {
          x=(tile_info.width+(border_width << 1))-image->columns;
          y=(tile_info.height+(border_width << 1))-image->rows;
          break;
        }
      }
      if (montage_info->frame)
        if ((tile_info.x > 4) && (tile_info.y > 4))
          {
            Image
              *framed_image;

            RectangleInfo
              frame_info;

            /*
              Put an ornamental border around this tile.
            */
            frame_info.width=tile_info.width+(border_width << 1);
            frame_info.height=tile_info.height+(border_width << 1)+
              (annotate_info.height+4);
            frame_info.x=(x > 0 ? x : border_width);
            frame_info.y=(y > 0 ? y : border_width);
            framed_image=FrameImage(image,&frame_info,bevel_width,&matte_color,
              &highlight_color,&shadow_color);
            if (framed_image != (Image *) NULL)
              {
                DestroyImage(image);
                image=framed_image;
              }
            x=0;
            y=0;
          }
      /*
        Composite background image with tile image.
      */
      CompositeImage(montage_image,montage_info->compose,image,
        x_offset+x,y_offset+y);
      if ((display != (Display *) NULL) && montage_info->shadow)
        {
          register int
            columns,
            rows,
            shift;

          /*
            Put a shadow under the tile to show depth.
          */
          for (rows=0; rows < image->rows; rows++)
          {
            p=montage_image->pixels+montage_image->columns*(y_offset+y+rows+4)+
              x_offset+x+image->columns;
            for (columns=0; columns < 4; columns++)
            {
              shift=((columns+rows) % 2) == 0 ? 9 : 10;
              p->red=background_color.red >> shift;
              p->green=background_color.green >> shift;
              p->blue=background_color.blue >> shift;
              p++;
            }
          }
          for (rows=0; rows < 4; rows++)
          {
            p=montage_image->pixels+montage_image->columns*
              (y_offset+y+image->rows+rows)+x_offset+x+4;
            for (columns=0; columns < image->columns; columns++)
            {
              shift=((columns+rows) % 2) == 0 ? 9 : 10;
              p->red=background_color.red >> shift;
              p->green=background_color.green >> shift;
              p->blue=background_color.blue >> shift;
              p++;
            }
          }
        }
      if ((display != (Display *) NULL) && (image->label != (char *) NULL))
        {
          /*
            Copy tile label to the composite image.
          */
          (void) strcpy(annotate_info.text,image->label);
          annotate_info.width=
            XTextWidth(font_info,annotate_info.text,strlen(annotate_info.text));
          max_width=tile_info.width+(resource_info->border_width << 1)-8;
          if (annotate_info.width >= max_width)
            {
              /*
                Label is too wide-- shorten.
              */
              q=annotate_info.text+strlen(annotate_info.text);
              do
              {
                *--q='\0';
                if ((int) strlen(annotate_info.text) > 2)
                  (void) strcpy(q-2,"...");
                annotate_info.width=XTextWidth(font_info,annotate_info.text,
                  strlen(annotate_info.text));
              } while (annotate_info.width >= max_width);
            }
          label_info.width=annotate_info.width;
          label_info.height=annotate_info.height;
          label_info.x=x_offset+((tile_info.width+(border_width << 1)) >> 1)-
            (annotate_info.width >> 1);
          label_info.y=y_offset;
          if (!montage_info->frame)
            label_info.y+=y+tile_info.y+image->rows+2;
          else
            label_info.y+=tile_info.height+(border_width << 1)-bevel_width-2;
          (void) sprintf(annotate_info.geometry,"%ux%u%+d%+d",
            label_info.width,label_info.height,label_info.x,label_info.y);
          (void) XAnnotateImage(display,&pixel_info,&annotate_info,
            !montage_info->frame,montage_image);
        }
      DestroyImage(image);
      if (((tile+1) % tiles_per_row) != 0)
        x_offset+=tile_info.width+(tile_info.x+border_width)*2;
      else
        {
          x_offset=tile_info.x;
          y_offset+=tile_info.height+(tile_info.y+border_width)*2+
            (display ? annotate_info.height+4 : 0);
        }
    }
    if (i < (number_images-1))
      {
        /*
          Allocate next image structure.
        */
        montage_image->next=AllocateImage((ImageInfo *) NULL);
        if (montage_image->next == (Image *) NULL)
          {
            DestroyImages(montage_image);
            return((Image *) NULL);
          }
        (void) strcpy(montage_image->next->filename,filename);
        montage_image->next->file=montage_image->file;
        montage_image->next->scene=montage_image->scene+1;
        montage_image->next->previous=montage_image;
        montage_image=montage_image->next;
        images+=number_tiles;
        montage_info->number_tiles-=number_tiles;
      }
  }
  (void) free((char *) annotate_info.text);
  if (display != (Display *) NULL)
    {
      /*
        Free X resources.
      */
      XFreeFont(display,font_info);
      XFree((char *) visual_info);
      XFree((char *) map_info);
    }
  while (montage_image->previous != (Image *) NULL)
    montage_image=montage_image->previous;
  return(montage_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e f r e s h W i n d o w                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XRefreshWindow refreshes an image in a X window.
%
%  The format of the XRefreshWindow routine is:
%
%      XRefreshWindow(display,window,event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
void XRefreshWindow(display,window,event)
Display
  *display;

XWindowInfo
  *window;

XEvent
  *event;
{
  int
    x,
    y;

  unsigned int
    height,
    width;

  if (event != (XEvent *) NULL)
    {
      /*
        Determine geometry from expose event.
      */
      x=event->xexpose.x;
      y=event->xexpose.y;
      width=event->xexpose.width;
      height=event->xexpose.height;
    }
  else
    {
      XEvent
        sans_event;

      /*
        Refresh entire window; discard outstanding expose events.
      */
      x=0;
      y=0;
      width=window->width;
      height=window->height;
      while (XCheckTypedWindowEvent(display,window->id,Expose,&sans_event));
    }
  /*
    Check boundary conditions.
  */
  if ((window->ximage->width-(x+window->x)) < width)
    width=window->ximage->width-(x+window->x);
  if ((window->ximage->height-(y+window->y)) < height)
    height=window->ximage->height-(y+window->y);
  /*
    Refresh image.
  */
  XSetClipMask(display,window->annotate_context,window->matte_pixmap);
  if (window->pixmap != (Pixmap) NULL)
    {
      if (window->depth > 1)
        XCopyArea(display,window->pixmap,window->id,window->annotate_context,
          x+window->x,y+window->y,width,height,x,y);
      else
        XCopyPlane(display,window->pixmap,window->id,window->highlight_context,
          x+window->x,y+window->y,width,height,x,y,1L);
    }
  else
    {
#ifdef HasSharedMemory
      if (window->shared_memory)
        {
          XShmPutImage(display,window->id,window->annotate_context,
            window->ximage,x+window->x,y+window->y,x,y,width,height,True);
          XSync(display,False);
        }
#endif
      if (!window->shared_memory)
        XPutImage(display,window->id,window->annotate_context,window->ximage,
          x+window->x,y+window->y,x,y,width,height);
    }
  XSetClipMask(display,window->annotate_context,None);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e t a i n W i n d o w C o l o r s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XRetainWindowColors sets X11 color resources on a window.  This
%  perserves the colors associated with an image displayed on the window.
%
%  The format of the XRetainWindowColors routine is:
%
%      XRetainWindowColors(display,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
void XRetainWindowColors(display,window)
Display
  *display;

Window
  window;
{
  Atom
    property;

  Pixmap
    pixmap;

  /*
    Put property on the window.
  */
  property=XInternAtom(display,"_XSETROOT_ID",False);
  if (property == (Atom) NULL)
    {
      Warning("Unable to create X property","_XSETROOT_ID");
      return;
    }
  pixmap=XCreatePixmap(display,window,1,1,1);
  if (pixmap == (Pixmap) NULL)
    {
      Warning("Unable to create X pixmap",(char *) NULL);
      return;
    }
  XChangeProperty(display,window,property,XA_PIXMAP,32,PropModeReplace,
    (unsigned char *) &pixmap,1);
  XSetCloseDownMode(display,RetainPermanent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e l e c t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XSelectWindow allows a user to select a window using the mouse.  If
%  the mouse moves, a cropping rectangle is drawn and the extents of the
%  rectangle is returned in the crop_info structure.
%
%  The format of the XSelectWindow function is:
%
%      target_window=XSelectWindow(display,crop_info)
%
%  A description of each parameter follows:
%
%    o window: XSelectWindow returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o crop_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any cropping rectangle.
%
%
*/
Window XSelectWindow(display,crop_info)
Display
  *display;

RectangleInfo
  *crop_info;
{
#define MinimumCropArea  (unsigned int) 9

  Cursor
    target_cursor;

  GC
    annotate_context;

  int
    presses,
    status,
    x_offset,
    y_offset;

  Window
    root_window,
    target_window;

  XEvent
    event;

  XGCValues
    context_values;

  /*
    Initialize graphic context.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  context_values.background=XBlackPixel(display,XDefaultScreen(display));
  context_values.foreground=XWhitePixel(display,XDefaultScreen(display));
  context_values.function=GXinvert;
  context_values.plane_mask=
    context_values.background ^ context_values.foreground;
  context_values.subwindow_mode=IncludeInferiors;
  annotate_context=XCreateGC(display,root_window,GCBackground | GCForeground |
    GCFunction | GCPlaneMask | GCSubwindowMode,&context_values);
  if (annotate_context == (GC) NULL)
    return(False);
  /*
    Grab the pointer using target cursor.
  */
  target_cursor=XMakeCursor(display,root_window,
    XDefaultColormap(display,XDefaultScreen(display)),"white","black");
  status=XGrabPointer(display,root_window,False,(unsigned int)
    (ButtonPressMask | ButtonReleaseMask | ButtonMotionMask),GrabModeSync,
    GrabModeAsync,root_window,target_cursor,CurrentTime);
  if (status != GrabSuccess)
    Error("Unable to grab the mouse",(char *) NULL);
  /*
    Select a window.
  */
  crop_info->width=0;
  crop_info->height=0;
  presses=0;
  target_window=(Window) NULL;
  x_offset=0;
  y_offset=0;
  do
  {
    if ((crop_info->width*crop_info->height) >= MinimumCropArea)
      XDrawRectangle(display,root_window,annotate_context,crop_info->x,
        crop_info->y,crop_info->width-1,crop_info->height-1);
    /*
      Allow another event.
    */
    XAllowEvents(display,SyncPointer,CurrentTime);
    XWindowEvent(display,root_window,ButtonPressMask | ButtonReleaseMask |
      ButtonMotionMask,&event);
    if ((crop_info->width*crop_info->height) >= MinimumCropArea)
      XDrawRectangle(display,root_window,annotate_context,crop_info->x,
        crop_info->y,crop_info->width-1,crop_info->height-1);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (target_window == (Window) NULL)
          {
            target_window=event.xbutton.subwindow;
            if (target_window == (Window) NULL)
              target_window=root_window;
          }
        x_offset=event.xbutton.x_root;
        y_offset=event.xbutton.y_root;
        crop_info->x=x_offset;
        crop_info->y=y_offset;
        crop_info->width=0;
        crop_info->height=0;
        presses++;
        break;
      }
      case ButtonRelease:
      {
        presses--;
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,&event));
        crop_info->x=event.xmotion.x;
        crop_info->y=event.xmotion.y;
        /*
          Check boundary conditions.
        */
        if (crop_info->x < x_offset)
          crop_info->width=(unsigned int) (x_offset-crop_info->x);
        else
          {
            crop_info->width=(unsigned int) (crop_info->x-x_offset);
            crop_info->x=x_offset;
          }
        if (crop_info->y < y_offset)
          crop_info->height=(unsigned int) (y_offset-crop_info->y);
        else
          {
            crop_info->height=(unsigned int) (crop_info->y-y_offset);
            crop_info->y=y_offset;
          }
      }
      default:
        break;
    }
  }
  while ((target_window == (Window) NULL) || (presses > 0));
  XUngrabPointer(display,CurrentTime);
  XFreeCursor(display,target_cursor);
  XFreeGC(display,annotate_context);
  if ((crop_info->width*crop_info->height) < MinimumCropArea)
    {
      crop_info->width=0;
      crop_info->height=0;
    }
  if ((crop_info->width != 0) && (crop_info->height != 0))
    target_window=root_window;
  return(target_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e t W i n d o w E x t e n t s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XSetWindowExtents resizes the window to a size determined by the
%  text and the font size.
%
%  The format of the XSetWindowExtents function is:
%
%      XSetWindowExtents(display,window,text)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o text: Specifies a pointer to a text string.
%
%
*/
void XSetWindowExtents(display,window,text)
Display
  *display;

XWindowInfo
  *window;

char
  *text;
{
  unsigned int
    height;

  XWindowChanges
    window_changes;

  window->width=XTextWidth(window->font_info,text,strlen(text))+
    ((3*window->font_info->max_bounds.width) >> 1);
  height=window->font_info->ascent+window->font_info->descent;
  window->height=(5*height) >> 2;
  window_changes.width=window->width;
  window_changes.height=window->height;
  XReconfigureWMWindow(display,window->id,window->screen,CWWidth | CWHeight,
    &window_changes);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X V i s u a l C l a s s N a m e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XVisualClassName returns the visual class name as a character
%  string.
%
%  The format of the XVisualClassName routine is:
%
%      visual_type=XVisualClassName(class)
%
%  A description of each parameter follows:
%
%    o visual_type: XVisualClassName returns the visual class as a character
%      string.
%
%    o class: Specifies the visual class.
%
%
*/
char *XVisualClassName(class)
int
  class;
{
  switch (class)
  {
    case StaticGray: return("StaticGray");
    case GrayScale: return("GrayScale");
    case StaticColor: return("StaticColor");
    case PseudoColor: return("PseudoColor");
    case TrueColor: return("TrueColor");
    case DirectColor: return("DirectColor");
  }
  return("unknown visual class");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y I D                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByID locates a child window with a given ID.  If not window
%  with the given name is found, 0 is returned.   Only the window specified
%  and its subwindows are searched.
%
%  The format of the XWindowByID function is:
%
%      child=XWindowByID(display,window,id)
%
%  A description of each parameter follows:
%
%    o child: XWindowByID returns the window with the specified
%      id.  If no windows are found, XWindowByID returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o id: Specifies the id of the window to locate.
%
%
*/
Window XWindowByID(display,root_window,id)
Display
  *display;

Window
  root_window;

unsigned long
  id;
{
  register int
    i;

  unsigned int
    number_children;

  Window
    child,
    *children,
    window;

  if (root_window == id)
    return(id);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByID(display,children[i],id);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByName locates a window with a given name on a display.
%  If no window with the given name is found, 0 is returned. If more than
%  one window has the given name, the first one is returned.  Only root and
%  its children are searched.
%
%  The format of the XWindowByName function is:
%
%      window=XWindowByName(display,root_window,name)
%
%  A description of each parameter follows:
%
%    o window: XWindowByName returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o root_window: Specifies the id of the root window.
%
%    o name: Specifies the name of the window to locate.
%
%
*/
Window XWindowByName(display,root_window,name)
Display
  *display;

Window
  root_window;

char
  *name;
{
  register int
    i;

  unsigned int
    number_children;

  Window
    *children,
    child,
    window;

  XTextProperty
    window_name;

  if (XGetWMName(display,root_window,&window_name) != 0)
    if (strcmp((char *) window_name.value,name) == 0)
      return(root_window);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByName(display,children[i],name);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y P r o p e r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByProperty locates a child window with a given property.
%  If no window with the given name is found, 0 is returned.  If more than
%  one window has the given property, the first one is returned.  Only the
%  window specified and its subwindows are searched.
%
%  The format of the XWindowByProperty function is:
%
%      child=XWindowByProperty(display,window,property)
%
%  A description of each parameter follows:
%
%    o child: XWindowByProperty returns the window id with the specified
%      property.  If no windows are found, XWindowByProperty returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o property: Specifies the property of the window to locate.
%
%
*/
static Window XWindowByProperty(display,window,property)
Display
  *display;

Window
  window;

Atom
  property;
{
  Atom
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned int
    i,
    number_children;

  unsigned long
    after,
    number_items;

  Window
    child,
    *children,
    parent,
    root;

  status=XQueryTree(display,window,&root,&parent,&children,&number_children);
  if (status == 0)
    return((Window) NULL);
  type=(Atom) NULL;
  child=(Window) NULL;
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
  {
    status=XGetWindowProperty(display,children[i],property,0L,0L,False,
      (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
    if ((status == Success) && (type != (Atom) NULL))
      child=children[i];
  }
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
    child=XWindowByProperty(display,children[i],property);
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(child);
}
